#ifndef _D_GNG_H_
#define _D_GNG_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "m6809_intf.h"
#include "raze/raze.h"

#undef CZ80

#define SOUND_LEN 192
int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));

/*static*/ unsigned char DrvInputPort0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/ unsigned char DrvInputPort1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/ unsigned char DrvInputPort2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/ unsigned char DrvDip[2]        = {0, 0};
/*static*/ unsigned char DrvInput[3]      = {0x00, 0x00, 0x00};
/*static*/ unsigned char DrvReset         = 0;

/*static*/ inline void DrvMakeInputs();
/*static*/ int GngaInit();
/*static*/ void DrvDraw();
/*static*/ int DrvFrame();
/*static*/ int DrvExit();
/*static*/ int DrvInit();
/*static*/ void make_cram_lut(void);
/*static*/ void DrvCalcPalette();
/*static*/ Uint16 cram_lut[32768*2];

/*static*/ unsigned char *Mem                 = NULL;
/*static*/ unsigned char *MemEnd              = NULL;
/*static*/ unsigned char *RamStart            = NULL;
/*static*/ unsigned char *RamEnd              = NULL;
/*static*/ unsigned char *DrvM6809Rom         = NULL;
/*static*/ unsigned char *DrvZ80Rom           = NULL;
/*static*/ unsigned char *DrvM6809Ram         = NULL;
/*static*/ unsigned char *DrvZ80Ram           = NULL;
/*static*/ unsigned char *DrvFgVideoRam       = NULL;
/*static*/ unsigned char *DrvBgVideoRam       = NULL;
/*static*/ unsigned char *DrvSpriteRam        = NULL;
/*static*/ unsigned char *DrvSpriteRamBuffer  = NULL;
/*static*/ unsigned char *DrvPaletteRam1      = NULL;
/*static*/ unsigned char *DrvPaletteRam2      = NULL;
/*static*/ unsigned char *DrvChars            = NULL;
/*static*/ unsigned char *DrvTiles            = NULL;
/*static*/ unsigned char *DrvSprites          = NULL;
///*static*/ unsigned char *DrvTempRom          = NULL;
///*static*/ unsigned int  *DrvPalette          = NULL;

/*static*/ unsigned char DrvRomBank;
/*static*/ unsigned char DrvBgScrollX[2];
/*static*/ unsigned char DrvBgScrollY[2];
/*static*/ unsigned char DrvSoundLatch;

/*static*/ int RomLoadOffset = 0;

/*static*/ int nCyclesDone[2], nCyclesTotal[2];
/*static*/ int nCyclesSegment;

///*static*/ int Diamond;

/*static*/ struct BurnInputInfo DrvInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL  , DrvInputPort0 + 6, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , DrvInputPort0 + 0, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , DrvInputPort0 + 7, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , DrvInputPort0 + 1, "p2 start"  },

	{"Up"                , BIT_DIGITAL  , DrvInputPort1 + 3, "p1 up"     },
	{"Down"              , BIT_DIGITAL  , DrvInputPort1 + 2, "p1 down"   },
	{"Left"              , BIT_DIGITAL  , DrvInputPort1 + 1, "p1 left"   },
	{"Right"             , BIT_DIGITAL  , DrvInputPort1 + 0, "p1 right"  },
	{"Fire 1"            , BIT_DIGITAL  , DrvInputPort1 + 4, "p1 fire 1" },
	{"Fire 2"            , BIT_DIGITAL  , DrvInputPort1 + 5, "p1 fire 2" },
	
	{"Up (Cocktail)"     , BIT_DIGITAL  , DrvInputPort2 + 3, "p2 up"     },
	{"Down (Cocktail)"   , BIT_DIGITAL  , DrvInputPort2 + 2, "p2 down"   },
	{"Left (Cocktail)"   , BIT_DIGITAL  , DrvInputPort2 + 1, "p2 left"   },
	{"Right (Cocktail)"  , BIT_DIGITAL  , DrvInputPort2 + 0, "p2 right"  },
	{"Fire 1 (Cocktail)" , BIT_DIGITAL  , DrvInputPort2 + 4, "p2 fire 1" },
	{"Fire 2 (Cocktail)" , BIT_DIGITAL  , DrvInputPort2 + 5, "p2 fire 2" },

	{"Reset"             , BIT_DIGITAL  , &DrvReset        , "reset"     },
	{"Service"           , BIT_DIGITAL  , DrvInputPort0 + 5, "service"   },
	{"Dip 1"             , BIT_DIPSWITCH, DrvDip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, DrvDip + 1       , "dip"       },
};

