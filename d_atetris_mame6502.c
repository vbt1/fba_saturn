// FB Alpha driver module Atari Tetris driver module
// Based on MAME driver by Zsolt Vasvari

// To do:
//	Hook up pokey (once ported) and verify bootleg set 2 sound

#include "d_atetris_mame6502.h"

int ovlInit(char *szShortName)
{

	struct BurnDriver nBurnDrvAtetris = {
		"atetris", "tetris",
		"Tetris (set 1)",
		atetrisRomInfo, atetrisRomName, AtetrisInputInfo, AtetrisDIPInfo,
		DrvInit, DrvExit, DrvFrame, NULL
	};

	struct BurnDriver *fba_drv = 	(struct BurnDriver *)FBA_DRV;
	if (strcmp(nBurnDrvAtetris.szShortName, szShortName) == 0)		memcpy(fba_drv,&nBurnDrvAtetris,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}

static inline UINT8 atetris_slapstic_read(UINT16 offset)
{
	UINT8 ret = Drv6502ROM[((SlapsticBank() & 1) * 0x4000) + (offset & 0x3fff)];

	if (offset & 0x2000) SlapsticTweak(offset & 0x1fff);

	return ret;
}

static inline void DrvPaletteUpdate(UINT16 offset)
{
	colBgAddr[offset] = cram_lut[DrvPalRAM[offset]];//RGB(r>>3,g>>3,b>>3);
}

/*static*/ UINT8 atetris_read(UINT16 address)
{
	if ((address & 0xc000) == 0x4000) 
	{
		return atetris_slapstic_read(address);
	}

// Remove if/when Pokey support is added!
	{
		switch (address & ~0x03e0)
		{
			case 0x2808:
				return DrvInputs[0] | vblank;

			case 0x2818:
				return DrvInputs[1];
		}
	}
}

/*static*/ void atetris_write_0x1000(UINT16 address, UINT8 data)
{
	address-=0x1000;
	if(DrvVidRAM[address]!=data)
	{
		DrvVidRAM[address] = data;
		
		address&=~1;
		UINT32 code  = DrvVidRAM[address + 0] | ((DrvVidRAM[address + 1] & 0x07) << 8);
		UINT32 color = DrvVidRAM[address + 1] >> 4;

		int x = map_offset_lut[address];
		ss_map2[x] = color;
		ss_map2[x+1] = code;
	}
}

void atetris_write_0x2000(UINT16 address, UINT8 data)
{
	DrvPalRAM[address & 0x00ff] = data;
	DrvPaletteUpdate(address & 0x00ff);
}

void atetris_write_0x2400(UINT16 address, UINT8 data)
{
	if (nvram_enable) {
		DrvNVRAM[address & 0x01ff] = data;
	}
	nvram_enable = 0;
}

void atetris_write_0x2800(UINT16 address, UINT8 data)
{
	return; // pokey1 & pokey2
}

void atetris_write_0x3000(UINT16 address, UINT8 data)
{
	switch (address & ~0x03ff)
	{
		case 0x3000:
			watchdog = 0;
		return;

		case 0x3400:
			nvram_enable = 1;
		return;

		case 0x3800:
//			M6502SetIRQLine(0, M6502_IRQSTATUS_NONE); modif derek
			M6502SetIRQLine(1, M6502_IRQSTATUS_NONE);
		return;

		case 0x3c00:
			// coin counter - (data & 0x20) -> 0, (data & 0x10) -> 1
		return;
	}
}

/*static*/ INT32 DrvDoReset(INT32 full_reset)
{
	if (full_reset) {
		memset(AllRam, 0, RamEnd - AllRam);
	}
FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"M6502OpenR             ",10,70);	
	M6502Open(0);
FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"M6502Reset             ",10,70);	
	M6502Reset();
FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"M6502Close             ",10,70);	
//	M6502Close();
FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"SlapsticReset             ",10,70);
	SlapsticReset();

	watchdog = 0;
	nvram_enable = 0;

	return 0;
}

/*static*/ INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;

	Drv6502ROM		= Next; Next += 0x010000;

