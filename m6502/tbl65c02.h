/*****************************************************************************
 *
 *	 tbl65c02.c
 *	 65c02 opcode functions and function pointer table
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

#if SUPP65C02

#undef	OP
#define OP(nn) INLINE void m65c02_##nn(void)

/*****************************************************************************
 *****************************************************************************
 *
 *	 overrides for 65C02 opcodes
 *
 *****************************************************************************
 * op	 temp	  cycles			 rdmem	 opc  wrmem   ********************/
#define m65c02_00 m6502_00									
#define m65c02_20 m6502_20									
#define m65c02_40 m6502_40									
#define m65c02_60 m6502_60									
OP(80) { int tmp;							 BRA(1);	  } 
#define m65c02_a0 m6502_a0									
#define m65c02_c0 m6502_c0									
#define m65c02_e0 m6502_e0									

#define m65c02_10 m6502_10									
#define m65c02_30 m6502_30									
#define m65c02_50 m6502_50									
#define m65c02_70 m6502_70									
#define m65c02_90 m6502_90									
#define m65c02_b0 m6502_b0									
#define m65c02_d0 m6502_d0									
#define m65c02_f0 m6502_f0									

#define m65c02_01 m6502_01									
#define m65c02_21 m6502_21									
#define m65c02_41 m6502_41									
#define m65c02_61 m6502_61									
#define m65c02_81 m6502_81									
#define m65c02_a1 m6502_a1									
#define m65c02_c1 m6502_c1									
#define m65c02_e1 m6502_e1									

#define m65c02_11 m6502_11									
#define m65c02_31 m6502_31									
#define m65c02_51 m6502_51									
#define m65c02_71 m6502_71									
#define m65c02_91 m6502_91									
#define m65c02_b1 m6502_b1									
#define m65c02_d1 m6502_d1									
#define m65c02_f1 m6502_f1									

#define m65c02_02 m6502_02									
#define m65c02_22 m6502_22									
#define m65c02_42 m6502_42									
#define m65c02_62 m6502_62									
#define m65c02_82 m6502_82									
#define m65c02_a2 m6502_a2									
#define m65c02_c2 m6502_c2									
#define m65c02_e2 m6502_e2									

OP(12) { int tmp; M6502_ICount -= 3; RD_ZPI; ORA;		  } 
OP(32) { int tmp; M6502_ICount -= 3; RD_ZPI; AND;		  } 
OP(52) { int tmp; M6502_ICount -= 3; RD_ZPI; EOR;		  } 
OP(72) { int tmp; M6502_ICount -= 3; RD_ZPI; ADC;		  } 
OP(92) { int tmp; M6502_ICount -= 4;		 STA; WR_ZPI; } 
OP(b2) { int tmp; M6502_ICount -= 3; RD_ZPI; LDA;		  } 
OP(d2) { int tmp; M6502_ICount -= 3; RD_ZPI; CMP;		  } 
OP(f2) { int tmp; M6502_ICount -= 3; RD_ZPI; SBC;		  } 

#define m65c02_03 m6502_03									
#define m65c02_23 m6502_23									
#define m65c02_43 m6502_43									
#define m65c02_63 m6502_63									
#define m65c02_83 m6502_83									
#define m65c02_a3 m6502_a3									
#define m65c02_c3 m6502_c3									
#define m65c02_e3 m6502_e3									

#define m65c02_13 m6502_13									
#define m65c02_33 m6502_33									
#define m65c02_53 m6502_53									
#define m65c02_73 m6502_73									
#define m65c02_93 m6502_93									
#define m65c02_b3 m6502_b3									
#define m65c02_d3 m6502_d3									
#define m65c02_f3 m6502_f3									

OP(04) { int tmp; M6502_ICount -= 3; RD_ZPG; TSB; WB_EA;  } 
#define m65c02_24 m6502_24									
#define m65c02_44 m6502_44									
OP(64) { int tmp; M6502_ICount -= 2;		 STZ; WR_ZPG; } 
#define m65c02_84 m6502_84									
#define m65c02_a4 m6502_a4									
#define m65c02_c4 m6502_c4									
#define m65c02_e4 m6502_e4									

