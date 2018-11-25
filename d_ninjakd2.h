#ifndef _D_NJAKD2_H_
#define _D_NJAKD2_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "saturn/saturn_snd.h"

#define VDP2_BASE           0x25e00000
#define VDP2_REGISTER_BASE  (VDP2_BASE+0x180000)
#define BGON    (*(volatile unsigned short *)(VDP2_REGISTER_BASE+0x20))


/*static*/  INT32 RobokidInit();
/*static*/  INT32 DrvExit();
/*static*/  INT32 DrvFrame();

/*static*/  UINT8 *AllMem;
/*static*/  UINT8 *MemEnd;
/*static*/  UINT8 *AllRam;
/*static*/  UINT8 *RamEnd;
/*static*/  UINT8 *DrvZ80ROM0;
/*static*///  UINT8 *DrvZ80ROM1;
/*static*/  UINT8 *DrvGfxROM0;
/*static*/  UINT8 *DrvGfxROM1;
/*static*/  UINT8 *DrvGfxROM2;
/*static*/  UINT8 *DrvGfxROM3;
/*static*/  UINT8 *DrvGfxROM4;
/*static*/  UINT8 *DrvGfxROM4Data1;
/*static*/  UINT8 *DrvGfxROM4Data2;
/*static*///  UINT8 *DrvZ80Key;
/*static*///  UINT8 *DrvSndROM;
/*static*/  UINT8 *DrvZ80RAM0;
/*static*///  UINT8 *DrvZ80RAM1;
/*static*/  UINT8 *DrvSprRAM;
/*static*/  UINT8 *DrvPalRAM;
/*static*/  UINT8 *DrvFgRAM;
/*static*/  UINT8 *DrvBgRAM;
/*static*/  UINT8 *DrvBgRAM0;
/*static*/  UINT8 *DrvBgRAM1;
/*static*/  UINT8 *DrvBgRAM2;

/*static*/ // UINT16 *pSpriteDraw;

/*static*/  UINT16 *DrvPalette;
/*static*/  UINT8 DrvRecalc;

/*static*/  UINT8 soundlatch;
/*static*/  UINT8 flipscreen;

/*static*/  UINT16 scrollx[3];
/*static*/  UINT16 scrolly[3];
/*static*/  UINT8 tilemap_enable[3];
/*static*/  UINT8 overdraw_enable;
/*static*/  UINT8 nZ80RomBank;
/*static*/  UINT8 nZ80RamBank[3];

/*static*/  //UINT8 m_omegaf_io_protection[3];
/*static*/  //UINT8 m_omegaf_io_protection_input;
/*static*/  //INT32 m_omegaf_io_protection_tic;

/*static*/  //INT32 ninjakd2_sample_offset;

/*static*/  UINT8 DrvJoy1[8];
/*static*/  UINT8 DrvJoy2[8];
/*static*/  UINT8 DrvJoy3[8];
/*static*/  UINT8 DrvDips[2];
/*static*/  UINT8 DrvInputs[3];
/*static*/  UINT8 DrvReset;

/*static*/  INT32 previous_coin[2];

/*static*/  struct BurnInputInfo DrvInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 coin"	},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 start"	},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy2 + 3,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy2 + 2,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy2 + 4,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy2 + 5,	"p1 fire 2"	},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy1 + 7,	"p2 coin"	},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy1 + 1,	"p2 start"	},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy3 + 3,	"p2 up"		},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy3 + 2,	"p2 down"	},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy3 + 1,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy3 + 0,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy3 + 4,	"p2 fire 1"	},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy3 + 5,	"p2 fire 2"	},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"		},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"		},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
};

STDINPUTINFO(Drv)

/*static*/  struct BurnInputInfo Drv2InputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 coin"	},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 start"	},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy2 + 3,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy2 + 2,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy2 + 4,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy2 + 5,	"p1 fire 2"	},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy1 + 7,	"p2 coin"	},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy1 + 1,	"p2 start"	},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy3 + 3,	"p2 up"		},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy3 + 2,	"p2 down"	},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy3 + 1,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy3 + 0,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy3 + 4,	"p2 fire 1"	},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy3 + 5,	"p2 fire 2"	},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"		},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"		},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
};

STDINPUTINFO(Drv2)
#if 0
/*static*/  struct BurnInputInfo OmegafInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy1 + 6,	"p1 coin"	},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 start"	},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy2 + 3,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy2 + 2,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy2 + 4,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy2 + 5,	"p1 fire 2"	},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy1 + 7,	"p2 coin"	},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy1 + 1,	"p2 start"	},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy3 + 3,	"p2 up"		},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy3 + 2,	"p2 down"	},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy3 + 1,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy3 + 0,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy3 + 4,	"p2 fire 1"	},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy3 + 5,	"p2 fire 2"	},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"		},
	{"Service",		BIT_DIGITAL,	DrvJoy1 + 5,	"service"	},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"		},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
};

STDINPUTINFO(Omegaf)

