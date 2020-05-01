#define nInterleave 10
void dummy();

/*static */inline void System1ClearOpposites(UINT8* nJoystickInputs)
{
	if ((*nJoystickInputs & 0x30) == 0x30) {
		*nJoystickInputs &= ~0x30;
	}
	if ((*nJoystickInputs & 0xc0) == 0xc0) {
		*nJoystickInputs &= ~0xc0;
	}
}

/*static*/ void System1MakeInputs()
{
	// Reset Inputs
	System1Input[0] = System1Input[1] = System1Input[2] = 0x00;
	unsigned int i;

	// Compile Digital Inputs
	for (i = 0; i < 8; i++) {
		System1Input[0] |= (System1InputPort0[i] & 1) << i;
		System1Input[1] |= (System1InputPort1[i] & 1) << i;
		System1Input[2] |= (System1InputPort2[i] & 1) << i;
	}

	// Clear Opposites
	System1ClearOpposites(&System1Input[0]);
	System1ClearOpposites(&System1Input[1]);
}

/*==============================================================================================
Decode Functions
===============================================================================================*/

/*static*/ void sega_decode(const UINT8 convtable[32][4])
{
	int A;

	int length = 0x10000;
	int cryptlen = 0x8000;
	UINT8 *rom = System1Rom1;
	UINT8 *decrypted = System1Fetch1;
	
	for (A = 0x0000;A < cryptlen;A++)
	{
		int xorval = 0;

		UINT8 src = rom[A];

		// pick the translation table from bits 0, 4, 8 and 12 of the address 
		int row = (A & 1) + (((A >> 4) & 1) << 1) + (((A >> 8) & 1) << 2) + (((A >> 12) & 1) << 3);

		// pick the offset in the table from bits 3 and 5 of the source data 
		int col = ((src >> 3) & 1) + (((src >> 5) & 1) << 1);
		// the bottom half of the translation table is the mirror image of the top 
		if (src & 0x80)
		{
			col = 3 - col;
			xorval = 0xa8;
		}

		// decode the opcodes 
		decrypted[A] = (src & ~0xa8) | (convtable[2*row][col] ^ xorval);

		// decode the data 
		rom[A] = (src & ~0xa8) | (convtable[2*row+1][col] ^ xorval);

		if (convtable[2*row][col] == 0xff)	// table incomplete! (for development) 
			decrypted[A] = 0xee;
		if (convtable[2*row+1][col] == 0xff)	// table incomplete! (for development) 
			rom[A] = 0xee;
	}

	// this is a kludge to catch anyone who has code that crosses the encrypted/ 
	// decrypted boundary. ssanchan does it 
	if (length > 0x8000)
	{
		int bytes = 0x4000;
		memcpyl(&decrypted[0x8000], &rom[0x8000], bytes);
	}
}

/*static*/ void sega_decode_2(UINT8 *pDest, UINT8 *pDestDec, const UINT8 opcode_xor[64],const INT32 opcode_swap_select[64],
		const UINT8 data_xor[64],const INT32 data_swap_select[64])
{
	INT32 A;
	/*static*/ const UINT8 swaptable[24][4] =
	{
		{ 6,4,2,0 }, { 4,6,2,0 }, { 2,4,6,0 }, { 0,4,2,6 },
		{ 6,2,4,0 }, { 6,0,2,4 }, { 6,4,0,2 }, { 2,6,4,0 },
		{ 4,2,6,0 }, { 4,6,0,2 }, { 6,0,4,2 }, { 0,6,4,2 },
		{ 4,0,6,2 }, { 0,4,6,2 }, { 6,2,0,4 }, { 2,6,0,4 },
		{ 0,6,2,4 }, { 2,0,6,4 }, { 0,2,6,4 }, { 4,2,0,6 },
		{ 2,4,0,6 }, { 4,0,2,6 }, { 2,0,4,6 }, { 0,2,4,6 },
	};


	UINT8 *rom = pDest;
	UINT8 *decrypted = pDestDec;

	for (A = 0x0000;A < 0x8000;A++)
	{
		INT32 row;
		UINT8 src;
		const UINT8 *tbl;


		src = rom[A];

		/* pick the translation table from bits 0, 3, 6, 9, 12 and 14 of the address */
		row = (A & 1) + (((A >> 3) & 1) << 1) + (((A >> 6) & 1) << 2)
				+ (((A >> 9) & 1) << 3) + (((A >> 12) & 1) << 4) + (((A >> 14) & 1) << 5);

		/* decode the opcodes */
		tbl = swaptable[opcode_swap_select[row]];
		decrypted[A] = BITSWAP08(src,7,tbl[0],5,tbl[1],3,tbl[2],1,tbl[3]) ^ opcode_xor[row];

		/* decode the data */
		tbl = swaptable[data_swap_select[row]];
		rom[A] = BITSWAP08(src,7,tbl[0],5,tbl[1],3,tbl[2],1,tbl[3]) ^ data_xor[row];
	}

//	memcpyl(System1Fetch1 + 0x8000, System1Rom1 + 0x8000, 0x4000);	
	memcpy(pDestDec + 0x8000, pDest + 0x8000, 0x4000);
}

