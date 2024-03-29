// FB Alpha Penguin-Kun Wars Driver Module
// Based on MAME Driver by David Haywood and Phil Stroffolino
#define CZ80 1
//#define RAZE 1  // `EMULATE_R_REGISTER obligatoire
#include "d_pkunwar.h"
#define nInterleave 256

#define PONY

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

	struct BurnDriver nBurnDrvpkunwar = {
		"pkunw", NULL,
		"Penguin-Kun Wars (US)",
		pkunwarRomInfo, pkunwarRomName, PkunwarInputInfo, PkunwarDIPInfo,
		DrvInit, DrvExit, DrvFrame
	};

	struct BurnDriver nBurnDrvNova2001u = {
		"nova2001", "pkunw",
		"Nova 2001 (US)",
		nova2001uRomInfo, nova2001uRomName, Nova2001InputInfo, Nova2001DIPInfo,
		NovaInit, DrvExit, NovaFrame
	};

	struct BurnDriver nBurnDrvNinjakun = {
		"ninjakun", "pkunw",
		"Ninjakun Majou no Bouken",
		ninjakunRomInfo, ninjakunRomName, NinjakunInputInfo, NinjakunDIPInfo,
		NinjakunInit, DrvExit, NinjakunFrame
	};
/*
	struct BurnDriver nBurnDrvRaiders5 = {
		"raiders5", "pkunw",
		"Raiders5\0",
		raiders5RomInfo, raiders5RomName, Raiders5InputInfo, Raiders5DIPInfo,
		Raiders5Init, DrvExit, Raiders5Frame
	};
*/
	if (strcmp(nBurnDrvpkunwar.szShortName, szShortName) == 0)
		memcpy(shared,&nBurnDrvpkunwar,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvNova2001u.szShortName, szShortName) == 0)
		memcpy(shared,&nBurnDrvNova2001u,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvNinjakun.szShortName, szShortName) == 0)
		memcpy(shared,&nBurnDrvNinjakun,sizeof(struct BurnDriver));
//	if (strcmp(nBurnDrvRaiders5.szShortName, szShortName) == 0)
//		memcpy(shared,&nBurnDrvRaiders5,sizeof(struct BurnDriver));
	ss_reg   = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
	
	return 0;
}

// Memory Handlers
//---------------------------------------------------------------------------------------------------------
inline void fg_nova2001_line(UINT16 offs,UINT8 *ram_base, UINT16 *ss_map)
{
	UINT32 attr = ram_base[offs + 0x400];
	UINT32 group = (attr >> 4) & 1;

	if (group != 0) return;
	UINT32 code = ram_base[offs + 0x000];
//	offs = (offs & 0x1f) | (offs / 0x20) <<6;
	offs = offs_lut[offs];
	ss_map[offs] = (attr & 0x0f) <<12 | code;
//	*((UINT16*)SS_MAP+offs_lut[offs]/2)= (attr & 0x0f) <<12 | code;
}

