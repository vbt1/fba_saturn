/*****************************************************************************
 *
 *	 tbl6510.c
 *	 6510 opcode functions and function pointer table
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
 *	   terms of its usage and license at any time, including retroactively
 *	 - This entire notice must remain in the source code.
 *
 *	 - Opcode information based on an Intel 386 '6510.asm' core
 *	   written by R.F. van Ee (1993)
 *	 - Cycle counts are guesswork :-)
 *
 *****************************************************************************/

#if SUPP6510

#undef	OP
#define OP(nn) INLINE void m6510_##nn(void)

/*****************************************************************************
 *****************************************************************************
 *
 *   overrides for 6510 opcodes
 *
 *****************************************************************************
 ********** insn   temp     cycles             rdmem   opc  wrmem   **********/
#define m6510_00 m6502_00									
#define m6510_20 m6502_20									
#define m6510_40 m6502_40									
#define m6510_60 m6502_60									
OP(80) {		  M6502_ICount -= 2;		 DOP;		  } 
#define m6510_a0 m6502_a0									
#define m6510_c0 m6502_c0									
#define m6510_e0 m6502_e0									

#define m6510_10 m6502_10									
#define m6510_30 m6502_30									
#define m6510_50 m6502_50									
#define m6510_70 m6502_70									
#define m6510_90 m6502_90									
#define m6510_b0 m6502_b0									
#define m6510_d0 m6502_d0									
#define m6510_f0 m6502_f0									

#define m6510_01 m6502_01									
#define m6510_21 m6502_21									
#define m6510_41 m6502_41									
#define m6510_61 m6502_61									
#define m6510_81 m6502_81									
#define m6510_a1 m6502_a1									
#define m6510_c1 m6502_c1									
#define m6510_e1 m6502_e1									

#define m6510_11 m6502_11									
#define m6510_31 m6502_31									
#define m6510_51 m6502_51									
#define m6510_71 m6502_71									
#define m6510_91 m6502_91									
#define m6510_b1 m6502_b1									
#define m6510_d1 m6502_d1									
#define m6510_f1 m6502_f1									

#define m6510_02 m6502_02									
#define m6510_22 m6502_22									
#define m6510_42 m6502_42									
#define m6510_62 m6502_62									
OP(82) {		  M6502_ICount -= 2;		 DOP;		  } 
#define m6510_a2 m6502_a2									
OP(c2) {		  M6502_ICount -= 2;		 DOP;		  } 
OP(e2) {		  M6502_ICount -= 2;		 DOP;		  } 

#define m6510_12 m6502_12									
#define m6510_32 m6502_32									
#define m6510_52 m6502_52									
#define m6510_72 m6502_72									
#define m6510_92 m6502_92									
#define m6510_b2 m6502_b2									 
#define m6510_d2 m6502_d2									 
#define m6510_f2 m6502_f2									 

OP(03) { int tmp; M6502_ICount -= 7; RD_IDX; SLO; WB_EA;  } 
OP(23) { int tmp; M6502_ICount -= 7; RD_IDX; RLA; WB_EA;  } 
OP(43) { int tmp; M6502_ICount -= 7; RD_IDX; SRE; WB_EA;  } 
OP(63) { int tmp; M6502_ICount -= 7; RD_IDX; RRA; WB_EA;  } 
OP(83) { int tmp; M6502_ICount -= 6;		 SAX; WR_IDX; } 
OP(a3) { int tmp; M6502_ICount -= 6; RD_IDX; LAX;		  } 
OP(c3) { int tmp; M6502_ICount -= 7; RD_IDX; DCP; WB_EA;  } 
OP(e3) { int tmp; M6502_ICount -= 7; RD_IDX; ISB; WB_EA;  } 

OP(13) { int tmp; M6502_ICount -= 6; RD_IDY; SLO; WB_EA;  } 
OP(33) { int tmp; M6502_ICount -= 6; RD_IDY; RLA; WB_EA;  } 
OP(53) { int tmp; M6502_ICount -= 6; RD_IDY; SRE; WB_EA;  } 
OP(73) { int tmp; M6502_ICount -= 6; RD_IDY; RRA; WB_EA;  } 
OP(93) { int tmp; M6502_ICount -= 5;		 SAX; WR_IDY; } 
OP(b3) { int tmp; M6502_ICount -= 5; RD_IDY; LAX;		  } 
OP(d3) { int tmp; M6502_ICount -= 6; RD_IDY; DCP; WB_EA;  } 
OP(f3) { int tmp; M6502_ICount -= 6; RD_IDY; ISB; WB_EA;  } 

