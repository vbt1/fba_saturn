#ifndef _D_1943_H_
#define _D_1943_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "snd/sn76496.h"
#include "snd/msm5205.h"
#include "czet.h"
#define SOUND_LEN 128
/*static*/ INT32 DrvInit();
/*static*/ INT32 DrvbInit();
/*static*/ INT32 DrvExit();
/*static*/ INT32 DrvFrame();
/*static*/ void DrvDraw();
void tile32x32toSaturn (unsigned char reverse, unsigned int num, unsigned char *pDest);

UINT8 DrvInputPort0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
UINT8 DrvInputPort1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
UINT8 DrvInputPort2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
UINT8 DrvDip[2]        = {0, 0};
UINT8 DrvInput[3]      = {0x00, 0x00, 0x00};
UINT8 DrvReset         = 0;

UINT8 *CZ80Context			= NULL;
UINT16 *map_offset_lut_fg	= NULL;
UINT16 *bgmap_lut           = NULL;
UINT32 *bgmap_buf			= NULL;
UINT8 *Mem                  = NULL;
UINT8 *MemEnd               = NULL;
UINT8 *RamStart             = NULL;
UINT8 *RamEnd               = NULL;
UINT8 *DrvZ80Rom1           = NULL;
UINT8 *DrvZ80Rom2           = NULL;
UINT8 *DrvZ80Ram1           = NULL;
UINT8 *DrvZ80Ram2           = NULL;
UINT8 *DrvVideoRam          = NULL;
UINT8 *DrvPaletteRam        = NULL;
UINT8 *DrvSpriteRam         = NULL;
UINT8 *DrvPromRed           = NULL;
UINT8 *DrvPromGreen         = NULL;
UINT8 *DrvPromBlue          = NULL;
UINT8 *DrvPromCharLookup    = NULL;
UINT8 *DrvPromBg2Lookup     = NULL;
UINT8 *DrvPromBg2PalBank    = NULL;
UINT8 *DrvPromBgLookup      = NULL;
UINT8 *DrvPromBgPalBank     = NULL;
UINT8 *DrvPromSpriteLookup  = NULL;
UINT8 *DrvPromSpritePalBank = NULL;
UINT8 *DrvBgTilemap         = NULL;
UINT8 *DrvBg2Tilemap        = NULL;
UINT8 *DrvChars             = NULL;
UINT8 *DrvBg2Tiles          = NULL;
UINT8 *DrvBgTiles           = NULL;
UINT8 *DrvSprites           = NULL;
UINT8 *DrvTempRom           = NULL;
UINT16 *DrvPalette          = NULL;

UINT8 DrvRecalc;

UINT8 DrvSoundLatch;
UINT8 DrvRomBank;
UINT8 DrvBg2ScrollX[2];
UINT8 DrvBgScrollX[2];
UINT8 DrvBgScrollY;
UINT8 DrvBg2On;
UINT8 DrvBg1On;
UINT8 DrvSpritesOn;
UINT8 DrvCharsOn;
UINT8 DrvProtValue;

INT32 bootleg = 0;

struct BurnInputInfo DrvInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL  , DrvInputPort0 + 6, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , DrvInputPort0 + 0, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , DrvInputPort0 + 7, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , DrvInputPort0 + 1, "p2 start"  },

	{"P1 Up"             , BIT_DIGITAL  , DrvInputPort1 + 3, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL  , DrvInputPort1 + 2, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL  , DrvInputPort1 + 1, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , DrvInputPort1 + 0, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , DrvInputPort1 + 4, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , DrvInputPort1 + 5, "p1 fire 2" },
	
	{"P2 Up"             , BIT_DIGITAL  , DrvInputPort2 + 3, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL  , DrvInputPort2 + 2, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL  , DrvInputPort2 + 1, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , DrvInputPort2 + 0, "p2 right"  },
	{"Fire 1"            , BIT_DIGITAL  , DrvInputPort2 + 4, "p2 fire 1" },
	{"Fire 2"            , BIT_DIGITAL  , DrvInputPort2 + 5, "p2 fire 2" },

	{"Reset"             , BIT_DIGITAL  , &DrvReset        , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH, DrvDip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, DrvDip + 1       , "dip"       },
};

STDINPUTINFO(Drv)

struct BurnDIPInfo DrvDIPList[]=
{
	// Default Values
	{0x11, 0xff, 0xff, 0xf8, NULL                     },
	{0x12, 0xff, 0xff, 0xff, NULL                     },

	// Dip 1
	{0   , 0xfe, 0   , 16  , "Difficulty"             },
	{0x11, 0x01, 0x0f, 0x0f, "1 (Easy)"               },
	{0x11, 0x01, 0x0f, 0x0e, "2"                      },
	{0x11, 0x01, 0x0f, 0x0d, "3"                      },
	{0x11, 0x01, 0x0f, 0x0c, "4"                      },
	{0x11, 0x01, 0x0f, 0x0b, "5"                      },
	{0x11, 0x01, 0x0f, 0x0a, "6"                      },
	{0x11, 0x01, 0x0f, 0x09, "7"                      },
	{0x11, 0x01, 0x0f, 0x08, "8 (Normal)"             },
	{0x11, 0x01, 0x0f, 0x07, "9"                      },
	{0x11, 0x01, 0x0f, 0x06, "10"                     },
	{0x11, 0x01, 0x0f, 0x05, "11"                     },
	{0x11, 0x01, 0x0f, 0x04, "12"                     },
	{0x11, 0x01, 0x0f, 0x03, "13"                     },
	{0x11, 0x01, 0x0f, 0x02, "14"                     },
	{0x11, 0x01, 0x0f, 0x01, "15"                     },
	{0x11, 0x01, 0x0f, 0x00, "16 (Difficult)"         },

