// FB Alpha Sega SG-1000-based arcade driver module
// Based on MAME driver by Tomasz Slanina
// Code by iq_132, fixups & bring up-to-date by dink Aug 18, 2014

#include    "machine.h"
#include "d_sg1000.h"
#include "raze\raze.h"
#define SAMPLE 7680L
#define nBurnSoundLen 128
//UINT8 *pTransDraw = NULL;
#define RAZE 1

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvsg1k_wboy = {
		"sg1000", NULL, 
		"Sega SG-1000",
		sg1k_wboyRomInfo, sg1k_wboyRomName, Sg1000InputInfo, Sg1000DIPInfo,
		DrvInit, DrvExit, DrvFrame, NULL, 
	};

	/*struct BurnDriver * */ //fba_drv = 	(struct BurnDriver *)FBA_DRV;
	memcpy(shared,&nBurnDrvsg1k_wboy,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
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
static void set_memory_map(int mapper)
{

#ifdef RAZE
	z80_init_memmap();

	z80_map_fetch (0x0000, 0xbfff, DrvZ80ROM);
	if(mapper == MAPPER_RAM_8K_EXT1)
	{
		z80_map_fetch (0x2000, 0x3fff, DrvZ80ExtRAM);
	}
	else
	{
//		z80_map_read (0xc000, 0xffff, DrvZ80RAM  );
//		z80_map_write(0xc000, 0xffff, DrvZ80RAM  ); 
		z80_add_read(0xc000, 0xffff, 0, &DrvZ80RAM[0]);
		z80_add_write(0xc000, 0xffff, 0, &DrvZ80RAM[0]);
	}

	z80_map_fetch (0xc000, 0xffff,  DrvZ80RAM);

	z80_end_memmap(); 

	z80_set_in((unsigned char (*)(unsigned short))&sg1000_read_port);
	z80_set_out((void (*)(unsigned short, unsigned char))&sg1000_write_port);

//	z80_add_read(0x0000, 0xbfff, 1, (void *)&sg1000_read_0000);
	z80_add_write(0x0000, 0xbfff, 1, (void *)&sg1000_write);

	z80_add_read(0x0000, 0xbfff, 0, &DrvZ80ROM[0]);
//	z80_add_write(0x0000, 0xbfff, 0, (void *)DrvZ80ROM);

	if(mapper == MAPPER_RAM_8K_EXT1)
	{
		z80_add_read(0xc000, 0xffff, 1, (void *)&sg1000_read_c000);
		z80_add_write(0xc000, 0xffff, 1, (void *)&sg1000_write_c000);
	}

	if(mapper == MAPPER_RAM_8K_EXT1 || mapper == MAPPER_RAM_8K_EXT2)
	{
		z80_add_read(0x2000, 0x3fff, 1, (void *)&sg1000_read_2000);
		z80_add_write(0x2000, 0x3fff, 1, (void *)&sg1000_write_2000);
	}
#else
	CZetInit(1);
	CZetOpen(0);
	CZetSetOutHandler(sg1000_write_port);
	CZetSetInHandler(sg1000_read_port);
	CZetSetWriteHandler(sg1000_write);
	CZetSetReadHandler(sg1000_read);

	CZetMapArea(0x0000, 0xbfff, 0, DrvZ80ROM);
//	CZetMapArea(0x0000, 0xbfff, 1, DrvZ80ROM);
	CZetMapArea(0x0000, 0xbfff, 2, DrvZ80ROM);

	CZetMapArea(0xc000, 0xc3ff, 0, DrvZ80RAM);
	CZetMapArea(0xc000, 0xc3ff, 1, DrvZ80RAM);
	CZetMapArea(0xc000, 0xc3ff, 2, DrvZ80RAM);

//	CZetMapArea(0xc000, 0xffff, 0, DrvZ80RAM);	 //read
//	CZetMapArea(0xc000, 0xffff, 1, DrvZ80RAM);	 //write
//	CZetMapArea(0xc000, 0xffff, 2, DrvZ80RAM);


	CZetMemEnd();
	CZetClose();
#endif










}
//-------------------------------------------------------------------------------------------------------------------------------------
static void load_rom()
{
//	DrvReset = 1;
	int mapper = MAPPER_NONE;
	memset(AllMem, 0, MemEnd-AllMem);
//	ppi8255_exit();
//	TMS9928AExit();
	strcpy(sg1k_wboyRomDesc[0].szName,GFS_IdToName(file_id));	

	if (strcmp(sg1k_wboyRomDesc[0].szName, "TWINBEE.SG") == 0
	|| strcmp(sg1k_wboyRomDesc[0].szName, "YIEARKUN.SG") == 0
	|| strcmp(sg1k_wboyRomDesc[0].szName, "LEGKAGE.SG") == 0
	|| strcmp(sg1k_wboyRomDesc[0].szName, "KINGVAL.SG") == 0
	|| strcmp(sg1k_wboyRomDesc[0].szName, "KNIGHTM.SG") == 0
	|| strcmp(sg1k_wboyRomDesc[0].szName, "TANKBAT.SG") == 0
	|| strcmp(sg1k_wboyRomDesc[0].szName, "BOMBERM.SG") == 0)
		mapper = MAPPER_RAM_8K_EXT1;

	if (strcmp(sg1k_wboyRomDesc[0].szName, "BOMBERS.SG") == 0
	|| strcmp(sg1k_wboyRomDesc[0].szName, "MAGKID.SG") == 0
	|| strcmp(sg1k_wboyRomDesc[0].szName, "CASTLE.SG") == 0
	|| strcmp(sg1k_wboyRomDesc[0].szName, "RALLYX.SG") == 0
	|| strcmp(sg1k_wboyRomDesc[0].szName, "ROADFIGH.SG") == 0)
		mapper = MAPPER_RAM_8K_EXT2;

	BurnLoadRom(DrvZ80ROM + 0x0000, 0, 1);
	set_memory_map(mapper);
	
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)GFS_IdToName(file_id),26,200);

	DrvDoReset();	
	z80_set_reg(Z80_REG_IR,0x00);
	z80_set_reg(Z80_REG_PC,0x0000);
	z80_set_reg(Z80_REG_SP,0x00);
	z80_set_reg(Z80_REG_IRQVector,0xff);
	
	PCM_MeStop(pcm);
	memset(SOUND_BUFFER,0x00,RING_BUF_SIZE*8);
	nSoundBufferPos=0;
	PCM_MeStart(pcm);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static UINT8 update_input1(void)
{
	unsigned int i=0,k;
	UINT8 temp = 0xFF;
	SysDevice	*device;
//	__port = PER_OpenPort();
//	PER_GetPort(__port);
	if(( device = PER_GetDeviceR( &__port[0], 0 )) != NULL )
	{
		pltriggerE[0] = pltrigger[0];
		pltrigger[0] = PER_GetTrigger( device );
		pltriggerE[0] = (pltrigger[0]) ^ (pltriggerE[0]);
		pltriggerE[0] = (pltrigger[0]) & (pltriggerE[0]);

		if((pltriggerE[0] & PER_DGT_S)!=0)
		{
			load_rom();
//			DrvDoReset();
		}

		for(i=10;i<12;i++)
		{
//			if((pltrigger[0] & pad_asign[i])!=0)
			if((pltriggerE[0] & pad_asign[i])!=0)
			{
				switch(pltriggerE[0] & pad_asign[i] )
				{
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

					case PER_DGT_TL:
					if (file_id>2)	file_id--;
					else				file_id=file_max;
						FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,
						(Uint8 *)"            ",26,200);
						FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,
						(Uint8 *)GFS_IdToName(file_id),26,200);

				    default:
					break;
				}
			}
		}
	}
	else	pltrigger[0] = pltriggerE[0] = 0;

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
 void __fastcall sg1000_write_port(unsigned short port, UINT8 data)
{
	switch (port & 0xff)
	{
		case 0x7f:
			SN76496Write(0, data);
		return;

		case 0xbe:
			TMS9928AWriteVRAM(data);
		return;

		case 0xbf:
			TMS9928AWriteRegs(data);
		return;

		case 0xdc:
		case 0xdd:
		case 0xde:
		case 0xdf:
			ppi8255_w(0, port & 3, data);
		return;
	}
}

