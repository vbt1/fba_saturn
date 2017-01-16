#ifndef _D_WIZ_H_
#define _D_WIZ_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "snd/ay8910.h"

#define nBurnSoundLen 128

UINT16 *map_offset_lut = NULL;

int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));
/*static*/ void DrvInitSaturn();
static INT32 WizInit();
static INT32 KungfutInit();
static INT32 StingerInit();
static INT32 ScionInit();

static INT32 DrvInit(int (*RomLoadCallback)(), int rotated);
static INT32  DrvExit();
static INT32  DrvFrame();
//static INT32  DrvDraw();

static void make_lut(int rotated);
INT32 (*DrvDraw)();

static UINT8 *AllMem = NULL;
static UINT8 *MemEnd = NULL;
static UINT8 *AllRam = NULL;
static UINT8 *RamEnd = NULL;
static UINT8 *DrvZ80ROM0 = NULL;
static UINT8 *DrvZ80Dec = NULL;
static UINT8 *DrvZ80ROM1 = NULL;
static UINT8 *DrvGfxROM0 = NULL;
static UINT8 *DrvGfxROM0b = NULL;
static UINT8 *DrvGfxROM1 = NULL;
static UINT8 *DrvColPROM = NULL;
static UINT8 *DrvZ80RAM0 = NULL;
static UINT8 *DrvZ80RAM1 = NULL;
static UINT8 *DrvVidRAM0 = NULL;
static UINT8 *DrvVidRAM1 = NULL;
static UINT8 *DrvColRAM0 = NULL;
static UINT8 *DrvColRAM1 = NULL;
static UINT8 *DrvSprRAM0 = NULL;
static UINT8 *DrvSprRAM1 = NULL;

static UINT16 *DrvPalette = NULL;
//static UINT8 DrvRecalc;

static UINT8 soundlatch = 0;
static UINT8 *sprite_bank = NULL;
static UINT8 *interrupt_enable = NULL;
static UINT8 *palette_bank = NULL;
static UINT8 *char_bank_select = NULL;
static UINT8 *screen_flip = NULL;
static UINT8 *background_color = NULL;

static INT16* pFMBuffer = NULL;
static INT16* pAY8910Buffer[9];

static UINT8 DrvInputs[2] = {0,0};
static UINT8 DrvJoy1[8] = {0,0,0,0,0,0,0,0};
static UINT8 DrvJoy2[8] = {0,0,0,0,0,0,0,0};
static UINT8 DrvDips[2] = {0,0};
static UINT8 DrvReset = 0;
static UINT8 Wizmode = 0;
static UINT8 Scionmodeoffset = 0;

static struct BurnInputInfo WizInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 coin"	},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 start"	},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy2 + 2,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy2 + 3,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 2"	},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy1 + 5,	"p2 coin"	},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy1 + 3,	"p2 start"	},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 6,	"p2 up"		},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 7,	"p2 down"	},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 5,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 4,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy1 + 1,	"p2 fire 1"	},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy1 + 7,	"p2 fire 2"	},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"		},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"		},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
};

STDINPUTINFO(Wiz)

static struct BurnInputInfo ScionInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 coin"},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 start"},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy2 + 4,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy2 + 5,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p1 left"},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 right"},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 1"},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 fire 2"},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy1 + 5,	"p2 coin"},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy1 + 3,	"p2 start"},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 6,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 7,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 left"},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 right"},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy1 + 7,	"p2 fire 1"},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy1 + 1,	"p2 fire 2"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"},
};

STDINPUTINFO(Scion)

static struct BurnInputInfo StingerInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 coin"},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 start"},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy2 + 4,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy2 + 5,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p1 left"},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 right"},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 1"},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 fire 2"},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy1 + 5,	"p2 coin"},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy1 + 3,	"p2 start"},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 6,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 7,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 left"},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 right"},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy1 + 7,	"p2 fire 1"},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy1 + 1,	"p2 fire 2"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"},
};

