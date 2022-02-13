#ifndef _D_NEWS_H_
#define _D_NEWS_H_

#include "burnint.h"
#include "snd/msm6295.h"
#include "saturn/ovl.h"
#include "czet.h"

//#include "saturn/sc_saturn.h"
//#include "psg.h"

#define nBurnSoundLen 128
#define SOUNDRATE   7680L

void dummy();
inline void initColors();
void NewsFrame();
int NewsInit();
int NewsExit();
void MSM6295RenderVBT(int nChip, short* pSoundBuf, int nSegmentLength);

Uint16 *cram_lut = NULL;//[4096];
UINT16 *map_offset_lut = NULL;//[0x400];
///*static*/ Uint16 *cram_lut;
#ifdef CACHE2
unsigned char 	*dirty_buffer = NULL;
//unsigned char 	*fg_dirtybuffer = NULL;
int	 			*MSM6295Context	= NULL;
#endif
extern unsigned char* MSM6295ROM;
extern int* pBuffer;
// Input Related Variables
unsigned char NewsInputPort0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
unsigned char NewsDip[1]        = {0};
unsigned char NewsInput[1]      = {0x00};
//unsigned char NewsReset         = 0;

// Memory Holders
//unsigned char *MemEnd           = NULL;
unsigned char *NewsRom          = NULL;
unsigned char *NewsRam          = NULL;
unsigned char *NewsFgVideoRam   = NULL;
unsigned char *NewsBgVideoRam   = NULL;
unsigned char *NewsPaletteRam   = NULL;
unsigned char *RamStart = NULL;
unsigned char *CZ80Context = NULL;

// Misc Variables, system control values, etc.
int BgPic = 1;

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

	{"Reset"             , BIT_DIGITAL  , NULL        , "reset"     },
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