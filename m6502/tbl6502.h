/*****************************************************************************
 *
 *	 tbl6502.c
 *	 6502 opcode functions and function pointer table
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

#if USE_GOTO

#ifdef INLINE
#undef INLINE
#endif
#define INLINE static __inline__

/* op    temp     cycles             rdmem   opc  wrmem   ********************/
m6502_80:
m6502_02:
m6502_22:
m6502_42:
m6502_62:
m6502_82:
m6502_c2: 
m6502_e2: 
m6502_12: 
m6502_32: 
m6502_52: 
m6502_72: 
m6502_92: 
m6502_b2: 
m6502_d2: 
m6502_f2: 
m6502_03: 
m6502_23: 
m6502_43: 
m6502_63: 
m6502_83: 
m6502_a3: 
m6502_c3: 
m6502_e3: 
m6502_13:
m6502_33:
m6502_53:
m6502_73:
m6502_93:
m6502_b3: 
m6502_d3:  
m6502_f3:
m6502_04:
m6502_14:
m6502_34:
m6502_44:
m6502_54:
m6502_64:
m6502_74:
m6502_d4:
m6502_f4:
m6502_07: 
m6502_27: 
m6502_47: 
m6502_67: 
m6502_87: 
m6502_a7: 
m6502_c7: 
m6502_e7: 
m6502_17: 
m6502_37: 
m6502_57: 
m6502_77: 
m6502_97: 
m6502_b7: 
m6502_d7: 
m6502_f7: 
m6502_89:
m6502_1a: 
m6502_3a: 
m6502_5a: 
m6502_7a:  
m6502_da: 
m6502_fa: 

m6502_0b: 
m6502_2b: 
m6502_4b: 
m6502_6b: 
m6502_8b: 
m6502_ab: 
m6502_cb: 
m6502_eb: 

m6502_1b: 
m6502_3b: 
m6502_5b: 
m6502_7b: 
m6502_9b: 
m6502_bb: 
m6502_db: 
m6502_fb: 
m6502_0c: 
m6502_1c: 
m6502_3c: 
m6502_5c: 
m6502_7c: 
m6502_9c:
m6502_dc: 
m6502_fc: 
m6502_9e: 
m6502_0f: 
m6502_2f: 
m6502_4f: 
m6502_6f: 
m6502_8f: 
m6502_af: 
m6502_cf: 
m6502_ef: 

m6502_1f: 
m6502_3f: 
m6502_5f: 
m6502_7f: 
m6502_9f: 
m6502_bf: 
m6502_df: 
m6502_ff: {		  M6502_ICount -= 2;		 		  goto m6502_end; }


m6502_00: {		  M6502_ICount -= 7;		 BRK;		  goto m6502_end; }
m6502_20: {		  M6502_ICount -= 6;		 JSR;		  goto m6502_end; }
m6502_40: {		  M6502_ICount -= 6;		 RTI;		  goto m6502_end; }
m6502_60: {		  M6502_ICount -= 6;		 RTS;		  goto m6502_end; }
m6502_a0: { int tmp; M6502_ICount -= 2; RD_IMM; LDY;		  goto m6502_end; }
m6502_c0: { int tmp; M6502_ICount -= 2; RD_IMM; CPY;		  goto m6502_end; }
m6502_e0: { int tmp; M6502_ICount -= 2; RD_IMM; CPX;		  goto m6502_end; }

m6502_10: { int tmp;							 BPL;		  goto m6502_end; }
m6502_30: { int tmp;							 BMI;		  goto m6502_end; }
m6502_50: { int tmp;							 BVC;		  goto m6502_end; }
m6502_70: { int tmp;							 BVS;		  goto m6502_end; }
m6502_90: { int tmp;							 BCC;		  goto m6502_end; }
m6502_b0: { int tmp;							 BCS;		  goto m6502_end; }
m6502_d0: { int tmp;							 BNE;		  goto m6502_end; }
m6502_f0: { int tmp;							 BEQ;		  goto m6502_end; }

m6502_01: { int tmp; M6502_ICount -= 6; RD_IDX; ORA;		  goto m6502_end; }
m6502_21: { int tmp; M6502_ICount -= 6; RD_IDX; AND;		  goto m6502_end; }
m6502_41: { int tmp; M6502_ICount -= 6; RD_IDX; EOR;		  goto m6502_end; }
m6502_61: { int tmp; M6502_ICount -= 6; RD_IDX; ADC;		  goto m6502_end; }
m6502_81: { int tmp; M6502_ICount -= 6;		 STA; WR_IDX; goto m6502_end; }
m6502_a1: { int tmp; M6502_ICount -= 6; RD_IDX; LDA;		  goto m6502_end; }
m6502_c1: { int tmp; M6502_ICount -= 6; RD_IDX; CMP;		  goto m6502_end; }
m6502_e1: { int tmp; M6502_ICount -= 6; RD_IDX; SBC;		  goto m6502_end; }

m6502_11: { int tmp; M6502_ICount -= 5; RD_IDY; ORA;		  goto m6502_end; }
m6502_31: { int tmp; M6502_ICount -= 5; RD_IDY; AND;		  goto m6502_end; }
m6502_51: { int tmp; M6502_ICount -= 5; RD_IDY; EOR;		  goto m6502_end; }
m6502_71: { int tmp; M6502_ICount -= 5; RD_IDY; ADC;		  goto m6502_end; }
m6502_91: { int tmp; M6502_ICount -= 6;		 STA; WR_IDY; goto m6502_end; }
m6502_b1: { int tmp; M6502_ICount -= 5; RD_IDY; LDA;		  goto m6502_end; }
m6502_d1: { int tmp; M6502_ICount -= 5; RD_IDY; CMP;		  goto m6502_end; }
m6502_f1: { int tmp; M6502_ICount -= 5; RD_IDY; SBC;		  goto m6502_end; }

