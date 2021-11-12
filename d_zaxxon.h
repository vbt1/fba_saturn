#ifndef _D_ZAXXON_H_
#define _D_ZAXXON_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "snd/sn76496.h"
#include "czet.h"

int CongoInit();
int DrvInit();
int ZaxxonbInit();
int sZaxxonInit();
int DrvExit();
void DrvFrame();
inline void DrvDraw();
void dummy();
inline void copyBitmap();
inline void rotate32_tile(unsigned int size,unsigned char flip, unsigned char *target);

UINT8 *bitmap = NULL;
UINT8 *ss_map264 = NULL;
INT16 *sx_lut = NULL;
INT16 *sy_lut = NULL;
UINT16 *charaddr_lut = NULL;
UINT32 *colpromoffs_lut = NULL;
UINT32 *map_lut = NULL;
UINT32 *srcxmask[240] = {NULL}; //[256];
UINT32 *srcx_buffer = NULL;
UINT8 *AllRam = NULL;
UINT8 *RamEnd = NULL;
UINT8 *DrvZ80ROM = NULL;
UINT8 *DrvZ80DecROM = NULL;
// UINT8 *DrvZ80ROM2 = NULL;
UINT8 *DrvColPROM = NULL;
UINT8 *DrvZ80RAM = NULL;
// UINT8 *DrvZ80RAM2 = NULL;
UINT8 *DrvSprRAM = NULL;
UINT8 *DrvVidRAM = NULL;
UINT8 *DrvColRAM = NULL;
UINT8 *CZ80Context = NULL;

UINT8 DrvJoy1[8] = {0,0,0,0,0,0,0,0};
UINT8 DrvJoy2[8] = {0,0,0,0,0,0,0,0};
UINT8 DrvJoy3[8] = {0,0,0,0,0,0,0,0};
UINT8 DrvJoy4[8] = {0,0,0,0,0,0,0,0};
UINT8 DrvDips[2] = {0,0};
UINT8 DrvInputs[8] = {0,0,0,0,0,0,0,0};

UINT8 *zaxxon_coin_enable = NULL;
UINT8 *zaxxon_coin_status = NULL;
UINT8 *zaxxon_coin_last = NULL;

UINT8 zaxxon_bg_enable = 0;
UINT8 interrupt_enable = 0;
UINT32  zaxxon_bg_scroll_x2=0;
UINT32  zaxxon_bg_scroll = 0;
UINT8 zaxxon_flipscreen = 0;

/*static*/ //UINT8 *congo_color_bank = NULL;
/*static*/// UINT8 *congo_fg_bank = NULL;
/*static*/// UINT8 *congo_custom = NULL;
/*static*/// UINT8 *soundlatch = NULL;

static struct BurnInputInfo ZaxxonInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy4 + 0,	"p1 coin"},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy3 + 2,	"p1 start"},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 left"},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 right"},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy4 + 1,	"p2 coin"},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy3 + 3,	"p2 start"},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 left"},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 right"},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy2 + 4,	"p2 fire 1"},

	{"Reset",		BIT_DIGITAL,	NULL,	"reset"},
	{"Service",		BIT_DIGITAL,	DrvJoy4 + 2,	"service"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"},
};

STDINPUTINFO(Zaxxon)

static struct BurnInputInfo CongoBongoInputList[] = {
	{"P1 Coin",		BIT_DIGITAL,	DrvJoy3 + 6,	"p1 coin"},
	{"P1 Start",		BIT_DIGITAL,	DrvJoy3 + 3,	"p1 start"},
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 up"},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 down"},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 left"},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 right"},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"},

	{"P2 Coin",		BIT_DIGITAL,	DrvJoy3 + 7,	"p2 coin"},
	{"P2 Start",		BIT_DIGITAL,	DrvJoy3 + 4,	"p2 start"},
	{"P2 Up",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 up"},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 down"},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 left"},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 right"},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy2 + 4,	"p2 fire 1"},

	{"Reset",		BIT_DIGITAL,	NULL,	"reset"},
	{"Service",		BIT_DIGITAL,	DrvJoy4 + 2,	"service"},
	{"Dip A",		BIT_DIPSWITCH,	DrvDips + 0,	"dip"},
	{"Dip B",		BIT_DIPSWITCH,	DrvDips + 1,	"dip"},
};

