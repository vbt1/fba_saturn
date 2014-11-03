#include "burnint.h"
#include "burn_sound.h"
#include "burn_ym2151.h"
extern unsigned int  nSoundBufferPos;

void (*BurnYM2151Render)(INT16* pSoundBuf, INT32 nSegmentLength);

UINT8 BurnYM2151Registers[0x0100];
UINT32 nBurnCurrentYM2151Register;

static INT32 nBurnYM2151SoundRate;

//static INT16* pBuffer;
static INT16* pYM2151Buffer[2];

static INT32 nBurnPosition;
static UINT32 nSampleSize;
static UINT32 nFractionalPosition;
static UINT32 nSamplesRendered;

static double YM2151Volumes[2];
static INT32 YM2151RouteDirs[2];
  
#include <stdarg.h>
//char buffer[80];
//int vspfunc(char *format, ...);
  /*
int vspfunc(char *format, ...)
{
   va_list aptr;
   int ret;

   va_start(aptr, format);
   ret = vsprintf(buffer, format, aptr);
   va_end(aptr);

   return(ret);
}
	  */

void YM2151RenderNormal(INT16* pSoundBuf, INT32 nSegmentLength)
{
#if defined FBA_DEBUG
	if (!DebugSnd_YM2151Initted) bprintf(PRINT_ERROR, _T("YM2151RenderNormal called without init\n"));
#endif
	INT16* pBuffer = (INT16*)0x25a20000;//0x00200000;	

	nBurnPosition += nSegmentLength;

	pYM2151Buffer[0] = pBuffer;
	pYM2151Buffer[1] = pBuffer + nSegmentLength;

	YM2151UpdateOne(0, pSoundBuf, nSegmentLength);
	
	for (INT32 n = 0; n < nSegmentLength; n++) {
		INT32 nLeftSample = 0, nRightSample = 0;
		
		if ((YM2151RouteDirs[BURN_SND_YM2151_YM2151_ROUTE_1] & BURN_SND_ROUTE_LEFT) == BURN_SND_ROUTE_LEFT) {
			nLeftSample += (INT32)(pYM2151Buffer[0][n] * YM2151Volumes[BURN_SND_YM2151_YM2151_ROUTE_1]);
		}
		if ((YM2151RouteDirs[BURN_SND_YM2151_YM2151_ROUTE_1] & BURN_SND_ROUTE_RIGHT) == BURN_SND_ROUTE_RIGHT) {
			nRightSample += (INT32)(pYM2151Buffer[0][n] * YM2151Volumes[BURN_SND_YM2151_YM2151_ROUTE_1]);
		}
		
		if ((YM2151RouteDirs[BURN_SND_YM2151_YM2151_ROUTE_2] & BURN_SND_ROUTE_LEFT) == BURN_SND_ROUTE_LEFT) {
			nLeftSample += (INT32)(pYM2151Buffer[1][n] * YM2151Volumes[BURN_SND_YM2151_YM2151_ROUTE_2]);
		}
		if ((YM2151RouteDirs[BURN_SND_YM2151_YM2151_ROUTE_2] & BURN_SND_ROUTE_RIGHT) == BURN_SND_ROUTE_RIGHT) {
			nRightSample += (INT32)(pYM2151Buffer[1][n] * YM2151Volumes[BURN_SND_YM2151_YM2151_ROUTE_2]);
		}
		
		nLeftSample = BURN_SND_CLIP(nLeftSample);
		nRightSample = BURN_SND_CLIP(nRightSample);
			
		pSoundBuf[(n << 1) + 0] = nLeftSample;
		pSoundBuf[(n << 1) + 1] = nRightSample;
	}
}


void YM2151RenderNormalVBT(INT16* pSoundBuf, INT32 nSegmentLength)
{
//	pBuffer = (INT16*)0x25a20000;//0x00200000;	
//	INT16* pBuffer = (INT16*)0x00200000;	
//	pYM2151Buffer[0] = pBuffer;
//	pYM2151Buffer[1] = pBuffer + nSegmentLength;
	YM2151UpdateOne(0, pSoundBuf, nSegmentLength);
}