//UINT32 offs_calc[512];
//---------------------------------------------------------------------------------------------------------
inline void bg_nova2001_line(UINT16 offs,UINT8 *ram_base, UINT16 *ss_map)
{

	UINT32 code = ram_base[offs + 0x000] + 0x200;
	UINT32 attr = ram_base[offs + 0x400];
//	offs = (offs & 0x1f) | (offs / 0x20) <<6;
	offs = offs_lut[offs];

	UINT16 *map = &ss_map[offs];
	map[0X20] = map[0X800] = map[0X820] = 
	map[0] = (attr & 0x0f) <<12 | code;
}
//---------------------------------------------------------------------------------------------------------
void fg_ninjakun_line(UINT16 offs,UINT8 *ram_base, UINT16 *ss_map)
{
	UINT32 code = ram_base[offs + 0x000];
	UINT32 attr = ram_base[offs + 0x400];	
//	offs = (offs & 0x1f) | (offs / 0x20) <<6;
	offs = offs_lut[offs];

	code |= (attr & 0x20) *8;
	ss_map[offs] = (attr & 0x0f) <<12 | code & 0xfff;
}
//---------------------------------------------------------------------------------------------------------
void bg_ninjakun_line(UINT16 offs,UINT8 *ram_base, UINT16 *ss_map)
{
	UINT32 code = ram_base[offs + 0x000] + 0x400;
	UINT32 attr = ram_base[offs + 0x400];

//	offs = (offs & 0x1f) | (offs / 0x20) <<6;
	offs = offs_lut[offs];

	UINT16 *map = (UINT16 *)ss_map+offs;
	code |= (attr & 0xC0) << 2;
	map[0X20] = map[0X800] = map[0X820] = 
	map[0] = (attr & 0x0f) <<12 | code & 0xfff;
}
//---------------------------------------------------------------------------------------------------------
/*
void fg_raiders5_line(UINT32 offs,UINT8 *ram_base, UINT16 *ss_map)
{
	UINT32 attr = ram_base[offs + 0x400];
	UINT32 code = ram_base[offs + 0x000];
//	offs = (offs & 0x1f) | (offs / 0x20) <<6;
	offs = offs_lut[offs];

	ss_map[offs] = (attr & 0xf0) <<8 | code;
}
//---------------------------------------------------------------------------------------------------------
void bg_raiders5_line(UINT32 offs,UINT8 *ram_base, UINT16 *ss_map)
{
	UINT32 attr = ram_base[offs + 0x400];
	UINT32 code = ram_base[offs + 0x000] + ((attr & 0x01) << 8) + 0x400;
//	offs = (offs & 0x1f) | (offs / 0x20) <<6;
	offs = offs_lut[offs];

	ss_map[offs+0X20] = ss_map[offs+0X800] = ss_map[offs+0X820] =
	ss_map[offs] = (attr & 0xf0) <<8 | code;
}
*/
//---------------------------------------------------------------------------------------------------------
/*
UINT8 __fastcall raiders5_main_read(UINT16 address)
{
	if(address >= 0x9000 && address <= 0x97ff)
	{
		return DrvBgRAM[(((address & 0x3ff) + (xscroll >> 3) + ((yscroll >> 3) << 5)) & 0x3ff) + (address & 0x400)];
//		return DrvBgRAM[address];
	}

	switch (address)
	{
		case 0xc001:
			return AY8910Read(0);

		case 0xc003:
			return AY8910Read(1);
	}

	return 0;
}
//---------------------------------------------------------------------------------------------------------
void __fastcall raiders5_main_write(UINT16 address, UINT8 data)
{
	if(address>=0x9000 && address<=0x97ff)
	{
		address = (((address & 0x3ff) + (xscroll >> 3) + ((yscroll >> 3) << 5)) & 0x3ff) + (address & 0x400);
		if(DrvBgRAM[address]!=data)
		{
			DrvBgRAM[address]=data;
			bg_raiders5_line(address & 0x3ff,DrvBgRAM,SCL_VDP2_VRAM_B0);
		}
		return;
	}

	if(address>=0x8800 && address<=0x8fff)
	{
		if(DrvMainROM[address]!=data)
		{
			DrvMainROM[address]=data;
			fg_raiders5_line(address &0x3ff,DrvFgRAM,SCL_VDP2_VRAM_A0);
		}
		return;
	}

	if(address >= 0xd000 && address <= 0xd1ff)
	{
		INT32 offset = address - 0xd000;

		if(DrvPalRAM[offset] != data)
		{
			DrvPalRAM[offset] = data;
			colAddr[offset] = cram_lut[data];

			if (offset < 16) {
				colAddr[0x200 + offset * 16 + 1] = cram_lut[data];

				if (offset != 1) {
					for (INT32 i = 0; i < 16; i++) 
					{
						colAddr[0x200 + offset + i * 16] = cram_lut[data];
					}
				}
			}
		}
		return;
	}

	switch (address)
	{
		case 0xa000:
			xscroll = data;
			break;

		case 0xa001:
			yscroll = data;
			break;

		case 0xc000:
			AY8910Write(0, 0, data);
			break;

		case 0xc001:
			AY8910Write(0, 1, data);
			break;

		case 0xc002:
			AY8910Write(1, 0, data);
			break;

		case 0xc003:
			AY8910Write(1, 1, data);
			break;

		case 0xa002:
			flipscreen = ~data & 0x01;
			break;
	}
}
//---------------------------------------------------------------------------------------------------------
UINT8 __fastcall raiders5_sub_read(UINT16 address)
{
	switch (address)
	{
		case 0x8001:
			return AY8910Read(0);

		case 0x8003:
			return AY8910Read(1);

		case 0x9000:
		case 0xc000:
		case 0xc800:
		case 0xd000:
			return 0; // NOP
	}

	return 0;
}

UINT8 __fastcall raiders5_in(UINT16 address)
{
	return 0; // NOP
}
//---------------------------------------------------------------------------------------------------------
void __fastcall raiders5_sub_write(UINT16 address, UINT8 data)
{
	switch (address)
	{
		case 0x8000:
			AY8910Write(0, 0, data);
			break;

		case 0x8001:
			AY8910Write(0, 1, data);
			break;

		case 0x8002:
			AY8910Write(1, 0, data);
			break;

		case 0x8003:
			AY8910Write(1, 1, data);
			break;

		case 0xe000:
			xscroll = data;
			break;

		case 0xe001:
			yscroll = data;
			break;

		case 0xe002:
			flipscreen = ~data & 0x01;
			break;
	}
}
*/
unsigned char __fastcall pkunwar_read(unsigned short address)
{
	switch (address)
	{
		case 0xa001:
			return AY8910Read(0);

		case 0xa003:
			return AY8910Read(1);
	}

	return 0;
}
//---------------------------------------------------------------------------------------------------------
void __fastcall pkunwar_write(unsigned short address, unsigned char data)
{
	switch (address)
	{
		case 0xa000:
			AY8910Write(0, 0, data);
		break;

		case 0xa001:
			AY8910Write(0, 1, data);
		break;

		case 0xa002:
			AY8910Write(1, 0, data);
		break;

		case 0xa003:
			AY8910Write(1, 1, data);
		break;
	}
}
//---------------------------------------------------------------------------------------------------------
/*
void __fastcall pkunwar_out(unsigned short address, unsigned char data)
{
	address &= 0xff;

//	if (address == 0) flipscreen = data & 1;
}*/
//---------------------------------------------------------------------------------------------------------
UINT8 __fastcall nova2001_read(UINT16 address)
{
	switch (address)
	{
		case 0xc000:
			return AY8910Read(0);

		case 0xc001:
			return AY8910Read(1);

		case 0xc004:
			watchdog = 0;
			return 0;

		case 0xc006:
			return DrvInputs[0];

		case 0xc007:
			return DrvInputs[1];

		case 0xc00e:
			return (DrvInputs[2] & 0x7f) | vblank;
	}

	return 0;
}
//---------------------------------------------------------------------------------------------------------
void __fastcall nova2001_write(UINT16 address, UINT8 data)
{
	if(address>=0xa800 && address<=0xafff)
	{
		if(DrvMainROM[address]!=data)
		{
			DrvMainROM[address]=data;
			bg_nova2001_line(address &0x3ff,DrvBgRAM,SCL_VDP2_VRAM_B0);
		}
		return;
	}

	if(address>=0xa000 && address<=0xa7ff)
	{
		if(DrvMainROM[address]!=data)
		{
			DrvMainROM[address]=data;
			fg_nova2001_line(address &0x3ff,DrvFgRAM,SCL_VDP2_VRAM_A0);
		}
		return;
	}

	switch (address)
	{
		case 0xbfff:
//			flipscreen = ~data & 0x01;
			break;

		case 0xc000:
			AY8910Write(0, 1, data);
			break;

		case 0xc002:
			AY8910Write(0, 0, data);
			break;

		case 0xc001:
			AY8910Write(1, 1, data);
			break;

		case 0xc003:
			AY8910Write(1, 0, data);
			break;
	}
}
//---------------------------------------------------------------------------------------------------------
UINT8 __fastcall ninjakun_main_read(UINT16 address)
{
	if ((address & 0xf800) == 0xc800) {
		return DrvBgRAM[(((address & 0x3ff) + (xscroll >> 3) + ((yscroll >> 3) << 5)) & 0x3ff) + (address & 0x400)];
	}

	switch (address)
	{
		case 0x8001:
			return AY8910Read(0);

		case 0x8003:
			return AY8910Read(1);

		case 0xa000:
			return DrvInputs[0] ^ 0xc0;

		case 0xa001:
			return DrvInputs[1] ^ 0xc0;

		case 0xa002:
			return ((vblank ? 0 : 2) | (ninjakun_ioctrl << 2));
	}

	return 0;
}
//---------------------------------------------------------------------------------------------------------------
void __fastcall ninjakun_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xc800) 
	{
		address = 0x7ff & (((address & 0x3ff) + (xscroll >> 3) + ((yscroll >> 3) << 5)) & 0x3ff) + (address & 0x400);

		if(DrvBgRAM[address]!=data)
		{
			DrvBgRAM[address]=data;
			bg_ninjakun_line(address  & 0x3ff, DrvBgRAM, SCL_VDP2_VRAM_B0);
		}
		return;
	}

	if(address >= 0xc000 && address <= 0xc7ff)
	{
		if(DrvMainROM[address]!=data)
		{
			DrvMainROM[address]=data;
			fg_ninjakun_line(address  & 0x3ff, DrvFgRAM, SCL_VDP2_VRAM_A0);
		}
		return;
	}

	if(address >= 0xd800 && address <= 0xd9ff)
	{
		UINT32 offset = address - 0xd800;

		if(DrvPalRAM[offset] != data)
		{
			DrvPalRAM[offset] = data;
			colBgAddr[offset] = colAddr[offset] = cram_lut[data];

			if (offset < 16) {
				unsigned int delta = 0x200 + offset * 16 + 1;
				colBgAddr[delta] = colAddr[delta] = cram_lut[data];

				/*if (offset != 1) {
					for (UINT32 i = 0; i < 16; i++) 
					{
						unsigned int delta = 0x200 + offset + i * 16;
						colBgAddr[delta] = colAddr[delta] = cram_lut[data];
					}
				}*/
			}
		}
		return;
	}

	switch (address)
	{
		case 0x8000:
			AY8910Write(0, 0, data);
			break;

		case 0x8001:
			AY8910Write(0, 1, data);
			break;

		case 0x8002:
			AY8910Write(1, 0, data);
			break;

		case 0x8003:
			AY8910Write(1, 1, data);
			break;

		case 0xa002:
			if (data == 0x80) ninjakun_ioctrl |=  0x01;
			if (data == 0x40) ninjakun_ioctrl &= ~0x02;
			break;

		case 0xa003:
//			flipscreen = ~data & 0x01;
			break;
	}
}
//---------------------------------------------------------------------------------------------------------------
UINT8 __fastcall ninjakun_sub_read(UINT16 address)
{

	if ((address & 0xf800) == 0xc800) {
		return DrvBgRAM[(((address & 0x3ff) + (xscroll >> 3) + ((yscroll >> 3) << 5)) & 0x3ff) + (address & 0x400)];
	}

	switch (address)
	{
		case 0x8001:
			return AY8910Read(0);

		case 0x8003:
			return AY8910Read(1);

		case 0xa000:
			return DrvInputs[0] ^ 0xc0;

		case 0xa001:
			return DrvInputs[1] ^ 0xc0;

		case 0xa002:
			return ((vblank ? 0 : 2) | (ninjakun_ioctrl << 2));
	}

	return 0;
}
//---------------------------------------------------------------------------------------------------------------
void __fastcall ninjakun_sub_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xc800) 
	{
		address = 0x7ff & (((address & 0x3ff) + (xscroll >> 3) + ((yscroll >> 3) << 5)) & 0x3ff) + (address & 0x400);

		if(DrvBgRAM[address]!=data)
		{
			DrvBgRAM[address]=data;
			bg_ninjakun_line(address & 0x3ff, DrvBgRAM, SCL_VDP2_VRAM_B0);
		}
		return;
	}

	if(address >= 0xc000 && address <= 0xc7ff)
	{
		if(DrvMainROM[address]!=data)
		{
			DrvMainROM[address]=data;
			fg_ninjakun_line(address  & 0x3ff, DrvFgRAM, SCL_VDP2_VRAM_A0);
		}
		return;
	}

	if(address >= 0xd800 && address <= 0xd9ff)
	{
		UINT32 offset = address - 0xd800;

		if(DrvPalRAM[offset] != data)
		{
			DrvPalRAM[offset] = data;
			colBgAddr[offset] = colAddr[offset] = cram_lut[data];

			if (offset < 16) {
				unsigned int delta = 0x200 + offset * 16 + 1;
				colBgAddr[delta] = colAddr[delta] = cram_lut[data];

				/*if (offset != 1) {
					for (INT32 i = 0; i < 16; i++) 
					{
						unsigned int delta = 0x200 + offset + i * 16;
						colBgAddr[delta] = colAddr[delta] = cram_lut[data];
					}
				}*/
			}
		}
		return;
	}

	switch (address)
	{
		case 0x8000:
			AY8910Write(0, 0, data);
			break;

		case 0x8001:
			AY8910Write(0, 1, data);
			break;

		case 0x8002:
			AY8910Write(1, 0, data);
			break;

		case 0x8003:
			AY8910Write(1, 1, data);
			break;

		case 0xa002:
			if (data == 0x40) ninjakun_ioctrl |=  0x02;
			if (data == 0x80) ninjakun_ioctrl &= ~0x01;
			break;

		case 0xa003:
		//	flipscreen = ~data & 0x01;
			break;
	}
}
//-------------------------------------------------------------------------------------------------
// AY8910 Ports


