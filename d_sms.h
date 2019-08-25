#ifndef _D_SMS_H_
#define _D_SMS_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "sega_int.h"
#include "raze/raze.h"
#include "czet.h"
static UINT8 update_input1(void);
static UINT8 update_input2(void);
void  FNT_Print256_2bpp(volatile Uint8 *vram,volatile Uint8 *str,Uint16 x,Uint16 y);
void *memset4_fast(void *, long, size_t);
UINT8 SMSJoy1[1]={NULL};

/* Game image structure */
typedef struct
{
    UINT8 *rom;
    UINT8 pages;
//    UINT8 type;
}t_cart;

#define TYPE_DOMESTIC   (1)

typedef struct
{
	UINT8 ram[0x2000];
	UINT8 sram[0x8000];
//#ifdef VBT
	UINT8 fcr[4];
//#endif
	UINT8 paused;
//	UINT8 save;
//	UINT8 country;
	UINT8 port_3F;
	UINT8 port_F2;
//	UINT8 use_fm;
	UINT8 irq;
#ifdef GG
	UINT8 psg_mask;
#endif
}t_sms;

#ifdef GG
unsigned  char *vram_dirty = NULL;
unsigned  char is_vram_dirty = 0;
#endif

/* Display timing (NTSC) */
#define MASTER_CLOCK        (3579545)
#define LINES_PER_FRAME     (262)
#define FRAMES_PER_SECOND   (60)
#define CYCLES_PER_LINE_50     ((MASTER_CLOCK / 50) / LINES_PER_FRAME)
#define CYCLES_PER_LINE_60     ((MASTER_CLOCK / 60) / LINES_PER_FRAME)

/* VDP context */
typedef struct
{
    UINT8 vram[0x4000];
    UINT8 cram[0x40];
    UINT8 reg[0x10];
    UINT8 status;
    UINT8 latch;
    UINT8 pending;
    UINT8 buffer;
    UINT8 code;
    UINT16 addr;
    UINT32 ntab;
    UINT32 satb;
    UINT32 line;
    INT32 left;
#ifdef GG
    UINT16 cram_latch;
#endif
//    UINT8 limit;
}t_vdp;

/* Global data */
/*static*/  t_cart cart = {.rom = NULL, .pages = 0};
/*static*/  t_sms sms = {.paused = 0, .port_3F = 0, .port_F2 = 0, .irq = 0};
/*static*/  t_vdp vdp = {.status = 0, .latch = 0, .pending = 0, .buffer = 0};
/*static*/  int running=0;
/*static*/  unsigned int first = 1;
/*static*/  unsigned int vsynch = 0;
/*static*/  int scroll_x=0,scroll_y=0;
/*static*/  UINT8 *dummy_write  = NULL; //[0x100];
/*static*/  UINT8 *SaturnMem = NULL;
/*static*/  UINT8 *MemEnd = NULL;
/*static*/  UINT16 *name_lut = NULL;
/*static*/  UINT32 *bp_lut = NULL;//[0x10000];
/*static*/  UINT16 *cram_lut = NULL;//[0x40];
/*static*/  UINT16 *map_lut = NULL;
/*static*/  UINT8 *CZ80Context = NULL;
//extern unsigned char play;
extern int file_id;
extern int file_max;

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

typedef	struct	SysDevice	{
	UINT8	type;
	UINT8	size;
	UINT8	data[1];
} SysDevice;

/*static*/  INT32 SMSInit();
/*static*/  INT32 SMSExit();
/*static*/  INT32 SMSDraw();
/*static*/  INT32 SMSFrame();
/*static*/  void z80_init(void);
/*static*/  void sms_init(void);
/*static*/  void sms_reset(void);
/*static*/  void sms_frame(void);
/*static*/  void vdp_run(t_vdp *vdp);
/*static*/  void vdp_reset(void);
/*static*/  void system_init(void);
/*static*/  void cpu_writemem8(unsigned int address, unsigned int data);
/*static*/  unsigned int cpu_readmem8(unsigned int address);
void PSG_Update(signed short *buffer, unsigned int length);
void PSG_Init(unsigned int clock, unsigned int rate);
void PSG_Write(unsigned int data);
Sint32 getNbFiles();
Sint32 GetFileSize(int file_id);
/*static*/  void load_rom(void);

//INT32 SMSScan(INT32 nAction, INT32 *pnMin);
/* Return values from the V counter */
static UINT8 vcnt[0x200] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA,
                                  0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};

/* Return values from the H counter */
static UINT8 hcnt[0x200] =
{
    0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
    0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
    0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
    0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F,
    0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F,
    0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D, 0x7E, 0x7F,
    0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
    0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9,
                      0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
    0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
    0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
    0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
    0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
    0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
    0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF,
};



static struct BurnDIPInfo SMSDIPList[] = {
	{0x3d, 0xff, 0xff, 0x00, NULL				},
	{0x3e, 0xff, 0xff, 0x00, NULL				},
	{0x3f, 0xff, 0xff, 0x01, NULL				},

};

STDDIPINFO(SMS)

static struct BurnInputInfo SMSInputList[] = {
	{"P1 Start",		BIT_DIGITAL,	SMSJoy1 + 3,	"p1 start"	}, // 0
/*	{"P1 Select",		BIT_DIGITAL,	SMSJoy1 + 2,	"p1 select"	},
	{"P1 Up",		BIT_DIGITAL,	SMSJoy1 + 4,	"p1 up"		},
	{"P1 Down",		BIT_DIGITAL,	SMSJoy1 + 6,	"p1 down"	},
	{"P1 Left",		BIT_DIGITAL,	SMSJoy1 + 7,	"p1 left"	},
	{"P1 Right",		BIT_DIGITAL,	SMSJoy1 + 5,	"p1 right"	},
	{"P1 Button 1",		BIT_DIGITAL,	SMSJoy1 + 1,	"p1 fire 1"	},
	{"P1 Button 2",		BIT_DIGITAL,	SMSJoy1 + 0,	"p1 fire 2"	},
	{"P1 Button 3",		BIT_DIGITAL,	SMSJoy1 + 8,	"p1 fire 3"	},
	{"P1 Button 4",		BIT_DIGITAL,	SMSJoy1 + 9,	"p1 fire 4"	},
	{"P1 Button 5",		BIT_DIGITAL,	SMSJoy1 + 10,	"p1 fire 5"	},
*///	{"P1 Button 6",		BIT_DIGITAL,	SMSJoy1 + 11,	"p1 fire 6"	},

};

STDINPUTINFO(SMS)

static struct BurnRomInfo sms_akmwRomDesc[] = {
//	{ "sms_akmw.sms", 0x20000, 0xAED9AAC4, BRF_PRG | BRF_ESS },
};

STD_ROM_PICK(sms_akmw)
STD_ROM_FN(sms_akmw)
#endif
