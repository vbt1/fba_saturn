#include "../burnint.h"
#include "../burn_sound.h"
#include "namco_snd.h"

#define SOUNDRATE   7680L //
#define INTERNAL_RATE	192000
#define MIXLEVEL	(1 << (16 - 4 - 4))
#define OUTPUT_LEVEL(n)		((n) * MIXLEVEL / chip->num_voices)
#define WAVEFORM_POSITION(n)	(((n) >> chip->f_fracbits) & 0x1f)
#define BURN_SND_CLIPVBT(A) ((A) < -0x8000 ? -0x8000 : (A) > 0x7fff ? 0x7fff : (A))

UINT8* NamcoSoundProm = NULL;
INT16 *p = (INT16*)0x00200000;

/*static*/ UINT8 namco_soundregs[0x40];
/*static*/ UINT8 *namco_wavedata = NULL;

/*static*/// struct namco_sound chip[1];// = NULL;
 struct namco_sound *chip = NULL;

/*static*/ void update_namco_waveform(INT32 offset, UINT8 data)
{
	if (chip->wave_size == 1)
	{
		INT16 wdata;
		/* use full byte, first 4 high bits, then low 4 bits */
		for (UINT32 v = 0; v < MAX_VOLUME; v++)
		{
			wdata = ((data >> 4) & 0x0f) - 8;
			chip->waveform[v][offset * 2] = OUTPUT_LEVEL(wdata * v);
			wdata = (data & 0x0f) - 8;
			chip->waveform[v][offset * 2 + 1] = OUTPUT_LEVEL(wdata * v);
		}
	}
	else
	{
		/* use only low 4 bits */
		for (UINT32 v = 0; v < MAX_VOLUME; v++)
			chip->waveform[v][offset] = OUTPUT_LEVEL(((data & 0x0f) - 8) * v);
	}
}

/*static*/ inline UINT32 namco_update_one(INT16 *buffer, INT32 length, const INT16 *wave, UINT32 counter, UINT32 freq)
{
	while (length-- > 0)
	{
		INT32 nLeftSample = 0, nRightSample = 0;
		nLeftSample = (INT32)(wave[WAVEFORM_POSITION(counter)]);
		nLeftSample = BURN_SND_CLIPVBT(nLeftSample);
		
		*buffer++ += nLeftSample;
		counter += freq * chip->update_step;
	}

	return counter;
}

void NamcoSoundUpdate(INT16* buffer, INT32 length)
{
#if defined FBA_DEBUG
//	if (!DebugSnd_NamcoSndInitted) bprintf(PRINT_ERROR, _T("NamcoSoundUpdate called without init\n"));
#endif

	sound_channel *voice;

	/* zap the contents of the buffer */
//	memset(buffer, 0, length * sizeof(*buffer) * 2);
	memset(buffer, 0, length * 4);


	/* if no sound, we're done */
	if (chip->sound_enable == 0)
		return;

	/* loop over each voice and add its contribution */
	for (voice = chip->channel_list; voice < chip->last_channel; voice++)
	{
		INT16 *mix = buffer;
		INT32 v = voice->volume[0];

		if (voice->noise_sw)
		{
			INT32 f = voice->frequency & 0xff;

			// only update if we have non-zero volume and frequency
			
			if (v && f)
			{
				INT32 hold_time = 1 << (chip->f_fracbits - 16);
				INT32 hold = voice->noise_hold;
				UINT32 delta = f << 4;
				UINT32 c = 0; voice->noise_counter;
				INT16 noise_data =OUTPUT_LEVEL(0x07 * (v >> 1));

				// add our contribution 
				for (UINT32 i = 0; i < length; i++)
				{
					INT32 cnt;

					if (voice->noise_state)
						*mix++ += noise_data;
					else
						*mix++ -= noise_data;

					if (hold)
					{
						hold--;
						continue;
					}

					hold = 	hold_time;


					c += delta;
					cnt = (c >> 12);
					c &= (1 << 12) - 1;
					for( ;cnt > 0; cnt--)
					{
						if ((voice->noise_seed + 1) & 2) voice->noise_state ^= 1;
						if (voice->noise_seed & 1) voice->noise_seed ^= 0x28000;
						voice->noise_seed >>= 1;
					}

				}
				// update the counter and hold time for this voice 
				voice->noise_counter = c;
				voice->noise_hold = hold;
			}

		}
		else
		{
			/* only update if we have non-zero volume and frequency */
			if (v && voice->frequency)
			{
				const INT16 *w = &chip->waveform[v][voice->waveform_select * 32];

				/* generate sound into buffer and update the counter for this voice */
				voice->counter = namco_update_one(mix, length, w, voice->counter, voice->frequency);
			}
		}
	}
}

void NamcoSoundWrite(UINT32 offset, UINT8 data)
{
#if defined FBA_DEBUG
//	if (!DebugSnd_NamcoSndInitted) bprintf(PRINT_ERROR, _T("NamcoSoundWrite called without init\n"));
#endif

	sound_channel *voice;
	UINT32 ch;

	data &= 0x0f;
	if (namco_soundregs[offset] == data)
		return;

	/* set the register */
	namco_soundregs[offset] = data;

	if (offset < 0x10)
		ch = (offset - 5) / 5;
	else if (offset == 0x10)
		ch = 0;
	else
		ch = (offset - 0x11) / 5;

	if (ch >= chip->num_voices)
		return;

	/* recompute the voice parameters */
	voice = chip->channel_list + ch;
	switch (offset - ch * 5)
	{
	case 0x05:
		voice->waveform_select = data & 7;
		break;

	case 0x10:
	case 0x11:
	case 0x12:
	case 0x13:
	case 0x14:
		/* the frequency has 20 bits */
		/* the first voice has extra frequency bits */
		voice->frequency = (ch == 0) ? namco_soundregs[0x10] : 0;
		voice->frequency += (namco_soundregs[ch * 5 + 0x11] << 4);
		voice->frequency += (namco_soundregs[ch * 5 + 0x12] << 8);
		voice->frequency += (namco_soundregs[ch * 5 + 0x13] << 12);
		voice->frequency += (namco_soundregs[ch * 5 + 0x14] << 16);	/* always 0 */
		break;

	case 0x15:
		voice->volume[0] = data;
		break;
	}
}

