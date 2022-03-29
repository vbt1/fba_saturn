// FB Alpha Pirate Ship Higemaru Module
// Based on MAME driver by Mirko Buffoni
#define CZ80 1
//#define RAZE 1
 
 #include "d_higemaru.h"

#define PONY

#ifdef PONY
#include "saturn/pcmstm.h"

int pcm[6];
Sint16 *nSoundBuffer[32];
extern unsigned int frame_x;
#endif

int ovlInit(char *szShortName)
{
	cleanBSS();

	struct BurnDriver nBurnDrvhigemaru = {
		"higema", NULL,
		"Pirate Ship Higemaru",
		higemaruRomInfo, higemaruRomName, DrvInputInfo, DrvDIPInfo,
		DrvInit, DrvExit, DrvFrame//NULL
	};

	memcpy(shared,&nBurnDrvhigemaru,sizeof(struct BurnDriver));
	ss_reg          = (SclNorscl *)SS_REG;
}

void __fastcall higemaru_write(unsigned short address, unsigned char data)
{
	switch (address)
	{
		case 0xc800:
//			flipscreen = data >> 7;
		break;

		case 0xc801:
		case 0xc802:
			AY8910Write(0, (address - 1) & 1, data);
		break;

		case 0xc803:
		case 0xc804:
			AY8910Write(1, (address - 1) & 1, data);
		break;
	}
//#ifndef RAZE
#if 0
	if(address>= 0xd000 && address <=0xd9ff)
	{
				Rom[address] = data;
/*		if(Rom[address]!=data)
		{
			bg_dirtybuffer[address&0x3ff] = 1;
			Rom[address] = data;
		}*/
	}
#endif
}

#ifdef RAZE
void __fastcall higemaru_write_d000(unsigned short address, unsigned char data)
{
	if(Rom[address]!=data)
	{
		bg_dirtybuffer[address&0x3ff] = 1;
		Rom[address] = data;
	}
}
#endif


unsigned char __fastcall higemaru_read(unsigned short address)
{
	unsigned char ret;
	unsigned int i;

	switch (address)
	{
		case 0xc000:
		{
			ret = 0xff;

			//for (i = 0; i < 8; i++) 
			for (i = 0; i < 7; i++) 
			{
				ret ^= DrvJoy1[i] << i;
			}

			return ret;
		}

		case 0xc001:
		{
			ret = 0xff;

			//for (i = 0; i < 8; i++) 
			for (i = 0; i < 7; i++) 
			{
				ret ^= DrvJoy2[i] << i;
			}

			return ret;
		}

		case 0xc002:
		{
			ret = DrvDips[0];

			for (i = 0; i < 8; i++)
				ret ^= DrvJoy3[i] << i;

			return ret;
		}

		case 0xc003:
			return DrvDips[1];

		case 0xc004:
			return DrvDips[2];
	}

	return 0;
}

int MemIndex()
{
	extern unsigned int _malloc_max_ram;
	UINT8 *Next; Next = (unsigned char *)&_malloc_max_ram;
	memset(Next, 0, MALLOC_MAX);

	Rom           = Next; Next += 0x10000;
	Gfx0           = Next; Next += 0x08000;
	Gfx1           = Next; Next += 0x08000;
//	Gfx0           = (unsigned char *)0x00200000;//Next; Next += 0x08000;
//	Gfx1           = (unsigned char *)0x00208000;//Next; Next += 0x08000;

	Prom					= Next; Next += 0x00300;
	CZ80Context		= Next; Next += sizeof(cz80_struc);
	map_offset_lut	= Next; Next += 1024 * sizeof(UINT16);
	MemEnd			= Next;

	return 0;
}

int DrvDoReset()
{
	memset (Rom + 0xd000, 0, 0x2000);

	AY8910Reset(0);
	AY8910Reset(1);
#ifdef CZ80
	CZetOpen(0);
	CZetReset();
	CZetClose();
#else
#ifdef RAZE
	z80_reset();
#endif
#endif
	return 0;
}

