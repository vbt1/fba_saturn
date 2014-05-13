#ifndef _D_SYS2_H_
#define _D_SYS2_H_

#include "d_sys1_common.h"
/*
#include "burnint.h"
#include "sn76496.h"
#include "bitswap.h"
//#include "mc8123.h"
//#include "saturn/sc_saturn.h"
#include "saturn/ovl.h"

#define SOUND_LEN 240
#define nSegmentLength 	SOUND_LEN / 10

#define COLADDR (FBUF_ADDR-0x400)
#define COLADDR_SPR	(COLADDR>>3)
//#define SC_RELEASE 1

int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));
*/
/*static*/ int ChplftbInit();
/*static*/ int System1Exit();
/*static*/ int System1Frame();

/*==============================================================================================
Input Definitions
===============================================================================================*/

/*static*/ struct BurnInputInfo ChplftbInputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , System1InputPort2 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , System1InputPort2 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , System1InputPort2 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , System1InputPort2 + 5, "p2 start"  },

	{"P1 Up"             , BIT_DIGITAL  , System1InputPort0 + 5, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL  , System1InputPort0 + 4, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL  , System1InputPort0 + 7, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , System1InputPort0 + 6, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , System1InputPort0 + 1, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , System1InputPort0 + 2, "p1 fire 2" },

	{"P2 Up"             , BIT_DIGITAL  , System1InputPort1 + 5, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL  , System1InputPort1 + 4, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL  , System1InputPort1 + 7, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , System1InputPort1 + 6, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , System1InputPort1 + 1, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL  , System1InputPort1 + 2, "p2 fire 2" },

	{"Reset"             , BIT_DIGITAL  , &System1Reset        , "reset"     },
	{"Service"           , BIT_DIGITAL  , System1InputPort2 + 3, "service"   },
	{"Test"              , BIT_DIGITAL  , System1InputPort2 + 2, "diag"      },
	{"Dip 1"             , BIT_DIPSWITCH, System1Dip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, System1Dip + 1       , "dip"       },
};

STDINPUTINFO(Chplftb)

/*static*/ struct BurnDIPInfo ChplftbDIPList[]=
{
	// Default Values
	{0x0f, 0xff, 0xff, 0xbe, NULL                     },
	{0xf0, 0xff, 0xff, 0xff, NULL                     },

	// Dip 1
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x0f, 0x01, 0x01, 0x00, "Upright"                },
	{0x0f, 0x01, 0x01, 0x01, "Cocktail"               },
	
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x0f, 0x01, 0x0c, 0x04, "3"                      },
	{0x0f, 0x01, 0x0c, 0x0c, "4"                      },
	{0x0f, 0x01, 0x06, 0x08, "5"                      },

	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x0f, 0x01, 0x40, 0x40, "Off"                    },
	{0x0f, 0x01, 0x40, 0x00, "On"                     },
	
	// Dip 2
	{0   , 0xfe, 0   , 8   , "Coinage"                },
	{0x10, 0x01, 0x07, 0x04, "4 Coins 1 Credit"       },
	{0x10, 0x01, 0x07, 0x05, "3 Coins 1 Credit"       },
	{0x10, 0x01, 0x07, 0x00, "4 Coins 2 Credits"      },
	{0x10, 0x01, 0x07, 0x06, "2 Coins 1 Credit"       },
	{0x10, 0x01, 0x07, 0x01, "3 Coins 2 Credits"      },
	{0x10, 0x01, 0x07, 0x02, "2 Coins 1 Credits"      },
	{0x10, 0x01, 0x07, 0x07, "1 Coin  1 Credit"       },
	{0x10, 0x01, 0x07, 0x03, "1 Coin  2 Credits"      },

	{0   , 0xfe, 0   , 2   , "Allow Continue"         },
	{0x10, 0x01, 0x10, 0x00, "Off"                    },
	{0x10, 0x01, 0x10, 0x10, "On"                     },
	
	{0   , 0xfe, 0   , 4   , "Mode"                   },
	{0x10, 0x01, 0xc0, 0xc0, "Normal Game"            },
	{0x10, 0x01, 0xc0, 0x80, "Free Play"              },
	{0x10, 0x01, 0xc0, 0x40, "Test Mode"              },
	{0x10, 0x01, 0xc0, 0x00, "Endless Game"           },
};

STDDIPINFO(Chplftb)

#undef SYSTEM1_COINAGE

/*==============================================================================================
ROM Descriptions
===============================================================================================*/

/*static*/ struct BurnRomInfo ChplftbRomDesc[] = {
	{ "epr7152.90",          0x008000, 0xfe49d83e, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "epr7153.91",          0x008000, 0x48697666, BRF_ESS | BRF_PRG }, //  1	Z80 #1 Program Code
	{ "epr7154.92",          0x008000, 0x56d6222a, BRF_ESS | BRF_PRG }, //  2	Z80 #1 Program Code
	
	{ "epr7130.126",        0x008000, 0x346af118, BRF_ESS | BRF_PRG }, //  3	Z80 #2 Program Code
	
	{ "epr7127.4",        0x008000, 0x1e708f6d, BRF_GRA },		  //  4 Tiles
	{ "epr7128.5",        0x008000, 0xb922e787, BRF_GRA },		  //  5 Tiles
	{ "epr7129.6",        0x008000, 0xbd3b6e6e, BRF_GRA },		  //  6 Tiles

	{ "epr7121.87",      0x008000, 0xf2b88f73, BRF_GRA },		  //  4 Sprites
	{ "epr7120.86",      0x008000, 0x517d7fd3, BRF_GRA },		  //  5 Sprites
	{ "epr7123.89",      0x008000, 0x8f16a303, BRF_GRA },		  //  6 Sprites
	{ "epr7122.88",      0x008000, 0x7c93f160, BRF_GRA },		  //  7 Sprites

	{ "pr7119.20",        0x000100, 0xb2a8260f, BRF_OPT },		  //  8 Red PROM
	{ "pr7118.14",        0x000100, 0x693e20c7, BRF_OPT },		  //  9 Green PROM
	{ "pr7117.8",          0x000100, 0x4124307e, BRF_OPT },		  //  10 Blue PROM
};

STD_ROM_PICK(Chplftb)
STD_ROM_FN(Chplftb)


/*==============================================================================================
Driver defs
===============================================================================================*/


#endif