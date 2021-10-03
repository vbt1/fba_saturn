// FB Alpha Black Tiger driver module
// Based on MAME driver by Paul Leaman

//#pragma GCC optimize("Os")

#include "d_blktiger.h"
#define RAZE0 1
#define nYM2203Clockspeed 3579545
//#define DEBUG_PCM 1
#define PCM_SFX 1
#define PCM_MUSIC 1

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

<l_oliveira> you're pressing it or not. releasing the key tells the chip to stop producing the sound
<l_oliveira> the CPU will send key off writes to the chips registers making all channels stop producing sound
<l_oliveira> on arcade games it's usually 0x0000 or 0xFFFF for stopping
<l_oliveira> or 0x00 if it's an 8 bit machine

<l_oliveira> some drivers you need to write twice to acknowledge
<l_oliveira> some games write 0xFF then 0x00 to mute
<l_oliveira> 0xFF is just to change the register to tell the driver to expect a command next time the value changes
<l_oliveira> like the CPU keeps pooling the register to see if the value changed and acknowledges only the second change
<l_oliveira> some registers actually fire an interrupt when there's a write and in that case the CPU doesn't need to keep polling so making double write is meaningless
<l_oliveira> depends on the arcade board design
<l_oliveira> I had to deal with that stuff when I modified CAPCOM/Sony Zinc 1 and 2 (PS1 based game boards) sound drivers to run on CPS2, I did create a translator stub code which converts the CPS2 shared RAM command string into the 4 writes on a 8bit port ZN games use to talk with the sound driver
<l_oliveira> the ZN sound drivers fire up a NMI every time the host writes to the I/O port and the driver acknowledges that a command is sent after the interrupt is fired four times
<l_oliveira> CPS2 Z80 just keeps pooling  at an fixed address of the shared ram. I re-purposed the NMI code and put my shared ram pooling stub on the main program loop of the Z80. Every time a command comes at the shared ram it emulates the four NMI behavior in software using the Z80 itself
*/
//---------------------------------------------------------------------------------------------------------------
int ovlInit(char *szShortName)
{
	cleanBSS();

	struct BurnDriver nBurnDrvBlktiger = {
		"blktiger", "blktgr",
		"Black Tiger",
		blktigerRomInfo, blktigerRomName, DrvInputInfo, DrvDIPInfo,
		DrvInit, DrvExit, DrvFrame
	};

	if (strcmp(nBurnDrvBlktiger.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvBlktiger,sizeof(struct BurnDriver));

	ss_reg   = (SclNorscl *)SS_REG;
	ss_regs = (SclSysreg *)SS_REGS;
	ss_regd = (SclDataset *)SS_REGD;
	
	return 0;
}

/*static*/void palette_write(UINT32 offset)
{
	UINT16 data = (DrvPalRAM[offset]) | (DrvPalRAM[offset | 0x400] << 8);

	if(offset >=0x300)
	{
// fg	 offset 300
		unsigned short position = remap4to16_lut[offset&0xff];
		colBgAddr2[position] = cram_lut[data];
	}
	else
	{
// bg 0x000-0xff
// sprites 0x200-0x2ff
		unsigned short position = remap16_lut[offset];

		colBgAddr[position] = cram_lut[data];
	}
}

/*static*/ void DrvRomBankswitch(INT32 bank)
{
	*DrvRomBank = bank & 0x0f;

	UINT32 nBank = 0x10000 + (bank & 0x0f) * 0x4000;

#ifdef RAZE0
	z80_map_read  (0x8000, 0xbfff, DrvZ80ROM0 + nBank);
	z80_map_fetch (0x8000, 0xbfff, DrvZ80ROM0 + nBank);
#endif
}

/*static*/ void DrvVidRamBankswitch(INT32 bank)
{
	*DrvVidBank = (bank & 0x03);
	UINT32 nBank = (bank & 3) * 0x1000;

#ifdef RAZE0
	z80_map_read  (0xc000, 0xcfff, DrvBgRAM + nBank);
	z80_map_fetch (0xc000, 0xcfff, DrvBgRAM + nBank);
#else
	CZetMapArea(0xc000, 0xcfff, 0, DrvBgRAM + nBank);
//	CZetMapArea(0xc000, 0xcfff, 1, DrvBgRAM + nBank);
	CZetMapArea(0xc000, 0xcfff, 2, DrvBgRAM + nBank);
#endif
}

//-------------------------------------------------------------------------------------------------------------------------------------
static inline void updateBgTile2Words(/*INT32 type,*/ UINT32 offs)
{
	UINT32 attr  = DrvBgRAM[(offs<<1) + 1];
	UINT32 color = (attr >> 3) & 0x0f;
	UINT32 code  = DrvBgRAM[(offs<<1)] + ((attr & 0x07) << 8);
	UINT32 flipx = attr & 0x80;

	UINT16 *map2 = (UINT16 *)&ss_map2[bg_map_lut[offs]];
	map2[0] = (flipx << 7) | color;
	map2[1] = (code*4)+0x1000;

/*
01:51:39<derek>	m_bg_tilemap8x4->set_transmask(0, 0xffff, 0x8000);  // split type 0 is totally transparent in front half 
01:51:40<derek>	m_bg_tilemap8x4->set_transmask(1, 0xfff0, 0x800f);  // split type 1 has pens 4-15 transparent in front half 
01:51:41<derek>	m_bg_tilemap8x4->set_transmask(2, 0xff00, 0x80ff);  // split type 1 has pens 8-15 transparent in front half 
01:51:42<derek>	m_bg_tilemap8x4->set_transmask(3, 0xf000, 0x8fff);  // split type 1 has pens 12-15 transparent in front half 
*/
}
//---------------------------------------------------------------------------------------------------------------

void blacktiger_write(UINT16 address, UINT8 data)
{
//	if ((address & 0xf800) == 0xd800) 				  // 	CZetMapArea(0xd800, 0xdfff, 0, DrvPalRAM);
	if (address >= 0xd800 && address <= 0xdfff)
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

UINT8 blacktiger_read(UINT16 address)
{
	return 0;
}

void blacktiger_out(UINT16 port, UINT8 data)
{
	switch (port & 0xff)
	{
		case 0x00:
		{
			*soundlatch = data;
#ifdef PLAY_SFX	
			unsigned int i;
			PcmStatus	*st=NULL;

		if(data!=255)
			if(sfx_list[data].loop==0 && data!=48)
			{
				for(i=0;i<8;i++)
				{
					PcmWork		*work = *(PcmWork **)pcm14[i];
					st = &work->status;
					st->cnt_loop = 0;
//					st->audio_process_fp = vbt_pcm_AudioProcess;
					st->need_ci = PCM_OFF;
#ifdef DEBUG_PCM
if(i==0)
{
					if(st->play ==PCM_STAT_PLAY_ERR_STOP)
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"errstp",40,20+i*10);
					else if (st->play ==PCM_STAT_PLAY_CREATE)
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"create",40,20+i*10);
					else if (st->play ==PCM_STAT_PLAY_PAUSE)
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pause ",40,20+i*10);
					else if (st->play ==PCM_STAT_PLAY_START)
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"start ",40,20+i*10);
					else if (st->play ==PCM_STAT_PLAY_HEADER)
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"header",40,20+i*10);
					else if (st->play ==PCM_STAT_PLAY_TIME)
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"playin",40,20+i*10);
					else if (st->play ==PCM_STAT_PLAY_END)
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"end   ",40,20+i*10);
					else
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"error ",40,40+i*10);
}
#endif
//					if (st->play == PCM_STAT_PLAY_CREATE && i>0) 
				if((st->play == PCM_STAT_PLAY_CREATE || st->play == PCM_STAT_PLAY_END) && i>0)

