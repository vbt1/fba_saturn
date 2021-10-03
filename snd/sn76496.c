#include "../burnint.h"
#include "../burn_sound.h"
#include "sn76496.h"

#define MAX_SN76496_CHIPS 5

#define MAX_OUTPUT 0x4FFF//0x7fff

#define STEP 0x10000

struct SN76496
{
	unsigned int UpdateStep;
	INT32 VolTable[16];	/* volume table         */
	INT32 Register[8];	/* registers */
	INT32 LastRegister;	/* last register written */
	INT32 Volume[4];	/* volume of voice 0-2 and noise */
	INT32 RNG;		/* noise generator      */
	INT32 NoiseMode;	/* active noise mode */
	INT32 FeedbackMask;     /* mask for feedback */
	INT32 WhitenoiseTaps;   /* mask for white noise taps */
	INT32 WhitenoiseInvert; /* white noise invert flag */
	INT32 Period[4];
	INT32 Count[4];
	INT32 Output[4];
	int bSignalAdd;
};

struct SN76496 Chip0 = { .LastRegister = 0, .RNG = 0, .NoiseMode = 0, .FeedbackMask = 0, .bSignalAdd = 0, .WhitenoiseInvert = 0, .WhitenoiseTaps = 0 };
struct SN76496 Chip1 = { .LastRegister = 0, .RNG = 0, .NoiseMode = 0, .FeedbackMask = 0, .bSignalAdd = 0, .WhitenoiseInvert = 0, .WhitenoiseTaps = 0 };
struct SN76496 Chip2 = { .LastRegister = 0, .RNG = 0, .NoiseMode = 0, .FeedbackMask = 0, .bSignalAdd = 0, .WhitenoiseInvert = 0, .WhitenoiseTaps = 0 };
struct SN76496 Chip3 = { .LastRegister = 0, .RNG = 0, .NoiseMode = 0, .FeedbackMask = 0, .bSignalAdd = 0, .WhitenoiseInvert = 0, .WhitenoiseTaps = 0 };
struct SN76496 Chip4 = { .LastRegister = 0, .RNG = 0, .NoiseMode = 0, .FeedbackMask = 0, .bSignalAdd = 0, .WhitenoiseInvert = 0, .WhitenoiseTaps = 0 };

