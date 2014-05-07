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

          ! 

! RAZE - register definitions



! get/set reg values





















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
            ! autodown IRQ feature
!define(`EMULATE1')                 ! helpful to debug
!define(`IRQ_CYCLES')		        ! spend cycles for IRQs
!define(`NO_PC_LIMIT')
!define(`BASED_PC')
          ! generate SH2 compatible code
          ! vbt changes
!define(`NO_READ_HANDLER')

!----------------------------------------------------------------------------!

! Notas de desarrollo
! Algunas instrucciones fijan las banderas sin usar los defines
! Las banderas A y F se deberian salvar/restaurar en los CALLGCC_START/END




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

	.rept 256
	.long 0
	.endr


.global _z80_Read
_z80_Read:

	
		.rept 256*2
		.long 0
		.endr
	



.global _z80_Write
_z80_Write:

	
		.rept 256*2
		.long 0
		.endr
	


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


	
		.align 2
		_z80_Mem_Handlers: .long _z80_memread, _z80_memwrite
	


.text



!----------------------------------------------------------------------------!
! Bring in the opcodes...

! RAZE - complete Z80 opcode list
!
! Conventions:
! R   = A,B,C,D,E,F,H,L
! RR  = AF,BC,DE,HL,SP
! RR2 = AF2,BC2,DE2,HL2
! XY  = IX,IY
! XYr = A,B,C,D,E,F,IXl,IXh,IYl,IYh
! mRR = (RR)
! mXY = (XY+dd)
! mNN = (nnnn)
! N   = nn     (unsigned)
! NN  = nnnn   (unsigned)
! +dd = nn     (signed)
! b   = bit (0-7)
! cc  = c,nc,z,nc,pe,po,m,p
! Rf  = refresh
! I   = interrupt register
!
! FDCB instructions not included - use DDCB with IY instead of IX

! **************************************************************************
! LD r,r


	
	
	
op_40:

	
	! Si el registro origen y destino es el mismo, nop
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_41:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_BC - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_42:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC,r5),r0
			mov.b r0,@(_z80_BC - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_43:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_BC - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_44:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_HL - _z80_BC,r5),r0
			mov.b r0,@(_z80_BC - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_45:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_HL - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_BC - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_47:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov r3,r0
			mov.b r0,@(_z80_BC - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_48:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC,r5),r0
			mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_49:

	
	! Si el registro origen y destino es el mismo, nop
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_4a:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC,r5),r0
			mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_4b:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_4c:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_HL - _z80_BC,r5),r0
			mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_4d:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_HL - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_4f:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov r3,r0
			mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_50:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC,r5),r0
			mov.b r0,@(_z80_DE - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_51:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_DE - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_52:

	
	! Si el registro origen y destino es el mismo, nop
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_53:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_DE - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_54:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_HL - _z80_BC,r5),r0
			mov.b r0,@(_z80_DE - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_55:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_HL - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_DE - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_57:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov r3,r0
			mov.b r0,@(_z80_DE - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_58:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC,r5),r0
			mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_59:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_5a:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC,r5),r0
			mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_5b:

	
	! Si el registro origen y destino es el mismo, nop
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_5c:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_HL - _z80_BC,r5),r0
			mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_5d:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_HL - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_5f:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov r3,r0
			mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_60:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC,r5),r0
			mov.b r0,@(_z80_HL - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_61:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_HL - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_62:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC,r5),r0
			mov.b r0,@(_z80_HL - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_63:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_HL - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_64:

	
	! Si el registro origen y destino es el mismo, nop
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_65:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_HL - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_HL - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_67:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov r3,r0
			mov.b r0,@(_z80_HL - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_68:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC,r5),r0
			mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_69:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_6a:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC,r5),r0
			mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_6b:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_6c:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_HL - _z80_BC,r5),r0
			mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_6d:

	
	! Si el registro origen y destino es el mismo, nop
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_6f:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov r3,r0
			mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_78:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC,r5),r0
			mov r0,r3
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_79:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC + 1,r5),r0
			mov r0,r3
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_7a:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC,r5),r0
			mov r0,r3
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_7b:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC + 1,r5),r0
			mov r0,r3
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_7c:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_HL - _z80_BC,r5),r0
			mov r0,r3
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_7d:

	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_HL - _z80_BC + 1,r5),r0
			mov r0,r3
		
	
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_7f:

	
	! Si el registro origen y destino es el mismo, nop
	
	
	
	jmp @r12
	add #-4,r7




! **************************************************************************
! LD r,r (r=IXl,IXh,IYl,IYh)


	
	
	
dd_44:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_IX - _z80_BC,r5),r0
			mov.b r0,@(_z80_BC - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
dd_45:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_IX - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_BC - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
fd_44:

	
	
	
	

	


	! Source register loading
	
		
	mov.l _z80_IY_1,r2
	mov.b @r2,r0

	

	! Destination register writing
	
		
			
	mov.b r0,@(_z80_BC - _z80_BC,r5)

		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
	
	
	
		_z80_IY_1: .long _z80_IY + 1
	




	
	
	
fd_45:

	
	
	
	

	


	! Source register loading
	
		
			
	mov.l _z80_IY_2,r2
	mov.b @r2,r0

		
	

	! Destination register writing
	
		
			
	mov.b r0,@(_z80_BC - _z80_BC,r5)

		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
	
	
	
		
			_z80_IY_2: .long _z80_IY
		
	




	
	
	
dd_4c:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_IX - _z80_BC,r5),r0
			mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
dd_4d:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_IX - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
fd_4c:

	
	
	
	

	


	! Source register loading
	
		
	mov.l _z80_IY_3,r2
	mov.b @r2,r0

	

	! Destination register writing
	
		
			
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)

		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
	
	
	
		_z80_IY_3: .long _z80_IY + 1
	




	
	
	
fd_4d:

	
	
	
	

	


	! Source register loading
	
		
			
	mov.l _z80_IY_4,r2
	mov.b @r2,r0

		
	

	! Destination register writing
	
		
			
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)

		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
	
	
	
		
			_z80_IY_4: .long _z80_IY
		
	




	
	
	
dd_54:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_IX - _z80_BC,r5),r0
			mov.b r0,@(_z80_DE - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
dd_55:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_IX - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_DE - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
fd_54:

	
	
	
	

	


	! Source register loading
	
		
	mov.l _z80_IY_5,r2
	mov.b @r2,r0

	

	! Destination register writing
	
		
			
	mov.b r0,@(_z80_DE - _z80_BC,r5)

		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
	
	
	
		_z80_IY_5: .long _z80_IY + 1
	




	
	
	
fd_55:

	
	
	
	

	


	! Source register loading
	
		
			
	mov.l _z80_IY_6,r2
	mov.b @r2,r0

		
	

	! Destination register writing
	
		
			
	mov.b r0,@(_z80_DE - _z80_BC,r5)

		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
	
	
	
		
			_z80_IY_6: .long _z80_IY
		
	




	
	
	
dd_5c:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_IX - _z80_BC,r5),r0
			mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
dd_5d:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_IX - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
fd_5c:

	
	
	
	

	


	! Source register loading
	
		
	mov.l _z80_IY_7,r2
	mov.b @r2,r0

	

	! Destination register writing
	
		
			
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)

		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
	
	
	
		_z80_IY_7: .long _z80_IY + 1
	




	
	
	
fd_5d:

	
	
	
	

	


	! Source register loading
	
		
			
	mov.l _z80_IY_8,r2
	mov.b @r2,r0

		
	

	! Destination register writing
	
		
			
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)

		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
	
	
	
		
			_z80_IY_8: .long _z80_IY
		
	




	
	
	
dd_7c:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_IX - _z80_BC,r5),r0
			mov r0,r3
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
dd_7d:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_IX - _z80_BC + 1,r5),r0
			mov r0,r3
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
fd_7c:

	
	
	
	

	


	! Source register loading
	
		
	mov.l _z80_IY_9,r2
	mov.b @r2,r0

	

	! Destination register writing
	
		
			
	mov r0,r3

		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
	
	
	
		_z80_IY_9: .long _z80_IY + 1
	




	
	
	
fd_7d:

	
	
	
	

	


	! Source register loading
	
		
			
	mov.l _z80_IY_10,r2
	mov.b @r2,r0

		
	

	! Destination register writing
	
		
			
	mov r0,r3

		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
	
	
	
		
			_z80_IY_10: .long _z80_IY
		
	




	
	
	
dd_60:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC,r5),r0
			mov.b r0,@(_z80_IX - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
dd_61:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_IX - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
dd_62:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC,r5),r0
			mov.b r0,@(_z80_IX - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
dd_63:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_IX - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-8,r7




!DEF(`dd',`64',`8',`0')       ! its a NOP anyway, dont include it
!   LD_Xr_Xr(`IXh',`IXh')


	
	
	
dd_65:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_IX - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_IX - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
dd_67:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov r3,r0
			mov.b r0,@(_z80_IX - _z80_BC,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
dd_68:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC,r5),r0
			mov.b r0,@(_z80_IX - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
dd_69:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_BC - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_IX - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
dd_6a:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC,r5),r0
			mov.b r0,@(_z80_IX - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
dd_6b:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_DE - _z80_BC + 1,r5),r0
			mov.b r0,@(_z80_IX - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
dd_6c:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov.b @(_z80_IX - _z80_BC,r5),r0
			mov.b r0,@(_z80_IX - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





!DEF(`dd',`6d',`8',`0')       ! its a NOP anyway, dont include it
!   LD_Xr_Xr(`IXl',`IXl')


	
	
	
dd_6f:

	
	
	! Si el registro origen y destino es el mismo, nop
	
		
			mov r3,r0
			mov.b r0,@(_z80_IX - _z80_BC + 1,r5)
		
	
	
	
	jmp @r12
	add #-8,r7





	
	
	
fd_60:

	
	
	
	

	


	! Source register loading
	
		
			
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

		
	

	! Destination register writing
	
		mov.l _z80_IY2_11,r1
		mov.b r0,@r1
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		_z80_IY2_11: .long _z80_IY + 1
	
	
	
		
	




	
	
	
fd_61:

	
	
	
	

	


	! Source register loading
	
		
			
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

		
	

	! Destination register writing
	
		mov.l _z80_IY2_12,r1
		mov.b r0,@r1
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		_z80_IY2_12: .long _z80_IY + 1
	
	
	
		
	




	
	
	
fd_62:

	
	
	
	

	


	! Source register loading
	
		
			
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

		
	

	! Destination register writing
	
		mov.l _z80_IY2_13,r1
		mov.b r0,@r1
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		_z80_IY2_13: .long _z80_IY + 1
	
	
	
		
	




	
	
	
fd_63:

	
	
	
	

	


	! Source register loading
	
		
			
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

		
	

	! Destination register writing
	
		mov.l _z80_IY2_14,r1
		mov.b r0,@r1
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		_z80_IY2_14: .long _z80_IY + 1
	
	
	
		
	




!DEF(`fd',`64',`8',`0')       ! its a NOP anyway, dont include it
!   LD_Yr_Yr(`IYh',`IYh')


	
	
	
fd_65:

	
	
	
	

	


	! Source register loading
	
		
			
	mov.l _z80_IY_15,r2
	mov.b @r2,r0

		
	

	! Destination register writing
	
		mov.l _z80_IY2_15,r1
		mov.b r0,@r1
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		_z80_IY2_15: .long _z80_IY + 1
	
	
	
		
			_z80_IY_15: .long _z80_IY
		
	




	
	
	
fd_67:

	
	
	
	

	


	! Source register loading
	
		
			
	! Load operand to r0
	mov r3,r0

		
	

	! Destination register writing
	
		mov.l _z80_IY2_16,r1
		mov.b r0,@r1
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		_z80_IY2_16: .long _z80_IY + 1
	
	
	
		
	




	
	
	
fd_68:

	
	
	
	

	


	! Source register loading
	
		
			
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

		
	

	! Destination register writing
	
		
			mov.l _z80_IY2_17,r1
			mov.b r0,@r1
		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
			_z80_IY2_17: .long _z80_IY
		
	
	
	
		
	




	
	
	
fd_69:

	
	
	
	

	


	! Source register loading
	
		
			
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

		
	

	! Destination register writing
	
		
			mov.l _z80_IY2_18,r1
			mov.b r0,@r1
		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
			_z80_IY2_18: .long _z80_IY
		
	
	
	
		
	




	
	
	
fd_6a:

	
	
	
	

	


	! Source register loading
	
		
			
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

		
	

	! Destination register writing
	
		
			mov.l _z80_IY2_19,r1
			mov.b r0,@r1
		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
			_z80_IY2_19: .long _z80_IY
		
	
	
	
		
	




	
	
	
fd_6b:

	
	
	
	

	


	! Source register loading
	
		
			
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

		
	

	! Destination register writing
	
		
			mov.l _z80_IY2_20,r1
			mov.b r0,@r1
		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
			_z80_IY2_20: .long _z80_IY
		
	
	
	
		
	




	
	
	
fd_6c:

	
	
	
	

	


	! Source register loading
	
		
	mov.l _z80_IY_21,r2
	mov.b @r2,r0

	

	! Destination register writing
	
		
			mov.l _z80_IY2_21,r1
			mov.b r0,@r1
		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
			_z80_IY2_21: .long _z80_IY
		
	
	
	
		_z80_IY_21: .long _z80_IY + 1
	




!DEF(`fd',`6d',`8',`0')       ! its a NOP anyway, dont include it
!   LD_Yr_Yr(`IYl',`IYl')


	
	
	
fd_6f:

	
	
	
	

	


	! Source register loading
	
		
			
	! Load operand to r0
	mov r3,r0

		
	

	! Destination register writing
	
		
			mov.l _z80_IY2_22,r1
			mov.b r0,@r1
		
	

	
	jmp @r12
	add #-8,r7

	
.align 2
	
		
			_z80_IY2_22: .long _z80_IY
		
	
	
	
		
	




! **************************************************************************
! LD r,n


	
	
	
op_06:

	
	
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_0e:

	
	
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_16:

	
	
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_1e:

	
	
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_26:

	
	
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_2e:

	
	
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_3e:

	
	
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r3
		add #1,r6
	
	extu.w r6,r6

	

	
	jmp @r12
	add #-7,r7




! **************************************************************************
! LD r,n (r=IXl,IXh,IYl,IYh)


	
	
	
dd_26:

   
	
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

		mov.b r0,@(_z80_IX - _z80_BC,r5)
	

	
	jmp @r12
	add #-11,r7




	
	
	
dd_2e:

   
	
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

		mov.b r0,@(_z80_IX - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-11,r7




	
	
	
fd_26:

   
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	mov.l _z80_IY_23,r1
	mov.b r0,@r1


	
	jmp @r12
	add #-11,r7


.align 2
	
	
		_z80_IY_23: .long _z80_IY + 1
	




	
	
	
fd_2e:

   
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	mov.l _z80_IY_24,r1
	mov.b r0,@r1


	
	jmp @r12
	add #-11,r7


.align 2
	
	
		
			_z80_IY_24: .long _z80_IY
		
	




! **************************************************************************
! LD r,(HL)


	
	
	
op_46:

	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	


	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_4e:

	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	


	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_56:

	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	


	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_5e:

	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	


	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_66:

	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	


	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_6e:

	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	


	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_7e:

	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	


	
		mov r1,r3
	

	
	jmp @r12
	add #-7,r7




! **************************************************************************
! LD r,(XY+d)


	
	
	
dd_46:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IX - _z80_BC,r5),r0          ! Ojo, carga con extension
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-19,r7




	
	
	
fd_46:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IY - _z80_BC,r5),r0          ! Ojo, carga con extension
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-19,r7




	
	
	
dd_4e:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IX - _z80_BC,r5),r0          ! Ojo, carga con extension
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-19,r7




	
	
	
fd_4e:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IY - _z80_BC,r5),r0          ! Ojo, carga con extension
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-19,r7




	
	
	
dd_56:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IX - _z80_BC,r5),r0          ! Ojo, carga con extension
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-19,r7




	
	
	
fd_56:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IY - _z80_BC,r5),r0          ! Ojo, carga con extension
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-19,r7




	
	
	
dd_5e:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IX - _z80_BC,r5),r0          ! Ojo, carga con extension
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-19,r7




	
	
	
fd_5e:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IY - _z80_BC,r5),r0          ! Ojo, carga con extension
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-19,r7




	
	
	
dd_66:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IX - _z80_BC,r5),r0          ! Ojo, carga con extension
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-19,r7




	
	
	
fd_66:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IY - _z80_BC,r5),r0          ! Ojo, carga con extension
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-19,r7




	
	
	
dd_6e:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IX - _z80_BC,r5),r0          ! Ojo, carga con extension
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-19,r7




	
	
	
fd_6e:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IY - _z80_BC,r5),r0          ! Ojo, carga con extension
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-19,r7




	
	
	
dd_7e:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IX - _z80_BC,r5),r0          ! Ojo, carga con extension
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	
	
		mov r1,r3
	

	
	jmp @r12
	add #-19,r7




	
	
	
fd_7e:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IY - _z80_BC,r5),r0          ! Ojo, carga con extension
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	
	
		mov r1,r3
	

	
	jmp @r12
	add #-19,r7




! **************************************************************************
! LD (HL),r


	
	
	
op_70:

	
	
		mov.b @(_z80_BC - _z80_BC,r5),r0
		mov r0,r1
!vbtvbt                
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	

	
	jmp @r12
	add #-7,r7




	
	
	
op_71:

	
	
		mov.b @(_z80_BC - _z80_BC + 1,r5),r0
		mov r0,r1
!vbtvbt                
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	

	
	jmp @r12
	add #-7,r7




	
	
	
op_72:

	
	
		mov.b @(_z80_DE - _z80_BC,r5),r0
		mov r0,r1
!vbtvbt                
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	

	
	jmp @r12
	add #-7,r7




	
	
	
op_73:

	
	
		mov.b @(_z80_DE - _z80_BC + 1,r5),r0
		mov r0,r1
!vbtvbt                
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	

	
	jmp @r12
	add #-7,r7




	
	
	
op_74:

	
	
		mov.b @(_z80_HL - _z80_BC,r5),r0
		mov r0,r1
!vbtvbt                
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	

	
	jmp @r12
	add #-7,r7




	
	
	
op_75:

	
	
		mov.b @(_z80_HL - _z80_BC + 1,r5),r0
		mov r0,r1
!vbtvbt                
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	

	
	jmp @r12
	add #-7,r7




	
	
	
op_77:

	
	
		mov.l @(_z80_HL - _z80_BC,r5),r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov r3,r1
		jsr @r2
		shlr16 r0
	

	

	
	jmp @r12
	add #-7,r7




! **************************************************************************
! LD (XY+d),r


	
	
	
dd_70:

	
	
		mov.b @(_z80_BC - _z80_BC,r5),r0
		mov r0,r1
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

		mov.w @(_z80_IX - _z80_BC,r5),r0        ! Ojo, carga con extension
		add r2,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	

	
	jmp @r12
	add #-19,r7




	
	
	
fd_70:

	
	
		mov.b @(_z80_BC - _z80_BC,r5),r0
		mov r0,r1
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

		mov.w @(_z80_IY - _z80_BC,r5),r0        ! Ojo, carga con extension
		add r2,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	

	
	jmp @r12
	add #-19,r7




	
	
	
dd_71:

	
	
		mov.b @(_z80_BC - _z80_BC + 1,r5),r0
		mov r0,r1
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

		mov.w @(_z80_IX - _z80_BC,r5),r0        ! Ojo, carga con extension
		add r2,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	

	
	jmp @r12
	add #-19,r7




	
	
	
fd_71:

	
	
		mov.b @(_z80_BC - _z80_BC + 1,r5),r0
		mov r0,r1
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

		mov.w @(_z80_IY - _z80_BC,r5),r0        ! Ojo, carga con extension
		add r2,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	

	
	jmp @r12
	add #-19,r7




	
	
	
dd_72:

	
	
		mov.b @(_z80_DE - _z80_BC,r5),r0
		mov r0,r1
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

		mov.w @(_z80_IX - _z80_BC,r5),r0        ! Ojo, carga con extension
		add r2,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	

	
	jmp @r12
	add #-19,r7




	
	
	
fd_72:

	
	
		mov.b @(_z80_DE - _z80_BC,r5),r0
		mov r0,r1
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

		mov.w @(_z80_IY - _z80_BC,r5),r0        ! Ojo, carga con extension
		add r2,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	

	
	jmp @r12
	add #-19,r7




	
	
	
dd_73:

	
	
		mov.b @(_z80_DE - _z80_BC + 1,r5),r0
		mov r0,r1
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

		mov.w @(_z80_IX - _z80_BC,r5),r0        ! Ojo, carga con extension
		add r2,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	

	
	jmp @r12
	add #-19,r7




	
	
	
fd_73:

	
	
		mov.b @(_z80_DE - _z80_BC + 1,r5),r0
		mov r0,r1
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

		mov.w @(_z80_IY - _z80_BC,r5),r0        ! Ojo, carga con extension
		add r2,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	

	
	jmp @r12
	add #-19,r7




	
	
	
dd_74:

	
	
		mov.b @(_z80_HL - _z80_BC,r5),r0
		mov r0,r1
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

		mov.w @(_z80_IX - _z80_BC,r5),r0        ! Ojo, carga con extension
		add r2,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	

	
	jmp @r12
	add #-19,r7




	
	
	
fd_74:

	
	
		mov.b @(_z80_HL - _z80_BC,r5),r0
		mov r0,r1
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

		mov.w @(_z80_IY - _z80_BC,r5),r0        ! Ojo, carga con extension
		add r2,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	

	
	jmp @r12
	add #-19,r7




	
	
	
dd_75:

	
	
		mov.b @(_z80_HL - _z80_BC + 1,r5),r0
		mov r0,r1
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

		mov.w @(_z80_IX - _z80_BC,r5),r0        ! Ojo, carga con extension
		add r2,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	

	
	jmp @r12
	add #-19,r7




	
	
	
fd_75:

	
	
		mov.b @(_z80_HL - _z80_BC + 1,r5),r0
		mov r0,r1
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

		mov.w @(_z80_IY - _z80_BC,r5),r0        ! Ojo, carga con extension
		add r2,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	

	
	jmp @r12
	add #-19,r7




	
	
	
dd_77:

	
	
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

		mov.w @(_z80_IX - _z80_BC,r5),r0         ! Ojo, carga con extension
		add r1,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov r3,r1
	

	

	
	jmp @r12
	add #-19,r7




	
	
	
fd_77:

	
	
		
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

		mov.w @(_z80_IY - _z80_BC,r5),r0         ! Ojo, carga con extension
		add r1,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov r3,r1
	

	

	
	jmp @r12
	add #-19,r7




! **************************************************************************
! LD (HL),n / LD (XY+d),n


	
	
	
op_36:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6


	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	
	jmp @r12
	add #-10,r7




	
	
	
dd_36:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6
     ! DIRT_REG = offset
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6
      ! TMP_REG = inmediate data
	mov.w @(_z80_IX - _z80_BC,r5),r0            ! Ojo, carga con extension
	add r2,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-19,r7




	
	
	
fd_36:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6
     ! DIRT_REG = offset
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6
      ! TMP_REG = inmediate data
	mov.w @(_z80_IY - _z80_BC,r5),r0            ! Ojo, carga con extension
	add r2,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-19,r7




! **************************************************************************
! LD A,(BC) / LD A,(DE) / LD A,(nn)


	
	
	
op_0a:

	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_BC - _z80_BC,r5),r0
	


	
		mov r1,r3
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_1a:

	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_DE - _z80_BC,r5),r0
	


	
		mov r1,r3
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_3a:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
		mov r1,r3

	

	
	jmp @r12
	add #-13,r7




! **************************************************************************
! LD (BC),A / LD (DE),A / LD (nn),A


	
	
	
op_02:

	
	
		mov.l @(_z80_BC - _z80_BC,r5),r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov r3,r1
		jsr @r2
		shlr16 r0
	

	

	
	jmp @r12
	add #-7,r7




	
	
	
op_12:

	
	
		mov.l @(_z80_DE - _z80_BC,r5),r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov r3,r1
		jsr @r2
		shlr16 r0
	

	

	
	jmp @r12
	add #-7,r7




	
	
	
op_32:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov r3,r1
	

	
	
	jmp @r12
	add #-13,r7




! **************************************************************************
! LD [ARI],[ARI]


	
	
	
ed_57:

	
	
	
	

	


	mov #_z80_I-_z80_BC,r0
	mov #0x08,r8         ! prepare to set V and keep the old carry
	mov.b @(r0,r5),r0  ! r0 = zI
	mov r0,r3
	
	! Set Z
	mov r0,r13

	mov #_z80_IFF2-_z80_BC,r0
	mov.b @(r0,r5),r0  ! r0 = zIFF2
	or r0,r8                ! put P/V flag

	
	jmp @r12
	add #-9,r7




	
	
	
ed_5f:

	
	
	
	

	



	! attempt to fake the R-register
	! take the ICount, invert it, divide by 4
	! this produces an increasing counter, roughly proportional to
	! the number of instructions done
	mov.l _z80_R2_26,r0
	mov r7,r1
	mov.b @r0,r0             ! r0 = R2
	mov #0x7F,r2       ! R mask
	not r1,r1
	and #0x80,r0             ! take bits 0-6 from R, bit 7 from R2
	shlr2 r1


	and r14,r2          ! DIRT_REG = R masked
	or r2,r0           ! r0 = result
	exts.b r0,r0

	! Cargar resultado
	mov r0,r3

	
	

	
	! Set Z
	mov r0,r13


	mov.l _z80_IFF2_26,r0
	mov #0x08,r8
	mov.b @r0,r0            ! r0 = zIFF2
	or r0,r8                ! put P/V flag

	
	jmp @r12
	add #-9,r7


.align 2
	_z80_IFF2_26: .long _z80_IFF2
	_z80_R2_26: .long _z80_R2



	
	
	
ed_47:

	
	
		mov #_z80_I-_z80_BC,r0
		mov.b r3,@(r0,r5)
	

	
	jmp @r12
	add #-9,r7




	
	
	
ed_4f:

	
	
	
	

	

	

	
	jmp @r12
	add #-9,r7


	



! **************************************************************************
! LD dd,nn


	
	
	
op_01:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	mov.w r0,@(_z80_BC - _z80_BC,r5)
	
	jmp @r12
	add #-10,r7




	
	
	
op_11:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	mov.w r0,@(_z80_DE - _z80_BC,r5)
	
	jmp @r12
	add #-10,r7




	
	
	
op_21:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	mov.w r0,@(_z80_HL - _z80_BC,r5)
	
	jmp @r12
	add #-10,r7




	
	
	
op_31:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	mov.w r0,@(_z80_SP - _z80_BC,r5)
	
	jmp @r12
	add #-10,r7




	
	
	
dd_21:

   
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	mov.w r0,@(_z80_IX - _z80_BC,r5)
	
	jmp @r12
	add #-14,r7




	
	
	
fd_21:

   
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	mov.w r0,@(_z80_IY - _z80_BC,r5)
	
	jmp @r12
	add #-14,r7




! **************************************************************************
! LD dd,(nn)


	
	
	
op_2a:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov.l r1,@-r15
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

	mov.l @r15+,r0
	shll8 r1
	extu.b r0,r0
	or r1,r0
   
	mov.w r0,@(_z80_HL - _z80_BC,r5)
	
	jmp @r12
	add #-16,r7




	
	
	
ed_4b:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov.l r1,@-r15
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

	mov.l @r15+,r0
	shll8 r1
	extu.b r0,r0
	or r1,r0
   
	mov.w r0,@(_z80_BC - _z80_BC,r5)
	
	jmp @r12
	add #-16,r7




	
	
	
ed_5b:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov.l r1,@-r15
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

	mov.l @r15+,r0
	shll8 r1
	extu.b r0,r0
	or r1,r0
   
	mov.w r0,@(_z80_DE - _z80_BC,r5)
	
	jmp @r12
	add #-16,r7




	
	
	
ed_6b:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov.l r1,@-r15
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

	mov.l @r15+,r0
	shll8 r1
	extu.b r0,r0
	or r1,r0
   
	mov.w r0,@(_z80_HL - _z80_BC,r5)
	
	jmp @r12
	add #-16,r7




	
	
	
ed_7b:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov.l r1,@-r15
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

	mov.l @r15+,r0
	shll8 r1
	extu.b r0,r0
	or r1,r0
   
	mov.w r0,@(_z80_SP - _z80_BC,r5)
	
	jmp @r12
	add #-16,r7




	
	
	
dd_2a:

   
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov.l r1,@-r15
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

	mov.l @r15+,r0
	shll8 r1
	extu.b r0,r0
	or r1,r0
   
	mov.w r0,@(_z80_IX - _z80_BC,r5)
	
	jmp @r12
	add #-20,r7




	
	
	
