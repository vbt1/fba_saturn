#include    "machine.h"
#include "d_sms.h"
#define OLD_SOUND 1
#define SOUNDRATE 7680L
#define TWO_WORDS 1
#define MAX_DIR 384*2
//GfsDirName dir_name_sms[512];
#ifdef GG0
unsigned char *disp_spr = NULL;
unsigned char curr_sprite=0;
#endif

#define PONY

#ifdef PONY
#include "saturn/pcmstm.h"

int pcm1=-1;
Sint16 *nSoundBuffer=NULL;
extern unsigned int frame_x;
extern unsigned int frame_y;
#endif

/* Attribute expansion table */
//-------------------------------------------------------------------------------------------------------------------------------------
int ovlInit(char *szShortName)
{
	cleanBSS();
#ifdef RAZE
	struct BurnDriver nBurnDrvsms_akmw = {
		"sms", NULL,
		"Sega Master System (Faze)",
		sms_akmwRomInfo, sms_akmwRomName, SMSInputInfo, SMSDIPInfo,
//		sms_akmwRomInfo, sms_akmwRomName, NULL, SMSDIPInfo,
		SMSInit, SMSExit, SMSFrame
	};
#else
#ifdef CZ80
	struct BurnDriver nBurnDrvsms_akmw = {
		"smscz", "sms",
		"Sega Master System (CZ80)",
		sms_akmwRomInfo, sms_akmwRomName, SMSInputInfo, SMSDIPInfo,
//		sms_akmwRomInfo, sms_akmwRomName, NULL, SMSDIPInfo,
		SMSInit, SMSExit, SMSFrame
	};
#else
#ifdef CZ80
	struct BurnDriver nBurnDrvsms_akmw = {
		"ggcz", "gg",
		"Sega Game Gear (CZ80)",
		sms_akmwRomInfo, sms_akmwRomName, SMSInputInfo, SMSDIPInfo,
//		sms_akmwRomInfo, sms_akmwRomName, NULL, SMSDIPInfo,
		SMSInit, SMSExit, SMSFrame
	};
#else
	struct BurnDriver nBurnDrvsms_akmw = {
		"gg", NULL,
		"Sega Game Gear (Faze)",
		sms_akmwRomInfo, sms_akmwRomName, SMSInputInfo, SMSDIPInfo,
//		sms_akmwRomInfo, sms_akmwRomName, NULL, SMSDIPInfo,
		SMSInit, SMSExit, SMSFrame
	};
#endif
#endif
#endif
/*struct BurnDriver * */ //fba_drv = 	(struct BurnDriver *)FBA_DRV;
	memcpy(shared,&nBurnDrvsms_akmw,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
//	ss_regs  = (SclSysreg *)SS_REGS;
	return 0;
}
//--------------------------------------------------------------------------------------------------------------------------------------
static Sint32 GetFileSize(int file_id)
{
	GfsHn gfs;
    Sint32 sctsize, nsct, lastsize;
    
    gfs = GFS_Open(file_id);
    GFS_GetFileSize(gfs, &sctsize, &nsct, &lastsize);

    GFS_Close(gfs);
	return (sctsize*(nsct-1) + lastsize);
}
//--------------------------------------------------------------------------------------------------------------------------------------
static void ChangeDir(char *dirname)
{
    Sint32 fid;
	GfsDirTbl dirtbl;
	static GfsDirId dir_name[MAX_DIR];
//	char dir_upr[12];
//	char *ptr=&dir_upr[0];
//	strcpy(dir_upr,dirname);
//	ptr = strupr(ptr);
//    fid = GFS_NameToId((Sint8 *)dir_upr);
    fid = GFS_NameToId((Sint8 *)dirname);
//	ptr = NULL;

	GFS_DIRTBL_TYPE(&dirtbl) = GFS_DIR_NAME;
	GFS_DIRTBL_DIRNAME(&dirtbl) = dir_name;
	GFS_DIRTBL_NDIR(&dirtbl) = MAX_DIR;

//	for (;;) {
    file_max = GFS_LoadDir(fid, &dirtbl)-2;
	GFS_SetDir(&dirtbl) ;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void	SetVblank2( void )
{
	__port = PER_OpenPort();
}
//-------------------------------------------------------------------------------------------------------------------------------------
 void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr		= (Uint16*)SCL_AllocColRam(SCL_NBG0,OFF);
	colAddr			= (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	SCL_AllocColRam(SCL_NBG1,OFF);
//	SCL_SetColRam(SCL_NBG1,0,8,palette);
	SCL_SetColRam(SCL_NBG1,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
#ifndef OLD_SOUND
 void sh2slave(unsigned int *nSoundBufferPosN)
{xxxx
#ifndef PONY	
	volatile signed short *nSoundBuffer = (signed short *)SOUND_BUFFER;
	SN76496Update(0, &nSoundBuffer[nSoundBufferPos],  128);
//	PSG_Update(&nSoundBuffer[nSoundBufferPos],  128);
	nSoundBufferPos+=128;
	if(nSoundBufferPos>=SAMPLE)//<<1)//256*hz)
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos);
		nSoundBufferPos=0;
	}
	PCM_Task(pcm);
#else
	yyyyyyyyyyyyyyyyyyyyyy
#endif
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
 void initLayers(void)
{
//    SclConfig	config;
// **29/01/2007 : VBT sauvegarde cycle patter qui fonctionne jusqu'à maintenant

	Uint16	CycleTb[]={
		  // VBT 04/02/2007 : cycle pattern qui fonctionne just test avec des ee
//#ifdef GG
#if 0
		0xffff, 0xff5e, //A1
		0xffff, 0xffff,	//A0
		0x0044, 0xeeff,   //B1
		0xffff, 0xffff  //B0
#else
		0xff5e, 0xeeee, //A1
		0xffee, 0xeeee,	//A0
		0x04ee, 0xeeee,   //B1
		0xffee, 0xeeee  //B0
#endif
	};
 	SclConfig	scfg;

//	SCL_InitConfigTb(&scfg);
	scfg.dispenbl		= ON;
	scfg.charsize		= SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
#ifdef TWO_WORDS
	scfg.pnamesize	= SCL_PN2WORD;
#else
	scfg.pnamesize	= SCL_PN1WORD;
	scfg.flip				= SCL_PN_10BIT;
#endif
	scfg.platesize	= SCL_PL_SIZE_1X1;
//#ifdef GG
#if 0
	scfg.coltype		= SCL_COL_TYPE_256;
#else
	scfg.coltype		= SCL_COL_TYPE_16;
#endif
	scfg.datatype	= SCL_CELL;
// vbt : active le prioritybit en mode 1word
//	scfg.patnamecontrl =  0x020c;// VRAM B1 ‚ÌƒIƒtƒZƒbƒg 
	scfg.patnamecontrl =  0x000c;// VRAM B1 ‚ÌƒIƒtƒZƒbƒg 
	scfg.plate_addr[0] = (UINT32 *)SS_MAP;
	SCL_SetConfig(SCL_NBG0, &scfg);
/********************************************/	

//	SCL_InitConfigTb(&scfg);
//	scfg.dispenbl 	 = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
#ifdef GG
	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
#endif
	scfg.datatype 	 = SCL_BITMAP;
	scfg.mapover       = SCL_OVER_0;
	scfg.plate_addr[0] = (UINT32 *)SS_FONT;
	SCL_SetConfig(SCL_NBG1, &scfg);
	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline  void initPosition(void)
{
	SCL_Open();
	ss_reg->n1_move_x = 0;
	ss_reg->n1_move_y = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline  void SaturnInitMem()
{
	SPR_InitSlaveSH();
	
	extern unsigned int _malloc_max_ram;
	UINT8 *Next; Next = (unsigned char *)&_malloc_max_ram;
	memset(Next, 0, MALLOC_MAX);

//	vdp.vram		= (UINT8 *)Next; Next += 0x4000*sizeof(UINT8);	
	name_lut		= (UINT16 *)Next; Next += 0x10000*sizeof(UINT16);
	bp_lut			= Next; Next += 0x10000*sizeof(UINT32);
#ifdef GG
	cram_lut		= (UINT16 *)Next; Next += 0x1000*sizeof(UINT16);
	vram_dirty		= Next; Next += 0x200;
#else
	cram_lut		= (UINT16 *)Next; Next += 0x40*sizeof(UINT16);
#endif
#ifdef GG0
	disp_spr		= Next; Next += 64;
#endif
	map_lut	 		= (UINT16 *)Next; Next += 0x800*sizeof(UINT16);

	dummy_write= Next; Next += 0x100*sizeof(UINT8);
	CZ80Context	= Next; Next += sizeof(cz80_struc);
	sms256kbRom	= Next;
}

#ifdef PONY
extern SclLineparam lp;

static void SCL_SetLineParamNBG0(SclLineparam *lp)
{
	Uint32	*addr;
	addr = &Scl_n_reg.lineaddr[0];
	*addr = (lp->line_addr >>1) & 0x0007ffff;
	SclProcess = 2; //obligatoire
}

void vbl()
{
	update_input1();
//	sdrv_vblank_rq();
	SCL_SetLineParamNBG0(&lp);
	sdrv_stm_vblank_rq();
}
#endif	

//-------------------------------------------------------------------------------------------------------------------------------------
 void DrvInitSaturn()
{
//	SPR_InitSlaveSH();
	nBurnSprites  = 67;//131;//27;
	nBurnLinescrollSize = 0x340;
	nSoundBufferPos = 0;//sound position à renommer

	SS_CACHE  = (Uint8  *)SCL_VDP2_VRAM_B1;
	SS_MAP    = (Uint16 *)SCL_VDP2_VRAM_B0;
	SS_FONT   = (Uint16 *)SCL_VDP2_VRAM_A1;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;
#ifdef TWO_WORDS
	ss_OtherPri       = (SclOtherPriRegister *)SS_OTHR;
#endif
	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_scl			= (Fixed32 *)SS_SCL;

	file_id			= 2; // bubble bobble
//	file_max		= getNbFiles();
		//8;//aleste
	initLayers();
	initColors();
	initPosition();

	SaturnInitMem();	
	
//	SaturnInitMem();
//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)" ",0,180);	
	make_lut();
#ifdef TWO_WORDS
    SS_SET_N0PRIN(4);
    SS_SET_S0PRIN(4);	
	SS_SET_N0SPRM(1);  // 1 for special priority	  // mode 2 pixel, mode 1 pattern, mode 0 by screen
	ss_regs->specialcode=0x000e; // sfcode, upper 8bits, function b, lower 8bits function a
	ss_regs->specialcode_sel=0; // sfsel, bit 0 for nbg0 // 1 sfcs, bit 0 = 1 for funcion code b, 0 for function code a
	SclProcess = 2; 
#else
    SS_SET_N0PRIN(5);
    SS_SET_S0PRIN(6);
#endif
	SS_SET_N1PRIN(7);

		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)" ",0,180);	

//	initSprites(256+48-1,192+16-1,256-1,192-1,48,16);
	initSprites(256-1,192-1,0,0,0,0);
	 initScrolling(ON,(void *)SCL_VDP2_VRAM_B0+0x4000);
		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)" ",0,180);	

//	drawWindow(32,192,192,14,52);
#ifndef PONY
	nBurnFunction = update_input1;
#else
	frame_x	= 0;
	nBurnFunction = vbl;	
#endif

#ifdef GG
	drawWindow(48,144,144,12,76);
#else
	drawWindow(0,192,192,2,66);
#endif
	SetVblank2();
#ifndef OLD_SOUND
//	SPR_RunSlaveSH((PARA_RTN*)sh2slave, &nSoundBufferPos);
#endif

#ifdef PONY2
	pcm1 = add_raw_pcm_buffer(0,SOUNDRATE,nBurnSoundLen*20);
	nSoundBuffer = (Sint16 *)(SNDRAM+(m68k_com->pcmCtrl[pcm1].hiAddrBits<<16) | m68k_com->pcmCtrl[pcm1].loAddrBits);
#endif

}
//-------------------------------------------------------------------------------------------------------------------------------------
 void sms_start()
{
		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)" ",0,180);	
