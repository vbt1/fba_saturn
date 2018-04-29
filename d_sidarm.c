// FB Alpha Side Arms driver module
// Based on MAME driver by Paul Leaman

#include "d_sidarm.h"
#define nScreenHeight 224
#define nScreenWidth 384

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvSidearms = {
		"sidarm", NULL,
		"Side Arms - Hyper Dyne (World, 861129)",
		sidearmsRomInfo, sidearmsRomName, SidearmsInputInfo, SidearmsDIPInfo,
		SidearmsInit, DrvExit, DrvFrame, NULL
	};

	if (strcmp(nBurnDrvSidearms.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvSidearms,sizeof(struct BurnDriver));

	ss_reg   = (SclNorscl *)SS_REG;
	ss_regs = (SclSysreg *)SS_REGS;
	ss_regd = (SclDataset *)SS_REGD;
}

/*static*/ void palette_write(INT32 offset)
{
	offset &= 0x3ff;

	UINT16 data = (DrvPalRAM[offset + 0x400] * 256) + DrvPalRAM[offset];

	UINT8 r = (data >> 4) & 0x0f;
	UINT8 g = (data >> 0) & 0x0f;
	UINT8 b = (data >> 8) & 0x0f;

	r |= r << 4;
	g |= g << 4;
	b |= b << 4;

	DrvPalette[offset] = BurnHighCol(r,g,b,0);

	if(offset >=0x300)
	{
// fg	 offset 300
		unsigned short position = remap4to16_lut[offset&0xff];
		colBgAddr2[position] = BurnHighCol(r,g,b,0);//DrvPalette[offset];
	}

}

/*static*/ void bankswitch(INT32 data)
{
	bank_data = data & 0x0f;

//	ZetMapMemory(DrvZ80ROM0 + 0x8000 + (bank_data * 0x4000), 0x8000, 0xbfff, MAP_ROM);
//	CZetMapArea(0xc000, 0xcfff, 0, DrvBgRAM + nBank);

	CZetMapArea (0x8000, 0xbfff, 0, DrvZ80ROM0 + 0x8000 + (bank_data * 0x4000));
	CZetMapArea (0x8000, 0xbfff, 2, DrvZ80ROM0 + 0x8000 + (bank_data * 0x4000));
}

/*static*/ void __fastcall sidearms_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xc000) {
		DrvPalRAM[address & 0x7ff] = data;
		palette_write(address);
		return;
	}

	switch (address)
	{
		case 0xc800:
			soundlatch = data;
		return;

		case 0xc801:
			bankswitch(data);
		return;

		case 0xc802:
			enable_watchdog = 1;
			watchdog = 0;
		return;

		case 0xc804:
		{
			// coin counters data & 0x01, data & 0x02
			// coin lockout data & 0x04, data & 0x08
#if 0
			if (data & 0x10) {
				CZetClose();
				CZetOpen(1);
				CZetReset();
				CZetClose();
				CZetOpen(0);
			}
#endif
			if (starfield_enable != (data & 0x20)) {
				starfield_enable = data & 0x20;
				hflop_74a = 1;
				starscrollx = starscrolly = 0;
			}

			character_enable = data & 0x40;
			flipscreen = data & 0x80;
		}
		return;

		case 0xc805:
		{
		//	INT32 last = starscrollx;

			starscrollx++;
			starscrollx &= 0x1ff;

			// According to MAME this is correct, but for some reason in
			// FBA it is seizure-inducing. Just disable it for now.
		//	if (starscrollx && (~last & 0x100)) hflop_74a ^= 1;
		}
		return;

		case 0xc806:
			starscrolly++;
			starscrolly &= 0xff;
		return;

		case 0xc808:
		case 0xc809:
			bgscrollx[address & 1] = data;
		return;

		case 0xc80a:
		case 0xc80b:
			bgscrolly[address & 1] = data;
		return;

		case 0xc80c:
			sprite_enable = data & 0x01;
			bglayer_enable = data & 0x02;
		return;
	}
}

/*static*/ UINT8 __fastcall sidearms_main_read(UINT16 address)
{
	switch (address)
	{
		case 0xc800:
		case 0xc801:
		case 0xc802:
			return DrvInputs[address & 3];

		case 0xc803:
		case 0xc804:
			return DrvDips[address - 0xc803];

		case 0xc805:
			return ((vblank) ? 0 : 0x80);
	}

	return 0;
}

/*static*/ void __fastcall turtship_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xe000) {
		DrvPalRAM[address & 0x7ff] = data;
		palette_write(address);
		return;
	}

	switch (address)
	{
		case 0xe800:
			soundlatch = data;
		return;

		case 0xe801:
			bankswitch(data);
		return;

		case 0xe802:
			enable_watchdog = 1;
			watchdog = 0;
		return;

		case 0xe804:
		{
			// coin counters data & 0x01, data & 0x02
			// coin lockout data & 0x04, data & 0x08
#if 0
			if (data & 0x10) {
				CZetClose();
				CZetOpen(1);
				CZetReset();
				CZetClose();
				CZetOpen(0);
			}
#endif
			character_enable = data & 0x40;
			flipscreen = data & 0x80;
		}
		return;

		case 0xe808:
		case 0xe809:
			bgscrollx[address & 1] = data;
		return;

		case 0xe80a:
		case 0xe80b:
			bgscrolly[address & 1] = data;
		return;

		case 0xe80c:
			sprite_enable = data & 0x01;
			bglayer_enable = data & 0x02;
		return;
	}
}

/*static*/ UINT8 turtship_input_read(INT32 offset)
{
	UINT8 ret = 0;
	UINT8 ports[5] = { DrvInputs[0], DrvInputs[1], DrvInputs[2], DrvDips[0], DrvDips[1] };

	for (INT32 i = 0; i < 5; i++) {
		ret |= ((ports[i] >> (offset & 7)) & 1) << i;
	}

	return ret;
}

