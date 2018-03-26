#ifndef _D_BLKTGR_H_
#define _D_BLKTGR_H_

#include "burnint.h"
#include "saturn/ovl.h"
#include "raze/raze.h"

int ovlInit(char *szShortName) __attribute__ ((boot,section(".boot")));
INT32 BurnTimerUpdate(INT32 *nCycles);
void BurnTimerEndFrame(INT32 *nCycles);
//INT32 BurnTimerUpdate(INT32 nCycles);
static INT32 DrvInit();
static INT32 DrvFMInit();
static INT32 DrvFMFrame();
static INT32 DrvExit();
static INT32 DrvFrame();
static INT32 DrvDraw();
static void make_lut(void);
static void DrvInitSaturn();
static void draw_sprites();
static void tile16x16toSaturn (unsigned char reverse, unsigned int num, unsigned char *pDest);
static void Set14PCM();
static PcmHn createHandle(PcmCreatePara *para);
void updateBgTile2Words(/*INT32 type,*/ UINT32 offs);
void errGfsFunc(void *obj, Sint32 ec);
void errStmFunc(void *obj, Sint32 ec);
void errPcmFunc(void *obj, Sint32 ec);

#define nBurnSoundLen 128
#define SOUNDRATE   7680L
//---------------------------------------------------------------------------------------------------------------
unsigned char current_pcm=255;
char *itoa(int i);

typedef struct
{
	int position;
	int size;
	unsigned char loop;
}SFX;

typedef struct
{
	int track_position;
	int position;
	int ring_position;
	int size;
	unsigned char num;
}PCM_INFO;

PCM_INFO pcm_info[14];

SFX sfx_list[68]=
{
	{0,0,0},
/*001.pcm*/{0,10290,0},
/*002.pcm*/{10290,27654,0},
/*003.pcm*/{37944,4376,0},
/*004.pcm*/{42320,6904,0},
/*005.pcm*/{49224,9786,0},
/*006.pcm*/{59010,9306,0},
/*007.pcm*/{68316,7666,0},
/*008.pcm*/{75982,5368,0},
/*009.pcm*/{81350,9990,0},
/*010.pcm*/{91340,5610,0},
/*011.pcm*/{96950,6440,0},
/*012.pcm*/{103390,3762,0},
/*013.pcm*/{107152,10910,0},
/*014.pcm*/{118062,6230,0},
/*015.pcm*/{124292,8420,0},
/*016.pcm*/{132712,6138,0},
/*017.pcm*/{138850,12274,0},
/*018.pcm*/{151124,10140,0},
/*019.pcm*/{161264,7252,0},
/*020.pcm*/{168516,9980,0},
/*021.pcm*/{178496,6582,0},
/*022.pcm*/{185078,5528,0},
/*023.pcm*/{190606,12284,0},
/*024.pcm*/{202890,16966,0},
/*025.pcm*/{219856,19194,0},
/*026.pcm*/{239050,3068,0},
/*027.pcm*/{242118,4946,0},
/*028.pcm*/{247064,3530,0},
/*029.pcm*/{250594,13038,0},
/*030.pcm*/{263632,5566,0},
	{-1,0,0},
/*032.pcm*/{269198,23024,0},	
/*033.pcm*/{0,1167350,1},
/*034.pcm*/{0,1176290,1},
/*035.pcm*/	{0,844802,1},	
	{-1,0,0},	{-1,0,0},	{-1,0,0},	{-1,0,0},	{-1,0,0},
	{-1,0,0},	{-1,0,0},	{-1,0,0},	
/*044.pcm*/{0,606742,1},	
	{-1,0,0},	{-1,0,0},	{-1,0,0},	
/*048.pcm*/{0,365708,1},
	{-1,0,0},	{-1,0,0},	{-1,0,0},	{-1,0,0},	{-1,0,0},	{-1,0,0},	{-1,0,0},	{-1,0,0},	{-1,0,0},
/*058.pcm*/{292222,5556,0},	
/*059.pcm*/{297778,58828,0},	
/*060.pcm*/{356606,9620,0},	
/*061.pcm*/{366226,14790,0},	
/*062.pcm*/{381016,9362,0},	
/*063.pcm*/{390378,33790,0},	
/*064.pcm*/{424168,6428,0},	
/*065.pcm*/{430596,27342,0},	
/*066.pcm*/{457938,4290,0},	
/*067.pcm*/{462228,9208,0},	
};
//---------------------------------------------------------------------------------------------------------------
#define VDP2_BASE           0x25e00000
#define VDP2_REGISTER_BASE  (VDP2_BASE+0x180000)
#define BGON    (*(volatile unsigned short *)(VDP2_REGISTER_BASE+0x20))
#define PLANEADDR1 (*(volatile unsigned short *)(VDP2_REGISTER_BASE+0x44)) 
#define PLANEADDR2 (*(volatile unsigned short *)(VDP2_REGISTER_BASE+0x46)) 
#define PLSZ    (*(volatile unsigned short *)(VDP2_REGISTER_BASE+0x3a))
#define VDP2_VRAM           VDP2_BASE
#define VDP2_CRAM           (VDP2_BASE+0x100000)
#define PNCN1   (*(volatile unsigned short *)(VDP2_REGISTER_BASE+0x32))

