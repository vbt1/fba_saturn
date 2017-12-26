// FB Alpha Black Tiger driver module
// Based on MAME driver by Paul Leaman

//#include "tiles_generic.h"
//#include "z80_intf.h"
#include "snd/burn_ym2203.h"
//#include "bitswap.h"
#include "d_blktiger.h"
#include    "machine.h"
#define RAZE0 1
#define SND 1
#define nYM2203Clockspeed 3579545
/*
<vbt1> where and when you update the nbg map
<vbt1> in loop, during vblank in , during vblank out ?
<vbt1> with dma, etc
<CyberWarriorX> vblank in
<CyberWarriorX> vblank in is the start of the vblank. vblank out is the end of it
*/
/*
vbt> <Kale_> Ok, have you asked to Arbee?
<vbt> <Kale_> The idea is first to write a sound engine to m68k, using a portion of the shared RAM as your i/o registers
<vbt> <Kale_> Then banging the individual registers according to what the 2203 expects
<vbt> <smf-> I might just emulate the games sound program on the 68000
<vbt> <smf-> I imagine the sh2 would be happier emulating one less z80
<vbt> <smf-> as it running in rom you could probably even do an upfront translation
*/

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
		DrvInit, DrvExit, DrvFrame, NULL
	};
#ifdef FM
	struct BurnDriver nBurnDrvFM = {
		"fm", "blktgr",
		"FM Test Driver",
		blktigerRomInfo, blktigerRomName, DrvInputInfo, DrvDIPInfo,
		DrvFMInit, DrvExit, DrvFMFrame, NULL //, DrvScan, &DrvRecalc, 0x400,
	};

	if (strcmp(nBurnDrvFM.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvFM,sizeof(struct BurnDriver));
#endif

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

#ifdef RAZE0
	z80_map_read  (0x8000, 0xbfff, DrvZ80ROM0 + nBank);
	z80_map_fetch (0x8000, 0xbfff, DrvZ80ROM0 + nBank);
#endif
}

static void DrvVidRamBankswitch(INT32 bank)
{
	*DrvVidBank = (bank & 0x03);
	INT32 nBank = (bank & 3) * 0x1000;

#ifdef RAZE0
	z80_map_read  (0xc000, 0xcfff, DrvBgRAM + nBank);
	z80_map_fetch (0xc000, 0xcfff, DrvBgRAM + nBank);
#else
	CZetMapArea(0xc000, 0xcfff, 0, DrvBgRAM + nBank);
//	CZetMapArea(0xc000, 0xcfff, 1, DrvBgRAM + nBank);
	CZetMapArea(0xc000, 0xcfff, 2, DrvBgRAM + nBank);
#endif
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
			updateBgTile2Words(/* *DrvScreenLayout,*/ address>>1);
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
/*			if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
			{
				SPR_WaitEndSlaveSH();
			}*/
			*soundlatch = data;
		}
		return;

		case 0x01:
			DrvRomBankswitch(data);
		return;

		case 0x03:
			if (DrvDips[2] & 1) {
				coin_lockout = ~data << 6;
			}
		return;

		case 0x04:
			if (data & 0x20) {
//				CZetClose();
#ifndef RAZE
/*			if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
			{
				SPR_WaitEndSlaveSH();
			}*/
				CZetOpen(1);
				CZetReset();
//				CZetClose();
#else
				z80_reset();
#endif
//				CZetOpen(0);
			}

			*flipscreen  =  data & 0x40;
			*DrvFgEnable = ~data & 0x80;

			ss_regs->dispenbl &= 0xfffb;
			ss_regs->dispenbl |= (*DrvFgEnable >> 5) & 0x0004;
			BGON = ss_regs->dispenbl;
		return;

		case 0x06:
			watchdog = 0;
		return;

		case 0x08:
			*DrvScrollx = (*DrvScrollx & 0xff00) | data;
			ss_reg->n1_move_y =  (((*DrvScrolly)+16)<<16) ;	  //  ? remettre
		return;

		case 0x09:
			*DrvScrollx = (*DrvScrollx & 0x00ff) | (data << 8);
			ss_reg->n1_move_y =  (((*DrvScrolly)+16)<<16) ;	  // ? remettre
		return;

		case 0x0a:
			*DrvScrolly = (*DrvScrolly & 0xff00) | data;
			ss_reg->n1_move_x =  (((*DrvScrollx))<<16) ;
		return;

		case 0x0b:
			*DrvScrolly = (*DrvScrolly & 0x00ff) | (data << 8);
			ss_reg->n1_move_x =  (((*DrvScrollx))<<16) ;
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
//				*DrvScreenLayout = 1;
				bg_map_lut = &bg_map_lut2x1[0];
				PLANEADDR1 = ss_regd->normap[2] = 0x3430;
			}
			else
			{
//				*DrvScreenLayout = 0;
				bg_map_lut = &bg_map_lut2x2[0];
				PLANEADDR1 = ss_regd->normap[2] = 0x3030;
			}
		}
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
#ifdef RAZE0
			return z80_get_reg(Z80_REG_DE)  >> 8;
#endif
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
	DrvGfxROM1	= SS_CACHE + 0x08000;
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
//	coin_lockout	= Next; Next += 0x000001;

	RamEnd			= Next;
	CZ80Context		= Next; Next += (0x1080*2);
	remap16_lut		= Next; Next += 768 * sizeof (UINT16);
	remap4to16_lut	= Next; Next += 256 * sizeof (UINT16);
	cram_lut			= Next; Next += 4096 * sizeof (UINT16);
	fg_map_lut		= Next; Next += 0x400 * sizeof (UINT16);
	bg_map_lut2x1	= Next; Next += 0x2000 * sizeof (UINT16);
	bg_map_lut2x2	= Next; Next += 0x2000 * sizeof (UINT16);
#ifdef SND
	ym_buffer			= (INT16*)Next; Next += 4096 * 4 * 2 * sizeof(INT16);
