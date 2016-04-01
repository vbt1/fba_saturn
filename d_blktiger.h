#ifndef _D_BLKTGR_H_
#define _D_BLKTGR_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "raze/raze.h"

int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));
static INT32 DrvInit();
static INT32 DrvExit();
static INT32 DrvFrame();
static INT32 DrvDraw();
static void make_lut(void);
static void DrvInitSaturn();
UINT16 *ss_map3;

static UINT8 *AllMem;
static UINT8 *MemEnd;
static UINT8 *AllRam;
static UINT8 *RamEnd;
static UINT8 *DrvZ80ROM0;
static UINT8 *DrvZ80ROM1;
static UINT8 *DrvGfxROM0;
static UINT8 *DrvGfxROM1;
static UINT8 *DrvGfxROM2;
static UINT8 *DrvZ80RAM0;
static UINT8 *DrvZ80RAM1;
static UINT8 *DrvSprRAM;
static UINT8 *DrvSprBuf;
static UINT8 *DrvPalRAM;
static UINT8 *DrvBgRAM;
static UINT8 *DrvTxRAM;
static UINT16 *DrvPalette;
static UINT8 DrvRecalc;

static UINT8 *DrvScreenLayout;
static UINT8 *DrvBgEnable;
static UINT8 *DrvFgEnable;
static UINT8 *DrvSprEnable;
static UINT8 *DrvVidBank;
static UINT8 *DrvRomBank;

static UINT8 *soundlatch;
static UINT8 *flipscreen;
static UINT8 *coin_lockout;
static INT32 watchdog;

static UINT16 *DrvScrollx;
static UINT16 *DrvScrolly;

static UINT8 DrvJoy1[8];
static UINT8 DrvJoy2[8];
static UINT8 DrvJoy3[8];
static UINT8 DrvDips[3];
static UINT8 DrvInputs[3];
static UINT8 DrvReset;

static INT32 nCyclesTotal[2];

static struct BurnInputInfo DrvInputList[] = {
	{"Coin 1"       	  , BIT_DIGITAL  , DrvJoy1 + 6,	 "p1 coin"  },
	{"Coin 2"       	  , BIT_DIGITAL  , DrvJoy1 + 7,	 "p2 coin"  },

	{"P1 Start"     	  , BIT_DIGITAL  , DrvJoy1 + 0,	 "p1 start" },

	{"P1 Up"        	  , BIT_DIGITAL  , DrvJoy2 + 3,  "p1 up"    },
	{"P1 Down"      	  , BIT_DIGITAL  , DrvJoy2 + 2,  "p1 down"  },
	{"P1 Left"      	  , BIT_DIGITAL  , DrvJoy2 + 1,  "p1 left"  },
	{"P1 Right"     	  , BIT_DIGITAL  , DrvJoy2 + 0,  "p1 right" },
	{"P1 Button 1"  	  , BIT_DIGITAL  , DrvJoy2 + 4,  "p1 fire 1"},
	{"P1 Button 2"  	  , BIT_DIGITAL  , DrvJoy2 + 5,  "p1 fire 2"},

	{"P2 Start"     	  , BIT_DIGITAL  , DrvJoy1 + 1,	 "p2 start" },

	{"P2 Up"        	  , BIT_DIGITAL  , DrvJoy3 + 3,  "p2 up"    },
	{"P2 Down"      	  , BIT_DIGITAL  , DrvJoy3 + 2,  "p2 down"  },
	{"P2 Left"      	  , BIT_DIGITAL  , DrvJoy3 + 1,  "p2 left"  },
	{"P2 Right"     	  , BIT_DIGITAL  , DrvJoy3 + 0,  "p2 right" },
	{"P2 Button 1"  	  , BIT_DIGITAL  , DrvJoy3 + 4,  "p2 fire 1"},
	{"P2 Button 2"  	  , BIT_DIGITAL  , DrvJoy3 + 5,  "p2 fire 2"},

	{"Reset",		    BIT_DIGITAL  , &DrvReset,	 "reset"    },
	{"Service",		    BIT_DIGITAL  , DrvJoy1 + 5,  "service"  },
	{"Dip 1",		    BIT_DIPSWITCH, DrvDips + 0,	 "dip"	    },
	{"Dip 2",		    BIT_DIPSWITCH, DrvDips + 1,	 "dip"	    },
	{"Dip 3",		    BIT_DIPSWITCH, DrvDips + 2,	 "dip"	    },
};

STDINPUTINFO(Drv)

static struct BurnDIPInfo DrvDIPList[]=
{
	{0x12, 0xff, 0xff, 0xff, NULL			},
 	{0x13, 0xff, 0xff, 0xff, NULL			},
 	{0x14, 0xff, 0x01, 0x01, NULL			},