	{0   , 0xfe, 0   , 2   , "2 Player Game"          },
	{0x11, 0x01, 0x10, 0x00, "1 Credit / 2 Players"   },
	{0x11, 0x01, 0x10, 0x10, "2 Credits / 2 Players"  },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x11, 0x01, 0x20, 0x20, "Off"                    },
	{0x11, 0x01, 0x20, 0x00, "On"                     },

	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x11, 0x01, 0x40, 0x40, "Off"                    },
	{0x11, 0x01, 0x40, 0x00, "On"                     },

	{0   , 0xfe, 0   , 2   , "Service Mode"           },
	{0x11, 0x01, 0x80, 0x80, "Off"                    },
	{0x11, 0x01, 0x80, 0x00, "On"                     },

	// Dip 2
	{0   , 0xfe, 0   , 8   , "Coin A"                 },
	{0x12, 0x01, 0x07, 0x00, "4 Coins 1 Play"         },
	{0x12, 0x01, 0x07, 0x01, "3 Coins 1 Play"         },
	{0x12, 0x01, 0x07, 0x02, "2 Coins 1 Play"         },
	{0x12, 0x01, 0x07, 0x07, "1 Coin 1 Play"          },
	{0x12, 0x01, 0x07, 0x06, "1 Coin 2 Plays"         },
	{0x12, 0x01, 0x07, 0x05, "1 Coin 3 Plays"         },
	{0x12, 0x01, 0x07, 0x04, "1 Coin 4 Plays"         },
	{0x12, 0x01, 0x07, 0x03, "1 Coin 5 Plays"         },

	{0   , 0xfe, 0   , 8   , "Coin B"                 },
	{0x12, 0x01, 0x38, 0x00, "4 Coins 1 Play"         },
	{0x12, 0x01, 0x38, 0x08, "3 Coins 1 Play"         },
	{0x12, 0x01, 0x38, 0x10, "2 Coins 1 Play"         },
	{0x12, 0x01, 0x38, 0x38, "1 Coin 1 Play"          },
	{0x12, 0x01, 0x38, 0x30, "1 Coin 2 Plays"         },
	{0x12, 0x01, 0x38, 0x28, "1 Coin 3 Plays"         },
	{0x12, 0x01, 0x38, 0x20, "1 Coin 4 Plays"         },
	{0x12, 0x01, 0x38, 0x18, "1 Coin 5 Plays"         },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x12, 0x01, 0x40, 0x00, "No"                     },
	{0x12, 0x01, 0x40, 0x40, "Yes"                    },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x12, 0x01, 0x80, 0x00, "Off"                    },
	{0x12, 0x01, 0x80, 0x80, "On"                     },
};

STDDIPINFO(Drv)

struct BurnRomInfo DrvRomDesc[] = {
	{ "bme01.12d",     0x08000, 0x55fd447e, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "bme02.13d",     0x10000, 0x073fc57c, BRF_ESS | BRF_PRG }, //	 1
	{ "bme03.14d",     0x10000, 0x835822c2, BRF_ESS | BRF_PRG }, //	 2
	
	{ "bm05.4k",       0x08000, 0xee2bd2d7, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program 

	{ "bm04.5h",       0x08000, 0x46cb9d3d, BRF_GRA },	     	 //  4	Characters
	
	{ "bm15.10f",      0x08000, 0x6b1a0443, BRF_GRA },	     	 //  5	BG Tiles
	{ "bm16.11f",      0x08000, 0x23c908c2, BRF_GRA },	     	 //  6
	{ "bm17.12f",      0x08000, 0x46bcdd07, BRF_GRA },	     	 //  7
	{ "bm18.14f",      0x08000, 0xe6ae7ba0, BRF_GRA },	     	 //  8
	{ "bm19.10j",      0x08000, 0x868ababc, BRF_GRA },	     	 //  9
	{ "bm20.11j",      0x08000, 0x0917e5d4, BRF_GRA },	     	 //  10
	{ "bm21.12j",      0x08000, 0x9bfb0d89, BRF_GRA },	     	 //  11
	{ "bm22.14j",      0x08000, 0x04f3c274, BRF_GRA },	     	 //  12
	
	{ "bm24.14k",      0x08000, 0x11134036, BRF_GRA },	     	 //  13	BG2 Tiles
	{ "bm25.14l",      0x08000, 0x092cf9c1, BRF_GRA },	     	 //  14
	
	{ "bm06.10a",      0x08000, 0x97acc8af, BRF_GRA },	     	 //  15	Sprites
	{ "bm07.11a",      0x08000, 0xd78f7197, BRF_GRA },	     	 //  16
	{ "bm08.12a",      0x08000, 0x1a626608, BRF_GRA },	     	 //  17
	{ "bm09.14a",      0x08000, 0x92408400, BRF_GRA },	     	 //  18
	{ "bm10.10c",      0x08000, 0x8438a44a, BRF_GRA },	     	 //  19
	{ "bm11.11c",      0x08000, 0x6c69351d, BRF_GRA },	     	 //  20
	{ "bm12.12c",      0x08000, 0x5e7efdb7, BRF_GRA },	     	 //  21
	{ "bm13.14c",      0x08000, 0x1143829a, BRF_GRA },	     	 //  22
		
	{ "bm14.5f",       0x08000, 0x4d3c6401, BRF_GRA },	     	 //  23	Tilemaps
	{ "bm23.8k",       0x08000, 0xa52aecbd, BRF_GRA },	     	 //  24
	
	{ "bm1.12a",       0x00100, 0x74421f18, BRF_GRA },	     	 //  25	PROMs
	{ "bm2.13a",       0x00100, 0xac27541f, BRF_GRA },	     	 //  26
	{ "bm3.14a",       0x00100, 0x251fb6ff, BRF_GRA },	     	 //  27
	{ "bm5.7f",        0x00100, 0x206713d0, BRF_GRA },	     	 //  28
	{ "bm10.7l",       0x00100, 0x33c2491c, BRF_GRA },	     	 //  29
	{ "bm9.6l",        0x00100, 0xaeea4af7, BRF_GRA },	     	 //  30
	{ "bm12.12m",      0x00100, 0xc18aa136, BRF_GRA },	     	 //  31
	{ "bm11.12l",      0x00100, 0x405aae37, BRF_GRA },	     	 //  32
	{ "bm8.8c",        0x00100, 0xc2010a9e, BRF_GRA },	     	 //  33
	{ "bm7.7c",        0x00100, 0xb56f30c3, BRF_GRA },	     	 //  34
	{ "bm4.12c",       0x00100, 0x91a8a2e1, BRF_GRA },	     	 //  35
	{ "bm6.4b",        0x00100, 0x0eaf5158, BRF_GRA },	     	 //  36
	
	{ "bm.7k",         0x01000, 0xcf4781bf, BRF_OPT | BRF_PRG }, //  37	MCU
};

STD_ROM_PICK(Drv)
STD_ROM_FN(Drv)

struct BurnRomInfo DrvuRomDesc[] = {
	{ "bmu01c.12d",    0x08000, 0xc686cc5c, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "bmu02c.13d",    0x10000, 0xd8880a41, BRF_ESS | BRF_PRG }, //	 1
	{ "bmu03c.14d",    0x10000, 0x3f0ee26c, BRF_ESS | BRF_PRG }, //	 2
	
