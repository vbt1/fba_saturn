#ifndef _D_HIGEMARU_H_
#define _D_HIGEMARU_H_

#include "burnint.h"
#include "snd/ay8910.h"
#include "saturn/ovl.h"
//#include "raze/raze.h"
//#include "saturn/sc_saturn.h"
#include "czet.h"
#define nBurnSoundLen 128
#define SOUNDRATE   7680L

/*static*/// unsigned char 	bg_dirtybuffer[2048];
UINT16 *map_offset_lut = NULL;//[1024];
/*static*/ unsigned char *MemEnd = NULL, *Rom = NULL, *Gfx0 = NULL, *Gfx1 = NULL, *Prom = NULL;
UINT8 *CZ80Context = NULL;
/*static*/ //short *pAY8910Buffer[6], *pFMBuffer = NULL;
/*static*/ 
unsigned char DrvJoy1[8] = {0,0,0,0,0,0,0,0};
unsigned char DrvJoy2[8] = {0,0,0,0,0,0,0,0};
unsigned char DrvJoy3[8] = {0,0,0,0,0,0,0,0};
unsigned char DrvDips[3] = {0,0,0}; 

PcmHn 			pcm6[6] = {NULL,NULL,NULL,NULL,NULL,NULL};
#define	PCM_ADDR	((void*)0x25a20000)
#define	PCM_SIZE	(4096L*2)				/* 2.. */
#define SOUNDRATE   7680L
void dummy();
static int DrvGfxDecode();
void DrvFrame();
static int DrvExit();
static int DrvInit();
static void Set6PCM();
void init_32_colors(unsigned int *t_pal,unsigned char *color_prom);

/*static*/ struct BurnInputInfo DrvInputList[] = {
	{"Coin 1"       , BIT_DIGITAL  , DrvJoy3 + 7,	"p1 coin"  },
	{"Coin 2"       , BIT_DIGITAL  , DrvJoy3 + 6,	"p2 coin"  },
	{"P1 Start"     , BIT_DIGITAL  , DrvJoy3 + 5,	"p1 start" },
	{"P2 Start"     , BIT_DIGITAL  , DrvJoy3 + 4,	"p2 start" },

	{"P1 Right"     , BIT_DIGITAL  , DrvJoy1 + 0, 	"p1 right" },
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy1 + 1, 	"p1 left"  },
	{"P1 Down"      , BIT_DIGITAL  , DrvJoy1 + 2, 	"p1 down"  },
	{"P1 Up"        , BIT_DIGITAL  , DrvJoy1 + 3, 	"p1 up"    },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy3 + 3,	"p1 fire 1"},

	{"P2 Right"     , BIT_DIGITAL  , DrvJoy2 + 0, 	"p2 right" },
	{"P2 Left"      , BIT_DIGITAL  , DrvJoy2 + 1, 	"p2 left"  },
	{"P2 Down"      , BIT_DIGITAL  , DrvJoy2 + 2, 	"p2 down"  },
	{"P2 Up"        , BIT_DIGITAL  , DrvJoy2 + 3, 	"p2 up"    },
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy3 + 1,	"p2 fire 1"},

	{"Reset"        , BIT_DIGITAL  , NULL  ,	"reset"    },
	{"Dip 1"        , BIT_DIPSWITCH, DrvDips + 0,   "dip 1"    },
	{"Dip 2"        , BIT_DIPSWITCH, DrvDips + 1,   "dip 2"    },
	{"Dip 3"        , BIT_DIPSWITCH, DrvDips + 2,   "dip 3"    },
};

STDINPUTINFO(Drv)

