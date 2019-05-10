// tester sur saturn ce qui est plus rapide entre utiliser le 2eme cpu et tout mettre dans une seule boucle
// a la limite en meme temps essayer de de pas remplir l'image de fond si un puyo est affiché dessus
// FB Alpha - "News" Driver
#define CZ80 1
//#define RAZE 1
#define SC_RELEASE 1
#define CACHE2 1
#define CACHE 1
#include "d_news.h"

int ovlInit(char *szShortName)
{
	cleanBSS();

	struct BurnDriver nBurnDrvNewsa = {
		"newsa", "news",
		"News (set 2)\0",
		NewsaRomInfo, NewsaRomName, NewsInputInfo, NewsaDIPInfo,
		NewsInit, NewsExit, NewsFrame, NULL//, NULL
	};

	struct BurnDriver nBurnDrvNews = {
		"news", NULL,
		"News (set 1)\0",
		NewsRomInfo, NewsRomName, NewsInputInfo, NewsDIPInfo,
		NewsInit, NewsExit, NewsFrame, NULL//, NULL
	};

    if (strcmp(nBurnDrvNewsa.szShortName, szShortName) == 0)
		memcpy(shared,&nBurnDrvNewsa,sizeof(struct BurnDriver));
	else
		memcpy(shared,&nBurnDrvNews,sizeof(struct BurnDriver));
	ss_reg          = (SclNorscl *)SS_REG;
}

/*static*/ void NewsMakeInputs()
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
/*static*/ int NewsDoReset()
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

	return 0;
}

/*static*/ void make_lut(void)
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

	j=0;
	for (UINT32 my = 0; my < 64; my+=2) 
	{
		for (UINT32 mx = 0; mx < 64; mx+=2) 
		{
			map_offset_lut[j] = (mx|(my<<6));
			ss_map[map_offset_lut[j]+0x40] =  0x00;
			ss_map[map_offset_lut[j]+0x41] =  0x01;
			j++;
		}
	}
}

/*static*/ unsigned char __fastcall NewsRead(unsigned short a)
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
/*static*/ void __fastcall NewsWrite(unsigned short a, unsigned char d)
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
				memset(bg_dirtybuffer,1,1024);
			}
#else
				BgPic = d;
#endif
			return;
		}
	}

#ifdef CACHE2
	if (a >= 0x8800 && a <= 0x8fff) 
	{
		if(RamStart[a]!=d)
		{
			RamStart[a]=d;
			bg_dirtybuffer[(a>>1)&0x3ff] = 1; 
		}
		return;
	}

#endif
#ifdef CACHE
	if (a >= 0x8000 && a <= 0x87ff) 
	{
		if(RamStart[a]!=d)
		{
			RamStart[a]=d;
			fg_dirtybuffer[(a>>1)&0x3ff] = 1; 
		}
		return;
	}
#endif

	if (a >= 0x9000 && a <= 0x91ff) 
	{
		if(RamStart[a]!=d)
		{
			RamStart[a] = d;
			a&=0x1ff;
			colBgAddr2[a / 2] = colBgAddr[a / 2] = cram_lut[NewsPaletteRam[a | 1] | (NewsPaletteRam[a & ~1] << 8)];
		}
		return;
	}
}

