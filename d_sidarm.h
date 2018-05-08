#ifndef _D_SIDARM_H_
#define _D_SIDARM_H_

#include "burnint.h"
#include "saturn/ovl.h"

INT32 SidearmsInit();
INT32 DrvExit();
INT32 DrvFrame();
INT32 SidearmsDraw();
void sidearms_draw_starfield(int *starfield_enable);
static char *itoa(i);
void copyBg();
static void tile32x32toSaturn (unsigned char reverse, unsigned int num, unsigned char *pDest);

/*static*/ UINT32 *bgmap_buf = NULL;
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
#ifdef SOUND
/*static*/ UINT8 *DrvZ80ROM1;
#endif
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
#ifdef SOUND
/*static*/ UINT8 *DrvZ80RAM1;
#endif
/*static*/ UINT16 *DrvPalette;
/*static*/ //UINT8 DrvRecalc;

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
/*static*/ //INT32 is_whizz = 0; // uses ym2151 instead of ym2203
/*static*/ //INT32 is_turtshipk = 0;

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

#endif