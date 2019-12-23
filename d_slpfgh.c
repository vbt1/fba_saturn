// Tiger Heli, Get Star / Guardian, & Slap Fight

#include "d_slpfgh.h"
//#define nVBlankCycles 248 * 4000000 / 60 / 262
//#define nInterleave 12
#define nInterleave 64
//#define nTigerHeliSpriteMask 0x3ff
//#define nTigerHeliTileMask 0xfff
#define nCyclesTotal0 4000000 / 60
#define nCyclesTotal1 2000000 / 60
#define nCycleSegment0 nCyclesTotal0 / nInterleave 
#define nCycleSegment1 nCyclesTotal1 / nInterleave 
	
int ovlInit(char *szShortName)
{
	cleanBSS();
//*((short*) 0xFFFFFE92) = *((short*) 0xFFFFFE92)&~0x1;

	struct BurnDriver nBurnDrvSlapBtJP = {
		"slapfib1", "slpfgh",
		"Slap Fight (bootleg set 1)",
		slapbtjpRomInfo, slapbtjpRomName, SlapfighInputInfo, SlapfighDIPInfo,
		DrvInit, DrvExit, DrvFrame
	};

	struct BurnDriver nBurnDrvTigerHB1 = {
		"tigerhb1", "slpfgh", 			
		"Tiger Heli (bootleg, set 1)",
		tigerhb1RomInfo, tigerhb1RomName, TigerhInputInfo, TigerhDIPInfo,
		DrvInit, DrvExit, DrvFrame
	};

	if (strcmp(nBurnDrvSlapBtJP.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvSlapBtJP,sizeof(struct BurnDriver));

	if (strcmp(nBurnDrvTigerHB1.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvTigerHB1,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}
//-------------------------------------------------------------------------------------------------------------------------------------
INT32 MemIndex()
{
	UINT8* Next; Next = Mem;
	CZ80Context			= (UINT8 *)Next; Next += (sizeof(cz80_struc)*2);

	DrvZ80ROM0			= (UINT8 *)Next; Next += 0x014000;		// Z80 main program
	DrvZ80RAM0			= (UINT8 *)Next; Next += 0x000800;		// Z80 main work RAM
	RamShared			= (UINT8 *)Next; Next += 0x000800;		// Shared RAM
	DrvTxtRAM			= (UINT8 *)Next; Next += 0x001000;
	DrvSprRAM			= (UINT8 *)Next; Next += 0x000800;
	DrvSprBuf			= (UINT8 *)Next; Next += 0x000800;
	DrvVidRAM			= (UINT8 *)Next; Next += 0x001300;
	map_offset_lut		= (UINT16 *)Next; Next += 0x800 * sizeof(UINT16);
	map_offset_lut2	 	= (UINT16 *)Next; Next += 0x800 * sizeof(UINT16);
	
	return 0;
}

// ---------------------------------------------------------------------------
//	Graphics
void DrvPaletteInit()
{
	UINT32 delta=0;
	for (UINT32 i = 0; i < 0x0100; i++) {
		UINT32 r, g, b;

		r = DrvVidRAM[i + 0x1000];	  // Red
		r |= r << 4;
		g = DrvVidRAM[i + 0x1100];	  // Green
		g |= g << 4;
		b = DrvVidRAM[i + 0x1200];	  // Blue
		b |= b << 4;

		colBgAddr[delta] = colBgAddr2[i] = /*TigerHeliPalette[i] =*/ BurnHighCol(r, g, b, 0);
		delta++; if ((delta & 3) == 0) delta += 12;
	}

	return;
}

// ---------------------------------------------------------------------------
UINT8 __fastcall tigerhReadCPU0(UINT16 a)
{
	switch (a) {
		case 0xE803: {
			
			UINT8 nProtectSequence[3] = { 0, 1, (0 + 5) ^ 0x56 };
			
			UINT8 val = nProtectSequence[nProtectIndex];
			nProtectIndex = (nProtectIndex + 1) % 3;
			return val;
		}

	}

	return 0;
}

UINT8 __fastcall tigerhReadCPU0_tigerhb1(UINT16 a)
{
	switch (a) {
		case 0xE803: {
			return 0x83;
		}
	}
	
	return 0;
}


void __fastcall tigerhWriteCPU0(UINT16 a, UINT8 d)
{
	if(a>= 0xD000 && a <= 0xD7FF)
	{
		a-= 0xD000;
		if(DrvVidRAM[a]!=d)
		{
			DrvVidRAM[a] = d;
			UINT32 attr  = d + (DrvVidRAM[0x800 + a] << 8);
			UINT32 code  = attr & nTigerHeliTileMask;
			UINT32 color = (attr & 0xf000) >> 12;

			UINT16 x	 = map_offset_lut[a];
			ss_map2[x]   = color;
			ss_map2[x+1] = code+0x1000;
		}
		return;
	}
	
	if(a>= 0xD800 && a <= 0xDFFF)
	{
		a-= 0xD000;
		if(DrvVidRAM[a]!=d)
		{
			DrvVidRAM[a] = d;
			a &=0x7ff;
			UINT32 attr  = DrvVidRAM[a] + (d  << 8);
			UINT32 code  = attr & nTigerHeliTileMask;
			UINT32 color = (attr & 0xf000) >> 12;

			UINT16 x	 = map_offset_lut[a];
			ss_map2[x]   = color;
			ss_map2[x+1] = code+0x1000;
		}
		return;
	}

	if(a>= 0xF000 && a <= 0xFFFF)
	{
		a-= 0xF000;
		if(DrvTxtRAM[a]!=d)
		{
			DrvTxtRAM[a] = d;
			UINT32 attr;
			if(a>=0x800)
			{
				a &=0x7ff;
				attr   = DrvTxtRAM[a] + (d << 8);
			}
			else
			{
				attr   = d + (DrvTxtRAM[0x800 + a] << 8);
			}
			UINT32 code  =  attr & 0x03ff;
			UINT32 color = (attr & 0xfc00) >> 10;

			UINT16 x	= map_offset_lut2[a];
			ss_map[x]	= color & 0x3f;
			ss_map[x+1] = code;
		}
		 return;
	}

	switch (a) {
		case 0xE800:
			scrollx = (scrollx & 0xff00) | d;
			break;
		case 0xE801:
			scrollx = (scrollx & 0x00ff) | (d << 8);
			break;
		case 0xE802:
			scrolly = d;
			break;

		case 0xe803: 

			break;

	}
}

UINT8 __fastcall tigerhInCPU0(UINT16 a)
{
	a &= 0xFF;

	switch (a) {
		case 0x00: {

			UINT8 nStatusSequence[3] = { 0xC7, 0x55, 0x00 };

			UINT8 nStatus = nStatusSequence[nStatusIndex];
			nStatusIndex++;
			if (nStatusIndex > 2) nStatusIndex = 0;
			return nStatus;
		}
	}

	return 0;
}

void __fastcall perfrman_write_port(UINT16 a, UINT8  d)
{
	a &= 0xFF;

	switch (a) {
		case 0x00:					// Assert reset line on sound CPU

//			if (bSoundCPUEnable) {
				CZetClose();
				CZetOpen(1);
				CZetReset();
				CZetClose();
				CZetOpen(0);

				sound_nmi_enable = 0;
//			}

			break;
		case 0x01:					// Release reset line on sound CPU
//			bSoundCPUEnable = true;
			break;

		case 0x02:
		case 0x03:
//			nFlipscreen = ~a & 1;
			break;

		case 0x06:					// Disable interrupts

			irq_enable = 0;
			CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);
			break;
		case 0x07:					// Enable interrupts

			irq_enable = 1;
			break;

		case 0x08:
			// ROM bank 0 selected
			CZetMapMemory(DrvZ80ROM0 + 0x8000, 0x8000, 0xBFFF, MAP_ROM);
			break;
		case 0x09:
			// ROM bank 1 selected
			CZetMapMemory(DrvZ80ROM0 + 0xC000, 0x8000, 0xBFFF, MAP_ROM);
			break;

		case 0x0c:
		case 0x0d:
//			nPalettebank = a & 1;
			break;;
	}
}

UINT8 __fastcall tigerhReadCPU1(UINT16 a)
{
	switch (a) {
		case 0xA081:
			return AY8910Read(0);
		case 0xA091:
			return AY8910Read(1);
	}

	return 0;
}

void __fastcall tigerhWriteCPU1(UINT16 a, UINT8 d)
{
	switch (a) {
		case 0xA080:
			AY8910Write(0, 0, d);
			break;
		case 0xA082:
			AY8910Write(0, 1, d);
			break;
		case 0xA090:
			AY8910Write(1, 0, d);
			break;
		case 0xA092:
			AY8910Write(1, 1, d);
			break;
		case 0xA0E0:
			sound_nmi_enable = true;
			break;
	}
}
/*
UINT8 __fastcall tigerhInCPU1(UINT16 a)
{
	return 0;
}

void __fastcall tigerhOutCPU1(UINT16 a, UINT8 d)
{
//	bprintf(PRINT_NORMAL, _T("Attempt by CPU1 to write port %02X -> %02X.\n"), a, d);
}
*/
UINT8 tigerhReadPort0(UINT32 data)
{
	return DrvInputs[0];
}
UINT8 tigerhReadPort1(UINT32 data)
{
	return DrvInputs[1];
}
UINT8 tigerhReadPort2(UINT32 data)
{
	return DrvDips[0];
}
UINT8 tigerhReadPort3(UINT32 data)
{
	return DrvDips[1];
}
// ---------------------------------------------------------------------------
INT32 DrvLoadRoms(UINT8 nWhichGame)
{
	UINT8* DrvGfxROM0	= (UINT8 *)(SS_CACHE); //Next; Next += 0x010000;
	UINT8* ss_vram		= (UINT8 *)SS_SPRAM;
	UINT8* DrvGfxROM2	= &ss_vram[0x2200]; //Next; Next += 3 * 16 * 16 * 256; // 3 banks, 16x16 tiles, 256 tiles (sprites)		
	UINT8* DrvGfxROM1	= (UINT8 *)(SS_CACHE+0x8000); //0x00280000;//Next; Next += 0x040000;

	// Z80 main program
	switch (nWhichGame) 
	{
		case 0:	// Tiger Heli
			if (BurnLoadRom(DrvZ80ROM0 + 0x0000, 0, 1)) return 1;
			if (BurnLoadRom(DrvZ80ROM0 + 0x4000, 1, 1)) return 1;
			if (BurnLoadRom(DrvZ80ROM0 + 0x8000, 2, 1)) return 1;
			break;
		case 2:	// Slap Fight
		{							
			if (BurnLoadRom(DrvZ80ROM0 + 0x0000, 0, 1)) return 1;
			if (BurnLoadRom(DrvZ80ROM0 + 0x8000, 1, 1)) return 1;
			break;
		}
	}

	// Sprites
	INT32 nRet = 0, nBaseROM = 0;
	switch (nWhichGame) {
		case 0:										// Tiger Heli
			nBaseROM = 3;
			break;
		case 2:										// Slap Fight
			nBaseROM = 2;
			break;
	}

	INT32 nSize;
	struct BurnRomInfo ri;

	ri.nType = 0;
	ri.nLen = 0;
	BurnDrvGetRomInfo(&ri, nBaseROM);
	nSize = ri.nLen;

	UINT8* pTemp = (UINT8*)0x00240000;

	for (UINT32 i = 0; i < 4; i++) {
		nRet |= BurnLoadRom(pTemp + nSize * i, nBaseROM + i, 1);
	}

	// Text layer
	switch (nWhichGame) 
	{
		case 0:										// Tiger Heli
			nBaseROM = 7;
			break;
		case 2:										// Slap Fight
			nBaseROM = 6;
			break;
	}
	pTemp = (UINT8*)0x002C0000;

	if (BurnLoadRom(pTemp + 0x0000, nBaseROM + 0, 1)) return 1;
	if (BurnLoadRom(pTemp + 0x2000, nBaseROM + 1, 1)) return 1;

	// Tile layer
	switch (nWhichGame) {
		case 0:										// Tiger Heli
			nBaseROM = 9;
			break;
		case 2:										// Slap Fight
			nBaseROM = 8;
			break;
	}

	ri.nType = 0;
	ri.nLen = 0;

	BurnDrvGetRomInfo(&ri, nBaseROM);
	nSize = ri.nLen;
	pTemp = (UINT8*)0x00280000;

	for (UINT32 i = 0; i < 4; i++) {
		nRet |= BurnLoadRom(pTemp + nSize * i, nBaseROM + i, 1);
	}

	INT32 Plane0[3]  = { 0x2000*8*0, 0x2000*8*1, 0x2000*8*2 };
	INT32 Plane1[4]  = { STEP4(0, (nSize)*8) };	
	INT32 XOffs0[16] = { STEP16(0,1) };
	INT32 YOffs0[8]  = { STEP8(0,8) };
	INT32 YOffs1[16] = { STEP16(0,16) };	
	
	pTemp = (UINT8*)0x002C0000;
	GfxDecode4Bpp(0x0400, 2,  8,  8, Plane0, XOffs0, YOffs0, 0x040, pTemp, DrvGfxROM0);
	pTemp = (UINT8*)0x00280000;
	GfxDecode4Bpp(nSize/8, 4,  8,  8, Plane1, XOffs0, YOffs0, 0x040, pTemp, DrvGfxROM1);
	pTemp = (UINT8*)0x00240000;
	GfxDecode4Bpp(nSize/32, 4, 16, 16, Plane1, XOffs0, YOffs1, 0x100, pTemp, DrvGfxROM2);		
	nTigerHeliTileMask = (nSize >> 3) - 1;

	rotate_tile(0x400,1,DrvGfxROM0);
	rotate_tile(nSize/8,1,DrvGfxROM1);
	rotate_tile16x16(nSize/32,DrvGfxROM2);

	// Colour PROMs
	switch (nWhichGame) {
		case 0:										// Tiger Heli
			nBaseROM = 13;
			break;
		case 2:										// Slap Fight
			nBaseROM = 12;
			break;
	}

	if (BurnLoadRom(DrvVidRAM + 0x1000, nBaseROM + 0, 1)) return 1;
	if (BurnLoadRom(DrvVidRAM + 0x1100, nBaseROM + 1, 1)) return 1;
	if (BurnLoadRom(DrvVidRAM + 0x1200, nBaseROM + 2, 1)) return 1;

	// Z80 program
	switch (nWhichGame) {
		case 0:										// Tiger Heli
			nBaseROM = 16;
			break;
		case 2:										// Slap Fight
			nBaseROM = 15;
			break;
	}
	if (BurnLoadRom(DrvZ80ROM0+0x12000, nBaseROM, 1)) return 1;

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initLayers()
{
    Uint16	CycleTb[]={
		0xfff4, 0x6fff, //A0
		0xffff, 0xffff,	//A1
		0x025e,0xffff,   //B0
		0xffff, 0xffff  //B1
//		0x4eff, 0x1fff, //B1
	};
 	SclConfig	scfg;

// 3 nbg
	scfg.dispenbl			= ON;
	scfg.charsize			= SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize		= SCL_PN2WORD; //2word
	scfg.platesize		= SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype			= SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype		= SCL_CELL;
	scfg.flip					= SCL_PN_10BIT; // on force à 0
	scfg.patnamecontrl =  0x0000; // a0 + 0x8000
	scfg.plate_addr[0] = (Uint32)SS_MAP;
	scfg.plate_addr[1] = NULL; //(Uint32)SS_MAP;
	scfg.plate_addr[2] = NULL; //(Uint32)SS_MAP;
	scfg.plate_addr[3] = NULL; //(Uint32)SS_MAP;
	SCL_SetConfig(SCL_NBG0, &scfg);
// 3 nbg

//	scfg.pnamesize		= SCL_PN1WORD; //2word
	scfg.platesize		= SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.flip					= SCL_PN_10BIT; // on force à 0
	scfg.patnamecontrl =  0x0000; // a1
	scfg.plate_addr[0] = (Uint32)SS_MAP2;
	scfg.plate_addr[1] = (Uint32)SS_MAP2+0x400;
	scfg.plate_addr[2] = (Uint32)SS_MAP2+0x400;
	scfg.plate_addr[3] = (Uint32)SS_MAP2+0x400;

	SCL_SetConfig(SCL_NBG2, &scfg);

//	scfg.dispenbl			= OFF;
	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
	scfg.plate_addr[0] = (Uint32)SS_FONT;
	scfg.plate_addr[1] = NULL;
	scfg.plate_addr[2] = NULL;
	scfg.plate_addr[3] = NULL;
// 3 nbg	
	SCL_SetConfig(SCL_NBG1, &scfg);
	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_SPR,ON);	
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG0,OFF);	  //ON
	SCL_SetColRamOffset(SCL_NBG2, 0,ON);
	SCL_SetColRam(SCL_NBG1,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	nBurnSprites = 259;
	nBurnLinescrollSize = 1;
	nSoundBufferPos = 0;

	SS_MAP  = ss_map   = (Uint16 *)(SCL_VDP2_VRAM_B1+0x10000);
	SS_MAP2 = ss_map2  = (Uint16 *)(SCL_VDP2_VRAM_B1+0x14000);
	SS_FONT = ss_font  = (Uint16 *)(SCL_VDP2_VRAM_B1);
	SS_CACHE= cache    = (Uint8  *)(SCL_VDP2_VRAM_A0);

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;
	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_regs->tvmode = 0x8021;

	SS_SET_N0PRIN(6);
	SS_SET_N1PRIN(7);
	SS_SET_N2PRIN(4);
	SS_SET_S0PRIN(5);

	initLayers();
	initColors();
	initSprites(240-1,256-1,0,0,-16,0);
	
	for (unsigned int i = 3; i <nBurnSprites; i++) 
	{
		ss_sprite[i].control   = ( JUMP_NEXT | FUNC_NORMALSP);
		ss_sprite[i].charSize  = 0x210;
		ss_sprite[i].drawMode  = ( ECD_DISABLE | COMPO_REP);
		ss_sprite[i].charAddr  = 0;
		ss_sprite[i].color     = 0;
		ss_sprite[i].ax    	   = -48;
		ss_sprite[i].ay        = -32;
	}
    ss_sprite[nBurnSprites-1].control	= CTRL_END;
    ss_sprite[nBurnSprites-1].link		= 0;        
    ss_sprite[nBurnSprites-1].drawMode	= 0;                
    ss_sprite[nBurnSprites-1].color		= 0;                
    ss_sprite[nBurnSprites-1].charAddr	= 0;                
    ss_sprite[nBurnSprites-1].charSize	= 0;	
	
	ss_reg->n0_move_x = 16<<16;
	ss_reg->n2_move_x = 0;

	PCM_MeStop(pcm);
	Set6PCM();

	drawWindow(0,256,0,0,68);
//	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
INT32 DrvExit()
{
	DrvDoReset();	
	CZetExit2();
	AY8910Exit(1);
	AY8910Exit(0);
	scrollx = scrolly = 0;
	nTigerHeliTileMask = nSndIrqFrame = 0;	

	for(UINT8 i=0;i<6;i++)
	{
		PCM_MeStop(pcm6[i]);
//		memset((Sint8 *)SOUND_BUFFER+(0x4000*(i+1)),0x00,RING_BUF_SIZE*8);
	}
	memset((void *)SOUND_BUFFER,0x00,0x4000*6);

// Deallocate all used memory
	map_offset_lut = map_offset_lut2 = NULL;
	CZ80Context = NULL;
	DrvZ80ROM0 = /*Rom02 = Rom03 =*/ NULL;
//	TigerHeliPaletteROM = NULL;
	DrvZ80RAM0 = RamShared = /*Ram03 =*/ NULL;
	DrvVidRAM = DrvSprRAM = DrvSprBuf = DrvTxtRAM = NULL;

	free(Mem);
	Mem = NULL;

	cleanDATA();
	cleanBSS();

	nSoundBufferPos = 0;

	return 0;
}

void DrvDoReset()
{
	irq_enable = 0;
	sound_nmi_enable = 0;
	
	nStatusIndex = nProtectIndex = 0;
	
	CZetOpen(0);
	CZetReset();
	CZetClose();

	CZetOpen(1);
	CZetReset();
	CZetClose();

//	cleanSprites();
	return;
}

INT32 DrvInit()
{
	DrvInitSaturn();
	UINT8 nWhichGame = 0;

	if (!strcmp(BurnDrvGetTextA(DRV_NAME), "tigerhb1")) 
	{
		nSndIrqFrame = 6;
		nWhichGame = 0;
	}

	if (!strcmp(BurnDrvGetTextA(DRV_NAME), "slapfib1"))
	{
		nSndIrqFrame = 3;
		nWhichGame = 2;
	}

	// Find out how much memory is needed
	Mem = NULL;
	MemIndex();

	if ((Mem = (UINT8*)BurnMalloc(MALLOC_MAX)) == NULL) 
	{
		return 1;
	}
	memset(Mem, 0, MALLOC_MAX);						   	// blank all memory
	MemIndex();										   	// Index the allocated memory
	
	if (DrvLoadRoms(nWhichGame)) return 1;
	
	make_lut();

	
	CZetInit2(2,CZ80Context);

	// Main CPU setup
	CZetOpen(0);

	// Program ROM
	CZetMapMemory(DrvZ80ROM0, 0x0000, 0x7FFF, MAP_ROM);
	// Banked ROM
	CZetMapMemory(DrvZ80ROM0 + 0x8000, 0x8000, 0xBFFF, MAP_ROM);
	// Work RAM
	CZetMapMemory(DrvZ80RAM0, 0xC000, 0xC7FF, MAP_RAM);
	// Shared RAM
	CZetMapMemory(RamShared, 0xC800, 0xCFFF, MAP_RAM);
	// Tile RAM
	CZetMapMemory(DrvVidRAM, 0xD000, 0xDFFF, MAP_ROM);		
	// Sprite RAM
	CZetMapMemory(DrvSprRAM, 0xE000, 0xE7FF, MAP_RAM);
	CZetMapMemory(DrvZ80ROM0 + 0x10c00, 0xec00, 0xeFFF, MAP_ROM);
	// Text RAM
	CZetMapMemory(DrvTxtRAM, 0xF000, 0xFFFF, MAP_ROM);

	if (!strcmp(BurnDrvGetTextA(DRV_NAME), "tigerhb1")) {
		CZetSetReadHandler(tigerhReadCPU0_tigerhb1);
	} else {
		CZetSetReadHandler(tigerhReadCPU0);
	}
	
	CZetSetWriteHandler(tigerhWriteCPU0);
	CZetSetInHandler(tigerhInCPU0);
	CZetSetOutHandler(perfrman_write_port);
	CZetClose();

	// Sound CPU setup
	CZetOpen(1);
	
	CZetSetReadHandler(tigerhReadCPU1);
	CZetSetWriteHandler(tigerhWriteCPU1);
//		CZetSetInHandler(tigerhInCPU1);
//		CZetSetOutHandler(tigerhOutCPU1);		

	// Program ROM
	CZetMapMemory(DrvZ80ROM0+0x12000, 0x0000, 0x1FFF, MAP_ROM);

	// Work RAM
	CZetMapMemory(RamShared, 0xC800, 0xCFFF, MAP_RAM);
	CZetClose();

	AY8910Init(0, 1500000, nBurnSoundRate, &tigerhReadPort0, &tigerhReadPort1, NULL, NULL);
	AY8910Init(1, 1500000, nBurnSoundRate, &tigerhReadPort2, &tigerhReadPort3, NULL, NULL);

	DrvPaletteInit();
	DrvDoReset();

	return 0;
}

inline void draw_sprites()
{
	UINT8 *ram = DrvSprBuf;
	SprSpCmd *ss_spritePtr = &ss_sprite[3];

	for (UINT32 i = 3; i < 259; i++)
	{
			ss_sprite[i].ax    = -48;
			ss_sprite[i].ay    = -32;
	}

	for (UINT32 offs = 0; offs < 0x800; offs += 4)
	{
		if( (ram[offs + 3] - 15) > -7)
		{
			UINT32 attr				= ram[offs + 2];
			UINT32 code				= (ram[offs + 0] | ((attr & 0xc0) << 2));// & nTigerHeliSpriteMask;
			ss_spritePtr->charAddr	= 0x440+(code<<4);
			ss_spritePtr->ay		= 280-(ram[offs + 1] | (attr << 8 & 0x100));
			ss_spritePtr->ax		= ram[offs + 3];
			ss_spritePtr->color		= (attr >> 1 & 0xf)<<4;
			*ss_spritePtr++;			
		}

	}
}

INT32 DrvFrame()
{
	INT32 nCyclesDone[2] = {0,0};

	CZetNewFrame();

	// Compile digital inputs
	{
		DrvInputs[0] = 0xff;
		DrvInputs[1] = 0xff;

		for (UINT32 i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
		}
	}

	SPR_RunSlaveSH((PARA_RTN*)updateSound,&nSoundBufferPos);
	unsigned int vblank = 1;
	
	for (INT32 i = 0; i < nInterleave; i++)
	{
		CZetOpen(0);
		nCyclesDone[0] += CZetRun(((i + 1) * nCycleSegment0) - nCyclesDone[0]);

//		if (i == 15) {
		if (i == 3) {
			vblank = 0;
		}

//		if (i == 255) {
			if (i == 63) {
			if (irq_enable) CZetSetIRQLine(0, CZET_IRQSTATUS_AUTO);
			vblank = 1;
			memcpyl(DrvSprBuf, DrvSprRAM, 0x0800);
		}
		CZetClose();

		CZetOpen(1);
		nCyclesDone[1] += CZetRun(((i + 1) * nCycleSegment1) - nCyclesDone[1]);

		if (((i % (nInterleave / nSndIrqFrame)) == ((nInterleave / nSndIrqFrame) - 1)) && sound_nmi_enable) {
			CZetNmi();
		}
		CZetClose();

	}
	ss_reg->n2_move_y = -scrollx-283;
	SPR_WaitEndSlaveSH();
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void rotate_tile16x16(unsigned int size, unsigned char *target)
{
	unsigned int i,j,k; //,l=0;
	unsigned char temp[16][16];
	unsigned char rot[16][16];

	for (k=0;k<size;k++)
	{
		for(i=0;i<16;i++)
			for(j=0;j<8;j++)
			{
				temp[i][j<<1]=target[(i*8)+j]>>4;
				temp[i][(j<<1)+1]=target[(i*8)+j]&0x0f;
			}

		memset(target,0,128);
		
		for(i=0;i<16;i++)
			for(j=0;j<16;j++)
			{
//				if(flip)
				 rot[15-i][j]= temp[j][i] ;
//				else
//				 rot[i][15-j]= temp[j][i] ;
			}

		for(i=0;i<16;i++)
			for(j=0;j<8;j++)
					target[(i*8)+j]    = (rot[i][j*2]<<4)|(rot[i][(j*2)+1]&0xf);
		target+=128;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void updateSound(unsigned int *nSoundBufferPos)
{
	unsigned short *nSoundBuffer1 = (unsigned short *)0x25a24000+nSoundBufferPos[0];
	
	AY8910UpdateDirect(0, &nSoundBuffer1[0], nBurnSoundLen);
	AY8910UpdateDirect(1, &nSoundBuffer1[0x6000], nBurnSoundLen);
	nSoundBufferPos[0]+=nBurnSoundLen;
	
	if(nSoundBufferPos[0]>=RING_BUF_SIZE>>1)
	{
		for (unsigned int i=0;i<6;i++)
		{
			PCM_NotifyWriteSize(pcm6[i], nSoundBufferPos[0]);
			PCM_Task(pcm6[i]); // bon emplacement
		}
		nSoundBufferPos[0]=0;
	}
	draw_sprites();
}
//-------------------------------------------------------------------------------------------------------------------------------------
void make_lut(void)
{
	unsigned int i;
	int sx, sy;

	for (i = 0; i < 0x800; i++)
	{
		sx = (35-((i) & 0x3f))<<6;//% 32;
		sy = ((i >> 6)) & 0x3f;
		
		if (sx>=0 && sy>=0)
			map_offset_lut2[i] = ((sx) | sy)<<1;

		sx = (63-((i) & 0x3f))<<6;//% 32;
		sy = ((i >> 6)) & 0x3f;
		
		if (sx>=0 && sy>=0)
			map_offset_lut[i] = ((sx) | sy)<<1;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static PcmHn createHandle(PcmCreatePara *para)
{
	PcmHn pcm;

	pcm = PCM_CreateMemHandle(para);
	if (pcm == NULL) {
		return NULL;
	}
	return pcm;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void Set6PCM()
{
	PcmCreatePara	para[6];
	PcmInfo 		info[6];
	PcmStatus	*st;
	static PcmWork g_movie_work[6];

	for (UINT32 i=0; i<6; i++)
	{
		PCM_PARA_WORK(&para[i]) = (struct PcmWork *)&g_movie_work[i];
		PCM_PARA_RING_ADDR(&para[i]) = (Sint8 *)PCM_ADDR+0x40000+(0x4000*(i+1));
		PCM_PARA_RING_SIZE(&para[i]) = RING_BUF_SIZE;
		PCM_PARA_PCM_ADDR(&para[i]) = PCM_ADDR+(0x4000*(i+1));
		PCM_PARA_PCM_SIZE(&para[i]) = PCM_SIZE;

		memset((Sint8 *)SOUND_BUFFER,0,SOUNDRATE*16);
		st = &g_movie_work[i].status;
		st->need_ci = PCM_ON;
	 
		PCM_INFO_FILE_TYPE(&info[i]) = PCM_FILE_TYPE_NO_HEADER;			
		PCM_INFO_DATA_TYPE(&info[i])=PCM_DATA_TYPE_RLRLRL;//PCM_DATA_TYPE_LRLRLR;
		PCM_INFO_FILE_SIZE(&info[i]) = RING_BUF_SIZE;//SOUNDRATE*2;//0x4000;//214896;
		PCM_INFO_CHANNEL(&info[i]) = 0x01;
		PCM_INFO_SAMPLING_BIT(&info[i]) = 16;

		PCM_INFO_SAMPLING_RATE(&info[i])	= SOUNDRATE;//30720L;//44100L;
		PCM_INFO_SAMPLE_FILE(&info[i]) = RING_BUF_SIZE;//SOUNDRATE*2;//30720L;//214896;
		pcm6[i] = createHandle(&para[i]);

		PCM_SetPcmStreamNo(pcm6[i], i);

		PCM_SetInfo(pcm6[i], &info[i]);
		PCM_ChangePcmPara(pcm6[i]);

		PCM_MeSetLoop(pcm6[i], 0);//SOUNDRATE*120);
		PCM_Start(pcm6[i]);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------