/*static*/ void sega_decode_317(UINT8 *pDest, UINT8 *pDestDec, INT32 order, INT32 opcode_shift, INT32 data_shift)
{
	/*static*/ const UINT8 xor1_317[1+64] =
	{
		0x54,
		0x14,0x15,0x41,0x14,0x50,0x55,0x05,0x41,0x01,0x10,0x51,0x05,0x11,0x05,0x14,0x55,
		0x41,0x05,0x04,0x41,0x14,0x10,0x45,0x50,0x00,0x45,0x00,0x00,0x00,0x45,0x00,0x00,
		0x54,0x04,0x15,0x10,0x04,0x05,0x11,0x44,0x04,0x01,0x05,0x00,0x44,0x15,0x40,0x45,
		0x10,0x15,0x51,0x50,0x00,0x15,0x51,0x44,0x15,0x04,0x44,0x44,0x50,0x10,0x04,0x04,
	};

	/*static*/ const UINT8 xor2_317[2+64] =
	{
		0x04,
		0x44,
		0x15,0x51,0x41,0x10,0x15,0x54,0x04,0x51,0x05,0x55,0x05,0x54,0x45,0x04,0x10,0x01,
		0x51,0x55,0x45,0x55,0x45,0x04,0x55,0x40,0x11,0x15,0x01,0x40,0x01,0x11,0x45,0x44,
		0x40,0x05,0x15,0x15,0x01,0x50,0x00,0x44,0x04,0x50,0x51,0x45,0x50,0x54,0x41,0x40,
		0x14,0x40,0x50,0x45,0x10,0x05,0x50,0x01,0x40,0x01,0x50,0x50,0x50,0x44,0x40,0x10,
	};

	/*static*/ const INT32 swap1_317[1+64] =
	{
		 7,
		 1,11,23,17,23, 0,15,19,
		20,12,10, 0,18,18, 5,20,
		13, 0,18,14, 5, 6,10,21,
		 1,11, 9, 3,21, 4, 1,17,
		 5, 7,16,13,19,23,20, 2,
		10,23,23,15,10,12, 0,22,
		14, 6,15,11,17,15,21, 0,
		 6, 1, 1,18, 5,15,15,20,
	};

	/*static*/ const INT32 swap2_317[2+64] =
	{
		 7,
		12,
		18, 8,21, 0,22,21,13,21,
		20,13,20,14, 6, 3, 5,20,
		 8,20, 4, 8,17,22, 0, 0,
		 6,17,17, 9, 0,16,13,21,
		 3, 2,18, 6,11, 3, 3,18,
		18,19, 3, 0, 5, 0,11, 8,
		 8, 1, 7, 2,10, 8,10, 2,
		 1, 3,12,16, 0,17,10, 1,
	};

	if (order)
		sega_decode_2( pDest, pDestDec, xor2_317+opcode_shift, swap2_317+opcode_shift, xor1_317+data_shift, swap1_317+data_shift );
	else
		sega_decode_2( pDest, pDestDec, xor1_317+opcode_shift, swap1_317+opcode_shift, xor2_317+data_shift, swap2_317+data_shift );
}

/*static*/ void fdwarrio_decode(void)
{
	/*static*/ const UINT8 opcode_xor[64] =
	{
		0x40,0x50,0x44,0x54,0x41,0x51,0x45,0x55,
		0x40,0x50,0x44,0x54,0x41,0x51,0x45,0x55,
		0x40,0x50,0x44,0x54,0x41,0x51,0x45,0x55,
		0x40,0x50,0x44,0x54,0x41,0x51,0x45,0x55,
		0x40,0x50,0x44,0x54,0x41,0x51,0x45,0x55,
		0x40,0x50,0x44,0x54,0x41,0x51,0x45,0x55,
		0x40,0x50,0x44,0x54,0x41,0x51,0x45,0x55,
		0x40,0x50,0x44,0x54,0x41,0x51,0x45,0x55,
	};

	/*static*/ const UINT8 data_xor[64] =
	{
		0x10,0x04,0x14,0x01,0x11,0x05,0x15,0x00,
		0x10,0x04,0x14,0x01,0x11,0x05,0x15,0x00,
		0x10,0x04,0x14,0x01,0x11,0x05,0x15,0x00,
		0x10,0x04,0x14,0x01,0x11,0x05,0x15,0x00,
		0x10,0x04,0x14,0x01,0x11,0x05,0x15,0x00,
		0x10,0x04,0x14,0x01,0x11,0x05,0x15,0x00,
		0x10,0x04,0x14,0x01,0x11,0x05,0x15,0x00,
		0x10,0x04,0x14,0x01,0x11,0x05,0x15,0x00,
	};

	/*static*/ const INT32 opcode_swap_select[64] =
	{
		4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,
		6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,
		8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,
		10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,
	};

	/*static*/ const INT32 data_swap_select[64] =
	{
		  4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,
		6,6,6,6,6,6,6,6,7,7,7,7,7,7,7,7,
		8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9,
		10,10,10,10,10,10,10,10,11,11,11,11,11,11,11,11,
		12,
	};

	sega_decode_2(System1Rom1, System1Fetch1, opcode_xor,opcode_swap_select,data_xor,data_swap_select);
}

/*==============================================================================================
Allocate Memory
===============================================================================================*/

/*static*/ int MemIndex()
{
	UINT8 *Next; Next = Mem;

	System1Rom1            = Next; Next += 0x040000;
	System1Fetch1          = Next; Next += 0x010000;
	System1Rom2            = Next; Next += 0x008000;
	System1PromRed         = Next; Next += 0x000100;
	System1PromGreen       = Next; Next += 0x000100;
	System1PromBlue        = Next; Next += 0x000100;

//	System1Ram1            = Next; Next += 0x0020fd;
	System1Ram1            = Next; Next += 0x002100;
	System1Ram2            = Next; Next += 0x000800;
	System1SpriteRam       = Next; Next += 0x000200;
	System1PaletteRam      = Next; Next += 0x000600;
	System1BgRam           = Next; Next += 0x000800;
//	System1VideoRam        = Next; Next += 0x000700;
	System1VideoRam        = Next; Next += 0x004000;
	RamStart1 = RamStart	= System1VideoRam-0xe800;
	System1ScrollXRam	   = System1VideoRam + 0x7C0;
	System1BgCollisionRam  = Next; Next += 0x000400;
	System1SprCollisionRam = Next; Next += 0x000400;
	System1deRam           = Next; Next += 0x000200;
	System1efRam           = Next; Next += 0x000100;
	System1ScrollY          = System1efRam + 0xbd;
	System1ScrollX           = System1efRam + 0xfc;

	System1f4Ram           = Next; Next += 0x000400;
	System1fcRam           = Next; Next += 0x000400; // +3 pour avoir un code aligné pour SpriteOnScreenMap
	SpriteOnScreenMap      = Next; Next += 0x10000;
	System1Sprites         = Next; Next += System1SpriteRomSize;

	width_lut			= Next; Next += 256 * sizeof(UINT8);
	cram_lut			= (UINT16 *)Next; Next += 256 * sizeof(UINT16);
	remap8to16_lut		= (UINT16 *)Next; Next += 512 * sizeof(UINT16);
	map_offset_lut		= (UINT16 *)Next; Next += 0x800 * sizeof(UINT16);
//	code_lut			= Next; Next += System1NumTiles * sizeof(UINT16);
	cpu_lut				= (UINT32 *)Next; Next += 10*sizeof(UINT32);
//	color_lut			= Next; Next += 0x2000 * sizeof(UINT8);
	map_cache			= (UINT16 *)Next; Next += (0x800*16) * sizeof(UINT32);
	map_dirty			= Next; Next += 0x0008;
	CZ80Context			= Next; Next += 2*sizeof(cz80_struc);

	return 0;
}
/*==============================================================================================
Reset Functions
===============================================================================================*/

