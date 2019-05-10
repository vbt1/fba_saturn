#include "d_zaxxon.h"
//#define RAZE 1
#define draw_background(x) draw_background_test2()
//#define draw_background(x) draw_background_not_rotated(x)

int ovlInit(char *szShortName)
{
	cleanBSS();

/*	struct BurnDriver nBurnDrvCongo = {
		"congo", "zaxxon",
		"Congo Bongo\0",
		congoRomInfo, congoRomName, CongoBongoInputInfo, ZaxxonDIPInfo,
		CongoInit, DrvExit, DrvFrame, NULL, 
	};
*/
	struct BurnDriver nBurnDrvZaxxon = {
		"zaxxon", NULL,
		"Zaxxon (set 1)",
		zaxxonRomInfo, zaxxonRomName, ZaxxonInputInfo, ZaxxonDIPInfo,
		DrvInit, DrvExit, DrvFrame, NULL, 
	};
/*	struct BurnDriver nBurnDrvZaxxonb = {
		"zaxxonb", "zaxxon",
		"Jackson\0",
		zaxxonRomInfo, zaxxonRomName, ZaxxonInputInfo, ZaxxonDIPInfo,
		ZaxxonbInit, DrvExit, DrvFrame, NULL, 
	};
*/
struct BurnDriver nBurnDrvSzaxxon = {
		"szaxxon", "zaxxon",
		"Super Zaxxon",
		szaxxonRomInfo, szaxxonRomName, ZaxxonInputInfo, ZaxxonDIPInfo,
		sZaxxonInit, DrvExit, DrvFrame, NULL, 
	};

//    if (strcmp(nBurnDrvCongo.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvCongo,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvZaxxon.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvZaxxon,sizeof(struct BurnDriver));
//    if (strcmp(nBurnDrvZaxxonb.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvZaxxonb,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvSzaxxon.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvSzaxxon,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}

UINT8 __fastcall zaxxon_read8000(UINT16 address)
{
	return DrvVidRAM[address & 0x3ff];
}

UINT8 __fastcall zaxxon_readA000(UINT16 address)
{
	return DrvSprRAM[address & 0xff];
}

UINT8 __fastcall zaxxon_read(UINT16 address)
{
#ifndef RAZE
	if (address >= 0x8000 && address <= 0x9fff) {
		return DrvVidRAM[address & 0x3ff];
	}

	if (address >= 0xa000 && address <= 0xafff) {
		return DrvSprRAM[address & 0xff];
	}
#endif

	if (address >= (0xc000+0x18fc) && address <= (0xc003+0x18fc)) 
	{
		address-= 0x18fc;
	}

	if (address >= (0xc100+0x18fc) && address <= (0xc100+0x18ff)) 
	{
		address-= 0x18ff;
	}

	if (address >= (0xe03c+0x1f00) && address <= (0xe03f+0x1f00)) 
	{
		address-= 0x1f00;
	}
	
	if (address >= 0xe03c && address <= 0xe03f) 
	{
/*  AM_RANGE(0xe03c, 0xe03f) AM_MIRROR(0x1f00) AM_DEVREADWRITE(PPI8255, "ppi8255", ppi8255_r, ppi8255_w) */
//	bprintf (PRINT_NORMAL, _T("ppi8255_r %02x\n"),address);
		return 0;//ppi8255_r(0, address  & 0x03);
	}

	switch (address)
	{
		case 0xc000:
			// sw00
		return DrvInputs[0];

		case 0xc001:
			// sw01 
		return DrvInputs[1];

		case 0xc002:
			// dsw02
		return DrvDips[0];

		case 0xc003:
			// dsw03
		return DrvDips[1];

		case 0xc100:
		{

//			UINT8 ret = 0;




/*
 * IN2 (bits NOT inverted)
 * bit 7 : CREDIT
 * bit 6 : COIN 2
 * bit 5 : COIN 1
 * bit 4 : ?
 * bit 3 : START 2
 * bit 2 : START 1

*/

/*
   Games supported:
        * Zaxxon		    yes
        * Super Zaxxon		yes
        * Future Spy		yes
        * Razmatazz		    no sound+bad controls sega_universal_sound_board_rom ?
        * Ixion			    no
        * Congo Bongo		yes

   To do:
	Need to add Sega USB support.
	Analog inputs for Raxmatazz and Ixion
	Clean up video code
*/
//DrvInputs[2];
			return DrvInputs[2];// sw100
		}
	}

	return 0;
}
/*
void __fastcall zaxxon_sound_write(UINT16 value)
{
	bprintf (PRINT_NORMAL, _T("zaxxon_sound_write %02d\n"), value);

      switch (value)
		{
			case 0x04:
	bprintf (PRINT_NORMAL, _T("BurnSamplePlay 11\n"));

				BurnSampleStop(10);
				if(!BurnSampleGetStatus(11))
					BurnSamplePlay(11);
				break;
			case 0x00:
			case 0x08:
	bprintf (PRINT_NORMAL, _T("BurnSamplePlay 10\n"));
				if(!BurnSampleGetStatus(10))
					BurnSamplePlay(10);
				BurnSampleStop(11);
				break;
			case 0x0c:
	bprintf (PRINT_NORMAL, _T("BurnSampleStop\n"));
				BurnSampleStop(10);
				BurnSampleStop(11);
				break;
		}
}*/

//#if 0
void ZaxxonPPIWriteA(UINT8 data)
{
//	bprintf (PRINT_NORMAL, _T("ZaxxonPPIWriteA %02x\n"),data);
	UINT8 diff = data ^ sound_state[0];
	sound_state[0] = data;
	if(1)
	{
	/* PLAYER SHIP A/B: volume */
//	sample_set_volume(10, 0.5 + 0.157 * (data & 0x03));
//	sample_set_volume(11, 0.5 + 0.157 * (data & 0x03));
//	BurnSampleSetAllRoutes(10, 0.5 + 0.157 * (data & 0x03), BURN_SND_ROUTE_BOTH);
//	BurnSampleSetAllRoutes(11, 0.5 + 0.157 * (data & 0x03), BURN_SND_ROUTE_BOTH);

	/* PLAYER SHIP C: channel 10 */
//	if ((diff & 0x04) && !(data & 0x04)) BurnSamplePlay(10);
//	if ((diff & 0x04) &&  (data & 0x04)) BurnSampleStop(10);

	/* PLAYER SHIP D: channel 11 */
//	if ((diff & 0x08) && !(data & 0x08)) BurnSamplePlay(11);
//	if ((diff & 0x08) &&  (data & 0x08)) BurnSampleStop(11);

	/* HOMING MISSILE: channel 0 */
//	if ((diff & 0x10) && !(data & 0x10)) BurnSamplePlay(0);
//	if ((diff & 0x10) &&  (data & 0x10)) BurnSampleStop(0);

	/* BASE MISSILE: channel 1 */
//	if ((diff & 0x20) && !(data & 0x20)) BurnSamplePlay(1);

	/* LASER: channel 2 */
//	if ((diff & 0x40) && !(data & 0x40)) BurnSamplePlay(2);
//	if ((diff & 0x40) &&  (data & 0x40)) BurnSampleStop(2);

	/* BATTLESHIP: channel 3 */
//	if ((diff & 0x80) && !(data & 0x80)) BurnSamplePlay(3);
//	if ((diff & 0x80) &&  (data & 0x80)) BurnSampleStop(3);
	}
}
//#endif

void ZaxxonPPIWriteB(UINT8 data)
{

//	bprintf (PRINT_NORMAL, _T("ZaxxonPPIWriteB %02x\n"),data);
	if(1)
	{
	UINT8 diff = data ^ sound_state[1];
	sound_state[1] = data;

	/* S-EXP: channel 4 */
//	if ((diff & 0x10) && !(data & 0x10)) BurnSamplePlay(4);

	/* M-EXP: channel 5 */
//	if ((diff & 0x20) && !(data & 0x20) && !BurnSampleGetStatus(5)) BurnSamplePlay(5);

	/* CANNON: channel 6 */
//	if ((diff & 0x80) && !(data & 0x80)) BurnSamplePlay(6);
	}
}

void ZaxxonPPIWriteC(UINT8 data)
{
//	bprintf (PRINT_NORMAL, _T("ZaxxonPPIWriteC %02x\n"),data);

//	if(1)
	{
	UINT8 diff = data ^ sound_state[2];
	sound_state[2] = data;

	/* SHOT: channel 7 */
//	if ((diff & 0x01) && !(data & 0x01)) BurnSamplePlay(7);

	/* ALARM2: channel 8 */
//	if ((diff & 0x04) && !(data & 0x04)) BurnSamplePlay(8);

	/* ALARM3: channel 9 */
//	if ((diff & 0x08) && !(data & 0x08) && !BurnSampleGetStatus(9)) BurnSamplePlay(9);
	}
}

void CongoPPIWriteB(UINT8 data)
{
//	bprintf (PRINT_NORMAL, _T("CongoPPIWriteB %02x\n"),data);
	UINT8 diff = data ^ sound_state[1];
	sound_state[1] = data;

//	if(1)
	{
		/* GORILLA: channel 0 */
//		if ((diff & 0x02) && !(data & 0x02) && !BurnSampleGetStatus(0)) BurnSamplePlay(0);
	}
}

void CongoPPIWriteC(UINT8 data)
{
//	bprintf (PRINT_NORMAL, _T("CongoPPIWriteC %02x\n"),data);
	UINT8 diff = data ^ sound_state[2];
	sound_state[2] = data;

	if(1)
	{
		/* BASS DRUM: channel 1 */
//		if ((diff & 0x01) && !(data & 0x01)) BurnSamplePlay(1);
//		if ((diff & 0x01) &&  (data & 0x01)) BurnSampleStop(1);

		/* CONGA (LOW): channel 2 */
//		if ((diff & 0x02) && !(data & 0x02)) BurnSamplePlay(2);
//		if ((diff & 0x02) &&  (data & 0x02)) BurnSampleStop(2);
	
		/* CONGA (HIGH): channel 3 */
//		if ((diff & 0x04) && !(data & 0x04)) BurnSamplePlay(3);
//		if ((diff & 0x04) &&  (data & 0x04)) BurnSampleStop(3);
	
		/* RIM: channel 4 */
//		if ((diff & 0x08) && !(data & 0x08)) BurnSamplePlay(4);
//		if ((diff & 0x08) &&  (data & 0x08)) BurnSampleStop(4);
	}
}

void __fastcall zaxxon_write8000(UINT16 address, UINT8 data)
{
	// set up mirroring

	// video ram
	address &= 0x3ff;
	if(DrvVidRAM[address] != data)
	{
		DrvVidRAM[address] = data;
		UINT32 colpromoffs = colpromoffs_lut[address];
		UINT32 x = map_lut[address];
		ss_map2[x] = (DrvColPROM[colpromoffs] & 0x0f);
		ss_map2[x+1] = data;
	}
}

void __fastcall zaxxon_writeA000(UINT16 address, UINT8 data)
{
	DrvSprRAM[address & 0xff] = data;
}

/*static*/ void zaxxon_coin_inserted(UINT8 param)
{
	if (zaxxon_coin_last[param] != DrvJoy4[param])
	{
		zaxxon_coin_status[param] = zaxxon_coin_enable[param];
	}
}

/*static*/ UINT8 zaxxon_coin_r(UINT8 param)
{
	return zaxxon_coin_status[param];
}

/*static*/ void zaxxon_coin_enable_w(UINT8 offset, UINT8 data)
{
	zaxxon_coin_enable[offset] = data;
	if (!zaxxon_coin_enable[offset])
		zaxxon_coin_status[offset] = 0;
}

void __fastcall zaxxon_write(UINT16 address, UINT8 data)
{
#ifndef RAZE
	// video ram
	if (address >= 0x8000 && address <= 0x9fff) 
	{
		address &= 0x3ff;
		if(DrvVidRAM[address] != data)
		{
			DrvVidRAM[address] = data;
			UINT32 colpromoffs = colpromoffs_lut[address];
			UINT32 x = map_lut[address];
			ss_map2[x] = (DrvColPROM[colpromoffs] & 0x0f);
			ss_map2[x+1] = data;
		}
		return;
	}

	if (address >= 0xa000 && address <= 0xafff) {
		DrvSprRAM[address & 0xff] = data;
		return;
	}
#endif

	if (address >= 0xff3c && address <= 0xff3f) 
	{
		address-= 0x1f00;
	}
	else
	{
		if ((address & 0xe700) == 0xc000) address &= ~0x18f8;
		if ((address & 0xe000) == 0xe000) address &= ~0x1f00;
	}

	if (address >= 0xe03c && address <= 0xe03f) {
//		bprintf (PRINT_NORMAL, _T("2 zaxxon_sound_write %4.4x %2.2x\n"), address, data);
//		zaxxon_sound_write(address & 0x0c);
//		ppi8255_w(0, address  & 0x03, data);
//bprintf (PRINT_NORMAL, _T("2 ppi8255_w done\n"));
		return;
	}

	switch (address)
	{
		case 0xc000:
		case 0xc001:
		case 0xc002:
			zaxxon_coin_enable_w(address & 3, data & 1);
		return;

		case 0xc003:
		case 0xc004:
			// coin counter_w
		return;

		case 0xc006:
			zaxxon_flipscreen = ~data & 1;
		return;

		case 0xe03c:
		case 0xe03d:
		case 0xe03e:
		case 0xe03f:
//			ppi8255_w(0, address  & 0x03, data);
		return;

		case 0xe0f0:
			interrupt_enable = data & 1;
#ifndef RAZE
			if (~data & 1) 
			{
				CZetOpen(0);
				CZetLowerIrq();
//				CZetRun(1);
				CZetClose();
			}
#else
			if (~data & 1) 
		{
	z80_lower_IRQ();
//	z80_emulate(0);
		}
#endif
		return;

		case 0xe0f1:
			*zaxxon_fg_color = (data & 1) << 7;
		return;

		case 0xe0f8:
			zaxxon_bg_scroll &= 0xf00;
			zaxxon_bg_scroll |= data;
			zaxxon_bg_scroll_x2 = ((zaxxon_bg_scroll <<1)^ 0xfff) + 1;
	return;

		case 0xe0f9:
			zaxxon_bg_scroll &= 0x0ff;
			zaxxon_bg_scroll |= (data & 0x07) << 8;
			zaxxon_bg_scroll_x2 = ((zaxxon_bg_scroll <<1)^ 0xfff) + 1;
		return; 
			
		case 0xe0fa:
//			*zaxxon_bg_color = data << 7;
			ss_sprite[3].color          = 0x400+data*128;//ou * 256 ?

//			ss_sprite[3].color = data;
		return;

		case 0xe0fb:
			*zaxxon_bg_enable = data & 1;
		return;
	}
}

/*

UINT8 __fastcall congo_read(UINT16 address)
{
	// address mirroring
	if ((address & 0xe008) == 0xc000) address &= ~0x1fc4;
	if ((address & 0xe008) == 0xc008) address &= ~0x1fc7;

	switch (address)
	{
		case 0xc000:
			return DrvInputs[0];        // in0
		case 0xc001:
			return DrvInputs[1];        // in1
		case 0xc002:
			return DrvDips[0]; // dsw02                    // dsw1
		case 0xc003:
			return DrvDips[1]; // dsw03                    // dsw2
		case 0xc008:
			return DrvInputs[2];
	}

	return 0;
}

// CZet.cpp
void CZetRunAdjust(int nCycles);

void congo_sprite_custom_w(int offset, UINT8 data)
{
	congo_custom[offset] = data;

	// seems to trigger on a write of 1 to the 4th byte 
	if (offset == 3 && data == 0x01)
	{
		UINT16 saddr = congo_custom[0] | (congo_custom[1] << 8);
		INT32 count = congo_custom[2];

		// count cycles (just a guess) 
		CZetRunAdjust(-(count * 5));

		// this is just a guess; the chip is hardwired to the spriteram 
		while (count-- >= 0)
		{
			UINT8 daddr = DrvZ80RAM[saddr & 0xfff] * 4;
			DrvSprRAM[(daddr + 0) & 0xff] = DrvZ80RAM[(saddr + 1) & 0xfff];
			DrvSprRAM[(daddr + 1) & 0xff] = DrvZ80RAM[(saddr + 2) & 0xfff];
			DrvSprRAM[(daddr + 2) & 0xff] = DrvZ80RAM[(saddr + 3) & 0xfff];
			DrvSprRAM[(daddr + 3) & 0xff] = DrvZ80RAM[(saddr + 4) & 0xfff];
			saddr += 0x20;
		}
	}
}

void __fastcall congo_write(UINT16 address, UINT8 data)
{
	// address mirroring
	if ((address & 0xe000) == 0xc000) address &= ~0x1fc0;

	switch (address)
	{
		case 0xc018:
		case 0xc019:
		case 0xc01a:
			zaxxon_coin_enable_w(address & 3, data & 1);
		return;

		case 0xc01b:
		case 0xc01c:
			// zaxxon_coin_counter_w
		return;

		case 0xc01d:
			*zaxxon_bg_enable = data & 1;
		return;

		case 0xc01e:
			zaxxon_flipscreen = ~data & 1;
		return;

		case 0xc01f:
			*interrupt_enable = data & 1;
			if (~data & 1) CZetLowerIrq();
		return;

		case 0xc021:
			*zaxxon_fg_color = data << 7;
		return;

		case 0xc023:
			*zaxxon_bg_color = data << 7;
		return;

		case 0xc026:
			*congo_fg_bank = data & 1;
		return;

		case 0xc027:
			*congo_color_bank = data & 1;
		return;

		case 0xc028:
		//case 0xc02a:
			*zaxxon_bg_scroll &= 0xf00;
			*zaxxon_bg_scroll |= data;
		return;

		case 0xc029:
		//case 0xc02b:
			*zaxxon_bg_scroll &= 0x0ff;
			*zaxxon_bg_scroll |= (data & 0x07) << 8;
		return; 
			
		case 0xc030:
		case 0xc031:
		case 0xc032:
		case 0xc033:
			congo_sprite_custom_w(address & 3, data);
		return;

		case 0xc038:
		case 0xc039:
		case 0xc03a:
		case 0xc03b:
		case 0xc03c:
		case 0xc03d:
		case 0xc03e:
		case 0xc03f:
			*soundlatch = data;
		return;
	}
}

void __fastcall congo_sound_write(UINT16 address, UINT8 data)
{
	// address mirroring
	if ((address & 0xe000) == 0x6000) address &= ~0x1fff;
	if ((address & 0xe000) == 0x8000) address &= ~0x1ffc;
	if ((address & 0xe000) == 0xa000) address &= ~0x1fff;

	switch (address)
	{
		case 0x6000:
			SN76496Write(0, data);
		return;

		case 0x8000:
		case 0x8001:
		case 0x8002:
		case 0x8003:
			ppi8255_w(0, address  & 0x03, data);
		return;

		case 0xa000:
			SN76496Write(1, data);
		return;
	}
}

UINT8 __fastcall congo_sound_read(UINT16 address)
{
	// address mirroring
	if ((address & 0xe000) == 0x8000) address &= ~0x1ffc;

	switch (address)
	{
		case 0x8000:
		case 0x8001:
		case 0x8002:
		case 0x8003:
			return ppi8255_r(0, address & 0x03);
	}

	return 0;
}	*/
//-------------------------------------------------------------------------------------------------------------------------------------
void GfxDecode(INT32 num, INT32 numPlanes, INT32 xSize, INT32 ySize, INT32 planeoffsets[], INT32 xoffsets[], INT32 yoffsets[], INT32 modulo, UINT8 *pSrc, UINT8 *pDest)
{
	for (UINT32 c = 0; c < num; c++) {
	
		UINT8 *dp = pDest + (c * xSize * ySize);
		memset(dp, 0, xSize * ySize);
	
		for (UINT32 plane = 0; plane < numPlanes; plane++) {
			INT32 planebit = 1 << (numPlanes - 1 - plane);
			INT32 planeoffs = (c * modulo) + planeoffsets[plane];
		
			for (UINT32 y = 0; y < ySize; y++) {
				INT32 yoffs = planeoffs + yoffsets[y];
				dp = pDest + (c * xSize * ySize) + (y * xSize);
			
				for (UINT32 x = 0; x < xSize; x++) {
					if (readbit(pSrc, yoffs + xoffsets[x])) dp[x] |= planebit;
				}
			}
		}
	}
}

/*static*/int DrvGfxDecode()
{
	UINT32 CharPlane[2] = { 0x4000 * 1, 0x4000 * 0 };
	UINT32 TilePlane[3] = { 0x10000 * 2, 0x10000 * 1, 0x10000 * 0 };
	UINT32 SpritePlane[3] = { 0x20000 * 2, 0x20000 * 1, 0x20000 * 0 };
	UINT32 SpriteXOffs[32] = { 0, 1, 2, 3, 4, 5, 6, 7,
			8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7,
			16*8+0, 16*8+1, 16*8+2, 16*8+3, 16*8+4, 16*8+5, 16*8+6, 16*8+7,
			24*8+0, 24*8+1, 24*8+2, 24*8+3, 24*8+4, 24*8+5, 24*8+6, 24*8+7 };
	UINT32 SpriteYOffs[32] = { 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8,
			32*8, 33*8, 34*8, 35*8, 36*8, 37*8, 38*8, 39*8,
			64*8, 65*8, 66*8, 67*8, 68*8, 69*8, 70*8, 71*8,
			96*8, 97*8, 98*8, 99*8, 100*8, 101*8, 102*8, 103*8 };

	UINT8 *tmp = (UINT8*)0x00200000;//(UINT8*) size (0xc000);

 	UINT8 *ss_vram	= (UINT8 *)SS_SPRAM;

	UINT8 *DrvGfxROM0 = cache;
	UINT8 *DrvGfxROM1	= (UINT8 *)0x25a60000;
	UINT8 *DrvGfxROM2	= &ss_vram[0x1100];//Next; Next += 0x020000;
	UINT8 *DrvGfxROM3	= (UINT8 *)DrvGfxROM1+0x010000;

	memcpy (tmp, DrvGfxROM0, 0x1000);
// foreground (text)
	GfxDecode4Bpp(0x0100, 2,  8,  8, CharPlane,   SpriteXOffs, SpriteYOffs, 0x040, tmp, DrvGfxROM0);
	rotate_tile(0x0100,0,cache);
	memcpy (tmp, DrvGfxROM1, 0x6000);
// background
	GfxDecode(0x0400, 3,  8,  8, TilePlane,   SpriteXOffs, SpriteYOffs, 0x040, tmp, DrvGfxROM1);

	memcpy (tmp, DrvGfxROM2, 0xc000);
// sprites
	GfxDecode4Bpp(0x0080, 3, 32, 32, SpritePlane, SpriteXOffs, SpriteYOffs, 0x400, tmp, DrvGfxROM2);

	rotate32_tile(0x0080,0,DrvGfxROM2);

//	free (tmp);
	tmp=NULL;

	return 0;
}

/*static*/void DrvPaletteInit(INT32 len)
{
	INT32 delta=0;

	for (UINT32 i = 0; i < len; i++)
	{
		INT32 bit0, bit1, bit2, r, g, b;

		bit0 = (DrvColPROM[i] >> 0) & 0x01;
		bit1 = (DrvColPROM[i] >> 1) & 0x01;
		bit2 = (DrvColPROM[i] >> 2) & 0x01;
		r = bit0 * 33 + bit1 * 70 + bit2 * 151;

		bit0 = (DrvColPROM[i] >> 3) & 0x01;
		bit1 = (DrvColPROM[i] >> 4) & 0x01;
		bit2 = (DrvColPROM[i] >> 5) & 0x01;
		g = bit0 * 33 + bit1 * 70 + bit2 * 151;

		bit0 = (DrvColPROM[i] >> 6) & 0x01;
		bit1 = (DrvColPROM[i] >> 7) & 0x01;
		b = bit0 * 78 + bit1 * 168;
		colBgAddr2[i]=colBgAddr[delta] =BurnHighCol(r, g, b, 0);
		delta++; if ((delta & 7) == 0) delta += 8;
	}
	DrvColPROM += 0x100;
}

/*static*/void bg_layer_init()
{
	memset(zaxxon_bg_pixmap,0x01,0x100000);
//	INT32 len = (hardware_type == 2) ? 0x2000 : 0x4000;
	INT32 len = 0x4000;
	INT32 mask = len-1;

	UINT8 *DrvGfxROM1	= (UINT8 *)0x25a60000;
	UINT8 *DrvGfxROM3	= (UINT8 *)DrvGfxROM1+0x010000;

	for (UINT32 offs = 0; offs < 32 * 512; offs++)
	{
		INT32 sx = (offs & 0x1f) << 3;
		INT32 sy = (offs >> 5) << 3;

		INT32 moffs = offs & mask;

		INT32 code = (DrvGfxROM3[moffs]) | ((DrvGfxROM3[moffs | len] & 3) << 8);
		INT32 color = (DrvGfxROM3[moffs | len] & 0xf0) >> 1;

		UINT8 *src = DrvGfxROM1 + (code << 6);

		for (INT32 y = 0; y < 8; y++, sy++) 
		{
			for (INT32 x = 0; x < 8; x++) 
			{
				zaxxon_bg_pixmap[sy * 256 + sx + x] = src[(y << 3) | x] | color;
			}
		}
	}
/* // à retester
		for (INT32 y = 0; y < 8; y++, sy++) 
		{
			for (INT32 x = 0; x < 8; x++) 
			{
				zaxxon_bg_pixmap[sy * 256 + sx + x] = src[(y << 3) | x] | color;
			}
		}
	}
	unsigned char toto[0x4000];
	memcpy(zaxxon_bg_pixmap,zaxxon_bg_pixmap,0x4000);

	for (INT32 offs = 0; offs < 32 * 512; offs+=512)
	{
		memcpy(toto,zaxxon_bg_pixmap,512);
//		zaxxon_bg_pixmap[sy * 256 + sx + x] = src[(y << 3) | x] | color;
		zaxxon_bg_pixmapzaxxon_bg_pixmap[sy * 256 + sx + x];
	}*/
}

/*static*/int DrvDoReset()
{
	memset (AllRam, 0, RamEnd - AllRam);

#ifndef RAZE
	CZetOpen(0);
	CZetReset();
	CZetClose();
#else
	z80_reset();
#endif
	//
	/*
	if (hardware_type == 2) {
		CZetOpen(1);
		CZetReset();
		CZetClose();
	}
	   */
	return 0;
}

/*static*/INT32 DrvInit()
{
	DrvInitSaturn();
//	memset(AllMem, 0, MALLOC_MAX);
	memset(bitmap,	 0x00,0xE000);
 	UINT8 *ss_vram	= (UINT8 *)SS_SPRAM;

	UINT8 *DrvGfxROM0 = cache;
	UINT8 *DrvGfxROM1	= (UINT8 *)0x25a60000;
	UINT8 *DrvGfxROM2	= &ss_vram[0x1100];//Next; Next += 0x020000;
	UINT8 *DrvGfxROM3	= (UINT8 *)DrvGfxROM1+0x010000;

	memset(DrvGfxROM2+0x00010000,0x00,0xE000);
	{
		for (int i = 0; i < 3; i++) {
			if (BurnLoadRom(DrvZ80ROM  + i * 0x2000,  0 + i, 1)) return 1;
			if (BurnLoadRom(DrvGfxROM1 + i * 0x2000,  5 + i, 1)) return 1;
			if (BurnLoadRom(DrvGfxROM2 + i * 0x4000,  8 + i, 1)) return 1;
			if (BurnLoadRom(DrvGfxROM3 + i * 0x2000, 11 + i, 1)) return 1;
		}

		for (int i = 0; i < 2; i++) {
			if (BurnLoadRom(DrvGfxROM0 + i * 0x0800,  3 + i, 1)) return 1;
			if (BurnLoadRom(DrvColPROM + i * 0x0100, 15 + i, 1)) return 1;
		}

		if (BurnLoadRom(DrvGfxROM3 + 0x6000,  14, 1)) return 1;

		DrvGfxDecode();

		DrvPaletteInit(0x100);
//		DrvPaletteInit(0x200);

		bg_layer_init();

//		DrvGfxROM3 = NULL;
//		DrvGfxROM1 = NULL;

	}
#ifndef RAZE
	CZetInit2(1,CZ80Context);
	CZetOpen(0);
	CZetMapArea(0x0000, 0x5fff, 0, DrvZ80ROM);
	CZetMapArea(0x0000, 0x5fff, 2, DrvZ80ROM);
	CZetMapArea(0x6000, 0x6fff, 0, DrvZ80RAM);
	CZetMapArea(0x6000, 0x6fff, 1, DrvZ80RAM);
	CZetMapArea(0x6000, 0x6fff, 2, DrvZ80RAM); // read 0 // write 1 // fetch 2
	CZetMapArea(0x8000, 0x83ff, 0, DrvVidRAM);
//	CZetMapArea(0x8000, 0x83ff, 1, DrvVidRAM);
	CZetMapArea(0x8000, 0x83ff, 2, DrvVidRAM);
	CZetMapArea(0xa000, 0xa0ff, 0, DrvSprRAM);
	CZetMapArea(0xa000, 0xa0ff, 1, DrvSprRAM);
	CZetSetWriteHandler(zaxxon_write);
	CZetSetReadHandler(zaxxon_read);
	//ZetMemEnd();
	CZetClose();
#else
  	z80_init_memmap();

	z80_map_fetch(0x0000, 0x5fff, DrvZ80ROM);
	z80_map_read(0x0000, 0x5fff, DrvZ80ROM);

	z80_map_fetch(0x6000, 0x6fff, DrvZ80RAM);
	z80_map_read(0x6000, 0x6fff, DrvZ80RAM);
	z80_map_write(0x6000, 0x6fff, DrvZ80RAM);
	z80_map_fetch(0x8000, 0x83ff, DrvVidRAM);
	z80_map_read(0x8000, 0x83ff, DrvVidRAM);
	z80_map_fetch(0xa000, 0xa0ff, DrvSprRAM);
	z80_map_read(0xa000, 0xa0ff, DrvSprRAM);

	z80_add_write(0x8000, 0x9fff, 1, (void *)&zaxxon_write8000);
	z80_add_write(0xA000, 0xAfff, 1, (void *)&zaxxon_writeA000);
	z80_add_write(0xB000, 0xffff, 1, (void *)&zaxxon_write);
	z80_add_read(0x8000, 0x9fff, 1, (void *)&zaxxon_read8000);
	z80_add_read(0xA000, 0xAfff, 1, (void *)&zaxxon_readA000);
	z80_add_read(0xB000,  0xffff, 1, (void *)&zaxxon_read);
	
	z80_end_memmap();   

#endif


	//CZetMemEnd();

//	SN76489Init(0, 4000000 / 1, 0);
//	SN76489Init(1, 4000000 / 4, 1);
	DrvDoReset();
	return 0;
}

/*static*/INT32 DrvExit()
{
	SPR_InitSlaveSH();

	nBurnLinescrollSize = 1;
	nSoundBufferPos = 0;
#ifndef RAZE
	CZetExit2();
#else
	z80_stop_emulating();
#endif
	cleanSprites();
	CZ80Context = MemEnd = AllRam = RamEnd = DrvZ80ROM = DrvZ80DecROM = DrvZ80ROM2 = NULL;
	DrvColPROM = DrvZ80RAM = DrvZ80RAM2 = DrvSprRAM = DrvVidRAM = DrvColRAM = NULL;
	zaxxon_bg_pixmap = 	zaxxon_fg_color = zaxxon_bg_color= NULL;
	zaxxon_bg_enable = NULL;
/*	zaxxon_flipscreen =*/ zaxxon_coin_enable = /*soundlatch =*/ NULL;
	zaxxon_coin_status = zaxxon_coin_last = sound_state = NULL;
	zaxxon_bg_scroll = 0;
	zaxxon_flipscreen = 0;
	interrupt_enable = 0;
	zaxxon_bg_scroll_x2 = 0;	
	AllMem = NULL;

	ss_map264 = NULL;
	bitmap = NULL;
	map_lut = NULL;
	colpromoffs_lut = NULL;
	sx_lut = NULL;
	sy_lut = NULL;
	charaddr_lut = NULL;

	for (int i = 0; i < 240; i++) {
		srcxmask[i] = NULL;
	}
	srcx_buffer = NULL;

	free(SaturnMem);
	SaturnMem = NULL;

	return 0;
}

// ajouter srcxmask en param
/*static*/void draw_background_test2_no_color(/*UINT32 *srcmask,*/ UINT8 *ss_map264,UINT8 *zaxxon_bg_pixmap,unsigned int yoffset)
{
	/* loop over visible rows */
	for (unsigned int y = 0; y < 224; ++y)
	{
//		UINT8 *src = zaxxon_bg_pixmap + (((y + yoffset) & 4095) << 8);
		UINT8 *src = zaxxon_bg_pixmap + (((y + yoffset) & 4095) << 8);
		UINT32 *srcptr = (UINT32 *)srcxmask[y];
		memcpy(&ss_map264[y*240],&src[*srcptr],216);
	}
}

// martinman : Store u32 instead of u8
/*
vbt> thanks for help & good night =)
<MartinMan> If you can get rid of the overhead by
<MartinMan> making custom code for each of the 4 rotations, then I
<MartinMan> would do that. Overhead = the indirections.
<vbt> what's indirection
<MartinMan> Yeah, timemachinetime here, too
<MartinMan> pixel[a[x]]
<MartinMan> instead of pixel[x]
*/

INT32 find_minimum_y(UINT8 value)
{
	INT32 flipmask = 0x00;//*zaxxon_flipscreen ? 0xff : 0x00;
	INT32 flipconst = 0xf1;//*zaxxon_flipscreen ? 0xef : 0xf1;
	INT32 y;

	/* first find a 16-pixel bucket where we hit */
	for (y = 0; y < 256; y += 16)
	{
		INT32 sum = (value + flipconst + 1) + (y ^ flipmask);
		if ((sum & 0xe0) == 0xe0)
			break;
	}

	/* then scan backwards until we no longer match */
	while (1)
	{
		INT32 sum = (value + flipconst + 1) + ((y - 1) ^ flipmask);
		if ((sum & 0xe0) != 0xe0)
			break;
		y--;
	}

	/* add one line since we draw sprites on the previous line */
	return (y + 1) & 0xff;
}


int find_minimum_x(UINT8 value)
{
	INT32 flipmask = zaxxon_flipscreen ? 0xff : 0x00;
	INT32 x;

	/* the sum of the X position plus a constant specifies the address within */
	/* the line bufer; if we're flipped, we will write backwards */
	x = (value + 0xef + 1) ^ flipmask;
	if (flipmask)
		x -= 31;
	return x & 0xff;
}

/*static*/void draw_sprites()
{
//	int flipmask = *zaxxon_flipscreen ? 0xff : 0x00;
//#define flipmask 0xff
#define flipxmask 0x140
#define flipymask 0x180
#define flipmaskxy flipxmask|flipymask

/* only the lower half of sprite RAM is read during rendering */
	unsigned int j=4;

	for (int offs = 0x7c; offs >= 0; offs -= 4)
	{
		ss_sprite[j].ax			= sy_lut[DrvSprRAM[offs]];
		if(ss_sprite[j].ax<224)
		{
			int flip = (DrvSprRAM[offs + 1] ^ 0x00) & flipmaskxy;
			ss_sprite[j].ay			= sx_lut[DrvSprRAM[offs + 3]];
			ss_sprite[j].color		= (DrvSprRAM[offs + 2] & 0x1f)<<4;
			ss_sprite[j].charAddr	= charaddr_lut[DrvSprRAM[offs + 1]];
		}
		else
		{
			ss_sprite[j].ax			= 256;
			ss_sprite[j].ay			= 256;
			ss_sprite[j].charAddr	= 0;		
		}
		++j;
	}
	ss_sprite[j].control = CTRL_END;
	ss_sprite[j].drawMode = 0;
	ss_sprite[j].charAddr	= 0;
	ss_sprite[j].charSize		= 0;
	ss_sprite[j].ax	= 0;
	ss_sprite[j].ay	= 0;
}

/*static*/int DrvDraw()
{
	*(UINT16 *)0x25E00000=RGB( 0, 0, 0 );
		SPR_RunSlaveSH((PARA_RTN*)draw_sprites, NULL);
	if (*zaxxon_bg_enable)
	{
		draw_background_test2_no_color(/*(UINT32 *)srcxmask,*/ss_map264,zaxxon_bg_pixmap,zaxxon_bg_scroll_x2);
	}
	else
	{
		memset4_fast(bitmap+0xE80,0x10101010,0xCA00);
	}
//	draw_sprites(); //0x140, 0x180);
	copyBitmap();

	SPR_WaitEndSlaveSH();
	return 0;
}

/*static*/ void zaxxon_coin_lockout()
{
	// soft-coin lockout - prevents 30 coins per 1 insert-coin keypress.
	if (DrvJoy4[0]) // a coin inserted
		zaxxon_coin_inserted(0);
	if (DrvJoy4[1]) // b coin inserted
		zaxxon_coin_inserted(1);
	if (DrvJoy4[2]) // service pressed
		zaxxon_coin_inserted(2);
	DrvInputs[2] += (zaxxon_coin_r(0)) ? 0x20 : 0;
	DrvInputs[2] += (zaxxon_coin_r(1)) ? 0x40 : 0;
	DrvInputs[2] += (zaxxon_coin_r(2)) ? 0x80 : 0;
	zaxxon_coin_last[0] = DrvJoy4[0];
	zaxxon_coin_last[1] = DrvJoy4[1];
	zaxxon_coin_last[2] = DrvJoy4[2];
	// end soft-coin lockout
}

/*static*/ int DrvFrame()
{
	{
		DrvInputs[0] = 0x00;
		DrvInputs[1] = 0x00;
		DrvInputs[2] = 0x00;
		for (INT32 i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
			// DrvInputs[2] ^= (DrvJoy3[i] & 1) << i;
		}
		DrvInputs[2] ^= (DrvJoy3[2] & 1) << 2;   //  start 1
		DrvInputs[2] ^= (DrvJoy3[3] & 1) << 3;   //  start 2
		zaxxon_coin_lockout();
	}

#ifndef RAZE
	CZetOpen(0);
	CZetRun(38016);//(3041250 / 60);
	if (interrupt_enable) 
 	{
		CZetRaiseIrq(0);
//		CZetRun(1);
	}
	CZetClose();
#else
	z80_emulate(3041250 / 60);
	if (interrupt_enable) 
	{	
		z80_raise_IRQ(0);
//		z80_emulate(0);
	}
#endif

	DrvDraw();
	return 0;
}

/*static*/void zaxxonb_decode()
{
	/*static*/const UINT8 data_xortable[2][8] =
	{
		{ 0x0a,0x0a,0x22,0x22,0xaa,0xaa,0x82,0x82 },	/* ...............0 */
		{ 0xa0,0xaa,0x28,0x22,0xa0,0xaa,0x28,0x22 },	/* ...............1 */
	};

	/*static*/const UINT8 opcode_xortable[8][8] =
	{
		{ 0x8a,0x8a,0x02,0x02,0x8a,0x8a,0x02,0x02 },	/* .......0...0...0 */
		{ 0x80,0x80,0x08,0x08,0xa8,0xa8,0x20,0x20 },	/* .......0...0...1 */
		{ 0x8a,0x8a,0x02,0x02,0x8a,0x8a,0x02,0x02 },	/* .......0...1...0 */
		{ 0x02,0x08,0x2a,0x20,0x20,0x2a,0x08,0x02 },	/* .......0...1...1 */
		{ 0x88,0x0a,0x88,0x0a,0xaa,0x28,0xaa,0x28 },	/* .......1...0...0 */
		{ 0x80,0x80,0x08,0x08,0xa8,0xa8,0x20,0x20 },	/* .......1...0...1 */
		{ 0x88,0x0a,0x88,0x0a,0xaa,0x28,0xaa,0x28 },	/* .......1...1...0 */
		{ 0x02,0x08,0x2a,0x20,0x20,0x2a,0x08,0x02 } 	/* .......1...1...1 */
	};

	INT32 A;
	UINT8 *rom = DrvZ80ROM;
	INT32 size = 0x6000;
	UINT8 *decrypt = DrvZ80DecROM;
#ifndef RAZE	 
	CZetOpen(0);
	CZetMapArea2(0x0000, 0x5fff, 2, DrvZ80DecROM, DrvZ80ROM );
	CZetClose();
#endif

	for (A = 0x0000; A < size; A++)
	{
		INT32 i,j;
		UINT8 src;

		src = rom[A];

		/* pick the translation table from bit 0 of the address */
		i = A & 1;

		/* pick the offset in the table from bits 1, 3 and 5 of the source data */
		j = ((src >> 1) & 1) + (((src >> 3) & 1) << 1) + (((src >> 5) & 1) << 2);
		/* the bottom half of the translation table is the mirror image of the top */
		if (src & 0x80) j = 7 - j;

		/* decode the ROM data */
		rom[A] = src ^ data_xortable[i][j];

		/* now decode the opcodes */
		/* pick the translation table from bits 0, 4, and 8 of the address */
		i = ((A >> 0) & 1) + (((A >> 4) & 1) << 1) + (((A >> 8) & 1) << 2);
		decrypt[A] = src ^ opcode_xortable[i][j];
	}
}

/*static*/int ZaxxonbInit()
{
	INT32 nRet = DrvInit();

//	if (nRet == 0) {
		zaxxonb_decode();
//	}

	return nRet;
}

/*static*/void sega_decode(const UINT8 convtable[32][4])
{
	int A;

	int length = 0x6000;
	int cryptlen = length;
	UINT8 *rom = DrvZ80ROM;
	UINT8 *decrypted = DrvZ80DecROM;
#ifndef RAZE	
	CZetOpen(0);
	CZetMapArea2(0x0000, 0x5fff, 2, DrvZ80DecROM, DrvZ80ROM);
	CZetClose();
#endif	
	for (A = 0x0000;A < cryptlen;A++)
	{
		INT32 xorval = 0;
		UINT8 src = rom[A];
		/* pick the translation table from bits 0, 4, 8 and 12 of the address */
		INT32 row = (A & 1) + (((A >> 4) & 1) << 1) + (((A >> 8) & 1) << 2) + (((A >> 12) & 1) << 3);
		/* pick the offset in the table from bits 3 and 5 of the source data */
		INT32 col = ((src >> 3) & 1) + (((src >> 5) & 1) << 1);
		/* the bottom half of the translation table is the mirror image of the top */
		if (src & 0x80)
		{
			col = 3 - col;
			xorval = 0xa8;
		}

		/* decode the opcodes */
		decrypted[A] = (src & ~0xa8) | (convtable[2*row][col] ^ xorval);

		/* decode the data */
		rom[A] = (src & ~0xa8) | (convtable[2*row+1][col] ^ xorval);

		if (convtable[2*row][col] == 0xff)	/* table incomplete! (for development) */
			decrypted[A] = 0xee;
		if (convtable[2*row+1][col] == 0xff)	/* table incomplete! (for development) */
			rom[A] = 0xee;
	}
}

void szaxxon_decode()
{
	/*static*/const UINT8 convtable[32][4] =
	{
		/*       opcode                   data                     address      */
		/*  A    B    C    D         A    B    C    D                           */
		{ 0x88,0xa8,0x80,0xa0 }, { 0x28,0x20,0xa8,0xa0 },	/* ...0...0...0...0 */
		{ 0x08,0x28,0x88,0xa8 }, { 0x88,0x80,0x08,0x00 },	/* ...0...0...0...1 */
		{ 0xa8,0x28,0xa0,0x20 }, { 0x20,0xa0,0x00,0x80 },	/* ...0...0...1...0 */
		{ 0x88,0xa8,0x80,0xa0 }, { 0x28,0x20,0xa8,0xa0 },	/* ...0...0...1...1 */
		{ 0x08,0x28,0x88,0xa8 }, { 0x88,0x80,0x08,0x00 },	/* ...0...1...0...0 */
		{ 0x88,0xa8,0x80,0xa0 }, { 0x28,0x20,0xa8,0xa0 },	/* ...0...1...0...1 */
		{ 0xa8,0x28,0xa0,0x20 }, { 0x20,0xa0,0x00,0x80 },	/* ...0...1...1...0 */
		{ 0x08,0x28,0x88,0xa8 }, { 0x88,0x80,0x08,0x00 },	/* ...0...1...1...1 */
		{ 0x08,0x28,0x88,0xa8 }, { 0x88,0x80,0x08,0x00 },	/* ...1...0...0...0 */
		{ 0x88,0xa8,0x80,0xa0 }, { 0x28,0x20,0xa8,0xa0 },	/* ...1...0...0...1 */
		{ 0x88,0xa8,0x80,0xa0 }, { 0x28,0x20,0xa8,0xa0 },	/* ...1...0...1...0 */
		{ 0xa8,0x28,0xa0,0x20 }, { 0x20,0xa0,0x00,0x80 },	/* ...1...0...1...1 */
		{ 0xa8,0x28,0xa0,0x20 }, { 0x20,0xa0,0x00,0x80 },	/* ...1...1...0...0 */
		{ 0xa8,0x28,0xa0,0x20 }, { 0x20,0xa0,0x00,0x80 },	/* ...1...1...0...1 */
		{ 0x08,0x28,0x88,0xa8 }, { 0x88,0x80,0x08,0x00 },	/* ...1...1...1...0 */
		{ 0x88,0xa8,0x80,0xa0 }, { 0x28,0x20,0xa8,0xa0 }	/* ...1...1...1...1 */
	};

	sega_decode(convtable);
}

/*static*/int sZaxxonInit()
{
	INT32 nRet = DrvInit();

	if (nRet == 0) {
		szaxxon_decode();
	}

	return nRet;
}
#if 0
void futspy_decode()
{
	/*static*/const UINT8 convtable[32][4] =
	{
		/*       opcode                   data                     address      */
		/*  A    B    C    D         A    B    C    D                           */
		{ 0x28,0x08,0x20,0x00 }, { 0x28,0x08,0x20,0x00 },	/* ...0...0...0...0 */
		{ 0x80,0x00,0xa0,0x20 }, { 0x08,0x88,0x00,0x80 },	/* ...0...0...0...1 */
		{ 0x80,0x00,0xa0,0x20 }, { 0x08,0x88,0x00,0x80 },	/* ...0...0...1...0 */
		{ 0xa0,0x80,0x20,0x00 }, { 0x20,0x28,0xa0,0xa8 },	/* ...0...0...1...1 */
		{ 0x28,0x08,0x20,0x00 }, { 0x88,0x80,0xa8,0xa0 },	/* ...0...1...0...0 */
		{ 0x80,0x00,0xa0,0x20 }, { 0x08,0x88,0x00,0x80 },	/* ...0...1...0...1 */
		{ 0x80,0x00,0xa0,0x20 }, { 0x20,0x28,0xa0,0xa8 },	/* ...0...1...1...0 */
		{ 0x20,0x28,0xa0,0xa8 }, { 0x08,0x88,0x00,0x80 },	/* ...0...1...1...1 */
		{ 0x88,0x80,0xa8,0xa0 }, { 0x28,0x08,0x20,0x00 },	/* ...1...0...0...0 */
		{ 0x80,0x00,0xa0,0x20 }, { 0xa0,0x80,0x20,0x00 },	/* ...1...0...0...1 */
		{ 0x20,0x28,0xa0,0xa8 }, { 0x08,0x88,0x00,0x80 },	/* ...1...0...1...0 */
		{ 0x80,0x00,0xa0,0x20 }, { 0x20,0x28,0xa0,0xa8 },	/* ...1...0...1...1 */
		{ 0x88,0x80,0xa8,0xa0 }, { 0x88,0x80,0xa8,0xa0 },	/* ...1...1...0...0 */
		{ 0x80,0x00,0xa0,0x20 }, { 0x08,0x88,0x00,0x80 },	/* ...1...1...0...1 */
		{ 0x80,0x00,0xa0,0x20 }, { 0x28,0x08,0x20,0x00 },	/* ...1...1...1...0 */
		{ 0x20,0x28,0xa0,0xa8 }, { 0xa0,0x80,0x20,0x00 }	/* ...1...1...1...1 */
	};


	sega_decode(convtable);
}


/*static*/int futspyInit()
{
	futspy_sprite = 1;

	INT32 nRet = DrvInit();

	if (nRet == 0) {
		futspy_decode();
	}

	return nRet;
}

void nprinces_decode()
{
	/*static*/const UINT8 convtable[32][4] =
	{
		/*       opcode                   data                     address      */
		/*  A    B    C    D         A    B    C    D                           */
		{ 0x08,0x88,0x00,0x80 }, { 0xa0,0x20,0x80,0x00 },	/* ...0...0...0...0 */
		{ 0xa8,0xa0,0x28,0x20 }, { 0x88,0xa8,0x80,0xa0 },	/* ...0...0...0...1 */
		{ 0x88,0x80,0x08,0x00 }, { 0x28,0x08,0xa8,0x88 },	/* ...0...0...1...0 */
		{ 0x88,0xa8,0x80,0xa0 }, { 0x28,0x08,0xa8,0x88 },	/* ...0...0...1...1 */
		{ 0x88,0xa8,0x80,0xa0 }, { 0xa0,0x20,0x80,0x00 },	/* ...0...1...0...0 */
		{ 0xa8,0xa0,0x28,0x20 }, { 0xa8,0xa0,0x28,0x20 },	/* ...0...1...0...1 */
		{ 0x88,0x80,0x08,0x00 }, { 0x88,0xa8,0x80,0xa0 },	/* ...0...1...1...0 */
		{ 0x88,0xa8,0x80,0xa0 }, { 0x88,0xa8,0x80,0xa0 },	/* ...0...1...1...1 */
		{ 0xa0,0x20,0x80,0x00 }, { 0xa0,0x20,0x80,0x00 },	/* ...1...0...0...0 */
		{ 0x08,0x88,0x00,0x80 }, { 0x28,0x08,0xa8,0x88 },	/* ...1...0...0...1 */
		{ 0x88,0xa8,0x80,0xa0 }, { 0x88,0x80,0x08,0x00 },	/* ...1...0...1...0 */
		{ 0x88,0xa8,0x80,0xa0 }, { 0x28,0x08,0xa8,0x88 },	/* ...1...0...1...1 */
		{ 0x88,0xa8,0x80,0xa0 }, { 0x88,0xa8,0x80,0xa0 },	/* ...1...1...0...0 */
		{ 0x88,0xa8,0x80,0xa0 }, { 0x88,0xa8,0x80,0xa0 },	/* ...1...1...0...1 */
		{ 0x88,0x80,0x08,0x00 }, { 0x88,0x80,0x08,0x00 },	/* ...1...1...1...0 */
		{ 0x08,0x88,0x00,0x80 }, { 0x28,0x08,0xa8,0x88 }	/* ...1...1...1...1 */
	};


	sega_decode(convtable);
}

/*static*/int razmatazInit()
{
	hardware_type = 1;

	INT32 nRet = DrvInit();

	if (nRet == 0) {
		nprinces_decode();
	}

	return nRet;
}

/*static*/int ixionInit()
{
	int nRet;

	hardware_type = 1;

	nRet = DrvInit();

	if (nRet == 0) {
		szaxxon_decode();
	}

	return DrvInit();
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
// el piratero : utilise dma_scu et retester !
void copyBitmap()
{
//	memcpyl(ss_map+264,bitmap,0x10000);
//	memcpyl(DrvGfxROM2+0x00010000,bitmap,0x10000);
//	DMA_CpuMemCopy1(ss_map+264,bitmap,0x1C000);
//	DMA_ScuMemCopy(DrvGfxROM2+0x00010000,bitmap+0xE80,0xD240);

 	UINT8 *ss_vram	= (UINT8 *)SS_SPRAM;
	UINT8 *DrvGfxROM2	= &ss_vram[0x1100];//Next; Next += 0x020000;

	DMA_ScuMemCopy(DrvGfxROM2+0x00010000,bitmap+0xE80,0xCA00);
	while(DMA_ScuResult()==2);
//	memcpyl(DrvGfxROM2+0x00010000,bitmap+0xE80,0xCA00);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initLayers()
{
    Uint16	CycleTb[]={
		0x1f56, 0xff26, //A0
		0xffff, 0xffff,	//A1
		0xf5f2,0x4eff,   //B0
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
//	scfg.plate_addr[0] = (Uint32)SS_MAP2;
	scfg.plate_addr[0] = (Uint32)SS_MAP2;
	scfg.plate_addr[1] = 0x00;
//	SCL_SetConfig(SCL_NBG1, &scfg);
	SCL_SetConfig(SCL_NBG2, &scfg);
// 3 nbg

	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
	scfg.plate_addr[0] = (Uint32)SS_FONT;

// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);
	scfg.dispenbl      = OFF;
//	scfg.coltype       = SCL_COL_TYPE_256;//SCL_COL_TYPE_256;
//	scfg.plate_addr[0] = (Uint32)SS_MAP;
//	scfg.plate_addr[0] = (Uint32)SS_MAP;
	SCL_SetConfig(SCL_NBG1, &scfg);
//	SCL_SetConfig(SCL_NBG2, &scfg);
	
	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initPosition()
{
	SCL_Open();
	ss_reg->n1_move_y =  16 <<16;
	ss_reg->n2_move_y =  16;//(0<<16) ;
	ss_reg->n2_move_x =  8;//(0<<16) ;
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initColors()
{
	/*unsigned short palette[8]=
{
RGB( 164>>3, 181>>3, 197>>3),RGB( 0,0,0 ),RGB( 164>>3, 181>>3, 197>>3),RGB( 214>>3, 230>>3, 247>>3 ),
RGB( 0, 0, 0 ),RGB( 0,0,0 ),RGB( 164>>3, 247>>3, 197>>3 ),RGB( 99>>3, 197>>3, 148>>3 ),
};*/
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
//	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);	  //ON
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);	  //ON
	(Uint16*)SCL_AllocColRam(SCL_NBG3,ON);
	(Uint16*)SCL_AllocColRam(SCL_NBG3,ON);
	(Uint16*)SCL_AllocColRam(SCL_NBG3,ON);
//	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);//OFF);
	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);//OFF);
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void SaturnInitMem()
{
	UINT8 *Next; Next = (UINT8 *)SaturnMem;
//	bitmap				= Next; Next += 0x10000;
	bitmap				= Next; Next += 0xE000;
	map_lut	 			= Next; Next += 0x400*sizeof(UINT32);
	colpromoffs_lut	= Next; Next += 0x400*sizeof(UINT32);
	sx_lut				= Next; Next += 256*sizeof(INT16);
	sy_lut				= Next; Next += 256*sizeof(INT16);
	charaddr_lut		= Next; Next += 256*sizeof(UINT16);
	srcx_buffer		= Next; Next += 256*240*sizeof(UINT32);
	CZ80Context		= Next; Next += sizeof(cz80_struc);


//--------------
	AllMem					= Next;
	DrvZ80ROM			= Next; Next += 0x010000;
	DrvZ80DecROM	= Next; Next += 0x010000;
	DrvZ80ROM2		= Next; Next += 0x010000;

//	DrvGfxROM0		= cache;

//	UINT8 *ss_vram	= (UINT8 *)SS_SPRAM;
//	DrvGfxROM2		= &ss_vram[0x1100];//Next; Next += 0x020000;

//	DrvGfxROM1		= Next; Next += 0x010000;
//	DrvGfxROM3		= Next; Next += 0x010000;

	DrvColPROM		= Next; Next += 0x000200;

	zaxxon_bg_pixmap = (UINT8*)0x00200000;

	AllRam			= Next;

	DrvZ80RAM		= Next; Next += 0x001000;
	DrvZ80RAM2		= Next; Next += 0x001000;
	DrvSprRAM		= Next; Next += 0x000100;
	DrvVidRAM		= Next; Next += 0x000400;
	DrvColRAM		= Next; Next += 0x000400;

//	interrupt_enable	= Next; Next += 0x000001;

	zaxxon_fg_color		= Next; Next += 0x000001;
	zaxxon_bg_color		= Next; Next += 0x000001;
	zaxxon_bg_enable	= Next; Next += 0x000001;
//	congo_color_bank	= Next; Next += 0x000001;
//	congo_fg_bank		= Next; Next += 0x000001;
//	congo_custom		= Next; Next += 0x000004;
//	zaxxon_flipscreen	= Next; Next += 0x000001;
	zaxxon_coin_enable	= Next; Next += 0x000004;
	zaxxon_coin_status	= Next; Next += 0x000004;
	zaxxon_coin_last	= Next; Next += 0x000004;

//	zaxxon_bg_scroll	= (UINT32*)Next; Next += 0x000001 * sizeof(INT32);

//	soundlatch		= Next; Next += 0x000001;

	sound_state		= Next; Next += 0x000003;

	RamEnd		= Next;
//--------------
	MemEnd			= Next;	
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
	DMA_ScuInit();
//	nBurnSoundLen = 256;//192;//320; // ou 128 ?
	SS_MAP  = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B1;
	SS_MAP2 = ss_map2  =(Uint16 *)SCL_VDP2_VRAM_A1;
	SS_FONT = ss_font  =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_CACHE= cache    =(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclSpPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;
	ss_OtherPri     = (SclOtherPriRegister *)SS_OTHR;
	ss_BgColMix		= (SclBgColMixRegister *)SS_BGMIX;
	ss_sprite		= (SprSpCmd *)SS_SPRIT;
//	ss_scl			= (Fixed32 *)SS_SCL;

	nBurnSprites = 36;

	SS_SET_N0PRIN(7);
	SS_SET_S0PRIN(4);
	SS_SET_N1PRIN(3);
	SS_SET_N2PRIN(5);

	ss_regs->tvmode = 0x8011;

	initLayers();
	initPosition();
	initColors();	
	SaturnInitMem();

	SaturnMem = (UINT8 *)malloc(MALLOC_MAX);
	if(SaturnMem==NULL)
	{
		FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"malloc SaturnMem failed",4,50);
		return 1;
	}
	memset(SaturnMem, 0, MALLOC_MAX);
	SaturnInitMem();

	for(unsigned int i = 0;i<240;i++)
	{
		srcxmask[i] = srcx_buffer + (256*i);
	}
//	memset(CZ80Context,0x00,sizeof(cz80_struc));

	make_lut();
//3 nbg
	initSprites(256-1,256-1,0,0,0,0);  //les deux derniers => delta

	ss_map264 = ((UINT8 *)bitmap)+ (247)+0xE80-225;// + (247);

	ss_sprite[3].color          = 0x400;
	ss_sprite[3].charAddr    = 0x2220;// 0x2000 => 0x80 sprites <<6
	ss_sprite[3].control       = ( JUMP_NEXT | FUNC_DISTORSP ); // | flip);
//	ss_sprite[3].control       = ( JUMP_NEXT | FUNC_NORMALSP ); // | flip);
	ss_sprite[3].drawMode = ( COLOR_4 | ECD_DISABLE | COMPO_REP); //256 colors
//	ss_sprite[3].charSize    = 0x1DFF;  // 232x*224y
//	ss_sprite[3].charSize    = 0x1EE8;  // 232x*224y
	ss_sprite[3].charSize    = 0x1EE0;  // 224x*224y

//	ss_sprite[3].ax = 16;
//	ss_sprite[3].ay = 0; //-15

	ss_sprite[3].bx			= 223+16;
	ss_sprite[3].by			= 239-31;
	ss_sprite[3].ax			= 223+16;
	ss_sprite[3].ay			= 0-31;
	ss_sprite[3].dx			= 0+16;
	ss_sprite[3].dy			= 0-31;
	ss_sprite[3].cx			= 0+16;
	ss_sprite[3].cy			= 239-31;

	for (unsigned int j = 4; j<nBurnSprites; ++j)
	{
		ss_sprite[j].control      = ( JUMP_NEXT | FUNC_NORMALSP);
		ss_sprite[j].drawMode= ( ECD_DISABLE | COMPO_REP);		
		ss_sprite[j].charSize   = 0x420;
	}

    ss_sprite[nBurnSprites-1].control			= CTRL_END;
    ss_sprite[nBurnSprites-1].link				= 0;        
    ss_sprite[nBurnSprites-1].drawMode	= 0;                
    ss_sprite[nBurnSprites-1].color			= 0;                
    ss_sprite[nBurnSprites-1].charAddr		= 0;                
    ss_sprite[nBurnSprites-1].charSize		= 0;

	drawWindow(0,240,0,4,68);
}

//-------------------------------------------------------------------------------------------------------------------------------------
void rotate32_tile(unsigned int size,unsigned char flip, unsigned char *target)
{
	unsigned int i,j,k,l=0;
	unsigned char temp[32][32];
	unsigned char rot[32][32];

	for (k=0;k<size;k++)
	{
		for(i=0;i<32;i++)
			for(j=0;j<16;j++)
			{
				temp[i][j<<1]=target[l+(i*16)+j]>>4;
				temp[i][(j<<1)+1]=target[l+(i*16)+j]&0x0f;
			}

		memset(&target[l],0,32*16);
		
		for(i=0;i<32;i++)
			for(j=0;j<32;j++)
			{
				if(flip)
				 rot[31-i][j]= temp[j][i] ;
				else
				 rot[i][31-j]= temp[j][i] ;
			}

		for(i=0;i<32;i++)
			for(j=0;j<16;j++)
					target[l+(i*16)+j]    = (rot[i][j*2]<<4)|(rot[i][(j*2)+1]&0xf);
		l+=(32*16);
	}		
}
//-------------------------------------------------------------------------------------------------------------------------------------
void make_lut()
{
	for (int i = 0; i<256; i++)
	{
//		sx_lut[256-i] = find_minimum_x(i)-15;
		sx_lut[256-i] = 201-find_minimum_x(i)+14;
	}

	for (int i = 0; i<256; i++)
	{
//		sy_lut[256-i] = 234-find_minimum_y(i);
		sy_lut[256-i] = find_minimum_y(i)-14;//+10;	// bouge x !!!
	}

	for (int i = 0; i<256; i++)
	{
		charaddr_lut[i] = 0x220+((i&0x3f)<<6);
	}

	for(unsigned int i=0;i<0x400;i++)
	{
		int sx = (i & 0x1f);
		int sy = (i >> 5);

		colpromoffs_lut[i] = (sx | ((sy >> 2) << 5));

		sx = sx <<7;
		sy = (32-sy)<<1;

		map_lut[i]= sx|sy;
	}

	int flipoffs = 0x3f;
	int xmask = 255;
	int srcx,vf;
	int flipmask = 0x00;

	for (unsigned int y = 0; y < 240; y++)
	{
		int offset = ((y >> 1) ^ 0xff) + 1 + flipoffs;
		vf = y ^ flipmask;

		for(int x=0;x<256;x++)
		{
			srcx = x ^ flipmask;
			srcx += ((vf >> 1) ^ 0xff) + 1;
			srcx += flipoffs;
			srcxmask[y][x] = srcx & xmask;
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*
<Guillaume> int ** t;
<Guillaume> t = malloc(sizeof(int **) * size1);
<Guillaume> for(i = 0;i < size1;i++) t[i] = malloc(sizeof(int *) * size2);
*/