void SN76496Update(int Num, short* pSoundBuf, int Length)
{
	int i;
//	int Temp;

	struct SN76496 *R = &Chip0;
	
	if (Num >= MAX_SN76496_CHIPS) return;
	
	if (Num == 1) R = &Chip1;
	if (Num == 2) R = &Chip2;
	if (Num == 3) R = &Chip3;
	if (Num == 4) R = &Chip4;
	
	/* If the volume is 0, increase the counter */
	for (i = 0;i < 4;i++)
	{
		if (R->Volume[i] == 0)
		{
			/* note that I do count += length, NOT count = length + 1. You might think */
			/* it's the same since the volume is 0, but doing the latter could cause */
			/* interferencies when the program is rapidly modulating the volume. */
			if (R->Count[i] <= Length*STEP) R->Count[i] += Length*STEP;
		}
	}

	while (Length > 0)
	{
		int Vol[4];
//		unsigned int Out;
		int Left;

		/* vol[] keeps track of how long each square wave stays */
		/* in the 1 position during the sample period. */
		Vol[0] = Vol[1] = Vol[2] = Vol[3] = 0;

		for (i = 0;i < 3;i++)
		{
			if (R->Output[i]) Vol[i] += R->Count[i];
			R->Count[i] -= STEP;
			/* Period[i] is the half period of the square wave. Here, in each */
			/* loop I add Period[i] twice, so that at the end of the loop the */
			/* square wave is in the same status (0 or 1) it was at the start. */
			/* vol[i] is also incremented by Period[i], since the wave has been 1 */
			/* exactly half of the time, regardless of the initial position. */
			/* If we exit the loop in the middle, Output[i] has to be inverted */
			/* and vol[i] incremented only if the exit status of the square */
			/* wave is 1. */
			while (R->Count[i] <= 0)
			{
				if (R->Period[i]==0)
				{
					R->Period[i]=50;
				}
				R->Count[i] += R->Period[i];

				if (R->Count[i] > 0)
				{
					R->Output[i] ^= 1;
					if (R->Output[i]) Vol[i] += R->Period[i];
					{
						break;
					}
				}
				R->Count[i] += R->Period[i];
				Vol[i] += R->Period[i];
			}
			if (R->Output[i]) Vol[i] -= R->Count[i];
		}

		Left = STEP;
		do
		{
			int NextEvent;

			if (R->Count[3] < Left) NextEvent = R->Count[3];
			else NextEvent = Left;

			if (R->Output[3]) Vol[3] += R->Count[3];
			R->Count[3] -= NextEvent;
			if (R->Count[3] <= 0)
			{
		        if (R->NoiseMode == 1) /* White Noise Mode */
		        {
			        if (((R->RNG & R->WhitenoiseTaps) != R->WhitenoiseTaps) && ((R->RNG & R->WhitenoiseTaps) != 0)) /* crappy xor! */
					{
				        R->RNG >>= 1;
				        R->RNG |= R->FeedbackMask;
					}
					else
					{
				        R->RNG >>= 1;
					}
					R->Output[3] = R->WhitenoiseInvert ? !(R->RNG & 1) : R->RNG & 1;
				}
				else /* Periodic noise mode */
				{
			        if (R->RNG & 1)
					{
				        R->RNG >>= 1;
				        R->RNG |= R->FeedbackMask;
					}
					else
					{
				        R->RNG >>= 1;
					}
					R->Output[3] = R->RNG & 1;
				}
				R->Count[3] += R->Period[3];
				if (R->Output[3]) Vol[3] += R->Period[3];
			}
			if (R->Output[3]) Vol[3] -= R->Count[3];

			Left -= NextEvent;
		} while (Left > 0);
		unsigned int Out = Vol[0] * R->Volume[0] + Vol[1] * R->Volume[1] + Vol[2] * R->Volume[2] + Vol[3] * R->Volume[3];
		if (Out > MAX_OUTPUT * STEP) Out = MAX_OUTPUT * STEP;

		Out /= STEP;
		
		if (R->bSignalAdd) {
			int Temp = pSoundBuf[0] + Out;
			if (Temp > 32767) Temp = 32767;
			if (Temp < -32768) Temp = -32768;
			*pSoundBuf++ = Temp;			
		} else {
			*pSoundBuf++ = Out;
		}
		Length--;
	}
}

