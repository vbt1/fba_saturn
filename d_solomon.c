#include "d_solomon.h"

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvSolomon = {
		"solomon", "solomn",
		"Solomon's Key (US)",
		SolomonRomInfo, SolomonRomName, SolomonInputInfo, SolomonDIPInfo,
		SolomonInit, SolomonExit, SolomonFrame, NULL
	};
	memcpy(shared,&nBurnDrvSolomon,sizeof(struct BurnDriver));
	ss_reg          = (SclNorscl *)SS_REG;
}

inline void SolomonClearOpposites(UINT8* nJoystickInputs)
{
	if ((*nJoystickInputs & 0x03) == 0x03) {
		*nJoystickInputs &= ~0x03;
	}
	if ((*nJoystickInputs & 0x0c) == 0x0c) {
		*nJoystickInputs &= ~0x0c;
	}
}

inline void SolomonMakeInputs()
{
	// Reset Inputs
	SolomonInput[0] = SolomonInput[1] = SolomonInput[2] = 0x00;

	// Compile Digital Inputs
	for (INT32 i = 0; i < 8; i++) {
		SolomonInput[0] |= (SolomonInputPort0[i] & 1) << i;
		SolomonInput[1] |= (SolomonInputPort1[i] & 1) << i;
		SolomonInput[2] |= (SolomonInputPort2[i] & 1) << i;
	}

	// Clear Opposites
	SolomonClearOpposites(&SolomonInput[0]);
	SolomonClearOpposites(&SolomonInput[1]);
}

INT32 SolomonDoReset()
{
	SolomonIrqFire = 0;
	SolomonFlipScreen = 0;
	SolomonSoundLatch = 0;

	for (INT32 i = 0; i < 2; i++) {
		CZetOpen(i);
		CZetReset();
		CZetClose();
	}

	for (INT32 i = 0; i < 3; i++) {
		AY8910Reset(i);
	}

//	HiscoreReset();

	return 0;
}

UINT8 __fastcall SolomonRead1(UINT16 a)
{
	switch (a) {
		case 0xe600: {
			return SolomonInput[0];
		}

		case 0xe601: {
			return SolomonInput[1];
		}

		case 0xe602: {
			return SolomonInput[2];
		}

		case 0xe604: {
			return SolomonDip[0];
		}

		case 0xe605: {
			return SolomonDip[1];
		}
	}

	return 0;
}

void __fastcall SolomonWrite1(UINT16 a, UINT8 d)
{
	switch (a) {
		case 0xe600: {
			SolomonIrqFire = d;
			return;
		}

		case 0xe604: {
			SolomonFlipScreen = d & 1;
			return;
		}

		case 0xe800: {
			SolomonSoundLatch = d;
			CZetClose();
			CZetOpen(1);
			CZetNmi();
			CZetClose();
			CZetOpen(0);
			return;
		}
	}

	if(a>=0xdc00 && a<=0xdfff)
	{
//		if(RamStart[a]!=d)
		{
			RamStart[a]=d;
			a &= 0x3ff;

			UINT32 x			= map_offset_lut[a];
			UINT32 Attr		= SolomonBgColourRam[a];
			UINT32 Code		= d + (Attr & 0x07) << 8;
			UINT8 Colour		= (Attr & 0x70) >> 4;

			ss_map[x] = Colour|8;
			ss_map[x+1] = Code;
		}
	}
}

UINT8 __fastcall SolomonRead2(UINT16 a)
{
	switch (a) {
		case 0x8000: {
			return SolomonSoundLatch;
		}
	}

	return 0;
}

void __fastcall SolomonPortWrite2(UINT16 a, UINT8 d)
{
	a &= 0xff;

	switch (a) {
		case 0x10: {
			AY8910Write(0, 0, d);
			return;
		}

		case 0x11: {
			AY8910Write(0, 1, d);
			return;
		}

		case 0x20: {
			AY8910Write(1, 0, d);
			return;
		}

		case 0x21: {
			AY8910Write(1, 1, d);
			return;
		}

		case 0x30: {
			AY8910Write(2, 0, d);
			return;
		}

		case 0x31: {
			AY8910Write(2, 1, d);
			return;
		}
	}
}

