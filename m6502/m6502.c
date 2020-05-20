/*****************************************************************************
 *
 *	 m6502.c
 *	 Portable 6502/65c02/6510 emulator
 *
 *	 Copyright (c) 1998 Juergen Buchmueller, all rights reserved.
 *
 *	 - This source code is released as freeware for non-commercial purposes.
 *	 - You are free to use and redistribute this code in modified or
 *	   unmodified form, provided you list me in the credits.
 *	 - If you modify this source code, you must add a notice to each modified
 *	   source file that it has been changed.  If you're a nice person, you
 *	   will clearly mark each change too.  :)
 *	 - If you wish to use this for commercial purposes, please contact me at
 *	   pullmoll@t-online.de
 *	 - The author of this copywritten work reserves the right to change the
 *     terms of its usage and license at any time, including retroactively
 *   - This entire notice must remain in the source code.
 *
 *****************************************************************************/

#include <stdio.h>
#include "memory.h"
#include "string.h"
//#include "driver.h"
#include "m6502.h"
#include "m6502ops.h"

int     M6502_ICount = 0;
int 	M6502_Type = M6502_PLAIN;
static	void(**insn)(void);
static	M6502_Regs	m6502;
/* 6502 Flags */
#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_R 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80

void M6502WriteByte(unsigned short Address, unsigned char Data);
// A table for the test
static unsigned char M6502_byTestTable[ 256 ];

// Value and Flag Data
struct value_table_tag
{
  unsigned char byValue;
  unsigned char byFlag;
};
static unsigned char M6502_byTestTable[256];
static struct value_table_tag M6502_ASL[256];
static struct value_table_tag M6502_LSR[256];
static struct value_table_tag M6502_ROL[2][256];
static struct value_table_tag M6502_ROR[2][256];

extern	unsigned char *Read[0x100];
extern 	unsigned char *Write[0x100];
extern	unsigned char *Fetch[0x100];

/***************************************************************
 * include the opcode macros, functions and tables
 ***************************************************************/
 
#if !USE_GOTO 
#include "tbl6502.h"
#include "tbl65c02.h"
#include "tbl6510.h"
#endif
/*****************************************************************************
 *
 *		6502 CPU interface functions
 *
 *****************************************************************************/

unsigned M6502_GetPC (void)
{
	return PCD;
}

void M6502_GetRegs (M6502_Regs *Regs)
{
	*Regs = m6502;
}

void M6502_SetRegs (M6502_Regs *Regs)
{
	m6502 = *Regs;
}

void M6502_Reset(void)
{
FNT_Print256_2bpp((volatile unsigned char *)0x25e40000,(unsigned char *)"switch             ",10,70);		
	switch (M6502_Type)
	{
#if SUPP65C02
        case M6502_65C02:
			insn = insn65c02;
            break;
#endif
#if SUPP6510
        case M6502_6510:
			insn = insn6510;
			break;
#endif
        default:
#if !USE_GOTO		
            insn = insn6502;
#endif			
            break;
    }
FNT_Print256_2bpp((volatile unsigned char *)0x25e40000,(unsigned char *)"memset             ",10,70);
    /* wipe out the m6502 structure */
	memset(&m6502, 0, sizeof(M6502_Regs));

	/* set T, I and Z flags */
	P = FLAG_R | FLAG_I | FLAG_Z;

    /* stack starts at 0x01ff */
	m6502.sp.D = 0x1ff;
FNT_Print256_2bpp((volatile unsigned char *)0x25e40000,(unsigned char *)"RDMEM             ",10,70);

    /* read the reset vector into PC */
	PCL = RDMEM(M6502_RST_VEC);
	PCH = RDMEM(M6502_RST_VEC+1);
	change_pc16(PCD);
FNT_Print256_2bpp((volatile unsigned char *)0x25e40000,(unsigned char *)"M6502_Clear_pending_Interrupts             ",10,70);
    /* clear pending interrupts */
    M6502_Clear_Pending_Interrupts();
}

