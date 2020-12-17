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

static UINT8 atetris_slapstic_read(UINT16 offset)
{
	UINT8 ret = Drv6502ROM[((SlapsticBank() & 1) * 0x4000) + (offset & 0x3fff)];

	if (offset & 0x2000) 
		SlapsticTweak(offset & 0x1fff);

	return ret;
}

static inline void DrvPaletteUpdate(UINT16 offset)
{
	colBgAddr[offset] = cram_lut[DrvPalRAM[offset]];//RGB(r>>3,g>>3,b>>3);
}

UINT8 atetris_read_0x2800(UINT16 address)
{
// Remove if/when Pokey support is added!
	{
		switch (address & ~0x03e0)
		{
			case 0x2808:
				return (DrvInputs[0] & ~0x40) | vblank;

			case 0x2818:
				return DrvInputs[1];
		}
	}
	return 0;
}

void atetris_write_0x1000(UINT16 address, UINT8 data)
{
	address-=0x1000;
	if(DrvVidRAM[address]!=data)
	{
		DrvVidRAM[address] = data;
		
		address&=~1;
		UINT16 code  = DrvVidRAM[address + 0] | ((DrvVidRAM[address + 1] & 0x07) << 8);
		UINT16 color = DrvVidRAM[address + 1] >> 4;

		UINT16 x = map_offset_lut[address];
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
			libM6502SetIRQLine(1, M6502_IRQSTATUS_NONE);
		return;

		case 0x3c00:
			// coin counter - (data & 0x20) -> 0, (data & 0x10) -> 1
		return;
	}
}

INT32 DrvDoReset(INT32 full_reset)
{
	if (full_reset) {
		memset(AllRam, 0, RamEnd - AllRam);
	}
//FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"M6502OpenR             ",10,70);	
//	M6502Open(0);
FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"M6502Reset             ",10,70);	
	libM6502_Reset();
//FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"M6502Close             ",10,70);	
//	M6502Close();
//FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"SlapsticReset             ",10,70);
	SlapsticReset();

	watchdog = 0;
	nvram_enable = 0;

	return 0;
}

INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;

	Drv6502ROM		= Next; Next += 0x010000;
	DrvNVRAM		= Next; Next += 0x000200;

	AllRam			= Next;

	DrvVidRAM		= Next; Next += 0x001000;
	Drv6502RAM		= Next; Next += 0x001000;
	DrvPalRAM		= Next; Next += 0x000100;

	RamEnd			= Next;
	
	cram_lut		= Next; Next += 256 * sizeof(UINT16);
	map_offset_lut	= Next; Next += 0x800 * 2 * sizeof(UINT16);
	
	return 0;
}

INT32 CommonInit(INT32 boot)
{
	AllMem = NULL;
	MemIndex();
//	if ((AllMem = (UINT8 *)BurnMalloc(MALLOC_MAX)) == NULL) return 1;
//	memset(AllMem, 0, MALLOC_MAX);
	if ((AllMem = (UINT8 *)BurnMalloc(0x14500)) == NULL) return 1;
	memset(AllMem, 0, 0x14500);
	MemIndex();
	make_lut();
	
	{
		if (BurnLoadRom(Drv6502ROM, 0, 1)) return 1;
//		if (BurnLoadRom(DrvGfxROM , 1, 1)) return 1;
		if (BurnLoadRom(cache , 1, 1)) return 1;
	}
FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"libM6502Init             ",10,70);	
	libM6502Init(0, TYPE_M6502);
//	M6502Open(0);
FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"libM6502MapMemory             ",10,70);
	libM6502MapMemory(Drv6502RAM,		0x0000, 0x0fff, M6502_RAM);
