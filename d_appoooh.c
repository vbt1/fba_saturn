#include "d_appoooh.h"
// FB Alpha Appoooh driver module
// Based on MAME driver
// Todo:
//   finish msm5205/adpcm implementation
//   cleanup(s)
//   Robo Wres init ok?

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvAppoooh = {
		"appoooh", "appooo",
		"Appoooh", 
		appooohRomInfo, appooohRomName, AppooohInputInfo, AppooohDIPInfo,
		DrvInit, DrvExit, DrvFrame, DrvDraw //, DrvScan, &DrvRecalc, 512,
	};

	struct BurnDriver nBurnDrvRobowres = {
		"robowres", NULL,
		"Robo Wres 2001",
		robowresRomInfo, robowresRomName, AppooohInputInfo, RobowresDIPInfo,
		DrvInit, DrvExit, DrvFrame, NULL//,DrvDraw, DrvScan, &DrvRecalc, 512,
	};

	if (strcmp(nBurnDrvAppoooh.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvAppoooh,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvRobowres.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvRobowres,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}

static void DrvPaletteInit()
{
	UINT16 delta = 0, delta2 = 0;
	for (INT32 i = 0; i < 0x220; i++)
	{
		INT32 bit0,bit1,bit2,r,g,b;
		UINT8 pen;

		if (i < 0x100)
		/* charset #1 */
			pen = (DrvColPROM[0x020 + (i - 0x000)] & 0x0f) | 0x00;
		else
		/* charset #2 */
			pen = (DrvColPROM[0x120 + (i - 0x100)] & 0x0f) | 0x10;

		/* red component */
		bit0 = (DrvColPROM[pen] >> 0) & 0x01;
		bit1 = (DrvColPROM[pen] >> 1) & 0x01;
		bit2 = (DrvColPROM[pen] >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		/* green component */
		bit0 = (DrvColPROM[pen] >> 3) & 0x01;
		bit1 = (DrvColPROM[pen] >> 4) & 0x01;
		bit2 = (DrvColPROM[pen] >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		/* blue component */
		bit0 = 0;
		bit1 = (DrvColPROM[pen] >> 6) & 0x01;
		bit2 = (DrvColPROM[pen] >> 7) & 0x01;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		if (i>=0x00 && i < 0x100)
		{
			colBgAddr2[delta] = RGB(r,g,b); // fg
			delta++; if ((delta & 7) == 0) delta += 8;
		}
		else
		{
			DrvPalette[delta2] = RGB(r,g,b);			   // bg !!!!	correct
			delta2++; if ((delta2 & 7) == 0) delta2 += 8;
		}
	}
}

static void DrvRobowresPaletteInit()
{
	for (INT32 i = 0; i < 0x220; i++)
	{
		INT32 bit0, bit1, bit2, r, g, b;


		UINT8 pen = DrvColPROM[0x020 + i] & 0x0f;


		/* red component */
		bit0 = (DrvColPROM[pen] >> 0) & 0x01;
		bit1 = (DrvColPROM[pen] >> 1) & 0x01;
		bit2 = (DrvColPROM[pen] >> 2) & 0x01;
		r = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;


		/* green component */
		bit0 = (DrvColPROM[pen] >> 3) & 0x01;
		bit1 = (DrvColPROM[pen] >> 4) & 0x01;
		bit2 = (DrvColPROM[pen] >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;


		/* blue component */
		bit0 = 0;
		bit1 = (DrvColPROM[pen] >> 6) & 0x01;
		bit2 = (DrvColPROM[pen] >> 7) & 0x01;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		DrvPalette[i] = RGB(r,g,b);
	}
}

static INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;

	DrvMainROM	 	= Next; Next += 0x40000;

	AllRam			= DrvMainROM + 0xe000;

	DrvRAM0			= DrvMainROM + 0xe000;
	DrvRAM1			= DrvMainROM + 0xe800;
	DrvSprRAM0	 	= DrvMainROM + 0xf000;
	DrvFgVidRAM	= DrvMainROM + 0xf020;
	DrvFgColRAM	= DrvMainROM + 0xf420;
	DrvSprRAM1	 	= DrvMainROM + 0xf800;
	DrvBgVidRAM	= DrvMainROM + 0xf820;
	DrvBgColRAM	= DrvMainROM + 0xfc20;
	RamEnd			= DrvMainROM + 0x10000;

	DrvColPROM		= Next; Next += 0x00220;
	DrvSoundROM	= Next; Next += 0x0a000;
//	DrvPalette        = (UINT32*)Next; Next += 0x00220 * sizeof(UINT32);
	DrvPalette        = (UINT16*)colBgAddr;
	DrvGfxTMP0		= Next; Next += 0x0c000;
	DrvGfxTMP1		= Next; Next += 0x0c000;
//	charaddr_lut		= Next; Next += 0x00400 * sizeof(UINT16);

	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	DrvGfxROM0		= SS_CACHE;//Next; Next += 0x0c000*8;
	DrvGfxROM1		= SS_CACHE + 0x0c000*4; //*4;
	DrvGfxROM2		= (UINT8 *)(ss_vram+0x1100);//Next; Next += 0x0c000*8;
	DrvGfxROM3		= DrvGfxROM2 + 0x0c000*2; //*4;

	MemEnd			= Next;

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*inline*/ static int readbit(const UINT8 *src, int bitnum)
{
	return src[bitnum / 8] & (0x80 >> (bitnum % 8));
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void GfxDecode4Bpp(int num, int numPlanes, int xSize, int ySize, int planeoffsets[], int xoffsets[], int yoffsets[], int modulo, unsigned char *pSrc, unsigned char *pDest)
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
				}
			}
//			sc_check();
//			wait_vblank();
		}
	}
//	wait_vblank();
}

