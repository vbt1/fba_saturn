// tester sur saturn ce qui est plus rapide entre utiliser le 2eme cpu et tout mettre dans une seule boucle
// a la limite en meme temps essayer de de pas remplir l'image de fond si un puyo est affich� dessus
// FB Alpha - "News" Driver
#define CZ80 1
#define PONY
//#define RAZE 1
#define SC_RELEASE 1
#define CACHE2 1
#define CACHE 1
#include "d_news.h"

#ifdef PONY
#include "saturn/pcmstm.h"

int pcm1=0;
Sint16 *nSoundBuffer=NULL;
extern unsigned short frame_x;
extern unsigned short frame_y;
#endif
 
 
int ovlInit(char *szShortName)
{
	cleanBSS();

	struct BurnDriver nBurnDrvNewsa = {
		"newsa", "news",
		"News (set 2)",
		NewsaRomInfo, NewsaRomName, NewsInputInfo, NewsaDIPInfo,
		NewsInit, NewsExit, NewsFrame//, NULL
	};

	struct BurnDriver nBurnDrvNews = {
		"news", NULL,
		"News (set 1)",
		NewsRomInfo, NewsRomName, NewsInputInfo, NewsDIPInfo,
		NewsInit, NewsExit, NewsFrame//, NULL
	};

    if (strcmp(nBurnDrvNewsa.szShortName, szShortName) == 0)
		memcpy(shared,&nBurnDrvNewsa,sizeof(struct BurnDriver));
	else
		memcpy(shared,&nBurnDrvNews,sizeof(struct BurnDriver));
	ss_reg          = (SclNorscl *)SS_REG;
	
	return 0;
}

inline  void NewsMakeInputs()
{
	// Reset Inputs
	NewsInput[0] = 0x00;
	unsigned int i;
	// Compile Digital Inputs
	for (i = 0; i < 8; i++) {
		NewsInput[0] |= (NewsInputPort0[i] & 1) << i;
	}

	// Clear Opposites
	if ((NewsInput[0] & 0x0c) == 0x0c) {
		NewsInput[0] &= ~0x0c;
	}
	if ((NewsInput[0] & 0x30) == 0x30) {
		NewsInput[0] &= ~0x30;
	}	
}

// Misc Driver Functions and Memory Handlers
void NewsDoReset()
{
	BgPic = 0;

#ifdef CZ80
//	CZetOpen(0);
	CZetReset();
//	CZetClose();
#endif

#ifdef RAZE
	z80_reset();
#endif
	MSM6295Reset(0);
}

inline void make_lut(void)
{
    UINT32 j;
	for(j = 0; j < 4096; j++)
	{
		UINT32 r = (j >> 8) & 0x0f;
		UINT32 g = (j >> 4) & 0x0f;
		UINT32 b = (j >> 0) & 0x0f;

		r = (r << 4) | r;
		g = (g << 4) | g;
		b = (b << 4) | b;
        cram_lut[j] =BurnHighCol(r,g,b,0);
    }

	UINT16 *map_lut = &map_offset_lut[0];

	for (UINT32 offs = 0; offs < 1024; offs++) 
	{
		*map_lut++ = (offs & 0x1f) | (offs / 0x20) <<6;
	}
}

unsigned char __fastcall NewsRead(unsigned short a)
{
	switch (a) {
		case 0xc000: {
			return NewsDip[0];
		}

		case 0xc001: {
			return 0xff - NewsInput[0];
		}

		case 0xc002: {
			return MSM6295ReadStatus(0);
		}
	}

	return 0;
}
void __fastcall NewsWrite(unsigned short a, unsigned char d)
{
	switch (a) {
		case 0xc002: {
			MSM6295Command(0, d);
			return;
		}

		case 0xc003: {
#ifdef CACHE2
			if(BgPic != d)
			{
				BgPic = d;
				memset(dirty_buffer+0x400,1,1024);
			}
#else
				BgPic = d;
#endif
			return;
		}
	}

	if (a >= 0x8000 && a <= 0x8fff) 
	{
		if(RamStart[a]!=d)
		{
			RamStart[a]=d;
			dirty_buffer[(a>>1)&0xfff] = 1; 
		}
		return;
	}

	if (a >= 0x9000 && a <= 0x91ff) 
	{
		if(RamStart[a]!=d)
		{
			RamStart[a] = d;
			a&=0x1fe;
			a>>=1;
			colBgAddr2[a] = colBgAddr[a] = cram_lut[*((UINT16 *)NewsPaletteRam+a)];
		}
		return;
	}
}

