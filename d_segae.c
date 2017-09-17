#include "d_segae.h"
// based on MESS/MAME driver by David Haywood

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvHangonjr = {
		"hangonjr", "segae", 
		"Hang-On Jr.", 
		hangonjrRomInfo, hangonjrRomName, HangonjrInputInfo, HangonjrDIPInfo,
		DrvHangonJrInit, DrvExit, DrvFrame, DrvDraw
	};

	struct BurnDriver nBurnDrvTetrisse = {
		"tetrisse", "segae",
		"Tetris (Japan, System E)",
		TetrisseRomInfo, TetrisseRomName, TetrisseInputInfo, TetrisseDIPInfo,
		DrvTetrisInit, DrvExit, DrvFrame, DrvDraw
	};

	struct BurnDriver nBurnDrvTransfrm = {
		"transfrm", "segae",
		"Transformer",
		TransfrmRomInfo, TransfrmRomName, TransfrmInputInfo, TransfrmDIPInfo,
		DrvTransfrmInit, DrvExit, DrvFrame, DrvDraw
	};

	if (strcmp(nBurnDrvHangonjr.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvHangonjr,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvTetrisse.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvTetrisse,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvTransfrm.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvTransfrm,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}

static UINT8 __fastcall systeme_main_read(UINT16 address)
{
//	bprintf(0, _T("systeme_main_read adr %X.\n"), address);

	return 0;
}

static void __fastcall systeme_main_write(UINT16 address, UINT8 data)
{
	if(address >= 0x8000 && address <= 0xbfff)
	{
		segae_vdp_vram [1-segae_8000bank][(address - 0x8000) + (0x4000-(segae_vdp_vrambank[1-segae_8000bank] * 0x4000))] = data;

		return;
	}
}

static UINT32 scalerange(UINT32 x, UINT32 in_min, UINT32 in_max, UINT32 out_min, UINT32 out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


static UINT8 scale_wheel(UINT32 PaddlePortnum) {
	UINT8 Temp;

	Temp = 0x7f + (PaddlePortnum >> 4);
	if (Temp < 0x01) Temp = 0x01;
	if (Temp > 0xfe) Temp = 0xfe;
	Temp = scalerange(Temp, 0x3f, 0xbe, 0x20, 0xe0);
	return Temp;
}

static UINT8 scale_accel(UINT32 PaddlePortnum) {
	UINT8 Temp;

	Temp = PaddlePortnum >> 4;
	if (Temp < 0x08) Temp = 0x00; // sometimes default for digital button -> analog is "1"
	if (Temp > 0x30) Temp = 0xff;

	return Temp;
}


static UINT8 __fastcall hangonjr_port_f8_read (UINT8 port)
{
	UINT8 temp = 0;
	//bprintf(0, _T("Wheel %.04X  Accel %.04X\n"), scale_wheel(DrvWheel), scale_accel(DrvAccel));

	if (port_fa_last == 0x08)  /* 0000 1000 */ /* Angle */
		temp = scale_wheel(DrvWheel);

	if (port_fa_last == 0x09)  /* 0000 1001 */ /* Accel */
		temp = scale_accel(DrvAccel);

	return temp;
}

static inline void __fastcall hangonjr_port_fa_write (UINT8 data)
{
	/* Seems to write the same pattern again and again bits ---- xx-x used */
	port_fa_last = data;
}

static void segae_bankswitch (void)
{
	//ZetMapMemory(DrvMainROM + 0x10000 + ( rombank * 0x4000), 0x8000, 0xbfff, MAP_ROM);
	CZetMapArea(0x8000, 0xbfff, 0, DrvMainROM + 0x10000 + ( rombank * 0x4000));
	CZetMapArea(0x8000, 0xbfff, 2, DrvMainROM + 0x10000 + ( rombank * 0x4000));
}

static void __fastcall bank_write(UINT8 data)
{
	segae_vdp_vrambank[0]	= (data & 0x80) >> 7; /* Back  Layer VDP (0) VRAM Bank */
	segae_vdp_vrambank[1]	= (data & 0x40) >> 6; /* Front Layer VDP (1) VRAM Bank */
	segae_8000bank			= (data & 0x20) >> 5; /* 0x8000 Write Select */
	rombank					=  data & 0x0f;		  /* Rom Banking */
	segae_bankswitch();
}

static void segae_vdp_setregister ( UINT8 chip, UINT16 cmd )
{
	UINT8 regnumber;
	UINT8 regdata;

	regnumber = (cmd & 0x0f00) >> 8;
	regdata   = (cmd & 0x00ff);
	if (regnumber < 11) {
		segae_vdp_regs[chip][regnumber] = regdata;
	} else {
		/* Illegal, there aren't this many registers! */
	}
}

static void segae_vdp_processcmd ( UINT8 chip, UINT16 cmd )
{
	if ( (cmd & 0xf000) == 0x8000 ) { /*  1 0 0 0 - - - - - - - - - - - -  VDP Register Set */
		segae_vdp_setregister (chip, cmd);
	} else { /* Anything Else */
		segae_vdp_accessmode[chip] = (cmd & 0xc000) >> 14;
		segae_vdp_accessaddr[chip] = (cmd & 0x3fff);

		if ((segae_vdp_accessmode[chip]==0x03) && (segae_vdp_accessaddr[chip] > 0x1f) ) { /* Check Address is valid for CRAM */
			/* Illegal, CRAM isn't this large! */
			segae_vdp_accessaddr[chip] &= 0x1f;
		}

		if (segae_vdp_accessmode[chip] == 0x00) { /*  0 0 - - - - - - - - - - - - - -  VRAM Acess Mode (Special Read) */
			segae_vdp_readbuffer[chip] = segae_vdp_vram[chip][ segae_vdp_vrambank[chip]*0x4000 + segae_vdp_accessaddr[chip] ];
			segae_vdp_accessaddr[chip] += 1;
			segae_vdp_accessaddr[chip] &= 0x3fff;
		}
	}
}

static UINT8 segae_vdp_counter_r (UINT8 chip, UINT8 offset)
{
	UINT8 temp = 0;
	UINT16 sline;

	switch (offset)
	{
		case 0: /* port 0x7e, Vert Position (in scanlines) */
			sline = currentLine;
//			if (sline > 0xDA) sline -= 6;
//			temp = sline-1 ;
			if (sline > 0xDA) sline -= 5;
			temp = sline ;
			break;
		case 1: /* port 0x7f, Horz Position (in pixel clock cycles)  */
			/* unhandled for now */
			break;
	}
	return temp;
}

static UINT8 segae_vdp_data_r(UINT8 chip)
{
	UINT8 temp;

	segae_vdp_cmdpart[chip] = 0;

	temp = segae_vdp_readbuffer[chip];

	if (segae_vdp_accessmode[chip]==0x03) { /* CRAM Access */
		/* error CRAM can't be read!! */
	} else { /* VRAM */
		segae_vdp_readbuffer[chip] = segae_vdp_vram[chip][ segae_vdp_vrambank[chip]*0x4000 + segae_vdp_accessaddr[chip] ];
		segae_vdp_accessaddr[chip] += 1;
		segae_vdp_accessaddr[chip] &= 0x3fff;
	}
	return temp;
}

static UINT8 segae_vdp_reg_r ( UINT8 chip )
{
	UINT8 temp;

	temp = 0;

	temp |= (vintpending << 7);
	temp |= (hintpending << 6);

	hintpending = vintpending = 0;

	return temp;
}

static inline UINT8 pal2bit(UINT8 bits)
{
	bits &= 3;
	return (bits << 6) | (bits << 4) | (bits << 2) | bits;
}

static void segae_vdp_data_w ( UINT8 chip, UINT8 data )
{
	segae_vdp_cmdpart[chip] = 0;

	if (segae_vdp_accessmode[chip]==0x03) { /* CRAM Access */
		UINT8 r,g,b, temp;

		temp = segae_vdp_cram[chip][segae_vdp_accessaddr[chip]];

		segae_vdp_cram[chip][segae_vdp_accessaddr[chip]] = data;

		if (temp != data) 
		{

			r = (segae_vdp_cram[chip][segae_vdp_accessaddr[chip]] & 0x03) >> 0;
			g = (segae_vdp_cram[chip][segae_vdp_accessaddr[chip]] & 0x0c) >> 2;
			b = (segae_vdp_cram[chip][segae_vdp_accessaddr[chip]] & 0x30) >> 4;

			Palette[segae_vdp_accessaddr[chip] + 32*chip] = pal2bit(r) << 16 | pal2bit(g) << 8 | pal2bit(b);  //BurnHighCol(pal2bit(r), pal2bit(g), pal2bit(b), 0);
			DrvPalette[segae_vdp_accessaddr[chip] + 32*chip] = BurnHighCol(pal2bit(r), pal2bit(g), pal2bit(b), 0);
		}

		segae_vdp_accessaddr[chip] += 1;
		segae_vdp_accessaddr[chip] &= 0x1f;
	} else { /* VRAM Accesses */
		segae_vdp_vram[chip][ segae_vdp_vrambank[chip]*0x4000 + segae_vdp_accessaddr[chip] ] = data;
		segae_vdp_accessaddr[chip] += 1;
		segae_vdp_accessaddr[chip] &= 0x3fff;
	}
}

static void segae_vdp_reg_w ( UINT8 chip, UINT8 data )
{
	if (!segae_vdp_cmdpart[chip]) {
		segae_vdp_cmdpart[chip] = 1;
		segae_vdp_command[chip] = data;
	} else {
		segae_vdp_cmdpart[chip] = 0;
		segae_vdp_command[chip] |= (data << 8);
		segae_vdp_processcmd (chip, segae_vdp_command[chip]);
	}
}

/*static UINT8 input_r(INT32 offset)
{
	//bprintf(0, _T("input_r chip %X.\n"), offset);
	return 0xff;
}*/

static UINT8 __fastcall systeme_main_in(UINT16 port)
{
	port &= 0xff;

	switch (port) {
		case 0x7e: return segae_vdp_counter_r(0, 0);
		case 0x7f: return segae_vdp_counter_r(0, 1);
		case 0xba: return segae_vdp_data_r(0);
		case 0xbb: return segae_vdp_reg_r(0);

		case 0xbe: return segae_vdp_data_r(1);
		case 0xbf: return segae_vdp_reg_r(1);

		case 0xe0: return 0xff - DrvInput[0];
		case 0xe1: return 0xff - DrvInput[1];
//		case 0xe2: return input_r(port); // AM_RANGE(0xe2, 0xe2) AM_READ_PORT( "e2" )
		case 0xf2: return DrvDip[0];//input_r(port); // AM_RANGE(0xf2, 0xf2) AM_READ_PORT( "f2" ) /* DSW0 */
		case 0xf3: return DrvDip[1];//input_r(port); // AM_RANGE(0xf3, 0xf3) AM_READ_PORT( "f3" ) /* DSW1 */
		case 0xf8: return hangonjr_port_f8_read(port); // m_maincpu->space(AS_IO).install_read_handler(0xf8, 0xf8, read8_delegate(FUNC(systeme_state::hangonjr_port_f8_read), this));
	}	
	////bprintf(PRINT_NORMAL, _T("IO Read %x\n"), a);
	return 0;
}

static void __fastcall systeme_main_out(UINT16 port, UINT8 data)
{
	switch (port & 0xff)
	{
		case 0x7b: SN76496Write(0, data);
		break;

		case 0x7f: SN76496Write(1, data);
		break;

		case 0xba: segae_vdp_data_w(0, data);
		break;

		case 0xbb: segae_vdp_reg_w(0, data);
		break;

		case 0xbe: segae_vdp_data_w(1, data);
		break;

		case 0xbf:	segae_vdp_reg_w(1, data);
		break;

		case 0xf7:	bank_write(data);
		break;

		case 0xfa:	hangonjr_port_fa_write(data);
		break;
	}
}

static INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;

	DrvMainROM	 	    = Next; Next += 0x40000;
	
	AllRam				= Next;
	DrvRAM			    = Next; Next += 0x10000;

	segae_vdp_vram[0]	= Next; Next += 0x8000; /* 32kb (2 banks) */
	segae_vdp_vram[1]	= Next; Next += 0x8000; /* 32kb (2 banks) */

	segae_vdp_cram[0]	= Next; Next += 0x20;
	segae_vdp_regs[0]	= Next; Next += 0x20;

	segae_vdp_cram[1]	= Next; Next += 0x20;
	segae_vdp_regs[1]	= Next; Next += 0x20;

	cache_bitmap		= Next; Next += ( (16+256+16) * 192+17 ) + 0x0f; /* 16 pixels either side to simplify drawing + 0xf for alignment */

	DrvPalette			= (UINT32*)Next; Next += 0x040 * sizeof(UINT32);
	Palette			    = (UINT32*)Next; Next += 0x040 * sizeof(UINT32);

	RamEnd			= Next;
	MemEnd			= Next;

	return 0;
}

//-----------------------
static INT32 DrvExit()
{
	CZetExit();
//	SN76496Exit();
	BurnFree(AllMem);

	leftcolumnblank = 0;

	return 0;
}

static INT32 DrvDoReset()
{
	memset (DrvRAM, 0, RamEnd - DrvRAM);

	hintcount = 0;
	vintpending = 0;
	hintpending = 0;
//	SN76496Reset();
	CZetOpen(0);
	segae_bankswitch();
	CZetReset();
	CZetClose();
	
	return 0;
}

static inline void DrvClearOpposites(UINT8* nJoystickInputs)
{
	if ((*nJoystickInputs & 0x03) == 0x03) {
		*nJoystickInputs &= ~0x03;
	}
	if ((*nJoystickInputs & 0x0c) == 0x0c) {
		*nJoystickInputs &= ~0x0c;
	}
}

static inline void DrvMakeInputs()
{
	// Reset Inputs
	DrvInput[0] = DrvInput[1] = DrvInput[2] = DrvInput[3] = DrvInput[4] = 0x00;

	// Compile Digital Inputs
	for (INT32 i = 0; i < 8; i++) {
		DrvInput[0] |= (DrvJoy0[i] & 1) << i;
		DrvInput[1] |= (DrvJoy1[i] & 1) << i;
		DrvInput[2] |= (DrvJoy2[i] & 1) << i;
		DrvInput[3] |= (DrvJoy3[i] & 1) << i;
		DrvInput[4] |= (DrvJoy4[i] & 1) << i;
	}

	// Clear Opposites
	DrvClearOpposites(&DrvInput[0]);
	DrvClearOpposites(&DrvInput[1]);
}

static void segae_draw8pix_solid16(UINT8 *dest, UINT8 chip, UINT16 tile, UINT8 line, UINT8 flipx, UINT8 col)
{

	UINT32 pix8 = *(UINT32 *)&segae_vdp_vram[chip][(32)*tile + (4)*line + (0x4000) * segae_vdp_vrambank[chip]];
	UINT8  pix, coladd;

	if (!pix8 && !col) return; /*note only the colour 0 of each vdp is transparent NOT colour 16???, fixes sky in HangonJr */

	coladd = 16*col;

	if (flipx)	{
		pix = ((pix8 >> 0) & 0x01) | ((pix8 >> 7) & 0x02) | ((pix8 >> 14) & 0x04) | ((pix8 >> 21) & 0x08) ; pix+= coladd ; if (pix) dest[0] = pix+ 32*chip;
		pix = ((pix8 >> 1) & 0x01) | ((pix8 >> 8) & 0x02) | ((pix8 >> 15) & 0x04) | ((pix8 >> 22) & 0x08) ; pix+= coladd ; if (pix) dest[1] = pix+ 32*chip;
		pix = ((pix8 >> 2) & 0x01) | ((pix8 >> 9) & 0x02) | ((pix8 >> 16) & 0x04) | ((pix8 >> 23) & 0x08) ; pix+= coladd ; if (pix) dest[2] = pix+ 32*chip;
		pix = ((pix8 >> 3) & 0x01) | ((pix8 >>10) & 0x02) | ((pix8 >> 17) & 0x04) | ((pix8 >> 24) & 0x08) ; pix+= coladd ; if (pix) dest[3] = pix+ 32*chip;
		pix = ((pix8 >> 4) & 0x01) | ((pix8 >>11) & 0x02) | ((pix8 >> 18) & 0x04) | ((pix8 >> 25) & 0x08) ; pix+= coladd ; if (pix) dest[4] = pix+ 32*chip;
		pix = ((pix8 >> 5) & 0x01) | ((pix8 >>12) & 0x02) | ((pix8 >> 19) & 0x04) | ((pix8 >> 26) & 0x08) ; pix+= coladd ; if (pix) dest[5] = pix+ 32*chip;
		pix = ((pix8 >> 6) & 0x01) | ((pix8 >>13) & 0x02) | ((pix8 >> 20) & 0x04) | ((pix8 >> 27) & 0x08) ; pix+= coladd ; if (pix) dest[6] = pix+ 32*chip;
		pix = ((pix8 >> 7) & 0x01) | ((pix8 >>14) & 0x02) | ((pix8 >> 21) & 0x04) | ((pix8 >> 28) & 0x08) ; pix+= coladd ; if (pix) dest[7] = pix+ 32*chip;
	} else {
		pix = ((pix8 >> 7) & 0x01) | ((pix8 >>14) & 0x02) | ((pix8 >> 21) & 0x04) | ((pix8 >> 28) & 0x08) ; pix+= coladd ; if (pix) dest[0] = pix+ 32*chip;
		pix = ((pix8 >> 6) & 0x01) | ((pix8 >>13) & 0x02) | ((pix8 >> 20) & 0x04) | ((pix8 >> 27) & 0x08) ; pix+= coladd ; if (pix) dest[1] = pix+ 32*chip;
		pix = ((pix8 >> 5) & 0x01) | ((pix8 >>12) & 0x02) | ((pix8 >> 19) & 0x04) | ((pix8 >> 26) & 0x08) ; pix+= coladd ; if (pix) dest[2] = pix+ 32*chip;
		pix = ((pix8 >> 4) & 0x01) | ((pix8 >>11) & 0x02) | ((pix8 >> 18) & 0x04) | ((pix8 >> 25) & 0x08) ; pix+= coladd ; if (pix) dest[3] = pix+ 32*chip;
		pix = ((pix8 >> 3) & 0x01) | ((pix8 >>10) & 0x02) | ((pix8 >> 17) & 0x04) | ((pix8 >> 24) & 0x08) ; pix+= coladd ; if (pix) dest[4] = pix+ 32*chip;
		pix = ((pix8 >> 2) & 0x01) | ((pix8 >> 9) & 0x02) | ((pix8 >> 16) & 0x04) | ((pix8 >> 23) & 0x08) ; pix+= coladd ; if (pix) dest[5] = pix+ 32*chip;
		pix = ((pix8 >> 1) & 0x01) | ((pix8 >> 8) & 0x02) | ((pix8 >> 15) & 0x04) | ((pix8 >> 22) & 0x08) ; pix+= coladd ; if (pix) dest[6] = pix+ 32*chip;
		pix = ((pix8 >> 0) & 0x01) | ((pix8 >> 7) & 0x02) | ((pix8 >> 14) & 0x04) | ((pix8 >> 21) & 0x08) ; pix+= coladd ; if (pix) dest[7] = pix+ 32*chip;
	}
}

static void segae_draw8pix(UINT8 *dest, UINT8 chip, UINT16 tile, UINT8 line, UINT8 flipx, UINT8 col)
{

	UINT32 pix8 = *(UINT32 *)&segae_vdp_vram[chip][(32)*tile + (4)*line + (0x4000) * segae_vdp_vrambank[chip]];
	UINT8  pix, coladd;

	if (!pix8) return;

	coladd = 16*col+32*chip;

	if (flipx)	{
		pix = ((pix8 >> 0) & 0x01) | ((pix8 >> 7) & 0x02) | ((pix8 >> 14) & 0x04) | ((pix8 >> 21) & 0x08) ; if (pix) dest[0] = pix+ coladd;
		pix = ((pix8 >> 1) & 0x01) | ((pix8 >> 8) & 0x02) | ((pix8 >> 15) & 0x04) | ((pix8 >> 22) & 0x08) ; if (pix) dest[1] = pix+ coladd;
		pix = ((pix8 >> 2) & 0x01) | ((pix8 >> 9) & 0x02) | ((pix8 >> 16) & 0x04) | ((pix8 >> 23) & 0x08) ; if (pix) dest[2] = pix+ coladd;
		pix = ((pix8 >> 3) & 0x01) | ((pix8 >>10) & 0x02) | ((pix8 >> 17) & 0x04) | ((pix8 >> 24) & 0x08) ; if (pix) dest[3] = pix+ coladd;
		pix = ((pix8 >> 4) & 0x01) | ((pix8 >>11) & 0x02) | ((pix8 >> 18) & 0x04) | ((pix8 >> 25) & 0x08) ; if (pix) dest[4] = pix+ coladd;
		pix = ((pix8 >> 5) & 0x01) | ((pix8 >>12) & 0x02) | ((pix8 >> 19) & 0x04) | ((pix8 >> 26) & 0x08) ; if (pix) dest[5] = pix+ coladd;
		pix = ((pix8 >> 6) & 0x01) | ((pix8 >>13) & 0x02) | ((pix8 >> 20) & 0x04) | ((pix8 >> 27) & 0x08) ; if (pix) dest[6] = pix+ coladd;
		pix = ((pix8 >> 7) & 0x01) | ((pix8 >>14) & 0x02) | ((pix8 >> 21) & 0x04) | ((pix8 >> 28) & 0x08) ; if (pix) dest[7] = pix+ coladd;
	} else {
		pix = ((pix8 >> 7) & 0x01) | ((pix8 >>14) & 0x02) | ((pix8 >> 21) & 0x04) | ((pix8 >> 28) & 0x08) ; if (pix) dest[0] = pix+ coladd;
		pix = ((pix8 >> 6) & 0x01) | ((pix8 >>13) & 0x02) | ((pix8 >> 20) & 0x04) | ((pix8 >> 27) & 0x08) ; if (pix) dest[1] = pix+ coladd;
		pix = ((pix8 >> 5) & 0x01) | ((pix8 >>12) & 0x02) | ((pix8 >> 19) & 0x04) | ((pix8 >> 26) & 0x08) ; if (pix) dest[2] = pix+ coladd;
		pix = ((pix8 >> 4) & 0x01) | ((pix8 >>11) & 0x02) | ((pix8 >> 18) & 0x04) | ((pix8 >> 25) & 0x08) ; if (pix) dest[3] = pix+ coladd;
		pix = ((pix8 >> 3) & 0x01) | ((pix8 >>10) & 0x02) | ((pix8 >> 17) & 0x04) | ((pix8 >> 24) & 0x08) ; if (pix) dest[4] = pix+ coladd;
		pix = ((pix8 >> 2) & 0x01) | ((pix8 >> 9) & 0x02) | ((pix8 >> 16) & 0x04) | ((pix8 >> 23) & 0x08) ; if (pix) dest[5] = pix+ coladd;
		pix = ((pix8 >> 1) & 0x01) | ((pix8 >> 8) & 0x02) | ((pix8 >> 15) & 0x04) | ((pix8 >> 22) & 0x08) ; if (pix) dest[6] = pix+ coladd;
		pix = ((pix8 >> 0) & 0x01) | ((pix8 >> 7) & 0x02) | ((pix8 >> 14) & 0x04) | ((pix8 >> 21) & 0x08) ; if (pix) dest[7] = pix+ coladd;
	}
}

static void segae_draw8pixsprite(UINT8 *dest, UINT8 chip, UINT16 tile, UINT8 line)
{

	UINT32 pix8 = *(UINT32 *)&segae_vdp_vram[chip][(32)*tile + (4)*line + (0x4000) * segae_vdp_vrambank[chip]];
	UINT8  pix;

	if (!pix8) return; /*note only the colour 0 of each vdp is transparent NOT colour 16, fixes sky in HangonJr */

	pix = ((pix8 >> 7) & 0x01) | ((pix8 >>14) & 0x02) | ((pix8 >> 21) & 0x04) | ((pix8 >> 28) & 0x08) ; if (pix) dest[0] = pix+16+32*chip;
	pix = ((pix8 >> 6) & 0x01) | ((pix8 >>13) & 0x02) | ((pix8 >> 20) & 0x04) | ((pix8 >> 27) & 0x08) ; if (pix) dest[1] = pix+16+32*chip;
	pix = ((pix8 >> 5) & 0x01) | ((pix8 >>12) & 0x02) | ((pix8 >> 19) & 0x04) | ((pix8 >> 26) & 0x08) ; if (pix) dest[2] = pix+16+32*chip;
	pix = ((pix8 >> 4) & 0x01) | ((pix8 >>11) & 0x02) | ((pix8 >> 18) & 0x04) | ((pix8 >> 25) & 0x08) ; if (pix) dest[3] = pix+16+32*chip;
	pix = ((pix8 >> 3) & 0x01) | ((pix8 >>10) & 0x02) | ((pix8 >> 17) & 0x04) | ((pix8 >> 24) & 0x08) ; if (pix) dest[4] = pix+16+32*chip;
	pix = ((pix8 >> 2) & 0x01) | ((pix8 >> 9) & 0x02) | ((pix8 >> 16) & 0x04) | ((pix8 >> 23) & 0x08) ; if (pix) dest[5] = pix+16+32*chip;
	pix = ((pix8 >> 1) & 0x01) | ((pix8 >> 8) & 0x02) | ((pix8 >> 15) & 0x04) | ((pix8 >> 22) & 0x08) ; if (pix) dest[6] = pix+16+32*chip;
	pix = ((pix8 >> 0) & 0x01) | ((pix8 >> 7) & 0x02) | ((pix8 >> 14) & 0x04) | ((pix8 >> 21) & 0x08) ; if (pix) dest[7] = pix+16+32*chip;

}

static void segae_drawspriteline(UINT8 *dest, UINT8 chip, UINT8 line)
{
	/* todo: figure out what riddle of pythagoras hates about this */

	int nosprites;
	int loopcount;

	UINT16 spritebase;

	nosprites = 0;

	spritebase =  (segae_vdp_regs[chip][5] & 0x7e) << 7;
	spritebase += (segae_vdp_vrambank[chip] * 0x4000);

	/*- find out how many sprites there are -*/

	for (loopcount=0;loopcount<64;loopcount++) {
		UINT8 ypos;

		ypos = segae_vdp_vram[chip][spritebase+loopcount];

		if (ypos==208) {
			nosprites=loopcount;
			break;
		}
	}

//	if (!strcmp(Machine->gamedrv->name,"ridleofp")) nosprites = 63; /* why, there must be a bug elsewhere i guess ?! */

	/*- draw sprites IN REVERSE ORDER -*/

	for (loopcount = nosprites; loopcount >= 0;loopcount--) {
		int ypos;
		UINT8 sheight;

		ypos = segae_vdp_vram[chip][spritebase+loopcount] +1;

		if (segae_vdp_regs[chip][1] & 0x02) sheight=16; else sheight=8;

		if ( (line >= ypos) && (line < ypos+sheight) ) {
			int xpos;
			UINT8 sprnum;
			UINT8 spline;

			spline = line - ypos;

			xpos   = segae_vdp_vram[chip][spritebase+0x80+ (2*loopcount)];
			sprnum = segae_vdp_vram[chip][spritebase+0x81+ (2*loopcount)];

			if (segae_vdp_regs[chip][6] & 0x04)
				sprnum += 0x100;

			segae_draw8pixsprite(dest+xpos, chip, sprnum, spline);

		}
	}
}

static void segae_drawtilesline(UINT8 *dest, int line, UINT8 chip, UINT8 pri)
{
	/* todo: fix vscrolling (or is it something else causing the glitch on the "game over" screen of hangonjr, seems to be ..  */

	UINT8 hscroll;
	UINT8 vscroll;
	UINT16 tmbase;
	UINT8 tilesline, tilesline2;
	UINT8 coloffset, coloffset2;
	UINT8 loopcount;

	hscroll = (256-segae_vdp_regs[chip][8]);
	vscroll = segae_vdp_regs[chip][9];
	if (vscroll > 224) vscroll %= 224;

	tmbase =  (segae_vdp_regs[chip][2] & 0x0e) << 10;
	tmbase += (segae_vdp_vrambank[chip] * 0x4000);

	tilesline = (line + vscroll) >> 3;
	tilesline2= (line + vscroll) % 8;


	coloffset = (hscroll >> 3);
	coloffset2= (hscroll % 8);

	dest -= coloffset2;

	for (loopcount=0;loopcount<33;loopcount++) {

		UINT16 vram_offset, vram_word;
		UINT16 tile_no;
		UINT8  palette, priority, flipx, flipy;

		vram_offset = tmbase
					+ (2 * (32*tilesline + ((coloffset+loopcount)&0x1f) ) );
		vram_word = segae_vdp_vram[chip][vram_offset] | (segae_vdp_vram[chip][vram_offset+1] << 8);

		tile_no = (vram_word & 0x01ff);
		flipx =   (vram_word & 0x0200) >> 9;
		flipy =   (vram_word & 0x0400) >> 10;
		palette = (vram_word & 0x0800) >> 11;
		priority= (vram_word & 0x1000) >> 12;

		tilesline2= (line + vscroll) % 8;
		if (flipy) tilesline2 = 7-tilesline2;

		if (priority == pri) {
			if (chip == 0) segae_draw8pix_solid16(dest, chip, tile_no,tilesline2,flipx,palette);
			else segae_draw8pix(dest, chip, tile_no,tilesline2,flipx,palette);
		}
		dest+=8;
	}
}

static void segae_drawscanline(int line)
{
	UINT8* dest;

	dest = cache_bitmap + (16+256+16) * line;

	/* This should be cleared to bg colour, but which vdp determines that !, neither seems to be right, maybe its always the same col? */
	memset(dest, 0, 16+256+16);

	if (segae_vdp_regs[0][1] & 0x40) {
		segae_drawtilesline (dest+16, line, 0,0);
		segae_drawspriteline(dest+16, 0, line);
		segae_drawtilesline (dest+16, line, 0,1);
	}

	{
		if (segae_vdp_regs[1][1] & 0x40) {
			segae_drawtilesline (dest+16, line, 1,0);
			segae_drawspriteline(dest+16, 1, line);
			segae_drawtilesline (dest+16, line, 1,1);
		}
	}

	if (leftcolumnblank) memset(dest+16, 32+16, 8); /* Clear Leftmost column, there should be a register for this like on the SMS i imagine    */
							   			  /* on the SMS this is bit 5 of register 0 (according to CMD's SMS docs) for system E this  */
							   			  /* appears to be incorrect, most games need it blanked 99% of the time so we blank it      */

}

static INT32 DrvDraw()
{
	if (DrvRecalc) {
		for (INT32 i = 0; i < 0x40; i++) {
			DrvPalette[i] = BurnHighCol((Palette[i] >> 16) & 0xff, (Palette[i] >> 8) & 0xff, Palette[i] & 0xff, 0);
		}
		DrvRecalc = 0;
	}

/*	BurnTransferCopy(DrvPalette);

	UINT16 *pDst = pTransDraw;
	UINT8 *pSrc = &cache_bitmap[16];

	for (INT32 y = 0; y < nScreenHeight; y++)
	{
		for (INT32 x = 0; x < nScreenWidth; x++)
		{
			pDst[x] = pSrc[x];
		}
		pDst += nScreenWidth;
		pSrc += 288;
	}
*/
	return 0;
}

static void segae_interrupt ()
{
	if (currentLine == 0) {
		hintcount = segae_vdp_regs[1][10];
	}

	if (currentLine <= 192) {

		if (currentLine != 192) segae_drawscanline(currentLine);

		if (currentLine == 192)
			vintpending = 1;

		if (hintcount == 0) {
			hintcount = segae_vdp_regs[1][10];
			hintpending = 1;

			if  ((segae_vdp_regs[1][0] & 0x10)) {
				CZetSetIRQLine(0, CZET_IRQSTATUS_ACK);
				return;
			}

		} else {
			hintcount--;
		}
	}

	if (currentLine > 192) {
		hintcount = segae_vdp_regs[1][10];

		if ( (currentLine<0xe0) && (vintpending) ) {
			CZetSetIRQLine(0, CZET_IRQSTATUS_ACK);
		}
	}
}


static INT32 DrvFrame()
{
	INT32 nInterleave = 262;

	if (DrvReset) DrvDoReset();

	DrvMakeInputs();

	nCyclesTotal = 10738635 / 2 / 60;
	nCyclesDone = 0;
	currentLine = 0;

	CZetNewFrame();

	for (INT32 i = 0; i < nInterleave; i++) {
		INT32 nNext;

		// Run Z80 #1
		CZetOpen(0);
		nNext = (i + 1) * nCyclesTotal / nInterleave;
		nCyclesSegment = nNext - nCyclesDone;
		nCyclesDone += CZetRun(nCyclesSegment);
		currentLine = (i - 4) & 0xff;

		segae_interrupt();
		CZetClose();
	}

	signed short *nSoundBuffer = (signed short *)(0x25a20000+nSoundBufferPos*(sizeof(signed short)));
//	if (pBurnSoundOut)
	{
		SN76496Update(0, nSoundBuffer, nBurnSoundLen);
		SN76496Update(1, nSoundBuffer, nBurnSoundLen);
	}	

//	if (pBurnDraw) 
//	DrvDraw();

	nSoundBufferPos+=nBurnSoundLen; // DOIT etre deux fois la taille copiee

	if(nSoundBufferPos>=0x3C00)//RING_BUF_SIZE)
//	if(nSoundBufferPos>=(2048L*10))
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos);
		PCM_Task(pcm); // bon emplacement
		nSoundBufferPos=0;
	}

	return 0;
}

