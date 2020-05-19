/*

HNZVC

? = undefined
* = affected
- = unaffected
0 = cleared
1 = set
# = CCr directly affected by instruction
@ = special - carry set if bit 7 is set

*/

#ifdef NEW
static void illegal:
#else
illegal:
#endif
{
//	logerror("M6809: illegal opcode at %04x\n",PC);
}

// regroupement des NEGs
/* $00 NEG direct ?**** */
neg_di:
{
	UINT16 r,t;
	DIRBYTE(t);
	r = -t;
	CLR_NZVC;
	SET_FLAGS8(0,t,r);
	WM(EAD,r);
	
	RET (6);
}

/* $40 NEGA inherent ?**** */
nega:
{
	UINT16 r;
	r = -A;
	CLR_NZVC;
	SET_FLAGS8(0,A,r);
	A = r;
	RET(2);	
}

/* $50 NEGB inherent ?**** */
negb:
{
	UINT16 r;
	r = -B;
	CLR_NZVC;
	SET_FLAGS8(0,B,r);
	B = r;
	RET(2);	
}

/* $60 NEG indexed ?**** */
neg_ix:
{
	UINT16 r,t;
	fetch_effective_address();
	t = RM(EAD);
	r=-t;
	CLR_NZVC;
	SET_FLAGS8(0,t,r);
	WM(EAD,r);
	RET(6);	
}

/* $70 NEG extended ?**** */
neg_ex:
{
	UINT16 r,t;
	EXTBYTE(t); r=-t;
	CLR_NZVC; SET_FLAGS8(0,t,r);
	WM(EAD,r);
	RET(7);	
}

void oSWI()
{	
	CC |= CC_E; 			// HJB 980225: save entire state 
	PUSHWORD(pPC);
	PUSHWORD(pU);
	PUSHWORD(pY);
	PUSHWORD(pX);
	PUSHBYTE(DP);
	PUSHBYTE(B);
	PUSHBYTE(A);
	PUSHBYTE(CC);
}

/* $89 ADCA immediate ***** */
adca_im:
{
	UINT16 t,r;
	IMMBYTE(t);
	r = A + t + (CC & CC_C);
	CLR_HNZVC;
	SET_FLAGS8(A,t,r);
	SET_H(A,t,r);
	A = r;
	RET(2);	
}

/* $99 ADCA direct ***** */
adca_di:
{
	UINT16 t,r;
	DIRBYTE(t);
	r = A + t + (CC & CC_C);
	CLR_HNZVC;
	SET_FLAGS8(A,t,r);
	SET_H(A,t,r);
	A = r;
	RET(4);	
}

/* $a9 ADCA indexed ***** */
adca_ix:
{
	UINT16 t,r;
	fetch_effective_address();
	t = RM(EAD);
	r = A + t + (CC & CC_C);
	CLR_HNZVC;
	SET_FLAGS8(A,t,r);
	SET_H(A,t,r);
	A = r;
	RET(4);	
}

/* $b9 ADCA extended ***** */
adca_ex:
{
	UINT16 t,r;
	EXTBYTE(t);
	r = A + t + (CC & CC_C);
	CLR_HNZVC;
	SET_FLAGS8(A,t,r);
	SET_H(A,t,r);
	A = r;
	RET(5);	
}

/* $c9 ADCB immediate ***** */
adcb_im:
{
	UINT16 t,r;
	IMMBYTE(t);
	r = B + t + (CC & CC_C);
	CLR_HNZVC;
	SET_FLAGS8(B,t,r);
	SET_H(B,t,r);
	B = r;
	RET(2);	
}

/* $d9 ADCB direct ***** */
adcb_di:
{
	UINT16 t,r;
	DIRBYTE(t);
	r = B + t + (CC & CC_C);
	CLR_HNZVC;
	SET_FLAGS8(B,t,r);
	SET_H(B,t,r);
	B = r;
	RET(4);	
}

/* $e9 ADCB indexed ***** */
adcb_ix:
{
	UINT16 t,r;
	fetch_effective_address();
	t = RM(EAD);
	r = B + t + (CC & CC_C);
	CLR_HNZVC;
	SET_FLAGS8(B,t,r);
	SET_H(B,t,r);
	B = r;
	RET(4);	
}

/* $f9 ADCB extended ***** */
adcb_ex:
{
	UINT16 t,r;
	EXTBYTE(t);
	r = B + t + (CC & CC_C);
	CLR_HNZVC;
	SET_FLAGS8(B,t,r);
	SET_H(B,t,r);
	B = r;
	RET(5);	
}

/* $8B ADDA immediate ***** */
adda_im:
{
	UINT16 t,r;
	IMMBYTE(t);
	r = A + t;
	CLR_HNZVC;
	SET_FLAGS8(A,t,r);
	SET_H(A,t,r);
	A = r;
	RET(2);	
}

/* $9B ADDA direct ***** */
adda_di:
{
	UINT16 t,r;
	DIRBYTE(t);
	r = A + t;
	CLR_HNZVC;
	SET_FLAGS8(A,t,r);
	SET_H(A,t,r);
	A = r;
	RET(4);	
}

/* $aB ADDA indexed ***** */
adda_ix:
{
	UINT16 t,r;
	fetch_effective_address();
	t = RM(EAD);
	r = A + t;
	CLR_HNZVC;
	SET_FLAGS8(A,t,r);
	SET_H(A,t,r);
	A = r;
	RET(4);	
}

/* $bB ADDA extended ***** */
adda_ex:
{
	UINT16 t,r;
	EXTBYTE(t);
	r = A + t;
	CLR_HNZVC;
	SET_FLAGS8(A,t,r);
	SET_H(A,t,r);
	A = r;
	RET(5);	
}

/* $cB ADDB immediate ***** */
addb_im:
{
	UINT16 t,r;
	IMMBYTE(t);
	r = B + t;
	CLR_HNZVC;
	SET_FLAGS8(B,t,r);
	SET_H(B,t,r);
	B = r;
	RET(2);	
}

/* $dB ADDB direct ***** */
addb_di:
{
	UINT16 t,r;
	DIRBYTE(t);
	r = B + t;
	CLR_HNZVC;
	SET_FLAGS8(B,t,r);
	SET_H(B,t,r);
	B = r;
	RET(4);	
}

/* $eB ADDB indexed ***** */
addb_ix:
{
	UINT16 t,r;
	fetch_effective_address();
	t = RM(EAD);
	r = B + t;
	CLR_HNZVC;
	SET_FLAGS8(B,t,r);
	SET_H(B,t,r);
	B = r;
	RET(4);	
}

/* $fB ADDB extended ***** */
addb_ex:
{
	UINT16 t,r;
	EXTBYTE(t);
	r = B + t;
	CLR_HNZVC;
	SET_FLAGS8(B,t,r);
	SET_H(B,t,r);
	B = r;
	RET(5);	
}	

/* $80 SUBA immediate ?**** */
suba_im:
{
	UINT16 t,r;
	IMMBYTE(t);
	r = A - t;
	CLR_NZVC;
	SET_FLAGS8(A,t,r);
	A = r;
	RET(2);	
}

/* $82 SBCA immediate ?**** */
sbca_im:
{
	UINT16	  t,r;
	IMMBYTE(t);
	r = A - t - (CC & CC_C);
	CLR_NZVC;
	SET_FLAGS8(A,t,r);
	A = r;
	RET(2);	
}
/* $c0 SUBB immediate ?**** */
subb_im:
{
	UINT16	  t,r;
	IMMBYTE(t);
	r = B - t;
	CLR_NZVC;
	SET_FLAGS8(B,t,r);
	B = r;
	RET(2);	
}
/* $90 SUBA direct ?**** */
suba_di:
{
	UINT16	  t,r;
	DIRBYTE(t);
	r = A - t;
	CLR_NZVC;
	SET_FLAGS8(A,t,r);
	A = r;
	RET(4);	
}

/* $92 SBCA direct ?**** */
sbca_di:
{
	UINT16	  t,r;
	DIRBYTE(t);
	r = A - t - (CC & CC_C);
	CLR_NZVC;
	SET_FLAGS8(A,t,r);
	A = r;
	RET(4);	
}

/* $a0 SUBA indexed ?**** */
suba_ix:
{
	UINT16 t,r;
	fetch_effective_address();
	t = RM(EAD);
	r = A - t;
	CLR_NZVC;
	SET_FLAGS8(A,t,r);
	A = r;
	RET(4);	
}

/* $a2 SBCA indexed ?**** */
sbca_ix:
{
	UINT16	  t,r;
	fetch_effective_address();
	t = RM(EAD);
	r = A - t - (CC & CC_C);
	CLR_NZVC;
	SET_FLAGS8(A,t,r);
	A = r;
	RET(4);	
}

