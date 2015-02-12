// FB Alpha Penguin-Kun Wars Driver Module
// Based on MAME Driver by David Haywood and Phil Stroffolino
#define CZ80 1
//#define RAZE 1  // `EMULATE_R_REGISTER obligatoire

#include "d_pkunwar.h"

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvpkunwar = {
		"pkunw", NULL,
		"Penguin-Kun Wars (US)\0",
		pkunwarRomInfo, pkunwarRomName, DrvInputInfo, DrvDIPInfo,
		DrvInit, DrvExit, DrvFrame, NULL//, DrvDraw//, NULL
	};

	memcpy(shared,&nBurnDrvpkunwar,sizeof(struct BurnDriver));
	ss_reg          = (SclNorscl *)SS_REG;
}

// Memory Handlers

/*static*/ unsigned char __fastcall pkunwar_read(unsigned short address)
{
	switch (address)
	{
		case 0xa001:
			return AY8910Read(0);
		break;

		case 0xa003:
			return AY8910Read(1);
		break;
	}

	return 0;
}

/*static*/ void __fastcall pkunwar_write(unsigned short address, unsigned char data)
{
	switch (address)
	{
		case 0xa000:
			AY8910Write(0, 0, data);
		break;

		case 0xa001:
			AY8910Write(0, 1, data);
		break;

		case 0xa002:
			AY8910Write(1, 0, data);
		break;

		case 0xa003:
			AY8910Write(1, 1, data);
		break;
	}
}

/*static*/ void __fastcall pkunwar_out(unsigned short address, unsigned char data)
{
	address &= 0xff;

//	if (address == 0) flipscreen = data & 1;
}


//-------------------------------------------------------------------------------------------------
// AY8910 Ports


/*static*/ unsigned char pkunwar_port_0(unsigned int a)
{
	unsigned char ret = 0x7f | (vblank ^= 0x80);
	unsigned int i;
	for (i = 0; i < 8; i++) {
		ret ^= DrvJoy1[i] << i;
	}

	return ret;
}

/*static*/ unsigned char pkunwar_port_1(unsigned int a)
{
	unsigned char ret = 0xff;
	unsigned int i;
	for (i = 0; i < 8; i++) {
		ret ^= DrvJoy2[i] << i;
	}

	return ret;
}

/*static*/ unsigned char pkunwar_port_2(unsigned int a)
{
	return 0xff;
}

/*static*/ unsigned char pkunwar_port_3(unsigned int a)
{
	return DrvDips;
}


//-------------------------------------------------------------------------------------------------
// Initialization Routines


/*static*/ int DrvDoReset()
{
	DrvReset = 0;

	memset (Rom + 0x8000, 0, 0x1000);
	memset (Rom + 0xc000, 0, 0x0800);

//	flipscreen = 0;
#ifdef RAZE
	z80_reset();
#else
	CZetOpen(0);
	CZetReset();
	CZetClose();
#endif
	AY8910Reset(0);
	AY8910Reset(1);

	return 0;
}