#endif
	MemEnd			= Next;

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 DrvDoReset(INT32 full_reset)
{
	if (full_reset) {
		memset (AllRam, 0, RamEnd - AllRam);
	}

#ifdef RAZE0
	z80_reset();
	DrvRomBankswitch(1);
	DrvVidRamBankswitch(1);
#endif

#ifndef RAZE
	CZetOpen(1);
	CZetReset();
//	CZetClose();
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
// texte
	memcpy (tmp, DrvGfxROM0, 0x08000);
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
	GfxDecode4Bpp(0x0800, 4, 16, 16, Plane + 0, XOffs, YOffs, 0x200, tmp, DrvGfxROM1);

	tile16x16toSaturn(0,0x0400, DrvGfxROM1);
	tile16x16toSaturn(1,0x0400, DrvGfxROM1+0x20000);
	
 	for (int i=0;i<0x40000;i++ )
	{
		if ((DrvGfxROM1[i]& 0x0f)     ==0x00)DrvGfxROM1[i] = DrvGfxROM1[i] & 0xf0 | 0xf;
		else if ((DrvGfxROM1[i]& 0x0f)==0x0f) DrvGfxROM1[i] = DrvGfxROM1[i] & 0xf0;

		if ((DrvGfxROM1[i]& 0xf0)       ==0x00)DrvGfxROM1[i] = 0xf0 | DrvGfxROM1[i] & 0x0f;
		else if ((DrvGfxROM1[i]& 0xf0)==0xf0) DrvGfxROM1[i] = DrvGfxROM1[i] & 0x0f;
	}
//sprites
	memcpy (tmp, DrvGfxROM2, 0x40000);
	GfxDecode4Bpp(0x0800, 4, 16, 16, Plane + 0, XOffs, YOffs, 0x200, tmp, DrvGfxROM2);

 	for (int i=0;i<0x40000;i++ )
	{
		if ((DrvGfxROM2[i]& 0x0f)     ==0x00)DrvGfxROM2[i] = DrvGfxROM2[i] & 0xf0 | 0xf;
		else if ((DrvGfxROM2[i]& 0x0f)==0x0f) DrvGfxROM2[i] = DrvGfxROM2[i] & 0xf0;

		if ((DrvGfxROM2[i]& 0xf0)       ==0x00)DrvGfxROM2[i] = 0xf0 | DrvGfxROM2[i] & 0x0f;
		else if ((DrvGfxROM2[i]& 0xf0)==0xf0) DrvGfxROM2[i] = DrvGfxROM2[i] & 0x0f;
	}
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
//int aaa = 10;
static INT32 DrvSynchroniseStream(INT32 nSoundRate)
{
//	return (INT32)CZetTotalCyclesSlave() * nSoundRate / nYM2203Clockspeed;
/*	if(aaa < 200)
	{
		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)itoa(
			
		(INT32)CZetTotalCycles() * 44100 / nYM2203Clockspeed
		),10,aaa);	

		
		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)itoa(
			
		(INT32)CZetTotalCycles() * nSoundRate / nYM2203Clockspeed
		),80,aaa);	
		aaa+=10;
	}
	else
	{
		aaa=10;
	}*/
	return (INT32)CZetTotalCycles() * nSoundRate / nYM2203Clockspeed;
}

static INT32 DrvGetTime()
{
//	return (INT32)CZetTotalCyclesSlave() / nYM2203Clockspeed;
	return (INT32)CZetTotalCycles() / nYM2203Clockspeed;
}

static INT32 DrvInit()
{
	DrvInitSaturn();
	AllMem = NULL;

	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
#if  1
	if ((AllMem = (UINT8 *)malloc(nLen)) == NULL)
	{
		FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"malloc failed",4,80);
		return 1;
	}
#endif
	memset(AllMem, 0, nLen);
	MemIndex();
	make_lut();

	{
		for (INT32 i = 0; i < 5; i++) {
			if (BurnLoadRom(DrvZ80ROM0 + i * 0x10000, 0  + i, 1)) return 1;
		}

		if (BurnLoadRom(DrvZ80ROM1, 5, 1)) return 1;

		if (BurnLoadRom(DrvGfxROM0, 6, 1)) return 1;

		for (INT32 i = 0; i < 4; i++) {
			if (BurnLoadRom(DrvGfxROM1 + i * 0x10000, 7  + i, 1)) return 1;
			if (BurnLoadRom(DrvGfxROM2 + i * 0x10000, 11 + i, 1)) return 1;
		}

		DrvGfxDecode();
	}
	Set7PCM();
	drawWindow(0,224,240,0,64);
#ifndef RAZE
	CZetInit2(2,CZ80Context);
#else
	CZetInit2(1,CZ80Context);
#endif


#ifdef RAZE0
	z80_map_read(0x0000, 0x7fff, DrvZ80ROM0);
	z80_map_fetch(0x0000, 0x7fff, DrvZ80ROM0);
	z80_map_read(0xd000, 0xd7ff, DrvTxRAM);
	z80_map_fetch(0xd000, 0xd7ff, DrvTxRAM);
	z80_map_read(0xd800, 0xdfff, DrvPalRAM);
	z80_map_fetch(0xd800, 0xdfff, DrvPalRAM);
	z80_map_read(0xe000, 0xfdff, DrvZ80RAM0);
	z80_map_write(0xe000, 0xfdff, DrvZ80RAM0);
	z80_map_fetch(0xe000, 0xfdff, DrvZ80RAM0);
	z80_map_read(0xfe00, 0xffff, DrvSprRAM);
	z80_map_write(0xfe00, 0xffff, DrvSprRAM);
	z80_map_fetch(0xfe00, 0xffff, DrvSprRAM);
	z80_add_write(0xc000, 0xdfff, 1, (void *)&blacktiger_write);
//	z80_add_read(0xc000, 0xcfff, 1, (void *)&blacktiger_read);
	z80_set_in(blacktiger_in);
	z80_set_out(blacktiger_out);
#endif

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
#endif
#ifdef SND
	BurnYM2203Init(2, ym_buffer, nYM2203Clockspeed, &DrvFMIRQHandler, DrvSynchroniseStream, DrvGetTime, 0);
	BurnTimerAttachZet(nYM2203Clockspeed);
#endif
	DrvDoReset(1);

	for (INT32 i = 0; i < 0x400; i++) 
	{
		palette_write(i);
	}
	SCL_SetColRam(SCL_NBG0,8,8,palette);
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 DrvExit()
{
	SPR_InitSlaveSH();
#ifdef SND
	BurnYM2203Exit();
#endif
	CZetExit2();

	for(int i=0;i<7;i++)
	{
		PCM_MeStop(pcm7[i]);
		memset(SOUND_BUFFER+(0x4000*(i+1)),0x00,RING_BUF_SIZE*8);
	}

	CZ80Context = DrvZ80ROM0 = DrvZ80ROM1 = DrvGfxROM0 = DrvGfxROM1 = DrvGfxROM2 = NULL;
	DrvZ80RAM0	 = DrvZ80RAM1 = DrvPalRAM = DrvTxRAM = DrvBgRAM = DrvSprRAM = DrvSprBuf = NULL;
/*	DrvScreenLayout =*/ DrvBgEnable = DrvFgEnable = DrvSprEnable = DrvVidBank = DrvRomBank	= NULL;
	DrvPalette = DrvScrollx	= DrvScrolly = NULL;
	soundlatch = flipscreen /*= coin_lockout*/ = NULL;
	remap16_lut = remap4to16_lut	= cram_lut = fg_map_lut = bg_map_lut2x1 = bg_map_lut2x2 = NULL;
	MemEnd = AllRam = RamEnd = NULL;
	watchdog = 0;
	coin_lockout = 0;

	free (AllMem);
	AllMem = NULL;
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 DrvFrame()
{
// cheat code level
//DrvZ80RAM0[0xF3A1-0xe000]=6;
// cheat code invincible
//DrvZ80RAM0[0xE905-0xe000]= 0x01;
//DrvZ80RAM0[0xF424-0xe000]= 0x0F;

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

		DrvInputs[0] |= coin_lockout;
	}

	CZetNewFrame();

//	UINT32 nInterleave = 100;
	UINT32 nInterleave = 20;
	nCyclesTotal[0] = 3000000 / 60; ///2;
	nCyclesTotal[1] = nYM2203Clockspeed / 60; ///2;
	UINT32 nCyclesDone = 0;
/*	*(unsigned int*)OPEN_CSH_VAR(SS_Z80CY) = 0;
		SPR_RunSlaveSH(CZetNewFrame,NULL);
		if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
		{
			SPR_WaitEndSlaveSH();
		}*/

	CZetOpen(1);
//	BurnTimerEndFrame(&nCyclesTotal[1]);
	SPR_RunSlaveSH(BurnTimerEndFrame,&nCyclesTotal[1]);
	INT32 nNext, nCyclesSegment;

	for (UINT32 i = 0; i < nInterleave; i++) {
#ifdef SND
		// Run Z80 #2
/*		CZetOpen(1);
		INT32 nCycle = i * (nCyclesTotal[1] / nInterleave);
		BurnTimerUpdate(&nCycle);
		SPR_RunSlaveSH((PARA_RTN*)BurnTimerUpdate,&nCycle);
		if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
		{
			SPR_WaitEndSlaveSH();
		}*/
//		CZetClose();
#endif

		// Run Z80 #1
#ifdef RAZE0
		nNext = (i + 1) * nCyclesTotal[0] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone;
		nCyclesDone += z80_emulate(nCyclesSegment);
//		if (i == 98) z80_raise_IRQ(0);
//		if (i == 99) z80_lower_IRQ();
		if (i == 18) z80_raise_IRQ(0);
		if (i == 19) z80_lower_IRQ();
#endif

#ifdef SND
/*		if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
		{
			SPR_WaitEndSlaveSH();
		}*/
		// Run Z80 #2
//		CZetOpen(1);
//		INT32 nCycle = i * (nCyclesTotal[1] / nInterleave);
//		BurnTimerUpdate(&nCycle);
//		SPR_RunSlaveSH((PARA_RTN*)BurnTimerUpdate,&nCycle);
//		CZetClose();
#endif
	}
#ifdef SND
	if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
		{
			SPR_WaitEndSlaveSH();
		}
//	CZetOpen(1);
//	BurnTimerEndFrame(nCyclesTotal[1]);
	signed short *nSoundBuffer = (signed short *)0x25a24000+nSoundBufferPos; //*(sizeof(signed short)));
	YM2203UpdateNormal(nSoundBuffer, nBurnSoundLen);
	CZetClose();

	nSoundBufferPos+=(nBurnSoundLen); // DOIT etre deux fois la taille copiee

	if(nSoundBufferPos>=0x1000)//RING_BUF_SIZE)
	{
/*		PCM_NotifyWriteSize(pcm, nSoundBufferPos);
		PCM_Task(pcm); // bon emplacement
*/

		for (unsigned int i=0;i<7;i++)
		{
			PCM_NotifyWriteSize(pcm7[i], nSoundBufferPos);
			PCM_Task(pcm7[i]); // bon emplacement
		}

		nSoundBufferPos=0;
	}
#endif
	draw_sprites();
	memcpyl (DrvSprBuf, DrvSprRAM, 0x1200);
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
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
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 DrvFMFrame()
{
//	PCM_Task(pcm); 
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
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_SPR,OFF);
	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);

	SCL_SetColRam(SCL_NBG0,8,8,palette);	 // vbt ? remettre
}
//-------------------------------------------------------------------------------------------------------------------------------------
static PcmHn createHandle(PcmCreatePara *para)
{
	PcmHn pcm;

	pcm = PCM_CreateMemHandle(para);
	if (pcm == NULL) {
		return NULL;
	}
	return pcm;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void Set7PCM()
{
	PcmCreatePara	para[7];
	PcmInfo 		info[7];
	PcmStatus	*st;
	static PcmWork g_movie_work[7];

	for (int i=0; i<7; i++)
	{
		PCM_PARA_WORK(&para[i]) = (struct PcmWork *)&g_movie_work[i];
		PCM_PARA_RING_ADDR(&para[i]) = (Sint8 *)PCM_ADDR+0x40000+(0x4000*(i+1));
		PCM_PARA_RING_SIZE(&para[i]) = RING_BUF_SIZE;
		PCM_PARA_PCM_ADDR(&para[i]) = PCM_ADDR+(0x4000*(i+1));
		PCM_PARA_PCM_SIZE(&para[i]) = PCM_SIZE;

		memset((Sint8 *)SOUND_BUFFER,0,SOUNDRATE*16);
		st = &g_movie_work[i].status;
		st->need_ci = PCM_ON;
	 
		PCM_INFO_FILE_TYPE(&info[i]) = PCM_FILE_TYPE_NO_HEADER;			
		PCM_INFO_DATA_TYPE(&info[i])=PCM_DATA_TYPE_RLRLRL;//PCM_DATA_TYPE_LRLRLR;
		PCM_INFO_FILE_SIZE(&info[i]) = RING_BUF_SIZE;//SOUNDRATE*2;//0x4000;//214896;
		PCM_INFO_CHANNEL(&info[i]) = 0x01;
		PCM_INFO_SAMPLING_BIT(&info[i]) = 16;

		PCM_INFO_SAMPLING_RATE(&info[i])	= SOUNDRATE;//30720L;//44100L;
		PCM_INFO_SAMPLE_FILE(&info[i]) = RING_BUF_SIZE;//SOUNDRATE*2;//30720L;//214896;
		pcm7[i] = createHandle(&para[i]);

		PCM_SetPcmStreamNo(pcm7[i], i);

		PCM_SetInfo(pcm7[i], &info[i]);
		PCM_ChangePcmPara(pcm7[i]);

		PCM_MeSetLoop(pcm7[i], 0x3FF);//SOUNDRATE*120);
		PCM_Start(pcm7[i]);
	}
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
// commenter si  1 word
	for (UINT32 offs = 0; offs < 0x2000; offs++) // page 8 16x16
	{
		bg_map_lut2x2[offs] = bg_map_lut2x2[offs] * 2;
		bg_map_lut2x1[offs] = bg_map_lut2x1[offs] * 2;
	}

}
//-------------------------------------------------------------------------------------------------------------------------------------
void dummy()
{

}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);

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
	initSprites(256-1,224-1,0,0,0,-16);

	SCL_Open();
