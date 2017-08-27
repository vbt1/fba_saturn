// Wiz Todo:
//
// stinger: hook-up discrete samples
// scion: static in audio is normal (no kidding)
//
#include "d_wiz.h"
//#define RAZE0 1
#define RAZE1 1

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvWiz = {
		"wiz", NULL,
		"Wiz",
		wizRomInfo, wizRomName, WizInputInfo, WizDIPInfo,
		WizInit, DrvExit, DrvFrame, NULL 
	};

	struct BurnDriver nBurnDrvKungfut = {
		"kungfut", "wiz", 
		"Kung-Fu Taikun",
		kungfutRomInfo, kungfutRomName, KungfutInputInfo, KungfutDIPInfo,
		KungfutInit, DrvExit, DrvFrame, NULL  
	};

	struct BurnDriver nBurnDrvStinger = {
		"stinger", "wiz", 
		"Stinger",
		stingerRomInfo, stingerRomName, StingerInputInfo, StingerDIPInfo,
		StingerInit, DrvExit, DrvFrame,  NULL 
	};

	struct BurnDriver nBurnDrvScion = {
		"scion", "wiz", 
		"Scion", 
		scionRomInfo, scionRomName, ScionInputInfo, ScionDIPInfo,
		ScionInit, DrvExit, DrvFrame, NULL
	};

	if (strcmp(nBurnDrvWiz.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvWiz,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvKungfut.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvKungfut,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvStinger.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvStinger,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvScion.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvScion,sizeof(struct BurnDriver));

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
//-------------------------------------------------------------------------------------------------------------------------------------
void 	 fg_line(UINT16 offs,UINT8 data)
{
	UINT8 bank   = 2 + ((char_bank_select[0] << 1) | char_bank_select[1]);
	UINT8 palbank = (palette_bank[0] << 0) | (palette_bank[1] << 1);
	UINT32 sx		 = (offs & 0x1f);
	UINT32 code	 = data | (char_bank_select[1] << 8);
	UINT32 color	 = (DrvColRAM1[offs] & 0x07) | (palbank << 3);
	UINT32 x		 = map_offset_lut[offs]; 

	if(x >= 0x440 )
	{
		ss_map[x+0x40] = color;
		ss_map[x+0x41] = code+1;
	}
	ss_map[x] = color;
	ss_map[x+1] = code+1;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void 	 bg_line(UINT16 offs,UINT8 data)
{
	UINT8 bank   = 2 + ((char_bank_select[0] << 1) | char_bank_select[1]);
	UINT8 palbank = (palette_bank[0] << 0) | (palette_bank[1] << 1);
	UINT8 sx		= (offs & 0x1f);
	UINT8 sy		= (offs / 32)<<3;
	UINT32	color = (DrvSprRAM0[2 * sx + 1] & 0x04) | (data & 3) | (palbank << 3);
	UINT32 code = data | (bank << 8);
	UINT32 x		= map_offset_lut[offs];

	if(x < 0x440 )
	{
		if(sy>=16 && sy<240)
		{
			ss_font[x] = color ;
			ss_font[x+1] = code+0x201;
		}
	}
	else	
	{	
		ss_map2[x] = ss_map2[x+0x40] = color ;
		ss_map2[x+1] = ss_map2[x+0x41] = code+0x201;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void __fastcall wiz_main_write(UINT16 address, UINT8 data)
{
	switch (address)
	{
		case 0xc800:
		case 0xc801: // coin counter
		return;

		case 0xf000:
			*sprite_bank = data;
		return;

		case 0xf001:
			interrupt_enable[0] = data;
		return;

		case 0xf002:
		case 0xf003:
			palette_bank[address & 1] = data & 1;
		return;

		case 0xf004:
		case 0xf005:
			char_bank_select[address & 1] = data & 1;
		return;

		case 0xf006: // x
		case 0xf007: // y
			screen_flip[address & 1] = data;
			//bprintf(PRINT_NORMAL, _T("address %04d screen_flip %04d\n"),address,data );
		return;

		case 0xf008:
		case 0xf009:
		case 0xf00a:
		case 0xf00b:
		case 0xf00c:
		case 0xf00d:
		case 0xf00e:
		case 0xf00f: // nop
		return;

		case 0xf800:
			if (data != 0x90) {
				soundlatch = data;
			}
		return;

		case 0xf808:
		case 0xf80a: // discrete sound
		return;

		case 0xf818:
			*background_color = data;
		return;
	}
}

UINT8 __fastcall wiz_main_read(UINT16 address)
{
	switch (address)
	{
		case 0xf000:
			return DrvDips[0];

		case 0xf008:
			return DrvDips[1];

		case 0xf010:
			return DrvInputs[0];

		case 0xf018:
			return DrvInputs[1];

		case 0xf800:
			return 0; // watchdog
	}

	// Wiz protection
	if ((address & 0xfc00) == 0xd400)
	{
		if ((address & 0xff) == 0)
		{
			switch (DrvColRAM1[0])
			{
				case 0x35:
					return 0x25;

				case 0x8f:
					return 0x1f;

				case 0xa0:
					return 0x00;
			}
		}
		return DrvColRAM1[address & 0x3ff];
	}

	return 0;
}

void __fastcall wiz_sound_write(UINT16 address, UINT8 data)
{
	address &= 0x7fff;

	switch (address)
	{
		case 0x3000:
		case 0x7000:
			interrupt_enable[1] = data;
		return;

		case 0x4000:
		case 0x4001: if (Wizmode)
			AY8910Write(2, address & 1, data);
		return;

		case 0x5000:
		case 0x5001:
			AY8910Write(0, address & 1, data);
		return;

		case 0x6000:
		case 0x6001:
			AY8910Write(1, address & 1, data);
		return;
	}
}

UINT8 __fastcall wiz_sound_read(UINT16 address)
{
	address &= 0x7fff;

	switch (address)
	{
		case 0x3000:
		case 0x7000:
			return soundlatch;
	}

	return 0;
}

static INT32 DrvDoReset()
{
	memset (AllRam, 0, RamEnd - AllRam);

#ifdef RAZE0
	z80_reset();
#else
	CZetOpen(0);
	CZetReset();
	CZetClose();
#endif

#ifdef RAZE1
	z80_reset();
#else
	CZetOpen(1);
	CZetReset();
	CZetClose();
#endif

	AY8910Reset(0);
	AY8910Reset(1);
	AY8910Reset(2);

	return 0;
}

static void DrvPaletteInit()
{
	UINT32 delta=0;

	for (UINT32 i = 0; i < 0x100; i++)
	{
		UINT32 bit0 = (DrvColPROM[0x000 + i] >> 0) & 0x01;
		UINT32 bit1 = (DrvColPROM[0x000 + i] >> 1) & 0x01;
		UINT32 bit2 = (DrvColPROM[0x000 + i] >> 2) & 0x01;
		UINT32 bit3 = (DrvColPROM[0x000 + i] >> 3) & 0x01;
		UINT32 r = 0x0e * bit0 + 0x1f * bit1 + 0x42 * bit2 + 0x90 * bit3;

		bit0 = (DrvColPROM[0x100 + i] >> 0) & 0x01;
		bit1 = (DrvColPROM[0x100 + i] >> 1) & 0x01;
		bit2 = (DrvColPROM[0x100 + i] >> 2) & 0x01;
		bit3 = (DrvColPROM[0x100 + i] >> 3) & 0x01;
		UINT32 g = 0x0e * bit0 + 0x1f * bit1 + 0x42 * bit2 + 0x90 * bit3;

		bit0 = (DrvColPROM[0x200 + i] >> 0) & 0x01;
		bit1 = (DrvColPROM[0x200 + i] >> 1) & 0x01;
		bit2 = (DrvColPROM[0x200 + i] >> 2) & 0x01;
		bit3 = (DrvColPROM[0x200 + i] >> 3) & 0x01;
		UINT32 b = 0x0e * bit0 + 0x1f * bit1 + 0x42 * bit2 + 0x90 * bit3;

		DrvPalette[i] = colBgAddr[delta] = RGB(r>>3, g>>3, b>>3);
		delta++; if ((delta & 7) == 0) delta += 8;
	}
}


static INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;

	DrvZ80ROM0		= Next; Next += 0x010000;
	DrvZ80Dec			= Next; Next += 0x010000;
	DrvZ80ROM1		= Next; Next += 0x010000;

	DrvGfxROM0		= SS_CACHE; //Next; Next += 6 *  8 *  8 * 256; // 6 banks,   8x8 tiles, 256 tiles (characters)
	DrvGfxROM0b    = (SS_CACHE+0x2000); //SCL_VDP2_VRAM_B0;
 	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	DrvGfxROM1		= &ss_vram[0x1100]; //Next; Next += 3 * 16 * 16 * 256; // 3 banks, 16x16 tiles, 256 tiles (sprites)

	DrvColPROM		= Next; Next += 0x000300;

	DrvPalette		= (UINT16 *)colBgAddr2;//(unsigned int*)Next; Next += 0x0100 * sizeof(int);

	AllRam			= Next;

	DrvZ80RAM0		= Next; Next += 0x000800;
	DrvZ80RAM1		= Next; Next += 0x000400;

	DrvVidRAM0		= Next; Next += 0x000400;
	DrvVidRAM1		= Next; Next += 0x000400;

	DrvColRAM0		= Next; Next += 0x000400;
	DrvColRAM1		= Next; Next += 0x000400;

	DrvSprRAM0		= Next; Next += 0x000100;
	DrvSprRAM1		= Next; Next += 0x000100;

//	soundlatch		= Next; Next += 0x000001;

	sprite_bank		= Next; Next += 0x000001;
	interrupt_enable= Next; Next += 0x000002;
	palette_bank	= Next; Next += 0x000002;
	char_bank_select= Next; Next += 0x000002;
	screen_flip		= Next; Next += 0x000002;
	background_color= Next; Next += 0x000001;

	RamEnd			= Next;
	map_offset_lut = (UINT16*)Next; Next += 1024 * sizeof(UINT16);
	pFMBuffer	= (INT16*)Next; Next += nBurnSoundLen * 9 * sizeof(INT16);
	MemEnd			= Next;

	return 0;
}

static void DrvGfxDecode(UINT32 type, int rotated)
{
	INT32 Plane[3]  = { 0x4000*8, 0x2000*8, 0 };
//	INT32 XOffs[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 };
//	INT32 YOffs[16] = { 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 };
	INT32 YOffs[16] = { 7, 6, 5, 4, 3, 2, 1, 0, 8*8+7, 8*8+6, 8*8+5, 8*8+4, 8*8+3, 8*8+2, 8*8+1, 8*8+0 };
	INT32 XOffs[16] = { 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 };
	INT32 XOffs1[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 };
	INT32 YOffs1[16] = { 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 };

	UINT8 *tmp0 = (UINT8*)0x00200000;
	UINT8 *tmp1 = (UINT8*)malloc(0xc000);

	memcpy (tmp0, DrvGfxROM0, 0x6000);
	memcpy (tmp1, DrvGfxROM1, 0xc000);
	memset (DrvGfxROM0, 0, 0x6000);
	memset (DrvGfxROM1, 0, 0xc000);

// ok
	if (rotated)
	{
		GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs, YOffs, 0x040, tmp0 + 0x0000, DrvGfxROM0 + 0 *  8 *  4 * 256);
		GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs, YOffs, 0x040, tmp0 + 0x0800, DrvGfxROM0 + 1 *  8 *  4 * 256);

		if (type == 0)
		{
			GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs, YOffs, 0x040, tmp1 + 0x6000, DrvGfxROM0b + 0 *  8 *  4 * 256);
			GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs, YOffs, 0x040, tmp1 + 0x0000, DrvGfxROM0b + 1 *  8 *  4 * 256);
			GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs, YOffs, 0x040, tmp1 + 0x0800, DrvGfxROM0b + 2 *  8 *  4 * 256);
			GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs, YOffs, 0x040, tmp1 + 0x6800, DrvGfxROM0b + 3 *  8 *  4 * 256);
		} else {
			GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs, YOffs, 0x040, tmp1 + 0x0000, DrvGfxROM0b + 0 *  8 *  4 * 256);
			GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs, YOffs, 0x040, tmp1 + 0x0800, DrvGfxROM0b + 1 *  8 *  4 * 256);
		}
	}
	else
	{
		GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs1, YOffs1, 0x040, tmp0 + 0x0000, DrvGfxROM0 + 0 *  8 *  4 * 256);
		GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs1, YOffs1, 0x040, tmp0 + 0x0800, DrvGfxROM0 + 1 *  8 *  4 * 256);

		if (type == 0)
		{
			GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs1, YOffs1, 0x040, tmp1 + 0x6000, DrvGfxROM0b + 0 *  8 *  4 * 256);
			GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs1, YOffs1, 0x040, tmp1 + 0x0000, DrvGfxROM0b + 1 *  8 *  4 * 256);
			GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs1, YOffs1, 0x040, tmp1 + 0x0800, DrvGfxROM0b + 2 *  8 *  4 * 256);
			GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs1, YOffs1, 0x040, tmp1 + 0x6800, DrvGfxROM0b + 3 *  8 *  4 * 256);
		} else {
			GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs1, YOffs1, 0x040, tmp1 + 0x0000, DrvGfxROM0b + 0 *  8 *  4 * 256);
			GfxDecode4Bpp(256, 3,  8,  8, Plane, XOffs1, YOffs1, 0x040, tmp1 + 0x0800, DrvGfxROM0b + 1 *  8 *  4 * 256);
		}
	}

	GfxDecode4Bpp(256, 3, 16, 16, Plane, XOffs1, YOffs1, 0x100, tmp0 + 0x0000, DrvGfxROM1 + 0 * 16 * 8 * 256);
	GfxDecode4Bpp(256, 3, 16, 16, Plane, XOffs1, YOffs1, 0x100, tmp1 + 0x0000, DrvGfxROM1 + 1 * 16 * 8 * 256);
	GfxDecode4Bpp(256, 3, 16, 16, Plane, XOffs1, YOffs1, 0x100, tmp1 + 0x6000, DrvGfxROM1 + 2 * 16 * 8 * 256);
 
	//free (tmp0);
	tmp0=NULL;
	free (tmp1);
	tmp1 = NULL;
}

