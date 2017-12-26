#include "../burnint.h"
#include "../burn_sound.h"
#include "burn_ym2203.h"
#include "timer.h"
#define MAX_YM2203	2

//#define YM2203_Render 1
extern unsigned int  nSoundBufferPos;
INT32 nBurnFPS = 6000;
extern float dTime;

void (*BurnYM2203Update)(INT16* pSoundBuf, INT32 nSegmentEnd);

static INT32 (*BurnYM2203StreamCallback)(INT32 nSoundRate);

static INT32 nBurnYM2203SoundRate;

static INT16* pBuffer;
static INT16* pYM2203Buffer[4 * MAX_YM2203];

static INT32 nYM2203Position;
static INT32 nAY8910Position;

static UINT32 nSampleSize;
static INT32 nFractionalPosition;

static INT32 nNumChips = 0;

static INT32 bYM2203AddSignal;

INT32 bYM2203UseSeperateVolumes; // support custom Taito panning hardware

// ----------------------------------------------------------------------------
// Dummy functions

static void YM2203UpdateDummy(INT16* pSoundBuf, INT32 nSegmentEnd)
{
	return;
}

static INT32 YM2203StreamCallbackDummy(INT32 a)
{
	return 0;
}

// ----------------------------------------------------------------------------
// Execute YM2203 for part of a frame

static void AY8910Render(INT32 nSegmentLength)
{
#if defined FBA_DEBUG
	if (!DebugSnd_YM2203Initted) bprintf(PRINT_ERROR, _T("BurnYM2203 AY8910Render called without init\n"));
#endif

	if (nAY8910Position >= nSegmentLength) {
		return;
	}

	nSegmentLength -= nAY8910Position;

	unsigned short *nSoundBuffer1 = (unsigned short *)0x25a26000+nSoundBufferPos;

	AY8910UpdateDirect(0, &nSoundBuffer1[0x0000], nSegmentLength);
	if (nNumChips > 1) 
		AY8910UpdateDirect(1, &nSoundBuffer1[0x6000], nSegmentLength);
	nAY8910Position += nSegmentLength;
}

static void AY8910RenderOld(INT32 nSegmentLength)
{
#if defined FBA_DEBUG
	if (!DebugSnd_YM2203Initted) bprintf(PRINT_ERROR, _T("BurnYM2203 AY8910Render called without init\n"));
#endif

	if (nAY8910Position >= nSegmentLength) {
		return;
	}

	nSegmentLength -= nAY8910Position;

	pYM2203Buffer[1] = pBuffer + 1 * 4096 + 4 + nAY8910Position;
	pYM2203Buffer[2] = pBuffer + 2 * 4096 + 4 + nAY8910Position;
	pYM2203Buffer[3] = pBuffer + 3 * 4096 + 4 + nAY8910Position;

	AY8910Update(0, &pYM2203Buffer[1], nSegmentLength);
	
//	if (nNumChips > 1) 
	{
		pYM2203Buffer[5] = pBuffer + 5 * 4096 + 4 + nAY8910Position;
		pYM2203Buffer[6] = pBuffer + 6 * 4096 + 4 + nAY8910Position;
		pYM2203Buffer[7] = pBuffer + 7 * 4096 + 4 + nAY8910Position;

		AY8910Update(1, &pYM2203Buffer[5], nSegmentLength);
	}
/*	
	if (nNumChips > 2) {
		pYM2203Buffer[9] = pBuffer + 9 * 4096 + 4 + nAY8910Position;
		pYM2203Buffer[10] = pBuffer + 10 * 4096 + 4 + nAY8910Position;
		pYM2203Buffer[11] = pBuffer + 11 * 4096 + 4 + nAY8910Position;

		AY8910Update(2, &pYM2203Buffer[9], nSegmentLength);
	}
 */
	nAY8910Position += nSegmentLength;
}