	{ "bm05.4k",       0x08000, 0xee2bd2d7, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program 

	{ "bm04.5h",       0x08000, 0x46cb9d3d, BRF_GRA },	     	 //  4	Characters
	
	{ "bm15.10f",      0x08000, 0x6b1a0443, BRF_GRA },	     	 //  5	BG Tiles
	{ "bm16.11f",      0x08000, 0x23c908c2, BRF_GRA },	     	 //  6
	{ "bm17.12f",      0x08000, 0x46bcdd07, BRF_GRA },	     	 //  7
	{ "bm18.14f",      0x08000, 0xe6ae7ba0, BRF_GRA },	     	 //  8
	{ "bm19.10j",      0x08000, 0x868ababc, BRF_GRA },	     	 //  9
	{ "bm20.11j",      0x08000, 0x0917e5d4, BRF_GRA },	     	 //  10
	{ "bm21.12j",      0x08000, 0x9bfb0d89, BRF_GRA },	     	 //  11
	{ "bm22.14j",      0x08000, 0x04f3c274, BRF_GRA },	     	 //  12
	
	{ "bm24.14k",      0x08000, 0x11134036, BRF_GRA },	     	 //  13	BG2 Tiles
	{ "bm25.14l",      0x08000, 0x092cf9c1, BRF_GRA },	     	 //  14
	
	{ "bm06.10a",      0x08000, 0x97acc8af, BRF_GRA },	     	 //  15	Sprites
	{ "bm07.11a",      0x08000, 0xd78f7197, BRF_GRA },	     	 //  16
	{ "bm08.12a",      0x08000, 0x1a626608, BRF_GRA },	     	 //  17
	{ "bm09.14a",      0x08000, 0x92408400, BRF_GRA },	     	 //  18
	{ "bm10.10c",      0x08000, 0x8438a44a, BRF_GRA },	     	 //  19
	{ "bm11.11c",      0x08000, 0x6c69351d, BRF_GRA },	     	 //  20
	{ "bm12.12c",      0x08000, 0x5e7efdb7, BRF_GRA },	     	 //  21
	{ "bm13.14c",      0x08000, 0x1143829a, BRF_GRA },	     	 //  22
		
	{ "bm14.5f",       0x08000, 0x4d3c6401, BRF_GRA },	     	 //  23	Tilemaps
	{ "bm23.8k",       0x08000, 0xa52aecbd, BRF_GRA },	     	 //  24
	
	{ "bm1.12a",       0x00100, 0x74421f18, BRF_GRA },	     	 //  25	PROMs
	{ "bm2.13a",       0x00100, 0xac27541f, BRF_GRA },	     	 //  26
	{ "bm3.14a",       0x00100, 0x251fb6ff, BRF_GRA },	     	 //  27
	{ "bm5.7f",        0x00100, 0x206713d0, BRF_GRA },	     	 //  28
	{ "bm10.7l",       0x00100, 0x33c2491c, BRF_GRA },	     	 //  29
	{ "bm9.6l",        0x00100, 0xaeea4af7, BRF_GRA },	     	 //  30
	{ "bm12.12m",      0x00100, 0xc18aa136, BRF_GRA },	     	 //  31
	{ "bm11.12l",      0x00100, 0x405aae37, BRF_GRA },	     	 //  32
	{ "bm8.8c",        0x00100, 0xc2010a9e, BRF_GRA },	     	 //  33
	{ "bm7.7c",        0x00100, 0xb56f30c3, BRF_GRA },	     	 //  34
	{ "bm4.12c",       0x00100, 0x91a8a2e1, BRF_GRA },	     	 //  35
	{ "bm6.4b",        0x00100, 0x0eaf5158, BRF_GRA },	     	 //  36
	
	{ "bm.7k",         0x01000, 0xcf4781bf, BRF_OPT | BRF_PRG }, //  37	MCU
};

STD_ROM_PICK(Drvu)
STD_ROM_FN(Drvu)

struct BurnRomInfo DrvuaRomDesc[] = {
	{ "bmu01.12d",     0x08000, 0x793cf15f, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "bmu02.13d",     0x10000, 0x6f1353d5, BRF_ESS | BRF_PRG }, //	 1
	{ "bmu03.14d",     0x10000, 0x9e7c07f7, BRF_ESS | BRF_PRG }, //	 2
	
	{ "bm05.4k",       0x08000, 0xee2bd2d7, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program 

	{ "bm04.5h",       0x08000, 0x46cb9d3d, BRF_GRA },	     	 //  4	Characters
	
	{ "bm15.10f",      0x08000, 0x6b1a0443, BRF_GRA },	     	 //  5	BG Tiles
	{ "bm16.11f",      0x08000, 0x23c908c2, BRF_GRA },	     	 //  6
	{ "bm17.12f",      0x08000, 0x46bcdd07, BRF_GRA },	     	 //  7
	{ "bm18.14f",      0x08000, 0xe6ae7ba0, BRF_GRA },	     	 //  8
	{ "bm19.10j",      0x08000, 0x868ababc, BRF_GRA },	     	 //  9
	{ "bm20.11j",      0x08000, 0x0917e5d4, BRF_GRA },	     	 //  10
	{ "bm21.12j",      0x08000, 0x9bfb0d89, BRF_GRA },	     	 //  11
	{ "bm22.14j",      0x08000, 0x04f3c274, BRF_GRA },	     	 //  12
	
	{ "bm24.14k",      0x08000, 0x11134036, BRF_GRA },	     	 //  13	BG2 Tiles
	{ "bm25.14l",      0x08000, 0x092cf9c1, BRF_GRA },	     	 //  14
	
	{ "bm06.10a",      0x08000, 0x97acc8af, BRF_GRA },	     	 //  15	Sprites
	{ "bm07.11a",      0x08000, 0xd78f7197, BRF_GRA },	     	 //  16
	{ "bm08.12a",      0x08000, 0x1a626608, BRF_GRA },	     	 //  17
	{ "bm09.14a",      0x08000, 0x92408400, BRF_GRA },	     	 //  18
	{ "bm10.10c",      0x08000, 0x8438a44a, BRF_GRA },	     	 //  19
	{ "bm11.11c",      0x08000, 0x6c69351d, BRF_GRA },	     	 //  20
	{ "bm12.12c",      0x08000, 0x5e7efdb7, BRF_GRA },	     	 //  21
	{ "bm13.14c",      0x08000, 0x1143829a, BRF_GRA },	     	 //  22
		
	{ "bm14.5f",       0x08000, 0x4d3c6401, BRF_GRA },	     	 //  23	Tilemaps
	{ "bm23.8k",       0x08000, 0xa52aecbd, BRF_GRA },	     	 //  24
	
	{ "bm1.12a",       0x00100, 0x74421f18, BRF_GRA },	     	 //  25	PROMs
	{ "bm2.13a",       0x00100, 0xac27541f, BRF_GRA },	     	 //  26
	{ "bm3.14a",       0x00100, 0x251fb6ff, BRF_GRA },	     	 //  27
	{ "bm5.7f",        0x00100, 0x206713d0, BRF_GRA },	     	 //  28
	{ "bm10.7l",       0x00100, 0x33c2491c, BRF_GRA },	     	 //  29
	{ "bm9.6l",        0x00100, 0xaeea4af7, BRF_GRA },	     	 //  30
	{ "bm12.12m",      0x00100, 0xc18aa136, BRF_GRA },	     	 //  31
	{ "bm11.12l",      0x00100, 0x405aae37, BRF_GRA },	     	 //  32
	{ "bm8.8c",        0x00100, 0xc2010a9e, BRF_GRA },	     	 //  33
	{ "bm7.7c",        0x00100, 0xb56f30c3, BRF_GRA },	     	 //  34
	{ "bm4.12c",       0x00100, 0x91a8a2e1, BRF_GRA },	     	 //  35
	{ "bm6.4b",        0x00100, 0x0eaf5158, BRF_GRA },	     	 //  36
	
	{ "bm.7k",         0x01000, 0xcf4781bf, BRF_OPT | BRF_PRG }, //  37	MCU
};

STD_ROM_PICK(Drvua)
STD_ROM_FN(Drvua)

struct BurnRomInfo DrvjRomDesc[] = {
	{ "bm01b.12d",     0x08000, 0x363f9f3d, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "bm02b.13d",     0x10000, 0x7f0d7edc, BRF_ESS | BRF_PRG }, //	 1
	{ "bm03b.14d",     0x10000, 0x7093da2a, BRF_ESS | BRF_PRG }, //	 2
	
