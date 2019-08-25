/*
  Name: psg.c
  Copyright : Gens 1999-2005
  Author: Stéphane Dallongeville
  Date: 2002
  Description: PSG - SN76489 emulator
*/
#include <stdio.h>
#include <stdarg.h>

#include "psg_sms.h"


/* Defines */

#ifndef PI
#define PI 3.14159265358979323846
#endif

// Change MAX_OUTPUT to change PSG volume (default = 0x7FFF)

#define MAX_OUTPUT 0x4FFF

#define W_NOISE 0x12000
#define P_NOISE 0x08000

//#define NOISE_DEF 0x0f35
//#define NOISE_DEF 0x0001
#define NOISE_DEF 0x4000
  /*
#define PSG_DEBUG_LEVEL 0

#if PSG_DEBUG_LEVEL > 0

#define PSG_DEBUG_0(x)                              \
fprintf(psg_debug_file, (x));
#define PSG_DEBUG_1(x, a)                           \
fprintf(psg_debug_file, (x), (a));
#define PSG_DEBUG_2(x, a, b)                        \
fprintf(psg_debug_file, (x), (a), (b));
#define PSG_DEBUG_3(x, a, b, c)                     \
fprintf(psg_debug_file, (x), (a), (b), (c));
#define PSG_DEBUG_4(x, a, b, c, d)                  \
fprintf(psg_debug_file, (x), (a), (b), (c), (d));

#else

#define PSG_DEBUG_0(x)
#define PSG_DEBUG_1(x, a)
#define PSG_DEBUG_2(x, a, b)
#define PSG_DEBUG_3(x, a, b, c)
#define PSG_DEBUG_4(x, a, b, c, d)

#endif
	*/

/* Variables */
void PSG_Reset();
//void PSG_RestoreState(void);
unsigned int PSG_Step_Table[1024] = {0};
unsigned int PSG_Volume_Table[16] = {0};
unsigned int PSG_Noise_Step_Table[4] = {0};
//unsigned int PSG_Save[8];

struct _psg PSG = {.Current_Channel = 0, .Current_Register = 0, .Noise = 0};

#if PSG_DEBUG_LEVEL > 0
FILE *psg_debug_file = NULL;
#endif


/* Functions */

void PSG_Init(unsigned int clock, unsigned int rate)
{
    int i, j;
    double out;

#if PSG_DEBUG_LEVEL > 0
    if (psg_debug_file == NULL)
    {
        psg_debug_file = fopen("psg.log", "w");
        fprintf(psg_debug_file, "PSG logging :\n\n");
    }
#endif

    for(i = 1; i < 1024; i++)
    {
        // Step calculation

        out = (double) (clock) / (double) (i << 4);        // out = frequency
        out /= (double) (rate);
        out *= 65536.0;

        PSG_Step_Table[i] = (unsigned int) out;
    }

    PSG_Step_Table[0] = PSG_Step_Table[1];

    for(i = 0; i < 3; i++)
    {
        out = (double) (clock) / (double) (1 << (9 + i));
        out /= (double) (rate);
        out *= 65536.0;

        PSG_Noise_Step_Table[i] = (unsigned int) out;
    }

    PSG_Noise_Step_Table[3] = 0;

    out = (double) MAX_OUTPUT / 3.0;

    for (i = 0; i < 15; i++)
    {
        PSG_Volume_Table[i] = (unsigned int) out;
        out /= 1.258925412;        // = 10 ^ (2/20) = 2dB
    }

    PSG_Volume_Table[15] = 0;

    PSG_Reset();
}

void PSG_Reset()
{
    unsigned int i;

    PSG.Current_Register = 0;
    PSG.Current_Channel = 0;
    PSG.Noise = 0;
    PSG.Noise_Type = 0;

    for (i = 0; i < 4; i++)
    {
        PSG.Volume[i] = 0;
        PSG.Counter[i] = 0;
        PSG.CntStep[i] = 0;
    }
/*
    for (i = 0; i < 8; i += 2)
    {
        PSG_Save[i] = 0;
        PSG_Save[i + 1] = 0x0F;            // volume = OFF
    }
*/
//    PSG_RestoreState();                // Reset
}


