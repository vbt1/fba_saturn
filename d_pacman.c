// FB Alpha Puckman module
// Based on MAME driver by Nicola Salmoria and many others

// Fix Shoot the Bull inputs

#include "d_pacman.h"
#define nBurnSoundLen 192

///*static*/ INT32 nSoundBufferPos1=0;
#define CACHE 1

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvpengo2u = {
		"pengo2u", "pacm",
		"Pengo (set 2 not encrypted)\0",
		pengo2uRomInfo, pengo2uRomName, PengoInputInfo, PengoDIPInfo,
		pengouInit, DrvExit, DrvFrame, DrvDraw //, NULL, &DrvRecalc, 0x200,
	};

//	if (strcmp(nBurnDrvpengo2u.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvpengo2u,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}

UINT8 __fastcall pacman_read(UINT16 a)
{
	switch (a & ~0x003f)
	{
		case 0x5000: return DrvInputs[0];
		case 0x5040: return DrvInputs[1];
		case 0x5080: return DrvDips[2];
		case 0x50c0: return DrvDips[3];
	}

	if ((a & 0xfc00) == 0x4800) return 0xbf;

	return 0;
}

void __fastcall pacman_out_port(UINT16 a, UINT8 d)
{
	a &= 0xff;

	if (a == 0) // pacman & clones only
	{
		interrupt_mode = d;
//		CZetSetVector(d);
//		CZetCPUContext[i].nInterruptLatch = -1;
		CZetRaiseIrq(d);
//		ZetSetIRQLine(0, ZET_IRQSTATUS_NONE);
		CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);
	}
}

void __fastcall pengo_write(UINT16 a, UINT8 d)
{
#ifdef CACHE
	if(a>=0x8000 & a<=0x87ff)
	{
			if(PengoStart[a]!=d)
			{
				PengoStart[a]=d;
				bg_dirtybuffer[a&0x3ff]=1;
			}
			return;
	}
#endif
	////	CZetMapArea(0x8400, 0x87ff, 1, DrvColRAM);

	if ((a & 0xffe0) == 0x9000) {
		NamcoSoundWrite(a & 0x1f, d);
		return;
	}

	if ((a & 0xfff0) == 0x9020) {
		DrvSprRAM2[a & 0x0f] = d;
		return;
	}

	switch (a)
	{
		case 0x9040:
			interrupt_mask = d & 1;
		return;

		case 0x9042:
			palettebank = d;
		return;

		case 0x9043:
//			*flipscreen = d & 1;
		return;

		case 0x9044:
		case 0x9045: // coin counter
		return;

		case 0x9046:
			colortablebank = d;
		return;

		case 0x9047:
			charbank = d & 1;
			spritebank = d & 1;
		return;

		case 0x9070:
			watchdog = 0;
		return;
	}
}

UINT8 __fastcall pengo_read(UINT16 a)
{
	switch (a & ~0x003f)
	{
		case 0x9000: return DrvDips[3];
		case 0x9040: return DrvDips[2];
		case 0x9080: return DrvInputs[1];
		case 0x90c0: return DrvInputs[0];
	}

	return 0;
}

//------------------------------------------------------------------------------------------------------

static INT32 DrvDoReset(INT32 clear_ram)
{
	if (clear_ram) {
		memset (AllRam, 0, RamEnd - AllRam);
	}

	watchdog = 0;
	nPacBank = 0;

	CZetOpen(0);
	CZetReset();
	CZetClose();

//	AY8910Reset(0);

//	mschamp_counter = 0;
//	cannonb_bit_to_read = 0;
//	alibaba_mystery = 0;
	
	interrupt_mode = 0;
	interrupt_mask = 0;
	colortablebank = 0;
	palettebank = 0;
	spritebank = 0;	
	charbank = 0;
	
	return 0;
}

