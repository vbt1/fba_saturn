#ifndef _D_SIDARM_H_
#define _D_SIDARM_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "saturn/saturn_snd.h"

INT32 SidearmsInit();
INT32 DrvExit();
INT32 DrvFrame();
UINT32 SidearmsDraw();
void sidearms_draw_starfield(int *starfield_enable);
//void vblIn();
static void tile32x32toSaturn (unsigned char reverse, unsigned int num, unsigned char *pDest);
unsigned char current_pcm=255;

SFX sfx_sidarm[96] = {
/*000.pcm*/{0,230400,10},
	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},{0,0,0},
	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},{0,0,0},
	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},{0,0,0},
	{0,0,0},	{0,0,0},	
/*032.pcm*/ {0,307974,0},	// Opening Demo
/*033.pcm*/ {0,935758,60},	// Round 1
/*034.pcm*/ {0,1041490,60},	// Round 2
/*035.pcm*/ {0,893978,60},	// Round 3	
/*036.pcm*/ {0,820502,60},	// Round 4
/*037.pcm*/ {0,631918,60},	// Round 5
/*038.pcm*/ {0,723036,60},	// Round 6
/*039.pcm*/ {0,801736,60},	// Round 7
/*040.pcm*/ {0,862212,60},	// Round 8
/*041.pcm*/ {0,689660,60},	// Round 9	
/*042.pcm*/ {0,307380,60},	// Round Clear
	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},{0,0,0},
/*050.pcm*/ {0,827014,10},	// Battleship A
/*051.pcm*/ {0,866296,10},	// Battleship B	
/*052.pcm*/ {0,842806,10},	// Battleship C
/*053.pcm*/ {0,823754,10},	// Battleship D
/*054.pcm*/ {0,685770,10},	// Final Battleship D (Bahamut Centipede)
	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},{0,0,0},
	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},{0,0,0},
	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},{0,0,0},
	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},{0,0,0},
	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	{0,0,0},	
};


#define VDP2_BASE           0x25e00000
#define VDP2_REGISTER_BASE  (VDP2_BASE+0x180000)
#define BGON    (*(volatile unsigned short *)(VDP2_REGISTER_BASE+0x20))

/*static*/ UINT32 *bgmap_buf = NULL;
/*static*/ UINT16 *bgmap_lut = NULL;
/*static*/ UINT16 *remap4to16_lut = NULL;
/*static*/ UINT16 *map_lut = NULL;  //[256];
/*static*/ UINT16 *map_offset_lut = NULL;  //[2048];
/*static*/ UINT16 *cram_lut = NULL;  //[4096];

/*static*/ UINT8 *CZ80Context = NULL;
/*static*/ UINT8 *AllMem= NULL;
/*static*/ UINT8 *MemEnd= NULL;
/*static*/ UINT8 *AllRam= NULL;
/*static*/ UINT8 *RamEnd= NULL;
/*static*/ UINT8 *DrvZ80ROM0= NULL;
#ifdef SOUND
/*static*/ UINT8 *DrvZ80ROM1= NULL;
#endif
/*static*/ UINT8 *DrvStarMap= NULL;
/*static*/ UINT8 *DrvTileMap= NULL;
/*static*/ UINT8 *DrvVidRAM= NULL;
/*static*/ UINT8 *DrvSprBuf= NULL;
/*static*/ UINT8 *DrvSprRAM= NULL;
/*static*/ UINT8 *DrvPalRAM= NULL;
/*static*/ UINT8 *DrvZ80RAM0= NULL;
#ifdef SOUND
/*static*/ UINT8 *DrvZ80RAM1= NULL;
#endif
/*static*/ UINT8 *bgscrollx= NULL;
/*static*/ UINT8 *bgscrolly= NULL;
/*static*/ UINT8 starfield_enable=0;
/*static*/ UINT8 sprite_enable=0;
/*static*/ UINT8 bglayer_enable=0;
/*static*/ UINT8 bank_data=0;
/*static*/ UINT16 starscrollx=0;
/*static*/ UINT16 starscrolly=0;
/*static*/ UINT32 enable_watchdog=0;
/*static*/ UINT32 watchdog=0;
/*static*/ UINT32 vblank=0;