void PSG_Write(unsigned int data)
{
    if (data & 0x80)
    {
        PSG.Current_Register = (data & 0x70) >> 4;
        PSG.Current_Channel = PSG.Current_Register >> 1;

        data &= 0x0F;

        PSG.Register[PSG.Current_Register] = (PSG.Register[PSG.Current_Register] & 0x3F0) | data;

        if (PSG.Current_Register & 1)
        {
            // Volume
            PSG.Volume[PSG.Current_Channel] = PSG_Volume_Table[data];

//            PSG_DEBUG_2("channel %d    volume = %.8X\n", PSG.Current_Channel, PSG.Volume[PSG.Current_Channel]);
        }
        else
        {
            // Frequency
            if (PSG.Current_Channel != 3)
            {
                // Normal channel
                PSG.CntStep[PSG.Current_Channel] = PSG_Step_Table[PSG.Register[PSG.Current_Register]];

                if ((PSG.Current_Channel == 2) && ((PSG.Register[6] & 3) == 3))
                {
                    PSG.CntStep[3] = PSG.CntStep[2] >> 1;
                }

//                PSG_DEBUG_2("channel %d    step = %.8X\n", PSG.Current_Channel, PSG.CntStep[PSG.Current_Channel]);
            }
            else
            {
                // Noise channel
                PSG.Noise = NOISE_DEF;
                PSG_Noise_Step_Table[3] = PSG.CntStep[2] >> 1;
                PSG.CntStep[3] = PSG_Noise_Step_Table[data & 3];

                if (data & 4) PSG.Noise_Type = W_NOISE;
                else PSG.Noise_Type = P_NOISE;

//                PSG_DEBUG_1("channel N    type = %.2X\n", data);
            }
        }
    }
    else
    {
        if (!(PSG.Current_Register & 1))
        {
            // Frequency
            if (PSG.Current_Channel != 3)
            {
                PSG.Register[PSG.Current_Register] = (PSG.Register[PSG.Current_Register] & 0x0F) | ((data & 0x3F) << 4);

                PSG.CntStep[PSG.Current_Channel] = PSG_Step_Table[PSG.Register[PSG.Current_Register]];

                if ((PSG.Current_Channel == 2) && ((PSG.Register[6] & 3) == 3))
                {
                    PSG.CntStep[3] = PSG.CntStep[2] >> 1;
                }

//                PSG_DEBUG_2("channel %d    step = %.8X\n", PSG.Current_Channel, PSG.CntStep[PSG.Current_Channel]);
            }
        }
    }
}


void PSG_Update(signed short *buffer, unsigned int length)
{
    int i, j;
    signed short output;

    for(i = 0; i < length; i++)
    {
        output = 0;

        // channel 0, 1, 2
        for (j = 2; j >= 0; j--)
        {
            if (PSG.Counter[j] & 0x10000) output += PSG.Volume[j];
            PSG.Counter[j] += PSG.CntStep[j];
        }

        // noise channel (3)
        if (PSG.Noise & 1)
        {
            output += PSG.Volume[3];
            PSG.Noise ^= PSG.Noise_Type;
        }
        if (PSG.Counter[3] & 0x10000)
        {
            PSG.Counter[3] &= 0xFFFF;
            PSG.Noise >>= 1;
        }
        PSG.Counter[3] += PSG.CntStep[3];

        buffer[i] = output;
//        buffer[0][i] = output;
//        buffer[1][i] = output;
    }
}

/*
void PSG_SaveState(void)
{
    int i;

    for(i = 0; i < 8; i++) PSG_Save[i] = PSG.Register[i];
}
*/
/*
void PSG_RestoreState(void)
{
    int i;

    for(i = 0; i < 8; i++)
    {
        PSG_Write(0x80 | (i << 4) | (PSG_Save[i] & 0xF));
        PSG_Write((PSG_Save[i] >> 4) & 0x3F);
    }
}
*/