void DrvPaletteInit()
{
	unsigned int tmp[0x20];
	unsigned int i;

	init_32_colors(tmp,Prom);

	Prom += 0x100;
	int xvbt=0;

//palette 16 a 88
for (i = 0; i < 0x80; i+=4) 
	{
		colBgAddr[0x000 + i  +xvbt] = tmp[Prom[i+0]];
		colBgAddr[0x000 + i+1+xvbt] = tmp[Prom[i+1]];
		colBgAddr[0x000 + i+2+xvbt] = tmp[Prom[i+2]];
		colBgAddr[0x000 + i+3+xvbt] = tmp[Prom[i+3]];
		xvbt+=12;
	}

	//for (i = 0x80; i < 0x180; i+=4) 
	for (i = 0x80; i < 0x180; i+=4) 
	{
		colAddr[0x000 + i   - 0x80] = tmp[(Prom[i   + 0x80] & 0x0f) | 0x10];
		colAddr[0x000 + i+1 - 0x80] = tmp[(Prom[i+1 + 0x80] & 0x0f) | 0x10];
		colAddr[0x000 + i+2 - 0x80] = tmp[(Prom[i+2 + 0x80] & 0x0f) | 0x10];
		colAddr[0x000 + i+3 - 0x80] = tmp[(Prom[i+3 + 0x80] & 0x0f) | 0x10];
		//colAddr[i-0x80] = tmp[Prom[i]]; // pour les sprites
	}
//	*(Uint16 *)0x25E00000 = colBgAddr[0];  		   
	//colAddr[0x000] = RGB(10,10,10);
}
//-------------------------------------------------------------------------------------------------------------------------------------
int DrvGfxDecode()
{
	/*static*/ int Planes[4] = { 0x10004, 0x10000, 0x00004, 0x00000 };
	/*static*/ int XOffs[16] = { 0x000, 0x001, 0x002, 0x003, 0x008, 0x009, 0x00a, 0x00b,
				 0x100, 0x101, 0x102, 0x103, 0x108, 0x109, 0x10a, 0x10b };
	/*static*/ int YOffs[16] = { 0x000, 0x010, 0x020, 0x030, 0x040, 0x050, 0x060, 0x070,
				 0x080, 0x090, 0x0a0, 0x0b0, 0x0c0, 0x0d0, 0x0e0, 0x0f0 };
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;

	GfxDecode4Bpp(0x200, 2,  8,  8, Planes + 2, XOffs, YOffs, 0x080, Gfx0, cache);
	GfxDecode4Bpp(0x080, 4, 16, 16, Planes,     XOffs, YOffs, 0x200, Gfx1, &ss_vram[0x1100]);
	swapFirstLastColor(&ss_vram[0x1100],0x0f,0x8000);

	return 0;
}