fd_2a:

   
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov.l r1,@-r15
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

	mov.l @r15+,r0
	shll8 r1
	extu.b r0,r0
	or r1,r0
   
	mov.w r0,@(_z80_IY - _z80_BC,r5)
	
	jmp @r12
	add #-20,r7




! **************************************************************************
! LD (nnnn),dd


	
	
	
op_22:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0
    ! r0 = address

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r1    ! data
		jsr @r2
		shlr16 r1
	

	mov.l @(_z80_HL - _z80_BC,r5),r1
	shlr16 r1
	add #1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	


	
	jmp @r12
	add #-16,r7




	
	
	
ed_43:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0
    ! r0 = address

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_BC - _z80_BC,r5),r1    ! data
		jsr @r2
		shlr16 r1
	

	mov.l @(_z80_BC - _z80_BC,r5),r1
	shlr16 r1
	add #1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	


	
	jmp @r12
	add #-16,r7




	
	
	
ed_53:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0
    ! r0 = address

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_DE - _z80_BC,r5),r1    ! data
		jsr @r2
		shlr16 r1
	

	mov.l @(_z80_DE - _z80_BC,r5),r1
	shlr16 r1
	add #1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	


	
	jmp @r12
	add #-16,r7




	
	
	
ed_63:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0
    ! r0 = address

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r1    ! data
		jsr @r2
		shlr16 r1
	

	mov.l @(_z80_HL - _z80_BC,r5),r1
	shlr16 r1
	add #1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	


	
	jmp @r12
	add #-16,r7




	
	
	
ed_73:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0
    ! r0 = address

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_SP - _z80_BC,r5),r1    ! data
		jsr @r2
		shlr16 r1
	

	mov.l @(_z80_SP - _z80_BC,r5),r1
	shlr16 r1
	add #1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	


	
	jmp @r12
	add #-16,r7




	
	
	
dd_22:

   
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0
    ! r0 = address

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_IX - _z80_BC,r5),r1    ! data
		jsr @r2
		shlr16 r1
	

	mov.l @(_z80_IX - _z80_BC,r5),r1
	shlr16 r1
	add #1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	


	
	jmp @r12
	add #-20,r7




	
	
	
fd_22:

   
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1
        ! TMP_REG = address
	mov.w @(_z80_IY - _z80_BC,r5),r0              ! data
	mov r1,r2
	mov.l r0,@-r15            ! save data for 2nd write
	mov r0,r1
	mov r2,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	mov.l @r15+,r1
	shlr8 r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		add #1,r0
	


	
	jmp @r12
	add #-20,r7




! **************************************************************************
! LD SP,dd


	
	
	
op_f9:

	
	mov.w @(_z80_HL - _z80_BC,r5),r0
	mov.w r0,@(_z80_SP - _z80_BC,r5)

	
	jmp @r12
	add #-6,r7




	
	
	
dd_f9:

	
	
	mov.w @(_z80_IX - _z80_BC,r5),r0
	mov.w r0,@(_z80_SP - _z80_BC,r5)

	
	jmp @r12
	add #-10,r7





	
	
	
fd_f9:

	
	
	mov.w @(_z80_IY - _z80_BC,r5),r0
	mov.w r0,@(_z80_SP - _z80_BC,r5)

	
	jmp @r12
	add #-10,r7





! **************************************************************************
! PUSH rr


	
	
	
op_c5:

	
	
	
	

	


	
!	not af
		mov.w @(_z80_SP - _z80_BC,r5),r0    ! address
		mov.l @(_z80_BC - _z80_BC,r5),r1
!vbtvbt
		shlr16 r1
		add #-1,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	
    ! First write
		mov.l @(_z80_BC - _z80_BC,r5),r1
!vbtvbt
		shlr16 r1
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		add #-1,r0
	
     ! Second write
	

	! Writeback SR
	mov.w r0,@(_z80_SP - _z80_BC,r5)

	
	jmp @r12
	add #-11,r7


.align 2
	Byte_Flags_28: .long Byte_Flags



	
	
	
op_d5:

	
	
	
	

	


	
!	not af
		mov.w @(_z80_SP - _z80_BC,r5),r0    ! address
		mov.l @(_z80_DE - _z80_BC,r5),r1
!vbtvbt
		shlr16 r1
		add #-1,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	
    ! First write
		mov.l @(_z80_DE - _z80_BC,r5),r1
!vbtvbt
		shlr16 r1
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		add #-1,r0
	
     ! Second write
	

	! Writeback SR
	mov.w r0,@(_z80_SP - _z80_BC,r5)

	
	jmp @r12
	add #-11,r7


.align 2
	Byte_Flags_29: .long Byte_Flags



	
	
	
op_e5:

	
	
	
	

	


	
!	not af
		mov.w @(_z80_SP - _z80_BC,r5),r0    ! address
		mov.l @(_z80_HL - _z80_BC,r5),r1
!vbtvbt
		shlr16 r1
		add #-1,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	
    ! First write
		mov.l @(_z80_HL - _z80_BC,r5),r1
!vbtvbt
		shlr16 r1
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		add #-1,r0
	
     ! Second write
	

	! Writeback SR
	mov.w r0,@(_z80_SP - _z80_BC,r5)

	
	jmp @r12
	add #-11,r7


.align 2
	Byte_Flags_30: .long Byte_Flags



	
	
	
op_f5:

	
	
	
	

	


	
!	it af
		mov.w @(_z80_SP - _z80_BC,r5),r0
		mov r3,r1

		!totovbt
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		add #-1,r0
	
       ! First write

		mov.l r8,@-r15
		
	
	
	

	


	exts.b r13,r13
	mov.l Byte_Flags_32,r0
	mov.b @(r0,r13),r1    ! TMP_REG = SZ000P00

	mov r8,r0
	tst #0x08,r0     ! test P/V indicator
	bt/s .no_veval32
	or r11,r1                      ! TMP_REG = SZ000P0C

	mov #~0x04,r2
	shlr r0                                            ! V -> T
	and r2,r1                  ! TMP_REG = SZ00000C
	movt r2               ! DIRT_REG = V
	shll2 r2               ! V flag in place
	or r2,r1    ! TMP_REG = SZ000V0C

.no_veval32:
	shlr r8                            ! ignore V
	shlr r8                            ! T = H
	movt r2               ! DIRT_REG = H
	shll2 r2
	shll2 r2                ! H flag in place
	or r2,r1     ! TMP_REG = SZ0H0P0C

	shlr r8                             ! T = N
	movt r2                ! DIRT_REG = N
	shll r2
	or r2,r1
	extu.b r1,r1

		mov.w @(_z80_SP - _z80_BC,r5),r0
		mov.l @r15+,r8
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		add #-2,r0
	
       ! Second write
	

	! Writeback SR
	mov.w r0,@(_z80_SP - _z80_BC,r5)

	
	jmp @r12
	add #-11,r7


.align 2
	Byte_Flags_32: .long Byte_Flags



	
	
	
dd_e5:

   
	
	
	

	


	
!	not af
		mov.w @(_z80_SP - _z80_BC,r5),r0    ! address
		mov.l @(_z80_IX - _z80_BC,r5),r1
!vbtvbt
		shlr16 r1
		add #-1,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	
    ! First write
		mov.l @(_z80_IX - _z80_BC,r5),r1
!vbtvbt
		shlr16 r1
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		add #-1,r0
	
     ! Second write
	

	! Writeback SR
	mov.w r0,@(_z80_SP - _z80_BC,r5)

	
	jmp @r12
	add #-15,r7


.align 2
	Byte_Flags_33: .long Byte_Flags



	
	
	
fd_e5:

   
	
	
	

	


	
!	not af
		mov.w @(_z80_SP - _z80_BC,r5),r0    ! address
		mov.l @(_z80_IY - _z80_BC,r5),r1
!vbtvbt
		shlr16 r1
		add #-1,r0
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	
    ! First write
		mov.l @(_z80_IY - _z80_BC,r5),r1
!vbtvbt
		shlr16 r1
		
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		add #-1,r0
	
     ! Second write
	

	! Writeback SR
	mov.w r0,@(_z80_SP - _z80_BC,r5)

	
	jmp @r12
	add #-15,r7


.align 2
	Byte_Flags_34: .long Byte_Flags



! **************************************************************************
! POP rr


	
	
	
op_c1:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_SP - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
		mov.l r1,@-r15
		
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

		mov.l @r15+,r2
		extu.b r1,r1
		extu.b r2,r2
		add #1,r0
		shll8 r1
		! Writeback SR
		mov.w r0,@(_z80_SP - _z80_BC,r5)
		or r2,r1
		mov r1,r0
		mov.w r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-14,r7




	
	
	
op_d1:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_SP - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
		mov.l r1,@-r15
		
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

		mov.l @r15+,r2
		extu.b r1,r1
		extu.b r2,r2
		add #1,r0
		shll8 r1
		! Writeback SR
		mov.w r0,@(_z80_SP - _z80_BC,r5)
		or r2,r1
		mov r1,r0
		mov.w r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-14,r7




	
	
	
op_e1:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_SP - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
		mov.l r1,@-r15
		
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

		mov.l @r15+,r2
		extu.b r1,r1
		extu.b r2,r2
		add #1,r0
		shll8 r1
		! Writeback SR
		mov.w r0,@(_z80_SP - _z80_BC,r5)
		or r2,r1
		mov r1,r0
		mov.w r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-14,r7




	
	
	
op_f1:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_SP - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
		
	

		mov r1,r8
		
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV


		mov.l @(_z80_SP - _z80_BC,r5),r0
!vbtvbt
		shlr16 r0
		
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

		mov r1, r3
		add #1,r0
		! Writeback SR
		mov.w r0,@(_z80_SP - _z80_BC,r5)
	

	
	jmp @r12
	add #-14,r7




	
	
	
dd_e1:

   
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_SP - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
		mov.l r1,@-r15
		
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

		mov.l @r15+,r2
		extu.b r1,r1
		extu.b r2,r2
		add #1,r0
		shll8 r1
		! Writeback SR
		mov.w r0,@(_z80_SP - _z80_BC,r5)
		or r2,r1
		mov r1,r0
		mov.w r0,@(_z80_IX - _z80_BC,r5)
	

	
	jmp @r12
	add #-14,r7




	
	
	
fd_e1:

   
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_SP - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	mov.l r1,@-r15
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

	add #1,r0
	extu.b r1,r1
	! Writeback SR
	mov.w r0,@(_z80_SP - _z80_BC,r5)
	mov.l @r15+,r0
	extu.b r0,r0
	shll8 r1
	or r1,r0
	mov.w r0,@(_z80_IY - _z80_BC,r5)

	
	jmp @r12
	add #-14,r7




! **************************************************************************
! EX group


	
	
	
op_eb:

	
	mov.l @(_z80_DE - _z80_BC,r5),r0
	mov.l @(_z80_HL - _z80_BC,r5),r1

	mov.l r0,@(_z80_HL - _z80_BC,r5)
	mov.l r1,@(_z80_DE - _z80_BC,r5)
	shlr16 r0
	shlr16 r1

	
	jmp @r12
	add #-4,r7




	
	
	
op_08:

   
	
	
	
	

	


	exts.b r13,r13
	mov.l Byte_Flags_35,r0
	mov.b @(r0,r13),r1    ! TMP_REG = SZ000P00

	mov r8,r0
	tst #0x08,r0     ! test P/V indicator
	bt/s .no_veval35
	or r11,r1                      ! TMP_REG = SZ000P0C

	mov #~0x04,r2
	shlr r0                                            ! V -> T
	and r2,r1                  ! TMP_REG = SZ00000C
	movt r2               ! DIRT_REG = V
	shll2 r2               ! V flag in place
	or r2,r1    ! TMP_REG = SZ000V0C

.no_veval35:
	shlr r8                            ! ignore V
	shlr r8                            ! T = H
	movt r2               ! DIRT_REG = H
	shll2 r2
	shll2 r2                ! H flag in place
	or r2,r1     ! TMP_REG = SZ0H0P0C

	shlr r8                             ! T = N
	movt r2                ! DIRT_REG = N
	shll r2
	or r2,r1
	extu.b r1,r0


	mov r3,r1
	shll8 r1
	extu.w r1,r1
	or r0,r1     ! TMP_REG = AF

	mov.l @(_z80_AF2 - _z80_BC,r5),r8
	mov r8,r3
	
	

	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV
         ! F = F2
	shlr8 r3          ! A = A2
	mov.l r1,@(_z80_AF2 - _z80_BC,r5)    ! AF2 = AF

!	extu.b DIRT_REG,zF    ! F = F2
!	shlr8 DIRT_REG
!	DO_BITS_3_5(`zF')
!	mov DIRT_REG,zA       ! A = A2
!	mov.l TMP_REG,zAF2    ! AF2 = AF

    
	jmp @r12
	add #-4,r7


.align 2
	Byte_Flags_35: .long Byte_Flags



	
	
	
op_d9:

	
	! BC
	mov.l @(_z80_BC - _z80_BC,r5),r0
	mov.l @(_z80_BC2 - _z80_BC,r5),r1
	mov.l r1,@(_z80_BC - _z80_BC,r5)
	mov.l r0,@(_z80_BC2 - _z80_BC,r5)
!	shlr16 r0
!	shlr16 TMP_REG
	
	! DE
	mov.l @(_z80_DE - _z80_BC,r5),r0
	mov.l @(_z80_DE2 - _z80_BC,r5),r1
	mov.l r1,@(_z80_DE - _z80_BC,r5)
	mov.l r0,@(_z80_DE2 - _z80_BC,r5)
!	shlr16 r0
!	shlr16 TMP_REG
	
	! HL
	mov.l @(_z80_HL - _z80_BC,r5),r0
	mov.l @(_z80_HL2 - _z80_BC,r5),r1
	mov.l r1,@(_z80_HL - _z80_BC,r5)
	mov.l r0,@(_z80_HL2 - _z80_BC,r5)
!	shlr16 r0
!	shlr16 TMP_REG
	
	
	jmp @r12
	add #-4,r7




	
	
	
op_e3:

	
!vbtvbt
! à remettre ?
	
	
		mov.l @r10,r2
                mov.l @(_z80_SP - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	
      ! Load memory reg
!	MEMREAD(`mov.l zSP,r0')      ! Load memory reg

	mov.l r1,@-r15           ! save low byte read
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r1
		jsr @r2
		shlr16 r1
	

	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

	mov.l r1,@-r15           ! save high byte read
	mov.l @(_z80_HL - _z80_BC,r5),r1
	shlr16 r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	
   
	mov.l @r15+,r0                ! restore high byte read
	shll8 r0
	mov.l @r15+,r1
	extu.b r1,r1
	or r1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)

	
	jmp @r12
	add #-19,r7




	
	
	
dd_e3:

   
!vbtvbt
! à remettre ?
	
	
		mov.l @r10,r2
                mov.l @(_z80_SP - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	
      ! Load memory reg
!	MEMREAD(`mov.l zSP,r0')      ! Load memory reg

	mov.l r1,@-r15           ! save low byte read
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_IX - _z80_BC,r5),r1
		jsr @r2
		shlr16 r1
	

	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

	mov.l r1,@-r15           ! save high byte read
	mov.l @(_z80_IX - _z80_BC,r5),r1
	shlr16 r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	
   
	mov.l @r15+,r0                ! restore high byte read
	shll8 r0
	mov.l @r15+,r1
	extu.b r1,r1
	or r1,r0
	mov.w r0,@(_z80_IX - _z80_BC,r5)

	
	jmp @r12
	add #-23,r7




	
	
	
fd_e3:

   
!vbtvbt
!        MEMREAD(`shlr16 r0',`mov.w zSP,r0')       ! Load memory reg

        
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
       ! Load memory reg
	mov.l r1,@-r15           ! save low byte read
	mov.w @(_z80_IY - _z80_BC,r5),r0
	mov r0,r1
!vbtvbt        
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	

	mov.w @(_z80_SP - _z80_BC,r5),r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	

	mov.l r1,@-r15           ! save high byte read
	mov.w @(_z80_IY - _z80_BC,r5),r0
	mov r0,r1
	mov.w @(_z80_SP - _z80_BC,r5),r0
	add #1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	mov.l @r15+,r0                ! restore high byte read
	shll8 r0
	mov.l @r15+,r1
	extu.b r1,r1
	or r1,r0
	mov.w r0,@(_z80_IY - _z80_BC,r5)

	
	jmp @r12
	add #-23,r7




! **************************************************************************
! Block Transfer (LD[ID][R ])


	
	
	
ed_a0:

	
	
	! Read from (HL)
!vbtvbt        
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	

	! Post-xcrement HL register
	add #1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)

	! Write to (DE)
!vbtvbt        

	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.w @(_z80_DE - _z80_BC,r5),r0
	
        
	! Post-xcrement DE register
	add #1,r0
	mov.w r0,@(_z80_DE - _z80_BC,r5)

	! Flag calculation
	
	
	
	

	


	! Flag calculation
	

	mov #0x08,r8	
	mov.w @(_z80_BC - _z80_BC,r5),r0
	dt r0
	bt/s ldid_zero36
	mov.w r0,@(_z80_BC - _z80_BC,r5)

	! Set P/V flag
	mov #(0x08 | 0x01),r8	

ldid_zero36:


	
	jmp @r12
	add #-16,r7





	
	
	
ed_a8:

	
	
	! Read from (HL)
!vbtvbt        
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	

	! Post-xcrement HL register
	add #-1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)

	! Write to (DE)
!vbtvbt        

	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.w @(_z80_DE - _z80_BC,r5),r0
	
        
	! Post-xcrement DE register
	add #-1,r0
	mov.w r0,@(_z80_DE - _z80_BC,r5)

	! Flag calculation
	
	
	
	

	


	! Flag calculation
	

	mov #0x08,r8	
	mov.w @(_z80_BC - _z80_BC,r5),r0
	dt r0
	bt/s ldid_zero37
	mov.w r0,@(_z80_BC - _z80_BC,r5)

	! Set P/V flag
	mov #(0x08 | 0x01),r8	

ldid_zero37:


	
	jmp @r12
	add #-16,r7





	
	
	
ed_b0:

	
	
	! Flags 3-5 and weird stuff not implemented
	
		
	

	
	
	

	


ldxr_loop38:
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

!vbtvbt
!        shlr16 r0
	
!vbtvbt        
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_DE - _z80_BC,r5),r0            ! r0 = DE register
		jsr @r2
		shlr16 r0
	

	
	mov.l @(_z80_HL - _z80_BC,r5),r2      ! DIRT_REG = HL register
	mov.l @(_z80_BC - _z80_BC,r5),r1       ! TMP_REG = BC register
!vbtvbt 
        shlr16 r2
        shlr16 r1        

	
		add #1,r0
		add #1,r2
	

	dt r1               ! decrement BC
	mov.w r0,@(_z80_DE - _z80_BC,r5)             ! writeback zDE register
!vbtvbt utile car décrémenté       
!	extu.w DIRT_REG,DIRT_REG ! no sign extension required
!	extu.w TMP_REG,TMP_REG   ! no sign extension required
!vbtvbt        
!	mov.l TMP_REG,zBC        ! writeback zBC register
        shll16 r1
        shll16 r2                
	mov.l r1,@(_z80_BC - _z80_BC,r5)        ! writeback zBC register
	bt/s ldxr_zero38
!	mov.l DIRT_REG,zHL       ! writeback zHL register
        mov.l r2,@(_z80_HL - _z80_BC,r5)       ! writeback zHL register
	
	add #-21,r7
	
	cmp/pl r7
	bt ldxr_loop38

	! TODO: hacer un set posicional y otro no posicional