/*static*/ int System1DoReset()
{
	CZetOpen(0);
	CZetReset();
	CZetClose();
	z80_reset();

	System1ScrollX[0] = System1ScrollX[1] = System1ScrollY[0] = 0;
	System1BgScrollX = 0;
	System1BgScrollY = 0;
	System1VideoMode = 0;
	System1FlipScreen = 0;
	System1SoundLatch = 0;
	System1BgBankLatch =  0;
	System1BgBank = 0;
	System1BankedRom = 0;
	System1BankSwitch = 0;
	memset(map_dirty,1,8);
	return 0;
}

/*==============================================================================================
Memory Handlers
===============================================================================================*/

void System1BankRom(UINT32 System1RomBank)
{
	UINT32 BankAddress = (System1RomBank * 0x4000) + 0x10000;
	CZetMapArea(0x8000, 0xbfff, 0, System1Rom1 + BankAddress);
//	CZetMapArea2(0x8000, 0xbfff, 2, System1Fetch1 + BankAddress, System1Rom1 + BankAddress);
	CZetMapArea2(0x8000, 0xbfff, 2, System1Rom1 + BankAddress + 0x20000, System1Rom1 + BankAddress);
}

UINT8 __fastcall System1Z801PortRead(unsigned short a)
{
	a &= 0xff;
	switch (a) 
	{
		case 0x00: return 0xff - System1Input[0];
		case 0x04: return 0xff - System1Input[1];
		case 0x08: return 0xff - System1Input[2];
		case 0x0c: return System1Dip[0];
		case 0x0d: return System1Dip[1];
		case 0x10: return System1Dip[1];
		case 0x11: return System1Dip[0];
		case 0x15: return System1VideoMode;
		case 0x19: return System1VideoMode;
	}	
	return 0;
}

inline void __fastcall system1_soundport_w(UINT8 d)
{
	System1SoundLatch = d;
	z80_cause_NMI();
}

void __fastcall System1Z801PortWrite(unsigned short a, UINT8 d)
{
	a &= 0xff;
	
	switch (a) {
		case 0x14:
		case 0x18: 
		{
			system1_soundport_w(d);
			return;
		}
		
		case 0x15:		
		case 0x19: 
		{
			System1VideoMode = d;
			System1FlipScreen = d & 0x80;
			return;
		}
		case 0x1c: return; // NOP
	}
}

void system1_backgroundram_w(unsigned short a, UINT8 d)
{	 
	if(RamStart[a]!=d)
	{
		RamStart[a] = d;
		a&=~1;
		int Code;//, Colour;
		Code = (RamStart[a + 1] << 8) | RamStart[a + 0];
		Code = ((Code >> 4) & 0x800) | (Code & 0x7ff);

		unsigned int x = map_offset_lut[a&0x7ff];
		UINT16 *map = &ss_map[x]; 
//		ss_map[x]     = ss_map[x+0x40] = ss_map[x+0x1000] = ss_map[x+0x1040] = (Code >> 5) & 0x3f;;//color_lut[Code];
//		ss_map[x+1] = ss_map[x+0x41] = ss_map[x+0x1001] = ss_map[x+0x1041] = Code & (System1NumTiles-1);
		map[0] = map[0x40] = map[0x1000] = map[0x1040] = ((Code >> 5) & 0x3f)//	+0x3000;
																						|(((RamStart[a + 1] & 0x08)==8)?0x2000:0x0000);//color_lut[Code];
		map[1] = map[0x41] = map[0x1001] = map[0x1041] = Code & (System1NumTiles-1);
	}
}

void system1_foregroundram_w(unsigned short a, UINT8 d) 
{
	if(RamStart1[a]!=d)
	{
		RamStart1[a] = d;
		a&=~1;

		unsigned int Code = (RamStart1[a + 1] << 8) | RamStart1[a + 0];
		Code = ((Code >> 4) & 0x800) | (Code & 0x7ff);

		unsigned int x = map_offset_lut[a&0x7ff];
		UINT16 *map = &ss_map2[x];		
		map[0] = (Code >> 5) & 0x3f; // |(((RamStart[a + 1] & 0x08)==8)?0x2000:0x0000);;//color_lut[Code];
		map[1] = Code & (System1NumTiles-1);
	}
}

void __fastcall System1Z801ProgWrite(unsigned short a, UINT8 d)
{
	(*p[(a>>8)-0xd8])(a, d);
}

void system1_bgcollisionram_w(unsigned short a, UINT8 d)
{
	System1BgCollisionRam[a&0x3ff] = 0x7e;
}
void system1_sprcollisionram_w(unsigned short a, UINT8 d)
{
	System1SprCollisionRam[a&0x3ff]  = 0x7e;
}

void system1_paletteram_w(unsigned short a, UINT8 d)
{
	a&= 0x1ff;
	if(System1PaletteRam[a]!=d)
		{colAddr[a] = cram_lut[d];	System1PaletteRam[a] = d;}
}

void system1_paletteram2_w(unsigned short a, UINT8 d)
{
	a&= 0x3ff;
	if(System1PaletteRam[a]!=d)
		{colBgAddr[remap8to16_lut[a&0x1ff]] = cram_lut[d];	System1PaletteRam[a] = d;}
}

void system1_paletteram3_w(unsigned short a, UINT8 d)
{
		a&= 0x5ff;	
		if(System1PaletteRam[a]!=d)
			{	colBgAddr2[remap8to16_lut[a&0x1ff]] = cram_lut[d]; System1PaletteRam[a] = d;}
}

UINT8 __fastcall System1Z802ProgRead(unsigned int a)
{
	switch (a) {
		case 0xe000: {
			return System1SoundLatch;
		}
		
		case 0xffff: {
			return System1SoundLatch;
		}
	}
	return 0;
}

