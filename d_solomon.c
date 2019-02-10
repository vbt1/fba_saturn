#include "d_solomon.h"

#define	nCyclesTotal0 4000000 / 60
#define nCyclesTotal1 3072000 / 60 / 3
#define RAZE0 1
//#define USE_IDMA 1

#ifdef USE_IDMA
void vblIn()
{
	DMA_ScuIndirectMemCopy(ss_map2,bgmap2_buf,0x1000,0);
	SolomonCalcPalette();
//	DMA_ScuIndirectMemCopy(ss_map,bgmap_buf,0x1000,1);
}
#endif

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvSolomon = {
		"solomon", "solomn",
		"Solomon's Key (US)",
		SolomonRomInfo, SolomonRomName, SolomonInputInfo, SolomonDIPInfo,
		SolomonInit, SolomonExit, SolomonFrame, NULL
	};
	memcpy(shared,&nBurnDrvSolomon,sizeof(struct BurnDriver));
	ss_reg   = (SclNorscl *)SS_REG;
	ss_regs = (SclSysreg *)SS_REGS;
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

#ifdef RAZE0	 
	z80_reset();
#endif

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
#ifndef RAZE0
			CZetClose();
			CZetOpen(1);
#endif

		CZetNmi();

#ifndef RAZE0
			CZetClose();
			CZetOpen(0);
#endif
			return;
		}
	}
}

#ifdef RAZE0

void __fastcall SolomonWrite1_0xd000(UINT16 a, UINT8 d)
{
//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"Write1_0xd000                     ",20,100);
	if(RamStart[a]!=d)
	{
		RamStart[a]=d;
		unsigned int i = map_offset_lut[a&0x3ff];
#ifdef USE_IDMA
		bgmap2_buf[i] =  (d & 0x70) >> 4;
		bgmap2_buf[i+1] = 0x800|SolomonVideoRam[a&0x3ff] + ((d & 0x07)<<8);
#else
		ss_map2[i] =  (d & 0x70) >> 4;
		ss_map2[i+1] = 0x800|SolomonVideoRam[a&0x3ff] + ((d & 0x07)<<8);
#endif
	}
}

void __fastcall SolomonWrite1_0xd400(UINT16 a, UINT8 d)
{
//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"Write1_0xd400                     ",20,100);
	if(RamStart[a]!=d)
	{
		RamStart[a]=d;
		a&=0x3ff;
		unsigned int i = map_offset_lut[a];
#ifdef USE_IDMA
		bgmap2_buf[i+1] = 0x800|d + ((SolomonColourRam[a] & 0x07)<<8);
#else
		ss_map2[i+1] = 0x800|d + ((SolomonColourRam[a] & 0x07)<<8);
#endif
	}
}

void __fastcall SolomonWrite1_0xd800(UINT16 a, UINT8 d)
{
//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"Write1_0xd800                     ",20,100);
	if(RamStart[a]!=d)
	{
		RamStart[a]=d;

		UINT32 Colour = (d & 0x70) >> 4;
		UINT32 FlipX = (d & 0x80) >> 1;
		UINT32 FlipY = (d & 0x08) / 8;

		unsigned int i = map_offset_lut[a&0x3ff];
#ifdef USE_IDMAx
		bgmap_buf[i] =  8 | Colour | FlipX << 8 | FlipY << 16;
		bgmap_buf[i+1] = SolomonBgVideoRam[a&0x3ff] + ((d & 0x07)<<8);
#else
		ss_map[i] = 8 | Colour | FlipX << 8 | FlipY << 16;
		ss_map[i+1] = SolomonBgVideoRam[a&0x3ff] + ((d & 0x07)<<8);
#endif
	}
}

