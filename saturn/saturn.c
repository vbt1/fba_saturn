#include "saturn.h"

//#include "sc_saturn.h"
#define HEAP_WALK 1
#define GAME_BY_PAGE 16
//#define OVLADDR  0x060A5000
#define OVLADDR 0x060DA000
#define OVLAEND 0x060FF000
#define SIZEMAX  OVLAEND-OVLAEND //0x30000 //0x060FFC00-0x060CC000
#define LOWADDR 0x00200000
#define MALLOC_MAX 0xAA000

//#define DEBUG_DRV 1
volatile SysPort	*__port;
static trigger_t	pltrigger[2],pltriggerE[2];
extern unsigned char play;
unsigned char drvquit;
//-------------------------------------------------------------------------------------------------------------------------------------
void	UsrVblankIn( void )
{
#ifdef FONT
	char xx[4];
   PER_GetPort(__port);	
#endif
	PCM_MeVblIn();
	SCL_ScrollShow();
	
#ifndef ACTION_REPLAY

	if(play)
	{
		if(nBurnFunction!=NULL) nBurnFunction();
//		PCM_VblIn();
#endif
#ifdef FONT
		frame_y++;
					 
		if(frame_y==hz)
		{
				if(frame_displayed!=frame_x)
				{
					sprintf(xx,"%03d",frame_x);
					FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)xx,136,20);
					frame_displayed = frame_x;
				}

				frame_y=frame_x=0;
		}		   
#endif
#ifndef ACTION_REPLAY
	}
