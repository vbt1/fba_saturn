#pragma once
#include <stdint.h>
#include <string.h>
#include "./scsp.h"
#include "notes.h"

static uint8_t sn74xx_regs[256];
typedef struct
{
    uint8_t last_register;
    uint16_t registers[8];
    uint16_t period[4];
} sn76496_t;

static sn76496_t sn_chip_0;

typedef struct
{
    uint8_t oct;
    uint16_t fns;
} sn_scsp_map_t;

#include "sn_map.h"

//#define SN_NOTE (note_c_d_4)

// The SN76489 attenuates the volume by 2dB for each step in the volume register
// 32767, 26028, 20675, 16422, 13045, 10362,  8231,  6568,
// 5193,  4125,  3277,  2603,  2067,  1642,  1304,     0
uint8_t sn_scsp_tl[] = {

};

#define SN_NOTE (note_a4_square)
#define SN_PER_NOTE (sn_noise_per)
#define SN_NOISE_NOTE (sn_noise_per)

uint32_t sn_square_addr = (0x2000);
uint32_t sn_periodic_addr = (0x2400);
uint32_t sn_white_noise_addr = (0x2800);

void sn76496_init()
{
    snd_init();
    // copy sinetable
    for (int i = 0; i < 1; i++)
    {

        memcpy((void *)(SCSP_RAM + sn_square_addr + (i * sizeof(SN_NOTE))), SN_NOTE, sizeof(SN_NOTE));
        memcpy((void *)(SCSP_RAM + sn_periodic_addr + (i * sizeof(SN_PER_NOTE))), SN_PER_NOTE, sizeof(SN_PER_NOTE));
        memcpy((void *)(SCSP_RAM + sn_white_noise_addr + (i * sizeof(SN_NOISE_NOTE))), SN_NOISE_NOTE, sizeof(SN_NOISE_NOTE));
    }

    memset(sn74xx_regs, 0, 256);

    volatile uint16_t *control = (uint16_t *)0x25b00400;
    volatile uint16_t *intr = (uint16_t *)0x25b0042a;
    control[0] = 0xf; // master vol
    intr[0] = 0;      // irq

    // init slots...
    volatile scsp_slot_regs_t *slots = get_scsp_slot(0);
    // 8 chanl * 4 op
    for (int i = 0; i < 8 * 4; i++)
    {
        slots[i].raw[0] = (sn_square_addr << 16) + 0x130;
        slots[i].raw[1] = sn_square_addr & 0xffff;
        slots[i].raw[2] = 0;
        slots[i].raw[3] = sizeof(SN_NOTE);

        slots[i].raw[4] = 0x1F;
        slots[i].raw[5] = 0x1F;

        slots[i].raw[6] = 255;
        slots[i].raw[7] = 0;
        slots[i].raw[8] = 0;
        slots[i].raw[9] = 0;
        slots[i].raw[10] = 0;

        slots[i].raw[11] = 0xA000;
        slots[i].raw[12] = 0;
        slots[i].attack_rate = 31;
        slots[i].release_r = 31;
        slots[i].total_l = 7;

        // slots[i].dipan = 0;
        // slots[i].lpctl = 0;

        // slots[i].lpctl = 1;
        // slots[i].disdl = 7;
    }

    for (int i = 0; i < 4; i++)
    {
        slots[i].total_l = 0;
        slots[i].disdl = 7;
    }

    // slots[3].alfows = 3; // noise
    // slots[3].plfows = 3; // noise
    // slots[3].ssctl = 1;  // select noise

    for (int i = 0; i < 4; i++)
    {
        slots[i].total_l = 0;
        slots[i].kyonb = 1;
        slots[i].kyonex = 1;
    }

    sn76496_t *chip = &sn_chip_0;
    chip->last_register = 0;
    for (int i = 0; i < 8; i += 2)
    {
        chip->registers[i] = 0;
        chip->registers[i + 1] = 0x0f; /* volume = 0 */
    }
}

void sn76496_w(uint8_t dd)
{
    volatile scsp_slot_regs_t *slots = get_scsp_slot(0);
    uint16_t r = 0;
    sn76496_t *chip = &sn_chip_0;

    if (dd & 0x80)
    {
        // r = chan + type
        r = (dd & 0x70) >> 4;
        chip->last_register = r;
        chip->registers[r] = (chip->registers[r] & 0x3f0) | (dd & 0x0f);
    }
    else
    {
        r = chip->last_register;
    }

    uint8_t chan = r >> 1;
    uint16_t tone = 0;
    uint8_t vol = 0;

    switch (r)
    {
    case 0: // tone 0: frequency
    case 2: // tone 1: frequency
    case 4: // tone 2: frequency
        if ((dd & 0x80) == 0)
            chip->registers[r] = (chip->registers[r] & 0x0f) | ((dd & 0x3f) << 4);

        chip->period[chan] = chip->registers[r];

        tone = chip->registers[r] & 0x3ff;
        slots[chan].fns = sn_scsp_map[tone].fns;
        slots[chan].oct = sn_scsp_map[tone].oct;
        break;

    case 1: // tone 0: volume
    case 3: // tone 1: volume
    case 5: // tone 2: volume
    case 7: // noise: volume
        if ((dd & 0x80) == 0)
            chip->registers[r] = (chip->registers[r] & 0x3f0) | (dd & 0x0f);

        vol = chip->registers[r] & 0xf;
        slots[chan].total_l = vol << 2;
        break;
    case 6: // noise: frequency, mode
        if ((dd & 0x80) == 0)
            chip->registers[r] = (chip->registers[r] & 0x3f0) | (dd & 0x0f);

        uint16_t n = chip->registers[6];

        uint8_t tt = (n >> 2) & 3; // selects the mode (white (1) or "periodic" (0)).
        uint8_t rr = n & 3;        // select the shift rate

        tone = 0;

        slots[3].kyonb = 0;
        slots[3].kyonex = 0;
        if (tt == 1)
        {
            slots[3].sa = sn_white_noise_addr;
            slots[3].lea = sizeof(SN_PER_NOTE);
        }
        else
        {
            slots[3].sa = sn_periodic_addr;
            slots[3].lea = sizeof(SN_NOISE_NOTE);
        }
        slots[3].lpctl = 1;
        slots[3].pcm8b = 1;
        slots[3].kyonb = 1;
        slots[3].kyonex = 1;

        switch (rr)
        {
            // periodic
        case 0:
            slots[3].fns = sn_noise_scsp_map[0].fns;
            slots[3].oct = sn_noise_scsp_map[0].oct;
            break;
        case 1:
            slots[3].fns = sn_noise_scsp_map[1].fns;
            slots[3].oct = sn_noise_scsp_map[1].oct;
            break;
        case 2:
            slots[3].fns = sn_noise_scsp_map[2].fns;
            slots[3].oct = sn_noise_scsp_map[2].oct;
            break;
        case 3:
            tone = chip->period[2];
            slots[3].fns = sn_scsp_map[tone].fns;
            slots[3].oct = sn_scsp_map[tone].oct;
            break;
        }

        break;

    default:
        break;
    }
}