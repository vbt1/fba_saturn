/*****************************************************************************
 *
 *   m6502.c
 *   Portable 6502/65c02/65sc02/6510/n2a03 emulator V1.2
 *
 *   Copyright Juergen Buchmueller, all rights reserved.
 *   65sc02 core Copyright Peter Trauner.
 *   Deco16 portions Copyright Bryan McPhail.
 *
 *   - This source code is released as freeware for non-commercial purposes.
 *   - You are free to use and redistribute this code in modified or
 *     unmodified form, provided you list me in the credits.
 *   - If you modify this source code, you must add a notice to each modified
 *     source file that it has been changed.  If you're a nice person, you
 *     will clearly mark each change too.  :)
 *   - If you wish to use this for commercial purposes, please contact me at
 *     pullmoll@t-online.de
 *   - The author of this copywritten work reserves the right to change the
 *     terms of its usage and license at any time, including retroactively
 *   - This entire notice must remain in the source code.
 *
 *****************************************************************************/
/* 2.February 2000 PeT added 65sc02 subtype */
/* 10.March   2000 PeT added 6502 set overflow input line */
/* 13.September 2000 PeT N2A03 jmp indirect */

extern	unsigned char *Read[0x100];
extern 	unsigned char *Write[0x100];
extern	unsigned char *Fetch[0x100];

#include "../burnint.h"
#include "m6502.h"

#define M6502_INLINE	 static inline

#if ((HAS_M65SC02 || HAS_DECO16) && !HAS_M65C02)
#undef HAS_M65C02
#define HAS_M65C02 1
#endif

#define change_pc(newpc)	m6502.pc.d = (newpc)

void	(*const *insnActive)(void);

#include "ops02.h"
#include "ill02.h"


#define M6502_NMI_VEC	0xfffa
#define M6502_RST_VEC	0xfffc
#define M6502_IRQ_VEC	0xfffe

#define DECO16_RST_VEC	0xfff0
#define DECO16_IRQ_VEC	0xfff2
#define DECO16_NMI_VEC	0xfff4

#define VERBOSE 0

#define LOG(x)	do { if (VERBOSE) logerror x; } while (0)

/****************************************************************************
 * The 6502 registers.
 ****************************************************************************/

 static int m6502_IntOccured = 0;
static int m6502_ICount = 0;

static m6502_Regs m6502;

//static READ8_HANDLER( default_rdmem_id ) { return program_read_byte_8le(offset); }
//static WRITE8_HANDLER( default_wdmem_id ) { program_write_byte_8le(offset, data); }

/***************************************************************
 * include the opcode macros, functions and tables
 ***************************************************************/
//#include "t6502.c"

#if (HAS_M6510)
//#include "t6510.c"
#define insn6510 insn6502
#endif

//#include "opsn2a03.h"

//#if (HAS_N2A03)
//#include "tn2a03.c"
//#endif

//#include "opsc02.h"
/*
#if (HAS_M65SC02)
#include "t65sc02.c"
#endif

#if (HAS_DECO16)
#include "tdeco16.c"
#endif
*/
/*****************************************************************************
 *
 *      6502 CPU interface functions
 *
 *****************************************************************************/

static void m6502_common_init(UINT8 subtype, void (*const *insn)(void)/*, const char *type*/)
{
	memset(&m6502, 0, sizeof(m6502));
//	m6502.irq_callback = irqcallback;
	m6502.subtype = subtype;
	//m6502.insn = insn;
	insnActive = insn;
//	m6502.rdmem_id = default_rdmem_id;
//	m6502.wrmem_id = default_wdmem_id;

//	state_save_register_item(type, index, m6502.pc.w.l);
//	state_save_register_item(type, index, m6502.sp.w.l);
//	state_save_register_item(type, index, m6502.p);
//	state_save_register_item(type, index, m6502.a);
//	state_save_register_item(type, index, m6502.x);
//	state_save_register_item(type, index, m6502.y);
//	state_save_register_item(type, index, m6502.pending_irq);
//	state_save_register_item(type, index, m6502.after_cli);
//	state_save_register_item(type, index, m6502.nmi_state);
//	state_save_register_item(type, index, m6502.irq_state);
//	state_save_register_item(type, index, m6502.so_state);

#if (HAS_M6510) || (HAS_M6510T) || (HAS_M8502) || (HAS_M7501)
	if (subtype == SUBTYPE_6510)
	{
//		state_save_register_item(type, index, m6502.port);
//		state_save_register_item(type, index, m6502.ddr);
	}
#endif
}

void m6502_init()
{
	m6502_common_init(SUBTYPE_6502, NULL); //insn6502);
}

void m6502_reset(void)
{
	/* wipe out the rest of the m6502 structure */
	/* read the reset vector into PC */
	PCL = RDMEM(M6502_RST_VEC);
	PCH = RDMEM(M6502_RST_VEC+1);

	m6502.sp.d = 0x01ff;	/* stack pointer starts at page 1 offset FF */
	m6502.p = F_T|F_I|F_Z|F_B|(P&F_D);	/* set T, I and Z flags */
	m6502.pending_irq = 0;	/* nonzero if an IRQ is pending */
	m6502.after_cli = 0;	/* pending IRQ and last insn cleared I */
	m6502.irq_state = 0;
	m6502.nmi_state = 0;

	change_pc(PCD);
}