STDINPUTINFO(CongoBongo)


static struct BurnDIPInfo ZaxxonDIPList[]=
{
	// Default Values
	{0x10, 0xff, 0xff, 0x7f, NULL		},
	{0x11, 0xff, 0xff, 0x33, NULL		},

	{0   , 0xfe, 0   ,    1, "Service Mode (No Toggle)"		},
	{0x10, 0x01, 0x01, 0x00, "Off"		},

	{0   , 0xfe, 0   ,    4, "Bonus_Life"		},
	{0x10, 0x01, 0x03, 0x03, "10000"		},
	{0x10, 0x01, 0x03, 0x01, "20000"		},
	{0x10, 0x01, 0x03, 0x02, "30000"		},
	{0x10, 0x01, 0x03, 0x00, "40000"		},

	{0   , 0xfe, 0   ,    2, "Unused"		},
	{0x10, 0x01, 0x04, 0x00, "Off"		},
	{0x10, 0x01, 0x04, 0x04, "On"		},

	{0   , 0xfe, 0   ,    2, "Unused"		},
	{0x10, 0x01, 0x08, 0x00, "Off"		},
	{0x10, 0x01, 0x08, 0x08, "On"		},

	{0   , 0xfe, 0   ,    4, "Lives"		},
	{0x10, 0x01, 0x30, 0x30, "3"		},
	{0x10, 0x01, 0x30, 0x10, "4"		},
	{0x10, 0x01, 0x30, 0x20, "5"		},
	{0x10, 0x01, 0x30, 0x00, "Free_Play"		},

	{0   , 0xfe, 0   ,    2, "Sound"		},
	{0x10, 0x01, 0x40, 0x00, "Off"		},
	{0x10, 0x01, 0x40, 0x40, "On"		},

	{0   , 0xfe, 0   ,    2, "Cabinet"		},
	{0x10, 0x01, 0x80, 0x00, "Upright"		},
	{0x10, 0x01, 0x80, 0x80, "Cocktail"		},

	{0   , 0xfe, 0   ,    16, "SW2:!1,!2,!3,!4"		},
	{0x11, 0x01, 0x0f, 0x0f, "SW2:!1,!2,!3,!4"		},
	{0x11, 0x01, 0x0f, 0x07, "SW2:!1,!2,!3,!4"		},
	{0x11, 0x01, 0x0f, 0x0b, "SW2:!1,!2,!3,!4"		},
	{0x11, 0x01, 0x0f, 0x06, "2C/1C 5C/3C 6C/4C"		},
	{0x11, 0x01, 0x0f, 0x0a, "2C/1C 3C/2C 4C/3C"		},
	{0x11, 0x01, 0x0f, 0x03, "SW2:!1,!2,!3,!4"		},
	{0x11, 0x01, 0x0f, 0x02, "1C/1C 5C/6C"		},
	{0x11, 0x01, 0x0f, 0x0c, "1C/1C 4C/5C"		},
	{0x11, 0x01, 0x0f, 0x04, "1C/1C 2C/3C"		},
	{0x11, 0x01, 0x0f, 0x0d, "SW2:!1,!2,!3,!4"		},
	{0x11, 0x01, 0x0f, 0x08, "1C/2C 5C/11C"		},
	{0x11, 0x01, 0x0f, 0x00, "1C/2C 4C/9C"		},
	{0x11, 0x01, 0x0f, 0x05, "SW2:!1,!2,!3,!4"		},
	{0x11, 0x01, 0x0f, 0x09, "SW2:!1,!2,!3,!4"		},
	{0x11, 0x01, 0x0f, 0x01, "SW2:!1,!2,!3,!4"		},
	{0x11, 0x01, 0x0f, 0x0e, "SW2:!1,!2,!3,!4"		},

