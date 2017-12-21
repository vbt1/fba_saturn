#ifndef _D_VIGILANT_H_
#define _D_VIGILANT_H_

#include "burnint.h"
#include "burn_ym2151.h"
#include "snd/dac.h"
#include "sega_spr.h"
#include "sega_scl2.h"
#include "sega_pcm.h"
#include "saturn/ovl.h"

PcmHn 			pcm8[8];
#define	PCM_ADDR	((void*)0x25a20000)
#define	PCM_SIZE	(4096L*2)				/* 2.. */
#define SOUNDRATE   7680L //
#define nBurnSoundLen 140 //128//7680/55.0
unsigned int vbmap[4][0x1000];
int bg=-1;

int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));
int DrvInit();
int DrvExit();
void DrvFrame();
void CZetRunSlave(int *nCycles);

/*static*/unsigned char DrvInputPort0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/unsigned char DrvInputPort1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/unsigned char DrvInputPort2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/unsigned char DrvDip[3]        = {0, 0, 0};
/*static*/unsigned char DrvInput[3]      = {0x00, 0x00, 0x00};
/*static*/unsigned char DrvReset         = 0;

/*static*/unsigned char *Mem                 = NULL;
/*static*/unsigned char *MemEnd              = NULL;
/*static*/unsigned char *RamStart            = NULL;
/*static*/unsigned char *RamEnd              = NULL;
/*static*/unsigned char *DrvZ80Rom1          = NULL;
/*static*/unsigned char *DrvZ80Rom2          = NULL;
/*static*/unsigned char *DrvZ80Ram1          = NULL;
/*static*/unsigned char *DrvZ80Ram2          = NULL;
/*static*/unsigned char *DrvVideoRam         = NULL;
/*static*/unsigned char *DrvSpriteRam        = NULL;
/*static*/unsigned char *DrvPaletteRam       = NULL;
/*static*/unsigned char *DrvChars            = NULL;
/*static*/unsigned char *DrvBackTiles        = NULL;
/*static*/unsigned char *DrvSprites          = NULL;
/*static*/unsigned char *DrvSamples          = NULL;
/*static*/unsigned char *DrvTempRom          = NULL;
/*static*/unsigned int  *DrvPalette          = NULL;
unsigned char *CZ80Context          = NULL;

/*static*/unsigned char DrvRomBank;
/*static*/unsigned char DrvSoundLatch;
/*static*/unsigned char DrvIrqVector;

/*static*/int DrvRearColour;
/*static*/int DrvRearDisable;
/*static*/int DrvHorizScrollLo;
/*static*/int DrvHorizScrollHi;
/*static*/int DrvRearHorizScrollLo;
/*static*/int DrvRearHorizScrollHi;
/*static*/int DrvSampleAddress;

/*static*/int nCyclesDone[2], nCyclesTotal[2];
/*static*///int nCyclesSegment;

/*static*/unsigned char DrvHasYM2203 = 0;
/*static*/unsigned char DrvKikcubicDraw = 0;

#define VECTOR_INIT		0
#define YM2151_ASSERT		1
#define YM2151_CLEAR		2
#define Z80_ASSERT		3
#define Z80_CLEAR		4