m6502_a2: { int tmp; M6502_ICount -= 2; RD_IMM; LDX;		  goto m6502_end; }
m6502_24: { int tmp; M6502_ICount -= 3; RD_ZPG; BIT;		  goto m6502_end; }

m6502_84: { int tmp; M6502_ICount -= 3;		 STY; WR_ZPG; goto m6502_end; }
m6502_a4: { int tmp; M6502_ICount -= 3; RD_ZPG; LDY;		  goto m6502_end; }
m6502_c4: { int tmp; M6502_ICount -= 3; RD_ZPG; CPY;		  goto m6502_end; }
m6502_e4: { int tmp; M6502_ICount -= 3; RD_ZPG; CPX;		  goto m6502_end; }

m6502_94: { int tmp; M6502_ICount -= 4;		 STY; WR_ZPX; goto m6502_end; }
m6502_b4: { int tmp; M6502_ICount -= 4; RD_ZPX; LDY;		  goto m6502_end; }

m6502_05: { int tmp; M6502_ICount -= 3; RD_ZPG; ORA;		  goto m6502_end; }
m6502_25: { int tmp; M6502_ICount -= 3; RD_ZPG; AND;		  goto m6502_end; }
m6502_45: { int tmp; M6502_ICount -= 3; RD_ZPG; EOR;		  goto m6502_end; }
m6502_65: { int tmp; M6502_ICount -= 3; RD_ZPG; ADC;		  goto m6502_end; }
m6502_85: { int tmp; M6502_ICount -= 3;		 STA; WR_ZPG; goto m6502_end; }
m6502_a5: { int tmp; M6502_ICount -= 3; RD_ZPG; LDA;		  goto m6502_end; }
m6502_c5: { int tmp; M6502_ICount -= 3; RD_ZPG; CMP;		  goto m6502_end; }
m6502_e5: { int tmp; M6502_ICount -= 3; RD_ZPG; SBC;		  goto m6502_end; }

m6502_15: { int tmp; M6502_ICount -= 4; RD_ZPX; ORA;		  goto m6502_end; }
m6502_35: { int tmp; M6502_ICount -= 4; RD_ZPX; AND;		  goto m6502_end; }
m6502_55: { int tmp; M6502_ICount -= 4; RD_ZPX; EOR;		  goto m6502_end; }
m6502_75: { int tmp; M6502_ICount -= 4; RD_ZPX; ADC;		  goto m6502_end; }
m6502_95: { int tmp; M6502_ICount -= 4;		 STA; WR_ZPX; goto m6502_end; }
m6502_b5: { int tmp; M6502_ICount -= 4; RD_ZPX; LDA;		  goto m6502_end; }
m6502_d5: { int tmp; M6502_ICount -= 4; RD_ZPX; CMP;		  goto m6502_end; }
m6502_f5: { int tmp; M6502_ICount -= 4; RD_ZPX; SBC;		  goto m6502_end; }

m6502_06: { int tmp; M6502_ICount -= 5; RD_ZPG; ASL; WB_EA;  goto m6502_end; }
m6502_26: { int tmp; M6502_ICount -= 5; RD_ZPG; ROL; WB_EA;  goto m6502_end; }
m6502_46: { int tmp; M6502_ICount -= 5; RD_ZPG; LSR; WB_EA;  goto m6502_end; }
m6502_66: { int tmp; M6502_ICount -= 5; RD_ZPG; ROR; WB_EA;  goto m6502_end; }
m6502_86: { int tmp; M6502_ICount -= 3;		 STX; WR_ZPG; goto m6502_end; }
m6502_a6: { int tmp; M6502_ICount -= 3; RD_ZPG; LDX;		  goto m6502_end; }
m6502_c6: { int tmp; M6502_ICount -= 5; RD_ZPG; DEC; WB_EA;  goto m6502_end; }
m6502_e6: { int tmp; M6502_ICount -= 5; RD_ZPG; INC; WB_EA;  goto m6502_end; }

m6502_16: { int tmp; M6502_ICount -= 6; RD_ZPX; ASL; WB_EA;  goto m6502_end; }
m6502_36: { int tmp; M6502_ICount -= 6; RD_ZPX; ROL; WB_EA;  goto m6502_end; }
m6502_56: { int tmp; M6502_ICount -= 6; RD_ZPX; LSR; WB_EA;  goto m6502_end; }
m6502_76: { int tmp; M6502_ICount -= 6; RD_ZPX; ROR; WB_EA;  goto m6502_end; }
m6502_96: { int tmp; M6502_ICount -= 4;		 STX; WR_ZPY; goto m6502_end; }
m6502_b6: { int tmp; M6502_ICount -= 4; RD_ZPY; LDX;		  goto m6502_end; }
m6502_d6: { int tmp; M6502_ICount -= 6; RD_ZPX; DEC; WB_EA;  goto m6502_end; }
m6502_f6: { int tmp; M6502_ICount -= 6; RD_ZPX; INC; WB_EA;  goto m6502_end; }

m6502_08: {		  M6502_ICount -= 2;		 PHP;		  goto m6502_end; }
m6502_28: {		  M6502_ICount -= 2;		 PLP;		  goto m6502_end; }
m6502_48: {		  M6502_ICount -= 2;		 PHA;		  goto m6502_end; }
m6502_68: {		  M6502_ICount -= 2;		 PLA;		  goto m6502_end; }
m6502_88: {		  M6502_ICount -= 2;		 DEY;		  goto m6502_end; }
m6502_a8: {		  M6502_ICount -= 2;		 TAY;		  goto m6502_end; }
m6502_c8: {		  M6502_ICount -= 2;		 INY;		  goto m6502_end; }
m6502_e8: {		  M6502_ICount -= 2;		 INX;		  goto m6502_end; }