//	ss_reg->n1_move_y =  16 <<16;
	ss_reg->n2_move_y =  16;//(0<<16) ;
//	ss_reg->n2_move_x =  8;//(0<<16) ;
	SCL_Close();


	memset((Uint8 *)SCL_VDP2_VRAM_B1  ,0x22,0x8000);
	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Loading. Please Wait",24,40);
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
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------
static void tile16x16toSaturn (unsigned char reverse, unsigned int num, unsigned char *pDest)
{
	unsigned int c;
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
void updateBgTile2Words(/*INT32 type,*/ UINT32 offs)
{
	UINT32 ofst;
	UINT32 attr  = DrvBgRAM[(offs<<1) | 1];
	UINT32 color = (attr >> 3) & 0x0f;
	UINT32 code  = DrvBgRAM[(offs<<1)] | ((attr & 0x07) << 8); // + ((3-DrvVidBank[0])*0x400);
	UINT32 flipx = attr & 0x80;

	ofst = bg_map_lut[offs];
	ss_map2[ofst] = color | flipx << 7; //| 0x4000; // | flipx << 7; // vbt remttre le flip ?
	ss_map2[ofst+1] = (code*4)+0x1000; 
}

#ifdef FM
typedef struct{
    Uint16 *prg_adr;                        /* 68K??????i?[??????       */
    Uint16 prg_sz;                          /* 68K????????????           */
    Uint16 *ara_adr;                        /* ?????????i?[??????     */
    Uint16 ara_sz;                          /* ????????????(???P??)    */
}SndIniDt; 

#define SND_PRM_MODE(prm)       ((prm).mode)        /* ???????+????????? */
#define SND_PRM_SADR(prm)       ((prm).sadr)        /* PCM??????¸???i???*/
#define SND_PRM_SIZE(prm)       ((prm).size)        /* PCM??????¸????   */
#define SND_PRM_OFSET(prm)      ((prm).ofset)       /* PCM????Ð??J?n???? */
/******** SndPcmChgPrm(PCM??X?p?????[?^) ************************************/
#define SND_PRM_NUM(prm)        ((prm).num)         /* PCM????Ð????       */
#define SND_PRM_LEV(prm)        ((prm).lev)         /* ???c?Level         */
#define SND_PRM_PAN(prm)        ((prm).pan)         /* ???c?pan           */
#define SND_PRM_PICH(prm)       ((prm).pich)        /* PICH???              */
#define SND_R_EFCT_IN(prm)      ((prm).r_efct_in)   /* Efect in select(?E)   */
#define SND_R_EFCT_LEV(prm)     ((prm).r_efct_lev)  /* Efect send Level(?E)  */
#define SND_L_EFCT_IN(prm)      ((prm).l_efct_in)   /* Efect in select(??)   */
#define SND_L_EFCT_LEV(prm)     ((prm).l_efct_lev)  /* Efect send Level(??)  */
#define SND_PRM_TL(prm)     	((prm).lev)  		/* Master Level			 */


#define SCSP_REG_SET    0x0200                  /* SCSP???W?X?^???l        */
#define MEM_CLR_SIZE    0xb000                  /* ?????????????           */
#define ADR_SND_VECTOR  ((Uint8 *)0x25a00000)   /* ?T?E???h?x?N?^?A?h???X    */
#define ADR_SCSP_REG    ((Uint8 *)0x25b00400)   /* SCSP????????W?X?^      */
#define ADR_SND_MEM     ((Uint8 *)0x25a00000)   /* ?T?E???h?????????A?h???X*/
#define ADR_SND_VECTOR  ((Uint8 *)0x25a00000)   /* ?T?E???h?x?N?^?A?h???X    */

#define ADR_SYS_TBL     (ADR_SND_MEM + 0x400)   /* ??????????¨?           */
#define ADR_ARA_CRNT    (0x08)                /* ?????????CRNT????????*/
#define ARA_MAP_SIZE        0x2                 /* ????????????          */

#define B_LOAD_MARK         31                  /* ?]????????              */
#define M_LOAD_MARK        (0x1  << B_LOAD_MARK)/* ?]????????              */
#define B_END_MARK          31                  /* ????I?????              */

#define B_START_ADR         0                   /* ???i???                 */
#define M_START_ADR        (0xfffff << B_START_ADR) /* ???i???             */
#define B_ID_NUM            24                  /* ??????                  */
#define M_ID_NUM           (0xf  << B_ID_NUM)   /* ??????                  */
#define B_DATA_ID           28                  /* ??????@?@?@?@?@?@?@    */
#define M_DATA_ID          (0x7  << B_DATA_ID)  /* ??????@?@?@?@?@?@?@    */
#define M_END_MARK         (0x1  << B_END_MARK) /* ????I?????              */

#define ARA_MAP_0           0x0         /* ??????,??????,???J?n????    */
#define ARA_MAP_4           0x1         /* ?]????????,??????              */

#define ADR_SYS_INFO    (0x00)                  /* ??????ð??????        */
#define ADR_HOST_INT    (0x04)                  /* ?????????????        */
#define ADR_ARA_CRNT    (0x08)                /* ?????????CRNT????????*/
#define ADR_SYS_INT_WORK    (0x12)            /* ?????????????????     */
#define ADR_HARD_CHK_STAT   (0x18)            /* ???????????ð???i?[??    */
#define ADR_SONG_STAT   (0x80)                  /* song status               */
#define ADR_PCM         (0xa0)                  /* PCM                       */
#define ADR_SEQ         (0xb0)                  /* Sequence                  */
#define ADR_TL_VL       (0x90)                  /* Total volume              */
#define ADR_TL_HZ_VL    (0x94)                  /* ???g??????Volume        */
#define ADR_ARA_ADR     (0x08)                  /* ?????????¨?????? */
#define CHG_LONG(x)    (((x) * 2) + (0x4 - ( ((x) * 2) % 4) ))

#define POKE_W(adr, data)   (*((volatile Uint16 *)(adr)) = ((Uint16)(data))) /* ???  */
#define PEEK_W(adr)         (*((volatile Uint16 *)(adr)))                    /* ???  */
#define PEEK_L(adr)         (*((volatile Uint32 *)(adr)))                    /* ???  */
#define POKE_B(adr, data)   (*((volatile Uint8 *)(adr)) = ((Uint8)(data)))   /* ???  */
#define POKE_L(adr, data)   (*((volatile Uint32 *)(adr)) = ((Uint32)(data))) /* ???  */

#define SND_INI_PRG_ADR(sys_ini)    ((sys_ini).prg_adr) /* 68K??????i?[... */
#define SND_INI_PRG_SZ(sys_ini)     ((sys_ini).prg_sz)  /* 68K?????????... */
#define SND_INI_ARA_ADR(sys_ini)    ((sys_ini).ara_adr) /* ?????????i?[. */
#define SND_INI_ARA_SZ(sys_ini)     ((sys_ini).ara_sz)  /* ????????????. */
#define SND_KD_TONE         0x0                         /* ???F              */

#define SND_ADR_INTR_RESET  ((volatile Uint16 *)0x25b0042e)  /* SCSP???????????? */
#define SND_POKE_W(adr, data)   (*((volatile Uint16 *)(adr)) = ((Uint16)(data)))
#define SND_ADR_INTR_CTRL_WORD  (0x00)          /* Interrupt control word    */
#define SND_POKE_B(adr, data)   (*((volatile Uint8 *)(adr)) = ((Uint8)(data))) /* ???*/

#define SND_RET_SET     0                       /* ????I??                     */
#define SND_RET_NSET    1                       /* ???I??                     */
#define ADR_PRM_DATA    (0x02)                  /* ?p?????[?^                */
#define COM_SET_TL_VL      0x82                 /* Total Volume              */

#define COM_CHG_MIX        0x87                 /* Mixer change              */
#define COM_CHG_MIX_PRM    0x88                 /* Mixer parameter change    */
#define COM_CHK_HARD       0x89                 /* Hard check                */
#define COM_CHG_PCM_PRM    0x8a                 /* PCM parameter change      */
#define COM_CTRL_DIR_MIDI  0x09                 /* MIDI direct control       */

#define SND_SET_ENA_INT(ena_bit)\
    do{\
        snd_msk_work_work = get_imask();           /* ???????????Disable*/\
        set_imask(15);\
        SND_POKE_B(snd_adr_sys_int_work + SND_ADR_INTR_CTRL_WORD, (ena_bit));\
        set_imask(snd_msk_work_work);\
    }while(FALSE)

static Uint32 intrflag;
#define HOST_SET_RETURN(ret)\
    do{\
		intrflag=0;\
        return(ret);\
    }while(FALSE)

#define SET_PRM(no, set_prm)\
(POKE_B(adr_com_block + ADR_PRM_DATA + (no), (set_prm))) /* ??????Z?b?g      */

#define SIZE_COM_BLOCK      (0x10)              /* ?????????????          */
#define MAX_NUM_COM_BLOCK   8                   /* ???????????              */

#define SND_INT_PCM_ADR     (1 <<  7)           /* PCM play address ?X?V   */
#define SND_RESET_INT()\
        (SND_POKE_W(SND_ADR_INTR_RESET, (1 << 5)))

#define ADR_COM_DATA    (0x00)                  /* ?R?}???h                  */

#define NOW_ADR_COM_DATA                        /* ???????????????     */\
    (adr_com_block + ADR_COM_DATA)

#define MAX_ADR_COM_DATA                        /* ?o????????????     */\
    (adr_host_int_work + ADR_COM_DATA + (SIZE_COM_BLOCK * MAX_NUM_COM_BLOCK))

#define SET_COMMAND(set_com)\
(POKE_W((adr_com_block + ADR_COM_DATA), (Uint16)(set_com) << 8)) /* ?R?}???h?Z?b?g   */

#define SND_MD_MONO     (0 <<  7)                   /* ???                  */
#define SND_MD_STEREO   (1 <<  7)                   /* ???                  */
#define SND_MD_16       (0 <<  4)                   /* 16bitPCM              */
#define SND_MD_8        (1 <<  4)                   /* 8bitPCM               */

Uint8 *snd_adr_sys_int_work;                 /*??????????????????i?[*/
Uint32 snd_msk_work_work;                    /* sound priority msk        */

static volatile Uint32 *adr_snd_area_crnt;             /* ?????????CRNT????????*/
static volatile Uint8 *adr_sys_info_tbl;                 /* ??????ð???????i?[    */
static volatile Uint8 *adr_host_int_work;                /* ????????????????i?[*/
static volatile Uint8  *adr_com_block;                   /* ???????????????????      */
static volatile Uint16 *adr_song_stat;                   /* song status               */
static volatile Uint16 *adr_pcm;                         /* PCM                       */
static volatile Uint16 *adr_seq;                         /* Sequence                  */
static volatile Uint16 *adr_tl_vl;                       /* Total volume              */
static volatile Uint16 *adr_tl_hz_vl;                    /* ???g??????Volume        */
static Uint32 intrflag;

typedef struct{
    Uint8 mode;                             /* ???????+?????????         */
    Uint16 sadr;                            /* PCM??????¸???i???        */
    Uint16 size;                            /* PCM??????¸????             */
}SndPcmStartPrm;                            /* PCM?J?n?p?????[?^          */
typedef Uint8 SndSeqPri;                    /* Priorty level                 */

typedef Uint8 SndSeqNum;                    /* ?????U????                  */
typedef Uint8 SndAreaMap;                   /* ?????????f?[?^?^          */
typedef Uint8 SndTlVl;                      /* ?S?????f?[?^?^              */
typedef Uint8 SndEfctBnkNum;                /* Effct bank number?f?[?^?^     */
typedef Uint8 SndToneBnkNum;                /* ???F bank number?f?[?^?^      */
typedef Uint8 SndMixBnkNum;                 /* Mixer number?f?[?^?^          */
typedef Uint8 SndEfctOut;                   /* Effect out select?f?[?^?^     */
typedef Uint8 SndLev;                       /* Level?f?[?^?^                 */
typedef Sint8 SndPan;                       /* PAN?f?[?^?^                   */
typedef Uint8 SndRet;                       /* ???????s???f?[?^?^         */
typedef Uint8 SndHardPrm;                   /* ???????????????f?[?^?^     */
typedef Uint16 SndHardStat;                 /* ??????????ð???f?[?^?^      */
typedef Uint8 SndPcmNum;                    /* PCM????Ð????               */
typedef Uint8 SndEfctIn;                    /* Efect in select               */
typedef struct{
    SndPcmNum num;                          /* PCM????Ð????               */
    SndLev lev;                             /* ???c?Level                 */
    SndPan pan;                             /* ???c?pan                   */
    Uint16 pich;                            /* PICH???                      */
    SndEfctIn r_efct_in;                    /* Efect in select(?E?o??)       */
    SndLev r_efct_lev;                      /* Efect send Level(?E?o??)      */
    SndEfctIn l_efct_in;                    /* Efect in select(???o??)       */
    SndLev l_efct_lev;                      /* Efect send Level(???o??)      */
}SndPcmChgPrm;   

static void DmaClrZero(void *dst, Uint32 cnt)
{
    memset(dst, 0x00, cnt);
}


static Uint8 GetComBlockAdr(void)
{
    if(*NOW_ADR_COM_DATA){              /* ??O?????????????????l?????*/
        /* ???R?}???h?u???b?N?A?h???X????? ********************************/
        if(NOW_ADR_COM_DATA >= (MAX_ADR_COM_DATA - SIZE_COM_BLOCK)){
                                                    /* ?o?l???            */
            return OFF;                             /* ??????????      */
        }else{
            adr_com_block += SIZE_COM_BLOCK;        /* ???????????????i???*/
            while(NOW_ADR_COM_DATA < (MAX_ADR_COM_DATA - SIZE_COM_BLOCK)){
                if(*NOW_ADR_COM_DATA){
                    adr_com_block += SIZE_COM_BLOCK;
                }else{
                    return ON;                      /* ???????L??         */
                }
            }
            return OFF;                             /* ??????????         */
        }
    }else{
        adr_com_block = adr_host_int_work;  /* ?????????              */
        while(NOW_ADR_COM_DATA < (MAX_ADR_COM_DATA - SIZE_COM_BLOCK)){
            if(*NOW_ADR_COM_DATA){
                adr_com_block += SIZE_COM_BLOCK;
            }else{
                return ON;                          /* ???????L??         */
            }
        }
        return OFF;                                 /* ??????????         */
    }
}

 SndRet SND_SetTlVl(SndTlVl vol)
 {
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* ?????????????????? */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, vol);                            /* ?p?????[?^?Z?b?g          */
    SET_COMMAND(COM_SET_TL_VL);                 /* ?R?}???h?Z?b?g            */
    HOST_SET_RETURN(SND_RET_SET);
}

 SndRet SND_CtrlDirMidi(SndSeqNum seq_no, SndSeqPri seq_pri, Uint8 md_com,
                        Uint8 ch, Uint8 dt1, Uint8 dt2)
 {
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* ?????????????????? */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, (seq_pri << 3) | md_com);        /* ?p?????[?^?Z?b?g          */
    SET_PRM(1, (seq_no << 5) | ch);             /* ?p?????[?^?Z?b?g          */
    SET_PRM(2, dt1);                            /* ?p?????[?^?Z?b?g          */
    SET_PRM(3, dt2);                            /* ?p?????[?^?Z?b?g          */
    SET_COMMAND(COM_CTRL_DIR_MIDI);             /* ?R?}???h?Z?b?g            */
    HOST_SET_RETURN(SND_RET_SET);
}

