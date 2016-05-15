#ifndef _D_SLAPFGHT_H_
#define _D_SLAPFGHT_H_

//#include "taito_m68705.h"
#include "burnint.h"
#include "saturn/ovl.h"
#include "snd/ay8910.h"

#define nBurnSoundLen 128
#define	true	1
#define	false	0

int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));

static INT32 tigerhInit();
static INT32 tigerhExit();
static INT32 tigerhFrame();

static INT32 nWhichGame;
typedef int bool;
static bool bInterruptEnable;
static bool bSoundCPUEnable;
static bool bSoundNMIEnable;
static bool bVBlank;

static INT32 nStatusIndex;
static INT32 nProtectIndex;

static UINT8 getstar_e803_r();
static void getstar_e803_w();

// ---------------------------------------------------------------------------
// Inputs

static UINT8 tigerhInput[4] = {0,0,0,0};
static UINT8 tigerhInpJoy1[8];
static UINT8 tigerhInpMisc[8];
static UINT8 tigerhReset = 0;

// Dip Switch and Input Definitions
static struct BurnInputInfo tigerhInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	tigerhInpMisc + 6,	"p1 coin"},
	{"P1 Start",	BIT_DIGITAL,	tigerhInpMisc + 4,	"p1 start"},
	{"P1 Up",		BIT_DIGITAL,	tigerhInpJoy1 + 0,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	tigerhInpJoy1 + 1,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	tigerhInpJoy1 + 3,	"p1 left"},
	{"P1 Right",	BIT_DIGITAL,	tigerhInpJoy1 + 2,	"p1 right"},
	{"P1 Button 1",	BIT_DIGITAL,	tigerhInpMisc + 1,	"p1 fire 1"},
	{"P1 Button 2",	BIT_DIGITAL,	tigerhInpMisc + 0,	"p1 fire 2"},

	{"P2 Coin",		BIT_DIGITAL,	tigerhInpMisc + 7,	"p2 coin"},
	{"P2 Start",	BIT_DIGITAL,	tigerhInpMisc + 5,	"p2 start"},
	{"P2 Up",		BIT_DIGITAL,	tigerhInpJoy1 + 4,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	tigerhInpJoy1 + 5,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	tigerhInpJoy1 + 7,	"p2 left"},
	{"P2 Right",	BIT_DIGITAL,	tigerhInpJoy1 + 6,	"p2 right"},
	{"P2 Button 1",	BIT_DIGITAL,	tigerhInpMisc + 3,	"p2 fire 1"},
	{"P2 Button 2",	BIT_DIGITAL,	tigerhInpMisc + 2,	"p2 fire 2"},

	{"Reset",		BIT_DIGITAL,	&tigerhReset,		"reset"},

	{"Dip A",		BIT_DIPSWITCH,	tigerhInput + 2,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	tigerhInput + 3,	"dip"},
};

STDINPUTINFO(tigerh)

static struct BurnInputInfo getstarInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	tigerhInpMisc + 6,	"p1 coin"},
	{"P1 Start",	BIT_DIGITAL,	tigerhInpMisc + 4,	"p1 start"},
	{"P1 Up",		BIT_DIGITAL,	tigerhInpJoy1 + 0,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	tigerhInpJoy1 + 2,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	tigerhInpJoy1 + 3,	"p1 left"},
	{"P1 Right",	BIT_DIGITAL,	tigerhInpJoy1 + 1,	"p1 right"},
	{"P1 Button 1",	BIT_DIGITAL,	tigerhInpMisc + 1,	"p1 fire 1"},
	{"P1 Button 2",	BIT_DIGITAL,	tigerhInpMisc + 0,	"p1 fire 2"},

	{"P2 Coin",		BIT_DIGITAL,	tigerhInpMisc + 7,	"p2 coin"},
	{"P2 Start",	BIT_DIGITAL,	tigerhInpMisc + 5,	"p2 start"},
	{"P2 Up",		BIT_DIGITAL,	tigerhInpJoy1 + 4,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	tigerhInpJoy1 + 6,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	tigerhInpJoy1 + 7,	"p2 left"},
	{"P2 Right",	BIT_DIGITAL,	tigerhInpJoy1 + 5,	"p2 right"},
	{"P2 Button 1",	BIT_DIGITAL,	tigerhInpMisc + 3,	"p2 fire 1"},
	{"P2 Button 2",	BIT_DIGITAL,	tigerhInpMisc + 2,	"p2 fire 2"},

	{"Reset",		BIT_DIGITAL,	&tigerhReset,		"reset"},

	{"Dip A",		BIT_DIPSWITCH,	tigerhInput + 2,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	tigerhInput + 3,	"dip"},
};

STDINPUTINFO(getstar)

static struct BurnInputInfo performanInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	tigerhInpMisc + 6,	"p1 coin"},
	{"P1 Start",	BIT_DIGITAL,	tigerhInpMisc + 4,	"p1 start"},
	{"P1 Up",		BIT_DIGITAL,	tigerhInpJoy1 + 0,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	tigerhInpJoy1 + 1,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	tigerhInpJoy1 + 3,	"p1 left"},
	{"P1 Right",	BIT_DIGITAL,	tigerhInpJoy1 + 2,	"p1 right"},
	{"P1 Button 1",	BIT_DIGITAL,	tigerhInpMisc + 1,	"p1 fire 1"},
	{"P1 Button 2",	BIT_DIGITAL,	tigerhInpMisc + 0,	"p1 fire 2"},

	{"P2 Coin",		BIT_DIGITAL,	tigerhInpMisc + 7,	"p2 coin"},
	{"P2 Start",	BIT_DIGITAL,	tigerhInpMisc + 5,	"p2 start"},
	{"P2 Up",		BIT_DIGITAL,	tigerhInpJoy1 + 4,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	tigerhInpJoy1 + 5,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	tigerhInpJoy1 + 7,	"p2 left"},
	{"P2 Right",	BIT_DIGITAL,	tigerhInpJoy1 + 6,	"p2 right"},
	{"P2 Button 1",	BIT_DIGITAL,	tigerhInpMisc + 3,	"p2 fire 1"},
	{"P2 Button 2",	BIT_DIGITAL,	tigerhInpMisc + 2,	"p2 fire 2"},

	{"Reset",		BIT_DIGITAL,	&tigerhReset,		"reset"},

	{"Dip A",		BIT_DIPSWITCH,	tigerhInput + 2,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	tigerhInput + 3,	"dip"},
};

STDINPUTINFO(performan)

