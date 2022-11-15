#include "d_zaxxon.h"
//#define RAZE 1
#define draw_background(x) draw_background_test2()
//#define draw_background(x) draw_background_not_rotated(x)
void bg_layer_init();

int ovlInit(char *szShortName)
{
	cleanBSS();

/*	struct BurnDriver nBurnDrvCongo = {
		"congo", "zaxxon",
		"Congo Bongo\0",
		congoRomInfo, congoRomName, CongoBongoInputInfo, ZaxxonDIPInfo,
		CongoInit, DrvExit, DrvFrame
	};
*/
	struct BurnDriver nBurnDrvZaxxon = {
		"zaxxon", NULL,
		"Zaxxon (set 1)",
		zaxxonRomInfo, zaxxonRomName, ZaxxonInputInfo, SzaxxonDIPInfo,
		DrvInit, DrvExit, DrvFrame
	};
/*	struct BurnDriver nBurnDrvZaxxonb = {
		"zaxxonb", "zaxxon",
		"Jackson\0",
		zaxxonRomInfo, zaxxonRomName, ZaxxonInputInfo, ZaxxonDIPInfo,
		ZaxxonbInit, DrvExit, DrvFrame 
	};
*/
	struct BurnDriver nBurnDrvSzaxxon = {
		"szaxxon", "zaxxon",
		"Super Zaxxon",
		szaxxonRomInfo, szaxxonRomName, ZaxxonInputInfo, ZaxxonDIPInfo,
		sZaxxonInit, DrvExit, DrvFrame
	};

//    if (strcmp(nBurnDrvCongo.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvCongo,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvZaxxon.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvZaxxon,sizeof(struct BurnDriver));
//    if (strcmp(nBurnDrvZaxxonb.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvZaxxonb,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvSzaxxon.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvSzaxxon,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
	
	return 0;
}
#ifdef RAZE
UINT8 __fastcall zaxxon_read8000(UINT16 address)
{
	return DrvVidRAM[address & 0x3ff];
}

UINT8 __fastcall zaxxon_readA000(UINT16 address)
{
	return DrvSprRAM[address & 0xff];
}
#endif
UINT8 __fastcall zaxxon_read(UINT16 address)
{
	if ((address & 0xe700) == 0xc000) address &= ~0x18f8;
	if ((address & 0xe700) == 0xc100) address &= ~0x18ff;
	if ((address & 0xe000) == 0xe000) address &= ~0x1f00;	
	
#ifndef RAZE
	if (address >= 0x8000 && address <= 0x9fff) {
		return DrvVidRAM[address & 0x3ff];
	}

	if (address >= 0xa000 && address <= 0xafff) {
		return DrvSprRAM[address & 0xff];
	}
#endif
	
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
inline int readbit(const unsigned char *src, int bitnum)
{
	return src[bitnum / 8] & (0x80 >> (bitnum % 8));
}

#ifdef RAZE
void __fastcall zaxxon_write8000(UINT16 address, UINT8 data)
{
	// set up mirroring

	// video ram
	address &= 0x3ff;
	if(DrvVidRAM[address] != data)
	{
		DrvVidRAM[address] = data;
		UINT32 colpromoffs = colpromoffs_lut[address];
		UINT16 *map = (UINT16*)SS_MAP2+map_lut[address];

		map[0] = (DrvColPROM[colpromoffs] & 0x0f);
		map[1] = data;
	}
}

void __fastcall zaxxon_writeA000(UINT16 address, UINT8 data)
{
	DrvSprRAM[address & 0xff] = data;
}
#endif
void zaxxon_coin_inserted(UINT8 param)
{
	if (zaxxon_coin_last[param] != DrvJoy4[param])
	{
		zaxxon_coin_status[param] = zaxxon_coin_enable[param];
	}
}

UINT8 zaxxon_coin_r(UINT8 param)
{
	return zaxxon_coin_status[param];
}

void zaxxon_coin_enable_w(UINT8 offset, UINT8 data)
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
			UINT16 *map = (UINT16*)SS_MAP2+map_lut[address];			
//			UINT32 x = map_lut[address];
			map[0] = (DrvColPROM[colpromoffs] & 0x0f);
			map[1] = data;
		}
		return;
	}

	if (address >= 0xa000 && address <= 0xafff) {
		DrvSprRAM[address & 0xff] = data;
		return;
	}
