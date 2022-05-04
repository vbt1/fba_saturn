// 43-44 fps with CZ80 main & RAZE slave
// 17-18 fps with RAZE main & CZ80 slave
// FB Alpha Penguin-Kun Wars Driver Module
// Based on MAME Driver by David Haywood and Phil Stroffolino
//#pragma GCC optimize("Os")

#define CZ80 1
#define RAZE 1  // `EMULATE_R_REGISTER obligatoire
//#define USE_HANDLER_F 1
//#define USE_RAZE0 1
#define USE_RAZE1 1
#define CPU2_ENABLED 1

#include "d_sys1.h"
#include "d_sys1_common.c"

/** meilleure taille
.bss           0x00000000060f6760        0x0 /toolchain/bin/../lib/gcc/sh-elf/11.2.0/libgcc.a(_fpcmp_parts_df.o)
 *(COMMON)
                0x00000000060f6760                __bend = .
*/

//revoir WRITE_MEM16 et READ_MEM16
//modifier CZetSetReadHandler et CZetSetWriteHandler
//typedef void (*write_func)(unsigned short a, UINT8 d);
//static write_func p[36];

int ovlInit(char *szShortName)
{
	cleanBSS();
/*
	struct BurnDriver nBurnDrvWboyu = {
		"wboyu",  "sys1",
		"Wonder Boy (not encrypted)",
		WboyuRomInfo, WboyuRomName, WboyInputInfo, WboyuDIPInfo,
		WboyuInit, System1Exit, System1Frame
	};
*/
	struct BurnDriver nBurnDrvWbdeluxe = {
		"wbdeluxe", "sys1",
		"Wonder Boy Deluxe",
		WbdeluxeRomInfo, WbdeluxeRomName, WboyInputInfo, WbdeluxeDIPInfo,
		Wboy2uInit, System1Exit, System1Frame
	};

	struct BurnDriver nBurnDrvFlickys2 = {
		"flickys2", "sys1",
		"Flicky (128k, not encrypted)",
		Flickys2RomInfo, Flickys2RomName, FlickyInputInfo, FlickyDIPInfo,
		Flicks2Init, System1Exit, System1Frame
	};

	struct BurnDriver nBurnDrvMyhero = {
	"myhero", "sys1",
	"My Hero (US, not encrypted)",
	MyheroRomInfo, MyheroRomName, MyheroInputInfo, MyheroDIPInfo,
	MyheroInit, System1Exit, System1Frame
	};
/*
	struct BurnDriver nBurnDrvTeddybb = {
		"teddybb", "sys1",
		"TeddyBoy Blues (315-5115, New Ver.)",
		TeddybbRomInfo, TeddybbRomName, MyheroInputInfo, TeddybbDIPInfo,
		TeddybbInit, System1Exit, System1Frame
	};
*/
	struct BurnDriver nBurnDrvPitfall2u = {
		"pitfal2u", "sys1",
		"Pitfall II (not encrypted)",
		Pitfall2uRomInfo, Pitfall2uRomName, MyheroInputInfo, PitfalluDIPInfo,
		PitfalluInit, System1Exit, System1Frame
	};
 /*
	struct BurnDriver nBurnDrvFourdwarrio = {
		"4dwarrio", "sys1",
		"4-D Warriors (315-5162)",
		FourdwarrioRomInfo, FourdwarrioRomName, MyheroInputInfo, FourdwarrioDIPInfo,
		FourdwarrioInit, System1Exit, System1Frame
	};
*/
//	struct BurnDriver *fba_drv = 	(struct BurnDriver *)FBA_DRV;
//    if (strcmp(nBurnDrvWboyu.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvWboyu,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvFlickys2.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvFlickys2,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvMyhero.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvMyhero,sizeof(struct BurnDriver));
//    if (strcmp(nBurnDrvTeddybb.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvMyhero,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvPitfall2u.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvPitfall2u,sizeof(struct BurnDriver));
//    if (strcmp(nBurnDrvTeddybb.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvTeddybb,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvWbdeluxe.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvWbdeluxe,sizeof(struct BurnDriver));
//    if (strcmp(nBurnDrvFourdwarrio.szShortName, szShortName) == 0)		memcpy(fba_drv,&nBurnDrvFourdwarrio,sizeof(struct BurnDriver));

	ss_reg   = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
	
	return 0;
}

