//#pragma GCC optimize ("O2")
/*****************************************************************************
 *
 *	 m6502ops.h
 *	 Addressing mode and opcode macros for 6502,65c02,6510 CPUs
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
typedef unsigned char MHELE;
typedef unsigned char byte;
typedef unsigned short word;
typedef unsigned int dword;
typedef void (*write_func)(unsigned short a, byte d);
typedef byte (*read_func)(unsigned short a);

static unsigned char *Read[0x100];
static unsigned char *Write[0x100];
static write_func wf[0x100];
static read_func rf[0x100];
extern unsigned char *Drv6502RAM;
extern unsigned char *Drv6502ROM;

#if FAST_MEMORY
#define ABITS1_16		12
#define ABITS2_16		4
#define ABITS_MIN_16	0			/* minimum memory block is 1 byte */
#endif

/* 6502 flags */
#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_R 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80

/* some shortcuts for improved readability */
#define A	m6502.a
#define X	m6502.x
#define Y	m6502.y
#define P	m6502.p
#define S	m6502.sp.B.l
#define SPD m6502.sp.D

#if LAZY_FLAGS

#define NZ  m6502.nz

#define SET_NZ(n)   NZ = (((n) & FLAG_N) << 8) | (n)
#define SET_Z(n)	NZ = (NZ & 0x100) | (n)

#else

#define SET_NZ(n)				\
	P = (P & ~(FLAG_N | FLAG_Z)) |  ((n) & FLAG_N) | (((n) == 0) ? FLAG_Z : 0)

#define SET_Z(n)				\
    P = (P & ~FLAG_Z) | ((n) == 0) ? FLAG_Z : 0)

#endif

#define EAL m6502.ea.B.l
#define EAH m6502.ea.B.h
#define EAW m6502.ea.W.l
#define EAD m6502.ea.D

#define ZPL m6502.zp.B.l
#define ZPH m6502.zp.B.h
#define ZPW m6502.zp.W.l
#define ZPD m6502.zp.D

#define PCL m6502.pc.B.l
#define PCH m6502.pc.B.h
#define PCW m6502.pc.W.l
#define PCD m6502.pc.D

//#define change_pc16(pc) {if(cur_mrhard[(pc)>>(ABITS2_16+ABITS_MIN_16)]!=ophw)cpu_setOPbase16(pc);}
#define change_pc16(pc)	PCD = (pc)


#if FAST_MEMORY
//extern  unsigned char   *Write;
//extern	unsigned char	*Read;
//extern	byte	*RAM;
#endif

#define RSTF(a)  P &= ~(a)
#define SETF(a)  P |= (a)
#define TEST(a)  RSTF( FLAG_N | FLAG_Z ); SETF( M6502_byTestTable[ a ] )

/***************************************************************
 *  RDOP    read an opcode
 ***************************************************************/
//#define RDOP() M6502ReadOp(PCW++)
#define RDOP() RDMEM(PCW++)
#define RDOPD() RDMEMD(PCW++)

/***************************************************************
 *  RDOPARG read an opcode argument
 ***************************************************************/
#define RDOPARG() RDOP()
#define RDOPARGD() RDOPD()
#define RDOPARG16() M6502ReadWord(PCD);PCD+=2
///({unsigned char * pr = Read[(addr>>8)];pr[(addr+1)&0xFF]<<8 | pr[(addr&0xFF)]; })

/***************************************************************
 *  RDMEM   read memory
 ***************************************************************/
#if FAST_MEMORY
#define RDMEM(addr)                                             \
	((Read[(addr) >> 8]) ?		\
		M6502ReadByte(addr) : Read[(addr) >> 8][addr&0xff])
#define RDMEMD(addr) Read[(addr) >> 8][addr&0xff]
#define RDMEM16D(addr) ({unsigned char * pr = Read[(addr>>8)];pr[(addr+1)&0xFF]<<8 | pr[(addr&0xFF)]; })

#else
// inline faster !!!!	
static inline unsigned char M6502ReadByte(unsigned short Address)
{
	// check mem map
	unsigned char * pr = Read[(Address >> 8)];
	return (pr != NULL) ? pr[Address&0xff]:rf[Address>>8](Address);
}

static inline unsigned short M6502ReadWord(unsigned short Address)
{
	unsigned char * pr = Read[(Address >> 8)];
	read_func prf = rf[Address>>8];
	return (pr != NULL) ? pr[Address&0xff]|pr[(Address&0xff)+1]<<8:prf(Address)|prf(Address+1)<<8;	
}

#define RDMEM(addr) M6502ReadByte(addr)
#define RDMEMD(addr) Read[(addr) >> 8][addr&0xff]
//#define RDMEMD(addr) (addr<0x1000)?Drv6502RAM[addr]:Read[(addr) >> 8][addr&0xff]
#define RDMEM16D(addr) ({unsigned char * pr = Read[(addr>>8)];pr[(addr+1)&0xFF]<<8 | pr[(addr&0xFF)]; })
#endif

