
// 43-44 fps with CZ80 main & RAZE slave
// 17-18 fps with RAZE main & CZ80 slave
// FB Alpha Penguin-Kun Wars Driver Module
// Based on MAME Driver by David Haywood and Phil Stroffolino
#define CZ80 1
#define RAZE 1  // `EMULATE_R_REGISTER obligatoire
//#define USE_RAZE0 1
#define USE_RAZE1 1
#define CPU2_ENABLED 1
#define charSize(x, y) ((Uint16)( ((x >> 3) << 8) | (y)) )

#include "d_sys2.h"
#include "d_sys1_common.c"

UINT8 *System1MC8123Key=NULL;

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvChplftb = {
		"chplftb", "sys2",
		"Choplifter (Alternate)\0",
		ChplftbRomInfo, ChplftbRomName, ChplftbInputInfo, ChplftbDIPInfo,
		ChplftbInit, System1Exit, System1Frame, NULL//, NULL//,
	};

	struct BurnDriver nBurnDrvWbml = {
		"wbml", "sys2",
		"Wonder Boy in Monster Land (Jp New)\0",
		wbmlRomInfo, wbmlRomName, MyheroInputInfo, WbmlDIPInfo,
		WbmlInit, System1Exit, System1Frame, NULL
	};

	struct BurnDriver nBurnDrvWbmlb = {
		"wbmlb", "sys2",
		"Wonder Boy in Monster Land (English bootleg set 1)\0",
		wbmlbRomInfo, wbmlbRomName, MyheroInputInfo, WbmlDIPInfo,
		WbmljbInit, System1Exit, System1Frame, NULL
	};

	if (strcmp(nBurnDrvChplftb.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvChplftb,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvWbml.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvWbml,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvWbmlb.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvWbmlb,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}

/*static*/ UINT8 __fastcall WbmlZ801PortRead(unsigned short a)
{
	a &= 0xff;
	switch (a) 
	{
		case 0x00: return 0xff - System1Input[0];
		case 0x04: return 0xff - System1Input[1];
		case 0x08: return 0xff - System1Input[2];
		case 0x0c: return System1Dip[0];
		case 0x0d: return System1Dip[1];
		case 0x10: return System1Dip[1];
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17: return ppi8255_r(0, a & 3);
		case 0x19: return System1BankSwitch;
	}
	return 0;
}

inline void wbml_videoram_bank_latch_w (UINT8 d)
{
//	if(System1BgBankLatch != d)
	{
		System1BgBankLatch = d;
		System1BgBank = (d >> 1) & 0x03;	/* Select 4 banks of 4k, bit 2,1 */
		UINT8 *CurrentBank = (UINT8 *)(System1VideoRam + System1BgBank * 0x1000);
		RamStart1 = CurrentBank - 0xe000;
		// iq_132
		CZetMapArea(0xe000, 0xefff, 0, CurrentBank);
		CZetMapArea(0xe800, 0xefff, 1, CurrentBank+0x800);
		CZetMapArea(0xe000, 0xefff, 2, CurrentBank);
	}
}

void system2_foregroundram_w(unsigned short a, UINT8 d) 
{
	if(RamStart1[a]!=d)
	{
		RamStart1[a] = d;
		a&=~1;

		unsigned int Code = (RamStart1[a + 1] << 8) | RamStart1[a + 0];
		Code = ((Code >> 4) & 0x800) | (Code & 0x7ff);

		unsigned int x = map_offset_lut[a&0x7ff];
		ss_map2[x]   = (Code >> 5) & 0x3f; // |(((RamStart[a + 1] & 0x08)==8)?0x2000:0x0000);;//color_lut[Code];
		ss_map2[x+1] = Code & (System1NumTiles-1);
	}
}

void system2_backgroundram_w(unsigned short a, UINT8 d, unsigned char start)
{
//		UINT8 *source = System1VideoRam + (System1VideoRam[0x0740 + page*2] & 0x07)*0x800;
//	if(RamStart[a]!=d)
	{
		RamStart[a] = d;
		a&=~1;
		int Code;//, Colour;
		Code = (RamStart[a + 1] << 8) | RamStart[a + 0];
		Code = ((Code >> 4) & 0x800) | (Code & 0x7ff);

		unsigned int x = map_offset_lut[a&0x7ff];
		UINT16 *map = &ss_map[x]; 
		map[start]     = ((Code >> 5) & 0x3f)|(((RamStart[a + 1] & 0x08)==8)?0x2000:0x0000);//color_lut[Code];
		map[start+1] = Code & (System1NumTiles-1);
	}
}



void __fastcall WbmlZ801PortWrite(unsigned short a, UINT8 d)
{
	a &= 0x1f;
	switch (a) 
	{
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
			ppi8255_w(0, a & 3, d);
		return;
	}
}

void __fastcall WbmlZ801ProgWrite(UINT16 a, UINT8 d)
{
	if (a >= 0xf000 && a <= 0xf3ff) { System1BgCollisionRam[a - 0xf000] = 0x7e; return; }
	if (a >= 0xf800 && a <= 0xfbff) { System1SprCollisionRam[a - 0xf800] = 0x7e; return; }

	if (a >= 0xe000 && a <= 0xe7ff) 
	{
//		const int v[] = { 0, 0x40, 0x1000, 0x1040 };
		if(!System1BgBank)
		{
			system2_foregroundram_w(a, d);
			return;
		}
		RamStart1[a] = d;
		return;
	}
	if (a >= 0xe800 && a <= 0xefff) 
	{
		RamStart1[a] = d;
		return;
	}

	if (a >= 0xd800 && a <= 0xd9ff) { system1_paletteram_w(a,d); return; }
	if (a >= 0xda00 && a <= 0xdbff) { system1_paletteram2_w(a,d); return; }
	if (a >= 0xdc00 && a <= 0xddff) { system1_paletteram3_w(a,d); return; }
}

UINT8 __fastcall WbmlZ801ProgRead(unsigned short a)
{
	if (a >= 0xe000 && a <= 0xefff) 
	{ 
		return System1VideoRam[(0x1000*System1BgBank) + (a & 0xfff)];
	}
	return 0;
}

static void WbmlPPI0WriteA(UINT8 data)
{
	system1_soundport_w(data);
}

static void WbmlPPI0WriteB(UINT8 data)
{
	chplft_bankswitch_w(data);
}

static void WbmlPPI0WriteC(UINT8 data)
{
// refaire avec raze
	CZetClose();
	CZetOpen(1);
	CZetSetIRQLine(0x20, (data & 0x80) ? CZET_IRQSTATUS_NONE : CZET_IRQSTATUS_ACK);
	CZetClose();
	CZetOpen(0);

	wbml_videoram_bank_latch_w(data);
}

/*static*/ inline void chplft_bankswitch_w (UINT8 d)
{
	System1RomBank = (((d & 0x0c)>>2) );
	System1BankRomNoDecode();
	System1BankSwitch = d;
}

/*static*/ void __fastcall ChplftZ801ProgWrite(unsigned short a, UINT8 d)
{
	if (a >= 0xe000 && a <= 0xe7bf) { system1_foregroundram_w(a,d); return; }
	if (a >= 0xe800 && a <= 0xeeff) { system1_backgroundram_w(a,d); return; }
	if (a >= 0xf000 && a <= 0xf3ff) { System1BgCollisionRam[a - 0xf000] = 0x7e; return; }
	if (a >= 0xf800 && a <= 0xfbff) { System1SprCollisionRam[a - 0xf800] = 0x7e; return; }
//	if (a >= 0xd800 && a <= 0xd9ff) { system1_paletteram_w(a,d); return; }
	if (a >= 0xda00 && a <= 0xdbff) { system1_paletteram2_w(a,d); return; }
	if (a >= 0xdc00 && a <= 0xddff) { system1_paletteram3_w(a,d); return; }
   if (a == 0xefbd) { ss_reg->n0_move_y = d<<16; return; }
}

/*static*/ int System1CalcSprPalette()
{
	for (int i = 511; i > 0; i--) 
	{
		colAddr[i] = cram_lut[System1PaletteRam[i]];
	}
	return 0;
}

static void wbmljb_decode()
{
	return; // fake decode function
}

static void wbml_decode()
{
	mc8123_decrypt_rom(1, 4, System1Rom1, System1Rom1 + 0x20000, System1MC8123Key);
}

/*static*/ int ChplftbInit()
{
	int nRet;
	nBurnLinescrollSize = 0x380;
	System1ColourProms = 1;
	System1BankedRom = 1;

	nRet = System1Init(3, 0x8000, 1, 0x8000, 3, 0x8000, 4, 0x8000, 1);
	initColors2();
	initLayers2();
	nBurnFunction = System1CalcSprPalette;//System1CalcPalette;
	ss_reg->n1_move_y =  0 <<16;
	ss_reg->n1_move_x =  0 <<16;
	make_cram_lut();
	System1CalcPalette();
//	System1CalcSprPalette();
	SS_SET_S0PRIN(4);
	SS_SET_N1PRIN(7);
	SS_SET_N2PRIN(4);
	SS_SET_N0PRIN(5);

	SS_SET_N0SPRM(1);  // 1 for special priority

	initScrolling(ON,SCL_VDP2_VRAM_B0+0x4000);
	drawWindow(0,224,0,0,64);

	RamStart						= System1BgRam-0xe800;
	RamStart1						= System1VideoRam-0xe000;
#ifndef USE_RAZE0
	CZetOpen(0);

	CZetMapArea(0xd800, 0xddff, 0, System1PaletteRam);
//	CZetMapArea(0xd800, 0xddff, 1, System1PaletteRam);
	CZetMapArea(0xd800, 0xddff, 1, System1PaletteRam);
//	CZetMapArea(0xd800, 0xddff, 2, System1PaletteRam);

	CZetMapArea(0xe7c0, 0xe7ff, 0, System1ScrollXRam);
	CZetMapArea(0xe7c0, 0xe7ff, 1, System1ScrollXRam);
	CZetMapArea(0xe7c0, 0xe7ff, 2, System1ScrollXRam);


	CZetMapArea(0xe000, 0xe7ff, 0, System1VideoRam); //read
//	CZetMapArea(0xe000, 0xe7ff, 1, System1VideoRam);	 //write
	CZetMapArea(0xe000, 0xe7ff, 2, System1VideoRam); //fetch
	CZetMapArea(0xe800, 0xeeff, 0, System1BgRam);
//	CZetMapArea(0xe800, 0xeeff, 1, System1BgRam);
	CZetMapArea(0xe800, 0xeeff, 2, System1BgRam);

	CZetSetWriteHandler(ChplftZ801ProgWrite);

	CZetSetInHandler   (WbmlZ801PortRead);
	CZetSetOutHandler(WbmlZ801PortWrite);
	CZetClose();

	ppi8255_init(1);
	PPI0PortWriteA = WbmlPPI0WriteA;
	PPI0PortWriteB = WbmlPPI0WriteB;
	PPI0PortWriteC = NULL;

#endif
//	nBurnFunction = System1CalcPalette;
//	System1DoReset();
	return nRet;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void CommonWbmlInit()
{
	System1SpriteRam = &System1Ram1[0x1000];
	System1PaletteRam = &System1Ram1[0x1800];	 // à garder

	make_cram_lut();
	System1CalcPalette();

//   nBurnFunction = System1CalcPalette;
//	nBurnFunction = System1CalcSprPalette;//System1CalcPalette;
	ss_reg->n1_move_x = 4<<16;
	RamStart						= System1VideoRam-0xe000; // bg
	RamStart1						= System1VideoRam-0xe000;	 // fg
	drawWindow(0,224,0,0,65);

	CZetOpen(0);
	CZetMapArea2(0x0000, 0x7fff, 2, System1Rom1 + 0x20000, System1Rom1);
	CZetMapArea2(0x8000, 0xbfff, 2, System1Rom1 + 0x30000, System1Rom1 + 0x10000);  // 30 fetch et 10 pour code ?
	
	CZetMapArea(0xd800, 0xddff, 0, System1PaletteRam);
	CZetMemCallback(0xd800, 0xddff, 1);

	CZetMapArea(0xd000, 0xd1ff, 1, System1SpriteRam);

	CZetMemCallback(0xe000, 0xefff, 0);
	CZetMemCallback(0xe000, 0xefff, 1);
	CZetMemCallback(0xe000, 0xefff, 2);

	CZetMapArea(0xe000, 0xefff, 0, System1VideoRam); //read
//	CZetMapArea(0xe000, 0xefff, 1, System1VideoRam);	 //write
	CZetMapArea(0xe000, 0xefff, 2, System1VideoRam); //fetch

	CZetMemCallback(0xf000, 0xf3ff, 1);
	CZetMemCallback(0xf800, 0xfbff, 1);

	CZetSetReadHandler(WbmlZ801ProgRead);
	CZetSetWriteHandler(WbmlZ801ProgWrite);

	CZetSetInHandler   (WbmlZ801PortRead);
	CZetSetOutHandler(WbmlZ801PortWrite);
	CZetClose();

	ppi8255_init(1);
	PPI0PortWriteA = WbmlPPI0WriteA;
	PPI0PortWriteB = WbmlPPI0WriteB;
	PPI0PortWriteC = WbmlPPI0WriteC;

//	System1Draw = WbmlRender;
	memset(System1VideoRam,0x00,0x4000);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ int WbmljbInit()
{
	int nRet;

	System1ColourProms = 1;
	System1BankedRom = 2; // 2

	DecodeFunction = wbmljb_decode;
	nRet = System1Init(3, 0x10000, 1, 0x8000, 3, 0x8000, 4, 0x8000, 1);
	CommonWbmlInit();
	return nRet;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ int WbmlInit()
{
	int nRet;
	System1ColourProms = 1;
	System1BankedRom = 1;

	DecodeFunction = wbml_decode;

	System1MC8123Key = (UINT8*)0x00200000;
	memset(System1MC8123Key,0x00,0x2000);
	BurnLoadRom(System1MC8123Key, 15, 1);
	nRet = System1Init(3, 0x8000, 1, 0x8000, 3, 0x8000, 4, 0x8000, 1);
	System1MC8123Key = NULL;
	CommonWbmlInit();
	return nRet;
}

void renderTile(UINT32 offs,UINT32 code,UINT32 current_map)
{
	UINT32 color = (code >> 5) & 0x3f;
//				UINT32 priority = code & 0x800;
	code = ((code >> 4) & 0x800) | (code & 0x7ff);

	unsigned int x = map_offset_lut[offs];
	UINT16 *map = &ss_map[x]; 
	map[current_map] = ((code >> 5) & 0x3f); //|((code & 0x800)?0x3000:0x1000);//color_lut[Code];
	map[current_map+1] = code & (System1NumTiles-1);
}

unsigned int map_cache[4][0x800];

static void wbml_draw_bg( int trasp)
{
	ss_reg->n2_move_x = (260-(((System1VideoRam[0x7c0] >> 1) + ((System1VideoRam[0x7c1] & 1) << 7) - 256+5))) & 0xff;
	ss_reg->n2_move_y = System1VideoRam[0x7ba] & 0xff;

	const unsigned int v[] = { 0, 0x40, 0x1000, 0x1040 };

	for (unsigned int page=0; page < 4; page++)
	{
		unsigned int current_map=v[page];
		UINT8 *source = System1VideoRam + (System1VideoRam[0x0740 + page*2] & 0x07)*0x800;

		for(UINT32 offs = 0; offs <0x800;)
		{
			UINT32 code = source[offs + 0] + (source[offs + 1] << 8);
			if(map_cache[page][offs]!=code)
			{
 				map_cache[page][offs]=code;
				renderTile(offs,code,current_map);
			}
			offs+=2;
			code = source[offs + 0] + (source[offs + 1] << 8);
			if(map_cache[page][offs]!=code)
			{
 				map_cache[page][offs]=code;
				renderTile(offs,code,current_map);
			}
			offs+=2;
			code = source[offs + 0] + (source[offs + 1] << 8);
			if(map_cache[page][offs]!=code)
			{
 				map_cache[page][offs]=code;
				renderTile(offs,code,current_map);
			}
			offs+=2;
			code = source[offs + 0] + (source[offs + 1] << 8);
			if(map_cache[page][offs]!=code)
			{
 				map_cache[page][offs]=code;
				renderTile(offs,code,current_map);
			}
			offs+=2;

			code = source[offs + 0] + (source[offs + 1] << 8);
			if(map_cache[page][offs]!=code)
			{
 				map_cache[page][offs]=code;
				renderTile(offs,code,current_map);
			}
			offs+=2;
			code = source[offs + 0] + (source[offs + 1] << 8);
			if(map_cache[page][offs]!=code)
			{
 				map_cache[page][offs]=code;
				renderTile(offs,code,current_map);
			}
			offs+=2;
			code = source[offs + 0] + (source[offs + 1] << 8);
			if(map_cache[page][offs]!=code)
			{
 				map_cache[page][offs]=code;
				renderTile(offs,code,current_map);
			}
			offs+=2;
			code = source[offs + 0] + (source[offs + 1] << 8);
			if(map_cache[page][offs]!=code)
			{
 				map_cache[page][offs]=code;
				renderTile(offs,code,current_map);
			}
			offs+=2;
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void System1Render()
{
	System1DrawSprites();
	wbml_draw_bg(0);
}
//-------------------------------------------------------------------------------------------------------------------------------------
#if 1
void DrawSprite(unsigned int Num,unsigned int Bank, unsigned int addr, UINT16 Skip, UINT8 *SpriteBase)
{
	unsigned int Src = (SpriteBase[7] << 8) | SpriteBase[6];
	unsigned int Height = SpriteBase[1] - SpriteBase[0];
	unsigned int Width = width_lut[abs(Skip)];

	unsigned int values[] ={Src,Height,Skip,Width, Bank,nextSprite};
	spriteCache[addr]=nextSprite;

	{
	renderSpriteCache(values);
	unsigned int  nextaddr = nextSprite + (Height*Width)/8;

	nextSprite = nextaddr;
	
	unsigned int delta	= (Num+3);

	ss_sprite[delta].ax				= 11 + ((((SpriteBase[3] & 0x01) << 8) + SpriteBase[2] )/2);
	ss_sprite[delta].ay				= SpriteBase[0] + 1;
	ss_sprite[delta].charSize	= (Width<<6) + Height;
	ss_sprite[delta].color			= COLADDR_SPR | ((Num)<<2);
	ss_sprite[delta].charAddr	= 0x220+spriteCache[addr];

 //	int values2[] ={ss_sprite[delta].ax,ss_sprite[delta].ay,Skip,Height,Num};
//	updateCollisions(values2);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
//void DrawSpriteCache(unsigned int Num,unsigned int Bank, unsigned int addr,UINT16 Skip,UINT8 *SpriteBase)
void DrawSpriteCache(int Num,int Bank, int addr,INT16 Skip,UINT8 *SpriteBase)
{
	unsigned int Height = SpriteBase[1] - SpriteBase[0];
	unsigned int Width = width_lut[abs(Skip)];
	unsigned int delta	= (Num+3);

	{
//		vbx++;
	ss_sprite[delta].ax			= 11+ ((((SpriteBase[3] & 0x01) << 8) + SpriteBase[2] )/2);
	ss_sprite[delta].ay			= SpriteBase[0] + 1;
	ss_sprite[delta].charSize		= (Width<<6) + Height;
	ss_sprite[delta].color			= COLADDR_SPR | ((Num)<<2);
	ss_sprite[delta].charAddr		= 0x220+spriteCache[addr];

// 	int values[] ={ss_sprite[delta].ax,ss_sprite[delta].ay,Skip,Height,Num};
//	updateCollisions(values);
	}
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initColors2()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
 	colAddr             = (Uint16*)COLADDR;//(Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	colBgAddr         = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	SCL_AllocColRam(SCL_NBG0,OFF);
	colBgAddr2       = (Uint16*)SCL_AllocColRam(SCL_NBG0,ON);
	SCL_SetColRam(SCL_NBG1,8,4,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initLayers2()
{
    Uint16	CycleTb[]={
		0x2f64, 0xeeee, //A0
		0xffff, 0xffff,	//A1
		0xf6f0,0x55ee,   //B0
		0xffff, 0xffff  //B1
	};
 	SclConfig	scfg;

	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD;
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.plate_addr[0] = (Uint32)ss_map2;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.plate_addr[0] = (Uint32)ss_map;
	SCL_SetConfig(SCL_NBG0, &scfg);

	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.datatype 		 = SCL_BITMAP;
	scfg.mapover		 = SCL_OVER_0;
	scfg.plate_addr[0]	 = (Uint32)ss_font;
// 3 nbg	
	SCL_SetConfig(SCL_NBG1, &scfg);

	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
// <gamezfan> bootleg set 1 is the best outside of the virtual console version