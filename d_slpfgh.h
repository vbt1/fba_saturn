#ifndef _D_SLAPFGHT_H_
#define _D_SLAPFGHT_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "snd/ay8910.h"
#include "czet.h"

#define nBurnSoundLen 128
#define	true	1
#define	false	0

//void updateSound(unsigned int *nSoundBufferPos);
void updateSound();
INT32 DrvInit();
INT32 DrvExit();
void DrvFrame();
void DrvDoReset();
static void Set6PCM();
void PCM_MeStop(PcmHn hn);
inline void rotate_tile16x16(unsigned int size, unsigned char *target);
void  SCL_SetColRamOffset(Uint32 Object, Uint32 Offset,Uint8 transparent);
void DrvLoadRoms(UINT8 nWhichGame);

//typedef int bool;
UINT8 irq_enable = 0;
UINT8 sound_nmi_enable = 0;

UINT32 nStatusIndex = 0;
UINT8 nProtectIndex = 0;
UINT32 nSndIrqFrame = 0; //tigerh 6, slapf 3, perfr 4

INT32 scrollx = 0;
INT32 scrolly = 0;
UINT32 nTigerHeliTileMask = 0; 

UINT8 *DrvZ80ROM0 = NULL;
UINT8 *DrvZ80RAM0 = NULL;
//UINT8 *RamShared = NULL;
UINT8 *DrvVidRAM = NULL;
UINT8 *DrvSprRAM = NULL;
UINT8 *DrvSprBuf = NULL;
UINT8 *DrvTxtRAM = NULL;
UINT8 *CZ80Context = NULL;
UINT16 *map_offset_lut = NULL;
//UINT16 *map_offset_lut2 = NULL;

PcmHn pcm6[6] = {NULL,NULL,NULL,NULL,NULL,NULL};
#define	PCM_ADDR	((void*)0x25a20000)
#define	PCM_SIZE	(4096L*2)				/* 2.. */
#define SOUNDRATE   7680L
// ---------------------------------------------------------------------------
// Inputs

UINT8 DrvDips[2] = {0,0};
UINT8 DrvJoy1[8] = {0,0,0,0,0,0,0,0};
UINT8 DrvJoy2[8] = {0,0,0,0,0,0,0,0};
UINT8 DrvInputs[2] = {0,0};
// Dip Switch and Input Definitions