/* $b0 SUBA extended ?**** */
suba_ex:
{
	UINT16	  t,r;
	EXTBYTE(t);
	r = A - t;
	CLR_NZVC;
	SET_FLAGS8(A,t,r);
	A = r;
	RET(5);	
}

/* $b2 SBCA extended ?**** */
sbca_ex:
{
	UINT16	  t,r;
	EXTBYTE(t);
	r = A - t - (CC & CC_C);
	CLR_NZVC;
	SET_FLAGS8(A,t,r);
	A = r;
	RET(5);	
}

/* $c2 SBCB immediate ?**** */
sbcb_im:
{
	UINT16	  t,r;
	IMMBYTE(t);
	r = B - t - (CC & CC_C);
	CLR_NZVC;
	SET_FLAGS8(B,t,r);
	B = r;
	RET(2);	
}

/* $d0 SUBB direct ?**** */
subb_di:
{
	UINT16	  t,r;
	DIRBYTE(t);
	r = B - t;
	CLR_NZVC;
	SET_FLAGS8(B,t,r);
	B = r;
	RET(4);	
}

/* $d2 SBCB direct ?**** */
sbcb_di:
{
	UINT16	  t,r;
	DIRBYTE(t);
	r = B - t - (CC & CC_C);
	CLR_NZVC;
	SET_FLAGS8(B,t,r);
	B = r;
	RET(4);	
}

/* $e0 SUBB indexed ?**** */
subb_ix:
{
	UINT16	  t,r;
	fetch_effective_address();
	t = RM(EAD);
	r = B - t;
	CLR_NZVC;
	SET_FLAGS8(B,t,r);
	B = r;
	RET(4);	
}

/* $e2 SBCB indexed ?**** */
sbcb_ix:
{
	UINT16	  t,r;
	fetch_effective_address();
	t = RM(EAD);
	r = B - t - (CC & CC_C);
	CLR_NZVC;
	SET_FLAGS8(B,t,r);
	B = r;
	RET(4);	
}

/* $f0 SUBB extended ?**** */
subb_ex:
{
	UINT16	  t,r;
	EXTBYTE(t);
	r = B - t;
	CLR_NZVC;
	SET_FLAGS8(B,t,r);
	B = r;
	RET(5);	
}

/* $f2 SBCB extended ?**** */
sbcb_ex:
{
	UINT16	  t,r;
	EXTBYTE(t);
	r = B - t - (CC & CC_C);
	CLR_NZVC;
	SET_FLAGS8(B,t,r);
	B = r;
	RET(5);	
}

/* $04 LSR direct -0*-* */
lsr_di:
{
	UINT8 t;
	DIRBYTE(t);
	CLR_NZC;
	CC |= (t & CC_C);
	t >>= 1;
	SET_Z8(t);
	WM(EAD,t);
	RET (6);
}

/* $44 LSRA inherent -0*-* */
lsra:
{
	CLR_NZC;
	CC |= (A & CC_C);
	A >>= 1;
	SET_Z8(A);
	RET(2);	
}

/* $54 LSRB inherent -0*-* */
lsrb:
{
	CLR_NZC;
	CC |= (B & CC_C);
	B >>= 1;
	SET_Z8(B);
	RET(2);	
}

/* $64 LSR indexed -0*-* */
lsr_ix:
{
	UINT8 t;
	fetch_effective_address();
	t=RM(EAD);
	CLR_NZC;
	CC |= (t & CC_C);
	t>>=1; SET_Z8(t);
	WM(EAD,t);
	RET(6);	
}

/* $74 LSR extended -0*-* */
lsr_ex:
{
	UINT8 t;
	EXTBYTE(t); CLR_NZC; CC |= (t & CC_C);
	t>>=1; SET_Z8(t);
	WM(EAD,t);
	RET(7);	
}

/* $07 ASR direct ?**-* */
asr_di:
{
	UINT8 t;
	DIRBYTE(t);
	CLR_NZC;
	CC |= (t & CC_C);
	t = (t & 0x80) | (t >> 1);
	SET_NZ8(t);
	WM(EAD,t);
	RET (6);	
}

/* $47 ASRA inherent ?**-* */
asra:
{
	CLR_NZC;
	CC |= (A & CC_C);
	A = (A & 0x80) | (A >> 1);
	SET_NZ8(A);
	RET(2);	
}

/* $57 ASRB inherent ?**-* */
asrb:
{
	CLR_NZC;
	CC |= (B & CC_C);
	B= (B & 0x80) | (B >> 1);
	SET_NZ8(B);
	RET(2);	
}

/* $67 ASR indexed ?**-* */
asr_ix:
{
	UINT8 t;
	fetch_effective_address();
	t=RM(EAD);
	CLR_NZC;
	CC |= (t & CC_C);
	t=(t&0x80)|(t>>1);
	SET_NZ8(t);
	WM(EAD,t);
	RET(6);	
}

/* $77 ASR extended ?**-* */
asr_ex:
{
	UINT8 t;
	EXTBYTE(t); CLR_NZC; CC |= (t & CC_C);
	t=(t&0x80)|(t>>1);
	SET_NZ8(t);
	WM(EAD,t);
	RET(7);	
}	

/* $08 ASL direct ?**** */
asl_di:
{
	UINT16 t,r;
	DIRBYTE(t);
	r = t << 1;
	CLR_NZVC;
	SET_FLAGS8(t,t,r);
	WM(EAD,r);
	RET (6);	
}

/* $48 ASLA inherent ?**** */
asla:
{
	UINT16 r;
	r = A << 1;
	CLR_NZVC;
	SET_FLAGS8(A,A,r);
	A = r;
	RET(2);	
}

/* $58 ASLB inherent ?**** */
aslb:
{
	UINT16 r;
	r = B << 1;
	CLR_NZVC;
	SET_FLAGS8(B,B,r);
	B = r;
	RET(2);	
}


/* $68 ASL indexed ?**** */
asl_ix:
{
	UINT16 t,r;
	fetch_effective_address();
	t=RM(EAD);
	r = t << 1;
	CLR_NZVC;
	SET_FLAGS8(t,t,r);
	WM(EAD,r);
	RET(6);	
}

/* $78 ASL extended ?**** */
asl_ex:
{
	UINT16 t,r;
	EXTBYTE(t); r=t<<1;
	CLR_NZVC; SET_FLAGS8(t,t,r);
	WM(EAD,r);
	RET(7);	
}	

/* $83 SUBD (CMPD CMPU) immediate -**** */
subd_im:
{
	UINT32 r,d;
	PAIR b;
	IMMWORD(b);
	d = D;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	D = r;
	RET(4);	
}

/* $93 SUBD (CMPD CMPU) direct -**** */
subd_di:
{
	UINT32 r,d;
	PAIR b;
	DIRWORD(b);
	d = D;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	D = r;
	RET(6);	
}


/* $a3 SUBD (CMPD CMPU) indexed -**** */
subd_ix:
{
	UINT32 r,d;
	PAIR b;
	fetch_effective_address();
    b.d=RM16(EAD);
	d = D;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	D = r;
	RET(6);	
}

/* $b3 SUBD (CMPD CMPU) extended -**** */
subd_ex:
{
	UINT32 r,d;
	PAIR b;
	EXTWORD(b);
	d = D;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	D = r;
	RET(7);	
}

 ///////////////////////////////////////////////////////////////////////
/* $01 ILLEGAL */

/* $02 ILLEGAL */

/* $03 COM direct -**01 */
com_di:
{
	UINT8 t;
	DIRBYTE(t);
	t = ~t;
	CLR_NZV;
	SET_NZ8(t);
	SEC;
	WM(EAD,t);
	RET (6);
}

/* $05 ILLEGAL */

/* $06 ROR direct -**-* */
ror_di:
{
	UINT8 t,r;
	DIRBYTE(t);
	r= (CC & CC_C) << 7;
	CLR_NZC;
	CC |= (t & CC_C);
	r |= t>>1;
	SET_NZ8(r);
	WM(EAD,r);
	RET (6);
}

/* $09 ROL direct -**** */
rol_di:
{
	UINT16 t,r;
	DIRBYTE(t);
	r = (CC & CC_C) | (t << 1);
	CLR_NZVC;
	SET_FLAGS8(t,t,r);
	WM(EAD,r);
	RET (6);	
}

/* $0A DEC direct -***- */
dec_di:
{
	UINT8 t;
	DIRBYTE(t);
	--t;
	CLR_NZV;
	SET_FLAGS8D(t);
	WM(EAD,t);
	RET (6);	
}

/* $0B ILLEGAL */