void SN76496Write(int Num, int Data)
{
	struct SN76496 *R = &Chip0;
	int n, r, c;
	
	if (Num >= MAX_SN76496_CHIPS) return;
	
	if (Num == 1) R = &Chip1;
	if (Num == 2) R = &Chip2;
	if (Num == 3) R = &Chip3;
	if (Num == 4) R = &Chip4;
	
	if (Data & 0x80) {
		r = (Data & 0x70) >> 4;
		R->LastRegister = r;
		R->Register[r] = (R->Register[r] & 0x3f0) | (Data & 0x0f);
	} else {
		r = R->LastRegister;
	}
	
	c = r / 2;

#if 0
	switch (r)
	{
		case 0:	/* tone 0 : frequency */
		case 2:	/* tone 1 : frequency */
		case 4:	/* tone 2 : frequency */
		    if ((Data & 0x80) == 0) R->Register[r] = (R->Register[r] & 0x0f) | ((Data & 0x3f) << 4);

			R->Period[c] = R->UpdateStep * R->Register[r];
			if (R->Period[c] == 0) R->Period[c] = R->UpdateStep;
			if (r == 4)
			{
				/* update noise shift frequency */
				if ((R->Register[6] & 0x03) == 0x03)
					R->Period[3] = 2 * R->Period[2];
			}
			break;
		case 1:	/* tone 0 : volume */
		case 3:	/* tone 1 : volume */
		case 5:	/* tone 2 : volume */
		case 7:	/* noise  : volume */
			R->Volume[c] = R->VolTable[Data & 0x0f];
			if ((Data & 0x80) == 0) R->Register[r] = (R->Register[r] & 0x3f0) | (Data & 0x0f);
			break;
		case 6:	/* noise  : frequency, mode */
			{
			        if ((Data & 0x80) == 0) R->Register[r] = (R->Register[r] & 0x3f0) | (Data & 0x0f);
				n = R->Register[6];
				R->NoiseMode = (n & 4) ? 1 : 0;
				/* N/512,N/1024,N/2048,Tone #3 output */
				R->Period[3] = ((n&3) == 3) ? 2 * R->Period[2] : (R->UpdateStep << (5+(n&3)));
			        /* Reset noise shifter */
				R->RNG = R->FeedbackMask; /* this is correct according to the smspower document */
				//R->RNG = 0xF35; /* this is not, but sounds better in do run run */
				R->Output[3] = R->RNG & 1;
			}
			break;
	}
#else
static void (*const reg[8])(void) = {&&lbl00,&&lbl01,&&lbl00,&&lbl01,&&lbl00,&&lbl01,&&lbl06,&&lbl01};
#define DISPATCH() goto *reg[r]

		DISPATCH();

lbl00:	
/* tone 0 : frequency */
/* tone 1 : frequency */
/* tone 2 : frequency */
		    if ((Data & 0x80) == 0) R->Register[r] = (R->Register[r] & 0x0f) | ((Data & 0x3f) << 4);

			R->Period[c] = R->UpdateStep * R->Register[r];
			if (R->Period[c] == 0) R->Period[c] = R->UpdateStep;
			if (r == 4)
			{
				/* update noise shift frequency */
				if ((R->Register[6] & 0x03) == 0x03)
					R->Period[3] = 2 * R->Period[2];
			}
goto end;
/* tone 0 : volume */
/* tone 1 : volume */
/* tone 2 : volume */
/* noise  : volume */
lbl01:
			R->Volume[c] = R->VolTable[Data & 0x0f];
			if ((Data & 0x80) == 0) R->Register[r] = (R->Register[r] & 0x3f0) | (Data & 0x0f);
goto end;
lbl06:	/* noise  : frequency, mode */
			{
			        if ((Data & 0x80) == 0) R->Register[r] = (R->Register[r] & 0x3f0) | (Data & 0x0f);
				n = R->Register[6];
				R->NoiseMode = (n & 4) ? 1 : 0;
				/* N/512,N/1024,N/2048,Tone #3 output */
				R->Period[3] = ((n&3) == 3) ? 2 * R->Period[2] : (R->UpdateStep << (5+(n&3)));
			        /* Reset noise shifter */
				R->RNG = R->FeedbackMask; /* this is correct according to the smspower document */
				//R->RNG = 0xF35; /* this is not, but sounds better in do run run */
				R->Output[3] = R->RNG & 1;
			}

#endif	
end:
;
}

/*static*/ void SN76496SetGain(struct SN76496 *R,int Gain)
{
	// doit rester en type double !!
	double Out;
	Gain &= 0xff;

	/* increase max output basing on gain (0.2 dB per step) */
	Out = MAX_OUTPUT / 3;
	while (Gain-- > 0)
		Out *= ((double)1.023292992);	/* = (10 ^ (0.2/20)) */

	/* build volume table (2dB per step) */
	for (unsigned int i = 0;i < 15;i++)
	{
		/* limit volume to avoid clipping */
		if (Out > MAX_OUTPUT / 3) R->VolTable[i] = MAX_OUTPUT / 3;
		else R->VolTable[i] = (INT32)Out;

		Out /= ((double)1.258925412);	/* = 10 ^ (2/20) = 2dB */
	}
	R->VolTable[15] = 0;
}

