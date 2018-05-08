#ifndef _D_SIDARM_H_
#define _D_SIDARM_H_

#include "burnint.h"
#include "saturn/ovl.h"

INT32 SidearmsInit();
INT32 DrvExit();
INT32 DrvFrame();
INT32 SidearmsDraw();
static char *itoa(i);
static void tile32x32toSaturn (unsigned char reverse, unsigned int num, unsigned char *pDest);

/*static*/ UINT16 *bgmap_lut = NULL;
/*static*/ //UINT16 *remap16_lut = NULL;
/*static*/ UINT16 *remap4to16_lut = NULL;
/*static*/ UINT16 *map_lut = NULL;  //[256];
/*static*/ UINT16 *map_offset_lut = NULL;  //[2048];
/*static*/ //UINT16 *charaddr_lut = NULL; //[0x0800];
/*static*/ UINT16 cram_lut[4096];

/*static*/ UINT8 *AllMem;
/*static*/ UINT8 *MemEnd;
/*static*/ UINT8 *AllRam;
/*static*/ UINT8 *RamEnd;
/*static*/ UINT8 *DrvZ80ROM0;
/*static*/ UINT8 *DrvZ80ROM1;
/*static*/ UINT8 *DrvGfxROM0;
/*static*/ UINT8 *DrvGfxROM1;
/*static*/ UINT8 *DrvGfxROM2;
/*static*/ UINT8 *DrvStarMap;
/*static*/ UINT8 *DrvTileMap;
/*static*/ UINT8 *DrvVidRAM;
/*static*/ UINT8 *DrvSprBuf;
/*static*/ UINT8 *DrvSprRAM;
/*static*/ UINT8 *DrvPalRAM;
/*static*/ UINT8 *DrvZ80RAM0;
/*static*/ UINT8 *DrvZ80RAM1;

/*static*/ UINT16 *DrvPalette;
/*static*/ UINT8 DrvRecalc;

/*static*/ UINT8 *bgscrollx;
/*static*/ UINT8 *bgscrolly;

/*static*/ UINT8 flipscreen;
/*static*/ UINT8 soundlatch;
/*static*/ UINT8 starfield_enable;
/*static*/ UINT8 character_enable;
/*static*/ UINT8 sprite_enable;
/*static*/ UINT8 bglayer_enable;
/*static*/ UINT8 bank_data;

/*static*/ UINT16 starscrollx;
/*static*/ UINT16 starscrolly;

/*static*/ INT32 hflop_74a;

/*static*/ INT32 enable_watchdog;
/*static*/ INT32 watchdog;
/*static*/ INT32 vblank;
/*static*/ INT32 is_whizz = 0; // uses ym2151 instead of ym2203
/*static*/ INT32 is_turtshipk = 0;

/*static*/ UINT8 DrvJoy1[8];
/*static*/ UINT8 DrvJoy2[8];
/*static*/ UINT8 DrvJoy3[8];
/*static*/ UINT8 DrvJoy4[8];
/*static*/ UINT8 DrvJoy5[8];
/*static*/ UINT8 DrvDips[4];
/*static*/ UINT8 DrvInputs[5];
/*static*/ UINT8 DrvReset;

/*static*/ struct BurnInputInfo SidearmsInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 coin"	},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 start"	},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy2 + 3,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy2 + 2,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy2 + 4,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy2 + 5,	"p1 fire 2"	},
	{"P1 Button 3",		BIT_DIGITAL,	DrvJoy2 + 6,	"p1 fire 3"	},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy1 + 7,	"p2 coin"	},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy1 + 1,	"p2 start"	},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy3 + 3,	"p2 up"		},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy3 + 2,	"p2 down"	},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy3 + 1,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy3 + 0,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy3 + 4,	"p2 fire 1"	},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy3 + 5,	"p2 fire 2"	},
	{"P2 Button 3",		BIT_DIGITAL,	DrvJoy3 + 6,	"p2 fire 3"	},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"		},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"		},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
	{"Dip C",		BIT_DIPSWITCH,	DrvDips + 2,	"dip"		},
};

STDINPUTINFO(Sidearms)

/*static*/ struct BurnInputInfo TurtshipInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 coin"	},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 start"	},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy2 + 3,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy2 + 2,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy2 + 4,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy2 + 5,	"p1 fire 2"	},
	{"P1 Button 3",		BIT_DIGITAL,	DrvJoy2 + 6,	"p1 fire 3"	},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy1 + 7,	"p2 coin"	},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy1 + 1,	"p2 start"	},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy3 + 3,	"p2 up"		},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy3 + 2,	"p2 down"	},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy3 + 1,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy3 + 0,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy3 + 4,	"p2 fire 1"	},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy3 + 5,	"p2 fire 2"	},
	{"P2 Button 3",		BIT_DIGITAL,	DrvJoy3 + 6,	"p2 fire 3"	},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"		},
	{"Service",		BIT_DIGITAL,	DrvJoy1 + 5,	"service"	},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"		},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
};

STDINPUTINFO(Turtship)

/*static*/ struct BurnInputInfo DygerInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 coin"	},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 start"	},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy2 + 3,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy2 + 2,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy2 + 4,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy2 + 5,	"p1 fire 2"	},
	{"P1 Button 3",		BIT_DIGITAL,	DrvJoy2 + 6,	"p1 fire 3"	},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"},
};

STDINPUTINFO(Dyger)

/*static*/ struct BurnInputInfo WhizzInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 coin"	},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 start"	},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy2 + 3,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy2 + 2,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy4 + 0,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy4 + 1,	"p1 fire 2"	},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy1 + 3,	"p2 coin"	},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy1 + 1,	"p2 start"	},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy3 + 3,	"p2 up"		},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy3 + 2,	"p2 down"	},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy3 + 1,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy3 + 0,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy5 + 0,	"p2 fire 1"	},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy5 + 1,	"p2 fire 2"	},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"		},
	{"Service",		BIT_DIGITAL,	DrvJoy2 + 4,	"service"	},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"		},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
	{"Dip C",		BIT_DIPSWITCH,	DrvDips + 2,	"dip"		},
	{"Dip D",		BIT_DIPSWITCH,	DrvDips + 3,	"dip"		},
};

STDINPUTINFO(Whizz)

