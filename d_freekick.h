#ifndef _D_FREEKICK_H_
#define _D_FREEKICK_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "sn76496.h"
#include "bitswap.h"
#include "mc8123.h"
#include "8255ppi.h"

#define nBurnSoundLen 128
int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));
static INT32 DrvInit();
static INT32 DrvFreeKickInit();
static INT32 pbillrdInit();
static INT32 DrvExit();
static INT32 DrvFrame();
static INT32 DrvDraw();
static void DrvInitSaturn();
static void make_lut(void);

static UINT8 DrvJoy1[8];
static UINT8 DrvJoy2[8];

static UINT8 DrvInputs[2];
static UINT8 DrvDip[3];
static INT16 DrvDial1;
static INT16 DrvDial2;
static UINT8 DrvReset;

static UINT8 DrvRecalc;

static UINT8 *AllMem;
static UINT8 *MemEnd;
static UINT8 *AllRam;
static UINT8 *RamEnd;
static UINT8 *DrvRAM;
static UINT8 *DrvMainROM;
static UINT8 *DrvMainROMdec;
static UINT8 *DrvSndROM;
static UINT8 *DrvVidRAM;
static UINT8 *DrvSprRAM;
static UINT8 *DrvColRAM;
static UINT8 *DrvGfxROM0;
static UINT8 *DrvGfxROM1;
static UINT8 *DrvGfxTMP0;
static UINT8 *DrvGfxTMP1;
static UINT8 *DrvColPROM;
static UINT8 *MC8123Key;
static UINT16 *DrvPalette;
static UINT8 DrvZ80Bank0;
typedef void (*RenderSprite)(INT32);
static RenderSprite DrawSprite;

static UINT8 nmi_enable;
static UINT8 flipscreen;
static UINT8 coin;
static UINT8 spinner;
static UINT8 ff_data;
static UINT16 romaddr;

static UINT8 use_encrypted = 0;
static UINT8 countrunbmode = 0;
static UINT8 pbillrdmode = 0;
static UINT16 *map_offset_lut = NULL;

static void DrvPaletteInit();

static struct BurnInputInfo PbillrdInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 7,	"p1 coin"},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 start"},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 5,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 left"},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 right"},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 fire 1"},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 fire 2"},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy2 + 7,	"p2 coin"},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy2 + 6,	"p2 start"},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 5,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 4,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 left"},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 right"},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 fire 1"},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 fire 2"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDip + 0,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDip + 1,	"dip"},
};

STDINPUTINFO(Pbillrd)


static struct BurnDIPInfo PbillrdDIPList[]=
{
	{0x11, 0xff, 0xff, 0x1f, NULL		},
	{0x12, 0xff, 0xff, 0xff, NULL		},

	{0   , 0xfe, 0   ,    2, "Balls"		},
	{0x11, 0x01, 0x01, 0x01, "3"		},
	{0x11, 0x01, 0x01, 0x00, "5"		},

	{0   , 0xfe, 0   ,    4, "Bonus Ball"		},
	{0x11, 0x01, 0x06, 0x06, "10000, 30000 & 50000 Points"		},
	{0x11, 0x01, 0x06, 0x02, "20000 & 60000 Points"		},
	{0x11, 0x01, 0x06, 0x04, "30000 & 80000 Points"		},
	{0x11, 0x01, 0x06, 0x00, "Only 20000 Points"		},

	{0   , 0xfe, 0   ,    2, "Allow Continue"		},
	{0x11, 0x01, 0x10, 0x00, "No"		},
	{0x11, 0x01, 0x10, 0x10, "Yes"		},

	{0   , 0xfe, 0   ,    2, "Shot"		},
	{0x11, 0x01, 0x20, 0x00, "2"		},
	{0x11, 0x01, 0x20, 0x20, "3"		},

	{0   , 0xfe, 0   ,    2, "Cabinet"		},
	{0x11, 0x01, 0x40, 0x00, "Upright"		},
	{0x11, 0x01, 0x40, 0x40, "Cocktail"		},

	{0   , 0xfe, 0   ,    2, "Flip Screen"		},
	{0x11, 0x01, 0x80, 0x00, "Off"		},
	{0x11, 0x01, 0x80, 0x80, "On"		},

