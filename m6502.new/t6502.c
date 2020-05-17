/*****************************************************************************
 *
 *   tbl6502.c
 *   6502 opcode functions and function pointer table
 *
 *   Copyright Juergen Buchmueller, all rights reserved.
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
 *   2003-05-26  Fixed PHP, PLP, PHA, PLA cycle counts. [SJ]
 *   2004-04-30  Fixed STX (abs) cycle count. [SJ]
 *
 *****************************************************************************/

#undef	OP
//#define m6502_nn) M6502_INLINE void m6502_##nn(void)
//#define m6502_nn) m6502_##nn:

/*****************************************************************************
 *****************************************************************************
 *
 *   plain vanilla 6502 opcodes
 *
 *****************************************************************************
 * op    temp     cycles             rdmem   opc  wrmem   ********************/

m6502_00: {                  BRK;                 goto m6502_end; }/* 7 BRK */
m6502_20: {                  JSR;                 goto m6502_end; }/* 6 JSR */
m6502_40: {                  RTI;                 goto m6502_end; }/* 6 RTI */
m6502_60: {                  RTS;                 goto m6502_end; }/* 6 RTS */
m6502_80: { int tmp; RD_IMM; NOP;                 goto m6502_end; }/* 2 NOP IMM */
m6502_a0: { int tmp; RD_IMM; LDY;                 goto m6502_end; }/* 2 LDY IMM */
m6502_c0: { int tmp; RD_IMM; CPY;                 goto m6502_end; }/* 2 CPY IMM */
m6502_e0: { int tmp; RD_IMM; CPX;                 goto m6502_end; }/* 2 CPX IMM */

m6502_10: { BPL;                                  goto m6502_end; }/* 2-4 BPL REL */
m6502_30: { BMI;                                  goto m6502_end; }/* 2-4 BMI REL */
m6502_50: { BVC;                                  goto m6502_end; }/* 2-4 BVC REL */
m6502_70: { BVS;                                  goto m6502_end; }/* 2-4 BVS REL */
m6502_90: { BCC;                                  goto m6502_end; }/* 2-4 BCC REL */
m6502_b0: { BCS;                                  goto m6502_end; }/* 2-4 BCS REL */
m6502_d0: { BNE;                                  goto m6502_end; }/* 2-4 BNE REL */
m6502_f0: { BEQ;                                  goto m6502_end; }/* 2-4 BEQ REL */

m6502_01: { int tmp; RD_IDX; ORA;                 goto m6502_end; }/* 6 ORA IDX */
m6502_21: { int tmp; RD_IDX; AND;                 goto m6502_end; }/* 6 AND IDX */
m6502_41: { int tmp; RD_IDX; EOR;                 goto m6502_end; }/* 6 EOR IDX */
m6502_61: { int tmp; RD_IDX; ADC;                 goto m6502_end; }/* 6 ADC IDX */
m6502_81: { int tmp; STA; WR_IDX;                 goto m6502_end; }/* 6 STA IDX */
m6502_a1: { int tmp; RD_IDX; LDA;                 goto m6502_end; }/* 6 LDA IDX */
m6502_c1: { int tmp; RD_IDX; CMP;                 goto m6502_end; }/* 6 CMP IDX */
m6502_e1: { int tmp; RD_IDX; SBC;                 goto m6502_end; }/* 6 SBC IDX */

m6502_11: { int tmp; RD_IDY_P; ORA;               goto m6502_end; }/* 5 ORA IDY page penalty */
m6502_31: { int tmp; RD_IDY_P; AND;               goto m6502_end; }/* 5 AND IDY page penalty */
m6502_51: { int tmp; RD_IDY_P; EOR;               goto m6502_end; }/* 5 EOR IDY page penalty */
m6502_71: { int tmp; RD_IDY_P; ADC;               goto m6502_end; }/* 5 ADC IDY page penalty */
m6502_91: { int tmp; STA; WR_IDY_NP;              goto m6502_end; }/* 6 STA IDY */
m6502_b1: { int tmp; RD_IDY_P; LDA;               goto m6502_end; }/* 5 LDA IDY page penalty */
m6502_d1: { int tmp; RD_IDY_P; CMP;               goto m6502_end; }/* 5 CMP IDY page penalty */
m6502_f1: { int tmp; RD_IDY_P; SBC;               goto m6502_end; }/* 5 SBC IDY page penalty */

