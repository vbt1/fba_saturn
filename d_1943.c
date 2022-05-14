#include "SEGA_INT.H"
#include "SEGA_DMA.H"
#include "machine.h"
#include "d_1943.h"
// FB Alpha 1943 driver module
// Based on MAME driver by Paul Leaman

//#define nCyclesTotal 6000000
//#define nInterleave 256

//	INT32 nInterleave = 256;
//	INT32 nCyclesTotal[2] = { 6000000 / 60, 3000000 / 60 };
//	INT32 nCyclesDone[2] = { 0, 0 };
Uint16 *ss_map3;

int ovlInit(char *szShortName)
{
	cleanBSS();
	
	struct BurnDriver nBurnDrvNineteen43 = {
		"1943", NULL, 
		"1943: The Battle of Midway (Euro)",
		DrvRomInfo, DrvRomName, DrvInputInfo, DrvDIPInfo,
		DrvInit, DrvExit, DrvFrame
	};

	struct BurnDriver BurnDrvNineteen43b = {
		"1943b", "1943",
		"1943: The Battle of Midway (btlg1 JP)",
		DrvbRomInfo, DrvbRomName, DrvInputInfo, DrvDIPInfo,
		DrvbInit, DrvExit, DrvFrame
	};

	if (strcmp(nBurnDrvNineteen43.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvNineteen43,sizeof(struct BurnDriver));

	if (strcmp(BurnDrvNineteen43b.szShortName, szShortName) == 0) 
	memcpy(shared,&BurnDrvNineteen43b,sizeof(struct BurnDriver));

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
scfg.dispenbl 		 = ON; // bg1
	scfg.charsize       = SCL_CHAR_SIZE_2X2;//OK du 1*1 surtout pas toucher
	scfg.pnamesize      = SCL_PN2WORD;
	scfg.patnamecontrl =  0x00000;// VRAM B0 のオフセット 
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;
	scfg.datatype      = SCL_CELL;	
	scfg.plate_addr[0] = (Uint32)(ss_map3);//////
	scfg.plate_addr[1] = (Uint32)(ss_map3)+0x1000;
	scfg.plate_addr[2] = (Uint32)(ss_map3)+0x1000;
	scfg.plate_addr[3] = (Uint32)(ss_map3)+0x1000;
	SCL_SetConfig(SCL_NBG3, &scfg);	
	
// 3 nbg
	scfg.dispenbl      = ON; //bg2
	scfg.charsize          = SCL_CHAR_SIZE_2X2;//OK du 1*1 surtout pas toucher
	scfg.pnamesize      = SCL_PN2WORD;
//	scfg.patnamecontrl =  0x7fff;// VRAM B0 のオフセット 

	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;
	scfg.datatype      = SCL_CELL;	
	scfg.plate_addr[0] = (Uint32)(SS_MAP2);//////
	scfg.plate_addr[1] = 0;
	scfg.plate_addr[2] = 0;
	scfg.plate_addr[3] = 0;
	SCL_SetConfig(SCL_NBG1, &scfg);

	scfg.dispenbl      = ON;  // char
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN1WORD;
	scfg.flip          = SCL_PN_12BIT; 
	scfg.platesize     = SCL_PL_SIZE_1X1;
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.patnamecontrl =  0x000c;// VRAM A 0??のオフセット 
	scfg.plate_addr[0] = (Uint32)ss_map;
	scfg.plate_addr[1] = NULL; //(Uint32)(SS_MAP2+0x1000);//////
	scfg.plate_addr[2] = NULL; //(Uint32)(SS_MAP2+0x1000);	 // good	  0x400
	scfg.plate_addr[3] = NULL; //(Uint32)(SS_MAP2+0x1000);
	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.dispenbl 		 = OFF;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 		 = SCL_BITMAP;
	scfg.mapover		 = SCL_OVER_0;
	scfg.plate_addr[0]	 = (Uint32)SS_FONT;

// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr = (Uint16*)SCL_AllocColRam(SCL_NBG2,ON); // char
	SCL_AllocColRam(SCL_NBG3,OFF); // bg1
	SCL_AllocColRam(SCL_NBG1,OFF); // bg2
	SCL_AllocColRam(SCL_SPR,OFF);
	ss_regs->dispenbl &= 0xfbff; // remet la transparence
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void make_lut(void)
{
	memset(bgmap_lut1,0x00,0x20000);
	memset(bgmap_lut2,0x00,0x20000);
	UINT16 *pbgmap_lut = &bgmap_lut1[0];
	int delta=0;
	
// 8x2048x2
	for(UINT32 i = 0; i < 0x4000; i++)
	{
		if(i%8)
			delta+=2;
		
		UINT8 *pDrvBgTilemap = &DrvBgTilemap[((i&0x07)<<13)+delta];
		
		UINT32 attr		= pDrvBgTilemap[1];
		UINT32 code		= 4; //pDrvBgTilemap[0] & 0x7f;
		UINT32 color	= ((attr & 0x3c) >> 2);// + 0x18; a voir si decallage de palette utile
		UINT32 flip		= ((attr & 0xc0) >> 6);
		UINT32 flipx	= (flip >> 0) & 0x01;
		UINT32 flipy	= (flip >> 1) & 0x01;

		if(!flipx)
		{
			if(!flipy)
			{
				pbgmap_lut[0]=color;
				pbgmap_lut[1]=((code++)<<2)+0x1400;
				pbgmap_lut[2]=color;
				pbgmap_lut[3]=((code++)<<2)+0x1400;
				
				pbgmap_lut[4]=color;
				pbgmap_lut[5]=((code++)<<2)+0x1400;
				pbgmap_lut[6]=color;
				pbgmap_lut[7]=((code++)<<2)+0x1400;
			}
			else
			{
				pbgmap_lut[4]	= color | 0x8000;
				pbgmap_lut[5]	= ((code++)<<2)+0x1400;
				pbgmap_lut[6]	= color | 0x8000;
				pbgmap_lut[7]	= ((code++)<<2)+0x1400;
				
				pbgmap_lut[0]	= color | 0x8000;
				pbgmap_lut[1]	=	((code++)<<2)+0x1400;
				pbgmap_lut[2]	= color | 0x8000;
				pbgmap_lut[3]	=	((code++)<<2)+0x1400;
			}
		}
		else
		{
			if(!flipy)
			{
				pbgmap_lut[2]	= color | 0x4000;
				pbgmap_lut[3]	= ((code++)<<2)+0x1400;
				pbgmap_lut[0]	= color | 0x4000;
				pbgmap_lut[1]	= ((code++)<<2)+0x1400;
				
				pbgmap_lut[6]	= color | 0x4000;
				pbgmap_lut[7]	=	((code++)<<2)+0x1400;
				pbgmap_lut[4]	= color | 0x4000;
				pbgmap_lut[5]	=	((code++)<<2)+0x1400;
			}
			else
			{
				pbgmap_lut[6]	= color | 0xC000;
				pbgmap_lut[7]	= ((code++)<<2)+0x1400;
				pbgmap_lut[4]	= color | 0xC000;
				pbgmap_lut[5]	= ((code++)<<2)+0x1400;
				
				pbgmap_lut[2]	= color | 0xC000;
				pbgmap_lut[3]	=	((code++)<<2)+0x1400;
				pbgmap_lut[0]	= color | 0xC000;
				pbgmap_lut[1]	=	((code++)<<2)+0x1400;
			}
		}
		pbgmap_lut+=8;
	}

	pbgmap_lut = &bgmap_lut2[0];
	delta=0;
	
// 8x2048x2
	for(UINT32 i = 0; i < 0x4000; i++)
	{
		if(i%8)
			delta+=2;
		
		UINT8 *pDrvBg2Tilemap = &DrvBg2Tilemap[((i&0x07)<<13)+delta];
		
		UINT32 attr		= pDrvBg2Tilemap[1];
		UINT32 code		= pDrvBg2Tilemap[0] & 0x7f;
		UINT32 color	= ((attr & 0x3c) >> 2);// + 0x18; a voir si decallage de palette utile
		UINT32 flip		= ((attr & 0xc0) >> 6);
		UINT32 flipx	= (flip >> 0) & 0x01;
		UINT32 flipy	= (flip >> 1) & 0x01;

		if(!flipx)
		{
			if(!flipy)
			{
				pbgmap_lut[0]=color;
				pbgmap_lut[1]=((code++)<<2)+0x1000;
				pbgmap_lut[2]=color;
				pbgmap_lut[3]=((code++)<<2)+0x1000;
				
				pbgmap_lut[4]=color;
				pbgmap_lut[5]=((code++)<<2)+0x1000;
				pbgmap_lut[6]=color;
				pbgmap_lut[7]=((code++)<<2)+0x1000;
			}
			else
			{
				pbgmap_lut[4]	= color | 0x8000;
				pbgmap_lut[5]	= ((code++)<<2)+0x1000;
				pbgmap_lut[6]	= color | 0x8000;
				pbgmap_lut[7]	= ((code++)<<2)+0x1000;
				
				pbgmap_lut[0]	= color | 0x8000;
				pbgmap_lut[1]	=	((code++)<<2)+0x1000;
				pbgmap_lut[2]	= color | 0x8000;
				pbgmap_lut[3]	=	((code++)<<2)+0x1000;
			}
		}
		else
		{
			if(!flipy)
			{
				pbgmap_lut[2]	= color | 0x4000;
				pbgmap_lut[3]	= ((code++)<<2)+0x1000;
				pbgmap_lut[0]	= color | 0x4000;
				pbgmap_lut[1]	= ((code++)<<2)+0x1000;
				
				pbgmap_lut[6]	= color | 0x4000;
				pbgmap_lut[7]	=	((code++)<<2)+0x1000;
				pbgmap_lut[4]	= color | 0x4000;
				pbgmap_lut[5]	=	((code++)<<2)+0x1000;
			}
			else
			{
				pbgmap_lut[6]	= color | 0xC000;
				pbgmap_lut[7]	= ((code++)<<2)+0x1000;
				pbgmap_lut[4]	= color | 0xC000;
				pbgmap_lut[5]	= ((code++)<<2)+0x1000;
				
				pbgmap_lut[2]	= color | 0xC000;
				pbgmap_lut[3]	=	((code++)<<2)+0x1000;
				pbgmap_lut[0]	= color | 0xC000;
				pbgmap_lut[1]	=	((code++)<<2)+0x1000;
			}
		}
		pbgmap_lut+=8;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrvInitDamageXsound()
{
// init controller port
	*(UINT8 *)(0x2010007F)=0;
	*(UINT8 *)(0x2010007D)=3;
	*(UINT8 *)(0x20100079)=96;
    *(UINT8 *)(0x2010007B)=96;

// turn off the 68000
	*(UINT8 *)(0x2010001F)=7;
	for(int w=0;w<500;w++)
	{
		asm("nop\n"); // waste time
	}
	*(UINT16 *)(0x5B00400)=0x20F;

	for(int w=0;w<500;w++)
	{
		asm("nop\n"); // waste time
	}

	*(UINT16 *)(0x5B00402)=0x3F;

	for(int w=0;w<500;w++)
	{
		asm("nop\n"); // waste time
	}
extern GfsDirId *dir_name;
	signed int fid=GFS_NameToId("VGM68.BIN");
	GFS_Load(fid,0,(void *)0x5A00000,dir_name[fid].dirrec.size);

// turn on the 68000
	*(UINT8 *)(0x2010001F)=6;

	while(*(UINT16 *)(0x25A0010C)!=0)
	{
		for(int w=0;w<500;w++)
		{
			asm("nop\n"); // waste time
		}
	}

//	*(UINT16 *)(0x25A00108)=2; // stop

	UINT32 end = *(UINT32 *)(0x25A00100);

	GFS_Load(GFS_NameToId("035.VGM"),0,(void *)(0x5A00000+end),sfx_1943[35].size);

 //   memcpy((UINT16 *)(0x25A00104),(UINT16 *)(0x25A00100),sizeof(UINT16));//
 *(UINT32 *)(0x25A00104)=end;
 *(UINT16 *)(0x25A00108)=1; // start




/*
stop the current song by setting command word to 2, then wait for 68K to set it back to zero. Then write the address of the new song and set command word to 1

*/



}
//-------------------------------------------------------------------------------------------------------------------------------------
static void DrvInitSaturn()
{
//	SPR_InitSlaveSH();
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);

	nBurnSprites  = 128+4;//27;
/*	
		if( scfg->pnamesize == 1){  // SCL_PN1WORD
			if(scfg->charsize == 0) { // SCL_CHAR_SIZE_1X1
				boundary = 0x2000;
			} else {				// SCL_CHAR_SIZE_2X2
				boundary = 0x800;
			}
		} else {				// SCL_PN2WORD
			if(scfg->charsize == 0) { // SCL_CHAR_SIZE_1X1
				boundary = 0x4000;
			} else {			// SCL_CHAR_SIZE_2X2
				boundary = 0x1000;
			}
		}
*/


	SS_FONT        = NULL; //(Uint16 *)SCL_VDP2_VRAM_B1;
	SS_MAP          = ss_map    =(Uint16 *)SCL_VDP2_VRAM_B1+0x1E000;  //char boundary 0x2000
	SS_MAP2        = ss_map2  =(Uint16 *)SCL_VDP2_VRAM_B1+0x1D000; //0x01D000;   //bg1  boundary 0x1000
	SS_CACHE      = (Uint8  *)SCL_VDP2_VRAM_A0;
	ss_map3		=(Uint16 *)SCL_VDP2_VRAM_B1+0x001b000; 				 //bg2  boundary  0x1000
	
	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
//	ss_reg->n1_move_x = -8<<16;
//		ss_reg->n1_move_y =  (((-255))<<16) ;
	ss_reg->n2_move_x = 8;

//3 nbg
	SS_SET_S0PRIN(5); // sprite
	SS_SET_N0PRIN(7); // window
	SS_SET_N1PRIN(3); // bg2
	SS_SET_N2PRIN(6); // text
	SS_SET_N3PRIN(4); // bg1	
	ss_regs->tvmode = 0x8021;

	initLayers();
	initColors();
//	make_lut();
	initSprites(256-1,256-1,0,0,0,0);

    ss_sprite[nBurnSprites-1].control			= CTRL_END;
    ss_sprite[nBurnSprites-1].link				= 0;        
    ss_sprite[nBurnSprites-1].drawMode	= 0;                
    ss_sprite[nBurnSprites-1].color			= 0;                
    ss_sprite[nBurnSprites-1].charAddr		= 0;                
    ss_sprite[nBurnSprites-1].charSize		= 0;

//	nBurnFunction = CalcAll;
	DrvInitDamageXsound();  // a remettre
//	drawWindow(0,240,0,6,66); 
}
//-------------------------------------------------------------------------------------------------------------------------------------
UINT8 Drv1943ProtRead()
{
	if (bootleg) return 0;

	// This data comes from a table at $21a containing 64 entries, even is "case", odd is return value.
	switch (DrvProtValue) {
		case 0x24: return 0x1d;
		case 0x60: return 0xf7;
		case 0x01: return 0xac;
		case 0x55: return 0x50;
		case 0x56: return 0xe2;
		case 0x2a: return 0x58;
		case 0xa8: return 0x13;
		case 0x22: return 0x3e;
		case 0x3b: return 0x5a;
		case 0x1e: return 0x1b;
		case 0xe9: return 0x41;
		case 0x7d: return 0xd5;
		case 0x43: return 0x54;
		case 0x37: return 0x6f;
		case 0x4c: return 0x59;
		case 0x5f: return 0x56;
		case 0x3f: return 0x2f;
		case 0x3e: return 0x3d;
		case 0xfb: return 0x36;
		case 0x1d: return 0x3b;
		case 0x27: return 0xae;
		case 0x26: return 0x39;
		case 0x58: return 0x3c;
		case 0x32: return 0x51;
		case 0x1a: return 0xa8;
		case 0xbc: return 0x33;
		case 0x30: return 0x4a;
		case 0x64: return 0x12;
		case 0x11: return 0x40;
		case 0x33: return 0x35;
		case 0x09: return 0x17;
		case 0x25: return 0x04;
	}

	return 0;
}

UINT8 __fastcall Drv1943Read1(UINT16 a)
{
	switch (a) {
		case 0xc000: {
			return DrvInput[0];
		}
		
		case 0xc001: {
			return DrvInput[1];
		}
		
		case 0xc002: {
			return DrvInput[2];
		}
		
		case 0xc003: {
			return DrvDip[0];
		}
		
		case 0xc004: {
			return DrvDip[1];
		}
		
		case 0xc007: {
			return Drv1943ProtRead();
		}
	
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #1 Read => %04X\n"), a);
		}
	}

	return 0;
}

void __fastcall Drv1943Write1(UINT16 a, UINT8 d)
{
	switch (a) {
		case 0xc800: 
		{
			if(DrvSoundLatch != d)
			{
				DrvSoundLatch = d;
				
				switch (d) 
				{
					case 255: 
					{
						return;
					}
					case 0: 
					{
						*(UINT16 *)(0x25A00108)=2;
/*
						while(*(UINT16 *)(0x25A0010C)!=0)
						{
							for(int w=0;w<500;w++)
							{
								asm("nop\n"); // waste time
							}
						}
*/					
						Uint32 msk;

						msk = get_imask();
						set_imask(15);
						int cnt=100;
						while (--cnt > 0) {
							;
						}
						set_imask(msk);
						return;
					}
					
					default:
					{
						char vgm_file[14];
						sprintf(vgm_file, "%03d%s",d,".VGM");
						FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)vgm_file,70,130);

						if(sfx_1943[d].size!=0)
						{
						*(UINT16 *)(0x25A00108)=2;
				
						Uint32 msk;

						msk = get_imask();
						set_imask(15);
						int cnt=100;
						while (--cnt > 0) {
							;
						}
						set_imask(msk);							
							
							
							UINT32 end = *(UINT32 *)(0x25A00100);
							GFS_Load(GFS_NameToId(vgm_file),0,(void *)(0x5A00000+end),sfx_1943[d].size);
//							*(UINT32 *)(0x25A00104)=end; // vgm start address parameter
							*(UINT16 *)(0x25A00108)=1; // start
						}
						return;
					}
				}
			}
			return;
		}
		
		case 0xc804: {
			DrvRomBank = d & 0x1c;
//			CZetOpen(0);
			CZetMapMemory(DrvZ80Rom1 + 0x10000 + (DrvRomBank * 0x1000), 0x8000, 0xbfff, MAP_ROM);
//			CZetClose();
			DrvCharsOn = d & 0x80;
			return;
		}
		
		case 0xc806: {
//			BurnWatchdogWrite();
			return;
		}
		
		case 0xc807: {
			DrvProtValue = d;
			return;
		}
		
		case 0xd800: {
			DrvBgScrollX[0] = d;
			return;
		}
		
		case 0xd801: {
			DrvBgScrollX[1] = d;
			return;
		}
		
		case 0xd802: {
			DrvBgScrollY = d;
			return;
		}
		
		case 0xd803: {
			DrvBg2ScrollX[0] = d;
			return;
		}
		
		case 0xd804: {
			DrvBg2ScrollX[1] = d;
			return;
		}
		
		case 0xd806: {
			DrvBg1On = d & 0x10;
			DrvBg2On = d & 0x20;
			DrvSpritesOn = d & 0x40;
			return;
		}

		case 0xD808:
		case 0xD868:
		case 0xD888:
		case 0xD8A8: {
			return; // NOP
		}
		
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #1 Write => %04X, %02X\n"), a, d);
		}
	}
}

