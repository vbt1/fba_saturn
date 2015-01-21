#include    "machine.h"
#include "d_sms.h"
#define OLD_SOUND 1
#define SAMPLE 7680L
//GfsDirName dir_name_sms[512];

int ovlInit(char *szShortName)
{
//#define M_TRIM_THRESHOLD    -1
//#define M_TOP_PAD           -2

//	mallopt(M_TOP_PAD, 0);
//	mallopt(M_TRIM_THRESHOLD, 8);

//	struct BurnDriver *fba_drv = NULL;
	struct BurnDriver nBurnDrvsms_akmw = {
		"sms", NULL,
		"Sega Master System\0",
		sms_akmwRomInfo, sms_akmwRomName, SMSInputInfo, SMSDIPInfo,
//		sms_akmwRomInfo, sms_akmwRomName, NULL, SMSDIPInfo,
		SMSInit, SMSExit, SMSFrame, NULL
	};

	/*struct BurnDriver * */ //fba_drv = 	(struct BurnDriver *)FBA_DRV;
	memcpy(shared,&nBurnDrvsms_akmw,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
//	ss_regs  = (SclSysreg *)SS_REGS;
//	slob_init();
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void	SetVblank2( void ){
	int			imask;


	imask = get_imask();
	set_imask(2);
//	INT_ChgMsk(INT_MSK_NULL,INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);
	INT_ChgMsk(INT_MSK_NULL, INT_MSK_VBLK_OUT);
//	INT_SetScuFunc(INT_SCU_VBLK_IN,UsrVblankIn2);
	INT_SetScuFunc(INT_SCU_VBLK_OUT,update_input1);
//	INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT,INT_MSK_NULL);
	INT_ChgMsk(INT_MSK_VBLK_OUT,INT_MSK_NULL);
	set_imask(imask);
	__port = PER_OpenPort();
	
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr		= (Uint16*)SCL_AllocColRam(SCL_NBG0,OFF);
	colAddr			= (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	(Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
//	SCL_SetColRam(SCL_NBG1,0,8,palette);
	SCL_SetColRam(SCL_NBG1,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
#ifndef OLD_SOUND
static void sh2slave(unsigned int *nSoundBufferPos)
{
	volatile signed short *nSoundBuffer = (signed short *)0x25a20000;
//	PSG_Update(0,&nSoundBuffer[nSoundBufferPos[0]],  128);
	PSG_Update(&nSoundBuffer[nSoundBufferPos[0]],  128);
//	SN76496Update(0,&nSoundBuffer[nSoundBufferPos[0]],  128);
	nSoundBufferPos[0]+=128;//256; 
	if(nSoundBufferPos[0]>=SAMPLE)//<<1)//256*hz)
		nSoundBufferPos[0]=0;
	PCM_Task(pcm);

//	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = deltaSlave;
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
static void initLayers(void)
{
//    SclConfig	config;
// **29/01/2007 : VBT sauvegarde cycle patter qui fonctionne jusqu'à maintenant

	Uint16	CycleTb[]={
		  // VBT 04/02/2007 : cycle pattern qui fonctionne just test avec des ee
		0xff5e, 0xffff, //A1
		0xffff, 0xffff,	//A0
		0x04ee, 0xffff,   //B1
		0xffff, 0xffff  //B0
	};
 	SclConfig	scfg;

//	SCL_InitConfigTb(&scfg);
	scfg.dispenbl		= ON;
	scfg.charsize		= SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize	= SCL_PN1WORD;
	scfg.flip				= SCL_PN_10BIT;
	scfg.platesize	= SCL_PL_SIZE_1X1;
	scfg.coltype		= SCL_COL_TYPE_16;
	scfg.datatype	= SCL_CELL;
	scfg.patnamecontrl =  0x000c;// VRAM B1 ‚ÌƒIƒtƒZƒbƒg 
	scfg.plate_addr[0] = ss_map;
	SCL_SetConfig(SCL_NBG0, &scfg);
/********************************************/	

//	SCL_InitConfigTb(&scfg);
//	scfg.dispenbl 	 = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
//	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 	 = SCL_BITMAP;
	scfg.mapover       = SCL_OVER_0;
	scfg.plate_addr[0] = ss_font;
	SCL_SetConfig(SCL_NBG1, &scfg);
	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initPosition(void)
{
	SCL_Open();
	ss_reg->n1_move_x = 0;
	ss_reg->n1_move_y = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void SaturnInitMem()
{
	UINT8 *Next; Next = (UINT8 *)SaturnMem;
	name_lut		= Next; Next += 0x10000*sizeof(UINT16);
	bp_lut			= Next; Next += 0x10000*sizeof(UINT32);
	cram_lut		= Next; Next += 0x40*sizeof(UINT16);
	map_lut	 		= Next; Next += 0x800*sizeof(UINT16);
	dummy_write= Next; Next += 0x100*sizeof(UINT8);
	MemEnd			= Next;	
	 
/*	name_lut	= (UINT16 *)malloc(0x10000*sizeof(UINT16)); 
	cram_lut	= (UINT16 *)malloc(0x40*sizeof(UINT16));
	dummy_write = (unsigned *)malloc(0x100*sizeof(unsigned));	 */
}
//-------------------------------------------------------------------------------------------------------------------------------------
#ifndef OLD_SOUND
void dummy()
{

}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
static void DrvInitSaturn()
{
//	InitCDsms();
#ifndef OLD_SOUND
	SPR_InitSlaveSH();
	SPR_RunSlaveSH((PARA_RTN*)dummy, NULL);
#endif
	nBurnSprites  = 67;//131;//27;
	nBurnLinescrollSize = 0x340;
	nSoundBufferPos = 0;//sound position à renommer

	SS_CACHE = cache      =(Uint8  *)SCL_VDP2_VRAM_B1;
	SS_MAP     = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_FONT   = ss_font    =(Uint16 *)SCL_VDP2_VRAM_A1;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_scl			= (Fixed32 *)SS_SCL;

	file_id			= 2; // bubble bobble
//	file_max		= getNbFiles();
		//8;//aleste
//	name_lut	= (UINT16 *)malloc(0x10000*sizeof(UINT16));
//	bp_lut		= (UINT32 *)malloc(0x10000*sizeof(UINT32));
	
	SaturnInitMem();
	int nLen = MemEnd - (UINT8 *)0;
//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"malloc 1 bef       ",12,201);
	SaturnMem = (UINT8 *)malloc(nLen);
//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"malloc 1 aft    ",12,201);

	SaturnInitMem();
//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"malloc 3 aft       ",12,201);

	make_lut();
	
    SS_SET_N0PRIN(5);
    SS_SET_N1PRIN(7);
    SS_SET_S0PRIN(6);

	initLayers();
	initColors();
	initPosition();
//	initSprites(256+48-1,192+16-1,256-1,192-1,48,16);

	initSprites(256-1,192-1,0,0,0,0);
	
	 initScrolling(ON,SCL_VDP2_VRAM_B0+0x4000);
//	drawWindow(32,192,192,14,52);
	nBurnFunction = update_input1;
	drawWindow(0,192,192,2,66);
	SetVblank2();

//	extern int __malloc_trim_threshold;
//	extern int __malloc_top_pad;
//	extern mallinfo  __malloc_current_mallinfo;

//	__malloc_trim_threshold = 1024;

//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"A:Help",12,201);
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"C:Credits",127,201);
//			sauvegarder __malloc_sbrk_base puis restaurer à la fin
//   Uint32 __malloc_sbrk_base;
//__malloc_sbrk_base = &_bend;
/*	Uint8	*dst;
	for (dst = (Uint8 *)&_bend; dst < (Uint8 *)0x060A5000; dst++)
		*dst = 0;  */
 /*   for (dst = (Uint8 *)&__malloc_sbrk_base
*/
/*		Uint8	*dst;
    for (dst = (Uint8 *)&bss_start; dst < (Uint8 *)&bss_end; dst++)
			 *dst = 0;  
  */
/*	char toto[50];
	extern Uint32 __malloc_sbrk_base, _bend,_bstart,bss_start,bss_end;
	extern int __malloc_trim_threshold;
	extern int __malloc_top_pad;

	sprintf (toto,"malloc_top_pad %08x %08x",__malloc_top_pad,__malloc_sbrk_base) ;
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)toto,12,201);

	sprintf (toto,"malloc_trim_threshold %08x %08x",__malloc_trim_threshold,sbrk(0)) ;
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)toto,12,211);	   */
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void sms_start()
{
//	fba_drv->Frame = NULL;
//	z80_stop_emulating();
	nSoundBufferPos=0;
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	
	for (int i=0;i<256 ; ++i ) colAddr[i]=colBgAddr[i]=RGB( 0, 0, 0 );//palette2[0];
	*(UINT16 *)0x25E00000=RGB( 0, 0, 0 );//palette2[0];
	
	memset(&ss_vram[0x1100],0,0x10000-0x1100);
	memset((Uint8 *)cache,0x0,0x20000);
	memset((Uint8 *)ss_map,0,0x20000);
	memset((Uint8 *)SCL_VDP2_VRAM_A0,0,0x20000);

	scroll_x= scroll_y = 0;
 	SCL_Open();
    for(int i = 0; i < 0xC0; i++) ss_scl[i]= 0;

	ss_reg->n0_move_y =  scroll_y;
	sms_reset();
	load_rom();
  	system_init();
	running = 1;
//	fba_drv->Frame = SMSFrame;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 SMSInit(void)
{
	DrvInitSaturn();
	sms_start();

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 SMSExit(void)
{
	z80_stop_emulating();
	cart.rom = NULL;
	__port = NULL;
	nBurnFunction = NULL;

	dummy_write = MemEnd = NULL;
	cram_lut = map_lut = NULL;

	bp_lut = NULL;
	name_lut = NULL;

	free(SaturnMem);
	SaturnMem = NULL;

//	free(bp_lut);
//	bp_lut = NULL;

//	free(name_lut);
//	name_lut = NULL;

	nSoundBufferPos=0;
	running=0;
	first = 1;
	vsynch = 0;
	scroll_x=0;
	scroll_y=0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 SMSFrame(void)
{
	if(running)
	{
	*(Uint16 *)0x25E00000 = colBgAddr[0]; // set bg_color
//	*(UINT16 *)0x25E00000=RGB( 0, 0, 0 );//palette2[0];

		sms_frame();
		
#ifdef OLD_SOUND //
//		if(sound)
		{
			signed short *nSoundBuffer = (signed short *)0x25a20000;
//			PSG_Update(0,&nSoundBuffer[nSoundBufferPos],  128);
			PSG_Update(&nSoundBuffer[nSoundBufferPos],  128);
//			SN76496Update(0,&nSoundBuffer[nSoundBufferPos],  128);
			nSoundBufferPos+=128;//256; // DOIT etre deux fois la taille copiee

			if(nSoundBufferPos>=SAMPLE)//256*hz)
				nSoundBufferPos=0;
			PCM_Task(pcm);
		}
 #else
//		if(sound)
		{
//			if((*(unsigned char *)0xfffffe11 & 0x80) == 0)
			SPR_WaitEndSlaveSH();
			SPR_RunSlaveSH((PARA_RTN*)sh2slave, &nSoundBufferPos);
//			vb=1;
		}		
#endif
		ss_reg->n0_move_y =  scroll_y; //-(16<<16) ;
	}
	else
	{
		sms_start();
	}
//__port = PER_OpenPort();
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void sms_frame(void)
{
#if PROFILING
//		TIM_FRT_SET_16(0);
		line = bcountdiff = bcount = bcNum = 0;
#endif

	if(sms.paused && first==2)
	{
			z80_cause_NMI();
			first = 0;
	}
	vdp.line = 0;
	vdp.left = vdp.reg[10];
//    for(vdp.line = 0; vdp.line < 262; vdp.line++)
    for(; vdp.line <= 0xc0; ++vdp.line)
	{
		z80_emulate(228);
		vdp_run(&vdp);
		++vdp.line;
		z80_emulate(228);
		vdp_run(&vdp);
		++vdp.line;
		z80_emulate(228);
		vdp_run(&vdp);
	}

    for(; vdp.line < 0xE0; ++vdp.line)
	{
		z80_emulate(228);
       vdp.left = vdp.reg[10];

        if((vdp.status & 0x80) && (vdp.reg[1] & 0x20))
        {
            sms.irq = 1;
            z80_raise_IRQ(0);
        }
	}

    for(; vdp.line < 262; ++vdp.line)
	{
		z80_emulate(228);
       vdp.left = vdp.reg[10];
		++vdp.line;
		z80_emulate(228);
       vdp.left = vdp.reg[10];
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void load_rom(void)
{
	long fileSize;

	fileSize		 = GetFileSize(file_id);
	cart.rom	 = (UINT8 *) 0x00200000;
//	memset4_fast((Uint8 *)&cart.rom[0],0,0x100000);
	cart.pages	 = fileSize /0x4000;
	GFS_Load(file_id, 0, cart.rom, fileSize);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void system_init()
{
    /* Initialize the VDP emulation */
    vdp_reset();

    /* Initialize the SMS emulation */
    sms_init();

//SN76496_init(MASTER_CLOCK, 255, 44100); //VBT sample rate
//SN76496_init(MASTER_CLOCK, 255, 30720);
//SN76489Init(0,MASTER_CLOCK, 0);
//PSG_Init(0,MASTER_CLOCK, 7680);
	PSG_Init(MASTER_CLOCK, 7680);

//30720
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void sms_init(void)
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
static void vdp_reset(void)
{
    memset(&vdp, 0, sizeof(t_vdp));
}
//-------------------------------------------------------------------------------------------------------------------------------------
/* Write data to the VDP's control port */
static void vdp_ctrl_w(int data)
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
					scroll_x =  ((vdp.reg[r]) ^ 0xff) ;
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
static int vdp_ctrl_r(void)
{
    /* Save the status flags */
    UINT8 temp = vdp.status;

    /* Clear pending flag */
    vdp.pending = 0;
	vdp.status = 0;
	z80_lower_IRQ();
    /* Return the old status flags */
    return (temp);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void update_bg(t_vdp *vdp, int index)
{
//				if(index>=vdp.ntab && index<vdp.ntab+0x700)
// VBT 04/02/2007 : modif compilo
		if(index>=vdp->ntab)
			if( index<vdp->ntab+0x700)
		{
			UINT16 temp = *(UINT16 *)&vdp->vram[index&~1];
			int delta = map_lut[index - vdp->ntab];
			UINT16 *map = &ss_map[delta]; 
			map[0] =map[32] =map[0x700] =map[0x720] =name_lut[temp];
		}
		/* Mark patterns as dirty */
		UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
		UINT32 bp = *(UINT32 *)&vdp->vram[index & ~3];
		UINT32 *pg = (UINT32 *) &cache[0x00000 | (index & ~3)];
		UINT32 *sg = (UINT32 *)&ss_vram[0x1100 + (index & ~3)];
		UINT32 temp1 = bp_lut[bp & 0xFFFF];
		UINT32 temp2 = bp_lut[(bp>>16) & 0xFFFF];
		*sg= *pg = (temp1<<2 | temp2 );
//        *pg = (temp1<<2 | temp2 );
}
//-------------------------------------------------------------------------------------------------------------------------------------
/* Write data to the VDP's data port */
static void vdp_data_w(int data)
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
				ss_spritePtr = &ss_sprite[3];

				// Sprite dimensions 
				int height = (vdp.reg[1] & 0x02) ? 16 : 8;

				delta=(index-vdp.satb);
			// Pointer to sprite attribute table 
				UINT8 *st = (UINT8 *)&vdp.vram[vdp.satb];
				// Sprite Y position 
				int yp = st[delta];

				if(yp == 208) 
				{
					
					ss_spritePtr[delta].control = CTRL_END;
					ss_spritePtr[delta].drawMode = 0;
					ss_spritePtr[delta].charAddr	= 0;
					ss_spritePtr[delta].charSize		= 0;
					ss_spritePtr[delta].ax	= 0;
					ss_spritePtr[delta].ay	= 0;
//					nbSprites = delta+5;
//ajouter un flag
					break;
				}
				//Actual Y position is +1 
				yp ++;
				//Wrap Y coordinate for sprites > 240 
				if(yp > 240) yp -= 256;
				ss_spritePtr[delta].ay = yp;

				// Clip sprites on left edge 
				ss_spritePtr[delta].control = ( JUMP_NEXT | FUNC_NORMALSP);
				ss_spritePtr[delta].drawMode   = ( COLOR_0 | ECD_DISABLE | COMPO_REP);		
				ss_spritePtr[delta].charSize   = 0x100+ height;  //0x100
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
//-------------------------------------------------------------------------------------------------------------------------------------
/* Read data from the VDP's data port */
static int vdp_data_r(void)
{
    UINT8 temp = 0;
    vdp.pending = 0;
    temp = vdp.buffer;
//    vdp.buffer = vdp.vram[(vdp.addr & 0x3FFF)^1];
    vdp.buffer = vdp.vram[(vdp.addr & 0x3FFF)];
    vdp.addr = (vdp.addr + 1) & 0x3FFF;
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


static void vdp_run(t_vdp *vdp)
{
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
	/*	else
		{
			if(vdp->line == 0)
			{
				vdp->left = vdp->reg[10];
			} 
		}	  */

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
            z80_raise_IRQ(0);
        }
    }
/*    else
    {
        vdp->left = vdp->reg[10];

        if((vdp->line < 0xE0) && (vdp->status & 0x80) && (vdp->reg[1] & 0x20))
        {
            sms.irq = 1;
            z80_raise_IRQ(0);
        }
    }	*/
}
//-------------------------------------------------------------------------------------------------------------------------------------
static UINT8 vdp_vcounter_r(void)
{
    return (vcnt[(vdp.line & 0x1FF)]);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static UINT8 vdp_hcounter_r(void)
{
//    int pixel = (((z80_ICount % CYCLES_PER_LINE) / 4) * 3) * 2;
//  int pixel = (((Cz80_struc.CycleIO % CYCLES_PER_LINE) / 4) * 3) * 2;
  int pixel;
  if (hz==60)
	pixel = (((z80_get_cycles_elapsed() % CYCLES_PER_LINE_60) >>2) * 3) <<1;
  else
	pixel = (((z80_get_cycles_elapsed() % CYCLES_PER_LINE_50) >>2) * 3) <<1;
//	printf("Cz80_struc.CycleIO %d\n pixel %d\n",Cz80_struc.CycleIO,pixel);
    return (hcnt[((pixel >> 1) & 0x1FF)]);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void cz80_z80_writeport16(unsigned short PortNo, unsigned char data)
{
    switch(PortNo & 0xFF)
    {
        case 0x01: /* GG SIO */
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06: /* GG STEREO */
        case 0x7E: /* SN76489 PSG */
        case 0x7F:
//#ifdef SOUND
//			if (sound)
//				SN76496Write(0,data);
//				PSG_Write(0,data);
				PSG_Write(data);
//#endif
           break;

        case 0xBE: /* VDP DATA */
            vdp_data_w(data);
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
static UINT8 update_input1(void)
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
//			if((pltriggerE[0] & pad_asign[i])!=0)
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
//			if((pltrigger[0] & pad_asign[i])!=0)
			if((pltriggerE[0] & pad_asign[i])!=0)
			{
				switch(pltriggerE[0] & pad_asign[i] )
				{
					case PER_DGT_C:
						if (!sms.paused)	{ sms.paused = 1; first = 2;}
						else{ sms.paused = 0;}
						temp = 0xFF;
					break;
					case PER_DGT_X:


#ifdef PROFILING
	for (k=0;k<21 ;k++)
	{
		char toto2[50];
		sprintf(toto2,"%08d", bcount2[k]);
		FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)toto2,0,k*10);
	}

#else
  /*
#ifdef SOUND
						if(sound==1)
						{
							sound=0;
//							SYS_Exit(0);
							PCM_MeStop(pcm);
//							pcm_PauseOn(pcm);
						}
						else
						{
							sound=1;
//							pcm_PauseOn(pcm);
							PCM_MeSetLoop(pcm, SAMPLE);
							PCM_MeStart(pcm);
						}
#endif
*/
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

					 char str[50];

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
static UINT8 update_input2(void)
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
static void sms_reset(void)
{
	z80_reset();
    /* Clear SMS context */
    memset4_fast(dummy_write, 0, 0x100);
    memset4_fast(sms.ram, 0, 0x2000);
    memset4_fast(sms.sram, 0, 0x8000);
    sms.port_3F = sms.port_F2 = sms.irq = 0x00;
//    sms.psg_mask = 0xFF;


    /* Load memory maps with default values */
	z80_map_read (0x0000, 0x3FFF, &cart.rom[0]); 

	z80_map_read (0x4000, 0x7FFF, &cart.rom[0x4000]); 
	z80_map_read (0x8000, 0xBFFF, &cart.rom[0x8000]); 
	z80_map_write(0x0000, 0xBFFF, (unsigned char*)dummy_write);

	z80_map_read (0xC000, 0xDFFF, (unsigned char *)(&sms.ram[0])); 
	z80_map_write (0xC000, 0xDFFF, (unsigned char *)(&sms.ram[0])); 

	z80_map_read (0xE000, 0xFFFF, (unsigned char *)(&sms.ram[0])); 
	z80_map_write (0xE000, 0xFFFF, (unsigned char *)(&sms.ram[0])); 

#define Z80_MAP_HANDLED 1
//	z80_add_write(0xFFFC, 0xFFFF, Z80_MAP_HANDLED, (void *)&cpu_writemem8);
//	z80_add_read(0x0000, 0xFFFF, Z80_MAP_HANDLED, (void *)&cpu_readmem8);
	z80_add_write(0xFFFC, 0xFFFF, Z80_MAP_HANDLED, (void *)&cpu_writemem8);

	sms.fcr[0] = 0x00;
    sms.fcr[1] = 0x00;
    sms.fcr[2] = 0x01;
    sms.fcr[3] = 0x00;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void cpu_writemem8(unsigned int address, unsigned int data)
{
		sms.ram[address & 0x1FFF] = data;
// data & cart.pages, and set cart.pages to one less than you are
        UINT32 offset = (data % cart.pages) << 14; // VBT à corriger
// vbt 15/05/2008 : exophase :	 data & cart.pages, and set cart.pages to one less than you are
		sms.fcr[address& 3] = data;

        switch(address & 3)
        {
			case 0:

                if(data & 8)
                {
					offset = (data & 0x4) ? 0x4000 : 0x0000;

					z80_map_fetch(0x8000, 0xBFFF, (unsigned char *)(sms.sram + offset));
					z80_map_read(0x8000, 0xBFFF, (unsigned char *)(sms.sram + offset));
					z80_map_write(0x8000, 0xBFFF, (unsigned char *)(sms.sram + offset));
                }
                else
                {
					offset = ((sms.fcr[3] % cart.pages) << 14);
// vbt 15/05/2008 : exophase :	 data & cart.pages, and set cart.pages to one less than you are
					z80_map_fetch(0x8000, 0xBFFF, (unsigned char *)(cart.rom + offset));
					z80_map_read(0x8000, 0xBFFF, (unsigned char *)(cart.rom + offset));
					z80_map_write(0x8000, 0xBFFF, (unsigned char *)(dummy_write));
                }
				break;
            case 1:
				z80_map_fetch(0x0000, 0x3FFF, (unsigned char *)(cart.rom + offset));
				z80_map_read(0x0000, 0x3FFF, (unsigned char *)(cart.rom + offset));
			break;

            case 2:
				z80_map_fetch(0x4000, 0x7FFF, (unsigned char *)(cart.rom + offset));
				z80_map_read(0x4000, 0x7FFF, (unsigned char *)(cart.rom + offset));
            break;

            case 3:

			if(!(sms.fcr[0] & 0x08))
            {
					z80_map_fetch(0x8000, 0xBFFF, (unsigned char *)(cart.rom + offset));
					z80_map_read(0x8000, 0xBFFF, (unsigned char *)(cart.rom + offset));
            }
            break;
        }
    return;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static unsigned char cz80_z80_readport16(unsigned short PortNo)
{
    switch(PortNo & 0xFF)
    {
        case 0x7E: /* V COUNTER */
            return (vdp_vcounter_r());
            break;
    
        case 0x7F: /* H COUNTER */
		//	printf("vdp_hcounter_r\n");
            return (vdp_hcounter_r());
            break;
    
        case 0x00: /* INPUT #2 */
			return 0xff;
//            temp = 0xFF;
//			return (update_system());
//            if(input.system & INPUT_START) temp &= ~0x80;
//            if(sms.country == TYPE_DOMESTIC) temp &= ~0x40;
//            return (temp);
    
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
            return (vdp_data_r());
    
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
static void z80_init(void)
{
	z80_init_memmap();
//	z80_map_fetch(0x0000, 0xBFFF, (unsigned char *)&cart.rom);
//	z80_map_fetch(0xC000, 0xFFFF, (unsigned char *)&sms.ram);
#define Z80_MAP_HANDLED 1
#define Z80_MAP_DIRECT  0  /* Reads/writes are done directly */
//z80_add_read(0x0000, 0xFFFF, Z80_MAP_HANDLED, &cpu_readmem8); 

/* Bank #0 */ 
z80_map_fetch (0x0000, 0x3FFF, (unsigned char *)(&cart.rom[0])); 
z80_map_read (0x0000, 0x3FFF, (unsigned char *)(&cart.rom[0])); 

/* Bank #1 */ 
z80_map_fetch (0x4000, 0x7FFF, (unsigned char *)(&cart.rom[0x4000])); 
z80_map_read (0x4000, 0x7FFF, (unsigned char *)(&cart.rom[0x4000])); 

/* Bank #2 */ 
z80_map_fetch (0x8000, 0xBFFF, (unsigned char *)(&cart.rom[0x8000])); 
z80_map_read (0x8000, 0xBFFF, (unsigned char *)(&cart.rom[0x8000])); 

/* RAM */ 
z80_map_fetch (0xC000, 0xDFFF, (unsigned char *)(&sms.ram[0])); 
z80_map_read (0xC000, 0xDFFF, (unsigned char *)(&sms.ram[0])); 
z80_map_write (0xC000, 0xDFFF, (unsigned char *)(&sms.ram[0])); 

/* RAM (mirror) */ 
z80_map_fetch (0xE000, 0xFFFF, (unsigned char *)(&sms.ram[0])); 
z80_map_read (0xE000, 0xFFFF, (unsigned char *)(&sms.ram[0])); 
z80_map_write (0xE000, 0xFFFF, (unsigned char *)(&sms.ram[0])); 

//z80_add_write(0xC000, 0xFFFF, Z80_MAP_DIRECT, (void *)(&sms.ram[0]));
//z80_add_write(0xFFFC, 0xFFFF, Z80_MAP_HANDLED, (void *)&cpu_writemem8);
z80_add_write(0x0000, 0xFFFF, Z80_MAP_HANDLED, (void *)&cpu_writemem8);

	z80_end_memmap();     

	z80_set_in((unsigned char (*)(short unsigned int))&cz80_z80_readport16);
	z80_set_out((void (*)(short unsigned int, unsigned char))&cz80_z80_writeport16);
//	z80_set_fetch_callback(&debug);

	z80_reset();
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void make_map_lut()
{
	unsigned int row,column;

	for (int i = 0; i < 0x800;i++) 
	{
		row = i & 0x7C0;
		column = (i>>1) & 0x1F;
		map_lut[i] = row+column;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void make_name_lut()
{
	unsigned int i, j;
	for(j = 0; j < 0x10000; j++)
	{
		i = ((j >> 8) & 0xFF) | ((j  & 0xFF) <<8);
		unsigned int name = (i & 0x1FF);
		unsigned int flip = (i >> 9) & 3;
		unsigned int pal = (i >> 11) & 1;
		name_lut[j] = (pal << 12 | flip << 10 | name);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void make_bp_lut(void)
{
//	bp_lut = (UINT32 *)malloc(0x10000*sizeof(UINT32));
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
static void make_cram_lut(void)
{
//	cram_lut = (UINT16 *)malloc(0x40*sizeof(UINT16));

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
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void make_lut()
{
	make_name_lut();
	make_bp_lut();
	make_cram_lut();
	make_map_lut();
}
//-------------------------------------------------------------------------------------------------------------------------------------
