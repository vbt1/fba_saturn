// FB Alpha Side Arms driver module
// Based on MAME driver by Paul Leaman
//#define DEBUG 1
#include "SEGA_INT.H"
#include "SEGA_DMA.H"
#include "machine.h"
#include "d_sidarm.h"

//#define nInterleave 278/2
#define nInterleave 278
#define nCyclesTotal 4000000 / 60
#define nSegment (nCyclesTotal / nInterleave)
//#define RAZE 1

// idée de Dink : tester le starfield qui est dans ce commit :
// https://github.com/dinkc64/FBAlphaFB/commit/e70c09bcce851c8ece3d7c48f2bce0233394b333
// version améliorée
// https://github.com/dinkc64/FBAlphaFB/commit/ddd14379a7aa37e356e40067b0b8186b96947836

int ovlInit(char *szShortName)
{
	cleanBSS();

	struct BurnDriver nBurnDrvSidearms = {
		"sidarm", NULL,
		"Side Arms - Hyper Dyne (World, 861129)",
		sidearmsRomInfo, sidearmsRomName, SidearmsInputInfo, SidearmsDIPInfo,
		SidearmsInit, DrvExit, DrvFrame
	};

//	if (!strcmp(nBurnDrvSidearms.szShortName, szShortName)) 
	memcpy(shared,&nBurnDrvSidearms,sizeof(struct BurnDriver));

	ss_reg   = (SclNorscl *)SS_REG;
	ss_regs = (SclSysreg *)SS_REGS;
	ss_regd = (SclDataset *)SS_REGD;
}

/*static*/ void palette_write(INT32 offset)
{
	offset &= 0x3ff;
	UINT16 data = ((DrvPalRAM[offset + 0x400] * 256) + DrvPalRAM[offset]);

	if(offset >=0x300)
	{
// fg	 offset 300
		unsigned short position = remap4to16_lut[offset&0xff];
		colBgAddr2[position] = cram_lut[data];
		if(offset >=0x378)
		{
			colBgAddr[offset] = cram_lut[data];
			colBgAddr2[offset+0x88] = cram_lut[data];
		}

	}
	else
	{
		colBgAddr[offset] = cram_lut[data];
	}
}

/*static*/ void bankswitch(UINT32 data)
{
	UINT8 bank_data = data & 0x0f;

//	ZetMapMemory(DrvZ80ROM0 + 0x8000 + (bank_data * 0x4000), 0x8000, 0xbfff, MAP_ROM);
//	CZetMapArea(0xc000, 0xcfff, 0, DrvBgRAM + nBank);
#ifdef RAZE
	z80_map_read(0x8000, 0xbfff, DrvZ80ROM0 + 0x8000 + (bank_data * 0x4000));
	z80_map_fetch(0x8000, 0xbfff, DrvZ80ROM0 + 0x8000 + (bank_data * 0x4000));
#else
	CZetMapArea (0x8000, 0xbfff, 0, DrvZ80ROM0 + 0x8000 + (bank_data * 0x4000));
	CZetMapArea (0x8000, 0xbfff, 2, DrvZ80ROM0 + 0x8000 + (bank_data * 0x4000));
#endif
}

