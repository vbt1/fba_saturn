#include "burnint.h"
#include "burn_sound.h"
#include "burn_ym2151.h"
  
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
/*
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
*/
/*
void YM2151RenderNormalVBT(INT16* pSoundBuf, INT32 nSegmentLength)
{
//	pBuffer = (INT16*)0x25a20000;//0x00200000;	
//	INT16* pBuffer = (INT16*)0x00200000;	
//	pYM2151Buffer[0] = pBuffer;
//	pYM2151Buffer[1] = pBuffer + nSegmentLength;
	YM2151UpdateOne(0, pSoundBuf, nSegmentLength);
}
*/