void SND_Init2(SndIniDt *sys_ini)
{
    /** BEGIN ****************************************************************/
#ifdef _DMA_SCU
    DMA_ScuInit();                              /* DMA SCU??????????         */
#endif /* _DMA_SCU */

    PER_SMPC_SND_OFF();                         /* ?T?E???hOFF               */
    POKE_W(ADR_SCSP_REG, SCSP_REG_SET); 
                                                /* SCSP??????????         */
    DmaClrZero(ADR_SND_MEM, MEM_CLR_SIZE);      /* DMA???????[???N???A       */
    memcpy(ADR_SND_VECTOR,
                   (void *)(SND_INI_PRG_ADR(*sys_ini)),
                   SND_INI_PRG_SZ(*sys_ini));   /* 68K??????]??            */

    adr_sys_info_tbl = (Uint8 *)(ADR_SND_MEM + PEEK_L(ADR_SYS_TBL +
                                 ADR_SYS_INFO));
                                                /* ??????ð????????    */
    adr_host_int_work = (Uint8 *)(ADR_SND_MEM + PEEK_L(ADR_SYS_TBL +
                                  ADR_HOST_INT));
                                                /* ???????????????    */
    snd_adr_sys_int_work = (Uint8 *)(ADR_SND_MEM + 
                 ((Uint32)PEEK_W(ADR_SYS_TBL + ADR_SYS_INT_WORK) << 16
                  | (Uint32)PEEK_W(ADR_SYS_TBL + ADR_SYS_INT_WORK + 2)));
                                                /* ?????????????????   */
                                                
    adr_com_block = adr_host_int_work;  /* ????????????????????????????? */
                                                
    adr_snd_area_crnt = (Uint32 *)(ADR_SND_MEM + 
                                  PEEK_L(ADR_SYS_TBL + ADR_ARA_CRNT));
                                                /* ?????????CRNT????   */
    adr_song_stat = (Uint16 *)(adr_host_int_work + ADR_SONG_STAT);
    adr_pcm = (Uint16 *)(adr_host_int_work + ADR_PCM);
    adr_seq = (Uint16 *)(adr_host_int_work + ADR_SEQ);
    adr_tl_vl = (Uint16 *)(adr_host_int_work + ADR_TL_VL);
    adr_tl_hz_vl = (Uint16 *)(adr_host_int_work + ADR_TL_HZ_VL);

    memcpy((void *)
                    (PEEK_L(adr_sys_info_tbl + ADR_ARA_ADR) + ADR_SND_MEM),
                   (void *)(SND_INI_ARA_ADR(*sys_ini)),
                   CHG_LONG(SND_INI_ARA_SZ(*sys_ini))); /* ?????????]??  */
/* 1994/02/24 Start*/
    intrflag = 0;         /* ??????t???O??????? */
/* 1994/02/24 End */

    PER_SMPC_SND_ON();                          /* ?T?E???hON                */

}