STDINPUTINFO(Drv)

/*static*/ struct BurnDIPInfo DrvDIPList[]=
{
	// Default Values
	{0x12, 0xff, 0xff, 0xdf, NULL                     },
	{0x13, 0xff, 0xff, 0xfb, NULL                     },
	
	// Dip 1
	{0   , 0xfe, 0   , 16  , "Coinage"                },
	{0x12, 0x01, 0x0f, 0x02, "4 Coins 1 Play"         },
	{0x12, 0x01, 0x0f, 0x05, "3 Coins 1 Play"         },
	{0x12, 0x01, 0x0f, 0x08, "2 Coins 1 Play"         },
	{0x12, 0x01, 0x0f, 0x04, "3 Coins 2 Plays"        },
	{0x12, 0x01, 0x0f, 0x01, "4 Coins 3 Plays"        },	
	{0x12, 0x01, 0x0f, 0x0f, "1 Coin  1 Play"         },
	{0x12, 0x01, 0x0f, 0x03, "3 Coins 4 Plays"        },
	{0x12, 0x01, 0x0f, 0x07, "2 Coins 3 Plays"        },
	{0x12, 0x01, 0x0f, 0x0e, "1 Coin  2 Plays"        },
	{0x12, 0x01, 0x0f, 0x06, "2 Coins 5 Plays"        },
	{0x12, 0x01, 0x0f, 0x0d, "1 Coin  3 Plays"        },
	{0x12, 0x01, 0x0f, 0x0c, "1 Coin  4 Plays"        },
	{0x12, 0x01, 0x0f, 0x0b, "1 Coin  5 Plays"        },
	{0x12, 0x01, 0x0f, 0x0a, "1 Coin  6 Plays"        },
	{0x12, 0x01, 0x0f, 0x09, "1 Coin  7 Plays"        },
	{0x12, 0x01, 0x0f, 0x00, "Freeplay"               },
	
	{0   , 0xfe, 0   , 2   , "Coinage affects"        },
	{0x12, 0x01, 0x10, 0x10, "Coin A"                 },
	{0x12, 0x01, 0x10, 0x00, "Coin B"                 },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x12, 0x01, 0x20, 0x20, "Off"                    },
	{0x12, 0x01, 0x20, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Service Mode"           },
	{0x12, 0x01, 0x40, 0x40, "Off"                    },
	{0x12, 0x01, 0x40, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x12, 0x01, 0x80, 0x80, "Off"                    },
	{0x12, 0x01, 0x80, 0x00, "On"                     },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x13, 0x01, 0x03, 0x03, "3"                      },
	{0x13, 0x01, 0x03, 0x02, "4"                      },
	{0x13, 0x01, 0x03, 0x01, "5"                      },
	{0x13, 0x01, 0x03, 0x00, "7"                      },
	
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x13, 0x01, 0x04, 0x00, "Upright"                },
	{0x13, 0x01, 0x04, 0x04, "Cocktail"               },
	
	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x13, 0x01, 0x18, 0x18, "20k 70k 70k"            },
	{0x13, 0x01, 0x18, 0x10, "30k 80k 80k"            },
	{0x13, 0x01, 0x18, 0x08, "20k 80k"                },
	{0x13, 0x01, 0x18, 0x00, "30k 80k"                },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"             },
	{0x13, 0x01, 0x60, 0x40, "Easy"                   },
	{0x13, 0x01, 0x60, 0x60, "Normal"                 },
	{0x13, 0x01, 0x60, 0x20, "Difficult"              },
	{0x13, 0x01, 0x60, 0x00, "Very Difficult"         },
};

STDDIPINFO(Drv)

