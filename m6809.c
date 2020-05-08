/*** m6809: Portable 6809 emulator ******************************************

    Copyright John Butler

    References:

        6809 Simulator V09, By L.C. Benschop, Eidnhoven The Netherlands.

        m6809: Portable 6809 emulator, DS (6809 code in MAME, derived from
            the 6809 Simulator V09)

        6809 Microcomputer Programming & Interfacing with Experiments"
            by Andrew C. Staugaard, Jr.; Howard W. Sams & Co., Inc.

    System dependencies:    UINT16 must be 16 bit unsigned int
                            UINT8 must be 8 bit unsigned int
                            UINT32 must be more than 16 bits
                            arrays up to 65536 bytes must be supported
                            machine must be twos complement

    History:
991026 HJB:
    Fixed missing calls to cpu_changepc() for the TFR and EXG ocpodes.
    Replaced m6809_slapstic checks by a macro (CHANGE_PC). ESB still
    needs the tweaks.

991024 HJB:
    Tried to improve speed: Using bit7 of cycles1/2 as flag for multi
    byte opcodes is gone, those opcodes now call fetch_effective_address().
    Got rid of the slow/fast flags for stack (S and U) memory accesses.
    Minor changes to use 32 bit values as arguments to memory functions
    and added defines for that purpose (e.g. X = 16bit XD = 32bit).

990312 HJB:
    Added bugfixes according to Aaron's findings.
    Reset only sets CC_II and CC_IF, DP to zero and PC from reset vector.
990311 HJB:
    Added _info functions. Now uses static m6808_Regs struct instead
    of single statics. Changed the 16 bit registers to use the generic
    PAIR union. Registers defined using macros. Split the core into
    four execution loops for M6802, M6803, M6808 and HD63701.
    TST, TSTA and TSTB opcodes reset carry flag.
    Modified the read/write stack handlers to push LSB first then MSB
    and pull MSB first then LSB.

990228 HJB:
    Changed the interrupt handling again. Now interrupts are taken
    either right at the moment the lines are asserted or whenever
    an interrupt is enabled and the corresponding line is still
    asserted. That way the pending_interrupts checks are not
    needed anymore. However, the CWAI and SYNC flags still need
    some flags, so I changed the name to 'int_state'.
    This core also has the code for the old interrupt system removed.

990225 HJB:
    Cillegned up the code here and there, added some comments.
    Slightly changed the SAR opcodes (similiar to other CPU cores).
    Added symbolic names for the flag bits.
    Changed the way CWAI/Interrupt() handle CPU state saving.
    A new flag M6809_STATE in pending_interrupts is used to determine
    if a state save is needed on interrupt entry or already done by CWAI.
    Added M6809_IRQ_LINE and M6809_FIRQ_LINE defines to m6809.h
    Moved the internal interrupt_pending flags from m6809.h to m6809.c
    Changed CWAI cycles2[0x3c] to be 2 (plus all or at illegst 19 if
    CWAI actually pushes the entire state).
    Implemented undocumented TFR/EXG for undefined source and mixed 8/16
    bit transfers (they should transfer/exchange the constant $ff).
    Removed unused jmp/jsr _slap functions from 6809ops.c,
    m6809_slapstick check moved into the opcode functions.

*****************************************************************************/
//#include "debugger.h"
#include "burnint.h"
#include "m6809.h"
/* 6809 registers */
m6809_Regs m6809;

/* Enable big switch statement for the main opcodes */
//#ifndef BIG_SWITCH
//#define BIG_SWITCH  1
//#endif

//#define VERBOSE 0

//#define LOG(x)	do { if (VERBOSE) logerror x; } while (0)

//extern offs_t m6809_dasm(char *buffer, offs_t pc, const UINT8 *oprom, const UINT8 *opram);

#define M6809_INLINE		static	 inline
#define change_pc(newpc)	m6809.pc.w.l = (newpc)
#define M6809_CLEAR_LINE	0
#define M6809_INPUT_LINE_NMI	32
#define USE_CYCLES(A)		m6809.m6809_ICount -= (A);
#define RET(A)															\
        if ((m6809.m6809_ICount -= A) > 0) goto m6809_Exec;				\
        goto m6809_Exec_End;
#define DISPATCH_MAIN() goto *m6809_main[m6809.ireg]
#define DISPATCH10() goto *m6809_pref10[ireg2]
#define DISPATCH11() goto *m6809_pref11[ireg2]
#define DISPATCH_FETCH() goto *m6809_fetch[postbyte]

M6809_INLINE void fetch_effective_address( void );

/* flag bits in the cc register */
#define CC_C    0x01        /* Carry */
#define CC_V    0x02        /* Overflow */
#define CC_Z    0x04        /* Zero */
#define CC_N    0x08        /* Negative */
#define CC_II   0x10        /* Inhibit IRQ */
#define CC_H    0x20        /* Half (auxiliary) carry */
#define CC_IF   0x40        /* Inhibit FIRQ */
#define CC_E    0x80        /* entire state pushed */



#define pPPC    m6809.ppc
#define pPC 	m6809.pc
#define pU		m6809.u
#define pS		m6809.s
#define pX		m6809.x
#define pY		m6809.y
#define pD		m6809.d

#define	PPC		m6809.ppc.w.l
#define PC  	m6809.pc.w.l
#define PCD 	m6809.pc.d
#define U		m6809.u.w.l
#define UD		m6809.u.d
#define S		m6809.s.w.l
#define SD		m6809.s.d
#define X		m6809.x.w.l
#define XD		m6809.x.d
#define Y		m6809.y.w.l
#define YD		m6809.y.d
#define D   	m6809.d.w.l
#define A   	m6809.d.b.h
#define B		m6809.d.b.l
#define DP		m6809.dp.b.h
#define DPD 	m6809.dp.d
#define CC  	m6809.cc

static PAIR ea;         /* effective address */
#define EA	ea.w.l
#define EAD ea.d