static struct BurnDIPInfo tigerhDIPList[] = {
	// Defaults
	{0x11,	0xFF, 0xFF,	0x10, NULL},
	{0x12,	0xFF, 0xFF,	0x00, NULL},

	// DIP A
	{0,		0xFE, 0,	2,	  "Hero speed"},
	{0x11,	0x01, 0x80,	0x00, "Normal"},
	{0x11,	0x01, 0x80,	0x80, "Fast"},
	{0,		0xFE, 0,	2,	  "Service"},
	{0x11,	0x01, 0x40,	0x00, "Normal game"},
	{0x11,	0x01, 0x40,	0x40, "DIP switch test"},
	{0,		0xFE, 0,	2,	  "Screen mode"},
	{0x11,	0x01, 0x20,	0x00, "Normal screen"},
	{0x11,	0x01, 0x20,	0x20, "Invert screen"},
	{0,		0xFE, 0,	2,	  "Cabinet"},
	{0x11,	0x01, 0x10,	0x10, "Upright"},
	{0x11,	0x01, 0x10,	0x00, "Table"},
	{0,		0xFE, 0,	2,	  "Advertise sound"},
	{0x11,	0x01, 0x08,	0x00, "On"},
	{0x11,	0x01, 0x08,	0x08, "Off"},
	{0,		0xFE, 0,	7,	  "Coinage"},
	{0x11,	0x01, 0x07,	0x00, "1 coin 1 credit"},
	{0x11,	0x01, 0x07,	0x01, "1 coin 2 credits"},
	{0x11,	0x01, 0x07,	0x02, "1 coin 3 credits"},
	{0x11,	0x01, 0x07,	0x03, "2 coins 1 credit"},
	{0x11,	0x01, 0x07,	0x04, "2 coins 3 credits"},
	{0x11,	0x01, 0x07,	0x05, "3 coins 1 credit"},
	{0x11,	0x01, 0x07,	0x07, "Free play"},
	{0x11,	0x01, 0x07,	0x06, "3 coins 1 credit"},

	// DIP B
	{0,		0xFE, 0,	2,	  "Extend"},
	{0x12,	0x01, 0x10,	0x00, "20000 & 80000"},
	{0x12,	0x01, 0x10,	0x10, "50000 & 120000"},
	{0,		0xFE, 0,	4,	  "Game difficulty"},
	{0x12,	0x01, 0x0C,	0x00, "0"},
	{0x12,	0x01, 0x0C,	0x04, "1"},
	{0x12,	0x01, 0x0C,	0x08, "2"},
	{0x12,	0x01, 0x0C,	0x0C, "3"},
	{0,		0xFE, 0,	4,	  "Number of heroes"},
	{0x12,	0x01, 0x03,	0x00, "3"},
	{0x12,	0x01, 0x03,	0x01, "5"},
	{0x12,	0x01, 0x03,	0x02, "1"},
	{0x12,	0x01, 0x03,	0x03, "2"},
};

STDDIPINFO(tigerh)

static struct BurnDIPInfo getstarDIPList[] = {
	// Defaults
	{0x11,	0xFF, 0xFF,	0x10, NULL},

	// DIP A
	{0,		0xFE, 0,	2,	  "Service"},
	{0x11,	0x01, 0x40,	0x00, "Normal game"},
	{0x11,	0x01, 0x40,	0x40, "DIP switch test"},
	{0,		0xFE, 0,	2,	  "Screen mode"},
	{0x11,	0x01, 0x20,	0x00, "Normal screen"},
	{0x11,	0x01, 0x20,	0x20, "Invert screen"},
	{0,		0xFE, 0,	2,	  "Cabinet"},
	{0x11,	0x01, 0x10,	0x10, "Upright"},
	{0x11,	0x01, 0x10,	0x00, "Table"},
	{0,		0xFE, 0,	2,	  "Advertise sound"},
	{0x11,	0x01, 0x08,	0x00, "On"},
	{0x11,	0x01, 0x08,	0x08, "Off"},
	{0,		0xFE, 0,	7,	  "Coinage"},
	{0x11,	0x01, 0x07,	0x00, "1 coin 1 credit"},
	{0x11,	0x01, 0x07,	0x01, "1 coin 2 credits"},
	{0x11,	0x01, 0x07,	0x02, "1 coin 3 credits"},
	{0x11,	0x01, 0x07,	0x03, "2 coins 1 credit"},
	{0x11,	0x01, 0x07,	0x04, "2 coins 3 credits"},
	{0x11,	0x01, 0x07,	0x05, "3 coins 1 credit"},
	{0x11,	0x01, 0x07,	0x07, "Free play"},
	{0x11,	0x01, 0x07,	0x06, "3 coins 1 credit"},

	// DIP B
	{0,		0xFE, 0,	2,	  "Extend"},
	{0x12,	0x01, 0x10,	0x00, "30000 & 100000"},
	{0x12,	0x01, 0x10,	0x10, "50000 & 150000"},
	{0,		0xFE, 0,	4,	  "Game difficulty"},
	{0x12,	0x01, 0x0C,	0x00, "0"},
	{0x12,	0x01, 0x0C,	0x04, "1"},
	{0x12,	0x01, 0x0C,	0x08, "2"},
	{0x12,	0x01, 0x0C,	0x0C, "3"},
};

static struct BurnDIPInfo getstarHeroesDIPList[] =
{
	// Defaults
	{0x12,	0xFF, 0xFF,	0x00, NULL},

	// DIP B
	{0,		0xFE, 0,	4,	  "Number of heroes"},
	{0x12,	0x01, 0x03,	0x02, "1"},
	{0x12,	0x01, 0x03,	0x03, "2"},
	{0x12,	0x01, 0x03,	0x00, "3"},
	{0x12,	0x01, 0x03,	0x01, "5"},
};

static struct BurnDIPInfo getstarb2HeroesDIPList[] =
{
	// Defaults
	{0x12,	0xFF, 0xFF,	0x01, NULL},

	// DIP B
	{0,		0xFE, 0,	4,	  "Number of heroes"},
	{0x12,	0x01, 0x03,	0x01, "3"},
	{0x12,	0x01, 0x03,	0x02, "4"},
	{0x12,	0x01, 0x03,	0x03, "5"},
	{0x12,	0x01, 0x03,	0x00, "240 (cheat)"},
};

STDDIPINFOEXT(getstar, getstar, getstarHeroes)
STDDIPINFOEXT(getstarb2, getstar, getstarb2Heroes)

static struct BurnDIPInfo slapfighDIPList[] = {
	// Defaults
	{0x11,	0xFF, 0xFF,	0x80, NULL},
	{0x12,	0xFF, 0xFF,	0x00, NULL},