//#define SND 1
/*static*/ UINT8 *CZ80Context = NULL;
/*static*/ UINT16 *remap4to16_lut = NULL;//[256];
/*static*/ UINT16 *remap16_lut = NULL;//[768];
/*static*/ UINT16 *cram_lut = NULL;//[4096];
/*static*/ UINT16 *fg_map_lut = NULL;//[0x400];
/*static*/ UINT16 *bg_map_lut2x1 = NULL;//[0x2000];
/*static*/ UINT16 *bg_map_lut2x2 = NULL;//[0x2000];
/*static*/ UINT16 *bg_map_lut = NULL;
/*static*/ INT16 *ym_buffer = NULL;

/*static*/ UINT8 *AllMem = NULL;
/*static*/ UINT8 *MemEnd = NULL;
/*static*/ UINT8 *AllRam = NULL;
/*static*/ UINT8 *RamEnd = NULL;
/*static*/ UINT8 *DrvZ80ROM0 = NULL;
/*static*/ UINT8 *DrvZ80ROM1 = NULL;
/*static*/ UINT8 *DrvGfxROM0 = NULL;
/*static*/ UINT8 *DrvGfxROM1 = NULL;
/*static*/ UINT8 *DrvGfxROM2 = NULL;
/*static*/ UINT8 *DrvZ80RAM0 = NULL;
/*static*/ UINT8 *DrvZ80RAM1 = NULL;
/*static*/ UINT8 *DrvSprRAM = NULL;
/*static*/ UINT8 *DrvSprBuf = NULL;
/*static*/ UINT8 *DrvPalRAM = NULL;
/*static*/ UINT8 *DrvBgRAM = NULL;
/*static*/ UINT8 *DrvTxRAM = NULL;
/*static*/ UINT16 *DrvPalette = NULL;

/*static*/ UINT8 *DrvScreenLayout = NULL;
/*static*/ UINT8 *DrvBgEnable = NULL;
/*static*/ UINT8 *DrvFgEnable = NULL;
/*static*/ UINT8 *DrvSprEnable = NULL;
/*static*/ UINT8 *DrvVidBank = NULL;
/*static*/ UINT8 *DrvRomBank = NULL;

/*static*/ UINT8 *soundlatch = NULL;
/*static*/ UINT8 *flipscreen = NULL;
/*static*/ UINT16 *DrvScrollx = NULL;
/*static*/ UINT16 *DrvScrolly = NULL;
/*static*/ UINT8 coin_lockout = 0;

/*static*/ UINT8 DrvJoy1[8] = {0,0,0,0,0,0,0,0};
/*static*/ UINT8 DrvJoy2[8] = {0,0,0,0,0,0,0,0};
/*static*/ UINT8 DrvJoy3[8] = {0,0,0,0,0,0,0,0};
/*static*/ UINT8 DrvDips[3] = {0,0,0};
/*static*/ UINT8 DrvInputs[3] = {0,0,0};
/*static*/ UINT8 DrvReset = 0;
/*static*/ INT32 watchdog = 0;