/*static*/ UINT8 __fastcall turtship_main_read(UINT16 address)
{
	if ((address & 0xfff8) == 0xe800) {
		return turtship_input_read(address);
	}

	return 0;
}

/*static*/ void __fastcall whizz_main_write(UINT16 address, UINT8 data)
{
	switch (address)
	{
		case 0xc801:
			bankswitch(((data >> 5) & 2) | (data >> 7));
		return;

		case 0xc803:
		case 0xc805:
		return;		// nop
	}

	sidearms_main_write(address, data);
}

/*static*/ UINT8 __fastcall whizz_main_read(UINT16 address)
{
	switch (address)
	{
		case 0xc800:
			return DrvDips[1];

		case 0xc801:
			return DrvDips[2];

		case 0xc802:
			return DrvDips[3];

		case 0xc803:
			return (DrvInputs[0] & ~0x10) | (DrvDips[0] & 0x10);

		case 0xc804:
			return DrvInputs[1];

		case 0xc805:
			return DrvInputs[2];

		case 0xc806:
			return (DrvInputs[3] & 0xef) | ((vblank) ? 0x10 : 0);

		case 0xc807:
			return DrvInputs[4];
	}

	return 0;
}

/*static*/ void __fastcall sidearms_sound_write(UINT16 address, UINT8 data)
{
	switch (address)
	{
		case 0xf000:
		case 0xf001:
		case 0xf002:
		case 0xf003:
//			BurnYM2203Write((address/2)&1, address & 1, data);
		return;
	}
}

/*static*/ UINT8 __fastcall sidearms_sound_read(UINT16 address)
{
	switch (address)
	{
		case 0xd000:
			return soundlatch;

		case 0xf000:
		case 0xf001:
		case 0xf002:
		case 0xf003:
//			return BurnYM2203Read((address/2)&1, address & 1);
		return  0;
	}

	return 0;
}

/*static*/ void __fastcall whizz_sound_write_port(UINT16 port, UINT8 data)
{
	switch (port & 0xff)
	{
		case 0x00:
//			BurnYM2151SelectRegister(data);
		return;

		case 0x01:
//			BurnYM2151WriteRegister(data);
		return;

		case 0x40:
		return;		// nop
	}
}

/*static*/ UINT8 __fastcall whizz_sound_read_port(UINT16 port)
{
	switch (port & 0xff)
	{
		case 0x00:
		case 0x01:
			return 0; //BurnYM2151ReadStatus();

		case 0xc0:
			return soundlatch;
	}

	return 0;
}

//inline /*static*/ //void DrvYM2203IRQHandler(INT32, INT32 nStatus)
//{
//	ZetSetIRQLine(0, (nStatus) ? CPU_IRQSTATUS_ACK : CPU_IRQSTATUS_NONE);
//}

/*static*/ //void DrvYM2151IrqHandler(INT32 nStatus)
//{
//	ZetSetIRQLine(0, (nStatus) ? CPU_IRQSTATUS_ACK : CPU_IRQSTATUS_NONE);
//}

//inline /*static*/ INT32 DrvSynchroniseStream(INT32 nSoundRate)
//{
//	return (INT64)(ZetTotalCycles() * nSoundRate / 4000000);
//}

inline /*static*/ double DrvGetTime()
{
	return (double)CZetTotalCycles() / 4000000;
}

/*static*/ INT32 DrvDoReset(INT32 clear_mem)
{
	if (clear_mem) {
		memset (AllRam, 0, RamEnd - AllRam);
	}

	CZetOpen(0);
	CZetReset();
	CZetClose();
#ifdef SOUND
	ZetOpen(1);
	ZetReset();

//	if (is_whizz) {
//		BurnYM2151Reset();
//	} else {
//		BurnYM2203Reset();
//	}
	CZetClose();
#endif
	enable_watchdog = 0;
	watchdog = 0;

	flipscreen = 0;
	soundlatch = 0;
	character_enable = 0;
	sprite_enable = 0;
	bglayer_enable = 0;
	bank_data = 0;

	starfield_enable = 0;
	starscrollx = 0;
	starscrolly = 0;
	hflop_74a = 1;

	return 0;
}

/*static*/ INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;

	DrvZ80ROM0		= Next; Next += 0x018000;
	DrvZ80ROM1		= Next; Next += 0x008000;

//	DrvGfxROM0		= Next; Next += 0x010000;
	DrvGfxROM0		= SS_CACHE;
//	DrvGfxROM1		= Next; Next += 0x100000;
	DrvGfxROM1		= SS_CACHE + 0x04000;
//	DrvGfxROM2		= Next; Next += 0x080000;
	DrvGfxROM2	= (UINT8 *)(ss_vram+0x1100);

	DrvStarMap		= Next; Next += 0x008000;
	DrvTileMap		= Next; Next += 0x008000;

//	DrvPalette		= (UINT32*)Next; Next += 0x0400 * sizeof(UINT32);
	DrvPalette	= (UINT16*)colBgAddr;

	AllRam			= Next;

	DrvVidRAM		= Next; Next += 0x001000;
	DrvSprBuf		= Next; Next += 0x001000;
	DrvSprRAM		= Next; Next += 0x001000;
	DrvPalRAM		= Next; Next += 0x000800;
	DrvZ80RAM0		= Next; Next += 0x002000;
	DrvZ80RAM1		= Next; Next += 0x000800;

	bgscrollx		= Next; Next += 0x000002;
	bgscrolly		= Next; Next += 0x000002;

	RamEnd			= Next;

	remap4to16_lut	= Next; Next += 256 * sizeof (UINT16);
	map_lut				= Next; Next += 256 * sizeof (UINT16);
	map_offset_lut	= Next; Next += 4096 * sizeof (UINT16);
	cram_lut			= Next; Next += 4096 * sizeof (UINT16);
	charaddr_lut		= Next; Next += 0x800 * sizeof (UINT16);

	MemEnd			= Next;

	return 0;
}

