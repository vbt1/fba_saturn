// FB Alpha Black Tiger driver module
// Based on MAME driver by Paul Leaman

//#include "tiles_generic.h"
//#include "z80_intf.h"
#include "snd/burn_ym2203.h"
//#include "bitswap.h"
#include "d_blktiger.h"
#include    "machine.h"
#define RAZE0 1
#define nYM2203Clockspeed 3579545
#define DEBUG_PCM 1
#define PCM_SFX 1
//#define PCM_MUSIC 1
#define PCM_BLOCK_SIZE 0x4000 // 0x2000
#define	PCM_ADDR	((void*)0x25a20000)
#define	PCM_SIZE	(4096L*2)				/* 2.. */

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
void vbt_PCM_MeTask(PcmHn hn)
{
	PcmWork		*work 	= *(PcmWork **)hn;
	PcmStatus	*st 	= &work->status;
	Sint32		sample_now;

	st->cnt_task_call++;	/* É^ÉXÉNÉRÅ[ÉãÉJÉEÉìÉ^ÇÃÉJÉEÉìÉgÉAÉbÉv */

	/* çƒê∂ÉXÉeÅ[É^ÉXÉ`ÉFÉbÉN */
	if ((st->play <= PCM_STAT_PLAY_PAUSE)
	  ||(st->play >= PCM_STAT_PLAY_END)) {
		return;
	}

	/* ÉwÉbÉ_èàóù */
	if (st->play == PCM_STAT_PLAY_START && st->need_ci != PCM_ON) {
			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pcm_HeaderProcess  ",40,210);

		if (pcm_HeaderProcess(hn)) {
//			_VTV_PRINTF("pcm_HeaderProcess(hn)");
			return;	/* Ç‹ÇæÉwÉbÉ_ï™ÇÃÉfÅ[É^ãüããÇ™Ç»Ç¢ */
		}
	}

	/* ÉIÅ[ÉfÉBÉIèàóù */
//	(*st->audio_process_fp)(hn);
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"vbt_pcm_AudioProcess  ",40,210);

	vbt_pcm_AudioProcess(hn);

	/* çƒê∂èIóπèàóù */
	if (st->play == PCM_STAT_PLAY_TIME) {
		if (PCM_IsRingEmpty(hn)) {
			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"PCM_IsRingEmpty ",40,210);

			PCM_MeGetTimeTotal(hn, &sample_now);
			if (sample_now + PCM_HN_STOP_TRG_SAMPLE(hn) > st->sample_write) {
				/* Ç≥Ç†ÅAçƒê∂èIóπÇæÅI */
			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pcm_EndProcess     ",40,210);
				pcm_EndProcess(hn);
			}
		}
	}

	/* É^ÉCÉ}ÉXÉ^Å[ÉgÅEÇoÇbÇlçƒê∂ÉXÉ^Å[Égèàóù */
	if (st->play == PCM_STAT_PLAY_HEADER) {
			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pcm_StartTimer  ",40,210);
// vbt : redefinir pcm_StartTimer !!!!
		vbt_pcm_StartTimer(hn);
	}
	return;
}
//---------------------------------------------------------------------------------------------------------------
static Uint8 GetComBlockAdr(void)
{
    if(*NOW_ADR_COM_DATA)
	{              /* à»ëOÇÃÃﬁ€Ø∏Ç™à¯Ç´éÊÇËçœÇ›Ç≈Ç»Ç¢Ç©?*/
        /* éüÉRÉ}ÉìÉhÉuÉçÉbÉNÉAÉhÉåÉXê›íËèàóù ********************************/
//        if(NOW_ADR_COM_DATA >= (MAX_ADR_COM_DATA - SIZE_COM_BLOCK)){
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(NOW_ADR_COM_DATA),10,110);

		if(NOW_ADR_COM_DATA >= (MAX_ADR_COM_DATA - SIZE_COM_BLOCK))
		{
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"GetComBlockAdr1 OFF       ",40,100);

	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa((MAX_ADR_COM_DATA - SIZE_COM_BLOCK)),60,110);
