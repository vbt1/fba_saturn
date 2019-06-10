// FB Alpha Sega SG-1000-based arcade driver module
// Based on MAME driver by Tomasz Slanina
// Code by iq_132, fixups & bring up-to-date by dink Aug 18, 2014
//#define RAZE 1

#include    "machine.h"
#include "d_sg1000.h"

#define SAMPLE 7680L
#define nBurnSoundLen 128
#define nInterleave 16
#define nCyclesTotal 3579545 / 60
#define nSegmentLength nBurnSoundLen / nInterleave

int ovlInit(char *szShortName)
{
	cleanBSS();

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
/*static*/ void	SetVblank2( void ){
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
/*static*/ void set_memory_map(int mapper)
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
	CZetOpen(0);
	CZetSetOutHandler(sg1000_write_port);
	CZetSetInHandler(sg1000_read_port);

	if(mapper == MAPPER_RAM_8K_EXT2)
	{
		CZetSetWriteHandler(sg1000_write_ext2);
		CZetSetReadHandler(sg1000_read_ext2);

//		CZetMapArea(0x0000, 0x1fff, 0, DrvZ80ROM);
//		CZetMapArea(0x0000, 0x1fff, 1, DrvZ80ROM);

//		CZetMapArea(0x4000, 0xbfff, 0, DrvZ80ROM+0x4000);
//		CZetMapArea(0x4000, 0xbfff, 1, DrvZ80ROM+0x4000);
	}
	else
	{
		CZetSetWriteHandler(sg1000_write);
		CZetSetReadHandler(sg1000_read);

		CZetMapArea(0x0000, 0xbfff, 0, DrvZ80ROM);
		CZetMapArea(0x0000, 0xbfff, 1, DrvZ80ROM);
	}
	CZetMapArea(0x0000, 0xbfff, 2, DrvZ80ROM);

	CZetMapArea(0xc000, 0xffff, 0, DrvZ80RAM);	 //read
	CZetMapArea(0xc000, 0xffff, 1, DrvZ80RAM);	 //write
	CZetMapArea(0xc000, 0xffff, 2, DrvZ80RAM);

	CZetMemEnd();
	CZetClose();
#endif
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void load_rom()
{
	int mapper = MAPPER_NONE;
	memset (AllRam, 0, RamEnd - AllRam);

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

#ifdef RAZE
	if (strcmp(sg1k_wboyRomDesc[0].szName, "BOMBERS.SG") == 0
	|| strcmp(sg1k_wboyRomDesc[0].szName, "MAGKID.SG") == 0
	|| strcmp(sg1k_wboyRomDesc[0].szName, "CASTLE.SG") == 0
	|| strcmp(sg1k_wboyRomDesc[0].szName, "RALLYX.SG") == 0
	|| strcmp(sg1k_wboyRomDesc[0].szName, "ROADFIGH.SG") == 0)
#else
	if(strcmp(sg1k_wboyRomDesc[0].szName, "MAGKID.SG") == 0)
#endif
		mapper = MAPPER_RAM_8K_EXT2;

	BurnLoadRom(DrvZ80ROM + 0x0000, 0, 1);
	set_memory_map(mapper);
	
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)GFS_IdToName(file_id),26,200);

	DrvDoReset();
#ifdef RAZE	
	z80_set_reg(Z80_REG_IR,0x00);
	z80_set_reg(Z80_REG_PC,0x0000);
	z80_set_reg(Z80_REG_SP,0x00);
	z80_set_reg(Z80_REG_IRQVector,0xff);
#endif	
	PCM_MeStop(pcm);
	memset((void *)SOUND_BUFFER,0x00,RING_BUF_SIZE*8);
	nSoundBufferPos=0;
	PCM_MeStart(pcm);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void update_input1(void)
{
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
		}

		for(UINT32 i=10;i<12;i++)
		{
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

/*static*/ UINT8 __fastcall sg1000_read_port(unsigned short port)
{
	switch (port & 0xff)
	{
		case 0xbe:
			return SG_TMS9928AReadVRAM();

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

/*static*/ UINT8 sg1000_ppi8255_portA_read() { return DrvInputs[0]; }
/*static*/ UINT8 sg1000_ppi8255_portB_read() { return DrvInputs[1]; }
/*static*/ UINT8 sg1000_ppi8255_portC_read() { return DrvDips[0]; }

/*static*/ void sg1000_ppi8255_portC_write(UINT8 data)
{
	data &= 0x01; // coin counter
}

/*static*/ void vdp_interrupt(int state)
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

/*static*/ int DrvDoReset()
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

/*static*/ int MemIndex()
{
	UINT8 *Next; Next = AllMem;

	DrvZ80ROM		= Next; Next +=   0x10000;//Next += 0x010000;
	DrvZ80ExtRAM	= DrvZ80ROM + 0x02000;

	AllRam				= DrvZ80ROM + 0x0c000;
	DrvZ80RAM		= DrvZ80ROM + 0x0c000;

	RamEnd			= Next;
	CZ80Context		= Next; Next += sizeof(cz80_struc);
	TMSContext		= Next; Next += (0x4000+0x6000+0x1000);
	MemEnd			= Next;

	return 0;
}

/*static*/ void __fastcall sg1000_write(UINT16 address, UINT8 data)
{

}
#ifdef RAZE
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
#else
/*static*/ UINT8 /*__fastcall*/ sg1000_read_ext2(UINT16 address)
{
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"sg1000_read_ext2     ",26,180);

	 if(address>=0x2000 && address <=0x3fff)
		return DrvZ80ExtRAM[(address-0x2000)&0x1fff];
	 if(address <=0xbfff)
		return DrvZ80ROM[address];
	 return 0xff;
}

 // $2000-$3FFF mapped to 8k external RAM
/*static*/ void /*__fastcall*/ sg1000_write_ext2(UINT16 address, UINT8 data)
{
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"sg1000_write_ext2     ",26,180);

	 if(address>=0x2000 && address <=0x3fff)
		DrvZ80ExtRAM[(address-0x2000)&0x1fff] = data;
//	 else	if(address <=0xbfff)
//		DrvZ80ROM[address] = data;
}
#endif

/*static*/ UINT8 /*__fastcall*/ sg1000_read(UINT16 address)
{
	return 0xff;
}

/*static*/ int DrvInit()
{
	DrvInitSaturn();
	AllMem = NULL;
	MemIndex();
	if ((AllMem = (UINT8 *)malloc(MALLOC_MAX)) == NULL) return 1;
	memset(AllMem, 0, MALLOC_MAX);
	MemIndex();
//	memset(CZ80Context,0x00,0x1080);

	#ifndef RAZE
	CZetInit2(1,CZ80Context);
	#endif

	SN76489AInit(0, 3579545, 0);

	TMS9928AInit(TMS99x8A, 0x4000, 0, 0, vdp_interrupt,TMSContext);

	ppi8255_init(1);
	PPI0PortReadA	= sg1000_ppi8255_portA_read;
	PPI0PortReadB	= sg1000_ppi8255_portB_read;
	PPI0PortReadC	= sg1000_ppi8255_portC_read;
	PPI0PortWriteC	= sg1000_ppi8255_portC_write;
	load_rom();

	return 0;
}

/*static*/ int DrvExit()
{
	nBurnFunction = NULL;
//	wait_vblank();

#ifdef RAZE
	z80_stop_emulating();

	z80_set_in((unsigned char (*)(unsigned short))NULL);
	z80_set_out((void (*)(unsigned short, unsigned char))NULL);

	z80_add_write(0x0000, 0xbfff, 1, (void *)NULL);

	z80_add_read(0xc000, 0xffff, 1, (void *)NULL);
	z80_add_write(0xc000, 0xffff, 1, (void *)NULL);

	z80_add_read(0x2000, 0x3fff, 1, (void *)NULL);
	z80_add_write(0x2000, 0x3fff, 1, (void *)NULL);
#else
	CZetSetWriteHandler((void *)NULL);
	CZetSetReadHandler((void *)NULL);	
	CZetExit2();
#endif
	TMS9928AExit();
	ppi8255_exit();
//	SN76489AInit(0, 0, 0);	

	memset((void *)SOUND_BUFFER,0x00,0x20000);
	
	CZ80Context = TMSContext = MemEnd = AllRam = RamEnd = DrvZ80ROM = DrvZ80RAM = DrvZ80ExtRAM = NULL;
//	__port = NULL;
	free (AllMem);
	AllMem = NULL;

	cleanDATA();
	cleanBSS();

	nSoundBufferPos=0;	

	return 0;
}

/*static*/ int DrvFrame()
{
	{ // Compile Inputs
		memset (DrvInputs, 0xff, 2);
		for (UINT32 i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			if (i==6 || i==7)
				DrvInputs[1] ^= (DrvJoy1[i] & 1) << i;
			else
				DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
		}
	}

	UINT32 nCyclesDone = 0;
	UINT32 nSoundBufferPos2 = 0;

#ifndef RAZE
    CZetOpen(0);
#endif
	signed short *nSoundBuffer = (signed short *)0x25a20000;
	for (UINT32 i = 0; i < nInterleave; i++)
	{
#ifdef RAZE
		nCyclesDone += z80_emulate(nCyclesTotal / nInterleave);
#else
		nCyclesDone += CZetRun(nCyclesTotal / nInterleave);
#endif
		INT16* pSoundBuf = nSoundBuffer + nSoundBufferPos;
		SN76496Update(0, pSoundBuf, nSegmentLength);
		nSoundBufferPos += nSegmentLength;
		nSoundBufferPos2 += nSegmentLength;
	}

	TMS9928AInterrupt();
#ifndef RAZE
	CZetClose();
#endif
	TMS9928ADraw();
//	unsigned char *ss_vram = (unsigned char *)SS_SPRAM;

//	DMA_ScuIndirectMemCopy((ss_vram+0x1100+0x10000),tmpbmp,0x4000,0);

// Make sure the buffer is entirely filled.
	int nSegmentLength2 = nBurnSoundLen - nSoundBufferPos2;
	if (nSegmentLength2) 
	{
		INT16* pSoundBuf = nSoundBuffer + nSoundBufferPos;
		SN76496Update(0, pSoundBuf, nSegmentLength2);
		nSoundBufferPos += nSegmentLength2;
	}	   

	if(nSoundBufferPos>=RING_BUF_SIZE>>1)
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos);
		nSoundBufferPos=0;
			PCM_Task(pcm); // bon emplacement sinon le pcm stoppe
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colAddr			= (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	(Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
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

	scfg.dispenbl		= OFF;

	SCL_SetConfig(SCL_NBG0, &scfg);
	SCL_SetConfig(SCL_NBG2, &scfg);
	
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
/*void dummy()
{

} */
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn()
{
//	InitCDsms();
	nBurnSprites  = 4+32;//131;//27;
	nBurnLinescrollSize = 0;
	nSoundBufferPos = 0;//sound position à renommer

	SS_CACHE = cache      =(Uint8  *)SCL_VDP2_VRAM_B1;
	SS_MAP     = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_FONT   = ss_font    =(Uint16 *)SCL_VDP2_VRAM_A1;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	file_id			= 2; // bubble bobble

	ss_sprite[3].ax = 0;
	ss_sprite[3].ay = 0;

	ss_sprite[3].color          = 0x0;
	ss_sprite[3].charAddr    = 0x2220;// 0x2000 => 0x80 sprites <<6
	ss_sprite[3].control       = ( JUMP_NEXT | FUNC_NORMALSP); // | DIR_LRTBREV); // | flip);
	ss_sprite[3].drawMode = ( COLOR_0 | ECD_DISABLE | COMPO_REP); //256 colors
	ss_sprite[3].charSize    = 0x20C0;  // 256x*192y
	
    SS_SET_N0PRIN(0);
    SS_SET_N1PRIN(6);
    SS_SET_S0PRIN(5);

	initLayers();
	initColors();

	nBurnFunction = update_input1;
	drawWindow(0,192,192,2,66);
	SetVblank2();
}
//-------------------------------------------------------------------------------------------------------------------------------------


// End of driver, the following driver info. has been synthesized from hash/sg1000.xml of MESS

// Wonder Boy (Jpn, v1)


