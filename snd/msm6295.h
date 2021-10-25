// MSM6295 module header

#define MAX_MSM6295 (2)

void MSM6295Init(int nChip, int nSamplerate, char bAddSignal, int *MSM6295Context);
void MSM6295Reset(int nChip);
void MSM6295Exit(int nChip);
void MSM6295RenderVBT(int nChip, short* pSoundBuf, int nSegmentLength);
int MSM6295Render(int nChip, short* pSoundBuf, int nSegmenLength);
void MSM6295Command(int nChip, unsigned char nCommand);
void MSM6295SetBank(INT32 nChip, UINT8 *pRomData, INT32 nStart, INT32 nEnd);
//int MSM6295Scan(int nChip, int nAction);
void BurnSoundCopyClamp_Mono_C(int *Src, short *Dest, int Len);
//extern unsigned char* MSM6295ROM;
extern unsigned char* MSM6295SampleInfo[MAX_MSM6295][4];
extern unsigned char* MSM6295SampleData[MAX_MSM6295][4];

inline unsigned int MSM6295ReadStatus(const int nChip)
{
	extern unsigned int nMSM6295Status[MAX_MSM6295];

	return nMSM6295Status[nChip];
}

