// FB Alpha Green Beret driver module
// Based on MAME driver by Nicola Salmoria
//#define CZ80 1
#define RAZE 1
//#define CACHE 1

#include "d_gberet.h"

#define PONY

#ifdef PONY
#include "saturn/pcmstm.h"

int pcm1=-1;
Sint16 *nSoundBuffer=NULL;
extern unsigned int frame_x;
extern unsigned int frame_y;
#endif

int ovlInit(char *szShortName)
{
	cleanBSS();

	struct BurnDriver nBurnDrvGberet = {
		"gberet", NULL,
		"Green Beret",
		gberetRomInfo, gberetRomName, DrvInputInfo, gberetDIPInfo,
		gberetInit, DrvExit, DrvFrame
	};

	struct BurnDriver nBurnDrvMrgoemon = {
		"mrgoemon", "gberet",
		"Mr. Goemon (Japan)",
		mrgoemonRomInfo, mrgoemonRomName, DrvInputInfo, mrgoemonDIPInfo,
		mrgoemonInit, DrvExit, DrvFrame
	};

    if (strcmp(nBurnDrvGberet.szShortName, szShortName) == 0)
		memcpy(shared,&nBurnDrvGberet,sizeof(struct BurnDriver));
	else
		memcpy(shared,&nBurnDrvMrgoemon,sizeof(struct BurnDriver));
	ss_reg   = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
	
	return 0;
}

inline void mrgoemon_bankswitch(int nBank)
{
#ifdef CZ80
	CZetMapArea(0xf800, 0xffff, 0, Rom + 0x10000 + nBank);
	CZetMapArea(0xf800, 0xffff, 2, Rom + 0x10000 + nBank);
#else
#ifdef RAZE
	z80_map_fetch(0xf800, 0xffff, Rom + 0x10000 + nBank); //0 read
	z80_map_read (0xf800, 0xffff, Rom + 0x10000 + nBank); //2 fetch 
	z80_map_write(0xf800, 0xffff, Rom + 0x10000 + nBank); //1 write 
//#else
//	ZetMapArea(0xf800, 0xffff, 0, Rom + 0x10000 + nBank);
//	ZetMapArea(0xf800, 0xffff, 2, Rom + 0x10000 + nBank);
#endif
#endif
}

void gberet_write(unsigned short address, unsigned char data)
{
	switch (address)
	{
		case 0xe040:
		case 0xe041:
		case 0xe042:
		return;

		case 0xe043:
			gberet_spritebank = data;
		return;

		case 0xe044:
		{
			nmi_enable = data & 0x01;
			irq_enable = data & ((game_type & 2) ? 0x02 : 0x04);

//			flipscreen = data & 0x08;
		}
		return;

		case 0xf000:
		{
			// Coin counters
		
			if (game_type == 2) {
				mrgoemon_bankswitch((data >> 5) << 11);
			}	   
		}
		return;

		case 0xf400:
			SN76496Write(0, data);
//			PSG_Write(0,data);
//			PSG_Write(0,0x00);
		return;

		case 0xf600:	// watchdog
		return;

//		case 0xf900:
//		case 0xf901:
//			gberetb_scroll = (((address & 1) << 8) | data) + 0x38;
		return;
	}

#ifdef CACHE
#ifndef RAZE
	if (address >= 0xc000 && address <= 0xcfff) 
	{
		if(Rom[address]!=data)
		{
			bg_dirtybuffer[address&0x7ff] = 1;
			Rom[address] = data;
		}
		return;
	}
#endif
#endif

	if (address >= 0xe000 && address <= 0xe03f) {
		Rom[address] = data;
		return;
	}
}

#ifdef RAZE
#ifdef CACHE
void gberet_write_cxxx(unsigned short address, unsigned char data)
{
	if(Rom[address]!=data)
	{
		bg_dirtybuffer[address&0x7ff] = 1;
		Rom[address] = data;
	}
}
#endif
#endif