/* $OC INC direct -***- */
inc_di:
{
	UINT8 t;
	DIRBYTE(t);
	++t;
	CLR_NZV;
	SET_FLAGS8I(t);
	WM(EAD,t);
	RET (6);	
}

/* $OD TST direct -**0- */
tst_di:
{
	UINT8 t;
	DIRBYTE(t);
	CLR_NZV;
	SET_NZ8(t);
	RET (6);	
}

/* $0E JMP direct ----- */
jmp_di:
{
	DIRECT;
	PCD = EAD;
	CHANGE_PC;
	RET (3);	
}

/* $0F CLR direct -0100 */
clr_di:
{
	DIRECT;
	(void)RM(EAD);
	WM(EAD,0);
	CLR_NZVC;
	SEZ;
	RET (6);	
}

/* $10 FLAG */

/* $11 FLAG */

/* $12 NOP inherent ----- */
nop:
{
	goto m6809_Exec;
}

/* $13 SYNC inherent ----- */
sync:
{
	/* SYNC stops processing instructions until an interrupt request happens. */
	/* This doesn't require the corresponding interrupt to be enabled: if it */
	/* is disabled, execution continues with the next instruction. */
	m6809.int_state |= M6809_SYNC;	 /* HJB 990227 */
	CHECK_IRQ_LINES;
	/* if M6809_SYNC has not been cleared by CHECK_IRQ_LINES,
     * stop execution until the interrupt lines change. */
	if( m6809.int_state & M6809_SYNC )
		if (m6809.m6809_ICount > 0) 
		{
			m6809.m6809_ICount = 0;
			goto m6809_Exec_End;
		}
	RET (6);	
}

/* $14 ILLEGAL */

/* $15 ILLEGAL */

/* $16 LBRA relative ----- */
lbra:
{
	IMMWORD(ea);
	PC += EA;
	CHANGE_PC;
#if 0
	if ( EA == 0xfffd )  /* EHC 980508 speed up busy loop */
		if ( m6809.m6809_ICount > 0)
		{
			m6809.m6809_ICount = 0;
			goto m6809_Exec_End;
		}
#endif		
	RET (5);
}

/* $17 LBSR relative ----- */
lbsr:
{
	IMMWORD(ea);
	PUSHWORD(pPC);
	PC += EA;
	CHANGE_PC;
	RET (9);	
}

/* $18 ILLEGAL */

/* $19 DAA inherent (A) -**0* */
daa:
{
	UINT8 msn, lsn;
	UINT16 t, cf = 0;
	msn = A & 0xf0; lsn = A & 0x0f;
	if( lsn>0x09 || CC & CC_H) cf |= 0x06;
	if( msn>0x80 && lsn>0x09 ) cf |= 0x60;
	if( msn>0x90 || CC & CC_C) cf |= 0x60;
	t = cf + A;
	CLR_NZV; /* keep carry from previous operation */
	SET_NZ8((UINT8)t); SET_C8(t);
	A = t;
	RET (2);	
}

/* $1A ORCC immediate ##### */
orcc:
{
	UINT8 t;
	IMMBYTE(t);
	CC |= t;
	CHECK_IRQ_LINES;	/* HJB 990116 */
	RET (3);
}

/* $1B ILLEGAL */

/* $1C ANDCC immediate ##### */
andcc:
{
	UINT8 t;
	IMMBYTE(t);
	CC &= t;
	CHECK_IRQ_LINES;	/* HJB 990116 */
	RET (3);	
}

/* $1D SEX inherent -**-- */
sex:
{
	UINT16 t;
	t = SIGNED(B);
	D = t;
//  CLR_NZV;    Tim Lindner 20020905: verified that V flag is not affected
	CLR_NZ;
	SET_NZ16(t);
	RET (2);	
}

/* $1E EXG inherent ----- */
exg:
{
	UINT16 t1,t2;
	UINT8 tb;

	IMMBYTE(tb);
	if( (tb^(tb>>4)) & 0x08 )	/* HJB 990225: mixed 8/16 bit case? */
	{
		/* transfer $ff to both registers */
		t1 = t2 = 0xff;
	}
	else
	{
		switch(tb>>4) {
			case  0: t1 = D;  break;
			case  1: t1 = X;  break;
			case  2: t1 = Y;  break;
			case  3: t1 = U;  break;
			case  4: t1 = S;  break;
			case  5: t1 = PC; break;
			case  8: t1 = A;  break;
			case  9: t1 = B;  break;
			case 10: t1 = CC; break;
			case 11: t1 = DP; break;
			default: t1 = 0xff;
		}
		switch(tb&15) {
			case  0: t2 = D;  break;
			case  1: t2 = X;  break;
			case  2: t2 = Y;  break;
			case  3: t2 = U;  break;
			case  4: t2 = S;  break;
			case  5: t2 = PC; break;
			case  8: t2 = A;  break;
			case  9: t2 = B;  break;
			case 10: t2 = CC; break;
			case 11: t2 = DP; break;
			default: t2 = 0xff;
        }
	}
	switch(tb>>4) {
		case  0: D = t2;  break;
		case  1: X = t2;  break;
		case  2: Y = t2;  break;
		case  3: U = t2;  break;
		case  4: S = t2;  break;
		case  5: PC = t2; CHANGE_PC; break;
		case  8: A = t2;  break;
		case  9: B = t2;  break;
		case 10: CC = t2; break;
		case 11: DP = t2; break;
	}
	switch(tb&15) {
		case  0: D = t1;  break;
		case  1: X = t1;  break;
		case  2: Y = t1;  break;
		case  3: U = t1;  break;
		case  4: S = t1;  break;
		case  5: PC = t1; CHANGE_PC; break;
		case  8: A = t1;  break;
		case  9: B = t1;  break;
		case 10: CC = t1; break;
		case 11: DP = t1; break;
	}
	RET (8);	
}

/* $1F TFR inherent ----- */
tfr:
{
	UINT8 tb;
	UINT16 t;

	IMMBYTE(tb);
	if( (tb^(tb>>4)) & 0x08 )	/* HJB 990225: mixed 8/16 bit case? */
	{
		/* transfer $ff to register */
		t = 0xff;
    }
	else
	{
		switch(tb>>4) {
			case  0: t = D;  break;
			case  1: t = X;  break;
			case  2: t = Y;  break;
			case  3: t = U;  break;
			case  4: t = S;  break;
			case  5: t = PC; break;
			case  8: t = A;  break;
			case  9: t = B;  break;
			case 10: t = CC; break;
			case 11: t = DP; break;
			default: t = 0xff;
        }
	}
	switch(tb&15) {
		case  0: D = t;  break;
		case  1: X = t;  break;
		case  2: Y = t;  break;
		case  3: U = t;  break;
		case  4: S = t;  break;
		case  5: PC = t; CHANGE_PC; break;
		case  8: A = t;  break;
		case  9: B = t;  break;
		case 10: CC = t; break;
		case 11: DP = t; break;
    }
	RET (6);	
}

/* $20 BRA relative ----- */
bra:
{
	UINT8 t;
	IMMBYTE(t);
	PC += SIGNED(t);
    CHANGE_PC;
#if  0	
	/* JB 970823 - speed up busy loops */
	if( t == 0xfe )
		if( m6809.m6809_ICount > 0 )
		{
			m6809.m6809_ICount = 0;
			goto m6809_Exec_End;			
		}
#endif		
	RET (3);		
}

/* $21 BRN relative ----- */
brn:
{
	UINT8 t;
	IMMBYTE(t);
	RET (3);	
}

/* $1021 LBRN relative ----- */
lbrn:
{
	IMMWORD(ea);
	RET(5);
}

/* $22 BHI relative ----- */
bhi:
{
	BRANCH( !(CC & (CC_Z|CC_C)) );
	RET (3);
}

/* $1022 LBHI relative ----- */
lbhi:
{
	LBRANCH( !(CC & (CC_Z|CC_C)) );
	RET (5);
}

/* $23 BLS relative ----- */
bls:
{
	BRANCH( (CC & (CC_Z|CC_C)) );
	RET (3);	
}

/* $1023 LBLS relative ----- */
lbls:
{
	LBRANCH( (CC&(CC_Z|CC_C)) );
	RET (6);	
}

/* $24 BCC relative ----- */
bcc:
{
	BRANCH( !(CC&CC_C) );
	RET (6);	
}

/* $1024 LBCC relative ----- */
lbcc:
{
	LBRANCH( !(CC&CC_C) );
	RET (6);	
}

/* $25 BCS relative ----- */
bcs:
{
	BRANCH( (CC&CC_C) );
	RET (3);	
}

/* $1025 LBCS relative ----- */
lbcs:
{
	LBRANCH( (CC&CC_C) );
	RET (6);	
}