m6502_02: {                  KIL;                 goto m6502_end; }/* 1 KIL */
m6502_22: {                  KIL;                 goto m6502_end; }/* 1 KIL */
m6502_42: {                  KIL;                 goto m6502_end; }/* 1 KIL */
m6502_62: {                  KIL;                 goto m6502_end; }/* 1 KIL */
m6502_82: { int tmp; RD_IMM; NOP;                 goto m6502_end; }/* 2 NOP IMM */
m6502_a2: { int tmp; RD_IMM; LDX;                 goto m6502_end; }/* 2 LDX IMM */
m6502_c2: { int tmp; RD_IMM; NOP;                 goto m6502_end; }/* 2 NOP IMM */
m6502_e2: { int tmp; RD_IMM; NOP;                 goto m6502_end; }/* 2 NOP IMM */

m6502_12: { KIL;                                  goto m6502_end; }/* 1 KIL */
m6502_32: { KIL;                                  goto m6502_end; }/* 1 KIL */
m6502_52: { KIL;                                  goto m6502_end; }/* 1 KIL */
m6502_72: { KIL;                                  goto m6502_end; }/* 1 KIL */
m6502_92: { KIL;                                  goto m6502_end; }/* 1 KIL */
m6502_b2: { KIL;                                  goto m6502_end; }/* 1 KIL */
m6502_d2: { KIL;                                  goto m6502_end; }/* 1 KIL */
m6502_f2: { KIL;                                  goto m6502_end; }/* 1 KIL */

m6502_03: { int tmp; RD_IDX; WB_EA; SLO; WB_EA;   goto m6502_end; }/* 7 SLO IDX */
m6502_23: { int tmp; RD_IDX; WB_EA; RLA; WB_EA;   goto m6502_end; }/* 7 RLA IDX */
m6502_43: { int tmp; RD_IDX; WB_EA; SRE; WB_EA;   goto m6502_end; }/* 7 SRE IDX */
m6502_63: { int tmp; RD_IDX; WB_EA; RRA; WB_EA;   goto m6502_end; }/* 7 RRA IDX */
m6502_83: { int tmp;                SAX; WR_IDX;  goto m6502_end; }/* 6 SAX IDX */
m6502_a3: { int tmp; RD_IDX; LAX;                 goto m6502_end; }/* 6 LAX IDX */
m6502_c3: { int tmp; RD_IDX; WB_EA; DCP; WB_EA;   goto m6502_end; }/* 7 DCP IDX */
m6502_e3: { int tmp; RD_IDX; WB_EA; ISB; WB_EA;   goto m6502_end; }/* 7 ISB IDX */

m6502_13: { int tmp; RD_IDY_NP; WB_EA; SLO; WB_EA; goto m6502_end; }/* 7 SLO IDY */
m6502_33: { int tmp; RD_IDY_NP; WB_EA; RLA; WB_EA; goto m6502_end; }/* 7 RLA IDY */
m6502_53: { int tmp; RD_IDY_NP; WB_EA; SRE; WB_EA; goto m6502_end; }/* 7 SRE IDY */
m6502_73: { int tmp; RD_IDY_NP; WB_EA; RRA; WB_EA; goto m6502_end; }/* 7 RRA IDY */
m6502_93: { int tmp; EA_IDY_NP; SAH; WB_EA;        goto m6502_end; }/* 5 SAH IDY */
m6502_b3: { int tmp; RD_IDY_P; LAX;                goto m6502_end; }/* 5 LAX IDY page penalty */
m6502_d3: { int tmp; RD_IDY_NP; WB_EA; DCP; WB_EA; goto m6502_end; }/* 7 DCP IDY */
m6502_f3: { int tmp; RD_IDY_NP; WB_EA; ISB; WB_EA; goto m6502_end; }/* 7 ISB IDY */