//	z80_stop_emulating();
	nSoundBufferPos=0;
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	UINT8 *ss_map = (UINT8 *)SS_MAP;
	
	for (int i=0;i<256 ; ++i ) colAddr[i]=colBgAddr[i]=RGB( 0, 0, 0 );
	*(UINT16 *)0x25E00000=RGB( 0, 0, 0 );
	
	memset(&ss_vram[0x1100],0,0x10000-0x1100);
	memset((Uint8 *)SS_CACHE,0x0,0x20000);
	memset((Uint8 *)SS_MAP,0,0x20000);
#ifdef TWO_WORDS
	for (int i=1;i<0x2000 ; i+=2 ) ss_map[i]=0x3000;//palette2[0];
#endif
	memset((Uint8 *)SCL_VDP2_VRAM_A0,0,0x20000);

	scroll_x= scroll_y = 0;
 	SCL_Open();
#ifdef GG
     for(int i = 0; i < 0xC0; i++) ss_scl[i]= -1<<16;
#else
    for(int i = 0; i < 0xC0; i++) ss_scl[i]= 0;
#endif

	ss_reg->n0_move_y =  scroll_y;
#ifdef GG   
	 ss_reg->n0_move_x = 1<<16;
#endif
	sms_reset();
	load_rom();
	system_init();

	running = 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------
 INT32 SMSInit(void)
{
#ifndef RAZE
#ifdef GG
	ChangeDir("GG");
#else
	ChangeDir("SMS");
#endif
#else
	ChangeDir(".");
#endif
	DrvInitSaturn();
	SN76489Init(0,MASTER_CLOCK, 0);
	sms_start();

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------
 void wait_vblank(void)
{
     while((TVSTAT & 8) == 0);
     while((TVSTAT & 8) == 8);
}
//-------------------------------------------------------------------------------------------------------------------------------------
 INT32 SMSExit(void)
{
	nBurnFunction = NULL;
	SS_SET_N0SPRM(0);
	SclProcess = 2;
	ss_regs->specialcode=0x0000;	
//	wait_vblank();
//	sms_reset();

//	if((*(Uint8 *)0xfffffe11 & 0x80) != 0x80)
//		SPR_WaitEndSlaveSH();

//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
//	SPR_InitSlaveSH();
	memset((void *)SOUND_BUFFER,0x00,0x4000*8);
//	SPR_InitSlaveSH();
#ifdef RAZE
	z80_stop_emulating();
	z80_add_read(0x0000, 0xFFFF, Z80_MAP_HANDLED, (void *)NULL);
	z80_add_write(0x0000, 0xFFFF, Z80_MAP_HANDLED, (void *)NULL);
	z80_add_write(0xFFFC, 0xFFFF, Z80_MAP_HANDLED, (void *)NULL);
	z80_set_in((unsigned char (*)(short unsigned int))NULL);
	z80_set_out((void (*)(short unsigned int, unsigned char))NULL);
#endif
#ifdef CZ80
	CZetExit2();
	CZ80Context = NULL;
#endif
//	SN76496Exit();
	vdp_reset();
/*
	sms256kbRom = NULL;
	cart.rom = NULL;
	__port = NULL;
//	vdp.vram = NULL;
	dummy_write = NULL;
	cram_lut = map_lut = NULL;

	bp_lut = NULL;
	name_lut = NULL;
*/
#ifdef GG0
//	disp_spr = NULL;
	curr_sprite=0;	
#endif

#ifdef GG
	memset(ss_scl,0x00,192*4);
//	vram_dirty =  NULL;
#endif

#ifdef PONY
remove_raw_pcm_buffer(pcm1);
#endif

	running = 0;
	first = 0;
	vsynch = 0;
	scroll_x=scroll_y=0;	
	file_max = 0;
	file_id = 0;	
	
	//cleanDATA();
	cleanBSS();

	nSoundBufferPos=0;
	
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
#ifdef PONY
void SMSFrame_old();

void SMSFrame()
{
	pcm1 = add_raw_pcm_buffer(0,SOUNDRATE,nBurnSoundLen*20);

	nSoundBuffer = (Sint16 *)(SNDRAM+(m68k_com->pcmCtrl[pcm1].hiAddrBits<<16) | m68k_com->pcmCtrl[pcm1].loAddrBits);

//	InitCD(); // si on lance juste pour pang
//	ChangeDir("PANG");  // si on lance juste pour pang
	pcm_stream_host(SMSFrame_old);
}

void SMSFrame_old()
#else
 void SMSFrame(void)
 #endif
{
#ifdef GG0
	cleanSpritesGG();
#endif
	if(running)
	{
	*(Uint16 *)0x25E00000 = colBgAddr[0]; // set bg_color
//	*(UINT16 *)0x25E00000=RGB( 0, 0, 0 );//palette2[0];
//#ifdef GG
#if 0
		update_cache();
#endif

		
#ifdef OLD_SOUND //
		sms_frame();

#ifndef PONY
	volatile signed short *nSoundBuffer = (signed short *)SOUND_BUFFER;
//	PSG_Update(&nSoundBuffer[nSoundBufferPos],  128);
	SN76496Update(0, &nSoundBuffer[nSoundBufferPos],  128);
	nSoundBufferPos+=128;
	if(nSoundBufferPos>=SAMPLE)//<<1)//256*hz)
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos);
		nSoundBufferPos=0;
	}
	PCM_Task(pcm);
#else
	signed short *nSoundBuffer2 = (signed short *)nSoundBuffer+(nSoundBufferPos<<1);

	SN76496Update(0, nSoundBuffer2, nBurnSoundLen);
	
	nSoundBufferPos+=nBurnSoundLen;
	
	if(nSoundBufferPos>=nBurnSoundLen*10)
	{
		pcm_play(pcm1, PCM_SEMI, 7);
		nSoundBufferPos=0;
	}
#endif	

 #else
		SPR_RunSlaveSH((PARA_RTN*)sh2slave, &nSoundBufferPos);
		sms_frame();
		
		if((*(unsigned char *)0xfffffe11 & 0x80) == 0)
			SPR_WaitEndSlaveSH();				

#endif
		ss_reg->n0_move_y =  scroll_y; //-(16<<16) ;
	}
	else
	{
#ifndef OLD_SOUND //
/*	wait_vblank();
	PCM_MeStop(pcm);

	unsigned int *nSoundBuffer = (unsigned int *)SOUND_BUFFER;
	memset((void *)nSoundBuffer,0x00,0x2000 *sizeof(INT16) * 8 * 8);
	nSoundBufferPos=0;

		PCM_MeStart(pcm);
		*/
			if((*(unsigned char *)0xfffffe11 & 0x80) == 0)
			SPR_WaitEndSlaveSH();
//			wait_vblank();
//						sh2slave(&nSoundBufferPos);
#endif	
		sms_start();
	}
#ifdef PONY
	frame_x++;
	
	 if(frame_x>=frame_y)
		wait_vblank();		
#endif
	
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/inline void sms_frame(void)
{
#if PROFILING
//		TIM_FRT_SET_16(0);
		line = bcountdiff = bcount = bcNum = 0;
#endif

	if(sms.paused && first==2)
	{
#ifdef RAZE
	z80_cause_NMI();
#else
	CZetNmi();
#endif
			first = 0;
	}
/*	
	vdp.left = vdp.reg[10];
    for(vdp.line = 0; vdp.line < 0xc0; vdp.line++)
	{
#ifdef RAZE
		z80_emulate(228);
#else
		CZetRun(228);
#endif		

		if 	(vdp.line < 0x10 && (vdp.reg[0] & 0x40))

			ls_tbl[vdp.line] = (-47<<16);
		else

			if (vdp.line < 0xC0)
				ls_tbl[vdp.line] = x-(47<<16);

		vdp_run(&vdp);
	}
*/	
	
//	vdp.line = 0;
	vdp.left = vdp.reg[10];

//    for(vdp.line = 0; vdp.line < 262; vdp.line++)
    for(vdp.line = 0; vdp.line <= 0xc0; ++vdp.line)
	{
#ifdef RAZE
		z80_emulate(228);
#else
		CZetRun(228);
#endif
		vdp_run(&vdp);	
#ifdef GG0
	render_obj(vdp.line);
#endif
	}

    for(; vdp.line < 0xE0; ++vdp.line)
	{
#ifdef RAZE
		z80_emulate(228);
#else
		CZetRun(228);
#endif
       vdp.left = vdp.reg[10];

        if((vdp.status & 0x80) && (vdp.reg[1] & 0x20))
        {
            sms.irq = 1;
#ifdef RAZE
			z80_raise_IRQ(0);
#else
//			CZetSetIRQLine(0, CZET_IRQSTATUS_ACK);
			CZetRaiseIrq(0);
#endif
        }
	}

    for(; vdp.line < 262; ++vdp.line)
	{
#ifdef RAZE
		z80_emulate(228);
#else
		CZetRun(228);
#endif
       vdp.left = vdp.reg[10];
/*		++vdp.line;
#ifdef RAZE
		z80_emulate(228);
#else
		CZetRun(228);
#endif
       vdp.left = vdp.reg[10];
	   */
	}
	
}
//-------------------------------------------------------------------------------------------------------------------------------------
 void load_rom(void)
{
	long fileSize = GetFileSize(file_id);//dir_name[file_id].dirrec.size;
	
	
	if(fileSize<=0x40000)
	cart.rom	 = (UINT8 *) sms256kbRom;
	else
	cart.rom	 = (UINT8 *) LOWADDR;
//	memset4_fast((Uint8 *)&cart.rom[0],0,0x100000);
	cart.pages	 = fileSize /0x4000;
	GFS_Load(file_id, 0, cart.rom, fileSize);
}
//-------------------------------------------------------------------------------------------------------------------------------------
 void system_init()
{
    /* Initialize the VDP emulation */
    vdp_reset();

    /* Initialize the SMS emulation */
    sms_init();
//	PSG_Init(MASTER_CLOCK, 7680);
//	SN76489AInit(0, MASTER_CLOCK, 0);
	SN76496Reset(0);
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void sms_init(void)
{
	z80_init();
	sms_reset();
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*
int sms_irq_callback(int param)
{
    return (0);
}		   */
//-------------------------------------------------------------------------------------------------------------------------------------
/* Reset VDP emulation */
 inline void vdp_reset(void)
{
    memset(&vdp, 0, sizeof(t_vdp));
}
//-------------------------------------------------------------------------------------------------------------------------------------
/* Write data to the VDP's control port */
 void vdp_ctrl_w(int data)
{
    /* Waiting for the reset of the command? */
    if(vdp.pending == 0)
    {
        /* Save data for later */
        vdp.latch = data;

        /* Set pending flag */
        vdp.pending = 1;
    }
    else
    {
        /* Clear pending flag */
        vdp.pending = 0;

        /* Extract code bits */
        vdp.code = (data >> 6) & 3;

        /* Make address */
        vdp.addr = (data << 8 | vdp.latch) & 0x3FFF;

        /* Read VRAM for code 0 */
        if(vdp.code == 0)
        {
            /* Load buffer with current VRAM byte */
            vdp.buffer = vdp.vram[vdp.addr & 0x3FFF];

            /* Bump address */
            vdp.addr = (vdp.addr + 1) & 0x3FFF;
        }

        /* VDP register write */
//        if((data & 0xF0) == 0x80)
        if(vdp.code == 2)
        {
            int r = (data & 0x0F);
            /* Store register data */
            vdp.reg[r] = vdp.latch;
			switch(r)
			{
				case 8 :
//					x =  ((vdp.reg[r]) ^ 0xff) & 0xff;
#ifdef GG
					scroll_x =  ((vdp.reg[r]) ^ 0xff);
#else
					scroll_x =  ((vdp.reg[r]) ^ 0xff) ;
#endif
					scroll_x<<=16;
					break;

				case 9 :
					scroll_y = (vdp.reg[r]&0xff)<<16;
					break;
				/* Update table addresses */
				case 2 :
					vdp.ntab = (vdp.reg[2] << 10) & 0x3800;
					break;

				case 5 :
					vdp.satb = (vdp.reg[5] << 7) & 0x3F00;
					break;
			}
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------
/* Read the status flags */
inline int vdp_ctrl_r(void)
{
    /* Save the status flags */
    UINT8 temp = vdp.status;

    /* Clear pending flag */
    vdp.pending = 0;
	vdp.status = 0;
#ifdef RAZE
	z80_lower_IRQ();
#else
	CZetLowerIrq();
#endif
    /* Return the old status flags */
    return (temp);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/inline  void update_bg(t_vdp *vdp, int index)
{
//				if(index>=vdp.ntab && index<vdp.ntab+0x700)
// VBT 04/02/2007 : modif compilo
	if(index>=vdp->ntab)
	{
		if( index<vdp->ntab+0x700)
		{
			UINT16 temp = *(UINT16 *)&vdp->vram[index&~1];
			unsigned int delta = map_lut[index - vdp->ntab];
			UINT16 *map = 	(UINT16 *)SS_MAP+delta;
#ifdef TWO_WORDS
			map[0] =map[64] =map[0xE00] =map[0xE40] = name_lut[temp];//color + flip + prio
			map[1] =map[65] =map[0xE01] =map[0xE41] = ((temp >> 8) & 0xFF) | ((temp  & 0x01) <<8) + 0x3000; //tilenum c00 1800
#else
			map[0] =map[32] =map[0x700] =map[0x720] =name_lut[temp];
#endif
		}
	}
#ifdef GG0
	else if(index>=vdp->satb)
	{
		if(index < vdp->satb+64)
		{
			disp_spr[index-vdp->satb]=0;
		}
		else if(index>=vdp->satb+128 && index < vdp->satb+256)
		{
			unsigned char pos = ((index-vdp->satb)>>1) & 0x3f;
			disp_spr[pos]=0;
		}
	}
	else
	{
#endif
		/* Mark patterns as dirty */
		UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
		UINT8 *ss_cache = (UINT8 *)SS_CACHE;
		
		UINT32 bp = *(UINT32 *)&vdp->vram[index & ~3];
		UINT32 *pg = (UINT32 *)&ss_cache[0x00000 | (index & ~3)];
		UINT32 *sg = (UINT32 *)&ss_vram[0x1100 + (index & ~3)];
		UINT32 temp1 = bp_lut[bp & 0xFFFF];
		UINT32 temp2 = bp_lut[(bp>>16) & 0xFFFF];
		*sg= *pg = (temp1<<2 | temp2 );
#ifdef GG0
	}
#endif
//        *pg = (temp1<<2 | temp2 );
}
//-------------------------------------------------------------------------------------------------------------------------------------
#ifdef GG
#ifdef GG0
void cleanSpritesGG()
{

	memset4_fast(disp_spr,0,64);
	curr_sprite = 0;
	ss_spritePtr = &ss_sprite[3];
	
	for (unsigned int delta=3; delta<nBurnSprites; delta++)
	{
//		ss_sprite[delta].charSize   = 0;
//		ss_sprite[delta].charAddr   = 0;
		ss_spritePtr->ax   = -16;
		ss_spritePtr->ay   = -16;
		ss_spritePtr++;
	} 
}
//-------------------------------------------------------------------------------------------------------------------------------------
/* Draw sprites */
inline void render_obj(INT32 line)
{
    /* Sprite count for current line (8 max.) */
    unsigned int count = 0;

    /* Sprite dimensions */
    unsigned int width = 8;
    unsigned int height = (vdp.reg[1] & 0x02) ? 16 : 8;

    /* Pointer to sprite attribute table */
    UINT8 *st = (UINT8 *)&vdp.vram[vdp.satb];

    /* Adjust dimensions for double size sprites */
    if(vdp.reg[1] & 0x01)
    {
        width  *= 2;
        height *= 2;
    }

	SprSpCmd *ss_spritePtr;
	ss_spritePtr = (SprSpCmd *)ss_sprite;

    /* Draw sprites in front-to-back order */
    for(UINT8 i = 0; i < 64; i ++)
    {
		/* Sprite Y position */
		int yp = st[i];

		/* Actual Y position is +1 */
		yp += 1;

		/* Wrap Y coordinate for sprites > 240 */
		if(yp > 240) yp -= 256;
 
		/* Check if sprite falls on current line */
		if((line >= yp) && (line < (yp + height)))
		{
			if(disp_spr[i]==0)
			{
				disp_spr[i]=1;

		//		int	delta=(index-vdp.satb);
				/* End of sprite list marker? */
				if(yp == 208)
				{
					ss_spritePtr[curr_sprite].control = CTRL_END;
					ss_spritePtr[curr_sprite].drawMode = 0;
					ss_spritePtr[curr_sprite].charAddr	= 0;
					ss_spritePtr[curr_sprite].charSize		= 0;
					ss_spritePtr[curr_sprite].ax	= 0;
					ss_spritePtr[curr_sprite].ay	= 0;
					return;			
				}

				/* Bump sprite count */
				count += 1;

				/* Too many sprites on this line ? */
				if(count == 9) return;

				/* Pattern name */
				int n = st[0x81 + (i << 1)];

				/* Sprite X position */
				int xp = st[0x80 + (i << 1)];

				/* X position shift */
				if(vdp.reg[0] & 0x08) xp -= 8;

				/* Add MSB of pattern name */
				if(vdp.reg[6] & 0x04) n |= 0x0100;

				/* Mask LSB for 8x16 sprites */
				if(vdp.reg[1] & 0x02) n &= 0x01FE;

				/* Draw sprite */
				ss_spritePtr[curr_sprite].control	      = ( JUMP_NEXT | FUNC_NORMALSP);
				ss_spritePtr[curr_sprite].drawMode = ( COLOR_0 | ECD_DISABLE | COMPO_REP);		
				ss_spritePtr[curr_sprite].charSize    = (width<<5) + height;  //0x100
				ss_spritePtr[curr_sprite].charAddr   =  0x220+(n<<2);//0x100 + (height<<1)*(i+2);
				ss_spritePtr[curr_sprite].ax			  = xp;
				ss_spritePtr[curr_sprite].ay			  = yp;
				curr_sprite++;
			}
		}
    }
}
#endif
/* Update pattern cache with modified tiles */
#if 0
void update_cache(void)
{
    int i, x, y, c;
    int b0, b1, b2, b3;
    int i0, i1, i2, i3;

    if(!is_vram_dirty) return;
    is_vram_dirty = 0;

	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;

    for(i = 0; i < 0x200; i += 1)
    {
        if(vram_dirty[i])
        {
            vram_dirty[i] = 0;

            for(y = 0; y < 8; y += 1)
            {
                b0 = vdp.vram[(i << 5) | (y << 2) | (0)];
                b1 = vdp.vram[(i << 5) | (y << 2) | (1)];
                b2 = vdp.vram[(i << 5) | (y << 2) | (2)];
                b3 = vdp.vram[(i << 5) | (y << 2) | (3)];

                for(x = 0; x < 8; x += 1)
                {
                    i0 = (b0 >> (x ^ 7)) & 1;
                    i1 = (b1 >> (x ^ 7)) & 1;
                    i2 = (b2 >> (x ^ 7)) & 1;
                    i3 = (b3 >> (x ^ 7)) & 1;

                    c = (i3 << 3 | i2 << 2 | i1 << 1 | i0);

                    cache[0x00000 | (i << 6) | ((y  ) << 3) | (x)] = c;
//                    cache[0x08000 | (i << 6) | ((y  ) << 3) | (x ^ 7)] = c;
//                    cache[0x10000 | (i << 6) | ((y ^ 7) << 3) | (x)] = c;
//                    cache[0x18000 | (i << 6) | ((y ^ 7) << 3) | (x ^ 7)] = c;
                    ss_vram[0x01100 | (i << 6) | ((y  ) << 3) | (x)] = c;
                }
            }
        }
    }
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
inline void vdp_data_w(INT32 offset, UINT8 data)
{
    INT32 index;

    switch(offset & 1)
    {
        case 0: /* Data port */
            vdp.pending = 0;
            switch(vdp.code)
            {
                case 0: /* VRAM write */
                case 1: /* VRAM write */
                case 2: /* VRAM write */
                    index = (vdp.addr & 0x3FFF);
                    if(data != vdp.vram[index])
                    {
                        vdp.vram[index] = data;
						update_bg(&vdp,index);
                    }

 			if(index>=vdp.satb )
				if( index < vdp.satb+0x40)
			{


				// Sprite dimensions 
				unsigned int height = (vdp.reg[1] & 0x02) ? 16 : 8;
				unsigned int width  = 8;
				unsigned int delta=(index-vdp.satb);
			// Pointer to sprite attribute table 
				UINT8 *st = (UINT8 *)&vdp.vram[vdp.satb];
				// Sprite Y position 
				int yp = st[delta];
				SprSpCmd *ss_spritePtr;
				ss_spritePtr = &ss_sprite[3+delta];

				if(yp == 208) 
				{
					
					ss_spritePtr->control = CTRL_END;
					ss_spritePtr->drawMode = 0;
					ss_spritePtr->charAddr	= 0;
					ss_spritePtr->charSize		= 0;
					ss_spritePtr->ax	= 0;
					ss_spritePtr->ay	= 0;
//					nbSprites = delta+5;
//ajouter un flag
					break;
				}
				//Actual Y position is +1 
				yp ++;
				//Wrap Y coordinate for sprites > 240 
				if(yp > 240) yp -= 256;
				ss_spritePtr->ay = yp;

				/* Adjust dimensions for double size sprites */
				if(vdp.reg[1] & 0x01)
				{
					width  = 16;
					height *= 2;
				}

				// Clip sprites on left edge 
				ss_spritePtr->control = ( JUMP_NEXT | FUNC_NORMALSP);
				ss_spritePtr->drawMode   = ( COLOR_0 | ECD_DISABLE | COMPO_REP);		
				ss_spritePtr->charSize    = (width<<5) | height;  //0x100
			}

// VBT 04/02/2007 : modif compilo
//-----------------------------------------------------------------------------------------------
			if(index>=vdp.satb+0x80)
				if(index < vdp.satb+0x100)
			{
				UINT8 *st = (UINT8 *)&vdp.vram[vdp.satb];
				unsigned int delta=((index-(vdp.satb+0x80)))>>1;

				if((index-vdp.satb) &1)
				{
					int n = st[0x81 + (delta << 1)];
					//Add MSB of pattern name 
					if(vdp.reg[6] & 0x04) n |= 0x0100;
					//Mask LSB for 8x16 sprites 
					if(vdp.reg[1] & 0x02) n &= 0x01FE;

//					ss_sprite[delta+3].charAddr   =  0x110+(n<<2);
					ss_sprite[delta+3].charAddr   =  0x220+(n<<2);
				}
				else
				{
						//Sprite X position 
					int xp = st[0x80 + (delta << 1)];
					//X position shift 
					if(vdp.reg[0] & 0x08) xp -= 8;
					ss_sprite[delta+3].ax = xp;
				}
	
			}
//-----------------------------------------------------------------------------------------------
                    break;

                case 3: /* CRAM write */
                    if(vdp.addr & 1)
                    {
                        vdp.cram_latch = (vdp.cram_latch & 0x00FF) | ((data & 0xFF) << 8);
	                    vdp.cram[(vdp.addr & 0x3E) | (0)] = (vdp.cram_latch >> 0) & 0xFF;
                        vdp.cram[(vdp.addr & 0x3E) | (1)] = (vdp.cram_latch >> 8) & 0xFF;

						index = (vdp.addr >> 1) & 0x1F;
						colBgAddr[index] = cram_lut[vdp.cram_latch & 0x0fff];//RGB(r<<1,g<<1,b<<1);
						if(index >0x0f)
							colAddr[index&0x0f] =  colBgAddr[index];
                    }
                    else
                    {
                        vdp.cram_latch = (vdp.cram_latch & 0xFF00) | ((data & 0xFF) << 0);
                    }
                    break;
            }
            vdp.addr = (vdp.addr + 1) & 0x3FFF;
            return;

        case 1: /* Control port */
            if(vdp.pending == 0)
            {
                vdp.addr = (vdp.addr & 0x3F00) | (data & 0xFF);
                vdp.latch = data;
                vdp.pending = 1;
            }
            else
            {
                vdp.pending = 0;
                vdp.code = (data >> 6) & 3;
                vdp.addr = (data << 8 | vdp.latch) & 0x3FFF;

                if(vdp.code == 0)
                {
                    vdp.buffer = vdp.vram[vdp.addr & 0x3FFF];
                    vdp.addr = (vdp.addr + 1) & 0x3FFF;
                }

                if(vdp.code == 2)
                {
                    INT32 r = (data & 0x0F);
					/* Store register data */
					vdp.reg[r] = vdp.latch;
					switch(r)
					{
						case 8 :
		//					x =  ((vdp.reg[r]) ^ 0xff) & 0xff;
							scroll_x =  ((vdp.reg[r]) ^ 0xff) <<16;
//							scroll_x<<=16;
							break;

						case 9 :
							scroll_y = (vdp.reg[r]&0xff)<<16;
							break;
						/* Update table addresses */
						case 2 :
							vdp.ntab = (vdp.reg[2] << 10) & 0x3800;
							break;

						case 5 :
							vdp.satb = (vdp.reg[5] << 7) & 0x3F00;
							break;
					}
                }
            }
            return;
    }
}
#else
//-------------------------------------------------------------------------------------------------------------------------------------
/* Write data to the VDP's data port */
 void vdp_data_w(INT32 offset, UINT8 data)
{
    int index;
    int delta;

    /* Clear the pending flag */
    vdp.pending = 0;

    switch(vdp.code)
    {
        case 0: /* VRAM write */
        case 1: /* VRAM write */
        case 2: /* VRAM write */

            /* Get current address in VRAM */
            index = (vdp.addr & 0x3FFF);

            /* Only update if data is new */
            if(data != vdp.vram[index])
            {
				/* Store VRAM byte */
                vdp.vram[index] = data;
				update_bg(&vdp,index);
            }

//VBT : A REMETTRE A LA PLACE  de rederSprite des que le probleme sur yp=208 est résolu
// VBT04/02/2007 modif compilo
 			if(index>=vdp.satb )
				if( index < vdp.satb+0x40)
			{
				SprSpCmd *ss_spritePtr;


				// Sprite dimensions 
				unsigned int height = (vdp.reg[1] & 0x02) ? 16 : 8;

				delta=(index-vdp.satb);
			// Pointer to sprite attribute table 
				UINT8 *st = (UINT8 *)&vdp.vram[vdp.satb];
				// Sprite Y position 
				int yp = st[delta];
				ss_spritePtr = &ss_sprite[3+delta];
				
				if(yp == 208) 
				{
					
					ss_spritePtr->control = CTRL_END;
					ss_spritePtr->drawMode = 0;
					ss_spritePtr->charAddr	= 0;
					ss_spritePtr->charSize		= 0;
					ss_spritePtr->ax	= 0;
					ss_spritePtr->ay	= 0;
//					nbSprites = delta+5;
//ajouter un flag
					break;
				}
				//Actual Y position is +1 
				yp ++;
				//Wrap Y coordinate for sprites > 240 
				if(yp > 240) yp -= 256;
				ss_spritePtr->ay = yp;

				// Clip sprites on left edge 
				ss_spritePtr->control = ( JUMP_NEXT | FUNC_NORMALSP);
				ss_spritePtr->drawMode   = ( COLOR_0 | ECD_DISABLE | COMPO_REP);		
				ss_spritePtr->charSize   = 0x100| height;  //0x100
				break;
			}

// VBT 04/02/2007 : modif compilo

			if(index>=vdp.satb+0x80)
				if(index < vdp.satb+0x100)
			{
				UINT8 *st = (UINT8 *)&vdp.vram[vdp.satb];
				delta=((index-(vdp.satb+0x80)))>>1;

				if((index-vdp.satb) &1)
				{
					int n = st[0x81 + (delta << 1)];
					//Add MSB of pattern name 
					if(vdp.reg[6] & 0x04) n |= 0x0100;
					//Mask LSB for 8x16 sprites 
					if(vdp.reg[1] & 0x02) n &= 0x01FE;

//					ss_sprite[delta+3].charAddr   =  0x110+(n<<2);
					ss_sprite[delta+3].charAddr   =  0x220+(n<<2);
					break;					
				}
				else
				{
						//Sprite X position 
					int xp = st[0x80 + (delta << 1)];
					//X position shift 
					if(vdp.reg[0] & 0x08) xp -= 8;
					ss_sprite[delta+3].ax = xp;
				}

			}

            break;

        case 3: /* CRAM write */
			index = (vdp.addr & 0x1F);
			if(data != vdp.cram[index])
			{
				vdp.cram[index] = data;
				colBgAddr[index] = cram_lut[data & 0x3F];

				  if (index>0x0f)
					colAddr[index&0x0f] =  colBgAddr[index];
            }
            break;
    }
    /* Bump the VRAM address */
//     vdp.addr ++;
		vdp.addr =(vdp.addr + 1) & 0x3FFF;
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
/* Read data from the VDP's data port */
 int vdp_data_r(t_vdp *vdp)
{
 //   UINT8 temp = 0;
    vdp->pending = 0;
    UINT8 temp = vdp->buffer;
//    vdp.buffer = vdp.vram[(vdp.addr & 0x3FFF)^1];
   vdp->buffer = vdp->vram[(vdp->addr & 0x3FFF)];
    vdp->addr = (vdp->addr + 1) & 0x3FFF;
    return temp;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/* Process frame events */
//void vdp_run(INT32 line)
/*
		if 	(vdp.line < 0x10 && (vdp.reg[0] & 0x40))
			ss_scl[vdp.line] = 0;
		else
			if (vdp.line < 0xC0)
				ss_scl[vdp.line] = scroll_x;
*/


 void vdp_run(t_vdp *vdp)
{
#ifdef GG
    if(vdp->line <= 0xC0)
    {
        if(vdp->line == 0xC0)
        {
            vdp->status |= 0x80;
        }

        if(vdp->line == 0)
        {
            vdp->left = vdp->reg[10];
        }
		else
		{
			ss_scl[vdp->line-1] = scroll_x;
		}

        if(vdp->left == 0)
        {
            vdp->left = vdp->reg[10];
            vdp->status |= 0x40;
        }
        else
        {
            vdp->left -= 1;
        }
        if((vdp->status & 0x40) && (vdp->reg[0] & 0x10))
        {
            sms.irq = 1;
#ifdef RAZE
			z80_raise_IRQ(0);
#else
//			CZetSetIRQLine(0, CZET_IRQSTATUS_ACK);
			CZetRaiseIrq(0);
#endif
		}
    }
    else
    {
        vdp->left = vdp->reg[10];

        if((vdp->line < 0xE0) && (vdp->status & 0x80) && (vdp->reg[1] & 0x20))
        {
            sms.irq = 1;
#ifdef RAZE
			z80_raise_IRQ(0);
#else
//			CZetSetIRQLine(0, CZET_IRQSTATUS_ACK);
			CZetRaiseIrq(0);
#endif
		}
    }
#else
    if(vdp->line <= 0xC0)
    {
		if 	(vdp->line < 0x10 && (vdp->reg[0] & 0x40))
			ss_scl[vdp->line] = 0;
		else
			ss_scl[vdp->line] = scroll_x;

        if(vdp->line == 0xC0)
        {
           memcpyl(SS_SPRAM,ss_sprite,(nBurnSprites<<5) ) ;
            vdp->status |= 0x80;
        }

		if(vdp->left == 0)
        {
            vdp->left = vdp->reg[10];
            vdp->status |= 0x40;
        }
        else
        {
            vdp->left -= 1;
        }

        if((vdp->status & 0x40) && (vdp->reg[0] & 0x10))
        {
            sms.irq = 1;
#ifdef RAZE
			z80_raise_IRQ(0);
#else
//			CZetSetIRQLine(0, CZET_IRQSTATUS_ACK);
			CZetRaiseIrq(0);
#endif
        }
    }
#endif
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/inline  UINT8 vdp_vcounter_r(void)
{
    return (vcnt[(vdp.line & 0x1FF)]);
}
//-------------------------------------------------------------------------------------------------------------------------------------
 UINT8 vdp_hcounter_r(void)
{
//    int pixel = (((z80_ICount % CYCLES_PER_LINE) / 4) * 3) * 2;
//  int pixel = (((Cz80_struc.CycleIO % CYCLES_PER_LINE) / 4) * 3) * 2;
  int pixel;
#ifdef RAZE
  if (hz==60)
	pixel = (((z80_get_cycles_elapsed() % CYCLES_PER_LINE_60) >>2) * 3) <<1;
  else
	pixel = (((z80_get_cycles_elapsed() % CYCLES_PER_LINE_50) >>2) * 3) <<1;
#else
  if (hz==60)
	pixel = (((CZetTotalCycles() % CYCLES_PER_LINE_60) >>2) * 3) <<1;
  else
	pixel = (((CZetTotalCycles() % CYCLES_PER_LINE_50) >>2) * 3) <<1;
#endif
//	printf("Cz80_struc.CycleIO %d\n pixel %d\n",Cz80_struc.CycleIO,pixel);
    return (hcnt[((pixel >> 1) & 0x1FF)]);
}
//-------------------------------------------------------------------------------------------------------------------------------------
 unsigned char cz80_z80_readport16(unsigned short PortNo)
{
#ifdef GG
	UINT8 temp = 0xFF;
	SysDevice	*device;
#endif

    switch(PortNo & 0xFF)
    {
#ifdef GG
        case 0x01: /* GG SIO */
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
            return (0x00);
#endif
        case 0x7E: /* V COUNTER */
            return (vdp_vcounter_r());
            break;
    
        case 0x7F: /* H COUNTER */
		//	printf("vdp_hcounter_r\n");
            return (vdp_hcounter_r());
            break;
    
        case 0x00: /* INPUT #2 */
//
#ifdef GG
			if(( device = PER_GetDeviceR( &__port[0], 0 )) != NULL )
			{
				pltrigger[0]  = PER_GetTrigger( device );
				if((pltrigger[0] & PER_DGT_C)!=0)  temp &= ~0x80;	
			}
			return temp;
#else
 			return 0xff;
#endif

		case 0xC0: /* INPUT #0 */  
		case 0xDC:
			return (update_input1());
//			return 0xff;
		case 0xC1: /* INPUT #1 */
		case 0xDD:
//			return 0xff;
			return update_input2();

        case 0xBE: /* VDP DATA */
//			printf ("read port vdp_data_r\n");
            return (vdp_data_r(&vdp));
    
        case 0xBD:
        case 0xBF: /* VDP CTRL */
//			printf ("read port vdp_ctrl_r\n");
            return (vdp_ctrl_r());

        case 0xF2: /* YM2413 DETECT */
//            return (sms.port_F2);
            break;
    }
//    return (0xFF);      
    return (0);      
}
//-------------------------------------------------------------------------------------------------------------------------------------
 void cz80_z80_writeport16(unsigned short PortNo, unsigned char data)
{
    switch(PortNo & 0xFF)
    {
#ifdef GG
        case 0x01: /* GG SIO */
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
             break;
        case 0x06: /* GG STEREO */
            sms.psg_mask = (data & 0xFF);
            break;
#endif
        case 0x7E: /* SN76489 PSG */
        case 0x7F:
//#ifdef SOUND
//			if (sound)
				SN76496Write(0,data);
//				PSG_Write(0,data);
//				PSG_Write(data);
//#endif
           break;

        case 0xBE: /* VDP DATA */
            vdp_data_w(PortNo, data);
            break;

        case 0xBD: /* VDP CTRL */ 
        case 0xBF:
//			printf ("write vdp_ctrl_w");
            vdp_ctrl_w(data);
            break;

        case 0xF0: /* YM2413 */
        case 0xF1:
//            ym2413_write(0, PortNo & 1, data);
            break;

        case 0xF2: /* YM2413 DETECT */
//            sms.port_F2 = (data & 1);
            break;



        case 0x3F: /* TERRITORY CTRL. */
             sms.port_3F = ((data & 0x80) | (data & 0x20) << 1) & 0xC0;
            /*if(sms.country == TYPE_DOMESTIC)*/ //sms.port_3F ^= 0xC0;
            break;
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------
 UINT8 update_input1(void)
{
	unsigned int i=0,k;
	UINT8 temp = 0xFF;
	SysDevice	*device;
//	__port = PER_OpenPort();
//PER_GetPort(__port);	
	if(( device = PER_GetDeviceR( &__port[0], 0 )) != NULL )
	{
		pltriggerE[0] = pltrigger[0];
		pltrigger[0] = PER_GetTrigger( device );
		pltriggerE[0] = (pltrigger[0]) ^ (pltriggerE[0]);
		pltriggerE[0] = (pltrigger[0]) & (pltriggerE[0]);

		for(i=0;i<6;i++)
		{
			if((pltrigger[0] & pad_asign[i])!=0)
			{
				switch(pltrigger[0] & pad_asign[i] )
				{
					case PER_DGT_U: temp &= ~0x01; break;
					case PER_DGT_D: temp &= ~0x02; break;
					case PER_DGT_L: temp &= ~0x04; break;
					case PER_DGT_R: temp &= ~0x08; break;
					case PER_DGT_A: temp &= ~0x10; break;
					case PER_DGT_B: temp &= ~0x20; break;
					default: 	break;
				}
			}
		}

		for(;i<12;i++)
		{
			if((pltriggerE[0] & pad_asign[i])!=0)
			{
				switch(pltriggerE[0] & pad_asign[i] )
				{
#ifndef GG
					case PER_DGT_C:
						if (!sms.paused)	{ sms.paused = 1; first = 2;}
						else{ sms.paused = 0;}
						temp = 0xFF;
					break;
#endif
					case PER_DGT_X:

#ifdef PROFILING
	for (k=0;k<21 ;k++)
	{
		char toto2[50];
		sprintf(toto2,"%08d", bcount2[k]);
		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)toto2,0,k*10);
	}
#endif
					break;

					case PER_DGT_Y:
						if(vsynch==1)vsynch=0;
						else				vsynch=1;
					break;

#ifndef ACTION_REPLAY
					case PER_DGT_S: running=0; break;
					case PER_DGT_TR:
					if (file_id<=file_max)	file_id++;
					else							file_id=2;
//#ifdef FONT
						FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,
						(Uint8 *)"            ",26,200);
						FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,
						(Uint8 *)GFS_IdToName(file_id),26,200);					
//#endif
					break;

//					 char str[50];

					case PER_DGT_TL:
					if (file_id>2)	file_id--;
					else				file_id=file_max;
//#ifdef FONT
						FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,
						(Uint8 *)"            ",26,200);
						FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,
						(Uint8 *)GFS_IdToName(file_id),26,200);
//#endif
#endif
				    default:
					break;
				}
			}
		}
	}
	else	pltrigger[0] = pltriggerE[0] = 0;

	if(( device = PER_GetDeviceR( &__port[1], 0 )) != NULL )
	{
//		pltriggerE[1] = pltrigger[1];
		pltrigger[1]  = PER_GetTrigger( device );
//		pltriggerE[1] = (pltrigger[1]) ^ (pltriggerE[1]);
//		pltriggerE[1] = (pltrigger[1]) & (pltriggerE[1]);

		if((pltrigger[1] & PER_DGT_U)!=0)  temp &= ~0x40;
		if((pltrigger[1] & PER_DGT_D)!=0)  temp &= ~0x80;	
	}
	return temp;
}
//-------------------------------------------------------------------------------------------------------------------------------------
 UINT8 update_input2(void)
{
	unsigned int i=0;
	UINT8 temp = 0xFF;
	SysDevice	*device;

	if(( device = PER_GetDeviceR( &__port[1], 0 )) != NULL )
	{
	//	pltriggerE[1] = pltrigger[1];
		pltrigger[1]  = PER_GetTrigger( device );
	//	pltriggerE[1] = (pltrigger[1]) ^ (pltriggerE[1]);
	//	pltriggerE[1] = (pltrigger[1]) & (pltriggerE[1]);

		for(i=0;i<7;i++)
		{
			if((pltrigger[1] & pad_asign[i])!=0)
			{
				switch(pltrigger[1] & pad_asign[i] )
				{
					case PER_DGT_L: temp &= ~0x01; break;
					case PER_DGT_R: temp &= ~0x02; break;
					case PER_DGT_A: temp &= ~0x04; break;
					case PER_DGT_B: temp &= ~0x08; break;
					default: 	break;
				}
			}
		}
	}
	else pltrigger[1] = 0; //pltriggerE[1] = 0;
	return ((temp & 0x3F) | (sms.port_3F & 0xC0));
}
//-------------------------------------------------------------------------------------------------------------------------------------
 void sms_reset(void)
{
#ifdef RAZE
	z80_reset();
#else
	CZetReset();
#endif
    /* Clear SMS context */
    memset(dummy_write, 0, 0x100);
    memset(sms.ram, 0, 0x2000);
    memset(sms.sram, 0, 0x8000);
	memset(vdp.vram,0, 0x4000);
#ifdef GG0
	memset(disp_spr,0,64);
#endif
    sms.port_3F = sms.port_F2 = sms.irq = 0x00;
//    sms.psg_mask = 0xFF;
#if 0
    is_vram_dirty = 1;
    memset(vram_dirty, 1, 0x200);
	sms.ram[0] = 0xA8;
#endif
    /* Load memory maps with default values */
#ifdef RAZE
	z80_map_read (0x0000, 0x3FFF, cart.rom); 

	z80_map_read (0x4000, 0x7FFF, &cart.rom[0x4000]); 
	z80_map_read (0x8000, 0xBFFF, &cart.rom[0x8000]); 
	z80_map_write(0x0000, 0xBFFF, (unsigned char*)dummy_write);

	z80_map_read (0xC000, 0xDFFF, (unsigned char *)sms.ram); 
	z80_map_write (0xC000, 0xDFFF, (unsigned char *)sms.ram); 

	z80_map_read (0xE000, 0xFFFF, (unsigned char *)sms.ram); 
	z80_map_write (0xE000, 0xFFFF, (unsigned char *)sms.ram); 

#define Z80_MAP_HANDLED 1
//	z80_add_write(0xFFFC, 0xFFFF, Z80_MAP_HANDLED, (void *)&cpu_writemem8);
	z80_add_write(0xFFFC, 0xFFFF, Z80_MAP_HANDLED, (void *)&cpu_writemem8);
#else
//	CZetMapArea(0x0000, 0x3FFF, 0, cart.rom); 
	CZetMapMemory((unsigned char *)cart.rom, 0x0000, 0x3FFF, MAP_READ);
//	CZetMapArea(0x0000, 0x3FFF, 2, &cart.rom[0]); 
//	CZetMapArea (0x4000, 0x7FFF, 0, &cart.rom[0x4000]); 
	CZetMapMemory((unsigned char *)&cart.rom[0x4000], 0x4000, 0x7FFF, MAP_READ);	
//	CZetMapArea (0x4000, 0x7FFF, 2, &cart.rom[0x4000]); 
//	CZetMapArea (0x8000, 0xBFFF, 0, &cart.rom[0x8000]); 
	CZetMapMemory((unsigned char *)&cart.rom[0x8000], 0x8000, 0xBFFF, MAP_READ);		
//	CZetMapArea (0x8000, 0xBFFF, 2, &cart.rom[0x8000]); 
//	CZetMapArea (0x0000, 0xBFFF , 1, (unsigned char*)dummy_write);
	CZetMapMemory((unsigned char *)dummy_write, 0x0000, 0xBFFF, MAP_WRITE);	
//	CZetMapArea (0x0000, 0xBFFF , 2, (unsigned char*)dummy_write);

//	CZetMapArea (0xC000, 0xDFFF, 0, (unsigned char *)sms.ram); 
//	CZetMapArea (0xC000, 0xDFFF, 1, (unsigned char *)sms.ram); 
//	CZetMapArea (0xC000, 0xDFFF, 2, (unsigned char *)sms.ram); 
	CZetMapMemory((unsigned char *)sms.ram, 0xC000, 0xDFFF, MAP_RAM);

//	CZetMapArea (0xE000, 0xFFFF, 0, (unsigned char *)sms.ram); 
	CZetMapMemory((unsigned char *)sms.ram, 0xE000, 0xFFFF, MAP_READ);
//	CZetMapArea (0xE000, 0xFFFF, 1, (unsigned char *)sms.ram); 
//	CZetMapArea (0xE000, 0xFFFF, 2, (unsigned char *)sms.ram); 

//	CZetSetWriteHandler(cpu_writemem8);
	CZetSetWriteHandler2(0xFFFC, 0xFFFF,cpu_writemem8);

	CZetSetSP(0xdff0);
#endif

	sms.fcr[0] = 0x00;
    sms.fcr[1] = 0x00;
    sms.fcr[2] = 0x01;
    sms.fcr[3] = 0x00;
	
//	SN76496Reset();
	nSoundBufferPos=0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
 void cpu_writemem8(unsigned int address, unsigned int data)
{
	sms.ram[address & 0x1FFF] = data;
#ifdef CZ80x
	if(address >= 0xFFFC)
	{
#endif
// data & cart.pages, and set cart.pages to one less than you are
	UINT32 offset; // = (data % cart.pages) << 14; // VBT à corriger
// vbt 15/05/2008 : exophase :	 data & cart.pages, and set cart.pages to one less than you are
	sms.fcr[address& 3] = data;

	switch(address & 3)
	{
		case 0:

			if(data & 8)
			{
				offset = (data & 0x4) ? 0x4000 : 0x0000;
#ifdef RAZE
				z80_map_fetch(0x8000, 0xBFFF, (unsigned char *)(sms.sram + offset));
				z80_map_read(0x8000, 0xBFFF, (unsigned char *)(sms.sram + offset));
				z80_map_write(0x8000, 0xBFFF, (unsigned char *)(sms.sram + offset));
#else
				CZetMapMemory((unsigned char *)(sms.sram + offset), 0x8000, 0xBFFF, MAP_RAM); // ok

#endif
			}
			else
			{
				offset = ((sms.fcr[3] % cart.pages) << 14);
// vbt 15/05/2008 : exophase :	 data & cart.pages, and set cart.pages to one less than you are
#ifdef RAZE
				z80_map_fetch(0x8000, 0xBFFF, (unsigned char *)(cart.rom + offset));
				z80_map_read(0x8000, 0xBFFF, (unsigned char *)(cart.rom + offset));
				z80_map_write(0x8000, 0xBFFF, (unsigned char *)(dummy_write));
#else
				CZetMapMemory((unsigned char *)(cart.rom + offset), 0x8000, 0xBFFF, MAP_ROM);
//				CZetMapArea(0x8000, 0xBFFF, 0, (unsigned char *)(cart.rom + offset));
				CZetMapMemory((unsigned char *)(dummy_write), 0x8000, 0xBFFF, MAP_WRITE);
//				CZetMapArea(0x8000, 0xBFFF, 1, (unsigned char *)(dummy_write));
//				CZetMapArea(0x8000, 0xBFFF, 2, (unsigned char *)(cart.rom + offset));
#endif
			}
			break;
		case 1:
			offset = (data % cart.pages) << 14;
#ifdef RAZE
			z80_map_fetch(0x0000, 0x3FFF, (unsigned char *)(cart.rom + offset));
			z80_map_read(0x0000, 0x3FFF, (unsigned char *)(cart.rom + offset));
#else
			CZetMapMemory((unsigned char *)(cart.rom + offset), 0x0000, 0x3FFF, MAP_ROM);
#endif
		break;

		case 2:
			offset = (data % cart.pages) << 14;
#ifdef RAZE
			z80_map_fetch(0x4000, 0x7FFF, (unsigned char *)(cart.rom + offset));
			z80_map_read(0x4000, 0x7FFF, (unsigned char *)(cart.rom + offset));
#else
			CZetMapMemory((unsigned char *)(cart.rom + offset), 0x4000, 0x7FFF, MAP_ROM);
#endif
		break;

		case 3:

		if(!(sms.fcr[0] & 0x08))
		{
			offset = (data % cart.pages) << 14;
#ifdef RAZE
			z80_map_fetch(0x8000, 0xBFFF, (unsigned char *)(cart.rom + offset));
			z80_map_read(0x8000, 0xBFFF, (unsigned char *)(cart.rom + offset));
#else
			CZetMapMemory((unsigned char *)(cart.rom + offset), 0x8000, 0xBFFF, MAP_ROM);			
#endif
		}
		break;
	}
#ifdef CZ80x
	}
#endif
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*#ifdef CZ80
static void cpu_writemem16(unsigned int address, unsigned int data)
{
	cpu_writemem8(address, data & 0xFF);
	cpu_writemem8(address + 1, data >> 8);
}
#endif	   */
//-------------------------------------------------------------------------------------------------------------------------------------
inline void z80_init(void)
{
#ifdef RAZE
	z80_init_memmap();
//	z80_map_fetch(0x0000, 0xBFFF, (unsigned char *)&cart.rom);
//	z80_map_fetch(0xC000, 0xFFFF, (unsigned char *)&sms.ram);
#define Z80_MAP_HANDLED 1
#define Z80_MAP_DIRECT  0  /* Reads/writes are done directly */

/* Bank #0 */ 
z80_map_fetch (0x0000, 0x3FFF, (unsigned char *)cart.rom); 
z80_map_read (0x0000, 0x3FFF, (unsigned char *)cart.rom); 

/* Bank #1 */ 
z80_map_fetch (0x4000, 0x7FFF, (unsigned char *)(cart.rom+0x4000)); 
z80_map_read (0x4000, 0x7FFF, (unsigned char *)(cart.rom+0x4000)); 

/* Bank #2 */ 
z80_map_fetch (0x8000, 0xBFFF, (unsigned char *)(cart.rom+0x8000)); 
z80_map_read (0x8000, 0xBFFF, (unsigned char *)(cart.rom+0x8000)); 

/* RAM */ 
z80_map_fetch (0xC000, 0xDFFF, (unsigned char *)sms.ram); 
z80_map_read (0xC000, 0xDFFF, (unsigned char *)sms.ram); 
z80_map_write (0xC000, 0xDFFF, (unsigned char *)sms.ram); 

/* RAM (mirror) */ 
z80_map_fetch (0xE000, 0xFFFF, (unsigned char *)sms.ram); 
z80_map_read (0xE000, 0xFFFF, (unsigned char *)sms.ram); 
z80_map_write (0xE000, 0xFFFF, (unsigned char *)sms.ram); 

//z80_add_write(0xC000, 0xFFFF, Z80_MAP_DIRECT, (void *)sms.ram);
//z80_add_write(0xFFFC, 0xFFFF, Z80_MAP_HANDLED, (void *)&cpu_writemem8);
z80_add_write(0x0000, 0xFFFF, Z80_MAP_HANDLED, (void *)&cpu_writemem8);

	z80_end_memmap();     

	z80_set_in((unsigned char (*)(short unsigned int))&cz80_z80_readport16);
	z80_set_out((void (*)(short unsigned int, unsigned char))&cz80_z80_writeport16);
//	z80_set_fetch_callback(&debug);

	z80_reset();
#else
	CZetInit2(1,CZ80Context);
	CZetOpen(0);
/* Bank #0 */ 
	CZetMapMemory((unsigned char *)cart.rom, 0x0000, 0x3FFF, MAP_ROM);
				
/* Bank #1 */ 
	CZetMapMemory((unsigned char *)cart.rom+0x4000, 0x4000, 0x7FFF, MAP_ROM);

/* Bank #2 */ 
	CZetMapMemory((unsigned char *)cart.rom+0x8000, 0x8000, 0xBFFF, MAP_ROM);
	
/* RAM */
	CZetMapMemory((unsigned char *)cart.rom, 0xC000, 0xDFFF, MAP_RAM);
	
/* RAM (mirror) */ 
	CZetMapMemory((unsigned char *)cart.rom, 0xE000, 0xFFFF, MAP_ROM);
	
	CZetSetWriteHandler(cpu_writemem8);
	CZetSetInHandler(cz80_z80_readport16);
	CZetSetOutHandler(cz80_z80_writeport16);

	CZetReset();
#endif
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline  void make_map_lut()
{
	unsigned int row,column;

	for (int i = 0; i < 0x800;i++) 
	{
		row = i & 0x7C0;
		column = (i>>1) & 0x1F;
#ifdef TWO_WORDS
		map_lut[i] = (row+column)<<1;
#else
		map_lut[i] = row+column;
#endif
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline  void make_name_lut(void)
{
	unsigned int i, j;
	for(j = 0; j < 0x10000; j++)
	{
		i = ((j >> 8) & 0xFF) | ((j  & 0xFF) <<8);
		unsigned int flip = (i >> 9) & 3;
		unsigned int pal = (i >> 11) & 1;
#ifdef TWO_WORDS
/*
Bit 15 - 13: Unused
Bit 12: Priority flag
Bit 11: Which palette to use
Bit 10: Vertical Flip Flag
Bit 09: Horizontal Flip Flag
Bit 08 - 00 : Pattern Index 
*/
		unsigned int priority = (i >> 12) & 1;
		name_lut[j] = (flip << 14 | priority << 13 | pal);
#else
		unsigned int name = (i & 0x1FF);
		name_lut[j] = (pal << 12 | flip << 10 | name);
#endif
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline  void make_cram_lut(void)
{
#ifdef GG
    for(unsigned int j = 0; j < 0x1000; j++)
    {
       int r = (j >> 0) & 0x0F;
        int g = (j >> 4) & 0x0F;
        int b = (j >> 8) & 0x0F;
		r = (r << 1) | (r >> 3);
		g = (g << 1) | (g >> 3);
		b = (b << 1) | (b >> 3);
		cram_lut[j] =RGB(r,g,b);
    }
#else
    for(unsigned int j = 0; j < 0x40; j++)
    {
        int r = (j >> 0) & 3;
        int g = (j >> 2) & 3;
        int b = (j >> 4) & 3;
        r  = (r << 3) | (r << 1) | (r >> 1);
        g = (g << 3) | (g << 1) | (g >> 1);
        b = (b << 3) | (b << 1) | (b >> 1);
        cram_lut[j] =RGB(r,g,b);
    }
#endif
}
//-------------------------------------------------------------------------------------------------------------------------------------
 void make_lut()
{
	make_name_lut();
	make_bp_lut();
	make_cram_lut();
	make_map_lut();
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void make_bp_lut(void)
{
    unsigned int i, j;
    for(j = 0; j < 0x10000; j++)
    {
        UINT32 row = 0;
        i = ((j >> 8) & 0xFF) | ((j  & 0xFF) <<8);

        if(i & 0x8000) row |= 0x20000000;
        if(i & 0x4000) row |= 0x02000000;
        if(i & 0x2000) row |= 0x00200000;
        if(i & 0x1000) row |= 0x00020000;
        if(i & 0x0800) row |= 0x00002000;
        if(i & 0x0400) row |= 0x00000200;
        if(i & 0x0200) row |= 0x00000020;
        if(i & 0x0100) row |= 0x00000002;
        if(i & 0x0080) row |= 0x10000000;
        if(i & 0x0040) row |= 0x01000000;
        if(i & 0x0020) row |= 0x00100000;
        if(i & 0x0010) row |= 0x00010000;
        if(i & 0x0008) row |= 0x00001000;
        if(i & 0x0004) row |= 0x00000100;
        if(i & 0x0002) row |= 0x00000010;
        if(i & 0x0001) row |= 0x00000001;
        bp_lut[j] = row;
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------