#endif
//	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void   UsrVblankOut( void )
{
	do_keypad();
	InpMake(FBA_KEYPAD);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void	SetVblank( void )
{
	INT_ChgMsk(INT_MSK_NULL,INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);
	INT_SetScuFunc(INT_SCU_VBLK_IN,UsrVblankIn);
	INT_SetScuFunc(INT_SCU_VBLK_OUT,UsrVblankOut);
	INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT,INT_MSK_NULL);
	__port = PER_OpenPort();
}
//-------------------------------------------------------------------------------------------------------------------------------------
int main(void)
{
	Uint8	*dst;
    Uint16  loop;	
   
    // 1.Zero Set .bss Section
    for (dst = (Uint8 *)&_bstart; dst < (Uint8 *)&_bend; dst++)
		*dst = 0;
	
    for (dst = (Uint8 *)SystemWork, loop = 0; loop < SystemSize; loop++)
		*dst = 0;

	memset(&play,0x00,1024);
	SYS_CHGSYSCK(1);             //28mhz
	set_imask(0); 
	
	memset((UINT8*)SCL_VDP2_VRAM_A0,0x00,0x80000);
	
	ss_main();
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void	_spr2_initialize( void )
{
	set_imask(0);
    SCL_SET_SPTYPE(SCL_TYPE0);
    SCL_SET_SPCLMD(SCL_PALETTE);
    SCL_SET_SPWINEN(SCL_SP_WINDOW);
	SPR_Initial(&aVRAM);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void	_spr2_transfercommand()
{
	  memcpyl(aVRAM,smsSprite,(nBurnSprites<<5) ) ;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initScrolling(Uint8 enabled,void *address)
{
    SCL_InitLineParamTb(&lp);
	lp.delta_enbl=OFF;
	lp.cell_enbl=OFF;
    lp.v_enbl=OFF;
    lp.h_enbl=enabled;
	if(enabled==ON)
	{
		lp.line_addr=(Uint32 )&address[0];//SCL_VDP2_VRAM_B0+0x4000;
		SclAddrLsTbl[0] = lp.line_addr;//+0x20;
		SclAddrLsTbl[1] = (Uint32 )&ls_tbl[0];
	}
	else
	{
		lp.line_addr=0x00;
		SclAddrLsTbl[0] = 0x00;
		SclAddrLsTbl[1] = NULL;
		nBurnLinescrollSize = 1;
	}

	Scl_n_reg.linecontrl = (lp.h_enbl << 1) & 0x0002;
    lp.interval=0;

//	(*(Uint16 *)0x25F8009A) = 0x0003; 		// line scroll space
//	(*(Uint16 *)0x25F80020) = 0x0303;			// display enable
	SclProcess = 2;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initSound()
{
	sndInit();
	PCM_MeInit();
//	Sint8		*input_addr;
//	Sint8		*buf;
//	ADP_DecMono(input_addr, buf);
//	PCM_DeclareUseAdpcm();
	PcmCreatePara	para;
	PcmInfo 		info;
	PcmStatus	*st;
	static PcmWork g_movie_work;

	PCM_PARA_WORK(&para) = (struct PcmWork *)&g_movie_work;
	PCM_PARA_RING_ADDR(&para) = (Sint8 *)SOUND_BUFFER;
	PCM_PARA_RING_SIZE(&para) = RING_BUF_SIZE;
	PCM_PARA_PCM_ADDR(&para) = PCM_ADDR;
	PCM_PARA_PCM_SIZE(&para) = PCM_SIZE;

	memset((Sint8 *)SOUND_BUFFER,0,SOUNDRATE*16);
	st = &g_movie_work.status;
	st->need_ci = PCM_ON;
	st->cnt_loop = 0;
 
	PCM_INFO_FILE_TYPE(&info) = PCM_FILE_TYPE_NO_HEADER;			
	PCM_INFO_DATA_TYPE(&info)=PCM_DATA_TYPE_RLRLRL;//PCM_DATA_TYPE_LRLRLR;
	PCM_INFO_FILE_SIZE(&info) = RING_BUF_SIZE;//SOUNDRATE*2;//0x4000;//214896;
	PCM_INFO_CHANNEL(&info) = 0x01;
	PCM_INFO_SAMPLING_BIT(&info) = 16;

	PCM_INFO_SAMPLING_RATE(&info)	= SOUNDRATE;//30720L;//44100L;
	PCM_INFO_SAMPLE_FILE(&info) = RING_BUF_SIZE;//SOUNDRATE*2;//30720L;//214896;
//	pcm = PCM_CreateStmHandle(&para, stm);
	pcm = createHandle(&para);

	PCM_SetPcmStreamNo(pcm, 0);

	PCM_SetInfo(pcm, &info);
	PCM_ChangePcmPara(pcm);

//	PCM_MeSetLoop(pcm, 0x1FF);//SOUNDRATE*60);
	PCM_MeSetLoop(pcm, 0);//SOUNDRATE*120);

//	if (pcm == NULL) {
//		while(1);
//		return;
//	}
	PCM_Start(pcm);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void resetLayers()
{
	Uint16	CycleTb[]={
		0x55ee, 0xeeee, //A1
		0xffff, 0xffff,	//A0
		0xff44, 0xeeee,   //B1
		0xffff, 0xffff,  //B0
	};
 	SclConfig	scfg;

	SS_FONT = (Uint16 *)SCL_VDP2_VRAM_A1;
	SS_MAP  = (Uint16 *)SCL_VDP2_VRAM_B0;
	SS_MAP2 = (Uint16 *)SCL_VDP2_VRAM_A0;

	SCL_ParametersInit();

	scfg.dispenbl 	   = ON;
	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover       = SCL_OVER_0;

	scfg.coltype 	   = SCL_COL_TYPE_256;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.plate_addr[0] = (Uint32)SS_MAP;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG0, &scfg);

	scfg.coltype 	       = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.plate_addr[0]= (Uint32)SS_FONT;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG1, &scfg);

	scfg.dispenbl 	   = OFF;
	scfg.plate_addr[0]= (Uint32)NULL;
	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.dispenbl 	   = OFF;
	scfg.plate_addr[0]= (Uint32)NULL;
	SCL_SetConfig(SCL_NBG3, &scfg);

	SCL_SetCycleTable(CycleTb);

    SCL_SET_S0PRIN(0);
    SCL_SET_N0PRIN(2);
    SCL_SET_N1PRIN(3);
    SCL_SET_N2PRIN(1);
}
//--------------------------------------------------------------------------------------------------------------
static void resetColors()
{
	Uint16 *VRAM;

	VRAM = (Uint16 *)SCL_COLRAM_ADDR;
	for(Uint32 i = 0; i < 4096; i++ )
		*(VRAM++) = 0x8000;

	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	SCL_SET_SPCAOS(0);
	SCL_SET_N0CAOS(0);
	SCL_SET_N1CAOS(0);
	SCL_SET_N2CAOS(0);
	SCL_SET_N3CAOS(0);

	SCL_AllocColRam(SCL_NBG0,OFF);
	SCL_AllocColRam(SCL_NBG1,OFF);

	SCL_SetColRam(SCL_NBG1,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void resetSound()
{
	PCM_MeSetVolume(pcm,0);
	PCM_DrvChangePcmPara(pcm,-1,-1);
	PCM_Task(pcm);

	PCM_MeStop(pcm);
//	PCM_MeReset(pcm);
	memset(SOUND_BUFFER,0x00,RING_BUF_SIZE*16);
	nSoundBufferPos=0;
	PCM_MeStart(pcm);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initSaturn()
{
	nBurnFunction = NULL;
	play=1;

	initScrolling(OFF,NULL);

	InitCD();
//	memset((UINT8*)SCL_VDP2_VRAM_A0,0x00000000,0x80000);
	resetLayers();


	memset(pltrigger[0],0x00,sizeof(trigger_t));
	memset(pltriggerE[0],0x00,sizeof(trigger_t));

	play = 0;
	resetColors();
	initSprites(352-1,240-1,0,0,0,0);

	SetVblank();
	SCL_SetLineParamNBG0(&lp);
		memset((UINT8*)SCL_VDP2_VRAM_A0,0x00,0x80000);

wait_vblank();
	play=1;

	resetSound();

	play = 0;
	wait_vblank();

	SS_REG   = &Scl_n_reg;
	SS_REGD =	&Scl_d_reg;
	SS_REGS =	&Scl_s_reg;
	SS_REGW =	&Scl_w_reg;
	SS_SPRAM = &aVRAM[0];
 	SS_N0PRI = &SclBgPriNum;
	SS_SPPRI = &SclSpPriNum;
	SS_OTHR  = &SclOtherPri;
	SS_BGMIX = &SclBgColMix;
	SS_SPRIT = &smsSprite[0];
	SS_SCL	 = &ls_tbl[0];
	SS_PORT  = (SysPort *)__port;
//	SS_SCL1	 = &ls_tbl1[0];

	col[0]=0;
	col[1]=9;
	col[2]=10;
	col[3]=11;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void ss_main(void)
{
//		DMA_ScuInit();

#ifndef ACTION_REPLAY
//	CdUnlock();
	InitCD();
#endif
	hz = get_hz();
	initSound();
	CSH_Init(CSH_4WAY);
	initSaturn();
	BurnDrvAssignList();

#ifndef ACTION_REPLAY
	if(FntAsciiFontData2bpp==NULL)
		FntAsciiFontData2bpp = (Uint8*)malloc(1600);
	GFS_Load(GFS_NameToId("FONT.BIN"),0,(void *)FntAsciiFontData2bpp,1600);
#endif
//	unsigned char *Mem = malloc((unsigned char *)MALLOC_MAX);
//	memset(Mem,0x00,MALLOC_MAX);
//	free(Mem);
//	Mem=NULL;

	while(1)
	{
		display_menu();
	}
}
//--------------------------------------------------------------------------------------------------------------------------------------
static void wait_key(Uint8 key)
{
	SysDevice	*device;
	do
	{
		device = PER_GetDeviceR( &__port[0], 0 );
		pltrigger[0]  = PER_GetTrigger( device );
	}while((pltrigger[0] & PER_DGT_B)==0) ;
}
//--------------------------------------------------------------------------------------------------------------------------------------
static void load_img(int id)
{
// vbt ‡ remettre	
	void (*fp)(unsigned int);
	fp = (void *)LOWADDR;
	(*fp)(id);

	if(id!=0)
	{
	 	wait_key(PER_DGT_B);
		load_img(0);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static unsigned char update_input(unsigned int *current_page,unsigned char *loaded, unsigned char *modified)
{
	unsigned int i=0;
	SysDevice	*device;
//	__port = PER_OpenPort();
	//PER_GetPort(__port);

	if(play==0 && ( device = PER_GetDeviceR( &__port[0], 0 )) != NULL )
	{
		pltriggerE[0] = pltrigger[0];
		pltrigger[0]  = PER_GetTrigger( device );
		pltriggerE[0] = (pltrigger[0]) ^ (pltriggerE[0]);
		pltriggerE[0] = (pltrigger[0]) & (pltriggerE[0]);

		for(i=0;i<8;i++)
		{
			if((pltriggerE[0] & pad_asign[i])!=0)
			{
				switch(pltriggerE[0] & pad_asign[i] )
				{
					case PER_DGT_A: 
					load_img(1);
					*modified = 1;
					break;

					case PER_DGT_C: 
					load_img(2);
					*modified = 1;
					break;

					case PER_DGT_D: 
					if(nBurnDrvSelect < nBurnDrvCount-1 && nBurnDrvSelect < ((*current_page) * GAME_BY_PAGE)-1) nBurnDrvSelect++;
					*modified = 1;
//					else								 nBurnDrvSelect=0;
					break;

					case PER_DGT_U:
					if(nBurnDrvSelect >  ((*current_page)-1) * GAME_BY_PAGE)	nBurnDrvSelect--;
					*modified = 1;
//					else						nBurnDrvSelect=nBurnDrvCount-1;
					break;

					case PER_DGT_L: 
					if(*current_page > 1) nBurnDrvSelect = (--(*current_page)-1) * GAME_BY_PAGE;
					*modified = 1;
					break;

					case PER_DGT_R: 
					if(*current_page * GAME_BY_PAGE  < nBurnDrvCount) nBurnDrvSelect = (++(*current_page)-1) * GAME_BY_PAGE;
					*modified = 1;
					break;

					case PER_DGT_S:
					run_fba_emulator();
					loaded[0] = 0;
					*modified = 1;
#ifdef HEAP_WALK
	 heapWalk();
#endif
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"A:Help",12,201);
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"C:Credits",127,201);

  
					break;

					default:
					break;
				}
			}
		}
	}
	return 0;
}
//--------------------------------------------------------------------------------------------------------------
//int the_loop=1;
static void display_menu(void)
{
	unsigned int l;
	unsigned char loaded = 0;


//	sc_init();
//	the_loop = 1;
#ifdef HEAP_WALK
	heapWalk();
#endif
	unsigned int current_page = 1,m;
	unsigned char modified = 1;
	char game_name[40];

	do
	{
		if(!loaded)
		{
// nettoyage emplacement du driver
			memset((Uint8 *)OVLADDR,0x00,SIZEMAX);
// vbt ‡ remette			
#ifndef DEBUG_DRV
			GFS_Load(GFS_NameToId("IMG.BIN"),  0,(void *)LOWADDR, GFS_BUFSIZ_INF);
			load_img(0);	
#endif
			loaded=1;
		}
		m=0;
//		char page_header[50];
//		sprintf(page_header,"Game list:                       %02d/%02d",current_page, (nBurnDrvCount+GAME_BY_PAGE-1)/GAME_BY_PAGE);
//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)page_header,12,12);

		if(modified==1)
		{
			for (l=(current_page-1)*GAME_BY_PAGE;l<(current_page)*GAME_BY_PAGE && l<nBurnDrvCount;l++ )
			{
				sprintf(game_name,"%-38s",pDriver[l]->szFullNameA);
				if(l==nBurnDrvSelect)	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)game_name,20,40+m);	  //12+m
				else					 FNT_Print256_2bpp   ((volatile Uint8 *)SS_FONT,(Uint8 *)game_name,20,40+m);
				m+=10;
			}

			for (;l<(current_page)*GAME_BY_PAGE;l++ )
			{
				sprintf(game_name,"%-38s"," ");
				FNT_Print256_2bpp   ((volatile Uint8 *)SS_FONT,(Uint8 *)game_name,20,40+m);
				m+=10;
			}
			modified=0;
		}
//		__port = PER_OpenPort();

		update_input(&current_page,&loaded,&modified);
		//sc_check();
//		scd_logout("display_menu",0);


	}while(1);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void SCL_ParametersInit(void)
{
//	Uint16	i;
/*
 *	System Registers Area Initialization
 */
	Scl_s_reg.tvmode = 0x8001; /* HREZ = B'001(352 pixels) the others = 0 */
	Scl_s_reg.ramcontrl = 0x1000; /* color 16bit mode; VRAM A, B banked */
/*
 *	Normal Scroll Registers Area Initialization
 */
	Scl_n_reg.n0_move_x = 0;
	Scl_n_reg.n0_move_y = 0;
	Scl_n_reg.n0_delta_x = FIXED(1);
	Scl_n_reg.n0_delta_y = FIXED(1);
	Scl_n_reg.n1_move_x = 0;
	Scl_n_reg.n1_move_y = 0;
	Scl_n_reg.n1_delta_x = FIXED(1);
	Scl_n_reg.n1_delta_y = FIXED(1);
	Scl_n_reg.n2_move_x = 0;
	Scl_n_reg.n2_move_y = 0;
	Scl_n_reg.n3_move_x = 0;
	Scl_n_reg.n3_move_y = 0;
	Scl_n_reg.zoomenbl = 0;
	Scl_n_reg.linecontrl = 0;
	Scl_n_reg.celladdr = 0;
	Scl_n_reg.lineaddr[0] = 0;
	Scl_n_reg.lineaddr[1] = 0;
	Scl_n_reg.linecolmode = 0;
	Scl_n_reg.backcolmode = 0;

	if(SclProcess == 0)	SclProcess = 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void SCL_CopyReg()
{
	Uint16 *regaddr = (Uint16 *)REGADDR;
	memcpyl(&regaddr[0x38], &Scl_n_reg, sizeof(SclNorscl));

	if(!play)
	{
		regaddr[0] = Scl_s_reg.tvmode;		/* add				by C.Y	*/
		regaddr[1] = Scl_s_reg.extenbl;		/* add				by C.Y	*/
		regaddr[3] = Scl_s_reg.vramsize;		/* add				by C.Y	*/
		memcpyw(&regaddr[7] , &Scl_s_reg.ramcontrl , 26 );
		memcpyl(&regaddr[0x14], &Scl_d_reg, sizeof(SclDataset));
	    memcpyw(&regaddr[0x60], &Scl_w_reg, sizeof(SclWinscl));

		memcpyl((SclOtherPriRegister *)0x25F800E0, &SclOtherPri, sizeof(SclOtherPri));
		memcpyl((SclSpPriNumRegister *)0x25F800F0, &SclSpPriNum, sizeof(SclSpPriNum));
		memcpyl((SclBgPriNumRegister *)0x25F800F8, &SclBgPriNum, sizeof(SclBgPriNum));
		memcpyl((SclBgColMixRegister *)0x25F80108, &SclBgColMix, sizeof(SclBgColMix));
	}
    //SCL_Memcpyw(&regaddr[0x38], &Scl_n_reg, sizeof(SclNorscl));
//	memcpyl(&regaddr[0x38], &Scl_n_reg, sizeof(SclNorscl));
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void SCL_InitLineParamTb(SclLineparam *lp)
{
	lp->h_enbl = OFF;
	lp->v_enbl = OFF;
	lp->delta_enbl = OFF;
	lp->cell_enbl = OFF;
	lp->line_addr = 0x00;
	lp->cell_addr = 0x00;
	lp->interval  = 0;

	for(unsigned int i = 0; i< SCL_MAXLINE; i++) 
	{
//	for(i = 0; i< 192; i++) {
		if(i < SCL_MAXCELL) 
		{
			lp->cell_tbl[i] = 0;
		}
		lp->line_tbl[i].h  = FIXED(0);
		lp->line_tbl[i].v  = FIXED(i);
		lp->line_tbl[i].dh = FIXED(1);
//		lp->line_tbl[i].h  = (0<<16);
//		lp->line_tbl[i].v  = (i<<16);
//		lp->line_tbl[i].dh = (1<<16);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void	SCL_ScrollShow(void)
{
    switch(SclProcess){
	    case 1:
		SCL_CopyReg();
		SclProcess = 0;
		break;
	    case 2:			/* line scroll setting */
		//SCL_Memcpyw((void *)SclAddrLsTbl[0],(void *)SclAddrLsTbl[1], 0x300);
		memcpyl((void *)SclAddrLsTbl[0],(void *)SclAddrLsTbl[1], nBurnLinescrollSize);
//		memcpyl((void *)SclAddrLsTbl[2],(void *)SclAddrLsTbl[3], nBurnLinescrollSize1);
		SCL_CopyReg();
		SclProcess = 0;
		break;
		default:
		break;
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------
void SCL_Open(void)
{
	if(SclProcess == 1)	SclProcess = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void SCL_Close(void)
{
	if(SclProcess == 0)	SclProcess = 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void	SCL_SetCycleTable(Uint16 *tp)
{
	for(Uint8 i = 0; i<8; i++){
		Scl_s_reg.vramcyc[i] = tp[i];
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void SCL_SetConfig(Uint16 sclnum, SclConfig *scfg)
{
	Uint16	temp;
	Uint16	*mapoffset = 0;
	Uint16	*map = 0;
	Uint16	boundary = 0;
	Uint16	shift = 0;
										/* add by C.yoshida */

	Uint16	max = 0;
	Uint16	flip;
	Uint16	mapoffsetW;
										/* add by C.yoshida */

	flip = scfg->flip;				/* get flip bit*/
	flip = (flip<<14) & 0x4000;
	switch(sclnum) {
		case SCL_NBG0:
			temp = scfg->dispenbl;		/* display enable */
			temp &= 0x0001;
			Scl_s_reg.dispenbl &= 0xfffe;
			Scl_s_reg.dispenbl |= temp;

			temp = scfg->charsize;		/* char size */
			temp &= 0x0001;
			Scl_d_reg.charcontrl0 &= 0xfffe;
			Scl_d_reg.charcontrl0 |= temp;

			Scl_d_reg.patnamecontrl[0] = scfg->patnamecontrl;

			temp = scfg->pnamesize;		/* pattern name size */
			temp = (temp <<15) & 0x8000;
			Scl_d_reg.patnamecontrl[0] &= 0x7fff;
			Scl_d_reg.patnamecontrl[0] |= temp;

			Scl_d_reg.patnamecontrl[0] &= 0xbfff;	/* flip bit set */
			Scl_d_reg.patnamecontrl[0] |= flip;	/* flip bit set */

			temp = scfg->platesize;		/* plate size */
			temp &=  0x0003;
			Scl_d_reg.platesize &= 0xfffc;
			Scl_d_reg.platesize |= temp;

			temp = scfg->bmpsize;		/* bitmap size */
			temp = (temp << 2) & 0x000c;
			Scl_d_reg.charcontrl0 &= 0xfff3;
			Scl_d_reg.charcontrl0 |= temp;

			temp = scfg->coltype;		/* color type */
			temp = (temp <<4) & 0x0070;
			Scl_d_reg.charcontrl0 &= 0xff8f;
			Scl_d_reg.charcontrl0 |= temp;

			temp = scfg->datatype;		/* data type */
			temp = (temp <<1) & 0x0002;
			Scl_d_reg.charcontrl0 &= 0xfffd;
			Scl_d_reg.charcontrl0 |= temp;

			Scl_d_reg.mapoffset0 &= 0xfff0;	/* Init map offset */ 
			mapoffset = &Scl_d_reg.mapoffset0; /* map offset */ 
			shift = 0;
			map = &Scl_d_reg.normap[0];
			max = 2;
			break;
		case SCL_NBG1:
			temp = scfg->dispenbl;		/* display enable */
			temp = (temp << 1) & 0x0002;
			Scl_s_reg.dispenbl &= 0xfffD;
			Scl_s_reg.dispenbl |= temp;

			temp = scfg->charsize;		/* char size */
			temp = (temp<<8) & 0x0100;
			Scl_d_reg.charcontrl0 &= 0xfeff;
			Scl_d_reg.charcontrl0 |= temp;

			Scl_d_reg.patnamecontrl[1] = scfg->patnamecontrl;

			temp = scfg->pnamesize;		/* pattern name size */
			temp = (temp <<15) & 0x8000;
			Scl_d_reg.patnamecontrl[1] &= 0x7fff;
			Scl_d_reg.patnamecontrl[1] |= temp;

			Scl_d_reg.patnamecontrl[1] &= 0xbfff;
			Scl_d_reg.patnamecontrl[1] |= flip; /* flip bit set*/

			temp = scfg->platesize;		/* plate size */
			temp = (temp << 2) &  0x000c;
			Scl_d_reg.platesize &= 0xfff3;
			Scl_d_reg.platesize |= temp;

			temp = scfg->bmpsize;		/* bitmap size */
			temp = (temp << 10) & 0x0c00;
			Scl_d_reg.charcontrl0 &= 0xf3ff;
			Scl_d_reg.charcontrl0 |= temp;

			temp = scfg->coltype;		/* color type */
			temp = (temp <<12) & 0x3000;
			Scl_d_reg.charcontrl0 &= 0xcfff;
			Scl_d_reg.charcontrl0 |= temp;

			temp = scfg->datatype;		/* data type */
			temp = (temp <<9) & 0x0200;
			Scl_d_reg.charcontrl0 &= 0xfdff;
			Scl_d_reg.charcontrl0 |= temp;

			Scl_d_reg.mapoffset0 &= 0xff0f;	/* Init map offset */ 
			mapoffset = &Scl_d_reg.mapoffset0; /* map offset */ 
			shift = 4;
			map = &Scl_d_reg.normap[2];
			max = 2;
			break;

		case SCL_NBG2:
			temp = scfg->dispenbl;		/* display enable */
			temp = (temp << 2) & 0x0004;
			Scl_s_reg.dispenbl &= 0xfffb;
			Scl_s_reg.dispenbl |= temp;

			temp = scfg->charsize;		/* char size */
			temp = temp & 0x0001;
			Scl_d_reg.charcontrl1 &= 0xfffe;
			Scl_d_reg.charcontrl1 |= temp;

			Scl_d_reg.patnamecontrl[2] = scfg->patnamecontrl;

			temp = scfg->pnamesize;		/* pattern name size */
			temp = (temp <<15) & 0x8000;
			Scl_d_reg.patnamecontrl[2] &= 0x7fff;
			Scl_d_reg.patnamecontrl[2] |= temp;

			Scl_d_reg.patnamecontrl[2] &= 0xbfff;
			Scl_d_reg.patnamecontrl[2] |= flip; /* flip bit set*/

			temp = scfg->platesize;		/* plate size */
			temp = (temp << 4) &  0x0030;
			Scl_d_reg.platesize &= 0xffcf;
			Scl_d_reg.platesize |= temp;

			temp = scfg->coltype;		/* color type */
			temp = (temp <<1) & 0x0002;
			Scl_d_reg.charcontrl1 &= 0xfffd;
			Scl_d_reg.charcontrl1 |= temp;

			Scl_d_reg.mapoffset0 &= 0xf0ff;	/* Init map offset */ 
			mapoffset = &Scl_d_reg.mapoffset0; /* map offset */ 
			shift = 8;
			map = &Scl_d_reg.normap[4];
			max = 2;
			break;

		case SCL_NBG3:
			temp = scfg->dispenbl;		/* display enable */
			temp = (temp << 3) & 0x0008;
			Scl_s_reg.dispenbl &= 0xfff7;
			Scl_s_reg.dispenbl |= temp;

			temp = scfg->charsize;		/* char size */
			temp = (temp << 4) & 0x0010;
			Scl_d_reg.charcontrl1 &= 0xffef;
			Scl_d_reg.charcontrl1 |= temp;

			Scl_d_reg.patnamecontrl[3] = scfg->patnamecontrl;

			temp = scfg->pnamesize;		/* pattern name size */
			temp = (temp <<15) & 0x8000;
			Scl_d_reg.patnamecontrl[3] &= 0x7fff;
			Scl_d_reg.patnamecontrl[3] |= temp;

			Scl_d_reg.patnamecontrl[3] &= 0xbfff;
			Scl_d_reg.patnamecontrl[3] |= flip; /* flip bit set*/

			temp = scfg->platesize;		/* plate size */
			temp = (temp << 6) &  0x00c0;
			Scl_d_reg.platesize &= 0xff3f;
			Scl_d_reg.platesize |= temp;

			temp = scfg->coltype;		/* color type */
			temp = (temp <<5) & 0x0020;
			Scl_d_reg.charcontrl1 &= 0xffdf;
			Scl_d_reg.charcontrl1 |= temp;

			Scl_d_reg.mapoffset0 &= 0x0fff;	/* Init map offset */ 
			mapoffset = &Scl_d_reg.mapoffset0;	/* map offset */ 
			shift = 12;
			map = &Scl_d_reg.normap[6];
			max = 2;
			break;
	}
/*
 *	Set Map Address
 */

	if(scfg->datatype == SCL_BITMAP) {
		mapoffsetW = ((scfg->plate_addr[0] - SCL_VDP2_VRAM)/0x20000) & 0x0007;
		*mapoffset |= mapoffsetW << shift;
	} else {
		if( scfg->pnamesize == 1){
			if(scfg->charsize == 0) {
				boundary = 0x2000;
			} else {
				boundary = 0x800;
			}
		} else {
			if(scfg->charsize == 0) {
				boundary = 0x4000;
			} else {
				boundary = 0x1000;
			}
		}
		mapoffsetW = (0x01c0 & ((scfg->plate_addr[0] - SCL_VDP2_VRAM) / boundary))
			>> 6;
		*mapoffset |= mapoffsetW << shift;
	}

	for(Uint16 i = 0; i < max; i++) {
		map[i] = (0x003f & ((scfg->plate_addr[i * 2] - SCL_VDP2_VRAM)
				/ boundary));
		temp = (0x003f & ((scfg->plate_addr[i * 2 + 1] - SCL_VDP2_VRAM)
				/ boundary)) << 8;

		map[i] |= (temp & 0x3f00);
	}
//	if(SclProcess == 0)	SclProcess = 4;
//modif VBT
	if(SclProcess == 0)	SclProcess = 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void  SCL_SetColRam(Uint32 Object, Uint32 Index,Uint32 num,void *Color)
{
	Uint16	*ram16;
	ram16   = (Uint16 *)((Uint32)SCL_COLRAM_ADDR + ((SCL_GetColRamOffset(Object) * 0x200))) + Index;
	memcpyl(ram16,(Uint16 *)Color,num*2);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void  SCL_SetColRamOffset(Uint32 Object, Uint32 Offset,Uint8 transparent)
{
    if(Object & SCL_SPR)	SCL_SET_SPCAOS(Offset);

    if( (Object & SCL_NBG0)) //|| (Object & SCL_RBG1))
    {
	SCL_SET_N0CAOS(Offset);
	if(transparent)	Scl_s_reg.dispenbl |= 0x0100;
	else		Scl_s_reg.dispenbl &= 0xfeff;
    }

    if( (Object & SCL_NBG1)) // || (Object & SCL_EXBG))
    {
	SCL_SET_N1CAOS(Offset);
	if(transparent)	Scl_s_reg.dispenbl |= 0x0200;
	else		Scl_s_reg.dispenbl &= 0xfdff;
    }

    if(Object & SCL_NBG2)
    {
	SCL_SET_N2CAOS(Offset);
	if(transparent)	Scl_s_reg.dispenbl |= 0x0400;
	else		Scl_s_reg.dispenbl &= 0xfbff;
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------
Uint32  SCL_AllocColRam(Uint32 Surface, Uint8 transparent)
{
	for(Uint8 i=0;i<8;i++)
	{
		if(SclColRamAlloc256[i]==0)
		{
			SclColRamAlloc256[i]=Surface;
			SCL_SetColRamOffset(Surface,i,transparent);
			return(SCL_COLRAM_ADDR+(512*i));
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
// VBT passage en static
static Uint32  SCL_GetColRamOffset(Uint32 Object)
{
    switch(Object){
      case SCL_SPR:
	return(SCL_GET_SPCAOS());
	break;
      case SCL_NBG0:
//      case SCL_RBG1:
	return(SCL_GET_N0CAOS());
	break;
      case SCL_NBG1:
//      case SCL_EXBG:
	return(SCL_GET_N1CAOS());
	break;
      default:
	break;
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void SCL_SetLineParamNBG0(SclLineparam *lp)
{
	Uint32	*addr;
	addr = &Scl_n_reg.lineaddr[0];
	*addr = (lp->line_addr >>1) & 0x0007ffff;
	SclProcess = 2; //obligatoire
}
/*
static void SCL_SetLineParamNBG1(SclLineparam *lp)
{
	Uint32	*addr;
	addr = &Scl_n_reg.lineaddr[1];
	*addr = (lp->line_addr1 >>1) & 0x0007ffff;
	SclProcess = 2; //obligatoire
}*/
//-------------------------------------------------------------------------------------------------------------------------------------
void SPR_Initial(Uint8 **VRAM)
{
    int     iMask;
    iMask = get_imask();
    set_imask(15);                           // interrupt disable

	/* change frame buffer to auto change mode  */
    SPR_WRITE_REG(SPR_W_FBCR, SPR_FBCR_AUTO);

    /* trig set to auto start drawing */
    SPR_WRITE_REG(SPR_W_PTMR, 0x0002);
    *VRAM     = (Uint8*)VRAM_ADDR;
    *(*VRAM)     = 0x80;                 /* set END command to VRAM top area */
    *((*VRAM)+1) = 0x00;
    set_imask(iMask);                    /* interrupt enable             */
play = 0;

}
//-------------------------------------------------------------------------------------------------------------------------------------
void SPR_SetEraseData(Uint16 eraseData, Uint16 leftX, Uint16 topY,Uint16 rightX, Uint16 botY)
{
	SPR_WRITE_REG(SPR_W_EWDR, eraseData);      /* set erase write data */

	leftX >>= 3;
	SPR_WRITE_REG(SPR_W_EWLR, (leftX << 9) + topY); /* set erase screen left top */

	rightX++;
	rightX >>= 3;
	SPR_WRITE_REG(SPR_W_EWRR, (rightX << 9) + botY);   /* set erase screen right bottom */
}
//-------------------------------------------------------------------------------------------------------------------------------------
void SND_Init(SndIniDt *sys_ini)
{
	Uint8 *adr_sys_info_tbl;
	
	/** BEGIN ****************************************************************/

    PER_SMPC_SND_OFF();                         /* ÉTÉEÉìÉhOFF               */
    POKE_W(ADR_SCSP_REG, SCSP_REG_SET); 
                                                /* SCSPã§í ⁄ºﬁΩ¿ê›íË         */

//    DmaClrZero(ADR_SND_MEM, MEM_CLR_SIZE);      // DMAÉÅÉÇÉäÉ[ÉçÉNÉäÉA       
		memset(ADR_SND_MEM, 0x00, MEM_CLR_SIZE);

		//CopyMem(ADR_SND_VECTOR,
		memcpyw(ADR_SND_VECTOR,
                   (void *)(SND_INI_PRG_ADR(*sys_ini)),
                   SND_INI_PRG_SZ(*sys_ini));   // 68KÃﬂ€∏ﬁ◊—ì]ëó            

    adr_sys_info_tbl = (Uint8 *)(ADR_SND_MEM + PEEK_L(ADR_SYS_TBL +
                                 ADR_SYS_INFO));
                                                /* ºΩ√—èÓïÒ√∞ÃﬁŸ±ƒﬁ⁄ΩéÊìæ    */
    adr_host_int_work = (Uint8 *)(ADR_SND_MEM + PEEK_L(ADR_SYS_TBL +
                                  ADR_HOST_INT));
                                                
    adr_com_block = adr_host_int_work;  /* åªç›èëÇ´çûÇ›∫œ›ƒﬁÃﬁ€Ø∏±ƒﬁ⁄Ωèâä˙âª */

//		CopyMem((void *)
		memcpyw((void *)
                    (PEEK_L(adr_sys_info_tbl + ADR_ARA_ADR) + ADR_SND_MEM),
                   (void *)(SND_INI_ARA_ADR(*sys_ini)),
                   CHG_LONG(SND_INI_ARA_SZ(*sys_ini))); /* ª≥›ƒﬁ¥ÿ±œØÃﬂì]ëó  */
/* 1994/02/24 Start*/
    intrflag = 0;         /* äÑÇËçûÇ›ÉtÉâÉOÇÃèâä˙âª */
/* 1994/02/24 End */

    PER_SMPC_SND_ON();                          /* ÉTÉEÉìÉhON                */

}
//-------------------------------------------------------------------------------------------------------------------------------------
Uint8 SND_ChgMap(Uint8 area_no)
 {
/* 1994/02/24 Start */
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, area_no);                        /* ÉpÉâÉÅÅ[É^ÉZÉbÉg          */
    SET_COMMAND(COM_CHG_MAP);                   /* ÉRÉ}ÉìÉhÉZÉbÉg            */

    while(PEEK_W(adr_com_block + ADR_COM_DATA)) _WAIT_();
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, area_no);                        /* ÉpÉâÉÅÅ[É^ÉZÉbÉg          */
    SET_COMMAND(COM_CHG_MAP);                   /* ÉRÉ}ÉìÉhÉZÉbÉg            */
    while(PEEK_W(adr_com_block + ADR_COM_DATA)) _WAIT_();
    HOST_SET_RETURN(SND_RET_SET);
}

#define DMA_SCU_END     0
//-------------------------------------------------------------------------------------------------------------------------------------
static void sndInit(void)
{
	SndIniDt 	snd_init;
	unsigned char sound_map[]={0xFF,0xFF};
//	unsigned char sound_map[]={ 0x00, 0x00, 0xB0, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
// 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00 0x00, 0x07 0x4D, 0x8C, 0xFF, 0xFF                ......M...
//unsigned char sound_map[]={ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xB0, 0x00, 0x00, 0x07, 0x4D, 0x8C, 0xFF, 0xFF };

#ifndef ACTION_REPLAY
	GFS_Load(GFS_NameToId(SDDRV_NAME),0,(void *)SDDRV_ADDR,SDDRV_SIZE);
	SND_INI_PRG_ADR(snd_init) = (Uint16 *)SDDRV_ADDR;
	SND_INI_PRG_SZ(snd_init)  = (Uint16)  SDDRV_SIZE;
#else
	
	SND_INI_PRG_ADR(snd_init) = (Uint16 *)snddrv;
	SND_INI_PRG_SZ(snd_init)  = (Uint16)  sizeof(snddrv);
#endif

	SND_INI_ARA_ADR(snd_init) 	= (Uint16 *)sound_map;
	SND_INI_ARA_SZ(snd_init) 	= (Uint16)sizeof(sound_map);

	SND_Init(&snd_init);
	SND_ChgMap(0);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static PcmHn createHandle(PcmCreatePara *para)
{
	PcmHn pcm;

	pcm = PCM_CreateMemHandle(para);
	if (pcm == NULL) {
		return NULL;
	}
#ifdef _30_HZ
	PCM_NotifyWriteSize(pcm, SOUNDRATE*2*QUAL);
#else
//	PCM_NotifyWriteSize(pcm, RING_BUF_SIZE);//SOUNDRATE*2); // vbt ? revoir !!!!
#endif
	return pcm;
}
//-------------------------------------------------------------------------------------------------------------------------------------
SndRet SND_ChgPcm(SndPcmChgPrm *cprm)
{
/* 1994/02/24 Start */
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, SND_PRM_NUM(*cprm));
    SET_PRM(1, (SND_PRM_LEV(*cprm) << 5) | ChgPan(SND_PRM_PAN(*cprm)));
    SET_PRM(2, SND_PRM_PICH(*cprm) >> 8);
    SET_PRM(3, SND_PRM_PICH(*cprm));
    SET_PRM(4, (SND_R_EFCT_IN(*cprm) << 3) | SND_R_EFCT_LEV(*cprm));
    SET_PRM(5, (SND_L_EFCT_IN(*cprm) << 3) | SND_L_EFCT_LEV(*cprm));
    SET_COMMAND(COM_CHG_PCM_PRM);               /* ÉRÉ}ÉìÉhÉZÉbÉg            */
    HOST_SET_RETURN(SND_RET_SET);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static Uint8 GetComBlockAdr(void)
{
    if(*NOW_ADR_COM_DATA){              /* à»ëOÇÃÃﬁ€Ø∏Ç™à¯Ç´éÊÇËçœÇ›Ç≈Ç»Ç¢Ç©?*/
        /* éüÉRÉ}ÉìÉhÉuÉçÉbÉNÉAÉhÉåÉXê›íËèàóù ********************************/
        if(NOW_ADR_COM_DATA >= (MAX_ADR_COM_DATA - SIZE_COM_BLOCK)){
                                                    /* ç≈ëÂílÇ©?            */
            return OFF;                             /* Ãﬁ€Ø∏ãÛÇ´ñ≥Çµ      */
        }else{
            adr_com_block += SIZE_COM_BLOCK;        /* åªç›∫œ›ƒﬁÃﬁ€Ø∏∂≥›ƒ±ØÃﬂ*/
            while(NOW_ADR_COM_DATA < (MAX_ADR_COM_DATA - SIZE_COM_BLOCK)){
                if(*NOW_ADR_COM_DATA){
                    adr_com_block += SIZE_COM_BLOCK;
                }else{
                    return ON;                      /* Ãﬁ€Ø∏ãÛÇ´óLÇË         */
                }
            }
            return OFF;                             /* Ãﬁ€Ø∏ãÛÇ´ñ≥Çµ         */
        }
    }else{
        adr_com_block = adr_host_int_work;  /* Ãﬁ€Ø∏ÇÃêÊì™Ç÷              */
        while(NOW_ADR_COM_DATA < (MAX_ADR_COM_DATA - SIZE_COM_BLOCK)){
            if(*NOW_ADR_COM_DATA){
                adr_com_block += SIZE_COM_BLOCK;
            }else{
                return ON;                          /* Ãﬁ€Ø∏ãÛÇ´óLÇË         */
            }
        }
        return OFF;                                 /* Ãﬁ€Ø∏ãÛÇ´ñ≥Çµ         */
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------
static Uint16 ChgPan(SndPan pan)
{
    return(((pan) < 0) ? (~(pan) + 0x10 + 1) : (pan));
}
//-------------------------------------------------------------------------------------------------------------------------------------
SndRet SND_StartPcm(SndPcmStartPrm *sprm, SndPcmChgPrm *cprm)
{
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
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
SndRet SND_StopPcm(SndPcmNum pcm_num)
{
/* 1994/02/24 Start */
    if(intrflag) return(SND_RET_NSET);
    intrflag = 1;
/* 1994/02/24 End */
    if(GetComBlockAdr() == OFF) HOST_SET_RETURN(SND_RET_NSET);
    SET_PRM(0, pcm_num);                        /* ÉpÉâÉÅÅ[É^ÉZÉbÉg          */
    SET_COMMAND(COM_STOP_PCM);                  /* ÉRÉ}ÉìÉhÉZÉbÉg            */
    HOST_SET_RETURN(SND_RET_SET);
}
//-------------------------------------------------------------------------------------------------------------------------------------
#define CSH_CCR			(*(volatile Uint8 * )0xfffffe92)	/*	∑¨Øº≠∫›ƒ€∞Ÿ⁄ºﬁΩ¿±ƒﬁ⁄Ω	*/

#define CSH_4WAY			((Uint16)0x0000)	/*	4≥™≤æØƒ±øº¥≤√®Ãﬁ”∞ƒﬁ	*/
#define CSH_2WAY			((Uint16)0x0008)	/*	2≥™≤æØƒ±øº¥≤√®Ãﬁ”∞ƒﬁ	*/
#define CSH_DISABLE			((Uint16)0x0000)	/*	∑¨Øº≠√ﬁ®æ∞ÃﬁŸ”∞ƒﬁ		*/
#define CSH_ENABLE			((Uint16)0x0001)	/*	∑¨Øº≠≤»∞ÃﬁŸ”∞ƒﬁ			*/
#define CSH_CODE_ENABLE		((Uint16)0x0000)	/*	∫∞ƒﬁ≤»∞ÃﬁŸ”∞ƒﬁ			*/
#define CSH_CODE_DISABLE	((Uint16)0x0002)	/*	∫∞ƒﬁ√ﬁ®æ∞ÃﬁŸ”∞ƒﬁ		*/
#define CSH_DATA_ENABLE		((Uint16)0x0000)	/*	√ﬁ∞¿≤»∞ÃﬁŸ”∞ƒﬁ			*/
#define CSH_DATA_DISABLE	((Uint16)0x0004)	/*	√ﬁ∞¿√ﬁ®æ∞ÃﬁŸ”∞ƒﬁ		*/

#define CSH_SET_ENABLE(sw)		((Uint16)(CSH_CCR = (Uint8)((CSH_CCR & 0xfe)|(sw))))
#define CSH_SET_CODE_FILL(sw)	((Uint16)(CSH_CCR = (Uint8)((CSH_CCR & 0xfd)|(sw))))
#define CSH_SET_DATA_FILL(sw)	((Uint16)(CSH_CCR = (Uint8)((CSH_CCR & 0xfb)|(sw))))
#define CSH_SET_WAY_MODE(sw)	((Uint16)(CSH_CCR = (Uint8)((CSH_CCR & 0xf7)|(sw))))
#define CSH_SET_ACS_WAY(way)	((Uint16)(CSH_CCR = (Uint8)((CSH_CCR & 0x3f)|((way) << 6))))

void CSH_Init(Uint16 sw)
{
	Uint32 way;									/*	ÉAÉNÉZÉXÉEÉFÉCêßå‰ïœêî	*/
	Uint32 i;									/*	ÉAÉNÉZÉXÉâÉCÉìêßå‰ïœêî	*/
	Uint32 *adrs;								/*	±ƒﬁ⁄Ω±⁄≤óÃàÊêÊì™±ƒﬁ⁄Ω	*/

	CSH_SET_ENABLE(CSH_DISABLE);				/*	ÉLÉÉÉbÉVÉÖÉfÉBÉZÅ[ÉuÉã	*/
	for (way = 0; way < 4; way++) {				/*	ÇSÉEÉFÉCÉãÅ[Év			*/
		CSH_SET_ACS_WAY(way);					/*	ÉAÉNÉZÉXÉEÉFÉCéwíË		*/
		adrs = (Uint32 *)0x60000000;			/*	±ƒﬁ⁄Ω±⁄≤óÃàÊêÊì™±ƒﬁ⁄Ω	*/
		for (i = 0; i < 64; i++) {				/*	64ÉâÉCÉìÉãÅ[Év			*/
			*adrs = 0;							/*	ÇPÉâÉCÉìÇÃèÓïÒÉNÉäÉA	*/
			adrs += 4;							/*	éüÇÃÉâÉCÉìÇ…êiÇﬁ		*/
		}										/*	end for i				*/
	}											/*	end for way				*/
	CSH_SET_WAY_MODE(sw);						/*	ÉEÉFÉCÉÇÅ[Éhê›íË		*/
	CSH_SET_CODE_FILL(CSH_CODE_ENABLE);			/*	ÉRÅ[ÉhÉtÉBÉãÉCÉlÅ[ÉuÉã	*/
	CSH_SET_DATA_FILL(CSH_DATA_ENABLE);			/*	ÉfÅ[É^ÉtÉBÉãÉCÉlÅ[ÉuÉã	*/
	CSH_SET_ENABLE(CSH_ENABLE);					/*	ÉLÉÉÉbÉVÉÖÉCÉlÅ[ÉuÉã	*/
}
//-------------------------------------------------------------------------------------------------------------------------------------

void CSH_Purge(void *adrs, Uint32 P_size)
{
	typedef Uint32 Line[0x10/sizeof(Uint32)];	/* ÉâÉCÉìÇÕ 0x10 ÉoÉCÉgíPà  */
	Line *ptr, *end;
	Uint32 zero = 0;

	ptr = (void*)(((Uint32)adrs & 0x1fffffff) | 0x40000000);	/* ÉLÉÉÉbÉVÉÖÉpÅ[ÉWóÃàÊ */
	end = (void*)((Uint32)ptr + P_size - 0x10);	/* èIóπÉ|ÉCÉìÉ^Åi-0x10 ÇÕÉ|ÉXÉgÉCÉìÉNÉäÉÅÉìÉgÇÃà◊Åj */
	ptr = (void*)((Uint32)ptr & -sizeof(Line));	/* ÉâÉCÉìÉAÉâÉCÉÅÉìÉgÇ…êÆçá */
	do {
		(*ptr)[0] = zero;			/* ÉLÉÉÉbÉVÉÖÉpÅ[ÉW */
	} while (ptr++ < end);			/* É|ÉXÉgÉCÉìÉNÉäÉÅÉìÉgÇÕÉfÉBÉåÉCÉXÉçÉbÉgäàópÇÃà◊ */
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void wait_vblank(void)
{
     while((TVSTAT & 8) == 0);
     while((TVSTAT & 8) == 8);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static unsigned int get_hz(void)
{
	if((TVSTAT & 1) == 0)
		return 60;
	else
		return 50;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static int DoInputBlank(int bDipSwitch)
{
  int iJoyNum = 0;
   int i=0; 
  // Reset all inputs to undefined (even dip switches, if bDipSwitch==1)
  char controlName[12];
  
  DIPInfo.nDIP = 0;
  // Get the targets in the library for the Input Values
  for (i=0; i<nGameInpCount; i++)
  {
    struct BurnInputInfo bii;
    memset(&bii,0,sizeof(bii));
//    BurnDrvGetInputInfo(&bii,i);
	pDriver[nBurnDrvSelect]->GetInputInfo(&bii, i);
    
    //if (bDipSwitch==0 && bii.nType==2) continue; // Don't blank the dip switches
	
	if (bii.nType==BIT_DIPSWITCH)
	{
		if (DIPInfo.nDIP == 0)
		{
			DIPInfo.nFirstDIP = i;
			DIPInfo.nDIP = nGameInpCount - i;
			if (DIPInfo.DIPData== NULL)
				DIPInfo.DIPData = (struct GameInp *)malloc(DIPInfo.nDIP * sizeof(struct GameInp));
			memset(DIPInfo.DIPData,0,DIPInfo.nDIP * sizeof(struct GameInp));
		}
		DIPInfo.DIPData[i-DIPInfo.nFirstDIP].pVal = bii.pVal;
		DIPInfo.DIPData[i-DIPInfo.nFirstDIP].nType = bii.nType;
		DIPInfo.DIPData[i-DIPInfo.nFirstDIP].nConst = 0;
		DIPInfo.DIPData[i-DIPInfo.nFirstDIP].nBit = 0;
	}

	if (bii.szInfo[0]=='p')
		iJoyNum = bii.szInfo[1] - '1';	
	else
	{
	/*	if (strcmp(bii.szInfo, "diag") == 0 || strcmp(bii.szInfo, "test") == 0)
		{
			ServiceDip = bii.pVal;
		}	*/
		continue;
	}
	sprintf(controlName,"p%i coin",iJoyNum+1);
    if (strcmp(bii.szInfo, controlName) == 0)
    {
    	GameInp[iJoyNum][0].nBit = 4;
		GameInp[iJoyNum][0].pVal = bii.pVal;
		GameInp[iJoyNum][0].nType = bii.nType;
    }
    else {
	sprintf(controlName,"p%i start",iJoyNum+1);
    if (strcmp(bii.szInfo, controlName) == 0)
    {
    	GameInp[iJoyNum][1].nBit = 5;
		GameInp[iJoyNum][1].pVal = bii.pVal;
		GameInp[iJoyNum][1].nType = bii.nType;
		switch (iJoyNum)
		{
			case 0:
				P1Start = bii.pVal;
			break;
			case 1:
				P2Start = bii.pVal;
			break;
		}	
	}
    else {
	sprintf(controlName,"p%i up",iJoyNum+1);
    if (strcmp(bii.szInfo, controlName) == 0)
    {
    	GameInp[iJoyNum][2].nBit = 0;
		GameInp[iJoyNum][2].pVal = bii.pVal;
		GameInp[iJoyNum][2].nType = bii.nType;
    }
    else {
	sprintf(controlName,"p%i down",iJoyNum+1);
    if (strcmp(bii.szInfo, controlName) == 0)
    {
    	GameInp[iJoyNum][3].nBit = 1;
		GameInp[iJoyNum][3].pVal = bii.pVal;
		GameInp[iJoyNum][3].nType = bii.nType;
    }
    else {
	sprintf(controlName,"p%i left",iJoyNum+1);
    if (strcmp(bii.szInfo, controlName) == 0)
    {
    	GameInp[iJoyNum][4].nBit = 2;
		GameInp[iJoyNum][4].pVal = bii.pVal;
		GameInp[iJoyNum][4].nType = bii.nType;
    }
    else {
	sprintf(controlName,"p%i right",iJoyNum+1);
    if (strcmp(bii.szInfo, controlName) == 0)
    {
    	GameInp[iJoyNum][5].nBit = 3;
		GameInp[iJoyNum][5].pVal = bii.pVal;
		GameInp[iJoyNum][5].nType = bii.nType;
    }
    else {
	sprintf(controlName,"p%i fire 1",iJoyNum+1);
    if (strcmp(bii.szInfo, controlName) == 0)
    {
    	GameInp[iJoyNum][6].nBit = 6;
		GameInp[iJoyNum][6].pVal = bii.pVal;
		GameInp[iJoyNum][6].nType = bii.nType;
    }
    else {
	sprintf(controlName,"p%i fire 2",iJoyNum+1);
    if (strcmp(bii.szInfo, controlName) == 0)
    {
    	GameInp[iJoyNum][7].nBit = 7;
		GameInp[iJoyNum][7].pVal = bii.pVal;
		GameInp[iJoyNum][7].nType = bii.nType;
    }
    else {
	sprintf(controlName,"p%i fire 3",iJoyNum+1);
    if (strcmp(bii.szInfo, controlName) == 0)
    {
    	GameInp[iJoyNum][8].nBit = 8;
		GameInp[iJoyNum][8].pVal = bii.pVal;
		GameInp[iJoyNum][8].nType = bii.nType;
    }
    else {
	sprintf(controlName,"p%i fire 4",iJoyNum+1);
    if (strcmp(bii.szInfo, controlName) == 0)
    {
    	GameInp[iJoyNum][9].nBit = 9;
		GameInp[iJoyNum][9].pVal = bii.pVal;
		GameInp[iJoyNum][9].nType = bii.nType;
    }
    else {
	sprintf(controlName,"p%i fire 5",iJoyNum+1);
    if (strcmp(bii.szInfo, controlName) == 0)
    {
    	GameInp[iJoyNum][10].nBit = 10;
		GameInp[iJoyNum][10].pVal = bii.pVal;
		GameInp[iJoyNum][10].nType = bii.nType;
    }
    else {
	sprintf(controlName,"p%i fire 6",iJoyNum+1);
    if (strcmp(bii.szInfo, controlName) == 0)
    {
    	GameInp[iJoyNum][11].nBit = 11;
		GameInp[iJoyNum][11].pVal = bii.pVal;
		GameInp[iJoyNum][11].nType = bii.nType;
    }}}}}}}}}}}}
  }
  return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void InpExit()
{
//  bInputOk = false;
//  nGameInpCount = 0;
  if (DIPInfo.nDIP!=NULL)
	{
	memset(DIPInfo.DIPData,0,DIPInfo.nDIP * sizeof(struct GameInp));
		if (DIPInfo.DIPData!=NULL)
			free (DIPInfo.DIPData);
	}
	nGameInpCount = 0;
  DIPInfo.DIPData=NULL;
//  return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void InpMake(unsigned int key[])
{
	int i=0; 
	int down = 0;

	short joyNum=0;
//	for (joyNum=0;joyNum<numJoy;joyNum++)
	{
		for (i=0; i<12; i++)
		{
			if (GameInp[joyNum][i].pVal == NULL) continue;
			
			if ( GameInp[joyNum][i].nBit >= 0 )
			{
				down = key[joyNum] & (1U << GameInp[joyNum][i].nBit);
				
				if (GameInp[joyNum][i].nType!=1) {
					// Set analog controls to full
					if (down) *(GameInp[joyNum][i].pVal)=0xff; else *(GameInp[joyNum][i].pVal)=0x01;
				}
				else
				{
					// Binary controls
					if (down) *(GameInp[joyNum][i].pVal)=1;    else *(GameInp[joyNum][i].pVal)=0;
				}
			}
		}
	}
	for (i=0; i<(int)DIPInfo.nDIP; i++) {
		if (DIPInfo.DIPData[i].pVal == NULL)
			continue;
		*(DIPInfo.DIPData[i].pVal) = DIPInfo.DIPData[i].nConst;
	}
	if (P1P2Start)
	{
		*(P1Start) = *(P2Start) = 1;
	}
//	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void InpInit()
{
  unsigned int i=0; 
  int nRet=0;
//  bInputOk = 0;
  // Count the number of inputs
 nGameInpCount=0;
 for (i=0;i<0x1000;i++) 
	  {
//    nRet = BurnDrvGetInputInfo(NULL,i);
	nRet = pDriver[nBurnDrvSelect]->GetInputInfo(NULL, i);
    if (nRet!=0) {   // end of input list
    	nGameInpCount=i; 
    	break; 
    }
  }

  memset(GameInp,0,12*4*sizeof(struct GameInp));
  DoInputBlank(1);
}

//-------------------------------------------------------------------------------------------------------------------------------------
static void InpDIP()
{
	struct BurnDIPInfo bdi;
	struct GameInp* pgi;
	int i;  ///, j;
	int nDIPOffset = 0;

	// get dip switch offset 
	for (i = 0; BurnDrvGetDIPInfo(&bdi, i) == 0; i++)
		if (bdi.nFlags == 0xF0) {
			nDIPOffset = bdi.nInput;
			break;
		}

	// set DIP to default
	i = 0;
//	j = 40;
	char bDifficultyFound = 0;
	while (BurnDrvGetDIPInfo(&bdi, i) == 0) 
	{
//		char toto[100];
//		sprintf(toto,"%2d. %02x '%s'\n", bdi.nInput, bdi.nFlags, bdi.szText);
//		FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)toto,10,60);
		
		if (bdi.nFlags == 0xFF) 
		{
			pgi = DIPInfo.DIPData + (bdi.nInput + nDIPOffset - DIPInfo.nFirstDIP);
			pgi->nConst = (pgi->nConst & ~bdi.nMask) | (bdi.nSetting & bdi.nMask);
		}
		i++;
	}
	for (i=0,pgi=DIPInfo.DIPData; i<(int)DIPInfo.nDIP; i++,pgi++) {
		if (pgi->pVal == NULL)
			continue;
		*(pgi->pVal) = pgi->nConst;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static int DoLibInit() // Do Init of Burn library driver
{
	return (BurnDrvInit()) ? 3 : 0 ;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static int nDrvInit(int nDrvNum)
{
	void (*fp)(char *);
	char drv_file[14];
//	DrvExit(); // Make sure exited
	nBurnDrvSelect = nDrvNum; // set the driver number

	shared   = pDriver[nBurnDrvSelect];

	if(BurnDrvGetTextA(DRV_PARENT)==NULL)
		sprintf(drv_file,"d_%s.bin",BurnDrvGetTextA(DRV_NAME));
	else
		sprintf(drv_file,"d_%s.bin",BurnDrvGetTextA(DRV_PARENT));

// vbt ‡ remettre    
#ifndef DEBUG_DRV 
	GFS_Load(GFS_NameToId(strupr(drv_file)), 0, (void *)OVLADDR, GFS_BUFSIZ_INF);
#endif
	ChangeDir(BurnDrvGetTextA(DRV_NAME));

	fp = (void *)OVLADDR;
	(*fp)(pDriver[nBurnDrvSelect]->szShortName);

	if (BurnDrvInit()!=0) 
	{
		BurnDrvExit(); // Exit the driver
		return 1;
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void check_exit(Uint16 data)
{
	 if(            ((data & PER_DGT_S) != 0) 
			   && ((data & PER_DGT_X) != 0) 
			   && ((data & PER_DGT_Y) != 0) 
			   && ((data & PER_DGT_Z) != 0)
	   )
		_smpc_SYSRES();

	 if(            ((data & PER_DGT_S) != 0) 
			   && ((data & PER_DGT_A) != 0) 
			   && ((data & PER_DGT_B) != 0) 
			   && ((data & PER_DGT_C) != 0)
	   )
	{
		play = 0;
		drvquit = 1;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void do_keypad()
{
	FBA_KEYPAD[0] = 0;
	FBA_KEYPAD[1] = 0;
	FBA_KEYPAD[2] = 0;
	FBA_KEYPAD[3] = 0;
//	ServiceRequest =0;
	P1P2Start = 0;

	SysDevice	*device;

	if(( device = PER_GetDeviceR( &__port[0], 0 )) != NULL )
//	device = PER_GetDeviceR( &__port[0], 0 ); 
	{
		pltriggerE[0] = pltrigger[0];
		pltrigger[0]  = PER_GetTrigger( device );
		pltriggerE[0] = (pltrigger[0]) ^ (pltriggerE[0]);
		pltriggerE[0] = (pltrigger[0]) & (pltriggerE[0]);

		check_exit(pltrigger[0]);

		for(unsigned int i=0;i<12;i++)
		{
			if((pltrigger[0] & pad_asign[i])!=0)
			{
				switch(pltrigger[0] & pad_asign[i] )
				{
					case PER_DGT_U: FBA_KEYPAD[0] |= 0x0001; break;
					case PER_DGT_D: FBA_KEYPAD[0] |= 0x0002; break;
					case PER_DGT_L: FBA_KEYPAD[0] |= 0x0004; break;
					case PER_DGT_R: FBA_KEYPAD[0] |= 0x0008; break;
					case PER_DGT_B: FBA_KEYPAD[0] |= 0x0040; break;
					case PER_DGT_A: FBA_KEYPAD[0] |= 0x0080; break;
					case PER_DGT_C: FBA_KEYPAD[0] |= 0x0100; break;
					case PER_DGT_X: FBA_KEYPAD[0] |= 0x0200; break;
					case PER_DGT_Y: FBA_KEYPAD[0] |= 0x0400; break;
					case PER_DGT_Z: FBA_KEYPAD[0] |= 0x0800; break;
					case PER_DGT_TL: FBA_KEYPAD[0] |= 0x0010; break;
					case PER_DGT_TR: FBA_KEYPAD[0] |= 0x0020; break;
					case PER_DGT_S: FBA_KEYPAD[0] |= P1P2Start = 1; break;
				    default:
					break;
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void run_fba_emulator()
{
	nBurnSoundRate = SOUNDRATE;
	ChangeDir("GAMES");

	if (nDrvInit(nBurnDrvSelect) != 0) 
	{
		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"Driver initialisation failed! Likely causes are:",1,180);
		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"- Corrupt/Missing ROM(s)\n- I/O Error\n- Memory error\n\n",1,190);
//		while(1);
	}
	else
	{
		FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"                    ",24,40);
		InpInit();
		InpDIP();
		play = 1;
		drvquit = 0;
	//	PCM_Start(pcm);
		PCM_MeSetVolume(pcm,255);
		PCM_DrvChangePcmPara(pcm,-1,-1);
	//	PER_SMPC_SND_ON();
		SetVblank(); // a garder

		int (*Frame)();
		Frame = (int *)pDriver[nBurnDrvSelect]->Frame;

		if(lp.h_enbl)
		{
			while (play)
			{
				Frame();
	 
				SCL_SetLineParamNBG0(&lp);
				_spr2_transfercommand();
				frame_x++;

				 if(frame_x>=frame_y)
					wait_vblank();
			}
		}
		else
		{
			while (play)
			{
				Frame();
				SclProcess = 2; 
				_spr2_transfercommand();
				frame_x++;

				 if(frame_x>=frame_y)
					wait_vblank();
			}
		}
	}
	if(drvquit==1)
	{
		InpExit();
		if (nBurnDrvSelect < nBurnDrvCount) 
			BurnDrvExit();
		initSaturn();
		BurnDrvAssignList();
	}
	asm("nop\n");
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initSprites(int sx,int sy,int sx2, int sy2,int lx,int ly)
{
	_spr2_initialize();

	SPR_WRITE_REG(SPR_W_TVMR, 0x0007 & SPR_TV_NORMAL);//SPR_TV_ROT8);//SPR_TV_NORMAL);
	SPR_SetEraseData( 0x0000, 0, 0, sx, sy );

	memset(smsSprite,0,sizeof(SprSpCmd)*259);
    smsSprite[0].control    = (JUMP_NEXT | FUNC_SCLIP);

    smsSprite[0].ax         = sx2;
    smsSprite[0].ay         = sy2;

	smsSprite[0].cx         = sx;
    smsSprite[0].cy         = sy;

    smsSprite[1].control    =  (JUMP_NEXT | FUNC_LCOORD);

    smsSprite[1].ax         = lx;
    smsSprite[1].ay         = ly;

    smsSprite[2].control    = (JUMP_NEXT | FUNC_UCLIP);

    smsSprite[2].ax         = sx2;
    smsSprite[2].ay         = sy2;

	smsSprite[2].cx         = sx;
	smsSprite[2].cy         = sy;

	smsSprite[3].control    = CTRL_END;

	_spr2_transfercommand();
}
//-------------------------------------------------------------------------------------------------------------------------------------
void drawWindow(unsigned  int l1,unsigned  int l2,unsigned  int l3,unsigned  int vertleft,unsigned  int vertright)
{
	Uint16 *VRAM;    
	Uint32 x,j;  
	col[0]=10;
    VRAM = (Uint16 *)SS_FONT;
// barre horizontale haut
	for( x = 0; x < l1; x++ ) // 2 lignes
    {
		for( j = 0; j < 64; j++ ) *VRAM++ = 0xaaaa;
	}

	for( x = 0; x < l2; x++ ) 
    {
		for( j = 0; j < vertleft			  ; j++ ) *VRAM++ = 0xaaaa; // barre verticale gauche
		for( j = 0; j < 128-vertleft-vertright; j++ ) *VRAM++ = 0x0000; // noir
		for( j = 0; j < vertright			  ; j++ ) *VRAM++ = 0xaaaa; // barre verticale droite
	}
// barre horizontale bas
	for( x = 0; x < l3; x++ ) 
    {
		for( j = 0; j < 64; j++ ) *VRAM++ = 0xaaaa;
	}
	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Loading. Please Wait",24,40);
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"Loading. Please Wait...",20,40);
	
		play=0;
		SclProcess = 1;
		SetVblank();
		wait_vblank();
		play=1;
}
//-------------------------------------------------------------------------------------------------------------------------------------
#ifdef HEAP_WALK
extern UINT32  end;
extern UINT32  __malloc_free_list;
extern UINT32  _sbrk(int size);

void heapWalk(void)
{
    UINT32 chunkNumber = 1;
    // The __end__ linker symbol points to the beginning of the heap.
    UINT32 chunkCurr = (UINT32)&end;
    // __malloc_free_list is the head pointer to newlib-nano's link list of free chunks.
    UINT32 freeCurr = __malloc_free_list;
    // Calling _sbrk() with 0 reserves no more memory but it returns the current top of heap.
    UINT32 heapEnd = _sbrk(0);
    
//    printf("Heap Size: %lu\n", heapEnd - chunkCurr);
    char toto[200];
//	sprintf (toto,"Heap Size: %d  e%08x s%08x                     \n", heapEnd - chunkCurr,heapEnd, chunkCurr) ;
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)toto,12,216);

    // Walk through the chunks until we hit the end of the heap.
    while (chunkCurr < heapEnd)
    {
        // Assume the chunk is in use.  Will update later.
        int      isChunkFree = 0;
        // The first 32-bit word in a chunk is the size of the allocation.  newlib-nano over allocates by 8 bytes.
        // 4 bytes for this 32-bit chunk size and another 4 bytes to allow for 8 byte-alignment of returned pointer.
        UINT32 chunkSize = *(UINT32*)chunkCurr;
        // The start of the next chunk is right after the end of this one.
        UINT32 chunkNext = chunkCurr + chunkSize;
        
        // The free list is sorted by address.
        // Check to see if we have found the next free chunk in the heap.
        if (chunkCurr == freeCurr)
        {
            // Chunk is free so flag it as such.
            isChunkFree = 1;
            // The second 32-bit word in a free chunk is a pointer to the next free chunk (again sorted by address).
            freeCurr = *(UINT32*)(freeCurr + 4);
        }
        
        // Skip past the 32-bit size field in the chunk header.
        chunkCurr += 4;
        // 8-byte align the data pointer.
        chunkCurr = (chunkCurr + 7) & ~7;
        // newlib-nano over allocates by 8 bytes, 4 bytes for the 32-bit chunk size and another 4 bytes to allow for 8
        // byte-alignment of the returned pointer.
        chunkSize -= 8;
//        printf("Chunk: %lu  Address: 0xlX  Size: %lu  %s\n", chunkNumber, chunkCurr, chunkSize, isChunkFree ? "CHUNK FREE" : "");
        
	sprintf (toto,"%d A%04x  S%04d %s", chunkNumber, chunkCurr, chunkSize, isChunkFree ? "CHUNK FREE" : "") ;
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)toto,2,190+chunkNumber*9);
		
		chunkCurr = chunkNext;
        chunkNumber++;
    }
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"done",12,131);
}
#endif
