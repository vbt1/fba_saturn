#ifndef _D_MSX_H_
#define _D_MSX_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "sega_int.h"

//#include "z80_intf.h"
#include "czet.h"
#include "tms9928a.h"
#include "8255ppi.h"
//#include "bitswap.h"
#include "snd/k051649.h"
//#include "dac.h"
#include "snd/ay8910.h"

#define nBurnSoundLen 128  // div par 2 car utilisation integer

extern unsigned char play;

INT32 DrvFrame();
int DrvExit();
INT32 BasicDrvInit();
int DrvInit();
void DrvDoReset();

void PCM_MeInit(void);
void PCM_MeStart(PcmHn hn);
void PCM_MeStop(PcmHn hn);
void *memset4_fast(void *, long, size_t);
void dummy();
INT32 GetFileSize(int file_id);

int file_id = 2;
int file_max = 0;
#ifdef RAZE
#include "raze\raze.h"
void __fastcall msx_write_konami4(UINT16 address, UINT8 data);
void __fastcall msx_write_konami4scc(UINT16 address, UINT8 data);
void __fastcall msx_write_scc(UINT16 address, UINT8 data);
void __fastcall msx_write_scc2(UINT16 address, UINT8 data);
void __fastcall msx_write_ascii8(UINT16 address, UINT8 data);
#endif
UINT8 msx_ppi8255_portB_read();
void msx_ppi8255_portA_write(UINT8 data);
void msx_ppi8255_portC_write(UINT8 data);
UINT8 ay8910portAread(UINT32 offset);
void ay8910portAwrite(UINT32 offset, UINT32 data);
void ay8910portBwrite(UINT32 offset, UINT32 data);

void InsertCart(UINT8 *cartbuf, INT32 cartsize, INT32 nSlot);
void PageMap(INT32 CartSlot, const char *cMap); //("0:0:0:0:0:0:0:0")
void MapMegaROM(UINT8 nSlot, UINT8 nPg0, UINT8 nPg1, UINT8 nPg2, UINT8 nPg3);

void SetSlot(UINT8 nSlot);
void setFetch(UINT32 I, UINT8 *ram);
void vdp_interrupt(INT32 state);

void __fastcall msx_write(UINT16 address, UINT8 data);

UINT8 __fastcall msx_read_port(UINT16 port);
void __fastcall msx_write_port(UINT16 port, UINT8 data);

void updateSlaveSound();
void updateSlaveSoundSCC();
void Set8PCM();


PcmHn 			pcm8[8] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};
#define	PCM_ADDR	((void*)0x25a20000)
#define	PCM_SIZE	(4096L*2)				/* 2.. */
#define SOUNDRATE   7680L //

typedef	struct	SysDevice	{
	Uint8	type;
	Uint8	size;
	Uint8	data[1];
} SysDevice;

typedef UINT16	trigger_t;

static UINT16 pad_asign[]={
PER_DGT_U,PER_DGT_D,PER_DGT_R,PER_DGT_L,PER_DGT_A,PER_DGT_B,
PER_DGT_C,PER_DGT_S,PER_DGT_X,PER_DGT_Y,PER_DGT_TR,PER_DGT_TL,
};

static trigger_t	pltrigger[2],pltriggerE[2];

#define	SZ_PERIPHERAL	20
typedef	UINT8	SysPeripheral[SZ_PERIPHERAL+2];

typedef	struct	SysPort	{
	UINT8			id;
	UINT8			connectable;
	SysPeripheral	*peripheral;
} SysPort;

static SysPort	*__port;

const SysDevice	*PER_GetDeviceR( const SysPort	*port, Uint32	n );
SysPort	*PER_OpenPort( void );
trigger_t	PER_GetTrigger( const SysDevice	*this );

 UINT16 *SCCMixerBuffer	= NULL;
 UINT16 *SCCMixerTable	= NULL;
#ifndef RAZE
 UINT8 *CZ80Context = NULL;
#endif
 UINT8 *TMSContext = NULL;
 UINT8 *AllMem	= NULL;
 //UINT8 *MemEnd	= NULL;
 UINT8 *AllRam	= NULL;
// UINT8 *RamEnd	= NULL;
 UINT8 *maincpu	= NULL; // msx bios rom
 UINT8 *game      = NULL; // game cart rom, tape side A
#ifdef CASSETTE
 UINT8 *game2     = NULL; // tape side B
#endif
 UINT8 *main_mem	= NULL;
