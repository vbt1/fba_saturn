#include <math.h>
#include "../burnint.h"
#include "msm6295.h"

#define	true	1
#define	false	0
#define CLIP(A) ((A) < -0x8000 ? -0x8000 : (A) > 0x7fff ? 0x7fff : (A))
typedef char bool;

unsigned char* MSM6295ROM;
//unsigned char* MSM6295SampleInfo[MAX_MSM6295][4];
//unsigned char* MSM6295SampleData[MAX_MSM6295][4];

unsigned int nMSM6295Status[MAX_MSM6295];

typedef struct {
	int nOutput;
	int nVolume;
	int nPosition;
	int nSampleCount;
	int nSample;
	int nStep;
	int nDelta;

	int nBufPos;
	int nPlaying;
}MSM6295ChannelInfo;

static struct {
	int nVolume;
	int nSampleRate;
	int nSampleSize;
	int nFractionalPosition;

	// All current settings for each channel
	MSM6295ChannelInfo ChannelInfo[4];

	// Used for sending commands
	bool bIsCommand;
	int nSampleInfo;

} MSM6295[MAX_MSM6295];

static UINT8 *pBankPointer[MAX_MSM6295][0x40000/0x100];

#define MSM6295ReadData(chip, addr)	\
	pBankPointer[chip][((addr) >> 8) & 0x3ff][((addr) & 0xff)]

static unsigned int MSM6295VolumeTable[16];
static int MSM6295DeltaTable[49 * 16];
static int MSM6295StepShift[8] = {-1, -1, -1, -1, 2, 4, 6, 8};

static int* MSM6295ChannelData[MAX_MSM6295][4];

int* pBuffer = NULL;
static int nLastChip;

static bool bAdd;

void MSM6295SetBank(INT32 nChip, UINT8 *pRomData, INT32 nStart, INT32 nEnd)
{
	if (pRomData == NULL) return;

//	if (nEnd >= nStart) return;
//	if (nEnd >= 0x40000) nEnd = 0x40000;

	for (INT32 i = 0; i < ((nEnd - nStart) >> 8) + 1; i++)
	{
		pBankPointer[nChip][(nStart >> 8) + i] = pRomData + (i << 8);
	}
}