// VBT ajout
//		adr_com_block = adr_host_int_work;  /* Ãﬁ€Ø∏ÇÃêÊì™Ç÷              */

													/* ç≈ëÂílÇ©?            */
            return OFF;                             /* Ãﬁ€Ø∏ãÛÇ´ñ≥Çµ      */
        }
		else
		{
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"update adr_com_block                          ",40,110);

            adr_com_block += SIZE_COM_BLOCK;        /* åªç›∫œ›ƒﬁÃﬁ€Ø∏∂≥›ƒ±ØÃﬂ*/
            while(NOW_ADR_COM_DATA < (MAX_ADR_COM_DATA - SIZE_COM_BLOCK))
			{
                if(*NOW_ADR_COM_DATA)
				{
                    adr_com_block += SIZE_COM_BLOCK;
                }
				else
				{
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"GetComBlockAdr1 ON        ",40,110);

                    return ON;                      /* Ãﬁ€Ø∏ãÛÇ´óLÇË         */
                }
            }
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"GetComBlockAdr2 OFF       ",40,110);

            return OFF;                             /* Ãﬁ€Ø∏ãÛÇ´ñ≥Çµ         */
        }
    }
	else
	{
        adr_com_block = adr_host_int_work;  /* Ãﬁ€Ø∏ÇÃêÊì™Ç÷              */
        while(NOW_ADR_COM_DATA < (MAX_ADR_COM_DATA - SIZE_COM_BLOCK))
		{
            if(*NOW_ADR_COM_DATA)
			{
                adr_com_block += SIZE_COM_BLOCK;
            }
			else
			{
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"GetComBlockAdr2 ON        ",40,110);

                return ON;                          /* Ãﬁ€Ø∏ãÛÇ´óLÇË         */
            }
        }
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"GetComBlockAdr3 OFF       ",40,110);

        return OFF;                                 /* Ãﬁ€Ø∏ãÛÇ´ñ≥Çµ         */
    }
}
//---------------------------------------------------------------------------------------------------------------
void vbt_PCM_DrvStartPcm(PcmHn hn)
{
	PcmWork			*work 	= *(PcmWork **)hn;
	PcmPara			*para 	= &work->para;
	PcmStatus		*st 	= &work->status;
	Sint32			oct, shift_freq, fns;
	int				imask;
	SndPcmStartPrm	start_prm;
	SndPcmChgPrm	chg_prm;

	/* chg_prm ÇÃê›íË */
	oct 					= PCM_CALC_OCT(st->info.sampling_rate);
	shift_freq 				= PCM_CALC_SHIFT_FREQ(oct);
	fns 					= PCM_CALC_FNS(st->info.sampling_rate, 
											shift_freq);
	SND_PRM_NUM(chg_prm)	= (SndPcmNum)para->pcm_stream_no;
	SND_PRM_LEV(chg_prm)	= para->pcm_level;
	SND_PRM_PAN(chg_prm)	= para->pcm_pan;
	SND_PRM_PICH(chg_prm)	= PCM_SET_PITCH_WORD(oct, fns);
	SND_R_EFCT_IN(chg_prm)	= 0;
	SND_R_EFCT_LEV(chg_prm)	= 0;
	SND_L_EFCT_IN(chg_prm)	= 0;
	SND_L_EFCT_LEV(chg_prm)	= 0;

	/* start_prm ÇÃê›íË */
	SND_PRM_MODE(start_prm)		= (PCM_IS_MONORAL(st)        ? 0x00 : 0x80) |
								  (PCM_IS_16BIT_SAMPLING(st) ? 0x00 : 0x10); 
	SND_PRM_SADR(start_prm)		= (Uint16)PCM_SET_PCM_ADDR(para);
	SND_PRM_SIZE(start_prm)		= (Uint16)PCM_SET_PCM_SIZE(para);

    imask = get_imask();
    set_imask(15);
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"SND_StartPcm       ",40,210);

	while (vbt_SND_StartPcm(&start_prm, &chg_prm) == SND_RET_NSET) {
		set_imask(imask);
		pcm_Wait(100);
		set_imask(15);
	}
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pcm_InitPolling       ",40,210);

	pcm_InitPolling(para->pcm_stream_no);

    set_imask(imask);
}
//---------------------------------------------------------------------------------------------------------------
void vbt_pcm_StartTimer(PcmHn hn)
{
	PcmWork		*work 	= *(PcmWork **)hn;

	PcmStatus	*st 	= &work->status;
	int			imask;

	if (st->ring_write_offset < PCM_HN_START_TRG_SIZE(hn)) {
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"vbt_pcm_StartTimer1  ",40,210);

		return;
	}

	if (st->sample_write < PCM_HN_START_TRG_SAMPLE(hn)) {
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"vbt_pcm_StartTimer2  ",40,210);

		return;
	}

    imask = get_imask();
    set_imask(15);

	st->clock_scale = PCM_CLOCK_SCALE;
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"PCM_DrvStartPcm  ",40,210);

	vbt_PCM_DrvStartPcm(hn); 	/* PCMçƒê∂äJén */

	/* É^ÉCÉ}Å[ÉXÉ^Å[Ég */
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pcm_StartClock     ",40,210);
	pcm_StartClock(hn);
	st->play = PCM_STAT_PLAY_TIME;
	st->cnt_4ksample = 0;
	st->vbl_film_start = st->vbl_pcm_start = pcm_cnt_vbl_in;

    set_imask(imask);
}
//---------------------------------------------------------------------------------------------------------------
int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvBlktiger = {
		"blktiger", "blktgr",
		"Black Tiger",
		blktigerRomInfo, blktigerRomName, DrvInputInfo, DrvDIPInfo,
		DrvInit, DrvExit, DrvFrame, NULL
	};

	if (strcmp(nBurnDrvBlktiger.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvBlktiger,sizeof(struct BurnDriver));


	adr_host_int_work = (Uint8 *)(ADR_SND_MEM + PEEK_L(ADR_SYS_TBL +  ADR_HOST_INT));	
	adr_com_block = adr_host_int_work;
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

void vout(char *string, char *fmt, ...)                                         
{                                                                               
   va_list arg_ptr;                                                             
   va_start(arg_ptr, fmt);                                                      
   vsprintf(string, fmt, arg_ptr);                                              
   va_end(arg_ptr);                                                             
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
			*soundlatch = data;
			unsigned int i;
			PcmStatus	*st=NULL;

		if(data!=255)
			if(sfx_list[data].loop==0)
			{
				for(i=0;i<8;i++)
				{
					PcmWork		*work = *(PcmWork **)pcm14[i];
					st = &work->status;
					st->cnt_loop = 0;
//					st->audio_process_fp = vbt_pcm_AudioProcess;
					st->need_ci = PCM_OFF;
#ifdef DEBUG_PCM
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
#endif
					if (st->play == PCM_STAT_PLAY_CREATE && i>0) 
//vbt
//					if ((st->play == PCM_STAT_PLAY_CREATE || st->play == PCM_STAT_PLAY_END) && i>0) 
						break;
				}
 #ifdef DEBUG_PCM
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
//				if(i>0 && i<8 && data!=255 && data!=33 && data!=48 && sfx_list[data].size!=0)
				if(sfx_list[data].size!=0)
				{
					pcm_info[i].position = 0;
					pcm_info[i].track_position = sfx_list[data].position;
					pcm_info[i].size = sfx_list[data].size*8;
					pcm_info[i].num = data;
					PcmInfo 		info;

					PCM_INFO_FILE_TYPE(&info) = PCM_FILE_TYPE_NO_HEADER;			
					PCM_INFO_DATA_TYPE(&info)=PCM_DATA_TYPE_RLRLRL;//PCM_DATA_TYPE_LRLRLR;
					PCM_INFO_CHANNEL(&info) = 0x01;
					PCM_INFO_SAMPLING_BIT(&info) = 16;
					PCM_INFO_SAMPLING_RATE(&info)	= SOUNDRATE;//30720L;//44100L;
					PCM_INFO_FILE_SIZE(&info) = sfx_list[data].size;//SOUNDRATE*2;//0x4000;//214896;

					PCM_SetInfo(pcm14[i], &info);
					PCM_MeStart(pcm14[i]);

 #ifdef DEBUG_PCM
					char toto[50];
					char *titi=&toto[0];
					titi=itoa(data);
					FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"       ",70,20+i*10);
					FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)titi,70,20+i*10);
#endif
				}
