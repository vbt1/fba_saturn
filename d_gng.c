#define CACHE 1
//#define CZ80 1
//#define RAZE 1

#include "d_gng.h"
#include "m6809_intf.h"
#define nInterleave 25

int xScroll,yScroll;
unsigned int color[16];
 unsigned int nNext[25];

UINT16 map_offset_lut[2048];
UINT16 map_offset_lut_fg[1024];

int ovlInit(char *szShortName)
{
	cleanBSS();

struct BurnDriver nBurnDrvGng = {
	"gng", NULL,
	"Ghosts'n Goblins (World? set 1)\0",
	DrvRomInfo, DrvRomName, DrvInputInfo, DrvDIPInfo,
	DrvInit, DrvExit, DrvFrame, NULL//, NULL
};

struct BurnDriver nBurnDrvGnga = {
	"gnga", "gng",
	"Ghosts'n Goblins (World? set 2)\0",
	DrvaRomInfo, DrvaRomName, DrvInputInfo, DrvDIPInfo,
	GngaInit, DrvExit, DrvFrame, NULL//, NULL
};

    if (strcmp(nBurnDrvGng.szShortName, szShortName) == 0)
		memcpy(shared,&nBurnDrvGng,sizeof(struct BurnDriver));
	else
		memcpy(shared,&nBurnDrvGnga,sizeof(struct BurnDriver));
	ss_reg          = (SclNorscl *)SS_REG;
}

/*static*/ inline void DrvClearOpposites(unsigned char* nJoystickInputs)
{
	if ((*nJoystickInputs & 0x03) == 0x03) {
		*nJoystickInputs &= ~0x03;
	}
	if ((*nJoystickInputs & 0x0c) == 0x0c) {
		*nJoystickInputs &= ~0x0c;
	}
}

/*static*/ inline void DrvMakeInputs()
{
	int i;
	// Reset Inputs
	DrvInput[0] = DrvInput[1] = DrvInput[2] = 0x00;

	// Compile Digital Inputs
	for (i = 0; i < 8; i++) {
		DrvInput[0] |= (DrvInputPort0[i] & 1) << i;
		DrvInput[1] |= (DrvInputPort1[i] & 1) << i;
		DrvInput[2] |= (DrvInputPort2[i] & 1) << i;
	}

	// Clear Opposites
	DrvClearOpposites(&DrvInput[1]);
	DrvClearOpposites(&DrvInput[2]);
}

/*static*/ int MemIndex()
{
	unsigned char *Next; Next = Mem;
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;

	DrvM6809Rom            = Next; Next += 0x14000;
	DrvZ80Rom              = Next; Next += 0x08000;

	RamStart               = Next;

	DrvM6809Ram            = Next; Next += 0x01e00;
	DrvZ80Ram              = Next; Next += 0x00800;
	DrvSpriteRam           = Next; Next += 0x00200;
	DrvSpriteRamBuffer     = Next; Next += 0x00200;
	DrvFgVideoRam          = Next; Next += 0x00800;
	DrvBgVideoRam          = Next; Next += 0x00800;
	DrvPaletteRam1         = Next; Next += 0x00100;
	DrvPaletteRam2         = Next; Next += 0x00100;

	RamEnd                 = Next;

	DrvChars               = cache;//Next; Next += 0x400 * 8 * 8;
	DrvTiles               = (unsigned char *)SCL_VDP2_VRAM_B1;//Next; Next += 0x400 * 16 * 16;
	DrvSprites             = ss_vram+0x1100;//Next; Next += 0x400 * 16 * 16;
	//DrvPalette             = (unsigned int*)Next; Next += 0x00100 * sizeof(unsigned int);
	cram_lut				= Next; Next += 65536*2;
	MemEnd                 = Next;

	return 0;
}

/*static*/ int DrvDoReset()
{
//	M6809Open(0);
	M6809Reset();
	M6809Close();
#ifdef CZ80	
	CZetOpen(0);
	CZetReset();
	CZetClose();
#endif	

#ifdef RAZE
	z80_reset();
#endif
//	BurnYM2203Reset();
	
	DrvRomBank = 0;
	DrvBgScrollX[0] = DrvBgScrollX[1] = 0;
	DrvBgScrollY[0] = DrvBgScrollY[1] = 0;
	DrvSoundLatch = 0;

	return 0;
}

