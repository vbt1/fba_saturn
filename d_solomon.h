#ifndef _D_SOLOMON_H_
#define _D_SOLOMON_H_

#include "burnint.h"
#include "snd/ay8910.h"
#include "saturn/ovl.h"
#include "raze/raze.h"
#include "czet.h"

#define nBurnSoundLen 128

INT32 SolomonInit();
INT32 SolomonExit();
INT32 SolomonFrame();
void SolomonCalcPalette();
void updateSound();
void CZetRunSlave(int *nCycles);
void dummy();

/*static*/ UINT8 SolomonInputPort0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/ UINT8 SolomonInputPort1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/ UINT8 SolomonInputPort2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/ UINT8 SolomonDip[2]        = {0, 0};
/*static*/ UINT8 SolomonInput[3]      = {0x00, 0x00, 0x00};

/*static*/ UINT8 *Mem                 = NULL;
/*static*/ UINT8 *MemEnd              = NULL;
/*static*/ UINT8 *RamStart            = NULL;
/*static*/ UINT8 *SolomonZ80Rom1      = NULL;
/*static*/ UINT8 *SolomonZ80Rom2      = NULL;
/*static*/ UINT8 *SolomonZ80Ram1      = NULL;
/*static*/ UINT8 *SolomonZ80Ram2      = NULL;
/*static*/ UINT8 *SolomonColourRam    = NULL;
/*static*/ UINT8 *SolomonVideoRam     = NULL;
/*static*/ UINT8 *SolomonBgColourRam  = NULL;
/*static*/ UINT8 *SolomonBgVideoRam   = NULL;
/*static*/ UINT8 *SolomonSpriteRam    = NULL;
/*static*/ UINT8 *SolomonPaletteRam   = NULL;
/*static*/ UINT8 *CZ80Context = NULL;
#ifdef USE_IDMA
/*static*/ UINT16 *bgmap_buf = NULL;
/*static*/ UINT16 *bgmap2_buf = NULL;
#endif

UINT16* map_offset_lut      = NULL;
UINT16 *cram_lut = NULL;

/*static*/ INT32 SolomonIrqFire = 0;
/*static*/ INT32 SolomonSoundLatch = 0;
/*static*/ INT16* pFMBuffer = NULL;
/*static*/ INT16* pAY8910Buffer[9] = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

/*static*/ struct BurnInputInfo SolomonInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL  , SolomonInputPort2 + 2, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , SolomonInputPort2 + 0, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , SolomonInputPort2 + 3, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , SolomonInputPort2 + 1, "p2 start"  },

	{"Up"                , BIT_DIGITAL  , SolomonInputPort0 + 2, "p1 up"     },
	{"Down"              , BIT_DIGITAL  , SolomonInputPort0 + 3, "p1 down"   },
	{"Left"              , BIT_DIGITAL  , SolomonInputPort0 + 1, "p1 left"   },
	{"Right"             , BIT_DIGITAL  , SolomonInputPort0 + 0, "p1 right"  },
	{"Fire 1"            , BIT_DIGITAL  , SolomonInputPort0 + 5, "p1 fire 1" },
	{"Fire 2"            , BIT_DIGITAL  , SolomonInputPort0 + 4, "p1 fire 2" },

	{"Up (Cocktail)"     , BIT_DIGITAL  , SolomonInputPort1 + 2, "p2 up"     },
	{"Down (Cocktail)"   , BIT_DIGITAL  , SolomonInputPort1 + 3, "p2 down"   },
	{"Left (Cocktail)"   , BIT_DIGITAL  , SolomonInputPort1 + 1, "p2 left"   },
	{"Right (Cocktail)"  , BIT_DIGITAL  , SolomonInputPort1 + 0, "p2 right"  },
	{"Fire 1 (Cocktail)" , BIT_DIGITAL  , SolomonInputPort1 + 5, "p2 fire 1" },
	{"Fire 2 (Cocktail)" , BIT_DIGITAL  , SolomonInputPort1 + 4, "p2 fire 2" },

	{"Reset"             , BIT_DIGITAL  , NULL        , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH, SolomonDip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, SolomonDip + 1       , "dip"       },
};

STDINPUTINFO(Solomon)