!	SET_iVN(`ld')
	mov #0x08,r8
!	mov #(FLAG_S|FLAG_Z|FLAG_C),DIRT_REG
!	and DIRT_REG,zF

	
	mov #0x01,r2
	add #-2,r6              ! rewind to instruction start
	
	
	


	! jump to z80_finish
	mov.l z80_finish_39,r0
	jmp @r0
	or r2,r8

.align 2
	z80_finish_39: .long z80_finish

	
.align 5
ldxr_zero38:
	mov #0x08,r8
	


	
	jmp @r12
	add #-16,r7




	
	
	
ed_b8:

	
	
	! Flags 3-5 and weird stuff not implemented
	

	
	
	

	


ldxr_loop40:
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

!vbtvbt
!        shlr16 r0
	
!vbtvbt        
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_DE - _z80_BC,r5),r0            ! r0 = DE register
		jsr @r2
		shlr16 r0
	

	
	mov.l @(_z80_HL - _z80_BC,r5),r2      ! DIRT_REG = HL register
	mov.l @(_z80_BC - _z80_BC,r5),r1       ! TMP_REG = BC register
!vbtvbt 
        shlr16 r2
        shlr16 r1        

	
		add #-1,r0
		add #-1,r2
	

	dt r1               ! decrement BC
	mov.w r0,@(_z80_DE - _z80_BC,r5)             ! writeback zDE register
!vbtvbt utile car décrémenté       
!	extu.w DIRT_REG,DIRT_REG ! no sign extension required
!	extu.w TMP_REG,TMP_REG   ! no sign extension required
!vbtvbt        
!	mov.l TMP_REG,zBC        ! writeback zBC register
        shll16 r1
        shll16 r2                
	mov.l r1,@(_z80_BC - _z80_BC,r5)        ! writeback zBC register
	bt/s ldxr_zero40
!	mov.l DIRT_REG,zHL       ! writeback zHL register
        mov.l r2,@(_z80_HL - _z80_BC,r5)       ! writeback zHL register
	
	add #-21,r7
	
	cmp/pl r7
	bt ldxr_loop40

	! TODO: hacer un set posicional y otro no posicional
!	SET_iVN(`ld')
	mov #0x08,r8
!	mov #(FLAG_S|FLAG_Z|FLAG_C),DIRT_REG
!	and DIRT_REG,zF

	
	mov #0x01,r2
	add #-2,r6              ! rewind to instruction start
	
	
	


	! jump to z80_finish
	mov.l z80_finish_41,r0
	jmp @r0
	or r2,r8

.align 2
	z80_finish_41: .long z80_finish

	
.align 5
ldxr_zero40:
	mov #0x08,r8
	


	
	jmp @r12
	add #-16,r7




! **************************************************************************
! search group CP[ID][R ]


	
	
	
ed_a1:

	
	
	
	

	


	
!	mov #FLAG_C,DIRT_REG
!	and DIRT_REG,zF     ! isolate C flag
	mov.l @(_z80_HL - _z80_BC,r5),r0            ! Read from (HL)
	shlr16 r0
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8

	
	
		mov.l @r10,r2
                
		jsr @r2
		exts.b r3,r3
	


	mov r3,r2
	! Post-xcrement HL register
	add #1,r0
	sub r1,r2       ! compare result
	mov.w r0,@(_z80_HL - _z80_BC,r5)    ! writeback HL

	mov.w @(_z80_BC - _z80_BC,r5),r0
!	exts.b DIRT_REG,DIRT_REG
	dt r0
	bt/s cpi_zero42
	mov.w r0,@(_z80_BC - _z80_BC,r5)    ! writeback BC

	mov #0x01,r0    ! Set P if BC != 0
	or r0,r8

cpi_zero42:

	! SZ flag calculation
	mov r2,r13
	or #0x02,r0    ! set N flag
	or r0,r8

	! H flag calculation
	mov r2,r0
	xor r1,r0
	xor r3,r0
	and #0x10,r0
	shlr2 r0
	shlr r0
	or r0,r8
	
	


	
	jmp @r12
	add #-16,r7




	
	
	
ed_a9:

	
	
	
	

	


	
!	mov #FLAG_C,DIRT_REG
!	and DIRT_REG,zF     ! isolate C flag
	mov.l @(_z80_HL - _z80_BC,r5),r0            ! Read from (HL)
	shlr16 r0
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8

	
	
		mov.l @r10,r2
                
		jsr @r2
		exts.b r3,r3
	


	mov r3,r2
	! Post-xcrement HL register
	add #-1,r0
	sub r1,r2       ! compare result
	mov.w r0,@(_z80_HL - _z80_BC,r5)    ! writeback HL

	mov.w @(_z80_BC - _z80_BC,r5),r0
!	exts.b DIRT_REG,DIRT_REG
	dt r0
	bt/s cpi_zero43
	mov.w r0,@(_z80_BC - _z80_BC,r5)    ! writeback BC

	mov #0x01,r0    ! Set P if BC != 0
	or r0,r8

cpi_zero43:

	! SZ flag calculation
	mov r2,r13
	or #0x02,r0    ! set N flag
	or r0,r8

	! H flag calculation
	mov r2,r0
	xor r1,r0
	xor r3,r0
	and #0x10,r0
	shlr2 r0
	shlr r0
	or r0,r8
	
	


	
	jmp @r12
	add #-16,r7




	
	
	
ed_b1:

	
	
	
	

	


	
!	mov #FLAG_C,DIRT_REG
	exts.b r3,r3                  ! sign-extend zA to compare against (HL)
!	and DIRT_REG,zF            ! keep the old carry
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


cpir_loop44:

	! Read from (HL)
!vbtvbt        
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	


	mov.l @(_z80_BC - _z80_BC,r5),r2
	shlr16 r2

	! post-xcrement HL register
	add #1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)               ! writeback HL
	dt r2                ! decrement BC
!	extu.w DIRT_REG,DIRT_REG
!vbt à tester !!!!!
	shll16 r2
	bt/s cpir_end_bc44       ! end due to BC = 0
	mov.l r2,@(_z80_BC - _z80_BC,r5)

	
	cmp/eq r3,r1          ! end due to A = (HL)
	bt cpir_end_equal44

	add #-21,r7
	cmp/pl r7
	bt cpir_loop44

	! out of cycles, but not finished
	mov r3,r2
	sub r1,r2
	
	

	! SZ flag calculation
	mov #0x01, r0
	mov r2,r13
	or r0,r8

	! H flag calculation
	mov r2,r0
	xor r1,r0
	xor r3,r0
	and #0x10,r0
	shlr2 r0
	shlr r0
	or r0,r8
	
	
	
	
	
	


	! jump to z80_finish
	mov.l z80_finish_45,r0
	jmp @r0
	add #-2,r6

.align 2
	z80_finish_45: .long z80_finish
     ! rewind to instruction start and exit

.align 5
cpir_end_equal44:
	mov #0x04,r2
	or r2,r8
	
cpir_end_bc44:
	mov r3,r2
	sub r1,r2

	! SZ flag calculation
	mov r2,r13

	! H flag calculation
	mov r2,r0
	xor r1,r0
	xor r3,r0
	and #0x10,r0
	shlr2 r0
	shlr r0
	or r0,r8

	


	
	jmp @r12
	add #-16,r7




	
	
	
ed_b9:

	
	
	
	

	


	
!	mov #FLAG_C,DIRT_REG
	exts.b r3,r3                  ! sign-extend zA to compare against (HL)
!	and DIRT_REG,zF            ! keep the old carry
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


cpir_loop46:

	! Read from (HL)
!vbtvbt        
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	


	mov.l @(_z80_BC - _z80_BC,r5),r2
	shlr16 r2

	! post-xcrement HL register
	add #-1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)               ! writeback HL
	dt r2                ! decrement BC
!	extu.w DIRT_REG,DIRT_REG
!vbt à tester !!!!!
	shll16 r2
	bt/s cpir_end_bc46       ! end due to BC = 0
	mov.l r2,@(_z80_BC - _z80_BC,r5)

	
	cmp/eq r3,r1          ! end due to A = (HL)
	bt cpir_end_equal46

	add #-21,r7
	cmp/pl r7
	bt cpir_loop46

	! out of cycles, but not finished
	mov r3,r2
	sub r1,r2
	
	

	! SZ flag calculation
	mov #0x01, r0
	mov r2,r13
	or r0,r8

	! H flag calculation
	mov r2,r0
	xor r1,r0
	xor r3,r0
	and #0x10,r0
	shlr2 r0
	shlr r0
	or r0,r8
	
	
	
	
	
	


	! jump to z80_finish
	mov.l z80_finish_47,r0
	jmp @r0
	add #-2,r6

.align 2
	z80_finish_47: .long z80_finish
     ! rewind to instruction start and exit

.align 5
cpir_end_equal46:
	mov #0x04,r2
	or r2,r8
	
cpir_end_bc46:
	mov r3,r2
	sub r1,r2

	! SZ flag calculation
	mov r2,r13

	! H flag calculation
	mov r2,r0
	xor r1,r0
	xor r3,r0
	and #0x10,r0
	shlr2 r0
	shlr r0
	or r0,r8

	


	
	jmp @r12
	add #-16,r7




! **************************************************************************
! 8 bit Arithmatic


	
	
	
op_80:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-4,r7




	
	
	
op_81:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-4,r7




	
	
	
op_82:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-4,r7




	
	
	
op_83:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-4,r7




	
	
	
op_84:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-4,r7




	
	
	
op_85:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-4,r7




	
	
	
op_87:

	
	
	
	

	


	
	
	! Load operand to r0
	mov r3,r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-4,r7




	
	
	
dd_84:

   
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-8,r7




	
	
	
dd_85:

   
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC + 1,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-8,r7




	
	
	
fd_84:

   
	
	
	

	

	
	
	mov.l _z80_IY_57,r2
	mov.b @r2,r0

	
	
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-8,r7


.align 2
	
	
		_z80_IY_57: .long _z80_IY + 1
	




	
	
	
fd_85:

   
	
	
	

	

	
	
	mov.l _z80_IY_58,r2
	mov.b @r2,r0

	
	
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-8,r7


.align 2
	
	
		
			_z80_IY_58: .long _z80_IY
		
	




	
	
	
op_86:

	
	
	
	

	

!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	mov r1,r0
	
	
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-7,r7




	
	
	
op_c6:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-8,r7




	
	
	
dd_86:

	
	
	
	

	


	mov.w @(_z80_IX - _z80_BC,r5),r0
	mov r0,r1
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-19,r7




	
	
	
fd_86:

	
	
	
	

	


	mov.w @(_z80_IY - _z80_BC,r5),r0
	mov r0,r1
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	add r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	addv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(add,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	addc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-19,r7




	
	
	
op_88:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-4,r7




	
	
	
op_89:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-4,r7




	
	
	
op_8a:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-4,r7




	
	
	
op_8b:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-4,r7




	
	
	
op_8c:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-4,r7




	
	
	
op_8d:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-4,r7




	
	
	
op_8f:

	
	
	
	

	


	
	
	! Load operand to r0
	mov r3,r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-4,r7




	
	
	
dd_8c:

   
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-8,r7




	
	
	
dd_8d:

   
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC + 1,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-8,r7




	
	
	
fd_8c:

   
	
	
	

	

	
	
	mov.l _z80_IY_72,r2
	mov.b @r2,r0

	
	
	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-8,r7


.align 2
	
	
		_z80_IY_72: .long _z80_IY + 1
	




	
	
	
fd_8d:

   
	
	
	

	

	
	
	mov.l _z80_IY_73,r2
	mov.b @r2,r0

	
	
	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-8,r7


.align 2
	
	
		
			_z80_IY_73: .long _z80_IY
		
	




	
	
	
op_8e:

	
	
	
	

	

!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	mov r1,r0
	
	
	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-7,r7




	
	
	
op_ce:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-8,r7




	
	
	
dd_8e:

	
	
	
	

	


	mov.w @(_z80_IX - _z80_BC,r5),r0
	mov r0,r1
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-19,r7




	
	
	
fd_8e:

	
	
	
	

	


	mov.w @(_z80_IY - _z80_BC,r5),r0
	mov r0,r1
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	
	
	
	! Set N flag according to operation performed
!	ifelse(add,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #2,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	addc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	addv r0,r3
	movt r1             ! partial V flag
	addv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	addc r0,r13
	movt r1             ! partial C flag
	addc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-19,r7




	
	
	
op_90:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-4,r7




	
	
	
op_91:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-4,r7




	
	
	
op_92:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-4,r7




	
	
	
op_93:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-4,r7




	
	
	
op_94:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-4,r7




	
	
	
op_95:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-4,r7




	
	
	
op_97:

	
	
	
	

	


	
	
	! Load operand to r0
	mov r3,r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-4,r7




	
	
	
dd_94:

   
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-8,r7




	
	
	
dd_95:

   
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC + 1,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-8,r7




	
	
	
fd_94:

   
	
	
	

	

	
	
	mov.l _z80_IY_87,r2
	mov.b @r2,r0

	
	
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-8,r7


.align 2
	
	
		_z80_IY_87: .long _z80_IY + 1
	




	
	
	
fd_95:

   
	
	
	

	

	
	
	mov.l _z80_IY_88,r2
	mov.b @r2,r0

	
	
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-8,r7


.align 2
	
	
		
			_z80_IY_88: .long _z80_IY
		
	




	
	
	
op_96:

	
	
	
	

	

!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	mov r1,r0
	
	
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-7,r7




	
	
	
op_d6:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-8,r7




	
	
	
dd_96:

	
	
	
	

	


	mov.w @(_z80_IX - _z80_BC,r5),r0
	mov r0,r1
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-19,r7




	
	
	
fd_96:

	
	
	
	

	


	mov.w @(_z80_IY - _z80_BC,r5),r0
	mov r0,r1
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

		mov r3,r1       ! Save A register operand
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(sub,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
	subc r0,r3
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

		
	! Set Z
	mov r3,r13

	

	




	
	jmp @r12
	add #-19,r7




	
	
	
op_98:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-4,r7




	
	
	
op_99:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-4,r7




	
	
	
op_9a:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-4,r7




	
	
	
op_9b:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-4,r7




	
	
	
op_9c:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-4,r7




	
	
	
op_9d:

	
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-4,r7




	
	
	
op_9f:

	
	
	
	

	


	
	
	! Load operand to r0
	mov r3,r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-4,r7




	
	
	
dd_9c:

   
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-8,r7




	
	
	
dd_9d:

   
	
	
	

	


	
	
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC + 1,r5),r0

	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-8,r7




	
	
	
fd_9c:

   
	
	
	

	

	
	
	mov.l _z80_IY_102,r2
	mov.b @r2,r0

	
	
	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-8,r7


.align 2
	
	
		_z80_IY_102: .long _z80_IY + 1
	




	
	
	
fd_9d:

   
	
	
	

	

	
	
	mov.l _z80_IY_103,r2
	mov.b @r2,r0

	
	
	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-8,r7


.align 2
	
	
		
			_z80_IY_103: .long _z80_IY
		
	




	
	
	
op_9e:

	
	
	
	

	

!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	mov r1,r0
	
	
	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-7,r7




	
	
	
op_de:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-8,r7




	
	
	
dd_9e:

	
	
	
	

	


	mov.w @(_z80_IX - _z80_BC,r5),r0
	mov r0,r1
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-19,r7




	
	
	
fd_9e:

	
	
	
	

	


	mov.w @(_z80_IY - _z80_BC,r5),r0
	mov r0,r1
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	
	
	
	! Set N flag according to operation performed
!	ifelse(sub,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov #0x0F,r2       ! nibble mask for A operand
	mov r2,r13   ! nibble mask for r0 operand
	mov r2,r1     ! get mask to compare against result
	and r3,r2          ! save zA temporary masking for H flag calculation
	and r0,r13	

	cmp/pl r11           ! C -> T
	subc r13,r2
	cmp/hi r1,r2
	addc r8,r8               ! merge partial H flag in

	! V flag calculation

	
	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
    ! prepare register operands
	
	
		
		
		
			shll16 r11
			shll8 r11
		

		
		
		
	

	
	
		
		
		
			shll16 r3
			shll8 r3
		

		
		
		
	

	mov r3,r13         ! save zA to get C flag
	subv r0,r3
	movt r1             ! partial V flag
	subv r11,r3
	addc r8,r8               ! merge partial V flag in
	or r1,r8            ! V flag in flags register

	! C flag calculation	
	clrt
	subc r0,r13
	movt r1             ! partial C flag
	subc r11,r13
	movt r11             ! partial C flag in
	or r1,r11       ! C flag in register


	! Get result
	
	

	
	
		
		
		

		
		
		
			shlr16 r3
			shlr8 r3
		
	

	
	! Set ZSP flags
	
	! Set Z
	mov r3,r13





	
	jmp @r12
	add #-19,r7




! **************************************************************************
! 8 bit logical group


	
	
	
op_a0:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

		
	
	and r0,r3    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0


	

	
	jmp @r12
	add #-4,r7




	
	
	
op_a1:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

		
	
	and r0,r3    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0


	

	
	jmp @r12
	add #-4,r7




	
	
	
op_a2:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

		
	
	and r0,r3    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0


	

	
	jmp @r12
	add #-4,r7




	
	
	
op_a3:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

		
	
	and r0,r3    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0


	

	
	jmp @r12
	add #-4,r7




	
	
	
op_a4:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

		
	
	and r0,r3    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0


	

	
	jmp @r12
	add #-4,r7




	
	
	
op_a5:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

		
	
	and r0,r3    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0


	

	
	jmp @r12
	add #-4,r7




	
	
	
op_a7:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	
	    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0


	

	
	jmp @r12
	add #-4,r7




	
	
	
dd_a4:

   
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC,r5),r0

		
	
	and r0,r3    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0


	

	
	jmp @r12
	add #-8,r7




	
	
	
dd_a5:

   
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC + 1,r5),r0

		
	
	and r0,r3    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0


	

	
	jmp @r12
	add #-8,r7




	
	
	
fd_a4:

   
	
	
	

	


	
	mov.l _z80_IY_117,r2
	mov.b @r2,r0

	
	
	and r0,r3    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0



	
	jmp @r12
	add #-8,r7


.align 2
	_z80_IY_117: .long _z80_IY



	
	
	
fd_a5:

   
	
	
	

	


	
	mov.l _z80_IY_118,r2
	mov.b @r2,r0

	
	
	and r0,r3    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0



	
	jmp @r12
	add #-8,r7


.align 2
	_z80_IY_118: .long _z80_IY



	
	
	
op_e6:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
	and r0,r3    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0



	
	jmp @r12
	add #-8,r7




	
	
	
op_a6:

	
	
	
	

	

	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	

	mov r1,r0
	
	
	and r0,r3    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0



	
	jmp @r12
	add #-7,r7




	
	
	
dd_a6:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IX - _z80_BC,r5),r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	and r0,r3    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0



	
	jmp @r12
	add #-19,r7




	
	
	
fd_a6:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IY - _z80_BC,r5),r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	and r0,r3    ! do the operation

	
	! Set Z
	mov r3,r13

	
	


	mov #0,r11    ! C = 0
	mov #0x02,r8      ! H = 1, N,V = 0



	
	jmp @r12
	add #-19,r7




	
	
	
op_a8:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

		
	
	
		mov #0,r8
		xor r0, r3
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	



	

	
	jmp @r12
	add #-4,r7




	
	
	
op_a9:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

		
	
	
		mov #0,r8
		xor r0, r3
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	



	

	
	jmp @r12
	add #-4,r7




	
	
	
op_aa:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

		
	
	
		mov #0,r8
		xor r0, r3
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	



	

	
	jmp @r12
	add #-4,r7




	
	
	
op_ab:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

		
	
	
		mov #0,r8
		xor r0, r3
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	



	

	
	jmp @r12
	add #-4,r7




	
	
	
op_ac:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

		
	
	
		mov #0,r8
		xor r0, r3
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	



	

	
	jmp @r12
	add #-4,r7




	
	
	
op_ad:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

		
	
	
		mov #0,r8
		xor r0, r3
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	



	

	
	jmp @r12
	add #-4,r7




	
	
	
op_af:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	
	
		mov #0,r3
		mov r3,r8
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	



	

	
	jmp @r12
	add #-4,r7




	
	
	
dd_ac:

   
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC,r5),r0

		
	
	
		mov #0,r8
		xor r0, r3
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	



	

	
	jmp @r12
	add #-8,r7




	
	
	
dd_ad:

   
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC + 1,r5),r0

		
	
	
		mov #0,r8
		xor r0, r3
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	



	

	
	jmp @r12
	add #-8,r7




	
	
	
fd_ac:

   
	
	
	

	


	
	mov.l _z80_IY_132,r2
	mov.b @r2,r0

	
	
	
		mov #0,r8
		xor r0, r3
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	




	
	jmp @r12
	add #-8,r7


.align 2
	_z80_IY_132: .long _z80_IY



	
	
	
fd_ad:

   
	
	
	

	


	
	mov.l _z80_IY_133,r2
	mov.b @r2,r0

	
	
	
		mov #0,r8
		xor r0, r3
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	




	
	jmp @r12
	add #-8,r7


.align 2
	_z80_IY_133: .long _z80_IY



	
	
	
op_ee:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
	
		mov #0,r8
		xor r0, r3
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	




	
	jmp @r12
	add #-8,r7




	
	
	
op_ae:

	
	
	
	

	

	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	

	mov r1,r0
	
	
	
		mov #0,r8
		xor r0, r3
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	




	
	jmp @r12
	add #-7,r7




	
	
	
dd_ae:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IX - _z80_BC,r5),r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	
		mov #0,r8
		xor r0, r3
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	




	
	jmp @r12
	add #-19,r7




	
	
	
fd_ae:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IY - _z80_BC,r5),r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	
		mov #0,r8
		xor r0, r3
	
	
	mov r8,r11    ! Clear carry
	
	! Set Z
	mov r3,r13

	
	




	
	jmp @r12
	add #-19,r7




	
	
	
op_b0:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

		
	
	or r0,r3    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	



	

	
	jmp @r12
	add #-4,r7




	
	
	
op_b1:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

		
	
	or r0,r3    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	



	

	
	jmp @r12
	add #-4,r7




	
	
	
op_b2:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

		
	
	or r0,r3    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	



	

	
	jmp @r12
	add #-4,r7




	
	
	
op_b3:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

		
	
	or r0,r3    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	



	

	
	jmp @r12
	add #-4,r7




	
	
	
op_b4:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

		
	
	or r0,r3    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	



	

	
	jmp @r12
	add #-4,r7




	
	
	
op_b5:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

		
	
	or r0,r3    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	



	

	
	jmp @r12
	add #-4,r7




	
	
	
op_b7:

	
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	
	    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	



	

	
	jmp @r12
	add #-4,r7




	
	
	
dd_b4:

   
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC,r5),r0

		
	
	or r0,r3    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	



	

	
	jmp @r12
	add #-8,r7




	
	
	
dd_b5:

   
	
	
	

	


	! Si el registro es A no cargar, dado que no es conveniente
	! Si no es A, cargar y especificar r0
	
		
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC + 1,r5),r0

		
	
	or r0,r3    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	



	

	
	jmp @r12
	add #-8,r7




	
	
	
fd_b4:

   
	
	
	

	


	
	mov.l _z80_IY_147,r2
	mov.b @r2,r0

	
	
	or r0,r3    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	




	
	jmp @r12
	add #-8,r7


.align 2
	_z80_IY_147: .long _z80_IY



	
	
	
fd_b5:

   
	
	
	

	


	
	mov.l _z80_IY_148,r2
	mov.b @r2,r0

	
	
	or r0,r3    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	




	
	jmp @r12
	add #-8,r7


.align 2
	_z80_IY_148: .long _z80_IY



	
	
	
op_f6:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
	or r0,r3    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	




	
	jmp @r12
	add #-8,r7




	
	
	
op_b6:

	
	
	
	

	

	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	

	mov r1,r0
	
	
	or r0,r3    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	




	
	jmp @r12
	add #-7,r7




	
	
	
dd_b6:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IX - _z80_BC,r5),r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	or r0,r3    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	




	
	jmp @r12
	add #-19,r7




	
	
	
fd_b6:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IY - _z80_BC,r5),r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	or r0,r3    ! do the operation

	mov #0,r11    ! Flag C = 0

	! Fijar NVH aqui (N,H = 0, V = parity)
	
	! Set Z
	mov r3,r13

	mov #0,r8

	
	




	
	jmp @r12
	add #-19,r7




! **************************************************************************
! CP group


	
	
	
op_b8:

	
	
	
	

	


	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	



	
	jmp @r12
	add #-4,r7




	
	
	
op_b9:

	
	
	
	

	


	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	



	
	jmp @r12
	add #-4,r7




	
	
	
op_ba:

	
	
	
	

	


	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	



	
	jmp @r12
	add #-4,r7




	
	
	
op_bb:

	
	
	
	

	


	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	



	
	jmp @r12
	add #-4,r7




	
	
	
op_bc:

	
	
	
	

	


	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	



	
	jmp @r12
	add #-4,r7




	
	
	
op_bd:

	
	
	
	

	


	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	



	
	jmp @r12
	add #-4,r7




	
	
	
op_bf:

	
	
	
	

	


	
	! Load operand to r0
	mov r3,r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	



	
	jmp @r12
	add #-4,r7




	
	
	
dd_bc:

   
	
	
	

	


	
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	



	
	jmp @r12
	add #-8,r7




	
	
	
dd_bd:

   
	
	
	

	


	
	! Load operand to r0
	mov.b @(_z80_IX - _z80_BC + 1,r5),r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	



	
	jmp @r12
	add #-8,r7




	
	
	
fd_bc:

   
	
	
	

	


	
	mov.l _z80_IY_162,r2
	mov.b @r2,r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	



	
	jmp @r12
	add #-8,r7


.align 2
	
	
		_z80_IY_162: .long _z80_IY + 1
	




	
	
	
fd_bd:

   
	
	
	

	


	
	mov.l _z80_IY_163,r2
	mov.b @r2,r0

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	



	
	jmp @r12
	add #-8,r7


.align 2
	
	
		
			_z80_IY_163: .long _z80_IY
		
	




	
	
	
op_be:

	
	
	
	

	

!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	mov r1,r0
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	


	
	
	jmp @r12
	add #-7,r7




	
	
	
op_fe:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	



	
	jmp @r12
	add #-8,r7




	
	
	
dd_be:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IX - _z80_BC,r5),r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	



	
	jmp @r12
	add #-19,r7




	
	
	
fd_be:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IY - _z80_BC,r5),r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		add r1,r0
	

	mov r1,r0
	
	
	
	

	

	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8


	! H flag calculation
	mov r3, r13            ! save original A register value
	mov #0xF,r2           ! H masking
	and r2,r13   ! FLAGS_ZSP = A masked
	and r0,r2                ! DIRT_REG = r0 masked
	mov #0x10,r1
	sub r2,r13     ! do the operation
	cmp/hs r1,r13
	addc r8,r8                          ! set H flag

	
	

	! VC flag calculation
	
		mov r3,r1       ! Save A register operand
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

	

	
	
		
		
		
			shll16 r0
			shll8 r0
		

		
		
		
	
  ! Prepare register operand to operate
	subv r0,r1             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result

	
	!ifelse(cmp,`sub',`op r0,TMP_REG',`op`v' r0,TMP_REG')             ! TMP_REG = result
	!op r0,TMP_REG             ! TMP_REG = result
	addc r8,r8                   ! V flag set

	
		mov r3,r1
		
	
		
		
		
			shll16 r1
			shll8 r1
		

		
		
		
	

!		shld DIRT_REG,TMP_REG
	
	subc r0,r1
	movt r11        ! Carry flag set

	! Resultado presente en zA o TMP_REG
	! dependiendo del tipo de operacion (add/sub, cmp)

	
	

	
		
	
		
		
		

		
		
		
			shlr16 r1
			shlr8 r1
		
	

		
	! Set Z
	mov r1,r13

	

	



	
	jmp @r12
	add #-19,r7




! **************************************************************************
! DEC/INC 8 bit group


	
	
	
op_04:

	
	
	
	

	


	
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	mov.l INC_Table_168,r1
	mov.b @(r0,r1),r8
	add #1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_BC - _z80_BC,r5)


	
	jmp @r12
	add #-4,r7


.align 2
	INC_Table_168: .long INC_Table



	
	
	
op_0c:

	
	
	
	

	


	
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	mov.l INC_Table_169,r1
	mov.b @(r0,r1),r8
	add #1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)


	
	jmp @r12
	add #-4,r7


.align 2
	INC_Table_169: .long INC_Table



	
	
	
op_14:

	
	
	
	

	


	
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	mov.l INC_Table_170,r1
	mov.b @(r0,r1),r8
	add #1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_DE - _z80_BC,r5)


	
	jmp @r12
	add #-4,r7


.align 2
	INC_Table_170: .long INC_Table



	
	
	
op_1c:

	
	
	
	

	


	
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	mov.l INC_Table_171,r1
	mov.b @(r0,r1),r8
	add #1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)


	
	jmp @r12
	add #-4,r7


.align 2
	INC_Table_171: .long INC_Table



	
	
	
op_24:

	
	
	
	

	


	
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	mov.l INC_Table_172,r1
	mov.b @(r0,r1),r8
	add #1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_HL - _z80_BC,r5)


	
	jmp @r12
	add #-4,r7


.align 2
	INC_Table_172: .long INC_Table



	
	
	
op_2c:

	
	
	
	

	


	
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	mov.l INC_Table_173,r1
	mov.b @(r0,r1),r8
	add #1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)


	
	jmp @r12
	add #-4,r7


.align 2
	INC_Table_173: .long INC_Table



	
	
	
op_3c:

	
	
	
	

	


	
	exts.b r3,r0

	
	
	mov.l INC_Table_174,r1
	mov.b @(r0,r1),r8
	add #1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov r0,r3


	
	jmp @r12
	add #-4,r7


.align 2
	INC_Table_174: .long INC_Table



	
	
	
dd_24:

   
	
	
	

	


	
	mov.b @(_z80_IX - _z80_BC,r5),r0

	
	
	mov.l INC_Table_175,r1
	mov.b @(r0,r1),r8
	add #1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_IX - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7


.align 2
	INC_Table_175: .long INC_Table



	
	
	
dd_2c:

   
	
	
	

	


	
	mov.b @(_z80_IX - _z80_BC + 1,r5),r0

	
	
	mov.l INC_Table_176,r1
	mov.b @(r0,r1),r8
	add #1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_IX - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7


.align 2
	INC_Table_176: .long INC_Table



	
	
	
fd_24:

   
	
	
	

	


	
	mov.l _z80_IY_177,r2
	mov.b @r2,r0

	
	
	mov.l INC_Table_177,r1
	mov.b @(r0,r1),r8
	add #1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.l _z80_IY_177,r1
	mov.b r0,@r1


	
	jmp @r12
	add #-8,r7


.align 2
	INC_Table_177: .long INC_Table
	
	
		_z80_IY_177: .long _z80_IY + 1
	




	
	
	
fd_2c:

   
	
	
	

	


	
	mov.l _z80_IY_178,r2
	mov.b @r2,r0

	
	
	mov.l INC_Table_178,r1
	mov.b @(r0,r1),r8
	add #1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.l _z80_IY_178,r1
	mov.b r0,@r1


	
	jmp @r12
	add #-8,r7


.align 2
	INC_Table_178: .long INC_Table
	
	
		
			_z80_IY_178: .long _z80_IY
		
	




	
	
	
op_34:

	
	
	
	

	

!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	exts.b r1,r0         ! requires to be sign-extended
	
	
	mov.l INC_Table_179,r1
	mov.b @(r0,r1),r8
	add #1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	
	jmp @r12
	add #-11,r7


.align 2
	INC_Table_179: .long INC_Table



	
	
	
dd_34:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IX - _z80_BC,r5),r0
	add r1,r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0          ! requires to be sign-extended
	
	
	mov.l INC_Table_180,r1
	mov.b @(r0,r1),r8
	add #1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7


.align 2
	INC_Table_180: .long INC_Table



	
	
	
fd_34:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IY - _z80_BC,r5),r0
	add r1,r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0          ! requires to be sign-extended
	
	
	mov.l INC_Table_181,r1
	mov.b @(r0,r1),r8
	add #1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7


.align 2
	INC_Table_181: .long INC_Table



	
	
	
op_05:

	
	
	
	

	


	
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	mov.l DEC_Table_182,r1
	mov.b @(r0,r1),r8
	add #-1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_BC - _z80_BC,r5)


	
	jmp @r12
	add #-4,r7


.align 2
	DEC_Table_182: .long DEC_Table



	
	
	
op_0d:

	
	
	
	

	


	
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	mov.l DEC_Table_183,r1
	mov.b @(r0,r1),r8
	add #-1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)


	
	jmp @r12
	add #-4,r7


.align 2
	DEC_Table_183: .long DEC_Table



	
	
	
op_15:

	
	
	
	

	


	
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	mov.l DEC_Table_184,r1
	mov.b @(r0,r1),r8
	add #-1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_DE - _z80_BC,r5)


	
	jmp @r12
	add #-4,r7


.align 2
	DEC_Table_184: .long DEC_Table



	
	
	
op_1d:

	
	
	
	

	


	
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	mov.l DEC_Table_185,r1
	mov.b @(r0,r1),r8
	add #-1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)


	
	jmp @r12
	add #-4,r7


.align 2
	DEC_Table_185: .long DEC_Table



	
	
	
op_25:

	
	
	
	

	


	
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	mov.l DEC_Table_186,r1
	mov.b @(r0,r1),r8
	add #-1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_HL - _z80_BC,r5)


	
	jmp @r12
	add #-4,r7


.align 2
	DEC_Table_186: .long DEC_Table



	
	
	
op_2d:

	
	
	
	

	


	
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	mov.l DEC_Table_187,r1
	mov.b @(r0,r1),r8
	add #-1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)


	
	jmp @r12
	add #-4,r7


.align 2
	DEC_Table_187: .long DEC_Table



	
	
	
op_3d:

	
	
	
	

	


	
	exts.b r3,r0

	
	
	mov.l DEC_Table_188,r1
	mov.b @(r0,r1),r8
	add #-1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov r0,r3


	
	jmp @r12
	add #-4,r7


.align 2
	DEC_Table_188: .long DEC_Table



	
	
	
dd_25:

   
	
	
	

	


	
	mov.b @(_z80_IX - _z80_BC,r5),r0

	
	
	mov.l DEC_Table_189,r1
	mov.b @(r0,r1),r8
	add #-1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_IX - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7


.align 2
	DEC_Table_189: .long DEC_Table



	
	
	
dd_2d:

   
	
	
	

	


	
	mov.b @(_z80_IX - _z80_BC + 1,r5),r0

	
	
	mov.l DEC_Table_190,r1
	mov.b @(r0,r1),r8
	add #-1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.b r0,@(_z80_IX - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7


.align 2
	DEC_Table_190: .long DEC_Table



	
	
	
fd_25:

   
	
	
	

	


	
	mov.l _z80_IY_191,r2
	mov.b @r2,r0

	
	
	mov.l DEC_Table_191,r1
	mov.b @(r0,r1),r8
	add #-1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.l _z80_IY_191,r1
	mov.b r0,@r1


	
	jmp @r12
	add #-8,r7


.align 2
	DEC_Table_191: .long DEC_Table
	
	
		_z80_IY_191: .long _z80_IY + 1
	




	
	
	
fd_2d:

   
	
	
	

	


	
	mov.l _z80_IY_192,r2
	mov.b @r2,r0

	
	
	mov.l DEC_Table_192,r1
	mov.b @(r0,r1),r8
	add #-1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	
	mov.l _z80_IY_192,r1
	mov.b r0,@r1


	
	jmp @r12
	add #-8,r7


.align 2
	DEC_Table_192: .long DEC_Table
	
	
		
			_z80_IY_192: .long _z80_IY
		
	




	
	
	
op_35:

	
	
	
	

	

!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	exts.b r1,r0         ! requires to be sign-extended
	
	
	mov.l DEC_Table_193,r1
	mov.b @(r0,r1),r8
	add #-1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	
	jmp @r12
	add #-11,r7


.align 2
	DEC_Table_193: .long DEC_Table



	
	
	
dd_35:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IX - _z80_BC,r5),r0
	add r1,r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0          ! requires to be sign-extended
	
	
	mov.l DEC_Table_194,r1
	mov.b @(r0,r1),r8
	add #-1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7


.align 2
	DEC_Table_194: .long DEC_Table



	
	
	
fd_35:

	
	
	
	

	


	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	mov.w @(_z80_IY - _z80_BC,r5),r0
	add r1,r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0          ! requires to be sign-extended
	
	
	mov.l DEC_Table_195,r1
	mov.b @(r0,r1),r8
	add #-1,r0
	mov.l r11,@-r15
	mov.l r0,@-r15
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	mov.l @r15+,r0
	mov.l @r15+,r11
	
	



	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7


.align 2
	DEC_Table_195: .long DEC_Table



! **************************************************************************
! ADD HL,ss


	
	
	
op_09:

	
	
	mov #(0x08 | 0x01),r2    ! H, N = 0
	mov.l @(_z80_HL - _z80_BC,r5),r0       ! 1st operand
	and r2,r8
	mov.l @(_z80_BC - _z80_BC,r5),r1  ! 2nd operand
	
!vbt inutile pour big endian        
!	shll16 r0          ! prepare 1st operand
	clrt               ! T = 0 (required by the carry operation)
!vbt inutile pour big endian        
!	shll16 TMP_REG     ! prepare 2nd operand

	addc r1,r0    ! do it
	shlr16 r0          ! get the result to the lower word
!vbt
	shlr16 r2

	movt r11       ! set carry flag
	mov.w r0,@(_z80_HL - _z80_BC,r5)       ! writeback result

	

	


	
	jmp @r12
	add #-11,r7




	
	
	
op_19:

	
	
	mov #(0x08 | 0x01),r2    ! H, N = 0
	mov.l @(_z80_HL - _z80_BC,r5),r0       ! 1st operand
	and r2,r8
	mov.l @(_z80_DE - _z80_BC,r5),r1  ! 2nd operand
	
!vbt inutile pour big endian        
!	shll16 r0          ! prepare 1st operand
	clrt               ! T = 0 (required by the carry operation)