static void DrvGfxDecode()
{
	INT32 Planes0[3] = { 2*2048*8*8, 1*2048*8*8, 0*2048*8*8 }; /* the bitplanes are separated */
	INT32 XOffs0[8] = {7, 6, 5, 4, 3, 2, 1, 0};
	INT32 YOffs0[8] = { 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 };

	GfxDecode4Bpp(0x0800, 3,  8,  8, Planes0, XOffs0, YOffs0, 0x040, DrvGfxTMP0, DrvGfxROM0); // modulo 0x040 to verify !!!
	GfxDecode4Bpp(0x0800, 3,  8,  8, Planes0, XOffs0, YOffs0, 0x040, DrvGfxTMP1, DrvGfxROM1); // modulo 0x040 to verify !!!

	INT32 Planes1[3] = { 2*2048*8*8, 1*2048*8*8, 0*2048*8*8 }; /* the bitplanes are separated */
	INT32 XOffs1[16] = {7, 6, 5, 4, 3, 2, 1, 0 , 8*8+7,8*8+6,8*8+5,8*8+4,8*8+3,8*8+2,8*8+1,8*8+0};
	INT32 YOffs1[16] = {0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8};

	GfxDecode4Bpp(0x0200, 3, 16, 16, Planes1, XOffs1, YOffs1, 0x100, DrvGfxTMP0, DrvGfxROM2);
	GfxDecode4Bpp(0x0200, 3, 16, 16, Planes1, XOffs1, YOffs1, 0x100, DrvGfxTMP1, DrvGfxROM3);
}

static void bankswitch(INT32 data)
{
//			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"bankswitch             ",4,80);

	DrvZ80Bank0 = (data & 0x40);
// cpu_setbank(1,&RAM[data&0x40 ? 0x10000 : 0x0a000]);
// 1 bank de 16k																  
	if(DrvZ80Bank0)
	{
		//bprintf (0, _T("bankswitch %02d\n"), DrvZ80Bank0);
		CZetMapArea(0xa000, 0xdfff, 0, DrvMainROM + 0x10000);
		CZetMapArea(0xa000, 0xdfff, 2, DrvMainROM + 0x10000);
	}
	else
	{
		CZetMapArea(0xa000, 0xdfff, 0, DrvMainROM + 0x0a000);
		CZetMapArea(0xa000, 0xdfff, 2, DrvMainROM + 0x0a000);
	}
}