/* $26 BNE relative ----- */
bne:
{
	BRANCH( !(CC&CC_Z) );
	RET (3);	
}

/* $1026 LBNE relative ----- */
lbne:
{
	LBRANCH( !(CC&CC_Z) );
	RET (6);	
}

/* $27 BEQ relative ----- */
beq:
{
	BRANCH( (CC&CC_Z) );
	RET (3);	
}

/* $1027 LBEQ relative ----- */
lbeq:
{
	LBRANCH( (CC&CC_Z) );
	RET (6);	
}

/* $28 BVC relative ----- */
bvc:
{
	BRANCH( !(CC&CC_V) );
	RET (3);	
}

/* $1028 LBVC relative ----- */
lbvc:
{
	LBRANCH( !(CC&CC_V) );
	RET (6);	
}

/* $29 BVS relative ----- */
bvs:
{
	BRANCH( (CC&CC_V) );
	RET (3);	
}

/* $1029 LBVS relative ----- */
lbvs:
{
	LBRANCH( (CC&CC_V) );
	RET (6);	
}

/* $2A BPL relative ----- */
bpl:
{
	BRANCH( !(CC&CC_N) );
	RET (3);	
}

/* $102A LBPL relative ----- */
lbpl:
{
	LBRANCH( !(CC&CC_N) );
	RET (6);	
}

/* $2B BMI relative ----- */
bmi:
{
	BRANCH( (CC&CC_N) );
	RET (3);	
}

/* $102B LBMI relative ----- */
lbmi:
{
	LBRANCH( (CC&CC_N) );
	RET (6);	
}

/* $2C BGE relative ----- */
bge:
{
	BRANCH( !NXORV );
	RET (3);	
}

/* $102C LBGE relative ----- */
lbge:
{
	LBRANCH( !NXORV );
	RET (6);	
}

/* $2D BLT relative ----- */
blt:
{
	BRANCH( NXORV );
	RET (3);	
}

/* $102D LBLT relative ----- */
lblt:
{
	LBRANCH( NXORV );
	RET (6);	
}

/* $2E BGT relative ----- */
bgt:
{
	BRANCH( !(NXORV || (CC&CC_Z)) );
	RET (3);	
}

/* $102E LBGT relative ----- */
lbgt:
{
	LBRANCH( !(NXORV || (CC&CC_Z)) );
	RET (6);	
}

/* $2F BLE relative ----- */
ble:
{
	BRANCH( (NXORV || (CC&CC_Z)) );
	RET (3);	
}

/* $102F LBLE relative ----- */
lble:
{
	LBRANCH( (NXORV || (CC&CC_Z)) );
	RET (6);	
}

/* $30 LEAX indexed --*-- */
leax:
{
	fetch_effective_address();
    X = EA;
	CLR_Z;
	SET_Z(X);
	RET (4);	
}

/* $31 LEAY indexed --*-- */
leay:
{
	fetch_effective_address();
    Y = EA;
	CLR_Z;
	SET_Z(Y);
	RET (4);	
}

/* $32 LEAS indexed ----- */
leas:
{
	fetch_effective_address();
    S = EA;
	m6809.int_state |= M6809_LDS;
	RET (4);	
}

/* $33 LEAU indexed ----- */
leau:
{
	fetch_effective_address();
    U = EA;
	RET (4);	
}

/* $34 PSHS inherent ----- */
pshs:
{
	UINT8 t;
	IMMBYTE(t);
	if( t&0x80 ) { PUSHWORD(pPC); USE_CYCLES(2); }
	if( t&0x40 ) { PUSHWORD(pU);  USE_CYCLES(2); }
	if( t&0x20 ) { PUSHWORD(pY);  USE_CYCLES(2); }
	if( t&0x10 ) { PUSHWORD(pX);  USE_CYCLES(2); }
	if( t&0x08 ) { PUSHBYTE(DP);  USE_CYCLES(1); }
	if( t&0x04 ) { PUSHBYTE(B);   USE_CYCLES(1); }
	if( t&0x02 ) { PUSHBYTE(A);   USE_CYCLES(1); }
	if( t&0x01 ) { PUSHBYTE(CC);  USE_CYCLES(1); }

	RET(5);
}

/* $36 PSHU inherent ----- */
pshu:
{
	UINT8 t;
	IMMBYTE(t);
	if( t&0x80 ) { PSHUWORD(pPC); USE_CYCLES(2); }
	if( t&0x40 ) { PSHUWORD(pS);  USE_CYCLES(2); }
	if( t&0x20 ) { PSHUWORD(pY);  USE_CYCLES(2); }
	if( t&0x10 ) { PSHUWORD(pX);  USE_CYCLES(2); }
	if( t&0x08 ) { PSHUBYTE(DP);  USE_CYCLES(1); }
	if( t&0x04 ) { PSHUBYTE(B);   USE_CYCLES(1); }
	if( t&0x02 ) { PSHUBYTE(A);   USE_CYCLES(1); }
	if( t&0x01 ) { PSHUBYTE(CC);  USE_CYCLES(1); }

	RET(5);	
}

/* 35 PULS inherent ----- */
puls:
{
	UINT8 t;
	IMMBYTE(t);
	if( t&0x01 ) { PULLBYTE(CC); USE_CYCLES(1); }
	if( t&0x02 ) { PULLBYTE(A);  USE_CYCLES(1); }
	if( t&0x04 ) { PULLBYTE(B);  USE_CYCLES(1); }
	if( t&0x08 ) { PULLBYTE(DP); USE_CYCLES(1); }
	if( t&0x10 ) { PULLWORD(XD); USE_CYCLES(2); }
	if( t&0x20 ) { PULLWORD(YD); USE_CYCLES(2); }
	if( t&0x40 ) { PULLWORD(UD); USE_CYCLES(2); }
	if( t&0x80 ) { PULLWORD(PCD); CHANGE_PC; USE_CYCLES(2); }

	/* HJB 990225: moved check after all PULLs */
	if( t&0x01 ) { CHECK_IRQ_LINES; }
	RET(5);	
}

/* 37 PULU inherent ----- */
pulu:
{
	UINT8 t;
	IMMBYTE(t);
	if( t&0x01 ) { PULUBYTE(CC); USE_CYCLES(1); }
	if( t&0x02 ) { PULUBYTE(A);  USE_CYCLES(1); }
	if( t&0x04 ) { PULUBYTE(B);  USE_CYCLES(1); }
	if( t&0x08 ) { PULUBYTE(DP); USE_CYCLES(1); }
	if( t&0x10 ) { PULUWORD(XD); USE_CYCLES(2); }
	if( t&0x20 ) { PULUWORD(YD); USE_CYCLES(2); }
	if( t&0x40 ) { PULUWORD(SD); USE_CYCLES(2); }
	if( t&0x80 ) { PULUWORD(PCD); CHANGE_PC; USE_CYCLES(2); }

	/* HJB 990225: moved check after all PULLs */
	if( t&0x01 ) { CHECK_IRQ_LINES; }
	RET(5);	
}

/* $38 ILLEGAL */

/* $39 RTS inherent ----- */
rts:
{
	PULLWORD(PCD);
	CHANGE_PC;
	RET(5);	
}

/* $3A ABX inherent ----- */
abx:
{
	X += B;
	RET(3);	
}

/* $3B RTI inherent ##### */
rti:
{
	UINT8 t;
	PULLBYTE(CC);
	t = CC & CC_E;		/* HJB 990225: entire state saved? */
	if(t)
	{
        USE_CYCLES(9);
		PULLBYTE(A);
		PULLBYTE(B);
		PULLBYTE(DP);
		PULLWORD(XD);
		PULLWORD(YD);
		PULLWORD(UD);
	}
	PULLWORD(PCD);
	CHANGE_PC;
	CHECK_IRQ_LINES;	/* HJB 990116 */
	RET(6);	
}

/* $3C CWAI inherent ----1 */
cwai:
{
	UINT8 t;
	IMMBYTE(t);
	CC &= t;
	/*
     * CWAI stacks the entire machine state on the hardware stack,
     * then waits for an interrupt; when the interrupt is taken
     * later, the state is *not* saved again after CWAI.
     */
	CC |= CC_E; 		/* HJB 990225: save entire state */
	PUSHWORD(pPC);
	PUSHWORD(pU);
	PUSHWORD(pY);
	PUSHWORD(pX);
	PUSHBYTE(DP);
	PUSHBYTE(B);
	PUSHBYTE(A);
	PUSHBYTE(CC);
	m6809.int_state |= M6809_CWAI;	 /* HJB 990228 */
    CHECK_IRQ_LINES;    /* HJB 990116 */
	if( m6809.int_state & M6809_CWAI )
		if( m6809.m6809_ICount > 0 )
		{
			m6809.m6809_ICount = 0;
			goto m6809_Exec_End;
		}
	RET(20);		
}

