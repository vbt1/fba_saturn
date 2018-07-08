#include "saturn_snd.h"
//-------------------------------------------------------------------------------------------------------------------------------------
void stmInit(void)
{
	STM_Init(12, 24, stm_work);
//	STM_Init(4, 20, stm_work);
//	STM_Init(1, 2, stm_work);
	STM_SetErrFunc(errStmFunc, NULL);

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
}
//-------------------------------------------------------------------------------------------------------------------------------------
void stmClose(StmHn fp)
{
	STM_Close(fp);
}
//-------------------------------------------------------------------------------------------------------------------------------------
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
//	do{
	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"planté pcm",70,130);

	FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)texte,70,140);
	wait_vblank();

//	}while(1);
}
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
