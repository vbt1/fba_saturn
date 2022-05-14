// FB Alpha Bank Panic Driver Module
// Based on MAME driver by Nicola Salmoria
#define PONY
//#define CZ80 1
#define RAZE 1
#define CACHE 1
#define nCyclesTotal 2578000 / 60
#include "d_bankp.h"
#ifdef PONY
#include "saturn/pcmstm.h"

int pcm1=0;
Sint16 *nSoundBuffer=NULL;
extern unsigned short frame_x;
extern unsigned short frame_y;
#endif
 
int ovlInit(char *szShortName)
{
	cleanBSS();
	
	struct BurnDriver nBurnDrvbankp	 = {
	"bankp", NULL, 
	"Bank Panic",  
	bankpRomInfo, bankpRomName, bankpInputInfo, bankpDIPInfo,
	DrvInit, DrvBpExit, DrvFrame//, NULL, 
	};

	struct BurnDriver nBurnDrvcombh = {
	"combh", "bankp", 
	"Combat Hawk",
	combhRomInfo, combhRomName, combhInputInfo, combhDIPInfo,
	DrvChInit, DrvBpExit, DrvFrame//, NULL
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

	ss_reg   = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
//	ss_regd  = (SclDataset *)SS_REGD;
	return 0;
}

INT32 DrvChInit()
{
	DrvInit();
	ss_reg->n2_move_y =  0;//(0<<16) ;
	ss_reg->n2_move_x =  0;
}

inline void MemIndex()
{
	extern unsigned int _malloc_max_ram;
	UINT8 *Next; Next = (unsigned char *)&_malloc_max_ram;
	memset(Next, 0, MALLOC_MAX);
	
	Rom  = (UINT8 *)Next; Next += 0x0e000;
	Gfx0 = (UINT8 *)Next; Next += 0x10000;
	Gfx1 = (UINT8 *)Next; Next += 0x20000;
	Prom = (UINT8 *)Next; Next += 0x220;
	Palette = (UINT32 *)Next; Next += 0x100 * sizeof(UINT32);
	map_offset_lut	= (UINT16*)Next; Next += 0x400 * sizeof(UINT16);
	
	make_lut();	
}

/*static*/ INT32 DrvInit()
{
	DrvInitSaturn();
	MemIndex();

	for (UINT32 i = 0; i < 4; i++)
	{
		if (BurnLoadRom(Rom + i * 0x4000, i +  0, 1)) return 1;
	}
	if (BurnLoadRom(Gfx0 + 0x0000, 4, 1)) return 1;
	if (BurnLoadRom(Gfx0 + 0x2000, 5, 1)) return 1;

	for (UINT32 i = 0; i < 6; i++)
	{
		if (BurnLoadRom(Gfx1 + i * 0x2000, i +  6, 1)) return 1;
	}
	if (BurnLoadRom(Prom + 0x0000, 12, 1)) return 1;
	if (BurnLoadRom(Prom + 0x0020, 13, 1)) return 1;
	if (BurnLoadRom(Prom + 0x0120, 14, 1)) return 1;
	bankp_gfx_decode();
	bankp_palette_init();
#ifdef RAZE
	z80_init_memmap();
	z80_map_fetch (0x0000, 0xdfff, Rom + 0x0000); 
	z80_map_read  (0x0000, 0xdfff, Rom + 0x0000); 
	z80_map_fetch (0xe000, 0xefff, Rom + 0xe000);
	z80_map_read  (0xe000, 0xefff, Rom + 0xe000);
	z80_map_write (0xe000, 0xefff, Rom + 0xe000);
	z80_map_read  (0xf000, 0xffff, Rom + 0xf000);	
	z80_map_fetch (0xf000, 0xffff, Rom + 0xf000);

	z80_end_memmap();

#ifdef CACHE
	z80_add_write(0xf000, 0xf3ff, 1, (void *)&bankp_write_f000);
	z80_add_write(0xf400, 0xf7ff, 1, (void *)&bankp_write_f400);
	z80_add_write(0xf800, 0xfbff, 1, (void *)&bankp_write_f800);
	z80_add_write(0xfc00, 0xffff, 1, (void *)&bankp_write_fc00);
#else
	z80_map_write (0xf000, 0xffff, Rom + 0xf000); //1 write
#endif
	z80_set_in((unsigned char (*)(unsigned short))&bankp_in);
	z80_set_out((void (*)(unsigned short, unsigned char))&bankp_out);
#else
	CZetInit2(1,CZ80Context);
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

	DrvDoReset();
	return 0;
}

#ifdef CACHE
#ifdef RAZE
/*static*/ void __fastcall bankp_write_f000(unsigned short address, unsigned char data)
{
	if(Rom[address]!=data)
	{
		Rom[address] = data;
		fg_line(address&0x3ff);
	}
}

/*static*/ void __fastcall bankp_write_f400(unsigned short address, unsigned char data)
{
	if(Rom[address]!=data)
	{
		Rom[address] = data;
		fg_line(address&0x3ff);
	}
}

/*static*/ void __fastcall bankp_write_f800(unsigned short address, unsigned char data)
{
	if(Rom[address]!=data)
	{
		Rom[address] = data;
		bg_line(address&0x3ff);
	}
}

/*static*/ void __fastcall bankp_write_fc00(unsigned short address, unsigned char data)
{
	if(Rom[address]!=data)
	{
		Rom[address] = data;
		bg_line(address&0x3ff);
	}
}
#else
/*static*/ void __fastcall bankp_write(unsigned short address, unsigned char data)
{
	if (address >= 0xf000 && address <= 0xf7ff) 
	{
		if(Rom[address]!=data)
		{
			Rom[address] = data;
			fg_line(address&0x3ff);
		}
		return;
	}

	if (address >= 0xf800 && address <= 0xffff) 
	{
		if(Rom[address]!=data)
		{
			Rom[address] = data;
			bg_line(address&0x3ff);
		}
		return;
	}
}
#endif
#endif

/*static*/ UINT8 __fastcall bankp_in(UINT16 address)
{
	switch (address & 0xff)
	{
		case 0x00:
		case 0x01:
		case 0x02:
			return DrvInputs[address & 3];

		case 0x04:
			return DrvDips;
	}

	return 0;
}

/*static*/ void __fastcall bankp_out(UINT16 address, UINT8 data)
{
	switch (address & 0xff)
	{
		case 0x00: 
		case 0x01:
		case 0x02:		
//			PSG_Write(0,data);
			SN76496Write(address&3, data);
		return;

		case 0x05:
			if (flipscreen)
				ss_reg->n2_move_y = 272-(data);//-24;
			else
				ss_reg->n2_move_x = data;//-24;
		return;

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
		}
		break;
	}
}