/*static*/ struct BurnDIPInfo SidearmsDIPList[]=
{
	{0x13, 0xff, 0xff, 0xff, NULL			},
	{0x14, 0xff, 0xff, 0xfc, NULL			},
	{0x15, 0xff, 0xff, 0xff, NULL			},

	{0   , 0xfe, 0   ,    2, "Freeze"		},
	{0x13, 0x01, 0x08, 0x08, "Off"			},
	{0x13, 0x01, 0x08, 0x00, "On"			},

	{0   , 0xfe, 0   ,    0, "Difficulty"		},
	{0x14, 0x01, 0x07, 0x07, "0 (Easiest)"		},
	{0x14, 0x01, 0x07, 0x06, "1"			},
	{0x14, 0x01, 0x07, 0x05, "2"			},
	{0x14, 0x01, 0x07, 0x04, "3 (Normal)"		},
	{0x14, 0x01, 0x07, 0x03, "4"			},
	{0x14, 0x01, 0x07, 0x02, "5"			},
	{0x14, 0x01, 0x07, 0x01, "6"			},
	{0x14, 0x01, 0x07, 0x00, "7 (Hardest)"		},

	{0   , 0xfe, 0   ,    8, "Lives"		},
	{0x14, 0x01, 0x08, 0x08, "3"			},
	{0x14, 0x01, 0x08, 0x00, "5"			},

	{0   , 0xfe, 0   ,    2, "Bonus Life"		},
	{0x14, 0x01, 0x30, 0x30, "100000"		},
	{0x14, 0x01, 0x30, 0x20, "100000 100000"	},
	{0x14, 0x01, 0x30, 0x10, "150000 150000"	},
	{0x14, 0x01, 0x30, 0x00, "200000 200000"	},

	{0   , 0xfe, 0   ,    4, "Flip Screen"		},
	{0x14, 0x01, 0x40, 0x40, "Off"			},
	{0x14, 0x01, 0x40, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Service Mode"		},
	{0x14, 0x01, 0x80, 0x80, "Off"			},
	{0x14, 0x01, 0x80, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Coin A"		},
	{0x15, 0x01, 0x07, 0x00, "4 Coins 1 Credits"	},
	{0x15, 0x01, 0x07, 0x01, "3 Coins 1 Credits"	},
	{0x15, 0x01, 0x07, 0x02, "2 Coins 1 Credits"	},
	{0x15, 0x01, 0x07, 0x07, "1 Coin  1 Credits"	},
	{0x15, 0x01, 0x07, 0x06, "1 Coin  2 Credits"	},
	{0x15, 0x01, 0x07, 0x05, "1 Coin  3 Credits"	},
	{0x15, 0x01, 0x07, 0x04, "1 Coin  4 Credits"	},
	{0x15, 0x01, 0x07, 0x03, "1 Coin  6 Credits"	},

	{0   , 0xfe, 0   ,    8, "Coin B"		},
	{0x15, 0x01, 0x38, 0x00, "4 Coins 1 Credits"	},
	{0x15, 0x01, 0x38, 0x08, "3 Coins 1 Credits"	},
	{0x15, 0x01, 0x38, 0x10, "2 Coins 1 Credits"	},
	{0x15, 0x01, 0x38, 0x38, "1 Coin  1 Credits"	},
	{0x15, 0x01, 0x38, 0x30, "1 Coin  2 Credits"	},
	{0x15, 0x01, 0x38, 0x28, "1 Coin  3 Credits"	},
	{0x15, 0x01, 0x38, 0x20, "1 Coin  4 Credits"	},
	{0x15, 0x01, 0x38, 0x18, "1 Coin  6 Credits"	},

	{0   , 0xfe, 0   ,    8, "Allow Continue"	},
	{0x15, 0x01, 0x40, 0x00, "No"			},
	{0x15, 0x01, 0x40, 0x40, "Yes"			},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x15, 0x01, 0x80, 0x00, "Off"			},
	{0x15, 0x01, 0x80, 0x80, "On"			},
};

STDDIPINFO(Sidearms)

/*static*/ struct BurnDIPInfo TurtshipDIPList[]=
{
	{0x14, 0xff, 0xff, 0xbd, NULL			},
	{0x15, 0xff, 0xff, 0xeb, NULL			},

	{0   , 0xfe, 0   ,    2, "Invulnerability"	},
	{0x14, 0x01, 0x01, 0x01, "Off"			},
	{0x14, 0x01, 0x01, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Allow Continue"	},
	{0x14, 0x01, 0x02, 0x02, "No"			},
	{0x14, 0x01, 0x02, 0x00, "Yes"			},

	{0   , 0xfe, 0   ,    2, "Unknown"		},
	{0x14, 0x01, 0x04, 0x04, "Off"			},
	{0x14, 0x01, 0x04, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Flip Screen"		},
	{0x14, 0x01, 0x08, 0x08, "Off"			},
	{0x14, 0x01, 0x08, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Difficulty"		},
	{0x14, 0x01, 0x10, 0x10, "Normal"		},
	{0x14, 0x01, 0x10, 0x00, "Hard"			},

	{0   , 0xfe, 0   ,    8, "Lives"		},
	{0x14, 0x01, 0xe0, 0xe0, "1"			},
	{0x14, 0x01, 0xe0, 0x60, "2"			},
	{0x14, 0x01, 0xe0, 0xa0, "3"			},
	{0x14, 0x01, 0xe0, 0x20, "4"			},
	{0x14, 0x01, 0xe0, 0xc0, "5"			},
	{0x14, 0x01, 0xe0, 0x40, "6"			},
	{0x14, 0x01, 0xe0, 0x80, "7"			},
	{0x14, 0x01, 0xe0, 0x00, "8"			},

	{0   , 0xfe, 0   ,    2, "Service Mode"		},
	{0x15, 0x01, 0x01, 0x01, "Off"			},
	{0x15, 0x01, 0x01, 0x00, "On"			},

	{0   , 0xfe, 0   ,    0, "Bonus Life"		},
	{0x15, 0x01, 0x0c, 0x08, "Every 150000"		},
	{0x15, 0x01, 0x0c, 0x00, "Every 200000"		},
	{0x15, 0x01, 0x0c, 0x0c, "150000 only"		},
	{0x15, 0x01, 0x0c, 0x04, "200000 only"		},

	{0   , 0xfe, 0   ,    4, "Demo Sounds"		},
	{0x15, 0x01, 0x10, 0x10, "Off"			},
	{0x15, 0x01, 0x10, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Coinage"		},
	{0x15, 0x01, 0xe0, 0x00, "4 Coins 1 Credits"	},
	{0x15, 0x01, 0xe0, 0x80, "3 Coins 1 Credits"	},
	{0x15, 0x01, 0xe0, 0x40, "2 Coins 1 Credits"	},
	{0x15, 0x01, 0xe0, 0xe0, "1 Coin  1 Credits"	},
	{0x15, 0x01, 0xe0, 0x60, "1 Coin  2 Credits"	},
	{0x15, 0x01, 0xe0, 0xa0, "1 Coin  3 Credits"	},
	{0x15, 0x01, 0xe0, 0x20, "1 Coin  4 Credits"	},
};

STDDIPINFO(Turtship)

/*static*/ struct BurnDIPInfo DygerDIPList[]=
{
	{0x0a, 0xff, 0xff, 0xbd, NULL			},
	{0x0b, 0xff, 0xff, 0xf7, NULL			},

	{0   , 0xfe, 0   ,    0, "Allow Continue"	},
	{0x0a, 0x01, 0x02, 0x02, "No"			},
	{0x0a, 0x01, 0x02, 0x00, "Yes"			},

	{0   , 0xfe, 0   ,    2, "Flip Screen"		},
	{0x0a, 0x01, 0x08, 0x08, "Off"			},
	{0x0a, 0x01, 0x08, 0x00, "On"			},

	{0   , 0xfe, 0   ,    0, "Difficulty"		},
	{0x0a, 0x01, 0x10, 0x10, "Easy"			},
	{0x0a, 0x01, 0x10, 0x00, "Hard"			},

	{0   , 0xfe, 0   ,    2, "Lives"		},
	{0x0a, 0x01, 0xe0, 0xe0, "1"			},
	{0x0a, 0x01, 0xe0, 0x60, "2"			},
	{0x0a, 0x01, 0xe0, 0xa0, "3"			},
	{0x0a, 0x01, 0xe0, 0x20, "4"			},
	{0x0a, 0x01, 0xe0, 0xc0, "5"			},
	{0x0a, 0x01, 0xe0, 0x40, "6"			},
	{0x0a, 0x01, 0xe0, 0x80, "7"			},
	{0x0a, 0x01, 0xe0, 0x00, "8"			},

	{0   , 0xfe, 0   ,    2, "Service Mode"		},
	{0x0b, 0x01, 0x01, 0x01, "Off"			},
	{0x0b, 0x01, 0x01, 0x00, "On"			},

	{0   , 0xfe, 0   ,    8, "Bonus Life"		},
	{0x0b, 0x01, 0x0c, 0x04, "Every 150000"		},
	{0x0b, 0x01, 0x0c, 0x00, "Every 200000"		},
	{0x0b, 0x01, 0x0c, 0x0c, "150000 only"		},
	{0x0b, 0x01, 0x0c, 0x08, "200000 only"		},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x0b, 0x01, 0x10, 0x00, "Off"			},
	{0x0b, 0x01, 0x10, 0x10, "On"			},

	{0   , 0xfe, 0   ,    0, "Coinage"		},
	{0x0b, 0x01, 0xe0, 0x00, "4 Coins 1 Credits"	},
	{0x0b, 0x01, 0xe0, 0x80, "3 Coins 1 Credits"	},
	{0x0b, 0x01, 0xe0, 0x40, "2 Coins 1 Credits"	},
	{0x0b, 0x01, 0xe0, 0xe0, "1 Coin  1 Credits"	},
	{0x0b, 0x01, 0xe0, 0x60, "1 Coin  2 Credits"	},
	{0x0b, 0x01, 0xe0, 0xa0, "1 Coin  3 Credits"	},
	{0x0b, 0x01, 0xe0, 0x20, "1 Coin  4 Credits"	},
};

STDDIPINFO(Dyger)

/*static*/ struct BurnDIPInfo WhizzDIPList[]=
{
	{0x12, 0xff, 0xff, 0x10, NULL			},
	{0x13, 0xff, 0xff, 0xfc, NULL			},
	{0x14, 0xff, 0xff, 0xff, NULL			},
	{0x15, 0xff, 0xff, 0xff, NULL			},

	{0   , 0xfe, 0   ,    2, "Allow Continue"	},
	{0x12, 0x01, 0x10, 0x00, "No"			},
	{0x12, 0x01, 0x10, 0x10, "Yes"			},

	{0   , 0xfe, 0   ,    0, "Difficulty"		},
	{0x13, 0x01, 0x07, 0x07, "0 (Easiest)"		},
	{0x13, 0x01, 0x07, 0x06, "1"			},
	{0x13, 0x01, 0x07, 0x05, "2"			},
	{0x13, 0x01, 0x07, 0x04, "3 (Normal)"		},
	{0x13, 0x01, 0x07, 0x03, "4"			},
	{0x13, 0x01, 0x07, 0x02, "5"			},
	{0x13, 0x01, 0x07, 0x01, "6"			},
	{0x13, 0x01, 0x07, 0x00, "7 (Hardest)"		},

	{0   , 0xfe, 0   ,    8, "Flip Screen"		},
	{0x13, 0x01, 0x10, 0x10, "Off"			},
	{0x13, 0x01, 0x10, 0x00, "On"			},

	{0   , 0xfe, 0   ,    0, "Coin A"		},
	{0x14, 0x01, 0x07, 0x00, "4 Coins 1 Credits"	},
	{0x14, 0x01, 0x07, 0x01, "3 Coins 1 Credits"	},
	{0x14, 0x01, 0x07, 0x02, "2 Coins 1 Credits"	},
	{0x14, 0x01, 0x07, 0x07, "1 Coin  1 Credits"	},
	{0x14, 0x01, 0x07, 0x06, "1 Coin  2 Credits"	},
	{0x14, 0x01, 0x07, 0x05, "1 Coin  3 Credits"	},
	{0x14, 0x01, 0x07, 0x04, "1 Coin  4 Credits"	},
	{0x14, 0x01, 0x07, 0x03, "1 Coin  5 Credits"	},

	{0   , 0xfe, 0   ,    2, "Bonus Life"		},
	{0x14, 0x01, 0x18, 0x18, "100000 Only"		},
	{0x14, 0x01, 0x18, 0x10, "Every 100000"		},
	{0x14, 0x01, 0x18, 0x08, "Every 150000"		},
	{0x14, 0x01, 0x18, 0x00, "Every 200000"		},

	{0   , 0xfe, 0   ,    0, "Coin B"		},
	{0x15, 0x01, 0x07, 0x00, "4 Coins 1 Credits"	},
	{0x15, 0x01, 0x07, 0x01, "3 Coins 1 Credits"	},
	{0x15, 0x01, 0x07, 0x02, "2 Coins 1 Credits"	},
	{0x15, 0x01, 0x07, 0x07, "1 Coin  1 Credits"	},
	{0x15, 0x01, 0x07, 0x06, "1 Coin  2 Credits"	},
	{0x15, 0x01, 0x07, 0x05, "1 Coin  3 Credits"	},
	{0x15, 0x01, 0x07, 0x04, "1 Coin  4 Credits"	},
	{0x15, 0x01, 0x07, 0x03, "1 Coin  5 Credits"	},

	{0   , 0xfe, 0   ,    8, "Demo Sounds"		},
	{0x15, 0x01, 0x08, 0x00, "Off"			},
	{0x15, 0x01, 0x08, 0x08, "On"			},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x15, 0x01, 0x10, 0x10, "3"			},
	{0x15, 0x01, 0x10, 0x00, "5"			},
};

STDDIPINFO(Whizz)


// Side Arms - Hyper Dyne (World, 861129)

/*static*/ struct BurnRomInfo sidearmsRomDesc[] = {
	{ "sa03.bin",		0x8000, 0xe10fe6a0, 1 | BRF_PRG | BRF_ESS }, //  0 Main CPU
	{ "a_14e.rom",		0x8000, 0x4925ed03, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "a_12e.rom",		0x8000, 0x81d0ece7, 1 | BRF_PRG | BRF_ESS }, //  2

	{ "a_04k.rom",		0x8000, 0x34efe2d2, 2 | BRF_PRG | BRF_ESS }, //  3 Sound CPU

	{ "b_11j.rom",		0x8000, 0x134dc35b, 7 | BRF_GRA },           //  4 Starfield Data

	{ "a_10j.rom",		0x4000, 0x651fef75, 3 | BRF_GRA },           //  5 Characters

	{ "b_13d.rom",		0x8000, 0x3c59afe1, 4 | BRF_GRA },           //  6 Tiles
	{ "b_13e.rom",		0x8000, 0x64bc3b77, 4 | BRF_GRA },           //  7
	{ "b_13f.rom",		0x8000, 0xe6bcea6f, 4 | BRF_GRA },           //  8
	{ "b_13g.rom",		0x8000, 0xc71a3053, 4 | BRF_GRA },           //  9
	{ "b_14d.rom",		0x8000, 0x826e8a97, 4 | BRF_GRA },           // 10
	{ "b_14e.rom",		0x8000, 0x6cfc02a4, 4 | BRF_GRA },           // 11
	{ "b_14f.rom",		0x8000, 0x9b9f6730, 4 | BRF_GRA },           // 12
	{ "b_14g.rom",		0x8000, 0xef6af630, 4 | BRF_GRA },           // 13

	{ "b_11b.rom",		0x8000, 0xeb6f278c, 5 | BRF_GRA },           // 14 Sprites
	{ "b_13b.rom",		0x8000, 0xe91b4014, 5 | BRF_GRA },           // 15
	{ "b_11a.rom",		0x8000, 0x2822c522, 5 | BRF_GRA },           // 16
	{ "b_13a.rom",		0x8000, 0x3e8a9f75, 5 | BRF_GRA },           // 17
	{ "b_12b.rom",		0x8000, 0x86e43eda, 5 | BRF_GRA },           // 18
	{ "b_14b.rom",		0x8000, 0x076e92d1, 5 | BRF_GRA },           // 19
	{ "b_12a.rom",		0x8000, 0xce107f3c, 5 | BRF_GRA },           // 20
	{ "b_14a.rom",		0x8000, 0xdba06076, 5 | BRF_GRA },           // 21

	{ "b_03d.rom",		0x8000, 0x6f348008, 6 | BRF_GRA },           // 22 Tilemap

	{ "63s141.16h",		0x0100, 0x75af3553, 8 | BRF_OPT },           // 23 Proms
	{ "63s141.11h",		0x0100, 0xa6e4d68f, 8 | BRF_OPT },           // 24
	{ "63s141.15h",		0x0100, 0xc47c182a, 8 | BRF_OPT },           // 25
	{ "63s081.3j",		0x0020, 0xc5817816, 8 | BRF_OPT },           // 26
};

STD_ROM_PICK(sidearms)
STD_ROM_FN(sidearms)



#if (0)

// Side Arms - Hyper Dyne (US, 861202)

/*static*/ struct BurnRomInfo sidearmsuRomDesc[] = {
	{ "SAA_03.15E",		0x8000, 0x32ef2739, 1 | BRF_PRG | BRF_ESS }, //  0 Main CPU
	{ "a_14e.rom",		0x8000, 0x4925ed03, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "a_12e.rom",		0x8000, 0x81d0ece7, 1 | BRF_PRG | BRF_ESS }, //  2

	{ "a_04k.rom",		0x8000, 0x34efe2d2, 2 | BRF_PRG | BRF_ESS }, //  3 Sound CPU

	{ "b_11j.rom",		0x8000, 0x134dc35b, 7 | BRF_GRA },           //  4 Starfield Data

	{ "a_10j.rom",		0x4000, 0x651fef75, 3 | BRF_GRA },           //  5 Characters

	{ "b_13d.rom",		0x8000, 0x3c59afe1, 4 | BRF_GRA },           //  6 Tiles
	{ "b_13e.rom",		0x8000, 0x64bc3b77, 4 | BRF_GRA },           //  7
	{ "b_13f.rom",		0x8000, 0xe6bcea6f, 4 | BRF_GRA },           //  8
	{ "b_13g.rom",		0x8000, 0xc71a3053, 4 | BRF_GRA },           //  9
	{ "b_14d.rom",		0x8000, 0x826e8a97, 4 | BRF_GRA },           // 10
	{ "b_14e.rom",		0x8000, 0x6cfc02a4, 4 | BRF_GRA },           // 11
	{ "b_14f.rom",		0x8000, 0x9b9f6730, 4 | BRF_GRA },           // 12
	{ "b_14g.rom",		0x8000, 0xef6af630, 4 | BRF_GRA },           // 13

	{ "b_11b.rom",		0x8000, 0xeb6f278c, 5 | BRF_GRA },           // 14 Sprites
	{ "b_13b.rom",		0x8000, 0xe91b4014, 5 | BRF_GRA },           // 15
	{ "b_11a.rom",		0x8000, 0x2822c522, 5 | BRF_GRA },           // 16
	{ "b_13a.rom",		0x8000, 0x3e8a9f75, 5 | BRF_GRA },           // 17
	{ "b_12b.rom",		0x8000, 0x86e43eda, 5 | BRF_GRA },           // 18
	{ "b_14b.rom",		0x8000, 0x076e92d1, 5 | BRF_GRA },           // 19
	{ "b_12a.rom",		0x8000, 0xce107f3c, 5 | BRF_GRA },           // 20
	{ "b_14a.rom",		0x8000, 0xdba06076, 5 | BRF_GRA },           // 21

	{ "b_03d.rom",		0x8000, 0x6f348008, 6 | BRF_GRA },           // 22 Tilemap

	{ "63s141.16h",		0x0100, 0x75af3553, 8 | BRF_OPT },           // 23 Proms
	{ "63s141.11h",		0x0100, 0xa6e4d68f, 8 | BRF_OPT },           // 24
	{ "63s141.15h",		0x0100, 0xc47c182a, 8 | BRF_OPT },           // 25
	{ "63s081.3j",		0x0020, 0xc5817816, 8 | BRF_OPT },           // 26
};

STD_ROM_PICK(sidearmsu)
STD_ROM_FN(sidearmsu)

struct BurnDriver BurnDrvSidearmsu = {
	"sidearmsu", "sidearms", NULL, NULL, "1986",
	"Side Arms - Hyper Dyne (US, 861202)\0", NULL, "Capcom (Romstar license)", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, sidearmsuRomInfo, sidearmsuRomName, NULL, NULL, SidearmsInputInfo, SidearmsDIPInfo,
	SidearmsInit, DrvExit, DrvFrame, SidearmsDraw, DrvScan, &DrvRecalc, 0x800,
	384, 224, 4, 3
};


// Side Arms - Hyper Dyne (US, 861128)

/*static*/ struct BurnRomInfo sidearmsur1RomDesc[] = {
	{ "03",				0x8000, 0x9a799c45, 1 | BRF_PRG | BRF_ESS }, //  0 Main CPU
	{ "a_14e.rom",		0x8000, 0x4925ed03, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "a_12e.rom",		0x8000, 0x81d0ece7, 1 | BRF_PRG | BRF_ESS }, //  2

	{ "a_04k.rom",		0x8000, 0x34efe2d2, 2 | BRF_PRG | BRF_ESS }, //  3 Sound CPU

	{ "b_11j.rom",		0x8000, 0x134dc35b, 7 | BRF_GRA },           //  4 Starfield Data

	{ "a_10j.rom",		0x4000, 0x651fef75, 3 | BRF_GRA },           //  5 Characters

	{ "b_13d.rom",		0x8000, 0x3c59afe1, 4 | BRF_GRA },           //  6 Tiles
	{ "b_13e.rom",		0x8000, 0x64bc3b77, 4 | BRF_GRA },           //  7
	{ "b_13f.rom",		0x8000, 0xe6bcea6f, 4 | BRF_GRA },           //  8
	{ "b_13g.rom",		0x8000, 0xc71a3053, 4 | BRF_GRA },           //  9
	{ "b_14d.rom",		0x8000, 0x826e8a97, 4 | BRF_GRA },           // 10
	{ "b_14e.rom",		0x8000, 0x6cfc02a4, 4 | BRF_GRA },           // 11
	{ "b_14f.rom",		0x8000, 0x9b9f6730, 4 | BRF_GRA },           // 12
	{ "b_14g.rom",		0x8000, 0xef6af630, 4 | BRF_GRA },           // 13

	{ "b_11b.rom",		0x8000, 0xeb6f278c, 5 | BRF_GRA },           // 14 Sprites
	{ "b_13b.rom",		0x8000, 0xe91b4014, 5 | BRF_GRA },           // 15
	{ "b_11a.rom",		0x8000, 0x2822c522, 5 | BRF_GRA },           // 16
	{ "b_13a.rom",		0x8000, 0x3e8a9f75, 5 | BRF_GRA },           // 17
	{ "b_12b.rom",		0x8000, 0x86e43eda, 5 | BRF_GRA },           // 18
	{ "b_14b.rom",		0x8000, 0x076e92d1, 5 | BRF_GRA },           // 19
	{ "b_12a.rom",		0x8000, 0xce107f3c, 5 | BRF_GRA },           // 20
	{ "b_14a.rom",		0x8000, 0xdba06076, 5 | BRF_GRA },           // 21

	{ "b_03d.rom",		0x8000, 0x6f348008, 6 | BRF_GRA },           // 22 Tilemap

	{ "63s141.16h",		0x0100, 0x75af3553, 8 | BRF_OPT },           // 23 Proms
	{ "63s141.11h",		0x0100, 0xa6e4d68f, 8 | BRF_OPT },           // 24
	{ "63s141.15h",		0x0100, 0xc47c182a, 8 | BRF_OPT },           // 25
	{ "63s081.3j",		0x0020, 0xc5817816, 8 | BRF_OPT },           // 26
};

STD_ROM_PICK(sidearmsur1)
STD_ROM_FN(sidearmsur1)

struct BurnDriver BurnDrvSidearmsur1 = {
	"sidearmsur1", "sidearms", NULL, NULL, "1986",
	"Side Arms - Hyper Dyne (US, 861128)\0", NULL, "Capcom (Romstar license)", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, sidearmsur1RomInfo, sidearmsur1RomName, NULL, NULL, SidearmsInputInfo, SidearmsDIPInfo,
	SidearmsInit, DrvExit, DrvFrame, SidearmsDraw, DrvScan, &DrvRecalc, 0x800,
	384, 224, 4, 3
};


// Side Arms - Hyper Dyne (Japan, 861128)

/*static*/ struct BurnRomInfo sidearmsjRomDesc[] = {
	{ "a_15e.rom",		0x8000, 0x61ceb0cc, 1 | BRF_PRG | BRF_ESS }, //  0 Main CPU
	{ "a_14e.rom",		0x8000, 0x4925ed03, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "a_12e.rom",		0x8000, 0x81d0ece7, 1 | BRF_PRG | BRF_ESS }, //  2

	{ "a_04k.rom",		0x8000, 0x34efe2d2, 2 | BRF_PRG | BRF_ESS }, //  3 Sound CPU

	{ "b_11j.rom",		0x8000, 0x134dc35b, 7 | BRF_GRA },           //  4 Starfield Data

	{ "a_10j.rom",		0x4000, 0x651fef75, 3 | BRF_GRA },           //  5 Characters

	{ "b_13d.rom",		0x8000, 0x3c59afe1, 4 | BRF_GRA },           //  6 Tiles
	{ "b_13e.rom",		0x8000, 0x64bc3b77, 4 | BRF_GRA },           //  7
	{ "b_13f.rom",		0x8000, 0xe6bcea6f, 4 | BRF_GRA },           //  8
	{ "b_13g.rom",		0x8000, 0xc71a3053, 4 | BRF_GRA },           //  9
	{ "b_14d.rom",		0x8000, 0x826e8a97, 4 | BRF_GRA },           // 10
	{ "b_14e.rom",		0x8000, 0x6cfc02a4, 4 | BRF_GRA },           // 11
	{ "b_14f.rom",		0x8000, 0x9b9f6730, 4 | BRF_GRA },           // 12
	{ "b_14g.rom",		0x8000, 0xef6af630, 4 | BRF_GRA },           // 13

	{ "b_11b.rom",		0x8000, 0xeb6f278c, 5 | BRF_GRA },           // 14 Sprites
	{ "b_13b.rom",		0x8000, 0xe91b4014, 5 | BRF_GRA },           // 15
	{ "b_11a.rom",		0x8000, 0x2822c522, 5 | BRF_GRA },           // 16
	{ "b_13a.rom",		0x8000, 0x3e8a9f75, 5 | BRF_GRA },           // 17
	{ "b_12b.rom",		0x8000, 0x86e43eda, 5 | BRF_GRA },           // 18
	{ "b_14b.rom",		0x8000, 0x076e92d1, 5 | BRF_GRA },           // 19
	{ "b_12a.rom",		0x8000, 0xce107f3c, 5 | BRF_GRA },           // 20
	{ "b_14a.rom",		0x8000, 0xdba06076, 5 | BRF_GRA },           // 21

	{ "b_03d.rom",		0x8000, 0x6f348008, 6 | BRF_GRA },           // 22 Tilemap

	{ "63s141.16h",		0x0100, 0x75af3553, 8 | BRF_OPT },           // 23 Proms
	{ "63s141.11h",		0x0100, 0xa6e4d68f, 8 | BRF_OPT },           // 24
	{ "63s141.15h",		0x0100, 0xc47c182a, 8 | BRF_OPT },           // 25
	{ "63s081.3j",		0x0020, 0xc5817816, 8 | BRF_OPT },           // 26
};

STD_ROM_PICK(sidearmsj)
STD_ROM_FN(sidearmsj)

struct BurnDriver BurnDrvSidearmsj = {
	"sidearmsj", "sidearms", NULL, NULL, "1986",
	"Side Arms - Hyper Dyne (Japan, 861128)\0", NULL, "Capcom", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, sidearmsjRomInfo, sidearmsjRomName, NULL, NULL, SidearmsInputInfo, SidearmsDIPInfo,
	SidearmsInit, DrvExit, DrvFrame, SidearmsDraw, DrvScan, &DrvRecalc, 0x800,
	384, 224, 4, 3
};


// Turtle Ship (North America)

/*static*/ struct BurnRomInfo turtshipRomDesc[] = {
	{ "t-3.bin",		0x08000, 0xb73ed7f2, 1 | BRF_PRG | BRF_ESS }, //  0 Main CPU
	{ "t-2.3g",		0x08000, 0x2327b35a, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "t-1.bin",		0x08000, 0xa258ffec, 1 | BRF_PRG | BRF_ESS }, //  2

	{ "t-4.8a",		0x08000, 0x1cbe48e8, 2 | BRF_PRG | BRF_ESS }, //  3 Sound CPU

	{ "t-5.8k",		0x08000, 0x35c3dbc5, 3 | BRF_GRA },           //  4 Characters

	{ "t-8.1d",		0x10000, 0x30a857f0, 4 | BRF_GRA },           //  5 Tiles
	{ "t-10.3c",		0x10000, 0x76bb73bb, 4 | BRF_GRA },           //  6
	{ "t-11.3d",		0x10000, 0x53da6cb1, 4 | BRF_GRA },           //  7
	{ "t-6.1a",		0x10000, 0x45ce41ad, 4 | BRF_GRA },           //  8
	{ "t-7.1c",		0x10000, 0x3ccf11b9, 4 | BRF_GRA },           //  9
	{ "t-9.3a",		0x10000, 0x44762916, 4 | BRF_GRA },           // 10

	{ "t-13.1i",		0x10000, 0x599f5246, 5 | BRF_GRA },           // 11 Sprites
	{ "t-15.bin",		0x10000, 0x6489b7b4, 5 | BRF_GRA },           // 12
	{ "t-12.1g",		0x10000, 0xfb54cd33, 5 | BRF_GRA },           // 13
	{ "t-14.bin",		0x10000, 0x1b67b674, 5 | BRF_GRA },           // 14

	{ "t-16.9f",		0x08000, 0x1a5a45d7, 6 | BRF_GRA },           // 15 Tilemap
};

STD_ROM_PICK(turtship)
STD_ROM_FN(turtship)

struct BurnDriver BurnDrvTurtship = {
	"turtship", NULL, NULL, NULL, "1988",
	"Turtle Ship (North America)\0", NULL, "Philko (Sharp Image license)", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, turtshipRomInfo, turtshipRomName, NULL, NULL, TurtshipInputInfo, TurtshipDIPInfo,
	TurtshipInit, DrvExit, DrvFrame, TurtshipDraw, DrvScan, &DrvRecalc, 0x800,
	384, 224, 4, 3
};


// Turtle Ship (Japan)

/*static*/ struct BurnRomInfo turtshipjRomDesc[] = {
	{ "t-3.5g",		0x08000, 0x0863fc1c, 1 | BRF_PRG | BRF_ESS }, //  0 Main CPU
	{ "t-2.3g",		0x08000, 0x2327b35a, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "t-1.3e",		0x08000, 0x845a9ab0, 1 | BRF_PRG | BRF_ESS }, //  2

	{ "t-4.8a",		0x08000, 0x1cbe48e8, 2 | BRF_PRG | BRF_ESS }, //  3 Sound CPU

	{ "t-5.8k",		0x08000, 0x35c3dbc5, 3 | BRF_GRA },           //  4 Characters

	{ "t-8.1d",		0x10000, 0x30a857f0, 4 | BRF_GRA },           //  5 Tiles
	{ "t-10.3c",		0x10000, 0x76bb73bb, 4 | BRF_GRA },           //  6
	{ "t-11.3d",		0x10000, 0x53da6cb1, 4 | BRF_GRA },           //  7
	{ "t-6.1a",		0x10000, 0x45ce41ad, 4 | BRF_GRA },           //  8
	{ "t-7.1c",		0x10000, 0x3ccf11b9, 4 | BRF_GRA },           //  9
	{ "t-9.3a",		0x10000, 0x44762916, 4 | BRF_GRA },           // 10

	{ "t-13.1i",		0x10000, 0x599f5246, 5 | BRF_GRA },           // 11 Sprites
	{ "t-15.3i",		0x10000, 0xf30cfa90, 5 | BRF_GRA },           // 12
	{ "t-12.1g",		0x10000, 0xfb54cd33, 5 | BRF_GRA },           // 13
	{ "t-14.3g",		0x10000, 0xd636873c, 5 | BRF_GRA },           // 14

	{ "t-16.9f",		0x08000, 0x1a5a45d7, 6 | BRF_GRA },           // 15 Tilemap
};

STD_ROM_PICK(turtshipj)
STD_ROM_FN(turtshipj)

struct BurnDriver BurnDrvTurtshipj = {
	"turtshipj", "turtship", NULL, NULL, "1988",
	"Turtle Ship (Japan)\0", NULL, "Philko (Pacific Games license)", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, turtshipjRomInfo, turtshipjRomName, NULL, NULL, TurtshipInputInfo, TurtshipDIPInfo,
	TurtshipInit, DrvExit, DrvFrame, TurtshipDraw, DrvScan, &DrvRecalc, 0x800,
	384, 224, 4, 3
};


// Turtle Ship (Korea)

/*static*/ struct BurnRomInfo turtshipkRomDesc[] = {
	{ "turtship.003",	0x08000, 0xe7a7fc2e, 1 | BRF_PRG | BRF_ESS }, //  0 Main CPU
	{ "turtship.002",	0x08000, 0xe576f482, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "turtship.001",	0x08000, 0xa9b64240, 1 | BRF_PRG | BRF_ESS }, //  2

	{ "t-4.8a",		0x08000, 0x1cbe48e8, 2 | BRF_PRG | BRF_ESS }, //  3 Sound CPU

	{ "turtship.005",	0x04000, 0x651fef75, 3 | BRF_GRA },           //  4 Characters

	{ "turtship.008",	0x10000, 0xe0658469, 4 | BRF_GRA },           //  5 Tiles
	{ "t-10.3c",		0x10000, 0x76bb73bb, 4 | BRF_GRA },           //  6
	{ "t-11.3d",		0x10000, 0x53da6cb1, 4 | BRF_GRA },           //  7
	{ "turtship.006",	0x10000, 0xa7cce654, 4 | BRF_GRA },           //  8
	{ "t-7.1c",		0x10000, 0x3ccf11b9, 4 | BRF_GRA },           //  9
	{ "t-9.3a",		0x10000, 0x44762916, 4 | BRF_GRA },           // 10

	{ "t-13.1i",		0x10000, 0x599f5246, 5 | BRF_GRA },           // 11 Sprites
	{ "turtship.015",	0x10000, 0x69fd202f, 5 | BRF_GRA },           // 12
	{ "t-12.1g",		0x10000, 0xfb54cd33, 5 | BRF_GRA },           // 13
	{ "turtship.014",	0x10000, 0xb3ea74a3, 5 | BRF_GRA },           // 14

	{ "turtship.016",	0x08000, 0xaffd51dd, 6 | BRF_GRA },           // 15 Tilemap
};

STD_ROM_PICK(turtshipk)
STD_ROM_FN(turtshipk)

struct BurnDriver BurnDrvTurtshipk = {
	"turtshipk", "turtship", NULL, NULL, "1988",
	"Turtle Ship (Korea)\0", NULL, "Philko", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, turtshipkRomInfo, turtshipkRomName, NULL, NULL, TurtshipInputInfo, TurtshipDIPInfo,
	TurtshipkInit, DrvExit, DrvFrame, TurtshipDraw, DrvScan, &DrvRecalc, 0x800,
	384, 224, 4, 3
};


// Dyger (Korea set 1)

/*static*/ struct BurnRomInfo dygerRomDesc[] = {
	{ "d-3.5g",		0x08000, 0xbae9882e, 1 | BRF_PRG | BRF_ESS }, //  0 Main CPU
	{ "d-2.3g",		0x08000, 0x059ac4dc, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "d-1.3e",		0x08000, 0xd8440f66, 1 | BRF_PRG | BRF_ESS }, //  2

	{ "d-4.8a",		0x08000, 0x8a256c09, 2 | BRF_PRG | BRF_ESS }, //  3 Sound CPU

	{ "d-5.8k",		0x08000, 0xc4bc72a5, 3 | BRF_GRA },           //  4 Characters

	{ "d-10.1d",		0x10000, 0x9715880d, 4 | BRF_GRA },           //  5 Tiles
	{ "d-9.3c",		0x10000, 0x628dae72, 4 | BRF_GRA },           //  6
	{ "d-11.3d",		0x10000, 0x23248db1, 4 | BRF_GRA },           //  7
	{ "d-6.1a",		0x10000, 0x4ba7a437, 4 | BRF_GRA },           //  8
	{ "d-8.1c",		0x10000, 0x6c0f0e0c, 4 | BRF_GRA },           //  9
	{ "d-7.3a",		0x10000, 0x2c50a229, 4 | BRF_GRA },           // 10

	{ "d-14.1i",		0x10000, 0x99c60b26, 5 | BRF_GRA },           // 11 Sprites
	{ "d-15.3i",		0x10000, 0xd6475ecc, 5 | BRF_GRA },           // 12
	{ "d-12.1g",		0x10000, 0xe345705f, 5 | BRF_GRA },           // 13
	{ "d-13.3g",		0x10000, 0xfaf4be3a, 5 | BRF_GRA },           // 14

	{ "d-16.9f",		0x08000, 0x0792e8f2, 6 | BRF_GRA },           // 15 Tilemap
};

STD_ROM_PICK(dyger)
STD_ROM_FN(dyger)

struct BurnDriver BurnDrvDyger = {
	"dyger", NULL, NULL, NULL, "1989",
	"Dyger (Korea set 1)\0", NULL, "Philko", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S, GBF_VERSHOOT, 0,
	NULL, dygerRomInfo, dygerRomName, NULL, NULL, DygerInputInfo, DygerDIPInfo,
	TurtshipInit, DrvExit, DrvFrame, DygerDraw, DrvScan, &DrvRecalc, 0x800,
	224, 384, 3, 4
};


// Dyger (Korea set 2)

/*static*/ struct BurnRomInfo dygeraRomDesc[] = {
	{ "d-3.bin",		0x08000, 0xfc63da8b, 1 | BRF_PRG | BRF_ESS }, //  0 Main CPU
	{ "d-2.3g",		0x08000, 0x059ac4dc, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "d-1.3e",		0x08000, 0xd8440f66, 1 | BRF_PRG | BRF_ESS }, //  2

	{ "d-4.8a",		0x08000, 0x8a256c09, 2 | BRF_PRG | BRF_ESS }, //  3 Sound CPU

	{ "d-5.8k",		0x08000, 0xc4bc72a5, 3 | BRF_GRA },           //  4 Characters

	{ "d-10.1d",		0x10000, 0x9715880d, 4 | BRF_GRA },           //  5 Tiles
	{ "d-9.3c",		0x10000, 0x628dae72, 4 | BRF_GRA },           //  6
	{ "d-11.3d",		0x10000, 0x23248db1, 4 | BRF_GRA },           //  7
	{ "d-6.1a",		0x10000, 0x4ba7a437, 4 | BRF_GRA },           //  8
	{ "d-8.1c",		0x10000, 0x6c0f0e0c, 4 | BRF_GRA },           //  9
	{ "d-7.3a",		0x10000, 0x2c50a229, 4 | BRF_GRA },           // 10

	{ "d-14.1i",		0x10000, 0x99c60b26, 5 | BRF_GRA },           // 11 Sprites
	{ "d-15.3i",		0x10000, 0xd6475ecc, 5 | BRF_GRA },           // 12
	{ "d-12.1g",		0x10000, 0xe345705f, 5 | BRF_GRA },           // 13
	{ "d-13.3g",		0x10000, 0xfaf4be3a, 5 | BRF_GRA },           // 14

	{ "d-16.9f",		0x08000, 0x0792e8f2, 6 | BRF_GRA },           // 15 Tilemap
};

STD_ROM_PICK(dygera)
STD_ROM_FN(dygera)

struct BurnDriver BurnDrvDygera = {
	"dygera", "dyger", NULL, NULL, "1989",
	"Dyger (Korea set 2)\0", NULL, "Philko", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL, 2, HARDWARE_MISC_PRE90S, GBF_VERSHOOT, 0,
	NULL, dygeraRomInfo, dygeraRomName, NULL, NULL, DygerInputInfo, DygerDIPInfo,
	TurtshipInit, DrvExit, DrvFrame, DygerDraw, DrvScan, &DrvRecalc, 0x800,
	224, 384, 3, 4
};


// Twin Falcons

/*static*/ struct BurnRomInfo twinfalcRomDesc[] = {
	{ "t-15.bin",		0x08000, 0xe1f20144, 1 | BRF_PRG | BRF_ESS }, //  0 Main CPU
	{ "t-14.bin",		0x10000, 0xc499ff83, 1 | BRF_PRG | BRF_ESS }, //  1

	{ "t-1.b4",		0x08000, 0xb84bc980, 2 | BRF_PRG | BRF_ESS }, //  2 Sound CPU

	{ "t-6.r6",		0x08000, 0x8e4ca776, 3 | BRF_GRA },           //  3 Characters

	{ "t-10.y10",		0x10000, 0xb678ef5b, 4 | BRF_GRA },           //  4 Tiles
	{ "t-9.w10",		0x10000, 0xd7345fb9, 4 | BRF_GRA },           //  5
	{ "t-8.u10",		0x10000, 0x41428dac, 4 | BRF_GRA },           //  6
	{ "t-13.y11",		0x10000, 0x0eba10bd, 4 | BRF_GRA },           //  7
	{ "t-12.w11",		0x10000, 0xc65050ce, 4 | BRF_GRA },           //  8
	{ "t-11.u11",		0x10000, 0x51a2c65d, 4 | BRF_GRA },           //  9

	{ "t-2.a5",		0x10000, 0x9c106835, 5 | BRF_GRA },           // 10 Sprites
	{ "t-3.b5",		0x10000, 0x9b421ccf, 5 | BRF_GRA },           // 11
	{ "t-4.a7",		0x10000, 0x3a1db986, 5 | BRF_GRA },           // 12
	{ "t-5.b7",		0x10000, 0x9bd22190, 5 | BRF_GRA },           // 13

	{ "t-7.y8",		0x08000, 0xa8b5f750, 6 | BRF_GRA },           // 14 Tilemap
};

STD_ROM_PICK(twinfalc)
STD_ROM_FN(twinfalc)

struct BurnDriver BurnDrvTwinfalc = {
	"twinfalc", NULL, NULL, NULL, "1989",
	"Twin Falcons\0", NULL, "Philko (Poara Enterprises license)", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, twinfalcRomInfo, twinfalcRomName, NULL, NULL, WhizzInputInfo, WhizzDIPInfo,
	WhizzInit, DrvExit, DrvFrame, DygerDraw, DrvScan, &DrvRecalc, 0x800,
	384, 224, 4, 3
};


// Whizz

/*static*/ struct BurnRomInfo whizzRomDesc[] = {
	{ "t-15.l11",		0x08000, 0x73161302, 1 | BRF_PRG | BRF_ESS }, //  0 Main CPU
	{ "t-14.k11",		0x10000, 0xbf248879, 1 | BRF_PRG | BRF_ESS }, //  1

	{ "t-1.b4",		0x08000, 0xb84bc980, 2 | BRF_PRG | BRF_ESS }, //  2 Sound CPU

	{ "t-6.r6",		0x08000, 0x8e4ca776, 3 | BRF_GRA },           //  3 Characters

	{ "t-10.y10",		0x10000, 0xb678ef5b, 4 | BRF_GRA },           //  4 Tiles
	{ "t-9.w10",		0x10000, 0xd7345fb9, 4 | BRF_GRA },           //  5
	{ "t-8.u10",		0x10000, 0x41428dac, 4 | BRF_GRA },           //  6
	{ "t-13.y11",		0x10000, 0x0eba10bd, 4 | BRF_GRA },           //  7
	{ "t-12.w11",		0x10000, 0xc65050ce, 4 | BRF_GRA },           //  8
	{ "t-11.u11",		0x10000, 0x51a2c65d, 4 | BRF_GRA },           //  9

	{ "t-2.a5",		0x10000, 0x9c106835, 5 | BRF_GRA },           // 10 Sprites
	{ "t-3.b5",		0x10000, 0x9b421ccf, 5 | BRF_GRA },           // 11
	{ "t-4.a7",		0x10000, 0x3a1db986, 5 | BRF_GRA },           // 12
	{ "t-5.b7",		0x10000, 0x9bd22190, 5 | BRF_GRA },           // 13

	{ "t-7.y8",		0x08000, 0xa8b5f750, 6 | BRF_GRA },           // 14 Tilemap
};

STD_ROM_PICK(whizz)
STD_ROM_FN(whizz)

struct BurnDriver BurnDrvWhizz = {
	"whizz", "twinfalc", NULL, NULL, "1989",
	"Whizz\0", NULL, "Philko", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, whizzRomInfo, whizzRomName, NULL, NULL, WhizzInputInfo, WhizzDIPInfo,
	WhizzInit, DrvExit, DrvFrame, DygerDraw, DrvScan, &DrvRecalc, 0x800,
	384, 224, 4, 3
};
#endif

#endif