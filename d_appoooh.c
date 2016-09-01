#include "d_appoooh.h"
// FB Alpha Appoooh driver module
// Based on MAME driver
// Todo:
//   finish msm5205/adpcm implementation
//   cleanup(s)
//   Robo Wres init ok?
static UINT8 is_fg_dirty[0x400];

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvAppoooh = {
		"appoooh", "appooo",
		"Appoooh", 
		appooohRomInfo, appooohRomName, AppooohInputInfo, AppooohDIPInfo,
		DrvInit, DrvExit, DrvFrame, DrvDraw //, DrvScan, &DrvRecalc, 512,
	};

	struct BurnDriver nBurnDrvRobowres = {
		"robowres", "appooo",
		"Robo Wres 2001",
		robowresRomInfo, robowresRomName, AppooohInputInfo, RobowresDIPInfo,
		DrvRobowresInit, DrvExit, DrvFrame, NULL//,DrvDraw, DrvScan, &DrvRecalc, 512,
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
			colBgAddr2[delta] = BurnHighCol(r, g, b, 0); // fg
			delta++; if ((delta & 7) == 0) delta += 8;
		}
		else
		{
			DrvPalette[delta2] = BurnHighCol(r, g, b, 0);			   // bg !!!!	correct
			delta2++; if ((delta2 & 7) == 0) delta2 += 8;
		}
	}
}