static UINT8 __fastcall sg1000_read_port(unsigned short port)
{
	switch (port & 0xff)
	{
		case 0xbe:
			return TMS9928AReadVRAM();

		case 0xbf:
			return TMS9928AReadRegs();

/*		case 0xdc:
		case 0xdd:
		case 0xde:
		case 0xdf:
                return ppi8255_r(0, port & 3); screws up sg-1000 inputs! */
		case 0xdc:
			return DrvInputs[0];

		case 0xdd:
			return DrvInputs[1];

		case 0xde:
			return 0x80;
	}

	return 0;
}

static UINT8 sg1000_ppi8255_portA_read() { return DrvInputs[0]; }
static UINT8 sg1000_ppi8255_portB_read() { return DrvInputs[1]; }
static UINT8 sg1000_ppi8255_portC_read() { return DrvDips[0]; }

static void sg1000_ppi8255_portC_write(UINT8 data)
{
	data &= 0x01; // coin counter
}

static void vdp_interrupt(int state)
{
#ifdef RAZE
	if(state)
		z80_raise_IRQ(0);
	else
		z80_lower_IRQ();
#else
	CZetSetIRQLine(0, state ? CZET_IRQSTATUS_ACK : CZET_IRQSTATUS_NONE);
#endif
}

static int DrvDoReset()
{
	memset (AllRam, 0, RamEnd - AllRam);

#ifdef RAZE
//	z80_stop_emulating();
	z80_reset();
#else
	CZetOpen(0);
	CZetReset();
//	CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);
//	CZetRaiseIrq(0xff);
	CZetClose();
#endif
	TMS9928AReset();
	return 0;
}

