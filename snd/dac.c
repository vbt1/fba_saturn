#include "../burnint.h"
#include "../burn_sound.h"

#define DAC_NUM		(1)	// Maximum DAC chips
#define nBurnSoundLen 192
typedef struct //dac_info
{
	INT16	Output;
//	double 	nVolume;
	INT32 	nCurrentPosition;
	INT32	Initialized;
	INT32	OutputDir;
	INT32	(*pSyncCallback)();
}dac_info;

//struct dac_info dac_table[DAC_NUM];
/*static*/ dac_info dac_table[DAC_NUM];

/*static*/ INT16 UnsignedVolTable[256];
/*static*/ INT16 SignedVolTable[256];

/*static*/ //INT16 *lBuffer = NULL;
//static INT16 *rBuffer = NULL;

/*static*/ INT32 NumChips;

/*static*/ INT32 bAddSignal;
//extern INT16 *lBuffer;
INT16 *lBuffer = NULL;

/*static*/ void UpdateStream(INT32 chip, INT32 length)
{
	dac_info *ptr;
/* char toto[100];
char *titi = &toto[0];
titi=itoa(length);
 			FNT_Print256_2bpp((volatile unsigned char *)0x25e20000,(unsigned char *)"        ",4,60);

			FNT_Print256_2bpp((volatile unsigned char *)0x25e20000,(unsigned char *)titi,4,60);
*/

/*
	if (lBuffer == NULL) {	// delay buffer allocation for cases when fps is not 60

		lBuffer =  (INT16*)dacBuffer;
		memset (lBuffer, 0, nBurnSoundLen * sizeof(INT16));
	}*/
/*	if (rBuffer == NULL) {	// delay buffer allocation for cases when fps is not 60
		if((rBuffer = (INT16*)BurnMalloc(nBurnSoundLen * sizeof(INT16)))==NULL)
		{
			while(1);
		}
		memset (rBuffer, 0, nBurnSoundLen * sizeof(INT16));
	}
*/
        ptr = &dac_table[chip];
        if (ptr->Initialized == 0) return;

        if (length > nBurnSoundLen) length = nBurnSoundLen;
        length -= ptr->nCurrentPosition;
        if (length <= 0) return;

        INT16 *lbuf = lBuffer + ptr->nCurrentPosition;
//	INT16 *rbuf = rBuffer + ptr->nCurrentPosition;

//	INT16 lOut = ((ptr->OutputDir & BURN_SND_ROUTE_LEFT ) == BURN_SND_ROUTE_LEFT ) ? ptr->Output : 0;
//        INT16 rOut = ((ptr->OutputDir & BURN_SND_ROUTE_RIGHT) == BURN_SND_ROUTE_RIGHT) ? ptr->Output : 0;
	INT16 lOut = ptr->Output;
//        INT16 rOut = ptr->Output;

        ptr->nCurrentPosition += length;

//        if (rOut && lOut) {
        if (lOut) {
//		FNT_Print256_2bpp((volatile unsigned char *)0x25e20000,(unsigned char *)"a",4,10);
			
                while (length--) {
//                      *lbuf++ = *lbuf + lOut;
//			*rbuf++ = *rbuf + rOut;
                        *lbuf++ = BURN_SND_CLIP(*lbuf + lOut);
	//		*rbuf++ = BURN_SND_CLIP(*rbuf + rOut);
                }
//		FNT_Print256_2bpp((volatile unsigned char *)0x25e20000,(unsigned char *)"b",4,10);

 //       } else if (lOut) {
//		FNT_Print256_2bpp((volatile unsigned char *)0x25e20000,(unsigned char *)"c",4,10);
//			
//                while (length--) *lbuf++ = BURN_SND_CLIP(*lbuf + lOut);
//		FNT_Print256_2bpp((volatile unsigned char *)0x25e20000,(unsigned char *)"d",4,10);
//        } else if (rOut) {            
//		FNT_Print256_2bpp((volatile unsigned char *)0x25e20000,(unsigned char *)"e",4,10);
//                while (length--) *rbuf++ = BURN_SND_CLIP(*rbuf + rOut);
//		FNT_Print256_2bpp((volatile unsigned char *)0x25e20000,(unsigned char *)"f",4,10);
        }
}

void DACUpdate(INT16* Buffer, INT32 Length)
{
#if defined FBA_DEBUG
//	if (!DebugSnd_DACInitted) bprintf(PRINT_ERROR, _T("DACUpdate called without init\n"));
#endif

	dac_info *ptr;

	for (INT32 i = 0; i < NumChips; i++) 
	{
		UpdateStream(i, nBurnSoundLen);
	}

//	UpdateStream(0, nBurnSoundLen);
	INT16 *lbuf = lBuffer;
//	INT16 *rbuf = rBuffer;

	if (bAddSignal) {
		while (Length--) {
//			Buffer[0] = BURN_SND_CLIP((rbuf[0] + lbuf[0])/2 + Buffer[0]);
			Buffer[0] = BURN_SND_CLIP(lbuf[0] + Buffer[0]);
			Buffer++;
			lbuf[0] = 0; // clear buffer
//			rbuf[0] = 0; // clear buffer
			lbuf++;
//			rbuf++;
		}
	} else {
		while (Length--) {
//			Buffer[0] = BURN_SND_CLIP((rbuf[0] + lbuf[0])/2);
			Buffer[0] = lbuf[0];
			Buffer++;
			lbuf[0] = 0; // clear buffer
//			rbuf[0] = 0; // clear buffer
			lbuf++;
//			rbuf++;
		}
	}

	for (INT32 i = 0; i < NumChips; i++) {
		ptr = &dac_table[i];
		ptr->nCurrentPosition = 0;
	}
}