OP(04) {		  M6502_ICount -= 2;		 DOP;		  } 
#define m6510_24 m6502_24									
OP(44) {		  M6502_ICount -= 2;		 DOP;		  } 
OP(64) {		  M6502_ICount -= 2;		 DOP;		  } 
#define m6510_84 m6502_84									
#define m6510_a4 m6502_a4									
#define m6510_c4 m6502_c4									
#define m6510_e4 m6502_e4									

OP(14) {		  M6502_ICount -= 2;		 DOP;		  } 
OP(34) {		  M6502_ICount -= 2;		 DOP;		  } 
OP(54) {		  M6502_ICount -= 2;		 DOP;		  } 
OP(74) {		  M6502_ICount -= 2;		 DOP;		  } 
#define m6510_94 m6502_94									
#define m6510_b4 m6502_b4									
OP(d4) {		  M6502_ICount -= 2;		 DOP;		  } 
OP(f4) {		  M6502_ICount -= 2;		 DOP;		  } 

#define m6510_05 m6502_05									
#define m6510_25 m6502_25									
#define m6510_45 m6502_45									
#define m6510_65 m6502_65									
#define m6510_85 m6502_85									
#define m6510_a5 m6502_a5									
#define m6510_c5 m6502_c5									
#define m6510_e5 m6502_e5									

#define m6510_15 m6502_15									
#define m6510_35 m6502_35									
#define m6510_55 m6502_55									
#define m6510_75 m6502_75									
#define m6510_95 m6502_95									
#define m6510_b5 m6502_b5									
#define m6510_d5 m6502_d5									
#define m6510_f5 m6502_f5									

#define m6510_06 m6502_06									
#define m6510_26 m6502_26									
#define m6510_46 m6502_46									
#define m6510_66 m6502_66									
#define m6510_86 m6502_86									
#define m6510_a6 m6502_a6									
#define m6510_c6 m6502_c6									
#define m6510_e6 m6502_e6									

#define m6510_16 m6502_16									
#define m6510_36 m6502_36									
#define m6510_56 m6502_56									
#define m6510_76 m6502_76									
#define m6510_96 m6502_96									
#define m6510_b6 m6502_b6									
#define m6510_d6 m6502_d6									
#define m6510_f6 m6502_f6									

OP(07) { int tmp; M6502_ICount -= 5; RD_ZPG; SLO; WB_EA;  } 
OP(27) { int tmp; M6502_ICount -= 5; RD_ZPG; RLA; WB_EA;  } 
OP(47) { int tmp; M6502_ICount -= 5; RD_ZPG; SRE; WB_EA;  } 
OP(67) { int tmp; M6502_ICount -= 5; RD_ZPG; RRA; WB_EA;  } 
OP(87) { int tmp; M6502_ICount -= 3;		 SAX; WR_ZPG; } 
OP(a7) { int tmp; M6502_ICount -= 3; RD_ZPG; LAX;		  } 
OP(c7) { int tmp; M6502_ICount -= 5; RD_ZPG; DCP; WB_EA;  } 
OP(e7) { int tmp; M6502_ICount -= 5; RD_ZPG; ISB; WB_EA;  } 

OP(17) { int tmp; M6502_ICount -= 6; RD_ZPX; SLO; WB_EA;  } 
OP(37) { int tmp; M6502_ICount -= 6; RD_ZPX; RLA; WB_EA;  } 
OP(57) { int tmp; M6502_ICount -= 6; RD_ZPX; SRE; WB_EA;  } 
OP(77) { int tmp; M6502_ICount -= 6; RD_ZPX; RRA; WB_EA;  } 
OP(97) { int tmp; M6502_ICount -= 4;		 SAX; WR_ZPX; } 
OP(b7) { int tmp; M6502_ICount -= 4; RD_ZPX; LAX;		  } 
OP(d7) { int tmp; M6502_ICount -= 6; RD_ZPX; DCP; WB_EA;  } 
OP(f7) { int tmp; M6502_ICount -= 6; RD_ZPX; ISB; WB_EA;  } 