	// DIP A
	{0,		0xFE, 0,	2,	  "Cabinet"},
	{0x11,	0x01, 0x80,	0x80, "Up-right type"},
	{0x11,	0x01, 0x80,	0x00, "Table type"},
	{0,		0xFE, 0,	2,	  "Screen type"},
	{0x11,	0x01, 0x40,	0x00, "Normal screen"},
	{0x11,	0x01, 0x40,	0x40, "Invert screen"},
	{0,		0xFE, 0,	2,	  "Service"},
	{0x11,	0x01, 0x20,	0x00, "Normal game"},
	{0x11,	0x01, 0x20,	0x20, "Screen test mode"},
	{0,		0xFE, 0,	2,	  "Advertise sound"},
	{0x11,	0x01, 0x10,	0x00, "On"},
	{0x11,	0x01, 0x10,	0x10, "Off"},
	{0,		0xFE, 0,	4,	  "Coin A"},
	{0x11,	0x01, 0x0C,	0x00, "1 coin 1 play"},
	{0x11,	0x01, 0x0C,	0x08, "1 coin 2 plays"},
	{0x11,	0x01, 0x0C,	0x04, "2 coins 1 play"},
	{0x11,	0x01, 0x0C,	0x0C, "2 coins 3 plays"},
	{0,		0xFE, 0,	4,	  "Coin B"},
	{0x11,	0x01, 0x03,	0x00, "1 coin 1 play"},
	{0x11,	0x01, 0x03,	0x02, "1 coin 2 plays"},
	{0x11,	0x01, 0x03,	0x01, "2 coins 1 play"},
	{0x11,	0x01, 0x03,	0x03, "2 coins 3 plays"},

	// DIP B
	{0,		0xFE, 0,	4,	  "Game difficulty"},
	{0x12,	0x01, 0xC0,	0x00, "B"},
	{0x12,	0x01, 0xC0,	0x80, "A"},
	{0x12,	0x01, 0xC0,	0x40, "C"},
	{0x12,	0x01, 0xC0,	0xC0, "D"},
	{0,		0xFE, 0,	4,	  "Extend"},
	{0x12,	0x01, 0x30,	0x00, "30000, 100000"},
	{0x12,	0x01, 0x30,	0x20, "50000, 200000"},
	{0x12,	0x01, 0x30,	0x10, "50000 only"},
	{0x12,	0x01, 0x30,	0x30, "100000 only"},
	{0,		0xFE, 0,	4,	  "Fighter counts"},
	{0x12,	0x01, 0x0C,	0x00, "3"},
	{0x12,	0x01, 0x0C,	0x08, "5"},
	{0x12,	0x01, 0x0C,	0x04, "1"},
	{0x12,	0x01, 0x0C,	0x0C, "2"},
	{0,		0xFE, 0,	2,	  NULL},
	{0x12,	0x01, 0x02,	0x00, "Normal game"},
	{0x12,	0x01, 0x02,	0x02, "DIP-switch display"},
};

STDDIPINFO(slapfigh)

static struct BurnDIPInfo PerfrmanDIPList[]=
{
	{0x11, 0xff, 0xff, 0x10, NULL				},
	{0x12, 0xff, 0xff, 0x88, NULL				},

	{0   , 0xfe, 0   ,    7, "Coinage"			},
	{0x11, 0x01, 0x07, 0x05, "3 Coins 1 Credits"		},
	{0x11, 0x01, 0x07, 0x03, "2 Coins 1 Credits"		},
	{0x11, 0x01, 0x07, 0x00, "1 Coin  1 Credits"		},
	{0x11, 0x01, 0x07, 0x04, "2 Coins 3 Credits"		},
	{0x11, 0x01, 0x07, 0x01, "1 Coin  2 Credits"		},
	{0x11, 0x01, 0x07, 0x02, "1 Coin  3 Credits"		},
	{0x11, 0x01, 0x07, 0x07, "Free Play"			},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"			},
	{0x11, 0x01, 0x08, 0x08, "Off"				},
	{0x11, 0x01, 0x08, 0x00, "On"				},

//	{0   , 0xfe, 0   ,    2, "Cabinet"			},
//	{0x11, 0x01, 0x10, 0x10, "Upright"			},
//	{0x11, 0x01, 0x10, 0x00, "Cocktail"			},

	{0   , 0xfe, 0   ,    2, "Intermissions"		},
	{0x11, 0x01, 0x20, 0x00, "Off"				},
	{0x11, 0x01, 0x20, 0x20, "On"				},

	{0   , 0xfe, 0   ,    2, "Service Mode"			},
	{0x11, 0x01, 0x40, 0x00, "Off"				},
	{0x11, 0x01, 0x40, 0x40, "On"				},

	{0   , 0xfe, 0   ,    4, "Lives"			},
	{0x12, 0x01, 0x03, 0x02, "1"				},
	{0x12, 0x01, 0x03, 0x03, "2"				},
	{0x12, 0x01, 0x03, 0x00, "3"				},
	{0x12, 0x01, 0x03, 0x01, "5"				},

	{0   , 0xfe, 0   ,    4, "Difficulty"			},
	{0x12, 0x01, 0x0c, 0x00, "Easy"				},
	{0x12, 0x01, 0x0c, 0x04, "Medium"			},
	{0x12, 0x01, 0x0c, 0x08, "Hard"				},
	{0x12, 0x01, 0x0c, 0x0c, "Hardest"			},

	{0   , 0xfe, 0   ,   16, "Bonus Life"		},
	{0x12, 0x01, 0xf0, 0x40, "20k, 120k, then every 100k"	},
	{0x12, 0x01, 0xf0, 0x50, "40k, 120k, then every 100k"	},
	{0x12, 0x01, 0xf0, 0x60, "60k, 160k, then every 100k"	},
	{0x12, 0x01, 0xf0, 0x70, "Every 100k"			},
	{0x12, 0x01, 0xf0, 0x80, "20k, 220k, then every 200k"	},
	{0x12, 0x01, 0xf0, 0x90, "40k, 240k, then every 200k"	},
	{0x12, 0x01, 0xf0, 0xa0, "60k, 260k, then every 200k"	},
	{0x12, 0x01, 0xf0, 0xb0, "Every 200k"			},
	{0x12, 0x01, 0xf0, 0xc0, "20k, 320k, then every 300k"	},
	{0x12, 0x01, 0xf0, 0xd0, "40k, 340k, then every 300k"	},
	{0x12, 0x01, 0xf0, 0xe0, "60k, 360k, then every 300k"	},
	{0x12, 0x01, 0xf0, 0xf0, "Every 300k"			},
	{0x12, 0x01, 0xf0, 0x00, "20k only"			},
	{0x12, 0x01, 0xf0, 0x10, "40k only"			},
	{0x12, 0x01, 0xf0, 0x20, "60k only"			},
	{0x12, 0x01, 0xf0, 0x30, "None"				},
};