void __fastcall System1Z802ProgWrite(unsigned int a, UINT8 d)
{
	switch (a) {
		case 0xa000:
		case 0xa001:
		case 0xa002:
		case 0xa003: {
			SN76496Write(0, d);
//			PSG_Write(0,d);
			return;
		}
		
		case 0xc000:
		case 0xc001:
		case 0xc002:
		case 0xc003: {
			SN76496Write(1, d);
//			PSG_Write(1,d);
			return;
		}
	}
}
/*==============================================================================================
Driver Inits
===============================================================================================*/
void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));	
 	colAddr             = (Uint16*)COLADDR;//(Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	colBgAddr         = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_NBG0,OFF);
	colBgAddr2       = (Uint16*)SCL_AllocColRam(SCL_NBG2,ON);
//	unsigned short palette[4]={RGB( 164>>3, 181>>3, 197>>3),RGB( 0,0,0 ),RGB( 164>>3, 181>>3, 197>>3),RGB( 214>>3, 230>>3, 247>>3 )};
	SCL_SetColRam(SCL_NBG0,8,4,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initLayers()
{
    Uint16	CycleTb[]={
		0x1f56, 0xeeee, //A0
		0xffff, 0xffff,	//A1
		0xf5f2,0x4eee,   //B0
		0xffff, 0xffff  //B1
	};
 	SclConfig	scfg;

	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD;
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.plate_addr[0] = (Uint32)ss_map2;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG1, &scfg);

//	scfg.dispenbl      = ON;
//	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
//	scfg.pnamesize     = SCL_PN2WORD;
	scfg.platesize     = SCL_PL_SIZE_2X1; // ou 2X2 ?
//	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
//	scfg.datatype      = SCL_CELL;
	scfg.plate_addr[0] = (Uint32)ss_map;
	scfg.plate_addr[1] = (Uint32)ss_map+0x1000;
//	scfg.plate_addr[2] = (Uint32)ss_map+0x1000;
//	scfg.plate_addr[3] = (Uint32)ss_map+0x1000;
//	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG2, &scfg);

//	scfg.dispenbl 		 = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
//	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 		 = SCL_BITMAP;
	scfg.mapover		 = SCL_OVER_0;
	scfg.plate_addr[0]	 = (Uint32)ss_font;
// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initSpritesS1(void)
{
    int i;
	initSprites(256-1,240-1,0,0,-8,0);

	for (i=3;i<35 ;i++ )
	{
		if(flipscreen)
			ss_sprite[i].control = ( JUMP_NEXT |  FUNC_DISTORSP );
		else
			ss_sprite[i].control = ( JUMP_NEXT | FUNC_NORMALSP );

		ss_sprite[i].drawMode   = ( COLOR_1 | ECD_DISABLE | COMPO_REP);		
	}
	SS_SET_SPCLMD(1);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void make_lut(void)
{
	unsigned int i,delta=0;
	int sx, sy;

	for (i = 0; i < 512;i++) 
	{
		remap8to16_lut[i] = delta;
		delta++; if ((delta & 7) == 0) delta += 8;
	}

	for (i = 0; i < 0x800;i++) 
	{
		if(flipscreen==0)
		{
			sx = ((i >> 1) & 0x1f); //% 32;
			sy = i & (~0x3f);
		}
		else if(flipscreen==1)
		{
			sx = ((i) & 0x3f)<<5;//% 32;
			sy = (32-(i >> 6)) & 0x1f;
		}
		else
		{
			sx = 36-(32-(i >> 6)) & 0x1f;	
			sy = ((64-i) & 0x3f)<<5;//% 32;
		}

		map_offset_lut[i] = ((sx) | sy)<<1;
	}

//	for (i = 0; i < System1NumTiles;i++)code_lut[i] = (((i >> 4) & 0x800) | (i & 0x7ff))& (System1NumTiles-1);
	for (i = 0; i < 10; i++)						cpu_lut[i] = (i + 1) * nCyclesTotal[0] / 10;
	for(i=0;i<256;i++)							if(i%8==0)	width_lut[i] = i;else	width_lut[i] = (i + (7)) & ~(7);
//		width_lut[i] = (i + (7)) & ~(7);
//	for(i=0;i<0x2000;i++)		color_lut[i] = (i>>5) & 0x3f;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void SaturnInitMem()
{
	p[8] = system1_backgroundram_w;
	p[9] = system1_backgroundram_w;
	p[10] = system1_backgroundram_w;
	p[11] = system1_backgroundram_w;
	p[12] = system1_backgroundram_w;
	p[13] = system1_backgroundram_w;
	p[14] = system1_backgroundram_w;
	p[15] = system1_backgroundram_w;

	p[16] = system1_foregroundram_w;
	p[17] = system1_foregroundram_w;
	p[18] = system1_foregroundram_w;
	p[19] = system1_foregroundram_w;
	p[20] = system1_foregroundram_w;
	p[21] = system1_foregroundram_w;
	p[22] = system1_foregroundram_w;

	p[24] = system1_bgcollisionram_w;
	p[25] = system1_bgcollisionram_w;
	p[26] = system1_bgcollisionram_w;
	p[27] = system1_bgcollisionram_w;

	p[32] = system1_sprcollisionram_w;
	p[33] = system1_sprcollisionram_w;
	p[34] = system1_sprcollisionram_w;
	p[35] = system1_sprcollisionram_w;

	p[0] = system1_paletteram_w;
	p[1] = system1_paletteram_w;

	p[2] = system1_paletteram2_w;
	p[3] = system1_paletteram2_w;

	p[4] = system1_paletteram3_w;
	p[5] = system1_paletteram3_w;

//	p[23] = system1_ef_w;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	nSoundBufferPos = 0;
	nBurnSprites  = 35;
	SS_MAP     = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B1;//+0x1E000;
	SS_MAP2   = ss_map2 =(Uint16 *)SCL_VDP2_VRAM_A1;//+0x1C000;
	SS_FONT   = ss_font    =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_CACHE = cache     =(Uint8  *)SCL_VDP2_VRAM_A0;
	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;
	ss_OtherPri       = (SclOtherPriRegister *)SS_OTHR;
	ss_sprite  = (SprSpCmd *)SS_SPRIT;
	ss_scl      = (Fixed32 *)SS_SCL;

	SaturnInitMem();

	if(flipscreen)
	{
		ss_regs->tvmode = 0x8011;
		if(flipscreen==1)
		{
			ss_reg->n1_move_y =  8 <<16;
			ss_reg->n1_move_x =  8 <<16;
		}
		else if(flipscreen==2)
		{
			ss_reg->n1_move_y =  16 <<16;
	//		ss_reg->n1_move_x =  -32 <<16;
		}
	}
	UINT8 *ss_vram   = (UINT8 *)SS_SPRAM;
	memset(&ss_vram[0x1100],0x00,0x7EF00);
	SS_SET_N2PRIN(4);
	SS_SET_S0PRIN(4);
	SS_SET_N1PRIN(6);
	SS_SET_N0PRIN(7);
	
	SS_SET_N2SPRM(2);  // 1 for special priority
	ss_regs->specialcode=0x000e; // sfcode, upper 8bits, function b, lower 8bits function a
	ss_regs->specialcode_sel=0; // sfsel, bit 0 for nbg0 // 1 sfcs, bit 0 = 1 for funcion code b, 0 for function code a

	initLayers();
	initColors();
	initSpritesS1();
//	SPR_InitSlaveSH();
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
	if(flipscreen)
		drawWindow(0,240,0,8,64);
	else
		drawWindow(0,224,240,0,66);
}
//-------------------------------------------------------------------------------------------------------------------------------------
int System1Init(int nZ80Rom1Num, int nZ80Rom1Size, int nZ80Rom2Num, int nZ80Rom2Size, int nTileRomNum, int nTileRomSize, int nSpriteRomNum, int nSpriteRomSize, bool bReset)
{
	int nRet = 0, n, i, RomOffset;
	struct BurnRomInfo ri;

	System1NumTiles = (((nTileRomNum * nTileRomSize) / 3) * 8) / (8 * 8);
	System1SpriteRomSize = nSpriteRomNum * nSpriteRomSize;

	DrvInitSaturn();

	CollisionFunction = updateCollisions;
	//System1BgRamSize = 0x800;
	// Allocate and Blank all required memory
	Mem = NULL;
	MemIndex();
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"BurnMalloc                     ",20,100);
	if ((Mem = (UINT8 *)BurnMalloc(MALLOC_MAX)) == NULL) 
	{	
		return 1;
	}
	memset(Mem, 0, MALLOC_MAX);
	MemIndex();
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"memset                     ",20,100);
	UINT8 *System1TempRom = (UINT8*)0x00200000;
	memset((void *)System1TempRom, 0, 0x40000);
	// Load Z80 #1 Program roms
	RomOffset = 0;
	for (i = 0; i < nZ80Rom1Num; i++) {
		nRet = BurnLoadRom(System1Rom1 + (i * nZ80Rom1Size), i + RomOffset, 1); if (nRet != 0) return 1;
		BurnDrvGetRomInfo(&ri, i);
	}

	if (System1BankedRom)
	{
		memcpyl(System1TempRom, System1Rom1, 0x40000);
		memset(System1Rom1, 0, 0x40000);

		if (System1BankedRom == 1)
		{ // Encrypted, banked
			memcpyl(System1Rom1 + 0x00000, System1TempRom + 0x00000, 0x8000);
			memcpyl(System1Rom1 + 0x10000, System1TempRom + 0x08000, 0x8000);
			memcpyl(System1Rom1 + 0x18000, System1TempRom + 0x10000, 0x8000);
		}

		if (System1BankedRom == 2)
		{ // Unencrypted, banked
			memcpyl(System1Rom1 + 0x20000, System1TempRom + 0x00000, 0x8000);
			memcpyl(System1Rom1 + 0x00000, System1TempRom + 0x08000, 0x8000);
			memcpyl(System1Rom1 + 0x30000, System1TempRom + 0x10000, 0x8000);//fetch
			memcpyl(System1Rom1 + 0x10000, System1TempRom + 0x18000, 0x8000);
			memcpyl(System1Rom1 + 0x38000, System1TempRom + 0x20000, 0x8000);//fetch
			memcpyl(System1Rom1 + 0x18000, System1TempRom + 0x28000, 0x8000);

			if ((UINT32)nZ80Rom1Size == (ri.nLen * 2))
			{ // last rom half the size, reload it into the last slot
				memcpyl (System1Rom1 + 0x18000, System1TempRom + 0x20000, 0x8000);
			}
		}
	}

	memset((void *)System1Rom2, 0, 0x10000);
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"DecodeFunction                     ",20,100);
//wait_vblank();
	if (DecodeFunction) DecodeFunction();
	
	// Load Z80 #2 Program roms
	RomOffset += nZ80Rom1Num;
	for (i = 0; i < nZ80Rom2Num; i++) {
		nRet = BurnLoadRom(System1Rom2 + (i * nZ80Rom2Size), i + RomOffset, 1); if (nRet != 0) return 1;
	}
	
	// Load and decode tiles
	memset(System1TempRom, 0, 0x20000);
	RomOffset += nZ80Rom2Num;
	for (i = 0; i < nTileRomNum; i++) {
		nRet = BurnLoadRom(System1TempRom + (i * nTileRomSize), i + RomOffset, 1);
	}

	UINT32 TilePlaneOffsets[3]  = { RGN_FRAC((nTileRomSize * nTileRomNum), 0, 3), RGN_FRAC((nTileRomSize * nTileRomNum), 1, 3), RGN_FRAC((nTileRomSize * nTileRomNum), 2, 3) };

	UINT32 TileXOffsets[8]      = { 0, 1, 2, 3, 4, 5, 6, 7 };
	UINT32 TileYOffsets[8]      = { 0, 8, 16, 24, 32, 40, 48, 56 };
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"GfxDecode4Bpp                     ",20,100);

	if (System1NumTiles > 0x800)
	{
		UINT32 NoboranbTilePlaneOffsets[3]  = { 0, 0x40000, 0x80000 };
		GfxDecode4Bpp(System1NumTiles, 3, 8, 8, NoboranbTilePlaneOffsets, TileXOffsets, TileYOffsets, 0x40, System1TempRom, cache);
	}
	else
		GfxDecode4Bpp(System1NumTiles, 3, 8, 8, TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x40, System1TempRom, cache);

	System1TempRom = NULL;
	
	memset((void *)&ss_map2[2048],0,768);
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"rotate_tile                     ",20,100);

	if(flipscreen==1)		rotate_tile(System1NumTiles,0,cache);
	else if(flipscreen==2)	rotate_tile(System1NumTiles,1,cache);

	spriteCache = (UINT16*)(0x00200000);

	memset((unsigned char *)spriteCache,0xFF,0x80000);

	memset(System1Sprites, 0x00, System1SpriteRomSize);
	
	// Load Sprite roms
	RomOffset += nTileRomNum;
	for (i = 0; i < nSpriteRomNum; i++) 
	{
		nRet = BurnLoadRom(System1Sprites + (i * nSpriteRomSize), i + RomOffset, 1);
	}

	// Load Colour proms
	if (System1ColourProms) {
		RomOffset += nSpriteRomNum;
		nRet = BurnLoadRom(System1PromRed, 0 + RomOffset, 1);
		nRet = BurnLoadRom(System1PromGreen, 1 + RomOffset, 1);
		nRet = BurnLoadRom(System1PromBlue, 2 + RomOffset, 1);
	}
	// Setup the Z80 emulation