#ifdef YM2203_Render
static void YM2203Render(INT32 nSegmentLength)
{
#if defined FBA_DEBUG
	if (!DebugSnd_YM2203Initted) bprintf(PRINT_ERROR, _T("YM2203Render called without init\n"));
#endif

	if (nYM2203Position >= nSegmentLength) {
		return;
	}

	nSegmentLength -= nYM2203Position;

	pYM2203Buffer[0] = pBuffer + 0 * 4096 + 4 + nYM2203Position;

	YM2203UpdateOne(0, pYM2203Buffer[0], nSegmentLength);
	
	if (nNumChips > 1) 
	{
		pYM2203Buffer[4] = pBuffer + 4 * 4096 + 4 + nYM2203Position;

		YM2203UpdateOne(1, pYM2203Buffer[4], nSegmentLength);
	}

	nYM2203Position += nSegmentLength;
}
#endif
// ----------------------------------------------------------------------------
// Update the sound buffer
/*static*/ void YM2203UpdateNormal(INT16* pSoundBuf, INT32 nSegmentEnd)
{
//FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)"YM2203UpdateNormal            ",4,10);
#if defined FBA_DEBUG
	if (!DebugSnd_YM2203Initted) bprintf(PRINT_ERROR, _T("YM2203UpdateNormal called without init\n"));
#endif

	INT32 nSegmentLength = nSegmentEnd;
//	INT32 i;

	if (nSegmentEnd < nAY8910Position) {
		nSegmentEnd = nAY8910Position;
	}
	if (nSegmentEnd < nYM2203Position) {
		nSegmentEnd = nYM2203Position;
	}

	if (nSegmentLength > nBurnSoundLen) {
		nSegmentLength = nBurnSoundLen;
	}

#ifdef YM2203_Render
	YM2203Render(nSegmentEnd);
#endif
	AY8910Render(nSegmentEnd);

#if 1
//	pYM2203Buffer[0] = pBuffer + 4 + 0 * 4096;
//	pYM2203Buffer[1] = pBuffer + 4 + 1 * 4096;
//	pYM2203Buffer[2] = pBuffer + 4 + 2 * 4096;
//	pYM2203Buffer[3] = pBuffer + 4 + 3 * 4096;
//	if (nNumChips > 1) 
//	{
//		pYM2203Buffer[4] = pBuffer + 4 + 4 * 4096;
//		pYM2203Buffer[5] = pBuffer + 4 + 5 * 4096;
//		pYM2203Buffer[6] = pBuffer + 4 + 6 * 4096;
//		pYM2203Buffer[7] = pBuffer + 4 + 7 * 4096;
//	}
#if 1
//	for (INT32 n = nFractionalPosition; n < nSegmentLength; n++) 
//	{
//		INT32 nLeftSample = 0, nRightSample = 0;

//		nLeftSample += (INT32)pYM2203Buffer[1][n];
//		nLeftSample += (INT32)pYM2203Buffer[2][n];
//		nLeftSample += (INT32)pYM2203Buffer[3][n];
//		nLeftSample += (INT32)pYM2203Buffer[0][n];
//		nLeftSample += (INT32)pYM2203Buffer[5][n];
//		nLeftSample += (INT32)pYM2203Buffer[6][n];
//		nLeftSample += (INT32)pYM2203Buffer[7][n];
//		nLeftSample += (INT32)pYM2203Buffer[4][n];
		
//		nLeftSample = BURN_SND_CLIP(nLeftSample);
			
//		if (bYM2203AddSignal) 
//		{
//			pSoundBuf[n] = BURN_SND_CLIP(pSoundBuf[n] + nLeftSample);
//		} 
//		else 
//		{
//			pSoundBuf[n] = nLeftSample;
//		}
//	}

#endif
//	nFractionalPosition = nSegmentLength;
#if 1
	if (nSegmentEnd >= nBurnSoundLen) {
		INT32 nExtraSamples = nSegmentEnd - nBurnSoundLen;

//		for (i = 0; i < nExtraSamples; i++) {
//			pYM2203Buffer[0][i] = pYM2203Buffer[0][nBurnSoundLen + i];
//			pYM2203Buffer[1][i] = pYM2203Buffer[1][nBurnSoundLen + i];
//			pYM2203Buffer[2][i] = pYM2203Buffer[2][nBurnSoundLen + i];
//			pYM2203Buffer[3][i] = pYM2203Buffer[3][nBurnSoundLen + i];
//			if (nNumChips > 1) {
//				pYM2203Buffer[4][i] = pYM2203Buffer[4][nBurnSoundLen + i];
//				pYM2203Buffer[5][i] = pYM2203Buffer[5][nBurnSoundLen + i];
//				pYM2203Buffer[6][i] = pYM2203Buffer[6][nBurnSoundLen + i];
//				pYM2203Buffer[7][i] = pYM2203Buffer[7][nBurnSoundLen + i];
//			}
//		}
//		nFractionalPosition = 0;

		nYM2203Position = nExtraSamples;
		nAY8910Position = nExtraSamples;

		dTime += 100.0 / nBurnFPS;
//		dTime += 0,016667;
	}
#endif


#endif
}

// ----------------------------------------------------------------------------
// Callbacks for YM2203 core

void BurnYM2203UpdateRequest()
{
#if defined FBA_DEBUG
	if (!DebugSnd_YM2203Initted) bprintf(PRINT_ERROR, _T("BurnYM2203UpdateRequest called without init\n"));
#endif

#ifdef YM2203_Render
	YM2203Render(BurnYM2203StreamCallback(nBurnYM2203SoundRate));
#else
	BurnYM2203StreamCallback(nBurnYM2203SoundRate);
#endif
}

static void BurnAY8910UpdateRequest()
{
#if defined FBA_DEBUG
	if (!DebugSnd_YM2203Initted) bprintf(PRINT_ERROR, _T("BurnYM2203 BurnAY8910UpdateRequest called without init\n"));
#endif

	AY8910Render(BurnYM2203StreamCallback(nBurnYM2203SoundRate));
}

