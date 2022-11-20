#define BMP 1
#define SOUND 1
#define CZ80 1
//#define RAZE1 1
#define RAZE0 1
#define USE_MAP 1
#define CZET_SLAVE 1
#define nInterleave 256//140 // dac needs 128 NMIs
#define nCPUClockspeed 3579645 
#define nSegmentLength nBurnSoundLen / nInterleave
#define PONY 1
#include "d_vigilant.h"

void YM2151UpdateOneSlave();
static void Set8PCM();
UINT32 nCyclesTotal[2];
UINT32 nBurnCurrentYM2151Register;
//INT16 oldScroll =0;
extern unsigned short frame_x, frame_y;
void dummy()
{
	
}

int ovlInit(char *szShortName)
{
	cleanBSS();

	struct BurnDriver nBurnDrvVigilant = {
		"vigil", NULL,
		"Vigilante (World)\0",
		DrvRomInfo, DrvRomName, DrvInputInfo, DrvDIPInfo,
		DrvInit, DrvExit, DrvFrame, NULL //NULL
	};

	memcpy(shared,&nBurnDrvVigilant,sizeof(struct BurnDriver));
	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/void DrvInitSaturn();

/*static*/int MemIndex()
{
	extern unsigned int _malloc_max_ram;
	UINT8 *Next; Next = (unsigned char *)&_malloc_max_ram;
	memset(Next, 0, MALLOC_MAX);

	DrvZ80Rom1             = Next; Next += 0x28000;
	DrvZ80Rom2             = Next; Next += 0x10000;
	DrvSamples             = Next; Next += 0x10000;
	CZ80Context			  = Next; Next += (sizeof(cz80_struc)*2);
	//RamStart               = Next;

	DrvZ80Ram1             = Next; Next += 0x02000;
	DrvZ80Ram2             = Next; Next += 0x01000;
	DrvSpriteRam           = Next; Next += 0x00100;
	DrvPaletteRam          = Next; Next += 0x00800;
	DrvVideoRam            = Next; Next += 0x01000;

//	RamEnd                 = Next;
	
	vb_buffer				 = Next; Next += 0x4000 * sizeof(UINT32);
	DrvChars               = (UINT8 *)SS_CACHE;//Next; Next += 0x1000 * 8 * 8;
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	DrvSprites             = &ss_vram[0x1100];//Next; Next += 0x1000 * 16 * 16;
#ifndef BMP
	DrvBackTiles          = (UINT8 *)SS_CACHE+0x20000;//Next; Next += 0x4000 * 32;
#else
	DrvBackTiles          = (UINT8 *)SS_CACHE+0x30000;//Next; Next += 0x4000 * 32;
#endif
//	lBuffer					 = (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16) * 10;
	MemEnd                = Next;
	return 0;
}

/*static*/inline void DrvSetVector(INT32 Status)
{
	switch (Status) {
		case VECTOR_INIT: {
			DrvIrqVector = 0xff;
			break;
		}
		
		case YM2151_ASSERT: {
			DrvIrqVector &= 0xef;
			break;
		}
		
		case YM2151_CLEAR: {
			DrvIrqVector |= 0x10;
			break;
		}
		
		case Z80_ASSERT: {
			DrvIrqVector &= 0xdf;
			break;
		}
		
		case Z80_CLEAR: {
			DrvIrqVector |= 0x20;
			break;
		}
	}
	
	if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
	{
		SPR_WaitEndSlaveSH();
	}
	nCyclesDone[1]=*(unsigned int*)OPEN_CSH_VAR(SS_Z80CY);
	CZetOpen(1);

	if (DrvIrqVector == 0xff) 
	{
		CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);
	}
	else 
	{
		CZetSetIRQLine(DrvIrqVector, CZET_IRQSTATUS_ACK);
		nCyclesDone[1] += CZetRun(1000);
	}
}

/*static*/INT32 DrvDoReset()
{
	for (INT32 i = 0; i < 2; i++) {
		CZetOpen(i);
		CZetReset();
	
//	memset (lBuffer, 0, nBurnSoundLen * sizeof(INT16));

#ifdef RAZE0
		 z80_reset();
#endif

		if (i == 1) DrvSetVector(VECTOR_INIT);
//		CZetClose();
	}
	
#ifdef SOUND2
//	BurnYM2151Reset();
	YM2151ResetChip(0);
//	DACReset();
#endif
	
	DrvRomBank = 0;
	DrvSoundLatch = 0;
	DrvRearColour = 0;
	DrvRearDisable = 0;
	DrvHorizScrollLo = 0;
	DrvHorizScrollHi = 0;
	DrvRearHorizScrollLo = 0;
	DrvRearHorizScrollHi = 0;
	DrvSampleAddress = 0;

	return 0;
}