STDDIPINFO(Perfrman)

// ---------------------------------------------------------------------------

static UINT8 *Mem, *MemEnd, *RamStart, *RamEnd;

static UINT8 *Rom01, *Rom02, *Rom03;
static UINT8 *TigerHeliTileROM, *TigerHeliSpriteROM, *TigerHeliTextROM;

static UINT8 *Ram01, *RamShared, *Ram03;
static UINT8 *TigerHeliTileRAM, *TigerHeliSpriteRAM, *TigerHeliSpriteBuf, *TigerHeliTextRAM;

static UINT8* TigerHeliPaletteROM;
static UINT32* TigerHeliPalette;

static UINT8 nPalettebank;
static UINT8 nFlipscreen;

static INT16* pFMBuffer;
static INT16* pAY8910Buffer[6];

static INT32 use_mcu = 0;

 // ---------------------------------------------------------------------------
// Rom information


static struct BurnRomInfo perfrmanRomDesc[] = {
	{ "ci07.0",		0x4000, 0x7ad32eea, BRF_ESS | BRF_PRG }, //  0  CPU #0 code
	{ "ci08.1",		0x4000, 0x90a02d5f, BRF_ESS | BRF_PRG }, //  1

	{ "ci06.4",		0x2000, 0xdf891ad0, BRF_ESS | BRF_PRG }, //  2

	{ "ci02.7",		0x2000, 0x8efa960a, BRF_GRA },			 //  3 Background layer
	{ "ci01.6",		0x2000, 0x2e8e69df, BRF_GRA },			 //  4
	{ "ci00.5",		0x2000, 0x79e191f8, BRF_GRA },			 //  5

	{ "ci05.10",	0x2000, 0x809a4ccc, BRF_GRA },			 //  6 Sprite data
	{ "ci04.9",		0x2000, 0x026f27b3, BRF_GRA },			 //  7
	{ "ci03.8",		0x2000, 0x6410d9eb, BRF_GRA },			 //  8

	{ "ci14.16",	0x0100, 0x515f8a3b, BRF_GRA },			 //  9
	{ "ci13.15",	0x0100, 0xa9a397eb, BRF_GRA },			 // 10
	{ "ci12.14",	0x0100, 0x67f86e3d, BRF_GRA },			 // 11

	{ "ci11.11",	0x0100, 0xd492e6c2, BRF_OPT }, 			 // 12
	{ "ci10.12",	0x0100, 0x59490887, BRF_OPT }, 			 // 13
	{ "ci09.13",	0x0020, 0xaa0ca5a5, BRF_OPT }, 			 // 14
};

STD_ROM_PICK(perfrman)
STD_ROM_FN(perfrman)

static struct BurnRomInfo perfrmanuRomDesc[] = {
	{ "ci07.0",		0x4000, 0x7ad32eea, BRF_ESS | BRF_PRG }, //  0  CPU #0 code
	{ "ci108r5.1",	0x4000, 0x9d373efa, BRF_ESS | BRF_PRG }, //  1

	{ "ci06.4",		0x2000, 0xdf891ad0, BRF_ESS | BRF_PRG }, //  2

	{ "ci02.7",		0x2000, 0x8efa960a, BRF_GRA },			 //  3 Background layer
	{ "ci01.6",		0x2000, 0x2e8e69df, BRF_GRA },			 //  4
	{ "ci00.5",		0x2000, 0x79e191f8, BRF_GRA },			 //  5

	{ "ci05.10",	0x2000, 0x809a4ccc, BRF_GRA },			 //  6 Sprite data
	{ "ci04.9",		0x2000, 0x026f27b3, BRF_GRA },			 //  7
	{ "ci03.8",		0x2000, 0x6410d9eb, BRF_GRA },			 //  8

	{ "ci14.16",	0x0100, 0x515f8a3b, BRF_GRA },			 //  9
	{ "ci13.15",	0x0100, 0xa9a397eb, BRF_GRA },			 // 10
	{ "ci12.14",	0x0100, 0x67f86e3d, BRF_GRA },			 // 11

	{ "ci11.11",	0x0100, 0xd492e6c2, BRF_OPT }, 			 // 12
	{ "ci10.12",	0x0100, 0x59490887, BRF_OPT }, 			 // 13
	{ "ci09r1.13",	0x0020, 0xd9e92f6f, BRF_OPT }, 			 // 14
};

STD_ROM_PICK(perfrmanu)
STD_ROM_FN(perfrmanu)

static struct BurnRomInfo tigerhRomDesc[] = {
	{ "0.4",          0x004000, 0x4be73246, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "1.4",          0x004000, 0xaad04867, BRF_ESS | BRF_PRG }, //  1
	{ "2.4",          0x004000, 0x4843f15c, BRF_ESS | BRF_PRG }, //  2

	{ "a47_13.8j",    0x004000, 0x739a7e7e, BRF_GRA },			 //  3 Sprite data
	{ "a47_12.6j",    0x004000, 0xc064ecdb, BRF_GRA },			 //  4
	{ "a47_11.8h",    0x004000, 0x744fae9b, BRF_GRA },			 //  5
	{ "a47_10.6h",    0x004000, 0xe1cf844e, BRF_GRA },			 //  6

	{ "a47_05.6f",    0x002000, 0xc5325b49, BRF_GRA },			 //  7 Text layer
	{ "a47_04.6g",    0x002000, 0xcd59628e, BRF_GRA },			 //  8

	{ "a47_09.4m",    0x004000, 0x31fae8a8, BRF_GRA },			 //  9 Background layer
	{ "a47_08.6m",    0x004000, 0xe539af2b, BRF_GRA },			 // 10
	{ "a47_07.6n",    0x004000, 0x02fdd429, BRF_GRA },			 // 11
	{ "a47_06.6p",    0x004000, 0x11fbcc8c, BRF_GRA },			 // 12

	{ "82s129.12q",   0x000100, 0x2c69350d, BRF_GRA },			 // 13
	{ "82s129.12m",   0x000100, 0x7142e972, BRF_GRA },			 // 14
	{ "82s129.12n",   0x000100, 0x25f273f2, BRF_GRA },			 // 15

	{ "a47_03.12d",   0x002000, 0xd105260f, BRF_ESS | BRF_PRG }, // 16
	