/*static*/ void SN76496Init2(struct SN76496 *R, unsigned int Clock)
{
	unsigned int i;
	R->UpdateStep = (unsigned int)(((double)STEP * nBurnSoundRate * 16) / Clock);
		
	R->Volume[0] = R->Volume[1] = R->Volume[2] = R->Volume[3] = 0;

	R->LastRegister = 0;
	for (i = 0; i < 8; i += 2) {
		R->Register[i + 0] = 0x00;
		R->Register[i + 1] = 0x0f;
	}
	
	for (i = 0; i < 4; i++) {
		R->Output[i] = 0;
		R->Period[i] = R->Count[i] = R->UpdateStep;
	}
	
	R->FeedbackMask = 0x4000;
	R->WhitenoiseTaps = 0x03;
	R->WhitenoiseInvert = 1;
	
	R->RNG = R->FeedbackMask;
	R->Output[3] = R->RNG & 1;
}

/*static*/ void GenericStart(int Num, int Clock, int FeedbackMask, int NoiseTaps, int NoiseInvert, int SignalAdd)
{
	if (Num >= MAX_SN76496_CHIPS) return;
	
	//NumChips = Num;
	
	if (Num == 0) {
	//Chip0 = (struct SN76496*)BurnMalloc(sizeof(*Chip0));
		memset(&Chip0, 0, sizeof(Chip0));
	
		SN76496Init2(&Chip0, Clock);

		SN76496SetGain(&Chip0, 0);
		
		Chip0.FeedbackMask = FeedbackMask;
		Chip0.WhitenoiseTaps = NoiseTaps;
		Chip0.WhitenoiseInvert = NoiseInvert;
		Chip0.bSignalAdd = SignalAdd;
	}
	
	if (Num == 1) {
//		Chip1 = (struct SN76496*)BurnMalloc(sizeof(*Chip1));
		memset(&Chip1, 0, sizeof(Chip1));
	
		SN76496Init2(&Chip1, Clock);
		SN76496SetGain(&Chip1, 0);
		
		Chip1.FeedbackMask = FeedbackMask;
		Chip1.WhitenoiseTaps = NoiseTaps;
		Chip1.WhitenoiseInvert = NoiseInvert;
		Chip1.bSignalAdd = SignalAdd;
	}
	
	if (Num == 2) {
//		Chip2 = (struct SN76496*)BurnMalloc(sizeof(*Chip2));
		memset(&Chip2, 0, sizeof(Chip2));
	
		SN76496Init2(&Chip2, Clock);
		SN76496SetGain(&Chip2, 0);
		
		Chip2.FeedbackMask = FeedbackMask;
		Chip2.WhitenoiseTaps = NoiseTaps;
		Chip2.WhitenoiseInvert = NoiseInvert;
		Chip2.bSignalAdd = SignalAdd;
	}
	
	if (Num == 3) {
//		Chip3 = (struct SN76496*)BurnMalloc(sizeof(*Chip3));
		memset(&Chip3, 0, sizeof(Chip3));
	
		SN76496Init2(&Chip3, Clock);
		SN76496SetGain(&Chip3, 0);
		
		Chip3.FeedbackMask = FeedbackMask;
		Chip3.WhitenoiseTaps = NoiseTaps;
		Chip3.WhitenoiseInvert = NoiseInvert;
		Chip3.bSignalAdd = SignalAdd;
	}
	
	if (Num == 4) {
//		Chip4 = (struct SN76496*)BurnMalloc(sizeof(*Chip4));
		memset(&Chip4, 0, sizeof(Chip4));
	
		SN76496Init2(&Chip4, Clock);
		SN76496SetGain(&Chip4, 0);
		
		Chip4.FeedbackMask = FeedbackMask;
		Chip4.WhitenoiseTaps = NoiseTaps;
		Chip4.WhitenoiseInvert = NoiseInvert;
		Chip4.bSignalAdd = SignalAdd;
	}
}

void SN76489Init(int Num, int Clock, int SignalAdd)
{
	return GenericStart(Num, Clock, 0x4000, 0x03, 1, SignalAdd);
}

void SN76489AInit(int Num, int Clock, int SignalAdd)
{
	return GenericStart(Num, Clock, 0x8000, 0x06, 0, SignalAdd);
}