#define CHANGE_PC change_pc(PCD)

#define M6809_CWAI		8	/* set when CWAI is waiting for an interrupt */
#define M6809_SYNC		16	/* set when SYNC is waiting for an interrupt */
#define M6809_LDS		32	/* set when LDS occured at least once */

#define CHECK_IRQ_LINES 												\
	if( m6809.irq_state[M6809_IRQ_LINE] != M6809_CLEAR_LINE ||				\
		m6809.irq_state[M6809_FIRQ_LINE] != M6809_CLEAR_LINE )				\
		m6809.int_state &= ~M6809_SYNC; /* clear SYNC flag */			\
	if( m6809.irq_state[M6809_FIRQ_LINE]!=M6809_CLEAR_LINE && !(CC & CC_IF) ) \
	{																	\
		/* fast IRQ */													\
		/* HJB 990225: state already saved by CWAI? */					\
		if( m6809.int_state & M6809_CWAI )								\
		{																\
			m6809.int_state &= ~M6809_CWAI;  /* clear CWAI */			\
			m6809.extra_cycles += 7;		 /* subtract +7 cycles */	\
        }                                                               \
		else															\
		{																\
			CC &= ~CC_E;				/* save 'short' state */        \
			PUSHWORD(pPC);												\
			PUSHBYTE(CC);												\
			m6809.extra_cycles += 10;	/* subtract +10 cycles */		\
		}																\
		CC |= CC_IF | CC_II;			/* inhibit FIRQ and IRQ */		\
		PCD=RM16(0xfff6);												\
		CHANGE_PC;														\
	}																	\
	else																\
	if( m6809.irq_state[M6809_IRQ_LINE]!=M6809_CLEAR_LINE && !(CC & CC_II) )	\
	{																	\
		/* standard IRQ */												\
		/* HJB 990225: state already saved by CWAI? */					\
		if( m6809.int_state & M6809_CWAI )								\
		{																\
			m6809.int_state &= ~M6809_CWAI;  /* clear CWAI flag */		\
			m6809.extra_cycles += 7;		 /* subtract +7 cycles */	\
		}																\
		else															\
		{																\
			CC |= CC_E; 				/* save entire state */ 		\
			PUSHWORD(pPC);												\
			PUSHWORD(pU);												\
			PUSHWORD(pY);												\
			PUSHWORD(pX);												\
			PUSHBYTE(DP);												\
			PUSHBYTE(B);												\
			PUSHBYTE(A);												\
			PUSHBYTE(CC);												\
			m6809.extra_cycles += 19;	 /* subtract +19 cycles */		\
		}																\
		CC |= CC_II;					/* inhibit IRQ */				\
		PCD=RM16(0xfff8);												\
		CHANGE_PC;														\
	}

/* public globals */
//static int m6809.m6809_ICount;

/* these are re-defined in m6809.h TO RAM, ROM or functions in cpuintrf.c */
#define RM(Addr)		M6809_RDMEM(Addr)
#define WM(Addr,Value)	M6809_WRMEM(Addr,Value)
#define ROP(Addr)		M6809_RDOP(Addr)
#define ROP_ARG(Addr)	M6809_RDOP_ARG(Addr)

/* macros to access memory */
#define IMMBYTE(b)	b = ROP_ARG(PCD); PC++
#define IMMWORD(w)	w.d = (ROP_ARG(PCD)<<8) | ROP_ARG((PCD+1)&0xffff); PC+=2

#define PUSHBYTE(b) --S; WM(SD,b)
#define PUSHWORD(w) --S; WM(SD,w.b.l); --S; WM(SD,w.b.h)
#define PULLBYTE(b) b = RM(SD); S++
#define PULLWORD(w) w = RM(SD)<<8; S++; w |= RM(SD); S++

#define PSHUBYTE(b) --U; WM(UD,b);
#define PSHUWORD(w) --U; WM(UD,w.b.l); --U; WM(UD,w.b.h)
#define PULUBYTE(b) b = RM(UD); U++
#define PULUWORD(w) w = RM(UD)<<8; U++; w |= RM(UD); U++

#define CLR_HNZVC   CC&=~(CC_H|CC_N|CC_Z|CC_V|CC_C)
#define CLR_NZV 	CC&=~(CC_N|CC_Z|CC_V)
#define CLR_NZ		CC&=~(CC_N|CC_Z)
#define CLR_HNZC	CC&=~(CC_H|CC_N|CC_Z|CC_C)
#define CLR_NZVC	CC&=~(CC_N|CC_Z|CC_V|CC_C)
#define CLR_Z		CC&=~(CC_Z)
#define CLR_NZC 	CC&=~(CC_N|CC_Z|CC_C)
#define CLR_ZC		CC&=~(CC_Z|CC_C)

/* macros for CC -- CC bits affected should be reset before calling */
#define SET_Z(a)		if(!a)SEZ
#define SET_Z8(a)		SET_Z((UINT8)a)
#define SET_Z16(a)		SET_Z((UINT16)a)
//#define SET_N8(a)		CC|=((a&0x80)>>4)
//#define SET_N16(a)		CC|=((a&0x8000)>>12)
//#define SET_N8(a)		CC|=((a>>4)&0x8)
//#define SET_N16(a)		CC|=((a>>12)&0x8)

//#define SET_H(a,b,r)	CC|=(((a^b^r)&0x10)<<1)
//#define SET_C8(a)		CC|=((a&0x100)>>8)
//#define SET_C16(a)		CC|=((a&0x10000)>>16)

//#define SET_C8(a)		CC|=((a>>8)&0x1)
//#define SET_C16(a)		CC|=((a>>16)&0x1)

//#define SET_V8(a,b,r)	CC|=(((a^b^r^(r>>1))&0x80)>>6)
//#define SET_V16(a,b,r)	CC|=(((a^b^r^(r>>1))&0x8000)>>14)