/*==============================================================================================
Decode Functions
===============================================================================================*/
/*
void teddybb_decode(void)
{
	const UINT8 convtable[32][4] =
	{
		//       opcode                   data                     address      
		//  A    B    C    D         A    B    C    D                           
		{ 0x20,0x28,0x00,0x08 }, { 0x80,0x00,0xa0,0x20 },	// ...0...0...0...0 
		{ 0x20,0x28,0x00,0x08 }, { 0xa0,0xa8,0x20,0x28 },	// ...0...0...0...1 
		{ 0x28,0x08,0xa8,0x88 }, { 0xa0,0x80,0xa8,0x88 },	// ...0...0...1...0 
		{ 0xa0,0xa8,0x20,0x28 }, { 0xa0,0x80,0xa8,0x88 },	// ...0...0...1...1 
		{ 0x20,0x28,0x00,0x08 }, { 0x28,0x08,0xa8,0x88 },	// ...0...1...0...0 
		{ 0xa0,0xa8,0x20,0x28 }, { 0xa0,0xa8,0x20,0x28 },	// ...0...1...0...1 
		{ 0xa0,0x80,0xa8,0x88 }, { 0x28,0x08,0xa8,0x88 },	// ...0...1...1...0 
		{ 0xa0,0xa8,0x20,0x28 }, { 0x28,0x08,0xa8,0x88 },	// ...0...1...1...1 
		{ 0x80,0x00,0xa0,0x20 }, { 0x80,0x00,0xa0,0x20 },	// ...1...0...0...0 
		{ 0xa0,0x20,0xa8,0x28 }, { 0xa0,0xa8,0x20,0x28 },	// ...1...0...0...1 
		{ 0xa0,0x20,0xa8,0x28 }, { 0xa0,0x80,0xa8,0x88 },	// ...1...0...1...0 
		{ 0xa0,0x80,0xa8,0x88 }, { 0xa0,0x80,0xa8,0x88 },	// ...1...0...1...1 
		{ 0x80,0x00,0xa0,0x20 }, { 0x20,0x28,0x00,0x08 },	// ...1...1...0...0 
		{ 0xa0,0xa8,0x20,0x28 }, { 0xa0,0x20,0xa8,0x28 },	// ...1...1...0...1 
		{ 0x80,0x00,0xa0,0x20 }, { 0xa0,0x80,0xa8,0x88 },	// ...1...1...1...0 
		{ 0xa0,0xa8,0x20,0x28 }, { 0xa0,0x20,0xa8,0x28 }	// ...1...1...1...1 
	};

	sega_decode(convtable);
}
*/
/*==============================================================================================
Driver Inits
===============================================================================================*/
/*
INT32 WboyuInit()
{
	INT32 nRet = 0;
	nRet = System1Init(3, 0x4000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
	CollisionFunction = NULL;
	return nRet;
}
*/
INT32 Wboy2uInit()
{
	INT32 nRet = 0;
	nRet = System1Init(6, 0x2000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
	CollisionFunction = NULL;
	return nRet;
}

/*
INT32 TeddybbInit()
{
	DecodeFunction = teddybb_decode;

	return System1Init(3, 0x4000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
}
*/

/*int SeganinuInit()
{
	return System1Init(3, 0x4000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
}

int NprincsuInit()
{
	return System1Init(6, 0x2000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
}
*/

INT32 Flicks2Init()
{
	return System1Init(2, 0x4000, 1, 0x2000, 6, 0x2000, 2, 0x4000, 1);
}

INT32 PitfalluInit()
{
	int nRet;
	
	nRet = System1Init(3, 0x4000, 1, 0x2000, 6, 0x2000, 2, 0x4000, 1);
	nCyclesTotal[0] = 3600000 / 60;
	
	return nRet;
}

INT32 MyheroInit()
{
	return System1Init(3, 0x4000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
}

/* INT32 FourdwarrioInit()
{
	DecodeFunction = fdwarrio_decode;
	return System1Init(3, 0x4000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
}*/
/*==============================================================================================
Graphics Rendering
===============================================================================================*/
inline void renderSpriteCache(int *values);

static inline void DrawSprite(unsigned int Num, SprSpCmd *ss_spritePtr,UINT8 *SpriteBase)
{
	UINT32 Src  = __builtin_bswap16(*((UINT16 *)SpriteBase+3));
	UINT16 Skip = __builtin_bswap16(*((UINT16 *)SpriteBase+2));
	UINT32 Bank = 0x8000 * (((SpriteBase[3] & 0x80) >> 7) + ((SpriteBase[3] & 0x40) >> 5));
	Bank &= System1SpriteRomSize;

	unsigned int Height = SpriteBase[1] - SpriteBase[0];
	unsigned int Width = (Skip + (7)) & ~(7);
	unsigned int addr = Bank + ((Src + Skip) & 0x7fff);

	if (spriteCache[addr]==0xFFFF)
	{
		int values[] ={Src,Height,Skip,Width, Bank};
		spriteCache[addr]=nextSprite;
		renderSpriteCache(values);
		nextSprite+=(Width*Height)/8;
	}

	ss_spritePtr->ax		= 8+(((SpriteBase[3] & 0x01) << 8) + SpriteBase[2] )/2;
	ss_spritePtr->ay		= SpriteBase[0] + 1;
	ss_spritePtr->charSize	= (Width<<6) + Height;
	ss_spritePtr->color		= COLADDR_SPR | ((Num)<<2);
	ss_spritePtr->charAddr	= 0x220+spriteCache[addr];

	if(CollisionFunction)
	{
		int values[] ={ss_spritePtr->ax,ss_spritePtr->ay,Skip,Height,Num};
		updateCollisions(values);
	}
}

inline void System1Render()
{
	ss_reg->n2_move_x = 256-(((System1ScrollX[0] >> 1) + ((System1ScrollX[1] & 1) << 7) + 6) & 0xff);
	ss_reg->n2_move_y = System1ScrollY[0];
	System1DrawSprites(System1SpriteRam);
}