static INT32 SolomonMemIndex()
{
	UINT8 *Next; Next = Mem;

	SolomonZ80Rom1         = Next; Next += 0x10000;
	SolomonZ80Rom2         = Next; Next += 0x04000;

	RamStart               = Next;
	Next += 0x0B800;// vbt pour alignement
	SolomonZ80Ram1         = Next; Next += 0x01000;
	SolomonZ80Ram2         = Next; Next += 0x00800;
	SolomonColourRam       = Next; Next += 0x00400;
	SolomonVideoRam        = Next; Next += 0x00400;
	SolomonBgColourRam     = Next; Next += 0x00400;
	SolomonBgVideoRam      = Next; Next += 0x00400;
	SolomonSpriteRam       = Next; Next += 0x00080;
	SolomonPaletteRam      = Next; Next += 0x00200;

	RamEnd                 = Next;

//	SolomonBgTiles         = Next; Next += 2048 * 8 * 8;
//	SolomonFgTiles         = Next; Next += 2048 * 8 * 8;
//	SolomonSprites         = Next; Next += 2048 * 8 * 8;
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;

	SolomonBgTiles			= (UINT8 *)cache;
	SolomonFgTiles			= (UINT8 *)cache+0x10000;
	SolomonSprites         = &ss_vram[0x1100];

	pFMBuffer					= (INT16*)Next; Next += nBurnSoundLen * 9 * sizeof(INT16);
	map_offset_lut			= (UINT16*)Next; Next += 0x400 * sizeof(UINT16);
//	SolomonPalette         = (UINT32*)Next; Next += 0x00200 * sizeof(UINT32);

	MemEnd                 = Next;

	return 0;
}

static INT32 TilePlaneOffsets[4]   = { 0, 1, 2, 3 };
static INT32 TileXOffsets[8]       = { 0, 4, 8, 12, 16, 20, 24, 28 };
static INT32 TileYOffsets[8]       = { 0, 32, 64, 96, 128, 160, 192, 224 };
static INT32 SpritePlaneOffsets[4] = { 0, 131072, 262144, 393216 };
static INT32 SpriteXOffsets[16]    = { 0, 1, 2, 3, 4, 5, 6, 7, 64, 65, 66, 67, 68, 69, 70, 71 };
static INT32 SpriteYOffsets[16]    = { 0, 8, 16, 24, 32, 40, 48, 56, 128, 136, 144, 152, 160, 168, 176, 184 };