m6502_04: { int tmp; RD_ZPG; NOP;                  goto m6502_end; }/* 3 NOP ZPG */
m6502_24: { int tmp; RD_ZPG; BIT;                  goto m6502_end; }/* 3 BIT ZPG */
m6502_44: { int tmp; RD_ZPG; NOP;                  goto m6502_end; }/* 3 NOP ZPG */
m6502_64: { int tmp; RD_ZPG; NOP;                  goto m6502_end; }/* 3 NOP ZPG */
m6502_84: { int tmp; STY; WR_ZPG;                  goto m6502_end; }/* 3 STY ZPG */
m6502_a4: { int tmp; RD_ZPG; LDY;                  goto m6502_end; }/* 3 LDY ZPG */
m6502_c4: { int tmp; RD_ZPG; CPY;                  goto m6502_end; }/* 3 CPY ZPG */
m6502_e4: { int tmp; RD_ZPG; CPX;                  goto m6502_end; }/* 3 CPX ZPG */

m6502_14: { int tmp; RD_ZPX; NOP;                  goto m6502_end; }/* 4 NOP ZPX */
m6502_34: { int tmp; RD_ZPX; NOP;                  goto m6502_end; }/* 4 NOP ZPX */
m6502_54: { int tmp; RD_ZPX; NOP;                  goto m6502_end; }/* 4 NOP ZPX */
m6502_74: { int tmp; RD_ZPX; NOP;                  goto m6502_end; }/* 4 NOP ZPX */
m6502_94: { int tmp; STY; WR_ZPX;                  goto m6502_end; }/* 4 STY ZPX */
m6502_b4: { int tmp; RD_ZPX; LDY;                  goto m6502_end; }/* 4 LDY ZPX */
m6502_d4: { int tmp; RD_ZPX; NOP;                  goto m6502_end; }/* 4 NOP ZPX */
m6502_f4: { int tmp; RD_ZPX; NOP;                  goto m6502_end; }/* 4 NOP ZPX */

m6502_05: { int tmp; RD_ZPG; ORA;                  goto m6502_end; }/* 3 ORA ZPG */
m6502_25: { int tmp; RD_ZPG; AND;                  goto m6502_end; }/* 3 AND ZPG */
m6502_45: { int tmp; RD_ZPG; EOR;                  goto m6502_end; }/* 3 EOR ZPG */
m6502_65: { int tmp; RD_ZPG; ADC;                  goto m6502_end; }/* 3 ADC ZPG */
m6502_85: { int tmp; STA; WR_ZPG;                  goto m6502_end; }/* 3 STA ZPG */
m6502_a5: { int tmp; RD_ZPG; LDA;                  goto m6502_end; }/* 3 LDA ZPG */
m6502_c5: { int tmp; RD_ZPG; CMP;                  goto m6502_end; }/* 3 CMP ZPG */
m6502_e5: { int tmp; RD_ZPG; SBC;                  goto m6502_end; }/* 3 SBC ZPG */

m6502_15: { int tmp; RD_ZPX; ORA;                  goto m6502_end; }/* 4 ORA ZPX */
m6502_35: { int tmp; RD_ZPX; AND;                  goto m6502_end; }/* 4 AND ZPX */
m6502_55: { int tmp; RD_ZPX; EOR;                  goto m6502_end; }/* 4 EOR ZPX */
m6502_75: { int tmp; RD_ZPX; ADC;                  goto m6502_end; }/* 4 ADC ZPX */
m6502_95: { int tmp; STA; WR_ZPX;                  goto m6502_end; }/* 4 STA ZPX */
m6502_b5: { int tmp; RD_ZPX; LDA;                  goto m6502_end; }/* 4 LDA ZPX */
m6502_d5: { int tmp; RD_ZPX; CMP;                  goto m6502_end; }/* 4 CMP ZPX */
m6502_f5: { int tmp; RD_ZPX; SBC;                  goto m6502_end; }/* 4 SBC ZPX */

