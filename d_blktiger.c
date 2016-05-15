// FB Alpha Black Tiger driver module
// Based on MAME driver by Paul Leaman

//#include "tiles_generic.h"
//#include "z80_intf.h"
#include "snd/burn_ym2203.h"
//#include "bitswap.h"
#include "d_blktiger.h"
//#define RAZE 1
//#define BG_BANK 1
/*
<vbt1> where and when you update the nbg map
<vbt1> in loop, during vblank in , during vblank out ?
<vbt1> with dma, etc
<CyberWarriorX> vblank in
<CyberWarriorX> vblank in is the start of the vblank. vblank out is the end of it
*/
#define VDP2_BASE           0x25e00000
#define VDP2_REGISTER_BASE  (VDP2_BASE+0x180000)
#define BGON    (*(volatile unsigned short *)(VDP2_REGISTER_BASE+0x20))
#define PLSZ    (*(volatile unsigned short *)(VDP2_REGISTER_BASE+0x3a))
#define VDP2_VRAM           VDP2_BASE
#define VDP2_CRAM           (VDP2_BASE+0x100000)


#define PNCN1   (*(volatile unsigned short *)(VDP2_REGISTER_BASE+0x32))

static UINT16 remap4to16_lut[256];
static UINT16 remap16_lut[768];
static UINT16 cram_lut[4096];
static UINT16 fg_map_lut[0x400];
static UINT16 bg_map_lut2x1[0x2000];
static UINT16 bg_map_lut2x2[0x2000];
static UINT16 *bg_map_lut = NULL;
//static UINT32 bg_map_dirty[0x4000];

//#define RAZE 1
#define INT_DIGITS 19
char *itoa(i)
     int i;
{
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {			/* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}


int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvBlktiger = {
		"blktiger", "blktgr",
		"Black Tiger",
		blktigerRomInfo, blktigerRomName, DrvInputInfo, DrvDIPInfo,
		DrvInit, DrvExit, DrvFrame, NULL //, DrvScan, &DrvRecalc, 0x400,
	};
	
	if (strcmp(nBurnDrvBlktiger.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvBlktiger,sizeof(struct BurnDriver));

	ss_reg   = (SclNorscl *)SS_REG;
	ss_regs = (SclSysreg *)SS_REGS;
	ss_regd = (SclDataset *)SS_REGD;
}

static void palette_write(INT32 offset)
{
//	UINT8 r,g,b;
	UINT16 data = (DrvPalRAM[offset]) | (DrvPalRAM[offset | 0x400] << 8);

	if(offset >=0x300)
	{
// fg	 offset 300
		unsigned short position = remap4to16_lut[offset&0xff];
		colBgAddr2[position] = cram_lut[data];//RGB(r>>3, g>>3, b>>3);
	}
	else
	{
// bg 0x000-0xff
// sprites 0x200-0x2ff
		unsigned short position = remap16_lut[offset];
		DrvPalette[position] = cram_lut[data];//RGB(r>>3, g>>3, b>>3);
	}

}

static void DrvRomBankswitch(INT32 bank)
{
	*DrvRomBank = bank & 0x0f;

	INT32 nBank = 0x10000 + (bank & 0x0f) * 0x4000;

	CZetMapArea(0x8000, 0xbfff, 0, DrvZ80ROM0 + nBank);
	CZetMapArea(0x8000, 0xbfff, 2, DrvZ80ROM0 + nBank);
}

static void DrvVidRamBankswitch(INT32 bank)
{
	*DrvVidBank = bank & 0x03;

	INT32 nBank = (bank & 3) * 0x1000;

char toto[100];
char *titi = &toto[0];
titi=itoa(bank & 0x03);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"bank            ",4,10);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)titi,40,10);

	CZetMapArea(0xc000, 0xcfff, 0, DrvBgRAM + nBank);
//	CZetMapArea(0xc000, 0xcfff, 1, DrvBgRAM + nBank);
	CZetMapArea(0xc000, 0xcfff, 2, DrvBgRAM + nBank);
}

void __fastcall blacktiger_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xd800) 				  // 	CZetMapArea(0xd800, 0xdfff, 0, DrvPalRAM);
	{
		if(DrvPalRAM[address & 0x7ff] != data)
		{
			DrvPalRAM[address & 0x7ff] = data;
			palette_write(address & 0x3ff);
		}
		return;
	}

	if (address >= 0xd000 && address <= 0xd3ff) 
	{
		address &= 0x3ff;
		if(DrvTxRAM[address] != data)
		{
			DrvTxRAM[address] = data;
			UINT32 attr  =	DrvTxRAM[address | 0x400];
			UINT32 code  = data | ((attr & 0xe0) << 3);

			UINT32 x = fg_map_lut[address];
			ss_map[x] = (attr & 0x1f);
			ss_map[x+1] =code; 
		}
		return;
	}

	if (address >= 0xd400 && address <= 0xd7ff) 
	{
		address &= 0x7ff;
		if(DrvTxRAM[address] != data)
		{
			DrvTxRAM[address] = data;
			UINT32 code  = DrvTxRAM[address & 0x3ff] | ((data & 0xe0) << 3);

			UINT32 x = fg_map_lut[(address & 0x3ff)];
			ss_map[x] = (data & 0x1f);
			ss_map[x+1] =code;
		}
		return;
	}
// CZetMapArea(0xc000, 0xcfff, 1, DrvBgRAM + nBank);
	if (address >= 0xc000  && address <= 0xcfff) 
	{
		UINT32 nBank = (*DrvVidBank) * 0x1000;
		address = (address & 0xfff) + nBank;
		if(DrvBgRAM[address]!=data)
		{
			DrvBgRAM[address]=data;
			updateBgTile(*DrvScreenLayout, address>>1);
		}
		return;
	}
	return;
}

UINT8 __fastcall blacktiger_read(UINT16 address)
{
	return 0;
}

void __fastcall blacktiger_out(UINT16 port, UINT8 data)
{
	switch (port & 0xff)
	{
		case 0x00:
		{
			*soundlatch = data;
		}
		return;

		case 0x01:
			DrvRomBankswitch(data);
		return;

		case 0x03:
			if (DrvDips[2] & 1) {
				*coin_lockout = ~data << 6;
			}
		return;

		case 0x04:
			if (data & 0x20) {
				CZetClose();
#ifndef RAZE
				CZetOpen(1);
				CZetReset();
				CZetClose();
#else
				z80_reset();
#endif
				CZetOpen(0);
			}

			*flipscreen  =  data & 0x40;
			*DrvFgEnable = ~data & 0x80;
/*
char toto[100];
char *titi = &toto[0];
titi=itoa(~data & 0x80);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"fgenab            ",4,20);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)titi,40,20);
*/			ss_regs->dispenbl &= 0xfffb;
			ss_regs->dispenbl |= (*DrvFgEnable >> 5) & 0x0004;
			BGON = ss_regs->dispenbl;

		return;

		case 0x06:
			watchdog = 0;
		return;

		case 0x08:
			*DrvScrollx = (*DrvScrollx & 0xff00) | data;
			ss_reg->n1_move_y =  (((*DrvScrolly)+32)<<16) ;	  //  � remettre
//			ss_reg->n1_move_y =  ((1152)<<16) ;	  //  � remettre
		return;

		case 0x09:
			*DrvScrollx = (*DrvScrollx & 0x00ff) | (data << 8);
			ss_reg->n1_move_y =  (((*DrvScrolly)+32)<<16) ;	  // � remettre
//			ss_reg->n1_move_y =  ((1152)<<16) ;	  // � remettre
		return;

		case 0x0a:
			*DrvScrolly = (*DrvScrolly & 0xff00) | data;
			ss_reg->n1_move_x =  (((*DrvScrollx))<<16) ;
//			ss_reg->n1_move_x =  ((0)<<16) ;
		return;

		case 0x0b:
			*DrvScrolly = (*DrvScrolly & 0x00ff) | (data << 8);
			ss_reg->n1_move_x =  (((*DrvScrollx))<<16) ;
//			ss_reg->n1_move_x =  ((0)<<16) ;
		return;

		case 0x0c:
			*DrvSprEnable = ~data & 0x02;
			*DrvBgEnable  = ~data & 0x04;
  /*
			ss_regs->dispenbl &= 0xfffD;
			ss_regs->dispenbl |= (*DrvBgEnable >> 1) & 0x0002;
			BGON = ss_regs->dispenbl;			 */
/*
titi=itoa(~data & 0x02);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"spenab            ",4,30);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)titi,40,30);

titi=itoa(~data & 0x04);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"bgenab            ",4,40);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)titi,40,40);
*/		
		return;

		case 0x0d:
			DrvVidRamBankswitch(data);
		return;

		case 0x0e:
		{
			UINT32 plate_addr[4];

			if(data)		  // 1 = 128x64, 0 = 64x128
			{
				*DrvScreenLayout = 1;
//				ss_regd->platesize &= 0xfff3;
//				ss_regd->platesize |= 0x0004;		
				bg_map_lut = &bg_map_lut2x1[0];
	 /*
				plate_addr[0] = (Uint32)SS_MAP2;
				plate_addr[1] = (Uint32)(SS_MAP2+0x800);
				plate_addr[2] = (Uint32)(SS_MAP2+0x400);	 // good	  0x400
				plate_addr[3] = (Uint32)(SS_MAP2+0xC00);	 */  
			}
			else
			{
				*DrvScreenLayout = 0;
//				ss_regd->platesize &= 0xfff3;
//				ss_regd->platesize |= 0x000c;	 
				bg_map_lut = &bg_map_lut2x2[0];
 /*
				plate_addr[0] = (Uint32)(SS_MAP2);//////
				plate_addr[1] = (Uint32)(SS_MAP2);//////
				plate_addr[2] = (Uint32)(SS_MAP2+0x800);	 // good	  0x400
				plate_addr[3] = (Uint32)(SS_MAP2+0x800);  */
			}
			PLSZ = ss_regd->platesize;
 
			UINT16	*map = 0;
			UINT16	temp;
			map = &ss_regd->normap[2];

			for(UINT32 i = 0; i < 2; i++) 
			{
				map[i] = (0x003f & ((plate_addr[i * 2] - VDP2_BASE)	/ 0x800));
				temp = (0x003f & ((plate_addr[i * 2 + 1] - VDP2_BASE)	/ 0x800)) << 8;
				map[i] |= (temp & 0x3f00);
			}
		}
/*
titi=itoa(data ? 1 : 0);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"scrlayou            ",4,50);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)titi,40,50);
*/
		return;
	}
}