/***************************************************************
 *  WRMEM   write memory
 ***************************************************************/
#if FAST_MEMORY
#define WRMEM(addr,data)                                        \
	if (Write[(addr) >> (ABITS2_16 + ABITS_MIN_16)])		\
		M6502WriteByte(addr,data);								\
	else														\
		Write[(addr) >> (ABITS2_16 + ABITS_MIN_16)][addr&0xff] = data		
#define WRMEMD(addr,data) Write[(addr) >> 8][addr&0xff]=data
#define WRMEM16D(addr,data,data2) Write[(addr) >> 8][addr&0xff]=data;Write[(addr) >> 8][(addr&0xff)+1]=data2

#else
	
static inline void M6502WriteByte(unsigned short Address, unsigned char Data)
{
	// check mem map
	unsigned char * pr = Write[(Address >> 8)];
	if (pr != NULL) {
		pr[Address & 0xff] = Data;
		return;
	}
	wf[Address>>8](Address, Data);
}

#define WRMEM(addr,data) M6502WriteByte(addr,data)
#define WRMEMD(addr,data) Write[(addr) >> 8][addr&0xff]=data
//#define WRMEM16D(addr,data,data2) Write[(addr) >> 8][addr&0xff]=data;Write[(addr) >> 8][(addr&0xff)+1]=data2
#define WRMEM16D(addr,data,data2) Write[(addr) >> 8][addr&0xff]=data;Write[(addr) >> 8][(addr&0xff)+1]=data2
#endif

/***************************************************************
 *	BRA  branch relative
 *	extra cycle if page boundary is crossed
 ***************************************************************/
#define BRA(cond)                                               \
	if (cond)													\
	{															\
		tmp = RDOPARG();										\
		EAD = PCD + (signed char)tmp;							\
		M6502_ICount -= (PCH == EAH) ? 3 : 4;					\
		PCD = EAD;												\
		change_pc16(PCD);										\
	}															\
	else														\
	{															\
		PCW++;													\
		M6502_ICount -= 2;										\
	}

/***************************************************************
 *
 * Helper macros to build the effective address
 *
 ***************************************************************/

/***************************************************************
 *  EA = zero page address
 ***************************************************************/
#define EA_ZPG													\
	ZPL = RDOPARG();											\
	EAD = ZPD

/***************************************************************
 *  EA = zero page address + X
 ***************************************************************/
#define EA_ZPX													\
	ZPL = RDOPARG() + X;										\
	EAD = ZPD

/***************************************************************
 *  EA = zero page address + Y
 ***************************************************************/
#define EA_ZPY													\
	ZPL = RDOPARG() + Y;										\
	EAD = ZPD

/***************************************************************
 *  EA = absolute address
 ***************************************************************/
#define EA_ABS	EAD = RDOPARG16()

/***************************************************************
 *  EA = absolute address + X
 ***************************************************************/
#define EA_ABX													\
    EA_ABS;                                                     \
    EAD += X

/***************************************************************
 *	EA = absolute address + Y
 ***************************************************************/
#define EA_ABY                                                  \
	EA_ABS; 													\
	EAD += Y

/***************************************************************
 *	EA = zero page indirect (65c02 pre indexed w/o X)
 ***************************************************************/
#define EA_ZPI													\
	ZPL = RDOPARGD();											\
	EAD = RDMEM16D(ZPD);											\
	ZPL++

/***************************************************************
 *  EA = zero page + X indirect (pre indexed)
 ***************************************************************/
#define EA_IDX													\
	ZPL = RDOPARG() + X;										\
	EAD = RDMEM16D(ZPD);											\
	ZPL++
	
/***************************************************************
 *  EA = zero page indirect + Y (post indexed)
 *	subtract 1 cycle if page boundary is crossed
 ***************************************************************/
#define EA_IDY													\
	ZPL = RDOPARG();											\
	EAD = RDMEM16D(ZPD);											\
	ZPL++;														\
    if (EAL + Y > 0xff)                                         \
		M6502_ICount--; 										\
	EAD += Y
	
/***************************************************************
 *	EA = indirect (only used by JMP)
 ***************************************************************/
#define EA_IND													\
	EA_ABS; 													\
	tmp = RDMEMD(EAD);											\
	EAL++;	/* booby trap: stay in same page! ;-) */			\
	EAH = RDMEMD(EAD);											\
	EAL = tmp

/***************************************************************
 *	EA = indirect plus x (only used by 65c02 JMP)
 ***************************************************************/
#define EA_IAX                                                  \
	EA_IND; 													\
	if (EAL + X > 0xff) /* assumption; probably wrong ? */		\
		M6502_ICount--; 										\
    EAD += X

