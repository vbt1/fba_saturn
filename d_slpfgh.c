// Tiger Heli, Get Star / Guardian, & Slap Fight

#include "d_slpfgh.h"

#define nVBlankCycles 248 * 4000000 / 60 / 262
#define nInterleave 12

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvSlapBtJP = {
		"slapfib1", "slpfgh",
		"Slap Fight (bootleg set 1)",
		slapbtjpRomInfo, slapbtjpRomName, tigerhInputInfo, slapfighDIPInfo,
		tigerhInit, tigerhExit, tigerhFrame, NULL
	};

	struct BurnDriver nBurnDrvTigerHB1 = {
		"tigerhb1", "slpfgh", 			
		"Tiger Heli (bootleg, set 1)",
		tigerhb1RomInfo, tigerhb1RomName, tigerhInputInfo, tigerhDIPInfo,
		tigerhInit, tigerhExit, tigerhFrame, NULL
	};

	if (strcmp(nBurnDrvSlapBtJP.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvSlapBtJP,sizeof(struct BurnDriver));

	if (strcmp(nBurnDrvTigerHB1.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvTigerHB1,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/  INT32 MemIndex()
{
	UINT8* Next; Next = Mem;
	Rom01				= Next; Next += 0x012000;		// Z80 main program
	Rom02				= Next; Next += 0x002000;		// Z80 sound program
//	Rom03				= Next; Next += 0x000800;		// m68705 mcu program
	TigerHeliTextROM	= (UINT8 *)(SS_CACHE); //Next; Next += 0x010000;
 	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	TigerHeliSpriteROM	= &ss_vram[0x2200]; //Next; Next += 3 * 16 * 16 * 256; // 3 banks, 16x16 tiles, 256 tiles (sprites)
//		= 0x00240000;//Next; Next += 0x040000;
	TigerHeliTileROM	= (UINT8 *)(SS_CACHE+0x8000); //0x00280000;//Next; Next += 0x040000;
	CZ80Context			= Next; Next += (0x1080*2);
	RamStart				= Next;
	Ram01					= Next; Next += 0x000800;		// Z80 main work RAM
	RamShared			= Next; Next += 0x000800;		// Shared RAM
	TigerHeliTextRAM	= Next; Next += 0x001000;
	TigerHeliSpriteRAM= Next; Next += 0x000800;
	TigerHeliSpriteBuf	= Next; Next += 0x000800;
	TigerHeliTileRAM	= Next; Next += 0x001000;
//	Ram03					= Next; Next += 0x000080;
	RamEnd				= Next;
//	pFMBuffer				= (INT16*)Next; Next += nBurnSoundLen * 6 * sizeof(INT16);
	TigerHeliPaletteROM	= Next; Next += 0x000300;
	TigerHeliPalette		= (UINT32*)Next; Next += 0x000100 * sizeof(UINT32);
	map_offset_lut		= Next; Next += 0x800 * sizeof(UINT16);
	map_offset_lut2		= Next; Next += 0x800 * sizeof(UINT16);

	MemEnd				= Next;
	return 0;
}

// ---------------------------------------------------------------------------
//	Graphics

/*static*/  void TigerHeliPaletteInit()
{
	int delta=0;
	for (INT32 i = 0; i < 0x0100; i++) {
		INT32 r, g, b;

		r = TigerHeliPaletteROM[i + 0x0000];	  // Red
		r |= r << 4;
		g = TigerHeliPaletteROM[i + 0x0100];	  // Green
		g |= g << 4;
		b = TigerHeliPaletteROM[i + 0x0200];	  // Blue
		b |= b << 4;

		colBgAddr[delta] = colBgAddr2[i] = TigerHeliPalette[i] = BurnHighCol(r, g, b, 0);
		delta++; if ((delta & 3) == 0) delta += 12;
	}

	return;
}

// ---------------------------------------------------------------------------
UINT8 __fastcall tigerhReadCPU0(UINT16 a)
{
	if (a >= 0xc800 && a <= 0xcfff) {
		if (CZetGetPC(-1) == 0x6d34) return 0xff;
		return RamShared[a - 0xc800];
	}
	
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
	if (a >= 0xc800 && a <= 0xcfff) {
		if (CZetGetPC(-1) == 0x6d34) return 0xff;
		return RamShared[a - 0xc800];
	}
	
	switch (a) {
		case 0xE803: {
			return 0x83;
		}
	}
	
	return 0;
}


void __fastcall tigerhWriteCPU0(UINT16 a, UINT8 d)
{
	if(a>= 0xD000 && a <= 0xDFFF)
	{
		a-= 0xD000;
		if(TigerHeliTileRAM[a]!=d)
		{
			TigerHeliTileRAM[a] = d;

			UINT32 attr;
			if(a>=0x800)
			{
				a &=0x7ff;
				attr   = TigerHeliTileRAM[a & 0x7ff] + (d * 0x100);
			}
			else
			{
				attr   = d + (TigerHeliTileRAM[0x800 + a] * 0x100);
			}
			UINT32 code = (attr & 0x0fff) & nTigerHeliTileMask;
			UINT32 color = (attr & 0xf000) >> 12;

			UINT32 x		  = map_offset_lut[a];
			ss_map2[x]     = color;
			ss_map2[x+1] = code+0x1000;
		}
		return;
	}

	if(a>= 0xF000 && a <= 0xFFFF)
	{
		a-= 0xF000;
		if(TigerHeliTextRAM[a]!=d)
		{
			TigerHeliTextRAM[a] = d;
			UINT32 attr;
			if(a>=0x800)
			{
				a &=0x7ff;
				attr   = TigerHeliTextRAM[a & 0x7ff] + (d * 0x100);
			}
			else
			{
				attr   = d + (TigerHeliTextRAM[0x800 + a] * 0x100);
			}
			UINT32 code =  attr & 0x03ff;
			UINT32 color = (attr & 0xfc00) >> 10;

			UINT32 x		= map_offset_lut2[a];
			ss_map[x]		= color & 0x3f;
			ss_map[x+1] = code;
		}
		 return;
	}

	switch (a) {
		case 0xE800:
			nTigerHeliTileXPosLo = d;
			break;
		case 0xE801:
			nTigerHeliTileXPosHi = d;
			break;
		case 0xE802:
			nTigerHeliTileYPosLo = d;
			break;

		case 0xe803: 

			break;

	}
}

void __fastcall tigerhWriteCPU0_slapbtuk(UINT16 a, UINT8 d)
{
	switch (a) {
		case 0xE800:
			nTigerHeliTileXPosHi = d;
			break;
		case 0xE802:
			nTigerHeliTileYPosLo = d;
			break;
		case 0xE803:
			nTigerHeliTileXPosLo = d;
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

UINT8 __fastcall tigerhInCPU0_gtstarba(UINT16 a)
{
	a &= 0xFF;

	switch (a) {
		case 0x00: {
			if (CZetGetPC(-1) == 0x6d1e)	return 0;
			if (CZetGetPC(-1) == 0x6d24)	return 6;
			if (CZetGetPC(-1) == 0x6d2c)	return 2;
			if (CZetGetPC(-1) == 0x6d34)	return 4;
			return 0;
		}
	}

	return 0;
}

void __fastcall tigerhOutCPU0(UINT16 a, UINT8  d)
{
	a &= 0xFF;

	switch (a) {
		case 0x00:					// Assert reset line on sound CPU

			if (bSoundCPUEnable) {
				CZetClose();
				CZetOpen(1);
				CZetReset();
				CZetClose();
				CZetOpen(0);

				bSoundCPUEnable = false;
			}

			break;
		case 0x01:					// Release reset line on sound CPU
			bSoundCPUEnable = true;
			break;

		case 0x02:
		case 0x03:
			nFlipscreen = ~a & 1;
			break;

		case 0x06:					// Disable interrupts

			bInterruptEnable = false;
			CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);
			break;
		case 0x07:					// Enable interrupts

			bInterruptEnable = true;
			break;

		case 0x08:

			// ROM bank 0 selected
			CZetMapArea(0x8000, 0xBFFF, 0, Rom01 + 0x8000);
			CZetMapArea(0x8000, 0xBFFF, 2, Rom01 + 0x8000);
			break;
		case 0x09:

			// ROM bank 1 selected
			CZetMapArea(0x8000, 0xBFFF, 0, Rom01 + 0xC000);
			CZetMapArea(0x8000, 0xBFFF, 2, Rom01 + 0xC000);
			break;

		case 0x0c:
		case 0x0d:
			nPalettebank = a & 1;
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
			bSoundNMIEnable = true;
			break;
	}
}

UINT8 __fastcall tigerhInCPU1(UINT16 a)
{
	return 0;
}

void __fastcall tigerhOutCPU1(UINT16 a, UINT8 d)
{
//	bprintf(PRINT_NORMAL, _T("Attempt by CPU1 to write port %02X -> %02X.\n"), a, d);
}

/*static*/  UINT8 tigerhReadPort0(UINT32 data)
{
	return ~tigerhInput[0];
}
/*static*/  UINT8 tigerhReadPort1(UINT32 data)
{
	return ~tigerhInput[1];
}
/*static*/  UINT8 tigerhReadPort2(UINT32 data)
{
	return ~tigerhInput[2];
}
/*static*/  UINT8 tigerhReadPort3(UINT32 data)
{
	return ~tigerhInput[3];
}
// ---------------------------------------------------------------------------

/*static*/  INT32 tigerhLoadROMs()
{
	INT32 nRomOffset = 0;
	if (!strcmp(BurnDrvGetTextA(DRV_NAME), "slapfighb2")) nRomOffset = 1;
	if (!strcmp(BurnDrvGetTextA(DRV_NAME), "slapfighb3")) nRomOffset = 2;

	// Z80 main program
	switch (nWhichGame) {
		case 0:											// Tiger Heli
			if (BurnLoadRom(Rom01 + 0x0000, 0, 1)) {
				return 1;
			}
			if (BurnLoadRom(Rom01 + 0x4000, 1, 1)) {
				return 1;
			}
			if (BurnLoadRom(Rom01 + 0x8000, 2, 1)) {
				return 1;
			}
			break;
		case 2:	{										// Slap Fight
			if (!strcmp(BurnDrvGetTextA(DRV_NAME), "slapfighb2")) {
				if (BurnLoadRom(Rom01 + 0x0000, 0, 1)) {
					return 1;
				}
				if (BurnLoadRom(Rom01 + 0x4000, 1, 1)) {
					return 1;
				}
				if (BurnLoadRom(Rom01 + 0x8000, 2, 1)) {
					return 1;
				}
				break;
			} else {
				if (!strcmp(BurnDrvGetTextA(DRV_NAME), "slapfighb3")) {
					if (BurnLoadRom(Rom01 + 0x0000, 0, 1)) {
						return 1;
					}
					if (BurnLoadRom(Rom01 + 0x4000, 1, 1)) {
						return 1;
					}
					if (BurnLoadRom(Rom01 + 0x10000, 2, 1)) {
						return 1;
					}
					if (BurnLoadRom(Rom01 + 0x8000, 3, 1)) {
						return 1;
					}
					break;
				} else {
					if (BurnLoadRom(Rom01 + 0x0000, 0, 1)) {
						return 1;
					}
					if (BurnLoadRom(Rom01 + 0x8000, 1, 1)) {
						return 1;
					}
					break;
				}
			}
		}
	}

	// Sprites
	{
		INT32 nRet = 0, nBaseROM = 0;
		switch (nWhichGame) {
			case 0:										// Tiger Heli
				nBaseROM = 3;
				break;
			case 2:										// Slap Fight
				nBaseROM = 2 + nRomOffset;
				break;
		}

		INT32 nSize;

		{
			struct BurnRomInfo ri;

			ri.nType = 0;
			ri.nLen = 0;

			BurnDrvGetRomInfo(&ri, nBaseROM);

			nSize = ri.nLen;
		}

//		UINT8* pTemp = (UINT8*)size (nSize * 4);
		UINT8* pTemp = (UINT8*)0x00240000;
	//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"loadrom 4",4,80);

		for (INT32 i = 0; i < 4; i++) {
			nRet |= BurnLoadRom(pTemp + nSize * i, nBaseROM + i, 1);
		}
	//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"loadrom 5",4,80);

		for (INT32 i = 0; i < nSize; i++) {
			for (INT32 j = 0; j < 8; j++) {
				TigerHeliSpriteROM[(i << 3) + j]  = ((pTemp[i + nSize * 0] >> (7 - j)) & 1) << 3;
				TigerHeliSpriteROM[(i << 3) + j] |= ((pTemp[i + nSize * 1] >> (7 - j)) & 1) << 2;
				TigerHeliSpriteROM[(i << 3) + j] |= ((pTemp[i + nSize * 2] >> (7 - j)) & 1) << 1;
				TigerHeliSpriteROM[(i << 3) + j] |= ((pTemp[i + nSize * 3] >> (7 - j)) & 1) << 0;
			}
		}

		for (INT32 i = 0; i < nSize*8; i+=2) 
		{
			TigerHeliSpriteROM[(i >>1)]  = TigerHeliSpriteROM[i] << 4 | TigerHeliSpriteROM[i+1];
		}

//		BurnFree(pTemp);
		pTemp = NULL;

		nTigerHeliSpriteMask = (nSize >> 5) - 1;

		if (nRet) {
			return 1;
		}
	}

	// Text layer
	{
		INT32 nBaseROM = 0;
		switch (nWhichGame) {
			case 0:										// Tiger Heli
				nBaseROM = 7;
				break;
			case 1:										// Get Star
				nBaseROM = 7;
				break;
			case 2:										// Slap Fight
				nBaseROM = 6 + nRomOffset;
				break;
		}

//		UINT8* pTemp = (UINT8*) size (0x4000);
		UINT8* pTemp = (UINT8*)0x00240000;
	//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"loadrom 6",4,80);

		if (BurnLoadRom(pTemp + 0x0000, nBaseROM + 0, 1)) {
			return 1;
		}
	//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"loadrom 7",4,80);

		if (BurnLoadRom(pTemp + 0x2000, nBaseROM + 1, 1)) {
			return 1;
		}

		for (INT32 i = 0; i < 0x02000; i++) {
			for (INT32 j = 0; j < 8; j++) {
				TigerHeliTextROM[(i << 3) + j]  = ((pTemp[i + 0x0000] >> (7 - j)) & 1) << 1;
				TigerHeliTextROM[(i << 3) + j] |= ((pTemp[i + 0x2000] >> (7 - j)) & 1) << 0;
			}
		}

		for (INT32 i = 0; i < 0x10000; i+=2) 
		{
			TigerHeliTextROM[(i >>1)]  = TigerHeliTextROM[i] << 4 | TigerHeliTextROM[i+1];
		}
		pTemp = NULL;
	}

	// Tile layer
	{
		INT32 nRet = 0, nBaseROM = 0;
		switch (nWhichGame) {
			case 0:										// Tiger Heli
				nBaseROM = 9;
				break;
			case 1:										// Get Star
				nBaseROM = 9;
				break;
			case 2:										// Slap Fight
				nBaseROM = 8 + nRomOffset;
				break;
		}

		INT32 nSize;

		{
			struct BurnRomInfo ri;

			ri.nType = 0;
			ri.nLen = 0;

			BurnDrvGetRomInfo(&ri, nBaseROM);

			nSize = ri.nLen;
		}

//		UINT8* pTemp = (UINT8*)size (nSize * 4);
		UINT8* pTemp = (UINT8*)0x00240000;
	//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"loadrom 8",4,80);

		for (INT32 i = 0; i < 4; i++) {
			nRet |= BurnLoadRom(pTemp + nSize * i, nBaseROM + i, 1);
		}

  	//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"loadrom 9",4,80);

		for (INT32 i = 0; i < nSize; i++) {
			for (INT32 j = 0; j < 8; j++) {
				TigerHeliTileROM[(i << 3) + j]  = ((pTemp[i + nSize * 0] >> (7 - j)) & 1) << 3;
				TigerHeliTileROM[(i << 3) + j] |= ((pTemp[i + nSize * 1] >> (7 - j)) & 1) << 2;
				TigerHeliTileROM[(i << 3) + j] |= ((pTemp[i + nSize * 2] >> (7 - j)) & 1) << 1;
				TigerHeliTileROM[(i << 3) + j] |= ((pTemp[i + nSize * 3] >> (7 - j)) & 1) << 0;
			}
		}

		for (INT32 i = 0; i < nSize*8; i+=2) 
		{
			TigerHeliTileROM[(i >>1)]  = TigerHeliTileROM[i] << 4 | TigerHeliTileROM[i+1];
		}

//		BurnFree(pTemp);
		pTemp = NULL;

		nTigerHeliTileMask = (nSize >> 3) - 1;

		if (nRet) {
			return 1;
		}
	}

	// Colour PROMs
	{
		INT32 nBaseROM = 0;
		switch (nWhichGame) {
			case 0:										// Tiger Heli
				nBaseROM = 13;
				break;
			case 2:										// Slap Fight
				nBaseROM = 12 + nRomOffset;
				break;
		}
   	//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"loadrom10",4,80);

		if (BurnLoadRom(TigerHeliPaletteROM + 0x0000, nBaseROM + 0, 1)) {
			return 1;
		}
  	//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"loadrom11",4,80);

		if (BurnLoadRom(TigerHeliPaletteROM + 0x0100, nBaseROM + 1, 1)) {
			return 1;
		}
		if (BurnLoadRom(TigerHeliPaletteROM + 0x0200, nBaseROM + 2, 1)) {
			return 1;
		}
	}

	// Z80 program
	{
		INT32 nBaseROM = 0;
		switch (nWhichGame) {
			case 0:										// Tiger Heli
				nBaseROM = 16;
				break;
			case 2:										// Slap Fight
				nBaseROM = 15 + nRomOffset;
				break;
		}
  	//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"loadrom12",4,80);
		if (BurnLoadRom(Rom02, nBaseROM, 1)) {
			return 1;
		}
	}
  	//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"loadrom13",4,80);

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/  void initLayers()
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
/*static*/ void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_SPR,ON);	
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG0,OFF);	  //ON
	SCL_SetColRamOffset(SCL_NBG2, 0,ON);
	SCL_SetColRam(SCL_NBG1,8,8,palette);
//	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);//OFF);
//	(Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);

}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);

	nBurnSprites = 259; //256;
	nBurnLinescrollSize = 0;
	nSoundBufferPos = 0;

	SS_MAP  = ss_map   =(Uint16 *)(SCL_VDP2_VRAM_B1+0x10000);
	SS_MAP2 = ss_map2  = (Uint16 *)(SCL_VDP2_VRAM_B1+0x14000);
	SS_FONT = ss_font  = (Uint16 *)(SCL_VDP2_VRAM_B1);
	SS_CACHE= cache    =(Uint8  *)(SCL_VDP2_VRAM_A0);

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
//	ss_scl			= (Fixed32 *)SS_SCL;
//	ss_scl1			= (Fixed32 *)SS_SCL1;

	ss_regs->tvmode = 0x8021;
// 	SCL_SetWindow(SCL_W0,SCL_NBG0,SCL_NBG1,SCL_NBG1,16,0,240,256);
// 	SCL_SetWindow(SCL_W1,SCL_NBG1,SCL_NBG0,SCL_NBG0,16,0,240,256);

	SS_SET_N0PRIN(6);
	SS_SET_N1PRIN(7);
	SS_SET_N2PRIN(4);
	SS_SET_S0PRIN(5);

	initLayers();
	initColors();
	initSprites(240-1,256-1,-7,0,0,0);
	
	for (unsigned int i = 3; i <nBurnSprites; i++) 
	{
		ss_sprite[i].control   = ( JUMP_NEXT | FUNC_NORMALSP);
		ss_sprite[i].charSize  = 0x210;
		ss_sprite[i].drawMode  = ( ECD_DISABLE | COMPO_REP);
		ss_sprite[i].ax    = -48;
		ss_sprite[i].ay    =  -32;
	}
	ss_reg->n0_move_x = 16<<16;
	ss_reg->n2_move_x = 4;

	Set6PCM();

	drawWindow(0,256,0,2,68);
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/  INT32 tigerhExit()
{
	SPR_InitSlaveSH();
	CZetExit2();
	AY8910Exit(1);
	AY8910Exit(0);

	for(int i=0;i<6;i++)
	{
		PCM_MeStop(pcm6[i]);
		memset(SOUND_BUFFER+(0x4000*(i+1)),0x00,RING_BUF_SIZE*8);
	}

// Deallocate all used memory
	map_offset_lut = map_offset_lut2 = NULL;
	CZ80Context = MemEnd = RamStart = RamEnd = NULL;
	Rom01 = Rom02 = /*Rom03 =*/ NULL;
	TigerHeliTileROM = TigerHeliSpriteROM = TigerHeliTextROM = TigerHeliPaletteROM = NULL;
	Ram01 = RamShared = /*Ram03 =*/ NULL;
	TigerHeliTileRAM = TigerHeliSpriteRAM = TigerHeliSpriteBuf = TigerHeliTextRAM = NULL;
	TigerHeliPalette = NULL;

	free(Mem);
	Mem = NULL;

	nWhichGame = 0;
	bInterruptEnable = bSoundNMIEnable = bSoundCPUEnable = 0;
	nStatusIndex = nProtectIndex = 0;

	nTigerHeliTileXPosLo = nTigerHeliTileXPosHi = nTigerHeliTileYPosLo = 0;
	nTigerHeliTileMask = nTigerHeliSpriteMask = 0;	
	nStatusIndex = nProtectIndex = nPalettebank = nFlipscreen = 0;

	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
	nSoundBufferPos = 0;

	return 0;
}

/*static*/  void tigerhDoReset()
{
	bInterruptEnable = false;
	bSoundNMIEnable = false;
	bSoundCPUEnable = true;
	
	nStatusIndex = nProtectIndex = nPalettebank = nFlipscreen = 0;

	CZetOpen(0);
	CZetReset();
	CZetClose();

	CZetOpen(1);
	CZetReset();
	CZetClose();

	return;
}

/*static*/  INT32 tigerhInit()
{
	DrvInitSaturn();
	INT32 nLen;
	nWhichGame = -1;

	if (strcmp(BurnDrvGetTextA(DRV_NAME), "tigerh") == 0 || strcmp(BurnDrvGetTextA(DRV_NAME), "tigerhj") == 0 || strcmp(BurnDrvGetTextA(DRV_NAME), "tigerhb1") == 0 || strcmp(BurnDrvGetTextA(DRV_NAME), "tigerhb2") == 0 || strcmp(BurnDrvGetTextA(DRV_NAME), "tigerhb3") == 0) {
		nWhichGame = 0;
	}

	if (strcmp(BurnDrvGetTextA(DRV_NAME), "alcon") == 0 || strcmp(BurnDrvGetTextA(DRV_NAME), "slapfigh") == 0 || strcmp(BurnDrvGetTextA(DRV_NAME), "slapfib1") == 0 || strcmp(BurnDrvGetTextA(DRV_NAME), "slapfighb2") == 0 || strcmp(BurnDrvGetTextA(DRV_NAME), "slapfighb3") == 0) {
		nWhichGame = 2;
	}

	// Find out how much memory is needed
	Mem = NULL;
	MemIndex();
	nLen = MemEnd - (UINT8*)0;
//		sprintf(tmp0,"before malloc      ");

	if ((Mem = (UINT8*)BurnMalloc(nLen)) == NULL) 
	{
//		sprintf(tmp0,"malloc failed      ");
		return 1;
	}
	memset(Mem, 0, nLen);										   	// blank all memory
	MemIndex();													   	// Index the allocated memory
//		sprintf(tmp0,"malloc done      ");
	make_lut();

	// Load the roms into memory
	if (tigerhLoadROMs()) 
	{
//		sprintf(tmp0,"loadrom failed      ");
		return 1;
	}

	rotate_tile(0x400,1,TigerHeliTextROM);
	rotate_tile(0x800,1,TigerHeliTileROM);
	rotate_tile16x16(nTigerHeliSpriteMask+1,1,TigerHeliSpriteROM);

//	sprintf(tmp0,"loadrom done      ");
	
	if (!strcmp(BurnDrvGetTextA(DRV_NAME), "getstarb1")) Rom01[0x6d56] = 0xc3;

	{
//		CZetInit(2);
		CZetInit2(2,CZ80Context);

		// Main CPU setup
		CZetOpen(0);

		// Program ROM
		CZetMapArea(0x0000, 0x7FFF, 0, Rom01);
		CZetMapArea(0x0000, 0x7FFF, 2, Rom01);
		// Banked ROM
		CZetMapArea(0x8000, 0xBFFF, 0, Rom01 + 0x8000);
		CZetMapArea(0x8000, 0xBFFF, 2, Rom01 + 0x8000);

		// Work RAM
		CZetMapArea(0xC000, 0xC7FF, 0, Ram01);
		CZetMapArea(0xC000, 0xC7FF, 1, Ram01);
		CZetMapArea(0xC000, 0xC7FF, 2, Ram01);

		// Shared RAM
//		if (strcmp(BurnDrvGetTextA(DRV_NAME), "getstarb1")) {
//			CZetMapArea(0xC800, 0xCFFF, 0, RamShared);
//		}
		CZetMapArea(0xC800, 0xCFFF, 1, RamShared);
		CZetMapArea(0xC800, 0xCFFF, 2, RamShared);
		
		// Tile RAM
		CZetMapArea(0xD000, 0xDFFF, 0, TigerHeliTileRAM);
//		CZetMapArea(0xD000, 0xDFFF, 1, TigerHeliTileRAM);
		CZetMapArea(0xD000, 0xDFFF, 2, TigerHeliTileRAM);
		// Sprite RAM
		CZetMapArea(0xE000, 0xE7FF, 0, TigerHeliSpriteRAM);
		CZetMapArea(0xE000, 0xE7FF, 1, TigerHeliSpriteRAM);
		CZetMapArea(0xE000, 0xE7FF, 2, TigerHeliSpriteRAM);
		
		if (!strcmp(BurnDrvGetTextA(DRV_NAME), "slapfighb2") || !strcmp(BurnDrvGetTextA(DRV_NAME), "slapfighb3")) {
			CZetMapArea(0xec00, 0xeFFF, 0, Rom01 + 0x10c00);
			CZetMapArea(0xec00, 0xeFFF, 2, Rom01 + 0x10c00);
		}
		
		// Text RAM
		CZetMapArea(0xF000, 0xFFFF, 0, TigerHeliTextRAM);
//		CZetMapArea(0xF000, 0xFFFF, 1, TigerHeliTextRAM);
		CZetMapArea(0xF000, 0xFFFF, 2, TigerHeliTextRAM);


		if (!strcmp(BurnDrvGetTextA(DRV_NAME), "tigerhb1")) {
			CZetSetReadHandler(tigerhReadCPU0_tigerhb1);
		} else {
			CZetSetReadHandler(tigerhReadCPU0);
		}
		
		if (!strcmp(BurnDrvGetTextA(DRV_NAME), "slapfighb2") || !strcmp(BurnDrvGetTextA(DRV_NAME), "slapfighb3")) {
			CZetSetWriteHandler(tigerhWriteCPU0_slapbtuk);
		} else {
			CZetSetWriteHandler(tigerhWriteCPU0);
		}
		
//		if (!strcmp(BurnDrvGetTextA(DRV_NAME), "getstarb1")) {
//			CZetSetInHandler(tigerhInCPU0_gtstarba);
//		} else {
			CZetSetInHandler(tigerhInCPU0);
//		}
		
		CZetSetOutHandler(tigerhOutCPU0);

		CZetClose();

//		CZetInit(1);

		// Sound CPU setup
		CZetOpen(1);

		// Program ROM
		CZetMapArea(0x0000, 0x1FFF, 0, Rom02);
		CZetMapArea(0x0000, 0x1FFF, 2, Rom02);

		// Work RAM
		CZetMapArea(0xC800, 0xCFFF, 0, RamShared);
		CZetMapArea(0xC800, 0xCFFF, 1, RamShared);
		CZetMapArea(0xC800, 0xCFFF, 2, RamShared);


		CZetSetReadHandler(tigerhReadCPU1);
		CZetSetWriteHandler(tigerhWriteCPU1);
		CZetSetInHandler(tigerhInCPU1);
		CZetSetOutHandler(tigerhOutCPU1);

		CZetClose();
	}

	AY8910Init(0, 1500000, nBurnSoundRate, &tigerhReadPort0, &tigerhReadPort1, NULL, NULL);
	AY8910Init(1, 1500000, nBurnSoundRate, &tigerhReadPort2, &tigerhReadPort3, NULL, NULL);

	TigerHeliPaletteInit();
	tigerhDoReset();

	return 0;
}

/*static*/  inline void TigerHeliBufferSprites()
{
	memcpyl(TigerHeliSpriteBuf, TigerHeliSpriteRAM, 0x0800);
}

/*static*/  void draw_sprites()
{
	UINT8 *ram = TigerHeliSpriteBuf;
	SprSpCmd *ss_spritePtr = &ss_sprite[3];

	for (INT32 i = 3; i < 259; i++)
	{
			ss_sprite[i].ax    = -48;
			ss_sprite[i].ay    =  -32;
	}

	for (INT32 offs = 0; offs < 0x800; offs += 4)
	{
		if( (ram[offs + 3] - 15) > -7)
		{
			UINT32 attr					=  ram[offs + 2];
			UINT32 code					= (ram[offs + 0] | ((attr & 0xc0) << 2)) & nTigerHeliSpriteMask;
			ss_spritePtr->charAddr	= 0x440+(code<<4);
			ss_spritePtr->ay			= 280-(ram[offs + 1] | (attr << 8 & 0x100));// - (13);
			ss_spritePtr->ax			=  ram[offs + 3] - 11;
			ss_spritePtr->color		=  (attr >> 1 & 0xf)<<4;
			ss_spritePtr++;
		}
	}
}

/*static*/  inline INT32 CheckSleep(INT32 duration)
{
	return 0;
}

/*static*/  INT32 tigerhFrame()
{
	UINT32 nCyclesTotal[3] = {4000000 / 60,2000000 / 60};
	INT32 nCyclesDone[3] = {0,0};
//	UINT8 *tmp0 = (UINT8*)0x00200000;

	CZetNewFrame();

	// Compile digital inputs
	tigerhInput[0] = 0x00;
	tigerhInput[1] = 0x00;
	for (INT32 i = 0; i < 8; i++) {
		tigerhInput[0] |= (tigerhInpJoy1[i] & 1) << i;
		if (nWhichGame == 0 && i < 4) {
			tigerhInput[1] |= (tigerhInpMisc[i] & 1) << (i ^ 1);
		} else {
			tigerhInput[1] |= (tigerhInpMisc[i] & 1) << i;
		}
	}

	if ((tigerhInput[0] & 0x03) == 0x03) {
		tigerhInput[0] &= ~0x03;
	}
	if ((tigerhInput[0] & 0x0C) == 0x0C) {
		tigerhInput[0] &= ~0x0C;
	}
	if ((tigerhInput[0] & 0x30) == 0x30) {
		tigerhInput[0] &= ~0x30;
	}
	if ((tigerhInput[0] & 0xC0) == 0xC0) {
		tigerhInput[0] &= ~0xC0;
	}

	UINT32 nSoundNMIMask = 0;

	switch (nWhichGame) {
		case 0:
			nSoundNMIMask = 1;
			break;
		case 1:
			nSoundNMIMask = 3;
			break;
		case 2:
			nSoundNMIMask = 3;
			break;
/*
		case 9:
			nSoundNMIMask = 3; //??
			break;			*/
	}

	bool bVBlank = false;
 	SPR_RunSlaveSH((PARA_RTN*)updateSound, NULL);

	for (UINT32 i = 0; i < nInterleave; i++) 
	{
    	UINT32 nCurrentCPU;
		INT32 nNext, nCyclesSegment;

		nCurrentCPU = 0;
		CZetOpen(nCurrentCPU);

		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;

		if (nNext > nVBlankCycles && !bVBlank) 
		{
			nCyclesDone[nCurrentCPU] += CZetRun(nNext - nVBlankCycles);
			TigerHeliBufferSprites();
			bVBlank = true;

			if (bInterruptEnable) 
			{
				CZetSetIRQLine(0xff, CZET_IRQSTATUS_AUTO);
			}
		}

		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		if (bVBlank || (!CheckSleep(nCurrentCPU))) 
		{					// See if this CPU is busywaiting
			nCyclesDone[nCurrentCPU] += CZetRun(nCyclesSegment);
		} 
		else 
		{
			nCyclesDone[nCurrentCPU] += nCyclesSegment;
		}

		CZetClose();

		nCurrentCPU = 1;
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];

		if (bSoundCPUEnable) 
		{
			CZetOpen(nCurrentCPU);

			if ((i & nSoundNMIMask) == 0) 
			{
				if (bSoundNMIEnable) 
				{
					CZetNmi();
				}
			}
			nCyclesDone[nCurrentCPU] += CZetRun(nCyclesSegment);
			CZetClose();
		} 
		else 
		{
			nCyclesDone[nCurrentCPU] += nCyclesSegment;
		}
	}
//	draw_sprites();
	INT32 scrollx = (((nTigerHeliTileXPosHi << 8) + nTigerHeliTileXPosLo)) & 0x1ff;
	INT32 scrolly = (nTigerHeliTileYPosLo + 15) & 0xff;
	ss_reg->n2_move_y = -scrollx-283;
	if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
		SPR_WaitEndSlaveSH();
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void rotate_tile16x16(unsigned int size,unsigned char flip, unsigned char *target)
{
	unsigned int i,j,k,l=0;
	unsigned char temp[16][16];
	unsigned char rot[16][16];

	for (k=0;k<size;k++)
	{
		for(i=0;i<16;i++)
			for(j=0;j<8;j++)
			{
				temp[i][j<<1]=target[l+(i*8)+j]>>4;
				temp[i][(j<<1)+1]=target[l+(i*8)+j]&0x0f;
			}

		memset(&target[l],0,128);
		
		for(i=0;i<16;i++)
			for(j=0;j<16;j++)
			{
				if(flip)
				 rot[15-i][j]= temp[j][i] ;
				else
				 rot[i][15-j]= temp[j][i] ;
			}

		for(i=0;i<16;i++)
			for(j=0;j<8;j++)
					target[l+(i*8)+j]    = (rot[i][j*2]<<4)|(rot[i][(j*2)+1]&0xf);
		l+=128;
	}	
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void updateSound()
{
	unsigned int deltaSlave    = *(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);
	unsigned short *nSoundBuffer1 = (unsigned short *)0x25a24000+deltaSlave;

	AY8910UpdateDirect(0, &nSoundBuffer1[0], nBurnSoundLen);
	AY8910UpdateDirect(1, &nSoundBuffer1[0x6000], nBurnSoundLen);
	deltaSlave+=nBurnSoundLen;

	if(deltaSlave>=RING_BUF_SIZE>>1)
	{
		for (unsigned int i=0;i<6;i++)
		{
			PCM_NotifyWriteSize(pcm6[i], deltaSlave);
			PCM_Task(pcm6[i]); // bon emplacement
		}
		deltaSlave=0;
	}
	draw_sprites();
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = deltaSlave;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/  void make_lut(void)
{
	unsigned int i;
	int sx, sy;

	for (i = 0; i < 0x800; i++)
	{
/*		sx = (i & 0x3f);// * 8;
		sy = (i / 0x40);// * 8;
		map_offset_lut[i] = (sx|(sy<<6))<<1;

		sx = (i & 0x3f);// * 8;
		sy = (i / 0x40);// * 8;
		map_offset_lut2[i] = (sx|(sy<<6))<<1;
*/

// 		sx = (15-((i) & 0x1f))<<6;//% 32;
//		sy = ((i >> 6)) & 0x1f;
//		sx = (34-((i) & 0x3f))<<6;//% 32;
		sx = (35-((i) & 0x3f))<<6;//% 32;
		sy = ((i >> 6)) & 0x3f;
		map_offset_lut2[i] = ((sx) | sy)<<1;

		sx = (63-((i) & 0x3f))<<6;//% 32;
		sy = ((i >> 6)) & 0x3f;
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
/*static*/ void Set6PCM()
{
	PcmCreatePara	para[6];
	PcmInfo 		info[6];
	PcmStatus	*st;
	static PcmWork g_movie_work[6];

	for (int i=0; i<6; i++)
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