	{ "bm05.4k",       0x08000, 0xee2bd2d7, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program 

	{ "bm04.5h",       0x08000, 0x46cb9d3d, BRF_GRA },	     	 //  4	Characters
	
	{ "bm15.10f",      0x08000, 0x6b1a0443, BRF_GRA },	     	 //  5	BG Tiles
	{ "bm16.11f",      0x08000, 0x23c908c2, BRF_GRA },	     	 //  6
	{ "bm17.12f",      0x08000, 0x46bcdd07, BRF_GRA },	     	 //  7
	{ "bm18.14f",      0x08000, 0xe6ae7ba0, BRF_GRA },	     	 //  8
	{ "bm19.10j",      0x08000, 0x868ababc, BRF_GRA },	     	 //  9
	{ "bm20.11j",      0x08000, 0x0917e5d4, BRF_GRA },	     	 //  10
	{ "bm21.12j",      0x08000, 0x9bfb0d89, BRF_GRA },	     	 //  11
	{ "bm22.14j",      0x08000, 0x04f3c274, BRF_GRA },	     	 //  12
	
	{ "bm24.14k",      0x08000, 0x11134036, BRF_GRA },	     	 //  13	BG2 Tiles
	{ "bm25.14l",      0x08000, 0x092cf9c1, BRF_GRA },	     	 //  14
	
	{ "bm06.10a",      0x08000, 0x97acc8af, BRF_GRA },	     	 //  15	Sprites
	{ "bm07.11a",      0x08000, 0xd78f7197, BRF_GRA },	     	 //  16
	{ "bm08.12a",      0x08000, 0x1a626608, BRF_GRA },	     	 //  17
	{ "bm09.14a",      0x08000, 0x92408400, BRF_GRA },	     	 //  18
	{ "bm10.10c",      0x08000, 0x8438a44a, BRF_GRA },	     	 //  19
	{ "bm11.11c",      0x08000, 0x6c69351d, BRF_GRA },	     	 //  20
	{ "bm12.12c",      0x08000, 0x5e7efdb7, BRF_GRA },	     	 //  21
	{ "bm13.14c",      0x08000, 0x1143829a, BRF_GRA },	     	 //  22
		
	{ "bm14.5f",       0x08000, 0x4d3c6401, BRF_GRA },	     	 //  23	Tilemaps
	{ "bm23.8k",       0x08000, 0xa52aecbd, BRF_GRA },	     	 //  24
	
	{ "bm1.12a",       0x00100, 0x74421f18, BRF_GRA },	     	 //  25	PROMs
	{ "bm2.13a",       0x00100, 0xac27541f, BRF_GRA },	     	 //  26
	{ "bm3.14a",       0x00100, 0x251fb6ff, BRF_GRA },	     	 //  27
	{ "bm5.7f",        0x00100, 0x206713d0, BRF_GRA },	     	 //  28
	{ "bm10.7l",       0x00100, 0x33c2491c, BRF_GRA },	     	 //  29
	{ "bm9.6l",        0x00100, 0xaeea4af7, BRF_GRA },	     	 //  30
	{ "bm12.12m",      0x00100, 0xc18aa136, BRF_GRA },	     	 //  31
	{ "bm11.12l",      0x00100, 0x405aae37, BRF_GRA },	     	 //  32
	{ "bm8.8c",        0x00100, 0xc2010a9e, BRF_GRA },	     	 //  33
	{ "bm7.7c",        0x00100, 0xb56f30c3, BRF_GRA },	     	 //  34
	{ "bm4.12c",       0x00100, 0x91a8a2e1, BRF_GRA },	     	 //  35
	{ "bm6.4b",        0x00100, 0x0eaf5158, BRF_GRA },	     	 //  36
	
	{ "bm.7k",         0x01000, 0xcf4781bf, BRF_OPT | BRF_PRG }, //  37	MCU
};

STD_ROM_PICK(Drvj)
STD_ROM_FN(Drvj)

struct BurnRomInfo DrvjaRomDesc[] = {
	{ "bm01.12d",      0x08000, 0x232df705, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "bm02.13d",      0x10000, 0xaf971575, BRF_ESS | BRF_PRG }, //	 1
	{ "bm03.14d",      0x10000, 0x300ec713, BRF_ESS | BRF_PRG }, //	 2
	
