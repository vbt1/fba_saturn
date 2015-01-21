#ifndef _D_MITCHELL_H_
#define _D_MITCHELL_H_

//#define SC_RELEASE 1
#include "burnint.h"
#include "eeprom.h"
#include "saturn/ovl.h"
//#include "saturn/sc_saturn.h"
//#include "burn_ym2413.h"
//#include "msm6295.h"
int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));
/*static*/ int DrvFrame();
/*static*/ int DrvExit();
/*static*/ int SpangInit();
/*static*/ int PangInit();
/*static*/ void DrvInitSaturn();
void initSprites(int sx,int sy,int sx2, int sy2,int lx,int ly);
static UINT16 charaddr_lut[0x0800];
static Uint16 cram_lut[4096];
///*static*/ Uint16 *cram_lut;
/*static*/ void make_lut(void);
/*static*/ void DrvCalcPalette();
/*static*/ //unsigned char 	bg_dirtybuffer[4096];
static UINT16 map_lut[256];
static UINT16 map_offset_lut[2048];
///*static*/ unsigned char 	*bg_dirtybuffer;
static unsigned char 	color_dirty = 0;

static unsigned char DrvInputPort0[8]  = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort1[8]  = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort2[8]  = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort3[8]  = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort4[8]  = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort5[8]  = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort6[8]  = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort7[8]  = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort8[8]  = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort9[8]  = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort10[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInputPort11[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static unsigned char DrvInput[12]      = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static unsigned char DrvDip[2]         = {0, 0};
static unsigned char DrvReset          = 0;
static short         DrvDial1          = 0;
static short         DrvDial2          = 0;

extern unsigned char* MSM6295ROM;

static unsigned char *Mem                 = NULL;
static unsigned char *MemEnd              = NULL;
static unsigned char *RamStart            = NULL;
static unsigned char *DrvZ80Rom           = NULL;
static unsigned char *DrvZ80Code          = NULL;
static unsigned char *DrvSoundRom         = NULL;
static unsigned char *DrvZ80Ram           = NULL;
static unsigned char *DrvPaletteRam       = NULL;
static unsigned char *DrvAttrRam          = NULL;
static unsigned char *DrvVideoRam         = NULL;
static unsigned char *DrvSpriteRam        = NULL;
static unsigned char *DrvChars            = NULL;
static unsigned char *DrvSprites          = NULL;
static int nCyclesDone = 0; //, nCyclesTotal;
/*static*/ //int nCyclesSegment;

static unsigned char DrvRomBank;
static unsigned char DrvPaletteRamBank;
static unsigned char DrvOkiBank;
static unsigned char DrvFlipScreen;
static unsigned char DrvVideoBank;
static unsigned char DrvInput5Toggle;
static unsigned char DrvPort5Kludge;
static int DrvTileMask;
static unsigned char DrvHasEEPROM;
static int DrvNumColours;
static int DrvNVRamSize;
static int DrvNVRamAddress;
static unsigned char DrvDialSelected;
static int DrvDial[2];
static unsigned char DrvSoundLatch;

static unsigned char DrvInputType;

#define DRV_INPUT_TYPE_BLOCK		2

static struct BurnInputInfo PangInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL  , DrvInputPort0  + 7, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , DrvInputPort0  + 3, "p1 start"  },
	{"Start 2"           , BIT_DIGITAL  , DrvInputPort0  + 1, "p2 start"  },

	{"P1 Up"             , BIT_DIGITAL  , DrvInputPort1  + 7, "p1 up"     },
	{"P1 Down"           , BIT_DIGITAL  , DrvInputPort1  + 6, "p1 down"   },
	{"P1 Left"           , BIT_DIGITAL  , DrvInputPort1  + 5, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , DrvInputPort1  + 4, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , DrvInputPort1  + 3, "p1 fire 1" },
	{"P1 Fire 2"         , BIT_DIGITAL  , DrvInputPort1  + 2, "p1 fire 2" },
	
	{"P2 Up"             , BIT_DIGITAL  , DrvInputPort2  + 7, "p2 up"     },
	{"P2 Down"           , BIT_DIGITAL  , DrvInputPort2  + 6, "p2 down"   },
	{"P2 Left"           , BIT_DIGITAL  , DrvInputPort2  + 5, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , DrvInputPort2  + 4, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , DrvInputPort2  + 3, "p2 fire 1" },
	{"P2 Fire 2"         , BIT_DIGITAL  , DrvInputPort2  + 2, "p2 fire 2" },

	{"Reset"             , BIT_DIGITAL  , &DrvReset         , "reset"     },
	{"Service"           , BIT_DIGITAL  , DrvInputPort0  + 6, "service"   },
	{"Diagnostics"       , BIT_DIGITAL  , DrvInputPort3  + 1, "diag"      },
};

STDINPUTINFO(Pang)

static struct BurnInputInfo BlockInputList[] =
{
	{"Coin 1"            , BIT_DIGITAL  , DrvInputPort0  + 7, "p1 coin"   },
	{"Start 1"           , BIT_DIGITAL  , DrvInputPort0  + 3, "p1 start"  },
	{"Start 2"           , BIT_DIGITAL  , DrvInputPort0  + 1, "p2 start"  },