	{0   , 0xfe, 0   ,   16, "Coin A"		},
	{0x12, 0x01, 0x0f, 0x00, "5 Coins 1 Credits"		},
	{0x12, 0x01, 0x0f, 0x0c, "4 Coins 1 Credits"		},
	{0x12, 0x01, 0x0f, 0x0e, "3 Coins 1 Credits"		},
	{0x12, 0x01, 0x0f, 0x05, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0x0f, 0x06, "3 Coins 2 Credits"		},
	{0x12, 0x01, 0x0f, 0x04, "4 Coins 3 Credits"		},
	{0x12, 0x01, 0x0f, 0x0f, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0x0f, 0x08, "4 Coins 5 Credits"		},
	{0x12, 0x01, 0x0f, 0x0a, "3 Coins 4 Credits"		},
	{0x12, 0x01, 0x0f, 0x09, "2 Coins 3 Credits"		},
	{0x12, 0x01, 0x0f, 0x02, "3 Coins/5 Credits"		},
	{0x12, 0x01, 0x0f, 0x07, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0x0f, 0x01, "2 Coins 5 Credits"		},
	{0x12, 0x01, 0x0f, 0x0b, "1 Coin  3 Credits"		},
	{0x12, 0x01, 0x0f, 0x03, "1 Coin  4 Credits"		},
	{0x12, 0x01, 0x0f, 0x0d, "1 Coin  5 Credits"		},

	{0   , 0xfe, 0   ,    16, "Coin B"		},
	{0x12, 0x01, 0xf0, 0x00, "5 Coins 1 Credits"		},
	{0x12, 0x01, 0xf0, 0xc0, "4 Coins 1 Credits"		},
	{0x12, 0x01, 0xf0, 0xe0, "3 Coins 1 Credits"		},
	{0x12, 0x01, 0xf0, 0x50, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0xf0, 0x60, "3 Coins 2 Credits"		},
	{0x12, 0x01, 0xf0, 0x40, "4 Coins 3 Credits"		},
	{0x12, 0x01, 0xf0, 0xf0, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0xf0, 0x80, "4 Coins 5 Credits"		},
	{0x12, 0x01, 0xf0, 0xa0, "3 Coins 4 Credits"		},
	{0x12, 0x01, 0xf0, 0x90, "2 Coins 3 Credits"		},
	{0x12, 0x01, 0xf0, 0x20, "3 Coins/5 Credits"		},
	{0x12, 0x01, 0xf0, 0x70, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0xf0, 0x10, "2 Coins 5 Credits"		},
	{0x12, 0x01, 0xf0, 0xb0, "1 Coin  3 Credits"		},
	{0x12, 0x01, 0xf0, 0x30, "1 Coin  4 Credits"		},
	{0x12, 0x01, 0xf0, 0xd0, "1 Coin  5 Credits"		},
};

STDDIPINFO(Pbillrd)

static struct BurnInputInfo GigasInputList[] = {

	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 7,	"p1 coin"},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 start"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 left"},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 right"},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 fire 1"},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 fire 2"},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy2 + 7,	"p2 coin"},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy2 + 6,	"p2 start"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 left"},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 right"},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 fire 1"},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 fire 2"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDip + 0,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDip + 1,	"dip"},
};

STDINPUTINFO(Gigas)

static struct BurnDIPInfo GigasDIPList[]=
{
	{0x0d, 0xff, 0xff, 0x3f, NULL		},
	{0x0e, 0xff, 0xff, 0xff, NULL		},

	{0   , 0xfe, 0   ,    2, "Lives"		},
	{0x0d, 0x01, 0x01, 0x01, "3"		},
	{0x0d, 0x01, 0x01, 0x00, "5"		},