UINT8 __fastcall Drv1943Read2(UINT16 a)
{
	switch (a) {
		case 0xc800: {
			return DrvSoundLatch;
		}
		
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #2 Read => %04X\n"), a);
		}
	}

	return 0;
}

void __fastcall Drv1943Write2(UINT16 a, UINT8 d)
{
	switch (a) {
		case 0xe000:
		case 0xe001: {
//			BurnYM2203Write(0, a & 1, d);
			return;
		}
		
		case 0xe002:
		case 0xe003: {
//			BurnYM2203Write(1, a & 1, d);
			return;
		}
		
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #2 Write => %04X, %02X\n"), a, d);
		}
	}
}

INT32 CharPlaneOffsets[2]    = { 4, 0 };
INT32 CharXOffsets[8]        = { STEP4(0,1), STEP4(8,1) };
INT32 CharYOffsets[8]        = { STEP8(0,16) };
INT32 Bg2TilePlaneOffsets[4] = { 0x40004, 0x40000, 4, 0 };
INT32 BgTilePlaneOffsets[4]  = { 0x100004, 0x100000, 4, 0 };
INT32 TileXOffsets[32]       = { STEP4(0,1), STEP4(8,1), STEP4(512,1), STEP4(520,1), STEP4(1024,1), STEP4(1032,1), STEP4(1536,1), STEP4(1544,1) };
INT32 TileYOffsets[32]       = { STEP32(0,16) };
INT32 SpritePlaneOffsets[4]  = { 0x100004, 0x100000, 4, 0 };
INT32 SpriteXOffsets[16]     = { STEP4(0,1), STEP4(8,1), STEP4(256,1), STEP4(264,1) };
INT32 SpriteYOffsets[16]     = { STEP16(0,16) };