	{ "a47_14.6a",    0x000800, 0x4042489f, BRF_ESS | BRF_PRG }, // 17 MCU

	{ "pal16r4a.2e",  260,      0x00000000, BRF_NODUMP },
};


STD_ROM_PICK(tigerh)
STD_ROM_FN(tigerh)

static struct BurnRomInfo tigerhjRomDesc[] = {
	{ "a47_00.8p",    0x004000, 0xcbdbe3cc, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "a47_01.8n",    0x004000, 0x65df2152, BRF_ESS | BRF_PRG }, //  1
	{ "a47_02.8k",    0x004000, 0x633d324b, BRF_ESS | BRF_PRG }, //  2

	{ "a47_13.8j",    0x004000, 0x739a7e7e, BRF_GRA },			 //  3 Sprite data
	{ "a47_12.6j",    0x004000, 0xc064ecdb, BRF_GRA },			 //  4
	{ "a47_11.8h",    0x004000, 0x744fae9b, BRF_GRA },			 //  5
	{ "a47_10.6h",    0x004000, 0xe1cf844e, BRF_GRA },			 //  6

	{ "a47_05.6f",    0x002000, 0xc5325b49, BRF_GRA },			 //  7 Text layer
	{ "a47_04.6g",    0x002000, 0xcd59628e, BRF_GRA },			 //  8

	{ "a47_09.4m",    0x004000, 0x31fae8a8, BRF_GRA },			 //  9 Background layer
	{ "a47_08.6m",    0x004000, 0xe539af2b, BRF_GRA },			 // 10
	{ "a47_07.6n",    0x004000, 0x02fdd429, BRF_GRA },			 // 11
	{ "a47_06.6p",    0x004000, 0x11fbcc8c, BRF_GRA },			 // 12

	{ "82s129.12q",   0x000100, 0x2c69350d, BRF_GRA },			 // 13
	{ "82s129.12m",   0x000100, 0x7142e972, BRF_GRA },			 // 14
	{ "82s129.12n",   0x000100, 0x25f273f2, BRF_GRA },			 // 15

	{ "a47_03.12d",   0x002000, 0xd105260f, BRF_ESS | BRF_PRG }, // 16
	
	{ "a47_14.6a",    0x000800, 0x4042489f, BRF_ESS | BRF_PRG }, // 17 MCU
};


STD_ROM_PICK(tigerhj)
STD_ROM_FN(tigerhj)

static struct BurnRomInfo tigerhb1RomDesc[] = {
	{ "b0.5",         0x004000, 0x6ae7e13c, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "a4701.8n",    0x004000, 0x65df2152, BRF_ESS | BRF_PRG }, //  1
	{ "a4702.8k",    0x004000, 0x633d324b, BRF_ESS | BRF_PRG }, //  2

	{ "a4713.8j",    0x004000, 0x739a7e7e, BRF_GRA },			 //  3 Sprite data
	{ "a4712.6j",    0x004000, 0xc064ecdb, BRF_GRA },			 //  4
	{ "a4711.8h",    0x004000, 0x744fae9b, BRF_GRA },			 //  5
	{ "a4710.6h",    0x004000, 0xe1cf844e, BRF_GRA },			 //  6

	{ "a4705.6f",    0x002000, 0xc5325b49, BRF_GRA },			 //  7 Text layer
	{ "a4704.6g",    0x002000, 0xcd59628e, BRF_GRA },			 //  8

	{ "a4709.4m",    0x004000, 0x31fae8a8, BRF_GRA },			 //  9 Background layer
	{ "a4708.6m",    0x004000, 0xe539af2b, BRF_GRA },			 // 10
	{ "a4707.6n",    0x004000, 0x02fdd429, BRF_GRA },			 // 11
	{ "a4706.6p",    0x004000, 0x11fbcc8c, BRF_GRA },			 // 12

	{ "82s129.12q",   0x000100, 0x2c69350d, BRF_GRA },			 // 13
	{ "82s129.12m",   0x000100, 0x7142e972, BRF_GRA },			 // 14
	{ "82s129.12n",   0x000100, 0x25f273f2, BRF_GRA },			 // 15

	{ "a4703.12d",   0x002000, 0xd105260f, BRF_ESS | BRF_PRG }, // 16
};


STD_ROM_PICK(tigerhb1)
STD_ROM_FN(tigerhb1)

static struct BurnRomInfo tigerhb2RomDesc[] = {
	{ "rom00_09.bin", 0x004000, 0xef738c68, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "a47_01.8n",    0x004000, 0x65df2152, BRF_ESS | BRF_PRG }, //  1
	{ "rom02_07.bin", 0x004000, 0x36e250b9, BRF_ESS | BRF_PRG }, //  2

	{ "a47_13.8j",    0x004000, 0x739a7e7e, BRF_GRA },			 //  3 Sprite data
	{ "a47_12.6j",    0x004000, 0xc064ecdb, BRF_GRA },			 //  4
	{ "a47_11.8h",    0x004000, 0x744fae9b, BRF_GRA },			 //  5
	{ "a47_10.6h",    0x004000, 0xe1cf844e, BRF_GRA },			 //  6

	{ "a47_05.6f",    0x002000, 0xc5325b49, BRF_GRA },			 //  7 Text layer
	{ "a47_04.6g",    0x002000, 0xcd59628e, BRF_GRA },			 //  8

	{ "a47_09.4m",    0x004000, 0x31fae8a8, BRF_GRA },			 //  9 Background layer
	{ "a47_08.6m",    0x004000, 0xe539af2b, BRF_GRA },			 // 10
	{ "a47_07.6n",    0x004000, 0x02fdd429, BRF_GRA },			 // 11
	{ "a47_06.6p",    0x004000, 0x11fbcc8c, BRF_GRA },			 // 12

	{ "82s129.12q",   0x000100, 0x2c69350d, BRF_GRA },			 // 13
	{ "82s129.12m",   0x000100, 0x7142e972, BRF_GRA },			 // 14
	{ "82s129.12n",   0x000100, 0x25f273f2, BRF_GRA },			 // 15

	{ "a47_03.12d",   0x002000, 0xd105260f, BRF_ESS | BRF_PRG }, // 16
};


STD_ROM_PICK(tigerhb2)
STD_ROM_FN(tigerhb2)

static struct BurnRomInfo tigerhb3RomDesc[] = {
	{ "14",           0x004000, 0xca59dd73, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "13",           0x004000, 0x38bd54db, BRF_ESS | BRF_PRG }, //  1
	{ "a47_02.8k",    0x004000, 0x633d324b, BRF_ESS | BRF_PRG }, //  2