m6502_18: {		  M6502_ICount -= 2;		 CLC;		  goto m6502_end; }
m6502_38: {		  M6502_ICount -= 2;		 SEC;		  goto m6502_end; }
m6502_58: {		  M6502_ICount -= 2;		 CLI;		  goto m6502_end; }
m6502_78: {		  M6502_ICount -= 2;		 SEI;		  goto m6502_end; }
m6502_98: {		  M6502_ICount -= 2;		 TYA;		  goto m6502_end; }
m6502_b8: {		  M6502_ICount -= 2;		 CLV;		  goto m6502_end; }
m6502_d8: {		  M6502_ICount -= 2;		 CLD;		  goto m6502_end; }
m6502_f8: {		  M6502_ICount -= 2;		 SED;		  goto m6502_end; }

m6502_09: { int tmp; M6502_ICount -= 2; RD_IMM; ORA;		  goto m6502_end; }
m6502_29: { int tmp; M6502_ICount -= 2; RD_IMM; AND;		  goto m6502_end; }
m6502_49: { int tmp; M6502_ICount -= 2; RD_IMM; EOR;		  goto m6502_end; }
m6502_69: { int tmp; M6502_ICount -= 2; RD_IMM; ADC;		  goto m6502_end; }
m6502_a9: { int tmp; M6502_ICount -= 2; RD_IMM; LDA;		  goto m6502_end; }
m6502_c9: { int tmp; M6502_ICount -= 2; RD_IMM; CMP;		  goto m6502_end; }
m6502_e9: { int tmp; M6502_ICount -= 2; RD_IMM; SBC;		  goto m6502_end; }

m6502_19: { int tmp; M6502_ICount -= 4; RD_ABY; ORA;		  goto m6502_end; }
m6502_39: { int tmp; M6502_ICount -= 4; RD_ABY; AND;		  goto m6502_end; }
m6502_59: { int tmp; M6502_ICount -= 4; RD_ABY; EOR;		  goto m6502_end; }
m6502_79: { int tmp; M6502_ICount -= 4; RD_ABY; ADC;		  goto m6502_end; }
m6502_99: { int tmp; M6502_ICount -= 5;		 STA; WR_ABY; goto m6502_end; }
m6502_b9: { int tmp; M6502_ICount -= 4; RD_ABY; LDA;		  goto m6502_end; }
m6502_d9: { int tmp; M6502_ICount -= 4; RD_ABY; CMP;		  goto m6502_end; }
m6502_f9: { int tmp; M6502_ICount -= 4; RD_ABY; SBC;		  goto m6502_end; }

m6502_0a: { int tmp; M6502_ICount -= 2; RD_ACC; ASL; WB_ACC; goto m6502_end; }
m6502_2a: { int tmp; M6502_ICount -= 2; RD_ACC; ROL; WB_ACC; goto m6502_end; }
m6502_4a: { int tmp; M6502_ICount -= 2; RD_ACC; LSR; WB_ACC; goto m6502_end; }
m6502_6a: { int tmp; M6502_ICount -= 2; RD_ACC; ROR; WB_ACC; goto m6502_end; }
m6502_8a: {		  M6502_ICount -= 2;		 TXA;		  goto m6502_end; }
m6502_aa: {		  M6502_ICount -= 2;		 TAX;		  goto m6502_end; }
m6502_ca: {		  M6502_ICount -= 2;		 DEX;		  goto m6502_end; }
m6502_ea: {		  M6502_ICount -= 2;		 NOP;		  goto m6502_end; }


m6502_9a: {		  M6502_ICount -= 2;		 TXS;		  goto m6502_end; }
m6502_ba: {		  M6502_ICount -= 2;		 TSX;		  goto m6502_end; }

m6502_2c: { int tmp; M6502_ICount -= 4; RD_ABS; BIT;		  goto m6502_end; }
m6502_4c: {		  M6502_ICount -= 3; EA_ABS; JMP;		  goto m6502_end; }
m6502_6c: { int tmp; M6502_ICount -= 5; EA_IND; JMP;		  goto m6502_end; }
m6502_8c: { int tmp; M6502_ICount -= 4;		 STY; WR_ABS; goto m6502_end; }
m6502_ac: { int tmp; M6502_ICount -= 4; RD_ABS; LDY;		  goto m6502_end; }
m6502_cc: { int tmp; M6502_ICount -= 4; RD_ABS; CPY;		  goto m6502_end; }
m6502_ec: { int tmp; M6502_ICount -= 4; RD_ABS; CPX;		  goto m6502_end; }

m6502_bc: { int tmp; M6502_ICount -= 4; RD_ABX; LDY;		  goto m6502_end; }

m6502_0d: { int tmp; M6502_ICount -= 4; RD_ABS; ORA;		  goto m6502_end; }
m6502_2d: { int tmp; M6502_ICount -= 4; RD_ABS; AND;		  goto m6502_end; }
m6502_4d: { int tmp; M6502_ICount -= 4; RD_ABS; EOR;		  goto m6502_end; }
m6502_6d: { int tmp; M6502_ICount -= 4; RD_ABS; ADC;		  goto m6502_end; }
m6502_8d: { int tmp; M6502_ICount -= 4;		 STA; WR_ABS; goto m6502_end; }
m6502_ad: { int tmp; M6502_ICount -= 4; RD_ABS; LDA;		  goto m6502_end; }
m6502_cd: { int tmp; M6502_ICount -= 4; RD_ABS; CMP;		  goto m6502_end; }
m6502_ed: { int tmp; M6502_ICount -= 4; RD_ABS; SBC;		  goto m6502_end; }

m6502_1d: { int tmp; M6502_ICount -= 4; RD_ABX; ORA;		  goto m6502_end; }
m6502_3d: { int tmp; M6502_ICount -= 4; RD_ABX; AND;		  goto m6502_end; }
m6502_5d: { int tmp; M6502_ICount -= 4; RD_ABX; EOR;		  goto m6502_end; }
m6502_7d: { int tmp; M6502_ICount -= 4; RD_ABX; ADC;		  goto m6502_end; }
m6502_9d: { int tmp; M6502_ICount -= 5;		 STA; WR_ABX; goto m6502_end; }
m6502_bd: { int tmp; M6502_ICount -= 4; RD_ABX; LDA;		  goto m6502_end; }
m6502_dd: { int tmp; M6502_ICount -= 4; RD_ABX; CMP;		  goto m6502_end; }
m6502_fd: { int tmp; M6502_ICount -= 4; RD_ABX; SBC;		  goto m6502_end; }

