#ifndef _D_SYS2_H_
#define _D_SYS2_H_

#include "d_sys1_common.h"
#include "8255ppi.h"
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
/*static*/ int WbmlInit();
/*static*/ int WbmljbInit();

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

static struct BurnInputInfo WbmlInputList[] = {

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

//	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
//	{"Dip A",		BIT_DIPSWITCH,	DrvDip + 0,	"dip"},
};

STDINPUTINFO(Wbml)


static struct BurnDIPInfo WbmlDIPList[]=
{
	// Default Values
	{0x13, 0xff, 0xff, 0xff, NULL                     },
	{0x14, 0xff, 0xff, 0xfe, NULL                     },

	// Dip 1
	SYSTEM1_COINAGE(0x13)
	
	// Dip 2	
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x14, 0x01, 0x01, 0x00, "Upright"                },
	{0x14, 0x01, 0x01, 0x01, "Cocktail"               },

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x14, 0x01, 0x02, 0x00, "Off"		},
	{0x14, 0x01, 0x02, 0x02, "On"		},

	{0   , 0xfe, 0   ,    3, "Lives"		},
	{0x14, 0x01, 0x0c, 0x04, "3"		},
	{0x14, 0x01, 0x0c, 0x0c, "4"		},
	{0x14, 0x01, 0x0c, 0x08, "5"		},

	{0   , 0xfe, 0   ,    2, "Bonus Life"		},
	{0x14, 0x01, 0x10, 0x10, "30000 100000 200000"		},
	{0x14, 0x01, 0x10, 0x00, "50000 150000 250000"		},

	{0   , 0xfe, 0   ,    2, "Difficulty"		},
	{0x14, 0x01, 0x20, 0x20, "Easy"		},
	{0x14, 0x01, 0x20, 0x00, "Hard"		},

	{0   , 0xfe, 0   ,    2, "Test Mode"		},
	{0x14, 0x01, 0x40, 0x40, "Off"		},
	{0x14, 0x01, 0x40, 0x00, "On"		},

	{0   , 0xfe, 0   ,    2, "Unknown"		},
	{0x14, 0x01, 0x80, 0x80, "Off"		},
	{0x14, 0x01, 0x80, 0x00, "On"		},
};

STDDIPINFO(Wbml)

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

// Wonder Boy in Monster Land (Japan New Ver., MC-8123, 317-0043)

static struct BurnRomInfo wbmlRomDesc[] = {
	{ "ep11031a.90",	0x8000, 0xbd3349e5, 1 }, //  0 main
	{ "epr11032.91",	0x8000, 0x9d03bdb2, 1 }, //  1
	{ "epr11033.92",	0x8000, 0x7076905c, 1 }, //  2

	{ "epr11037.126",	0x8000, 0x7a4ee585, 3 }, //  4 sound

	{ "epr11034.4",		0x8000, 0x37a2077d, 4 }, //  5 gfx1
	{ "epr11035.5",		0x8000, 0xcdf2a21b, 4 }, //  6
	{ "epr11036.6",		0x8000, 0x644687fa, 4 }, //  7

	{ "epr11028.87",	0x8000, 0xaf0b3972, 5 }, //  8 gfx2
	{ "epr11027.86",	0x8000, 0x277d8f1d, 5 }, //  9
	{ "epr11030.89",	0x8000, 0xf05ffc76, 5 }, // 10
	{ "epr11029.88",	0x8000, 0xcedc9c61, 5 }, // 11

	{ "pr11026.20",		0x0100, 0x27057298, 6 }, // 12 proms
	{ "pr11025.14",		0x0100, 0x41e4d86b, 6 }, // 13
	{ "pr11024.8",		0x0100, 0x08d71954, 6 }, // 14
	{ "pr5317.37",		0x0100, 0x648350b8, 6 }, // 15

	{ "3170043.key",	0x2000, 0xe354abfc, 2 }, //  3 user1
};

STD_ROM_PICK(wbml)
STD_ROM_FN(wbml)

// Wonder Boy in Monster Land (Japan not encrypted)

static struct BurnRomInfo wbmljbRomDesc[] = {
	{ "wbml.01",		0x10000, 0x66482638, 1 }, //  0 main
	{ "m6.bin",		0x10000, 0x8c08cd11, 1 }, //  1
	{ "m7.bin",		0x10000, 0x11881703, 1 }, //  2

	{ "epr11037.126",	0x08000, 0x7a4ee585, 2 }, //  3 sound

	{ "epr11034.4",		0x08000, 0x37a2077d, 3 }, //  4 gfx1
	{ "epr11035.5",		0x08000, 0xcdf2a21b, 3 }, //  5
	{ "epr11036.6",		0x08000, 0x644687fa, 3 }, //  6