#define m6510_08 m6502_08									
#define m6510_28 m6502_28									
#define m6510_48 m6502_48									
#define m6510_68 m6502_68									
#define m6510_88 m6502_88									
#define m6510_a8 m6502_a8									
#define m6510_c8 m6502_c8									
#define m6510_e8 m6502_e8									

#define m6510_18 m6502_18									
#define m6510_38 m6502_38									
#define m6510_58 m6502_58									
#define m6510_78 m6502_78									
#define m6510_98 m6502_98									
#define m6510_b8 m6502_b8									
#define m6510_d8 m6502_d8									
#define m6510_f8 m6502_f8									

#define m6510_09 m6502_09									
#define m6510_29 m6502_29									
#define m6510_49 m6502_49									
#define m6510_69 m6502_69									
OP(89) {		  M6502_ICount -= 2;		 DOP;		  } 
#define m6510_a9 m6502_a9									
#define m6510_c9 m6502_c9									
#define m6510_e9 m6502_e9									

#define m6510_19 m6502_19									
#define m6510_39 m6502_39									
#define m6510_59 m6502_59									
#define m6510_79 m6502_79									
#define m6510_99 m6502_99									
#define m6510_b9 m6502_b9									
#define m6510_d9 m6502_d9									
#define m6510_f9 m6502_f9									

#define m6510_0a m6502_0a									
#define m6510_2a m6502_2a									
#define m6510_4a m6502_4a									
#define m6510_6a m6502_6a									
#define m6510_8a m6502_8a									
#define m6510_aa m6502_aa									
#define m6510_ca m6502_ca									
#define m6510_ea m6502_ea									

OP(1a) {		  M6502_ICount -= 2;		 NOP;		  } 
OP(3a) {		  M6502_ICount -= 2;		 NOP;		  } 
OP(5a) {		  M6502_ICount -= 2;		 NOP;		  } 
OP(7a) {		  M6502_ICount -= 2;		 NOP;		  } 
#define m6510_9a m6502_9a									
#define m6510_ba m6502_ba									
OP(da) {		  M6502_ICount -= 2;		 NOP;		  } 
OP(fa) {		  M6502_ICount -= 2;		 NOP;		  } 

OP(0b) { int tmp; M6502_ICount -= 2; RD_IMM; ANC;		  } 
OP(2b) { int tmp; M6502_ICount -= 2; RD_IMM; ANC;		  } 
OP(4b) { int tmp; M6502_ICount -= 2; RD_IMM; ASR; WB_EA;  } 
OP(6b) { int tmp; M6502_ICount -= 2; RD_IMM; ARR; WB_EA;  } 
OP(8b) { int tmp; M6502_ICount -= 2; RD_IMM; AXA; WB_EA;  } 
OP(ab) { int tmp; M6502_ICount -= 2; RD_IMM; LAX;		  } 
OP(cb) { int tmp; M6502_ICount -= 2; RD_IMM; ASX;		  } 
OP(eb) { int tmp; M6502_ICount -= 2; RD_IMM; SBC;		  } 

OP(1b) { int tmp; M6502_ICount -= 4; RD_ABY; SLO;		  } 
OP(3b) { int tmp; M6502_ICount -= 4; RD_ABY; RLA;		  } 
OP(5b) { int tmp; M6502_ICount -= 4; RD_ABY; SRE;		  } 
OP(7b) { int tmp; M6502_ICount -= 4; RD_ABY; RRA;		  } 
OP(9b) { int tmp; M6502_ICount -= 5;		 SSH; WR_ABY; } 
OP(bb) { int tmp; M6502_ICount -= 4; RD_ABY; AST;		  } 
OP(db) { int tmp; M6502_ICount -= 6; RD_ABY; DCP; WB_EA;  } 
OP(fb) { int tmp; M6502_ICount -= 6; RD_ABY; ISB; WB_EA;  } 

