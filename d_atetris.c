// FB Alpha driver module Atari Tetris driver module
// Based on MAME driver by Zsolt Vasvari

// To do:
//	Hook up pokey (once ported) and verify bootleg set 2 sound

#include "d_atetris.h"

int ovlInit(char *szShortName)
{
//	cleanBSS();
	struct BurnDriver nBurnDrvAtetris = {
		"atetris", "tetris", 
		"Tetris (set 1)\0", 
		atetrisRomInfo, atetrisRomName, AtetrisInputInfo, AtetrisDIPInfo,
		DrvInit, DrvExit, DrvFrame
	};

	struct BurnDriver *fba_drv = 	(struct BurnDriver *)FBA_DRV;
	if (strcmp(nBurnDrvAtetris.szShortName, szShortName) == 0)		memcpy(fba_drv,&nBurnDrvAtetris,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
	
	return 0;
}

/*static*/ inline UINT8 atetris_slapstic_read(UINT16 offset)
{
	UINT8 ret = Drv6502ROM[((SlapsticBank() & 1) * 0x4000) + (offset & 0x3fff)];

	if (offset & 0x2000) SlapsticTweak(offset & 0x1fff);

	return ret;
}

/*static*/ inline void DrvPaletteUpdate(UINT16 offset)
{
/*	INT32 r = (DrvPalRAM[offset] >> 5) & 7;
	INT32 g = (DrvPalRAM[offset] >> 2) & 7;
	INT32 b = (DrvPalRAM[offset] >> 0) & 3;

	r = (r << 5) | (r << 2) | (r >> 1);
	g = (g << 5) | (g << 2) | (g >> 1);
	b = (b << 6) | (b << 4) | (b << 2) | (b << 0);
 */

	colAddr[offset] = colBgAddr[offset] = colBgAddr2[offset] = cram_lut[DrvPalRAM[offset]];//RGB(r>>3,g>>3,b>>3);
//	DrvPalette[offset] = BurnHighCol(r, g, b, 0);
}

/*static*/ UINT8 atetris_read(UINT16 address)
{
	if ((address & 0xc000) == 0x4000) {
		return atetris_slapstic_read(address);
	}

// Remove if/when Pokey support is added!
#if 0
	if (is_Bootleg)
#endif
	{
		switch (address & ~0x03e0)
		{
			case 0x2808:
				return DrvInputs[0] | vblank;

			case 0x2818:
				return DrvInputs[1];
		}
	}
#if 0
	else
	{
		switch (address & ~0x03ef)
		{
			case 0x2800:
				return 0; // pokey1

			case 0x2810:
				return 0; // pokey2
		}
	}
#endif
	return 0;
}

/*static*/ void atetris_write(UINT16 address, UINT8 data)
{
	if ((address & 0xfc00) == 0x2000) {
		DrvPalRAM[address & 0x00ff] = data;
		DrvPaletteUpdate(address & 0x00ff);
		return;
	}

	if ((address & 0xfc00) == 0x2400) {
		if (nvram_enable) {
			DrvNVRAM[address & 0x01ff] = data;
		}
		nvram_enable = 0;
		return;
	}
#if 0
	if (is_Bootleg) // Bootleg set 2 sound system
	{
#endif		
		switch (address)
		{
			case 0x2802:
//				SN76496Write(0, data);
			return;

			case 0x2804:
//				SN76496Write(1, data);
			return;

			case 0x2806:
//				SN76496Write(2, data);
			return;
		}
#if 0		
	}
	else
	{
		switch (address & ~0x03ef)
		{
			case 0x2800: 	// pokey1
			return;

			case 0x2810: 	// pokey2
			return;
		}
	}			
#endif

	switch (address & ~0x03ff)
	{
		case 0x3000:
			watchdog = 0;
		return;

		case 0x3400:
			nvram_enable = 1;
		return;

		case 0x3800:
//			M6502SetIRQLine(0, M6502_IRQSTATUS_NONE);
			m6502SetIRQ(M6502_IRQSTATUS_NONE);
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
  
	M6502Open(0);
	M6502_Reset();
	M6502Close();

	SlapsticReset();

	watchdog = 0;
	nvram_enable = 0;

	return 0;
}
/*
/*static*/ void DrvGfxExpand()
{
	INT32 i;
	for (i = (0x10000 - 1) * 2; i >= 0; i-=2) {
		DrvGfxROM[i + 1] = DrvGfxROM[i/2] & 0x0f;
		DrvGfxROM[i + 0] = DrvGfxROM[i/2] >> 4;
	}
}
*/
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
	if ((AllMem = (UINT8 *)BurnMalloc(MALLOC_MAX)) == NULL) return 1;
	memset(AllMem, 0, MALLOC_MAX);
	MemIndex();

	{
		if (BurnLoadRom(Drv6502ROM, 0, 1)) return 1;

//		if (BurnLoadRom(DrvGfxROM , 1, 1)) return 1;
		if (BurnLoadRom((void *)SS_CACHE , 1, 1)) return 1;

//		memcpy (cache,DrvGfxROM,sizeof(DrvGfxROM));
//		memcpy (&ss_vram[0x1100],DrvGfxROM,sizeof(DrvGfxROM));
//		DrvGfxExpand();
	}
/*
	M6502Init(0, TYPE_M6502);
	M6502Open(0);
//	Crab6502_init(&c6502);
	M6502MapMemory(Drv6502RAM,		0x0000, 0x0fff, M6502_RAM);
	M6502MapMemory(DrvVidRAM,		0x1000, 0x1fff, M6502_RAM);
	M6502MapMemory(DrvPalRAM,		0x2000, 0x20ff, M6502_ROM);
	M6502MapMemory(DrvPalRAM,		0x2100, 0x21ff, M6502_ROM);
	M6502MapMemory(DrvPalRAM,		0x2200, 0x22ff, M6502_ROM);
	M6502MapMemory(DrvPalRAM,		0x2300, 0x23ff, M6502_ROM);
	M6502MapMemory(DrvNVRAM,		0x2400, 0x25ff, M6502_ROM);
	M6502MapMemory(DrvNVRAM,		0x2600, 0x27ff, M6502_ROM);
	M6502MapMemory(Drv6502ROM + 0x8000,	0x8000, 0xffff, M6502_ROM);
	M6502SetReadHandler(atetris_read);
//	Crab6502_set_memread(&c6502,atetris_read);
	M6502SetReadOpHandler(atetris_read);
	M6502SetReadOpArgHandler(atetris_read);
	M6502SetReadMemIndexHandler(atetris_read);
	M6502SetWriteHandler(atetris_write);
//	Crab6502_set_memwrite(&c6502,atetris_write);
	M6502SetWriteMemIndexHandler(atetris_write);
	M6502Close();
*/

	m6502Init(1);
	m6502Open(0);
//	Crab6502_init(&c6502);
	m6502MapMemory(Drv6502RAM,		0x0000, 0x0fff, M6502_RAM);
//	m6502MapMemory(DrvVidRAM,		0x1000, 0x1fff, M6502_RAM);
	m6502MapMemory(DrvVidRAM,		0x1000, 0x1fff, M6502_ROM);
	m6502MapMemory(DrvPalRAM,		0x2000, 0x20ff, M6502_ROM);
	m6502MapMemory(DrvPalRAM,		0x2100, 0x21ff, M6502_ROM);
	m6502MapMemory(DrvPalRAM,		0x2200, 0x22ff, M6502_ROM);
	m6502MapMemory(DrvPalRAM,		0x2300, 0x23ff, M6502_ROM);
	m6502MapMemory(DrvNVRAM,		0x2400, 0x25ff, M6502_ROM);
	m6502MapMemory(DrvNVRAM,		0x2600, 0x27ff, M6502_ROM);
	m6502MapMemory(Drv6502ROM + 0x8000,	0x8000, 0xffff, M6502_ROM);
	m6502SetReadHandler(atetris_read);
//	Crab6502_set_memread(&c6502,atetris_read);
//	m6502SetReadOpHandler(atetris_read);
//	m6502SetReadOpArgHandler(atetris_read);
//	m6502SetReadMemIndexHandler(atetris_read);
	m6502SetWriteHandler(atetris_write);
//	Crab6502_set_memwrite(&c6502,atetris_write);
//	m6502SetWriteMemIndexHandler(atetris_write);
	m6502Close();




	SlapsticInit(101);

	is_Bootleg = boot;
	master_clock = boot ? (14745600 / 8) : (14318180 / 8);

	if (is_Bootleg)	// Bootleg set 2 sound system
	{
		SN76496Init(0, master_clock, 0);
		SN76496Init(1, master_clock, 1);
		SN76496Init(2, master_clock, 1);
		SN76496SetRoute(0, 0.50, BURN_SND_ROUTE_BOTH);
		SN76496SetRoute(1, 0.50, BURN_SND_ROUTE_BOTH);
		SN76496SetRoute(2, 0.50, BURN_SND_ROUTE_BOTH);
	}

//	GenericTilesInit();

	memset (DrvNVRAM, 0xff, 0x200);

	DrvDoReset(1);

unsigned char *	DrvTempRom = (unsigned char *)0x00200000;	
memset(DrvTempRom,0x00,0x10000);
	return 0;
}

/*static*/ INT32 DrvExit()
{
//	GenericTilesExit();

//	M6502Exit();
	m6502Exit();
/*
	if (is_Bootleg)	// Bootleg set 2 sound system
	{
		SN76496Exit();
	}
*/
	MemEnd = AllRam RamEnd = Drv6502ROM = DrvGfxROM = DrvNVRAM = NULL;
	Drv6502RAM = DrvVidRAM = DrvPalRAM = DrvPalette = NULL;

	free (AllMem);
	AllMem = NULL;

	cram_lut = map_offset_lut = NULL;
	free(SaturnMem);
	SaturnMem = NULL;
	//cleanDATA();
	cleanBSS();
	return 0;
}

/*static*/ void DrawLayer()
{
	UINT16 offs;

	for (offs = 0; offs < 0x800; offs++)
	{
		UINT16 sx = (offs & 0x3f);// * 8;
		UINT16 sy = (offs / 0x40);// * 8;

//		if (sx >= 42 || sy >= 30) continue;

		UINT16 code  = DrvVidRAM[offs * 2 + 0] | ((DrvVidRAM[offs * 2 + 1] & 0x07) << 8);
		UINT16 color = DrvVidRAM[offs * 2 + 1] >> 4;

		UINT16 x = map_offset_lut[offs];

//		ss_map[x] = color;
//		ss_map[x+1] = code;

		ss_map2[x] = color;
		ss_map2[x+1] = code;

//		Render8x8Tile(pTransDraw, code, sx, sy, color, 4, 0, DrvGfxROM);
	}
}
/*
inline void updateBgTile2Words(UINT32 offs)
{
	UINT32 attr  = DrvBgRAM[(offs<<1) + 1];
	UINT32 color = (attr >> 3) & 0x0f;
	UINT32 code  = DrvBgRAM[(offs<<1)] + ((attr & 0x07) << 8);
	UINT32 flipx = attr & 0x80;

	UINT16 *map2 = (UINT16 *)&ss_map2[bg_map_lut[offs]];
	map2[0] = (flipx << 7) | color;
	map2[1] = (code*4)+0x1000;
}
*/

/*static*/ INT32 DrvDraw()
{
	INT32 i;

	if (DrvRecalc) {
		for (i = 0; i < 0x100; i++) {
			DrvPaletteUpdate(i);
		}

		DrvRecalc = 0;
	}

	DrawLayer();

	//BurnTransferCopy(DrvPalette);

	return 0;
}

/*static*/ INT32 DrvFrame()
{
	if (DrvReset) {
		DrvDoReset(1);
	}
	INT32 i;
	watchdog++;
	if (watchdog >= 180) {
		DrvDoReset(0);
	}

	{
		DrvInputs[0] = DrvDips[0] & 0xbc;
		DrvInputs[1] = 0;
		for (i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
		}
	}

	INT32 nInterleave = 262;
	INT32 nCyclesTotal[1] = { master_clock / 60 };
	INT32 nCyclesDone[1] = { 0 };

//	M6502Open(0);
	m6502Open(0);

	vblank = 0;

	for (i = 0; i < nInterleave; i++)
	{
//		nCyclesDone[0] += M6502Run(nCyclesTotal[0] / nInterleave);
		nCyclesDone[0] += m6502Run(nCyclesTotal[0] / nInterleave);

		if (i == 16 || i == 48 || i == 80 || i == 112 || i == 146 || i == 176 || i == 208 || i == 240)
//			M6502SetIRQLine(0, (i & 0x20) ? M6502_IRQSTATUS_ACK : M6502_IRQSTATUS_NONE);
			m6502SetIRQ((i & 0x20) ? M6502_IRQSTATUS_ACK : M6502_IRQSTATUS_NONE);

		if (i == 240) vblank = 0x40;
	}

//	M6502Close();
	m6502Close();

//	if (pBurnSoundOut) {
		if (is_Bootleg)	// Bootleg set 2 sound system
		{
//			SN76496Update(0, pBurnSoundOut, nBurnSoundLen);
//			SN76496Update(1, pBurnSoundOut, nBurnSoundLen);
//			SN76496Update(2, pBurnSoundOut, nBurnSoundLen);
		}
//	}

	DrvDraw();
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initColors()
{
 	colAddr             = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	colBgAddr         = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
	colBgAddr2       = (Uint16*)SCL_AllocColRam(SCL_NBG2,ON);
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

//	scfg.dispenbl      = ON;
//	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
//	scfg.pnamesize     = SCL_PN2WORD;
//	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
//	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
//	scfg.datatype      = SCL_CELL;
	scfg.plate_addr[0] = (Uint32)ss_map;
//	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG2, &scfg);

//	scfg.dispenbl 		 = ON;
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

	for (i = 0; i < 0x800; i++)
	{
		sx = (i & 0x3f);// * 8;
		sy = (i / 0x40);// * 8;

		map_offset_lut[i] = (sx|(sy<<6))<<1;
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
	map_offset_lut	= Next; Next += 0x800 * sizeof(UINT16);
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
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
	nBurnSprites  = 35;

	SS_MAP     = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B1;//+0x1E000;
	SS_MAP2   = ss_map2 =(Uint16 *)SCL_VDP2_VRAM_A1;//+0x1C000;
	SS_FONT   = (Uint16 *)SCL_VDP2_VRAM_B0;
	SS_CACHE = (Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;
	ss_OtherPri       = (SclOtherPriRegister *)SS_OTHR;

//	ss_sprite  = (SprSpCmd *)SS_SPRIT;
//	ss_vram   = (UINT8 *)SS_SPRAM;
//	ss_scl      = (Fixed32 *)SS_SCL;

	SaturnInitMem();
	SaturnMem = (UINT8 *)BurnMalloc(MALLOC_MAX); 
	SaturnInitMem();
	make_lut();
	
	SS_SET_S0PRIN(4);
	SS_SET_N1PRIN(5);
	SS_SET_N0PRIN(7);

	initLayers();
	initColors();
//	make_cram_lut();
//	make_width_lut();
//	initScrolling(OFF);
// 	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"DrvInitSaturn   done          ",10,70);
	
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"DrvInitSaturn dummy            ",10,70);
	drawWindow(0,224,0,0,66);
}
//-------------------------------------------------------------------------------------------------------------------------------------

/*static*/ INT32 DrvInit()
{
	DrvInitSaturn();
	return CommonInit(0);
}

/*static*/ INT32 BootInit()
{
	return CommonInit(1);
}