/* read a value into tmp */
#define RD_IMM	tmp = RDOPARG()
//#define RD_IMM	tmp = RDMEMD(PCW++)
#define RD_ACC	tmp = A
#define RD_ZPG	EA_ZPG; tmp = RDMEMD(EAD)
#define RD_ZPX	EA_ZPX; tmp = RDMEMD(EAD)
#define RD_ZPY	EA_ZPY; tmp = RDMEMD(EAD)
#define RD_ABS	EA_ABS; tmp = RDMEM(EAD)
#define RD_ABX	EA_ABX; tmp = RDMEM(EAD)
#define RD_ABY	EA_ABY; tmp = RDMEM(EAD)
#define RD_ZPI	EA_ZPI; tmp = RDMEMD(EAD)
#define RD_IDX	EA_IDX; tmp = RDMEM(EAD)
#define RD_IDY	EA_IDY; tmp = RDMEM(EAD)

/* write a value from tmp */
#define WR_ZPG	EA_ZPG; WRMEMD(EAD, tmp)
#define WR_ZPX	EA_ZPX; WRMEMD(EAD, tmp)
#define WR_ZPY	EA_ZPY; WRMEMD(EAD, tmp)
#define WR_ABS	EA_ABS; WRMEM(EAD, tmp)
#define WR_ABX	EA_ABX; WRMEM(EAD, tmp)
#define WR_ABY	EA_ABY; WRMEM(EAD, tmp)
#define WR_ZPI	EA_ZPI; WRMEMD(EAD, tmp)
#define WR_IDX	EA_IDX; WRMEM(EAD, tmp)
#define WR_IDY	EA_IDY; WRMEM(EAD, tmp)

/* write back a value from tmp to the last EA */
#define WB_ACC	A = (byte)tmp;
#define WB_EA	WRMEMD(EAD, tmp)

/***************************************************************
 ***************************************************************
 *			Macros to emulate the plain 6502 opcodes
 ***************************************************************
 ***************************************************************/

/***************************************************************
 * compose the real flag register by
 * including N and Z and set any
 * SET and clear any CLR bits also
 ***************************************************************/
#if LAZY_FLAGS

#define COMPOSE_P												\
	P = (P & ~(FLAG_N|FLAG_Z)) | (NZ >> 8) | ((NZ&0xff) ? 0:FLAG_Z)

#else

#define COMPOSE_P

#endif

/***************************************************************
 * push a register onto the stack
 ***************************************************************/
#define PUSH(Rg) WRMEMD(SPD, Rg); S--
#define PUSH16(Rg,Rg2) WRMEM16D(SPD, Rg,Rg2); S-=2

/***************************************************************
 * pull a register from the stack
 ***************************************************************/
#define PULL(Rg) S++; Rg = RDMEMD(SPD)
#define PULL16(Rg) S+=2; Rg = RDMEM16D((SPD-1))

/* 6502 ********************************************************
 *	ADC Add with carry
 ***************************************************************/
/* 
#define ADC 													\
	if (P & FLAG_D)												\
	{															\
	int c = (P & FLAG_C);											\
	int lo = (A & 0x0f) + (tmp & 0x0f) + c; 					\
	int hi = (A & 0xf0) + (tmp & 0xf0); 						\
		P &= ~(FLAG_V | FLAG_C);										\
		if (lo > 0x09)											\
		{														\
			hi += 0x10; 										\
			lo += 0x06; 										\
		}														\
		if (~(A^tmp) & (A^hi) & FLAG_N)							\
			P |= FLAG_V;											\
		if (hi > 0x90)											\
			hi += 0x60; 										\
		if (hi & 0xff00)										\
			P |= FLAG_C;											\
		A = (lo & 0x0f) + (hi & 0xf0);							\
	}															\
	else														\
	{															\
	int c = (P & FLAG_C);											\
	int sum = A + tmp + c;										\
		P &= ~(FLAG_V | FLAG_C);										\
		if (~(A^tmp) & (A^sum) & FLAG_N)							\
			P |= FLAG_V;											\
		if (sum & 0xff00)										\
			P |= FLAG_C;											\
		A = (byte) sum; 										\
	}															\
	SET_NZ(A)
*/
											\
#define ADC int wD0 = A + tmp + ( P & FLAG_C ); unsigned char byD1 = (unsigned char)wD0; RSTF( FLAG_N | FLAG_V | FLAG_Z | FLAG_C ); SETF( M6502_byTestTable[ byD1 ] | ( ( ~( A ^ tmp ) & ( A ^ byD1 ) & 0x80 ) ? FLAG_V : 0 ) | ( wD0 > 0xff ) ); A = byD1;

/* 6502 ********************************************************
 *	AND Logical and
 ***************************************************************/
