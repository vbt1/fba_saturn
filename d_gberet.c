// FB Alpha Green Beret driver module
// Based on MAME driver by Nicola Salmoria
//#define CZ80 1
//#define RAZE 1
#define CACHE 1
#include "d_gberet.h"

int ovlInit(char *szShortName)
{
	cleanBSS();

	struct BurnDriver nBurnDrvGberet = {
		"gberet", NULL,
		"Green Beret",
		gberetRomInfo, gberetRomName, GberetInputInfo, GberetDIPInfo,
		gberetInit, DrvExit, DrvFrame, DrvDraw//, NULL
	};

	struct BurnDriver nBurnDrvMrgoemon = {
		"mrgoemon", "gberet",
		"Mr. Goemon (Japan)",
		mrgoemonRomInfo, mrgoemonRomName, GberetInputInfo, MrgoemonDIPInfo,
		mrgoemonInit, DrvExit, DrvFrame, DrvDraw//, NULL
	};

    if (strcmp(nBurnDrvGberet.szShortName, szShortName) == 0)
		memcpy(shared,&nBurnDrvGberet,sizeof(struct BurnDriver));
	else
		memcpy(shared,&nBurnDrvMrgoemon,sizeof(struct BurnDriver));
	ss_reg          = (SclNorscl *)SS_REG;
}

void __fastcall gberet_write(UINT16 address, UINT8 data)
{
	if ((address & 0xffc0) == 0xe000) {
		DrvScrollRAM[address & 0x3f] = data;
		return;
	}

	switch (address)
	{
		case 0xe040:
		case 0xe041:
		case 0xe042:
		return;	// nopw

		case 0xe043:
			sprite_bank = data;
		return;

		case 0xe044:
		{
			INT32 ack_mask = ~data & irq_mask;

			if (ack_mask & 1) CZetSetIRQLine(0x20, CZET_IRQSTATUS_NONE);
			if (ack_mask & 6) CZetSetIRQLine(0x00, CZET_IRQSTATUS_NONE);

			irq_mask   = data & 0x07;
		}
		return;

		case 0xf000:
			// coin counter
		return;

		case 0xf200:
			soundlatch = data;
		return;

		case 0xf400:
			SN76496Write(0, soundlatch);
		return;

		case 0xf600:
		return;


	}

#ifdef CACHE
	if (address >= 0xc000 && address <= 0xcfff) 
//	ZetMapMemory(DrvColRAM,		0xc000, 0xc7ff, MAP_RAM);
//	ZetMapMemory(DrvVidRAM,		0xc800, 0xcfff, MAP_RAM);
	{
		if(DrvColRAM[address&0x7ff]!=data)
		{
			bg_dirtybuffer[address&0x7ff] = 1;
			DrvColRAM[address&0x7ff] = data;
		}
		return;
	}
#endif
}

UINT8 __fastcall gberet_read(UINT16 address)
{

	unsigned char nRet = 0xff;
	unsigned int i;

	switch (address)
	{
		case 0xf200:
			return DrvDips[1]; // dsw2

		case 0xf400:
			return DrvDips[2]; // dsw3

		case 0xf600:
			return DrvDips[0]; // dsw1

		case 0xf601:
			return DrvInputs[1]; // p2

		case 0xf602:
			return DrvInputs[0]; // p1

		case 0xf603:
			return DrvInputs[2]; // system
	}

	return 0;
}


void bankswitch(INT32 data)
{
	z80_bank = data | 0x80;

	CZetMapMemory(DrvZ80ROM + 0xc000 + (data & 7) * 0x800, 0xf800, 0xffff, MAP_ROM);
}

void __fastcall mrgoemon_write(UINT16 address, UINT8 data)
{
	switch (address)
	{
		case 0xf000:
			bankswitch(data >> 5);
		break; // break, not return!
	}

	gberet_write(address, data);
}

INT32 DrvDoReset(INT32 clear_mem)
{
	if (clear_mem) {
		memset (AllRam, 0, RamEnd - AllRam);
	}
#ifdef CZ80
	CZetOpen(0);
	CZetReset();
	CZetClose();
#endif

	irq_mask = 0;
	irq_timer = 0;
	sprite_bank = 0;
	z80_bank = 0;
	soundlatch = 0;

	return 0;
}

