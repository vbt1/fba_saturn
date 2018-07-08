//#include "../burn.h"
//#define SC_RELEASE 1
#define		_SPR3_
#include "sega_spr.h"
#include "sega_scl2.h"
#include "sega_pcm.h"

#include "../globals.h"

#ifndef _SATURN_SND_H_
#define _SATURN_SND_H_

#define PCM_BLOCK_SIZE 0x4000 // 0x2000
#define	PCM_ADDR	((void*)0x25a20000)
#define	PCM_SIZE	(4096L*2)				/* 2.. */
#define PCM_COPY_SIZE (4096L*2)
#define PCM_000_SIZE 230400
#define	RING_BUF_SIZE	(2048L*5)//(2048L*10)
#define SOUND_BUFFER    0x25a20000
#define SOUNDRATE   7680L

void SetStreamPCM();
void wait_vblank(void);

void errGfsFunc(void *obj, Sint32 ec);
void errStmFunc(void *obj, Sint32 ec);
void errPcmFunc(void *obj, Sint32 ec);

unsigned char stm_work[STM_WORK_SIZE(12, 24)];
//UINT8   stm_work[STM_WORK_SIZE(4, 20)];
StmHn stm;
StmGrpHn grp_hd;
void stmInit(void);
void stmClose(StmHn fp);
StmHn stmOpen(char *fname);
PcmHn 	pcmStream;
PcmCreatePara	paraStream;

#endif
