#define DEBUG 1
#include "saturn_snd.h"
#include <stdarg.h>
PcmHn 	pcmStream;
PcmCreatePara	paraStream;
unsigned char stm_work[STM_WORK_SIZE(12, 24)];
StmHn stm;
StmGrpHn grp_hd;
SFX *sfx_list;

void vout2(char *string, char *fmt, ...)                                         
{                                                                               
   va_list arg_ptr;                                                             
   va_start(arg_ptr, fmt);                                                      
   vsprintf(string, fmt, arg_ptr);                                              
   va_end(arg_ptr);                                                             
}
//-------------------------------------------------------------------------------------------------------------------------------------
void stmInit(void)
{
	STM_Init(12, 24, stm_work);
//	STM_Init(4, 20, stm_work);
//	STM_Init(1, 2, stm_work);
#ifdef DEBUG
	STM_SetErrFunc(errStmFunc, NULL);
	GFS_SetErrFunc(errGfsFunc, NULL);
	PCM_SetErrFunc(errPcmFunc, NULL);	
#endif
	grp_hd = STM_OpenGrp();
	if (grp_hd == NULL) {
		return;
	}
	STM_SetLoop(grp_hd, STM_LOOP_DFL, STM_LOOP_ENDLESS);
	STM_SetExecGrp(grp_hd);

	stm = stmOpen("000.PCM");
	STM_ResetTrBuf(stm);
}
//-------------------------------------------------------------------------------------------------------------------------------------
StmHn stmOpen(char *fname)
{
    Sint32 fid;
	StmKey key;

    fid = GFS_NameToId((Sint8 *)fname);
	STM_KEY_FN(&key) = STM_KEY_CN(&key) = STM_KEY_SMMSK(&key) = 
		STM_KEY_SMVAL(&key) = STM_KEY_CIMSK(&key) = STM_KEY_CIVAL(&key) =
		STM_KEY_NONE;
	return STM_OpenFid(grp_hd, fid, &key, STM_LOOP_READ); //STM_LOOP_NOREAD);
//	return STM_OpenFid(grp_hd, fid, &key, STM_LOOP_NOREAD);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void stmClose(StmHn fp)
{
	STM_Close(fp);
}
//-------------------------------------------------------------------------------------------------------------------------------------
#ifdef DEBUG
void vout(char *string, char *fmt, ...)                                         
{                                                                               
   va_list arg_ptr;                                                             
   va_start(arg_ptr, fmt);                                                      
   vsprintf(string, fmt, arg_ptr);                                              
   va_end(arg_ptr);                                                             
}

void errStmFunc(void *obj, Sint32 ec)
{
//	VTV_PRINTF((VTV_s, "S:ErrStm %X %X\n", obj, ec));
	char texte[50];
	vout(texte, "ErrStm %X %X",obj, ec); 
	texte[49]='\0';
	do{
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"plante stm                         ",40,120);
/*

		switch (state)
	{
		case CPUINFO_INT_INPUT_STATE + INPUT_LINE_NMI:
	if(st->play ==PCM_STAT_PLAY_ERR_STOP)
			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"errstp",40,40);
	else if (st->play ==PCM_STAT_PLAY_CREATE)
			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"create",40,40);

enum StmErrCode {
    STM_ERR_OK = GFS_ERR_OK, / * Normal completion * /
    STM_ERR_CDRD = GFS_ERR_CDRD, / * CD read error * /
    STM_ERR_CDNODISC = GFS_ERR_CDNODISC, / * CD not set * /
    STM_ERR_FID = GFS_ERR_FID, / * File identifier invalid * /
    STM_ERR_HNDL = GFS_ERR_HNDL, / * Handle is invalid * /
    STM_ERR_NUM = GFS_ERR_NUM, / * The number of bytes is negative * /
    STM_ERR_PARA = GFS_ERR_PARA, / * parameter is invalid * /
    STM_ERR_NOHNDL = GFS_ERR_NOHNDL, / * there is no available space on the handle * /
    STM_ERR_PUINUSE = GFS_ERR_PUINUSE, / * Pickup operation in progress * /
    STM_ERR_TMOUT = GFS_ERR_TMOUT, / * timeout * /
    STM_ERR_CDOPEN = GFS_ERR_CDOPEN, / * Tray is open * /
    STM_ERR_FATAL = GFS_ERR_FATAL, / * CD is FATAL state * /
    STM_ERR_END
};
*/

	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)texte,70,130);
	wait_vblank();

	}while(1);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void errGfsFunc(void *obj, Sint32 ec)
{
//	VTV_PRINTF((VTV_s, "S:ErrGfs %X %X\n", obj, ec));
	char texte[50];
	vout(texte, "ErrGfs %X %X",obj, ec); 
	texte[49]='\0';
	do{
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"planté gfs",70,130);

	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)texte,70,140);

    Sint32      ret;
    GfsErrStat  stat;
    GFS_GetErrStat(&stat);
    ret = GFS_ERR_CODE(&stat);

	vout(texte, "ErrGfsCode %X",ret); 
	texte[49]='\0';