/* 
#define AND 													\
	A = (byte)(A & tmp);										\
	SET_NZ(A)
*/
#define AND  A &= (tmp); TEST( A )

/* 6502 ********************************************************
 *	ASL Arithmetic shift left
 ***************************************************************/
/* 
#define ASL 													\
	P = (P & ~FLAG_C) | ((tmp >> 7) & FLAG_C);						\
	tmp = (byte)(tmp << 1); 									\
	SET_NZ(tmp)
*/
	
#define ASL 													\
	RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( M6502_ASL[tmp].byFlag) ; tmp = M6502_ASL[tmp].byValue;

/* 6502 ********************************************************
 *	BCC Branch if carry clear
 ***************************************************************/
#define BCC BRA(!(P & FLAG_C))

/* 6502 ********************************************************
 *	BCS Branch if carry set
 ***************************************************************/
#define BCS BRA(P & FLAG_C)

/* 6502 ********************************************************
 *	BEQ Branch if equal
 ***************************************************************/
#if LAZY_FLAGS
#define BEQ BRA(!(NZ & 0xff))
#else
#define BEQ BRA(P & FLAG_Z)
#endif

/* 6502 ********************************************************
 *	BIT Bit test
 ***************************************************************/
#if LAZY_FLAGS

#define BIT 													\
	P = (P & ~FLAG_V) | (tmp & FLAG_V);								\
	NZ = (tmp & FLAG_N) << 8;										\
	NZ |= tmp & A /* mask to lazy Z */

#else

#define BIT 													\
	P &= ~(FLAG_N|FLAG_V|FLAG_Z);										\
	P |= tmp & (FLAG_N|FLAG_V);										\
	if ((tmp & A) == 0) 										\
		P |= FLAG_Z

#endif

/* 6502 ********************************************************
 *	BMI Branch if minus
 ***************************************************************/
#if LAZY_FLAGS
#define BMI BRA(NZ & 0x8000)
#else
#define BMI BRA(P & FLAG_N)
#endif

/* 6502 ********************************************************
 *	BNE Branch if not equal
 ***************************************************************/
#if LAZY_FLAGS
#define BNE BRA(NZ & 0xff)
#else
#define BNE BRA(!(P & FLAG_Z))
#endif

/* 6502 ********************************************************
 *	BPL Branch if plus
 ***************************************************************/
#if LAZY_FLAGS
#define BPL BRA(!(NZ & 0x8000))
#else
#define BPL BRA(!(P & FLAG_N))
#endif

/* 6502 ********************************************************
 *	BRK Break
 *	increment PC, push PC hi, PC lo, flags (with B bit set),
 *	set I flag, reset D flag and jump via IRQ vector
 ***************************************************************/
#define BRK 													\
	PCW++;														\
	PUSH16(PCH,PCL);													\
	COMPOSE_P;													\
	PUSH(P | FLAG_B);												\
	P = (P | FLAG_I) & ~FLAG_D;										\
	PCD = RDMEM16D(M6502_IRQ_VEC);								\
	change_pc16(PCD)
/* 6502 ********************************************************
 * BVC	Branch if overflow clear
 ***************************************************************/
#define BVC BRA(!(P & FLAG_V))

/* 6502 ********************************************************
 * BVS	Branch if overflow set
 ***************************************************************/
#define BVS BRA(P & FLAG_V)

/* 6502 ********************************************************
 * CLC	Clear carry flag
 ***************************************************************/
#define CLC 													\
	P &= ~FLAG_C

/* 6502 ********************************************************
 * CLD	Clear decimal flag
 ***************************************************************/
#define CLD 													\
	P &= ~FLAG_D

/* 6502 ********************************************************
 * CLI	Clear interrupt flag
 ***************************************************************/
#define CLI 													\
	if (m6502.pending_irq && (P & FLAG_I)) 						\
		m6502.after_cli = 1;									\
	P &= ~FLAG_I

/* 6502 ********************************************************
 * CLV	Clear overflow flag
 ***************************************************************/
#define CLV 													\
	P &= ~FLAG_V

/* 6502 ********************************************************
 *	CMP Compare accumulator
 ***************************************************************/
/* 
#define CMP 													\
	P &= ~FLAG_C;													\
	if (A >= tmp)												\
		P |= FLAG_C;												\
	SET_NZ((byte)(A - tmp))
*/
#define CMP unsigned short wD0 = (unsigned short)A - (tmp); RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( M6502_byTestTable[ wD0 & 0xff ] | ( wD0 < 0x100 ? FLAG_C : 0 ) );
/* 6502 ********************************************************
 *	CPX Compare index X
 ***************************************************************/