m6502_0e: { int tmp; M6502_ICount -= 6; RD_ABS; ASL; WB_EA;  goto m6502_end; }
m6502_2e: { int tmp; M6502_ICount -= 6; RD_ABS; ROL; WB_EA;  goto m6502_end; }
m6502_4e: { int tmp; M6502_ICount -= 6; RD_ABS; LSR; WB_EA;  goto m6502_end; }
m6502_6e: { int tmp; M6502_ICount -= 6; RD_ABS; ROR; WB_EA;  goto m6502_end; }
m6502_8e: { int tmp; M6502_ICount -= 5;		 STX; WR_ABS; goto m6502_end; }
m6502_ae: { int tmp; M6502_ICount -= 4; RD_ABS; LDX;		  goto m6502_end; }
m6502_ce: { int tmp; M6502_ICount -= 6; RD_ABS; DEC; WB_EA;  goto m6502_end; }
m6502_ee: { int tmp; M6502_ICount -= 6; RD_ABS; INC; WB_EA;  goto m6502_end; }

m6502_1e: { int tmp; M6502_ICount -= 7; RD_ABX; ASL; WB_EA;  goto m6502_end; }
m6502_3e: { int tmp; M6502_ICount -= 7; RD_ABX; ROL; WB_EA;  goto m6502_end; }
m6502_5e: { int tmp; M6502_ICount -= 7; RD_ABX; LSR; WB_EA;  goto m6502_end; }
m6502_7e: { int tmp; M6502_ICount -= 7; RD_ABX; ROR; WB_EA;  goto m6502_end; }
m6502_be: { int tmp; M6502_ICount -= 4; RD_ABY; LDX;		  goto m6502_end; }
m6502_de: { int tmp; M6502_ICount -= 7; RD_ABX; DEC; WB_EA;  goto m6502_end; }
m6502_fe: { int tmp; M6502_ICount -= 7; RD_ABX; INC; WB_EA;  goto m6502_end; }

#else

//#ifdef DREAMCAST
#ifdef INLINE
#undef INLINE
#endif
#define INLINE static __inline__
//#endif

#undef	OP
#define OP(nn) INLINE void m6502_##nn(void)

/*****************************************************************************
 *****************************************************************************
 *
 *   plain vanilla 6502 opcodes
 *
 *****************************************************************************
 * op	 temp	  cycles			 rdmem	 opc  wrmem   ********************/

OP(00) {		  M6502_ICount -= 7;		 BRK;		  } 
OP(20) {		  M6502_ICount -= 6;		 JSR;		  } 
OP(40) {		  M6502_ICount -= 6;		 RTI;		  } 
OP(60) {		  M6502_ICount -= 6;		 RTS;		  } 
OP(80) {		  M6502_ICount -= 2;		 		  } 
OP(a0) { int tmp; M6502_ICount -= 2; RD_IMM; LDY;		  } 
OP(c0) { int tmp; M6502_ICount -= 2; RD_IMM; CPY;		  } 
OP(e0) { int tmp; M6502_ICount -= 2; RD_IMM; CPX;		  } 

OP(10) { int tmp;							 BPL;		  } 
OP(30) { int tmp;							 BMI;		  } 
OP(50) { int tmp;							 BVC;		  } 
OP(70) { int tmp;							 BVS;		  } 
OP(90) { int tmp;							 BCC;		  } 
OP(b0) { int tmp;							 BCS;		  } 
OP(d0) { int tmp;							 BNE;		  } 
OP(f0) { int tmp;							 BEQ;		  } 

OP(01) { int tmp; M6502_ICount -= 6; RD_IDX; ORA;		  } 
OP(21) { int tmp; M6502_ICount -= 6; RD_IDX; AND;		  } 
OP(41) { int tmp; M6502_ICount -= 6; RD_IDX; EOR;		  } 
OP(61) { int tmp; M6502_ICount -= 6; RD_IDX; ADC;		  } 
OP(81) { int tmp; M6502_ICount -= 6;		 STA; WR_IDX; } 
OP(a1) { int tmp; M6502_ICount -= 6; RD_IDX; LDA;		  } 
OP(c1) { int tmp; M6502_ICount -= 6; RD_IDX; CMP;		  } 
OP(e1) { int tmp; M6502_ICount -= 6; RD_IDX; SBC;		  } 

OP(11) { int tmp; M6502_ICount -= 5; RD_IDY; ORA;		  } 
OP(31) { int tmp; M6502_ICount -= 5; RD_IDY; AND;		  } 
OP(51) { int tmp; M6502_ICount -= 5; RD_IDY; EOR;		  } 
OP(71) { int tmp; M6502_ICount -= 5; RD_IDY; ADC;		  } 
OP(91) { int tmp; M6502_ICount -= 6;		 STA; WR_IDY; } 
OP(b1) { int tmp; M6502_ICount -= 5; RD_IDY; LDA;		  } 
OP(d1) { int tmp; M6502_ICount -= 5; RD_IDY; CMP;		  } 
OP(f1) { int tmp; M6502_ICount -= 5; RD_IDY; SBC;		  } 

OP(02) {		  M6502_ICount -= 2;		 		  } 
OP(22) {		  M6502_ICount -= 2;		 		  } 
OP(42) {		  M6502_ICount -= 2;		 		  } 
OP(62) {		  M6502_ICount -= 2;		 		  } 
OP(82) {		  M6502_ICount -= 2;		 		  } 
OP(a2) { int tmp; M6502_ICount -= 2; RD_IMM; LDX;		  } 
OP(c2) {		  M6502_ICount -= 2;		 		  } 
OP(e2) {		  M6502_ICount -= 2;		 		  } 