OP(14) { int tmp; M6502_ICount -= 3; RD_ZPG; TRB; WB_EA;  } 
OP(34) { int tmp; M6502_ICount -= 4; RD_ZPX; BIT;		  } 
#define m65c02_54 m6502_54									
OP(74) { int tmp; M6502_ICount -= 4;		 STZ; WR_ZPX; } 
OP(94) { int tmp; M6502_ICount -= 4;		 STY; WR_ZPX; } 
OP(b4) { int tmp; M6502_ICount -= 4; RD_ZPX; LDY;		  } 
#define m65c02_d4 m6502_d4									
#define m65c02_f4 m6502_f4									

#define m65c02_05 m6502_05									
#define m65c02_25 m6502_25									
#define m65c02_45 m6502_45									
#define m65c02_65 m6502_65									
#define m65c02_85 m6502_85									
#define m65c02_a5 m6502_a5									
#define m65c02_c5 m6502_c5									
#define m65c02_e5 m6502_e5									

#define m65c02_15 m6502_15									
#define m65c02_35 m6502_35									
#define m65c02_55 m6502_55									
#define m65c02_75 m6502_75									
#define m65c02_95 m6502_95									
#define m65c02_b5 m6502_b5									
#define m65c02_d5 m6502_d5									
#define m65c02_f5 m6502_f5									

#define m65c02_06 m6502_06									
#define m65c02_26 m6502_26									
#define m65c02_46 m6502_46									
#define m65c02_66 m6502_66									
#define m65c02_86 m6502_86									
#define m65c02_a6 m6502_a6									
#define m65c02_c6 m6502_c6									
#define m65c02_e6 m6502_e6									

#define m65c02_16 m6502_16									
#define m65c02_36 m6502_36									
#define m65c02_56 m6502_56									
#define m65c02_76 m6502_76									
#define m65c02_96 m6502_96									
#define m65c02_b6 m6502_b6									
#define m65c02_d6 m6502_d6									
#define m65c02_f6 m6502_f6									

OP(07) { int tmp; M6502_ICount -= 5; RD_ZPG; RMB(0);WB_EA;} 
OP(27) { int tmp; M6502_ICount -= 5; RD_ZPG; RMB(2);WB_EA;} 
OP(47) { int tmp; M6502_ICount -= 5; RD_ZPG; RMB(4);WB_EA;} 
OP(67) { int tmp; M6502_ICount -= 5; RD_ZPG; RMB(6);WB_EA;} 
OP(87) { int tmp; M6502_ICount -= 5; RD_ZPG; SMB(0);WB_EA;} 
OP(a7) { int tmp; M6502_ICount -= 5; RD_ZPG; SMB(2);WB_EA;} 
OP(c7) { int tmp; M6502_ICount -= 5; RD_ZPG; SMB(4);WB_EA;} 
OP(e7) { int tmp; M6502_ICount -= 5; RD_ZPG; SMB(6);WB_EA;} 

OP(17) { int tmp; M6502_ICount -= 5; RD_ZPG; RMB(1);WB_EA;} 
OP(37) { int tmp; M6502_ICount -= 5; RD_ZPG; RMB(3);WB_EA;} 
OP(57) { int tmp; M6502_ICount -= 5; RD_ZPG; RMB(5);WB_EA;} 
OP(77) { int tmp; M6502_ICount -= 5; RD_ZPG; RMB(7);WB_EA;} 
OP(97) { int tmp; M6502_ICount -= 5; RD_ZPG; SMB(1);WB_EA;} 
OP(b7) { int tmp; M6502_ICount -= 5; RD_ZPG; SMB(3);WB_EA;} 
OP(d7) { int tmp; M6502_ICount -= 5; RD_ZPG; SMB(5);WB_EA;} 
OP(f7) { int tmp; M6502_ICount -= 5; RD_ZPG; SMB(7);WB_EA;} 