// remettre 1 plus tard quand choplifter sera corrigé
	CZetInit2(2,CZ80Context);

	CZetOpen(0);
	CZetSetWriteHandler(System1Z801ProgWrite);
	CZetSetInHandler(System1Z801PortRead);
	CZetSetOutHandler(System1Z801PortWrite);
	CZetMapArea(0x0000, 0x7fff, 0, System1Rom1);
	CZetMapArea(0x8000, 0xbfff, 0, System1Rom1 + 0x8000);

	if (DecodeFunction) 
	{
		CZetMapArea2(0x0000, 0x7fff, 2, System1Fetch1, System1Rom1);
		CZetMapArea2(0x8000, 0xbfff, 2, System1Fetch1 + 0x10000, System1Rom1 + 0x10000);
	}
	else
	{
		CZetMapArea(0x0000, 0x7fff, 2, System1Rom1);
		CZetMapArea(0x8000, 0xbfff, 2, System1Rom1 + 0x8000);
	}
	CZetMapArea(0xc000, 0xcfff, 0, System1Ram1);
	CZetMapArea(0xc000, 0xcfff, 1, System1Ram1);
	CZetMapArea(0xc000, 0xcfff, 2, System1Ram1);
	CZetMapArea(0xd000, 0xd1ff, 0, System1SpriteRam);
	CZetMapArea(0xd000, 0xd1ff, 1, System1SpriteRam);
	CZetMapArea(0xd000, 0xd1ff, 2, System1SpriteRam);
	CZetMapArea(0xd200, 0xd7ff, 0, System1Ram1 + 0x1000);
	CZetMapArea(0xd200, 0xd7ff, 1, System1Ram1 + 0x1000);
	CZetMapArea(0xd200, 0xd7ff, 2, System1Ram1 + 0x1000);
	CZetMapArea(0xd800, 0xddff, 0, System1PaletteRam);