//static INT32 nCyclesTotal[2];

static struct BurnInputInfo DrvInputList[] = {
	{"Coin 1"       	  , BIT_DIGITAL  , DrvJoy1 + 6,	 "p1 coin"  },
	{"Coin 2"       	  , BIT_DIGITAL  , DrvJoy1 + 7,	 "p2 coin"  },

	{"P1 Start"     	  , BIT_DIGITAL  , DrvJoy1 + 0,	 "p1 start" },

	{"P1 Up"        	  , BIT_DIGITAL  , DrvJoy2 + 3,  "p1 up"    },
	{"P1 Down"      	  , BIT_DIGITAL  , DrvJoy2 + 2,  "p1 down"  },
	{"P1 Left"      	  , BIT_DIGITAL  , DrvJoy2 + 1,  "p1 left"  },
	{"P1 Right"     	  , BIT_DIGITAL  , DrvJoy2 + 0,  "p1 right" },
	{"P1 Button 1"  	  , BIT_DIGITAL  , DrvJoy2 + 4,  "p1 fire 1"},
	{"P1 Button 2"  	  , BIT_DIGITAL  , DrvJoy2 + 5,  "p1 fire 2"},

	{"P2 Start"     	  , BIT_DIGITAL  , DrvJoy1 + 1,	 "p2 start" },

	{"P2 Up"        	  , BIT_DIGITAL  , DrvJoy3 + 3,  "p2 up"    },
	{"P2 Down"      	  , BIT_DIGITAL  , DrvJoy3 + 2,  "p2 down"  },
	{"P2 Left"      	  , BIT_DIGITAL  , DrvJoy3 + 1,  "p2 left"  },
	{"P2 Right"     	  , BIT_DIGITAL  , DrvJoy3 + 0,  "p2 right" },
	{"P2 Button 1"  	  , BIT_DIGITAL  , DrvJoy3 + 4,  "p2 fire 1"},
	{"P2 Button 2"  	  , BIT_DIGITAL  , DrvJoy3 + 5,  "p2 fire 2"},

	{"Reset",		    BIT_DIGITAL  , &DrvReset,	 "reset"    },
	{"Service",		    BIT_DIGITAL  , DrvJoy1 + 5,  "service"  },
	{"Dip 1",		    BIT_DIPSWITCH, DrvDips + 0,	 "dip"	    },
	{"Dip 2",		    BIT_DIPSWITCH, DrvDips + 1,	 "dip"	    },
	{"Dip 3",		    BIT_DIPSWITCH, DrvDips + 2,	 "dip"	    },
};

STDINPUTINFO(Drv)

static struct BurnDIPInfo DrvDIPList[]=
{
	{0x12, 0xff, 0xff, 0xff, NULL			},
 	{0x13, 0xff, 0xff, 0xff, NULL			},
 	{0x14, 0xff, 0x01, 0x01, NULL			},

 	{0x12, 0xfe,    0,    8, "Coin A"		},
 	{0x12, 0x01, 0x07, 0x00, "4 Coin 1 Credits"	},
 	{0x12, 0x01, 0x07, 0x01, "3 Coin 1 Credits"	},
 	{0x12, 0x01, 0x07, 0x02, "2 Coin 1 Credits"	},
 	{0x12, 0x01, 0x07, 0x07, "1 Coin 1 Credits"	},
 	{0x12, 0x01, 0x07, 0x06, "1 Coin 2 Credits"	},
 	{0x12, 0x01, 0x07, 0x05, "1 Coin 3 Credits"	},
 	{0x12, 0x01, 0x07, 0x04, "1 Coin 4 Credits"	},
 	{0x12, 0x01, 0x07, 0x03, "1 Coin 5 Credits"	},

 	{0x12, 0xfe,    0,    8, "Coin B"		},
 	{0x12, 0x01, 0x38, 0x00, "4 Coin 1 Credits"	},
 	{0x12, 0x01, 0x38, 0x08, "3 Coin 1 Credits"	},
 	{0x12, 0x01, 0x38, 0x10, "2 Coin 1 Credits"	},
 	{0x12, 0x01, 0x38, 0x38, "1 Coin 1 Credits"	},
 	{0x12, 0x01, 0x38, 0x30, "1 Coin 2 Credits"	},
 	{0x12, 0x01, 0x38, 0x28, "1 Coin 3 Credits"	},
 	{0x12, 0x01, 0x38, 0x20, "1 Coin 4 Credits"	},
 	{0x12, 0x01, 0x38, 0x18, "1 Coin 5 Credits"	},