/*static*/ void pkunwar_palette_init(unsigned char *Prom)
{
	unsigned int i;
	for (i = 0; i < 0x200; i++)
	{
		int entry;
		int intensity,r,g,b;

		if ((i & 0xf) == 1)
		{
			entry = ((i & 0xf0) >> 4) | ((i & 0x100) >> 4);
		}
		else
		{
			entry = ((i & 0x0f) >> 0) | ((i & 0x100) >> 4);
		}

		intensity = Prom[entry] & 0x03;

		r = (((Prom[entry] >> 0) & 0x0c) | intensity) * 0x11;
		g = (((Prom[entry] >> 2) & 0x0c) | intensity) * 0x11;
		b = (((Prom[entry] >> 4) & 0x0c) | intensity) * 0x11;

        r =  (r >>3);
        g =  (g >>3);
        b =  (b >>3);

		colAddr[i] = colBgAddr[i] = RGB(r,g,b);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*inline*/ /*static*/ int readbit(const UINT8 *src, int bitnum)
{
	return src[bitnum / 8] & (0x80 >> (bitnum % 8));
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void GfxDecode4Bpp(int num, int numPlanes, int xSize, int ySize, int planeoffsets[], int xoffsets[], int yoffsets[], int modulo, unsigned char *pSrc, unsigned char *pDest)
{
	int c;
//	wait_vblank();
	for (c = 0; c < num; c++) {
		int plane, x, y;
	
		UINT8 *dp = pDest + (c * (xSize/2) * ySize);
		memset(dp, 0, (xSize/2) * ySize);
	
		for (plane = 0; plane < numPlanes; plane++) {
			int planebit = 1 << (numPlanes - 1 - plane);
			int planeoffs = (c * modulo) + planeoffsets[plane];
		
			for (y = 0; y < ySize; y++) {
				int yoffs = planeoffs + yoffsets[y];
				dp = pDest + (c * (xSize/2) * ySize) + (y * (xSize/2));
			
				for (x = 0; x < xSize; x+=2) {
					if (readbit(pSrc, yoffs + xoffsets[x])) dp[x>>1] |= (planebit&0x0f)<<4;
					if (readbit(pSrc, yoffs + xoffsets[x+1])) dp[x>>1] |= (planebit& 0x0f);
					//(NewsTiles[i+1]& 0x0f)| ((NewsTiles[i]& 0x0f) <<4)
				}
			}
		}
	}
}

/*static*/ void pkunwar_gfx_decode(unsigned char *Gfx)
{
	/*static*/ int PlaneOffsets[4] = { 0, 1, 2, 3 };

	/*static*/ int XOffsets[16] = {
		0x00000, 0x00004, 0x40000, 0x40004, 0x00008, 0x0000c, 0x40008, 0x4000c,
		0x00080, 0x00084, 0x40080, 0x40084, 0x00088, 0x0008c, 0x40088, 0x4008c
	};

	/*static*/ int YOffsets[16] = {
		0x000, 0x010, 0x020, 0x030, 0x040, 0x050, 0x060, 0x070,
		0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170
	};
	unsigned long i;

	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	GfxDecode4Bpp(0x800, 4,  8,  8, PlaneOffsets, XOffsets, YOffsets, 0x080, Gfx, cache);
	GfxDecode4Bpp(0x200, 4, 16, 16, PlaneOffsets, XOffsets, YOffsets, 0x200, Gfx, &ss_vram[0x1100]);
}

/*static*/ int LoadRoms()
{
	unsigned int i;
	UINT8 *tmp = (UINT8*)0x00200000;
	memset(tmp,0x00,0x20000);

	if (BurnLoadRom(Rom  + 0x0000, 0, 1)) return 1;
	if (BurnLoadRom(Rom  + 0x4000, 1, 1)) return 1;
	if (BurnLoadRom(Rom  + 0xe000, 2, 1)) return 1;

	if (BurnLoadRom(Gfx0 + 0x0000, 3, 1)) return 1;
	if (BurnLoadRom(Gfx0 + 0x4000, 4, 1)) return 1;
	if (BurnLoadRom(Gfx0 + 0x8000, 5, 1)) return 1;
	if (BurnLoadRom(Gfx0 + 0xc000, 6, 1)) return 1;

	for (i = 0; i < 8; i++) {
		int j = ((i & 1) << 2) | ((i >> 1) & 3);
		memcpy (tmp + j * 0x2000, Gfx0 + i * 0x2000, 0x2000);
	}

	pkunwar_gfx_decode(tmp);

	if (BurnLoadRom(tmp +  0x0000, 7, 1)) return 1;

	pkunwar_palette_init(tmp);
	memset(tmp,0x00,0x20000);
	tmp = NULL;

	return 0;
}

/*static*/ int DrvInit()
{
	DrvInitSaturn();

//	Mem = (unsigned char*)malloc(0x10000 + 0x20000 * 2 + 0x200 * sizeof(int));
	Mem = (unsigned char*)malloc(0x30000 + SOUND_LEN * 6 * sizeof(short));

	if (Mem == NULL) {
		return 1;
	}

	Rom  = Mem + 0x00000;
	Gfx0 = Mem + 0x10000;
//	Gfx1 = Mem + 0x30000;
	pFMBuffer = Mem + 0x30000;

	if (LoadRoms()) return 1;

#ifdef RAZE
	z80_init_memmap();
	z80_map_fetch (0x0000, 0x7fff, Rom + 0x0000); 
	z80_map_read  (0x0000, 0x7fff, Rom + 0x0000); //2 read

	z80_map_fetch(0x8000, 0x8fff, Rom + 0x8000); 
	z80_map_read (0x8000, 0x8fff, Rom + 0x8000); //1 write 
	z80_map_write(0x8000, 0x8fff, Rom + 0x8000); //2 read 
	z80_map_fetch(0xc000, 0xcfff, Rom + 0xc000); 
	z80_map_read (0xc000, 0xcfff, Rom + 0xc000); //1 write 
	z80_map_write(0xc000, 0xcfff, Rom + 0xc000); //2 read 
	z80_map_fetch(0xe000, 0xffff, Rom + 0xe000); 
	z80_map_read (0xe000, 0xffff, Rom + 0xe000); //2 read
//	z80_map_write(0xe000, 0xffff, Rom + 0xe000); //
	z80_add_write(0xa000, 0xa003, 1, (void *)&pkunwar_write);
	z80_add_read(0xa000,  0xa003, 1, (void *)&pkunwar_read);
	z80_end_memmap();   
	z80_set_out((void (*)(unsigned short int, unsigned char))&pkunwar_out);
#else
	CZetInit(1);
	CZetOpen(0);
	CZetSetOutHandler(pkunwar_out);
	CZetSetReadHandler(pkunwar_read);
	CZetSetWriteHandler(pkunwar_write);
	CZetMapArea(0x0000, 0x7fff, 0, Rom + 0x00000);
	CZetMapArea(0x0000, 0x7fff, 2, Rom + 0x00000);
	CZetMapArea(0x8000, 0x8fff, 0, Rom + 0x08000);
	CZetMapArea(0x8000, 0x8fff, 1, Rom + 0x08000);
	CZetMapArea(0xc000, 0xc7ff, 0, Rom + 0x0c000);
	CZetMapArea(0xc000, 0xc7ff, 1, Rom + 0x0c000);
	CZetMapArea(0xc000, 0xc7ff, 2, Rom + 0x0c000);
	CZetMapArea(0xe000, 0xffff, 0, Rom + 0x0e000);
	CZetMapArea(0xe000, 0xffff, 2, Rom + 0x0e000);
	CZetMemEnd();
	CZetClose();
#endif
	pAY8910Buffer[0] = pFMBuffer + SOUND_LEN * 0;
	pAY8910Buffer[1] = pFMBuffer + SOUND_LEN * 1;
	pAY8910Buffer[2] = pFMBuffer + SOUND_LEN * 2;
	pAY8910Buffer[3] = pFMBuffer + SOUND_LEN * 3;
	pAY8910Buffer[4] = pFMBuffer + SOUND_LEN * 4;
	pAY8910Buffer[5] = pFMBuffer + SOUND_LEN * 5;

	AY8910Init(0, 1500000, nBurnSoundRate, &pkunwar_port_0, &pkunwar_port_1, NULL, NULL);
	AY8910Init(1, 1500000, nBurnSoundRate, &pkunwar_port_2, &pkunwar_port_3, NULL, NULL);
	DrvDoReset();

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initLayers()
{
    Uint16	CycleTb[]={
		0x4eff, 0x1fff, //A0
		0xffff, 0xffff,	//A1
		0x2f6f,0xf5ff,   //B0
		0xffff, 0xffff  //B1
	};
 	SclConfig	scfg;

	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize   = SCL_PN1WORD;
	scfg.flip              = SCL_PN_12BIT; 
	scfg.platesize     = SCL_PL_SIZE_2X2; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.patnamecontrl =  0x000c;// VRAM B1 のオフセット 
	scfg.plate_addr[0] = SCL_VDP2_VRAM_A0;
	SCL_SetConfig(SCL_NBG1, &scfg);

	scfg.plate_addr[0] = SCL_VDP2_VRAM_B0;
	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.dispenbl 	   = ON;
	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
//	scfg.coltype 	   = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	       = SCL_OVER_0;
	scfg.plate_addr[0] = SCL_VDP2_VRAM_A1;
	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initPosition()
{
	SCL_Open();
	ss_reg->n1_move_y =  (32<<16) ;
	ss_reg->n1_move_x =  (-8<<16) ;
	ss_reg->n2_move_x =   (-8);
	ss_reg->n2_move_y =  32 ;
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initColors()
{
	colBgAddr = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	colAddr = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
	SCL_AllocColRam(SCL_NBG2,OFF);
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn()
{
	cleanSprites();
	SPR_InitSlaveSH();
	nBurnSprites  = 67;

//	ss_map  = (Uint16 *)SS_MAP;
//	ss_map2= (Uint16 *)SS_MAP2;
//	ss_font  = (Uint16 *)SS_FONT;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite           = (SprSpCmd *)SS_SPRIT;

	SS_CACHE      = cache = (Uint8 *)SCL_VDP2_VRAM_B1;

	SS_SET_S0PRIN(5);
	SS_SET_N0PRIN(7);
	SS_SET_N2PRIN(4);
	SS_SET_N1PRIN(6);

	initColors();
	initLayers();
	initPosition();
	initSprites(264-1,216-1,0,0,8,-32);
	drawWindow(0,192,192,0,62);
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
	//*(unsigned int*)OPEN_CSH_VAR(SOUND_LEN) = 128;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void cleanSprites()
{
	unsigned int delta;	
	for (delta=3; delta<nBurnSprites; delta++)
	{
		ss_sprite[delta].charSize   = 0;
		ss_sprite[delta].charAddr   = 0;
		ss_sprite[delta].ax   = 0;
		ss_sprite[delta].ay   = 0;
	} 
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ int DrvExit()
{
#ifndef RAZE
	CZetExit();
#endif
	AY8910Exit(0);
	AY8910Exit(1);

	Rom = Gfx0 = /*Gfx1 =*/ NULL;
	nBurnSprites=128;
	cleanSprites();

	for (int i = 0; i < 6; i++) {
		pAY8910Buffer[i] = NULL;
	}

//	free (pFMBuffer);
	pFMBuffer = NULL;
	free (Mem);
	Mem = NULL;

	vblank /*= flipscreen */= 0;

	return 0;

}


//-------------------------------------------------------------------------------------------------
// Drawing Routines
/*static*/ int DrawChars(int priority)
{
	int offs;

	if(priority==0)
		ss_map =(Uint16 *)SCL_VDP2_VRAM_B0;
	else
		ss_map =(Uint16 *)SCL_VDP2_VRAM_A0;

	for (offs = 0x3c0 - 1;offs >= 0x40;offs--)
	{
		int sx,sy;
		sx = (offs & 0x1f);
		sy = ((offs >> 5) & 0x1f)<<6;

		if (Rom[0x8c00 + offs] & 0x08 || !priority)
		{
			int num,color;
			num = Rom[0x8800 + offs] | ((Rom[0x8c00 + offs] & 7) << 8);
			color = 0x100 | (Rom[0x8c00 + offs] & 0xf0);
			ss_map[sx|sy] = (color>>4) <<12 | num;
		}
		else
		{
			ss_map[sx|sy] = 32;
		}
	}
   
	return 0;
}

/*static*/ int DrvDraw()
{ 
	int offs;

	DrawChars(0);
//	SPR_RunSlaveSH((PARA_RTN*)DrawChars, 0);
	cleanSprites();

	for (offs = 0;offs < 0x800;offs += 32)
	{
		int sx,sy,num,color,flip;
		unsigned int delta;

		sx = Rom[0x8001 + offs];
		sy = Rom[0x8002 + offs];
		delta = (offs>>5)+3;

		if (sy < 16 || sy > 215) 
		{	
//			ss_sprite[3+delta].charSize=0;
			continue;
		}
//		sy -= 32;
		flip  = (Rom[0x8000 + offs] & 0x03)<<4;
		num   = ((Rom[0x8000 + offs] & 0xfc) >> 2) + ((Rom[0x8003 + offs] & 7) << 6);
		color = Rom[0x8003 + offs] & 0xf0;

		ss_sprite[delta].ax = sx;
		ss_sprite[delta].ay = sy;
		ss_sprite[delta].color      = color;
		ss_sprite[delta].control    = ( JUMP_NEXT | FUNC_NORMALSP | flip);
		ss_sprite[delta].drawMode   = ( COLOR_0 | ECD_DISABLE | COMPO_REP);		
		ss_sprite[delta].charSize   = 0x210;  //0x100 16*16
		ss_sprite[delta].charAddr   = 0x220+(num<<4);
	}
	DrawChars(1);

	return 0;
}


/*static*/ int DrvFrame()
{
	if (DrvReset) {
		DrvDoReset();
	}

	vblank = 0;
#ifdef RAZE
	z80_emulate(3000000 / 60);
	z80_raise_IRQ(0);
	z80_emulate(0);
	z80_lower_IRQ(0);
	z80_emulate(0);
#else
	CZetOpen(0);
	CZetRun(3000000 / 60);
	CZetRaiseIrq(0);
	CZetClose();
#endif

	SPR_RunSlaveSH((PARA_RTN*)updateSound, NULL);
//	updateSound();
	DrvDraw();


//PCM_Task(pcm);
	SPR_WaitEndSlaveSH();  
	return 0;
}


//-------------------------------------------------------------------------------------------------
/*static*/ void updateSound()
{
	int nSample;
	int n;
	unsigned int deltaSlave;//soundLenSlave;//,titiSlave;
	signed short *nSoundBuffer = (signed short *)0x25a20000;
	deltaSlave    = *(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);

//	soundLenSlave = SOUND_LEN);
	AY8910Update(0, &pAY8910Buffer[0], SOUND_LEN);
	AY8910Update(1, &pAY8910Buffer[3], SOUND_LEN);

	for (n = 0; n < SOUND_LEN; n++) 
	{
		nSample  = pAY8910Buffer[0][n]; // >> 2;
		nSample += pAY8910Buffer[1][n]; // >> 2;
		nSample += pAY8910Buffer[2][n]; // >> 2;
		nSample += pAY8910Buffer[3][n]; // >> 2;
		nSample += pAY8910Buffer[4][n]; // >> 2;
		nSample += pAY8910Buffer[5][n]; // >> 2;

		nSample /=4;

		if (nSample < -32768) 
		{
			nSample = -32768;
		} 
		else 
		{
			if (nSample > 32767) 
			{
				nSample = 32767;
			}
		}
		nSoundBuffer[deltaSlave + n] = nSample;//pAY8910Buffer[5][n];//nSample;
	}

	if(deltaSlave>=RING_BUF_SIZE/2)
	{
		deltaSlave=0;
		PCM_Task(pcm); // bon emplacement
	}

	deltaSlave+=SOUND_LEN;

	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = deltaSlave;
}

