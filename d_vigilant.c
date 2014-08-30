//#include "tiles_generic.h"
//#include "burn_ym2151.h"
//#include "burn_ym2203.h"
//#include "dac.h"

#define BMP 1

#define CZ80 1
#define RAZE1 1
#define USE_MAP 1
#define USE_SPRITES 1
#define VBTLIB 1
//#include "tiles_generic.h"
//#include "burnint.h"
#include "psg.h"

#define		_SPR3_
#include "sega_spr.h"
#include "sega_scl2.h"
#include "sega_pcm.h"

 #include "d_vigilant.h"


unsigned int vbmap[4][0x1000];


int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvVigilant = {
		"vigil", NULL,
		"Vigilante (World)\0",
		DrvRomInfo, DrvRomName, DrvInputInfo, DrvDIPInfo,
		DrvInit, DrvExit, DrvFrame, NULL //NULL
	};

	memcpy(shared,&nBurnDrvVigilant,sizeof(struct BurnDriver));
	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}

//-------------------------------------------------------------------------------------------------------------------------------------
/*inline*/ /*static*/ int readbit(const UINT8 *src, int bitnum)
{
	return src[bitnum / 8] & (0x80 >> (bitnum % 8));
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void GfxDecode4Bpp(int num, int numPlanes, int xSize, int ySize, int planeoffsets[], int xoffsets[], int yoffsets[], int modulo, unsigned char *pSrc, unsigned char *pDest)
{
	int c;
//	wait_vblank();
	for (c = 0; c < num; c++) {
		int plane, x, y;
	
		UINT8 *dp = pDest + (c * (xSize/2) * ySize);
		memset(dp, 0, (xSize/2) * ySize);
	
		for (plane = 0; plane < numPlanes; plane++) {
			int planebit = 1 << (numPlanes - 1 - plane);
			int planeoffs = (c * modulo) + planeoffsets[plane];
		
			for (y = 0; y < ySize; y++) {
				int yoffs = planeoffs + yoffsets[y];
				dp = pDest + (c * (xSize/2) * ySize) + (y * (xSize/2));
			
				for (x = 0; x < xSize; x+=2) {
					if (readbit(pSrc, yoffs + xoffsets[x])) dp[x>>1] |= (planebit&0x0f)<<4;
					if (readbit(pSrc, yoffs + xoffsets[x+1])) dp[x>>1] |= (planebit& 0x0f);
					//(NewsTiles[i+1]& 0x0f)| ((NewsTiles[i]& 0x0f) <<4)
				}
			}
		}
	}
//	wait_vblank();
}



/*static*/void DrvInitSaturn();

/*static*/int MemIndex()
{
	unsigned char *Next; Next = Mem;

	DrvZ80Rom1             = Next; Next += 0x28000;
	DrvZ80Rom2             = Next; Next += 0x10000;
	DrvSamples             = Next; Next += 0x10000;

	RamStart               = Next;

	DrvZ80Ram1             = Next; Next += 0x02000;
	DrvZ80Ram2             = Next; Next += 0x01000;
	DrvSpriteRam           = Next; Next += 0x00100;
	DrvPaletteRam          = Next; Next += 0x00800;
	DrvVideoRam            = Next; Next += 0x01000;
	DrvPalette             = (unsigned int*)Next; Next += (512 + 32) * sizeof(unsigned int);

	RamEnd                 = Next;

	DrvChars               = cache;//Next; Next += 0x1000 * 8 * 8;
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	DrvSprites             = &ss_vram[0x1100];//Next; Next += 0x1000 * 16 * 16;
#ifndef BMP
	DrvBackTiles           = cache+0x20000;//Next; Next += 0x4000 * 32;
#else
	DrvBackTiles           = cache+0x30000;//Next; Next += 0x4000 * 32;
#endif

	MemEnd                 = Next;

	return 0;
}

/*static*/void DrvSetVector(int Status)
{
	switch (Status) {
		case VECTOR_INIT: {
			DrvIrqVector = 0xff;
			break;
		}
		
		case YM2151_ASSERT: {
			DrvIrqVector &= 0xef;
			break;
		}
		
		case YM2151_CLEAR: {
			DrvIrqVector |= 0x10;
			break;
		}
		
		case Z80_ASSERT: {
			DrvIrqVector &= 0xdf;
			break;
		}
		
		case Z80_CLEAR: {
			DrvIrqVector |= 0x20;
			break;
		}
	}
	
	if (DrvIrqVector == 0xff) {
//		ZetSetVector(DrvIrqVector);
		CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);
	} else {
//		CZetSetVector(DrvIrqVector);
		CZetSetIRQLine(0, CZET_IRQSTATUS_ACK);
		nCyclesDone[1] += CZetRun(1000);
	}
}