static int MemIndex()
{
	UINT8 *Next; Next = AllMem;

	DrvZ80ROM		= Next; Next +=   0x10000;//Next += 0x010000;
	DrvZ80ExtRAM	= DrvZ80ROM + 0x02000;

	AllRam				= DrvZ80ROM + 0x0c000;
	DrvZ80RAM		= DrvZ80ROM + 0x0c000;
	RamEnd			= Next;
	MemEnd			= Next;

	return 0;
}

static void __fastcall sg1000_write(UINT16 address, UINT8 data)
{

}

// mapper sg1000 mirror every 0x400
// $C000-$FFFF mapped to 2k mirrored RAM
/*static*/ UINT8 /*__fastcall*/ sg1000_read_c000(UINT16 address)
{
//	return DrvZ80RAM[(address-0xc000)&0x03ff];
	return DrvZ80ROM[address&0xc3ff];
}

/*static*/ UINT8 /*__fastcall*/ sg1000_write_c000(UINT16 address, UINT8 data)
{
//	DrvZ80RAM[(address-0xc000)&0x03ff] = data;
	DrvZ80ROM[address&0xc3ff] = data;
}

// $2000-$3FFF mapped to 8k external RAM
/*static*/ UINT8 /*__fastcall*/ sg1000_read_2000(UINT16 address)
{
	return DrvZ80ExtRAM[(address-0x2000)&0x1fff];
}

 // $2000-$3FFF mapped to 8k external RAM
/*static*/ UINT8 /*__fastcall*/ sg1000_write_2000(UINT16 address, UINT8 data)
{
	DrvZ80ExtRAM[(address-0x2000)&0x1fff] = data;
}


/*static*/ UINT8 /*__fastcall*/ sg1000_read(UINT16 address)
{
	return 0xff;
}