	{0   , 0xfe, 0   ,    16, "SW2:!5,!6,!7,!8"		},
	{0x11, 0x01, 0xf0, 0xf0, "SW2:!5,!6,!7,!8"		},
	{0x11, 0x01, 0xf0, 0x70, "SW2:!5,!6,!7,!8"		},
	{0x11, 0x01, 0xf0, 0xb0, "SW2:!5,!6,!7,!8"		},
	{0x11, 0x01, 0xf0, 0x60, "2C/1C 5C/3C 6C/4C"		},
	{0x11, 0x01, 0xf0, 0xa0, "2C/1C 3C/2C 4C/3C"		},
	{0x11, 0x01, 0xf0, 0x30, "SW2:!5,!6,!7,!8"		},
	{0x11, 0x01, 0xf0, 0x20, "1C/1C 5C/6C"		},
	{0x11, 0x01, 0xf0, 0xc0, "1C/1C 4C/5C"		},
	{0x11, 0x01, 0xf0, 0x40, "1C/1C 2C/3C"		},
	{0x11, 0x01, 0xf0, 0xd0, "SW2:!5,!6,!7,!8"		},
	{0x11, 0x01, 0xf0, 0x80, "1C/2C 5C/11C"		},
	{0x11, 0x01, 0xf0, 0x00, "1C/2C 4C/9C"		},
	{0x11, 0x01, 0xf0, 0x50, "SW2:!5,!6,!7,!8"		},
	{0x11, 0x01, 0xf0, 0x90, "SW2:!5,!6,!7,!8"		},
	{0x11, 0x01, 0xf0, 0x10, "SW2:!5,!6,!7,!8"		},
	{0x11, 0x01, 0xf0, 0xe0, "SW2:!5,!6,!7,!8"		},
};

STDDIPINFO(Zaxxon)

// Congo Bongo

static struct BurnRomInfo congoRomDesc[] = {
	{ "congo1.u35",		0x2000, 0x09355b5b, 1 }, //  0 main
	{ "congo2.u34",		0x2000, 0x1c5e30ae, 1 }, //  1
	{ "congo3.u33",		0x2000, 0x5ee1132c, 1 }, //  2
	{ "congo4.u32",		0x2000, 0x5332b9bf, 1 }, //  3

	{ "congo17.u11",	0x2000, 0x5024e673, 2 }, //  4 audio

	{ "congo5.u76",		0x1000, 0x7bf6ba2b, 3 }, //  5 gfx1

	{ "congo8.u93",		0x2000, 0xdb99a619, 4 }, //  6 gfx2
	{ "congo9.u94",		0x2000, 0x93e2309e, 4 }, //  7
	{ "congo10.u95",	0x2000, 0xf27a9407, 4 }, //  8

	{ "congo12.u78",	0x2000, 0x15e3377a, 5 }, //  9 gfx3
	{ "congo13.u79",	0x2000, 0x1d1321c8, 5 }, // 10
	{ "congo11.u77",	0x2000, 0x73e2709f, 5 }, // 11
	{ "congo14.104",	0x2000, 0xbf9169fe, 5 }, // 12
	{ "congo16.106",	0x2000, 0xcb6d5775, 5 }, // 13
	{ "congo15.105",	0x2000, 0x7b15a7a4, 5 }, // 14

	{ "congo6.u57",		0x2000, 0xd637f02b, 6 }, // 15 gfx4
	{ "congo7.u58",		0x2000, 0x80927943, 6 }, // 16

	{ "congo.u68",		0x0100, 0xb788d8ae, 7 }, // 17 proms
};

STD_ROM_PICK(congo)
STD_ROM_FN(congo)

// Zaxxon (set 1)

static struct BurnRomInfo zaxxonRomDesc[] = {
	{ "zaxxon3.u27",	0x2000, 0x6e2b4a30, 1 }, //  0 main
	{ "zaxxon2.u28",	0x2000, 0x1c9ea398, 1 }, //  1
	{ "zaxxon1.u29",	0x1000, 0x1c123ef9, 1 }, //  2

	{ "zaxxon14.u68",	0x0800, 0x07bf8c52, 2 }, //  3 gfx1
	{ "zaxxon15.u69",	0x0800, 0xc215edcb, 2 }, //  4

