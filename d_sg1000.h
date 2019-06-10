#ifndef _D_SG1000_H_
#define _D_SG1000_H_

#include "burnint.h"
//#include "eeprom.h"
#include "saturn/ovl.h"
#include "sega_int.h"

#include "tms9928a.h"
#include "8255ppi.h"
#include "snd/sn76496.h"
#ifdef RAZE
#include "raze\raze.h"
#endif

/*static*/ int DrvInit();
/*static*/ int DrvExit();
/*static*/ int DrvFrame();
/*static*/ int DrvDoReset();

void PCM_MeInit(void);
void PCM_MeStart(PcmHn hn);
void PCM_MeStop(PcmHn hn);
void *memset4_fast(void *, long, size_t);

/*static*/ void update_input1(void);
/*static*/ UINT8 __fastcall sg1000_read_port(unsigned short port);
void __fastcall sg1000_write_port(unsigned short port, UINT8 data);
/*static*/ UINT8 /*__fastcall*/ sg1000_read_0000(UINT16 address);
static void __fastcall sg1000_write(UINT16 address, UINT8 data);
/*static*/ UINT8 /*__fastcall*/ sg1000_read(UINT16 address);
#ifdef RAZE
/*static*/ UINT8 /*__fastcall*/ sg1000_read_c000(UINT16 address);
/*static*/ UINT8 /*__fastcall*/ sg1000_write_c000(UINT16 address, UINT8 data);
/*static*/ UINT8 /*__fastcall*/ sg1000_read_c000_ext2(UINT16 address);
/*static*/ UINT8 /*__fastcall*/ sg1000_write_c000_ext2(UINT16 address, UINT8 data);
/*static*/ UINT8 /*__fastcall*/ sg1000_read_2000(UINT16 address);
/*static*/ UINT8 /*__fastcall*/ sg1000_write_2000(UINT16 address, UINT8 data);
#else
/*static*/ void /*__fastcall*/ sg1000_write_ext2(UINT16 address, UINT8 data);
/*static*/ UINT8 /*__fastcall*/ sg1000_read_ext2(UINT16 address);
#endif
/*static*/ void vdp_interrupt(int state);
/*static*/ void sg1000_ppi8255_portC_write(UINT8 data);
/*static*/ UINT8 sg1000_ppi8255_portA_read();
/*static*/ UINT8 sg1000_ppi8255_portB_read();
/*static*/ UINT8 sg1000_ppi8255_portC_read();

extern int file_id;
extern int file_max;

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

#define MAPPER_NONE        (0x00)
#define MAPPER_TEREBI      (0x01)
#define MAPPER_RAM_8K_EXT1 (0x02)
#define MAPPER_RAM_8K_EXT2 (0x03)
#define MAPPER_SEGA        (0x10)
#define MAPPER_SEGA_X      (0x11)
#define MAPPER_93C46       (0x12)
#define MAPPER_CODIES      (0x13)
#define MAPPER_MULTI       (0x14)
#define MAPPER_KOREA       (0x15)
#define MAPPER_KOREA_16K   (0x16)
#define MAPPER_KOREA_8K    (0x20)
#define MAPPER_MSX         (0x21)
#define MAPPER_MSX_NEMESIS (0x22)

/*static*/ UINT8 *AllMem	= NULL;
/*static*/ UINT8 *MemEnd	= NULL;
/*static*/ UINT8 *AllRam	= NULL;
/*static*/ UINT8 *RamEnd	= NULL;
/*static*/ UINT8 *DrvZ80ROM	= NULL;
/*static*/ UINT8 *DrvZ80RAM	= NULL;
/*static*/ UINT8 *DrvZ80ExtRAM = NULL;
/*static*/ UINT8 *CZ80Context = NULL;
/*static*/ UINT8 *TMSContext = NULL;

/*static*/ UINT8 DrvInputs[2]={0,0};
/*static*/ UINT8 DrvJoy1[8]={0,0,0,0,0,0,0,0};
/*static*/ UINT8 DrvJoy2[8]={0,0,0,0,0,0,0,0};
/*static*/ UINT8 DrvDips[1]={0};

/*static*/ struct BurnInputInfo Sg1000InputList[] = {
	{"P1 Up",		BIT_DIGITAL,	DrvJoy1 + 0,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	DrvJoy1 + 1,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	DrvJoy1 + 2,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	DrvJoy1 + 3,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	DrvJoy1 + 4,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	DrvJoy1 + 5,	"p1 fire 2"	},

	{"P2 Up",		BIT_DIGITAL,	DrvJoy1 + 6,	"p2 up"		},
	{"P2 Down",		BIT_DIGITAL,	DrvJoy1 + 7,	"p2 down"	},
	{"P2 Left",		BIT_DIGITAL,	DrvJoy2 + 0,	"p2 left"	},
	{"P2 Right",		BIT_DIGITAL,	DrvJoy2 + 1,	"p2 right"	},
	{"P2 Button 1",		BIT_DIGITAL,	DrvJoy2 + 2,	"p2 fire 1"	},
	{"P2 Button 2",		BIT_DIGITAL,	DrvJoy2 + 3,	"p2 fire 2"	},

	{"Reset",		BIT_DIGITAL,	NULL,	"reset"		},
};



STDINPUTINFO(Sg1000)

static struct BurnDIPInfo Sg1000DIPList[]=
{
	{0x11, 0xff, 0xff, 0xbf, NULL			},

	{0   , 0xfe, 0   ,    4, "Coinage"		},
	{0x11, 0x01, 0x30, 0x00, "2 Coins 1 Credits"	},
	{0x11, 0x01, 0x30, 0x30, "1 Coin  1 Credits"	},
	{0x11, 0x01, 0x30, 0x20, "1 Coin  2 Credits"	},
	{0x11, 0x01, 0x30, 0x10, "1 Coin  3 Credits"	},

	{0   , 0xfe, 0   ,    2, "Demo Sounds"		},
	{0x11, 0x01, 0x40, 0x40, "Off"			},
	{0x11, 0x01, 0x40, 0x00, "On"			},

	{0   , 0xfe, 0   ,    2, "Language"		},
	{0x11, 0x01, 0x80, 0x00, "Japanese"		},
	{0x11, 0x01, 0x80, 0x80, "English"		},
};

STDDIPINFO(Sg1000)

static struct BurnRomInfo sg1k_wboyRomDesc[] = {
	{ "WonderB.sg",	0x0C000, 0xe8f0344d, BRF_PRG | BRF_ESS },
};

STD_ROM_PICK(sg1k_wboy)
STD_ROM_FN(sg1k_wboy)

#endif