/*static*/ void DrvGfxDecode()
{
	INT32 Plane0[2]  = { 4, 0 };
	INT32 Plane1[4]  = { 0x200000 + 4, 0x200000 + 0, 4, 0 };
	INT32 Plane2[4]  = { 0x100000 + 4, 0x100000 + 0, 4, 0 };
	INT32 XOffs0[16] = { STEP4(0,1), STEP4(8,1), STEP4(256,1), STEP4(256+8,1) };
	INT32 XOffs1[32] = { STEP4(0,1), STEP4(8,1), STEP4(512,1), STEP4(512+8,1), STEP4(1024,1), STEP4(1024+8,1), STEP4(1536,1), STEP4(1536+8,1) };
	INT32 YOffs[32]  = { STEP32(0,16) };

//	UINT8 *tmp = (UINT8*)BurnMalloc(0x80000);
//	if (tmp == NULL) {
//		return;
//	}
	UINT8 *tmp = (UINT8*)0x00200000; 
	memset(tmp,0x00,0x40000);
	memcpyl (tmp, DrvGfxROM0, 0x04000);
// text
	GfxDecode4Bpp(0x0400, 2,  8,  8, Plane0, XOffs0, YOffs, 0x080, tmp, DrvGfxROM0);

	for (int i=0;i<0x8000;i++ )
	{
		if ((DrvGfxROM0[i]& 0x03)     ==0x00)DrvGfxROM0[i] = DrvGfxROM0[i] & 0x30 | 0x3;
		else if ((DrvGfxROM0[i]& 0x03)==0x03) DrvGfxROM0[i] = DrvGfxROM0[i] & 0x30;

		if ((DrvGfxROM0[i]& 0x30)       ==0x00)DrvGfxROM0[i] = 0x30 | DrvGfxROM0[i] & 0x03;
		else if ((DrvGfxROM0[i]& 0x30)==0x30) DrvGfxROM0[i] = DrvGfxROM0[i] & 0x03;
	}	
// bg	
	memcpyl (tmp, DrvGfxROM1, 0x40000);

//	GfxDecode4Bpp(0x0400, 4, 32, 32, Plane1, XOffs1, YOffs, 0x800, tmp, DrvGfxROM1);
// sprites
	memcpyl (tmp, DrvGfxROM2, 0x40000);

	GfxDecode4Bpp(0x0800, 4, 16, 16, Plane2, XOffs0, YOffs, 0x200, tmp, DrvGfxROM2);
//	tile16x16toSaturn(1,0x0800, DrvGfxROM2);

//	BurnFree (tmp);
	tmp = NULL;
}

/*static*/ INT32 SidearmsInit()
{
	DrvInitSaturn();

	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"SidearmsInit       ",24,30);

	if ((AllMem = (UINT8 *)BurnMalloc(nLen)) == NULL) 
	{
		FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"malloc failed   ",24,30);
		return 1;
	}
	memset(AllMem, 0, nLen);
	MemIndex();
	make_lut();
	{
		FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"BurnLoadRom     ",24,30);

		if (BurnLoadRom(DrvZ80ROM0 + 0x00000,  0, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x08000,  1, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x10000,  2, 1)) return 1;

		if (BurnLoadRom(DrvZ80ROM1 + 0x00000,  3, 1)) return 1;

		if (BurnLoadRom(DrvStarMap + 0x00000,  4, 1)) return 1;
	
		if (BurnLoadRom(DrvGfxROM0 + 0x00000,  5, 1)) return 1;

		if (BurnLoadRom(DrvGfxROM1 + 0x00000,  6, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x08000,  7, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x10000,  8, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x18000,  9, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x40000, 10, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x48000, 11, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x50000, 12, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x58000, 13, 1)) return 1;

		if (BurnLoadRom(DrvGfxROM2 + 0x00000, 14, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x08000, 15, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x10000, 16, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x18000, 17, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x20000, 18, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x28000, 19, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x30000, 20, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x38000, 21, 1)) return 1;

		if (BurnLoadRom(DrvTileMap + 0x00000, 22, 1)) return 1;
		FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"DrvGfxDecode     ",24,30);

		DrvGfxDecode();
	}
		FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"CZetInit              ",24,30);

	CZetInit(1);
	CZetOpen(0);

	CZetMapArea(0x0000, 0x7fff, 0, DrvZ80ROM0);
	CZetMapArea(0x0000, 0x7fff, 2, DrvZ80ROM0);

	CZetMapArea(0xc000, 0xc7ff, 0, DrvPalRAM);
	CZetMapArea(0xc000, 0xc7ff, 2, DrvPalRAM);

	CZetMapArea(0xd000, 0xdfff, 0, DrvVidRAM);
	CZetMapArea(0xd000, 0xdfff, 1, DrvVidRAM);
	CZetMapArea(0xd000, 0xdfff, 2, DrvVidRAM);

	CZetMapArea(0xe000, 0xefff, 0, DrvZ80RAM0);
	CZetMapArea(0xe000, 0xefff, 1, DrvZ80RAM0);
	CZetMapArea(0xe000, 0xefff, 2, DrvZ80RAM0);

	CZetMapArea(0xf000, 0xffff, 0, DrvSprRAM);
	CZetMapArea(0xf000, 0xffff, 1, DrvSprRAM);
	CZetMapArea(0xf000, 0xffff, 2, DrvSprRAM);

	CZetSetWriteHandler(sidearms_main_write);
	CZetSetReadHandler(sidearms_main_read);
	CZetClose();
