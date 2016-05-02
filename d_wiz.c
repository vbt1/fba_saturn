// Wiz Todo:
//
// stinger: hook-up discrete samples
// scion: static in audio is normal (no kidding)
//
#include "d_wiz.h"

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

	memcpy(shared,&nBurnDrvWiz,sizeof(struct BurnDriver));
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
				*soundlatch = data;
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
			return *soundlatch;
	}

	return 0;
}

static INT32 DrvDoReset()
{
	memset (AllRam, 0, RamEnd - AllRam);

	CZetOpen(0);
	CZetReset();
	CZetClose();

	CZetOpen(1);
	CZetReset();
	CZetClose();

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

	soundlatch		= Next; Next += 0x000001;

	sprite_bank		= Next; Next += 0x000001;
	interrupt_enable= Next; Next += 0x000002;
	palette_bank	= Next; Next += 0x000002;
	char_bank_select= Next; Next += 0x000002;
	screen_flip		= Next; Next += 0x000002;
	background_color= Next; Next += 0x000001;

	RamEnd			= Next;

	{
		for (INT32 i = 0; i < 9; i++) {
			pAY8910Buffer[i] = (INT16*)Next; Next += nBurnSoundLen * sizeof(UINT16);
		}
	}

	MemEnd			= Next;

	return 0;
}

static void DrvGfxDecode(UINT32 type)
{
	INT32 Plane[3]  = { 0x4000*8, 0x2000*8, 0 };
//	INT32 XOffs[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 };
//	INT32 YOffs[16] = { 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 };
	INT32 YOffs[16] = { 7, 6, 5, 4, 3, 2, 1, 0, 8*8+7, 8*8+6, 8*8+5, 8*8+4, 8*8+3, 8*8+2, 8*8+1, 8*8+0 };
	INT32 XOffs[16] = { 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 };

	UINT8 *tmp0 = (UINT8*)0x00200000;
	UINT8 *tmp1 = (UINT8*)malloc(0xc000);

	memcpy (tmp0, DrvGfxROM0, 0x6000);
	memcpy (tmp1, DrvGfxROM1, 0xc000);
	memset (DrvGfxROM0, 0, 0x6000);
	memset (DrvGfxROM1, 0, 0xc000);

// ok
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

	INT32 XOffs1[16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8*8+0, 8*8+1, 8*8+2, 8*8+3, 8*8+4, 8*8+5, 8*8+6, 8*8+7 };
	INT32 YOffs1[16] = { 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 };


	GfxDecode4Bpp(256, 3, 16, 16, Plane, XOffs1, YOffs1, 0x100, tmp0 + 0x0000, DrvGfxROM1 + 0 * 16 * 8 * 256);
	GfxDecode4Bpp(256, 3, 16, 16, Plane, XOffs1, YOffs1, 0x100, tmp1 + 0x0000, DrvGfxROM1 + 1 * 16 * 8 * 256);
	GfxDecode4Bpp(256, 3, 16, 16, Plane, XOffs1, YOffs1, 0x100, tmp1 + 0x6000, DrvGfxROM1 + 2 * 16 * 8 * 256);
 
	//free (tmp0);
	tmp0=NULL;
	free (tmp1);
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

	DrvGfxDecode(0);

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

	DrvGfxDecode(1);

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

	DrvGfxDecode(1);

	return 0;
}

static INT32 DrvInit(int (*RomLoadCallback)())
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

	{
		if (RomLoadCallback()) return 1;

		DrvPaletteInit();
	}

	CZetInit(2);
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

//	CZetInit(1);
	CZetOpen(1);
	CZetMapArea(0x0000, 0x1fff, 0, DrvZ80ROM1);
	CZetMapArea(0x0000, 0x1fff, 2, DrvZ80ROM1);
	CZetMapArea(0x2000, 0x23ff, 0, DrvZ80RAM1);
	CZetMapArea(0x2000, 0x23ff, 1, DrvZ80RAM1);
	CZetMapArea(0x2000, 0x23ff, 2, DrvZ80RAM1);
	CZetSetWriteHandler(wiz_sound_write);
	CZetSetReadHandler(wiz_sound_read);
	CZetClose();

	AY8910Init(0, 1536000, nBurnSoundRate, NULL, NULL, NULL, NULL);
	AY8910Init(1, 1536000, nBurnSoundRate, NULL, NULL, NULL, NULL);
	AY8910Init(2, 1536000, nBurnSoundRate, NULL, NULL, NULL, NULL);