static void __fastcall appoooh_write(unsigned short address, unsigned char data)
{
//			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"appoooh_write             ",4,80);

//	
//	bprintf (0, _T("appoooh_write %04x %02d\n"), address,data);
	
	 if(address>=0xf020 && address <=0xf3ff)
	{
		DrvFgVidRAM[address-0xf020] = data;
		return;
	}

	 if(address>=0xf420 && address <=0xf7ff)
	{
		DrvFgColRAM[address-0xf420] = data;
		return;
	}

	 if(address>=0xf820 && address <=0xfbff)
	{
		DrvBgVidRAM[address-0xf820] = data;
		return;
	}

	 if(address>=0xfc20 && address <=0xffff)
	{
		DrvBgColRAM[address-0xfc20] = data;
		return;
	}
	DrvMainROM[address] = data;
}

static unsigned char __fastcall appoooh_read(unsigned short address)
{
//			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"appoooh_read             ",4,80);

//	bprintf (0, _T("appoooh_read %04x\n"), address);
  
	 if(address>=0xf020 && address <=0xf3ff)
	{
		return DrvFgVidRAM[address-0xf020];
	}

	 if(address>=0xf420 && address <=0xf7ff)
	{
		return DrvFgColRAM[address-0xf420];
	}

	 if(address>=0xf820 && address <=0xfbff)
	{
		return DrvBgVidRAM[address-0xf820];
	}

	 if(address>=0xfc20 && address <=0xffff)
	{
		return DrvBgColRAM[address-0xfc20];
	}	
/*
	if(address >= 0xa000 && address <= 0xdfff)
	{
		bprintf (0, _T("address >= 0xa000 && address <= 0xdfff\n"));
	}	  */

	if(address<0x12000)
		return DrvMainROM[address];
	else
	{
//		bprintf (0, _T("address %04x\n"),address);
		return 0;
	}
}

inline static INT32 DrvMSM5205SynchroniseStream(INT32 nSoundRate)
{
	return (INT32)((double)CZetTotalCycles() * nSoundRate / (nCyclesTotal * 130));
}

static void appoooh_adpcm_w( UINT8 data )
{
	adpcm_address = data << 8;

	MSM5205ResetWrite(0, 0);
	adpcm_data = 0xffffffff;
}

static void appoooh_out_w( UINT8 data )
{
	interrupt_enable = (data & 0x01);

	if ((data & 0x02) != flipscreen) {
		flipscreen = data & 0x02;
	}

	priority = (data & 0x30) >> 4;

	bankswitch(data);
}

static void DrvMSM5205Int()
{
	if (adpcm_address != 0xffffffff) {
		if (adpcm_data == 0xffffffff) {
			adpcm_data = DrvSoundROM[adpcm_address++];

			MSM5205DataWrite(0, adpcm_data >> 4);
			MSM5205VCLKWrite(0, 1);
			MSM5205VCLKWrite(0, 0);

			if (adpcm_data == 0x70) {
				adpcm_address = 0xffffffff;
				MSM5205ResetWrite(0, 1);
			}
		} else {
			MSM5205DataWrite(0, adpcm_data & 0x0f);
			MSM5205VCLKWrite(0, 1);
			MSM5205VCLKWrite(0, 0);
			adpcm_data = -1;
		}
	}
}

