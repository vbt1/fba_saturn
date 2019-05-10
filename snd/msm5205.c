#include "../burnint.h"
#include "msm5205.h"
#include "math.h"
#define SOUND_LEN 192
#define MAX_MSM5205	 2
/*static*/ INT32 nNumChips = 0;
#define HZ 60

typedef struct 
{
	INT32 data;               /* next adpcm data              */
	INT32 vclk;               /* vclk signal (external mode)  */
	INT32 reset;              /* reset pin signal             */
	INT32 prescaler;          /* prescaler selector S1 and S2 */
	INT32 bitwidth;           /* bit width selector -3B/4B    */
	INT32 signal;             /* current ADPCM signal         */
	INT32 step;               /* current ADPCM step           */
	float volume;
//	INT32 output_dir;

//	INT32 use_seperate_vols;  /* support custom Taito panning hardware */
//	double left_volume;
//	double right_volume;

	INT32 clock;		  /* clock rate */

	void (*vclk_callback)();  /* VCLK callback              */
	INT32 (*stream_sync)(INT32);
	INT32 select;       	  /* prescaler / bit width selector        */
	INT32 bAdd;
	INT32 streampos;

	INT32 diff_lookup[49*16];
} _MSM5205_state;

INT16 *stream[MAX_MSM5205];
/*static*/ _MSM5205_state chips[MAX_MSM5205];
/*static*/ _MSM5205_state *voice;

/*static*/ void MSM5205_playmode(INT32 chip, INT32 select);

/*static const*/ INT32 index_shift[8] = { -1, -1, -1, -1, 2, 4, 6, 8 };

/*static*/ void ComputeTables(UINT32 chip)
{
	voice = &chips[chip];

	/* nibble to bit map */
	/*static const*/ INT8 nbl2bit[16][4] =
	{
		{ 1, 0, 0, 0}, { 1, 0, 0, 1}, { 1, 0, 1, 0}, { 1, 0, 1, 1},
		{ 1, 1, 0, 0}, { 1, 1, 0, 1}, { 1, 1, 1, 0}, { 1, 1, 1, 1},
		{-1, 0, 0, 0}, {-1, 0, 0, 1}, {-1, 0, 1, 0}, {-1, 0, 1, 1},
		{-1, 1, 0, 0}, {-1, 1, 0, 1}, {-1, 1, 1, 0}, {-1, 1, 1, 1}
	};

	/* loop over all possible steps */
	INT32 d[49] = {
		16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 50, 55, 60, 66, 73, 80, 88, 97,
		107, 118, 130, 143, 157, 173, 190, 209, 230, 253, 279, 307, 337, 371, 408, 
		449, 494, 544, 598, 658, 724, 796, 876, 963, 1060, 1166, 1282, 1411, 1552
	};
	for (UINT8 step = 0; step <= 48; step++)
	{
		INT32 stepval = d[step];

		/* loop over all nibbles and compute the difference */
		for (UINT32 nib = 0; nib < 16; nib++)
		{
			voice->diff_lookup[(step*16) + nib] = nbl2bit[nib][0] *
				(stepval   * nbl2bit[nib][1] +
				 stepval/2 * nbl2bit[nib][2] +
				 stepval/4 * nbl2bit[nib][3] +
				 stepval/8);
		}
	}
}

/*static*/ void MSM5205_playmode(INT32 chip, INT32 select)
{
	/*static const*/ INT32 prescaler_table[2][4] = { {96,48,64,0}, {160, 40, 80, 20} };
	INT32 prescaler = prescaler_table[(select >> 3) & 1][select & 3];
	INT32 bitwidth = (select & 4) ? 4 : 3;

	if( voice->prescaler != prescaler )
	{
		voice->prescaler = prescaler;
	}

	if( voice->bitwidth != bitwidth )
	{
		voice->bitwidth = bitwidth;
	}
}

/*static*/ 
void MSM5205StreamUpdate(INT32 chip)
{
	voice = &chips[chip];

//	UINT32 len = voice->stream_sync((SOUND_LEN * hz) / 100);
	UINT32 len = voice->stream_sync((SOUND_LEN * HZ));
	if (len > (UINT32)SOUND_LEN) len = SOUND_LEN;
	UINT32 pos = voice->streampos;

	if (pos >= len) return;

	len -= pos;
	voice->streampos = pos + len;
	
	if (pos == 0) {
		memset (stream[chip], 0, SOUND_LEN * sizeof(INT16));
	}

	if(voice->streampos!=0)
	{
		INT16 *buffer = stream[chip];
		buffer += pos;
		
		if(voice->signal)
		{
			INT32 i = 0;

			INT32 volval = (INT32)((voice->signal * 16)); // * voice->volume);
			INT16 val = volval;
			while (len)
			{
				buffer[i] = val;
				len--; i++;
			}
		} else {
			memset (buffer, 0, sizeof(INT16) * len);
		}
	}
}