OP(12) {		  M6502_ICount -= 2;		 		  } 
OP(32) {		  M6502_ICount -= 2;		 		  } 
OP(52) {		  M6502_ICount -= 2;		 		  } 
OP(72) {		  M6502_ICount -= 2;		 		  } 
OP(92) {		  M6502_ICount -= 2;		 		  } 
OP(b2) {		  M6502_ICount -= 2;		 		  } 
OP(d2) {		  M6502_ICount -= 2;		 		  } 
OP(f2) {		  M6502_ICount -= 2;		 		  } 

OP(03) {		  M6502_ICount -= 2;		 		  } 
OP(23) {		  M6502_ICount -= 2;		 		  } 
OP(43) {		  M6502_ICount -= 2;		 		  } 
OP(63) {		  M6502_ICount -= 2;		 		  } 
OP(83) {		  M6502_ICount -= 2;		 		  } 
OP(a3) {		  M6502_ICount -= 2;		 		  }  
OP(c3) {		  M6502_ICount -= 2;		 		  }  
OP(e3) {		  M6502_ICount -= 2;		 		  }  

OP(13) {		  M6502_ICount -= 2;		 		  }  
OP(33) {		  M6502_ICount -= 2;		 		  }  
OP(53) {		  M6502_ICount -= 2;		 		  }  
OP(73) {		  M6502_ICount -= 2;		 		  }  
OP(93) {		  M6502_ICount -= 2;		 		  }  
OP(b3) {		  M6502_ICount -= 2;		 		  }  
OP(d3) {		  M6502_ICount -= 2;		 		  }  
OP(f3) {		  M6502_ICount -= 2;		 		  }  

OP(04) {		  M6502_ICount -= 2;		 		  }  
OP(24) { int tmp; M6502_ICount -= 3; RD_ZPG; BIT;		  } 
OP(44) {		  M6502_ICount -= 2;		 		  }  
OP(64) {		  M6502_ICount -= 2;		 		  }  
OP(84) { int tmp; M6502_ICount -= 3;		 STY; WR_ZPG; } 
OP(a4) { int tmp; M6502_ICount -= 3; RD_ZPG; LDY;		  } 
OP(c4) { int tmp; M6502_ICount -= 3; RD_ZPG; CPY;		  } 
OP(e4) { int tmp; M6502_ICount -= 3; RD_ZPG; CPX;		  } 

OP(14) {		  M6502_ICount -= 2;		 		  }  
OP(34) {		  M6502_ICount -= 2;		 		  }  
OP(54) {		  M6502_ICount -= 2;		 		  }  
OP(74) {		  M6502_ICount -= 2;		 		  }  
OP(94) { int tmp; M6502_ICount -= 4;		 STY; WR_ZPX; } 
OP(b4) { int tmp; M6502_ICount -= 4; RD_ZPX; LDY;		  } 
OP(d4) {		  M6502_ICount -= 2;		 		  }  
OP(f4) {		  M6502_ICount -= 2;		 		  }  

OP(05) { int tmp; M6502_ICount -= 3; RD_ZPG; ORA;		  } 
OP(25) { int tmp; M6502_ICount -= 3; RD_ZPG; AND;		  } 
OP(45) { int tmp; M6502_ICount -= 3; RD_ZPG; EOR;		  } 
OP(65) { int tmp; M6502_ICount -= 3; RD_ZPG; ADC;		  } 
OP(85) { int tmp; M6502_ICount -= 3;		 STA; WR_ZPG; } 
OP(a5) { int tmp; M6502_ICount -= 3; RD_ZPG; LDA;		  } 
OP(c5) { int tmp; M6502_ICount -= 3; RD_ZPG; CMP;		  } 
OP(e5) { int tmp; M6502_ICount -= 3; RD_ZPG; SBC;		  } 

OP(15) { int tmp; M6502_ICount -= 4; RD_ZPX; ORA;		  } 
OP(35) { int tmp; M6502_ICount -= 4; RD_ZPX; AND;		  } 
OP(55) { int tmp; M6502_ICount -= 4; RD_ZPX; EOR;		  } 
OP(75) { int tmp; M6502_ICount -= 4; RD_ZPX; ADC;		  } 
OP(95) { int tmp; M6502_ICount -= 4;		 STA; WR_ZPX; } 
OP(b5) { int tmp; M6502_ICount -= 4; RD_ZPX; LDA;		  } 
OP(d5) { int tmp; M6502_ICount -= 4; RD_ZPX; CMP;		  } 
OP(f5) { int tmp; M6502_ICount -= 4; RD_ZPX; SBC;		  } 

//OP(06_old) { int tmp; M6502_ICount -= 5; RD_ZPG; ASL; WB_EA;  } 
OP(06) { int tmp; M6502_ICount -= 5; RD_ZPG; ASL; WB_EA;  } 

OP(26) { int tmp; M6502_ICount -= 5; RD_ZPG; ROL; WB_EA;  } 
OP(46) { int tmp; M6502_ICount -= 5; RD_ZPG; LSR; WB_EA;  } 
OP(66) { int tmp; M6502_ICount -= 5; RD_ZPG; ROR; WB_EA;  } 
OP(86) { int tmp; M6502_ICount -= 3;		 STX; WR_ZPG; } 
OP(a6) { int tmp; M6502_ICount -= 3; RD_ZPG; LDX;		  } 
OP(c6) { int tmp; M6502_ICount -= 5; RD_ZPG; DEC; WB_EA;  } 
OP(e6) { int tmp; M6502_ICount -= 5; RD_ZPG; INC; WB_EA;  } 

//OP(16_old) { int tmp; M6502_ICount -= 6; RD_ZPX; ASL; WB_EA;  } 
OP(16) { int tmp; M6502_ICount -= 6; RD_ZPX; ASL; WB_EA;  } 