void __fastcall SolomonWrite1_0xdc00(UINT16 a, UINT8 d)
{
//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"Write1_0xdc00                     ",20,100);
	if(RamStart[a]!=d)
	{
		RamStart[a]=d;
		a&=0x3ff;
		unsigned int i = map_offset_lut[a];
#ifdef USE_IDMAx
		bgmap_buf[i+1] = d + ((SolomonBgColourRam[a] & 0x07)<<8);
#else
		ss_map[i+1] = d + ((SolomonBgColourRam[a] & 0x07)<<8);
#endif
	}
}
#endif

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
	CZ80Context					= Next; Next += (0x1080*2);

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

	pFMBuffer					= (INT16*)Next; Next += nBurnSoundLen * 9 * sizeof(INT16);
	map_offset_lut			= (UINT16*)Next; Next += 0x400 * sizeof(UINT16);
	cram_lut					= (UINT16*)Next; Next += 4096 * sizeof(UINT16);
#ifdef USE_IDMA
	bgmap_buf				= Next; Next += 0x1000 * sizeof (UINT16);//bgmap_lut + 0x20000;
	bgmap2_buf				= Next; Next += 0x1000 * sizeof (UINT16);//bgmap_lut + 0x20000;
#endif
//	SolomonPalette         = (UINT32*)Next; Next += 0x00200 * sizeof(UINT32);

	MemEnd                 = Next;

	return 0;
}

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
	UINT8 *SolomonTempRom	= (UINT8 *)0x00200000;
	UINT8 *ss_vram					= (UINT8 *)SS_SPRAM;
	UINT8 *SolomonBgTiles		= (UINT8 *)cache;
	UINT8 *SolomonFgTiles		= (UINT8 *)cache+0x10000;
	UINT8 *SolomonSprites		= &ss_vram[0x1100];

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
	SolomonTempRom = NULL;
	// Setup the Z80 emulation
//	CZetInit(2);
	CZetInit2(2,CZ80Context);

#ifndef RAZE0
	CZetOpen(0);
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
	CZetSetReadHandler(SolomonRead1);
	CZetSetWriteHandler(SolomonWrite1);

	CZetClose();
#else
	z80_init_memmap();
 	z80_map_read  (0x0000, 0xbfff, SolomonZ80Rom1);
 	z80_map_fetch (0x0000, 0xbfff, SolomonZ80Rom1);
 	z80_map_read  (0xc000, 0xcfff, SolomonZ80Ram1);
 	z80_map_write (0xc000, 0xcfff, SolomonZ80Ram1);
 	z80_map_fetch (0xc000, 0xcfff, SolomonZ80Ram1);
 	z80_map_read  (0xd000, 0xd3ff, SolomonColourRam);
// 	z80_map_write (0xd000, 0xd3ff, SolomonColourRam);
 	z80_map_fetch (0xd000, 0xd3ff, SolomonColourRam);
 	z80_map_read  (0xd400, 0xd7ff, SolomonVideoRam);
// 	z80_map_write (0xd400, 0xd7ff, SolomonVideoRam);
 	z80_map_fetch (0xd400, 0xd7ff, SolomonVideoRam);
 	z80_map_read  (0xd800, 0xdbff, SolomonBgColourRam);
// 	z80_map_write (0xd800, 0xdbff, SolomonBgColourRam);
 	z80_map_fetch (0xd800, 0xdbff, SolomonBgColourRam);
 	z80_map_read  (0xdc00, 0xdfff, SolomonBgVideoRam);
// 	z80_map_write (0xdc00, 0xdfff, SolomonBgVideoRam);
 	z80_map_fetch (0xdc00, 0xdfff, SolomonBgVideoRam);
 	z80_map_read  (0xe000, 0xe07f, SolomonSpriteRam);
 	z80_map_write (0xe000, 0xe07f, SolomonSpriteRam);
 	z80_map_fetch (0xe000, 0xe07f, SolomonSpriteRam);
 	z80_map_read  (0xe400, 0xe5ff, SolomonPaletteRam);
 	z80_map_write (0xe400, 0xe5ff, SolomonPaletteRam);
 	z80_map_fetch (0xe400, 0xe5ff, SolomonPaletteRam);
	z80_map_read(0xf000, 0xffff, SolomonZ80Rom1 + 0xf000);
	z80_map_fetch(0xf000, 0xffff, SolomonZ80Rom1 + 0xf000);

	z80_end_memmap();

	z80_add_read(0xe600, 0xe60f, 1, (void *)&SolomonRead1);
	z80_add_write(0xd000, 0xd3ff, 1, (void *)&SolomonWrite1_0xd000);
	z80_add_write(0xd400, 0xd7ff, 1, (void *)&SolomonWrite1_0xd400);
	z80_add_write(0xd800, 0xdbff, 1, (void *)&SolomonWrite1_0xd800);
	z80_add_write(0xdc00, 0xdfff,  1, (void *)&SolomonWrite1_0xdc00);
	z80_add_write(0xe600, 0xe60f, 1, (void *)&SolomonWrite1);
	z80_add_write(0xe800, 0xe80f, 1, (void *)&SolomonWrite1);