//	CZetMapArea(0xd800, 0xddff, 1, System1PaletteRam);
//	CZetMapArea(0xd800, 0xddff, 2, System1PaletteRam);
	CZetMapArea(0xde00, 0xdfff, 0, System1deRam);
	CZetMapArea(0xde00, 0xdfff, 1, System1deRam);
	CZetMapArea(0xde00, 0xdfff, 2, System1deRam);
	CZetMapArea(0xe000, 0xe7ff, 0, System1BgRam); // on enleve le read pour tester wbml
//	CZetMapArea(0xe000, 0xe7ff, 1, System1BgRam);
	CZetMapArea(0xe000, 0xe7ff, 2, System1BgRam);
	CZetMapArea(0xe800, 0xeeff, 0, System1VideoRam);
//	CZetMapArea(0xe800, 0xeeff, 1, System1VideoRam);
	CZetMapArea(0xe800, 0xeeff, 2, System1VideoRam);
	CZetMapArea(0xef00, 0xefff, 0, System1efRam);
// VBT ajout
	CZetMapArea(0xef00, 0xefff, 1, System1efRam);

	CZetMapArea(0xef00, 0xefff, 2, System1efRam);
	CZetMapArea(0xf000, 0xf3ff, 0, System1BgCollisionRam); // read 0 // write 1 // fetch 2
	CZetMapArea(0xf000, 0xf3ff, 2, System1BgCollisionRam);
	CZetMapArea(0xf400, 0xf7ff, 0, System1f4Ram);
	CZetMapArea(0xf400, 0xf7ff, 1, System1f4Ram);
	CZetMapArea(0xf400, 0xf7ff, 2, System1f4Ram);
	CZetMapArea(0xf800, 0xfbff, 0, System1SprCollisionRam);
	CZetMapArea(0xf800, 0xfbff, 2, System1SprCollisionRam);
	CZetMapArea(0xfc00, 0xffff, 0, System1fcRam);
	CZetMapArea(0xfc00, 0xffff, 1, System1fcRam);
	CZetMapArea(0xfc00, 0xffff, 2, System1fcRam);	
	CZetMemEnd();
	CZetClose();

	z80_init_memmap();
	z80_add_read(0xe000, 0xe001, 1, (void *)&System1Z802ProgRead); 
	z80_add_write(0xa000, 0xa003, 1, (void *)&System1Z802ProgWrite);
	z80_add_write(0xc000, 0xc003, 1, (void *)&System1Z802ProgWrite);
	z80_map_read  (0x0000, 0x7fff, System1Rom2);
	z80_map_fetch (0x0000, 0x7fff, System1Rom2);
	z80_map_read  (0x8000, 0x87ff, System1Ram2);
	z80_map_write (0x8000, 0x87ff, System1Ram2); //ajout
	z80_map_fetch (0x8000, 0x87ff, System1Ram2);
// derek fix for wbml sound :)
	z80_map_read  (0x8800, 0x8fff, System1Ram2);
	z80_map_write (0x8800, 0x8fff, System1Ram2); //ajout
	z80_map_fetch (0x8800, 0x8fff, System1Ram2);

	z80_end_memmap();
	z80_reset();