static int DrvInit()
{
	DrvInitSaturn();

	AllMem = NULL;
	MemIndex();
	int nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)malloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();
//	make_lut();
	load_rom();
// 	set_memory_map(0);
	SN76489AInit(0, 3579545, 0);

	TMS9928AInit(TMS99x8A, 0x4000, 0, 0, vdp_interrupt);

	ppi8255_init(1);
	PPI0PortReadA	= sg1000_ppi8255_portA_read;
	PPI0PortReadB	= sg1000_ppi8255_portB_read;
	PPI0PortReadC	= sg1000_ppi8255_portC_read;
	PPI0PortWriteC	= sg1000_ppi8255_portC_write;

	DrvDoReset();

	return 0;
}

static int DrvExit()
{
#ifdef RAZE
	z80_stop_emulating();
#else
	CZetExit();
#endif
	ppi8255_exit();
	TMS9928AExit();
	nBurnFunction = NULL;
	//pTransDraw = 
	/*color_2bpp_lut =*/ MemEnd = AllRam = RamEnd = DrvZ80ROM = DrvZ80Dec = DrvZ80RAM = DrvZ80ExtRAM = NULL;
//	SN76496Exit();

	free (AllMem);
	AllMem = NULL;
	DrvReset = 0;
	return 0;
}