	{ "bm05.4k",       0x08000, 0xee2bd2d7, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program 

	{ "bm04.5h",       0x08000, 0x46cb9d3d, BRF_GRA },	     	 //  4	Characters
	
	{ "bm15.10f",      0x08000, 0x6b1a0443, BRF_GRA },	     	 //  5	BG Tiles
	{ "bm16.11f",      0x08000, 0x23c908c2, BRF_GRA },	     	 //  6
	{ "bm17.12f",      0x08000, 0x46bcdd07, BRF_GRA },	     	 //  7
	{ "bm18.14f",      0x08000, 0xe6ae7ba0, BRF_GRA },	     	 //  8
	{ "bm19.10j",      0x08000, 0x868ababc, BRF_GRA },	     	 //  9
	{ "bm20.11j",      0x08000, 0x0917e5d4, BRF_GRA },	     	 //  10
	{ "bm21.12j",      0x08000, 0x9bfb0d89, BRF_GRA },	     	 //  11
	{ "bm22.14j",      0x08000, 0x04f3c274, BRF_GRA },	     	 //  12
	
	{ "bm24.14k",      0x08000, 0x11134036, BRF_GRA },	     	 //  13	BG2 Tiles
	{ "bm25.14l",      0x08000, 0x092cf9c1, BRF_GRA },	     	 //  14
	
	{ "bm06.10a",      0x08000, 0x97acc8af, BRF_GRA },	     	 //  15	Sprites
	{ "bm07.11a",      0x08000, 0xd78f7197, BRF_GRA },	     	 //  16
	{ "bm08.12a",      0x08000, 0x1a626608, BRF_GRA },	     	 //  17
	{ "bm09.14a",      0x08000, 0x92408400, BRF_GRA },	     	 //  18
	{ "bm10.10c",      0x08000, 0x8438a44a, BRF_GRA },	     	 //  19
	{ "bm11.11c",      0x08000, 0x6c69351d, BRF_GRA },	     	 //  20
	{ "bm12.12c",      0x08000, 0x5e7efdb7, BRF_GRA },	     	 //  21
	{ "bm13.14c",      0x08000, 0x1143829a, BRF_GRA },	     	 //  22
		
	{ "bm14.5f",       0x08000, 0x4d3c6401, BRF_GRA },	     	 //  23	Tilemaps
	{ "bm23.8k",       0x08000, 0xa52aecbd, BRF_GRA },	     	 //  24
	
	{ "bm1.12a",       0x00100, 0x74421f18, BRF_GRA },	     	 //  25	PROMs
	{ "bm2.13a",       0x00100, 0xac27541f, BRF_GRA },	     	 //  26
	{ "bm3.14a",       0x00100, 0x251fb6ff, BRF_GRA },	     	 //  27
	{ "bm5.7f",        0x00100, 0x206713d0, BRF_GRA },	     	 //  28
	{ "bm10.7l",       0x00100, 0x33c2491c, BRF_GRA },	     	 //  29
	{ "bm9.6l",        0x00100, 0xaeea4af7, BRF_GRA },	     	 //  30
	{ "bm12.12m",      0x00100, 0xc18aa136, BRF_GRA },	     	 //  31
	{ "bm11.12l",      0x00100, 0x405aae37, BRF_GRA },	     	 //  32
	{ "bm8.8c",        0x00100, 0xc2010a9e, BRF_GRA },	     	 //  33
	{ "bm7.7c",        0x00100, 0xb56f30c3, BRF_GRA },	     	 //  34
	{ "bm4.12c",       0x00100, 0x91a8a2e1, BRF_GRA },	     	 //  35
	{ "bm6.4b",        0x00100, 0x0eaf5158, BRF_GRA },	     	 //  36
	
	{ "bm.7k",         0x01000, 0xcf4781bf, BRF_OPT | BRF_PRG }, //  37	MCU
};

STD_ROM_PICK(Drvja)
STD_ROM_FN(Drvja)

struct BurnRomInfo DrvjahRomDesc[] = {
	{ "bm01_hack.12d", 0x08000, 0xf6935937, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "bm02.13d",      0x10000, 0xaf971575, BRF_ESS | BRF_PRG }, //	 1
	{ "bm03.14d",      0x10000, 0x300ec713, BRF_ESS | BRF_PRG }, //	 2
	
	{ "bm05.4k",       0x08000, 0xee2bd2d7, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program

	{ "bm04.5h",       0x08000, 0x46cb9d3d, BRF_GRA },	     	 //  4	Characters
	
	{ "bm15.10f",      0x08000, 0x6b1a0443, BRF_GRA },	     	 //  5	BG Tiles
	{ "bm16.11f",      0x08000, 0x23c908c2, BRF_GRA },	     	 //  6
	{ "bm17.12f",      0x08000, 0x46bcdd07, BRF_GRA },	     	 //  7
	{ "bm18.14f",      0x08000, 0xe6ae7ba0, BRF_GRA },	     	 //  8
	{ "bm19.10j",      0x08000, 0x868ababc, BRF_GRA },	     	 //  9
	{ "bm20.11j",      0x08000, 0x0917e5d4, BRF_GRA },	     	 //  10
	{ "bm21.12j",      0x08000, 0x9bfb0d89, BRF_GRA },	     	 //  11
	{ "bm22.14j",      0x08000, 0x04f3c274, BRF_GRA },	     	 //  12
	
	{ "bm24.14k",      0x08000, 0x11134036, BRF_GRA },	     	 //  13	BG2 Tiles
	{ "bm25.14l",      0x08000, 0x092cf9c1, BRF_GRA },	     	 //  14
	
	{ "bm06.10a",      0x08000, 0x97acc8af, BRF_GRA },	     	 //  15	Sprites
	{ "bm07.11a",      0x08000, 0xd78f7197, BRF_GRA },	     	 //  16
	{ "bm08.12a",      0x08000, 0x1a626608, BRF_GRA },	     	 //  17
	{ "bm09.14a",      0x08000, 0x92408400, BRF_GRA },	     	 //  18
	{ "bm10.10c",      0x08000, 0x8438a44a, BRF_GRA },	     	 //  19
	{ "bm11.11c",      0x08000, 0x6c69351d, BRF_GRA },	     	 //  20
	{ "bm12.12c",      0x08000, 0x5e7efdb7, BRF_GRA },	     	 //  21
	{ "bm13.14c",      0x08000, 0x1143829a, BRF_GRA },	     	 //  22
		
	{ "bm14.5f",       0x08000, 0x4d3c6401, BRF_GRA },	     	 //  23	Tilemaps
	{ "bm23.8k",       0x08000, 0xa52aecbd, BRF_GRA },	     	 //  24
	
	{ "bm1.12a",       0x00100, 0x74421f18, BRF_GRA },	     	 //  25	PROMs
	{ "bm2.13a",       0x00100, 0xac27541f, BRF_GRA },	     	 //  26
	{ "bm3.14a",       0x00100, 0x251fb6ff, BRF_GRA },	     	 //  27
	{ "bm5.7f",        0x00100, 0x206713d0, BRF_GRA },	     	 //  28
	{ "bm10.7l",       0x00100, 0x33c2491c, BRF_GRA },	     	 //  29
	{ "bm9.6l",        0x00100, 0xaeea4af7, BRF_GRA },	     	 //  30
	{ "bm12.12m",      0x00100, 0xc18aa136, BRF_GRA },	     	 //  31
	{ "bm11.12l",      0x00100, 0x405aae37, BRF_GRA },	     	 //  32
	{ "bm8.8c",        0x00100, 0xc2010a9e, BRF_GRA },	     	 //  33
	{ "bm7.7c",        0x00100, 0xb56f30c3, BRF_GRA },	     	 //  34
	{ "bm4.12c",       0x00100, 0x91a8a2e1, BRF_GRA },	     	 //  35
	{ "bm6.4b",        0x00100, 0x0eaf5158, BRF_GRA },	     	 //  36
	
	{ "bm.7k",         0x01000, 0xcf4781bf, BRF_OPT | BRF_PRG }, //  37	MCU
};

STD_ROM_PICK(Drvjah)
STD_ROM_FN(Drvjah)

struct BurnRomInfo DrvbRomDesc[] = {
	{ "1.12d",         0x08000, 0x9a2d70ab, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "bm02.13d",      0x10000, 0xaf971575, BRF_ESS | BRF_PRG }, //	 1
	{ "bm03.14d",      0x10000, 0x300ec713, BRF_ESS | BRF_PRG }, //	 2
	