	{0   , 0xfe, 0   ,    4, "Bonus Life"		},
	{0x0d, 0x01, 0x06, 0x06, "20000 & 60000, Every 60000 Points"		},
	{0x0d, 0x01, 0x06, 0x02, "20000 & 60000 Points"		},
	{0x0d, 0x01, 0x06, 0x04, "30000 & 80000, Every 80000 Points"		},
	{0x0d, 0x01, 0x06, 0x00, "Only 20000 Points"		},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x0d, 0x01, 0x18, 0x18, "Easy"		},
	{0x0d, 0x01, 0x18, 0x10, "Normal"		},
	{0x0d, 0x01, 0x18, 0x08, "Hard"		},
	{0x0d, 0x01, 0x18, 0x00, "Hardest"		},

	{0   , 0xfe, 0   ,    2, "Allow Continue"		},
	{0x0d, 0x01, 0x20, 0x00, "No"		},
	{0x0d, 0x01, 0x20, 0x20, "Yes"		},

	{0   , 0xfe, 0   ,    2, "Cabinet"		},
	{0x0d, 0x01, 0x40, 0x00, "Upright"		},
	{0x0d, 0x01, 0x40, 0x40, "Cocktail"		},

	{0   , 0xfe, 0   ,    2, "Flip Screen"		},
	{0x0d, 0x01, 0x80, 0x00, "Off"		},
	{0x0d, 0x01, 0x80, 0x80, "On"		},

	{0   , 0xfe, 0   ,   16, "Coin A"		},
	{0x0e, 0x01, 0x0f, 0x00, "5 Coins 1 Credits"		},
	{0x0e, 0x01, 0x0f, 0x0c, "4 Coins 1 Credits"		},
	{0x0e, 0x01, 0x0f, 0x0e, "3 Coins 1 Credits"		},
	{0x0e, 0x01, 0x0f, 0x05, "2 Coins 1 Credits"		},
	{0x0e, 0x01, 0x0f, 0x06, "3 Coins 2 Credits"		},
	{0x0e, 0x01, 0x0f, 0x04, "4 Coins 3 Credits"		},
	{0x0e, 0x01, 0x0f, 0x0f, "1 Coin  1 Credits"		},
	{0x0e, 0x01, 0x0f, 0x08, "4 Coins 5 Credits"		},
	{0x0e, 0x01, 0x0f, 0x0a, "3 Coins 4 Credits"		},
	{0x0e, 0x01, 0x0f, 0x09, "2 Coins 3 Credits"		},
	{0x0e, 0x01, 0x0f, 0x02, "3 Coins/5 Credits"		},
	{0x0e, 0x01, 0x0f, 0x07, "1 Coin  2 Credits"		},
	{0x0e, 0x01, 0x0f, 0x01, "2 Coins 5 Credits"		},
	{0x0e, 0x01, 0x0f, 0x0b, "1 Coin  3 Credits"		},
	{0x0e, 0x01, 0x0f, 0x03, "1 Coin  4 Credits"		},
	{0x0e, 0x01, 0x0f, 0x0d, "1 Coin  5 Credits"		},

	{0   , 0xfe, 0   ,    16, "Coin B"		},
	{0x0e, 0x01, 0xf0, 0x00, "5 Coins 1 Credits"		},
	{0x0e, 0x01, 0xf0, 0xc0, "4 Coins 1 Credits"		},
	{0x0e, 0x01, 0xf0, 0xe0, "3 Coins 1 Credits"		},
	{0x0e, 0x01, 0xf0, 0x50, "2 Coins 1 Credits"		},
	{0x0e, 0x01, 0xf0, 0x60, "3 Coins 2 Credits"		},
	{0x0e, 0x01, 0xf0, 0x40, "4 Coins 3 Credits"		},
	{0x0e, 0x01, 0xf0, 0xf0, "1 Coin  1 Credits"		},
	{0x0e, 0x01, 0xf0, 0x80, "4 Coins 5 Credits"		},
	{0x0e, 0x01, 0xf0, 0xa0, "3 Coins 4 Credits"		},
	{0x0e, 0x01, 0xf0, 0x90, "2 Coins 3 Credits"		},
	{0x0e, 0x01, 0xf0, 0x20, "3 Coins/5 Credits"		},
	{0x0e, 0x01, 0xf0, 0x70, "1 Coin  2 Credits"		},
	{0x0e, 0x01, 0xf0, 0x10, "2 Coins 5 Credits"		},
	{0x0e, 0x01, 0xf0, 0xb0, "1 Coin  3 Credits"		},
	{0x0e, 0x01, 0xf0, 0x30, "1 Coin  4 Credits"		},
	{0x0e, 0x01, 0xf0, 0xd0, "1 Coin  5 Credits"		},
};

STDDIPINFO(Gigas)

static struct BurnDIPInfo Gigasm2DIPList[]=
{
	{0x0d, 0xff, 0xff, 0x3f, NULL		},
	{0x0e, 0xff, 0xff, 0xff, NULL		},

	{0   , 0xfe, 0   ,    2, "Lives"		},
	{0x0d, 0x01, 0x01, 0x01, "3"		},
	{0x0d, 0x01, 0x01, 0x00, "5"		},

