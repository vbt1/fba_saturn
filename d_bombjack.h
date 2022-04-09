#ifndef _D_BOMBJACK_H_
#define _D_BOMBJACK_H_

#include "burnint.h"
#include "snd/ay8910.h"
#include "sega_spr.h"
#include "sega_pcm.h"
#include "saturn/ovl.h"
#include "czet.h"

#define nBurnSoundLen 128
#define SOUNDRATE   7680L

UINT16 *map_offset_lut = NULL;
//UINT16 *mapbg_offset_lut = NULL;
UINT8 *CZ80Context = NULL;
UINT16 *cram_lut = NULL;

UINT32 CalcCol(UINT16 nColour);
INT32 DrvInit();
void DrvZInit();
INT32 DrvExit();
void DrvFrame();
void DrvDoReset();

UINT8 DrvJoy1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
UINT8 DrvJoy2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
UINT8 DrvJoy3[8] = {0, 0, 0, 0, 0, 0, 0, 0};
UINT8 DrvDips[2] = {0, 0};
UINT8 DrvInputs[3] = {0, 0, 0};

//UINT8 DrvReset = 0;
INT32 nmi_mask = 0;
UINT8 soundlatch = 0;
UINT32 BgSel = 0xFFFF;

//UINT8 *MemEnd = NULL;
//UINT8 *RamStart = NULL;
//UINT8 *RamEnd = NULL;
UINT8 *BjGfx = NULL;
UINT8 *BjMap = NULL;
UINT8 *DrvZ80ROM0 = NULL;
UINT8 *DrvZ80RAM0 = NULL;
UINT8 *DrvColRAM = NULL;
UINT8 *DrvVidRAM = NULL;
UINT8 *DrvSprRAM = NULL;

// sound cpu
UINT8 *DrvZ80ROM1 = NULL;
UINT8 *DrvZ80RAM1 = NULL;

// pallete
UINT8 *DrvPalRAM = NULL;
//UINT32 *BjPalReal = NULL;

INT16* pFMBuffer = NULL;
INT16* pAY8910Buffer[9];

// Dip Switch and Input Definitions
static struct BurnInputInfo BombjackInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy3 + 0,	"p1 coin"	},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy3 + 2,	"p1 start"	},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"	},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy3 + 1,	"p2 coin"	},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy3 + 3,	"p2 start"	},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 up"		},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 down"	},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy2 + 4,	"p2 fire 1"	},

	{"Reset",		BIT_DIGITAL,	NULL,	"reset"		},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"		},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
};

STDINPUTINFO(Bombjack)

static struct BurnDIPInfo BombjackDIPList[]=
{
	{0x0f, 0xff, 0xff, 0xc0, NULL			},
	{0x10, 0xff, 0xff, 0x50, NULL			},

	{0   , 0xfe, 0   ,    4, "Coin A"		},
	{0x0f, 0x01, 0x03, 0x00, "1 Coin  1 Credits"	},
	{0x0f, 0x01, 0x03, 0x01, "1 Coin  2 Credits"	},
	{0x0f, 0x01, 0x03, 0x02, "1 Coin  3 Credits"	},
	{0x0f, 0x01, 0x03, 0x03, "1 Coin  6 Credits"	},

