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
#endif
	grp_hd = STM_OpenGrp();
	if (grp_hd == NULL) {
		return;
	}
	STM_SetLoop(grp_hd, STM_LOOP_DFL, STM_LOOP_ENDLESS);
	STM_SetExecGrp(grp_hd);
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
void errStmFunc(void *obj, Sint32 ec)
{
//	VTV_PRINTF((VTV_s, "S:ErrStm %X %X\n", obj, ec));
	char texte[50];
	vout(texte, "ErrStm %X %X",obj, ec); 
	texte[49]='\0';
	do{
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"plante stm                         ",40,130);

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
//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"   ",80,140);	
//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"        ",80,150);	
//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)itoa(d),80,140);	
//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)itoa(sfx_list[d].size),80,150);	

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
			PCM_INFO_FILE_SIZE(&info) =sfx_list[d].size;//SOUNDRATE*2;//0x4000;//214896;
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
/*		else
		{
			while(1);
		}
*/
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