static INT32 DrvInit(UINT8 game)
{
	DrvInitSaturn();

	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)BurnMalloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();

	if (BurnLoadRom(DrvMainROM + 0x00000,  0, 1)) return 1;	// ( "rom5.ic7",   0x00000, 0x08000, CRC(d63925a7) SHA1(699f222d9712fa42651c753fe75d7b60e016d3ad) ) /* Fixed Code */
	if (BurnLoadRom(DrvMainROM + 0x10000,  1, 1)) return 1;	// ( "rom4.ic5",   0x10000, 0x08000, CRC(ee3caab3) SHA1(f583cf92c579d1ca235e8b300e256ba58a04dc90) )
	if (BurnLoadRom(DrvMainROM + 0x18000,  2, 1)) return 1;	// ( "rom3.ic4",   0x18000, 0x08000, CRC(d2ba9bc9) SHA1(85cf2a801883bf69f78134fc4d5075134f47dc03) )

	if(game)
	{
		if (BurnLoadRom(DrvMainROM + 0x20000,  3, 1)) return 1;	// ( "rom2.ic3",   0x20000, 0x08000, CRC(e14da070) SHA1(f8781f65be5246a23c1f492905409775bbf82ea8) )
		if (BurnLoadRom(DrvMainROM + 0x28000,  4, 1)) return 1; // ( "rom1.ic2",   0x28000, 0x08000, CRC(3810cbf5) SHA1(c8d5032522c0c903ab3d138f62406a66e14a5c69) )
	}

	CZetInit(1);
	CZetOpen(0);