int DrvInit()
{
	DrvInitSaturn();
	MemIndex();

//	for (i = 0; i < 6; i++) {
//		pAY8910Buffer[i] = pFMBuffer + SOUND_LEN * i;
//	}

	{
		for (UINT32 i = 0; i < 4; i++) {
			if (BurnLoadRom(Rom + i * 0x2000, i + 0, 1)) return 1;
		}

		if (BurnLoadRom(Gfx0 + 0x0000, 4, 1)) return 1;
		if (BurnLoadRom(Gfx1 + 0x0000, 5, 1)) return 1;
		if (BurnLoadRom(Gfx1 + 0x2000, 6, 1)) return 1;

		for (UINT32 i = 0; i < 3; i++) {
			if (BurnLoadRom(Prom + i * 0x100, i + 7, 1)) return 1;
		}

		if (DrvGfxDecode()) return 1;
		DrvPaletteInit();
	}
#ifdef CZ80
	CZetInit2(1,CZ80Context);
	CZetOpen(0);
	CZetMapArea(0x0000, 0x7fff, 0, Rom + 0x0000);
	CZetMapArea(0x0000, 0x7fff, 2, Rom + 0x0000);
	CZetMapArea(0xd000, 0xd7ff, 0, Rom + 0xd000);
	CZetMapArea(0xd000, 0xd9ff, 1, Rom + 0xd000);
	CZetMapArea(0xe000, 0xefff, 0, Rom + 0xe000);
	CZetMapArea(0xe000, 0xefff, 1, Rom + 0xe000);
	CZetMapArea(0xe000, 0xefff, 2, Rom + 0xe000);
	CZetSetWriteHandler(higemaru_write);
	CZetSetReadHandler(higemaru_read);
	CZetClose();
#else
#ifdef RAZE
	z80_init_memmap();

 	z80_map_fetch (0x0000, 0x7fff, Rom + 0x0000); 
	z80_map_read  (0x0000, 0x7fff, Rom + 0x0000); //2 fetch

	z80_map_fetch (0xd000, 0xd7ff, Rom + 0xd000); 
	z80_map_read  (0xd000, 0xd9ff, Rom + 0xd000); //1 write
//	z80_map_write (0xd000, 0xd9ff, Rom + 0xd000); //2 fetch 
	z80_map_write (0xd800, 0xd9ff, Rom + 0xd800); //2 fetch 

	z80_map_fetch (0xe000, 0xefff, Rom + 0xe000); 
	z80_map_read  (0xe000, 0xefff, Rom + 0xe000); //1 write
	z80_map_write (0xe000, 0xefff, Rom + 0xe000); //2 fetch 

//	z80_add_write(0xc800, 0xcfff, 1, (void *)&higemaru_write);
//	z80_add_read(0xc000, 0xc1ff, 1, (void *)&higemaru_read);
   
   	z80_add_write(0xd000, 0xd7ff, 1, (void *)&higemaru_write_d000);
	z80_add_write(0xc800, 0xc804, 1, (void *)&higemaru_write);
	z80_add_read (0xc000, 0xc004, 1, (void *)&higemaru_read);

	z80_end_memmap(); 
#endif
#endif
//	AY8910Init(0, 1500000, nBurnSoundRate, NULL, NULL, NULL, NULL);
//	AY8910Init(1, 1500000, nBurnSoundRate, NULL, NULL, NULL, NULL);
	AY8910Init(0, 1500000, nBurnSoundRate, NULL, NULL, NULL, NULL);
	AY8910Init(1, 1500000, nBurnSoundRate, NULL, NULL, NULL, NULL);

	make_lut();
	DrvDoReset();
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initLayers()
{
    Uint16	CycleTb[]={
		0x1f5f, 0xffff, //A0
		0xffff, 0xffff,	//A1
		0xffff,0x4eff,   //B0
		0xffff, 0xffff  //B1
	};

	SclConfig	scfg;
// 3 nbg
	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD;
//	scfg.flip          = SCL_PN_12BIT; 
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
//	scfg.patnamecontrl =  0x000c;// VRAM B1 のオフセット 
	//for(i=0;i<4;i++)   
	scfg.plate_addr[0] = (Uint32)ss_map;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG1, &scfg);

//	scfg.dispenbl 	     = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
//	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 	     = SCL_BITMAP;
	scfg.mapover         = SCL_OVER_0;
	scfg.plate_addr[0]   = (Uint32)ss_font;
// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initPosition()
{
	SCL_Open();
	ss_reg->n1_move_x =  (-8<<16) ;
	ss_reg->n1_move_y =  (32<<16) ;
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initColors()
{
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
	(Uint16*)SCL_AllocColRam(SCL_NBG3,OFF);
 	colAddr    = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void make_lut(void)
{
	unsigned int i,delta=0;
	int sx, sy, row,col;

	for (i = 0; i < 0x400;i++) 
	{
		int sx = (i) & 0x1f;
		int sy = (((i+0x40) >> 5) & 0x1f)<<6;
		map_offset_lut[i] = (sx | sy)<<1;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
	nBurnSprites  = 51;//27;

	SS_CACHE = cache      =(Uint8  *)SCL_VDP2_VRAM_A0;
	SS_MAP     = ss_map     =(Uint16 *)SCL_VDP2_VRAM_A1;
	SS_FONT   = ss_font    =(Uint16 *)SCL_VDP2_VRAM_B0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;

//3 nbg
	SS_SET_S0PRIN(6);
	SS_SET_N0PRIN(7);
	SS_SET_N1PRIN(4);
//	SCL_SET_N2PRIN(3);

	initLayers();
	initPosition();
	initColors();
//	memset(bg_dirtybuffer,1,1024);
	initSprites(256+8-1,224-1,0,0,8,-16);

	drawWindow(0,224,0,2,62);
#ifdef PONY
	frame_x	= 0;
	nBurnFunction = sdrv_stm_vblank_rq;
#else
	PCM_MeStop(pcm);
	Set6PCM();	
#endif	
}
//-------------------------------------------------------------------------------------------------------------------------------------
int DrvExit()
{
	DrvDoReset();
#ifdef RAZE
	z80_stop_emulating();
	z80_add_write(0xd000, 0xd7ff, 1, (void *)NULL);
	z80_add_write(0xc800, 0xc804, 1, (void *)NULL);
	z80_add_read (0xc000, 0xc004, 1, (void *)NULL);
#else
	CZetExit2();
#endif

	AY8910Exit(1);
	AY8910Exit(0);

	memset((void *)SOUND_BUFFER,0x00,0x4000*6);
	
	CZ80Context = MemEnd = Rom = Gfx0 = Gfx1 = Prom = NULL;
	map_offset_lut = NULL;
#ifdef PONY
	for(unsigned int i=0;i<6;i++)
	{
		remove_raw_pcm_buffer(pcm[i]);
	}
#else
	for(unsigned int i=0;i<6;i++)
	{
		PCM_MeStop(pcm6[i]);
		PCM_DestroyMemHandle(pcm6[i]);
	}	
#endif
	//cleanDATA();
	cleanBSS();

	nSoundBufferPos = 0;
	return 0;
}

void DrvDrawSprites()
{
	 // sprites
	for (int offs = 0x170; offs >= 0; offs -= 16)
	{
		int code,color,flip;

		code  = Rom[0xd880 + offs] & 0x7f;
		color = Rom[0xd884 + offs] & 0x0f;
//		sx    = Rom[0xd88c + offs];
//		sy    = Rom[0xd888 + offs];
		flip  = Rom[0xd884 + offs] & 0x30;
			
		unsigned int delta=((offs)>>4)+3;
		ss_sprite[delta].ax = Rom[0xd88c + offs];
		ss_sprite[delta].ay = Rom[0xd888 + offs];

		ss_sprite[delta].control    = ( JUMP_NEXT | FUNC_NORMALSP | flip);
		ss_sprite[delta].drawMode   = ( ECD_DISABLE | COMPO_REP);	// 16 couleurs
		ss_sprite[delta].charSize   = 0x210;  //0x100 16*16
		ss_sprite[delta].charAddr   = 0x220+(code<<4);
		ss_sprite[delta].color      = (color<<4);		
	}
}
void DrvDrawBackground()
{
 // back ground
	for (int offs = 0x40; offs < 0x3c0; offs++)
	{
#ifdef RAZE
		if(bg_dirtybuffer[offs])
#endif
		{
#ifdef RAZE
			bg_dirtybuffer[offs]=0;
#endif
			unsigned int code = Rom[0xd000 + offs] | ((Rom[0xd400 + offs] & 0x80) << 1);
			unsigned int color = Rom[0xd400 + offs] & 0x1f;
			unsigned int x = map_offset_lut[offs];
			ss_map[x] = color;
			ss_map[x+1] =  code;
		}
	}
//	DrvDrawSprites ();
// 	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------
#ifdef PONY
void DrvFrame_old();

void DrvFrame()
{

	for (unsigned int i=0;i<6;i++)
	{
		pcm[i] = add_raw_pcm_buffer(0,SOUNDRATE,nBurnSoundLen*20*2);
		nSoundBuffer[i] = (Sint16 *)(SNDRAM+(m68k_com->pcmCtrl[pcm[i]].hiAddrBits<<16) | m68k_com->pcmCtrl[pcm[i]].loAddrBits);
	}
//	InitCD(); // si on lance juste pour pang
//	ChangeDir("PANG");  // si on lance juste pour pang
	pcm_stream_host(DrvFrame_old);
}

void DrvFrame_old()
#else
 void DrvFrame()
 #endif
{
//	if (DrvReset) {
//		DrvDoReset();
//	}
  	SPR_RunSlaveSH((PARA_RTN*)DrvDrawBackground, NULL);

#ifdef CZ80
	
	CZetRun(23333);

	CZetRaiseIrq(0xd7);
	CZetSetIRQLine(0xd7, 0);
	CZetRun(23334);
	CZetSetIRQLine(0xcf, 0);
	CZetRaiseIrq(0xcf);

#else
#ifdef RAZE
			z80_emulate(23333);
			z80_raise_IRQ(0xd7);
//			z80_emulate(0);
//			z80_lower_IRQ(0xd7);
//			z80_emulate(0);

			z80_emulate(23334);
			z80_raise_IRQ(0xcf);
	//		z80_emulate(0);
	//		z80_lower_IRQ(0xcf);
	//		z80_emulate(0);
#endif
#endif

	updateSound();
	DrvDrawSprites();
if((*(unsigned char *)0xfffffe11 & 0x80) == 0)
	SPR_WaitEndSlaveSH();
#ifdef PONY
	_spr2_transfercommand();
	frame_x++;	
#endif	
}

//-------------------------------------------------------------------------------------------------------------------------------------
void updateSound()
{
#ifndef PONY	
	unsigned short *nSoundBuffer1 = (unsigned short *)0x25a24000+nSoundBufferPos;

	AY8910UpdateDirect(0, &nSoundBuffer1[0], nBurnSoundLen);
	AY8910UpdateDirect(1, &nSoundBuffer1[0x6000], nBurnSoundLen);
	nSoundBufferPos+=nBurnSoundLen;

	if(nSoundBufferPos>=RING_BUF_SIZE>>1)
	{
		for (unsigned int i=0;i<6;i++)
		{
			PCM_NotifyWriteSize(pcm6[i], nSoundBufferPos);
			PCM_Task(pcm6[i]); // bon emplacement
		}
		nSoundBufferPos=0;
	}
#else
//	signed short *nSoundBuffer2 = (signed short *)nSoundBuffer[0]+(nSoundBufferPos<<1);
//	signed short *nSoundBuffer3 = (signed short *)nSoundBuffer[3]+(nSoundBufferPos<<1);
	AY8910UpdateDirect(0, &nSoundBuffer[pcm[0]][nSoundBufferPos<<1], &nSoundBuffer[pcm[1]][nSoundBufferPos<<1], &nSoundBuffer[pcm[2]][nSoundBufferPos<<1], nBurnSoundLen);
	AY8910UpdateDirect(1, &nSoundBuffer[pcm[3]][nSoundBufferPos<<1], &nSoundBuffer[pcm[4]][nSoundBufferPos<<1],&nSoundBuffer[pcm[5]][nSoundBufferPos<<1], nBurnSoundLen);
	nSoundBufferPos+=nBurnSoundLen;

	if(nSoundBufferPos>=nBurnSoundLen*10)
	{
	//	int i=0;
		for (unsigned int i=0;i<6;i++)
		{
			pcm_play(pcm[i], PCM_SEMI, 7);
		}
		nSoundBufferPos=0;
	}		
#endif	
}
#ifndef PONY
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
static void Set6PCM()
{
	PcmCreatePara	para[6];
	PcmInfo 		info[6];
	PcmStatus	*st;
	static PcmWork g_movie_work[6];

	for (int i=0; i<6; i++)
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
		pcm6[i] = createHandle(&para[i]);

		PCM_SetPcmStreamNo(pcm6[i], i);

		PCM_SetInfo(pcm6[i], &info[i]);
		PCM_ChangePcmPara(pcm6[i]);

		PCM_MeSetLoop(pcm6[i], 0);//SOUNDRATE*120);
		PCM_Start(pcm6[i]);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
#endif