 	{0x12, 0xfe,    0,    2, "Flip Screen"		},
 	{0x12, 0x01, 0x40, 0x40, "Off"			},
 	{0x12, 0x01, 0x40, 0x00, "On"			},

 	{0x12, 0xfe,    0,    2, "Test"			},
 	{0x12, 0x01, 0x80, 0x80, "Off"			},
 	{0x12, 0x01, 0x80, 0x00, "On"			},

 	{0x13, 0xfe,    0,    4, "Lives"		},
 	{0x13, 0x01, 0x03, 0x02, "2"			},
 	{0x13, 0x01, 0x03, 0x03, "3"			},
 	{0x13, 0x01, 0x03, 0x01, "5"			},
 	{0x13, 0x01, 0x03, 0x00, "7"			},

 	{0x13, 0xfe,    0,    8, "Difficulty"		},
 	{0x13, 0x01, 0x1c, 0x1c, "1 (Easiest)"		},
 	{0x13, 0x01, 0x1c, 0x18, "2"			},
 	{0x13, 0x01, 0x1c, 0x14, "3"			},
 	{0x13, 0x01, 0x1c, 0x10, "4"			},
 	{0x13, 0x01, 0x1c, 0x0c, "5 (Normal)"		},
 	{0x13, 0x01, 0x1c, 0x08, "6"			},
 	{0x13, 0x01, 0x1c, 0x04, "7"			},
 	{0x13, 0x01, 0x1c, 0x00, "8 (Hardest)"		},

 	{0x13, 0xfe,    0,    2, "Demo Sounds"		},
 	{0x13, 0x01, 0x20, 0x00, "Off"			},
 	{0x13, 0x01, 0x20, 0x20, "On"			},

 	{0x13, 0xfe,    0,    2, "Allow Continue"	},
 	{0x13, 0x01, 0x40, 0x00, "No"			},
 	{0x13, 0x01, 0x40, 0x40, "Yes"			},

 	{0x13, 0xfe,    0,    2, "Cabinet"		},
 	{0x13, 0x01, 0x80, 0x00, "Upright"		},
 	{0x13, 0x01, 0x80, 0x80, "Cocktail"		},

 	{0x14, 0xfe,    0,    2, "Coin Lockout Present?"},
 	{0x14, 0x01, 0x01, 0x01, "Yes"			},
 	{0x14, 0x01, 0x01, 0x00, "No"			},
};

STDDIPINFO(Drv)

// Black Tiger

static struct BurnRomInfo blktigerRomDesc[] = {
	{ "bdu01a.5e",		0x08000, 0xa8f98f22, 1 | BRF_PRG | BRF_ESS }, //  0 - Z80 #0 Code
	{ "bdu02a.6e",		0x10000, 0x7bef96e8, 1 | BRF_PRG | BRF_ESS }, //  1
	{ "bdu03a.8e",		0x10000, 0x4089e157, 1 | BRF_PRG | BRF_ESS }, //  2
	{ "bd04.9e",		0x10000, 0xed6af6ec, 1 | BRF_PRG | BRF_ESS }, //  3
	{ "bd05.10e",		0x10000, 0xae59b72e, 1 | BRF_PRG | BRF_ESS }, //  4

	{ "bd06.1l",		0x08000, 0x2cf54274, 2 | BRF_PRG | BRF_ESS }, //  5 - Z80 #0 Code

	{ "bd15.2n",		0x08000, 0x70175d78, 3 | BRF_GRA },           //  6 - Characters

	{ "bd12.5b",		0x10000, 0xc4524993, 4 | BRF_GRA },           //  7 - Background Tiles
	{ "bd11.4b",		0x10000, 0x7932c86f, 4 | BRF_GRA },           //  8
	{ "bd14.9b",		0x10000, 0xdc49593a, 4 | BRF_GRA },           //  9
	{ "bd13.8b",		0x10000, 0x7ed7a122, 4 | BRF_GRA },           // 10