UINT8 __fastcall appoooh_in(UINT16 address)
{
	UINT8 ret = 0;

	switch (address & 0xff)
	{
		case 0x00:
		{
			for (INT32 i = 0; i < 8; i++) ret |= DrvJoy1[i] << i;
			return ret;
		}

		case 0x01:
		{
			for (INT32 i = 0; i < 8; i++) ret |= DrvJoy2[i] << i;
			return ret;
		}

		case 0x03:
		{
			return DrvDip[0];
		}

		case 0x04:
			for (INT32 i = 0; i < 8; i++) ret |= DrvJoy3[i] << i;
			return ret;
	}

	return 0;
}

void __fastcall appoooh_out(UINT16 address, UINT8 data)
{
	switch (address & 0xff)
	{
		case 0x00:
			SN76496Write(0, data);
		break;
		
		case 0x01:
			SN76496Write(1, data);
		break;
		
		case 0x02:
			SN76496Write(2, data);
		break;

		case 0x03:
			appoooh_adpcm_w(data);
		break;

		case 0x04:
			appoooh_out_w(data);
		break;

		case 0x05:
			scroll_x = data; // not used.
		break;
	}
}

static void DrawSprites(UINT8 *sprite, UINT32 tileoffset, UINT8 spriteoffset)
{
//			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrawSprites             ",4,60);

	INT32 offs;
	INT32 flipy = 0; //flip_screen_get(gfx->machine);
	UINT32 i = 3 + spriteoffset;

	for (offs = 0x20 - 4; offs >= 0; offs -= 4)
	{
		INT32 sy    = 240 - sprite[offs + 0];
		UINT32 code  = (sprite[offs + 1] >> 2) + ((sprite[offs + 2] >> 5) & 0x07) * 0x40;
		UINT32 color = (sprite[offs + 2] & 0x0f);//+(spriteoffset*2);    /* TODO: bit 4 toggles continuously, what is it? */
		INT32 sx    = sprite[offs + 3];
		UINT32 flipx = (sprite[offs + 1] & 0x01) << 4;
		code		  &= 0x1ff;
		code		 += tileoffset;

		if(sx >= 248)
			sx -= 256;

		if (flipy)
		{
			sx = 239 - sx;
			sy = 239 - sy;
			flipx = !flipx;
		}

		ss_sprite[i].ax		= sx;
		ss_sprite[i].ay		= sy;
		ss_sprite[i].control    = ( JUMP_NEXT | FUNC_NORMALSP | flipx);
		ss_sprite[i].charAddr = 0x220 +(code << 4); //charaddr_lut[code&0x3ff]; //0x220 +(code << 4);//
		ss_sprite[i].color     = (color<<4);
//		if(!spriteoffset)
//			ss_sprite[i].color+=0x100;
		++i;
	}
//			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrawSprites end            ",4,60);
}

static INT32 DrawTiles()
{
	INT32 offs;

	for (offs = 0x3e0 - 1;offs >= 0;offs--)
	{
		INT32 sx,sy,code,color,flipx;
		sx = offs & 0x1f;
		sy = (offs<<1) & (~0x3f);

		UINT32 x = (sx| sy)<<1;
	/* char set #2 */
		code = DrvBgVidRAM[offs] + 256 * ((DrvBgColRAM[offs]>>5) & 0x07);
		color = DrvBgColRAM[offs] & 0x0f;
		flipx = (DrvBgColRAM[offs] & 0x10) <<10;

		ss_map2[x] = flipx | color;
		ss_map2[x+1] =code+0x2000;
	/* char set #1 */
		code = DrvFgVidRAM[offs] + 256 * ((DrvFgColRAM[offs]>>5) & 7);
		color = DrvFgColRAM[offs] & 0x0f;
		flipx = (DrvFgColRAM[offs] & 0x10) <<10;

		ss_map[x] = flipx | color;
		ss_map[x+1] = code;
	}
	return 0;
}