/* $3D MUL inherent --*-@ */
mul:
{
	UINT16 t;
	t = A * B;
	CLR_ZC; SET_Z16(t); if(t&0x80) SEC;
	D = t;
	RET(11);	
}

/* $3E ILLEGAL */

/* $3F SWI (SWI2 SWI3) absolute indirect ----- */
swi:
{
	oSWI(); // HJB 980225: save entire state 
	CC |= CC_IF | CC_II;	/* inhibit FIRQ and IRQ */
	PCD=RM16(0xfffa);
	CHANGE_PC;
	RET(19);	
}

/* $103F SWI2 absolute indirect ----- */
swi2:
{
	oSWI(); // HJB 980225: save entire state
	PCD = RM16(0xfff4);
	CHANGE_PC;
	RET(20);	
}

/* $113F SWI3 absolute indirect ----- */
swi3:
{
	oSWI(); // HJB 980225: save entire state 
	PCD = RM16(0xfff2);
	CHANGE_PC;
	RET(20);	
}

/* $41 ILLEGAL */

/* $42 ILLEGAL */

/* $43 COMA inherent -**01 */
coma:
{
	A = ~A;
	CLR_NZV;
	SET_NZ8(A);
	SEC;
	RET(2);	
}

/* $45 ILLEGAL */

/* $46 RORA inherent -**-* */
rora:
{
	UINT8 r;
	r = (CC & CC_C) << 7;
	CLR_NZC;
	CC |= (A & CC_C);
	r |= A >> 1;
	SET_NZ8(r);
	A = r;
	RET(2);
}

/* $49 ROLA inherent -**** */
rola:
{
	UINT16 t,r;
	t = A;
	r = (CC & CC_C) | (t<<1);
	CLR_NZVC; SET_FLAGS8(t,t,r);
	A = r;
	RET(2);	
}

/* $4A DECA inherent -***- */
deca:
{
	--A;
	CLR_NZV;
	SET_FLAGS8D(A);
	RET(2);	
}

/* $4B ILLEGAL */

/* $4C INCA inherent -***- */
inca:
{
	++A;
	CLR_NZV;
	SET_FLAGS8I(A);
	RET(2);	
}

/* $4D TSTA inherent -**0- */
tsta:
{
	CLR_NZV;
	SET_NZ8(A);
	RET(2);	
}

/* $4E ILLEGAL */

/* $4F CLRA inherent -0100 */
clra:
{
	A = 0;
	CLR_NZVC; SEZ;
	RET(2);	
}

/* $51 ILLEGAL */

/* $52 ILLEGAL */

/* $53 COMB inherent -**01 */
comb:
{
	B = ~B;
	CLR_NZV;
	SET_NZ8(B);
	SEC;
	RET(2);		
}

/* $55 ILLEGAL */

/* $56 RORB inherent -**-* */
rorb:
{
	UINT8 r;
	r = (CC & CC_C) << 7;
	CLR_NZC;
	CC |= (B & CC_C);
	r |= B >> 1;
	SET_NZ8(r);
	B = r;
	RET(2);	
}

/* $59 ROLB inherent -**** */
rolb:
{
	UINT16 t,r;
	t = B;
	r = CC & CC_C;
	r |= t << 1;
	CLR_NZVC;
	SET_FLAGS8(t,t,r);
	B = r;
	RET(2);	
}

/* $5A DECB inherent -***- */
decb:
{
	--B;
	CLR_NZV;
	SET_FLAGS8D(B);
	RET(2);	
}

/* $5B ILLEGAL */

/* $5C INCB inherent -***- */
incb:
{
	++B;
	CLR_NZV;
	SET_FLAGS8I(B);
	RET(2);	
}

/* $5D TSTB inherent -**0- */
tstb:
{
	CLR_NZV;
	SET_NZ8(B);
	RET(2);	
}

/* $5E ILLEGAL */

/* $5F CLRB inherent -0100 */
clrb:
{
	B = 0;
	CLR_NZVC; SEZ;
	RET(2);	
}

/* $61 ILLEGAL */

/* $62 ILLEGAL */

/* $63 COM indexed -**01 */
com_ix:
{
	UINT8 t;
	fetch_effective_address();
	t = ~RM(EAD);
	CLR_NZV;
	SET_NZ8(t);
	SEC;
	WM(EAD,t);
	RET(6);	
}

/* $65 ILLEGAL */

/* $66 ROR indexed -**-* */
ror_ix:
{
	UINT8 t,r;
	fetch_effective_address();
	t=RM(EAD);
	r = (CC & CC_C) << 7;
	CLR_NZC;
	CC |= (t & CC_C);
	r |= t>>1; SET_NZ8(r);
	WM(EAD,r);
	RET(6);	
}

/* $69 ROL indexed -**** */
rol_ix:
{
	UINT16 t,r;
	fetch_effective_address();
	t=RM(EAD);
	r = CC & CC_C;
	r |= t << 1;
	CLR_NZVC;
	SET_FLAGS8(t,t,r);
	WM(EAD,r);
	RET(6);	
}

/* $6A DEC indexed -***- */
dec_ix:
{
	UINT8 t;
	fetch_effective_address();
	t = RM(EAD) - 1;
	CLR_NZV; SET_FLAGS8D(t);
	WM(EAD,t);
	RET(6);	
}

/* $6B ILLEGAL */

/* $6C INC indexed -***- */
inc_ix:
{
	UINT8 t;
	fetch_effective_address();
	t = RM(EAD) + 1;
	CLR_NZV; SET_FLAGS8I(t);
	WM(EAD,t);
	RET(6);	
}

/* $6D TST indexed -**0- */
tst_ix:
{
	UINT8 t;
	fetch_effective_address();
	t = RM(EAD);
	CLR_NZV;
	SET_NZ8(t);
	RET(6);	
}

/* $6E JMP indexed ----- */
jmp_ix:
{
	fetch_effective_address();
	PCD = EAD;
	CHANGE_PC;
	RET(3);	
}

/* $6F CLR indexed -0100 */
clr_ix:
{
	fetch_effective_address();
	(void)RM(EAD);
	WM(EAD,0);
	CLR_NZVC; SEZ;
	RET(6);	
}

/* $71 ILLEGAL */

/* $72 ILLEGAL */

/* $73 COM extended -**01 */
com_ex:
{
	UINT8 t;
	EXTBYTE(t); t = ~t;
	CLR_NZV; SET_NZ8(t); SEC;
	WM(EAD,t);
	RET(7);	
}

/* $75 ILLEGAL */

/* $76 ROR extended -**-* */
ror_ex:
{
	UINT8 t,r;
	EXTBYTE(t); r=(CC & CC_C) << 7;
	CLR_NZC; CC |= (t & CC_C);
	r |= t>>1; SET_NZ8(r);
	WM(EAD,r);
	RET(7);	
}

/* $79 ROL extended -**** */
rol_ex:
{
	UINT16 t,r;
	EXTBYTE(t); r = (CC & CC_C) | (t << 1);
	CLR_NZVC; SET_FLAGS8(t,t,r);
	WM(EAD,r);
	RET(7);	
}

/* $7A DEC extended -***- */
dec_ex:
{
	UINT8 t;
	EXTBYTE(t); --t;
	CLR_NZV; SET_FLAGS8D(t);
	WM(EAD,t);
	RET(7);	
}

/* $7B ILLEGAL */

/* $7C INC extended -***- */
inc_ex:
{
	UINT8 t;
	EXTBYTE(t); ++t;
	CLR_NZV; SET_FLAGS8I(t);
	WM(EAD,t);
	RET(7);	
}

/* $7D TST extended -**0- */
tst_ex:
{
	UINT8 t;
	EXTBYTE(t); CLR_NZV; SET_NZ8(t);
	RET(7);	
}

/* $7E JMP extended ----- */
jmp_ex:
{
	EXTENDED;
	PCD = EAD;
	CHANGE_PC;
	RET(4);	
}

/* $7F CLR extended -0100 */
clr_ex:
{
	EXTENDED;
	(void)RM(EAD);
	WM(EAD,0);
	CLR_NZVC; SEZ;
	RET(7);	
}

/* $81 CMPA immediate ?**** */
cmpa_im:
{
	UINT16	  t,r;
	IMMBYTE(t);
	r = A - t;
	CLR_NZVC;
	SET_FLAGS8(A,t,r);
	RET(2);	
}

/* $1083 CMPD immediate -**** */
cmpd_im:
{
	UINT32 r,d;
	PAIR b;
	IMMWORD(b);
	d = D;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(5);	
}