static INT32 WizLoadRoms()
{
	if (BurnLoadRom(DrvZ80ROM0 + 0x00000,  0, 1)) return 1;
	if (BurnLoadRom(DrvZ80ROM0 + 0x04000,  1, 1)) return 1;
	if (BurnLoadRom(DrvZ80ROM0 + 0x08000,  2, 1)) return 1;

	if (BurnLoadRom(DrvZ80ROM1 + 0x00000,  3, 1)) return 1;

	if (BurnLoadRom(DrvGfxROM0 + 0x00000,  7, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM0 + 0x04000,  8, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM0 + 0x08000,  9, 1)) return 1;

	for (UINT32 i = 0; i < 0xc000; i++) {
		DrvGfxROM1[((i & 0x2000) * 3) + ((i & 0xc000) >> 1) + (i & 0x1fff)] = DrvGfxROM0[i];
	}

	if (BurnLoadRom(DrvGfxROM0 + 0x00000,  4, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM0 + 0x02000,  5, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM0 + 0x04000,  6, 1)) return 1;

	if (BurnLoadRom(DrvColPROM + 0x00000, 10, 1)) return 1;
	if (BurnLoadRom(DrvColPROM + 0x00100, 11, 1)) return 1;
	if (BurnLoadRom(DrvColPROM + 0x00200, 12, 1)) return 1;

	DrvGfxDecode(0, 1);

	return 0;
}