m6502_06: { int tmp; RD_ZPG; WB_EA; ASL; WB_EA;    goto m6502_end; }/* 5 ASL ZPG */
m6502_26: { int tmp; RD_ZPG; WB_EA; ROL; WB_EA;    goto m6502_end; }/* 5 ROL ZPG */
m6502_46: { int tmp; RD_ZPG; WB_EA; LSR; WB_EA;    goto m6502_end; }/* 5 LSR ZPG */
m6502_66: { int tmp; RD_ZPG; WB_EA; ROR; WB_EA;    goto m6502_end; }/* 5 ROR ZPG */
m6502_86: { int tmp; STX; WR_ZPG;                  goto m6502_end; }/* 3 STX ZPG */
m6502_a6: { int tmp; RD_ZPG; LDX;                  goto m6502_end; }/* 3 LDX ZPG */
m6502_c6: { int tmp; RD_ZPG; WB_EA; DEC; WB_EA;    goto m6502_end; }/* 5 DEC ZPG */
m6502_e6: { int tmp; RD_ZPG; WB_EA; INC; WB_EA;    goto m6502_end; }/* 5 INC ZPG */

m6502_16: { int tmp; RD_ZPX; WB_EA; ASL; WB_EA;    goto m6502_end; }/* 6 ASL ZPX */
m6502_36: { int tmp; RD_ZPX; WB_EA; ROL; WB_EA;    goto m6502_end; }/* 6 ROL ZPX */
m6502_56: { int tmp; RD_ZPX; WB_EA; LSR; WB_EA;    goto m6502_end; }/* 6 LSR ZPX */
m6502_76: { int tmp; RD_ZPX; WB_EA; ROR; WB_EA;    goto m6502_end; }/* 6 ROR ZPX */
m6502_96: { int tmp; STX; WR_ZPY;                  goto m6502_end; }/* 4 STX ZPY */
m6502_b6: { int tmp; RD_ZPY; LDX;                  goto m6502_end; }/* 4 LDX ZPY */
m6502_d6: { int tmp; RD_ZPX; WB_EA; DEC; WB_EA;    goto m6502_end; }/* 6 DEC ZPX */
m6502_f6: { int tmp; RD_ZPX; WB_EA; INC; WB_EA;    goto m6502_end; }/* 6 INC ZPX */

m6502_07: { int tmp; RD_ZPG; WB_EA; SLO; WB_EA;    goto m6502_end; }/* 5 SLO ZPG */
m6502_27: { int tmp; RD_ZPG; WB_EA; RLA; WB_EA;    goto m6502_end; }/* 5 RLA ZPG */
m6502_47: { int tmp; RD_ZPG; WB_EA; SRE; WB_EA;    goto m6502_end; }/* 5 SRE ZPG */
m6502_67: { int tmp; RD_ZPG; WB_EA; RRA; WB_EA;    goto m6502_end; }/* 5 RRA ZPG */
m6502_87: { int tmp; SAX; WR_ZPG;                  goto m6502_end; }/* 3 SAX ZPG */
m6502_a7: { int tmp; RD_ZPG; LAX;                  goto m6502_end; }/* 3 LAX ZPG */
m6502_c7: { int tmp; RD_ZPG; WB_EA; DCP; WB_EA;    goto m6502_end; }/* 5 DCP ZPG */
m6502_e7: { int tmp; RD_ZPG; WB_EA; ISB; WB_EA;    goto m6502_end; }/* 5 ISB ZPG */

m6502_17: { int tmp; RD_ZPX; WB_EA; SLO; WB_EA;    goto m6502_end; }/* 6 SLO ZPX */
m6502_37: { int tmp; RD_ZPX; WB_EA; RLA; WB_EA;    goto m6502_end; }/* 6 RLA ZPX */
m6502_57: { int tmp; RD_ZPX; WB_EA; SRE; WB_EA;    goto m6502_end; }/* 6 SRE ZPX */
m6502_77: { int tmp; RD_ZPX; WB_EA; RRA; WB_EA;    goto m6502_end; }/* 6 RRA ZPX */
m6502_97: { int tmp; SAX; WR_ZPY;                  goto m6502_end; }/* 4 SAX ZPY */
m6502_b7: { int tmp; RD_ZPY; LAX;                  goto m6502_end; }/* 4 LAX ZPY */
m6502_d7: { int tmp; RD_ZPX; WB_EA; DCP; WB_EA;    goto m6502_end; }/* 6 DCP ZPX */
m6502_f7: { int tmp; RD_ZPX; WB_EA; ISB; WB_EA;    goto m6502_end; }/* 6 ISB ZPX */

