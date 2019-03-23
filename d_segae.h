#ifndef _D_SEGAE_H_
#define _D_SEGAE_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "snd/sn76496.h"
#include "bitswap.h"
#include "mc8123.h"

#define nBurnSoundLen 128

/*static*/ UINT16 *name_lut = NULL;
/*static*/ UINT32 *bp_lut = NULL;//[0x10000];
/*static*/ UINT16 *cram_lut = NULL;//[0x40];
/*static*/ UINT16 *map_lut = NULL;
/*static*/ UINT8 *CZ80Context = NULL;

/*static*/ INT32 DrvFantzn2Init();
/*static*/ INT32 DrvOpaopaInit();
/*static*/ INT32 DrvHangonJrInit();
/*static*/ INT32 DrvTetrisInit();
/*static*/ INT32 DrvTransfrmInit();
/*static*/ INT32 DrvSlapshtrInit();
/*static*/ INT32 DrvAstroflInit();
/*static*/ INT32 DrvExit();
/*static*/ INT32 DrvFrame();
/*static*/ void DrvInitSaturnS(UINT8 game);
/*static*/ void update_bg(UINT8 chip, UINT32 index);
/*static*/ void update_sprites(UINT8 chip, UINT32 index);
void SCL_SetLineParamNBG1();

/*static*/ UINT8 DrvJoy0[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/ UINT8 DrvJoy1[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/ UINT8 DrvJoy2[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/ UINT8 DrvJoy3[8] = {0, 0, 0, 0, 0, 0, 0, 0};
/*static*/ UINT8 DrvJoy4[8] = {0, 0, 0, 0, 0, 0, 0, 0};

/*static*/ UINT8 DrvInput[5] = {0,0,0,0,0};
/*static*/ UINT8 DrvDip[2] = {0, 0};
/*static*/ UINT8 DrvReset = 0;

/*static*/ INT32 DrvWheel = 0;
/*static*/ INT32 DrvAccel = 0;

/*static*/ UINT8 *AllMem;
/*static*/ UINT8 *MemEnd;
/*static*/ UINT8 *AllRam;
/*static*/ UINT8 *RamEnd;
/*static*/ UINT8 *DrvRAM;
/*static*/ UINT8 *DrvMainROM;
/*static*/ UINT8 *DrvMainROMFetch;
/*static*/ UINT8 *mc8123key;
///*static*/ UINT32 *DrvPalette;
///*static*/ UINT32 *Palette;
///*static*/ UINT8 *cache_bitmap;

/*static*/ UINT8 segae_8000bank = 0;
/*static*/ UINT8 port_fa_last = 0;
/*static*/ UINT8 rombank = 0;

/*static*/ UINT8 mc8123 = 0; // enabled?
/*static*/ UINT8 mc8123_banked = 0; // enabled?

UINT8 hintcount = 0;			/* line interrupt counter, decreased each scanline */
UINT8 vintpending = 0;
UINT8 hintpending = 0;

//UINT8 m_port_select;
INT16 currentLine = 0;

//UINT8 leftcolumnblank = 0; // most games need this, except tetris

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
UINT32 ntab[CHIPS];
UINT32 satb[CHIPS];
/*static*/ int scroll_x[CHIPS]={0,0};
/*static*/ int scroll_y[CHIPS]={0,0};

typedef UINT16	trigger_t;

/*static*/ UINT16 pad_asign[]={
PER_DGT_U,PER_DGT_D,PER_DGT_R,PER_DGT_L,PER_DGT_A,PER_DGT_B,
PER_DGT_C,PER_DGT_S,PER_DGT_X,PER_DGT_Y,PER_DGT_TR,PER_DGT_TL,
};

trigger_t	pltrigger[2],pltriggerE[2];

#define	SZ_PERIPHERAL	20
typedef	UINT8	SysPeripheral[SZ_PERIPHERAL+2];

typedef	struct	SysPort	{
	UINT8			id;
	UINT8			connectable;
	SysPeripheral	*peripheral;
} SysPort;

typedef	struct	SysDevice	{
	UINT8	type;
	UINT8	size;
	UINT8	data[1];
} SysDevice;

/*static*/ struct BurnInputInfo TransfrmInputList[] = {

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
	{"Service Mode",		BIT_DIGITAL,	DrvJoy0 + 2,	"diag"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDip + 0,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDip + 1,	"dip"},
};
STDINPUTINFO(Transfrm)

/*static*/ struct BurnInputInfo Segae2pInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy0 + 0,	"p1 coin"},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy0 + 6,	"p1 start"},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 right"},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy0 + 1,	"p2 coin"},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy0 + 7,	"p2 start"},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 left"},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 right"},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy2 + 4,	"p2 fire 1"},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy2 + 5,	"p2 fire 2"},

	{"Reset",		BIT_DIGITAL,	&DrvReset,	"reset"},
	{"Service",		BIT_DIGITAL,	DrvJoy0 + 3,	"service"},
	{"Service Mode",		BIT_DIGITAL,	DrvJoy0 + 2,	"diag"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDip + 0,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDip + 1,	"dip"},
};