unsigned char DrvGngM6809ReadByte(unsigned short Address)
{
	switch (Address) {
		case 0x3000: {
			return 0xff - DrvInput[0];
		}
		
		case 0x3001: {
			return 0xff - DrvInput[1];
		}
		
		case 0x3002: {
			return 0xff - DrvInput[2];
		}
		
		case 0x3003: {
			return DrvDip[0];
		}
		
		case 0x3004: {
			return DrvDip[1];
		}
		
		case 0x3c00: {
			// nop
			return 0;
		}
	}
	
//	bprintf(PRINT_NORMAL, _T("M6809 Read Byte -> %04X\n"), Address);
	
	return 0;
}

static void bg_render(unsigned short Address, unsigned char *BgVideoRam)
{
	unsigned int Code = *BgVideoRam;
	unsigned int Attr = BgVideoRam[0x400];
	Code += (Attr & 0xc0) << 2;

	unsigned short x = map_offset_lut[Address];
	unsigned int Flip = (Attr & 0x30) << 6;
	ss_map2[x+1024] = ss_map2[x] = color[Attr & 0x07] | Flip | Code;//0x400; //Colour<<12 | Flip | Code;//0x400;
}

static void fg_render(unsigned short Address, unsigned char *FgVideoRam)
{
	unsigned int Code = *FgVideoRam;
	unsigned int Attr = FgVideoRam[0x400];
	Code += (Attr & 0xc0) << 2;

	unsigned short x = map_offset_lut_fg[Address]; //(mx|(my<<6));
	unsigned int Flip = (Attr & 0x30) << 6;

	ss_map[x] =  color[Attr & 0x0f] | Flip | Code; //Colour << 12 | Flip | Code;
}