// Function to Allocate and Index required memory
inline void MemIndex()
{
	extern unsigned int _malloc_max_ram;
	UINT8 *Next; Next = (unsigned char *)&_malloc_max_ram;
	memset(Next, 0, MALLOC_MAX);

	NewsRom			= Next; Next += 0x10000;
	MSM6295ROM		= Next; Next += 0x40000;

	NewsRam			= Next; Next += 0x02000;
	RamStart		= Next-0x8000;
	NewsFgVideoRam	= Next; Next += 0x01000;
//	NewsBgVideoRam	= Next; Next += 0x00800;
	NewsPaletteRam	= Next; Next += 0x00200;
	cram_lut		= (UINT16*)Next; Next += (4096*2);
	map_offset_lut	= (UINT16*)Next; Next += (0x400*2);
	dirty_buffer	= Next; Next += 2048;
//	fg_dirtybuffer	= Next; Next += 1024;
	CZ80Context		= Next; Next += sizeof(cz80_struc);
	MSM6295Context	= (int *)Next; Next += 4 * 0x1000 * sizeof(int);
//	MemEnd = Next;
}
//-------------------------------------------------------------------------------------------------------------------------------------
// Driver Init and Exit Functions
int NewsInit()
{
	INT32 TilePlaneOffsets[4] = { 0, 1, 2, 3 };
	INT32 TileXOffsets[8]     = { 0, 4, 8, 12, 16, 20, 24, 28 };
	INT32 TileYOffsets[8]     = { 0, 32, 64, 96, 128, 160, 192, 224 };

//	int nRet = 0;
	DrvInitSaturn();
	MemIndex();

#ifdef CACHE2
	memset(dirty_buffer,1,2048);
#endif

	make_lut();

	unsigned char *NewsTempGfx = (unsigned char*)(0x00240000);

	// Load Z80 Program Rom
	BurnLoadRom(NewsRom, 0, 1); //if (nRet != 0) return 1;
// Load, byte-swap and decode Tile Roms
	BurnLoadRom(NewsTempGfx + 0x00000, 1, 2); //if (nRet != 0) return 1;
	BurnLoadRom(NewsTempGfx + 0x00001, 2, 2); //if (nRet != 0) return 1;

	GfxDecode4Bpp(16384, 4, 8, 8, TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x100, NewsTempGfx, (void *)SS_CACHE);//NewsTiles);

	BurnLoadRom(MSM6295ROM, 3, 1); //if (nRet != 0) return 1;
	// Setup the Z80 emulation
#ifdef CZ80
	CZetInit2(1,CZ80Context);
	CZetOpen(0);
	CZetMapMemory(NewsRom,	0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory(NewsFgVideoRam,	0x8000, 0x8fff, MAP_ROM);
	CZetMapMemory(NewsRam,	0xe000, 0xffff, MAP_READ|MAP_WRITE);
//	CZetMemEnd();
	CZetSetReadHandler(NewsRead);
	CZetSetWriteHandler(NewsWrite);
//	CZetClose();
#endif

#ifdef RAZE
	z80_init_memmap();
	z80_map_fetch (0x0000, 0x7fff, NewsRom        ); 
	z80_map_read  (0x0000, 0x7fff, NewsRom        ); //1 read

	z80_map_fetch(0x8000, 0x87ff, NewsFgVideoRam  ); 
	z80_map_read (0x8000, 0x87ff, NewsFgVideoRam  ); //2 write
	z80_map_write(0x8000, 0x87ff, NewsFgVideoRam  ); //1 read 

	z80_map_fetch(0x8800, 0x8fff, NewsBgVideoRam  ); 
	z80_map_read (0x8800, 0x8fff, NewsBgVideoRam  ); //2 write
	z80_map_write(0x8800, 0x8fff, NewsBgVideoRam  ); //1 read 
	z80_map_fetch(0xe000, 0xffff, NewsRam        ); 
	z80_map_read (0xe000, 0xffff, NewsRam         ); //2 write
	z80_map_write(0xe000, 0xffff, NewsRam         ); //1 read 

	z80_add_write(0x9000, 0x91ff, 1, (void *)&NewsWrite);
	z80_add_write(0xc002, 0xc003, 1, (void *)&NewsWrite);
//	z80_add_write(0x8000, 0xffff, 1, (void *)&NewsWrite);
	z80_add_read (0xc000, 0xc002, 1, (void *)&NewsRead );
//	z80_add_read (0x0000, 0xffff, 1, (void *)&NewsRead );

	z80_end_memmap();   
#endif
	// Setup the OKIM6295 emulation
	MSM6295Init(0, 8000, 0, MSM6295Context);
	// Reset the driver
	NewsDoReset();

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initLayers()
{
    Uint16	CycleTb[]={
		0x1f56, 0xeeee, //A0
		0xeeee, 0xeeee,	//A1
		0xf5f2, 0x4efe,   //B0
		0xeeee, 0xeeee  //B1
//		0x4eff, 0x1fff, //B1
	};
 	SclConfig	scfg;
// 3 nbg
	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD; //2word
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.plate_addr[0] = (Uint32)SS_MAP2;
	scfg.plate_addr[1] = (Uint32)0;
	scfg.plate_addr[2] = (Uint32)0;
	scfg.plate_addr[3] = (Uint32)0;
	SCL_SetConfig(SCL_NBG1, &scfg);
// 3 nbg
	scfg.plate_addr[0] = (Uint32)SS_MAP;
	scfg.plate_addr[1] = (Uint32)0;
	scfg.plate_addr[2] = (Uint32)0;
	scfg.plate_addr[3] = (Uint32)0;
	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.dispenbl 		 = OFF;

//	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
//	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
//	scfg.datatype 		 = SCL_BITMAP;
//	scfg.mapover		 = SCL_OVER_0;
//	scfg.plate_addr[0]	 = (Uint32)SS_FONT;
  
// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);	
}
 //-------------------------------------------------------------------------------------------------------------------------------------
inline void initPosition()
{
	SCL_Open();
	ss_reg->n1_move_y = (16<<16);
	ss_reg->n1_move_x = 0;
	ss_reg->n2_move_y = (16);
	ss_reg->n2_move_x = 0;
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void initColors()
{
//	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));	
	colBgAddr =(Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
	colBgAddr2=(Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrvInitSaturn()
{
	SPR_InitSlaveSH();
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
	nBurnSprites  = 3;
	
	SS_MAP  = (Uint16 *)SCL_VDP2_VRAM_B1;
	SS_MAP2 = (Uint16 *)SCL_VDP2_VRAM_A1;
//	SS_FONT = NULL; //(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_CACHE = (Uint8  *)SCL_VDP2_VRAM_A0;				
	ss_BgPriNum  = (SclBgPriNumRegister *)SS_N0PRI;

//	SS_SET_N0PRIN(7);
	SS_SET_N2PRIN(5);
	SS_SET_N1PRIN(4);

	initLayers();
//	initSprites(264-1,216-1,0,0,8,-32);
	initPosition();
	initColors();

#ifdef PONY
	frame_x	= 0;
	nBurnFunction = sdrv_stm_vblank_rq;	
#endif	
}
//-------------------------------------------------------------------------------------------------------------------------------------
int NewsExit()
{
	/*
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
	NewsDoReset();
	MSM6295Exit(0);
#ifdef CZ80
//	CZetSetReadHandler(NULL);
//	CZetSetWriteHandler(NULL);
	CZetExit2();
#endif

#ifdef RAZE
	z80_stop_emulating();
	z80_add_write(0x9000, 0x91ff, 1, (void *)NULL);
	z80_add_write(0xc002, 0xc003, 1, (void *)NULL);
	z80_add_read (0xc000, 0xc002, 1, (void *)NULL);
#else
//	ZetExit();
#endif
*/
//	memset(SS_MAP,0,0x20000);
//	memset(SS_MAP2,0,0x20000);	
//	memset(NewsInputPort0,0x00,8);
//	NewsDip[0] = NewsInput[0]      = 0;
//	wait_vblank();

#ifdef PONY
memset(nSoundBuffer,0x00,0x8000);
remove_raw_pcm_buffer(pcm1);
#endif

	//cleanDATA();
	cleanBSS();

	nSoundBufferPos=0;
	return 0;
}

void NewsRenderFgLayer()
{
	UINT16 Code, Colour;
	UINT32 *map;
	UINT16 *lut_ptr=(UINT16 *)map_offset_lut;
	UINT16 *fg_ptr =(UINT16 *)NewsFgVideoRam;
	
	for (UINT16 TileIndex=0;TileIndex<0x400 ; TileIndex++)
	{
#ifdef CACHE
		if (dirty_buffer[TileIndex])
		{
			dirty_buffer[TileIndex] = 0;
#endif
			Code = *fg_ptr;
			Colour = Code >> 12;
			Code &= 0x0fff;
			
			map = ((UINT32*)SS_MAP)+(*lut_ptr);
			*map = Colour<<16 | Code;
			map[0x20] = 0xa0002;
#ifdef CACHE
		}
#endif

#ifdef CACHE2
		if (dirty_buffer[TileIndex+0x400])
		{
			dirty_buffer[TileIndex+0x400] = 0;
#endif
			Code = fg_ptr[0x400];			
			Colour = Code >> 12;
			Code &= 0x0fff;
			if ((Code & 0x0e00) == 0xe00) Code = (Code & 0x1ff) | (BgPic << 9);

			map = ((UINT32*)SS_MAP2)+(*lut_ptr);
			*map = Colour<<16 | Code;
#ifdef CACHE2
		}
#endif
		lut_ptr++;
		fg_ptr++;
	}
}
#ifdef PONY
void NewsFrame_old();

void NewsFrame()
{
	pcm1 = add_raw_pcm_buffer(0,SOUNDRATE,nBurnSoundLen*20);

	nSoundBuffer = (Sint16 *)(SNDRAM+(m68k_com->pcmCtrl[pcm1].hiAddrBits<<16) | m68k_com->pcmCtrl[pcm1].loAddrBits);
	pcm_stream_host(NewsFrame_old);
}

void NewsFrame_old()
#else
// Frame Function
void NewsFrame()
#endif
{

//	if (NewsReset) NewsDoReset();
	NewsMakeInputs();
#ifdef CZ80
//	CZetOpen(0);
//	CZetRun(8000000 / 60);
//	CZetRun(5500000 / 60);
	CZetRun(5400000 / 60);
	CZetRaiseIrq(0);
//	CZetClose();
#endif

#ifdef RAZE
	z80_emulate(5500000 / 60);
	z80_raise_IRQ(0);
	z80_emulate(0);
//	z80_lower_IRQ(0);
//	z80_emulate(0);
#endif

	SPR_RunSlaveSH((PARA_RTN*)NewsRenderFgLayer, NULL);
//	NewsRenderBgLayer();

#ifndef PONY
	signed short *nSoundBuffer = (signed short *)0x25a20000;
	MSM6295RenderVBT(0, &nSoundBuffer[nSoundBufferPos], SOUND_LEN);
	nSoundBufferPos+=SOUND_LEN;

	if(nSoundBufferPos>0x1200)
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos);
		PCM_Task(pcm); // bon emplacement
		nSoundBufferPos=0;
	}
#else
	signed short *nSoundBuffer2 = (signed short *)nSoundBuffer+(nSoundBufferPos<<1);

	MSM6295RenderVBT(0, &nSoundBuffer2[nSoundBufferPos], nBurnSoundLen);
	
	nSoundBufferPos+=nBurnSoundLen;
	
	if(nSoundBufferPos>=nBurnSoundLen*10)
	{
		pcm_play(pcm1, PCM_SEMI, 7);
		nSoundBufferPos=0;
	}
#endif	
	SPR_WaitEndSlaveSH();
	
#ifdef PONY
	frame_x++;
	
	 if(frame_x>=frame_y)
		wait_vblank();	
#endif	
}
