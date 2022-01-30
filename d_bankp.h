#ifndef _D_BANKP_H_
#define _D_BANKP_H_

#include "snd/sn76496.h"
#include "burnint.h"
#include "saturn/ovl.h"
#include "raze/raze.h"
#define SOUND_LEN 256

//UINT8 *MemEnd = NULL;
UINT16 *map_offset_lut = NULL;
UINT8 *Rom = NULL, *Gfx0 = NULL, *Gfx1 = NULL, *Prom = NULL;
UINT32 *Palette = NULL;
UINT8 DrvJoy1[8] = {0,0,0,0,0,0,0,0};
UINT8 DrvJoy2[8] = {0,0,0,0,0,0,0,0};
UINT8 DrvJoy3[8] = {0,0,0,0,0,0,0,0};
UINT8 DrvInputs[3] = {0,0,0};
UINT8 DrvDips=0;
UINT8 /*scroll_x,*/ priority=0, flipscreen=0, interrupt_enable=0;

void rotate_tile(unsigned int size,unsigned char flip, unsigned char *target);
void init_32_colors(unsigned int *t_pal,unsigned char *color_prom);
unsigned char __fastcall bankp_in(unsigned short address);
inline void 	 bg_line(UINT16 offs);
inline void 	 fg_line(UINT16 offs);
void __fastcall bankp_write(unsigned short address, unsigned char data);
void __fastcall bankp_out(UINT16 address, UINT8 data);
void __fastcall bankp_write_f000(unsigned short address, unsigned char data);
void __fastcall bankp_write_f400(unsigned short address, unsigned char data);
void __fastcall bankp_write_f800(unsigned short address, unsigned char data);
void __fastcall bankp_write_fc00(unsigned short address, unsigned char data);

//void 	 bg_line(INT32 offs,INT32 flipx);
//void 	 fg_line(INT32 offs,INT32 flipx);

void bankp_palette_init();
static inline void bankp_gfx_decode();
void DrvDoReset();
INT32 DrvBpExit();
void DrvFrame();
INT32 DrvInit();
INT32 DrvChInit();
inline void MemIndex();

static struct BurnInputInfo bankpInputList[] = {
	{"P1 Coin"      , BIT_DIGITAL  , DrvJoy1 + 5,	"p1 coin"  },
	{"P1 Start"     , BIT_DIGITAL  , DrvJoy2 + 5,	"p1 start" },
	{"P1 Left"      , BIT_DIGITAL  , DrvJoy1 + 3, 	"p1 left"  },
	{"P1 Right"     , BIT_DIGITAL  , DrvJoy1 + 1, 	"p1 right" },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2"  , BIT_DIGITAL  , DrvJoy1 + 7,	"p1 fire 2"},
	{"P1 Button 3"  , BIT_DIGITAL  , DrvJoy3 + 0,	"p1 fire 3"},

	{"P2 Coin"      , BIT_DIGITAL  , DrvJoy1 + 6,	"p2 coin"  },
	{"P2 Start"     , BIT_DIGITAL  , DrvJoy2 + 6,	"p2 start" },
	{"P2 Left"      , BIT_DIGITAL  , DrvJoy2 + 3, 	"p2 left"  },
	{"P2 Right"     , BIT_DIGITAL  , DrvJoy2 + 1, 	"p2 right" },
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy2 + 4,	"p2 fire 1"},
	{"P2 Button 2"  , BIT_DIGITAL  , DrvJoy2 + 7,	"p2 fire 2"},
	{"P2 Button 3"  , BIT_DIGITAL  , DrvJoy3 + 1,	"p2 fire 3"},

	{"P3 Coin"      , BIT_DIGITAL  , DrvJoy3 + 2,	"p3 coin"  },

	{"Reset"        , BIT_DIGITAL  , NULL  ,	"reset"    },
	{"Dip 1"        , BIT_DIPSWITCH, &DrvDips,   "dip"      },
};

STDINPUTINFO(bankp)

