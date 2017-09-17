#ifndef _D_SEGAE_H_
#define _D_SEGAE_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "sn76496.h"
#include "bitswap.h"

#define nBurnSoundLen 128

static UINT16 *name_lut = NULL;
static UINT32 *bp_lut = NULL;//[0x10000];
static UINT16 *cram_lut = NULL;//[0x40];
static UINT16 *map_lut = NULL;
static UINT8 *CZ80Context = NULL;
UINT32 ntab;
UINT32 satb;

int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));
static INT32 DrvHangonJrInit();
static INT32 DrvTetrisInit();
static INT32 DrvTransfrmInit();
static INT32 DrvExit();
static INT32 DrvFrame();
static INT32 DrvDraw();
static void DrvInitSaturn();
static void make_lut();
static void update_bg(int index);

static UINT8 DrvJoy0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static UINT8 DrvJoy1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static UINT8 DrvJoy2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static UINT8 DrvJoy3[8] = {0, 0, 0, 0, 0, 0, 0, 0};
static UINT8 DrvJoy4[8] = {0, 0, 0, 0, 0, 0, 0, 0};

static UINT8 DrvInput[5];
static UINT8 DrvDip[2];
static UINT8 DrvReset;
static UINT8 DrvRecalc;

static INT32 DrvWheel = 0;
static INT32 DrvAccel = 0;

static INT32 nCyclesDone, nCyclesTotal;
static INT32 nCyclesSegment;

static UINT8 *AllMem;
static UINT8 *MemEnd;
static UINT8 *AllRam;
static UINT8 *RamEnd;
static UINT8 *DrvRAM;
static UINT8 *DrvMainROM;
//static UINT32 *DrvPalette;
//static UINT32 *Palette;
static UINT8 *cache_bitmap;

static UINT8 segae_8000bank;
static UINT8 port_fa_last;
static UINT8 rombank;

static UINT8 hintcount;			/* line interrupt counter, decreased each scanline */
UINT8 vintpending;
UINT8 hintpending;

//UINT8 m_port_select;
UINT8 currentLine = 0;

UINT8 leftcolumnblank = 0; // most games need this, except tetris

#define CHIPS 2							/* There are 2 VDP Chips */

UINT8  segae_vdp_cmdpart[CHIPS];		/* VDP Command Part Counter */
UINT16 segae_vdp_command[CHIPS];		/* VDP Command Word */

UINT8  segae_vdp_accessmode[CHIPS];		/* VDP Access Mode (VRAM, CRAM) */
UINT16 segae_vdp_accessaddr[CHIPS];		/* VDP Access Address */
UINT8  segae_vdp_readbuffer[CHIPS];		/* VDP Read Buffer */

UINT8 *segae_vdp_vram[CHIPS];			/* Pointer to VRAM */
UINT8 *segae_vdp_cram[CHIPS];			/* Pointer to the VDP's CRAM */
UINT8 *segae_vdp_regs[CHIPS];			/* Pointer to the VDP's Registers */

UINT8 segae_vdp_vrambank[CHIPS];		/* Current VRAM Bank number (from writes to Port 0xf7) */

static struct BurnInputInfo TransfrmInputList[] = {

	{"P1 Coin",		BIT_DIGITAL,	DrvJoy0 + 0,	"p1 coin"},
	{"P1 Start",	BIT_DIGITAL,	DrvJoy0 + 6,	"p1 start"},
	{"P2 Coin",		BIT_DIGITAL,	DrvJoy0 + 1,	"p2 coin"},
	{"P2 Start",	BIT_DIGITAL,	DrvJoy0 + 7,	"p2 start"},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 right"},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
	{"Service",		BIT_DIGITAL,	DrvJoy0 + 3,	"service"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDip + 0,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDip + 1,	"dip"},
};
STDINPUTINFO(Transfrm)


static struct BurnDIPInfo TransfrmDIPList[]=
{
	{0x0c, 0xff, 0xff, 0xff, NULL		},
	{0x0d, 0xff, 0xff, 0xfc, NULL		},

	{0   , 0xfe, 0   ,    2, "1 Player Only"		},
	{0x0d, 0x01, 0x01, 0x00, "Off"		},
	{0x0d, 0x01, 0x01, 0x01, "On"		},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x0d, 0x01, 0x02, 0x02, "Off"		},
	{0x0d, 0x01, 0x02, 0x00, "On"		},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x0d, 0x01, 0x0c, 0x0c, "3"		},
	{0x0d, 0x01, 0x0c, 0x08, "4"		},
	{0x0d, 0x01, 0x0c, 0x04, "5"		},
	{0x0d, 0x01, 0x0c, 0x00, "Infinite (Cheat)"		},

	{0   , 0xfe, 0   ,    4, "Bonus Life"		},
	{0x0d, 0x01, 0x30, 0x20, "10k, 30k, 50k and 70k"		},
	{0x0d, 0x01, 0x30, 0x30, "20k, 60k, 100k and 140k"		},
	{0x0d, 0x01, 0x30, 0x10, "30k, 80k, 130k and 180k"		},
	{0x0d, 0x01, 0x30, 0x00, "50k, 150k and 250k"		},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x0d, 0x01, 0xc0, 0x40, "Easy"		},
	{0x0d, 0x01, 0xc0, 0xc0, "Medium"		},
	{0x0d, 0x01, 0xc0, 0x80, "Hard"		},
	{0x0d, 0x01, 0xc0, 0x00, "Hardest"		},
};

STDDIPINFO(Transfrm)

