#include "d_freekick.h"

// Gigas / Freekick / Counter Run / Perfect Billiard for FBA, ported by vbt
#define nInterleave 256

int ovlInit(char *szShortName)
{
	 struct BurnDriver nBurnDrvPbillrd = {
		"pbillrd", "freek",
		"Perfect Billiard",
		pbillrdRomInfo, pbillrdRomName, PbillrdInputInfo, PbillrdDIPInfo,
		pbillrdInit, DrvExit, DrvFrame, DrvDraw
	};

	struct BurnDriver nBurnDrvFreekickb1 = {
		"freekb1", "freek",
		"Free Kick (bootleg set 1)",
		freekickb1RomInfo, freekickb1RomName, FreekckInputInfo, FreekckDIPInfo,
		DrvFreeKickInit, DrvExit, DrvFrame, DrvDraw
	};

	struct BurnDriver nBurnDrvCountrunb = {
		"countrnb", "freek",
		"Counter Run (bootleg set 1)",
		countrunbRomInfo, countrunbRomName, CountrunInputInfo, CountrunDIPInfo,
		DrvFreeKickInit, DrvExit, DrvFrame, DrvDraw
	};

	struct BurnDriver nBurnDrvGigasb = {
		"gigasb", "freek",
		"Gigas (bootleg)",
		gigasbRomInfo, gigasbRomName, GigasInputInfo, GigasDIPInfo,
		DrvInit, DrvExit, DrvFrame, DrvDraw
	};

	struct BurnDriver nBurnDrvGigasm2 = {
		"gigasm2b", "freek",
		"Gigas Mark II", 
		 gigasm2RomInfo, gigasm2RomName, GigasInputInfo, Gigasm2DIPInfo,
		DrvInit, DrvExit, DrvFrame, DrvDraw
	};

	struct BurnDriver nBurnDrvOmega = {
		"omega", "freek",
		"Omega",
		omegaRomInfo, omegaRomName, GigasInputInfo, OmegaDIPInfo,
		DrvInit, DrvExit, DrvFrame, DrvDraw
	};

	if (strcmp(nBurnDrvPbillrd.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvPbillrd,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvFreekickb1.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvFreekickb1,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvCountrunb.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvCountrunb,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvGigasb.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvGigasb,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvGigasm2.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvGigasm2,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvOmega.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvOmega,sizeof(struct BurnDriver));
	
	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}

/*static*/ INT32 DrvDoReset()
{
	memset (AllRam, 0, RamEnd - AllRam);
	coin = 0;
	nmi_enable = 0;
	ff_data = 0;
	romaddr = 0;

	spinner = 0;
	DrvDial1 = 0;
	DrvDial2 = 0;

	CZetOpen(0);
	CZetReset();
	CZetClose();

	return 0;
}

/*static*/ void pbillrd_draw_sprite(INT32 offs)
{
	INT32 sx = DrvSprRAM[offs + 3];
	INT32 sy = 224 - DrvSprRAM[offs + 2];

	if(sx > 0 && sy > 0)
	{

		UINT32 code = DrvSprRAM[offs + 0];
		UINT32 color = DrvSprRAM[offs + 1] & 0x0f;

		ss_sprite[sprite_number].ax		= sx;
		ss_sprite[sprite_number].ay		= sy;
		ss_sprite[sprite_number].charAddr = 0x220 +(code << 4);
		ss_sprite[sprite_number].color     = (color<<4);
	}
	else
	{
		ss_sprite[sprite_number].ax		= -16;
		ss_sprite[sprite_number].ay		= -16;
	}
	sprite_number++;
}

void countrun_draw_sprite(INT32 offs)
{
	INT32 sx = DrvSprRAM[offs + 3];
	INT32 sy = 232-DrvSprRAM[offs + 0];

	if(sx > 0 && sy > 0)
	{	
		UINT32 code = DrvSprRAM[offs + 1] + ((DrvSprRAM[offs + 2] & 0x20) << 3)  & 0x1ff;
		UINT32 color = DrvSprRAM[offs + 2] & 0x1f;
		UINT32 flipx	= (DrvSprRAM[offs + 2] & 0x80) >> 3;    //?? unused ?
		UINT32 flipy	= 0x20 - (((DrvSprRAM[offs + 2] & 0x40)) >> 1);

		ss_sprite[sprite_number].ax		= sx;
		ss_sprite[sprite_number].ay		= sy;
		ss_sprite[sprite_number].control   = ( JUMP_NEXT | FUNC_NORMALSP | flipy | flipx);
		ss_sprite[sprite_number].charAddr = 0x220 +(code << 4);
		ss_sprite[sprite_number].color     = (color<<4);
	}
	else
	{
		ss_sprite[sprite_number].ax		= -16;
		ss_sprite[sprite_number].ay		= -16;
	}
	sprite_number++;
}

void freekick_draw_sprite(INT32 offs)
{
	INT32 sx = 224 - DrvSprRAM[offs + 3];
	INT32 sy = 240 - DrvSprRAM[offs + 0];

	if(sx > 0 && sy > 0)
	{	
		UINT32 code = DrvSprRAM[offs + 1] + ((DrvSprRAM[offs + 2] & 0x20) << 3)  & 0x1ff;
		UINT32 color = DrvSprRAM[offs + 2] & 0x1f;
		UINT32 flipx	= (!DrvSprRAM[offs + 2] & 0x80) >> 2;    //?? unused ?
		UINT32 flipy	= 0x10-(((DrvSprRAM[offs + 2] & 0x40)) >> 2);

		ss_sprite[sprite_number].ax		= sy;
		ss_sprite[sprite_number].ay		= sx;
		ss_sprite[sprite_number].control   = ( JUMP_NEXT | FUNC_NORMALSP | flipy | flipx);
		ss_sprite[sprite_number].charAddr = 0x220 +(code << 4);
		ss_sprite[sprite_number].color     = (color<<4);
		sprite_number++;
	}
}

/*static*/ void gigas_draw_sprite(INT32 offs)
{
	INT32 sx = 224 - DrvSprRAM[offs + 3];
	INT32 sy = 240 - DrvSprRAM[offs + 2];

	if(sx > 0 && sy > 0)
	{
		UINT32 code = (DrvSprRAM[offs + 0] | ((DrvSprRAM[offs + 1] & 0x20) << 3)) & 0x1ff;
		UINT32 color = DrvSprRAM[offs + 1] & 0x1f;

		ss_sprite[sprite_number].ax		= sy;
		ss_sprite[sprite_number].ay		= sx;
		ss_sprite[sprite_number].charAddr = 0x220 +(code << 4);
		ss_sprite[sprite_number].color     = (color<<4);
		sprite_number++;
	}
}

/*static*/ INT32 DrvDraw()
{
	// Draw tiles
	for (INT32 offs = 0x3ff; offs >= 0; offs--)
	{
		UINT32 code  = DrvVidRAM[offs] + ((DrvVidRAM[offs + 0x400] & 0xe0) << 3);
		UINT32 color = DrvVidRAM[offs + 0x400] & 0x1f;
		UINT32 x = map_offset_lut[offs]; 

		ss_map2[x] = color;
		ss_map2[x+1] =code; 
	}

	for (UINT32 offs = 0; offs < 0x100; offs += 4)
	{
		DrawSprite(offs);
	}
	return 0;
}

/*static*/ void __fastcall freekick_write(UINT16 address, UINT8 data)
{
	switch (address)
	{
//	AM_RANGE(0xec00, 0xec03) AM_DEVREADWRITE("ppi8255_0", i8255_device, read, write)
		case 0xec00:
		case 0xec01:
		case 0xec02:
		case 0xec03:
			ppi8255_w(0, address & 0x03, data);
			return;

//	AM_RANGE(0xf000, 0xf003) AM_DEVREADWRITE("ppi8255_1", i8255_device, read, write)
		case 0xf000:
		case 0xf001:
		case 0xf002:
		case 0xf003:
			ppi8255_w(1, address & 0x03, data);
			return;

//	AM_RANGE(0xf800, 0xf800) AM_READ_PORT("IN0") AM_WRITE(flipscreen_w)
		case 0xf800:
		case 0xf801:
			return;

		case 0xf802:
		case 0xf803:
			coin = ~data & 1;
			return;

		case 0xf804: 
//			bprintf(0, _T("nmi enable %X\n"), data);
			nmi_enable = data & 1;
			return;

// AM_RANGE(0xf806, 0xf806) AM_WRITE(spinner_select_w)
		case 0xf806:
			spinner = data & 1;
		return;

		case 0xfc00: 
			SN76496Write(0, data);
		return;
		
		case 0xfc01:
			SN76496Write(1, data);
		return;
		
		case 0xfc02:
			SN76496Write(2, data);
		return;

		case 0xfc03:
			SN76496Write(3, data);
		return;
	}
}

UINT8 __fastcall freekick_read(UINT16 address)
{
	switch (address)
	{
//	AM_RANGE(0xec00, 0xec03) AM_DEVREADWRITE("ppi8255_0", i8255_device, read, write)
		case 0xec00:
		case 0xec01:
		case 0xec02:
		case 0xec03:
			return ppi8255_r(0, address & 0x03);

//	AM_RANGE(0xf000, 0xf003) AM_DEVREADWRITE("ppi8255_1", i8255_device, read, write)
		case 0xf000:
		case 0xf001:
		case 0xf002:
		case 0xf003:
			return ppi8255_r(1, address & 0x03);

		case 0xf800: {
			return DrvInputs[0];
		}

		case 0xf801: {
			return DrvInputs[1];
		}

//	AM_RANGE(0xf802, 0xf802) AM_READNOP //MUST return bit 0 = 0, otherwise game resets
		case 0xf802:
			return 0;

//	AM_RANGE(0xf803, 0xf803) AM_READ(spinner_r)
		case 0xf803:  
		{
			if(spinner)
				return DrvDial2;
			else
				return DrvDial1;
		}
	}
	return 0;
}

/*static*/ void pbillrd_setbank(UINT8 banknum)
{
	DrvZ80Bank0 = banknum; // for savestates

	UINT32 bankloc = 0x10000 + banknum * 0x4000;

/*	if (use_encrypted) {
		ZetMapArea(0x8000, 0xbfff, 0, DrvMainROM + bankloc); // read
		ZetMapArea(0x8000, 0xbfff, 2, DrvMainROMdec + bankloc, DrvMainROM + bankloc); // fetch ops(encrypted), opargs(unencrypted)
	} else {	*/
		CZetMapArea(0x8000, 0xbfff, 0, DrvMainROM + bankloc); // read
		CZetMapArea(0x8000, 0xbfff, 2, DrvMainROM + bankloc); // fetch
//	}
}

/*static*/ void __fastcall gigas_write(UINT16 address, UINT8 data)
{
//	AM_RANGE(0xe000, 0xe000) AM_WRITENOP
//	AM_RANGE(0xe002, 0xe003) AM_WRITE(coin_w)
//	AM_RANGE(0xe004, 0xe004) AM_WRITE(nmi_enable_w)
//	AM_RANGE(0xe005, 0xe005) AM_WRITENOP
	switch (address)
	{
		case 0xe000:
//		case 0xe001:
//			flipscreen = data;
			return;

		case 0xe002:
		case 0xe003:
			coin = ~data & 1;
			return;

		case 0xe004: //bprintf(0, _T("nmi enable %X\n"), data);
			nmi_enable = data & 1;
			return;
		case 0xe005:
			return;
// 	AM_RANGE(0xf000, 0xf000) AM_WRITE(SMH_NOP) //bankswitch ?

		case 0xf000: {
			if (pbillrdmode) {
				pbillrd_setbank(data & 1);
			}
			return;
		}

//	AM_RANGE(0xfc00, 0xfc00) AM_WRITE(sn76496_0_w)
//	AM_RANGE(0xfc01, 0xfc01) AM_WRITE(sn76496_1_w)
//	AM_RANGE(0xfc02, 0xfc02) AM_WRITE(sn76496_2_w)
//	AM_RANGE(0xfc03, 0xfc03) AM_WRITE(sn76496_3_w)

		case 0xfc00: 
			SN76496Write(0, data);
		return;
		
		case 0xfc01:
			SN76496Write(1, data);
		return;
		
		case 0xfc02:
			SN76496Write(2, data);
		return;

		case 0xfc03:
			SN76496Write(3, data);
		return;
	}
}

UINT8 __fastcall gigas_read(UINT16 address)
{
	switch (address)
	{
		case 0xe000:
		{
			return DrvInputs[0];
			//	AM_RANGE(0xe000, 0xe000) AM_READ_PORT("IN0") AM_WRITENOP // probably not flipscreen
		}

		case 0xe800:
		{
			return DrvInputs[1];
			//	AM_RANGE(0xe800, 0xe800) AM_READ_PORT("IN1")
		}

		case 0xf000: {
			return DrvDip[0];//	AM_RANGE(0xf000, 0xf000) AM_READ_PORT("DSW1") AM_WRITENOP //bankswitch ?
		}

		case 0xf800: {
			return DrvDip[1];//	AM_RANGE(0xf800, 0xf800) AM_READ_PORT("DSW2")
		}
	}
	return 0;
}

UINT8 __fastcall freekick_in(UINT16 address)
{
	switch (address & 0xff)
	{
		case 0xff:
            // 	AM_RANGE(0xff, 0xff) AM_READWRITE(freekick_ff_r, freekick_ff_w)
			return ff_data;
		break;
	}

	return 0;
}

void __fastcall freekick_out(UINT16 address, UINT8 data)
{
	switch (address & 0xff)
	{
		case 0xff:
            // 	AM_RANGE(0xff, 0xff) AM_READWRITE(freekick_ff_r, freekick_ff_w)
			ff_data = data;
		break;
	}
}

/*static*/ UINT8 freekick_ppiread_1_c()
{
	return DrvSndROM[romaddr & 0x7fff];
}

/*static*/ void freekick_ppi_write_1_a(UINT8 data)
{
	romaddr = (romaddr & 0xff00) | data;
}

/*static*/ void freekick_ppi_write_1_b(UINT8 data)
{
	romaddr = (romaddr & 0x00ff) | (data << 8);
}

/*static*/ UINT8 freekick_ppiread_2_a()
{
	return DrvDip[0];
}

/*static*/ UINT8 freekick_ppiread_2_b()
{
	return DrvDip[1];
}

/*static*/ UINT8 freekick_ppiread_2_c()
{
	return DrvDip[2];
}

UINT8 __fastcall gigas_in(UINT16 address)
{
	switch (address & 0xff)
	{
		case 0x00: 
			if(spinner)
				return DrvDial2;
			else
				return DrvDial1;
		break;
		
		case 0x01:
			return DrvDip[2];
		break;
	}

	return 0;
}

void __fastcall gigas_out(UINT16 address, UINT8 data)
{
	switch (address & 0xff)
	{
		case 0x00:
			spinner = data & 1;
		break;
	}
}


/*static*/ INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;
//	DrvMainROM	 	= Next; Next += 0x40000;
	DrvMainROM	 	= Next; Next += 0x40000;
	DrvMainROMdec   = Next; Next += 0x20000;
//	DrvMainROMdec   = Next; Next += 0x10000;
	DrvSndROM		= Next; Next += 0x10000;
	MC8123Key		= Next; Next += 0x02000;
	DrvColPROM		= Next; Next += 0x00600;
	CZ80Context		= Next; Next += 0x1080;
	map_offset_lut  =  Next; Next +=0x400*sizeof(UINT16);
	AllRam				= Next;

	DrvRAM			= Next; Next += 0x02000; // 0x0e000 - 0x0c000
	DrvVidRAM		= Next; Next += 0x00800;
	DrvSprRAM		= Next; Next += 0x00100;
	DrvColRAM		= Next; Next += 0x00600;

	RamEnd			= Next;

	MemEnd			= Next;

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvGfxDecode()
{
	UINT32 Planes0[3] = { RGN_FRAC(0xc000, 2,3), RGN_FRAC(0xc000, 1,3), RGN_FRAC(0xc000, 0,3) };
	UINT32 XOffs0[8]  = {0, 1, 2, 3, 4, 5, 6, 7};
	UINT32 YOffs0[8]  = {STEP8(0, 8)};

	UINT32 Planes1[3] = { RGN_FRAC(0xc000, 0,3),RGN_FRAC(0xc000, 2,3),RGN_FRAC(0xc000, 1,3) };
	UINT32 XOffs1[16] = {0, 1, 2, 3, 4, 5, 6, 7,128+0,128+1,128+2,128+3,128+4,128+5,128+6,128+7};

//	INT32 YOffs1[16] = {0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 8*8, 9*8, 10*8, 11*8,12*8,13*8,14*8,15*8};
	UINT32 YOffs1[16] = {15*8, 14*8, 13*8, 12*8, 11*8, 10*8, 9*8, 8*8, 7*8, 6*8, 5*8, 4*8,3*8,2*8,1*8,0*8};
	UINT8 *DrvGfxTMP0 = (UINT8 *)0x00200000;
	UINT8 *DrvGfxTMP1 = (UINT8 *)0x00218000;

	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	UINT8 *DrvGfxROM1		= (UINT8 *)(ss_vram+0x1100); // 0x200 * 16 * 16
	UINT8 *DrvGfxROM0		= SS_CACHE; // 0x800 * 8 * 8

	GfxDecode4Bpp(0x0800, 3,  8,  8, Planes0, XOffs0, YOffs0, 0x40, DrvGfxTMP0, DrvGfxROM0);
	GfxDecode4Bpp(0x0200, 3, 16, 16, Planes1, XOffs1, YOffs1, 0x100, DrvGfxTMP1, DrvGfxROM1);

	if(!countrunbmode && !pbillrdmode)
	{
		rotate_tile(0x800,1,DrvGfxROM0);
		rotate_tile16x16(0x200,1,DrvGfxROM1);
	}
}

/*static*/ void DrvPaletteInit()
{
	UINT32 len = 0x200;
	UINT32 delta = 0;

	for (UINT32 i = 0; i < len; i++)
	{
		UINT32 bit0,bit1,bit2,bit3,r,g,b;

		bit0 = (DrvColPROM[i] >> 0) & 0x01;
		bit1 = (DrvColPROM[i] >> 1) & 0x01;
		bit2 = (DrvColPROM[i] >> 2) & 0x01;
		bit3 = (DrvColPROM[i] >> 3) & 0x01;
		r = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		bit0 = (DrvColPROM[i + len] >> 0) & 0x01;
		bit1 = (DrvColPROM[i + len] >> 1) & 0x01;
		bit2 = (DrvColPROM[i + len] >> 2) & 0x01;
		bit3 = (DrvColPROM[i + len] >> 3) & 0x01;
		g = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		bit0 = (DrvColPROM[i + len * 2] >> 0) & 0x01;
		bit1 = (DrvColPROM[i + len * 2] >> 1) & 0x01;
		bit2 = (DrvColPROM[i + len * 2] >> 2) & 0x01;
		bit3 = (DrvColPROM[i + len * 2] >> 3) & 0x01;
		b = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

		colBgAddr[delta] = BurnHighCol(r,g,b,0); // fg
		delta++; if ((delta & 7) == 0) delta += 8;
	}
}

/* INT32 GigasDecode()
{
	mc8123_decrypt_rom(0, 0, DrvMainROM, DrvMainROM + 0x10000, MC8123Key);
	return 0;
}*/

/*static*/ INT32 LoadRoms()
{
	UINT32 rom_number = 0;

	countrunbmode = !strcmp(BurnDrvGetTextA(DRV_NAME), "countrnb");

	if (!strcmp(BurnDrvGetTextA(DRV_NAME), "countrnb") || 
		!strcmp(BurnDrvGetTextA(DRV_NAME), "freekick") 
	) 
	{
		if (BurnLoadRom(DrvMainROM,  rom_number++, 1)) return 1;

		if (BurnLoadRom(DrvSndROM,   rom_number++, 1)) return 1;	// sound rom
	}

	if (!strcmp(BurnDrvGetTextA(DRV_NAME), "pbillrd") 
//	||	!strcmp(BurnDrvGetTextA(DRV_NAME), "pbillrdsa")
		)
	{
		if (BurnLoadRom(DrvMainROM,  rom_number++, 1)) return 1;
		if (BurnLoadRom(DrvMainROM + 0x04000,  rom_number++, 1)) return 1;
		memmove(DrvMainROM + 0x10000, DrvMainROM + 0x08000, 0x4000);
		if (BurnLoadRom(DrvMainROM + 0x14000,  rom_number++, 1)) return 1;
	}

	if (!strcmp(BurnDrvGetTextA(DRV_NAME), "omega"))
	{
		if (BurnLoadRom(DrvMainROM,  rom_number++, 1)) return 1;
		if (BurnLoadRom(DrvMainROM + 0x04000,  rom_number++, 1)) return 1;

		if (BurnLoadRom(MC8123Key,  rom_number++, 1)) return 1;

		mc8123_decrypt_rom(0, 1, DrvMainROM, DrvMainROMdec, MC8123Key);
		use_encrypted = 1;
	}

	if (!strcmp(BurnDrvGetTextA(DRV_NAME), "gigasb")) 
	{
		if (BurnLoadRom(DrvMainROM + 0x10000,  rom_number++, 1)) return 1;
		memmove(DrvMainROM + 0x00000, DrvMainROM + 0x14000, 0x4000);
		if (BurnLoadRom(DrvMainROM + 0x14000,  rom_number++, 1)) return 1;
		memmove(DrvMainROM + 0x04000, DrvMainROM + 0x1c000, 0x8000);
	}

	if (!strcmp(BurnDrvGetTextA(DRV_NAME), "gigasm2b")) 
	{
		if (BurnLoadRom(DrvMainROM + 0x10000,  rom_number++, 1)) return 1;
		memmove(DrvMainROM + 0x00000, DrvMainROM + 0x14000, 0x4000);

		if (BurnLoadRom(DrvMainROM + 0x14000,  rom_number++, 1)) return 1;
		memmove(DrvMainROM + 0x04000, DrvMainROM + 0x18000, 0x4000);

		if (BurnLoadRom(DrvMainROM + 0x18000,  rom_number++, 1)) return 1;
		memmove(DrvMainROM + 0x08000, DrvMainROM + 0x1c000, 0x4000);
	}
	UINT8 *DrvGfxTMP0 = (UINT8 *)0x00200000;
	UINT8 *DrvGfxTMP1 = (UINT8 *)0x00218000;
	memset(DrvGfxTMP0, 0xff, 0x40000);
	// Gfx char
	if (BurnLoadRom(DrvGfxTMP0  + 0x00000,  rom_number++, 1)) return 1; // ( "4.3k", 0x00000, 0x04000
	if (BurnLoadRom(DrvGfxTMP0  + 0x04000,  rom_number++, 1)) return 1; // ( "5.3h", 0x04000, 0x04000
	if (BurnLoadRom(DrvGfxTMP0  + 0x08000,  rom_number++, 1)) return 1; // ( "6.3g", 0x08000, 0x04000
	// Gfx sprite
	if (BurnLoadRom(DrvGfxTMP1  + 0x00000,  rom_number++, 1)) return 1; // ( "1.3p", 0x00000, 0x04000
	if (BurnLoadRom(DrvGfxTMP1  + 0x04000,  rom_number++, 1)) return 1; // ( "3.3l", 0x04000, 0x04000
	if (BurnLoadRom(DrvGfxTMP1  + 0x08000,  rom_number++, 1)) return 1; // ( "2.3n", 0x08000, 0x04000
	// Opcode Decryption PROMS
	//		GigasDecode(); - not used due to incomplete "gigas" romset.
	// Palette
	if (BurnLoadRom(DrvColPROM + 0x000000,	rom_number++, 1)) return 1; // ( "3a.bin", 0x0000, 0x0100
	if (BurnLoadRom(DrvColPROM + 0x000100,  rom_number++, 1)) return 1; // ( "4d.bin", 0x0100, 0x0100
	if (BurnLoadRom(DrvColPROM + 0x000200,	rom_number++, 1)) return 1; // ( "4a.bin", 0x0200, 0x0100
	if (BurnLoadRom(DrvColPROM + 0x000300,	rom_number++, 1)) return 1; // ( "3d.bin", 0x0300, 0x0100
	if (BurnLoadRom(DrvColPROM + 0x000400,	rom_number++, 1)) return 1; // ( "3b.bin", 0x0400, 0x0100
	if (BurnLoadRom(DrvColPROM + 0x000500,	rom_number++, 1)) return 1; // ( "3c.bin", 0x0500, 0x0100
	return 0;
}

/*static*/ INT32 DrvFreeKickInit()
{
	DrvInitSaturn();
	DrawSprite = freekick_draw_sprite;
	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)BurnMalloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();

	LoadRoms();

	if(countrunbmode)
	{
		DrawSprite = countrun_draw_sprite;
		drawWindow(0,224,240,0,64);
	}

	DrvPaletteInit();
	DrvGfxDecode();

	CZetInit2(1,CZ80Context);
	CZetOpen(0);
	CZetMapMemory(DrvMainROM,	0x0000, 0xcfff, MAP_ROM);
	CZetMapMemory(DrvRAM,		0xd000, 0xdfff, MAP_RAM);
	CZetMapMemory(DrvVidRAM,	0xe000, 0xe7ff, MAP_RAM);
	CZetMapMemory(DrvSprRAM,	0xe800, 0xe8ff, MAP_RAM);

	ppi8255_init(2);

	PPI0PortReadC  = freekick_ppiread_1_c;
	PPI0PortWriteA = freekick_ppi_write_1_a;
	PPI0PortWriteB = freekick_ppi_write_1_b;

	PPI1PortReadA = freekick_ppiread_2_a;
	PPI1PortReadB = freekick_ppiread_2_b;
	PPI1PortReadC = freekick_ppiread_2_c;

	CZetSetReadHandler(freekick_read);
	CZetSetWriteHandler(freekick_write);
	CZetSetInHandler(freekick_in);
	CZetSetOutHandler(freekick_out);

	CZetClose();

	SN76489AInit(0, 12000000/4, 0);
	SN76489AInit(1, 12000000/4, 1);
	SN76489AInit(2, 12000000/4, 1);
	SN76489AInit(3, 12000000/4, 1);

	make_lut();
	DrvDoReset();
	return 0;
}