UINT8 __fastcall VigilanteZ80Read1(UINT16 a)
{
	return 0;
}
#ifdef RAZE0
void __fastcall VigilanteZ80Write1_c020(UINT16 a, UINT8 d)
{
		DrvSpriteRam[a - 0xc020] = d;
}

void __fastcall VigilanteZ80Write1_c800(UINT16 a, UINT8 d)
{
	unsigned int Offset = a & 0x7ff;

	if (DrvPaletteRam[Offset]!=d)
	{
		unsigned short Bank = Offset & 0x400;

		unsigned char r, g, b;
		DrvPaletteRam[Offset] = d;
		Offset &= 0xff;

		r = (DrvPaletteRam[Bank + Offset + 0x000] ); //& 0x1f;//0x1c;
		g = (DrvPaletteRam[Bank + Offset + 0x100] ); //& 0x1f;//0x1c;
		b = (DrvPaletteRam[Bank + Offset + 0x200] );// & 0x1f;//0x1c;
	// 16 palettes	
		colAddr[(Bank >> 2) + Offset] = RGB(r,g,b);
	}
}

void __fastcall VigilanteZ80Write1_d000(UINT16 a, UINT8 d)
{
		a&=0xfff;
		if(DrvVideoRam[a]!=d)
		{
			DrvVideoRam[a]=d;
			a&=~1;
			unsigned char Attr = DrvVideoRam[a + 1];
			unsigned char Colour = Attr & 0x0f;
			unsigned int Tile = DrvVideoRam[a + 0] | ((Attr & 0xf0) << 4);

			UINT16 *map = &ss_map[a]; 

			if( Colour >= 4) // pas de transp
			{
				if(a > 768)
					map[0] = map[0x2000] = Colour + (((Colour & 0x0c) == 0x0c) ?0x2000:0x0000);
				else
					map[0] = map[0x2000] = Colour + 0x2000;	
			}
			else	  // transparence
			{
				map[0] = map[0x2000] = Colour + 0x1000;
			}
			map[1] = map[0x2001] = Tile;
		}
}
#endif

UINT8 __fastcall VigilanteZ80PortRead1(UINT16 a)
{
	a &= 0xff;
	
	switch (a) {
		case 0x00: {
			return 0xff - DrvInput[0];
		}
		
		case 0x01: {
			return 0xff - DrvInput[1];
		}
		
		case 0x02: {
			return 0xff - DrvInput[2];
		}
		
		case 0x03: {
			return DrvDip[0];
		}
		
		case 0x04: {
			return DrvDip[1];
		}
		
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #1 Port Read => %02X\n"), a);
		}
	}

	return 0;
}

void __fastcall VigilanteZ80PortWrite1(UINT16 a, UINT8 d)
{
	if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
	{
		SPR_WaitEndSlaveSH();
	}
//	nCyclesDone[1]=*(unsigned int*)OPEN_CSH_VAR(SS_Z80CY);

	a &= 0xff;
	
	switch (a) {
		case 0x00: {
			DrvSoundLatch = d;


//	CZetOpen(1);
			DrvSetVector(Z80_ASSERT);

			return;
		}
		
		case 0x01: {
			// output port?
			return;
		}
		
		case 0x04: {
			DrvRomBank = d & 0x07;

#ifdef RAZE0
			z80_map_read  (0x8000, 0xbfff, DrvZ80Rom1 + 0x10000 + (DrvRomBank * 0x4000));
			z80_map_fetch (0x8000, 0xbfff, DrvZ80Rom1 + 0x10000 + (DrvRomBank * 0x4000));
#else
			CZetMapArea(0x8000, 0xbfff, 0, DrvZ80Rom1 + 0x10000 + (DrvRomBank * 0x4000));
			CZetMapArea(0x8000, 0xbfff, 2, DrvZ80Rom1 + 0x10000 + (DrvRomBank * 0x4000));
#endif
			return;
		}
		
		case 0x80: {
			DrvHorizScrollLo = d;
			return;
		}
		
		case 0x81: {
			DrvHorizScrollHi = (d & 0x01) << 8;
			return;
		}
		
		case 0x82: {
			DrvRearHorizScrollLo = d;
			return;
		}
		
		case 0x83: {
			DrvRearHorizScrollHi = (d & 0x07) << 8;
			return;
		}
		
		case 0x84: {

			if(DrvRearColour != d & 0x0d)
			{
				DrvRearColour = d & 0x0d;

				for (unsigned int i = 0; i < 16; ++i) 
				{
					unsigned char r, g, b;

					r = (4/3*(DrvPaletteRam[0x400 + 16 * DrvRearColour + i]));
					g = (4/3*(DrvPaletteRam[0x500 + 16 * DrvRearColour + i]));
					b = (4/3*(DrvPaletteRam[0x600 + 16 * DrvRearColour + i]));

					colBgAddr[i] = RGB(r,g,b);//BurnHighCol(r, g, b, 0);

					r = (4/3*(DrvPaletteRam[0x400 + 16 * DrvRearColour + 32 + i]));
					g = (4/3*(DrvPaletteRam[0x500 + 16 * DrvRearColour + 32 + i]));
					b = (4/3*(DrvPaletteRam[0x600 + 16 * DrvRearColour + 32 + i]));

					colBgAddr[16 + i] = RGB(r,g,b);//BurnHighCol(r, g, b, 0);
				}
			}
			DrvRearDisable = d & 0x40;
			return;
		}
		
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #1 Port Write => %02X, %02X\n"), a, d);
		}
	}
}

