// 43-44 fps with CZ80 main & RAZE slave
// 17-18 fps with RAZE main & CZ80 slave
// FB Alpha Penguin-Kun Wars Driver Module
// Based on MAME Driver by David Haywood and Phil Stroffolino
#define CZ80 1
#define RAZE 1  // `EMULATE_R_REGISTER obligatoire
//#define USE_RAZE0 1
#define USE_RAZE1 1
#define CPU2_ENABLED 1

#include "d_sys1h.h"
#include "d_sys1_common.c"

//revoir WRITE_MEM16 et READ_MEM16
//modifier CZetSetReadHandler et CZetSetWriteHandler

int ovlInit(char *szShortName)
{
	cleanBSS();

	struct BurnDriver nBurnDrvBlockgal = {
		"blockgal", "sys1h",
		"Block Gal (MC-8123B, 317-0029)\0",
		BlockgalRomInfo, BlockgalRomName, BlockgalInputInfo, BlockgalDIPInfo,
		BlockgalInit, System1Exit, System1Frame
	};

	struct BurnDriver nBurnDrvGardia = {
		"gardia", "sys1h",
		"Gardia (317-0006)\0",
		GardiaRomInfo, GardiaRomName, MyheroInputInfo, GardiaDIPInfo,
		GardiaInit, System1Exit, System1Frame
	};

	struct BurnDriver nBurnDrvStarjack = {
		"starjack", "sys1h",
		"Star Jacker (Sega)\0", 
		StarjackRomInfo, StarjackRomName, MyheroInputInfo, StarjackDIPInfo,
		StarjackInit, System1Exit, System1Frame
	};

struct BurnDriver nBurnDrvRaflesia = {
	"raflesia", "sys1h",
	"Rafflesia (315-5162)\0", 
	RaflesiaRomInfo, RaflesiaRomName, MyheroInputInfo, RaflesiaDIPInfo,
	RaflesiaInit, System1Exit, System1Frame, NULL
};

//	struct BurnDriver *fba_drv = 	(struct BurnDriver *)FBA_DRV;
    if (strcmp(nBurnDrvBlockgal.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvBlockgal,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvGardia.szShortName, szShortName) == 0)		memcpy(shared,&nBurnDrvGardia,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvStarjack.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvStarjack,sizeof(struct BurnDriver));
    if (strcmp(nBurnDrvRaflesia.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvRaflesia,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}

/*static*/ void BlockgalMakeInputs()
{
	System1Input[2] = 0x00;
	
	for (UINT32 i = 0; i < 8; i++) {
		System1Input[2] |= (System1InputPort2[i] & 1) << i;
	}
	
	if (System1InputPort0[0]) BlockgalDial1 += 0x04;
	if (System1InputPort0[1]) BlockgalDial1 -= 0x04;
	
	if (System1InputPort0[2]) BlockgalDial2 += 0x04;
	if (System1InputPort0[3]) BlockgalDial2 -= 0x04;;
}

/*==============================================================================================
Decode Functions
===============================================================================================*/
/*static*/ void gardia_decode()
{
	sega_decode_317( System1Rom1, System1Fetch1, 1, 1, 1 );
}

/*static*/ void blockgal_decode()
{
	mc8123_decrypt_rom(0, 0, System1Rom1, System1Fetch1, (UINT8*)0x002FC000);
}
/*==============================================================================================
Memory Handlers
===============================================================================================*/
UINT8 __fastcall BlockgalZ801PortRead(unsigned short a)
{
	a &= 0xff;
	switch (a) 
	{
		case 0x00: return BlockgalDial1;
		case 0x04: return BlockgalDial2;
		case 0x08: return 0xff - System1Input[2];
		case 0x0c: return System1Dip[0];
		case 0x0d: return System1Dip[1];
		case 0x10: return System1Dip[1];
		case 0x11: return System1Dip[0];
		case 0x15: return System1VideoMode;
		case 0x19: return System1VideoMode;
	}	
	return 0;
}

void __fastcall BrainZ801PortWrite(unsigned short a, UINT8 d)
{
	a &= 0xff;
	
	switch (a) {
		case 0x14:
		case 0x18: {system1_soundport_w(d); return;}
		case 0x15:
		case 0x19: {
			System1VideoMode = d;
			System1FlipScreen = d & 0x80;
			System1BankRom(((d & 0x04) >> 2) + ((d & 0x40) >> 5));
			return;
		}
	}
}
/*==============================================================================================
Driver Inits
===============================================================================================*/
/*static*/ INT32 GardiaInit()
{
	int nRet;

	System1ColourProms = 1;
	System1BankedRom = 1;
	flipscreen = 2;

	DecodeFunction = gardia_decode;
	
	nRet = System1Init(3, 0x8000, 1, 0x4000, 3, 0x4000, 4, 0x8000, 0);
	
	CZetOpen(0);
	CZetSetOutHandler(BrainZ801PortWrite);
	CZetClose();
		
	System1DoReset();
	
	return nRet;
}

/*static*/ INT32 BlockgalInit()
{
	int nRet;
	flipscreen = 1;
	UINT8 *System1MC8123Key = (UINT8*)0x002FC000;
	BurnLoadRom(System1MC8123Key, 14, 1);
	DecodeFunction = blockgal_decode;

	nRet = System1Init(2, 0x4000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
	
	CZetOpen(0);
	CZetSetInHandler(BlockgalZ801PortRead);
	CZetClose();
	MakeInputsFunction = BlockgalMakeInputs;

	nCyclesTotal[0] = 3000000 / hz ;
	nCyclesTotal[1] = 3000000 / hz ;

	for (UINT32 i = 0; i < 10; i++) cpu_lut[i] = (i + 1) * nCyclesTotal[0] / 10;
	return nRet;
}

/*static*/ INT32 StarjackInit()
{
	flipscreen = 2;
	return System1Init(6, 0x2000, 1, 0x2000, 6, 0x2000, 2, 0x4000, 1);
}

/*static*/ INT32 RaflesiaInit()
{
	DecodeFunction = fdwarrio_decode;
	flipscreen = 2;
	return System1Init(3, 0x4000, 1, 0x2000, 6, 0x2000, 4, 0x4000, 1);
}
/*==============================================================================================
Graphics Rendering
===============================================================================================*/
void DrawSprite(unsigned int Num,unsigned int Bank, unsigned int addr, UINT16 Skip, UINT8 *SpriteBase)
{
	unsigned int Src = (SpriteBase[7] << 8) | SpriteBase[6];
	unsigned int Height = SpriteBase[1] - SpriteBase[0];
	unsigned int Width = width_lut[ABS(Skip)];
	unsigned int values[] ={Src,Height,Skip,Width, Bank,nextSprite};
	renderSpriteCache(values);
	spriteCache[addr]=nextSprite;
	nextSprite = nextSprite+(Width*Height)/8;

	unsigned int delta	= (Num+3);

	ss_sprite[delta].ay			= (((SpriteBase[3] & 0x01) << 8) + SpriteBase[2] )/2-8;
	if(flipscreen==2)
	{
		int toto = 216-ss_sprite[delta].ay;
		ss_sprite[delta].ay			= toto+Width*2;
		ss_sprite[delta].by			= toto;
		ss_sprite[delta].cy			= toto;
//		ss_sprite[delta].dy			= ss_sprite[delta].ay;
		ss_sprite[delta].ax			= SpriteBase[0] + 41;
//		ss_sprite[delta].bx			= ss_sprite[delta].ax;
		ss_sprite[delta].cx			= ss_sprite[delta].ax+Height;
//		ss_sprite[delta].dx			= ss_sprite[delta].cx;
	}
	else
	{
		ss_sprite[delta].ax		= 256-SpriteBase[0]+8;
		ss_sprite[delta].by		= ss_sprite[delta].ay+Width*2;
		ss_sprite[delta].cy		= ss_sprite[delta].by;
//		ss_sprite[delta].dy		= ss_sprite[delta].ay;

//		ss_sprite[delta].bx			= ss_sprite[delta].ax;
		ss_sprite[delta].cx			= ss_sprite[delta].ax-Height;
//		ss_sprite[delta].dx			= ss_sprite[delta].cx;
	}
	ss_sprite[delta].dy			= ss_sprite[delta].ay;
	ss_sprite[delta].bx			= ss_sprite[delta].ax;
	ss_sprite[delta].dx			= ss_sprite[delta].cx;

	ss_sprite[delta].charSize	= (Width<<6) + Height;
	ss_sprite[delta].color			= COLADDR_SPR | ((Num)<<2);
	ss_sprite[delta].charAddr	= 0x220+spriteCache[addr];

 	int values2[] ={ss_sprite[delta].ax,ss_sprite[delta].ay,Skip,Height,Num};
//	SPR_WaitEndSlaveSH();
	updateCollisions(values2);
//	SPR_RunSlaveSH((PARA_RTN*)updateCollisions,&values2);
}

void DrawSpriteCache(int Num,int Bank, int addr,INT16 Skip,UINT8 *SpriteBase)
{
	unsigned int Src = (SpriteBase[7] << 8) | SpriteBase[6];
	unsigned int Height = SpriteBase[1] - SpriteBase[0];
	unsigned int Width = width_lut[ABS(Skip)];

	unsigned int delta	= (Num+3);

	ss_sprite[delta].ay			= (((SpriteBase[3] & 0x01) << 8) + SpriteBase[2] )/2-8;
	if(flipscreen==2)
	{
		int toto = 216-ss_sprite[delta].ay;
		ss_sprite[delta].ax			= SpriteBase[0] + 41;
		ss_sprite[delta].ay			= toto+Width*2;
		ss_sprite[delta].by			= toto;
		ss_sprite[delta].cy			= toto;
//		ss_sprite[delta].dy			= ss_sprite[delta].ay;

//		ss_sprite[delta].bx			= ss_sprite[delta].ax;
		ss_sprite[delta].cx			= ss_sprite[delta].ax+Height;
//		ss_sprite[delta].dx			= ss_sprite[delta].cx;
	}
	else
	{
		ss_sprite[delta].ax		= 256-SpriteBase[0]+8;
		ss_sprite[delta].by		= ss_sprite[delta].ay+Width*2;
		ss_sprite[delta].cy		= ss_sprite[delta].by;
//		ss_sprite[delta].dy		= ss_sprite[delta].ay;

//		ss_sprite[delta].bx			= ss_sprite[delta].ax;
		ss_sprite[delta].cx			= ss_sprite[delta].ax-Height;
//		ss_sprite[delta].dx			= ss_sprite[delta].cx;
	}
	ss_sprite[delta].dy			= ss_sprite[delta].ay;
	ss_sprite[delta].bx			= ss_sprite[delta].ax;
	ss_sprite[delta].dx			= ss_sprite[delta].cx;

	ss_sprite[delta].charSize	= (Width<<6) + Height;
	ss_sprite[delta].color			= COLADDR_SPR | ((Num)<<2);
	ss_sprite[delta].charAddr	= 0x220+spriteCache[addr];

 	int values[] ={ss_sprite[delta].ax,ss_sprite[delta].ay,Skip,Height,Num};
//	SPR_WaitEndSlaveSH();
	updateCollisions(values);
//	SPR_RunSlaveSH((PARA_RTN*)updateCollisions,&values);
}

inline void System1Render()
{
	if(flipscreen==1)	
	{
		ss_reg->n2_move_y = System1BgScrollX = 256-(((System1ScrollX[0] >> 1) + ((System1ScrollX[1] & 1) << 7) + 6) & 0xff);
		ss_reg->n2_move_x = System1ScrollY[0]+8;
	}
	else 
	{
		ss_reg->n2_move_y = System1BgScrollX = (((System1ScrollX[0] >> 1) + ((System1ScrollX[1] & 1) << 7) + 6) & 0xff) + 16;
		ss_reg->n2_move_x = System1ScrollY[0];
	}
	System1BgScrollY = (-System1ScrollY[0] & 0xff);
	System1DrawSprites();
}