/*static*/ void __fastcall sidearms_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xc000) {

		if(DrvPalRAM[address & 0x7ff] != data)
		{
			DrvPalRAM[address & 0x7ff] = data;
			palette_write(address);
		}
		return;
	}

	if (address >= 0xd000 && address <= 0xd7ff) 
	{
		address &= 0x7ff;
		if(DrvVidRAM[address] != data)
		{
			DrvVidRAM[address] = data;
			UINT32 attr  =	DrvVidRAM[address | 0x800];
			UINT32 code  = data  | ((attr & 0xc0) << 2);

			UINT32 x = map_offset_lut[(address & 0x7ff)];
			ss_map[x] = attr & 0x3f;
			ss_map[x+1] =code; 
		}
		return;
	}	
	
	if (address >= 0xd800 && address <= 0xdfff) 
	{
		address &= 0xfff;
		if(DrvVidRAM[address] != data)
		{
			DrvVidRAM[address] = data;
			UINT32 code  = DrvVidRAM[address & 0x7ff] | ((data & 0xc0) << 2);

			UINT32 x = map_offset_lut[(address & 0x7ff)];
			ss_map[x] = data & 0x3f;
			ss_map[x+1] =code;
		}
		return;
	}	

	switch (address)
	{
		case 0xc800:
			if(current_pcm!=data) 
			{
				if (data==0 || (data >=0x20 && data <=0x36))
					PlayStreamPCM(data,current_pcm);
				current_pcm = data;
			}
		return;

		case 0xc801:
			bankswitch(data);
		return;

		case 0xc802:
			enable_watchdog = 1;
			watchdog = 0;
		return;

		case 0xc804:
		{
			// coin counters data & 0x01, data & 0x02
			// coin lockout data & 0x04, data & 0x08
#if 0
			if (data & 0x10) {
				CZetClose();
				CZetOpen(1);
				CZetReset();
				CZetClose();
				CZetOpen(0);
			}
#endif
			if (starfield_enable != (data & 0x20)) {
				starfield_enable = data & 0x20;

				ss_regs->dispenbl &= 0xfffe;
				ss_regs->dispenbl |= (starfield_enable >> 5) & 0x0001;
				BGON = ss_regs->dispenbl;

//				hflop_74a = 1;
				starscrollx = starscrolly = 0;
			}
		}
		return;

		case 0xc805:
		{
			starscrollx++;
			starscrollx &= 0x1ff;
			// According to MAME this is correct, but for some reason in
			// FBA it is seizure-inducing. Just disable it for now.
		//	if (starscrollx && (~last & 0x100)) hflop_74a ^= 1;
		}
		return;

		case 0xc806:
			starscrolly++;
			starscrolly &= 0xff;
		return;

		case 0xc808:
		case 0xc809:
			bgscrollx[address & 1] = data;
		return;

		case 0xc80a:
		case 0xc80b:
			bgscrolly[address & 1] = data;
		return;

		case 0xc80c:
			sprite_enable = data & 0x01;
			bglayer_enable = data & 0x02;

			ss_regs->dispenbl &= 0xfffD;
			ss_regs->dispenbl |= bglayer_enable;
			BGON = ss_regs->dispenbl;
		return;
	}
}

/*static*/ UINT8 __fastcall sidearms_main_read(UINT16 address)
{
	switch (address)
	{
		case 0xc800:
		case 0xc801:
		case 0xc802:
			return DrvInputs[address & 3];

		case 0xc803:
		case 0xc804:
			return DrvDips[address - 0xc803];

		case 0xc805:
			return ((vblank) ? 0 : 0x80);
	}

	return 0;
}
#ifdef SOUND
/*static*/ void __fastcall sidearms_sound_write(UINT16 address, UINT8 data)
{
	switch (address)
	{
		case 0xf000:
		case 0xf001:
		case 0xf002:
		case 0xf003:
//			BurnYM2203Write((address/2)&1, address & 1, data);
		return;
	}
}

/*static*/ UINT8 __fastcall sidearms_sound_read(UINT16 address)
{
	switch (address)
	{
		case 0xd000:
			return current_pcm;

		case 0xf000:
		case 0xf001:
		case 0xf002:
		case 0xf003:
//			return BurnYM2203Read((address/2)&1, address & 1);
		return  0;
	}

	return 0;
}
#endif

/*static*/ INT32 DrvDoReset(INT32 clear_mem)
{
	if (clear_mem) {
		memset (DrvVidRAM, 0, RamEnd - DrvVidRAM);
	}
#ifdef RAZE
	z80_reset();
#else
	CZetOpen(0);
	CZetReset();
	CZetClose();
#endif
#ifdef SOUND
	ZetOpen(1);
	ZetReset();
	CZetClose();
#endif
	enable_watchdog = 0;
	watchdog = 0;

	current_pcm = 0;
	sprite_enable = 0;
	bglayer_enable = 0;

	starfield_enable = 0;
	starscrollx = 0;
	starscrolly = 0;
	
	UINT8 *ss_vram		= (UINT8 *)SS_SPRAM;
	memset4_fast((UINT8 *)(ss_vram+0x43100),0x00,0x3E000);

	for (UINT32 i = 0; i < 0x400; i++) 
	{
		palette_write(i);
	}
	return 0;
}

/*static*/ INT32 MemIndex()
{
	UINT8 *Next; 
	Next			= AllMem;
	CZ80Context		= Next; Next += sizeof(cz80_struc);
	DrvZ80ROM0		= Next; Next += 0x018000;
#ifdef SOUND
	DrvZ80ROM1		= Next; Next += 0x008000;
#endif
	DrvStarMap		= Next; Next += 0x008000;
//	DrvStarMap		= 0x00200000;
	DrvTileMap		= Next; Next += 0x008000;
//	DrvTileMap		= 0x00208000;
//	AllRam			= Next;

	DrvVidRAM		= Next; Next += 0x001000;
	DrvSprBuf		= Next; Next += 0x001000;
	DrvSprRAM		= Next; Next += 0x001000;
	DrvPalRAM		= Next; Next += 0x000800;
	DrvZ80RAM0		= Next; Next += 0x002000;
#ifdef SOUND
	DrvZ80RAM1		= Next; Next += 0x000800;
#endif
	bgscrollx		= Next; Next += 0x000002;
	bgscrolly		= Next; Next += 0x000002;

	RamEnd			= Next;

	bgmap_lut	 	= Next; Next += 0x020000 * sizeof (UINT16);
//
	bgmap_buf		= Next; Next += 0x800 * sizeof (UINT32);//bgmap_lut + 0x20000;
//	bgmap_buf		= bgmap_lut + 0x20000;

	remap4to16_lut	= Next; Next += 256 * sizeof (UINT16);
	map_lut			= Next; Next += 256 * sizeof (UINT16);
	map_offset_lut	= Next; Next += 8192 * sizeof (UINT16);
	cram_lut		= Next; Next += 4096 * sizeof (UINT16);
//	charaddr_lut	= Next; Next += 0x800 * sizeof (UINT16);
//	MemEnd			= Next;

	return 0;
}

