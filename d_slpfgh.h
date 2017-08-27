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
void updateSound();
static void make_lut(void);
static INT32 tigerhInit();
static INT32 tigerhExit();
static INT32 tigerhFrame();
static void Set6PCM();

typedef int bool;
static bool bInterruptEnable = 0;
static bool bSoundCPUEnable = 0;
static bool bSoundNMIEnable = 0;
static bool bVBlank = 0;

static INT32 nWhichGame = 0;
static UINT32 nStatusIndex = 0;
static UINT32 nProtectIndex = 0;

static INT32 nTigerHeliTileXPosLo = 0;
static INT32 nTigerHeliTileXPosHi = 0;
static INT32 nTigerHeliTileYPosLo = 0;
static UINT32 nTigerHeliTileMask = 0; 
static UINT32 nTigerHeliSpriteMask = 0;
static UINT8 nPalettebank = 0;
static UINT8 nFlipscreen = 0;
static UINT8 tigerhReset = 0;

static UINT8 *Mem = NULL;
static UINT8 *MemEnd = NULL;
static UINT8 *RamStart = NULL; 
static UINT8 *RamEnd = NULL;

static UINT8 *Rom01 = NULL;
static UINT8 *Rom02 = NULL;
static UINT8 *TigerHeliTileROM = NULL;
static UINT8 *TigerHeliSpriteROM = NULL;
static UINT8 *TigerHeliTextROM = NULL;
static UINT8 *Ram01 = NULL;
static UINT8 *RamShared = NULL;
static UINT8 *TigerHeliTileRAM = NULL;
static UINT8 *TigerHeliSpriteRAM = NULL;
static UINT8 *TigerHeliSpriteBuf = NULL;
static UINT8 *TigerHeliTextRAM = NULL;
static UINT8 *CZ80Context = NULL;
static UINT8 *TigerHeliPaletteROM = NULL;
static INT16 *pFMBuffer = NULL;
static INT16 *pAY8910Buffer[6];
static UINT32 *TigerHeliPalette = NULL;

static UINT16 *map_offset_lut = NULL;
static UINT16 *map_offset_lut2 = NULL;

PcmHn 			pcm6[6];
#define	PCM_ADDR	((void*)0x25a20000)
#define	PCM_SIZE	(4096L*2)				/* 2.. */
#define SOUNDRATE   7680L
// ---------------------------------------------------------------------------
// Inputs

static UINT8 tigerhInput[4] = {0,0,0,0};
static UINT8 tigerhInpJoy1[8] = {0,0,0,0,0,0,0,0};
static UINT8 tigerhInpMisc[8] = {0,0,0,0,0,0,0,0};

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

static struct BurnRomInfo slapfighRomDesc[] = {
	{ "a7700.8p",    0x008000, 0x674c0e0f, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "a7701.8n",    0x008000, 0x3c42e4a7, BRF_ESS | BRF_PRG }, //  1

	{ "a7712.8j",    0x008000, 0x8545d397, BRF_GRA },			 //  2 Sprite data
	{ "a7711.7j",    0x008000, 0xb1b7b925, BRF_GRA },			 //  3
	{ "a7710.8h",    0x008000, 0x422d946b, BRF_GRA },			 //  4
	{ "a7709.7h",    0x008000, 0x587113ae, BRF_GRA },			 //  5

	{ "a7704.6f",    0x002000, 0x2ac7b943, BRF_GRA },			 //  6 Text layer
	{ "a7703.6g",    0x002000, 0x33cadc93, BRF_GRA },			 //  7

	{ "a7708.6k",    0x008000, 0xb6358305, BRF_GRA },			 //  8 Background layer
	{ "a7707.6m",    0x008000, 0xe92d9d60, BRF_GRA },			 //  9
	{ "a7706.6n",    0x008000, 0x5faeeea3, BRF_GRA },			 // 10
	{ "a7705.6p",    0x008000, 0x974e2ea9, BRF_GRA },			 // 11

	{ "2182s129.12q",0x000100, 0xa0efaf99, BRF_GRA },			 // 12
	{ "2082s129.12m",0x000100, 0xa56d57e5, BRF_GRA },			 // 13
	{ "1982s129.12n",0x000100, 0x5cbf9fbf, BRF_GRA },			 // 14

	{ "a7702.12d",   0x002000, 0x87f4705a, BRF_ESS | BRF_PRG }, // 15

	{ "a7713.6a",    0x000800, 0xa70c81d9, BRF_ESS | BRF_PRG }, // 16 MCU ROM
};


STD_ROM_PICK(slapfigh)
STD_ROM_FN(slapfigh)

static struct BurnRomInfo slapbtjpRomDesc[] = {
	{ "sfr19jb.bin", 0x008000, 0x9a7ac8b3, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
	{ "sfrh.bin",    0x008000, 0x3c42e4a7, BRF_ESS | BRF_PRG }, //  1

	{ "sfr03.bin",   0x008000, 0x8545d397, BRF_GRA },			 //  2 Sprite data
	{ "sfr01.bin",   0x008000, 0xb1b7b925, BRF_GRA },			 //  3
	{ "sfr04.bin",   0x008000, 0x422d946b, BRF_GRA },			 //  4
	{ "sfr02.bin",   0x008000, 0x587113ae, BRF_GRA },			 //  5

	{ "sfr11.bin",   0x002000, 0x2ac7b943, BRF_GRA },			 //  6 Text layer
	{ "sfr10.bin",   0x002000, 0x33cadc93, BRF_GRA },			 //  7

	{ "sfr06.bin",   0x008000, 0xb6358305, BRF_GRA },			 //  8 Background layer
	{ "sfr09.bin",   0x008000, 0xe92d9d60, BRF_GRA },			 //  9
	{ "sfr08.bin",   0x008000, 0x5faeeea3, BRF_GRA },			 // 10
	{ "sfr07.bin",   0x008000, 0x974e2ea9, BRF_GRA },			 // 11

	{ "sfcol21.bin", 0x000100, 0xa0efaf99, BRF_GRA },			 // 12
	{ "sfcol20.bin", 0x000100, 0xa56d57e5, BRF_GRA },			 // 13
	{ "sfcol19.bin", 0x000100, 0x5cbf9fbf, BRF_GRA },			 // 14

	{ "sfr05.bin",   0x002000, 0x87f4705a, BRF_ESS | BRF_PRG }, // 15
};
STD_ROM_PICK(slapbtjp)
STD_ROM_FN(slapbtjp)
#endif