	{ "a47_13.8j",    0x004000, 0x739a7e7e, BRF_GRA },			 //  3 Sprite data
	{ "a47_12.6j",    0x004000, 0xc064ecdb, BRF_GRA },			 //  4
	{ "a47_11.8h",    0x004000, 0x744fae9b, BRF_GRA },			 //  5
	{ "a47_10.6h",    0x004000, 0xe1cf844e, BRF_GRA },			 //  6

	{ "a47_05.6f",    0x002000, 0xc5325b49, BRF_GRA },			 //  7 Text layer
	{ "a47_04.6g",    0x002000, 0xcd59628e, BRF_GRA },			 //  8

	{ "a47_09.4m",    0x004000, 0x31fae8a8, BRF_GRA },			 //  9 Background layer
	{ "a47_08.6m",    0x004000, 0xe539af2b, BRF_GRA },			 // 10
	{ "a47_07.6n",    0x004000, 0x02fdd429, BRF_GRA },			 // 11
	{ "a47_06.6p",    0x004000, 0x11fbcc8c, BRF_GRA },			 // 12

	{ "82s129.12q",   0x000100, 0x2c69350d, BRF_GRA },			 // 13
	{ "82s129.12m",   0x000100, 0x7142e972, BRF_GRA },			 // 14
	{ "82s129.12n",   0x000100, 0x25f273f2, BRF_GRA },			 // 15

	{ "a47_03.12d",   0x002000, 0xd105260f, BRF_ESS | BRF_PRG }, // 16
};


STD_ROM_PICK(tigerhb3)
STD_ROM_FN(tigerhb3)

static struct BurnRomInfo getstarRomDesc[] = {
	{ "a68_00-1.8p",  0x004000, 0x6a8bdc6c, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "a68_01-1.8n",  0x004000, 0xebe8db3c, BRF_ESS | BRF_PRG }, //  1
	{ "a68_02-1.8k",  0x008000, 0x343e8415, BRF_ESS | BRF_PRG }, //  2

	{ "a68-13.8j",    0x008000, 0x643fb282, BRF_GRA },			 //  3 Sprite data
	{ "a68-12.6j",    0x008000, 0x11f74e32, BRF_GRA },			 //  4
	{ "a68-11.8h",    0x008000, 0xf24158cf, BRF_GRA },			 //  5
	{ "a68-10.6h",    0x008000, 0x83161ed0, BRF_GRA },			 //  6

	{ "a68_05-1.6f",  0x002000, 0x06f60107, BRF_GRA },			 //  7 Text layer
	{ "a68_04-1.6g",  0x002000, 0x1fc8f277, BRF_GRA },			 //  8

	{ "a68_09.4m",    0x008000, 0xa293cc2e, BRF_GRA },			 //  9 Background layer
	{ "a68_08.6m",    0x008000, 0x37662375, BRF_GRA },			 // 10
	{ "a68_07.6n",    0x008000, 0xcf1a964c, BRF_GRA },			 // 11
	{ "a68_06.6p",    0x008000, 0x05f9eb9a, BRF_GRA },			 // 12

	{ "rom21.12q",    0x000100, 0xd6360b4d, BRF_GRA },			 // 13
	{ "rom20.12m",    0x000100, 0x4ca01887, BRF_GRA },			 // 14
	{ "rom19.12p",    0x000100, 0x513224f0, BRF_GRA },			 // 15

	{ "a68-03.12d",   0x002000, 0x18daa44c, BRF_ESS | BRF_PRG }, // 16

	{ "a68_14.6a",    0x000800, 0x00000000, BRF_NODUMP | BRF_OPT | BRF_PRG }, // 17 MCU ROM
};


STD_ROM_PICK(getstar)
STD_ROM_FN(getstar)

static struct BurnRomInfo getstarjRomDesc[] = {
	{ "a68_00.8p",    0x004000, 0xad1a0143, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "a68_01.8n",    0x004000, 0x3426eb7c, BRF_ESS | BRF_PRG }, //  1
	{ "a68_02.8k",    0x008000, 0x3567da17, BRF_ESS | BRF_PRG }, //  2

	{ "a68-13.8j",    0x008000, 0x643fb282, BRF_GRA },			 //  3 Sprite data
	{ "a68-12.6j",    0x008000, 0x11f74e32, BRF_GRA },			 //  4
	{ "a68-11.8h",    0x008000, 0xf24158cf, BRF_GRA },			 //  5
	{ "a68-10.6h",    0x008000, 0x83161ed0, BRF_GRA },			 //  6
	
	{ "a68_05.6f",    0x002000, 0xe3d409e7, BRF_GRA },			 //  7 Text layer
	{ "a68_04.6g",    0x002000, 0x6e5ac9d4, BRF_GRA },			 //  8

	{ "a68_09.4m",    0x008000, 0xa293cc2e, BRF_GRA },			 //  9 Background layer
	{ "a68_08.6m",    0x008000, 0x37662375, BRF_GRA },			 // 10
	{ "a68_07.6n",    0x008000, 0xcf1a964c, BRF_GRA },			 // 11
	{ "a68_06.6p",    0x008000, 0x05f9eb9a, BRF_GRA },			 // 12

	{ "rom21.12q",    0x000100, 0xd6360b4d, BRF_GRA },			 // 13
	{ "rom20.12m",    0x000100, 0x4ca01887, BRF_GRA },			 // 14
	{ "rom19.12p",    0x000100, 0x513224f0, BRF_GRA },			 // 15

	{ "a68-03.12d",   0x002000, 0x18daa44c, BRF_ESS | BRF_PRG }, // 16

	{ "68705.6a",     0x000800, 0x00000000, BRF_NODUMP | BRF_OPT | BRF_PRG }, // 17 MCU ROM
};


STD_ROM_PICK(getstarj)
STD_ROM_FN(getstarj)

static struct BurnRomInfo gtstarb2RomDesc[] = {
	{ "gs_14.rom",    0x004000, 0x1a57a920, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "gs_13.rom",    0x004000, 0x805f8e77, BRF_ESS | BRF_PRG }, //  1
	{ "a68_02.bin",   0x008000, 0x3567da17, BRF_ESS | BRF_PRG }, //  2

	{ "a68-13",       0x008000, 0x643fb282, BRF_GRA },			 //  3 Sprite data
	{ "a68-12",       0x008000, 0x11f74e32, BRF_GRA },			 //  4
	{ "a68-11",       0x008000, 0xf24158cf, BRF_GRA },			 //  5
	{ "a68-10",       0x008000, 0x83161ed0, BRF_GRA },			 //  6