/* $1183 CMPU immediate -**** */
cmpu_im:
{
	UINT32 r, d;
	PAIR b;
	IMMWORD(b);
	d = U;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(5);	
}

/* $84 ANDA immediate -**0- */
anda_im:
{
	UINT8 t;
	IMMBYTE(t);
	A &= t;
	CLR_NZV;
	SET_NZ8(A);
	RET(2);	
}

/* $85 BITA immediate -**0- */
bita_im:
{
	UINT8 t,r;
	IMMBYTE(t);
	r = A & t;
	CLR_NZV;
	SET_NZ8(r);
	RET(2);	
}

/* $86 LDA immediate -**0- */
lda_im:
{
	IMMBYTE(A);
	CLR_NZV;
	SET_NZ8(A);
	RET(2);	
}

/* is this a legal instruction? */
/* $87 STA immediate -**0- */
sta_im:
{
	CLR_NZV;
	SET_NZ8(A);
	IMM8;
	WM(EAD,A);
	RET(2);	
}

/* $88 EORA immediate -**0- */
eora_im:
{
	UINT8 t;
	IMMBYTE(t);
	A ^= t;
	CLR_NZV;
	SET_NZ8(A);
	RET(2);	
}

/* $8A ORA immediate -**0- */
ora_im:
{
	UINT8 t;
	IMMBYTE(t);
	A |= t;
	CLR_NZV;
	SET_NZ8(A);
	RET(2);	
}

/* $8C CMPX (CMPY CMPS) immediate -**** */
cmpx_im:
{
	UINT32 r,d;
	PAIR b;
	IMMWORD(b);
	d = X;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(4);	
}

/* $108C CMPY immediate -**** */
cmpy_im:
{
	UINT32 r,d;
	PAIR b;
	IMMWORD(b);
	d = Y;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(5);	
}

/* $118C CMPS immediate -**** */
cmps_im:
{
	UINT32 r,d;
	PAIR b;
	IMMWORD(b);
	d = S;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(5);	
}

/* $8D BSR ----- */
bsr:
{
	UINT8 t;
	IMMBYTE(t);
	PUSHWORD(pPC);
	PC += SIGNED(t);
	CHANGE_PC;
	RET(7);	
}

/* $8E LDX (LDY) immediate -**0- */
ldx_im:
{
	IMMWORD(pX);
	CLR_NZV;
	SET_NZ16(X);
	RET(3);	
}

/* $108E LDY immediate -**0- */
ldy_im:
{
	IMMWORD(pY);
	CLR_NZV;
	SET_NZ16(Y);
	RET(4);	
}

/* is this a legal instruction? */
/* $8F STX (STY) immediate -**0- */
stx_im:
{
	CLR_NZV;
	SET_NZ16(X);
	IMM16;
	WM16(EAD,&pX);
	RET(2);
}

/* is this a legal instruction? */
/* $108F STY immediate -**0- */
sty_im:
{
	CLR_NZV;
	SET_NZ16(Y);
	IMM16;
	WM16(EAD,&pY);
	RET(4);	
}

/* $91 CMPA direct ?**** */
cmpa_di:
{
	UINT16	  t,r;
	DIRBYTE(t);
	r = A - t;
	CLR_NZVC;
	SET_FLAGS8(A,t,r);
	RET(4);	
}

/* $1093 CMPD direct -**** */
cmpd_di:
{
	UINT32 r,d;
	PAIR b;
	DIRWORD(b);
	d = D;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(7);	
}

/* $1193 CMPU direct -**** */
cmpu_di:
{
	UINT32 r,d;
	PAIR b;
	DIRWORD(b);
	d = U;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(U,b.d,r);
	RET(7);	
}

/* $94 ANDA direct -**0- */
anda_di:
{
	UINT8 t;
	DIRBYTE(t);
	A &= t;
	CLR_NZV;
	SET_NZ8(A);
	RET(4);	
}

/* $95 BITA direct -**0- */
bita_di:
{
	UINT8 t,r;
	DIRBYTE(t);
	r = A & t;
	CLR_NZV;
	SET_NZ8(r);
	RET(4);	
}

/* $96 LDA direct -**0- */
lda_di:
{
	DIRBYTE(A);
	CLR_NZV;
	SET_NZ8(A);
	RET(4);	
}

/* $97 STA direct -**0- */
sta_di:
{
	CLR_NZV;
	SET_NZ8(A);
	DIRECT;
	WM(EAD,A);
	RET(4);	
}

/* $98 EORA direct -**0- */
eora_di:
{
	UINT8 t;
	DIRBYTE(t);
	A ^= t;
	CLR_NZV;
	SET_NZ8(A);
	RET(4);	
}

/* $9A ORA direct -**0- */
ora_di:
{
	UINT8 t;
	DIRBYTE(t);
	A |= t;
	CLR_NZV;
	SET_NZ8(A);
	RET(4);	
}

/* $9C CMPX (CMPY CMPS) direct -**** */
cmpx_di:
{
	UINT32 r,d;
	PAIR b;
	DIRWORD(b);
	d = X;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(6);	
}

/* $109C CMPY direct -**** */
cmpy_di:
{
	UINT32 r,d;
	PAIR b;
	DIRWORD(b);
	d = Y;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(7);	
}

/* $119C CMPS direct -**** */
cmps_di:
{
	UINT32 r,d;
	PAIR b;
	DIRWORD(b);
	d = S;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(7);	
}

/* $9D JSR direct ----- */
jsr_di:
{
	DIRECT;
	PUSHWORD(pPC);
	PCD = EAD;
	CHANGE_PC;
	RET(7);	
}

/* $9E LDX (LDY) direct -**0- */
ldx_di:
{
	DIRWORD(pX);
	CLR_NZV;
	SET_NZ16(X);
	RET(5);	
}

/* $109E LDY direct -**0- */
ldy_di:
{
	DIRWORD(pY);
	CLR_NZV;
	SET_NZ16(Y);
	RET(6);	
}

/* $9F STX (STY) direct -**0- */
stx_di:
{
	CLR_NZV;
	SET_NZ16(X);
	DIRECT;
	WM16(EAD,&pX);
	RET(5);	
}

/* $109F STY direct -**0- */
sty_di:
{
	CLR_NZV;
	SET_NZ16(Y);
	DIRECT;
	WM16(EAD,&pY);
	RET(6);	
}

/* $a1 CMPA indexed ?**** */
cmpa_ix:
{
	UINT16 t,r;
	fetch_effective_address();
	t = RM(EAD);
	r = A - t;
	CLR_NZVC;
	SET_FLAGS8(A,t,r);
	RET(4);	
}

/* $10a3 CMPD indexed -**** */
cmpd_ix:
{
	UINT32 r,d;
	PAIR b;
	fetch_effective_address();
    b.d=RM16(EAD);
	d = D;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(7);	
}

/* $11a3 CMPU indexed -**** */
cmpu_ix:
{
	UINT32 r;
	PAIR b;
	fetch_effective_address();
    b.d=RM16(EAD);
	r = U - b.d;
	CLR_NZVC;
	SET_FLAGS16(U,b.d,r);
	RET(7);	
}

/* $a4 ANDA indexed -**0- */
anda_ix:
{
	fetch_effective_address();
	A &= RM(EAD);
	CLR_NZV;
	SET_NZ8(A);
	RET(4);	
}

/* $a5 BITA indexed -**0- */
bita_ix:
{
	UINT8 r;
	fetch_effective_address();
	r = A & RM(EAD);
	CLR_NZV;
	SET_NZ8(r);
	RET(4);	
}

/* $a6 LDA indexed -**0- */
lda_ix:
{
	fetch_effective_address();
	A = RM(EAD);
	CLR_NZV;
	SET_NZ8(A);
	RET(4);	
}

/* $a7 STA indexed -**0- */
sta_ix:
{
	fetch_effective_address();
    CLR_NZV;
	SET_NZ8(A);
	WM(EAD,A);
	RET(4);	
}

/* $a8 EORA indexed -**0- */
eora_ix:
{
	fetch_effective_address();
	A ^= RM(EAD);
	CLR_NZV;
	SET_NZ8(A);
	RET(4);	
}

/* $aA ORA indexed -**0- */
ora_ix:
{
	fetch_effective_address();
	A |= RM(EAD);
	CLR_NZV;
	SET_NZ8(A);
	RET(4);	
}

/* $aC CMPX (CMPY CMPS) indexed -**** */
cmpx_ix:
{
	UINT32 r,d;
	PAIR b;
	fetch_effective_address();
    b.d=RM16(EAD);
	d = X;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(6);	
}

/* $10aC CMPY indexed -**** */
cmpy_ix:
{
	UINT32 r,d;
	PAIR b;
	fetch_effective_address();
    b.d=RM16(EAD);
	d = Y;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(7);	
}