void DACWrite(INT32 Chip, UINT8 Data)
{
#if defined FBA_DEBUG
// 	if (!DebugSnd_DACInitted) bprintf(PRINT_ERROR, _T("DACWrite called without init\n"));
	if (Chip > NumChips) bprintf(PRINT_ERROR, _T("DACWrite called with invalid chip number %x\n"), Chip);
#endif

	dac_info *ptr;

	ptr = &dac_table[Chip];

	UpdateStream(Chip, ptr->pSyncCallback());

//	ptr->Output = (INT32)(UnsignedVolTable[Data] * ptr->nVolume);
	ptr->Output = (INT32)(UnsignedVolTable[Data]);
}

void DACSignedWrite(INT32 Chip, UINT8 Data)
{
#if defined FBA_DEBUG
//	if (!DebugSnd_DACInitted) bprintf(PRINT_ERROR, _T("DACSignedWrite called without init\n"));
	if (Chip > NumChips) bprintf(PRINT_ERROR, _T("DACSignedWrite called with invalid chip number %x\n"), Chip);
#endif

	dac_info *ptr;

	ptr = &dac_table[Chip];

	UpdateStream(Chip, ptr->pSyncCallback());

//	ptr->Output = (INT32)(SignedVolTable[Data] * ptr->nVolume);
	ptr->Output = (INT32)(SignedVolTable[Data]);
}

static void DACBuildVolTables()
{
	for (INT32 i = 0;i < 256;i++) {
		UnsignedVolTable[i] = i * 0x101 / 2;
		SignedVolTable[i] = i * 0x101 - 0x8000;
	}
}

void DACInit(INT32 Num, UINT32 Clock, INT32 bAdd, INT32 (*pSyncCB)())
{
#if defined FBA_DEBUG
	if (Num >= DAC_NUM) bprintf (PRINT_ERROR, _T("DACInit called for too many chips (%d)! Change DAC_NUM (%d)!\n"), Num, DAC_NUM);
	if (pSyncCB == NULL) bprintf (PRINT_ERROR, _T("DACInit called with NULL callback!\n"));
#endif

	dac_info *ptr;

//	DebugSnd_DACInitted = 1;
	
	NumChips = Num + 1;

	ptr = &dac_table[Num];

	memset (ptr, 0, sizeof(dac_info));

	ptr->Initialized = 1;
//	ptr->nVolume = 1.00;
	ptr->OutputDir = BURN_SND_ROUTE_BOTH;
	ptr->pSyncCallback = pSyncCB;

	DACBuildVolTables(); // necessary to build for every chip?
	
	bAddSignal = bAdd;
}

void DACSetRoute(INT32 Chip, double nVolume, INT32 nRouteDir)
{
#if defined FBA_DEBUG
//	if (!DebugSnd_DACInitted) bprintf(PRINT_ERROR, _T("DACSetRoute called without init\n"));
	if (Chip > NumChips) bprintf(PRINT_ERROR, _T("DACSetRoute called with invalid chip %i\n"), Chip);
#endif

	dac_info *ptr;

	ptr = &dac_table[Chip];
//	ptr->nVolume = nVolume;
	ptr->OutputDir = nRouteDir;
}

void DACReset()
{
#if defined FBA_DEBUG
//	if (!DebugSnd_DACInitted) bprintf(PRINT_ERROR, _T("DACReset called without init\n"));
#endif

	dac_info *ptr;

	for (INT32 i = 0; i < NumChips; i++) {
		ptr = &dac_table[i];

		ptr->nCurrentPosition = 0;
		ptr->Output = 0;
	}
}

void DACExit()
{
#if defined FBA_DEBUG
//	if (!DebugSnd_DACInitted) bprintf(PRINT_ERROR, _T("DACExit called without init\n"));
#endif

	dac_info *ptr;

	for (INT32 i = 0; i < DAC_NUM; i++) {
		ptr = &dac_table[i];

		ptr->Initialized = 0;
		ptr->pSyncCallback = NULL;
	}

	NumChips = 0;
	
//	DebugSnd_DACInitted = 0;

//	free (lBuffer);
//	lBuffer = NULL;
//	free (rBuffer);
}
   /*
INT32 DACScan(INT32 nAction,INT32 *pnMin)
{
#if defined FBA_DEBUG
//	if (!DebugSnd_DACInitted) bprintf(PRINT_ERROR, _T("DACScan called without init\n"));
#endif
	
	if (pnMin != NULL) {
		*pnMin = 0x029719;
	}
	
	dac_info *ptr;

	if (nAction & ACB_DRIVER_DATA) {
		for (INT32 i = 0; i < NumChips; i++) {
			ptr = &dac_table[i];

			SCAN_VAR(ptr->Output);
		}
	}

	return 0;
}
	 */