void DrvGngM6809WriteByte(unsigned short Address, unsigned char Data)
{
#ifdef CACHE
	if (Address>=0x3a00)
	{
#endif
		switch (Address) {
			case 0x3a00: {
				DrvSoundLatch = Data;
				return;
			}
			
			case 0x3b08: {
				if(DrvBgScrollX[0] != Data)
				{
					DrvBgScrollX[0] = Data;
					xScroll = DrvBgScrollX[0] | (DrvBgScrollX[1] << 8);
//					xScroll &= 0x1ff;
					xScroll &= 0x1ff;
					xScroll <<= 16;
					ss_reg->n1_move_x =  xScroll;
				}
				return;
			}
			
			case 0x3b09: {
				if(DrvBgScrollX[1] != Data)
				{
					DrvBgScrollX[1] = Data;
					xScroll = DrvBgScrollX[0] | (DrvBgScrollX[1] << 8);
					xScroll &= 0x1ff;
					xScroll <<= 16;
					ss_reg->n1_move_x =  xScroll;
				}
				return;
			}
			
			case 0x3b0a: {
				if(DrvBgScrollY[0] != Data)
				{
					DrvBgScrollY[0] = Data;
					yScroll = DrvBgScrollY[0] | (DrvBgScrollY[1] << 8);
					yScroll &= 0x1ff;
					yScroll+=16;
					yScroll <<= 16;
					ss_reg->n1_move_y =  yScroll;	
				}
				return;
			}
			
			case 0x3b0b: {
				if(DrvBgScrollY[1] != Data)
				{
					DrvBgScrollY[1] = Data;
					yScroll = DrvBgScrollY[0] | (DrvBgScrollY[1] << 8);
					yScroll &= 0x1ff;
					yScroll+=16;
					yScroll <<= 16;
					ss_reg->n1_move_x =  xScroll;
				}
				return;
			}
			
			case 0x3c00: {
				// nop
				return;
			}
			
			case 0x3d00: {
				// flipscreen
				return;
			}
			
			case 0x3d01: {
				// ???
				return;
			}
			
			case 0x3e00: {
				DrvRomBank = Data & 3;
				if (Data == 4) {
					DrvRomBank = 4;
//					M6809MapMemory(DrvM6809Rom, 0x4000, 0x5fff, M6809_ROM);
					M6809MapMemory2(DrvM6809Rom, 0x4000, 0x5fff);
				} else {
//					M6809MapMemory(DrvM6809Rom + 0xc000 + (DrvRomBank * 0x2000), 0x4000, 0x5fff, M6809_ROM);
					M6809MapMemory2(DrvM6809Rom + 0xc000 + (DrvRomBank * 0x2000), 0x4000, 0x5fff);
				}
				return;
			}
		}
#ifdef CACHE
	}
	else
	{
		if(Address>=0x2000 && Address<=0x23FF)
		{
			Address&=0x3ff;
			if(DrvFgVideoRam[Address]!=Data)
			{
 				DrvFgVideoRam[Address] = Data;
				fg_render(Address,&DrvFgVideoRam[Address]);
			}
			return;
		}

		if(Address>=0x2400 && Address<=0x27FF)
		{
			Address&=0x7ff;
			if(DrvFgVideoRam[Address]!=Data)
			{
				DrvFgVideoRam[Address] = Data;
				Address&=0x3ff;
				fg_render(Address,&DrvFgVideoRam[Address]);
			}
			return;
		}

		if(Address>=0x2800 && Address<=0x2BFF)
		{
			Address&=0x3ff;
			if(DrvBgVideoRam[Address]!=Data)
			{
				DrvBgVideoRam[Address] = Data;
				bg_render(Address,&DrvBgVideoRam[Address]);
			}
			return;
		}

		if(Address>=0x2C00 && Address<=0x2fff)
		{
			Address&=0x7ff;
			if(DrvBgVideoRam[Address]!=Data)
			{
				DrvBgVideoRam[Address] = Data;
				Address&=0x3ff;
				bg_render(Address,&DrvBgVideoRam[Address]);
			}
			return;
		}
	}
#endif	
//	bprintf(PRINT_NORMAL, _T("M6809 Write Byte -> %04X, %02X\n"), Address, Data);
}

unsigned char __fastcall DrvGngZ80Read(unsigned short a)
{
	switch (a) {
		case 0xc800: {
			return DrvSoundLatch;
		}
		
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 Read => %04X\n"), a);
		}
	}
	return 0;
}

void __fastcall DrvGngZ80Write(unsigned short a, unsigned char d)
{
	switch (a) {
		case 0xe000: {
//			BurnYM2203Write(0, 0, d);
			return;
		}
		
		case 0xe001: {
//			BurnYM2203Write(0, 1, d);
			return;
		}
		
		case 0xe002: {
//			BurnYM2203Write(1, 0, d);
			return;
		}
		
		case 0xe003: {
//			BurnYM2203Write(1, 1, d);
			return;
		}
		
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 Write => %04X, %02X\n"), a, d);
		}
	}
}