//	CZetMapMemory(DrvMainROM, 0x0000, 0x7fff, MAP_ROM);
	CZetMapArea(0x0000, 0x7fff, 0, DrvMainROM + 0x0000);
	CZetMapArea(0x0000, 0x7fff, 2, DrvMainROM + 0x0000);

//	CZetMapMemory(DrvRAM, 0xc000, 0xffff, MAP_RAM);
	CZetMapArea(0xc000, 0xffff, 0, DrvRAM);
	CZetMapArea(0xc000, 0xffff, 1, DrvRAM);
	CZetMapArea(0xc000, 0xffff, 2, DrvRAM);

	CZetSetWriteHandler(systeme_main_write);
	CZetSetReadHandler(systeme_main_read);
	CZetSetInHandler(systeme_main_in);
	CZetSetOutHandler(systeme_main_out);

	CZetClose();

	SN76489Init(0, 10738635 / 3, 0);
	SN76489Init(1, 10738635 / 3, 1);

//	SN76496SetRoute(0, 0.50, BURN_SND_ROUTE_BOTH);
//	SN76496SetRoute(1, 0.50, BURN_SND_ROUTE_BOTH);

//	GenericTilesInit();

	DrvDoReset();

	return 0;
}

static INT32 DrvTransfrmInit()
{
	leftcolumnblank = 1;

	return DrvInit(2);
}