/*static*/ void MSM5205_vclk_callback(INT32 chip)
{
	voice = &chips[chip];

	if(voice->vclk_callback)(*voice->vclk_callback)();

	INT32 new_signal;

	if(voice->reset)
	{
		new_signal = 0;
		voice->step = 0;
	}
	else
	{
		INT32 val = voice->data;
		new_signal = voice->signal + voice->diff_lookup[voice->step * 16 + (val & 15)];
		if (new_signal > 2047) new_signal = 2047;
		else if (new_signal < -2048) new_signal = -2048;
		voice->step += index_shift[val & 7];
		if (voice->step > 48) voice->step = 48;
		else if (voice->step < 0) voice->step = 0;
	}
	// update when signal changed 
	if( voice->signal != new_signal)
	{
//		signed short *nSoundBuffer = (signed short *)(0x25a20000);
		MSM5205StreamUpdate(chip);
		voice->signal = new_signal;
	}
}

//#define CLIP(A) ((A) < -0x8000 ? -0x8000 : (A) > 0x7fff ? 0x7fff : (A))
#define CLIP(A) ((A) < -0x5000 ? -0x5000 : (A) > 0x4fff ? 0x4fff : (A))

void MSM5205InitPos(INT32 chip)
{
	voice = &chips[chip];
	voice->streampos = 0;
}

void MSM5205Render(INT32 chip, INT16 *buffer, INT32 len)
{
	voice = &chips[chip];
	INT16 *source = stream[chip];

	MSM5205StreamUpdate(chip);
	voice->streampos = 0;
	
	for (INT32 i = 0; i < len; i++) 
	{
//		int	Temp = buffer[0] + source[i];
		int	Temp = source[i];
		if (Temp > 32767) Temp = 32767;
		else {if (Temp < -32768) Temp = -32768;}
		*buffer++ = Temp;
	}
}

void MSM5205RenderDirect(INT32 chip, INT16 *buffer, INT32 len)
{
	voice = &chips[chip];
	INT16 *source = stream[chip];

	MSM5205StreamUpdate(chip);
	voice->streampos = 0;
	
	for (UINT32 i = 0; i < len; i++) 
	{
		*buffer++ = *source++;
	}
}

void MSM5205RenderDirectSlave(INT32 chip)
{
	unsigned int deltaSlave		= *(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);
	signed short *nSoundBuffer= (signed short *)(0x25a24000+deltaSlave*(sizeof(signed short)));

	voice = &chips[chip];
	INT16 *source = stream[chip];
	MSM5205StreamUpdate(chip);
	voice->streampos = 0;
	
	for (UINT32 i = 0; i < 128; i++) 
	{
		*nSoundBuffer++ = *source++;
	}
}

void MSM5205Reset()
{
//#if defined FBA_DEBUG
//	if (!DebugSnd_MSM5205Initted) bprintf(PRINT_ERROR, _T("MSM5205Reset called without init\n"));
//#endif

	for (INT32 chip = 0; chip < MAX_MSM5205; chip++)
	{
		voice = &chips[chip];

		if (stream[chip] == NULL) continue;

		voice->data   = 0;
		voice->vclk    = 0;
		voice->reset   = 0;
		voice->signal  = 0;
		voice->step    = 0;

		MSM5205_playmode(chip,voice->select);
		voice->streampos = 0;
	}
}

void MSM5205Init(INT32 chip, INT16 *addr, INT32 (*stream_sync)(INT32), INT32 clock, void (*vclk_callback)(), INT32 select, INT32 bAdd)
{
//	DebugSnd_MSM5205Initted = 1;

	voice = &chips[chip];

	memset (voice, 0, sizeof(_MSM5205_state));

	voice->stream_sync	= stream_sync;
	voice->vclk_callback	= vclk_callback;
	voice->select		= select;
	voice->clock		= clock;
	voice->bAdd		= bAdd;
//	voice->volume	= 1.00;
//	voice->volume	= nVolume;
//	voice->output_dir = BURN_SND_ROUTE_BOTH;
	
	stream[chip]		= (INT16*)addr;
	ComputeTables (chip);
	nNumChips = chip;
}