#if 0
#define GFS_ERR_OK 0 / * Successful completion * /
#define GFS_ERR_CDRD (-1) / * CD read error * /
#define GFS_ERR_CDNODISC (-2) / * CD is not set * /
#define GFS_ERR_CDROM (-3) / * Disk is not CD-ROM * /
#define GFS_ERR_DIRTBL (-4) / * Invalid directory management table * /
#define GFS_ERR_OPENMAX (-5) / * Value of maximum open number is invalid * /
#define GFS_ERR_DIR (-6) / * The specified file is not a directory * /
#define GFS_ERR_CDBFS (-7) / * CD block file system * /

#define GFS_ERR_NONAME (-8) / * can not handle file name * /
#define GFS_ERR_NEXIST (-9) / * The specified file does not exist * /
#define GFS_ERR_FID (- 10) / * Invalid file identifier * /
#define GFS_ERR_HNDL (-11) / * File handle is invalid * /
#define GFS_ERR_SEEK (-12) / * Invalid seek position * /
#define GFS_ERR_ORG (-13) / * Invalid reference level value * /
#define GFS_ERR_NUM (-14) / * The number of bytes is negative * /
#define GFS_ERR_OFS (-15) / * Invalid offset * /
#define GFS_ERR_FBUSY (-16) / * Processing of specified file still remains * /
#define GFS_ERR_PARA (-17) / * Invalid parameter * /
#define GFS_ERR_BUSY (-18) / * Library function in progress * /
#define GFS_ERR_NOHNDL (-19) / * There is no space available on the file handle * /
#define GFS_ERR_PUINUSE (-20) / * Pickup operation in progress * /
#define GFS_ERR_ALIGN (-21) / * Working area is not on 4-byte boundary * /
#define GFS_ERR_TMOUT (-22) / * timeout * /
#define GFS_ERR_CDOPEN (-23) / * Tray is open * /
#define GFS_ERR_BFUL (-24) / * Stop reading buffer full * /
#define GFS_ERR_FATAL (-25) / * CD is FATAL state * /
#endif

	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)texte,70,150);

	wait_vblank();

	}while(1);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void errPcmFunc(void *obj, Sint32 ec)
{
//	VTV_PRINTF((VTV_s, "S:ErrPcm %X %X\n", obj, ec));
	char texte[50];
	vout(texte, "ErrPcm %X %X",obj, ec); 
	texte[49]='\0';
	do{
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"planté pcm",70,130);

	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)texte,70,140);
	wait_vblank();

	}while(1);
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
void SetStreamPCM()
{
	PcmInfo 		info;
	static PcmWork g_movie_work;
	PcmStatus	*st;

	PCM_PARA_WORK(&paraStream)			= (struct PcmWork *)&g_movie_work;

	PCM_PARA_RING_ADDR(&paraStream)	= (Sint8 *)PCM_ADDR+0x40000+PCM_BLOCK_SIZE;
	PCM_PARA_RING_SIZE(&paraStream)		= RING_BUF_SIZE;

	PCM_PARA_PCM_ADDR(&paraStream)	= PCM_ADDR+PCM_BLOCK_SIZE;
	PCM_PARA_PCM_SIZE(&paraStream)		= PCM_SIZE;

	memset((Sint8 *)SOUND_BUFFER,0,SOUNDRATE*16);
	st = &g_movie_work.status;
	st->need_ci = PCM_OFF;

	PCM_INFO_FILE_TYPE(&info) = PCM_FILE_TYPE_NO_HEADER;			
	PCM_INFO_DATA_TYPE(&info)=PCM_DATA_TYPE_RLRLRL;//PCM_DATA_TYPE_LRLRLR;
	PCM_INFO_CHANNEL(&info) = 0x01;
	PCM_INFO_SAMPLING_BIT(&info) = 16;

	PCM_INFO_SAMPLING_RATE(&info)	= SOUNDRATE;//30720L;//44100L;

	PCM_INFO_FILE_SIZE(&info) = PCM_000_SIZE;//sfx_list[0].size;//SOUNDRATE*2;//0x4000;//214896;

	STM_ResetTrBuf(stm);
	pcmStream = PCM_CreateStmHandle(&paraStream, stm);
	PCM_SetPcmStreamNo(pcmStream, 1);
	PCM_SetInfo(pcmStream, &info);
	PCM_ChangePcmPara(pcmStream);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
void PlayStreamPCM(unsigned char d, unsigned char current_pcm)
{
//	if(current_pcm!=d && (d==0 || (d >=0x20 && d <=0x3D)))
//	{
#ifdef DEBUG
		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"   ",80,140);	
		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"        ",80,150);	
		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)itoa(d),80,140);	
		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)itoa(sfx_list[d].size),80,150);	