	{ "bd08.5a",		0x10000, 0xe2f17438, 5 | BRF_GRA },           // 11 - Sprites
	{ "bd07.4a",		0x10000, 0x5fccbd27, 5 | BRF_GRA },           // 12
	{ "bd10.9a",		0x10000, 0xfc33ccc6, 5 | BRF_GRA },           // 13
	{ "bd09.8a",		0x10000, 0xf449de01, 5 | BRF_GRA },           // 14

	{ "bd01.8j",		0x00100, 0x29b459e5, 6 | BRF_OPT },           // 15 - Proms (not used)
	{ "bd02.9j",		0x00100, 0x8b741e66, 6 | BRF_OPT },           // 16
	{ "bd03.11k",		0x00100, 0x27201c75, 6 | BRF_OPT },           // 17
	{ "bd04.11l",		0x00100, 0xe5490b68, 6 | BRF_OPT },           // 18

	{ "bd.6k",  		0x01000, 0xac7d14f1, 7 | BRF_PRG | BRF_OPT }, // 19 I8751 Mcu Code
};

STD_ROM_PICK(blktiger)
STD_ROM_FN(blktiger)
#endif

//---------------------------------------------------------------------------------------------------------------
typedef struct{
    Uint8 mode;                             /* ｽﾃﾚｵ･ﾓﾉｸﾛ+ｻﾝﾌﾟﾘﾝｸﾞﾚｰﾄ         */
    Uint16 sadr;                            /* PCMｽﾄﾘｰﾑﾊﾞｯﾌｧｽﾀｰﾄｱﾄﾞﾚｽ        */
    Uint16 size;                            /* PCMｽﾄﾘｰﾑﾊﾞｯﾌｧｻｲｽﾞ             */
}SndPcmStartPrm;                            /* PCM開始パラメータ          */

typedef Uint8 SndPcmNum;                    /* PCMｽﾄﾘｰﾑ再生番号               */
typedef Uint8 SndEfctIn;                    /* Efect in select               */
typedef Uint8 SndLev;                       /* Levelデータ型                 */
typedef Sint8 SndPan;                       /* PANデータ型                   */
typedef Uint8 SndRet;                       /* ｺﾏﾝﾄﾞ実行状態データ型         */
typedef Uint8 SndPcmNum;                    /* PCMｽﾄﾘｰﾑ再生番号               */

typedef struct{
    SndPcmNum num;                          /* PCMｽﾄﾘｰﾑ再生番号               */
    SndLev lev;                             /* ﾀﾞｲﾚｸﾄ音Level                 */
    SndPan pan;                             /* ﾀﾞｲﾚｸﾄ音pan                   */
    Uint16 pich;                            /* PICHﾜｰﾄﾞ                      */
    SndEfctIn r_efct_in;                    /* Efect in select(右出力)       */
    SndLev r_efct_lev;                      /* Efect send Level(右出力)      */
    SndEfctIn l_efct_in;                    /* Efect in select(左出力)       */
    SndLev l_efct_lev;                      /* Efect send Level(左出力)      */
}SndPcmChgPrm;                              /* PCM変更パラメータ           */

void vbt_pcm_StartTimer(PcmHn hn);
void vbt_pcm_EndProcess(PcmHn hn);
void vbt_PCM_DrvStopPcm(PcmHn hn);
void vbt_PCM_MeStop(PcmHn hn);
void vbt_PCM_MeTask(PcmHn hn);
static void vbt_pcm_AudioMix(PcmHn hn);
SndRet vbt_SND_StartPcm(SndPcmStartPrm *sprm, SndPcmChgPrm *cprm);
void vbt_PCM_DrvStartPcm(PcmHn hn);
void vbt_pcm_AudioProcess(PcmHn hn);


static void pcm_Wait(int cnt);
static Uint8 GetComBlockAdr(void);
static Uint16 ChgPan(SndPan pan);