/*static*/ INT32 DrvInit()
{
	DrvInitSaturn();
	DrawSprite = gigas_draw_sprite;

	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)BurnMalloc(nLen)) == NULL) 
	{
	return 1;
	}
	memset(AllMem, 0, nLen);
	MemIndex();
	LoadRoms();

	if(pbillrdmode)
	{
		DrawSprite = pbillrd_draw_sprite;
		ss_reg->n1_move_y =  16 <<16;
		drawWindow(0,224,240,0,64);
	}
	DrvPaletteInit();
	DrvGfxDecode();

	CZetInit2(1,CZ80Context);
	CZetOpen(0);

	if (pbillrdmode) {
		CZetMapArea(0x0000, 0x7fff, 0, DrvMainROM); // read
		CZetMapArea(0x0000, 0x7fff, 2, DrvMainROM); // fetch
/*
		if (use_encrypted) {
			ZetMapArea(0x0000, 0x7fff, 0, DrvMainROM);
			ZetMapArea(0x0000, 0x7fff, 2, DrvMainROMdec, DrvMainROM); // fetch ops(encrypted), opargs(unencrypted)
		}
*/
		pbillrd_setbank(0);
	} else { // gigas*
		CZetMapArea(0x0000, 0xbfff, 0, DrvMainROM);
		CZetMapArea2(0x0000, 0xbfff, 2, DrvMainROM + 0x10000, DrvMainROM);

		if (use_encrypted) {
			CZetMapArea(0x0000, 0xbfff, 0, DrvMainROM);
			CZetMapArea2(0x0000, 0xbfff, 2, DrvMainROMdec, DrvMainROM); // fetch ops(encrypted), opargs(unencrypted)
		}
	}

	CZetMapMemory(DrvRAM,		0xc000, 0xcfff, MAP_RAM);
	CZetMapMemory(DrvVidRAM,	0xd000, 0xd7ff, MAP_RAM);
	CZetMapMemory(DrvSprRAM,	0xd800, 0xd8ff, MAP_RAM);
	CZetMapMemory(DrvRAM + 0x1000,	0xd900, 0xdfff, MAP_RAM);

	ppi8255_init(1);

	CZetSetReadHandler(gigas_read); // Memory
	CZetSetWriteHandler(gigas_write);

	CZetSetInHandler(gigas_in); // IO Port
	CZetSetOutHandler(gigas_out);

	CZetClose();
	wait_vblank();
	SN76489AInit(0, 12000000/4, 0);
	SN76489AInit(1, 12000000/4, 1);
	SN76489AInit(2, 12000000/4, 1);
	SN76489AInit(3, 12000000/4, 1);

	make_lut();

	DrvDoReset();

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void rotate_tile16x16(unsigned int size,unsigned char flip, unsigned char *target)
{
	unsigned int i,j,l=0;
	unsigned char temp[16][16];
	unsigned char rot[16][16];

	for (unsigned int k=0;k<size;k++)
	{
		for(i=0;i<16;i++)
			for(j=0;j<8;j++)
			{
				temp[i][j<<1]=target[l+(i*8)+j]>>4;
				temp[i][(j<<1)+1]=target[l+(i*8)+j]&0x0f;
			}

		memset(&target[l],0,128);
		
		for(i=0;i<16;i++)
			for(j=0;j<16;j++)
			{
				if(flip)
				 rot[15-i][j]= temp[j][i] ;
				else
				 rot[i][15-j]= temp[j][i] ;
			}

		for(i=0;i<16;i++)
			for(j=0;j<8;j++)
					target[l+(i*8)+j]    = (rot[i][j*2]<<4)|(rot[i][(j*2)+1]&0xf);
		l+=128;
	}	
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initLayers()
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
	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD; //2word
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.flip          = SCL_PN_10BIT; // on force à 0
	scfg.plate_addr[0] = (Uint32)SS_MAP2;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG1, &scfg);