!vbt inutile pour big endian        
!	shll16 TMP_REG     ! prepare 2nd operand

	addc r1,r0    ! do it
	shlr16 r0          ! get the result to the lower word
!vbt
	shlr16 r2

	movt r11       ! set carry flag
	mov.w r0,@(_z80_HL - _z80_BC,r5)       ! writeback result

	

	


	
	jmp @r12
	add #-11,r7




	
	
	
op_29:

	
	
	mov #(0x08 | 0x01),r2    ! H, N = 0
	mov.l @(_z80_HL - _z80_BC,r5),r0       ! 1st operand
	and r2,r8
	mov.l @(_z80_HL - _z80_BC,r5),r1  ! 2nd operand
	
!vbt inutile pour big endian        
!	shll16 r0          ! prepare 1st operand
	clrt               ! T = 0 (required by the carry operation)
!vbt inutile pour big endian        
!	shll16 TMP_REG     ! prepare 2nd operand

	addc r1,r0    ! do it
	shlr16 r0          ! get the result to the lower word
!vbt
	shlr16 r2

	movt r11       ! set carry flag
	mov.w r0,@(_z80_HL - _z80_BC,r5)       ! writeback result

	

	


	
	jmp @r12
	add #-11,r7




	
	
	
op_39:

	
	
	mov #(0x08 | 0x01),r2    ! H, N = 0
	mov.l @(_z80_HL - _z80_BC,r5),r0       ! 1st operand
	and r2,r8
	mov.l @(_z80_SP - _z80_BC,r5),r1  ! 2nd operand
	
!vbt inutile pour big endian        
!	shll16 r0          ! prepare 1st operand
	clrt               ! T = 0 (required by the carry operation)
!vbt inutile pour big endian        
!	shll16 TMP_REG     ! prepare 2nd operand

	addc r1,r0    ! do it
	shlr16 r0          ! get the result to the lower word
!vbt
	shlr16 r2

	movt r11       ! set carry flag
	mov.w r0,@(_z80_HL - _z80_BC,r5)       ! writeback result

	

	


	
	jmp @r12
	add #-11,r7




	
	
	
dd_09:

   
	
	mov #(0x08 | 0x01),r2    ! H, N = 0
	mov.l @(_z80_IX - _z80_BC,r5),r0       ! 1st operand
	and r2,r8
	mov.l @(_z80_BC - _z80_BC,r5),r1  ! 2nd operand
	
!vbt inutile pour big endian        
!	shll16 r0          ! prepare 1st operand
	clrt               ! T = 0 (required by the carry operation)
!vbt inutile pour big endian        
!	shll16 TMP_REG     ! prepare 2nd operand

	addc r1,r0    ! do it
	shlr16 r0          ! get the result to the lower word
!vbt
	shlr16 r2

	movt r11       ! set carry flag
	mov.w r0,@(_z80_IX - _z80_BC,r5)       ! writeback result

	

	


	
	jmp @r12
	add #-15,r7




	
	
	
dd_19:

   
	
	mov #(0x08 | 0x01),r2    ! H, N = 0
	mov.l @(_z80_IX - _z80_BC,r5),r0       ! 1st operand
	and r2,r8
	mov.l @(_z80_DE - _z80_BC,r5),r1  ! 2nd operand
	
!vbt inutile pour big endian        
!	shll16 r0          ! prepare 1st operand
	clrt               ! T = 0 (required by the carry operation)
!vbt inutile pour big endian        
!	shll16 TMP_REG     ! prepare 2nd operand

	addc r1,r0    ! do it
	shlr16 r0          ! get the result to the lower word
!vbt
	shlr16 r2

	movt r11       ! set carry flag
	mov.w r0,@(_z80_IX - _z80_BC,r5)       ! writeback result

	

	


	
	jmp @r12
	add #-15,r7




	
	
	
dd_29:

   
	
	mov #(0x08 | 0x01),r2    ! H, N = 0
	mov.l @(_z80_IX - _z80_BC,r5),r0       ! 1st operand
	and r2,r8
	mov.l @(_z80_IX - _z80_BC,r5),r1  ! 2nd operand
	
!vbt inutile pour big endian        
!	shll16 r0          ! prepare 1st operand
	clrt               ! T = 0 (required by the carry operation)
!vbt inutile pour big endian        
!	shll16 TMP_REG     ! prepare 2nd operand

	addc r1,r0    ! do it
	shlr16 r0          ! get the result to the lower word
!vbt
	shlr16 r2

	movt r11       ! set carry flag
	mov.w r0,@(_z80_IX - _z80_BC,r5)       ! writeback result

	

	


	
	jmp @r12
	add #-15,r7




	
	
	
dd_39:

   
	
	mov #(0x08 | 0x01),r2    ! H, N = 0
	mov.l @(_z80_IX - _z80_BC,r5),r0       ! 1st operand
	and r2,r8
	mov.l @(_z80_SP - _z80_BC,r5),r1  ! 2nd operand
	
!vbt inutile pour big endian        
!	shll16 r0          ! prepare 1st operand
	clrt               ! T = 0 (required by the carry operation)
!vbt inutile pour big endian        
!	shll16 TMP_REG     ! prepare 2nd operand

	addc r1,r0    ! do it
	shlr16 r0          ! get the result to the lower word
!vbt
	shlr16 r2

	movt r11       ! set carry flag
	mov.w r0,@(_z80_IX - _z80_BC,r5)       ! writeback result

	

	


	
	jmp @r12
	add #-15,r7




	
	
	
fd_09:

   
	
	mov #(0x08 | 0x01),r2    ! H, N = 0
	mov.l @(_z80_IY - _z80_BC,r5),r0       ! 1st operand
	and r2,r8
	mov.l @(_z80_BC - _z80_BC,r5),r1  ! 2nd operand
	
!vbt inutile pour big endian        
!	shll16 r0          ! prepare 1st operand
	clrt               ! T = 0 (required by the carry operation)
!vbt inutile pour big endian        
!	shll16 TMP_REG     ! prepare 2nd operand

	addc r1,r0    ! do it
	shlr16 r0          ! get the result to the lower word
!vbt
	shlr16 r2

	movt r11       ! set carry flag
	mov.w r0,@(_z80_IY - _z80_BC,r5)       ! writeback result

	

	


	
	jmp @r12
	add #-15,r7




	
	
	
fd_19:

   
	
	mov #(0x08 | 0x01),r2    ! H, N = 0
	mov.l @(_z80_IY - _z80_BC,r5),r0       ! 1st operand
	and r2,r8
	mov.l @(_z80_DE - _z80_BC,r5),r1  ! 2nd operand
	
!vbt inutile pour big endian        
!	shll16 r0          ! prepare 1st operand
	clrt               ! T = 0 (required by the carry operation)
!vbt inutile pour big endian        
!	shll16 TMP_REG     ! prepare 2nd operand

	addc r1,r0    ! do it
	shlr16 r0          ! get the result to the lower word
!vbt
	shlr16 r2

	movt r11       ! set carry flag
	mov.w r0,@(_z80_IY - _z80_BC,r5)       ! writeback result

	

	


	
	jmp @r12
	add #-15,r7




	
	
	
fd_29:

   
	
	mov #(0x08 | 0x01),r2    ! H, N = 0
	mov.l @(_z80_IY - _z80_BC,r5),r0       ! 1st operand
	and r2,r8
	mov.l @(_z80_IY - _z80_BC,r5),r1  ! 2nd operand
	
!vbt inutile pour big endian        
!	shll16 r0          ! prepare 1st operand
	clrt               ! T = 0 (required by the carry operation)
!vbt inutile pour big endian        
!	shll16 TMP_REG     ! prepare 2nd operand

	addc r1,r0    ! do it
	shlr16 r0          ! get the result to the lower word
!vbt
	shlr16 r2

	movt r11       ! set carry flag
	mov.w r0,@(_z80_IY - _z80_BC,r5)       ! writeback result

	

	


	
	jmp @r12
	add #-15,r7




	
	
	
fd_39:

   
	
	mov #(0x08 | 0x01),r2    ! H, N = 0
	mov.l @(_z80_IY - _z80_BC,r5),r0       ! 1st operand
	and r2,r8
	mov.l @(_z80_SP - _z80_BC,r5),r1  ! 2nd operand
	
!vbt inutile pour big endian        
!	shll16 r0          ! prepare 1st operand
	clrt               ! T = 0 (required by the carry operation)
!vbt inutile pour big endian        
!	shll16 TMP_REG     ! prepare 2nd operand

	addc r1,r0    ! do it
	shlr16 r0          ! get the result to the lower word
!vbt
	shlr16 r2

	movt r11       ! set carry flag
	mov.w r0,@(_z80_IY - _z80_BC,r5)       ! writeback result

	

	


	
	jmp @r12
	add #-15,r7




	
	
	
ed_4a:

	
	
	
	mov.l @(_z80_HL - _z80_BC,r5),r0          ! first operand
	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8

	mov.l @(_z80_BC - _z80_BC,r5),r1     ! second operand
!vbt inutile pour big endian        
	!shll16 r0             ! prepare first operand
	shll r8                 !  H = 0
	mov r0,r13      ! save 1st operand to get C
!vbt inutile pour big endian        
	!shll16 TMP_REG        ! prepare second operand
	shll16 r11        ! prepare C flag

	! V flag calculation
	addv r1,r0
	movt r2         ! partial overflow flag
	addv r11,r0
	addc r8,r8            ! partial overflow flag
	or r2,r8        ! set V in

	! Carry flag calculation
	clrt
	mov r13,r0      ! reload first operand
	addc r1,r0
	movt r2         ! DIRT_REG = partial carry flag
	clrt
	addc r11,r0         ! r0 = result
	movt r11
	or r2,r11  ! C flag

	! TODO: emulate undoc
	

	! Write result
	shlr16 r0             ! get result in place
	mov.w r0,@(_z80_HL - _z80_BC,r5)

	! Get signed result in r0
	tst r0,r0
	movt r13
	dt r13
	extu.b r13,r13
	shlr r13
	shlr8 r0
	or r0,r13

	



	
	jmp @r12
	add #-15,r7




	
	
	
ed_5a:

	
	
	
	mov.l @(_z80_HL - _z80_BC,r5),r0          ! first operand
	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8

	mov.l @(_z80_DE - _z80_BC,r5),r1     ! second operand
!vbt inutile pour big endian        
	!shll16 r0             ! prepare first operand
	shll r8                 !  H = 0
	mov r0,r13      ! save 1st operand to get C
!vbt inutile pour big endian        
	!shll16 TMP_REG        ! prepare second operand
	shll16 r11        ! prepare C flag

	! V flag calculation
	addv r1,r0
	movt r2         ! partial overflow flag
	addv r11,r0
	addc r8,r8            ! partial overflow flag
	or r2,r8        ! set V in

	! Carry flag calculation
	clrt
	mov r13,r0      ! reload first operand
	addc r1,r0
	movt r2         ! DIRT_REG = partial carry flag
	clrt
	addc r11,r0         ! r0 = result
	movt r11
	or r2,r11  ! C flag

	! TODO: emulate undoc
	

	! Write result
	shlr16 r0             ! get result in place
	mov.w r0,@(_z80_HL - _z80_BC,r5)

	! Get signed result in r0
	tst r0,r0
	movt r13
	dt r13
	extu.b r13,r13
	shlr r13
	shlr8 r0
	or r0,r13

	



	
	jmp @r12
	add #-15,r7




	
	
	
ed_6a:

	
	
	
	mov.l @(_z80_HL - _z80_BC,r5),r0          ! first operand
	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8

	mov.l @(_z80_HL - _z80_BC,r5),r1     ! second operand
!vbt inutile pour big endian        
	!shll16 r0             ! prepare first operand
	shll r8                 !  H = 0
	mov r0,r13      ! save 1st operand to get C
!vbt inutile pour big endian        
	!shll16 TMP_REG        ! prepare second operand
	shll16 r11        ! prepare C flag

	! V flag calculation
	addv r1,r0
	movt r2         ! partial overflow flag
	addv r11,r0
	addc r8,r8            ! partial overflow flag
	or r2,r8        ! set V in

	! Carry flag calculation
	clrt
	mov r13,r0      ! reload first operand
	addc r1,r0
	movt r2         ! DIRT_REG = partial carry flag
	clrt
	addc r11,r0         ! r0 = result
	movt r11
	or r2,r11  ! C flag

	! TODO: emulate undoc
	

	! Write result
	shlr16 r0             ! get result in place
	mov.w r0,@(_z80_HL - _z80_BC,r5)

	! Get signed result in r0
	tst r0,r0
	movt r13
	dt r13
	extu.b r13,r13
	shlr r13
	shlr8 r0
	or r0,r13

	



	
	jmp @r12
	add #-15,r7




	
	
	
ed_7a:

	
	
	
	mov.l @(_z80_HL - _z80_BC,r5),r0          ! first operand
	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8

	mov.l @(_z80_SP - _z80_BC,r5),r1     ! second operand
!vbt inutile pour big endian        
	!shll16 r0             ! prepare first operand
	shll r8                 !  H = 0
	mov r0,r13      ! save 1st operand to get C
!vbt inutile pour big endian        
	!shll16 TMP_REG        ! prepare second operand
	shll16 r11        ! prepare C flag

	! V flag calculation
	addv r1,r0
	movt r2         ! partial overflow flag
	addv r11,r0
	addc r8,r8            ! partial overflow flag
	or r2,r8        ! set V in

	! Carry flag calculation
	clrt
	mov r13,r0      ! reload first operand
	addc r1,r0
	movt r2         ! DIRT_REG = partial carry flag
	clrt
	addc r11,r0         ! r0 = result
	movt r11
	or r2,r11  ! C flag

	! TODO: emulate undoc
	

	! Write result
	shlr16 r0             ! get result in place
	mov.w r0,@(_z80_HL - _z80_BC,r5)

	! Get signed result in r0
	tst r0,r0
	movt r13
	dt r13
	extu.b r13,r13
	shlr r13
	shlr8 r0
	or r0,r13

	



	
	jmp @r12
	add #-15,r7




	
	
	
ed_42:

	
	
	
	mov.l @(_z80_HL - _z80_BC,r5),r0          ! first operand
	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8

	mov.l @(_z80_BC - _z80_BC,r5),r1     ! second operand
!vbt inutile pour big endian        
	!shll16 r0             ! prepare first operand
	shll r8                 !  H = 0
	mov r0,r13      ! save 1st operand to get C
!vbt inutile pour big endian        
	!shll16 TMP_REG        ! prepare second operand
	shll16 r11        ! prepare C flag

	! V flag calculation
	subv r1,r0
	movt r2         ! partial overflow flag
	subv r11,r0
	addc r8,r8            ! partial overflow flag
	or r2,r8        ! set V in

	! Carry flag calculation
	clrt
	mov r13,r0      ! reload first operand
	subc r1,r0
	movt r2         ! DIRT_REG = partial carry flag
	clrt
	subc r11,r0         ! r0 = result
	movt r11
	or r2,r11  ! C flag

	! TODO: emulate undoc
	

	! Write result
	shlr16 r0             ! get result in place
	mov.w r0,@(_z80_HL - _z80_BC,r5)

	! Get signed result in r0
	tst r0,r0
	movt r13
	dt r13
	extu.b r13,r13
	shlr r13
	shlr8 r0
	or r0,r13

	



	
	jmp @r12
	add #-15,r7




	
	
	
ed_52:

	
	
	
	mov.l @(_z80_HL - _z80_BC,r5),r0          ! first operand
	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8

	mov.l @(_z80_DE - _z80_BC,r5),r1     ! second operand
!vbt inutile pour big endian        
	!shll16 r0             ! prepare first operand
	shll r8                 !  H = 0
	mov r0,r13      ! save 1st operand to get C
!vbt inutile pour big endian        
	!shll16 TMP_REG        ! prepare second operand
	shll16 r11        ! prepare C flag

	! V flag calculation
	subv r1,r0
	movt r2         ! partial overflow flag
	subv r11,r0
	addc r8,r8            ! partial overflow flag
	or r2,r8        ! set V in

	! Carry flag calculation
	clrt
	mov r13,r0      ! reload first operand
	subc r1,r0
	movt r2         ! DIRT_REG = partial carry flag
	clrt
	subc r11,r0         ! r0 = result
	movt r11
	or r2,r11  ! C flag

	! TODO: emulate undoc
	

	! Write result
	shlr16 r0             ! get result in place
	mov.w r0,@(_z80_HL - _z80_BC,r5)

	! Get signed result in r0
	tst r0,r0
	movt r13
	dt r13
	extu.b r13,r13
	shlr r13
	shlr8 r0
	or r0,r13

	



	
	jmp @r12
	add #-15,r7




	
	
	
ed_62:

	
	
	
	mov.l @(_z80_HL - _z80_BC,r5),r0          ! first operand
	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8

	mov.l @(_z80_HL - _z80_BC,r5),r1     ! second operand
!vbt inutile pour big endian        
	!shll16 r0             ! prepare first operand
	shll r8                 !  H = 0
	mov r0,r13      ! save 1st operand to get C
!vbt inutile pour big endian        
	!shll16 TMP_REG        ! prepare second operand
	shll16 r11        ! prepare C flag

	! V flag calculation
	subv r1,r0
	movt r2         ! partial overflow flag
	subv r11,r0
	addc r8,r8            ! partial overflow flag
	or r2,r8        ! set V in

	! Carry flag calculation
	clrt
	mov r13,r0      ! reload first operand
	subc r1,r0
	movt r2         ! DIRT_REG = partial carry flag
	clrt
	subc r11,r0         ! r0 = result
	movt r11
	or r2,r11  ! C flag

	! TODO: emulate undoc
	

	! Write result
	shlr16 r0             ! get result in place
	mov.w r0,@(_z80_HL - _z80_BC,r5)

	! Get signed result in r0
	tst r0,r0
	movt r13
	dt r13
	extu.b r13,r13
	shlr r13
	shlr8 r0
	or r0,r13

	



	
	jmp @r12
	add #-15,r7




	
	
	
ed_72:

	
	
	
	mov.l @(_z80_HL - _z80_BC,r5),r0          ! first operand
	
	! Set N flag according to operation performed