static void pacman_palette_init()
{
	UINT32 t_pal[32];

	for (INT32 i = 0; i < 32; i++)
	{
		INT32 bit0 = (DrvColPROM[i] >> 0) & 0x01;
		INT32 bit1 = (DrvColPROM[i] >> 1) & 0x01;
		INT32 bit2 = (DrvColPROM[i] >> 2) & 0x01;
		UINT8 r = (bit0 * 33) + (bit1 * 71) + (bit2 * 151);

		bit0 = (DrvColPROM[i] >> 3) & 0x01;
		bit1 = (DrvColPROM[i] >> 4) & 0x01;
		bit2 = (DrvColPROM[i] >> 5) & 0x01;
		UINT8 g = (bit0 * 33) + (bit1 * 71) + (bit2 * 151);

		bit0 = (DrvColPROM[i] >> 6) & 0x01;
		bit1 = (DrvColPROM[i] >> 7) & 0x01;
		UINT8 b = (bit0 * 81) + (bit1 * 174);

		t_pal[i] =	 RGB(r>>3,g>>3,b>>3);
	}
	int delta=0;

	for (INT32 i = 0; i < 256; i++)
	{
		UINT8 ctabentry = DrvColPROM[i + 0x100] & 0x0f;

		/*colAddr[i]=*/colBgAddr[delta] = /*Palette[0x000 + i] =*/ t_pal[ctabentry + 0x00];
		/*colAddr[i+256]=*/colBgAddr[delta+1024] = /*Palette[0x100 + i] =*/ t_pal[ctabentry + 0x10];
		delta++; if ((delta & 3) == 0) delta += 12;

//		Palette[0x000 + i] = t_pal[ctabentry + 0x00];
//		Palette[0x100 + i] = t_pal[ctabentry + 0x10];
	}

//	DrvRecalc = 1;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*inline*/ static int readbit(const UINT8 *src, int bitnum)
{
	return src[bitnum / 8] & (0x80 >> (bitnum % 8));
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void rotate_tile16x16(unsigned int size,unsigned char flip, unsigned char *target)
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
				 rot[i][j]= temp[j][i] ;
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

static void GfxDecode4Bpp(int num, int numPlanes, int xSize, int ySize, int planeoffsets[], int xoffsets[], int yoffsets[], int modulo, unsigned char *pSrc, unsigned char *pDest)
{
	int c;
//	wait_vblank();
	for (c = 0; c < num; c++) {
		int plane, x, y;
	
		UINT8 *dp = pDest + (c * (xSize/2) * ySize);
		memset(dp, 0, (xSize/2) * ySize);
	
		for (plane = 0; plane < numPlanes; plane++) {
			int planebit = 1 << (numPlanes - 1 - plane);
			int planeoffs = (c * modulo) + planeoffsets[plane];
		
			for (y = 0; y < ySize; y++) {
				int yoffs = planeoffs + yoffsets[y];
				dp = pDest + (c * (xSize/2) * ySize) + (y * (xSize/2));
			
				for (x = 0; x < xSize; x+=2) {
					if (readbit(pSrc, yoffs + xoffsets[x])) dp[x>>1] |= (planebit&0x0f)<<4;
					if (readbit(pSrc, yoffs + xoffsets[x+1])) dp[x>>1] |= (planebit& 0x0f);
					//(NewsTiles[i+1]& 0x0f)| ((NewsTiles[i]& 0x0f) <<4)
				}
			}
//			sc_check();
//			wait_vblank();
		}
	}
//	wait_vblank();
}

static void convert_gfx()
{
	/*static*/ INT32 PlaneOffsets[2]  = { 0, 4 };
	/*static*/ INT32 CharXOffsets[8]  = { 64, 65, 66, 67, 0, 1, 2, 3 };
	/*static*/ INT32 SpriXOffsets[16] = { 8*8, 8*8+1, 8*8+2, 8*8+3, 16*8+0, 16*8+1, 16*8+2, 16*8+3, 24*8+0, 24*8+1, 24*8+2, 24*8+3, 0, 1, 2, 3 };
	/*static*/ INT32 YOffsets[16]     = { 0*8, 1*8, 2*8, 3*8, 4*8, 5*8, 6*8, 7*8, 32*8, 33*8, 34*8, 35*8, 36*8, 37*8, 38*8, 39*8 };

	INT32 size = (game_select == PENGO) ? 0x2000 : 0x1000;

	UINT8 *tmp = (UINT8*)0x00200000;
//	UINT8 *tmp = (UINT8*)malloc(size * 2);
	memset(tmp,0x00,size * 2);

	memcpy (tmp, DrvGfxROM, size * 2);
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	GfxDecode4Bpp(((size * 4) / 0x040), 2,  8,  8, PlaneOffsets, CharXOffsets + 0, YOffsets, 0x080, tmp,        cache);//DrvGfxROM + 0x0000);
	GfxDecode4Bpp(((size * 4) / 0x100), 2, 16, 16, PlaneOffsets, SpriXOffsets + 0, YOffsets, 0x200, tmp + size, &ss_vram[0x1100]);//DrvGfxROM + 0x8000);
	rotate_tile(((size * 4) / 0x040),0,cache);
	rotate_tile16x16(((size * 4) / 0x100),0,&ss_vram[0x1100]);
	ss_vram = NULL;
	memset(tmp,0x00,size * 2);
//	free(tmp);
	tmp = NULL;
}

static INT32 pacman_load()
{
	char* pRomName = "";
	struct BurnRomInfo ri;

	INT32 pOffset = 0;
	UINT8 *gLoad = DrvGfxROM;
	UINT8 *cLoad = DrvColPROM;
	UINT8 *sLoad = NamcoSoundProm;
	UINT8 *qLoad = DrvQROM;

	for (INT32 i = 0; !BurnDrvGetRomName(&pRomName, i, 0); i++) {

		BurnDrvGetRomInfo(&ri, i);

		if ((ri.nType & 7) == 1) {
			if (BurnLoadRom(DrvZ80ROM + pOffset, i, 1)) return 1;
		  
		/*	if (game_select == MSPACMAN) {
				pOffset += 0x1000;
			} else {	*/
				pOffset += ri.nLen;
	/*		}

			if (pOffset == 0x4000 && game_select != PENGO) {
				pOffset = 0x8000;
			}			 */

			continue;
		}

		if ((ri.nType & 7) == 2) {
			if (BurnLoadRom(gLoad, i, 1)) return 1;
			gLoad += ri.nLen;

			continue;
		}

		if ((ri.nType & 7) == 3) {
			if (BurnLoadRom(cLoad, i, 1)) return 1;
			cLoad += 0x100;

			continue;
		}
		
		if ((ri.nType & 7) == 4) {
			if (BurnLoadRom(sLoad, i, 1)) return 1;
			sLoad += 0x100;

			continue;
		}

		if ((ri.nType & 7) == 7) {
			if (BurnLoadRom(qLoad, i, 1)) return 1;
			qLoad += ri.nLen;

			continue;
		}	
	}
	gLoad = cLoad = sLoad = qLoad = DrvQROM = NULL;
	return 0;
}

static INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;

	DrvZ80ROM		= Next; Next += 0x020000;

	DrvQROM			= Next;
	DrvGfxROM		= (UINT8 *)cache;
	DrvColPROM		= Next; Next += 0x000500;
	NamcoSoundProm		= Next; Next += 0x000200;
	//Palette			= (UINT32*)Next; Next += 0x0200 * sizeof(UINT32);
	AllRam			= Next;

	DrvZ80RAM		= Next; Next += 0x001000;
	DrvSprRAM		= DrvZ80RAM + 0x7f0;
	DrvSprRAM2		= Next; Next += 0x000010;
	PengoStart		= Next - 0x8000;
	DrvVidRAM		= Next; Next += 0x000400;
	DrvColRAM		= Next; Next += 0x000400;
//	flipscreen		= Next; Next += 0x000001;

	RamEnd			= Next;
/*
	pAY8910Buffer[0] = (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[1] = (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[2] = (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
*/
	bg_dirtybuffer	= Next; Next += 0x400 * sizeof(UINT8);
	map_offset_lut	= Next; Next += 0x400 * sizeof(UINT16);
	ofst_lut				= Next; Next += 0x400 * sizeof(UINT16);

	MemEnd			= Next;

	return 0;
}

static void PengoMap()
{
	CZetMapArea(0x0000, 0x7fff, 0, DrvZ80ROM);
//	CZetMapArea(0x0000, 0x7fff, 2, DrvZ80ROM);
	CZetMapArea2(0x0000, 0x7fff, 2, DrvZ80ROM + 0x8000, DrvZ80ROM);
	CZetMapArea(0x8000, 0x83ff, 0, DrvVidRAM);
#ifndef CACHE
	CZetMapArea(0x8000, 0x83ff, 1, DrvVidRAM);
#endif
	CZetMapArea(0x8000, 0x83ff, 2, DrvVidRAM);
	CZetMapArea(0x8400, 0x87ff, 0, DrvColRAM);
#ifndef CACHE
	CZetMapArea(0x8400, 0x87ff, 1, DrvColRAM);
#endif
	CZetMapArea(0x8400, 0x87ff, 2, DrvColRAM);
	CZetMapArea(0x8800, 0x8fff, 0, DrvZ80RAM);
	CZetMapArea(0x8800, 0x8fff, 1, DrvZ80RAM);
	CZetMapArea(0x8800, 0x8fff, 2, DrvZ80RAM);

	CZetSetWriteHandler(pengo_write);
	CZetSetReadHandler(pengo_read);
}

static INT32 DrvInit(void (*mapCallback)(), void (*pInitCallback)(), INT32 select)
{
	DrvInitSaturn();
	game_select = select;

	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)malloc(nLen)) == NULL) return 1;

	memset(AllMem, 0, nLen);
	MemIndex();

	make_lut();
	memset(bg_dirtybuffer,1,0x400);

	if (pacman_load()) return 1;

	if (pInitCallback) {
		pInitCallback();
	}
	convert_gfx();
	pacman_palette_init();

	CZetInit(1);
	CZetOpen(0);
	mapCallback();
	CZetClose();