INT32 MemIndex()
{
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	UINT8 *Next; Next = AllMem;

	DrvZ80ROM		= Next; Next += 0x010000;

	DrvGfxROM0		= cache; //Next; Next += 0x008000;
	DrvGfxROM1		= &ss_vram[0x1100]; //Next; Next += 0x020000;

	DrvColPROM		= Next; Next += 0x000220;

	DrvPalette	       = (UINT16 *)colBgAddr;//(unsigned int*)Next; Next += 0x00200 * sizeof(unsigned int);

	AllRam			= Next;

	DrvZ80RAM		= Next; Next += 0x001000;
	DrvVidRAM		= Next; Next += 0x000800;
	DrvColRAM		= Next; Next += 0x000800;
	DrvSprRAM0		= Next; Next += 0x000100;
	DrvSprRAM1		= Next; Next += 0x000100;
	DrvSprRAM2		= Next; Next += 0x000200;
	DrvScrollRAM	= Next; Next += 0x000100;

	RamEnd			= Next;

	MemEnd			= Next;

	return 0;
}

void DrvGfxDecode()
{
	INT32 Plane[4]  = { STEP4(0,1) };
	INT32 XOffs[16] = { STEP8(0,4), STEP8(256,4) };
	INT32 YOffs[16] = { STEP8(0,32), STEP8(512,32) };

	Graphics_Decode(Plane, XOffs, YOffs, Plane, XOffs, YOffs, 0x400);
	UINT8 *tmp = (UINT8*)0x00200000;

	memcpy (tmp, DrvGfxROM0, 0x04000);

	GfxDecode4Bpp(0x0200, 4,  8,  8, Plane, XOffs, YOffs, 0x100, tmp, DrvGfxROM0);

	memcpy (tmp, DrvGfxROM1, 0x10000);

	GfxDecode4Bpp(0x0200, 4, 16, 16, Plane, XOffs, YOffs, 0x400, tmp, DrvGfxROM1);
}

INT32 DrvInit(INT32 game_select)
{
	DrvInitSaturn();

	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)BurnMalloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, MALLOC_MAX);
	MemIndex();

	if (game_select == 0) // gberet
	{
		INT32 k = 0;
		if (BurnLoadRom(DrvZ80ROM  + 0x00000,  k++, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM  + 0x04000,  k++, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM  + 0x08000,  k++, 1)) return 1;

		if (BurnLoadRom(DrvGfxROM0 + 0x00000,  k++, 1)) return 1;

		if (BurnLoadRom(DrvGfxROM1 + 0x00000,  k++, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x04000,  k++, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x08000,  k++, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x0c000,  k++, 1)) return 1;

		if (BurnLoadRom(DrvColPROM + 0x00000,  k++, 1)) return 1;
		if (BurnLoadRom(DrvColPROM + 0x00020,  k++, 1)) return 1;
		if (BurnLoadRom(DrvColPROM + 0x00120,  k++, 1)) return 1;

		DrvGfxDecode();
	}
	else if (game_select == 1) // mrgoemon
	{
		INT32 k = 0;
		if (BurnLoadRom(DrvZ80ROM  + 0x00000,  k++, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM  + 0x08000,  k++, 1)) return 1;

		if (BurnLoadRom(DrvGfxROM0 + 0x00000,  k++, 1)) return 1;

		if (BurnLoadRom(DrvGfxROM1 + 0x00000,  k++, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x08000,  k++, 1)) return 1;

		if (BurnLoadRom(DrvColPROM + 0x00000,  k++, 1)) return 1;
		if (BurnLoadRom(DrvColPROM + 0x00020,  k++, 1)) return 1;
		if (BurnLoadRom(DrvColPROM + 0x00120,  k++, 1)) return 1;

		DrvGfxDecode();
	}

	DrvPaletteInit();

#ifdef CZ80
	CZetInit(1);
	CZetOpen(0);
	CZetSetReadHandler(gberet_read);