#endif
			}
			else
			{
					PcmWork		*work = *(PcmWork **)pcm14[0];
					st = &work->status;
					st->cnt_loop = 0;
//					st->audio_process_fp = vbt_pcm_AudioProcess;
					st->need_ci = PCM_OFF;
#ifdef DEBUG_PCM
					if(st->play ==PCM_STAT_PLAY_ERR_STOP)
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"errstp",40,40+i*10);
					else if (st->play ==PCM_STAT_PLAY_CREATE)
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"create",40,40+i*10);
					else if (st->play ==PCM_STAT_PLAY_PAUSE)
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pause ",40,40+i*10);
					else if (st->play ==PCM_STAT_PLAY_START)
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"start ",40,40+i*10);
					else if (st->play ==PCM_STAT_PLAY_HEADER)
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"header",40,40+i*10);
					else if (st->play ==PCM_STAT_PLAY_TIME)
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"playin",40,40+i*10);
					else if (st->play ==PCM_STAT_PLAY_END)
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"end   ",40,40+i*10);
					else
							FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"error ",40,40+i*10);
#endif
				if(	current_pcm != data)
				{
					current_pcm = data;
					st->play = PCM_STAT_PLAY_END;
//					PCM_MeStop(pcm14[0]);
					pcm_EndProcess(pcm14[0]);
//					PCM_GfsReset(pcm14[0]);

					PCM_DestroyStmHandle(pcm14[0]);
					stmClose(stm);
//					STM_ResetTrBuf(stm);
					char pcm_file[14];

					vout(pcm_file, "%03d%s",(int)data,".PCM"); 
					pcm_file[7]='\0';
					PcmInfo 		info;

					PCM_INFO_FILE_TYPE(&info) = PCM_FILE_TYPE_NO_HEADER;			
					PCM_INFO_DATA_TYPE(&info)=PCM_DATA_TYPE_RLRLRL;//PCM_DATA_TYPE_LRLRLR;
					PCM_INFO_CHANNEL(&info) = 0x01;
					PCM_INFO_SAMPLING_BIT(&info) = 16;
					PCM_INFO_SAMPLING_RATE(&info)	= SOUNDRATE;//30720L;//44100L;
#ifdef DEBUG_PCM
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)pcm_file,70,60);
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"gfs_size              ",40,200);
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(sfx_list[data].size),80,200);
#endif
	PCM_INFO_FILE_SIZE(&info) = sfx_list[data].size;//SOUNDRATE*2;//0x4000;//214896;

					if((stm = stmOpen(pcm_file))==NULL)
						FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"stream failed              ",40,210);

					STM_ResetTrBuf(stm);

					pcm14[0] = PCM_CreateStmHandle(&para[0], stm);