static struct BurnInputInfo SlapfighInputList[] = {
	{"P1 Coin",			BIT_DIGITAL,	DrvJoy2 + 6,	"p1 coin"	},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy2 + 4,	"p1 start"	},
	{"P1 Up",			BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"		},
	{"P1 Down",			BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"	},
	{"P1 Left",			BIT_DIGITAL,	DrvJoy1 + 3,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy2 + 1,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 fire 2"	},

	{"P2 Coin",			BIT_DIGITAL,	DrvJoy2 + 7,	"p2 coin"	},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy2 + 5,	"p2 start"	},
	{"P2 Up",			BIT_DIGITAL,	DrvJoy1 + 4,	"p2 up"		},
	{"P2 Down",			BIT_DIGITAL,	DrvJoy1 + 5,	"p2 down"	},
	{"P2 Left",			BIT_DIGITAL,	DrvJoy1 + 7,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy1 + 6,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 fire 1"	},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 fire 2"	},

	{"Reset",			BIT_DIGITAL,	NULL,		"reset"		},
	{"Dip A",			BIT_DIPSWITCH,	DrvDips + 0,	"dip"		},
	{"Dip B",			BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
};

STDINPUTINFO(Slapfigh)

static struct BurnInputInfo TigerhInputList[] = {
	{"P1 Coin",			BIT_DIGITAL,	DrvJoy2 + 6,	"p1 coin"	},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy2 + 4,	"p1 start"	},
	{"P1 Up",			BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"		},
	{"P1 Down",			BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"	},
	{"P1 Left",			BIT_DIGITAL,	DrvJoy1 + 3,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy2 + 0,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy2 + 1,	"p1 fire 2"	},

	{"P2 Coin",			BIT_DIGITAL,	DrvJoy2 + 7,	"p2 coin"	},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy2 + 5,	"p2 start"	},
	{"P2 Up",			BIT_DIGITAL,	DrvJoy1 + 4,	"p2 up"		},
	{"P2 Down",			BIT_DIGITAL,	DrvJoy1 + 5,	"p2 down"	},
	{"P2 Left",			BIT_DIGITAL,	DrvJoy1 + 7,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy1 + 6,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 fire 1"	},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 fire 2"	},

	{"Reset",			BIT_DIGITAL,	NULL,		"reset"		},
	{"Dip A",			BIT_DIPSWITCH,	DrvDips + 0,	"dip"		},
	{"Dip B",			BIT_DIPSWITCH,	DrvDips + 1,	"dip"		},
};

STDINPUTINFO(Tigerh)

static struct BurnDIPInfo TigerhDIPList[]=
{
	{0x11, 0xff, 0xff, 0x6f, NULL							},
	{0x12, 0xff, 0xff, 0xeb, NULL							},

	{0   , 0xfe, 0   ,    7, "Coinage"						},
	{0x11, 0x01, 0x07, 0x02, "3 Coins 1 Credits"			},
	{0x11, 0x01, 0x07, 0x04, "2 Coins 1 Credits"			},
	{0x11, 0x01, 0x07, 0x07, "1 Coin  1 Credits"			},
	{0x11, 0x01, 0x07, 0x03, "2 Coins 3 Credits"			},
	{0x11, 0x01, 0x07, 0x06, "1 Coin  2 Credits"			},
	{0x11, 0x01, 0x07, 0x05, "1 Coin  3 Credits"			},
	{0x11, 0x01, 0x07, 0x00, "Free Play"					},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"					},
	{0x11, 0x01, 0x08, 0x00, "Off"							},
	{0x11, 0x01, 0x08, 0x08, "On"							},

	{0   , 0xfe, 0   ,    2, "Cabinet"						},
	{0x11, 0x01, 0x10, 0x00, "Upright"						},
	{0x11, 0x01, 0x10, 0x10, "Cocktail"						},

	{0   , 0xfe, 0   ,    2, "Flip Screen"					},
	{0x11, 0x01, 0x20, 0x20, "Off"							},
	{0x11, 0x01, 0x20, 0x00, "On"							},

	{0   , 0xfe, 0   ,    2, "Service Mode"					},
	{0x11, 0x01, 0x40, 0x00, "On"							},
	{0x11, 0x01, 0x40, 0x40, "Off"							},

	{0   , 0xfe, 0   ,    2, "Player Speed"					},
	{0x11, 0x01, 0x80, 0x80, "Normal"						},
	{0x11, 0x01, 0x80, 0x00, "Fast"							},

	{0   , 0xfe, 0   ,    4, "Lives"						},
	{0x12, 0x01, 0x03, 0x01, "1"							},
	{0x12, 0x01, 0x03, 0x00, "2"							},
	{0x12, 0x01, 0x03, 0x03, "3"							},
	{0x12, 0x01, 0x03, 0x02, "5"							},

	{0   , 0xfe, 0   ,    4, "Difficulty"					},
	{0x12, 0x01, 0x0c, 0x0c, "Easy"							},
	{0x12, 0x01, 0x0c, 0x08, "Medium"						},
	{0x12, 0x01, 0x0c, 0x04, "Hard"							},
	{0x12, 0x01, 0x0c, 0x00, "Hardest"						},

	{0   , 0xfe, 0   ,    2, "Bonus Life"					},
	{0x12, 0x01, 0x10, 0x10, "20k and every 80k"			},
	{0x12, 0x01, 0x10, 0x00, "50k and every 120k"			},
};

STDDIPINFO(Tigerh)

static struct BurnDIPInfo SlapfighDIPList[]=
{
	{0x11, 0xff, 0xff, 0x7f, NULL							},
	{0x12, 0xff, 0xff, 0xff, NULL							},

	{0   , 0xfe, 0   ,    4, "Coin B"						},
	{0x11, 0x01, 0x03, 0x02, "2 Coins 1 Credits"			},
	{0x11, 0x01, 0x03, 0x03, "1 Coin  1 Credits"			},
	{0x11, 0x01, 0x03, 0x00, "2 Coins 3 Credits"			},
	{0x11, 0x01, 0x03, 0x01, "1 Coin  2 Credits"			},

	{0   , 0xfe, 0   ,    4, "Coin A"						},
	{0x11, 0x01, 0x0c, 0x08, "2 Coins 1 Credits"			},
	{0x11, 0x01, 0x0c, 0x0c, "1 Coin  1 Credits"			},
	{0x11, 0x01, 0x0c, 0x00, "2 Coins 3 Credits"			},
	{0x11, 0x01, 0x0c, 0x04, "1 Coin  2 Credits"			},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"					},
	{0x11, 0x01, 0x10, 0x00, "Off"							},
	{0x11, 0x01, 0x10, 0x10, "On"							},

	{0   , 0xfe, 0   ,    2, "Screen Test"					},
	{0x11, 0x01, 0x20, 0x20, "Off"							},
	{0x11, 0x01, 0x20, 0x00, "On"							},

	{0   , 0xfe, 0   ,    2, "Flip Screen"					},
	{0x11, 0x01, 0x40, 0x40, "Off"							},
	{0x11, 0x01, 0x40, 0x00, "On"							},
			
	{0   , 0xfe, 0   ,    2, "Cabinet"						},
	{0x11, 0x01, 0x80, 0x00, "Upright"						},
	{0x11, 0x01, 0x80, 0x80, "Cocktail"						},

	{0   , 0xfe, 0   ,    2, "Service Mode"					},
	{0x12, 0x01, 0x02, 0x02, "Off"							},
	{0x12, 0x01, 0x02, 0x00, "On"							},

	{0   , 0xfe, 0   ,    4, "Lives"						},
	{0x12, 0x01, 0x0c, 0x08, "1"							},
	{0x12, 0x01, 0x0c, 0x00, "2"							},
	{0x12, 0x01, 0x0c, 0x0c, "3"							},
	{0x12, 0x01, 0x0c, 0x04, "5"							},

	{0   , 0xfe, 0   ,    4, "Bonus Life"					},
	{0x12, 0x01, 0x30, 0x30, "30000 100000"					},
	{0x12, 0x01, 0x30, 0x10, "50000 200000"					},
	{0x12, 0x01, 0x30, 0x20, "50000"						},
	{0x12, 0x01, 0x30, 0x00, "100000"						},

	{0   , 0xfe, 0   ,    4, "Difficulty"					},
	{0x12, 0x01, 0xc0, 0x40, "Easy"							},
	{0x12, 0x01, 0xc0, 0xc0, "Medium"						},
	{0x12, 0x01, 0xc0, 0x80, "Hard"							},
	{0x12, 0x01, 0xc0, 0x00, "Hardest"						},
};

STDDIPINFO(Slapfigh)

static struct BurnRomInfo tigerhb1RomDesc[] = {
	{ "b0.5",         0x004000, 0x6ae7e13c, BRF_ESS | BRF_PRG }, //  0 CPU #0 code
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
};
STD_ROM_PICK(tigerhb1)
STD_ROM_FN(tigerhb1)

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