/*static*/ void DrvGfxDecode()
{
	UINT8 *ss_vram		= (UINT8 *)SS_SPRAM;
	UINT32 Plane0[2]  = { 4, 0 };
	UINT32 Plane1[4]  = { 0x200000 + 4, 0x200000 + 0, 4, 0 };
	UINT32 Plane2[4]  = { 0x100000 + 4, 0x100000 + 0, 4, 0 };
	UINT32 XOffs0[16] = { STEP4(0,1), STEP4(8,1), STEP4(256,1), STEP4(256+8,1) };
	UINT32 XOffs1[32] = { STEP4(0,1), STEP4(8,1), STEP4(512,1), STEP4(512+8,1), STEP4(1024,1), STEP4(1024+8,1), STEP4(1536,1), STEP4(1536+8,1) };
	UINT32 YOffs[32]  = { STEP32(0,16) };
	UINT8 *DrvGfxROM1	 = (UINT8 *)0x00240000;	
	UINT8 *DrvGfxROM1b	 = (UINT8 *)(SS_CACHE + 0x4000);
	UINT8 *DrvGfxROM0	 = (UINT8 *)SS_CACHE;
	UINT8 *DrvGfxROM2	 = (UINT8*)0x002C0000;//(UINT8 *)(ss_vram + 0x1100);
	UINT8 *DrvGfxROM2b	 = (UINT8*)(ss_vram + 0x1100);
	
	UINT8 *tmp = (UINT8*)0x00200000; 

	memcpyl (tmp, DrvGfxROM0, 0x04000);
// text
	GfxDecode4Bpp(0x0400, 2,  8,  8, Plane0, XOffs0, YOffs, 0x080, tmp, DrvGfxROM0);

	for (UINT32 i=0;i<0x8000;i++ )
	{
		if ((DrvGfxROM0[i]& 0x03)     ==0x00)DrvGfxROM0[i] = DrvGfxROM0[i] & 0x30 | 0x3;
		else if ((DrvGfxROM0[i]& 0x03)==0x03) DrvGfxROM0[i] = DrvGfxROM0[i] & 0x30;

		if ((DrvGfxROM0[i]& 0x30)     ==0x00)DrvGfxROM0[i] = 0x30 | DrvGfxROM0[i] & 0x03;
		else if ((DrvGfxROM0[i]& 0x30)==0x30) DrvGfxROM0[i] = DrvGfxROM0[i] & 0x03;
	}	
wait_vblank();
	
// sprites // ok
	GfxDecode4Bpp(0x0800, 4, 16, 16, Plane2, XOffs0, YOffs, 0x200, DrvGfxROM2, tmp);

	for (UINT32 i=0;i<0x40000;i++ )
	{
		if ((tmp[i]& 0x0f)     ==0x00)tmp[i] = tmp[i] & 0xf0 | 0xf;
		else if ((tmp[i]& 0x0f)==0x0f) tmp[i] = tmp[i] & 0xf0;

		if ((tmp[i]& 0xf0)       ==0x00)tmp[i] = 0xf0 | tmp[i] & 0x0f;
		else if ((tmp[i]& 0xf0)==0xf0) tmp[i] = tmp[i] & 0x0f;
	}
	memcpyl (DrvGfxROM2b, tmp, 0x40000);
// bg
	GfxDecode4Bpp(0x0400, 4, 32, 32, Plane1, XOffs1, YOffs, 0x800, DrvGfxROM1, tmp);
	tile32x32toSaturn(1,0x0200, tmp);

	for (UINT32 i=0;i<0x70000;i++ )
	{
		if ((tmp[i]& 0x0f)     ==0x00)tmp[i] = tmp[i] & 0xf0 | 0xf;
		else if ((tmp[i]& 0x0f)==0x0f) tmp[i] = tmp[i] & 0xf0;

		if ((tmp[i]& 0xf0)       ==0x00)tmp[i] = 0xf0 | tmp[i] & 0x0f;
		else if ((tmp[i]& 0xf0)==0xf0) tmp[i] = tmp[i] & 0x0f;
	}
	memcpyl (DrvGfxROM1b, tmp, 0x70000);

	wait_vblank();
}