//					PCM_MeReset(pcm14[0]);
//STM_MovePickup(stm, 0);

					PCM_SetPcmStreamNo(pcm14[0], 0);
					PCM_SetInfo(pcm14[0], &info);

					work = *(PcmWork **)pcm14[0];
					st->cnt_loop = 0;
//					st->audio_process_fp = vbt_pcm_AudioProcess;
					st->need_ci = PCM_OFF;
					PCM_MeStart(pcm14[0]);
	//						STM_ExecServer();

		/* çƒê∂É^ÉXÉN */
	//	PCM_Task(pcm14[0]);

//st->play = PCM_STAT_PLAY_TIME;
				}
			}
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
			*DrvBgEnable = ~data & 0x02;
			*DrvSprEnable  = ~data & 0x04;
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
#ifdef SND
	DrvZ80ROM1	= Next; Next += 0x008000;
#endif
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	DrvGfxROM0	= SS_CACHE; //Next; Next += 0x020000;
//	DrvGfxROM1	= SS_CACHE + 0x010000;
	DrvGfxROM1	= SS_CACHE + 0x08000;
	DrvGfxROM2	= (UINT8 *)(ss_vram+0x1100);

	DrvPalette	= (UINT16*)colBgAddr; //(UINT32*)Next; Next += 0x0400 * sizeof(UINT32);

	AllRam		= Next;

	DrvZ80RAM0	= Next; Next += 0x001e00;
#ifdef SND
	DrvZ80RAM1	= Next; Next += 0x000800;
#endif
	DrvPalRAM	= Next; Next += 0x000800;
	DrvTxRAM		= Next; Next += 0x000800;
	DrvBgRAM	= Next; Next += 0x004000;
	DrvSprRAM	= Next; Next += 0x001200;
	DrvSprBuf		= Next; Next += 0x001200;

	DrvScreenLayout	= Next; Next += 0x000001;
	DrvBgEnable		= Next; Next += 0x000001;
	DrvFgEnable		= Next; Next += 0x000001;
	DrvSprEnable	= Next; Next += 0x000001;

	DrvVidBank	= Next; Next += 0x000001;
	DrvRomBank	= Next; Next += 0x000001;

	DrvScrollx	= (UINT16*)Next; Next += 0x0001 * sizeof (UINT16);
	DrvScrolly	= (UINT16*)Next; Next += 0x0001 * sizeof (UINT16);

	soundlatch	= Next; Next += 0x000001;
	flipscreen	= Next; Next += 0x000001;
