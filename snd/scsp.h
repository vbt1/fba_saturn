#pragma once
#include <stdint.h>

#define SCSP_RAM (0x25a00000)
#define SCSP_SLOT_ADDR (0x25b00000)

#define SCSP_COMMON_ADDR (0x25b00400)

#define COMREG          0x01FUL
#define OREG(x)			(0x20100021UL + ((x) << 1))
#define SMPC(x)         (0x20100000UL + (x))
#define SF              0x063UL

#define IREG(x) (0x20100001UL + ((x) << 1))

#define sound_driver_size 4188

#define MEMORY_READ(t, x)                                                      \
    (*(volatile uint ## t ## _t *)(x))
	
#define MEMORY_WRITE(t, x, y)                                                  \
do {                                                                           \
        (*(volatile uint ## t ## _t *)(x) = (y));                              \
} while (0)

typedef enum cpu_smpc_cmd {
        /// SMPC command to turn master CPU on.
        SMPC_SMC_MSHON    = 0x00,
        /// SMPC command to turn slave CPU on.
        SMPC_SMC_SSHON    = 0x02,
        /// SMPC command to turn slave CPU off.
        SMPC_SMC_SSHOFF   = 0x03,
        /// SMPC command to turn sound CPU on.
        SMPC_SMC_SNDON    = 0x06,
        /// SMPC command to turn sound CPU off.
        SMPC_SMC_SNDOFF   = 0x07,
        /// SMPC command to turn CD-block on.
        SMPC_SMC_CDON     = 0x08,
        /// SMPC command to turn CD-block off.
        SMPC_SMC_CDOFF    = 0x09,
        /// SMPC command to reset the system.
        SMPC_SMC_SYSRES   = 0x0D,
        /// SMPC command to change clock to 352-mode.
        SMPC_SMC_CKCHG352 = 0x0E,
        /// SMPC command to change clock to 320-mode.
        SMPC_SMC_CKCHG320 = 0x0F,
        /// SMPC command to acquire SMPC status and peripheral data.
        SMPC_SMC_INTBACK  = 0x10,
        /// SMPC command to set RTC time.
        SMPC_RTC_SETTIME  = 0x16,
        /// SMPC command to set data to SMEM.
        SMPC_SMC_SETSMEM  = 0x17,
        /// SMPC command to send an NMI request to the master CPU.
        SMPC_SMC_NMIREQ   = 0x18,
        /// SMPC command to enable NMI generation.
        SMPC_SMC_RESENAB  = 0x19,
        /// SMPC command to disable NMI generation.
        SMPC_SMC_RESDISA  = 0x1A
} cpu_smpc_cmd_t;

smpc_smc_wait(int enter)
{
        volatile unsigned char *reg_sf;
        reg_sf = (volatile unsigned char *)SMPC(SF);

        while ((*reg_sf & 0x01) == 0x01);

        if (enter) {
                *reg_sf = 0x01;
        }
}

smpc_smc_call(cpu_smpc_cmd_t cmd)
{
//        smpc_smc_wait(1);
		smpc_wait_till_ready();

        MEMORY_WRITE(8, SMPC(COMREG), cmd);

        smpc_smc_wait(0);

        return MEMORY_READ(8, OREG(31));
}

#define SMPC_CMD_SNDON				0x06
#define SMPC_CMD_SNDOFF				0x07
#define SMPC_SMC_SNDOFF				0x07
/* E:\esp_saturn\loader_yaul\CANAL.VGM (14/09/2016 01:52:02)
   DébutPosition(h): 00000000, FinPosition(h): 0001E3BF, Longueur(h): 0001E3C0 */
smpc_smc_sndoff_call(void)
{
        return smpc_smc_call(SMPC_SMC_SNDOFF);
}

smpc_smc_sndon_call(void)
{
        return smpc_smc_call(SMPC_SMC_SNDON);
}

void snd_init()
{
   *(volatile uint8_t *)(0x25B00400) = 0x02;
   // Turn off Sound CPU
   smpc_smc_sndoff_call();

   // Make sure SCSP is set to 512k mode
   *(volatile uint8_t *)(0x25B00400) = 0x02;

   // Clear Sound Ram
   for (int i = 0; i < 0x80000; i += 4)
      *(volatile uint32_t *)(0x25A00000 + i) = 0x00000000;

   // Copy driver over
 //  for (int i = 0; i < sound_driver_size; i++)
 //     *(volatile uint8_t *)(0x25A00000 + i) = sound_driver[i];
	GFS_Load(GFS_NameToId("SDRV.BIN"),0,(void *)0x25A00000,sound_driver_size);
	
   // Turn on Sound CPU again
   smpc_smc_sndon_call();
}

typedef union scsp_common_reg
{
    uint16_t raw[1];
    uint16_t raw8[2];
} scsp_common_reg_t;

typedef union scsp_slot_regs
{
    uint16_t raw[16];
    uint32_t raw32[8];
    struct
    {
        /*
        // 0x00
        uint16_t  _ : 1;
        uint16_t  __ : 1;
        uint16_t  ___ : 1;

        uint16_t  kx : 1;
        uint16_t  kb : 1;
        uint16_t  sbctl : 2;
        uint16_t  ssctl : 2;
        uint16_t  lpctl : 2;
        uint16_t  pcm8b : 1;
        uint16_t  sah : 4;

        // 0x02
        uint16_t sal;
        */
        // 0x00
        uint32_t _ : 1;
        uint32_t __ : 1;
        uint32_t ___ : 1;

        uint32_t kyonex : 1;
        uint32_t kyonb : 1;
        uint32_t sbctl : 2;
        uint32_t ssctl : 2;
        uint32_t lpctl : 2;
        uint32_t pcm8b : 1;
        uint32_t sa : 20;

        // 0x04
        uint16_t lsa;

        // 0x06
        uint16_t lea;

        // 0x08
        uint16_t d2r : 5;
        uint16_t d1r : 5;
        uint16_t eghold : 1;
        uint16_t attack_rate : 5;

        // 0x0a
        uint16_t _____ : 1;
        uint16_t loop_start : 1;
        uint16_t kr_scale : 4;
        uint16_t decay_l : 5;
        uint16_t release_r : 5;

        // 0x0c
        uint16_t ______ : 6;
        uint16_t stwinh : 1;
        uint16_t sdir : 1;
        uint16_t total_l : 8;

        // 0x0e
        uint16_t modulation_l : 4;
        uint16_t md_xsl : 6;
        uint16_t md_ysl : 6;

        // 0x10
        uint16_t _______ : 1;
        uint16_t oct : 4;
        uint16_t ________ : 1;
        uint16_t fns : 10;

        // 0x12
        uint16_t lfo_re : 1;
        uint16_t lfo_fq : 5;
        uint16_t plfows : 2;
        uint16_t plfos : 3;
        uint16_t alfows : 2;
        uint16_t alfos : 3;

        // 0x14
        uint16_t _________ : 9;

        uint16_t isel : 4;
        uint16_t imxl : 3;
        // 0x16
        uint16_t disdl : 3;
        uint16_t dipan : 5;
        uint16_t efsdl : 3;
        uint16_t efpan : 5;

        // 0x18 pad
        uint8_t _end[8];
    } __attribute__((packed));
}
__attribute__((aligned(16)))
scsp_slot_regs_t;

#define get_scsp_slot(x) ((volatile scsp_slot_regs_t *)(SCSP_SLOT_ADDR + (x * 0x20)))
#define get_scsp_reg() ((volatile scsp_common_reg_t *)SCSP_COMMON_ADDR)