	{0   , 0xfe, 0   ,    4, "Coin B"		},
	{0x0f, 0x01, 0x0c, 0x04, "2 Coins 1 Credits"	},
	{0x0f, 0x01, 0x0c, 0x00, "1 Coin  1 Credits"	},
	{0x0f, 0x01, 0x0c, 0x08, "1 Coin  2 Credits"	},
	{0x0f, 0x01, 0x0c, 0x0c, "1 Coin  3 Credits"	},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x0f, 0x01, 0x30, 0x30, "2"			},
	{0x0f, 0x01, 0x30, 0x00, "3"			},
	{0x0f, 0x01, 0x30, 0x10, "4"			},
	{0x0f, 0x01, 0x30, 0x20, "5"			},

	{0   , 0xfe, 0   ,    2, "Cabinet"		},
	{0x0f, 0x01, 0x40, 0x40, "Upright"		},
	{0x0f, 0x01, 0x40, 0x00, "Cocktail"		},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x0f, 0x01, 0x80, 0x00, "Off"			},
	{0x0f, 0x01, 0x80, 0x80, "On"			},

	{0   , 0xfe, 0   ,    8, "Bonus Life"		},
	{0x10, 0x01, 0x07, 0x02, "Every 30k"		},
	{0x10, 0x01, 0x07, 0x01, "Every 100k"		},
	{0x10, 0x01, 0x07, 0x07, "50k, 100k and 300k"	},
	{0x10, 0x01, 0x07, 0x05, "50k and 100k"		},
	{0x10, 0x01, 0x07, 0x03, "50k only"		},
	{0x10, 0x01, 0x07, 0x06, "100k and 300k"	},
	{0x10, 0x01, 0x07, 0x04, "100k only"		},
	{0x10, 0x01, 0x07, 0x00, "None"			},

	{0   , 0xfe, 0   ,    4, "Bird Speed"		},
	{0x10, 0x01, 0x18, 0x00, "Easy"			},
	{0x10, 0x01, 0x18, 0x08, "Medium"		},
	{0x10, 0x01, 0x18, 0x10, "Hard"			},
	{0x10, 0x01, 0x18, 0x18, "Hardest"		},

	{0   , 0xfe, 0   ,    4, "Enemies Number & Speed"},
	{0x10, 0x01, 0x60, 0x20, "Easy"			},
	{0x10, 0x01, 0x60, 0x00, "Medium"		},
	{0x10, 0x01, 0x60, 0x40, "Hard"			},
	{0x10, 0x01, 0x60, 0x60, "Hardest"		},

	{0   , 0xfe, 0   ,    2, "Special Coin"		},
	{0x10, 0x01, 0x80, 0x00, "Easy"			},
	{0x10, 0x01, 0x80, 0x80, "Hard"			},
};

STDDIPINFO(Bombjack)

// Bomb Jack (set 1)
static struct BurnRomInfo BombjackRomDesc[] = {
	{ "09j01b.bin",    0x2000, 0xc668dc30, BRF_ESS | BRF_PRG },		//  0 Z80 code
	{ "10l01b.bin",    0x2000, 0x52a1e5fb, BRF_ESS | BRF_PRG },		//  1
	{ "11m01b.bin",    0x2000, 0xb68a062a, BRF_ESS | BRF_PRG },		//  2
	{ "12n01b.bin",    0x2000, 0x1d3ecee5, BRF_ESS | BRF_PRG },		//  3
	{ "13.1r",          0x2000, 0x70e0244d, BRF_ESS | BRF_PRG },		//  4

	// graphics 3 bit planes:
	{ "03e08t.bin",    0x1000, 0x9f0470d5, BRF_GRA },			 // chars
	{ "04h08t.bin",    0x1000, 0x81ec12e6, BRF_GRA },
	{ "05k08t.bin",    0x1000, 0xe87ec8b1, BRF_GRA },

	{ "14j07b.bin",    0x2000, 0x101c858d, BRF_GRA },			 // sprites
	{ "15l07b.bin",    0x2000, 0x013f58f2, BRF_GRA },
	{ "16m07b.bin",    0x2000, 0x94694097, BRF_GRA },

	{ "06l08t.bin",    0x2000, 0x51eebd89, BRF_GRA },			 // background tiles
	{ "07n08t.bin",    0x2000, 0x9dd98e9d, BRF_GRA },
	{ "08r08t.bin",    0x2000, 0x3155ee7d, BRF_GRA },

	{ "02p04t.bin",    0x1000, 0x398d4a02, BRF_GRA },			 // background tilemaps