UINT8 __fastcall blacktiger_in(UINT16 port)
{
	switch (port & 0xff)
	{
		case 0x00:
		case 0x01:
		case 0x02:
			return DrvInputs[port & 3];

		case 0x03:
		case 0x04:
			return DrvDips[~port & 1];

		case 0x05:
			return 0x01;

		case 0x07:
			return CZetDe(-1) >> 8;
	}

	return 0;
}

void __fastcall blacktiger_sound_write(UINT16 address, UINT8 data)
{
	switch (address)
	{
		case 0xe000:
#ifdef SND
			BurnYM2203Write(0, 0, data);
#endif
			return;

		case 0xe001:
#ifdef SND
			BurnYM2203Write(0, 1, data);
#endif
			return;

		case 0xe002:
#ifdef SND
			BurnYM2203Write(1, 0, data);
#endif
			return;

		case 0xe003:
#ifdef SND
			BurnYM2203Write(1, 1, data);
#endif
			return;
	}
}

UINT8 __fastcall blacktiger_sound_read(UINT16 address)
{
	switch (address)
	{
		case 0xc800:
			return *soundlatch;

		case 0xe000:
#ifdef SND
			return BurnYM2203Read(0, 0);
#else
		return 0;
#endif

		case 0xe001:
#ifdef SND
			return BurnYM2203Read(0, 1);
#else
		return 0;
#endif

		case 0xe002:
#ifdef SND
			return BurnYM2203Read(1, 0);
#else
			return 0;
#endif

		case 0xe003:
#ifdef SND
			return BurnYM2203Read(1, 1);
#else
			return 0;
#endif
	}

	return 0;
}

static INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;

	DrvZ80ROM0	= Next; Next += 0x050000;
	DrvZ80ROM1	= Next; Next += 0x008000;

	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	DrvGfxROM0	= SS_CACHE; //Next; Next += 0x020000;
