//extern UINT8* NamcoSoundProm;
void NamcoSoundUpdate(INT16* buffer, INT32 length);
void NamcoSoundUpdateStereo(INT16* buffer, INT32 length);
void NamcoSoundWrite(UINT32 offset, UINT8 data);
//void NamcoSoundInit(INT32 clock, INT32 num_voices);
void NamcoSoundInit(INT32 clock, INT32 num_voices, UINT8 *Namcocontext);
//void NacmoSoundSetRoute(INT32 nIndex, int nVolume, INT32 nRouteDir);
void NamcoSoundExit();
void NamcoSoundReset();
void NamcoSoundScan(INT32 nAction,INT32 *pnMin);

void namcos1_custom30_write(INT32 offset, INT32 data);
UINT8 namcos1_custom30_read(INT32 offset);

#define BURN_SND_NAMCOSND_ROUTE_1		0
#define BURN_SND_NAMCOSND_ROUTE_2		1

#define NacmoSoundSetAllRoutes(v, d)						\
	NacmoSoundSetRoute(BURN_SND_NAMCOSND_ROUTE_1, v, d);	\
	NacmoSoundSetRoute(BURN_SND_NAMCOSND_ROUTE_2, v, d);

#define MAX_VOICES 8
#define MAX_VOLUME 16

typedef struct
{
	UINT32 frequency;
	UINT32 counter;
	INT32 volume[2];
	INT32 noise_sw;
	INT32 noise_state;
	INT32 noise_seed;
	UINT32 noise_counter;
	INT32 noise_hold;
	INT32 waveform_select;
} sound_channel;

typedef struct namco_sound
{
	sound_channel channel_list[MAX_VOICES];
	sound_channel *last_channel;

	INT32 wave_size;
	INT32 num_voices;
	INT32 sound_enable;
	INT32 namco_clock;
	INT32 sample_rate;
	INT32 f_fracbits;
	INT32 stereo;

	INT16 *waveform[MAX_VOLUME];
	
	UINT32 update_step;
	
//	double gain[2];
//	INT32 output_dir[2];
} namco_sound;

