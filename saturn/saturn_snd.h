//#include "../burn.h"
//#define SC_RELEASE 1
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#define		_SPR3_
#include "sega_spr.h"
#include "sega_scl2.h"
#include "sega_pcm.h"
//#include "pcm_mem.h"
#include "../globals.h"

#ifndef _SATURN_SND_H_
#define _SATURN_SND_H_

#define PCM_BLOCK_SIZE 0x4000 // 0x2000
#define	PCM_ADDR	((void*)0x25a20000)
#define	PCM_SIZE	(4096L*2)				/* 2.. */
#define PCM_COPY_SIZE (4096L*2)
#define PCM_000_SIZE 230400L
#define	RING_BUF_SIZE	(2048L*5)//(2048L*10)
#define SOUND_BUFFER    0x25a20000
#define SOUNDRATE   7680L

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


void SetStreamPCM();
void PlayStreamPCM(unsigned char d, unsigned char current_pcm);
void UpdateStreamPCM(Uint8 data, PcmHn *hn, PcmCreatePara *para);
void playMusic(PcmHn *hn);
void wait_vblank(void);
//char *itoa(int i);

void errGfsFunc(void *obj, Sint32 ec);
void errStmFunc(void *obj, Sint32 ec);
void errPcmFunc(void *obj, Sint32 ec);

void stmInit(void);
void stmClose(StmHn fp);

StmHn stmOpen(char *fname);

extern PcmHn 	pcmStream;
extern PcmCreatePara	paraStream;
extern unsigned char stm_work[STM_WORK_SIZE(12, 24)];
extern StmHn stm;
//extern StmGrpHn grp_hd;
extern void PCM_MeTask(PcmHn hn);
extern void PCM_MeStop(PcmHn hn);
extern SFX *sfx_list;
#endif