/*
tilemap_callback( bg2 )
{
	INT32 Attr = DrvBg2Tilemap[offs * 2 + 1];
	INT32 Code = DrvBg2Tilemap[offs * 2 + 0];

	TILE_SET_INFO(0, Code, ((Attr >> 2) & 0xf) + 0x18, TILE_FLIPYX(Attr >> 6));
}

tilemap_callback( bg1 )
{
	INT32 Attr = DrvBgTilemap[offs * 2 + 1];
	INT32 Code = DrvBgTilemap[offs * 2 + 0] + ((Attr & 0x01) << 8);

	TILE_SET_INFO(1, Code, ((Attr >> 2) & 0xf) + 0x08, TILE_FLIPYX(Attr >> 6));
}

tilemap_callback( fg )
{
	INT32 Attr = DrvPaletteRam[offs];
	INT32 Code = DrvVideoRam[offs] + ((Attr & 0xe0) << 3);

	TILE_SET_INFO(2, Code, Attr, 0);
}
*/

INT32 DrvDoReset(INT32 clear_mem)
{
	CZetOpen(0);
	CZetReset();
	CZetClose();
/*
	CZetOpen(1);
	CZetReset();
//	BurnYM2203Reset();
	CZetClose();
*/
//	BurnWatchdogReset();

	if (clear_mem) {
		DrvRomBank = 0;
		DrvSoundLatch = 0;
		DrvBg2ScrollX[0] = 0;
		DrvBg2ScrollX[1] = 0;
		DrvBgScrollX[0] = 0;
		DrvBgScrollX[1] = 0;
		DrvBgScrollY = 0;
		DrvBg2On = 0;
		DrvBg1On = 0;
		DrvSpritesOn = 0;
		DrvCharsOn = 0;
		DrvProtValue = 0;

//		HiscoreReset();
	}

	return 0;
}