OP(36) { int tmp; M6502_ICount -= 6; RD_ZPX; ROL; WB_EA;  } 
OP(56) { int tmp; M6502_ICount -= 6; RD_ZPX; LSR; WB_EA;  } 
OP(76) { int tmp; M6502_ICount -= 6; RD_ZPX; ROR; WB_EA;  } 
OP(96) { int tmp; M6502_ICount -= 4;		 STX; WR_ZPY; } 
OP(b6) { int tmp; M6502_ICount -= 4; RD_ZPY; LDX;		  } 
OP(d6) { int tmp; M6502_ICount -= 6; RD_ZPX; DEC; WB_EA;  } 
OP(f6) { int tmp; M6502_ICount -= 6; RD_ZPX; INC; WB_EA;  } 

OP(07) {		  M6502_ICount -= 2;		 		  }  
OP(27) {		  M6502_ICount -= 2;		 		  }  
OP(47) {		  M6502_ICount -= 2;		 		  }  
OP(67) {		  M6502_ICount -= 2;		 		  }  
OP(87) {		  M6502_ICount -= 2;		 		  }  
OP(a7) {		  M6502_ICount -= 2;		 		  }  
OP(c7) {		  M6502_ICount -= 2;		 		  }  
OP(e7) {		  M6502_ICount -= 2;		 		  }  

OP(17) {		  M6502_ICount -= 2;		 		  }  
OP(37) {		  M6502_ICount -= 2;		 		  }  
OP(57) {		  M6502_ICount -= 2;		 		  }  
OP(77) {		  M6502_ICount -= 2;		 		  }  
OP(97) {		  M6502_ICount -= 2;		 		  }  
OP(b7) {		  M6502_ICount -= 2;		 		  }  
OP(d7) {		  M6502_ICount -= 2;		 		  }  
OP(f7) {		  M6502_ICount -= 2;		 		  }  

OP(08) {		  M6502_ICount -= 2;		 PHP;		  } 
OP(28) {		  M6502_ICount -= 2;		 PLP;		  } 
OP(48) {		  M6502_ICount -= 2;		 PHA;		  } 
OP(68) {		  M6502_ICount -= 2;		 PLA;		  } 
OP(88) {		  M6502_ICount -= 2;		 DEY;		  } 
OP(a8) {		  M6502_ICount -= 2;		 TAY;		  } 
OP(c8) {		  M6502_ICount -= 2;		 INY;		  } 
OP(e8) {		  M6502_ICount -= 2;		 INX;		  } 

OP(18) {		  M6502_ICount -= 2;		 CLC;		  } 
OP(38) {		  M6502_ICount -= 2;		 SEC;		  } 
OP(58) {		  M6502_ICount -= 2;		 CLI;		  } 
OP(78) {		  M6502_ICount -= 2;		 SEI;		  } 
OP(98) {		  M6502_ICount -= 2;		 TYA;		  } 
OP(b8) {		  M6502_ICount -= 2;		 CLV;		  } 
OP(d8) {		  M6502_ICount -= 2;		 CLD;		  } 
OP(f8) {		  M6502_ICount -= 2;		 SED;		  } 

OP(09) { int tmp; M6502_ICount -= 2; RD_IMM; ORA;		  } 
OP(29) { int tmp; M6502_ICount -= 2; RD_IMM; AND;		  } 
OP(49) { int tmp; M6502_ICount -= 2; RD_IMM; EOR;		  } 
OP(69) { int tmp; M6502_ICount -= 2; RD_IMM; ADC;		  } 
OP(89) {		  M6502_ICount -= 2;		 		  }  
OP(a9) { int tmp; M6502_ICount -= 2; RD_IMM; LDA;		  } 
OP(c9) { int tmp; M6502_ICount -= 2; RD_IMM; CMP;		  } 
OP(e9) { int tmp; M6502_ICount -= 2; RD_IMM; SBC;		  } 

OP(19) { int tmp; M6502_ICount -= 4; RD_ABY; ORA;		  } 
OP(39) { int tmp; M6502_ICount -= 4; RD_ABY; AND;		  } 
OP(59) { int tmp; M6502_ICount -= 4; RD_ABY; EOR;		  } 
OP(79) { int tmp; M6502_ICount -= 4; RD_ABY; ADC;		  } 
OP(99) { int tmp; M6502_ICount -= 5;		 STA; WR_ABY; } 
OP(b9) { int tmp; M6502_ICount -= 4; RD_ABY; LDA;		  } 
OP(d9) { int tmp; M6502_ICount -= 4; RD_ABY; CMP;		  } 
OP(f9) { int tmp; M6502_ICount -= 4; RD_ABY; SBC;		  } 

//OP(0a_old) { int tmp; M6502_ICount -= 2; RD_ACC; ASL; WB_ACC; }
OP(0a) { int tmp; M6502_ICount -= 2; RD_ACC; ASL; WB_ACC; }

OP(2a) { int tmp; M6502_ICount -= 2; RD_ACC; ROL; WB_ACC; } 
OP(4a) { int tmp; M6502_ICount -= 2; RD_ACC; LSR; WB_ACC; } 
OP(6a) { int tmp; M6502_ICount -= 2; RD_ACC; ROR; WB_ACC; } 
OP(8a) {		  M6502_ICount -= 2;		 TXA;		  } 
OP(aa) {		  M6502_ICount -= 2;		 TAX;		  } 
OP(ca) {		  M6502_ICount -= 2;		 DEX;		  } 
OP(ea) {		  M6502_ICount -= 2;		 NOP;		  } 

OP(1a) {		  M6502_ICount -= 2;		 		  }  
OP(3a) {		  M6502_ICount -= 2;		 		  }  
OP(5a) {		  M6502_ICount -= 2;		 		  }  
OP(7a) {		  M6502_ICount -= 2;		 		  }  
OP(9a) {		  M6502_ICount -= 2;		 TXS;		  } 
OP(ba) {		  M6502_ICount -= 2;		 TSX;		  } 
OP(da) {		  M6502_ICount -= 2;		 		  }  
OP(fa) {		  M6502_ICount -= 2;		 		  }  

