
// 43-44 fps with CZ80 main & RAZE slave
// 17-18 fps with RAZE main & CZ80 slave
// FB Alpha Penguin-Kun Wars Driver Module
// Based on MAME Driver by David Haywood and Phil Stroffolino
#define CZ80 1
#define RAZE 1  // `EMULATE_R_REGISTER obligatoire
//#define USE_RAZE0 1
#define USE_RAZE1 1
#define CPU2_ENABLED 1
#define charSize(x, y) ((Uint16)( ((x >> 3) << 8) | (y)) )

#include "d_sys2.h"
#include "d_sys1_common.c"
UINT8 *CurrentBank = NULL; // vbt à mettre dans drvexit !!!

int ovlInit(char *szShortName)
{
	cleanBSS();

	struct BurnDriver nBurnDrvChplftb = {
		"chplftb", "sys2",
		"Choplifter (Alternate)",
		ChplftbRomInfo, ChplftbRomName, ChplftbInputInfo, ChplftbDIPInfo,
		ChplftbInit, System1Exit, System1Frame
	};

	struct BurnDriver nBurnDrvWbml = {
		"wbml", "sys2",
		"Wonder Boy in Monster Land (Jp New)",
		wbmlRomInfo, wbmlRomName, MyheroInputInfo, WbmlDIPInfo,
		WbmlInit, System1Exit, System1Frame
	};

	struct BurnDriver nBurnDrvWbmlb = {
		"wbmlb", "sys2",
		"Wonder Boy in Monster Land (English bootleg set 1)",
		wbmlbRomInfo, wbmlbRomName, MyheroInputInfo, WbmlDIPInfo,
		WbmljbInit, System1Exit, System1Frame
	};

	struct BurnDriver nBurnDrvWbmlvc = {
		"wbmlvc", "sys2",
		"Wonder Boy in Monster Land (EN VC)",
		wbmlvcRomInfo, wbmlvcRomName, MyheroInputInfo, WbmlDIPInfo,
		WbmljbInit, System1Exit, System1Frame
	};

	if (strcmp(nBurnDrvChplftb.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvChplftb,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvWbml.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvWbml,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvWbmlb.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvWbmlb,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvWbmlvc.szShortName, szShortName) == 0)memcpy(shared,&nBurnDrvWbmlvc,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}

/*static*/ inline void chplft_bankswitch_w (UINT8 d)
{
	System1BankRomNoDecode(((d & 0x0c)>>2));
	System1BankSwitch = d;
}

void __fastcall ChplftZ801PortWrite(UINT16 a, UINT8 d)
{
	a &= 0xff;
	switch (a)
	{
		case 0x14: { system1_soundport_w(d);    return; }
		case 0x15: { chplft_bankswitch_w(d);   return; }
	}
}

void __fastcall System2Z801PortWrite(UINT16 a, UINT8 d)
{
	a &= 0x1f;
	switch (a) 
	{
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
			ppi8255_w(0, a & 3, d);
		return;
	}
}

UINT8 __fastcall ChplftZ801PortRead(UINT16 a)
{
	a &= 0xff;
	switch (a)
	{
		case 0x00: return 0xff - System1Input[0];
		case 0x04: return 0xff - System1Input[1];
		case 0x08: return 0xff - System1Input[2];
		case 0x0c: return System1Dip[0];
		case 0x0d: return System1Dip[1];
		case 0x10: return System1Dip[1];
		case 0x15: return System1BankSwitch;
		case 0x16: return System1BgBankLatch;
		case 0x19: return System1BankSwitch;
	}
	return 0;
}

UINT8 __fastcall System2Z801PortRead(UINT16 a)
{
	a &= 0x1f;
	switch (a) 
	{
		case 0x00: return 0xff - System1Input[0];
		case 0x04: return 0xff - System1Input[1];
		case 0x08: return 0xff - System1Input[2];
		case 0x0c: return System1Dip[0];
		case 0x0d: return System1Dip[1];
		case 0x10: return System1Dip[1];
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17: return ppi8255_r(0, a & 3);
		case 0x19: return System1BankSwitch;
	}
	return 0;
}

inline void System2_videoram_bank_latch_w (UINT8 d)
{
	if(System1BgBankLatch != d)
	{
		System1BgBankLatch = d;
		System1BgBank = (d >> 1) & 0x03;	// Select 4 banks of 4k, bit 2,1 
		CurrentBank = (UINT8 *)(System1VideoRam + System1BgBank * 0x1000);
		RamStart1 = CurrentBank - 0xe000;
	}
}

inline void system2_foregroundram_w(UINT16 a, UINT8 d) 
{
//	RamStart1						= System1VideoRam-0xe000;	 // fg
	if(RamStart1[a]!=d)
	{
		RamStart1[a] = d;
		
		switch (a&0x7ff)
		{
			case 0x740: 
			case 0x742:
			case 0x744:
			case 0x746:
				map_dirty[d&7]=1;
			break;
		}		

		a&=~1;

		unsigned int Code = (RamStart1[a + 1] << 8) | RamStart1[a + 0];
		Code = ((Code >> 4) & 0x800) | (Code & 0x7ff);

		unsigned int x = map_offset_lut[a&0x7ff];
		UINT16 *map = &ss_map2[x];		
		map[0] = (Code >> 5) & 0x3f; // |(((RamStart[a + 1] & 0x08)==8)?0x2000:0x0000);;//color_lut[Code];
		map[1] = Code & (System1NumTiles-1);
	}
}

static void __fastcall System2Z801ProgWrite(UINT16 a, UINT8 d)
{
	if (a >= 0xf000 && a <= 0xf3ff) { System1BgCollisionRam[a - 0xf000] = 0x7e; return; }
	if (a >= 0xf800 && a <= 0xfbff) { System1SprCollisionRam[a - 0xf800] = 0x7e; return; }
	if (a >= 0xe000 && a <= 0xefff)
	{
		if(System1BgBank==0 && a <= 0xe7ff)
		{
			system2_foregroundram_w(a, d);
			return;
		}


		if(CurrentBank[(a & 0xfff)]!=d)
		{
			CurrentBank[(a & 0xfff)] = d;
			a&=0xffe;

			unsigned int Code = (CurrentBank[a+1]<<8)|CurrentBank[a];
			Code = ((Code >> 4) & 0x800) | (Code & 0x7ff);

			unsigned int x = map_offset_lut[a&0x7ff];
			a = (System1BgBank<<1) + (a>>11);
			map_dirty[a] = 1;

			UINT16 *map = &map_cache[x+(a<<12)];
			map[0] = ((Code >> 5) & 0x3f);
			map[1] = Code & (System1NumTiles-1);
		}
		return;
	}
	if (a >= 0xd800 && a <= 0xd9ff) { system1_paletteram_w(a,d); return; }
	if (a >= 0xda00 && a <= 0xdbff) { system1_paletteram2_w(a,d); return; }
	if (a >= 0xdc00 && a <= 0xddff) { system1_paletteram3_w(a,d); return; }
}

static UINT8 __fastcall System2Z801ProgRead(UINT16 a)
{
	if (a >= 0xe000 && a <= 0xefff) 
	{ 
		return CurrentBank[a & 0xfff];
	}
	return 0;
}

static void System2PPI0WriteA(UINT8 data)
{
	system1_soundport_w(data);
}

static void System2PPI0WriteB(UINT8 data)
{
	System2_bankswitch_w(data);
}

static void System2PPI0WriteC(UINT8 data)
{
	if(data & 0x80)
	{
		z80_raise_IRQ(0x20);
//		z80_emulate(0);
	}
	else
	{
		z80_lower_IRQ();
//		z80_emulate(0);
	}

	System2_videoram_bank_latch_w(data);
}

void System1BankRomNoDecode(UINT32 System1RomBank)
{
	int BankAddress = (System1RomBank << 14) + 0x10000;
	CZetMapArea(0x8000, 0xbfff, 0, System1Rom1 + BankAddress);
	CZetMapArea(0x8000, 0xbfff, 2, System1Rom1 + BankAddress);
}

inline void System2_bankswitch_w (UINT8 d)
{
	System1BankRom((d & 0x0c) >> 2);
	System1BankSwitch = d;
}

void __fastcall ChplftZ801ProgWrite(UINT16 a, UINT8 d)
{
	if (a >= 0xe000 && a <= 0xe7bf) { system1_foregroundram_w(a,d); return; }
	if (a >= 0xe800 && a <= 0xeeff) { system1_backgroundram_w(a,d); return; }
	if (a >= 0xf000 && a <= 0xf3ff) { System1BgCollisionRam[a - 0xf000] = 0x7e; return; }
	if (a >= 0xf800 && a <= 0xfbff) { System1SprCollisionRam[a - 0xf800] = 0x7e; return; }
//	if (a >= 0xd800 && a <= 0xd9ff) { system1_paletteram_w(a,d); return; }
	if (a >= 0xda00 && a <= 0xdbff) { system1_paletteram2_w(a,d); return; }
	if (a >= 0xdc00 && a <= 0xddff) { system1_paletteram3_w(a,d); return; }
   if (a == 0xefbd) { ss_reg->n0_move_y = d<<16; return; }
}

int System1CalcSprPalette()
{
	Uint16 *color = &colAddr[0];
	
	for (unsigned int i = 0; i <512; i++) 
	{
		*color++ = cram_lut[System1PaletteRam[i]];
	}
	return 0;
}

static void wbmljb_decode()
{
	return; // fake decode function
}

static void wbml_decode()
{
	mc8123_decrypt_rom(1, 4, System1Rom1, System1Rom1 + 0x20000, (UINT8*)0x002FC000);
}

int ChplftbInit()
{
	int nRet;
	nBurnLinescrollSize = 0x380;
	System1ColourProms = 1;
	System1BankedRom = 1;

	nRet = System1Init(3, 0x8000, 1, 0x8000, 3, 0x8000, 4, 0x8000, 1);
//	initColors2();
//	initLayers2();
	nBurnFunction = System1CalcSprPalette;//System1CalcPalette;
	ss_reg->n1_move_y =  0 <<16;
	ss_reg->n1_move_x =  0 <<16;
	make_cram_lut();
	System1CalcPalette();
//	System1CalcSprPalette();
	SS_SET_S0PRIN(4);
	SS_SET_N1PRIN(7);
	SS_SET_N2PRIN(4);
	SS_SET_N0PRIN(5);

	SS_SET_N0SPRM(1);  // 1 for special priority

	initScrolling(ON,SCL_VDP2_VRAM_B0+0x4000);
	drawWindow(0,224,0,0,64);
	
	CurrentBank						= System1VideoRam;			
	RamStart						= System1BgRam-0xe800;
	RamStart1						= System1VideoRam-0xe000;
/*	CZetOpen(0);

	CZetMapArea(0xd800, 0xddff, 0, System1PaletteRam);
//	CZetMapArea(0xd800, 0xddff, 1, System1PaletteRam);
	CZetMapArea(0xd800, 0xddff, 1, System1PaletteRam);
//	CZetMapArea(0xd800, 0xddff, 2, System1PaletteRam);

	CZetMapArea(0xe7c0, 0xe7ff, 0, System1ScrollXRam);
	CZetMapArea(0xe7c0, 0xe7ff, 1, System1ScrollXRam);
	CZetMapArea(0xe7c0, 0xe7ff, 2, System1ScrollXRam);


//	CZetMapArea(0xe000, 0xe7ff, 0, System1VideoRam); //read
//	CZetMapArea(0xe000, 0xe7ff, 1, System1VideoRam);	 //write
	CZetMapArea(0xe000, 0xe7ff, 2, System1VideoRam); //fetch
	CZetMapArea(0xe800, 0xeeff, 0, System1BgRam);
//	CZetMapArea(0xe800, 0xeeff, 1, System1BgRam);
	CZetMapArea(0xe800, 0xeeff, 2, System1BgRam);

	CZetSetWriteHandler(ChplftZ801ProgWrite);

	CZetSetInHandler   (ChplftZ801PortRead);
	CZetSetOutHandler  (ChplftZ801PortWrite);
	CZetClose();

	ppi8255_init(1);
	PPI0PortWriteA = System2PPI0WriteA;
	PPI0PortWriteB = System2PPI0WriteB;
	PPI0PortWriteC = NULL;
*/
	CZetOpen(0);

	CZetMapArea(0xe7c0, 0xe7ff, 0, System1ScrollXRam);
	CZetMapArea(0xe7c0, 0xe7ff, 1, System1ScrollXRam);
	CZetMapArea(0xe7c0, 0xe7ff, 2, System1ScrollXRam);

//	CZetMapArea(0xe000, 0xe7ff, 0, System1VideoRam); //read
//	CZetMapArea(0xe000, 0xe7ff, 1, System1VideoRam);	//write
	CZetMapArea(0xe000, 0xe7ff, 2, System1VideoRam); //fetch

	CZetMapArea(0xe800, 0xeeff, 0, System1BgRam);
//	CZetMapArea(0xe800, 0xeeff, 1, System1BgRam);
	CZetMapArea(0xe800, 0xeeff, 2, System1BgRam);

	CZetSetInHandler(ChplftZ801PortRead);
	CZetSetOutHandler(ChplftZ801PortWrite);
	CZetClose();

	return nRet;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void CommonWbmlInit()
{
	System1SpriteRam = &System1Ram1[0x1000];
	System1PaletteRam = &System1Ram1[0x1800];	 // ? garder

	make_cram_lut();
	System1CalcPalette();

//   nBurnFunction = System1CalcPalette;
//	nBurnFunction = System1CalcSprPalette;//System1CalcPalette;
	ss_reg->n1_move_x = 4<<16;
	CurrentBank						= System1VideoRam;
	RamStart1						= System1VideoRam-0xe000;	 // fg
	drawWindow(0,224,0,0,65);

	CZetOpen(0);

	CZetMemCallback(0xd000, 0xd1ff, 0);
	CZetMemCallback(0xd000, 0xd1ff, 1);
	CZetMemCallback(0xd000, 0xd1ff, 2);
	CZetMemCallback(0xd200, 0xd7ff, 0);
	CZetMemCallback(0xd200, 0xd7ff, 1);
	CZetMemCallback(0xd200, 0xd7ff, 2);
	CZetMemCallback(0xe000, 0xefff, 0);
	CZetMemCallback(0xe000, 0xefff, 1);
	CZetMemCallback(0xe000, 0xefff, 2);
	CZetMemCallback(0xf000, 0xf3ff, 0);
	CZetMemCallback(0xf000, 0xf3ff, 1);
	CZetMemCallback(0xf000, 0xf3ff, 2);
	CZetMemCallback(0xf800, 0xfbff, 0);
	CZetMemCallback(0xf800, 0xfbff, 1);
	CZetMemCallback(0xf800, 0xfbff, 2);

	CZetMapArea(0x0000, 0x7fff, 0, System1Rom1);
	CZetMapArea(0x8000, 0xbfff, 0, System1Rom1 + 0x8000);

	CZetMapArea2(0x0000, 0x7fff, 2, System1Rom1 + 0x20000, System1Rom1);
	CZetMapArea2(0x8000, 0xbfff, 2, System1Rom1 + 0x30000, System1Rom1 + 0x10000);  // 30 fetch et 10 pour code ?
	
	CZetMapArea(0xc000, 0xcfff, 0, System1Ram1);
	CZetMapArea(0xc000, 0xcfff, 1, System1Ram1);
	CZetMapArea(0xc000, 0xcfff, 2, System1Ram1);

	CZetMapArea(0xd000, 0xd7ff, 0, System1SpriteRam);
	CZetMapArea(0xd000, 0xd7ff, 1, System1SpriteRam);
	CZetMapArea(0xd000, 0xd7ff, 2, System1SpriteRam);

  	CZetMapArea(0xd800, 0xddff, 0, System1PaletteRam);
//	CZetMapArea(0xd800, 0xddff, 1, System1PaletteRam);
//	CZetMapArea(0xd800, 0xddff, 2, System1PaletteRam);
//	CZetMapArea(0xd800, 0xddff, 0, System1PaletteRam);
//	CZetMemCallback(0xd800, 0xddff, 1);
//	CZetMapArea(0xd000, 0xd1ff, 1, System1SpriteRam);

	CZetMapArea(0xe000, 0xefff, 0, System1VideoRam); //read
//	CZetMapArea(0xe000, 0xefff, 1, System1VideoRam);	 //write
	CZetMapArea(0xe000, 0xefff, 2, System1VideoRam); //fetch

	CZetMapArea(0xf400, 0xf7ff, 0, System1f4Ram);
	CZetMapArea(0xf400, 0xf7ff, 1, System1f4Ram);
	CZetMapArea(0xf400, 0xf7ff, 2, System1f4Ram);
	CZetMapArea(0xf800, 0xfbff, 0, System1SprCollisionRam);
	CZetMapArea(0xf800, 0xfbff, 2, System1SprCollisionRam);
	CZetMapArea(0xfc00, 0xffff, 0, System1fcRam);
	CZetMapArea(0xfc00, 0xffff, 1, System1fcRam);
	CZetMapArea(0xfc00, 0xffff, 2, System1fcRam);

	CZetSetReadHandler(System2Z801ProgRead);
	CZetSetWriteHandler(System2Z801ProgWrite);

	CZetSetInHandler   (System2Z801PortRead);
	CZetSetOutHandler(System2Z801PortWrite);
	CZetClose();

	ppi8255_init(1);
	PPI0PortWriteA = System2PPI0WriteA;
	PPI0PortWriteB = System2PPI0WriteB;
	PPI0PortWriteC = System2PPI0WriteC;

//	System1Draw = WbmlRender;
	memset(System1VideoRam,0x00,0x4000);
}


static INT32 System2Init(INT32 nZ80Rom1Num, INT32 nZ80Rom1Size, INT32 nZ80Rom2Num, INT32 nZ80Rom2Size, INT32 nTileRomNum, INT32 nTileRomSize, INT32 nSpriteRomNum, INT32 nSpriteRomSize, bool bReset)
{
	UINT32 TilePlaneOffsets[3]  = { RGN_FRAC((nTileRomSize * nTileRomNum), 0, 3), RGN_FRAC((nTileRomSize * nTileRomNum), 1, 3), RGN_FRAC((nTileRomSize * nTileRomNum), 2, 3) };
	UINT32 TileXOffsets[8]      = { 0, 1, 2, 3, 4, 5, 6, 7 };
	UINT32 TileYOffsets[8]      = { 0, 8, 16, 24, 32, 40, 48, 56 };
	UINT32 nRet = 0, i, RomOffset;
	struct BurnRomInfo ri;

	System1NumTiles = (((nTileRomNum * nTileRomSize) / 3) * 8) / (8 * 8);
	System1SpriteRomSize = nSpriteRomNum * nSpriteRomSize;

	DrvInitSaturn();

	CollisionFunction = updateCollisions;
	//System1BgRamSize = 0x800;
	// Allocate and Blank all required memory
	Mem = NULL;
	MemIndex();

	if ((Mem = (UINT8 *)BurnMalloc(MALLOC_MAX)) == NULL) 
	{	
		return 1;
	}
	memset(Mem, 0, MALLOC_MAX);
	MemIndex();

	UINT8 *	System1TempRom = (UINT8*)0x00200000;
	memset(System1TempRom, 0, 0x40000);
	// Load Z80 #1 Program roms
	RomOffset = 0;
	for (i = 0; i < nZ80Rom1Num; i++) {
		if(BurnLoadRom(System1Rom1 + (i * nZ80Rom1Size), i + RomOffset, 1)) return 1;
		BurnDrvGetRomInfo(&ri, i);
	}

	if (System1BankedRom)
	{
		memcpy(System1TempRom, System1Rom1, 0x40000);
		memset(System1Rom1, 0, 0x40000);

		if (System1BankedRom == 1)
		{ // Encrypted, banked
			memcpy(System1Rom1 + 0x00000, System1TempRom + 0x00000, 0x8000);
			memcpy(System1Rom1 + 0x10000, System1TempRom + 0x08000, 0x8000);
			memcpy(System1Rom1 + 0x18000, System1TempRom + 0x10000, 0x8000);
		}

		if (System1BankedRom == 2)
		{ // Unencrypted, banked
			memcpy(System1Rom1 + 0x20000, System1TempRom + 0x00000, 0x8000);
			memcpy(System1Rom1 + 0x00000, System1TempRom + 0x08000, 0x8000);
			memcpy(System1Rom1 + 0x30000, System1TempRom + 0x10000, 0x8000);//fetch
			memcpy(System1Rom1 + 0x10000, System1TempRom + 0x18000, 0x8000);
			memcpy(System1Rom1 + 0x38000, System1TempRom + 0x20000, 0x8000);//fetch
			memcpy(System1Rom1 + 0x18000, System1TempRom + 0x28000, 0x8000);

			if ((UINT32)nZ80Rom1Size == (ri.nLen * 2))
			{ // last rom half the size, reload it into the last slot
				memcpy (System1Rom1 + 0x18000, System1TempRom + 0x20000, 0x8000);
			}
		}
	}

	memset(System1Rom2, 0, 0x10000);

	if (DecodeFunction) DecodeFunction();

	// Load Z80 #2 Program roms
	RomOffset += nZ80Rom1Num;
	for (i = 0; i < nZ80Rom2Num; i++) {
		if(BurnLoadRom(System1Rom2 + (i * nZ80Rom2Size), i + RomOffset, 1)) return 1;
	}

	// Load and decode tiles
	memset(System1TempRom, 0, 0x20000);
	RomOffset += nZ80Rom2Num;
	for (i = 0; i < nTileRomNum; i++) {
		if(BurnLoadRom(System1TempRom + (i * nTileRomSize), i + RomOffset, 1)) return 1;
	}

	GfxDecode4Bpp(System1NumTiles, 3, 8, 8, TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x40, System1TempRom, cache);

	System1TempRom = NULL;

	memset(&ss_map2[2048],0,768);

	if(flipscreen==1)			rotate_tile(System1NumTiles,0,cache);
	else if(flipscreen==2)	rotate_tile(System1NumTiles,1,cache);

	spriteCache = (UINT16*)(0x00200000);

	memset((unsigned char *)spriteCache,0xFF,0x80000);

	// Load Sprite roms
	RomOffset += nTileRomNum;
	for (i = 0; i < nSpriteRomNum; i++) 
	{
		nRet = BurnLoadRom(System1Sprites + (i * nSpriteRomSize), i + RomOffset, 1);
	}

	// Load Colour proms
	if (System1ColourProms) {
		RomOffset += nSpriteRomNum;
		nRet = BurnLoadRom(System1PromRed, 0 + RomOffset, 1);
		nRet = BurnLoadRom(System1PromGreen, 1 + RomOffset, 1);
		nRet = BurnLoadRom(System1PromBlue, 2 + RomOffset, 1);
	}

	// Setup the Z80 emulation
	CZetInit2(2,CZ80Context);

	CZetOpen(0);
	CZetSetReadHandler(System2Z801ProgRead);
	CZetSetWriteHandler(System2Z801ProgWrite);
	CZetSetInHandler(System2Z801PortRead);
	CZetSetOutHandler(System2Z801PortWrite);

	CZetMapArea(0x0000, 0x7fff, 0, System1Rom1);
	CZetMapArea(0x8000, 0xbfff, 0, System1Rom1 + 0x8000);
	if (DecodeFunction) 
	{
		CZetMapArea2(0x0000, 0x7fff, 2, System1Rom1 + 0x20000, System1Rom1);
		CZetMapArea2(0x8000, 0xbfff, 2, System1Rom1 + 0x30000, System1Rom1 + 0x10000);  // 30 fetch, 10 for code(?)
	}
	else
	{
		CZetMapArea(0x0000, 0x7fff, 2, System1Rom1);
		CZetMapArea(0x8000, 0xbfff, 2, System1Rom1 + 0x8000);
	}
	CZetMapArea(0xc000, 0xcfff, 0, System1Ram1);
	CZetMapArea(0xc000, 0xcfff, 1, System1Ram1);
	CZetMapArea(0xc000, 0xcfff, 2, System1Ram1);
	CZetMapArea(0xd000, 0xd7ff, 0, System1SpriteRam);
	CZetMapArea(0xd000, 0xd7ff, 1, System1SpriteRam);
	CZetMapArea(0xd000, 0xd7ff, 2, System1SpriteRam);
	CZetMapArea(0xd800, 0xdfff, 0, System1PaletteRam);
//	CZetMapArea(0xd800, 0xdfff, 1, System1PaletteRam);
//	CZetMapArea(0xd800, 0xdfff, 2, System1PaletteRam);
//----------
// vbt ajout pour s2
	CZetMapArea(0xe000, 0xefff, 0, System1VideoRam); //read
//	CZetMapArea(0xe000, 0xefff, 1, System1VideoRam);	 //write
	CZetMapArea(0xe000, 0xefff, 2, System1VideoRam); //fetch
//---------
	CZetMapArea(0xf000, 0xf3ff, 0, System1BgCollisionRam);
	CZetMapArea(0xf000, 0xf3ff, 2, System1BgCollisionRam);
	CZetMapArea(0xf400, 0xf7ff, 0, System1f4Ram);
	CZetMapArea(0xf400, 0xf7ff, 1, System1f4Ram);
	CZetMapArea(0xf400, 0xf7ff, 2, System1f4Ram);
	CZetMapArea(0xf800, 0xfbff, 0, System1SprCollisionRam);
	CZetMapArea(0xf800, 0xfbff, 2, System1SprCollisionRam);
	CZetMapArea(0xfc00, 0xffff, 0, System1fcRam);
	CZetMapArea(0xfc00, 0xffff, 1, System1fcRam);
	CZetMapArea(0xfc00, 0xffff, 2, System1fcRam);
	CZetClose();

	z80_init_memmap();
	z80_add_read(0xe000, 0xe001, 1, (void *)&System1Z802ProgRead); 
	z80_add_write(0xa000, 0xa003, 1, (void *)&System1Z802ProgWrite);
	z80_add_write(0xc000, 0xc003, 1, (void *)&System1Z802ProgWrite);
	z80_map_read  (0x0000, 0x7fff, System1Rom2);
	z80_map_fetch (0x0000, 0x7fff, System1Rom2);
	z80_map_read  (0x8000, 0x87ff, System1Ram2);
	z80_map_write (0x8000, 0x87ff, System1Ram2); //ajout
	z80_map_fetch (0x8000, 0x87ff, System1Ram2);
// derek fix for wbml sound :)
	z80_map_read  (0x8800, 0x8fff, System1Ram2);
	z80_map_write (0x8800, 0x8fff, System1Ram2); //ajout
	z80_map_fetch (0x8800, 0x8fff, System1Ram2);

	z80_end_memmap();
	z80_reset();
	
	memset(SpriteOnScreenMap, 255, 256 * 256);
	
//	System1SpriteXOffset = 1;
	
	nCyclesTotal[0] = 3000000 / hz;
	nCyclesTotal[1] = 3000000 / hz;

	SN76489AInit(0, 2000000, 0);
	SN76489AInit(1, 4000000, 1);
	
	make_lut();
//	make_cram_lut();
	MakeInputsFunction = System1MakeInputs;

	System1BgRam = NULL;
//	System1SpriteXOffset = 15;

	CZetOpen(0);
	CZetMemCallback(0xf000, 0xf3ff, 1); // collision ram written through write handler
	CZetMemCallback(0xf800, 0xfbff, 1); // ""
	CZetClose();

	ppi8255_init(1);
	PPI0PortWriteA = System2PPI0WriteA;
	PPI0PortWriteB = System2PPI0WriteB;
	PPI0PortWriteC = System2PPI0WriteC;

//	System1Draw = System2Render;
	memset(System1VideoRam, 0x00, 0x4000); // mapped through bank_latch

	if (bReset) 
	{
		ppi8255_init(1);
		System1DoReset();
	}
 // VBT saturn S2
	System1SpriteRam = &System1Ram1[0x1000];
	System1PaletteRam = &System1Ram1[0x1800];	 // ? garder

	make_cram_lut();
	System1CalcPalette();

	ss_reg->n1_move_x = 4<<16;
	CurrentBank						= System1VideoRam;
	RamStart1						= System1VideoRam-0xe000;	 // fg
	drawWindow(0,224,0,0,65);	

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
int WbmljbInit()
{
	int nRet;

	System1ColourProms = 1;
	System1BankedRom = 2;

	DecodeFunction = wbmljb_decode;
 	nRet = System1Init(3, 0x10000, 1, 0x8000, 3, 0x8000, 4, 0x8000, 1);
//	nRet = System2Init(3, 0x8000, 1, 0x8000, 3, 0x8000, 4, 0x8000, 1);

	CommonWbmlInit();
	System1ScrollXRam	= NULL;
 	RamStart			= NULL; //System1VideoRam-0xe000; // bg

	
//	System1DoReset();
	return nRet;
}
//-------------------------------------------------------------------------------------------------------------------------------------
int WbmlInit()
{
	int nRet;
	System1ColourProms = 1;
	System1BankedRom = 1;

	DecodeFunction = wbml_decode;
	UINT8 *System1MC8123Key = (UINT8*)0x002FC000;
	BurnLoadRom(System1MC8123Key, 15, 1);
	nRet = System1Init(3, 0x8000, 1, 0x8000, 3, 0x8000, 4, 0x8000, 1);
	CommonWbmlInit();
	return nRet;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void wbml_draw_bg(UINT8* vram)
{
	ss_reg->n2_move_x = (255-((vram[0x80] >> 1) + ((vram[0x81] & 1) << 7))) &0xff;
	ss_reg->n2_move_y = vram[0x7a]; // & 0x1f;
	unsigned char real_page;
	
	const unsigned int v[] = {0, 0x40,0x1000,0x1040};

	for (unsigned int page=0; page < 4; page++)
	{
		if(!(real_page= (vram[page*2] & 0x07)))
			continue;
		
		if(map_dirty[real_page] == 1)
		{
			map_dirty[real_page] = 0;
			unsigned short *map = &ss_map[v[page]];
			unsigned short *mapc = &map_cache[real_page*0x1000];			
			
			for (unsigned int i=0;i<8 ;i++ )
			{
				memcpyl(map,mapc,128);
				mapc+=128;
				map+=128;
				
				memcpyl(map,mapc,128);
				mapc+=128;
				map+=128;

				memcpyl(map,mapc,128);
				mapc+=128;
				map+=128;

				memcpyl(map,mapc,128);
				mapc+=128;
				map+=128;				
			}			
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void System1Render()
{
	System1DrawSprites();
	wbml_draw_bg(&System1VideoRam[0x740]);
}
//-------------------------------------------------------------------------------------------------------------------------------------
#if 1
void DrawSprite(unsigned int Num,unsigned int Bank, unsigned int addr, UINT16 Skip, UINT8 *SpriteBase)
{
	unsigned int Src = (SpriteBase[7] << 8) | SpriteBase[6];
	unsigned int Height = SpriteBase[1] - SpriteBase[0];
	unsigned int Width = width_lut[ABS(Skip)];

	unsigned int values[] ={Src,Height,Skip,Width, Bank,nextSprite};
	spriteCache[addr]=nextSprite;
	renderSpriteCache(values);

	unsigned int delta	= (Num+3);

	ss_sprite[delta].ax			= 11 + ((((SpriteBase[3] & 0x01) << 8) + SpriteBase[2] )/2);
	ss_sprite[delta].ay			= SpriteBase[0] + 1;
	ss_sprite[delta].charSize	= (Width<<6) + Height;
	ss_sprite[delta].color		= COLADDR_SPR | ((Num)<<2);
	ss_sprite[delta].charAddr	= 0x220+nextSprite;

	nextSprite += (Height*Width)/8;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrawSpriteCache(int Num,int Bank, int addr,INT16 Skip,UINT8 *SpriteBase)
{
	unsigned int Height = SpriteBase[1] - SpriteBase[0];
	unsigned int Width  = width_lut[ABS(Skip)];
	unsigned int delta	= (Num+3);

	ss_sprite[delta].ax			= 11+ ((((SpriteBase[3] & 0x01) << 8) + SpriteBase[2] )/2);
	ss_sprite[delta].ay			= SpriteBase[0] + 1;
	ss_sprite[delta].charSize	= (Width<<6) + Height;
	ss_sprite[delta].color		= COLADDR_SPR | ((Num)<<2);
	ss_sprite[delta].charAddr	= 0x220+spriteCache[addr];
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
void initColors2()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
 	colAddr             = (Uint16*)COLADDR;//(Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	colBgAddr         = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_NBG0,OFF);
	colBgAddr2       = (Uint16*)SCL_AllocColRam(SCL_NBG0,ON);
	SCL_SetColRam(SCL_NBG1,8,4,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initLayers2()
{
    Uint16	CycleTb[]={
		0x2f64, 0xeeee, //A0
		0xffff, 0xffff,	//A1
		0xf6f0,0x55ee,   //B0
		0xffff, 0xffff  //B1
	};
 	SclConfig	scfg;

	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD;
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.plate_addr[0] = (Uint32)ss_map2;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.plate_addr[0] = (Uint32)ss_map;
	SCL_SetConfig(SCL_NBG0, &scfg);

	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.datatype 		 = SCL_BITMAP;
	scfg.mapover		 = SCL_OVER_0;
	scfg.plate_addr[0]	 = (Uint32)ss_font;
// 3 nbg	
	SCL_SetConfig(SCL_NBG1, &scfg);

	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
// <gamezfan> bootleg set 1 is the best outside of the virtual console version