static INT32 DrvDraw()
{
	DrawTiles();
	/* draw sprites */
	if(priority) {
		SS_SET_N2PRIN(6);
		SS_SET_S0PRIN(5);
		/* sprite set #1 */
		DrawSprites(DrvSprRAM0,0, 0);
		/* sprite set #2 */
		DrawSprites(DrvSprRAM1, 0x300, 8);
	}
	else
	{
		SS_SET_N2PRIN(5);
		SS_SET_S0PRIN(6);
		/* sprite set #2 */
		DrawSprites(DrvSprRAM1, 0x300, 8);
		/* sprite set #1 */
		DrawSprites(DrvSprRAM0,0, 0);
	}
	memcpyl((SclSpPriNumRegister *)0x25F800F0, ss_SpPriNum, sizeof(ss_SpPriNum));
	memcpyl((SclBgPriNumRegister *)0x25F800F8, ss_BgPriNum, sizeof(ss_BgPriNum));

	return 0;
}

static INT32 DrvDoReset()
{
	memset (AllRam, 0, RamEnd - AllRam);
	DrvZ80Bank0 = 0;
	scroll_x = 0;
	flipscreen = 0;
	adpcm_address = 0xffffffff;
	adpcm_data = 0;
	MSM5205Reset();

	CZetOpen(0);
	CZetReset();
	bankswitch(0);
	CZetClose();

	return 0;
}

static INT32 DrvRobowresLoadRoms()
{
	if (BurnLoadRom(DrvMainROM + 0x00000, 0, 1)) return 1; //epr-7540.13d
	if (BurnLoadRom(DrvMainROM + 0x08000, 1, 1)) return 1; //epr-7541.14d
	if (BurnLoadRom(DrvMainROM + 0x14000, 2, 1)) return 1; //epr-7542.15d

	if (BurnLoadRom(DrvGfxTMP0 + 0x00000, 3, 1)) return 1;	// epr-7544.7h
	if (BurnLoadRom(DrvGfxTMP0 + 0x08000, 4, 1)) return 1;	// epr-7545.6h
	if (BurnLoadRom(DrvGfxTMP0 + 0x10000, 5, 1)) return 1;	// epr-7546.5h

	if (BurnLoadRom(DrvGfxTMP1 + 0x00000, 6, 1)) return 1;	// epr-7547.7d
	if (BurnLoadRom(DrvGfxTMP1 + 0x08000, 7, 1)) return 1;	// epr-7548.6d
	if (BurnLoadRom(DrvGfxTMP1 + 0x10000, 8, 1)) return 1;	// epr-7549.5d

	if (BurnLoadRom(DrvColPROM + 0x0000,  9, 1)) return 1;	// pr7571.10a
	if (BurnLoadRom(DrvColPROM + 0x0020, 10, 1)) return 1;	// pr7572.7f
	if (BurnLoadRom(DrvColPROM + 0x0120, 11, 1)) return 1;	// pr7573.7g

	if (BurnLoadRom(DrvSoundROM + 0x0000, 12, 1)) return 1;	// epr-7543.12b

	return 0;
}