#ifdef SOUND
	ZetInit(1);
	ZetOpen(1);
	ZetMapMemory(DrvZ80ROM1,	0x0000, 0x7fff, MAP_ROM);
	ZetMapMemory(DrvZ80RAM1,	0xc000, 0xc7ff, MAP_RAM);
	ZetSetWriteHandler(sidearms_sound_write);
	ZetSetReadHandler(sidearms_sound_read);
	ZetClose();
#endif
//	BurnYM2203Init(2,  4000000, &DrvYM2203IRQHandler, DrvSynchroniseStream, DrvGetTime, 0);
//	BurnTimerAttachZet(4000000);
//	BurnYM2203SetRoute(0, BURN_SND_YM2203_YM2203_ROUTE,   0.25, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(0, BURN_SND_YM2203_AY8910_ROUTE_1, 0.15, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(0, BURN_SND_YM2203_AY8910_ROUTE_2, 0.15, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(0, BURN_SND_YM2203_AY8910_ROUTE_3, 0.15, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(1, BURN_SND_YM2203_YM2203_ROUTE,   0.25, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(1, BURN_SND_YM2203_AY8910_ROUTE_1, 0.15, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(1, BURN_SND_YM2203_AY8910_ROUTE_2, 0.15, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(1, BURN_SND_YM2203_AY8910_ROUTE_3, 0.15, BURN_SND_ROUTE_BOTH);

//	GenericTilesInit();

	DrvDoReset(1);

	drawWindow(0,224,240,0,0);
	return 0;
}

/*static*/ /*INT32 TurtshipInit()
{
	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)BurnMalloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();

	{
		if (BurnLoadRom(DrvZ80ROM0 + 0x00000,  0, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x08000,  1, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x10000,  2, 1)) return 1;

		if (BurnLoadRom(DrvZ80ROM1 + 0x00000,  3, 1)) return 1;
	
		if (BurnLoadRom(DrvGfxROM0 + 0x00000,  4, 1)) return 1;
		if (is_turtshipk == 0) memcpy (DrvGfxROM0, DrvGfxROM0 + 0x4000, 0x4000);

		if (BurnLoadRom(DrvGfxROM1 + 0x00000,  5, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x10000,  6, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x20000,  7, 1)) return 1;
		memcpy (DrvGfxROM1 + 0x30000, DrvGfxROM1 + 0x10000, 0x10000);
		if (BurnLoadRom(DrvGfxROM1 + 0x40000,  8, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x50000,  9, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x60000, 10, 1)) return 1;
		memcpy (DrvGfxROM1 + 0x70000, DrvGfxROM1 + 0x50000, 0x10000);

		if (BurnLoadRom(DrvGfxROM2 + 0x00000, 11, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x10000, 12, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x20000, 13, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x30000, 14, 1)) return 1;

		if (BurnLoadRom(DrvTileMap + 0x00000, 15, 1)) return 1;

		DrvGfxDecode();
	}

	CZetInit(1);
	CZetOpen(0);
	CZetMapMemory(DrvZ80ROM0,	0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory(DrvZ80RAM0,	0xc000, 0xcfff, MAP_RAM);
	CZetMapMemory(DrvSprRAM,		0xd000, 0xdfff, MAP_RAM);
	CZetMapMemory(DrvPalRAM,		0xe000, 0xe7ff, MAP_ROM);
	CZetMapMemory(DrvVidRAM,		0xf000, 0xffff, MAP_RAM);
	CZetSetWriteHandler(turtship_main_write);
	CZetSetReadHandler(turtship_main_read);
	CZetClose();
#ifdef SOUND
	ZetInit(1);
	ZetOpen(1);
	ZetMapMemory(DrvZ80ROM1,	0x0000, 0x7fff, MAP_ROM);
	ZetMapMemory(DrvZ80RAM1,	0xc000, 0xc7ff, MAP_RAM);
	ZetSetWriteHandler(sidearms_sound_write);
	ZetSetReadHandler(sidearms_sound_read);
	ZetClose();
#endif
//	BurnYM2203Init(2,  4000000, &DrvYM2203IRQHandler, DrvSynchroniseStream, DrvGetTime, 0);
//	BurnTimerAttachZet(4000000);
//	BurnYM2203SetRoute(0, BURN_SND_YM2203_YM2203_ROUTE,   0.25, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(0, BURN_SND_YM2203_AY8910_ROUTE_1, 0.15, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(0, BURN_SND_YM2203_AY8910_ROUTE_2, 0.15, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(0, BURN_SND_YM2203_AY8910_ROUTE_3, 0.15, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(1, BURN_SND_YM2203_YM2203_ROUTE,   0.25, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(1, BURN_SND_YM2203_AY8910_ROUTE_1, 0.15, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(1, BURN_SND_YM2203_AY8910_ROUTE_2, 0.15, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(1, BURN_SND_YM2203_AY8910_ROUTE_3, 0.15, BURN_SND_ROUTE_BOTH);

//	GenericTilesInit();

	DrvDoReset(1);

	return 0;
}
*/