#define m65c02_08 m6502_08                                  
#define m65c02_28 m6502_28									
#define m65c02_48 m6502_48									
#define m65c02_68 m6502_68									
#define m65c02_88 m6502_88									
#define m65c02_a8 m6502_a8									
#define m65c02_c8 m6502_c8									
#define m65c02_e8 m6502_e8									

#define m65c02_18 m6502_18									
#define m65c02_38 m6502_38									
#define m65c02_58 m6502_58									
#define m65c02_78 m6502_78									
#define m65c02_98 m6502_98									
#define m65c02_b8 m6502_b8									
#define m65c02_d8 m6502_d8									
#define m65c02_f8 m6502_f8									

#define m65c02_09 m6502_09									
#define m65c02_29 m6502_29									
#define m65c02_49 m6502_49									
#define m65c02_69 m6502_69									
OP(89) { int tmp; M6502_ICount -= 2; RD_IMM; BIT;		  } 
#define m65c02_a9 m6502_a9									
#define m65c02_c9 m6502_c9									
#define m65c02_e9 m6502_e9									

#define m65c02_19 m6502_19									
#define m65c02_39 m6502_39									
#define m65c02_59 m6502_59									
#define m65c02_79 m6502_79									
#define m65c02_99 m6502_99									
#define m65c02_b9 m6502_b9									
#define m65c02_d9 m6502_d9									
#define m65c02_f9 m6502_f9									

#define m65c02_0a m6502_0a									
#define m65c02_2a m6502_2a									
#define m65c02_4a m6502_4a									
#define m65c02_6a m6502_6a									
#define m65c02_8a m6502_8a									
#define m65c02_aa m6502_aa									
#define m65c02_ca m6502_ca									
#define m65c02_ea m6502_ea									

OP(1a) {		  M6502_ICount -= 2;		 INA;		  } 
OP(3a) {		  M6502_ICount -= 2;		 DEA;		  } 
OP(5a) {		  M6502_ICount -= 3;		 PHY;		  } 
OP(7a) {		  M6502_ICount -= 4;		 PLY;		  } 
#define m65c02_9a m6502_9a									
#define m65c02_ba m6502_ba									
OP(da) {		  M6502_ICount -= 3;		 PHX;		  } 
OP(fa) {		  M6502_ICount -= 4;		 PLX;		  } 

#define m65c02_0b m6502_0b									
#define m65c02_2b m6502_2b									
#define m65c02_4b m6502_4b									
#define m65c02_6b m6502_6b									
#define m65c02_8b m6502_8b									
#define m65c02_ab m6502_ab									
#define m65c02_cb m6502_cb									 
#define m65c02_eb m6502_eb									 

#define m65c02_1b m6502_1b									 
#define m65c02_3b m6502_3b									 
#define m65c02_5b m6502_5b									 
#define m65c02_7b m6502_7b									 
#define m65c02_9b m6502_9b									 
#define m65c02_bb m6502_bb									 
#define m65c02_db m6502_db									 
#define m65c02_fb m6502_fb									 

OP(0c) { int tmp; M6502_ICount -= 2; RD_ABS; TSB; WB_EA;  } 
#define m65c02_2c m6502_2c									
#define m65c02_4c m6502_4c									
#define m65c02_6c m6502_6c									
#define m65c02_8c m6502_8c									
#define m65c02_ac m6502_ac									
#define m65c02_cc m6502_cc									
#define m65c02_ec m6502_ec									

OP(1c) { int tmp; M6502_ICount -= 4; RD_ABS; TRB; WB_EA;  }  
OP(3c) { int tmp; M6502_ICount -= 4; RD_ABX; BIT;		  }  
#define m65c02_5c m6502_5c									 
OP(7c) { int tmp; M6502_ICount -= 2; EA_IAX; JMP;		  }  
OP(9c) { int tmp; M6502_ICount -= 4;		 STZ; WR_ABS; }  
OP(bc) { int tmp; M6502_ICount -= 4; RD_ABX; LDY;		  }  
#define m65c02_dc m6502_dc									 
#define m65c02_fc m6502_fc									 