/* $11aC CMPS indexed -**** */
cmps_ix:
{
	UINT32 r,d;
	PAIR b;
	fetch_effective_address();
    b.d=RM16(EAD);
	d = S;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(7);	
}

/* $aD JSR indexed ----- */
jsr_ix:
{
	fetch_effective_address();
    PUSHWORD(pPC);
	PCD = EAD;
	CHANGE_PC;
	RET(7);	
}

/* $aE LDX (LDY) indexed -**0- */
ldx_ix:
{
	fetch_effective_address();
    X=RM16(EAD);
	CLR_NZV;
	SET_NZ16(X);
	RET(5);	
}

/* $10aE LDY indexed -**0- */
ldy_ix:
{
	fetch_effective_address();
    Y=RM16(EAD);
	CLR_NZV;
	SET_NZ16(Y);
	RET(6);	
}

/* $aF STX (STY) indexed -**0- */
stx_ix:
{
	fetch_effective_address();
    CLR_NZV;
	SET_NZ16(X);
	WM16(EAD,&pX);
	RET(5);	
}

/* $10aF STY indexed -**0- */
sty_ix:
{
	fetch_effective_address();
    CLR_NZV;
	SET_NZ16(Y);
	WM16(EAD,&pY);
	RET(6);	
}

/* $b1 CMPA extended ?**** */
cmpa_ex:
{
	UINT16	  t,r;
	EXTBYTE(t);
	r = A - t;
	CLR_NZVC;
	SET_FLAGS8(A,t,r);
	RET(5);	
}

/* $10b3 CMPD extended -**** */
cmpd_ex:
{
	UINT32 r,d;
	PAIR b;
	EXTWORD(b);
	d = D;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(8);	
}

/* $11b3 CMPU extended -**** */
cmpu_ex:
{
	UINT32 r,d;
	PAIR b;
	EXTWORD(b);
	d = U;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(8);	
}

/* $b4 ANDA extended -**0- */
anda_ex:
{
	UINT8 t;
	EXTBYTE(t);
	A &= t;
	CLR_NZV;
	SET_NZ8(A);
	RET(5);	
}

/* $b5 BITA extended -**0- */
bita_ex:
{
	UINT8 t,r;
	EXTBYTE(t);
	r = A & t;
	CLR_NZV; SET_NZ8(r);
	RET(5);	
}

/* $b6 LDA extended -**0- */
lda_ex:
{
	EXTBYTE(A);
	CLR_NZV;
	SET_NZ8(A);
	RET(5);	
}

/* $b7 STA extended -**0- */
sta_ex:
{
	CLR_NZV;
	SET_NZ8(A);
	EXTENDED;
	WM(EAD,A);
	RET(5);	
}

/* $b8 EORA extended -**0- */
eora_ex:
{
	UINT8 t;
	EXTBYTE(t);
	A ^= t;
	CLR_NZV;
	SET_NZ8(A);
	RET(5);	
}

/* $bA ORA extended -**0- */
ora_ex:
{
	UINT8 t;
	EXTBYTE(t);
	A |= t;
	CLR_NZV;
	SET_NZ8(A);
	RET(5);	
}

/* $bC CMPX (CMPY CMPS) extended -**** */
cmpx_ex:
{
	UINT32 r,d;
	PAIR b;
	EXTWORD(b);
	d = X;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(7);	
}

/* $10bC CMPY extended -**** */
cmpy_ex:
{
	UINT32 r,d;
	PAIR b;
	EXTWORD(b);
	d = Y;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(8);	
}

/* $11bC CMPS extended -**** */
cmps_ex:
{
	UINT32 r,d;
	PAIR b;
	EXTWORD(b);
	d = S;
	r = d - b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	RET(8);	
}

/* $bD JSR extended ----- */
jsr_ex:
{
	EXTENDED;
	PUSHWORD(pPC);
	PCD = EAD;
	CHANGE_PC;
	RET(8);	
}

/* $bE LDX (LDY) extended -**0- */
ldx_ex:
{
	EXTWORD(pX);
	CLR_NZV;
	SET_NZ16(X);
	RET(6);	
}

/* $10bE LDY extended -**0- */
ldy_ex:
{
	EXTWORD(pY);
	CLR_NZV;
	SET_NZ16(Y);
	RET(7);	
}

/* $bF STX (STY) extended -**0- */
stx_ex:
{
	CLR_NZV;
	SET_NZ16(X);
	EXTENDED;
	WM16(EAD,&pX);
	RET(6);	
}

/* $10bF STY extended -**0- */
sty_ex:
{
	CLR_NZV;
	SET_NZ16(Y);
	EXTENDED;
	WM16(EAD,&pY);
	RET(7);	
}

/* $c1 CMPB immediate ?**** */
cmpb_im:
{
	UINT16	  t,r;
	IMMBYTE(t);
	r = B - t;
	CLR_NZVC; SET_FLAGS8(B,t,r);
	RET(2);	
}

/* $c3 ADDD immediate -**** */
addd_im:
{
	UINT32 r,d;
	PAIR b;
	IMMWORD(b);
	d = D;
	r = d + b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	D = r;
	RET(4);	
}

/* $c4 ANDB immediate -**0- */
andb_im:
{
	UINT8 t;
	IMMBYTE(t);
	B &= t;
	CLR_NZV;
	SET_NZ8(B);
	RET(2);	
}

/* $c5 BITB immediate -**0- */
bitb_im:
{
	UINT8 t,r;
	IMMBYTE(t);
	r = B & t;
	CLR_NZV;
	SET_NZ8(r);
	RET(2);	
}

/* $c6 LDB immediate -**0- */
ldb_im:
{
	IMMBYTE(B);
	CLR_NZV;
	SET_NZ8(B);
	RET(2);	
}

/* is this a legal instruction? */
/* $c7 STB immediate -**0- */
stb_im:
{
	CLR_NZV;
	SET_NZ8(B);
	IMM8;
	WM(EAD,B);
	RET(2);	
}

/* $c8 EORB immediate -**0- */
eorb_im:
{
	UINT8 t;
	IMMBYTE(t);
	B ^= t;
	CLR_NZV;
	SET_NZ8(B);
	RET(2);	
}

/* $cA ORB immediate -**0- */
orb_im:
{
	UINT8 t;
	IMMBYTE(t);
	B |= t;
	CLR_NZV;
	SET_NZ8(B);
	RET(2);	
}

/* $cC LDD immediate -**0- */
ldd_im:
{
	IMMWORD(pD);
	CLR_NZV;
	SET_NZ16(D);
	RET(3);	
}

/* is this a legal instruction? */
/* $cD STD immediate -**0- */
std_im:
{
	CLR_NZV;
	SET_NZ16(D);
    IMM16;
	WM16(EAD,&pD);
	RET(2);	
}

/* $cE LDU (LDS) immediate -**0- */
ldu_im:
{
	IMMWORD(pU);
	CLR_NZV;
	SET_NZ16(U);
	RET(3);	
}

/* $10cE LDS immediate -**0- */
lds_im:
{
	IMMWORD(pS);
	CLR_NZV;
	SET_NZ16(S);
	m6809.int_state |= M6809_LDS;
	RET(4);	
}

/* is this a legal instruction? */
/* $cF STU (STS) immediate -**0- */
stu_im:
{
	CLR_NZV;
	SET_NZ16(U);
    IMM16;
	WM16(EAD,&pU);
	RET(3);	
}

/* is this a legal instruction? */
/* $10cF STS immediate -**0- */
sts_im:
{
	CLR_NZV;
	SET_NZ16(S);
    IMM16;
	WM16(EAD,&pS);
	RET(4);	
}

/* $d1 CMPB direct ?**** */
cmpb_di:
{
	UINT16	  t,r;
	DIRBYTE(t);
	r = B - t;
	CLR_NZVC;
	SET_FLAGS8(B,t,r);
	RET(4);	
}

/* $d3 ADDD direct -**** */
addd_di:
{
	UINT32 r,d;
	PAIR b;
	DIRWORD(b);
	d = D;
	r = d + b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	D = r;
	RET(6);	
}

/* $d4 ANDB direct -**0- */
andb_di:
{
	UINT8 t;
	DIRBYTE(t);
	B &= t;
	CLR_NZV;
	SET_NZ8(B);
	RET(4);	
}

/* $d5 BITB direct -**0- */
bitb_di:
{
	UINT8 t,r;
	DIRBYTE(t);
	r = B & t;
	CLR_NZV;
	SET_NZ8(r);
	RET(4);	
}

/* $d6 LDB direct -**0- */
ldb_di:
{
	DIRBYTE(B);
	CLR_NZV;
	SET_NZ8(B);
	RET(4);	
}