!	ifelse(op,`add',`mov #FLG_iV,zF',`mov #(FLG_iV | FLG_N),zF')
	mov #3,r8

	mov.l @(_z80_SP - _z80_BC,r5),r1     ! second operand
!vbt inutile pour big endian        
	!shll16 r0             ! prepare first operand
	shll r8                 !  H = 0
	mov r0,r13      ! save 1st operand to get C
!vbt inutile pour big endian        
	!shll16 TMP_REG        ! prepare second operand
	shll16 r11        ! prepare C flag

	! V flag calculation
	subv r1,r0
	movt r2         ! partial overflow flag
	subv r11,r0
	addc r8,r8            ! partial overflow flag
	or r2,r8        ! set V in

	! Carry flag calculation
	clrt
	mov r13,r0      ! reload first operand
	subc r1,r0
	movt r2         ! DIRT_REG = partial carry flag
	clrt
	subc r11,r0         ! r0 = result
	movt r11
	or r2,r11  ! C flag

	! TODO: emulate undoc
	

	! Write result
	shlr16 r0             ! get result in place
	mov.w r0,@(_z80_HL - _z80_BC,r5)

	! Get signed result in r0
	tst r0,r0
	movt r13
	dt r13
	extu.b r13,r13
	shlr r13
	shlr8 r0
	or r0,r13

	



	
	jmp @r12
	add #-15,r7




	
	
	
op_03:

	
	mov.w @(_z80_BC - _z80_BC,r5),r0
	add #1,r0
	mov.w r0,@(_z80_BC - _z80_BC,r5)

	
	jmp @r12
	add #-6,r7




	
	
	
op_13:

	
	mov.w @(_z80_DE - _z80_BC,r5),r0
	add #1,r0
	mov.w r0,@(_z80_DE - _z80_BC,r5)

	
	jmp @r12
	add #-6,r7




	
	
	
op_23:

	
	mov.w @(_z80_HL - _z80_BC,r5),r0
	add #1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)

	
	jmp @r12
	add #-6,r7




	
	
	
op_33:

	
	mov.w @(_z80_SP - _z80_BC,r5),r0
	add #1,r0
	mov.w r0,@(_z80_SP - _z80_BC,r5)

	
	jmp @r12
	add #-6,r7




	
	
	
dd_23:

   
	mov.w @(_z80_IX - _z80_BC,r5),r0
	add #1,r0
	mov.w r0,@(_z80_IX - _z80_BC,r5)

	
	jmp @r12
	add #-10,r7




	
	
	
fd_23:

   
	mov.w @(_z80_IY - _z80_BC,r5),r0
	add #1,r0
	mov.w r0,@(_z80_IY - _z80_BC,r5)

	
	jmp @r12
	add #-10,r7




	
	
	
op_0b:

	
	mov.w @(_z80_BC - _z80_BC,r5),r0
	add #-1,r0
	mov.w r0,@(_z80_BC - _z80_BC,r5)

	
	jmp @r12
	add #-6,r7




	
	
	
op_1b:

	
	mov.w @(_z80_DE - _z80_BC,r5),r0
	add #-1,r0
	mov.w r0,@(_z80_DE - _z80_BC,r5)

	
	jmp @r12
	add #-6,r7




	
	
	
op_2b:

	
	mov.w @(_z80_HL - _z80_BC,r5),r0
	add #-1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)

	
	jmp @r12
	add #-6,r7




	
	
	
op_3b:

	
	mov.w @(_z80_SP - _z80_BC,r5),r0
	add #-1,r0
	mov.w r0,@(_z80_SP - _z80_BC,r5)

	
	jmp @r12
	add #-6,r7




	
	
	
dd_2b:

   
	mov.w @(_z80_IX - _z80_BC,r5),r0
	add #-1,r0
	mov.w r0,@(_z80_IX - _z80_BC,r5)

	
	jmp @r12
	add #-10,r7




	
	
	
fd_2b:

   
	mov.w @(_z80_IY - _z80_BC,r5),r0
	add #-1,r0
	mov.w r0,@(_z80_IY - _z80_BC,r5)

	
	jmp @r12
	add #-10,r7




! **************************************************************************
! rotate/shift


	
	
	
op_07:

	
	
	exts.b r3,r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	
		mov #(0x08 | 0x01),r2    ! isolate P flag
		and r2,r8
		rotl r0
		movt r11
		and #0xFE,r0
		or r11,r0     ! bit0 = bit7
		
	

	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	

	
	mov r0,r3


	
	jmp @r12
	add #-4,r7




	
	
	
op_0f:

	
	
	exts.b r3,r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	
		mov #(0x08 | 0x01),r2    ! isolate P flag
		and r2,r8
		rotr r0
		movt r11
		and #0x7F,r0      ! clear most significant bit
		mov r11,r1
		shll8 r1
		shlr r1
		or r1,r0
		
	

	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	

	
	mov r0,r3


	
	jmp @r12
	add #-4,r7




	
	
	
op_17:

	
	
	exts.b r3,r0

	
	! Rotate Left Arithmetic
	
		mov #(0x08 | 0x01),r2    ! isolate P flag
		cmp/pl r11    ! C -> T
		and r2,r8
		rotcl r0
		movt r11      ! get new carry flag
		
	

	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	

	
	mov r0,r3


	
	jmp @r12
	add #-4,r7




	
	
	
op_1f:

	
	
	exts.b r3,r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	
		mov #(0x08 | 0x01),r2    ! isolate P flag
		cmp/pl r11    ! C -> T
		and r2,r8
		movt r1
		shlr r0
		movt r11      ! get new carry
		and #0x7F,r0      ! clear most significant bit
		shll8 r1
		shlr r1
		or r1,r0     ! merge old carry in the result
		
	

	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	

	
	mov r0,r3


	
	jmp @r12
	add #-4,r7




	
	
	
cb_00:

	
	
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	

	
	mov.b r0,@(_z80_BC - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_01:

	
	
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	

	
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_02:

	
	
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	

	
	mov.b r0,@(_z80_DE - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_03:

	
	
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	

	
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_04:

	
	
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	

	
	mov.b r0,@(_z80_HL - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_05:

	
	
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	

	
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_07:

	
	
	exts.b r3,r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	

	
	mov r0,r3


	
	jmp @r12
	add #-8,r7




	
	
	
cb_06:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_00:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_BC - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_01:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_02:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_DE - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_03:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_04:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_HL - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_05:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_06:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_07:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	
		rotl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	


	! Write result in a register
	mov r0,r3

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
cb_08:

	
	
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	

	
	mov.b r0,@(_z80_BC - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_09:

	
	
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	

	
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_0a:

	
	
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	

	
	mov.b r0,@(_z80_DE - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_0b:

	
	
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	

	
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_0c:

	
	
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	

	
	mov.b r0,@(_z80_HL - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_0d:

	
	
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	

	
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_0f:

	
	
	exts.b r3,r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	

	
	mov r0,r3


	
	jmp @r12
	add #-8,r7




	
	
	
cb_0e:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_08:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_BC - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_09:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_0a:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_DE - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_0b:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_0c:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_HL - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_0d:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_0e:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_0f:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	
		shlr r0
		movt r1       ! TMP_REG = C
		exts.b r0,r0

		
			mov r1,r11   ! set carry
			shll8 r1
			shlr r1
		

		and #0x7F,r0
		mov #0,r8    ! N,H,V = 0
		or r1,r0
		
	! Set Z
	mov r0,r13

	

	! Rotate Left
	

	! Rotate Right
	


	! Write result in a register
	mov r0,r3

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
cb_10:

	
	
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	

	
	mov.b r0,@(_z80_BC - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_11:

	
	
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	

	
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_12:

	
	
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	

	
	mov.b r0,@(_z80_DE - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_13:

	
	
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	

	
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_14:

	
	
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	

	
	mov.b r0,@(_z80_HL - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_15:

	
	
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	

	
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_17:

	
	
	exts.b r3,r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	

	
	mov r0,r3


	
	jmp @r12
	add #-8,r7




	
	
	
cb_16:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_10:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_BC - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_11:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_12:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_DE - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_13:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_14:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_HL - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_15:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	


	! Write result in a register
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_16:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_17:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	
		cmp/pl r11 ! C -> T
		rotcl r0
		movt r11
		mov #0,r8    ! N,H,V = 0
		
	! Set Z
	mov r0,r13

	

	! Rotate Right
	


	! Write result in a register
	mov r0,r3

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
cb_18:

	
	
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	

	
	mov.b r0,@(_z80_BC - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_19:

	
	
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	

	
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_1a:

	
	
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	

	
	mov.b r0,@(_z80_DE - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_1b:

	
	
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	

	
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_1c:

	
	
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	

	
	mov.b r0,@(_z80_HL - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_1d:

	
	
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	

	
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_1f:

	
	
	exts.b r3,r0

	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	

	
	mov r0,r3


	
	jmp @r12
	add #-8,r7




	
	
	
cb_1e:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_18:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	


	! Write result in a register
	mov.b r0,@(_z80_BC - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_19:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	


	! Write result in a register
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_1a:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	


	! Write result in a register
	mov.b r0,@(_z80_DE - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_1b:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	


	! Write result in a register
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_1c:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	


	! Write result in a register
	mov.b r0,@(_z80_HL - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_1d:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	


	! Write result in a register
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_1e:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_1f:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	! Rotate Left Arithmetic
	

	! Rotate Right Arithmetic
	

	! Rotate Left to Carry Arithmetic
	

	! Rotate Right to Carry Arithmetic
	

	! Rotate Left to Carry
	

	! Rotate Right to Carry
	

	! Rotate Left
	

	! Rotate Right
	
		mov #0,r8     ! N,H,V = 0
		mov r11,r1  ! get old carry
		rotr r0
		movt r11  ! set carry
		shll8 r1
		and #0x7F,r0
		shlr r1
		or r1,r0     ! put old carry in result
		
	! Set Z
	mov r0,r13

	


	! Write result in a register
	mov r0,r3

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	

	
	
	jmp @r12
	add #-23,r7




	
	
	
cb_20:

	
	
	
	

	


	
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_21:

	
	
	
	

	


	
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_22:

	
	
	
	

	


	
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_23:

	
	
	
	

	


	
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_24:

	
	
	
	

	


	
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_25:

	
	
	
	

	


	
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_27:

	
	
	
	

	


	
	exts.b r3,r0

	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov r0,r3


	
	jmp @r12
	add #-8,r7




	
	
	
cb_26:

	
	
	
	

	

!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	exts.b r1,r0
	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_20:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_21:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_22:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_23:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_24:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_25:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_26:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_27:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	shal r0
	mov #0,r8
	movt r11   ! C
	
!	exts.b r0,r0

	
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov r0,r3

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
cb_28:

	
	
	
	

	


	
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_29:

	
	
	
	

	


	
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_2a:

	
	
	
	

	


	
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_2b:

	
	
	
	

	


	
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_2c:

	
	
	
	

	


	
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_2d:

	
	
	
	

	


	
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_2f:

	
	
	
	

	


	
	exts.b r3,r0

	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov r0,r3


	
	jmp @r12
	add #-8,r7




	
	
	
cb_2e:

	
	
	
	

	

!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	exts.b r1,r0
	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_28:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_29:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_2a:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_2b:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_2c:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_2d:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_2e:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_2f:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
		shar r0
		mov #0,r8
		movt r11
	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov r0,r3

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
cb_30:

	
	
	
	

	


	
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_31:

	
	
	
	

	


	
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_32:

	
	
	
	

	


	
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_33:

	
	
	
	

	


	
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_34:

	
	
	
	

	


	
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_35:

	
	
	
	

	


	
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_37:

	
	
	
	

	


	
	exts.b r3,r0

	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov r0,r3


	
	jmp @r12
	add #-8,r7




	
	
	
cb_36:

	
	
	
	

	

!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	exts.b r1,r0
	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_30:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_31:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_32:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_33:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_34:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_35:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_36:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_37:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	shll r0
	mov #0,r8
	movt r11   ! C
	add #1,r0
!	exts.b r0,r0

	
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov r0,r3

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
cb_38:

	
	
	
	

	


	
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_39:

	
	
	
	

	


	
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_3a:

	
	
	
	

	


	
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_3b:

	
	
	
	

	


	
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_3c:

	
	
	
	

	


	
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_3d:

	
	
	
	

	


	
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)


	
	jmp @r12
	add #-8,r7




	
	
	
cb_3f:

	
	
	
	

	


	
	exts.b r3,r0

	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov r0,r3


	
	jmp @r12
	add #-8,r7




	
	
	
cb_3e:

	
	
	
	

	

!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	exts.b r1,r0
	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_38:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_39:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_BC - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_3a:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_3b:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_DE - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_3c:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_3d:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov.b r0,@(_z80_HL - _z80_BC + 1,r5)

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_3e:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_3f:

	
	
	
	

	


	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.l r0,@-r15
	

	exts.b r1,r0
	
	
	
	
	
		extu.b r0,r0    ! requires to be not sign-extended
		mov #0,r8
		shlr r0
		movt r11
	
	
	! Banderas Z,S y P
	
	! Set Z
	mov r0,r13

	! Bits 3-5 SR
	
	


	
	mov r0,r3

	mov r0,r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r15+,r0
	


	
	jmp @r12
	add #-23,r7




	
	
	
ed_67:

	
	
	
	

	

!vbtvbt	
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	mov #0x01,r2
	mov #0x0f,r0               ! mask for A register value
	and r2,r8          ! keep old carry flag
	and r3,r0                     ! r0 = low order bits of A register
	extu.b r1,r1
	mov #0x0f,r2   ! mask for low order bits of memory operand
	and r1,r2 ! DIRT_REG = low order bits of memory operand
	shll2 r0
	shlr2 r1
	shll2 r0
	shlr2 r1
	or r0,r1        ! merge in low order bits of A register
	mov #0xf0,r0
	and r0,r3                ! isolate high order bits of A register
	or r2,r3       ! merge in high order bits of memory operand
	
	! Set Z
	mov r3,r13

	
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	
	jmp @r12
	add #-18,r7




	
	
	
ed_6f:

	
	
	
	

	

!vbtvbt	
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	mov #0,r8
	mov #0x0f,r0               ! mask for A register value
	and r3,r0                     ! r0 = low order bits of A register
	mov #0xf0,r2   ! mask for high order bits of memory operand
	extu.b r2,r2
	and r1,r2 ! DIRT_REG = high order bits of memory operand
	shlr2 r2
	shll2 r1
	shlr2 r2
	shll2 r1
	or r0,r1        ! merge in low order bits of A register
	mov #0xf0,r0
	and r0,r3                ! isolate high order bits of A register
	or r2,r3       ! merge in high order bits of memory operand
	
	! Set Z
	mov r3,r13

	
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	
	jmp @r12
	add #-18,r7




! **************************************************************************
! BIT group


	
	
	
cb_40:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_262
	mov #(0x08 | 0x02),r8

	
		bra finish_262
		mov #1,r13
	

bit_is_clear_262:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_262:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_41:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_263
	mov #(0x08 | 0x02),r8

	
		bra finish_263
		mov #1,r13
	

bit_is_clear_263:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_263:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_42:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_264
	mov #(0x08 | 0x02),r8

	
		bra finish_264
		mov #1,r13
	

bit_is_clear_264:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_264:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_43:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_265
	mov #(0x08 | 0x02),r8

	
		bra finish_265
		mov #1,r13
	

bit_is_clear_265:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_265:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_44:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_266
	mov #(0x08 | 0x02),r8

	
		bra finish_266
		mov #1,r13
	

bit_is_clear_266:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_266:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_45:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_267
	mov #(0x08 | 0x02),r8

	
		bra finish_267
		mov #1,r13
	

bit_is_clear_267:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_267:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_47:

	
	
	! Load operand to r0
	mov r3,r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_268
	mov #(0x08 | 0x02),r8

	
		bra finish_268
		mov #1,r13
	

bit_is_clear_268:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_268:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_46:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_269
	mov #(0x08 | 0x02),r8

	
		bra finish_269
		mov #1,r13
	

bit_is_clear_269:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_269:

	
	



	
	jmp @r12
	add #-12,r7




	
	
	
ddcb_40:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_270
	mov #(0x08 | 0x02),r8

	
		bra finish_270
		mov #1,r13
	

bit_is_clear_270:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_270:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_41:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_271
	mov #(0x08 | 0x02),r8

	
		bra finish_271
		mov #1,r13
	

bit_is_clear_271:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_271:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_42:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_272
	mov #(0x08 | 0x02),r8

	
		bra finish_272
		mov #1,r13
	

bit_is_clear_272:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_272:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_43:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_273
	mov #(0x08 | 0x02),r8

	
		bra finish_273
		mov #1,r13
	

bit_is_clear_273:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_273:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_44:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_274
	mov #(0x08 | 0x02),r8

	
		bra finish_274
		mov #1,r13
	

bit_is_clear_274:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_274:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_45:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_275
	mov #(0x08 | 0x02),r8

	
		bra finish_275
		mov #1,r13
	

bit_is_clear_275:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_275:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_46:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_276
	mov #(0x08 | 0x02),r8

	
		bra finish_276
		mov #1,r13
	

bit_is_clear_276:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_276:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_47:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 0),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_277
	mov #(0x08 | 0x02),r8

	
		bra finish_277
		mov #1,r13
	

bit_is_clear_277:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_277:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
cb_48:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_278
	mov #(0x08 | 0x02),r8

	
		bra finish_278
		mov #1,r13
	

bit_is_clear_278:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_278:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_49:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_279
	mov #(0x08 | 0x02),r8

	
		bra finish_279
		mov #1,r13
	

bit_is_clear_279:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_279:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_4a:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_280
	mov #(0x08 | 0x02),r8

	
		bra finish_280
		mov #1,r13
	

bit_is_clear_280:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_280:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_4b:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_281
	mov #(0x08 | 0x02),r8

	
		bra finish_281
		mov #1,r13
	

bit_is_clear_281:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_281:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_4c:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_282
	mov #(0x08 | 0x02),r8

	
		bra finish_282
		mov #1,r13
	

bit_is_clear_282:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_282:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_4d:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_283
	mov #(0x08 | 0x02),r8

	
		bra finish_283
		mov #1,r13
	

bit_is_clear_283:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_283:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_4f:

	
	
	! Load operand to r0
	mov r3,r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_284
	mov #(0x08 | 0x02),r8

	
		bra finish_284
		mov #1,r13
	

bit_is_clear_284:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_284:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_4e:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_285
	mov #(0x08 | 0x02),r8

	
		bra finish_285
		mov #1,r13
	

bit_is_clear_285:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_285:

	
	



	
	jmp @r12
	add #-12,r7




	
	
	
ddcb_48:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_286
	mov #(0x08 | 0x02),r8

	
		bra finish_286
		mov #1,r13
	

bit_is_clear_286:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_286:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_49:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_287
	mov #(0x08 | 0x02),r8

	
		bra finish_287
		mov #1,r13
	

bit_is_clear_287:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_287:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_4a:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_288
	mov #(0x08 | 0x02),r8

	
		bra finish_288
		mov #1,r13
	

bit_is_clear_288:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_288:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_4b:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_289
	mov #(0x08 | 0x02),r8

	
		bra finish_289
		mov #1,r13
	

bit_is_clear_289:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_289:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_4c:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_290
	mov #(0x08 | 0x02),r8

	
		bra finish_290
		mov #1,r13
	

bit_is_clear_290:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_290:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_4d:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_291
	mov #(0x08 | 0x02),r8

	
		bra finish_291
		mov #1,r13
	

bit_is_clear_291:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_291:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_4e:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_292
	mov #(0x08 | 0x02),r8

	
		bra finish_292
		mov #1,r13
	

bit_is_clear_292:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_292:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_4f:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 1),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_293
	mov #(0x08 | 0x02),r8

	
		bra finish_293
		mov #1,r13
	

bit_is_clear_293:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_293:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
cb_50:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_294
	mov #(0x08 | 0x02),r8

	
		bra finish_294
		mov #1,r13
	

bit_is_clear_294:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_294:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_51:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_295
	mov #(0x08 | 0x02),r8

	
		bra finish_295
		mov #1,r13
	

bit_is_clear_295:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_295:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_52:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_296
	mov #(0x08 | 0x02),r8

	
		bra finish_296
		mov #1,r13
	

bit_is_clear_296:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_296:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_53:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_297
	mov #(0x08 | 0x02),r8

	
		bra finish_297
		mov #1,r13
	

bit_is_clear_297:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_297:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_54:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_298
	mov #(0x08 | 0x02),r8

	
		bra finish_298
		mov #1,r13
	

bit_is_clear_298:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_298:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_55:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_299
	mov #(0x08 | 0x02),r8

	
		bra finish_299
		mov #1,r13
	

bit_is_clear_299:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_299:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_57:

	
	
	! Load operand to r0
	mov r3,r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_300
	mov #(0x08 | 0x02),r8

	
		bra finish_300
		mov #1,r13
	

bit_is_clear_300:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_300:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_56:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_301
	mov #(0x08 | 0x02),r8

	
		bra finish_301
		mov #1,r13
	

bit_is_clear_301:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_301:

	
	



	
	jmp @r12
	add #-12,r7




	
	
	
ddcb_50:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_302
	mov #(0x08 | 0x02),r8

	
		bra finish_302
		mov #1,r13
	

bit_is_clear_302:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_302:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_51:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_303
	mov #(0x08 | 0x02),r8

	
		bra finish_303
		mov #1,r13
	

bit_is_clear_303:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_303:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_52:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_304
	mov #(0x08 | 0x02),r8

	
		bra finish_304
		mov #1,r13
	

bit_is_clear_304:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_304:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_53:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_305
	mov #(0x08 | 0x02),r8

	
		bra finish_305
		mov #1,r13
	

bit_is_clear_305:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_305:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_54:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_306
	mov #(0x08 | 0x02),r8

	
		bra finish_306
		mov #1,r13
	

bit_is_clear_306:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_306:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_55:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_307
	mov #(0x08 | 0x02),r8

	
		bra finish_307
		mov #1,r13
	

bit_is_clear_307:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_307:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_56:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_308
	mov #(0x08 | 0x02),r8

	
		bra finish_308
		mov #1,r13
	

bit_is_clear_308:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_308:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_57:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 2),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_309
	mov #(0x08 | 0x02),r8

	
		bra finish_309
		mov #1,r13
	

bit_is_clear_309:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_309:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
cb_58:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_310
	mov #(0x08 | 0x02),r8

	
		bra finish_310
		mov #1,r13
	

bit_is_clear_310:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_310:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_59:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_311
	mov #(0x08 | 0x02),r8

	
		bra finish_311
		mov #1,r13
	

bit_is_clear_311:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_311:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_5a:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_312
	mov #(0x08 | 0x02),r8

	
		bra finish_312
		mov #1,r13
	

bit_is_clear_312:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_312:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_5b:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_313
	mov #(0x08 | 0x02),r8

	
		bra finish_313
		mov #1,r13
	

bit_is_clear_313:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_313:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_5c:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_314
	mov #(0x08 | 0x02),r8

	
		bra finish_314
		mov #1,r13
	

bit_is_clear_314:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_314:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_5d:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_315
	mov #(0x08 | 0x02),r8

	
		bra finish_315
		mov #1,r13
	

bit_is_clear_315:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_315:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_5f:

	
	
	! Load operand to r0
	mov r3,r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_316
	mov #(0x08 | 0x02),r8

	
		bra finish_316
		mov #1,r13
	

bit_is_clear_316:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_316:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_5e:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_317
	mov #(0x08 | 0x02),r8

	
		bra finish_317
		mov #1,r13
	

bit_is_clear_317:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_317:

	
	



	
	jmp @r12
	add #-12,r7




	
	
	
ddcb_58:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_318
	mov #(0x08 | 0x02),r8

	
		bra finish_318
		mov #1,r13
	

bit_is_clear_318:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_318:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_59:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_319
	mov #(0x08 | 0x02),r8

	
		bra finish_319
		mov #1,r13
	

bit_is_clear_319:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_319:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_5a:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_320
	mov #(0x08 | 0x02),r8

	
		bra finish_320
		mov #1,r13
	

bit_is_clear_320:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_320:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_5b:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_321
	mov #(0x08 | 0x02),r8

	
		bra finish_321
		mov #1,r13
	

bit_is_clear_321:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_321:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_5c:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_322
	mov #(0x08 | 0x02),r8

	
		bra finish_322
		mov #1,r13
	

bit_is_clear_322:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_322:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_5d:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_323
	mov #(0x08 | 0x02),r8

	
		bra finish_323
		mov #1,r13
	

bit_is_clear_323:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_323:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_5e:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_324
	mov #(0x08 | 0x02),r8

	
		bra finish_324
		mov #1,r13
	

bit_is_clear_324:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_324:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_5f:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 3),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_325
	mov #(0x08 | 0x02),r8

	
		bra finish_325
		mov #1,r13
	

bit_is_clear_325:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_325:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
cb_60:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_326
	mov #(0x08 | 0x02),r8

	
		bra finish_326
		mov #1,r13
	

bit_is_clear_326:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_326:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_61:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_327
	mov #(0x08 | 0x02),r8

	
		bra finish_327
		mov #1,r13
	

bit_is_clear_327:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_327:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_62:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_328
	mov #(0x08 | 0x02),r8

	
		bra finish_328
		mov #1,r13
	

bit_is_clear_328:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_328:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_63:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_329
	mov #(0x08 | 0x02),r8

	
		bra finish_329
		mov #1,r13
	

bit_is_clear_329:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_329:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_64:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_330
	mov #(0x08 | 0x02),r8

	
		bra finish_330
		mov #1,r13
	

bit_is_clear_330:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_330:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_65:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_331
	mov #(0x08 | 0x02),r8

	
		bra finish_331
		mov #1,r13
	

bit_is_clear_331:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_331:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_67:

	
	
	! Load operand to r0
	mov r3,r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_332
	mov #(0x08 | 0x02),r8

	
		bra finish_332
		mov #1,r13
	

bit_is_clear_332:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_332:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_66:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_333
	mov #(0x08 | 0x02),r8

	
		bra finish_333
		mov #1,r13
	

bit_is_clear_333:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_333:

	
	



	
	jmp @r12
	add #-12,r7




	
	
	
ddcb_60:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_334
	mov #(0x08 | 0x02),r8

	
		bra finish_334
		mov #1,r13
	

bit_is_clear_334:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_334:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_61:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_335
	mov #(0x08 | 0x02),r8

	
		bra finish_335
		mov #1,r13
	

bit_is_clear_335:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_335:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_62:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_336
	mov #(0x08 | 0x02),r8

	
		bra finish_336
		mov #1,r13
	

bit_is_clear_336:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_336:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_63:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_337
	mov #(0x08 | 0x02),r8

	
		bra finish_337
		mov #1,r13
	

bit_is_clear_337:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_337:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_64:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_338
	mov #(0x08 | 0x02),r8

	
		bra finish_338
		mov #1,r13
	

bit_is_clear_338:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_338:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_65:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_339
	mov #(0x08 | 0x02),r8

	
		bra finish_339
		mov #1,r13
	

bit_is_clear_339:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_339:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_66:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_340
	mov #(0x08 | 0x02),r8

	
		bra finish_340
		mov #1,r13
	

bit_is_clear_340:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_340:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_67:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 4),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_341
	mov #(0x08 | 0x02),r8

	
		bra finish_341
		mov #1,r13
	

bit_is_clear_341:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_341:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
cb_68:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_342
	mov #(0x08 | 0x02),r8

	
		bra finish_342
		mov #1,r13
	

bit_is_clear_342:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_342:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_69:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_343
	mov #(0x08 | 0x02),r8

	
		bra finish_343
		mov #1,r13
	

bit_is_clear_343:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_343:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_6a:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_344
	mov #(0x08 | 0x02),r8

	
		bra finish_344
		mov #1,r13
	

bit_is_clear_344:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_344:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_6b:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_345
	mov #(0x08 | 0x02),r8

	
		bra finish_345
		mov #1,r13
	

bit_is_clear_345:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_345:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_6c:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_346
	mov #(0x08 | 0x02),r8

	
		bra finish_346
		mov #1,r13
	

bit_is_clear_346:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_346:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_6d:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_347
	mov #(0x08 | 0x02),r8

	
		bra finish_347
		mov #1,r13
	

bit_is_clear_347:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_347:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_6f:

	
	
	! Load operand to r0
	mov r3,r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_348
	mov #(0x08 | 0x02),r8

	
		bra finish_348
		mov #1,r13
	

bit_is_clear_348:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_348:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_6e:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_349
	mov #(0x08 | 0x02),r8

	
		bra finish_349
		mov #1,r13
	

bit_is_clear_349:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_349:

	
	



	
	jmp @r12
	add #-12,r7




	
	
	
ddcb_68:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_350
	mov #(0x08 | 0x02),r8

	
		bra finish_350
		mov #1,r13
	

bit_is_clear_350:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_350:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_69:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_351
	mov #(0x08 | 0x02),r8

	
		bra finish_351
		mov #1,r13
	

bit_is_clear_351:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_351:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_6a:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_352
	mov #(0x08 | 0x02),r8

	
		bra finish_352
		mov #1,r13
	

bit_is_clear_352:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_352:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_6b:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_353
	mov #(0x08 | 0x02),r8

	
		bra finish_353
		mov #1,r13
	

bit_is_clear_353:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_353:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_6c:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_354
	mov #(0x08 | 0x02),r8

	
		bra finish_354
		mov #1,r13
	

bit_is_clear_354:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_354:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_6d:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_355
	mov #(0x08 | 0x02),r8

	
		bra finish_355
		mov #1,r13
	

bit_is_clear_355:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_355:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_6e:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_356
	mov #(0x08 | 0x02),r8

	
		bra finish_356
		mov #1,r13
	

bit_is_clear_356:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_356:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_6f:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 5),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_357
	mov #(0x08 | 0x02),r8

	
		bra finish_357
		mov #1,r13
	

bit_is_clear_357:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_357:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
cb_70:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_358
	mov #(0x08 | 0x02),r8

	
		bra finish_358
		mov #1,r13
	

bit_is_clear_358:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_358:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_71:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_359
	mov #(0x08 | 0x02),r8

	
		bra finish_359
		mov #1,r13
	

bit_is_clear_359:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_359:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_72:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_360
	mov #(0x08 | 0x02),r8

	
		bra finish_360
		mov #1,r13
	

bit_is_clear_360:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_360:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_73:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_361
	mov #(0x08 | 0x02),r8

	
		bra finish_361
		mov #1,r13
	

bit_is_clear_361:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_361:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_74:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_362
	mov #(0x08 | 0x02),r8

	
		bra finish_362
		mov #1,r13
	

bit_is_clear_362:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_362:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_75:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_363
	mov #(0x08 | 0x02),r8

	
		bra finish_363
		mov #1,r13
	

bit_is_clear_363:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_363:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_77:

	
	
	! Load operand to r0
	mov r3,r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_364
	mov #(0x08 | 0x02),r8

	
		bra finish_364
		mov #1,r13
	

bit_is_clear_364:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_364:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_76:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_365
	mov #(0x08 | 0x02),r8

	
		bra finish_365
		mov #1,r13
	

bit_is_clear_365:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_365:

	
	



	
	jmp @r12
	add #-12,r7




	
	
	
ddcb_70:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_366
	mov #(0x08 | 0x02),r8

	
		bra finish_366
		mov #1,r13
	

bit_is_clear_366:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_366:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_71:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_367
	mov #(0x08 | 0x02),r8

	
		bra finish_367
		mov #1,r13
	

bit_is_clear_367:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_367:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_72:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_368
	mov #(0x08 | 0x02),r8

	
		bra finish_368
		mov #1,r13
	

bit_is_clear_368:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_368:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_73:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_369
	mov #(0x08 | 0x02),r8

	
		bra finish_369
		mov #1,r13
	

bit_is_clear_369:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_369:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_74:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_370
	mov #(0x08 | 0x02),r8

	
		bra finish_370
		mov #1,r13
	

bit_is_clear_370:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_370:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_75:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_371
	mov #(0x08 | 0x02),r8

	
		bra finish_371
		mov #1,r13
	

bit_is_clear_371:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_371:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_76:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_372
	mov #(0x08 | 0x02),r8

	
		bra finish_372
		mov #1,r13
	

bit_is_clear_372:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_372:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_77:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 6),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_373
	mov #(0x08 | 0x02),r8

	
		bra finish_373
		mov #1,r13
	

bit_is_clear_373:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_373:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
cb_78:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_374
	mov #(0x08 | 0x02),r8

	
		bra finish_374
		mov #-1,r13
	

bit_is_clear_374:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_374:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_79:

	
	
	! Load operand to r0
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_375
	mov #(0x08 | 0x02),r8

	
		bra finish_375
		mov #-1,r13
	

bit_is_clear_375:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_375:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_7a:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_376
	mov #(0x08 | 0x02),r8

	
		bra finish_376
		mov #-1,r13
	

bit_is_clear_376:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_376:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_7b:

	
	
	! Load operand to r0
	mov.b @(_z80_DE - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_377
	mov #(0x08 | 0x02),r8

	
		bra finish_377
		mov #-1,r13
	

bit_is_clear_377:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_377:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_7c:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC,r5),r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_378
	mov #(0x08 | 0x02),r8

	
		bra finish_378
		mov #-1,r13
	

bit_is_clear_378:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_378:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_7d:

	
	
	! Load operand to r0
	mov.b @(_z80_HL - _z80_BC + 1,r5),r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_379
	mov #(0x08 | 0x02),r8

	
		bra finish_379
		mov #-1,r13
	

bit_is_clear_379:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_379:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_7f:

	
	
	! Load operand to r0
	mov r3,r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_380
	mov #(0x08 | 0x02),r8

	
		bra finish_380
		mov #-1,r13
	

bit_is_clear_380:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_380:

	
	



	
	jmp @r12
	add #-8,r7




	
	
	
cb_7e:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_381
	mov #(0x08 | 0x02),r8

	
		bra finish_381
		mov #-1,r13
	

bit_is_clear_381:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_381:

	
	



	
	jmp @r12
	add #-12,r7




	
	
	
ddcb_78:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_382
	mov #(0x08 | 0x02),r8

	
		bra finish_382
		mov #-1,r13
	

bit_is_clear_382:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_382:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_79:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_383
	mov #(0x08 | 0x02),r8

	
		bra finish_383
		mov #-1,r13
	

bit_is_clear_383:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_383:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_7a:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_384
	mov #(0x08 | 0x02),r8

	
		bra finish_384
		mov #-1,r13
	

bit_is_clear_384:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_384:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_7b:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_385
	mov #(0x08 | 0x02),r8

	
		bra finish_385
		mov #-1,r13
	

bit_is_clear_385:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_385:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_7c:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_386
	mov #(0x08 | 0x02),r8

	
		bra finish_386
		mov #-1,r13
	

bit_is_clear_386:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_386:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_7d:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_387
	mov #(0x08 | 0x02),r8

	
		bra finish_387
		mov #-1,r13
	

bit_is_clear_387:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_387:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_7e:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_388
	mov #(0x08 | 0x02),r8

	
		bra finish_388
		mov #-1,r13
	

bit_is_clear_388:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_388:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
ddcb_7f:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	mov r1,r0

	
	
	
	

	


	tst #(1 << 7),r0
	
	! bit 3 and 5 are always cleared
	! (actually they are bizarrely set, but
	! theres no point emulating that, its just
	! plain silly...)

	bt/s bit_is_clear_389
	mov #(0x08 | 0x02),r8

	
		bra finish_389
		mov #-1,r13
	

bit_is_clear_389:
	mov #(0x02 | 0x08 | 0x01),r1
	mov #0,r13                  ! set Z flag
	or r1,r8

finish_389:

	
	



	
	jmp @r12
	add #-20,r7




	
	
	
cb_80:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				and #~(1 << 0),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_81:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				and #~(1 << 0),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_82:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				and #~(1 << 0),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_83:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				and #~(1 << 0),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_84:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				and #~(1 << 0),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_85:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				and #~(1 << 0),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_87:

	
	
	
		
			
				mov #~(1 << 0),r2
				and r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_86:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #~(1 << 0),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_80:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 0),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_81:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 0),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_82:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 0),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_83:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 0),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_84:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 0),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_85:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 0),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_86:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 0),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_87:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 0),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_88:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				and #~(1 << 1),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_89:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				and #~(1 << 1),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_8a:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				and #~(1 << 1),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_8b:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				and #~(1 << 1),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_8c:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				and #~(1 << 1),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_8d:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				and #~(1 << 1),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_8f:

	
	
	
		
			
				mov #~(1 << 1),r2
				and r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_8e:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #~(1 << 1),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_88:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 1),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_89:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 1),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_8a:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 1),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_8b:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 1),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_8c:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 1),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_8d:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 1),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_8e:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 1),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_8f:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 1),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_90:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				and #~(1 << 2),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_91:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				and #~(1 << 2),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_92:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				and #~(1 << 2),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_93:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				and #~(1 << 2),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_94:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				and #~(1 << 2),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_95:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				and #~(1 << 2),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_97:

	
	
	
		
			
				mov #~(1 << 2),r2
				and r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_96:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #~(1 << 2),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_90:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 2),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_91:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 2),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_92:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 2),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_93:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 2),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_94:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 2),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_95:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 2),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_96:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 2),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_97:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 2),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_98:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				and #~(1 << 3),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_99:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				and #~(1 << 3),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_9a:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				and #~(1 << 3),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_9b:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				and #~(1 << 3),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_9c:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				and #~(1 << 3),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_9d:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				and #~(1 << 3),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_9f:

	
	
	
		
			
				mov #~(1 << 3),r2
				and r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_9e:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #~(1 << 3),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_98:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 3),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_99:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 3),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_9a:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 3),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_9b:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 3),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_9c:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 3),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_9d:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 3),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_9e:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 3),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_9f:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 3),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_a0:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				and #~(1 << 4),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_a1:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				and #~(1 << 4),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_a2:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				and #~(1 << 4),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_a3:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				and #~(1 << 4),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_a4:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				and #~(1 << 4),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_a5:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				and #~(1 << 4),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_a7:

	
	
	
		
			
				mov #~(1 << 4),r2
				and r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_a6:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #~(1 << 4),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_a0:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 4),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_a1:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 4),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_a2:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 4),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_a3:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 4),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_a4:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 4),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_a5:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 4),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_a6:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 4),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_a7:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 4),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_a8:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				and #~(1 << 5),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_a9:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				and #~(1 << 5),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_aa:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				and #~(1 << 5),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_ab:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				and #~(1 << 5),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_ac:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				and #~(1 << 5),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_ad:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				and #~(1 << 5),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_af:

	
	
	
		
			
				mov #~(1 << 5),r2
				and r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_ae:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #~(1 << 5),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_a8:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 5),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_a9:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 5),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_aa:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 5),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_ab:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 5),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_ac:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 5),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_ad:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 5),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_ae:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 5),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_af:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 5),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_b0:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				and #~(1 << 6),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_b1:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				and #~(1 << 6),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_b2:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				and #~(1 << 6),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_b3:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				and #~(1 << 6),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_b4:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				and #~(1 << 6),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_b5:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				and #~(1 << 6),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_b7:

	
	
	
		
			
				mov #~(1 << 6),r2
				and r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_b6:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #~(1 << 6),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_b0:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 6),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_b1:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 6),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_b2:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 6),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_b3:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 6),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_b4:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 6),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_b5:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 6),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_b6:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 6),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_b7:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 6),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_b8:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				and #~(1 << 7),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_b9:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				and #~(1 << 7),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_ba:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				and #~(1 << 7),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_bb:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				and #~(1 << 7),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_bc:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				and #~(1 << 7),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_bd:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				and #~(1 << 7),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_bf:

	
	
	
		
			
				mov #~(1 << 7),r2
				and r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_be:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #~(1 << 7),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_b8:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 7),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_b9:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 7),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_ba:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 7),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_bb:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 7),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_bc:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 7),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_bd:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 7),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_be:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 7),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_bf:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #~(1 << 7),r2
			and r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_c0:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				or #(1 << 0),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_c1:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				or #(1 << 0),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_c2:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				or #(1 << 0),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_c3:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				or #(1 << 0),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_c4:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				or #(1 << 0),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_c5:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				or #(1 << 0),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_c7:

	
	
	
		
			
				mov #(1 << 0),r2
				or r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_c6:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #(1 << 0),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_c0:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 0),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_c1:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 0),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_c2:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 0),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_c3:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 0),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_c4:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 0),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_c5:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 0),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_c6:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 0),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_c7:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 0),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_c8:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				or #(1 << 1),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_c9:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				or #(1 << 1),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_ca:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				or #(1 << 1),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_cb:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				or #(1 << 1),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_cc:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				or #(1 << 1),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_cd:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				or #(1 << 1),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_cf:

	
	
	
		
			
				mov #(1 << 1),r2
				or r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_ce:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #(1 << 1),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_c8:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 1),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_c9:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 1),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_ca:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 1),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_cb:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 1),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_cc:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 1),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_cd:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 1),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_ce:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 1),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_cf:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 1),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_d0:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				or #(1 << 2),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_d1:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				or #(1 << 2),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_d2:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				or #(1 << 2),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_d3:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				or #(1 << 2),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_d4:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				or #(1 << 2),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_d5:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				or #(1 << 2),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_d7:

	
	
	
		
			
				mov #(1 << 2),r2
				or r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_d6:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #(1 << 2),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_d0:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 2),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_d1:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 2),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_d2:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 2),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_d3:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 2),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_d4:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 2),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_d5:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 2),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_d6:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 2),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_d7:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 2),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_d8:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				or #(1 << 3),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_d9:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				or #(1 << 3),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_da:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				or #(1 << 3),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_db:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				or #(1 << 3),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_dc:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				or #(1 << 3),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_dd:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				or #(1 << 3),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_df:

	
	
	
		
			
				mov #(1 << 3),r2
				or r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_de:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #(1 << 3),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_d8:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 3),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_d9:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 3),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_da:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 3),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_db:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 3),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_dc:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 3),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_dd:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 3),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_de:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 3),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_df:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 3),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_e0:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				or #(1 << 4),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_e1:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				or #(1 << 4),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_e2:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				or #(1 << 4),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_e3:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				or #(1 << 4),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_e4:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				or #(1 << 4),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_e5:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				or #(1 << 4),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_e7:

	
	
	
		
			
				mov #(1 << 4),r2
				or r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_e6:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #(1 << 4),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_e0:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 4),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_e1:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 4),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_e2:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 4),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_e3:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 4),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_e4:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 4),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_e5:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 4),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_e6:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 4),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_e7:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 4),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_e8:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				or #(1 << 5),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_e9:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				or #(1 << 5),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_ea:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				or #(1 << 5),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_eb:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				or #(1 << 5),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_ec:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				or #(1 << 5),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_ed:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				or #(1 << 5),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_ef:

	
	
	
		
			
				mov #(1 << 5),r2
				or r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_ee:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #(1 << 5),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_e8:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 5),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_e9:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 5),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_ea:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 5),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_eb:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 5),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_ec:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 5),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_ed:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 5),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_ee:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 5),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_ef:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 5),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_f0:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				or #(1 << 6),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_f1:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				or #(1 << 6),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_f2:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				or #(1 << 6),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_f3:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				or #(1 << 6),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_f4:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				or #(1 << 6),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_f5:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				or #(1 << 6),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_f7:

	
	
	
		
			
				mov #(1 << 6),r2
				or r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_f6:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #(1 << 6),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_f0:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 6),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_f1:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 6),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_f2:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 6),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_f3:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 6),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_f4:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 6),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_f5:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 6),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_f6:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 6),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_f7:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 6),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




	
	
	
cb_f8:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC,r5),r0
				or #(1 << 7),r0
				mov.b r0,@(_z80_BC - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_f9:

	
	
	
		
			
				mov.b @(_z80_BC - _z80_BC + 1,r5),r0
				or #(1 << 7),r0
				mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_fa:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC,r5),r0
				or #(1 << 7),r0
				mov.b r0,@(_z80_DE - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_fb:

	
	
	
		
			
				mov.b @(_z80_DE - _z80_BC + 1,r5),r0
				or #(1 << 7),r0
				mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_fc:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC,r5),r0
				or #(1 << 7),r0
				mov.b r0,@(_z80_HL - _z80_BC,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_fd:

	
	
	
		
			
				mov.b @(_z80_HL - _z80_BC + 1,r5),r0
				or #(1 << 7),r0
				mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_ff:

	
	
	
		
			
				mov #(1 << 7),r2
				or r2,r3
			
		
	


	
	jmp @r12
	add #-8,r7




	
	
	
cb_fe:

	
!vbtvbt
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	

	
	
		
			mov #(1 << 7),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-15,r7




	
	
	
ddcb_f8:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 7),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_f9:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 7),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_fa:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 7),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_fb:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 7),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_fc:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 7),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_fd:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 7),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r0
		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)
	

	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_fe:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 7),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	
	jmp @r12
	add #-23,r7




	
	
	