INT32 SolomonInit()
{
	DrvInitSaturn();
	INT32 nRet = 0, nLen;

	INT32 TilePlaneOffsets[4]   = { 0, 1, 2, 3 };
	INT32 TileXOffsets[8]       = { 0, 4, 8, 12, 16, 20, 24, 28 };
	INT32 TileYOffsets[8]       = { 0, 32, 64, 96, 128, 160, 192, 224 };
	INT32 SpritePlaneOffsets[4] = { 0, 131072, 262144, 393216 };
	INT32 SpriteXOffsets[16]    = { 0, 1, 2, 3, 4, 5, 6, 7, 64, 65, 66, 67, 68, 69, 70, 71 };
	INT32 SpriteYOffsets[16]    = { 0, 8, 16, 24, 32, 40, 48, 56, 128, 136, 144, 152, 160, 168, 176, 184 };

	// Allocate and Blank all required memory

	Mem = NULL;
	SolomonMemIndex();
	nLen = MemEnd - (UINT8 *)0;
	if ((Mem = (UINT8 *)BurnMalloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);
	SolomonMemIndex();
	make_lut();

//	SolomonTempRom = (UINT8 *)BurnMalloc(0x10000);
	SolomonTempRom = (UINT8 *)0x00200000;

	// Load Z80 #1 Program Roms
	nRet = BurnLoadRom(SolomonZ80Rom1, 0, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(SolomonTempRom, 1, 1); if (nRet != 0) return 1;
	memcpy(SolomonZ80Rom1 + 0x4000, SolomonTempRom + 0x4000, 0x4000);
	memcpy(SolomonZ80Rom1 + 0x8000, SolomonTempRom + 0x0000, 0x4000);
	memset(SolomonTempRom, 0, 0x10000);
	nRet = BurnLoadRom(SolomonTempRom, 2, 1); if (nRet != 0) return 1;
	memcpy(SolomonZ80Rom1 + 0xf000, SolomonTempRom, 0x1000);
	
	// Load Z80 #2 Program Rom
	nRet = BurnLoadRom(SolomonZ80Rom2, 3, 1); if (nRet != 0) return 1;

	// Load and decode Bg Tiles
	memset(SolomonTempRom, 0, 0x10000);
	nRet = BurnLoadRom(SolomonTempRom + 0x0000, 6, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(SolomonTempRom + 0x8000, 7, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(2048, 4, 8, 8, TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x100, SolomonTempRom, SolomonBgTiles);

	// Load and decode Fg Tiles
	memset(SolomonTempRom, 0, 0x10000);
	nRet = BurnLoadRom(SolomonTempRom + 0x0000, 4, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(SolomonTempRom + 0x8000, 5, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(2048, 4, 8, 8, TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x100, SolomonTempRom, SolomonFgTiles);

	// Load and decode Sprite Tiles
	memset(SolomonTempRom, 0, 0x10000);
	nRet = BurnLoadRom(SolomonTempRom + 0x0000,  8, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(SolomonTempRom + 0x4000,  9, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(SolomonTempRom + 0x8000, 10, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(SolomonTempRom + 0xc000, 11, 1); if (nRet != 0) return 1;
//	SolomonDecodeSprites(SolomonSprites, 2048, 0x0000, 0x4000, 0x8000, 0xc000);
	GfxDecode4Bpp(512, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x100, SolomonTempRom, SolomonSprites);

	// Setup the Z80 emulation
	CZetInit(2);
	CZetOpen(0);
	CZetSetReadHandler(SolomonRead1);
	CZetSetWriteHandler(SolomonWrite1);
	CZetMapArea(0x0000, 0xbfff, 0, SolomonZ80Rom1         );
	CZetMapArea(0x0000, 0xbfff, 2, SolomonZ80Rom1         );
	CZetMapArea(0xc000, 0xcfff, 0, SolomonZ80Ram1         );
	CZetMapArea(0xc000, 0xcfff, 1, SolomonZ80Ram1         );
	CZetMapArea(0xc000, 0xcfff, 2, SolomonZ80Ram1         );
	CZetMapArea(0xd000, 0xd3ff, 0, SolomonColourRam       );
	CZetMapArea(0xd000, 0xd3ff, 1, SolomonColourRam       );
	CZetMapArea(0xd000, 0xd3ff, 2, SolomonColourRam       );
	CZetMapArea(0xd400, 0xd7ff, 0, SolomonVideoRam        );
	CZetMapArea(0xd400, 0xd7ff, 1, SolomonVideoRam        );
	CZetMapArea(0xd400, 0xd7ff, 2, SolomonVideoRam        );
	CZetMapArea(0xd800, 0xdbff, 0, SolomonBgColourRam     );
	CZetMapArea(0xd800, 0xdbff, 1, SolomonBgColourRam     );
	CZetMapArea(0xd800, 0xdbff, 2, SolomonBgColourRam     );
	CZetMapArea(0xdc00, 0xdfff, 0, SolomonBgVideoRam      );
	CZetMapArea(0xdc00, 0xdfff, 1, SolomonBgVideoRam      );
	CZetMapArea(0xdc00, 0xdfff, 2, SolomonBgVideoRam      );
	CZetMapArea(0xe000, 0xe07f, 0, SolomonSpriteRam       );
	CZetMapArea(0xe000, 0xe07f, 1, SolomonSpriteRam       );
	CZetMapArea(0xe000, 0xe07f, 2, SolomonSpriteRam       );
	CZetMapArea(0xe400, 0xe5ff, 0, SolomonPaletteRam      );
	CZetMapArea(0xe400, 0xe5ff, 1, SolomonPaletteRam      );
	CZetMapArea(0xe400, 0xe5ff, 2, SolomonPaletteRam      );
	CZetMapArea(0xf000, 0xffff, 0, SolomonZ80Rom1 + 0xf000);
	CZetMapArea(0xf000, 0xffff, 2, SolomonZ80Rom1 + 0xf000);
	CZetClose();

//	CZetInit(1);
	CZetOpen(1);
	CZetSetReadHandler(SolomonRead2);
	CZetSetOutHandler(SolomonPortWrite2);
	CZetMapArea(0x0000, 0x3fff, 0, SolomonZ80Rom2         );
	CZetMapArea(0x0000, 0x3fff, 2, SolomonZ80Rom2         );
	CZetMapArea(0x4000, 0x47ff, 0, SolomonZ80Ram2         );
	CZetMapArea(0x4000, 0x47ff, 1, SolomonZ80Ram2         );
	CZetMapArea(0x4000, 0x47ff, 2, SolomonZ80Ram2         );
	CZetClose();

//	BurnFree(SolomonTempRom);
	SolomonTempRom = NULL;

	pAY8910Buffer[0] = pFMBuffer + nBurnSoundLen * 0;
	pAY8910Buffer[1] = pFMBuffer + nBurnSoundLen * 1;
	pAY8910Buffer[2] = pFMBuffer + nBurnSoundLen * 2;
	pAY8910Buffer[3] = pFMBuffer + nBurnSoundLen * 3;
	pAY8910Buffer[4] = pFMBuffer + nBurnSoundLen * 4;
	pAY8910Buffer[5] = pFMBuffer + nBurnSoundLen * 5;
	pAY8910Buffer[6] = pFMBuffer + nBurnSoundLen * 6;
	pAY8910Buffer[7] = pFMBuffer + nBurnSoundLen * 7;
	pAY8910Buffer[8] = pFMBuffer + nBurnSoundLen * 8;

	AY8910Init(0, 1500000, nBurnSoundRate, NULL, NULL, NULL, NULL);
	AY8910Init(1, 1500000, nBurnSoundRate, NULL, NULL, NULL, NULL);
	AY8910Init(2, 1500000, nBurnSoundRate, NULL, NULL, NULL, NULL);
//	AY8910SetAllRoutes(0, 0.12, BURN_SND_ROUTE_BOTH);
//	AY8910SetAllRoutes(1, 0.12, BURN_SND_ROUTE_BOTH);
//	AY8910SetAllRoutes(2, 0.12, BURN_SND_ROUTE_BOTH);

//	GenericTilesInit();

	// Reset the driver
	SolomonDoReset();

	return 0;
}

INT32 SolomonExit()
{
	CZetExit();

	for (INT32 i = 0; i < 3; i++) {
		AY8910Exit(i);
	}

//	GenericTilesExit();

	BurnFree(Mem);

	return 0;
}

void SolomonRenderLayer()
{
	for (UINT32 Offs = 0; Offs < 0x400; Offs++) 
	{
 
		INT32 Attr, Code, Colour, FlipX, FlipY;

		Attr = SolomonBgColourRam[Offs];
		Code = SolomonBgVideoRam[Offs] + 256 * (Attr & 0x07);
		Colour = (Attr & 0x70) >> 4;
		FlipX = Attr & 0x80;
//		FlipY = Attr & 0x08;

		unsigned int i = map_offset_lut[Offs];
		ss_map[i] = 8 | Colour | FlipX << 7;
		ss_map[i+1] = Code;

		Code = SolomonVideoRam[Offs] + 256 * (SolomonColourRam[Offs] & 0x07);
		Colour = (SolomonColourRam[Offs] & 0x70) >> 4;

		ss_map2[i] = Colour;
		ss_map2[i+1] = 0x800|Code;
	}
}

void SolomonRenderFgLayer()
{
	for (INT32 Offs = 0x400 - 1; Offs >= 0; Offs--) 
	{
//		INT32 sx, sy, 
		INT32 Code, Colour;

//		sx = (Offs % 32);
//		sy = (Offs / 32);

//		UINT32 sx		= Offs & 0x1f;
//		UINT32 sy		= ((Offs<<1) & (~0x3f));

		Code = SolomonVideoRam[Offs] + 256 * (SolomonColourRam[Offs] & 0x07);
		Colour = (SolomonColourRam[Offs] & 0x70) >> 4;

/*		if (SolomonFlipScreen) {
			sx = 31 - sx;
			sy = 31 - sy;
		}*/

//		ss_map2[sx|sy] = Colour << 12 | Code;
		unsigned int i = map_offset_lut[Offs]; //(sx|sy)<<1;
		ss_map2[i] = Colour;
		ss_map2[i+1] = 0x800|Code;
//		ss_map2[sx|sy] = Offs;

//		sx *= 8;
//		sy *= 8;
//		sy -= 16;
/*
		if (sx >= 0 && sx < 247 && sy >= 0 && sy < 215) {
			if (!SolomonFlipScreen) {
				Render8x8Tile_Mask(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, SolomonFgTiles);
			} else {
				Render8x8Tile_Mask_FlipXY(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, SolomonFgTiles);
			}
		} else {
			if (!SolomonFlipScreen) {
				Render8x8Tile_Mask_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, SolomonFgTiles);
			} else {
				Render8x8Tile_Mask_FlipXY_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, SolomonFgTiles);
			}
		}
*/
	}
}

void SolomonRenderSpriteLayer()
{
	for (INT32 Offs = 0x80 - 4; Offs >= 0; Offs -= 4) {
		INT32 sx, sy, Attr, Code, Colour, FlipX, FlipY;

		sx = SolomonSpriteRam[Offs + 3];
		sy = 241 - SolomonSpriteRam[Offs + 2];
		Attr = SolomonSpriteRam[Offs + 1];
		Code = SolomonSpriteRam[Offs] + 16 * (Attr & 0x10);
		Colour = (Attr & 0x0e) >> 1;
		FlipX = Attr & 0x40;
		FlipY = Attr & 0x80;

		if (SolomonFlipScreen & 1) {
			sx = 240 - sx;
			sy = 240 - sy;
			FlipX = !FlipX;
			FlipY = !FlipY;
		}

		sy -= 16;
/*
		if (sx >= 0 && sx < 239 && sy >= 0 && sy < 207) {
			if (!FlipY) {
				if (!FlipX) {
					Render16x16Tile_Mask(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, SolomonSprites);
				} else {
					Render16x16Tile_Mask_FlipX(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, SolomonSprites);
				}
			} else {
				if (!FlipX) {
					Render16x16Tile_Mask_FlipY(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, SolomonSprites);
				} else {
					Render16x16Tile_Mask_FlipXY(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, SolomonSprites);
				}
			}
		} else {
			if (!FlipY) {
				if (!FlipX) {
					Render16x16Tile_Mask_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, SolomonSprites);
				} else {
					Render16x16Tile_Mask_FlipX_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, SolomonSprites);
				}
			} else {
				if (!FlipX) {
					Render16x16Tile_Mask_FlipY_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, SolomonSprites);
				} else {
					Render16x16Tile_Mask_FlipXY_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0, 0, SolomonSprites);
				}
			}
		}
*/
	}
}

inline static UINT32 CalcCol(UINT16 nColour)
{
	INT32 r, g, b;

	r = (nColour >> 0) & 0x0f;
	g = (nColour >> 4) & 0x0f;
	b = (nColour >> 8) & 0x0f;

	r = (r << 4) | r;
	g = (g << 4) | g;
	b = (b << 4) | b;

	return BurnHighCol(r, g, b, 0);
}

INT32 SolomonCalcPalette()
{
	unsigned int delta=0;

	for (INT32 i = 0; i < 0x200; i++) 
	{
		colBgAddr[0x100 | (i / 2)] = colBgAddr[i / 2] = CalcCol(SolomonPaletteRam[i & ~1] | (SolomonPaletteRam[i | 1] << 8));
	}

	return 0;
}

inline void SolomonDraw()
{
	SolomonCalcPalette();
	SolomonRenderLayer();
}

INT32 SolomonFrame()
{
	INT32 nInterleave = 2;
	INT32 nSoundBufferPos = 0;

//	if (SolomonReset) SolomonDoReset();
	SolomonMakeInputs();

	nCyclesTotal[0] = 4000000 / 60;
	nCyclesTotal[1] = 3072000 / 60;
	nCyclesDone[0] = nCyclesDone[1] = 0;

	for (INT32 i = 0; i < nInterleave; i++) 
	{
		INT32 nCurrentCPU, nNext;

		// Run Z80 #1
		nCurrentCPU = 0;
		CZetOpen(nCurrentCPU);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesDone[nCurrentCPU] += CZetRun(nCyclesSegment);
		if (i == 1) if(SolomonIrqFire) CZetNmi();
		CZetClose();

		// Run Z80 #2
		nCurrentCPU = 1;
		CZetOpen(nCurrentCPU);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesSegment = CZetRun(nCyclesSegment);
		nCyclesDone[nCurrentCPU] += nCyclesSegment;
		CZetSetIRQLine(0, CZET_IRQSTATUS_AUTO);
		CZetClose();

		// Render Sound Segment
		{
//			signed short *nSoundBuffer = (signed short *)0x25a20000;

//			INT32 nSegmentLength = nBurnSoundLen / nInterleave;
//			INT16* pSoundBuf = nSoundBuffer + (nSoundBufferPos << 1);
//			AY8910Render(&pAY8910Buffer[0], pSoundBuf, nSegmentLength, 0);
//			AY8910Update(0, &pAY8910Buffer[0], nBurnSoundLen);
//			nSoundBufferPos += nSegmentLength;
		}
	}

//	updateSound();

	SolomonDraw();

	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void updateSound()
{
	int nSample;
	int n;
	unsigned int deltaSlave;//soundLenSlave;//,titiSlave;
	signed short *nSoundBuffer = (signed short *)0x25a20000;
	deltaSlave    = *(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);

	AY8910Update(0, &pAY8910Buffer[0], nBurnSoundLen);
	AY8910Update(1, &pAY8910Buffer[3], nBurnSoundLen);
	AY8910Update(2, &pAY8910Buffer[6], nBurnSoundLen);

	for (n = 0; n < nBurnSoundLen; n++) 
	{
		nSample  = pAY8910Buffer[0][n]; // >> 2;
		nSample += pAY8910Buffer[1][n]; // >> 2;
		nSample += pAY8910Buffer[2][n]; // >> 2;
		nSample += pAY8910Buffer[3][n]; // >> 2;
		nSample += pAY8910Buffer[4][n]; // >> 2;
		nSample += pAY8910Buffer[5][n]; // >> 2;
		nSample += pAY8910Buffer[6][n]; // >> 2;
		nSample += pAY8910Buffer[7][n]; // >> 2;
		nSample += pAY8910Buffer[8][n]; // >> 2;

		nSample /=4;

		if (nSample < -32768) 
		{
			nSample = -32768;
		} 
		else 
		{
			if (nSample > 32767) 
			{
				nSample = 32767;
			}
		}	
		nSoundBuffer[deltaSlave + n] = nSample;
//		nSoundBuffer[nSoundBufferPos + n] = nSample;
	}

	if(deltaSlave>=RING_BUF_SIZE/2)
//	if(nSoundBufferPos>=RING_BUF_SIZE/2)
	{
		PCM_NotifyWriteSize(pcm, deltaSlave);
		PCM_Task(pcm); // bon emplacement
		deltaSlave=0;
//		nSoundBufferPos = 0;
	}

	deltaSlave+=nBurnSoundLen;
//	nSoundBufferPos+=nBurnSoundLen;

	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = deltaSlave;
}

//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initLayers()
{

    Uint16	CycleTb[]={
		0x1f56, 0xeeee, //A0
		0xffff, 0xffff,	//A1
		0xf5f2,0x4eee,   //B0
		0xffff, 0xffff  //B1
	};

/*
    Uint16	CycleTb[]={
		0xf2ff, 0xffff, //A0
		0xfff0, 0x45ef,	//A1
		0x1fff, 0xffff,   //B0
		0xffff, 0xffff  //B1
	};
*/
/*
voir page 58 vdp2
voir plutot p355 vdp2
*/
 	SclConfig	scfg;
//A0 0x0000 0000
//A1 0x0004 0x005
//B0 0x0008
//B1 0x000c 1100
// 3 nbg
	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD;
	scfg.flip          = SCL_PN_10BIT; 
	scfg.platesize     = SCL_PL_SIZE_1X1;
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.patnamecontrl =  0x0000;// VRAM A 0??のオフセット 
	scfg.plate_addr[0] = (Uint32)ss_map;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG2, &scfg);
// 3 nbg
//	scfg.dispenbl      = ON;
	scfg.charsize          = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize      = SCL_PN2WORD;
//	scfg.patnamecontrl =  0x00008;// VRAM B0 のオフセット 
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
//	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
//	scfg.datatype      = SCL_CELL;

	scfg.coltype       = SCL_COL_TYPE_16;
	scfg.platesize     = SCL_PL_SIZE_1X1;
	scfg.plate_addr[0] = (Uint32)ss_map2;
//	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG1, &scfg);

//	scfg.dispenbl 		 = OFF;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 		 = SCL_BITMAP;
	scfg.mapover		 = SCL_OVER_0;
	scfg.plate_addr[0]	 = (Uint32)ss_font;

// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
	SCL_AllocColRam(SCL_NBG2,ON);
	SCL_AllocColRam(SCL_NBG3,OFF);
	colAddr = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
//	SCL_SetColRamOffset(SCL_NBG2,0,OFF);
	ss_regs->dispenbl &= 0xfbff;
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_NBG0,OFF);
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void make_lut(void)
{
	for (UINT32 i = 0; i < 1024;i++) 
	{
		UINT32 sx		= i & 0x1f;
		UINT32 sy		= ((i<<1) & (~0x3f));

		map_offset_lut[i] = (sx| sy)<<1;
	}

/*	
	for (UINT32 i = 0; i < 256;i++) 
	{
		INT32 sy = (i % 16) <<5;//<<6
		INT32 sx = (15 - (i / 16));//<<1;
		mapbg_offset_lut[i] = (sx| sy);//<<1;
	}

	for (UINT32 i = 0; i < 4096; i++) 
	{
		cram_lut[i] = CalcCol(i);
	}*/
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DrvInitSaturn()
{
	SPR_InitSlaveSH();
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);

	nBurnSprites  = 24+4;//27;

	SS_MAP     = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B1;//+0x1E000;
	SS_MAP2   = ss_map2 =(Uint16 *)SCL_VDP2_VRAM_A1;//+0x1C000;
	SS_FONT   = ss_font    =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_CACHE = cache     =(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
//	ss_reg->n1_move_x = -8<<16;
//	ss_reg->n2_move_x = 8;
	ss_reg->n1_move_y =  16 <<16;
	ss_reg->n2_move_y =  16;


//3 nbg
	SS_SET_S0PRIN(6);
	SS_SET_N0PRIN(7);
	SS_SET_N1PRIN(5);
	SS_SET_N2PRIN(4);
	ss_regs->tvmode = 0x8011;

	initLayers();
	initColors();
	initSprites(256-1,240-1,0,0,7,0);

    ss_sprite[nBurnSprites-1].control			= CTRL_END;
    ss_sprite[nBurnSprites-1].link				= 0;        
    ss_sprite[nBurnSprites-1].drawMode	= 0;                
    ss_sprite[nBurnSprites-1].color			= 0;                
    ss_sprite[nBurnSprites-1].charAddr		= 0;                
    ss_sprite[nBurnSprites-1].charSize		= 0;

//	nBurnFunction = CalcAll;
	drawWindow(0,240,0,0,64); 
}
//-------------------------------------------------------------------------------------------------------------------------------------
