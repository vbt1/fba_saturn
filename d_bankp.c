// FB Alpha Bank Panic Driver Module
// Based on MAME driver by Nicola Salmoria

//#define CZ80 1
#define RAZE 1
#define CACHE 1
#include "d_bankp.h"

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvbankp	 = {
	"bankp", NULL, 
	"Bank Panic\0",  
	bankpRomInfo, bankpRomName, bankpInputInfo, bankpDIPInfo,
	DrvInit, DrvBpExit, DrvFrame, NULL//, NULL, 
	};

	struct BurnDriver nBurnDrvcombh = {
	"combh", "bankp", 
	"Combat Hawk\0",
	combhRomInfo, combhRomName, combhInputInfo, combhDIPInfo,
	DrvChInit, DrvBpExit, DrvFrame, NULL//, NULL
	};

    if (strcmp(nBurnDrvbankp.szShortName, szShortName) == 0)
	{
		flipscreen = 0;
		memcpy(shared,&nBurnDrvbankp,sizeof(struct BurnDriver));
	}
	else
	{
		flipscreen = 1;
		memcpy(shared,&nBurnDrvcombh,sizeof(struct BurnDriver));
	}

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}

static INT32 DrvChInit()
{
	DrvInit();
	ss_reg->n2_move_y =  0;//(0<<16) ;
	ss_reg->n2_move_x =  0;
}

static INT32 MemIndex()
{
//	Mem = (UINT8 *)0x00200000;
	unsigned char *Next; Next = Mem;

	Rom = Next; Next += 0x10000;
	Gfx0 = Next; Next += 0x10000;
	Gfx1 = Next; Next += 0x20000;
	Prom = Next; Next += 0x300;
	Palette = (int *)Next; Next += 0x800;
	map_offset_lut	= (UINT16*)Next; Next += 0x400 * sizeof(UINT16);
	MemEnd         = Next;

	return 0;
}