/*static*/ INT32 SidearmsInit()
{
	DrvInitSaturn();

	AllMem = NULL;
	MemIndex();
	UINT8 *ss_vram		 = (UINT8 *)SS_SPRAM;
//	UINT8 *DrvGfxROM1	 = (UINT8 *)(SS_CACHE + 0x4000);
	UINT8 *DrvGfxROM1	 = (UINT8 *)0x00240000;
//	UINT8 *DrvGfxROM2	 = (UINT8 *)(ss_vram + 0x1100);
	UINT8 *DrvGfxROM2	 = (UINT8*)0x002C0000;//(UINT8 *)(ss_vram + 0x1100);


	if ((AllMem = (UINT8 *)BurnMalloc(MALLOC_MAX)) == NULL) 
	{
		return 1;
	}
	memset(AllMem, 0, MALLOC_MAX);
	MemIndex();
	
	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Loading. Please Wait",24,40);
	{
		if (BurnLoadRom(DrvZ80ROM0 + 0x00000,  0, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x08000,  1, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x10000,  2, 1)) return 1;
#ifdef SOUND
		if (BurnLoadRom(DrvZ80ROM1 + 0x00000,  3, 1)) return 1;
#endif
		if (BurnLoadRom(DrvStarMap + 0x00000,  4, 1)) return 1;
	
		if (BurnLoadRom(SS_CACHE + 0x00000,  5, 1)) return 1;

		if (BurnLoadRom(DrvGfxROM1 + 0x00000,  6, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x08000,  7, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x10000,  8, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x18000,  9, 1)) return 1;

		if (BurnLoadRom(DrvGfxROM1 + 0x40000, 10, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x48000, 11, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x50000, 12, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x58000, 13, 1)) return 1;

		if (BurnLoadRom(DrvGfxROM2 + 0x00000, 14, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x08000, 15, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x10000, 16, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x18000, 17, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x20000, 18, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x28000, 19, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x30000, 20, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x38000, 21, 1)) return 1;

		if (BurnLoadRom(DrvTileMap + 0x00000, 22, 1)) return 1;
		DrvGfxDecode();
	}
#ifdef RAZE
  	z80_init_memmap();

	z80_map_fetch(0x0000, 0x7fff, DrvZ80ROM0);
	z80_map_read(0x0000, 0x7fff, DrvZ80ROM0);

	z80_map_fetch(0xc000, 0xc7ff, DrvPalRAM);
	z80_map_read(0xc000, 0xc7ff, DrvPalRAM);

	z80_map_fetch(0xd000, 0xdfff, DrvVidRAM);
	z80_map_read(0xd000, 0xdfff, DrvVidRAM);

	z80_map_fetch(0xe000, 0xefff, DrvZ80RAM0);
	z80_map_read(0xe000, 0xefff, DrvZ80RAM0);
	z80_map_write(0xe000, 0xefff, DrvZ80RAM0);

	z80_map_fetch(0xf000, 0xffff, DrvSprRAM);
	z80_map_read(0xf000, 0xffff, DrvSprRAM);
	z80_map_write(0xf000, 0xffff, DrvSprRAM);

	z80_add_write(0xc800, 0xdfff, 1, (void *)&sidearms_main_write);
	z80_add_read(0xc800, 0xc805, 1, (void *)&sidearms_main_read);
	
	z80_end_memmap(); 
#else
	CZetInit2(1,CZ80Context);
	CZetOpen(0);

	CZetMapMemory(DrvZ80ROM0,	0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory(DrvPalRAM,	0xc000, 0xc7ff, MAP_ROM);
	CZetMapMemory(DrvVidRAM,	0xd000, 0xdfff, MAP_ROM);
	CZetMapMemory(DrvZ80RAM0,	0xe000, 0xefff, MAP_RAM);
	CZetMapMemory(DrvSprRAM,	0xf000,	 0xffff, MAP_RAM);
	CZetSetWriteHandler(sidearms_main_write);
	CZetSetReadHandler(sidearms_main_read);
	CZetClose();
#endif

#ifdef SOUND
	ZetInit(1);
	ZetOpen(1);
	ZetMapMemory(DrvZ80ROM1,	0x0000, 0x7fff, MAP_ROM);
	ZetMapMemory(DrvZ80RAM1,	0xc000, 0xc7ff, MAP_RAM);
	ZetSetWriteHandler(sidearms_sound_write);
	ZetSetReadHandler(sidearms_sound_read);
	ZetClose();
#endif
	DrvDoReset(1);
	make_lut();
	SS_SET_N0PRIN(3); // star field
	SclProcess = 2;  // pour activer maj
//	drawWindow(0,224,240,0,0);

	UINT8 *lineptr = (Uint8 *)0x0280000;
	UINT8 *lineptr2 = (Uint8 *)SS_FONT;

	for (UINT32 y = 0; y < 256; y++) 
	{
		for (UINT32 x = 0; x < 512; x+=2) 
		{
			UINT8 c1 = (lineptr[0]&0x0f)<<4;
			UINT8 c2 = (lineptr[1]&0x0f);
			lineptr2[0] = c1|c2;
			lineptr+=2;
			lineptr2++;
		}
	}
//-------------------------------------------------
	stmInit();
	SetStreamPCM();
	PCM_Start(pcmStream);
//-------------------------------------------------
	return 0;
}

/*static*/ INT32 DrvExit()
{
	nBurnFunction = NULL;
	wait_vblank();
	DrvDoReset(1);
//	memset(ss_map2,0x00,0x20000);
#ifdef RAZE

#else
	CZetExit2();
#endif

	STM_ResetTrBuf(stm);
	PCM_MeStop(pcmStream);
	PCM_DestroyStmHandle(pcmStream);
	stmClose(stm);

	bgmap_buf = NULL;
	cram_lut = bgmap_lut = remap4to16_lut = map_lut = map_offset_lut = NULL;
	CZ80Context = /*AllRam =*/ RamEnd = DrvZ80ROM0 = NULL;
	DrvStarMap = DrvTileMap = DrvVidRAM = DrvSprBuf = DrvSprRAM = DrvPalRAM = DrvZ80RAM0 = bgscrollx = bgscrolly = NULL;
	free (AllMem);
	AllMem = NULL;

	vblank=0;

	cleanDATA();
	cleanBSS();

	nSoundBufferPos=0;
	return 0;
}

inline void transfer_bg_layer()
{
	if(bglayer_enable)
	{
		SDMA_ScuCst(DMA_SCU_CH2,ss_map2,bgmap_buf,0x800);
		while(SDMA_ScuResult(DMA_SCU_CH2) != DMA_SCU_END);
	}
}

/*static*/ void draw_bg_layer()
{
	INT32 scrollx = ((((bgscrollx[1] << 8) + bgscrollx[0]) & 0xfff) + 64) & 0xfff;
	INT32 scrolly = ((((bgscrolly[1] << 8) + bgscrolly[0]) & 0xfff) + 16) & 0xfff; 
	UINT32 *map = (UINT32 *)bgmap_buf;

	INT32 offs = 2 * (scrollx >> 5) + 0x100 * (scrolly >> 5);

	for (UINT32 k=0;k<32 ;k++ ) // row
	{
		for (UINT32 i=0;i<32 ;i+=2 ) // colon
		{
			UINT32 offset = offs + i;
			offset = (offset & 0xf801) | ((offset & 0x0700) >> 7) | ((offset & 0x00fe) << 3) & 0x7fff;
			UINT32 *pDrvTileMap = ((UINT32 *)bgmap_lut)+(offset<<1);

			map[i+0]	= pDrvTileMap[0];
			map[i+1]	= pDrvTileMap[1];
			map[i+32]	= pDrvTileMap[2];
			map[i+33]	= pDrvTileMap[3];
		}
		offs += 256;
		map+= 64;
	}
	ss_reg->n1_move_x =  (((scrollx&0x1f)+16)<<16) ;
	ss_reg->n1_move_y =  (((scrolly&0x1f))<<16) ;
}

/*static*/ void draw_sprites_region(INT32 start, UINT32 end)
{
	UINT32 delta	= (start/32)+3;
	SprSpCmd *ss_spritePtr = &ss_sprite[delta];

	for (INT32 offs = end - 32; offs >= start; offs -= 32)
	{
		if (!DrvSprBuf[offs + 2] || DrvSprBuf[offs + 5] == 0xc3) continue;

		UINT32 attr  = DrvSprBuf[offs + 1];
		UINT32 code  = DrvSprBuf[offs] + ((attr << 3) & 0x700);
		ss_spritePtr->ax			= DrvSprBuf[offs + 3] + ((attr << 4) & 0x100);
		ss_spritePtr->ay			= DrvSprBuf[offs + 2];
		ss_spritePtr->color		= (attr & 0xf)<<4;//Colour<<4;
		ss_spritePtr->charAddr	= 0x220+(code<<4);
		*ss_spritePtr++;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ UINT32 SidearmsDraw()
{
	if (starfield_enable)
	{
		ss_reg->n0_move_x = starscrollx<<16;
		ss_reg->n0_move_y = starscrolly<<16;
	}

	if (bglayer_enable) 
	{
		transfer_bg_layer();
		SPR_RunSlaveSH((PARA_RTN*)draw_bg_layer, NULL);
	}
	else
	{
		SPR_RunSlaveSH((PARA_RTN*)dummy, NULL);
	}

	cleanSprites();

	if (sprite_enable) {
		draw_sprites_region(0x0700, 0x0800);
		draw_sprites_region(0x0e00, 0x1000);
		draw_sprites_region(0x0800, 0x0f00);
		draw_sprites_region(0x0000, 0x0700);
	}
	SPR_WaitEndSlaveSH();

	return 0;
}

/*static*/ INT32 DrvFrame()
{
	watchdog++;
	if (watchdog > 180 && enable_watchdog) {
		DrvDoReset(0);
	}
	{
		memset (DrvInputs, 0xff, 5);

		for (UINT32 i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
			DrvInputs[2] ^= (DrvJoy3[i] & 1) << i;
			DrvInputs[3] ^= (DrvJoy4[i] & 1) << i;
			DrvInputs[4] ^= (DrvJoy5[i] & 1) << i;
		}
	}
	vblank = 0;
	for (UINT32 i = 0; i < nInterleave; i++) {
#ifdef RAZE
		z80_emulate(nSegment);
z80_raise_IRQ(0);
		if (i == 274) {z80_raise_IRQ(0); vblank = 1; }
		if (i == 276) z80_lower_IRQ();
//		if (i == 137) {z80_raise_IRQ(0); vblank = 1; }
//		if (i == 138) z80_lower_IRQ();

#else
		CZetOpen(0);
		CZetRun(nSegment);
		if (i == 274) {CZetSetIRQLine(0, CZET_IRQSTATUS_ACK); vblank = 1; }
		if (i == 276) CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);
//		if (i == 137) {CZetSetIRQLine(0, CZET_IRQSTATUS_ACK); vblank = 1; }
//		if (i == 138) CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);

		CZetClose();
#endif
	}
	SidearmsDraw();
	memcpyl (DrvSprBuf, DrvSprRAM, 0x1000);
	playMusic(&pcmStream);
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initLayers()
{
    Uint16	CycleTb[]={
		0xff56, 0xffff, //A0
		0xffff, 0xffff,	//A1
		0x15f2,0x4eff,   //B0
		0xffff, 0xffff  //B1
//		0x4eff, 0x1fff, //B1
	};
 	SclConfig	scfg;

// 3 nbg
// nbg1 16*16 background
// ? passer en 1 word ?
	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_2X2;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD; //2word
	scfg.platesize     = SCL_PL_SIZE_2X2; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.patnamecontrl =  0x0008;// VRAM A0?
	scfg.flip          = SCL_PN_10BIT; // on force ? 0

				scfg.plate_addr[0] = (Uint32)(SS_MAP2);//////
				scfg.plate_addr[1] = (Uint32)(SS_MAP2);//////
				scfg.plate_addr[2] = (Uint32)(SS_MAP2+0x1000);	 // good	  0x400
				scfg.plate_addr[3] = (Uint32)(SS_MAP2+0x1000);

	SCL_SetConfig(SCL_NBG1, &scfg);
// 3 nbg
	scfg.pnamesize     = SCL_PN2WORD; //2word
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.charsize      = SCL_CHAR_SIZE_1X1;
	scfg.plate_addr[0] = (Uint32)SS_MAP;
	scfg.plate_addr[1] = (Uint32)SS_MAP;
	scfg.plate_addr[2] = (Uint32)SS_MAP;
	scfg.plate_addr[3] = (Uint32)SS_MAP;
// nbg2 8x8 foreground
	scfg.dispenbl      = ON;
	SCL_SetConfig(SCL_NBG2, &scfg);

 	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
	scfg.plate_addr[0] = (Uint32)SS_FONT;
	scfg.dispenbl      = ON;
	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_SPR,OFF);
	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_NBG0,OFF);
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
//	DMA_ScuInit();
//    INT_ChgMsk(INT_MSK_DMA0, INT_MSK_NULL);
//    INT_ChgMsk(INT_MSK_DMA1, INT_MSK_NULL);
	   INT_ChgMsk(INT_MSK_DMA2, INT_MSK_NULL);

 	SS_MAP  = ss_map		=(Uint16 *)SCL_VDP2_VRAM_B1+0xC000;		   //c
	SS_MAP2 = ss_map2	=(Uint16 *)SCL_VDP2_VRAM_B1+0x8000;			//8000
	SS_FONT = ss_font		=(Uint16 *)SCL_VDP2_VRAM_B1;
	SS_CACHE= cache		=(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum	 = (SclSpPriNumRegister *)SS_N0PRI;
	ss_SpPriNum	 = (SclSpPriNumRegister *)SS_SPPRI;
	ss_OtherPri	= (SclOtherPriRegister *)SS_OTHR;
	ss_BgColMix	= (SclBgColMixRegister *)SS_BGMIX;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_scl			= (Fixed32 *)SS_SCL;
	sfx_list			= &sfx_sidarm[0];

	nBurnLinescrollSize = 0;
	nBurnSprites = 128+3;

	SS_SET_N0PRIN(7); // star field
	SS_SET_N1PRIN(4); // bg
	SS_SET_N2PRIN(6); // fg
	SS_SET_S0PRIN(4); // sp0

	initLayers();
	initColors();
	initSprites(352-1,224-1,0,0,-80,-16); // ne plus modifier

	SCL_Open();
	ss_reg->n2_move_y =  16;
	ss_reg->n2_move_x =  80;
	SCL_Close();

	memset((Uint8 *)SCL_VDP2_VRAM_B1  ,0x22,0x8000);
	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Loading. Please Wait",24,40);
//	memset((Uint8 *)ss_map2,0x11,0x8000);
//	memset((Uint8 *)ss_map3,0,0x2000);
//	memset((Uint8 *)bg_map_dirty,1,0x4000);
	SprSpCmd *ss_spritePtr;
	
	for (unsigned int i = 3; i <nBurnSprites; i++) 
	{
		ss_spritePtr				= &ss_sprite[i];
		ss_spritePtr->control   = ( JUMP_NEXT | FUNC_NORMALSP);
		ss_spritePtr->drawMode  = ( ECD_DISABLE | COMPO_REP);	// 16 couleurs
		ss_spritePtr->charSize  = 0x210;  //0x100 16*16
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void make_lut(void)
{
    UINT32 j=0,my;
	UINT8 r,g,b;

   	for (UINT32 i = 0; i < 4096;i++) 
	{
		r = (i >> 4) & 0x0f;
		g = (i >> 0) & 0x0f;
		b = (i >> 8) & 0x0f;

		r |= r << 4;
		g |= g << 4;
		b |= b << 4;
		cram_lut[i] = BurnHighCol(r,g,b,0);
	}

	for (UINT32 i = 0; i < 256;i++) 
	{
		if ((i%4)==0)
		{
			remap4to16_lut[i] = j+3;
		}
		else if (((i+1)%4)==0)
		{
			remap4to16_lut[i] = j-3;
		} 
		else
		{
			remap4to16_lut[i] = j;
		}
		j++; if ((j & 3) == 0) j += 12;

		map_lut[i] = ((i & 0x80)<<7) | i & 0x7f;
	}

	j = 0;
	for (my = 0; my < 64; my++) 
	{
		for (UINT32 i = 0; i < 128; i++) 
		{
			map_offset_lut[j] = ((i)|(my<<7))<<1;
			j++;
		}
	}

	memset(bgmap_lut,0x00,0x8000);
	UINT16 *pbgmap_lut = &bgmap_lut[0];

	for(UINT32 i = 0; i < 0x8000; i+=2)
	{
		UINT8 *pDrvTileMap = &DrvTileMap[i];
		
		UINT32 attr		= pDrvTileMap[1];
		UINT32 code		= (pDrvTileMap[0] + ((attr & 0x01) * 256))*4;
		UINT32 color		= attr / 8;
		UINT32 flipx		= attr & 0x02;
		UINT32 flipy		= attr & 0x04;

		if(!flipx)
		{
			if(!flipy)
			{
				pbgmap_lut[0]=color;
				pbgmap_lut[1]=((code++)<<2)+0x800;
				pbgmap_lut[2]=color;
				pbgmap_lut[3]=((code++)<<2)+0x800;
				
				pbgmap_lut[4]=color;
				pbgmap_lut[5]=((code++)<<2)+0x800;
				pbgmap_lut[6]=color;
				pbgmap_lut[7]=((code++)<<2)+0x800;
			}
			else
			{
				pbgmap_lut[4]	= color | 0x8000;
				pbgmap_lut[5]	= ((code++)<<2)+0x800;
				pbgmap_lut[6]	= color | 0x8000;
				pbgmap_lut[7]	= ((code++)<<2)+0x800;
				
				pbgmap_lut[0]	= color | 0x8000;
				pbgmap_lut[1]	=	((code++)<<2)+0x800;
				pbgmap_lut[2]	= color | 0x8000;
				pbgmap_lut[3]	=	((code++)<<2)+0x800;
			}
		}
		else
		{
			if(!flipy)
			{
				pbgmap_lut[2]	= color | 0x4000;
				pbgmap_lut[3]	= ((code++)<<2)+0x800;
				pbgmap_lut[0]	= color | 0x4000;
				pbgmap_lut[1]	= ((code++)<<2)+0x800;
				
				pbgmap_lut[6]	= color | 0x4000;
				pbgmap_lut[7]	=	((code++)<<2)+0x800;
				pbgmap_lut[4]	= color | 0x4000;
				pbgmap_lut[5]	=	((code++)<<2)+0x800;
			}
			else
			{
				pbgmap_lut[6]	= color | 0xC000;
				pbgmap_lut[7]	= ((code++)<<2)+0x800;
				pbgmap_lut[4]	= color | 0xC000;
				pbgmap_lut[5]	= ((code++)<<2)+0x800;
				
				pbgmap_lut[2]	= color | 0xC000;
				pbgmap_lut[3]	=	((code++)<<2)+0x800;
				pbgmap_lut[0]	= color | 0xC000;
				pbgmap_lut[1]	=	((code++)<<2)+0x800;
			}
		}
		pbgmap_lut+=8;
	}

	UINT8 *lineptr = (Uint8 *)0x0280000;
	memset(lineptr,0x00,0x60000);

//	for (INT32 xx = 0; xx < 256; xx++)
	{
//		for (INT32 yy = 0; yy < 256; yy++)
		{
//			UINT32 _hcount_191 = xx & 0xff;
//			UINT32 _hcount_191 = 0;

			for (INT32 y = 0; y < 256; y++)
			{
				UINT32 hadd_283 = 1;//_hcount_191 & ~0x1f;
				UINT32 vadd_283 = 0 + y;

				INT32 i = (vadd_283<<4) & 0xff0;
//				i |= (hflop_74a^(hadd_283>>8)) << 3;
				i |= (1^(hadd_283>>8)) << 3;
				i |= (hadd_283>>5) & 7;
				UINT32 latch_374 = DrvStarMap[i + 0x3000];

				hadd_283 = /*_hcount_191*/ - 1;

				for (UINT32 x = 0; x < 512; lineptr++, x++)
				{
					i = hadd_283;
					hadd_283 = /*_hcount_191 +*/ (x & 0xff);
					vadd_283 = 0 + y;

					if (!((vadd_283 ^ (x>>3)) & 4)) continue;
					if ((vadd_283 | (hadd_283>>1)) & 2) continue;

					if ((i & 0x1f)==0x1f)
					{
						i  = (vadd_283<<4) & 0xff0;
//						i |= (hflop_74a^(hadd_283>>8)) << 3;
						i |= (1^(hadd_283>>8)) << 3;
						i |= (hadd_283>>5) & 7;
						latch_374 = DrvStarMap[i + 0x3000];
					}

					if ((~((latch_374^hadd_283)^1) & 0x1f)) continue;

		//			*lineptr = (UINT16)((latch_374>>5) | 0x378); // num?ro de couleur de la palette du bg > 0x300
					*lineptr = (latch_374>>5) &0xf; // vbt : essai sans offset
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void tile32x32toSaturn (unsigned char reverse, unsigned int num, unsigned char *pDest)
{
	for (unsigned int c = 0; c < num; c++) 
	{
		unsigned char tile[512];

		UINT8 *dpM = pDest + (c * 512);
		memcpyl(tile,dpM,512);
		UINT8 *dpO = &tile[0];

		for (unsigned int l=0;l<4;l++) // 4 par 4
		{
			for (unsigned int k=0;k<16;k+=4) // 4 par 4
			{
				memcpy(&dpM[0],&dpO[k],4);
				memcpy(&dpM[4],&dpO[k+16],4);
				memcpy(&dpM[8],&dpO[k+32],4);
				memcpy(&dpM[12],&dpO[k+48],4);
				memcpy(&dpM[16],&dpO[k+64],4);
				memcpy(&dpM[20],&dpO[k+80],4);
				memcpy(&dpM[24],&dpO[k+96],4);
				memcpy(&dpM[28],&dpO[k+112],4);
				dpM+=32;
			}
			dpO+=128;
		}
// reordering
		dpM = pDest + (c * 512);
		memcpyl(tile,dpM,512);
// 0&1 corrects
		memcpy(&dpM[2*32],&tile[4*32],32);
		memcpy(&dpM[3*32],&tile[5*32],32);

		memcpy(&dpM[4*32],&tile[2*32],32);
		memcpy(&dpM[5*32],&tile[3*32],32);

		memcpy(&dpM[10*32],&tile[12*32],32);
		memcpy(&dpM[11*32],&tile[13*32],32);

		memcpy(&dpM[12*32],&tile[10*32],32);
		memcpy(&dpM[13*32],&tile[11*32],32);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
