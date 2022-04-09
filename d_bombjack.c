#include "d_bombjack.h"
#define RAZE 1

void SoundUpdate2(INT32 *length2);
inline void DecodeTiles16_4Bpp(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3);
void CalcAll();

//UINT32 bg_cache[1024];
#define PONY

#ifdef PONY
#include "saturn/pcmstm.h"

int pcm1=-1;
Sint16 *nSoundBuffer=NULL;
extern unsigned short frame_x;
extern unsigned short frame_y;
#endif

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
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initLayers()
{

    Uint16	CycleTb[]={
		0x4e12, 0x6eee, //A0
		0xeeee, 0xeeee,	//A1
		0xfe5e, 0xeeee,   //B0
		0xeeee, 0xeeee  //B1
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

//	scfg.dispenbl 		 = OFF;
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
inline void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
//	SCL_AllocColRam(SCL_NBG3,OFF);
//	colAddr = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
//	SCL_SetColRamOffset(SCL_NBG2,0,OFF);
	ss_regs->dispenbl &= 0xfbff;

	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void make_lut(void)
{
	for (UINT32 i = 0; i < 1024;i++) 
	{
		INT32 sx = ((i) % 32) <<6; //% 32;
		INT32 sy = (32 - ((i) / 32));
		map_offset_lut[i] = (sx| sy);//<<1;

		sy = (i % 16) <<5;//<<6
		sx = (15 - (i / 16));//<<1;
		map_offset_lut[0x400+i] = (sx| sy);//<<1;		
	}

	for (UINT32 i = 0; i < 4096; i++) 
	{
		UINT8 r = (i >> 0) & 0x0f;
		UINT8 g = (i >> 4) & 0x0f;
		UINT8 b = (i >> 8) & 0x0f;

		cram_lut[i] = BurnHighCol(r+r*16, g+g*16, b+b*16, 0);		
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrvInitSaturn()
{
	SPR_InitSlaveSH();
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);

	nBurnSprites  = 24+4;//27;

	SS_FONT  = (Uint16 *)SCL_VDP2_VRAM_A1;
	SS_MAP   = ss_map   =(Uint16 *)SCL_VDP2_VRAM_A1+0x08000;
	SS_MAP2  = ss_map2  =(Uint16 *)SCL_VDP2_VRAM_A1+0x0B000;
	SS_CACHE = (Uint8  *)SCL_VDP2_VRAM_A0;

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
	initSprites(256-1,240-1,0,0,7,0);

    ss_sprite[nBurnSprites-1].control		= CTRL_END;
    ss_sprite[nBurnSprites-1].link			= 0;        
    ss_sprite[nBurnSprites-1].drawMode		= 0;                
    ss_sprite[nBurnSprites-1].color			= 0;                
    ss_sprite[nBurnSprites-1].charAddr		= 0;                
    ss_sprite[nBurnSprites-1].charSize		= 0;

#ifdef PONY
	frame_x	= 0;
//	nBurnFunction = sdrv_stm_vblank_rq;
#endif	
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
void __fastcall bombjack_main_write_9000(UINT16 addr,UINT8 val)
{
	addr &=0x3ff;

	if (DrvVidRAM[addr]!=val)
	{
		DrvVidRAM[addr]=val;
		UINT32 code = val + ((DrvColRAM[addr] & 0x10) << 4);
		UINT32 color = DrvColRAM[addr] & 0x0f;

		UINT32 offs = map_offset_lut[addr];
		ss_map[offs] = color << 12 | code;
	}
}

void __fastcall bombjack_main_write_9400(UINT16 addr,UINT8 val)
{
	addr &=0x3ff;

	if (DrvColRAM[addr]!=val)
	{
		DrvColRAM[addr]=val;
		UINT32 code = DrvVidRAM[addr] + ((val & 0x10) << 4);

		UINT32 offs = map_offset_lut[addr];
		ss_map[offs] = (val & 0x0f) << 12 | code;
	}
}
/*
UINT8 __fastcall BjMemRead_9820(UINT16 addr)
{
	return DrvSprRAM[addr - 0x9820];
}

void __fastcall bombjack_main_write_9820(UINT16 addr,UINT8 val)
{
	DrvSprRAM[addr - 0x9820] = val;
}
*/
void __fastcall bombjack_main_write_b000(UINT16 addr,UINT8 data)
{
	if (addr==0xb000)
	{
		nmi_mask = data & 1;
	}
	DrvZ80RAM0[addr]=data;
}

void __fastcall bombjack_main_write_b800(UINT16 addr,UINT8 data)
{
	soundlatch=data;
}
#else
void __fastcall bombjack_main_write(UINT16 addr,UINT8 data)
{

	if (addr >= 0x9820 && addr <= 0x987f) { DrvSprRAM[addr - 0x9820] = data; return; }

	if(addr==0xb800)
	{
		soundlatch=data;
		return;
	}
	
	if (addr==0xb000)
	{
		nmi_mask = data & 1;
	}
	DrvZ80RAM0[addr]=data;
}
#endif
UINT8 __fastcall bombjack_sound_read(UINT16 address)
{
//	switch (address)
	{
//		case 0x6000:
		if(address==0x6000)
		{
			UINT8 ret = soundlatch;
			soundlatch = 0;
			return ret;
		}
	}

	return 0;
}

void __fastcall bombjack_sound_write(UINT16 port, UINT8 data)
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
//-------------------------------------------------------------------------------------------------------------------------------------
inline void DecodeTiles4Bpp(UINT8 *TilePointer, UINT32 num,INT32 off1,INT32 off2, INT32 off3)
{
	DecodeTiles(TilePointer, num,off1,off2, off3);	
	unsigned char *TilePointerP=(unsigned char *)TilePointer;

	for (UINT32 i=0;i<64*num;i+=2)
	{
		unsigned char c1 = TilePointer[i]&0x0f;
		unsigned char c2 = TilePointer[i+1]&0x0f;
		*TilePointerP++=c2| ((c1<<4)& 0xf0);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DecodeTiles(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
{
	INT32 c,y,x,dat1,dat2,dat3,col1;
	UINT8* gfx = BjGfx;
	
	for (c=0;c<num;c++)
	{
		for (y=0;y<8;y++)
		{
			dat1=gfx[off1];
			dat2=gfx[off2];
			dat3=gfx[off3];
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
			gfx++;
		}
	}
 }
//-------------------------------------------------------------------------------------------------------------------------------------
inline void DecodeTiles16_4BppTile(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
{
	DecodeTiles(TilePointer, num,off1,off2, off3);
	unsigned char tiles4[256];
	unsigned char *TilePointerP=(unsigned char *)TilePointer;
	unsigned char *TilePointerP2=(unsigned char *)TilePointer;	
	unsigned char *tiles4p;	
	
 /*2,0,3,1 */
	for (int i=0;i<num;i+=4)
	{
		memcpyl(tiles4,TilePointerP,256);

		tiles4p=(unsigned char *)tiles4;
		
		for (int j=0;j<8 ;j++ )
		{
			TilePointerP[0]=tiles4p[128];
			TilePointerP[8]=tiles4p[128+8];
			TilePointerP[16]=tiles4p[128+16];
			TilePointerP[24]=tiles4p[128+24];
			TilePointerP[32]=tiles4p[128+32];
			TilePointerP[40]=tiles4p[128+40];
			TilePointerP[48]=tiles4p[128+48];
			TilePointerP[56]=tiles4p[128+56];

			TilePointerP[64]=tiles4p[0];
			TilePointerP[64+8]=tiles4p[8];
			TilePointerP[64+16]=tiles4p[16];
			TilePointerP[64+24]=tiles4p[24];
			TilePointerP[64+32]=tiles4p[32];
			TilePointerP[64+40]=tiles4p[40];
			TilePointerP[64+48]=tiles4p[48];
			TilePointerP[64+56]=tiles4p[56];

			TilePointerP[128]=tiles4p[192];
			TilePointerP[128+8]=tiles4p[192+8];
			TilePointerP[128+16]=tiles4p[192+16];
			TilePointerP[128+24]=tiles4p[192+24];
			TilePointerP[128+32]=tiles4p[192+32];
			TilePointerP[128+40]=tiles4p[192+40];
			TilePointerP[128+48]=tiles4p[192+48];
			TilePointerP[128+56]=tiles4p[192+56];

			TilePointerP[192]=tiles4p[64];
			TilePointerP[192+8]=tiles4p[64+8];
			TilePointerP[192+16]=tiles4p[64+16];
			TilePointerP[192+24]=tiles4p[64+24];
			TilePointerP[192+32]=tiles4p[64+32];
			TilePointerP[192+40]=tiles4p[64+40];
			TilePointerP[192+48]=tiles4p[64+48];
			TilePointerP[192+56]=tiles4p[64+56];
			TilePointerP++;
			tiles4p++;
		}
		
		for (int c=0;c<256;c+=2)
		{
			unsigned char c1 = TilePointerP[c-8]&0x0f;
			unsigned char c2 = TilePointerP[c+1-8]&0x0f;
			*TilePointerP2++=c2| c1<<4;
		}		
		TilePointerP+=248;		
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void DecodeTiles32_4Bpp(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
{
	unsigned char tiles4[512];
	unsigned char *TilePointerP=(unsigned char *)TilePointer;
	unsigned char *tiles4p;
	
 /*2,0,3,1 */
	for (int i=0;i<num;i++)
	{
		tiles4p=(unsigned char *)tiles4;
		
		for (int j=0;j<512 ;j++ )
		{
			*tiles4p++=TilePointerP[j];
		}	

		tiles4p=(unsigned char *)tiles4;

		for (int j=0;j<8 ;j++ )
		{
			TilePointerP[0]=tiles4p[256];
			TilePointerP[16]=tiles4p[256+8];
			TilePointerP[32]=tiles4p[256+16];
			TilePointerP[48]=tiles4p[256+24];
			TilePointerP[64]=tiles4p[256+32];
			TilePointerP[80]=tiles4p[256+40];
			TilePointerP[96]=tiles4p[256+48];
			TilePointerP[112]=tiles4p[256+56];
			TilePointerP[128]=tiles4p[256+64];
			TilePointerP[144]=tiles4p[256+72];
			TilePointerP[160]=tiles4p[256+80];
			TilePointerP[176]=tiles4p[256+88];
			TilePointerP[192]=tiles4p[256+96];
			TilePointerP[208]=tiles4p[256+104];
			TilePointerP[224]=tiles4p[256+112];
			TilePointerP[240]=tiles4p[256+120];

			TilePointerP[8]=tiles4p[0];
			TilePointerP[24]=tiles4p[8];
			TilePointerP[40]=tiles4p[16];
			TilePointerP[56]=tiles4p[24];
			TilePointerP[72]=tiles4p[32];
			TilePointerP[88]=tiles4p[40];
			TilePointerP[104]=tiles4p[48];
			TilePointerP[120]=tiles4p[56];
			TilePointerP[136]=tiles4p[64];
			TilePointerP[152]=tiles4p[72];
			TilePointerP[168]=tiles4p[80];
			TilePointerP[184]=tiles4p[88];
			TilePointerP[200]=tiles4p[96];
			TilePointerP[216]=tiles4p[104];
			TilePointerP[232]=tiles4p[112];
			TilePointerP[248]=tiles4p[120];

 			TilePointerP[256]=tiles4p[384];
			TilePointerP[256+16]=tiles4p[384+8];
			TilePointerP[256+32]=tiles4p[384+16];
			TilePointerP[256+48]=tiles4p[384+24];
			TilePointerP[256+64]=tiles4p[384+32];
			TilePointerP[256+80]=tiles4p[384+40];
			TilePointerP[256+96]=tiles4p[384+48];
			TilePointerP[256+112]=tiles4p[384+56];
			TilePointerP[256+128]=tiles4p[384+64];
			TilePointerP[256+144]=tiles4p[384+72];
			TilePointerP[256+160]=tiles4p[384+80];
			TilePointerP[256+176]=tiles4p[384+88];
			TilePointerP[256+192]=tiles4p[384+96];
			TilePointerP[256+208]=tiles4p[384+104];
			TilePointerP[256+224]=tiles4p[384+112];
			TilePointerP[256+240]=tiles4p[384+120];

			TilePointerP[256+8]=tiles4p[128];
			TilePointerP[256+24]=tiles4p[128+8];
			TilePointerP[256+40]=tiles4p[128+16];
			TilePointerP[256+56]=tiles4p[128+24];
			TilePointerP[256+72]=tiles4p[128+32];
			TilePointerP[256+88]=tiles4p[128+40];
			TilePointerP[256+104]=tiles4p[128+48];
			TilePointerP[256+120]=tiles4p[128+56];
			TilePointerP[256+136]=tiles4p[128+64];
			TilePointerP[256+152]=tiles4p[128+72];
			TilePointerP[256+168]=tiles4p[128+80];
			TilePointerP[256+184]=tiles4p[128+88];
			TilePointerP[256+200]=tiles4p[128+96];
			TilePointerP[256+216]=tiles4p[128+104];
			TilePointerP[256+232]=tiles4p[128+112];
			TilePointerP[256+248]=tiles4p[128+120];
			TilePointerP++;
			tiles4p++;
		}
		TilePointerP+=504;		
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void DecodeTiles16_4Bpp(UINT8 *TilePointer, INT32 num,INT32 off1,INT32 off2, INT32 off3)
{
	DecodeTiles(TilePointer, num,off1,off2, off3);
	unsigned char tiles4[256];
	unsigned char *TilePointerP=(unsigned char *)TilePointer;
	unsigned char *TilePointerP2=(unsigned char *)TilePointer;		
	unsigned char *tiles4p;	

	for (int i=0;i<num;i+=4)
	{
		memcpyl(tiles4,TilePointerP,256);
		
		tiles4p=(unsigned char *)tiles4;
		
		for (int j=0;j<8 ;j++ )
		{
			TilePointerP[0]=tiles4p[128];
			TilePointerP[16]=tiles4p[128+8];
			TilePointerP[32]=tiles4p[128+16];
			TilePointerP[48]=tiles4p[128+24];
			TilePointerP[64]=tiles4p[128+32];
			TilePointerP[80]=tiles4p[128+40];
			TilePointerP[96]=tiles4p[128+48];
			TilePointerP[112]=tiles4p[128+56];

 			TilePointerP[8]=tiles4p[0];
			TilePointerP[24]=tiles4p[8];
			TilePointerP[40]=tiles4p[16];
			TilePointerP[56]=tiles4p[24];
			TilePointerP[72]=tiles4p[32];
			TilePointerP[88]=tiles4p[40];
			TilePointerP[104]=tiles4p[48];
			TilePointerP[120]=tiles4p[56];

			TilePointerP[128]=tiles4p[192];
			TilePointerP[128+16]=tiles4p[192+8];
			TilePointerP[128+32]=tiles4p[192+16];
			TilePointerP[128+48]=tiles4p[192+24];
			TilePointerP[128+64]=tiles4p[192+32];
			TilePointerP[128+80]=tiles4p[192+40];
			TilePointerP[128+96]=tiles4p[192+48];
			TilePointerP[128+112]=tiles4p[192+56];

			TilePointerP[128+8]=tiles4p[64];
			TilePointerP[128+24]=tiles4p[64+8];
			TilePointerP[128+40]=tiles4p[64+16];
			TilePointerP[128+56]=tiles4p[64+24];
			TilePointerP[128+72]=tiles4p[64+32];
			TilePointerP[128+88]=tiles4p[64+40];
			TilePointerP[128+104]=tiles4p[64+48];
			TilePointerP[128+120]=tiles4p[64+56];
			TilePointerP++;
			tiles4p++;
		}

		for (int c=0;c<256;c+=2)
		{
			unsigned char c1 = TilePointerP[c-8]&0x0f;
			unsigned char c2 = TilePointerP[c+1-8]&0x0f;
			*TilePointerP2++=c2| c1<<4;
		}
		TilePointerP+=248;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrvDoReset()
{
	memset (DrvZ80RAM0, 0, CZ80Context - DrvZ80RAM0);
	
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
	
//	__port = PER_OpenPort();
}

inline void MemIndex()
{
	extern unsigned int _malloc_max_ram;
	UINT8 *Next; Next = (unsigned char *)&_malloc_max_ram;
	memset(Next, 0, MALLOC_MAX);

	DrvZ80ROM0	= Next; Next += 0x10000;
	DrvZ80ROM1	= Next; Next += 0x02000;	
	BjGfx	  = Next; Next += 0x25000;
	BjMap	  = Next; Next += 0x02000;

//	RamStart  = Next;
	DrvZ80RAM0	= Next; Next += 0x10000;
	DrvZ80RAM1	= Next; Next += 0x01000;
	DrvPalRAM	= Next; Next += 0x00100;
	DrvVidRAM	= Next; Next += 0x00400;
	DrvColRAM	= Next; Next += 0x00400;	
	DrvSprRAM	= Next; Next += 0x00100;
//	RamEnd	  = Next;

	CZ80Context	= Next; Next += sizeof(cz80_struc)*2;
	pFMBuffer	= (INT16*)Next; Next += nBurnSoundLen * 9 * sizeof(INT16);
//	BjPalReal	= (UINT32*)Next; Next += 0x0080 * sizeof(UINT32);
	map_offset_lut = (UINT16*)Next; Next += 2048 * sizeof(UINT16);
//	mapbg_offset_lut = (UINT16*)Next; Next += 1024 * sizeof(UINT16);
	cram_lut = (UINT16*)Next; Next += 4096 * sizeof(UINT16);

//	MemEnd = Next;
}
//-------------------------------------------------------------------------------------------------------------------------------------
INT32 DrvInit()
{
	DrvInitSaturn();
	MemIndex();

	make_lut();
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"BurnMalloc                  ",10,70);
	
	INT32 RomOffset = 0;

	for (INT32 i = 0; i < 5; i++) {
		BurnLoadRom(DrvZ80ROM0 + (0x2000 * i), i, 1);
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
	BurnLoadRom(DrvZ80ROM1, RomOffset + 10, 1); // load Sound CPU
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"rom2                  ",10,70);
	// Set memory access & Init
	DrvZInit();

	UINT8 *ss_vram  = (UINT8 *)SS_SPRAM;
	UINT8 *sprites	= &ss_vram[0x1100];
	UINT8 *tiles	= (UINT8 *)SCL_VDP2_VRAM_B0;
	UINT8 *text	  	= (UINT8 *)SS_CACHE;

	DecodeTiles4Bpp(text,512,0,0x1000,0x2000);
	DecodeTiles16_4Bpp(sprites,1024,0x7000,0x5000,0x3000);
	DecodeTiles32_4Bpp(sprites+0x4000,32,0x7000,0x5000,0x3000);
	DecodeTiles16_4BppTile(tiles,1024,0x9000,0xB000,0xD000);
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"DrvDoReset                  ",10,70);
	DrvDoReset();
	nBurnFunction = CalcAll;
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"end DrvDoReset                  ",10,70);	
	return 0;
}

INT32 DrvExit()
{	 
//	nBurnFunction = NULL;
//	wait_vblank();	
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

	AY8910Exit(2);
	AY8910Exit(1);
	AY8910Exit(0);

	pFMBuffer = NULL;

#ifdef PONY
	remove_raw_pcm_buffer(pcm1);
#endif
	//cleanDATA();
	cleanBSS();

	return 0;
}

void CalcAll()
{
#ifdef PONY
	sdrv_stm_vblank_rq();
#endif	

	UINT32 delta=0;

	for (UINT32 i = 0; i < 0x100; i+=2) 
	{
		colBgAddr[delta] = cram_lut[DrvPalRAM[i] | (DrvPalRAM[i+1] << 8)];
		delta++; if ((delta & 7) == 0) delta += 8;
	}
}

 void BjRenderBgLayer(UINT32 BgSel)
 {
 //	INT32 BgSel=DrvZ80RAM0[0x9e00];

 	for (UINT32 tileCount = 0; tileCount < 256;tileCount++) 
 	{
 		UINT32 offs = (BgSel & 0x07) * 0x200 + tileCount;
		UINT32 Code = (BgSel & 0x10) ? BjMap[offs] : 0;
 
		UINT32 attr = BjMap[offs + 0x100];
 		UINT32 Colour = attr & 0x0f;
 
 		offs = map_offset_lut[0x400+tileCount];
 		ss_map2[offs] = Colour << 12 | Code;
 	}
 }


void draw_sprites()
{
	SprSpCmd *ss_spritePtr = &ss_sprite[3];
	UINT8* spr=&DrvSprRAM[0x60 - 4];
	
	for (UINT8 offs = 0; offs <24; offs++)
	{
		UINT32 flipx, flipy, code, colour;//, big;

		flipx = (spr[1] & 0x80)/8;
		flipy = (spr[1] & 0x40)>>1;

		code   = spr[0] & 0x7f;
		colour = (spr[1] & 0x0f);
		INT32 size  = spr[0] >> 7;

		ss_spritePtr->control		= ( JUMP_NEXT | FUNC_NORMALSP | flipx | flipy);
//			ss_spritePtr->drawMode	= ( COLOR_0 | COMPO_REP);
		ss_spritePtr->ax			= spr[2];
		ss_spritePtr->ay			= spr[3];
		ss_spritePtr->color			= colour<<4;

		if (size) 
		{
			code = 0x80 | ((code * 4) & 0x7c);
			ss_spritePtr->charSize= 0x420;
		}
		else
		{
			code &= 0x7f;
			ss_spritePtr->charSize= 0x210;			
		}
		ss_spritePtr->charAddr	= 0x220+((code)<<4 );
		
		ss_spritePtr++;
		spr-=4;
	}
}

#ifdef PONY
void DrvFrame_old();

void DrvFrame()
{
	pcm1 = add_raw_pcm_buffer(0,SOUNDRATE,nBurnSoundLen*20);

	nSoundBuffer = (Sint16 *)(SNDRAM+(m68k_com->pcmCtrl[pcm1].hiAddrBits<<16) | m68k_com->pcmCtrl[pcm1].loAddrBits);

//	InitCD(); // si on lance juste pour pang
//	ChangeDir("PANG");  // si on lance juste pour pang
	pcm_stream_host(DrvFrame_old);
}

void DrvFrame_old()
#else
void DrvFrame()
#endif
{
	DrvInputs[0] = 0x00;
	DrvInputs[1] = 0x00;
	DrvInputs[2] = 0x00;

	for (INT32 i = 0; i < 8; i++) {
		DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
		DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
		DrvInputs[2] ^= (DrvJoy3[i] & 1) << i;
	}

//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"BjFrame                  ",10,70);

	INT32 nInterleave = 10;
//	INT32 nInterleave = 5;
	INT32 nSoundBufferPos1 = 0;
	INT32 nCyclesSegment = 0;

	UINT32 nCyclesDone[2] = {0,0};
//	UINT32 nCyclesTotal[2] = {4000000 / 600,3000000 / 600};
	UINT32 nCyclesTotal[2] = {3000000 / 600,2250000 / 600};

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
	
#ifndef PONY
	signed short *nSoundBuffer = (signed short *)0x25a20000;
	
	INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos1;
	if (nSegmentLength) 
	{
		SoundUpdate(&nSoundBuffer[nSoundBufferPos],nSegmentLength);
	}
#else
	signed short *nSoundBuffer2 = (signed short *)nSoundBuffer+(nSoundBufferPos<<1);
	SoundUpdate(nSoundBuffer2,nBurnSoundLen);
#endif
//	SPR_RunSlaveSH((PARA_RTN*)BjDrawSprites, NULL);
//	CalcAll();

	if(BgSel!=DrvZ80RAM0[0x9e00])
	{
		BgSel=DrvZ80RAM0[0x9e00];
		BjRenderBgLayer(BgSel);
	}

	draw_sprites();

#ifdef PONY
	_spr2_transfercommand();
	SclProcess = 2;
	frame_x++;
	
	 if(frame_x>=frame_y)
		wait_vblank();	
#endif
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
	if((*(Uint8 *)0xfffffe11 & 0x80) != 0x80)	
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
#ifndef PONY
//	if(deltaSlave>=RING_BUF_SIZE/2)
	if(nSoundBufferPos>=RING_BUF_SIZE/2)
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos);
		PCM_Task(pcm); // bon emplacement
		nSoundBufferPos=0;
	}
#else
	if(nSoundBufferPos>=nBurnSoundLen*10)
	{
		pcm_play(pcm1, PCM_SEMI, 7);
		nSoundBufferPos=0;
	}	
#endif
//	deltaSlave+=nBurnSoundLen;
}

void DrvZInit()
{
	// Init the z80
	CZetInit2(2,CZ80Context);
	// Main CPU setup
#ifdef RAZE
	z80_init_memmap();
	z80_map_fetch (0x0000,0x7fff,DrvZ80ROM0+0x0000); 
	z80_map_read  (0x0000,0x7fff,DrvZ80ROM0+0x0000);  
	z80_map_fetch (0xc000,0xdfff,DrvZ80ROM0+0x8000); 
	z80_map_read  (0xc000,0xdfff,DrvZ80ROM0+0x8000);  

	z80_map_fetch (0x8000,0x8fff,DrvZ80RAM0+0x8000);
	z80_map_read  (0x8000,0x8fff,DrvZ80RAM0+0x8000);
	z80_map_write (0x8000,0x8fff,DrvZ80RAM0+0x8000);

	z80_map_fetch (0x9000,0x93ff,DrvVidRAM);
	z80_map_read  (0x9000,0x93ff,DrvVidRAM);
//	z80_map_write (0x9000,0x93ff,DrvVidRAM);

	z80_map_fetch (0x9400,0x97ff,DrvColRAM);
	z80_map_read  (0x9400,0x97ff,DrvColRAM);
//	z80_map_write (0x9400,0x97ff,DrvColRAM);

	z80_map_fetch (0x9820, 0x987f,DrvSprRAM);
	z80_map_read  (0x9820, 0x987f,DrvSprRAM);
	z80_map_write (0x9820, 0x987f,DrvSprRAM);


	z80_map_fetch (0x9c00,0x9cff,DrvPalRAM);
	z80_map_read  (0x9c00,0x9cff,DrvPalRAM);
	z80_map_write (0x9c00,0x9cff,DrvPalRAM);

	z80_map_fetch (0x9e00,0x9e00,DrvZ80RAM0+0x9e00);
	z80_map_read  (0x9e00,0x9e00,DrvZ80RAM0+0x9e00);
	z80_map_write (0x9e00,0x9e00,DrvZ80RAM0+0x9e00);
//	z80_end_memmap();

//	z80_add_read(0x9820, 0x987f, 1, (void *)&BjMemRead_9820);
	z80_add_read(0xb000, 0xb005, 1, (void *)&bombjack_main_read);

	z80_add_write(0x9000,0x93ff, 1, (void *)&bombjack_main_write_9000);
	z80_add_write(0x9400,0x97ff, 1, (void *)&bombjack_main_write_9400);

//	z80_add_write(0x9820, 0x987f, 1, (void *)&bombjack_main_write_9820);
	z80_add_write(0xb000, 0xb000, 1, (void *)&bombjack_main_write_b000);
	z80_add_write(0xb800, 0xb800, 1, (void *)&bombjack_main_write_b800);
#else
	CZetOpen(0);

	CZetMapArea    (0x0000,0x7fff,0,DrvZ80ROM0+0x0000); // Direct Read from ROM
	CZetMapArea    (0x0000,0x7fff,2,DrvZ80ROM0+0x0000); // Direct Fetch from ROM
	
	CZetMapArea    (0xc000,0xdfff,0,DrvZ80ROM0+0x8000); // Direct Read from ROM
	CZetMapArea    (0xc000,0xdfff,2,DrvZ80ROM0+0x8000); // Direct Fetch from ROM

	CZetMapArea    (0x8000,0x8fff,0,DrvZ80RAM0+0x8000);
	CZetMapArea    (0x8000,0x8fff,1,DrvZ80RAM0+0x8000);

	CZetMapArea    (0x9000,0x93ff,0,DrvVidRAM);
	CZetMapArea    (0x9000,0x93ff,1,DrvVidRAM);

	CZetMapArea    (0x9400,0x97ff,0,DrvColRAM);
	CZetMapArea    (0x9400,0x97ff,1,DrvColRAM);

//	CZetMapArea    (0x9820,0x987f,0,DrvSprRAM);
//	CZetMapArea    (0x9820,0x987f,1,DrvSprRAM);

	CZetMapArea    (0x9c00,0x9cff,0,DrvPalRAM);
	CZetMapArea    (0x9c00,0x9cff,1,DrvPalRAM);

	CZetMapArea    (0x9e00,0x9e00,0,DrvZ80RAM0+0x9e00);
	CZetMapArea    (0x9e00,0x9e00,1,DrvZ80RAM0+0x9e00);

	//	CZetMapArea    (0xb000,0xb000,0,DrvZ80RAM0+0xb000);
	//	CZetMapArea    (0xb000,0xb000,1,DrvZ80RAM0+0xb000);

	//	CZetMapArea    (0xb800,0xb800,0,DrvZ80RAM0+0xb800);
	//	CZetMapArea    (0xb800,0xb800,1,DrvZ80RAM0+0xb800);

	CZetSetReadHandler(bombjack_main_read);
	CZetSetWriteHandler(bombjack_main_write);
	CZetClose();
#endif
	CZetOpen(1);
//	CZetMapArea    (0x0000,0x1fff,0,DrvZ80ROM1); // Direct Read from ROM
//	CZetMapArea    (0x0000,0x1fff,2,DrvZ80ROM1); // Direct Fetch from ROM
	CZetMapMemory(DrvZ80ROM1, 0x0000,0x1fff, MAP_ROM);
	
//	CZetMapArea    (0x4000,0x43ff,0,DrvZ80RAM1);
//	CZetMapArea    (0x4000,0x43ff,1,DrvZ80RAM1);
//	CZetMapArea    (0x4000,0x43ff,2,DrvZ80RAM1); // fetch from ram?
	CZetMapMemory(DrvZ80RAM1, 0x4000,0x43ff, MAP_RAM);
	
//	CZetMapArea    (0xff00,0xffff,0,DrvZ80RAM1);
//	CZetMapArea    (0xff00,0xffff,1,DrvZ80RAM1);
//	CZetMapArea    (0xff00,0xffff,2,DrvZ80RAM1); // more fetch from ram? What the hell . .

	//	CZetMapArea    (0x6000,0x6000,0,DrvZ80RAM0+0xb800);
	//	CZetMapArea    (0x6000,0x6000,1,DrvZ80RAM0+0xb800);
	CZetSetReadHandler(bombjack_sound_read);
	CZetSetOutHandler(bombjack_sound_write);
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
}