	{0   , 0xfe, 0   ,    4, "Bonus Life"		},
	{0x0d, 0x01, 0x06, 0x06, "20000 & 60000, Every 60000 Points"		},
	{0x0d, 0x01, 0x06, 0x02, "20000 & 60000 Points"		},
	{0x0d, 0x01, 0x06, 0x04, "30000 & 80000, Every 80000 Points"		},
	{0x0d, 0x01, 0x06, 0x00, "Only 20000 Points"		},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x0d, 0x01, 0x18, 0x18, "Easy"		},
	{0x0d, 0x01, 0x18, 0x10, "Normal"		},
	{0x0d, 0x01, 0x18, 0x08, "Hard"		},
	{0x0d, 0x01, 0x18, 0x00, "Hardest"		},

	{0   , 0xfe, 0   ,    2, "Allow Continue"		},
	{0x0d, 0x01, 0x20, 0x00, "No"		},
	{0x0d, 0x01, 0x20, 0x20, "Yes"		},

	{0   , 0xfe, 0   ,    2, "Cabinet"		},
	{0x0d, 0x01, 0x40, 0x00, "Upright"		},
	{0x0d, 0x01, 0x40, 0x40, "Cocktail"		},

	{0   , 0xfe, 0   ,    2, "Flip Screen"		},
	{0x0d, 0x01, 0x80, 0x00, "Off"		},
	{0x0d, 0x01, 0x80, 0x80, "On"		},

	{0   , 0xfe, 0   ,   16, "Coin A"		},
	{0x0e, 0x01, 0x0f, 0x00, "5 Coins 1 Credits"		},
	{0x0e, 0x01, 0x0f, 0x0c, "4 Coins 1 Credits"		},
	{0x0e, 0x01, 0x0f, 0x0e, "3 Coins 1 Credits"		},
	{0x0e, 0x01, 0x0f, 0x05, "2 Coins 1 Credits"		},
	{0x0e, 0x01, 0x0f, 0x06, "3 Coins 2 Credits"		},
	{0x0e, 0x01, 0x0f, 0x04, "4 Coins 3 Credits"		},
	{0x0e, 0x01, 0x0f, 0x0f, "1 Coin  1 Credits"		},
	{0x0e, 0x01, 0x0f, 0x08, "4 Coins 5 Credits"		},
	{0x0e, 0x01, 0x0f, 0x0a, "3 Coins 4 Credits"		},
	{0x0e, 0x01, 0x0f, 0x09, "2 Coins 3 Credits"		},
	{0x0e, 0x01, 0x0f, 0x02, "3 Coins/5 Credits"		},
	{0x0e, 0x01, 0x0f, 0x07, "1 Coin  2 Credits"		},
	{0x0e, 0x01, 0x0f, 0x01, "2 Coins 5 Credits"		},
	{0x0e, 0x01, 0x0f, 0x0b, "1 Coin  3 Credits"		},
	{0x0e, 0x01, 0x0f, 0x03, "1 Coin  4 Credits"		},
	{0x0e, 0x01, 0x0f, 0x0d, "1 Coin  5 Credits"		},

	{0   , 0xfe, 0   ,    16, "Coin B"		},
	{0x0e, 0x01, 0xf0, 0x00, "5 Coins 1 Credits"		},
	{0x0e, 0x01, 0xf0, 0xc0, "4 Coins 1 Credits"		},
	{0x0e, 0x01, 0xf0, 0xe0, "3 Coins 1 Credits"		},
	{0x0e, 0x01, 0xf0, 0x50, "2 Coins 1 Credits"		},
	{0x0e, 0x01, 0xf0, 0x60, "3 Coins 2 Credits"		},
	{0x0e, 0x01, 0xf0, 0x40, "4 Coins 3 Credits"		},
	{0x0e, 0x01, 0xf0, 0xf0, "1 Coin  1 Credits"		},
	{0x0e, 0x01, 0xf0, 0x80, "4 Coins 5 Credits"		},
	{0x0e, 0x01, 0xf0, 0xa0, "3 Coins 4 Credits"		},
	{0x0e, 0x01, 0xf0, 0x90, "2 Coins 3 Credits"		},
	{0x0e, 0x01, 0xf0, 0x20, "3 Coins/5 Credits"		},
	{0x0e, 0x01, 0xf0, 0x70, "1 Coin  2 Credits"		},
	{0x0e, 0x01, 0xf0, 0x10, "2 Coins 5 Credits"		},
	{0x0e, 0x01, 0xf0, 0xb0, "1 Coin  3 Credits"		},
	{0x0e, 0x01, 0xf0, 0x30, "1 Coin  4 Credits"		},
	{0x0e, 0x01, 0xf0, 0xd0, "1 Coin  5 Credits"		},
};

STDDIPINFO(Gigasm2)

static struct BurnInputInfo FreekckInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 7,	"p1 coin"},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 start"},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 5,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 left"},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 right"},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 fire 1"},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 fire 2"},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy2 + 7,	"p2 coin"},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy2 + 6,	"p2 start"},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 5,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 4,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 left"},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 right"},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 fire 1"},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 fire 2"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDip + 0,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDip + 1,	"dip"},
	{"Dip C",		BIT_DIPSWITCH,	DrvDip + 2,	"dip"},
};

STDINPUTINFO(Freekck)


static struct BurnDIPInfo FreekckDIPList[]=
{
	{0x11, 0xff, 0xff, 0xbf, NULL		},
	{0x12, 0xff, 0xff, 0xff, NULL		},
	{0x13, 0xff, 0xff, 0xfe, NULL		},

