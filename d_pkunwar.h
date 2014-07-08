#ifndef _D_PKUNWAR_H_
#define _D_PKUNWAR_H_

#include "burnint.h"
#include "ay8910.h"
#include "saturn/ovl.h"

#define SOUND_LEN 128
int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));

/*static*/ void updateSound();
/*static*/ unsigned char DrvJoy1[8], DrvJoy2[8], DrvDips, DrvReset;
/*static*/ short *pAY8910Buffer[6], *pFMBuffer = NULL;
/*static*/ unsigned char *Mem = NULL, *Rom = NULL, *Gfx0 = NULL, *Gfx1 = NULL;

 void DrvInitSaturn();
 void cleanSprites();
///*static*/ int flipscreen, vblank;
/*static*/ int vblank;
/*static*/ int DrvDraw();
/*static*/ int DrvFrame();
/*static*/ int DrvExit();
 /*static*/ int DrvInit();
//-------------------------------------------------------------------------------------------------
// Input Handlers
/*static*/ struct BurnInputInfo DrvInputList[] = {
	{"P1 Coin"      , BIT_DIGITAL  , DrvJoy2 + 7,	"p1 coin"  },
	{"P1 start"  ,    BIT_DIGITAL  , DrvJoy1 + 5,	"p1 start" },
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy1 + 0, 	"p1 left"  },
	{"P1 Right"     , BIT_DIGITAL  , DrvJoy1 + 1, 	"p1 right" },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy1 + 2,	"p1 fire 1"},

	{"P2 start"  ,    BIT_DIGITAL  , DrvJoy2 + 5,	"p2 start" },
	{"P2 Left"      , BIT_DIGITAL  , DrvJoy2 + 0, 	"p2 left"  },
	{"P2 Right"     , BIT_DIGITAL  , DrvJoy2 + 1, 	"p2 right" },
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy2 + 2,	"p2 fire 1"},

	{"Service Mode",  BIT_DIGITAL,   DrvJoy2 + 6,   "diag"     },

	{"Reset",	  BIT_DIGITAL  , &DrvReset,	"reset"    },
	{"Dip 1",	  BIT_DIPSWITCH, &DrvDips ,	"dip"	   },
};

STDINPUTINFO(Drv)

/*static*/ struct BurnDIPInfo DrvDIPList[]=
{
	// Default Values
	{0x0b, 0xff, 0xff, 0xfb, NULL 			},

	{0   , 0xfe, 0   , 4   , "Coinage"		},
	{0x0b, 0x01, 0x03, 0x00, "3C 1C"		},
	{0x0b, 0x01, 0x03, 0x02, "2C 1C"		},
	{0x0b, 0x01, 0x03, 0x03, "1C 1C"		},
	{0x0b, 0x01, 0x03, 0x01, "1C 2C"		},

	{0   , 0xfe, 0   , 2   , "Cabinet"		},
	{0x0b, 0x01, 0x04, 0x00, "Upright"		},
	{0x0b, 0x01, 0x04, 0x04, "Cocktail"		},

	{0   , 0xfe, 0   , 2   , "Demo Sounds"		},
	{0x0b, 0x01, 0x08, 0x00, "Off"			},
	{0x0b, 0x01, 0x08, 0x08, "On"			},

	{0   , 0xfe, 0   , 4   , "Difficulty"		},
	{0x0b, 0x01, 0x30, 0x10, "Easy"			},
	{0x0b, 0x01, 0x30, 0x30, "Medium"		},
	{0x0b, 0x01, 0x30, 0x20, "Hard"			},
	{0x0b, 0x01, 0x30, 0x00, "Hardest"		},

	{0   , 0xfe, 0   , 2   , "Flip screen"		},
	{0x0b, 0x01, 0x40, 0x40, "Off"			},
	{0x0b, 0x01, 0x40, 0x00, "On"			},

	{0   , 0xfe, 0   , 2   , "Free Play"		},
	{0x0b, 0x01, 0x80, 0x80, "Off"			},
	{0x0b, 0x01, 0x80, 0x00, "On"			},
};

STDDIPINFO(Drv)
//-------------------------------------------------------------------------------------------------
// Game drivers

// Penguin-Kun Wars (US)

/*static*/ struct BurnRomInfo pkunwarRomDesc[] = {
	{ "pkwar.01r",    0x4000, 0xce2d2c7b, 1 | BRF_PRG | BRF_ESS },	//  0 Z80 Code
	{ "pkwar.02r",    0x4000, 0xabc1f661, 1 | BRF_PRG | BRF_ESS },	//  1
	{ "pkwar.03r",    0x2000, 0x56faebea, 1 | BRF_PRG | BRF_ESS },	//  2

	{ "pkwar.01y",    0x4000, 0x428d3b92, 2 | BRF_GRA },		//  3 Graphics
	{ "pkwar.02y",    0x4000, 0xce1da7bc, 2 | BRF_GRA },		//  4
	{ "pkwar.03y",    0x4000, 0x63204400, 2 | BRF_GRA },		//  5
	{ "pkwar.04y",    0x4000, 0x061dfca8, 2 | BRF_GRA },		//  6

	{ "pkwar.col",    0x0020, 0xaf0fc5e2, 3 | BRF_GRA },		//  7 Color Prom
};

STD_ROM_PICK(pkunwar)
STD_ROM_FN(pkunwar)
#endif