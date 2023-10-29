#pragma once
#include <stdint.h>
#include <string.h>
#include "scsp.h"
#include "notes.h"

static uint8_t sn74xx_regs[256];
typedef struct
{
    uint8_t latched_chan;
    uint8_t last_register;
    uint16_t registers[8];
    uint16_t period[4];
    uint8_t vol[4];
    uint8_t noise_type;
} sn76496_t;

static sn76496_t sn_chip_0;

typedef struct
{
    uint8_t oct;
    uint16_t fns;
} sn_scsp_map_t;

#include "sn_map.h"

#define SN_NOTE (note_a4)
#define SN_PER_NOTE (sn_noise_per)
#define SN_NOISE_NOTE (sn76xxx_noise_a4)

uint32_t sn_square_addr = (0x2000);
uint32_t sn_periodic_addr = (0x2400);
uint32_t sn_white_noise_addr = (0x3400);

// Math.pow(2.0, -(x/16.0));
const uint8_t volume_map[] = {
    0, 5, 11, 16, 21, 26, 32, 37, 43, 48, 53, 58, 64, 69, 74, 255};

//#include "tone_func.h"

void sn76496_w(uint8_t dd);

void sn_fix_note(uint16_t *note, uint32_t sz, uint16_t h, uint16_t l)
{
    for (int i = 0; i < sz; i++)
    {
        note[i] = note[i] & 0x8000 ? h : l;
    }
}