void SN76494Init(int Num, int Clock, int SignalAdd)
{
	return GenericStart(Num, Clock, 0x8000, 0x06, 0, SignalAdd);
}

void SN76496Init(int Num, int Clock, int SignalAdd)
{
	return GenericStart(Num, Clock, 0x8000, 0x06, 0, SignalAdd);
}

void SN76496Exit()
{
	memset(&Chip0, 0, sizeof(struct SN76496));
	memset(&Chip1, 0, sizeof(struct SN76496));
	memset(&Chip2, 0, sizeof(struct SN76496));
	memset(&Chip3, 0, sizeof(struct SN76496));
	memset(&Chip4, 0, sizeof(struct SN76496));
}
/*
void SN76496SetRoute(int Num, double nVolume, int nRouteDir)
{
#if defined FBA_DEBUG
	if (!DebugSnd_SN76496Initted) bprintf(PRINT_ERROR, _T("SN76496SetRoute called without init\n"));
	if (Num > NumChips) bprintf(PRINT_ERROR, _T("SN76496SetRoute called with invalid chip %i\n"), Num);
#endif

//	struct SN76496 *R = Chip0;
	struct SN76496 *R = &Chip0;
	if (Num >= MAX_SN76496_CHIPS) return;
	if (Num == 1) R = &Chip1;
	if (Num == 2) R = &Chip2;
	if (Num == 3) R = &Chip3;
	if (Num == 4) R = &Chip4;
	
//	R->nVolume = nVolume;
//	R->nOutputDir = nRouteDir;
}


void SN76496Exit()
{
	NumChips = 0;
	
//	if (Chip0!=NULL)
//	{
//		free(Chip0);
//		Chip0 = NULL;
//	}
	if (Chip1!=NULL)
	{
		free(Chip1);
		Chip1 = NULL;
	}
	if (Chip2!=NULL)
	{
		free(Chip2);
		Chip2 = NULL;
	}
	if (Chip3!=NULL)
	{
		free(Chip3);
		Chip3 = NULL;
	}
	if (Chip4!=NULL)
	{
		free(Chip4);
		Chip4 = NULL;
	}
}
*/
/*
int SN76496Scan(int nAction,int *pnMin)
{
	struct BurnArea ba;
	char szName[16];
	
	if ((nAction & ACB_DRIVER_DATA) == 0) {
		return 1;
	}
	
	if (pnMin != NULL) {
		*pnMin = 0x029675;
	}
	
	sprintf(szName, "SN76496 #0");
	ba.Data		= &Chip0;
	ba.nLen		= sizeof(struct SN76496);
	ba.nAddress = 0;
	ba.szName	= szName;
	BurnAcb(&ba);
	
	if (NumChips >= 1) {
		sprintf(szName, "SN76496 #1");
		ba.Data		= &Chip1;
		ba.nLen		= sizeof(struct SN76496);
		ba.nAddress = 0;
		ba.szName	= szName;
		BurnAcb(&ba);
	}
	
	if (NumChips >= 2) {
		sprintf(szName, "SN76496 #2");
		ba.Data		= &Chip2;
		ba.nLen		= sizeof(struct SN76496);
		ba.nAddress = 0;
		ba.szName	= szName;
		BurnAcb(&ba);
	}
	
	if (NumChips >= 3) {
		sprintf(szName, "SN76496 #3");
		ba.Data		= &Chip3;
		ba.nLen		= sizeof(struct SN76496);
		ba.nAddress = 0;
		ba.szName	= szName;
		BurnAcb(&ba);
	}
	
	if (NumChips >= 4) {
		sprintf(szName, "SN76496 #4");
		ba.Data		= &Chip4;
		ba.nLen		= sizeof(struct SN76496);
		ba.nAddress = 0;
		ba.szName	= szName;
		BurnAcb(&ba);
	}
	
	return 0;
}
*/

#undef MAX_SN76496_CHIPS
#undef MAX_OUTPUT
#undef STEP