/*static*/ struct BurnDIPInfo DrvjDIPList[]=
{
	// Default Values
	{0x12, 0xff, 0xff, 0xdf, NULL                     },
	{0x13, 0xff, 0xff, 0xfb, NULL                     },
	
	// Dip 1
	{0   , 0xfe, 0   , 16  , "Coinage"                },
	{0x12, 0x01, 0x0f, 0x02, "4 Coins 1 Play"         },
	{0x12, 0x01, 0x0f, 0x05, "3 Coins 1 Play"         },
	{0x12, 0x01, 0x0f, 0x08, "2 Coins 1 Play"         },
	{0x12, 0x01, 0x0f, 0x04, "3 Coins 2 Plays"        },
	{0x12, 0x01, 0x0f, 0x01, "4 Coins 3 Plays"        },	
	{0x12, 0x01, 0x0f, 0x0f, "1 Coin  1 Play"         },
	{0x12, 0x01, 0x0f, 0x03, "3 Coins 4 Plays"        },
	{0x12, 0x01, 0x0f, 0x07, "2 Coins 3 Plays"        },
	{0x12, 0x01, 0x0f, 0x0e, "1 Coin  2 Plays"        },
	{0x12, 0x01, 0x0f, 0x06, "2 Coins 5 Plays"        },
	{0x12, 0x01, 0x0f, 0x0d, "1 Coin  3 Plays"        },
	{0x12, 0x01, 0x0f, 0x0c, "1 Coin  4 Plays"        },
	{0x12, 0x01, 0x0f, 0x0b, "1 Coin  5 Plays"        },
	{0x12, 0x01, 0x0f, 0x0a, "1 Coin  6 Plays"        },
	{0x12, 0x01, 0x0f, 0x09, "1 Coin  7 Plays"        },
	{0x12, 0x01, 0x0f, 0x00, "Freeplay"               },
	
	{0   , 0xfe, 0   , 2   , "Coinage affects"        },
	{0x12, 0x01, 0x10, 0x10, "Coin A"                 },
	{0x12, 0x01, 0x10, 0x00, "Coin B"                 },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x12, 0x01, 0x20, 0x20, "Off"                    },
	{0x12, 0x01, 0x20, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Service Mode"           },
	{0x12, 0x01, 0x40, 0x40, "Off"                    },
	{0x12, 0x01, 0x40, 0x00, "On"                     },
	
	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x12, 0x01, 0x80, 0x80, "Off"                    },
	{0x12, 0x01, 0x80, 0x00, "On"                     },
	
	// Dip 2
	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x13, 0x01, 0x03, 0x03, "3"                      },
	{0x13, 0x01, 0x03, 0x02, "4"                      },
	{0x13, 0x01, 0x03, 0x01, "5"                      },
	{0x13, 0x01, 0x03, 0x00, "7"                      },
	
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x13, 0x01, 0x04, 0x00, "Upright"                },
	{0x13, 0x01, 0x04, 0x04, "Cocktail"               },
	
	{0   , 0xfe, 0   , 4   , "Bonus Life"             },
	{0x13, 0x01, 0x18, 0x18, "20k 70k 70k"            },
	{0x13, 0x01, 0x18, 0x10, "30k 80k 80k"            },
	{0x13, 0x01, 0x18, 0x08, "20k 80k"                },
	{0x13, 0x01, 0x18, 0x00, "30k 80k"                },
	
	{0   , 0xfe, 0   , 4   , "Difficulty"             },
	{0x13, 0x01, 0x60, 0x40, "Easy"                   },
	{0x13, 0x01, 0x60, 0x60, "Normal"                 },
	{0x13, 0x01, 0x60, 0x20, "Difficult"              },
	{0x13, 0x01, 0x60, 0x00, "Very Difficult"         },
	
	{0   , 0xfe, 0   , 2   , "Invulnerability"        },
	{0x13, 0x01, 0x80, 0x80, "Off"                    },
	{0x13, 0x01, 0x80, 0x00, "On"                     },
};

STDDIPINFO(Drvj)