	{ "bm05.4k",       0x08000, 0xee2bd2d7, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program 

	{ "4.5h",          0x08000, 0x0aba2096, BRF_GRA },	     	 //  4	Characters
	
	{ "15.12f",        0x10000, 0x622b4fba, BRF_GRA },	     	 //  5	BG Tiles
	{ "16.14f",        0x10000, 0x25471a8d, BRF_GRA },	     	 //  6
	{ "17.12j",        0x10000, 0x9da79653, BRF_GRA },	     	 //  7
	{ "18.14j",        0x10000, 0x1f3aced8, BRF_GRA },	     	 //  8
	
	{ "bm24.14k",      0x08000, 0x11134036, BRF_GRA },	     	 //  9	BG2 Tiles
	{ "bm25.14l",      0x08000, 0x092cf9c1, BRF_GRA },	     	 //  10
	
	{ "bm06.10a",      0x08000, 0x97acc8af, BRF_GRA },	     	 //  11	Sprites
	{ "bm07.11a",      0x08000, 0xd78f7197, BRF_GRA },	     	 //  12
	{ "bm08.12a",      0x08000, 0x1a626608, BRF_GRA },	     	 //  13
	{ "bm09.14a",      0x08000, 0x92408400, BRF_GRA },	     	 //  14
	{ "bm10.10c",      0x08000, 0x8438a44a, BRF_GRA },	     	 //  15
	{ "bm11.11c",      0x08000, 0x6c69351d, BRF_GRA },	     	 //  16
	{ "bm12.12c",      0x08000, 0x5e7efdb7, BRF_GRA },	     	 //  17
	{ "bm13.14c",      0x08000, 0x1143829a, BRF_GRA },	     	 //  18
		
	{ "bm14.5f",       0x08000, 0x4d3c6401, BRF_GRA },	     	 //  19	Tilemaps
	{ "bm23.8k",       0x08000, 0xa52aecbd, BRF_GRA },	     	 //  20
	
	{ "bm1.12a",       0x00100, 0x74421f18, BRF_GRA },	     	 //  21	PROMs
	{ "bm2.13a",       0x00100, 0xac27541f, BRF_GRA },	     	 //  22
	{ "bm3.14a",       0x00100, 0x251fb6ff, BRF_GRA },	     	 //  23
	{ "bm5.7f",        0x00100, 0x206713d0, BRF_GRA },	     	 //  24
	{ "bm10.7l",       0x00100, 0x33c2491c, BRF_GRA },	     	 //  25
	{ "bm9.6l",        0x00100, 0xaeea4af7, BRF_GRA },	     	 //  26
	{ "bm12.12m",      0x00100, 0xc18aa136, BRF_GRA },	     	 //  27
	{ "bm11.12l",      0x00100, 0x405aae37, BRF_GRA },	     	 //  28
	{ "bm8.8c",        0x00100, 0xc2010a9e, BRF_GRA },	     	 //  29
	{ "bm7.7c",        0x00100, 0xb56f30c3, BRF_GRA },	     	 //  30
	{ "bm4.12c",       0x00100, 0x91a8a2e1, BRF_GRA },	     	 //  31
	{ "bm6.4b",        0x00100, 0x0eaf5158, BRF_GRA },	     	 //  32
};

STD_ROM_PICK(Drvb)
STD_ROM_FN(Drvb)

struct BurnRomInfo Drvb2RomDesc[] = {
	{ "u28.bin",       0x08000, 0xb3b7c7cd, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "u27.bin",       0x10000, 0xaf971575, BRF_ESS | BRF_PRG }, //	 1
	{ "u26.bin",       0x10000, 0x300ec713, BRF_ESS | BRF_PRG }, //	 2
	
	{ "u88.bin",       0x08000, 0xee2bd2d7, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program 

	{ "u62.bin",       0x08000, 0x0aba2096, BRF_GRA },	     	 //  4	Characters
	
	{ "u66.bin",       0x10000, 0x075e9a7f, BRF_GRA },	     	 //  5	BG Tiles
	{ "u65.bin",       0x10000, 0x05aca09a, BRF_GRA },	     	 //  6
	{ "u79.bin",       0x10000, 0x0f4b7e0e, BRF_GRA },	     	 //  7
	{ "u70.bin",       0x10000, 0x61a90c0a, BRF_GRA },	     	 //  8
	
	{ "u102.bin",      0x08000, 0x11134036, BRF_GRA },	     	 //  9	BG2 Tiles
	{ "u116.bin",      0x08000, 0x092cf9c1, BRF_GRA },	     	 //  10
	
	{ "u9.bin",        0x08000, 0x97acc8af, BRF_GRA },	     	 //  11	Sprites
	{ "u10.bin",       0x08000, 0xd78f7197, BRF_GRA },	     	 //  12
	{ "u8.bin",        0x10000, 0xed5c788a, BRF_GRA },	     	 //  13
	{ "u18.bin",       0x08000, 0x8438a44a, BRF_GRA },	     	 //  14
	{ "u19.bin",       0x08000, 0x6c69351d, BRF_GRA },	     	 //  15
	{ "u17.bin",       0x10000, 0x4b42445e, BRF_GRA },	     	 //  16
		
	{ "u58.bin",       0x08000, 0x4d3c6401, BRF_GRA },	     	 //  17	Tilemaps
	{ "u96.bin",       0x08000, 0x254c881d, BRF_GRA },	     	 //  18
	
	{ "bm1.12a",       0x00100, 0x74421f18, BRF_GRA },	     	 //  19	PROMs
	{ "bm2.13a",       0x00100, 0xac27541f, BRF_GRA },	     	 //  20
	{ "bm3.14a",       0x00100, 0x251fb6ff, BRF_GRA },	     	 //  21
	{ "bm5.7f",        0x00100, 0x206713d0, BRF_GRA },	     	 //  22
	{ "bm10.7l",       0x00100, 0x33c2491c, BRF_GRA },	     	 //  23
	{ "bm9.6l",        0x00100, 0xaeea4af7, BRF_GRA },	     	 //  24
	{ "bm12.12m",      0x00100, 0xc18aa136, BRF_GRA },	     	 //  25
	{ "bm11.12l",      0x00100, 0x405aae37, BRF_GRA },	     	 //  26
	{ "bm8.8c",        0x00100, 0xc2010a9e, BRF_GRA },	     	 //  27
	{ "bm7.7c",        0x00100, 0xb56f30c3, BRF_GRA },	     	 //  28
	{ "bm4.12c",       0x00100, 0x91a8a2e1, BRF_GRA },	     	 //  29
	{ "bm6.4b",        0x00100, 0x0eaf5158, BRF_GRA },	     	 //  30
};

STD_ROM_PICK(Drvb2)
STD_ROM_FN(Drvb2)

struct BurnRomInfo DrvbjRomDesc[] = {
	{ "mkb03.12d",     0x08000, 0xb3b7c7cd, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "bm02.13d",      0x10000, 0xaf971575, BRF_ESS | BRF_PRG }, //	 1
	{ "bm03.14d",      0x10000, 0x300ec713, BRF_ESS | BRF_PRG }, //	 2
	