/*static*/ /*INT32 TurtshipkInit()
{
	is_turtshipk = 1;
	return TurtshipInit();
}
*/
/*static*/ /*INT32 WhizzInit()
{
	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)BurnMalloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();

	{
		if (BurnLoadRom(DrvZ80ROM0 + 0x00000,  0, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x08000,  1, 1)) return 1;

		if (BurnLoadRom(DrvZ80ROM1 + 0x00000,  2, 1)) return 1;
	
		if (BurnLoadRom(DrvGfxROM0 + 0x00000,  3, 1)) return 1;
		memcpy (DrvGfxROM0, DrvGfxROM0 + 0x4000, 0x4000);

		if (BurnLoadRom(DrvGfxROM1 + 0x00000,  4, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x10000,  5, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x20000,  6, 1)) return 1;
		memcpy (DrvGfxROM1 + 0x30000, DrvGfxROM1 + 0x10000, 0x10000);
		if (BurnLoadRom(DrvGfxROM1 + 0x40000,  7, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x50000,  8, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x60000,  9, 1)) return 1;
		memcpy (DrvGfxROM1 + 0x70000, DrvGfxROM1 + 0x50000, 0x10000);

		if (BurnLoadRom(DrvGfxROM2 + 0x00000, 10, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x10000, 11, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x20000, 12, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x30000, 13, 1)) return 1;

		if (BurnLoadRom(DrvTileMap + 0x00000, 14, 1)) return 1;

		DrvGfxDecode();
	}

	CZetInit(1);
	CZetOpen(0);
	CZetMapMemory(DrvZ80ROM0,	0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory(DrvPalRAM,		0xc000, 0xc7ff, MAP_ROM);
	CZetMapMemory(DrvVidRAM,		0xd000, 0xdfff, MAP_RAM);
	CZetMapMemory(DrvZ80RAM0,	0xe000, 0xefff, MAP_RAM);
	CZetMapMemory(DrvSprRAM,		0xf000, 0xffff, MAP_RAM);
	CZetSetWriteHandler(whizz_main_write);
	CZetSetReadHandler(whizz_main_read);
	CZetClose();
#ifdef SOUND
	ZetInit(1);
	ZetOpen(1);
	ZetMapMemory(DrvZ80ROM1,	0x0000, 0x7fff, MAP_ROM);
	ZetMapMemory(DrvZ80RAM1,	0xf800, 0xffff, MAP_RAM);
	ZetSetOutHandler(whizz_sound_write_port);
	ZetSetInHandler(whizz_sound_read_port);
	ZetClose();
#endif
	BurnYM2151Init(4000000);
	BurnYM2151SetIrqHandler(&DrvYM2151IrqHandler);
	BurnYM2151SetAllRoutes(1.00, BURN_SND_ROUTE_BOTH);

	GenericTilesInit();

	is_whizz = 1;

	DrvDoReset(1);

	return 0;
}
*/

/*static*/ INT32 DrvExit()
{
//	GenericTilesExit();

	CZetExit();

//	if (is_whizz) {
//		BurnYM2151Exit();
//	} else {
//		BurnYM2203Exit();
//	}

	BurnFree (AllMem);

	is_whizz = 0;
	is_turtshipk = 0;

	return 0;
}

/*static*/ void sidearms_draw_starfield()
{
	UINT16 *lineptr = NULL;//pTransDraw;
	UINT32 _hcount_191 = starscrollx & 0xff;

	for (INT32 y = 16; y < (16+nScreenHeight); y++)
	{
		UINT32 hadd_283 = _hcount_191 & ~0x1f;
		UINT32 vadd_283 = starscrolly + y;

		INT32 i = (vadd_283<<4) & 0xff0;
		i |= (hflop_74a^(hadd_283>>8)) << 3;
		i |= (hadd_283>>5) & 7;
		UINT32 latch_374 = DrvStarMap[i + 0x3000];

		hadd_283 = _hcount_191 - 1;

		for (INT32 x = 0; x < nScreenWidth; lineptr++, x++)
		{
			i = hadd_283;
			hadd_283 = _hcount_191 + (x & 0xff);
			vadd_283 = starscrolly + y;

			if (!((vadd_283 ^ (x>>3)) & 4)) continue;
			if ((vadd_283 | (hadd_283>>1)) & 2) continue;

			if ((i & 0x1f)==0x1f)
			{
				i  = (vadd_283<<4) & 0xff0;
				i |= (hflop_74a^(hadd_283>>8)) << 3;
				i |= (hadd_283>>5) & 7;
				latch_374 = DrvStarMap[i + 0x3000];
			}

			if ((~((latch_374^hadd_283)^1) & 0x1f)) continue;

			*lineptr = (UINT16)((latch_374>>5) | 0x378);
		}
	}
}

/*static*/ void draw_bg_layer(INT32 type)
{
	INT32 scrollx = ((((bgscrollx[1] << 8) + bgscrollx[0]) & 0xfff) + 64) & 0xfff;
	INT32 scrolly = ((((bgscrolly[1] << 8) + bgscrolly[0]) & 0xfff) + 16) & 0xfff; 

	for (INT32 y = 0; y < 256; y += 32) {

		INT32 sy = y - (scrolly & 0x1f);
		if (sy >= nScreenHeight) continue;

		INT32 starty = (((scrolly + y) & 0xfff) / 0x20) * 0x80;
	
		for (INT32 x = 0; x < 416; x += 32) {

			INT32 sx = x - (scrollx & 0x1f);
			if (sx >= nScreenWidth) continue;

			INT32 offs = (((scrollx + x) & 0xfff) / 0x20) + starty;
			INT32 ofst = ((offs & 0x3c00) << 1) | ((offs & 0x0380) >> 6) | ((offs & 0x7f) << 4);

			INT32 attr  = DrvTileMap[ofst + 1];
			INT32 code  = DrvTileMap[ofst + 0] + ((attr & 0x01) * 256);
			INT32 color = attr >> 3;
			INT32 flipx = attr & 0x02;
			INT32 flipy = attr & 0x04;
/*
			if (type)
			{
				if (flipy) {
					if (flipx) {
						Render32x32Tile_FlipXY_Clip(pTransDraw, code | ((attr & 0x80) << 2), sx, sy, color & 0x0f, 4, 0, DrvGfxROM1);
					} else {
						Render32x32Tile_FlipY_Clip(pTransDraw, code | ((attr & 0x80) << 2), sx, sy, color & 0x0f, 4, 0, DrvGfxROM1);
					}
				} else {
					if (flipx) {
						Render32x32Tile_FlipX_Clip(pTransDraw, code | ((attr & 0x80) << 2), sx, sy, color & 0x0f, 4, 0, DrvGfxROM1);
					} else {
						Render32x32Tile_Clip(pTransDraw, code | ((attr & 0x80) << 2), sx, sy, color & 0x0f, 4, 0, DrvGfxROM1);
					}
				}
			} else {
				if (flipy) {
					if (flipx) {
						Render32x32Tile_Mask_FlipXY_Clip(pTransDraw, code, sx, sy, color, 4, 0x0f, 0, DrvGfxROM1);
					} else {
						Render32x32Tile_Mask_FlipY_Clip(pTransDraw, code, sx, sy, color, 4, 0x0f, 0, DrvGfxROM1);
					}
				} else {
					if (flipx) {
						Render32x32Tile_Mask_FlipX_Clip(pTransDraw, code, sx, sy, color, 4, 0x0f, 0, DrvGfxROM1);
					} else {
						Render32x32Tile_Mask_Clip(pTransDraw, code, sx, sy, color, 4, 0x0f, 0, DrvGfxROM1);
					}
				}
			}
*/
		}
	}
}