	{ "01h03t.bin",    0x2000, 0x8407917d, BRF_ESS | BRF_SND },		// sound CPU
};

STD_ROM_PICK(Bombjack)
STD_ROM_FN(Bombjack)

/*
// Bomb Jack (set 2)
struct BurnRomInfo Bombjac2RomDesc[] = {
	{ "09_j01b.bin",    0x2000, 0xc668dc30, BRF_ESS | BRF_PRG },		//  0 Z80 code
	{ "10_l01b.bin",    0x2000, 0x52a1e5fb, BRF_ESS | BRF_PRG },		//  1
	{ "11_m01b.bin",    0x2000, 0xb68a062a, BRF_ESS | BRF_PRG },		//  2
	{ "12_n01b.bin",    0x2000, 0x1d3ecee5, BRF_ESS | BRF_PRG },		//  3
	{ "13_r01b.bin",    0x2000, 0xbcafdd29, BRF_ESS | BRF_PRG },		//  4

	// graphics 3 bit planes:
	{ "03_e08t.bin",    0x1000, 0x9f0470d5, BRF_GRA },			 // chars
	{ "04_h08t.bin",    0x1000, 0x81ec12e6, BRF_GRA },
	{ "05_k08t.bin",    0x1000, 0xe87ec8b1, BRF_GRA },

	{ "14_j07b.bin",    0x2000, 0x101c858d, BRF_GRA },			 // sprites
	{ "15_l07b.bin",    0x2000, 0x013f58f2, BRF_GRA },
	{ "16_m07b.bin",    0x2000, 0x94694097, BRF_GRA },

	{ "06_l08t.bin",    0x2000, 0x51eebd89, BRF_GRA },			 // background tiles
	{ "07_n08t.bin",    0x2000, 0x9dd98e9d, BRF_GRA },
	{ "08_r08t.bin",    0x2000, 0x3155ee7d, BRF_GRA },

	{ "02_p04t.bin",    0x1000, 0x398d4a02, BRF_GRA },			 // background tilemaps

	{ "01_h03t.bin",    0x2000, 0x8407917d, BRF_ESS | BRF_SND },		// sound CPU
};

STD_ROM_PICK(Bombjac2)
STD_ROM_FN(Bombjac2)
*/
/*
struct BurnDriver BurnDrvBombjack = {
	"bombjack", NULL, NULL, NULL, "1984",
	"Bomb Jack (set 1)\0", NULL, "Tehkan", "Bomb Jack",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING,2,HARDWARE_MISC_PRE90S, GBF_PLATFORM, 0,
	NULL, BombjackRomInfo,BombjackRomName, NULL, NULL,DrvInputInfo,DrvDipsInfo,
	BjInit,BjExit,BjFrame,NULL,BjScan,
	NULL,0x80,224,256,3,4
};

struct BurnDriver BurnDrvBombjac2 = {
	"bombjack2", "bombjack", NULL, NULL, "1984",
	"Bomb Jack (set 2)\0", NULL, "Tehkan", "Bomb Jack",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE,2,HARDWARE_MISC_PRE90S, GBF_PLATFORM, 0,
	NULL, Bombjac2RomInfo,Bombjac2RomName, NULL, NULL,DrvInputInfo,DrvDipsInfo,
	BjInit,BjExit,BjFrame,NULL,BjScan,
	NULL,0x80,224,256,3,4
};

struct BurnDriver BurnDrvBombjackt = {
	"bombjackt", "bombjack", NULL, NULL, "1984",
	"Bomb Jack (Tecfri, Spain)\0", NULL, "Tehkan (Tecfri License)", "Bomb Jack",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE,2,HARDWARE_MISC_PRE90S, GBF_PLATFORM, 0,
	NULL, BombjacktRomInfo,BombjacktRomName, NULL, NULL,DrvInputInfo,DrvDipsInfo,
	BjtInit,BjExit,BjFrame,NULL,BjScan,
	NULL,0x80,224,256,3,4
};
*/
#endif