static void GetSndMapInfo(void **adr, Uint32 **ladr, Uint16 data_kind, Uint16 data_no)
{
    Uint32 i = 0;
    Uint32 map0;

//    adr_snd_area_crnt = (Uint32 *)(ADR_SND_MEM + 
//                                  PEEK_L(ADR_SYS_TBL + ADR_ARA_CRNT));

    map0 = PEEK_L(adr_snd_area_crnt + ARA_MAP_0);

    for(i = 1; (map0 & M_END_MARK) != M_END_MARK; i++){
        if((((map0 & M_DATA_ID) >> B_DATA_ID) == (Uint32)data_kind) &&
           (((map0 & M_ID_NUM) >> B_ID_NUM) == (Uint32)data_no)){
            *adr = (void *)(ADR_SND_MEM +
                       ((map0 & M_START_ADR) >> B_START_ADR)
              );
            *ladr = (Uint32 *)(adr_snd_area_crnt + ARA_MAP_SIZE * (i - 1) +
                     ARA_MAP_4);
            break;
        }
        map0 = PEEK_L(adr_snd_area_crnt + ARA_MAP_SIZE * i + ARA_MAP_0);
    }
}
static void CopyMem(void *dst, void *src, Uint32 cnt)
{
#ifndef _DMA_SCU
	memcpy(dst, src, cnt);
#else
                                                /*****************************/
    DMA_ScuMemCopy(dst, src, cnt);
    while(DMA_SCU_END != DMA_ScuResult());
#endif /* _DMA_SCU */
}
void SND_MoveData(Uint16 *source, Uint32 size,Uint16 data_kind, Uint16 data_no)
{
    void *adr;                                  /* ?]????A?h???X            */
    Uint32 *load_mark_adr;                      /* ?]?????r?b?g??????  */
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"getsndmap    ",4,80);

    GetSndMapInfo(&adr, &load_mark_adr, data_kind, data_no);
                                                /* ????????????      */
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"memcpy    ",4,80);

FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(adr),4,90);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(source),4,100);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(size),4,110);



    CopyMem(adr, (void *)source, size);         /* ?????????]??         */

FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"poke l    ",4,80);

FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(load_mark_adr),4,120);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa((*load_mark_adr | M_LOAD_MARK)),4,130);
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(size),4,110);

	POKE_L(load_mark_adr, (*load_mark_adr | M_LOAD_MARK));
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"poke l done   ",4,80);
                                                /* ?]???????? ??            */
}

 SndRet SND_ChgMix(SndToneBnkNum tone_no, SndMixBnkNum mix_no)
 {
/* 1994/02/24 Start */
#if 0
    HOST_SET_INIT();                            /* ?????????????????? */
#endif
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, tone_no);                        /* ?p?????[?^?Z?b?g          */
    SET_PRM(1, mix_no);                         /* ?p?????[?^?Z?b?g          */
    SET_COMMAND(COM_CHG_MIX);                   /* ?R?}???h?Z?b?g            */
    HOST_SET_RETURN(SND_RET_SET);
}
typedef unsigned char bool;
char	MidiLastKeyOn[ 16 ];
INT16	MidiOneShotCnt[ 16 ];

typedef struct
{
	char	patch;
	char	pitch;
	char	volume;
	char	pan;
} Midi;

Midi MidiStatus[ 16 ];
int	SfxVolume;
typedef struct
{
	char a, b, c, d;
} MidiCmd;

MidiCmd MidiCmdBuf[ 64 ];

int	MidiBufInIdx = 0;
int	MidiBufOutIdx = 0;
int	MidiBufCnt = 0;
bool	MidiBufBusy = TRUE;

/*--------------------------------------------------------------------------*\
\*--------------------------------------------------------------------------*/

void DoMidiCmd( void );
void SendMidiCmd( char a, char b, char c, char d );

void DoMidiInterrupt( void )
{
	if( !MidiBufBusy )
	{
		DoMidiCmd();
		DoMidiCmd();
		DoMidiCmd();
	}
}