// Function to Allocate and Index required memory
/*static*/ int MemIndex()
{
	unsigned char *Next; Next = Mem;

	NewsRom			= Next; Next += 0x10000;
//	MSM6295ROM		= Next; Next += 0x40000;

	NewsRam			= Next; Next += 0x02000;
	RamStart		= Next-0x8000;
	NewsFgVideoRam	= Next; Next += 0x00800;
	NewsBgVideoRam	= Next; Next += 0x00800;
	NewsPaletteRam	= Next; Next += 0x00200;
	CZ80Context		= Next; Next += sizeof(cz80_struc);
	pBuffer			= (int *)Next; Next += nBurnSoundRate * sizeof(int);
	cram_lut		= (UINT16*)Next; Next += (4096*2);
	map_offset_lut	= (UINT16*)Next; Next += (0x400*2);
	bg_dirtybuffer	= Next; Next += 1024;
	fg_dirtybuffer	= Next; Next += 1024;
	MSM6295Context	= (int *)Next; Next += 4 * 0x1000 * sizeof(int);
	MemEnd = Next;

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
// Driver Init and Exit Functions
/*static*/ int NewsInit()
{
 UINT32 TilePlaneOffsets[4] = { 0, 1, 2, 3 };
 UINT32 TileXOffsets[8]       = { 0, 4, 8, 12, 16, 20, 24, 28 };
 UINT32 TileYOffsets[8]       = { 0, 32, 64, 96, 128, 160, 192, 224 };

	int nRet = 0;
	DrvInitSaturn();

	// Allocate and Blank all required memory
	Mem = NULL;
	MemIndex();
	MemEnd - (unsigned char *)0;
	if ((Mem = (unsigned char *)malloc(MALLOC_MAX)) == NULL) 
	{	
		return 1;
	}
	memset(Mem, 0, MALLOC_MAX);
	MemIndex();
#ifdef CACHE2
	memset(bg_dirtybuffer,1,1024);
#endif
#ifdef CACHE
	memset(fg_dirtybuffer,1,1024);
#endif
	make_lut();

	unsigned char *NewsTempGfx = (unsigned char*)(0x00240000);

	// Load Z80 Program Rom
	nRet = BurnLoadRom(NewsRom, 0, 1); if (nRet != 0) return 1;
// Load, byte-swap and decode Tile Roms
	nRet = BurnLoadRom(NewsTempGfx + 0x00000, 1, 2); //if (nRet != 0) return 1;
	nRet = BurnLoadRom(NewsTempGfx + 0x00001, 2, 2); //if (nRet != 0) return 1;

	GfxDecode4Bpp(16384, 4, 8, 8, TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x100, NewsTempGfx, cache);//NewsTiles);
	NewsTempGfx = NULL;

	MSM6295ROM = (unsigned char *)0x00200000;
	memset(MSM6295ROM,0x00,0x40000);

	nRet = BurnLoadRom(MSM6295ROM, 3, 1); //if (nRet != 0) return 1;
	// Setup the Z80 emulation
#ifdef CZ80
	CZetInit2(1,CZ80Context);
	CZetOpen(0);
	CZetMapArea(0x0000, 0x7fff, 0, NewsRom        );
	CZetMapArea(0x0000, 0x7fff, 2, NewsRom        );
	CZetMapArea(0x8000, 0x87ff, 0, NewsFgVideoRam );
#ifndef CACHE
	CZetMapArea(0x8000, 0x87ff, 1, NewsFgVideoRam );
#endif
	CZetMapArea(0x8000, 0x87ff, 2, NewsFgVideoRam );
	CZetMapArea(0x8800, 0x8fff, 0, NewsBgVideoRam );
#ifndef CACHE2
	CZetMapArea(0x8800, 0x8fff, 1, NewsBgVideoRam );
#endif
	CZetMapArea(0x8800, 0x8fff, 2, NewsBgVideoRam );
	CZetMapArea(0xe000, 0xffff, 0, NewsRam        );
	CZetMapArea(0xe000, 0xffff, 1, NewsRam        );
	CZetMapArea(0xe000, 0xffff, 2, NewsRam        );
	CZetMemEnd();
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
	MSM6295Init(0, 8000, 100, 0, MSM6295Context);
	// Reset the driver
	NewsDoReset();

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initLayers()
{
    Uint16	CycleTb[]={
		0x1f56, 0xffff, //A0
		0xffff, 0xffff,	//A1
		0xf5f2, 0x4eff,   //B0
		0xffff, 0xffff  //B1
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
	scfg.plate_addr[0] = (Uint32)ss_map2;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG1, &scfg);
// 3 nbg
	scfg.plate_addr[0] = (Uint32)ss_map;
	scfg.plate_addr[1] = (Uint32)ss_map;
	scfg.plate_addr[2] = (Uint32)ss_map;
	scfg.plate_addr[3] = (Uint32)ss_map;

	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.dispenbl 		 = OFF;

	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 		 = SCL_BITMAP;
	scfg.mapover		 = SCL_OVER_0;
	scfg.plate_addr[0]	 = (Uint32)SS_FONT;
  
// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);	
}
 //-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initPosition()
{
	SCL_Open();
	ss_reg->n1_move_y = (16<<16);
	ss_reg->n1_move_x = 0;
	ss_reg->n2_move_y = (16);
	ss_reg->n2_move_x = 0;
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initColors()
{
	colBgAddr =(Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
	colBgAddr2=(Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
	nBurnSprites  = 3;
	
	SS_MAP  = ss_map   = (Uint16 *)SCL_VDP2_VRAM_B1;
	SS_MAP2 = ss_map2 = (Uint16 *)SCL_VDP2_VRAM_A1;
	SS_FONT =  ss_font =  NULL; //(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_CACHE = cache     = (Uint8  *)SCL_VDP2_VRAM_A0;				
	ss_BgPriNum     = (SclSpPriNumRegister *)SS_N0PRI;


	SS_SET_N0PRIN(7);
	SS_SET_N2PRIN(5);
	SS_SET_N1PRIN(4);

	initLayers();
	initPosition();

	initColors();

//	drawWindow(0,240,0,0,64);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ int NewsExit()
{
	SPR_InitSlaveSH();
	MSM6295Exit(0);
#ifdef CZ80
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
	
	nSoundBufferPos=0;
	PCM_Task(pcm);
	MSM6295ROM = NULL;
	MSM6295Context = NULL;
	CZ80Context	= MemEnd = RamStart = NewsRom = NewsRam = NULL;
	NewsFgVideoRam = NewsBgVideoRam = NewsPaletteRam = NULL;
	bg_dirtybuffer = fg_dirtybuffer = NULL;
	cram_lut = map_offset_lut = NULL;
	pBuffer = NULL;
	free(Mem);
	Mem = NULL;
	BgPic = 1;
	return 0;
}

// Graphics Emulation
/*static*/ void NewsRenderBgLayer()
{
	UINT32 Code, Colour, x;
	
	 for (UINT32 TileIndex=0;TileIndex<0x400 ; TileIndex++)
	 {
#ifdef CACHE2
		if (bg_dirtybuffer[TileIndex])
		{
			bg_dirtybuffer[TileIndex] = 0;
#endif
			Code = (NewsBgVideoRam[TileIndex * 2] << 8) | NewsBgVideoRam[TileIndex * 2 + 1];
			Colour = Code >> 12;
			Code &= 0x0fff;
			if ((Code & 0x0e00) == 0xe00) Code = (Code & 0x1ff) | (BgPic << 9);

			x = map_offset_lut[TileIndex];
			ss_map2[x] = Colour;
			ss_map2[x+1] =  Code;

#ifdef CACHE2
		}
#endif
	}
}

/*static*/ void NewsRenderFgLayer()
{
	UINT32 Code, Colour, x;

	for (UINT32 TileIndex=0;TileIndex<0x400 ; TileIndex++)
	{

#ifdef CACHE
		if (fg_dirtybuffer[TileIndex])
		{
			fg_dirtybuffer[TileIndex] = 0;
#endif
			Code = (NewsFgVideoRam[TileIndex * 2] << 8) | NewsFgVideoRam[TileIndex * 2 + 1];
			Colour = Code >> 12;
			Code &= 0x0fff;

			x = map_offset_lut[TileIndex];
			ss_map[x] = Colour;
			ss_map[x+1] =  Code;

			ss_map[x+0x40] =  10;
			ss_map[x+0x41] =  0x02;

#ifdef CACHE
		}
#endif
	}

}

// Frame Function
/*static*/ int NewsFrame()
{

	if (NewsReset) NewsDoReset();
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
	NewsRenderBgLayer();
	signed short *nSoundBuffer = (signed short *)0x25a20000;
	MSM6295RenderVBT(0, &nSoundBuffer[nSoundBufferPos], SOUND_LEN);
	nSoundBufferPos+=SOUND_LEN;

	if(nSoundBufferPos>0x1B00)
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos);
		PCM_Task(pcm); // bon emplacement
		nSoundBufferPos=0;
	}
	SPR_WaitEndSlaveSH();  
	return 0;
}
