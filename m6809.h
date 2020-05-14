/*** m6809: Portable 6809 emulator ******************************************/

//#pragma once

#ifndef __M6809_H__
#define __M6809_H__

//#include "cpuintrf.h"

/* 6809 Registers */
typedef struct
{
	PAIR	pc; 		/* Program counter */
	PAIR	ppc;		/* Previous program counter */
	PAIR	d;			/* Accumulator a and b */
	PAIR	dp; 		/* Direct Page register (page in MSB) */
	PAIR	u, s;		/* Stack pointers */
	PAIR	x, y;		/* Index registers */
    UINT8   cc;
	UINT8	ireg;		/* First opcode */
	UINT8	irq_state[2];
    int     extra_cycles; /* cycles used up by interrupts */
    int     (*irq_callback)(int irqline);
    UINT8   int_state;  /* SYNC and CWAI flags */
    UINT8   nmi_state;
	int m6809_ICount;	
} m6809_Regs;

enum
{
	M6809_PC=1, M6809_S, M6809_CC ,M6809_A, M6809_B, M6809_U, M6809_X, M6809_Y,
	M6809_DP
};

#define M6809_IRQ_LINE	0	/* IRQ line number */
#define M6809_FIRQ_LINE 1   /* FIRQ line number */

//void m6809_get_info(UINT32 state, cpuinfo *info);
//void m6809e_get_info(UINT32 state, cpuinfo *info);

void m6809_init(int (*irqcallback)(int));
void m6809_reset(void);
int m6809_execute(int cycles);
void m6809_set_irq_line(int irqline, int state);
void m6809_get_context(void *dst);
void m6809_set_context(void *src);

//unsigned char M6809ReadByte(unsigned int Address);
//unsigned short M6809ReadWord(unsigned int Address);
//void M6809WriteByte(unsigned short Address, unsigned char Data);
//void M6809WriteWord(unsigned short Address, unsigned short Data);
void M6809WriteWord(unsigned int Address, unsigned char Data, unsigned char Data2);
unsigned char M6809ReadOp(unsigned short Address);
unsigned char M6809ReadOpArg(unsigned short Address);
extern unsigned char *Fetch[0x100];
extern unsigned char *Read[0x100];
extern unsigned char *Write[0x100];
//extern	unsigned short *ReadW[0x100];
//extern	unsigned short *FetchW[0x100];

inline void M6809WriteByte(unsigned short Address, unsigned char Data)
{
	// check mem map
	unsigned char * pr = Write[(Address >> 8)];
	if (pr != NULL) {
		pr[Address & 0xff] = Data;
		return;
	}
	DrvGngM6809WriteByte(Address, Data);
}

inline unsigned char M6809ReadByte(unsigned int Address)
{
	// check mem map
	unsigned char * pr = Read[(Address >> 8)];
	if (pr != NULL) {
		return pr[Address&0xff];
	}
	return DrvGngM6809ReadByte(Address);
}

inline void M6809WriteWord(unsigned int Address, unsigned char Data, unsigned char Data2)
{
	// check mem map
	unsigned char * pr = (unsigned char *)Write[(Address >> 8)];
	
	if (pr != NULL) 
	{
		pr[(Address)&0xff] = Data;		
		pr[(Address+1)&0xff] = Data2;		
	}
	else
	{
		DrvGngM6809WriteByte(Address, Data);
		DrvGngM6809WriteByte(Address+1, Data2);
	}
}

/****************************************************************************/
/* Read a byte from given memory location                                   */
/****************************************************************************/
//#define M6809_RDMEM(Addr) ((unsigned)M6809ReadByte(Addr))
#define M6809_RDMEM(Addr) ((unsigned)M6809ReadByte(Addr))
//#define M6809_RDMEM16(Addr) ((unsigned)M6809ReadWord(Addr))
#define M6809_RDMEM16(Addr) ((Read[(Addr)>>8][(Addr&0xFF)] <<8)| (Read[(Addr)>>8][(Addr&0xff)+1]))

/****************************************************************************/
/* Write a byte to given memory location                                    */
/****************************************************************************/
#define M6809_WRMEM(Addr,Value)   (M6809WriteByte(Addr,Value))
#define M6809_WRMEM16(Addr,Value,Value2) (M6809WriteWord(Addr,Value,Value2))

/****************************************************************************/
/* Z80_RDOP() is identical to Z80_RDMEM() except it is used for reading     */
/* opcodes. In case of system with memory mapped I/O, this function can be  */
/* used to greatly speed up emulation                                       */
/****************************************************************************/
#define M6809_RDOP(Addr) Fetch[(Addr)>>8][(Addr)&0xFF] //((unsigned)M6809ReadOp(Addr))

/****************************************************************************/
/* Z80_RDOP_ARG() is identical to Z80_RDOP() except it is used for reading  */
/* opcode arguments. This difference can be used to support systems that    */
/* use different encoding mechanisms for opcodes and opcode arguments       */
/****************************************************************************/
#define M6809_RDOP_ARG(Addr) Read[(Addr)>>8][(Addr)&0xFF] //((unsigned)M6809ReadOpArg(Addr))
//#define M6809_RDOP_ARG16(Addr) ({unsigned char * pr = Read[(Addr >> 8)]; \
//Addr&=0xFF; \
//((pr[(Addr)] <<8)| (pr[(Addr)+1]));})
//((Read[(Addr)>>8][(Addr&0xFF)] <<8)| (Read[(Addr)>>8][(Addr&0xff)+1]))//((unsigned)M6809ReadOpArg16(Addr))

//#define M6809_RDOP_ARG16(Addr) ReadW[(Addr)>>8][(Addr&0xFF)]
#define M6809_RDOP_ARG16(Addr) ((Read[(Addr)>>8][(Addr&0xFF)] <<8)| (Read[(Addr)>>8][(Addr&0xff)+1]))


#ifndef FALSE
#    define FALSE 0
#endif
#ifndef TRUE
#    define TRUE (!FALSE)
#endif



#endif /* __M6809_H__ */