//#define SET_V8(a,b,r)	CC|=(((a^b^r^(r>>1))>>6)&0x2)
//#define SET_V16(a,b,r)	CC|=(((a^b^r^(r>>1))>>14)&0x2)

/*
	UINT32 r,d;
	PAIR b;
*/

static const UINT8 flags8i[256]=	 /* increment */
{
CC_Z,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
CC_N|CC_V,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N
};
static const UINT8 flags8d[256]= /* decrement */
{
CC_Z,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,CC_V,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,
CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N,CC_N
};
#define SET_FLAGS8I(a)		{CC|=flags8i[(a)&0xff];}
#define SET_FLAGS8D(a)		{CC|=flags8d[(a)&0xff];}

/* combos */
//#define SET_NZ8(a)			{SET_N8(a);SET_Z(a);}
//#define SET_NZ16(a)			{SET_N16(a);SET_Z(a);}
//#define SET_FLAGS8(a,b,r)	{SET_N8(r);SET_Z8(r);SET_V8(a,b,r);SET_C8(r);}
//#define SET_FLAGS16(a,b,r)	{SET_N16(r);SET_Z16(r);SET_V16(a,b,r);SET_C16(r);}





/* for treating an unsigned byte as a signed word */
#define SIGNED(b) ((UINT16)(b&0x80?b|0xff00:b))

/* macros for addressing modes (postbytes have their own code) */
#define DIRECT	EAD = DPD; IMMBYTE(ea.b.l)
#define IMM8	EAD = PCD; PC++
#define IMM16	EAD = PCD; PC+=2
#define EXTENDED IMMWORD(ea)

/* macros to set status flags */
#if defined(SEC)
#undef SEC
#endif
#define SEC CC|=CC_C
#define CLC CC&=~CC_C
#define SEZ CC|=CC_Z
#define CLZ CC&=~CC_Z
#define SEN CC|=CC_N
#define CLN CC&=~CC_N
#define SEV CC|=CC_V
#define CLV CC&=~CC_V
#define SEH CC|=CC_H
#define CLH CC&=~CC_H

/* macros for convenience */
#define DIRBYTE(b) {DIRECT;b=RM(EAD);}
#define DIRWORD(w) {DIRECT;w.d=RM16(EAD);}
#define EXTBYTE(b) {EXTENDED;b=RM(EAD);}
#define EXTWORD(w) {EXTENDED;w.d=RM16(EAD);}

/* macros for branch instructions */
#define BRANCH(f) { 					\
	UINT8 t;							\
	IMMBYTE(t); 						\
	if( f ) 							\
	{									\
		PC += SIGNED(t);				\
		CHANGE_PC;						\
	}									\
}

#define LBRANCH(f) {                    \
	PAIR t; 							\
	IMMWORD(t); 						\
	if( f ) 							\
	{									\
		USE_CYCLES(1);				\
		PC += t.w.l;					\
		CHANGE_PC;						\
	}									\
}

#define NXORV  ((CC&CC_N)^((CC&CC_V)<<2))

/* macros for setting/getting registers in TFR/EXG instructions */
#if 0

#if (!BIG_SWITCH)
/* timings for 1-byte opcodes */
static const UINT8 cycles1[] =
{
	/*   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
  /*0*/  6, 0, 0, 6, 6, 0, 6, 6, 6, 6, 6, 0, 6, 6, 3, 6,
  /*1*/  0, 0, 2, 4, 0, 0, 5, 9, 0, 2, 3, 0, 3, 2, 8, 6,
  /*2*/  3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
  /*3*/  4, 4, 4, 4, 5, 5, 5, 5, 0, 5, 3, 6,20,11, 0,19,
  /*4*/  2, 0, 0, 2, 2, 0, 2, 2, 2, 2, 2, 0, 2, 2, 0, 2,
  /*5*/  2, 0, 0, 2, 2, 0, 2, 2, 2, 2, 2, 0, 2, 2, 0, 2,
  /*6*/  6, 0, 0, 6, 6, 0, 6, 6, 6, 6, 6, 0, 6, 6, 3, 6,
  /*7*/  7, 0, 0, 7, 7, 0, 7, 7, 7, 7, 7, 0, 7, 7, 4, 7,
  /*8*/  2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 2, 4, 7, 3, 0,
  /*9*/  4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 6, 7, 5, 5,
  /*A*/  4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 6, 7, 5, 5,
  /*B*/  5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 5, 7, 8, 6, 6,
  /*C*/  2, 2, 2, 4, 2, 2, 2, 2, 2, 2, 2, 2, 3, 0, 3, 3,
  /*D*/  4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5,
  /*E*/  4, 4, 4, 6, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5,
  /*F*/  5, 5, 5, 7, 5, 5, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6
};

/* timings for 2-byte opcodes */
static unsigned char cycles2[] =
{
	/*    0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F */
  /*0*/	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*1*/	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*2*/	  0,  5,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
  /*3*/	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 20,
  /*4*/	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*5*/	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*6*/	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*7*/	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  /*8*/	  0,  0,  0,  5,  0,  0,  0,  0,  0,  0,  0,  0,  5,  0,  4,  0,
  /*9*/	  0,  0,  0,  7,  0,  0,  0,  0,  0,  0,  0,  0,  7,  0,  6,  6,
  /*A*/	  0,  0,  0,  7,  0,  0,  0,  0,  0,  0,  0,  0,  7,  0,  6,  6,
  /*B*/	  0,  0,  0,  8,  0,  0,  0,  0,  0,  0,  0,  0,  8,  0,  7,  7,
  /*C*/	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,  0,
  /*D*/	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  6,  6,
  /*E*/	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  6,  6,
  /*F*/	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  7,  7
};

#endif
#endif

 M6809_INLINE void SET_N8(UINT32 a)	
{
	CC|=((a>>4)&0x8);
}