//	coin_lockout	= Next; Next += 0x000001;

	RamEnd			= Next;
#ifdef CZET
	CZ80Context		= Next; Next += (0x1080*2);
#endif
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
		memset(SOUND_BUFFER,0x00,RING_BUF_SIZE*8);
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
#ifdef SND
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
#endif
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
	PCM_MeStop(pcm);

	int fid				= GFS_NameToId((Sint8 *)"SFX.ROM");
	long fileSize	= GetFileSize(fid);

	GFS_Load(fid, 0, (UINT8*)0x00200000, fileSize);

	stmInit();
	stm = stmOpen("048.PCM");
	STM_ResetTrBuf(stm);
	Set14PCM();

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

	nSoundBufferPos=0;
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
	for(int i=0;i<14;i++)
	{
		PCM_MeStop(pcm14[i]);
		memset(SOUND_BUFFER+(PCM_BLOCK_SIZE*(i+1)),0x00,RING_BUF_SIZE*8);
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
DrvZ80RAM0[0xE905-0xe000]= 0x01;
DrvZ80RAM0[0xF424-0xe000]= 0x0F;

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

// streaming de la musique
//	PCM_EntryNext(pcm[1]);
//	PCM_NotifyWriteSize(pcm14[0], 8192);
//		if(stm!=NULL)
#ifdef PCM_MUSIC
		{
			STM_ExecServer();
//	smpStmTask(stm);

//			vbt_PCM_MeTask(pcm14[0]);
			PCM_MeTask(pcm14[0]);
//			if (STM_IsTrBufFull(stm) == TRUE) 
			{
				STM_ResetTrBuf(stm);
			}
		}
#endif

#ifdef PCM_SFX
		for (unsigned int i=1;i<8;i++)
		{
			if(pcm_info[i].position<pcm_info[i].size && pcm_info[i].num != 0xff)
			{
				int size=4096;
				if(pcm_info[i].position+(size*2)>pcm_info[i].size)
				{
//					memcpy((INT16 *)(PCM_ADDR+(PCM_BLOCK_SIZE*(i+1)))+pcm_info[i].position,(INT16*)(0x00200000+pcm_info[i].track_position),size);
					memcpy((INT16 *)(PCM_ADDR+(PCM_BLOCK_SIZE*i))+pcm_info[i].position,(INT16*)(0x00200000+pcm_info[i].track_position),size);
					size=pcm_info[i].size-pcm_info[i].position;
					pcm_info[i].num = 0xff;
				}
				else
//					memcpyl((INT16 *)(PCM_ADDR+(PCM_BLOCK_SIZE*(i+1)))+pcm_info[i].position,(INT16*)(0x00200000+pcm_info[i].track_position),size);
					memcpyl((INT16 *)(PCM_ADDR+(PCM_BLOCK_SIZE*i))+pcm_info[i].position,(INT16*)(0x00200000+pcm_info[i].track_position),size);

//				memcpy((INT16 *)(PCM_ADDR+(0x4000*(i+1)))+pcm_info[i].position,(INT16*)(0x00200000+pcm_info[i].track_position),size);
				pcm_info[i].track_position+=size;
				pcm_info[i].position+=size;
//				if(pcm_info[i].num == 0xff)
//					size=0x900;
				PCM_NotifyWriteSize(pcm14[i], size);
//				PCM_Task(pcm14[i]);
				vbt_PCM_MeTask(pcm14[i]);
			}
			else
			{
#ifdef DEBUG_PCM
				FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"      ",100,40+i*10);
				FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(PCM_HN_CNT_LOOP(pcm14[i])),100,40+i*10);
#endif
//				PCM_MeStop(pcm14[i]);
//					PCM_Task(pcm14[i]);
	//			PCM_Task(pcm14[i]);				
	//								unsigned int 	errChk = 0;
					PcmWork		*work = *(PcmWork **)pcm14[i];
					PcmStatus	*st= &work->status;				
//					while(SND_StopPcm(i))	{if (errChk++ > 512) break;	}
//					PCM_MeStop(pcm14[i]);
				PCM_MeReset(pcm14[i]);
//vbt
//st->play = PCM_STAT_PLAY_END;
//					PCM_MeStop(pcm14[0]);
//vbt
//pcm_EndProcess(pcm14[i]);
//					memcpyl((INT16 *)(PCM_ADDR+(PCM_BLOCK_SIZE*i))+pcm_info[i].position,(INT16*)(0x00200000+pcm_info[i].track_position),size);

//					st->play = PCM_STAT_PLAY_END;
				memset((INT16 *)(PCM_ADDR+(PCM_BLOCK_SIZE*i)),0x00,PCM_BLOCK_SIZE);
//				memset((INT16 *)(PCM_ADDR+(PCM_BLOCK_SIZE*(i+1))),0x00,PCM_BLOCK_SIZE);
			}
		}