unsigned char __fastcall VigilanteZ80Read2(UINT16 a)
{
	return 0;
}

void __fastcall VigilanteZ80Write2(UINT16 a, UINT8 d)
{

}

UINT8 __fastcall VigilanteZ80PortRead2(UINT16 a)
{
	if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
	{
		SPR_WaitEndSlaveSH();
	}

	a &= 0xff;
	
	switch (a) {
		case 0x01: {
#ifdef SOUND2
			return BurnYM2151ReadStatus();
#else
			return 0;
#endif
		}
		
		case 0x80: {
			return DrvSoundLatch;
		}
		
		case 0x84: {
			return DrvSamples[DrvSampleAddress];
		}
		
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #2 Port Read => %02X\n"), a);
		}
	}

	return 0;
}
int vbt=0;
void __fastcall VigilanteZ80PortWrite2(UINT16 a, UINT8 d)
{
	a &= 0xff;
	
	switch (a) {
		case 0x00: {
#ifdef SOUND
//		FNT_Print256_2bpp((volatile Uint8 *)0x25e20000,(Uint8 *)"W SelectR",10,120);
//			BurnYM2151SelectRegister(d);
			nBurnCurrentYM2151Register = d;
#endif
			return;
		}
		
		case 0x01: {			
#ifdef SOUND
char toto[50];

//sprintf(toto,"reg %02x %02x use : %04x",nBurnCurrentYM2151Register, d,vbt++);

//		FNT_Print256_2bpp((volatile Uint8 *)0x25e20000,(Uint8 *)toto,10,120);
//			BurnYM2151WriteRegister(d);
//			YM2151WriteReg(0, nBurnCurrentYM2151Register, d);
			ym2151_w(nBurnCurrentYM2151Register, d);
#endif
			return;
		}
		
		case 0x80: {
			DrvSampleAddress = (DrvSampleAddress & 0xff00) | ((d << 0) & 0x00ff);
			return;
		}
		
		case 0x81: {
//		FNT_Print256_2bpp((volatile Uint8 *)0x25e20000,(Uint8 *)"W Samp81",10,120);
			DrvSampleAddress = (DrvSampleAddress & 0x00ff) | ((d << 8) & 0xff00);
			return;
		}
		
		case 0x82: {
#ifdef SOUND

	if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
	{
		SPR_WaitEndSlaveSH();
	}

//			DACSignedWrite(0, d);
#endif
			DrvSampleAddress = (DrvSampleAddress + 1) & 0xffff;
			return;
		}
		
		case 0x83: {
			DrvSetVector(Z80_CLEAR);
			return;
		}
		
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #2 Port Write => %02X, %02X\n"), a, d);
		}
	}
}
#ifdef SOUND
/*static*/void VigilantYM2151IrqHandler(INT32 Irq)
{
	if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
	{
		SPR_WaitEndSlaveSH();
	}
//	nCyclesDone[1]=*(unsigned int*)OPEN_CSH_VAR(SS_Z80CY);
	CZetOpen(1);

	if (Irq) {
		DrvSetVector(YM2151_ASSERT);
	} else {
		DrvSetVector(YM2151_CLEAR);
	}
}

