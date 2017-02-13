#ifndef _D_MSX_H_
#define _D_MSX_H_

//#include "tiles_generic.h"
#include "burnint.h"
#include "saturn/ovl.h"
#include "z80_intf.h"
#include "driver.h"
#include "tms9928a.h"
#include "8255ppi.h"
#include "bitswap.h"
//#include "k051649.h"
//#include "dac.h"

#ifndef BUILD_WIN32
INT32 nReplayExternalDataCount = 0;
UINT8 *ReplayExternalData = NULL;
#endif

#include "snd/ay8910.h"

#define nBurnSoundLen 128

int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));

static int DrvFrame();
static int DrvExit();
static int DrvInit();
static void DrvInitSaturn();

static INT16 *pAY8910Buffer[6];

static UINT8 *AllMem	= NULL;
static UINT8 *MemEnd	= NULL;
static UINT8 *AllRam	= NULL;
static UINT8 *RamEnd	= NULL;
static UINT8 *maincpu	= NULL; // msx bios rom
static UINT8 *game      = NULL; // game cart rom, tape side A
static UINT8 *game2     = NULL; // tape side B
static UINT8 *main_mem	= NULL;
static UINT8 *kanji_rom = NULL;
static UINT8 *game_sram = NULL;

static UINT8 *curtape   = NULL; // pointer(only) to currently inserted tape.
static INT32 curtapelen = 0;

static UINT8 use_kanji     = 0;
static UINT8 msx_basicmode = 0;

static UINT8 DrvInputs[2];