#endif
	draw_sprites();
	memcpyl (DrvSprBuf, DrvSprRAM, 0x1200);
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void draw_sprites()
{
	UINT32 delta	= 3;

	for (INT32 offs = 0x200 - 4; offs >= 0; offs -= 4)
	{
		UINT32 attr = DrvSprBuf[offs+1];
		INT32 sx = DrvSprBuf[offs + 3] - ((attr & 0x10) << 4);
		INT32 sy = DrvSprBuf[offs + 2];

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
		delta++;
	}
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
static void Set14PCM()
{
//	PcmCreatePara	para[14];
	PcmInfo 		info[14];
	PcmStatus	*st;
	static PcmWork g_movie_work[14];

	for (int i=0; i<8; i++)
	{
		PCM_PARA_WORK(&para[i])			= (struct PcmWork *)&g_movie_work[i];
//		PCM_PARA_RING_ADDR(&para[i])	= (Sint8 *)PCM_ADDR+0x40000+(0x4000*(i+1));
		if(i==0)
		{
//			PCM_PARA_RING_ADDR(&para[i])	= (Sint8 *)PCM_ADDR+0x64000;
			PCM_PARA_RING_ADDR(&para[i])	= (Sint8 *)PCM_ADDR+0x40000;
			PCM_PARA_RING_SIZE(&para[i])		= RING_BUF_SIZE; //0x20000;


		}
		else
		{
//			PCM_PARA_RING_ADDR(&para[i])	= (Sint8 *)PCM_ADDR+0x40000+(0x4000*(i+1));
			PCM_PARA_RING_ADDR(&para[i])	= (Sint8 *)PCM_ADDR+0x60000+(PCM_BLOCK_SIZE*(i+1));
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
			PCM_INFO_FILE_SIZE(&info[i]) = sfx_list[48].size;//SOUNDRATE*2;//0x4000;//214896;


			STM_ResetTrBuf(stm);
			pcm14[i] = PCM_CreateStmHandle(&para[i], stm);
			PCM_SetPcmStreamNo(pcm14[i], i);
			PCM_SetInfo(pcm14[i], &info[i]);
//			PCM_ChangePcmPara(pcm14[i]);	
		}
		else
		{
			PCM_INFO_SAMPLE_FILE(&info[i]) = RING_BUF_SIZE;//SOUNDRATE*2;//30720L;//214896;
			PCM_INFO_FILE_SIZE(&info[i]) = RING_BUF_SIZE;//SOUNDRATE*2;//0x4000;//214896;
			
			pcm14[i] = createHandle(&para[i]);

			PCM_SetPcmStreamNo(pcm14[i], i);

			PCM_SetInfo(pcm14[i], &info[i]);
			PCM_ChangePcmPara(pcm14[i]);	
		}
// VBT : enleve la lecture en boucle !! merci zeromu!!!
		*(volatile UINT16*)(0x25A00000 + 0x100000 + 0x20 * i) &= ~0x60;
		PCM_Start(pcm14[i]);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void stmInit(void)
{
	STM_Init(12, 24, stm_work);
//	STM_Init(4, 20, stm_work);
//	STM_Init(1, 2, stm_work);

	STM_SetErrFunc(errStmFunc, NULL);

	grp_hd = STM_OpenGrp();
	if (grp_hd == NULL) {
		return;
	}
	STM_SetLoop(grp_hd, STM_LOOP_DFL, STM_LOOP_ENDLESS);
	STM_SetExecGrp(grp_hd);
}
//-------------------------------------------------------------------------------------------------------------------------------------
StmHn stmOpen(char *fname)
{
    Sint32 fid;
	StmKey key;

    fid = GFS_NameToId((Sint8 *)fname);
	STM_KEY_FN(&key) = STM_KEY_CN(&key) = STM_KEY_SMMSK(&key) = 
		STM_KEY_SMVAL(&key) = STM_KEY_CIMSK(&key) = STM_KEY_CIVAL(&key) =
		STM_KEY_NONE;
	return STM_OpenFid(grp_hd, fid, &key, STM_LOOP_READ); //STM_LOOP_NOREAD);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void stmClose(StmHn fp)
{
	STM_Close(fp);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void errStmFunc(void *obj, Sint32 ec)
{
//	VTV_PRINTF((VTV_s, "S:ErrStm %X %X\n", obj, ec));
	char texte[50];
	vout(texte, "ErrStm %X %X",obj, ec); 
	texte[49]='\0';
	do{
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"plante stm                         ",40,130);

	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)texte,70,130);
	wait_vblank();

	}while(1);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void errGfsFunc(void *obj, Sint32 ec)
{
//	VTV_PRINTF((VTV_s, "S:ErrGfs %X %X\n", obj, ec));
	char texte[50];
	vout(texte, "ErrGfs %X %X",obj, ec); 
	texte[49]='\0';
	do{
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"plantÈ gfs",70,130);

	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)texte,70,140);
	wait_vblank();

	}while(1);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void errPcmFunc(void *obj, Sint32 ec)
{
//	VTV_PRINTF((VTV_s, "S:ErrPcm %X %X\n", obj, ec));
	char texte[50];
	vout(texte, "ErrPcm %X %X",obj, ec); 
	texte[49]='\0';
//	do{
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"plantÈ pcm",70,130);

	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)texte,70,140);
	wait_vblank();