ddcb_ff:

	
	
	
		mov.l @r10,r2
                
		jsr @r2
		nop
	

	
	
		
			mov #(1 << 7),r2
			or r2,r1
		
	

	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	

	
	! Extra store
	
		mov r1,r3
	

	
	jmp @r12
	add #-23,r7




! **************************************************************************
! jump group


	
	
	
op_c3:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll8 r0
	or r2,r0

	extu.w r0,r6



	
	jmp @r12
	add #-10,r7




	
	
	
op_da:

   
	
	
	

	


	
	
		
			
				
					tst r11,r11
				
			
		
	

	bt dont_take_jump390

	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1

	extu.w r1,r6


	
	jmp @r12
	add #-10,r7


	.align 5
dont_take_jump390:
	add #2,r6    ! skip address
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_d2:

   
	
	
	

	


	
	
		
			
				
					tst r11,r11
				
			
		
	

	bf dont_take_jump391

	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1

	extu.w r1,r6


	
	jmp @r12
	add #-10,r7


	.align 5
dont_take_jump391:
	add #2,r6    ! skip address
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_ca:

   
	
	
	

	


	
	
		mov r13,r0
		tst #0xFF,r0
	

	bf dont_take_jump392

	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1

	extu.w r1,r6


	
	jmp @r12
	add #-10,r7


	.align 5
dont_take_jump392:
	add #2,r6    ! skip address
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_c2:

   
	
	
	

	


	
	
		mov r13,r0
		tst #0xFF,r0
	

	bt dont_take_jump393

	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1

	extu.w r1,r6


	
	jmp @r12
	add #-10,r7


	.align 5
dont_take_jump393:
	add #2,r6    ! skip address
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_ea:

   
	
	
	

	


	
	
		
			
				! TODO: P/V flag evaluation
				mov r8,r0
				tst #0x01,r0
			
		
	

	bt dont_take_jump394

	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1

	extu.w r1,r6


	
	jmp @r12
	add #-10,r7


	.align 5
dont_take_jump394:
	add #2,r6    ! skip address
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_e2:

   
	
	
	

	


	
	
		
			
				! TODO: P/V flag evaluation
				mov r8,r0
				tst #0x01,r0
			
		
	

	bf dont_take_jump395

	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1

	extu.w r1,r6


	
	jmp @r12
	add #-10,r7


	.align 5
dont_take_jump395:
	add #2,r6    ! skip address
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_fa:

   
	
	
	

	


	
	
		
			mov r13,r0
			tst #0x80,r0
		
	

	bt dont_take_jump396

	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1

	extu.w r1,r6


	
	jmp @r12
	add #-10,r7


	.align 5
dont_take_jump396:
	add #2,r6    ! skip address
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_f2:

   
	
	
	

	


	
	
		
			mov r13,r0
			tst #0x80,r0
		
	

	bf dont_take_jump397

	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1

	extu.w r1,r6


	
	jmp @r12
	add #-10,r7


	.align 5
dont_take_jump397:
	add #2,r6    ! skip address
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_18:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6


! vbt BUG !!!!! cela saute des instructions obligatoires
        add r0,r6
	
	extu.w r6,r6

	
	jmp @r12
	add #-12,r7




	
	
	
op_38:

   
	
	
	

	


	
	
		
			
				
					tst r11,r11
				
			
		
	

! vbt change for op_28 to test zF and FLAG_ZSP
!	mov FLAGS_ZSP,r0
!	tst zF,r0
	bt dont_take_jump398

	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6


! vbt BUG !!!!! cela saute des instructions obligatoires
        add r0,r6
	
	extu.w r6,r6

	
	jmp @r12
	add #-12,r7



	.align 5
dont_take_jump398:
	add #1,r6        ! skip over the displacement
	extu.w r6,r6
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_30:

   
	
	
	

	


	
	
		
			
				
					tst r11,r11
				
			
		
	

! vbt change for op_28 to test zF and FLAG_ZSP
!	mov FLAGS_ZSP,r0
!	tst zF,r0
	bf dont_take_jump399

	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6


! vbt BUG !!!!! cela saute des instructions obligatoires
        add r0,r6
	
	extu.w r6,r6

	
	jmp @r12
	add #-12,r7



	.align 5
dont_take_jump399:
	add #1,r6        ! skip over the displacement
	extu.w r6,r6
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_28:

   
	
	
	

	


	
	
		mov r13,r0
		tst #0xFF,r0
	

! vbt change for op_28 to test zF and FLAG_ZSP
!	mov FLAGS_ZSP,r0
!	tst zF,r0
	bf dont_take_jump400

	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6


! vbt BUG !!!!! cela saute des instructions obligatoires
        add r0,r6
	
	extu.w r6,r6

	
	jmp @r12
	add #-12,r7



	.align 5
dont_take_jump400:
	add #1,r6        ! skip over the displacement
	extu.w r6,r6
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_20:

   
	
	
	

	


	
	
		mov r13,r0
		tst #0xFF,r0
	

! vbt change for op_28 to test zF and FLAG_ZSP
!	mov FLAGS_ZSP,r0
!	tst zF,r0
	bt dont_take_jump401

	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6


! vbt BUG !!!!! cela saute des instructions obligatoires
        add r0,r6
	
	extu.w r6,r6

	
	jmp @r12
	add #-12,r7



	.align 5
dont_take_jump401:
	add #1,r6        ! skip over the displacement
	extu.w r6,r6
	

	
	jmp @r12
	add #-7,r7




	
	
	
op_e9:

	
	mov.l @(_z80_HL - _z80_BC,r5),r0
	shlr16 r0
	extu.w r0,r6


	
	jmp @r12
	add #-4,r7




	
	
	
dd_e9:

   
	mov.l @(_z80_IX - _z80_BC,r5),r0
	shlr16 r0
	extu.w r0,r6


	
	jmp @r12
	add #-8,r7




	
	
	
fd_e9:

   
	mov.l @(_z80_IY - _z80_BC,r5),r0
	shlr16 r0
	extu.w r0,r6


	
	jmp @r12
	add #-8,r7




	
	
	
op_10:

	
	
	
	

	


	mov.b @(_z80_BC - _z80_BC,r5),r0
	dt r0
	bt/s dont_take_jump402
	mov.b r0,@(_z80_BC - _z80_BC,r5)

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	add r0,r6
	extu.w r6,r6

	
	jmp @r12
	add #-13,r7


	.align 5
dont_take_jump402:
	add #1,r6        ! skip over the displacement
	extu.w r6,r6

	
	jmp @r12
	add #-8,r7




! **************************************************************************
! call and return group


	
	
	
op_cd:

	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1


	! Write the PC
	mov.l @(_z80_SP - _z80_BC,r5),r0
!vbt à tester
        shlr16 r0      
!vbt déplacement du mov car sauvegarde la mauvaise valeur pour op_c0
!vbt modification annullée
!	mov zPC,TMP_REG        
	mov.l r1,@-r15     ! Save the new PC into the stack
	mov r6,r1        
!	mov TMP_REG,zPC

       ! Un-base PC

	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
	mov r6,r1
       ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov.l @r15+,r6
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	extu.w r6,r6          ! Prepare PC
       ! Base PC

	
	jmp @r12
	add #-17,r7




	
	
	
op_c4:

   
	
	
	

	


	
	
		mov r13,r0
		tst #0xFF,r0
	

	bt dont_take_call_403
	
	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1


	! Write the PC
	mov.l @(_z80_SP - _z80_BC,r5),r0
!vbt à tester
        shlr16 r0      
!vbt déplacement du mov car sauvegarde la mauvaise valeur pour op_c0
!vbt modification annullée
!	mov zPC,TMP_REG        
	mov.l r1,@-r15     ! Save the new PC into the stack
	mov r6,r1        
!	mov TMP_REG,zPC

       ! Un-base PC

	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
	mov r6,r1
       ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov.l @r15+,r6
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	extu.w r6,r6          ! Prepare PC
       ! Base PC

	
	jmp @r12
	add #-10,r7



dont_take_call_403:
	add #2,r6
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_cc:

   
	
	
	

	


	
	
		mov r13,r0
		tst #0xFF,r0
	

	bf dont_take_call_404
	
	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1


	! Write the PC
	mov.l @(_z80_SP - _z80_BC,r5),r0
!vbt à tester
        shlr16 r0      
!vbt déplacement du mov car sauvegarde la mauvaise valeur pour op_c0
!vbt modification annullée
!	mov zPC,TMP_REG        
	mov.l r1,@-r15     ! Save the new PC into the stack
	mov r6,r1        
!	mov TMP_REG,zPC

       ! Un-base PC

	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
	mov r6,r1
       ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov.l @r15+,r6
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	extu.w r6,r6          ! Prepare PC
       ! Base PC

	
	jmp @r12
	add #-10,r7



dont_take_call_404:
	add #2,r6
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_d4:

   
	
	
	

	


	
	
		
			
				
					tst r11,r11
				
			
		
	

	bf dont_take_call_405
	
	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1


	! Write the PC
	mov.l @(_z80_SP - _z80_BC,r5),r0
!vbt à tester
        shlr16 r0      
!vbt déplacement du mov car sauvegarde la mauvaise valeur pour op_c0
!vbt modification annullée
!	mov zPC,TMP_REG        
	mov.l r1,@-r15     ! Save the new PC into the stack
	mov r6,r1        
!	mov TMP_REG,zPC

       ! Un-base PC

	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
	mov r6,r1
       ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov.l @r15+,r6
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	extu.w r6,r6          ! Prepare PC
       ! Base PC

	
	jmp @r12
	add #-10,r7



dont_take_call_405:
	add #2,r6
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_dc:

   
	
	
	

	


	
	
		
			
				
					tst r11,r11
				
			
		
	

	bt dont_take_call_406
	
	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1


	! Write the PC
	mov.l @(_z80_SP - _z80_BC,r5),r0
!vbt à tester
        shlr16 r0      
!vbt déplacement du mov car sauvegarde la mauvaise valeur pour op_c0
!vbt modification annullée
!	mov zPC,TMP_REG        
	mov.l r1,@-r15     ! Save the new PC into the stack
	mov r6,r1        
!	mov TMP_REG,zPC

       ! Un-base PC

	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
	mov r6,r1
       ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov.l @r15+,r6
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	extu.w r6,r6          ! Prepare PC
       ! Base PC

	
	jmp @r12
	add #-10,r7



dont_take_call_406:
	add #2,r6
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_e4:

   
	
	
	

	


	
	
		
			
				! TODO: P/V flag evaluation
				mov r8,r0
				tst #0x01,r0
			
		
	

	bf dont_take_call_407
	
	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1


	! Write the PC
	mov.l @(_z80_SP - _z80_BC,r5),r0
!vbt à tester
        shlr16 r0      
!vbt déplacement du mov car sauvegarde la mauvaise valeur pour op_c0
!vbt modification annullée
!	mov zPC,TMP_REG        
	mov.l r1,@-r15     ! Save the new PC into the stack
	mov r6,r1        
!	mov TMP_REG,zPC

       ! Un-base PC

	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
	mov r6,r1
       ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov.l @r15+,r6
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	extu.w r6,r6          ! Prepare PC
       ! Base PC

	
	jmp @r12
	add #-10,r7



dont_take_call_407:
	add #2,r6
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_ec:

   
	
	
	

	


	
	
		
			
				! TODO: P/V flag evaluation
				mov r8,r0
				tst #0x01,r0
			
		
	

	bt dont_take_call_408
	
	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1


	! Write the PC
	mov.l @(_z80_SP - _z80_BC,r5),r0
!vbt à tester
        shlr16 r0      
!vbt déplacement du mov car sauvegarde la mauvaise valeur pour op_c0
!vbt modification annullée
!	mov zPC,TMP_REG        
	mov.l r1,@-r15     ! Save the new PC into the stack
	mov r6,r1        
!	mov TMP_REG,zPC

       ! Un-base PC

	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
	mov r6,r1
       ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov.l @r15+,r6
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	extu.w r6,r6          ! Prepare PC
       ! Base PC

	
	jmp @r12
	add #-10,r7



dont_take_call_408:
	add #2,r6
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_f4:

   
	
	
	

	


	
	
		
			mov r13,r0
			tst #0x80,r0
		
	

	bf dont_take_call_409
	
	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1


	! Write the PC
	mov.l @(_z80_SP - _z80_BC,r5),r0
!vbt à tester
        shlr16 r0      
!vbt déplacement du mov car sauvegarde la mauvaise valeur pour op_c0
!vbt modification annullée
!	mov zPC,TMP_REG        
	mov.l r1,@-r15     ! Save the new PC into the stack
	mov r6,r1        
!	mov TMP_REG,zPC

       ! Un-base PC

	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
	mov r6,r1
       ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov.l @r15+,r6
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	extu.w r6,r6          ! Prepare PC
       ! Base PC

	
	jmp @r12
	add #-10,r7



dont_take_call_409:
	add #2,r6
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_fc:

   
	
	
	

	


	
	
		
			mov r13,r0
			tst #0x80,r0
		
	

	bt dont_take_call_410
	
	
	
	
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	shll8 r1
	or r2,r1


	! Write the PC
	mov.l @(_z80_SP - _z80_BC,r5),r0
!vbt à tester
        shlr16 r0      
!vbt déplacement du mov car sauvegarde la mauvaise valeur pour op_c0
!vbt modification annullée
!	mov zPC,TMP_REG        
	mov.l r1,@-r15     ! Save the new PC into the stack
	mov r6,r1        
!	mov TMP_REG,zPC

       ! Un-base PC

	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
	mov r6,r1
       ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov.l @r15+,r6
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	extu.w r6,r6          ! Prepare PC
       ! Base PC

	
	jmp @r12
	add #-10,r7



dont_take_call_410:
	add #2,r6
	extu.w r6,r6

	
	jmp @r12
	add #-10,r7




	
	
	
op_c9:

	
	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	
	jmp @r12
	add #-10,r7




	
	
	
op_c0:

   
	
	
	

	


	
	
		mov r13,r0
		tst #0xFF,r0
	

	bt dont_take_ret_411

	
	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	
	jmp @r12
	add #-5,r7



dont_take_ret_411:
	
	jmp @r12
	add #-5,r7




	
	
	
op_c8:

   
	
	
	

	


	
	
		mov r13,r0
		tst #0xFF,r0
	

	bf dont_take_ret_412

	
	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	
	jmp @r12
	add #-5,r7



dont_take_ret_412:
	
	jmp @r12
	add #-5,r7




	
	
	
op_d0:

   
	
	
	

	


	
	
		
			
				
					tst r11,r11
				
			
		
	

	bf dont_take_ret_413

	
	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	
	jmp @r12
	add #-5,r7



dont_take_ret_413:
	
	jmp @r12
	add #-5,r7




	
	
	
op_d8:

   
	
	
	

	


	
	
		
			
				
					tst r11,r11
				
			
		
	

	bt dont_take_ret_414

	
	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	
	jmp @r12
	add #-5,r7



dont_take_ret_414:
	
	jmp @r12
	add #-5,r7




	
	
	
op_e0:

   
	
	
	

	


	
	
		
			
				! TODO: P/V flag evaluation
				mov r8,r0
				tst #0x01,r0
			
		
	

	bf dont_take_ret_415

	
	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	
	jmp @r12
	add #-5,r7



dont_take_ret_415:
	
	jmp @r12
	add #-5,r7




	
	
	
op_e8:

   
	
	
	

	


	
	
		
			
				! TODO: P/V flag evaluation
				mov r8,r0
				tst #0x01,r0
			
		
	

	bt dont_take_ret_416

	
	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	
	jmp @r12
	add #-5,r7



dont_take_ret_416:
	
	jmp @r12
	add #-5,r7




	
	
	
op_f0:

   
	
	
	

	


	
	
		
			mov r13,r0
			tst #0x80,r0
		
	

	bf dont_take_ret_417

	
	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	
	jmp @r12
	add #-5,r7



dont_take_ret_417:
	
	jmp @r12
	add #-5,r7




	
	
	
op_f8:

   
	
	
	

	


	
	
		
			mov r13,r0
			tst #0x80,r0
		
	

	bt dont_take_ret_418

	
	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	
	jmp @r12
	add #-5,r7



dont_take_ret_418:
	
	jmp @r12
	add #-5,r7




	
	
	
ed_4d:

	
	
	
	

	


	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	! Restore IFF1 & signal the BUS
	mov.l _z80_IFF2_419,r0
	mov.b @r0,r1        ! TMP_REG = IFF2
	mov.l _z80_RetI_419,r2
	mov.l @r2,r2
	mov.l _z80_IFF1_419,r0
	jsr @r2
	mov.b r1,@r0

	
	jmp @r12
	add #-14,r7

	
.align 2
	_z80_IFF1_419: .long _z80_IFF1
	_z80_IFF2_419: .long _z80_IFF2
	_z80_RetI_419: .long _z80_RetI



	
	
	
ed_45:

	
	
	
	

	


	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	! Restore IFF1 & signal the BUS
	mov.l _z80_IFF2_420,r0

	mov.b @r0,r1
	dt r0                ! IFF1 is immediately before IFF2
	mov.b r1,@r0

	
	jmp @r12
	add #-14,r7

	
.align 2
	_z80_IFF1_420: .long _z80_IFF1
	_z80_IFF2_420: .long _z80_IFF2



	
	
	
ed_5d:

	
	
	
	

	


	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	! Restore IFF1 & signal the BUS
	mov.l _z80_IFF2_421,r0

	mov.b @r0,r1
	dt r0                ! IFF1 is immediately before IFF2
	mov.b r1,@r0

	
	jmp @r12
	add #-14,r7

	
.align 2
	_z80_IFF1_421: .long _z80_IFF1
	_z80_IFF2_421: .long _z80_IFF2



	
	
	
ed_55:

	
	
	
	

	


	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	! Restore IFF1 & signal the BUS
	mov.l _z80_IFF2_422,r0

	mov.b @r0,r1
	dt r0                ! IFF1 is immediately before IFF2
	mov.b r1,@r0

	
	jmp @r12
	add #-14,r7

	
.align 2
	_z80_IFF1_422: .long _z80_IFF1
	_z80_IFF2_422: .long _z80_IFF2



	
	
	
ed_6d:

	
	
	
	

	


	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	! Restore IFF1 & signal the BUS
	mov.l _z80_IFF2_423,r0

	mov.b @r0,r1
	dt r0                ! IFF1 is immediately before IFF2
	mov.b r1,@r0

	
	jmp @r12
	add #-14,r7

	
.align 2
	_z80_IFF1_423: .long _z80_IFF1
	_z80_IFF2_423: .long _z80_IFF2



	
	
	
ed_65:

	
	
	
	

	


	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	! Restore IFF1 & signal the BUS
	mov.l _z80_IFF2_424,r0

	mov.b @r0,r1
	dt r0                ! IFF1 is immediately before IFF2
	mov.b r1,@r0

	
	jmp @r12
	add #-14,r7

	
.align 2
	_z80_IFF1_424: .long _z80_IFF1
	_z80_IFF2_424: .long _z80_IFF2



	
	
	
ed_7d:

	
	
	
	

	


	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	! Restore IFF1 & signal the BUS
	mov.l _z80_IFF2_425,r0

	mov.b @r0,r1
	dt r0                ! IFF1 is immediately before IFF2
	mov.b r1,@r0

	
	jmp @r12
	add #-14,r7

	
.align 2
	_z80_IFF1_425: .long _z80_IFF1
	_z80_IFF2_425: .long _z80_IFF2



	
	
	
ed_75:

	
	
	
	

	


	
!vbtvbt
	
	
		mov.l @r10,r2
                
		jsr @r2
		mov.w @(_z80_SP - _z80_BC,r5),r0
	
      ! Read the 1st byte of the PC
	extu.b r1,r6
	
	
		mov.l @r10,r2
                
		jsr @r2
		add #1,r0
	
         ! Read the 2nd byte of the PC
	extu.b r1,r1
	add #1,r0
	shll8 r1
	mov.w r0,@(_z80_SP - _z80_BC,r5)                 ! Writeback SP
	or r1,r6
    ! Base PC


	! Restore IFF1 & signal the BUS
	mov.l _z80_IFF2_426,r0

	mov.b @r0,r1
	dt r0                ! IFF1 is immediately before IFF2
	mov.b r1,@r0

	
	jmp @r12
	add #-14,r7

	
.align 2
	_z80_IFF1_426: .long _z80_IFF1
	_z80_IFF2_426: .long _z80_IFF2



	
	
	
op_c7:

	
	! Write the PC
	mov.w @(_z80_SP - _z80_BC,r5),r0

	mov r6,r1
    ! Un-base PC
	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
mov r6,r1
    ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov #0x0,r6             ! Get the new PC (it does not require unsigned extension)
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	    ! Base PC

	
	jmp @r12
	add #-11,r7




	
	
	
op_cf:

	
	! Write the PC
	mov.w @(_z80_SP - _z80_BC,r5),r0

	mov r6,r1
    ! Un-base PC
	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
mov r6,r1
    ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov #0x8,r6             ! Get the new PC (it does not require unsigned extension)
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	    ! Base PC

	
	jmp @r12
	add #-11,r7




	
	
	
op_d7:

	
	! Write the PC
	mov.w @(_z80_SP - _z80_BC,r5),r0

	mov r6,r1
    ! Un-base PC
	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
mov r6,r1
    ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov #0x10,r6             ! Get the new PC (it does not require unsigned extension)
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	    ! Base PC

	
	jmp @r12
	add #-11,r7




	
	
	
op_df:

	
	! Write the PC
	mov.w @(_z80_SP - _z80_BC,r5),r0

	mov r6,r1
    ! Un-base PC
	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
mov r6,r1
    ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov #0x18,r6             ! Get the new PC (it does not require unsigned extension)
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	    ! Base PC

	
	jmp @r12
	add #-11,r7




	
	
	
op_e7:

	
	! Write the PC
	mov.w @(_z80_SP - _z80_BC,r5),r0

	mov r6,r1
    ! Un-base PC
	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
mov r6,r1
    ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov #0x20,r6             ! Get the new PC (it does not require unsigned extension)
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	    ! Base PC

	
	jmp @r12
	add #-11,r7




	
	
	
op_ef:

	
	! Write the PC
	mov.w @(_z80_SP - _z80_BC,r5),r0

	mov r6,r1
    ! Un-base PC
	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
mov r6,r1
    ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov #0x28,r6             ! Get the new PC (it does not require unsigned extension)
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	    ! Base PC

	
	jmp @r12
	add #-11,r7




	
	
	
op_f7:

	
	! Write the PC
	mov.w @(_z80_SP - _z80_BC,r5),r0

	mov r6,r1
    ! Un-base PC
	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
mov r6,r1
    ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov #0x30,r6             ! Get the new PC (it does not require unsigned extension)
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	    ! Base PC

	
	jmp @r12
	add #-11,r7




	
	
	
op_ff:

	
	! Write the PC
	mov.w @(_z80_SP - _z80_BC,r5),r0

	mov r6,r1
    ! Un-base PC
	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	

	add #-1,r0
mov r6,r1
    ! Un-base PC
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		nop
	


	mov #0x38,r6             ! Get the new PC (it does not require unsigned extension)
	mov.w r0,@(_z80_SP - _z80_BC,r5)            ! Writeback SP
	    ! Base PC

	
	jmp @r12
	add #-11,r7




! **************************************************************************
! I/O


	
	
	
op_d3:

	
	
	
	

	


	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
		mov r3,r0
		mov r3,r1
	
	shll8 r0
	or r2,r0

	
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_Out_427, r2
	mov.l @r2,r2
	extu.w r0,r4   ! first parameter  (address)
	jsr @r2
	extu.b r1,r5   ! second parameter (data)
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	



	
	jmp @r12
	add #-11,r7