int M6502_Execute(int cycles)
{
#if USE_GOTO	
static void (*const insn6502[0x100])(void) = {
	&&m6502_00,&&m6502_01,&&m6502_02,&&m6502_03,&&m6502_04,&&m6502_05,&&m6502_06,&&m6502_07,
	&&m6502_08,&&m6502_09,&&m6502_0a,&&m6502_0b,&&m6502_0c,&&m6502_0d,&&m6502_0e,&&m6502_0f,
	&&m6502_10,&&m6502_11,&&m6502_12,&&m6502_13,&&m6502_14,&&m6502_15,&&m6502_16,&&m6502_17,
	&&m6502_18,&&m6502_19,&&m6502_1a,&&m6502_1b,&&m6502_1c,&&m6502_1d,&&m6502_1e,&&m6502_1f,
	&&m6502_20,&&m6502_21,&&m6502_22,&&m6502_23,&&m6502_24,&&m6502_25,&&m6502_26,&&m6502_27,
	&&m6502_28,&&m6502_29,&&m6502_2a,&&m6502_2b,&&m6502_2c,&&m6502_2d,&&m6502_2e,&&m6502_2f,
	&&m6502_30,&&m6502_31,&&m6502_32,&&m6502_33,&&m6502_34,&&m6502_35,&&m6502_36,&&m6502_37,
	&&m6502_38,&&m6502_39,&&m6502_3a,&&m6502_3b,&&m6502_3c,&&m6502_3d,&&m6502_3e,&&m6502_3f,
	&&m6502_40,&&m6502_41,&&m6502_42,&&m6502_43,&&m6502_44,&&m6502_45,&&m6502_46,&&m6502_47,
	&&m6502_48,&&m6502_49,&&m6502_4a,&&m6502_4b,&&m6502_4c,&&m6502_4d,&&m6502_4e,&&m6502_4f,
	&&m6502_50,&&m6502_51,&&m6502_52,&&m6502_53,&&m6502_54,&&m6502_55,&&m6502_56,&&m6502_57,
	&&m6502_58,&&m6502_59,&&m6502_5a,&&m6502_5b,&&m6502_5c,&&m6502_5d,&&m6502_5e,&&m6502_5f,
	&&m6502_60,&&m6502_61,&&m6502_62,&&m6502_63,&&m6502_64,&&m6502_65,&&m6502_66,&&m6502_67,
	&&m6502_68,&&m6502_69,&&m6502_6a,&&m6502_6b,&&m6502_6c,&&m6502_6d,&&m6502_6e,&&m6502_6f,
	&&m6502_70,&&m6502_71,&&m6502_72,&&m6502_73,&&m6502_74,&&m6502_75,&&m6502_76,&&m6502_77,
	&&m6502_78,&&m6502_79,&&m6502_7a,&&m6502_7b,&&m6502_7c,&&m6502_7d,&&m6502_7e,&&m6502_7f,
	&&m6502_80,&&m6502_81,&&m6502_82,&&m6502_83,&&m6502_84,&&m6502_85,&&m6502_86,&&m6502_87,
	&&m6502_88,&&m6502_89,&&m6502_8a,&&m6502_8b,&&m6502_8c,&&m6502_8d,&&m6502_8e,&&m6502_8f,
	&&m6502_90,&&m6502_91,&&m6502_92,&&m6502_93,&&m6502_94,&&m6502_95,&&m6502_96,&&m6502_97,
	&&m6502_98,&&m6502_99,&&m6502_9a,&&m6502_9b,&&m6502_9c,&&m6502_9d,&&m6502_9e,&&m6502_9f,
	&&m6502_a0,&&m6502_a1,&&m6502_a2,&&m6502_a3,&&m6502_a4,&&m6502_a5,&&m6502_a6,&&m6502_a7,
	&&m6502_a8,&&m6502_a9,&&m6502_aa,&&m6502_ab,&&m6502_ac,&&m6502_ad,&&m6502_ae,&&m6502_af,
	&&m6502_b0,&&m6502_b1,&&m6502_b2,&&m6502_b3,&&m6502_b4,&&m6502_b5,&&m6502_b6,&&m6502_b7,
	&&m6502_b8,&&m6502_b9,&&m6502_ba,&&m6502_bb,&&m6502_bc,&&m6502_bd,&&m6502_be,&&m6502_bf,
	&&m6502_c0,&&m6502_c1,&&m6502_c2,&&m6502_c3,&&m6502_c4,&&m6502_c5,&&m6502_c6,&&m6502_c7,
	&&m6502_c8,&&m6502_c9,&&m6502_ca,&&m6502_cb,&&m6502_cc,&&m6502_cd,&&m6502_ce,&&m6502_cf,
	&&m6502_d0,&&m6502_d1,&&m6502_d2,&&m6502_d3,&&m6502_d4,&&m6502_d5,&&m6502_d6,&&m6502_d7,
	&&m6502_d8,&&m6502_d9,&&m6502_da,&&m6502_db,&&m6502_dc,&&m6502_dd,&&m6502_de,&&m6502_df,
	&&m6502_e0,&&m6502_e1,&&m6502_e2,&&m6502_e3,&&m6502_e4,&&m6502_e5,&&m6502_e6,&&m6502_e7,
	&&m6502_e8,&&m6502_e9,&&m6502_ea,&&m6502_eb,&&m6502_ec,&&m6502_ed,&&m6502_ee,&&m6502_ef,
	&&m6502_f0,&&m6502_f1,&&m6502_f2,&&m6502_f3,&&m6502_f4,&&m6502_f5,&&m6502_f6,&&m6502_f7,
	&&m6502_f8,&&m6502_f9,&&m6502_fa,&&m6502_fb,&&m6502_fc,&&m6502_fd,&&m6502_fe,&&m6502_ff
};
	#define DISPATCH() goto *insn6502[op]
#endif
	
    M6502_ICount = cycles;

	change_pc16(PCD);

    do
    {
		if (m6502.halt)
            break;

#if USE_GOTO
		unsigned char op = RDOP();
		DISPATCH();
		
#include "tbl6502.h"
		
m6502_end:
#else
	        insn[RDOP()]();
#endif		

        /* check if the I flag was just reset (interrupts enabled) */
        if (m6502.after_cli)
            m6502.after_cli = 0;
        else
		if (m6502.pending_nmi || (m6502.pending_irq && !(P & FLAG_I)))
        {
            if (m6502.pending_nmi)
            {
                m6502.pending_nmi = 0;
				EAD = M6502_NMI_VEC;
            }
            else
            {
                m6502.pending_irq = 0;
				EAD = M6502_IRQ_VEC;
            }
			M6502_ICount -= 7;
			m6502.halt = 0;
			PUSH(PCH);
			PUSH(PCL);
			COMPOSE_P;
			PUSH(P & ~FLAG_B);
			P = (P & ~FLAG_D) | FLAG_I;		/* knock out D and set I flag */
			PCD = RDMEM16(EAD);
			change_pc16(PCD);
        }

    } while (M6502_ICount > 0);

    return cycles - M6502_ICount;
}