static UINT8 DrvJoy1[8];
static UINT8 DrvJoy2[8];
static UINT8 DrvJoy4[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static UINT8 DrvDips[1];
static UINT8 DrvReset;
static UINT8 DrvNMI = 0;

static struct BurnInputInfo MSXInputList[] = {
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"},
};

STDINPUTINFO(MSX)

static struct BurnDIPInfo MSXDIPList[]=
{
	//{0x17, 0xff, 0xff, 0x10, NULL		},

	{0   , 0xfe, 0   ,    2, "BIOS - NOTE: Changes require re-start!"	},
	{0x17, 0x01, 0x01, 0x00, "Normal (REQ: msx.rom)"	},
	{0x17, 0x01, 0x01, 0x01, "Japanese (REQ: msxj.rom, kanji.rom" },

	{0   , 0xfe, 0   ,    2, "Hertz"	},
	{0x17, 0x01, 0x10, 0x00, "50hz (Europe)"	},
	{0x17, 0x01, 0x10, 0x10, "60hz (Japan, US)"	},

	{0   , 0xfe, 0   ,    2, "Key-Clicker / 1-Bit DAC"	},
	{0x17, 0x01, 0x02, 0x00, "Off"	},
	{0x17, 0x01, 0x02, 0x02, "On"	},

	{0   , 0xfe, 0   ,    2, "Swap Joyports"	},
	{0x17, 0x01, 0x20, 0x00, "Normal"	},
	{0x17, 0x01, 0x20, 0x20, "Swapped"	},

	{0   , 0xfe, 0   ,    2, "Map Cursor Keys to Joy #1"	},
	{0x17, 0x01, 0x80, 0x00, "Off"	},
	{0x17, 0x01, 0x80, 0x80, "On"	},

	{0   , 0xfe, 0   ,    2, "Tape Side"	},
	{0x17, 0x01, 0x40, 0x00, "Side A"	},
	{0x17, 0x01, 0x40, 0x40, "Side B"	},
};

static struct BurnDIPInfo MSXDefaultDIPList[]=
{
	{0x17, 0xff, 0xff, 0x10, NULL		},
};

static struct BurnDIPInfo MSXJBIOSDIPList[]=
{
	{0x17, 0xff, 0xff, 0x11, NULL		},
};

static struct BurnDIPInfo MSX50hzJoySwapDIPList[]=
{
	{0x17, 0xff, 0xff, 0x20, NULL		},
};

static struct BurnDIPInfo MSX50hzDIPList[]=
{
	{0x17, 0xff, 0xff, 0x20, NULL		},
};

static struct BurnDIPInfo MSXJoySwapDIPList[]=
{
	{0x17, 0xff, 0xff, 0x30, NULL		},
};

static struct BurnDIPInfo MSXMapCursorToJoy1DIPList[]=
{
	{0x17, 0xff, 0xff, 0x80, NULL		},
};

static struct BurnDIPInfo MSXMapCursorToJoy1_60hzDIPList[]=
{
	{0x17, 0xff, 0xff, 0x80+0x10, NULL		},
};

static struct BurnDIPInfo MSXKeyClickerDACDIPList[]=
{
	{0x17, 0xff, 0xff, 0x02, NULL		},
};

STDDIPINFOEXT(MSX, MSXDefault, MSX)
STDDIPINFOEXT(MSXJapan, MSXJBIOS, MSX)
STDDIPINFOEXT(MSXJoyport2, MSXJoySwap, MSX)
STDDIPINFOEXT(MSXEuropeJoyport2, MSX50hzJoySwap, MSX)
STDDIPINFOEXT(MSXEurope, MSX50hz, MSX)
STDDIPINFOEXT(MSXJoyCursor, MSXMapCursorToJoy1, MSX)
STDDIPINFOEXT(MSXJoyCursor60hz, MSXMapCursorToJoy1_60hz, MSX)
STDDIPINFOEXT(MSXKeyClick, MSXKeyClickerDAC, MSX)

// ROM mapper types:
#define MAP_KONGEN8  0      // KonamiGeneric 8k
#define MAP_KONGEN16 1      // KonamiGeneric 16k
#define MAP_KONAMI5  2      // Konami SCC
#define MAP_KONAMI4  3      // Konami
#define MAP_ASCII8   4      // ASCII 8
#define MAP_ASCII16  5      // ASCII 16
#define MAP_DOOLY    6      // Dooly
#define MAP_CROSSBL  7      // Cross Blaim
#define MAP_RTYPE    8      // R-Type

#define MAXSLOTS    4
#define MAXMAPPERS  9

#define MAX_MSX_CARTSIZE 0x200000

// Machine Config
#define DEFAULT_BIOSSLOT 0
#define DEFAULT_CARTSLOTA 1
#define DEFAULT_CARTSLOTB 2
#define DEFAULT_RAMSLOT 3

static UINT8 SwapJoyports = 0; // Swap Joyport DIP
static UINT8 Joyselect = 0;    // read from Joystick 0 or 1? (internal)
static UINT8 Hertz60 = 0;      // DIP setting.
static UINT8 BiosmodeJapan = 0;// DIP setting.

// Game-based kludges
static INT32 MapCursorToJoy1 = 0; // Map Cursor Keys & space to Joy1

static INT32 VBlankKludge = 0; // For VoidRunner (joystick selection hangs)
static INT32 SwapRamslot = 0; // For Toshiba-EMI's Break Out!
static INT32 SwapButton2 = 0; // Swaps Joy button#2 with 'm', for Xenon and Astro Marine Corps
static INT32 SwapSlash = 0; // For Square Dancer, swaps the / key with the Japanese Underscore key

static INT32 CASMode = 0;      // Using .cas file?
static INT32 CASPos = 0;       // Internal tape position counter
static INT32 CASFrameCounter = 0; // for autoloading
static INT32 CASSide = 0; // Tape Side, 0 = A, 1 = B
static INT32 CASSideLast = 0; // used for detecting side changes

static INT32 BIOSSLOT = 0;      // Machine slot configuration
static INT32 CARTSLOTA = 0;
static INT32 CARTSLOTB = 0;
static INT32 RAMSLOT = 0;

static UINT8 *RAM[8];          // Mapped address space
static UINT8 *EmptyRAM = NULL; // Unmapped stuff points here
static UINT8 *MemMap[4][8];    // [prislot] [page]

static UINT8 *RAMData;         // main flat-chunk of ram
static UINT8 RAMMapper[4];
static UINT8 RAMMask;
static INT32 RAMPages = 4;

static UINT8 *SRAMData[MAXSLOTS]; // ascii8/16 sram

static UINT8 *ROMData[MAXSLOTS];  // flat chunk of cart-rom
static UINT8 ROMMapper[MAXSLOTS][4];
static UINT8 ROMMask[MAXSLOTS];
static UINT8 ROMType[MAXSLOTS];

// Game-specific mappers
static UINT8 dooly_prot;
static UINT8 crossblaim_selected_bank;
static UINT8 *crossblaim_bank_base[4];
static UINT8 rtype_selected_bank;
static UINT8 *rtype_bank_base[2];

static INT32 CurRomSizeA = 0; // cart A
static INT32 CurRomSizeB = 0; // cart B

static UINT8 WriteMode[MAXSLOTS];
static UINT8 PSL[MAXSLOTS]; // primary slot list
static UINT8 PSLReg; // primary slot register
static UINT8 SCCReg[MAXSLOTS]; // Konami-scc enable register

static UINT8 Kana, KanaByte; // Kanji-rom stuff

static UINT8 ppiC_row;
static UINT8 keyRows[12];
static INT32 charMatrix[][3] = {
	{'0', 0, 0}, {')', 0, 0}, {'1', 0, 1}, {'!', 0, 1}, {'2', 0, 2}, {'@', 0, 2},
	{'3', 0, 3}, {'#', 0, 3}, {'4', 0, 4}, {'$', 0, 4}, {'5', 0, 5}, {'%', 0, 5},
	{'6', 0, 6}, {'^', 0, 6}, {'7', 0, 7}, {'&', 0, 7},

	{'8', 1, 0}, {'*', 1, 0}, {'9', 1, 1}, {'(', 1, 1}, {'-', 1, 2}, {'_', 1, 2},
	{'=', 1, 3}, {'+', 1, 3}, {'\\', 1, 4}, {'|', 1, 4}, {'[', 1, 5}, {'{', 1, 5},
	{']', 1, 6}, {'}', 1, 6}, {';', 1, 7}, {':', 1, 7},

	{'A', 2, 6}, {'B', 2, 7}, {'a', 2, 6}, {'b', 2, 7},
	{'/', 2, 4}, {'?', 2, 4}, {'.', 2, 3}, {'>', 2, 3},
	{',', 2, 2}, {'<', 2, 2}, {'`', 2, 1}, {'~', 2, 1},
	{'\'', 2, 0}, {'"', 2, 0},

	{'C', 3, 0}, {'D', 3, 1}, {'E', 3, 2}, {'F', 3, 3}, {'G', 3, 4}, {'H', 3, 5},
	{'I', 3, 6}, {'J', 3, 7},
	{'c', 3, 0}, {'d', 3, 1}, {'e', 3, 2}, {'f', 3, 3}, {'g', 3, 4}, {'h', 3, 5},
	{'i', 3, 6}, {'j', 3, 7},

	{'K', 4, 0}, {'L', 4, 1}, {'M', 4, 2}, {'N', 4, 3}, {'O', 4, 4}, {'P', 4, 5},
	{'Q', 4, 6}, {'R', 4, 7},
	{'k', 4, 0}, {'l', 4, 1}, {'m', 4, 2}, {'n', 4, 3}, {'o', 4, 4}, {'p', 4, 5},
	{'q', 4, 6}, {'r', 4, 7},

	{'S', 5, 0}, {'T', 5, 1}, {'U', 5, 2}, {'V', 5, 3}, {'W', 5, 4}, {'X', 5, 5},
	{'Y', 5, 6}, {'Z', 5, 7},
	{'s', 5, 0}, {'t', 5, 1}, {'u', 5, 2}, {'v', 5, 3}, {'w', 5, 4}, {'x', 5, 5},
	{'y', 5, 6}, {'z', 5, 7}, 
	{/*VK_RETURN, 	  */0x0d,  7, 7},
	{/*VK_ESCAPE, 	  */0x1b,  7, 2},
	{/*VK_SPACE,	  */' ',   8, 0},
	{/*VK_F1, 		  */0xf1,  6, 5},
	{/*VK_F2, 		  */0xf2,  6, 6},
	{/*VK_F3, 		  */0xf3,  6, 7},
	{/*VK_F4, 		  */0xf4,  7, 0},
	{/*VK_F5,		  */0xf5,  7, 1},
	{/*VK_F6, 		  */0xf6,  7, 6},

	{/*J-SPADE        */0xe0,  2, 5},

	{/*VK_SHIFT       */0x10,  6, 0},
	{/*VK_CONTROL     */0x11,  6, 1},
	{/*VK_TAB         */0x09,  7, 3},
	//{VK_STOP, 	  7, 4},
	{/*VK_BACK        */0x08,  7, 5},
	//{VK_HOME,	  8, 1},
	//{VK_INSERT,   8, 2},
	{/*VK_DELETE      */0x2e,  8, 3},
	{/*VK_UP,		  */0xf8,  8, 5},
	{/*VK_DOWN, 	  */0xf9,  8, 6},
	{/*VK_LEFT, 	  */0xfa,  8, 4},
	{/*VK_RIGHT, 	  */0xfb,  8, 7},
	{'\0', 0, 0} // NULL/END.
};

static struct BurnRomInfo emptyRomDesc[] = {
	{ "",                    0,          0, 0 },
};

// MSX1 BIOS
static struct BurnRomInfo msx_msxRomDesc[] = {
    { "msx.rom",     0x8000, 0xa317e6b4, BRF_BIOS }, // 0x80 - standard bios
    { "msxj.rom",    0x8000, 0x071135e0, BRF_BIOS | BRF_OPT }, // 0x81 - japanese bios
    { "kanji.rom",   0x40000, 0x1f6406fb, BRF_BIOS | BRF_OPT }, // 0x82 - kanji support
};

STD_ROM_PICK(msx_msx)
STD_ROM_FN(msx_msx)

// 1942 (Jpn)

static struct BurnRomInfo MSX_1942RomDesc[] = {
	{ "1942.rom",	0x20000, 0xa27787af, BRF_PRG | BRF_ESS },
};
STDROMPICKEXT(MSX_1942, MSX_1942, msx_msx)
STD_ROM_FN(MSX_1942)

#endif