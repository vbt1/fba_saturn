#ifndef _D_SG1000_H_
#define _D_SG1000_H_

#include "burnint.h"
#include "eeprom.h"
#include "saturn/ovl.h"

#include "tms9928a.h"
#include "8255ppi.h"
#include "sn76496.h"

extern int file_id;

int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));
static int DrvInit();
static int DrvExit();
static int DrvFrame();

static UINT8 *AllMem	= NULL;
static UINT8 *MemEnd	= NULL;
static UINT8 *AllRam	= NULL;
static UINT8 *RamEnd	= NULL;
static UINT8 *DrvZ80ROM	= NULL;
static UINT8 *DrvZ80Dec = NULL;
static UINT8 *DrvZ80RAM	= NULL;

static UINT8 DrvInputs[2];
static UINT8 DrvJoy1[8];
static UINT8 DrvJoy2[8];
static UINT8 DrvDips[1];
static UINT8 DrvReset = 0;

static struct BurnInputInfo Sg1000InputList[] = {
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"	},

	{"P2 Up",		BIT_DIGITAL,	DrvJoy1 + 6,	"p2 up"		},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy1 + 7,	"p2 down"	},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 fire 1"	},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 fire 2"	},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"		},
};

STDINPUTINFO(Sg1000)

static struct BurnDIPInfo Sg1000DIPList[]=
{
	{0x11, 0xff, 0xff, 0xbf, NULL			},

	{0   , 0xfe, 0   ,    4, "Coinage"		},
	{0x11, 0x01, 0x30, 0x00, "2 Coins 1 Credits"	},
	{0x11, 0x01, 0x30, 0x30, "1 Coin  1 Credits"	},
	{0x11, 0x01, 0x30, 0x20, "1 Coin  2 Credits"	},
	{0x11, 0x01, 0x30, 0x10, "1 Coin  3 Credits"	},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x11, 0x01, 0x40, 0x40, "Off"			},
	{0x11, 0x01, 0x40, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Language"		},
	{0x11, 0x01, 0x80, 0x00, "Japanese"		},
	{0x11, 0x01, 0x80, 0x80, "English"		},
};

STDDIPINFO(Sg1000)

static struct BurnRomInfo sg1k_wboyRomDesc[] = {
	{ "WonderB.sg",	0x08000, 0xe8f0344d, BRF_PRG | BRF_ESS },
};

STD_ROM_PICK(sg1k_wboy)
STD_ROM_FN(sg1k_wboy)

#endif