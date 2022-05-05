// 43-44 fps with CZ80 main & RAZE slave
// 17-18 fps with RAZE main & CZ80 slave
// FB Alpha Penguin-Kun Wars Driver Module
// Based on MAME Driver by David Haywood and Phil Stroffolino
#define CZ80 1
#define RAZE 1  // `EMULATE_R_REGISTER obligatoire
#define USE_HANDLER_F 1
//#define USE_RAZE0 1
#define USE_RAZE1 1
#define CPU2_ENABLED 1
#define charSize(x, y) ((Uint16)( ((x >> 3) << 8) | (y)) )
#define SYS2 1

void SDMA_ScuCst(unsigned int ch, void *dst, void *src, unsigned int cnt);
unsigned int SDMA_ScuResult(unsigned int ch);
void CZetSetWriteHandler2(unsigned short nStart, unsigned short nEnd,void (*pHandler)(unsigned short, unsigned char));
#include "d_sys2.h"
#include "d_sys1_common.c"
static UINT8 *CurrentBank = NULL; // vbt à mettre dans drvexit !!!
void wbml_draw_bg();
UINT16 *map_cache;
UINT8 *map_dirty;

inline void system2_foregroundram_w(UINT16 a, UINT8 d) ;

int ovlInit(char *szShortName)
{
	cleanBSS();
/*
	struct BurnDriver nBurnDrvChplftb = {
		"chplftb", "sys2",
		"Choplifter (Alternate)",
		ChplftbRomInfo, ChplftbRomName, ChplftbInputInfo, ChplftbDIPInfo,
		ChplftbInit, System1Exit, System1Frame
	};
*/
/*	struct BurnDriver nBurnDrvWbml = {
		"wbml", "sys2",
		"Wonder Boy in Monster Land (Jp New)",
		wbmlRomInfo, wbmlRomName, MyheroInputInfo, WbmlDIPInfo,
		WbmlInit, System1Exit, System1Frame
	};

	struct BurnDriver nBurnDrvWbmlb = {
		"wbmlb", "sys2",
		"Wonder Boy in Monster Land (English bootleg set 1)",
		wbmlbRomInfo, wbmlbRomName, MyheroInputInfo, WbmlDIPInfo,
		WbmljbInit, System1Exit, System1Frame
	};
*/
	struct BurnDriver nBurnDrvWbmlvc = {
		"wbmlvc", "sys2",
		"Wonder Boy in Monster Land (EN VC)",
		wbmlvcRomInfo, wbmlvcRomName, MyheroInputInfo, WbmlDIPInfo,
		WbmljbInit, System1Exit, System1Frame
	};

//	if (strcmp(nBurnDrvChplftb.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvChplftb,sizeof(struct BurnDriver));
//	if (strcmp(nBurnDrvWbml.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvWbml,sizeof(struct BurnDriver));
//	if (strcmp(nBurnDrvWbmlb.szShortName, szShortName) == 0)	memcpy(shared,&nBurnDrvWbmlb,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvWbmlvc.szShortName, szShortName) == 0)memcpy(shared,&nBurnDrvWbmlvc,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
	
	return 0;
}
/*
inline void chplft_bankswitch_w (UINT8 d)
{
	System1BankRomNoDecode(((d & 0x0c)>>2));
	System1BankSwitch = d;
}

void __fastcall ChplftZ801PortWrite(UINT16 a, UINT8 d)
{
	a &= 0xff;
	switch (a)
	{
		case 0x14: { system1_soundport_w(d);    return; }
		case 0x15: { chplft_bankswitch_w(d);   return; }
	}
}
*/
void __fastcall System2Z801PortWrite(UINT16 a, UINT8 d)
{
	switch (a&0x1f) 
	{
		case 0x14:
		case 0x15:
		case 0x16:
		case 0x17:
			ppi8255_w(0, a & 3, d);
		return;
	}
}
/*
UINT8 __fastcall ChplftZ801PortRead(UINT16 a)
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
		case 0x15: return System1BankSwitch;
		case 0x16: return System1BgBankLatch;
		case 0x19: return System1BankSwitch;
	}
	return 0;
}*/

UINT8 __fastcall System2Z801PortRead(UINT16 a)
{
	a &= 0x1f;
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
//		case 0x19: return System1BankSwitch;
	}
	return 0;
}