/*static*/int DrvDoReset()
{
	unsigned int i;
	for (i = 0; i < 2; i++) {
		CZetOpen(i);
		CZetReset();
		if (i == 1) DrvSetVector(VECTOR_INIT);
		CZetClose();
	}
	

//	DACReset();
	
	DrvRomBank = 0;
	DrvSoundLatch = 0;
	DrvRearColour = 0;
	DrvRearDisable = 0;
	DrvHorizScrollLo = 0;
	DrvHorizScrollHi = 0;
	DrvRearHorizScrollLo = 0;
	DrvRearHorizScrollHi = 0;
	DrvSampleAddress = 0;

	return 0;
}

unsigned char __fastcall VigilanteZ80Read1(unsigned short a)
{
	switch (a) {
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #1 Read => %04X\n"), a);
		}
	}

	return 0;
}

void __fastcall VigilanteZ80Write1(unsigned short a, unsigned char d)
{
	if (a >= 0xc020 && a <= 0xc0df) {
		DrvSpriteRam[a - 0xc020] = d;
		return;
	}
	
	if (a >= 0xc800 && a <= 0xcfff) 
	{
		int Offset = a & 0x7ff;
	
		if (DrvPaletteRam[Offset]!=d)
		{
			int Bank = Offset & 0x400;
			int r, g, b;
			DrvPaletteRam[Offset] = d;
			
			Offset &= 0xff;
			r = (DrvPaletteRam[Bank + Offset + 0x000] ) & 0x1f;//0x1c;
			g = (DrvPaletteRam[Bank + Offset + 0x100] ) & 0x1f;//0x1c;
			b = (DrvPaletteRam[Bank + Offset + 0x200] ) & 0x1f;//0x1c;
		// 16 palettes	
			


//			colAddr[(Bank >> 2) + Offset] = RGB(r,g,b)+2;
			colAddr[(Bank >> 2) + Offset] = RGB(r,g,b);

/*
			if ((Bank >> 2) + Offset <256)
			{
				colAddr[(Bank >> 2) + Offset] = RGB(r,g,b);
				//spr
			}
			else
			{
				colAddr[(Bank >> 2) + Offset] = RGB(r,g,b);
			}
			
*/		}

		return;
	}

	if (a >= 0xd000 && a <= 0xdfff) 
	{
		a&=0xfff;
		DrvVideoRam[a]=d;
		fg_dirtybuffer[a>>1] = 1;
	}




	
	switch (a) {
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #1 Write => %04X, %02X\n"), a, d);
		}
	}
}

unsigned char __fastcall VigilanteZ80PortRead1(unsigned short a)
{
	a &= 0xff;
	
	switch (a) {
		case 0x00: {
			return 0xff - DrvInput[0];
		}
		
		case 0x01: {
			return 0xff - DrvInput[1];
		}
		
		case 0x02: {
			return 0xff - DrvInput[2];
		}
		
		case 0x03: {
			return DrvDip[0];
		}
		
		case 0x04: {
			return DrvDip[1];
		}
		
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #1 Port Read => %02X\n"), a);
		}
	}

	return 0;
}


void __fastcall VigilanteZ80PortWrite1(unsigned short a, unsigned char d)
{
	a &= 0xff;
	
	switch (a) {
		case 0x00: {
			DrvSoundLatch = d;
			CZetClose();
			CZetOpen(1);
			DrvSetVector(Z80_ASSERT);
			CZetClose();
			CZetOpen(0);
			return;
		}
		
		case 0x01: {
			// output port?
			return;
		}
		
		case 0x04: {
			DrvRomBank = d & 0x07;
			CZetMapArea(0x8000, 0xbfff, 0, DrvZ80Rom1 + 0x10000 + (DrvRomBank * 0x4000));
			CZetMapArea(0x8000, 0xbfff, 2, DrvZ80Rom1 + 0x10000 + (DrvRomBank * 0x4000));
			return;
		}
		
		case 0x80: {
			DrvHorizScrollLo = d;
			return;
		}
		
		case 0x81: {
			DrvHorizScrollHi = (d & 0x01) << 8;
			return;
		}
		
		case 0x82: {
			DrvRearHorizScrollLo = d;
			return;
		}
		
		case 0x83: {
			DrvRearHorizScrollHi = (d & 0x07) << 8;
			return;
		}
		
		case 0x84: {
			DrvRearColour = d & 0x0d;
			DrvRearDisable = d & 0x40;
			return;
		}
		
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #1 Port Write => %02X, %02X\n"), a, d);
		}
	}
}

unsigned char __fastcall VigilanteZ80Read2(unsigned short a)
{
	switch (a) {
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #2 Read => %04X\n"), a);
		}
	}

	return 0;
}

void __fastcall VigilanteZ80Write2(unsigned short a, unsigned char d)
{
	switch (a) {
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #2 Write => %04X, %02X\n"), a, d);
		}
	}
}