static int DrvFrame()
{
/*	if (DrvReset) {
//		while(1);
		DrvDoReset();
	}
*/
	{ // Compile Inputs
		memset (DrvInputs, 0xff, 2);
		for (int i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			if (i==6 || i==7)
				DrvInputs[1] ^= (DrvJoy1[i] & 1) << i;
			else
				DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
		}
	}
 
	INT32 nInterleave = 16;
	INT32 nCyclesTotal[1] = { 3579545 / 60 };
	INT32 nCyclesDone[1] = { 0 };
	INT32 nSoundBufferPos2 = 0;

#ifndef RAZE
    CZetOpen(0);
#endif
	signed short *nSoundBuffer = (signed short *)0x25a20000;
	for (INT32 i = 0; i < nInterleave; i++)
	{
#ifdef RAZE
		nCyclesDone[0] += z80_emulate(nCyclesTotal[0] / nInterleave);
#else
		nCyclesDone[0] += CZetRun(nCyclesTotal[0] / nInterleave);
#endif
		// Render Sound Segment
	/*	if (pBurnSoundOut) 
		{			 */

			INT32 nSegmentLength = nBurnSoundLen / nInterleave;
			INT16* pSoundBuf = nSoundBuffer + nSoundBufferPos;
			SN76496Update(0, pSoundBuf, nSegmentLength);
			nSoundBufferPos += nSegmentLength;
			nSoundBufferPos2 += nSegmentLength;
	//	}
		//

	}

	TMS9928AInterrupt();
#ifndef RAZE
	CZetClose();
#endif
	// Make sure the buffer is entirely filled.
	
//	if (pBurnSoundOut) {
		INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos2;
		INT16* pSoundBuf = nSoundBuffer + nSoundBufferPos;
		if (nSegmentLength) {
			SN76496Update(0, pSoundBuf, nSegmentLength);
			nSoundBufferPos += nSegmentLength;
		}	   
		
//	} 

		if(nSoundBufferPos>=RING_BUF_SIZE/2.5)
		{
			nSoundBufferPos=0;
//				PCM_Task(pcm); // bon emplacement
		}
	PCM_Task(pcm); 
 
	//if (pBurnDraw) 
	{
		TMS9928ADraw();
	}
	return 0;
}
/*
//-------------------------------------------------------------------------------------------------------------------------------------
void	SetVblank2( void ){
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
*/
/*static*/ void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
//	colBgAddr		= (Uint16*)SCL_AllocColRam(SCL_NBG0,OFF);
	colAddr			= (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	(Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
//	SCL_SetColRam(SCL_NBG1,0,8,palette);
	SCL_SetColRam(SCL_NBG1,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initLayers(void)
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
	scfg.dispenbl		= OFF;
	
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
	scfg.dispenbl 	 = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 	 = SCL_BITMAP;
	scfg.mapover       = SCL_OVER_0;
	scfg.plate_addr[0] = ss_font;
	SCL_SetConfig(SCL_NBG1, &scfg);
	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initPosition(void)
{
	SCL_Open();
	ss_reg->n1_move_x = 0;
	ss_reg->n1_move_y = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ /*void SaturnInitMem()
{

	UINT8 *Next; Next = (UINT8 *)SaturnMem;
	name_lut		= Next; Next += 0x10000*sizeof(UINT16);
	bp_lut			= Next; Next += 0x10000*sizeof(UINT32);
	cram_lut		= Next; Next += 0x40*sizeof(UINT16);
	map_lut	 		= Next; Next += 0x800*sizeof(UINT16);
	dummy_write= Next; Next += 0x100*sizeof(unsigned);
	MemEnd			= Next;	
}	*/
//-------------------------------------------------------------------------------------------------------------------------------------
/*void dummy()
{

} */
//-------------------------------------------------------------------------------------------------------------------------------------
/*
static void make_lut()
{
	unsigned char bg,fg;

	for (int bg=0;bg<16;bg++)
	{
		for (int fg=0;fg<16;fg++)
		{
			unsigned int *position =	 &color_2bpp_lut[(bg|fg<<4)*4];
			position[0] = bg|bg<<4;
			position[1] = fg|bg<<4;
			position[2] = bg|fg<<4;
			position[3] = fg|fg<<4;
		}
	}
}
*/
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn()
{
//	InitCDsms();
//	SPR_InitSlaveSH();
//	SPR_RunSlaveSH((PARA_RTN*)dummy, NULL);
	nBurnSprites  = 4+32;//131;//27;
	nBurnLinescrollSize = 0;
	nSoundBufferPos = 0;//sound position à renommer

	SS_CACHE = cache      =(Uint8  *)SCL_VDP2_VRAM_B1;
	SS_MAP     = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_FONT   = ss_font    =(Uint16 *)SCL_VDP2_VRAM_A1;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_scl			= (Fixed32 *)SS_SCL;
//	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
//	pTransDraw	= (ss_vram+0x1100);
	file_id			= 2; // bubble bobble
//	file_max		= getNbFiles();

//	SaturnInitMem();
	int nLen = MemEnd - (UINT8 *)0;
//	SaturnMem = (UINT8 *)malloc(nLen);
//	bp_lut		= (UINT32 *)malloc(0x10000*sizeof(UINT32));
//	SaturnInitMem();
	ss_sprite[3].ax = 0;
	ss_sprite[3].ay = 0;

	ss_sprite[3].color          = 0x0;
	ss_sprite[3].charAddr    = 0x2220;// 0x2000 => 0x80 sprites <<6
	ss_sprite[3].control       = ( JUMP_NEXT | FUNC_NORMALSP); // | DIR_LRTBREV); // | flip);
	ss_sprite[3].drawMode = ( COLOR_0 | ECD_DISABLE | COMPO_REP); //256 colors
	ss_sprite[3].charSize    = 0x20C0;  // 256x*192y
	
	initLayers();
	
    SS_SET_N0PRIN(0);
    SS_SET_N1PRIN(6);
    SS_SET_S0PRIN(5);

//	initLayers();
	initColors();
//	initPosition();
//	initSprites(256+48-1,192+16-1,256-1,192-1,48,16);

//	initSprites(256-1,192-1,0,0,0,0);

//	 initScrolling(ON,SCL_VDP2_VRAM_B0+0x4000);
//	drawWindow(32,192,192,14,52);
	nBurnFunction = update_input1;
	drawWindow(0,192,192,2,66);
	SetVblank2();
}
//-------------------------------------------------------------------------------------------------------------------------------------


// End of driver, the following driver info. has been synthesized from hash/sg1000.xml of MESS

// Wonder Boy (Jpn, v1)