INT32 MemIndex()
{
	extern unsigned int _malloc_max_ram;
	UINT8 *Next; Next = (unsigned char *)&_malloc_max_ram;
	memset(Next, 0, MALLOC_MAX);
	
	CZ80Context			   = Next; Next += 2* sizeof(cz80_struc);
	DrvZ80Rom1             = Next; Next += 0x30000;
//	DrvZ80Rom2             = Next; Next += 0x08000;
	DrvPromRed             = Next; Next += 0x00100;
	DrvPromGreen           = Next; Next += 0x00100;
	DrvPromBlue            = Next; Next += 0x00100;
	DrvPromCharLookup      = Next; Next += 0x00100;
	DrvPromBg2Lookup       = Next; Next += 0x00100;
	DrvPromBg2PalBank      = Next; Next += 0x00100;
	DrvPromBgLookup        = Next; Next += 0x00100;
	DrvPromBgPalBank       = Next; Next += 0x00100;
	DrvPromSpriteLookup    = Next; Next += 0x00100;
	DrvPromSpritePalBank   = Next; Next += 0x00100;
	DrvBgTilemap           = Next; Next += 0x08000;
	DrvBg2Tilemap          = Next; Next += 0x08000;

	RamStart               = Next;

	DrvZ80Ram1             = Next; Next += 0x01000;
	DrvZ80Ram2             = Next; Next += 0x00800;
	DrvVideoRam            = Next; Next += 0x00400;
	DrvPaletteRam          = Next; Next += 0x00400;
	DrvSpriteRam           = Next; Next += 0x01000;

	RamEnd                 = Next;

//	DrvChars               = Next; Next += 2048 * 8 * 8;
	DrvChars			   = (UINT8 *)SS_CACHE + 0x50000;
//	DrvBg2Tiles            = Next; Next += 128 * 32 * 32;
	DrvBg2Tiles            = (UINT8 *)(SS_CACHE + 0x40000);
	DrvBgTiles             = (UINT8 *)(SS_CACHE + 0x00000); //Next; Next += 512 * 32 * 32;
//	DrvSprites             = Next; Next += 2048 * 16 * 16;
	UINT8 *ss_vram			= (UINT8 *)SS_SPRAM;
	DrvSprites				= &ss_vram[0x1100];
//	DrvPalette             = (UINT32*)Next; Next += 0x00380 * sizeof(UINT32);
	DrvPalette			   = (UINT16*)colBgAddr;
//	bgmap_buf			   = Next; Next += 0x800 * sizeof (UINT32);//bgmap_lut + 0x20000;
//	map_offset_lut_fg	   = (UINT16*)Next; Next += 2048 * sizeof(UINT16);	
	bgmap_lut2	 		   = (UINT16 *)0x00200000; //Next; Next += 0x020000 * sizeof (UINT16);
	bgmap_lut1	 		   = (UINT16 *)0x00220000; //Next; Next += 0x020000 * sizeof (UINT16);
//	 Next += 0x020000;

	MemEnd                 = Next;

	return 0;
}

INT32 CommonInit(INT32 (*load)())
{
	DrvInitSaturn();
	// Allocate and Blank all required memory
	MemIndex();
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"load      ",70,130);

	if (load()) return 1;

	// Setup the Z80 emulation
	CZetInit2(2,CZ80Context);
	CZetOpen(0);
	CZetMapMemory(DrvZ80Rom1,		0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory(DrvZ80Rom1 + 0x10000,	0x8000, 0xbfff, MAP_ROM);
	CZetMapMemory(DrvVideoRam,		0xd000, 0xd3ff, MAP_RAM);
	CZetMapMemory(DrvPaletteRam,		0xd400, 0xd7ff, MAP_RAM);
	CZetMapMemory(DrvZ80Ram1,		0xe000, 0xefff, MAP_RAM);
	CZetMapMemory(DrvSpriteRam,		0xf000, 0xffff, MAP_RAM);
	CZetSetReadHandler(Drv1943Read1);
	CZetSetWriteHandler(Drv1943Write1);
	CZetClose();
	/*
//	CZetInit(1);
	CZetOpen(1);
	CZetMapMemory(DrvZ80Rom2,		0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory(DrvZ80Ram2,		0xc000, 0xc7ff, MAP_RAM);
	CZetSetReadHandler(Drv1943Read2);
	CZetSetWriteHandler(Drv1943Write2);
	CZetClose();
*/
//	BurnWatchdogInit(DrvDoReset, 180);
/*
	BurnYM2203Init(2, 1500000, NULL, 0);
	BurnTimerAttachZet(3000000);
	BurnYM2203SetRoute(0, BURN_SND_YM2203_YM2203_ROUTE, 0.10, BURN_SND_ROUTE_BOTH);
	BurnYM2203SetRoute(0, BURN_SND_YM2203_AY8910_ROUTE_1, 0.15, BURN_SND_ROUTE_BOTH);
	BurnYM2203SetRoute(0, BURN_SND_YM2203_AY8910_ROUTE_2, 0.15, BURN_SND_ROUTE_BOTH);
	BurnYM2203SetRoute(0, BURN_SND_YM2203_AY8910_ROUTE_3, 0.15, BURN_SND_ROUTE_BOTH);
	BurnYM2203SetRoute(1, BURN_SND_YM2203_YM2203_ROUTE, 0.10, BURN_SND_ROUTE_BOTH);
	BurnYM2203SetRoute(1, BURN_SND_YM2203_AY8910_ROUTE_1, 0.15, BURN_SND_ROUTE_BOTH);
	BurnYM2203SetRoute(1, BURN_SND_YM2203_AY8910_ROUTE_2, 0.15, BURN_SND_ROUTE_BOTH);
	BurnYM2203SetRoute(1, BURN_SND_YM2203_AY8910_ROUTE_3, 0.15, BURN_SND_ROUTE_BOTH);

*/
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvDoReset      ",70,130);
	make_lut();
	// Reset the driver
	DrvDoReset(1);
	DrvCalcPalette();
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"make_lut        ",70,130);

FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"end make_lut        ",70,130);
//	drawWindow(0,256,0,2,70); // good
//	drawWindow(0,256,0,0,50); 
	return 0;
}