.align 2
	_z80_Out_427: .long _z80_Out



	
	
	
op_db:

	
	
	
	

	


	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	extu.b r2,r2

	
	! Load operand to r0
	mov r3,r0

	shll8 r0
	or r2,r0

	
		
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_In_428, r2
	mov.l @r2,r2
	jsr @r2
	extu.w r0,r4          ! SH requires to get parameter value here
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


		mov r0,r3
		
	
	
	jmp @r12
	add #-11,r7


.align 2
	_z80_In_428: .long _z80_In



	
	
	
ed_40:

	
	
	
	

	


	mov.w @(_z80_BC - _z80_BC,r5),r0
	mov #0,r8

	
		
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_In_429, r2
	mov.l @r2,r2
	jsr @r2
	extu.w r0,r4          ! SH requires to get parameter value here
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


		mov.b r0,@(_z80_BC - _z80_BC,r5)          ! write the result
	
	
	
	

	
	! Set Z
	mov r0,r13

	
	
	jmp @r12
	add #-12,r7


.align 2
	_z80_In_429: .long _z80_In



	
	
	
ed_48:

	
	
	
	

	


	mov.w @(_z80_BC - _z80_BC,r5),r0
	mov #0,r8

	
		
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_In_430, r2
	mov.l @r2,r2
	jsr @r2
	extu.w r0,r4          ! SH requires to get parameter value here
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


		mov.b r0,@(_z80_BC - _z80_BC + 1,r5)          ! write the result
	
	
	
	

	
	! Set Z
	mov r0,r13

	
	
	jmp @r12
	add #-12,r7


.align 2
	_z80_In_430: .long _z80_In



	
	
	
ed_50:

	
	
	
	

	


	mov.w @(_z80_BC - _z80_BC,r5),r0
	mov #0,r8

	
		
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_In_431, r2
	mov.l @r2,r2
	jsr @r2
	extu.w r0,r4          ! SH requires to get parameter value here
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


		mov.b r0,@(_z80_DE - _z80_BC,r5)          ! write the result
	
	
	
	

	
	! Set Z
	mov r0,r13

	
	
	jmp @r12
	add #-12,r7


.align 2
	_z80_In_431: .long _z80_In



	
	
	
ed_58:

	
	
	
	

	


	mov.w @(_z80_BC - _z80_BC,r5),r0
	mov #0,r8

	
		
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_In_432, r2
	mov.l @r2,r2
	jsr @r2
	extu.w r0,r4          ! SH requires to get parameter value here
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


		mov.b r0,@(_z80_DE - _z80_BC + 1,r5)          ! write the result
	
	
	
	

	
	! Set Z
	mov r0,r13

	
	
	jmp @r12
	add #-12,r7


.align 2
	_z80_In_432: .long _z80_In



	
	
	
ed_60:

	
	
	
	

	


	mov.w @(_z80_BC - _z80_BC,r5),r0
	mov #0,r8

	
		
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_In_433, r2
	mov.l @r2,r2
	jsr @r2
	extu.w r0,r4          ! SH requires to get parameter value here
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


		mov.b r0,@(_z80_HL - _z80_BC,r5)          ! write the result
	
	
	
	

	
	! Set Z
	mov r0,r13

	
	
	jmp @r12
	add #-12,r7


.align 2
	_z80_In_433: .long _z80_In



	
	
	
ed_68:

	
	
	
	

	


	mov.w @(_z80_BC - _z80_BC,r5),r0
	mov #0,r8

	
		
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_In_434, r2
	mov.l @r2,r2
	jsr @r2
	extu.w r0,r4          ! SH requires to get parameter value here
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


		mov.b r0,@(_z80_HL - _z80_BC + 1,r5)          ! write the result
	
	
	
	

	
	! Set Z
	mov r0,r13

	
	
	jmp @r12
	add #-12,r7


.align 2
	_z80_In_434: .long _z80_In



	
	
	
ed_70:

	
	
	
	

	


	mov.w @(_z80_BC - _z80_BC,r5),r0
	mov #0,r8
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_In_435, r2
	mov.l @r2,r2
	jsr @r2
	extu.w r0,r4          ! SH requires to get parameter value here
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	



	
	

	
	! Set Z
	mov r0,r13


	
	jmp @r12
	add #-12,r7


.align 2
	_z80_In_435: .long _z80_In



	
	
	
ed_78:

	
	
	
	

	


	mov.w @(_z80_BC - _z80_BC,r5),r0
	mov #0,r8

	
		
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_In_436, r2
	mov.l @r2,r2
	jsr @r2
	extu.w r0,r4          ! SH requires to get parameter value here
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


		mov r0,r3
		
	
	
	

	
	! Set Z
	mov r0,r13

	
	
	jmp @r12
	add #-12,r7


.align 2
	_z80_In_436: .long _z80_In



	
	
	
ed_41:

	
	
	
	

	


	mov.l @(_z80_BC - _z80_BC,r5),r1
	shlr16 r1
!vbt pas testé
!vbt correct
	
		mov.b @(_z80_BC - _z80_BC,r5),r0
		
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_Out_437, r2
	mov.l @r2,r2
	extu.w r1,r4   ! first parameter  (address)
	jsr @r2
	extu.b r0,r5   ! second parameter (data)
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


	

	
	jmp @r12
	add #-12,r7


.align 2
	_z80_Out_437: .long _z80_Out



	
	
	
ed_49:

	
	
	
	

	


	mov.l @(_z80_BC - _z80_BC,r5),r1
	shlr16 r1
!vbt pas testé
!vbt correct
	
		mov.b @(_z80_BC - _z80_BC + 1,r5),r0
		
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_Out_438, r2
	mov.l @r2,r2
	extu.w r1,r4   ! first parameter  (address)
	jsr @r2
	extu.b r0,r5   ! second parameter (data)
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


	

	
	jmp @r12
	add #-12,r7


.align 2
	_z80_Out_438: .long _z80_Out



	
	
	
ed_51:

	
	
	
	

	


	mov.l @(_z80_BC - _z80_BC,r5),r1
	shlr16 r1
!vbt pas testé
!vbt correct
	
		mov.b @(_z80_DE - _z80_BC,r5),r0
		
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_Out_439, r2
	mov.l @r2,r2
	extu.w r1,r4   ! first parameter  (address)
	jsr @r2
	extu.b r0,r5   ! second parameter (data)
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


	

	
	jmp @r12
	add #-12,r7


.align 2
	_z80_Out_439: .long _z80_Out



	
	
	
ed_59:

	
	
	
	

	


	mov.l @(_z80_BC - _z80_BC,r5),r1
	shlr16 r1
!vbt pas testé
!vbt correct
	
		mov.b @(_z80_DE - _z80_BC + 1,r5),r0
		
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_Out_440, r2
	mov.l @r2,r2
	extu.w r1,r4   ! first parameter  (address)
	jsr @r2
	extu.b r0,r5   ! second parameter (data)
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


	

	
	jmp @r12
	add #-12,r7


.align 2
	_z80_Out_440: .long _z80_Out



	
	
	
ed_61:

	
	
	
	

	


	mov.l @(_z80_BC - _z80_BC,r5),r1
	shlr16 r1
!vbt pas testé
!vbt correct
	
		mov.b @(_z80_HL - _z80_BC,r5),r0
		
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_Out_441, r2
	mov.l @r2,r2
	extu.w r1,r4   ! first parameter  (address)
	jsr @r2
	extu.b r0,r5   ! second parameter (data)
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


	

	
	jmp @r12
	add #-12,r7


.align 2
	_z80_Out_441: .long _z80_Out



	
	
	
ed_69:

	
	
	
	

	


	mov.l @(_z80_BC - _z80_BC,r5),r1
	shlr16 r1
!vbt pas testé
!vbt correct
	
		mov.b @(_z80_HL - _z80_BC + 1,r5),r0
		
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_Out_442, r2
	mov.l @r2,r2
	extu.w r1,r4   ! first parameter  (address)
	jsr @r2
	extu.b r0,r5   ! second parameter (data)
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


	

	
	jmp @r12
	add #-12,r7


.align 2
	_z80_Out_442: .long _z80_Out



	
	
	
ed_71:

	
	
	
	

	


	mov.w @(_z80_BC - _z80_BC,r5),r0
	mov #0,r1
	
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_Out_443, r2
	mov.l @r2,r2
	extu.w r0,r4   ! first parameter  (address)
	jsr @r2
	extu.b r1,r5   ! second parameter (data)
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	



	
	jmp @r12
	add #-12,r7


.align 2
	_z80_Out_443: .long _z80_Out



	
	
	
ed_79:

	
	
	
	

	


	mov.l @(_z80_BC - _z80_BC,r5),r1
	shlr16 r1
!vbt pas testé
!vbt correct
	
		
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_Out_444, r2
	mov.l @r2,r2
	extu.w r1,r4   ! first parameter  (address)
	jsr @r2
	extu.b r3,r5   ! second parameter (data)
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


	

	
	jmp @r12
	add #-12,r7


.align 2
	_z80_Out_444: .long _z80_Out



	
	
	
ed_a2:

	
! FIXME - Parity flag is not emulated

	
	
	

	


	
	! In from BC
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0
	extu.b r0,r0            ! high byte clear? (documentation is fuzzy)
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_In_445, r2
	mov.l @r2,r2
	jsr @r2
	extu.w r0,r4          ! SH requires to get parameter value here
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


	mov r0,r1

	

	! Decrease B
	mov.b @(_z80_BC - _z80_BC,r5),r0
	add #-1,r0
	mov.b r0,@(_z80_BC - _z80_BC,r5)
	
	

	
	! Set Z
	mov r0,r13

	
	! Write to (HL)
!vbtvbt        
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	


	! Writeback HL register
	add #1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)

    ! This is a prime example of the weirdness of the Z80...
	


	
	jmp @r12
	add #-16,r7


.align 2
	_z80_In_445: .long _z80_In



	
	
	
ed_aa:

	
! FIXME - Parity flag is not emulated

	
	
	

	


	
	! In from BC
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0
	extu.b r0,r0            ! high byte clear? (documentation is fuzzy)
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_In_446, r2
	mov.l @r2,r2
	jsr @r2
	extu.w r0,r4          ! SH requires to get parameter value here
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


	mov r0,r1

	

	! Decrease B
	mov.b @(_z80_BC - _z80_BC,r5),r0
	add #-1,r0
	mov.b r0,@(_z80_BC - _z80_BC,r5)
	
	

	
	! Set Z
	mov r0,r13

	
	! Write to (HL)
!vbtvbt        
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	


	! Writeback HL register
	add #-1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)

    ! This is a prime example of the weirdness of the Z80...
	


	
	jmp @r12
	add #-16,r7


.align 2
	_z80_In_446: .long _z80_In



	
	
	
ed_b2:

	
	
 
	
	

	

linxr_loop447:
	! In from BC
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0
	extu.b r0,r0            ! high byte clear? (documentation is fuzzy)
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_In_447, r2
	mov.l @r2,r2
	jsr @r2
	extu.w r0,r4          ! SH requires to get parameter value here
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


	mov r0,r1

	

	! Decrease B
	mov.b @(_z80_BC - _z80_BC,r5),r0
	add #-1,r0
	mov.b r0,@(_z80_BC - _z80_BC,r5)
	
	

	
	! Set Z
	mov r0,r13

	
	! Write to (HL)
!vbtvbt        
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	


	! Writeback HL register
	add #1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)

    ! This is a prime example of the weirdness of the Z80...
	

.align 5
	_z80_In_447: .long _z80_Out
.align 5
linxr_zero447:
nop




	
	jmp @r12
	add #-16,r7




	
	
	
ed_ba:

	
	
 
	
	

	

linxr_loop448:
	! In from BC
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0
	extu.b r0,r0            ! high byte clear? (documentation is fuzzy)
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_In_448, r2
	mov.l @r2,r2
	jsr @r2
	extu.w r0,r4          ! SH requires to get parameter value here
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	


	mov r0,r1

	

	! Decrease B
	mov.b @(_z80_BC - _z80_BC,r5),r0
	add #-1,r0
	mov.b r0,@(_z80_BC - _z80_BC,r5)
	
	

	
	! Set Z
	mov r0,r13

	
	! Write to (HL)
!vbtvbt        
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.w @(_z80_HL - _z80_BC,r5),r0
	


	! Writeback HL register
	add #-1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)

    ! This is a prime example of the weirdness of the Z80...
	

.align 5
	_z80_In_448: .long _z80_Out
.align 5
linxr_zero448:
nop




	
	jmp @r12
	add #-16,r7




	
	
	
ed_a3:

	
! FIXME - Parity flag is not emulated

	
	
	

	


	
	! Read from (HL)
!vbtvbt        
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	

	! Writeback HL register
	add #1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)
	
	! Out to BC
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0
	extu.b r0,r0            ! high byte clear? (documentation is fuzzy)
	
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_Out_449, r2
	mov.l @r2,r2
	extu.w r0,r4   ! first parameter  (address)
	jsr @r2
	extu.b r1,r5   ! second parameter (data)
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	



	! Decrease B
	mov.b @(_z80_BC - _z80_BC,r5),r0
	add #-1,r0
	mov.b r0,@(_z80_BC - _z80_BC,r5)
	
	! Set Z
	mov r0,r13

	
	


	! This is a prime example of the weirdness of the Z80...
	


	
	jmp @r12
	add #-16,r7


.align 2
	_z80_Out_449: .long _z80_Out



	
	
	
ed_ab:

	
! FIXME - Parity flag is not emulated

	
	
	

	


	
	! Read from (HL)
!vbtvbt        
	
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	

	! Writeback HL register
	add #-1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)
	
	! Out to BC
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0
	extu.b r0,r0            ! high byte clear? (documentation is fuzzy)
	
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_Out_450, r2
	mov.l @r2,r2
	extu.w r0,r4   ! first parameter  (address)
	jsr @r2
	extu.b r1,r5   ! second parameter (data)
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	



	! Decrease B
	mov.b @(_z80_BC - _z80_BC,r5),r0
	add #-1,r0
	mov.b r0,@(_z80_BC - _z80_BC,r5)
	
	! Set Z
	mov r0,r13

	
	


	! This is a prime example of the weirdness of the Z80...
	


	
	jmp @r12
	add #-16,r7


.align 2
	_z80_Out_450: .long _z80_Out



	
	
	
ed_b3:

	
	

	
	

	

!!`lotxr_loop'ln:
!vbtvbt        
	! Read from (HL)
        
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	

	! Writeback HL register
	add #1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)
	
	! Out to BC
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0
	extu.b r0,r0            ! high byte clear? (documentation is fuzzy)
!vbt à corriger        
	
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_Out_451, r2
	mov.l @r2,r2
	extu.w r0,r4   ! first parameter  (address)
	jsr @r2
	extu.b r1,r5   ! second parameter (data)
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	



	! Decrease B
	mov.b @(_z80_BC - _z80_BC,r5),r0
	dt r0
	bt/s lotxr_zero451
	mov.b r0,@(_z80_BC - _z80_BC,r5)

	add #-5,r7        
	add #-2,r6
	bra lotxr_end451:

lotxr_zero451:
	
	! Set Z
	mov r0,r13

	
	


.align 5
lotxr_end451:


	jmp @r12
	add #-16,r7


!ALIGN
	_z80_Out_451: .long _z80_Out

	
	jmp @r12
	add #-16,r7




	
	
	
ed_bb:

	
	

	
	

	

!!`lotxr_loop'ln:
!vbtvbt        
	! Read from (HL)
        
	
		mov.l @r10,r2
                mov.l @(_z80_HL - _z80_BC,r5),r0
		jsr @r2
		shlr16 r0
	


	

	! Writeback HL register
	add #-1,r0
	mov.w r0,@(_z80_HL - _z80_BC,r5)
	
	! Out to BC
	mov.b @(_z80_BC - _z80_BC + 1,r5),r0
	extu.b r0,r0            ! high byte clear? (documentation is fuzzy)
!vbt à corriger        
	
	
	
	
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

	mov.l _z80_Out_452, r2
	mov.l @r2,r2
	extu.w r0,r4   ! first parameter  (address)
	jsr @r2
	extu.b r1,r5   ! second parameter (data)
	
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	



	! Decrease B
	mov.b @(_z80_BC - _z80_BC,r5),r0
	dt r0
	bt/s lotxr_zero452
	mov.b r0,@(_z80_BC - _z80_BC,r5)

	add #-5,r7        
	add #-2,r6
	bra lotxr_end452:

lotxr_zero452:
	
	! Set Z
	mov r0,r13

	
	


.align 5
lotxr_end452:


	jmp @r12
	add #-16,r7


!ALIGN
	_z80_Out_452: .long _z80_Out

	
	jmp @r12
	add #-16,r7




! **************************************************************************
! misc


	
	
	
op_27:

	
	
	
	

	


	mov r8,r0          ! r0 = flags
	mov.l DAA_Table_453,r1
	tst #0x02,r0
	extu.b r3,r3      ! Para calcular desp. tabla DAA
	and #0x2,r0       ! Enmascarar registro banderas
	shll8 r0
	bt/s .daa_nh_set
	or r3,r0          ! r0 = zAF
	tst #0x04,r0