static INT32 KungfutLoadRoms()
{
	if (BurnLoadRom(DrvZ80ROM0 + 0x00000,  0, 1)) return 1;
	if (BurnLoadRom(DrvZ80ROM0 + 0x04000,  1, 1)) return 1;
	if (BurnLoadRom(DrvZ80ROM0 + 0x08000,  2, 1)) return 1;

	if (BurnLoadRom(DrvZ80ROM1 + 0x00000,  3, 1)) return 1;

	if (BurnLoadRom(DrvGfxROM0 + 0x00000,  4, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM0 + 0x02000,  5, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM0 + 0x04000,  6, 1)) return 1;

	if (BurnLoadRom(DrvGfxROM1 + 0x00000,  7, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM1 + 0x02000,  8, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM1 + 0x04000,  9, 1)) return 1;

	if (BurnLoadRom(DrvColPROM + 0x00000, 10, 1)) return 1;
	if (BurnLoadRom(DrvColPROM + 0x00100, 11, 1)) return 1;
	if (BurnLoadRom(DrvColPROM + 0x00200, 12, 1)) return 1;

	DrvGfxDecode(1,0);

	return 0;
}

static INT32 StingerLoadRoms()
{
	if (BurnLoadRom(DrvZ80ROM0 + 0x00000,  0, 1)) return 1;
	if (BurnLoadRom(DrvZ80ROM0 + 0x02000,  1, 1)) return 1;
	if (BurnLoadRom(DrvZ80ROM0 + 0x04000,  2, 1)) return 1;
	if (BurnLoadRom(DrvZ80ROM0 + 0x06000,  3, 1)) return 1;
	if (BurnLoadRom(DrvZ80ROM0 + 0x08000,  4, 1)) return 1;

	if (BurnLoadRom(DrvZ80ROM1 + 0x00000,  5, 1)) return 1;

	if (BurnLoadRom(DrvGfxROM0 + 0x00000,  6, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM0 + 0x02000,  7, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM0 + 0x04000,  8, 1)) return 1;

	if (BurnLoadRom(DrvGfxROM1 + 0x00000,  9, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM1 + 0x02000, 10, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM1 + 0x04000, 11, 1)) return 1;

	if (BurnLoadRom(DrvColPROM + 0x00000, 12, 1)) return 1;
	if (BurnLoadRom(DrvColPROM + 0x00100, 13, 1)) return 1;
	if (BurnLoadRom(DrvColPROM + 0x00200, 14, 1)) return 1;

	DrvGfxDecode(1,1);

	return 0;
}