//	DrvGfxROM1	= SS_CACHE + 0x010000;
	DrvGfxROM1	= SS_CACHE + 0x010000;
	DrvGfxROM2	= (UINT8 *)(ss_vram+0x1100);

	DrvPalette	= (UINT16*)colBgAddr; //(UINT32*)Next; Next += 0x0400 * sizeof(UINT32);

	AllRam		= Next;

	DrvZ80RAM0	= Next; Next += 0x001e00;
	DrvZ80RAM1	= Next; Next += 0x000800;

	DrvPalRAM	= Next; Next += 0x000800;
	DrvTxRAM	= Next; Next += 0x000800;
	DrvBgRAM	= Next; Next += 0x004000;
	DrvSprRAM	= Next; Next += 0x001200;
	DrvSprBuf	= Next; Next += 0x001200;

	DrvScreenLayout	= Next; Next += 0x000001;
	DrvBgEnable	= Next; Next += 0x000001;
	DrvFgEnable	= Next; Next += 0x000001;
	DrvSprEnable	= Next; Next += 0x000001;

	DrvVidBank	= Next; Next += 0x000001;
	DrvRomBank	= Next; Next += 0x000001;

	DrvScrollx	= (UINT16*)Next; Next += 0x0001 * sizeof (UINT16);
	DrvScrolly	= (UINT16*)Next; Next += 0x0001 * sizeof (UINT16);

	soundlatch	= Next; Next += 0x000001;
	flipscreen	= Next; Next += 0x000001;
	coin_lockout	= Next; Next += 0x000001;

	RamEnd		= Next;

	MemEnd		= Next;

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*inline*/ static int readbit(const UINT8 *src, int bitnum)
{
	return src[bitnum / 8] & (0x80 >> (bitnum % 8));
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void tile16x16toSaturn (int num, unsigned char *pDest)
{
	int c;
	for (c = 0; c < num; c++) 
	{
		unsigned char new_tile[128];
		UINT8 *dpM = pDest + (c * 128);
		memcpy(new_tile,dpM,128);
		unsigned int i=0,j=0,k=0;

		for (k=0;k<128;k+=64)
		{
			dpM = pDest + ((c * 128)+k);

			for (i=0;i<32;i+=4,j+=8)
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
		}
	}
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

static INT32 DrvDoReset(INT32 full_reset)
{
	if (full_reset) {
		memset (AllRam, 0, RamEnd - AllRam);
	}

	CZetOpen(0);
	CZetReset();
	DrvRomBankswitch(1);
	DrvVidRamBankswitch(1);
	CZetClose();
#ifndef RAZE
	CZetOpen(1);
	CZetReset();
	CZetClose();
#else
	z80_reset();
#endif
#ifdef SND
	BurnYM2203Reset();
#endif
	watchdog = 0;

	return 0;
}

static INT32 DrvGfxDecode()
{
	INT32 Plane[4] = { ((0x40000 * 8) / 2) + 4, ((0x40000 * 8) / 2) + 0, 4, 0 };
	INT32 XOffs[16] = { 0, 1, 2, 3, 8+0, 8+1, 8+2, 8+3,
			16*16+0, 16*16+1, 16*16+2, 16*16+3, 16*16+8+0, 16*16+8+1, 16*16+8+2, 16*16+8+3 };
	INT32 YOffs[16] = { 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16 };

	UINT8 *tmp = (UINT8*)0x00200000; //malloc(0x40000);
//	if (tmp == NULL) {
//		return 1;
//	}
// texte
	memcpy (tmp, DrvGfxROM0, 0x08000);
// 	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvGfxDecode1          ",4,90);
	GfxDecode4Bpp(0x0800, 2,  8,  8, Plane + 2, XOffs, YOffs, 0x080, tmp, DrvGfxROM0);

	for (int i=0;i<0x10000;i++ )
	{
		if ((DrvGfxROM0[i]& 0x03)     ==0x00)DrvGfxROM0[i] = DrvGfxROM0[i] & 0x30 | 0x3;
		else if ((DrvGfxROM0[i]& 0x03)==0x03) DrvGfxROM0[i] = DrvGfxROM0[i] & 0x30;

		if ((DrvGfxROM0[i]& 0x30)       ==0x00)DrvGfxROM0[i] = 0x30 | DrvGfxROM0[i] & 0x03;
		else if ((DrvGfxROM0[i]& 0x30)==0x30) DrvGfxROM0[i] = DrvGfxROM0[i] & 0x03;
	}

//bg
	memcpy (tmp, DrvGfxROM1, 0x40000);
// 	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvGfxDecode2          ",4,100);
//	GfxDecode4Bpp(0x0800, 4, 16, 16, Plane + 0, XOffs, YOffs, 0x200, tmp, DrvGfxROM1);
	GfxDecode4Bpp(0x0800, 4, 16, 16, Plane + 0, XOffs, YOffs, 0x200, tmp, DrvGfxROM1);
// 	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvGfxDecode3          ",4,110);

	tile16x16toSaturn(0x0800, DrvGfxROM1);
	
 	for (int i=0;i<0x40000;i++ )
	{
		if ((DrvGfxROM1[i]& 0x0f)     ==0x00)DrvGfxROM1[i] = DrvGfxROM1[i] & 0xf0 | 0xf;
		else if ((DrvGfxROM1[i]& 0x0f)==0x0f) DrvGfxROM1[i] = DrvGfxROM1[i] & 0xf0;

		if ((DrvGfxROM1[i]& 0xf0)       ==0x00)DrvGfxROM1[i] = 0xf0 | DrvGfxROM1[i] & 0x0f;
		else if ((DrvGfxROM1[i]& 0xf0)==0xf0) DrvGfxROM1[i] = DrvGfxROM1[i] & 0x0f;
	}
//sprites
	memcpy (tmp, DrvGfxROM2, 0x40000);
// 	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvGfxDecode4          ",4,120);

	GfxDecode4Bpp(0x0800, 4, 16, 16, Plane + 0, XOffs, YOffs, 0x200, tmp, DrvGfxROM2);

 	for (int i=0;i<0x40000;i++ )
	{
		if ((DrvGfxROM2[i]& 0x0f)     ==0x00)DrvGfxROM2[i] = DrvGfxROM2[i] & 0xf0 | 0xf;
		else if ((DrvGfxROM2[i]& 0x0f)==0x0f) DrvGfxROM2[i] = DrvGfxROM2[i] & 0xf0;

		if ((DrvGfxROM2[i]& 0xf0)       ==0x00)DrvGfxROM2[i] = 0xf0 | DrvGfxROM2[i] & 0x0f;
		else if ((DrvGfxROM2[i]& 0xf0)==0xf0) DrvGfxROM2[i] = DrvGfxROM2[i] & 0x0f;
	}

 //	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvGfxDecode5 end          ",4,130);

//	free (tmp);
	
	return 0;
}

static void DrvFMIRQHandler(INT32 irq, INT32 nStatus)
{
	if (nStatus & 1) {
		CZetSetIRQLine(0xff, CZET_IRQSTATUS_ACK);
	} else {
		CZetSetIRQLine(0,    CZET_IRQSTATUS_NONE);
	}
}

static INT32 DrvSynchroniseStream(INT32 nSoundRate)
{
	return (INT32)CZetTotalCycles() * nSoundRate / 3579545;
}

static double DrvGetTime()
{
	return (double)CZetTotalCycles() / 3579545;
}

static INT32 DrvInit()
{
	DrvInitSaturn();
	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)malloc(nLen)) == NULL)
	{
//		FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"malloc failed",4,80);
		return 1;
	}
	memset(AllMem, 0, nLen);
	MemIndex();