unsigned char pkunwar_port_0(unsigned int a)
{

	unsigned char ret = 0x7f | (vblank ^= 0x80);
	unsigned int i;
	for (i = 0; i < 8; i++) {
		ret ^= DrvJoy1[i] << i;
	}

	return ret;
	
//	return (DrvInputs[0] & 0x7f) | (vblank ? 0 : 0x80);	
}

unsigned char pkunwar_port_1(unsigned int a)
{

	unsigned char ret = 0xff;
	unsigned int i;
	for (i = 0; i < 8; i++) {
		ret ^= DrvJoy2[i] << i;
	}

	return ret;
	
//	return (DrvInputs[1] & ~0x40) | (DrvDips[1] & 0x40);	
}

unsigned char pkunwar_port_2(unsigned int a)
{
	return 0xff;
}

unsigned char pkunwar_port_3(unsigned int a)
{
	return DrvDips[0];
}

void nova2001_scroll_x_w(UINT32 offset,UINT32 data)
{
	xscroll = data;
}

void nova2001_scroll_y_w(UINT32 offset,UINT32 data)
{
	yscroll = data;
}

UINT8 nova2001_port_3(UINT32 data)
{
	return DrvDips[0];
}

UINT8 nova2001_port_4(UINT32 data)
{
	return DrvDips[1];
}
/*
UINT8 raiders5_port_0(UINT32 data)
{
	return (DrvInputs[0] & 0x7f) | (vblank ? 0 : 0x80);
}
*/
//-------------------------------------------------------------------------------------------------
// Initialization Routines


void DrvDoReset()
{
//	DrvReset = 0;

	memset (DrvBgRAM, 0, 0x1000);
	memset (DrvMainROM + 0xc000, 0, 0x0800);

//	flipscreen = 0;
#ifdef RAZE
	z80_reset();
#else
	CZetOpen(0);
	CZetReset();
	CZetClose();
#endif
	AY8910Reset(0);
	AY8910Reset(1);

//	flipscreen = 0;
	watchdog = 0;

	xscroll = 0;
	yscroll = 0;
	ninjakun_ioctrl = 0;
}

inline void MemIndex()
{
	extern unsigned int _malloc_max_ram;
	UINT8 *Next; Next = (unsigned char *)&_malloc_max_ram;
	memset(Next, 0, MALLOC_MAX);
	
	DrvMainROM	= Next; Next += 0x030000;
//	DrvSubROM	= Next; Next += 0x010000;
	DrvGfxROM0	= Next; Next += 0x020000;
	DrvColPROM  = Next; Next += 0x000020;
	
	pFMBuffer	= (short *)Next; Next += nBurnSoundLen * 6 * sizeof(short);

	pAY8910Buffer[0] = pFMBuffer + nBurnSoundLen * 0;
	pAY8910Buffer[1] = pFMBuffer + nBurnSoundLen * 1;
	pAY8910Buffer[2] = pFMBuffer + nBurnSoundLen * 2;
	pAY8910Buffer[3] = pFMBuffer + nBurnSoundLen * 3;
	pAY8910Buffer[4] = pFMBuffer + nBurnSoundLen * 4;
	pAY8910Buffer[5] = pFMBuffer + nBurnSoundLen * 5;
	
	offs_lut = (UINT16 *)Next; Next += 0x400 * (sizeof(UINT16));
//	map_lut  = (UINT16 *)Next; Next += 0x400 * (sizeof(UINT16));
	cram_lut = (UINT16 *)Next; Next += 512 * (sizeof(UINT16));
	CZ80Context	= Next; Next += sizeof(cz80_struc)*2;
	
//	DrvGfxTmp	= Next; Next += 0x020000;	
//	MemEnd	= Next;
	make_lut();
}