static INT32 DrvInit()
{
//	flipscreen = 0;
	DrvInitSaturn();

	int nLen;
	Mem = NULL;
	MemIndex();
	nLen = MemEnd - (unsigned char *)0;

	if ((Mem = (unsigned char *)malloc(nLen)) == NULL) return 1;
	memset(Mem, 0, nLen);
//	memset(Gfx0, 0, 0x10000);
//	memset(Gfx1, 0, 0x20000);
	MemIndex();

	make_lut();
/*
	Mem = (unsigned char*)malloc(0x10000 + 0x10000 + 0x20000 + 0x300 + 0x800);

	if (Mem == NULL) {
		return 1;
	}
//	char* pszFilename;
//	struct BurnRomInfo ri;

	Rom  = Mem + 0x00000;
	Gfx0 = Mem + 0x10000;
	Gfx1 = Mem + 0x20000;
	Prom = Mem + 0x40000;
	Palette = (int*)(Mem + 0x40200);
*/

	unsigned int i;
	for (i = 0; i < 4; i++)
	{
		if (BurnLoadRom(Rom + i * 0x4000, i +  0, 1)) return 1;
	}
	if (BurnLoadRom(Gfx0 + 0x0000, 4, 1)) return 1;
	if (BurnLoadRom(Gfx0 + 0x2000, 5, 1)) return 1;

	
	for (i = 0; i < 6; i++)
	{
		if (BurnLoadRom(Gfx1 + i * 0x2000, i +  6, 1)) return 1;
	}

	if (BurnLoadRom(Prom + 0x0000, 12, 1)) return 1;
	if (BurnLoadRom(Prom + 0x0020, 13, 1)) return 1;
	if (BurnLoadRom(Prom + 0x0120, 14, 1)) return 1;
	if (bankp_gfx_decode()) return 1;
	bankp_palette_init();
#ifdef RAZE
	z80_init_memmap();
	z80_map_fetch (0x0000, 0xdfff, Rom + 0x0000); 
	z80_map_read  (0x0000, 0xdfff, Rom + 0x0000); 
	z80_map_fetch (0xe000, 0xefff, Rom + 0xe000);
	z80_map_read  (0xe000, 0xefff, Rom + 0xe000);
	z80_map_write (0xe000, 0xefff, Rom + 0xe000);
//	z80_map_fetch (0xf000, 0xffff, Rom + 0xf000);
	z80_map_read  (0xf000, 0xffff, Rom + 0xf000);

#ifdef CACHE
//	z80_map_write (0xf800, 0xffff, Rom + 0xf800); //1 write
#else
	z80_map_write (0xf000, 0xffff, Rom + 0xf000); //1 write
#endif
	
	z80_end_memmap();

#ifdef CACHE
	z80_add_write(0xf000, 0xf3ff, 1, (void *)&bankp_write_f000);
	z80_add_write(0xf400, 0xf7ff, 1, (void *)&bankp_write_f400);
	z80_add_write(0xf800, 0xfbff, 1, (void *)&bankp_write_f800);
	z80_add_write(0xfc00, 0xffff, 1, (void *)&bankp_write_fc00);
#endif

	z80_set_in((unsigned char (*)(unsigned short))&bankp_in);
	z80_set_out((void (*)(unsigned short, unsigned char))&bankp_out);
#else
	CZetInit(1);
	CZetOpen(0);
	CZetSetInHandler(bankp_in);
	CZetSetOutHandler(bankp_out);
	CZetMapArea(0x0000, 0xdfff, 0, Rom + 0x0000);
	CZetMapArea(0x0000, 0xdfff, 2, Rom + 0x0000);
	CZetMapArea(0xe000, 0xefff, 0, Rom + 0xe000);
	CZetMapArea(0xe000, 0xefff, 1, Rom + 0xe000);
	CZetMapArea(0xe000, 0xefff, 2, Rom + 0xe000);
	CZetMapArea(0xf000, 0xffff, 0, Rom + 0xf000);
#ifndef CACHE
	CZetMapArea(0xf000, 0xffff, 1, Rom + 0xf000);
#else
	CZetSetWriteHandler(bankp_write);
#endif
	CZetMemEnd();
	CZetClose();
#endif
	SN76489Init(0, 15468000 / 6, 0);
	SN76489Init(1, 15468000 / 6, 1);
	SN76489Init(2, 15468000 / 6, 1);
//	PSG_Init(0, 15468000 / 6, 0);
//	PSG_Init(1, 15468000 / 6, 0);
//	PSG_Init(2, 15468000 / 6, 0);

	DrvDoReset();

	return 0;
}

#ifdef CACHE
#ifdef RAZE
static void __fastcall bankp_write_f000(unsigned short address, unsigned char data)
{
	if(Rom[address]!=data)
	{
		Rom[address] = data;
		fg_line(address&0x3ff, (0x3<< 14));
	}
}

static void __fastcall bankp_write_f400(unsigned short address, unsigned char data)
{
	if(Rom[address]!=data)
	{
		Rom[address] = data;
		fg_line(address&0x3ff, (0x3<< 14));
	}
}

static void __fastcall bankp_write_f800(unsigned short address, unsigned char data)
{
	if(Rom[address]!=data)
	{
		Rom[address] = data;
		bg_line(address&0x3ff, (0x3<< 14));
	}
}

static void __fastcall bankp_write_fc00(unsigned short address, unsigned char data)
{
	if(Rom[address]!=data)
	{
		Rom[address] = data;
		bg_line(address&0x3ff, (0x3<< 14));
	}
}
#else
static void __fastcall bankp_write(unsigned short address, unsigned char data)
{
	if (address >= 0xf000 && address <= 0xf7ff) 
	{
		if(Rom[address]!=data)
		{
//			fg_dirtybuffer[address&0x3ff] = 1;
			Rom[address] = data;
			fg_line(address&0x3ff, (0x3<< 14));
		}
		return;
	}

	if (address >= 0xf800 && address <= 0xffff) 
	{
		if(Rom[address]!=data)
		{
//			bg_dirtybuffer[address&0x3ff] = 1;
			Rom[address] = data;
			bg_line(address&0x3ff, (0x3<< 14));
		}
		return;
	}
}
#endif
#endif

