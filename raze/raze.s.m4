! ne pas toucher fonctionne avec higemaru,pkunwar, wonderboy - remettre emulate_r pour green beret ?
!----------------------------------------------------------------------------!
! RAZE-SH4 Z80 emulator.
! version 0.01
!
! Copyright (c) 1999 Richard Mitton
! Rewritten for the SH4 platform by Oscar Orallo
!
! This may only be distributed as part of the complete RAZE package.
! See RAZE.TXT for license information.
! Starting date: Around first quarter of 2005
! Last update: 03/01/2007
!----------------------------------------------------------------------------!

! You are not expected to understand this.

define(`BIG_ENDIAN')          ! 

include(`raze.reg')

!----------------------------------------------------------------------------!
! Comment these in/out for faster speed (less accurate):
!define(`EMULATE_UNDOC_FLAGS')      ! a couple of undocumented flags
!define(`EMULATE_BITS_3_5')         ! bits 3/5 of the flags (undocumented)
!define(`EMULATE_WEIRD_STUFF')      ! misc *very obscure* undocumented behaviour
!define(`EMULATE_R_REGISTER')       ! precise R register (not usually needed)
!define(`NO_EXTRA_CYCLES')      	   ! Only for debugging
!define(`USE_FETCH_CALLBACK')       ! call a callback for every fetch (slow!)
!define(`SINGLE_MEM_BLOCK')         ! treat memory map as a whole block
!define(`SINGLE_FETCH')             ! fetch opcodes from a single memory block
!define(`SINGLE_MEM_HANDLER')       ! use single memory handler
define(`AUTODOWN_IRQS')            ! autodown IRQ feature
!define(`EMULATE1')                 ! helpful to debug
!define(`IRQ_CYCLES')		        ! spend cycles for IRQs
!define(`NO_PC_LIMIT')
!define(`BASED_PC')
define(`SH2_COMPAT_CODE')          ! generate SH2 compatible code
define(`VBT')          ! vbt changes
!define(`NO_READ_HANDLER')

!----------------------------------------------------------------------------!

! Notas de desarrollo
! Algunas instrucciones fijan las banderas sin usar los defines
! Las banderas A y F se deberian salvar/restaurar en los CALLGCC_START/END


define(`lntag',`0')

! Registers: see raze.reg
!
! GCC wants [r0-r7] to be preserved at all times

!.section .bss
.data

! The current context
.align 5
context_start:
_z80_AF: .long 0
_z80_BC: .long 0
_z80_DE: .long 0
_z80_HL: .long 0
_z80_IX: .long 0
_z80_IY: .long 0
_z80_PC: .long 0
_z80_SP: .long 0
_z80_AF2: .long 0
_z80_BC2: .long 0
_z80_DE2: .long 0
_z80_HL2: .long 0
_z80_IFF1: .byte 0
_z80_IFF2: .byte 0
_z80_R: .word 0,0   ! dato long
_z80_R2: .word 0,0  ! dato long
_z80_I: .byte 0
_z80_flag35: .byte 0
_z80_IM: .byte 0
_z80_IRQVector: .byte 0
_z80_IRQLine: .byte 0
_z80_halted: .byte 0
_z80_Extra_Cycles: .long 0
registers_end:

_z80_Fetch:
ifdef(`SINGLE_FETCH',
`
	.long 0
',
`
	.rept 256
	.long 0
	.endr
')

.global _z80_Read
_z80_Read:
ifdef(`SINGLE_MEM_BLOCK',
`
	.long 0
',
`
	ifdef(`SINGLE_MEM_HANDLER',
	`
		.long 0
	',
	`
		.rept 256*2
		.long 0
		.endr
	')
')


.global _z80_Write
_z80_Write:
ifdef(`SINGLE_MEM_BLOCK',
`
	.long 0
',
`
	ifdef(`SINGLE_MEM_HANDLER',
	`
		.long 0
	',
	`
		.rept 256*2
		.long 0
		.endr
	')
')

_z80_In: .long 0
_z80_Out: .long 0
_z80_RetI: .long 0
_z80_Fetch_Callback: .long 0
context_end:
fill: .long 0     ! safety gap, so z80_reset can use 32-bit transfers

! Variables
_z80_ICount: .long 0
_z80_Initial_ICount: .long 0
_z80_TempICount: .long 0
_z80_afterEI: .long 0

ifdef(`SINGLE_MEM_BLOCK',`',
`
	ifdef(`SINGLE_MEM_HANDLER',`',
	`
		.align 2
		_z80_Mem_Handlers: .long _z80_memread, _z80_memwrite
	')
')

.text

divert(-1)

!----------------------------------------------------------------------------!
! Increment lntag value
! INC_LNTAG
define(`INC_LNTAG',
`
	define(`lntag',incr(lntag))
')

define(`DEF_LN',
`
	INC_LNTAG
	define(`ln',lntag)
')

! Generates an 8-bit IY z80 register memory reference tag
! GEN_IY_TAG reg8
define(`GEN_IY_TAG',
`
	ifelse($1,`IYh',
	`
		`_z80_IY_'ln: .long _z80_IY + 1
	',
	`
		ifelse($1,`IYl',
		`
			`_z80_IY_'ln: .long _z80_IY
		')
	')
')

! Saves a SH register into stack
! SAVE_REG
define(`SAVE_REG',`mov.l $1,@-r15')

! Restores a SH register from stack
! RESTORE_REG
define(`RESTORE_REG',`mov.l @r15+,$1')


! Loads 8-bit z80 register into r0 without regarding sign-extension
! LOAD_ZREG reg8
define(`LOAD_ZREG',
`
	! Load operand to r0
	ifelse($1,`A',`mov z$1,r0',`mov.b z$1,r0')
')

! Loads 8-bit z80 register into r0 sign-extended
! LOAD_ZREG_S reg8
define(`LOAD_ZREG_S',
`
	ifelse($1,`A',`exts.b z$1,r0',`mov.b z$1,r0')
')

! Writes back 8-bit z80 register from r0
! WRITE_ZREG reg8
define(`WRITE_ZREG',
`
	ifelse($1,`A',`mov r0,z$1',`mov.b r0,z$1')
')

! Loads 8-bit IY (low/high) z80 register into r0 sign-extended
! LOAD_SRC_REG
define(`LOAD_YREG',
`
	mov.l `_z80_IY_'ln,DIRT_REG
	mov.b @DIRT_REG,r0
')

! Writes back 8-bit IY (low/high) z80 register from r0
! WRITE_YREG
define(`WRITE_YREG',
`
	mov.l `_z80_IY_'ln,TMP_REG
	mov.b r0,@TMP_REG
')


! Fetch callback procedure
! FETCH_CALLBACK
define(`FETCH_CALLBACK',
`
	ifdef(`USE_FETCH_CALLBACK',
	`
		CALLGCC_START
		mov #_z80_Fetch_Callback - REF, r0
		mov.l @(r0,REF_REG),DIRT_REG
		jsr @DIRT_REG	
		mov ezPC,r4
		CALLGCC_END
	')
')

! Generate a memory tag
! GEN_MT
define(`GEN_MT',`$1_$2: .long $1')

! Prepare to call a GCC function
! CALLGCC_START
define(`CALLGCC_START',
`
	ifdef(`USE_FETCH_CALLBACK',
	`
		! An extra precaution
		SAVE_REG(`r0')
		SAVE_REG(`r1')
		SAVE_REG(`r2')
	')

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	SAVE_REG(`r3')
	SAVE_REG(`r4')
	SAVE_REG(`r5')
SAVE_REG(`r6')
	SAVE_REG(`r7')
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register
')

! Return from a GCC function
! CALLGCC_END
define(`CALLGCC_END',
`
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	RESTORE_REG(`r7')
RESTORE_REG(`r6')
	RESTORE_REG(`r5')
	RESTORE_REG(`r4')
	RESTORE_REG(`r3')
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	ifdef(`USE_FETCH_CALLBACK',
	`
		! An extra precaution
		RESTORE_REG(`r2')
		RESTORE_REG(`r1')
		RESTORE_REG(`r0')
	')
')

! Read a byte from the PC, into $1
! GETBYTE
define(`GETBYTE',
`
	GETDISP(`$1')
	extu.b $1,$1
')

! Read a byte from the PC, sign-extended into $1
! GETDISP
define(`GETDISP',
`
	FETCH_CALLBACK
	ifdef(`SINGLE_FETCH',
	`
		ifdef(`BASED_PC',
		`
			mov.b @zPC+,$1
		',
		`
			mov FETCH_REG,r0
			mov.b @(r0,zPC),$1
			add #1,zPC
		')
	',
	`
		mov zPC,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,FETCH_REG),r0
		mov.b @(r0,zPC),$1
		add #1,zPC
	')
	ifdef(`NO_PC_LIMIT',`',`extu.w zPC,zPC')
')

! Read a word from the PC, into $1, sign-extended
! GETWORD
define(`GETWORD',
`
	ifelse($1,DIRT_REG,`error DIRT_REG register usage')
	GETBYTE(`DIRT_REG')
	GETDISP(`$1')
	shll8 $1
	or DIRT_REG,$1
')

! Read a byte from memory (r0) into TMP_REG
! MEMREAD
define(`MEMREAD',
`
	ifdef(`SINGLE_MEM_BLOCK',
	`
		mov.l @MEM_REG,DIRT_REG
		$1
extu.w r0,r0
		mov.b @(r0,DIRT_REG),TMP_REG
	',
	`
		mov.l @MEM_REG,DIRT_REG
                $2
		jsr @DIRT_REG
		$1
	')
')

! Write a byte in TMP_REG to memory (r0)
! MEMWRITE
define(`MEMWRITE',
`
	ifdef(`SINGLE_MEM_BLOCK',
	`
		mov.l @(4,MEM_REG),DIRT_REG
		$1
extu.w r0,r0
		mov.b TMP_REG,@(r0,DIRT_REG)
	',
	`
		mov.l @(4,MEM_REG),DIRT_REG
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                $2
		jsr @DIRT_REG
		$1
	')
')

! Read a byte from port ($1) into r0
! IOREAD
define(`IOREAD',
`
	ifelse($1,`DIRT_REG',`error DIRT_REG usage')
	CALLGCC_START
	mov.l `_z80_In_'ln, DIRT_REG
	mov.l @DIRT_REG,DIRT_REG
	jsr @DIRT_REG
	extu.w $1,r4          ! SH requires to get parameter value here
	CALLGCC_END
')

! Write a byte in $2 to port ($1)
! IOWRITE
define(`IOWRITE',
`
	ifelse($1,`DIRT_REG',`error DIRT_REG usage')
	ifelse($2,`DIRT_REG',`error DIRT_REG usage')
	CALLGCC_START
	mov.l `_z80_Out_'ln, DIRT_REG
	mov.l @DIRT_REG,DIRT_REG
	extu.w $1,r4   ! first parameter  (address)
	jsr @DIRT_REG
	extu.b $2,r5   ! second parameter (data)
	CALLGCC_END
')

! Align code to the default boundary
! ALIGN
define(`ALIGN',`.align 5')
! à valider avec Fox68k
!define(`ALIGN',`.align 2')

! Align data to the default boundary
! ALIGN_DATA
define(`ALIGN_DATA',`.align 2')

! Define an instruction
! DEF prefix, opcode, std_cycles, extra_cycles
define(`DEF',
`
	define(`STD_CYCLES',`$3')
	define(`STD_BACKUP_CYCLES',`$3')
	define(`EXTRA_CYCLES',`$4')
$1_$2:
')

! Subtract the clock cycles for this instruction from the ICount
! and jump to the next instruction if there are clock cycles remaining
define(`DO_CYCLES_NEXT',
`
	jmp @ZFINISH
	add #-STD_CYCLES,ICOUNT_REG
')

! Subtract the extra cycles for this instruction from the ICount
! and jump to the next instruction if there are clock cycles remaining
define(`DO_EXTRA_CYCLES_NEXT',
`
	jmp @ZFINISH
	add #-EXTRA_CYCLES,ICOUNT_REG
')

! Subtract the extra clock cycles for this instruction from the ICount
define(`DO_EXTRA_CYCLES',
`
	add #-EXTRA_CYCLES,ICOUNT_REG
')

! Finish straight away
! $1 = delayed SH instruction
define(`EXIT',
`
	INC_LNTAG

	! jump to z80_finish
	mov.l `z80_finish_'lntag,r0
	jmp @r0
	$1

ALIGN_DATA
	`z80_finish_'lntag: .long z80_finish
')

! Jump to the next instruction
! $1 = Delayed SH instruction
define(`NEXT',
`
	GETDISP(r0)
	shll2 r0
	mov.l @(r0,OPTABLE_REG),r0
	ifdef(`EMULATE_R_REGISTER',`add #1,zR')
	jmp @r0
	$1
')

! Add extra cycles
define(`ADD_EXTRA_CYCLES',
`
ifdef(`IRQ_CYCLES',
`
	mov #(_z80_Extra_Cycles - REF),r0
	mov.l @(r0,REF_REG),DIRT_REG
	shlr16 DIRT_REG
	add #$1,DIRT_REG
	mov.l DIRT_REG,@(r0,REF_REG)
')
')

! Check the IRQ line, cause an interrupt if needed
define(`CHECK_IRQ',
`
	! check the IRQ line
	mov #(_z80_IRQLine - REF),r0
	mov.b @(r0,REF_REG),r0    ! r0 = z80_IRQLine
	tst r0,r0
	bt .finish_chkirq_$1

	! check ints are enabled
	mov #(_z80_IFF1 - REF),r0
	mov.b @(r0,REF_REG),r0    ! r0 = z80_IFF1
	tst r0,r0
	bt .finish_chkirq_$1

ifdef(`AUTODOWN_IRQS',
`
	mov #0,DIRT_REG
	mov #(_z80_IRQLine - REF),r0
	mov.b DIRT_REG,@(r0,REF_REG)   ! z80_IRQLine = 0
')
	! disable interrupts
	mov #0,DIRT_REG
	mov #(_z80_IFF1 - REF),r0
	mov.b DIRT_REG,@(r0,REF_REG)
	mov #(_z80_IFF2 - REF),r0
	mov.b DIRT_REG,@(r0,REF_REG)

	ifdef(`EMULATE_R_REGISTER',`add #1,zR')

	mov #(_z80_halted - REF),r0
	mov.b DIRT_REG,@(r0,REF_REG)	! Un-HALT the Z80

	! push the PC into the z80 stack
	mov.w ezSP,r0
	mov ezPC,TMP_REG
	add #-1,r0
	MEMWRITE(`shlr8 TMP_REG')     ! first byte
	add #-1,r0
	MEMWRITE(`mov ezPC,TMP_REG')  ! second byte

	mov.w r0,zSP                  ! update SP
	mov #(_z80_IM - REF),r0
	mov.b @(r0,REF_REG),r0        ! r0 = z80_IM
	tst r0,r0
	bf .not_im0_$1

	! IM 0, 13 T-states
	ADD_EXTRA_CYCLES(`13')
	mov #(_z80_IRQVector - REF),r0
	mov.b @(r0,REF_REG),r0    ! r0 = z80_IRQVector
	extu.b r0,r0
	add #-127,r0              ! substract 0xC7 = 199
	add #-72,r0
	bra .finish_chkirq_$1
	extu.b r0,zPC

ALIGN
.not_im0_$1:
	dt r0
	bf .not_im1_$1
	
	! IM 1, 13 T-states
	ADD_EXTRA_CYCLES(`13')
	bra .finish_chkirq_$1
	mov #0x38,ezPC

ALIGN
.not_im1_$1:
	! IM 2, 19 T-states
	ADD_EXTRA_CYCLES(`19')
	mov #(_z80_IRQVector - REF),r0
	mov.b @(r0,REF_REG),TMP_REG    ! TMP_REG = z80_IRQVector
	mov #(_z80_I - REF),r0
	mov.b @(r0,REF_REG),r0               ! r0 = z80_I
	extu.b TMP_REG,TMP_REG
	shll8 r0
	MEMREAD(`or TMP_REG,r0')
	add #1,r0
	MEMREAD(`extu.b TMP_REG,ezPC')
	extu.b TMP_REG,TMP_REG
	shll8 TMP_REG
	or TMP_REG,ezPC

.finish_chkirq_$1:
')

!----------------------------------------------------------------------------!
! Helper functions

! Cache z80 status register into SH register set
define(`CACHE_CCR',
`
	mov zF,r0
	tst #FLAG_Z,r0               ! test Z flag
	movt FLAGS_ZSP               ! set in register
	and #FLAG_S,r0               ! isolate S flag
	or r0,FLAGS_ZSP              ! merge S flag in
	exts.b FLAGS_ZSP,FLAGS_ZSP   ! get S in place
	
	mov zF,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,zF             ! P/V indicator set
	movt FLAGS_C          ! get cached C
	shlr r0               ! T = N
	addc zF,zF            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt DIRT_REG
	tst DIRT_REG,DIRT_REG
	addc zF,zF            ! zF = 00000iNH
	shlr r0               ! T = V
	addc zF,zF            ! zF = 0000iNHV
')

! Restore z80 status register into $1 SH register
define(`WRITEBACK_CCR',
`
	DEF_LN

	exts.b FLAGS_ZSP,FLAGS_ZSP
	mov.l `Byte_Flags_'ln,r0
	mov.b @(r0,FLAGS_ZSP),TMP_REG    ! TMP_REG = SZ000P00

	mov zF,r0
	tst #FLG_iV,r0     ! test P/V indicator
	bt/s `.no_veval'ln
	or FLAGS_C,TMP_REG                      ! TMP_REG = SZ000P0C

	mov #~FLAG_P,DIRT_REG
	shlr r0                                            ! V -> T
	and DIRT_REG,TMP_REG                  ! TMP_REG = SZ00000C
	movt DIRT_REG               ! DIRT_REG = V
	shll2 DIRT_REG               ! V flag in place
	or DIRT_REG,TMP_REG    ! TMP_REG = SZ000V0C

`.no_veval'ln:
	shlr zF                            ! ignore V
	shlr zF                            ! T = H
	movt DIRT_REG               ! DIRT_REG = H
	shll2 DIRT_REG
	shll2 DIRT_REG                ! H flag in place
	or DIRT_REG,TMP_REG     ! TMP_REG = SZ0H0P0C

	shlr zF                             ! T = N
	movt DIRT_REG                ! DIRT_REG = N
	shll DIRT_REG
	or DIRT_REG,TMP_REG
	extu.b TMP_REG,$1
')

define(`SET_iVN',
`
	! Set N flag according to operation performed
!	ifelse($1,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	ifelse($1,`add',`mov #2,zF',`mov #3,zF')
')

! Sets ZSP flags
! $1 register to evalute flags from
define(`SET_ZSP',
`
	! Set Z
	ifelse($1,`FLAGS_ZSP',`',`mov $1,FLAGS_ZSP')
')

! Test z80 condition (S,Z,P,H,V,N,C)
! $1 flag to evaluate
define(`TEST_CONDITION',
`
	ifelse($1,`Z',
	`
		mov FLAGS_ZSP,r0
		tst #0xFF,r0
	',
	`
		ifelse($1,`S',
		`
			mov FLAGS_ZSP,r0
			tst #0x80,r0
		',
		`
			ifelse($1,`P',
			`
				! TODO: P/V flag evaluation
				mov zF,r0
				tst #FLG_P,r0
			',
			`
				ifelse($1,`C',
				`
					tst FLAGS_C,FLAGS_C
				',
				`
					mov zF,r0
					tst #FLG_$1,r0
				')
			')
		')
	')
')

! Evaluate bits 3-5 of the z80 status register
define(`DO_BITS_3_5',
`
	ifdef(`EMULATE_BITS_3_5',`lds $1,MACL')
')

define(`SHLD_OP',
`
	ifdef(`SH2_COMPAT_CODE',
	`
		ifelse($3,`8',`shll8 $2')
		ifelse($3,`16',`shll16 $2')
		ifelse($3,`24',
		`
			shll16 $2
			shll8 $2
		')

		ifelse($3,`-8',`shlr8 $2')
		ifelse($3,`-16',`shlr16 $2')
		ifelse($3,`-24',
		`
			shlr16 $2
			shlr8 $2
		')
	',
	`
		shld $1,$2
	')
')

! Add/Sub($1) A, r0
define(`DO_ADDSUB',
`
	DO_BITS_3_5(`r0')
	define(`op',`ifelse($1,`cmp',`sub',$1)')

	SET_iVN(`op')

	! H flag calculation
	mov zA, FLAGS_ZSP            ! save original A register value
	mov #0xF,DIRT_REG           ! H masking
	and DIRT_REG,FLAGS_ZSP   ! FLAGS_ZSP = A masked
	and r0,DIRT_REG                ! DIRT_REG = r0 masked
	mov #FLAG_H,TMP_REG
	op DIRT_REG,FLAGS_ZSP     ! do the operation
	cmp/hs TMP_REG,FLAGS_ZSP
	addc zF,zF                          ! set H flag

	ifdef(`SH2_COMPAT_CODE',
	`
	',
	`
		mov #24,DIRT_REG             ! to prepare operands
	')

	! VC flag calculation
	ifelse($1,`cmp',
	`
		mov zA,TMP_REG       ! Save A register operand
		SHLD_OP(`DIRT_REG',`TMP_REG',24)
	',
	`
		SHLD_OP(`DIRT_REG',`zA',24)
		mov zA,TMP_REG       ! Save A register operand
	')

	SHLD_OP(`DIRT_REG',`r0',24)  ! Prepare register operand to operate
	op`v' r0,TMP_REG             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse($1,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc zF,zF                   ! V flag set

	ifelse($1,`cmp',
	`
		mov zA,TMP_REG
		SHLD_OP(`DIRT_REG',`TMP_REG',24)
!		shld DIRT_REG,TMP_REG
	')
	ifelse($1,`cmp',`subc r0,TMP_REG',`$1c r0,zA')
	movt FLAGS_C        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	ifdef(`SH2_COMPAT_CODE',
	`
	',
	`
		mov #-24,DIRT_REG
	')

	ifelse($1,`cmp',
	`
		SHLD_OP(`DIRT_REG',`TMP_REG',-24)
		SET_ZSP(`TMP_REG')
	',
	`
		SHLD_OP(`DIRT_REG',`zA',-24)
		SET_ZSP(`zA')
	')

	undefine(`op')
')

! Adc/Sbc($1) A, r0
define(`DO_ADCSBC',
`
	SET_iVN(`$1')

	! H flag calculation
	mov #0x0F,DIRT_REG       ! nibble mask for A operand
	mov DIRT_REG,FLAGS_ZSP   ! nibble mask for r0 operand
	mov DIRT_REG,TMP_REG     ! get mask to compare against result
	and zA,DIRT_REG          ! save zA temporary masking for H flag calculation
	and r0,FLAGS_ZSP	

	cmp/pl FLAGS_C           ! C -> T
	$1c FLAGS_ZSP,DIRT_REG
	cmp/hi TMP_REG,DIRT_REG
	addc zF,zF               ! merge partial H flag in

	! V flag calculation

	ifdef(`SH2_COMPAT_CODE',
	`
	',
	`
		mov #24,TMP_REG             ! to prepare operands
	')

	SHLD_OP(`TMP_REG',`r0',24)    ! prepare register operands
	SHLD_OP(`TMP_REG',`FLAGS_C',24)
	SHLD_OP(`TMP_REG',`zA',24)
	mov zA,FLAGS_ZSP         ! save zA to get C flag
	$1v r0,zA
	movt TMP_REG             ! partial V flag
	$1v FLAGS_C,zA
	addc zF,zF               ! merge partial V flag in
	or TMP_REG,zF            ! V flag in flags register

	! C flag calculation	
	clrt
	$1c r0,FLAGS_ZSP
	movt TMP_REG             ! partial C flag
	$1c FLAGS_C,FLAGS_ZSP
	movt FLAGS_C             ! partial C flag in
	or TMP_REG,FLAGS_C       ! C flag in register


	! Get result
	ifdef(`SH2_COMPAT_CODE',
	`
	',
	`
		mov #-24,TMP_REG             ! to prepare operands
	')

	SHLD_OP(`TMP_REG',`zA',-24)
	
	! Set ZSP flags
	SET_ZSP(`zA')
')


! Add A, r0
define(`DO_ADD',
`
	DO_ADDSUB(`add')
')

! Sub A, r0
define(`DO_SUB',
`
	DO_ADDSUB(`sub')
')

! Cp A, r0
define(`DO_CP',
`
	DO_ADDSUB(`cmp')
')

! Adc A, r0
define(`DO_ADC',
`
	DO_ADCSBC(`add')
')

! Sbc A, r0
define(`DO_SBC',
`
	DO_ADCSBC(`sub')
')

! And A, $1
define(`DO_AND',
`
	ifelse($1,`A',`',`and r0,zA')    ! do the operation

	SET_ZSP(`zA')
	DO_BITS_3_5(`zA')

	mov #0,FLAGS_C    ! C = 0
	mov #FLG_H,zF      ! H = 1, N,V = 0
')

! Or A, $1
define(`DO_OR',
`
	ifelse($1,`A',`',`or r0,zA')    ! do the operation

	mov #0,FLAGS_C    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	SET_ZSP(`zA')
	mov #0,zF

	DO_BITS_3_5(`zA')
')

! Xor A, $1
define(`DO_XOR',
`
	ifelse($1,`A',
	`
		mov #0,zA
		mov zA,zF
	',
	`
		mov #0,zF
		xor r0, zA
	')
	
	mov zF,FLAGS_C    ! Clear carry
	SET_ZSP(`zA')
	DO_BITS_3_5(`zA')
')

! Perform logic operation against the A register
! Log $1 A, $2
define(`DO_LOG',
`
	DO_$1($2)
')

! TODO: Quitar save_reg/restore_reg
! Inc/Dec operation
define(`DO_INCDEC',
`
	mov.l $1`_Table_'ln,TMP_REG
	mov.b @(r0,TMP_REG),zF
	ifelse($1,`INC',`add #1,r0',`add #-1,r0')
	SAVE_REG(`FLAGS_C')
	SAVE_REG(`r0')
	CACHE_CCR
	RESTORE_REG(`r0')
	RESTORE_REG(`FLAGS_C')
	DO_BITS_3_5(`r0')
')

! Inc r0
define(`DO_INC',
`
	DO_INCDEC(`INC')
')

! Dec r0
define(`DO_DEC',
`
	DO_INCDEC(`DEC')
')

! Add16 $1, $2
define(`DO_ADD16',
`
	mov #(FLG_iV | FLG_V),DIRT_REG    ! H, N = 0
	mov.l z$1,r0       ! 1st operand
	and DIRT_REG,zF
	mov.l z$2,TMP_REG  ! 2nd operand
	ifdef(`EMULATE_UNDOC_FLAGS2',
	`
		mov r0,DIRT_REG
	')
!vbt inutile pour big endian        
!	shll16 r0          ! prepare 1st operand
	clrt               ! T = 0 (required by the carry operation)
!vbt inutile pour big endian        
!	shll16 TMP_REG     ! prepare 2nd operand

	addc TMP_REG,r0    ! do it
	shlr16 r0          ! get the result to the lower word
!vbt
	shlr16 DIRT_REG

	movt FLAGS_C       ! set carry flag
	mov.w r0,z$1       ! writeback result

	ifdef(`EMULATE_UNDOC_FLAGS',
	`
! vbt 07/08/2009 a tester : utile
		shlr16 TMP_REG    ! prepare 2nd operand

		xor r0,DIRT_REG
		xor TMP_REG,DIRT_REG
		mov #FLAG_H,TMP_REG
		shlr8 DIRT_REG
		and TMP_REG,DIRT_REG
		shlr2 DIRT_REG
		shlr DIRT_REG
		mov zF,r0
		and #~FLG_H,r0
		or DIRT_REG,r0
		mov r0,zF
	')

	ifdef(`EMULATE_BITS_3_5',
	`
		shlr8 r0
		DO_BITS_3_5(`r0')
	')
')

! TODO: finish from result
! Adc16/Sbc16 op, $2, $3
define(`DO_ADCSBC16',
`
	mov.l z$2,r0          ! first operand
	SET_iVN(`op')
	mov.l z$3,TMP_REG     ! second operand
!vbt inutile pour big endian        
	!shll16 r0             ! prepare first operand
	shll zF                 !  H = 0
	mov r0,FLAGS_ZSP      ! save 1st operand to get C
!vbt inutile pour big endian        
	!shll16 TMP_REG        ! prepare second operand
	shll16 FLAGS_C        ! prepare C flag

	! V flag calculation
	$1v TMP_REG,r0
	movt DIRT_REG         ! partial overflow flag
	$1v FLAGS_C,r0
	addc zF,zF            ! partial overflow flag
	or DIRT_REG,zF        ! set V in

	! Carry flag calculation
	clrt
	mov FLAGS_ZSP,r0      ! reload first operand
	$1c TMP_REG,r0
	movt DIRT_REG         ! DIRT_REG = partial carry flag
	clrt
	$1c FLAGS_C,r0         ! r0 = result
	movt FLAGS_C
	or DIRT_REG,FLAGS_C  ! C flag

	! TODO: emulate undoc
	ifdef(`EMULATE_UNDOC_FLAGS2',
	`
		! H flag calculation
		mov FLAGS_ZSP,DIRT_REG    ! DIRT_REG = reload first operand
		xor r0,DIRT_REG                   ! xor first operand - result
		xor TMP_REG,DIRT_REG        ! xor second operand
		shlr16 DIRT_REG
		shlr8 DIRT_REG
		mov #FLAG_H,TMP_REG
		and TMP_REG,DIRT_REG
		shlr2 DIRT_REG
		shlr DIRT_REG
		or DIRT_REG,zF
	')

	! Write result
	shlr16 r0             ! get result in place
	mov.w r0,z$2

	! Get signed result in r0
	tst r0,r0
	movt FLAGS_ZSP
	dt FLAGS_ZSP
	extu.b FLAGS_ZSP,FLAGS_ZSP
	shlr FLAGS_ZSP
	shlr8 r0
	or r0,FLAGS_ZSP

	ifdef(`EMULATE_BITS_3_5',
	`
		DO_BITS_3_5(`FLAGS_ZSP')
	')
')

! Adc16 $1, $2
define(`DO_ADC16',
`
	DO_ADCSBC16(`add',$1,$2)
')

! Sbc16 $1, $2
define(`DO_SBC16',
`
	DO_ADCSBC16(`sub',$1,$2)
')

! Rotate method
! DO_ROT method (r0)
define(`DO_ROT',
`
	! Rotate Left Arithmetic
	ifelse($1,`RLA',
	`
		mov #(FLG_iV | FLG_V),DIRT_REG    ! isolate P flag
		cmp/pl FLAGS_C    ! C -> T
		and DIRT_REG,zF
		rotcl r0
		movt FLAGS_C      ! get new carry flag
		DO_BITS_3_5(`r0')
	')

	! Rotate Right Arithmetic
	ifelse($1,`RRA',
	`
		mov #(FLG_iV | FLG_V),DIRT_REG    ! isolate P flag
		cmp/pl FLAGS_C    ! C -> T
		and DIRT_REG,zF
		movt TMP_REG
		shlr r0
		movt FLAGS_C      ! get new carry
		and #0x7F,r0      ! clear most significant bit
		shll8 TMP_REG
		shlr TMP_REG
		or TMP_REG,r0     ! merge old carry in the result
		DO_BITS_3_5(`r0')
	')

	! Rotate Left to Carry Arithmetic
	ifelse($1,`RLCA',
	`
		mov #(FLG_iV | FLG_V),DIRT_REG    ! isolate P flag
		and DIRT_REG,zF
		rotl r0
		movt FLAGS_C
		and #0xFE,r0
		or FLAGS_C,r0     ! bit0 = bit7
		DO_BITS_3_5(`r0')
	')

	! Rotate Right to Carry Arithmetic
	ifelse($1,`RRCA',
	`
		mov #(FLG_iV | FLG_V),DIRT_REG    ! isolate P flag
		and DIRT_REG,zF
		rotr r0
		movt FLAGS_C
		and #0x7F,r0      ! clear most significant bit
		mov FLAGS_C,TMP_REG
		shll8 TMP_REG
		shlr TMP_REG
		or TMP_REG,r0
		DO_BITS_3_5(`r0')
	')

	! Rotate Left to Carry
	ifelse($1,`RLC',
	`
		rotl r0
		movt FLAGS_C
		mov #0,zF    ! N,H,V = 0
		SET_ZSP(`r0')
	')

	! Rotate Right to Carry
	ifelse($1,`RRC',
	`
		shlr r0
		movt TMP_REG       ! TMP_REG = C
		exts.b r0,r0

		ifdef(`SH2_COMPAT_CODE',
		`
			mov TMP_REG,FLAGS_C   ! set carry
			shll8 TMP_REG
			shlr TMP_REG
		',
		`
			mov #7,DIRT_REG
			mov TMP_REG,FLAGS_C   ! set carry
			shld DIRT_REG,TMP_REG
		')

		and #0x7F,r0
		mov #0,zF    ! N,H,V = 0
		or TMP_REG,r0
		SET_ZSP(`r0')
	')

	! Rotate Left
	ifelse($1,`RL',
	`
		cmp/pl FLAGS_C ! C -> T
		rotcl r0
		movt FLAGS_C
		mov #0,zF    ! N,H,V = 0
		SET_ZSP(`r0')
	')

	! Rotate Right
	ifelse($1,`RR',
	`
		mov #0,zF     ! N,H,V = 0
		mov FLAGS_C,TMP_REG  ! get old carry
		rotr r0
		movt FLAGS_C  ! set carry
		shll8 TMP_REG
		and #0x7F,r0
		shlr TMP_REG
		or TMP_REG,r0     ! put old carry in result
		SET_ZSP(`r0')
	')
')

! Shift method
! Shift (r0)
define(`DO_SHF',
`
	ifelse($1,`SLA',`SHLX(`a')')
	ifelse($1,`SLL',`SHLX(`l')')
	ifelse($1,`SRA',
	`
		shar r0
		mov #0,zF
		movt FLAGS_C
	')
	ifelse($1,`SRL',
	`
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,zF
		shlr r0
		movt FLAGS_C
	')
	
	! Banderas Z,S y P
	SET_ZSP(`r0')
	! Bits 3-5 SR
	DO_BITS_3_5(`r0')
')

! BIT n      n = bit (0-7)
define(`DO_BIT',
`
	DEF_LN

	tst #(1 << $1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s `bit_is_clear_'ln
	mov #(FLG_iV | FLG_H),zF

	ifelse($1,`7',
	`
		bra `finish_'ln
		mov #-1,FLAGS_ZSP
	',
	`
		bra `finish_'ln
		mov #1,FLAGS_ZSP
	')

`bit_is_clear_'ln:
	mov #(FLG_H | FLG_iV | FLG_P),TMP_REG
	mov #0,FLAGS_ZSP                  ! set Z flag
	or TMP_REG,zF

`finish_'ln:

	DO_BITS_3_5(`r0')
')

! SET r,n      n = bit (0-7)
!              r = 8-bit register
define(`DO_SET',
`
	ifelse($1,`r0',
	`
		or #(1 << $2),$1
	',
	`
		ifelse($1,TMP_REG,
		`
			mov #(1 << $2),DIRT_REG
			or DIRT_REG,$1
		',
		`
			ifelse($1,`A',
			`
				mov #(1 << $2),DIRT_REG
				or DIRT_REG,z$1
			',
			`
				mov.b z$1,r0
				or #(1 << $2),r0
				mov.b r0,z$1
			')
		')
	')
')

! RES r,n      n = bit (0-7)
!              r = 8-bit register
define(`DO_RES',
`
	ifelse($1,`r0',
	`
		and #~(1 << $2),$1
	',
	`
		ifelse($1,TMP_REG,
		`
			mov #~(1 << $2),DIRT_REG
			and DIRT_REG,$1
		',
		`
			ifelse($1,`A',
			`
				mov #~(1 << $2),DIRT_REG
				and DIRT_REG,z$1
			',
			`
				mov.b z$1,r0
				and #~(1 << $2),r0
				mov.b r0,z$1
			')
		')
	')
')

! Calculate flags for LDI/LDD instructions
! LDX_FLAGS
define(`LDX_FLAGS',
`
	DEF_LN

	! Flag calculation
	ifdef(`EMULATE_BITS_3_5',
	`
		extu.b TMP_REG,r0
		add r0,zA
		shll2 r0
		shll2 r0
		and #FLAG_5,r0
		mov r0,DIRT_REG
		mov TMP_REG,r0
		and #FLAG_3,r0
		or r0,DIRT_REG
		DO_BITS_3_5(`DIRT_REG')
	')

	mov #FLG_iV,zF	
	mov.w zBC,r0
	dt r0
	bt/s `ldid_zero'ln
	mov.w r0,zBC

	! Set P/V flag
	mov #(FLG_iV | FLG_P),zF	

`ldid_zero'ln:
')

! Perform arithmetic against the A register
! DO_ART $1(op) $2(src)
define(`DO_ART',
`
	ifelse($2,`r0',`',`LOAD_ZREG($2)')
	DO_$1
')

! Compare A against (HL), post-xcrement
! CPX operation
define(`CPX',
`
!	mov #FLAG_C,DIRT_REG
!	and DIRT_REG,zF     ! isolate C flag
	mov.l zHL,r0            ! Read from (HL)
	shlr16 r0
	SET_iVN(`sub')
	MEMREAD(`exts.b zA,zA')

	mov zA,DIRT_REG
	! Post-xcrement HL register
	ifelse($1,`I',`add #1,r0',`add #-1,r0')
	sub TMP_REG,DIRT_REG       ! compare result
	mov.w r0,zHL    ! writeback HL

	mov.w zBC,r0
!	exts.b DIRT_REG,DIRT_REG
	dt r0
	bt/s `cpi_zero'ln
	mov.w r0,zBC    ! writeback BC

	mov #FLG_P,r0    ! Set P if BC != 0
	or r0,zF

`cpi_zero'ln:

	! SZ flag calculation
	mov DIRT_REG,FLAGS_ZSP
	or #FLAG_N,r0    ! set N flag
	or r0,zF

	! H flag calculation
	mov DIRT_REG,r0
	xor TMP_REG,r0
	xor zA,r0
	and #FLAG_H,r0
	shlr2 r0
	shlr r0
	or r0,zF
	
	ifdef(`EMULATE_BITS_3_5',
	`
		! TODO
	')
')

! Compare A against (HL), post-xcrement, repeat
! CPXR operation
define(`CPXR',
`
!	mov #FLAG_C,DIRT_REG
	exts.b zA,zA                  ! sign-extend zA to compare against (HL)
!	and DIRT_REG,zF            ! keep the old carry
	SET_iVN(`sub')

`cpir_loop'ln:

	! Read from (HL)
!vbtvbt        
	MEMREAD(`mov.w zHL,r0')

	mov.l zBC,DIRT_REG
	shlr16 DIRT_REG

	! post-xcrement HL register
	ifelse($1,`I',`add #1,r0',`add #-1,r0')
	mov.w r0,zHL               ! writeback HL
	dt DIRT_REG                ! decrement BC
!	extu.w DIRT_REG,DIRT_REG
!vbt à tester !!!!!
	shll16 DIRT_REG
	bt/s `cpir_end_bc'ln       ! end due to BC = 0
	mov.l DIRT_REG,zBC

	ifdef(`EMULATE_R_REGISTER',`add #1,zR')
	cmp/eq zA,TMP_REG          ! end due to A = (HL)
	bt `cpir_end_equal'ln

	add #-EXTRA_CYCLES,ICOUNT_REG
	cmp/pl ICOUNT_REG
	bt `cpir_loop'ln

	! out of cycles, but not finished
	mov zA,DIRT_REG
	sub TMP_REG,DIRT_REG
	
	ifdef(`EMULATE_BITS_3_5',`SAVE_REG(`DIRT_REG')')

	! SZ flag calculation
	mov #FLG_P, r0
	mov DIRT_REG,FLAGS_ZSP
	or r0,zF

	! H flag calculation
	mov DIRT_REG,r0
	xor TMP_REG,r0
	xor zA,r0
	and #FLAG_H,r0
	shlr2 r0
	shlr r0
	or r0,zF
	
	ifdef(`EMULATE_BITS_3_5',
	`
		mov #FLG_H,DIRT_REG
		RESTORE_REG(`TMP_REG')
		tst DIRT_REG,zF
		bt `cpxr_hzero'ln
		dt TMP_REG
		
		`cpxr_hzero'ln:
		mov #FLAG_3,r0
		mov TMP_REG,DIRT_REG
		and r0,DIRT_REG
		shll2 TMP_REG
		shll2 TMP_REG
		mov #FLAG_5,r0
		and r0,TMP_REG
		or TMP_REG,DIRT_REG
		DO_BITS_3_5(`DIRT_REG')
	')
	
	EXIT(`add #-2,zPC')     ! rewind to instruction start and exit

ALIGN
`cpir_end_equal'ln:
	mov #FLAG_P,DIRT_REG
	or DIRT_REG,zF
	
`cpir_end_bc'ln:
	mov zA,DIRT_REG
	sub TMP_REG,DIRT_REG

	! SZ flag calculation
	mov DIRT_REG,FLAGS_ZSP

	! H flag calculation
	mov DIRT_REG,r0
	xor TMP_REG,r0
	xor zA,r0
	and #FLAG_H,r0
	shlr2 r0
	shlr r0
	or r0,zF

	ifdef(`EMULATE_BITS_3_5',
	`
		mov #FLAG_H,DIRT_REG
		RESTORE_REG(`TMP_REG')
		tst DIRT_REG,zF
		bt `cpxr_hzero2'ln
		dt TMP_REG
		
		`cpxr_hzero2'ln:
		mov #FLAG_3,r0
		mov TMP_REG,DIRT_REG
		and r0,DIRT_REG
		shll2 TMP_REG
		shll2 TMP_REG
		mov #FLAG_5,r0
		and r0,TMP_REG
		or TMP_REG,DIRT_REG
		DO_BITS_3_5(`DIRT_REG')
	')
')

! Move from (HL) to (DE), post-increment, repeat
! LDXR operation
define(`LDXR',
`
	! Flags 3-5 and weird stuff not implemented
	ifelse($1,`I',
	`
		ifdef(`EMULATE_WEIRD_STUFF',
		`
			! Code marked with CG is from Charles Gallagher, to fix the
			! Spectrum game "Quazatron"
		')
	')

	DEF_LN

`ldxr_loop'ln:
!vbtvbt
	MEMREAD(`shlr16 r0',`mov.l zHL,r0')
!vbtvbt
!        shlr16 r0
	ifdef(`EMULATE_BITS_3_5',`! TODO')
!vbtvbt        
	MEMWRITE(`shlr16 r0',`mov.l zDE,r0            ! r0 = DE register')
	
	mov.l zHL,DIRT_REG      ! DIRT_REG = HL register
	mov.l zBC,TMP_REG       ! TMP_REG = BC register
!vbtvbt 
        shlr16 DIRT_REG
        shlr16 TMP_REG        

	ifelse($1,`I',
	`
		add #1,r0
		add #1,DIRT_REG
	',
	`
		add #-1,r0
		add #-1,DIRT_REG
	')

	dt TMP_REG               ! decrement BC
	mov.w r0,zDE             ! writeback zDE register
!vbtvbt utile car décrémenté       
!	extu.w DIRT_REG,DIRT_REG ! no sign extension required
!	extu.w TMP_REG,TMP_REG   ! no sign extension required
!vbtvbt        
!	mov.l TMP_REG,zBC        ! writeback zBC register
        shll16 TMP_REG
        shll16 DIRT_REG                
	mov.l TMP_REG,zBC        ! writeback zBC register
	bt/s `ldxr_zero'ln
!	mov.l DIRT_REG,zHL       ! writeback zHL register
        mov.l DIRT_REG,zHL       ! writeback zHL register
	
	add #-EXTRA_CYCLES,ICOUNT_REG
	ifdef(`EMULATE_R_REGISTER',`add #1,zR')
	cmp/pl ICOUNT_REG
	bt `ldxr_loop'ln

	! TODO: hacer un set posicional y otro no posicional
!	SET_iVN(`ld')
	mov #FLG_iV,zF
!	mov #(FLAG_S|FLAG_Z|FLAG_C),DIRT_REG
!	and DIRT_REG,zF

	ifdef(`EMULATE_BITS_3_5',
	`
		! TODO
	')
	mov #FLG_P,DIRT_REG
	add #-2,zPC              ! rewind to instruction start
	EXIT(`or DIRT_REG,zF')
	
ALIGN
`ldxr_zero'ln:
	mov #FLG_iV,zF
	ifdef(`EMULATE_BITS_3_5',
	`
		! TODO
	')
')


! In from BC into (HL), xcrease HL, decrease B
! INX operation
define(`INX',
`
	! In from BC
	mov.b zC,r0
	extu.b r0,r0            ! high byte clear? (documentation is fuzzy)
	IOREAD(`r0')
	mov r0,TMP_REG

	ifdef(`EMULATE_WEIRD_STUFF',`SAVE_REG(`TMP_REG')')

	! Decrease B
	mov.b zB,r0
	add #-1,r0
	mov.b r0,zB
	DO_BITS_3_5(`r0')
	SET_ZSP(`r0')
	
	! Write to (HL)
!vbtvbt        
	MEMWRITE(`mov.w zHL,r0')

	! Writeback HL register
	ifelse($1,`I',`add #1,r0',`add #-1,r0')
	mov.w r0,zHL

    ! This is a prime example of the weirdness of the Z80...
	ifdef(`EMULATE_WEIRD_STUFF',
	`
		mov.b zC,r0
		extu.b r0,r0
		RESTORE_REG(`TMP_REG')
		extu.b TMP_REG,TMP_REG
		add #1,r0
		add TMP_REG,r0
		shlr8 r0          ! Test to see if there is any data in the upper bits
		tst r0,r0
		bt `no_carry'ln
		
		mov #(FLAG_H|FLAG_C),DIRT_REG
		or DIRT_REG,zF
		
	`no_carry'ln:
		mov TMP_REG,r0
		ifdef(`SH2_COMPAT_CODE',
		`
			shlr2 r0
			shlr2 r0
			shlr2 r0
		',
		`
			mov #-6,DIRT_REG
			shld DIRT_REG,r0
		')
		and #FLAG_N,r0
		or r0,zF
	')
')

! In from BC into (HL), xcrease HL, decrease B, repeat
! INXR operation
define(`INXR',
`
 DEF_LN
`linxr_loop'ln:
	! In from BC
	mov.b zC,r0
	extu.b r0,r0            ! high byte clear? (documentation is fuzzy)
	IOREAD(`r0')
	mov r0,TMP_REG

	ifdef(`EMULATE_WEIRD_STUFF',`SAVE_REG(`TMP_REG')')

	! Decrease B
	mov.b zB,r0
	add #-1,r0
	mov.b r0,zB
	DO_BITS_3_5(`r0')
	SET_ZSP(`r0')
	
	! Write to (HL)
!vbtvbt        
	MEMWRITE(`mov.w zHL,r0')

	! Writeback HL register
	ifelse($1,`I',`add #1,r0',`add #-1,r0')
	mov.w r0,zHL

    ! This is a prime example of the weirdness of the Z80...
	ifdef(`EMULATE_WEIRD_STUFF',
	`
		mov.b zC,r0
		extu.b r0,r0
		RESTORE_REG(`TMP_REG')
		extu.b TMP_REG,TMP_REG
		add #1,r0
		add TMP_REG,r0
		shlr8 r0          ! Test to see if there is any data in the upper bits
		tst r0,r0
		bt `no_carry'ln
		
		mov #(FLAG_H|FLAG_C),DIRT_REG
		or DIRT_REG,zF

		
	cmp/pl ICOUNT_REG
	bt `linxr_loop'ln        


	`no_carry'ln:
		mov TMP_REG,r0
		ifdef(`SH2_COMPAT_CODE',
		`
			shlr2 r0
			shlr2 r0
			shlr2 r0
		',
		`
			mov #-6,DIRT_REG
			shld DIRT_REG,r0
		')
		and #FLAG_N,r0
		or r0,zF
	')

ALIGN
	`_z80_In_'ln: .long _z80_Out
ALIGN
`linxr_zero'ln:
nop


')

! Out from (HL) into BC, xcrease HL, decrease B
! OUTX operation
define(`OUTX',
`
	! Read from (HL)
!vbtvbt        
	MEMREAD(`shlr16 r0',`mov.l zHL,r0')

	ifdef(`EMULATE_WEIRD_STUFF',`SAVE_REG(`TMP_REG')')

	! Writeback HL register
	ifelse($1,`I',`add #1,r0',`add #-1,r0')
	mov.w r0,zHL
	
	! Out to BC
	mov.b zC,r0
	extu.b r0,r0            ! high byte clear? (documentation is fuzzy)
	IOWRITE(`r0',`TMP_REG')

	! Decrease B
	mov.b zB,r0
	add #-1,r0
	mov.b r0,zB
	SET_ZSP(`r0')
	DO_BITS_3_5(`r0')

	! This is a prime example of the weirdness of the Z80...
	ifdef(`EMULATE_WEIRD_STUFF',
	`
		mov.b zC,r0
		extu.b r0,r0
		RESTORE_REG(`TMP_REG')
		extu.b TMP_REG,TMP_REG
		add #1,r0
		add TMP_REG,r0
		shlr8 r0          ! Test to see if there is any data in the upper bits
		tst r0,r0
		bt `no_carry'ln
		
		mov #(FLAG_H|FLAG_C),DIRT_REG
		or DIRT_REG,zF
		
	`no_carry'ln:
		mov TMP_REG,r0

		ifdef(`SH2_COMPAT_CODE',
		`
			shlr2 r0
			shlr2 r0
			shlr2 r0
		',
		`
			mov #-6,DIRT_REG
			shld DIRT_REG,r0
		')

		and #FLAG_N,r0
		or r0,zF
	')
')

! Out from (HL) into BC, xcrease HL, decrease B, repeat
! OTXR operation
define(`OTXR',
`
DEF_LN
!!`lotxr_loop'ln:
!vbtvbt        
	! Read from (HL)
        MEMREAD(`shlr16 r0',`mov.l zHL,r0')

	ifdef(`EMULATE_WEIRD_STUFF',`SAVE_REG(`TMP_REG')')

	! Writeback HL register
	ifelse($1,`I',`add #1,r0',`add #-1,r0')
	mov.w r0,zHL
	
	! Out to BC
	mov.b zC,r0
	extu.b r0,r0            ! high byte clear? (documentation is fuzzy)
!vbt à corriger        
	IOWRITE(`r0',`TMP_REG')

	! Decrease B
	mov.b zB,r0
	dt r0
	bt/s `lotxr_zero'ln
	mov.b r0,zB

	add #-5,ICOUNT_REG        
	add #-2,zPC
	bra `lotxr_end'ln:

`lotxr_zero'ln:
	SET_ZSP(`r0')
	DO_BITS_3_5(`r0')

ALIGN
`lotxr_end'ln:

DO_CYCLES_NEXT

!ALIGN
	`_z80_Out_'ln: .long _z80_Out
')



! Shift left logically/arithmetically
! SHL operation
define(`SHLX',
`
	sh$1l r0
	mov #0,zF
	movt FLAGS_C   ! C
	ifelse($1,`l',`add #1,r0')
!	exts.b r0,r0
')

! Move from (HL) to (DE), post-xcrement
! LDX operation
define(`LDX',
`
	! Read from (HL)
!vbtvbt        
	MEMREAD(`mov.w zHL,r0')
	! Post-xcrement HL register
	ifelse($1,`I',`add #1,r0',`add #-1,r0')
	mov.w r0,zHL

	! Write to (DE)
!vbtvbt        
MEMWRITE(`mov.w zDE,r0')        
	! Post-xcrement DE register
	ifelse($1,`I',`add #1,r0',`add #-1,r0')
	mov.w r0,zDE

	! Flag calculation
	LDX_FLAGS

	DO_CYCLES_NEXT
')

! Pop the PC
! Trashes r0 and TMP_REG
define(`POP_PC',
`
!vbtvbt
	MEMREAD(`mov.w zSP,r0')      ! Read the 1st byte of the PC
	extu.b TMP_REG,zPC
	MEMREAD(`add #1,r0')         ! Read the 2nd byte of the PC
	extu.b TMP_REG,TMP_REG
	add #1,r0
	shll8 TMP_REG
	mov.w r0,zSP                 ! Writeback SP
	or TMP_REG,zPC
ifdef(`BASED_PC',`add FETCH_REG,ezPC')    ! Base PC
')

!----------------------------------------------------------------------------!
! Here are the interface functions:

! Move a register to another
! LD_R_R d8, s8      s8 = src reg, d8 = dest reg
define(`LD_R_R',
`
	! Si el registro origen y destino es el mismo, nop
	ifelse($1,$2,`',
	`
		ifelse($1,`A',
		`
			mov.b z$2,r0
			mov r0,z$1
		',
		`
			ifelse($2,`A',`mov z$2,r0',`mov.b z$2,r0')
			mov.b r0,z$1
		')
	')
	
	DO_CYCLES_NEXT
')

! Move a register to another (IXh/IXl)
! LD_XYr_XYr d8, s8      s8 = src reg, d8 = dest reg
define(`LD_Xr_Xr',
`
	LD_R_R($1,$2)
')

! Move a register to another (IXh/IXl)
! LD_XYr_XYr d8, s8      s8 = src reg, d8 = dest reg
define(`LD_Yr_Yr',
`
	DEF_LN

	! Source register loading
	ifelse($2,`IYh',
	`
		LOAD_YREG
	',
	`
		ifelse($2,`IYl',
		`
			LOAD_YREG
		',
		`
			LOAD_ZREG($2)
		')
	')

	! Destination register writing
	ifelse($1,`IYh',
	`
		mov.l `_z80_IY2_'ln,TMP_REG
		mov.b r0,@TMP_REG
	',
	`
		ifelse($1,`IYl',
		`
			mov.l `_z80_IY2_'ln,TMP_REG
			mov.b r0,@TMP_REG
		',
		`
			WRITE_ZREG($1)
		')
	')

	DO_CYCLES_NEXT
	
ALIGN_DATA
	ifelse($1,`IYh',
	`
		`_z80_IY2_'ln: .long _z80_IY + 1
	',
	`
		ifelse($1,`IYl',
		`
			`_z80_IY2_'ln: .long _z80_IY
		')
	')
	GEN_IY_TAG($2)
')


! Move an immediate byte into a register
! LD_R_N d8      d8 = dest reg
define(`LD_R_N',
`
	ifelse($1,`A',
	`
		GETDISP(`z$1')
	',
	`
		GETDISP(`r0')
		mov.b r0,z$1
	')

	DO_CYCLES_NEXT
')

! Move an immediate byte into IY register
! LD_R_N d8      d8 = dest reg
define(`LD_IY_N',
`
	DEF_LN

	GETDISP(`r0')
	WRITE_YREG

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_IY_TAG($1)
')

! Move from (RR) into a register
! LD_R_mRR d8, s16      d8 = dest reg, s16 = src reg
define(`LD_R_mRR',
`
!vbtvbt
	MEMREAD(`mov.w z$2,r0')

	ifelse($1,`A',
	`
		mov TMP_REG,z$1
	',
	`
		mov TMP_REG,r0
		mov.b r0,z$1
	')

	DO_CYCLES_NEXT
')

! Move from (XY+dd) into a register
! LD_R_mXY d8, s16      d8 = dest reg, s16 = src reg
define(`LD_R_mXY',
`
	GETDISP(`TMP_REG')
	mov.w z$2,r0          ! Ojo, carga con extension
	MEMREAD(`add TMP_REG,r0')
	
	ifelse($1,`A',
	`
		mov TMP_REG,z$1
	',
	`
		mov TMP_REG,r0
		mov.b r0,z$1
	')

	DO_CYCLES_NEXT
')

! Move from a register into (RR)
! LD_mRR_R d16, s8      d16 = destreg, s8 = src reg
define(`LD_mRR_R',
`
	ifelse($2,`A',
	`
		mov.l z$1,r0
		MEMWRITE(`shlr16 r0',`mov z$2,TMP_REG')
	',
	`
		mov.b z$2,r0
		mov r0,TMP_REG
!vbtvbt                
		MEMWRITE(`shlr16 r0',`mov.l z$1,r0')
	')

	DO_CYCLES_NEXT
')

! Move from a register into (XY+dd)
! LD_mXY_R d16, s8      d16 = dest reg, s8 = src reg
define(`LD_mXY_R',
`
	ifelse($2,`A',
	`
		GETDISP(`TMP_REG')
		mov.w z$1,r0         ! Ojo, carga con extension
		add TMP_REG,r0
		MEMWRITE(`mov z$2,TMP_REG')
	',
	`
		mov.b z$2,r0
		mov r0,TMP_REG
		GETDISP(`DIRT_REG')
		mov.w z$1,r0        ! Ojo, carga con extension
		add DIRT_REG,r0
		MEMWRITE(`nop')
	')

	DO_CYCLES_NEXT
')

! Move an immediate into (RR)
! LD_mRR_N d16          d16 = dest reg
define(`LD_mRR_N',
`
	GETDISP(`TMP_REG')

	MEMWRITE(`shlr16 r0',`mov.l z$1,r0')

	DO_CYCLES_NEXT
')

! Move an immediate into (XY+dd)
! LD_mXY_N d16          d16 = dest reg
define(`LD_mXY_N',
`
	GETDISP(`DIRT_REG')     ! DIRT_REG = offset
	GETDISP(`TMP_REG')      ! TMP_REG = inmediate data
	mov.w z$1,r0            ! Ojo, carga con extension
	add DIRT_REG,r0
	MEMWRITE(`nop')

	DO_CYCLES_NEXT
')

! Move from (NN) into a register
! LD_R_mNN d8          d8 = dest reg
define(`LD_R_mNN',
`
	GETWORD(`r0')
	MEMREAD(`nop')
	ifelse($1,`A',
	`
		mov TMP_REG,z$1

	',
	`
		mov TMP_REG,r0
		mov.b r0,z$1
	')

	DO_CYCLES_NEXT
')

! Move from a register into (NN)
! LD_mNN_R s8          s8 = src reg
define(`LD_mNN_R',
`
	GETWORD(`r0')
	ifelse($1,`A',
	`
		MEMWRITE(`mov z$1,TMP_REG')
	',
	`
		MEMWRITE(`mov.l z$1,TMP_REG')
	')
	DO_CYCLES_NEXT
')

! Move the Interrupt register into a register
! LD_R_I d8             d8 = dest reg
define(`LD_R_I',
`
	DEF_LN

	mov #_z80_I-REF,r0
	mov #FLG_iV,zF         ! prepare to set V and keep the old carry
	mov.b @(r0,REF_REG),r0  ! r0 = zI
	ifelse($1,`A',`mov r0,z$1',`mov.b r0,z$1')
	SET_ZSP(`r0')
	mov #_z80_IFF2-REF,r0
	mov.b @(r0,REF_REG),r0  ! r0 = zIFF2
	or r0,zF                ! put P/V flag

	DO_CYCLES_NEXT
')

! Move a register into the Interrupt register
! LD_I_R s8             s8 = src reg
define(`LD_I_R',
`
	ifelse($1,`A',
	`
		mov #_z80_I-REF,r0
		mov.b z$1,@(r0,REF_REG)
	',
	`
		mov.b z$1,r0
		mov r0,TMP_REG
		mov #_z80_I-REF,r0
		mov.b TMP_REG,@(r0,REF_REG)
	')

	DO_CYCLES_NEXT
')

! Move the Refresh register into a register
! LD_R_Rf d8             d8 = dest reg
define(`LD_R_Rf',
`
	DEF_LN

ifdef(`EMULATE_R_REGISTER',
`
	mov.l `_z80_R2_'ln,r0
	mov.b @r0,r0             ! r0 = R2
	mov #0x7F,DIRT_REG       ! R mask
	and #0x80,r0             ! take bits 0-6 from R, bit 7 from R2		
',
`
	! attempt to fake the R-register
	! take the ICount, invert it, divide by 4
	! this produces an increasing counter, roughly proportional to
	! the number of instructions done
	mov.l `_z80_R2_'ln,r0
	mov ICOUNT_REG,TMP_REG
	mov.b @r0,r0             ! r0 = R2
	mov #0x7F,DIRT_REG       ! R mask
	not TMP_REG,TMP_REG
	and #0x80,r0             ! take bits 0-6 from R, bit 7 from R2
	shlr2 TMP_REG
')

	and zR,DIRT_REG          ! DIRT_REG = R masked
	or DIRT_REG,r0           ! r0 = result
	exts.b r0,r0

	! Cargar resultado
	ifelse($1,`A',`mov r0,z$1',`mov.b r0,z$1')

	DO_BITS_3_5(`r0')
	SET_ZSP(`r0')

	mov.l `_z80_IFF2_'ln,r0
	mov #FLG_iV,zF
	mov.b @r0,r0            ! r0 = zIFF2
	or r0,zF                ! put P/V flag

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT(`_z80_IFF2',ln)
	GEN_MT(`_z80_R2',ln)
')

! Move a register into the Refresh register
! LD_Rf_R s8             s8 = src reg
define(`LD_Rf_R',
`
	DEF_LN
	ifdef(`EMULATE_R_REGISTER',
	`
		ifelse($1,`A',
		`
			mov z$1,zR
		',
		`
			mov.b z$1,r0
			mov r0,zR
		')

		mov.l `_z80_R2_'ln,DIRT_REG
		ifelse($1,`A',`mov.b z$1,@DIRT_REG',`mov.b r0,@DIRT_REG')
	')

	DO_CYCLES_NEXT

	ifdef(`EMULATE_R_REGISTER',
	`
	ALIGN_DATA
		GEN_MT(`_z80_R2',ln)
	')
')

! Move a 16-bit immediate into a register
! LD_RR_NN d16             d16 = dest reg
define(`LD_RR_NN',
`
	GETWORD(`r0')
	mov.w r0,z$1
	DO_CYCLES_NEXT
')

! Move from (NN) into a 16-bit register
! LD_RR_mNN d16             d16 = dest reg
define(`LD_RR_mNN',
`
	GETWORD(`r0')
	MEMREAD(`nop')
	SAVE_REG(`TMP_REG')
	MEMREAD(`add #1,r0')
	RESTORE_REG(`r0')
	shll8 TMP_REG
	extu.b r0,r0
	or TMP_REG,r0
   
	mov.w r0,z$1
	DO_CYCLES_NEXT
')

! Move from a 16-bit register into (NN)
! LD_mNN_RR s16             s16 = src reg
define(`LD_mNN_RR',
`
	GETWORD(`r0')    ! r0 = address

	MEMWRITE(`shlr16 TMP_REG',`mov.l z$1,TMP_REG    ! data')
	mov.l z$1,TMP_REG
	shlr16 TMP_REG
	add #1,r0
	MEMWRITE(`shlr8 TMP_REG')

	DO_CYCLES_NEXT
')

! Move from IY 16-bit register into (NN)
! LD_mNN_IY
define(`LD_mNN_IY',
`
	GETWORD(`TMP_REG')        ! TMP_REG = address
	mov.w zIY,r0              ! data
	mov TMP_REG,DIRT_REG
	SAVE_REG(`r0')            ! save data for 2nd write
	mov r0,TMP_REG
	mov DIRT_REG,r0
	MEMWRITE(`nop')
	RESTORE_REG(`TMP_REG')
	shlr8 TMP_REG
	MEMWRITE(`add #1,r0')

	DO_CYCLES_NEXT
')

! Move from a 16-bit register into another
! LD_RR_RR d16, s16             s16 = src reg, d16 = dest reg
define(`LD_RR_RR',
`
	mov.w z$2,r0
	mov.w r0,z$1

	DO_CYCLES_NEXT
')

! Move from a 16-bit register into another
! LD_RR_XY d16, s16             s16 = src reg, d16 = dest reg
define(`LD_RR_XY',
`
	LD_RR_RR($1,$2)
')

! Push a word on the stack
! PUSH_RR      s16               s16 = src reg
define(`PUSH_RR',
`
	DEF_LN

	ifelse($1,`AF',
	`
!	it af
		mov.w zSP,r0
		mov zA,TMP_REG

		!totovbt
		MEMWRITE(`add #-1,r0')       ! First write

		SAVE_REG(`zF')
		WRITEBACK_CCR(`TMP_REG')
		mov.w zSP,r0
		RESTORE_REG(`zF')
		MEMWRITE(`add #-2,r0')       ! Second write
	',
	`
!	not af
		mov.w zSP,r0    ! address
		mov.l z$1,TMP_REG
!vbtvbt
		shlr16 TMP_REG
		add #-1,r0
		MEMWRITE(`shlr8 TMP_REG')    ! First write
		mov.l z$1,TMP_REG
!vbtvbt
		shlr16 TMP_REG
		MEMWRITE(`add #-1,r0')     ! Second write
	')

	! Writeback SR
	mov.w r0,zSP

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT(`Byte_Flags',ln)
')

! Pop a word from the stack
! POP_RR      s16               s16 = src reg
define(`POP_RR',
`
!vbtvbt
	MEMREAD(`shlr16 r0',`mov.l zSP,r0')
	ifelse($1,`AF',
	`
		DO_BITS_3_5(`TMP_REG')
		mov TMP_REG,zF
		CACHE_CCR

		mov.l zSP,r0
!vbtvbt
		shlr16 r0
		MEMREAD(`add #1,r0')
		mov TMP_REG, zA
		add #1,r0
		! Writeback SR
		mov.w r0,zSP
	',
	`
		SAVE_REG(`TMP_REG')
		MEMREAD(`add #1,r0')
		RESTORE_REG(`DIRT_REG')
		extu.b TMP_REG,TMP_REG
		extu.b DIRT_REG,DIRT_REG
		add #1,r0
		shll8 TMP_REG
		! Writeback SR
		mov.w r0,zSP
		or DIRT_REG,TMP_REG
		mov TMP_REG,r0
		mov.w r0,z$1
	')

	DO_CYCLES_NEXT
')


! Pop a word from the stack to IY
! POP_IY
define(`POP_IY',
`
!vbtvbt
	MEMREAD(`shlr16 r0',`mov.l zSP,r0')
	SAVE_REG(`TMP_REG')
	MEMREAD(`add #1,r0')
	add #1,r0
	extu.b TMP_REG,TMP_REG
	! Writeback SR
	mov.w r0,zSP
	RESTORE_REG(`r0')
	extu.b r0,r0
	shll8 TMP_REG
	or TMP_REG,r0
	mov.w r0,zIY

	DO_CYCLES_NEXT
')


! Exchange 2 16-bit registers
! EX_RR_RR      s16, d16      s16 = src reg, d16 = dest reg
define(`EX_RR_RR',
`
	mov.l z$1,r0
	mov.l z$2,TMP_REG

	mov.l r0,z$2
	mov.l TMP_REG,z$1
	shlr16 r0
	shlr16 TMP_REG

	DO_CYCLES_NEXT
')

! Exchange AF with AF'
! EX_AF_AF2
! TODO: Banderas 3-5
define(`EX_AF_AF2',
`
	WRITEBACK_CCR(`r0')

	mov zA,TMP_REG
	shll8 TMP_REG
	extu.w TMP_REG,TMP_REG
	or r0,TMP_REG     ! TMP_REG = AF

	mov.l zAF2,zF
	mov zF,zA
	DO_BITS_3_5(`zF')
	CACHE_CCR         ! F = F2
	shlr8 zA          ! A = A2
	mov.l TMP_REG,zAF2    ! AF2 = AF

!	extu.b DIRT_REG,zF    ! F = F2
!	shlr8 DIRT_REG
!	DO_BITS_3_5(`zF')
!	mov DIRT_REG,zA       ! A = A2
!	mov.l TMP_REG,zAF2    ! AF2 = AF

    DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT(`Byte_Flags',ln)
')

! Exchange the 16-bit registers with their primes
! EXX
define(`EXX',
`
	! BC
	mov.l zBC,r0
	mov.l zBC2,TMP_REG
	mov.l TMP_REG,zBC
	mov.l r0,zBC2
!	shlr16 r0
!	shlr16 TMP_REG
	
	! DE
	mov.l zDE,r0
	mov.l zDE2,TMP_REG
	mov.l TMP_REG,zDE
	mov.l r0,zDE2
!	shlr16 r0
!	shlr16 TMP_REG
	
	! HL
	mov.l zHL,r0
	mov.l zHL2,TMP_REG
	mov.l TMP_REG,zHL
	mov.l r0,zHL2
!	shlr16 r0
!	shlr16 TMP_REG
	
	DO_CYCLES_NEXT
')

! Exchange a 16-bit register with (RR)
! EX_RR_mRR     d16, m16      d16 = dest reg, m16 = memory reg
define(`EX_RR_mRR',
`
!vbtvbt
! à remettre ?
	MEMREAD(`shlr16 r0',`mov.l z$2,r0')      ! Load memory reg
!	MEMREAD(`mov.l z$2,r0')      ! Load memory reg

	SAVE_REG(`TMP_REG')           ! save low byte read
	MEMWRITE(`shlr16 TMP_REG',`mov.l z$1,TMP_REG')
	MEMREAD(`add #1,r0')
	SAVE_REG(`TMP_REG')           ! save high byte read
	mov.l z$1,TMP_REG
	shlr16 TMP_REG
	MEMWRITE(`shlr8 TMP_REG')   
	RESTORE_REG(`r0')                ! restore high byte read
	shll8 r0
	RESTORE_REG(`TMP_REG')
	extu.b TMP_REG,TMP_REG
	or TMP_REG,r0
	mov.w r0,z$1

	DO_CYCLES_NEXT
')


! Exchange IY 16-bit register with (RR)
! EX_IY_mRR     m16      m16 = memory reg
define(`EX_IY_mRR',
`
!vbtvbt
!        MEMREAD(`shlr16 r0',`mov.w z$1,r0')       ! Load memory reg

        MEMREAD(`mov.w z$1,r0')       ! Load memory reg
	SAVE_REG(`TMP_REG')           ! save low byte read
	mov.w zIY,r0
	mov r0,TMP_REG
!vbtvbt        
	MEMWRITE(`mov.w z$1,r0')
	mov.w z$1,r0
	MEMREAD(`add #1,r0')
	SAVE_REG(`TMP_REG')           ! save high byte read
	mov.w zIY,r0
	mov r0,TMP_REG
	mov.w z$1,r0
	add #1,r0
	MEMWRITE(`shlr8 TMP_REG')
	RESTORE_REG(`r0')                ! restore high byte read
	shll8 r0
	RESTORE_REG(`TMP_REG')
	extu.b TMP_REG,TMP_REG
	or TMP_REG,r0
	mov.w r0,zIY

	DO_CYCLES_NEXT
')


! Move from (HL) to (DE), post-increment
! LDI
define(`LDI',
`
	LDX(`I')
')

! Move from (HL) to (DE), post-decrement
! LDD
define(`LDD',
`
	LDX(`D')
')

! Move from (HL) to (DE), post-increment, repeat
! LDIR
define(`LDIR',
`
	LDXR(`I')

	DO_CYCLES_NEXT
')

! Move from (HL) to (DE), post-decrement, repeat
! LDDR
define(`LDDR',
`
	LDXR(`D')

	DO_CYCLES_NEXT
')

! Compare A against (HL), post-increment
! CPI
define(`CPI',
`
	DEF_LN

	CPX(`I')

	DO_CYCLES_NEXT
')

! Compare A against (HL), post-decrement
! CPD
define(`CPD',
`
	DEF_LN

	CPX(`D')

	DO_CYCLES_NEXT
')

! Compare A against (HL), post-increment, repeat
! CPIR
define(`CPIR',
`
	DEF_LN

	CPXR(`I')

	DO_CYCLES_NEXT
')

! Compare A against (HL), post-decrement, repeat
! CPDR
define(`CPDR',
`
	DEF_LN

	CPXR(`D')

	DO_CYCLES_NEXT
')

! Perform arithmetic on the A register
! ART_R method, src8
define(`ART_R',
`
	DEF_LN

	DO_ART($1,$2)

	DO_CYCLES_NEXT
')

! Perform arithmetic on the A register
! ART_IY method, src8
define(`ART_IY',
`
	DEF_LN
	
	LOAD_YREG
	DO_ART($1,`r0')

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_IY_TAG($2)
')


! Perform arithmetic on the A register (using an immediate)
! ART_N method
define(`ART_N',
`
	DEF_LN

	GETDISP(`r0')
	DO_ART($1,`r0')

	DO_CYCLES_NEXT
')

! Perform arithmetic on the A register, using (XY)
! ART_mXY method, src16
define(`ART_mXY',
`
	DEF_LN

	mov.w z$2,r0
	mov r0,TMP_REG
	GETDISP(`r0')
	MEMREAD(`add TMP_REG,r0')
	mov TMP_REG,r0
	DO_ART($1,`r0')

	DO_CYCLES_NEXT
')

! Perform arithmetic on the A register using (RR)
! ART_mRR method, src16
define(`ART_mRR',
`
	DEF_LN
!vbtvbt
	MEMREAD(`shlr16 r0',`mov.l z$2,r0')
	mov TMP_REG,r0
	DO_ART($1,`r0')

	DO_CYCLES_NEXT
')

! Perform logic on the A register
! LOG_R method, src8
define(`LOG_R',
`
	DEF_LN

	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	ifelse($2,`A',
	`
		DO_LOG($1,$2)
	',
	`
		LOAD_ZREG($2)
		DO_LOG($1,`r0')
	')

	DO_CYCLES_NEXT
')

! Perform logic on the A register
! LOG_IY method
define(`LOG_IY',
`
	DEF_LN

	LOAD_YREG
	DO_LOG($1,`r0')

	DO_CYCLES_NEXT

ALIGN_DATA
	`_z80_IY_'ln: .long `_z80_IY'ifelse($1,`IYh',`+1')
')

! Perform logic on the A register (using an immediate)
! LOG_N method
define(`LOG_N',
`
	DEF_LN

	GETDISP(`r0')
	DO_LOG($1,`r0')

	DO_CYCLES_NEXT
')

! Perform logic on the A register, using (RR)
! LOG_mRR method, src16
define(`LOG_mRR',
`
	DEF_LN
	MEMREAD(`mov.w z$2,r0')
	mov TMP_REG,r0
	DO_LOG($1,`r0')

	DO_CYCLES_NEXT
')

! Perform logic on the A register, using (XY+dd)
! LOG_mXY method, src16
define(`LOG_mXY',
`
	DEF_LN

	GETDISP(`TMP_REG')
	mov.w z$2,r0
	MEMREAD(`add TMP_REG,r0')
	mov TMP_REG,r0
	DO_LOG($1,`r0')

	DO_CYCLES_NEXT
')

! Compare against the A register
! CP_R src8
define(`CP_R',
`
	DEF_LN

	LOAD_ZREG($1)
	DO_CP

	DO_CYCLES_NEXT
')

! Compare IY against the A register
! CP_R src8
define(`CP_IY',
`
	DEF_LN

	LOAD_YREG
	DO_CP

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_IY_TAG($1)
')

! Compare against the A register, using an immediate
! CP_N
define(`CP_N',
`
	DEF_LN

	GETDISP(`r0')
	DO_CP

	DO_CYCLES_NEXT
')

! Compare against the A register, using (RR)
! CP_mRR src16
define(`CP_mRR',
`
	DEF_LN
!vbtvbt
	MEMREAD(`shlr16 r0',`mov.l z$1,r0')
	mov TMP_REG,r0
	DO_CP
	
	DO_CYCLES_NEXT
')

! Compare against the A register, using (XY+dd)
! CP_mXY src16
define(`CP_mXY',
`
	DEF_LN

	GETDISP(`TMP_REG')
	mov.w z$1,r0
	MEMREAD(`add TMP_REG,r0')
	mov TMP_REG,r0
	DO_CP

	DO_CYCLES_NEXT
')

! Inc/dec a register
! DECINC_R method, reg8
define(`DECINC_R',
`
	DEF_LN

	LOAD_ZREG_S($2)
	DO_$1
	WRITE_ZREG($2)

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT($1`_Table',ln)
')


! Inc/dec IY register
! DECINC_IY method, reg8
define(`DECINC_IY',
`
	DEF_LN

	LOAD_YREG
	DO_$1
	WRITE_YREG

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT($1`_Table',ln)
	GEN_IY_TAG($2)
')


! Inc/dec (RR)
! DECINC_mRR method
define(`DECINC_mRR',
`
	DEF_LN
!vbtvbt
	MEMREAD(`shlr16 r0',`mov.l z$2,r0')
	exts.b TMP_REG,r0         ! requires to be sign-extended
	DO_$1
	mov r0,TMP_REG
	MEMWRITE(`shlr16 r0',`mov.l z$2,r0')

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT($1`_Table',ln)
')

! Inc/dec (XY+dd)
! DECINC_mXY method
define(`DECINC_mXY',
`
	DEF_LN

	GETDISP(`TMP_REG')
	mov.w z$2,r0
	add TMP_REG,r0
	MEMREAD(`SAVE_REG(`r0')')
	exts.b TMP_REG,r0          ! requires to be sign-extended
	DO_$1
	mov r0,TMP_REG
	MEMWRITE(`RESTORE_REG(`r0')')

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT($1`_Table',ln)
')

! Add two 16-bit registers
! ADD_RR_RR dest16, src16
define(`ADD_RR_RR',
`
	DO_ADD16($1,$2)

	DO_CYCLES_NEXT
')

! Add two 16-bit registers, with carry
! ADC_RR_RR dest16, src16
define(`ADC_RR_RR',
`
	DO_ADC16($1,$2)

	DO_CYCLES_NEXT
')

! Subtract two 16-bit registers, with carry
! SBC_RR_RR dest16, src16
define(`SBC_RR_RR',
`
	DO_SBC16($1,$2)

	DO_CYCLES_NEXT
')

! Inc/dec a 16-bit register
! DECINC_RR method, reg16
define(`DECINC_RR',
`
	mov.w z$2,r0
	ifelse($1,`INC',`add #1,r0',`add #-1,r0')
	mov.w r0,z$2

	DO_CYCLES_NEXT
')

! Rotate a register
! ROT_R method, reg8
define(`ROT_R',
`
	LOAD_ZREG_S($2)
	DO_ROT($1)
	WRITE_ZREG($2)

	DO_CYCLES_NEXT
')

! Rotate (RR)
! ROT_mRR method, src16
define(`ROT_mRR',
`
!vbtvbt
	MEMREAD(`shlr16 r0',`mov.l z$2,r0')
	exts.b TMP_REG,r0
	DO_ROT($1)
	mov r0,TMP_REG
	MEMWRITE(`shlr16 r0',`mov.l z$2,r0')

	DO_CYCLES_NEXT
')

! Rotate (XY+dd)
! ROT_mXY method
define(`ROT_mXY',
`
	MEMREAD(`SAVE_REG(`r0')')
	exts.b TMP_REG,r0
	DO_ROT($1)
	mov r0,TMP_REG
	MEMWRITE(`RESTORE_REG(`r0')')

	DO_CYCLES_NEXT
')

! Rotate (XY+dd), and store into a register
! ROT_mXY_R method, dest8
define(`ROT_mXY_R',
`
	MEMREAD(`SAVE_REG(`r0')')
	exts.b TMP_REG,r0
	DO_ROT($1)

	! Write result in a register
	ifelse($2,`A',`mov r0,z$2',`mov.b r0,z$2')

	mov r0,TMP_REG
	MEMWRITE(`RESTORE_REG(`r0')')
	
	DO_CYCLES_NEXT
')

! Shift a register
! SHF_R method, reg8
define(`SHF_R',
`
	DEF_LN

	LOAD_ZREG_S($2)
	DO_SHF($1)
	WRITE_ZREG($2)

	DO_CYCLES_NEXT
')

! Shift (RR)
! SHF_mRR method, src16
define(`SHF_mRR',
`
	DEF_LN
!vbtvbt
	MEMREAD(`shlr16 r0',`mov.l z$2,r0')
	exts.b TMP_REG,r0
	DO_SHF($1)
	mov r0,TMP_REG
	MEMWRITE(`shlr16 r0',`mov.l z$2,r0')

	DO_CYCLES_NEXT
')

! Shift (XY+dd)
! SHF_mXY method
define(`SHF_mXY',
`
	DEF_LN

	MEMREAD(`SAVE_REG(`r0')')
	exts.b TMP_REG,r0
	DO_SHF($1)
	mov r0,TMP_REG
	MEMWRITE(`RESTORE_REG(`r0')')

	DO_CYCLES_NEXT
')

! Shift (XY+dd), and store into a register
! SHF_mXY_R method, dest8
define(`SHF_mXY_R',
`
	DEF_LN

	MEMREAD(`SAVE_REG(`r0')')
	exts.b TMP_REG,r0
	DO_SHF($1)
	WRITE_ZREG($2)
	mov r0,TMP_REG
	MEMWRITE(`RESTORE_REG(`r0')')

	DO_CYCLES_NEXT
')

! Rotate A/(HL) left, via nibbles
! RLD
define(`RLD',
`
	DEF_LN
!vbtvbt	
	MEMREAD(`shlr16 r0',`mov.l zHL,r0')

	mov #0,zF
	mov #0x0f,r0               ! mask for A register value
	and zA,r0                     ! r0 = low order bits of A register
	mov #0xf0,DIRT_REG   ! mask for high order bits of memory operand
	extu.b DIRT_REG,DIRT_REG
	and TMP_REG,DIRT_REG ! DIRT_REG = high order bits of memory operand
	shlr2 DIRT_REG
	shll2 TMP_REG
	shlr2 DIRT_REG
	shll2 TMP_REG
	or r0,TMP_REG        ! merge in low order bits of A register
	mov #0xf0,r0
	and r0,zA                ! isolate high order bits of A register
	or DIRT_REG,zA       ! merge in high order bits of memory operand
	SET_ZSP(`zA')
	DO_BITS_3_5(`zA')
	MEMWRITE(`shlr16 r0',`mov.l zHL,r0')

	DO_CYCLES_NEXT
')

! Rotate A/(HL) right, via nibbles
! RRD
define(`RRD',
`
	DEF_LN
!vbtvbt	
	MEMREAD(`shlr16 r0',`mov.l zHL,r0')

	mov #FLAG_C,DIRT_REG
	mov #0x0f,r0               ! mask for A register value
	and DIRT_REG,zF          ! keep old carry flag
	and zA,r0                     ! r0 = low order bits of A register
	extu.b TMP_REG,TMP_REG
	mov #0x0f,DIRT_REG   ! mask for low order bits of memory operand
	and TMP_REG,DIRT_REG ! DIRT_REG = low order bits of memory operand
	shll2 r0
	shlr2 TMP_REG
	shll2 r0
	shlr2 TMP_REG
	or r0,TMP_REG        ! merge in low order bits of A register
	mov #0xf0,r0
	and r0,zA                ! isolate high order bits of A register
	or DIRT_REG,zA       ! merge in high order bits of memory operand
	SET_ZSP(`zA')
	DO_BITS_3_5(`zA')
	MEMWRITE(`shlr16 r0',`mov.l zHL,r0')

	DO_CYCLES_NEXT
')

! Bit-test a register
! BIT_R src8, bit
define(`BIT_R_b',
`
	LOAD_ZREG($1)
	DO_BIT($2)

	DO_CYCLES_NEXT
')

! Bit-test (RR)
! BIT_mRR_b reg, bit
define(`BIT_mRR_b',
`
!vbtvbt
	MEMREAD(`shlr16 r0',`mov.l z$1,r0')
	mov TMP_REG,r0

	DO_BIT(`$2')

	DO_CYCLES_NEXT
')

! Bit-test (XY+dd)
! BIT_mXY_b bit
define(`BIT_mXY_b',
`
	MEMREAD(`nop')
	mov TMP_REG,r0

	DO_BIT(`$1')

	DO_CYCLES_NEXT
')

! Bit-clear a register
! RES_R_b src8, bit
define(`RES_R_b',
`
	DO_RES($1,$2)

	DO_CYCLES_NEXT
')

! Bit-clear (RR)
! RES_mRR_b src16, bit
define(`RES_mRR_b',
`
!vbtvbt
	MEMREAD(`shlr16 r0',`mov.l z$1,r0')
	DO_RES(`TMP_REG',$2)
	MEMWRITE(`nop')

	DO_CYCLES_NEXT
')

! Bit-clear (XY+dd)
! RES_mXY_b bit
define(`RES_mXY_b',
`
	MEMREAD(`nop')
	DO_RES(`TMP_REG',$1)
	MEMWRITE(`nop')

	DO_CYCLES_NEXT
')

! Bit-clear (XY+dd), with extra store
! RES_mXY_b_R bit, dest8
define(`RES_mXY_b_R',
`
	MEMREAD(`nop')
	DO_RES(`TMP_REG',$1)
	MEMWRITE(`nop')

	! Extra store
	ifelse($2,`A',
	`
		mov TMP_REG,z$2
	',
	`
		mov TMP_REG,r0
		mov.b r0,z$2
	')

	DO_CYCLES_NEXT
')

! Bit-set a register
! SET_R_b src8, bit
define(`SET_R_b',
`
	DO_SET($1,$2)

	DO_CYCLES_NEXT
')

! Bit-set (RR)
! SET_mRR_b src16, bit
define(`SET_mRR_b',
`
!vbtvbt
	MEMREAD(`shlr16 r0',`mov.l z$1,r0')
	DO_SET(`TMP_REG',$2)
	MEMWRITE(`nop')

	DO_CYCLES_NEXT
')

! Bit-set (XY+dd)
! SET_mXY_b bit
define(`SET_mXY_b',
`
	MEMREAD(`nop')
	DO_SET(`TMP_REG',$1)
	MEMWRITE(`nop')

	DO_CYCLES_NEXT
')

! Bit-set (XY+dd), with extra store
! SET_mXY_b_R bit, dest8
define(`SET_mXY_b_R',
`
	MEMREAD(`nop')
	DO_SET(`TMP_REG',$1)
	MEMWRITE(`nop')
	
	! Extra store
	ifelse($2,`A',
	`
		mov TMP_REG,z$2
	',
	`
		mov TMP_REG,r0
		mov.b r0,z$2
	')

	DO_CYCLES_NEXT
')

! Jump to an absolute address
! JP_NN
define(`JP_NN',
`
	GETWORD(`r0')
	extu.w r0,zPC

ifdef(`BASED_PC',`add FETCH_REG,zPC')

	DO_CYCLES_NEXT
')

! Jump to an absolute address, conditionally
! JP_cc_NN nz,cc
define(`JP_cc_NN',
`
	DEF_LN

	TEST_CONDITION(`$2')
	b$1 `dont_take_jump'ln

	GETWORD(`TMP_REG')
	extu.w TMP_REG,zPC
ifdef(`BASED_PC',`add FETCH_REG,zPC')

	DO_CYCLES_NEXT

	ALIGN
`dont_take_jump'ln:
	add #2,zPC    ! skip address
	ifdef(`NO_PC_LIMIT',`',`extu.w zPC,zPC')

	DO_CYCLES_NEXT
')

! Jump to an relative address
! JR_N
define(`JR_N',
`
	GETDISP(`r0')

! vbt BUG !!!!! cela saute des instructions obligatoires
        add r0,zPC
	
	ifdef(`NO_PC_LIMIT',`',`extu.w zPC,zPC')

	DO_CYCLES_NEXT
')

! Jump to an relative address, conditionally
! JR_cc_N nz,cc
define(`JR_cc_N',
`
	DEF_LN

	TEST_CONDITION(`$2')
! vbt change for op_28 to test zF and FLAG_ZSP
!	mov FLAGS_ZSP,r0
!	tst zF,r0
	b$1 `dont_take_jump'ln

	define(`STD_CYCLES',`EXTRA_CYCLES')
	JR_N

	ALIGN
`dont_take_jump'ln:
	add #1,zPC        ! skip over the displacement
	ifdef(`NO_PC_LIMIT',`',`extu.w zPC,zPC')
	define(`STD_CYCLES',`STD_BACKUP_CYCLES')

	DO_CYCLES_NEXT
')

! Jump to an absolute address (in a register)
! JP_RR addr16
define(`JP_RR',
`
	mov.l z$1,r0
	shlr16 r0
	extu.w r0,zPC
ifdef(`BASED_PC',`add FETCH_REG,zPC')

	DO_CYCLES_NEXT
')

! Decrease B, if non-zero, jump to a relative address
! DJNZ_N
define(`DJNZ_N',
`
	DEF_LN

	mov.b zB,r0
	dt r0
	bt/s `dont_take_jump'ln
	mov.b r0,zB

	GETDISP(`r0')
	add r0,zPC
	ifdef(`NO_PC_LIMIT',`',`extu.w zPC,zPC')

	DO_EXTRA_CYCLES_NEXT

	ALIGN
`dont_take_jump'ln:
	add #1,zPC        ! skip over the displacement
	ifdef(`NO_PC_LIMIT',`',`extu.w zPC,zPC')

	DO_CYCLES_NEXT
')

! Call an absolute address
! CALL_NN
define(`CALL_NN',
`
	GETWORD(`TMP_REG')

	! Write the PC
	mov.l zSP,r0
!vbt à tester
        shlr16 r0      
!vbt déplacement du mov car sauvegarde la mauvaise valeur pour op_c0
!vbt modification annullée
!	mov zPC,TMP_REG        
	SAVE_REG(`TMP_REG')     ! Save the new PC into the stack
	mov zPC,TMP_REG        
!	mov TMP_REG,zPC

ifdef(`BASED_PC',`sub FETCH_REG,TMP_REG')       ! Un-base PC

	add #-1,r0
	MEMWRITE(`shlr8 TMP_REG')
	add #-1,r0
	mov zPC,TMP_REG
ifdef(`BASED_PC',`sub FETCH_REG,zPC')       ! Un-base PC
	MEMWRITE(`nop')

	RESTORE_REG(`zPC')
	mov.w r0,zSP            ! Writeback SP
	extu.w zPC,zPC          ! Prepare PC
ifdef(`BASED_PC',`add FETCH_REG,zPC')       ! Base PC

	DO_CYCLES_NEXT
')

! Call an absolute address, conditionally
! CALL_cc_NN
define(`CALL_cc_NN',
`
	DEF_LN

	TEST_CONDITION(`$2')
	b$1 `dont_take_call_'ln
	
	CALL_NN

`dont_take_call_'ln:
	add #2,zPC
	ifdef(`NO_PC_LIMIT',`',`extu.w zPC,zPC')

	DO_CYCLES_NEXT
')

! Pop the PC
! RET
define(`RET',
`
	POP_PC

	DO_CYCLES_NEXT
')

! Pop the PC, conditionally
! RET_cc
define(`RET_cc',
`
	DEF_LN

	TEST_CONDITION(`$2')
	b$1 `dont_take_ret_'ln

	RET

`dont_take_ret_'ln:
	DO_CYCLES_NEXT
')

! Pop the PC, restore IFF1, signal the BUS
! RETI
define(`RETI',
`
	DEF_LN

	POP_PC

	! Restore IFF1 & signal the BUS
	mov.l `_z80_IFF2_'ln,r0
	mov.b @r0,TMP_REG        ! TMP_REG = IFF2
	mov.l `_z80_RetI_'ln,DIRT_REG
	mov.l @DIRT_REG,DIRT_REG
	mov.l `_z80_IFF1_'ln,r0
	jsr @DIRT_REG
	mov.b TMP_REG,@r0

	DO_CYCLES_NEXT
	
ALIGN_DATA
	GEN_MT(`_z80_IFF1',ln)
	GEN_MT(`_z80_IFF2',ln)
	GEN_MT(`_z80_RetI',ln)
')

! Pop the PC, restore IFF1
! RETN
define(`RETN',
`
	DEF_LN

	POP_PC

	! Restore IFF1 & signal the BUS
	mov.l `_z80_IFF2_'ln,r0

	mov.b @r0,TMP_REG
	dt r0                ! IFF1 is immediately before IFF2
	mov.b TMP_REG,@r0

	DO_CYCLES_NEXT
	
ALIGN_DATA
	GEN_MT(`_z80_IFF1',ln)
	GEN_MT(`_z80_IFF2',ln)
')

! Call an absolute address
! RST addr8
define(`RST',
`
	! Write the PC
	mov.w zSP,r0

	mov zPC,TMP_REG
ifdef(`BASED_PC',`sub FETCH_REG,TMP_REG')    ! Un-base PC
	add #-1,r0
	MEMWRITE(`shlr8 TMP_REG')
	add #-1,r0
mov zPC,TMP_REG
ifdef(`BASED_PC',`sub FETCH_REG,TMP_REG')    ! Un-base PC
	MEMWRITE(`nop')

	mov #$1,zPC             ! Get the new PC (it does not require unsigned extension)
	mov.w r0,zSP            ! Writeback SP
	ifdef(`BASED_PC',`add FETCH_REG,ezPC')    ! Base PC

	DO_CYCLES_NEXT
')

! Out R to R*256+N
! OUT_N_R src8
define(`OUT_N_R',
`
	DEF_LN

	GETBYTE(`DIRT_REG')
	ifelse($1,`A',
	`
		mov z$1,r0
		mov z$1,TMP_REG
	',
	`
		mov.b z$1,r0
		mov r0,TMP_REG
	')
	shll8 r0
	or DIRT_REG,r0

	IOWRITE(`r0',`TMP_REG')

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT(`_z80_Out',ln)
')

! In from R*256+N, into R
! IN_R_N dest8
define(`IN_R_N',
`
	DEF_LN

	GETBYTE(`DIRT_REG')
	LOAD_ZREG($1)
	shll8 r0
	or DIRT_REG,r0

	ifelse($1,`A',
		`
		IOREAD(`r0')
		mov r0,z$1
		',
		`
		IOREAD(`r0')
		mov.b r0,z$1
	')
	
	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT(`_z80_In',ln)
')

! In from BC
! IN_R dest8
define(`IN_R',
`
	DEF_LN

	mov.w zBC,r0
	mov #0,zF

	ifelse($1,`A',
		`
		IOREAD(`r0')
		mov r0,z$1
		',
		`
		IOREAD(`r0')
		mov.b r0,z$1          ! write the result
	')
	
	DO_BITS_3_5(`r0')
	SET_ZSP(`r0')
	
	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT(`_z80_In',ln)
')

! In from BC, ignore result
! IN_F
define(`IN_F',
`
	DEF_LN

	mov.w zBC,r0
	mov #0,zF
	IOREAD(`r0')

	DO_BITS_3_5(`r0')
	SET_ZSP(`r0')

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT(`_z80_In',ln)
')

! Out to BC
! OUT_R src8
define(`OUT_R',
`
	DEF_LN

	mov.l zBC,TMP_REG
	shlr16 TMP_REG
!vbt pas testé
!vbt correct
	ifelse($1,`A',
	`
		IOWRITE(`TMP_REG',`zA')
	',
	`
		mov.b z$1,r0
		IOWRITE(`TMP_REG',`r0')
	')

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT(`_z80_Out',ln)
')

! Out zero to BC
! OUT_0
define(`OUT_0',
`
	DEF_LN

	mov.w zBC,r0
	mov #0,TMP_REG
	IOWRITE(`r0',`TMP_REG')

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT(`_z80_Out',ln)
')

! In from BC into (HL), increase HL, decrease B
define(`INI',
`
! FIXME - Parity flag is not emulated

	DEF_LN

	INX(`I')

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT(`_z80_In',ln)
')

! In from BC into (HL), decrease HL, decrease B
define(`IND',
`
! FIXME - Parity flag is not emulated

	DEF_LN

	INX(`D')

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT(`_z80_In',ln)
')

! In from BC into (HL), increase HL, decrease B, repeat
define(`INIR',
`
	INXR(`I')

	DO_CYCLES_NEXT
')

! In from BC into (HL), decrease HL, decrease B, repeat
define(`INDR',
`
	INXR(`D')

	DO_CYCLES_NEXT
')

! Out from (HL) into BC, increase HL, decrease B
define(`OUTI',
`
! FIXME - Parity flag is not emulated

	DEF_LN

	OUTX(`I')

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT(`_z80_Out',ln)
')

! Out from (HL) into BC, decrease HL, decrease B
define(`OUTD',
`
! FIXME - Parity flag is not emulated

	DEF_LN

	OUTX(`D')

	DO_CYCLES_NEXT

ALIGN_DATA
	GEN_MT(`_z80_Out',ln)
')

! Out from (HL) into BC, increase HL, decrease B, repeat
define(`OTIR',
`
	OTXR(`I')
	DO_CYCLES_NEXT
')

! Out from (HL) into BC, decrease HL, decrease B, repeat
define(`OTDR',
`
	OTXR(`D')
	DO_CYCLES_NEXT
')

! BCD adjust
define(`DAA',
`
	DEF_LN

	mov zF,r0          ! r0 = flags
	mov.l `DAA_Table_'ln,TMP_REG
	tst #FLG_H,r0
	extu.b zA,zA      ! Para calcular desp. tabla DAA
	and #0x2,r0       ! Enmascarar registro banderas
	shll8 r0
	bt/s .daa_nh_set
	or zA,r0          ! r0 = zAF
	tst #FLG_N,r0

!define(`FLG_V',`0x01') ! 01h         ! parity/overflow
!define(`FLG_P',`0x01') ! 01h         ! parity/overflow
!define(`FLG_H',`0x02') ! 02h         ! half-carry
!define(`FLG_N',`0x04') ! 04h         ! negative
!define(`FLG_iV',`0x08') ! 08h        ! V flag indicator

	mov r0,DIRT_REG
	mov zF,r0          ! r0 = flags
	tst #0x04,r0
	bf/s .daa_n_set 
	mov DIRT_REG,r0
	and #0xff,r0
.daa_n_set:
	mov.l daatbl_offset_453,r2
	add DIRT_REG,TMP_REG
.daa_nh_set:
	shll r0
	mov.w @(r0,TMP_REG),zA
	mov zA,zF
	shlr8 zF
	exts.b zA,zA
	CACHE_CCR

	DO_BITS_3_5(`zF')

	DO_CYCLES_NEXT

ALIGN_DATA
	`daatbl_offset_'ln: .long 512*4
	GEN_MT(`DAA_Table',ln)
')

! Complement A
define(`CPL',
`
	not zA,zA
	! H and N is set, not affected otherwise
	mov #(FLG_H|FLG_N),r0
	DO_BITS_3_5(`zA')
	or r0,zF

	DO_CYCLES_NEXT
')

! Negate A
define(`NEG',
`
	DEF_LN

	mov.l `NEG_Table_'ln,r0
	extu.b zA,zA
	mov.b @(r0,zA),zF
	neg zA,zA
	CACHE_CCR
	DO_BITS_3_5(`zA')

	DO_CYCLES_NEXT

	ALIGN_DATA
	GEN_MT(`NEG_Table',ln)
')

! Set carry flag
define(`SCF',
`
	mov #(FLG_iV | FLG_P),DIRT_REG    ! Z,S and P not affected
	mov #1,FLAGS_C
	and DIRT_REG,zF
	DO_BITS_3_5(`zA')

	DO_CYCLES_NEXT
')

! Complement carry flag
define(`CCF',
`
	mov #~(FLG_N | FLG_H),DIRT_REG
	tst FLAGS_C,FLAGS_C     ! not C -> T
	and DIRT_REG,zF         ! reset N flag
	mov FLAGS_C,TMP_REG     ! TMP_REG = old carry
	movt FLAGS_C            ! FLAGS_C = not C
	shll TMP_REG            ! get H flag in place
	or TMP_REG,zF           ! H = old carry

	DO_BITS_3_5(`zA')
	DO_CYCLES_NEXT
')

! No operation
define(`NOP',
`
	DO_CYCLES_NEXT
')

! Halt the processor
define(`HALT',
`
	DEF_LN

	mov.l `_z80_TempICount_'ln,DIRT_REG
	mov.l @DIRT_REG,TMP_REG    ! TempICount
	mov #3,r0
	and r0,ICOUNT_REG
	and r0,TMP_REG

	tst ICOUNT_REG,ICOUNT_REG
	bt halt_zero

	add #-4,TMP_REG
	add #-4,ICOUNT_REG

halt_zero:
	mov.l TMP_REG,@DIRT_REG
	mov.l `_z80_halted_'ln,DIRT_REG
	mov #1,r0
	EXIT(`mov.b r0,@DIRT_REG')

	ALIGN_DATA
	GEN_MT(`_z80_halted',ln)
	GEN_MT(`_z80_TempICount',ln)
')

! Enable interrupts
define(`EI',
`
	DEF_LN

	! set the P/V bit, for use in "LD I,A" etc
	mov #FLG_P,r0
	mov.l `_z80_IFF1_'ln,TMP_REG
	mov.l `_z80_IFF2_'ln,DIRT_REG
	mov.b r0,@TMP_REG            ! setting IFF1
	mov.b r0,@DIRT_REG          ! setting IFF2

	mov.l `_z80_afterEI_'ln,TMP_REG
	mov #1,r0
	mov.b r0,@TMP_REG

	! force the next instruction to be executed regardless of ICount
	! then force an IRQ check after it.	
	! set ICount to one so it will terminate
	! after the next instruction.
	! we can then pick it up again and carry on.
	mov.l `_z80_TempICount_'ln,TMP_REG
	mov.l ICOUNT_REG,@TMP_REG

	NEXT(`mov #1,ICOUNT_REG')

	ALIGN_DATA
	GEN_MT(`_z80_TempICount',ln)
	GEN_MT(`_z80_IFF1',ln)
	GEN_MT(`_z80_IFF2',ln)
	GEN_MT(`_z80_afterEI',ln)
')

! Disable interrupts
define(`DI',
`
	DEF_LN

	mov #0,r0
	mov.l `_z80_IFF1_'ln,TMP_REG
	mov.w r0,@TMP_REG    ! Loads zero in zIFF1 and zIFF2

	DO_CYCLES_NEXT
	
	ALIGN_DATA
	GEN_MT(`_z80_IFF1',ln)
')

! Set interrupt mode
define(`IM',
`
	DEF_LN

	mov.l `_z80_IM_'ln,TMP_REG
	mov #$1,r0
	mov.b r0,@TMP_REG

	DO_CYCLES_NEXT
	
	ALIGN_DATA
	GEN_MT(`_z80_IM',ln)
')

divert

!----------------------------------------------------------------------------!
! Bring in the opcodes...

include(`raze.inc')
!----------------------------------------------------------------------------!

!----------------------------------------------------------------------------!
! Support functions:

DEF(`op',`cb',`0',`0')
	GETDISP(`r0')
	mov.l CBTable_addr,TMP_REG
	shll2 r0
	add TMP_REG,r0
	mov.l @r0,r0
	jmp @r0
	ifdef(`EMULATE_R_REGISTER',`add #1,zR',`nop')

ALIGN_DATA
	CBTable_addr: .long CBTable


DEF(`op',`ed',`0',`0')
	GETDISP(`r0')
	mov.l EDTable_addr,TMP_REG
	shll2 r0
	mov.l @(r0,TMP_REG),r0
	jmp @r0
	ifdef(`EMULATE_R_REGISTER',`add #1,zR',`nop')

	ALIGN_DATA
	EDTable_addr: .long EDTable


DEF(`op',`dd',`0',`0')
	GETDISP(`r0')
	mov.l DDTable_addr,TMP_REG
	shll2 r0
	mov.l @(r0,TMP_REG),r0
	jmp @r0
	ifdef(`EMULATE_R_REGISTER',`add #1,zR',`nop')

ALIGN_DATA
	DDTable_addr: .long DDTable


DEF(`op',`fd',`0',`0')
	GETDISP(`r0')
	mov.l FDTable_addr,TMP_REG
	shll2 r0
	mov.l @(r0,TMP_REG),r0
	jmp @r0
	ifdef(`EMULATE_R_REGISTER',`add #1,zR',`nop')

ALIGN_DATA
	FDTable_addr: .long FDTable

DEF(`dd',`cb',`0',`0')
	GETDISP(`TMP_REG')
	GETDISP(`DIRT_REG')
	mov.l DDCBTable_addr,r0
	shll2 DIRT_REG
	mov.l @(r0,DIRT_REG),DIRT_REG
	mov.l zIX,r0
	shlr16 r0
	ifdef(`EMULATE_R_REGISTER',`add #1,zR')
	jmp @DIRT_REG
	add TMP_REG,r0

ALIGN_DATA
	DDCBTable_addr: .long DDCBTable

DEF(`fd',`cb',`0',`0')
	GETDISP(`TMP_REG')
	GETDISP(`DIRT_REG')
	mov.l DDCBTable_addr2,r0
	shll2 DIRT_REG
	mov.l @(r0,DIRT_REG),DIRT_REG
	mov.w zIY,r0
	ifdef(`EMULATE_R_REGISTER',`add #1,zR')
	jmp @DIRT_REG
	add TMP_REG,r0

ALIGN_DATA
	DDCBTable_addr2: .long DDCBTable

DEF(`ed',`xx',`8',`0')
	ifdef(`EMULATE_R_REGISTER',`add #1,zR')
	DO_CYCLES_NEXT

DEF(`dd',`xx',`4',`0')
	add #-1,zPC
	ifdef(`NO_PC_LIMIT',`',`extu.w zPC,zPC')

   DO_CYCLES_NEXT

DEF(`fd',`xx',`0',`0')
	add #-1,zPC
	ifdef(`NO_PC_LIMIT',`',`extu.w zPC,zPC')

   DO_CYCLES_NEXT


! called when out-of-cycles
ALIGN
z80_finish:
	mov.l _z80_afterEI_zf,TMP_REG
	mov.b @TMP_REG,r0
ifdef(BASED_PC',`sub FETCH_REG,zPC')    ! Un-base PC
	cmp/eq #1,r0
	bf really_finish

	! Revisar este codigo
	mov.l _z80_ICount_zf,r0
	mov.l @(_z80_TempICount-_z80_ICount,r0),DIRT_REG
	add DIRT_REG,ICOUNT_REG

	mov #0,r0
	mov.b r0,@TMP_REG    ! _z80_afterEI = 0
	CHECK_IRQ(`finish')
	add #-4,ICOUNT_REG     ! subtract EIs cycles
	cmp/pl ICOUNT_REG
	bf really_finish
	NEXT(`nop')

ALIGN
really_finish:

	! Writeback flags
	WRITEBACK_CCR(`zF')
!	mov #~(FLAG_3|FLAG_5),TMP_REG
!	mov ZFLAG35,r0
!	and TMP_REG,zF
!	and #(FLAG_3|FLAG_5),r0    ! isolate 3-5 bits
!	or zF,r0

	! Writeback A register
	shll8 zA
	mov.l _z80_AF_zf,TMP_REG
	or zA,zF
	mov.w zF,@TMP_REG

	! Writeback zR register
	ifdef(`EMULATE_R_REGISTER',
	`
		mov.l _z80_R_zf,r0
		mov.b zR,@r0
	')

	! Writeback PC & calculate cycles executed
	mov.l _z80_ICount_zf,r0
	mov.l ICOUNT_REG,@r0

ifdef(`BIG_ENDIAN',
`
!	shll16 zPC
',
`
	extu.w zPC,zPC
')

	mov.l @(_z80_Initial_ICount - _z80_ICount,r0),r0
	mov.l zPC,ezPC_MEM
	sub ICOUNT_REG,r0

	lds.l @r15+,pr          ! restoring SH procedure register
	ifdef(`EMULATE_R_REGISTER',`RESTORE_REG(`zR')')
	RESTORE_REG(`r13')
	RESTORE_REG(`r12')
	RESTORE_REG(`r11')
	RESTORE_REG(`r10')
	RESTORE_REG(`r9')
	rts
	RESTORE_REG(`r8')

	ALIGN_DATA
	_z80_afterEI_zf: .long _z80_afterEI
	_z80_ICount_zf:  .long _z80_ICount
	_z80_Fetch_zf:   .long _z80_Fetch
	_z80_AF_zf:      .long _z80_AF
	_z80_R_zf:       .long _z80_R
	GEN_MT(`Byte_Flags',ln)


! Fetch decode
ALIGN
z80_fdl:
	GETDISP(r0)
	shll2 r0
	cmp/pl ICOUNT_REG
ifdef(`EMULATE1',`',
`
	bt/s .contexec
	mov.l @(r0,OPTABLE_REG),r0
')
	bra z80_finish
	add #-1,zPC

.contexec:
	jmp @r0
	ifdef(`EMULATE_R_REGISTER',`add #1,zR',`nop')

ifdef(`SINGLE_MEM_BLOCK',`',
`
! Memread
ALIGN
_z80_memread:
	ifdef(`SINGLE_MEM_HANDLER',
	`
		CALLGCC_START
		mov.l _z80_Read_mr,TMP_REG
		mov.l @TMP_REG,TMP_REG
		jsr @TMP_REG
		extu.w r0,r4
		CALLGCC_END
		rts
		exts.b r0,TMP_REG

	ALIGN_DATA
		_z80_Read_mr: .long _z80_Read
	',
	`
! vbt inutile shlr16 fait avant        
		extu.w r0,r0
		mov r0,TMP_REG    ! Load address into TMP_REG
		shlr8 TMP_REG
		shll2 TMP_REG
		mov.l _z80_Read_mr,MEM_REG
		shll TMP_REG
		add MEM_REG,TMP_REG
		ifdef(`NO_READ_HANDLER',`',`
			mov.l @TMP_REG,MEM_REG
			tst MEM_REG,MEM_REG
			bf .use_rhandler
		')
		mov.l @(4,TMP_REG),TMP_REG
		mov.l _z80_memread_mr,MEM_REG    ! restore MEM_REG address
		rts
		mov.b @(r0,TMP_REG),TMP_REG
ifdef(`NO_READ_HANDLER',`',
	`ALIGN
	.use_rhandler:
		SAVE_REG(`r0')
		CALLGCC_START
		jsr @MEM_REG
		extu.w r0,r4
		CALLGCC_END
		mov.l _z80_memread_mr,MEM_REG    ! restore MEM_REG address
		exts.b r0,TMP_REG
		rts
		RESTORE_REG(`r0')
      ')
	ALIGN_DATA
		_z80_Read_mr:    .long _z80_Read
		_z80_memread_mr: .long _z80_Mem_Handlers
	')


! Memwrite
ALIGN
_z80_memwrite:
	ifdef(`SINGLE_MEM_HANDLER',
	`
		SAVE_REG(`r0')
		CALLGCC_START
		mov TMP_REG,r5
		mov.l _z80_Write_mr,TMP_REG
		mov.l @TMP_REG,TMP_REG
		jsr @TMP_REG
		extu.w r0,r4        ! Load address
		CALLGCC_END
		rts
		RESTORE_REG(`r0')

	ALIGN_DATA
		_z80_Write_mr: .long _z80_Write
	',
	`
		extu.w r0,r0
		mov r0,DIRT_REG    ! Load address into DIRT_REG
		shlr8 DIRT_REG
		shll2 DIRT_REG
		mov.l _z80_Write_mw,MEM_REG
		shll DIRT_REG
		add MEM_REG,DIRT_REG
		mov.l @DIRT_REG,MEM_REG
		tst MEM_REG,MEM_REG
		bf .use_whandler

		mov.l @(4,DIRT_REG),DIRT_REG
		mov.l _z80_memwrite_mw,MEM_REG    ! restore MEM_REG address
		rts
		mov.b TMP_REG,@(r0,DIRT_REG)

	ALIGN
	.use_whandler:
		SAVE_REG(`r0')
		CALLGCC_START
		extu.w r0,r4         ! Load address
		jsr @MEM_REG
		extu.b TMP_REG,r5
		CALLGCC_END
		mov.l _z80_memwrite_mw,MEM_REG    ! restore MEM_REG address
		rts
		RESTORE_REG(`r0')

	ALIGN_DATA
		_z80_Write_mw:    .long _z80_Write
		_z80_memwrite_mw: .long _z80_Mem_Handlers
	')
')

! Default handlers
ALIGN
z80_default_in:
z80_default_read:
	rts
	nop

z80_default_out:
z80_default_write:
z80_default_reti:
z80_default_fetch_callback:
	rts
	nop

! called to look up a read handler
! address is on the stack
!read_dispatcher:
!	rts
!	nop

! called to look up a write handler
! address+value are on the stack
!write_dispatcher:
!	rts
!	nop


! int z80_emulate(int cycles)
! Emulate for 'cycles' T-states,
! Returns the number of cycles actually emulated.
ALIGN
.global _z80_emulate
_z80_emulate:
	! Codigo de ciclado inicial
	mov.l _z80_Extra_Cycles_emul,r2
	mov.l @r2,r1                ! r1 = Extra Cycles
	mov #0,r0
	mov r4,ICOUNT_REG           ! ICOUNT_REG = required cycles
	mov.l r0,@r2                ! Extra Cycles = 0
	mov.l _z80_afterEI_emul,r4
	mov.b r0,@r4                ! AfterEI = 0

	ifdef(`NO_EXTRA_CYCLES',`',`add r1,ICOUNT_REG')

	mov.l _z80_ICount_emul,r0
	mov.l ICOUNT_REG,@r0        ! ICount set up
	mov.l ICOUNT_REG,@(_z80_Initial_ICount - _z80_ICount,r0)  ! Initial ICount

	! Salvar registros a usar en la pila
	SAVE_REG(`r8')
	mov.l REF_REG_emul,REF_REG         ! Reference register
	SAVE_REG(`r9')
	mov.l _z80_AF_emul,r0
	SAVE_REG(`r10')
	mov.l _OpTable_emul,OPTABLE_REG    ! Load opcode table
	SAVE_REG(`r11')


	ifdef(`VBT2',
	`mov.l titi,r9
	',
	`
	')

	ifdef(`SH2_COMPAT_CODE',
	`
	',
	`
		pref @REF_REG                      ! Prefetch z80 registers
	')

	SAVE_REG(`r12')
	SAVE_REG(`r13')
	ifdef(`EMULATE_R_REGISTER',`SAVE_REG(`zR')')
	sts.l pr,@-r15          ! saving SH procedure register

	! Cache de registros z80
	mov.l @r0,zF                   ! zF
	ifdef(`VBT',
	`shlr16 zF
	',
	`
	')        
	mov.l z80_fdl_emul,ZFINISH     ! Load fdl address to execute fdl
	mov zF,zA
	mov.l @(_z80_PC-_z80_AF,r0),ezPC    ! ezPC
	CACHE_CCR

	! Cache de registro R
	ifdef(`EMULATE_R_REGISTER',
	`
		mov.l _z80_R_emul,TMP_REG
		mov.b @TMP_REG,zR
		extu.b zR,zR
	')

	ifdef(`VBT2',
	`
	',
	`mov.l _z80_Fetch_emul,FETCH_REG    ! FETCH_REG
	')        
	
	mov.l _z80_Mem_Handlers_emul,MEM_REG
	
	ifdef(`VBT2',
	`
	',
	`ifdef(`SINGLE_FETCH',`mov.l @FETCH_REG,FETCH_REG')
	')        

	mov.l _z80_halted_emul,DIRT_REG
	mov.b @DIRT_REG,TMP_REG
	tst TMP_REG,TMP_REG                ! testing halt
	bt/s not_halted
	ifdef(`EMULATE_BITS_3_5',`DO_BITS_3_5(`zF')',`nop')

	! Z80 CPU halted
	mov.l _z80_ICount_emul,TMP_REG
	mov ICOUNT_REG,r0
	and #3,r0                    ! If we're halted, just skip it all.
	shlr8 zA                     ! set up A register to be uncached correctly 
	add #-4,r0

	bra z80_finish
	mov r0,ICOUNT_REG


ALIGN
not_halted:
ifdef(`BASED_PC',`add FETCH_REG,zPC')
	NEXT(`shlr8 zA    ! set up A register')

	ALIGN_DATA
	_z80_Extra_Cycles_emul: .long _z80_Extra_Cycles
	_z80_ICount_emul:       .long _z80_ICount
	_z80_AF_emul:           .long _z80_AF
	_z80_R_emul:            .long _z80_R
	_z80_halted_emul:       .long _z80_halted
	_z80_afterEI_emul:      .long _z80_afterEI
	_OpTable_emul:          .long OpTable
	_z80_Fetch_emul:        .long _z80_Fetch
	ifdef(`VBT',
	`titi:                  .long 0x060BD000
	',
	`
	')


	ifdef(`SINGLE_MEM_BLOCK',
	`
		_z80_Mem_Handlers_emul: .long _z80_Read
	',
	`
		_z80_Mem_Handlers_emul: .long _z80_Mem_Handlers
	')
	REF_REG_emul:           .long REF
	z80_fdl_emul:           .long z80_fdl


! void z80_reset(void)
! Resets the Z80.
ALIGN
.global _z80_reset
_z80_reset:
	mov #0,r0       ! just zero it all
	mov.l context_start_reset,r1
	mov.l registers_end_reset,r2

rloop:
	mov.l r0,@-r2
	cmp/eq r1,r2
	bf rloop

	mov #-1,r0                             ! r0 = -1
	mov.w r0,@(_z80_IX-context_start,r1)   ! IX = 0xFFFF
	mov.w r0,@(_z80_IY-context_start,r1)   ! IY = 0xFFFF
	mov #0x40,r0
	rts
ifdef(`BIG_ENDIAN',
`
	mov.b r0,@(_z80_AF-context_start+1,r1) ! AF = 0x0040
',
`
	mov.b r0,@(_z80_AF-context_start,r1) ! AF = 0x0040
')

	ALIGN_DATA
	context_start_reset: .long context_start
	registers_end_reset: .long registers_end


! void z80_raise_IRQ(UBYTE vector)
! Causes an interrupt with the given vector.
! In IM 0, the vector has to be an RST opcode.
! If interrupts are disabled, the interrupt will still happen as long as
! the IRQ line stays raised.
ALIGN
.global _z80_raise_IRQ
_z80_raise_IRQ:
	mov.l _z80_IRQVector_raise,r1
	mov #1,r0
	mov.b r0,@(_z80_IRQLine-_z80_IRQVector,r1) ! z80_IRQLine = 1
	mov.b r4,@r1                               ! z80_IRQVector = vector

	SAVE_REG(`r8')
	SAVE_REG(`r9')
	SAVE_REG(`r10')
	SAVE_REG(`r11')
	SAVE_REG(`r12')
	ifdef(`EMULATE_BITS_3_5',`SAVE_REG(`r13')')
	ifdef(`EMULATE_R_REGISTER',`SAVE_REG(`zR')')

	sts.l pr,@-r15          ! saving SH procedure register

	mov.l REF_REG_ri,REF_REG                   ! Reference register
	mov.l _z80_PC_ri,r0

	! Cache R register
	ifdef(`EMULATE_R_REGISTER',
	`
		mov.l _z80_R_ri,TMP_REG
		mov.b @TMP_REG,zR
		extu.b zR,zR
	')

	mov.l @r0,ezPC                             ! ezPC
	mov.l _z80_Mem_Handlers_ri,MEM_REG

	CHECK_IRQ(`raise')

	extu.w zPC,zPC
	mov.l zPC,ezPC_MEM

	! Writeback zR register
	ifdef(`EMULATE_R_REGISTER',
	`
		mov.l _z80_R_ri,r0
		mov.b zR,@r0
	')

	lds.l @r15+,pr          ! restoring SH procedure register

	ifdef(`EMULATE_R_REGISTER',`RESTORE_REG(`zR')')
	ifdef(`EMULATE_BITS_3_5',`RESTORE_REG(`r13')')
	RESTORE_REG(`r12')
	RESTORE_REG(`r11')
	RESTORE_REG(`r10')
	RESTORE_REG(`r9')
	rts
	RESTORE_REG(`r8')

	ALIGN_DATA
	_z80_IRQVector_raise: .long _z80_IRQVector
	_z80_PC_ri:           .long _z80_PC
	REF_REG_ri:           .long REF
	ifdef(`SINGLE_MEM_BLOCK',
	`
		_z80_Mem_Handlers_ri:         .long _z80_Read
	',
	`
		_z80_Mem_Handlers_ri:         .long _z80_Mem_Handlers
	')
	_z80_R_ri:            .long _z80_R


! void z80_lower_IRQ(void)
! Lowers the IRQ line.
ALIGN
.global _z80_lower_IRQ
_z80_lower_IRQ:
	mov #0,r0
	mov.l _z80_IRQLine_lower,r1
	rts
	mov.b r0,@r1          ! z80_IRQLine = 0

	ALIGN_DATA
	_z80_IRQLine_lower: .long _z80_IRQLine


! void z80_cause_NMI(void)
! Causes a non-maskable interrupt.
! This will always be accepted.
! There is no raise/lower functions, as the NMI is edge-triggered.
ALIGN
.global _z80_cause_NMI
_z80_cause_NMI:

	SAVE_REG(`MEM_REG')
	SAVE_REG(`REF_REG')
	sts.l pr,@-r15               ! saving SH procedure register
	mov.l REF_REG_cnmi,REF_REG   ! Reference register

	! Reset IFF1 and z80_halted
	mov.l _z80_IFF1_cnmi,r1
	mov #0,r0
	mov.b r0,@r1
	mov.l _z80_halted_cnmi,r1
	mov.b r0,@r1

ifdef(`EMULATE_R_REGISTER',
`
	mov.l _z80_R_cnmi,r1
	mov.b @r1,r0
	add #1,r0
	mov.b r0,@r1	
')

	! Add extra cycles
	mov.l _z80_Extra_Cycles_cnmi,r0
	mov.l @r0,r1
	ifdef(`VBT',`shlr16 r1',`')
	add #11,r1
	mov.l r1,@r0

	mov.l _z80_PC_cnmi,r4
	mov.l _z80_Mem_Handlers_cnmi,MEM_REG    ! Necesario para las escrituras
	ifdef(`VBT',`mov.l @r4,TMP_REG',`mov.w @r4,TMP_REG')
 	mov.l _z80_SP_cnmi,r5
	mov.w @r5,r0
	add #-1,r0
	shlr8 TMP_REG
	MEMWRITE(`mov r0,r7')

	add #-1,r7
	mov r7,r0
	ifdef(`VBT',`MEMWRITE(`mov.l @r4,TMP_REG')',`MEMWRITE(`mov.w @r4,TMP_REG')')

	mov.w r7,@r5    ! Store the new SP
	mov #0x66,r1
	ifdef(`VBT',`mov.l r1,@r4',`mov.w r1,@r4')    ! Set the new PC to 66h

	lds.l @r15+,pr          ! restoring SH procedure register
	RESTORE_REG(`REF_REG')
	rts
	RESTORE_REG(`MEM_REG')

ALIGN_DATA
	_z80_Extra_Cycles_cnmi: .long _z80_Extra_Cycles
	_z80_IFF1_cnmi: .long _z80_IFF1
	_z80_halted_cnmi: .long _z80_halted
	_z80_R_cnmi: .long _z80_R
	_z80_PC_cnmi: .long _z80_PC
	_z80_SP_cnmi: .long _z80_SP
	REF_REG_cnmi: .long REF
	ifdef(`SINGLE_MEM_BLOCK',
	`
		_z80_Mem_Handlers_cnmi: .long _z80_Read
	',
	`
		_z80_Mem_Handlers_cnmi: .long _z80_Mem_Handlers
	')


! int z80_get_context_size(void)
! Returns the size of the context, in bytes
ALIGN
.global _z80_get_context_size
_z80_get_context_size:
	mov.l context_end_gcs,r0
	mov.l context_start_gcs,r1
	sub r1,r0

	rts
	add #4,r0       ! safety gap
	
	ALIGN_DATA
	context_end_gcs:   .long context_end
	context_start_gcs: .long context_start


! void z80_set_context(void *context)
! Copy the given context to the current Z80.
ALIGN
.global _z80_set_context
_z80_set_context:
	mov.l context_end_sc,r1
	mov.l context_start_sc,r0
	add #-4,r1
	
sc_loop:
	mov.l @r4+,r2
	mov.l r2,@r0
	cmp/eq r0,r1
	bf/s sc_loop
	add #4,r0

	rts
	mov #0,r0
	
	ALIGN_DATA
	context_start_sc: .long context_start
	context_end_sc: .long context_end


! void z80_get_context(void *context)
! Copy the current Z80 to the given context.
ALIGN
.global _z80_get_context
_z80_get_context:
	mov.l context_start_gc,r0
	mov.l context_end_gc,r1
	
gc_loop:
	mov.l @r0+,r2
	mov.l r2,@r4
	cmp/eq r0,r1
	bf/s gc_loop
	add #4,r4

	rts
	mov #0,r0
	
	ALIGN_DATA
	context_start_gc: .long context_start
	context_end_gc: .long context_end


! int z80_get_cycles_elapsed(void)
! Returns the number of cycles emulated since z80_emulate was called.
ALIGN
.global _z80_get_cycles_elapsed
_z80_get_cycles_elapsed:
	mov.l _z80_ICount_gce,r1
	mov.l @r1,r3
	mov.l @(_z80_Initial_ICount-_z80_ICount,r1),r2
	sub r2,r3
	
	mov.b @(_z80_afterEI-_z80_ICount,r1),r0
	cmp/eq #0,r0
	bt z80_gcd_fin
	
	mov.l @(_z80_TempICount-_z80_ICount,r1),r2
	sub r2,r3

z80_gcd_fin:
	rts
	nop

	ALIGN_DATA
	_z80_ICount_gce: .long _z80_ICount


! void z80_stop_emulating(void)
! Stops the emulation dead. (waits for the current instruction to finish
! first though).
! The return value from z80_execute will obviously be lower than expected.
ALIGN
.global _z80_stop_emulating
_z80_stop_emulating:
	mov.l _z80_ICount_se,r1
	mov.l @r1,r2       ! r2 = ICount
	mov.l @(_z80_Initial_ICount-_z80_ICount,r1),r3 ! r3 = Initial_ICount
	sub r2,r3
	mov.b @(_z80_afterEI-_z80_ICount,r1),r0
	tst r0,r0
	bt z80_se_fin

	mov.l @(_z80_TempICount-_z80_ICount,r1),r2
	sub r2,r3

z80_se_fin:
	mov #0,r2
	mov.l r2,@r1
	mov.l r2,@(_z80_TempICount-_z80_ICount,r1)

	rts
	mov.l r3,@(_z80_Initial_ICount-_z80_ICount,r1) ! Initial_ICount updated

	ALIGN_DATA
	_z80_ICount_se: .long _z80_ICount


! void z80_skip_idle(void)
! Stops the emulation dead. (waits for the current instruction to finish
! first though).
! The return value from z80_execute will appear as though it had executed
! all the instructions like normal.
ALIGN
.global _z80_skip_idle
_z80_skip_idle:
	mov.l _z80_ICount_si,r0
	mov #0,r1
	mov.l r1,@r0
	rts
	mov.l r1,@(_z80_TempICount-_z80_ICount,r0)

	ALIGN_DATA
	_z80_ICount_si: .long _z80_ICount

! void z80_do_wait_states(int n)
! Halts the CPU temporarily, to execute 'n' memory wait states.
ALIGN
.global _z80_do_wait_states
_z80_do_wait_states:
	mov.l _z80_Initial_ICount_dws,r0
	mov.l @r0,r1
	add r4,r1
	rts
	mov.l r1,@r0

	ALIGN_DATA
	_z80_Initial_ICount_dws: .long _z80_Initial_ICount


! UWORD z80_get_reg(z80_register reg)
! Return the value of the specified register.
ALIGN
.global _z80_get_reg
_z80_get_reg:
	extu.w r4,r4
	mov #Z80_REG_HL2,r1
	cmp/hi r1,r4
	bt .nirreg

	! Normal register loading
	mov.l _context_start_gr,r0
	shll2 r4
        
!vbt pas utile ?????        
ifdef(`BIG_ENDIAN',
`
	mov.l @(r0,r4),r0
	rts
	swap.w r0,r0
',
`
	rts
	mov.l @(r0,r4),r0
')

.nirreg:
	mov #Z80_REG_IFF1,r1
	cmp/eq r1,r4
	bf .irreg2

	mov.l _reg_iff1,r0
	rts
	mov.b @r0,r0

.irreg2:
	mov #Z80_REG_IFF2,r1
	cmp/eq r1,r4
	bf .irreg3

	mov.l _reg_iff2,r0
	rts
	mov.b @r0,r0

.irreg3:
	mov #Z80_REG_IRQLine,r1
	cmp/eq r1,r4
	bf .irreg4

	mov.l _reg_irqline,r0
	rts
	mov.b @r0,r0

.irreg4:
	mov #Z80_REG_Halted,r1
	cmp/eq r1,r4
	bf .irreg5

	mov.l _z80_halted_gr,r0
	rts
	mov.b @r0,r0

.irreg5:
	mov #Z80_REG_IM,r1
	cmp/eq r1,r4
	bf .irreg6

	mov.l _reg_im,r0
	mov.b @r0,r0
	rts
	extu.b r0,r0

.irreg6:
	mov #Z80_REG_IRQVector,r1
	cmp/eq r1,r4
	bf .irreg_gr

	mov.l _reg_irqvector,r0
	mov.b @r0,r0
	rts
	extu.b r0,r0

.irreg_gr:
	mov.l _reg_z80_R,r0
	mov.b @r0,r2                       ! r2 = R bajo (word)
	mov #0x7f,r4                       ! r4 = mascara R
	mov.l _reg_z80_R2,r0
	mov.b @r0,r3                       ! r3 = R2 bajo (word)
	and r4,r2                          ! r2 = R enmascarado
	mov #0x80,r4                       ! r4 = mascara R2 extendida
	mov.l _reg_z80_I,r1
	mov.b @r1,r0                       ! r0 = I
	extu.b r4,r4                       ! r4 = mascara R2 sin extension
	and r4,r3                          ! r3 = R2 enmascarado
	extu.b r0,r0                       ! r0 = I sin extension
	or r2,r3                           ! r3 = R completo
	shll8 r0                           ! colocar I en registro
	rts
	or r3,r0                           ! cargar R en resultado

ALIGN_DATA
	_context_start_gr: .long context_start
	_reg_iff1:         .long _z80_IFF1
	_reg_iff2:         .long _z80_IFF2
	_reg_irqline:      .long _z80_IRQLine
	_z80_halted_gr:    .long _z80_halted
	_reg_im:           .long _z80_IM
	_reg_irqvector:    .long _z80_IRQVector
	_reg_z80_R:        .long _z80_R
	_reg_z80_R2:      .long _z80_R2
	_reg_z80_I:        .long _z80_I


! void z80_set_reg(z80_register reg, UWORD value)
! Set the specified register to a new value.
ALIGN
.global _z80_set_reg
_z80_set_reg:
	extu.w r4,r4
	mov #Z80_REG_HL2,r1
	cmp/hi r1,r4
	bt .sirreg

	! Normal register set
	mov.l _context_start_sr,r0
	shll2 r4
	rts
	mov.l r5,@(r0,r4)

.sirreg:
	mov #Z80_REG_IFF1,r1
	cmp/eq r1,r4
	bf .srreg2

	tst r5,r5
	movt r0
	mov.l _sreg_iff1,r1
	dt r0
	and #FLAG_P,r0
	rts
	mov.b r0,@r1

.srreg2:
	mov #Z80_REG_IFF2,r1
	cmp/eq r1,r4
	bf .srreg3

	tst r5,r5
	movt r0
	mov.l _sreg_iff2,r1
	dt r0
	and #FLAG_P,r0
	rts
	mov.b r0,@r1

.srreg3:
	mov #Z80_REG_IRQLine,r1
	cmp/eq r1,r4
	bf .srreg4

	tst r5,r5
	movt r0
	mov.l _sreg_irqline,r1
	dt r0
	and #FLAG_P,r0
	rts
	mov.b r0,@r1

.srreg4:
	mov #Z80_REG_Halted,r1
	cmp/eq r1,r4
	bf .srreg5

	tst r5,r5
	movt r0
	mov.l _z80_shalted_gr,r1
	dt r0
	and #FLAG_P,r0
	rts
	mov.b r0,@r1

.srreg5:
	mov #Z80_REG_IM,r1
	cmp/eq r1,r4
	bf .srreg6

	mov.l _sreg_im,r0
	rts
	mov.b r5,@r0


.srreg6:
	mov #Z80_REG_IRQVector,r1
	cmp/eq r1,r4
	bf .srreg_gr

	mov.l _sreg_irqvector,r0
	rts
	mov.b r5,@r0


.srreg_gr:
	! Fijar registro IR
	! No se puede cargar como palabra larga por alineamiento
	mov.l _sreg_z80_R,r0

	mov r5,r1
	mov.w r5,@r0        ! first R word
	shlr16 r1
	add #2,r0
	mov.w r1,@r0        ! second R word
	mov.l _sreg_z80_R2,r0
	mov.w r5,@r0        ! first R2 word
	add #2,r0
	mov.w r1,@r0        ! second R2 word
	shlr8 r5
	mov.l _sreg_z80_I,r0
	rts
	mov.b r5,@r0        ! I register


ALIGN_DATA
	_context_start_sr: .long context_start
	_sreg_iff1:         .long _z80_IFF1
	_sreg_iff2:         .long _z80_IFF2
	_sreg_irqline:      .long _z80_IRQLine
	_z80_shalted_gr:    .long _z80_halted
	_sreg_im:           .long _z80_IM
	_sreg_irqvector:    .long _z80_IRQVector
	_sreg_z80_R:        .long _z80_R
	_sreg_z80_R2:       .long _z80_R2
	_sreg_z80_I:        .long _z80_I



! void z80_init_memmap(void)
! Reset the current memory map
ALIGN
.global _z80_init_memmap
_z80_init_memmap:

	! set the default in/out/reti/fetch handlers
	mov.l _z80_In_imm,r0
	mov.l z80_default_in_imm,r1
	mov.l r1,@r0                       ! z80_In
	mov.l z80_default_out_imm,r1
	mov.l r1,@(_z80_Out-_z80_In,r0)    ! z80_Out
	mov.l z80_default_reti_imm,r1
	mov.l r1,@(_z80_RetI-_z80_In,r0)   ! z80_RetI
	mov.l z80_def_fetch_callback_addr,r1
	ifdef(`SINGLE_MEM_BLOCK',`rts')
	mov.l r1,@(_z80_Fetch_Callback-_z80_In,r0)   ! z80_Fetch_Callback
	
ifdef(`SINGLE_MEM_BLOCK',`',
`
	ifdef(`SINGLE_MEM_HANDLER',`',
	`
		! reset the read table
		mov.l _z80_Read_imm,r0
		mov.l z80_default_read_imm,r1
		mov.l rw_table_length,r2
		mov r2,r3                   ! save rw table length

	rrt_loop:
		mov.l r1,@r0
		dt r2
		bf/s rrt_loop
		add #8,r0


		! reset the write table
		mov.l _z80_Write_imm,r0
		mov.l z80_default_write_imm,r1

	rwt_loop:
		mov.l r1,@r0
		dt r3
		bf/s rwt_loop
		add #8,r0

		rts
		nop
	')
')

	ALIGN_DATA
	_z80_In_imm:         .long _z80_In
	z80_default_in_imm:  .long z80_default_in
	z80_default_out_imm: .long z80_default_out
	z80_default_reti_imm: .long z80_default_reti
	z80_def_fetch_callback_addr: .long z80_default_fetch_callback

ifdef(`SINGLE_MEM_BLOCK',`',
`
	ifdef(`SINGLE_MEM_HANDLER',`',
	`
		_z80_Read_imm: .long _z80_Read
		_z80_Write_imm: .long _z80_Write
		z80_default_read_imm: .long z80_default_read
		z80_default_write_imm: .long z80_default_write
		rw_table_length: .long 256
	')
')


! void z80_end_memmap(void)
! Finishes the current memory map
! Empty, just to keep RAZE compatibility
ALIGN
.global _z80_end_memmap
_z80_end_memmap:
	rts
	nop


! void z80_map_fetch(UWORD start, UWORD end, UBYTE *memory)
! Set the given area for op-code fetching
! start/end = the area it covers
! memory = the ROM/RAM to read from
ALIGN
.global _z80_map_fetch
_z80_map_fetch:
	ifdef(`VBT2',
	`
        mov.l r6,@r9
	',
	`
	')
! r9 est dans r6 
! les params sont dans r4 à r6


	extu.w r4,r4    ! 16-bit start address
	mov.l _z80_Fetch_addr,r0
	sub r4,r6

ifdef(`SINGLE_FETCH',
`
	rts
	mov.l r6,@r0
',
`
	mov #1,r1
	extu.w r5,r5   ! 16-bit end address
	shll8 r1       ! r1 = incr. banco = 256

mf_loop:
    ! if theres no area left inbetween, stop now.
	mov r4,r7
	shlr8 r7

	cmp/hi r5,r4
	bt/s mf_finish
	shll2 r7

	mov.l r6,@(r0,r7)
	
	bra mf_loop
	add r1,r4

mf_finish:
	rts
	nop
')

	ALIGN_DATA
	_z80_Fetch_addr: .long _z80_Fetch


! void z80_map_read(UWORD start, UWORD end, UBYTE *memory)
! Moves a Z80_MAP_MAPPED area to use a different region of memory
! start/end = the area it covers (must be page-aligned)
! memory = the ROM/RAM to read from
ALIGN
.global _z80_map_read
_z80_map_read:

ifdef(`SINGLE_MEM_HANDLER',`',
`
	extu.w r4,r4
	mov.l _z80_Read_addr,r0
	sub r4,r6
')

ifdef(`SINGLE_MEM_BLOCK',
`
	rts
	mov.l r6,@r0
',
`
	ifdef(`SINGLE_MEM_HANDLER',
	`
		rts
		nop
	',
	`
		mov #1,r1
		extu.w r5,r5

		ifdef(`SH2_COMPAT_CODE',
		`
		',
		`
			mov #3,r3      ! 2^3 ancho datos descriptores
		')

		shll8 r1       ! r1 = incr. banco = 256
		mov #0,r2

	mr_loop:
		! if theres no area left inbetween, stop now.
		mov r4,r7
		shlr8 r7
	
		cmp/hi r5,r4
		bt/s mr_finish

		ifdef(`SH2_COMPAT_CODE',
		`
			shll2 r7
			shll r7
		',
		`
			shld r3,r7
		')

		mov.l r2,@(r0,r7)    ! volcar 0
		add #4,r7
		mov.l r6,@(r0,r7)
		bra mr_loop          ! continuar
		add r1,r4            ! incrementando banco

	mr_finish:
		rts
		nop
	')
')

ifdef(`SINGLE_MEM_HANDLER',`',
`
	ALIGN_DATA
	_z80_Read_addr: .long _z80_Read
')


! void z80_map_write(UWORD start, UWORD end, UBYTE *memory)
! Moves a Z80_MAP_MAPPED area to use a different region of memory
! start/end = the area it covers (must be page-aligned)
! memory = the ROM/RAM to read from
ALIGN
.global _z80_map_write
_z80_map_write:

ifdef(`SINGLE_MEM_HANDLER',`',
`
	extu.w r4,r4
	mov.l _z80_Write_addr,r0
	sub r4,r6
')

ifdef(`SINGLE_MEM_BLOCK',
`
	rts
	mov.l r6,@r0
',
`
	ifdef(`SINGLE_MEM_HANDLER',
	`
		rts
		nop
	',
	`
		mov #1,r1
		extu.w r5,r5

		ifdef(`SH2_COMPAT_CODE',
		`
		',
		`
			mov #3,r3      ! 2^3 ancho datos descriptores
		')

		shll8 r1       ! r1 = incr. banco = 256
		mov #0,r2

	mw_loop:
		! if theres no area left inbetween, stop now.
		mov r4,r7
		shlr8 r7

		cmp/hi r5,r4
		bt/s mw_finish

		ifdef(`SH2_COMPAT_CODE',
		`
			shll2 r7
			shll r7
		',
		`
			shld r3,r7
		')

		mov.l r2,@(r0,r7)
		add #4,r7
		mov.l r6,@(r0,r7)
		bra mw_loop          ! continuar
		add r1,r4            ! incr. banco

	mw_finish:
		rts
		nop
	')
')

ifdef(`SINGLE_MEM_HANDLER',`',
`
	ALIGN_DATA
	_z80_Write_addr: .long _z80_Write
')

! void z80_add_read(UWORD start, UWORD end, int method, void *data)
! Add a READ memory handler to the memory map
! start/end = the area it covers
! method = 0 for direct, 1 for handled
! data = RAM for direct, handler for handled

ifdef(`NO_READ_HANDLER',`',
	`ALIGN
	.global _z80_add_read
	_z80_add_read:

	ifdef(`SINGLE_MEM_BLOCK',
	`
		rts
		nop
	',
	`
		ifdef(`SINGLE_MEM_HANDLER',
		`
			tst r6,r6    ! test direct access
			bt ar_finish

			mov.l _z80_Read_ar,r0
			rts
			mov.l r7,@r0
		',
		`
			extu.w r4,r4
			mov.l _z80_Read_ar,r0
			mov #1,r1
			extu.w r5,r5

			! if direct, subtract the start from the memory area
			tst r6,r6

			bf/s ar_dont_adjust
			shll8 r1       ! r1 = incr. banco = 256

			mov #0,r2

			sub r4,r7
			bra ar_loop
			mov r7,r6

		ar_dont_adjust:
			mov #0,r6
			mov r7,r2

		ar_loop:
			! if theres no area left inbetween, stop now.
			mov r4,r7
			shlr8 r7
		
			cmp/hi r5,r4
			bt/s ar_finish	
			shll2 r7
			shll r7

			mov.l r2,@(r0,r7)
			add #4,r7
			mov.l r6,@(r0,r7)
			bra ar_loop          ! continuar
			add r1,r4            ! incr. banco

		')

		ar_finish:
			rts
			nop
	')

	ifdef(`SINGLE_MEM_BLOCK',`',
	`
		ALIGN_DATA
		_z80_Read_ar: .long _z80_Read
	')
')

! void z80_add_write(UWORD start, UWORD end, int method, void *data)
! Add a WRITE memory handler to the memory map
! start/end = the area it covers
! method = 0 for direct, 1 for handled
! data = RAM for direct, handler for handled
ALIGN
.global _z80_add_write
_z80_add_write:

ifdef(`SINGLE_MEM_BLOCK',
`
	rts
	nop
',
`
	ifdef(`SINGLE_MEM_HANDLER',
	`
		tst r6,r6    ! test direct access
		bt aw_finish

		mov.l _z80_Write_aw,r0
		rts
		mov.l r7,@r0
	',
	`
		extu.w r4,r4
		mov.l _z80_Write_aw,r0
		mov #1,r1
		extu.w r5,r5

		! if direct, subtract the start from the memory area
		tst r6,r6

		bf/s aw_dont_adjust
		shll8 r1       ! r1 = incr. banco = 256

		mov #0,r2

		sub r4,r7
		bra aw_loop
		mov r7,r6

	aw_dont_adjust:
		mov #0,r6
		mov r7,r2

	aw_loop:
		! if theres no area left inbetween, stop now.
		mov r4,r7
		shlr8 r7
	
		cmp/hi r5,r4
		bt/s aw_finish	
		shll2 r7
		shll r7

		mov.l r2,@(r0,r7)
		add #4,r7
		mov.l r6,@(r0,r7)
		bra aw_loop          ! continuar
		add r1,r4            ! incr. banco
	')

	aw_finish:
		rts
		nop
')

ifdef(`SINGLE_MEM_BLOCK',`',
`
	ALIGN_DATA
	_z80_Write_aw: .long _z80_Write
')


! void z80_set_in(UBYTE (*handler)(UWORD port))
! Set the IN port handler to the given function
ALIGN
.global _z80_set_in
_z80_set_in:
	mov.l _z80_In_si,r0
	rts
	mov.l r4,@r0

	ALIGN_DATA
	_z80_In_si: .long _z80_In


! void z80_set_out(void (*handler)(UWORD port, UBYTE value))
! Set the OUT port handler to the given function
ALIGN
.global _z80_set_out
_z80_set_out:
	mov.l _z80_Out_so,r0
	rts
	mov.l r4,@r0
   
	ALIGN_DATA
	_z80_Out_so: .long _z80_Out


! void z80_set_reti(void (*handler)(void))
! Set the RETI handler to the given function
ALIGN
.global _z80_set_reti
_z80_set_reti:
	mov.l _z80_RetI_sr,r0
	rts
	mov.l r4,@r0

	ALIGN_DATA
	_z80_RetI_sr: .long _z80_RetI


! void z80_set_fetch_callback(void (*handler)(UWORD pc))
! Set the fetch callback to the given function
ALIGN
.global _z80_set_fetch_callback
_z80_set_fetch_callback:
	mov.l _z80_FC_sfc,r0
	rts
	mov.l r4,@r0
   
	ALIGN_DATA
	_z80_FC_sfc: .long _z80_Fetch_Callback


!----------------------------------------------------------------------------!
! the jump tables
.data
ALIGN
   .long op_80, op_81, op_82, op_83, op_84, op_85, op_86, op_87
   .long op_88, op_89, op_8a, op_8b, op_8c, op_8d, op_8e, op_8f
   .long op_90, op_91, op_92, op_93, op_94, op_95, op_96, op_97
   .long op_98, op_99, op_9a, op_9b, op_9c, op_9d, op_9e, op_9f
   .long op_a0, op_a1, op_a2, op_a3, op_a4, op_a5, op_a6, op_a7
   .long op_a8, op_a9, op_aa, op_ab, op_ac, op_ad, op_ae, op_af
   .long op_b0, op_b1, op_b2, op_b3, op_b4, op_b5, op_b6, op_b7
   .long op_b8, op_b9, op_ba, op_bb, op_bc, op_bd, op_be, op_bf
   .long op_c0, op_c1, op_c2, op_c3, op_c4, op_c5, op_c6, op_c7
   .long op_c8, op_c9, op_ca, op_cb, op_cc, op_cd, op_ce, op_cf
   .long op_d0, op_d1, op_d2, op_d3, op_d4, op_d5, op_d6, op_d7
   .long op_d8, op_d9, op_da, op_db, op_dc, op_dd, op_de, op_df
   .long op_e0, op_e1, op_e2, op_e3, op_e4, op_e5, op_e6, op_e7
   .long op_e8, op_e9, op_ea, op_eb, op_ec, op_ed, op_ee, op_ef
   .long op_f0, op_f1, op_f2, op_f3, op_f4, op_f5, op_f6, op_f7
   .long op_f8, op_f9, op_fa, op_fb, op_fc, op_fd, op_fe, op_ff
OpTable:
   .long op_00, op_01, op_02, op_03, op_04, op_05, op_06, op_07
   .long op_08, op_09, op_0a, op_0b, op_0c, op_0d, op_0e, op_0f
   .long op_10, op_11, op_12, op_13, op_14, op_15, op_16, op_17
   .long op_18, op_19, op_1a, op_1b, op_1c, op_1d, op_1e, op_1f
   .long op_20, op_21, op_22, op_23, op_24, op_25, op_26, op_27
   .long op_28, op_29, op_2a, op_2b, op_2c, op_2d, op_2e, op_2f
   .long op_30, op_31, op_32, op_33, op_34, op_35, op_36, op_37
   .long op_38, op_39, op_3a, op_3b, op_3c, op_3d, op_3e, op_3f
   .long op_40, op_41, op_42, op_43, op_44, op_45, op_46, op_47
   .long op_48, op_49, op_4a, op_4b, op_4c, op_4d, op_4e, op_4f
   .long op_50, op_51, op_52, op_53, op_54, op_55, op_56, op_57
   .long op_58, op_59, op_5a, op_5b, op_5c, op_5d, op_5e, op_5f
   .long op_60, op_61, op_62, op_63, op_64, op_65, op_66, op_67
   .long op_68, op_69, op_6a, op_6b, op_6c, op_6d, op_6e, op_6f
   .long op_70, op_71, op_72, op_73, op_74, op_75, op_76, op_77
   .long op_78, op_79, op_7a, op_7b, op_7c, op_7d, op_7e, op_7f


   .long cb_80, cb_81, cb_82, cb_83, cb_84, cb_85, cb_86, cb_87
   .long cb_88, cb_89, cb_8a, cb_8b, cb_8c, cb_8d, cb_8e, cb_8f
   .long cb_90, cb_91, cb_92, cb_93, cb_94, cb_95, cb_96, cb_97
   .long cb_98, cb_99, cb_9a, cb_9b, cb_9c, cb_9d, cb_9e, cb_9f
   .long cb_a0, cb_a1, cb_a2, cb_a3, cb_a4, cb_a5, cb_a6, cb_a7
   .long cb_a8, cb_a9, cb_aa, cb_ab, cb_ac, cb_ad, cb_ae, cb_af
   .long cb_b0, cb_b1, cb_b2, cb_b3, cb_b4, cb_b5, cb_b6, cb_b7
   .long cb_b8, cb_b9, cb_ba, cb_bb, cb_bc, cb_bd, cb_be, cb_bf
   .long cb_c0, cb_c1, cb_c2, cb_c3, cb_c4, cb_c5, cb_c6, cb_c7
   .long cb_c8, cb_c9, cb_ca, cb_cb, cb_cc, cb_cd, cb_ce, cb_cf
   .long cb_d0, cb_d1, cb_d2, cb_d3, cb_d4, cb_d5, cb_d6, cb_d7
   .long cb_d8, cb_d9, cb_da, cb_db, cb_dc, cb_dd, cb_de, cb_df
   .long cb_e0, cb_e1, cb_e2, cb_e3, cb_e4, cb_e5, cb_e6, cb_e7
   .long cb_e8, cb_e9, cb_ea, cb_eb, cb_ec, cb_ed, cb_ee, cb_ef
   .long cb_f0, cb_f1, cb_f2, cb_f3, cb_f4, cb_f5, cb_f6, cb_f7
   .long cb_f8, cb_f9, cb_fa, cb_fb, cb_fc, cb_fd, cb_fe, cb_ff
CBTable:
   .long cb_00, cb_01, cb_02, cb_03, cb_04, cb_05, cb_06, cb_07
   .long cb_08, cb_09, cb_0a, cb_0b, cb_0c, cb_0d, cb_0e, cb_0f
   .long cb_10, cb_11, cb_12, cb_13, cb_14, cb_15, cb_16, cb_17
   .long cb_18, cb_19, cb_1a, cb_1b, cb_1c, cb_1d, cb_1e, cb_1f
   .long cb_20, cb_21, cb_22, cb_23, cb_24, cb_25, cb_26, cb_27
   .long cb_28, cb_29, cb_2a, cb_2b, cb_2c, cb_2d, cb_2e, cb_2f
   .long cb_30, cb_31, cb_32, cb_33, cb_34, cb_35, cb_36, cb_37
   .long cb_38, cb_39, cb_3a, cb_3b, cb_3c, cb_3d, cb_3e, cb_3f
   .long cb_40, cb_41, cb_42, cb_43, cb_44, cb_45, cb_46, cb_47
   .long cb_48, cb_49, cb_4a, cb_4b, cb_4c, cb_4d, cb_4e, cb_4f
   .long cb_50, cb_51, cb_52, cb_53, cb_54, cb_55, cb_56, cb_57
   .long cb_58, cb_59, cb_5a, cb_5b, cb_5c, cb_5d, cb_5e, cb_5f
   .long cb_60, cb_61, cb_62, cb_63, cb_64, cb_65, cb_66, cb_67
   .long cb_68, cb_69, cb_6a, cb_6b, cb_6c, cb_6d, cb_6e, cb_6f
   .long cb_70, cb_71, cb_72, cb_73, cb_74, cb_75, cb_76, cb_77
   .long cb_78, cb_79, cb_7a, cb_7b, cb_7c, cb_7d, cb_7e, cb_7f


   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_a0, ed_a1, ed_a2, ed_a3, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_a8, ed_a9, ed_aa, ed_ab, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_b0, ed_b1, ed_b2, ed_b3, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_b8, ed_b9, ed_ba, ed_bb, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx 
 EDTable:
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx, ed_xx
   .long ed_40, ed_41, ed_42, ed_43, ed_44, ed_45, ed_46, ed_47
   .long ed_48, ed_49, ed_4a, ed_4b, ed_4c, ed_4d, ed_4e, ed_4f
   .long ed_50, ed_51, ed_52, ed_53, ed_54, ed_55, ed_56, ed_57
   .long ed_58, ed_59, ed_5a, ed_5b, ed_5c, ed_5d, ed_5e, ed_5f
   .long ed_60, ed_61, ed_62, ed_63, ed_64, ed_65, ed_66, ed_67
   .long ed_68, ed_69, ed_6a, ed_6b, ed_6c, ed_6d, ed_6e, ed_6f
   .long ed_70, ed_71, ed_72, ed_73, ed_74, ed_75, ed_76, ed_xx
   .long ed_78, ed_79, ed_7a, ed_7b, ed_7c, ed_7d, ed_7e, ed_xx


   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_84, dd_85, dd_86, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_8c, dd_8d, dd_8e, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_94, dd_95, dd_96, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_9c, dd_9d, dd_9e, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_a4, dd_a5, dd_a6, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_ac, dd_ad, dd_ae, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_b4, dd_b5, dd_b6, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_bc, dd_bd, dd_be, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_cb, dd_xx, dd_xx, dd_xx, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx
   .long dd_xx, dd_e1, dd_xx, dd_e3, dd_xx, dd_e5, dd_xx, dd_xx
   .long dd_xx, dd_e9, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx
   .long dd_xx, dd_f9, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx
DDTable:
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx
   .long dd_xx, dd_09, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx
   .long dd_xx, dd_19, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx
   .long dd_xx, dd_21, dd_22, dd_23, dd_24, dd_25, dd_26, dd_xx
   .long dd_xx, dd_29, dd_2a, dd_2b, dd_2c, dd_2d, dd_2e, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_34, dd_35, dd_36, dd_xx
   .long dd_xx, dd_39, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_44, dd_45, dd_46, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_4c, dd_4d, dd_4e, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_54, dd_55, dd_56, dd_xx
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_5c, dd_5d, dd_5e, dd_xx
   .long dd_60, dd_61, dd_62, dd_63, dd_xx, dd_65, dd_66, dd_67
   .long dd_68, dd_69, dd_6a, dd_6b, dd_6c, dd_xx, dd_6e, dd_6f
   .long dd_70, dd_71, dd_72, dd_73, dd_74, dd_75, dd_xx, dd_77
   .long dd_xx, dd_xx, dd_xx, dd_xx, dd_7c, dd_7d, dd_7e, dd_xx


   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_84, fd_85, fd_86, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_8c, fd_8d, fd_8e, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_94, fd_95, fd_96, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_9c, fd_9d, fd_9e, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_a4, fd_a5, fd_a6, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_ac, fd_ad, fd_ae, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_b4, fd_b5, fd_b6, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_bc, fd_bd, fd_be, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_cb, fd_xx, fd_xx, fd_xx, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx
   .long fd_xx, fd_e1, fd_xx, fd_e3, fd_xx, fd_e5, fd_xx, fd_xx
   .long fd_xx, fd_e9, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx
   .long fd_xx, fd_f9, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx
FDTable:
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx
   .long fd_xx, fd_09, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx
   .long fd_xx, fd_19, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx
   .long fd_xx, fd_21, fd_22, fd_23, fd_24, fd_25, fd_26, fd_xx
   .long fd_xx, fd_29, fd_2a, fd_2b, fd_2c, fd_2d, fd_2e, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_34, fd_35, fd_36, fd_xx
   .long fd_xx, fd_39, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_44, fd_45, fd_46, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_4c, fd_4d, fd_4e, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_54, fd_55, fd_56, fd_xx
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_5c, fd_5d, fd_5e, fd_xx
   .long fd_60, fd_61, fd_62, fd_63, fd_xx, fd_65, fd_66, fd_67
   .long fd_68, fd_69, fd_6a, fd_6b, fd_6c, fd_xx, fd_6e, fd_6f
   .long fd_70, fd_71, fd_72, fd_73, fd_74, fd_75, fd_xx, fd_77
   .long fd_xx, fd_xx, fd_xx, fd_xx, fd_7c, fd_7d, fd_7e, fd_xx


   .long ddcb_80, ddcb_81, ddcb_82, ddcb_83, ddcb_84, ddcb_85, ddcb_86, ddcb_87
   .long ddcb_88, ddcb_89, ddcb_8a, ddcb_8b, ddcb_8c, ddcb_8d, ddcb_8e, ddcb_8f
   .long ddcb_90, ddcb_91, ddcb_92, ddcb_93, ddcb_94, ddcb_95, ddcb_96, ddcb_97
   .long ddcb_98, ddcb_99, ddcb_9a, ddcb_9b, ddcb_9c, ddcb_9d, ddcb_9e, ddcb_9f
   .long ddcb_a0, ddcb_a1, ddcb_a2, ddcb_a3, ddcb_a4, ddcb_a5, ddcb_a6, ddcb_a7
   .long ddcb_a8, ddcb_a9, ddcb_aa, ddcb_ab, ddcb_ac, ddcb_ad, ddcb_ae, ddcb_af
   .long ddcb_b0, ddcb_b1, ddcb_b2, ddcb_b3, ddcb_b4, ddcb_b5, ddcb_b6, ddcb_b7
   .long ddcb_b8, ddcb_b9, ddcb_ba, ddcb_bb, ddcb_bc, ddcb_bd, ddcb_be, ddcb_bf
   .long ddcb_c0, ddcb_c1, ddcb_c2, ddcb_c3, ddcb_c4, ddcb_c5, ddcb_c6, ddcb_c7
   .long ddcb_c8, ddcb_c9, ddcb_ca, ddcb_cb, ddcb_cc, ddcb_cd, ddcb_ce, ddcb_cf
   .long ddcb_d0, ddcb_d1, ddcb_d2, ddcb_d3, ddcb_d4, ddcb_d5, ddcb_d6, ddcb_d7
   .long ddcb_d8, ddcb_d9, ddcb_da, ddcb_db, ddcb_dc, ddcb_dd, ddcb_de, ddcb_df
   .long ddcb_e0, ddcb_e1, ddcb_e2, ddcb_e3, ddcb_e4, ddcb_e5, ddcb_e6, ddcb_e7
   .long ddcb_e8, ddcb_e9, ddcb_ea, ddcb_eb, ddcb_ec, ddcb_ed, ddcb_ee, ddcb_ef
   .long ddcb_f0, ddcb_f1, ddcb_f2, ddcb_f3, ddcb_f4, ddcb_f5, ddcb_f6, ddcb_f7
   .long ddcb_f8, ddcb_f9, ddcb_fa, ddcb_fb, ddcb_fc, ddcb_fd, ddcb_fe, ddcb_ff
DDCBTable:
   .long ddcb_00, ddcb_01, ddcb_02, ddcb_03, ddcb_04, ddcb_05, ddcb_06, ddcb_07
   .long ddcb_08, ddcb_09, ddcb_0a, ddcb_0b, ddcb_0c, ddcb_0d, ddcb_0e, ddcb_0f
   .long ddcb_10, ddcb_11, ddcb_12, ddcb_13, ddcb_14, ddcb_15, ddcb_16, ddcb_17
   .long ddcb_18, ddcb_19, ddcb_1a, ddcb_1b, ddcb_1c, ddcb_1d, ddcb_1e, ddcb_1f
   .long ddcb_20, ddcb_21, ddcb_22, ddcb_23, ddcb_24, ddcb_25, ddcb_26, ddcb_27
   .long ddcb_28, ddcb_29, ddcb_2a, ddcb_2b, ddcb_2c, ddcb_2d, ddcb_2e, ddcb_2f
   .long ddcb_30, ddcb_31, ddcb_32, ddcb_33, ddcb_34, ddcb_35, ddcb_36, ddcb_37
   .long ddcb_38, ddcb_39, ddcb_3a, ddcb_3b, ddcb_3c, ddcb_3d, ddcb_3e, ddcb_3f
   .long ddcb_40, ddcb_41, ddcb_42, ddcb_43, ddcb_44, ddcb_45, ddcb_46, ddcb_47
   .long ddcb_48, ddcb_49, ddcb_4a, ddcb_4b, ddcb_4c, ddcb_4d, ddcb_4e, ddcb_4f
   .long ddcb_50, ddcb_51, ddcb_52, ddcb_53, ddcb_54, ddcb_55, ddcb_56, ddcb_57
   .long ddcb_58, ddcb_59, ddcb_5a, ddcb_5b, ddcb_5c, ddcb_5d, ddcb_5e, ddcb_5f
   .long ddcb_60, ddcb_61, ddcb_62, ddcb_63, ddcb_64, ddcb_65, ddcb_66, ddcb_67
   .long ddcb_68, ddcb_69, ddcb_6a, ddcb_6b, ddcb_6c, ddcb_6d, ddcb_6e, ddcb_6f
   .long ddcb_70, ddcb_71, ddcb_72, ddcb_73, ddcb_74, ddcb_75, ddcb_76, ddcb_77
   .long ddcb_78, ddcb_79, ddcb_7a, ddcb_7b, ddcb_7c, ddcb_7d, ddcb_7e, ddcb_7f


!----------------------------------------------------------------------------!
! Main flag tables
ALIGN

.byte 0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80
.byte 0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84
.byte 0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84
.byte 0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80
.byte 0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84
.byte 0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80
.byte 0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84,0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80
.byte 0x84,0x80,0x80,0x84,0x80,0x84,0x84,0x80,0x80,0x84,0x84,0x80,0x84,0x80,0x80,0x84
Byte_Flags:
.byte 0x44,0x00,0x00,0x04,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04
.byte 0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00
.byte 0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00
.byte 0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04
.byte 0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00
.byte 0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04
.byte 0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00,0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04
.byte 0x00,0x04,0x04,0x00,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x00,0x04,0x04,0x00

ALIGN
! flag tables ('borrowed' from MAZE, by Ishmair)
.byte  128,128,128,128,128,128,128,136,136,136,136,136
.byte  136,136,136,144,128,128,128,128,128,128,128,136,136,136,136,136,136,136,136,176
.byte  160,160,160,160,160,160,160,168,168,168,168,168,168,168,168,176,160,160,160,160
.byte  160,160,160,168,168,168,168,168,168,168,168,144,128,128,128,128,128,128,128,136
.byte  136,136,136,136,136,136,136,144,128,128,128,128,128,128,128,136,136,136,136,136
.byte  136,136,136,176,160,160,160,160,160,160,160,168,168,168,168,168,168,168,168,176
.byte  160,160,160,160,160,160,160,168,168,168,168,168,168,168,168,80
INC_Table:
.byte  0,0,0,0,0,0,0,8,8,8,8,8,8,8,8,16,0,0,0,0
.byte  0,0,0,8,8,8,8,8,8,8,8,48,32,32,32,32,32,32,32,40
.byte  40,40,40,40,40,40,40,48,32,32,32,32,32,32,32,40,40,40,40,40
.byte  40,40,40,16,0,0,0,0,0,0,0,8,8,8,8,8,8,8,8,16
.byte  0,0,0,0,0,0,0,8,8,8,8,8,8,8,8,48,32,32,32,32
.byte  32,32,32,40,40,40,40,40,40,40,40,48,32,32,32,32,32,32,32,40
.byte  40,40,40,40,40,40,40,148

ALIGN
.byte  62,130,130,130,130,130,130,130,130,138,138,138
.byte  138,138,138,138,154,130,130,130,130,130,130,130,130,138,138,138,138,138,138,138
.byte  154,162,162,162,162,162,162,162,162,170,170,170,170,170,170,170,186,162,162,162
.byte  162,162,162,162,162,170,170,170,170,170,170,170,186,130,130,130,130,130,130,130
.byte  130,138,138,138,138,138,138,138,154,130,130,130,130,130,130,130,130,138,138,138
.byte  138,138,138,138,154,162,162,162,162,162,162,162,162,170,170,170,170,170,170,170
.byte  186,162,162,162,162,162,162,162,162,170,170,170,170,170,170,170
DEC_Table:
.byte  186,66,2,2,2,2,2,2,2,10,10,10,10,10,10,10,26,2,2,2
.byte  2,2,2,2,2,10,10,10,10,10,10,10,26,34,34,34,34,34,34,34
.byte  34,42,42,42,42,42,42,42,58,34,34,34,34,34,34,34,34,42,42,42
.byte  42,42,42,42,58,2,2,2,2,2,2,2,2,10,10,10,10,10,10,10
.byte  26,2,2,2,2,2,2,2,2,10,10,10,10,10,10,10,26,34,34,34
.byte  34,34,34,34,34,42,42,42,42,42,42,42,58,34,34,34,34,34,34,34
.byte  34,42,42,42,42,42,42,42

ALIGN
! DAA table ('borrowed' from Z80Emul, by unknown)
DAA_Table:
.word  0x4400, 0x0001, 0x0002, 0x0403, 0x0004, 0x0405, 0x0406, 0x0007
.word  0x0808, 0x0C09, 0x1010, 0x1411, 0x1412, 0x1013, 0x1414, 0x1015
.word  0x0010, 0x0411, 0x0412, 0x0013, 0x0414, 0x0015, 0x0016, 0x0417
.word  0x0C18, 0x0819, 0x3020, 0x3421, 0x3422, 0x3023, 0x3424, 0x3025
.word  0x2020, 0x2421, 0x2422, 0x2023, 0x2424, 0x2025, 0x2026, 0x2427
.word  0x2C28, 0x2829, 0x3430, 0x3031, 0x3032, 0x3433, 0x3034, 0x3435
.word  0x2430, 0x2031, 0x2032, 0x2433, 0x2034, 0x2435, 0x2436, 0x2037
.word  0x2838, 0x2C39, 0x1040, 0x1441, 0x1442, 0x1043, 0x1444, 0x1045
.word  0x0040, 0x0441, 0x0442, 0x0043, 0x0444, 0x0045, 0x0046, 0x0447
.word  0x0C48, 0x0849, 0x1450, 0x1051, 0x1052, 0x1453, 0x1054, 0x1455
.word  0x0450, 0x0051, 0x0052, 0x0453, 0x0054, 0x0455, 0x0456, 0x0057
.word  0x0858, 0x0C59, 0x3460, 0x3061, 0x3062, 0x3463, 0x3064, 0x3465
.word  0x2460, 0x2061, 0x2062, 0x2463, 0x2064, 0x2465, 0x2466, 0x2067
.word  0x2868, 0x2C69, 0x3070, 0x3471, 0x3472, 0x3073, 0x3474, 0x3075
.word  0x2070, 0x2471, 0x2472, 0x2073, 0x2474, 0x2075, 0x2076, 0x2477
.word  0x2C78, 0x2879, 0x9080, 0x9481, 0x9482, 0x9083, 0x9484, 0x9085
.word  0x8080, 0x8481, 0x8482, 0x8083, 0x8484, 0x8085, 0x8086, 0x8487
.word  0x8C88, 0x8889, 0x9490, 0x9091, 0x9092, 0x9493, 0x9094, 0x9495
.word  0x8490, 0x8091, 0x8092, 0x8493, 0x8094, 0x8495, 0x8496, 0x8097
.word  0x8898, 0x8C99, 0x5500, 0x1101, 0x1102, 0x1503, 0x1104, 0x1505
.word  0x4500, 0x0101, 0x0102, 0x0503, 0x0104, 0x0505, 0x0506, 0x0107
.word  0x0908, 0x0D09, 0x1110, 0x1511, 0x1512, 0x1113, 0x1514, 0x1115
.word  0x0110, 0x0511, 0x0512, 0x0113, 0x0514, 0x0115, 0x0116, 0x0517
.word  0x0D18, 0x0919, 0x3120, 0x3521, 0x3522, 0x3123, 0x3524, 0x3125
.word  0x2120, 0x2521, 0x2522, 0x2123, 0x2524, 0x2125, 0x2126, 0x2527
.word  0x2D28, 0x2929, 0x3530, 0x3131, 0x3132, 0x3533, 0x3134, 0x3535
.word  0x2530, 0x2131, 0x2132, 0x2533, 0x2134, 0x2535, 0x2536, 0x2137
.word  0x2938, 0x2D39, 0x1140, 0x1541, 0x1542, 0x1143, 0x1544, 0x1145
.word  0x0140, 0x0541, 0x0542, 0x0143, 0x0544, 0x0145, 0x0146, 0x0547
.word  0x0D48, 0x0949, 0x1550, 0x1151, 0x1152, 0x1553, 0x1154, 0x1555
.word  0x0550, 0x0151, 0x0152, 0x0553, 0x0154, 0x0555, 0x0556, 0x0157
.word  0x0958, 0x0D59, 0x3560, 0x3161, 0x3162, 0x3563, 0x3164, 0x3565
.word  0x2560, 0x2161, 0x2162, 0x2563, 0x2164, 0x2565, 0x2566, 0x2167
.word  0x2968, 0x2D69, 0x3170, 0x3571, 0x3572, 0x3173, 0x3574, 0x3175
.word  0x2170, 0x2571, 0x2572, 0x2173, 0x2574, 0x2175, 0x2176, 0x2577
.word  0x2D78, 0x2979, 0x9180, 0x9581, 0x9582, 0x9183, 0x9584, 0x9185
.word  0x8180, 0x8581, 0x8582, 0x8183, 0x8584, 0x8185, 0x8186, 0x8587
.word  0x8D88, 0x8989, 0x9590, 0x9191, 0x9192, 0x9593, 0x9194, 0x9595
.word  0x8590, 0x8191, 0x8192, 0x8593, 0x8194, 0x8595, 0x8596, 0x8197
.word  0x8998, 0x8D99, 0xB5A0, 0xB1A1, 0xB1A2, 0xB5A3, 0xB1A4, 0xB5A5
.word  0xA5A0, 0xA1A1, 0xA1A2, 0xA5A3, 0xA1A4, 0xA5A5, 0xA5A6, 0xA1A7
.word  0xA9A8, 0xADA9, 0xB1B0, 0xB5B1, 0xB5B2, 0xB1B3, 0xB5B4, 0xB1B5
.word  0xA1B0, 0xA5B1, 0xA5B2, 0xA1B3, 0xA5B4, 0xA1B5, 0xA1B6, 0xA5B7
.word  0xADB8, 0xA9B9, 0x95C0, 0x91C1, 0x91C2, 0x95C3, 0x91C4, 0x95C5
.word  0x85C0, 0x81C1, 0x81C2, 0x85C3, 0x81C4, 0x85C5, 0x85C6, 0x81C7
.word  0x89C8, 0x8DC9, 0x91D0, 0x95D1, 0x95D2, 0x91D3, 0x95D4, 0x91D5
.word  0x81D0, 0x85D1, 0x85D2, 0x81D3, 0x85D4, 0x81D5, 0x81D6, 0x85D7
.word  0x8DD8, 0x89D9, 0xB1E0, 0xB5E1, 0xB5E2, 0xB1E3, 0xB5E4, 0xB1E5
.word  0xA1E0, 0xA5E1, 0xA5E2, 0xA1E3, 0xA5E4, 0xA1E5, 0xA1E6, 0xA5E7
.word  0xADE8, 0xA9E9, 0xB5F0, 0xB1F1, 0xB1F2, 0xB5F3, 0xB1F4, 0xB5F5
.word  0xA5F0, 0xA1F1, 0xA1F2, 0xA5F3, 0xA1F4, 0xA5F5, 0xA5F6, 0xA1F7
.word  0xA9F8, 0xADF9, 0x5500, 0x1101, 0x1102, 0x1503, 0x1104, 0x1505
.word  0x4500, 0x0101, 0x0102, 0x0503, 0x0104, 0x0505, 0x0506, 0x0107
.word  0x0908, 0x0D09, 0x1110, 0x1511, 0x1512, 0x1113, 0x1514, 0x1115
.word  0x0110, 0x0511, 0x0512, 0x0113, 0x0514, 0x0115, 0x0116, 0x0517
.word  0x0D18, 0x0919, 0x3120, 0x3521, 0x3522, 0x3123, 0x3524, 0x3125
.word  0x2120, 0x2521, 0x2522, 0x2123, 0x2524, 0x2125, 0x2126, 0x2527
.word  0x2D28, 0x2929, 0x3530, 0x3131, 0x3132, 0x3533, 0x3134, 0x3535
.word  0x2530, 0x2131, 0x2132, 0x2533, 0x2134, 0x2535, 0x2536, 0x2137
.word  0x2938, 0x2D39, 0x1140, 0x1541, 0x1542, 0x1143, 0x1544, 0x1145
.word  0x0140, 0x0541, 0x0542, 0x0143, 0x0544, 0x0145, 0x0146, 0x0547
.word  0x0D48, 0x0949, 0x1550, 0x1151, 0x1152, 0x1553, 0x1154, 0x1555
.word  0x0550, 0x0151, 0x0152, 0x0553, 0x0154, 0x0555, 0x0556, 0x0157
.word  0x0958, 0x0D59, 0x3560, 0x3161, 0x3162, 0x3563, 0x3164, 0x3565
.word  0x4600, 0x0201, 0x0202, 0x0603, 0x0204, 0x0605, 0x0606, 0x0207
.word  0x0A08, 0x0E09, 0x0204, 0x0605, 0x0606, 0x0207, 0x0A08, 0x0E09
.word  0x0210, 0x0611, 0x0612, 0x0213, 0x0614, 0x0215, 0x0216, 0x0617
.word  0x0E18, 0x0A19, 0x0614, 0x0215, 0x0216, 0x0617, 0x0E18, 0x0A19
.word  0x2220, 0x2621, 0x2622, 0x2223, 0x2624, 0x2225, 0x2226, 0x2627
.word  0x2E28, 0x2A29, 0x2624, 0x2225, 0x2226, 0x2627, 0x2E28, 0x2A29
.word  0x2630, 0x2231, 0x2232, 0x2633, 0x2234, 0x2635, 0x2636, 0x2237
.word  0x2A38, 0x2E39, 0x2234, 0x2635, 0x2636, 0x2237, 0x2A38, 0x2E39
.word  0x0240, 0x0641, 0x0642, 0x0243, 0x0644, 0x0245, 0x0246, 0x0647
.word  0x0E48, 0x0A49, 0x0644, 0x0245, 0x0246, 0x0647, 0x0E48, 0x0A49
.word  0x0650, 0x0251, 0x0252, 0x0653, 0x0254, 0x0655, 0x0656, 0x0257

.word  0x0A58, 0x0E59, 0x0254, 0x0655, 0x0656, 0x0257, 0x0A58, 0x0E59
.word  0x2660, 0x2261, 0x2262, 0x2663, 0x2264, 0x2665, 0x2666, 0x2267
.word  0x2A68, 0x2E69, 0x2264, 0x2665, 0x2666, 0x2267, 0x2A68, 0x2E69
.word  0x2270, 0x2671, 0x2672, 0x2273, 0x2674, 0x2275, 0x2276, 0x2677
.word  0x2E78, 0x2A79, 0x2674, 0x2275, 0x2276, 0x2677, 0x2E78, 0x2A79
.word  0x8280, 0x8681, 0x8682, 0x8283, 0x8684, 0x8285, 0x8286, 0x8687
.word  0x8E88, 0x8A89, 0x8684, 0x8285, 0x8286, 0x8687, 0x8E88, 0x8A89
.word  0x8690, 0x8291, 0x8292, 0x8693, 0x8294, 0x8695, 0x8696, 0x8297
.word  0x8A98, 0x8E99, 0x2334, 0x2735, 0x2736, 0x2337, 0x2B38, 0x2F39
.word  0x0340, 0x0741, 0x0742, 0x0343, 0x0744, 0x0345, 0x0346, 0x0747
.word  0x0F48, 0x0B49, 0x0744, 0x0345, 0x0346, 0x0747, 0x0F48, 0x0B49
.word  0x0750, 0x0351, 0x0352, 0x0753, 0x0354, 0x0755, 0x0756, 0x0357
.word  0x0B58, 0x0F59, 0x0354, 0x0755, 0x0756, 0x0357, 0x0B58, 0x0F59
.word  0x2760, 0x2361, 0x2362, 0x2763, 0x2364, 0x2765, 0x2766, 0x2367
.word  0x2B68, 0x2F69, 0x2364, 0x2765, 0x2766, 0x2367, 0x2B68, 0x2F69
.word  0x2370, 0x2771, 0x2772, 0x2373, 0x2774, 0x2375, 0x2376, 0x2777
.word  0x2F78, 0x2B79, 0x2774, 0x2375, 0x2376, 0x2777, 0x2F78, 0x2B79
.word  0x8380, 0x8781, 0x8782, 0x8383, 0x8784, 0x8385, 0x8386, 0x8787
.word  0x8F88, 0x8B89, 0x8784, 0x8385, 0x8386, 0x8787, 0x8F88, 0x8B89
.word  0x8790, 0x8391, 0x8392, 0x8793, 0x8394, 0x8795, 0x8796, 0x8397
.word  0x8B98, 0x8F99, 0x8394, 0x8795, 0x8796, 0x8397, 0x8B98, 0x8F99
.word  0xA7A0, 0xA3A1, 0xA3A2, 0xA7A3, 0xA3A4, 0xA7A5, 0xA7A6, 0xA3A7
.word  0xABA8, 0xAFA9, 0xA3A4, 0xA7A5, 0xA7A6, 0xA3A7, 0xABA8, 0xAFA9
.word  0xA3B0, 0xA7B1, 0xA7B2, 0xA3B3, 0xA7B4, 0xA3B5, 0xA3B6, 0xA7B7
.word  0xAFB8, 0xABB9, 0xA7B4, 0xA3B5, 0xA3B6, 0xA7B7, 0xAFB8, 0xABB9
.word  0x87C0, 0x83C1, 0x83C2, 0x87C3, 0x83C4, 0x87C5, 0x87C6, 0x83C7
.word  0x8BC8, 0x8FC9, 0x83C4, 0x87C5, 0x87C6, 0x83C7, 0x8BC8, 0x8FC9
.word  0x83D0, 0x87D1, 0x87D2, 0x83D3, 0x87D4, 0x83D5, 0x83D6, 0x87D7
.word  0x8FD8, 0x8BD9, 0x87D4, 0x83D5, 0x83D6, 0x87D7, 0x8FD8, 0x8BD9
.word  0xA3E0, 0xA7E1, 0xA7E2, 0xA3E3, 0xA7E4, 0xA3E5, 0xA3E6, 0xA7E7
.word  0xAFE8, 0xABE9, 0xA7E4, 0xA3E5, 0xA3E6, 0xA7E7, 0xAFE8, 0xABE9
.word  0xA7F0, 0xA3F1, 0xA3F2, 0xA7F3, 0xA3F4, 0xA7F5, 0xA7F6, 0xA3F7
.word  0xABF8, 0xAFF9, 0xA3F4, 0xA7F5, 0xA7F6, 0xA3F7, 0xABF8, 0xAFF9
.word  0x4700, 0x0301, 0x0302, 0x0703, 0x0304, 0x0705, 0x0706, 0x0307
.word  0x0B08, 0x0F09, 0x0304, 0x0705, 0x0706, 0x0307, 0x0B08, 0x0F09
.word  0x0310, 0x0711, 0x0712, 0x0313, 0x0714, 0x0315, 0x0316, 0x0717
.word  0x0F18, 0x0B19, 0x0714, 0x0315, 0x0316, 0x0717, 0x0F18, 0x0B19
.word  0x2320, 0x2721, 0x2722, 0x2323, 0x2724, 0x2325, 0x2326, 0x2727
.word  0x2F28, 0x2B29, 0x2724, 0x2325, 0x2326, 0x2727, 0x2F28, 0x2B29
.word  0x2730, 0x2331, 0x2332, 0x2733, 0x2334, 0x2735, 0x2736, 0x2337
.word  0x2B38, 0x2F39, 0x2334, 0x2735, 0x2736, 0x2337, 0x2B38, 0x2F39
.word  0x0340, 0x0741, 0x0742, 0x0343, 0x0744, 0x0345, 0x0346, 0x0747
.word  0x0F48, 0x0B49, 0x0744, 0x0345, 0x0346, 0x0747, 0x0F48, 0x0B49
.word  0x0750, 0x0351, 0x0352, 0x0753, 0x0354, 0x0755, 0x0756, 0x0357
.word  0x0B58, 0x0F59, 0x0354, 0x0755, 0x0756, 0x0357, 0x0B58, 0x0F59
.word  0x2760, 0x2361, 0x2362, 0x2763, 0x2364, 0x2765, 0x2766, 0x2367
.word  0x2B68, 0x2F69, 0x2364, 0x2765, 0x2766, 0x2367, 0x2B68, 0x2F69
.word  0x2370, 0x2771, 0x2772, 0x2373, 0x2774, 0x2375, 0x2376, 0x2777
.word  0x2F78, 0x2B79, 0x2774, 0x2375, 0x2376, 0x2777, 0x2F78, 0x2B79
.word  0x8380, 0x8781, 0x8782, 0x8383, 0x8784, 0x8385, 0x8386, 0x8787
.word  0x8F88, 0x8B89, 0x8784, 0x8385, 0x8386, 0x8787, 0x8F88, 0x8B89
.word  0x8790, 0x8391, 0x8392, 0x8793, 0x8394, 0x8795, 0x8796, 0x8397
.word  0x8B98, 0x8F99, 0x8394, 0x8795, 0x8796, 0x8397, 0x8B98, 0x8F99
.word  0x0406, 0x0007, 0x0808, 0x0C09, 0x0C0A, 0x080B, 0x0C0C, 0x080D
.word  0x080E, 0x0C0F, 0x1010, 0x1411, 0x1412, 0x1013, 0x1414, 0x1015
.word  0x0016, 0x0417, 0x0C18, 0x0819, 0x081A, 0x0C1B, 0x081C, 0x0C1D
.word  0x0C1E, 0x081F, 0x3020, 0x3421, 0x3422, 0x3023, 0x3424, 0x3025
.word  0x2026, 0x2427, 0x2C28, 0x2829, 0x282A, 0x2C2B, 0x282C, 0x2C2D
.word  0x2C2E, 0x282F, 0x3430, 0x3031, 0x3032, 0x3433, 0x3034, 0x3435
.word  0x2436, 0x2037, 0x2838, 0x2C39, 0x2C3A, 0x283B, 0x2C3C, 0x283D
.word  0x283E, 0x2C3F, 0x1040, 0x1441, 0x1442, 0x1043, 0x1444, 0x1045
.word  0x0046, 0x0447, 0x0C48, 0x0849, 0x084A, 0x0C4B, 0x084C, 0x0C4D
.word  0x0C4E, 0x084F, 0x1450, 0x1051, 0x1052, 0x1453, 0x1054, 0x1455
.word  0x0456, 0x0057, 0x0858, 0x0C59, 0x0C5A, 0x085B, 0x0C5C, 0x085D
.word  0x085E, 0x0C5F, 0x3460, 0x3061, 0x3062, 0x3463, 0x3064, 0x3465
.word  0x2466, 0x2067, 0x2868, 0x2C69, 0x2C6A, 0x286B, 0x2C6C, 0x286D
.word  0x286E, 0x2C6F, 0x3070, 0x3471, 0x3472, 0x3073, 0x3474, 0x3075
.word  0x2076, 0x2477, 0x2C78, 0x2879, 0x287A, 0x2C7B, 0x287C, 0x2C7D
.word  0x2C7E, 0x287F, 0x9080, 0x9481, 0x9482, 0x9083, 0x9484, 0x9085
.word  0x8086, 0x8487, 0x8C88, 0x8889, 0x888A, 0x8C8B, 0x888C, 0x8C8D
.word  0x8C8E, 0x888F, 0x9490, 0x9091, 0x9092, 0x9493, 0x9094, 0x9495
.word  0x8496, 0x8097, 0x8898, 0x8C99, 0x8C9A, 0x889B, 0x8C9C, 0x889D
.word  0x889E, 0x8C9F, 0x5500, 0x1101, 0x1102, 0x1503, 0x1104, 0x1505
.word  0x0506, 0x0107, 0x0908, 0x0D09, 0x0D0A, 0x090B, 0x0D0C, 0x090D
.word  0x090E, 0x0D0F, 0x1110, 0x1511, 0x1512, 0x1113, 0x1514, 0x1115
.word  0x0116, 0x0517, 0x0D18, 0x0919, 0x091A, 0x0D1B, 0x091C, 0x0D1D
.word  0x0D1E, 0x091F, 0x3120, 0x3521, 0x3522, 0x3123, 0x3524, 0x3125
.word  0x2126, 0x2527, 0x2D28, 0x2929, 0x292A, 0x2D2B, 0x292C, 0x2D2D
.word  0x2D2E, 0x292F, 0x3530, 0x3131, 0x3132, 0x3533, 0x3134, 0x3535
.word  0x2536, 0x2137, 0x2938, 0x2D39, 0x2D3A, 0x293B, 0x2D3C, 0x293D
.word  0x293E, 0x2D3F, 0x1140, 0x1541, 0x1542, 0x1143, 0x1544, 0x1145
.word  0x0146, 0x0547, 0x0D48, 0x0949, 0x094A, 0x0D4B, 0x094C, 0x0D4D
.word  0x0D4E, 0x094F, 0x1550, 0x1151, 0x1152, 0x1553, 0x1154, 0x1555
.word  0x0556, 0x0157, 0x0958, 0x0D59, 0x0D5A, 0x095B, 0x0D5C, 0x095D
.word  0x095E, 0x0D5F, 0x3560, 0x3161, 0x3162, 0x3563, 0x3164, 0x3565
.word  0x2566, 0x2167, 0x2968, 0x2D69, 0x2D6A, 0x296B, 0x2D6C, 0x296D
.word  0x296E, 0x2D6F, 0x3170, 0x3571, 0x3572, 0x3173, 0x3574, 0x3175
.word  0x2176, 0x2577, 0x2D78, 0x2979, 0x297A, 0x2D7B, 0x297C, 0x2D7D
.word  0x2D7E, 0x297F, 0x9180, 0x9581, 0x9582, 0x9183, 0x9584, 0x9185
.word  0x8186, 0x8587, 0x8D88, 0x8989, 0x898A, 0x8D8B, 0x898C, 0x8D8D
.word  0x8D8E, 0x898F, 0x9590, 0x9191, 0x9192, 0x9593, 0x9194, 0x9595
.word  0x8596, 0x8197, 0x8998, 0x8D99, 0x8D9A, 0x899B, 0x8D9C, 0x899D
.word  0x899E, 0x8D9F, 0xB5A0, 0xB1A1, 0xB1A2, 0xB5A3, 0xB1A4, 0xB5A5
.word  0xA5A6, 0xA1A7, 0xA9A8, 0xADA9, 0xADAA, 0xA9AB, 0xADAC, 0xA9AD
.word  0xA9AE, 0xADAF, 0xB1B0, 0xB5B1, 0xB5B2, 0xB1B3, 0xB5B4, 0xB1B5
.word  0xA1B6, 0xA5B7, 0xADB8, 0xA9B9, 0xA9BA, 0xADBB, 0xA9BC, 0xADBD
.word  0xADBE, 0xA9BF, 0x95C0, 0x91C1, 0x91C2, 0x95C3, 0x91C4, 0x95C5
.word  0x85C6, 0x81C7, 0x89C8, 0x8DC9, 0x8DCA, 0x89CB, 0x8DCC, 0x89CD
.word  0x89CE, 0x8DCF, 0x91D0, 0x95D1, 0x95D2, 0x91D3, 0x95D4, 0x91D5
.word  0x81D6, 0x85D7, 0x8DD8, 0x89D9, 0x89DA, 0x8DDB, 0x89DC, 0x8DDD
.word  0x8DDE, 0x89DF, 0xB1E0, 0xB5E1, 0xB5E2, 0xB1E3, 0xB5E4, 0xB1E5
.word  0xA1E6, 0xA5E7, 0xADE8, 0xA9E9, 0xA9EA, 0xADEB, 0xA9EC, 0xADED
.word  0xADEE, 0xA9EF, 0xB5F0, 0xB1F1, 0xB1F2, 0xB5F3, 0xB1F4, 0xB5F5
.word  0xA5F6, 0xA1F7, 0xA9F8, 0xADF9, 0xADFA, 0xA9FB, 0xADFC, 0xA9FD
.word  0xA9FE, 0xADFF, 0x5500, 0x1101, 0x1102, 0x1503, 0x1104, 0x1505
.word  0x0506, 0x0107, 0x0908, 0x0D09, 0x0D0A, 0x090B, 0x0D0C, 0x090D
.word  0x090E, 0x0D0F, 0x1110, 0x1511, 0x1512, 0x1113, 0x1514, 0x1115
.word  0x0116, 0x0517, 0x0D18, 0x0919, 0x091A, 0x0D1B, 0x091C, 0x0D1D
.word  0x0D1E, 0x091F, 0x3120, 0x3521, 0x3522, 0x3123, 0x3524, 0x3125
.word  0x2126, 0x2527, 0x2D28, 0x2929, 0x292A, 0x2D2B, 0x292C, 0x2D2D
.word  0x2D2E, 0x292F, 0x3530, 0x3131, 0x3132, 0x3533, 0x3134, 0x3535
.word  0x2536, 0x2137, 0x2938, 0x2D39, 0x2D3A, 0x293B, 0x2D3C, 0x293D
.word  0x293E, 0x2D3F, 0x1140, 0x1541, 0x1542, 0x1143, 0x1544, 0x1145
.word  0x0146, 0x0547, 0x0D48, 0x0949, 0x094A, 0x0D4B, 0x094C, 0x0D4D
.word  0x0D4E, 0x094F, 0x1550, 0x1151, 0x1152, 0x1553, 0x1154, 0x1555
.word  0x0556, 0x0157, 0x0958, 0x0D59, 0x0D5A, 0x095B, 0x0D5C, 0x095D
.word  0x095E, 0x0D5F, 0x3560, 0x3161, 0x3162, 0x3563, 0x3164, 0x3565
.word  0xBEFA, 0xBAFB, 0xBEFC, 0xBAFD, 0xBAFE, 0xBEFF, 0x4600, 0x0201
.word  0x0202, 0x0603, 0x0204, 0x0605, 0x0606, 0x0207, 0x0A08, 0x0E09
.word  0x1E0A, 0x1A0B, 0x1E0C, 0x1A0D, 0x1A0E, 0x1E0F, 0x0210, 0x0611
.word  0x0612, 0x0213, 0x0614, 0x0215, 0x0216, 0x0617, 0x0E18, 0x0A19
.word  0x1A1A, 0x1E1B, 0x1A1C, 0x1E1D, 0x1E1E, 0x1A1F, 0x2220, 0x2621
.word  0x2622, 0x2223, 0x2624, 0x2225, 0x2226, 0x2627, 0x2E28, 0x2A29
.word  0x3A2A, 0x3E2B, 0x3A2C, 0x3E2D, 0x3E2E, 0x3A2F, 0x2630, 0x2231
.word  0x2232, 0x2633, 0x2234, 0x2635, 0x2636, 0x2237, 0x2A38, 0x2E39
.word  0x3E3A, 0x3A3B, 0x3E3C, 0x3A3D, 0x3A3E, 0x3E3F, 0x0240, 0x0641
.word  0x0642, 0x0243, 0x0644, 0x0245, 0x0246, 0x0647, 0x0E48, 0x0A49
.word  0x1A4A, 0x1E4B, 0x1A4C, 0x1E4D, 0x1E4E, 0x1A4F, 0x0650, 0x0251
.word  0x0252, 0x0653, 0x0254, 0x0655, 0x0656, 0x0257, 0x0A58, 0x0E59
.word  0x1E5A, 0x1A5B, 0x1E5C, 0x1A5D, 0x1A5E, 0x1E5F, 0x2660, 0x2261
.word  0x2262, 0x2663, 0x2264, 0x2665, 0x2666, 0x2267, 0x2A68, 0x2E69
.word  0x3E6A, 0x3A6B, 0x3E6C, 0x3A6D, 0x3A6E, 0x3E6F, 0x2270, 0x2671
.word  0x2672, 0x2273, 0x2674, 0x2275, 0x2276, 0x2677, 0x2E78, 0x2A79
.word  0x3A7A, 0x3E7B, 0x3A7C, 0x3E7D, 0x3E7E, 0x3A7F, 0x8280, 0x8681
.word  0x8682, 0x8283, 0x8684, 0x8285, 0x8286, 0x8687, 0x8E88, 0x8A89
.word  0x9A8A, 0x9E8B, 0x9A8C, 0x9E8D, 0x9E8E, 0x9A8F, 0x8690, 0x8291
.word  0x8292, 0x8693, 0x2334, 0x2735, 0x2736, 0x2337, 0x2B38, 0x2F39
.word  0x3F3A, 0x3B3B, 0x3F3C, 0x3B3D, 0x3B3E, 0x3F3F, 0x0340, 0x0741
.word  0x0742, 0x0343, 0x0744, 0x0345, 0x0346, 0x0747, 0x0F48, 0x0B49
.word  0x1B4A, 0x1F4B, 0x1B4C, 0x1F4D, 0x1F4E, 0x1B4F, 0x0750, 0x0351
.word  0x0352, 0x0753, 0x0354, 0x0755, 0x0756, 0x0357, 0x0B58, 0x0F59
.word  0x1F5A, 0x1B5B, 0x1F5C, 0x1B5D, 0x1B5E, 0x1F5F, 0x2760, 0x2361
.word  0x2362, 0x2763, 0x2364, 0x2765, 0x2766, 0x2367, 0x2B68, 0x2F69
.word  0x3F6A, 0x3B6B, 0x3F6C, 0x3B6D, 0x3B6E, 0x3F6F, 0x2370, 0x2771
.word  0x2772, 0x2373, 0x2774, 0x2375, 0x2376, 0x2777, 0x2F78, 0x2B79
.word  0x3B7A, 0x3F7B, 0x3B7C, 0x3F7D, 0x3F7E, 0x3B7F, 0x8380, 0x8781
.word  0x8782, 0x8383, 0x8784, 0x8385, 0x8386, 0x8787, 0x8F88, 0x8B89
.word  0x9B8A, 0x9F8B, 0x9B8C, 0x9F8D, 0x9F8E, 0x9B8F, 0x8790, 0x8391
.word  0x8392, 0x8793, 0x8394, 0x8795, 0x8796, 0x8397, 0x8B98, 0x8F99
.word  0x9F9A, 0x9B9B, 0x9F9C, 0x9B9D, 0x9B9E, 0x9F9F, 0xA7A0, 0xA3A1
.word  0xA3A2, 0xA7A3, 0xA3A4, 0xA7A5, 0xA7A6, 0xA3A7, 0xABA8, 0xAFA9
.word  0xBFAA, 0xBBAB, 0xBFAC, 0xBBAD, 0xBBAE, 0xBFAF, 0xA3B0, 0xA7B1
.word  0xA7B2, 0xA3B3, 0xA7B4, 0xA3B5, 0xA3B6, 0xA7B7, 0xAFB8, 0xABB9
.word  0xBBBA, 0xBFBB, 0xBBBC, 0xBFBD, 0xBFBE, 0xBBBF, 0x87C0, 0x83C1
.word  0x83C2, 0x87C3, 0x83C4, 0x87C5, 0x87C6, 0x83C7, 0x8BC8, 0x8FC9
.word  0x9FCA, 0x9BCB, 0x9FCC, 0x9BCD, 0x9BCE, 0x9FCF, 0x83D0, 0x87D1
.word  0x87D2, 0x83D3, 0x87D4, 0x83D5, 0x83D6, 0x87D7, 0x8FD8, 0x8BD9
.word  0x9BDA, 0x9FDB, 0x9BDC, 0x9FDD, 0x9FDE, 0x9BDF, 0xA3E0, 0xA7E1
.word  0xA7E2, 0xA3E3, 0xA7E4, 0xA3E5, 0xA3E6, 0xA7E7, 0xAFE8, 0xABE9
.word  0xBBEA, 0xBFEB, 0xBBEC, 0xBFED, 0xBFEE, 0xBBEF, 0xA7F0, 0xA3F1
.word  0xA3F2, 0xA7F3, 0xA3F4, 0xA7F5, 0xA7F6, 0xA3F7, 0xABF8, 0xAFF9
.word  0xBFFA, 0xBBFB, 0xBFFC, 0xBBFD, 0xBBFE, 0xBFFF, 0x4700, 0x0301
.word  0x0302, 0x0703, 0x0304, 0x0705, 0x0706, 0x0307, 0x0B08, 0x0F09
.word  0x1F0A, 0x1B0B, 0x1F0C, 0x1B0D, 0x1B0E, 0x1F0F, 0x0310, 0x0711
.word  0x0712, 0x0313, 0x0714, 0x0315, 0x0316, 0x0717, 0x0F18, 0x0B19
.word  0x1B1A, 0x1F1B, 0x1B1C, 0x1F1D, 0x1F1E, 0x1B1F, 0x2320, 0x2721
.word  0x2722, 0x2323, 0x2724, 0x2325, 0x2326, 0x2727, 0x2F28, 0x2B29
.word  0x3B2A, 0x3F2B, 0x3B2C, 0x3F2D, 0x3F2E, 0x3B2F, 0x2730, 0x2331
.word  0x2332, 0x2733, 0x2334, 0x2735, 0x2736, 0x2337, 0x2B38, 0x2F39
.word  0x3F3A, 0x3B3B, 0x3F3C, 0x3B3D, 0x3B3E, 0x3F3F, 0x0340, 0x0741
.word  0x0742, 0x0343, 0x0744, 0x0345, 0x0346, 0x0747, 0x0F48, 0x0B49
.word  0x1B4A, 0x1F4B, 0x1B4C, 0x1F4D, 0x1F4E, 0x1B4F, 0x0750, 0x0351
.word  0x0352, 0x0753, 0x0354, 0x0755, 0x0756, 0x0357, 0x0B58, 0x0F59
.word  0x1F5A, 0x1B5B, 0x1F5C, 0x1B5D, 0x1B5E, 0x1F5F, 0x2760, 0x2361
.word  0x2362, 0x2763, 0x2364, 0x2765, 0x2766, 0x2367, 0x2B68, 0x2F69
.word  0x3F6A, 0x3B6B, 0x3F6C, 0x3B6D, 0x3B6E, 0x3F6F, 0x2370, 0x2771
.word  0x2772, 0x2373, 0x2774, 0x2375, 0x2376, 0x2777, 0x2F78, 0x2B79
.word  0x3B7A, 0x3F7B, 0x3B7C, 0x3F7D, 0x3F7E, 0x3B7F, 0x8380, 0x8781
.word  0x8782, 0x8383, 0x8784, 0x8385, 0x8386, 0x8787, 0x8F88, 0x8B89
.word  0x9B8A, 0x9F8B, 0x9B8C, 0x9F8D, 0x9F8E, 0x9B8F, 0x8790, 0x8391
.word  0x8392, 0x8793, 0x8394, 0x8795, 0x8796, 0x8397, 0x8B98, 0x8F99

ALIGN
NEG_Table:
.byte 0x42,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xB3,0xB3,0xB3,0xB3,0xB3,0xB3,0xB3
.byte 0xA3,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xB3,0xB3,0xB3,0xB3,0xB3,0xB3,0xB3
.byte 0xA3,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x93,0x93,0x93,0x93,0x93,0x93,0x93
.byte 0x83,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x93,0x93,0x93,0x93,0x93,0x93,0x93
.byte 0x83,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xB3,0xB3,0xB3,0xB3,0xB3,0xB3,0xB3
.byte 0xA3,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xBB,0xB3,0xB3,0xB3,0xB3,0xB3,0xB3,0xB3
.byte 0xA3,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x93,0x93,0x93,0x93,0x93,0x93,0x93
.byte 0x83,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x9B,0x93,0x93,0x93,0x93,0x93,0x93,0x93
.byte 0x87,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x33,0x33,0x33,0x33,0x33,0x33,0x33
.byte 0x23,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x33,0x33,0x33,0x33,0x33,0x33,0x33
.byte 0x23,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x13,0x13,0x13,0x13,0x13,0x13,0x13
.byte 0x03,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x13,0x13,0x13,0x13,0x13,0x13,0x13
.byte 0x03,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x33,0x33,0x33,0x33,0x33,0x33,0x33
.byte 0x23,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x3B,0x33,0x33,0x33,0x33,0x33,0x33,0x33
.byte 0x23,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x13,0x13,0x13,0x13,0x13,0x13,0x13
.byte 0x03,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x1B,0x13,0x13,0x13,0x13,0x13,0x13,0x13

!- the end ------ (blimey) --------------------------------------------------!
