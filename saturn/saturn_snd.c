//#define DEBUG 1
#include "saturn_snd.h"
PcmHn 	pcmStream = NULL;
PcmCreatePara	paraStream = {.ring_size = 0, .pcm_size = 0, .ring_addr = NULL, .pcm_addr = NULL};
unsigned char stm_work[STM_WORK_SIZE(12, 24)] = {NULL};
StmHn stm = NULL;
StmGrpHn grp_hd = NULL;
SFX *sfx_list = NULL;

void vout(char *string, char *fmt, ...)                                         
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
	return STM_OpenFid(grp_hd, fid, &key, STM_LOOP_READ); 
}
//-------------------------------------------------------------------------------------------------------------------------------------
void stmClose(StmHn fp)
{
	STM_Close(fp);
}
//-------------------------------------------------------------------------------------------------------------------------------------
#ifdef DEBUG

void errStmFunc(void *obj, Sint32 ec)
{
	char texte[50];
	vout(texte, "ErrStm %X %X",obj, ec); 
	texte[49]='\0';
	do{
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"plante stm                         ",40,120);
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)texte,70,130);
	wait_vblank();

	}while(1);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void errGfsFunc(void *obj, Sint32 ec)
{
	char texte[50];
	vout(texte, "ErrGfs %X %X",obj, ec); 
	texte[49]='\0';
	do{
//	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"planté gfs",70,130);
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)texte,70,140);

    Sint32      ret;
    GfsErrStat  stat;
    GFS_GetErrStat(&stat);
    ret = GFS_ERR_CODE(&stat);

	vout(texte, "ErrGfsCode %X",ret); 
	texte[49]='\0';

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
/*
ec = 0x305
voir sega_stm.h à traduire
*/
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
	PCM_INFO_DATA_TYPE(&info)=PCM_DATA_TYPE_RLRLRL;
	PCM_INFO_CHANNEL(&info) = 0x01;
	PCM_INFO_SAMPLING_BIT(&info) = 16;

	PCM_INFO_SAMPLING_RATE(&info)	= SOUNDRATE;

	PCM_INFO_FILE_SIZE(&info) = PCM_000_SIZE;

	STM_ResetTrBuf(stm);
	pcmStream = PCM_CreateStmHandle(&paraStream, stm);
	PCM_SetPcmStreamNo(pcmStream, 0);
	PCM_SetInfo(pcmStream, &info);
	PCM_ChangePcmPara(pcmStream);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
void UpdateStreamPCM(Uint8 data, PcmHn *hn, PcmCreatePara *para)
{
	char pcm_file[14];

	vout(pcm_file, "%03d%s",data,".PCM"); 
	PcmInfo 		info;

	PCM_INFO_FILE_TYPE(&info) = PCM_FILE_TYPE_NO_HEADER;			
	PCM_INFO_DATA_TYPE(&info)=PCM_DATA_TYPE_RLRLRL;
	PCM_INFO_CHANNEL(&info) = 0x01;
	PCM_INFO_SAMPLING_BIT(&info) = 16;
	PCM_INFO_SAMPLING_RATE(&info)	= SOUNDRATE;
	PCM_INFO_FILE_SIZE(&info) = sfx_list[data].size*10;

	stm = stmOpen(pcm_file);

	STM_ResetTrBuf(stm);

	hn[0] = PCM_CreateStmHandle(&para[0], stm);

	PCM_SetPcmStreamNo(hn[0], 0);
	PCM_SetInfo(hn[0], &info);

	PCM_Start(hn[0]);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void PlayStreamPCM(unsigned char d, unsigned char current_pcm)
{
#ifdef DEBUG
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"   ",80,140);	
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"        ",80,150);	
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)itoa(d),80,140);	
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)itoa(sfx_list[d].size),80,150);	
#endif

	if(current_pcm!=0x3D)
	{
		PCM_MeStop(pcmStream);
		PCM_DestroyStmHandle(pcmStream);
		stmClose(stm);
	}

	if(d!=0x3D) // 0x3D stop
	{
		UpdateStreamPCM(d,&pcmStream, &paraStream);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void playMusic(PcmHn *hn)
{
#ifdef DEBUG
	PcmWork		*work = *(PcmWork **)hn;
	PcmStatus	*st= &work->status;

	if(st->play ==PCM_STAT_PLAY_ERR_STOP)
			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"errstp",40,40);
	else if (st->play ==PCM_STAT_PLAY_CREATE)
			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"create",40,40);
	else if (st->play ==PCM_STAT_PLAY_PAUSE)
			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"pause ",40,40);
	else if (st->play ==PCM_STAT_PLAY_START)
			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"start ",40,40);
	else if (st->play ==PCM_STAT_PLAY_HEADER)
			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"header",40,40);
	else if (st->play ==PCM_STAT_PLAY_TIME)
			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"playin",40,40);
	else if (st->play ==PCM_STAT_PLAY_END)
			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"end   ",40,40);
	else
			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"error ",40,40);
#endif
    STM_ExecServer();

	PCM_MeTask(hn[0]);

	if (STM_IsTrBufFull(hn[0]) == TRUE) 
	{
		STM_ResetTrBuf(hn[0]);
	}
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