// 3 nbg
	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
	scfg.plate_addr[0] = (Uint32)SS_FONT;

// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);

//	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.plate_addr[0] = (Uint32)SS_MAP;
//	scfg.plate_addr[1] = (Uint32)SS_MAP;
//	scfg.plate_addr[2] = (Uint32)SS_MAP;
//	scfg.plate_addr[3] = (Uint32)SS_MAP;
	scfg.dispenbl      = OFF;
	SCL_SetConfig(SCL_NBG2, &scfg);
//	scfg.dispenbl      = OFF;
	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
	SCL_AllocColRam(SCL_NBG0,OFF);
	colBgAddr2  = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void make_lut(void)
{
	for (UINT32 i = 0; i < 1024;i++) 
	{
		UINT32	sx, sy;

		if(countrunbmode || pbillrdmode)
		{
			sx = i & 0x1f;
			sy = (i<<1) & (~0x3f);
		}
		else
		{
			sx = (31-((i) & 0x1f))<<6;//% 32;
			sy = ((i >> 5)) & 0x3f;
		}
		map_offset_lut[i] = (sx | sy)<<1;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn()
{
//	SPR_InitSlaveSH();

 	SS_MAP  = ss_map		=(Uint16 *)SCL_VDP2_VRAM_B1+0x8000;
	SS_MAP2 = ss_map2	=(Uint16 *)SCL_VDP2_VRAM_B1+0xC000;
	SS_FONT = ss_font		=(Uint16 *)SCL_VDP2_VRAM_B1;
	SS_CACHE= cache		=(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum	 = (SclSpPriNumRegister *)SS_N0PRI;
	ss_SpPriNum	 = (SclSpPriNumRegister *)SS_SPPRI;
	ss_OtherPri	= (SclOtherPriRegister *)SS_OTHR;
	ss_BgColMix	= (SclBgColMixRegister *)SS_BGMIX;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_scl			= (Fixed32 *)SS_SCL;

	nBurnLinescrollSize = 0;
	nBurnSprites = 64+3;

//3 nbg
	SS_SET_N0PRIN(7);
	SS_SET_N1PRIN(4);
	SS_SET_N2PRIN(6);
	SS_SET_S0PRIN(5);

	ss_regs->tvmode = 0x8011;
	initLayers();
	initColors();
	initSprites(256-1,256-1,0,0,0,0);
	ss_reg->n1_move_y =  16 <<16;

	memset((Uint8 *)ss_map  ,0,0x2000);
	memset((Uint8 *)ss_map2,0,0x2000);

	SprSpCmd *ss_spritePtr;
	
	for (unsigned int i = 3; i <nBurnSprites; i++) 
	{
		ss_spritePtr				= &ss_sprite[i];
		if (pbillrdmode)
			ss_spritePtr->control   = ( JUMP_NEXT | FUNC_NORMALSP | DIR_TBREV);
		else
			ss_spritePtr->control   = ( JUMP_NEXT | FUNC_NORMALSP | DIR_LRREV);
		ss_spritePtr->drawMode  = ( ECD_DISABLE | COMPO_REP);	// 16 couleurs
		ss_spritePtr->charSize  = 0x210;  //0x100 16*16
	}
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
	drawWindow(0,240,240,4,68);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ INT32 DrvExit()
{
	CZetExit2();
	ppi8255_exit();

	CZ80Context = MemEnd = AllRam = RamEnd = DrvRAM = DrvMainROM = DrvMainROMdec = DrvSndROM = NULL;
	DrvVidRAM = DrvSprRAM = DrvColRAM = DrvColPROM = NULL;
	MC8123Key = NULL;
	map_offset_lut = NULL;
	DrawSprite = NULL;
	free (AllMem);
	AllMem = NULL;

	ff_data = 0;
	nmi_enable = 0;
	countrunbmode = 0;
	pbillrdmode = 0;
	use_encrypted = 0;
	DrvZ80Bank0 = 0;
	DrvReset = 0;
	romaddr = 0;
	coin = 0;
	spinner = 0;
	sprite_number = 0;
	nSoundBufferPos = 0;

	DrvDial1 = DrvDial2 = 0; 
	return 0;
}

/*static*/ INT32 DrvFrame()
{
	sprite_number = 3;
	DrvInputs[0] = 0xff; // Active LOW
	DrvInputs[1] = 0xff;

	for (UINT32 i = 0; i < 8; i++) {
		DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
		DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
	}

	{ // spinner calculation stuff.
		if (DrvJoy1[2]) DrvDial1 -= 0x04;
		if (DrvJoy1[3]) DrvDial1 += 0x04;
		if (DrvDial1 >= 0x100) DrvDial1 = 0;
		if (DrvDial1 < 0) DrvDial1 = 0xfc;

		if (DrvJoy2[2]) DrvDial2 -= 0x04;
		if (DrvJoy2[3]) DrvDial2 += 0x04;
		if (DrvDial2 >= 0x100) DrvDial2 = 0;
		if (DrvDial2 < 0) DrvDial2 = 0xfc;
	}

	UINT32 nCyclesTotal = nCyclesTotal = (countrunbmode) ? 6000000 / 60 : 3072000 / 60;

	CZetOpen(0);
	for (UINT32 i = 0; i < nInterleave; i++) {
		CZetRun(nCyclesTotal / nInterleave);

		if (i % 128 == 127)
			CZetSetIRQLine(0, CZET_IRQSTATUS_AUTO); // audio irq


		if (i == (nInterleave - 1) && nmi_enable) { // vblank
			CZetNmi();
		}
	}
	CZetClose();

	signed short *nSoundBuffer = (signed short *)(0x25a20000+nSoundBufferPos*(sizeof(signed short)));

		SN76496Update(0, nSoundBuffer, nBurnSoundLen);
		SN76496Update(1, nSoundBuffer, nBurnSoundLen);
		SN76496Update(2, nSoundBuffer, nBurnSoundLen);
		SN76496Update(3, nSoundBuffer, nBurnSoundLen);

	nSoundBufferPos+=(nBurnSoundLen); // DOIT etre deux fois la taille copiee

	if(nSoundBufferPos>=0x3C00)
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos);
		PCM_Task(pcm); // bon emplacement
		nSoundBufferPos=0;
	}

		DrvDraw();

	return 0;
}

/*static*/ INT32 pbillrdInit()
{
	pbillrdmode = 1;

	return DrvInit();
}