unsigned char gberet_read(unsigned short address)
{

	unsigned char nRet = 0xff;
	unsigned int i;

	switch (address)
	{
		case 0xf200:
//			return DrvDips[1]; // 0x78;//7 vies+1 vie a 30k+easy mode
			return DrvDips[1];

		case 0xf400:
			return DrvDips[2];

		case 0xf600:
//			return DrvDips[0]; // 0x00;//freeplay
			return DrvDips[0];

		case 0xf601:
		{
//			if (game_type & 1) return DrvDips[3];

			for (i = 0; i < 8; i++) nRet ^= DrvJoy2[i] << i;

			return nRet;
		}


		case 0xf602:
		{
			for (i = 0; i < 8; i++) nRet ^= DrvJoy1[i] << i;

			return nRet;
		}


		case 0xf603:
		{
			for (i = 0; i < 8; i++) nRet ^= DrvJoy3[i] << i;

			return nRet;
		}
	}

	if (address >= 0xe000 && address <= 0xe03f) {
		return Rom[address];
	}
/*
	if (game_type != 1) return 0;

	if (address >= 0xe900 && address <= 0xe9ff) {
		return Rom[address];
	}
*/
	return 0;
}

void DrvDoReset()
{
//	memset (Rom+0x10000, 0, MemEnd - (Rom+0x10000));	
	
	nmi_enable = 0;
	irq_enable = 0;
	gberet_spritebank = 0;

#ifdef CZ80
	CZetOpen(0);
	CZetReset();
#else
#ifdef RAZE
	z80_reset();
//#else
//	ZetOpen(0);
//	ZetReset();
#endif
#endif

	if (game_type & 2) {
		mrgoemon_bankswitch(0);
	}

#ifdef CZ80
	CZetClose();
//#else
//#ifndef RAZE
//	ZetClose();
//#endif
#endif
}


inline void MemIndex()
{
	extern unsigned int _malloc_max_ram;
	UINT8 *Next; Next = (unsigned char *)&_malloc_max_ram;
	memset(Next, 0, MALLOC_MAX);

	Rom            = Next; Next += 0x14000;
	MemEnd         = Next;
#ifdef CACHE
	bg_dirtybuffer = Next; Next += 2048;
#endif
	load_rom	   = Next;
}