	{0   , 0xfe, 0   ,    2, "Lives"		},
	{0x11, 0x01, 0x01, 0x01, "3"		},
	{0x11, 0x01, 0x01, 0x00, "5"		},

	{0   , 0xfe, 0   ,    4, "Bonus Life"		},
	{0x11, 0x01, 0x06, 0x06, "2-3-4-5-60000 Points"		},
	{0x11, 0x01, 0x06, 0x02, "3-4-5-6-7-80000 Points"		},
	{0x11, 0x01, 0x06, 0x04, "20000 & 60000 Points"		},
	{0x11, 0x01, 0x06, 0x00, "ONLY 20000 Points"		},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x11, 0x01, 0x18, 0x18, "Easy"		},
	{0x11, 0x01, 0x18, 0x10, "Normal"		},
	{0x11, 0x01, 0x18, 0x08, "Hard"		},
	{0x11, 0x01, 0x18, 0x00, "Hardest"		},

	{0   , 0xfe, 0   ,    2, "Allow Continue"		},
	{0x11, 0x01, 0x20, 0x00, "No"		},
	{0x11, 0x01, 0x20, 0x20, "Yes"		},

	{0   , 0xfe, 0   ,    2, "Cabinet"		},
	{0x11, 0x01, 0x40, 0x00, "Upright"		},
	{0x11, 0x01, 0x40, 0x40, "Cocktail"		},

	{0   , 0xfe, 0   ,    2, "Flip Screen"		},
	{0x11, 0x01, 0x80, 0x80, "Off"		},
	{0x11, 0x01, 0x80, 0x00, "On"		},

	{0   , 0xfe, 0   ,    16, "Coin A"		},
	{0x12, 0x01, 0x0f, 0x00, "5 Coins 1 Credits"		},
	{0x12, 0x01, 0x0f, 0x0c, "4 Coins 1 Credits"		},
	{0x12, 0x01, 0x0f, 0x0e, "3 Coins 1 Credits"		},
	{0x12, 0x01, 0x0f, 0x05, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0x0f, 0x06, "3 Coins 2 Credits"		},
	{0x12, 0x01, 0x0f, 0x04, "4 Coins 3 Credits"		},
	{0x12, 0x01, 0x0f, 0x0f, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0x0f, 0x08, "4 Coins 5 Credits"		},
	{0x12, 0x01, 0x0f, 0x0a, "3 Coins 4 Credits"		},
	{0x12, 0x01, 0x0f, 0x09, "2 Coins 3 Credits"		},
	{0x12, 0x01, 0x0f, 0x02, "3 Coins/5 Credits"		},
	{0x12, 0x01, 0x0f, 0x07, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0x0f, 0x01, "2 Coins 5 Credits"		},
	{0x12, 0x01, 0x0f, 0x0b, "1 Coin  3 Credits"		},
	{0x12, 0x01, 0x0f, 0x03, "1 Coin  4 Credits"		},
	{0x12, 0x01, 0x0f, 0x0d, "1 Coin  5 Credits"		},

	{0   , 0xfe, 0   ,    16, "Coin B"		},
	{0x12, 0x01, 0xf0, 0x00, "5 Coins 1 Credits"		},
	{0x12, 0x01, 0xf0, 0xe0, "3 Coins 1 Credits"		},
	{0x12, 0x01, 0xf0, 0x50, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0xf0, 0x60, "3 Coins 2 Credits"		},
	{0x12, 0x01, 0xf0, 0xf0, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0xf0, 0xa0, "3 Coins 4 Credits"		},
	{0x12, 0x01, 0xf0, 0x90, "2 Coins 3 Credits"		},
	{0x12, 0x01, 0xf0, 0x20, "3 Coins/5 Credits"		},
	{0x12, 0x01, 0xf0, 0x70, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0xf0, 0x10, "2 Coins 5 Credits"		},
	{0x12, 0x01, 0xf0, 0xb0, "1 Coin  3 Credits"		},
	{0x12, 0x01, 0xf0, 0x30, "1 Coin  4 Credits"		},
	{0x12, 0x01, 0xf0, 0xd0, "1 Coin  5 Credits"		},
	{0x12, 0x01, 0xf0, 0xc0, "1 Coin/10 Credits"		},
	{0x12, 0x01, 0xf0, 0x40, "1 Coin/25 Credits"		},
	{0x12, 0x01, 0xf0, 0x80, "1 Coin/50 Credits"		},

	{0   , 0xfe, 0   ,    2, "Manufacturer"		},
	{0x13, 0x01, 0x01, 0x00, "Nihon System"		},
	{0x13, 0x01, 0x01, 0x01, "Sega/Nihon System"		},

	{0   , 0xfe, 0   ,    0, "Coin Slots"		},
	{0x13, 0x01, 0x80, 0x00, "1"		},
	{0x13, 0x01, 0x80, 0x80, "2"		},
};