OP(0b) {		  M6502_ICount -= 2;		 		  }  
OP(2b) {		  M6502_ICount -= 2;		 		  }  
OP(4b) {		  M6502_ICount -= 2;		 		  }  
OP(6b) {		  M6502_ICount -= 2;		 		  }  
OP(8b) {		  M6502_ICount -= 2;		 		  }  
OP(ab) {		  M6502_ICount -= 2;		 		  }  
OP(cb) {		  M6502_ICount -= 2;		 		  }  
OP(eb) {		  M6502_ICount -= 2;		 		  }  

OP(1b) {		  M6502_ICount -= 2;		 		  }  
OP(3b) {		  M6502_ICount -= 2;		 		  }  
OP(5b) {		  M6502_ICount -= 2;		 		  }  
OP(7b) {		  M6502_ICount -= 2;		 		  }  
OP(9b) {		  M6502_ICount -= 2;		 		  }  
OP(bb) {		  M6502_ICount -= 2;		 		  }  
OP(db) {		  M6502_ICount -= 2;		 		  }  
OP(fb) {		  M6502_ICount -= 2;		 		  }  

OP(0c) {		  M6502_ICount -= 2;		 		  }  
OP(2c) { int tmp; M6502_ICount -= 4; RD_ABS; BIT;		  } 
OP(4c) {		  M6502_ICount -= 3; EA_ABS; JMP;		  } 
OP(6c) { int tmp; M6502_ICount -= 5; EA_IND; JMP;		  } 
OP(8c) { int tmp; M6502_ICount -= 4;		 STY; WR_ABS; } 
OP(ac) { int tmp; M6502_ICount -= 4; RD_ABS; LDY;		  } 
OP(cc) { int tmp; M6502_ICount -= 4; RD_ABS; CPY;		  } 
OP(ec) { int tmp; M6502_ICount -= 4; RD_ABS; CPX;		  } 

OP(1c) {		  M6502_ICount -= 2;		 		  }  
OP(3c) {		  M6502_ICount -= 2;		 		  }  
OP(5c) {		  M6502_ICount -= 2;		 		  }  
OP(7c) {		  M6502_ICount -= 2;		 		  }  
OP(9c) {		  M6502_ICount -= 2;		 		  }  
OP(bc) { int tmp; M6502_ICount -= 4; RD_ABX; LDY;		  } 
OP(dc) {		  M6502_ICount -= 2;		 		  }  
OP(fc) {		  M6502_ICount -= 2;		 		  }  

OP(0d) { int tmp; M6502_ICount -= 4; RD_ABS; ORA;		  } 
OP(2d) { int tmp; M6502_ICount -= 4; RD_ABS; AND;		  } 
OP(4d) { int tmp; M6502_ICount -= 4; RD_ABS; EOR;		  } 
OP(6d) { int tmp; M6502_ICount -= 4; RD_ABS; ADC;		  } 
OP(8d) { int tmp; M6502_ICount -= 4;		 STA; WR_ABS; } 
OP(ad) { int tmp; M6502_ICount -= 4; RD_ABS; LDA;		  } 
OP(cd) { int tmp; M6502_ICount -= 4; RD_ABS; CMP;		  } 
OP(ed) { int tmp; M6502_ICount -= 4; RD_ABS; SBC;		  } 

OP(1d) { int tmp; M6502_ICount -= 4; RD_ABX; ORA;		  } 
OP(3d) { int tmp; M6502_ICount -= 4; RD_ABX; AND;		  } 
OP(5d) { int tmp; M6502_ICount -= 4; RD_ABX; EOR;		  } 
OP(7d) { int tmp; M6502_ICount -= 4; RD_ABX; ADC;		  } 
OP(9d) { int tmp; M6502_ICount -= 5;		 STA; WR_ABX; } 
OP(bd) { int tmp; M6502_ICount -= 4; RD_ABX; LDA;		  } 
OP(dd) { int tmp; M6502_ICount -= 4; RD_ABX; CMP;		  } 
OP(fd) { int tmp; M6502_ICount -= 4; RD_ABX; SBC;		  } 
//OP(0e_old) { int tmp; M6502_ICount -= 6; RD_ABS; ASL; WB_EA;  } 
OP(0e) { int tmp; M6502_ICount -= 6; RD_ABS; ASL; WB_EA;  } 
OP(2e) { int tmp; M6502_ICount -= 6; RD_ABS; ROL; WB_EA;  } 
OP(4e) { int tmp; M6502_ICount -= 6; RD_ABS; LSR; WB_EA;  } 
OP(6e) { int tmp; M6502_ICount -= 6; RD_ABS; ROR; WB_EA;  } 
OP(8e) { int tmp; M6502_ICount -= 5;		 STX; WR_ABS; } 
OP(ae) { int tmp; M6502_ICount -= 4; RD_ABS; LDX;		  } 
OP(ce) { int tmp; M6502_ICount -= 6; RD_ABS; DEC; WB_EA;  } 
OP(ee) { int tmp; M6502_ICount -= 6; RD_ABS; INC; WB_EA;  } 

//OP(1e_old) { int tmp; M6502_ICount -= 7; RD_ABX; ASL; WB_EA;  } 
OP(1e) { int tmp; M6502_ICount -= 7; RD_ABX; ASL; WB_EA;  } 

OP(3e) { int tmp; M6502_ICount -= 7; RD_ABX; ROL; WB_EA;  } 
OP(5e) { int tmp; M6502_ICount -= 7; RD_ABX; LSR; WB_EA;  } 
OP(7e) { int tmp; M6502_ICount -= 7; RD_ABX; ROR; WB_EA;  } 
OP(9e) {		  M6502_ICount -= 2;		 		  }  
OP(be) { int tmp; M6502_ICount -= 4; RD_ABY; LDX;		  } 
OP(de) { int tmp; M6502_ICount -= 7; RD_ABX; DEC; WB_EA;  } 
OP(fe) { int tmp; M6502_ICount -= 7; RD_ABX; INC; WB_EA;  } 