static void DrvRobowresPaletteInit()
{
	UINT16 delta = 0, delta2 = 0;
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

static INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;

	DrvMainROM	 	= Next; Next += 0x24000;
	DrvFetch		= Next; Next += 0x24000;

	AllRam			= Next;
	DrvRAM0			= Next; Next += 0x800;
	DrvRAM1			= Next; Next += 0x800;
	DrvRAM2			= Next; Next += 0x1000;
	DrvSprRAM0	 	= Next; Next += 0x800;
	DrvFgVidRAM	    = Next; Next += 0x800;
	DrvFgColRAM	    = Next; Next += 0x800;
	DrvSprRAM1	 	= Next; Next += 0x800;
	DrvBgVidRAM	    = Next; Next += 0x800;
	DrvBgColRAM	    = Next; Next += 0x800;
	RamEnd			= Next;

	DrvColPROM		= Next; Next += 0x00220;
	DrvSoundROM	    = Next; Next += 0x0a000;
//	DrvSoundROM	= (UINT8*)0x2F6000;
	DrvPalette        = (UINT16*)colBgAddr;
	map_offset_lut  =  Next; Next +=0x400*sizeof(UINT16);

	DrvGfxTMP0		= (UINT8 *)0x00200000;
	DrvGfxTMP1		= (UINT8 *)0x00218000;
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	DrvGfxROM0		= SS_CACHE;
	DrvGfxROM1		= SS_CACHE + 0x30000;
	DrvGfxROM2		= (UINT8 *)(ss_vram+0x1100);
	DrvGfxROM3		= DrvGfxROM2 + 0x18000;

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
	unsigned int c;
	unsigned int size = (xSize/2) * ySize;
//	wait_vblank();
	for (c = 0; c < num; c++) {
		int plane, x, y;
		UINT8 *dp = pDest + (c * size);
		memset(dp, 0, size);
	
		for (plane = 0; plane < numPlanes; plane++) {
			int planebit = 1 << (numPlanes - 1 - plane);
			int planeoffs = (c * modulo) + planeoffsets[plane];
		
			for (y = 0; y < ySize; y++) {
				int yoffs = planeoffs + yoffsets[y];
				dp = pDest + (c * size) + (y * (xSize/2));
			
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

static void DrvRobowresGfxDecode()
{
	INT32 Planes0[3] = { RGN_FRAC(0x18000, 2,3), RGN_FRAC(0x18000, 1,3), RGN_FRAC(0x18000, 0,3) };
	INT32 XOffs0[8] = { 7, 6, 5, 4, 3, 2, 1, 0 };
	INT32 YOffs0[8] = { 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8 };

	GfxDecode4Bpp(0x1000, 3,  8,  8, Planes0, XOffs0, YOffs0, 0x040, DrvGfxTMP0, DrvGfxROM0);
	GfxDecode4Bpp(0x1000, 3,  8,  8, Planes0, XOffs0, YOffs0, 0x040, DrvGfxTMP1, DrvGfxROM1);

	INT32 Planes1[3] = { RGN_FRAC(0x18000, 2,3),RGN_FRAC(0x18000, 1,3),RGN_FRAC(0x18000, 0,3) };
	INT32 XOffs1[16] = { 7, 6, 5, 4, 3, 2, 1, 0, 8*8+7, 8*8+6, 8*8+5, 8*8+4, 8*8+3, 8*8+2, 8*8+1, 8*8+0 };
	INT32 YOffs1[16] = { 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 16*8, 17*8, 18*8, 19*8, 20*8, 21*8, 22*8, 23*8 };

	GfxDecode4Bpp(0x0400, 3, 16, 16, Planes1, XOffs1, YOffs1, 0x100, DrvGfxTMP0, &DrvGfxTMP1[0x30000]);
	memcpy(DrvGfxROM2,&DrvGfxTMP1[0x40000],0x18000);
	GfxDecode4Bpp(0x0400, 3, 16, 16, Planes1, XOffs1, YOffs1, 0x100, DrvGfxTMP1, &DrvGfxTMP1[0x30000]);
	memcpy(DrvGfxROM3,&DrvGfxTMP1[0x40000],0x18000);
}

static void bankswitch(INT32 data)
{
	DrvZ80Bank0 = (data & 0x40);
// 1 bank de 16k																  
	if(DrvZ80Bank0)
	{
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
	if(address >= 0xf000 && address <= 0xf01f)
	{
		DrvSprRAM0[address-0xf000] = data;
		return;
	}

	if(address >= 0xf020 && address <= 0xf41f/*0xf3ff*/)
	{
		address-=0xf020;
		if(DrvFgVidRAM[address] != data)
		{
			DrvFgVidRAM[address] = data;
			is_fg_dirty[address] = 1;
		}
		return;
	}

	if(address >= 0xf420 && address <= 0xf7ff)
	{
		address-=0xf420;

		if(DrvFgColRAM[address]!=data || is_fg_dirty[address]==1)
		{
			is_fg_dirty[address] = 0;
			DrvFgColRAM[address]=data;
			UINT32 color,flipx;
			UINT32 x = map_offset_lut[address]; //(sx| sy)<<1;
		// char set #1
			color = data & 0x0f;
			flipx = (data & 0x10) <<10;		
			ss_map[x] = flipx | color;
			INT32 code = DrvFgVidRAM[address] + 256 * ((data>>5) & 0x07);
			ss_map[x+1] =code; 
		}
		return;
	}

	if(address >= 0xf800 && address <= 0xf81f)
	{
		DrvSprRAM1[address-0xf800] = data;
		return;
	}

	if(address >= 0xf820 && address <= 0xfc1f/*0xfbff*/)
	{
		address-=0xf820;
		if(DrvBgVidRAM[address]!=data)
		{
			DrvBgVidRAM[address] = data;
			UINT32 x = map_offset_lut[address]; //(sx| sy)<<1;
			UINT32 code = data + (256 * ((DrvBgColRAM[address]>>5) & 0x07));
			ss_map2[x+1] =code+0x2000; 
		}
		return;
	}

	if(address >= 0xfc20 && address <= 0xffff)
	{
		address-=0xfc20;

//		if(DrvBgColRAM[address]!=data)
		{
//			is_bg_dirty[address] = 0;
			DrvBgColRAM[address]=data;
			UINT32 color,flipx;
			UINT32 x = map_offset_lut[address]; //(sx| sy)<<1;
		// char set #2 
			color = data & 0x0f;
			flipx = (data & 0x10) <<10;
			ss_map2[x] = flipx | color;
			UINT32 code = DrvBgVidRAM[address] + 256 * ((data>>5) & 0x07);
			ss_map2[x+1] =code+0x2000; 
		}
		return;
	}
}

static unsigned char __fastcall appoooh_read(unsigned short address)
{
	if(address >= 0xf000 && address <= 0xf01f)
	{
		return DrvSprRAM0[address-0xf000];
	}

	if(address >= 0xf020 && address <= 0xf41f/*0xf3ff*/)
	{
		return DrvFgVidRAM[address-0xf020];
	}

	if(address >= 0xf420 && address <= 0xf7ff)
	{
		return DrvFgColRAM[address-0xf420];
	}

	if(address >= 0xf800 && address <= 0xf81f)
	{
		return DrvSprRAM1[address-0xf800];
	}

	if(address >= 0xf820 && address <= 0xfc1f/*0xfbff*/)
	{
		return DrvBgVidRAM[address-0xf820];
	}

	if(address >= 0xfc20 && address <= 0xffff)
	{
		return DrvBgColRAM[address-0xfc20];
	}
	return 0;
}

inline static INT32 DrvMSM5205SynchroniseStream(INT32 nSoundRate)
{

	return (INT32)((double)CZetTotalCycles() * nSoundRate / (nCyclesTotal));
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
	UINT32 i = 3 + spriteoffset;

	for (INT32 offs = 0x20 - 4; offs >= 0; offs -= 4)
	{
		INT32 sy    = 240 - sprite[offs + 0];
		UINT32 code  = (sprite[offs + 1] >> 2) + ((sprite[offs + 2] >> 5) & 0x07) * 0x40;
		UINT32 color = (sprite[offs + 2] & 0x0f);    /* TODO: bit 4 toggles continuously, what is it? */
		INT32 sx    = sprite[offs + 3];
		UINT32 flipx = (sprite[offs + 1] & 0x01) << 4;
		code		  &= 0x1ff;
		code		 += tileoffset;

		if(sx >= 248)
			sx -= 256;

		ss_sprite[i].ax		= sx;
		ss_sprite[i].ay		= sy;
		ss_sprite[i].control    = ( JUMP_NEXT | FUNC_NORMALSP | flipx);
		ss_sprite[i].charAddr = 0x220 +(code << 4); //charaddr_lut[code&0x3ff]; //0x220 +(code << 4);//
		ss_sprite[i].color     = (color<<4);
		++i;
	}
}

static INT32 DrvDraw()
{
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

static INT32 DrvLoadRoms()
{
	for (INT32 i = 0; i < 5; i++)
		if (BurnLoadRom(DrvMainROM + i * 0x2000, i +  0, 1)) return 1;

 	if (BurnLoadRom(DrvMainROM + 0x0a000,  5, 1)) return 1;	// epr-5911.bin
	if (BurnLoadRom(DrvMainROM + 0x0c000,  6, 1)) return 1;	// epr-5913.bin

	if (BurnLoadRom(DrvMainROM + 0x10000,  7, 1)) return 1;	// epr-5912.bin
	if (BurnLoadRom(DrvMainROM + 0x12000,  8, 1)) return 1;	// epr-5914.bin

	if (BurnLoadRom(DrvGfxTMP0 + 0x00000,  9, 1)) return 1;	// epr-5895.bin
	if (BurnLoadRom(DrvGfxTMP0 + 0x04000, 10, 1)) return 1;	// epr-5896.bin
	if (BurnLoadRom(DrvGfxTMP0 + 0x08000, 11, 1)) return 1;	// epr-5897.bin

	if (BurnLoadRom(DrvGfxTMP1 + 0x00000, 12, 1)) return 1;	// epr-5898.bin
	if (BurnLoadRom(DrvGfxTMP1 + 0x04000, 13, 1)) return 1;	// epr-5899.bin
	if (BurnLoadRom(DrvGfxTMP1 + 0x08000, 14, 1)) return 1;	// epr-5900.bin

	if (BurnLoadRom(DrvColPROM + 0x0000, 15, 1)) return 1;	// pr5921.prm
	if (BurnLoadRom(DrvColPROM + 0x0020, 16, 1)) return 1;	// pr5922.prm
	if (BurnLoadRom(DrvColPROM + 0x0120, 17, 1)) return 1;	// pr5923.prm

	if (BurnLoadRom(DrvSoundROM + 0x0000, 18, 1)) return 1;	// epr-5901.bin
	if (BurnLoadRom(DrvSoundROM + 0x2000, 19, 1)) return 1;	// epr-5902.bin
	if (BurnLoadRom(DrvSoundROM + 0x4000, 20, 1)) return 1;	// epr-5903.bin
	if (BurnLoadRom(DrvSoundROM + 0x6000, 21, 1)) return 1;	// epr-5904.bin
	if (BurnLoadRom(DrvSoundROM + 0x8000, 22, 1)) return 1;	// epr-5905.bin

	return 0;
}

static INT32 DrvRobowresLoadRoms()
{
	if (BurnLoadRom(DrvMainROM + 0x00000, 0, 1)) return 1; //epr-7540.13d
	if (BurnLoadRom(DrvMainROM + 0x08000, 1, 1)) return 1; //epr-7541.14d
	if (BurnLoadRom(DrvMainROM + 0x14000, 2, 1)) return 1; //epr-7542.15d
	memset(DrvMainROM + 0x0e000, 0, 0x2000); // right?? -dink

	memcpy (DrvMainROM + 0x10000, DrvMainROM + 0x16000, 0x4000);

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

static INT32 DrvCommonInit()
{
	nCyclesTotal = 3072000;

	CZetInit(1);
	CZetOpen(0);

	CZetMapArea(0x0000, 0x7fff, 0, DrvMainROM + 0x0000);
	CZetMapArea(0x0000, 0x7fff, 2, DrvMainROM + 0x0000);
	CZetMapArea(0x8000, 0x9fff, 0, DrvMainROM + 0x8000);
	CZetMapArea(0x8000, 0x9fff, 2, DrvMainROM + 0x8000);

 	bankswitch(0);

	if (game_select == 1) 
	{ // map decoded fetch for robowres
		CZetMapArea2(0x0000, 0x7fff, 0, DrvFetch, DrvMainROM);
		CZetMapArea2(0x0000, 0x7fff, 2, DrvFetch, DrvMainROM);
	}

	CZetMapArea(0xe000, 0xe7ff, 0, DrvRAM0);
	CZetMapArea(0xe000, 0xe7ff, 1, DrvRAM0);
	CZetMapArea(0xe000, 0xe7ff, 2, DrvRAM0);

	CZetMapArea(0xe800, 0xefff, 0, DrvRAM1);
	CZetMapArea(0xe800, 0xefff, 1, DrvRAM1);
	CZetMapArea(0xe800, 0xefff, 2, DrvRAM1);

 	CZetSetWriteHandler(appoooh_write);
	CZetSetReadHandler(appoooh_read);
	CZetSetInHandler(appoooh_in);
	CZetSetOutHandler(appoooh_out);

	CZetClose();
	
	SN76489Init(0, 18432000 / 6, 0);
	SN76489Init(1, 18432000 / 6, 1);
	SN76489Init(2, 18432000 / 6, 1);

	MSM5205Init(0, DrvMSM5205SynchroniseStream, 384000, DrvMSM5205Int, MSM5205_S64_4B, 1, 0.50);

	make_lut();
	DrvDoReset();
	return 0;
}

static void sega_decode_2(UINT8 *pDest, UINT8 *pDestDec, const UINT8 xor_table[128],const int swap_table[128])
{
	int A;
	UINT8 swaptable[24][4] =
	{
		{ 6,4,2,0 }, { 4,6,2,0 }, { 2,4,6,0 }, { 0,4,2,6 },
		{ 6,2,4,0 }, { 6,0,2,4 }, { 6,4,0,2 }, { 2,6,4,0 },
		{ 4,2,6,0 }, { 4,6,0,2 }, { 6,0,4,2 }, { 0,6,4,2 },
		{ 4,0,6,2 }, { 0,4,6,2 }, { 6,2,0,4 }, { 2,6,0,4 },
		{ 0,6,2,4 }, { 2,0,6,4 }, { 0,2,6,4 }, { 4,2,0,6 },
		{ 2,4,0,6 }, { 4,0,2,6 }, { 2,0,4,6 }, { 0,2,4,6 },
	};

	UINT8 *rom = DrvMainROM;
	UINT8 *decrypted = DrvFetch; 

	for (A = 0x0000;A < 0x8000;A++)
	{
		int row;
		UINT8 src;
		const UINT8 *tbl;


		src = rom[A];

		/* pick the translation table from bits 0, 3, 6, 9, 12 and 14 of the address */
		row = (A & 1) + (((A >> 3) & 1) << 1) + (((A >> 6) & 1) << 2)
				+ (((A >> 9) & 1) << 3) + (((A >> 12) & 1) << 4) + (((A >> 14) & 1) << 5);

		/* decode the opcodes */
		tbl = swaptable[swap_table[2*row]];
		decrypted[A] = BITSWAP08(src,7,tbl[0],5,tbl[1],3,tbl[2],1,tbl[3]) ^ xor_table[2*row];

		/* decode the data */
		tbl = swaptable[swap_table[2*row+1]];
		rom[A] = BITSWAP08(src,7,tbl[0],5,tbl[1],3,tbl[2],1,tbl[3]) ^ xor_table[2*row+1];
	}
}

void sega_decode_315(UINT8 *pDest, UINT8 *pDestDec)
{
	static const UINT8 xor_table[128] =
	{
		0x00,0x45,0x41,0x14,0x10,0x55,0x51,0x01,0x04,0x40,0x45,0x11,0x14,0x50,
		0x00,0x05,0x41,0x44,0x10,0x15,0x51,0x54,0x04,
		0x00,0x45,0x41,0x14,0x10,0x55,0x05,0x01,0x44,0x40,0x15,0x11,0x54,0x50,
		0x00,0x05,0x41,0x44,0x10,0x15,0x51,0x01,0x04,
		0x40,0x45,0x11,0x14,0x50,0x55,0x05,0x01,0x44,0x40,0x15,0x11,0x54,0x04,
		0x00,0x45,0x41,0x14,0x50,
		0x00,0x05,0x41,0x44,0x10,0x15,0x51,0x54,0x04,
		0x00,0x45,0x41,0x14,0x50,0x55,0x05,0x01,0x44,0x40,0x15,0x11,0x54,0x50,
		0x00,0x05,0x41,0x44,0x10,0x55,0x51,0x01,0x04,
		0x40,0x45,0x11,0x14,0x50,0x55,0x05,0x01,0x44,0x40,0x15,0x51,0x54,0x04,
		0x00,0x45,0x41,0x14,0x10,0x55,0x51,0x01,0x04,
		0x40,0x45,0x11,0x54,0x50,0x00,0x05,0x41,
	};

	static const int swap_table[128] =
	{
			8, 9,11,13,15, 0, 2, 4, 6,
			8, 9,11,13,15, 1, 2, 4, 6,
			8, 9,11,13,15, 1, 2, 4, 6,
			8, 9,11,13,15, 1, 2, 4, 6,
			8,10,11,13,15, 1, 2, 4, 6,
			8,10,11,13,15, 1, 2, 4, 6,
			8,10,11,13,15, 1, 3, 4, 6,
			8,
			7, 1, 2, 4, 6, 0, 1, 3, 5,
			7, 1, 2, 4, 6, 0, 1, 3, 5,
			7, 1, 2, 4, 6, 0, 2, 3, 5,
			7, 1, 2, 4, 6, 0, 2, 3, 5,
			7, 1, 2, 4, 6, 0, 2, 3, 5,
			7, 1, 3, 4, 6, 0, 2, 3, 5,
			7, 1, 3, 4, 6, 0, 2, 4, 5,
			7,
	};
	sega_decode_2(pDest, pDestDec, xor_table, swap_table);
}
 
static INT32 DrvRobowresInit()
{
	DrvInitSaturn();
	game_select = 1;
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
		
 	sega_decode_315( DrvMainROM, DrvFetch );
	DrvRobowresPaletteInit();
	DrvRobowresGfxDecode();
	DrvCommonInit();

	return 0;
}

static INT32 DrvInit()
{
	DrvInitSaturn();

	game_select = 0;
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
	if(DrvLoadRoms()) return 1;
	DrvPaletteInit();
	DrvGfxDecode();
	DrvCommonInit();
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
		UINT32	sx = i & 0x1f;
		UINT32	sy = (i<<1) & (~0x3f);
		map_offset_lut[i] = (sx| sy)<<1;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DrvInitSaturn()
{
	SPR_InitSlaveSH();

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

	memset((Uint8 *)ss_map  ,0,0x2000);
	memset((Uint8 *)ss_map2,0,0x2000);
	memset(is_fg_dirty,1,0x400);

	SprSpCmd *ss_spritePtr;
	unsigned int i = 3;
	
	for (i = 3; i <nBurnSprites; i++) 
	{
		ss_spritePtr				= &ss_sprite[i];
		ss_spritePtr->control   = ( JUMP_NEXT | FUNC_NORMALSP);
		ss_spritePtr->drawMode  = ( ECD_DISABLE | COMPO_REP);	// 16 couleurs
		ss_spritePtr->charSize  = 0x210;  //0x100 16*16
	}
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
	drawWindow(0,224,240,0,64);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 DrvExit()
{
	SPR_InitSlaveSH();
	CZetExit();

	MSM5205Exit();

	free (AllMem);

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void dummy()
{

}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 DrvFrame()
{
	if (DrvReset) {
		DrvDoReset();
	}

	memset (DrvInputs, 0x00, 3);

	for (INT32 i = 0; i < 8; i++) {
		DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
		DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
		DrvInputs[2] ^= (DrvJoy3[i] & 1) << i;

	}
	UINT32 nInterleave = SOUND_LEN/2; //(hz==50)?128:100; //100 for 60hz MSM5205CalcInterleave(0, nCyclesTotal);//128
	UINT32 cycles = nCyclesTotal / 60 / nInterleave;
	CZetNewFrame();

//	CZetOpen(0);

	for (INT32 i = 0; i < nInterleave; i++) {

		CZetRun(cycles);
		if (interrupt_enable && i == (nInterleave - 1))
			CZetNmi();
		SPR_WaitEndSlaveSH();
	  	SPR_RunSlaveSH((PARA_RTN*)MSM5205Update, NULL);
//		MSM5205Update();
	}
//	CZetClose();
	DrvDraw();

	signed short *nSoundBuffer = (signed short *)(0x25a20000+nSoundBufferPos*(sizeof(signed short)));
	SN76496Update(0, nSoundBuffer, SOUND_LEN);
	SN76496Update(1, nSoundBuffer, SOUND_LEN);
#if 1
	SN76496Update(2, nSoundBuffer, SOUND_LEN);
	MSM5205Render(0, nSoundBuffer, SOUND_LEN);
#else
	SN76496MSM5205Update(2, nSoundBuffer, SOUND_LEN);
#endif
	nSoundBufferPos+=(SOUND_LEN); // DOIT etre deux fois la taille copiee
	if(nSoundBufferPos>=0x2000)//RING_BUF_SIZE)
	{
		PCM_Task(pcm); // bon emplacement
		nSoundBufferPos=0;
	}
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