	{ "a68_05.bin",   0x002000, 0xe3d409e7, BRF_GRA },			 //  7 Text layer
	{ "a68_04.bin",   0x002000, 0x6e5ac9d4, BRF_GRA },			 //  8

	{ "a68_09",       0x008000, 0xa293cc2e, BRF_GRA },			 //  9 Background layer
	{ "a68_08",       0x008000, 0x37662375, BRF_GRA },			 // 10
	{ "a68_07",       0x008000, 0xcf1a964c, BRF_GRA },			 // 11
	{ "a68_06",       0x008000, 0x05f9eb9a, BRF_GRA },			 // 12

	{ "rom21",        0x000100, 0xd6360b4d, BRF_GRA },			 // 13
	{ "rom20",        0x000100, 0x4ca01887, BRF_GRA },			 // 14
	{ "rom19",        0x000100, 0x513224f0, BRF_GRA },			 // 15

	{ "a68-03",       0x002000, 0x18daa44c, BRF_ESS | BRF_PRG }, // 16
};


STD_ROM_PICK(gtstarb2)
STD_ROM_FN(gtstarb2)

static struct BurnRomInfo gtstarb1RomDesc[] = {
	{ "gs_rb_1.bin",  0x004000, 0x9afad7e0, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "gs_rb_2.bin",  0x004000, 0x5feb0a60, BRF_ESS | BRF_PRG }, //  1
	{ "gs_rb_3.bin",  0x008000, 0xe3cfb1ba, BRF_ESS | BRF_PRG }, //  2

	{ "a68-13",       0x008000, 0x643fb282, BRF_GRA },			 //  3 Sprite data
	{ "a68-12",       0x008000, 0x11f74e32, BRF_GRA },			 //  4
	{ "a68-11",       0x008000, 0xf24158cf, BRF_GRA },			 //  5
	{ "a68-10",       0x008000, 0x83161ed0, BRF_GRA },			 //  6

	{ "a68_05.bin",   0x002000, 0xe3d409e7, BRF_GRA },			 //  7 Text layer
	{ "a68_04.bin",   0x002000, 0x6e5ac9d4, BRF_GRA },			 //  8

	{ "a68_09",       0x008000, 0xa293cc2e, BRF_GRA },			 //  9 Background layer
	{ "a68_08",       0x008000, 0x37662375, BRF_GRA },			 // 10
	{ "a68_07",       0x008000, 0xcf1a964c, BRF_GRA },			 // 11
	{ "a68_06",       0x008000, 0x05f9eb9a, BRF_GRA },			 // 12

	{ "rom21",        0x000100, 0xd6360b4d, BRF_GRA },			 // 13
	{ "rom20",        0x000100, 0x4ca01887, BRF_GRA },			 // 14
	{ "rom19",        0x000100, 0x513224f0, BRF_GRA },			 // 15

	{ "a68-03",       0x002000, 0x18daa44c, BRF_ESS | BRF_PRG }, // 16
};


STD_ROM_PICK(gtstarb1)
STD_ROM_FN(gtstarb1)

static struct BurnRomInfo alconRomDesc[] = {
	{ "a77_00-1.8p",  0x008000, 0x2ba82d60, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "a77_01-1.8n",  0x008000, 0x18bb2f12, BRF_ESS | BRF_PRG }, //  1

	{ "a77_12.8j",    0x008000, 0x8545d397, BRF_GRA },			 //  2 Sprite data
	{ "a77_11.7j",    0x008000, 0xb1b7b925, BRF_GRA },			 //  3
	{ "a77_10.8h",    0x008000, 0x422d946b, BRF_GRA },			 //  4
	{ "a77_09.7h",    0x008000, 0x587113ae, BRF_GRA },			 //  5

	{ "a77_04-1.6f",  0x002000, 0x31003483, BRF_GRA },			 //  6 Text layer
	{ "a77_03-1.6g",  0x002000, 0x404152c0, BRF_GRA },			 //  7

	{ "a77_08.6k",    0x008000, 0xb6358305, BRF_GRA },			 //  8 Background layer
	{ "a77_07.6m",    0x008000, 0xe92d9d60, BRF_GRA },			 //  9
	{ "a77_06.6n",    0x008000, 0x5faeeea3, BRF_GRA },			 // 10
	{ "a77_05.6p",    0x008000, 0x974e2ea9, BRF_GRA },			 // 11

	{ "21_82s129.12q",0x000100, 0xa0efaf99, BRF_GRA },			 // 12
	{ "20_82s129.12m",0x000100, 0xa56d57e5, BRF_GRA },			 // 13
	{ "19_82s129.12n",0x000100, 0x5cbf9fbf, BRF_GRA },			 // 14

	{ "a77_02.12d",   0x002000, 0x87f4705a, BRF_ESS | BRF_PRG }, // 15

	{ "a77_13.6a",    0x000800, 0xa70c81d9, BRF_ESS | BRF_PRG }, // 16 MCU ROM
};


STD_ROM_PICK(alcon)
STD_ROM_FN(alcon)

static struct BurnRomInfo slapfighRomDesc[] = {
	{ "a77_00.8p",    0x008000, 0x674c0e0f, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "a77_01.8n",    0x008000, 0x3c42e4a7, BRF_ESS | BRF_PRG }, //  1

	{ "a77_12.8j",    0x008000, 0x8545d397, BRF_GRA },			 //  2 Sprite data
	{ "a77_11.7j",    0x008000, 0xb1b7b925, BRF_GRA },			 //  3
	{ "a77_10.8h",    0x008000, 0x422d946b, BRF_GRA },			 //  4
	{ "a77_09.7h",    0x008000, 0x587113ae, BRF_GRA },			 //  5

	{ "a77_04.6f",    0x002000, 0x2ac7b943, BRF_GRA },			 //  6 Text layer
	{ "a77_03.6g",    0x002000, 0x33cadc93, BRF_GRA },			 //  7

	{ "a77_08.6k",    0x008000, 0xb6358305, BRF_GRA },			 //  8 Background layer
	{ "a77_07.6m",    0x008000, 0xe92d9d60, BRF_GRA },			 //  9
	{ "a77_06.6n",    0x008000, 0x5faeeea3, BRF_GRA },			 // 10
	{ "a77_05.6p",    0x008000, 0x974e2ea9, BRF_GRA },			 // 11