void MSM6295Reset(int nChip)
{
	nMSM6295Status[nChip] = 0;
	MSM6295[nChip].bIsCommand = false;

	MSM6295[nChip].nFractionalPosition = 0;
	unsigned int nChannel;
	for (nChannel = 0; nChannel < 4; nChannel++) {
		// Set initial bank information
/*
		MSM6295SampleInfo[nChip][nChannel] = MSM6295ROM + (nChip * 0x0100000) + (nChannel << 8);
		MSM6295SampleData[nChip][nChannel] = MSM6295ROM + (nChip * 0x0100000) + (nChannel << 16);
*/
		MSM6295[nChip].ChannelInfo[nChannel].nPlaying = 0;
		memset(MSM6295ChannelData[nChip][nChannel], 0, 0x1000 * sizeof(int));
		MSM6295[nChip].ChannelInfo[nChannel].nBufPos = 4;
	}

		// set bank data only if DataPointer has not already been set
		if (pBankPointer[nChip][0] == NULL) {
			MSM6295SetBank(nChip, MSM6295ROM + (nChip * 0x0100000), 0, 0x3ffff); // set initial bank (compatibility)
	}
}
//void MSM6295Render_Linear(int nChip, int* pBuf, int nSegmentLength)
void MSM6295Render_Linear(int nChip, short* pBuf, int nSegmentLength)
{
	static int nPreviousSample[MAX_MSM6295], nCurrentSample[MAX_MSM6295];
	int nVolume = MSM6295[nChip].nVolume;
	int nFractionalPosition = MSM6295[nChip].nFractionalPosition;

	int nChannel, nDelta, nSample;
	MSM6295ChannelInfo* pChannelInfo;

	while (nSegmentLength--)
	{
		if (nFractionalPosition >= 0x1000) {

			nPreviousSample[nChip] = nCurrentSample[nChip];

			do {
				nCurrentSample[nChip] = 0;

				for (nChannel = 0; nChannel < 4; nChannel++) {
					if (nMSM6295Status[nChip] & (1 << nChannel)) {
						pChannelInfo = &MSM6295[nChip].ChannelInfo[nChannel];

						// Check for end of sample
						if (pChannelInfo->nSampleCount-- == 0) {
							nMSM6295Status[nChip] &= ~(1 << nChannel);
							MSM6295[nChip].ChannelInfo[nChannel].nPlaying = 0;
							continue;
						}

						// Get new delta from ROM
						if (pChannelInfo->nPosition & 1) {
							nDelta = pChannelInfo->nDelta & 0x0F;
						} else {
/*
							pChannelInfo->nDelta = MSM6295SampleData[nChip][(pChannelInfo->nPosition >> 17) & 3][(pChannelInfo->nPosition >> 1) & 0xFFFF];
							nDelta = pChannelInfo->nDelta >> 4;
*/
							pChannelInfo->nDelta = MSM6295ReadData(nChip, (pChannelInfo->nPosition >> 1) & 0x3ffff);
							nDelta = pChannelInfo->nDelta >> 4;
						}

						// Compute new sample
						nSample = pChannelInfo->nSample + MSM6295DeltaTable[(pChannelInfo->nStep << 4) + nDelta];

						if (nSample > 2047) {
							nSample = 2047;
						} else {
							if (nSample < -2048) {
								nSample = -2048;
							}
						}
						pChannelInfo->nSample = nSample;
						pChannelInfo->nOutput = (nSample * pChannelInfo->nVolume);

						// Update step value
						pChannelInfo->nStep = pChannelInfo->nStep + MSM6295StepShift[nDelta & 7];
						if (pChannelInfo->nStep > 48) {
							pChannelInfo->nStep = 48;
						} else {
							if (pChannelInfo->nStep < 0) {
								pChannelInfo->nStep = 0;
							}
						}

						nCurrentSample[nChip] += pChannelInfo->nOutput / 16;

						// Advance sample position
						pChannelInfo->nPosition++;
					}
				}

				nFractionalPosition -= 0x1000;

			} while (nFractionalPosition >= 0x1000);
		}

		// Compute linearly interpolated sample
		nSample = nPreviousSample[nChip] + (((nCurrentSample[nChip] - nPreviousSample[nChip]) * nFractionalPosition) >> 12);



		// Scale all 4 channels
		nSample *= nVolume;
//		*pBuf++ += nSample;
		*pBuf++ += CLIP((nSample >> 8));
		nFractionalPosition += MSM6295[nChip].nSampleSize;
	}

	MSM6295[nChip].nFractionalPosition = nFractionalPosition;
}


/*
int MSM6295Render(int nChip, short* pSoundBuf, int nSegmentLength)
{
	if (nChip == 0) {
		memset(pBuffer, 0, nSegmentLength * sizeof(int));
	}

	if (0)//nInterpolation >= 3) 
	{
		MSM6295Render_Cubic(nChip, pBuffer, nSegmentLength);
	} else {
		MSM6295Render_Linear(nChip, pBuffer, nSegmentLength);
	}

	if (nChip == nLastChip)	{
		{
// VBt à remettre
			
			if (bAdd) {
				BurnSoundCopyClamp_Mono_Add_C(pBuffer, pSoundBuf, nSegmentLength);
			} else {
				BurnSoundCopyClamp_Mono_C(pBuffer, pSoundBuf, nSegmentLength);
			}		   
		}
	}

	return 0;
}
*/

int MSM6295RenderVBT(int nChip, short* pSoundBuf, int nSegmentLength)
{
//	if (nChip == 0) {
//		memset(pBuffer, 0, nSegmentLength * sizeof(int));
		memset(pSoundBuf, 0, nSegmentLength * sizeof(short));
//		memset(pBufferVBT, 0, nSegmentLength * sizeof(int));
//	}
//	MSM6295Render_Linear(nChip, pBuffer, nSegmentLength);
	MSM6295Render_Linear(nChip, pSoundBuf, nSegmentLength);
//	BurnSoundCopyClamp_Mono_C(pBuffer, pSoundBuf, nSegmentLength);
	return 0;
}