/*static*/ int CharPlaneOffsets[2]   = { 4, 0 };
/*static*/ int CharXOffsets[8]       = { 0, 1, 2, 3, 8, 9, 10, 11 };
/*static*/ int CharYOffsets[8]       = { 0, 16, 32, 48, 64, 80, 96, 112 };
/*static*/ int TilePlaneOffsets[3]   = { 0x80000, 0x40000, 0 };
/*static*/ int TileXOffsets[16]      = { 0, 1, 2, 3, 4, 5, 6, 7, 128, 129, 130, 131, 132, 133, 134, 135 };
/*static*/ int TileYOffsets[16]      = { 0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120 };
/*static*/ int SpritePlaneOffsets[4] = { 0x80004, 0x80000, 4, 0 };
/*static*/ int SpriteXOffsets[16]    = { 0, 1, 2, 3, 8, 9, 10, 11, 256, 257, 258, 259, 264, 265, 266, 267 };
/*static*/ int SpriteYOffsets[16]    = { 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240 };
/*
inline  int DrvSynchroniseStream(int nSoundRate)
{
#ifdef CZ80	
	return (long long)(CZetTotalCycles() * nSoundRate / 3000000);
#endif
	return  0;
}

inline double DrvGetTime()
{
	#ifdef CZ80	
	return (double)CZetTotalCycles() / 3000000;
	#endif
		return 0;
}
*/
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void tile16x16toSaturn (int num, unsigned char *pDest)
{
	int c;
	for (c = 0; c < num; c++) 
	{
		unsigned char new_tile[128];
		UINT8 *dpM = pDest + (c * 128);
		memcpy(new_tile,dpM,128);
		unsigned int i=0,j=0,k=0;

		for (k=0;k<128;k+=64)
		{
			dpM = pDest + ((c * 128)+k);

			for (i=0;i<32;i+=4,j+=8)
			{
				dpM[i]=new_tile[j];
				dpM[i+1]=new_tile[j+1];
				dpM[i+2]=new_tile[j+2];
				dpM[i+3]=new_tile[j+3];
				dpM[i+32]=new_tile[j+4];
				dpM[i+33]=new_tile[j+5];
				dpM[i+34]=new_tile[j+6];
				dpM[i+35]=new_tile[j+7];
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ int DrvInit()
{
	int nRet = 0, nLen;

	DrvInitSaturn();

	// Allocate and Blank all required memory
	Mem = NULL;

	MemIndex();
	nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);
	MemIndex();

	unsigned char *	DrvTempRom = (unsigned char *)0x00200000;
	memset(DrvTempRom,0x00,0x20000);
	// Load M6809 Program Roms
	if (RomLoadOffset == 2) {
		nRet = BurnLoadRom(DrvM6809Rom + 0x00000, 0, 1); if (nRet != 0) return 1;
		nRet = BurnLoadRom(DrvM6809Rom + 0x04000, 1, 1); if (nRet != 0) return 1;
		nRet = BurnLoadRom(DrvM6809Rom + 0x08000, 2, 1); if (nRet != 0) return 1;
		nRet = BurnLoadRom(DrvM6809Rom + 0x0c000, 3, 1); if (nRet != 0) return 1;
		nRet = BurnLoadRom(DrvM6809Rom + 0x10000, 4, 1); if (nRet != 0) return 1;
	} else {
		nRet = BurnLoadRom(DrvM6809Rom + 0x00000, 0, 1); if (nRet != 0) return 1;
		nRet = BurnLoadRom(DrvM6809Rom + 0x04000, 1, 1); if (nRet != 0) return 1;
		nRet = BurnLoadRom(DrvM6809Rom + 0x0c000, 2, 1); if (nRet != 0) return 1;
	}
		
	// Load Z80 Program Roms
	nRet = BurnLoadRom(DrvZ80Rom + 0x00000, 3 + RomLoadOffset, 1); if (nRet != 0) return 1;
	
	// Load and decode the chars
	nRet = BurnLoadRom(DrvTempRom, 4 + RomLoadOffset, 1); if (nRet != 0) return 1;
//	GfxDecode4Bpp(0x400, 2, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, DrvChars);
	GfxDecode4Bpp(0x400, 2, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, DrvChars);

   unsigned int i;

	for (i=0;i<0x10000;i++ )
	{
		if ((DrvChars[i]& 0x03)       ==0x00)DrvChars[i] = DrvChars[i] & 0xf0 | 0x3;
		else if ((DrvChars[i]& 0x03)==0x03) DrvChars[i] = DrvChars[i] & 0xf0;

		if ((DrvChars[i]& 0x30)       ==0x00)DrvChars[i] = 0x30 | DrvChars[i] & 0x0f;
		else if ((DrvChars[i]& 0x30)==0x30) DrvChars[i] = DrvChars[i] & 0x0f;
	}



/*		 int vbtxx=0;
	for(vbtxx=0;vbtxx<16;vbtxx++)
	{
	 memset(&DrvChars[vbtxx*64], (vbtxx<<4)|vbtxx, 32);
	}	 						*/
//FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"decode1",136,60);	
	// Load and decode the tiles
	memset(DrvTempRom, 0, 0x20000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  5 + RomLoadOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x04000,  6 + RomLoadOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x08000,  7 + RomLoadOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x0c000,  8 + RomLoadOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000,  9 + RomLoadOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x14000, 10 + RomLoadOffset, 1); if (nRet != 0) return 1;
	memset(DrvTiles,0x00,0x20000);
	GfxDecode4Bpp(0x400, 3, 16, 16, TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x100, DrvTempRom, DrvTiles);
	tile16x16toSaturn(0x400, DrvTiles);

//	GfxDecode4Bpp(0x400, 3, 16, 16, TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x100, DrvTempRom, DrvSprites);
//	GfxDecode(0x400, 3, 16, 16, TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x100, DrvTempRom, DrvTiles);
/*
		 int vbtxx=0;
	for(vbtxx=0;vbtxx<16;vbtxx++)
	{
	 memset(&DrvTiles[vbtxx*32], (vbtxx<<4)|vbtxx, 32);
	}
  */

//FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"decode2",136,60);	
	// Load and decode the sprites
	memset(DrvTempRom, 0xff, 0x20000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000, 11 + RomLoadOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x04000, 12 + RomLoadOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x08000, 13 + RomLoadOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000, 14 + RomLoadOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x14000, 15 + RomLoadOffset, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x18000, 16 + RomLoadOffset, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(0x400, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x200, DrvTempRom, DrvSprites);
//FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"decode3",136,60);	


	for (i=0;i<0x10000;i++ )
	{
		if ((DrvSprites[i]& 0x0f)       ==0x00)DrvSprites[i] = DrvSprites[i] & 0xf0 | 0xf;
		else if ((DrvSprites[i]& 0x0f)==0x0f) DrvSprites[i] = DrvSprites[i] & 0xf0;

		if ((DrvSprites[i]& 0xf0)       ==0x00)DrvSprites[i] = 0xf0 | DrvSprites[i] & 0x0f;
		else if ((DrvSprites[i]& 0xf0)==0xf0) DrvSprites[i] = DrvSprites[i] & 0x0f;
	}

//	free(DrvTempRom);
	
	// Setup the M6809 emulation
	M6809Init(1);
//	M6809Open(0);
	M6809MapMemory(DrvM6809Ram          , 0x0000, 0x1dff, M6809_RAM);
	M6809MapMemory(DrvSpriteRam         , 0x1e00, 0x1fff, M6809_RAM);
#ifdef CACHE
	M6809MapMemory(DrvFgVideoRam        , 0x2000, 0x27ff, M6809_ROM);
//	M6809MapMemory(DrvFgVideoRam        , 0x2000, 0x27ff, M6809_RAM);
	M6809MapMemory(DrvBgVideoRam        , 0x2800, 0x2fff, M6809_ROM);
#else
	M6809MapMemory(DrvFgVideoRam        , 0x2000, 0x27ff, M6809_RAM);
	M6809MapMemory(DrvBgVideoRam        , 0x2800, 0x2fff, M6809_RAM);
#endif
	M6809MapMemory(DrvPaletteRam2       , 0x3800, 0x38ff, M6809_RAM);
	M6809MapMemory(DrvPaletteRam1       , 0x3900, 0x39ff, M6809_RAM);
	M6809MapMemory(DrvM6809Rom          , 0x4000, 0x5fff, M6809_ROM);
	M6809MapMemory(DrvM6809Rom + 0x2000 , 0x6000, 0xffff, M6809_ROM);
	M6809SetReadByteHandler(DrvGngM6809ReadByte);
//	M6809SetWriteByteHandler(DrvGngM6809WriteByte);
	M6809Close();
	#ifdef CZ80	
	// Setup the Z80 emulation
	CZetInit(1);
	CZetOpen(0);
	CZetSetReadHandler(DrvGngZ80Read);
	CZetSetWriteHandler(DrvGngZ80Write);
	CZetMapArea(0x0000, 0x7fff, 0, DrvZ80Rom             );
	CZetMapArea(0x0000, 0x7fff, 2, DrvZ80Rom             );
	CZetMapArea(0xc000, 0xc7ff, 0, DrvZ80Ram             );
	CZetMapArea(0xc000, 0xc7ff, 1, DrvZ80Ram             );
	CZetMapArea(0xc000, 0xc7ff, 2, DrvZ80Ram             );
	CZetMemEnd();
	CZetClose();	
	#endif

#ifdef RAZE
	z80_map_fetch (0x0000, 0x7fff, DrvZ80Rom); 
	z80_map_read  (0x0000, 0x7fff, DrvZ80Rom); //0 read 
	z80_map_fetch (0xc000, 0xc7ff, DrvZ80Ram); //2 fetch
	z80_map_read  (0xc000, 0xc7ff, DrvZ80Ram); //1 write 
	z80_map_write (0xc000, 0xc7ff, DrvZ80Ram); //0 read 

   z80_add_write(0xe000, 0xe00f, 1, (void *)&DrvGngZ80Write);
   z80_add_read(0xc800, 0xc80f, 1, (void *)&DrvGngZ80Read);
   z80_end_memmap();
#endif
//	BurnYM2203Init(2, 1500000, NULL, DrvSynchroniseStream, DrvGetTime, 0);
//	BurnTimerAttachCZet(3000000);

//	GenericTilesInit();
	make_cram_lut();
	nBurnFunction = DrvCalcPalette;
	// Reset the driver
	DrvDoReset();

	return 0;
}

/*static*/ int GngaInit()
{
	RomLoadOffset = 2;
	
	return DrvInit();
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initLayers()
{
    Uint16	CycleTb[]={
		0x1f56, 0xffff, //A0
		0xffff, 0xffff,	//A1
		0xf5f2, 0x4eff,   //B0
		0xffff, 0xffff  //B1
//		0x4eff, 0x1fff, //B1
	};
/*
voir page 58 vdp2
voir plutot p355 vdp2
*/
 	SclConfig	scfg;
//A0 0x0000 0000
//A1 0x0004 0x005
//B0 0x0008
//B1 0x000c 1100
// 3 nbg
	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN1WORD;
	scfg.flip          = SCL_PN_10BIT; 
	scfg.platesize     = SCL_PL_SIZE_1X1;
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.patnamecontrl =  0x0000;// VRAM B1 のオフセット 
	scfg.plate_addr[0] = (Uint32)ss_map;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG2, &scfg);
// 3 nbg
//	scfg.dispenbl      = ON;
	scfg.charsize          = SCL_CHAR_SIZE_2X2;//OK du 1*1 surtout pas toucher
	scfg.pnamesize      = SCL_PN1WORD;
	scfg.patnamecontrl =  0x000c;// VRAM B1 のオフセット 
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
//	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
//	scfg.datatype      = SCL_CELL;

	scfg.coltype       = SCL_COL_TYPE_16;
	scfg.platesize     = SCL_PL_SIZE_2X1;
	scfg.plate_addr[0] = (Uint32)ss_map2;
//	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG1, &scfg);

	scfg.dispenbl 		 = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 		 = SCL_BITMAP;
	scfg.mapover		 = SCL_OVER_0;
	scfg.plate_addr[0]	 = (Uint32)ss_font;

// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);	
}
 //-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initPosition()
{
	SCL_Open();
	ss_reg->n1_move_x = 0;
	ss_reg->n1_move_y = (16<<16);
	ss_reg->n2_move_y = (16);
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr    = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
	colAddr        = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	colBgAddr2  = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);
// 	
	SCL_AllocColRam(SCL_NBG0,OFF);
	SCL_SetColRam(SCL_NBG0,8,4,palette);
//	SCL_AllocColRam(SCL_NBG3,OFF);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn()
{
	nBurnSprites  = 131;
	//pBurnSoundOut = (short *)malloc (nBurnSoundLen * sizeof(short)*4);

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite           = (SprSpCmd *)SS_SPRIT;

//	SS_MAP          = ss_map    =(Uint16 *)SCL_VDP2_VRAM_B0+0x10000;
//	SS_MAP2        = ss_map2  =(Uint16 *)SCL_VDP2_VRAM_B0+0x00000;
	SS_FONT        = ss_font     =(Uint16 *)SCL_VDP2_VRAM_A1;
	SS_MAP          = ss_map    =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_MAP2        = ss_map2  =(Uint16 *)SCL_VDP2_VRAM_A1+0x08000;
//	SS_FONT        = ss_font     =(Uint16 *)NULL;
	SS_CACHE      = cache       =(Uint8  *)SCL_VDP2_VRAM_A0;

	SS_SET_N0PRIN(7);
	SS_SET_S0PRIN(5);
	SS_SET_N2PRIN(6);
	SS_SET_N1PRIN(4);

	initLayers();
	initPosition();
	initColors();
	initSprites(256-1,224-1,0,0,0,0);

//	make_cram_lut();
	int j=0;
	for (int my = 0; my < 64; my++) 
	{
		for (int mx = 0; mx < 32; mx++) 
		{
			map_offset_lut[j] = my|(mx<<5);
			j++;
		}
	}
	j=0;

	for (int my = 0; my < 32; my++) 
	{
		for (int mx = 0; mx < 32; mx++) 
		{
 			map_offset_lut_fg[j] = (mx|(my<<6));
			j++;
		}
	}
	
	for (j = 0; j < 16; j++) 
	{
		color[j] = j << 12;
	}

	nCyclesTotal[0] = 1500000 / 60;
//	nCyclesTotal[1] = 1000000 / 60;

	for (j = 0; j < nInterleave; j++) 
	{
		nNext[j] = (j + 1) * nCyclesTotal[0] / nInterleave;
	}

	drawWindow(0,240,0,0,64);  
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ int DrvExit()
{
	M6809Exit();
	#ifdef CZ80	
	CZetExit();
	#endif
//	BurnYM2203Exit();
	
//	GenericTilesExit();
	
	free(Mem);
	Mem = NULL;
	
	DrvRomBank = 0;
	DrvBgScrollX[0] = DrvBgScrollX[1] = 0;
	DrvBgScrollY[0] = DrvBgScrollY[1] = 0;
	DrvSoundLatch = 0;
	
	RomLoadOffset = 0;
//	Diamond = 0;

	return 0;
}

/*static*/ inline unsigned char pal4bit(unsigned char bits)
{
	bits &= 0x0f;
	return (bits << 4) | bits;
}

/*static*/ void make_cram_lut(void)
{
    unsigned int j;
    for(j = 0; j < 32768*2; j++)
    {
		int r = pal4bit(j >> 12);
		int g = pal4bit(j >> 8);
		int b = pal4bit(j >> 4);

        cram_lut[j] =RGB(r>>3,g>>3,b>>3);
    }
}

/*static*/ void DrvCalcPalette()
{
	unsigned int i,j,delta=0;
	unsigned short *colAddr1 = &colBgAddr[0];
	
	for (i = 0; i < 64; i+=8) 
	{
		for (j = 0; j < 8; j++) 
		{
			unsigned int Val = DrvPaletteRam1[i+j] + (DrvPaletteRam2[i+j] << 8);
			*(colAddr1++) = cram_lut[Val];
		}
		colAddr1 += 8;
	}

	for (i = 64; i < 128; i++) 
	{
		unsigned int Val = DrvPaletteRam1[i] + (DrvPaletteRam2[i] << 8);
		colAddr[i-64] = cram_lut[Val];
	}

	colAddr[15] = colAddr[0];
	colAddr[31] = colAddr[16];
	colAddr[47] = colAddr[32];
	colAddr[63] = colAddr[48];

	delta=0;
	
	for (i = 128,j=0; i < 256; i+=4) 
	{
		for (j = 0; j < 4; j++) 
		{
			unsigned int Val = DrvPaletteRam1[i+j] + (DrvPaletteRam2[i+j] << 8);
			colBgAddr2[i+j+delta-128] = cram_lut[Val];
		}
		colBgAddr2[i+3+delta-128] = colBgAddr2[i+delta-128];
		colBgAddr2[i+delta-128]     = colBgAddr2[i+3+delta-128];
		delta += 12;  
	}

}

/*static*/ void DrvRenderSprites()
{
	for (int Offs = 0x200 - 4; Offs >= 0; Offs -= 4) 
	{
		UINT8 Attr = DrvSpriteRamBuffer[Offs + 1];
		int sx = DrvSpriteRamBuffer[Offs + 3] - (0x100 * (Attr & 0x01));
		int sy = DrvSpriteRamBuffer[Offs + 2];
		int delta	= ((Offs/4)+3);

		if (sx>= 0 && sy >= 0 && sx < 256)
		{
			int flip = (Attr & 0x0C)<<2;
			int Code = DrvSpriteRamBuffer[Offs + 0] + ((Attr << 2) & 0x300);
//			int Colour = (Attr >> 4) & 3;

			ss_sprite[delta].control			= ( JUMP_NEXT | FUNC_NORMALSP | flip);
			ss_sprite[delta].drawMode	= ( ECD_DISABLE | COMPO_REP);

			ss_sprite[delta].ax			= sx;
			ss_sprite[delta].ay			= sy-16;
			ss_sprite[delta].charSize		= 0x210;
			ss_sprite[delta].color			    = Attr & 0x30;//Colour<<4;
			ss_sprite[delta].charAddr		= 0x220+(Code<<4);
		}
		else
		{
			ss_sprite[delta].charSize   = 0;
	//		ss_sprite[delta].charAddr   = 0;
			ss_sprite[delta].ax   = 0;
	//		ss_sprite[delta].ay   = 0;
		} 
	}
}

/*static*/ int DrvFrame()
{
	if (DrvReset) DrvDoReset();

	DrvMakeInputs();

	nCyclesDone[0] = /*nCyclesDone[1] =*/ 0;
	#ifdef CZ80	
	CZetNewFrame();
	#endif
	for (UINT32 i = 0; i < nInterleave; ++i) 
	{
		// Run M6809
		M6809Open(0);
		nCyclesSegment = nNext[i] - nCyclesDone[0];
		nCyclesDone[0] += m6809_execute(nCyclesSegment);//M6809Run(nCyclesSegment);
		if (i == 24) {
			M6809SetIRQ(0, M6809_IRQSTATUS_AUTO);
		}
		M6809Close();
		
		// Run Z80
		#ifdef CZ80	
//		CZetOpen(0);
		BurnTimerUpdate(i * (nCyclesTotal[1] / nInterleave));
		if (i == 5 || i == 10 || i == 15 || i == 20) CZetSetIRQLine(0, CZET_IRQSTATUS_ACK);
		if (i == 6 || i == 11 || i == 16 || i == 21) CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);
//		CZetClose();
		#endif

		#ifdef RAZE
		if (i == 5 || i == 10 || i == 15 || i == 20) z80_raise_IRQ(0);
		if (i == 6 || i == 11 || i == 16 || i == 21) z80_lower_IRQ();
		#endif
		// Render Sound Segment
/*		if (pBurnSoundOut) {
			int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
			short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			CZetOpen(0);
			BurnYM2203Update(pSoundBuf, nSegmentLength);
			CZetClose();
			nSoundBufferPos += nSegmentLength;
		}
*/
	}
	
	// Make sure the buffer is entirely filled.
/*	if (pBurnSoundOut) {
		int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
		short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
		if (nSegmentLength) {
			CZetOpen(0);
			BurnYM2203Update(pSoundBuf, nSegmentLength);
			CZetClose();
		}
	}
*/	
	#ifdef CZ80	
//	CZetOpen(0);
	BurnTimerEndFrame(nCyclesTotal[1]);
//	CZetClose();
	#endif
//	if (pBurnDraw) 
//	DrvDraw();
	DrvRenderSprites();
	
	memcpyl(DrvSpriteRamBuffer, DrvSpriteRam, 0x200);

	return 0;
}
