#ifndef _D_SYS1_COMMON_H_
#define _D_SYS1_COMMON_H_

#include "burnint.h"
#include "sn76496.h"
#include "bitswap.h"
#include "mc8123.h"
#include "raze/raze.h"
//#include "saturn/sc_saturn.h"
#include "saturn/ovl.h"

#define SOUND_LEN 240
#define nSegmentLength 	SOUND_LEN / 10

#define COLADDR (FBUF_ADDR-0x400)
#define COLADDR_SPR	(COLADDR>>3)
//#define SC_RELEASE 1

typedef void (*write_func)(unsigned short a, UINT8 d);
/*static*/ write_func p[36];

int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));
/*static*/ int System1CalcPalette();
/*static*/ int System1Exit();
/*static*/ int System1Frame();
/*static*/ UINT8 *SaturnMem = NULL;
/*static*/ UINT16 *remap8to16_lut = NULL;
/*static*/ UINT16 *map_offset_lut = NULL;
///*static*/ Uint16 *code_lut = NULL;
/*static*/ int *cpu_lut = NULL;
/*static*/ Uint16 *cram_lut = NULL;
/*static*/ Uint8 *width_lut = NULL;
/*static*/ UINT8 *ss_vram = NULL;
/*static*/ UINT16 *spriteCache = NULL;
typedef int bool;

//typedef struct { UINT8 x, y, width, yend } sprite_collision; 
//sprite_collision sprites_collision[32];
/*static*/ UINT16 nextSprite=0;
/*static*/ UINT8 flipscreen=0;
/*static*/ void make_cram_lut(void);
/*static*/ void updateCollisions(int *values);
void memcpyl(void *, void *, int);
void initScrolling(Uint8 enabled,void *address);
void drawWindow(unsigned  int l1,unsigned  int l2,unsigned  int l3,unsigned  int vertleft,unsigned  int vertright);
void rotate_tile(unsigned int size,unsigned char flip, unsigned char *target);
void DrawSprite(unsigned int Num,unsigned int Bank, unsigned int addr,UINT16 Skip,UINT8 *SpriteBase);
void DrawSpriteCache(unsigned int Num,unsigned int Bank, unsigned int addr,UINT16 Skip,UINT8 *SpriteBase);
void initSprites(int sx,int sy,int sx2, int sy2,int lx,int ly);
void *memset4_fast(void *, long, size_t);
void DrvInitSaturn();

/*static*/ UINT8 System1InputPort0[8]    = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/ UINT8 System1InputPort1[8]    = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/ UINT8 System1InputPort2[8]    = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/ UINT8 System1Dip[2]           = {0, 0};
/*static*/ UINT8 System1Input[3]         = {0x00, 0x00, 0x00 };
/*static*/ UINT8 System1Reset            = 0;

/*static*/ UINT8 *Mem                    = NULL;
/*static*/ UINT8 *MemEnd                 = NULL;
/*static*/ UINT8 *RamStart               = NULL;
/*static*/ UINT8 *RamStart1               = NULL;
/*static*/ UINT8 *System1Rom1            = NULL;
/*static*/ UINT8 *System1Rom2            = NULL;
/*static*/ UINT8 *System1PromRed         = NULL;
/*static*/ UINT8 *System1PromGreen       = NULL;
/*static*/ UINT8 *System1PromBlue        = NULL;
/*static*/ UINT8 *System1Ram1            = NULL;
/*static*/ UINT8 *System1Ram2            = NULL;
/*static*/ UINT8 *System1SpriteRam       = NULL;
/*static*/ UINT8 *System1PaletteRam      = NULL;
/*static*/ UINT8 *System1BgRam           = NULL;
/*static*/ UINT8 *System1VideoRam        = NULL;
/*static*/ UINT8 *System1ScrollXRam      = NULL;
/*static*/ UINT8 *System1BgCollisionRam  = NULL;
/*static*/ UINT8 *System1SprCollisionRam = NULL;
/*static*/ UINT8 *System1deRam           = NULL;
/*static*/ UINT8 *System1efRam           = NULL;
/*static*/ UINT8 *System1f4Ram           = NULL;
/*static*/ UINT8 *System1fcRam           = NULL;
/*static*/ UINT8 *System1Tiles           = NULL;
/*static*/ UINT8 *System1Sprites         = NULL;
/*static*/ UINT8 *SpriteOnScreenMap      = NULL;
/*static*/ UINT8 *System1Fetch1          = NULL;
/*static*/ //UINT8 *System1MC8123Key       = NULL;
/*static*/ UINT8  *System1ScrollX = NULL;
/*static*/ UINT8  *System1ScrollY = NULL;

/*static*/ int            System1BgScrollX = 0;
/*static*/ int            System1BgScrollY = 0;
/*static*/ int            System1VideoMode = 0;
/*static*/ int            System1FlipScreen = 0;
/*static*/ int            System1SoundLatch = 0;
/*static*/ int            System1RomBank = 0;
/*static*/ int            System1BankSwitch = 0;

/*static*/ UINT8 System1BgBankLatch;
/*static*/ UINT8 System1BgBank;

/*static*/ UINT8  BlockgalDial1;
/*static*/ UINT8  BlockgalDial2;

/*static*/ int System1SpriteRomSize;
/*static*/ int System1NumTiles;
/*static*/ int System1ColourProms = 0;
/*static*/ int System1BankedRom = 0;