INT32 DrvLoad()
{
	INT32 nRet = 0;

//	DrvTempRom = (UINT8 *)BurnMalloc(0x40000);
	DrvTempRom = (UINT8 *)(0x00200000);

	// Load Z80 #1 Program Roms
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x00000, 0, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x10000, 1, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x20000, 2, 1); if (nRet != 0) return 1;
	
	// Load Z80 #2 Program Roms
//	nRet = BurnLoadRom(DrvZ80Rom2 + 0x00000, 3, 1); if (nRet != 0) return 1;
	
	// Load and decode the bg2 tiles
	memset(DrvTempRom, 0, 0x40000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000, 13, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x08000, 14, 1); if (nRet != 0) return 1;
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"GfxDecode 2      ",70,130);
	GfxDecode4Bpp(128, 4, 32, 32, Bg2TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x800, DrvTempRom, DrvBg2Tiles);
	
	tile32x32toSaturn(1,128, DrvBg2Tiles);
	swapFirstLastColor(DrvBg2Tiles,0x0f,0x10000);
	wait_vblank();
	
	// Load and decode the bg tiles
	memset(DrvTempRom, 0, 0x40000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  5, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x08000,  6, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000,  7, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x18000,  8, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000,  9, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x28000, 10, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x30000, 11, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x38000, 12, 1); if (nRet != 0) return 1;
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"GfxDecode 3      ",70,130);
	GfxDecode4Bpp(512, 4, 32, 32, BgTilePlaneOffsets, TileXOffsets, TileYOffsets, 0x800, DrvTempRom, DrvBgTiles);
//	GfxDecode4Bpp(512, 4, 32, 32, BgTilePlaneOffsets, TileXOffsets, TileYOffsets, 0x800, DrvTempRom, DrvBgTiles);

	tile32x32toSaturn(1,512, DrvBgTiles);

	// Load and decode the sprites
	memset(DrvTempRom, 0, 0x40000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000, 15, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x08000, 16, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000, 17, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x18000, 18, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000, 19, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x28000, 20, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x30000, 21, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x38000, 22, 1); if (nRet != 0) return 1;
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"GfxDecode 4      ",70,130);
	GfxDecode4Bpp(2048, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x200, DrvTempRom, DrvSprites);
	rotate_tile16x16(2048,DrvSprites);
	
	// Load the Tilemaps
	nRet = BurnLoadRom(DrvBgTilemap,         23, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvBg2Tilemap,        24, 1); if (nRet != 0) return 1;
	
	// Load the PROMs
	nRet = BurnLoadRom(DrvPromRed,           25, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromGreen,         26, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBlue,          27, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromCharLookup,    28, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBgLookup,      29, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBgPalBank,     30, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBg2Lookup,     31, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBg2PalBank,    32, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromSpriteLookup,  33, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromSpritePalBank, 34, 1); if (nRet != 0) return 1;

	// Load and decode the chars
	nRet = BurnLoadRom(DrvTempRom, 4, 1); if (nRet != 0) return 1;
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"GfxDecode 1      ",70,130);

	GfxDecode4Bpp(2048, 2, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, DrvChars);
	rotate_tile(2048,1,DrvChars);
	
	DrvTempRom = NULL;	

	return nRet;
}

INT32 DrvInit()
{
	return CommonInit(DrvLoad);
}

INT32 DrvbLoad()
{
	INT32 nRet = 0;

//	DrvTempRom = (UINT8 *)BurnMalloc(0x40000);
	DrvTempRom = (UINT8 *)(0x00200000);

	// Load Z80 #1 Program Roms
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x00000, 0, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x10000, 1, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x20000, 2, 1); if (nRet != 0) return 1;
	
	// Load Z80 #2 Program Roms