STDINPUTINFO(Stinger)

static struct BurnInputInfo KungfutInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 coin"},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 start"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p1 left"},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 right"},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 fire 1"},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 fire 2"},
	{"P1 Button 3",		BIT_DIGITAL,	DrvJoy2 + 4,	"p1 fire 3"},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy1 + 5,	"p2 coin"},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy1 + 3,	"p2 start"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 6,	"p2 left"},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 right"},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy1 + 7,	"p2 fire 1"},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy1 + 1,	"p2 fire 2"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"},
};

STDINPUTINFO(Kungfut)

static struct BurnDIPInfo StingerDIPList[]=
{
	{0x11, 0xff, 0xff, 0xef, NULL		},
	{0x12, 0xff, 0xff, 0xae, NULL		},

	{0   , 0xfe, 0   ,    8, "Coin A"		},
	{0x11, 0x01, 0x07, 0x01, "4 Coins 1 Credits"		},
	{0x11, 0x01, 0x07, 0x02, "3 Coins 1 Credits"		},
	{0x11, 0x01, 0x07, 0x04, "2 Coins 1 Credits"		},
	{0x11, 0x01, 0x07, 0x00, "3 Coins 2 Credits"		},
	{0x11, 0x01, 0x07, 0x07, "1 Coin  1 Credits"		},
	{0x11, 0x01, 0x07, 0x03, "2 Coins 3 Credits"		},
	{0x11, 0x01, 0x07, 0x06, "1 Coin  2 Credits"		},
	{0x11, 0x01, 0x07, 0x05, "1 Coin  3 Credits"		},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x11, 0x01, 0x18, 0x00, "2"		},
	{0x11, 0x01, 0x18, 0x08, "3"		},
	{0x11, 0x01, 0x18, 0x10, "4"		},
	{0x11, 0x01, 0x18, 0x18, "5"		},

	{0   , 0xfe, 0   ,    8, "Bonus Life"		},
	{0x11, 0x01, 0xe0, 0xe0, "20000 50000"		},
	{0x11, 0x01, 0xe0, 0xc0, "20000 60000"		},
	{0x11, 0x01, 0xe0, 0xa0, "20000 70000"		},
	{0x11, 0x01, 0xe0, 0x80, "20000 80000"		},
	{0x11, 0x01, 0xe0, 0x60, "20000 90000"		},
	{0x11, 0x01, 0xe0, 0x40, "30000 80000"		},
	{0x11, 0x01, 0xe0, 0x20, "30000 90000"		},
	{0x11, 0x01, 0xe0, 0x00, "None"		},

	{0   , 0xfe, 0   ,    2, "Debug Mode"		},
	{0x12, 0x01, 0x01, 0x00, "Off"		},
	{0x12, 0x01, 0x01, 0x01, "On"		},

	{0   , 0xfe, 0   ,    8, "Coin B"		},
	{0x12, 0x01, 0x0e, 0x00, "5 Coins 1 Credits"		},
	{0x12, 0x01, 0x0e, 0x08, "4 Coins 1 Credits"		},
	{0x12, 0x01, 0x0e, 0x0c, "3 Coins 1 Credits"		},
	{0x12, 0x01, 0x0e, 0x0a, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0x0e, 0x04, "3 Coins 2 Credits"		},
	{0x12, 0x01, 0x0e, 0x0e, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0x0e, 0x02, "2 Coins 3 Credits"		},
	{0x12, 0x01, 0x0e, 0x06, "1 Coin  2 Credits"		},

	{0   , 0xfe, 0   ,    4, "Bongo Time"		},
	{0x12, 0x01, 0x30, 0x30, "Long"		},
	{0x12, 0x01, 0x30, 0x20, "Medium"		},
	{0x12, 0x01, 0x30, 0x10, "Short"		},
	{0x12, 0x01, 0x30, 0x00, "Shortest"		},

	{0   , 0xfe, 0   ,    2, "Difficulty"		},
	{0x12, 0x01, 0x40, 0x00, "Normal"		},
	{0x12, 0x01, 0x40, 0x40, "Hard"		},

	{0   , 0xfe, 0   ,    2, "Cabinet"		},
	{0x12, 0x01, 0x80, 0x80, "Upright"		},
	{0x12, 0x01, 0x80, 0x00, "Cocktail"		},
};