#define A(a, b, c, d) {a, b, (UINT8*)(c), d}
static struct BurnInputInfo HangonjrInputList[] = {

    {"P1 Coin",     BIT_DIGITAL,    DrvJoy0 + 0,    "p1 coin"},
    {"P1 Start",    BIT_DIGITAL,    DrvJoy0 + 4,    "p1 start"},
	A("P1 Steering", BIT_ANALOG_REL, &DrvWheel,     "p1 x-axis"),
	A("P1 Accelerate", BIT_ANALOG_REL, &DrvAccel,   "p1 z-axis"),

	{"Reset",		BIT_DIGITAL,	&DrvReset,	    "reset"},
	{"Service",		BIT_DIGITAL,	DrvJoy0 + 3,	"service"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDip + 0,	    "dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDip + 1,	    "dip"},
};

STDINPUTINFO(Hangonjr)
#undef A


static struct BurnDIPInfo HangonjrDIPList[]=
{
	{0x06, 0xff, 0xff, 0xff, NULL		},
	{0x07, 0xff, 0xff, 0x04, NULL		},

	{0   , 0xfe, 0   ,    4, "Enemies"		},
	{0x06, 0x01, 0x06, 0x06, "Easy"		},
	{0x06, 0x01, 0x06, 0x04, "Medium"		},
	{0x06, 0x01, 0x06, 0x02, "Hard"		},
	{0x06, 0x01, 0x06, 0x00, "Hardest"		},

	{0   , 0xfe, 0   ,    4, "Time Adj."		},
	{0x06, 0x01, 0x18, 0x18, "Easy"		},
	{0x06, 0x01, 0x18, 0x10, "Medium"		},
	{0x06, 0x01, 0x18, 0x08, "Hard"		},
	{0x06, 0x01, 0x18, 0x00, "Hardest"		},

	{0   , 0xfe, 0   ,    2, "Service Mode (No Toggle)"		},
	{0x07, 0x01, 0x04, 0x04, "Off"		},
	{0x07, 0x01, 0x04, 0x00, "On"		},
};

STDDIPINFO(Hangonjr)

static struct BurnInputInfo TetrisseInputList[] = {

	{"P1 Coin",		BIT_DIGITAL,	DrvJoy0 + 0,	"p1 coin"},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy0 + 6,	"p1 start"},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 right"},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
	{"Service",		BIT_DIGITAL,	DrvJoy0 + 3,	"service"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDip + 0,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDip + 1,	"dip"},
};

STDINPUTINFO(Tetrisse)


static struct BurnDIPInfo TetrisseDIPList[]=
{
	{0x0a, 0xff, 0xff, 0x30, NULL		},
	{0x0b, 0xff, 0xff, 0x04, NULL		},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x0a, 0x01, 0x02, 0x02, "Off"		},
	{0x0a, 0x01, 0x02, 0x00, "On"		},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x0a, 0x01, 0x30, 0x20, "Easy"		},
	{0x0a, 0x01, 0x30, 0x30, "Normal"		},
	{0x0a, 0x01, 0x30, 0x10, "Hard"		},
	{0x0a, 0x01, 0x30, 0x00, "Hardest"		},

	{0   , 0xfe, 0   ,    2, "Service Mode (No Toggle)"		},
	{0x0b, 0x01, 0x04, 0x04, "Off"		},
	{0x0b, 0x01, 0x04, 0x00, "On"		},
};
STDDIPINFO(Tetrisse)

//-----------------------

// Hang-On Jr.

static struct BurnRomInfo hangonjrRomDesc[] = {
	{ "rom5.ic7",	0x8000, 0xd63925a7, 1 }, //  0 maincpu
	{ "rom4.ic5",	0x8000, 0xee3caab3, 1 }, //  1
	{ "rom3.ic4",	0x8000, 0xd2ba9bc9, 1 }, //  2
	{ "rom2.ic3",	0x8000, 0xe14da070, 1 }, //  3
	{ "rom1.ic2",	0x8000, 0x3810cbf5, 1 }, //  4
};

STD_ROM_PICK(hangonjr)
STD_ROM_FN(hangonjr)

//  Tetris (Japan, System E)

static struct BurnRomInfo TetrisseRomDesc[] = {

	{ "epr12213.7", 0x8000, 0xef3c7a38, BRF_ESS | BRF_PRG }, // 0 maincpu
	{ "epr12212.5", 0x8000, 0x28b550bf, BRF_ESS | BRF_PRG }, // 1
	{ "epr12211.4", 0x8000, 0x5aa114e9, BRF_ESS | BRF_PRG }, // 2
};

STD_ROM_PICK(Tetrisse)
STD_ROM_FN(Tetrisse)

//  Transformer

static struct BurnRomInfo TransfrmRomDesc[] = {

	{ "ic7.top", 0x8000, 0xccf1d123, BRF_ESS | BRF_PRG }, // 0 maincpu
	{ "epr-7347.ic5", 0x8000, 0xdf0f639f, BRF_ESS | BRF_PRG }, // 1
	{ "epr-7348.ic4", 0x8000, 0x0f38ea96, BRF_ESS | BRF_PRG }, // 2
	{ "ic3.top", 0x8000, 0x9d485df6, BRF_ESS | BRF_PRG }, // 3
	{ "epr-7350.ic2", 0x8000, 0x0052165d, BRF_ESS | BRF_PRG }, // 4
};

STD_ROM_PICK(Transfrm)
STD_ROM_FN(Transfrm)

#endif