PcmHn 	pcm14[14];
PcmCreatePara	para[14];
static volatile Uint8  *adr_com_block;
static volatile Uint8 *adr_host_int_work;                /* ﾎｽﾄｲﾝﾀﾌｪｰｽﾜｰｸ先頭ｱﾄﾞﾚｽ格納*/
static Uint32 intrflag;
extern signed int pcm_cnt_vbl_in;

UINT8   stm_work[STM_WORK_SIZE(12, 24)];
//UINT8   stm_work[STM_WORK_SIZE(4, 20)];
StmHn stm;
StmGrpHn grp_hd;
void stmInit(void);
void stmClose(StmHn fp);
StmHn stmOpen(char *fname);

#define PCM_IS_LRLRLR(st)	((st)->info.data_type == PCM_DATA_TYPE_LRLRLR)
#define PCM_IS_RLRLRL(st)	((st)->info.data_type == PCM_DATA_TYPE_RLRLRL)
#define PCM_IS_LLLRRR(st)	((st)->info.data_type == PCM_DATA_TYPE_LLLRRR)
#define PCM_IS_RRRLLL(st)	((st)->info.data_type == PCM_DATA_TYPE_RRRLLL)
#define PCM_IS_ADPCM_SG(st)	((st)->info.data_type == PCM_DATA_TYPE_ADPCM_SG)
#define PCM_IS_ADPCM_SCT(st) ((st)->info.data_type == PCM_DATA_TYPE_ADPCM_SCT)
#define PCM_IS_LR_MIX(st) 	(PCM_IS_LRLRLR(st) || PCM_IS_RLRLRL(st))
#define PCM_IS_LR_BLOCK(st) (PCM_IS_LLLRRR(st) || PCM_IS_RRRLLL(st))
#define PCM_IS_ADPCM(st) 	(PCM_IS_ADPCM_SG(st) || PCM_IS_ADPCM_SCT(st))
#define PCM_IS_MONORAL(st)	((st)->info.channel == 0x01)
#define PCM_IS_8BIT_SAMPLING(st)	((st)->info.sampling_bit <= 0x08)
#define PCM_SAMPLE2BSIZE(st, sample)	\
			(PCM_IS_8BIT_SAMPLING(st) ? (sample) : (sample) << 1)
#define PCM_1CH2NCH(st, a)	(PCM_IS_MONORAL(st) ? (a) : (a) << 1)

#define PCM_SCSP_FREQUENCY					(44100L)

static const Sint8	logtbl[] = {
/* 0 */		0, 
/* 1 */		1, 
/* 2 */		2, 2, 
/* 4 */		3, 3, 3, 3, 
/* 8 */		4, 4, 4, 4, 4, 4, 4, 4, 
/* 16 */	5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 
/* 32 */	6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 
			6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 
/* 64 */	7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
			7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 
/* 128 */	8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 
			8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	};

/* 1,3,4,5,10 bit mask */
#define PCM_MSK1(a)				((a)&0x0001)
#define PCM_MSK3(a)				((a)&0x0007)
#define PCM_MSK4(a)				((a)&0x000F)
#define PCM_MSK5(a)				((a)&0x001F)
#define PCM_MSK10(a)			((a)&0x03FF)

/* オクターブ値の計算 */
#define PCM_CALC_OCT(smpling_rate) 											\
		((Sint32)logtbl[PCM_SCSP_FREQUENCY / ((smpling_rate) + 1)])

/* シフト基準周波数の計算 */
#define PCM_CALC_SHIFT_FREQ(oct)											\
		(PCM_SCSP_FREQUENCY >> (oct))

/* ＦＮＳの計算 */
#define PCM_CALC_FNS(smpling_rate, shift_freq)								\
		((((smpling_rate) - (shift_freq)) << 10) / (shift_freq))
#define PCM_SOUND_BASE_ADDR		(0x25A00000)
#define PCM_SYS_IF_TBL			(PCM_SOUND_BASE_ADDR + 0x400)
#define PCM_SYS_IF_WORK			(PCM_SOUND_BASE_ADDR + 0x480)
#define PCM_HOST_IF_WORK		(PCM_SOUND_BASE_ADDR + *(Uint32 *)(PCM_SYS_IF_TBL + 4))
#define PCM_CMD_BLK(n)			(PCM_HOST_IF_WORK + 0x10 * (n))
#define PCM_PLAY_ADDRESS(n)		(*(Uint8 *)(PCM_HOST_IF_WORK + 0xA0 + 2 * (n)))
#define PCM_COMMAND_START_PCM				(0x85)
#define SYS_GETSYSCK \
        (*(volatile Uint32*)0x6000324)
