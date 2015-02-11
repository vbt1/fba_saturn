#include "d_bombjack.h"
#define RAZE 1

static void DecodeTiles16_4Bpp(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3);
UINT32 BgSel=0xFFFF;
UINT32 bg_cache[1024];

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
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initLayers()
{
    Uint16	CycleTb[]={
		0x1f56, 0xffff, //A0
		0xffff, 0xffff,	//A1
		0xf5f2, 0x4eff,   //B0
		0xffff, 0xffff  //B1
//		0x4eff, 0x1fff, //B1
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
	scfg.pnamesize     = SCL_PN1WORD;
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
	scfg.charsize          = SCL_CHAR_SIZE_2X2;//OK du 1*1 surtout pas toucher
	scfg.pnamesize      = SCL_PN1WORD;
	scfg.patnamecontrl =  0x00008;// VRAM B0 のオフセット 
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
//	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
//	scfg.datatype      = SCL_CELL;

	scfg.coltype       = SCL_COL_TYPE_16;
	scfg.platesize     = SCL_PL_SIZE_1X1;
	scfg.plate_addr[0] = (Uint32)ss_map2;
//	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG1, &scfg);

	scfg.dispenbl 		 = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 		 = SCL_BITMAP;
	scfg.mapover		 = SCL_OVER_0;
	scfg.plate_addr[0]	 = (Uint32)ss_font;

// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);	
}

static void initLayersOld()
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
	scfg.charsize      = SCL_CHAR_SIZE_2X2;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD; //2word
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.plate_addr[0] = (Uint32)SS_MAP2;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG1, &scfg);
// 3 nbg
	scfg.plate_addr[0] = (Uint32)SS_MAP;
	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
	scfg.plate_addr[0] = (Uint32)SS_FONT;

// 3 nbg
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	SCL_SetConfig(SCL_NBG0, &scfg);
	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	colAddr = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);

	SCL_SetColRam(SCL_NBG0,8,8,palette);