/*static*/  struct BurnDIPInfo MnightDIPList[]=
{
	{0x11, 0xff, 0xff, 0xcf, NULL				},
	{0x12, 0xff, 0xff, 0xff, NULL				},

//	{0   , 0xfe, 0   ,    2, "Flip Screen"			},
//	{0x11, 0x01, 0x01, 0x01, "Off"				},
//	{0x11, 0x01, 0x01, 0x00, "On"				},

	{0   , 0xfe, 0   ,    2, "Bonus Life"			},
	{0x11, 0x01, 0x02, 0x02, "30k and every 50k"		},
	{0x11, 0x01, 0x02, 0x00, "50k and every 80k"		},

	{0   , 0xfe, 0   ,    2, "Difficulty"			},
	{0x11, 0x01, 0x04, 0x04, "Normal"			},
	{0x11, 0x01, 0x04, 0x00, "Difficult"			},

	{0   , 0xfe, 0   ,    2, "Infinite Lives"		},
	{0x11, 0x01, 0x08, 0x08, "Off"				},
	{0x11, 0x01, 0x08, 0x00, "On"				},

//	{0   , 0xfe, 0   ,    2, "Cabinet"			},
//	{0x11, 0x01, 0x10, 0x00, "Upright"			},
//	{0x11, 0x01, 0x10, 0x10, "Cocktail"			},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"			},
	{0x11, 0x01, 0x20, 0x20, "Off"				},
	{0x11, 0x01, 0x20, 0x00, "On"				},

	{0   , 0xfe, 0   ,    4, "Lives"			},
	{0x11, 0x01, 0xc0, 0x80, "2"				},
	{0x11, 0x01, 0xc0, 0xc0, "3"				},
	{0x11, 0x01, 0xc0, 0x40, "4"				},
	{0x11, 0x01, 0xc0, 0x00, "5"				},

	{0   , 0xfe, 0   ,    2, "Service Mode"			},
	{0x12, 0x01, 0x01, 0x01, "Off"				},
	{0x12, 0x01, 0x01, 0x00, "On"				},

	{0   , 0xfe, 0   ,    8, "Coinage"			},
	{0x12, 0x01, 0xe0, 0x00, "5 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x20, "4 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x40, "3 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x60, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0xe0, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0xe0, 0xc0, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0xe0, 0xa0, "1 Coin  3 Credits"		},
	{0x12, 0x01, 0xe0, 0x80, "1 Coin  4 Credits"		},
};

STDDIPINFO(Mnight)

/*static*/  struct BurnDIPInfo Ninjakd2DIPList[]=
{
	{0x11, 0xff, 0xff, 0x6f, NULL				},
	{0x12, 0xff, 0xff, 0xfd, NULL				},

//	{0   , 0xfe, 0   ,    2, "Flip Screen"			},
//	{0x11, 0x01, 0x01, 0x01, "Off"				},
//	{0x11, 0x01, 0x01, 0x00, "On"				},

	{0   , 0xfe, 0   ,    4, "Bonus Life"			},
	{0x11, 0x01, 0x06, 0x04, "20000 and every 50000"	},
	{0x11, 0x01, 0x06, 0x06, "30000 and every 50000"	},
	{0x11, 0x01, 0x06, 0x02, "50000 and every 50000"	},
	{0x11, 0x01, 0x06, 0x00, "None"				},

	{0   , 0xfe, 0   ,    2, "Allow Continue"		},
	{0x11, 0x01, 0x08, 0x00, "No"				},
	{0x11, 0x01, 0x08, 0x08, "Yes"				},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"			},
	{0x11, 0x01, 0x10, 0x10, "Off"				},
	{0x11, 0x01, 0x10, 0x00, "On"				},

	{0   , 0xfe, 0   ,    2, "Difficulty"			},
	{0x11, 0x01, 0x20, 0x20, "Normal"			},
	{0x11, 0x01, 0x20, 0x00, "Hard"				},

	{0   , 0xfe, 0   ,    2, "Lives"			},
	{0x11, 0x01, 0x40, 0x40, "3"				},
	{0x11, 0x01, 0x40, 0x00, "4"				},

	{0   , 0xfe, 0   ,    2, "Language"			},
	{0x11, 0x01, 0x80, 0x00, "English"			},
	{0x11, 0x01, 0x80, 0x80, "Japanese"			},

	{0   , 0xfe, 0   ,    2, "Service Mode"			},
	{0x12, 0x01, 0x01, 0x01, "Off"				},
	{0x12, 0x01, 0x01, 0x00, "On"				},
		
//	{0   , 0xfe, 0   ,    2, "Cabinet"			},
//	{0x12, 0x01, 0x02, 0x00, "Upright"			},
//	{0x12, 0x01, 0x02, 0x02, "Cocktail"			},

	{0   , 0xfe, 0   ,    2, "Credit Service"		},
	{0x12, 0x01, 0x04, 0x00, "Off"				},
	{0x12, 0x01, 0x04, 0x04, "On"				},

	{0   , 0xfe, 0   ,    8, "Coin B"			},
	{0x12, 0x01, 0x18, 0x00, "2 Coins/1 Credit, 6/4"	},
	{0x12, 0x01, 0x18, 0x18, "1 Coin/1 Credit, 3/4"		},
	{0x12, 0x01, 0x18, 0x10, "1 Coin/2 Credits, 2/6, 3/10"	},
	{0x12, 0x01, 0x18, 0x08, "1 Coin/3 Credits, 3/12"	},
	{0x12, 0x01, 0x18, 0x00, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0x18, 0x18, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0x18, 0x10, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0x18, 0x08, "1 Coin  3 Credits"		},

	{0   , 0xfe, 0   ,   16, "Coin A"			},
	{0x12, 0x01, 0xe0, 0x00, "5 Coins/1 Credit, 15/4"	},
	{0x12, 0x01, 0xe0, 0x20, "4 Coins/1 Credit, 12/4"	},
	{0x12, 0x01, 0xe0, 0x40, "3 Coins/1 Credit, 9/4"	},
	{0x12, 0x01, 0xe0, 0x60, "2 Coins/1 Credit, 6/4"	},
	{0x12, 0x01, 0xe0, 0xe0, "1 Coin/1 Credit, 3/4"		},
	{0x12, 0x01, 0xe0, 0xc0, "1 Coin/2 Credits, 2/6, 3/10"	},
	{0x12, 0x01, 0xe0, 0xa0, "1 Coin/3 Credits, 3/12"	},
	{0x12, 0x01, 0xe0, 0x80, "1 Coin  4 Credits"		},
	{0x12, 0x01, 0xe0, 0x00, "5 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x20, "4 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x40, "3 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x60, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0xe0, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0xe0, 0xc0, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0xe0, 0xa0, "1 Coin  3 Credits"		},
	{0x12, 0x01, 0xe0, 0x80, "1 Coin  4 Credits"		},
};

STDDIPINFO(Ninjakd2)

/*static*/  struct BurnDIPInfo RdactionDIPList[]=
{
	{0x11, 0xff, 0xff, 0x6f, NULL				},
	{0x12, 0xff, 0xff, 0xfd, NULL				},

//	{0   , 0xfe, 0   ,    2, "Flip Screen"			},
//	{0x11, 0x01, 0x01, 0x01, "Off"				},
//	{0x11, 0x01, 0x01, 0x00, "On"				},

	{0   , 0xfe, 0   ,    3, "Bonus Life"			},
	{0x11, 0x01, 0x06, 0x04, "20000 and every 50000"	},
	{0x11, 0x01, 0x06, 0x06, "30000 and every 50000"	},
	{0x11, 0x01, 0x06, 0x02, "50000 and every 100000"	},

	{0   , 0xfe, 0   ,    2, "Allow Continue"		},
	{0x11, 0x01, 0x08, 0x00, "No"				},
	{0x11, 0x01, 0x08, 0x08, "Yes"				},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"			},
	{0x11, 0x01, 0x10, 0x10, "Off"				},
	{0x11, 0x01, 0x10, 0x00, "On"				},

	{0   , 0xfe, 0   ,    2, "Difficulty"			},
	{0x11, 0x01, 0x20, 0x20, "Normal"			},
	{0x11, 0x01, 0x20, 0x00, "Hard"				},

	{0   , 0xfe, 0   ,    2, "Lives"			},
	{0x11, 0x01, 0x40, 0x40, "3"				},
	{0x11, 0x01, 0x40, 0x00, "4"				},

	{0   , 0xfe, 0   ,    2, "Language"			},
	{0x11, 0x01, 0x80, 0x00, "English"			},
	{0x11, 0x01, 0x80, 0x80, "Japanese"			},

	{0   , 0xfe, 0   ,    2, "Service Mode"			},
	{0x12, 0x01, 0x01, 0x01, "Off"				},
	{0x12, 0x01, 0x01, 0x00, "On"				},

//	{0   , 0xfe, 0   ,    2, "Cabinet"			},
//	{0x12, 0x01, 0x02, 0x00, "Upright"			},
//	{0x12, 0x01, 0x02, 0x02, "Cocktail"			},

	{0   , 0xfe, 0   ,    2, "Credit Service"		},
	{0x12, 0x01, 0x04, 0x00, "Off"				},
	{0x12, 0x01, 0x04, 0x04, "On"				},

	{0   , 0xfe, 0   ,    8, "Coin B"			},
	{0x12, 0x01, 0x18, 0x00, "2 Coins/1 Credit, 6/4"	},
	{0x12, 0x01, 0x18, 0x18, "1 Coin/1 Credit, 3/4"		},
	{0x12, 0x01, 0x18, 0x10, "1 Coin/2 Credits, 2/6, 3/10"	},
	{0x12, 0x01, 0x18, 0x08, "1 Coin/3 Credits, 3/12"	},
	{0x12, 0x01, 0x18, 0x00, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0x18, 0x18, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0x18, 0x10, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0x18, 0x08, "1 Coin  3 Credits"		},

	{0   , 0xfe, 0   ,   16, "Coin A"			},
	{0x12, 0x01, 0xe0, 0x00, "5 Coins/1 Credit, 15/4"	},
	{0x12, 0x01, 0xe0, 0x20, "4 Coins/1 Credit, 12/4"	},
	{0x12, 0x01, 0xe0, 0x40, "3 Coins/1 Credit, 9/4"	},
	{0x12, 0x01, 0xe0, 0x60, "2 Coins/1 Credit, 6/4"	},
	{0x12, 0x01, 0xe0, 0xe0, "1 Coin/1 Credit, 3/4"		},
	{0x12, 0x01, 0xe0, 0xc0, "1 Coin/2 Credits, 2/6, 3/10"	},
	{0x12, 0x01, 0xe0, 0xa0, "1 Coin/3 Credits, 3/12"	},
	{0x12, 0x01, 0xe0, 0x80, "1 Coin  4 Credits"		},
	{0x12, 0x01, 0xe0, 0x00, "5 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x20, "4 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x40, "3 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x60, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0xe0, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0xe0, 0xc0, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0xe0, 0xa0, "1 Coin  3 Credits"		},
	{0x12, 0x01, 0xe0, 0x80, "1 Coin  4 Credits"		},
};

STDDIPINFO(Rdaction)

/*static*/  struct BurnDIPInfo ArkareaDIPList[]=
{
	{0x11, 0xff, 0xff, 0xef, NULL				},
	{0x12, 0xff, 0xff, 0xff, NULL				},

	{0   , 0xfe, 0   ,    4, "Coinage"			},
	{0x11, 0x01, 0x03, 0x00, "3 Coins 1 Credits"		},
	{0x11, 0x01, 0x03, 0x01, "2 Coins 1 Credits"		},
	{0x11, 0x01, 0x03, 0x03, "1 Coin  1 Credits"		},
	{0x11, 0x01, 0x03, 0x02, "1 Coin  2 Credits"		},

//	{0   , 0xfe, 0   ,    2, "Flip Screen"			},
//	{0x11, 0x01, 0x04, 0x04, "Off"				},
//	{0x11, 0x01, 0x04, 0x00, "On"				},

	{0   , 0xfe, 0   ,    0, "Demo Sounds"			},
	{0x11, 0x01, 0x10, 0x10, "Off"				},
	{0x11, 0x01, 0x10, 0x00, "On"				},

	{0   , 0xfe, 0   ,    2, "Difficulty"			},
	{0x11, 0x01, 0x20, 0x20, "Normal"			},
	{0x11, 0x01, 0x20, 0x00, "Hard"				},

	{0   , 0xfe, 0   ,    2, "Bonus Life"			},
	{0x11, 0x01, 0x40, 0x40, "50000 and every 50000"	},
	{0x11, 0x01, 0x40, 0x00, "100000 and every 100000"	},

	{0   , 0xfe, 0   ,    2, "Lives"			},
	{0x11, 0x01, 0x80, 0x80, "3"				},
	{0x11, 0x01, 0x80, 0x00, "4"				},

	{0   , 0xfe, 0   ,    2, "Service Mode"			},
	{0x12, 0x01, 0x01, 0x01, "Off"				},
	{0x12, 0x01, 0x01, 0x00, "On"				},
};

STDDIPINFO(Arkarea)
#endif 

/*static*/  struct BurnDIPInfo RobokidDIPList[]=
{
	{0x11, 0xff, 0xff, 0xcf, NULL				},
	{0x12, 0xff, 0xff, 0xff, NULL				},

//	{0   , 0xfe, 0   ,    2, "Flip Screen"			},
//	{0x11, 0x01, 0x01, 0x01, "Off"				},
//	{0x11, 0x01, 0x01, 0x00, "On"				},

	{0   , 0xfe, 0   ,    2, "Bonus Life"			},
	{0x11, 0x01, 0x02, 0x02, "50000 and every 100000"		},
	{0x11, 0x01, 0x02, 0x00, "None"				},

	{0   , 0xfe, 0   ,    2, "Difficulty"			},
	{0x11, 0x01, 0x04, 0x04, "Normal"			},
	{0x11, 0x01, 0x04, 0x00, "Hard"				},

	{0   , 0xfe, 0   ,    2, "Free Play"			},
	{0x11, 0x01, 0x08, 0x08, "Off"				},
	{0x11, 0x01, 0x08, 0x00, "On"				},

//	{0   , 0xfe, 0   ,    2, "Cabinet"			},
//	{0x11, 0x01, 0x10, 0x00, "Upright"			},
//	{0x11, 0x01, 0x10, 0x10, "Cocktail"			},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"			},
	{0x11, 0x01, 0x20, 0x20, "Off"				},
	{0x11, 0x01, 0x20, 0x00, "On"				},

	{0   , 0xfe, 0   ,    4, "Lives"			},
	{0x11, 0x01, 0xc0, 0x80, "2"				},
	{0x11, 0x01, 0xc0, 0xc0, "3"				},
	{0x11, 0x01, 0xc0, 0x40, "4"				},
	{0x11, 0x01, 0xc0, 0x00, "5"				},

	{0   , 0xfe, 0   ,    2, "Service Mode"			},
	{0x12, 0x01, 0x01, 0x01, "Off"				},
	{0x12, 0x01, 0x01, 0x00, "On"				},

	{0   , 0xfe, 0   ,    8, "Coinage"			},
	{0x12, 0x01, 0xe0, 0x00, "5 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x20, "4 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x40, "3 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x60, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0xe0, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0xe0, 0xc0, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0xe0, 0xa0, "1 Coin  3 Credits"		},
	{0x12, 0x01, 0xe0, 0x80, "1 Coin  4 Credits"		},
};

STDDIPINFO(Robokid)
#if	 0
/*static*/  struct BurnDIPInfo RobokidjDIPList[]=
{
	{0x11, 0xff, 0xff, 0xcf, NULL				},
	{0x12, 0xff, 0xff, 0xff, NULL				},

//	{0   , 0xfe, 0   ,    2, "Flip Screen"			},
//	{0x11, 0x01, 0x01, 0x01, "Off"				},
//	{0x11, 0x01, 0x01, 0x00, "On"				},

	{0   , 0xfe, 0   ,    2, "Bonus Life"			},
	{0x11, 0x01, 0x02, 0x02, "30000 and every 50000"	},
	{0x11, 0x01, 0x02, 0x00, "50000 and every 80000"	},

	{0   , 0xfe, 0   ,    2, "Difficulty"			},
	{0x11, 0x01, 0x04, 0x04, "Normal"			},
	{0x11, 0x01, 0x04, 0x00, "Hard"				},

	{0   , 0xfe, 0   ,    2, "Free Play"			},
	{0x11, 0x01, 0x08, 0x08, "Off"				},
	{0x11, 0x01, 0x08, 0x00, "On"				},

//	{0   , 0xfe, 0   ,    2, "Cabinet"			},
//	{0x11, 0x01, 0x10, 0x00, "Upright"			},
//	{0x11, 0x01, 0x10, 0x10, "Cocktail"			},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"			},
	{0x11, 0x01, 0x20, 0x20, "Off"				},
	{0x11, 0x01, 0x20, 0x00, "On"				},

	{0   , 0xfe, 0   ,    4, "Lives"			},
	{0x11, 0x01, 0xc0, 0x80, "2"				},
	{0x11, 0x01, 0xc0, 0xc0, "3"				},
	{0x11, 0x01, 0xc0, 0x40, "4"				},
	{0x11, 0x01, 0xc0, 0x00, "5"				},

	{0   , 0xfe, 0   ,    2, "Service Mode"			},
	{0x12, 0x01, 0x01, 0x01, "Off"				},
	{0x12, 0x01, 0x01, 0x00, "On"				},

	{0   , 0xfe, 0   ,    8, "Coinage"			},
	{0x12, 0x01, 0xe0, 0x00, "5 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x20, "4 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x40, "3 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0x60, "2 Coins 1 Credits"		},
	{0x12, 0x01, 0xe0, 0xe0, "1 Coin  1 Credits"		},
	{0x12, 0x01, 0xe0, 0xc0, "1 Coin  2 Credits"		},
	{0x12, 0x01, 0xe0, 0xa0, "1 Coin  3 Credits"		},
	{0x12, 0x01, 0xe0, 0x80, "1 Coin  4 Credits"		},
};

STDDIPINFOEXT(Robokidj, Robokid, Robokidj)

/*static*/  struct BurnDIPInfo OmegafDIPList[]=
{
	{0x12, 0xff, 0xff, 0xff, NULL				},
	{0x13, 0xff, 0xff, 0xff, NULL				},

//	{0   , 0xfe, 0   ,    2, "Flip Screen"			},
//	{0x12, 0x01, 0x01, 0x01, "Off"				},
//	{0x12, 0x01, 0x01, 0x00, "On"				},

	{0   , 0xfe, 0   ,    4, "Difficulty"			},
	{0x12, 0x01, 0x06, 0x00, "Easy"				},
	{0x12, 0x01, 0x06, 0x06, "Normal"			},
	{0x12, 0x01, 0x06, 0x02, "Hard"				},
	{0x12, 0x01, 0x06, 0x04, "Hardest"			},

	{0   , 0xfe, 0   ,    2, "Service Mode"			},
	{0x12, 0x01, 0x08, 0x08, "Off"				},
	{0x12, 0x01, 0x08, 0x00, "On"				},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"			},
	{0x12, 0x01, 0x20, 0x00, "Off"				},
	{0x12, 0x01, 0x20, 0x20, "On"				},

	{0   , 0xfe, 0   ,    4, "Lives"			},
	{0x12, 0x01, 0xc0, 0x00, "2"				},
	{0x12, 0x01, 0xc0, 0xc0, "3"				},
	{0x12, 0x01, 0xc0, 0x40, "4"				},
	{0x12, 0x01, 0xc0, 0x80, "5"				},

	{0   , 0xfe, 0   ,    4, "Bonus Life"			},
	{0x13, 0x01, 0x03, 0x00, "200000"			},
	{0x13, 0x01, 0x03, 0x03, "300000"			},
	{0x13, 0x01, 0x03, 0x01, "500000"			},
	{0x13, 0x01, 0x03, 0x02, "1000000"			},

	{0   , 0xfe, 0   ,    8, "Coin B"			},
	{0x13, 0x01, 0x1c, 0x00, "5 Coins 1 Credits"		},
	{0x13, 0x01, 0x1c, 0x10, "4 Coins 1 Credits"		},
	{0x13, 0x01, 0x1c, 0x08, "3 Coins 1 Credits"		},
	{0x13, 0x01, 0x1c, 0x18, "2 Coins 1 Credits"		},
	{0x13, 0x01, 0x1c, 0x1c, "1 Coin  1 Credits"		},
	{0x13, 0x01, 0x1c, 0x0c, "1 Coin  2 Credits"		},
	{0x13, 0x01, 0x1c, 0x14, "1 Coin  3 Credits"		},
	{0x13, 0x01, 0x1c, 0x04, "1 Coin  4 Credits"		},

	{0   , 0xfe, 0   ,    8, "Coin A"			},
	{0x13, 0x01, 0xe0, 0x00, "5 Coins 1 Credits"		},
	{0x13, 0x01, 0xe0, 0x80, "4 Coins 1 Credits"		},
	{0x13, 0x01, 0xe0, 0x40, "3 Coins 1 Credits"		},
	{0x13, 0x01, 0xe0, 0xc0, "2 Coins 1 Credits"		},
	{0x13, 0x01, 0xe0, 0xe0, "1 Coin  1 Credits"		},
	{0x13, 0x01, 0xe0, 0x60, "1 Coin  2 Credits"		},
	{0x13, 0x01, 0xe0, 0xa0, "1 Coin  3 Credits"		},
	{0x13, 0x01, 0xe0, 0x20, "1 Coin  4 Credits"		},
};

STDDIPINFO(Omegaf)

// Ninja-Kid II / NinjaKun Ashura no Shou (set 1)

/*static*/  struct BurnRomInfo ninjakd2RomDesc[] = {
	{ "nk2_01.rom",		0x08000, 0x3cdbb906, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "nk2_02.rom",		0x08000, 0xb5ce9a1a, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "nk2_03.rom",		0x08000, 0xad275654, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "nk2_04.rom",		0x08000, 0xe7692a77, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "nk2_05.rom",		0x08000, 0x5dac9426, 1 | BRF_PRG | BRF_ESS }, //  4

	{ "nk2_06.rom",		0x10000, 0xd3a18a79, 2 | BRF_PRG | BRF_ESS }, //  5 Z80 #1 Code (mc8123 encrypted)

	{ "nk2_12.rom",		0x08000, 0xdb5657a9, 3 | BRF_GRA },           //  6 Foreground Tiles

	{ "nk2_08.rom",		0x10000, 0x1b79c50a, 4 | BRF_GRA },           //  7 Sprite Tiles
	{ "nk2_07.rom",		0x10000, 0x0be5cd13, 4 | BRF_GRA },           //  8

	{ "nk2_11.rom",		0x10000, 0x41a714b3, 5 | BRF_GRA },           //  9 Background Tiles
	{ "nk2_10.rom",		0x10000, 0xc913c4ab, 5 | BRF_GRA },           // 10

	{ "nk2_09.rom",		0x10000, 0xc1d2d170, 6 | BRF_GRA },           // 11 Samples (8 bit unsigned)

	{ "ninjakd2.key",	0x02000, 0xec25318f, 7 | BRF_PRG | BRF_ESS }, // 12 mc8123 key
};

STD_ROM_PICK(ninjakd2)
STD_ROM_FN(ninjakd2)
/*
struct BurnDriver BurnDrvNinjakd2 = {
	"ninjakd2", NULL, NULL, NULL, "1987",
	"Ninja-Kid II / NinjaKun Ashura no Shou (set 1)\0", NULL, "UPL", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_SCRFIGHT, 0,
	NULL, ninjakd2RomInfo, ninjakd2RomName, NULL, NULL, DrvInputInfo, Ninjakd2DIPInfo,
	Ninjakd2Init, DrvExit, DrvFrame, Ninjakd2Draw, DrvScan, &DrvRecalc, 0x300,
	256, 192, 4, 3
};
*/

// Ninja-Kid II / NinjaKun Ashura no Shou (set 2, bootleg?)

/*static*/  struct BurnRomInfo ninjakd2aRomDesc[] = {
	{ "nk2_01.bin",		0x08000, 0xe6adca65, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "nk2_02.bin",		0x08000, 0xd9284bd1, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "nk2_03.rom",		0x08000, 0xad275654, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "nk2_04.rom",		0x08000, 0xe7692a77, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "nk2_05.bin",		0x08000, 0x960725fb, 1 | BRF_PRG | BRF_ESS }, //  4

	{ "nk2_06.bin",		0x10000, 0x7bfe6c9e, 2 | BRF_PRG | BRF_ESS }, //  5 Z80 #1 Code

	{ "nk2_12.rom",		0x08000, 0xdb5657a9, 3 | BRF_GRA },           //  6 Foreground Tiles

	{ "nk2_08.rom",		0x10000, 0x1b79c50a, 4 | BRF_GRA },           //  7 Sprite Tiles
	{ "nk2_07.rom",		0x10000, 0x0be5cd13, 4 | BRF_GRA },           //  8

	{ "nk2_11.rom",		0x10000, 0x41a714b3, 5 | BRF_GRA },           //  9 Background Tiles
	{ "nk2_10.rom",		0x10000, 0xc913c4ab, 5 | BRF_GRA },           // 10

	{ "nk2_09.rom",		0x10000, 0xc1d2d170, 6 | BRF_GRA },           // 11 Samples (8 bit unsigned)
};

STD_ROM_PICK(ninjakd2a)
STD_ROM_FN(ninjakd2a)
/*
struct BurnDriver BurnDrvNinjakd2a = {
	"ninjakd2a", "ninjakd2", NULL, NULL, "1987",
	"Ninja-Kid II / NinjaKun Ashura no Shou (set 2, bootleg?)\0", NULL, "UPL", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_SCRFIGHT, 0,
	NULL, ninjakd2aRomInfo, ninjakd2aRomName, NULL, NULL, DrvInputInfo, Ninjakd2DIPInfo,
	Ninjakd2DecryptedInit, DrvExit, DrvFrame, Ninjakd2Draw, DrvScan, &DrvRecalc, 0x300,
	256, 192, 4, 3
};
*/

// Ninja-Kid II / NinjaKun Ashura no Shou (set 3, bootleg?)

/*static*/  struct BurnRomInfo ninjakd2bRomDesc[] = {
	{ "1.3s",			0x08000, 0xcb4f4624, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "2.3q",			0x08000, 0x0ad0c100, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "nk2_03.rom",		0x08000, 0xad275654, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "nk2_04.rom",		0x08000, 0xe7692a77, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "nk2_05.rom",		0x08000, 0x5dac9426, 1 | BRF_PRG | BRF_ESS }, //  4

	{ "nk2_06.bin",		0x10000, 0x7bfe6c9e, 2 | BRF_PRG | BRF_ESS }, //  5 Z80 #1 Code

	{ "nk2_12.rom",		0x08000, 0xdb5657a9, 3 | BRF_GRA },           //  6 Foreground Tiles

	{ "nk2_08.rom",		0x10000, 0x1b79c50a, 4 | BRF_GRA },           //  7 Sprite Tiles
	{ "nk2_07.rom",		0x10000, 0x0be5cd13, 4 | BRF_GRA },           //  8

	{ "nk2_11.rom",		0x10000, 0x41a714b3, 5 | BRF_GRA },           //  9 Background Tiles
	{ "nk2_10.rom",		0x10000, 0xc913c4ab, 5 | BRF_GRA },           // 10

	{ "nk2_09.rom",		0x10000, 0xc1d2d170, 6 | BRF_GRA },           // 11 Samples (8 bit unsigned)
};

STD_ROM_PICK(ninjakd2b)
STD_ROM_FN(ninjakd2b)
/*
struct BurnDriver BurnDrvNinjakd2b = {
	"ninjakd2b", "ninjakd2", NULL, NULL, "1987",
	"Ninja-Kid II / NinjaKun Ashura no Shou (set 3, bootleg?)\0", NULL, "UPL", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_SCRFIGHT, 0,
	NULL, ninjakd2bRomInfo, ninjakd2bRomName, NULL, NULL, DrvInputInfo, RdactionDIPInfo,
	Ninjakd2DecryptedInit, DrvExit, DrvFrame, Ninjakd2Draw, DrvScan, &DrvRecalc, 0x300,
	256, 192, 4, 3
};
*/

// Ninja-Kid II / NinjaKun Ashura no Shou (set 4)
// close to set 3

/*static*/  struct BurnRomInfo ninjakd2cRomDesc[] = {
	{ "1.3u",			0x08000, 0x06096412, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "2.3t",			0x08000, 0x9ed9a994, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "nk2_03.rom",		0x08000, 0xad275654, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "nk2_04.rom",		0x08000, 0xe7692a77, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "5.3m",			0x08000, 0x800d4951, 1 | BRF_PRG | BRF_ESS }, //  4

	{ "nk2_06.rom",		0x10000, 0xd3a18a79, 2 | BRF_PRG | BRF_ESS }, //  5 Z80 #1 Code (mc8123 encrypted)

	{ "nk2_12.rom",		0x08000, 0xdb5657a9, 3 | BRF_GRA },           //  6 Foreground Tiles

	{ "nk2_08.rom",		0x10000, 0x1b79c50a, 4 | BRF_GRA },           //  7 Sprite Tiles
	{ "nk2_07.rom",		0x10000, 0x0be5cd13, 4 | BRF_GRA },           //  8

	{ "nk2_11.rom",		0x10000, 0x41a714b3, 5 | BRF_GRA },           //  9 Background Tiles
	{ "nk2_10.rom",		0x10000, 0xc913c4ab, 5 | BRF_GRA },           // 10

	{ "nk2_09.rom",		0x10000, 0xc1d2d170, 6 | BRF_GRA },           // 11 Samples (8 bit unsigned)
	
	{ "ninjakd2.key",	0x02000, 0xec25318f, 7 | BRF_PRG | BRF_ESS }, // 12 mc8123 key
};

STD_ROM_PICK(ninjakd2c)
STD_ROM_FN(ninjakd2c)
/*
struct BurnDriver BurnDrvNinjakd2c = {
	"ninjakd2c", "ninjakd2", NULL, NULL, "1987",
	"Ninja-Kid II / NinjaKun Ashura no Shou (set 4)\0", NULL, "UPL", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_SCRFIGHT, 0,
	NULL, ninjakd2cRomInfo, ninjakd2cRomName, NULL, NULL, DrvInputInfo, RdactionDIPInfo,
	Ninjakd2Init, DrvExit, DrvFrame, Ninjakd2Draw, DrvScan, &DrvRecalc, 0x300,
	256, 192, 4, 3
};
*/

// Rad Action / NinjaKun Ashura no Shou

/*static*/  struct BurnRomInfo rdactionRomDesc[] = {
	{ "1.3u",			0x08000, 0x5c475611, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "2.3s",			0x08000, 0xa1e23bd2, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "nk2_03.rom",		0x08000, 0xad275654, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "nk2_04.rom",		0x08000, 0xe7692a77, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "nk2_05.bin",		0x08000, 0x960725fb, 1 | BRF_PRG | BRF_ESS }, //  4

	{ "nk2_06.rom",		0x10000, 0xd3a18a79, 2 | BRF_PRG | BRF_ESS }, //  5 Z80 #1 Code (mc8123 encrypted)

	{ "12.5n",			0x08000, 0x0936b365, 3 | BRF_GRA },           //  6 Foreground Tiles

	{ "nk2_08.rom",		0x10000, 0x1b79c50a, 4 | BRF_GRA },           //  7 Sprite Tiles
	{ "nk2_07.rom",		0x10000, 0x0be5cd13, 4 | BRF_GRA },           //  8

	{ "nk2_11.rom",		0x10000, 0x41a714b3, 5 | BRF_GRA },           //  9 Background Tiles
	{ "nk2_10.rom",		0x10000, 0xc913c4ab, 5 | BRF_GRA },           // 10

	{ "nk2_09.rom",		0x10000, 0xc1d2d170, 6 | BRF_GRA },           // 11 Samples (8 bit unsigned)

	{ "ninjakd2.key",	0x02000, 0xec25318f, 7 | BRF_PRG | BRF_ESS }, // 12 mc8123 key
};

STD_ROM_PICK(rdaction)
STD_ROM_FN(rdaction)
/*
struct BurnDriver BurnDrvRdaction = {
	"rdaction", "ninjakd2", NULL, NULL, "1987",
	"Rad Action / NinjaKun Ashura no Shou\0", NULL, "UPL (World Games license)", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_SCRFIGHT, 0,
	NULL, rdactionRomInfo, rdactionRomName, NULL, NULL, DrvInputInfo, RdactionDIPInfo,
	Ninjakd2Init, DrvExit, DrvFrame, Ninjakd2Draw, DrvScan, &DrvRecalc, 0x300,
	256, 192, 4, 3
};
*/

// JT-104 (title screen modification of Rad Action)
// identical to rdaction set with different gfx rom and decrypted sound rom

/*static*/  struct BurnRomInfo jt104RomDesc[] = {
	{ "1.3u",			0x08000, 0x5c475611, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "2.3s",			0x08000, 0xa1e23bd2, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "nk2_03.rom",		0x08000, 0xad275654, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "nk2_04.rom",		0x08000, 0xe7692a77, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "nk2_05.bin",		0x08000, 0x960725fb, 1 | BRF_PRG | BRF_ESS }, //  4

	{ "nk2_06.bin",		0x10000, 0x7bfe6c9e, 2 | BRF_PRG | BRF_ESS }, //  5 Z80 #1 Code

	{ "jt_104_12.bin",	0x08000, 0xc038fadb, 3 | BRF_GRA },           //  6 Foreground Tiles

	{ "nk2_08.rom",		0x10000, 0x1b79c50a, 4 | BRF_GRA },           //  7 Sprite Tiles
	{ "nk2_07.rom",		0x10000, 0x0be5cd13, 4 | BRF_GRA },           //  8

	{ "nk2_11.rom",		0x10000, 0x41a714b3, 5 | BRF_GRA },           //  9 Background Tiles
	{ "nk2_10.rom",		0x10000, 0xc913c4ab, 5 | BRF_GRA },           // 10

	{ "nk2_09.rom",		0x10000, 0xc1d2d170, 6 | BRF_GRA },           // 11 Samples (8 bit unsigned)

	{ "ninjakd2.key",	0x02000, 0xec25318f, 7 | BRF_PRG | BRF_ESS }, // 12 mc8123 key
};

STD_ROM_PICK(jt104)
STD_ROM_FN(jt104)
/*
struct BurnDriver BurnDrvJt104 = {
	"jt104", "ninjakd2", NULL, NULL, "1987",
	"JT-104 (title screen modification of Rad Action)\0", NULL, "UPL (United Amusements license)", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_SCRFIGHT, 0,
	NULL, jt104RomInfo, jt104RomName, NULL, NULL, DrvInputInfo, RdactionDIPInfo,
	Ninjakd2DecryptedInit, DrvExit, DrvFrame, Ninjakd2Draw, DrvScan, &DrvRecalc, 0x300,
	256, 192, 4, 3
};
*/

// Mutant Night 

/*static*/  struct BurnRomInfo mnightRomDesc[] = {
	{ "1.j19",			0x08000, 0x56678d14, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "2.j17",			0x08000, 0x2a73f88e, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "3.j16",			0x08000, 0xc5e42bb4, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "4.j14",			0x08000, 0xdf6a4f7a, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "5.j12",			0x08000, 0x9c391d1b, 1 | BRF_PRG | BRF_ESS }, //  4

	{ "6.j7",			0x10000, 0xa0782a31, 2 | BRF_PRG | BRF_ESS }, //  5 Z80 #1 Code

	{ "13.b10",			0x08000, 0x8c177a19, 3 | BRF_GRA },           //  6 Foreground Tiles

	{ "9.e11",			0x10000, 0x4883059c, 4 | BRF_GRA },           //  7 Sprite Tiles
	{ "8.e12",			0x10000, 0x02b91445, 4 | BRF_GRA },           //  8
	{ "7.e14",			0x10000, 0x9f08d160, 4 | BRF_GRA },           //  9

	{ "12.b20",			0x10000, 0x4d37e0f4, 5 | BRF_GRA },           // 10 Background Tiles
	{ "11.b22",			0x10000, 0xb22cbbd3, 5 | BRF_GRA },           // 11
	{ "10.b23",			0x10000, 0x65714070, 5 | BRF_GRA },           // 12
};

STD_ROM_PICK(mnight)
STD_ROM_FN(mnight)
/*
struct BurnDriver BurnDrvMnight = {
	"mnight", NULL, NULL, NULL, "1987",
	"Mutant Night\0", NULL, "UPL", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_SCRFIGHT, 0,
	NULL, mnightRomInfo, mnightRomName, NULL, NULL, DrvInputInfo, MnightDIPInfo,
	MnightInit, DrvExit, DrvFrame, MnightDraw, DrvScan, &DrvRecalc, 0x300,
	256, 192, 4, 3
};
*/

// Mutant Night (Japan)

/*static*/  struct BurnRomInfo mnightjRomDesc[] = {
	{ "1.j19",			0x08000, 0x56678d14, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "2.j17",			0x08000, 0x2a73f88e, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "3.j16",			0x08000, 0xc5e42bb4, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "4.j14",			0x08000, 0xdf6a4f7a, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "5.j12",			0x08000, 0x9c391d1b, 1 | BRF_PRG | BRF_ESS }, //  4

	{ "6.j7",			0x10000, 0xa0782a31, 2 | BRF_PRG | BRF_ESS }, //  5 Z80 #1 Code

	{ "13.b10",			0x08000, 0x37b8221f, 3 | BRF_GRA },           //  6 Foreground Tiles

	{ "9.e11",			0x10000, 0x4883059c, 4 | BRF_GRA },           //  7 Sprite Tiles
	{ "8.e12",			0x10000, 0x02b91445, 4 | BRF_GRA },           //  8
	{ "7.e14",			0x10000, 0x9f08d160, 4 | BRF_GRA },           //  9

	{ "12.b20",			0x10000, 0x4d37e0f4, 5 | BRF_GRA },           // 10 Background Tiles
	{ "11.b22",			0x10000, 0xb22cbbd3, 5 | BRF_GRA },           // 11
	{ "10.b23",			0x10000, 0x65714070, 5 | BRF_GRA },           // 12
};

STD_ROM_PICK(mnightj)
STD_ROM_FN(mnightj)
/*
struct BurnDriver BurnDrvMnightj = {
	"mnightj", "mnight", NULL, NULL, "1987",
	"Mutant Night (Japan)\0", NULL, "UPL (Kawakus license)", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_SCRFIGHT, 0,
	NULL, mnightjRomInfo, mnightjRomName, NULL, NULL, DrvInputInfo, MnightDIPInfo,
	MnightInit, DrvExit, DrvFrame, MnightDraw, DrvScan, &DrvRecalc, 0x300,
	256, 192, 4, 3
};
*/

// Ark Area

/*static*/  struct BurnRomInfo arkareaRomDesc[] = {
	{ "arkarea.008",	0x08000, 0x1ce1b5b9, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "arkarea.009",	0x08000, 0xdb1c81d1, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "arkarea.010",	0x08000, 0x5a460dae, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "arkarea.011",	0x08000, 0x63f022c9, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "arkarea.012",	0x08000, 0x3c4c65d5, 1 | BRF_PRG | BRF_ESS }, //  4

	{ "arkarea.013",	0x08000, 0x2d409d58, 2 | BRF_PRG | BRF_ESS }, //  5 Z80 #1 Code

	{ "arkarea.004",	0x08000, 0x69e36af2, 3 | BRF_GRA },           //  6 Foreground Tiles

	{ "arkarea.007",	0x10000, 0xd5684a27, 4 | BRF_GRA },           //  7 Sprite Tiles
	{ "arkarea.006",	0x10000, 0x2c0567d6, 4 | BRF_GRA },           //  8
	{ "arkarea.005",	0x10000, 0x9886004d, 4 | BRF_GRA },           //  9

	{ "arkarea.003",	0x10000, 0x6f45a308, 5 | BRF_GRA },           // 10 Background Tiles
	{ "arkarea.002",	0x10000, 0x051d3482, 5 | BRF_GRA },           // 11
	{ "arkarea.001",	0x10000, 0x09d11ab7, 5 | BRF_GRA },           // 12
};

STD_ROM_PICK(arkarea)
STD_ROM_FN(arkarea)
/*
struct BurnDriver BurnDrvArkarea = {
	"arkarea", NULL, NULL, NULL, "1988",
	"Ark Area\0", NULL, "UPL", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_SHOOT, 0,
	NULL, arkareaRomInfo, arkareaRomName, NULL, NULL, Drv2InputInfo, ArkareaDIPInfo,
	MnightInit, DrvExit, DrvFrame, MnightDraw, DrvScan, &DrvRecalc, 0x300,
	256, 192, 4, 3
};
*/
#endif

// Atomic Robo-kid (World, Type-2)

/*static*/  struct BurnRomInfo robokidRomDesc[] = {
	{ "robokid1.18j",	0x10000, 0x378c21fc, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "robokid2.18k",	0x10000, 0xddef8c5a, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "robokid3.15k",	0x10000, 0x05295ec3, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "robokid4.12k",	0x10000, 0x3bc3977f, 1 | BRF_PRG | BRF_ESS }, //  3

	{ "robokid.k7",		0x10000, 0xf490a2e9, 2 | BRF_PRG | BRF_ESS }, //  4 Z80 #1 Code

	{ "robokid.b9",		0x08000, 0xfac59c3f, 3 | BRF_GRA },           //  5 Foreground Tiles

	{ "robokid.15f",	0x10000, 0xba61f5ab, 4 | BRF_GRA },           //  6 Sprite Tiles
	{ "robokid.16f",	0x10000, 0xd9b399ce, 4 | BRF_GRA },           //  7
	{ "robokid.17f",	0x10000, 0xafe432b9, 4 | BRF_GRA },           //  8
	{ "robokid.18f",	0x10000, 0xa0aa2a84, 4 | BRF_GRA },           //  9

	{ "robokid.19c",	0x10000, 0x02220421, 5 | BRF_GRA },           // 10 Background Layer 0 Tiles
	{ "robokid.20c",	0x10000, 0x02d59bc2, 5 | BRF_GRA },           // 11
	{ "robokid.17d",	0x10000, 0x2fa29b99, 5 | BRF_GRA },           // 12
	{ "robokid.18d",	0x10000, 0xae15ce02, 5 | BRF_GRA },           // 13
	{ "robokid.19d",	0x10000, 0x784b089e, 5 | BRF_GRA },           // 14
	{ "robokid.20d",	0x10000, 0xb0b395ed, 5 | BRF_GRA },           // 15
	{ "robokid.19f",	0x10000, 0x0f9071c6, 5 | BRF_GRA },           // 16

	{ "robokid.12c",	0x10000, 0x0ab45f94, 6 | BRF_GRA },           // 17 Background Layer 1 Tiles
	{ "robokid.14c",	0x10000, 0x029bbd4a, 6 | BRF_GRA },           // 18
	{ "robokid.15c",	0x10000, 0x7de67ebb, 6 | BRF_GRA },           // 19
	{ "robokid.16c",	0x10000, 0x53c0e582, 6 | BRF_GRA },           // 20
	{ "robokid.17c",	0x10000, 0x0cae5a1e, 6 | BRF_GRA },           // 21
	{ "robokid.18c",	0x10000, 0x56ac7c8a, 6 | BRF_GRA },           // 22
	{ "robokid.15d",	0x10000, 0xcd632a4d, 6 | BRF_GRA },           // 23
	{ "robokid.16d",	0x10000, 0x18d92b2b, 6 | BRF_GRA },           // 24

	{ "robokid.12a",	0x10000, 0xe64d1c10, 7 | BRF_GRA },           // 25 Background Layer 2 Tiles
	{ "robokid.14a",	0x10000, 0x8f9371e4, 7 | BRF_GRA },           // 26
	{ "robokid.15a",	0x10000, 0x469204e7, 7 | BRF_GRA },           // 27
	{ "robokid.16a",	0x10000, 0x4e340815, 7 | BRF_GRA },           // 28
	{ "robokid.17a",	0x10000, 0xf0863106, 7 | BRF_GRA },           // 29
	{ "robokid.18a",	0x10000, 0xfdff7441, 7 | BRF_GRA },           // 30
	
	{ "prom82s.129",	0x00100, 0x4dd96f67, 0 | BRF_OPT },
};

STD_ROM_PICK(robokid)
STD_ROM_FN(robokid)
/*
struct BurnDriver BurnDrvRobokid = {
	"robokid", NULL, NULL, NULL, "1988",
	"Atomic Robo-kid (World, Type-2)\0", NULL, "UPL", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, robokidRomInfo, robokidRomName, NULL, NULL, DrvInputInfo, RobokidDIPInfo,
	RobokidInit, DrvExit, DrvFrame, RobokidDraw, RobokidScan, &DrvRecalc, 0x400,
	256, 192, 4, 3
};
*/
#if	 0
// Atomic Robo-kid (Japan, Type-2, set 1)

/*static*/  struct BurnRomInfo robokidjRomDesc[] = {
	{ "1.29",			0x10000, 0x59a1e2ec, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "2.30",			0x10000, 0xe3f73476, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "robokid3.15k",	0x10000, 0x05295ec3, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "robokid4.12k",	0x10000, 0x3bc3977f, 1 | BRF_PRG | BRF_ESS }, //  3

	{ "robokid.k7",		0x10000, 0xf490a2e9, 2 | BRF_PRG | BRF_ESS }, //  4 Z80 #1 Code

	{ "robokid.b9",		0x08000, 0xfac59c3f, 3 | BRF_GRA },           //  5 Foreground Tiles

	{ "robokid.15f",	0x10000, 0xba61f5ab, 4 | BRF_GRA },           //  6 Sprite Tiles
	{ "robokid.16f",	0x10000, 0xd9b399ce, 4 | BRF_GRA },           //  7
	{ "robokid.17f",	0x10000, 0xafe432b9, 4 | BRF_GRA },           //  8
	{ "robokid.18f",	0x10000, 0xa0aa2a84, 4 | BRF_GRA },           //  9

	{ "robokid.19c",	0x10000, 0x02220421, 5 | BRF_GRA },           // 10 Background Layer 0 Tiles
	{ "robokid.20c",	0x10000, 0x02d59bc2, 5 | BRF_GRA },           // 11
	{ "robokid.17d",	0x10000, 0x2fa29b99, 5 | BRF_GRA },           // 12
	{ "robokid.18d",	0x10000, 0xae15ce02, 5 | BRF_GRA },           // 13
	{ "robokid.19d",	0x10000, 0x784b089e, 5 | BRF_GRA },           // 14
	{ "robokid.20d",	0x10000, 0xb0b395ed, 5 | BRF_GRA },           // 15
	{ "robokid.19f",	0x10000, 0x0f9071c6, 5 | BRF_GRA },           // 16

	{ "robokid.12c",	0x10000, 0x0ab45f94, 6 | BRF_GRA },           // 17 Background Layer 1 Tiles
	{ "robokid.14c",	0x10000, 0x029bbd4a, 6 | BRF_GRA },           // 18
	{ "robokid.15c",	0x10000, 0x7de67ebb, 6 | BRF_GRA },           // 19
	{ "robokid.16c",	0x10000, 0x53c0e582, 6 | BRF_GRA },           // 20
	{ "robokid.17c",	0x10000, 0x0cae5a1e, 6 | BRF_GRA },           // 21
	{ "robokid.18c",	0x10000, 0x56ac7c8a, 6 | BRF_GRA },           // 22
	{ "robokid.15d",	0x10000, 0xcd632a4d, 6 | BRF_GRA },           // 23
	{ "robokid.16d",	0x10000, 0x18d92b2b, 6 | BRF_GRA },           // 24

	{ "robokid.12a",	0x10000, 0xe64d1c10, 7 | BRF_GRA },           // 25 Background Layer 2 Tiles
	{ "robokid.14a",	0x10000, 0x8f9371e4, 7 | BRF_GRA },           // 26
	{ "robokid.15a",	0x10000, 0x469204e7, 7 | BRF_GRA },           // 27
	{ "robokid.16a",	0x10000, 0x4e340815, 7 | BRF_GRA },           // 28
	{ "robokid.17a",	0x10000, 0xf0863106, 7 | BRF_GRA },           // 29
	{ "robokid.18a",	0x10000, 0xfdff7441, 7 | BRF_GRA },           // 30
	
	{ "prom82s129.cpu",	0x00100, 0x4dd96f67, 0 | BRF_OPT },
};

STD_ROM_PICK(robokidj)
STD_ROM_FN(robokidj)
/*
struct BurnDriver BurnDrvRobokidj = {
	"robokidj", "robokid", NULL, NULL, "1988",
	"Atomic Robo-kid (Japan, Type-2, set 1)\0", NULL, "UPL", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, robokidjRomInfo, robokidjRomName, NULL, NULL, DrvInputInfo, RobokidjDIPInfo,
	RobokidInit, DrvExit, DrvFrame, RobokidDraw, RobokidScan, &DrvRecalc, 0x400,
	256, 192, 4, 3
};
*/

// Atomic Robo-kid (Japan, Type-2, set 2)

/*static*/  struct BurnRomInfo robokidj2RomDesc[] = {
	{ "1_rom29.18j",	0x10000, 0x969fb951, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "2_rom30.18k",	0x10000, 0xc0228b63, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "robokid3.15k",	0x10000, 0x05295ec3, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "robokid4.12k",	0x10000, 0x3bc3977f, 1 | BRF_PRG | BRF_ESS }, //  3

	{ "robokid.k7",		0x10000, 0xf490a2e9, 2 | BRF_PRG | BRF_ESS }, //  4 Z80 #1 Code

	{ "robokid.b9",		0x08000, 0xfac59c3f, 3 | BRF_GRA },           //  5 Foreground Tiles

	{ "robokid.15f",	0x10000, 0xba61f5ab, 4 | BRF_GRA },           //  6 Sprite Tiles
	{ "robokid.16f",	0x10000, 0xd9b399ce, 4 | BRF_GRA },           //  7
	{ "robokid.17f",	0x10000, 0xafe432b9, 4 | BRF_GRA },           //  8
	{ "robokid.18f",	0x10000, 0xa0aa2a84, 4 | BRF_GRA },           //  9

	{ "robokid.19c",	0x10000, 0x02220421, 5 | BRF_GRA },           // 10 Background Layer 0 Tiles
	{ "robokid.20c",	0x10000, 0x02d59bc2, 5 | BRF_GRA },           // 11
	{ "robokid.17d",	0x10000, 0x2fa29b99, 5 | BRF_GRA },           // 12
	{ "robokid.18d",	0x10000, 0xae15ce02, 5 | BRF_GRA },           // 13
	{ "robokid.19d",	0x10000, 0x784b089e, 5 | BRF_GRA },           // 14
	{ "robokid.20d",	0x10000, 0xb0b395ed, 5 | BRF_GRA },           // 15
	{ "robokid.19f",	0x10000, 0x0f9071c6, 5 | BRF_GRA },           // 16

	{ "robokid.12c",	0x10000, 0x0ab45f94, 6 | BRF_GRA },           // 17 Background Layer 1 Tiles
	{ "robokid.14c",	0x10000, 0x029bbd4a, 6 | BRF_GRA },           // 18
	{ "robokid.15c",	0x10000, 0x7de67ebb, 6 | BRF_GRA },           // 19
	{ "robokid.16c",	0x10000, 0x53c0e582, 6 | BRF_GRA },           // 20
	{ "robokid.17c",	0x10000, 0x0cae5a1e, 6 | BRF_GRA },           // 21
	{ "robokid.18c",	0x10000, 0x56ac7c8a, 6 | BRF_GRA },           // 22
	{ "robokid.15d",	0x10000, 0xcd632a4d, 6 | BRF_GRA },           // 23
	{ "robokid.16d",	0x10000, 0x18d92b2b, 6 | BRF_GRA },           // 24

	{ "robokid.12a",	0x10000, 0xe64d1c10, 7 | BRF_GRA },           // 25 Background Layer 2 Tiles
	{ "robokid.14a",	0x10000, 0x8f9371e4, 7 | BRF_GRA },           // 26
	{ "robokid.15a",	0x10000, 0x469204e7, 7 | BRF_GRA },           // 27
	{ "robokid.16a",	0x10000, 0x4e340815, 7 | BRF_GRA },           // 28
	{ "robokid.17a",	0x10000, 0xf0863106, 7 | BRF_GRA },           // 29
	{ "robokid.18a",	0x10000, 0xfdff7441, 7 | BRF_GRA },           // 30
	
	{ "prom82s129.cpu",	0x00100, 0x4dd96f67, 0 | BRF_OPT },
};

STD_ROM_PICK(robokidj2)
STD_ROM_FN(robokidj2)
/*
struct BurnDriver BurnDrvRobokidj2 = {
	"robokidj2", "robokid", NULL, NULL, "1988",
	"Atomic Robo-kid (Japan, Type-2, set 2)\0", NULL, "UPL", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, robokidj2RomInfo, robokidj2RomName, NULL, NULL, DrvInputInfo, RobokidjDIPInfo,
	RobokidInit, DrvExit, DrvFrame, RobokidDraw, RobokidScan, &DrvRecalc, 0x400,
	256, 192, 4, 3
};
*/

// Atomic Robo-kid (Japan)

/*static*/  struct BurnRomInfo robokidj3RomDesc[] = {
	{ "robokid1.18j",	0x10000, 0x77a9332a, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "robokid2.18k",	0x10000, 0x715ecee4, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "robokid3.15k",	0x10000, 0xce12fa86, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "robokid4.12k",	0x10000, 0x97e86600, 1 | BRF_PRG | BRF_ESS }, //  3

	{ "robokid.k7",		0x10000, 0xf490a2e9, 2 | BRF_PRG | BRF_ESS }, //  4 Z80 #1 Code

	{ "robokid.b9",		0x08000, 0xfac59c3f, 3 | BRF_GRA },           //  5 Foreground Tiles

	{ "robokid.15f",	0x10000, 0xba61f5ab, 4 | BRF_GRA },           //  6 Sprite Tiles
	{ "robokid.16f",	0x10000, 0xd9b399ce, 4 | BRF_GRA },           //  7
	{ "robokid.17f",	0x10000, 0xafe432b9, 4 | BRF_GRA },           //  8
	{ "robokid.18f",	0x10000, 0xa0aa2a84, 4 | BRF_GRA },           //  9

	{ "robokid.19c",	0x10000, 0x02220421, 5 | BRF_GRA },           // 10 Background Layer 0 Tiles
	{ "robokid.20c",	0x10000, 0x02d59bc2, 5 | BRF_GRA },           // 11
	{ "robokid.17d",	0x10000, 0x2fa29b99, 5 | BRF_GRA },           // 12
	{ "robokid.18d",	0x10000, 0xae15ce02, 5 | BRF_GRA },           // 13
	{ "robokid.19d",	0x10000, 0x784b089e, 5 | BRF_GRA },           // 14
	{ "robokid.20d",	0x10000, 0xb0b395ed, 5 | BRF_GRA },           // 15
	{ "robokid.19f",	0x10000, 0x0f9071c6, 5 | BRF_GRA },           // 16

	{ "robokid.12c",	0x10000, 0x0ab45f94, 6 | BRF_GRA },           // 17 Background Layer 1 Tiles
	{ "robokid.14c",	0x10000, 0x029bbd4a, 6 | BRF_GRA },           // 18
	{ "robokid.15c",	0x10000, 0x7de67ebb, 6 | BRF_GRA },           // 19
	{ "robokid.16c",	0x10000, 0x53c0e582, 6 | BRF_GRA },           // 20
	{ "robokid.17c",	0x10000, 0x0cae5a1e, 6 | BRF_GRA },           // 21
	{ "robokid.18c",	0x10000, 0x56ac7c8a, 6 | BRF_GRA },           // 22
	{ "robokid.15d",	0x10000, 0xcd632a4d, 6 | BRF_GRA },           // 23
	{ "robokid.16d",	0x10000, 0x18d92b2b, 6 | BRF_GRA },           // 24

	{ "robokid.12a",	0x10000, 0xe64d1c10, 7 | BRF_GRA },           // 25 Background Layer 2 Tiles
	{ "robokid.14a",	0x10000, 0x8f9371e4, 7 | BRF_GRA },           // 26
	{ "robokid.15a",	0x10000, 0x469204e7, 7 | BRF_GRA },           // 27
	{ "robokid.16a",	0x10000, 0x4e340815, 7 | BRF_GRA },           // 28
	{ "robokid.17a",	0x10000, 0xf0863106, 7 | BRF_GRA },           // 29
	{ "robokid.18a",	0x10000, 0xfdff7441, 7 | BRF_GRA },           // 30
	
	{ "prom82s129.cpu",	0x00100, 0x4dd96f67, 0 | BRF_OPT },
};

STD_ROM_PICK(robokidj3)
STD_ROM_FN(robokidj3)
/*
struct BurnDriver BurnDrvRobokidj3 = {
	"robokidj3", "robokid", NULL, NULL, "1988",
	"Atomic Robo-kid (Japan)\0", NULL, "UPL", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_HORSHOOT, 0,
	NULL, robokidj3RomInfo, robokidj3RomName, NULL, NULL, DrvInputInfo, RobokidjDIPInfo,
	RobokidInit, DrvExit, DrvFrame, RobokidDraw, RobokidScan, &DrvRecalc, 0x400,
	256, 192, 4, 3
};
*/

// Omega Fighter

/*static*/  struct BurnRomInfo omegafRomDesc[] = {
	{ "1.5",		0x20000, 0x57a7fd96, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "6.4l",		0x20000, 0x6277735c, 1 | BRF_PRG | BRF_ESS }, //  1

	{ "7.7m",		0x10000, 0xd40fc8d5, 2 | BRF_PRG | BRF_ESS }, //  2 Z80 #1 Code

	{ "4.18h",		0x08000, 0x9e2d8152, 3 | BRF_GRA },           //  3 Foreground Tiles

	{ "8.23m",		0x20000, 0x0bd2a5d1, 4 | BRF_GRA },           //  4 Sprite Tiles

	{ "2back1.27b",		0x80000, 0x21f8a32e, 5 | BRF_GRA },           //  5 Background Layer 0 Tiles

	{ "1back2.15b",		0x80000, 0x6210ddcc, 6 | BRF_GRA },           //  6 Background Layer 1 Tiles

	{ "3back3.5f",		0x80000, 0xc31cae56, 7 | BRF_GRA },           //  7 Background Layer 2 Tiles
};

STD_ROM_PICK(omegaf)
STD_ROM_FN(omegaf)
/*
struct BurnDriver BurnDrvOmegaf = {
	"omegaf", NULL, NULL, NULL, "1989",
	"Omega Fighter\0", NULL, "UPL", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_VERSHOOT, 0,
	NULL, omegafRomInfo, omegafRomName, NULL, NULL, OmegafInputInfo, OmegafDIPInfo,
	OmegafInit, DrvExit, DrvFrame, OmegafDraw, OmegafScan, &DrvRecalc, 0x400,
	192, 256, 3, 4
};
*/

// Omega Fighter Special

/*static*/  struct BurnRomInfo omegafsRomDesc[] = {
	{ "5.3l",		0x20000, 0x503a3e63, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 #0 Code
	{ "6.4l",		0x20000, 0x6277735c, 1 | BRF_PRG | BRF_ESS }, //  1

	{ "7.7m",		0x10000, 0xd40fc8d5, 2 | BRF_PRG | BRF_ESS }, //  2 Z80 #1 Code

	{ "4.18h",		0x08000, 0x9e2d8152, 3 | BRF_GRA },           //  3 Foreground Tiles

	{ "8.23m",		0x20000, 0x0bd2a5d1, 4 | BRF_GRA },           //  4 Sprite Tiles

	{ "2back1.27b",		0x80000, 0x21f8a32e, 5 | BRF_GRA },           //  5 Background Layer 0 Tiles

	{ "1back2.15b",		0x80000, 0x6210ddcc, 6 | BRF_GRA },           //  6 Background Layer 1 Tiles

	{ "3back3.5f",		0x80000, 0xc31cae56, 7 | BRF_GRA },           //  7 Background Layer 2 Tiles
};

STD_ROM_PICK(omegafs)
STD_ROM_FN(omegafs)
/*
struct BurnDriver BurnDrvOmegafs = {
	"omegafs", "omegaf", NULL, NULL, "1989",
	"Omega Fighter Special\0", NULL, "UPL", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL | BDF_HISCORE_SUPPORTED, 2, HARDWARE_MISC_PRE90S, GBF_VERSHOOT, 0,
	NULL, omegafsRomInfo, omegafsRomName, NULL, NULL, OmegafInputInfo, OmegafDIPInfo,
	OmegafInit, DrvExit, DrvFrame, OmegafDraw, OmegafScan, &DrvRecalc, 0x400,
	192, 256, 3, 4
};
*/
#endif

#endif