OP(0f) {		  M6502_ICount -= 2;		 		  }  
OP(2f) {		  M6502_ICount -= 2;		 		  }  
OP(4f) {		  M6502_ICount -= 2;		 		  }  
OP(6f) {		  M6502_ICount -= 2;		 		  }  
OP(8f) {		  M6502_ICount -= 2;		 		  }  
OP(af) {		  M6502_ICount -= 2;		 		  }  
OP(cf) {		  M6502_ICount -= 2;		 		  }  
OP(ef) {		  M6502_ICount -= 2;		 		  }  

OP(1f) {		  M6502_ICount -= 2;		 		  }  
OP(3f) {		  M6502_ICount -= 2;		 		  }  
OP(5f) {		  M6502_ICount -= 2;		 		  }  
OP(7f) {		  M6502_ICount -= 2;		 		  }  
OP(9f) {		  M6502_ICount -= 2;		 		  }  
OP(bf) {		  M6502_ICount -= 2;		 		  }  
OP(df) {		  M6502_ICount -= 2;		 		  }  
OP(ff) {		  M6502_ICount -= 2;		 		  }  

/* and here's the array of function pointers */

static void (*insn6502[0x100])(void) = {
	m6502_00,m6502_01,m6502_02,m6502_03,m6502_04,m6502_05,m6502_06,m6502_07,
	m6502_08,m6502_09,m6502_0a,m6502_0b,m6502_0c,m6502_0d,m6502_0e,m6502_0f,
	m6502_10,m6502_11,m6502_12,m6502_13,m6502_14,m6502_15,m6502_16,m6502_17,
	m6502_18,m6502_19,m6502_1a,m6502_1b,m6502_1c,m6502_1d,m6502_1e,m6502_1f,
	m6502_20,m6502_21,m6502_22,m6502_23,m6502_24,m6502_25,m6502_26,m6502_27,
	m6502_28,m6502_29,m6502_2a,m6502_2b,m6502_2c,m6502_2d,m6502_2e,m6502_2f,
	m6502_30,m6502_31,m6502_32,m6502_33,m6502_34,m6502_35,m6502_36,m6502_37,
	m6502_38,m6502_39,m6502_3a,m6502_3b,m6502_3c,m6502_3d,m6502_3e,m6502_3f,
	m6502_40,m6502_41,m6502_42,m6502_43,m6502_44,m6502_45,m6502_46,m6502_47,
	m6502_48,m6502_49,m6502_4a,m6502_4b,m6502_4c,m6502_4d,m6502_4e,m6502_4f,
	m6502_50,m6502_51,m6502_52,m6502_53,m6502_54,m6502_55,m6502_56,m6502_57,
	m6502_58,m6502_59,m6502_5a,m6502_5b,m6502_5c,m6502_5d,m6502_5e,m6502_5f,
	m6502_60,m6502_61,m6502_62,m6502_63,m6502_64,m6502_65,m6502_66,m6502_67,
	m6502_68,m6502_69,m6502_6a,m6502_6b,m6502_6c,m6502_6d,m6502_6e,m6502_6f,
	m6502_70,m6502_71,m6502_72,m6502_73,m6502_74,m6502_75,m6502_76,m6502_77,
	m6502_78,m6502_79,m6502_7a,m6502_7b,m6502_7c,m6502_7d,m6502_7e,m6502_7f,
	m6502_80,m6502_81,m6502_82,m6502_83,m6502_84,m6502_85,m6502_86,m6502_87,
	m6502_88,m6502_89,m6502_8a,m6502_8b,m6502_8c,m6502_8d,m6502_8e,m6502_8f,
	m6502_90,m6502_91,m6502_92,m6502_93,m6502_94,m6502_95,m6502_96,m6502_97,
	m6502_98,m6502_99,m6502_9a,m6502_9b,m6502_9c,m6502_9d,m6502_9e,m6502_9f,
	m6502_a0,m6502_a1,m6502_a2,m6502_a3,m6502_a4,m6502_a5,m6502_a6,m6502_a7,
	m6502_a8,m6502_a9,m6502_aa,m6502_ab,m6502_ac,m6502_ad,m6502_ae,m6502_af,
	m6502_b0,m6502_b1,m6502_b2,m6502_b3,m6502_b4,m6502_b5,m6502_b6,m6502_b7,
	m6502_b8,m6502_b9,m6502_ba,m6502_bb,m6502_bc,m6502_bd,m6502_be,m6502_bf,
	m6502_c0,m6502_c1,m6502_c2,m6502_c3,m6502_c4,m6502_c5,m6502_c6,m6502_c7,
	m6502_c8,m6502_c9,m6502_ca,m6502_cb,m6502_cc,m6502_cd,m6502_ce,m6502_cf,
	m6502_d0,m6502_d1,m6502_d2,m6502_d3,m6502_d4,m6502_d5,m6502_d6,m6502_d7,
	m6502_d8,m6502_d9,m6502_da,m6502_db,m6502_dc,m6502_dd,m6502_de,m6502_df,
	m6502_e0,m6502_e1,m6502_e2,m6502_e3,m6502_e4,m6502_e5,m6502_e6,m6502_e7,
	m6502_e8,m6502_e9,m6502_ea,m6502_eb,m6502_ec,m6502_ed,m6502_ee,m6502_ef,
	m6502_f0,m6502_f1,m6502_f2,m6502_f3,m6502_f4,m6502_f5,m6502_f6,m6502_f7,
	m6502_f8,m6502_f9,m6502_fa,m6502_fb,m6502_fc,m6502_fd,m6502_fe,m6502_ff
};


#endif