void DrvDoReset()
{
	memset (Gfx0, 0, 0x2000);
//	SN76496Reset(0);
//	SN76496Reset(1);
//	SN76496Reset(2);
#ifdef RAZE
//	z80_reset();
#else
	CZetOpen(0);
	CZetReset();
	CZetClose();
#endif
	priority = 0;
	interrupt_enable = 0;
}

void bankp_palette_init()
{
	unsigned int t_pal[32];
	unsigned int i;

	init_32_colors(t_pal,Prom);
	
	Prom+=32;
//nbg2 mauvais

	unsigned int delta=0;
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
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void bankp_gfx_decode()
{
	 INT32 Char1PlaneOffsets[2] = { 0x00, 0x04 };
	 INT32 Char2PlaneOffsets[3] = { 0x00, 0x20000, 0x40000 };
	 INT32 Char1XOffsets[8]     = { 0x43, 0x42, 0x41, 0x40, 0x03, 0x02, 0x01, 0x00 };
	 INT32 Char2XOffsets[8]     = { 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00 };
	 INT32 CharYOffsets[8]      = { 0x00, 0x08, 0x10, 0x18, 0x20, 0x28, 0x30, 0x38 };

	UINT8 *ss_cache =(UINT8 *)SS_CACHE;

	GfxDecode4Bpp(0x400, 2, 8, 8, Char1PlaneOffsets, Char1XOffsets, CharYOffsets, 0x080, Gfx0, ss_cache+0x10000);
	GfxDecode4Bpp(0x800, 3, 8, 8, Char2PlaneOffsets, Char2XOffsets, CharYOffsets, 0x040, Gfx1, ss_cache);
	if(flipscreen)
	{
		rotate_tile(0x400,0,ss_cache+0x10000);
		rotate_tile(0x800,0,ss_cache);
	}

	unsigned char *cache_ptr1=(unsigned char *)(ss_cache+0x30000);
	unsigned char *cache_ptr2=(unsigned char *)(ss_cache+0x10000);
	unsigned char *cache_ptr3=(unsigned char *)(ss_cache+0x50000);
	unsigned char *cache_ptr4=(unsigned char *)(ss_cache+0x00000);
	
	for (UINT32 i=0x00000;i<0x10000;i++ ) 
	{
		*cache_ptr1++=(*cache_ptr2++)+0x44;
		*cache_ptr3++=(*cache_ptr4++)+0x88;
	}
/*	
//nbg2  
	for (UINT32 i=0x10000;i<0x20000;i++ )
		cache[i+0x20000] = cache[i]+0x44;
 //nbg1
	for (UINT32 i=0x00000;i<0x10000;i++ )
		cache[i+0x50000] = cache[i]+0x88;
*/	
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initLayers()
{
    Uint16	CycleTb[]={
		0x1f56, 0xeeee, //A0
		0xeeee, 0xeeee,	//A1
		0xf5f2,0x4eff,   //B0
		0xeeee, 0xeeee  //B1
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
	scfg.flip          = SCL_PN_10BIT; // on force à 0
	scfg.plate_addr[0] = (Uint32)SS_MAP2;
	scfg.plate_addr[1] = (Uint32)0;
	scfg.plate_addr[2] = (Uint32)0;
	scfg.plate_addr[3] = (Uint32)0;
	SCL_SetConfig(SCL_NBG1, &scfg);
// 3 nbg
	scfg.platesize     = SCL_PL_SIZE_2X2; // ou 2X2 ?
	scfg.plate_addr[0] = (Uint32)SS_MAP;
	scfg.plate_addr[1] = (Uint32)SS_MAP;
	scfg.plate_addr[2] = (Uint32)SS_MAP;
	scfg.plate_addr[3] = (Uint32)SS_MAP;

	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	   = SCL_OVER_0;
	scfg.plate_addr[0] = (Uint32)SS_FONT;
	scfg.plate_addr[1] = (Uint32)0;
	scfg.plate_addr[2] = (Uint32)0;
	scfg.plate_addr[3] = (Uint32)0;
// 3 nbg	
	SCL_SetConfig(SCL_NBG0, &scfg);
	SCL_SetCycleTable(CycleTb);	
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initPosition()
{
	SCL_Open();
	ss_reg->n1_move_y =  16 <<16;
	ss_reg->n2_move_y =  16;//(0<<16) ;
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initColors()
{
	colBgAddr  = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);	  //ON
	SCL_AllocColRam(SCL_NBG3,ON);
	SCL_AllocColRam(SCL_NBG3,ON);
	colBgAddr2 = (Uint16*)SCL_AllocColRam(SCL_NBG2,OFF);//OFF);
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void make_lut(void)
{
	INT32 sx, sy;

	for (UINT32 i = 0; i < 0x400;i++) 
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
void DrvInitSaturn()
{
//	nBurnSoundLen = 256;//192;//320; // ou 128 ?
	SS_MAP  = (Uint16 *)SCL_VDP2_VRAM_B1;
	SS_MAP2 = (Uint16 *)SCL_VDP2_VRAM_A1;
	SS_FONT = (Uint16 *)SCL_VDP2_VRAM_B0;
	SS_CACHE= (Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;
	ss_OtherPri     = (SclOtherPriRegister *)SS_OTHR;
	ss_BgColMix		= (SclBgColMixRegister *)SS_BGMIX;
	nBurnLinescrollSize = 1;
	nBurnSprites = 3;

//3 nbg
	SS_SET_N0PRIN(7);
	SS_SET_N1PRIN(4);
	SS_SET_N2PRIN(5);

	initLayers();
	initPosition();
	initColors();
	drawWindow(0,224,240,6,66);
	
#ifdef PONY
	frame_x	= 0;
	nBurnFunction = sdrv_stm_vblank_rq;	
#endif	
}

//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ INT32 DrvBpExit()
{
	DrvDoReset();
#ifdef RAZE
	z80_stop_emulating();
	//z80_add_read(0x0000, 0xffff, 1, (void *)NULL);
	z80_add_write(0xf000, 0xf3ff, 1, (void *)NULL);
	z80_add_write(0xf400, 0xf7ff, 1, (void *)NULL);
	z80_add_write(0xf800, 0xfbff, 1, (void *)NULL);
	z80_add_write(0xfc00, 0xffff, 1, (void *)NULL);
	z80_set_in((unsigned char (*)(unsigned short))NULL);
	z80_set_out((void (*)(unsigned short, unsigned char))NULL);
#else
	CZetExit2();
	CZ80Context = NULL;
#endif	
	SN76496Exit();
/*
	Rom  = Gfx0 = Gfx1 = Prom = NULL;
	Palette = NULL;
	map_offset_lut = NULL;

//	free(Mem);
//	Mem = NULL;
	DrvDips=priority=flipscreen=interrupt_enable=0;
*/	
#ifdef PONY
remove_raw_pcm_buffer(pcm1);
#endif

	//cleanDATA();
	cleanBSS();

	nSoundBufferPos=0;
		
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void 	 bg_line(UINT16 offs)
{
	UINT32 code, color;
	UINT32 flipx = 0xC000;
	UINT8 *rom = Rom+0xf800+offs;
	code = *rom;
	rom+=0x400;
	code |= ((*rom & 7) << 8);
	color = (*rom >> 4) & 0x0f;

	if (!flipscreen) flipx = (*rom & 0x08) << 11;
	
	UINT16 *map2 = ((UINT16 *)SS_MAP2)+map_offset_lut[offs];	
//NBG1
	map2[0] = map2[0x40] = flipx | color;

	if(color==1 || color==0xd)	map2[1] = map2[0x41] = code;
	else						map2[1] = map2[0x41] = code+0x2800;
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void 	 fg_line(UINT16 offs)
{
	UINT32 code, color;
	UINT32 flipx = 0xC000;
	UINT8 *rom = &Rom[0xf000];
	code = rom[offs];
	rom+=0x400;
	code |= ((rom[offs] & 3) << 8);
	color = (rom[offs] >> 3) & 0x1f;

	if(!flipscreen)	flipx = (rom[offs] & 0x04) << 12;
	
	UINT16 *map = ((UINT16 *)SS_MAP)+map_offset_lut[offs];	
//NBG2		   //0x40
	map[0] = map[0x40] = map[0x1000] = map[0x1040] = flipx | color;//color /8;

	if(Palette[color<<2]==0x8000)	map[1] = map[0x41] = map[0x1001] = map[0x1041] = code+0x800;
	else							map[1] = map[0x41] = map[0x1001] = map[0x1041] = code+0x1800;//2048  //0x1800
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
	memset (DrvInputs, 0, 3);

	for (INT32 i = 0; i < 8; i++) {
		DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
		DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
		DrvInputs[2] ^= (DrvJoy3[i] & 1) << i;
	}	
	
#ifdef RAZE
	z80_emulate(nCyclesTotal);
//z80_emulate(3867120 / 60);
//3867120
	if (interrupt_enable) z80_cause_NMI();
#else
//	CZetOpen(0);
	CZetRun(nCyclesTotal);
	if (interrupt_enable) CZetNmi();
//	CZetClose();
#endif

#ifndef PONY	
	signed short *nSoundBuffer = (signed short *)0x25a20000;
	SN76496Update(0, &nSoundBuffer[nSoundBufferPos], SOUND_LEN);
	SN76496Update(1, &nSoundBuffer[nSoundBufferPos], SOUND_LEN);
	SN76496Update(2, &nSoundBuffer[nSoundBufferPos], SOUND_LEN);

	nSoundBufferPos+=(SOUND_LEN); // DOIT etre deux fois la taille copiee

	if(nSoundBufferPos>=0x3C00)//RING_BUF_SIZE)
//	if(nSoundBufferPos>=(2048L*10))
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos);
		PCM_Task(pcm); // bon emplacement
		nSoundBufferPos=0;
	}
#else
	signed short *nSoundBuffer2 = (signed short *)nSoundBuffer+(nSoundBufferPos<<1);

	SN76496Update(0, &nSoundBuffer2[0], nBurnSoundLen);
	SN76496Update(1, &nSoundBuffer2[0], nBurnSoundLen);
	SN76496Update(2, &nSoundBuffer2[0], nBurnSoundLen);
	
	nSoundBufferPos+=nBurnSoundLen;
	
	if(nSoundBufferPos>=nBurnSoundLen*10)
	{
		pcm_play(pcm1, PCM_SEMI, 7);
		nSoundBufferPos=0;
	}
	
	SclProcess = 1;
	frame_x++;
	
	 if(frame_x>=frame_y)
		wait_vblank();	
#endif	
}
