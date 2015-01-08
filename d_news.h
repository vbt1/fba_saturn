#ifndef _D_NEWS_H_
#define _D_NEWS_H_

#include "burnint.h"
#include "msm6295.h"
#include "saturn/ovl.h"

//#include "saturn/sc_saturn.h"
//#include "psg.h"

#define SOUND_LEN 192

int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));
/*static*/ void initColors();
/*static*/ void DrvInitSaturn();
/*static*/ int NewsFrame();
/*static*/ int NewsInit();
/*static*/ int NewsExit();
int MSM6295RenderVBT(int nChip, short* pSoundBuf, int nSegmentLength);

static Uint16 cram_lut[4096];
static UINT16 map_offset_lut[0x400];
///*static*/ Uint16 *cram_lut;
#ifdef CACHE2
/*static*/ unsigned char 	bg_dirtybuffer[1024];
/*static*/ unsigned char 	fg_dirtybuffer[1024];
/*static*/ unsigned char	new_pic;
#endif
extern unsigned char* MSM6295ROM;
// Input Related Variables
static unsigned char NewsInputPort0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char NewsDip[1]        = {0};
static unsigned char NewsInput[1]      = {0x00};
static unsigned char NewsReset         = 0;

// Memory Holders
static unsigned char *Mem              = NULL;
static unsigned char *MemEnd           = NULL;
static unsigned char *NewsRom          = NULL;
static unsigned char *NewsRam          = NULL;
static unsigned char *NewsFgVideoRam   = NULL;
static unsigned char *NewsBgVideoRam   = NULL;
static unsigned char *NewsPaletteRam   = NULL;
static unsigned char *RamStart = NULL;
// Misc Variables, system control values, etc.
static int BgPic = 1;

// Dip Switch and Input Definitions
static struct BurnInputInfo NewsInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL  , NewsInputPort0 + 0, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , NewsInputPort0 + 1, "p1 start"  },

	{"Up"                , BIT_DIGITAL  , NewsInputPort0 + 2, "p1 up"     },
	{"Down"              , BIT_DIGITAL  , NewsInputPort0 + 3, "p1 down"   },
	{"Left"              , BIT_DIGITAL  , NewsInputPort0 + 4, "p1 left"   },
	{"Right"             , BIT_DIGITAL  , NewsInputPort0 + 5, "p1 right"  },
	{"Fire 1"            , BIT_DIGITAL  , NewsInputPort0 + 6, "p1 fire 1" },
	{"Fire 2"            , BIT_DIGITAL  , NewsInputPort0 + 7, "p1 fire 2" },

	{"Reset"             , BIT_DIGITAL  , &NewsReset        , "reset"     },
	{"Dip 1"             , BIT_DIPSWITCH, NewsDip + 0       , "dip"       },
};

STDINPUTINFO(News)

static struct BurnDIPInfo NewsDIPList[]=
{
	// Default Values
	{0x09, 0xff, 0xff, 0xdf, NULL                     },

	// Dip 1
	{0   , 0xfe, 0   , 4   , "Coinage"                },
	{0x09, 0x01, 0x03, 0x00, "3 Coins 1 Credit"       },
	{0x09, 0x01, 0x03, 0x01, "2 Coins 1 Credit"       },
	{0x09, 0x01, 0x03, 0x03, "1 Coin  1 Credit"       },
	{0x09, 0x01, 0x03, 0x02, "1 Coin  2 Credits"      },

	{0   , 0xfe, 0   , 4   , "Difficulty"             },
	{0x09, 0x01, 0x0c, 0x0c, "Easy"                   },
	{0x09, 0x01, 0x0c, 0x08, "Medium"                 },
	{0x09, 0x01, 0x0c, 0x04, "Hard"                   },
	{0x09, 0x01, 0x0c, 0x00, "Hardest"                },

	{0   , 0xfe, 0   , 2   , "Helps"                  },
	{0x09, 0x01, 0x10, 0x10, "1"                      },
	{0x09, 0x01, 0x10, 0x00, "2"                      },

	{0   , 0xfe, 0   , 2   , "Copyright"              },
	{0x09, 0x01, 0x20, 0x00, "Poby"                   },
	{0x09, 0x01, 0x20, 0x20, "Virus"                  },
};

STDDIPINFO(News)

static struct BurnDIPInfo NewsaDIPList[]=
{
	// Default Values
	{0x09, 0xff, 0xff, 0xdf, NULL                     },

	// Dip 1
	{0   , 0xfe, 0   , 4   , "Coinage"                },
	{0x09, 0x01, 0x03, 0x00, "3 Coins 1 Credit"       },
	{0x09, 0x01, 0x03, 0x01, "2 Coins 1 Credit"       },
	{0x09, 0x01, 0x03, 0x03, "1 Coin  1 Credit"       },
	{0x09, 0x01, 0x03, 0x02, "1 Coin  2 Credits"      },

	{0   , 0xfe, 0   , 4   , "Difficulty"             },
	{0x09, 0x01, 0x0c, 0x0c, "Easy"                   },
	{0x09, 0x01, 0x0c, 0x08, "Medium"                 },
	{0x09, 0x01, 0x0c, 0x04, "Hard"                   },
	{0x09, 0x01, 0x0c, 0x00, "Hardest"                },

	{0   , 0xfe, 0   , 2   , "Helps"                  },
	{0x09, 0x01, 0x10, 0x10, "1"                      },
	{0x09, 0x01, 0x10, 0x00, "2"                      },
};

STDDIPINFO(Newsa)

// Rom Definitions
static struct BurnRomInfo NewsRomDesc[] = {
	{ "virus.4",       0x08000, 0xaa005dfb, BRF_ESS | BRF_PRG }, //  0	Z80 Program Code

	{ "virus.2",       0x40000, 0xb5af58d8, BRF_GRA },			 //  1	Tiles
	{ "virus.3",       0x40000, 0xa4b1c175, BRF_GRA },			 //  2	Tiles

	{ "virus.1",       0x40000, 0x41f5935a, BRF_SND },			 //  3	Samples
};


STD_ROM_PICK(News)
STD_ROM_FN(News)

static struct BurnRomInfo NewsaRomDesc[] = {
	{ "newsa.4", 0x10000, 0x74a257da, BRF_ESS | BRF_PRG }, //  0	Z80 Program Code

	{ "virus.2",       0x40000, 0xb5af58d8, BRF_GRA },			 //  1	Tiles
	{ "virus.3",       0x40000, 0xa4b1c175, BRF_GRA },			 //  2	Tiles

	{ "virus.1",       0x40000, 0x41f5935a, BRF_SND },			 //  3	Samples
};


STD_ROM_PICK(Newsa)
STD_ROM_FN(Newsa)

#endif