void MSM5205Exit()
{
#if defined FBA_DEBUG
//	if (!DebugSnd_MSM5205Initted) bprintf(PRINT_ERROR, _T("MSM5205Exit called without init\n"));
#endif

	for (INT32 chip = 0; chip < MAX_MSM5205; chip++)
	{
		voice = &chips[chip];

		if (stream[chip] == NULL) continue;

		memset (voice, 0, sizeof(_MSM5205_state));

//		free (stream[chip]);
		stream[chip] = NULL;
	}
	
//	DebugSnd_MSM5205Initted = 0;
	nNumChips = 0;
}

void MSM5205VCLKWrite(INT32 chip, INT32 vclk)
{
/*
#if defined FBA_DEBUG
//	if (!DebugSnd_MSM5205Initted) bprintf(PRINT_ERROR, _T("MSM5205VCLKWrite called without init\n"));
	if (chip > nNumChips) bprintf(PRINT_ERROR, _T("MSM5205VCLKWrite called with invalid chip %x\n"), chip);
#endif
*/
	voice = &chips[chip];

	if (voice->prescaler == 0)
	{
		if( voice->vclk != vclk)
		{
			voice->vclk = vclk;
			if( !vclk ) MSM5205_vclk_callback(chip);
		}
	}
}

void MSM5205ResetWrite(INT32 chip, INT32 reset)
{
/*
#if defined FBA_DEBUG
//	if (!DebugSnd_MSM5205Initted) bprintf(PRINT_ERROR, _T("MSM5205ResetWrite called without init\n"));
	if (chip > nNumChips) bprintf(PRINT_ERROR, _T("MSM5205ResetWrite called with invalid chip %x\n"), chip);
#endif
*/
	voice = &chips[chip];
	voice->reset = reset;
}

void MSM5205DataWrite(INT32 chip, INT32 data)
{
/*
#if defined FBA_DEBUG
//	if (!DebugSnd_MSM5205Initted) bprintf(PRINT_ERROR, _T("MSM5205DataWrite called without init\n"));
	if (chip > nNumChips) bprintf(PRINT_ERROR, _T("MSM5205DataWrite called with invalid chip %x\n"), chip);
#endif
*/
	voice = &chips[chip];

	if( voice->bitwidth == 4)
		voice->data = data & 0x0f;
	else
		voice->data = (data & 0x07)<<1;
}

void MSM5205PlaymodeWrite(INT32 chip, INT32 select)
{
/*
#if defined FBA_DEBUG
//	if (!DebugSnd_MSM5205Initted) bprintf(PRINT_ERROR, _T("MSM5205PlaymodeWrite called without init\n"));
	if (chip > nNumChips) bprintf(PRINT_ERROR, _T("MSM5205PlaymodeWrite called with invalid chip %x\n"), chip);
#endif
*/
	voice = &chips[chip];
	MSM5205_playmode(chip,select);
}

void MSM5205Update()
{

#if defined FBA_DEBUG
//	if (!DebugSnd_MSM5205Initted) bprintf(PRINT_ERROR, _T("MSM5205Update called without init\n"));
#endif

	for (INT32 chip = 0; chip < MAX_MSM5205; chip++)
	{
		voice = &chips[chip];

		if (voice->prescaler) {
			MSM5205_vclk_callback(chip);
		} else {
			if (stream[chip]) {
				MSM5205StreamUpdate(chip);
			}
		}
	}
}

// see MSM5205_playmode for a more in-depth explanation of this
INT32 MSM5205CalcInterleave(INT32 chip, INT32 cpu_speed)
{
/*
#if defined FBA_DEBUG
//	if (!DebugSnd_MSM5205Initted) bprintf(PRINT_ERROR, _T("MSM5205CalcInterleave called without init\n"));
	if (chip > nNumChips) bprintf(PRINT_ERROR, _T("MSM5205CalcInterleave called with invalid chip %x\n"), chip);
#endif
*/
/*	static const*/ INT32 table[2][4] = { {96, 48, 64, 0}, {160, 40, 80, 20} };
/*
	char toto[100];
	char *titi = &toto[0];
	titi=itoa(cpu_speed);

	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)"speed ",4,20);

	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)titi,80,20);
*/
	voice = &chips[chip];
/*
	titi=itoa(voice->select);
	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)"select ",4,30);

	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)titi,80,30);
*/
	if ((voice->select & 3) == 3) {
		return 133;  // (usually...)
	}
/*
	titi=itoa(HZ);
	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)"hz ",4,40);

	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)titi,80,40);
*/
	INT32 ret = cpu_speed / (cpu_speed / (voice->clock / table[(voice->select >> 3) & 1][voice->select & 3]));
/*
	titi=itoa(ret / HZ );
	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)"ret ",4,50);

	FNT_Print256_2bpp((volatile unsigned char *)0x25e60000,(unsigned char *)titi,80,50);
*/
	return ret / HZ;
}