//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"BurnLoadRom 1        ",4,20);

	{
		for (INT32 i = 0; i < 5; i++) {
			if (BurnLoadRom(DrvZ80ROM0 + i * 0x10000, 0  + i, 1)) return 1;
		}
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"BurnLoadRom 2        ",4,30);
		if (BurnLoadRom(DrvZ80ROM1, 5, 1)) return 1;
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"BurnLoadRom 3        ",4,40);
		if (BurnLoadRom(DrvGfxROM0, 6, 1)) return 1;
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"BurnLoadRom 4        ",4,50);
		for (INT32 i = 0; i < 4; i++) {
			if (BurnLoadRom(DrvGfxROM1 + i * 0x10000, 7  + i, 1)) return 1;
			if (BurnLoadRom(DrvGfxROM2 + i * 0x10000, 11 + i, 1)) return 1;
		}
 //	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvGfxDecode          ",4,60);
		DrvGfxDecode();
	}
//  	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"CZetInit          ",4,70);
#ifndef BG_BANK
	drawWindow(0,224,240,0,64);
#endif

#ifndef RAZE
	CZetInit(2);
#else
	CZetInit(1);
#endif
	CZetOpen(0);
	CZetMapArea(0x0000, 0x7fff, 0, DrvZ80ROM0);
	CZetMapArea(0x0000, 0x7fff, 2, DrvZ80ROM0);
	CZetMapArea(0xd000, 0xd7ff, 0, DrvTxRAM);
//	CZetMapArea(0xd000, 0xd7ff, 1, DrvTxRAM);
	CZetMapArea(0xd000, 0xd7ff, 2, DrvTxRAM);
	CZetMapArea(0xd800, 0xdfff, 0, DrvPalRAM);
//	CZetMapArea(0xd800, 0xdfff, 1, DrvPalRAM);
	CZetMapArea(0xd800, 0xdfff, 2, DrvPalRAM);
	CZetMapArea(0xe000, 0xfdff, 0, DrvZ80RAM0);
	CZetMapArea(0xe000, 0xfdff, 1, DrvZ80RAM0);
	CZetMapArea(0xe000, 0xfdff, 2, DrvZ80RAM0);
	CZetMapArea(0xfe00, 0xffff, 0, DrvSprRAM);
	CZetMapArea(0xfe00, 0xffff, 1, DrvSprRAM);
	CZetMapArea(0xfe00, 0xffff, 2, DrvSprRAM);
	CZetSetWriteHandler(blacktiger_write);
	CZetSetReadHandler(blacktiger_read);
	CZetSetInHandler(blacktiger_in);
	CZetSetOutHandler(blacktiger_out);
	CZetClose();

//	CZetInit(1);
#ifndef RAZE
	CZetOpen(1);
	CZetMapArea(0x0000, 0x7fff, 0, DrvZ80ROM1);
	CZetMapArea(0x0000, 0x7fff, 2, DrvZ80ROM1);
	CZetMapArea(0xc000, 0xc7ff, 0, DrvZ80RAM1);
	CZetMapArea(0xc000, 0xc7ff, 1, DrvZ80RAM1);
	CZetMapArea(0xc000, 0xc7ff, 2, DrvZ80RAM1);
	CZetSetWriteHandler(blacktiger_sound_write);
	CZetSetReadHandler(blacktiger_sound_read);
	CZetClose();
#else
	z80_map_fetch (0x0000, 0x7fff, DrvZ80ROM1);
	z80_map_read  (0x0000, 0x7fff, DrvZ80ROM1);
	z80_map_fetch (0xc000, 0xc7ff, DrvZ80RAM1);
	z80_map_read  (0xc000, 0xc7ff, DrvZ80RAM1);
	z80_map_write (0xc000, 0xc7ff, DrvZ80RAM1);

	z80_add_write(0xe000, 0xe003, 1, (void *)&blacktiger_sound_write);
	z80_add_read(0xc800, 0xe003, 1, (void *)&blacktiger_sound_read); 
#endif

//	GenericTilesInit();
#ifdef SND
	BurnYM2203Init(2, 3579545, &DrvFMIRQHandler, DrvSynchroniseStream, DrvGetTime, 7680L);

	BurnTimerAttachZet(3579545);
#endif
//	BurnYM2203SetAllRoutes(0, 0.15, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetAllRoutes(1, 0.15, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetPSGVolume(0, 0.05);
//	BurnYM2203SetPSGVolume(1, 0.05);
//  	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvDoReset          ",4,80);

	DrvDoReset(1);

	for (INT32 i = 0; i < 0x400; i++) 
	{
		palette_write(i);
	}
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
// � passer en 1 word ?
	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_2X2;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN1WORD; //2word
	scfg.platesize     = SCL_PL_SIZE_2X2; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.patnamecontrl =  0x0008;// VRAM A0?
	scfg.flip          = SCL_PN_10BIT; // on force � 0
 //2x1