m6502_08: { RD_DUM; PHP;                           goto m6502_end; }/* 3 PHP */
m6502_28: { RD_DUM; PLP;                           goto m6502_end; }/* 4 PLP */
m6502_48: { RD_DUM; PHA;                           goto m6502_end; }/* 3 PHA */
m6502_68: { RD_DUM; PLA;                           goto m6502_end; }/* 4 PLA */
m6502_88: { RD_DUM; DEY;                           goto m6502_end; }/* 2 DEY */
m6502_a8: { RD_DUM; TAY;                           goto m6502_end; }/* 2 TAY */
m6502_c8: { RD_DUM; INY;                           goto m6502_end; }/* 2 INY */
m6502_e8: { RD_DUM; INX;                           goto m6502_end; }/* 2 INX */

m6502_18: { RD_DUM; CLC;                           goto m6502_end; }/* 2 CLC */
m6502_38: { RD_DUM; SEC;                           goto m6502_end; }/* 2 SEC */
m6502_58: { RD_DUM; CLI;                           goto m6502_end; }/* 2 CLI */
m6502_78: { RD_DUM; SEI;                           goto m6502_end; }/* 2 SEI */
m6502_98: { RD_DUM; TYA;                           goto m6502_end; }/* 2 TYA */
m6502_b8: { RD_DUM; CLV;                           goto m6502_end; }/* 2 CLV */
m6502_d8: { RD_DUM; CLD;                           goto m6502_end; }/* 2 CLD */
m6502_f8: { RD_DUM; SED;                           goto m6502_end; }/* 2 SED */

m6502_09: { int tmp; RD_IMM; ORA;                  goto m6502_end; }/* 2 ORA IMM */
m6502_29: { int tmp; RD_IMM; AND;                  goto m6502_end; }/* 2 AND IMM */
m6502_49: { int tmp; RD_IMM; EOR;                  goto m6502_end; }/* 2 EOR IMM */
m6502_69: { int tmp; RD_IMM; ADC;                  goto m6502_end; }/* 2 ADC IMM */
m6502_89: { int tmp; RD_IMM; NOP;                  goto m6502_end; }/* 2 NOP IMM */
m6502_a9: { int tmp; RD_IMM; LDA;                  goto m6502_end; }/* 2 LDA IMM */
m6502_c9: { int tmp; RD_IMM; CMP;                  goto m6502_end; }/* 2 CMP IMM */
m6502_e9: { int tmp; RD_IMM; SBC;                  goto m6502_end; }/* 2 SBC IMM */

m6502_19: { int tmp; RD_ABY_P; ORA;                goto m6502_end; }/* 4 ORA ABY page penalty */
m6502_39: { int tmp; RD_ABY_P; AND;                goto m6502_end; }/* 4 AND ABY page penalty */
m6502_59: { int tmp; RD_ABY_P; EOR;                goto m6502_end; }/* 4 EOR ABY page penalty */
m6502_79: { int tmp; RD_ABY_P; ADC;                goto m6502_end; }/* 4 ADC ABY page penalty */
m6502_99: { int tmp; STA; WR_ABY_NP;               goto m6502_end; }/* 5 STA ABY */
m6502_b9: { int tmp; RD_ABY_P; LDA;                goto m6502_end; }/* 4 LDA ABY page penalty */
m6502_d9: { int tmp; RD_ABY_P; CMP;                goto m6502_end; }/* 4 CMP ABY page penalty */
m6502_f9: { int tmp; RD_ABY_P; SBC;                goto m6502_end; }/* 4 SBC ABY page penalty */

