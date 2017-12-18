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

	SolomonZ80Ram1         = Next; Next += 0x01000;
	SolomonZ80Ram2         = Next; Next += 0x00800;
	SolomonColourRam       = Next; Next += 0x00400;
	SolomonVideoRam        = Next; Next += 0x00400;
	SolomonBgColourRam     = Next; Next += 0x00400;
	SolomonBgVideoRam      = Next; Next += 0x00400;
	SolomonSpriteRam       = Next; Next += 0x00080;
	SolomonPaletteRam      = Next; Next += 0x00200;

	RamEnd                 = Next;

	SolomonBgTiles         = Next; Next += 2048 * 8 * 8;
	SolomonFgTiles         = Next; Next += 2048 * 8 * 8;
	SolomonSprites         = Next; Next += 2048 * 8 * 8;
	pFMBuffer              = (INT16*)Next; Next += nBurnSoundLen * 9 * sizeof(INT16);
	SolomonPalette         = (UINT32*)Next; Next += 0x00200 * sizeof(UINT32);

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
	INT32 nRet = 0, nLen;

	// Allocate and Blank all required memory
	Mem = NULL;
	SolomonMemIndex();
	nLen = MemEnd - (UINT8 *)0;
	if ((Mem = (UINT8 *)BurnMalloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);
	SolomonMemIndex();

	SolomonTempRom = (UINT8 *)BurnMalloc(0x10000);

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
	CZetInit(0);
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

	CZetInit(1);
	CZetOpen(1);
	CZetSetReadHandler(SolomonRead2);
	CZetSetOutHandler(SolomonPortWrite2);
	CZetMapArea(0x0000, 0x3fff, 0, SolomonZ80Rom2         );
	CZetMapArea(0x0000, 0x3fff, 2, SolomonZ80Rom2         );
	CZetMapArea(0x4000, 0x47ff, 0, SolomonZ80Ram2         );
	CZetMapArea(0x4000, 0x47ff, 1, SolomonZ80Ram2         );
	CZetMapArea(0x4000, 0x47ff, 2, SolomonZ80Ram2         );
	CZetClose();

	BurnFree(SolomonTempRom);

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
	AY8910SetAllRoutes(0, 0.12, BURN_SND_ROUTE_BOTH);
	AY8910SetAllRoutes(1, 0.12, BURN_SND_ROUTE_BOTH);
	AY8910SetAllRoutes(2, 0.12, BURN_SND_ROUTE_BOTH);

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

void SolomonRenderBgLayer()
{
	for (INT32 Offs = 0; Offs < 0x400; Offs++) {
		INT32 sx, sy, Attr, Code, Colour, FlipX, FlipY;

		sx = (Offs % 32);
		sy = (Offs / 32);
		Attr = SolomonBgColourRam[Offs];
		Code = SolomonBgVideoRam[Offs] + 256 * (Attr & 0x07);
		Colour = (Attr & 0x70) >> 4;
		FlipX = Attr & 0x80;
		FlipY = Attr & 0x08;

		if (SolomonFlipScreen) {
			sx = 31 - sx;
			sy = 31 - sy;
			FlipX = !FlipX;
			FlipY = !FlipY;
		}

		sx *= 8;
		sy *= 8;
		sy -= 16;
/*
		if (sx >= 0 && sx < 247 && sy >= 0 && sy < 215) {
			if (!FlipY) {
				if (!FlipX) {
					Render8x8Tile_Mask(pTransDraw, Code, sx, sy, Colour, 4, 0, 128, SolomonBgTiles);
				} else {
					Render8x8Tile_Mask_FlipX(pTransDraw, Code, sx, sy, Colour, 4, 0, 128, SolomonBgTiles);
				}
			} else {
				if (!FlipX) {
					Render8x8Tile_Mask_FlipY(pTransDraw, Code, sx, sy, Colour, 4, 0, 128, SolomonBgTiles);
				} else {
					Render8x8Tile_Mask_FlipXY(pTransDraw, Code, sx, sy, Colour, 4, 0, 128, SolomonBgTiles);
				}
			}
		} else {
			if (!FlipY) {
				if (!FlipX) {
					Render8x8Tile_Mask_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0, 128, SolomonBgTiles);
				} else {
					Render8x8Tile_Mask_FlipX_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0, 128, SolomonBgTiles);
				}
			} else {
				if (!FlipX) {
					Render8x8Tile_Mask_FlipY_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0, 128, SolomonBgTiles);
				} else {
					Render8x8Tile_Mask_FlipXY_Clip(pTransDraw, Code, sx, sy, Colour, 4, 0, 128, SolomonBgTiles);
				}
			}
		}
*/
	}
}

void SolomonRenderFgLayer()
{
	for (INT32 Offs = 0x400 - 1; Offs >= 0; Offs--) {
		INT32 sx, sy, Code, Colour;

		sx = (Offs % 32);
		sy = (Offs / 32);
		Code = SolomonVideoRam[Offs] + 256 * (SolomonColourRam[Offs] & 0x07);
		Colour = (SolomonColourRam[Offs] & 0x70) >> 4;

		if (SolomonFlipScreen) {
			sx = 31 - sx;
			sy = 31 - sy;
		}

		sx *= 8;
		sy *= 8;
		sy -= 16;
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
	for (INT32 i = 0; i < 0x200; i++) {
		SolomonPalette[i / 2] = CalcCol(SolomonPaletteRam[i & ~1] | (SolomonPaletteRam[i | 1] << 8));
	}

	return 0;
}

void SolomonDraw()
{
//	BurnTransferClear();
	SolomonCalcPalette();
	SolomonRenderBgLayer();
	SolomonRenderFgLayer();
	SolomonRenderSpriteLayer();
//	BurnTransferCopy(SolomonPalette);
}

INT32 SolomonFrame()
{
	INT32 nInterleave = 2;
	INT32 nSoundBufferPos = 0;

	if (SolomonReset) SolomonDoReset();

	SolomonMakeInputs();

	nCyclesTotal[0] = 4000000 / 60;
	nCyclesTotal[1] = 3072000 / 60;
	nCyclesDone[0] = nCyclesDone[1] = 0;

	for (INT32 i = 0; i < nInterleave; i++) {
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
//		if (pBurnSoundOut) 
		{
			signed short *nSoundBuffer = (signed short *)0x25a20000;

			INT32 nSegmentLength = nBurnSoundLen / nInterleave;
			INT16* pSoundBuf = nSoundBuffer + (nSoundBufferPos << 1);
//			AY8910Render(&pAY8910Buffer[0], pSoundBuf, nSegmentLength, 0);
			AY8910Update(0, &pAY8910Buffer[0], nBurnSoundLen);

			nSoundBufferPos += nSegmentLength;
		}
	}

	// Make sure the buffer is entirely filled.
//	if (pBurnSoundOut) 
	{
		signed short *nSoundBuffer = (signed short *)0x25a20000;

		INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos;
		INT16* pSoundBuf = nSoundBuffer + (nSoundBufferPos << 1);
		if (nSegmentLength) 
		{
			AY8910Render(&pAY8910Buffer[0], pSoundBuf, nSegmentLength, 0);
		}
	}

//	if (pBurnDraw) 
	SolomonDraw();

	return 0;
}