	{"P1 Left"           , BIT_DIGITAL  , DrvInputPort11 + 0, "p1 left"   },
	{"P1 Right"          , BIT_DIGITAL  , DrvInputPort11 + 1, "p1 right"  },
	{"P1 Fire 1"         , BIT_DIGITAL  , DrvInputPort1  + 7, "p1 fire 1" },
		
	{"P2 Left"           , BIT_DIGITAL  , DrvInputPort11 + 2, "p2 left"   },
	{"P2 Right"          , BIT_DIGITAL  , DrvInputPort11 + 3, "p2 right"  },
	{"P2 Fire 1"         , BIT_DIGITAL  , DrvInputPort2  + 7, "p2 fire 1" },
	
	{"Reset"             , BIT_DIGITAL  , &DrvReset         , "reset"     },
	{"Service"           , BIT_DIGITAL  , DrvInputPort0  + 6, "service"   },
	{"Diagnostics"       , BIT_DIGITAL  , DrvInputPort3  + 1, "diag"      },
};

STDINPUTINFO(Block)

static struct BurnRomInfo PangRomDesc[] = {
	{ "pang6.bin",     0x08000, 0x68be52cd, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "pang7.bin",     0x20000, 0x4a2e70f6, BRF_ESS | BRF_PRG }, //	 1
	
	{ "pang09.bin",   0x20000, 0x3a5883f5, BRF_GRA },	     //  2	Characters
	{ "bb3.bin",       0x20000, 0x79a8ed08, BRF_GRA },	     //  3
	{ "pang11.bin",   0x20000, 0x166a16ae, BRF_GRA },	     //  4
	{ "bb5.bin",       0x20000, 0x2fb3db6c, BRF_GRA },	     //  5
	
	{ "bb10.bin",      0x20000, 0xfdba4f6e, BRF_GRA },	     //  6	Sprites
	{ "bb9.bin",       0x20000, 0x39f47a63, BRF_GRA },	     //  7
	
	{ "bb1.bin",       0x20000, 0xc52e5b8e, BRF_SND },	     //  8	Samples
};

STD_ROM_PICK(Pang)
STD_ROM_FN(Pang)

static struct BurnRomInfo SpangRomDesc[] = {
	{ "spe06.rom",    0x08000, 0x1af106fb, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "spe07.rom",    0x20000, 0x208b5f54, BRF_ESS | BRF_PRG }, //	 1
	{ "spe08.rom",    0x20000, 0x2bc03ade, BRF_ESS | BRF_PRG }, //	 2
	
	{ "spe02.rom",    0x20000, 0x63c9dfd2, BRF_GRA },	     //  3	Characters
	{ "03.bin",         0x20000, 0x3ae28bc1, BRF_GRA },	     //  4
	{ "spe04.rom",    0x20000, 0x9d7b225b, BRF_GRA },	     //  5
	{ "05.bin",         0x20000, 0x4a060884, BRF_GRA },	     //  6
	
	{ "spj102k.bin",  0x20000, 0xeedd0ade, BRF_GRA },	     //  7	Sprites
	{ "spj091k.bin",  0x20000, 0x04b41b75, BRF_GRA },	     //  8
	
	{ "spe01.rom",    0x20000, 0x2d19c133, BRF_SND },	     //  9	Samples
	
	{ "eeprom.bin", 0x80, 0xdeae1291, BRF_OPT },
};

STD_ROM_PICK(Spang)
STD_ROM_FN(Spang)

/*static*/ /*struct BurnRomInfo BlockRomDesc[] = {
	{ "ble_05.rom",    0x08000, 0xc12e7f4c, BRF_ESS | BRF_PRG }, //  0	Z80 #1 Program Code
	{ "ble_06.rom",    0x20000, 0xcdb13d55, BRF_ESS | BRF_PRG }, //	 1
	{ "ble_07.rom",    0x20000, 0x1d114f13, BRF_ESS | BRF_PRG }, //	 2
	
	{ "bl_08.rom",     0x20000, 0xaa0f4ff1, BRF_GRA },	     //  3	Characters
	{ "bl_09.rom",     0x20000, 0x6fa8c186, BRF_GRA },	     //  4
	{ "bl_18.rom",     0x20000, 0xc0acafaf, BRF_GRA },	     //  5
	{ "bl_19.rom",     0x20000, 0x1ae942f5, BRF_GRA },	     //  6
	
	{ "bl_16.rom",     0x20000, 0xfadcaff7, BRF_GRA },	     //  7	Sprites
	{ "bl_17.rom",     0x20000, 0x5f8cab42, BRF_GRA },	     //  8
	
	{ "bl_01.rom",     0x20000, 0xc2ec2abb, BRF_SND },	     //  9	Samples
};

STD_ROM_PICK(Block)
STD_ROM_FN(Block)
*/
static const eeprom_interface MitchellEEPROMIntf =
{
	6,
	16,
	"0110",
	"0101",
	"0111",
	0,
	0,
	0,
	0
};


#endif