//	AY8910Init(0, 1789750, nBurnSoundRate, NULL, NULL, NULL, NULL);
//	AY8910SetAllRoutes(0, 0.75, BURN_SND_ROUTE_BOTH);
//	if (game_select == DREMSHPR) AY8910SetAllRoutes(0, 0.50, BURN_SND_ROUTE_BOTH);

//	SN76496Init(0, 1789750, 0);
//	SN76496Init(1, 1789750, 1);	
//	SN76496SetRoute(0, 0.75, BURN_SND_ROUTE_BOTH);
//	SN76496SetRoute(1, 0.75, BURN_SND_ROUTE_BOTH);

	NamcoSoundInit(18432000 / 6 / 32, 3);
//	NacmoSoundSetAllRoutes(1.00, BURN_SND_ROUTE_BOTH);

//	GenericTilesInit();
//  	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"DrvDoReset   before            ",10,20);	

	DrvDoReset(1);
  //	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"DrvDoReset   after            ",10,20);	

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initLayers()
{
    Uint16	CycleTb[]={
		0x1f56, 0xffff, //A0
		0xffff, 0xffff,	//A1
		0xf5f2,0x4eff,   //B0
		0xffff, 0xffff  //B1
//		0x4eff, 0x1fff, //B1
	};
 	SclConfig	scfg;

// 3 nbg
	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD; //2word
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.plate_addr[0] = (Uint32)SS_MAP2;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG1, &scfg);
// 3 nbg
//	scfg.plate_addr[0] = (Uint32)SS_MAP;
	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
	scfg.plate_addr[0] = (Uint32)SS_FONT;

// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);
	SCL_SetCycleTable(CycleTb);
	
	scfg.dispenbl      = OFF;
	SCL_SetConfig(SCL_NBG2, &scfg);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);	  //ON
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void make_lut(void)
{
	unsigned int i,delta=0;
	int sx, sy, row,col;

	for (i = 0; i < 0x400;i++) 
	{

		sx = (i % 36);
		sy = i / 36;

		row = sy + 2;
		col = sx - 2;

		if (col & 0x20)
			ofst_lut[i] = row + ((col & 0x1f) << 5);
		else
			ofst_lut[i] = col + (row << 5);

		sy = (col+2)<<6;
		sx = 	30-row;
		map_offset_lut[i] = (sx|sy)<<1;	
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrvInitSaturn()
{
	SS_MAP  = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B1;
	SS_MAP2 = ss_map2  =(Uint16 *)SCL_VDP2_VRAM_A1;
	SS_FONT = ss_font  =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_CACHE= cache    =(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclSpPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;
	ss_OtherPri       = (SclOtherPriRegister *)SS_OTHR;

	ss_sprite  = (SprSpCmd *)SS_SPRIT;
//	ss_regs->tvmode = 0x8011;
	ss_regs->tvmode = 0x80d1;
//	ss_regs->tvmode = 0x8081;
//	ss_reg->n1_delta_y = FIXED(1.2);
//	ss_reg->n0_delta_y = FIXED(0.5);
 
//	SCL_Open();
/*	ss_reg->n0_move_x = 0;
	ss_reg->n1_move_x = 0;
	ss_reg->n0_move_y = 0;
	ss_reg->n1_move_y = 0;
 */  
	ss_reg->n1_delta_y = FIXED(0.6);
	ss_reg->n0_delta_y = FIXED(0.5);

	nBurnLinescrollSize = 0x300;
	nBurnSprites = 32+3;

//3 nbg
	SS_SET_N0PRIN(7);
	SS_SET_S0PRIN(6);
	SS_SET_N1PRIN(4);
//	SS_SET_N2PRIN(5);

	initLayers();
	initColors();
	initSprites(256-1,240-1,0,0,0,0);
	drawWindow(0,240,0,2,70);
}



static INT32 DrvExit()
{
	NamcoSoundExit();
	CZetExit();

	game_select = PACMAN;
	nPacBank = -1;

	DrvZ80ROM = DrvQROM = DrvGfxROM = DrvColPROM = NamcoSoundProm = NULL;
	/*/DrvTransTable = Palette =*/ AllRam = DrvZ80RAM = DrvSprRAM = DrvSprRAM2 = NULL;
	DrvColRAM= DrvVidRAM = /*flipscreen =*/ RamEnd = NULL;
	PengoStart = bg_dirtybuffer = MemEnd = NULL;
	map_offset_lut = ofst_lut = NULL;
	free (AllMem);
	AllMem = NULL;
	return 0;
}

static void DrawBackground()
{
	for (UINT16 offs = 0; offs < 36 * 28; offs++)
	{
		INT16 ofst = ofst_lut[offs];
#ifdef CACHE
		if(bg_dirtybuffer[ofst]==1)
		{
			bg_dirtybuffer[ofst]=0;
# endif
			INT32 code  = (charbank << 8) | DrvVidRAM[ofst];
			INT32 color = (DrvColRAM[ofst] & 0x1f) | (colortablebank << 5) | (palettebank << 6);
			int x = map_offset_lut[offs];

			ss_map2[x]=color;
			ss_map2[x+1]=code;
#ifdef CACHE
		}
#endif
	}
}

static void DrawSprites()
{
	for (INT32 offs = 0x10 - 2;offs >= 0;offs -= 2)
	{
		INT32 code   = (DrvSprRAM[offs] >> 2) | (spritebank << 6);
		INT32 color  = (DrvSprRAM[offs + 1] & 0x1f ) | (colortablebank << 5) | (palettebank << 6);

		INT32 sx     = DrvSprRAM2[offs + 1];
		INT32 sy     = DrvSprRAM2[offs];
		INT32 flipx  = DrvSprRAM [offs] & 1;
		INT32 flipy  = DrvSprRAM [offs] & 2;

		INT32 flip  = (flipy>>1| flipx<<1)<<4;

		unsigned int delta=((offs)>>1)+3;
		ss_sprite[delta].ax = 248-sy;
		ss_sprite[delta].ay = (272-sx)*0.8333;

		ss_sprite[delta].control    = ( JUMP_NEXT | FUNC_NORMALSP | flip);
		ss_sprite[delta].charSize   = 0x210;  //0x100 16*16
		ss_sprite[delta].charAddr   = 0x220+((code)<<4);
		ss_sprite[delta].color      = color<<4;		
	}
}

static INT32 DrvDraw()
{
	DrawBackground();
	DrawSprites();

	return 0;
}

static INT32 DrvFrame()
{
	watchdog++;
	if (watchdog >= 16) {
		DrvDoReset(0);
	}

	if (DrvReset) {
		DrvDoReset(1);
	}

	{
		memset (DrvInputs, 0, 2);

		for (INT32 i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
		}

//		if (/*!acitya &&*/ game_select != SHOOTBUL) {
			if ((DrvInputs[0] & 6) == 6) DrvInputs[0] &= ~0x06;
			if ((DrvInputs[0] & 9) == 9) DrvInputs[0] &= ~0x09;
			if ((DrvInputs[1] & 6) == 6) DrvInputs[1] &= ~0x06;
			if ((DrvInputs[1] & 9) == 9) DrvInputs[1] &= ~0x09;
//		}

		DrvInputs[0] ^= DrvDips[0];
		DrvInputs[1] ^= DrvDips[1];

		nAnalogAxis[0] -= DrvAxis[0];
		nAnalogAxis[1] -= DrvAxis[1];
		
		nCharAxis[0] = (DrvAxis[0] >> 12) & 0x0f;
		nCharAxis[1] = (DrvAxis[1] >> 12) & 0x0f;
	}

	CZetOpen(0);
	
	INT32 nInterleave = nBurnSoundLen;
	INT32 nSoundBufferPos1 = 0;
	
	INT32 nCyclesTotal = (18432000 / 6) / 60;
	INT32 nCyclesDone = 0;
	INT32 nCyclesSegment;
	
	for (INT32 i = 0; i < nInterleave; i++) 
	{
		INT32 nNext;
		
		nNext = (i + 1) * nCyclesTotal / nInterleave;
		nCyclesSegment = nNext - nCyclesDone;
		nCyclesDone += CZetRun(nCyclesSegment);
		
			if (i == (nInterleave - 1) && interrupt_mask) {
//				CZetSetVector(interrupt_mode);
				CZetRaiseIrq(interrupt_mode);
//				ZetSetIRQLine(0, ZET_IRQSTATUS_AUTO);
				CZetSetIRQLine(0, CZET_IRQSTATUS_AUTO);
			}
		
//		if (pBurnSoundOut) 
//		{
			INT32 nSegmentLength = nBurnSoundLen / nInterleave;
//			INT16* pSoundBuf = pBurnSoundOut + (nSoundBufferPos1 << 1);
//			Sint8 *nSoundBuffer = (Sint8 *)0x25a20000;
			Sint16 *nSoundBuffer = (Sint16 *)0x25a20000;

//	SN76496Update(0, &nSoundBuffer[nSoundBufferPos], SOUND_LEN);

			
			if (nSegmentLength) {
						NamcoSoundUpdate(&nSoundBuffer[nSoundBufferPos], nSegmentLength);
//						NamcoSoundUpdate(pSoundBuf, nSegmentLength);
//					}
				}
//			}

			nSoundBufferPos += nSegmentLength;
			nSoundBufferPos1 += nSegmentLength;
//		}
	}
	
//	if (pBurnSoundOut) {
//		int nSegmentLength = nBurnSoundLen - nSoundBufferPos;
		INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos1;
//		INT16* pSoundBuf = pBurnSoundOut + (nSoundBufferPos1 << 1);

		if (nSegmentLength) 
		{
			Sint16 *nSoundBuffer = (Sint16 *)0x25a20000;
			NamcoSoundUpdate(&nSoundBuffer[nSoundBufferPos], nSegmentLength);
			nSoundBufferPos += nSegmentLength;
		}
	
	CZetClose();

 // 	Sint8 *nSoundBuffer = (Sint8 *)0x25a20000;
 /* 	Sint16 *nSoundBuffer = (Sint16 *)0x25a20000;

	for (int n = 0; n < nBurnSoundLen; n++) 
	{
//		int nSample  = pSoundBuf[n];
		nSoundBuffer[nSoundBufferPos + n] = pSoundBuf[n];
//		nSoundBuffer[nSoundBufferPos + (n << 1) + 0] = (nSample>>8)&0xFF;//pAY8910Buffer[5][n];//nSample;
//		nSoundBuffer[nSoundBufferPos + (n << 1) + 1] = nSample&0xFF;//pAY8910Buffer[5][n];//nSample;
	}
	nSoundBufferPos+=(nBurnSoundLen);//<<1); 
*/
   /*
	if (pBurnDraw) {
		DrvDraw();
	}
   */
     DrvDraw();
	if(nSoundBufferPos>=RING_BUF_SIZE/2)//0x4800-nSegmentLength)//
	{
		PCM_Task(pcm);
		nSoundBufferPos=0;
	}
	
	return 0;
}
//------------------------------------------------------------------------------------------------------
static void PengoGraphicsReorder()
{
	UINT8 *tmp = (UINT8*)0x00200000;
	memset(tmp,0x00,0x20000);
	memcpy (tmp + 0x0000, DrvGfxROM + 0x2000, 0x1000);
	memcpy (tmp + 0x1000, DrvGfxROM + 0x1000, 0x1000);
	memcpy (DrvGfxROM + 0x1000, tmp + 0x0000, 0x2000);
	tmp = NULL;
}

static void PengouCallback()
{
	memcpy (DrvZ80ROM + 0x8000, DrvZ80ROM, 0x8000);

	PengoGraphicsReorder();
}

static INT32 pengouInit()
{
	return DrvInit(PengoMap, PengouCallback, PENGO);
}
/*static void MspacmanDecode()
{
#define ADD0SWAP(x) BITSWAP16(x,15,14,13,12,11,3,7,9,10,8,6,5,4,2,1,0)
#define ADD1SWAP(x) BITSWAP16(x,15,14,13,12,11,8,7,5,9,10,6,3,4,2,1,0)
#define DATASWAP(x) BITSWAP08(x,0,4,5,7,6,3,2,1)

	//static const UINT16 tab[10 * 8] = { // even is dst, odd is src
		0x0410, 0x8008, 0x08E0, 0x81D8, 0x0A30, 0x8118, 0x0BD0, 0x80D8, 
		0x0C20, 0x8120, 0x0E58, 0x8168, 0x0EA8, 0x8198, 0x1000, 0x8020, 
		0x1008, 0x8010, 0x1288, 0x8098, 0x1348, 0x8048, 0x1688, 0x8088, 
		0x16B0, 0x8188, 0x16D8, 0x80C8, 0x16F8, 0x81C8, 0x19A8, 0x80A8, 
		0x19B8, 0x81A8, 0x2060, 0x8148, 0x2108, 0x8018, 0x21A0, 0x81A0, 
		0x2298, 0x80A0, 0x23E0, 0x80E8, 0x2418, 0x8000, 0x2448, 0x8058, 
		0x2470, 0x8140, 0x2488, 0x8080, 0x24B0, 0x8180, 0x24D8, 0x80C0, 
		0x24F8, 0x81C0, 0x2748, 0x8050, 0x2780, 0x8090, 0x27B8, 0x8190, 
		0x2800, 0x8028, 0x2B20, 0x8100, 0x2B30, 0x8110, 0x2BF0, 0x81D0, 
		0x2CC0, 0x80D0, 0x2CD8, 0x80E0, 0x2CF0, 0x81E0, 0x2D60, 0x8160
	};

	memcpy (DrvZ80ROM + 0x0b000, DrvZ80ROM + 0x0a000, 0x01000);
	memcpy (DrvZ80ROM + 0x10000, DrvZ80ROM + 0x00000, 0x03000);

	for (INT32 i = 0; i < 0x1000; i++)
	{
		DrvZ80ROM[0x13000+i] = DATASWAP(DrvZ80ROM[0xb000+ADD0SWAP(i)]);
	}

	for (INT32 i = 0; i < 0x800; i++)
	{
		DrvZ80ROM[0x18000+i] = DATASWAP(DrvZ80ROM[0x8000+ADD1SWAP(i)]);
		DrvZ80ROM[0x18800+i] = DATASWAP(DrvZ80ROM[0x9800+ADD0SWAP(i)]);
		DrvZ80ROM[0x19000+i] = DATASWAP(DrvZ80ROM[0x9000+ADD0SWAP(i)]);
		DrvZ80ROM[0x19800+i] = DrvZ80ROM[0x1800+i];
	}

	memcpy (DrvZ80ROM + 0x1a000, DrvZ80ROM + 0x02000, 0x2000);

	for (INT32 i = 0; i < 80; i+=2) { // apply patches
		memcpy (DrvZ80ROM + 0x10000 + tab[i], DrvZ80ROM + 0x10000 + tab[i+1], 8);
	}

	memcpy (DrvZ80ROM + 0x8000, DrvZ80ROM, 0x4000);
}
*/
/*static INT32 mspacmanInit()
{
	return DrvInit(MspacmanMap, MspacmanDecode, MSPACMAN);
}
*/ 