void M6502_Cause_Interrupt(int type)
{
	if (type == M6502_INT_NMI)
		m6502.pending_nmi = 1;
	else
	if (type == M6502_INT_IRQ)
		m6502.pending_irq = 1;
}

void M6502_Clear_Pending_Interrupts(void)
{
	m6502.pending_nmi = M6502_INT_NONE;
	m6502.pending_irq = M6502_INT_NONE;
}

void M6502_Init()
{
  unsigned char idx;
  unsigned char idx2;

  // Make a table for the test
  idx = 0;
  do
  {
    if ( idx == 0 )
      M6502_byTestTable[ 0 ] = FLAG_Z;
    else
    if ( idx > 127 )
      M6502_byTestTable[ idx ] = FLAG_N;
    else
      M6502_byTestTable[ idx ] = 0;

    ++idx;
  } while ( idx != 0 );

  // Make a table ASL
  idx = 0;
  do
  {
    M6502_ASL[ idx ].byValue = idx << 1;
    M6502_ASL[ idx ].byFlag = 0;

    if ( idx > 127 )
      M6502_ASL[ idx ].byFlag = FLAG_C;

    if ( M6502_ASL[ idx ].byValue == 0 )
      M6502_ASL[ idx ].byFlag |= FLAG_Z;
    else
    if ( M6502_ASL[ idx ].byValue & 0x80 )
      M6502_ASL[ idx ].byFlag |= FLAG_N;

    ++idx;
  } while ( idx != 0 );

  // Make a table LSR
  idx = 0;
  do
  {
    M6502_LSR[ idx ].byValue = idx >> 1;
    M6502_LSR[ idx ].byFlag = 0;

    if ( idx & 1 )
      M6502_LSR[ idx ].byFlag = FLAG_C;

    if ( M6502_LSR[ idx ].byValue == 0 )
      M6502_LSR[ idx ].byFlag |= FLAG_Z;

    ++idx;
  } while ( idx != 0 );

  // Make a table ROL
  for ( idx2 = 0; idx2 < 2; ++idx2 )
  {
    idx = 0;
    do
    {
      M6502_ROL[ idx2 ][ idx ].byValue = ( idx << 1 ) | idx2;
      M6502_ROL[ idx2 ][ idx ].byFlag = 0;

      if ( idx > 127 )
        M6502_ROL[ idx2 ][ idx ].byFlag = FLAG_C;

      if ( M6502_ROL[ idx2 ][ idx ].byValue == 0 )
        M6502_ROL[ idx2 ][ idx ].byFlag |= FLAG_Z;
      else
      if ( M6502_ROL[ idx2 ][ idx ].byValue & 0x80 )
        M6502_ROL[ idx2 ][ idx ].byFlag |= FLAG_N;

      ++idx;
    } while ( idx != 0 );
  }

  // Make a table ROR
  for ( idx2 = 0; idx2 < 2; ++idx2 )
  {
    idx = 0;
    do
    {
      M6502_ROR[ idx2 ][ idx ].byValue = ( idx >> 1 ) | ( idx2 << 7 );
      M6502_ROR[ idx2 ][ idx ].byFlag = 0;

      if ( idx & 1 )
        M6502_ROR[ idx2 ][ idx ].byFlag = FLAG_C;

      if ( M6502_ROR[ idx2 ][ idx ].byValue == 0 )
        M6502_ROR[ idx2 ][ idx ].byFlag |= FLAG_Z;
      else
      if ( M6502_ROR[ idx2 ][ idx ].byValue & 0x80 )
        M6502_ROR[ idx2 ][ idx ].byFlag |= FLAG_N;

      ++idx;
    } while ( idx != 0 );
  }	
}