//	CZetSetWriteHandler(gberet_write);


	CZetMapMemory(DrvZ80ROM,	0x0000, 0xbfff, MAP_ROM);
	CZetMapMemory(DrvColRAM,	0xc000, 0xc7ff, MAP_ROM);
	CZetMapMemory(DrvVidRAM,	0xc800, 0xcfff, MAP_ROM);
	CZetMapMemory(DrvSprRAM1,	0xd000, 0xd0ff, MAP_RAM);
	CZetMapMemory(DrvSprRAM0,	0xd100, 0xd1ff, MAP_RAM); // 100-1ff
	CZetMapMemory(DrvZ80RAM,	0xd200, 0xdfff, MAP_RAM);
	CZetMapMemory(DrvScrollRAM,	0xe000, 0xe0ff, MAP_ROM); // handler
	CZetMapMemory(DrvSprRAM2,	0xe800, 0xe9ff, MAP_RAM); // 100-1ff (bootleg)
	CZetSetWriteHandler((game_select == 1) ? mrgoemon_write : ((game_select == 2) ? gberetb_write : gberet_write));	
	CZetSetReadHandler(gberet_read);

	CZetMemEnd();
	CZetClose();
#endif
	SN76489Init(0, 18432000 / 12, 0);
	DrvDoReset(1);

	return 0;
}

INT32 DrvExit()
{
	DrvDoReset(1);
	SPR_InitSlaveSH();
	CZetExit2();
	SN76489Init(0, 0, 0);
	
	nBurnSprites=128;
	cleanSprites();
	memset(ss_scl,0x00,nBurnLinescrollSize);
	MemEnd=DrvZ80ROM=DrvGfxROM0=DrvGfxROM1=DrvColPROM=NULL;
	DrvPalette=NULL;
	free(AllMem);
	AllMem=NULL;

	_30_HZ=0;
	cleanDATA();
	cleanBSS();

	nSoundBufferPos=0;

	return 0;
}

void DrvPaletteInit()
{
	UINT32 tab[0x20];

	init_32_colors(tab,DrvColPROM);

	DrvColPROM += 0x20;

	for (UINT32 i = 0; i < 0x100; i++)
	{
		unsigned char ctabentry;

		ctabentry = (DrvColPROM[0x000 + i] & 0x0f) | 0x10;
		colBgAddr2[i]=DrvPalette[0x000 + i] = tab[ctabentry];
		ctabentry = (DrvColPROM[0x100 + i] & 0x0f);
		colBgAddr2[0x100 +i ]=DrvPalette[0x100 + i] = tab[ctabentry];
	}
}