/*
	scfg.plate_addr[0] = (Uint32)SS_MAP2;
	scfg.plate_addr[1] = (Uint32)(SS_MAP2+0x800);
	scfg.plate_addr[2] = (Uint32)(SS_MAP2+0x400);	 // good	  0x400
	scfg.plate_addr[3] = (Uint32)(SS_MAP2+0xC00);
*/

	scfg.plate_addr[0] = (Uint32)(SS_MAP2);
	scfg.plate_addr[1] = (Uint32)(SS_MAP2+0x800);
	scfg.plate_addr[2] = (Uint32)(SS_MAP2+0x800);	 // good	  0x400
	scfg.plate_addr[3] = (Uint32)(SS_MAP2+0x800);
	
//2x2	 vertical
/*	scfg.plate_addr[0] = (Uint32)(SS_MAP2);
	scfg.plate_addr[1] = (Uint32)(SS_MAP2);
	scfg.plate_addr[2] = (Uint32)(SS_MAP2+0x800);	 // good	  0x400
	scfg.plate_addr[3] = (Uint32)(SS_MAP2+0x800);
*/
 /*
	scfg.plate_addr[4] = (Uint32)(SS_MAP2+0x800);
	scfg.plate_addr[6] = (Uint32)(SS_MAP2+0xC00);
*/
	SCL_SetConfig(SCL_NBG1, &scfg);
#ifdef BG_BANK
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.patnamecontrl =  0x000c;// VRAM A0?
	scfg.plate_addr[0] = (Uint32)ss_map3;
	SCL_SetConfig(SCL_NBG0, &scfg);
#endif
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

 #ifndef BG_BANK
 	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
	scfg.plate_addr[0] = (Uint32)SS_FONT;
	scfg.dispenbl      = ON;
//	scfg.dispenbl      = OFF;

	SCL_SetConfig(SCL_NBG0, &scfg);
#endif

	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_SPR,OFF);
	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);
#ifdef BG_BANK
	ss_regs->dispenbl &= 0xfeff;
#else
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);

	SCL_SetColRam(SCL_NBG0,8,8,palette);	 // vbt � remettre
#endif
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void make_lut(void)
{
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
//		remap4to16_lut[i] = delta;
		delta++; if ((delta & 3) == 0) delta += 12;
	}

	for (i = 0; i < 768;i++) 
	{
		if ((i%16)==0)
		{
			remap16_lut[i] = i+15;
		}
		else if (((i+1)%16)==0)
		{
			remap16_lut[i] = i-15;
		} 
		else	
		{
			remap16_lut[i] = i;
		}
	}

	for (i = 0; i < 0x400; i++)
	{
		int sy = ((i >> 5) & 0x1f)<<7;
		int sx = (i & 0x1f)<<1;
		fg_map_lut[i] = sy | sx;
	}
//-------------------------------------------------------------------------------
//#if 1
	INT32 sx, sy;