//vbt
//					if ((st->play == PCM_STAT_PLAY_CREATE || st->play == PCM_STAT_PLAY_END) && i>0) 
						break;
				}
 #ifdef DEBUG_PCM1
				FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"0",10,20);
				if(i==1)
				FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"1",10,30);
				if(i==2)
				FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"2",10,40);
				if(i==3)
				FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"3",10,50);
				if(i==4)
				FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"4",10,60);
				if(i==5)
				FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"5",10,70);
				if(i==6)
				FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"6",10,80);
				if(i==7)
				FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"7",10,90);
				if(i==8)
				FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"8",10,100);
#endif

#ifdef PCM_SFX
				if(sfx_list[data].size!=0)
				{
					pcm_info[i].position = 0;
					pcm_info[i].ring_position = 0;
					pcm_info[i].track_position = sfx_list[data].position;
					pcm_info[i].size = sfx_list[data].size*8;
					pcm_info[i].num = data;
					PcmInfo 		info;

					PCM_INFO_FILE_TYPE(&info) = PCM_FILE_TYPE_NO_HEADER;			
					PCM_INFO_DATA_TYPE(&info)=PCM_DATA_TYPE_RLRLRL;//PCM_DATA_TYPE_LRLRLR;
					PCM_INFO_CHANNEL(&info) = 0x01;
					PCM_INFO_SAMPLING_BIT(&info) = 16;
					PCM_INFO_SAMPLING_RATE(&info)	= SOUNDRATE;//30720L;//44100L;
					PCM_INFO_FILE_SIZE(&info) = pcm_info[i].size; //sfx_list[data].size;//SOUNDRATE*2;//0x4000;//214896;
					PCM_SetInfo(pcm14[i], &info);

					st->play = PCM_STAT_PLAY_HEADER;
				}
#endif
			}
			else
			{
				if(	current_pcm != data)
				{
					current_pcm = data;
					PCM_MeStop(pcm14[0]);
//					pcm_EndProcess(pcm14[0]);
					PCM_DestroyStmHandle(pcm14[0]);
					stmClose(stm);
					UpdateStreamPCM(data, &pcm14[0], &para[0]);
				}
			}
#endif
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
#ifdef SND
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
#endif
//				CZetOpen(0);
			}