inline void DrvCreatePalette()
{ 
	unsigned int *tmp = (unsigned int *)load_rom+0x30000;
	unsigned char *Prom = (unsigned char *)load_rom+0x20000;
	unsigned short *Palette = (unsigned short*)colBgAddr;//(unsigned int*)Next; Next += 0x00200 * sizeof(unsigned int);
	UINT16  *Palette2=(UINT16  *)colBgAddr2;
	unsigned char ctabentry;
		
	init_32_colors(&tmp[0],Prom);

	Prom += 0x20;

	for (unsigned int i = 0; i < 0x100; i++)
	{
		ctabentry = (*Prom & 0x0f) | 0x10;
		*Palette2=*Palette = tmp[ctabentry];
		ctabentry = (Prom[0x100] & 0x0f);
		Palette2[0x100]=Palette[0x100] = tmp[ctabentry];
		Palette2++;
		Palette++;
		Prom++;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void Graphics_Decode(int *CharPlanes, int *CharXOffs, int *CharYOffs, int *SprPlanes, int *SprXOffs, int *SprYOffs, int SprMod)
{
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
//	memset(cache,0x00,0x40000);
	UINT8 *Gfx0 = (unsigned char *)load_rom;
	UINT8 *Gfx1 = (unsigned char *)load_rom+0x10000;
	
	GfxDecode4Bpp(0x200, 4,  8,  8, CharPlanes, CharXOffs, CharYOffs, 0x100, Gfx0, SS_CACHE);
	GfxDecode4Bpp(0x200, 4, 16, 16, SprPlanes, SprXOffs, SprYOffs, SprMod, Gfx1, &ss_vram[0x1100]);
}

inline void DrvGfxDecode()
{
	int Planes[4] = { 0, 1, 2, 3 };
	int XOffs[16] = { 0, 4, 8, 12, 16, 20, 24, 28, 256, 260, 264, 268, 272, 276, 280, 284 };
	int YOffs[16] = { 0, 32, 64, 96, 128, 160, 192, 224, 512, 544, 576, 608, 640, 672, 704, 736 };

	Graphics_Decode(Planes, XOffs, YOffs, Planes, XOffs, YOffs, 0x400);
}

void GetRoms()
{
	char* pRomName;
	struct BurnRomInfo ri;
	unsigned char *Load0 = Rom;
	unsigned char *Load1 = (unsigned char *)load_rom;
	unsigned char *Load2 = (unsigned char *)load_rom+0x10000;
	unsigned char *Load3 = (unsigned char *)load_rom+0x20000;
	memset(Load1,0x00,0x30000);
	
	for (unsigned int i = 0; !BurnDrvGetRomName(&pRomName, i, 0); i++) {

		BurnDrvGetRomInfo(&ri, i);

		if ((ri.nType & 7) == 1) {
			if (BurnLoadRom(Load0, i, 1)) return ;
			Load0 += ri.nLen;

			continue;
		}

		if ((ri.nType & 7) == 2) {
			if (BurnLoadRom(Load1, i, 1)) return ;
			Load1 += ri.nLen;

			continue;
		}

		if ((ri.nType & 7) == 3) {
			if (BurnLoadRom(Load2, i, 1)) return ;
			Load2 += ri.nLen;

			continue;
		}

		if ((ri.nType & 7) == 4) {
			if (BurnLoadRom(Load3, i, 1)) return ;
			Load3 += ri.nLen;

			continue;
		}
	}

	if (game_type == 2) {
		memcpy (Rom + 0x10000, Rom + 0x0c000, 0x04000);
		memset (Rom + 0x0c000, 0, 0x04000);
	} 		  
}

int DrvInit()
{
	DrvInitSaturn();
	MemIndex();

#ifdef CACHE
	memset(bg_dirtybuffer,1,2048);
#endif	
	
	GetRoms();
	DrvGfxDecode();
	DrvCreatePalette();

#ifdef CZ80
//	if(game_type&2)
//	{
	CZetInit2(1,CZ80Context);
	CZetOpen(0);
	CZetSetReadHandler(gberet_read);
	CZetSetWriteHandler(gberet_write);

	CZetMapArea(0x0000, 0xbfff, 0, Rom + 0x0000);
	CZetMapArea(0x0000, 0xbfff, 2, Rom + 0x0000);
	CZetMapArea(0xc000, 0xcfff, 0, Rom + 0xc000);
	CZetMapArea(0xc000, 0xcfff, 1, Rom + 0xc000);
	CZetMapArea(0xd000, 0xd0ff, 0, Rom + 0xd000);
	CZetMapArea(0xd000, 0xd0ff, 1, Rom + 0xd000);
	CZetMapArea(0xd100, 0xd1ff, 0, Rom + 0xd100);
	CZetMapArea(0xd100, 0xd1ff, 1, Rom + 0xd100);
	CZetMapArea(0xd200, 0xdfff, 0, Rom + 0xd200);
	CZetMapArea(0xd200, 0xdfff, 1, Rom + 0xd200);
	CZetMapArea(0xd200, 0xdfff, 2, Rom + 0xd200);
	if (game_type == 2) {
		CZetMapArea(0xe800, 0xe8ff, 0, Rom + 0xe800);
		CZetMapArea(0xe800, 0xe8ff, 1, Rom + 0xe800);
		CZetMapArea(0xe800, 0xe8ff, 2, Rom + 0xe800);
	}
	CZetMemEnd();
	CZetClose();
//	}
#else
//	else
//	{
#ifdef RAZE
	z80_init_memmap();
/*
0000-bfff ROM
c000-c7ff Color RAM
c800-cfff Video RAM
d000-d0c0 Sprites (bank 0)
d100-d1c0 Sprites (bank 1)
d200-dfff RAM
e000-e01f ZRAM1 line scroll registers
e020-e03f ZRAM2 bit 8 of line scroll registers
*/
	z80_map_fetch (0x0000, 0xbfff, Rom + 0x0000); 
	z80_map_read  (0x0000, 0xbfff, Rom + 0x0000); //0 read 
//16/06/2012/		z80_map_fetch (0xc000, 0xcfff, Rom + 0xc000); 		  //

	z80_map_read  (0xc000, 0xcfff, Rom + 0xc000); //0 read

#ifdef CACHE
//	z80_map_write (0xc000, 0xc7ff, Rom + 0xc000); //1 write
#else
	z80_map_write (0xc000, 0xcfff, Rom + 0xc000); //1 write
#endif
//		z80_map_fetch (0xd000, 0xd0ff, Rom + 0xd000); 	  //

	z80_map_fetch (0xd000, 0xdfff, Rom + 0xd000); //2 fetch
	z80_map_read  (0xd000, 0xdfff, Rom + 0xd000); //1 write 
	z80_map_write (0xd000, 0xdfff, Rom + 0xd000); //0 read 
	
	if (game_type == 2) {
		z80_map_fetch (0xe800, 0xe8ff, Rom + 0xe800);
		z80_map_read  (0xe800, 0xe8ff, Rom + 0xe800); //1 read ?
		z80_map_write (0xe800, 0xe8ff, Rom + 0xe800); //2 write ?
	}

#ifdef CACHE
	z80_add_write(0xc000, 0xcfff, 1, (void *)&gberet_write_cxxx);
#endif

	z80_add_write(0xe000, 0xe03f, 1, (void *)&gberet_write);
	z80_add_write(0xe040, 0xe044, 1, (void *)&gberet_write);
//	if (game_type == 1)
//		z80_add_write(0xe900, 0xe9ff, 1, (void *)&gberet_write);
	z80_add_write(0xf000, 0xf000, 1, (void *)&gberet_write);
	z80_add_write(0xf400, 0xf400, 1, (void *)&gberet_write);
	z80_add_write(0xf600, 0xf600, 1, (void *)&gberet_write);
//	z80_add_write(0xf900, 0xf901, 1, (void *)&gberet_write);

	z80_add_read (0xe000, 0xe03f, 1, (void *)&gberet_read);
//	if (game_type == 1)
//		z80_add_read (0xe900, 0xe9ff, 1, (void *)&gberet_read);
	z80_add_read (0xf200, 0xf200, 1, (void *)&gberet_read);
	z80_add_read (0xf400, 0xf400, 1, (void *)&gberet_read);
	z80_add_read (0xf600, 0xf603, 1, (void *)&gberet_read);

//		z80_add_read (0xe040, 0xe04f, 1, (void *)&gberet_read);
//	if (game_type & 1) {
//		z80_add_write(0xe900, 0xe9ff, 1, (void *)&gberet_write);
//		z80_add_read (0xe900, 0xe9ff, 1, (void *)&gberet_read);
//	}	
//	z80_add_read (0xf000, 0xf901, 1, (void *)&gberet_read);

	z80_end_memmap();     
#endif
#endif
	SN76489AInit(0, 18432000 / 12, 0);
	DrvDoReset();
	return 0;
}

INT32 DrvExit()
{
	DrvDoReset();
#ifdef RAZE
	z80_stop_emulating();

#ifdef CACHE
	z80_add_write(0xc000, 0xcfff, 1, (void *)NULL);
#endif

	z80_add_write(0xe000, 0xe03f, 1, (void *)NULL);
	z80_add_write(0xe040, 0xe044, 1, (void *)NULL);
	z80_add_write(0xf000, 0xf000, 1, (void *)NULL);
	z80_add_write(0xf400, 0xf400, 1, (void *)NULL);
	z80_add_write(0xf600, 0xf600, 1, (void *)NULL);
	z80_add_read (0xe000, 0xe03f, 1, (void *)NULL);
	z80_add_read (0xf200, 0xf200, 1, (void *)NULL);
	z80_add_read (0xf400, 0xf400, 1, (void *)NULL);
	z80_add_read (0xf600, 0xf603, 1, (void *)NULL);
#else
	CZetExit2();
#endif
	SN76496Exit();
	
	nBurnSprites=128;
	cleanSprites();
	memset(ss_scl,0x00,nBurnLinescrollSize);
	
#ifdef PONY
	remove_raw_pcm_buffer(pcm1);
#endif	
/*
	bg_dirtybuffer=MemEnd=Rom=NULL;
	game_type=0;
	_30_HZ=0;
	

nmi_enable = irq_enable = 0;
gberet_spritebank = 0;

	
	memset(DrvJoy1,0x00,8);
	memset(DrvJoy2,0x00,8);
	memset(DrvJoy3,0x00,8);
	DrvDips[0] = DrvDips[1] = 0;
*/	
	//cleanDATA();
	cleanBSS();

	nSoundBufferPos=0;

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void initLayers()
{

    Uint16	CycleTb[]={
		0xfe5e, 0xeeee, //A1
		0xeeee, 0xeeee,	//A0
		0x04fe, 0xeeee,   //B1
		0xeeee, 0xeeee  //B0
	};

 	SclConfig	scfg;
	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN1WORD;
	scfg.flip          = SCL_PN_10BIT; 
	scfg.platesize     = SCL_PL_SIZE_2X2;
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.patnamecontrl =  0x000c;// VRAM B1 ??I?t?Z?b?g 
	scfg.plate_addr[0] = (Uint32)ss_map;
	SCL_SetConfig(SCL_NBG0, &scfg);
	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initPosition()
{
	SCL_Open();
	ss_reg->n0_move_y =  (8<<16) ;
//	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void initColors()
{
	colBgAddr = (Uint16*)SCL_AllocColRam(SCL_NBG0,ON);
	SCL_AllocColRam(SCL_SPR,OFF);
	SCL_AllocColRam(SCL_NBG1,OFF);
	SCL_SetColRam(SCL_NBG1,8,8,palette);
}

#ifdef PONY
extern SclLineparam lp;

void SCL_SetLineParamNBG0(SclLineparam *lp)
{
	Uint32	*addr;
	addr = &Scl_n_reg.lineaddr[0];
	*addr = (lp->line_addr >>1) & 0x0007ffff;
	SclProcess = 2; //obligatoire
}
#endif	
//-------------------------------------------------------------------------------------------------------------------------------------
void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
	nBurnSprites = 51;
	nSoundBufferPos = 0;

	SS_CACHE = (Uint8 *)SCL_VDP2_VRAM_B1;
	SS_MAP   = ss_map  =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_MAP2  = ss_map2 =(Uint16 *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
	ss_scl			= (Fixed32 *)SS_SCL;

	SS_SET_S0PRIN(6);
	SS_SET_N1PRIN(7);
	SS_SET_N2PRIN(4);
	SS_SET_N0PRIN(5);

	initLayers();
	initPosition();
	initColors();
	initSprites(256-1,224-1,8,0,0,-16);
	//play=1;
//	drawWindow(0,240,0,2,66);
	initScrolling(ON,(void *)SCL_VDP2_VRAM_B0+0x4000);
//	memset(&ss_scl[0],16<<16,64);
	memset(&ss_scl[0],16<<16,128);
	memset(&ss_map[0],0,0x4000);
	memset(&ss_map2[0],0,0x4000);
	drawWindow(0,240,0,2,66);
//	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
#ifdef PONY
	frame_x	= 0;
	nBurnFunction = sdrv_stm_vblank_rq;	
#endif	
}
//-------------------------------------------------------------------------------------------------------------------------------------
void gberet_draw_sprites()
{
	unsigned char *sr = Rom + 0xd000 + ((~gberet_spritebank & 8) << 5);
	SprSpCmd *ss_spritePtr;
	
	ss_spritePtr = &ss_sprite[3];
	
	for (unsigned int offs = 0; offs < 48; offs++)
	{
		if (sr[3])
		{
			unsigned int  attr = sr[1];
			unsigned int  code = sr[0] + ((attr & 0x40) << 2);
			unsigned int flip = attr & 0x30;

			ss_spritePtr->ax		 = sr[2] - 2 * (attr & 0x80);
			ss_spritePtr->ay		 = sr[3];
			ss_spritePtr->color      = (attr & 0x0f) << 4;
			ss_spritePtr->control    = ( JUMP_NEXT | FUNC_NORMALSP | flip);
			ss_spritePtr->drawMode   = ( ECD_DISABLE | COMPO_REP);		
			ss_spritePtr->charSize   = 0x210;  //0x100 16*16
			ss_spritePtr->charAddr   = 0x220+(code<<4);
			ss_spritePtr++;
		}
		sr+=4;
	}
}

inline void DrvDraw()
{
/*	
	unsigned int offs;
	unsigned char *ColorRom = &Rom[0xc040];
	unsigned char *VideoRom = &Rom[0xc840];
	int scroll = Rom[0xe006] | (Rom[0xe026] << 8);
	UINT8 	*bg_dirty=&bg_dirtybuffer[0x40];
	UINT16*map  =(Uint16 *)SCL_VDP2_VRAM_B0;
	UINT16*map2 =(Uint16 *)SCL_VDP2_VRAM_A0;
	memset(&ss_scl[16],(scroll<<16),0x340);

	for (offs = 0; offs < 0x780; offs++)
	{
#ifdef CACHE
		if (*bg_dirty)
		{
			*bg_dirty = 0;
#endif
			unsigned int attr = *ColorRom;
			unsigned int code = *VideoRom + ((attr & 0x40)<<2);
			unsigned int color = attr & 0x0f;
			unsigned int flip = attr & 0x30;

			*map2 = map2[0x1000] = *map = map[0x1000] = (color << 12 | flip << 6 | (code&0x1FF)) ;
#ifdef CACHE
		}
		bg_dirty++;
		ColorRom++;
		VideoRom++;
		map2++;
		map++;
#endif
	}
*/
	INT32 scroll = Rom[0xe006] | (Rom[0xe026] << 8);
	unsigned char *DrvColRAM = &Rom[0xc040];
	unsigned char *DrvVidRAM = &Rom[0xc840];
	memset4_fast(&ss_scl[64],scroll | (scroll<<16),0x300);

//	for (UINT32 offs = 0x40; offs < 0x7c0; offs++)
	for (UINT32 offs = 0; offs < 0x780; offs++)	
	{
#ifdef CACHE
		if (bg_dirtybuffer[offs])
		{
			bg_dirtybuffer[offs] = 0;
#endif
			UINT32 attr = DrvColRAM[offs];
			UINT32 code = DrvVidRAM[offs] + ((attr & 0x40)<<2);
			UINT32 color = attr & 0x0f;
			UINT32 flip = attr & 0x30;

			ss_map2[offs] = ss_map2[offs+0x1000] = ss_map[offs] = ss_map[offs+0x1000] = (color << 12 | flip << 6 | code&0x1FF) ;
#ifdef CACHE
		}
#endif
	}	
}
//-------------------------------------------------------------------------------------------------------------------------------------
void renderSound(unsigned int *nSoundBufferPos)
{
#ifndef PONY	
/*
	signed short *	nSoundBuffer = (signed short *)0x25a20000;
	SN76496Update(0, &nSoundBuffer[nSoundBufferPos[0]], nBurnSoundLen);
	nSoundBufferPos[0]+=nBurnSoundLen;

	if(nSoundBufferPos[0]>=RING_BUF_SIZE/2)//0x2400)
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos[0]);
		PCM_Task(pcm); // bon emplacement
		nSoundBufferPos[0]=0;
	}
	*/
#else
	signed short *nSoundBuffer2 = (signed short *)nSoundBuffer+(nSoundBufferPos[0]<<1);

	SN76496Update(0, nSoundBuffer2, nBurnSoundLen);
	
	nSoundBufferPos[0]+=nBurnSoundLen;
	
	if(nSoundBufferPos[0]>=nBurnSoundLen*10)
	{
		pcm_play(pcm1, PCM_SEMI, 7);
		nSoundBufferPos[0]=0;
	}
#endif	
}
//-------------------------------------------------------------------------------------------------------------------------------------
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

	unsigned int nInterleave = game_type ? 16 : 32;
#ifdef CZ80
	if(game_type & 2)
		CZetOpen(0);
#endif
	unsigned int nCyclesDone = 0, nCyclesTotal;

if (_30_HZ)
#ifdef RAZE
	nCyclesTotal = 3072000 / 3.5 / (6000 / 256);
#else
	nCyclesTotal = 3072000 / 3 / (6000 / 256);
#endif
else
#ifdef RAZE
	nCyclesTotal = 3072000 / 7 / (6000 / 256);
#else
	nCyclesTotal = 3072000 / 9 / (6000 / 256);
#endif

	for (unsigned int i = 0; i < nInterleave; i++)
	{
#ifdef CZ80
		nCyclesDone += CZetRun(nCyclesTotal / nInterleave);
#else
#ifdef RAZE
		nCyclesDone += z80_emulate(nCyclesTotal / nInterleave);
#endif
#endif

		if (irq_enable && i == (nInterleave - 1)) {
#ifdef CZ80
			CZetRaiseIrq(0);
#else
#ifdef RAZE
			z80_raise_IRQ(0);
			z80_emulate(1);
//			z80_lower_IRQ(0);
//			z80_emulate(0);
#endif
#endif
		}

		if (nmi_enable && (i & 1)) {
#ifdef CZ80
			CZetNmi();
#else
#ifdef RAZE
			z80_cause_NMI();
#endif
#endif
		}
	}

#ifdef CZ80
	if(game_type & 2)
		CZetClose();
#endif

	SPR_RunSlaveSH((PARA_RTN*)renderSound,&nSoundBufferPos);
	DrvDraw();

	cleanSprites();
	gberet_draw_sprites();
	
	SPR_WaitEndSlaveSH();
#ifdef PONY
	ss_reg     = (SclNorscl *)SS_REG;
//	int scroll = Rom[0xe006] | (Rom[0xe026] << 8);
	_spr2_transfercommand();
	SCL_SetLineParamNBG0(&lp);
//	SclProcess = 2;
//	ss_reg->n0_move_y = 32<<16;
//	ss_reg->n0_move_x = scroll<<16;	
	frame_x++;
	
	 if(frame_x>=frame_y)
		wait_vblank();
#endif	
}
int gberetInit()
{
	game_type = 0;
	_30_HZ =1;
	nBurnLinescrollSize = 0x360;
//	nBurnSprites = 51;
	return DrvInit();
}

int mrgoemonInit()
{
	game_type = 2;
	_30_HZ=0;
	nBurnLinescrollSize = 0x380;
//	nBurnSprites = 51;
	return DrvInit();
}