STDDIPINFO(Freekck)

static struct BurnInputInfo CountrunInputList[] = {

	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 7,	"p1 coin"},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 start"},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 5,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 left"},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 right"},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 fire 1"},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 fire 2"},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy2 + 7,	"p2 coin"},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy2 + 6,	"p2 start"},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 5,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 4,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 left"},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 right"},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 fire 1"},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 fire 2"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDip + 0,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDip + 1,	"dip"},
	{"Dip C",		BIT_DIPSWITCH,	DrvDip + 2,	"dip"},
};

STDINPUTINFO(Countrun)


static struct BurnDIPInfo CountrunDIPList[]=
{
	{0x11, 0xff, 0xff, 0xbf, NULL		},
	{0x12, 0xff, 0xff, 0xff, NULL		},
	{0x13, 0xff, 0xff, 0xfe, NULL		},

	{0   , 0xfe, 0   ,    2, "Lives"		},
	{0x11, 0x01, 0x01, 0x01, "3"		},
	{0x11, 0x01, 0x01, 0x00, "2"		},

	{0   , 0xfe, 0   ,    4, "Bonus Life"		},
	{0x11, 0x01, 0x06, 0x06, "20000, 60000 and every 60000 Points"		},
	{0x11, 0x01, 0x06, 0x02, "30000, 80000 and every 80000 Points"		},
	{0x11, 0x01, 0x06, 0x04, "20000 & 60000 Points"		},
	{0x11, 0x01, 0x06, 0x00, "ONLY 20000 Points"		},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x11, 0x01, 0x18, 0x18, "Easy"		},
	{0x11, 0x01, 0x18, 0x10, "Normal"		},
	{0x11, 0x01, 0x18, 0x08, "Hard"		},
	{0x11, 0x01, 0x18, 0x00, "Hardest"		},

	{0   , 0xfe, 0   ,    2, "Allow Continue"		},
	{0x11, 0x01, 0x20, 0x00, "No"		},
	{0x11, 0x01, 0x20, 0x20, "Yes"		},

	{0   , 0xfe, 0   ,    2, "Cabinet"		},
	{0x11, 0x01, 0x40, 0x00, "Upright"		},
	{0x11, 0x01, 0x40, 0x40, "Cocktail"		},

	{0   , 0xfe, 0   ,    2, "Flip Screen"		},
	{0x11, 0x01, 0x80, 0x80, "Off"		},
	{0x11, 0x01, 0x80, 0x00, "On"		},

	{0   , 0xfe, 0   ,    16, "Coin A"		},
	{0x12, 0x01, 0x0f, 0x00, "5 Coins 1 Credits"		},
	{0x12, 0x01, 0x0f, 0x0c, "4 Coins 1 Credits"		},
	{0x12, 0x01, 0x0f, 0x0e, "3 Coins 1 Credits"		},
	{0x12, 0x01, 0x0f, 0x05, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0x0f, 0x06, "3 Coins 2 Credits"		},
	{0x12, 0x01, 0x0f, 0x04, "4 Coins 3 Credits"		},
	{0x12, 0x01, 0x0f, 0x0f, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0x0f, 0x08, "4 Coins 5 Credits"		},
	{0x12, 0x01, 0x0f, 0x0a, "3 Coins 4 Credits"		},
	{0x12, 0x01, 0x0f, 0x09, "2 Coins 3 Credits"		},
	{0x12, 0x01, 0x0f, 0x02, "3 Coins/5 Credits"		},
	{0x12, 0x01, 0x0f, 0x07, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0x0f, 0x01, "2 Coins 5 Credits"		},
	{0x12, 0x01, 0x0f, 0x0b, "1 Coin  3 Credits"		},
	{0x12, 0x01, 0x0f, 0x03, "1 Coin  4 Credits"		},
	{0x12, 0x01, 0x0f, 0x0d, "1 Coin  5 Credits"		},

	{0   , 0xfe, 0   ,    16, "Coin B"		},
	{0x12, 0x01, 0xf0, 0x00, "5 Coins 1 Credits"		},
	{0x12, 0x01, 0xf0, 0xe0, "3 Coins 1 Credits"		},
	{0x12, 0x01, 0xf0, 0x50, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0xf0, 0x60, "3 Coins 2 Credits"		},
	{0x12, 0x01, 0xf0, 0xf0, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0xf0, 0xa0, "3 Coins 4 Credits"		},
	{0x12, 0x01, 0xf0, 0x90, "2 Coins 3 Credits"		},
	{0x12, 0x01, 0xf0, 0x20, "3 Coins/5 Credits"		},
	{0x12, 0x01, 0xf0, 0x70, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0xf0, 0x10, "2 Coins 5 Credits"		},
	{0x12, 0x01, 0xf0, 0xb0, "1 Coin  3 Credits"		},
	{0x12, 0x01, 0xf0, 0x30, "1 Coin  4 Credits"		},
	{0x12, 0x01, 0xf0, 0xd0, "1 Coin  5 Credits"		},
	{0x12, 0x01, 0xf0, 0xc0, "1 Coin/10 Credits"		},
	{0x12, 0x01, 0xf0, 0x40, "1 Coin/25 Credits"		},
	{0x12, 0x01, 0xf0, 0x80, "1 Coin/50 Credits"		},

	{0   , 0xfe, 0   ,    2, "Manufacturer"		},
	{0x13, 0x01, 0x01, 0x00, "Nihon System"		},
	{0x13, 0x01, 0x01, 0x01, "Sega/Nihon System"		},

	{0   , 0xfe, 0   ,    0, "Coin Slots"		},
	{0x13, 0x01, 0x80, 0x00, "1"		},
	{0x13, 0x01, 0x80, 0x80, "2"		},

};