#define PCM_IS_26MHZ		(SYS_GETSYSCK == 0)
#define PCM_IS_PAL			(*(volatile Uint16 *)0x25f80004 & 0x0001)
#define PCM_CLK_CPU_NTSC26	(26874100L)
#define PCM_CLK_CPU_NTSC28	(28636400L)
#define PCM_CLK_CPU_PAL26	(26687500L)
#define PCM_CLK_CPU_PAL28	(28437500L)
#define PCM_CLK_CPU_NTSC	(PCM_IS_26MHZ ? PCM_CLK_CPU_NTSC26 : PCM_CLK_CPU_NTSC28)
#define PCM_CLK_CPU_PAL		(PCM_IS_26MHZ ? PCM_CLK_CPU_PAL26 : PCM_CLK_CPU_PAL28)
#define PCM_CLK_CPU			(PCM_IS_PAL ? PCM_CLK_CPU_PAL : PCM_CLK_CPU_NTSC)
#define PCM_ROUND_SHIFT_R(a, n)		((((a) >> ((n) - 1)) + 1) >> 1)
#define PCM_CLOCK_SCALE		(PCM_ROUND_SHIFT_R(PCM_CLK_CPU, 7 + 8))
#define PCM_IS_16BIT_SAMPLING(st)	(((st)->info.sampling_bit <= 0x10) && \
									((st)->info.sampling_bit > 0x08))
#define PCM_SET_STMNO(para)													\
		((Uint8)PCM_MSK3((para)->pcm_stream_no))
#define PCM_SET_LEVEL_PAN(para)												\
		((Uint8)((PCM_MSK3((para)->pcm_level) << 5) | PCM_MSK5((para)->pcm_pan)))
#define PCM_SET_LEVEL_PAN2(level, pan)										\
		((Uint8)((PCM_MSK3(level) << 5) | PCM_MSK5(pan)))
#define PCM_SET_PITCH_WORD(oct, fns)										\
		((Uint16)((PCM_MSK4(-(oct)) << 11) | PCM_MSK10(fns)))