// ----------------------------------------------------------------------------
// Initialisation, etc.

void BurnYM2203Reset()
{
#if defined FBA_DEBUG
	if (!DebugSnd_YM2203Initted) bprintf(PRINT_ERROR, _T("BurnYM2203Reset called without init\n"));
#endif

	BurnTimerReset();
	
	for (INT32 i = 0; i < nNumChips; i++) {
		YM2203ResetChip(i);
		AY8910Reset(i);
	}
}

void BurnYM2203Exit()
{
#if defined FBA_DEBUG
	if (!DebugSnd_YM2203Initted) bprintf(PRINT_ERROR, _T("BurnYM2203Exit called without init\n"));
#endif

	YM2203Shutdown();
	
	for (INT32 i = 0; i < nNumChips; i++) {
		AY8910Exit(i);
	}

	BurnTimerExit();

	if (pBuffer) {
//		free(pBuffer);
		pBuffer = NULL;
	}
	
	nNumChips = 0;
	bYM2203AddSignal = 0;
	bYM2203UseSeperateVolumes = 0;
	
//	DebugSnd_YM2203Initted = 0;
}

INT32 BurnYM2203Init(INT32 num, INT16 *addr, INT32 nClockFrequency, FM_IRQHANDLER IRQCallback, INT32 (*StreamCallback)(INT32), float (*GetTimeCallback)(), INT32 bAddSignal)
{
//	DebugSnd_YM2203Initted = 1;
//	nBurnFPS = ; //hz * 100.00;
	if (num > MAX_YM2203) num = MAX_YM2203;
	
	BurnTimerInit(&YM2203TimerOver, GetTimeCallback);
	if (nBurnSoundRate <= 0) {
		BurnYM2203StreamCallback = YM2203StreamCallbackDummy;

		BurnYM2203Update = YM2203UpdateDummy;

		for (INT32 i = 0; i < num; i++) {
			AY8910InitYM(i, nClockFrequency, 11025, NULL, NULL, NULL, NULL, BurnAY8910UpdateRequest);
		}
		YM2203Init(num, nClockFrequency, 11025, &BurnOPNTimerCallback, IRQCallback);
		return 0;
	}

	BurnYM2203StreamCallback = StreamCallback;
	{
		nBurnYM2203SoundRate = nBurnSoundRate;
		BurnYM2203Update = YM2203UpdateNormal;
	}

	for (INT32 i = 0; i < num; i++) {
		AY8910InitYM(i, nClockFrequency, nBurnYM2203SoundRate, NULL, NULL, NULL, NULL, BurnAY8910UpdateRequest);
	}
	
	YM2203Init(num, nClockFrequency, nBurnYM2203SoundRate, &BurnOPNTimerCallback, IRQCallback);

	pBuffer = (INT16 *)addr;

	memset(pBuffer, 0, 4096 * 4 * num * sizeof(INT16));

	nYM2203Position = 0;
	nAY8910Position = 0;
	nFractionalPosition = 0;
	
	nNumChips = num;
	bYM2203AddSignal = bAddSignal;

	return 0;
}
//-------------------------------------------------------------------------------------------------
/*static*/ void updateAY8910Sound()
{
	int nSample;
	int n;
	unsigned int deltaSlave;//soundLenSlave;//,titiSlave;
	signed short *nSoundBuffer = (signed short *)0x25a20000;
	deltaSlave    = nSoundBufferPos; //*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);
//	signed short *nSoundBuffer = (signed short *)(0x25a20000+nSoundBufferPos*(sizeof(signed short)));

//	soundLenSlave = SOUND_LEN);
	AY8910Update(0, &pYM2203Buffer[0], nBurnSoundLen);
	AY8910Update(1, &pYM2203Buffer[3], nBurnSoundLen);

	for (n = 0; n < nBurnSoundLen; n++) 
	{
		nSample  = pYM2203Buffer[0][n]; // >> 2;
		nSample += pYM2203Buffer[1][n]; // >> 2;
		nSample += pYM2203Buffer[2][n]; // >> 2;
		nSample += pYM2203Buffer[3][n]; // >> 2;
		nSample += pYM2203Buffer[4][n]; // >> 2;
		nSample += pYM2203Buffer[5][n]; // >> 2;

		nSample /=4;

		if (nSample < -32768) 
		{
			nSample = -32768;
		} 
		else 
		{
			if (nSample > 32767) 
			{
				nSample = 32767;
			}
		}
		nSoundBuffer[deltaSlave + n] = nSample;//pAY8910Buffer[5][n];//nSample;
	}

	if(deltaSlave>=RING_BUF_SIZE/2)
	{
		deltaSlave=0;
		PCM_Task(pcm); // bon emplacement
	}

	deltaSlave+=nBurnSoundLen;

//	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = deltaSlave;
	nSoundBufferPos = deltaSlave;
}

#undef MAX_YM2203