static UINT8 __fastcall bankp_in(UINT16 address)
{
	UINT8 ret = 0;

	switch (address & 0xff)
	{
		case 0x00:
		{
			for (INT32 i = 0; i < 8; i++) ret |= DrvJoy1[i] << i;

			// limit controls to 2-way
			if ((ret & 0x05) == 0x05) ret &= 0xfa;
			if ((ret & 0x0a) == 0x0a) ret &= 0xf5;

			return ret;
		}

		case 0x01:
		{
			for (INT32 i = 0; i < 8; i++) ret |= DrvJoy2[i] << i;

			// limit controls to 2-way
			if ((ret & 0x05) == 0x05) ret &= 0xfa;
			if ((ret & 0x0a) == 0x0a) ret &= 0xf5;

			return ret;
		}

		case 0x02:
		{
			for (INT32 i = 0; i < 8; i++) ret |= DrvJoy3[i] << i;

			return ret;
		}

		case 0x04:
			return DrvDips;
	}

	return 0;
}

static void __fastcall bankp_out(UINT16 address, UINT8 data)
{
	switch (address & 0xff)
	{
		case 0x00: 
//			PSG_Write(0,data);
			SN76496Write(0, data);
		break;
		
		case 0x01:
//			PSG_Write(1,data);
			SN76496Write(1, data);
		break;
		
		case 0x02:
//			PSG_Write(2,data);
			SN76496Write(2, data);
		break;

		case 0x05:
/*
		scroll_x = data;
*/
		if (flipscreen)
			ss_reg->n2_move_y = 272-(data);//-24;
		else
			ss_reg->n2_move_x = data;//-24;
		break;

		case 0x07:
		{
			priority = data & 0x03;

			interrupt_enable = (data >> 4) & 1;

			if (priority & 0x02)
		//		SCL_SetPriority(SCL_NBG1,6);
				SCL_SET_N1PRIN(6);
			else
		//		SCL_SetPriority(SCL_NBG1,4);
				SCL_SET_N1PRIN(4);

//			flipscreen = data & 0x20;
		}
		break;
	}
}

static INT32 DrvDoReset()
{
	DrvReset = 0;

	memset (Mem + 0xe000, 0, 0x2000);
#ifdef RAZE
//	z80_reset();
#else
	CZetOpen(0);
	CZetReset();
	CZetClose();
#endif
	/*scroll_x = 0, */priority = 0;// flipscreen = 0;
	interrupt_enable = 0;

	return 0;
}