#endif

	// address mirroring
	if ((address & 0xe700) == 0xc000) address &= ~0x18f8;
	if ((address & 0xe000) == 0xe000) address &= ~0x1f00;

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
//			zaxxon_flipscreen = ~data & 1;
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
//			*zaxxon_fg_color = (data & 1) << 7;
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
			ss_sprite[3].color          = 0x400+(data<<7);//ou * 256 ?
//			ss_sprite[3].color = data;
		return;

		case 0xe0fb:
			zaxxon_bg_enable = data & 1;
		return;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void GfxDecode(UINT32 num, UINT32 numPlanes, UINT32 xSize, UINT32 ySize, INT32 planeoffsets[], INT32 xoffsets[], INT32 yoffsets[], INT32 modulo, UINT8 *pSrc, UINT8 *pDest)
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

void DrvGfxDecode()
{
	INT32 CharPlane[2] = { 0x4000 * 1, 0x4000 * 0 };
	INT32 TilePlane[3] = { 0x10000 * 2, 0x10000 * 1, 0x10000 * 0 };
	INT32 SpritePlane[3] = { 0x20000 * 2, 0x20000 * 1, 0x20000 * 0 };
	INT32 SpriteXOffs[32] = { STEP8(0, 1), STEP8(64, 1), STEP8(128, 1), STEP8(192, 1) };
	INT32 SpriteYOffs[32] = { STEP8(0, 8), STEP8(256, 8), STEP8(512, 8), STEP8(768, 8) };

	UINT8 *tmp = (UINT8*)RamEnd;//(UINT8*) size (0xc000);

 	UINT8 *ss_vram	= (UINT8 *)SS_SPRAM;

	UINT8 *DrvGfxROM0 = (UINT8 *)SS_CACHE;
	UINT8 *DrvGfxROM1	= (UINT8 *)0x25a60000;
	UINT8 *DrvGfxROM2	= &ss_vram[0x1100];//Next; Next += 0x020000;
//	UINT8 *DrvGfxROM3	= (UINT8 *)DrvGfxROM1+0x010000;

	memcpy (tmp, DrvGfxROM0, 0x1000);
// foreground (text)
	GfxDecode4Bpp(0x0100, 2,  8,  8, CharPlane,   SpriteXOffs, SpriteYOffs, 0x040, tmp, DrvGfxROM0);
	rotate_tile(0x0100,0,(UINT8 *)SS_CACHE);
	memcpy (tmp, DrvGfxROM1, 0x6000);
// background
	GfxDecode(0x0400, 3,  8,  8, TilePlane,   SpriteXOffs, SpriteYOffs, 0x040, tmp, DrvGfxROM1);

	memcpy (tmp, DrvGfxROM2, 0xc000);
// sprites
	GfxDecode4Bpp(0x0080, 3, 32, 32, SpritePlane, SpriteXOffs, SpriteYOffs, 0x400, tmp, DrvGfxROM2);

	rotate32_tile(0x0080,/*0,*/DrvGfxROM2);

//	free (tmp);
//	tmp=NULL;
}