/*static*/ struct BurnDIPInfo SolomonDIPList[]=
{
	// Default Values
	{0x11, 0xff, 0xff, 0x02, NULL                     },
	{0x12, 0xff, 0xff, 0x00, NULL                     },

	// Dip 1
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x11, 0x01, 0x01, 0x01, "Off"                    },
	{0x11, 0x01, 0x01, 0x00, "On"                     },

	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x11, 0x01, 0x02, 0x02, "Upright"                },
	{0x11, 0x01, 0x02, 0x00, "Cocktail"               },

	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x11, 0x01, 0x0c, 0x0c, "2"                      },
	{0x11, 0x01, 0x0c, 0x00, "3"                      },
	{0x11, 0x01, 0x0c, 0x08, "4"                      },
	{0x11, 0x01, 0x0c, 0x04, "5"                      },

	{0   , 0xfe, 0   , 4   , "Coin A"                 },
	{0x11, 0x01, 0xc0, 0x80, "2 Coins 1 Play"         },
	{0x11, 0x01, 0xc0, 0x00, "1 Coin  1 Play"         },
	{0x11, 0x01, 0xc0, 0x40, "1 Coin  2 Play"         },
	{0x11, 0x01, 0xc0, 0xc0, "1 Coin  3 Play"         },

	{0   , 0xfe, 0   , 4   , "Coin B"                 },
	{0x11, 0x01, 0x30, 0x20, "2 Coins 1 Play"         },
	{0x11, 0x01, 0x30, 0x00, "1 Coin  1 Play"         },
	{0x11, 0x01, 0x30, 0x10, "1 Coin  2 Play"         },
	{0x11, 0x01, 0x30, 0x30, "1 Coin  3 Play"         },

	// Dip 2
	{0   , 0xfe, 0   , 4   , "Difficulty"             },
	{0x12, 0x01, 0x03, 0x02, "Easy"                   },
	{0x12, 0x01, 0x03, 0x00, "Normal"                 },
	{0x12, 0x01, 0x03, 0x01, "Harder"                 },
	{0x12, 0x01, 0x03, 0x03, "Difficult"              },

	{0   , 0xfe, 0   , 4   , "Timer Speed"            },
	{0x12, 0x01, 0x0c, 0x08, "Slow"                   },
	{0x12, 0x01, 0x0c, 0x00, "Normal"                 },
	{0x12, 0x01, 0x0c, 0x04, "Faster"                 },
	{0x12, 0x01, 0x0c, 0x0c, "Fastest"                },

	{0   , 0xfe, 0   , 2   , "Extra"                  },
	{0x12, 0x01, 0x10, 0x00, "Normal"                 },
	{0x12, 0x01, 0x10, 0x10, "Difficult"              },

	{0   , 0xfe, 0   , 8   , "Bonus Life"             },
	{0x12, 0x01, 0xe0, 0x00, "30k  200k 500k"         },
	{0x12, 0x01, 0xe0, 0x80, "100k 300k 800k"         },
	{0x12, 0x01, 0xe0, 0x40, "30k  200k"              },
	{0x12, 0x01, 0xe0, 0xc0, "100k 300k"              },
	{0x12, 0x01, 0xe0, 0x20, "30k"                    },
	{0x12, 0x01, 0xe0, 0xa0, "100k"                   },
	{0x12, 0x01, 0xe0, 0x60, "200k"                   },
	{0x12, 0x01, 0xe0, 0xe0, "None"                   },
};

STDDIPINFO(Solomon)

/*static*/ struct BurnRomInfo SolomonRomDesc[] = {
	{ "6.3f",          0x04000, 0x645eb0f3, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "7.3h",          0x08000, 0x1bf5c482, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "8.3jk",         0x08000, 0x0a6cdefc, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code

	{ "1.3jk",         0x04000, 0xfa6e562e, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program Code

	{ "12.3t",         0x08000, 0xb371291c, BRF_GRA },			 //  4	Characters
	{ "11.3r",         0x08000, 0x6f94d2af, BRF_GRA },			 //  5	Characters
	{ "10.3p",         0x08000, 0x8310c2a1, BRF_GRA },			 //  6	Characters
	{ "9.3m",          0x08000, 0xab7e6c42, BRF_GRA },			 //  7	Characters
	{ "2.5lm",         0x04000, 0x80fa2be3, BRF_GRA },			 //  8	Sprites
	{ "3.6lm",         0x04000, 0x236106b4, BRF_GRA },			 //  9	Sprites
	{ "4.7lm",         0x04000, 0x088fe5d9, BRF_GRA },			 //  10	Sprites
	{ "5.8lm",         0x04000, 0x8366232a, BRF_GRA },			 //  11	Sprites
};

STD_ROM_PICK(Solomon)
STD_ROM_FN(Solomon)

#endif