typedef struct
{
	char	prog;
	char	key;

} MidiData;

#define	MIDI_PATCH( chan, val )				{ SendMidiCmd( 4, chan,  val,    0 ); }
#define	MIDI_PITCH( chan, val )				{ SendMidiCmd( 6, chan,    0,  val ); }
#define	MIDI_BANK( chan, val )				{ SendMidiCmd( 3, chan,   32,  val ); }
#define	MIDI_VOLUME( chan, val )			{ SendMidiCmd( 3, chan,    7,  val ); }
#define	MIDI_PAN( chan, val )				{ SendMidiCmd( 3, chan,   10,  val ); }
#define	MIDI_KEY( chan, note, velo )		{ SendMidiCmd( 1, chan, note, velo ); }
#define	MIDI_OFF( chan, note )				{ SendMidiCmd( 0, chan, note,  127 ); }

void MidiPatch( int chan, int val )
{
	/* Issuing a 'patch' midi command to the Sega sound driver resets the pitch bend and pan, but not volume. */

	if( MidiStatus[ chan ].patch != val )
	{
		MidiStatus[ chan ].patch	= val;
		MidiStatus[ chan ].pitch	= 64;
		MidiStatus[ chan ].pan		= 64;

		/* The Sega library sometimes seems to 'miss' a patch change. Let's reducee the chance of that happening. */
		MIDI_PATCH( chan, val );
		MIDI_PATCH( chan, val );
	}
}

void MidiBank( int chan, int val )
{
	/* Issuing a 'bank' midi command to the Sega sound driver resets everything. */
	MidiStatus[ chan ].patch	= -1;
	MidiStatus[ chan ].pitch	= 64;
	MidiStatus[ chan ].pan		= 64;
	MidiStatus[ chan ].volume	= 64;
	MIDI_BANK( chan, val );
}

void MidiVolume( int chan, int val )
{
	if( MidiStatus[ chan ].volume != val )
	{
		MidiStatus[ chan ].volume = val;
		val = ( val * SfxVolume ) >> 12;
		MIDI_VOLUME( chan, val );
	}
}

void MidiPan( int chan, int val )
{
	if( MidiStatus[ chan ].pan != val )
	{
		MidiStatus[ chan ].pan = val;
		MIDI_PAN( chan, val );
	}
}

void MidiPitch( int chan, int val )
{
	if( MidiStatus[ chan ].pitch != val )
	{
		MidiStatus[ chan ].pitch = val;
		MIDI_PITCH( chan, val );
	}
}

void _MidiKeyOff( int chan, int note )
{
	MIDI_OFF( chan, note );
}

void _MidiKeyOn( int chan, int note )
{
	MIDI_KEY( chan, note, 64 );
}

void MidiKeyOff( int chan )
{
	if( MidiLastKeyOn[ chan ] )
	{
		_MidiKeyOff( chan, MidiLastKeyOn[ chan ] );
		MidiLastKeyOn[ chan ] = 0;
	}
	MidiOneShotCnt[ chan ] = 0;
}

void MidiKeyOn( int chan, int note )
{
	MidiKeyOff( chan );
	_MidiKeyOn( chan, note );

	MidiLastKeyOn[ chan ] = note;
}

void MidiReset( int chan )
{
	MidiStatus[ chan ].patch	= -1;
	MidiStatus[ chan ].pitch	= -1;
	MidiStatus[ chan ].pan		= -1;
	MidiStatus[ chan ].volume	= -1;

	MidiBank( chan, 0 );
	MidiKeyOff( chan );
	MidiVolume( chan, 127 );
	MidiPan( chan, 64 );
	MidiPitch( chan, 64 );
}

void MidiDuration( int chan, int len )
{
	MidiOneShotCnt[ chan ] = len;
}

/*--------------------------------------------------------------------------*\
\*--------------------------------------------------------------------------*/

void DoMidiCmd( void )
{
	/* Variables. */
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"xxxxxx",4,20);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(MidiBufCnt),4,30);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"xxxxxx",4,40);

	int		a, b, c, d;
	/* Code. */
	if( MidiBufCnt != 0 )
	{
		a = MidiCmdBuf[ MidiBufOutIdx ].a;
		b = MidiCmdBuf[ MidiBufOutIdx ].b;
		c = MidiCmdBuf[ MidiBufOutIdx ].c;
		d = MidiCmdBuf[ MidiBufOutIdx ].d;

		if( 0 == SND_CtrlDirMidi( 0, 0, a, b, c, d ) )
		{
			/* Command successfully transmitted. */
			MidiBufOutIdx = ( MidiBufOutIdx + 1 ) & 63;
			MidiBufCnt -= 1;
		}
	}
}

void SendMidiCmd( char a, char b, char c, char d )
{
	/* Variables. */
	int	was_busy;
	/* Code. */
	was_busy = MidiBufBusy;
	MidiBufBusy = TRUE;
	/* If buffer full, send some data now, to make space. */
	while( MidiBufCnt == 64 ) DoMidiCmd();

	/* Store command parameters. */
	MidiCmdBuf[ MidiBufInIdx ].a = a;
	MidiCmdBuf[ MidiBufInIdx ].b = b;
	MidiCmdBuf[ MidiBufInIdx ].c = c;
	MidiCmdBuf[ MidiBufInIdx ].d = d;

	MidiBufInIdx = ( MidiBufInIdx + 1 ) & 63;
	MidiBufCnt += 1;
	MidiBufBusy = was_busy;
}

enum
{
	ROADKILL_CHAN,
	ROADKILL_CHAN_2,
	CRASH_CHAN,
	CRASH_CHAN_2,
	CRASH_CHAN_3,
	CRASH_CHAN_4,
	SKID_LCHAN,
	SKID_RCHAN,
	HORN_CHAN,
	ENGINE_CHAN,
	ENGINE_NOISE_LCHAN,
	ENGINE_NOISE_RCHAN,
	SPEECH_CHAN,
	DOPPLER_CHAN_0,
	DOPPLER_CHAN_1,
};

MidiData CrashMidiData[] =
{
	{  6, 62 },
	{  4, 55 },
	{  9, 64 },
	{ 10, 60 },
	{  5, 69 },
	{  4, 60 },
	{  6, 54 },
	{  9, 56 },

	{  6, 59 },
	{  9, 60 },
	{ 10, 57 },
	{  5, 63 },
	{  4, 61 },
	{ 11, 64 },
	{  9, 56 },
	{  4, 59 },

	{  6, 62 },
	{  4, 56 },
	{  9, 62 },
	{ 10, 60 },
	{  5, 63 },
	{  4, 61 },
	{  6, 50 },
	{  9, 52 },

	{  6, 62 },
	{  9, 64 },
	{ 10, 53 },
	{  5, 61 },
	{  4, 64 },
	{  6, 54 },
	{  9, 52 },
	{  4, 60 },
};
	int SystemClock;
	int LastCrashClock,CrashCnt;
#define	MIDI_KILL1		0
#define	MIDI_KILL2		1
#define	MIDI_KILL3		2
#define	MIDI_KILL4		67
#define	MIDI_KILL5		68
#define	MIDI_KILL6		36	/* KEY=57 short */
#define	MIDI_KILL7		26	/* KEY=57 short */