static INT32 ScionLoadRoms()
{
	if (BurnLoadRom(DrvZ80ROM0 + 0x00000,  0, 1)) return 1;
	if (BurnLoadRom(DrvZ80ROM0 + 0x02000,  1, 1)) return 1;
	if (BurnLoadRom(DrvZ80ROM0 + 0x04000,  2, 1)) return 1;
	if (BurnLoadRom(DrvZ80ROM0 + 0x06000,  3, 1)) return 1;
	if (BurnLoadRom(DrvZ80ROM0 + 0x08000,  4, 1)) return 1;

	if (BurnLoadRom(DrvZ80ROM1 + 0x00000,  5, 1)) return 1;

	if (BurnLoadRom(DrvGfxROM0 + 0x00000,  6, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM0 + 0x02000,  7, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM0 + 0x04000,  8, 1)) return 1;

	if (BurnLoadRom(DrvGfxROM1 + 0x00000,  9, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM1 + 0x02000, 10, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM1 + 0x04000, 11, 1)) return 1;

	if (BurnLoadRom(DrvColPROM + 0x00000, 12, 1)) return 1;
	if (BurnLoadRom(DrvColPROM + 0x00100, 13, 1)) return 1;
	if (BurnLoadRom(DrvColPROM + 0x00200, 14, 1)) return 1;

	DrvGfxDecode(1,0);

	return 0;
}

static INT32 DrvInit(int (*RomLoadCallback)(), int rotated)
{
	DrvInitSaturn();

	AllMem = NULL;
	MemIndex();
	UINT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)malloc(nLen)) == NULL)
	{
		return 1;
	}
	memset(AllMem, 0, nLen);
	MemIndex();

	make_lut(rotated);

	{
		if (RomLoadCallback()) return 1;
		DrvPaletteInit();
	}

	CZetInit(2);

#ifdef RAZE0
	z80_init_memmap();
	z80_map_fetch	(0x0000,0xbfff,DrvZ80ROM0); 
	z80_map_read	(0x0000,0xbfff,DrvZ80ROM0);  
	z80_map_fetch	(0xc000,0xc7ff,DrvZ80RAM0); 
	z80_map_read	(0xc000,0xc7ff,DrvZ80RAM0);  
	z80_map_write	(0xc000,0xc7ff,DrvZ80RAM0);  

	z80_map_read	(0xd000, 0xd3ff, DrvVidRAM1);
	z80_map_write	(0xd000, 0xd3ff, DrvVidRAM1);
	z80_map_fetch	(0xd000, 0xd3ff, DrvVidRAM1);
	z80_map_write	(0xd400, 0xd7ff, DrvColRAM1);
	z80_map_fetch	(0xd400, 0xd7ff, DrvColRAM1);
	z80_map_read	(0xd800, 0xd8ff, DrvSprRAM1); // 00 - 3f attributs, 40-5f sprites, 60+ junk
	z80_map_write	(0xd800, 0xd8ff, DrvSprRAM1);
	z80_map_fetch	(0xd800, 0xd8ff, DrvSprRAM1);
	z80_map_read	(0xe000, 0xe3ff, DrvVidRAM0);
	z80_map_write	(0xe000, 0xe3ff, DrvVidRAM0);
	z80_map_fetch	(0xe000, 0xe3ff, DrvVidRAM0);
	z80_map_read	(0xe400, 0xe7ff, DrvColRAM0); //just ram?
	z80_map_write	(0xe400, 0xe7ff, DrvColRAM0);
	z80_map_fetch	(0xe400, 0xe7ff, DrvColRAM0);
	z80_map_read	(0xe800, 0xe8ff, DrvSprRAM0); // 00 - 3f attributs, 40-5f sprites, 60+ junk
	z80_map_write	(0xe800, 0xe8ff, DrvSprRAM0);
	z80_map_fetch	(0xe800, 0xe8ff, DrvSprRAM0);

	z80_add_write(0xc800, 0xc80f, 1, (void *)&wiz_main_write);
	z80_add_write(0xf000, 0xf00f, 1, (void *)&wiz_main_write);
	z80_add_write(0xf800, 0xf81f, 1, (void *)&wiz_main_write);

	z80_add_read(0xd400, 0xd7ff, 1, (void *)&wiz_main_read);
	z80_add_read(0xf000, 0xf01f, 1, (void *)&wiz_main_read);
	z80_add_read(0xf800, 0xf01f, 1, (void *)&wiz_main_read);
#else
	CZetOpen(0);
	CZetMapArea(0x0000, 0xbfff, 0, DrvZ80ROM0);
	CZetMapArea(0x0000, 0xbfff, 2, DrvZ80ROM0);
	CZetMapArea(0xc000, 0xc7ff, 0, DrvZ80RAM0);
	CZetMapArea(0xc000, 0xc7ff, 1, DrvZ80RAM0);
	CZetMapArea(0xc000, 0xc7ff, 2, DrvZ80RAM0);
	CZetMapArea(0xd000, 0xd3ff, 0, DrvVidRAM1);
	CZetMapArea(0xd000, 0xd3ff, 1, DrvVidRAM1);
	CZetMapArea(0xd000, 0xd3ff, 2, DrvVidRAM1);
	CZetMapArea(0xd400, 0xd7ff, 1, DrvColRAM1);
	CZetMapArea(0xd400, 0xd7ff, 2, DrvColRAM1);
	CZetMapArea(0xd800, 0xd8ff, 0, DrvSprRAM1); // 00 - 3f attributs, 40-5f sprites, 60+ junk
	CZetMapArea(0xd800, 0xd8ff, 1, DrvSprRAM1);
	CZetMapArea(0xd800, 0xd8ff, 2, DrvSprRAM1);
	CZetMapArea(0xe000, 0xe3ff, 0, DrvVidRAM0);
	CZetMapArea(0xe000, 0xe3ff, 1, DrvVidRAM0);
	CZetMapArea(0xe000, 0xe3ff, 2, DrvVidRAM0);
	CZetMapArea(0xe400, 0xe7ff, 0, DrvColRAM0); //just ram?
	CZetMapArea(0xe400, 0xe7ff, 1, DrvColRAM0);
	CZetMapArea(0xe400, 0xe7ff, 2, DrvColRAM0);
	CZetMapArea(0xe800, 0xe8ff, 0, DrvSprRAM0); // 00 - 3f attributs, 40-5f sprites, 60+ junk
	CZetMapArea(0xe800, 0xe8ff, 1, DrvSprRAM0);
	CZetMapArea(0xe800, 0xe8ff, 2, DrvSprRAM0);
	CZetSetWriteHandler(wiz_main_write);
	CZetSetReadHandler(wiz_main_read);
	CZetClose();