//	AY8910SetAllRoutes(0, 0.10, BURN_SND_ROUTE_BOTH);
//	AY8910SetAllRoutes(1, 0.10, BURN_SND_ROUTE_BOTH);
//	AY8910SetAllRoutes(2, 0.10, BURN_SND_ROUTE_BOTH);

//	GenericTilesInit();

	DrvDoReset();

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initLayers()
{
    Uint16	CycleTb[]={
		0x1f56, 0xffff, //A0
		0xffff, 0xffff,	//A1
		0xf5f2,0x4eff,   //B0
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
	scfg.plate_addr[0] = (Uint32)SS_MAP2;
	scfg.plate_addr[1] = (Uint32)SS_MAP2;
	scfg.plate_addr[2] = (Uint32)SS_MAP2;
	scfg.plate_addr[3] = (Uint32)SS_MAP2;
	SCL_SetConfig(SCL_NBG1, &scfg);
// 3 nbg

	scfg.pnamesize		= SCL_PN1WORD; //2word
	scfg.platesize		= SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.flip					= SCL_PN_12BIT; // on force à 0
	scfg.patnamecontrl =  0x0000; // a0 + 0x8000
	scfg.plate_addr[0] = (Uint32)SS_MAP;
	scfg.plate_addr[1] = (Uint32)SS_MAP;
	scfg.plate_addr[2] = (Uint32)SS_MAP;
	scfg.plate_addr[3] = (Uint32)SS_MAP;

	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
	scfg.plate_addr[0] = (Uint32)SS_FONT;

// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);
	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initPosition()
{
	SCL_Open();
	ss_reg->n0_move_y =  (16<<16) ;
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);	  //ON
	(Uint16*)SCL_AllocColRam(SCL_NBG3,ON);
	(Uint16*)SCL_AllocColRam(SCL_NBG3,ON);
	(Uint16*)SCL_AllocColRam(SCL_NBG3,ON);
//	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);//OFF);
	(Uint16*)SCL_AllocColRam(SCL_NBG0,OFF);
	SCL_SetColRam(SCL_NBG0,8,8,palette);
	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	nBurnSprites = 32;
	nSoundBufferPos = 0;

	SS_MAP  = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B1;
	SS_MAP2 = ss_map2  =(Uint16 *)SCL_VDP2_VRAM_B1+0xC000;
	SS_FONT = ss_font  =(Uint16 *)SCL_VDP2_VRAM_A1;
	SS_CACHE= cache    =(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
//	ss_scl			= (Fixed32 *)SS_SCL;
#ifdef CACHE
	memset(bg_dirtybuffer,1,2048);
#endif
	ss_regs->tvmode = 0x8011;

	SS_SET_S0PRIN(6);
	SS_SET_N0PRIN(7);
	SS_SET_N1PRIN(4);
	SS_SET_N2PRIN(5);

	initLayers();
	initPosition();
	initColors();
	initSprites(256-1,256-1,8,0,-16,0);
	//play=1;
//	drawWindow(0,240,0,2,66);
//	initScrolling(ON,SCL_VDP2_VRAM_B0+0x4000);
//	memset(&ss_scl[0],16<<16,64);
//	memset(&ss_scl[0],16<<16,128);

	drawWindow(0,256,0,4,68);
//	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 DrvExit()
{
	CZetExit();

	AY8910Exit(0);
	AY8910Exit(1);
	AY8910Exit(2);

	free (AllMem);
	AllMem = NULL;

	Wizmode = 0;
	Scionmodeoffset = 0;

	return 0;
}


static void draw_background(INT16 bank, INT16 palbank, INT16 colortype)
{
//	SCL_Open();
	ss_reg->n1_move_x =  ((DrvSprRAM0[16])<<16) ;

	for (INT16 offs = 0x3ff; offs >= 0; offs--)
	{
		INT16 sx  = (offs & 0x1f);
//		UINT8 sy = (((offs / 32)<<3)); // - DrvSprRAM0[2 * sx + 0]) &0xff;
		UINT8 sy = (((offs / 32)<<3)) &0xff; // - DrvSprRAM0[2 * sx + 0]) &0xff;
		INT16 color;

//			UINT32 sx1 = (31-((offs) & 0x1f))<<6;
//			UINT32 sy1 = ((offs) >> 5);
			

		if (colortype) 
		{
			color = (DrvSprRAM0[2 * sx | 1] & 0x07) | (palbank << 3);
		}
		else 
		{
			color = (DrvSprRAM0[2 * sx + 1] & 0x04) | (DrvVidRAM0[offs] & 3) | (palbank << 3);
		}

		UINT16 code  = DrvVidRAM0[offs] | (bank << 8);

//		UINT32 x = (((sy>>3)*64)|sx)<<1;
		UINT32 x = ((sy>>3)<<1)|((31-sx)*128);
		ss_map2[x] = ss_map2[x+0x40] = color ;
		ss_map2[x+1] = ss_map2[x+0x41] = ((code+0x200)&0x5FF) ;
/* 
		if (screen_flip[1]) { // flipy
			if (screen_flip[0]) { // flipx
				Render8x8Tile_Mask_FlipXY_Clip(pTransDraw, code, (sx << 3) ^ 0xf8, sy - 16, color, 3, 0, 0, DrvGfxROM0);
			} else {
//				Render8x8Tile_Mask_FlipY_Clip(pTransDraw, code, sx << 3, (248 - (sy <<3)) + 16, color, 3, 0, 0, DrvGfxROM0);
				Render8x8Tile_Mask_FlipY_Clip(pTransDraw, code, sx << 3, sy - 16, color, 3, 0, 0, DrvGfxROM0);
			}
		} else {
			if (screen_flip[0]) { // flipx
//				Render8x8Tile_Mask_FlipX_Clip(pTransDraw, code, (sx << 3) ^ 0xf8, (sy <<3) + 16, color, 3, 0, 0, DrvGfxROM0);
				Render8x8Tile_Mask_FlipX_Clip(pTransDraw, code, (sx << 3) ^ 0xf8, sy - 16, color, 3, 0, 0, DrvGfxROM0);
			} else {
//				Render8x8Tile_Mask_Clip(pTransDraw, code, sx << 3, (sy <<3) + 16, color, 3, 0, 0, DrvGfxROM0);
				Render8x8Tile_Mask_Clip(pTransDraw, code, (sx << 3)-Scionmodeoffset, sy - 16, color, 3, 0, 0, DrvGfxROM0);
			}
		}
*/ 
	}
}

static void draw_foreground(INT16 palbank, INT16 colortype)
{
	for (INT16 offs = 0x3ff; offs >= 0; offs--)
	{
		INT32 sx    = (offs & 0x1f);
		UINT8 sy    = (((offs / 32)<<3) - DrvSprRAM1[2 * sx + 0]);
 		INT16 code  = DrvVidRAM1[offs] | (char_bank_select[1] << 8);
		INT16 color = DrvColRAM1[sx << 1 | 1] & 7;
		INT16 scroll;

//		sx = (offs & 0x1f);
//		UINT32 sy   =(offs / 32) <<6;
//		INT32 sx = (31-((offs & 0x1f) - (DrvSprRAM1[2 * sx1 + 0]>>3))) & 0x1f;
//		sx = sx << 6;
//		UINT32 sy = ((offs) >> 5);

		if (colortype)
		{
			color = (DrvSprRAM1[2 * sx + 1] & 0x07);
		}
		else
		{
			color = (DrvColRAM1[offs] & 0x07);
		}

		color |= (palbank << 3);
		scroll = (8*sy + 256 - DrvVidRAM1[2 * sx]) % 256;
/*		if (screen_flip[1])
		{
			scroll = (248 - scroll) % 256;
		}

		if (screen_flip[0]) sx = 31 - sx;
*/
//		color = 0;
//		UINT32 x = ((sy>>3)*64)|sx;
		UINT32 x = ((sy>>3))|((31-sx)*64);
		ss_map[x] = ss_map[offs+0x1000] = (color << 12 | /*flip << 6 |*/ code&0x1FF) ;

//		Render8x8Tile_Mask_Clip(pTransDraw, code, (sx << 3)-Scionmodeoffset, sy-16, color, 3, 0, 0, DrvGfxROM0);
	}
}

static void draw_sprites(UINT8 *ram, INT16 palbank, INT16 bank)
{
	UINT32 delta=3;

	for (INT16 offs = 0x1c; offs >= 0; offs -= 4)
	{
		INT16 sy =    240 - ram[offs + 0];
		INT16 code  = ram[offs + 1] | (bank << 8);
		INT16 color = (ram[offs + 2] & 0x07) | (palbank << 3);
		INT16 sx =    ram[offs + 3];
		if (!sx || !sy) 
		{
			ss_sprite[delta].ax		= ss_sprite[delta].ay		= 	ss_sprite[delta].bx		= ss_sprite[delta].by		= 0;
			ss_sprite[delta].cy		= 	ss_sprite[delta].cx		= 	ss_sprite[delta].dx		= ss_sprite[delta].dy		= 0;
			continue;
		}

	ss_sprite[delta].control		= ( JUMP_NEXT | FUNC_DISTORSP);

	ss_sprite[delta].ax		= sy+16;
	ss_sprite[delta].ay		= 256-sx;
	ss_sprite[delta].bx		= ss_sprite[delta].ax;
	ss_sprite[delta].by		= ss_sprite[delta].ay-16;
	ss_sprite[delta].cy		= ss_sprite[delta].by;
	ss_sprite[delta].cx		= ss_sprite[delta].ax+16;
	ss_sprite[delta].dx		= ss_sprite[delta].cx;
	ss_sprite[delta].dy		= ss_sprite[delta].ay;

		ss_sprite[delta].color			= color<<3;
		ss_sprite[delta].charSize	= 0x210;
		ss_sprite[delta].charAddr	= 0x220+((code)<<4 );
		delta++;
	}
}

static INT32 DrvDraw()
{
	INT16 palbank = (palette_bank[0] << 0) | (palette_bank[1] << 1);
/*
	if (DrvRecalc) {
		DrvPaletteInit();
		DrvRecalc = 0;
	}

	for (INT32 i = 0; i < nScreenWidth * nScreenHeight; i++) {
		pTransDraw[i] = *background_color;
	} 
*/
	draw_background(2 + ((char_bank_select[0] << 1) | char_bank_select[1]), palbank, 0);
	draw_foreground(palbank, 0);

	draw_sprites(DrvSprRAM1 + 0x40, palbank, 0);
	draw_sprites(DrvSprRAM0 + 0x40, palbank, 1 + *sprite_bank);

//	BurnTransferCopy(DrvPalette);

	return 0;
}

static INT32 StingerDraw()
{
	INT16 palbank = (palette_bank[0] << 0) | (palette_bank[1] << 1);

	if (DrvRecalc) {
		DrvPaletteInit();
		DrvRecalc = 0;
	}
/*
	for (INT32 i = 0; i < nScreenWidth * nScreenHeight; i++) {
		pTransDraw[i] = *background_color;
	}
*/
	draw_background(2 + char_bank_select[0], palbank, 1);
	draw_foreground(palbank, 1);

	draw_sprites(DrvSprRAM1 + 0x40, palbank, 0);
	draw_sprites(DrvSprRAM0 + 0x40, palbank, 1);

//	BurnTransferCopy(DrvPalette);

	return 0;
}

static INT32 KungfutDraw()
{
	INT16 palbank = (palette_bank[0] << 0) | (palette_bank[1] << 1);

	if (DrvRecalc) {
		DrvPaletteInit();
		DrvRecalc = 0;
	}
/*
	for (INT32 i = 0; i < nScreenWidth * nScreenHeight; i++) {
		pTransDraw[i] = *background_color;
	}
*/
	draw_background(2 + char_bank_select[0], palbank, 0);
	draw_foreground(palbank, 0);

	draw_sprites(DrvSprRAM1 + 0x40, palbank, 0);
	draw_sprites(DrvSprRAM0 + 0x40, palbank, 1);

//	BurnTransferCopy(DrvPalette);
	
	return 0;
}

static INT32 DrvFrame()
{
/*	if (DrvReset) {
		DrvDoReset();
	}
 */
	{
		memset (DrvInputs, 0, 2);
		for (INT16 i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
		}
	}

	INT32 nInterleave = 16;
	INT32 nCyclesTotal[2] = { 3072000 / 60, 1789750 / 60 };
	INT32 nCyclesDone[2]  = { 0, 0 };

	for (INT32 i = 0; i < nInterleave; i++)
	{
		CZetOpen(0);
		nCyclesDone[0] += CZetRun(nCyclesTotal[0] / nInterleave);
		if ((i & 0x0f) == 0x0f && interrupt_enable[0]) CZetNmi();
		CZetClose();

		CZetOpen(1);
		nCyclesDone[1] += CZetRun(nCyclesTotal[1] / nInterleave);
		if ((i & 0x03) == 0x03 && interrupt_enable[1]) CZetNmi();
		CZetClose();
	}
 /*
	if (pBurnSoundOut) {
		AY8910Render(&pAY8910Buffer[0], pBurnSoundOut, nBurnSoundLen, 0);
	}
 */ 
//	if (pBurnDraw) {
//		BurnDrvRedraw();
//	}
	DrvDraw();
	return 0;
}

static INT32 WizInit()
{
	Wizmode = 1;

	return DrvInit(WizLoadRoms);
}

static INT32 KungfutInit()
{
	return DrvInit(KungfutLoadRoms);
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
	INT32 nRet = DrvInit(StingerLoadRoms);

	if (nRet == 0) {
		StingerDecode();
	}

	return nRet;
}

static INT32 ScionInit()
{
	Scionmodeoffset = 8*4; // 8 8x8char offset

	return DrvInit(StingerLoadRoms);
}