//	}while(1);
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
	GFS_SetErrFunc(errGfsFunc, NULL);
	PCM_SetErrFunc(errPcmFunc, NULL);

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
//	nBurnFunction = PCM_VblIn;//smpVblIn;

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
//---------------------------------------------------------------------------------------------------------------
static void vbt_pcm_AudioMix(PcmHn hn)
{
	PcmWork		*work 	= *(PcmWork **)hn;
	PcmStatus	*st 	= &work->status;
	Sint32		size_write, size_write_total;
	Sint32		stock_bsize, 		/* ÉäÉìÉOÉoÉbÉtÉ@ÇÃëçç›å… [byte] 	*/
				size_mono, 			/* ÇPÉ`ÉÉÉìÉlÉãï™ÇÃç›å… [byte/1ch] 	*/
				size_copy; 			/* ÉRÉsÅ[Ç∑ÇÈÉTÉCÉY [byte/1ch] 		*/
	Sint8 		*addr_write1, *addr_write2;

#ifdef _PCMD
	Sint32 		copy_idx, i;
#endif

//	_VTV_PRINTF("pcm_AudioMix");
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pcm_AudioMix       ",40,130);
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(work->para.pcm_stream_no),100,130);

	if (st->ring_write_offset > st->info.file_size) {
		/* ÉäÉìÉOÉoÉbÉtÉ@Ç…ÇÕ EOF à»ç~ÇÃÉSÉ~Ç‡ãüããÇ≥ÇÍÇƒÇ¢ÇÈ */
//		_VTV_PRINTF("pcm_AudioMix st->info.file_size - st->ring_read_offset");
		stock_bsize = st->info.file_size - st->ring_read_offset;
	} else {
//		_VTV_PRINTF("pcm_AudioMix st->ring_write_offset - st->ring_read_offset");
		stock_bsize = st->ring_write_offset - st->ring_read_offset;
	}

	/* ÇPÉ`ÉÉÉìÉlÉãï™ÇÃç›å… [byte/1ch] */
	if (PCM_IS_MONORAL(st)) {
		size_mono = stock_bsize;
	} else {
		size_mono = stock_bsize >> 1;
	}
	if (PCM_IS_8BIT_SAMPLING(st)) {
//		_VTV_PRINTF("\n8bits!!!");
		size_mono &= 0xfffffffe;
		size_mono &= 0xfffffffc;
	} else {
		size_mono &= 0xfffffffc;
	}

	size_mono = MIN(size_mono, 
		PCM_SAMPLE2BSIZE(st, st->info.sample_file - st->sample_write_file));

	/* ÉIÅ[ÉfÉBÉIÉfÅ[É^Ç™ÉçÅ[ÉhÇ≈Ç´ÇÈÇ©ÇÉ`ÉFÉbÉNÇ∑ÇÈ */
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pcm_GetPcmWrite       ",40,140);

	pcm_GetPcmWrite(hn, &addr_write1, &addr_write2, 
						&size_write, &size_write_total);

	if (size_write_total > st->pcm_bsize) {
#ifdef DEBUG_PCM
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"  ",40,150);
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(work->para.pcm_stream_no),40,150);

	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"cnt_load_miss        ",40,160);