#endif
//	CZetInit(1);

#ifdef RAZE1
	z80_init_memmap();
	z80_map_fetch	(0x0000,0x1fff,DrvZ80ROM1); 
	z80_map_read	(0x0000,0x1fff,DrvZ80ROM1);  
	z80_map_fetch	(0x2000,0x23ff,DrvZ80RAM1); 
	z80_map_read	(0x2000,0x23ff,DrvZ80RAM1);  
	z80_map_write	(0x2000,0x23ff,DrvZ80RAM1);
	
	z80_add_write(0x4000, 0x400f, 1, (void *)&wiz_sound_write);
	z80_add_write(0x5000, 0x500f, 1, (void *)&wiz_sound_write);
	z80_add_write(0x6000, 0x600f, 1, (void *)&wiz_sound_write);
	z80_add_write(0x7000, 0x700f, 1, (void *)&wiz_sound_write);

	z80_add_read(0x3000, 0x300f, 1, (void *)&wiz_sound_read);
	z80_add_read(0x7000, 0x700f, 1, (void *)&wiz_sound_read);

#else
	CZetOpen(1);
	CZetMapArea(0x0000, 0x1fff, 0, DrvZ80ROM1);
	CZetMapArea(0x0000, 0x1fff, 2, DrvZ80ROM1);
	CZetMapArea(0x2000, 0x23ff, 0, DrvZ80RAM1);
	CZetMapArea(0x2000, 0x23ff, 1, DrvZ80RAM1);
	CZetMapArea(0x2000, 0x23ff, 2, DrvZ80RAM1);
	CZetSetWriteHandler(wiz_sound_write);
	CZetSetReadHandler(wiz_sound_read);
	CZetClose();