STDINPUTINFO(Segae2p)

/*static*/ struct BurnDIPInfo TransfrmDIPList[]=
{
	{0x0d, 0xff, 0xff, 0xff, NULL		},
	{0x0e, 0xff, 0xff, 0xfc, NULL		},

	{0   , 0xfe, 0   ,    2, "1 Player Only"		},
	{0x0e, 0x01, 0x01, 0x00, "Off"		},
	{0x0e, 0x01, 0x01, 0x01, "On"		},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x0e, 0x01, 0x02, 0x02, "Off"		},
	{0x0e, 0x01, 0x02, 0x00, "On"		},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x0e, 0x01, 0x0c, 0x0c, "3"		},
	{0x0e, 0x01, 0x0c, 0x08, "4"		},
	{0x0e, 0x01, 0x0c, 0x04, "5"		},
	{0x0e, 0x01, 0x0c, 0x00, "Infinite (Cheat)"		},

	{0   , 0xfe, 0   ,    4, "Bonus Life"		},
	{0x0e, 0x01, 0x30, 0x20, "10k, 30k, 50k and 70k"		},
	{0x0e, 0x01, 0x30, 0x30, "20k, 60k, 100k and 140k"		},
	{0x0e, 0x01, 0x30, 0x10, "30k, 80k, 130k and 180k"		},
	{0x0e, 0x01, 0x30, 0x00, "50k, 150k and 250k"		},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x0e, 0x01, 0xc0, 0x40, "Easy"		},
	{0x0e, 0x01, 0xc0, 0xc0, "Medium"		},
	{0x0e, 0x01, 0xc0, 0x80, "Hard"		},
	{0x0e, 0x01, 0xc0, 0x00, "Hardest"		},
};

STDDIPINFO(Transfrm)

#define A(a, b, c, d) {a, b, (UINT8*)(c), d}
/*static*/ struct BurnInputInfo HangonjrInputList[] = {

    {"P1 Coin",     BIT_DIGITAL,    DrvJoy0 + 0,    "p1 coin"},
    {"P1 Start",    BIT_DIGITAL,    DrvJoy0 + 4,    "p1 start"},
	A("P1 Steering", BIT_ANALOG_REL, &DrvWheel,     "p1 x-axis"),
	A("P1 Accelerate", BIT_ANALOG_REL, &DrvAccel,   "p1 z-axis"),

	{"Reset",		BIT_DIGITAL,	&DrvReset,	    "reset"},
	{"Service",		BIT_DIGITAL,	DrvJoy0 + 3,	"service"},
	{"Service Mode",		BIT_DIGITAL,	DrvJoy0 + 2,	"diag"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDip + 0,	    "dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDip + 1,	    "dip"},
};

STDINPUTINFO(Hangonjr)
#undef A


/*static*/ struct BurnDIPInfo HangonjrDIPList[]=
{
	{0x07, 0xff, 0xff, 0xff, NULL		},
	{0x08, 0xff, 0xff, 0x14, NULL		},

	{0   , 0xfe, 0   ,    4, "Enemies"		},
	{0x08, 0x01, 0x06, 0x06, "Easy"		},
	{0x08, 0x01, 0x06, 0x04, "Medium"		},
	{0x08, 0x01, 0x06, 0x02, "Hard"		},
	{0x08, 0x01, 0x06, 0x00, "Hardest"		},

	{0   , 0xfe, 0   ,    4, "Time Adj."		},
	{0x08, 0x01, 0x18, 0x18, "Easy"		},
	{0x08, 0x01, 0x18, 0x10, "Medium"		},
	{0x08, 0x01, 0x18, 0x08, "Hard"		},
	{0x08, 0x01, 0x18, 0x00, "Hardest"		},
};

STDDIPINFO(Hangonjr)

/*static*/ struct BurnInputInfo TetrisseInputList[] = {

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


/*static*/ struct BurnDIPInfo TetrisseDIPList[]=
{
	{0x0a, 0xff, 0xff, 0xff, NULL		},
	{0x0b, 0xff, 0xff, 0x30, NULL		},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x0b, 0x01, 0x02, 0x02, "Off"		},
	{0x0b, 0x01, 0x02, 0x00, "On"		},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x0b, 0x01, 0x30, 0x20, "Easy"		},
	{0x0b, 0x01, 0x30, 0x30, "Normal"		},
	{0x0b, 0x01, 0x30, 0x10, "Hard"		},
	{0x0b, 0x01, 0x30, 0x00, "Hardest"		},
};
STDDIPINFO(Tetrisse)