unsigned char __fastcall VigilanteZ80PortRead2(unsigned short a)
{
	a &= 0xff;
	
	switch (a) {
		case 0x01: {
//			return BurnYM2151ReadStatus();
			return ;
		}
		
		case 0x80: {
//			return DrvSoundLatch;
		}
		
		case 0x84: {
//			return DrvSamples[DrvSampleAddress];
		}
		
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #2 Port Read => %02X\n"), a);
		}
	}

	return 0;
}

void __fastcall VigilanteZ80PortWrite2(unsigned short a, unsigned char d)
{
	a &= 0xff;
	
	switch (a) {
		case 0x00: {
//			BurnYM2151SelectRegister(d);
			return;
		}
		
		case 0x01: {			
//			BurnYM2151WriteRegister(d);
			return;
		}
		
		case 0x80: {
//			DrvSampleAddress = (DrvSampleAddress & 0xff00) | ((d << 0) & 0x00ff);
			return;
		}
		
		case 0x81: {
//			DrvSampleAddress = (DrvSampleAddress & 0x00ff) | ((d << 8) & 0xff00);
			return;
		}
		
		case 0x82: {
//			DACSignedWrite(d);
//			DrvSampleAddress = (DrvSampleAddress + 1) & 0xffff;
			return;
		}
		
		case 0x83: {
			DrvSetVector(Z80_CLEAR);
			return;
		}
		
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #2 Port Write => %02X, %02X\n"), a, d);
		}
	}
}


/*static*/void VigilantYM2151IrqHandler(int Irq)
{
	if (Irq) {
		DrvSetVector(YM2151_ASSERT);
	} else {
		DrvSetVector(YM2151_CLEAR);
	}
}






/*static*/int CharPlaneOffsets[4]         = { 0x80000, 0x80004, 0, 4 };
/*static*/int CharXOffsets[8]             = { 0, 1, 2, 3, 64, 65, 66, 67 };
/*static*/int CharYOffsets[8]             = { 0, 8, 16, 24, 32, 40, 48, 56 };
/*static*/int SpritePlaneOffsets[4]       = { 0x200000, 0x200004, 0, 4 };
/*static*/int SpriteXOffsets[16]          = { 0, 1, 2, 3, 128, 129, 130, 131, 256, 257, 258, 259, 384, 385, 386, 387 };
/*static*/int SpriteYOffsets[16]          = { 0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120 };
/*static*/int BackTilePlaneOffsets[4]     = { 0, 2, 4, 6 };
/*static*/int BackTileXOffsets[32]        = { 1, 0, 9, 8, 17, 16, 25, 24, 33, 32, 41, 40, 49, 48, 57, 56, 65, 64, 73, 72, 81, 80, 89, 88, 97, 96, 105, 104, 113, 112, 121, 120 };
/*static*/int BackTileYOffsets[1]         = { 0 };

/*static*/int DrvInit()
{
	DrvInitSaturn();

	int nRet = 0, nLen;

	// Allocate and Blank all required memory
	Mem = NULL;
	MemIndex();
	nLen = MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);
	MemIndex();

//	DrvTempRom = (unsigned char *)malloc(0x80000);
	DrvTempRom = (unsigned char*)(0x00200000);

	// Load Z80 #1 Program Roms
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x00000,  0, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x10000,  1, 1); if (nRet != 0) return 1;
	
	// Load Z80 #2 Program Roms
	nRet = BurnLoadRom(DrvZ80Rom2 + 0x00000,  2, 1); if (nRet != 0) return 1;
	
	// Load and decode the chars
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  3, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000,  4, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(0x1000, 4, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, DrvChars);

//memset(DrvChars,0x00,4*8);
//memset(&DrvChars[0x20*4*8],0x33,4*8);
//	GfxDecode4Bpp(0x1000, 4, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, DrvChars);

	// Load and decode the sprites
	memset(DrvTempRom, 0, 0x80000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  5, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000,  6, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000,  7, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x30000,  8, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x40000,  9, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x50000, 10, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x60000, 11, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x70000, 12, 1); if (nRet != 0) return 1;

//	GfxDecode(0x1000, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x200, DrvTempRom, DrvSprites);
	GfxDecode4Bpp(0x1000, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x200, DrvTempRom, DrvSprites);
	// Load and decode the bg tiles
	memset(DrvTempRom, 0, 0x80000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000, 13, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000, 14, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000, 15, 1); if (nRet != 0) return 1;