/*static*/ void draw_fg_layer()
{
	for (INT32 offs = 0; offs < 64 * 32; offs++)
	{
		INT32 sx = ((offs & 0x3f) * 8) - 64;
		INT32 sy = ((offs / 0x40) * 8) - 16;

//		if (sx >= nScreenWidth || sx < 0 || sy >= nScreenHeight || sy < 0) continue;

		INT32 attr = DrvVidRAM[0x800 + offs];
		INT32 code = DrvVidRAM[0x000 + offs] | ((attr & 0xc0) << 2);
		INT32 color = attr & 0x3f;

		 sx		= offs & 0x3f;
		 sy		= ((offs<<1) & (~0x3f));
		int x = map_offset_lut[offs];

		ss_map[x] = color; //(attr & 0x1f);
		ss_map[x+1] =code; 
//		Render8x8Tile_Mask_Clip(pTransDraw, code, sx, sy, color, 2, 3, 0x300, DrvGfxROM0);
	}
}

/*static*/ void draw_sprites_region(INT32 start, INT32 end)
{
	UINT32 delta	= (start/32)+3;

	for (INT32 offs = end - 32; offs >= start; offs -= 32)
	{
		INT32 sy = DrvSprBuf[offs + 2];
		if (!sy || DrvSprBuf[offs + 5] == 0xc3) continue;

		INT32 attr  = DrvSprBuf[offs + 1];
		INT32 color = attr & 0xf;
		INT32 code  = DrvSprBuf[offs] + ((attr << 3) & 0x700);
		INT32 sx    = DrvSprBuf[offs + 3] + ((attr << 4) & 0x100);

			ss_sprite[delta].control		= ( JUMP_NEXT | FUNC_NORMALSP); // | flipx;
			ss_sprite[delta].drawMode	= ( ECD_DISABLE | COMPO_REP);

			ss_sprite[delta].ax			= sx;
			ss_sprite[delta].ay			= sy;
			ss_sprite[delta].charSize		= 0x210;
			ss_sprite[delta].color			    = color<<4;//Colour<<4;
			ss_sprite[delta].charAddr		= 0x220+(code<<4);

			delta++;


//		Render16x16Tile_Mask_Clip(pTransDraw, code, sx - 64, sy - 16, color, 4, 0x0f, 0x200, DrvGfxROM2);
	}
}

/*static*/ INT32 SidearmsDraw()
{
	if (DrvRecalc) {
		for (INT32 offs = 0; offs < 0x400; offs++) {
			palette_write(offs);
		}

		DrvRecalc = 0;
	}

//	BurnTransferClear();

	if (starfield_enable) {
		sidearms_draw_starfield();
	}

	if (bglayer_enable) {
		draw_bg_layer(0);
	}

	if (sprite_enable) {
		draw_sprites_region(0x0700, 0x0800);
		draw_sprites_region(0x0e00, 0x1000);
		draw_sprites_region(0x0800, 0x0f00);
		draw_sprites_region(0x0000, 0x0700);
	}

	if (character_enable) {
		draw_fg_layer();
	}

//	BurnTransferCopy(DrvPalette);

	return 0;
}

/*static*/ INT32 TurtshipDraw()
{
	if (DrvRecalc) {
		for (INT32 offs = 0; offs < 0x400; offs++) {
			palette_write(offs);
		}

		DrvRecalc = 0;
	}

	if (bglayer_enable) {
		draw_bg_layer(1);
	} else {
//		BurnTransferClear();
	}

	if (sprite_enable) {
		draw_sprites_region(0x0700, 0x0800);
		draw_sprites_region(0x0e00, 0x1000);
		draw_sprites_region(0x0800, 0x0f00);
		draw_sprites_region(0x0000, 0x0700);
	}

	if (character_enable) {
		draw_fg_layer();
	}

//	BurnTransferCopy(DrvPalette);

	return 0;
}

/*static*/ INT32 DygerDraw()
{
	if (DrvRecalc) {
		for (INT32 offs = 0; offs < 0x400; offs++) {
			palette_write(offs);
		}

		DrvRecalc = 0;
	}

	if (bglayer_enable) {
		draw_bg_layer(1);
	} else {
//		BurnTransferClear();
	}

	if (sprite_enable) {
		draw_sprites_region(0x0000, 0x1000);
	}

	if (character_enable) {
		draw_fg_layer();
	}

//	BurnTransferCopy(DrvPalette);

	return 0;
}