/* 
#define CPX 													\
	P &= ~FLAG_C;													\
	if (X >= tmp)												\
		P |= FLAG_C;												\
	SET_NZ((byte)(X - tmp))
*/
#define CPX unsigned short wD0 = (unsigned short)X - (tmp); RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( M6502_byTestTable[ wD0 & 0xff ] | ( wD0 < 0x100 ? FLAG_C : 0 ) );

/* 6502 ********************************************************
 *	CPY Compare index Y
 ***************************************************************/
/* 
#define CPY 													\
	P &= ~FLAG_C;													\
	if (Y >= tmp)												\
		P |= FLAG_C;												\
	SET_NZ((byte)(Y - tmp))
*/
#define CPY unsigned short wD0 = (unsigned short)Y - (tmp); RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( M6502_byTestTable[ wD0 & 0xff ] | ( wD0 < 0x100 ? FLAG_C : 0 ) );


/* 6502 ********************************************************
 *	DEC Decrement memory
 ***************************************************************/
#define DEC 													\
	tmp = (byte)--tmp;											\
	TEST(tmp)

/* 6502 ********************************************************
 *	DEX Decrement index X
 ***************************************************************/
#define DEX 													\
	X = (byte)--X;												\
	TEST(X)

/* 6502 ********************************************************
 *	DEY Decrement index Y
 ***************************************************************/
#define DEY 													\
	Y = (byte)--Y;												\
	TEST(Y)

/* 6502 ********************************************************
 *	EOR Logical exclusive or
 ***************************************************************/
/*
#define EOR 													\
	A = (byte)(A ^ tmp);										\
	SET_NZ(A)
*/	
#define EOR  A ^= (tmp); TEST( A )	

/* 6502 ********************************************************
 *	INC Increment memory
 ***************************************************************/
#define INC 													\
	tmp = (byte)++tmp;											\
	TEST(tmp)

/* 6502 ********************************************************
 *	INX Increment index X
 ***************************************************************/
#define INX 													\
	X = (byte)++X;												\
	TEST(X)

/* 6502 ********************************************************
 *	INY Increment index Y
 ***************************************************************/
#define INY 													\
	Y = (byte)++Y;												\
	TEST(Y)

/* 6502 ********************************************************
 *	JMP Jump to address
 *	set PC to the effective address
 ***************************************************************/
#define JMP 													\
	PCD = EAD;													\
	change_pc16(PCD)

/* 6502 ********************************************************
 *	JSR Jump to subroutine
 *	decrement PC (sic!) push PC hi, push PC lo and set
 *	PC to the effective address
 ***************************************************************/
#define JSR 													\
	EAL = RDOPARG();											\
	PUSH(PCH);													\
	PUSH(PCL);													\
	EAH = RDOPARG();											\
	PCD = EAD;													\
	change_pc16(PCD)

/* 6502 ********************************************************
 *	LDA Load accumulator
 ***************************************************************/
#define LDA 													\
	A = (byte)tmp;												\
	TEST( A )

/* 6502 ********************************************************
 *	LDX Load index X
 ***************************************************************/
#define LDX 													\
	X = (byte)tmp;												\
	TEST( X )

/* 6502 ********************************************************
 *	LDY Load index Y
 ***************************************************************/
#define LDY 													\
	Y = (byte)tmp;												\
	TEST( Y )

/* 6502 ********************************************************
 *	LSR Logic shift right
 *	0 -> [7][6][5][4][3][2][1][0] -> C
 ***************************************************************/
/*
#define LSR 													\
	P = (P & ~FLAG_C) | (tmp & FLAG_C);								\
	tmp = (byte)tmp >> 1;										\
	SET_NZ(tmp)
*/
#define LSR RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( M6502_LSR[tmp].byFlag) ; tmp = M6502_LSR[tmp].byValue;	


/* 6502 ********************************************************
 *	NOP No operation
 ***************************************************************/
#define NOP

/* 6502 ********************************************************
 *	ORA Logical inclusive or
 ***************************************************************/
/*
#define ORA 													\
	A = (byte)(A | tmp);										\
	SET_NZ(A)
*/

#define ORA  A |= (tmp); TEST( A )

/* 6502 ********************************************************
 *	PHA Push accumulator
 ***************************************************************/
#define PHA 													\
	PUSH(A)

/* 6502 ********************************************************
 *	PHP Push processor status (flags)
 ***************************************************************/
#define PHP 													\
	COMPOSE_P;													\
	PUSH(P)

/* 6502 ********************************************************
 *	PLA Pull accumulator
 ***************************************************************/
#define PLA 													\
	PULL(A);													\
	TEST(A)


/* 6502 ********************************************************
 *	PLP Pull processor status (flags)
 ***************************************************************/
#if LAZY_FLAGS

#define PLPX 													\
	if (P & FLAG_I)												\
	{															\
		PULL(P);												\
		if (m6502.pending_irq && !(P & FLAG_I))					\
			m6502.after_cli = 1;								\
	}															\
	else														\
	{															\
		PULL(P);												\
	}															\
	P |= FLAG_R;													\
	NZ = ((P & FLAG_N) << 8) | ((P & FLAG_Z) ^ FLAG_Z)