static INT32 DrvLoadRoms()
{
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 1      ",4,90);

	for (INT32 i = 0; i < 5; i++)
		if (BurnLoadRom(DrvMainROM + i * 0x2000, i +  0, 1)) return 1;
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 2      ",4,90);

 	if (BurnLoadRom(DrvMainROM + 0x0a000,  5, 1)) return 1;	// epr-5911.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 3      ",4,90);

	if (BurnLoadRom(DrvMainROM + 0x0c000,  6, 1)) return 1;	// epr-5913.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 4      ",4,90);

	if (BurnLoadRom(DrvMainROM + 0x10000,  7, 1)) return 1;	// epr-5912.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 5      ",4,90);

	if (BurnLoadRom(DrvMainROM + 0x12000,  8, 1)) return 1;	// epr-5914.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 6      ",4,90);

	if (BurnLoadRom(DrvGfxTMP0 + 0x00000,  9, 1)) return 1;	// epr-5895.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 7      ",4,90);

	if (BurnLoadRom(DrvGfxTMP0 + 0x04000, 10, 1)) return 1;	// epr-5896.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 8      ",4,90);

	if (BurnLoadRom(DrvGfxTMP0 + 0x08000, 11, 1)) return 1;	// epr-5897.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 9      ",4,90);

	if (BurnLoadRom(DrvGfxTMP1 + 0x00000, 12, 1)) return 1;	// epr-5898.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 10      ",4,90);

	if (BurnLoadRom(DrvGfxTMP1 + 0x04000, 13, 1)) return 1;	// epr-5899.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 11      ",4,90);

	if (BurnLoadRom(DrvGfxTMP1 + 0x08000, 14, 1)) return 1;	// epr-5900.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 12      ",4,90);

	if (BurnLoadRom(DrvColPROM + 0x0000, 15, 1)) return 1;	// pr5921.prm
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 13      ",4,90);

	if (BurnLoadRom(DrvColPROM + 0x0020, 16, 1)) return 1;	// pr5922.prm
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 14      ",4,90);

	if (BurnLoadRom(DrvColPROM + 0x0120, 17, 1)) return 1;	// pr5923.prm
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 15      ",4,90);

	if (BurnLoadRom(DrvSoundROM + 0x0000, 18, 1)) return 1;	// epr-5901.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 16      ",4,90);

	if (BurnLoadRom(DrvSoundROM + 0x2000, 19, 1)) return 1;	// epr-5902.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 17      ",4,90);

	if (BurnLoadRom(DrvSoundROM + 0x4000, 20, 1)) return 1;	// epr-5903.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 18      ",4,90);

	if (BurnLoadRom(DrvSoundROM + 0x6000, 21, 1)) return 1;	// epr-5904.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 19      ",4,90);

	if (BurnLoadRom(DrvSoundROM + 0x8000, 22, 1)) return 1;	// epr-5905.bin
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms 20      ",4,90);

	return 0;
}  
static INT32 DrvRobowresInit()
{
	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)malloc(nLen)) == NULL) 
	{
		FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"malloc failed",4,80);
		return 1;
	}
	memset(AllMem, 0, nLen);
	MemIndex();
	if(DrvRobowresLoadRoms()) return 1;
	DrvRobowresPaletteInit();

	return 0;
}

static INT32 DrvInit()
{
	DrvInitSaturn();

	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)malloc(nLen)) == NULL)
	{
		FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"malloc failed",4,80);
		return 1;
	}
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvLoadRoms      ",4,80);
	memset(AllMem, 0, nLen);
	MemIndex();
	if(DrvLoadRoms()) return 1;
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvPaletteInit      ",4,80);
	DrvPaletteInit();
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvGfxDecode      ",4,80);
	DrvGfxDecode();
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"CZetInit              ",4,80);

	nCyclesTotal = 3072000 / 60;

	CZetInit(1);
	CZetOpen(0);
	CZetSetInHandler(appoooh_in);
	CZetSetOutHandler(appoooh_out);
	CZetMapArea(0x0000, 0x7fff, 0, DrvMainROM + 0x0000);	 // AM_RANGE(0x0000, 0x7fff) AM_ROM
	CZetMapArea(0x0000, 0x7fff, 2, DrvMainROM + 0x0000);
	CZetMapArea(0x8000, 0x9fff, 0, DrvMainROM + 0x8000);	// AM_RANGE(0x8000, 0x9fff) AM_ROM
	CZetMapArea(0x8000, 0x9fff, 2, DrvMainROM + 0x8000);

	//ZetMapMemory(DrvMainROM			, 0xe000, 0xe7ff, MAP_RAM);	// AM_RANGE(0xe000, 0xe7ff) AM_RAM

	CZetMapArea(0x0a000, 0xdfff, 0, DrvMainROM + 0x0a000   );
	CZetMapArea(0x0a000, 0xdfff, 2, DrvMainROM + 0x0a000   );