	{ "21_82s129.12q",0x000100, 0xa0efaf99, BRF_GRA },			 // 12
	{ "20_82s129.12m",0x000100, 0xa56d57e5, BRF_GRA },			 // 13
	{ "19_82s129.12n",0x000100, 0x5cbf9fbf, BRF_GRA },			 // 14

	{ "a77_02.12d",   0x002000, 0x87f4705a, BRF_ESS | BRF_PRG }, // 15

	{ "a77_13.6a",    0x000800, 0xa70c81d9, BRF_ESS | BRF_PRG }, // 16 MCU ROM
};


STD_ROM_PICK(slapfigh)
STD_ROM_FN(slapfigh)

static struct BurnRomInfo slapbtjpRomDesc[] = {
	{ "sf_r19jb.bin", 0x008000, 0x9a7ac8b3, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "sf_rh.bin",    0x008000, 0x3c42e4a7, BRF_ESS | BRF_PRG }, //  1

	{ "sf_r03.bin",   0x008000, 0x8545d397, BRF_GRA },			 //  2 Sprite data
	{ "sf_r01.bin",   0x008000, 0xb1b7b925, BRF_GRA },			 //  3
	{ "sf_r04.bin",   0x008000, 0x422d946b, BRF_GRA },			 //  4
	{ "sf_r02.bin",   0x008000, 0x587113ae, BRF_GRA },			 //  5

	{ "sf_r11.bin",   0x002000, 0x2ac7b943, BRF_GRA },			 //  6 Text layer
	{ "sf_r10.bin",   0x002000, 0x33cadc93, BRF_GRA },			 //  7

	{ "sf_r06.bin",   0x008000, 0xb6358305, BRF_GRA },			 //  8 Background layer
	{ "sf_r09.bin",   0x008000, 0xe92d9d60, BRF_GRA },			 //  9
	{ "sf_r08.bin",   0x008000, 0x5faeeea3, BRF_GRA },			 // 10
	{ "sf_r07.bin",   0x008000, 0x974e2ea9, BRF_GRA },			 // 11

	{ "sf_col21.bin", 0x000100, 0xa0efaf99, BRF_GRA },			 // 12
	{ "sf_col20.bin", 0x000100, 0xa56d57e5, BRF_GRA },			 // 13
	{ "sf_col19.bin", 0x000100, 0x5cbf9fbf, BRF_GRA },			 // 14

	{ "sf_r05.bin",   0x002000, 0x87f4705a, BRF_ESS | BRF_PRG }, // 15
};


STD_ROM_PICK(slapbtjp)
STD_ROM_FN(slapbtjp)

static struct BurnRomInfo slapbtukRomDesc[] = {
	{ "sf_r19eb.bin", 0x004000, 0x2efe47af, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "sf_r20eb.bin", 0x004000, 0xf42c7951, BRF_ESS | BRF_PRG }, //  1
	{ "sf_rh.bin",    0x008000, 0x3c42e4a7, BRF_ESS | BRF_PRG }, //  1

	{ "sf_r03.bin",   0x008000, 0x8545d397, BRF_GRA },			 //  2 Sprite data
	{ "sf_r01.bin",   0x008000, 0xb1b7b925, BRF_GRA },			 //  3
	{ "sf_r04.bin",   0x008000, 0x422d946b, BRF_GRA },			 //  4
	{ "sf_r02.bin",   0x008000, 0x587113ae, BRF_GRA },			 //  5

	{ "sf_r11.bin",   0x002000, 0x2ac7b943, BRF_GRA },			 //  6 Text layer
	{ "sf_r10.bin",   0x002000, 0x33cadc93, BRF_GRA },			 //  7

	{ "sf_r06.bin",   0x008000, 0xb6358305, BRF_GRA },			 //  8 Background layer
	{ "sf_r09.bin",   0x008000, 0xe92d9d60, BRF_GRA },			 //  9
	{ "sf_r08.bin",   0x008000, 0x5faeeea3, BRF_GRA },			 // 10
	{ "sf_r07.bin",   0x008000, 0x974e2ea9, BRF_GRA },			 // 11

	{ "sf_col21.bin", 0x000100, 0xa0efaf99, BRF_GRA },			 // 12
	{ "sf_col20.bin", 0x000100, 0xa56d57e5, BRF_GRA },			 // 13
	{ "sf_col19.bin", 0x000100, 0x5cbf9fbf, BRF_GRA },			 // 14

	{ "sf_r05.bin",   0x002000, 0x87f4705a, BRF_ESS | BRF_PRG }, // 15
};


STD_ROM_PICK(slapbtuk)
STD_ROM_FN(slapbtuk)

static struct BurnRomInfo slapfgtrRomDesc[] = {
	{ "k1-10.u90",    0x004000, 0x2efe47af, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "k1-09.u89",    0x004000, 0x17c187c5, BRF_ESS | BRF_PRG }, //  1
	{ "k1-08.u88",    0x002000, 0x945af97f, BRF_ESS | BRF_PRG }, //  1
	{ "k1-07.u87",    0x008000, 0x3c42e4a7, BRF_ESS | BRF_PRG }, //  1

	{ "k1-15.u60",    0x008000, 0x8545d397, BRF_GRA },			 //  2 Sprite data
	{ "k1-13.u50",    0x008000, 0xb1b7b925, BRF_GRA },			 //  3
	{ "k1-14.u59",    0x008000, 0x422d946b, BRF_GRA },			 //  4
	{ "k1-12.u49",    0x008000, 0x587113ae, BRF_GRA },			 //  5

	{ "k1-02.u57" ,   0x002000, 0x2ac7b943, BRF_GRA },			 //  6 Text layer
	{ "k1-03.u58",    0x002000, 0x33cadc93, BRF_GRA },			 //  7

	{ "k1-01.u49" ,   0x008000, 0xb6358305, BRF_GRA },			 //  8 Background layer
	{ "k1-04.u62",    0x008000, 0xe92d9d60, BRF_GRA },			 //  9
	{ "k1-05.u63",    0x008000, 0x5faeeea3, BRF_GRA },			 // 10
	{ "k1-06.u64",    0x008000, 0x974e2ea9, BRF_GRA },			 // 11

	{ "sf_col21.bin", 0x000100, 0xa0efaf99, BRF_GRA },			 // 12
	{ "sf_col20.bin", 0x000100, 0xa56d57e5, BRF_GRA },			 // 13
	{ "sf_col19.bin", 0x000100, 0x5cbf9fbf, BRF_GRA },			 // 14

	{ "k1-11.u89",    0x002000, 0x87f4705a, BRF_ESS | BRF_PRG }, // 15
};

STD_ROM_PICK(slapfgtr)
STD_ROM_FN(slapfgtr)

#endif