#else

#define PLP 													\
	if (P & FLAG_I)												\
	{															\
		PULL(P);												\
		if (m6502.pending_irq && !(P & FLAG_I))					\
			m6502.after_cli = 1;								\
	}															\
	else														\
	{															\
		PULL(P);												\
	}															\
	P |= FLAG_R

#endif
//#define PLP POP( F ); SETF( FLAG_R ); CLK( 4 );  

/* 6502 ********************************************************
 * ROL	Rotate left
 *	new C <- [7][6][5][4][3][2][1][0] <- C
 ***************************************************************/
/*
#define ROL 													\
	tmp = (tmp << 1) | (P & FLAG_C);								\
	P = (P & ~FLAG_C) | ((tmp >> 8) & FLAG_C);						\
	tmp = (byte)tmp;											\
	SET_NZ(tmp)
*/	
#define ROL int byD0 = P & FLAG_C; RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( M6502_ROL[ byD0 ][ tmp ].byFlag ); tmp = M6502_ROL[ byD0 ][ tmp ].byValue 
	
	

/* 6502 ********************************************************
 * ROR	Rotate right
 *	C -> [7][6][5][4][3][2][1][0] -> new C
 ***************************************************************/
/* 
#define ROR 													\
	tmp |= (P & FLAG_C) << 8;										\
	P = (P & ~FLAG_C) | (tmp & FLAG_C);								\
	tmp = (byte)(tmp >> 1); 									\
	SET_NZ(tmp)
*/

#define ROR int byD0 = P & FLAG_C; RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( M6502_ROR[ byD0 ][ tmp ].byFlag ); tmp = M6502_ROR[ byD0 ][ tmp ].byValue 

/* 6502 ********************************************************
 * RTI	Return from interrupt
 * pull flags, pull PC lo, pull PC hi and increment PC
 *	PCW++;
 ***************************************************************/
#if LAZY_FLAGS

#define RTI 													\
	PULL(P);													\
	PULL16(PCD);													\
    P |= FLAG_R;                                                   \
	NZ = ((P & FLAG_N) << 8) | ((P & FLAG_Z) ^ FLAG_Z);					\
	if (m6502.pending_irq && !(P & FLAG_I))						\
		m6502.after_cli = 1;									\
	change_pc16(PCD)

#else

#define RTI 													\
	PULL(P);													\
	PULL16(PCD);													\
    P |= FLAG_R;                                                   \
    if (m6502.pending_irq && !(P & FLAG_I))                        \
		m6502.after_cli = 1;									\
    change_pc16(PCD)

#endif

/* 6502 ********************************************************
 *	RTS Return from subroutine
 *	pull PC lo, PC hi and increment PC
 ***************************************************************/
#define RTS 													\
	PULL16(PCD);													\
	PCW++;														\
	change_pc16(PCD)

/* 6502 ********************************************************
 *	SBC Subtract with carry
 ***************************************************************/
/* 
#define SBC 													\
	if (P & FLAG_D)												\
	{															\
	int c = (P & FLAG_C) ^ FLAG_C;									\
	int sum = A - tmp - c;										\
	int lo = (A & 0x0f) - (tmp & 0x0f) - c; 					\
	int hi = (A & 0xf0) - (tmp & 0xf0); 						\
		P &= ~(FLAG_V | FLAG_C);										\
		if ((A^tmp) & (A^sum) & FLAG_N)							\
			P |= FLAG_V;											\
		if (lo & 0xf0)											\
			lo -= 6;											\
		if (lo & 0x80)											\
			hi -= 0x10; 										\
		if (hi & 0x0f00)										\
			hi -= 0x60; 										\
		if ((sum & 0xff00) == 0)								\
			P |= FLAG_C;											\
		A = (lo & 0x0f) + (hi & 0xf0);							\
	}															\
	else														\
	{															\
	int c = (P & FLAG_C) ^ FLAG_C;									\
	int sum = A - tmp - c;										\
		P &= ~(FLAG_V | FLAG_C);										\
		if ((A^tmp) & (A^sum) & FLAG_N)							\
			P |= FLAG_V;											\
		if ((sum & 0xff00) == 0)								\
			P |= FLAG_C;											\
		A = (byte) sum; 										\
	}															\
	SET_NZ(A)
*/

#define SBC int wD0 = A - tmp - ( ~P & FLAG_C ); unsigned char byD1 = (unsigned char)wD0; RSTF( FLAG_N | FLAG_V | FLAG_Z | FLAG_C ); SETF( M6502_byTestTable[ byD1 ] | ( ( ( A ^ tmp ) & ( A ^ byD1 ) & 0x80 ) ? FLAG_V : 0 ) | ( wD0 < 0x100 ) ); A = byD1;