/*static*/ struct BurnRomInfo DrvRomDesc[] = {
	{ "gg4.bin",       0x04000, 0x66606beb, BRF_ESS | BRF_PRG }, //  0	M6809 Program Code
	{ "gg3.bin",       0x08000, 0x9e01c65e, BRF_ESS | BRF_PRG }, //	 1
	{ "gg5.bin",       0x08000, 0xd6397b2b, BRF_ESS | BRF_PRG }, //	 2
	
	{ "gg2.bin",       0x08000, 0x615f5b6f, BRF_ESS | BRF_PRG }, //  3	Z80 Program 
	
	{ "gg1.bin",       0x04000, 0xecfccf07, BRF_GRA },	     //  4	Characters
	
	{ "gg11.bin",      0x04000, 0xddd56fa9, BRF_GRA },	     //  5	Tiles
	{ "gg10.bin",      0x04000, 0x7302529d, BRF_GRA },	     //  6
	{ "gg9.bin",       0x04000, 0x20035bda, BRF_GRA },	     //  7
	{ "gg8.bin",       0x04000, 0xf12ba271, BRF_GRA },	     //  8
	{ "gg7.bin",       0x04000, 0xe525207d, BRF_GRA },	     //  9
	{ "gg6.bin",       0x04000, 0x2d77e9b2, BRF_GRA },	     //  10
	
	{ "gg17.bin",      0x04000, 0x93e50a8f, BRF_GRA },	     //  11	Sprites
	{ "gg16.bin",      0x04000, 0x06d7e5ca, BRF_GRA },	     //  12
	{ "gg15.bin",      0x04000, 0xbc1fe02d, BRF_GRA },	     //  13
	{ "gg14.bin",      0x04000, 0x6aaf12f9, BRF_GRA },	     //  14
	{ "gg13.bin",      0x04000, 0xe80c3fca, BRF_GRA },	     //  15
	{ "gg12.bin",      0x04000, 0x7780a925, BRF_GRA },	     //  16
	
	{ "tbp24s10.14k",  0x00100, 0x0eaf5158, BRF_GRA },	     //  17	PROMs
	{ "63s141.2e",     0x00100, 0x4a1285a4, BRF_GRA },	     //  18
	
	{ "gg-pal10l8.bin",0x0002c, 0x87f1b7e0, BRF_GRA },	     //  19	PLDs
};

STD_ROM_PICK(Drv)
STD_ROM_FN(Drv)

/*static*/ struct BurnRomInfo DrvaRomDesc[] = {
	{ "gng.n10",       0x04000, 0x60343188, BRF_ESS | BRF_PRG }, //  0	M6809 Program Code
	{ "gng.n9",        0x04000, 0xb6b91cfb, BRF_ESS | BRF_PRG }, //	 1
	{ "gng.n8",        0x04000, 0xa5cfa928, BRF_ESS | BRF_PRG }, //	 2
	{ "gng.n13",       0x04000, 0xfd9a8dda, BRF_ESS | BRF_PRG }, //	 3
	{ "gng.n12",       0x04000, 0x13cf6238, BRF_ESS | BRF_PRG }, //	 4
	
	{ "gg2.bin",       0x08000, 0x615f5b6f, BRF_ESS | BRF_PRG }, //  5	Z80 Program 
	
	{ "gg1.bin",       0x04000, 0xecfccf07, BRF_GRA },	     //  6	Characters
	
	{ "gg11.bin",      0x04000, 0xddd56fa9, BRF_GRA },	     //  7	Tiles
	{ "gg10.bin",      0x04000, 0x7302529d, BRF_GRA },	     //  8
	{ "gg9.bin",       0x04000, 0x20035bda, BRF_GRA },	     //  9
	{ "gg8.bin",       0x04000, 0xf12ba271, BRF_GRA },	     //  10
	{ "gg7.bin",       0x04000, 0xe525207d, BRF_GRA },	     //  11
	{ "gg6.bin",       0x04000, 0x2d77e9b2, BRF_GRA },	     //  12
	
	{ "gg17.bin",      0x04000, 0x93e50a8f, BRF_GRA },	     //  13	Sprites
	{ "gg16.bin",      0x04000, 0x06d7e5ca, BRF_GRA },	     //  14
	{ "gg15.bin",      0x04000, 0xbc1fe02d, BRF_GRA },	     //  15
	{ "gg14.bin",      0x04000, 0x6aaf12f9, BRF_GRA },	     //  16
	{ "gg13.bin",      0x04000, 0xe80c3fca, BRF_GRA },	     //  17
	{ "gg12.bin",      0x04000, 0x7780a925, BRF_GRA },	     //  18
	
	{ "tbp24s10.14k",  0x00100, 0x0eaf5158, BRF_GRA },	     //  19	PROMs
	{ "63s141.2e",     0x00100, 0x4a1285a4, BRF_GRA },	     //  20
};

STD_ROM_PICK(Drva)
STD_ROM_FN(Drva)


#endif