M6809_INLINE void SET_N16(UINT32 a)	
{
//	CC|=((a>>12)&0x8);
	CC|=((a&0x8000)>>12); // vbt semble plus rapide
}
//#define SET_N8(a)		CC|=((a>>4)&0x8)
//#define SET_N16(a)		CC|=((a>>12)&0x8)

M6809_INLINE void SET_H(UINT32 a,UINT8 b,UINT32 r)
{
	CC|=(((a^b^r)&0x10)<<1);
}

//#define SET_H(a,b,r)	CC|=(((a^b^r)&0x10)<<1)
/*
M6809_INLINE void SET_C8(UINT32 a)	
{
	CC|=((a>>8)&0x1);
}

M6809_INLINE void SET_C16(UINT32 a)	
{
	CC|=((a>>16)&0x1);
}
*/
#define SET_C8(a)	CC|=((a>>8)&0x1)
#define SET_C16(a)	CC|=((a>>16)&0x1)
#define RM16( Addr ) RM(Addr) << 8 | RM(Addr+1)

M6809_INLINE void SET_V8(UINT32 a,UINT8 b,UINT32 r)	
{
	CC|=(((a^b^r^(r>>1))>>6)&0x2);
}

M6809_INLINE void SET_V16(UINT32 a,UINT8 b,UINT32 r)	
{
//	CC|=(((a^b^r^(r>>1))>>14)&0x2);
	CC|=(((a^b^r^(r>>1))&0x2000)>>14);  // vbt semble plus rapide
}
//#define SET_V8(a,b,r)	CC|=(((a^b^r^(r>>1))>>6)&0x2)
//#define SET_V16(a,b,r)	CC|=(((a^b^r^(r>>1))>>14)&0x2)

M6809_INLINE void SET_FLAGS16(UINT32 a,UINT8 b,UINT32 r)	
{
	 SET_N16(r);
	 SET_Z16(r);
	 SET_V16(a,b,r);
	 SET_C16(r);
}
// #define SET_FLAGS16(a,b,r)	{SET_N16(r);SET_Z16(r);SET_V16(a,b,r);SET_C16(r);}

M6809_INLINE void SET_FLAGS8(UINT32 a,UINT8 b,UINT32 r)	
{
	SET_N8(r);
	SET_Z8(r);
	SET_V8(a,b,r);
	SET_C8(r);
}
// #define SET_FLAGS8(a,b,r)	{SET_N8(r);SET_Z8(r);SET_V8(a,b,r);SET_C8(r);}

M6809_INLINE void SET_NZ8(UINT32 a)	
{
	SET_N8(a);
	SET_Z(a);
}

M6809_INLINE void SET_NZ16(UINT32 a)	
{
	SET_N16(a);
	SET_Z(a);
}

M6809_INLINE void WM16( UINT32 Addr, PAIR *p )
{
	WM( Addr, p->b.h );
	WM( (Addr+1)&0xffff, p->b.l );
}

/****************************************************************************
 * Get all registers in given buffer
 ****************************************************************************/
void m6809_get_context(void *dst)
{
	if( dst )
		*(m6809_Regs*)dst = m6809;
}

/****************************************************************************
 * Set all registers to given values
 ****************************************************************************/
void m6809_set_context(void *src)
{
//	if( src )
		m6809 = *(m6809_Regs*)src;
	CHANGE_PC;

    CHECK_IRQ_LINES;
}

/****************************************************************************/
/* Reset registers to their initial values                                  */
/****************************************************************************/
void m6809_init(int (*irqcallback)(int))
{
	m6809.irq_callback = irqcallback;
}

void m6809_reset(void)
{
	m6809.int_state = 0;
	m6809.nmi_state = M6809_CLEAR_LINE;
	m6809.irq_state[0] = M6809_CLEAR_LINE;
	m6809.irq_state[1] = M6809_CLEAR_LINE;

	DPD = 0;			/* Reset direct page register */

    CC |= CC_II;        /* IRQ disabled */
    CC |= CC_IF;        /* FIRQ disabled */

	PCD = RM16(0xfffe);
	CHANGE_PC;
}

/*
static void m6809_exit(void)
{

}*/

/* Generate interrupts */
/****************************************************************************
 * Set IRQ line state
 ****************************************************************************/
void m6809_set_irq_line(int irqline, int state)
{
	if (irqline == M6809_INPUT_LINE_NMI)
	{
		if (m6809.nmi_state == state) return;
		m6809.nmi_state = state;
//		LOG(("M6809#%d set_irq_line (NMI) %d\n", cpu_getactivecpu(), state));
		if( state == M6809_CLEAR_LINE ) return;

		/* if the stack was not yet initialized */
	    if( !(m6809.int_state & M6809_LDS) ) return;

	    m6809.int_state &= ~M6809_SYNC;
		/* HJB 990225: state already saved by CWAI? */
		if( m6809.int_state & M6809_CWAI )
		{
			m6809.int_state &= ~M6809_CWAI;
			m6809.extra_cycles += 7;	/* subtract +7 cycles next time */
	    }
		else
		{
			CC |= CC_E; 				/* save entire state */
			PUSHWORD(pPC);
			PUSHWORD(pU);
			PUSHWORD(pY);
			PUSHWORD(pX);
			PUSHBYTE(DP);
			PUSHBYTE(B);
			PUSHBYTE(A);
			PUSHBYTE(CC);
			m6809.extra_cycles += 19;	/* subtract +19 cycles next time */
		}
		CC |= CC_IF | CC_II;			/* inhibit FIRQ and IRQ */
		PCD = RM16(0xfffc);
		CHANGE_PC;
	}
	else if (irqline < 2)
	{
//	    LOG(("M6809#%d set_irq_line %d, %d\n", cpu_getactivecpu(), irqline, state));
		m6809.irq_state[irqline] = state;
		if (state == M6809_CLEAR_LINE) return;
		CHECK_IRQ_LINES;
	}
}