#define	MIDI_PICKUP		8

#define MIDI_EXPLOSION	12
#define MIDI_EXPLOSION2	42
#define MIDI_EXPLOSION3	 5

#define	MIDI_Z_RIGHT	14
#define	MIDI_Z_MAKEIT	15
#define	MIDI_Z_LEFT		16
#define	MIDI_Z_DONTSTOP	17
#define	MIDI_Z_GOTBOMB	21
#define	MIDI_Z_NICEMOVE	24
#define	MIDI_Z_UHOH		25
#define	MIDI_Z_WHOAH	26
#define	MIDI_Z_TRYNOT	27
#define	MIDI_Z_NOPLEASE	28
#define	MIDI_Z_WATCHIT	29
#define	MIDI_Z_NONONO	36
#define	MIDI_Z_AIMING	59
#define	MIDI_Z_STRAIGHT	61
#define	MIDI_Z_WRONG	62

#define	MIDI_PLING		57

#define	MIDI_B_BUMPY	18
#define	MIDI_B_HANGON	19
#define	MIDI_B_AIRBAGS	22
#define	MIDI_B_YIPPEE	30
#define	MIDI_B_GOGOGO	31
#define	MIDI_B_SOMEONE	32
#define	MIDI_B_EATTHIS	33
#define	MIDI_B_BUCKLE	37
#define	MIDI_B_LICENSE	40
#define	MIDI_B_OOPS		41
#define	MIDI_B_SORRYPAL	58

#define	MIDI_ENG_CAR	50	// KEY 62
#define	MIDI_ENG_TRUCK	51	// KEY 61
#define	MIDI_ENG_SPORT	52	// KEY 70

#define	MIDI_ENG_CARN	56	// KEY 60
#define	MIDI_ENG_TRUCKN	43

#define	MIDI_HRN_TRUCK	64	/* Key 36 */
#define	MIDI_HRN_CAR	63	/* Key 36 */
#define	MIDI_HRN_SIREN	34
#define	MIDI_CHOPPER	35


#define Z_PAN 64 + 4
#define B_PAN 64 - 4

int	EngineKey;
int	EngineNoiseKey;
int	EngineRev;
int	EnginePitchVal;
int	EnginePitchMax;
int	EnginePitchAdd;
int	EngineType;
int	HornStatus;

void RonsSfxCrash( void )
{
	/* Variables. */

	int	chan;
	int	select;

	/* Code. */

//	if( SfxQuietCnt ) return;

//	if( ( UINT32 )( SystemClock - LastCrashClock ) > 15 )
	{
//		LastCrashClock = SystemClock;

		select = CrashCnt & 31;
		chan = CRASH_CHAN + ( CrashCnt & 3 );
		MidiPatch( chan, CrashMidiData[ select ].prog );
		MidiVolume( chan, 64 );
		MidiPan( chan, 64 - 7 );
		MidiKeyOn( chan, CrashMidiData[ select ].key );
		MidiDuration( chan, 200 );
		CrashCnt++;

		select = CrashCnt & 31;
		chan = CRASH_CHAN + ( CrashCnt & 3 );
		MidiPatch( chan, CrashMidiData[ select ].prog );
		MidiVolume( chan, 64 );
		MidiPan( chan, 64 + 7 );
		MidiKeyOn( chan, CrashMidiData[ select ].key );
		MidiDuration( chan, 200 );
		CrashCnt++;
	}
}
void RonsSfxApplyVolume( void )
{
	/* Variables. */

	int	i, vol;

	/* Code. */

	/* Use this to apply the set volume to running channels. */

	for( i = 0; i < 16; i++ )
	{
		vol = 64;
		MidiStatus[ i ].volume = -1;		/* Force a rewrite. */
		MidiVolume( i, vol );
	}
}
void RonsSfxVolumeTestSound( void )
{
	/* Variables. */

	int	chan;

	/* Code. */

	chan = CRASH_CHAN + ( CrashCnt & 3 );
	MidiStatus[ chan ].volume = -1;			/* Force volume update. */
	MidiPatch( chan, MIDI_PICKUP );
	MidiVolume( chan, 64 );
	MidiPan( chan, 64 );
	MidiKeyOn( chan, 62 );
	MidiDuration( chan, 200 );
	CrashCnt++;
}
static INT32 DrvFMInit()
{
	DrvInitSaturn();

	void * buffer_ptr,	* data_ptr;
	SndIniDt	snd_init;
	int		len, fid;
	data_ptr = buffer_ptr = (void *)0x00200000; //0x6080000;
/*
Map for our example :
char map[] = { 0x00, 0x00, 0xB0, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
                       <>              <------>           <----------->      <------------------>
						\Tone Data   \ Start			\ Size of			\ Map Terminator
						#0 (first)		Address		Tone Bank
*/
#define DRIVER_FNAME	"SDDRVS.TSK"
#define TONE_FNAME		"TONE1.BIN"
#define MAP_FNAME		"MAP.BIN"
#if 1

	char map[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x00, 0x07, 0x4D, 0x8C, 0xFF, 0xFF };
//	char map[] = { 0x00, 0x00, 0xB0, 0x00, 0x00, 0x00,0x00,0x04, 0x02, 0xFF, 0xF0, 0x00, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00 };
	fid		= GFS_NameToId((Sint8 *)DRIVER_FNAME);
	len	= GetFileSize(fid);
	if(fid>0 && len >0)	{GFS_Load(fid, 0, ( void * )data_ptr, len);}
	else {FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"not loaded ",4,70);}

	SND_INI_PRG_ADR( snd_init ) = data_ptr;
	SND_INI_PRG_SZ( snd_init ) = len;

	SND_INI_ARA_ADR(snd_init) 	= (Uint16 *)map;
	SND_INI_ARA_SZ(snd_init) 	= (Uint16)sizeof(map);

	SND_Init2(&snd_init);
	SND_ChgMap(0);

	SfxVolume = ( 4096 + 255 ) >> 1;
#endif
	data_ptr = buffer_ptr;

fid		= GFS_NameToId((Sint8 *)TONE_FNAME);
len	= GetFileSize(fid);
GFS_Load(fid, 0, ( void * )data_ptr, len);

	SND_MoveData( data_ptr, len, SND_KD_TONE, 0 );
	MidiBufBusy = TRUE;
	MidiBufCnt =  0;
#if 0

	for(int i = 0; i < 16; i++ )	{		MidiReset( i );	}
//	while( MidiBufCnt != 0 ) {		DoMidiCmd();	}
	MidiBufBusy = FALSE;
    SND_ChgMix(0,0);
    SND_SetTlVl(15);
#endif

while(1)
	{
MidiBufCnt=0;
MidiBufBusy=FALSE;
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"midi play         ",4,80);
#if 0

	for (int i=0; i<16; i++)
	{
//	MidiBank(i, 0);
//	MidiPatch( i, 0 );
//	MidiVolume( i, 115 );
//	MidiPan( i, 64 );
	MidiKeyOn( i, 56 );
	}
	DoMidiInterrupt();

	PCM_Task();
#endif

FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"midi play  end       ",4,80);
	}
	return 0;
}


#endif