STDDIPINFO(Countrun)

// Perfect Billiard

static struct BurnRomInfo pbillrdRomDesc[] = {
	{ "pb.18",	0x4000, 0x9e6275ac, 1 }, //  0 maincpu
	{ "pb.7",	0x8000, 0xdd438431, 1 }, //  1
	{ "pb.9",	0x4000, 0x089ce80a, 1 }, //  2

	{ "pb.4",	0x4000, 0x2f4d4dd3, 2 }, //  3 gfx1
	{ "pb.5",	0x4000, 0x9dfccbd3, 2 }, //  4
	{ "pb.6",	0x4000, 0xb5c3f6f6, 2 }, //  5

	{ "10619.3r",	0x2000, 0x3296b9d9, 3 }, //  6 gfx2
	{ "10621.3m",	0x2000, 0x3dca8e4b, 3 }, //  7
	{ "10620.3n",	0x2000, 0xee76b079, 3 }, //  8

	{ "82s129.3a",	0x0100, 0x44802169, 4 }, //  9 proms
	{ "82s129.4d",	0x0100, 0x69ca07cc, 4 }, // 10
	{ "82s129.4a",	0x0100, 0x145f950a, 4 }, // 11
	{ "82s129.3d",	0x0100, 0x43d24e17, 4 }, // 12
	{ "82s129.3b",	0x0100, 0x7fdc872c, 4 }, // 13
	{ "82s129.3c",	0x0100, 0xcc1657e5, 4 }, // 14
};

STD_ROM_PICK(pbillrd)
STD_ROM_FN(pbillrd)

// Free Kick (bootleg set 1)

static struct BurnRomInfo freekickb1RomDesc[] = {
	{ "freekbl8.q7",	0x10000, 0x4208cfe5, 1 | BRF_PRG | BRF_ESS }, //  0 maincpu

	{ "11.1e",		0x08000, 0xa6030ba9, 2 | BRF_GRA },           //  1 user1

	{ "12.1h",		0x04000, 0xfb82e486, 3 | BRF_GRA },           //  2 gfx1
	{ "13.1j",		0x04000, 0x3ad78ee2, 3 | BRF_GRA },           //  3
	{ "14.1l",		0x04000, 0x0185695f, 3 | BRF_GRA },           //  4

	{ "15.1m",		0x04000, 0x0fa7c13c, 4 | BRF_GRA },           //  5 gfx2
	{ "16.1p",		0x04000, 0x2b996e89, 4 | BRF_GRA },           //  6
	{ "17.1r",		0x04000, 0xe7894def, 4 | BRF_GRA },           //  7

	{ "24s10n.8j",		0x00100, 0x53a6bc21, 5 | BRF_GRA },           //  8 proms
	{ "24s10n.7j",		0x00100, 0x38dd97d8, 5 | BRF_GRA },           //  9
	{ "24s10n.8k",		0x00100, 0x18e66087, 5 | BRF_GRA },           // 10
	{ "24s10n.7k",		0x00100, 0xbc21797a, 5 | BRF_GRA },           // 11
	{ "24s10n.8h",		0x00100, 0x8aac5fd0, 5 | BRF_GRA },           // 12
	{ "24s10n.7h",		0x00100, 0xa507f941, 5 | BRF_GRA },           // 13

//	{ "pal16l8.q10.bin",	0x00001, 0x00000000, 6 | BRF_NODUMP | BRF_GRA },           // 14 pals
//	{ "pal16l8.r1.bin",	0x00001, 0x00000000, 6 | BRF_NODUMP | BRF_GRA },           // 15
//	{ "pal16l8.s1.bin",	0x00001, 0x00000000, 6 | BRF_NODUMP | BRF_GRA },           // 16
};