#define m65c02_0d m6502_0d									 
#define m65c02_2d m6502_2d									 
#define m65c02_4d m6502_4d									
#define m65c02_6d m6502_6d									
#define m65c02_8d m6502_8d									
#define m65c02_ad m6502_ad									
#define m65c02_cd m6502_cd									
#define m65c02_ed m6502_ed									

#define m65c02_1d m6502_1d									
#define m65c02_3d m6502_3d									
#define m65c02_5d m6502_5d									
#define m65c02_7d m6502_7d									
#define m65c02_9d m6502_9d									
#define m65c02_bd m6502_bd									
#define m65c02_dd m6502_dd									
#define m65c02_fd m6502_fd									

#define m65c02_0e m6502_0e									
#define m65c02_2e m6502_2e									
#define m65c02_4e m6502_4e									
#define m65c02_6e m6502_6e									
#define m65c02_8e m6502_8e									
#define m65c02_ae m6502_ae									
#define m65c02_ce m6502_ce									
#define m65c02_ee m6502_ee									

#define m65c02_1e m6502_1e									
#define m65c02_3e m6502_3e									
#define m65c02_5e m6502_5e									
#define m65c02_7e m6502_7e									
#define m65c02_9e m6502_9e									
#define m65c02_be m6502_be									
#define m65c02_de m6502_de									
#define m65c02_fe m6502_fe									

OP(0f) { int tmp; M6502_ICount -= 5; RD_ZPG; BBR(0);	  } 
OP(2f) { int tmp; M6502_ICount -= 5; RD_ZPG; BBR(2);	  } 
OP(4f) { int tmp; M6502_ICount -= 5; RD_ZPG; BBR(4);	  } 
OP(6f) { int tmp; M6502_ICount -= 5; RD_ZPG; BBR(6);	  } 
OP(8f) { int tmp; M6502_ICount -= 5; RD_ZPG; BBS(0);	  } 
OP(af) { int tmp; M6502_ICount -= 5; RD_ZPG; BBS(2);	  } 
OP(cf) { int tmp; M6502_ICount -= 5; RD_ZPG; BBS(4);	  } 
OP(ef) { int tmp; M6502_ICount -= 5; RD_ZPG; BBS(6);	  } 

OP(1f) { int tmp; M6502_ICount -= 5; RD_ZPG; BBR(1);	  } 
OP(3f) { int tmp; M6502_ICount -= 5; RD_ZPG; BBR(3);	  } 
OP(5f) { int tmp; M6502_ICount -= 5; RD_ZPG; BBR(5);	  } 
OP(7f) { int tmp; M6502_ICount -= 5; RD_ZPG; BBR(7);	  } 
OP(9f) { int tmp; M6502_ICount -= 5; RD_ZPG; BBS(1);	  } 
OP(bf) { int tmp; M6502_ICount -= 5; RD_ZPG; BBS(3);	  } 
OP(df) { int tmp; M6502_ICount -= 5; RD_ZPG; BBS(5);	  } 
OP(ff) { int tmp; M6502_ICount -= 5; RD_ZPG; BBS(7);	  } 