//			*flipscreen  =  data & 0x40;
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
			*DrvBgEnable = ~data & 0x02;
/*			*DrvSprEnable  = ~data & 0x04; */
  /*
			ss_regs->dispenbl &= 0xfffD;
			ss_regs->dispenbl |= (*DrvBgEnable >> 1) & 0x0002;
			BGON = ss_regs->dispenbl;			 */
		return;

		case 0x0d:
			DrvVidRamBankswitch(data);
		return;

		case 0x0e:
		{
//			UINT32 plate_addr[4];

			if(data)		  // 1 = 128x64, 0 = 64x128
			{
				bg_map_lut = &bg_map_lut2x1[0];
				PLANEADDR1 = ss_regd->normap[2] = 0x3430;
			}
			else
			{
				bg_map_lut = &bg_map_lut2x2[0];
				PLANEADDR1 = ss_regd->normap[2] = 0x3030;
			}
		}
		return;
	}
}

UINT8 blacktiger_in(UINT16 port)
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

void blacktiger_sound_write(UINT16 address, UINT8 data)
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

UINT8 blacktiger_sound_read(UINT16 address)
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

/*static*/ INT32 MemIndex()
{
	extern unsigned int _malloc_max_ram;
	UINT8 *Next; Next = (unsigned char *)&_malloc_max_ram;
	memset(Next, 0, MALLOC_MAX);
	
	DrvZ80ROM0	= Next; Next += 0x050000;
#ifdef SND
	DrvZ80ROM1	= Next; Next += 0x008000;
#endif
//	DrvPalette	= (UINT16*)colBgAddr; //(UINT32*)Next; Next += 0x0400 * sizeof(UINT32);

	AllRam		= Next;

	DrvZ80RAM0	= Next; Next += 0x001e00;
#ifdef SND
	DrvZ80RAM1	= Next; Next += 0x000800;
#endif
	DrvPalRAM	= Next; Next += 0x000800;
	DrvTxRAM		= Next; Next += 0x000800;
	DrvBgRAM	= Next; Next += 0x004000;
	DrvSprRAM	= Next; Next += 0x000200;
	DrvSprBuf		= Next; Next += 0x000200;

//	DrvScreenLayout	= Next; Next += 0x000001;
	DrvBgEnable		= Next; Next += 0x000001;
	DrvFgEnable		= Next; Next += 0x000001;
//	DrvSprEnable	= Next; Next += 0x000001;

	DrvVidBank	= Next; Next += 0x000001;
	DrvRomBank	= Next; Next += 0x000001;

	DrvScrollx	= (UINT16*)Next; Next += 0x0001 * sizeof (UINT16);
	DrvScrolly	= (UINT16*)Next; Next += 0x0001 * sizeof (UINT16);

	soundlatch	= Next; Next += 0x000002;
//	flipscreen	= Next; Next += 0x000001;
//	coin_lockout	= Next; Next += 0x000001;

	RamEnd			= Next;
#ifdef CZET
	CZ80Context		= (UINT8 *)Next; Next += (sizeof(cz80_struc)*2);
#endif
	remap16_lut		= (UINT16 *)Next; Next += 768 * sizeof (UINT16);
	remap4to16_lut	= (UINT16 *)Next; Next += 256 * sizeof (UINT16);
	cram_lut		= (UINT16 *)Next; Next += 4096 * sizeof (UINT16);
	fg_map_lut		= (UINT16 *)Next; Next += 0x400 * sizeof (UINT16);
	bg_map_lut2x1	= (UINT16 *)Next; Next += 0x2000 * sizeof (UINT16);
	bg_map_lut2x2	= (UINT16 *)Next; Next += 0x2000 * sizeof (UINT16);
#ifdef SND
	ym_buffer		= (INT16*)Next; Next += 4096 * 4 * 2 * sizeof(INT16);
#endif
	MemEnd			= Next;

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ INT32 DrvDoReset(INT32 full_reset)
{
	if (full_reset) {
		memset (AllRam, 0, RamEnd - AllRam);
		memset((void *)SOUND_BUFFER,0x00,RING_BUF_SIZE*8);
	}

#ifdef RAZE0
	z80_reset();
	DrvRomBankswitch(1);
	DrvVidRamBankswitch(1);
#endif

#ifdef SND
#ifndef RAZE
	CZetOpen(1);
	CZetReset();
//	CZetClose();
#else
	z80_reset();
#endif
#endif

#ifdef SND
	BurnYM2203Reset();
#endif
	watchdog = 0;
	current_pcm = 0;
//	__port = PER_OpenPort();

	return 0;
}

/*static*/ INT32 DrvGfxDecode()
{
	INT32 Plane[4] = { ((0x40000 * 8) / 2) + 4, ((0x40000 * 8) / 2) + 0, 4, 0 };
	INT32 XOffs[16] = { 0, 1, 2, 3, 8+0, 8+1, 8+2, 8+3,
			16*16+0, 16*16+1, 16*16+2, 16*16+3, 16*16+8+0, 16*16+8+1, 16*16+8+2, 16*16+8+3 };
	INT32 YOffs[16] = { 0*16, 1*16, 2*16, 3*16, 4*16, 5*16, 6*16, 7*16,
			8*16, 9*16, 10*16, 11*16, 12*16, 13*16, 14*16, 15*16 };

			UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	UINT8 *DrvGfxROM0	= (UINT8 *)SS_CACHE; //Next; Next += 0x020000;
	UINT8 *DrvGfxROM1	= (UINT8 *)(SS_CACHE + 0x08000);
	UINT8 *DrvGfxROM2	= (UINT8 *)(ss_vram+0x1100);

	UINT8 *tmp = (UINT8*)LOWADDR;
	memset(tmp,0x00,0x40000);
// texte
	memcpyl (tmp, DrvGfxROM0, 0x08000);
	GfxDecode4Bpp(0x0800, 2,  8,  8, Plane + 2, XOffs, YOffs, 0x080, tmp, DrvGfxROM0);
	swapFirstLastColor(DrvGfxROM0,0x03,0x10000);
//bg
	memcpyl (tmp, DrvGfxROM1, 0x40000);
	GfxDecode4Bpp(0x0800, 4, 16, 16, Plane + 0, XOffs, YOffs, 0x200, tmp, DrvGfxROM1);
	tile16x16toSaturn(1,0x0800, DrvGfxROM1);

//sprites
	memcpyl (tmp, DrvGfxROM2, 0x40000);
	GfxDecode4Bpp(0x0800, 4, 16, 16, Plane + 0, XOffs, YOffs, 0x200, tmp, DrvGfxROM2);
	swapFirstLastColor(DrvGfxROM1,0x0f,0x40000);	
	swapFirstLastColor(DrvGfxROM2,0x0f,0x40000);	
	return 0;
}

/*static*/ INT32 DrvInit()
{
	DrvInitSaturn();
	MemIndex();

	make_lut();
	{
		for (INT32 i = 0; i < 5; i++) {
			if (BurnLoadRom(DrvZ80ROM0 + i * 0x10000, 0  + i, 1)) return 1;
		}
#ifdef SND
		if (BurnLoadRom(DrvZ80ROM1, 5, 1)) return 1;
#endif

		UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
		UINT8 *DrvGfxROM0	= (UINT8 *)SS_CACHE; //Next; Next += 0x020000;
		UINT8 *DrvGfxROM1	= (UINT8 *)(SS_CACHE + 0x08000);
		UINT8 *DrvGfxROM2	= (UINT8 *)(ss_vram+0x1100);
	
		if (BurnLoadRom(DrvGfxROM0, 6, 1)) return 1;

		for (INT32 i = 0; i < 4; i++) {
			if (BurnLoadRom(DrvGfxROM1 + i * 0x10000, 7  + i, 1)) return 1;
			if (BurnLoadRom(DrvGfxROM2 + i * 0x10000, 11 + i, 1)) return 1;
		}
		DrvGfxDecode();
	}
	PCM_MeStop(pcm);

	int fid			= GFS_NameToId((Sint8 *)"SFX.ROM");
	long fileSize	= GetFileSize(fid);

	GFS_Load(fid, 0, (UINT8*)LOWADDR, fileSize);
#ifdef PCM_MUSIC
	stmInit();
	Set14PCM();
#endif
	drawWindow(0,224,240,0,64);
#ifdef CZET
#ifndef RAZE
	CZetInit2(2,CZ80Context);
#else
	CZetInit2(1,CZ80Context);
#endif
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

#ifdef CZET
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
#endif
#ifdef SND
	BurnYM2203Init(2, ym_buffer, nYM2203Clockspeed, &DrvFMIRQHandler, DrvSynchroniseStream, DrvGetTime, 0);
	BurnTimerAttachZet(nYM2203Clockspeed);
#endif
	DrvDoReset(1);

	for (UINT32 i = 0; i < 0x400; i++) 
	{
		palette_write(i);
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ INT32 DrvExit()
{
	DrvDoReset(1);
#ifdef RAZE0
	z80_stop_emulating();
	z80_add_write(0xc000, 0xdfff, 1, (void *)NULL);
	z80_set_in((unsigned char (*)(unsigned short))NULL);
	z80_set_out((void (*)(unsigned short, unsigned char))NULL);
#endif

#ifdef SND
	BurnYM2203Exit();
#endif
#ifdef CZET
	CZetExit2();
#endif
#ifdef PCM_SFX
	for(unsigned int i=0;i<8;i++)
	{
		PCM_MeStop(pcm14[i]);
	}
	memset((void *)SOUND_BUFFER,0x00,PCM_BLOCK_SIZE*8);
#endif	
#ifdef PCM_MUSIC	
	STM_ResetTrBuf(stm);
	PCM_DestroyStmHandle(pcm14[0]);
	stmClose(stm);
#endif
	CZ80Context = DrvZ80ROM0 = DrvZ80ROM1 = /*DrvGfxROM0 = DrvGfxROM1 = DrvGfxROM2 =*/ NULL;
	DrvZ80RAM0	 = DrvZ80RAM1 = DrvPalRAM = DrvTxRAM = DrvBgRAM = DrvSprRAM = DrvSprBuf = NULL;
	DrvBgEnable = DrvFgEnable = /*DrvSprEnable =*/ DrvVidBank = DrvRomBank	= NULL;
	/*DrvPalette =*/ DrvScrollx	= DrvScrolly = NULL;
	soundlatch = /*flipscreen =*/ NULL;
	remap16_lut = remap4to16_lut	= cram_lut = fg_map_lut = bg_map_lut2x1 = bg_map_lut2x2 = NULL;
	MemEnd = AllRam = RamEnd = NULL;

//	free (AllMem);
//	AllMem = NULL;

	cleanDATA();
	cleanBSS();
    SCL_SET_S0PRIN(0);
	nSoundBufferPos=0;

	SS_SET_N1SPRM(0);
	SclProcess = 2;
	ss_regs->specialcode=0x0000;		
	wait_vblank();
	
	
	
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ INT32 DrvFrame()
{
// cheat code level
//DrvZ80RAM0[0xF3A1-0xe000]=4; // niveau 4 pour transparence
// cheat code invincible
//DrvZ80RAM0[0xE905-0xe000]= 0x01;
//DrvZ80RAM0[0xF424-0xe000]= 0x0F;

//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvFrame start      ",100,40);

*(Uint16 *)0x25E00000 = colBgAddr[0];
	if (watchdog >= 180) {
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvFrame reset      ",100,40);
		
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
//	UINT32 nInterleave = 100;
	UINT32 nInterleave = 20;
	UINT32 nCyclesTotal = 3000000 / 60; ///2;
	UINT32 nCyclesDone = 0;
#ifdef CZET
	CZetNewFrame();
	CZetOpen(1);
#endif
	INT32 nNext, nCyclesSegment;

	for (UINT32 i = 0; i < nInterleave; i++) {

#ifdef RAZE0
		nNext = (i + 1) * nCyclesTotal / nInterleave;
		nCyclesSegment = nNext - nCyclesDone;
		nCyclesDone += z80_emulate(nCyclesSegment);
//		if (i == 98) z80_raise_IRQ(0);
//		if (i == 99) z80_lower_IRQ();
		if (i == 18) z80_raise_IRQ(0);
		if (i == 19) z80_lower_IRQ();
#endif
	}

#ifdef PCM_MUSIC
		playMusic(&pcm14[0]);
#endif

#ifdef PCM_SFX
		for (unsigned int i=1;i<8;i++)
		{
			if(pcm_info[i].position<pcm_info[i].size && pcm_info[i].num != 0xff)
			{
				int size=PCM_COPY_SIZE;
				if(pcm_info[i].position+(size*2)>pcm_info[i].size)
				{
//					memcpy((INT16 *)(PCM_ADDR+(PCM_BLOCK_SIZE*i))+pcm_info[i].position,(INT16*)(0x00200000+pcm_info[i].track_position),size);
					size=pcm_info[i].size-pcm_info[i].position;
					pcm_info[i].num = 0xff;
				}
// VBt : ne pas utiliser de memcpyl				
				memcpy((INT16 *)(PCM_ADDR+(PCM_BLOCK_SIZE*i))+pcm_info[i].ring_position,(INT16*)(0x00200000+pcm_info[i].track_position),size);
				pcm_info[i].track_position+=size;
				pcm_info[i].position+=size;
				pcm_info[i].ring_position+=size;
				if(pcm_info[i].ring_position>=RING_BUF_SIZE)
				{
					pcm_info[i].ring_position=0;
				}
	//				if(PCM_GetPlayStatus(pcm14[i]) != PCM_STAT_PLAY_CREATE && PCM_GetPlayStatus(pcm14[i]) != PCM_STAT_PLAY_END)
					{
						PCM_NotifyWriteSize(pcm14[i], size);
						PCM_Task(pcm14[i]);
//						vbt_PCM_MeTask(pcm14[i]);
					}
			}
			else
			{
//#ifdef DEBUG_PCM
//				FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"      ",100,40+i*10);
//				FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(PCM_HN_CNT_LOOP(pcm14[i])),100,40+i*10);
//#endif
//				if(PCM_GetPlayStatus(pcm14[i]) != PCM_STAT_PLAY_CREATE)
				{
//if(i==0)
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)" stopping pcm 0 !!!!     ",100,80);
					PCM_MeStop(pcm14[i]);

//					memset((INT16 *)(PCM_ADDR+(PCM_BLOCK_SIZE*i)),0x00,RING_BUF_SIZE);
				}
			}
		}
#endif
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvFrame draw_sprites      ",100,40);

	draw_sprites();
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvFrame memcpyl      ",100,40);

	memcpyl (DrvSprBuf, DrvSprRAM, 0x200);
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvFrame end        ",100,40);
	
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void draw_sprites()
{
//	UINT32 delta	= 3;
	SprSpCmd *ss_spritePtr = &ss_sprite[3];

	for (INT32 offs = 0x200 - 4; offs >= 0; offs -= 4)
	{
		UINT32 attr = DrvSprBuf[offs+1];
		INT32 sx = DrvSprBuf[offs + 3] - ((attr & 0x10) << 4);
		INT32 sy = DrvSprBuf[offs + 2];

		if (sy < -15 || sy > 239 || sx < -15 || sx > 255)
		{
			ss_spritePtr->charSize		= 0;
		}
		else
		{
			UINT32 code = DrvSprBuf[offs] | ((attr & 0xe0) << 3);
			UINT32 flipx = ((attr & 0x08) << 1);

			ss_spritePtr->control	= ( JUMP_NEXT | FUNC_NORMALSP) | flipx;
			ss_spritePtr->drawMode	= ( ECD_DISABLE | COMPO_REP);

			ss_spritePtr->ax			= sx;
			ss_spritePtr->ay			= sy;
			ss_spritePtr->charSize	= 0x210;
			ss_spritePtr->color		= (attr & 0x07)<<4;
			ss_spritePtr->charAddr	= 0x220+(code<<4);
		}
//		delta++;
		ss_spritePtr++;
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

	scfg.plate_addr[0] = (Uint32)(SS_MAP2);
	scfg.plate_addr[1] = (Uint32)(SS_MAP2+0x1000);
	scfg.plate_addr[2] = (Uint32)(SS_MAP2+0x1000);	 // good	  0x400
	scfg.plate_addr[3] = (Uint32)(SS_MAP2+0x1000);

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
/*static*/ void initColors()
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
#ifdef PCM_SFX
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ PcmHn createHandle(PcmCreatePara *para)
{
	PcmHn pcm;

	pcm = PCM_CreateMemHandle(para);
	if (pcm == NULL) {
		return NULL;
	}
	return pcm;
}
//-------------------------------------------------------------------------------------------------------------------------------------

/*static*/ void Set14PCM()
{
//	PcmCreatePara	para[14];
	PcmInfo 		info[8];
	static PcmWork g_movie_work[8];
	PcmStatus	*st;

	for (UINT32 i=0; i<8; i++)
	{
		PCM_PARA_WORK(&para[i])			= (struct PcmWork *)&g_movie_work[i];
//		PCM_PARA_RING_ADDR(&para[i])	= (Sint8 *)PCM_ADDR+0x40000+(0x4000*(i+1));
#if 0
		if(i==0)
		{
//			PCM_PARA_RING_ADDR(&para[i])	= (Sint8 *)PCM_ADDR+0x64000;
			PCM_PARA_RING_ADDR(&para[i])	= (Sint8 *)PCM_ADDR+0x40000;
			PCM_PARA_RING_SIZE(&para[i])		= RING_BUF_SIZE; //0x20000;


		}
		else
#endif
		{
//			PCM_PARA_RING_ADDR(&para[i])	= (Sint8 *)PCM_ADDR+0x40000+(0x4000*(i+1));
//			PCM_PARA_RING_ADDR(&para[i])	= (Sint8 *)PCM_ADDR+0x60000+(PCM_BLOCK_SIZE*i);
			PCM_PARA_RING_ADDR(&para[i])	= (Sint8 *)PCM_ADDR+0x40000+(PCM_BLOCK_SIZE*i);
			PCM_PARA_RING_SIZE(&para[i])		= RING_BUF_SIZE;//<<1;
		}
//		PCM_PARA_PCM_ADDR(&para[i])	= PCM_ADDR+(0x4000*(i+1));
		PCM_PARA_PCM_ADDR(&para[i])	= PCM_ADDR+(PCM_BLOCK_SIZE*i); //*(i+1));
		PCM_PARA_PCM_SIZE(&para[i])		= PCM_SIZE;

		memset((Sint8 *)SOUND_BUFFER,0,SOUNDRATE*16);
		st = &g_movie_work[i].status;
		st->need_ci = PCM_OFF;
//		st->audio_process_fp = vbt_pcm_AudioProcess;
	 
		PCM_INFO_FILE_TYPE(&info[i]) = PCM_FILE_TYPE_NO_HEADER;			
		PCM_INFO_DATA_TYPE(&info[i])=PCM_DATA_TYPE_RLRLRL;//PCM_DATA_TYPE_LRLRLR;
		PCM_INFO_CHANNEL(&info[i]) = 0x01;
		PCM_INFO_SAMPLING_BIT(&info[i]) = 16;

		PCM_INFO_SAMPLING_RATE(&info[i])	= SOUNDRATE;//30720L;//44100L;

		if(i==0)
		{
//			int fid		= GFS_NameToId("048.pcm");	
//			int fz = GetFileSize(fid);
			PCM_INFO_FILE_SIZE(&info[i]) = sfx_list[0].size;//SOUNDRATE*2;//0x4000;//214896;

#ifdef PCM_MUSIC
			STM_ResetTrBuf(stm);
			pcm14[i] = PCM_CreateStmHandle(&para[i], stm);
			PCM_SetPcmStreamNo(pcm14[i], i);
			PCM_SetInfo(pcm14[i], &info[i]);
// vbt ajout 
//			PCM_SetPcmCmdBlockNo(pcm14[i], i);
//			*(volatile UINT16*)(0x25A00000 + 0x100000 + 0x20 * i) &= 0xFF9F;//~0x60;
			PCM_ChangePcmPara(pcm14[i]);
#endif			
		}
		else
		{
			PCM_INFO_SAMPLE_FILE(&info[i]) = RING_BUF_SIZE;//SOUNDRATE*2;//30720L;//214896;
			PCM_INFO_FILE_SIZE(&info[i]) = PCM_COPY_SIZE;//SOUNDRATE*2;//0x4000;//214896;
#ifdef PCM_MUSIC			
			pcm14[i] = createHandle(&para[i]);

			PCM_SetPcmStreamNo(pcm14[i], i);

			PCM_SetInfo(pcm14[i], &info[i]);
// vbt : ajout
//			PCM_SetPcmCmdBlockNo(pcm14[i], i);
//			*(volatile UINT16*)(0x25A00000 + 0x100000 + 0x20 * i) &= 0xFF9F;//~0x60;
			PCM_ChangePcmPara(pcm14[i]);
#endif			
		}
// VBT : enleve la lecture en boucle !! merci zeromu!!!
		PCM_Start(pcm14[i]);
	}
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void make_lut(void)
{
	UINT32 i,delta=0;

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
	UINT32 sx, sy;
	UINT16* bm_2x1 = (UINT16*)bg_map_lut2x1;
	UINT16* bm_2x2 = (UINT16*)bg_map_lut2x2;

	for (UINT32 offs = 0; offs < 0x100; offs++)
	{
		sx = (offs & 0x0f);
		sy = ((offs) >> 4)<<5;
		bm_2x1[0x0000] = bm_2x2[0x0000] = (sx+0x0000)|sy;
		bm_2x1[0x0100] = bm_2x2[0x0100] = (sx+0x0010)|sy;
		bm_2x1[0x0200] = bm_2x2[0x0200] = (sx+0x0400)|sy;
		bm_2x1[0x0300] = bm_2x2[0x0300] = (sx+0x0410)|sy;

		bm_2x1[0x1000] = bm_2x2[0x0800] = (sx+0x0800)|sy;
		bm_2x1[0x1100] = bm_2x2[0x0900] = (sx+0x0810)|sy;
		bm_2x1[0x1200] = bm_2x2[0x0A00] = (sx+0x0C00)|sy;
		bm_2x1[0x1300] = bm_2x2[0x0B00] = (sx+0x0C10)|sy;
		
		bm_2x1[0x0400] = bm_2x2[0x1000] = (sx+0x1000)|sy;
		bm_2x1[0x0500] = bm_2x2[0x1100] = (sx+0x1010)|sy;
		bm_2x1[0x0600] = bm_2x2[0x1200] = (sx+0x1400)|sy;
		bm_2x1[0x0700] = bm_2x2[0x1300] = (sx+0x1410)|sy;

		bm_2x1[0x1400] = bm_2x2[0x1800] = (sx+0x1800)|sy;
		bm_2x1[0x1500] = bm_2x2[0x1900] = (sx+0x1810)|sy;
		bm_2x1[0x1600] = bm_2x2[0x1A00] = (sx+0x1C00)|sy;
		bm_2x1[0x1700] = bm_2x2[0x1B00] = (sx+0x1C10)|sy;
		
		sy = ((offs+0x100) >> 4)<<5;
		bm_2x1[0x0800] = bm_2x2[0x0400] = (sx+0x0000)|sy;
		bm_2x1[0x0900] = bm_2x2[0x0500] = (sx+0x0010)|sy;
		bm_2x1[0x0A00] = bm_2x2[0x0600] = (sx+0x0400)|sy;
		bm_2x1[0x0B00] = bm_2x2[0x0700] = (sx+0x0410)|sy;
		
		bm_2x1[0x1800] = bm_2x2[0x0C00] = (sx+0x0800)|sy;
		bm_2x1[0x1900] = bm_2x2[0x0D00] = (sx+0x0810)|sy;
		bm_2x1[0x1A00] = bm_2x2[0x0E00] = (sx+0x0C00)|sy;
		bm_2x1[0x1B00] = bm_2x2[0x0F00] = (sx+0x0C10)|sy;
		
		bm_2x1[0x0C00] = bm_2x2[0x1400] = (sx+0x1000)|sy;
		bm_2x1[0x0D00] = bm_2x2[0x1500] = (sx+0x1010)|sy;
		bm_2x1[0x0E00] = bm_2x2[0x1600] = (sx+0x1400)|sy;
		bm_2x1[0x0F00] = bm_2x2[0x1700] = (sx+0x1410)|sy;

		bm_2x1[0x1C00] = bm_2x2[0x1C00] = (sx+0x1800)|sy;
		bm_2x1[0x1D00] = bm_2x2[0x1D00] = (sx+0x1810)|sy;
		bm_2x1[0x1E00] = bm_2x2[0x1E00] = (sx+0x1C00)|sy;
		bm_2x1[0x1F00] = bm_2x2[0x1F00] = (sx+0x1C10)|sy;
		
		bm_2x1++;
		bm_2x2++;
	}
//---------------------------------------------------------------------------------
// commenter si  1 word
	for (UINT32 offs = 0; offs < 0x2000; offs++) // page 8 16x16
	{
		bg_map_lut2x2[offs] = bg_map_lut2x2[offs] * 2;
		bg_map_lut2x1[offs] = bg_map_lut2x1[offs] * 2;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn()
{
//	SPR_InitSlaveSH();
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
#ifdef DEBUG
	GFS_SetErrFunc(errGfsFunc, NULL);
	PCM_SetErrFunc(errPcmFunc, NULL);
#endif
 	SS_MAP  = ss_map	=(Uint16 *)SCL_VDP2_VRAM_B1+0xC000;		   //c
	SS_MAP2 = ss_map2	=(Uint16 *)SCL_VDP2_VRAM_B1+0x8000;			//8000
// 	SS_MAP  = ss_map	=(Uint16 *)SCL_VDP2_VRAM_B1;		   //c
//	SS_MAP2 = ss_map2	=(Uint16 *)SCL_VDP2_VRAM_B1+0x8000;			//8
	SS_FONT = ss_font	=(Uint16 *)SCL_VDP2_VRAM_B1+0x0000;
//SS_FONT = ss_font		=(Uint16 *)NULL;
	SS_CACHE= cache		=(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum	 = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum	 = (SclSpPriNumRegister *)SS_SPPRI;
	ss_OtherPri	= (SclOtherPriRegister *)SS_OTHR;
	ss_BgColMix	= (SclBgColMixRegister *)SS_BGMIX;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_scl			= (Fixed32 *)SS_SCL;
#ifdef PCM_SFX	
	sfx_list		= &sfx_blktiger[0];
#endif
	nBurnLinescrollSize = 1;
	nBurnSprites = 128+3;
//	nBurnFunction = PCM_VblIn;//smpVblIn;

//3 nbg
	SS_SET_N0PRIN(7); // window
	SS_SET_N1PRIN(4); // bg
	SS_SET_N2PRIN(6); // fg
	SS_SET_S0PRIN(4); // sp

	SS_SET_N1SPRM(1);  // 1 for special priority
	ss_regs->specialcode=0x000e; // sfcode, upper 8bits, function b, lower 8bits function a
	ss_regs->specialcode_sel=0; // sfsel, bit 0 for nbg0 // 1 sfcs, bit 0 = 1 for funcion code b, 0 for function code a

	initLayers();
	initColors();
	initSprites(256-1,224-1,0,0,0,-16);

	SCL_Open();
	ss_reg->n2_move_y =  16;
	SCL_Close();


	memset((Uint8 *)SCL_VDP2_VRAM_B1  ,0x22,0x10000);
	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Loading. Please Wait",24,40);
	memset((Uint8 *)ss_map2,0x11,0x10000);
	SprSpCmd *ss_spritePtr;
	
	for (unsigned int i = 3; i <nBurnSprites; i++) 
	{
		ss_spritePtr				= &ss_sprite[i];
		ss_spritePtr->control   = ( JUMP_NEXT | FUNC_NORMALSP);
		ss_spritePtr->drawMode  = ( ECD_DISABLE | COMPO_REP);	// 16 couleurs
		ss_spritePtr->charSize  = 0x210;  //0x100 16*16
	}
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------
void tile16x16toSaturn (unsigned char reverse, unsigned int num, unsigned char *pDest)
{
	unsigned int c;
	UINT8 *dpM = pDest;	
		
	for (c = 0; c < num; c++) 
	{
		unsigned char xnew_tile[128];

		memcpyl(xnew_tile,dpM,128);
		unsigned char i=0,k=0;
		unsigned char *new_tile=(unsigned char *)xnew_tile;

		for (k=0;k<2;k++)
		{
			for (i=0;i<8;i++)
			{
				if(reverse)
				{
					dpM[0]=*new_tile++;
					dpM[1]=*new_tile++;
					dpM[2]=*new_tile++;
					dpM[3]=*new_tile++;
					dpM[32]=*new_tile++;
					dpM[33]=*new_tile++;
					dpM[34]=*new_tile++;
					dpM[35]=*new_tile++;
					dpM+=4;
				}
				else
				{					
					dpM[32]=*new_tile++;
					dpM[33]=*new_tile++;
					dpM[34]=*new_tile++;
					dpM[35]=*new_tile++;
					dpM[0]=*new_tile++;
					dpM[1]=*new_tile++;
					dpM[2]=*new_tile++;
					dpM[3]=*new_tile++;
					dpM+=4;
				}
			}
			dpM+=32;
		}
	}
}