	{ "bm05.4k",       0x08000, 0xee2bd2d7, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program 

	{ "bm04.5h",       0x08000, 0x46cb9d3d, BRF_GRA },	     	 //  4	Characters
	
	{ "mkb12.12f",     0x10000, 0x075e9a7f, BRF_GRA },	     	 //  5	BG Tiles
	{ "mkb11.14f",     0x10000, 0x05aca09a, BRF_GRA },	     	 //  6
	{ "mkb09.12j",     0x10000, 0x0f4b7e0e, BRF_GRA },	     	 //  7
	{ "mkb10.14j",     0x10000, 0x61a90c0a, BRF_GRA },	     	 //  8
	
	{ "mkb08.14k",     0x08000, 0x798215e3, BRF_GRA },	     	 //  9	BG2 Tiles
	{ "bm25.14l",      0x08000, 0x092cf9c1, BRF_GRA },	     	 //  10
	
	{ "bm06.10a",      0x08000, 0x97acc8af, BRF_GRA },	     	 //  11	Sprites
	{ "bm07.11a",      0x08000, 0xd78f7197, BRF_GRA },	     	 //  12
	{ "bm08.12a",      0x08000, 0x1a626608, BRF_GRA },	     	 //  13
	{ "bm09.14a",      0x08000, 0x92408400, BRF_GRA },	     	 //  14
	{ "bm10.10c",      0x08000, 0x8438a44a, BRF_GRA },	     	 //  15
	{ "bm11.11c",      0x08000, 0x6c69351d, BRF_GRA },	     	 //  16
	{ "bm12.12c",      0x08000, 0x5e7efdb7, BRF_GRA },	     	 //  17
	{ "bm13.14c",      0x08000, 0x1143829a, BRF_GRA },	     	 //  18
		
	{ "bm14.5f",       0x08000, 0x4d3c6401, BRF_GRA },	     	 //  19	Tilemaps
	{ "mkb07.8k",      0x08000, 0xae1b317f, BRF_GRA },	     	 //  20
	
	{ "bm1.12a",       0x00100, 0x74421f18, BRF_GRA },	     	 //  21	PROMs
	{ "bm2.13a",       0x00100, 0xac27541f, BRF_GRA },	     	 //  22
	{ "bm3.14a",       0x00100, 0x251fb6ff, BRF_GRA },	     	 //  23
	{ "bm5.7f",        0x00100, 0x206713d0, BRF_GRA },	     	 //  24
	{ "bm10.7l",       0x00100, 0x33c2491c, BRF_GRA },	     	 //  25
	{ "bm9.6l",        0x00100, 0xaeea4af7, BRF_GRA },	     	 //  26
	{ "bm12.12m",      0x00100, 0xc18aa136, BRF_GRA },	     	 //  27
	{ "bm11.12l",      0x00100, 0x405aae37, BRF_GRA },	     	 //  28
	{ "bm8.8c",        0x00100, 0xc2010a9e, BRF_GRA },	     	 //  29
	{ "bm7.7c",        0x00100, 0xb56f30c3, BRF_GRA },	     	 //  30
	{ "bm4.12c",       0x00100, 0x91a8a2e1, BRF_GRA },	     	 //  31
	{ "bm6.4b",        0x00100, 0x0eaf5158, BRF_GRA },	     	 //  32
};

STD_ROM_PICK(Drvbj)
STD_ROM_FN(Drvbj)

struct BurnRomInfo DrvkaiRomDesc[] = {
	{ "bmk01.12d",     0x08000, 0x7d2211db, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "bmk02.13d",     0x10000, 0x2ebbc8c5, BRF_ESS | BRF_PRG }, //	 1
	{ "bmk03.14d",     0x10000, 0x475a6ac5, BRF_ESS | BRF_PRG }, //	 2
	
