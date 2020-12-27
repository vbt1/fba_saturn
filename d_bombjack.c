#include "d_bombjack.h"
#define RAZE 1
#define SOUND 1
void SoundUpdate2(INT32 *length2);
void DecodeTiles16_4Bpp(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3);
INT32 CalcAll();
UINT32 BgSel=0xFFFF;
//UINT32 bg_cache[1024];

int ovlInit(char *szShortName)
{
	cleanBSS();

	struct BurnDriver nBurnDrvBombjack = {
		"bombja", NULL,
		"Bomb Jack (set 1)",
		BombjackRomInfo,BombjackRomName, BombjackInputInfo,BombjackDIPInfo,
		DrvInit,DrvExit,DrvFrame
	};

	memcpy(shared,&nBurnDrvBombjack,sizeof(struct BurnDriver));
	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initLayers()
{

    Uint16	CycleTb[]={
		0x4e12, 0x6fff, //A0
		0xffff, 0xffff,	//A1
		0xff5f, 0xffff,   //B0
		0xffff, 0xffff  //B1
//		0x4eff, 0x1fff, //B1
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
void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
	SCL_AllocColRam(SCL_NBG3,OFF);
	colAddr = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
//	SCL_SetColRamOffset(SCL_NBG2,0,OFF);
	ss_regs->dispenbl &= 0xfbff;

	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void make_lut(void)
{
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
void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);

	nBurnSprites  = 24+4;//27;

	SS_FONT        = ss_font     =(Uint16 *)SCL_VDP2_VRAM_A1;
	SS_MAP          = ss_map    =(Uint16 *)SCL_VDP2_VRAM_A1+0x08000;
	SS_MAP2        = ss_map2  =(Uint16 *)SCL_VDP2_VRAM_A1+0x0B000;
	SS_CACHE      = cache      =(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_reg->n1_move_x = -8<<16;
	ss_reg->n2_move_x = 8;

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

    ss_sprite[nBurnSprites-1].control			= CTRL_END;
    ss_sprite[nBurnSprites-1].link				= 0;        
    ss_sprite[nBurnSprites-1].drawMode	= 0;                
    ss_sprite[nBurnSprites-1].color			= 0;                
    ss_sprite[nBurnSprites-1].charAddr		= 0;                
    ss_sprite[nBurnSprites-1].charSize		= 0;

	nBurnFunction = CalcAll;
	drawWindow(0,240,0,6,66); 
}
//-------------------------------------------------------------------------------------------------------------------------------------
UINT8 __fastcall bombjack_main_read(UINT16 address)
{
	switch (address)
	{
		case 0xb000:
		case 0xb001:
		case 0xb002:
			return DrvInputs[address & 3];

		case 0xb003:
			return 0; // watchdog?

		case 0xb004:
		case 0xb005:
			return DrvDips[address & 1];
	}

	return 0;
}


#ifdef RAZE
void __fastcall BjMemWrite_9000(UINT16 addr,UINT8 val)
{
	addr &=0x3ff;

	if (BjVidRam[addr]!=val)
	{
		BjVidRam[addr]=val;
		UINT32 code = val + ((BjColRam[addr] & 0x10) << 4);
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
		UINT32 code = BjVidRam[addr] + ((val & 0x10) << 4);

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

void __fastcall BjMemWrite_b000(UINT16 addr,UINT8 data)
{
	if (addr==0xb000)
	{
		nmi_mask = data & 1;
	}
	BjRam[addr]=data;
}

void __fastcall BjMemWrite_b800(UINT16 addr,UINT8 data)
{
	soundlatch=data;
}
#endif
void __fastcall BjMemWrite(UINT16 addr,UINT8 data)
{
	if (addr >= 0x9820 && addr <= 0x987f) { BjSprRam[addr - 0x9820] = data; return; }

	if(addr==0xb800)
	{
		soundlatch=data;
		return;
	}
	
	if (addr==0xb000)
	{
		nmi_mask = data & 1;
	}
	BjRam[addr]=data;
}

UINT8 __fastcall bombjack_sound_read(UINT16 address)
{
	switch (address)
	{
		case 0x6000:
		{
			UINT8 ret = soundlatch;
			soundlatch = 0;
			return ret;
		}
	}

	return 0;
}

void __fastcall bombjack_sound_write_port(UINT16 port, UINT8 data)
{
	switch (port & 0xff)
	{
		case 0x00:
		case 0x01:
			AY8910Write(0, port & 1, data);
		return;

		case 0x10:
		case 0x11:
			AY8910Write(1, port & 1, data);
		return;

		case 0x80:
		case 0x81:
			AY8910Write(2, port & 1, data);
		return;
	}
}


INT32 DrvZInit()
{
	// Init the z80
	CZetInit2(2,CZ80Context);
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
	z80_add_read(0xb000, 0xb005, 1, (void *)&bombjack_main_read);

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

	CZetSetReadHandler(bombjack_main_read);
	CZetSetWriteHandler(BjMemWrite);
	CZetClose();
#endif
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
	CZetSetReadHandler(bombjack_sound_read);
	CZetSetOutHandler(bombjack_sound_write_port);
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
void DecodeTiles4Bpp(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
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
void DecodeTiles(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
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
void DecodeTiles16_4BppTile(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
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
void DecodeTiles32_4Bpp(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
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
void DecodeTiles16_4Bpp(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
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
INT32 DrvDoReset()
{
	nmi_mask = 0;
	soundlatch = 0;
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

INT32 MemIndex()
{
	UINT8 *Next; Next = Mem;

	BjRom		  = Next; Next += 0x10000;
//	BjGfx		  = Next; Next += 0x0F000;
	BjGfx		  = (UINT8 *)0x00200000;
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

	CZ80Context					= Next; Next += sizeof(cz80_struc)*2;
	pFMBuffer	= (INT16*)Next; Next += nBurnSoundLen * 9 * sizeof(INT16);
//	BjPalReal	= (UINT32*)Next; Next += 0x0080 * sizeof(UINT32);
	map_offset_lut = (UINT16*)Next; Next += 1024 * sizeof(UINT16);
	mapbg_offset_lut = (UINT16*)Next; Next += 256 * sizeof(UINT16);
	cram_lut = (UINT16*)Next; Next += 4096 * sizeof(UINT16);
	MemEnd = Next;

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
INT32 DrvInit()
{
	DrvInitSaturn();
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"DrvInitSaturn             ",10,70);
	// Allocate and Blank all required memory
	Mem = NULL;
	MemIndex();
	if ((Mem = (UINT8 *)BurnMalloc(MALLOC_MAX)) == NULL) return 1;
	memset(Mem, 0, MALLOC_MAX);
	MemIndex();
	make_lut();
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"BurnMalloc                  ",10,70);
	
	INT32 RomOffset = 0;

	for (INT32 i = 0; i < 5; i++) {
		BurnLoadRom(BjRom + (0x2000 * i), i, 1);
	}
		
	RomOffset = 5;

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
	DrvZInit();

	UINT8 *ss_vram    = (UINT8 *)SS_SPRAM;
	UINT8 *sprites	  = &ss_vram[0x1100];
	UINT8 *tiles	  = (UINT8 *)SCL_VDP2_VRAM_B0;
	UINT8 *	text	  = cache;

	DecodeTiles4Bpp(text,512,0,0x1000,0x2000);
	DecodeTiles16_4Bpp(sprites,1024,0x7000,0x5000,0x3000);
	DecodeTiles32_4Bpp(sprites+0x4000,32,0x7000,0x5000,0x3000);
	DecodeTiles16_4BppTile(tiles,1024,0x9000,0xB000,0xD000);

	DrvDoReset();
	return 0;
}

INT32 DrvExit()
{	 
	nBurnFunction = NULL;
	wait_vblank();	
	DrvDoReset();
//	SPR_InitSlaveSH();
#ifdef RAZE
	z80_stop_emulating();

	z80_add_read(0x9820, 0x987f, 1, (void *)NULL);
	z80_add_read(0xb000, 0xb005, 1, (void *)NULL);

	z80_add_write(0x9000,0x93ff, 1, (void *)NULL);
	z80_add_write(0x9400,0x97ff, 1, (void *)NULL);

	z80_add_write(0x9820, 0x987f, 1, (void *)NULL);
	z80_add_write(0xb000, 0xb000, 1, (void *)NULL);
	z80_add_write(0xb800, 0xb800, 1, (void *)NULL);
#endif

	CZetExit2();

	AY8910Exit(0);
	AY8910Exit(1);
	AY8910Exit(2);

	cram_lut = map_offset_lut = mapbg_offset_lut = NULL;
	CZ80Context = MemEnd = RamStart = RamEnd = BjGfx = BjMap = BjRom = BjRam = BjColRam = NULL;
	BjVidRam = BjSprRam = SndRom = SndRam = BjPalSrc = NULL;

	for (int i = 0; i < 9; i++) {
		pAY8910Buffer[i] = NULL;
	}

//	free (pFMBuffer);
	pFMBuffer = NULL;
//	GenericTilesExit();
	free(Mem);
	Mem = NULL;
	BgSel = 0;

	cleanDATA();
	cleanBSS();

	return 0;
}

UINT32 CalcCol(UINT16 nColour)
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

INT32 CalcAll()
{
	UINT32 delta=0;
	for (UINT32 i = 0; i < 0x100; i+=2) 
	{
		colAddr[i / 2] = colBgAddr[delta] = cram_lut[BjPalSrc[i] | (BjPalSrc[i+1] << 8)];
//		CalcCol(BjPalSrc[i] | (BjPalSrc[i+1] << 8));
		 		delta++; if ((delta & 7) == 0) delta += 8;
	}

	return 0;
}

void BjRenderBgLayer(UINT32 BgSel)
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


void draw_sprites()
{
	INT32 offs;
	SprSpCmd *ss_spritePtr = &ss_sprite[3];
	
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
//		ss_spritePtr->ax				= BjSprRam[offs+2] & 0xff;
		UINT32 flipx, flipy, code, colour;//, big;

		flipx = (BjSprRam[offs+1] & 0x80)/8;
		flipy = (BjSprRam[offs+1] & 0x40)>>1;

		code   = BjSprRam[offs] & 0x7f;
		colour = (BjSprRam[offs+1] & 0x0f);
//			big      = (BjSprRam[offs] & 0x80);

		ss_spritePtr->control		= ( JUMP_NEXT | FUNC_NORMALSP | flipx | flipy);
//			ss_spritePtr->drawMode	= ( COLOR_0 | COMPO_REP);
		ss_spritePtr->ax				= BjSprRam[offs+2];
		ss_spritePtr->ay				= BjSprRam[offs+3];
		ss_spritePtr->color			= colour<<3;

		if (!(BjSprRam[offs] & 0x80))
		{
			ss_spritePtr->charSize= 0x210;
		}
		else
		{
			code&=31;
			code*=4;
			code+=0x80;
			ss_spritePtr->charSize= 0x420;
		}
		ss_spritePtr->charAddr	= 0x220+((code)<<4 );

		*ss_spritePtr++;
	}
}

INT32 DrvFrame()
{
	{
		DrvInputs[0] = 0x00;
		DrvInputs[1] = 0x00;
		DrvInputs[1] = 0x00;

		for (INT32 i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
			DrvInputs[2] ^= (DrvJoy3[i] & 1) << i;
		}
	}
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"BjFrame                  ",10,70);

	INT32 nInterleave = 10;
//	INT32 nInterleave = 5;
	INT32 nSoundBufferPos1 = 0;
	INT32 nCyclesSegment = 0;

	UINT32 nCyclesDone[2] = {0,0};
//	UINT32 nCyclesTotal[2] = {4000000 / 600,3000000 / 600};
	UINT32 nCyclesTotal[2] = {3000000 / 600,2250000 / 600};

	signed short *nSoundBuffer = (signed short *)0x25a20000;

	for (INT32 i = 0; i < nInterleave; i++) 
	{
		UINT32 nNext;

#ifdef RAZE
		nNext = (i + 1) * nCyclesTotal[0];// / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[0];
		nCyclesDone[0] += z80_emulate(nCyclesSegment);

		if (nmi_mask && i == (nInterleave - 1)) z80_cause_NMI();
#else
		// Run Z80 #1
		CZetOpen(0);
		nNext = (i + 1) * nCyclesTotal[0];// / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[0];
		nCyclesDone[0] += CZetRun(nCyclesSegment);
		if (nmi_mask && i == (nInterleave - 1)) CZetNmi();
		CZetClose();
#endif

		// Run Z80 #2
		CZetOpen(1);
		nNext = (i + 1) * nCyclesTotal[1];// / nInterleave;
		nCyclesSegment = nNext - nCyclesDone[1];
		nCyclesSegment = CZetRun(nCyclesSegment);
		nCyclesDone[1] += nCyclesSegment;
		if (i == (nInterleave - 1)) CZetNmi();
		CZetClose();

		// Render Sound Segment
/*		if (pBurnSoundOut) {
			INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos;
			INT16* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
			AY8910Render(&pAY8910Buffer[0], pSoundBuf, nSegmentLength, 0);
			nSoundBufferPos += nSegmentLength;
		}		 */
#ifdef SOUND2
//		SPR_WaitEndSlaveSH();
		INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos1;


//		SoundUpdate(&nSoundBuffer[nSoundBufferPos],nSegmentLength);
		SPR_RunSlaveSH((PARA_RTN*)SoundUpdate2, &nSegmentLength);

		nSoundBufferPos1 += nSegmentLength;
#endif
	}
#ifdef SOUND
	INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos1;
	if (nSegmentLength) 
	{
		SoundUpdate(&nSoundBuffer[nSoundBufferPos],nSegmentLength);
	}
#endif
//	SPR_RunSlaveSH((PARA_RTN*)BjDrawSprites, NULL);
//	CalcAll();

	if(BgSel!=BjRam[0x9e00])
	{
		BgSel=BjRam[0x9e00];
		BjRenderBgLayer(BgSel);
	}
	//BjRenderFgLayer();
	draw_sprites();
//	 SPR_WaitEndSlaveSH();
	return 0;
}

void AY8910Update1Slave(INT32 *length)
{
	AY8910Update(1, &pAY8910Buffer[3], length[0]);
}
#ifdef SOUND2
//-------------------------------------------------------------------------------------------------
void SoundUpdate2(INT32 *length2)
{
	int nSample;
	unsigned int	 length = length2[0];
	unsigned int deltaSlave;//soundLenSlave;//,titiSlave;
	signed short *nSoundBuffer = (signed short *)0x25a20000;
	deltaSlave    = *(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);
	INT16* buffer = &nSoundBuffer[deltaSlave];
//	soundLenSlave = SOUND_LEN);
	AY8910Update(0, &pAY8910Buffer[0], length);
	AY8910Update(1, &pAY8910Buffer[3], length);
	AY8910Update(2, &pAY8910Buffer[6], length);

	for (unsigned int n = 0; n < length; n++) 
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

		nSample /= 4;

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
		*buffer++ = nSample;//pAY8910Buffer[5][n];//nSample;
	}
	deltaSlave+=length;

//	if(deltaSlave>=RING_BUF_SIZE/2)
	if(deltaSlave>=RING_BUF_SIZE/2)
	{
		PCM_NotifyWriteSize(pcm, deltaSlave);
		PCM_Task(pcm); // bon emplacement
//		deltaSlave=0;
		deltaSlave=0;
	}
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = deltaSlave;

//	deltaSlave+=nBurnSoundLen;
}
#endif
//-------------------------------------------------------------------------------------------------
void SoundUpdate(INT16* buffer, INT32 length)
{
	int nSample;
//	unsigned int deltaSlave;//soundLenSlave;//,titiSlave;
//	signed short *nSoundBuffer = (signed short *)0x25a20000;
//	deltaSlave    = *(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);

//	soundLenSlave = SOUND_LEN);
	SPR_RunSlaveSH((PARA_RTN*)AY8910Update1Slave, &length);
	AY8910Update(0, &pAY8910Buffer[0], length);
//	AY8910Update(1, &pAY8910Buffer[3], length);
//	SPR_RunSlaveSH((PARA_RTN*)AY8910Update1Slave, &length);
	AY8910Update(2, &pAY8910Buffer[6], length);
	SPR_WaitEndSlaveSH();

	for (unsigned int n = 0; n < length; n++) 
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

		nSample /= 4;

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
		*buffer++ = nSample;//pAY8910Buffer[5][n];//nSample;
	}
	nSoundBufferPos+=length;

//	if(deltaSlave>=RING_BUF_SIZE/2)
	if(nSoundBufferPos>=RING_BUF_SIZE/2)
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos);
		PCM_Task(pcm); // bon emplacement
		nSoundBufferPos=0;
	}

//	deltaSlave+=nBurnSoundLen;
}