//	DrvGfxROM		= Next; Next += 0x020000;

	DrvPalette		= (UINT32*)Next; Next += 0x0100 * sizeof(UINT32);

	DrvNVRAM		= Next; Next += 0x000200;

	AllRam			= Next;

	DrvVidRAM		= Next; Next += 0x001000;
	Drv6502RAM		= Next; Next += 0x001000;
	DrvPalRAM		= Next; Next += 0x000100;

	RamEnd			= Next;

	MemEnd			= Next;

	return 0;
}

//Crab6502_t c6502;

/*static*/ INT32 CommonInit(INT32 boot)
{
	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)BurnMalloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();

	{
		if (BurnLoadRom(Drv6502ROM, 0, 1)) return 1;

//		if (BurnLoadRom(DrvGfxROM , 1, 1)) return 1;
		if (BurnLoadRom(cache , 1, 1)) return 1;

//		memcpy (cache,DrvGfxROM,sizeof(DrvGfxROM));
//		memcpy (&ss_vram[0x1100],DrvGfxROM,sizeof(DrvGfxROM));
//		DrvGfxExpand();
	}

FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"M6502Init             ",10,70);

	M6502Init(0, TYPE_M6502);
FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"M6502Open             ",10,70);	
	M6502Open(0);
//	Crab6502_init(&c6502);
FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"M6502MapMemory             ",10,70);
	M6502MapMemory(Drv6502RAM,		0x0000, 0x0fff, M6502_RAM);
//	M6502MapMemory(DrvVidRAM,		0x1000, 0x1fff, M6502_RAM);
	M6502MapMemory(DrvPalRAM,		0x2000, 0x20ff, M6502_ROM);
	M6502MapMemory(DrvPalRAM,		0x2100, 0x21ff, M6502_ROM);
	M6502MapMemory(DrvPalRAM,		0x2200, 0x22ff, M6502_ROM);
	M6502MapMemory(DrvPalRAM,		0x2300, 0x23ff, M6502_ROM);
	M6502MapMemory(DrvNVRAM,		0x2400, 0x25ff, M6502_ROM);
	M6502MapMemory(DrvNVRAM,		0x2600, 0x27ff, M6502_ROM);
	M6502MapMemory(Drv6502ROM + 0x8000,	0x8000, 0xffff, M6502_ROM);
FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"M6502SetReadHandler             ",10,70);	
	M6502SetReadHandler(atetris_read);
	
//	M6502SetReadOpHandler(atetris_read);
//	M6502SetReadOpArgHandler(atetris_read);
//	M6502SetWriteHandler(atetris_write);

	M6502SetWriteHandler(0x3000, 0x3cff,atetris_write_0x3000);
	M6502SetWriteHandler(0x2800, 0x281f,atetris_write_0x2800);
	M6502SetWriteHandler(0x2400, 0x25ff,atetris_write_0x2400);
	M6502SetWriteHandler(0x2000, 0x20ff,atetris_write_0x2000);
	M6502SetWriteHandler(0x1000, 0x1fff,atetris_write_0x1000);
FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"M6502Close             ",10,70);		
	M6502Close();

	SlapsticInit(101);

	is_Bootleg = boot;
	master_clock = boot ? (14745600 / 8) : (14318180 / 8);
//master_clock = master_clock /3;

	if (is_Bootleg)	// Bootleg set 2 sound system
	{
		SN76496Init(0, master_clock, 0);
		SN76496Init(1, master_clock, 1);
		SN76496Init(2, master_clock, 1);
//		SN76496SetRoute(0, 0.50, BURN_SND_ROUTE_BOTH);
//		SN76496SetRoute(1, 0.50, BURN_SND_ROUTE_BOTH);
//		SN76496SetRoute(2, 0.50, BURN_SND_ROUTE_BOTH);
	}

//	GenericTilesInit();

	memset (DrvNVRAM, 0xff, 0x200);
FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"DrvDoReset             ",10,70);
	DrvDoReset(1);

	if (DrvRecalc) {
		for (INT32 i = 0; i < 0x100; i++) {
			DrvPaletteUpdate(i);
		}

		DrvRecalc = 0;
	}
	return 0;
}

/*static*/ INT32 DrvExit()
{
//	GenericTilesExit();

	M6502Exit();
/*
	if (is_Bootleg)	// Bootleg set 2 sound system
	{
		SN76496Exit();
	}
*/
	Drv6502ROM = DrvPalette = DrvNVRAM = AllRam = DrvVidRAM = NULL;
	Drv6502RAM = DrvPalRAM = RamEnd  = MemEnd = NULL;
	free (AllMem);
	AllMem = NULL;

	cram_lut = map_offset_lut= NULL;
	free (SaturnMem);
	SaturnMem = NULL;

	return 0;
}

/*static*/ INT32 DrvFrame()
{
	watchdog++;
	if (watchdog >= 180) {
		DrvDoReset(0);
	}

	{
		DrvInputs[0] = DrvDips[0] & 0xbc;
		DrvInputs[1] = 0;
		for (INT32 i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
		}
	}

	INT32 nInterleave = 65;
	INT32 nCyclesTotal = { master_clock / 60 / nInterleave};
//	INT32 nCyclesDone = { 0 };
//	M6502Open(0);

	vblank = 0;

	for (INT32 i = 0; i < nInterleave; i++)
	{
		M6502Run(nCyclesTotal);
//		nCyclesDone[0] += M6502Run(nCyclesTotal[0]);

//		if (i == 16 || i == 48 || i == 80 || i == 112 || i == 146 || i == 176 || i == 208 || i == 240)
//		if (i == 4 || i == 12 || i == 20 || i == 28 || i == 36 || i == 44 || i == 52 || i == 60)
		if((i-4)%8==0)
		{
//					M6502SetIRQLine(0, (i & 0x10) ? M6502_IRQSTATUS_ACK : M6502_IRQSTATUS_NONE);  modif derek
					M6502SetIRQLine(1, (i & 0x10) ? M6502_IRQSTATUS_ACK : M6502_IRQSTATUS_NONE);
			if (i == 60) vblank = 0x40;					
		}
//		if (i == 240) vblank = 0x40;

	}
//	M6502Close();

//	if (pBurnSoundOut) {
//		if (is_Bootleg)	// Bootleg set 2 sound system
//		{
//			SN76496Update(0, pBurnSoundOut, nBurnSoundLen);
//			SN76496Update(1, pBurnSoundOut, nBurnSoundLen);
//			SN76496Update(2, pBurnSoundOut, nBurnSoundLen);
//		}
//	}

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initColors()
{
// 	colAddr             = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	colBgAddr         = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
//	colBgAddr2       = (Uint16*)SCL_AllocColRam(SCL_NBG2,ON);
	SCL_SetColRam(SCL_NBG0,8,4,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initLayers()
{
    Uint16	CycleTb[]={
		0x1f56, 0xeeee, //A0
		0xffff, 0xffff,	//A1
		0xf5f2,0x4eee,   //B0
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
	SCL_SetConfig(SCL_NBG1, &scfg);

	scfg.dispenbl      = OFF;
//	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
//	scfg.pnamesize     = SCL_PN2WORD;
//	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
//	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
//	scfg.datatype      = SCL_CELL;
	scfg.plate_addr[0] = (Uint32)ss_map;
//	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.dispenbl 		 = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
//	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 		 = SCL_BITMAP;
	scfg.mapover		 = SCL_OVER_0;
	scfg.plate_addr[0]	 = (Uint32)ss_font;
// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void make_lut(void)
{
	unsigned int i,delta=0;
	int sx, sy;

	for (i = 0; i < 256;i++) 
	{
		INT32 r = (i >> 5) & 7;
		INT32 g = (i >> 2) & 7;
		INT32 b = (i >> 0) & 3;

		r =  (r << 5) | (r << 2) | (r >> 1);
		g = (g << 5) | (g << 2) | (g >> 1);
		b = (b << 6) | (b << 4) | (b << 2) | (b << 0);

		cram_lut[i] = RGB(r>>3,g>>3,b>>3);
	}

	for (i = 0; i < 0x800*2; i+=2)
	{
		sx = ((i>>1) & 0x3f);// * 8;
		sy = ((i>>1) / 0x40);// * 8;

		map_offset_lut[i] = (sx|(sy<<6))<<1;
		map_offset_lut[i+1] = (sx|(sy<<6))<<1;		
	}

//	colAddr[offset] = colBgAddr[offset] = colBgAddr2[offset] = RGB(r>>3,g>>3,b>>3);
/*	
	for (i = 0; i < 512;i++) 
	{
		remap8to16_lut[i] = delta;
		delta++; if ((delta & 7) == 0) delta += 8;
	}

	for (i = 0; i < 0x800;i++) 
	{
		sx = ((i >> 1) & 0x1f); //% 32;
		sy = i & (~0x3f);
		map_offset_lut[i] = ((sx) | sy)<<1;
	}

	for (i = 0; i < System1NumTiles;i++)code_lut[i] = ((i >> 4) & 0x800) | (i & 0x7ff);
	for (i = 0; i < 10; i++)						cpu_lut[i] = (i + 1) * nCyclesTotal[0] / 10;
	for(i=0;i<256;i++)							width_lut[i] = (i + (7)) & ~(7);
//	for(i=0;i<0x2000;i++)		color_lut[i] = (i>>5) & 0x3f;		   */
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void SaturnInitMem()
{
	UINT8 *Next; Next = (UINT8 *)SaturnMem;

//	width_lut			= Next; Next += 256 * sizeof(UINT8);
	cram_lut			= Next; Next += 256 * sizeof(UINT16);
//	remap8to16_lut	= Next; Next += 512 * sizeof(UINT16);
	map_offset_lut	= Next; Next += 0x800 * 2 * sizeof(UINT16);
//	code_lut			= Next; Next += System1NumTiles * sizeof(UINT16);
//	cpu_lut				= Next; Next += 10*sizeof(int);
//	color_lut			= Next; Next += 0x2000 * sizeof(UINT8);
	MemEnd			= Next;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn()
{
//	FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"DrvInitSaturn             ",10,70);
	SPR_InitSlaveSH();
	nBurnSprites  = 35;

	SS_MAP     = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B1;//+0x1E000;
	SS_MAP2   = ss_map2 =(Uint16 *)SCL_VDP2_VRAM_A1;//+0x1C000;
	SS_FONT   = ss_font    =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_CACHE = cache     =(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;
	ss_OtherPri       = (SclOtherPriRegister *)SS_OTHR;

//	ss_sprite  = (SprSpCmd *)SS_SPRIT;
//	ss_vram   = (UINT8 *)SS_SPRAM;
//	ss_scl      = (Fixed32 *)SS_SCL;

	SaturnInitMem();
	int nLen = MemEnd - (UINT8 *)0;
	SaturnMem = (UINT8 *)BurnMalloc(nLen); 
	SaturnInitMem();
	make_lut();
//	spriteCache =(UINT16*)BurnMalloc(0x20000*sizeof(UINT16));
//	memset(spriteCache,0xFF,0x20000*sizeof(UINT16));
//	memset(&ss_vram[0x1100],0x00,0x7EF00);

	SS_SET_S0PRIN(4);
	SS_SET_N1PRIN(5);
	SS_SET_N0PRIN(7);

	initLayers();
	initColors();
//	SCL_Open();
//	Scl_n_reg.n1_move_x = 32<<16;
//	Scl_n_reg.n2_move_x = 16;
//	initScrolling(ON);
//	make_cram_lut();
//	make_width_lut();
//	initScrolling(OFF);
// 	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"DrvInitSaturn   done          ",10,70);
	
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"DrvInitSaturn dummy            ",10,70);
	drawWindow(0,224,0,0,44);
}
//-------------------------------------------------------------------------------------------------------------------------------------

/*static*/ INT32 DrvInit()
{
	DrvInitSaturn();
	return CommonInit(0);
}