void DrvPaletteInit(UINT32 len)
{
	UINT32 delta=0;

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

void bg_layer_init()
{
	UINT8 *zaxxon_bg_pixmap = (UINT8*)LOWADDR;
	memset(zaxxon_bg_pixmap,0x01,0x100000);
//	INT32 len = (hardware_type == 2) ? 0x2000 : 0x4000;
	INT32 len = 0x4000;
	INT32 mask = len-1;

	UINT8 *DrvGfxROM1	= (UINT8 *)0x25a60000;
	UINT8 *DrvGfxROM3	= (UINT8 *)DrvGfxROM1+0x010000;

	for (UINT32 offs = 0; offs < 32 * 512; offs++)
	{
		INT32 sx = (offs & 0x1f) << 3;
		INT32 sy = ((offs >> 5) << 3) * 256;
		zaxxon_bg_pixmap = (UINT8*)LOWADDR + sy + sx;
		UINT32 moffs = offs & mask;

		UINT32 code = (DrvGfxROM3[moffs]) | ((DrvGfxROM3[moffs | len] & 3) << 8);
		UINT32 color = (DrvGfxROM3[moffs | len] & 0xf0) >> 1;

		UINT8 *src = DrvGfxROM1 + (code << 6);

		for (INT32 y = 0; y < 64; y+=8) 
		{
			zaxxon_bg_pixmap[0] = src[y | 0] | color;
			zaxxon_bg_pixmap[1] = src[y | 1] | color;
			zaxxon_bg_pixmap[2] = src[y | 2] | color;
			zaxxon_bg_pixmap[3] = src[y | 3] | color;
			zaxxon_bg_pixmap[4] = src[y | 4] | color;
			zaxxon_bg_pixmap[5] = src[y | 5] | color;
			zaxxon_bg_pixmap[6] = src[y | 6] | color;
			zaxxon_bg_pixmap[7] = src[y | 7] | color;
			zaxxon_bg_pixmap+=256;
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

void DrvDoReset()
{
	memset (DrvZ80RAM, 0, RamEnd - DrvZ80RAM);

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
}

INT32 DrvInit()
{
	DrvInitSaturn();
//	memset(AllMem, 0, MALLOC_MAX);
	memset(bitmap,	 0x00,0xE000);
 	UINT8 *ss_vram	= (UINT8 *)SS_SPRAM;

	UINT8 *DrvGfxROM0	= (UINT8 *)SS_CACHE;
	UINT8 *DrvGfxROM1	= (UINT8 *)0x25a60000;
	UINT8 *DrvGfxROM2	= &ss_vram[0x1100];//Next; Next += 0x020000;
	UINT8 *DrvGfxROM3	= (UINT8 *)DrvGfxROM1+0x010000;

	memset(DrvGfxROM2+0x00010000,0x00,0xE000);
	{
		if (BurnLoadRom(DrvZ80ROM  + 0x0000,  0, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM  + 0x2000,  1, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM  + 0x4000,  2, 1)) return 1;
	
		if (BurnLoadRom(DrvGfxROM0 + 0x0000,  3, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM0 + 0x0800,  4, 1)) return 1;
	
		if (BurnLoadRom(DrvGfxROM1 + 0x0000,  5, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x2000,  6, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x4000,  7, 1)) return 1;
	
		if (BurnLoadRom(DrvGfxROM2 + 0x0000,  8, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x4000,  9, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x8000, 10, 1)) return 1;
	
		if (BurnLoadRom(DrvGfxROM3 + 0x0000, 11, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM3 + 0x2000, 12, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM3 + 0x4000, 13, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM3 + 0x6000, 14, 1)) return 1;
	
		if (BurnLoadRom(DrvColPROM + 0x0000, 15, 1)) return 1;
		if (BurnLoadRom(DrvColPROM + 0x0100, 16, 1)) return 1;

		DrvGfxDecode();

		DrvPaletteInit(0x200);


		bg_layer_init();
	}
#ifndef RAZE
	CZetInit2(1,CZ80Context);
	CZetOpen(0);
	CZetMapMemory(DrvZ80ROM,	0x0000, 0x5fff, MAP_ROM);
	CZetMapMemory(DrvZ80RAM,	0x6000, 0x6fff, MAP_RAM);
	CZetMapMemory(DrvVidRAM,	0x8000, 0x83ff, MAP_ROM);
	CZetMapMemory(DrvSprRAM,	0xa000, 0xa0ff, MAP_READ|MAP_WRITE);
//	CZetMapArea(0xa000, 0xa0ff, 0, DrvSprRAM);
//	CZetMapArea(0xa000, 0xa0ff, 1, DrvSprRAM);
	
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

INT32 DrvExit()
{
/*	
	DrvDoReset();
//	while(0 != DMA_ScuResult());
//	wait_vblank();
//	SPR_RunSlaveSH((PARA_RTN*)dummy, NULL);
#ifndef RAZE
	CZetExit2();
#else
	z80_stop_emulating();
#endif
//	memset(ss_map2,0x00,0x20000);
//	memset(bitmap,0x00,0xe000);
//	SprSpCmd *ss_spritePtr = &ss_sprite[3];
//	memset(ss_spritePtr,0x00,128*sizeof(SprSpCmd));
//	cleanSprites();
	cleanSprites();
	
	wait_vblank();
*/	
	//cleanDATA();
	cleanBSS();

	nSoundBufferPos=0;
	return 0;
}

// ajouter srcxmask en param
inline void draw_background_test2_no_color(/*UINT32 *srcmask,*/ UINT8 *ss_map264,unsigned int yoffset)
{
	UINT8 *zaxxon_bg_pixmap = (UINT8*)LOWADDR;
	/* loop over visible rows */
	UINT8 *ss_map264_ptr= ss_map264;
		UINT32 **srcptr = (UINT32 **)srcxmask;
	for (unsigned int y = 0; y < 224; y++)
	{
		UINT8 *src = zaxxon_bg_pixmap + (((y + yoffset) & 4095) << 8);
		memcpy(ss_map264_ptr,&src[**srcptr++],216);
		ss_map264_ptr+=240;
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

INT16 find_minimum_y(UINT8 value)
{
	INT16 flipmask = 0x00;//*zaxxon_flipscreen ? 0xff : 0x00;
	INT16 flipconst = 0xf1;//*zaxxon_flipscreen ? 0xef : 0xf1;
	INT16 y;

	/* first find a 16-pixel bucket where we hit */
	for (y = 0; y < 256; y += 16)
	{
		INT16 sum = (value + flipconst + 1) + (y ^ flipmask);
		if ((sum & 0xe0) == 0xe0)
			break;
	}

	/* then scan backwards until we no longer match */
	while (1)
	{
		INT16 sum = (value + flipconst + 1) + ((y - 1) ^ flipmask);
		if ((sum & 0xe0) != 0xe0)
			break;
		y--;
	}

	/* add one line since we draw sprites on the previous line */
	return (y + 1) & 0xff;
}


INT16 find_minimum_x(UINT8 value)
{
	INT16 flipmask = 0x00;
	INT16 x;

	/* the sum of the X position plus a constant specifies the address within */
	/* the line bufer; if we're flipped, we will write backwards */
	x = (value + 0xef + 1) ^ flipmask;
	if (flipmask)
		x -= 31;
	return x & 0xff;
}

void draw_sprites()
{
//	int flipmask = *zaxxon_flipscreen ? 0xff : 0x00;
//#define flipmask 0xff
#define flipxmask 0x140
#define flipymask 0x180
#define flipmaskxy flipxmask|flipymask

/* only the lower half of sprite RAM is read during rendering */
//	unsigned int j=4;
	SprSpCmd *ss_spritePtr;
	ss_spritePtr = &ss_sprite[4];

	for (INT32 offs = 0x7c; offs >= 0; offs -= 4)
	{
		INT16 ax			= sy_lut[DrvSprRAM[offs]];
		if(ax>0 && ax<224)
		{
//			int flip = (DrvSprRAM[offs + 1] ^ 0x00) & flipmaskxy;
			ss_spritePtr->ax		= ax;
			ss_spritePtr->ay		= sx_lut[DrvSprRAM[offs + 3]];
			ss_spritePtr->color		= (DrvSprRAM[offs + 2] & 0x1f)<<4;
			ss_spritePtr->charAddr	= charaddr_lut[DrvSprRAM[offs + 1]];
		}
		else
		{
			ss_spritePtr->ax		= 256;
			ss_spritePtr->ay		= 256;
			ss_spritePtr->charAddr	= 0;
//			ss_spritePtr->charSize = 0;			
		}
		ss_spritePtr++;
	}
	
	ss_spritePtr->control = CTRL_END;
	ss_spritePtr->drawMode = 0;
	ss_spritePtr->charAddr = 0;
	ss_spritePtr->charSize = 0;
	ss_spritePtr->ax	= 0;
	ss_spritePtr->ay	= 0;
}

inline void DrvDraw()
{
	*(UINT16 *)0x25E00000=RGB( 0, 0, 0 );
	//	SPR_RunSlaveSH((PARA_RTN*)draw_sprites, NULL);
	draw_sprites();
		
	if (zaxxon_bg_enable)
	{
		draw_background_test2_no_color(/*(UINT32 *)srcxmask,*/ss_map264,zaxxon_bg_scroll_x2);
	}
	else
	{
		memset(bitmap+0xE80,0x10101010,0xCA00);
	}
	copyBitmap();

//	return 0;
}

inline void zaxxon_coin_lockout()
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

void DrvFrame()
{
	DrvDraw();
	{
		DrvInputs[0] = 0x00;
		DrvInputs[1] = 0x00;
		DrvInputs[2] = 0x00;
		for (UINT32 i = 0; i < 8; i++) {
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
//	SPR_WaitEndSlaveSH();
}
void sega_decode(const UINT8 convtable[32][4])
{
	INT32 A;
	INT32 length = 0x6000;
	INT32 cryptlen = length;
	UINT8 *rom = DrvZ80ROM;
	UINT8 *decrypted = DrvZ80DecROM;
	
	memcpy (DrvZ80DecROM, DrvZ80ROM, 0x6000);
	
#ifndef RAZE	
	CZetOpen(0);
//	CZetMapArea2(0x0000, 0x5fff, 2, DrvZ80DecROM, DrvZ80ROM);
		CZetMapMemory2(DrvZ80DecROM, DrvZ80ROM, 0x0000, 0x5fff, MAP_FETCH);
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
	UINT8 convtable[32][4] =
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

int sZaxxonInit()
{
	DrvInit();
	szaxxon_decode();

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
// el piratero : utilise dma_scu et retester !
inline void copyBitmap()
{
 	UINT8 *ss_vram	= (UINT8 *)SS_SPRAM;
	UINT8 *DrvGfxROM2	= &ss_vram[0x1100];//Next; Next += 0x020000;

//	DMA_ScuMemCopy(DrvGfxROM2+0x00010000,bitmap+0xE80,0xCA00);
//	while(DMA_ScuResult()==2);
//	while(0 != DMA_ScuResult());
	memcpyl(DrvGfxROM2+0x00010000,bitmap+0xE80,0xCA00);
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void initLayers()
{
    Uint16	CycleTb[]={
		0x1f56, 0xff26, //A0
		0xeeee, 0xeeee,	//A1
		0xf5f2,0x4eff,   //B0
		0xeeee, 0xeeee  //B1
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
	scfg.plate_addr[1] = (Uint32)0;
	scfg.plate_addr[2] = (Uint32)0;
	scfg.plate_addr[3] = (Uint32)0;
//	SCL_SetConfig(SCL_NBG1, &scfg);
	SCL_SetConfig(SCL_NBG2, &scfg);
// 3 nbg

	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
	scfg.plate_addr[0] = (Uint32)SS_FONT;
	scfg.plate_addr[1] = (Uint32)0;
	scfg.plate_addr[2] = (Uint32)0;
	scfg.plate_addr[3] = (Uint32)0;
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
inline void initPosition()
{
	SCL_Open();
	ss_reg->n1_move_y =  16 <<16;
	ss_reg->n2_move_y =  16;//(0<<16) ;
	ss_reg->n2_move_x =  8;//(0<<16) ;
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);	  //ON
	SCL_AllocColRam(SCL_NBG3,ON);
	SCL_AllocColRam(SCL_NBG3,ON);
	SCL_AllocColRam(SCL_NBG3,ON);
	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);//OFF);
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void SaturnInitMem()
{
	extern unsigned int _malloc_max_ram;
	UINT8 *Next; Next = (unsigned char *)&_malloc_max_ram;
	memset(Next, 0, MALLOC_MAX);
	
	bitmap			= Next; Next += 0xE000;
	map_lut	 		= (UINT32*)Next; Next += 0x400*sizeof(UINT32);
	colpromoffs_lut	= (UINT32*)Next; Next += 0x400*sizeof(UINT32);
	sx_lut			= (INT16*)Next; Next += 256*sizeof(INT16);
	sy_lut			= (INT16*)Next; Next += 256*sizeof(INT16);
	charaddr_lut	= (UINT16*)Next; Next += 256*sizeof(UINT16);
	srcx_buffer		= (UINT32*)Next; Next += 256*240*sizeof(UINT32);
	CZ80Context		= Next; Next += sizeof(cz80_struc);


//--------------
	DrvZ80ROM		= Next; Next += 0x010000;
	DrvZ80DecROM	= Next; Next += 0x010000;
//	DrvZ80ROM2		= Next; Next += 0x010000;
	DrvColPROM		= Next; Next += 0x000200;

	//AllRam			= Next;

	DrvZ80RAM		= Next; Next += 0x001000;
//	DrvZ80RAM2		= Next; Next += 0x001000;
	DrvSprRAM		= Next; Next += 0x000100;
	DrvVidRAM		= Next; Next += 0x000400;
//	DrvColRAM		= Next; Next += 0x000400;

	zaxxon_coin_enable	= Next; Next += 0x000004;
	zaxxon_coin_status	= Next; Next += 0x000004;
	zaxxon_coin_last	= Next; Next += 0x000004;

//	soundlatch		= Next; Next += 0x000001;
//	sound_state		= Next; Next += 0x000001;

	RamEnd		= Next;
//--------------
//	MemEnd			= Next;	
}
//-------------------------------------------------------------------------------------------------------------------------------------
 void DrvInitSaturn()
{
//	SPR_InitSlaveSH();
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
//	DMA_ScuInit();
//	nBurnSoundLen = 256;//192;//320; // ou 128 ?
	SS_MAP  = NULL;
	SS_MAP2 = (Uint16 *)SCL_VDP2_VRAM_A1;
	SS_FONT = (Uint16 *)SCL_VDP2_VRAM_B0;
	SS_CACHE= (Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
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


//	memset(CZ80Context,0x00,sizeof(cz80_struc));

//	make_lut();
//3 nbg
	initSprites(256-1,256-1,0,0,0,0);  //les deux derniers => delta

	ss_map264 = ((UINT8 *)bitmap)+ (247)+0xE80-225;// + (247);

	SprSpCmd *ss_spritePtr = &ss_sprite[3];	
	
	ss_spritePtr->color       = 0x400;
	ss_spritePtr->charAddr    = 0x2220;// 0x2000 => 0x80 sprites <<6
	ss_spritePtr->control     = ( JUMP_NEXT | FUNC_DISTORSP ); // | flip);
//	ss_spritePtr->control     = ( JUMP_NEXT | FUNC_NORMALSP ); // | flip);
	ss_spritePtr->drawMode    = ( COLOR_4 | ECD_DISABLE | COMPO_REP); //256 colors
//	ss_spritePtr->charSize    = 0x1DFF;  // 232x*224y
//	ss_spritePtr->charSize    = 0x1EE8;  // 232x*224y
	ss_spritePtr->charSize    = 0x1EE0;  // 224x*224y

//	ss_spritePtr->ax = 16;
//	ss_spritePtr->ay = 0; //-15

	ss_spritePtr->bx			= 223+16;
	ss_spritePtr->by			= 239-31;
	ss_spritePtr->ax			= 223+16;
	ss_spritePtr->ay			= 0-31;
	ss_spritePtr->dx			= 0+16;
	ss_spritePtr->dy			= 0-31;
	ss_spritePtr->cx			= 0+16;
	ss_spritePtr->cy			= 239-31;
	ss_spritePtr++;
	
	for (unsigned int j = 4; j<nBurnSprites; j++)
	{
		ss_spritePtr->control   = ( JUMP_NEXT | FUNC_NORMALSP);
		ss_spritePtr->drawMode	= ( ECD_DISABLE | COMPO_REP);		
		ss_spritePtr->charSize  = 0x420;
		ss_spritePtr++;
	}

    ss_spritePtr->control	= CTRL_END;
    ss_spritePtr->link		= 0;        
    ss_spritePtr->drawMode	= 0;
    ss_spritePtr->color		= 0;                
    ss_spritePtr->charAddr	= 0;                
    ss_spritePtr->charSize	= 0;

	drawWindow(0,240,0,4,68);
	make_lut();
}

//-------------------------------------------------------------------------------------------------------------------------------------
void rotate32_tile(unsigned int size,/*unsigned char flip,*/ unsigned char *target)
{
	unsigned int i,j,k;

	unsigned char *temp[32],*rot[32], *t;
	UINT8 *Next=RamEnd;

	for (k=0;k<32;k++)
	{
		rot[k]= Next+32;		
		temp[k]= Next; Next += 256;
	}
	t=target;
	for (k=0;k<size;k++)
	{
		for(i=0;i<32;i++)
		{
			for(j=0;j<16;j++)
			{
				temp[i][j<<1]=t[(i*16)+j]>>4;
				temp[i][(j<<1)+1]=t[(i*16)+j]&0x0f;
			}
		}

		memset(t,0,32*16);
		
		for(i=0;i<32;i++)
		{
			for(j=0;j<32;j++)
			{

				 rot[i][31-j]= temp[j][i] ;
			}


			for(j=0;j<16;j++)
				*t++    = (rot[i][j*2]<<4)|(rot[i][(j*2)+1]&0xf);
		}
	}		
}
//-------------------------------------------------------------------------------------------------------------------------------------
void make_lut()
{

	for (int i = 0; i<256; i++)
	{
		sx_lut[i] = 201-find_minimum_x(255-i)+14;
		sy_lut[i] = find_minimum_y(255-i)-14;//+10;	// bouge x !!!	
		charaddr_lut[i] = 0x220+((i&0x3f)<<6);		
	}

	for(unsigned int i=0;i<0x400;i++)
	{
		int sx = (i & 0x1f);
		int sy = (i >> 5);

		if ((sx | ((sy >> 2) << 5)) < 0x200)
			colpromoffs_lut[i] = (sx | ((sy >> 2) << 5));

		sx = sx <<7;
		sy = (32-sy)<<1;

		if (sy>=0)
			map_lut[i]= sx|sy;
	}

	int flipoffs = 0x3f;
	int xmask = 255;
	int srcx,vf;
	int flipmask = 0x00;

	for (unsigned int y = 0; y < 240; y++)
	{
//		int offset = ((y >> 1) ^ 0xff) + 1 + flipoffs;
		vf = y ^ flipmask;
		srcxmask[y] = srcx_buffer + (256*y);

		for(int x=0;x<256;x++)
		{
			srcx = x ^ flipmask;
			srcx += ((vf >> 1) ^ 0xff) + 1;
			srcx += flipoffs;
			*srcxmask[y]++ = srcx & xmask;
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
