#include "d_bombjack.h"


int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvBombjack = {
		"bombja", NULL,
		"Bomb Jack (set 1)",
		BombjackRomInfo,BombjackRomName, DrvInputInfo,BjDIPInfo,
		BjInit,BjExit,BjFrame,NULL
	};

	memcpy(shared,&nBurnDrvBombjack,sizeof(struct BurnDriver));
	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initLayers()
{
    Uint16	CycleTb[]={
		0x1f5f, 0xffff, //A0
		0xffff, 0xffff,	//A1
		0xffff,0x4eff,   //B0
		0xffff, 0xffff  //B1
	};

	SclConfig	scfg;
// 3 nbg
	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD;
//	scfg.flip          = SCL_PN_12BIT; 
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
//	scfg.patnamecontrl =  0x000c;// VRAM B1 のオフセット 
	//for(i=0;i<4;i++)   
	scfg.plate_addr[0] = (Uint32)ss_map;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG1, &scfg);

//	scfg.dispenbl 	     = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
//	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 	     = SCL_BITMAP;
	scfg.mapover         = SCL_OVER_0;
	scfg.plate_addr[0]   = (Uint32)ss_font;
// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initPosition()
{
	SCL_Open();
	ss_reg->n1_move_x =  (-8<<16) ;
	ss_reg->n1_move_y =  (32<<16) ;
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initColors()
{
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
	(Uint16*)SCL_AllocColRam(SCL_NBG3,OFF);
 	colAddr    = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void make_lut(void)
{
	unsigned int i,delta=0;
	int sx, sy, row,col;

	for (i = 0; i < 0x400;i++) 
	{
		int sx = (i) & 0x1f;
		int sy = (((i+0x40) >> 5) & 0x1f)<<6;
//		map_offset_lut[i] = (sx | sy)<<1;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DrvInitSaturn()
{
	nBurnSprites  = 51;//27;

	SS_CACHE = cache      =(Uint8  *)SCL_VDP2_VRAM_A0;
	SS_MAP     = ss_map     =(Uint16 *)SCL_VDP2_VRAM_A1;
	SS_FONT   = ss_font    =(Uint16 *)SCL_VDP2_VRAM_B0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;

//3 nbg
	SS_SET_S0PRIN(6);
	SS_SET_N0PRIN(7);
	SS_SET_N1PRIN(4);
//	SCL_SET_N2PRIN(3);

	initLayers();
	initPosition();
	initColors();
	make_lut();
	initSprites(256+8-1,224-1,0,0,8,-16);
	drawWindow(0,224,0,2,62); 
}
//-------------------------------------------------------------------------------------------------------------------------------------

static INT32 DrvDoReset()
{
	bombjackIRQ = 0;
	latch = 0;
	for (INT32 i = 0; i < 2; i++) {
		CZetOpen(i);
		CZetReset();
		CZetClose();
	}

	for (INT32 i = 0; i < 3; i++) {
		AY8910Reset(i);
	}
	return 0;
}


UINT8 __fastcall BjMemRead(UINT16 addr)
{
	UINT8 inputs=0;
	
	if (addr >= 0x9820 && addr <= 0x987f) return BjSprRam[addr - 0x9820];

	if (addr==0xb000) {
		if (DrvJoy1[5])
			inputs|=0x01;
		if (DrvJoy1[4])
			inputs|=0x02;
		if (DrvJoy1[2])
			inputs|=0x04;
		if (DrvJoy1[3])
			inputs|=0x08;
		if (DrvJoy1[6])
			inputs|=0x10;
		return inputs;
	}
	if (addr==0xb001) {
		if (DrvJoy2[5])
			inputs|=0x01;
		if (DrvJoy2[4])
			inputs|=0x02;
		if (DrvJoy2[2])
			inputs|=0x04;
		if (DrvJoy2[3])
			inputs|=0x08;
		if (DrvJoy2[6])
			inputs|=0x10;
		return inputs;
	}
	if (addr==0xb002) {
		if (DrvJoy1[0])
			inputs|=0x01;
		if (DrvJoy1[1])
			inputs|=0x04;
		if (DrvJoy2[0])
			inputs|=0x02;
		if (DrvJoy2[1])
			inputs|=0x08;
		return inputs;
	}
	if (addr==0xb004) {
		return BjDip[0]; // Dip Sw(1)
	}
	if (addr==0xb005) {
		return BjDip[1]; // Dip Sw(2)
	}
	return 0;
}

void __fastcall BjMemWrite(UINT16 addr,UINT8 val)
{
	if (addr >= 0x9820 && addr <= 0x987f) { BjSprRam[addr - 0x9820] = val; return; }
	
	if (addr==0xb000)
	{
		bombjackIRQ = val;
	}
	if(addr==0xb800)
	{
		latch=val;
		return;
	}
	BjRam[addr]=val;
}

UINT8 __fastcall SndMemRead(UINT16 a)
{
	if (a==0xFF00)
	{
		return 0x7f;
	}
	if(a==0x6000)
	{
		INT32 res;
		res = latch;
		latch = 0;
		return res;
	}
	return 0;
}



void __fastcall SndPortWrite(UINT16 a, UINT8 d)
{
	a &= 0xff;
	switch (a) {
		case 0x00: {
			AY8910Write(0, 0, d);
			return;
				   }
		case 0x01: {
			AY8910Write(0, 1, d);
			return;
				   }
		case 0x10: {
			AY8910Write(1, 0, d);
			return;
				   }
		case 0x11: {
			AY8910Write(1, 1, d);
			return;
				   }
		case 0x80: {
			AY8910Write(2, 0, d);
			return;
				   }
		case 0x81: {
			AY8910Write(2, 1, d);
			return;
				   }
	}
}

static INT32 BjZInit()
{
	// Init the z80
	CZetInit(0);
	// Main CPU setup
	CZetOpen(0);

	CZetMapArea    (0x0000,0x7fff,0,BjRom+0x0000); // Direct Read from ROM
	CZetMapArea    (0x0000,0x7fff,2,BjRom+0x0000); // Direct Fetch from ROM
	CZetMapArea    (0xc000,0xdfff,0,BjRom+0x8000); // Direct Read from ROM
	CZetMapArea    (0xc000,0xdfff,2,BjRom+0x8000); // Direct Fetch from ROM

	CZetMapArea    (0x8000,0x8fff,0,BjRam+0x8000);
	CZetMapArea    (0x8000,0x8fff,1,BjRam+0x8000);

	CZetMapArea    (0x9000,0x93ff,0,BjVidRam);
	CZetMapArea    (0x9000,0x93ff,1,BjVidRam);

	CZetMapArea    (0x9400,0x97ff,0,BjColRam);
	CZetMapArea    (0x9400,0x97ff,1,BjColRam);

//	CZetMapArea    (0x9820,0x987f,0,BjSprRam);
//	CZetMapArea    (0x9820,0x987f,1,BjSprRam);

	CZetMapArea    (0x9c00,0x9cff,0,BjPalSrc);
	CZetMapArea    (0x9c00,0x9cff,1,BjPalSrc);

	CZetMapArea    (0x9e00,0x9e00,0,BjRam+0x9e00);
	CZetMapArea    (0x9e00,0x9e00,1,BjRam+0x9e00);

	//	CZetMapArea    (0xb000,0xb000,0,BjRam+0xb000);
	//	CZetMapArea    (0xb000,0xb000,1,BjRam+0xb000);

	//	CZetMapArea    (0xb800,0xb800,0,BjRam+0xb800);
	//	CZetMapArea    (0xb800,0xb800,1,BjRam+0xb800);

	CZetSetReadHandler(BjMemRead);
	CZetSetWriteHandler(BjMemWrite);
	CZetClose();

	CZetInit(1);
	CZetOpen(1);
	CZetMapArea    (0x0000,0x1fff,0,SndRom); // Direct Read from ROM
	CZetMapArea    (0x0000,0x1fff,2,SndRom); // Direct Fetch from ROM
	CZetMapArea    (0x4000,0x43ff,0,SndRam);
	CZetMapArea    (0x4000,0x43ff,1,SndRam);
	CZetMapArea    (0x4000,0x43ff,2,SndRam); // fetch from ram?
	CZetMapArea    (0xff00,0xffff,0,SndRam);
	CZetMapArea    (0xff00,0xffff,1,SndRam);
	CZetMapArea    (0xff00,0xffff,2,SndRam); // more fetch from ram? What the hell . .

	//	CZetMapArea    (0x6000,0x6000,0,BjRam+0xb800);
	//	CZetMapArea    (0x6000,0x6000,1,BjRam+0xb800);
	CZetSetReadHandler(SndMemRead);
	CZetSetOutHandler(SndPortWrite);
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
	AY8910SetAllRoutes(0, 0.13, BURN_SND_ROUTE_BOTH);
	AY8910SetAllRoutes(1, 0.13, BURN_SND_ROUTE_BOTH);
	AY8910SetAllRoutes(2, 0.13, BURN_SND_ROUTE_BOTH);

	// remember to do CZetReset() in main driver

	DrvDoReset();
	return 0;
}



static void DecodeTiles(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
{
	INT32 c,y,x,dat1,dat2,dat3,col;
	for (c=0;c<num;c++)
	{
		for (y=0;y<8;y++)
		{
			dat1=BjGfx[off1 + (c * 8) + y];
			dat2=BjGfx[off2 + (c * 8) + y];
			dat3=BjGfx[off3 + (c * 8) + y];
			for (x=0;x<8;x++)
			{
				col=0;
				if (dat1&1){ col |= 4;}
				if (dat2&1){ col |= 2;}
				if (dat3&1){ col |= 1;}
				TilePointer[(c * 64) + ((7-x) * 8) + (7 - y)]=col;
				dat1>>=1;
				dat2>>=1;
				dat3>>=1;
			}
		}
	}
}


static INT32 MemIndex()
{
	UINT8 *Next; Next = Mem;

	BjRom		  = Next; Next += 0x10000;
	BjGfx		  = Next; Next += 0x0F000;
	BjMap		  = Next; Next += 0x02000;
	SndRom	  = Next; Next += 0x02000;
	RamStart  = Next;
	BjRam		  = Next; Next += 0x10000;
	SndRam	  = Next; Next += 0x01000;
	BjPalSrc  = Next; Next += 0x00100;
	BjVidRam  = Next; Next += 0x00400;
	BjColRam  = Next; Next += 0x00400;
	BjSprRam  = Next; Next += 0x00060;
	RamEnd	  = Next;
	text		  = Next; Next += 512 * 8 * 8;
	sprites	  = Next; Next += 1024 * 8 * 8;
	tiles		  = Next; Next += 1024 * 8 * 8;
	pFMBuffer	= (INT16*)Next; Next += nBurnSoundLen * 9 * sizeof(INT16);
	BjPalReal	= (UINT32*)Next; Next += 0x0080 * sizeof(UINT32);
	MemEnd	  = Next;

	return 0;
}


static INT32 BjInit()
{
	DrvInitSaturn();
	// Allocate and Blank all required memory
	Mem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((Mem = (UINT8 *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);
	MemIndex();
	
	INT32 RomOffset = 0;

	if (BjIsBombjackt) {
		for (INT32 i = 0; i < 3; i++) {
			BurnLoadRom(BjRom + (0x4000 * i), i, 1);
		}
		
		RomOffset = 3;
	} else {
		for (INT32 i = 0; i < 5; i++) {
			BurnLoadRom(BjRom + (0x2000 * i), i, 1);
		}
		
		RomOffset = 5;
	}

	for (INT32 i = 0; i < 3; i++) {
		BurnLoadRom(BjGfx + (0x1000 * i), i + RomOffset, 1);
	}

	BurnLoadRom(BjGfx + 0x3000, RomOffset + 3, 1);
	BurnLoadRom(BjGfx + 0x5000, RomOffset + 4, 1);
	BurnLoadRom(BjGfx + 0x7000, RomOffset + 5, 1);

	BurnLoadRom(BjGfx + 0x9000, RomOffset + 6, 1);
	BurnLoadRom(BjGfx + 0xB000, RomOffset + 7, 1);
	BurnLoadRom(BjGfx + 0xD000, RomOffset + 8, 1);

	BurnLoadRom(BjMap, RomOffset + 9, 1); // load Background tile maps
	BurnLoadRom(SndRom, RomOffset + 10, 1); // load Sound CPU

	// Set memory access & Init
	BjZInit();

	DecodeTiles(text,512,0,0x1000,0x2000);
	DecodeTiles(sprites,1024,0x7000,0x5000,0x3000);
	DecodeTiles(tiles,1024,0x9000,0xB000,0xD000);
	// done

//	GenericTilesInit();

	DrvDoReset();
	return 0;
}

static INT32 BjtInit()
{
	BjIsBombjackt = 1;
	
	return BjInit();
}

static INT32 BjExit()
{
	CZetExit();

	for (INT32 i = 0; i < 3; i++) {
		AY8910Exit(i);
	}

//	GenericTilesExit();
	free(Mem);
	Mem = NULL;
	
	BjIsBombjackt = 0;
	
	return 0;
}

static UINT32 CalcCol(UINT16 nColour)
{
	INT32 r, g, b;

	r = (nColour >> 0) & 0x0f;
	g = (nColour >> 4) & 0x0f;
	b = (nColour >> 8) & 0x0f;

	r = (r << 4) | r;
	g = (g << 4) | g;
	b = (b << 4) | b;

	return RGB(r, g, b);
}

static INT32 CalcAll()
{
	for (INT32 i = 0; i < 0x100; i++) {
		BjPalReal[i / 2] = CalcCol(BjPalSrc[i & ~1] | (BjPalSrc[i | 1] << 8));
	}

	return 0;
}

static void BjRenderFgLayer()
{
	for (INT32 tileCount = 0; tileCount < 1024 ;tileCount++) 
	{
		INT32 code = BjVidRam[tileCount] + 16 * (BjColRam[tileCount] & 0x10);
		INT32 color = BjColRam[tileCount] & 0x0f;
		INT32 sy = (tileCount % 32);
		INT32 sx = 31 - (tileCount / 32);

		sx<<=3;
		sx-=16;
		sy<<=3;
		if (sx >= 0 && sx < 215 && sy >= 0 && sy < 246)
		{
//			Render8x8Tile_Mask(pTransDraw, code,sx,sy,color, 3, 0, 0, text);
		}
		else
		{
//			Render8x8Tile_Mask_Clip(pTransDraw, code,sx,sy,color, 3, 0, 0, text);
		}
	}
}


static void BjRenderBgLayer()
{
	for (INT32 tileCount = 0; tileCount < 256;tileCount++) {
		INT32 FlipX;

		INT32 BgSel=BjRam[0x9e00];

		INT32 offs = (BgSel & 0x07) * 0x200 + tileCount;
		INT32 Code = (BgSel & 0x10) ? BjMap[offs] : 0;

		INT32 attr = BjMap[offs + 0x100];
		INT32 Colour = attr & 0x0f;
		//INT32 flags = (attr & 0x80) ? TILE_FLIPY : 0;


		INT32 sy = (tileCount % 16);
		INT32 sx = 15 - (tileCount / 16);
		FlipX = attr & 0x80;

		/*if (SolomonFlipScreen) {
		sx = 31 - sx;
		sy = 31 - sy;
		FlipX = !FlipX;
		FlipY = !FlipY;
		}*/

		sx <<= 4;
		sx -=16;
		sy <<= 4;
		Code <<= 2;
		if (sx >= 0 && sx < 215 && sy >= 0 && sy < 246) {
 /*
			if (FlipX&0x80)
			{
				Render8x8Tile_Mask(pTransDraw, Code+0,sx+8,sy+0, Colour, 3, 0, 0, tiles);
				Render8x8Tile_Mask(pTransDraw, Code+1,sx+8,sy+8, Colour, 3, 0, 0, tiles);
				Render8x8Tile_Mask(pTransDraw, Code+2,sx+0,sy+0, Colour, 3, 0, 0, tiles);
				Render8x8Tile_Mask(pTransDraw, Code+3,sx+0,sy+8, Colour, 3, 0, 0, tiles);

			}
			else
			{
				Render8x8Tile_Mask(pTransDraw, Code+0,sx+8,sy+0, Colour, 3, 0, 0, tiles);
				Render8x8Tile_Mask(pTransDraw, Code+1,sx+8,sy+8, Colour, 3, 0, 0, tiles);
				Render8x8Tile_Mask(pTransDraw, Code+2,sx+0,sy+0, Colour, 3, 0, 0, tiles);
				Render8x8Tile_Mask(pTransDraw, Code+3,sx+0,sy+8, Colour, 3, 0, 0, tiles);
			}
*/
		} else {
/*
			if (FlipX&0x80)
			{
				Render8x8Tile_Mask_Clip(pTransDraw, Code+0,sx+8,sy+0, Colour, 3, 0, 0, tiles);
				Render8x8Tile_Mask_Clip(pTransDraw, Code+1,sx+8,sy+8, Colour, 3, 0, 0, tiles);
				Render8x8Tile_Mask_Clip(pTransDraw, Code+2,sx+0,sy+0, Colour, 3, 0, 0, tiles);
				Render8x8Tile_Mask_Clip(pTransDraw, Code+3,sx+0,sy+8, Colour, 3, 0, 0, tiles);

			}
			else
			{
				Render8x8Tile_Mask_Clip(pTransDraw, Code+0,sx+8,sy+0, Colour, 3, 0, 0, tiles);
				Render8x8Tile_Mask_Clip(pTransDraw, Code+1,sx+8,sy+8, Colour, 3, 0, 0, tiles);
				Render8x8Tile_Mask_Clip(pTransDraw, Code+2,sx+0,sy+0, Colour, 3, 0, 0, tiles);
				Render8x8Tile_Mask_Clip(pTransDraw, Code+3,sx+0,sy+8, Colour, 3, 0, 0, tiles);
			}
*/
		}
	}
}


static void BjDrawSprites()
{
	INT32 offs;

	for (offs = 0x60 - 4; offs >= 0; offs -= 4)
	{

		/*
		abbbbbbb cdefgggg hhhhhhhh iiiiiiii

		a        use big sprites (32x32 instead of 16x16)
		bbbbbbb  sprite code
		c        x flip
		d        y flip (used only in death sequence?)
		e        ? (set when big sprites are selected)
		f        ? (set only when the bonus (B) materializes?)
		gggg     color
		hhhhhhhh x position
		iiiiiiii y position
		*/
		INT32 sx,sy,flipx,flipy, code, colour, big;


		sy = BjSprRam[offs+3];
		if (BjSprRam[offs] & 0x80)
			sx = BjSprRam[offs+2];
		else
			sx = BjSprRam[offs+2];
		flipx = BjSprRam[offs+1] & 0x80;
		flipy =	BjSprRam[offs+1] & 0x40;

		code = BjSprRam[offs] & 0x7f;
		colour = (BjSprRam[offs+1] & 0x0f);
		big = (BjSprRam[offs] & 0x80);

		//sy -= 32;

		sx -=16;
		if (!big)
		{
			code <<= 2;
			if (sx >= 0 && sx < 215 && sy >= 0 && sy < 246) {
				if (!flipy) {
/*					if (!flipx) {
						Render8x8Tile_Mask(pTransDraw, code + 0, sx + 8, sy + 0, colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code + 1, sx + 8, sy + 8, colour, 3, 0, 0, sprites);					
						Render8x8Tile_Mask(pTransDraw, code + 2, sx + 0, sy + 0, colour, 3, 0, 0, sprites);		
						Render8x8Tile_Mask(pTransDraw, code + 3, sx + 0, sy + 8, colour, 3, 0, 0, sprites);	
					} else {
						Render8x8Tile_Mask_FlipX(pTransDraw, code + 2, sx + 8, sy + 0, colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_FlipX(pTransDraw, code + 3, sx + 8, sy + 8, colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_FlipX(pTransDraw, code + 0, sx + 0, sy + 0, colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_FlipX(pTransDraw, code + 1, sx + 0, sy + 8, colour, 3, 0, 0, sprites);
					}	*/
				} else {
/*					if (!flipx) {
						Render8x8Tile_Mask_FlipY(pTransDraw, code + 1, sx + 8, sy + 0, colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_FlipY(pTransDraw, code + 0, sx + 8, sy + 8, colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_FlipY(pTransDraw, code + 3, sx + 0, sy + 0, colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_FlipY(pTransDraw, code + 2, sx + 0, sy + 8, colour, 3, 0, 0, sprites);
					} else {
						Render8x8Tile_Mask_FlipXY(pTransDraw, code + 3, sx + 8, sy + 0, colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_FlipXY(pTransDraw, code + 2, sx + 8, sy + 8, colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_FlipXY(pTransDraw, code + 1, sx + 0, sy + 0, colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_FlipXY(pTransDraw, code + 0, sx + 0, sy + 8, colour, 3, 0, 0, sprites);
					}*/
				}
			} else {
/*				if (!flipy) {
					if (!flipx) {
						Render8x8Tile_Mask_Clip(pTransDraw, code + 0, sx + 8, sy + 0, colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code + 1, sx + 8, sy + 8, colour, 3, 0, 0, sprites);					
						Render8x8Tile_Mask_Clip(pTransDraw, code + 2, sx + 0, sy + 0, colour, 3, 0, 0, sprites);		
						Render8x8Tile_Mask_Clip(pTransDraw, code + 3, sx + 0, sy + 8, colour, 3, 0, 0, sprites);	
					} else {
						Render8x8Tile_Mask_FlipX_Clip(pTransDraw, code + 2, sx + 8, sy + 0, colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_FlipX_Clip(pTransDraw, code + 3, sx + 8, sy + 8, colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_FlipX_Clip(pTransDraw, code + 0, sx + 0, sy + 0, colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_FlipX_Clip(pTransDraw, code + 1, sx + 0, sy + 8, colour, 3, 0, 0, sprites);
					}
				} else {
					if (!flipx) {
						Render8x8Tile_Mask_FlipY_Clip(pTransDraw, code + 1, sx + 0, sy + 0, colour, 4, 0, 0, sprites);
						Render8x8Tile_Mask_FlipY_Clip(pTransDraw, code + 0, sx + 0, sy + 8, colour, 4, 0, 0, sprites);
						Render8x8Tile_Mask_FlipY_Clip(pTransDraw, code + 3, sx + 8, sy + 0, colour, 4, 0, 0, sprites);
						Render8x8Tile_Mask_FlipY_Clip(pTransDraw, code + 2, sx + 8, sy + 8, colour, 4, 0, 0, sprites);
					} else {
						Render8x8Tile_Mask_FlipXY_Clip(pTransDraw, code + 3, sx + 8, sy + 0, colour, 4, 0, 0, sprites);
						Render8x8Tile_Mask_FlipXY_Clip(pTransDraw, code + 2, sx + 8, sy + 8, colour, 4, 0, 0, sprites);
						Render8x8Tile_Mask_FlipXY_Clip(pTransDraw, code + 1, sx + 0, sy + 0, colour, 4, 0, 0, sprites);
						Render8x8Tile_Mask_FlipXY_Clip(pTransDraw, code + 0, sx + 0, sy + 8, colour, 4, 0, 0, sprites);
					}
				}
*/
			}
		}
		else
		{	
			code&=31;
			code <<= 4;
			sx-=1;
			if (sx >= 0 && sx < 215 && sy >= 0 && sy < 246) 
			{
/*				if (!flipy) 
				{
					if (!flipx) {
						Render8x8Tile_Mask(pTransDraw, code+512,sx+8+16,sy,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+513,sx+8+16,sy+8,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+514,sx+16,sy,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+515,sx+16,sy+8,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+516,sx+8+16,sy+16,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+517,sx+8+16,sy+8+16,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+518,sx+16,sy+16,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+519,sx+16,sy+8+16,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+520,sx+8,sy,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+521,sx+8,sy+8,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+522,sx,sy,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+523,sx,sy+8,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+524,sx+8,sy+16,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+525,sx+8,sy+8+16,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+526,sx,sy+16,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask(pTransDraw, code+527,sx,sy+8+16,colour, 3, 0, 0, sprites);
					}
				}
*/
			}
			else
			{
/*				if (!flipy) 
				{
					if (!flipx) {
						Render8x8Tile_Mask_Clip(pTransDraw, code+512,sx+8+16,sy,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+513,sx+8+16,sy+8,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+514,sx+16,sy,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+515,sx+16,sy+8,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+516,sx+8+16,sy+16,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+517,sx+8+16,sy+8+16,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+518,sx+16,sy+16,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+519,sx+16,sy+8+16,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+520,sx+8,sy,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+521,sx+8,sy+8,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+522,sx,sy,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+523,sx,sy+8,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+524,sx+8,sy+16,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+525,sx+8,sy+8+16,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+526,sx,sy+16,colour, 3, 0, 0, sprites);
						Render8x8Tile_Mask_Clip(pTransDraw, code+527,sx,sy+8+16,colour, 3, 0, 0, sprites);
					}
				}
*/
			}
		}
	}
}

static INT32 BjFrame()
{
	if (DrvReset) {	// Reset machine
		DrvDoReset();
	}

	INT32 nInterleave = 10;
	INT32 nSoundBufferPos = 0;

	nCyclesTotal[0] = 4000000 / 60;
	nCyclesTotal[1] = 3000000 / 60;
	nCyclesDone[0] = nCyclesDone[1] = 0;

	for (INT32 i = 0; i < nInterleave; i++) {
		INT32 nCurrentCPU, nNext;

		// Run Z80 #1
		nCurrentCPU = 0;
		CZetOpen(nCurrentCPU);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesDone[nCurrentCPU] += CZetRun(nCyclesSegment);
		if (i == 1) 
		{
			if(bombjackIRQ)
			{
				CZetNmi();
			}
		}
		CZetClose();

		// Run Z80 #2
		nCurrentCPU = 1;
		CZetOpen(nCurrentCPU);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesSegment = CZetRun(nCyclesSegment);
		nCyclesDone[nCurrentCPU] += nCyclesSegment;
		CZetClose();

		// Render Sound Segment
/*		if (pBurnSoundOut) {
			INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos;
			INT16* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			AY8910Render(&pAY8910Buffer[0], pSoundBuf, nSegmentLength, 0);
			nSoundBufferPos += nSegmentLength;
		}		 */
	}

	// Make sure the buffer is entirely filled.
/*	if (pBurnSoundOut) {
		INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos;
		INT16* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
		if (nSegmentLength) {
			AY8910Render(&pAY8910Buffer[0], pSoundBuf, nSegmentLength, 0);
		}
	}
*/
	/*CZetOpen(0);
	if (BjRam[0xb000])
	CZetNmi();
	CZetClose();*/

	CZetOpen(1);
	CZetNmi();
	CZetClose();


//	if (pBurnDraw != NULL)
	{
//		BurnTransferClear();
		CalcAll();

		BjRenderBgLayer();
		BjRenderFgLayer();
		BjDrawSprites();
//		BurnTransferCopy(BjPalReal);
	}
	return 0;
}