#endif

		if(current_pcm!=0x3D)
		{
			PCM_MeStop(pcmStream);
			pcm_EndProcess(pcmStream);
			PCM_DestroyStmHandle(pcmStream);
			stmClose(stm);
		}

		if(d!=0x3D) // 0x3D stop
		{
			char pcm_file[14];

			vout2(pcm_file, "%03d%s",d,".PCM"); 
			PcmInfo info;

			PCM_INFO_FILE_TYPE(&info) = PCM_FILE_TYPE_NO_HEADER;			
			PCM_INFO_DATA_TYPE(&info)=PCM_DATA_TYPE_RLRLRL;//PCM_DATA_TYPE_LRLRLR;
			PCM_INFO_CHANNEL(&info) = 0x01;
			PCM_INFO_SAMPLING_BIT(&info) = 16;
			PCM_INFO_SAMPLING_RATE(&info)	= SOUNDRATE;//30720L;//44100L;
			PCM_INFO_FILE_SIZE(&info) =sfx_list[d].size*sfx_list[d].loop;//SOUNDRATE*2;//0x4000;//214896;
/*
			PcmWork		*work = *(PcmWork **)pcm14[0];
			PcmStatus	*st = &work->status;
			st->cnt_loop = sfx_list[d].loop;*/
//			PCM_SetLoop(pcmStream, sfx_list[d].loop);

			stm = stmOpen(pcm_file);
			STM_ResetTrBuf(stm);

			pcmStream = PCM_CreateStmHandle(&paraStream, stm);

			PCM_SetPcmStreamNo(pcmStream, 1);
			PCM_SetInfo(pcmStream, &info);

			PcmWork		*work = *(PcmWork **)pcmStream;
			PcmStatus	*st = &work->status;
			st->need_ci = PCM_OFF;
//			STM_SetLoop(grp_hd, STM_LOOP_DFL, STM_LOOP_ENDLESS);

			PCM_SetLoadNum(pcmStream, 10);
			PCM_SetTrModeCd(pcmStream, PCM_TRMODE_SDMA);
			PCM_Set1TaskSample(pcmStream, 3192);

			PCM_Start(pcmStream);
		}
//	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
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