/*static*/ UINT8 DrvJoy1[8]={0,0,0,0,0,0,0,0};
/*static*/ UINT8 DrvJoy2[8]={0,0,0,0,0,0,0,0};
/*static*/ UINT8 DrvJoy3[8]={0,0,0,0,0,0,0,0};
/*static*/ UINT8 DrvJoy4[8]={0,0,0,0,0,0,0,0};
/*static*/ UINT8 DrvJoy5[8]={0,0,0,0,0,0,0,0};
/*static*/ UINT8 DrvDips[4]={0,0,0,0};
/*static*/ UINT8 DrvInputs[5]={0,0,0,0,0};
/*static*/ UINT8 DrvReset=0;

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
};

STDINPUTINFO(Sidearms)

/*static*/ struct BurnDIPInfo SidearmsDIPList[]=
{
	{0x13, 0xff, 0xff, 0xfc, NULL			},
	{0x14, 0xff, 0xff, 0xff, NULL			},

	{0   , 0xfe, 0   ,    8, "Difficulty"		},
	{0x13, 0x01, 0x07, 0x07, "0 (Easiest)"		},
	{0x13, 0x01, 0x07, 0x06, "1"			},
	{0x13, 0x01, 0x07, 0x05, "2"			},
	{0x13, 0x01, 0x07, 0x04, "3 (Normal)"		},
	{0x13, 0x01, 0x07, 0x03, "4"			},
	{0x13, 0x01, 0x07, 0x02, "5"			},
	{0x13, 0x01, 0x07, 0x01, "6"			},
	{0x13, 0x01, 0x07, 0x00, "7 (Hardest)"		},

	{0   , 0xfe, 0   ,    2, "Lives"		},
	{0x13, 0x01, 0x08, 0x08, "3"			},
	{0x13, 0x01, 0x08, 0x00, "5"			},

	{0   , 0xfe, 0   ,    4, "Bonus Life"		},
	{0x13, 0x01, 0x30, 0x30, "100000"		},
	{0x13, 0x01, 0x30, 0x20, "100000 100000"	},
	{0x13, 0x01, 0x30, 0x10, "150000 150000"	},
	{0x13, 0x01, 0x30, 0x00, "200000 200000"	},

	{0   , 0xfe, 0   ,    2, "Flip Screen"		},
	{0x13, 0x01, 0x40, 0x40, "Off"			},
	{0x13, 0x01, 0x40, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Service Mode"		},
	{0x13, 0x01, 0x80, 0x80, "Off"			},
	{0x13, 0x01, 0x80, 0x00, "On"			},

	{0   , 0xfe, 0   ,    8, "Coin A"		},
	{0x14, 0x01, 0x07, 0x00, "4 Coins 1 Credits"	},
	{0x14, 0x01, 0x07, 0x01, "3 Coins 1 Credits"	},
	{0x14, 0x01, 0x07, 0x02, "2 Coins 1 Credits"	},
	{0x14, 0x01, 0x07, 0x07, "1 Coin  1 Credits"	},
	{0x14, 0x01, 0x07, 0x06, "1 Coin  2 Credits"	},
	{0x14, 0x01, 0x07, 0x05, "1 Coin  3 Credits"	},
	{0x14, 0x01, 0x07, 0x04, "1 Coin  4 Credits"	},
	{0x14, 0x01, 0x07, 0x03, "1 Coin  6 Credits"	},

	{0   , 0xfe, 0   ,    8, "Coin B"		},
	{0x14, 0x01, 0x38, 0x00, "4 Coins 1 Credits"	},
	{0x14, 0x01, 0x38, 0x08, "3 Coins 1 Credits"	},
	{0x14, 0x01, 0x38, 0x10, "2 Coins 1 Credits"	},
	{0x14, 0x01, 0x38, 0x38, "1 Coin  1 Credits"	},
	{0x14, 0x01, 0x38, 0x30, "1 Coin  2 Credits"	},
	{0x14, 0x01, 0x38, 0x28, "1 Coin  3 Credits"	},
	{0x14, 0x01, 0x38, 0x20, "1 Coin  4 Credits"	},
	{0x14, 0x01, 0x38, 0x18, "1 Coin  6 Credits"	},

	{0   , 0xfe, 0   ,    2, "Allow Continue"	},
	{0x14, 0x01, 0x40, 0x00, "No"			},
	{0x14, 0x01, 0x40, 0x40, "Yes"			},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x14, 0x01, 0x80, 0x00, "Off"			},
	{0x14, 0x01, 0x80, 0x80, "On"			},
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