OP(0c) {		  M6502_ICount -= 2;		 TOP;		  } 
#define m6510_2c m6502_2c									
#define m6510_4c m6502_4c									
#define m6510_6c m6502_6c									
#define m6510_8c m6502_8c									
#define m6510_ac m6502_ac									
#define m6510_cc m6502_cc									
#define m6510_ec m6502_ec									

OP(1c) {		  M6502_ICount -= 2;		 TOP;		  } 
OP(3c) {		  M6502_ICount -= 2;		 TOP;		  } 
OP(5c) {		  M6502_ICount -= 2;		 TOP;		  } 
OP(7c) {		  M6502_ICount -= 2;		 TOP;		  } 
OP(9c) { int tmp; M6502_ICount -= 5;		 SYH; WR_ABX; } 
#define m6510_bc m6502_bc									
OP(dc) {		  M6502_ICount -= 2;		 TOP;		  } 
OP(fc) {		  M6502_ICount -= 2;		 TOP;		  } 

#define m6510_0d m6502_0d									
#define m6510_2d m6502_2d									
#define m6510_4d m6502_4d									
#define m6510_6d m6502_6d									
#define m6510_8d m6502_8d									
#define m6510_ad m6502_ad									
#define m6510_cd m6502_cd									
#define m6510_ed m6502_ed									

#define m6510_1d m6502_1d									
#define m6510_3d m6502_3d									
#define m6510_5d m6502_5d									
#define m6510_7d m6502_7d									
#define m6510_9d m6502_9d									
#define m6510_bd m6502_bd									
#define m6510_dd m6502_dd									
#define m6510_fd m6502_fd									

#define m6510_0e m6502_0e									
#define m6510_2e m6502_2e									
#define m6510_4e m6502_4e									
#define m6510_6e m6502_6e									
#define m6510_8e m6502_8e									
#define m6510_ae m6502_ae									
#define m6510_ce m6502_ce									
#define m6510_ee m6502_ee									

#define m6510_1e m6502_1e									
#define m6510_3e m6502_3e									
#define m6510_5e m6502_5e									
#define m6510_7e m6502_7e									
#define m6510_9e m6502_9e									
#define m6510_be m6502_be									
#define m6510_de m6502_de									
#define m6510_fe m6502_fe									

OP(0f) { int tmp; M6502_ICount -= 6; RD_ABS; SLO; WB_EA;  } 
OP(2f) { int tmp; M6502_ICount -= 6; RD_ABS; RLA; WB_EA;  } 
OP(4f) { int tmp; M6502_ICount -= 6; RD_ABS; SRE; WB_EA;  } 
OP(6f) { int tmp; M6502_ICount -= 6; RD_ABS; RRA; WB_EA;  } 
OP(8f) { int tmp; M6502_ICount -= 4;		 SAX; WR_ABS; } 
OP(af) { int tmp; M6502_ICount -= 5; RD_ABS; LAX;		  } 
OP(cf) { int tmp; M6502_ICount -= 6; RD_ABS; DCP; WB_EA;  } 
OP(ef) { int tmp; M6502_ICount -= 6; RD_ABS; ISB; WB_EA;  } 

OP(1f) { int tmp; M6502_ICount -= 4; RD_ABX; SLO; WB_EA;  } 
OP(3f) { int tmp; M6502_ICount -= 4; RD_ABX; RLA; WB_EA;  } 
OP(5f) { int tmp; M6502_ICount -= 4; RD_ABX; SRE; WB_EA;  } 
OP(7f) { int tmp; M6502_ICount -= 4; RD_ABX; RRA; WB_EA;  } 
OP(9f) { int tmp; M6502_ICount -= 6;		 SAH; WR_ABY; } 
OP(bf) { int tmp; M6502_ICount -= 6; RD_ABY; LAX;		  } 
OP(df) { int tmp; M6502_ICount -= 7; RD_ABX; DCP; WB_EA;  } 
OP(ff) { int tmp; M6502_ICount -= 7; RD_ABX; ISB; WB_EA;  } 