inline void System2_videoram_bank_latch_w (UINT8 d)
{
	if(System1BgBankLatch != d)
	{
		System1BgBankLatch = d;
		System1BgBank = (d >> 1) & 0x03;	// Select 4 banks of 4k, bit 2,1 
		CurrentBank = (UINT8 *)(System1VideoRam + System1BgBank * 0x1000);
		RamStart1 = CurrentBank - 0xe000;
	}
}
#ifndef USE_HANDLER_F
 void system2_foregroundram_w(UINT16 a, UINT8 d) 
#else
void system2_foregroundram_w(UINT16 a, UINT8 d) 
#endif
{
	if(RamStart1[a]!=d)
	{
		RamStart1[a] = d;
	
		switch (a&0x7ff)
		{
			case 0x740: 
			case 0x742:
			case 0x744:
			case 0x746:
				map_dirty[d&7]=1;
			break;
		}		

		a&=~1;

		UINT8 *rs = (UINT8 *)(RamStart1+a);
		UINT16 Code = __builtin_bswap16(*((UINT16 *)rs));
		Code = ((Code >> 4) & 0x800) | (Code & 0x7ff);

		UINT16 *mapf2 = SS_MAP2+map_offset_lut[a&0x7ff];	
		mapf2[0] = (Code >> 5) & 0x3f;
		mapf2[1] = Code & (System1NumTiles-1);
	}
}

#ifndef USE_HANDLER_F
 void system2_backgroundram_w(UINT16 a, UINT8 d) 
#else
void system2_backgroundram_w(UINT16 a, UINT8 d) 
#endif
{
	if(CurrentBank[(a & 0xfff)]!=d)
	{
		CurrentBank[(a & 0xfff)] = d;

		switch (a&0xfff)
		{
			case 0x740: 
			case 0x742:
			case 0x744:
			case 0x746:
				map_dirty[d&7]=1;
			break;
		}

		a&=0xffe;

		UINT8 *rs = (UINT8 *)(CurrentBank+a);
		UINT16 Code = __builtin_bswap16(*((UINT16 *)rs));

		Code = ((Code >> 4) & 0x800) | (Code & 0x7ff);

		unsigned char real_page = (a>>11)+(System1BgBank<<1);
		map_dirty[real_page] = 1;
		
		UINT16 *map = &map_cache[(a&0x7ff)|(real_page*0x1000)];
		map[0] = ((Code >> 5) & 0x3f)|(((rs[1] & 0x08)==0x8)?0x2000:0x0000);
		map[1] = Code & (System1NumTiles-1);
	}
}

#ifndef  USE_HANDLER_F
void __fastcall system2_foregroundram_w2(UINT16 a, UINT8 d)
{
	if(System1BgBank==0)
	{
		system2_foregroundram_w(a,d);
	}
	else
		system2_backgroundram_w(a,d);
}
#else
void __fastcall System2Z801ProgWrite(UINT16 a, UINT8 d)
{
	if (a >= 0xe000 && a <= 0xefff) 
	{
		if(System1BgBank==0 && a <= 0xe7ff)
		{
			system2_foregroundram_w(a,d);
			return;
		}

		if(RamStart1[a]!=d)
		{
			RamStart1[a] = d;

			switch (a&0xfff)
			{
				case 0x740: 
				case 0x742:
				case 0x744:
				case 0x746:
					map_dirty[d&7]=1;
				break;
			}
			a&=0xffe;

			UINT8 *rs = (UINT8 *)(CurrentBank+a);
			UINT16 Code = __builtin_bswap16(*((UINT16 *)rs));

			Code = ((Code >> 4) & 0x800) | (Code & 0x7ff);

			unsigned char real_page = (a>>11)+(System1BgBank<<1);
			map_dirty[real_page] = 1;

			UINT16 *map = &map_cache[(a&0x7ff)|(real_page*0x1000)];
			map[0] = ((Code >> 5) & 0x3f)|(((rs[1] & 0x08)==0x8)?0x2000:0x0000);
			map[1] = Code & (System1NumTiles-1);
		}
		return; 
	}
	if (a >= 0xf000 && a <= 0xf3ff) 
	{ 
		System1BgCollisionRam[a&0x3ff] = 0x7e;
		return; 
	}
	if (a >= 0xf800 && a <= 0xfbff) 
	{ 
		System1SprCollisionRam[a&0x3ff]  = 0x7e;
		return; 
	}
	if (a >= 0xd800 && a <= 0xd9ff) 
	{ 	
		colAddr[a&0x1ff] = cram_lut[d]; 
		return; 
	}
	if (a >= 0xda00 && a <= 0xdbff) 
	{
		colBgAddr[remap8to16_lut[a&0x1ff]] = cram_lut[d];
		return;
	}

	if (a >= 0xdc00 && a <= 0xddff) 
	{	
		colBgAddr2[remap8to16_lut[a&0x1ff]] = cram_lut[d];
	}	
}