/* $d7 STB direct -**0- */
stb_di:
{
	CLR_NZV;
	SET_NZ8(B);
	DIRECT;
	WM(EAD,B);
	RET(4);	
}

/* $d8 EORB direct -**0- */
eorb_di:
{
	UINT8 t;
	DIRBYTE(t);
	B ^= t;
	CLR_NZV;
	SET_NZ8(B);
	RET(4);	
}

/* $dA ORB direct -**0- */
orb_di:
{
	UINT8 t;
	DIRBYTE(t);
	B |= t;
	CLR_NZV;
	SET_NZ8(B);
	RET(4);	
}

/* $dC LDD direct -**0- */
ldd_di:
{
	DIRWORD(pD);
	CLR_NZV;
	SET_NZ16(D);
	RET(5);	
}

/* $dD STD direct -**0- */
std_di:
{
	CLR_NZV;
	SET_NZ16(D);
    DIRECT;
	WM16(EAD,&pD);
	RET(5);	
}

/* $dE LDU (LDS) direct -**0- */
ldu_di:
{
	DIRWORD(pU);
	CLR_NZV;
	SET_NZ16(U);
	RET(5);	
}

/* $10dE LDS direct -**0- */
lds_di:
{
	DIRWORD(pS);
	CLR_NZV;
	SET_NZ16(S);
	m6809.int_state |= M6809_LDS;
	RET(6);	
}

/* $dF STU (STS) direct -**0- */
stu_di:
{
	CLR_NZV;
	SET_NZ16(U);
	DIRECT;
	WM16(EAD,&pU);
	RET(5);	
}

/* $10dF STS direct -**0- */
sts_di:
{
	CLR_NZV;
	SET_NZ16(S);
	DIRECT;
	WM16(EAD,&pS);
	RET(6);	
}

/* $e1 CMPB indexed ?**** */
cmpb_ix:
{
	UINT16	  t,r;
	fetch_effective_address();
	t = RM(EAD);
	r = B - t;
	CLR_NZVC;
	SET_FLAGS8(B,t,r);
	RET(4);	
}

/* $e3 ADDD indexed -**** */
addd_ix:
{
	UINT32 r,d;
    PAIR b;
    fetch_effective_address();
	b.d=RM16(EAD);
	d = D;
	r = d + b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	D = r;
	RET(6);	
}

/* $e4 ANDB indexed -**0- */
andb_ix:
{
	fetch_effective_address();
	B &= RM(EAD);
	CLR_NZV;
	SET_NZ8(B);
	RET(4);	
}

/* $e5 BITB indexed -**0- */
bitb_ix:
{
	UINT8 r;
	fetch_effective_address();
	r = B & RM(EAD);
	CLR_NZV;
	SET_NZ8(r);
	RET(4);	
}

/* $e6 LDB indexed -**0- */
ldb_ix:
{
	fetch_effective_address();
	B = RM(EAD);
	CLR_NZV;
	SET_NZ8(B);
	RET(4);	
}

/* $e7 STB indexed -**0- */
stb_ix:
{
	fetch_effective_address();
    CLR_NZV;
	SET_NZ8(B);
	WM(EAD,B);
	RET(4);	
}

/* $e8 EORB indexed -**0- */
eorb_ix:
{
	fetch_effective_address();
	B ^= RM(EAD);
	CLR_NZV;
	SET_NZ8(B);
	RET(4);	
}

/* $eA ORB indexed -**0- */
orb_ix:
{
	fetch_effective_address();
	B |= RM(EAD);
	CLR_NZV;
	SET_NZ8(B);
	RET(4);	
}

/* $eC LDD indexed -**0- */
ldd_ix:
{
	fetch_effective_address();
    D=RM16(EAD);
	CLR_NZV; SET_NZ16(D);
	RET(5);	
}

/* $eD STD indexed -**0- */
std_ix:
{
	fetch_effective_address();
    CLR_NZV;
	SET_NZ16(D);
	WM16(EAD,&pD);
	RET(5);	
}

/* $eE LDU (LDS) indexed -**0- */
ldu_ix:
{
	fetch_effective_address();
    U=RM16(EAD);
	CLR_NZV;
	SET_NZ16(U);
	RET(5);	
}

/* $10eE LDS indexed -**0- */
lds_ix:
{
	fetch_effective_address();
    S=RM16(EAD);
	CLR_NZV;
	SET_NZ16(S);
	m6809.int_state |= M6809_LDS;
	RET(6);	
}

/* $eF STU (STS) indexed -**0- */
stu_ix:
{
	fetch_effective_address();
    CLR_NZV;
	SET_NZ16(U);
	WM16(EAD,&pU);
	RET(5);	
}

/* $10eF STS indexed -**0- */
sts_ix:
{
	fetch_effective_address();
    CLR_NZV;
	SET_NZ16(S);
	WM16(EAD,&pS);
	RET(6);	
}

/* $f1 CMPB extended ?**** */
cmpb_ex:
{
	UINT16	  t,r;
	EXTBYTE(t);
	r = B - t;
	CLR_NZVC;
	SET_FLAGS8(B,t,r);
	RET(5);	
}

/* $f3 ADDD extended -**** */
addd_ex:
{
	UINT32 r,d;
	PAIR b;
	EXTWORD(b);
	d = D;
	r = d + b.d;
	CLR_NZVC;
	SET_FLAGS16(d,b.d,r);
	D = r;
	RET(7);	
}

/* $f4 ANDB extended -**0- */
andb_ex:
{
	UINT8 t;
	EXTBYTE(t);
	B &= t;
	CLR_NZV;
	SET_NZ8(B);
	RET(5);	
}

/* $f5 BITB extended -**0- */
bitb_ex:
{
	UINT8 t,r;
	EXTBYTE(t);
	r = B & t;
	CLR_NZV;
	SET_NZ8(r);
	RET(5);	
}

/* $f6 LDB extended -**0- */
ldb_ex:
{
	EXTBYTE(B);
	CLR_NZV;
	SET_NZ8(B);
	RET(5);	
}

/* $f7 STB extended -**0- */
stb_ex:
{
	CLR_NZV;
	SET_NZ8(B);
	EXTENDED;
	WM(EAD,B);
	RET(5);	
}

/* $f8 EORB extended -**0- */
eorb_ex:
{
	UINT8 t;
	EXTBYTE(t);
	B ^= t;
	CLR_NZV;
	SET_NZ8(B);
	RET(5);	
}

/* $fA ORB extended -**0- */
orb_ex:
{
	UINT8 t;
	EXTBYTE(t);
	B |= t;
	CLR_NZV;
	SET_NZ8(B);
	RET(5);	
}

/* $fC LDD extended -**0- */
ldd_ex:
{
	EXTWORD(pD);
	CLR_NZV;
	SET_NZ16(D);
	RET(6);	
}

/* $fD STD extended -**0- */
std_ex:
{
	CLR_NZV;
	SET_NZ16(D);
    EXTENDED;
	WM16(EAD,&pD);
	RET(6);	
}

/* $fE LDU (LDS) extended -**0- */
ldu_ex:
{
	EXTWORD(pU);
	CLR_NZV;
	SET_NZ16(U);
	RET(6);	
}

/* $10fE LDS extended -**0- */
lds_ex:
{
	EXTWORD(pS);
	CLR_NZV;
	SET_NZ16(S);
	m6809.int_state |= M6809_LDS;
	RET(7);	
}

/* $fF STU (STS) extended -**0- */
stu_ex:
{
	CLR_NZV;
	SET_NZ16(U);
	EXTENDED;
	WM16(EAD,&pU);
	RET(6);	
}

/* $10fF STS extended -**0- */
sts_ex:
{
	CLR_NZV;
	SET_NZ16(S);
	EXTENDED;
	WM16(EAD,&pS);
	RET(7);	
}

/* $10xx opcodes */
pref10:
{
	UINT8 ireg2 = ROP(PCD);
	PC++;
/*
//---------------
// vbt : test utilisation des opcodes	
	unsigned int *	DrvTempRom = (unsigned int *)0x00200000;		
	DrvTempRom[ireg2]++;
	if(DrvTempRom[ireg2]==0x000fffff)
		while(1);		
	*/
	DISPATCH10();
}

/* $11xx opcodes */
pref11:
{
	UINT8 ireg2 = ROP(PCD);
	PC++;

//---------------
// vbt : test utilisation des opcodes	
/*
	unsigned int *	DrvTempRom = (unsigned int *)0x00200000;		
	DrvTempRom[ireg2]++;
	if(DrvTempRom[ireg2]==0x000fffff)
		while(1);		
	*/
	DISPATCH11();
}