//	GfxDecode4Bpp(0x4000, 4, 32, 1, BackTilePlaneOffsets, BackTileXOffsets, BackTileYOffsets, 0x80, DrvTempRom, DrvBackTiles);
	GfxDecode4Bpp(0x4000, 4, 32, 1, BackTilePlaneOffsets, BackTileXOffsets, BackTileYOffsets, 0x80, DrvTempRom, DrvBackTiles);

	Bitmap2Tile(DrvBackTiles);
	
	// Load sample Roms
	nRet = BurnLoadRom(DrvSamples + 0x00000, 16, 1); if (nRet != 0) return 1;
	// Setup the Z80 emulation
	CZetInit(2);
	CZetOpen(0);
	CZetSetReadHandler(VigilanteZ80Read1);
	CZetSetWriteHandler(VigilanteZ80Write1);
	CZetSetInHandler(VigilanteZ80PortRead1);
	CZetSetOutHandler(VigilanteZ80PortWrite1);
	
	// read 0 // write 1 // fetch 2
	CZetMapArea(0x0000, 0x7fff, 0, DrvZ80Rom1             );
	CZetMapArea(0x0000, 0x7fff, 2, DrvZ80Rom1             );
	CZetMapArea(0x8000, 0xbfff, 0, DrvZ80Rom1 + 0x10000   );
	CZetMapArea(0x8000, 0xbfff, 2, DrvZ80Rom1 + 0x10000   );
	CZetMapArea(0xc800, 0xcfff, 0, DrvPaletteRam          );
	CZetMapArea(0xc800, 0xcfff, 2, DrvPaletteRam          );
	CZetMapArea(0xd000, 0xdfff, 0, DrvVideoRam            );
//	CZetMapArea(0xd000, 0xdfff, 1, DrvVideoRam            );
	CZetMapArea(0xd000, 0xdfff, 2, DrvVideoRam            );
	CZetMapArea(0xe000, 0xefff, 0, DrvZ80Ram1             );
	CZetMapArea(0xe000, 0xefff, 1, DrvZ80Ram1             );
	CZetMapArea(0xe000, 0xefff, 2, DrvZ80Ram1             );
	CZetMemEnd();
	CZetClose();
	
	CZetOpen(1);
	CZetSetReadHandler(VigilanteZ80Read2);
	CZetSetWriteHandler(VigilanteZ80Write2);
	CZetSetInHandler(VigilanteZ80PortRead2);
	CZetSetOutHandler(VigilanteZ80PortWrite2);	
	CZetMapArea(0x0000, 0xbfff, 0, DrvZ80Rom2             );
	CZetMapArea(0x0000, 0xbfff, 2, DrvZ80Rom2             );
	CZetMapArea(0xf000, 0xffff, 0, DrvZ80Ram2             );
	CZetMapArea(0xf000, 0xffff, 1, DrvZ80Ram2             );
	CZetMapArea(0xf000, 0xffff, 2, DrvZ80Ram2             );
	CZetMemEnd();
	CZetClose();
	
//	BurnSetRefreshRate(55.0);
	nCyclesTotal[0] = 3579645 / 55 /2;
	nCyclesTotal[1] = 3579645 / 55 /2;
	
//	GenericTilesInit();
//	BurnYM2151Init(3579645, 25.0);
//	BurnYM2151SetIrqHandler(&VigilantYM2151IrqHandler);	
//	DACInit(0, 1);
//	DACSetVolShift(1);
	
	DrvDoReset();
// 	memset4_fast(ss_font,0x00000000,0x10000);
//		drawWindow(0,224,240,6,66);
	return 0;
}


//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/void initColors()
{
//#ifdef VBTLIB

	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colAddr             = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);//
	colBgAddr2          = (Uint16*)SCL_AllocColRam(SCL_NBG0,ON);
//	SCL_AllocColRam(SCL_NBG1,ON);
//	SCL_SetColRamOffset(SCL_NBG2, 1,OFF);
#ifndef BMP
	colBgAddr           = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
	for(int i=0;i<4096;i++)
		colBgAddr2[i]=0x8001;
#else
	colBgAddr           = (Uint16*)SCL_AllocColRam(SCL_NBG2,ON);
	for(int i=0;i<4096;i++)
		colBgAddr2[i]=0x8001;

	SCL_AllocColRam(SCL_NBG3,OFF); // inutile ?

	SCL_AllocColRam(SCL_NBG1,OFF);
	SCL_SetColRam(SCL_NBG1,8,8,palette);