static struct BurnInputInfo combhInputList[] = {
	{"P1 Coin"      , BIT_DIGITAL  , DrvJoy1 + 5,	"p1 coin"  },
	{"P1 Start"     , BIT_DIGITAL  , DrvJoy2 + 5,	"p1 start" },
	{"P1 Up"        , BIT_DIGITAL  , DrvJoy1 + 0, 	"p1 up"    },
	{"P1 Down"      , BIT_DIGITAL  , DrvJoy1 + 2, 	"p1 down"  },
	{"P1 Button 1"  , BIT_DIGITAL  , DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2"  , BIT_DIGITAL  , DrvJoy1 + 7,	"p1 fire 2"},
	{"P1 Button 3"  , BIT_DIGITAL  , DrvJoy3 + 0,	"p1 fire 3"},

	{"P2 Coin"      , BIT_DIGITAL  , DrvJoy1 + 6,	"p2 coin"  },
	{"P2 Start"     , BIT_DIGITAL  , DrvJoy2 + 6,	"p2 start" },
	{"P2 Up"        , BIT_DIGITAL  , DrvJoy2 + 0, 	"p2 up"    },
	{"P2 Down"      , BIT_DIGITAL  , DrvJoy2 + 2, 	"p2 down"  },
	{"P2 Button 1"  , BIT_DIGITAL  , DrvJoy2 + 4,	"p2 fire 1"},
	{"P2 Button 2"  , BIT_DIGITAL  , DrvJoy2 + 7,	"p2 fire 2"},
	{"P2 Button 3"  , BIT_DIGITAL  , DrvJoy3 + 1,	"p2 fire 3"},

	{"P3 Coin"      , BIT_DIGITAL  , DrvJoy3 + 2,	"p3 coin"  },

	{"Reset"        , BIT_DIGITAL  , NULL  ,	"reset"    },
	{"Dip 1"        , BIT_DIPSWITCH, &DrvDips,   "dip"      },

};

STDINPUTINFO(combh)

static struct BurnDIPInfo bankpDIPList[]=
{
	{0x10, 0xff, 0xff, 0xc0, NULL					},

	{0   , 0xfe, 0   , 4   , "Coin A/B"				},
	{0x10, 0x01, 0x03, 0x03, "3C 1C"				},
	{0x10, 0x01, 0x03, 0x02, "2C 1C"				},
	{0x10, 0x01, 0x03, 0x00, "1C 1C"				},
	{0x10, 0x01, 0x03, 0x01, "1C 2C"				},

	{0   , 0xfe, 0   , 2   , "Coin C"				},
	{0x10, 0x01, 0x04, 0x04, "2C 1C"				},
	{0x10, 0x01, 0x04, 0x00, "1C 1C"				},

	{0   , 0xfe, 0   , 2   , "Lives"				},
	{0x10, 0x01, 0x08, 0x00, "3"					},
	{0x10, 0x01, 0x08, 0x08, "4"					},

	{0   , 0xfe, 0   , 2   , "Bonus Life"	 		},
	{0x10, 0x01, 0x10, 0x00, "70K 200K 500K..."		},
	{0x10, 0x01, 0x10, 0x10, "100K 400K 800K..."	},

	{0   , 0xfe, 0   , 2   , "Difficulty"			},
	{0x10, 0x01, 0x20, 0x00, "Easy"					},
	{0x10, 0x01, 0x20, 0x20, "Hard"					},

	{0   , 0xfe, 0   , 2   , "Demo Sounds"			},
	{0x10, 0x01, 0x40, 0x00, "Off"					},
	{0x10, 0x01, 0x40, 0x40, "On"					},

	{0   , 0xfe, 0   , 2   , "Cabinet"				},
	{0x10, 0x01, 0x80, 0x80, "Upright"				},
	{0x10, 0x01, 0x80, 0x00, "Cocktail"				},
};

STDDIPINFO(bankp)

static struct BurnDIPInfo combhDIPList[]=
{
	{0x10, 0xff, 0xff, 0x10, NULL					},

	{0   , 0xfe, 0   , 2   , "Flip Screen"			},
	{0x10, 0x01, 0x01, 0x00, "Off"					},
	{0x10, 0x01, 0x01, 0x01, "On"					},

	{0   , 0xfe, 0   , 4   , "Coinage"				},
	{0x10, 0x01, 0x06, 0x06, "2C 1C"				},
	{0x10, 0x01, 0x06, 0x00, "1C 1C"				},
	{0x10, 0x01, 0x06, 0x02, "1C 2C"				},
	{0x10, 0x01, 0x06, 0x04, "1C 3C"				},

	{0   , 0xfe, 0   , 2   , "Lives"				},
	{0x10, 0x01, 0x08, 0x00, "3"					},
	{0x10, 0x01, 0x08, 0x08, "4"					},

	{0   , 0xfe, 0   , 2   , "Cabinet"				},
	{0x10, 0x01, 0x10, 0x10, "Upright" 				},
	{0x10, 0x01, 0x10, 0x00, "Cocktail"				},

	{0   , 0xfe, 0   , 2   , "Difficulty"			},
	{0x10, 0x01, 0x40, 0x00, "Easy"					},
	{0x10, 0x01, 0x40, 0x40, "Hard"					},