#define PCM_SET_PCM_ADDR(para) 	(((Uint32)((para)->pcm_addr)) >> 4)
#define PCM_SET_PCM_SIZE(para) 	((para)->pcm_size)
#define SND_PRM_NUM(prm)        ((prm).num)         /* PCMｽﾄﾘｰﾑ再生番号       */
#define SND_PRM_LEV(prm)        ((prm).lev)         /* ﾀﾞｲﾚｸﾄ音Level         */
#define SND_PRM_PAN(prm)        ((prm).pan)         /* ﾀﾞｲﾚｸﾄ音pan           */
#define SND_PRM_PICH(prm)       ((prm).pich)        /* PICHﾜｰﾄﾞ              */
#define SND_R_EFCT_IN(prm)      ((prm).r_efct_in)   /* Efect in select(右)   */
#define SND_R_EFCT_LEV(prm)     ((prm).r_efct_lev)  /* Efect send Level(右)  */
#define SND_L_EFCT_IN(prm)      ((prm).l_efct_in)   /* Efect in select(左)   */
#define SND_L_EFCT_LEV(prm)     ((prm).l_efct_lev)  /* Efect send Level(左)  */
#define SND_PRM_TL(prm)     	((prm).lev)  		/* Master Level			 */
#define SND_PRM_MODE(prm)       ((prm).mode)        /* ｽﾃﾚｵ･ﾓﾉｸﾛ+ｻﾝﾌﾟﾘﾝｸﾞﾚｰﾄ */
#define SND_PRM_SADR(prm)       ((prm).sadr)        /* PCMｽﾄﾘｰﾑﾊﾞｯﾌｧｽﾀｰﾄｱﾄﾞﾚｽ*/
#define SND_PRM_SIZE(prm)       ((prm).size)        /* PCMｽﾄﾘｰﾑﾊﾞｯﾌｧｻｲｽﾞ   */
#define SND_PRM_OFSET(prm)      ((prm).ofset)       /* PCMｽﾄﾘｰﾑ再生開始ｵﾌｾｯﾄ */
#define SND_RET_SET     0                       /* 正常終了                     */
#define SND_RET_NSET    1                       /* 異常終了                     */
#define COM_START_PCM      0x85                 /* PCM start                 */
#define ADR_COM_DATA    (0x00)                  /* コマンド                  */
#define ADR_PRM_DATA    (0x02)                  /* パラメータ                */
#define ADR_HOST_INT    (0x04)                  /* ﾎｽﾄｲﾝﾀﾌｪｰｽﾜｰｸｱﾄﾞﾚｽ        */
#define ADR_SND_MEM     ((Uint8 *)0x25a00000)   /* サウンドメモリ先頭アドレス*/
#define ADR_SND_VECTOR  ((Uint8 *)0x25a00000)   /* サウンドベクタアドレス    */
//#define ADR_SYS_TBL     (ADR_SND_MEM + 0x400)   /* ｼｽﾃﾑｲﾝﾀﾌｪｰｽ領域           */
#define ADR_SYS_TBL     (ADR_SND_MEM + 0x400)   /* ｼｽﾃﾑｲﾝﾀﾌｪｰｽ領域           */
#define SIZE_COM_BLOCK      (0x10)              /* ｺﾏﾝﾄﾞﾌﾞﾛｯｸｻｲｽﾞ          */
#define MAX_NUM_COM_BLOCK   8                   /* ｺﾏﾝﾄﾞﾌﾞﾛｯｸ数              */


#define MAX_ADR_COM_DATA                        /* 最大ｺﾏﾝﾄﾞﾃﾞｰﾀｱﾄﾞﾚｽ     */\
    (adr_host_int_work + ADR_COM_DATA + (SIZE_COM_BLOCK * MAX_NUM_COM_BLOCK))
#define NOW_ADR_COM_DATA                        /* 現在ｺﾏﾝﾄﾞﾃﾞｰﾀｱﾄﾞﾚｽ     */\
    (adr_com_block + ADR_COM_DATA)
#define HOST_SET_RETURN(ret)\
    do{\
		intrflag=0;\
        return(ret);\
    }while(FALSE)

#define POKE_B(adr, data)   (*((volatile Uint8 *)(adr)) = ((Uint8)(data)))   /* ﾊﾞｲﾄ  */
#define POKE_W(adr, data)   (*((volatile Uint16 *)(adr)) = ((Uint16)(data))) /* ﾜｰﾄﾞ  */

#define PEEK_L(adr)         (*((volatile Uint32 *)(adr)))                    /* ﾛﾝｸﾞ  */


#define SET_COMMAND(set_com)\
(POKE_W((adr_com_block + ADR_COM_DATA), (Uint16)(set_com) << 8)) /* コマンドセット   */

#define SET_PRM(no, set_prm)\
(POKE_B(adr_com_block + ADR_PRM_DATA + (no), (set_prm))) /* ﾊﾟﾗﾒｰﾀセット      */
//---------------------------------------------------------------------------------------------------------------
static void wait_vblank(void)
{
     while((TVSTAT & 8) == 0);
     while((TVSTAT & 8) == 8);
}

//---------------------------------------------------------------------------------------------------------------
static void pcm_Wait(int cnt)
{
	while (--cnt > 0) {
		;
	}
}
//---------------------------------------------------------------------------------------------------------------
static Uint16 ChgPan(SndPan pan)
{
    return(((pan) < 0) ? (~(pan) + 0x10 + 1) : (pan));
}
//---------------------------------------------------------------------------------------------------------------
#define INT_DIGITS 19
char *itoa(int i)
{
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {			/* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}

//PcmInfo 		info[14];
//#undef pcm_AudioProcess
//#define pcm_AudioProcess vbt_pcm_AudioProcess
//---------------------------------------------------------------------------------------------------------------