//-------------------------------------------------------------------------------------------------------------------------------------
void initLayers()
{

    Uint16	CycleTb[]={
		0xff5e, 0xffff, //A1
		0xffff, 0xffff,	//A0
		0x04ff, 0xffff,   //B1
		0xffff, 0xffff  //B0
	};

 	SclConfig	scfg;
	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN1WORD;
	scfg.flip          = SCL_PN_10BIT; 
	scfg.platesize     = SCL_PL_SIZE_2X2;
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.patnamecontrl =  0x000c;// VRAM B1 のオフセット 
	scfg.plate_addr[0] = (Uint32)ss_map;
	SCL_SetConfig(SCL_NBG0, &scfg);
	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initPosition()
{
	SCL_Open();
	ss_reg->n0_move_y =  (16<<16) ;
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initColors()
{
	colBgAddr = (Uint16*)SCL_AllocColRam(SCL_NBG0,ON);
	SCL_AllocColRam(SCL_SPR,OFF);
	(Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
	SCL_SetColRam(SCL_NBG1,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
	nBurnSprites = 51;
	nSoundBufferPos = 0;

	SS_CACHE = cache   =(Uint8 *)SCL_VDP2_VRAM_B1;
	SS_MAP   = ss_map  =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_MAP2  = ss_map2 =(Uint16 *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_scl			= (Fixed32 *)SS_SCL;
#ifdef CACHE
	memset(bg_dirtybuffer,1,2048);
#endif

	SS_SET_S0PRIN(6);
	SS_SET_N1PRIN(7);
	SS_SET_N2PRIN(4);
	SS_SET_N0PRIN(5);

	initLayers();
	initPosition();
	initColors();
	initSprites(256-1,224-1,8,0,0,-16);
	//play=1;
//	drawWindow(0,240,0,2,66);
	initScrolling(ON,(void *)SCL_VDP2_VRAM_B0+0x4000);
//	memset(&ss_scl[0],16<<16,64);
	memset(&ss_scl[0],16<<16,128);
	drawWindow(0,240,0,2,66);
//	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void draw_sprites()
{
	UINT8 *ram = (sprite_bank & 8) ? DrvSprRAM1 : DrvSprRAM0;
	SprSpCmd *ss_spritePtr;
	ss_spritePtr = &ss_sprite[3];
	
	for (UINT32 offs = 0; offs < 0xc0; offs += 4)
	{
		if (ram[offs + 3])
		{
			UINT32 attr = ram[offs + 1];
			UINT32 code = ram[offs + 0] + ((attr & 0x40) << 2);
			UINT32 flip = attr & 0x30;

			ss_spritePtr->ax			= ram[offs + 2] - 2 * (attr & 0x80);
			ss_spritePtr->ay			= ram[offs + 3];
			ss_spritePtr->color      = (attr & 0x0f) << 4;
			ss_spritePtr->control    = ( JUMP_NEXT | FUNC_NORMALSP | flip);
			ss_spritePtr->drawMode   = ( ECD_DISABLE | COMPO_REP);		
			ss_spritePtr->charSize   = 0x210;  //0x100 16*16
			ss_spritePtr->charAddr   = 0x220+(code<<4);
			*ss_spritePtr++;			
		}
	}
}

int DrvDraw()
{
	INT32 scroll = DrvScrollRAM[0] | (DrvScrollRAM[32] << 8);

	memset4_fast(&ss_scl[16],scroll | (scroll<<16),0x340);

	for (UINT32 offs = 0x40; offs < 0x7c0; offs++)
	{
#ifdef CACHE
		if (bg_dirtybuffer[offs])
		{
			bg_dirtybuffer[offs] = 0;
#endif
			UINT32 attr = DrvColRAM[offs];
			UINT32 code = DrvVidRAM[offs] + ((attr & 0x40)<<2);
			UINT32 color = attr & 0x0f;
			UINT32 flip = attr & 0x30;

			ss_map2[offs] = ss_map2[offs+0x1000] = ss_map[offs] = ss_map[offs+0x1000] = (color << 12 | flip << 6 | code&0x1FF) ;
#ifdef CACHE
		}
#endif
	}
	return 0;
}
int DrvFrame()
{
	unsigned int nInterleave = game_type ? 16 : 32;

#ifdef CZ80
	if(game_type & 2)
		CZetOpen(0);
#endif
	unsigned int nCyclesDone = 0, nCyclesTotal;

if (_30_HZ)
	nCyclesTotal = 3072000 / 3 / (6000 / 256);
else
	nCyclesTotal = 3072000 / 9 / (6000 / 256);

	for (unsigned int i = 0; i < nInterleave; i++)
	{
		nCyclesDone += CZetRun(nCyclesTotal / nInterleave);

		if (irq_enable && i == (nInterleave - 1)) {
			CZetRaiseIrq(0);
		}

		if (nmi_enable && (i & 1)) {
			CZetNmi();
		}
	}


	if(game_type & 2)
		CZetClose();

	SPR_RunSlaveSH((PARA_RTN*)renderSound,&nSoundBufferPos);
	DrvDraw();

	cleanSprites();
	draw_sprites();
	SPR_WaitEndSlaveSH();

	return 0;
}
int gberetInit()
{
	game_type = 0;
	_30_HZ =1;
	nBurnLinescrollSize = 0x360;
//	nBurnSprites = 51;
	return DrvInit(0);
}

int mrgoemonInit()
{
	game_type = 2;
	_30_HZ=0;
	nBurnLinescrollSize = 0x380;
//	nBurnSprites = 51;
	return DrvInit(1);
}

//-------------------------------------------------------------------------------------------------------------------------------------
void renderSound(unsigned int *nSoundBufferPos)
{
	signed short *	nSoundBuffer = (signed short *)0x25a20000;
	SN76496Update(0, &nSoundBuffer[nSoundBufferPos[0]], SOUND_LEN);
	nSoundBufferPos[0]+=SOUND_LEN;

	if(nSoundBufferPos[0]>=RING_BUF_SIZE/2)//0x2400)
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos[0]);
		PCM_Task(pcm); // bon emplacement
		nSoundBufferPos[0]=0;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------