/*static*/struct BurnInputInfo DrvInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL  , DrvInputPort0 + 3, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , DrvInputPort0 + 0, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , DrvInputPort2 + 4, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , DrvInputPort0 + 1, "p2 start"  },

	{"Up"                , BIT_DIGITAL  , DrvInputPort1 + 3, "p1 up"     },
	{"Down"              , BIT_DIGITAL  , DrvInputPort1 + 2, "p1 down"   },
	{"Left"              , BIT_DIGITAL  , DrvInputPort1 + 1, "p1 left"   },
	{"Right"             , BIT_DIGITAL  , DrvInputPort1 + 0, "p1 right"  },
	{"Fire 1"            , BIT_DIGITAL  , DrvInputPort1 + 5, "p1 fire 1" },
	{"Fire 2"            , BIT_DIGITAL  , DrvInputPort1 + 7, "p1 fire 2" },
	
	{"Up (Cocktail)"     , BIT_DIGITAL  , DrvInputPort2 + 3, "p2 up"     },
	{"Down (Cocktail)"   , BIT_DIGITAL  , DrvInputPort2 + 2, "p2 down"   },
	{"Left (Cocktail)"   , BIT_DIGITAL  , DrvInputPort2 + 1, "p2 left"   },
	{"Right (Cocktail)"  , BIT_DIGITAL  , DrvInputPort2 + 0, "p2 right"  },
	{"Fire 1 (Cocktail)" , BIT_DIGITAL  , DrvInputPort2 + 5, "p2 fire 1" },
	{"Fire 2 (Cocktail)" , BIT_DIGITAL  , DrvInputPort2 + 7, "p2 fire 2" },

	{"Reset"             , BIT_DIGITAL  , &DrvReset        , "reset"     },
	{"Service"           , BIT_DIGITAL  , DrvInputPort0 + 2, "service"   },
	{"Dip 1"             , BIT_DIPSWITCH, DrvDip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, DrvDip + 1       , "dip"       },
};

STDINPUTINFO(Drv)


/*static*/inline void DrvClearOpposites(unsigned char* nJoystickInputs)
{
	if ((*nJoystickInputs & 0x03) == 0x03) {
		*nJoystickInputs &= ~0x03;
	}
	if ((*nJoystickInputs & 0x0c) == 0x0c) {
		*nJoystickInputs &= ~0x0c;
	}
}

/*static*/inline void DrvMakeInputs()
{
	// Reset Inputs
	DrvInput[0] = DrvInput[1] = DrvInput[2] = 0x00;
	unsigned int i;
	// Compile Digital Inputs
	for (i = 0; i < 8; i++) {
		DrvInput[0] |= (DrvInputPort0[i] & 1) << i;
		DrvInput[1] |= (DrvInputPort1[i] & 1) << i;
		DrvInput[2] |= (DrvInputPort2[i] & 1) << i;
	}

	// Clear Opposites
	DrvClearOpposites(&DrvInput[1]);
	DrvClearOpposites(&DrvInput[2]);
}