m6502_0a: { int tmp; RD_DUM; RD_ACC; ASL; WB_ACC;  goto m6502_end; }/* 2 ASL A */
m6502_2a: { int tmp; RD_DUM; RD_ACC; ROL; WB_ACC;  goto m6502_end; }/* 2 ROL A */
m6502_4a: { int tmp; RD_DUM; RD_ACC; LSR; WB_ACC;  goto m6502_end; }/* 2 LSR A */
m6502_6a: { int tmp; RD_DUM; RD_ACC; ROR; WB_ACC;  goto m6502_end; }/* 2 ROR A */
m6502_8a: { RD_DUM; TXA;                           goto m6502_end; }/* 2 TXA */
m6502_aa: { RD_DUM; TAX;                           goto m6502_end; }/* 2 TAX */
m6502_ca: { RD_DUM; DEX;                           goto m6502_end; }/* 2 DEX */
m6502_ea: { RD_DUM; NOP;                           goto m6502_end; }/* 2 NOP */

m6502_1a: { RD_DUM; NOP;                           goto m6502_end; }/* 2 NOP */
m6502_3a: { RD_DUM; NOP;                           goto m6502_end; }/* 2 NOP */
m6502_5a: { RD_DUM; NOP;                           goto m6502_end; }/* 2 NOP */
m6502_7a: { RD_DUM; NOP;                           goto m6502_end; }/* 2 NOP */
m6502_9a: { RD_DUM; TXS;                           goto m6502_end; }/* 2 TXS */
m6502_ba: { RD_DUM; TSX;                           goto m6502_end; }/* 2 TSX */
m6502_da: { RD_DUM; NOP;                           goto m6502_end; }/* 2 NOP */
m6502_fa: { RD_DUM; NOP;                           goto m6502_end; }/* 2 NOP */

m6502_0b: { int tmp; RD_IMM; ANC;                  goto m6502_end; }/* 2 ANC IMM */
m6502_2b: { int tmp; RD_IMM; ANC;                  goto m6502_end; }/* 2 ANC IMM */
m6502_4b: { int tmp; RD_IMM; ASR; WB_ACC;          goto m6502_end; }/* 2 ASR IMM */
m6502_6b: { int tmp; RD_IMM; ARR; WB_ACC;          goto m6502_end; }/* 2 ARR IMM */
m6502_8b: { int tmp; RD_IMM; AXA;                  goto m6502_end; }/* 2 AXA IMM */
m6502_ab: { int tmp; RD_IMM; OAL;                  goto m6502_end; }/* 2 OAL IMM */
m6502_cb: { int tmp; RD_IMM; ASX;                  goto m6502_end; }/* 2 ASX IMM */
m6502_eb: { int tmp; RD_IMM; SBC;                  goto m6502_end; }/* 2 SBC IMM */

m6502_1b: { int tmp; RD_ABY_NP; WB_EA; SLO; WB_EA; goto m6502_end; }/* 7 SLO ABY */
m6502_3b: { int tmp; RD_ABY_NP; WB_EA; RLA; WB_EA; goto m6502_end; }/* 7 RLA ABY */
m6502_5b: { int tmp; RD_ABY_NP; WB_EA; SRE; WB_EA; goto m6502_end; }/* 7 SRE ABY */
m6502_7b: { int tmp; RD_ABY_NP; WB_EA; RRA; WB_EA; goto m6502_end; }/* 7 RRA ABY */
m6502_9b: { int tmp; EA_ABY_NP; SSH; WB_EA;        goto m6502_end; }/* 5 SSH ABY */
m6502_bb: { int tmp; RD_ABY_P; AST;                goto m6502_end; }/* 4 AST ABY page penalty */
m6502_db: { int tmp; RD_ABY_NP; WB_EA; DCP; WB_EA; goto m6502_end; }/* 7 DCP ABY */
m6502_fb: { int tmp; RD_ABY_NP; WB_EA; ISB; WB_EA; goto m6502_end; }/* 7 ISB ABY */

m6502_0c: { int tmp; RD_ABS; NOP;                  goto m6502_end; }/* 4 NOP ABS */
m6502_2c: { int tmp; RD_ABS; BIT;                  goto m6502_end; }/* 4 BIT ABS */
m6502_4c: { EA_ABS; JMP;                           goto m6502_end; }/* 3 JMP ABS */
m6502_6c: { int tmp; EA_IND; JMP;                  goto m6502_end; }/* 5 JMP IND */
m6502_8c: { int tmp; STY; WR_ABS;                  goto m6502_end; }/* 4 STY ABS */
m6502_ac: { int tmp; RD_ABS; LDY;                  goto m6502_end; }/* 4 LDY ABS */
m6502_cc: { int tmp; RD_ABS; CPY;                  goto m6502_end; }/* 4 CPY ABS */
m6502_ec: { int tmp; RD_ABS; CPX;                  goto m6502_end; }/* 4 CPX ABS */