/*static*/ INT32 DrvFrame()
{
//	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Drvframe   ",24,40);

	watchdog++;
	if (watchdog > 180 && enable_watchdog) {
		DrvDoReset(0);
	}
/*
	if (DrvReset) {
		DrvDoReset(1);
	}
*/
	CZetNewFrame();

	{
		memset (DrvInputs, 0xff, 5);

		for (INT32 i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
			DrvInputs[2] ^= (DrvJoy3[i] & 1) << i;
			DrvInputs[3] ^= (DrvJoy4[i] & 1) << i;
			DrvInputs[4] ^= (DrvJoy5[i] & 1) << i;
		}
	}

	INT32 nSoundBufferPos = 0;
	INT32 nInterleave = 278;
	INT32 nCyclesTotal[2] =  { 4000000 / 60, 4000000 / 60 };
	INT32 nCyclesDone[2] = { 0, 0 };

	vblank = 0;

	for (INT32 i = 0; i < nInterleave; i++) {

		INT32 nSegment = nCyclesTotal[0] / nInterleave;

		CZetOpen(0);
		nCyclesDone[0] += CZetRun(nSegment);
		if (i == 274) {CZetSetIRQLine(0, CZET_IRQSTATUS_ACK); vblank = 1; }
		if (i == 276) CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);
		nSegment = CZetTotalCycles();
		CZetClose();
#if 0
		CZetOpen(1);
		if (is_whizz) {
			nCyclesDone[1] += CZetRun(nSegment - CZetTotalCycles());
			if (i == 274) CZetSetIRQLine(0, CZET_IRQSTATUS_ACK);
			if (i == 276) CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);
/*
			if (pBurnSoundOut) {
				INT32 nSegmentLength = nBurnSoundLen / nInterleave;
				INT16* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
				BurnYM2151Render(pSoundBuf, nSegmentLength);
				nSoundBufferPos += nSegmentLength;
			}*/
		} else {
//			BurnTimerUpdate(nSegment);
		}
		CZetClose();
#endif
	}
#if 0
	CZetOpen(1);

	if (is_whizz == 0) {
	//	BurnTimerEndFrame(nCyclesTotal[1]);
	}
/*
	if (pBurnSoundOut) {
		if (is_whizz) {
			INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos;
			INT16* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
	
			if (nSegmentLength) {
//				BurnYM2151Render(pSoundBuf, nSegmentLength);
			}
		} else {
//			BurnYM2203Update(pBurnSoundOut, nBurnSoundLen);
		}
	}
*/
	CZetClose();