/* execute instructions on this CPU until icount expires */
int m6809_execute(int cycles)	/* NS 970908 */
{
static void (*const m6809_main[0x100])(void) = {
	&&neg_di, &&neg_di, &&illegal,&&com_di, &&lsr_di, &&illegal,&&ror_di, &&asr_di, 	/* 00 */
	&&asl_di, &&rol_di, &&dec_di, &&illegal,&&inc_di, &&tst_di, &&jmp_di, &&clr_di,
	&&pref10, &&pref11, &&nop,	&&sync,	&&illegal,&&illegal,&&lbra,	&&lbsr,		/* 10 */
	&&illegal,&&daa,	&&orcc,	&&illegal,&&andcc,	&&sex,	&&exg,	&&tfr,
	&&bra,	&&brn,	&&bhi,	&&bls,	&&bcc,	&&bcs,	&&bne,	&&beq,		/* 20 */
	&&bvc,	&&bvs,	&&bpl,	&&bmi,	&&bge,	&&blt,	&&bgt,	&&ble,
	&&leax,	&&leay,	&&leas,	&&leau,	&&pshs,	&&puls,	&&pshu,	&&pulu,		/* 30 */
	&&illegal,&&rts,	&&abx,	&&rti,	&&cwai,	&&mul,	&&illegal,&&swi,
	&&nega,	&&illegal,&&illegal,&&coma,	&&lsra,	&&illegal,&&rora,	&&asra,		/* 40 */
	&&asla,	&&rola,	&&deca,	&&illegal,&&inca,	&&tsta,	&&illegal,&&clra,
	&&negb,	&&illegal,&&illegal,&&comb,	&&lsrb,	&&illegal,&&rorb,	&&asrb,		/* 50 */
	&&aslb,	&&rolb,	&&decb,	&&illegal,&&incb,	&&tstb,	&&illegal,&&clrb,
	&&neg_ix, &&illegal,&&illegal,&&com_ix, &&lsr_ix, &&illegal,&&ror_ix, &&asr_ix, 	/* 60 */
	&&asl_ix, &&rol_ix, &&dec_ix, &&illegal,&&inc_ix, &&tst_ix, &&jmp_ix, &&clr_ix,
	&&neg_ex, &&illegal,&&illegal,&&com_ex, &&lsr_ex, &&illegal,&&ror_ex, &&asr_ex, 	/* 70 */
	&&asl_ex, &&rol_ex, &&dec_ex, &&illegal,&&inc_ex, &&tst_ex, &&jmp_ex, &&clr_ex,
	&&suba_im,&&cmpa_im,&&sbca_im,&&subd_im,&&anda_im,&&bita_im,&&lda_im, &&sta_im, 	/* 80 */
	&&eora_im,&&adca_im,&&ora_im, &&adda_im,&&cmpx_im,&&bsr,	&&ldx_im, &&stx_im,
	&&suba_di,&&cmpa_di,&&sbca_di,&&subd_di,&&anda_di,&&bita_di,&&lda_di, &&sta_di, 	/* 90 */
	&&eora_di,&&adca_di,&&ora_di, &&adda_di,&&cmpx_di,&&jsr_di, &&ldx_di, &&stx_di,
	&&suba_ix,&&cmpa_ix,&&sbca_ix,&&subd_ix,&&anda_ix,&&bita_ix,&&lda_ix, &&sta_ix, 	/* &&a0 */
	&&eora_ix,&&adca_ix,&&ora_ix, &&adda_ix,&&cmpx_ix,&&jsr_ix, &&ldx_ix, &&stx_ix,
	&&suba_ex,&&cmpa_ex,&&sbca_ex,&&subd_ex,&&anda_ex,&&bita_ex,&&lda_ex, &&sta_ex, 	/* b0 */
	&&eora_ex,&&adca_ex,&&ora_ex, &&adda_ex,&&cmpx_ex,&&jsr_ex, &&ldx_ex, &&stx_ex,
	&&subb_im,&&cmpb_im,&&sbcb_im,&&addd_im,&&andb_im,&&bitb_im,&&ldb_im, &&stb_im, 	/* &&c0 */
	&&eorb_im,&&adcb_im,&&orb_im, &&addb_im,&&ldd_im, &&std_im, &&ldu_im, &&stu_im,
	&&subb_di,&&cmpb_di,&&sbcb_di,&&addd_di,&&andb_di,&&bitb_di,&&ldb_di, &&stb_di, 	/* &&d0 */
	&&eorb_di,&&adcb_di,&&orb_di, &&addb_di,&&ldd_di, &&std_di, &&ldu_di, &&stu_di,
	&&subb_ix,&&cmpb_ix,&&sbcb_ix,&&addd_ix,&&andb_ix,&&bitb_ix,&&ldb_ix, &&stb_ix, 	/* &&e0 */
	&&eorb_ix,&&adcb_ix,&&orb_ix, &&addb_ix,&&ldd_ix, &&std_ix, &&ldu_ix, &&stu_ix,
	&&subb_ex,&&cmpb_ex,&&sbcb_ex,&&addd_ex,&&andb_ex,&&bitb_ex,&&ldb_ex, &&stb_ex, 	/* f0 */
	&&eorb_ex,&&adcb_ex,&&orb_ex, &&addb_ex,&&ldd_ex, &&std_ex, &&ldu_ex, &&stu_ex
};

static void (*const m6809_pref10[0x100])(void) = {
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, 	/* 00 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,		/* 10 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&lbrn, &&lbhi, &&lbls, &&lbcc, &&lbcs, &&lbne, &&lbeq,		/* 20 */
	&&lbvc, &&lbvs, &&lbpl, &&lbmi, &&lbge, &&lblt, &&lbgt, &&lble,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,		/* 30 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&swi2,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,		/* 40 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,		/* 50 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, 	/* 60 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, 	/* 70 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&cmpd_im, &&illegal, &&illegal, &&illegal, &&illegal, 	/* 80 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&cmpy_im, &&illegal, &&ldy_im, &&sty_im,
	&&illegal, &&illegal, &&illegal, &&cmpd_di, &&illegal, &&illegal, &&illegal, &&illegal, 	/* 90 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&cmpy_di, &&illegal, &&ldy_di, &&sty_di,
	&&illegal, &&illegal, &&illegal, &&cmpd_ix, &&illegal, &&illegal, &&illegal, &&illegal, 	/* a0 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&cmpy_ix, &&illegal, &&ldy_ix, &&sty_ix,
	&&illegal, &&illegal, &&illegal, &&cmpd_ex, &&illegal, &&illegal, &&illegal, &&illegal, 	/* b0 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&cmpy_ex, &&illegal, &&ldy_ex, &&sty_ex,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, 	/* c0 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&lds_im, &&sts_im,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, 	/* d0 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&lds_di, &&sts_di,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, 	/* e0 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&lds_ix, &&sts_ix,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, 	/* f0 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&lds_ex, &&sts_ex
};
static void (*const m6809_pref11[0x100])(void) = {
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, 	/* 00 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,		/* 10 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,		/* 20 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,		/* 30 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&swi3,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,		/* 40 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,		/* 50 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, 	/* 60 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, 	/* 70 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&cmpu_im, &&illegal, &&illegal, &&illegal, &&illegal, 	/* 80 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&cmps_im, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&cmpu_di, &&illegal, &&illegal, &&illegal, &&illegal, 	/* 90 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&cmps_di, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&cmpu_ix, &&illegal, &&illegal, &&illegal, &&illegal, 	/* a0 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&cmps_ix, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&cmpu_ex, &&illegal, &&illegal, &&illegal, &&illegal, 	/* b0 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&cmps_ex, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, 	/* c0 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, 	/* d0 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, 	/* e0 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal,
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, 	/* f0 */
	&&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal, &&illegal
};	
    m6809.m6809_ICount = cycles - m6809.extra_cycles;
	m6809.extra_cycles = 0;

	if (m6809.int_state & (M6809_CWAI | M6809_SYNC))
	{
		m6809.m6809_ICount = 0;
	}
	else
	{
m6809_Exec:			
		pPPC = pPC;
		m6809.ireg = ROP(PCD);
		PC++;
		DISPATCH_MAIN();

/* includes the static function prototypes and the master opcode table */
#include "6809ops.c"		

m6809_Exec_End:
        USE_CYCLES(m6809.extra_cycles);
		m6809.extra_cycles = 0;
    }

    return cycles - m6809.m6809_ICount;   /* NS 970908 */
}