STD_ROM_PICK(freekickb1)
STD_ROM_FN(freekickb1)

// Counter Run (bootleg set 1)

static struct BurnRomInfo countrunbRomDesc[] = {
	{ "rom_cpu.bin",	0x10000, 0xf65639ae, 1 }, //  0 maincpu

	{ "crun.e1",		0x08000, 0x2c3b6f8f, 2 }, //  1 user1

	{ "crun.h1",		0x04000, 0x3385b7b5, 3 }, //  2 gfx1
	{ "crun.j1",			0x04000, 0x58dc148d, 3 }, //  3
	{ "crun.l1",			0x04000, 0x3201f1e9, 3 }, //  4

	{ "crun.m1",		0x04000, 0x1efab3b4, 4 }, //  5 gfx2
	{ "crun.p1",		0x04000, 0xd0bf8d42, 4 }, //  6
	{ "crun.r1",			0x04000, 0x4bb4a3e3, 4 }, //  7

	{ "24s10n.8j",			0x00100, 0x63c114ad, 5 }, //  8 proms
	{ "24s10n.7j",			0x00100, 0xd16f95cc, 5 }, //  9
	{ "24s10n.8k",		0x00100, 0x217db2c1, 5 }, // 10
	{ "24s10n.7k",		0x00100, 0x8d983949, 5 }, // 11
	{ "24s10n.8h",		0x00100, 0x33e87550, 5 }, // 12
	{ "24s10n.7h",		0x00100, 0xc77d0077, 5 }, // 13
};

STD_ROM_PICK(countrunb)
STD_ROM_FN(countrunb)

// Gigas (bootleg)

static struct BurnRomInfo gigasbRomDesc[] = {
	{ "g7.bin",	0x08000, 0xdaf4e88d, 1 }, //  0 maincpu
	{ "g8.bin",	0x10000, 0x4ab4c1f1, 1 }, //  1

	{ "g4.bin",	0x04000, 0x8ed78981, 2 }, //  2 gfx1
	{ "g5.bin",	0x04000, 0x0645ec2d, 2 }, //  3
	{ "g6.bin",	0x04000, 0x99e9cb27, 2 }, //  4

	{ "g1.bin",	0x04000, 0xd78fae6e, 3 }, //  5 gfx2
	{ "g3.bin",	0x04000, 0x37df4a4c, 3 }, //  6
	{ "g2.bin",	0x04000, 0x3a46e354, 3 }, //  7

	{ "1.pr",	0x00100, 0xa784e71f, 4 }, //  8 proms
	{ "6.pr",	0x00100, 0x376df30c, 4 }, //  9
	{ "5.pr",	0x00100, 0x4edff5bd, 4 }, // 10
	{ "4.pr",	0x00100, 0xfe201a4e, 4 }, // 11
	{ "2.pr",	0x00100, 0x5796cc4a, 4 }, // 12
	{ "3.pr",	0x00100, 0x28b5ee4c, 4 }, // 13
};

STD_ROM_PICK(gigasb)
STD_ROM_FN(gigasb)

// Gigas Mark II (bootleg)

static struct BurnRomInfo gigasm2RomDesc[] = {
	{ "8.rom",	0x08000, 0xc00a4a6c, 1 }, //  0 maincpu
	{ "7.rom",	0x08000, 0x92bd9045, 1 }, //  1
	{ "9.rom",	0x08000, 0xa3ef809c, 1 }, //  1

	{ "4.rom",	0x04000, 0x20b3405f, 2 }, //  2 gfx1
	{ "5.rom",	0x04000, 0xd04ecfa8, 2 }, //  3
	{ "6.rom",	0x04000, 0x33776801, 2 }, //  4

	{ "1.rom",	0x04000, 0xf64cbd1e, 3 }, //  5 gfx2
	{ "3.rom",	0x04000, 0xc228df19, 3 }, //  6
	{ "2.rom",	0x04000, 0xa6ad9ce2, 3 }, //  7

	{ "1.pr",	0x00100, 0xa784e71f, 4 }, //  8 proms
	{ "6.pr",	0x00100, 0x376df30c, 4 }, //  9
	{ "5.pr",	0x00100, 0x4edff5bd, 4 }, // 10
	{ "4.pr",	0x00100, 0xfe201a4e, 4 }, // 11
	{ "2.pr",	0x00100, 0x5796cc4a, 4 }, // 12
	{ "3.pr",	0x00100, 0x28b5ee4c, 4 }, // 13
};

STD_ROM_PICK(gigasm2)
STD_ROM_FN(gigasm2)

#endif