void MSM6295Command(int nChip, unsigned char nCommand)
{
	if (MSM6295[nChip].bIsCommand) {
		// Process second half of command
		int nChannel, nSampleStart, nSampleCount;
		int nVolume = nCommand & 0x0F;
		nCommand >>= 4;

		MSM6295[nChip].bIsCommand = false;

		for (nChannel = 0; nChannel < 4; nChannel++) 
		{
			if (nCommand & (0x01 << nChannel)) 
			{
				if (MSM6295[nChip].ChannelInfo[nChannel].nPlaying == 0) 
				{
					int nBank = (MSM6295[nChip].nSampleInfo & 0x0300) >> 8;
					MSM6295[nChip].nSampleInfo &= 0xFF;
	/*
					nSampleStart = MSM6295SampleInfo[nChip][nBank][MSM6295[nChip].nSampleInfo + 0];
					nSampleStart <<= 8;
					nSampleStart |= MSM6295SampleInfo[nChip][nBank][MSM6295[nChip].nSampleInfo + 1];
					nSampleStart <<= 8;
					nSampleStart |= MSM6295SampleInfo[nChip][nBank][MSM6295[nChip].nSampleInfo + 2];
					nSampleStart <<= 1;

					nSampleCount = MSM6295SampleInfo[nChip][nBank][MSM6295[nChip].nSampleInfo + 3];
					nSampleCount <<= 8;
					nSampleCount |= MSM6295SampleInfo[nChip][nBank][MSM6295[nChip].nSampleInfo + 4];
					nSampleCount <<= 8;
					nSampleCount |= MSM6295SampleInfo[nChip][nBank][MSM6295[nChip].nSampleInfo + 5];
					nSampleCount <<= 1;
	*/
					nSampleStart  = MSM6295ReadData(nChip, (MSM6295[nChip].nSampleInfo & 0x03ff) + 0) << 17;
					nSampleStart |= MSM6295ReadData(nChip, (MSM6295[nChip].nSampleInfo & 0x03ff) + 1) <<  9;
					nSampleStart |= MSM6295ReadData(nChip, (MSM6295[nChip].nSampleInfo & 0x03ff) + 2) <<  1;

					nSampleCount  = MSM6295ReadData(nChip, (MSM6295[nChip].nSampleInfo & 0x03ff) + 3) << 17;
					nSampleCount |= MSM6295ReadData(nChip, (MSM6295[nChip].nSampleInfo & 0x03ff) + 4) <<  9;
					nSampleCount |= MSM6295ReadData(nChip, (MSM6295[nChip].nSampleInfo & 0x03ff) + 5) <<  1;

					MSM6295[nChip].nSampleInfo &= 0xFF;

//					if (nSampleCount < 0x80000) {
					nSampleCount -= nSampleStart;

					if (nSampleCount < 0x80000) 
					{
						// Start playing channel
						MSM6295[nChip].ChannelInfo[nChannel].nVolume = MSM6295VolumeTable[nVolume];
						MSM6295[nChip].ChannelInfo[nChannel].nPosition = nSampleStart;
						MSM6295[nChip].ChannelInfo[nChannel].nSampleCount = nSampleCount;
						MSM6295[nChip].ChannelInfo[nChannel].nStep = 0;
						MSM6295[nChip].ChannelInfo[nChannel].nSample = -1;
						MSM6295[nChip].ChannelInfo[nChannel].nPlaying = 1;
						MSM6295[nChip].ChannelInfo[nChannel].nOutput = 0;

						nMSM6295Status[nChip] |= nCommand;
					}
				}
			}
		}
	} 
	else 
	{
		// Process command
		if (nCommand & 0x80) 
		{
			MSM6295[nChip].nSampleInfo = (nCommand & 0x7F) << 3;
			MSM6295[nChip].bIsCommand = true;
		} 
		else 
		{
			// Stop playing samples
			INT32 nChannel;
			nCommand >>= 3;
			nMSM6295Status[nChip] &= ~nCommand;

			for (nChannel = 0; nChannel < 4; nChannel++, nCommand>>=1) 
			{
				if (nCommand & 1) 
				{
					MSM6295[nChip].ChannelInfo[nChannel].nPlaying = 0;
				}
			}
		}
	}
}

