#ifndef _D_APPOOOH_H_
#define _D_APPOOOH_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "sn76496.h"
#include "msm5205.h"
#define SOUND_LEN 192
int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));
static INT32 DrvInit();
static INT32 DrvRobowresInit();
static INT32 DrvExit();
static INT32 DrvFrame();
static INT32 DrvDraw();
static void make_lut(void);
static void DrvInitSaturn();

#include "sn76496.h"
//#include "msm5205.h"

static INT32 DrvZ80Bank0;
static UINT8 DrvJoy1[8];
static UINT8 DrvJoy2[8];
static UINT8 DrvJoy3[8];
static UINT8 DrvInputs[3];
static UINT8 DrvDip[2];
static UINT8 DrvReset;
static UINT8 DrvRecalc;

static UINT8 *AllMem;
static UINT8 *MemEnd;
static UINT8 *AllRam;
static UINT8 *RamEnd;
static UINT8 *DrvRAM0;
static UINT8 *DrvRAM1;
static UINT8 *DrvRAM2;
static UINT8 *DrvFgVidRAM;
static UINT8 *DrvBgVidRAM;
static UINT8 *DrvSprRAM0;
static UINT8 *DrvSprRAM1;
static UINT8 *DrvFgColRAM;
static UINT8 *DrvBgColRAM;
static UINT8 *DrvGfxROM0;
static UINT8 *DrvGfxROM1;
static UINT8 *DrvGfxROM2;
static UINT8 *DrvGfxROM3;
static UINT8 *DrvGfxTMP0;
static UINT8 *DrvGfxTMP1;
static UINT8 *DrvColPROM;
static UINT8 *DrvMainROM;
static UINT8 *DrvSoundROM;
static UINT8 *DrvFetch;
//static UINT32 *DrvPalette;
static UINT16 *DrvPalette;
static UINT16 *map_offset_lut = NULL;
static UINT16 *charaddr_lut = NULL;
static UINT8 scroll_x;
static UINT8 flipscreen;
static UINT8 priority;
static UINT8 interrupt_enable;
static UINT32 adpcm_data;
static UINT32 adpcm_address;
static INT32 nCyclesTotal;
static INT32 game_select; // 1 = robowres

static struct BurnInputInfo AppooohInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 5,	"p1 coin"},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy2 + 5,	"p1 start"},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 left"},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 right"},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 7,	"p1 fire 2"},
	{"P1 Button 3",		BIT_DIGITAL,	DrvJoy3 + 0,	"p1 fire 3"},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy1 + 6,	"p2 coin"},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy2 + 6,	"p2 start"},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 left"},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 right"},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy2 + 4,	"p2 fire 1"},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy2 + 7,	"p2 fire 2"},
	{"P2 Button 3",		BIT_DIGITAL,	DrvJoy3 + 1,	"p2 fire 3"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDip + 0,	"dip"},
};

STDINPUTINFO(Appoooh)


static struct BurnDIPInfo AppooohDIPList[]=
{
	{0x13, 0xff, 0xff, 0x60, NULL		},

	{0   , 0xfe, 0   ,    8, "Coin A"		},
	{0x13, 0x01, 0x07, 0x03, "4 Coins 1 Credits"		},
	{0x13, 0x01, 0x07, 0x02, "3 Coins 1 Credits"		},
	{0x13, 0x01, 0x07, 0x01, "2 Coins 1 Credits"		},
	{0x13, 0x01, 0x07, 0x00, "1 Coin  1 Credits"		},
	{0x13, 0x01, 0x07, 0x07, "2 Coins 3 Credits"		},
	{0x13, 0x01, 0x07, 0x04, "1 Coin  2 Credits"		},
	{0x13, 0x01, 0x07, 0x05, "1 Coin  3 Credits"		},
	{0x13, 0x01, 0x07, 0x06, "1 Coin  4 Credits"		},

	{0   , 0xfe, 0   ,    4, "Coin B"		},
	{0x13, 0x01, 0x18, 0x18, "3 Coins 1 Credits"		},
	{0x13, 0x01, 0x18, 0x10, "2 Coins 1 Credits"		},
	{0x13, 0x01, 0x18, 0x00, "1 Coin  1 Credits"		},
	{0x13, 0x01, 0x18, 0x08, "1 Coin  2 Credits"		},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x13, 0x01, 0x20, 0x00, "Off"		},
	{0x13, 0x01, 0x20, 0x20, "On"		},

	{0   , 0xfe, 0   ,    2, "Cabinet"		},
	{0x13, 0x01, 0x40, 0x40, "Upright"		},
	{0x13, 0x01, 0x40, 0x00, "Cocktail"		},

	{0   , 0xfe, 0   ,    2, "Difficulty"		},
	{0x13, 0x01, 0x80, 0x00, "Easy"		},
	{0x13, 0x01, 0x80, 0x80, "Hard"		},
};

STDDIPINFO(Appoooh)

static struct BurnDIPInfo RobowresDIPList[]=
{
	{0x13, 0xff, 0xff, 0x60, NULL		},