void YM2151RenderMono(short* pSoundBuf, int nSegmentLength)
{
	YM2151UpdateOne(0, pSoundBuf, nSegmentLength);
}

void BurnYM2151Reset()
{
#if defined FBA_DEBUG
	if (!DebugSnd_YM2151Initted) bprintf(PRINT_ERROR, _T("BurnYM2151Reset called without init\n"));
#endif

	memset(&BurnYM2151Registers, 0, sizeof(BurnYM2151Registers));
	YM2151ResetChip(0);
}

void BurnYM2151Exit()
{
#if defined FBA_DEBUG
	if (!DebugSnd_YM2151Initted) bprintf(PRINT_ERROR, _T("BurnYM2151Exit called without init\n"));
#endif

	BurnYM2151SetIrqHandler(NULL);
	BurnYM2151SetPortHandler(NULL);

	YM2151Shutdown();

//	if (pBuffer) {
//		free(pBuffer);
//		pBuffer = NULL;
//	}
	
//	DebugSnd_YM2151Initted = 0;
}

INT32 BurnYM2151Init(INT32 nClockFrequency)
{
//	DebugSnd_YM2151Initted = 1;
		/*
	if (nBurnSoundRate <= 0) {
		YM2151Init(1, nClockFrequency, 11025);
		return 0;
	} */
/*
	if (nFMInterpolation == 3) {
		// Set YM2151 core samplerate to match the hardware
		nBurnYM2151SoundRate = nClockFrequency >> 6;
		// Bring YM2151 core samplerate within usable range
		while (nBurnYM2151SoundRate > nBurnSoundRate * 3) {
			nBurnYM2151SoundRate >>= 1;
		}

		BurnYM2151Render = YM2151RenderResample;
	} else
*/	
	{
//		nBurnYM2151SoundRate = 11025;//nBurnSoundRate;
		nBurnYM2151SoundRate = nBurnSoundRate;
//		BurnYM2151Render = YM2151RenderNormal;
	}

	YM2151Init(1, nClockFrequency, nBurnYM2151SoundRate);

//	pBuffer = (INT16*)malloc(65536 * 2 * sizeof(INT16));
	INT16 pBuffer = (INT16*)0x00200000;
//	memset(pBuffer, 1, 65536 * 2 * sizeof(INT16));

	memset(pBuffer, 1, 0x1000000);

	nSampleSize = (UINT32)nBurnYM2151SoundRate * (1 << 16) / nBurnSoundRate;
	nFractionalPosition = 4 << 16;
	nSamplesRendered = 0;
	nBurnPosition = 0;
	memset(&BurnYM2151Registers, 0, sizeof(BurnYM2151Registers));
	
	// default routes
	YM2151Volumes[BURN_SND_YM2151_YM2151_ROUTE_1] = 1.00;
	YM2151Volumes[BURN_SND_YM2151_YM2151_ROUTE_2] = 1.00;
	YM2151RouteDirs[BURN_SND_YM2151_YM2151_ROUTE_1] = BURN_SND_ROUTE_BOTH;
	YM2151RouteDirs[BURN_SND_YM2151_YM2151_ROUTE_2] = BURN_SND_ROUTE_BOTH;

	return 0;
}

void BurnYM2151SetRoute(INT32 nIndex, double nVolume, INT32 nRouteDir)
{
#if defined FBA_DEBUG
//	if (!DebugSnd_YM2151Initted) bprintf(PRINT_ERROR, _T("BurnYM2151SetRoute called without init\n"));
	if (nIndex < 0 || nIndex > 1) bprintf(PRINT_ERROR, _T("BurnYM2151SetRoute called with invalid index %i\n"), nIndex);
#endif
	
	YM2151Volumes[nIndex] = nVolume;
	YM2151RouteDirs[nIndex] = nRouteDir;
}