/*static*/struct BurnDIPInfo DrvDIPList[]=
{
	// Default Values
	{0x12, 0xff, 0xff, 0xff, NULL                     },
	{0x13, 0xff, 0xff, 0xfd, NULL                     },
	
	// Dip 1
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x12, 0x01, 0x03, 0x02, "2"                      },
	{0x12, 0x01, 0x03, 0x03, "3"                      },
	{0x12, 0x01, 0x03, 0x01, "4"                      },
	{0x12, 0x01, 0x03, 0x00, "5"                      },
	
	{0   , 0xfe, 0   , 2   , "Difficulty"             },
	{0x12, 0x01, 0x04, 0x04, "Normal"                 },
	{0x12, 0x01, 0x04, 0x00, "Hard"                   },
	
	{0   , 0xfe, 0   , 2   , "Energy Decrease Rate"   },
	{0x12, 0x01, 0x08, 0x08, "Slow"                   },
	{0x12, 0x01, 0x08, 0x00, "Fast"                   },
	
	{0   , 0xfe, 0   , 16  , "Coinage"                },
	{0x12, 0x01, 0xf0, 0xa0, "6 Coins 1 Play"         },
	{0x12, 0x01, 0xf0, 0xb0, "5 Coins 1 Play"         },
	{0x12, 0x01, 0xf0, 0xc0, "4 Coins 1 Play"         },
	{0x12, 0x01, 0xf0, 0xd0, "3 Coins 1 Play"         },
	{0x12, 0x01, 0xf0, 0x10, "8 Coins 3 Plays"        },
	{0x12, 0x01, 0xf0, 0xe0, "2 Coins 1 Play"         },
	{0x12, 0x01, 0xf0, 0x20, "5 Coins 3 Plays"        },
	{0x12, 0x01, 0xf0, 0x30, "3 Coins 2 Plays"        },
	{0x12, 0x01, 0xf0, 0xf0, "1 Coin  1 Play"         },
	{0x12, 0x01, 0xf0, 0x40, "2 Coins 3 Plays"        },
	{0x12, 0x01, 0xf0, 0x90, "1 Coin  2 Plays"        },
	{0x12, 0x01, 0xf0, 0x80, "1 Coin  3 Plays"        },
	{0x12, 0x01, 0xf0, 0x70, "1 Coin  4 Plays"        },
	{0x12, 0x01, 0xf0, 0x60, "1 Coin  5 Plays"        },
	{0x12, 0x01, 0xf0, 0x50, "1 Coin  6 Plays"        },
	{0x12, 0x01, 0xf0, 0x00, "Freeplay"               },
	
	// Dip 2
	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x13, 0x01, 0x01, 0x01, "Off"                    },
	{0x13, 0x01, 0x01, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x13, 0x01, 0x02, 0x00, "Upright"                },
	{0x13, 0x01, 0x02, 0x02, "Cocktail"               },
	
	{0   , 0xfe, 0   , 2   , "Coin Mode"              },
	{0x13, 0x01, 0x04, 0x04, "Mode 1"                 },
	{0x13, 0x01, 0x04, 0x00, "Mode 2"                 },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x13, 0x01, 0x08, 0x00, "Off"                    },
	{0x13, 0x01, 0x08, 0x08, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x13, 0x01, 0x10, 0x00, "Off"                    },
	{0x13, 0x01, 0x10, 0x10, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Stop Mode"              },
	{0x13, 0x01, 0x20, 0x20, "Off"                    },
	{0x13, 0x01, 0x20, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Invulnerability"        },
	{0x13, 0x01, 0x40, 0x40, "Off"                    },
	{0x13, 0x01, 0x40, 0x00, "On"                     },
};

STDDIPINFO(Drv)

/*static*/struct BurnRomInfo DrvRomDesc[] = {
	{ "g07c03.bin",   0x08000, 0x9dcca081, BRF_ESS | BRF_PRG },	//  0	Z80 #1 Program Code
	{ "j07c04.bin",   0x10000, 0xe0159105, BRF_ESS | BRF_PRG },	//  1
	
	{ "g05c02.bin",   0x10000, 0x10582b2d, BRF_ESS | BRF_PRG },	//  2	Z80 #2 Program 
	
	{ "f05c08.bin",   0x10000, 0x01579d20, BRF_GRA },		//  3	Characters
	{ "h05c09.bin",   0x10000, 0x4f5872f0, BRF_GRA },		//  4
	
	{ "n07c12.bin",   0x10000, 0x10af8eb2, BRF_GRA },		//  5	Sprites
	{ "k07c10.bin",   0x10000, 0x9576f304, BRF_GRA },		//  6
	{ "o07c13.bin",   0x10000, 0xb1d9d4dc, BRF_GRA },		//  7
	{ "l07c11.bin",   0x10000, 0x4598be4a, BRF_GRA },		//  8
	{ "t07c16.bin",   0x10000, 0xf5425e42, BRF_GRA },		//  9
	{ "p07c14.bin",   0x10000, 0xcb50a17c, BRF_GRA },		//  10
	{ "v07c17.bin",   0x10000, 0x959ba3c7, BRF_GRA },		//  11
	{ "s07c15.bin",   0x10000, 0x7f2e91c5, BRF_GRA },		//  12
	
	{ "d01c05.bin",   0x10000, 0x81b1ee5c, BRF_GRA },		//  13	Background
	{ "e01c06.bin",   0x10000, 0xd0d33673, BRF_GRA },		//  14
	{ "f01c07.bin",   0x10000, 0xaae81695, BRF_GRA },		//  15
	
	{ "d04c01.bin",   0x10000, 0x9b85101d, BRF_SND },		//  16	Samples
	
	{ "pal16l8.8r",    0x00104, 0x00000000, BRF_GRA | BRF_NODUMP },	//  17	PALs
	{ "pal16l8.4m",    0x00104, 0x6c628a26, BRF_GRA },		//  18
	{ "pal16l8.1b",    0x00104, 0x00000000, BRF_GRA | BRF_NODUMP },	//  19
};

STD_ROM_PICK(Drv)
STD_ROM_FN(Drv)

#endif