/* 6502 ********************************************************
 *	SEC Set carry flag
 ***************************************************************/
#define SEC 													\
	P |= FLAG_C

/* 6502 ********************************************************
 *	SED Set decimal flag
 ***************************************************************/
#define SED 													\
	P |= FLAG_D

/* 6502 ********************************************************
 *	SEI Set interrupt flag
 ***************************************************************/
#define SEI 													\
	P |= FLAG_I

/* 6502 ********************************************************
 * STA	Store accumulator
 ***************************************************************/
#define STA 													\
	tmp = A

/* 6502 ********************************************************
 * STX	Store index X
 ***************************************************************/
#define STX 													\
	tmp = X

/* 6502 ********************************************************
 * STY	Store index Y
 ***************************************************************/
#define STY 													\
	tmp = Y

/* 6502 ********************************************************
 * TAX	Transfer accumulator to index X
 ***************************************************************/
#define TAX 													\
	X = A;														\
	TEST(X)

/* 6502 ********************************************************
 * TAY	Transfer accumulator to index Y
 ***************************************************************/
#define TAY 													\
	Y = A;														\
	TEST(Y)

/* 6502 ********************************************************
 * TSX	Transfer stack LSB to index X
 ***************************************************************/
#define TSX 													\
	X = S;														\
	TEST(X)

/* 6502 ********************************************************
 * TXA	Transfer index X to accumulator
 ***************************************************************/
#define TXA 													\
	A = X;														\
	TEST(A)

/* 6502 ********************************************************
 * TXS	Transfer index X to stack LSB
 * no flags changed (sic!)
 ***************************************************************/
#define TXS 													\
	S = X

/* 6502 ********************************************************
 * TYA	Transfer index Y to accumulator
 ***************************************************************/
#define TYA 													\
	A = Y;														\
	TEST(A)

/***************************************************************
 ***************************************************************
 *			Macros to emulate the 65C02 opcodes
 ***************************************************************
 ***************************************************************/

/* 65C02 *******************************************************
 *	BBR Branch if bit is reset
 ***************************************************************/
#define BBR(bit)												\
	BRA(!(tmp & (1<<bit)))

/* 65C02 *******************************************************
 *	BBS Branch if bit is set
 ***************************************************************/
#define BBS(bit)												\
	BRA(tmp & (1<<bit))

/* 65C02 *******************************************************
 *	DEA Decrement accumulator
 ***************************************************************/
#define DEA 													\
	A = (byte)--A;												\
	TEST(A)

/* 65C02 *******************************************************
 *	INA Increment accumulator
 ***************************************************************/
#define INA 													\
	A = (byte)++A;												\
	TEST(A)

/* 65C02 *******************************************************
 *	PHX Push index X
 ***************************************************************/
#define PHX 													\
	PUSH(X)

/* 65C02 *******************************************************
 *	PHY Push index Y
 ***************************************************************/
#define PHY 													\
	PUSH(Y)

/* 65C02 *******************************************************
 *	PLX Pull index X
 ***************************************************************/
#define PLX 													\
	PULL(X)

/* 65C02 *******************************************************
 *	PLY Pull index Y
 ***************************************************************/
#define PLY 													\
	PULL(Y)

/* 65C02 *******************************************************
 *	RMB Reset memory bit
 ***************************************************************/
#define RMB(bit)												\
	tmp &= ~(1<<bit)

/* 65C02 *******************************************************
 *	SMB Set memory bit
 ***************************************************************/
#define SMB(bit)												\
	tmp |= (1<<bit)

/* 65C02 *******************************************************
 * STZ	Store zero
 ***************************************************************/
#define STZ                                                     \
    tmp = 0

/* 65C02 *******************************************************
 * TRB	Test and reset bits
 ***************************************************************/
#define TRB                                                     \
	SET_Z(tmp&A);												\
	tmp &= ~A

/* 65C02 *******************************************************
 * TSB	Test and set bits
 ***************************************************************/
#define TSB 													\
	SET_Z(tmp&A);												\
	tmp |= A

/***************************************************************
 ***************************************************************
 *			Macros to emulate the 6510 opcodes
 ***************************************************************
 ***************************************************************/

/* 6510 ********************************************************
 *	ANC logical and, set carry from bit of A
 ***************************************************************/
#define ANC 													\
	P &= ~FLAG_C;													\
	A = (byte)(A & tmp);										\
	if (A & 0x80)												\
		P |= FLAG_C;												\
	TEST(A)

/* 6510 ********************************************************
 *	ASR logical and, logical shift right
 ***************************************************************/
#define ASR 													\
	tmp = (byte)(A & tmp);										\
	LSR