m6502_1c: { int tmp; RD_ABX_P; NOP;                goto m6502_end; }/* 4 NOP ABX page penalty */
m6502_3c: { int tmp; RD_ABX_P; NOP;                goto m6502_end; }/* 4 NOP ABX page penalty */
m6502_5c: { int tmp; RD_ABX_P; NOP;                goto m6502_end; }/* 4 NOP ABX page penalty */
m6502_7c: { int tmp; RD_ABX_P; NOP;                goto m6502_end; }/* 4 NOP ABX page penalty */
m6502_9c: { int tmp; EA_ABX_NP; SYH; WB_EA;        goto m6502_end; }/* 5 SYH ABX */
m6502_bc: { int tmp; RD_ABX_P; LDY;                goto m6502_end; }/* 4 LDY ABX page penalty */
m6502_dc: { int tmp; RD_ABX_P; NOP;                goto m6502_end; }/* 4 NOP ABX page penalty */
m6502_fc: { int tmp; RD_ABX_P; NOP;                goto m6502_end; }/* 4 NOP ABX page penalty */

m6502_0d: { int tmp; RD_ABS; ORA;                  goto m6502_end; }/* 4 ORA ABS */
m6502_2d: { int tmp; RD_ABS; AND;                  goto m6502_end; }/* 4 AND ABS */
m6502_4d: { int tmp; RD_ABS; EOR;                  goto m6502_end; }/* 4 EOR ABS */
m6502_6d: { int tmp; RD_ABS; ADC;                  goto m6502_end; }/* 4 ADC ABS */
m6502_8d: { int tmp; STA; WR_ABS;                  goto m6502_end; }/* 4 STA ABS */
m6502_ad: { int tmp; RD_ABS; LDA;                  goto m6502_end; }/* 4 LDA ABS */
m6502_cd: { int tmp; RD_ABS; CMP;                  goto m6502_end; }/* 4 CMP ABS */
m6502_ed: { int tmp; RD_ABS; SBC;                  goto m6502_end; }/* 4 SBC ABS */

m6502_1d: { int tmp; RD_ABX_P; ORA;                goto m6502_end; }/* 4 ORA ABX page penalty */
m6502_3d: { int tmp; RD_ABX_P; AND;                goto m6502_end; }/* 4 AND ABX page penalty */
m6502_5d: { int tmp; RD_ABX_P; EOR;                goto m6502_end; }/* 4 EOR ABX page penalty */
m6502_7d: { int tmp; RD_ABX_P; ADC;                goto m6502_end; }/* 4 ADC ABX page penalty */
m6502_9d: { int tmp; STA; WR_ABX_NP;               goto m6502_end; }/* 5 STA ABX */
m6502_bd: { int tmp; RD_ABX_P; LDA;                goto m6502_end; }/* 4 LDA ABX page penalty */
m6502_dd: { int tmp; RD_ABX_P; CMP;                goto m6502_end; }/* 4 CMP ABX page penalty */
m6502_fd: { int tmp; RD_ABX_P; SBC;                goto m6502_end; }/* 4 SBC ABX page penalty */

m6502_0e: { int tmp; RD_ABS; WB_EA; ASL; WB_EA;    goto m6502_end; }/* 6 ASL ABS */
m6502_2e: { int tmp; RD_ABS; WB_EA; ROL; WB_EA;    goto m6502_end; }/* 6 ROL ABS */
m6502_4e: { int tmp; RD_ABS; WB_EA; LSR; WB_EA;    goto m6502_end; }/* 6 LSR ABS */
m6502_6e: { int tmp; RD_ABS; WB_EA; ROR; WB_EA;    goto m6502_end; }/* 6 ROR ABS */
m6502_8e: { int tmp; STX; WR_ABS;                  goto m6502_end; }/* 4 STX ABS */
m6502_ae: { int tmp; RD_ABS; LDX;                  goto m6502_end; }/* 4 LDX ABS */
m6502_ce: { int tmp; RD_ABS; WB_EA; DEC; WB_EA;    goto m6502_end; }/* 6 DEC ABS */
m6502_ee: { int tmp; RD_ABS; WB_EA; INC; WB_EA;    goto m6502_end; }/* 6 INC ABS */