	{0   , 0xfe, 0   ,    8, "Coin A"		},
	{0x13, 0x01, 0x07, 0x03, "4 Coins 1 Credits"		},
	{0x13, 0x01, 0x07, 0x02, "3 Coins 1 Credits"		},
	{0x13, 0x01, 0x07, 0x01, "2 Coins 1 Credits"		},
	{0x13, 0x01, 0x07, 0x00, "1 Coin  1 Credits"		},
	{0x13, 0x01, 0x07, 0x07, "2 Coins 3 Credits"		},
	{0x13, 0x01, 0x07, 0x04, "1 Coin  2 Credits"		},
	{0x13, 0x01, 0x07, 0x05, "1 Coin  3 Credits"		},
	{0x13, 0x01, 0x07, 0x06, "1 Coin  4 Credits"		},

	{0   , 0xfe, 0   ,    4, "Coin B"		},
	{0x13, 0x01, 0x18, 0x18, "3 Coins 1 Credits"		},
	{0x13, 0x01, 0x18, 0x10, "2 Coins 1 Credits"		},
	{0x13, 0x01, 0x18, 0x00, "1 Coin  1 Credits"		},
	{0x13, 0x01, 0x18, 0x08, "1 Coin  2 Credits"		},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x13, 0x01, 0x20, 0x00, "Off"		},
	{0x13, 0x01, 0x20, 0x20, "On"		},

	{0   , 0xfe, 0   ,    2, "Cabinet"		},
	{0x13, 0x01, 0x40, 0x40, "Upright"		},
	{0x13, 0x01, 0x40, 0x00, "Cocktail"		},

	{0   , 0xfe, 0   ,    0, "Language"		},
	{0x01, 0x01, 0x80, 0x00, "Japanese"		},
	{0x01, 0x01, 0x80, 0x80, "English"		},
};

STDDIPINFO(Robowres)

// Appoooh

static struct BurnRomInfo appooohRomDesc[] = {
	{ "epr5906.bin",	0x2000, 0xfffae7fe, 1 }, //  0 maincpu
	{ "epr5907.bin",	0x2000, 0x57696cd6, 1 }, //  1
	{ "epr5908.bin",	0x2000, 0x4537cddc, 1 }, //  2
	{ "epr5909.bin",	0x2000, 0xcf82718d, 1 }, //  3
	{ "epr5910.bin",	0x2000, 0x312636da, 1 }, //  4
	{ "epr5911.bin",	0x2000, 0x0bc2acaa, 1 }, //  5
	{ "epr5913.bin",	0x2000, 0xf5a0e6a7, 1 }, //  6
	{ "epr5912.bin",	0x2000, 0x3c3915ab, 1 }, //  7
	{ "epr5914.bin",	0x2000, 0x58792d4a, 1 }, //  8

	{ "epr5895.bin",	0x4000, 0x4b0d4294, 2 }, //  9 gfx1
	{ "epr5896.bin",	0x4000, 0x7bc84d75, 2 }, // 10
	{ "epr5897.bin",	0x4000, 0x745f3ffa, 2 }, // 11

	{ "epr5898.bin",	0x4000, 0xcf01644d, 3 }, // 12 gfx2
	{ "epr5899.bin",	0x4000, 0x885ad636, 3 }, // 13
	{ "epr5900.bin",	0x4000, 0xa8ed13f3, 3 }, // 14

	{ "pr5921.prm",		0x0020, 0xf2437229, 4 }, // 15 proms
	{ "pr5922.prm",		0x0100, 0x85c542bf, 4 }, // 16
	{ "pr5923.prm",		0x0100, 0x16acbd53, 4 }, // 17

	{ "epr5901.bin",	0x2000, 0x170a10a4, 5 }, // 18 adpcm
	{ "epr5902.bin",	0x2000, 0xf6981640, 5 }, // 19
	{ "epr5903.bin",	0x2000, 0x0439df50, 5 }, // 20
	{ "epr5904.bin",	0x2000, 0x9988f2ae, 5 }, // 21
	{ "epr5905.bin",	0x2000, 0xfb5cd70e, 5 }, // 22
};

STD_ROM_PICK(appoooh)
STD_ROM_FN(appoooh)


static struct BurnRomInfo robowresRomDesc[] = {
	{ "epr7540.13d",	0x8000, 0xa2a54237, 1 }, //  0 maincpu
	{ "epr7541.14d",	0x8000, 0xcbf7d1a8, 1 }, //  1
	{ "epr7542.15d",	0x8000, 0x3475fbd4, 1 }, //  2

	{ "epr7544.7h",	0x8000, 0x07b846ce, 2 }, //  3 gfx1
	{ "epr7545.6h",	0x8000, 0xe99897be, 2 }, // 4
	{ "epr7546.5h",	0x8000, 0x1559235a, 2 }, // 5

	{ "epr7547.7d",	0x8000, 0xb87ad4a4, 3 }, // 6 gfx2
	{ "epr7548.6d",	0x8000, 0x8b9c75b3, 3 }, // 7
	{ "epr7549.5d",	0x8000, 0xf640afbb, 3 }, // 8

	{ "pr7571.10a",	0x0020, 0xe82c6d5c, 4 }, // 9 proms
	{ "pr7572.7f",		0x0100, 0x2b083d0c, 4 }, // 10
	{ "pr7573.7g",	0x0100, 0x2b083d0c, 4 }, // 11

	{ "epr7543",		0x8000, 0x4d108c49, 5 }, // 18 adpcm
};

STD_ROM_PICK(robowres)
STD_ROM_FN(robowres)
#endif