STDDIPINFO(Stinger)

static struct BurnDIPInfo Stinger2DIPList[]=
{
	{0x11, 0xff, 0xff, 0xef, NULL		},
	{0x12, 0xff, 0xff, 0xa0, NULL		},

	{0   , 0xfe, 0   ,    8, "Coin A"		},
	{0x11, 0x01, 0x07, 0x01, "4 Coins 1 Credits"		},
	{0x11, 0x01, 0x07, 0x02, "3 Coins 1 Credits"		},
	{0x11, 0x01, 0x07, 0x04, "2 Coins 1 Credits"		},
	{0x11, 0x01, 0x07, 0x00, "3 Coins 2 Credits"		},
	{0x11, 0x01, 0x07, 0x07, "1 Coin  1 Credits"		},
	{0x11, 0x01, 0x07, 0x03, "2 Coins 3 Credits"		},
	{0x11, 0x01, 0x07, 0x06, "1 Coin  2 Credits"		},
	{0x11, 0x01, 0x07, 0x05, "1 Coin  3 Credits"		},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x11, 0x01, 0x18, 0x00, "2"		},
	{0x11, 0x01, 0x18, 0x08, "3"		},
	{0x11, 0x01, 0x18, 0x10, "4"		},
	{0x11, 0x01, 0x18, 0x18, "5"		},

	{0   , 0xfe, 0   ,    8, "Bonus Life"		},
	{0x11, 0x01, 0xe0, 0xe0, "20000 50000"		},
	{0x11, 0x01, 0xe0, 0xc0, "20000 60000"		},
	{0x11, 0x01, 0xe0, 0xa0, "20000 70000"		},
	{0x11, 0x01, 0xe0, 0x80, "20000 80000"		},
	{0x11, 0x01, 0xe0, 0x60, "20000 90000"		},
	{0x11, 0x01, 0xe0, 0x40, "30000 80000"		},
	{0x11, 0x01, 0xe0, 0x20, "30000 90000"		},
	{0x11, 0x01, 0xe0, 0x00, "None"		},

	{0   , 0xfe, 0   ,    2, "Debug Mode"		},
	{0x12, 0x01, 0x01, 0x00, "Off"		},
	{0x12, 0x01, 0x01, 0x01, "On"		},

	{0   , 0xfe, 0   ,    2, "Unused"		},
	{0x12, 0x01, 0x02, 0x00, "Off"		},
	{0x12, 0x01, 0x02, 0x02, "On"		},

	{0   , 0xfe, 0   ,    2, "Unused"		},
	{0x12, 0x01, 0x04, 0x00, "Off"		},
	{0x12, 0x01, 0x04, 0x04, "On"		},

	{0   , 0xfe, 0   ,    2, "Unused"		},
	{0x12, 0x01, 0x08, 0x00, "Off"		},
	{0x12, 0x01, 0x08, 0x08, "On"		},

	{0   , 0xfe, 0   ,    8, "Coin B"		},
	{0x12, 0x01, 0x70, 0x70, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0x70, 0x60, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0x70, 0x50, "1 Coin  3 Credits"		},
	{0x12, 0x01, 0x70, 0x40, "1 Coin  4 Credits"		},
	{0x12, 0x01, 0x70, 0x30, "1 Coin  5 Credits"		},
	{0x12, 0x01, 0x70, 0x20, "1 Coin  6 Credits"		},
	{0x12, 0x01, 0x70, 0x10, "1 Coin  7 Credits"		},
	{0x12, 0x01, 0x70, 0x00, "1 Coin  8 Credits"		},

	{0   , 0xfe, 0   ,    2, "Cabinet"		},
	{0x12, 0x01, 0x80, 0x80, "Upright"		},
	{0x12, 0x01, 0x80, 0x00, "Cocktail"		},
};

