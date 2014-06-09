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

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvChplftb = {
		"chplftb", "sys2",
		"Choplifter (Alternate)\0",
		ChplftbRomInfo, ChplftbRomName, ChplftbInputInfo, ChplftbDIPInfo,
		ChplftbInit, System1Exit, System1Frame, NULL//, NULL//,
	};

//	struct BurnDriver *fba_drv = 	(struct BurnDriver *)FBA_DRV;
	memcpy(shared,&nBurnDrvChplftb,sizeof(struct BurnDriver));

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
		case 0x15: return System1BankSwitch;
		case 0x16: return System1BgBankLatch;
		case 0x19: return System1BankSwitch;
	}
	return 0;
}

/*static*/ inline void chplft_bankswitch_w (UINT8 d)
{
	System1RomBank = (((d & 0x0c)>>2) );
	System1BankRomNoDecode();
	System1BankSwitch = d;
}


/*static*/ void __fastcall ChplftZ801PortWrite(unsigned short a, UINT8 d)
{
	a &= 0xff;
	switch (a) 
	{
		case 0x14:{system1_soundport_w(d);	return;}
		case 0x15:{chplft_bankswitch_w(d);	return;}
	}
}

/*static*/ void __fastcall ChplftZ801ProgWrite(unsigned short a, UINT8 d)
{
	if (a >= 0xe000 && a <= 0xe7ff) { system1_foregroundram_w(a,d); return; }
	if (a >= 0xe800 && a <= 0xeeff) { system1_backgroundram_w(a,d); return; }
	if (a >= 0xf000 && a <= 0xf3ff) { System1BgCollisionRam[a - 0xf000] = 0x7e; return; }
	if (a >= 0xf800 && a <= 0xfbff) { System1SprCollisionRam[a - 0xf800] = 0x7e; return; }
//	if (a >= 0xd800 && a <= 0xd9ff) { system1_paletteram_w(a,d); return; }
//	if (a >= 0xda00 && a <= 0xdbff) { system1_paletteram2_w(a,d); return; }
//	if (a >= 0xdc00 && a <= 0xddff) { system1_paletteram3_w(a,d); return; }

	switch (a) {
		case 0xefbd: {
//			System1ScrollY = d;
//			if(flipscreen)d+=8;
//				ss_reg->n0_move_y = (d+16)<<16;
				ss_reg->n0_move_y = d<<16;
			break;
		}
			/*
		case 0xe7c0: {
			System1ScrollX[0] = d;
			break;
		}
		
		case 0xe7c1: {
			System1ScrollX[1] = d;
			break;
		}	 */
	}
	 /*
	if (a >= 0xef00 && a <= 0xefff) {
		System1efRam[a - 0xef00] = d;
		return;
	}		   */
	
//	bprintf(PRINT_NORMAL, _T("Prog Write %x, %x\n"), a, d);
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
	nBurnFunction = System1CalcPalette;
	ss_reg->n1_move_y =  0 <<16;
	ss_reg->n1_move_x =  0 <<16;

	SS_SET_S0PRIN(4);
	SS_SET_N1PRIN(7);
	SS_SET_N2PRIN(5);
	SS_SET_N0PRIN(3);
	initScrolling(ON);
	drawWindow(0,224,0,0,64);

	RamStart						= System1BgRam-0xe800;
	RamStart1						= System1VideoRam-0xe000;
#ifndef USE_RAZE0
	CZetOpen(0);

	CZetMapArea(0xd800, 0xddff, 1, System1PaletteRam);
	CZetMapArea(0xd800, 0xddff, 2, System1PaletteRam);

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
	CZetSetOutHandler(ChplftZ801PortWrite);
	CZetClose();
#endif
//	System1DoReset();
	return nRet;
}

//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void System1Render()
{
	System1BgScrollX = 256-(((System1ScrollX[0] >> 1) + ((System1ScrollX[1] & 1) << 7) + 6) & 0xff);
	INT32 Offs, sx, sy;

	UINT16 *vbt = ((UINT16*)ss_scl);
	for (Offs = 0; Offs < 64; Offs +=2) 
	{
		sy = (Offs <<3);
		vbt[sy]     = vbt[sy+2]   = vbt[sy+4]   = vbt[sy+6]  = 
		vbt[sy+8] = vbt[sy+10] = vbt[sy+12] = vbt[sy+14] = 
		256-((System1ScrollXRam[(Offs) & ~1] >> 1) + ((System1ScrollXRam[Offs | 1] & 1) << 7) &0xff);
	} 
	System1DrawSprites();
//	SPR_WaitEndSlaveSH();
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrawSprite(int Num,int Bank, int addr, INT16 Skip, UINT8 *SpriteBase)
{
	int Src = (SpriteBase[7] << 8) | SpriteBase[6];
	unsigned int Height = SpriteBase[1] - SpriteBase[0];
	unsigned int Width = width_lut[abs(Skip)];

	int values[] ={Src,Height,Skip,Width, Bank,nextSprite};
	renderSpriteCache(values);
	spriteCache[addr]=nextSprite;
	nextSprite = nextSprite+(Width*Height)/8;
	
	unsigned int delta	= (Num+3);

	ss_sprite[delta].ax				= (((SpriteBase[3] & 0x01) << 8) + SpriteBase[2] )/2;
	ss_sprite[delta].ay				= SpriteBase[0] + 1;
	ss_sprite[delta].charSize	= (Width<<6) + Height;
	ss_sprite[delta].color			= COLADDR_SPR | ((Num)<<2);
	ss_sprite[delta].charAddr	= 0x220+spriteCache[addr];

 	int values2[] ={ss_sprite[delta].ax,ss_sprite[delta].ay,Skip,Height,Num};
	updateCollisions(values2);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrawSpriteCache(int Num,int Bank, int addr,INT16 Skip,UINT8 *SpriteBase)
{
	unsigned int Height = SpriteBase[1] - SpriteBase[0];
	unsigned int Width = width_lut[abs(Skip)];
	unsigned int delta	= (Num+3);

	ss_sprite[delta].ax			= (((SpriteBase[3] & 0x01) << 8) + SpriteBase[2] )/2;
	ss_sprite[delta].ay			= SpriteBase[0] + 1;
	ss_sprite[delta].charSize		= (Width<<6) + Height;
	ss_sprite[delta].color			= COLADDR_SPR | ((Num)<<2);
	ss_sprite[delta].charAddr		= 0x220+spriteCache[addr];

 	int values[] ={ss_sprite[delta].ax,ss_sprite[delta].ay,Skip,Height,Num};
	updateCollisions(values);
}
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