#endif

}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/void initLayers()
{
    Uint16	CycleTb[]={
		0x46ff, 0xffff, //A0
		0xffff, 0xffff,	//A1
		0x025e,0xeeee,   //B0
		0xffff, 0xeeee  //B1
	};
// nbg0 b1+a0
//4 tiles or bitmap
//0 map
 	SclConfig	scfg;

	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD;
	scfg.platesize     = SCL_PL_SIZE_2X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;
	scfg.datatype      = SCL_CELL;
		scfg.plate_addr[0] = (Uint32)ss_map;
		scfg.plate_addr[1] = (Uint32)ss_map;
		scfg.plate_addr[2] = (Uint32)ss_map;
		scfg.plate_addr[3] = (Uint32)ss_map;
	SCL_SetConfig(SCL_NBG0, &scfg);

	scfg.dispenbl      = ON;
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.plate_addr[0] = (Uint32)ss_map2;
	scfg.plate_addr[1] = (Uint32)ss_map2;
	scfg.plate_addr[2] = (Uint32)ss_map2;
	scfg.plate_addr[3] = (Uint32)ss_map2;
#ifndef BMP
	SCL_SetConfig(SCL_NBG1, &scfg);
#else
	SCL_SetConfig(SCL_NBG2, &scfg);
	scfg.dispenbl      = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
		scfg.plate_addr[0] = (Uint32)ss_font;
	SCL_SetConfig(SCL_NBG1, &scfg);
#endif
/*
#ifdef USE_MAP		  // 3 nbg
	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_2X2;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD;
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
		scfg.plate_addr[0] = (Uint32)ss_map;
		scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG2, &scfg);
#endif*/
/********************************************/	
//	SCL_InitConfigTb(&scfg);
/*	scfg.dispenbl 		 = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.coltype 			 = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 		 = SCL_BITMAP;
	scfg.mapover			 = SCL_OVER_0;
	scfg.plate_addr[0]	 = (Uint32)ss_font;
// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);
*/
	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/void initSpritesS1(void)
{
    int i;

//	initSprites(256-1,224-1,0,0,-128,0);
	initSprites(352-1,224-1,0,0,0,0);

	for (i=3;i<27 ;i++ )
	{
		ss_sprite[i].control = ( JUMP_NEXT | FUNC_NORMALSP );
//		ss_sprite[i].drawMode   = ( COLOR_1 | ECD_DISABLE | COMPO_REP);		
	}
//	SCL_SET_SPCLMD(1);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void Bitmap2Tile(unsigned char *DrvBackTiles)
{
	char titi[8*256];
	for (int m=0;m<0x30000;m+=8*256)
	{
		memcpy(titi,&DrvBackTiles[m],8*256);

		for (int l=0; l<256; l+=4)
		{
			for (int k=0;k<256*8 ; k+=256)
			{
				DrvBackTiles[m+0+l*8+k/64]=titi[0+k+l];
				DrvBackTiles[m+1+l*8+k/64]=titi[1+k+l];
				DrvBackTiles[m+2+l*8+k/64]=titi[2+k+l];
				DrvBackTiles[m+3+l*8+k/64]=titi[3+k+l];
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/void DrvInitSaturn()
{
//	nBurnSoundLen = 128;
	nBurnSprites = 27;
	nBurnLinescrollSize = 0x380;

	SS_MAP  = ss_map  =(Uint16 *)SCL_VDP2_VRAM_B1;
	SS_MAP2 = ss_map2 =(Uint16 *)SCL_VDP2_VRAM_A1+0x8000;
#ifdef BMP
	SS_FONT = ss_font = (SCL_VDP2_VRAM_A1+0x00000); //(Uint16 *)SCL_VDP2_VRAM_A1;//(Uint16 *)SCL_VDP2_VRAM_B0;
#endif
	SS_CACHE= cache   =(Uint8 *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;
	ss_OtherPri     = (SclOtherPriRegister *)SS_OTHR;
	ss_BgColMix		= (SclBgColMixRegister *)SS_BGMIX;
	ss_sprite		= (SprSpCmd *)SS_SPRIT;
//	ss_vram			= (UINT8 *)SS_SPRAM;
	ss_scl			= (Fixed32 *)SS_SCL;

//	Scl_s_reg.tvmode &= 0xfff0;
//	Scl_s_reg.tvmode |= 0x0001;
//	if(SclProcess == 0)	SclProcess = 1;

//    SPR_WRITE_REG(SPR_W_TVMR, 0x0007 & SPR_TV_NORMAL);
	
//	SPR_SetEraseData( 0x0000, 0, 0, 320-1, 224-1 );

	SS_SET_S0PRIN(4);
	SS_SET_N0PRIN(4);
#ifndef BMP
	SS_SET_N2PRIN(6);
	SS_SET_N1PRIN(2);
#else
	SS_SET_N2PRIN(2);
	SS_SET_N1PRIN(6);
#endif

	SS_SET_N0SPRM(1);  // 1 for special priority

	SS_SET_CCRTMD(0); // color calc ratio mode bit
	SS_SET_CCMD(0);   // color calc mode bit

	SS_SET_N0CCEN(1); // color calc enable
	SS_SET_N0SCCM(2); // color calc mode 1 on tile, 2 by dot, 3 color data with msb
	SS_SET_N0CCRT(23); // color calc ratio

	ss_regs->specialcode=0x0001; // sfcode, upper 8bits, function b, lower 8bits function a
	ss_regs->specialcode_sel=0; // sfsel, bit 0 for nbg0 // 1 sfcs, bit 0 = 1 for funcion code b, 0 for function code a

    /* Enable line and vertical cell scroll for NBG0 */
#ifdef VBTLIB
	(*(Uint16 *)0x25F8009A) = 0x0003; 
	(*(Uint16 *)0x25F80020) = 0x0303;
#endif
//SclBgColMixRegister  *SclRealBgColMix = (SclBgColMixRegister *)0x25F80108;
//	memset(aVRAM+0x880,0x00,0x40000);
//	memset(cache,0,sizeof(cache));

	initLayers();

//	initPosition();
	initColors();
	initSpritesS1();
	initScrolling(ON,SCL_VDP2_VRAM_B1+(0x20000-0xC00));
	drawWindow(0,224,240,0,64);

//	initScrolling();
//	drawGrey();
#ifndef BMP
int tile_start = 0x1000;
#else
int tile_start = 0x1800;
#endif

	Uint16 *vbt = ((Uint16*)ss_scl);
//	for (Offset = 0; Offset < 0x1000; Offset += 2) 
	for (int Offset = 0; Offset < 96; Offset += 2) 
	{
		vbt[Offset]=128;
	}

	memset(fg_dirtybuffer,1,2048);

	for(int i=0;i<0x1000;i+=2)
	{
		if( ((i>>1)/64)*8 <128)
			ss_map2[i]=0;
		else
			ss_map2[i]=1;


		int	sx = (i>>1) & 0x3f;//% 32;

		if(sx>=48)
		{
			vbmap[0][(i>>1)]    =vbmap[0][(i>>1)-48];     // ok
			vbmap[1][(i>>1)]    = tile_start+(i>>1);		  // ok
			vbmap[1][(i>>1)-48] = tile_start+(i>>1);		  // ok
		}
		else
		{
			vbmap[0][(i>>1)]    = tile_start+(i>>1);		  // ok
		}

		if(sx<16)
		{
//			vbmap[2][(i>>1)+32] = 0x1000+(64*32*2)+(i>>1);// ok // fin de map 2, 32 a 48 // ou prendre tout ?
		}
		if(sx>=16) // &&
		{
			vbmap[3][(i>>1)-16]   = tile_start+(64*32*2)+(i>>1); // map 3 de 0 a 48
//			if(sx<32)
//				vbmap[3][(i>>1)+32]= vbmap[3][(i>>1)-16]; // map 3 de 48 a 64
		}

		if(sx<32)
		{
			vbmap[1][(i>>1)+16] = tile_start+(64*32*1)+(i>>1);// ok
			vbmap[2][(i>>1)+32] = tile_start+(64*32*2)+(i>>1);
//			vbmap[2][(i>>1)+32] = (64*32*2)+(i>>1);// ok
//			vbmap[3][(i>>1)]    = (64*32*3)+(i>>1);
		}
		else
		{
			vbmap[2][(i>>1)-32] = tile_start+(64*32*1)+(i>>1);// ok // début map 2, 0 a 32
			if(sx<48)
				vbmap[2][(i>>1)+16] = vbmap[2][(i>>1)-32]; // ok // map 2, 48 a 64
//			vbmap[3][(i>>1)-32] = (64*32*2)+(i>>1);// ok
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/int DrvExit()
{
	CZetExit();
//	BurnYM2151Exit();
//	BurnYM2203Exit();
	
//	GenericTilesExit();
	
	if (Mem) {
		free(Mem);
		Mem = NULL;
	}
	
	DrvRomBank = 0;
	DrvSoundLatch = 0;
	DrvIrqVector = 0;
	DrvRearColour = 0;
	DrvRearDisable = 0;
	DrvHorizScrollLo = 0;
	DrvHorizScrollHi = 0;
	DrvRearHorizScrollLo = 0;
	DrvRearHorizScrollHi = 0;
	DrvSampleAddress = 0;
	
	DrvHasYM2203 = 0;
	DrvKikcubicDraw = 0;

	return 0;
}
int bg=-1;
/*static*/void DrvRenderBackground()
{
	int Scroll = 0x17a - (DrvRearHorizScrollLo + DrvRearHorizScrollHi);
	if (Scroll > 0) Scroll -= 2048;
//	SCL_Open();
#ifndef BMP
	ss_reg->n1_move_x = -Scroll<<16;
#else
	ss_reg->n2_move_x = -Scroll;
#endif

	if (bg!=-Scroll/384)
	{
		bg=-Scroll/384;

		for (unsigned int Offset = 0; Offset < 0x1000; Offset += 2) 
		{
			UINT16 *map2 = &ss_map2[Offset];
//			map2[0] = 0; // couleur précalculée
			map2[1] = vbmap[bg][Offset/2];
		}
	}

		for (unsigned int i = 0; i < 16; i++) 
		{
			int r, g, b;

			r = (4/3*(DrvPaletteRam[0x400 + 16 * DrvRearColour + i])) & 0xff;
			g = (4/3*(DrvPaletteRam[0x500 + 16 * DrvRearColour + i])) & 0xff;
			b = (4/3*(DrvPaletteRam[0x600 + 16 * DrvRearColour + i])) & 0xff;

			colBgAddr[i] = RGB(r,g,b);//BurnHighCol(r, g, b, 0);

			r = (4/3*(DrvPaletteRam[0x400 + 16 * DrvRearColour + 32 + i])) & 0xff;
			g = (4/3*(DrvPaletteRam[0x500 + 16 * DrvRearColour + 32 + i])) & 0xff;
			b = (4/3*(DrvPaletteRam[0x600 + 16 * DrvRearColour + 32 + i])) & 0xff;

			colBgAddr[16 + i] = RGB(r,g,b);//BurnHighCol(r, g, b, 0);
		}
}

/*static*/void DrvDrawForeground(int Priority, int Opaque)
{
	int Scroll = ((DrvHorizScrollLo + DrvHorizScrollHi)& 0x1ff) +128;
	unsigned int Offset;
	INT32 ScrollBg = 0x17a - (DrvRearHorizScrollLo + DrvRearHorizScrollHi);
	if (ScrollBg > 0) Scroll -= 2048;

	UINT16 *vbt = ((UINT16*)ss_scl);
//	ss_reg->n2_move_x = Scroll;
	for (Offset = 0; Offset < 0x1000; Offset += 2) 
	{
		if(Offset>=96 && Offset <448)
			vbt[Offset] =Scroll;

		if(fg_dirtybuffer[Offset>>1])
		{
			int Attr = DrvVideoRam[Offset + 1];
			int Colour = Attr & 0x0f;
			int Tile = DrvVideoRam[Offset + 0] | ((Attr & 0xf0) << 4);

			UINT16 *map = &ss_map[Offset]; 



			if( Colour >= 4) // pas de transp
			{
//				map[0] = map[0x2000] = Colour + (((Colour & 0x0c) == 0x0c || Offset <=96) ?0x2000:0x0000);
				if(Offset <=768)
					map[0] = map[0x2000] = Colour + 0x2000;
				else
					map[0] = map[0x2000] = Colour + (((Colour & 0x0c) == 0x0c) ?0x2000:0x0000);

//				map[0] = map[0x2000] = Colour + (((Colour & 0x0c) == 0x0c || Offset <=96) ?0x2000:0x0000);
			//	map[1] = map[0x2001] = (((Colour & 0x0c) == 0x0c) ?7:Tile);
				map[1] = map[0x2001] = Tile;
			}
			else	  // transparence
			{
				map[0] = map[0x2000] = Colour + 0x1000;
				map[1] = map[0x2001] = Tile;
			}

			fg_dirtybuffer[Offset>>1]=0;
		}
	}
}

/*static*/void DrvDrawSprites()
{
	int DrvSpriteRamSize = 0xc0;
//	if (DrvKikcubicDraw) DrvSpriteRamSize = 0x100;
//	unsigned int Offset;

//	for (Offset = 0; Offset < DrvSpriteRamSize; Offset += 8) {
	for (int i = 0; i < DrvSpriteRamSize; i += 8) 
	{
		int Code, Colour, sx, sy, h,flip;//xFlip, yFlip, h;

		Code = DrvSpriteRam[i + 4] | ((DrvSpriteRam[i + 5] & 0x0f) << 8);
		Colour = DrvSpriteRam[i + 0] & 0x0f;
		sx = (DrvSpriteRam[i + 6] | ((DrvSpriteRam[i + 7] & 0x01) << 8));
		sy = 256 + 128 - (DrvSpriteRam[i + 2] | ((DrvSpriteRam[i + 3] & 0x01) << 8));
		flip = (DrvSpriteRam[i + 5] & 0xC0)>>2;
		h = 1 << ((DrvSpriteRam[i + 5] & 0x30) >> 4);
		sy -= 16 * h;

		Code &= ~(h - 1);

			int delta=(i>>3)+3;
			ss_sprite[delta].ax = sx-128;
			ss_sprite[delta].ay = sy;
		//	ss_sprite[delta].color      = ((int)colAddr)>>3 | ((Colour<<2));//Colour<<4;
			ss_sprite[delta].color      = Colour<<4;
			ss_sprite[delta].control    = ( JUMP_NEXT | FUNC_NORMALSP | flip);
			ss_sprite[delta].drawMode   = ( COLOR_0 | ECD_DISABLE | COMPO_REP);		
			ss_sprite[delta].charSize   = 0x200|(h<<4);  //0x100 16*16
			ss_sprite[delta].charAddr   = 0x220+(Code<<4);

/*
		
		if (0) {
			sx -= 64;
		} else {
			sx -= 128;
		}*/
	//	int y;

	//	for (y = 0; y < h; y++) 
		//	{
			/*int c = Code;

			if (yFlip) {
				c += h - 1 - y;
			} else {
				c += y;
			}
			*/
/*
			if (sx > 16 && sx < (nScreenWidth - 16) && (sy + (16 * y)) > 16 && (sy + (16 * y)) < (nScreenHeight - 16)) {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY(pTransDraw, c, sx, sy + (16 * y), Colour, 4, 0, 0, DrvSprites);
					} else {
						Render16x16Tile_Mask_FlipX(pTransDraw, c, sx, sy + (16 * y), Colour, 4, 0, 0, DrvSprites);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY(pTransDraw, c, sx, sy + (16 * y), Colour, 4, 0, 0, DrvSprites);
					} else {
						Render16x16Tile_Mask(pTransDraw, c, sx, sy + (16 * y), Colour, 4, 0, 0, DrvSprites);
					}
				}
			} else {
				if (xFlip) {
					if (yFlip) {
						Render16x16Tile_Mask_FlipXY_Clip(pTransDraw, c, sx, sy + (16 * y), Colour, 4, 0, 0, DrvSprites);
					} else {
						Render16x16Tile_Mask_FlipX_Clip(pTransDraw, c, sx, sy + (16 * y), Colour, 4, 0, 0, DrvSprites);
					}
				} else {
					if (yFlip) {
						Render16x16Tile_Mask_FlipY_Clip(pTransDraw, c, sx, sy + (16 * y), Colour, 4, 0, 0, DrvSprites);
					} else {
						Render16x16Tile_Mask_Clip(pTransDraw, c, sx, sy + (16 * y), Colour, 4, 0, 0, DrvSprites);
					}
				}
			}
*/
//		}
	}
}

/*static*/void DrvDraw()
{
//	BurnTransferClear();	
		
	if (DrvRearDisable) {
		DrvDrawForeground(0, 1);
//		DrvDrawSprites();
//		DrvDrawForeground(1, 0);
	} else {
		DrvRenderBackground();
		DrvDrawForeground(0, 0);
//		DrvDrawSprites();
//		DrvDrawForeground(1, 0);
	}
//	DrvDrawForeground(0, 0);
	DrvDrawSprites();
//	BurnTransferCopy(DrvPalette);
}


/*static*/int DrvFrame()
{
//	SS_SET_N0CCRT(50);

	int nInterleave = nBurnSoundLen;
	int nFireNmiEveryFrames = 3;
	if (nBurnSoundRate == 11025) nFireNmiEveryFrames = 2;
	if (nBurnSoundRate == 44100) nFireNmiEveryFrames = 6;
	if (nBurnSoundRate == 48000) nFireNmiEveryFrames = 7;
	int nSoundBufferPos = 0;
	
	if (DrvReset) DrvDoReset();

	DrvMakeInputs();

	nCyclesDone[0] = nCyclesDone[1] = 0;
	
	CZetNewFrame();
	unsigned int i;

//	nBurnSprites = 27;


	for (i = 0; i < nInterleave; i++) {
		int nCurrentCPU, nNext;

		// Run Z80 #1
		nCurrentCPU = 0;
		CZetOpen(nCurrentCPU);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesDone[nCurrentCPU] += CZetRun(nCyclesSegment);
		if (i == (nInterleave - 1)) CZetRaiseIrq(0);
		CZetClose();

		// Run Z80 #2
		nCurrentCPU = 1;
		CZetOpen(nCurrentCPU);
		if (!DrvHasYM2203) {
			nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
			nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
			nCyclesDone[nCurrentCPU] += CZetRun(nCyclesSegment);
		} else {
//			BurnTimerUpdate(i * (nCyclesTotal[nCurrentCPU] / nInterleave));
		}
		if ((i % nFireNmiEveryFrames) == 0) {
			CZetNmi();
		}
		CZetClose();
/*		
		if (pBurnSoundOut) {
			int nSegmentLength = nBurnSoundLen / nInterleave;
			short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			CZetOpen(1);
			if (DrvHasYM2203) {
//				BurnYM2203Update(pSoundBuf, nSegmentLength);
			} else {
//				BurnYM2151Render(pSoundBuf, nSegmentLength);
			}
			CZetClose();
//			DACUpdate(pSoundBuf, nSegmentLength);
			nSoundBufferPos += nSegmentLength;
		}
*/
	}
	
	if (DrvHasYM2203) {
		CZetOpen(1);
//		BurnTimerEndFrame(nCyclesTotal[1]);
		CZetClose();
	}
/*	
	if (pBurnSoundOut) {
		int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
		
		if (nSegmentLength) {
			short* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			CZetOpen(1);
			if (DrvHasYM2203) {
//				BurnYM2203Update(pSoundBuf, nSegmentLength);
			} else {
//				BurnYM2151Render(pSoundBuf, nSegmentLength);
			}
			CZetClose();
//			DACUpdate(pSoundBuf, nSegmentLength);
		}
	}
*/	
//	if (pBurnDraw) 
	{
			DrvDraw();	
	}
	return 0;
}
#undef VECTOR_INIT
#undef YM2151_ASSERT
#undef YM2151_CLEAR
#undef Z80_ASSERT
#undef Z80_CLEAR