//int aaa = 10;
/*static*/ INT32 VigilantSyncDAC()
{
//#ifdef CZET_SLAVE
/*		if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
		{
			SPR_WaitEndSlaveSH();
		}
//		nCyclesDone[1]=*(unsigned int*)OPEN_CSH_VAR(SS_Z80CY);
//		CZetOpen(1);
#endif
//	if(aaa < 200)
	{
		FNT_Print256_2bpp((volatile Uint8 *)0x25e20000,(Uint8 *)itoa(
			
		(INT32)(float)	(nBurnSoundLen * (nCyclesDone[1] / ((nCPUClockspeed) / (6000 / 100.0000))))
		),10,20);	

		
		FNT_Print256_2bpp((volatile Uint8 *)0x25e20000,(Uint8 *)itoa(
			
		(INT32)(float)	(nBurnSoundLen * (CZetTotalCycles() / ((nCPUClockspeed) / (6000 / 100.0000))))
		),80,20);	
		
		
		
	//	aaa+=10;
	}*/
	return 		(INT32)(float)	(nBurnSoundLen * (CZetTotalCycles() / ((nCPUClockspeed) / (600000.00))))
	;
}
#endif
/*static*/INT32 DrvInit()
{
	DrvInitSaturn();

	INT32 nRet = 0;
/*static*/UINT32 CharPlaneOffsets[4]         = { 0x80000, 0x80004, 0, 4 };
/*static*/UINT32 CharXOffsets[8]             = { 0, 1, 2, 3, 64, 65, 66, 67 };
/*static*/UINT32 CharYOffsets[8]             = { 0, 8, 16, 24, 32, 40, 48, 56 };
/*static*/UINT32 SpritePlaneOffsets[4]       = { 0x200000, 0x200004, 0, 4 };
/*static*/UINT32 SpriteXOffsets[16]          = { 0, 1, 2, 3, 128, 129, 130, 131, 256, 257, 258, 259, 384, 385, 386, 387 };
/*static*/UINT32 SpriteYOffsets[16]          = { 0, 8, 16, 24, 32, 40, 48, 56, 64, 72, 80, 88, 96, 104, 112, 120 };
/*static*/UINT32 BackTilePlaneOffsets[4]     = { 0, 2, 4, 6 };
/*static*/UINT32 BackTileXOffsets[32]        = { 1, 0, 9, 8, 17, 16, 25, 24, 33, 32, 41, 40, 49, 48, 57, 56, 65, 64, 73, 72, 81, 80, 89, 88, 97, 96, 105, 104, 113, 112, 121, 120 };
/*static*/UINT32 BackTileYOffsets[1]         = { 0 };

	// Allocate and Blank all required memory
	MemIndex();

	vbmap[0] = vb_buffer + (0x1000*0);
	vbmap[1] = vb_buffer + (0x1000*1);
	vbmap[2] = vb_buffer + (0x1000*2);
	vbmap[3] = vb_buffer + (0x1000*3);

	UINT8 *DrvTempRom = (UINT8 *)(0x00200000);
FNT_Print256_2bpp((volatile Uint8 *)0x25e20000,(Uint8 *)"load rom                 ",10,100);
	// Load Z80 #1 Program Roms
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x00000,  0, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x10000,  1, 1); if (nRet != 0) return 1;
	
	// Load Z80 #2 Program Roms
	nRet = BurnLoadRom(DrvZ80Rom2 + 0x00000,  2, 1); if (nRet != 0) return 1;
	
	// Load and decode the chars
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  3, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000,  4, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(0x1000, 4, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, DrvChars);

	// Load and decode the sprites
	memset(DrvTempRom, 0, 0x80000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  5, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000,  6, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000,  7, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x30000,  8, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x40000,  9, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x50000, 10, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x60000, 11, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x70000, 12, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(0x1000, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x200, DrvTempRom, DrvSprites);

	// Load and decode the bg tiles
	memset(DrvTempRom, 0, 0x80000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000, 13, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000, 14, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000, 15, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(0x4000, 4, 32, 1, BackTilePlaneOffsets, BackTileXOffsets, BackTileYOffsets, 0x80, DrvTempRom, DrvBackTiles);

	Bitmap2Tile(DrvBackTiles);
	
	// Load sample Roms
	nRet = BurnLoadRom(DrvSamples + 0x00000, 16, 1); if (nRet != 0) return 1;
FNT_Print256_2bpp((volatile Uint8 *)0x25e20000,(Uint8 *)"load rom done                ",10,100);
	// Setup the Z80 emulation
#ifndef RAZE1	
	CZetInit2(2,CZ80Context);
#endif

#ifdef RAZE0
	z80_init_memmap();
//	z80_add_read(0x0000, 0xffff, 1, (void *)&VigilanteZ80Read1); 	   // inutile
//	z80_add_write(0x0000, 0xffff, 1, (void *)&VigilanteZ80Write1);
	z80_add_write(0xc020, 0xc0df, 1, (void *)&VigilanteZ80Write1_c020);
	z80_add_write(0xc800, 0xcfff,  1, (void *)&VigilanteZ80Write1_c800);
	z80_add_write(0xd000, 0xdfff,  1, (void *)&VigilanteZ80Write1_d000);

	z80_set_in((void (*)(unsigned short int, unsigned char))&VigilanteZ80PortRead1);
	z80_set_out((void (*)(unsigned short int, unsigned char))&VigilanteZ80PortWrite1);
 
	z80_map_read  (0x0000, 0x7fff, DrvZ80Rom1);
	z80_map_fetch (0x0000, 0x7fff, DrvZ80Rom1);
	z80_map_read  (0x8000, 0xbfff, DrvZ80Rom1 + 0x10000);
	z80_map_fetch (0x8000, 0xbfff, DrvZ80Rom1 + 0x10000);
	z80_map_read  (0xc800, 0xcfff, DrvPaletteRam);
	z80_map_fetch (0xc800, 0xcfff, DrvPaletteRam);
	z80_map_read (0xd000, 0xdfff, DrvVideoRam);
	z80_map_fetch (0xd000, 0xdfff, DrvVideoRam);
	z80_map_read  (0xe000, 0xefff, DrvZ80Ram1);
	z80_map_write (0xe000, 0xefff, DrvZ80Ram1);
	z80_map_fetch (0xe000, 0xefff, DrvZ80Ram1);
	z80_end_memmap();
#endif
	
#ifndef RAZE1
	CZetOpen(1);
	CZetSetReadHandler(VigilanteZ80Read2);
	CZetSetWriteHandler(VigilanteZ80Write2);
	CZetSetInHandler(VigilanteZ80PortRead2);
	CZetSetOutHandler(VigilanteZ80PortWrite2);	
	CZetMapArea(0x0000, 0xbfff, 0, DrvZ80Rom2             );
	CZetMapArea(0x0000, 0xbfff, 2, DrvZ80Rom2             );
	CZetMapArea(0xf000, 0xffff, 0, DrvZ80Ram2             );
	CZetMapArea(0xf000, 0xffff, 1, DrvZ80Ram2             );
	CZetMapArea(0xf000, 0xffff, 2, DrvZ80Ram2             );
	CZetMemEnd();
//	CZetClose();
#endif

	nCyclesTotal[0] = nCPUClockspeed / 55;
	nCyclesTotal[1] = nCPUClockspeed / 55;
	
#ifdef SOUND2
//	BurnYM2151Init(3579645);
	YM2151Init(1, nCPUClockspeed, nBurnSoundRate);	 //11025);//
	BurnYM2151SetIrqHandler(&VigilantYM2151IrqHandler);	

//	DACInit(0, 0, 1, VigilantSyncDAC);
//	DACInit(0, 0, 0, VigilantSyncDAC);
//	DACSetRoute(0, 0.45, BURN_SND_ROUTE_BOTH);
#endif
FNT_Print256_2bpp((volatile Uint8 *)0x25e20000,(Uint8 *)"DrvDoReset                 ",10,100);
	DrvDoReset();

	return 0;
}

void xxx(int *i)
{
	const int cyc = nCPUClockspeed / 55 / nInterleave;

	z80_emulate(cyc);
	if (i[0] == (nInterleave - 1)) 
	{
		z80_raise_IRQ(0);
		z80_emulate(1);
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/void DrvFrame()
{
//FNT_Print256_2bpp((volatile Uint8 *)0x25e20000,(Uint8 *)"DrvFrame                 ",10,100);
//		VigilantYM2151IrqHandler(0);
	DrvMakeInputs();
	nCyclesDone[0] = nCyclesDone[1] = /*SS_Z80CY =*/ 0;

	CZetNewFrame();

	for (UINT32 i = 0; i < nInterleave; ++i) 
	{
		// Run Z80 #1
#ifdef RAZE0
//		nNext = (i + 1) * nCyclesTotal[0] / nInterleave;
//		nCyclesSegment = nNext - nCyclesDone[0];
		int nCyclesSegment = nCyclesTotal[0] / nInterleave;

		xxx(&i);
//	SPR_RunSlaveSH((PARA_RTN*)xxx,&i);
/*		nCyclesDone[0] += z80_emulate(nCyclesSegment);

		if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
		{
			//SPR_WaitEndSlaveSH();
		}

		if (i == (nInterleave - 1)) 
		{
			z80_raise_IRQ(0);
			z80_emulate(1);
		}
*/
#endif

#if CZET_SLAVE

		CZetOpen(1);

			int nNext = (i + 1) * nCyclesTotal[1] / nInterleave;
			int nCyclesSegment2 = nNext - nCyclesDone[1];
//	FNT_Print256_2bpp((volatile Uint8 *)0x25e20000,(Uint8 *)"CZetRun                 ",10,100);			
			nCyclesDone[1] += CZetRun(nCyclesSegment2);


		if (i & 1) 
		{
			CZetNmi();
		}
/*
	if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
	{
		SPR_WaitEndSlaveSH();
	}		
*/
		CZetOpen(1);
//		YM2151UpdateOneSlave();

#endif
	}
#ifdef SOUND2

	volatile signed short *	pBurnSoundOut = (signed short *)0x25a24000;
	signed short* buffers = pBurnSoundOut + (nSoundBufferPos/2);
	DACUpdate(buffers, nBurnSoundLen);
#endif
	DrvRenderDrawSound();
	
//	VigilantYM2151IrqHandler(1);
	
	
#ifdef PONY
	_spr2_transfercommand();
	frame_x++;
	
	 if(frame_x>=frame_y)
		wait_vblank();	
#endif	
//	FNT_Print256_2bpp((volatile Uint8 *)0x25e20000,(Uint8 *)"DrvFrame end                ",10,100);
	
}

void DrvRenderDrawSound()
{
//		SPR_RunSlaveSH((PARA_RTN*)DrvDrawSprites, NULL);	
	
	DrvDrawForeground();
	DrvDrawSprites();
	if (!DrvRearDisable) 
	{
		DrvRenderBackground();
	}

#ifdef SOUND2
	if(nSoundBufferPos>=RING_BUF_SIZE/2)
	{
		unsigned int i=0;
		for (i=0;i<8;i++)
		{
			PCM_NotifyWriteSize(pcm8[i], nSoundBufferPos);
			PCM_Task(pcm8[i]); // bon emplacement
		}
		nSoundBufferPos=0;
	}
#endif	

}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colAddr             = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);//
	colBgAddr2          = (Uint16*)SCL_AllocColRam(SCL_NBG0,ON);
#ifndef BMP
	colBgAddr           = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
	for(int i=0;i<4096;i++)
		colBgAddr2[i]=0x8001;
#else
	colBgAddr           = (Uint16*)SCL_AllocColRam(SCL_NBG2,ON);
	for(int i=0;i<4096;i++)
		colBgAddr2[i]=0x8001;

	SCL_AllocColRam(SCL_NBG3,OFF); // inutile ?

	SCL_AllocColRam(SCL_NBG1,OFF);
	SCL_SetColRam(SCL_NBG1,8,8,palette);
#endif
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/void initLayers()
{
    Uint16	CycleTb[]={
		0x4526, 0xefff, //A0
		0xffff, 0xffff,	//A1
		0x0ff6,0xeeee,   //B0
		0xffff, 0xeeee  //B1
	};
// nbg0 map b1+ tile a0
// nbg1 bitmap dans a1
// nbg2 tile dans	 a1,b0 et map dans a1
//4 tiles or bitmap
//0 map
 	SclConfig	scfg;

	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD;
	scfg.platesize     = SCL_PL_SIZE_2X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;
	scfg.datatype      = SCL_CELL;
		scfg.plate_addr[0] = (Uint32)ss_map;
		scfg.plate_addr[1] = (Uint32)ss_map;
		scfg.plate_addr[2] = (Uint32)ss_map;
		scfg.plate_addr[3] = (Uint32)ss_map;
	SCL_SetConfig(SCL_NBG0, &scfg);

	scfg.dispenbl      = ON;
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.plate_addr[0] = (Uint32)ss_map2;
	scfg.plate_addr[1] = (Uint32)ss_map2;
	scfg.plate_addr[2] = (Uint32)ss_map2;
	scfg.plate_addr[3] = (Uint32)ss_map2;
#ifndef BMP
	SCL_SetConfig(SCL_NBG1, &scfg);
#else
	SCL_SetConfig(SCL_NBG2, &scfg);
	scfg.dispenbl      = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
		scfg.plate_addr[0] = (Uint32)SS_FONT;
	SCL_SetConfig(SCL_NBG1, &scfg);
#endif
	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------

void PrecalcBgMap(void)
{
#ifndef BMP
	int tile_start = 0x1000;
#else
	int tile_start = 0x1800;
#endif

	for(int i=0;i<0x1000;i+=2)
	{
		if( ((i>>1)/64)*8 <128)
			ss_map2[i]=0;
		else
			ss_map2[i]=1;

		int	sx = (i>>1) & 0x3f;//% 32;

		if(sx>=48)
		{
			vbmap[0][(i>>1)]    =vbmap[0][(i>>1)-48]+0x180;     // ok
			vbmap[1][(i>>1)]    = tile_start+(i>>1)+0x180;		  // ok
			vbmap[1][(i>>1)-48] = tile_start+(i>>1)+0x180;		  // ok
		}
		else
		{
			vbmap[0][(i>>1)]    = tile_start+(i>>1)+0x180;		  // ok
		}

		if(sx<16)
		{
//			vbmap[2][(i>>1)+32] = 0x1000+(64*32*2)+(i>>1);// ok // fin de map 2, 32 a 48 // ou prendre tout ?
		}
		if(sx>=16) // &&
		{
			vbmap[3][(i>>1)-16]   = tile_start+(64*32*2)+(i>>1)+0x180; // map 3 de 0 a 48
//			if(sx<32)
//				vbmap[3][(i>>1)+32]= vbmap[3][(i>>1)-16]; // map 3 de 48 a 64
		}

		if(sx<32)
		{
			vbmap[1][(i>>1)+16] = tile_start+(64*32*1)+(i>>1)+0x180;// ok
			vbmap[2][(i>>1)+32] = tile_start+(64*32*2)+(i>>1)+0x180;
//			vbmap[2][(i>>1)+32] = (64*32*2)+(i>>1);// ok
//			vbmap[3][(i>>1)]    = (64*32*3)+(i>>1);
		}
		else
		{
			vbmap[2][(i>>1)-32] = tile_start+(64*32*1)+(i>>1)+0x180;// ok // d?but map 2, 0 a 32
			if(sx<48)
				vbmap[2][(i>>1)+16] = vbmap[2][(i>>1)-32]; // ok // map 2, 48 a 64
//			vbmap[3][(i>>1)-32] = (64*32*2)+(i>>1);// ok
		}
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------

void Bitmap2Tile(unsigned char *DrvBackTiles)
{
	char table[8*256];
	for (int m=0;m<0x30000;m+=8*256)
	{
		memcpy(table,&DrvBackTiles[m],8*256);

		for (int l=0; l<256; l+=4)
		{
			for (int k=0;k<256*8 ; k+=256)
			{
				DrvBackTiles[m+0+l*8+k/64]=table[0+k+l];
				DrvBackTiles[m+1+l*8+k/64]=table[1+k+l];
				DrvBackTiles[m+2+l*8+k/64]=table[2+k+l];
				DrvBackTiles[m+3+l*8+k/64]=table[3+k+l];
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/void DrvInitSaturn()
{
	ym2151_init();
	SPR_InitSlaveSH();
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
	nSoundBufferPos=0;
	nBurnSprites = 27;
	nBurnLinescrollSize = 0x380;

	SS_MAP  = ss_map  =(Uint16 *)SCL_VDP2_VRAM_B1;
	SS_MAP2 = ss_map2 = (Uint16 *)SCL_VDP2_VRAM_A1+0x8000;
#ifdef BMP
	SS_FONT = (SCL_VDP2_VRAM_A1+0x00000); //(Uint16 *)SCL_VDP2_VRAM_A1;//(Uint16 *)SCL_VDP2_VRAM_B0;
#endif
	SS_CACHE= (Uint8 *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;
	ss_OtherPri     = (SclOtherPriRegister *)SS_OTHR;
	ss_BgColMix		= (SclBgColMixRegister *)SS_BGMIX;
	ss_sprite		= (SprSpCmd *)SS_SPRIT;
//	ss_vram			= (UINT8 *)SS_SPRAM;
	ss_scl			= (Fixed32 *)SS_SCL;

	SS_SET_S0PRIN(4);
	SS_SET_N0PRIN(4);
#ifndef BMP
	SS_SET_N2PRIN(6);
	SS_SET_N1PRIN(2);
#else
	SS_SET_N2PRIN(2);
	SS_SET_N1PRIN(6);
#endif

	SS_SET_N0SPRM(1);  // 1 for special priority

	SS_SET_CCRTMD(0); // color calc ratio mode bit
	SS_SET_CCMD(0);   // color calc mode bit

	SS_SET_N0CCEN(1); // color calc enable
	SS_SET_N0SCCM(2); // color calc mode 1 on tile, 2 by dot, 3 color data with msb
	SS_SET_N0CCRT(23); // color calc ratio

	ss_regs->specialcode=0x0001; // sfcode, upper 8bits, function b, lower 8bits function a
	ss_regs->specialcode_sel=0; // sfsel, bit 0 for nbg0 // 1 sfcs, bit 0 = 1 for funcion code b, 0 for function code a

    /* Enable line and vertical cell scroll for NBG0 */
	(*(Uint16 *)0x25F8009A) = 0x0003; 
	(*(Uint16 *)0x25F80020) = 0x0303;

	initLayers();
	initColors();
	initSprites(352-1,224-1,0,0,0,0);
	initScrolling(ON,SCL_VDP2_VRAM_B1+(0x20000-0xC00));
	drawWindow(0,224,240,0,64);

	Uint16 *vbt = ((Uint16*)ss_scl);

	for (int Offset = 0; Offset < 96; Offset += 2) 
	{
		vbt[Offset]=128;
	}

	SprSpCmd *ss_spritePtr;
	
	for (int i = 3; i <27; i++) 
	{
		ss_spritePtr				= &ss_sprite[i];
		ss_spritePtr->control   = ( JUMP_NEXT | FUNC_NORMALSP);
		ss_spritePtr->drawMode   = ( COLOR_0 | ECD_DISABLE | COMPO_REP);		
//		ss_spritePtr->charSize  = 0x210;  //0x100 16*16
	}

	PrecalcBgMap();
	
#ifdef PONY
	frame_x	= 0;
//	nBurnFunction = sdrv_stm_vblank_rq;
#endif	
	
//	Set8PCM();
#ifdef SOUND
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
#endif
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
#ifdef SOUND2
//-------------------------------------------------------------------------------------------------------------------------------------
static void Set8PCM()
{
	PcmCreatePara	para[8];
	PcmInfo 		info[8];
	PcmStatus	*st;
	static PcmWork g_movie_work[8];

	for (int i=0; i<8; i++)
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
		pcm8[i] = createHandle(&para[i]);

		PCM_SetPcmStreamNo(pcm8[i], i);

		PCM_SetInfo(pcm8[i], &info[i]);
		PCM_ChangePcmPara(pcm8[i]);

		PCM_MeSetLoop(pcm8[i], 0);//SOUNDRATE*120);
		PCM_Start(pcm8[i]);
	}
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ INT32 DrvExit()
{
	SPR_InitSlaveSH();
	CZetExit2();

#ifdef SOUND2
	for(int i=0;i<8;i++)
	{
		PCM_MeStop(pcm8[i]);
		memset(SOUND_BUFFER+(0x4000*(i+1)),0x00,RING_BUF_SIZE*8);
	}
//	BurnYM2151Exit();
	BurnYM2151SetIrqHandler(NULL);
//	BurnYM2151SetPortHandler(NULL);
	YM2151Shutdown();
	DACExit();
#endif

	CZ80Context = NULL;
	MemEnd = DrvZ80Rom1 = DrvZ80Rom2 = DrvZ80Ram1 = DrvZ80Ram2 = NULL;
	DrvVideoRam = DrvSpriteRam = DrvPaletteRam = DrvChars = DrvBackTiles = DrvSprites = NULL;
	DrvSamples = CZ80Context = NULL;
	vbmap[0] = vbmap[1] = vbmap[2] = vbmap[3] = NULL;
	/*DrvPalette =*/ //lBuffer = NULL;
	vb_buffer = NULL;

	DrvRomBank = 0;
	DrvSoundLatch = 0;
	DrvIrqVector = 0;
	DrvRearColour = 0;
	DrvRearDisable = 0;
	DrvHorizScrollLo = 0;
	DrvHorizScrollHi = 0;
	DrvRearHorizScrollLo = 0;
	DrvRearHorizScrollHi = 0;
	DrvSampleAddress = 0;
	
//	DrvHasYM2203 = 0;
//	DrvKikcubicDraw = 0;

	//cleanDATA();
	cleanBSS();

	nSoundBufferPos=0;
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/void DrvRenderBackground()
{
	int Scroll = 0x17a - (DrvRearHorizScrollLo + DrvRearHorizScrollHi);
	if (Scroll > 0) Scroll -= 2048;
//	SCL_Open();
#ifndef BMP
	ss_reg->n1_move_x = -Scroll<<16;
#else
	ss_reg->n2_move_x = -Scroll;
#endif

	if (bg!=-Scroll/384)
	{
		bg=-Scroll/384;
		unsigned int *map1 = (unsigned int *)(vbmap[bg]); //+0x180);

		for (unsigned int Offset = 0x300; Offset < 0x1000;Offset+=16) 
		{
			UINT16 *map2 = &ss_map2[Offset];
//			map2[0] = 0; // couleur pr�calcul�e
			map2[1] = *map1++;
			map2[3] = *map1++;
			map2[5] = *map1++;
			map2[7] = *map1++;
			map2[9] = *map1++;
			map2[11] = *map1++;
			map2[13] = *map1++;
			map2[15] = *map1++;
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvDrawForeground()
{
	INT16 Scroll = ((DrvHorizScrollLo + DrvHorizScrollHi)& 0x1ff) +128;
	if(oldScroll!=Scroll)
	{
		INT16 ScrollBg = 0x17a - (DrvRearHorizScrollLo + DrvRearHorizScrollHi);
		if (ScrollBg > 0) Scroll -= 2048;
		oldScroll=Scroll;
		memset4_fast(&ss_scl[48],Scroll | (Scroll<<16),0x2C0);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/void DrvDrawSprites()
{
	SprSpCmd *ss_spritePtr;
	ss_spritePtr = &ss_sprite[3];

	for (UINT32 i = 0; i < 0xc0; i += 8) 
	{
		int sx, sy, h;

		UINT32 Code = DrvSpriteRam[i + 4] | ((DrvSpriteRam[i + 5] & 0x0f) << 8);
		sx = (DrvSpriteRam[i + 6] | ((DrvSpriteRam[i + 7] & 0x01) << 8));
		sy = 384 - (DrvSpriteRam[i + 2] | ((DrvSpriteRam[i + 3] & 0x01) << 8));
		h = 1 << ((DrvSpriteRam[i + 5] & 0x30) >> 4);
		sy -= (h<<4);

		Code &= ~(h - 1);

		ss_spritePtr->ax = sx-128;
		ss_spritePtr->ay = sy;
		ss_spritePtr->color         = (DrvSpriteRam[i + 0] & 0x0f)<<4;
		ss_spritePtr->control      = (DrvSpriteRam[i + 5] & 0xC0)>>2;
		ss_spritePtr->charSize   = 0x200|(h<<4);  //0x100 16*16
		ss_spritePtr->charAddr  = 0x220+(Code<<4);
		ss_spritePtr++;
	}
}

#undef VECTOR_INIT
#undef YM2151_ASSERT
#undef YM2151_CLEAR
#undef Z80_ASSERT
#undef Z80_CLEAR