	{ "bmk05.4k",      0x08000, 0x25f37957, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program 

	{ "bmk04.5h",      0x08000, 0x884a8692, BRF_GRA },	     	 //  4	Characters
	
	{ "bm15.10f",      0x08000, 0x6b1a0443, BRF_GRA },	     	 //  5	BG Tiles
	{ "bmk16.11f",     0x08000, 0x9416fe0d, BRF_GRA },	     	 //  6
	{ "bmk17.12f",     0x08000, 0x3d5acab9, BRF_GRA },	     	 //  7
	{ "bmk18.14f",     0x08000, 0x7b62da1d, BRF_GRA },	     	 //  8
	{ "bm19.10j",      0x08000, 0x868ababc, BRF_GRA },	     	 //  9
	{ "bmk20.11j",     0x08000, 0xb90364c1, BRF_GRA },	     	 //  10
	{ "bmk21.12j",     0x08000, 0x8c7fe74a, BRF_GRA },	     	 //  11
	{ "bmk22.14j",     0x08000, 0xd5ef8a0e, BRF_GRA },	     	 //  12
	
	{ "bmk24.14k",     0x08000, 0xbf186ef2, BRF_GRA },	     	 //  13	BG2 Tiles
	{ "bmk25.14l",     0x08000, 0xa755faf1, BRF_GRA },	     	 //  14
	
	{ "bmk06.10a",     0x08000, 0x5f7e38b3, BRF_GRA },	     	 //  15	Sprites
	{ "bmk07.11a",     0x08000, 0xff3751fd, BRF_GRA },	     	 //  16
	{ "bmk08.12a",     0x08000, 0x159d51bd, BRF_GRA },	     	 //  17
	{ "bmk09.14a",     0x08000, 0x8683e3d2, BRF_GRA },	     	 //  18
	{ "bmk10.10c",     0x08000, 0x1e0d9571, BRF_GRA },	     	 //  19
	{ "bmk11.11c",     0x08000, 0xf1fc5ee1, BRF_GRA },	     	 //  20
	{ "bmk12.12c",     0x08000, 0x0f50c001, BRF_GRA },	     	 //  21
	{ "bmk13.14c",     0x08000, 0xfd1acf8e, BRF_GRA },	     	 //  22
		
	{ "bmk14.5f",      0x08000, 0xcf0f5a53, BRF_GRA },	     	 //  23	Tilemaps
	{ "bmk23.8k",      0x08000, 0x17f77ef9, BRF_GRA },	     	 //  24
	
	{ "bmk1.12a",      0x00100, 0xe001ea33, BRF_GRA },	     	 //  25	PROMs
	{ "bmk2.13a",      0x00100, 0xaf34d91a, BRF_GRA },	     	 //  26
	{ "bmk3.14a",      0x00100, 0x43e9f6ef, BRF_GRA },	     	 //  27
	{ "bmk5.7f",       0x00100, 0x41878934, BRF_GRA },	     	 //  28
	{ "bmk10.7l",      0x00100, 0xde44b748, BRF_GRA },	     	 //  29
	{ "bmk9.6l",       0x00100, 0x59ea57c0, BRF_GRA },	     	 //  30
	{ "bmk12.12m",     0x00100, 0x8765f8b0, BRF_GRA },	     	 //  31
	{ "bmk11.12l",     0x00100, 0x87a8854e, BRF_GRA },	     	 //  32
	{ "bmk8.8c",       0x00100, 0xdad17e2d, BRF_GRA },	     	 //  33
	{ "bmk7.7c",       0x00100, 0x76307f8d, BRF_GRA },	     	 //  34
	{ "bm4.12c",       0x00100, 0x91a8a2e1, BRF_GRA },	     	 //  35
	{ "bm6.4b",        0x00100, 0x0eaf5158, BRF_GRA },	     	 //  36
	
	{ "bm.7k",         0x01000, 0xcf4781bf, BRF_OPT | BRF_PRG }, //  37	MCU
};

STD_ROM_PICK(Drvkai)
STD_ROM_FN(Drvkai)

struct BurnRomInfo DrvmiiRomDesc[] = {
	/* Prototype, location test or limited release? - PCB had genuine CAPCOM labels on the roms */
	{ "01.12d",        0x08000, 0x8ba22485, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "02.13d",        0x10000, 0x659a5455, BRF_ESS | BRF_PRG }, //	 1
	{ "03.14d",        0x10000, 0x159ea771, BRF_ESS | BRF_PRG }, //	 2
	
	{ "05.4k",         0x08000, 0x25f37957, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program 

	{ "04.5h",         0x08000, 0x8190e092, BRF_GRA },	     	 //  4	Characters
	
	{ "bm15.10f",      0x08000, 0x6b1a0443, BRF_GRA },	     	 //  5	BG Tiles
	{ "bm16.11f",      0x08000, 0x23c908c2, BRF_GRA },	     	 //  6
	{ "17.12f",        0x08000, 0x3d5acab9, BRF_GRA },	     	 //  7
	{ "18.14f",        0x08000, 0x7b62da1d, BRF_GRA },	     	 //  8
	{ "bm19.10j",      0x08000, 0x868ababc, BRF_GRA },	     	 //  9
	{ "bm20.11j",      0x08000, 0x0917e5d4, BRF_GRA },	     	 //  10
	{ "21.12j",        0x08000, 0x8c7fe74a, BRF_GRA },	     	 //  11
	{ "22.14j",        0x08000, 0xd5ef8a0e, BRF_GRA },	     	 //  12
	
	{ "24.14k",        0x08000, 0xa0074c68, BRF_GRA },	     	 //  13	BG2 Tiles
	{ "25.14l",        0x08000, 0xf979b2f2, BRF_GRA },	     	 //  14
	
	{ "06.10a",        0x08000, 0xb261d5d7, BRF_GRA },	     	 //  15	Sprites
	{ "07.11a",        0x08000, 0x2af8a6f2, BRF_GRA },	     	 //  16
	{ "08.12a",        0x08000, 0x159d51bd, BRF_GRA },	     	 //  17
	{ "09.14a",        0x08000, 0x70d9f9a7, BRF_GRA },	     	 //  18
	{ "10.10c",        0x08000, 0xde539920, BRF_GRA },	     	 //  19
	{ "11.11c",        0x08000, 0xa6abf183, BRF_GRA },	     	 //  20
	{ "12.12c",        0x08000, 0x0f50c001, BRF_GRA },	     	 //  21
	{ "13.14c",        0x08000, 0xf065f619, BRF_GRA },	     	 //  22
		
	{ "14.5f",         0x08000, 0x02a899f1, BRF_GRA },	     	 //  23	Tilemaps
	{ "23.8k",         0x08000, 0xb6dfdf85, BRF_GRA },	     	 //  24
	
	//  PCB had standard BM0x for bproms 1 through 3, but clearly these should use the Kai BPROMs for correct colors
    //  BPROMs 4 through 8 macth the Kai set - labels were a non descript yellow dot with prom number
    //  BPROMs 9 through 12 are unique - labels were a non descript yellow dot with prom number
	{ "bmk1.12a",      0x00100, 0xe001ea33, BRF_GRA },	     	 //  25	PROMs
	{ "bmk2.13a",      0x00100, 0xaf34d91a, BRF_GRA },	     	 //  26
	{ "bmk3.14a",      0x00100, 0x43e9f6ef, BRF_GRA },	     	 //  27
	{ "5.7f",          0x00100, 0x41878934, BRF_GRA },	     	 //  28
	{ "10.7l",         0x00100, 0xdb53adf0, BRF_GRA },	     	 //  29
	{ "9.6l",          0x00100, 0x75d5cc90, BRF_GRA },	     	 //  30
	{ "12.12m",        0x00100, 0x784bdf33, BRF_GRA },	     	 //  31
	{ "11.12l",        0x00100, 0x6fb2e170, BRF_GRA },	     	 //  32
	{ "8.8c",          0x00100, 0xdad17e2d, BRF_GRA },	     	 //  33
	{ "7.7c",          0x00100, 0x76307f8d, BRF_GRA },	     	 //  34
	{ "4.12c",         0x00100, 0x91a8a2e1, BRF_GRA },	     	 //  35
	{ "6.4b",          0x00100, 0x0eaf5158, BRF_GRA },	     	 //  36
	
	{ "bm.7k",         0x01000, 0xcf4781bf, BRF_OPT | BRF_PRG }, //  37	MCU
};

STD_ROM_PICK(Drvmii)
STD_ROM_FN(Drvmii)

#endif