 	{0x12, 0xfe,    0,    8, "Coin A"		},
 	{0x12, 0x01, 0x07, 0x00, "4 Coin 1 Credits"	},
 	{0x12, 0x01, 0x07, 0x01, "3 Coin 1 Credits"	},
 	{0x12, 0x01, 0x07, 0x02, "2 Coin 1 Credits"	},
 	{0x12, 0x01, 0x07, 0x07, "1 Coin 1 Credits"	},
 	{0x12, 0x01, 0x07, 0x06, "1 Coin 2 Credits"	},
 	{0x12, 0x01, 0x07, 0x05, "1 Coin 3 Credits"	},
 	{0x12, 0x01, 0x07, 0x04, "1 Coin 4 Credits"	},
 	{0x12, 0x01, 0x07, 0x03, "1 Coin 5 Credits"	},

 	{0x12, 0xfe,    0,    8, "Coin B"		},
 	{0x12, 0x01, 0x38, 0x00, "4 Coin 1 Credits"	},
 	{0x12, 0x01, 0x38, 0x08, "3 Coin 1 Credits"	},
 	{0x12, 0x01, 0x38, 0x10, "2 Coin 1 Credits"	},
 	{0x12, 0x01, 0x38, 0x38, "1 Coin 1 Credits"	},
 	{0x12, 0x01, 0x38, 0x30, "1 Coin 2 Credits"	},
 	{0x12, 0x01, 0x38, 0x28, "1 Coin 3 Credits"	},
 	{0x12, 0x01, 0x38, 0x20, "1 Coin 4 Credits"	},
 	{0x12, 0x01, 0x38, 0x18, "1 Coin 5 Credits"	},

 	{0x12, 0xfe,    0,    2, "Flip Screen"		},
 	{0x12, 0x01, 0x40, 0x40, "Off"			},
 	{0x12, 0x01, 0x40, 0x00, "On"			},

 	{0x12, 0xfe,    0,    2, "Test"			},
 	{0x12, 0x01, 0x80, 0x80, "Off"			},
 	{0x12, 0x01, 0x80, 0x00, "On"			},

 	{0x13, 0xfe,    0,    4, "Lives"		},
 	{0x13, 0x01, 0x03, 0x02, "2"			},
 	{0x13, 0x01, 0x03, 0x03, "3"			},
 	{0x13, 0x01, 0x03, 0x01, "5"			},
 	{0x13, 0x01, 0x03, 0x00, "7"			},

 	{0x13, 0xfe,    0,    8, "Difficulty"		},
 	{0x13, 0x01, 0x1c, 0x1c, "1 (Easiest)"		},
 	{0x13, 0x01, 0x1c, 0x18, "2"			},
 	{0x13, 0x01, 0x1c, 0x14, "3"			},
 	{0x13, 0x01, 0x1c, 0x10, "4"			},
 	{0x13, 0x01, 0x1c, 0x0c, "5 (Normal)"		},
 	{0x13, 0x01, 0x1c, 0x08, "6"			},
 	{0x13, 0x01, 0x1c, 0x04, "7"			},
 	{0x13, 0x01, 0x1c, 0x00, "8 (Hardest)"		},

 	{0x13, 0xfe,    0,    2, "Demo Sounds"		},
 	{0x13, 0x01, 0x20, 0x00, "Off"			},
 	{0x13, 0x01, 0x20, 0x20, "On"			},

 	{0x13, 0xfe,    0,    2, "Allow Continue"	},
 	{0x13, 0x01, 0x40, 0x00, "No"			},
 	{0x13, 0x01, 0x40, 0x40, "Yes"			},

 	{0x13, 0xfe,    0,    2, "Cabinet"		},
 	{0x13, 0x01, 0x80, 0x00, "Upright"		},
 	{0x13, 0x01, 0x80, 0x80, "Cocktail"		},

 	{0x14, 0xfe,    0,    2, "Coin Lockout Present?"},
 	{0x14, 0x01, 0x01, 0x01, "Yes"			},
 	{0x14, 0x01, 0x01, 0x00, "No"			},
};

STDDIPINFO(Drv)
/*
struct BurnDriver BurnDrvBlkdrgon = {
	"blkdrgon", "blktiger", NULL, NULL, "1987",
	"Black Dragon (Japan)\0", NULL, "Capcom", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARWARE_CAPCOM_MISC, GBF_PLATFORM | GBF_SCRFIGHT, 0,
	NULL, blkdrgonRomInfo, blkdrgonRomName, NULL, NULL, DrvInputInfo, DrvDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x400,
	256, 224, 4, 3
};
*/

// Black Dragon (bootleg)

static struct BurnRomInfo blkdrgonbRomDesc[] = {
	{ "a1",				0x08000, 0x7caf2ba0, 1 | BRF_PRG | BRF_ESS }, //  0 - Z80 #0 Code
	{ "blkdrgon.6e",	0x10000, 0x7d39c26f, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "a3",				0x10000, 0xf4cd0f39, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "blkdrgon.9e",	0x10000, 0x4d1d6680, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "blkdrgon.10e",	0x10000, 0xc8d0c45e, 1 | BRF_PRG | BRF_ESS }, //  4

	{ "bd-06.1l",		0x08000, 0x2cf54274, 2 | BRF_PRG | BRF_ESS }, //  5 - Z80 #0 Code

	{ "b5",				0x08000, 0x852ad2b7, 3 | BRF_GRA },           //  6 - Characters