!define(`FLG_V',`0x01') ! 01h         ! parity/overflow
!define(`FLG_P',`0x01') ! 01h         ! parity/overflow
!define(`FLG_H',`0x02') ! 02h         ! half-carry
!define(`FLG_N',`0x04') ! 04h         ! negative
!define(`FLG_iV',`0x08') ! 08h        ! V flag indicator

	mov r0,r2
	mov r8,r0          ! r0 = flags
	tst #0x04,r0
	bf/s .daa_n_set 
	mov r2,r0
	and #0xff,r0
.daa_n_set:
	mov.l daatbl_offset_453,r2
	add r2,r1
.daa_nh_set:
	shll r0
	mov.w @(r0,r1),r3
	mov r3,r8
	shlr8 r8
	exts.b r3,r3
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV


	
	


	
	jmp @r12
	add #-4,r7


.align 2
	daatbl_offset_453: .long 512*4
	DAA_Table_453: .long DAA_Table



	
	
	
op_2f:

	
	not r3,r3
	! H and N is set, not affected otherwise
	mov #(0x02|0x04),r0
	
	

	or r0,r8

	
	jmp @r12
	add #-4,r7




	
	
	
ed_44:

	
	
	
	

	


	mov.l NEG_Table_454,r0
	extu.b r3,r3
	mov.b @(r0,r3),r8
	neg r3,r3
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	
	


	
	jmp @r12
	add #-8,r7


	.align 2
	NEG_Table_454: .long NEG_Table



	
	
	
ed_4c:

	
	
	
	

	


	mov.l NEG_Table_455,r0
	extu.b r3,r3
	mov.b @(r0,r3),r8
	neg r3,r3
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	
	


	
	jmp @r12
	add #-8,r7


	.align 2
	NEG_Table_455: .long NEG_Table



	
	
	
ed_54:

	
	
	
	

	


	mov.l NEG_Table_456,r0
	extu.b r3,r3
	mov.b @(r0,r3),r8
	neg r3,r3
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	
	


	
	jmp @r12
	add #-8,r7


	.align 2
	NEG_Table_456: .long NEG_Table



	
	
	
ed_5c:

	
	
	
	

	


	mov.l NEG_Table_457,r0
	extu.b r3,r3
	mov.b @(r0,r3),r8
	neg r3,r3
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	
	


	
	jmp @r12
	add #-8,r7


	.align 2
	NEG_Table_457: .long NEG_Table



	
	
	
ed_64:

	
	
	
	

	


	mov.l NEG_Table_458,r0
	extu.b r3,r3
	mov.b @(r0,r3),r8
	neg r3,r3
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	
	


	
	jmp @r12
	add #-8,r7


	.align 2
	NEG_Table_458: .long NEG_Table



	
	
	
ed_6c:

	
	
	
	

	


	mov.l NEG_Table_459,r0
	extu.b r3,r3
	mov.b @(r0,r3),r8
	neg r3,r3
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	
	


	
	jmp @r12
	add #-8,r7


	.align 2
	NEG_Table_459: .long NEG_Table



	
	
	
ed_74:

	
	
	
	

	


	mov.l NEG_Table_460,r0
	extu.b r3,r3
	mov.b @(r0,r3),r8
	neg r3,r3
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	
	


	
	jmp @r12
	add #-8,r7


	.align 2
	NEG_Table_460: .long NEG_Table



	
	
	
ed_7c:

	
	
	
	

	


	mov.l NEG_Table_461,r0
	extu.b r3,r3
	mov.b @(r0,r3),r8
	neg r3,r3
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV

	
	


	
	jmp @r12
	add #-8,r7


	.align 2
	NEG_Table_461: .long NEG_Table



	
	
	
op_3f:

	
	mov #~(0x04 | 0x02),r2
	tst r11,r11     ! not C -> T
	and r2,r8         ! reset N flag
	mov r11,r1     ! TMP_REG = old carry
	movt r11            ! FLAGS_C = not C
	shll r1            ! get H flag in place
	or r1,r8           ! H = old carry

	
	

	
	jmp @r12
	add #-4,r7




	
	
	
op_37:

	
	mov #(0x08 | 0x01),r2    ! Z,S and P not affected
	mov #1,r11
	and r2,r8
	
	


	
	jmp @r12
	add #-4,r7




	
	
	
op_00:

	
	
	jmp @r12
	add #-4,r7




	
	
	
op_76:

	
	
	
	

	


	mov.l _z80_TempICount_462,r2
	mov.l @r2,r1    ! TempICount
	mov #3,r0
	and r0,r7
	and r0,r1

	tst r7,r7
	bt halt_zero

	add #-4,r1
	add #-4,r7

halt_zero:
	mov.l r1,@r2
	mov.l _z80_halted_462,r2
	mov #1,r0
	
	
	


	! jump to z80_finish
	mov.l z80_finish_463,r0
	jmp @r0
	mov.b r0,@r2

.align 2
	z80_finish_463: .long z80_finish


	.align 2
	_z80_halted_462: .long _z80_halted
	_z80_TempICount_462: .long _z80_TempICount



	
	
	
op_fb:

	
	
	
	

	


	! set the P/V bit, for use in "LD I,A" etc
	mov #0x01,r0
	mov.l _z80_IFF1_464,r1
	mov.l _z80_IFF2_464,r2
	mov.b r0,@r1            ! setting IFF1
	mov.b r0,@r2          ! setting IFF2

	mov.l _z80_afterEI_464,r1
	mov #1,r0
	mov.b r0,@r1

	! force the next instruction to be executed regardless of ICount
	! then force an IRQ check after it.	
	! set ICount to one so it will terminate
	! after the next instruction.
	! we can then pick it up again and carry on.
	mov.l _z80_TempICount_464,r1
	mov.l r7,@r1

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll2 r0
	mov.l @(r0,r4),r0
	
	jmp @r0
	mov #1,r7


	.align 2
	_z80_TempICount_464: .long _z80_TempICount
	_z80_IFF1_464: .long _z80_IFF1
	_z80_IFF2_464: .long _z80_IFF2
	_z80_afterEI_464: .long _z80_afterEI



	
	
	
op_f3:

	
	
	
	

	


	mov #0,r0
	mov.l _z80_IFF1_465,r1
	mov.w r0,@r1    ! Loads zero in zIFF1 and zIFF2

	
	jmp @r12
	add #-4,r7

	
	.align 2
	_z80_IFF1_465: .long _z80_IFF1



	
	
	
ed_46:

	
	
	
	

	


	mov.l _z80_IM_466,r1
	mov #0,r0
	mov.b r0,@r1

	
	jmp @r12
	add #-8,r7

	
	.align 2
	_z80_IM_466: .long _z80_IM



	
	
	
ed_66:

	
	
	
	

	


	mov.l _z80_IM_467,r1
	mov #0,r0
	mov.b r0,@r1

	
	jmp @r12
	add #-8,r7

	
	.align 2
	_z80_IM_467: .long _z80_IM



	
	
	
ed_56:

	
	
	
	

	


	mov.l _z80_IM_468,r1
	mov #1,r0
	mov.b r0,@r1

	
	jmp @r12
	add #-8,r7

	
	.align 2
	_z80_IM_468: .long _z80_IM



	
	
	
ed_76:

	
	
	
	

	


	mov.l _z80_IM_469,r1
	mov #1,r0
	mov.b r0,@r1

	
	jmp @r12
	add #-8,r7

	
	.align 2
	_z80_IM_469: .long _z80_IM



	
	
	
ed_4e:

	
	
	
	

	


	mov.l _z80_IM_470,r1
	mov #1,r0
	mov.b r0,@r1

	
	jmp @r12
	add #-8,r7

	
	.align 2
	_z80_IM_470: .long _z80_IM



	
	
	
ed_6e:

	
	
	
	

	


	mov.l _z80_IM_471,r1
	mov #1,r0
	mov.b r0,@r1

	
	jmp @r12
	add #-8,r7

	
	.align 2
	_z80_IM_471: .long _z80_IM



	
	
	
ed_5e:

	
	
	
	

	


	mov.l _z80_IM_472,r1
	mov #2,r0
	mov.b r0,@r1

	
	jmp @r12
	add #-8,r7

	
	.align 2
	_z80_IM_472: .long _z80_IM



	
	
	
ed_7e:

	
	
	
	

	


	mov.l _z80_IM_473,r1
	mov #2,r0
	mov.b r0,@r1

	
	jmp @r12
	add #-8,r7

	
	.align 2
	_z80_IM_473: .long _z80_IM



! --- the end ---

!----------------------------------------------------------------------------!

!----------------------------------------------------------------------------!
! Support functions:


	
	
	
op_cb:

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	mov.l CBTable_addr,r1
	shll2 r0
	add r1,r0
	mov.l @r0,r0
	jmp @r0
	nop

.align 2
	CBTable_addr: .long CBTable



	
	
	
op_ed:

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	mov.l EDTable_addr,r1
	shll2 r0
	mov.l @(r0,r1),r0
	jmp @r0
	nop

	.align 2
	EDTable_addr: .long EDTable



	
	
	
op_dd:

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	mov.l DDTable_addr,r1
	shll2 r0
	mov.l @(r0,r1),r0
	jmp @r0
	nop

.align 2
	DDTable_addr: .long DDTable



	
	
	
op_fd:

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	mov.l FDTable_addr,r1
	shll2 r0
	mov.l @(r0,r1),r0
	jmp @r0
	nop

.align 2
	FDTable_addr: .long FDTable


	
	
	
dd_cb:

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	mov.l DDCBTable_addr,r0
	shll2 r2
	mov.l @(r0,r2),r2
	mov.l @(_z80_IX - _z80_BC,r5),r0
	shlr16 r0
	
	jmp @r2
	add r1,r0

.align 2
	DDCBTable_addr: .long DDCBTable


	
	
	
fd_cb:

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r1
		add #1,r6
	
	extu.w r6,r6

	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r2
		add #1,r6
	
	extu.w r6,r6

	mov.l DDCBTable_addr2,r0
	shll2 r2
	mov.l @(r0,r2),r2
	mov.w @(_z80_IY - _z80_BC,r5),r0
	
	jmp @r2
	add r1,r0

.align 2
	DDCBTable_addr2: .long DDCBTable


	
	
	
ed_xx:

	
	
	jmp @r12
	add #-8,r7



	
	
	
dd_xx:

	add #-1,r6
	extu.w r6,r6

   
	jmp @r12
	add #-4,r7



	
	
	
fd_xx:

	add #-1,r6
	extu.w r6,r6

   
	jmp @r12
	add #-0,r7



! called when out-of-cycles
.align 5
z80_finish:
	mov.l _z80_afterEI_zf,r1
	mov.b @r1,r0
    ! Un-base PC
	cmp/eq #1,r0
	bf really_finish

	! Revisar este codigo
	mov.l _z80_ICount_zf,r0
	mov.l @(_z80_TempICount-_z80_ICount,r0),r2
	add r2,r7

	mov #0,r0
	mov.b r0,@r1    ! _z80_afterEI = 0
	
	! check the IRQ line
	mov #(_z80_IRQLine - _z80_BC),r0
	mov.b @(r0,r5),r0    ! r0 = z80_IRQLine
	tst r0,r0
	bt .finish_chkirq_finish

	! check ints are enabled
	mov #(_z80_IFF1 - _z80_BC),r0
	mov.b @(r0,r5),r0    ! r0 = z80_IFF1
	tst r0,r0
	bt .finish_chkirq_finish


	mov #0,r2
	mov #(_z80_IRQLine - _z80_BC),r0
	mov.b r2,@(r0,r5)   ! z80_IRQLine = 0

	! disable interrupts
	mov #0,r2
	mov #(_z80_IFF1 - _z80_BC),r0
	mov.b r2,@(r0,r5)
	mov #(_z80_IFF2 - _z80_BC),r0
	mov.b r2,@(r0,r5)

	

	mov #(_z80_halted - _z80_BC),r0
	mov.b r2,@(r0,r5)	! Un-HALT the Z80

	! push the PC into the z80 stack
	mov.w @(_z80_SP - _z80_BC,r5),r0
	mov r6,r1
	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	
     ! first byte
	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov r6,r1
	
  ! second byte

	mov.w r0,@(_z80_SP - _z80_BC,r5)                  ! update SP
	mov #(_z80_IM - _z80_BC),r0
	mov.b @(r0,r5),r0        ! r0 = z80_IM
	tst r0,r0
	bf .not_im0_finish

	! IM 0, 13 T-states
	


	mov #(_z80_IRQVector - _z80_BC),r0
	mov.b @(r0,r5),r0    ! r0 = z80_IRQVector
	extu.b r0,r0
	add #-127,r0              ! substract 0xC7 = 199
	add #-72,r0
	bra .finish_chkirq_finish
	extu.b r0,r6

.align 5
.not_im0_finish:
	dt r0
	bf .not_im1_finish
	
	! IM 1, 13 T-states
	


	bra .finish_chkirq_finish
	mov #0x38,r6

.align 5
.not_im1_finish:
	! IM 2, 19 T-states
	


	mov #(_z80_IRQVector - _z80_BC),r0
	mov.b @(r0,r5),r1    ! TMP_REG = z80_IRQVector
	mov #(_z80_I - _z80_BC),r0
	mov.b @(r0,r5),r0               ! r0 = z80_I
	extu.b r1,r1
	shll8 r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		or r1,r0
	

	add #1,r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		extu.b r1,r6
	

	extu.b r1,r1
	shll8 r1
	or r1,r6

.finish_chkirq_finish:

	add #-4,r7     ! subtract EIs cycles
	cmp/pl r7
	bf really_finish
	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll2 r0
	mov.l @(r0,r4),r0
	
	jmp @r0
	nop


.align 5
really_finish:

	! Writeback flags
	
	
	
	

	


	exts.b r13,r13
	mov.l Byte_Flags_474,r0
	mov.b @(r0,r13),r1    ! TMP_REG = SZ000P00

	mov r8,r0
	tst #0x08,r0     ! test P/V indicator
	bt/s .no_veval474
	or r11,r1                      ! TMP_REG = SZ000P0C

	mov #~0x04,r2
	shlr r0                                            ! V -> T
	and r2,r1                  ! TMP_REG = SZ00000C
	movt r2               ! DIRT_REG = V
	shll2 r2               ! V flag in place
	or r2,r1    ! TMP_REG = SZ000V0C

.no_veval474:
	shlr r8                            ! ignore V
	shlr r8                            ! T = H
	movt r2               ! DIRT_REG = H
	shll2 r2
	shll2 r2                ! H flag in place
	or r2,r1     ! TMP_REG = SZ0H0P0C

	shlr r8                             ! T = N
	movt r2                ! DIRT_REG = N
	shll r2
	or r2,r1
	extu.b r1,r8

!	mov #~(FLAG_3|FLAG_5),TMP_REG
!	mov ZFLAG35,r0
!	and TMP_REG,zF
!	and #(FLAG_3|FLAG_5),r0    ! isolate 3-5 bits
!	or zF,r0

	! Writeback A register
	shll8 r3
	mov.l _z80_AF_zf,r1
	or r3,r8
	mov.w r8,@r1

	! Writeback zR register
	

	! Writeback PC & calculate cycles executed
	mov.l _z80_ICount_zf,r0
	mov.l r7,@r0


!	shll16 zPC


	mov.l @(_z80_Initial_ICount - _z80_ICount,r0),r0
	mov.l r6,@(_z80_PC - _z80_BC,r5)
	sub r7,r0

	lds.l @r15+,pr          ! restoring SH procedure register
	
	mov.l @r15+,r13
	mov.l @r15+,r12
	mov.l @r15+,r11
	mov.l @r15+,r10
	mov.l @r15+,r9
	rts
	mov.l @r15+,r8

	.align 2
	_z80_afterEI_zf: .long _z80_afterEI
	_z80_ICount_zf:  .long _z80_ICount
	_z80_Fetch_zf:   .long _z80_Fetch
	_z80_AF_zf:      .long _z80_AF
	_z80_R_zf:       .long _z80_R
	Byte_Flags_474: .long Byte_Flags


! Fetch decode
.align 5
z80_fdl:
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll2 r0
	cmp/pl r7

	bt/s .contexec
	mov.l @(r0,r4),r0

	bra z80_finish
	add #-1,r6

.contexec:
	jmp @r0
	nop


! Memread
.align 5
_z80_memread:
	
! vbt inutile shlr16 fait avant        
		extu.w r0,r0
		mov r0,r1    ! Load address into TMP_REG
		shlr8 r1
		shll2 r1
		mov.l _z80_Read_mr,r10
		shll r1
		add r10,r1
		
			mov.l @r1,r10
			tst r10,r10
			bf .use_rhandler
		
		mov.l @(4,r1),r1
		mov.l _z80_memread_mr,r10    ! restore MEM_REG address
		rts
		mov.b @(r0,r1),r1
.align 5
	.use_rhandler:
		mov.l r0,@-r15
		
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

		jsr @r10
		extu.w r0,r4
		
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	

		mov.l _z80_memread_mr,r10    ! restore MEM_REG address
		exts.b r0,r1
		rts
		mov.l @r15+,r0
      
	.align 2
		_z80_Read_mr:    .long _z80_Read
		_z80_memread_mr: .long _z80_Mem_Handlers
	


! Memwrite
.align 5
_z80_memwrite:
	
		extu.w r0,r0
		mov r0,r2    ! Load address into DIRT_REG
		shlr8 r2
		shll2 r2
		mov.l _z80_Write_mw,r10
		shll r2
		add r10,r2
		mov.l @r2,r10
		tst r10,r10
		bf .use_whandler

		mov.l @(4,r2),r2
		mov.l _z80_memwrite_mw,r10    ! restore MEM_REG address
		rts
		mov.b r1,@(r0,r2)

	.align 5
	.use_whandler:
		mov.l r0,@-r15
		
	

!ifdef(`BASED_PC',`sub FETCH_REG,zPC')
!	mov.l zPC,ezPC_MEM
	mov.l r3,@-r15
	mov.l r4,@-r15
	mov.l r5,@-r15
mov.l r6,@-r15
	mov.l r7,@-r15
!	ifdef(`EMULATE_BITS_3_5',`SALVAR MACL')
	sts.l pr,@-r15          ! saving SH procedure register

		extu.w r0,r4         ! Load address
		jsr @r10
		extu.b r1,r5
		
	lds.l @r15+,pr          ! restoring SH procedure register
!	ifdef(`EMULATE_BITS_3_5',`RESTAURAR MACL')
	mov.l @r15+,r7
mov.l @r15+,r6
	mov.l @r15+,r5
	mov.l @r15+,r4
	mov.l @r15+,r3
!	mov.l ezPC_MEM,zPC
!ifdef(`BASED_PC',`add FETCH_REG,zPC')

	

		mov.l _z80_memwrite_mw,r10    ! restore MEM_REG address
		rts
		mov.l @r15+,r0

	.align 2
		_z80_Write_mw:    .long _z80_Write
		_z80_memwrite_mw: .long _z80_Mem_Handlers
	


! Default handlers
.align 5
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
.align 5
.global _z80_emulate
_z80_emulate:
	! Codigo de ciclado inicial
	mov.l _z80_Extra_Cycles_emul,r2
	mov.l @r2,r1                ! r1 = Extra Cycles
	mov #0,r0
	mov r4,r7           ! ICOUNT_REG = required cycles
	mov.l r0,@r2                ! Extra Cycles = 0
	mov.l _z80_afterEI_emul,r4
	mov.b r0,@r4                ! AfterEI = 0

	add r1,r7

	mov.l _z80_ICount_emul,r0
	mov.l r7,@r0        ! ICount set up
	mov.l r7,@(_z80_Initial_ICount - _z80_ICount,r0)  ! Initial ICount

	! Salvar registros a usar en la pila
	mov.l r8,@-r15
	mov.l REF_REG_emul,r5         ! Reference register
	mov.l r9,@-r15
	mov.l _z80_AF_emul,r0
	mov.l r10,@-r15
	mov.l _OpTable_emul,r4    ! Load opcode table
	mov.l r11,@-r15


	
	

	
	

	mov.l r12,@-r15
	mov.l r13,@-r15
	
	sts.l pr,@-r15          ! saving SH procedure register

	! Cache de registros z80
	mov.l @r0,r8                   ! zF
	shlr16 r8
	        
	mov.l z80_fdl_emul,r12     ! Load fdl address to execute fdl
	mov r8,r3
	mov.l @(_z80_PC-_z80_AF,r0),r6    ! ezPC
	
	mov r8,r0
	tst #0x40,r0               ! test Z flag
	movt r13               ! set in register
	and #0x80,r0               ! isolate S flag
	or r0,r13              ! merge S flag in
	exts.b r13,r13   ! get S in place
	
	mov r8,r0             ! reload flags
	shlr r0               ! T = C
	mov #1,r8             ! P/V indicator set
	movt r11          ! get cached C
	shlr r0               ! T = N
	addc r8,r8            ! zF = 000000iN
	tst #0x4,r0           ! test H flag
	movt r2
	tst r2,r2
	addc r8,r8            ! zF = 00000iNH
	shlr r0               ! T = V
	addc r8,r8            ! zF = 0000iNHV


	! Cache de registro R
	

	mov.l _z80_Fetch_emul,r9    ! FETCH_REG
	        
	
	mov.l _z80_Mem_Handlers_emul,r10
	
	
	        

	mov.l _z80_halted_emul,r2
	mov.b @r2,r1
	tst r1,r1                ! testing halt
	bt/s not_halted
	nop

	! Z80 CPU halted
	mov.l _z80_ICount_emul,r1
	mov r7,r0
	and #3,r0                    ! If we're halted, just skip it all.
	shlr8 r3                     ! set up A register to be uncached correctly 
	add #-4,r0

	bra z80_finish
	mov r0,r7


.align 5
not_halted:

	
	
	
	

	
		mov r6,r0
		shlr8 r0
		shll2 r0
		mov.l @(r0,r9),r0
		mov.b @(r0,r6),r0
		add #1,r6
	
	extu.w r6,r6

	shll2 r0
	mov.l @(r0,r4),r0
	
	jmp @r0
	shlr8 r3    ! set up A register


	.align 2
	_z80_Extra_Cycles_emul: .long _z80_Extra_Cycles
	_z80_ICount_emul:       .long _z80_ICount
	_z80_AF_emul:           .long _z80_AF
	_z80_R_emul:            .long _z80_R
	_z80_halted_emul:       .long _z80_halted
	_z80_afterEI_emul:      .long _z80_afterEI
	_OpTable_emul:          .long OpTable
	_z80_Fetch_emul:        .long _z80_Fetch
	titi:                  .long 0x060BD000
	


	
		_z80_Mem_Handlers_emul: .long _z80_Mem_Handlers
	
	REF_REG_emul:           .long _z80_BC
	z80_fdl_emul:           .long z80_fdl


! void z80_reset(void)
! Resets the Z80.
.align 5
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

	mov.b r0,@(_z80_AF-context_start+1,r1) ! AF = 0x0040


	.align 2
	context_start_reset: .long context_start
	registers_end_reset: .long registers_end


! void z80_raise_IRQ(UBYTE vector)
! Causes an interrupt with the given vector.
! In IM 0, the vector has to be an RST opcode.
! If interrupts are disabled, the interrupt will still happen as long as
! the IRQ line stays raised.
.align 5
.global _z80_raise_IRQ
_z80_raise_IRQ:
	mov.l _z80_IRQVector_raise,r1
	mov #1,r0
	mov.b r0,@(_z80_IRQLine-_z80_IRQVector,r1) ! z80_IRQLine = 1
	mov.b r4,@r1                               ! z80_IRQVector = vector

	mov.l r8,@-r15
	mov.l r9,@-r15
	mov.l r10,@-r15
	mov.l r11,@-r15
	mov.l r12,@-r15
	
	

	sts.l pr,@-r15          ! saving SH procedure register

	mov.l REF_REG_ri,r5                   ! Reference register
	mov.l _z80_PC_ri,r0

	! Cache R register
	

	mov.l @r0,r6                             ! ezPC
	mov.l _z80_Mem_Handlers_ri,r10

	
	! check the IRQ line
	mov #(_z80_IRQLine - _z80_BC),r0
	mov.b @(r0,r5),r0    ! r0 = z80_IRQLine
	tst r0,r0
	bt .finish_chkirq_raise

	! check ints are enabled
	mov #(_z80_IFF1 - _z80_BC),r0
	mov.b @(r0,r5),r0    ! r0 = z80_IFF1
	tst r0,r0
	bt .finish_chkirq_raise


	mov #0,r2
	mov #(_z80_IRQLine - _z80_BC),r0
	mov.b r2,@(r0,r5)   ! z80_IRQLine = 0

	! disable interrupts
	mov #0,r2
	mov #(_z80_IFF1 - _z80_BC),r0
	mov.b r2,@(r0,r5)
	mov #(_z80_IFF2 - _z80_BC),r0
	mov.b r2,@(r0,r5)

	

	mov #(_z80_halted - _z80_BC),r0
	mov.b r2,@(r0,r5)	! Un-HALT the Z80

	! push the PC into the z80 stack
	mov.w @(_z80_SP - _z80_BC,r5),r0
	mov r6,r1
	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		shlr8 r1
	
     ! first byte
	add #-1,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov r6,r1
	
  ! second byte

	mov.w r0,@(_z80_SP - _z80_BC,r5)                  ! update SP
	mov #(_z80_IM - _z80_BC),r0
	mov.b @(r0,r5),r0        ! r0 = z80_IM
	tst r0,r0
	bf .not_im0_raise

	! IM 0, 13 T-states
	


	mov #(_z80_IRQVector - _z80_BC),r0
	mov.b @(r0,r5),r0    ! r0 = z80_IRQVector
	extu.b r0,r0
	add #-127,r0              ! substract 0xC7 = 199
	add #-72,r0
	bra .finish_chkirq_raise
	extu.b r0,r6

.align 5
.not_im0_raise:
	dt r0
	bf .not_im1_raise
	
	! IM 1, 13 T-states
	


	bra .finish_chkirq_raise
	mov #0x38,r6

.align 5
.not_im1_raise:
	! IM 2, 19 T-states
	


	mov #(_z80_IRQVector - _z80_BC),r0
	mov.b @(r0,r5),r1    ! TMP_REG = z80_IRQVector
	mov #(_z80_I - _z80_BC),r0
	mov.b @(r0,r5),r0               ! r0 = z80_I
	extu.b r1,r1
	shll8 r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		or r1,r0
	

	add #1,r0
	
	
		mov.l @r10,r2
                
		jsr @r2
		extu.b r1,r6
	

	extu.b r1,r1
	shll8 r1
	or r1,r6

.finish_chkirq_raise:


	extu.w r6,r6
	mov.l r6,@(_z80_PC - _z80_BC,r5)

	! Writeback zR register
	

	lds.l @r15+,pr          ! restoring SH procedure register

	
	
	mov.l @r15+,r12
	mov.l @r15+,r11
	mov.l @r15+,r10
	mov.l @r15+,r9
	rts
	mov.l @r15+,r8

	.align 2
	_z80_IRQVector_raise: .long _z80_IRQVector
	_z80_PC_ri:           .long _z80_PC
	REF_REG_ri:           .long _z80_BC
	
		_z80_Mem_Handlers_ri:         .long _z80_Mem_Handlers
	
	_z80_R_ri:            .long _z80_R


! void z80_lower_IRQ(void)
! Lowers the IRQ line.
.align 5
.global _z80_lower_IRQ
_z80_lower_IRQ:
	mov #0,r0
	mov.l _z80_IRQLine_lower,r1
	rts
	mov.b r0,@r1          ! z80_IRQLine = 0

	.align 2
	_z80_IRQLine_lower: .long _z80_IRQLine


! void z80_cause_NMI(void)
! Causes a non-maskable interrupt.
! This will always be accepted.
! There is no raise/lower functions, as the NMI is edge-triggered.
.align 5
.global _z80_cause_NMI
_z80_cause_NMI:

	mov.l r10,@-r15
	mov.l r5,@-r15
	sts.l pr,@-r15               ! saving SH procedure register
	mov.l REF_REG_cnmi,r5   ! Reference register

	! Reset IFF1 and z80_halted
	mov.l _z80_IFF1_cnmi,r1
	mov #0,r0
	mov.b r0,@r1
	mov.l _z80_halted_cnmi,r1
	mov.b r0,@r1



	! Add extra cycles
	mov.l _z80_Extra_Cycles_cnmi,r0
	mov.l @r0,r1
	shlr16 r1
	add #11,r1
	mov.l r1,@r0

	mov.l _z80_PC_cnmi,r4
	mov.l _z80_Mem_Handlers_cnmi,r10    ! Necesario para las escrituras
	mov.l @r4,r1
 	mov.l _z80_SP_cnmi,r5
	mov.w @r5,r0
	add #-1,r0
	shlr8 r1
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov r0,r7
	


	add #-1,r7
	mov r7,r0
	
	
		mov.l @(4,r10),r2
!vbtvbttest                
!                mov #12,r14                
!vbtvbt                
                
		jsr @r2
		mov.l @r4,r1
	


	mov.w r7,@r5    ! Store the new SP
	mov #0x66,r1
	mov.l r1,@r4    ! Set the new PC to 66h

	lds.l @r15+,pr          ! restoring SH procedure register
	mov.l @r15+,r5
	rts
	mov.l @r15+,r10

.align 2
	_z80_Extra_Cycles_cnmi: .long _z80_Extra_Cycles
	_z80_IFF1_cnmi: .long _z80_IFF1
	_z80_halted_cnmi: .long _z80_halted
	_z80_R_cnmi: .long _z80_R
	_z80_PC_cnmi: .long _z80_PC
	_z80_SP_cnmi: .long _z80_SP
	REF_REG_cnmi: .long _z80_BC
	
		_z80_Mem_Handlers_cnmi: .long _z80_Mem_Handlers
	


! int z80_get_context_size(void)
! Returns the size of the context, in bytes
.align 5
.global _z80_get_context_size
_z80_get_context_size:
	mov.l context_end_gcs,r0
	mov.l context_start_gcs,r1
	sub r1,r0

	rts
	add #4,r0       ! safety gap
	
	.align 2
	context_end_gcs:   .long context_end
	context_start_gcs: .long context_start


! void z80_set_context(void *context)
! Copy the given context to the current Z80.
.align 5
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
	
	.align 2
	context_start_sc: .long context_start
	context_end_sc: .long context_end


! void z80_get_context(void *context)
! Copy the current Z80 to the given context.
.align 5
.global _z80_get_context
_z80_get_context:
	mov.l context_start_gc,r0
	mov.l context_end_gc,r1
nop
nop
nop
gc_loop:
	mov.l @r0+,r2
	mov.l r2,@r4
	cmp/eq r0,r1
	bf/s gc_loop
	add #4,r4

	rts
	mov #0,r0
	
	.align 2
	context_start_gc: .long context_start
	context_end_gc: .long context_end


! int z80_get_cycles_elapsed(void)
! Returns the number of cycles emulated since z80_emulate was called.
.align 5
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

	.align 2
	_z80_ICount_gce: .long _z80_ICount


! void z80_stop_emulating(void)
! Stops the emulation dead. (waits for the current instruction to finish
! first though).
! The return value from z80_execute will obviously be lower than expected.
.align 5
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

	.align 2
	_z80_ICount_se: .long _z80_ICount


! void z80_skip_idle(void)
! Stops the emulation dead. (waits for the current instruction to finish
! first though).
! The return value from z80_execute will appear as though it had executed
! all the instructions like normal.
.align 5
.global _z80_skip_idle
_z80_skip_idle:
	mov.l _z80_ICount_si,r0
	mov #0,r1
	mov.l r1,@r0
	rts
	mov.l r1,@(_z80_TempICount-_z80_ICount,r0)

	.align 2
	_z80_ICount_si: .long _z80_ICount

! void z80_do_wait_states(int n)
! Halts the CPU temporarily, to execute 'n' memory wait states.
.align 5
.global _z80_do_wait_states
_z80_do_wait_states:
	mov.l _z80_Initial_ICount_dws,r0
	mov.l @r0,r1
	add r4,r1
	rts
	mov.l r1,@r0

	.align 2
	_z80_Initial_ICount_dws: .long _z80_Initial_ICount


! UWORD z80_get_reg(z80_register reg)
! Return the value of the specified register.
.align 5
.global _z80_get_reg
_z80_get_reg:
	extu.w r4,r4
	mov #11,r1
	cmp/hi r1,r4
	bt .nirreg

	! Normal register loading
	mov.l _context_start_gr,r0
	shll2 r4
        
!vbt pas utile ?????        

	mov.l @(r0,r4),r0
	rts
	swap.w r0,r0


.nirreg:
	mov #12,r1
	cmp/eq r1,r4
	bf .irreg2

	mov.l _reg_iff1,r0
	rts
	mov.b @r0,r0

.irreg2:
	mov #13,r1
	cmp/eq r1,r4
	bf .irreg3

	mov.l _reg_iff2,r0
	rts
	mov.b @r0,r0

.irreg3:
	mov #17,r1
	cmp/eq r1,r4
	bf .irreg4

	mov.l _reg_irqline,r0
	rts
	mov.b @r0,r0

.irreg4:
	mov #18,r1
	cmp/eq r1,r4
	bf .irreg5

	mov.l _z80_halted_gr,r0
	rts
	mov.b @r0,r0

.irreg5:
	mov #15,r1
	cmp/eq r1,r4
	bf .irreg6

	mov.l _reg_im,r0
	mov.b @r0,r0
	rts
	extu.b r0,r0

.irreg6:
	mov #16,r1
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

.align 2
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
.align 5
.global _z80_set_reg
_z80_set_reg:
	extu.w r4,r4
	mov #11,r1
	cmp/hi r1,r4
	bt .sirreg

	! Normal register set
	mov.l _context_start_sr,r0
	shll2 r4
	rts
	mov.l r5,@(r0,r4)

.sirreg:
	mov #12,r1
	cmp/eq r1,r4
	bf .srreg2

	tst r5,r5
	movt r0
	mov.l _sreg_iff1,r1
	dt r0
	and #0x04,r0
	rts
	mov.b r0,@r1

.srreg2:
	mov #13,r1
	cmp/eq r1,r4
	bf .srreg3

	tst r5,r5
	movt r0
	mov.l _sreg_iff2,r1
	dt r0
	and #0x04,r0
	rts
	mov.b r0,@r1

.srreg3:
	mov #17,r1
	cmp/eq r1,r4
	bf .srreg4

	tst r5,r5
	movt r0
	mov.l _sreg_irqline,r1
	dt r0
	and #0x04,r0
	rts
	mov.b r0,@r1

.srreg4:
	mov #18,r1
	cmp/eq r1,r4
	bf .srreg5

	tst r5,r5
	movt r0
	mov.l _z80_shalted_gr,r1
	dt r0
	and #0x04,r0
	rts
	mov.b r0,@r1

.srreg5:
	mov #15,r1
	cmp/eq r1,r4
	bf .srreg6

	mov.l _sreg_im,r0
	rts
	mov.b r5,@r0


.srreg6:
	mov #16,r1
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


.align 2
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
.align 5
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
	
	mov.l r1,@(_z80_Fetch_Callback-_z80_In,r0)   ! z80_Fetch_Callback
	

	
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
	


	.align 2
	_z80_In_imm:         .long _z80_In
	z80_default_in_imm:  .long z80_default_in
	z80_default_out_imm: .long z80_default_out
	z80_default_reti_imm: .long z80_default_reti
	z80_def_fetch_callback_addr: .long z80_default_fetch_callback


	
		_z80_Read_imm: .long _z80_Read
		_z80_Write_imm: .long _z80_Write
		z80_default_read_imm: .long z80_default_read
		z80_default_write_imm: .long z80_default_write
		rw_table_length: .long 256
	



! void z80_end_memmap(void)
! Finishes the current memory map
! Empty, just to keep RAZE compatibility
.align 5
.global _z80_end_memmap
_z80_end_memmap:
	rts
	nop


! void z80_map_fetch(UWORD start, UWORD end, UBYTE *memory)
! Set the given area for op-code fetching
! start/end = the area it covers
! memory = the ROM/RAM to read from
.align 5
.global _z80_map_fetch
_z80_map_fetch:
	
	
! r9 est dans r6 
! les params sont dans r4 à r6


	extu.w r4,r4    ! 16-bit start address
	mov.l _z80_Fetch_addr,r0
	sub r4,r6


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


	.align 2
	_z80_Fetch_addr: .long _z80_Fetch


! void z80_map_read(UWORD start, UWORD end, UBYTE *memory)
! Moves a Z80_MAP_MAPPED area to use a different region of memory
! start/end = the area it covers (must be page-aligned)
! memory = the ROM/RAM to read from
.align 5
.global _z80_map_read
_z80_map_read:


	extu.w r4,r4
	mov.l _z80_Read_addr,r0
	sub r4,r6



	
		mov #1,r1
		extu.w r5,r5

		
		

		shll8 r1       ! r1 = incr. banco = 256
		mov #0,r2

	mr_loop:
		! if theres no area left inbetween, stop now.
		mov r4,r7
		shlr8 r7
	
		cmp/hi r5,r4
		bt/s mr_finish

		
			shll2 r7
			shll r7
		

		mov.l r2,@(r0,r7)    ! volcar 0
		add #4,r7
		mov.l r6,@(r0,r7)
		bra mr_loop          ! continuar
		add r1,r4            ! incrementando banco

	mr_finish:
		rts
		nop
	



	.align 2
	_z80_Read_addr: .long _z80_Read



! void z80_map_write(UWORD start, UWORD end, UBYTE *memory)
! Moves a Z80_MAP_MAPPED area to use a different region of memory
! start/end = the area it covers (must be page-aligned)
! memory = the ROM/RAM to read from
.align 5
.global _z80_map_write
_z80_map_write:


	extu.w r4,r4
	mov.l _z80_Write_addr,r0
	sub r4,r6



	
		mov #1,r1
		extu.w r5,r5

		
		

		shll8 r1       ! r1 = incr. banco = 256
		mov #0,r2

	mw_loop:
		! if theres no area left inbetween, stop now.
		mov r4,r7
		shlr8 r7

		cmp/hi r5,r4
		bt/s mw_finish

		
			shll2 r7
			shll r7
		

		mov.l r2,@(r0,r7)
		add #4,r7
		mov.l r6,@(r0,r7)
		bra mw_loop          ! continuar
		add r1,r4            ! incr. banco

	mw_finish:
		rts
		nop
	



	.align 2
	_z80_Write_addr: .long _z80_Write


! void z80_add_read(UWORD start, UWORD end, int method, void *data)
! Add a READ memory handler to the memory map
! start/end = the area it covers
! method = 0 for direct, 1 for handled
! data = RAM for direct, handler for handled

.align 5
	.global _z80_add_read
	_z80_add_read:

	
		
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

		

		ar_finish:
			rts
			nop
	

	
		.align 2
		_z80_Read_ar: .long _z80_Read
	


! void z80_add_write(UWORD start, UWORD end, int method, void *data)
! Add a WRITE memory handler to the memory map
! start/end = the area it covers
! method = 0 for direct, 1 for handled
! data = RAM for direct, handler for handled
.align 5
.global _z80_add_write
_z80_add_write:


	
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
	

	aw_finish:
		rts
		nop



	.align 2
	_z80_Write_aw: .long _z80_Write



! void z80_set_in(UBYTE (*handler)(UWORD port))
! Set the IN port handler to the given function
.align 5
.global _z80_set_in
_z80_set_in:
	mov.l _z80_In_si,r0
	rts
	mov.l r4,@r0

	.align 2
	_z80_In_si: .long _z80_In


! void z80_set_out(void (*handler)(UWORD port, UBYTE value))
! Set the OUT port handler to the given function
.align 5
.global _z80_set_out
_z80_set_out:
	mov.l _z80_Out_so,r0
	rts
	mov.l r4,@r0
   
	.align 2
	_z80_Out_so: .long _z80_Out


! void z80_set_reti(void (*handler)(void))
! Set the RETI handler to the given function
.align 5
.global _z80_set_reti
_z80_set_reti:
	mov.l _z80_RetI_sr,r0
	rts
	mov.l r4,@r0

	.align 2
	_z80_RetI_sr: .long _z80_RetI


! void z80_set_fetch_callback(void (*handler)(UWORD pc))
! Set the fetch callback to the given function
.align 5
.global _z80_set_fetch_callback
_z80_set_fetch_callback:
	mov.l _z80_FC_sfc,r0
	rts
	mov.l r4,@r0
   
	.align 2
	_z80_FC_sfc: .long _z80_Fetch_Callback


!----------------------------------------------------------------------------!
! the jump tables
.data
.align 5
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
.align 5

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

.align 5
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

.align 5
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

.align 5
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

.align 5
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
