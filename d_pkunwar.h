#ifndef _D_PKUNWAR_H_
#define _D_PKUNWAR_H_

#include "burnint.h"
#include "ay8910.h"
#include "saturn/ovl.h"

#define SOUND_LEN 128
int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));

/*static*/ void updateSound();
/*static*/ unsigned char DrvJoy1[8], DrvJoy2[8], DrvJoy3[8], DrvDips[2], DrvReset;
static UINT8 DrvInputs[8];
/*static*/ short *pAY8910Buffer[6], *pFMBuffer = NULL;
/*static*/ unsigned char *Mem = NULL;
/*static*/ UINT8 *DrvMainROM = NULL;
/*static*/ UINT8 *DrvGfxROM0 = NULL;
/*static*/ UINT8 *DrvGfxROM1 = NULL;
/*static*/ UINT8 *AllMem = NULL;
/*static*/ UINT8 *MemEnd = NULL;
static UINT8 *DrvMainRAM = NULL;
static UINT8 *DrvColPROM = NULL;
static UINT8 *DrvBgRAM = NULL;
static UINT8 *DrvFgRAM = NULL;
static UINT8 *DrvSprRAM = NULL;

static INT32 DrvCoinHold = 0;
static INT32 DrvCoinHoldframecnt = 0;
static INT32 flipscreen;
static INT32 xscroll;
static INT32 yscroll;
static INT32 watchdog;

 void DrvInitSaturn();
 void cleanSprites();
///*static*/ int flipscreen, vblank;
/*static*/ int vblank;
/*static*/ int DrvDraw();
/*static*/ int DrvFrame();
/*static*/ int DrvExit();
 /*static*/ int DrvInit();
 static INT32 NovaInit();
 static INT32 NovaFrame();
 static INT32 NovaDraw();
 static void DrvGfxDescramble(UINT8 *gfx);
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
	{"Dip 1",	  BIT_DIPSWITCH, DrvDips + 0,	"dip"	   },
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

static struct BurnDIPInfo Nova2001DIPList[]=
{
	{0x10, 0xff, 0xff, 0xfe, NULL			},
	{0x11, 0xff, 0xff, 0xf8, NULL			},

	{0   , 0xfe, 0   ,    2, "Cabinet"		},
	{0x10, 0x01, 0x01, 0x00, "Upright"		},
	{0x10, 0x01, 0x01, 0x01, "Cocktail"		},

	{0   , 0xfe, 0   ,    2, "Lives"		},
	{0x10, 0x01, 0x02, 0x02, "3"			},
	{0x10, 0x01, 0x02, 0x00, "4"			},

	{0   , 0xfe, 0   ,    2, "1st Bonus Life"	},
	{0x10, 0x01, 0x04, 0x04, "20K"			},
	{0x10, 0x01, 0x04, 0x00, "30K"			},

	{0   , 0xfe, 0   ,    4, "Extra Bonus Life"	},
	{0x10, 0x01, 0x18, 0x18, "60K"			},
	{0x10, 0x01, 0x18, 0x10, "70K"			},
	{0x10, 0x01, 0x18, 0x08, "90K"			},
	{0x10, 0x01, 0x18, 0x00, "None"			},

	{0   , 0xfe, 0   ,    4, "Coinage"		},
	{0x10, 0x01, 0x60, 0x40, "2 Coins 1 Credits"	},
	{0x10, 0x01, 0x60, 0x00, "2 Coins 2 Credits"	},
	{0x10, 0x01, 0x60, 0x60, "1 Coin  1 Credits"	},
	{0x10, 0x01, 0x60, 0x20, "1 Coin  2 Credits"	},

	{0   , 0xfe, 0   ,    2, "Free Play"		},
	{0x10, 0x01, 0x80, 0x80, "Off"			},
	{0x10, 0x01, 0x80, 0x00, "On"			},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x11, 0x01, 0x03, 0x00, "Easy"			},
	{0x11, 0x01, 0x03, 0x03, "Medium"		},
	{0x11, 0x01, 0x03, 0x02, "Hard"			},
	{0x11, 0x01, 0x03, 0x01, "Hardest"		},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x11, 0x01, 0x04, 0x04, "Off"			},
	{0x11, 0x01, 0x04, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "High Score Names"	},
	{0x11, 0x01, 0x08, 0x00, "3 Letters"		},
	{0x11, 0x01, 0x08, 0x08, "8 Letters"		},

	{0   , 0xfe, 0   ,    2, "Service Mode"		},
	{0x11, 0x01, 0x80, 0x80, "Off"			},
	{0x11, 0x01, 0x80, 0x00, "On"			},
};

STDDIPINFO(Nova2001)

static struct BurnInputInfo Nova2001InputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy3 + 0,	"p1 coin"	},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy3 + 1,	"p1 start"	},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 7,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 2"	},

	{"P2 Start",		BIT_DIGITAL,	DrvJoy3 + 2,	"p2 start"	},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 up"		},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 down"	},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy2 + 7,	"p2 fire 1"	},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy2 + 6,	"p2 fire 2"	},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"		},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"		},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
};

STDINPUTINFO(Nova2001)
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

// Nova 2001 (US)

static struct BurnRomInfo nova2001uRomDesc[] = {
	{ "nova2001.1",		0x2000, 0xb79461bd, 1 | BRF_PRG | BRF_ESS}, //  0 Z80 Code
	{ "nova2001.2",	        0x2000, 0xfab87144, 1 | BRF_PRG | BRF_ESS}, //  1
	{ "3.6f",		0x2000, 0xb2849038, 1 | BRF_PRG | BRF_ESS}, //  2
	{ "4.6g",	        0x1000, 0x6b5bb12d, 1 | BRF_PRG | BRF_ESS}, //  3

	{ "nova2001.5",		0x2000, 0x8ea576e8, 2 | BRF_GRA },          //  4 Graphics
	{ "nova2001.6",		0x2000, 0x0c61656c, 2 | BRF_GRA },          //  5
	{ "7.12n",		0x2000, 0x9ebd8806, 2 | BRF_GRA },          //  6
	{ "8.12l",		0x2000, 0xd1b18389, 2 | BRF_GRA },          //  7

	{ "nova2001.clr",	0x0020, 0xa2fac5cd, 3 | BRF_GRA },          //  8 Color Prom
};

STD_ROM_PICK(nova2001u)
STD_ROM_FN(nova2001u)
#endif