/*static*/ struct BurnDIPInfo Fantzn2DIPList[]=
{
	{0x0d, 0xff, 0xff, 0xff, NULL		}, // coinage (missing for now)
	{0x0e, 0xff, 0xff, 0xfc, NULL		},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x0e, 0x01, 0x02, 0x02, "Off"		},
	{0x0e, 0x01, 0x02, 0x00, "On"		},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x0e, 0x01, 0x0c, 0x00, "2"		},
	{0x0e, 0x01, 0x0c, 0x0c, "3"		},
	{0x0e, 0x01, 0x0c, 0x08, "4"		},
	{0x0e, 0x01, 0x0c, 0x04, "5"		},

	{0   , 0xfe, 0   ,    4, "Timer"		},
	{0x0e, 0x01, 0x30, 0x20, "90"		},
	{0x0e, 0x01, 0x30, 0x30, "80"		},
	{0x0e, 0x01, 0x30, 0x10, "70"		},
	{0x0e, 0x01, 0x30, 0x00, "60"		},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x0e, 0x01, 0xc0, 0x80, "Easy"		},
	{0x0e, 0x01, 0xc0, 0xc0, "Normal"		},
	{0x0e, 0x01, 0xc0, 0x40, "Hard"		},
	{0x0e, 0x01, 0xc0, 0x00, "Hardest"		},
};

STDDIPINFO(Fantzn2)

/*static*/ struct BurnDIPInfo OpaopaDIPList[]=
{
	{0x13, 0xff, 0xff, 0xff, NULL		}, // coinage defs.
	{0x14, 0xff, 0xff, 0xff, NULL		},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x14, 0x01, 0x02, 0x02, "Off"		},
	{0x14, 0x01, 0x02, 0x00, "On"		},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x14, 0x01, 0x0c, 0x00, "2"		},
	{0x14, 0x01, 0x0c, 0x0c, "3"		},
	{0x14, 0x01, 0x0c, 0x08, "4"		},
	{0x14, 0x01, 0x0c, 0x04, "5"		},

	{0   , 0xfe, 0   ,    4, "Bonus Life"		},
	{0x14, 0x01, 0x30, 0x20, "25k, 45k and 70k"		},
	{0x14, 0x01, 0x30, 0x30, "40k, 60k and 90k"		},
	{0x14, 0x01, 0x30, 0x10, "50k and 90k"		},
	{0x14, 0x01, 0x30, 0x00, "None"		},

	{0   , 0xfe, 0   ,    4, "Difficulty"		},
	{0x14, 0x01, 0xc0, 0x80, "Easy"		},
	{0x14, 0x01, 0xc0, 0xc0, "Normal"		},
	{0x14, 0x01, 0xc0, 0x40, "Hard"		},
	{0x14, 0x01, 0xc0, 0x00, "Hardest"		},
};

STDDIPINFO(Opaopa)

//-----------------------

// Hang-On Jr.

/*static*/ struct BurnRomInfo hangonjrRomDesc[] = {
	{ "rom5.ic7",	0x8000, 0xd63925a7, 1 }, //  0 maincpu
	{ "rom4.ic5",	0x8000, 0xee3caab3, 1 }, //  1
	{ "rom3.ic4",	0x8000, 0xd2ba9bc9, 1 }, //  2
	{ "rom2.ic3",	0x8000, 0xe14da070, 1 }, //  3
	{ "rom1.ic2",	0x8000, 0x3810cbf5, 1 }, //  4
};

STD_ROM_PICK(hangonjr)
STD_ROM_FN(hangonjr)

//  Tetris (Japan, System E)

/*static*/ struct BurnRomInfo TetrisseRomDesc[] = {

	{ "epr12213.7", 0x8000, 0xef3c7a38, BRF_ESS | BRF_PRG }, // 0 maincpu
	{ "epr12212.5", 0x8000, 0x28b550bf, BRF_ESS | BRF_PRG }, // 1
	{ "epr12211.4", 0x8000, 0x5aa114e9, BRF_ESS | BRF_PRG }, // 2
};