//		_VTV_PRINTF("\n\nmissed");
		st->cnt_load_miss++;
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(st->cnt_load_miss),100,160);

	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pcm_bsize            ",40,170);
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(st->pcm_bsize),80,170);

	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"sz_wt_totl            ",40,180);
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(size_write_total),80,180);
#endif

//		VTV_PRINTF((VTV_s, "P:LMis %d\n buf%X < writ%X\n", 
//			st->cnt_load_miss, st->pcm_bsize, size_write_total));
//		VTV_PRINTF((VTV_s, " r%X w%X\n", 
//			(st)->ring_read_offset, (st)->ring_write_offset));
	}
#ifdef DEBUG_PCM
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"file_size              ",40,190);
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(st->info.file_size),80,190);
#endif
	size_copy = MIN(size_mono, size_write_total);


	if (size_copy == 0) {
		return;
	}
	size_copy = MIN(size_copy, st->onetask_size);

	/* ÉRÉsÅ[èÓïÒÉeÅ[ÉuÉãÇÃê›íË */
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pcm_SetMixCopyTbl      ",40,140);
#ifdef DEBUG_PCM
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pcm_SetMixCopyTbl     ",40,210);
#endif
	pcm_SetMixCopyTbl(hn, size_copy);

	/* ÉRÉsÅ[ÇÃé¿çs (èÌÇ…ÅACPU ÉvÉçÉOÉâÉÄì]ëó) */
#ifdef DEBUG_PCM
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pcm_ExecMixCopyTbl      ",40,140);
#endif
	pcm_ExecMixCopyTbl(hn);

	/* ì«Ç›éÊÇËà íuçXêV */
#ifdef DEBUG_PCM
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pcm_RenewRingRead      ",40,140);
#endif
	pcm_RenewRingRead(hn, PCM_1CH2NCH(st, size_copy));
#ifdef DEBUG_PCM
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pcm_AudioMix end      ",40,140);
#endif
	return;
}
//---------------------------------------------------------------------------------------------------------------
 SndRet vbt_SND_StartPcm(SndPcmStartPrm *sprm, SndPcmChgPrm *cprm)
 {
/* 1994/02/24 Start */
// vbt variable statique ...
//    if(intrflag) return(SND_RET_NSET);
//    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, SND_PRM_MODE(*sprm) | SND_PRM_NUM(*cprm));
    SET_PRM(1, (SND_PRM_LEV(*cprm) << 5) | ChgPan(SND_PRM_PAN(*cprm)));
    SET_PRM(2, SND_PRM_SADR(*sprm) >> 8);
    SET_PRM(3, SND_PRM_SADR(*sprm));
    SET_PRM(4, SND_PRM_SIZE(*sprm) >> 8);
    SET_PRM(5, SND_PRM_SIZE(*sprm));
    SET_PRM(6, SND_PRM_PICH(*cprm) >> 8);
    SET_PRM(7, SND_PRM_PICH(*cprm));
    SET_PRM(8, (SND_R_EFCT_IN(*cprm) << 3) | SND_R_EFCT_LEV(*cprm));
    SET_PRM(9, (SND_L_EFCT_IN(*cprm) << 3) | SND_L_EFCT_LEV(*cprm));
    SET_PRM(11, 0);
    SET_COMMAND(COM_START_PCM);                 /* ÉRÉ}ÉìÉhÉZÉbÉg            */
    HOST_SET_RETURN(SND_RET_SET);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void vbt_pcm_AudioProcess(PcmHn hn)
{
	PcmWork		*work 	= *(PcmWork **)hn;
	PcmStatus	*st 	= &work->status;

	if (PCM_IS_LR_MIX(st)) {
		/* ÇkÇqç¨ç›ÉIÅ[ÉfÉBÉIèàóù */
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"vbt_pcm_AudioMix     ",40,210);
		vbt_pcm_AudioMix(hn);
	} else if (PCM_IS_LR_BLOCK(st)) {
		/* ÇkÇqÉuÉçÉbÉNÉIÅ[ÉfÉBÉIèàóù */
		/* pcm_AudioBlock(hn); */
	}
}
//---------------------------------------------------------------------------------------------------------------