/*
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
	(Uint16*)SCL_AllocColRam(SCL_NBG3,OFF);
 	colAddr    = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);

	SCL_SetColRam(SCL_NBG0,8,8,palette);
	*/
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void make_lut(void)
{
/*
		INT32 sx = ((tileCount) % 32) <<6; //% 32;
		INT32 sy = (32 - ((tileCount) / 32));
		int offs = (sx| sy);//<<1;
*/
	for (UINT32 i = 0; i < 1024;i++) 
	{
		INT32 sx = ((i) % 32) <<6; //% 32;
		INT32 sy = (32 - ((i) / 32));
		map_offset_lut[i] = (sx| sy);//<<1;
	}
	
	for (UINT32 i = 0; i < 256;i++) 
	{
		INT32 sy = (i % 16) <<5;//<<6
		INT32 sx = (15 - (i / 16));//<<1;
		mapbg_offset_lut[i] = (sx| sy);//<<1;
	}

	for (UINT32 i = 0; i < 4096; i++) 
	{
		cram_lut[i] = CalcCol(i);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	nBurnSprites  = 51;//27;

	SS_FONT        = ss_font     =(Uint16 *)SCL_VDP2_VRAM_A1;
	SS_MAP          = ss_map    =(Uint16 *)SCL_VDP2_VRAM_A1+0x08000;
	SS_MAP2        = ss_map2  =(Uint16 *)SCL_VDP2_VRAM_A1+0x0B000;
	SS_CACHE      = cache      =(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_reg->n1_move_x = -8<<16;
//3 nbg
	SS_SET_S0PRIN(6);
	SS_SET_N0PRIN(7);
	SS_SET_N1PRIN(4);
	SS_SET_N2PRIN(5);
	ss_regs->tvmode = 0x8011;

	initLayers();
	initColors();
	make_lut();
	initSprites(256-1,240-1,0,0,7,0);
	drawWindow(0,240,0,6,66); 
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

#ifdef RAZE
	z80_reset();
#endif

	for (INT32 i = 0; i < 3; i++) {
		AY8910Reset(i);
	}
	return 0;
}


UINT8 __fastcall BjMemRead(UINT16 addr)
{
	UINT8 inputs=0;
	
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

	if (addr >= 0x9820 && addr <= 0x987f) return BjSprRam[addr - 0x9820];

	return 0;
}
#ifdef RAZE
void __fastcall BjMemWrite_9000(UINT16 addr,UINT8 val)
{
	addr &=0x3ff;

	if (BjVidRam[addr]!=val)
	{
		BjVidRam[addr]=val;
		UINT32 code = val + 16 * (BjColRam[addr] & 0x10);
		UINT32 color = BjColRam[addr] & 0x0f;

		UINT32 offs = map_offset_lut[addr];
		ss_map[offs] = color << 12 | code;
	}
}

void __fastcall BjMemWrite_9400(UINT16 addr,UINT8 val)
{
	addr &=0x3ff;

	if (BjColRam[addr]!=val)
	{
		BjColRam[addr]=val;
		UINT32 code = BjVidRam[addr] + 16 * (val & 0x10);

		UINT32 offs = map_offset_lut[addr];
		ss_map[offs] = (val & 0x0f) << 12 | code;
	}
}

UINT8 __fastcall BjMemRead_9820(UINT16 addr)
{
	return BjSprRam[addr - 0x9820];
}

void __fastcall BjMemWrite_9820(UINT16 addr,UINT8 val)
{
	BjSprRam[addr - 0x9820] = val;
}

void __fastcall BjMemWrite_b000(UINT16 addr,UINT8 val)
{
	if (addr==0xb000)
	{
		bombjackIRQ = val;
	}
	BjRam[addr]=val;
}

void __fastcall BjMemWrite_b800(UINT16 addr,UINT8 val)
{
	latch=val;
}
#endif
void __fastcall BjMemWrite(UINT16 addr,UINT8 val)
{
	if (addr >= 0x9820 && addr <= 0x987f) { BjSprRam[addr - 0x9820] = val; return; }

	if(addr==0xb800)
	{
		latch=val;
		return;
	}
	
	if (addr==0xb000)
	{
		bombjackIRQ = val;
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
	CZetInit(2);
	// Main CPU setup
#ifdef RAZE
	z80_init_memmap();
	z80_map_fetch (0x0000,0x7fff,BjRom+0x0000); 
	z80_map_read  (0x0000,0x7fff,BjRom+0x0000);  
	z80_map_fetch (0xc000,0xdfff,BjRom+0x8000); 
	z80_map_read  (0xc000,0xdfff,BjRom+0x8000);  

//	z80_map_fetch (0x8000,0x8fff,BjRam+0x8000);
	z80_map_read  (0x8000,0x8fff,BjRam+0x8000);
	z80_map_write (0x8000,0x8fff,BjRam+0x8000);

//	z80_map_fetch (0x9000,0x93ff,BjVidRam);
	z80_map_read  (0x9000,0x93ff,BjVidRam);
//	z80_map_write (0x9000,0x93ff,BjVidRam);

//	z80_map_fetch (0x9400,0x97ff,BjColRam);
	z80_map_read  (0x9400,0x97ff,BjColRam);
//	z80_map_write (0x9400,0x97ff,BjColRam);

//	z80_map_fetch (0x9c00,0x9cff,BjPalSrc);
	z80_map_read  (0x9c00,0x9cff,BjPalSrc);
	z80_map_write (0x9c00,0x9cff,BjPalSrc);

//	z80_map_fetch (0x9e00,0x9e00,BjRam+0x9e00);
	z80_map_read  (0x9e00,0x9e00,BjRam+0x9e00);
	z80_map_write (0x9e00,0x9e00,BjRam+0x9e00);
	z80_end_memmap();

	z80_add_read(0x9820, 0x987f, 1, (void *)&BjMemRead_9820);
	z80_add_read(0xb000, 0xb005, 1, (void *)&BjMemRead);

	z80_add_write(0x9000,0x93ff, 1, (void *)&BjMemWrite_9000);
	z80_add_write(0x9400,0x97ff, 1, (void *)&BjMemWrite_9400);

	z80_add_write(0x9820, 0x987f, 1, (void *)&BjMemWrite_9820);
	z80_add_write(0xb000, 0xb000, 1, (void *)&BjMemWrite_b000);
	z80_add_write(0xb800, 0xb800, 1, (void *)&BjMemWrite_b800);
#else
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
#endif
//	CZetInit(1);
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
//	AY8910SetAllRoutes(0, 0.13, BURN_SND_ROUTE_BOTH);
//	AY8910SetAllRoutes(1, 0.13, BURN_SND_ROUTE_BOTH);
//	AY8910SetAllRoutes(2, 0.13, BURN_SND_ROUTE_BOTH);

	// remember to do CZetReset() in main driver

	DrvDoReset();
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DecodeTiles4Bpp(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
{
	INT32 c,y,x,dat1,dat2,dat3,col1,col2;
	for (c=0;c<num;c++)
	{
		for (y=0;y<8;y++)
		{
			dat1=BjGfx[off1 + (c * 8) + y];
			dat2=BjGfx[off2 + (c * 8) + y];
			dat3=BjGfx[off3 + (c * 8) + y];
			for (x=0;x<8;x++)
			{
				col1=0;
				if (dat1&1){ col1 |= 4;}
				if (dat2&1){ col1 |= 2;}
				if (dat3&1){ col1 |= 1;}
				TilePointer[(c * 64) + ((7-x) * 8) + (7 - y)]=col1;
			   	dat1>>=1;
				dat2>>=1;
				dat3>>=1;
			}
		}
	}
  
	for (c=0;c<num*8*8;c+=2)
	{
		unsigned char c1 = TilePointer[c]&0x0f;
		unsigned char c2 = TilePointer[c+1]&0x0f;
	   TilePointer[c>>1]=c2| (c1<<4)& 0xf0;
	}	  
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DecodeTiles(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
{
	INT32 c,y,x,dat1,dat2,dat3,col1,col2;
	for (c=0;c<num;c++)
	{
		for (y=0;y<8;y++)
		{
			dat1=BjGfx[off1 + (c * 8) + y];
			dat2=BjGfx[off2 + (c * 8) + y];
			dat3=BjGfx[off3 + (c * 8) + y];
			for (x=0;x<8;x++)
			{
				col1=0;
				if (dat1&1){ col1 |= 4;}
				if (dat2&1){ col1 |= 2;}
				if (dat3&1){ col1 |= 1;}
				TilePointer[(c * 64) + ((7-x) * 8) + (7 - y)]=col1;
			   	dat1>>=1;
				dat2>>=1;
				dat3>>=1;
			}
		}
	}
 }
//-------------------------------------------------------------------------------------------------------------------------------------
static void DecodeTiles16_4BppTile(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
{
	DecodeTiles(TilePointer, num,off1,off2, off3);
	unsigned char tiles4[8*8*4];
 /*2,0,3,1 */
	for (int i=0;i<num;i+=4)
	{
		for (int j=0;j<64 ;j++ )
		{
			tiles4[j]=TilePointer[((64*i))+j];
			tiles4[j+64]=TilePointer[(64*(i+1))+j];
			tiles4[j+128]=TilePointer[(64*(i+2))+j];
			tiles4[j+192]=TilePointer[(64*(i+3))+j];
		}	

		for (int j=0;j<8 ;j++ )
		{
			TilePointer[(64*i)+j]=tiles4[128+j];
			TilePointer[(64*i)+j+8]=tiles4[128+j+8];
			TilePointer[(64*i)+j+16]=tiles4[128+j+16];
			TilePointer[(64*i)+j+24]=tiles4[128+j+24];
			TilePointer[(64*i)+j+32]=tiles4[128+j+32];
			TilePointer[(64*i)+j+40]=tiles4[128+j+40];
			TilePointer[(64*i)+j+48]=tiles4[128+j+48];
			TilePointer[(64*i)+j+56]=tiles4[128+j+56];

			TilePointer[64*(i+1)+j]=tiles4[0+j];
			TilePointer[64*(i+1)+j+8]=tiles4[0+j+8];
			TilePointer[64*(i+1)+j+16]=tiles4[0+j+16];
			TilePointer[64*(i+1)+j+24]=tiles4[0+j+24];
			TilePointer[64*(i+1)+j+32]=tiles4[0+j+32];
			TilePointer[64*(i+1)+j+40]=tiles4[0+j+40];
			TilePointer[64*(i+1)+j+48]=tiles4[0+j+48];
			TilePointer[64*(i+1)+j+56]=tiles4[0+j+56];

			TilePointer[64*(i+2)+j]=tiles4[192+j];
			TilePointer[64*(i+2)+j+8]=tiles4[192+j+8];
			TilePointer[64*(i+2)+j+16]=tiles4[192+j+16];
			TilePointer[64*(i+2)+j+24]=tiles4[192+j+24];
			TilePointer[64*(i+2)+j+32]=tiles4[192+j+32];
			TilePointer[64*(i+2)+j+40]=tiles4[192+j+40];
			TilePointer[64*(i+2)+j+48]=tiles4[192+j+48];
			TilePointer[64*(i+2)+j+56]=tiles4[192+j+56];

			TilePointer[64*(i+3)+j]=tiles4[64+j];
			TilePointer[64*(i+3)+j+8]=tiles4[64+j+8];
			TilePointer[64*(i+3)+j+16]=tiles4[64+j+16];
			TilePointer[64*(i+3)+j+24]=tiles4[64+j+24];
			TilePointer[64*(i+3)+j+32]=tiles4[64+j+32];
			TilePointer[64*(i+3)+j+40]=tiles4[64+j+40];
			TilePointer[64*(i+3)+j+48]=tiles4[64+j+48];
			TilePointer[64*(i+3)+j+56]=tiles4[64+j+56];
		}
	}
	
	for (int c=0;c<num*16*16;c+=2)
	{
		unsigned char c1 = TilePointer[c]&0x0f;
		unsigned char c2 = TilePointer[c+1]&0x0f;
	   TilePointer[c>>1]=c2| c1<<4;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DecodeTiles32_4Bpp(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
{
	unsigned char tiles4[32*16*4];
 /*2,0,3,1 */
	for (int i=0;i<num;i++)
	{
		for (int j=0;j<512 ;j++ )
		{
			tiles4[j]=TilePointer[((512*i))+j];
		}	

		for (int j=0;j<8 ;j++ )
		{
			TilePointer[(512*i)+j]=tiles4[256+j];
			TilePointer[(512*i)+j+16]=tiles4[256+j+8];
			TilePointer[(512*i)+j+32]=tiles4[256+j+16];
			TilePointer[(512*i)+j+48]=tiles4[256+j+24];
			TilePointer[(512*i)+j+64]=tiles4[256+j+32];
			TilePointer[(512*i)+j+80]=tiles4[256+j+40];
			TilePointer[(512*i)+j+96]=tiles4[256+j+48];
			TilePointer[(512*i)+j+112]=tiles4[256+j+56];
			TilePointer[(512*i)+j+128]=tiles4[256+j+64];
			TilePointer[(512*i)+j+144]=tiles4[256+j+72];
			TilePointer[(512*i)+j+160]=tiles4[256+j+80];
			TilePointer[(512*i)+j+176]=tiles4[256+j+88];
			TilePointer[(512*i)+j+192]=tiles4[256+j+96];
			TilePointer[(512*i)+j+208]=tiles4[256+j+104];
			TilePointer[(512*i)+j+224]=tiles4[256+j+112];
			TilePointer[(512*i)+j+240]=tiles4[256+j+120];

			TilePointer[(512*i)+j+8]=tiles4[0+j];
			TilePointer[(512*i)+j+24]=tiles4[0+j+8];
			TilePointer[(512*i)+j+40]=tiles4[0+j+16];
			TilePointer[(512*i)+j+56]=tiles4[0+j+24];
			TilePointer[(512*i)+j+72]=tiles4[0+j+32];
			TilePointer[(512*i)+j+88]=tiles4[0+j+40];
			TilePointer[(512*i)+j+104]=tiles4[0+j+48];
			TilePointer[(512*i)+j+120]=tiles4[0+j+56];
			TilePointer[(512*i)+j+136]=tiles4[0+j+64];
			TilePointer[(512*i)+j+152]=tiles4[0+j+72];
			TilePointer[(512*i)+j+168]=tiles4[0+j+80];
			TilePointer[(512*i)+j+184]=tiles4[0+j+88];
			TilePointer[(512*i)+j+200]=tiles4[0+j+96];
			TilePointer[(512*i)+j+216]=tiles4[0+j+104];
			TilePointer[(512*i)+j+232]=tiles4[0+j+112];
			TilePointer[(512*i)+j+248]=tiles4[0+j+120];

 			TilePointer[(512*i)+256+j]=tiles4[384+j];
			TilePointer[(512*i)+256+j+16]=tiles4[384+j+8];
			TilePointer[(512*i)+256+j+32]=tiles4[384+j+16];
			TilePointer[(512*i)+256+j+48]=tiles4[384+j+24];
			TilePointer[(512*i)+256+j+64]=tiles4[384+j+32];
			TilePointer[(512*i)+256+j+80]=tiles4[384+j+40];
			TilePointer[(512*i)+256+j+96]=tiles4[384+j+48];
			TilePointer[(512*i)+256+j+112]=tiles4[384+j+56];
			TilePointer[(512*i)+256+j+128]=tiles4[384+j+64];
			TilePointer[(512*i)+256+j+144]=tiles4[384+j+72];
			TilePointer[(512*i)+256+j+160]=tiles4[384+j+80];
			TilePointer[(512*i)+256+j+176]=tiles4[384+j+88];
			TilePointer[(512*i)+256+j+192]=tiles4[384+j+96];
			TilePointer[(512*i)+256+j+208]=tiles4[384+j+104];
			TilePointer[(512*i)+256+j+224]=tiles4[384+j+112];
			TilePointer[(512*i)+256+j+240]=tiles4[384+j+120];

			TilePointer[(512*i)+256+j+8]=tiles4[128+j];
			TilePointer[(512*i)+256+j+24]=tiles4[128+j+8];
			TilePointer[(512*i)+256+j+40]=tiles4[128+j+16];
			TilePointer[(512*i)+256+j+56]=tiles4[128+j+24];
			TilePointer[(512*i)+256+j+72]=tiles4[128+j+32];
			TilePointer[(512*i)+256+j+88]=tiles4[128+j+40];
			TilePointer[(512*i)+256+j+104]=tiles4[128+j+48];
			TilePointer[(512*i)+256+j+120]=tiles4[128+j+56];
			TilePointer[(512*i)+256+j+136]=tiles4[128+j+64];
			TilePointer[(512*i)+256+j+152]=tiles4[128+j+72];
			TilePointer[(512*i)+256+j+168]=tiles4[128+j+80];
			TilePointer[(512*i)+256+j+184]=tiles4[128+j+88];
			TilePointer[(512*i)+256+j+200]=tiles4[128+j+96];
			TilePointer[(512*i)+256+j+216]=tiles4[128+j+104];
			TilePointer[(512*i)+256+j+232]=tiles4[128+j+112];
			TilePointer[(512*i)+256+j+248]=tiles4[128+j+120];
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DecodeTiles16_4Bpp(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
{
	DecodeTiles(TilePointer, num,off1,off2, off3);
	unsigned char tiles4[8*8*4];

	for (int i=0;i<num;i+=4)
	{
		for (int j=0;j<64 ;j++ )
		{
			tiles4[j]=TilePointer[((64*i))+j];
			tiles4[j+64]=TilePointer[(64*(i+1))+j];
			tiles4[j+128]=TilePointer[(64*(i+2))+j];
			tiles4[j+192]=TilePointer[(64*(i+3))+j];	  
		}

		for (int j=0;j<8 ;j++ )
		{
			TilePointer[(64*i)+j]=tiles4[128+j];
			TilePointer[(64*i)+j+16]=tiles4[128+j+8];
			TilePointer[(64*i)+j+32]=tiles4[128+j+16];
			TilePointer[(64*i)+j+48]=tiles4[128+j+24];
			TilePointer[(64*i)+j+64]=tiles4[128+j+32];
			TilePointer[(64*i)+j+80]=tiles4[128+j+40];
			TilePointer[(64*i)+j+96]=tiles4[128+j+48];
			TilePointer[(64*i)+j+112]=tiles4[128+j+56];

 			TilePointer[(64*i)+j+8]=tiles4[0+j];
			TilePointer[(64*i)+j+24]=tiles4[0+j+8];
			TilePointer[(64*i)+j+40]=tiles4[0+j+16];
			TilePointer[(64*i)+j+56]=tiles4[0+j+24];
			TilePointer[(64*i)+j+72]=tiles4[0+j+32];
			TilePointer[(64*i)+j+88]=tiles4[0+j+40];
			TilePointer[(64*i)+j+104]=tiles4[0+j+48];
			TilePointer[(64*i)+j+120]=tiles4[0+j+56];

			TilePointer[(64*i)+j+128]=tiles4[192+j];
			TilePointer[(64*i)+j+128+16]=tiles4[192+j+8];
			TilePointer[(64*i)+j+128+32]=tiles4[192+j+16];
			TilePointer[(64*i)+j+128+48]=tiles4[192+j+24];
			TilePointer[(64*i)+j+128+64]=tiles4[192+j+32];
			TilePointer[(64*i)+j+128+80]=tiles4[192+j+40];
			TilePointer[(64*i)+j+128+96]=tiles4[192+j+48];
			TilePointer[(64*i)+j+128+112]=tiles4[192+j+56];

			TilePointer[(64*i)+j+128+8]=tiles4[64+j];
			TilePointer[(64*i)+j+128+24]=tiles4[64+j+8];
			TilePointer[(64*i)+j+128+40]=tiles4[64+j+16];
			TilePointer[(64*i)+j+128+56]=tiles4[64+j+24];
			TilePointer[(64*i)+j+128+72]=tiles4[64+j+32];
			TilePointer[(64*i)+j+128+88]=tiles4[64+j+40];
			TilePointer[(64*i)+j+128+104]=tiles4[64+j+48];
			TilePointer[(64*i)+j+128+120]=tiles4[64+j+56];
		}
	}

	for (int c=0;c<num*16*16;c+=2)
	{
		unsigned char c1 = TilePointer[c]&0x0f;
		unsigned char c2 = TilePointer[c+1]&0x0f;
	   TilePointer[c>>1]=c2| c1<<4;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
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
	text		  = cache;//Next; Next += 512 * 8 * 8;
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;

	sprites	  = &ss_vram[0x1100];//Next; Next += 1024 * 8 * 8;
	tiles		  = (UINT8 *)SCL_VDP2_VRAM_B0;//+0x10000;//Next; Next += 1024 * 8 * 8;
	pFMBuffer	= (INT16*)Next; Next += nBurnSoundLen * 9 * sizeof(INT16);
//	BjPalReal	= (UINT32*)Next; Next += 0x0080 * sizeof(UINT32);
	map_offset_lut = (UINT16*)Next; Next += 1024 * sizeof(UINT16);
	mapbg_offset_lut = (UINT16*)Next; Next += 256 * sizeof(UINT16);
	cram_lut = (UINT16*)Next; Next += 4096 * sizeof(UINT16);
	MemEnd = Next;

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 BjInit()
{
	DrvInitSaturn();
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"DrvInitSaturn             ",10,70);
	// Allocate and Blank all required memory
	Mem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((Mem = (UINT8 *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);
	MemIndex();
	make_lut();
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"malloc                  ",10,70);
	
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
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"rom1                  ",10,70);

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
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"rom2                  ",10,70);
	// Set memory access & Init
	BjZInit();

	DecodeTiles4Bpp(text,512,0,0x1000,0x2000);
	DecodeTiles16_4Bpp(sprites,1024,0x7000,0x5000,0x3000);
	DecodeTiles32_4Bpp(sprites+0x4000,32,0x7000,0x5000,0x3000);
	DecodeTiles16_4BppTile(tiles,1024,0x9000,0xB000,0xD000);

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
#ifdef RAZE
	z80_stop_emulating();
#endif

	CZetExit();

	for (INT32 i = 0; i < 3; i++) {
		AY8910Exit(i);
	}
	cram_lut = map_offset_lut = mapbg_offset_lut = NULL;
MemEnd = RamStart = RamEnd = BjGfx = BjMap = BjRom = BjRam = BjColRam = NULL;
BjVidRam = BjSprRam = SndRom = SndRam = text = sprites = tiles = BjPalSrc = NULL;
	for (int i = 0; i < 9; i++) {
		pAY8910Buffer[i] = NULL;
	}

//	free (pFMBuffer);
	pFMBuffer = NULL;
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

	return RGB(r>>3, g>>3, b>>3);
}

static INT32 CalcAll()
{
	int delta=0;
	for (UINT32 i = 0; i < 0x100; i+=2) 
	{
		colAddr[i / 2] = colBgAddr[delta] = cram_lut[BjPalSrc[i] | (BjPalSrc[i+1] << 8)];
//		CalcCol(BjPalSrc[i] | (BjPalSrc[i+1] << 8));
		 		delta++; if ((delta & 7) == 0) delta += 8;
	}

	return 0;
}

static void BjRenderFgLayer()
{
	for (UINT32 tileCount = 0; tileCount < 1024 ;tileCount++) 
	{
		UINT32 code = BjVidRam[tileCount] + 16 * (BjColRam[tileCount] & 0x10);
		UINT32 color = BjColRam[tileCount] & 0x0f;

		UINT32 offs = map_offset_lut[tileCount];
		ss_map[offs] = color << 12 | code;
	}
}

static void BjRenderBgLayer(UINT32 BgSel)
{
//	INT32 BgSel=BjRam[0x9e00];

	for (UINT32 tileCount = 0; tileCount < 256;tileCount++) 
	{
		UINT32 offs = (BgSel & 0x07) * 0x200 + tileCount;
		UINT32 Code = (BgSel & 0x10) ? BjMap[offs] : 0;

		UINT32 attr = BjMap[offs + 0x100];
		UINT32 Colour = attr & 0x0f;

		offs = mapbg_offset_lut[tileCount];
		ss_map2[offs] = Colour << 12 | Code;
	}
}


static void BjDrawSprites()
{
	INT32 offs,delta=3;

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
		UINT32 flipx, flipy, code, colour, big;

		flipx = (BjSprRam[offs+1] & 0x80)>>3;
		flipy = (BjSprRam[offs+1] & 0x40)>>1;

		code   = BjSprRam[offs] & 0x7f;
		colour = (BjSprRam[offs+1] & 0x0f);
		big      = (BjSprRam[offs] & 0x80);

		ss_sprite[delta].control		= ( JUMP_NEXT | FUNC_NORMALSP | flipx | flipy);
		ss_sprite[delta].drawMode	= ( COLOR_0 | COMPO_REP);
		ss_sprite[delta].ax				= BjSprRam[offs+2];
		ss_sprite[delta].ay				= BjSprRam[offs+3];
		ss_sprite[delta].color			= colour<<3;

		if (!big)
		{
			ss_sprite[delta].charSize= 0x210;
		}
		else
		{
			code&=31;
			code*=4;
			code+=0x80;
			ss_sprite[delta].charSize= 0x420;
		}
		ss_sprite[delta].charAddr	= 0x220+((code)<<4 );
		delta++;
	}
}

static INT32 BjFrame()
{
	if (DrvReset) 
	{	// Reset machine
		DrvDoReset();
	}
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"BjFrame                  ",10,70);

	INT32 nInterleave = 10;
	INT32 nSoundBufferPos1 = 0;

	nCyclesTotal[0] = 4000000 / 600;
	nCyclesTotal[1] = 3000000 / 600;
	nCyclesDone[0] = nCyclesDone[1] = 0;
	signed short *nSoundBuffer = (signed short *)0x25a20000;

	for (INT32 i = 0; i < nInterleave; i++) 
	{
		INT32 nCurrentCPU, nNext;


#ifdef RAZE
		nNext = (i + 1) * nCyclesTotal[0];// / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[0];
		nCyclesDone[0] += z80_emulate(nCyclesSegment);
		if (i == 1) 
		{
			if(bombjackIRQ)
			{
				z80_cause_NMI();
			}
		}
#else
		// Run Z80 #1
		nCurrentCPU = 0;
		CZetOpen(nCurrentCPU);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU];// / nInterleave;
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
#endif
		// Run Z80 #2
		nCurrentCPU = 1;
		CZetOpen(nCurrentCPU);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU];// / nInterleave;
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
/*
		INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos1;

		SoundUpdate(&nSoundBuffer[nSoundBufferPos],nSegmentLength);
		nSoundBufferPos1 += nSegmentLength;	*/
	}
/*
	INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos1;
	if (nSegmentLength) 
	{
		SoundUpdate(&nSoundBuffer[nSoundBufferPos],nSegmentLength);
	}
*/
	SoundUpdate(&nSoundBuffer[nSoundBufferPos],nBurnSoundLen);

	// Make sure the buffer is entirely filled.
/*	if (pBurnSoundOut) {
		INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos;
		INT16* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
		if (nSegmentLength) {
			AY8910Render(&pAY8910Buffer[0], pSoundBuf, nSegmentLength, 0);
		}
	}
*/
	CZetOpen(1);
	CZetNmi();
	CZetClose();

	CalcAll();
	if(BgSel!=BjRam[0x9e00])
	{
		BgSel=BjRam[0x9e00];
		BjRenderBgLayer(BgSel);
	}
	//BjRenderFgLayer();
	BjDrawSprites();

	return 0;
}
//-------------------------------------------------------------------------------------------------
void SoundUpdate(INT16* buffer, INT32 length)
{
	int nSample;
	int n;
//	unsigned int deltaSlave;//soundLenSlave;//,titiSlave;
//	signed short *nSoundBuffer = (signed short *)0x25a20000;
//	deltaSlave    = *(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);

//	soundLenSlave = SOUND_LEN);
	AY8910Update(0, &pAY8910Buffer[0], length);
	AY8910Update(1, &pAY8910Buffer[3], length);
	AY8910Update(2, &pAY8910Buffer[6], length);

	for (n = 0; n < length; n++) 
	{
		nSample  = pAY8910Buffer[0][n] >> 2;
		nSample += pAY8910Buffer[1][n] >> 2;
		nSample += pAY8910Buffer[2][n] >> 2;
		nSample += pAY8910Buffer[3][n] >> 2;
		nSample += pAY8910Buffer[4][n] >> 2;
		nSample += pAY8910Buffer[5][n] >> 2;
		nSample += pAY8910Buffer[6][n] >> 2;
		nSample += pAY8910Buffer[7][n] >> 2;
		nSample += pAY8910Buffer[8][n] >> 2;

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
//		nSoundBuffer[deltaSlave + n] = nSample;//pAY8910Buffer[5][n];//nSample;
		buffer[n] = nSample;//pAY8910Buffer[5][n];//nSample;
	}

//	if(deltaSlave>=RING_BUF_SIZE/2)
	if(nSoundBufferPos>=RING_BUF_SIZE/2)
	{
//		deltaSlave=0;
		nSoundBufferPos=0;
		PCM_Task(pcm); // bon emplacement
	}

//	deltaSlave+=nBurnSoundLen;
	nSoundBufferPos+=length;
}

/*static*/ void updateSound()
{
	int nSample;
	int n;
	unsigned int deltaSlave;//soundLenSlave;//,titiSlave;
	signed short *nSoundBuffer = (signed short *)0x25a20000;
	deltaSlave    = *(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);

//	soundLenSlave = SOUND_LEN);
	AY8910Update(0, &pAY8910Buffer[0], nBurnSoundLen);
	AY8910Update(1, &pAY8910Buffer[3], nBurnSoundLen);
	AY8910Update(2, &pAY8910Buffer[6], nBurnSoundLen);

	for (n = 0; n < nBurnSoundLen; n++) 
	{
		nSample  = pAY8910Buffer[0][n] >> 2;
		nSample += pAY8910Buffer[1][n] >> 2;
		nSample += pAY8910Buffer[2][n] >> 2;
		nSample += pAY8910Buffer[3][n] >> 2;
		nSample += pAY8910Buffer[4][n] >> 2;
		nSample += pAY8910Buffer[5][n] >> 2;
		nSample += pAY8910Buffer[6][n] >> 2;
		nSample += pAY8910Buffer[7][n] >> 2;
		nSample += pAY8910Buffer[8][n] >> 2;

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
		nSoundBuffer[deltaSlave + n] = nSample;//pAY8910Buffer[5][n];//nSample;
	}

	if(deltaSlave>=RING_BUF_SIZE/2)
	{
		deltaSlave=0;
		PCM_Task(pcm); // bon emplacement
	}

	deltaSlave+=nBurnSoundLen;

	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = deltaSlave;
}