#endif

	pAY8910Buffer[0] = pFMBuffer + nBurnSoundLen * 0;
	pAY8910Buffer[1] = pFMBuffer + nBurnSoundLen * 1;
	pAY8910Buffer[2] = pFMBuffer + nBurnSoundLen * 2;
	pAY8910Buffer[3] = pFMBuffer + nBurnSoundLen * 3;
	pAY8910Buffer[4] = pFMBuffer + nBurnSoundLen * 4;
	pAY8910Buffer[5] = pFMBuffer + nBurnSoundLen * 5;
	pAY8910Buffer[6] = pFMBuffer + nBurnSoundLen * 6;
	pAY8910Buffer[7] = pFMBuffer + nBurnSoundLen * 7;
	pAY8910Buffer[8] = pFMBuffer + nBurnSoundLen * 8;

	AY8910Init(0, 1536000, nBurnSoundRate, NULL, NULL, NULL, NULL);
	AY8910Init(1, 1536000, nBurnSoundRate, NULL, NULL, NULL, NULL);
	AY8910Init(2, 1536000, nBurnSoundRate, NULL, NULL, NULL, NULL);
	DrvDoReset();

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initLayers()
{
    Uint16	CycleTb[]={
		0xff45, 0x6fff, //A0
		0xffff, 0xffff,	//A1
		0x012f,0xffff,   //B0
		0xffff, 0xffff  //B1
//		0x4eff, 0x1fff, //B1
	};
 	SclConfig	scfg;

// 3 nbg
	scfg.dispenbl			= ON;
	scfg.charsize			= SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize		= SCL_PN2WORD; //2word
	scfg.platesize		= SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype			= SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype		= SCL_CELL;
	scfg.flip					= SCL_PN_10BIT; // on force à 0
	scfg.patnamecontrl =  0x0000; // a0 + 0x8000
	scfg.plate_addr[0] = (Uint32)SS_MAP;
	scfg.plate_addr[1] = (Uint32)SS_MAP;
	scfg.plate_addr[2] = (Uint32)SS_MAP;
	scfg.plate_addr[3] = (Uint32)SS_MAP;
	SCL_SetConfig(SCL_NBG0, &scfg);
// 3 nbg

//	scfg.pnamesize		= SCL_PN1WORD; //2word
	scfg.platesize		= SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.flip					= SCL_PN_12BIT; // on force à 0
	scfg.patnamecontrl =  0x0000; // a0 + 0x8000
	scfg.plate_addr[0] = (Uint32)SS_MAP2;
	scfg.plate_addr[1] = (Uint32)SS_MAP2;
	scfg.plate_addr[2] = (Uint32)SS_MAP2;
	scfg.plate_addr[3] = (Uint32)SS_MAP2;

	SCL_SetConfig(SCL_NBG1, &scfg);

//	scfg.dispenbl			= OFF;
//	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
//	scfg.datatype 	   = SCL_BITMAP;
//	scfg.mapover	   = SCL_OVER_0;
	scfg.plate_addr[0] = (Uint32)SS_FONT;
	scfg.plate_addr[1] = NULL;
	scfg.plate_addr[2] = NULL;
	scfg.plate_addr[3] = NULL;
// 3 nbg	
	SCL_SetConfig(SCL_NBG2, &scfg);
	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);	  //ON
	(Uint16*)SCL_AllocColRam(SCL_NBG3,ON);
	(Uint16*)SCL_AllocColRam(SCL_NBG3,ON);
	(Uint16*)SCL_AllocColRam(SCL_NBG3,ON);
	ss_regs->dispenbl &= 0xfbff;

	SCL_SetColRam(SCL_NBG1,8,8,palette);
	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void make_lut(int rotated)
{
	if (rotated)
	{
		for (UINT32 i = 0; i < 1024;i++) 
		{
			INT32 sx    = (i & 0x1f);
			UINT8 sy    = (((i / 32)<<3))&0xff;// - DrvSprRAM1[2 * sx + 0]);

			map_offset_lut[i] = ((sy>>3)<<1)|((31-sx)*128);
		}
	}
	else
	{
		for (UINT32 i = 0; i < 1024;i++) 
		{
			INT32 sy = ((i >> 5) & 0x1f)<<7;
			INT32 sx = (i & 0x1f)<<1;
			map_offset_lut[i] = sy | sx;
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	nBurnSprites = 19;
	nBurnLinescrollSize = 0x400;
//	nBurnLinescrollSize1 = 0x400;
	nSoundBufferPos = 0;

	SS_MAP  = ss_map   =(Uint16 *)(SCL_VDP2_VRAM_B0+0x14000);
	SS_MAP2 = ss_map2  =(Uint16 *)(SCL_VDP2_VRAM_B0+0x18000);
	SS_FONT = ss_font  = (Uint16 *)(SCL_VDP2_VRAM_B0+0x1C000);
	SS_CACHE= cache    =(Uint8  *)(SCL_VDP2_VRAM_A0+32);

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_scl			= (Fixed32 *)SS_SCL;
//	ss_scl1			= (Fixed32 *)SS_SCL1;

	ss_regs->tvmode = 0x8011;
 	SCL_SetWindow(SCL_W0,SCL_NBG0,SCL_NBG1,SCL_NBG1,16,0,240,256);
 	SCL_SetWindow(SCL_W1,SCL_NBG1,SCL_NBG0,SCL_NBG0,16,0,240,256);

	SS_SET_N0PRIN(6);
	SS_SET_N1PRIN(5);
	SS_SET_N2PRIN(4);
	SS_SET_S0PRIN(7);

	initLayers();
	initColors();

	initSprites(240-1,256-1,16,0,16,0);
	initScrolling(ON,SCL_VDP2_VRAM_B1);
	memset((Uint8 *)ss_map, 0x11,0x4000);
	memset((Uint8 *)ss_map2,0x11,0x4000);
	memset((Uint8 *)ss_font,0x11,0x4000);

	memset(&ss_scl[0],0,240);

//	drawWindow(0,256,0,4,68);
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 DrvExit()
{
	nSoundBufferPos=0;
	SPR_InitSlaveSH();
#ifdef RAZE0
	z80_stop_emulating();
#endif
#ifdef RAZE1
	z80_stop_emulating();
	z80_map_fetch	(0x0000,0x1fff,(void *)NULL); 
	z80_map_read	(0x0000,0x1fff,(void *)NULL);  
	z80_map_fetch	(0x2000,0x23ff,(void *)NULL); 
	z80_map_read	(0x2000,0x23ff,(void *)NULL);  
	z80_map_write	(0x2000,0x23ff,(void *)NULL);
	
	z80_add_write(0x4000, 0x400f, 1, (void *)NULL);
	z80_add_write(0x5000, 0x500f, 1, (void *)NULL);
	z80_add_write(0x6000, 0x600f, 1, (void *)NULL);
	z80_add_write(0x7000, 0x700f, 1, (void *)NULL);

	z80_add_read(0x3000, 0x300f, 1, (void *)NULL);
	z80_add_read(0x7000, 0x700f, 1, (void *)NULL);
#endif	
	CZetExit();

	AY8910Exit(2);
	AY8910Exit(1);
	AY8910Exit(0);

	for (int i = 0; i < 9; i++) {
		pAY8910Buffer[i] = NULL;
	}

	pFMBuffer = NULL;

	MemEnd = AllRam = RamEnd = DrvZ80ROM0 = DrvZ80Dec = DrvZ80ROM1 = DrvGfxROM0 = NULL;
	DrvGfxROM0b = DrvGfxROM1 = DrvColPROM = DrvZ80RAM0 = DrvZ80RAM1 = DrvVidRAM0 = NULL;
	DrvVidRAM1 = DrvColRAM0  = DrvColRAM1 = DrvSprRAM0 = DrvSprRAM1 = NULL;

/*	soundlatch =*/ sprite_bank = interrupt_enable = palette_bank = char_bank_select = screen_flip = background_color = NULL;
	DrvPalette = NULL;

	free(AllMem);
	AllMem = NULL;

	DrvReset  = 0;
	Wizmode = 0;
	Scionmodeoffset = 0;

 	SCL_SetWindow(SCL_W0,NULL,NULL,NULL,0,0,0,0);
 	SCL_SetWindow(SCL_W1,NULL,NULL,NULL,0,0,0,0);
	return 0;
}

static void draw_background(INT16 bank, INT16 palbank, INT16 colortype)
{
	for (INT16 offs = 0x3ff; offs >= 0; offs--)
	{
		INT16 sx  = (offs & 0x1f);
		UINT8 sy = (offs / 32)<<3;
		UINT16 color;

		if (colortype) 
		{
			color = (DrvSprRAM0[2 * sx | 1] & 0x07) | (palbank << 3);
		}
		else 
		{
			color = (DrvSprRAM0[2 * sx + 1] & 0x04) | (DrvVidRAM0[offs] & 3) | (palbank << 3);
		}

		UINT16 code  = DrvVidRAM0[offs] | (bank << 8);

		UINT32 x = map_offset_lut[offs];
		if(x < 0x440 )
		{
			if(sy>=16 && sy<240)
			{
				ss_font[x] = color ;
				ss_font[x+1] = ((code+0x201));
			}
		}
		else	
		{	
			ss_map2[x] = ss_map2[x+0x40] = color ;
			ss_map2[x+1] = ss_map2[x+0x41] = ((code+0x201)); //&0x5FF) ;
		}
	}
}

static void draw_foreground(INT16 palbank, INT16 colortype)
{
	for (INT16 offs = 0x3ff; offs >= 0; offs--)
	{
		INT32 sx     = (offs & 0x1f);
  		INT16 code = DrvVidRAM1[offs] | (char_bank_select[1] << 8);
		INT16 color = 0;//DrvColRAM1[sx << 1 | 1] & 7;

		if (colortype)
		{
			color = (DrvSprRAM1[2 * sx + 1] & 0x07);
		}
		else
		{
			color = (DrvColRAM1[offs] & 0x07);
		}

		color |= (palbank << 3);

		UINT32 x = map_offset_lut[offs]; 
		if(x >= 0x440 )
		{
			ss_map[x+0x40] = color;
			ss_map[x+0x41] = code+1;
		}
		ss_map[x] = color;
		ss_map[x+1] = code+1;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void draw_sprites(UINT8 *ram, INT16 palbank, INT16 bank, UINT8 delta, UINT8 rotated)
{
	for (INT16 offs = 0x1c; offs >= 0; offs -= 4)
	{
		INT16 sy =    240 - ram[offs + 0];
		INT16 code  = ram[offs + 1] | (bank << 8);
		INT16 color = (ram[offs + 2] & 0x07) | (palbank << 3);
		INT16 sx =    ram[offs + 3];
		if (!sx || !sy) 
		{
			ss_sprite[delta].ax		= ss_sprite[delta].ay		= 0;
			continue;
		}

		if(rotated)
		{
			ss_sprite[delta].control		= ( JUMP_NEXT | FUNC_DISTORSP);
			ss_sprite[delta].drawMode = UCLPIN_ENABLE;		

			ss_sprite[delta].ax		= sy-16;
			ss_sprite[delta].ay		= 256-sx;
			ss_sprite[delta].bx		= ss_sprite[delta].ax;
			ss_sprite[delta].by		= ss_sprite[delta].ay-16;
			ss_sprite[delta].cy		= ss_sprite[delta].by;
			ss_sprite[delta].cx		= ss_sprite[delta].ax+16;
			ss_sprite[delta].dx		= ss_sprite[delta].cx;
			ss_sprite[delta].dy		= ss_sprite[delta].ay;
		}
		else
		{
			ss_sprite[delta].ax		= sx-16;
			ss_sprite[delta].ay		= sy;
		}

		ss_sprite[delta].color			= color<<3;
		ss_sprite[delta].charSize	= 0x210;
		ss_sprite[delta].charAddr	= 0x220+((code)<<4 );
		delta++;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void cleanSprites()
{
	unsigned int delta;	
	for (delta=3; delta<nBurnSprites; delta++)
	{
		ss_sprite[delta].control	   = ( JUMP_NEXT | FUNC_NORMALSP);
		ss_sprite[delta].charSize   = 0;
		ss_sprite[delta].charAddr   = 0;
		ss_sprite[delta].ax   = 0;
		ss_sprite[delta].ay   = 0;
	} 
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void updateSound()
{
	int nSample;
	int n;
	unsigned int deltaSlave;//soundLenSlave;//,titiSlave;
	signed short *nSoundBuffer = (signed short *)0x25a20000;
	deltaSlave    = *(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);

	AY8910Update(0, &pAY8910Buffer[0], nBurnSoundLen);
	AY8910Update(1, &pAY8910Buffer[3], nBurnSoundLen);
	AY8910Update(2, &pAY8910Buffer[6], nBurnSoundLen);

	for (n = 0; n < nBurnSoundLen; n++) 
	{
		nSample  = pAY8910Buffer[0][n]; // >> 2;
		nSample += pAY8910Buffer[1][n]; // >> 2;
		nSample += pAY8910Buffer[2][n]; // >> 2;
		nSample += pAY8910Buffer[3][n]; // >> 2;
		nSample += pAY8910Buffer[4][n]; // >> 2;
		nSample += pAY8910Buffer[5][n]; // >> 2;
		nSample += pAY8910Buffer[6][n]; // >> 2;
		nSample += pAY8910Buffer[7][n]; // >> 2;
		nSample += pAY8910Buffer[8][n]; // >> 2;

		nSample /=4;

		if (nSample < -32768) 
		{
			nSample = -32768;
		} 
		else 
		{
			if (nSample > 32767) 
			{
				nSample = 32767;
			}
		}	
		nSoundBuffer[deltaSlave + n] = nSample;
//		nSoundBuffer[nSoundBufferPos + n] = nSample;
	}

	if(deltaSlave>=RING_BUF_SIZE/2)
//	if(nSoundBufferPos>=RING_BUF_SIZE/2)
	{
		PCM_NotifyWriteSize(pcm, deltaSlave);
		PCM_Task(pcm); // bon emplacement
		deltaSlave=0;
//		nSoundBufferPos = 0;
	}

	deltaSlave+=nBurnSoundLen;
//	nSoundBufferPos+=nBurnSoundLen;

	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = deltaSlave;
}
/*
char buffer[80];
int vspfunc(char *format, ...);

int vspfunc(char *format, ...)
{
   va_list aptr;
   int ret;

   va_start(aptr, format);
   ret = vsprintf(buffer, format, aptr);
   va_end(aptr);

   return(ret);
}*/
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 WizDraw()
{
	INT16 palbank = (palette_bank[0] << 0) | (palette_bank[1] << 1);
	*(Uint16 *)0x25E00000 = DrvPalette[*background_color];

//vspfunc("id %03d %4x       ",*background_color,DrvPalette[*background_color]);

//memcpy((UINT8*)0x00200000,buffer,80);



//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)toto,4,50);
	cleanSprites();

	ss_reg->n1_move_x =  ((DrvSprRAM0[16])<<16) ;
	for (INT16 i = 1; i <32; i++)
	{
		memset(&ss_scl[((i-1)*8)],DrvSprRAM1[64-(i*2)],32);
	}

	draw_background(2 + ((char_bank_select[0] << 1) | char_bank_select[1]), palbank, 0);
	draw_foreground(palbank, 0);
 
	draw_sprites(DrvSprRAM1 + 0x40, palbank, 0, 3, 1);
	draw_sprites(DrvSprRAM0 + 0x40, palbank, 1 + *sprite_bank, 11, 1);

	return 0;
}

static INT32 ScionDraw()
{
	INT16 palbank = (palette_bank[0] << 0) | (palette_bank[1] << 1);
	*(Uint16 *)0x25E00000 = DrvPalette[*background_color];
	cleanSprites();

	ss_reg->n1_move_x =  ((DrvSprRAM0[16])<<16) ;
	for (INT16 i = 0; i <32; i++)
	{
		memset(&ss_scl[((i-1)*8)],DrvSprRAM1[64-(i*2)],32);
	}

	draw_background(2 + ((char_bank_select[0] << 1) | char_bank_select[1]), palbank, 0);
	draw_foreground(palbank, 0);

	draw_sprites(DrvSprRAM1 + 0x40, palbank, 0, 3, 0);
	draw_sprites(DrvSprRAM0 + 0x40, palbank, 1 + *sprite_bank, 11, 0);

	return 0;
}

static INT32 StingerDraw()
{
	INT16 palbank = (palette_bank[0] << 0) | (palette_bank[1] << 1);
	*(Uint16 *)0x25E00000 = DrvPalette[*background_color];
	cleanSprites();

	draw_background(2 + char_bank_select[0], palbank, 1);
	draw_foreground(palbank, 1);

	draw_sprites(DrvSprRAM1 + 0x40, palbank, 0, 3, 1);
	draw_sprites(DrvSprRAM0 + 0x40, palbank, 1, 11, 1);

	return 0;
}

static INT32 KungfutDraw()
{
	INT16 palbank = (palette_bank[0] << 0) | (palette_bank[1] << 1);
	*(Uint16 *)0x25E00000 = DrvPalette[*background_color];
	cleanSprites();

	draw_background(2 + char_bank_select[0], palbank, 0);
	draw_foreground(palbank, 0);

	draw_sprites(DrvSprRAM1 + 0x40, palbank, 0, 3, 0);
	draw_sprites(DrvSprRAM0 + 0x40, palbank, 1, 11, 0);
	return 0;
}

static INT32 DrvFrame()
{
	memset (DrvInputs, 0, 2);
	for (INT16 i = 0; i < 8; i++) 
	{
		DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
		DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
	}

	INT32 nInterleave = 16;
	INT32 nCyclesTotal[2] = { 3072000 / 60, 1789750 / 60 };
	INT32 nCyclesDone[2]  = { 0, 0 };

 	SPR_RunSlaveSH((PARA_RTN*)updateSound, NULL);

	for (INT32 i = 0; i < nInterleave; i++)
	{
#ifdef RAZE0
		nCyclesDone[0] += z80_emulate(nCyclesTotal[0] / nInterleave);

		if ((i & 0x0f) == 0x0f && interrupt_enable[0]) z80_cause_NMI();
#else
		CZetOpen(0);
		nCyclesDone[0] += CZetRun(nCyclesTotal[0] / nInterleave);
		if ((i & 0x0f) == 0x0f && interrupt_enable[0]) CZetNmi();
		CZetClose();
#endif

#ifdef RAZE1
		nCyclesDone[1] += z80_emulate(nCyclesTotal[1] / nInterleave);
		if ((i & 0x03) == 0x03 && interrupt_enable[1]) z80_cause_NMI();
#else
		CZetOpen(1);
		nCyclesDone[1] += CZetRun(nCyclesTotal[1] / nInterleave);
		if ((i & 0x03) == 0x03 && interrupt_enable[1]) CZetNmi();
		CZetClose();
#endif
	}
//	updateSound();
 /*
	if (pBurnSoundOut) {
		AY8910Render(&pAY8910Buffer[0], pBurnSoundOut, nBurnSoundLen, 0);
	}
 */ 
	DrvDraw();
	SPR_WaitEndSlaveSH();
	return 0;
}

static INT32 WizInit()
{
	Wizmode = 1;
	DrvDraw = WizDraw;
	return DrvInit(WizLoadRoms,1);
}

static INT32 KungfutInit()
{
	DrvDraw = KungfutDraw;
	int result = DrvInit(KungfutLoadRoms,0);
 	SCL_SetWindow(SCL_W0,SCL_NBG0,SCL_NBG1,SCL_NBG1,0,0,255,256);
 	SCL_SetWindow(SCL_W1,SCL_NBG1,SCL_NBG0,SCL_NBG0,0,0,256,256);

	return result;
}

static void StingerDecode()
{
	INT32 swap_xor_table[4][4] =
	{
		{ 7,3,5, 0xa0 },
		{ 3,7,5, 0x88 },
		{ 5,3,7, 0x80 },
		{ 5,7,3, 0x28 }
	};

	for (INT32 A = 0; A < 0x10000; A++)
	{
		if (A & 0x2040)
		{
			DrvZ80Dec[A] = DrvZ80ROM0[A];
		}
		else
		{
			INT32 *tbl = swap_xor_table[((A >> 3) & 1) + (((A >> 5) & 1) << 1)];

			DrvZ80Dec[A] = BITSWAP08(DrvZ80ROM0[A],tbl[0],6,tbl[1],4,tbl[2],2,1,0) ^ tbl[3];
		}
	}

	CZetOpen(0);
	CZetMapArea2(0x0000, 0xbfff, 2, DrvZ80Dec, DrvZ80ROM0);
	CZetClose();
}

static INT32 StingerInit()
{
	DrvDraw = StingerDraw;
	INT32 nRet = DrvInit(StingerLoadRoms,1);

	if (nRet == 0) {
		StingerDecode();
	}

	return nRet;
}

static INT32 ScionInit()
{
	DrvDraw = ScionDraw;
	Scionmodeoffset = 8*4; // 8 8x8char offset

	return DrvInit(ScionLoadRoms,0);
}