#ifdef KANJI
 UINT8 *kanji_rom = NULL;
 UINT8 use_kanji     = 0;
#endif
 UINT8 *game_sram = NULL;
/*static*/// UINT8 *tmpbmp = NULL;

#ifdef CASSETTE
 UINT8 *curtape   = NULL; // pointer(only) to currently inserted tape.
 INT32 curtapelen = 0;
#endif
/*static*/	UINT8 stop 			= 0;
// UINT8 msx_basicmode = 0;

 UINT8 DrvInputs[2]={0,0};

 UINT8 DrvJoy1[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
 UINT8 DrvJoy2[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
 UINT8 DrvJoy4[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
 UINT8 DrvDips[1]={0};
/*static*///  UINT8 DrvReset = 0;
 UINT8 DrvNMI = 0;

struct BurnInputInfo MSXInputList[] = {
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"},
	{"P1 Right",	BIT_DIGITAL,	DrvJoy1 + 3,	"p1 right"},
	{"P1 Button 1",	BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2",	BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"},

	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 left"},
	{"P2 Right",	BIT_DIGITAL,	DrvJoy2 + 3,	"p2 right"},
	{"P2 Button 1",	BIT_DIGITAL,	DrvJoy2 + 4,	"p2 fire 1"},
	{"P2 Button 2",	BIT_DIGITAL,	DrvJoy2 + 5,	"p2 fire 2"},

	{"Key F1",	    BIT_DIGITAL,	DrvJoy4 + 3,	"p1 F1" },
	{"Key F2",	    BIT_DIGITAL,	DrvJoy4 + 4,	"p1 F2" },
	{"Key F3",	    BIT_DIGITAL,	DrvJoy4 + 5,	"p1 F3" },
	{"Key F4",	    BIT_DIGITAL,	DrvJoy4 + 6,	"p1 F4" },
	{"Key F5",	    BIT_DIGITAL,	DrvJoy4 + 7,	"p1 F5" },
	{"Key F6",	    BIT_DIGITAL,	DrvJoy4 + 8,	"p1 F6" },

	{"Key UP",	    BIT_DIGITAL,	DrvJoy4 + 9,	"p1 KEYUP" },
	{"Key DOWN",	BIT_DIGITAL,	DrvJoy4 + 10,	"p1 KEYDOWN" },
	{"Key LEFT",	BIT_DIGITAL,	DrvJoy4 + 11,	"p1 KEYLEFT" },
	{"Key RIGHT",	BIT_DIGITAL,	DrvJoy4 + 12,	"p1 KEYRIGHT" },

	{"Reset",		BIT_DIGITAL,	NULL,	    "reset"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"},
};

STDINPUTINFO(MSX)

struct BurnDIPInfo MSXDIPList[]=
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

struct BurnDIPInfo MSXDefaultDIPList[]=
{
	{0x17, 0xff, 0xff, 0x10, NULL		},
};

struct BurnDIPInfo MSXJBIOSDIPList[]=
{
	{0x17, 0xff, 0xff, 0x11, NULL		},
};

struct BurnDIPInfo MSX50hzJoySwapDIPList[]=
{
	{0x17, 0xff, 0xff, 0x20, NULL		},
};

struct BurnDIPInfo MSX50hzDIPList[]=
{
	{0x17, 0xff, 0xff, 0x20, NULL		},
};

struct BurnDIPInfo MSXJoySwapDIPList[]=
{
	{0x17, 0xff, 0xff, 0x30, NULL		},
};

struct BurnDIPInfo MSXMapCursorToJoy1DIPList[]=
{
	{0x17, 0xff, 0xff, 0x80, NULL		},
};

struct BurnDIPInfo MSXMapCursorToJoy1_60hzDIPList[]=
{
	{0x17, 0xff, 0xff, 0x80+0x10, NULL		},
};

struct BurnDIPInfo MSXKeyClickerDACDIPList[]=
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

//#define MAX_MSX_CARTSIZE 0x200000
#define MAX_MSX_CARTSIZE 0x100000

// Machine Config
#define DEFAULT_BIOSSLOT 0
#define DEFAULT_CARTSLOTA 1
#define DEFAULT_CARTSLOTB 2
#define DEFAULT_RAMSLOT 3

 UINT8 SwapJoyports = 0; // Swap Joyport DIP
 UINT8 Joyselect = 0;    // read from Joystick 0 or 1? (internal)
 UINT8 Hertz60 = 0;      // DIP setting.
 UINT8 BiosmodeJapan = 0;// DIP setting.

// Game-based kludges
//static INT32 MapCursorToJoy1 = 0; // Map Cursor Keys & space to Joy1

 INT32 VBlankKludge = 0; // For VoidRunner (joystick selection hangs)
#ifdef CASSETTE
 INT32 CASMode = 0;      // Using .cas file?
 INT32 CASPos = 0;       // Internal tape position counter
 INT32 CASFrameCounter = 0; // for autoloading
 INT32 CASSide = 0; // Tape Side, 0 = A, 1 = B
 INT32 CASSideLast = 0; // used for detecting side changes
#endif
 INT32 BIOSSLOT = 0;      // Machine slot configuration
 INT32 CARTSLOTA = 0;
 INT32 CARTSLOTB = 0;
 INT32 RAMSLOT = 0;

 UINT8 *RAM[8]={NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}; // Mapped address space
 UINT8 *EmptyRAM = NULL; // Unmapped stuff points here
 UINT8 *MemMap[4][8]={{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
								{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
								{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
								{NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}};    // [prislot] [page]

 UINT8 *RAMData = NULL;         // main flat-chunk of ram
 UINT8 RAMMapper[4]={0,0,0,0};
 UINT8 RAMMask = 0;
 INT32 RAMPages = 4;

 UINT8 *SRAMData[MAXSLOTS]={NULL,NULL,NULL,NULL}; // ascii8/16 sram

 UINT8 *ROMData[MAXSLOTS]={NULL,NULL,NULL,NULL};  // flat chunk of cart-rom
 UINT8 ROMMapper[MAXSLOTS][4]={{0,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}};
 UINT8 ROMMask[MAXSLOTS]={0,0,0,0};
 UINT8 ROMType[MAXSLOTS]={0,0,0,0};

// Game-specific mappers
 UINT8 dooly_prot = 0;
 UINT8 crossblaim_selected_bank = 0;
 UINT8 *crossblaim_bank_base[4]={NULL,NULL,NULL,NULL};
 UINT8 rtype_selected_bank = 0;
 UINT8 *rtype_bank_base[2]={NULL,NULL};

 INT32 CurRomSizeA = 0; // cart A
 INT32 CurRomSizeB = 0; // cart B

 UINT8 WriteMode[MAXSLOTS]={0,0,0,0};
 UINT8 PSL[MAXSLOTS]={0,0,0,0}; // primary slot list
 UINT8 PSLReg=0; // primary slot register
 UINT8 SCCReg[MAXSLOTS]={0,0,0,0}; // Konami-scc enable register

 UINT8 Kana=0, KanaByte=0; // Kanji-rom stuff

 UINT8 ppiC_row=0;
 UINT8 keyRows[12]={0,0,0,0,0,0,0,0,0,0,0,0};
#ifdef CASSETTE
INT32 charMatrix[][3] = {
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

struct BurnRomInfo emptyRomDesc[] = {
	{ "",                    0,          0, 0 },
};
#endif 

// MSX1 BIOS
struct BurnRomInfo msx_msxRomDesc[] = {
    { "msx.rom",     0x8000, 0xa317e6b4, BRF_BIOS }, // 0x80 - standard bios
    { "msxj.rom",    0x8000, 0x071135e0, BRF_BIOS | BRF_OPT }, // 0x81 - japanese bios
//    { "kanji.rom",   0x40000, 0x1f6406fb, BRF_BIOS | BRF_OPT }, // 0x82 - kanji support
};

STD_ROM_PICK(msx_msx)
STD_ROM_FN(msx_msx)

// 1942 (Jpn)

struct BurnRomInfo MSX_1942RomDesc[] = {
//	{ "1942.rom",	0x2000, 0xa27787af, BRF_PRG | BRF_ESS },
	{ "1942.rom",	0x20000, 0xa27787af, BRF_PRG | BRF_ESS },
    { "msx.rom",     0x8000, 0xa317e6b4, BRF_BIOS }, // 0x80 - standard bios
    { "msxj.rom",    0x8000, 0x071135e0, BRF_BIOS | BRF_OPT }, // 0x81 - japanese bios
    { "kanji.rom",   0x40000, 0x1f6406fb, BRF_BIOS | BRF_OPT }, // 0x82 - kanji support
};
STD_ROM_PICK(MSX_1942)
STD_ROM_FN(MSX_1942)

//STDROMPICKEXT(MSX_1942, MSX_1942, msx_msx)
//STD_ROM_FN(MSX_1942)

#endif