void pkunwar_palette_init()
{
	for (UINT32 i = 0; i < 0x200; i++)
	{
		UINT32 entry;
		UINT32 intensity,r,g,b;

		if ((i & 0xf) == 1)
		{
			entry = ((i & 0xf0) >> 4) | ((i & 0x100) >> 4);
		}
		else
		{
			entry = ((i & 0x0f) >> 0) | ((i & 0x100) >> 4);
		}

		intensity = DrvColPROM[entry] & 0x03;

		r = (((DrvColPROM[entry] >> 0) & 0x0c) | intensity) * 0x11;
		g = (((DrvColPROM[entry] >> 2) & 0x0c) | intensity) * 0x11;
		b = (((DrvColPROM[entry] >> 4) & 0x0c) | intensity) * 0x11;

		colAddr[i] = colBgAddr[i] = RGB(r>>3,g>>3,b>>3);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
#define STEP2(start, step)	((start) + ((step)*0)), ((start) + ((step)*1))
#define STEP4(start, step)	STEP2(start, step),  STEP2((start)+((step)*2), step)
#define STEP8(start, step)	STEP4(start, step),  STEP4((start)+((step)*4), step)

void DrvGfxDecode(UINT8 *src)
{
	INT32 Planes[4]    = { STEP4(0,1) };
	INT32 XOffsets[16] = { STEP8(0,4), STEP8(256,4) };
	INT32 YOffsets[16] = { STEP8(0,32), STEP8(512,32) };

	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	GfxDecode4Bpp(0x800, 4,  8,  8, Planes, XOffsets, YOffsets, 0x100, src, (UINT8 *)SCL_VDP2_VRAM_B1);
	GfxDecode4Bpp(0x200, 4, 16, 16, Planes, XOffsets, YOffsets, 0x400, src, &ss_vram[0x1100]);
}

inline void lineswap_gfx_roms(UINT8 *rom, INT32 len, const INT32 bit)
{
	UINT8 *tmp = (UINT8*)rom+len;

	const INT32 mask = (1 << (bit + 1)) - 1;

	for (INT32 sa = 0; sa < len; sa++)
	{
		const INT32 da = (sa & ~mask) | ((sa << 1) & mask) | ((sa >> bit) & 1);
		tmp[da] = rom[sa];
	}

	memcpyl (rom, tmp, len);
}

int DrvInit()
{
	DrvInitSaturn(0);
//	MemIndex();

	DrvBgRAM = (UINT8*)DrvMainROM + 0x8000;
	UINT8 *DrvMainRAM = (UINT8*)DrvMainROM + 0xc000;
	CZetInit2(1,CZ80Context);

	LoadRoms();

#ifdef RAZE
	z80_init_memmap();
	z80_map_fetch (0x0000, 0x7fff, DrvMainROM + 0x0000); 
	z80_map_read  (0x0000, 0x7fff, DrvMainROM + 0x0000); //2 read

	z80_map_fetch(0x8000, 0x8fff, DrvBgRAM); 
	z80_map_read (0x8000, 0x8fff, DrvBgRAM); //1 write 
	z80_map_write(0x8000, 0x8fff, DrvBgRAM); //2 read 
	z80_map_fetch(0xc000, 0xcfff, DrvMainRAM); 
	z80_map_read (0xc000, 0xcfff, DrvMainRAM); //1 write 
	z80_map_write(0xc000, 0xcfff, DrvMainRAM); //2 read 
	z80_map_fetch(0xe000, 0xffff, DrvMainROM + 0xe000); 
	z80_map_read (0xe000, 0xffff, DrvMainROM + 0xe000); //2 read
//	z80_map_write(0xe000, 0xffff, DrvMainROM + 0xe000); //
	z80_add_write(0xa000, 0xa003, 1, (void *)&pkunwar_write);
	z80_add_read(0xa000,  0xa003, 1, (void *)&pkunwar_read);
	z80_end_memmap();   
//	z80_set_out((void (*)(unsigned short int, unsigned char))&pkunwar_out);
#else
//	CZetInit(1);
	CZetOpen(0);
//	CZetSetOutHandler(pkunwar_out);
	CZetSetReadHandler(pkunwar_read);
	CZetSetWriteHandler(pkunwar_write);
	
	CZetMapMemory(DrvMainROM,	0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory(DrvMainROM + 0x8000,	0x8000, 0x8fff, MAP_READ|MAP_WRITE);	
	CZetMapMemory(DrvMainROM + 0x0c000,	0xc000, 0xc7ff, MAP_RAM);		
	CZetMapMemory(DrvMainROM + 0x0e000,	0xe000, 0xffff, MAP_ROM);	
//	CZetMemEnd();
	CZetClose();
#endif

	AY8910Init(0, 1500000, nBurnSoundRate, &pkunwar_port_0, &pkunwar_port_1, NULL, NULL);
	AY8910Init(1, 1500000, nBurnSoundRate, &pkunwar_port_2, &pkunwar_port_3, NULL, NULL);
	DrvDoReset();

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
INT32 NovaInit()
{
	DrvInitSaturn(1);
//	MemIndex();
	
	DrvFgRAM = (UINT8*)DrvMainROM + 0xa000;
	DrvBgRAM = (UINT8*)DrvMainROM + 0xa800;
	DrvSprRAM = (UINT8*)DrvMainROM + 0xb000;
	UINT8 *DrvMainRAM = (UINT8*)DrvMainROM + 0xe000;

//	make_nova_lut();
	NovaLoadRoms();

	CZetInit2(1,CZ80Context);
	CZetOpen(0);
	CZetSetReadHandler(nova2001_read);
	CZetSetWriteHandler(nova2001_write);

	CZetMapMemory(DrvMainROM,	0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory(DrvFgRAM,	0xa000, 0xa7ff, MAP_READ);
//	CZetMapArea(0xa000, 0xa7ff, 0, DrvFgRAM);
//	CZetMapArea(0xa000, 0xa7ff, 1, DrvFgRAM);

//	CZetMapArea(0xa800, 0xafff, 0, DrvBgRAM);
	CZetMapMemory(DrvBgRAM,	0xa800, 0xafff, MAP_READ);	
//	CZetMapArea(0xa800, 0xafff, 1, DrvBgRAM); // write

	CZetMapMemory(DrvSprRAM,	0xb000, 0xb7ff, MAP_READ|MAP_WRITE);
	CZetMapMemory(DrvMainRAM,	0xe000, 0xe7ff, MAP_READ|MAP_WRITE);
	CZetClose();

    AY8910Init(0, 2000000, nBurnSoundRate, NULL, NULL, &nova2001_scroll_x_w, &nova2001_scroll_y_w);
    AY8910Init(1, 2000000, nBurnSoundRate, &nova2001_port_3, &nova2001_port_4, NULL, NULL);

	DrvDoReset();

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void LoadRoms()
{
	UINT8 *DrvGfxTmp = (UINT8*)DrvGfxROM0;
	
	BurnLoadRom(DrvMainROM  + 0x0000, 0, 1);
	BurnLoadRom(DrvMainROM  + 0x4000, 1, 1);
	BurnLoadRom(DrvMainROM  + 0xe000, 2, 1);
	
	BurnLoadRom(DrvGfxTmp + 0x0000, 3, 1);
	BurnLoadRom(DrvGfxTmp + 0x4000, 4, 1);
	BurnLoadRom(DrvGfxTmp + 0x8000, 5, 1);
	BurnLoadRom(DrvGfxTmp + 0xc000, 6, 1);

//	DrvGfxDescramble(DrvGfxTmp);
	lineswap_gfx_roms(DrvGfxROM0, 0x10000, 13);
	DrvGfxDecode(DrvGfxTmp);

	BurnLoadRom(DrvColPROM +  0x0000, 7, 1);

	pkunwar_palette_init();
}

inline void NovaLoadRoms()
{
	UINT8 *DrvGfxTmp = (UINT8*)DrvGfxROM0;
	
	BurnLoadRom(DrvMainROM + 0x0000, 0, 1);
	BurnLoadRom(DrvMainROM + 0x2000, 1, 1);
	BurnLoadRom(DrvMainROM + 0x4000, 2, 1);
	BurnLoadRom(DrvMainROM + 0x6000, 3, 1);
	BurnLoadRom(DrvMainROM + 0x7000, 3, 1);

	BurnLoadRom(DrvGfxTmp + 0x0000, 4, 2);
	BurnLoadRom(DrvGfxTmp + 0x0001, 5, 2);
	BurnLoadRom(DrvGfxTmp + 0x4000, 6, 2);
	BurnLoadRom(DrvGfxTmp + 0x4001, 7, 2);

	DrvGfxDecode(DrvGfxTmp);

	BurnLoadRom(DrvColPROM, 8, 1);

	pkunwar_palette_init();
}

void NinjakunLoadRoms()
{
	UINT8 *DrvGfxTmp = (UINT8*)DrvGfxROM0;

	BurnLoadRom(DrvMainROM + 0x0000,  0, 1);
	BurnLoadRom(DrvMainROM + 0x2000,  1, 1);
	BurnLoadRom(DrvMainROM + 0x4000,  2, 1);
	BurnLoadRom(DrvMainROM + 0x6000,  3, 1);
	BurnLoadRom(DrvMainROM + 0x20000,  4, 1);

	BurnLoadRom(DrvGfxTmp + 0x0000,  5, 2);
	BurnLoadRom(DrvGfxTmp + 0x0001,  6, 2);
	BurnLoadRom(DrvGfxTmp + 0x4000,  7, 2);
	BurnLoadRom(DrvGfxTmp + 0x4001,  8, 2);

	BurnLoadRom(DrvGfxTmp +0x8000 + 0x0000,  9, 2);
	BurnLoadRom(DrvGfxTmp +0x8000 + 0x0001, 10, 2);
	BurnLoadRom(DrvGfxTmp +0x8000 + 0x4000, 11, 2);
	BurnLoadRom(DrvGfxTmp +0x8000 + 0x4001, 12, 2);

	DrvGfxDecode(DrvGfxTmp);
}
/*
int Raiders5LoadRoms()
{
	UINT8 *tmp = (UINT8*)0x00240000;
	memset(tmp,0x00,0x20000);
	BurnLoadRom(DrvMainROM + 0x0000,  0, 1)) return 1;
	BurnLoadRom(DrvMainROM + 0x4000,  1, 1)) return 1;
	BurnLoadRom(DrvMainROM  + 0x20000,  2, 1)) return 1;
	BurnLoadRom(DrvGfxROM0 + 0x0000,  3, 1)) return 1;
	BurnLoadRom(DrvGfxROM0 + 0x4000,  4, 1)) return 1;
	BurnLoadRom(DrvGfxROM0 + 0x8000,  5, 1)) return 1;

	for (UINT32 i = 0; i < 8; i++) {
		UINT32 j = ((i & 1) << 2) | ((i >> 1) & 3);
		memcpy (tmp + j * 0x2000, DrvGfxROM0 + i * 0x2000, 0x2000);
	}

	pkunwar_gfx_decode(tmp);
	tmp = NULL;
	return 0;
}
*/

void NinjakunDoReset()
{
	DrvDoReset();
#ifdef RAZE
	z80_reset();
#else
	CZetOpen(1);
	CZetReset();
	CZetClose();
#endif
}
//-------------------------------------------------------------------------------------------------------------------------------------
INT32 NinjakunInit()
{
	DrvInitSaturn(3);
//	MemIndex();

	DrvFgRAM = DrvMainROM + 0xc000;
	DrvBgRAM = DrvMainROM + 0xc800;
	DrvSprRAM = DrvMainROM + 0xd000;
	DrvPalRAM = DrvMainROM + 0xd800;
	UINT8 *DrvMainRAM = DrvMainROM + 0x10000;

//	make_lut();
//	make_nova_lut();
	NinjakunLoadRoms();

	CZetInit2(2,CZ80Context);
	CZetOpen(0);
	CZetSetReadHandler(ninjakun_main_read);
	CZetSetWriteHandler(ninjakun_main_write);
//	CZetMapMemory(DrvMainROM,	0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory(DrvMainROM,		0x0000, 0x1fff, MAP_ROM);
	CZetMapMemory(DrvMainROM + 0x2000,	0x2000, 0x7fff, MAP_ROM);

//	CZetMapArea(0xc000, 0xc7ff, 0, DrvFgRAM);
	CZetMapMemory(DrvFgRAM,		0xc000, 0xc7ff, MAP_ROM);
	CZetMapMemory(DrvSprRAM,	0xd000, 0xd7ff, MAP_RAM);
//	CZetMapArea(0xd800, 0xd9ff, 0, DrvPalRAM);
	CZetMapMemory(DrvPalRAM,	0xd800, 0xd9ff, MAP_ROM);
	CZetMapMemory(DrvMainRAM,	0xe000, 0xe7ff, MAP_RAM);

	CZetClose();

#ifdef RAZE
 	z80_init_memmap();

	z80_add_read(0x0000, 0xffff, 1, (void *)&ninjakun_sub_read); 
	z80_add_write(0x0000, 0xffff, 1, (void *)&ninjakun_sub_write);

	z80_map_read  (0x0000, 0x1fff, DrvMainROM+0x20000);
	z80_map_fetch (0x0000, 0x1fff, DrvMainROM+0x20000);

	z80_map_read (0x2000, 0x7fff, DrvMainROM + 0x2000);
	z80_map_fetch(0x2000, 0x7fff, DrvMainROM + 0x2000);

 	z80_map_read(0xc000, 0xc7ff, DrvFgRAM);
	z80_map_write(0xc000, 0xc7ff, DrvFgRAM);

	z80_map_read  (0xc800, 0xcfff, DrvBgRAM);
	z80_map_write (0xc800, 0xcfff, DrvBgRAM); //ajout

	z80_map_read  (0xd000, 0xd7ff, DrvSprRAM);
	z80_map_write (0xd000, 0xd7ff, DrvSprRAM); //ajout

	z80_map_read  (0xd800, 0xd9ff, DrvPalRAM);
	z80_map_write (0xd800, 0xd9ff, DrvPalRAM); //ajout

	z80_map_read  (0xe000, 0xe3ff, DrvMainRAM + 0x0400);
	z80_map_write (0xe000, 0xe3ff, DrvMainRAM + 0x0400); //ajout

	z80_map_read  (0xe400, 0xe7ff, DrvMainRAM + 0x0000);
	z80_map_write (0xe400, 0xe7ff, DrvMainRAM + 0x0000); //ajout

	z80_end_memmap();
#else
//	ZetInit(1);
	CZetOpen(1);
	CZetSetReadHandler(ninjakun_sub_read);
	CZetSetWriteHandler(ninjakun_sub_write);

	CZetMapMemory(DrvMainROM + 0x20000,	0x0000, 0x1fff, MAP_ROM);
	CZetMapMemory(DrvMainROM + 0x2000,	0x2000, 0x7fff, MAP_ROM);
	CZetMapMemory(DrvFgRAM,				0xc000, 0xc7ff, MAP_ROM);
	CZetMapMemory(DrvSprRAM,			0xd000, 0xd7ff, MAP_RAM);
	CZetMapMemory(DrvPalRAM,			0xd800, 0xd9ff, MAP_ROM);
	CZetMapMemory(DrvMainRAM+ 0x0400,	0xe000, 0xe3ff, MAP_RAM);
	CZetMapMemory(DrvMainRAM,			0xe400, 0xe7ff, MAP_RAM);
	CZetClose();
#endif

	AY8910Init(0, 3000000, nBurnSoundRate, &nova2001_port_3, &nova2001_port_4, NULL, NULL);
	AY8910Init(1, 3000000, nBurnSoundRate, NULL, NULL, &nova2001_scroll_x_w, &nova2001_scroll_y_w);

	NinjakunDoReset();
	
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/* INT32 Raiders5Init()
{
	DrvInitSaturn(2);
	make_lut();
	MemIndex();

	DrvSprRAM = DrvMainROM + 0x8000;
	DrvFgRAM = DrvMainROM + 0x8800;
	DrvBgRAM = DrvMainROM + 0x9000;
	DrvPalRAM = DrvMainROM + 0xd000;
	DrvMainRAM = DrvMainROM + 0xe000;
//	DrvSubROM = DrvMainROM += 0x010000;

	make_nova_lut();
	if (Raiders5LoadRoms()) return 1;

	CZetInit2(2,CZ80Context);
	CZetOpen(0);
	CZetSetInHandler(raiders5_in);
	CZetSetReadHandler(raiders5_main_read);
	CZetSetWriteHandler(raiders5_main_write);

	CZetMapArea(0x0000, 0x7fff, 0, DrvMainROM);
	CZetMapArea(0x0000, 0x7fff, 2, DrvMainROM);

	CZetMapArea(0x8000, 0x87ff, 0, DrvSprRAM);
	CZetMapArea(0x8000, 0x87ff, 1, DrvSprRAM);

	CZetMapArea(0x8800, 0x8fff, 0, DrvFgRAM);
//	CZetMapArea(0x8800, 0x8fff, 1, DrvFgRAM);

	CZetMapArea(0x9000, 0x97ff, 0, DrvBgRAM);
//	CZetMapArea(0x9000, 0x97ff, 1, DrvBgRAM);

	CZetMapArea(0xd000, 0xd1ff, 0, DrvPalRAM);
//	CZetMapArea(0xd000, 0xd1ff, 1, DrvPalRAM);

	CZetMapArea(0xe000, 0xe7ff, 0, DrvMainRAM);
	CZetMapArea(0xe000, 0xe7ff, 1, DrvMainRAM);

	CZetClose();

#ifdef RAZE
 	z80_init_memmap();
	z80_set_in((unsigned char (*)(unsigned short))&raiders5_in);
	z80_add_read(0x0000, 0xffff, 1, (void *)&raiders5_sub_read); 
	z80_add_write(0x0000, 0xfff, 1, (void *)&raiders5_sub_write);

	z80_map_read (0x0000, 0x3fff, DrvMainROM+0x20000);
	z80_map_fetch(0x0000, 0x3fff, DrvMainROM+0x20000);

	z80_map_read  (0xa000, 0xa7ff, DrvMainRAM);
	z80_map_write (0xa000, 0xa7ff, DrvMainRAM);
	z80_map_fetch (0xa000, 0xa7ff, DrvMainRAM);

	z80_end_memmap();
#else
	CZetOpen(1);
	CZetSetInHandler(raiders5_in); // a verifier
	CZetSetReadHandler(raiders5_sub_read);
	CZetSetWriteHandler(raiders5_sub_write);

	CZetMapArea(0x0000, 0x3fff, 0, DrvMainROM+0x20000);
	CZetMapArea(0x0000, 0x3fff, 2, DrvMainROM+0x20000);

	CZetMapArea(0xa000, 0xa7ff, 0, DrvMainRAM);
	CZetMapArea(0xa000, 0xa7ff, 1, DrvMainRAM);

	CZetClose();
#endif

	AY8910Init(0, 1500000, nBurnSoundRate, &raiders5_port_0, &pkunwar_port_1, NULL, NULL);
	AY8910Init(1, 1500000, nBurnSoundRate, &nova2001_port_3, &nova2001_port_4, NULL, NULL);
	NinjakunDoReset();

	return 0;
} */
//-------------------------------------------------------------------------------------------------------------------------------------
inline void initLayers()
{
    Uint16	CycleTb[]={
		0x4eee, 0x1eee, //A0
		0xeeee, 0xeeee,	//A1
		0x2f6e,0xf5ee,   //B0
		0xeeee, 0xeeee  //B1
	};
 	SclConfig	scfg;

	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize   = SCL_PN1WORD;
	scfg.flip              = SCL_PN_12BIT; 
	scfg.platesize     = SCL_PL_SIZE_2X2; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.patnamecontrl =  0x000c;// VRAM B1 のオフセット 
	scfg.plate_addr[0] = SCL_VDP2_VRAM_A0;
	scfg.plate_addr[1] = (Uint32)0;
	scfg.plate_addr[2] = (Uint32)0;
	scfg.plate_addr[3] = (Uint32)0;	
	SCL_SetConfig(SCL_NBG1, &scfg);

	scfg.plate_addr[0] = SCL_VDP2_VRAM_B0;
	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.dispenbl 	   = ON;
	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
//	scfg.coltype 	   = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	       = SCL_OVER_0;
	scfg.plate_addr[0] = SCL_VDP2_VRAM_A1;
	scfg.plate_addr[1] = (Uint32)0;
	scfg.plate_addr[2] = (Uint32)0;
	scfg.plate_addr[3] = (Uint32)0;	
	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);

	memset(SCL_VDP2_VRAM_B0,0x00,0x2000);
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void initPosition()
{
	SCL_Open();
	ss_reg->n1_move_y =  (32<<16) ;
	ss_reg->n1_move_x =  (-8<<16) ;
	ss_reg->n2_move_x =   (-8);
	ss_reg->n2_move_y =  32 ;
	SCL_Close();
}

//-------------------------------------------------------------------------------------------------------------------------------------
inline void initNinjaKunColors()
{
	colAddr = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);	 // nbg2 bg
	SCL_AllocColRam(SCL_NBG2,ON);	 // correct
	SCL_AllocColRam(SCL_NBG3,OFF);
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void initNovaColors()
{
	colBgAddr = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	colAddr = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
	SCL_AllocColRam(SCL_NBG2,OFF);
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void initColors()
{
	colBgAddr = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	colAddr = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
	SCL_AllocColRam(SCL_NBG2,OFF);
}
//-------------------------------------------------------------------------------------------------------------------------------------
#ifdef PONY
/*
extern SclLineparam lp;

static void SCL_SetLineParamNBG0(SclLineparam *lp)
{
	Uint32	*addr;
	addr = &Scl_n_reg.lineaddr[0];
	*addr = (lp->line_addr >>1) & 0x0007ffff;
	SclProcess = 2; //obligatoire
}*/
//-------------------------------------------------------------------------------------------------------------------------------------
#endif	
//-------------------------------------------------------------------------------------------------------------------------------------
void DrvInitSaturn(INT32 i)
{
	SPR_InitSlaveSH();
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);
	nBurnSprites  = 67;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

//	ss_sprite       = (SprSpCmd *)SS_SPRIT;

//	SS_CACHE        = (Uint32 *)SCL_VDP2_VRAM_B1;

	SS_SET_S0PRIN(5);
	SS_SET_N0PRIN(7);
	SS_SET_N2PRIN(4);
	SS_SET_N1PRIN(6);
	initPosition();

//	cleanSprites();

	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));

	if (i == 0)
	{
		initColors();
		SCL_Open();
		ss_reg->n1_move_x =  (0<<16) ;
	}
	if (i == 1)
		initNovaColors();
//	if(i == 2)
//		initRaiders5Colors();
	if(i == 3)
		initNinjaKunColors();

	SCL_AllocColRam(SCL_NBG3,OFF);		
	SCL_AllocColRam(SCL_NBG0,OFF);		
	SCL_SetColRam(SCL_NBG0,8,8,palette);
	initLayers();

	initSprites(264-1,216-1,0,0,8,-32);
	drawWindow(0,192,192,2,62);
//	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
	
	MemIndex();
	
#ifdef PONY
	pcm1 = add_raw_pcm_buffer(0,SOUNDRATE,nBurnSoundLen*20);

	nSoundBuffer = (Sint16 *)(SNDRAM+(m68k_com->pcmCtrl[pcm1].hiAddrBits<<16) | m68k_com->pcmCtrl[pcm1].loAddrBits);

	frame_x = frame_y = 0;
	nBurnFunction = sdrv_stm_vblank_rq;	
#endif	
}
//-------------------------------------------------------------------------------------------------------------------------------------
int DrvExit()
{
//	NinjakunDoReset();
//	nBurnSprites=128;
//	cleanSprites();

//	AY8910Exit(1);
//	AY8910Exit(0);
#ifndef RAZE
//	CZetExit2();
#endif
//wait_vblank();
#ifdef PONY
//memset(nSoundBuffer,0x00,0x8000);
remove_raw_pcm_buffer(pcm1);
#endif	

	//cleanDATA();
//	cleanBSS();

//	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
	nSoundBufferPos=0;
	return 0;
}
//-------------------------------------------------------------------------------------------------
// Drawing Routines

void DrawChars(UINT8 priority, UINT16 *my_map)
{
	UINT8 *rom_ptr = (UINT8*)DrvMainROM+0x8800;
	UINT16 *m = (UINT16 *)offs_lut;

	for (UINT32 offs = 0;offs < 0x37f;offs++)
	{
		if (rom_ptr[0x400] & 0x08 || !priority)
		{
			unsigned int num,color;
			num = rom_ptr[0] | ((rom_ptr[0x400] & 7) << 8);
			color = 0x100 | (rom_ptr[0x400] & 0xf0);
			my_map[*m++] = (color>>4) <<12 | num;
		}
		else
		{
			my_map[*m++] = 32;
		}
		rom_ptr++;
	}
}
//-------------------------------------------------------------------------------------------------
inline void DrvDraw()
{ 
	DrawChars(0,(Uint16 *)SCL_VDP2_VRAM_B0);
//	SPR_RunSlaveSH((PARA_RTN*)DrawChars, 0);
	cleanSprites();

	SprSpCmd *ss_spritePtr;
	ss_spritePtr = &((SprSpCmd *)SS_SPRIT)[3];
	UINT8 *DrvBgRAMptr = (UINT8*)DrvBgRAM;
	
	for (UINT8 offs = 0;offs < 0x40;offs++)
	{
		int sy,num,flip;

		sy = DrvBgRAMptr[2];

		if (sy < 16 || sy > 215) 
		{	
			DrvBgRAMptr+=32;
//			ss_sprite[3+delta].charSize=0;
			continue;
		}
//		sy -= 32;
		flip  = (DrvBgRAMptr[0] & 0x03)<<4;
		num   = ((DrvBgRAMptr[0] & 0xfc) >> 2) + ((DrvBgRAMptr[3] & 7) << 6);

		ss_spritePtr->ax = DrvBgRAMptr[1];
		ss_spritePtr->ay = sy;
		ss_spritePtr->color      = DrvBgRAMptr[3] & 0xf0;
		ss_spritePtr->control    = ( JUMP_NEXT | FUNC_NORMALSP | flip);
		ss_spritePtr->drawMode   = ( COLOR_0 | ECD_DISABLE | COMPO_REP);
		ss_spritePtr->charSize   = 0x210;  //0x100 16*16
		ss_spritePtr->charAddr   = 0x220+(num<<4);
		ss_spritePtr++;
		DrvBgRAMptr+=32;		
	}
	DrawChars(1,(Uint16 *)SCL_VDP2_VRAM_A0);
}

#ifdef PONY
void NovaFrame_old();

void NovaFrame()
{
//	InitCD(); // si on lance juste pour pang
//	ChangeDir("PANG");  // si on lance juste pour pang
	pcm_stream_host(NovaFrame_old);
}

void NovaFrame_old()
#else
void NovaFrame()
#endif
{
	watchdog++;
 	SPR_RunSlaveSH((PARA_RTN*)updateSound, NULL);
	
	{
		memset (DrvInputs, 0xff, 3);

		for (UINT32 i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
			DrvInputs[2] ^= (DrvJoy3[i] & 1) << i;
		}

		// Nova 2001 - if the coin pulse is too long or too short, the game will reset.
		// It will also reset if coined up like 5 in a row really fast, but that isn't handled in the code below.
		if (DrvJoy3[0]) {
			DrvCoinHold = 4; // hold coin input for 3 frames - first one is ignored
			DrvCoinHoldframecnt = 0;
		}

		if (DrvCoinHold) {
			DrvCoinHold--;
			DrvInputs[2] = 0xFF; // clear coin input
			if (DrvCoinHoldframecnt)
				DrvInputs[2] = 0xFF ^ 1;
		}
		DrvCoinHoldframecnt++;
	}

	vblank = 0;

	UINT32 nCyclesTotal = 3000000 / 60 / nInterleave;

	CZetOpen(0);
	for (UINT32 i = 0; i < nInterleave; i++) 
	{
		CZetRun(nCyclesTotal);
		if (i == 240) 
		{
			CZetSetIRQLine(0, CZET_IRQSTATUS_AUTO);
			vblank = 0x80;
		}
	}
	CZetClose();

	ss_reg->n2_move_x =   xscroll-8;
	ss_reg->n2_move_y =  yscroll+32 ;
	nova_draw_sprites();

#ifdef PONY
	_spr2_transfercommand();
	SclProcess = 1;	
	frame_x++;
	
	 if(frame_x>=frame_y)
		wait_vblank();		
#endif	
}
/*
void DrvPalRAMUpdate()
{
	for (UINT32 i = 0; i < 16; i++) {
		if (i != 1) { // ??
			for (INT32 j = 0; j < 16; j++) {
				DrvPalRAM[0x200 + i + j * 16 + 0] = DrvPalRAM[i];
			}
		}
		DrvPalRAM[0x200 + i * 16 + 1] = DrvPalRAM[i];
	}

	for (UINT32 i = 0; i < 0x300; i++) 
	{
		UINT8 color = DrvPalRAM[i];
		colAddr[i] = colBgAddr[i] = cram_lut[color];
	}
}
*/


#ifdef PONY
void DrvFrame_old();

void DrvFrame()
{
	pcm_stream_host(DrvFrame_old);
}

void DrvFrame_old()
#else
void DrvFrame()
#endif
{
/*
	memset (DrvInputs, 0xff, 2);

	for (INT32 i = 0; i < 8; i++) {
		DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
		DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
	}	
*/	
	vblank = 0;
#ifdef RAZE
	z80_emulate(3000000 / 60);
	z80_raise_IRQ(0);
	z80_emulate(0);
	z80_lower_IRQ();
	z80_emulate(0);
#else
	CZetOpen(0);
	CZetRun(3000000 / 60);
	CZetRaiseIrq(0);
	CZetClose();
#endif

	SPR_RunSlaveSH((PARA_RTN*)updateSound, NULL);
	DrvDraw();
	if((*(Uint8 *)0xfffffe11 & 0x80) != 0x80)
		SPR_WaitEndSlaveSH();
#ifdef PONY
	_spr2_transfercommand();
	frame_x++;
	
	if(frame_x>=frame_y)
		wait_vblank();		
#endif	
}

inline void NinjakunDraw()
{
	ss_reg->n2_move_x =  xscroll - 8;
	ss_reg->n2_move_y =  yscroll + 32;//-64;
	nova_draw_sprites();
}

//0x100
//0x400
//0x400
#ifdef PONY
void NinjakunFrame_old();

void NinjakunFrame()
{
//	InitCD(); // si on lance juste pour pang
//	ChangeDir("PANG");  // si on lance juste pour pang
	pcm_stream_host(NinjakunFrame_old);
}

void NinjakunFrame_old()
#else
void NinjakunFrame()
#endif
{
	watchdog++;

	memset (DrvInputs, 0xff, 3);

	for (UINT32 i = 0; i < 8; i++) {
		DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
		DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
	}

	vblank = 0;
//	UINT32 nInterleave = 256;
	UINT32 nCyclesTotal = 1500000 / 60;
	UINT32 nCyclesSegment = nCyclesTotal / nInterleave;	
//	UINT32 nCyclesDone = 0;
	SPR_RunSlaveSH((PARA_RTN*)updateSound, NULL);
	
	for (UINT32 i = 0; i < nInterleave; i++) {
		CZetOpen(0);
		CZetRun(nCyclesSegment);
		INT32 sync_cycles = CZetTotalCycles();
		if (i == 250) {
			CZetRaiseIrq(0);
			CZetRun(100);
			CZetLowerIrq();
			vblank = 1;
		}
		CZetClose();

//		
//		updateSound();

#ifdef RAZE
//		ZetOpen(1);
		nCyclesDone = z80_emulate(sync_cycles-z80_get_cycles_elapsed());
//		nCyclesTotalZ+= nCyclesDone;
//		ZetRun(sync_cycles - ZetTotalCycles());
		if (i == 63 || i == 127 || i == 195 || i == 255) {
//			ZetSetIRQLine(0, CPU_IRQSTATUS_HOLD);
			z80_raise_IRQ(0);
			z80_emulate(100);
			z80_lower_IRQ();
			z80_emulate(1);
		}

//		ZetClose();
#else
		CZetOpen(1);
		CZetRun(sync_cycles - CZetTotalCycles());

		if (i == 63 || i == 127 || i == 195 || i == 255) {

			CZetRaiseIrq(0);
			CZetRun(100);
			CZetLowerIrq();
		}

		CZetClose();

#endif


	}
	NinjakunDraw();
	
#ifdef PONY
	_spr2_transfercommand();
	SclProcess = 1;
	frame_x++;
	
	 if(frame_x>=frame_y)
		wait_vblank();		
#endif	
}

void nova_draw_sprites()
{
	SprSpCmd *ss_spritePtr;	
	ss_spritePtr = &((SprSpCmd *)SS_SPRIT)[3];
	UINT8 *DrvSprRAMptr = (UINT8*)DrvSprRAM;
	
	for (UINT8 offs = 0; offs < 0x40; offs++)
	{
		UINT8 attr = DrvSprRAMptr[3];
		
		if (attr & 0x80)
		{
//			ss_spritePtr->charSize   = 0;
			ss_spritePtr->charAddr   = 0;
			ss_spritePtr->ax   = 0;
			ss_spritePtr->ay   = 0;
			ss_spritePtr++;
			DrvSprRAMptr+=32;
			continue;
		}
		
		UINT8 code = DrvSprRAMptr[0];		
/*
		ss_spritePtr->ax = DrvSprRAM[offs+1] - ((attr & 0x40) << 2);
		ss_spritePtr->ay = DrvSprRAM[offs+2];
*/
		ss_spritePtr->ax = DrvSprRAMptr[1] - ((attr & 0x40) << 2);
		ss_spritePtr->ay = DrvSprRAMptr[2];
		ss_spritePtr->color      = attr & 0x0f;
		ss_spritePtr->control    = ( JUMP_NEXT | FUNC_NORMALSP | (attr & 0x30));
		ss_spritePtr->drawMode   = ( COLOR_0 | ECD_DISABLE | COMPO_REP);
		ss_spritePtr->charSize   = 0x210;  //0x100 16*16
		ss_spritePtr->charAddr   = 0x220+(code<<4);
		ss_spritePtr++;
		DrvSprRAMptr+=32;
	}
}

//-------------------------------------------------------------------------------------------------
void updateSound()
{
	int nSample;
	unsigned int deltaSlave;//soundLenSlave;//,titiSlave;
	deltaSlave    = *(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);

#ifndef PONY
	signed short *nSoundBuffer = (signed short *)0x25a20000+deltaSlave;	
	
	AY8910Update(0, &pAY8910Buffer[0], nBurnSoundLen);
	AY8910Update(1, &pAY8910Buffer[3], nBurnSoundLen);

	for (UINT16 n = 0; n < nBurnSoundLen; n++) 
	{
		nSample  = pAY8910Buffer[0][n]; // >> 2;
		nSample += pAY8910Buffer[1][n]; // >> 2;
		nSample += pAY8910Buffer[2][n]; // >> 2;
		nSample += pAY8910Buffer[3][n]; // >> 2;
		nSample += pAY8910Buffer[4][n]; // >> 2;
		nSample += pAY8910Buffer[5][n]; // >> 2;
		nSample /=4;
		
		*nSoundBuffer++ = BURN_SND_CLIP(nSample);//pAY8910Buffer[5][n];//nSample;
	}

	if(deltaSlave>=RING_BUF_SIZE/2)
	{
		PCM_NotifyWriteSize(pcm, deltaSlave);
		PCM_Task(pcm); // bon emplacement
		deltaSlave=0;
	}

	deltaSlave+=nBurnSoundLen;
#else
	AY8910Update(0, &pAY8910Buffer[0], nBurnSoundLen);
	AY8910Update(1, &pAY8910Buffer[3], nBurnSoundLen);	

	signed short *nSoundBuffer2 = (signed short *)nSoundBuffer+deltaSlave;

	for (UINT16 n = 0; n < nBurnSoundLen; n++) 
	{
		nSample  = pAY8910Buffer[0][n]; // >> 2;
		nSample += pAY8910Buffer[1][n]; // >> 2;
		nSample += pAY8910Buffer[2][n]; // >> 2;
		nSample += pAY8910Buffer[3][n]; // >> 2;
		nSample += pAY8910Buffer[4][n]; // >> 2;
		nSample += pAY8910Buffer[5][n]; // >> 2;
		nSample /=4;
		
		*nSoundBuffer2++ = BURN_SND_CLIP(nSample);//pAY8910Buffer[5][n];//nSample;
	}
	
	deltaSlave+=(nBurnSoundLen*2);
	
	if(deltaSlave>=nBurnSoundLen*10)
	{
		pcm_play(pcm1, PCM_SEMI, 7);
		deltaSlave=0;
	}	
#endif
//	nSoundBufferPos = deltaSlave;
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = deltaSlave;
}
//-------------------------------------------------------------------------------------------------
inline void make_lut()
{
	for (UINT16 i = 0; i < 256; i++) 
	{
		UINT32 intensity = i & 0x03;
//		INT32 shift = ((i & 0x0f) == 1) ? 4 : 0;
//		INT32 entry = ((i >> shift) & 0xf) | ((i & 0x100) >> 4);

		UINT32 r = (((i >> 0) & 0x0c) | intensity) * 0x11;
		UINT32 g = (((i >> 2) & 0x0c) | intensity) * 0x11;
		UINT32 b = (((i >> 4) & 0x0c) | intensity) * 0x11;

		cram_lut[i] = BurnHighCol(r, g, b, 0);
	}

	for (UINT16 i = 0; i < 0x400; i++) 
	{
		offs_lut[i]=(i & 0x1f) | (i / 0x20) <<6;;
	}	
}
//-------------------------------------------------------------------------------------------------