STDDIPINFO(Stinger2)

static struct BurnDIPInfo ScionDIPList[]=
{
	{0x11, 0xff, 0xff, 0x05, NULL		},
	{0x12, 0xff, 0xff, 0x00, NULL		},

	{0   , 0xfe, 0   ,    2, "Cabinet"		},
	{0x11, 0x01, 0x01, 0x01, "Upright"		},
	{0x11, 0x01, 0x01, 0x00, "Cocktail"		},

	{0   , 0xfe, 0   ,    2, "Difficulty"		},
	{0x11, 0x01, 0x02, 0x00, "Easy"		},
	{0x11, 0x01, 0x02, 0x02, "Hard"		},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x11, 0x01, 0x0c, 0x00, "2"		},
	{0x11, 0x01, 0x0c, 0x04, "3"		},
	{0x11, 0x01, 0x0c, 0x08, "4"		},
	{0x11, 0x01, 0x0c, 0x0c, "5"		},

	{0   , 0xfe, 0   ,    4, "Bonus Life"		},
	{0x11, 0x01, 0x30, 0x00, "20000 40000"		},
	{0x11, 0x01, 0x30, 0x20, "20000 60000"		},
	{0x11, 0x01, 0x30, 0x10, "20000 80000"		},
	{0x11, 0x01, 0x30, 0x30, "30000 90000"		},

	{0   , 0xfe, 0   ,    2, "Unused"		},
	{0x11, 0x01, 0x40, 0x40, "Off"		},
	{0x11, 0x01, 0x40, 0x00, "On"		},

	{0   , 0xfe, 0   ,    2, "Unused"		},
	{0x11, 0x01, 0x80, 0x80, "Off"		},
	{0x11, 0x01, 0x80, 0x00, "On"		},

	{0   , 0xfe, 0   ,    8, "Coin A"		},
	{0x12, 0x01, 0x07, 0x07, "5 Coins 1 Credits"		},
	{0x12, 0x01, 0x07, 0x03, "4 Coins 1 Credits"		},
	{0x12, 0x01, 0x07, 0x05, "3 Coins 1 Credits"		},
	{0x12, 0x01, 0x07, 0x01, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0x07, 0x00, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0x07, 0x04, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0x07, 0x02, "1 Coin  3 Credits"		},
	{0x12, 0x01, 0x07, 0x06, "1 Coin  5 Credits"		},

	{0   , 0xfe, 0   ,    4, "Coin B"		},
	{0x12, 0x01, 0x18, 0x18, "3 Coins 1 Credits"		},
	{0x12, 0x01, 0x18, 0x08, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0x18, 0x00, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0x18, 0x10, "1 Coin  2 Credits"		},

	{0   , 0xfe, 0   ,    1, "Unused"		},
	{0x12, 0x01, 0x20, 0x00, "Off"		},

	{0   , 0xfe, 0   ,    1, "Unused"		},
	{0x12, 0x01, 0x40, 0x00, "Off"		},

	{0   , 0xfe, 0   ,    1, "Unused"		},
	{0x12, 0x01, 0x80, 0x00, "Off"		},
};

STDDIPINFO(Scion)

static struct BurnDIPInfo KungfutDIPList[]=
{
	{0x0e, 0xff, 0xff, 0x20, NULL		},
	{0x0f, 0xff, 0xff, 0x0c, NULL		},