	{0   , 0xfe, 0   , 2   , "Fuel"					},
	{0x10, 0x01, 0x80, 0x00, "120 Units" 			},
	{0x10, 0x01, 0x80, 0x80, "90 Units"				},
};

STDDIPINFO(combh)
//int ovlInit(char *szShortName);
//extern int DrvInit();

// Bank Panic

static struct BurnRomInfo bankpRomDesc[] = {
	{ "epr6175.bin",       0x4000, 0x044552b8, 1 | BRF_ESS | BRF_PRG },    //  0 Z80 Code	
	{ "epr6174.bin",       0x4000, 0xd29b1598, 1 | BRF_ESS | BRF_PRG },    //  1	
	{ "epr6173.bin",       0x4000, 0xb8405d38, 1 | BRF_ESS | BRF_PRG },    //  2	
	{ "epr6176.bin",       0x2000, 0xc98ac200, 1 | BRF_ESS | BRF_PRG },    //  3	

	{ "epr6165.bin",       0x2000, 0xaef34a93, 2 | BRF_GRA },	       //  4 Playfield #1 chars
	{ "epr6166.bin",       0x2000, 0xca13cb11, 2 | BRF_GRA },	       //  5

	{ "epr6172.bin",       0x2000, 0xc4c4878b, 3 | BRF_GRA },	       //  6 Playfield #2 chars
	{ "epr6171.bin",       0x2000, 0xa18165a1, 3 | BRF_GRA },	       //  7
	{ "epr6170.bin",       0x2000, 0xb58aa8fa, 3 | BRF_GRA },	       //  8
	{ "epr6169.bin",       0x2000, 0x1aa37fce, 3 | BRF_GRA },	       //  9
	{ "epr6168.bin",       0x2000, 0x05f3a867, 3 | BRF_GRA },	       // 10
	{ "epr6167.bin",       0x2000, 0x3fa337e1, 3 | BRF_GRA },	       // 11

	{ "pr6177.clr",        0x0020, 0xeb70c5ae, 4 | BRF_GRA },	       // 12 (unsigned int*)Palette
	{ "pr6178.clr",        0x0100, 0x0acca001, 4 | BRF_GRA },	       // 13 Charset #1 lut
	{ "pr6179.clr",        0x0100, 0xe53bafdb, 4 | BRF_GRA },	       // 14 Charset #2 lut

	{ "3155074.2c",   0x025b, 0x2e57bbba, 0 | BRF_OPT },	       // 15
//	{ "3155073.pal16l4",  0x0001, 0x00000000, 0 | BRF_OPT | BRF_NODUMP }, // 16 read protected
};

STD_ROM_PICK(bankp)
STD_ROM_FN(bankp)

// Combat Hawk

static struct BurnRomInfo combhRomDesc[] = {
	{ "epr10904.7e",      0x4000, 0x4b106335, 1 | BRF_ESS | BRF_PRG },    //  0 Z80 Code	
	{ "epr10905.7f",      0x4000, 0xa76fc390, 1 | BRF_ESS | BRF_PRG },    //  1	
	{ "epr10906.7h",      0x4000, 0x16d54885, 1 | BRF_ESS | BRF_PRG },    //  2	
	{ "epr10903.7d",      0x2000, 0xb7a59cab, 1 | BRF_ESS | BRF_PRG },    //  3	

	{ "epr10914.5l",      0x2000, 0x7d7a2340, 2 | BRF_GRA },	       //  4 Playfield #1 chars
	{ "epr10913.5k",      0x2000, 0xd5c1a8ae, 2 | BRF_GRA },	       //  5

	{ "epr10907.5b",      0x2000, 0x08e5eea3, 3 | BRF_GRA },	       //  6 Playfield #2 chars
	{ "epr10908.5d",      0x2000, 0xd9e413f5, 3 | BRF_GRA },	       //  7
	{ "epr10909.5e",      0x2000, 0xfec7962c, 3 | BRF_GRA },	       //  8
	{ "epr10910.5f",      0x2000, 0x33db0fa7, 3 | BRF_GRA },	       //  9
	{ "epr10911.5h",      0x2000, 0x565d9e6d, 3 | BRF_GRA },	       // 10
	{ "epr10912.5i",      0x2000, 0xcbe22738, 3 | BRF_GRA },	       // 11

	{ "pr10900.8a",       0x0020, 0xf95fcd66, 4 | BRF_GRA },	       // 12 (unsigned int*)Palette
	{ "pr10901.6f",       0x0100, 0x6fd981c8, 4 | BRF_GRA },	       // 13 Charset #1 lut
	{ "pr10902.5a",       0x0100, 0x84d6bded, 4 | BRF_GRA },	       // 14 Charset #2 lut

	{ "3155074.2c",   0x025b, 0x2e57bbba, 0 | BRF_OPT },	       // 15
//	{ "3155073.pal16l4",  0x0001, 0x00000000, 0 | BRF_OPT | BRF_NODUMP }, // 16 read protected
};

STD_ROM_PICK(combh)
STD_ROM_FN(combh)

#endif