static INT32 DrvHangonJrInit()
{
	leftcolumnblank = 1;

	return DrvInit(1);
}

static INT32 DrvTetrisInit()
{
	return DrvInit(0);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr		= (Uint16*)SCL_AllocColRam(SCL_NBG0,OFF);
	colAddr			= (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	(Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
//	SCL_SetColRam(SCL_NBG1,0,8,palette);
	SCL_SetColRam(SCL_NBG1,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initLayers(void)
{
//    SclConfig	config;
// **29/01/2007 : VBT sauvegarde cycle patter qui fonctionne jusqu'à maintenant

	Uint16	CycleTb[]={
		  // VBT 04/02/2007 : cycle pattern qui fonctionne just test avec des ee
//#ifdef GG
#if 0
		0xffff, 0xff5e, //A1
		0xffff, 0xffff,	//A0
		0x0044, 0xeeff,   //B1
		0xffff, 0xffff  //B0
#else
		0xff5e, 0xffff, //A1
		0xffff, 0xffff,	//A0
		0x04ee, 0xffff,   //B1
		0xffff, 0xffff  //B0
#endif
	};
 	SclConfig	scfg;

//	SCL_InitConfigTb(&scfg);
	scfg.dispenbl		= ON;
	scfg.charsize		= SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
#ifdef TWO_WORDS
	scfg.pnamesize	= SCL_PN2WORD;
#else
	scfg.pnamesize	= SCL_PN1WORD;
	scfg.flip				= SCL_PN_10BIT;
#endif
	scfg.platesize	= SCL_PL_SIZE_1X1;
//#ifdef GG
#if 0
	scfg.coltype		= SCL_COL_TYPE_256;
#else
	scfg.coltype		= SCL_COL_TYPE_16;
#endif
	scfg.datatype	= SCL_CELL;
// vbt : active le prioritybit en mode 1word
//	scfg.patnamecontrl =  0x020c;// VRAM B1 ‚ÌƒIƒtƒZƒbƒg 
	scfg.patnamecontrl =  0x000c;// VRAM B1 ‚ÌƒIƒtƒZƒbƒg 
	scfg.plate_addr[0] = ss_map;
	SCL_SetConfig(SCL_NBG0, &scfg);
/********************************************/	

//	SCL_InitConfigTb(&scfg);
//	scfg.dispenbl 	 = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
#ifdef GG
	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
#endif
	scfg.datatype 	 = SCL_BITMAP;
	scfg.mapover       = SCL_OVER_0;
	scfg.plate_addr[0] = ss_font;
	SCL_SetConfig(SCL_NBG1, &scfg);
	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initPosition(void)
{
	SCL_Open();
	ss_reg->n1_move_x = 0;
	ss_reg->n1_move_y = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DrvInitSaturn()
{
	SPR_InitSlaveSH();

	nBurnSprites  = 67;//131;//27;
	nBurnLinescrollSize = 0x340;
	nSoundBufferPos = 0;//sound position à renommer

	SS_CACHE = cache      =(Uint8  *)SCL_VDP2_VRAM_B1;
	SS_MAP     = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_FONT   = ss_font    =(Uint16 *)SCL_VDP2_VRAM_A1;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;
#ifdef TWO_WORDS
	ss_OtherPri       = (SclOtherPriRegister *)SS_OTHR;
#endif
	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_scl			= (Fixed32 *)SS_SCL;

#ifdef TWO_WORDS
    SS_SET_N0PRIN(4);
    SS_SET_S0PRIN(4);	
	SS_SET_N0SPRM(1);  // 1 for special priority	  // mode 2 pixel, mode 1 pattern, mode 0 by screen
	ss_regs->specialcode=0x000e; // sfcode, upper 8bits, function b, lower 8bits function a
	ss_regs->specialcode_sel=0; // sfsel, bit 0 for nbg0 // 1 sfcs, bit 0 = 1 for funcion code b, 0 for function code a
	SclProcess = 2; 
#else
    SS_SET_N0PRIN(5);
    SS_SET_S0PRIN(6);
#endif
	SS_SET_N1PRIN(7);

	initLayers();
	initColors();
	initPosition();
		FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)" ",0,180);	

//	initSprites(256+48-1,192+16-1,256-1,192-1,48,16);
	initSprites(256-1,192-1,0,0,0,0);
	 initScrolling(ON,SCL_VDP2_VRAM_B0+0x4000);
		FNT_Print256_2bpp((volatile Uint8 *)ss_font,(Uint8 *)" ",0,180);	

	drawWindow(0,192,192,2,66);
}