	{0   , 0xfe, 0   ,    8, "Coin A"		},
	{0x0e, 0x01, 0x07, 0x07, "5 Coins 1 Credits"		},
	{0x0e, 0x01, 0x07, 0x03, "4 Coins 1 Credits"		},
	{0x0e, 0x01, 0x07, 0x05, "3 Coins 1 Credits"		},
	{0x0e, 0x01, 0x07, 0x01, "2 Coins 1 Credits"		},
	{0x0e, 0x01, 0x07, 0x00, "1 Coin  1 Credits"		},
	{0x0e, 0x01, 0x07, 0x04, "1 Coin  2 Credits"		},
	{0x0e, 0x01, 0x07, 0x02, "1 Coin  3 Credits"		},
	{0x0e, 0x01, 0x07, 0x06, "1 Coin  5 Credits"		},

	{0   , 0xfe, 0   ,    4, "Coin B"		},
	{0x0e, 0x01, 0x18, 0x18, "3 Coins 1 Credits"		},
	{0x0e, 0x01, 0x18, 0x08, "2 Coins 1 Credits"		},
	{0x0e, 0x01, 0x18, 0x00, "1 Coin  1 Credits"		},
	{0x0e, 0x01, 0x18, 0x10, "1 Coin  2 Credits"		},

	{0   , 0xfe, 0   ,    2, "2 Players Game"		},
	{0x0e, 0x01, 0x20, 0x00, "1 Credit"		},
	{0x0e, 0x01, 0x20, 0x20, "2 Credits"		},

	{0   , 0xfe, 0   ,    2, "Unused"		},
	{0x0e, 0x01, 0x40, 0x00, "Off"		},
	{0x0e, 0x01, 0x40, 0x40, "On"		},

	{0   , 0xfe, 0   ,    2, "Unused"		},
	{0x0e, 0x01, 0x80, 0x00, "Off"		},
	{0x0e, 0x01, 0x80, 0x80, "On"		},

	{0   , 0xfe, 0   ,    2, "Difficulty"		},
	{0x0f, 0x01, 0x01, 0x00, "Easy"		},
	{0x0f, 0x01, 0x01, 0x01, "Hard"		},

	{0   , 0xfe, 0   ,    2, "Unused"		},
	{0x0f, 0x01, 0x02, 0x00, "Off"		},
	{0x0f, 0x01, 0x02, 0x02, "On"		},

	{0   , 0xfe, 0   ,    2, "Microphone"		},
	{0x0f, 0x01, 0x04, 0x04, "Off"		},
	{0x0f, 0x01, 0x04, 0x00, "On"		},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x0f, 0x01, 0x18, 0x00, "2"		},
	{0x0f, 0x01, 0x18, 0x08, "3"		},
	{0x0f, 0x01, 0x18, 0x10, "4"		},
	{0x0f, 0x01, 0x18, 0x18, "5"		},

	{0   , 0xfe, 0   ,    3, "Bonus Life"		},
	{0x0f, 0x01, 0x60, 0x00, "20000 40000"		},
	{0x0f, 0x01, 0x60, 0x10, "20000 80000"		},
	{0x0f, 0x01, 0x60, 0x30, "30000 90000"		},

	{0   , 0xfe, 0   ,    2, "Unused"		},
	{0x0f, 0x01, 0x80, 0x00, "Off"		},
	{0x0f, 0x01, 0x80, 0x80, "On"		},
};

STDDIPINFO(Kungfut)


static struct BurnDIPInfo WizDIPList[]=
{
	{0x11, 0xff, 0xff, 0x00, NULL			},
	{0x12, 0xff, 0xff, 0x10, NULL			},

	{0   , 0xfe, 0   ,    8, "Coin A"		},
	{0x11, 0x01, 0x07, 0x07, "5 Coins 1 Credits"	},
	{0x11, 0x01, 0x07, 0x03, "4 Coins 1 Credits"	},
	{0x11, 0x01, 0x07, 0x05, "3 Coins 1 Credits"	},
	{0x11, 0x01, 0x07, 0x01, "2 Coins 1 Credits"	},
	{0x11, 0x01, 0x07, 0x00, "1 Coin  1 Credits"	},
	{0x11, 0x01, 0x07, 0x04, "1 Coin  2 Credits"	},
	{0x11, 0x01, 0x07, 0x02, "1 Coin  3 Credits"	},
	{0x11, 0x01, 0x07, 0x06, "1 Coin  5 Credits"	},