STD_ROM_PICK(Tetrisse)
STD_ROM_FN(Tetrisse)

//  Transformer

/*static*/ struct BurnRomInfo TransfrmRomDesc[] = {

	{ "ic7.top", 0x8000, 0xccf1d123, BRF_ESS | BRF_PRG }, // 0 maincpu
	{ "epr7347.ic5", 0x8000, 0xdf0f639f, BRF_ESS | BRF_PRG }, // 1
	{ "epr7348.ic4", 0x8000, 0x0f38ea96, BRF_ESS | BRF_PRG }, // 2
	{ "ic3.top", 0x8000, 0x9d485df6, BRF_ESS | BRF_PRG }, // 3
	{ "epr7350.ic2", 0x8000, 0x0052165d, BRF_ESS | BRF_PRG }, // 4
};

STD_ROM_PICK(Transfrm)
STD_ROM_FN(Transfrm)

//  Astro Flash

/*static*/ struct BurnRomInfo AstroflRomDesc[] = {

	{ "epr7723.ic7", 0x8000, 0x66061137, BRF_ESS | BRF_PRG }, // 0 maincpu
	{ "epr7347.ic5", 0x8000, 0xdf0f639f, BRF_ESS | BRF_PRG }, // 1
	{ "epr7348.ic4", 0x8000, 0x0f38ea96, BRF_ESS | BRF_PRG }, // 2
	{ "epr7349.ic3", 0x8000, 0xf8c352d5, BRF_ESS | BRF_PRG }, // 3
	{ "epr7350.ic2", 0x8000, 0x0052165d, BRF_ESS | BRF_PRG }, // 4
};

STD_ROM_PICK(Astrofl)
STD_ROM_FN(Astrofl)

// Fantasy Zone II - The Tears of Opa-Opa (MC-8123, 317-0057)

/*static*/ struct BurnRomInfo fantzn2RomDesc[] = {
	{ "epr11416.ic7",	0x08000, 0x76db7b7b, 1 | BRF_PRG | BRF_ESS }, //  0 maincpu (encr)
	{ "epr11415.ic5",	0x10000, 0x57b45681, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "epr11413.ic3",	0x10000, 0xa231dc85, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "epr11414.ic4",	0x10000, 0x6f7a9f5f, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "epr11412.ic2",	0x10000, 0xb14db5af, 1 | BRF_PRG | BRF_ESS }, //  4

	{ "3170057.key",	0x02000, 0xee43d0f0, 2 | BRF_GRA },           //  5 key
};

STD_ROM_PICK(fantzn2)
STD_ROM_FN(fantzn2)

// Opa Opa (MC-8123, 317-0042)

/*static*/ struct BurnRomInfo opaopaRomDesc[] = {
	{ "epr11054.ic7",	0x8000, 0x024b1244, 1 | BRF_PRG | BRF_ESS }, //  0 maincpu  (encr)
	{ "epr11053.ic5",	0x8000, 0x6bc41d6e, 1 | BRF_PRG | BRF_ESS }, //  1          ""
	{ "epr11052.ic4",	0x8000, 0x395c1d0a, 1 | BRF_PRG | BRF_ESS }, //  2          ""
	{ "epr11051.ic3",	0x8000, 0x4ca132a2, 1 | BRF_PRG | BRF_ESS }, //  3          ""
	{ "epr11050.ic2",	0x8000, 0xa165e2ef, 1 | BRF_PRG | BRF_ESS }, //  4          ""

	{ "3170042.key",	0x2000, 0xd6312538, 2 | BRF_GRA },           //  5 key
};

STD_ROM_PICK(opaopa)
STD_ROM_FN(opaopa)

//  Slap Shooter

/*static*/ struct BurnRomInfo slapshtrRomDesc[] = {

	{ "epr7351.ic7", 0x8000, 0x894adb04, BRF_ESS | BRF_PRG }, // 0 maincpu
	{ "epr7352.ic5", 0x8000, 0x61c938b6, BRF_ESS | BRF_PRG }, // 1
	{ "epr7353.ic4", 0x8000, 0x8ee2951a, BRF_ESS | BRF_PRG }, // 2
	{ "epr7354.ic3", 0x8000, 0x41482aa0, BRF_ESS | BRF_PRG }, // 3
	{ "epr7355.ic2", 0x8000, 0xc67e1aef, BRF_ESS | BRF_PRG }, // 4
};
STD_ROM_PICK(slapshtr)
STD_ROM_FN(slapshtr)

#endif