void sn76496_init()
{
    snd_init();
    // copy sinetable
    for (int i = 0; i < 1; i++)
    {
        sn_fix_note((uint16_t *)SN_NOTE, sizeof(SN_NOTE) / 2, 0x2000, 0);
        // sn_fix_note((uint16_t *)SN_PER_NOTE, sizeof(SN_PER_NOTE) / 2);
        sn_fix_note((uint16_t *)SN_NOISE_NOTE, sizeof(SN_NOISE_NOTE) / 2, 0x1000, 0);

        memcpy((void *)(SCSP_RAM + sn_square_addr + (i * sizeof(SN_NOTE))), SN_NOTE, sizeof(SN_NOTE));
        memcpy((void *)(SCSP_RAM + sn_periodic_addr + (i * sizeof(SN_PER_NOTE))), SN_PER_NOTE, sizeof(SN_PER_NOTE));
        memcpy((void *)(SCSP_RAM + sn_white_noise_addr + (i * sizeof(SN_NOISE_NOTE))), SN_NOISE_NOTE, sizeof(SN_NOISE_NOTE));
    }

    memset(sn74xx_regs, 0, 256);

    volatile uint16_t *control = (uint16_t *)0x25b00400;
    volatile uint16_t *intr = (uint16_t *)0x25b0042a;
    control[0] = 0x20f; // master vol
    intr[0] = 0;      // irq

    // init slots...
    volatile scsp_slot_regs_t *slots = get_scsp_slot(0);

    for (int i = 0; i < 32; i++)
    {
        slots[i].kyonb = 0;
        slots[i].kyonex = 0;
    }

    for (int i = 0; i < 5; i++)
    {
        scsp_slot_regs_t slot = {};
        memset(&slot, 0, sizeof(scsp_slot_regs_t));

        //
        slot.sa = sn_square_addr;
        slot.lsa = 0;
        slot.lea = (sizeof(SN_NOTE) / 2);

        // chan 3 => noise
        if (i == 3)
        {
            slot.sa = sn_white_noise_addr;
            slot.lea = (sizeof(SN_NOISE_NOTE) / 2);
        }

        // chan 4 => per
        if (i == 4)
        {
            slot.sa = sn_periodic_addr;
            slot.lea = (sizeof(SN_PER_NOTE) / 2);
        }

        // uned 16 bit
        slot.pcm8b = 0;
        slot.sbctl = 0;

        slot.lpctl = 1;
        slot.attack_rate = 31;
        slot.release_r = 31;
//        slot.loop_start = 1;  // passage à 0 steve + ced
        slot.loop_start = 0;
        slot.kr_scale = 0;
        slot.sdir = 0;
        slot.disdl = 7;
        slot.total_l = 0xff;

        slot.fns = 0;
        slot.oct = 0;

        // memcpy(&slots[i], 0, sizeof(scsp_slot_regs_t));
        for (int r = 0; r < 16; r++)
        {
            slots[i].raw[r] = slot.raw[r];
        }
    }

    for (int i = 0; i < 4; i++)
    {
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

    // __dbg_tuneA5(&slots[0]);

    // dbg...
    if (1)
    {
        // channel 0 tone 0xfe 440hz
        sn76496_w(0b10001110); // 0b1_00_0_1110
        sn76496_w(0b00001111); // 0b0_00_0_1111
        // set vol to max
        sn76496_w(0b10010000); // 0b1_00_1_0000
    }
 //       sn76496_w(0b10010000); // 0b1_00_1_0000
    // periodic noise
    if (0)
    {
        // use periodic noise
        sn76496_w(0b11100110); // 0b1_11_0_0000
        // set vol to max
        sn76496_w(0b11110000); // 0b1_11_1_0000
    }
}

void sn76496_w(uint8_t dd)
{
    // return;
    volatile scsp_slot_regs_t *slots = get_scsp_slot(0);
    sn76496_t *chip = &sn_chip_0;

    // // emu_printf("sn76496_w %02x\n", dd);

    uint16_t r = 0;
    uint16_t n = 0;
    uint8_t chan = 0;

    if ((dd & 0x80) == 0x80)
    {
        r = (dd & 0x70) >> 4;
        chip->last_register = r;
        chip->latched_chan = chan;

        chip->registers[r] = (chip->registers[r] & 0x3f0) | (dd & 0x0f);
    }
    else
    {
        r = chip->last_register;
    }

    chan = (r >> 1) & 3;
    switch (r)
    {
        // tone: frequency
    case 0:
    case 2:
    case 4:
        if ((dd & 0x80) == 0)
            chip->registers[r] = (chip->registers[r] & 0x0f) | ((dd & 0x3f) << 4);
        if (chip->registers[r] != 0)
            chip->period[chan] = chip->registers[r] & 0x3ff;
        else
            chip->period[chan] = 0x400;
        break;
        // noise: frequency, mode
    case 6:
        n = chip->registers[6];
        chip->noise_type = n & 4;
        if ((dd & 0x80) == 0)
            chip->registers[r] = (chip->registers[r] & 0x3f0) | (dd & 0x0f);
        // N/512,N/1024,N/2048,Tone #3 output
        chip->period[3] = ((n & 3) == 3) ? (chip->period[2] << 1) : (1 << (5 + (n & 3)));
        break;
        // tone/noise: volume
    case 1:
    case 3:
    case 5:
    case 7:
        chip->vol[chan] = dd & 0x0f;
        if ((dd & 0x80) == 0)
            chip->registers[r] = (chip->registers[r] & 0x3f0) | (dd & 0x0f);

        // emu_printf("sn76496_w vol chan %x %d\n", chip->vol[chan], chan);
        break;
    }

    // if (chan < 3) // skip noise
    {

        if (chan == 3)
        {
            // return;
            volatile scsp_slot_regs_t *slot_noise = &slots[3];
            volatile scsp_slot_regs_t *slot_per = &slots[4];
            volatile scsp_slot_regs_t *slot = chip->noise_type ? slot_noise : slot_per;

            slot->kyonb = 0;
//            slot->kyonex = 0;
            slot->kyonex = 1; // vbt : modification ced + steve

            if (chip->noise_type)
            {
                slot_per->total_l = 0xff;
                slot_per->kyonb = 0;
                slot_per->kyonex = 0;
            }
            else
            {
                slot_noise->total_l = 0xff;
                slot_noise->kyonb = 0;
                slot_noise->kyonex = 0;
            }

            slot->fns = sn_scsp_map[chip->period[chan]].fns;
            slot->oct = sn_scsp_map[chip->period[chan]].oct;
            slot->total_l = volume_map[chip->vol[chan]];

            // need to wait for reset ?
            asm("nop");
            asm("nop");
            asm("nop");
            asm("nop");

            slot->kyonb = 1;
            slot->kyonex = 1;

            // emu_printf("sn76496_w noise per %x\n", chip->period[chan]);
            // emu_printf("sn76496_w noise fns %d\n", sn_scsp_map[chip->period[chan]].fns);
            // emu_printf("sn76496_w noise oct %d\n", sn_scsp_map[chip->period[chan]].oct);
            // emu_printf("sn76496_w noise vol %d\n", volume_map[chip->vol[chan]]);
        }
        else
        {
            // return;
            //* return;
            slots[chan].fns = sn_scsp_map[chip->period[chan]].fns;
            slots[chan].oct = sn_scsp_map[chip->period[chan]].oct;
            slots[chan].total_l = volume_map[chip->vol[chan]];
            // slots[chan].total_l = chip->vol[chan];

            // slots[chan].kyonb = 1;
            // slots[chan].kyonex = 1;
        }
    }
}