	{0   , 0xfe, 0   ,    4, "Coin B"		},
	{0x11, 0x01, 0x18, 0x08, "2 Coins 1 Credits"	},
	{0x11, 0x01, 0x18, 0x00, "1 Coin  1 Credits"	},
	{0x11, 0x01, 0x18, 0x18, "2 Coins 3 Credits"	},
	{0x11, 0x01, 0x18, 0x10, "1 Coin  2 Credits"	},

	{0   , 0xfe, 0   ,    2, "Free Play"		},
	{0x11, 0x01, 0x80, 0x00, "Off"			},
	{0x11, 0x01, 0x80, 0x80, "On"			},

	{0   , 0xfe, 0   ,    2, "Cabinet"		},
	{0x12, 0x01, 0x01, 0x00, "Upright"		},
	{0x12, 0x01, 0x01, 0x01, "Cocktail"		},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x12, 0x01, 0x06, 0x00, "Easy"			},
	{0x12, 0x01, 0x06, 0x02, "Normal"		},
	{0x12, 0x01, 0x06, 0x04, "Hard"			},
	{0x12, 0x01, 0x06, 0x06, "Hardest"		},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x12, 0x01, 0x18, 0x08, "1"			},
	{0x12, 0x01, 0x18, 0x10, "3"			},
	{0x12, 0x01, 0x18, 0x18, "5"			},
	{0x12, 0x01, 0x18, 0x00, "255 (Cheat)"		},

	{0   , 0xfe, 0   ,    4, "Bonus Life"		},
	{0x12, 0x01, 0x60, 0x00, "10000 30000"		},
	{0x12, 0x01, 0x60, 0x20, "20000 40000"		},
	{0x12, 0x01, 0x60, 0x40, "30000 60000"		},
	{0x12, 0x01, 0x60, 0x60, "40000 80000"		},
};

STDDIPINFO(Wiz)

static struct BurnRomInfo wizRomDesc[] = {
	{ "ic07_01.bin",	0x4000, 0xc05f2c78, 1 }, //  0 maincpu
	{ "ic05_03.bin",	0x4000, 0x7978d879, 1 }, //  1
	{ "ic06_02.bin",	0x4000, 0x9c406ad2, 1 }, //  2

	{ "ic57_10.bin",	0x2000, 0x8a7575bd, 2 }, //  3 audiocpu

	{ "ic12_04.bin",	0x2000, 0x8969acdd, 3 }, //  4 gfx1
	{ "ic13_05.bin",	0x2000, 0x2868e6a5, 3 }, //  5
	{ "ic14_06.bin",	0x2000, 0xb398e142, 3 }, //  6

	{ "ic03_07.bin",	0x4000, 0x297c02fc, 4 }, //  7 gfx2
	{ "ic02_08.bin",	0x4000, 0xede77d37, 4 }, //  8
	{ "ic01_09.bin",	0x4000, 0x4d86b041, 4 }, //  9

	{ "ic23_31.bin",	0x0100, 0x2dd52fb2, 5 }, // 10 proms
	{ "ic23_32.bin",	0x0100, 0x8c2880c9, 5 }, // 11
	{ "ic23_33.bin",	0x0100, 0xa488d761, 5 }, // 12
};

STD_ROM_PICK(wiz)
STD_ROM_FN(wiz)

// Kung-Fu Taikun

static struct BurnRomInfo kungfutRomDesc[] = {
	{ "p1.bin",	0x4000, 0xb1e56960, 1 }, //  0 maincpu
	{ "p3.bin",	0x4000, 0x6fc346f8, 1 }, //  1
	{ "p2.bin",	0x4000, 0x042cc9c5, 1 }, //  2

	{ "1.bin",	0x2000, 0x68028a5d, 2 }, //  3 audiocpu