	{ "zaxxon6.113",	0x2000, 0x6e07bb68, 3 }, //  5 gfx2
	{ "zaxxon5.112",	0x2000, 0x0a5bce6a, 3 }, //  6
	{ "zaxxon4.111",	0x2000, 0xa5bf1465, 3 }, //  7

	{ "zaxxon11.u77",	0x2000, 0xeaf0dd4b, 4 }, //  8 gfx3
	{ "zaxxon12.u78",	0x2000, 0x1c5369c7, 4 }, //  9
	{ "zaxxon13.u79",	0x2000, 0xab4e8a9a, 4 }, // 10

	{ "zaxxon8.u91",	0x2000, 0x28d65063, 5 }, // 11 gfx4
	{ "zaxxon7.u90",	0x2000, 0x6284c200, 5 }, // 12
	{ "zaxxon10.u93",	0x2000, 0xa95e61fd, 5 }, // 13
	{ "zaxxon9.u92",	0x2000, 0x7e42691f, 5 }, // 14

	{ "zaxxon.u98",		0x0100, 0x6cc6695b, 6 }, // 15 proms
	{ "zaxxon.u72",		0x0100, 0xdeaa21f7, 6 }, // 16
};

STD_ROM_PICK(zaxxon)
STD_ROM_FN(zaxxon)

static struct BurnRomInfo zaxxonbRomDesc[] = {
	{ "zaxxonb3.u27",	0x2000, 0x125bca1c, 1 }, //  0 main
	{ "zaxxonb2.u28",	0x2000, 0xc088df92, 1 }, //  1
	{ "zaxxonb1.u29",	0x1000, 0xe7bdc417, 1 }, //  2

	{ "zaxxon14.u68",	0x0800, 0x07bf8c52, 2 }, //  3 gfx1
	{ "zaxxon15.u69",	0x0800, 0xc215edcb, 2 }, //  4

	{ "zaxxon6.113",	0x2000, 0x6e07bb68, 3 }, //  5 gfx2
	{ "zaxxon5.112",	0x2000, 0x0a5bce6a, 3 }, //  6
	{ "zaxxon4.111",	0x2000, 0xa5bf1465, 3 }, //  7

	{ "zaxxon11.u77",	0x2000, 0xeaf0dd4b, 4 }, //  8 gfx3
	{ "zaxxon12.u78",	0x2000, 0x1c5369c7, 4 }, //  9
	{ "zaxxon13.u79",	0x2000, 0xab4e8a9a, 4 }, // 10

	{ "zaxxon8.u91",	0x2000, 0x28d65063, 5 }, // 11 gfx4
	{ "zaxxon7.u90",	0x2000, 0x6284c200, 5 }, // 12
	{ "zaxxon10.u93",	0x2000, 0xa95e61fd, 5 }, // 13
	{ "zaxxon9.u92",	0x2000, 0x7e42691f, 5 }, // 14

	{ "zaxxon.u98",		0x0100, 0x6cc6695b, 6 }, // 15 proms
	{ "zaxxon.u72",		0x0100, 0xdeaa21f7, 6 }, // 16
};

STD_ROM_PICK(zaxxonb)
STD_ROM_FN(zaxxonb)

// Super Zaxxon

static struct BurnRomInfo szaxxonRomDesc[] = {
	{ "suzaxxon.3",	0x2000, 0xaf7221da, 1 }, //  0 main
	{ "suzaxxon.2",	0x2000, 0x1b90fb2a, 1 }, //  1
	{ "suzaxxon.1",	0x1000, 0x07258b4a, 1 }, //  2

	{ "suzaxxon.14",	0x0800, 0xbccf560c, 2 }, //  3 gfx1
	{ "suzaxxon.15",	0x0800, 0xd28c628b, 2 }, //  4

	{ "suzaxxon.6",	0x2000, 0xf51af375, 3 }, //  5 gfx2
	{ "suzaxxon.5",	0x2000, 0xa7de021d, 3 }, //  6
	{ "suzaxxon.4",	0x2000, 0x5bfb3b04, 3 }, //  7

	{ "suzaxxon.11",	0x2000, 0x1503ae41, 4 }, //  8 gfx3
	{ "suzaxxon.12",	0x2000, 0x3b53d83f, 4 }, //  9
	{ "suzaxxon.13",	0x2000, 0x581e8793, 4 }, // 10