/* 6510 ********************************************************
 * AST	and stack; transfer to accumulator and index X
 * logical and stack (LSB) with data, transfer result to S
 * transfer result to accumulator and index X also
 ***************************************************************/
#define AST 													\
	S &= tmp;													\
	A = X = S;													\
	TEST(A)

/* 6510 ********************************************************
 *	ARR logical and, rotate right
 ***************************************************************/
#define ARR 													\
	tmp = (byte)(A & tmp);										\
	ROR

/* 6510 ********************************************************
 *	ASX logical and X w/ A, subtract data from X
 ***************************************************************/
#define ASX 													\
	P &= ~FLAG_C;													\
	X &= A; 													\
	if (X >= tmp)												\
		P |= FLAG_C;												\
	X = (byte)(X - tmp);										\
	TEST(X)

/* 6510 ********************************************************
 *	AXA transfer index X to accumulator, logical and
 ***************************************************************/
#define AXA 													\
	tmp = (byte)(X & tmp);										\
	TEST(tmp)

/* 6510 ********************************************************
 *	DCP decrement data and compare
 ***************************************************************/
#define DCP 													\
	tmp = (byte)--tmp;											\
	P &= ~FLAG_C;													\
	if (A >= tmp)												\
		P |= FLAG_C;												\
    TEST((byte)(A - tmp))

/* 6502 ********************************************************
 *	DOP double no operation
 ***************************************************************/
#define DOP 													\
	PCW++

/* 6510 ********************************************************
 *	ISB increment and subtract with carry
 ***************************************************************/
#define ISB 													\
	tmp = (byte)++tmp;											\
    SBC

/* 6510 ********************************************************
 *  LAX load accumulator and index X
 ***************************************************************/
#define LAX 													\
	A = X = (byte)tmp;											\
	TEST(A)

/* 6510 ********************************************************
 * RLA	rotate left and logical and accumulator
 *	new C <- [7][6][5][4][3][2][1][0] <- C
 ***************************************************************/
#define RLA 													\
	tmp = (tmp << 1) | (P & FLAG_C);								\
	P = (P & ~FLAG_C) | ((tmp >> 8) & FLAG_C);						\
	tmp = (byte)tmp;											\
	A &= tmp;													\
    TEST(A)

/* 6510 ********************************************************
 * RRA	rotate right and add with carry
 *  C -> [7][6][5][4][3][2][1][0] -> C
 ***************************************************************/
#define RRA 													\
	tmp |= (P & FLAG_C) << 8;										\
	P = (P & ~FLAG_C) | (tmp & FLAG_C);								\
	tmp = (byte)(tmp >> 1); 									\
    ADC

/* 6510 ********************************************************
 * SAX	logical and accumulator with index X and store
 ***************************************************************/
#define SAX 													\
	tmp = A & X;												\
	TEST(tmp)

/* 6510 ********************************************************
 *	SLO shift left and logical or
 ***************************************************************/
#define SLO 													\
	P = (P & ~FLAG_C) | ((tmp >> 7) & FLAG_C);						\
	tmp = (byte)(tmp << 1); 									\
	A |= tmp;													\
    TEST(A)

/* 6510 ********************************************************
 *	SRE logical shift right and logical exclusive or
 *  0 -> [7][6][5][4][3][2][1][0] -> C
 ***************************************************************/
#define SRE 													\
	P = (P & ~FLAG_C) | (tmp & FLAG_C);								\
	tmp = (byte)tmp >> 1;										\
	A ^= tmp;													\
    TEST(A)

/* 6510 ********************************************************
 * SAH	store accumulator and index X and high + 1
 * result = accumulator and index X and memory [PC+1] + 1
 ***************************************************************/
#define SAH 													\
	tmp = A & X;												\
	tmp &= (M6502ReadOpArg((PCD + 1) & 0xffff) + 1)

/* 6510 ********************************************************
 * SSH	store stack high
 * logical and accumulator with index X, transfer result to S
 * logical and result with memory [PC+1] + 1
 ***************************************************************/
#define SSH 													\
	tmp = S = A & X;											\
	tmp &= (byte)(M6502ReadOpArg((PCD + 1) & 0xffff) + 1)

/* 6510 ********************************************************
 * SXH	store index X high
 * logical and index X with memory[PC+1] and store the result
 ***************************************************************/
#define SXH 													\
	tmp = X & (byte)(M6502ReadOpArg((PCD + 1) & 0xffff)

/* 6510 ********************************************************
 * SYH	store index Y and (high + 1)
 * logical and index Y with memory[PC+1] + 1 and store the result
 ***************************************************************/
#define SYH 													\
	tmp = Y & (byte)(M6502ReadOpArg((PCD + 1) & 0xffff) + 1)

/* 6510 ********************************************************
 *	TOP triple no operation
 ***************************************************************/
#define TOP 													\
	PCD += 2