//        AM_RANGE(0xa000, 0xdfff) AM_ROMBANK("bank1")	voir bank_switch
	CZetMapArea(0xe000, 0xe7ff, 0, DrvRAM0);	// AM_RANGE(0xe000, 0xe7ff) AM_RAM
	CZetMapArea(0xe000, 0xe7ff, 1, DrvRAM0);	// AM_RANGE(0xe000, 0xe7ff) AM_RAM
	CZetMapArea(0xe000, 0xe7ff, 2, DrvRAM0);	// AM_RANGE(0xe000, 0xe7ff) AM_RAM

	CZetMapArea(0xe800, 0xefff, 0, DrvRAM1);		// AM_RANGE(0xe800, 0xefff) AM_RAM // RAM ? 
	CZetMapArea(0xe800, 0xefff, 1, DrvRAM1);		// AM_RANGE(0xe800, 0xefff) AM_RAM // RAM ? 
	CZetMapArea(0xe800, 0xefff, 2, DrvRAM1);		// AM_RANGE(0xe800, 0xefff) AM_RAM // RAM ? 

 	CZetSetWriteHandler(appoooh_write);
	CZetSetReadHandler(appoooh_read);
//	ZetMapMemory(DrvRAM2, 0xf000, 0xffff, MAP_RAM);	//AM_RANGE(0xf000, 0xffff) AM_RAM

	CZetClose();
	
	SN76489Init(0, 18432000 / 6, 0);
	SN76489Init(1, 18432000 / 6, 1);
	SN76489Init(2, 18432000 / 6, 1);
//	SN76496SetRoute(0, 0.30, BURN_SND_ROUTE_BOTH);
//	SN76496SetRoute(1, 0.30, BURN_SND_ROUTE_BOTH);
//	SN76496SetRoute(2, 0.30, BURN_SND_ROUTE_BOTH);

//	MSM5205Init(0, DrvMSM5205SynchroniseStream, 384000, DrvMSM5205Int, MSM5205_S64_4B, 1);
	MSM5205Init(0, DrvMSM5205SynchroniseStream, 384000, DrvMSM5205Int, MSM5205_S64_4B, 0);
//	MSM5205SetRoute(0, 0.50, BURN_SND_ROUTE_BOTH);

//	GenericTilesInit();
	DrvDoReset();

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
//	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.plate_addr[0] = (Uint32)SS_MAP;
//	scfg.plate_addr[1] = (Uint32)SS_MAP;
//	scfg.plate_addr[2] = (Uint32)SS_MAP;
//	scfg.plate_addr[3] = (Uint32)SS_MAP;

	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
	scfg.plate_addr[0] = (Uint32)SS_FONT;

// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);
//	scfg.dispenbl      = OFF;
	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr2  = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);
	SCL_AllocColRam(SCL_NBG0,OFF);
	(Uint16*)SCL_AllocColRam(SCL_NBG3,ON);
	(Uint16*)SCL_AllocColRam(SCL_NBG3,ON);
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void make_lut(void)
{
/*	for (UINT32 i = 0; i<0x400; i++)
	{
		charaddr_lut[i] = 0x220+(i<<4);
	}					   */

	for (UINT32 i = 0; i < 1024;i++) 
	{
/*
//			sx = offs % 32;
//			sy = offs / 32;
	*/

//		INT32 sx = ((i) % 32) <<6; //% 32;
//		INT32 sy = (i) / 32;
		UINT32	sx = i & 0x1f;
		UINT32	sy = (i<<1) & (~0x3f);
		map_offset_lut[i] = (sx| sy)<<1;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DrvInitSaturn()
{
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
	nBurnSprites = 16+3;

//3 nbg
	SS_SET_N0PRIN(7);
	SS_SET_N1PRIN(4);
	SS_SET_N2PRIN(6);
	SS_SET_S0PRIN(5);

	initLayers();
	initColors();
	initSprites(256-1,224-1,8,0,0,-8);
	make_lut();

	memset((Uint8 *)ss_map  ,0,0x2000);
	memset((Uint8 *)ss_map2,0,0x2000);

	SprSpCmd *ss_spritePtr;
	unsigned int i = 3;
	
	for (i = 3; i <nBurnSprites; i++) 
	{
		ss_spritePtr				= &ss_sprite[i];
		ss_spritePtr->control   = ( JUMP_NEXT | FUNC_NORMALSP);
		ss_spritePtr->drawMode  = ( ECD_DISABLE | COMPO_REP);	// 16 couleurs
		ss_spritePtr->charSize  = 0x210;  //0x100 16*16
	}
	drawWindow(0,224,240,0,64);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 DrvExit()
{
//	GenericTilesExit();
	CZetExit();

//	SN76496Exit();
	MSM5205Exit();

	free (AllMem);

	return 0;
}

static INT32 DrvFrame()
{
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvFrame              ",4,80);

	if (DrvReset) {
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvDoReset              ",4,80);

		DrvDoReset();
	}

	memset (DrvInputs, 0x00, 3); // DrvJoy1 = active low, 2, 3 = active high

	for (INT32 i = 0; i < 8; i++) {
		DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
		DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
		DrvInputs[2] ^= (DrvJoy3[i] & 1) << i;

	}
	INT32 nInterleave = MSM5205CalcInterleave(0, nCyclesTotal);

	CZetOpen(0);

	CZetRun(nCyclesTotal);

	if (interrupt_enable) CZetNmi();

	MSM5205Update();
	
/*	if (pBurnSoundOut) 
	{
		SN76496Update(0, pBurnSoundOut, nBurnSoundLen);
		SN76496Update(1, pBurnSoundOut, nBurnSoundLen);
		SN76496Update(2, pBurnSoundOut, nBurnSoundLen);
		MSM5205Render(0, pBurnSoundOut, nBurnSoundLen);
	} */
	CZetClose();
	DrvDraw();

	signed short *nSoundBuffer = (signed short *)0x25a20000;
//	SN76496Update(0, &nSoundBuffer[nSoundBufferPos], SOUND_LEN);
//	SN76496Update(1, &nSoundBuffer[nSoundBufferPos], SOUND_LEN);
//	SN76496Update(2, &nSoundBuffer[nSoundBufferPos], SOUND_LEN);
	MSM5205Render(0, &nSoundBuffer[nSoundBufferPos], SOUND_LEN);

	nSoundBufferPos+=(SOUND_LEN); // DOIT etre deux fois la taille copiee
	if(nSoundBufferPos>=0x3C00)//RING_BUF_SIZE)
	{
		PCM_Task(pcm); // bon emplacement
		nSoundBufferPos=0;
	}

//	}
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvFrame END              ",4,80);

	return 0;
}
/*
static INT32 DrvScan(INT32 nAction, INT32 *pnMin)
{
	struct BurnArea ba;
	
	if (pnMin != NULL) {
		*pnMin = 0x029702;
	}

	if (nAction & ACB_MEMORY_RAM) {
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = AllRam;
		ba.nLen	  = RamEnd-AllRam;
		ba.szName = "All Ram";
		BurnAcb(&ba);
	}

	if (nAction & ACB_DRIVER_DATA) {

		ZetScan(nAction);

		SN76496Scan(nAction, pnMin);

		MSM5205Scan(nAction, pnMin);

		SCAN_VAR(priority);
		SCAN_VAR(interrupt_enable);
		SCAN_VAR(flipscreen);
		SCAN_VAR(DrvZ80Bank0);
		SCAN_VAR(scroll_x);
		
		if (nAction & ACB_WRITE) {
			ZetOpen(0);
			bankswitch(DrvZ80Bank0);
			ZetClose();
		}
	}
	return 0;
}
*/