void m6502_exit(void)
{
	/* nothing to do yet */
}

void m6502_get_context (void *dst)
{
	if( dst ) 
		*(m6502_Regs*)dst = m6502;
}

void m6502_set_context (void *src)
{
//	if( src )
//	{
		m6502 = *(m6502_Regs*)src;
//		if (m6502.subtype == SUBTYPE_6502)  insnActive = insn6502;
		insnActive = NULL; //insn6502;
//		if (m6502.subtype == SUBTYPE_65C02) insnActive = insn65c02;
//		if (m6502.subtype == SUBTYPE_2A03)  insnActive = insn2a03;
//		if (m6502.subtype == SUBTYPE_65SC02)insnActive = insn65sc02;
//		if (m6502.subtype == SUBTYPE_DECO16)insnActive = insndeco16;
//		if (m6502.subtype == SUBTYPE_6510)  insnActive = insn6510;
		change_pc(PCD);
//	}
}

M6502_INLINE void m6502_take_irq(void)
{
	if( !(P & F_I) )
	{
		EAD = M6502_IRQ_VEC;
		m6502_ICount -= 2;
		PUSH16(PCH,PCL);
		PUSH(P & ~F_B);
		P |= F_I;		/* set I flag */
		PCD = RDMEM16(EAD);
//		LOG(("M6502#%d takes IRQ ($%04x)\n", cpu_getactivecpu(), PCD));
		/* call back the cpuintrf to let it clear the line */
		if (m6502.irq_callback) (*m6502.irq_callback)(0);
		change_pc(PCD);
	}
	m6502.pending_irq = 0;
}

int m6502_execute(int cycles)
{
	m6502_ICount = cycles;

	change_pc(PCD);

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
	do
	{
		UINT8 op;
		PPC = PCD;

//		debugger_instruction_hook(Machine, PCD);

		/* if an irq is pending, take it now */
		if( m6502.pending_irq )
			m6502_take_irq();

		op = RDOP();
/*
		unsigned int *	DrvTempRom = (unsigned int *)0x00200000;		
		DrvTempRom[op]++;
		if(DrvTempRom[op]==0x000fffff)
			while(1);
*/		
		DISPATCH();


		
#include "t6502.c"
		//(*m6502.insn[op])();
//		(*insnActive[op])();
m6502_end:


		/* check if the I flag was just reset (interrupts enabled) */
		if( m6502.after_cli )
		{
//			LOG(("M6502#%d after_cli was >0", cpu_getactivecpu()));
			m6502.after_cli = 0;
			if (m6502.irq_state != M6502_CLEAR_LINE)
			{
//				LOG((": irq line is asserted: set pending IRQ\n"));
				m6502.pending_irq = 1;
			}
			else
			{
//				LOG((": irq line is clear\n"));
			}
		}
		else {
			if ( m6502.pending_irq == 2 ) {
				if ( m6502_IntOccured - m6502_ICount > 1 ) {
					m6502.pending_irq = 1;
				}
			}
			if( m6502.pending_irq == 1 )
				m6502_take_irq();
			if ( m6502.pending_irq == 2 ) {
				m6502.pending_irq = 1;
			}
		}

	} while (m6502_ICount > 0);

	return cycles - m6502_ICount;
}

void m6502_set_irq_line(int irqline, int state)
{
	if (irqline == M6502_INPUT_LINE_NMI)
	{
		if (m6502.nmi_state == state) return;
		m6502.nmi_state = state;
		if( state != M6502_CLEAR_LINE )
		{
//			LOG(( "M6502#%d set_nmi_line(ASSERT)\n", cpu_getactivecpu()));
			EAD = M6502_NMI_VEC;
			m6502_ICount -= 2;

			PUSH16(PCH,PCL);
			PUSH(P & ~F_B);
			P |= F_I;		/* set I flag */
			PCD = RDMEM16(EAD);
//			LOG(("M6502#%d takes NMI ($%04x)\n", cpu_getactivecpu(), PCD));
			change_pc(PCD);
		}
	}
	else
	{
		if( irqline == M6502_SET_OVERFLOW )
		{
			if( m6502.so_state && !state )
			{
//				LOG(( "M6502#%d set overflow\n", cpu_getactivecpu()));
				P|=F_V;
			}
			m6502.so_state=state;
			return;
		}
		m6502.irq_state = state;
		if( state != M6502_CLEAR_LINE )
		{
//			LOG(( "M6502#%d set_irq_line(ASSERT)\n", cpu_getactivecpu()));
			m6502.pending_irq = 1;
//          m6502.pending_irq = 2;
			m6502_IntOccured = m6502_ICount;
		}
	}
}

UINT32 m6502_get_pc()
{
	return m6502.pc.d;
}

/****************************************************************************
 * 2A03 section
 ****************************************************************************/
#if (HAS_N2A03)

void n2a03_init()
{
	m6502_common_init(SUBTYPE_2A03, NULL); //insn2a03);
}

/* The N2A03 is integrally tied to its PSG (they're on the same die).
   Bit 7 of address $4011 (the PSG's DPCM control register), when set,
   causes an IRQ to be generated.  This function allows the IRQ to be called
   from the PSG core when such an occasion arises. */
void n2a03_irq(void)
{
  m6502_take_irq();
}

#endif