/*static*/ struct BurnDIPInfo DrvDIPList[]=
{
	// Default Values
	{0x0f, 0xff, 0xff, 0xff, NULL                     },
	{0x10, 0xff, 0xff, 0xff, NULL                     },
	{0x11, 0xff, 0xff, 0xff, NULL                     },

	// Dip 1
	{0   , 0xfe, 0   , 2   , "Freeze"                 },
	{0x0f, 0x01, 0x04, 0x04, "Off"		          },
	{0x0f, 0x01, 0x04, 0x00, "On"		          },
	
	// Dip 2
	{0   , 0xfe, 0   , 8   , "Coin A"                 },
	{0x10, 0x01, 0x07, 0x01, "5 Coins 1 Play"         },
	{0x10, 0x01, 0x07, 0x02, "4 Coins 1 Play"         },
	{0x10, 0x01, 0x07, 0x03, "3 Coins 1 Play"         },
	{0x10, 0x01, 0x07, 0x04, "1 Coins 1 Play"         },
	{0x10, 0x01, 0x07, 0x07, "1 Coin  1 Play"         },
	{0x10, 0x01, 0x07, 0x06, "1 Coin  2 Plays"        },
	{0x10, 0x01, 0x07, 0x05, "1 Coin  3 Plays"        },
	{0x10, 0x01, 0x07, 0x00, "Freeplay"               },

	{0   , 0xfe, 0   , 8   , "Coin B"                 },
	{0x10, 0x01, 0x38, 0x08, "5 Coins 1 Play"         },
	{0x10, 0x01, 0x38, 0x10, "4 Coins 1 Play"         },
	{0x10, 0x01, 0x38, 0x18, "3 Coins 1 Play"         },
	{0x10, 0x01, 0x38, 0x20, "1 Coins 1 Play"         },
	{0x10, 0x01, 0x38, 0x38, "1 Coin  1 Play"         },
	{0x10, 0x01, 0x38, 0x30, "1 Coin  2 Plays"        },
	{0x10, 0x01, 0x38, 0x28, "1 Coin  3 Plays"        },
	{0x10, 0x01, 0x38, 0x00, "Freeplay"               },

	{0   , 0xfe, 0   , 4   , "Lives"                  },
	{0x10, 0x01, 0xc0, 0x80, "1"                      },
	{0x10, 0x01, 0xc0, 0x40, "2"                      },
	{0x10, 0x01, 0xc0, 0xc0, "3"                      },
	{0x10, 0x01, 0xc0, 0x00, "5"                      },

	// Dip 3
	{0   , 0xfe, 0   , 2   , "Cabinet"                },
	{0x11, 0x01, 0x01, 0x00, "Upright"                },
	{0x11, 0x01, 0x01, 0x01, "Cocktail"               },
	
	{0   , 0xfe, 0   , 8   , "Bonus Life"             },
	{0x11, 0x01, 0x0e, 0x0e, "10k  50k  50k"          },
	{0x11, 0x01, 0x0e, 0x0c, "10k  60k  60k"          },
	{0x11, 0x01, 0x0e, 0x0a, "20k  60k  60k"          },
	{0x11, 0x01, 0x0e, 0x08, "20k  70k  70k"          },
	{0x11, 0x01, 0x0e, 0x06, "30k  70k  70k"          },
	{0x11, 0x01, 0x0e, 0x04, "30k  80k  80k"          },
	{0x11, 0x01, 0x0e, 0x02, "40k 100k 100k"          },
	{0x11, 0x01, 0x0e, 0x00, "None"                   },
	
	{0   , 0xfe, 0   , 2   , "Demo Sounds"            },
	{0x11, 0x01, 0x10, 0x00, "Off"                    },
	{0x11, 0x01, 0x10, 0x10, "On"                     },

	{0   , 0xfe, 0   , 2   , "Demo Music"             },
	{0x11, 0x01, 0x20, 0x00, "Off"                    },
	{0x11, 0x01, 0x20, 0x20, "On"                     },

	{0   , 0xfe, 0   , 2   , "Flip Screen"            },
	{0x11, 0x01, 0x40, 0x40, "Off"                    },
	{0x11, 0x01, 0x40, 0x00, "On"                     },
};

STDDIPINFO(Drv)

// Pirate Ship Higemaru

/*static*/ struct BurnRomInfo higemaruRomDesc[] = {
	{ "hg4.p12", 	0x2000, 0xdc67a7f9, 1 | BRF_PRG | BRF_ESS }, //  0 Z80 Code
	{ "hg5.m12", 	0x2000, 0xf65a4b68, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "hg6.p11", 	0x2000, 0x5f5296aa, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "hg7.m11", 	0x2000, 0xdc5d455d, 1 | BRF_PRG | BRF_ESS }, //  3

	{ "hg3.m1", 	0x2000, 0xb37b88c8, 2 | BRF_GRA }, 	     //	 4 Characters

	{ "hg1.c14", 	0x2000, 0xef4c2f5d, 3 | BRF_GRA },	     //	 5 Sprites
	{ "hg2.e14", 	0x2000, 0x9133f804, 3 | BRF_GRA },	     //  6

	{ "hgb3.l6", 	0x0020, 0x629cebd8, 4 | BRF_GRA },	     //	 7 Color Proms
	{ "hgb5.m4", 	0x0100, 0xdbaa4443, 4 | BRF_GRA },	     //	 8
	{ "hgb1.h7", 	0x0100, 0x07c607ce, 4 | BRF_GRA },	     //	 9

	{ "hgb4.l9", 	0x0100, 0x712ac508, 0 | BRF_OPT },	     //	10 Misc. Proms (not used)
	{ "hgb2.k7", 	0x0100, 0x4921635c, 0 | BRF_OPT },	     //	11
};

STD_ROM_PICK(higemaru)
STD_ROM_FN(higemaru)
#endif