static void (*insn65c02[0x100])(void) = {
	m65c02_00,m65c02_01,m65c02_02,m65c02_03,m65c02_04,m65c02_05,m65c02_06,m65c02_07,
	m65c02_08,m65c02_09,m65c02_0a,m65c02_0b,m65c02_0c,m65c02_0d,m65c02_0e,m65c02_0f,
	m65c02_10,m65c02_11,m65c02_12,m65c02_13,m65c02_14,m65c02_15,m65c02_16,m65c02_17,
	m65c02_18,m65c02_19,m65c02_1a,m65c02_1b,m65c02_1c,m65c02_1d,m65c02_1e,m65c02_1f,
	m65c02_20,m65c02_21,m65c02_22,m65c02_23,m65c02_24,m65c02_25,m65c02_26,m65c02_27,
	m65c02_28,m65c02_29,m65c02_2a,m65c02_2b,m65c02_2c,m65c02_2d,m65c02_2e,m65c02_2f,
	m65c02_30,m65c02_31,m65c02_32,m65c02_33,m65c02_34,m65c02_35,m65c02_36,m65c02_37,
	m65c02_38,m65c02_39,m65c02_3a,m65c02_3b,m65c02_3c,m65c02_3d,m65c02_3e,m65c02_3f,
	m65c02_40,m65c02_41,m65c02_42,m65c02_43,m65c02_44,m65c02_45,m65c02_46,m65c02_47,
	m65c02_48,m65c02_49,m65c02_4a,m65c02_4b,m65c02_4c,m65c02_4d,m65c02_4e,m65c02_4f,
	m65c02_50,m65c02_51,m65c02_52,m65c02_53,m65c02_54,m65c02_55,m65c02_56,m65c02_57,
	m65c02_58,m65c02_59,m65c02_5a,m65c02_5b,m65c02_5c,m65c02_5d,m65c02_5e,m65c02_5f,
	m65c02_60,m65c02_61,m65c02_62,m65c02_63,m65c02_64,m65c02_65,m65c02_66,m65c02_67,
	m65c02_68,m65c02_69,m65c02_6a,m65c02_6b,m65c02_6c,m65c02_6d,m65c02_6e,m65c02_6f,
	m65c02_70,m65c02_71,m65c02_72,m65c02_73,m65c02_74,m65c02_75,m65c02_76,m65c02_77,
	m65c02_78,m65c02_79,m65c02_7a,m65c02_7b,m65c02_7c,m65c02_7d,m65c02_7e,m65c02_7f,
	m65c02_80,m65c02_81,m65c02_82,m65c02_83,m65c02_84,m65c02_85,m65c02_86,m65c02_87,
	m65c02_88,m65c02_89,m65c02_8a,m65c02_8b,m65c02_8c,m65c02_8d,m65c02_8e,m65c02_8f,
	m65c02_90,m65c02_91,m65c02_92,m65c02_93,m65c02_94,m65c02_95,m65c02_96,m65c02_97,
	m65c02_98,m65c02_99,m65c02_9a,m65c02_9b,m65c02_9c,m65c02_9d,m65c02_9e,m65c02_9f,
	m65c02_a0,m65c02_a1,m65c02_a2,m65c02_a3,m65c02_a4,m65c02_a5,m65c02_a6,m65c02_a7,
	m65c02_a8,m65c02_a9,m65c02_aa,m65c02_ab,m65c02_ac,m65c02_ad,m65c02_ae,m65c02_af,
	m65c02_b0,m65c02_b1,m65c02_b2,m65c02_b3,m65c02_b4,m65c02_b5,m65c02_b6,m65c02_b7,
	m65c02_b8,m65c02_b9,m65c02_ba,m65c02_bb,m65c02_bc,m65c02_bd,m65c02_be,m65c02_bf,
	m65c02_c0,m65c02_c1,m65c02_c2,m65c02_c3,m65c02_c4,m65c02_c5,m65c02_c6,m65c02_c7,
	m65c02_c8,m65c02_c9,m65c02_ca,m65c02_cb,m65c02_cc,m65c02_cd,m65c02_ce,m65c02_cf,
	m65c02_d0,m65c02_d1,m65c02_d2,m65c02_d3,m65c02_d4,m65c02_d5,m65c02_d6,m65c02_d7,
	m65c02_d8,m65c02_d9,m65c02_da,m65c02_db,m65c02_dc,m65c02_dd,m65c02_de,m65c02_df,
	m65c02_e0,m65c02_e1,m65c02_e2,m65c02_e3,m65c02_e4,m65c02_e5,m65c02_e6,m65c02_e7,
	m65c02_e8,m65c02_e9,m65c02_ea,m65c02_eb,m65c02_ec,m65c02_ed,m65c02_ee,m65c02_ef,
	m65c02_f0,m65c02_f1,m65c02_f2,m65c02_f3,m65c02_f4,m65c02_f5,m65c02_f6,m65c02_f7,
	m65c02_f8,m65c02_f9,m65c02_fa,m65c02_fb,m65c02_fc,m65c02_fd,m65c02_fe,m65c02_ff
};

#endif	/* SUPP65C02 */