#endif
static UINT8 __fastcall System2Z801ProgRead(UINT16 a)
{
	if (a >= 0xe000 && a <= 0xefff) 
	{ 
		return RamStart1[a];
	}
	return 0;
}

static void System2PPI0WriteA(UINT8 data)
{
	system1_soundport_w(data);
}

static void System2PPI0WriteB(UINT8 data)
{
	System2_bankswitch_w(data);
}

static void System2PPI0WriteC(UINT8 data)
{
	if(data & 0x80)
	{
		z80_raise_IRQ(0x20);
//		z80_emulate(0);
	}
	else
	{
		z80_lower_IRQ();
//		z80_emulate(0);
	}

	System2_videoram_bank_latch_w(data);
}

inline void System2_bankswitch_w (UINT8 d)
{
	if(System1BankSwitch!=d)
	{
	System1BankRom((d & 0x0c) >> 2);
	System1BankSwitch = d;
	}
}

static void wbmljb_decode()
{
	return; // fake decode function
}

//-------------------------------------------------------------------------------------------------------------------------------------
void CommonWbmlInit()
{
	System1SpriteRam = &System1Ram1[0x1000];

	ss_reg->n1_move_x = 4<<16;
	CurrentBank						= System1VideoRam;
	RamStart1						= System1VideoRam-0xe000;	 // fg
	drawWindow(0,224,0,0,65);

	CZetOpen(0);


	CZetMemCallback(0xd000, 0xd1ff, 0);
	CZetMemCallback(0xd000, 0xd1ff, 1);
	CZetMemCallback(0xd000, 0xd1ff, 2);
	CZetMemCallback(0xd200, 0xd7ff, 0);
	CZetMemCallback(0xd200, 0xd7ff, 1);
	CZetMemCallback(0xd200, 0xd7ff, 2);
	CZetMemCallback(0xe000, 0xefff, 0);
	CZetMemCallback(0xe000, 0xefff, 1);
	CZetMemCallback(0xe000, 0xefff, 2);
	CZetMemCallback(0xf000, 0xf3ff, 0);
	CZetMemCallback(0xf000, 0xf3ff, 1);
	CZetMemCallback(0xf000, 0xf3ff, 2);
	CZetMemCallback(0xf800, 0xfbff, 0);
	CZetMemCallback(0xf800, 0xfbff, 1);
	CZetMemCallback(0xf800, 0xfbff, 2);

	CZetMapArea(0x0000, 0x7fff, 0, System1Rom1);
	CZetMapArea(0x8000, 0xbfff, 0, System1Rom1 + 0x8000);

//	CZetMapArea2(0x0000, 0x7fff, 2, System1Rom1 + 0x20000, System1Rom1);
//	CZetMapArea2(0x8000, 0xbfff, 2, System1Rom1 + 0x30000, System1Rom1 + 0x10000);  // 30 fetch et 10 pour code ?
	CZetMapMemory2(System1Rom1 + 0x20000, System1Rom1, 0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory2(System1Rom1 + 0x30000, System1Rom1 + 0x10000, 0x8000, 0xbfff, MAP_ROM);
	
	CZetMapArea(0xc000, 0xcfff, 0, System1Ram1);
	CZetMapArea(0xc000, 0xcfff, 1, System1Ram1);
	CZetMapArea(0xc000, 0xcfff, 2, System1Ram1);

	CZetMapArea(0xd000, 0xd7ff, 0, System1SpriteRam);
	CZetMapArea(0xd000, 0xd7ff, 1, System1SpriteRam);
	CZetMapArea(0xd000, 0xd7ff, 2, System1SpriteRam);

// 	CZetMapArea(0xd800, 0xddff, 0, System1PaletteRam);
//	CZetMapArea(0xd800, 0xddff, 1, System1PaletteRam);
//	CZetMapArea(0xd800, 0xddff, 2, System1PaletteRam);
//	CZetMapArea(0xd800, 0xddff, 0, System1PaletteRam);
//	CZetMemCallback(0xd800, 0xddff, 1);
//	CZetMapArea(0xd000, 0xd1ff, 1, System1SpriteRam);

	CZetMapArea(0xe000, 0xefff, 0, System1VideoRam); //read
//	CZetMapArea(0xe000, 0xefff, 1, System1VideoRam);	 //write
	CZetMapArea(0xe000, 0xefff, 2, System1VideoRam); //fetch

	CZetMapArea(0xf400, 0xf7ff, 0, System1f4Ram);
	CZetMapArea(0xf400, 0xf7ff, 1, System1f4Ram);
	CZetMapArea(0xf400, 0xf7ff, 2, System1f4Ram);
	CZetMapArea(0xf800, 0xfbff, 0, System1SprCollisionRam);
	CZetMapArea(0xf800, 0xfbff, 2, System1SprCollisionRam);
	CZetMapArea(0xfc00, 0xffff, 0, System1fcRam);
	CZetMapArea(0xfc00, 0xffff, 1, System1fcRam);
	CZetMapArea(0xfc00, 0xffff, 2, System1fcRam);

	CZetSetReadHandler(System2Z801ProgRead);
//	CZetSetWriteHandler(System2Z801ProgWrite);
//	CZetSetWriteHandler(NULL);
#ifdef USE_HANDLER_F
	CZetSetWriteHandler(System2Z801ProgWrite);
#else
	CZetSetWriteHandler2(0xd800, 0xd9ff,system1_paletteram_w);
	CZetSetWriteHandler2(0xda00, 0xdbff,system1_paletteram2_w);
	CZetSetWriteHandler2(0xdc00, 0xddff,system1_paletteram3_w);
	
	CZetSetWriteHandler2(0xe000,0xe7ff,system2_foregroundram_w2);
	CZetSetWriteHandler2(0xe800,0xefff,system2_backgroundram_w);	

	CZetSetWriteHandler2(0xf000,0xf3ff,system1_bgcollisionram_w);
	CZetSetWriteHandler2(0xf800,0xfbff,system1_sprcollisionram_w);
#endif
	CZetSetInHandler  (System2Z801PortRead);
	CZetSetOutHandler (System2Z801PortWrite);
	CZetClose();

	ppi8255_init(1);
	PPI0PortWriteA = System2PPI0WriteA;
	PPI0PortWriteB = System2PPI0WriteB;
	PPI0PortWriteC = System2PPI0WriteC;

//	System1Draw = WbmlRender;
	memset(System1VideoRam,0x00,0x4000);

	nCyclesTotal[0] = 2500000 / hz;
	nCyclesTotal[1] = 2500000 / hz;
	
}

//-------------------------------------------------------------------------------------------------------------------------------------
int WbmljbInit()
{
	int nRet;

	System1ColourProms = 1;
	System1BankedRom = 2;

	DecodeFunction = wbmljb_decode;
 	nRet = System1Init(3, 0x10000, 1, 0x8000, 3, 0x8000, 4, 0x8000, 1);
//	nRet = System2Init(3, 0x8000, 1, 0x8000, 3, 0x8000, 4, 0x8000, 1);
	CommonWbmlInit();
	System1ScrollXRam	= NULL;
 	RamStart			= NULL; //System1VideoRam-0xe000; // bg

//	nBurnFunction = wbml_draw_bg;
//	System1DoReset();
	return nRet;
}

//-------------------------------------------------------------------------------------------------------------------------------------

void wbml_draw_bg(UINT8* vram)
{
	SclProcess = 1;	
	
	ss_reg->n2_move_x = (255-((vram[0x80] >> 1) + ((vram[0x81] & 1) << 7))) &0xff;
	ss_reg->n2_move_y = vram[0x7a]; // & 0x1f;
	
	unsigned char real_page;
	const unsigned int v[] = {0, 0x40,0x1000,0x1040};

	for (unsigned int page=0; page < 4; page++)
	{
		real_page= (vram[page*2] & 0x07);
		if(!(real_page)&&!map_dirty[real_page])
			continue;

		map_dirty[real_page] = 0;

		register unsigned short *map = (unsigned short *)SS_MAP+v[page];
		register unsigned short *mapc = &map_cache[real_page*0x1000];			
		
		for (unsigned int i=0;i<32 ;i++ )
		{
			memcpyl(map,mapc,128);
//			DMA_ScuMemCopy(map, mapc, 128);
			mapc+=64;
			map+=128;

		}		
		
		
/*		
		register unsigned int *map = (unsigned int *)SS_MAP+v[page]/2;
		register unsigned int *mapc = &map_cache[real_page*0x1000];			
		
		for (unsigned int i=0;i<32 ;i++ )
		{
			memcpyl(map,mapc,128);
//			DMA_ScuMemCopy(map, mapc, 128);
			mapc+=32;
			map+=64;

		}
*/		
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void System1Render()
{
SclProcess = 1;	
	
	System1DrawSprites(System1SpriteRam);
	wbml_draw_bg(&CurrentBank[0x740]);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static inline void DrawSprite(unsigned int Num, SprSpCmd *ss_spritePtr,UINT8 *SpriteBase)
{
	UINT32 Src  = __builtin_bswap16(*((UINT16 *)SpriteBase+3));
	UINT16 Skip = __builtin_bswap16(*((UINT16 *)SpriteBase+2));
	UINT32 Bank = 0x8000 * (((SpriteBase[3] & 0x80) >> 7) + ((SpriteBase[3] & 0x40) >> 5));
	Bank &= System1SpriteRomSize;

	unsigned int Height = SpriteBase[1] - SpriteBase[0];
	unsigned int Width = (Skip + (7)) & ~(7);
	unsigned int addr = Bank + ((Src + Skip) & 0x7fff);

	if (spriteCache[addr]==0xFFFF)
	{
		int values[] ={Src,Height,Skip,Width, Bank};
		spriteCache[addr]=nextSprite;
		renderSpriteCache(values);
		nextSprite+=(Width*Height)/8;
	}

	ss_spritePtr->ax		= 11+(((SpriteBase[3] & 0x01) << 8) + SpriteBase[2] )/2;
	ss_spritePtr->ay		= SpriteBase[0] + 1;
	ss_spritePtr->charSize	= (Width<<6) + Height;
	ss_spritePtr->color		= COLADDR_SPR | ((Num)<<2);
	ss_spritePtr->charAddr	= 0x220+spriteCache[addr];
}

#if 0
void DrawSprite(unsigned int Num,unsigned int Bank, UINT16 Skip,SprSpCmd *ss_spritePtr, UINT8 *SpriteBase)
{
	unsigned int Src = (SpriteBase[7] << 8) | SpriteBase[6];
	unsigned int Height = SpriteBase[1] - SpriteBase[0];
	unsigned int Width = width_lut[Skip];

	unsigned int values[] ={Src,Height,Skip,Width, Bank,nextSprite};
//	spriteCache[addr]=nextSprite;
	renderSpriteCache(values);

	ss_spritePtr->ax		= 11 + ((((SpriteBase[3] & 0x01) << 8) + SpriteBase[2] )/2);
	ss_spritePtr->ay		= SpriteBase[0] + 1;
	ss_spritePtr->charSize	= (Width<<6) + Height;
	ss_spritePtr->color		= COLADDR_SPR | ((Num)<<2);
	ss_spritePtr->charAddr	= 0x220+nextSprite;

	nextSprite += (Height*Width)/8;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrawSpriteCache(int Num,int addr,INT16 Skip,SprSpCmd *ss_spritePtr, UINT8 *SpriteBase)
{
	unsigned int Height = SpriteBase[1] - SpriteBase[0];
	unsigned int Width  = width_lut[ABS(Skip)];

	ss_spritePtr->ax		= 11+ ((((SpriteBase[3] & 0x01) << 8) + SpriteBase[2] )/2);
	ss_spritePtr->ay		= SpriteBase[0] + 1;
	ss_spritePtr->charSize	= (Width<<6) + Height;
	ss_spritePtr->color		= COLADDR_SPR | ((Num)<<2);
	ss_spritePtr->charAddr	= 0x220+spriteCache[addr];
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
void initColors2()
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
void initLayers2()
{
    Uint16	CycleTb[]={
		0x2f64, 0xeeee, //A0
		0xeeee, 0xeeee,	//A1
		0xf6f0,0x55ee,   //B0
		0xeeee, 0xeeee  //B1
	};
 	SclConfig	scfg;

	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD;
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.plate_addr[0] = (Uint32)SS_MAP2;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.plate_addr[0] = (Uint32)SS_MAP;
	SCL_SetConfig(SCL_NBG0, &scfg);

	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.datatype 		 = SCL_BITMAP;
	scfg.mapover		 = SCL_OVER_0;
	scfg.plate_addr[0]	 = (Uint32)SS_FONT;
// 3 nbg	
	SCL_SetConfig(SCL_NBG1, &scfg);

	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
// <gamezfan> bootleg set 1 is the best outside of the virtual console version