/*static*/ void namcos1_sound_write(INT32 offset, INT32 data)
{
	/* verify the offset */
	if (offset > 63)
	{
	//	logerror("NAMCOS1 sound: Attempting to write past the 64 registers segment\n");
		return;
	}

	if (namco_soundregs[offset] == data)
		return;

	/* set the register */
	namco_soundregs[offset] = data;

	UINT32 ch = offset / 8;
	if (ch >= chip->num_voices)
		return;

	/* recompute the voice parameters */
	sound_channel *voice = chip->channel_list + ch;

	switch (offset - ch * 8)
	{
		case 0x00:
			voice->volume[0] = data & 0x0f;
			break;

		case 0x01:
			voice->waveform_select = (data >> 4) & 15;
		case 0x02:
		case 0x03:
			/* the frequency has 20 bits */
			voice->frequency = (namco_soundregs[ch * 8 + 0x01] & 15) << 16;	/* high bits are from here */
			voice->frequency += namco_soundregs[ch * 8 + 0x02] << 8;
			voice->frequency += namco_soundregs[ch * 8 + 0x03];
			break;

		case 0x04:
			voice->volume[1] = data & 0x0f;
	
			INT32 nssw = ((data & 0x80) >> 7);
			if (++voice == chip->last_channel)
				voice = chip->channel_list;
			voice->noise_sw = nssw;
			break;
	}
}

/*static*/ INT32 build_decoded_waveform()
{
	INT32 size;

	if (NamcoSoundProm != NULL)
		namco_wavedata = NamcoSoundProm;

	/* 20pacgal has waves in RAM but old sound system */
	if (NamcoSoundProm == NULL && chip->num_voices != 3)
	{
		chip->wave_size = 1;
		size = 32 * 16;		/* 32 samples, 16 waveforms */
	}
	else
	{
		chip->wave_size = 0;
		size = 32 * 8;		/* 32 samples, 8 waveforms */
	}

	for (UINT32 v = 0; v < MAX_VOLUME; v++)
	{
		chip->waveform[v] = p;
		p += size;
	}

	/* We need waveform data. It fails if region is not specified. */
	if (namco_wavedata)
	{
		for (UINT32 offset = 0; offset < 256; offset++)
			update_namco_waveform(offset, namco_wavedata[offset]);
	}

	return 0;
}

void NamcoSoundInit(INT32 clock, INT32 num_voices, UINT8 *Namcocontext)
{
//	DebugSnd_NamcoSndInitted = 1;
	
	INT32 clock_multiple;
	sound_channel *voice;
	
	chip = (struct namco_sound*)Namcocontext;
	memset(chip, 0, sizeof(*chip));
	
	memset(namco_soundregs, 0, 0x40);

	chip->num_voices = num_voices;
	chip->last_channel = chip->channel_list + chip->num_voices;
	chip->stereo = 0;

	/* adjust internal clock */
	chip->namco_clock = clock;
	for (clock_multiple = 0; chip->namco_clock < INTERNAL_RATE; clock_multiple++)
		chip->namco_clock *= 2;

	chip->f_fracbits = clock_multiple + 15;

	/* adjust output clock */
	chip->sample_rate = chip->namco_clock;

	/* build the waveform table */
	if (build_decoded_waveform()) return;
	
	/* start with sound enabled, many games don't have a sound enable register */
	chip->sound_enable = 1;

	/* reset all the voices */
	for (voice = chip->channel_list; voice < chip->last_channel; voice++)
	{
		voice->frequency = 0;
		voice->volume[0] = voice->volume[1] = 0;
		voice->waveform_select = 0;
		voice->counter = 0;
		voice->noise_sw = 0;
		voice->noise_state = 0;
		voice->noise_seed = 1;
		voice->noise_counter = 0;
		voice->noise_hold = 0;
	}
	voice = NULL;
	chip->update_step = INTERNAL_RATE / SOUNDRATE;
}

void NamcoSoundExit()
{
#if defined FBA_DEBUG
//	if (!DebugSnd_NamcoSndInitted) bprintf(PRINT_ERROR, _T("NamcoSoundExit called without init\n"));
#endif
	namco_wavedata = NULL;
	NamcoSoundProm = NULL;
	chip->last_channel = NULL;

	for (UINT32 v = 0; v < MAX_VOLUME; v++)
	{
		chip->waveform[v] = NULL;
	}

//	free(p);
//	memset(p,0x00,0x30000);
	p = NULL;

//	if (namco_soundregs) {
//		free(namco_soundregs);
//		namco_soundregs = NULL;
//	}

//	if (chip) {
//		memset(chip, 0, sizeof(*chip));
//		free(chip);
		chip = NULL;
//	}
	
//	DebugSnd_NamcoSndInitted = 0;
}