	{ "2.bin",	0x2000, 0x5c3ef697, 3 }, //  4 gfx1
	{ "3.bin",	0x2000, 0x905e81fa, 3 }, //  5
	{ "4.bin",	0x2000, 0x965bb5d1, 3 }, //  6

	{ "5.bin",	0x2000, 0x763bb61a, 4 }, //  7 gfx2
	{ "6.bin",	0x2000, 0xc9649fce, 4 }, //  8
	{ "7.bin",	0x2000, 0x32f02c13, 4 }, //  9

	{ "82s129.0",	0x0100, 0xeb823177, 5 }, // 10 proms
	{ "82s129.1",	0x0100, 0x6eec5dd9, 5 }, // 11
	{ "82s129.2",	0x0100, 0xc31eb3e6, 5 }, // 12
};

STD_ROM_PICK(kungfut)
STD_ROM_FN(kungfut)

// Stinger

static struct BurnRomInfo stingerRomDesc[] = {
	{ "15j.bin",	0x2000, 0x1a2ca600, 1 }, //  0 maincpu
	{ "26j.bin",	0x2000, 0x957cd39c, 1 }, //  1
	{ "38j.bin",	0x2000, 0x404c932e, 1 }, //  2
	{ "49j.bin",	0x2000, 0x2d570f91, 1 }, //  3
	{ "510j.bin",	0x2000, 0xc841795c, 1 }, //  4

	{ "69f.bin",	0x2000, 0x79757f0c, 2 }, //  5 audiocpu

	{ "79e.bin",	0x2000, 0x775489be, 3 }, //  6 gfx1
	{ "811e.bin",	0x2000, 0x43c61b3f, 3 }, //  7
	{ "914e.bin",	0x2000, 0xc9ed8fc7, 3 }, //  8

	{ "109h.bin",	0x2000, 0x6fc3a22d, 4 }, //  9 gfx2
	{ "1111h.bin",	0x2000, 0x3df1f57e, 4 }, // 10
	{ "1214h.bin",	0x2000, 0x2fbe1391, 4 }, // 11

	{ "stinger.a7",	0x0100, 0x52c06fc2, 5 }, // 12 proms
	{ "stinger.b7",	0x0100, 0x9985e575, 5 }, // 13
	{ "stinger.a8",	0x0100, 0x76b57629, 5 }, // 14
};

STD_ROM_PICK(stinger)
STD_ROM_FN(stinger)

// Scion

static struct BurnRomInfo scionRomDesc[] = {
	{ "sc1.bin",	0x2000, 0x8dcad575, 1 }, //  0 maincpu
	{ "sc2.bin",	0x2000, 0xf608e0ba, 1 }, //  1
	{ "sc3.bin",	0x2000, 0x915289b9, 1 }, //  2
	{ "4.9j",	0x2000, 0x0f40d002, 1 }, //  3
	{ "5.10j",	0x2000, 0xdc4923b7, 1 }, //  4

	{ "sc6.bin",	0x2000, 0x09f5f9c1, 2 }, //  5 audiocpu

	{ "7.10e",	0x2000, 0x223e0d2a, 3 }, //  6 gfx1
	{ "8.12e",	0x2000, 0xd3e39b48, 3 }, //  7
	{ "9.15e",	0x2000, 0x630861b5, 3 }, //  8

	{ "10.10h",	0x2000, 0x0d2a0d1e, 4 }, //  9 gfx2
	{ "11.12h",	0x2000, 0xdc6ef8ab, 4 }, // 10
	{ "12.15h",	0x2000, 0xc82c28bf, 4 }, // 11

	{ "82s129.7a",	0x0100, 0x2f89d9ea, 5 }, // 12 proms
	{ "82s129.7b",	0x0100, 0xba151e6a, 5 }, // 13
	{ "82s129.8a",	0x0100, 0xf681ce59, 5 }, // 14
};

STD_ROM_PICK(scion)
STD_ROM_FN(scion)

#endif