#endif

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

	// Reset the driver
	SolomonDoReset();

#ifdef USE_IDMA
	nBurnFunction = vblIn;
#else
	nBurnFunction = SolomonCalcPalette;
#endif	
	return 0;
}

INT32 SolomonExit()
{
	nBurnFunction = NULL;
	SPR_InitSlaveSH();

	z80_stop_emulating();
	z80_add_read(0xe600, 0xe60f, 1, (void *)NULL);
	z80_add_write(0xd000, 0xd3ff, 1, (void *)NULL);
	z80_add_write(0xd400, 0xd7ff, 1, (void *)NULL);
	z80_add_write(0xd800, 0xdbff, 1, (void *)NULL);
	z80_add_write(0xdc00, 0xdfff,  1, (void *)NULL);
	z80_add_write(0xe600, 0xe60f, 1, (void *)NULL);
	z80_add_write(0xe800, 0xe80f, 1, (void *)NULL);

	CZetExit2();

	for (INT32 i = 0; i < 3; i++) {
		AY8910Exit(i);
	}
	MemEnd = RamStart = RamEnd = SolomonZ80Rom1 = SolomonZ80Rom2 = NULL;
	SolomonZ80Ram1 = SolomonZ80Ram2 = SolomonColourRam = SolomonVideoRam = NULL;
	SolomonBgColourRam = SolomonBgVideoRam = SolomonSpriteRam = NULL;
	SolomonPaletteRam = CZ80Context = NULL;
	bgmap_buf = bgmap2_buf = map_offset_lut = cram_lut = NULL;

	for (int i = 0; i < 9; i++) {
		pAY8910Buffer[i] = NULL;
	}
	pFMBuffer = NULL;

	free(Mem);
	Mem = NULL;

	SolomonIrqFire = 0;
	SolomonFlipScreen = 0;
	SolomonSoundLatch = 0;
	SolomonReset = 0;
	return 0;
}