	{ "suzaxxon.8",	0x2000, 0xdd1b52df, 5 }, // 11 gfx4
	{ "suzaxxon.7",	0x2000, 0xb5bc07f0, 5 }, // 12
	{ "suzaxxon.10",	0x2000, 0x68e84174, 5 }, // 13
	{ "suzaxxon.9",	0x2000, 0xa509994b, 5 }, // 14

	{ "suzaxxon.u98",	0x0100, 0x15727a9f, 6 }, // 15 proms
	{ "suzaxxon.u72",	0x0100, 0xdeaa21f7, 6 }, // 16
};

STD_ROM_PICK(szaxxon)
STD_ROM_FN(szaxxon)
/*
struct BurnDriver BurnDrvCongo = {
	"congo", NULL, NULL, NULL, "1983",
	"Congo Bongo\0", NULL, "Sega", "hardware",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_ORIENTATION_VERTICAL | BDF_ORIENTATION_FLIPPED, 2, HARDWARE_MISC_PRE90S, GBF_MISC, 0,
	NULL, congoRomInfo, congoRomName, NULL, NULL, CongoBongoInputInfo, ZaxxonDIPInfo, //CongoInputInfo, CongoDIPInfo,
	CongoInit, DrvExit, DrvFrame, DrvDraw, NULL, &DrvRecalc, 0,
	224, 256, 3, 4
};
*/


// Tip Top

static struct BurnRomInfo tiptopRomDesc[] = {
	{ "tiptop1.u35",	0x2000, 0xe19dc77b, 1 }, //  0 main
	{ "tiptop2.u34",	0x2000, 0x3fcd3b6e, 1 }, //  1
	{ "tiptop3.u33",	0x2000, 0x1c94250b, 1 }, //  2
	{ "tiptop4.u32",	0x2000, 0x577b501b, 1 }, //  3

	{ "congo17.u11",	0x2000, 0x5024e673, 2 }, //  4 audio

	{ "congo5.u76",		0x1000, 0x7bf6ba2b, 3 }, //  5 gfx1

	{ "congo8.u93",		0x2000, 0xdb99a619, 4 }, //  6 gfx2
	{ "congo9.u94",		0x2000, 0x93e2309e, 4 }, //  7
	{ "congo10.u95",	0x2000, 0xf27a9407, 4 }, //  8

	{ "congo12.u78",	0x2000, 0x15e3377a, 5 }, //  9 gfx3
	{ "congo13.u79",	0x2000, 0x1d1321c8, 5 }, // 10
	{ "congo11.u77",	0x2000, 0x73e2709f, 5 }, // 11
	{ "congo14.u104",	0x2000, 0xbf9169fe, 5 }, // 12
	{ "congo16.u106",	0x2000, 0xcb6d5775, 5 }, // 13
	{ "congo15.u105",	0x2000, 0x7b15a7a4, 5 }, // 14

	{ "congo6.u57",		0x2000, 0xd637f02b, 6 }, // 15 gfx4
	{ "congo7.u58",		0x2000, 0x80927943, 6 }, // 16

	{ "congo.u68",		0x0100, 0xb788d8ae, 7 }, // 17 proms
};

STD_ROM_PICK(tiptop)
STD_ROM_FN(tiptop)
/*
struct BurnDriver BurnDrvTiptop = {
	"tiptop", "congo", NULL, NULL, "1983",
	"Tip Top\0", NULL, "Sega", "hardware",
	NULL, NULL, NULL, NULL,
	BDF_GAME_WORKING | BDF_CLONE | BDF_ORIENTATION_VERTICAL | BDF_ORIENTATION_FLIPPED, 2, HARDWARE_MISC_PRE90S, GBF_MISC, 0,
	NULL, tiptopRomInfo, tiptopRomName, NULL, NULL, ZaxxonInputInfo, ZaxxonDIPInfo, //CongoInputInfo, CongoDIPInfo,
	CongoInit, DrvExit, DrvFrame, DrvDraw, NULL, &DrvRecalc, 0,
	224, 256, 3, 4
};
*/
#endif