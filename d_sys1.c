// 43-44 fps with CZ80 main & RAZE slave
// 17-18 fps with RAZE main & CZ80 slave
// FB Alpha Penguin-Kun Wars Driver Module
// Based on MAME Driver by David Haywood and Phil Stroffolino
#define CZ80 1
#define RAZE 1  // `EMULATE_R_REGISTER obligatoire
//#define USE_RAZE0 1
#define USE_RAZE1 1
#define CPU2_ENABLED 1

#include "d_sys1.h"
#include "d_sys1_common.c"

//revoir WRITE_MEM16 et READ_MEM16
//modifier CZetSetReadHandler et CZetSetWriteHandler
//typedef void (*write_func)(unsigned short a, UINT8 d);
//static write_func p[36];

int ovlInit(char *szShortName)
{
	cleanBSS();

	struct BurnDriver nBurnDrvWboyu = {
		"wboyu",  "sys1",
		"Wonder Boy (not encrypted)\0",
		WboyuRomInfo, WboyuRomName, WboyInputInfo, WboyuDIPInfo,
		WboyuInit, System1Exit, System1Frame
	};

	struct BurnDriver nBurnDrvWbdeluxe = {
		"wbdeluxe", "sys1",
		"Wonder Boy Deluxe\0",
		WbdeluxeRomInfo, WbdeluxeRomName, WboyInputInfo, WbdeluxeDIPInfo,
		Wboy2uInit, System1Exit, System1Frame
	};

	struct BurnDriver nBurnDrvFlickys2 = {
		"flickys2", "sys1",
		"Flicky (128k, System 2, not encrypted)\0",
		Flickys2RomInfo, Flickys2RomName, FlickyInputInfo, FlickyDIPInfo,
		Flicks2Init, System1Exit, System1Frame
	};

	struct BurnDriver nBurnDrvMyhero = {
	"myhero", "sys1",
	"My Hero (US, not encrypted)\0",
	MyheroRomInfo, MyheroRomName, MyheroInputInfo, MyheroDIPInfo,
	MyheroInit, System1Exit, System1Frame
	};

	struct BurnDriver nBurnDrvTeddybb = {
		"teddybb", "sys1",
		"TeddyBoy Blues (315-5115, New Ver.)\0",
		TeddybbRomInfo, TeddybbRomName, MyheroInputInfo, TeddybbDIPInfo,
		TeddybbInit, System1Exit, System1Frame
	};

	struct BurnDriver nBurnDrvPitfall2u = {
		"pitfal2u", "sys1",
		"Pitfall II (not encrypted)\0",
		Pitfall2uRomInfo, Pitfall2uRomName, MyheroInputInfo, PitfalluDIPInfo,
		PitfalluInit, System1Exit, System1Frame
	};
 /*
	struct BurnDriver nBurnDrvFourdwarrio = {
		"4dwarrio", "sys1",
		"4-D Warriors (315-5162)\0",
		FourdwarrioRomInfo, FourdwarrioRomName, MyheroInputInfo, FourdwarrioDIPInfo,
		FourdwarrioInit, System1Exit, System1Frame
	};
*/
//	struct BurnDriver *fba_drv = 	(struct BurnDriver *)FBA_DRV;
    if (strcmp(nBurnDrvWboyu.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvWboyu,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvFlickys2.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvFlickys2,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvMyhero.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvMyhero,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvTeddybb.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvMyhero,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvPitfall2u.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvPitfall2u,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvTeddybb.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvTeddybb,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvWbdeluxe.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvWbdeluxe,sizeof(struct BurnDriver));
//    if (strcmp(nBurnDrvFourdwarrio.szShortName, szShortName) == 0)		memcpy(fba_drv,&nBurnDrvFourdwarrio,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
	
	return 0;
}

/*==============================================================================================
Decode Functions
===============================================================================================*/

/*static*/ void teddybb_decode(void)
{
	/*static*/ const UINT8 convtable[32][4] =
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
/*==============================================================================================
Driver Inits
===============================================================================================*/
/*static*/ INT32 WboyuInit()
{
	INT32 nRet = 0;
	nRet = System1Init(3, 0x4000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
	CollisionFunction = NULL;
	return nRet;
}

/*static*/ INT32 Wboy2uInit()
{
	INT32 nRet = 0;
	nRet = System1Init(6, 0x2000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
	CollisionFunction = NULL;
	return nRet;
}

/*static*/ INT32 TeddybbInit()
{
	DecodeFunction = teddybb_decode;

	return System1Init(3, 0x4000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
}


/*int SeganinuInit()
{
	return System1Init(3, 0x4000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
}

/*int NprincsuInit()
{
	return System1Init(6, 0x2000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
}
*/

/*static*/ int Flicks2Init()
{
	return System1Init(2, 0x4000, 1, 0x2000, 6, 0x2000, 2, 0x4000, 1);
}

/*static*/ INT32 PitfalluInit()
{
	int nRet;
	
	nRet = System1Init(3, 0x4000, 1, 0x2000, 6, 0x2000, 2, 0x4000, 1);
	nCyclesTotal[0] = 3600000 / 60;
	
	return nRet;
}

/*static*/ INT32 MyheroInit()
{
	return System1Init(3, 0x4000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
}

/*static*//* INT32 FourdwarrioInit()
{
	DecodeFunction = fdwarrio_decode;

	return System1Init(3, 0x4000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
}*/
/*==============================================================================================
Graphics Rendering
===============================================================================================*/
/*
void fillSpriteCollision(unsigned int Num, int *values)
{
	sprites_collision[Num].x=sprites_collision[Num].y=sprites_collision[Num].width=sprites_collision[Num].yend=0;
	int skip = values[2]<<1;
	if(values[0]>0){sprites_collision[Num].x=values[0] & 0xff;};	  //x max 255
	if(values[1]>0){sprites_collision[Num].y=values[1] & 0xff;};	  //y max 255
	if(skip<0)	    // width<0
	{
		if(values[0]>ABS(skip))		// x > abs(width)	
		{
			sprites_collision[Num].width=ABS(skip)  & 0xff;// width>0
			sprites_collision[Num].x=(values[0]+skip) & 0xff; // x=x+width
		}
	}
	else
	{
			if(sprites_collision[Num].width+sprites_collision[Num].x>255)sprites_collision[Num].width=(255-sprites_collision[Num].x) & 0xff; //width+x max 255
			else sprites_collision[Num].width=skip  & 0xff;// max 255
	}
	if(values[1]+values[3]>0)
		sprites_collision[Num].yend=(values[1]+values[3]) & 0xff; // height max 255
}
*/
inline void renderSpriteCache(int *values);

void DrawSprite(unsigned int Num,unsigned int Bank, UINT16 Skip, SprSpCmd *ss_spritePtr,UINT8 *SpriteBase)
{
	int Src = (SpriteBase[7] << 8) | SpriteBase[6];
	unsigned int Height = SpriteBase[1] - SpriteBase[0];
	unsigned int Width = width_lut[Skip];

	int values[] ={Src,Height,Skip,Width, Bank,nextSprite};
	renderSpriteCache(values);

	ss_spritePtr->ax		= (((SpriteBase[3] & 0x01) << 8) + SpriteBase[2] )/2;
	ss_spritePtr->ay		= SpriteBase[0] + 1;
	ss_spritePtr->charSize	= (Width<<6) + Height;
	ss_spritePtr->color		= COLADDR_SPR | ((Num)<<2);
	ss_spritePtr->charAddr	= 0x220+nextSprite;

	if(CollisionFunction)
	{
	 	int values2[] ={ss_spritePtr->ax,ss_spritePtr->ay,Skip,Height,Num};
//		fillSpriteCollision(Num,values2);
		updateCollisions(values2);
	}
	nextSprite+=(Width*Height)/8;
}

void DrawSpriteCache(int Num,int addr,INT16 Skip,SprSpCmd *ss_spritePtr, UINT8 *SpriteBase)
{
	unsigned int Height = SpriteBase[1] - SpriteBase[0];
	unsigned int Width = width_lut[Skip];

	ss_spritePtr->ax		= (((SpriteBase[3] & 0x01) << 8) + SpriteBase[2] )/2;
	ss_spritePtr->ay		= SpriteBase[0] + 1;
	ss_spritePtr->charSize	= (Width<<6) + Height;
	ss_spritePtr->color		= COLADDR_SPR | ((Num)<<2);
	ss_spritePtr->charAddr	= 0x220+spriteCache[addr];

	if(CollisionFunction)
	{
		int values[] ={ss_spritePtr->ax,ss_spritePtr->ay,Skip,Height,Num};
//		fillSpriteCollision(Num,values);
		updateCollisions(values);
	}
}

inline void System1Render()
{
	ss_reg->n2_move_x = System1BgScrollX = 256-(((System1ScrollX[0] >> 1) + ((System1ScrollX[1] & 1) << 7) + 6) & 0xff);
	System1BgScrollY = (-System1ScrollY[0] & 0xff);
	ss_reg->n2_move_y = System1ScrollY[0];
	System1DrawSprites();
}