//	libM6502MapMemory(DrvVidRAM,		0x1000, 0x1fff, M6502_RAM);
	libM6502MapMemory(DrvPalRAM,		0x2000, 0x20ff, M6502_ROM);
	libM6502MapMemory(DrvPalRAM,		0x2100, 0x21ff, M6502_ROM);
	libM6502MapMemory(DrvPalRAM,		0x2200, 0x22ff, M6502_ROM);
	libM6502MapMemory(DrvPalRAM,		0x2300, 0x23ff, M6502_ROM);
	libM6502MapMemory(DrvNVRAM,		0x2400, 0x25ff, M6502_ROM);
	libM6502MapMemory(DrvNVRAM,		0x2600, 0x27ff, M6502_ROM);
	libM6502MapMemory(Drv6502ROM + 0x8000,	0x8000, 0xffff, M6502_ROM);
FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"libM6502SetReadHandler             ",10,70);
	libM6502SetReadHandler(0x2800, 0x281f,atetris_read_0x2800);
//	libM6502SetReadHandler(0x4000, 0x7fff,atetris_read_0x4000);	
	libM6502SetReadHandler(0x4000, 0x7fff,atetris_slapstic_read);	

	libM6502SetWriteHandler(0x3000, 0x3cff,atetris_write_0x3000);
	libM6502SetWriteHandler(0x2800, 0x281f,atetris_write_0x2800);
	libM6502SetWriteHandler(0x2400, 0x25ff,atetris_write_0x2400);
	libM6502SetWriteHandler(0x2000, 0x20ff,atetris_write_0x2000);
	libM6502SetWriteHandler(0x1000, 0x1fff,atetris_write_0x1000);
//	M6502Close();

	SlapsticInit(101);

	is_Bootleg = boot;
	master_clock = boot ? (14745600 / 8) : (14318180 / 8);
/*
	if (is_Bootleg)	// Bootleg set 2 sound system
	{
		SN76496Init(0, master_clock, 0);
		SN76496Init(1, master_clock, 1);
		SN76496Init(2, master_clock, 1);
//		SN76496SetRoute(0, 0.50, BURN_SND_ROUTE_BOTH);
//		SN76496SetRoute(1, 0.50, BURN_SND_ROUTE_BOTH);
//		SN76496SetRoute(2, 0.50, BURN_SND_ROUTE_BOTH);
	}
*/
	memset (DrvNVRAM, 0xff, 0x200);

	DrvDoReset(1);

	for (INT32 i = 0; i < 0x100; i++) 
	{
		DrvPaletteUpdate(i);
	}
	return 0;
}

INT32 DrvExit()
{
//	M6502Exit();
/*
	if (is_Bootleg)	// Bootleg set 2 sound system
	{
		SN76496Exit();
	}
*/
	Drv6502ROM = DrvNVRAM = AllRam = DrvVidRAM = NULL;
	Drv6502RAM = DrvPalRAM = RamEnd  = NULL;
	cram_lut = map_offset_lut= NULL;
	free (AllMem);
	AllMem = NULL;

	return 0;
}

void DrvFrame()
{
FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)"DrvFrame                 ",10,70);	
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

	UINT8 nInterleave = 65;
	UINT32 nCyclesTotal = { master_clock / 60 / nInterleave};
//	M6502Open(0);

	vblank = 0x40;

	for (UINT32 i = 0; i < nInterleave; i++)
	{
		libM6502_Execute(nCyclesTotal);

		if (i == 12 || i == 28 || i == 44 || i == 60)	
		{
//					libM6502SetIRQLine(0, (i & 0x10) ? M6502_IRQSTATUS_ACK : M6502_IRQSTATUS_NONE);  modif derek
			libM6502SetIRQLine(1, M6502_IRQSTATUS_ACK);
			if (i == 60) vblank = 0;					
		}
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

//	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initColors()
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
void make_lut(void)
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
void DrvInitSaturn()
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

	SS_SET_S0PRIN(4);
	SS_SET_N1PRIN(5);
	SS_SET_N0PRIN(7);

	initLayers();
	initColors();
	
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
	drawWindow(0,224,0,0,44);
}
//-------------------------------------------------------------------------------------------------------------------------------------

INT32 DrvInit()
{
	DrvInitSaturn();
	return CommonInit(0);
}

