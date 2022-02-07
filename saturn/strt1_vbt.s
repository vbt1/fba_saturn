!=======================================================================
!	strt1_vbt.s -- サンプルスタートモジュール１
!	Copyright(c) 1994 SEGA
!	Written by H.E on 1994-02-15 Ver.0.80
!	Updated by M.K on 1994-11-11 Ver.1.10
!Note:
!	スタックポインタの初期化、Ｃモジュール実行開始ルーチンの呼び出し、
!	およびセクションアドレステーブルの定義をする。
!=======================================================================
!	.SECTION .entry
!	.ALIGN=4
	.IMPORT	__INIT
!
!
	.globl START	
START:
!	MOV.L	STACKPTR_R,R0
	mov.l	STACKPTR_R, r15	! load stack
	MOV.L	AP_START_R,R0
	JMP	@R0
	NOP
!
	.ALIGN	4
AP_START_R:
	.long	__INIT
STACKPTR_R:
	.long	_stack

	.ALIGN	4
	.globl	__D_BGN
	.globl	__D_END
	.globl	__B_BGN
	.globl	__B_END
	
__D_BGN:
__D_END:
__B_BGN:
__B_END:
	
STARTOF_R:
	.long 	__D_BGN
ENDOF_R:
	.long   __D_END	
STARTOF_B:
	.long 	__B_BGN
ENDOF_B:
	.long 	__B_END

.END
!====== End of file ====================================================