void MSM6295Exit(int nChip)
{
	unsigned int nChannel;
	for (nChannel = 0; nChannel < 4; nChannel++) {
		free(MSM6295ChannelData[nChip][3-nChannel]);
//		free(MSM6295ChannelData[nChip][nChannel]);
//		MSM6295ChannelData[nChip][3-nChannel] = NULL;
		MSM6295ChannelData[nChip][3-nChannel] = NULL;
	}
	if(pBuffer!=NULL)
//		free(pBuffer);
	pBuffer = NULL;
}

int ipow(int base, int exp)
{
    int result = 1;
    for (;;)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        if (!exp)
            break;
        base *= base;
    }

    return result;
}

float powerf(float x, int y) 
{ 
    float temp; 
    if( y == 0) 
       return 1; 
    temp = powerf(x, y/2);        
    if (y%2 == 0) 
        return temp*temp; 
    else
    { 
        if(y > 0) 
            return x*temp*temp; 
        else
            return (temp*temp)/x; 
    } 
}  
int MSM6295Init(int nChip, int nSamplerate, double fMaxVolume, bool bAddSignal, int *MSM6295Context)
{
	bAdd = bAddSignal;

	// Convert volume from percentage
	MSM6295[nChip].nVolume = (int)((fMaxVolume * 256) / 100 + 0.5);

	MSM6295[nChip].nSampleRate = nSamplerate;
	if (nBurnSoundRate > 0) {
		MSM6295[nChip].nSampleSize = (nSamplerate << 12) / nBurnSoundRate;
	} else {
		MSM6295[nChip].nSampleSize = (nSamplerate << 12) / 11025;
	}
	MSM6295[nChip].nFractionalPosition = 0;

	nMSM6295Status[nChip] = 0;
	MSM6295[nChip].bIsCommand = false;

	if (nChip == 0) {
		nLastChip = 0;
	} else {
		if (nLastChip < nChip) {
			nLastChip = nChip;
		}
	}
	unsigned int i,n;
	// Compute sample deltas
// vbt correct
	for (i = 0; i < 49; i++) {
//		int nStep = (int)(pow(1.1, (double)i) * 16.0);
		int nStep = (int)(powerf(1.1, i) * 16.0);
		for (n = 0; n < 16; n++) {
			int nDelta = nStep >> 3;
			if (n & 1) {
				nDelta += nStep >> 2;
			}
			if (n & 2) {
				nDelta += nStep >> 1;
			}
			if (n & 4) {
				nDelta += nStep;
			}
			if (n & 8) {
				nDelta = -nDelta;
			}
			MSM6295DeltaTable[(i << 4) + n] = nDelta;
		}

	}

	// Compute volume levels
// vbt correct
	for (i = 0; i < 16; i++) {
		double nVolume = 256.0;
		for (n = i; n > 0; n--) {
			nVolume /= 1.412537545;
		}
		MSM6295VolumeTable[i] = (unsigned int)(nVolume + 0.5);
	}
	unsigned int nChannel;
	for (nChannel = 0; nChannel < 4; nChannel++) {
		MSM6295ChannelData[nChip][nChannel] = (int*)(MSM6295Context+(nChannel * 0x1000 * sizeof(int)));
//		(int*)BurnMalloc(0x1000 * sizeof(int));
	}

	memset (pBankPointer[nChip], 0, (0x40000/0x100) * sizeof(UINT8*));
	MSM6295Reset(nChip);

	return 0;
}