m6502_1e: { int tmp; RD_ABX_NP; WB_EA; ASL; WB_EA; goto m6502_end; }/* 7 ASL ABX */
m6502_3e: { int tmp; RD_ABX_NP; WB_EA; ROL; WB_EA; goto m6502_end; }/* 7 ROL ABX */
m6502_5e: { int tmp; RD_ABX_NP; WB_EA; LSR; WB_EA; goto m6502_end; }/* 7 LSR ABX */
m6502_7e: { int tmp; RD_ABX_NP; WB_EA; ROR; WB_EA; goto m6502_end; }/* 7 ROR ABX */
m6502_9e: { int tmp; EA_ABY_NP; SXH; WB_EA;	goto m6502_end; }/* 5 SXH ABY */
m6502_be: { int tmp; RD_ABY_P; LDX;		goto m6502_end; }/* 4 LDX ABY page penalty */
m6502_de: { int tmp; RD_ABX_NP; WB_EA; DEC; WB_EA; goto m6502_end; }/* 7 DEC ABX */
m6502_fe: { int tmp; RD_ABX_NP; WB_EA; INC; WB_EA; goto m6502_end; }/* 7 INC ABX */

m6502_0f: { int tmp; RD_ABS; WB_EA; SLO; WB_EA;    goto m6502_end; }/* 6 SLO ABS */
m6502_2f: { int tmp; RD_ABS; WB_EA; RLA; WB_EA;    goto m6502_end; }/* 6 RLA ABS */
m6502_4f: { int tmp; RD_ABS; WB_EA; SRE; WB_EA;    goto m6502_end; }/* 6 SRE ABS */
m6502_6f: { int tmp; RD_ABS; WB_EA; RRA; WB_EA;    goto m6502_end; }/* 6 RRA ABS */
m6502_8f: { int tmp; SAX; WR_ABS;                  goto m6502_end; }/* 4 SAX ABS */
m6502_af: { int tmp; RD_ABS; LAX;                  goto m6502_end; }/* 4 LAX ABS */
m6502_cf: { int tmp; RD_ABS; WB_EA; DCP; WB_EA;    goto m6502_end; }/* 6 DCP ABS */
m6502_ef: { int tmp; RD_ABS; WB_EA; ISB; WB_EA;    goto m6502_end; }/* 6 ISB ABS */

m6502_1f: { int tmp; RD_ABX_NP; WB_EA; SLO; WB_EA; goto m6502_end; }/* 7 SLO ABX */
m6502_3f: { int tmp; RD_ABX_NP; WB_EA; RLA; WB_EA; goto m6502_end; }/* 7 RLA ABX */
m6502_5f: { int tmp; RD_ABX_NP; WB_EA; SRE; WB_EA; goto m6502_end; }/* 7 SRE ABX */
m6502_7f: { int tmp; RD_ABX_NP; WB_EA; RRA; WB_EA; goto m6502_end; }/* 7 RRA ABX */
m6502_9f: { int tmp; EA_ABY_NP; SAH; WB_EA;        goto m6502_end; }/* 5 SAH ABY */
m6502_bf: { int tmp; RD_ABY_P; LAX;                goto m6502_end; }/* 4 LAX ABY page penalty */
m6502_df: { int tmp; RD_ABX_NP; WB_EA; DCP; WB_EA; goto m6502_end; }/* 7 DCP ABX */
m6502_ff: { int tmp; RD_ABX_NP; WB_EA; ISB; WB_EA; goto m6502_end; }/* 7 ISB ABX */

/* and here's the array of function pointers */
/*
static void (*const insn6502[0x100])(void) = {
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
*/