M6809_INLINE void fetch_effective_address( void )
{
static void (*const m6809_fetch[0x100])(void) = {
	&&l00,	&&l00,	&&l00,	&&l00,	&&l00,	&&l00,	&&l00,	&&l00,	&&l00,	&&l00,	&&l00,	&&l00,	&&l00,	&&l00,	&&l00,	&&l00,	&&l10,	&&l10,	&&l10,	&&l10,	&&l10,	&&l10,	&&l10,	&&l10,	&&l10,	&&l10,	&&l10,	&&l10,	&&l10,	&&l10,	&&l10,	&&l10,
	&&l20,	&&l20,	&&l20,	&&l20,	&&l20,	&&l20,	&&l20,	&&l20,	&&l20,	&&l20,	&&l20,	&&l20,	&&l20,	&&l20,	&&l20,	&&l20,	&&l30,	&&l30,	&&l30,	&&l30,	&&l30,	&&l30,	&&l30,	&&l30,	&&l30,	&&l30,	&&l30,	&&l30,	&&l30,	&&l30,	&&l30,	&&l30,
	&&l40,	&&l40,	&&l40,	&&l40,	&&l40,	&&l40,	&&l40,	&&l40,	&&l40,	&&l40,	&&l40,	&&l40,	&&l40,	&&l40,	&&l40,	&&l40,	&&l50,	&&l50,	&&l50,	&&l50,	&&l50,	&&l50,	&&l50,	&&l50,	&&l50,	&&l50,	&&l50,	&&l50,	&&l50,	&&l50,	&&l50,	&&l50,
	&&l60,	&&l60,	&&l60,	&&l60,	&&l60,	&&l60,	&&l60,	&&l60,	&&l60,	&&l60,	&&l60,	&&l60,	&&l60,	&&l60,	&&l60,	&&l60,	&&l70,	&&l70,	&&l70,	&&l70,	&&l70,	&&l70,	&&l70,	&&l70,	&&l70,	&&l70,	&&l70,	&&l70,	&&l70,	&&l70,	&&l70,	&&l70,
	&&l80,	&&l81,	&&l82,	&&l83,	&&l84,	&&l85,	&&l86,	&&illeg,	&&l88,	&&l89,	&&illeg,	&&l8b,	&&l8c,	&&l8d,	&&illeg,	&&l8f,	&&l90,	&&l91,	&&l92,	&&l93,	&&l94,	&&l95,	&&l96,	&&illeg,	&&l98,	&&l99,	&&illeg,	&&l9b,	&&l9c,	&&l9d,	&&illeg,	&&l9f,
	&&la0,	&&la1,	&&la2,	&&la3,	&&la4,	&&la5,	&&la6,	&&illeg,	&&la8,	&&la9,	&&illeg,	&&lab,	&&l8c,	&&l8d,	&&illeg,	&&l8f,	&&lb0,	&&lb1,	&&lb2,	&&lb3,	&&lb4,	&&lb5,	&&lb6,	&&illeg,	&&lb8,	&&lb9,	&&illeg,	&&lbb,	&&l9c,	&&l9d,	&&illeg,	&&l9f,
	&&lc0,	&&lc1,	&&lc2,	&&lc3,	&&lc4,	&&lc5,	&&lc6,	&&illeg,	&&lc8,	&&lc9,	&&illeg,	&&lcb,	&&l8c,	&&l8d,	&&illeg,	&&l8f,	&&ld0,	&&ld1,	&&ld2,	&&ld3,	&&ld4,	&&ld5,	&&ld6,	&&illeg,	&&ld8,	&&ld9,	&&illeg,	&&ldb,	&&l9c,	&&l9d,	&&illeg,	&&l9f,
	&&le0,	&&le1,	&&le2,	&&le3,	&&le4,	&&le5,	&&le6,	&&illeg,	&&le8,	&&le9,	&&illeg,	&&leb,	&&l8c,	&&l8d,	&&illeg,	&&l8f,	&&lf0,	&&lf1,	&&lf2,	&&lf3,	&&lf4,	&&lf5,	&&lf6,	&&illeg,	&&lf8,	&&lf9,	&&illeg,	&&lfb,	&&l9c,	&&l9d,	&&illeg,	&&l9f
};	
	UINT8 postbyte = ROP_ARG(PCD);
	PC++;
	DISPATCH_FETCH();

l00: EA=X+postbyte;										USE_CYCLES(1);   goto end_fetch;
l10: EA=X+postbyte-0x20;								USE_CYCLES(1);   goto end_fetch;
l20: EA=Y+postbyte-0x20;								USE_CYCLES(1);   goto end_fetch;
l30: EA=Y+postbyte-0x40;								USE_CYCLES(1);   goto end_fetch;
l40: EA=U+postbyte-0x40;								USE_CYCLES(1);   goto end_fetch;
l50: EA=U+postbyte-0x60;								USE_CYCLES(1);   goto end_fetch;
l60: EA=S+postbyte-0x60;								USE_CYCLES(1);   goto end_fetch;
l70: EA=S+postbyte-0x80;								USE_CYCLES(1);   goto end_fetch;
l80: EA=X;	X++;										USE_CYCLES(2);   goto end_fetch;
l81: EA=X;	X+=2;										USE_CYCLES(3);   goto end_fetch;
l82: X--; 	EA=X;										USE_CYCLES(2);   goto end_fetch;
l83: X-=2;	EA=X;										USE_CYCLES(3);   goto end_fetch;
l84: EA=X;																 goto end_fetch;
l85: EA=X+SIGNED(B);									USE_CYCLES(1);   goto end_fetch;
l86: EA=X+SIGNED(A);									USE_CYCLES(1);   goto end_fetch;
illeg:EA=0;																 goto end_fetch; /*ILLEGAL*/
l88: IMMBYTE(EA); 	EA=X+SIGNED(EA);					USE_CYCLES(1);   goto end_fetch; /* this is a hack to make Vectrex work. It should be USE_CYCLES(1). Dunno where the cycle was lost :( */
l89: IMMWORD(ea); 	EA+=X;								USE_CYCLES(4);   goto end_fetch;
l8b: EA=X+D;											USE_CYCLES(4);   goto end_fetch;
l8c: IMMBYTE(EA); 	EA=PC+SIGNED(EA);					USE_CYCLES(1);   goto end_fetch;
l8d: IMMWORD(ea); 	EA+=PC; 							USE_CYCLES(5);   goto end_fetch;
l8f: IMMWORD(ea); 										USE_CYCLES(5);   goto end_fetch;
l90: EA=X;	X++;						EAD=RM16(EAD);	USE_CYCLES(5);   goto end_fetch; /* Indirect ,R+ not in my specs */
l91: EA=X;	X+=2;						EAD=RM16(EAD);	USE_CYCLES(6);   goto end_fetch;
l92: X--; 	EA=X;						EAD=RM16(EAD);	USE_CYCLES(5);   goto end_fetch;
l93: X-=2;	EA=X;						EAD=RM16(EAD);	USE_CYCLES(6);   goto end_fetch;
l94: EA=X;								EAD=RM16(EAD);	USE_CYCLES(3);   goto end_fetch;
l95: EA=X+SIGNED(B);					EAD=RM16(EAD);	USE_CYCLES(4);   goto end_fetch;
l96: EA=X+SIGNED(A);					EAD=RM16(EAD);	USE_CYCLES(4);   goto end_fetch;
l98: IMMBYTE(EA); 	EA=X+SIGNED(EA);	EAD=RM16(EAD);	USE_CYCLES(4);   goto end_fetch;
l99: IMMWORD(ea); 	EA+=X;				EAD=RM16(EAD);	USE_CYCLES(7);   goto end_fetch;
l9b: EA=X+D;							EAD=RM16(EAD);	USE_CYCLES(7);   goto end_fetch;
l9c: IMMBYTE(EA); 	EA=PC+SIGNED(EA);	EAD=RM16(EAD);	USE_CYCLES(4);   goto end_fetch;
l9d: IMMWORD(ea); 	EA+=PC; 			EAD=RM16(EAD);	USE_CYCLES(8);   goto end_fetch;
l9f: IMMWORD(ea); 						EAD=RM16(EAD);	USE_CYCLES(8);   goto end_fetch;
la0: EA=Y;	Y++;										USE_CYCLES(2);   goto end_fetch;
la1: EA=Y;	Y+=2;										USE_CYCLES(3);   goto end_fetch;
la2: Y--; 	EA=Y;										USE_CYCLES(2);   goto end_fetch;
la3: Y-=2;	EA=Y;										USE_CYCLES(3);   goto end_fetch;
la4: EA=Y;																 goto end_fetch;
la5: EA=Y+SIGNED(B);									USE_CYCLES(1);   goto end_fetch;
la6: EA=Y+SIGNED(A);									USE_CYCLES(1);   goto end_fetch;
la8: IMMBYTE(EA); 	EA=Y+SIGNED(EA);					USE_CYCLES(1);   goto end_fetch;
la9: IMMWORD(ea); 	EA+=Y;								USE_CYCLES(4);   goto end_fetch;
lab: EA=Y+D;											USE_CYCLES(4);   goto end_fetch;
lb0: EA=Y;	Y++;						EAD=RM16(EAD);	USE_CYCLES(5);   goto end_fetch;
lb1: EA=Y;	Y+=2;						EAD=RM16(EAD);	USE_CYCLES(6);   goto end_fetch;
lb2: Y--; 	EA=Y;						EAD=RM16(EAD);	USE_CYCLES(5);   goto end_fetch;
lb3: Y-=2;	EA=Y;						EAD=RM16(EAD);	USE_CYCLES(6);   goto end_fetch;
lb4: EA=Y;								EAD=RM16(EAD);	USE_CYCLES(3);   goto end_fetch;
lb5: EA=Y+SIGNED(B);					EAD=RM16(EAD);	USE_CYCLES(4);   goto end_fetch;
lb6: EA=Y+SIGNED(A);					EAD=RM16(EAD);	USE_CYCLES(4);   goto end_fetch;
lb8: IMMBYTE(EA); 	EA=Y+SIGNED(EA);	EAD=RM16(EAD);	USE_CYCLES(4);   goto end_fetch;
lb9: IMMWORD(ea); 	EA+=Y;				EAD=RM16(EAD);	USE_CYCLES(7);   goto end_fetch;
lbb: EA=Y+D;							EAD=RM16(EAD);	USE_CYCLES(7);   goto end_fetch;
lc0: EA=U;			U++;								USE_CYCLES(2);   goto end_fetch;
lc1: EA=U;			U+=2;								USE_CYCLES(3);   goto end_fetch;
lc2: U--; 			EA=U;								USE_CYCLES(2);   goto end_fetch;
lc3: U-=2;			EA=U;								USE_CYCLES(3);   goto end_fetch;
lc4: EA=U;																 goto end_fetch;
lc5: EA=U+SIGNED(B);									USE_CYCLES(1);   goto end_fetch;
lc6: EA=U+SIGNED(A);									USE_CYCLES(1);   goto end_fetch;
lc8: IMMBYTE(EA); 	EA=U+SIGNED(EA);					USE_CYCLES(1);   goto end_fetch;
lc9: IMMWORD(ea); 	EA+=U;								USE_CYCLES(4);   goto end_fetch;
lcb: EA=U+D;											USE_CYCLES(4);   goto end_fetch;
ld0: EA=U;	U++;						EAD=RM16(EAD);	USE_CYCLES(5);   goto end_fetch;
ld1: EA=U;	U+=2;						EAD=RM16(EAD);	USE_CYCLES(6);   goto end_fetch;
ld2: U--; 	EA=U;						EAD=RM16(EAD);	USE_CYCLES(5);   goto end_fetch;
ld3: U-=2;	EA=U;						EAD=RM16(EAD);	USE_CYCLES(6);   goto end_fetch;
ld4: EA=U;								EAD=RM16(EAD);	USE_CYCLES(3);   goto end_fetch;
ld5: EA=U+SIGNED(B);					EAD=RM16(EAD);	USE_CYCLES(4);   goto end_fetch;
ld6: EA=U+SIGNED(A);					EAD=RM16(EAD);	USE_CYCLES(4);   goto end_fetch;
ld8: IMMBYTE(EA); 	EA=U+SIGNED(EA);	EAD=RM16(EAD);	USE_CYCLES(4);   goto end_fetch;
ld9: IMMWORD(ea); 	EA+=U;				EAD=RM16(EAD);	USE_CYCLES(7);   goto end_fetch;
ldb: EA=U+D;							EAD=RM16(EAD);	USE_CYCLES(7);   goto end_fetch;
le0: EA=S;	S++;										USE_CYCLES(2);   goto end_fetch;
le1: EA=S;	S+=2;										USE_CYCLES(3);   goto end_fetch;
le2: S--; 	EA=S;										USE_CYCLES(2);   goto end_fetch;
le3: S-=2;	EA=S;										USE_CYCLES(3);   goto end_fetch;
le4: EA=S;																 goto end_fetch;
le5: EA=S+SIGNED(B);									USE_CYCLES(1);   goto end_fetch;
le6: EA=S+SIGNED(A);									USE_CYCLES(1);   goto end_fetch;
le8: IMMBYTE(EA); 	EA=S+SIGNED(EA);					USE_CYCLES(1);   goto end_fetch;
le9: IMMWORD(ea); 	EA+=S;								USE_CYCLES(4);   goto end_fetch;
leb: EA=S+D;											USE_CYCLES(4);   goto end_fetch;
lf0: EA=S;	S++;						EAD=RM16(EAD);	USE_CYCLES(5);   goto end_fetch;
lf1: EA=S;	S+=2;						EAD=RM16(EAD);	USE_CYCLES(6);   goto end_fetch;
lf2: S--; 	EA=S;						EAD=RM16(EAD);	USE_CYCLES(5);   goto end_fetch;
lf3: S-=2;	EA=S;						EAD=RM16(EAD);	USE_CYCLES(6);   goto end_fetch;
lf4: EA=S;								EAD=RM16(EAD);	USE_CYCLES(3);   goto end_fetch;
lf5: EA=S+SIGNED(B);					EAD=RM16(EAD);	USE_CYCLES(4);   goto end_fetch;
lf6: EA=S+SIGNED(A);					EAD=RM16(EAD);	USE_CYCLES(4);   goto end_fetch;
lf8: IMMBYTE(EA); 	EA=S+SIGNED(EA);	EAD=RM16(EAD);	USE_CYCLES(4);   goto end_fetch;
lf9: IMMWORD(ea); 	EA+=S;				EAD=RM16(EAD);	USE_CYCLES(7);   goto end_fetch;
lfb: EA=S+D;							EAD=RM16(EAD);	USE_CYCLES(7);   goto end_fetch;
end_fetch:
;	
}