// x0-63,y0-15
	for (UINT32 offs = 0; offs < 0x100; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f);
		sy = ((offs) >> 4);				  // page 0 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x100; offs < 0x200; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+16;
		sy = ((offs-0x100) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x200; offs < 0x300; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x400;
		sy = ((offs-0x200) >> 4);				  // page 2 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x300; offs < 0x400; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x400+16;
		sy = ((offs-0x300) >> 4);				  // page 3 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}
// x0-63,y16-31
	for (UINT32 offs = 0x400; offs < 0x500; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f);
		sy = ((offs-0x300) >> 4);				  // page 0 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x500; offs < 0x600; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+16;
		sy = ((offs-0x400) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x600; offs < 0x700; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x400;
		sy = ((offs-0x500) >> 4);				  // page 2 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x700; offs < 0x800; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x400+16;
		sy = ((offs-0x600) >> 4);				  // page 3 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}
// x0-63,y32-47
	for (UINT32 offs = 0x800; offs < 0x900; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800+0;
		sy = ((offs-0x800) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x900; offs < 0xA00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800+16;
		sy = ((offs-0x900) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0xA00; offs < 0xB00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800+0x400;
		sy = ((offs-0xA00) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0xB00; offs < 0xC00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800+0x400+16;
		sy = ((offs-0xB00) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}
// x0-63,y48-63
	for (UINT32 offs = 0xC00; offs < 0xD00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800;
		sy = ((offs-0xB00) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0xD00; offs < 0xE00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800+16;
		sy = ((offs-0xC00) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0xE00; offs < 0xF00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800+0x400;
		sy = ((offs-0xD00) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0xF00; offs < 0x1000; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800+0x400+16;
		sy = ((offs-0xE00) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}
// x64-127-63,y0-15
	for (UINT32 offs = 0x1000; offs < 0x1100; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000;
		sy = ((offs-0x1000) >> 4);				  // page 0 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1100; offs < 0x1200; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+16;
		sy = ((offs-0x1100) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1200; offs < 0x1300; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x400;
		sy = ((offs-0x1200) >> 4);				  // page 2 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1300; offs < 0x1400; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x400+16;
		sy = ((offs-0x1300) >> 4);				  // page 3 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}
// x64-127,y16-31
	for (UINT32 offs = 0x1400; offs < 0x1500; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000;
		sy = ((offs-0x1300) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}
 
	for (UINT32 offs = 0x1500; offs < 0x1600; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+16;
		sy = ((offs-0x1400) >> 4);	  // page 9 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1600; offs < 0x1700; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x400;
		sy = ((offs-0x1500) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}
	
	for (UINT32 offs = 0x1700; offs < 0x1800; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x400+16;
		sy = ((offs-0x1600) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}
// x64-127,y32-47
	for (UINT32 offs = 0x1800; offs < 0x1900; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800+0;
		sy = ((offs-0x1800) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1900; offs < 0x1A00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800+16;
		sy = ((offs-0x1900) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1A00; offs < 0x1B00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800+0x400;
		sy = ((offs-0x1A00) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1B00; offs < 0x1C00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800+0x400+16;
		sy = ((offs-0x1B00) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}
// x64-127,y48-63
	for (UINT32 offs = 0x1C00; offs < 0x1D00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800;
		sy = ((offs-0x1B00) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1D00; offs < 0x1E00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800+16;
		sy = ((offs-0x1C00) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1E00; offs < 0x1F00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800+0x400;
		sy = ((offs-0x1D00) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1F00; offs < 0x2000; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800+0x400+16;
		sy = ((offs-0x1E00) >> 4);				  // page 1 16x16
		bg_map_lut2x2[offs] = sx|(sy<<5);
	}
//#else
//	INT32 sx, sy;
// x0-63,y0-15
	for (UINT32 offs = 0; offs < 0x100; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f);
		sy = ((offs) >> 4);				  // page 0 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x100; offs < 0x200; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+16;
		sy = ((offs-0x100) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x200; offs < 0x300; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x400;
		sy = ((offs-0x200) >> 4);				  // page 2 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x300; offs < 0x400; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x400+16;
		sy = ((offs-0x300) >> 4);				  // page 3 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}
//---------------------------------------------------------------------------------
// x0-63,y16-31
	for (UINT32 offs = 0x800; offs < 0x900; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f);
		sy = ((offs-0x700) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}
 
	for (UINT32 offs = 0x900; offs < 0xA00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+16;
		sy = ((offs-0x800) >> 4);	  // page 9 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0xA00; offs < 0xB00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x400;
		sy = ((offs-0x900) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}
	
	for (UINT32 offs = 0xB00; offs < 0xC00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x400+16;
		sy = ((offs-0xA00) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}
//---------------------------------------------------------------------------------
// x0-63,y32-47
	for (UINT32 offs = 0x1000; offs < 0x1100; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800+0;
		sy = ((offs-0x1000) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1100; offs < 0x1200; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800+16;
		sy = ((offs-0x1100) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1200; offs < 0x1300; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800+0x400;
		sy = ((offs-0x1200) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1300; offs < 0x1400; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800+0x400+16;
		sy = ((offs-0x1300) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}
//---------------------------------------------------------------------------------
// x0-63,y48-63
	for (UINT32 offs = 0x1800; offs < 0x1900; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800;
		sy = ((offs-0x1700) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1900; offs < 0x1A00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800+16;
		sy = ((offs-0x1800) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1A00; offs < 0x1B00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800+0x400;
		sy = ((offs-0x1900) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1B00; offs < 0x1C00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x800+0x400+16;
		sy = ((offs-0x1A00) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}
//---------------------------------------------------------------------------------
// x64-127-63,y0-15
	for (UINT32 offs = 0x400; offs < 0x500; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000;
		sy = ((offs-0x400) >> 4);				  // page 0 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x500; offs < 0x600; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+16;
		sy = ((offs-0x500) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x600; offs < 0x700; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x400;
		sy = ((offs-0x600) >> 4);				  // page 2 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x700; offs < 0x800; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x400+16;
		sy = ((offs-0x700) >> 4);				  // page 3 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}
//---------------------------------------------------------------------------------
// x64-127,y16-31
	for (UINT32 offs = 0xC00; offs < 0xD00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000;
		sy = ((offs-0xB00) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}
 
	for (UINT32 offs = 0xD00; offs < 0xE00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+16;
		sy = ((offs-0xC00) >> 4);	  // page 9 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0xE00; offs < 0xF00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x400;
		sy = ((offs-0xD00) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}
	
	for (UINT32 offs = 0xF00; offs < 0x1000; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x400+16;
		sy = ((offs-0xE00) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}
//---------------------------------------------------------------------------------
// x64-127,y32-47
	for (UINT32 offs = 0x1400; offs < 0x1500; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800+0;
		sy = ((offs-0x1400) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1500; offs < 0x1600; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800+16;
		sy = ((offs-0x1500) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1600; offs < 0x1700; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800+0x400;
		sy = ((offs-0x1600) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1700; offs < 0x1800; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800+0x400+16;
		sy = ((offs-0x1700) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}
//---------------------------------------------------------------------------------
// x64-127,y48-63
	for (UINT32 offs = 0x1C00; offs < 0x1D00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800;
		sy = ((offs-0x1B00) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1D00; offs < 0x1E00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800+16;
		sy = ((offs-0x1C00) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1E00; offs < 0x1F00; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800+0x400;
		sy = ((offs-0x1D00) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}

	for (UINT32 offs = 0x1F00; offs < 0x2000; offs++) // page 8 16x16
	{
		sx = (offs & 0x0f)+0x1000+0x800+0x400+16;
		sy = ((offs-0x1E00) >> 4);				  // page 1 16x16
		bg_map_lut2x1[offs] = sx|(sy<<5);
	}
//---------------------------------------------------------------------------------
//#endif
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DrvInitSaturn()
{
	SPR_InitSlaveSH();

 	SS_MAP  = ss_map		=(Uint16 *)SCL_VDP2_VRAM_B1+0xC000;		   //c
	SS_MAP2 = ss_map2	=(Uint16 *)SCL_VDP2_VRAM_B1+0x8000;			//8000
// 	SS_MAP  = ss_map		=(Uint16 *)SCL_VDP2_VRAM_B1;		   //c
//	SS_MAP2 = ss_map2	=(Uint16 *)SCL_VDP2_VRAM_B1+0x8000;			//8
#ifdef BG_BANK
	ss_map3	=(Uint16 *)SCL_VDP2_VRAM_B1+0x0000;								//a
	SS_FONT = NULL; //ss_font		=(Uint16 *)SCL_VDP2_VRAM_B1;
#else
SS_FONT = ss_font		=(Uint16 *)SCL_VDP2_VRAM_B1+0x0000;
//SS_FONT = ss_font		=(Uint16 *)NULL;
#endif
	SS_CACHE= cache		=(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum	 = (SclSpPriNumRegister *)SS_N0PRI;
	ss_SpPriNum	 = (SclSpPriNumRegister *)SS_SPPRI;
	ss_OtherPri	= (SclOtherPriRegister *)SS_OTHR;
	ss_BgColMix	= (SclBgColMixRegister *)SS_BGMIX;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_scl			= (Fixed32 *)SS_SCL;

	nBurnLinescrollSize = 0;
	nBurnSprites = 128+3;

//3 nbg
#ifdef BG_BANK
	SS_SET_N0PRIN(5);
#else
	SS_SET_N0PRIN(7);
#endif
	SS_SET_N1PRIN(5);
	SS_SET_N2PRIN(7);
	SS_SET_S0PRIN(6);

//	SS_SET_N2SPRM(2);  // 1 for special priority
//	ss_regs->specialcode=0x000e; // sfcode, upper 8bits, function b, lower 8bits function a
//	ss_regs->specialcode_sel=0; // sfsel, bit 0 for nbg0 // 1 sfcs, bit 0 = 1 for funcion code b, 0 for function code a


	initLayers();
	initColors();
	initSprites(256-1,224-1,0,0,0,-32);

	SCL_Open();
//	ss_reg->n1_move_y =  16 <<16;
	ss_reg->n2_move_y =  16;//(0<<16) ;
//	ss_reg->n2_move_x =  8;//(0<<16) ;
	SCL_Close();


	memset((Uint8 *)SCL_VDP2_VRAM_B1  ,0x22,0x8000);
	memset((Uint8 *)ss_map2,0x11,0x8000);
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
	make_lut();
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 DrvExit()
{
#ifdef SND
	BurnYM2203Exit();
#endif
	CZetExit();
//	GenericTilesExit();

	free (AllMem);

	return 0;
}


void updateBgTile(INT32 type, UINT32 offs)
{
	UINT32 ofst;
	UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
	UINT32 color = (attr >> 3) & 0x0f;
	UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
	UINT32 flipx = attr & 0x80;

	ofst = bg_map_lut[offs];
	ss_map2[ofst]=(code&0x3ff) | color<<12 | flipx <<3;
}

static void draw_bg(INT32 type, INT32 layer)
{
// PNCN1 = 0x000c;
// Priority masks should be enabled, but I don't see anywhere that they are used?
//	INT32 scrollx = (*DrvScrollx)     & (0x3ff | (0x200 << type));
//	INT32 scrolly = ((*DrvScrolly)+16) & (0x7ff >> type);
//	ss_reg->n1_move_x =  ((((scrollx-1) & 0x0f))<<16) ;
//	ss_reg->n1_move_y =  ((((scrolly-1) & 0x0f))<<16) ;

//ss_reg->n1_move_y =  (((*DrvScrolly)+32)<<16) ;
//ss_reg->n1_move_x =  (((*DrvScrollx))<<16) ;

  /*
	char toto[100];
	char *titi = &toto[0];
 	titi=itoa(scrollx);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"       ",4,10);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)titi,4,10);
 	titi=itoa(scrolly);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)titi,4,20);
	 */
//	for (UINT32 offs = 000; offs < 0x100; offs++) // page 1 16x16
//	for (UINT32 offs = 0x100; offs < 0x200; offs++) // page 2 16x16
//	for (UINT32 offs = 0x200; offs < 0x300; offs++) // page 3 16x16
//	for (UINT32 offs = 0x300; offs < 0x400; offs++) // page 4 16x16
//	for (UINT32 offs = 0x400; offs < 0x500; offs++) // page 5 16x16
//	for (UINT32 offs = 0x500; offs < 0x600; offs++) // page 6 16x16
//	for (UINT32 offs = 0x600; offs < 0x700; offs++) // page 7 16x16
//	for (UINT32 offs = 0x700; offs < 0x800; offs++) // page 8 16x16
//memset(ss_map2,0x33,0x1000*sizeof(unsigned short));
//memset(&ss_map2[0x1000],0x22,0x1000*sizeof(unsigned short));
//memset(&ss_map2[0x2000],0x33,0x1000*sizeof(unsigned short));
//memset(&ss_map2[0x3000],0x44,0x1000*sizeof(unsigned short));
/*	
	for (UINT32 offs = 0; offs < 0x2000; offs++) // page 8 16x16
	{
		UINT32 ofst;
 		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ofst = bg_map_lut[offs];
		ss_map2[ofst]=(code&0x3ff) | color<<12 | flipx <<3;
	} */
//-------------------------------------------------------------------------------
#if 0
// x0-63,y0-15
	for (UINT32 offs = 0; offs < 0x100; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f);
		sy = ((offs) >> 4);				  // page 0 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}

	for (UINT32 offs = 0x100; offs < 0x200; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f)+16;
		sy = ((offs-0x100) >> 4);				  // page 1 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}

	for (UINT32 offs = 0x200; offs < 0x300; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f)+0x400;
		sy = ((offs-0x200) >> 4);				  // page 2 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}

	for (UINT32 offs = 0x300; offs < 0x400; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f)+0x400+16;
		sy = ((offs-0x300) >> 4);				  // page 3 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}
//---------------------------------------------------------------------------------
// x0-63,y16-31
 // working
	for (UINT32 offs = 0x800; offs < 0x900; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f);
		sy = ((offs-0x700) >> 4);				  // page 1 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}
 
	for (UINT32 offs = 0x900; offs < 0xA00; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f)+16;
		sy = ((offs-0x800) >> 4);	  // page 9 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}

	for (UINT32 offs = 0xA00; offs < 0xB00; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f)+0x400;
		sy = ((offs-0x900) >> 4);				  // page 1 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}
	
	for (UINT32 offs = 0xB00; offs < 0xC00; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f)+0x400+16;
		sy = ((offs-0xA00) >> 4);				  // page 1 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}
//---------------------------------------------------------------------------------
// x0-63,y32-47
	for (UINT32 offs = 0x1000; offs < 0x1100; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f)+0x800+0;
		sy = ((offs-0x1000) >> 4);				  // page 1 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}

	for (UINT32 offs = 0x1100; offs < 0x1200; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f)+0x800+16;
		sy = ((offs-0x1100) >> 4);				  // page 1 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}

	for (UINT32 offs = 0x1200; offs < 0x1300; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f)+0x800+0x400;
		sy = ((offs-0x1200) >> 4);				  // page 1 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}

	for (UINT32 offs = 0x1300; offs < 0x1400; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f)+0x800+0x400+16;
		sy = ((offs-0x1300) >> 4);				  // page 1 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}
//---------------------------------------------------------------------------------
// x0-63,y48-63
	for (UINT32 offs = 0x1800; offs < 0x1900; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f)+0x800;
		sy = ((offs-0x1700) >> 4);				  // page 1 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}

	for (UINT32 offs = 0x1900; offs < 0x1A00; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f)+0x800+16;
		sy = ((offs-0x1800) >> 4);				  // page 1 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}

	for (UINT32 offs = 0x1A00; offs < 0x1B00; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f)+0x800+0x400;
		sy = ((offs-0x1900) >> 4);				  // page 1 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}

	for (UINT32 offs = 0x1B00; offs < 0x1C00; offs++) // page 8 16x16
	{
		INT32 sx, sy, ofst;
		sx = (offs & 0x0f)+0x800+0x400+16;
		sy = ((offs-0x1A00) >> 4);				  // page 1 16x16

		UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
		INT32 color = (attr >> 3) & 0x0f;
		UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8);
		UINT32 flipx = attr & 0x80;

		ss_map2[sx|(sy<<5)]=(code&0x3ff) | color<<12 | flipx <<3;
	}
#endif
//---------------------------------------------------------------------------------
}

static void draw_sprites()
{
	for (INT32 offs = 0x200 - 4; offs >= 0; offs -= 4)
	{
		UINT32 attr = DrvSprBuf[offs+1];
		INT32 sx = DrvSprBuf[offs + 3] - ((attr & 0x10) << 4);
		INT32 sy = DrvSprBuf[offs + 2];

		UINT32 delta	= ((offs/4)+3);

		if (sy < -15 || sy > 239 || sx < -15 || sx > 255)
		{
			ss_sprite[delta].charSize		= 0;
		}
		else
		{
			UINT32 code = DrvSprBuf[offs] | ((attr & 0xe0) << 3);
			UINT32 color = attr & 0x07;
			UINT32 flipx = (attr & 0x08) << 1;

			ss_sprite[delta].control		= ( JUMP_NEXT | FUNC_NORMALSP) | flipx;
			ss_sprite[delta].drawMode	= ( ECD_DISABLE | COMPO_REP);

			ss_sprite[delta].ax			= sx;
			ss_sprite[delta].ay			= sy;
			ss_sprite[delta].charSize		= 0x210;
			ss_sprite[delta].color			    = color<<4;//Colour<<4;
			ss_sprite[delta].charAddr		= 0x220+(code<<4);
		}
	}
}

static INT32 DrvFrame()
{
//		FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)"dfvframe start           ",4,10);
#ifndef BG_BANK
		SCL_SetColRam(SCL_NBG0,8,8,palette);
#endif
//  	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvFrame          ",4,10);

//DrvZ80RAM0[0xF3A1-0xe000]=2;
DrvZ80RAM0[0xE905-0xe000]= 0x01;
DrvZ80RAM0[0xF424-0xe000]= 0x0F;


	if (DrvReset) {
		DrvDoReset(1);
	}

	if (watchdog >= 180) {
		DrvDoReset(0);
	}
	watchdog++;

	{
		DrvInputs[0] = DrvInputs[1] = DrvInputs[2] = 0xff;

		for (INT32 i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
			DrvInputs[2] ^= (DrvJoy3[i] & 1) << i;
		}

		DrvInputs[0] |= *coin_lockout;
	}
//	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)"CZetNewFrame           ",4,20);

	CZetNewFrame();

	INT32 nInterleave = 100;
//	INT32 nInterleave = 20;
	nCyclesTotal[0] = 4000000 / 60; ///2;
	nCyclesTotal[1] = 3000000 / 60; ///2;
	INT32 nCyclesDone[2] = { 0, 0 };
	
	for (INT32 i = 0; i < nInterleave; i++) {
		INT32 nCurrentCPU, nNext, nCyclesSegment;
//	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)"CZetOpen 0           ",4,20);
		// Run Z80 #1
		nCurrentCPU = 0;
		CZetOpen(nCurrentCPU);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesDone[nCurrentCPU] += CZetRun(nCyclesSegment);
		if (i == 98) CZetSetIRQLine(0, CZET_IRQSTATUS_ACK);
		if (i == 99) CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);
//		if (i == 18) CZetSetIRQLine(0, CZET_IRQSTATUS_ACK);
//		if (i == 19) CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);
		CZetClose();
//	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)"CZetOpen 1           ",4,20);
		// Run Z80 #2
#ifndef RAZE
		nCurrentCPU = 1;
		CZetOpen(nCurrentCPU);
//	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)"BurnTimerUpdate           ",4,20);
#ifdef SND
		BurnTimerUpdate(i * (nCyclesTotal[nCurrentCPU] / nInterleave));
#endif
		CZetClose();
#endif
	}
#ifndef RAZE
	CZetOpen(1);
//	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)"BurnTimerEndFrame           ",4,20);
#ifdef SND
	BurnTimerEndFrame(nCyclesTotal[1]);
//	if (pBurnSoundOut)
	signed short *nSoundBuffer = (signed short *)(0x25a20000+nSoundBufferPos*(sizeof(signed short)));
//	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)"BurnYM2203Update           ",4,20);
	BurnYM2203Update(nSoundBuffer, nBurnSoundLen);
#endif
	CZetClose();

#ifdef SND
	nSoundBufferPos+=(nBurnSoundLen); // DOIT etre deux fois la taille copiee
	if(nSoundBufferPos>=0x2000)//RING_BUF_SIZE)
	{
		PCM_Task(pcm); // bon emplacement
		nSoundBufferPos=0;
	}
#endif

#endif
//	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)"draw_sprites           ",4,20);
	
	draw_sprites();
//	UINT32 position = ((*DrvScrollx)+32) / 16 + (((*DrvScrolly) /16)+32)*128;
//	PNCN1 = ((ss_map2[position]&0x7ff)<0x400) ? 0x8008 : 0x800c;

/*
	for (INT32 i = 0; i < nScreenWidth * nScreenHeight; i++) 
	{
		pTransDraw[i] = 0x3ff;
	}
*/
	memcpyl (DrvSprBuf, DrvSprRAM, 0x1200);

//	ss_reg->n1_move_x =  ((1024)<<16) ;
//	ss_reg->n1_move_y =  ((384)<<16) ;

//		FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)"dfvframe end           ",4,10);
	return 0;
}

/*
static INT32 DrvScan(INT32 nAction, INT32 *pnMin)
{
	struct BurnArea ba;
	
	if (pnMin != NULL) {
		*pnMin = 0x029698;
	}

	if (nAction & ACB_MEMORY_RAM) {
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = AllRam;
		ba.nLen	  = RamEnd-AllRam;
		ba.szName = "All Ram";
		BurnAcb(&ba);
	}

	if (nAction & ACB_DRIVER_DATA) {
		CZetScan(nAction);

		BurnYM2203Scan(nAction, pnMin);
	}

	CZetOpen(0);
	DrvRomBankswitch(*DrvRomBank);
	DrvVidRamBankswitch(*DrvVidBank);
	CZetClose();

	return 0;
}
*/