static void (*insn6510[0x100])(void) = {
	m6510_00,m6510_01,m6510_02,m6510_03,m6510_04,m6510_05,m6510_06,m6510_07,
	m6510_08,m6510_09,m6510_0a,m6510_0b,m6510_0c,m6510_0d,m6510_0e,m6510_0f,
	m6510_10,m6510_11,m6510_12,m6510_13,m6510_14,m6510_15,m6510_16,m6510_17,
	m6510_18,m6510_19,m6510_1a,m6510_1b,m6510_1c,m6510_1d,m6510_1e,m6510_1f,
	m6510_20,m6510_21,m6510_22,m6510_23,m6510_24,m6510_25,m6510_26,m6510_27,
	m6510_28,m6510_29,m6510_2a,m6510_2b,m6510_2c,m6510_2d,m6510_2e,m6510_2f,
	m6510_30,m6510_31,m6510_32,m6510_33,m6510_34,m6510_35,m6510_36,m6510_37,
	m6510_38,m6510_39,m6510_3a,m6510_3b,m6510_3c,m6510_3d,m6510_3e,m6510_3f,
	m6510_40,m6510_41,m6510_42,m6510_43,m6510_44,m6510_45,m6510_46,m6510_47,
	m6510_48,m6510_49,m6510_4a,m6510_4b,m6510_4c,m6510_4d,m6510_4e,m6510_4f,
	m6510_50,m6510_51,m6510_52,m6510_53,m6510_54,m6510_55,m6510_56,m6510_57,
	m6510_58,m6510_59,m6510_5a,m6510_5b,m6510_5c,m6510_5d,m6510_5e,m6510_5f,
	m6510_60,m6510_61,m6510_62,m6510_63,m6510_64,m6510_65,m6510_66,m6510_67,
	m6510_68,m6510_69,m6510_6a,m6510_6b,m6510_6c,m6510_6d,m6510_6e,m6510_6f,
	m6510_70,m6510_71,m6510_72,m6510_73,m6510_74,m6510_75,m6510_76,m6510_77,
	m6510_78,m6510_79,m6510_7a,m6510_7b,m6510_7c,m6510_7d,m6510_7e,m6510_7f,
	m6510_80,m6510_81,m6510_82,m6510_83,m6510_84,m6510_85,m6510_86,m6510_87,
	m6510_88,m6510_89,m6510_8a,m6510_8b,m6510_8c,m6510_8d,m6510_8e,m6510_8f,
	m6510_90,m6510_91,m6510_92,m6510_93,m6510_94,m6510_95,m6510_96,m6510_97,
	m6510_98,m6510_99,m6510_9a,m6510_9b,m6510_9c,m6510_9d,m6510_9e,m6510_9f,
	m6510_a0,m6510_a1,m6510_a2,m6510_a3,m6510_a4,m6510_a5,m6510_a6,m6510_a7,
	m6510_a8,m6510_a9,m6510_aa,m6510_ab,m6510_ac,m6510_ad,m6510_ae,m6510_af,
	m6510_b0,m6510_b1,m6510_b2,m6510_b3,m6510_b4,m6510_b5,m6510_b6,m6510_b7,
	m6510_b8,m6510_b9,m6510_ba,m6510_bb,m6510_bc,m6510_bd,m6510_be,m6510_bf,
	m6510_c0,m6510_c1,m6510_c2,m6510_c3,m6510_c4,m6510_c5,m6510_c6,m6510_c7,
	m6510_c8,m6510_c9,m6510_ca,m6510_cb,m6510_cc,m6510_cd,m6510_ce,m6510_cf,
	m6510_d0,m6510_d1,m6510_d2,m6510_d3,m6510_d4,m6510_d5,m6510_d6,m6510_d7,
	m6510_d8,m6510_d9,m6510_da,m6510_db,m6510_dc,m6510_dd,m6510_de,m6510_df,
	m6510_e0,m6510_e1,m6510_e2,m6510_e3,m6510_e4,m6510_e5,m6510_e6,m6510_e7,
	m6510_e8,m6510_e9,m6510_ea,m6510_eb,m6510_ec,m6510_ed,m6510_ee,m6510_ef,
	m6510_f0,m6510_f1,m6510_f2,m6510_f3,m6510_f4,m6510_f5,m6510_f6,m6510_f7,
	m6510_f8,m6510_f9,m6510_fa,m6510_fb,m6510_fc,m6510_fd,m6510_fe,m6510_ff
};

#endif  /* SUPP6510 */