	{ "blkdrgon.5b",	0x10000, 0x22d0a4b0, 4 | BRF_GRA },           //  7 - Background Tiles
	{ "b1",				0x10000, 0x053ab15c, 4 | BRF_GRA },           //  8
	{ "blkdrgon.9b",	0x10000, 0x9498c378, 4 | BRF_GRA },           //  9
	{ "b3",				0x10000, 0x9dc6e943, 4 | BRF_GRA },           // 10

	{ "bd-08.5a",		0x10000, 0xe2f17438, 5 | BRF_GRA },           // 11 - Sprites
	{ "bd-07.4a",		0x10000, 0x5fccbd27, 5 | BRF_GRA },           // 12
	{ "bd-10.9a",		0x10000, 0xfc33ccc6, 5 | BRF_GRA },           // 13
	{ "bd-09.8a",		0x10000, 0xf449de01, 5 | BRF_GRA },           // 14

	{ "bd01.8j",		0x00100, 0x29b459e5, 6 | BRF_OPT },           // 15 - Proms (not used)
	{ "bd02.9j",		0x00100, 0x8b741e66, 6 | BRF_OPT },           // 16
	{ "bd03.11k",		0x00100, 0x27201c75, 6 | BRF_OPT },           // 17
	{ "bd04.11l",		0x00100, 0xe5490b68, 6 | BRF_OPT },           // 18
};

STD_ROM_PICK(blkdrgonb)
STD_ROM_FN(blkdrgonb)
/*
struct BurnDriver BurnDrvBlkdrgonb = {
	"blkdrgonb", "blktiger", NULL, NULL, "1987",
	"Black Dragon (bootleg)\0", NULL, "bootleg", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE, 2, HARWARE_CAPCOM_MISC, GBF_PLATFORM | GBF_SCRFIGHT, 0,
	NULL, blkdrgonbRomInfo, blkdrgonbRomName, NULL, NULL, DrvInputInfo, DrvDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x400,
	256, 224, 4, 3
};
*/
// Black Tiger

static struct BurnRomInfo blktigerRomDesc[] = {
	{ "bdu01a.5e",		0x08000, 0xa8f98f22, 1 | BRF_PRG | BRF_ESS }, //  0 - Z80 #0 Code
	{ "bdu02a.6e",		0x10000, 0x7bef96e8, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "bdu03a.8e",		0x10000, 0x4089e157, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "bd04.9e",		0x10000, 0xed6af6ec, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "bd05.10e",		0x10000, 0xae59b72e, 1 | BRF_PRG | BRF_ESS }, //  4

	{ "bd06.1l",		0x08000, 0x2cf54274, 2 | BRF_PRG | BRF_ESS }, //  5 - Z80 #0 Code

	{ "bd15.2n",		0x08000, 0x70175d78, 3 | BRF_GRA },           //  6 - Characters

	{ "bd12.5b",		0x10000, 0xc4524993, 4 | BRF_GRA },           //  7 - Background Tiles
	{ "bd11.4b",		0x10000, 0x7932c86f, 4 | BRF_GRA },           //  8
	{ "bd14.9b",		0x10000, 0xdc49593a, 4 | BRF_GRA },           //  9
	{ "bd13.8b",		0x10000, 0x7ed7a122, 4 | BRF_GRA },           // 10

	{ "bd08.5a",		0x10000, 0xe2f17438, 5 | BRF_GRA },           // 11 - Sprites
	{ "bd07.4a",		0x10000, 0x5fccbd27, 5 | BRF_GRA },           // 12
	{ "bd10.9a",		0x10000, 0xfc33ccc6, 5 | BRF_GRA },           // 13
	{ "bd09.8a",		0x10000, 0xf449de01, 5 | BRF_GRA },           // 14

	{ "bd01.8j",		0x00100, 0x29b459e5, 6 | BRF_OPT },           // 15 - Proms (not used)
	{ "bd02.9j",		0x00100, 0x8b741e66, 6 | BRF_OPT },           // 16
	{ "bd03.11k",		0x00100, 0x27201c75, 6 | BRF_OPT },           // 17
	{ "bd04.11l",		0x00100, 0xe5490b68, 6 | BRF_OPT },           // 18

	{ "bd.6k",  		0x01000, 0xac7d14f1, 7 | BRF_PRG | BRF_OPT }, // 19 I8751 Mcu Code
};

STD_ROM_PICK(blktiger)
STD_ROM_FN(blktiger)
/*
struct BurnDriver BurnDrvBlktiger = {
	"blktiger", NULL, NULL, NULL, "1987",
	"Black Tiger\0", NULL, "Capcom", "Miscellaneous",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING, 2, HARWARE_CAPCOM_MISC, GBF_PLATFORM | GBF_SCRFIGHT, 0,
	NULL, blktigerRomInfo, blktigerRomName, NULL, NULL, DrvInputInfo, DrvDIPInfo,
	DrvInit, DrvExit, DrvFrame, DrvDraw, DrvScan, &DrvRecalc, 0x400,
	256, 224, 4, 3
};
*/
#endif