	{ "epr11028.87",	0x08000, 0xaf0b3972, 4 }, //  7 gfx2
	{ "epr11027.86",	0x08000, 0x277d8f1d, 4 }, //  8
	{ "epr11030.89",	0x08000, 0xf05ffc76, 4 }, //  9
	{ "epr11029.88",	0x08000, 0xcedc9c61, 4 }, // 10

	{ "pr11026.20",		0x00100, 0x27057298, 5 }, // 11 proms
	{ "pr11025.14",		0x00100, 0x41e4d86b, 5 }, // 12
	{ "pr11024.8",		0x00100, 0x08d71954, 5 }, // 13
	{ "pr5317.37",		0x00100, 0x648350b8, 5 }, // 14
};

STD_ROM_PICK(wbmljb)
STD_ROM_FN(wbmljb)

// Wonder Boy in Monster Land (English bootleg set 1)

static struct BurnRomInfo wbmlbRomDesc[] = {
/*	{ "wbml.01",		0x10000, 0x66482638, 1 }, //  0 maincpu
	{ "wbml.02",		0x10000, 0x48746bb6, 1 }, //  1
	{ "wbml.03",		0x10000, 0xd57ba8aa, 1 }, //  2

	{ "epr11037.126",	0x08000, 0x7a4ee585, 2 }, //  3 soundcpu

	{ "wbml.08",		0x08000, 0xbbea6afe, 3 }, //  4 tiles
	{ "wbml.09",		0x08000, 0x77567d41, 3 }, //  5
	{ "wbml.10",		0x08000, 0xa52ffbdd, 3 }, //  6
*/
	{ "galaxy.ic0",	0x10000, 0x66482638, 1 }, //  0 maincpu
	{ "galaxy.ic1",	0x10000, 0x89a8ab93, 1 }, //  1
	{ "galaxy.ic2",	0x08000, 0x39e07286, 1 }, //  2

	{ "epr11037.126",	0x08000, 0x7a4ee585, 2 }, //  3 soundcpu

	{ "galaxy.ic4",		0x08000, 0xab75d056, 3 }, //  4 tiles
	{ "galaxy.ic6",		0x08000, 0x6bb5e601, 3 }, //  5
	{ "galaxy.ic5",		0x08000, 0x3c11d151, 3 }, //  6

	{ "epr11028.87",	0x08000, 0xaf0b3972, 4 }, //  7 sprites
	{ "epr11027.86",	0x08000, 0x277d8f1d, 4 }, //  8
	{ "epr11030.89",	0x08000, 0xf05ffc76, 4 }, //  9
	{ "epr11029.88",	0x08000, 0xcedc9c61, 4 }, // 10

	{ "pr11026.20",		0x00100, 0x27057298, 5 }, // 11 palette
	{ "pr11025.14",		0x00100, 0x41e4d86b, 5 }, // 12
	{ "pr11024.8",		0x00100, 0x08d71954, 5 }, // 13

	{ "pr5317.37",		0x00100, 0x648350b8, 6 }, // 14 proms
};

STD_ROM_PICK(wbmlb)
STD_ROM_FN(wbmlb)

// Wonder Boy in Monster Land (English, Virtual Console)
static struct BurnRomInfo wbmlvcRomDesc[] = {

    { "vc.90",        0x10000, 0x093c4852, BRF_ESS | BRF_PRG }, //  0 Z80 #1 Program Code
    { "vc.91",        0x10000, 0x7e973ece, BRF_ESS | BRF_PRG }, //  1
    { "vc.92",        0x08000, 0x32661e7e, BRF_ESS | BRF_PRG }, //  2
    { "epr11037.126",    0x08000, 0x7a4ee585, BRF_ESS | BRF_PRG }, //  3 Z80 #2 Program Code
    { "vc.ic4",            0x08000, 0x820bee59, BRF_GRA }, //  4 Tiles
    { "vc.ic6",            0x08000, 0xa9a1447e, BRF_GRA }, //  5
    { "vc.ic5",            0x08000, 0x359026a0, BRF_GRA }, //  6
    { "epr11028.87",    0x08000, 0xaf0b3972, BRF_GRA }, //  7 Sprites
    { "epr11027.86",    0x08000, 0x277d8f1d, BRF_GRA }, //  8
    { "epr11030.89",    0x08000, 0xf05ffc76, BRF_GRA }, //  9
    { "epr11029.88",    0x08000, 0xcedc9c61, BRF_GRA }, // 10
    { "pr11026.20",        0x00100, 0x27057298, BRF_GRA }, // 11 Red PROM
    { "pr11025.14",        0x00100, 0x41e4d86b, BRF_GRA }, // 12 Blue
    { "pr11024.8",        0x00100, 0x08d71954, BRF_GRA }, // 13 Green
    { "pr5317.37",        0x00100, 0x648350b8, BRF_GRA }, // 14 Timing PROM
};
STD_ROM_PICK(wbmlvc)
STD_ROM_FN(wbmlvc)
/*==============================================================================================
Driver defs
===============================================================================================*/


#endif