//	memset4_fast(SpriteOnScreenMap, 255, 256 * 256); plante sur saturn
	memset(SpriteOnScreenMap, 255, 0x10000);

	nCyclesTotal[0] = 2500000 / hz ;//3500000
	nCyclesTotal[1] = 2500000 / hz ;//3500000

	SN76489AInit(0, 2000000, 0);	  //2000000

	SN76489AInit(1, 4000000, 1);//4000000
	
	make_lut();
	make_cram_lut();
	MakeInputsFunction = System1MakeInputs;
	
	// Reset the driver
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"System1DoReset                     ",20,100);

	if (bReset) 
	{	
		System1DoReset();
	}
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"System1CalcPalette                     ",20,100);
	
	System1CalcPalette();

	System1efRam[0xfe] = 0x4f;
	System1efRam[0xff] = 0x4b;
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
int System1Exit()
{
	SS_SET_N0SPRM(0);
	SS_SET_N2SPRM(0);
	ss_regs->specialcode=0x0000;
	nBurnFunction = NULL;
	System1DoReset();
	CZetExit2();

	z80_stop_emulating();
	z80_add_read(0xe000, 0xe001, 1, (void *)NULL); 
	z80_add_write(0xa000, 0xa003, 1, (void *)NULL);
	z80_add_write(0xc000, 0xc003, 1, (void *)NULL);
	z80_set_in((unsigned char (*)(unsigned short))NULL);
	z80_set_out((void (*)(unsigned short, unsigned char))NULL);
	nBurnFunction = NULL;

    while(((*(volatile unsigned short *)0x25F80004) & 8) == 8);
    while(((*(volatile unsigned short *)0x25F80004) & 8) == 0);
	memset(map_dirty,1,0);
	CZ80Context = NULL;

	SN76496Exit();

RamStart1 = RamStart               = NULL;
System1Rom1 = System1Rom2 = NULL;
System1PromRed = System1PromGreen = System1PromBlue = NULL;
System1Ram1 = System1Ram2 = NULL;
System1SpriteRam = System1PaletteRam = NULL;
System1BgRam = System1VideoRam = NULL;
System1ScrollXRam = System1BgCollisionRam = NULL;
System1SprCollisionRam = NULL;
System1deRam = System1efRam = System1f4Ram = System1fcRam = NULL;
/*System1Tiles =*/ SpriteOnScreenMap = NULL;
System1Fetch1 = NULL;
System1ScrollX = System1ScrollY = NULL;

remap8to16_lut = NULL;
map_offset_lut = NULL;
cpu_lut = NULL;
cram_lut = NULL;
width_lut = NULL;
spriteCache = NULL;
map_cache = NULL;
map_dirty = NULL;

	System1Sprites = NULL;
	free(Mem);
	Mem = NULL;
	DecodeFunction = NULL;
	MakeInputsFunction = NULL;
	CollisionFunction = NULL;

	SPR_InitSlaveSH();
	
	cleanDATA();
	cleanBSS();

	nSoundBufferPos=0;	
	return 0;
}
/*==============================================================================================
Graphics Rendering
===============================================================================================*/
/*static*/ void updateCollisions(int *values)
{
	int x=values[0],y=values[1];
	int xr,yr,SpriteOnScreen;
	int xend=x+values[2];
	int yend=y+values[3];
	int Num=values[4];
//	int y256;
	if(x < 0) x = 0;
	if(y < 0) y = 0;

	for (;y<yend ; y++)
	{
		if (y > 255) continue;

		yr = (((y - System1BgScrollY) & 0xff) >>3)<<5;
//		y256 = y<<8;
		UINT8 *sprScreenMap= &SpriteOnScreenMap[(y<<8)+x];

		for(x=values[0];x<xend;x++)
		{
			if (x > 255) continue;

			if (*sprScreenMap != 255) 
			{
				SpriteOnScreen = *sprScreenMap;
				System1SprCollisionRam[SpriteOnScreen + (32 * Num)] = 0xff;
			}
			*sprScreenMap++ = Num;

			xr = ((x - System1BgScrollX) & 0xff) / 8;
 // vbt à remettre !!!!
			if (System1BgRam[2 * ( yr + xr) + 1] & 0x10) 
			{
				System1BgCollisionRam[0x20 + Num] = 0xff;
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ int System1CalcPalette()
{
	unsigned int delta=0;		
	UINT8 *System1PaletteRam512   = System1PaletteRam+512;
	UINT8 *System1PaletteRam1024 = System1PaletteRam+1024;

	for (int i = 511; i > 0; i--) 
	{
/*		colAddr[i]				    = cram_lut[System1PaletteRam[i]];
		colBgAddr[delta]		= cram_lut[*System1PaletteRam512++];
		colBgAddr2[delta]		= cram_lut[*System1PaletteRam1024++];	  */
		colAddr[i]				    = cram_lut[System1PaletteRam[i]];
		colBgAddr[delta]		= cram_lut[*System1PaletteRam512];
		++System1PaletteRam512;
		colBgAddr2[delta]		= cram_lut[*System1PaletteRam1024];
		++System1PaletteRam1024;
		delta++; if ((delta & 7) == 0) delta += 8;  
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void renderSpriteCache(int *values)
//(int Src,unsigned int Height,INT16 Skip,unsigned int Width, int Bank)
{
	int Src = values[0];
	UINT32 Height = values[1];
	INT32 Skip = values[2];
	UINT32 Width  = values[3];
	UINT8 *ss_vram   = (UINT8 *)SS_SPRAM;
	UINT8 *spriteVRam=(Uint8 *)&ss_vram[0x1100+(values[5]<<3)];
	UINT8 *spr = &System1Sprites[values[4]];

	for (UINT32 Row = 0; Row < Height; Row++) 
	{
		int Src2;
		Src = Src2 = Src + Skip;
		unsigned int n = Row*Width;

		UINT8 Colour1, Colour2, Data;		
// remarque martin faire 2 boucles while
		if(Src & 0x8000)
		{
			while(1) 
			{
				Data = spr[(Src2 & 0x7fff)];

				Src2--;
				Colour1 = Data & 0x0f;

				if (Colour1 == 0x0f) break;
				Colour2 = Data >> 4;

				if (Colour2 == 0x0f) 
				{
					spriteVRam[n]=(Colour1<<4);// | 0x0;
					break;
				}
				spriteVRam[n++]=Colour2 | (Colour1<<4);
			}
		}
		else
		{
			while(1) 
			{
				Data = spr[(Src2 & 0x7fff)];

				Src2++;
				Colour1 = Data >> 4;

				if (Colour1 == 0x0f) break;
				Colour2 = Data & 0x0f;

				if (Colour2 == 0x0f) 
				{
					spriteVRam[n]=Data & 0xf0;// | 0x0;
					break;
				}
				spriteVRam[n++]=Data ;//Colour2 | (Colour1<<4);
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void System1DrawSprites()
{
	UINT8 *SpriteBase;
	if(CollisionFunction)
	{
		memset4_fast(SpriteOnScreenMap, 255, 256 * 256);
	}
	
	SprSpCmd *ss_spritePtr = &ss_sprite[3];	
	
// VBT 25/06/2017 : coorection dink pour pitfall 2
	if (System1SpriteRam[0] == 0xff)
	{
		for (UINT32 i = 0; i < 32; ++i)
		{
			ss_spritePtr[i].ax = ss_spritePtr[i].ay = ss_spritePtr[i].charSize = ss_spritePtr[i].charAddr = 0;
		}
		return; // 0xff in first byte of spriteram is sprite-disable mode
	}

	for (UINT32 i = 0; i < 32; ++i) 
	{
		SpriteBase = System1SpriteRam + (i << 4);
		if (SpriteBase[1] && (SpriteBase[1] - SpriteBase[0] > 0))
		{	
			UINT32 Src = (SpriteBase[7] << 8) | SpriteBase[6];
			UINT32 Bank = 0x8000 * (((SpriteBase[3] & 0x80) >> 7) + ((SpriteBase[3] & 0x40) >> 5));
			Bank &= System1SpriteRomSize -1;
			UINT16 Skip = ((SpriteBase[5] << 8) | SpriteBase[4]);
			unsigned int addr = Bank + ((Src + Skip) & 0x7fff);

			if (spriteCache[addr]!=0xFFFF)
				DrawSpriteCache(i,Bank,addr,Skip,SpriteBase);
			else
				 DrawSprite(i,Bank,addr,Skip,SpriteBase);
		}
		else
		{
			ss_spritePtr[i].ax = ss_spritePtr[i].ay = ss_spritePtr[i].charSize = ss_spritePtr[i].charAddr = 0;
			sprites_collision[i].width=0;
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void renderSound(unsigned int *nSoundBufferPos)
{
	signed short *nSoundBuffer = (signed short *)0x25a20000;
//	unsigned int  deltaSlave    = *(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);
	SN76496Update(0, &nSoundBuffer[*nSoundBufferPos], nSegmentLength);
	SN76496Update(1, &nSoundBuffer[*nSoundBufferPos], nSegmentLength);
	nSoundBufferPos[0]+=nSegmentLength;
	//	nSoundBufferPos[0]+= nSegmentLength;
//	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = deltaSlave;
}

/*==============================================================================================
Frame functions
===============================================================================================*/
int System1Frame()
{
	MakeInputsFunction();
	unsigned int nCyclesDone[2] = {0,0};
	
	for (UINT32 i = 0; i < nInterleave; i++) {
		
		// Run Z80 #1
#ifdef CZ80
		CZetOpen(0);
#endif
		INT32 nCyclesSegment = cpu_lut[i] - nCyclesDone[0];
#ifdef CZ80
		nCyclesDone[0] += CZetRun(nCyclesSegment);
#endif

#ifdef CZ80
		if (i == 9) CZetRaiseIrq(0);
		CZetClose();
#endif
		SPR_RunSlaveSH((PARA_RTN*)renderSound,&nSoundBufferPos);
//vbt à précalculer !!!
//		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
//		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesSegment = cpu_lut[i] - nCyclesDone[1];
#ifdef USE_RAZE1
		nCyclesSegment = z80_emulate(nCyclesSegment);
		nCyclesDone[1] += nCyclesSegment;
		if (i == 2 || i == 4 || i == 6 || i == 8) 
		{
			z80_raise_IRQ(0);
			z80_emulate(1);
//			z80_lower_IRQ(0);
			z80_lower_IRQ();
			z80_emulate(1);
		}
#endif
	if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
		SPR_WaitEndSlaveSH();
	}

	System1Render();

	if(nSoundBufferPos>=RING_BUF_SIZE/2)//0x4800-nSegmentLength)//
	{
  		PCM_NotifyWriteSize(pcm, nSoundBufferPos);		
		nSoundBufferPos=0;
	}
	PCM_Task(pcm);
// evite plantage sur teddy boy	
	if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
		SPR_WaitEndSlaveSH();
//	sc_check();
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void make_cram_lut(void)
{
	if (System1ColourProms) 
	{
		for (UINT32 j = 0; j < 256; j++) 
		{
			UINT32 bit0, bit1, bit2, bit3, r, g, b, val;

			val = System1PromRed[j];
			bit0 = (val >> 0) & 0x01;
			bit1 = (val >> 1) & 0x01;
			bit2 = (val >> 2) & 0x01;
			bit3 = (val >> 3) & 0x01;
			r = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;
		
			val = System1PromGreen[j];
			bit0 = (val >> 0) & 0x01;
			bit1 = (val >> 1) & 0x01;
			bit2 = (val >> 2) & 0x01;
			bit3 = (val >> 3) & 0x01;
			g = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;
		
			val = System1PromBlue[j];
			bit0 = (val >> 0) & 0x01;
			bit1 = (val >> 1) & 0x01;
			bit2 = (val >> 2) & 0x01;
			bit3 = (val >> 3) & 0x01;
			b = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;

			cram_lut[j] = BurnHighCol(r, g, b, 0);	
		}
	}
	else
	{
		for(UINT32 j = 0; j < 256; j++)
		{
			int r = (j >> 0) & 7;
			int g = (j >> 3) & 7;
			int b = (j >> 6);

			r = (r << 2) | (r >> 1);
			g = (g << 2) | (g >> 1);
			b = (b << 3) | (b << 1) | (b >> 1);
			cram_lut[j] = RGB(r,g,b);
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