void SolomonRenderSpriteLayer()
{
	SprSpCmd *ss_spritePtr = &ss_sprite[0];

	for (INT32 Offs = 0x80 - 4; Offs >= 0; Offs -= 4) 
	{
		UINT32 Attr, Code;

		Attr = SolomonSpriteRam[Offs + 1];
		Code = SolomonSpriteRam[Offs] + 16 * (Attr & 0x10);

		ss_spritePtr->charAddr	= 0x220+(Code<<4);
		ss_spritePtr->control		= (Attr & 0xC0) >> 2;
		ss_spritePtr->ay			= 225 - SolomonSpriteRam[Offs + 2];
		ss_spritePtr->ax			=  8 + SolomonSpriteRam[Offs + 3];
		ss_spritePtr->color		=  (Attr & 0x0e)*8;
		ss_spritePtr++;
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

void SolomonCalcPalette()
{
	for (INT32 i = 0; i < 0x200; i++) 
	{
		colBgAddr[0x100 | (i / 2)] = colBgAddr[i / 2] = cram_lut[SolomonPaletteRam[i & ~1] | (SolomonPaletteRam[i | 1] << 8)];
	}
}

inline void SolomonDraw()
{
//	SolomonCalcPalette();
	SolomonRenderSpriteLayer();
}

INT32 SolomonFrame()
{
	SolomonMakeInputs();

	UINT32 nCyclesDone0 = 0, nCyclesDone1 = 0;
	*(unsigned int*)OPEN_CSH_VAR(SS_Z80CY) = 0;

	SPR_RunSlaveSH((PARA_RTN*)updateSound, NULL);

	INT32 nNext;
//--i = 0--------------------------------------------------------------------------------
	// Run Z80 #1
	nCyclesDone0 += z80_emulate(nCyclesTotal0 >> 1);
	// Run Z80 #2
	nNext = (nCyclesTotal1 >>1);
	CZetRunSlave(&nNext);
//--i = 1--------------------------------------------------------------------------------
	// Run Z80 #1
	z80_emulate(nCyclesTotal0 - nCyclesDone0);
	if(SolomonIrqFire) z80_cause_NMI();
	CZetSetIRQLine(0, CZET_IRQSTATUS_AUTO);
	// Run Z80 #2
	nCyclesDone1 += *(unsigned int*)OPEN_CSH_VAR(SS_Z80CY);
	nNext = nCyclesTotal1 - nCyclesDone1;
	CZetRunSlave(&nNext);

	SolomonDraw();
	SPR_WaitEndSlaveSH();
	CZetSetIRQLine(0, CZET_IRQSTATUS_AUTO);

//	updateSound();
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------
/*
void AY8910Update1Slave()
{
	AY8910Update(1, &pAY8910Buffer[3], nBurnSoundLen);
}
*/
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void updateSound()
{
	int nSample;
	signed short *nSoundBuffer		= (signed short *)0x25a20000+nSoundBufferPos;

//	SPR_RunSlaveSH((PARA_RTN*)AY8910Update1Slave, NULL);
	AY8910Update(0, &pAY8910Buffer[0], nBurnSoundLen);
	AY8910Update(1, &pAY8910Buffer[3], nBurnSoundLen);
	AY8910Update(2, &pAY8910Buffer[6], nBurnSoundLen);

	for (unsigned int n = 0; n < nBurnSoundLen; n++) 
	{
		nSample  = pAY8910Buffer[0][n]; 
		nSample += pAY8910Buffer[1][n];
		nSample += pAY8910Buffer[2][n];
		nSample += pAY8910Buffer[3][n];
		nSample += pAY8910Buffer[4][n];
		nSample += pAY8910Buffer[5][n];
		nSample += pAY8910Buffer[6][n];
		nSample += pAY8910Buffer[7][n];
		nSample += pAY8910Buffer[8][n];

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
		*nSoundBuffer++ = nSample;
	}

	if(nSoundBufferPos>=RING_BUF_SIZE/2)
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos);
		PCM_Task(pcm); // bon emplacement
		nSoundBufferPos=0;
	}

	nSoundBufferPos+=nBurnSoundLen;
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

	for (UINT32 i = 0; i < 4096; i++) 
	{
		cram_lut[i] = CalcCol(i);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void dummy()
{

}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DrvInitSaturn()
{
	SPR_InitSlaveSH();
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
#ifdef USE_IDMA
	DMA_ScuInit();
#endif

	nBurnSprites  = 32+4;//27;

	SS_MAP     = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B1;//+0x1E000;
	SS_MAP2   = ss_map2 =(Uint16 *)SCL_VDP2_VRAM_A1;//+0x1C000;
	SS_FONT   = ss_font    =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_CACHE = cache     =(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_reg->n1_move_y =  16 <<16;
	ss_reg->n2_move_y =  16;
	ss_reg->n1_move_x =  -8 <<16;
	ss_reg->n2_move_x =  -8;

//3 nbg
	SS_SET_S0PRIN(6);
	SS_SET_N0PRIN(7);
	SS_SET_N1PRIN(5);
	SS_SET_N2PRIN(4);
	ss_regs->tvmode = 0x8011;

	initLayers();
	initColors();
	initSprites(256-1,240-1,0,0,0,0);

	memset(ss_map,0,0x20000);

	for (unsigned int i = 3; i <nBurnSprites; i++) 
	{
		ss_sprite[i].control   = ( JUMP_NEXT | FUNC_NORMALSP);
		ss_sprite[i].charSize  = 0x210;
		ss_sprite[i].drawMode  = ( ECD_DISABLE | COMPO_REP);
		ss_sprite[i].ax    = -48;
		ss_sprite[i].ay    =  -32;
	}
	drawWindow(0,240,0,0,62); 
}
//-------------------------------------------------------------------------------------------------------------------------------------