static INT32 bankp_palette_init()
{
	unsigned int t_pal[32];
	unsigned int i;

	init_32_colors(t_pal,Prom);
	
	Prom+=32;
//nbg2 mauvais

	int delta=0;
	for (i = 0;i < 32 * 4;i++) {
		colBgAddr2[delta] = colBgAddr2[delta+4] = Palette[i] = t_pal[*Prom & 0x0f];
		delta++; if ((delta & 3) == 0) delta += 12;
		Prom++;
	}
	Prom += 128;
	delta=0;
	for (i = 0;i < 16 * 8;i++) {
		colBgAddr[delta] = colBgAddr[delta+8] = Palette[i+0x80] = t_pal[*Prom & 0x0f];
		delta++; if ((delta & 7) == 0) delta += 8;
		Prom++;
	}
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*inline*/ static int readbit(const UINT8 *src, int bitnum)
{
	return src[bitnum / 8] & (0x80 >> (bitnum % 8));
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

static int bankp_gfx_decode()
{
	 int Char1PlaneOffsets[2] = { 0x00, 0x04 };
	 int Char2PlaneOffsets[3] = { 0x00, 0x20000, 0x40000 };
	 int Char1XOffsets[8]     = { 0x43, 0x42, 0x41, 0x40, 0x03, 0x02, 0x01, 0x00 };
	 int Char2XOffsets[8]     = { 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00 };
	 int CharYOffsets[8]      = { 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38 };

	GfxDecode4Bpp(0x400, 2, 8, 8, Char1PlaneOffsets, Char1XOffsets, CharYOffsets, 0x080, Gfx0, cache+0x10000);
	GfxDecode4Bpp(0x800, 3, 8, 8, Char2PlaneOffsets, Char2XOffsets, CharYOffsets, 0x040, Gfx1, cache);
	if(flipscreen)
	{
		rotate_tile(0x400,0,cache+0x10000);
		rotate_tile(0x800,0,cache);
	}
	unsigned int i;
//nbg2  
	for (i=0x10000;i<0x20000;i++ )
		cache[i+0x20000] = cache[i]+0x44;
 //nbg1
	for (i=0x00000;i<0x10000;i++ )
		cache[i+0x50000] = cache[i]+0x88;

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initLayers()
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
	scfg.plate_addr[0] = (Uint32)SS_MAP;
	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
	scfg.plate_addr[0] = (Uint32)SS_FONT;

// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);
	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initPosition()
{
	SCL_Open();
	ss_reg->n1_move_y =  16 <<16;
	ss_reg->n2_move_y =  16;//(0<<16) ;
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void initColors()
{
	/*unsigned short palette[8]=
{
RGB( 164>>3, 181>>3, 197>>3),RGB( 0,0,0 ),RGB( 164>>3, 181>>3, 197>>3),RGB( 214>>3, 230>>3, 247>>3 ),
RGB( 0, 0, 0 ),RGB( 0,0,0 ),RGB( 164>>3, 247>>3, 197>>3 ),RGB( 99>>3, 197>>3, 148>>3 ),
};*/
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);	  //ON
	(Uint16*)SCL_AllocColRam(SCL_NBG3,ON);
	(Uint16*)SCL_AllocColRam(SCL_NBG3,ON);
	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);//OFF);
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void make_lut(void)
{
	unsigned int i,delta=0;
	int sx, sy;

	for (i = 0; i < 0x400;i++) 
	{
		if(!flipscreen)
		{
			sx = i & 0x1f;
			sy = (i<<1) & (~0x3f);
		}
		else
		{
			sx = (32-((i) & 0x1f))<<6;
			sy = ((i) >> 5)+1;
		}

		map_offset_lut[i] = (sx|sy)<<1;	
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*
static void SaturnInitMem()
{
//	UINT8 *Next; Next = (UINT8 *)SaturnMem;
//	bg_dirtybuffer	= Next; Next += 0x400 * sizeof(UINT8);
//	fg_dirtybuffer		= Next; Next += 0x400 * sizeof(UINT8);
//	map_offset_lut	= (UINT16*)Next; Next += 0x400 * sizeof(UINT16);
//	MemEnd			= Next;
}	 */
//-------------------------------------------------------------------------------------------------------------------------------------
static void DrvInitSaturn()
{
//	nBurnSoundLen = 256;//192;//320; // ou 128 ?
	SS_MAP  = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B1;
	SS_MAP2 = ss_map2  =(Uint16 *)SCL_VDP2_VRAM_A1;
	SS_FONT = ss_font  =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_CACHE= cache    =(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclSpPriNumRegister *)SS_N0PRI;
	nBurnLinescrollSize = 0x300;
	nBurnSprites = 3;
	/*
	SaturnInitMem();
	int nLen = MemEnd - (UINT8 *)0;
	SaturnMem = (UINT8 *)malloc(nLen); 
	SaturnInitMem();  */
	
//	memset(bg_dirtybuffer,1,1024);
//	memset(fg_dirtybuffer,1,1024);
//3 nbg
	SS_SET_N0PRIN(7);
	SS_SET_N1PRIN(4);
	SS_SET_N2PRIN(5);
//	SCL_SET_N1PRIN(6);

	initLayers();
	initPosition();
	initColors();
	drawWindow(0,224,240,6,66);
}

//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ INT32 DrvBpExit()
{
//	InpExit();
		//PCM_Task(pcm); // bon emplacement
		nSoundBufferPos=0;
#ifdef RAZE
	z80_stop_emulating();
#else
	CZetExit();
#endif	
//	SN76496Exit();

	Rom  = 	Gfx0 = Gfx1 = Prom = Palette = NULL;
	MemEnd = map_offset_lut = /*bg_dirtybuffer = fg_dirtybuffer =*/ NULL;

	free(Mem);
	Mem = NULL;

//	layer_delta = 0;
//	free(SaturnMem);
//	SaturnMem = NULL;
	DrvDips = priority = flipscreen = interrupt_enable = 0;

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void 	 bg_line(INT32 offs,INT32 flipx)
{
	INT32 code, color, x;
	code = Rom[0xf800+offs] | ((Rom[0xfc00+offs] & 7) << 8);
	color = (Rom[0xfc00+offs] >> 4) & 0x0f;

	if (!flipscreen) flipx = (Rom[0xfc00+offs] & 0x08) << 11;
	x = map_offset_lut[offs];
//NBG1
	ss_map2[x] = ss_map2[x+0x40] = flipx | color;

	if(color==1 || color==0xd)	ss_map2[x+1] = ss_map2[x+0x41] = code;
	else									ss_map2[x+1] = ss_map2[x+0x41] = code+0x2800;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void 	 fg_line(INT32 offs,INT32 flipx)
{
	INT32 code, color, x;
	code = Rom[0xf000+offs] | ((Rom[0xf400+offs] & 3) << 8);
	color = (Rom[0xf400+offs] >> 3) & 0x1f;

	if(!flipscreen)	flipx = (Rom[0xf400+offs] & 0x04) << 12;
	x = map_offset_lut[offs];
//NBG2		   //0x40
	ss_map[x] = ss_map[x+0x40] = ss_map[x+0x1000] = ss_map[x+0x1040] = flipx | color;//color /8;

	if(Palette[color<<2]==0x8000)	ss_map[x+1] = ss_map[x+0x41] = ss_map[x+0x1001] = ss_map[x+0x1041] = code+0x800;
	else											ss_map[x+1] = ss_map[x+0x41] = ss_map[x+0x1001] = ss_map[x+0x1041] = code+0x1800;//2048  //0x1800
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 DrvFrame()
{
	if (DrvReset) {
		DrvDoReset();
	}
#ifdef RAZE
	z80_emulate(2578000 / 60);
//z80_emulate(3867120 / 60);
//3867120
	if (interrupt_enable) z80_cause_NMI();
#else
//	CZetOpen(0);
	CZetRun(2578000 / 60);
	if (interrupt_enable) CZetNmi();
//	CZetClose();
#endif	
	short *nSoundBuffer = (short *)0x25a20000;
	SN76496Update(0, &nSoundBuffer[nSoundBufferPos], SOUND_LEN);
	SN76496Update(1, &nSoundBuffer[nSoundBufferPos], SOUND_LEN);
	SN76496Update(2, &nSoundBuffer[nSoundBufferPos], SOUND_LEN);

	nSoundBufferPos+=(SOUND_LEN); // DOIT etre deux fois la taille copiee

	if(nSoundBufferPos>=0x3C00)//RING_BUF_SIZE)
//	if(nSoundBufferPos>=(2048L*10))
	{
		PCM_Task(pcm); // bon emplacement
		nSoundBufferPos=0;
	}

//	DrvDraw();
	return 0;
}