//	nRet = BurnLoadRom(DrvZ80Rom2 + 0x00000, 3, 1); if (nRet != 0) return 1;
	
	// Load and decode the chars
	nRet = BurnLoadRom(DrvTempRom, 4, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(2048, 2, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, DrvChars);
	
	// Load and decode the bg2 tiles
	memset(DrvTempRom, 0, 0x40000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  9, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x08000, 10, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(128, 4, 32, 32, Bg2TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x800, DrvTempRom, DrvBg2Tiles);
	
	// Load and decode the bg tiles
	memset(DrvTempRom, 0, 0x40000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  5, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000,  6, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000,  7, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x30000,  8, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(512, 4, 32, 32, BgTilePlaneOffsets, TileXOffsets, TileYOffsets, 0x800, DrvTempRom, DrvBgTiles);
	
	// Load and decode the sprites
	memset(DrvTempRom, 0, 0x40000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000, 11, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x08000, 12, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000, 13, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x18000, 14, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000, 15, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x28000, 16, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x30000, 17, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x38000, 18, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(2048, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x200, DrvTempRom, DrvSprites);
	
	// Load the Tilemaps
	nRet = BurnLoadRom(DrvBgTilemap,         19, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvBg2Tilemap,        20, 1); if (nRet != 0) return 1;
	
	// Load the PROMs
	nRet = BurnLoadRom(DrvPromRed,           21, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromGreen,         22, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBlue,          23, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromCharLookup,    24, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBgLookup,      25, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBgPalBank,     26, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBg2Lookup,     27, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBg2PalBank,    28, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromSpriteLookup,  29, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromSpritePalBank, 30, 1); if (nRet != 0) return 1;
	
	DrvTempRom = NULL;
	
	return nRet;
}

INT32 DrvbInit()
{
	bootleg = 1;
	return CommonInit(DrvbLoad);
}

INT32 DrvbjLoad()
{
	INT32 nRet = 0;

//	DrvTempRom = (UINT8 *)BurnMalloc(0x40000);
	DrvTempRom = (UINT8 *)(0x00200000);

	// Load Z80 #1 Program Roms
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x00000, 0, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x10000, 1, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x20000, 2, 1); if (nRet != 0) return 1;
	
	// Load Z80 #2 Program Roms
//	nRet = BurnLoadRom(DrvZ80Rom2 + 0x00000, 3, 1); if (nRet != 0) return 1;
	
	// Load and decode the chars
	nRet = BurnLoadRom(DrvTempRom, 4, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(2048, 2, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, DrvChars);
	
	// Load and decode the bg2 tiles
	memset(DrvTempRom, 0, 0x40000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  9, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x08000, 10, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(128, 4, 32, 32, Bg2TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x800, DrvTempRom, DrvBg2Tiles);

	memset(DrvTempRom, 0, 0x40000);
//	UINT8 *pTemp = (UINT8*)BurnMalloc(0x40000);
	UINT8 *pTemp = (UINT8 *)(0x00240000);

	nRet = BurnLoadRom(pTemp + 0x00000,  5, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(pTemp + 0x10000,  6, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(pTemp + 0x20000,  7, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(pTemp + 0x30000,  8, 1); if (nRet != 0) return 1;
	memcpy(DrvTempRom + 0x00000, pTemp + 0x00000, 0x8000);
	memcpy(DrvTempRom + 0x10000, pTemp + 0x08000, 0x8000);
	memcpy(DrvTempRom + 0x08000, pTemp + 0x10000, 0x8000);
	memcpy(DrvTempRom + 0x18000, pTemp + 0x18000, 0x8000);
	memcpy(DrvTempRom + 0x20000, pTemp + 0x20000, 0x8000);
	memcpy(DrvTempRom + 0x30000, pTemp + 0x28000, 0x8000);
	memcpy(DrvTempRom + 0x28000, pTemp + 0x30000, 0x8000);
	memcpy(DrvTempRom + 0x38000, pTemp + 0x38000, 0x8000);
	pTemp = NULL;

	GfxDecode4Bpp(512, 4, 32, 32, BgTilePlaneOffsets, TileXOffsets, TileYOffsets, 0x800, DrvTempRom, DrvBgTiles);
	
	// Load and decode the sprites
	memset(DrvTempRom, 0, 0x40000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000, 11, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x08000, 12, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000, 13, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x18000, 14, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000, 15, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x28000, 16, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x30000, 17, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x38000, 18, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(2048, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x200, DrvTempRom, DrvSprites);
	
	// Load the Tilemaps
	nRet = BurnLoadRom(DrvBgTilemap,         19, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom,           20, 1); if (nRet != 0) return 1;
	memcpy(DrvBg2Tilemap + 0x6000, DrvTempRom + 0x0000, 0x2000);
	memcpy(DrvBg2Tilemap + 0x4000, DrvTempRom + 0x2000, 0x2000);
	memcpy(DrvBg2Tilemap + 0x2000, DrvTempRom + 0x4000, 0x2000);
	memcpy(DrvBg2Tilemap + 0x0000, DrvTempRom + 0x6000, 0x2000);
	
	// Load the PROMs
	nRet = BurnLoadRom(DrvPromRed,           21, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromGreen,         22, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBlue,          23, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromCharLookup,    24, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBgLookup,      25, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBgPalBank,     26, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBg2Lookup,     27, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBg2PalBank,    28, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromSpriteLookup,  29, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromSpritePalBank, 30, 1); if (nRet != 0) return 1;
	
	DrvTempRom = NULL;

	return nRet;
}

INT32 DrvbjInit()
{
	bootleg = 1;

	return CommonInit(DrvbjLoad);
}

INT32 Drvb2Load()
{
	INT32 nRet = 0;

//	DrvTempRom = (UINT8 *)BurnMalloc(0x40000);
	DrvTempRom = (UINT8 *)(0x00200000);

	// Load Z80 #1 Program Roms
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x00000, 0, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x10000, 1, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvZ80Rom1 + 0x20000, 2, 1); if (nRet != 0) return 1;
	
	// Load Z80 #2 Program Roms
//	nRet = BurnLoadRom(DrvZ80Rom2 + 0x00000, 3, 1); if (nRet != 0) return 1;
	
	// Load and decode the chars
	nRet = BurnLoadRom(DrvTempRom, 4, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(2048, 2, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, DrvChars);
	
	// Load and decode the bg2 tiles
	memset(DrvTempRom, 0, 0x40000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  9, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x08000, 10, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(128, 4, 32, 32, Bg2TilePlaneOffsets, TileXOffsets, TileYOffsets, 0x800, DrvTempRom, DrvBg2Tiles);
	
	memset(DrvTempRom, 0, 0x40000);
//	UINT8 *pTemp = (UINT8*)BurnMalloc(0x40000);
	UINT8 *pTemp = DrvTempRom = (UINT8 *)(0x00200000);

	nRet = BurnLoadRom(pTemp + 0x00000,  5, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(pTemp + 0x10000,  6, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(pTemp + 0x20000,  7, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(pTemp + 0x30000,  8, 1); if (nRet != 0) return 1;
	memcpy(DrvTempRom + 0x00000, pTemp + 0x00000, 0x8000);
	memcpy(DrvTempRom + 0x10000, pTemp + 0x08000, 0x8000);
	memcpy(DrvTempRom + 0x08000, pTemp + 0x10000, 0x8000);
	memcpy(DrvTempRom + 0x18000, pTemp + 0x18000, 0x8000);
	memcpy(DrvTempRom + 0x20000, pTemp + 0x20000, 0x8000);
	memcpy(DrvTempRom + 0x30000, pTemp + 0x28000, 0x8000);
	memcpy(DrvTempRom + 0x28000, pTemp + 0x30000, 0x8000);
	memcpy(DrvTempRom + 0x38000, pTemp + 0x38000, 0x8000);

	pTemp = NULL;

	GfxDecode4Bpp(512, 4, 32, 32, BgTilePlaneOffsets, TileXOffsets, TileYOffsets, 0x800, DrvTempRom, DrvBgTiles);
	
	// Load and decode the sprites
	memset(DrvTempRom, 0, 0x40000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000, 11, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x08000, 12, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x10000, 13, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000, 14, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x28000, 15, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x30000, 16, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(2048, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x200, DrvTempRom, DrvSprites);
	
	// Load the Tilemaps
	nRet = BurnLoadRom(DrvBgTilemap,         17, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom,           18, 1); if (nRet != 0) return 1;
	memcpy(DrvBg2Tilemap + 0x6000, DrvTempRom + 0x0000, 0x2000);
	memcpy(DrvBg2Tilemap + 0x4000, DrvTempRom + 0x2000, 0x2000);
	memcpy(DrvBg2Tilemap + 0x2000, DrvTempRom + 0x4000, 0x2000);
	memcpy(DrvBg2Tilemap + 0x0000, DrvTempRom + 0x6000, 0x2000);
	
	// Load the PROMs
	nRet = BurnLoadRom(DrvPromRed,           19, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromGreen,         20, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBlue,          21, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromCharLookup,    22, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBgLookup,      23, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBgPalBank,     24, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBg2Lookup,     25, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromBg2PalBank,    26, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromSpriteLookup,  27, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvPromSpritePalBank, 28, 1); if (nRet != 0) return 1;
	
	DrvTempRom = NULL;

	return nRet;
}

INT32 Drvb2Init()
{
	bootleg = 1;

	return CommonInit(Drvb2Load);
}

INT32 DrvExit()
{
	CZetExit2();
//	BurnYM2203Exit();
	
//	GenericTilesExit();
	
	DrvRomBank = 0;
	DrvSoundLatch = 0;
	DrvBg2ScrollX[0] = 0;
	DrvBg2ScrollX[1] = 0;
	DrvBgScrollX[0] = 0;
	DrvBgScrollX[1] = 0;
	DrvBgScrollY = 0;
	DrvBg2On = 0;
	DrvBg1On = 0;
	DrvSpritesOn = 0;
	DrvCharsOn = 0;
	DrvProtValue = 0;
	bootleg = 0;

	return 0;
}

void DrvCalcPalette()
{
	INT32 i, delta=0;
	UINT32 Palette[256];
	
	for (i = 0; i < 256; i++) {
		INT32 bit0, bit1, bit2, bit3, r, g, b;
		
		bit0 = (DrvPromRed[i] >> 0) & 0x01;
		bit1 = (DrvPromRed[i] >> 1) & 0x01;
		bit2 = (DrvPromRed[i] >> 2) & 0x01;
		bit3 = (DrvPromRed[i] >> 3) & 0x01;
		r = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;
		
		bit0 = (DrvPromGreen[i] >> 0) & 0x01;
		bit1 = (DrvPromGreen[i] >> 1) & 0x01;
		bit2 = (DrvPromGreen[i] >> 2) & 0x01;
		bit3 = (DrvPromGreen[i] >> 3) & 0x01;
		g = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;
		
		bit0 = (DrvPromBlue[i] >> 0) & 0x01;
		bit1 = (DrvPromBlue[i] >> 1) & 0x01;
		bit2 = (DrvPromBlue[i] >> 2) & 0x01;
		bit3 = (DrvPromBlue[i] >> 3) & 0x01;
		b = 0x0e * bit0 + 0x1f * bit1 + 0x43 * bit2 + 0x8f * bit3;
		
		Palette[i] = BurnHighCol(r, g, b, 0);
	}
// nbg 2 - 16 palettes de 4 couleurs // char	
	for (i = 0; i < 0x80; i+=4) {
		DrvPalette[i+0+delta] = Palette[(DrvPromCharLookup[i+0] & 0x0f) | 0x40];
		DrvPalette[i+1+delta] = Palette[(DrvPromCharLookup[i+1] & 0x0f) | 0x40];
		DrvPalette[i+2+delta] = Palette[(DrvPromCharLookup[i+2] & 0x0f) | 0x40];
		DrvPalette[i+3+delta] = Palette[(DrvPromCharLookup[i+3] & 0x0f) | 0x40];
		delta+=12;
	}
//0x000 a 0x0ff nbg2 char
//0x100 a 0x1ff	nbg3 bg1
//0x200 a 0x2ff nbg1 bg2
//0x300 a 0x3ff sprites
// bg 2 - 16 palettes de 16 couleurs

// on commence a 512
	for (i = 0x180; i < 0x280; i++) {
// bg 2 - 16 palettes de 16 couleurs		
		DrvPalette[i+0x80] = Palette[((DrvPromBg2PalBank[i - 0x180] & 0x03) << 4) | (DrvPromBg2Lookup[i - 0x180] & 0x0f)];
// bg 1 - 16 palettes de 16 couleurs			
		DrvPalette[i-0x80] = Palette[((DrvPromBgPalBank[i - 0x180] & 0x03) << 4) | (DrvPromBgLookup[i - 0x180] & 0x0f)];
// sprites		
		DrvPalette[i+0x180] = Palette[((DrvPromSpritePalBank[i - 0x180] & 0x07) << 4) | (DrvPromSpriteLookup[i - 0x180] & 0x0f) | 0x80];		
	}
/*
// sprites
	for (i = 0x280; i < 0x380; i++) {
		DrvPalette[i+0x80] = Palette[((DrvPromSpritePalBank[i - 0x280] & 0x07) << 4) | (DrvPromSpriteLookup[i - 0x280] & 0x0f) | 0x80];
	}

// bg 1 - 16 palettes de 16 couleurs	
	
	for (i = 0x80; i < 0x180; i++) {
		DrvPalette[i+0x80] = Palette[((DrvPromBgPalBank[i - 0x80] & 0x03) << 4) | (DrvPromBgLookup[i - 0x80] & 0x0f)];
	}
*/	
}

void DrvRenderBg2Layer()
{
//	INT32 mx, my, Offs, Attr, Code, Colour, x, y, TileIndex, xScroll, Flip, 
INT32 xFlip, yFlip;
//	xFlip = 0;
//	yFlip = 0;
/*
char tutu[50];


//xScroll = DrvBg2ScrollX[0];// + (256 * DrvBg2ScrollX[1]);
	
sprintf(tutu,"scroll2 %04d x2 %02d   ",DrvBg2ScrollX[0],DrvBg2ScrollX[1]);

FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)tutu,70,130);		

sprintf(tutu,"scroll1 %04d x2 %02d   ",DrvBgScrollX[0],DrvBgScrollX[1]);

FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)tutu,70,140);	
*/	
	ss_reg->n1_move_y = (256-DrvBg2ScrollX[0])<<16;
	ss_reg->n3_move_y = (256-DrvBgScrollX[0]);
	
	UINT32 *map1 = (UINT32 *)&ss_map3[0];//bgmap_buf;
	UINT32 *map2 = (UINT32 *)&ss_map2[0];//bgmap_buf;
// 4096 pour autre  bg sur 65536
//	INT32 offs = (DrvBg2ScrollX[1]+1)*8*8*2 ;//1024 * 2; //2 * (xScroll >> 5) ;
//	INT32 offs = 0; //DrvBg2ScrollX[1]<<8; //2 * (DrvBg2ScrollX[0] >> 5) + 0x100 * (DrvBg2ScrollX[1] >> 5);
//	ss_reg->n1_move_y =  (((-128))<<16) ;
	for (INT32 x = 0; x < 16; x++) // c'est y // 32*(8+1) = 256 + 32 for wrap
	{
		for (INT32 offset = 0; offset < 16; offset+=2)//2048 // 32*8 = 256 c'est x
		{
//			INT32 offx = (1024 * 2 +8*8*2)+offset+((16-x)*8*2); 
			INT32 offx = (((DrvBg2ScrollX[1]+1)^1)*8*2)+offset+((16-x)*8*2); 
			INT32 offx1 = (((DrvBgScrollX[1]+1)^1)*8*2)+offset+((16-x)*8*2);  //((DrvBgScrollX[1]+2)*8*2)+offset+((16-x)*8*2); 

			INT32 attr = DrvBg2Tilemap[offx + 1];
			INT32 code = (DrvBg2Tilemap[offx] + ((attr & 1) << 8))*4;
			INT32 color = ((attr & 0x3c) >> 2) ; //+ color_offset;
/*
			INT32 Flip = (attr & 0xc0) >> 6;
			xFlip = (Flip >> 0) & 0x01;
			yFlip = (Flip >> 1) & 0x01;
*/

			map2[offset+0+0x000]	= (color<<16)|0x4000+(code<<2);
			map2[offset+1+0x000]	= (color<<16)|0x4000+((code+1)<<2);
			map2[offset+32+0x000]	= (color<<16)|0x4000+((code+2)<<2);
			map2[offset+33+0x000]	= (color<<16)|0x4000+((code+3)<<2);

			INT32 attr1 = DrvBgTilemap[offx1 + 1];
			INT32 code1 = (DrvBgTilemap[offx1] + ((attr1 & 1) << 8))*4;
			INT32 color1 = ((attr1 & 0x3c) >> 2) + 16; //+ color_offset;			
			
			map1[offset+0+0x000]	= (color1<<16)| 0x0000|(code1<<2);
			map1[offset+1+0x000]	= (color1<<16)| 0x0000|((code1+1)<<2);
			map1[offset+32+0x000]	= (color1<<16)| 0x0000|((code1+2)<<2);
			map1[offset+33+0x000]	= (color1<<16)| 0x0000|((code1+3)<<2);
		}
		map1+= 64;
		map2+= 64;		
	}

//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvRenderBg2Layer end       ",70,130);	//

//		SDMA_ScuCst(DMA_SCU_CH2,ss_map2,bgmap_buf,0x800);
//		while(SDMA_ScuResult(DMA_SCU_CH2) != DMA_SCU_END);
}

void DrvRenderSprites(INT32 Priority)
{	
	SprSpCmd *ss_spritePtr = &ss_sprite[3];

	for (INT32 Offs = 0x1000 - 32; Offs >= 0; Offs -= 32) {
		UINT32 Attr = DrvSpriteRam[Offs + 1];

/*
		if (Colour != 0x0a && Colour != 0x0b)
		{
			ss_spritePtr->charSize		= 0;
		}
		else*/
	//	{
		ss_spritePtr->control	= ( JUMP_NEXT | FUNC_NORMALSP);
		ss_spritePtr->drawMode	= ( ECD_DISABLE | COMPO_REP);
		ss_spritePtr->ax		= DrvSpriteRam[Offs + 2]-16;
		ss_spritePtr->ay		= 240-(DrvSpriteRam[Offs + 3] - ((Attr & 0x10) << 4));
		ss_spritePtr->charSize	= 0x210;
		ss_spritePtr->color		= (Attr & 0x0f)<<4;//Colour<<4;
		ss_spritePtr->charAddr	= 0x220+((DrvSpriteRam[Offs] + ((Attr & 0xe0) << 3))<<4);
		ss_spritePtr++;
	//	}
	}
	
}

static void DrvRenderCharLayer()
{
	INT32 mx, my, Attr, Code, /*Colour, x, y,*/ TileIndex = 0;

	for (my = 0; my < 32; my++) {
		for (mx = 0; mx < 32; mx++) {
			Attr = DrvPaletteRam[TileIndex];
			Code = DrvVideoRam[TileIndex] + ((Attr & 0xe0) << 3);
//			Colour = Attr & 0x1f;
			unsigned short x = ((31-((mx) & 0x1f))<<6|(my)); //<<1; // map_offset_lut_fg[TileIndex]; //(mx|(my<<6));

//			ss_map[x]   = Colour;
//			ss_map[x+1] = Code;
// palette > 16 pas utilisees
// pn12bits 0fff tile et 16 palettes
			ss_map[x] = (Attr & 0x0f) <<12 | Code;
			TileIndex++;
		}
	}
}

void DrvDraw()
{
	DrvRenderCharLayer();
	DrvRenderSprites(0);
	DrvRenderBg2Layer();
//	DrvRenderSprites(1);

//afficher DrvBg1On!!!!!

/*	if (DrvRecalc) {
		DrvCalcPalette();
		DrvRecalc = 0;
	}
	GenericTilemapSetScrollX(0, DrvBg2ScrollX[0] + (256 * DrvBg2ScrollX[1]));
	GenericTilemapSetScrollX(1, DrvBgScrollX[0] + (256 * DrvBgScrollX[1]));
	GenericTilemapSetScrollY(1, DrvBgScrollY);

	if (DrvBg2On && nBurnLayer & 1) GenericTilemapDraw(0, pTransDraw, 0);
	else BurnTransferClear();

	if (DrvSpritesOn) DrvRenderSprites(0);

	if (DrvBg1On && nBurnLayer & 2) GenericTilemapDraw(1, pTransDraw, 0);

	if (DrvSpritesOn) DrvRenderSprites(1);

	if (DrvCharsOn && nBurnLayer & 4) GenericTilemapDraw(2, pTransDraw, 0);

	BurnTransferCopy(DrvPalette);
*/
//	return 0;
}

inline void DrvClearOpposites(UINT8* nJoystickInputs)
{
	if ((*nJoystickInputs & 0x03) == 0x00) {
		*nJoystickInputs |= 0x03;
	}
	if ((*nJoystickInputs & 0x0c) == 0x00) {
		*nJoystickInputs |= 0x0c;
	}
}

inline void DrvMakeInputs()
{
	// Reset Inputs
	DrvInput[0] = DrvInput[1] = DrvInput[2] = 0xff;

	// Compile Digital Inputs
	for (INT32 i = 0; i < 8; i++) {
		DrvInput[0] ^= (DrvInputPort0[i] & 1) << i;
		DrvInput[1] ^= (DrvInputPort1[i] & 1) << i;
		DrvInput[2] ^= (DrvInputPort2[i] & 1) << i;
	}

	// Clear Opposites
	DrvClearOpposites(&DrvInput[1]);
	DrvClearOpposites(&DrvInput[2]);
}

INT32 DrvFrame()
{
//	BurnWatchdogUpdate();
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvFrame      ",70,130);
/*
	if (DrvReset) {
		DrvDoReset(1);
	}
*/
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"DrvMakeInputs      ",70,130);

	DrvMakeInputs();

	INT32 nInterleave = 256;
	INT32 nCyclesTotal[2] = { 6000000 / 60, 3000000 / 60 };
	INT32 nCyclesDone[2] = { 0, 0 };
	INT32 nCyclesSegment;

//	CZetNewFrame();

	for (INT32 i = 0; i < nInterleave; i++) {
		INT32 nCurrentCPU, nNext;

		// Run Z80 #1
		nCurrentCPU = 0;
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"CZetOpen      ",70,130);

		CZetOpen(nCurrentCPU);
		nNext = (i + 1) * nCyclesTotal[nCurrentCPU] / nInterleave;
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"CZetRun      ",70,130);

		nCyclesSegment = nNext - nCyclesDone[nCurrentCPU];
		nCyclesDone[nCurrentCPU] += CZetRun(nCyclesSegment);
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"CZetSetIRQLine      ",70,130);

		if (i == 240) CZetSetIRQLine(0, CZET_IRQSTATUS_HOLD); //CZET_IRQSTATUS_HOLD);
					else {
				CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);
			}
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"CZetClose      ",70,130);

		CZetClose();
/*
		// Run Z80 #2
		nCurrentCPU = 1;
		CZetOpen(nCurrentCPU);
//		BurnTimerUpdate((i + 1) * (nCyclesTotal[1] / nInterleave));
		// execute IRQ quarterly
		if (i%(nInterleave/4) == (nInterleave/4)-1) CZetSetIRQLine(0, CZET_IRQSTATUS_HOLD);
					 else {
				CZetSetIRQLine(0, CZET_IRQSTATUS_NONE);
			}
		CZetClose();
	*/
	}

/*	
	ZetOpen(1);
	BurnTimerEndFrame(nCyclesTotal[1]);
	ZetClose();
	
	// Make sure the buffer is entirely filled.
	if (pBurnSoundOut) {
		ZetOpen(1);
		BurnYM2203Update(pBurnSoundOut, nBurnSoundLen);
		ZetClose();
	}
*/	
	DrvDraw();

	return 0;
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

		memcpy(&dpM[0*32],&tile[3*32],32);// okkkkk
		memcpy(&dpM[1*32],&tile[7*32],32);
		memcpy(&dpM[2*32],&tile[2*32],32);//okkkkk
		memcpy(&dpM[3*32],&tile[6*32],32);

		memcpy(&dpM[4*32],&tile[11*32],32);
		memcpy(&dpM[5*32],&tile[15*32],32);
		memcpy(&dpM[6*32],&tile[10*32],32);
		memcpy(&dpM[7*32],&tile[14*32],32);			

		memcpy(&dpM[8*32],&tile[1*32],32);
		memcpy(&dpM[9*32],&tile[5*32],32);
		memcpy(&dpM[10*32],&tile[0*32],32);
		memcpy(&dpM[11*32],&tile[4*32],32);

		memcpy(&dpM[12*32],&tile[9*32],32);
		memcpy(&dpM[13*32],&tile[13*32],32);
		memcpy(&dpM[14*32],&tile[8*32],32);
		memcpy(&dpM[15*32],&tile[12*32],32);

		rotate_tile(16,1,dpM);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void rotate_tile16x16(unsigned int size, unsigned char *target)
{
	unsigned int i,j,k; //,l=0;
	unsigned char temp[16][16];

	for (k=0;k<size;k++)
	{
		for(i=0;i<16;i++)
			for(j=0;j<16;j+=2)
			{
				temp[i][j]=target[(i*8)+(j/2)]>>4;
				temp[i][j+1]=target[(i*8)+(j/2)]&0x0f;
			}

		for(i=0;i<16;i++)
			for(j=0;j<16;j+=2)
				target[((15-i)*8)+(j)/2]    = (temp[j][i]<<4)|(temp[j+1][i]&0xf);
		target+=128;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