typedef void (*Decode)();
/*static*/ Decode DecodeFunction;
/*static*/ void System1Render();

typedef void (*MakeInputs)();
/*static*/ MakeInputs MakeInputsFunction;

/*static*/ int nCyclesDone[2], nCyclesTotal[2];
/*static*/ int nCyclesSegment=0;

/*==============================================================================================
Input Definitions
===============================================================================================*/

/*static*/ struct BurnInputInfo BlockgalInputList[] = {
	{"Coin 1"            , BIT_DIGITAL  , System1InputPort2 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , System1InputPort2 + 4, "p1 start"  },
	{"Coin 2"            , BIT_DIGITAL  , System1InputPort2 + 1, "p2 coin"   },
	{"Start 2"           , BIT_DIGITAL  , System1InputPort2 + 5, "p2 start"  },

	{"P1 Left"           , BIT_DIGITAL  , System1InputPort0 + 0, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , System1InputPort0 + 1, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , System1InputPort2 + 6, "p1 fire 1" },

	{"P2 Left"           , BIT_DIGITAL  , System1InputPort0 + 2, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , System1InputPort0 + 3, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , System1InputPort2 + 7, "p2 fire 1" },

	{"Reset"             , BIT_DIGITAL  , &System1Reset        , "reset"     },
	{"Service"           , BIT_DIGITAL  , System1InputPort2 + 3, "service"   },
	{"Dip 1"             , BIT_DIPSWITCH, System1Dip + 0       , "dip"       },
	{"Dip 2"             , BIT_DIPSWITCH, System1Dip + 1       , "dip"       },
};

STDINPUTINFO(Blockgal)

/*static*/ struct BurnInputInfo MyheroInputList[] = {
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

STDINPUTINFO(Myhero)

#define SYSTEM1_COINAGE(dipval)								\
	{0   , 0xfe, 0   , 16   , "Coin A"                },				\
	{dipval, 0x01, 0x0f, 0x07, "4 Coins 1 Credit"       },				\
	{dipval, 0x01, 0x0f, 0x08, "3 Coins 1 Credit"       },				\
	{dipval, 0x01, 0x0f, 0x09, "2 Coins 1 Credit"       },				\
	{dipval, 0x01, 0x0f, 0x05, "2 Coins 1 Credit 4/2 5/3 6/4"},			\
	{dipval, 0x01, 0x0f, 0x04, "2 Coins 1 Credit 4/3"   },				\
	{dipval, 0x01, 0x0f, 0x0f, "1 Coin  1 Credit"       },				\
	{dipval, 0x01, 0x0f, 0x00, "1 Coin  1 Credit"       },				\
	{dipval, 0x01, 0x0f, 0x03, "1 Coin  1 Credit 5/6"   },				\
	{dipval, 0x01, 0x0f, 0x02, "1 Coin  1 Credit 4/5"   },				\
	{dipval, 0x01, 0x0f, 0x01, "1 Coin  1 Credit 2/3"   },				\
	{dipval, 0x01, 0x0f, 0x06, "2 Coins 3 Credits"      },				\
	{dipval, 0x01, 0x0f, 0x0e, "1 Coin  2 Credits"      },				\
	{dipval, 0x01, 0x0f, 0x0d, "1 Coin  3 Credits"      },				\
	{dipval, 0x01, 0x0f, 0x0c, "1 Coin  4 Credits"      },				\
	{dipval, 0x01, 0x0f, 0x0b, "1 Coin  5 Credits"      },				\
	{dipval, 0x01, 0x0f, 0x0a, "1 Coin  6 Credits"      },				\
											\
	{0   , 0xfe, 0   , 16   , "Coin B"                },				\
	{dipval, 0x01, 0xf0, 0x70, "4 Coins 1 Credit"       },				\
	{dipval, 0x01, 0xf0, 0x80, "3 Coins 1 Credit"       },				\
	{dipval, 0x01, 0xf0, 0x90, "2 Coins 1 Credit"       },				\
	{dipval, 0x01, 0xf0, 0x50, "2 Coins 1 Credit 4/2 5/3 6/4"},			\
	{dipval, 0x01, 0xf0, 0x40, "2 Coins 1 Credit 4/3"   },				\
	{dipval, 0x01, 0xf0, 0xf0, "1 Coin  1 Credit"       },				\
	{dipval, 0x01, 0xf0, 0x00, "1 Coin  1 Credit"       },				\
	{dipval, 0x01, 0xf0, 0x30, "1 Coin  1 Credit 5/6"   },				\
	{dipval, 0x01, 0xf0, 0x20, "1 Coin  1 Credit 4/5"   },				\
	{dipval, 0x01, 0xf0, 0x10, "1 Coin  1 Credit 2/3"   },				\
	{dipval, 0x01, 0xf0, 0x60, "2 Coins 3 Credits"      },				\
	{dipval, 0x01, 0xf0, 0xe0, "1 Coin  2 Credits"      },				\
	{dipval, 0x01, 0xf0, 0xd0, "1 Coin  3 Credits"      },				\
	{dipval, 0x01, 0xf0, 0xc0, "1 Coin  4 Credits"      },				\
	{dipval, 0x01, 0xf0, 0xb0, "1 Coin  5 Credits"      },				\
	{dipval, 0x01, 0xf0, 0xa0, "1 Coin  6 Credits"      },



#endif