#endif	
//	if (pBurnDraw) {
//		BurnDrvRedraw();
//	}
	SidearmsDraw();
	memcpy (DrvSprBuf, DrvSprRAM, 0x1000);

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initLayers()
{
    Uint16	CycleTb[]={
		0xff56, 0xffff, //A0
		0xffff, 0xffff,	//A1
		0x15f2,0x4eff,   //B0
		0xffff, 0xffff  //B1
//		0x4eff, 0x1fff, //B1
	};
 	SclConfig	scfg;

// 3 nbg
// nbg1 16*16 background
// ? passer en 1 word ?
	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_2X2;//OK du 1*1 surtout pas toucher
//	scfg.pnamesize     = SCL_PN1WORD; //2word
	scfg.pnamesize     = SCL_PN2WORD; //2word
	scfg.platesize     = SCL_PL_SIZE_2X2; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
//	scfg.patnamecontrl =  0x0008;// VRAM A0?
	scfg.patnamecontrl =  0x0008;// VRAM A0?
	scfg.flip          = SCL_PN_10BIT; // on force ? 0
 //2x1
/*
	scfg.plate_addr[0] = (Uint32)SS_MAP2;
	scfg.plate_addr[1] = (Uint32)(SS_MAP2+0x800);
	scfg.plate_addr[2] = (Uint32)(SS_MAP2+0x400);	 // good	  0x400
	scfg.plate_addr[3] = (Uint32)(SS_MAP2+0xC00);
*/
// pour 2x1
				scfg.plate_addr[0] = (Uint32)(SS_MAP2);
				scfg.plate_addr[1] = (Uint32)(SS_MAP2+0x1000);
				scfg.plate_addr[2] = (Uint32)(SS_MAP2+0x1000);	 // good	  0x400
				scfg.plate_addr[3] = (Uint32)(SS_MAP2+0x1000);
// pour 2x2
/*
				scfg.plate_addr[0] = (Uint32)(SS_MAP2);//////
				scfg.plate_addr[1] = (Uint32)(SS_MAP2);//////
				scfg.plate_addr[2] = (Uint32)(SS_MAP2+0x1000);	 // good	  0x400
				scfg.plate_addr[3] = (Uint32)(SS_MAP2+0x1000);
*/
	SCL_SetConfig(SCL_NBG1, &scfg);
// 3 nbg
	scfg.pnamesize     = SCL_PN2WORD; //2word
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.charsize      = SCL_CHAR_SIZE_1X1;
	scfg.plate_addr[0] = (Uint32)SS_MAP;
	scfg.plate_addr[1] = (Uint32)SS_MAP;
	scfg.plate_addr[2] = (Uint32)SS_MAP;
	scfg.plate_addr[3] = (Uint32)SS_MAP;
// nbg2 8x8 foreground
	SCL_SetConfig(SCL_NBG2, &scfg);

 	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
	scfg.plate_addr[0] = (Uint32)SS_FONT;
	scfg.dispenbl      = ON;
//	scfg.dispenbl      = OFF;

	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_SPR,OFF);
	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_SetColRamOffset(SCL_NBG0, 3,OFF);
	SCL_SetColRam(SCL_NBG0,8,8,palette);	 // vbt ? remettre
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DrvInitSaturn()
{
	SPR_InitSlaveSH();
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
//	GFS_SetErrFunc(errGfsFunc, NULL);
//	PCM_SetErrFunc(errPcmFunc, NULL);

 	SS_MAP  = ss_map		=(Uint16 *)SCL_VDP2_VRAM_B1+0xC000;		   //c
	SS_MAP2 = ss_map2	=(Uint16 *)SCL_VDP2_VRAM_B1+0x8000;			//8000
// 	SS_MAP  = ss_map		=(Uint16 *)SCL_VDP2_VRAM_B1;		   //c
//	SS_MAP2 = ss_map2	=(Uint16 *)SCL_VDP2_VRAM_B1+0x8000;			//8
	SS_FONT = ss_font		=(Uint16 *)SCL_VDP2_VRAM_B1+0x0000;
//SS_FONT = ss_font		=(Uint16 *)NULL;
	SS_CACHE= cache		=(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum	 = (SclSpPriNumRegister *)SS_N0PRI;
	ss_SpPriNum	 = (SclSpPriNumRegister *)SS_SPPRI;
	ss_OtherPri	= (SclOtherPriRegister *)SS_OTHR;
	ss_BgColMix	= (SclBgColMixRegister *)SS_BGMIX;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_scl			= (Fixed32 *)SS_SCL;

	nBurnLinescrollSize = 0;
	nBurnSprites = 128+3;
//	nBurnFunction = PCM_VblIn;//smpVblIn;

//3 nbg
	SS_SET_N0PRIN(7); // window
	SS_SET_N1PRIN(4); // bg
	SS_SET_N2PRIN(6); // fg
	SS_SET_S0PRIN(4); // sp

//	SS_SET_N1SPRM(1);  // 1 for special priority
//	ss_regs->specialcode=0x000e; // sfcode, upper 8bits, function b, lower 8bits function a
//	ss_regs->specialcode_sel=0; // sfsel, bit 0 for nbg0 // 1 sfcs, bit 0 = 1 for funcion code b, 0 for function code a

	initLayers();
	initColors();
	initSprites(256-1,224-1,0,0,0,-16);

	SCL_Open();
//	ss_reg->n1_move_y =  16 <<16;
	ss_reg->n2_move_y =  16;
	ss_reg->n2_move_x =  80;//(0<<16) ;
//	ss_reg->n2_move_x =  8;//(0<<16) ;
	SCL_Close();


	memset((Uint8 *)SCL_VDP2_VRAM_B1  ,0x22,0x8000);
	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Loading. Please Wait",24,40);
//	memset((Uint8 *)ss_map2,0x11,0x8000);
//	memset((Uint8 *)ss_map3,0,0x2000);
//	memset((Uint8 *)bg_map_dirty,1,0x4000);
	SprSpCmd *ss_spritePtr;
	unsigned int i = 3;
	
	for (i = 3; i <nBurnSprites; i++) 
	{
		ss_spritePtr				= &ss_sprite[i];
		ss_spritePtr->control   = ( JUMP_NEXT | FUNC_NORMALSP);
		ss_spritePtr->drawMode  = ( ECD_DISABLE | COMPO_REP);	// 16 couleurs
		ss_spritePtr->charSize  = 0x210;  //0x100 16*16
	}
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void make_lut(void)
{
    UINT32 j,mx, my;

	unsigned int i,delta=0;

   	for (i = 0; i < 4096;i++) 
	{
		UINT8 r,g,b;
//	UINT16 data = (DrvPalRAM[offset]) | (DrvPalRAM[offset | 0x400] << 8);

		r = (i >> 4) & 0x0f;
		g = (i >> 0) & 0x0f;
		b = (i >> 8) & 0x0f;

		r |= r << 4;
		g |= g << 4;
		b |= b << 4;
		cram_lut[i] = RGB(r>>3, g>>3, b>>3);
	}

	for (i = 0; i < 256;i++) 
	{
		if ((i%4)==0)
		{
			remap4to16_lut[i] = delta+3;
		}
		else if (((i+1)%4)==0)
		{
			remap4to16_lut[i] = delta-3;
		} 
		else
		{
			remap4to16_lut[i] = delta;
		}
		delta++; if ((delta & 3) == 0) delta += 12;
	}

	j = 0;
	for (my = 0; my < 64; my++) 
	{
		for (mx = 0; mx < 128; mx++) 
		{
			map_offset_lut[j] = ((mx)|(my<<7))<<1;
			j++;
		}
	}

	for (UINT32 i = 0; i<0x800; i++)
	{
		charaddr_lut[i] = 0x220+(i<<4);
	}

	for (UINT32 i = 0; i<256; i++)
	{
		map_lut[i] = ((i & 0x80)<<7) | i & 0x7f;
	}

/*	for (int i = 0; i < nInterleave; i++)
	{
			cpu_lut[i] = ((i + 1) * nCyclesTotal) / nInterleave;
	}*/
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void tile16x16toSaturn (unsigned char reverse, unsigned int num, unsigned char *pDest)
{
	unsigned int c;
	for (c = 0; c < num; c++) 
	{
		unsigned char new_tile[128];
		UINT8 *dpM = pDest + (c * 128);
		memcpyl(new_tile,dpM,128);
		unsigned int i=0,j=0,k=0;

		for (k=0;k<128;k+=64)
		{
			dpM = pDest + ((c * 128)+k);

			for (i=0;i<32;i+=4,j+=8)
			{
				if(reverse)
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
				else
				{
					dpM[i+32]=new_tile[j];
					dpM[i+33]=new_tile[j+1];
					dpM[i+34]=new_tile[j+2];
					dpM[i+35]=new_tile[j+3];
					dpM[i+0]=new_tile[j+4];
					dpM[i+1]=new_tile[j+5];
					dpM[i+2]=new_tile[j+6];
					dpM[i+3]=new_tile[j+7];
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
