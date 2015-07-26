// FB Alpha Penguin-Kun Wars Driver Module
// Based on MAME Driver by David Haywood and Phil Stroffolino
#define CZ80 1
#define RAZE 1  // `EMULATE_R_REGISTER obligatoire

#include "d_pkunwar.h"

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvpkunwar = {
		"pkunw", NULL,
		"Penguin-Kun Wars (US)\0",
		pkunwarRomInfo, pkunwarRomName, DrvInputInfo, DrvDIPInfo,
		DrvInit, DrvExit, DrvFrame, NULL//, DrvDraw//, NULL
	};

	struct BurnDriver nBurnDrvNova2001u = {
		"nova2001", "pkunw",
		"Nova 2001 (US)\0",
		nova2001uRomInfo, nova2001uRomName, Nova2001InputInfo, Nova2001DIPInfo,
		NovaInit, DrvExit, NovaFrame, NovaDraw
	};

	struct BurnDriver nBurnDrvNinjakun = {
		"ninjakun", "pkunw",
		"Ninjakun Majou no Bouken\0",
		ninjakunRomInfo, ninjakunRomName, NinjakunInputInfo, NinjakunDIPInfo,
		NinjakunInit, DrvExit, NinjakunFrame, NinjakunDraw
	};

	struct BurnDriver nBurnDrvRaiders5 = {
		"raiders5", "pkunw",
		"Raiders5\0",
		raiders5RomInfo, raiders5RomName, Raiders5InputInfo, Raiders5DIPInfo,
		Raiders5Init, DrvExit, Raiders5Frame, Raiders5Draw
	};

	if (strcmp(nBurnDrvpkunwar.szShortName, szShortName) == 0)
		memcpy(shared,&nBurnDrvpkunwar,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvNova2001u.szShortName, szShortName) == 0)
		memcpy(shared,&nBurnDrvNova2001u,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvNinjakun.szShortName, szShortName) == 0)
		memcpy(shared,&nBurnDrvNinjakun,sizeof(struct BurnDriver));
	if (strcmp(nBurnDrvRaiders5.szShortName, szShortName) == 0)
		memcpy(shared,&nBurnDrvRaiders5,sizeof(struct BurnDriver));
	ss_reg = (SclNorscl *)SS_REG;
}

// Memory Handlers

static UINT8 __fastcall raiders5_main_read(UINT16 address)
{
	if(address >= 0x9000 && address <= 0x97ff)
	{
		return DrvBgRAM[(((address & 0x3ff) + (xscroll >> 3) + ((yscroll >> 3) << 5)) & 0x3ff) + (address & 0x400)];
	}

	if(address >= 0xd000 && address <= 0xd1ff)
	{
		return DrvPalRAM[address - 0xd000];
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

static void DrvPalRAMUpdateR5()
{
	for (INT32 i = 0; i < 0x300; i++) {
		INT32 intensity = DrvPalRAM[i] & 0x03;

		INT32 r = (((DrvPalRAM[i] >> 0) & 0x0c) | intensity) * 0x11;
		INT32 g = (((DrvPalRAM[i] >> 2) & 0x0c) | intensity) * 0x11;
		INT32 b = (((DrvPalRAM[i] >> 4) & 0x0c) | intensity) * 0x11;

//		DrvPalette[i] = BurnHighCol(r, g, b, 0);
        r =  (r >>3);
        g =  (g >>3);
        b =  (b >>3);

		colAddr[i] = colBgAddr[i] = RGB(r,g,b);
	}
}

static void __fastcall raiders5_main_write(UINT16 address, UINT8 data)
{
	if(address >= 0x9000 && address <= 0x97ff)
	{
		DrvBgRAM[(((address & 0x3ff) + (xscroll >> 3) + ((yscroll >> 3) << 5)) & 0x3ff) + (address & 0x400)] = data;
		return;
	}

	//ZetMapMemory(DrvPalRAM,			0xd000, 0xd1ff, MAP_RAM);
	if(address >= 0xd000 && address <= 0xd1ff)
	{
		INT32 offset = address - 0xd000;
		DrvPalRAM[offset] = data;
		if (offset < 16) {
			DrvPalRAM[0x200 + offset * 16 + 1] = data;

			if (offset != 1) {
				for (INT32 i = 0; i < 16; i++) {
					DrvPalRAM[0x200 + offset + i * 16] = data;
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

static UINT8 __fastcall raiders5_sub_read(UINT16 address)
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

static void __fastcall raiders5_sub_write(UINT16 address, UINT8 data)
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

/*static*/ unsigned char __fastcall pkunwar_read(unsigned short address)
{
	switch (address)
	{
		case 0xa001:
			return AY8910Read(0);
		break;

		case 0xa003:
			return AY8910Read(1);
		break;
	}

	return 0;
}

/*static*/ void __fastcall pkunwar_write(unsigned short address, unsigned char data)
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

/*static*/ void __fastcall pkunwar_out(unsigned short address, unsigned char data)
{
	address &= 0xff;

//	if (address == 0) flipscreen = data & 1;
}

static UINT8 __fastcall nova2001_read(UINT16 address)
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

static void __fastcall nova2001_write(UINT16 address, UINT8 data)
{
	if(address>=0xa800 && address<=0xafff)
	{
		if(DrvMainROM[address]!=data)
		{
			DrvMainROM[address]=data;
			bg_line(address &0x3ff,DrvBgRAM);
		}
		return;
	}

	if(address>=0xa000 && address<=0xa7ff)
	{
		if(DrvMainROM[address]!=data)
		{
			DrvMainROM[address]=data;
			fg_line(address &0x3ff,DrvFgRAM);
		}
		return;
	}

	switch (address)
	{
		case 0xbfff:
			flipscreen = ~data & 0x01;
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

static UINT8 __fastcall ninjakun_main_read(UINT16 address)
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

static void __fastcall ninjakun_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xc800) {
		DrvBgRAM[(((address & 0x3ff) + (xscroll >> 3) + ((yscroll >> 3) << 5)) & 0x3ff) + (address & 0x400)] = data;
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
			flipscreen = ~data & 0x01;
			break;
	}
}

static UINT8 __fastcall ninjakun_sub_read(UINT16 address)
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

static void __fastcall ninjakun_sub_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xc800) {
		DrvBgRAM[(((address & 0x3ff) + (xscroll >> 3) + ((yscroll >> 3) << 5)) & 0x3ff) + (address & 0x400)] = data;
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


/*static*/ unsigned char pkunwar_port_0(unsigned int a)
{
	unsigned char ret = 0x7f | (vblank ^= 0x80);
	unsigned int i;
	for (i = 0; i < 8; i++) {
		ret ^= DrvJoy1[i] << i;
	}

	return ret;
}

/*static*/ unsigned char pkunwar_port_1(unsigned int a)
{
	unsigned char ret = 0xff;
	unsigned int i;
	for (i = 0; i < 8; i++) {
		ret ^= DrvJoy2[i] << i;
	}

	return ret;
}

/*static*/ unsigned char pkunwar_port_2(unsigned int a)
{
	return 0xff;
}

/*static*/ unsigned char pkunwar_port_3(unsigned int a)
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

static UINT8 nova2001_port_3(UINT32 data)
{
	return DrvDips[0];
}

static UINT8 nova2001_port_4(UINT32 data)
{
	return DrvDips[1];
}

static UINT8 raiders5_port_0(UINT32 data)
{
	return (DrvInputs[0] & 0x7f) | (vblank ? 0 : 0x80);
}
//-------------------------------------------------------------------------------------------------
// Initialization Routines


/*static*/ int DrvDoReset()
{
	DrvReset = 0;

	memset (DrvBgRAM, 0, 0x1000);
	memset (DrvMainRAM, 0, 0x0800);

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

	flipscreen = 0;
	watchdog = 0;

	xscroll = 0;
	yscroll = 0;
	return 0;
}

/*static*/ void pkunwar_palette_init()
{
	unsigned int i;
	for (i = 0; i < 0x200; i++)
	{
		int entry;
		int intensity,r,g,b;

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

        r =  (r >>3);
        g =  (g >>3);
        b =  (b >>3);

		colAddr[i] = colBgAddr[i] = RGB(r,g,b);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*inline*/ /*static*/ int readbit(const UINT8 *src, int bitnum)
{
	return src[bitnum / 8] & (0x80 >> (bitnum % 8));
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void GfxDecode4Bpp(int num, int numPlanes, int xSize, int ySize, int planeoffsets[], int xoffsets[], int yoffsets[], int modulo, unsigned char *pSrc, unsigned char *pDest)
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
		}
	}
}

/*static*/ void pkunwar_gfx_decode(unsigned char *Gfx)
{
	/*static*/ int PlaneOffsets[4] = { 0, 1, 2, 3 };

	/*static*/ int XOffsets[16] = {
		0x00000, 0x00004, 0x40000, 0x40004, 0x00008, 0x0000c, 0x40008, 0x4000c,
		0x00080, 0x00084, 0x40080, 0x40084, 0x00088, 0x0008c, 0x40088, 0x4008c
	};

	/*static*/ int YOffsets[16] = {
		0x000, 0x010, 0x020, 0x030, 0x040, 0x050, 0x060, 0x070,
		0x100, 0x110, 0x120, 0x130, 0x140, 0x150, 0x160, 0x170
	};
	unsigned long i;

	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	GfxDecode4Bpp(0x800, 4,  8,  8, PlaneOffsets, XOffsets, YOffsets, 0x80, Gfx, cache);
	GfxDecode4Bpp(0x200, 4, 16, 16, PlaneOffsets, XOffsets, YOffsets, 0x200, Gfx, &ss_vram[0x1100]);
}

#define STEP2(start, step)	((start) + ((step)*0)), ((start) + ((step)*1))
#define STEP4(start, step)	STEP2(start, step),  STEP2((start)+((step)*2), step)
#define STEP8(start, step)	STEP4(start, step),  STEP4((start)+((step)*4), step)

/*static*/ void nova_gfx_decode(unsigned char *Gfx)
{
	INT32 Planes[4]    = { STEP4(0,1) };
	INT32 XOffsets[16] = { STEP8(0,4), STEP8(256,4) };
	INT32 YOffsets[16] = { STEP8(0,32), STEP8(512,32) };

	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
	GfxDecode4Bpp(0x800, 4,  8,  8, Planes, XOffsets, YOffsets, 0x100, Gfx, cache);
	GfxDecode4Bpp(0x200, 4, 16, 16, Planes, XOffsets, YOffsets, 0x400, Gfx, &ss_vram[0x1100]);
}

/*static*/ int LoadRoms()
{
	UINT8 *tmp = (UINT8*)0x00200000;
	memset(tmp,0x00,0x20000);

	if (BurnLoadRom(DrvMainROM  + 0x0000, 0, 1)) return 1;
	if (BurnLoadRom(DrvMainROM  + 0x4000, 1, 1)) return 1;
	if (BurnLoadRom(DrvMainROM  + 0xe000, 2, 1)) return 1;

	if (BurnLoadRom(DrvGfxROM0 + 0x0000, 3, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM0 + 0x4000, 4, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM0 + 0x8000, 5, 1)) return 1;
	if (BurnLoadRom(DrvGfxROM0 + 0xc000, 6, 1)) return 1;

	for (INT32 i = 0; i < 8; i++) {
		int j = ((i & 1) << 2) | ((i >> 1) & 3);
		memcpy (tmp + j * 0x2000, DrvGfxROM0 + i * 0x2000, 0x2000);
	}

	pkunwar_gfx_decode(tmp);

	if (BurnLoadRom(DrvColPROM +  0x0000, 7, 1)) return 1;

	pkunwar_palette_init();
	memset(tmp,0x00,0x20000);
	tmp = NULL;

	return 0;
}

/*static*/ int NovaLoadRoms()
{
	UINT8 *tmp = (UINT8*)0x00240000;
	memset(tmp,0x00,0x20000);
	if (BurnLoadRom(DrvMainROM + 0x0000, 0, 1)) return 1;
	if (BurnLoadRom(DrvMainROM + 0x2000, 1, 1)) return 1;
	if (BurnLoadRom(DrvMainROM + 0x4000, 2, 1)) return 1;
	if (BurnLoadRom(DrvMainROM + 0x6000, 3, 1)) return 1;
	if (BurnLoadRom(DrvMainROM + 0x7000, 3, 1)) return 1;

	if (BurnLoadRom(tmp + 0x0000, 4, 2)) return 1;
	if (BurnLoadRom(tmp + 0x0001, 5, 2)) return 1;
	if (BurnLoadRom(tmp + 0x4000, 6, 2)) return 1;
	if (BurnLoadRom(tmp + 0x4001, 7, 2)) return 1;

	nova_gfx_decode(tmp);

	if (BurnLoadRom(DrvColPROM, 8, 1)) return 1;

	pkunwar_palette_init();
	memset(tmp,0x00,0x20000);
	tmp = NULL;

	return 0;
}

/*static*/ int NinjakunLoadRoms()
{
	UINT8 *tmp = (UINT8*)0x00240000;
	memset(tmp,0x00,0x20000);

	if (BurnLoadRom(DrvMainROM + 0x0000,  0, 1)) return 1;
	if (BurnLoadRom(DrvMainROM + 0x2000,  1, 1)) return 1;
	if (BurnLoadRom(DrvMainROM + 0x4000,  2, 1)) return 1;
	if (BurnLoadRom(DrvMainROM + 0x6000,  3, 1)) return 1;

	if (BurnLoadRom(DrvSubROM  + 0x0000,  4, 1)) return 1;

	if (BurnLoadRom(tmp + 0x0000,  5, 2)) return 1;
	if (BurnLoadRom(tmp + 0x0001,  6, 2)) return 1;
	if (BurnLoadRom(tmp + 0x4000,  7, 2)) return 1;
	if (BurnLoadRom(tmp + 0x4001,  8, 2)) return 1;

	if (BurnLoadRom(tmp +0x8000 + 0x0000,  9, 2)) return 1;
	if (BurnLoadRom(tmp +0x8000 + 0x0001, 10, 2)) return 1;
	if (BurnLoadRom(tmp +0x8000 + 0x4000, 11, 2)) return 1;
	if (BurnLoadRom(tmp +0x8000 + 0x4001, 12, 2)) return 1;

	nova_gfx_decode(tmp);
	tmp = NULL;

	return 0;
}


/*static*/ int Raiders5LoadRoms()
{
	UINT8 *tmp = (UINT8*)0x00240000;
	memset(tmp,0x00,0x20000);
	if (BurnLoadRom(DrvMainROM + 0x0000,  0, 1)) return 1;
	if (BurnLoadRom(DrvMainROM + 0x4000,  1, 1)) return 1;
	if (BurnLoadRom(DrvSubROM  + 0x0000,  2, 1)) return 1;
	if (BurnLoadRom(tmp + 0x0000,  3, 1)) return 1;
	if (BurnLoadRom(tmp + 0x4000,  4, 1)) return 1;
	if (BurnLoadRom(tmp + 0x8000,  5, 1)) return 1;

	nova_gfx_decode(tmp);
	tmp = NULL;
/*
	DrvGfxDescramble(DrvGfxROM0);
	DrvGfxDescramble(DrvGfxROM2);
	DrvGfxDecode(DrvGfxROM0, DrvGfxROM1, 1);
	DrvGfxDecode(DrvGfxROM0, DrvGfxROM0, 0);

	DrvGfxDecode(DrvGfxROM2, DrvGfxROM2, 0);
*/
	return 0;
}

static INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;
	DrvMainROM	 = Next; Next += 0x010000;
	DrvSubROM	= Next; Next += 0x020000;
	DrvGfxROM0	= Next; Next += 0x030000;
	DrvColPROM= Next; Next += 0x000020;
	pFMBuffer	= Next; Next += SOUND_LEN * 6 * sizeof(short);
	MemEnd	= Next;
}

/*static*/ int DrvInit()
{
	DrvInitSaturn(0);

	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)malloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();
	DrvBgRAM = DrvMainROM + 0x8000;
	DrvMainRAM = DrvMainROM + 0xc000;

	if (LoadRoms()) return 1;

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
	z80_set_out((void (*)(unsigned short int, unsigned char))&pkunwar_out);
#else
	CZetInit(1);
	CZetOpen(0);
	CZetSetOutHandler(pkunwar_out);
	CZetSetReadHandler(pkunwar_read);
	CZetSetWriteHandler(pkunwar_write);
	CZetMapArea(0x0000, 0x7fff, 0, DrvMainROM + 0x00000);
	CZetMapArea(0x0000, 0x7fff, 2, DrvMainROM + 0x00000);
	CZetMapArea(0x8000, 0x8fff, 0, DrvMainROM + 0x08000);
	CZetMapArea(0x8000, 0x8fff, 1, DrvMainROM + 0x08000);
	CZetMapArea(0xc000, 0xc7ff, 0, DrvMainROM + 0x0c000);
	CZetMapArea(0xc000, 0xc7ff, 1, DrvMainROM + 0x0c000);
	CZetMapArea(0xc000, 0xc7ff, 2, DrvMainROM + 0x0c000);
	CZetMapArea(0xe000, 0xffff, 0, DrvMainROM + 0x0e000);
	CZetMapArea(0xe000, 0xffff, 2, DrvMainROM + 0x0e000);
	CZetMemEnd();
	CZetClose();
#endif
	pAY8910Buffer[0] = pFMBuffer + SOUND_LEN * 0;
	pAY8910Buffer[1] = pFMBuffer + SOUND_LEN * 1;
	pAY8910Buffer[2] = pFMBuffer + SOUND_LEN * 2;
	pAY8910Buffer[3] = pFMBuffer + SOUND_LEN * 3;
	pAY8910Buffer[4] = pFMBuffer + SOUND_LEN * 4;
	pAY8910Buffer[5] = pFMBuffer + SOUND_LEN * 5;

	AY8910Init(0, 1500000, nBurnSoundRate, &pkunwar_port_0, &pkunwar_port_1, NULL, NULL);
	AY8910Init(1, 1500000, nBurnSoundRate, &pkunwar_port_2, &pkunwar_port_3, NULL, NULL);
	DrvDoReset();

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 NovaInit()
{
	DrvInitSaturn(1);
	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)malloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();
	DrvFgRAM = DrvMainROM + 0xa000;
	DrvBgRAM = DrvMainROM + 0xa800;
	DrvSprRAM = DrvMainROM + 0xb000;
	DrvMainRAM = DrvMainROM + 0xe000;

	if (NovaLoadRoms()) return 1;

	CZetInit(1);
	CZetOpen(0);
	CZetSetReadHandler(nova2001_read);
	CZetSetWriteHandler(nova2001_write);

	CZetMapArea(0x0000, 0x7fff, 0, DrvMainROM);
	CZetMapArea(0x0000, 0x7fff, 2, DrvMainROM);

	CZetMapArea(0xa000, 0xa7ff, 0, DrvFgRAM);
//	CZetMapArea(0xa000, 0xa7ff, 1, DrvFgRAM);

	CZetMapArea(0xa800, 0xafff, 0, DrvBgRAM);
//	CZetMapArea(0xa800, 0xafff, 1, DrvBgRAM); // write

	CZetMapArea(0xb000, 0xb7ff, 0, DrvSprRAM);
	CZetMapArea(0xb000, 0xb7ff, 1, DrvSprRAM);

	CZetMapArea(0xe000, 0xe7ff, 0, DrvMainRAM);
	CZetMapArea(0xe000, 0xe7ff, 1, DrvMainRAM);
	CZetClose();

	pAY8910Buffer[0] = pFMBuffer + SOUND_LEN * 0;
	pAY8910Buffer[1] = pFMBuffer + SOUND_LEN * 1;
	pAY8910Buffer[2] = pFMBuffer + SOUND_LEN * 2;
	pAY8910Buffer[3] = pFMBuffer + SOUND_LEN * 3;
	pAY8910Buffer[4] = pFMBuffer + SOUND_LEN * 4;
	pAY8910Buffer[5] = pFMBuffer + SOUND_LEN * 5;

    AY8910Init(0, 2000000, nBurnSoundRate, NULL, NULL, &nova2001_scroll_x_w, &nova2001_scroll_y_w);
    AY8910Init(1, 2000000, nBurnSoundRate, &nova2001_port_3, &nova2001_port_4, NULL, NULL);

	DrvDoReset();

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 NinjakunDoReset()
{
	DrvDoReset();

#ifdef RAZE
	z80_reset();
#else
	CZetOpen(1);
	CZetReset();
	CZetClose();
#endif

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 NinjakunInit()
{
	DrvInitSaturn(1);

	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)malloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();
	DrvFgRAM = DrvMainROM + 0xc000;
	DrvBgRAM = DrvMainROM + 0xc800;
	DrvSprRAM = DrvMainROM + 0xd000;
	DrvPalRAM = DrvMainROM + 0xd800;
	DrvMainRAM = DrvMainROM + 0xe000;
	DrvSubRAM = DrvMainROM += 0xe800;
	DrvSubROM = DrvMainROM += 0x010000;

	NinjakunLoadRoms();

	CZetInit(2);
	CZetOpen(0);
	CZetSetReadHandler(ninjakun_main_read);
	CZetSetWriteHandler(ninjakun_main_write);
 	CZetMapArea(0x0000, 0x7fff, 0, DrvMainROM);
	CZetMapArea(0x0000, 0x7fff, 2, DrvMainROM);

	CZetMapArea(0xc000, 0xc7ff, 0, DrvFgRAM);
	CZetMapArea(0xc000, 0xc7ff, 1, DrvFgRAM);

	CZetMapArea(0xc800, 0xcfff, 0, DrvBgRAM);
	CZetMapArea(0xc800, 0xcfff, 1, DrvBgRAM); // write

	CZetMapArea(0xd000, 0xd7ff, 0, DrvSprRAM);
	CZetMapArea(0xd000, 0xd7ff, 1, DrvSprRAM);

	CZetMapArea(0xd800, 0xd9ff, 0, DrvPalRAM);
	CZetMapArea(0xd800, 0xd9ff, 1, DrvPalRAM);

 	CZetMapArea(0xe000, 0xe7ff, 0, DrvMainRAM);
	CZetMapArea(0xe000, 0xe7ff, 1, DrvMainRAM);

	CZetClose();

#ifdef RAZE
 	z80_init_memmap();
	z80_add_read(0xe000, 0xe001, 1, (void *)&ninjakun_sub_read); 
	z80_add_write(0xa000, 0xa003, 1, (void *)&ninjakun_sub_write);
	z80_map_read  (0x0000, 0x1fff, DrvSubROM);
	z80_map_fetch (0x0000, 0x1fff, DrvSubROM);

	z80_map_read (0x2000, 0x7fff, DrvMainROM + 0x2000);
	z80_map_fetch(0x2000, 0x7fff, DrvMainROM + 0x2000);

	z80_map_read  (0xc800, 0xcfff, DrvBgRAM);
	z80_map_write (0xc800, 0xcfff, DrvBgRAM); //ajout
	z80_map_fetch (0xc800, 0xcfff, DrvBgRAM);

	z80_map_read  (0xd000, 0xd7ff, DrvSprRAM);
	z80_map_write (0xd000, 0xd7ff, DrvSprRAM); //ajout
	z80_map_fetch (0xd000, 0xd7ff, DrvSprRAM);

	z80_map_read  (0xd800, 0xd9ff, DrvPalRAM);
	z80_map_write (0xd800, 0xd9ff, DrvPalRAM); //ajout
	z80_map_fetch (0xd800, 0xd9ff, DrvPalRAM);

	z80_map_read  (0xe000, 0xe3ff, DrvMainRAM + 0x0400);
	z80_map_write (0xe000, 0xe3ff, DrvMainRAM + 0x0400); //ajout
	z80_map_fetch (0xe000, 0xe3ff, DrvMainRAM + 0x0400);

	z80_map_read  (0xe400, 0xe7ff, DrvMainRAM + 0x0000);
	z80_map_write (0xe400, 0xe7ff, DrvMainRAM + 0x0000); //ajout
	z80_map_fetch (0xe400, 0xe7ff, DrvMainRAM + 0x0000);

	z80_end_memmap();
#else
	
//	ZetInit(1);
	CZetOpen(1);
	CZetSetReadHandler(ninjakun_sub_read);
	CZetSetWriteHandler(ninjakun_sub_write);

	CZetMapArea(0x0000, 0x1fff, 0, DrvSubROM);
	CZetMapArea(0x0000, 0x1fff, 2, DrvSubROM);

	CZetMapArea(0x2000, 0x7fff, 0, DrvMainROM + 0x2000);
	CZetMapArea(0x2000, 0x7fff, 2, DrvMainROM + 0x2000);

	CZetMapArea(0xc000, 0xc7ff, 0, DrvFgRAM);
	CZetMapArea(0xc000, 0xc7ff, 1, DrvFgRAM);

	CZetMapArea(0xc800, 0xcfff, 0, DrvBgRAM);
	CZetMapArea(0xc800, 0xcfff, 1, DrvBgRAM); // write

	CZetMapArea(0xd000, 0xd7ff, 0, DrvSprRAM);
	CZetMapArea(0xd000, 0xd7ff, 1, DrvSprRAM);

	CZetMapArea(0xd800, 0xd9ff, 0, DrvPalRAM);
	CZetMapArea(0xd800, 0xd9ff, 1, DrvPalRAM);

 	CZetMapArea(0xe000, 0xe3ff, 0, DrvMainRAM+ 0x0400);
	CZetMapArea(0xe000, 0xe3ff, 1, DrvMainRAM+ 0x0400);

 	CZetMapArea(0xe400, 0xe7ff, 0, DrvMainRAM+ 0x0000);
	CZetMapArea(0xe400, 0xe7ff, 1, DrvMainRAM+ 0x0000);
	CZetClose();
#endif

	pAY8910Buffer[0] = pFMBuffer + SOUND_LEN * 0;
	pAY8910Buffer[1] = pFMBuffer + SOUND_LEN * 1;
	pAY8910Buffer[2] = pFMBuffer + SOUND_LEN * 2;
	pAY8910Buffer[3] = pFMBuffer + SOUND_LEN * 3;
	pAY8910Buffer[4] = pFMBuffer + SOUND_LEN * 4;
	pAY8910Buffer[5] = pFMBuffer + SOUND_LEN * 5;

	AY8910Init(0, 3000000, nBurnSoundRate, &nova2001_port_3, &nova2001_port_4, NULL, NULL);
	AY8910Init(1, 3000000, nBurnSoundRate, NULL, NULL, &nova2001_scroll_x_w, &nova2001_scroll_y_w);

	NinjakunDoReset();

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static INT32 Raiders5Init()
{
	DrvInitSaturn(1);

	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	if ((AllMem = (UINT8 *)malloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();

	DrvSprRAM = DrvMainROM + 0x8000;
	DrvFgRAM = DrvMainROM + 0x8800;
	DrvBgRAM = DrvMainROM + 0x9000;
	DrvPalRAM = DrvMainROM + 0xd000;
	DrvMainRAM = DrvMainROM + 0xe000;
	DrvSubROM = DrvMainROM += 0x010000;

	if (Raiders5LoadRoms()) return 1;

	CZetInit(2);
	CZetOpen(0);
	CZetSetInHandler(raiders5_in);
	CZetSetReadHandler(raiders5_main_read);
	CZetSetWriteHandler(raiders5_main_write);

	CZetMapArea(0x0000, 0x7fff, 0, DrvMainROM);
	CZetMapArea(0x0000, 0x7fff, 2, DrvMainROM);

	CZetMapArea(0x8000, 0x87ff, 0, DrvSprRAM);
	CZetMapArea(0x8000, 0x87ff, 1, DrvSprRAM);

	CZetMapArea(0x8800, 0x8fff, 0, DrvFgRAM);
	CZetMapArea(0x8800, 0x8fff, 1, DrvFgRAM);

//	CZetMapArea(0x9000, 0x97ff, 0, DrvBgRAM);
//	CZetMapArea(0x9000, 0x97ff, 1, DrvBgRAM);

//	CZetMapArea(0xd000, 0xd1ff, 0, DrvPalRAM);
//	CZetMapArea(0xd000, 0xd1ff, 1, DrvPalRAM);

	CZetMapArea(0xe000, 0xe7ff, 0, DrvMainRAM);
	CZetMapArea(0xe000, 0xe7ff, 1, DrvMainRAM);

	CZetClose();

	CZetInit(1);
	CZetOpen(1);
	CZetSetInHandler(raiders5_in); // a verifier
	CZetSetReadHandler(raiders5_sub_read);
	CZetSetWriteHandler(raiders5_sub_write);

	CZetMapArea(0x0000, 0x3fff, 0, DrvSubROM);
	CZetMapArea(0x0000, 0x3fff, 2, DrvSubROM);

	CZetMapArea(0xa000, 0xa7ff, 0, DrvMainRAM);
	CZetMapArea(0xa000, 0xa7ff, 1, DrvMainRAM);

	CZetClose();

	pAY8910Buffer[0] = pFMBuffer + SOUND_LEN * 0;
	pAY8910Buffer[1] = pFMBuffer + SOUND_LEN * 1;
	pAY8910Buffer[2] = pFMBuffer + SOUND_LEN * 2;
	pAY8910Buffer[3] = pFMBuffer + SOUND_LEN * 3;
	pAY8910Buffer[4] = pFMBuffer + SOUND_LEN * 4;
	pAY8910Buffer[5] = pFMBuffer + SOUND_LEN * 5;

	AY8910Init(0, 1500000, nBurnSoundRate, &raiders5_port_0, &pkunwar_port_1, NULL, NULL);
	AY8910Init(1, 1500000, nBurnSoundRate, &nova2001_port_3, &nova2001_port_4, NULL, NULL);

	AY8910SetAllRoutes(0, 0.25, BURN_SND_ROUTE_BOTH);
	AY8910SetAllRoutes(1, 0.25, BURN_SND_ROUTE_BOTH);

//	GenericTilesInit();

	NinjakunDoReset();

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initLayers()
{
    Uint16	CycleTb[]={
		0x4eff, 0x1fff, //A0
		0xffff, 0xffff,	//A1
		0x2f6f,0xf5ff,   //B0
		0xffff, 0xffff  //B1
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
	SCL_SetConfig(SCL_NBG1, &scfg);

	scfg.plate_addr[0] = SCL_VDP2_VRAM_B0;
	SCL_SetConfig(SCL_NBG2, &scfg);

	scfg.dispenbl 	   = ON;
	scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
//	scfg.coltype 	   = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 	   = SCL_BITMAP;
	scfg.mapover	       = SCL_OVER_0;
	scfg.plate_addr[0] = SCL_VDP2_VRAM_A1;
	SCL_SetConfig(SCL_NBG0, &scfg);

	SCL_SetCycleTable(CycleTb);

	memset(SCL_VDP2_VRAM_B0,0x00,0x2000);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initPosition()
{
	SCL_Open();
	ss_reg->n1_move_y =  (32<<16) ;
	ss_reg->n1_move_x =  (-8<<16) ;
	ss_reg->n2_move_x =   (-8);
	ss_reg->n2_move_y =  32 ;
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initNovaColors()
{
	colBgAddr = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	SCL_AllocColRam(SCL_NBG3,OFF);
	colAddr = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
//	SclColRamAlloc256[3]=SCL_NBG2;
// 	SCL_SetColRamOffset(SCL_NBG2,3,OFF);
	SCL_AllocColRam(SCL_NBG2,OFF);
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initColors()
{
	colBgAddr = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	colAddr = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
//	SclColRamAlloc256[3]=SCL_NBG2;
// 	SCL_SetColRamOffset(SCL_NBG2,3,OFF);
	SCL_AllocColRam(SCL_NBG2,OFF);
	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn(INT32 i)
{
	cleanSprites();
	SPR_InitSlaveSH();
	nBurnSprites  = 67;

//	ss_map  = (Uint16 *)SS_MAP;
//	ss_map2= (Uint16 *)SS_MAP2;
//	ss_font  = (Uint16 *)SS_FONT;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite           = (SprSpCmd *)SS_SPRIT;

	SS_CACHE      = cache = (Uint8 *)SCL_VDP2_VRAM_B1;

	SS_SET_S0PRIN(5);
	SS_SET_N0PRIN(7);
	SS_SET_N2PRIN(4);
	SS_SET_N1PRIN(6);
	if (i == 0)
		initColors();
	else
		initNovaColors();
	initLayers();
	initPosition();
	initSprites(264-1,216-1,0,0,8,-32);
	drawWindow(0,192,192,2,62);
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
	//*(unsigned int*)OPEN_CSH_VAR(SOUND_LEN) = 128;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void cleanSprites()
{
	unsigned int delta;	
	for (delta=3; delta<nBurnSprites; delta++)
	{
		ss_sprite[delta].charSize   = 0;
		ss_sprite[delta].charAddr   = 0;
		ss_sprite[delta].ax   = 0;
		ss_sprite[delta].ay   = 0;
	} 
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ int DrvExit()
{
#ifndef RAZE
	CZetExit();
#endif
	AY8910Exit(0);
	AY8910Exit(1);

	DrvMainROM = DrvGfxROM0 = DrvColPROM = DrvBgRAM = DrvMainRAM = NULL;
	nBurnSprites=128;
	cleanSprites();

	for (int i = 0; i < 6; i++) {
		pAY8910Buffer[i] = NULL;
	}

//	free (pFMBuffer);
	pFMBuffer = NULL;
	free (Mem);
	Mem = NULL;

	vblank /*= flipscreen */= 0;

	return 0;

}


//-------------------------------------------------------------------------------------------------
// Drawing Routines
/*static*/ int DrawChars(int priority)
{
	int offs;

	if(priority==0)
		ss_map =(Uint16 *)SCL_VDP2_VRAM_B0;
	else
		ss_map =(Uint16 *)SCL_VDP2_VRAM_A0;

	for (offs = 0x3c0 - 1;offs >= 0x40;offs--)
	{
		int sx,sy;
		sx = (offs & 0x1f);
		sy = ((offs >> 5) & 0x1f)<<6;

		if (DrvMainROM[0x8c00 + offs] & 0x08 || !priority)
		{
			int num,color;
			num = DrvMainROM[0x8800 + offs] | ((DrvMainROM[0x8c00 + offs] & 7) << 8);
			color = 0x100 | (DrvMainROM[0x8c00 + offs] & 0xf0);
			ss_map[sx|sy] = (color>>4) <<12 | num;
		}
		else
		{
			ss_map[sx|sy] = 32;
		}
	}
   
	return 0;
}

/*static*/ int DrvDraw()
{ 
	int offs;

	DrawChars(0);
//	SPR_RunSlaveSH((PARA_RTN*)DrawChars, 0);
	cleanSprites();

	for (offs = 0;offs < 0x800;offs += 32)
	{
		int sx,sy,num,color,flip;
		unsigned int delta;

		sx = DrvBgRAM[1 + offs];
		sy = DrvBgRAM[2 + offs];
		delta = (offs>>5)+3;

		if (sy < 16 || sy > 215) 
		{	
//			ss_sprite[3+delta].charSize=0;
			continue;
		}
//		sy -= 32;
		flip  = (DrvBgRAM[offs] & 0x03)<<4;
		num   = ((DrvBgRAM[offs] & 0xfc) >> 2) + ((DrvBgRAM[offs + 3] & 7) << 6);
		color = DrvBgRAM[offs + 3] & 0xf0;

		ss_sprite[delta].ax = sx;
		ss_sprite[delta].ay = sy;
		ss_sprite[delta].color      = color;
		ss_sprite[delta].control    = ( JUMP_NEXT | FUNC_NORMALSP | flip);
		ss_sprite[delta].drawMode   = ( COLOR_0 | ECD_DISABLE | COMPO_REP);
		ss_sprite[delta].charSize   = 0x210;  //0x100 16*16
		ss_sprite[delta].charAddr   = 0x220+(num<<4);
	}
	DrawChars(1);

	return 0;
}

static INT32 NovaFrame()
{
	if (DrvReset) {
		DrvDoReset();
	}
	watchdog++;

	{
		memset (DrvInputs, 0xff, 3);

		for (INT32 i = 0; i < 8; i++) {
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
//	INT32 nInterleave = 256;
	INT32 nInterleave = 32;
	INT32 nCyclesTotal = 3000000 / 60;

 	SPR_RunSlaveSH((PARA_RTN*)updateSound, NULL);

	CZetOpen(0);
	for (INT32 i = 0; i < nInterleave; i++) 
	{
		CZetRun(nCyclesTotal / nInterleave);
//		if (i == 240) 
		if (i == 30) 
		{
			CZetSetIRQLine(0, CZET_IRQSTATUS_AUTO);
			vblank = 0x80;
		}
	}
	CZetClose();

	ss_reg->n2_move_x =   xscroll-8;
	ss_reg->n2_move_y =  yscroll+32 ;
	nova_draw_sprites(0x000);

	SPR_WaitEndSlaveSH();
	return 0;
}

static void DrvPalRAMUpdate()
{
	for (INT32 i = 0; i < 16; i++) {
		if (i != 1) { // ??
			for (INT32 j = 0; j < 16; j++) {
				DrvPalRAM[0x200 + i + j * 16 + 0] = DrvPalRAM[i];
			}
		}
		DrvPalRAM[0x200 + i * 16 + 1] = DrvPalRAM[i];
	}

	for (INT32 i = 0; i < 0x300; i++) {
		INT32 intensity = DrvPalRAM[i] & 0x03;

		INT32 r = (((DrvPalRAM[i] >> 0) & 0x0c) | intensity) * 0x11;
		INT32 g = (((DrvPalRAM[i] >> 2) & 0x0c) | intensity) * 0x11;
		INT32 b = (((DrvPalRAM[i] >> 4) & 0x0c) | intensity) * 0x11;

//		DrvPalette[i] = BurnHighCol(r, g, b, 0);
        r =  (r >>3);
        g =  (g >>3);
        b =  (b >>3);

		colAddr[i] = colBgAddr[i] = RGB(r,g,b);
	}
}

static INT32 NinjakunDraw()
{
	DrvPalRAMUpdate();

//	BurnTransferClear();

//	if (nBurnLayer & 1) draw_layer(DrvBgRAM, DrvGfxROM2 + 0x0000, 2, 0x100, 0);

//	if (nBurnLayer & 2) draw_layer(DrvFgRAM, DrvGfxROM0 + 0x0000, 3, 0x000, 1);

//	if (nBurnLayer & 4) nova_draw_sprites(0x200);

//	if (nBurnLayer & 8) draw_layer(DrvFgRAM, DrvGfxROM0 + 0x0000, 3, 0x000, 0);

//	BurnTransferCopy(DrvPalette);

draw_layer(DrvFgRAM, (Uint16 *)SCL_VDP2_VRAM_A0, 3, 0x000, 0);
nova_draw_sprites(0x200);

	return 0;
}

static INT32 NinjakunFrame()
{
	if (DrvReset) {
		NinjakunDoReset();
	}
	watchdog++;

	memset (DrvInputs, 0xff, 3);

	for (INT32 i = 0; i < 8; i++) {
		DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
		DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
	}

	vblank = 0;
	INT32 nInterleave = 256;
	INT32 nCyclesTotal = 3000000 / 60;
	INT32 nCyclesDone = 0, nCyclesTotalZ =0;
	for (INT32 i = 0; i < nInterleave; i++) {
		CZetOpen(0);
		CZetRun(nCyclesTotal / nInterleave);
		INT32 sync_cycles = CZetTotalCycles();
		if (i == 250) {
//			ZetSetIRQLine(0, CPU_IRQSTATUS_HOLD);
			CZetSetIRQLine(0, 1);
			CZetRun(100);
			CZetSetIRQLine(0, 0);
			CZetRun(0);

			vblank = 1;
		}
		CZetClose();

#ifdef RAZE
//		ZetOpen(1);
		nCyclesDone = z80_emulate(nCyclesTotal / nInterleave);//sync_cycles - nCyclesTotalZ);
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
//			ZetSetIRQLine(0, CPU_IRQSTATUS_HOLD);
			CZetSetIRQLine(0, 1);
			CZetRun(100);
			CZetSetIRQLine(0, 0);
			CZetRun(0);

		}

		CZetClose();
#endif


	}

//	if (pBurnSoundOut) {
//		AY8910Render(&pAY8910Buffer[0], pBurnSoundOut, nBurnSoundLen, 0);
//	}
  	SPR_RunSlaveSH((PARA_RTN*)updateSound, NULL);
//	updateSound();
	NinjakunDraw();


//PCM_Task(pcm);
	SPR_WaitEndSlaveSH();
//	if (pBurnDraw) {
		
//	}

	return 0;
}

static INT32 Raiders5Frame()
{
	if (DrvReset) 
	{
		NinjakunDoReset();
	}
	watchdog++;

	memset (DrvInputs, 0xff, 2);

	for (INT32 i = 0; i < 8; i++) {
		DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
		DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
	}

	vblank = 0;
	//INT32 Multiplier = 8; // needs high multiplier for inter-processor communication w/shared memory
	INT32 nInterleave = 2000; //256*Multiplier;
	INT32 nCyclesTotal = 3000000 / 60;

	for (INT32 i = 0; i < nInterleave; i++) 
	{
		CZetOpen(0);
		CZetRun(nCyclesTotal / nInterleave);
		//INT32 sync_cycles = ZetTotalCycles();
		if (i == 1880) {
//			CZetSetIRQLine(0, CPU_IRQSTATUS_HOLD);
			CZetSetIRQLine(0, 1);
			CZetRun(100);
			CZetSetIRQLine(0, 0);
			CZetRun(0);

			vblank = 1;
		}
		CZetClose();

		CZetOpen(1);
		CZetRun(nCyclesTotal / nInterleave);//sync_cycles - ZetTotalCycles());
		if (i%(nInterleave/4) == (nInterleave/4)-10) {
//			ZetSetIRQLine(0, CPU_IRQSTATUS_HOLD);
			CZetSetIRQLine(0, 1);
			CZetRun(100);
			CZetSetIRQLine(0, 0);
			CZetRun(0);
		}
		CZetClose();
	}

//	if (pBurnSoundOut) {
//		AY8910Render(&pAY8910Buffer[0], pBurnSoundOut, nBurnSoundLen, 0);
//	}

  	SPR_RunSlaveSH((PARA_RTN*)updateSound, NULL);
//	updateSound();
	Raiders5Draw();


//PCM_Task(pcm);
	SPR_WaitEndSlaveSH();

	return 0;
}

static INT32 Raiders5Draw()
{
	DrvPalRAMUpdateR5();
//	draw_layer(DrvFgRAM, (Uint16 *)SCL_VDP2_VRAM_A0, 1, 0x000, 0);
//	draw_fglayer(DrvFgRAM, (Uint16 *)SCL_VDP2_VRAM_A0, 1, 0x000, 0);
//	draw_layer(DrvFgRAM, DrvGfxROM0 + 0x0000, 1, 0x000, 1);
//draw_bglayer(DrvBgRAM, (Uint16 *)SCL_VDP2_VRAM_B0, 0, 0x100, 0);
 
//	draw_layer(DrvBgRAM, DrvGfxROM2 + 0x0000, 6, 0x100, 0);
	draw_layer(DrvBgRAM, (Uint16 *)SCL_VDP2_VRAM_B0, 6, 0x100, 0);
//	pkunwar_draw_sprites(0x200, 0, 0x0f);

	cleanSprites();

	for (INT32 offs = 0;offs < 0x800;offs += 32)
	{
		int sx,sy,num,color,flip;
		unsigned int delta;

		sx = DrvBgRAM[1 + offs];
		sy = DrvBgRAM[2 + offs];
		delta = (offs>>5)+3;

		if (sy < 16 || sy > 215) 
		{	
//			ss_sprite[3+delta].charSize=0;
			continue;
		}
//		sy -= 32;
		flip  = (DrvBgRAM[offs] & 0x03)<<4;
		num   = ((DrvBgRAM[offs] & 0xfc) >> 2) + ((DrvBgRAM[offs + 3] & 7) << 6);
		color = DrvBgRAM[offs + 3] & 0x0f; //0xf0;

		ss_sprite[delta].ax = sx;
		ss_sprite[delta].ay = sy;
		ss_sprite[delta].color      = color;
		ss_sprite[delta].control    = ( JUMP_NEXT | FUNC_NORMALSP | flip);
		ss_sprite[delta].drawMode   = ( COLOR_0 | ECD_DISABLE | COMPO_REP);
		ss_sprite[delta].charSize   = 0x210;  //0x100 16*16
		ss_sprite[delta].charAddr   = 0x220+(num<<4);
	}








//	draw_layer(DrvFgRAM, DrvGfxROM0 + 0x0000, 7, 0x000, 0);
	draw_layer(DrvFgRAM, (Uint16 *)SCL_VDP2_VRAM_A0, 7, 0x000, 0);
	return 0;
}

/*static*/ void draw_layer(UINT8 *ram_base, UINT16 *gfx_base, INT32 config, INT32 color_base, INT32 priority)
{
	INT32 color_shift = 0;
	INT32 code_extend = -1;
	INT32 code_extend_shift = 0;
	INT32 group_select_bit = -1;
	INT32 transparent = 0xff; // opaque
	INT32 enable_scroll = 0;
	INT32 color_mask = 0x0f;
	INT32 xskew = 0;
 	ss_map = (Uint16 *)gfx_base;
	switch (config)
	{
		case 0: // nova2001 background
//			ss_map =(Uint16 *)SCL_VDP2_VRAM_B0;
			enable_scroll = 1;
		break;

		case 1: // nova2001 foreground
//			ss_map =(Uint16 *)SCL_VDP2_VRAM_A0;
			group_select_bit = 4;
			transparent = 0;
		break;

		case 2: // ninjakun background
			code_extend = 3;
			code_extend_shift = 6;
			enable_scroll = 1;
		break;

		case 3: // ninjakun foreground
			code_extend = 1;
			code_extend_shift = 5;
			transparent = 0;
		break;

		case 4: // pkunwar background
			code_extend = 7;
			code_extend_shift = 0;
			color_shift = 4;
			color_mask = 0xf0;
		break;

		case 5: // pkunwar foreground (background + transparency + group)
			code_extend = 7;
			code_extend_shift = 0;
			color_shift = 4;
			group_select_bit = 3;
			transparent = 0;
			color_mask = 0xf0;
		break;

		case 6: // raiders5 background   <--- something is wrong here(?) -dink
			code_extend = 1;
			code_extend_shift = 0;
			color_shift = 4;
			enable_scroll = 1;
			color_mask = 0x0f;
			xskew = 8;
		break;

		case 7: // raiders5 foreground
			color_shift = 4;
			transparent = 0;
			color_mask = 0xf0;
		break;
	}

	if (enable_scroll) 
	{
		ss_reg->n2_move_x =   xscroll-8;
		ss_reg->n2_move_y =  yscroll+32 ;
	}

	for (INT32 offs = 0; offs < 32 * 32; offs++)
	{
//		if(bg_dirty[offs])
		{
//			bg_dirty[offs]=0;
			INT32 sx = (offs & 0x1f);
			INT32 sy = (offs / 0x20) <<6;

			INT32 code = ram_base[offs + 0x000];
			INT32 attr = ram_base[offs + 0x400];

			INT32 color = (attr & color_mask) >> color_shift;

			INT32 group = 0;

			if (group_select_bit != -1) {
				group = (attr >> group_select_bit) & 1;

				if (group != priority) continue;
			}

			if (code_extend != -1) code |= ((attr >> code_extend_shift) & code_extend) << 8;
			if (config==6) {//dink
				code = ram_base[offs + 0x000] + ((attr & 0x01) << 8);
				color = (attr >> 4) & 0x0f;
			}
			if(config==0)
			{
				code+=0x200;
				ss_map[sx|sy+0X20] = ss_map[(sx|sy)+0X800] = ss_map[(sx|sy)+0X820] = (color) <<12 | code;
			}
			ss_map[sx|sy] = color <<12 | code;
		}
	}
}

static void fg_line(UINT16 offs,UINT8 *ram_base)
{
	Uint16 *ss_map = (Uint16 *)SCL_VDP2_VRAM_A0;
	INT32 sx = (offs & 0x1f);
	INT32 sy = (offs / 0x20) <<6;

	INT32 code = ram_base[offs + 0x000];
	INT32 attr = ram_base[offs + 0x400];

	INT32 color = (attr & 0x0f);
	INT32 group = (attr >> 4) & 1;

	if (group != 0) return;
	ss_map[sx|sy] = color <<12 | code;
}

static void bg_line(UINT16 offs,UINT8 *ram_base)
{
	Uint16 *ss_map = (Uint16 *)SCL_VDP2_VRAM_B0;
	INT32 sx = (offs & 0x1f);
	INT32 sy = (offs / 0x20) <<6;

	INT32 code = ram_base[offs + 0x000] + 0x200;
	INT32 attr = ram_base[offs + 0x400];
	INT32 color = (attr & 0x0f);

	ss_map[sx|sy+0X20] = ss_map[(sx|sy)+0X800] = ss_map[(sx|sy)+0X820] = (color) <<12 | code;
	ss_map[sx|sy] = color <<12 | code;
}

static void nova_draw_sprites(INT32 color_base)
{
	for (INT32 offs = 0; offs < 0x800; offs += 32)
	{
		INT32 attr = DrvSprRAM[offs+3];
		INT32 delta = (offs>>5)+3;
		INT32 code = DrvSprRAM[offs+0];

		if (attr & 0x80)
		{
			ss_sprite[delta].charSize   = 0;
			ss_sprite[delta].charAddr   = 0;
			ss_sprite[delta].ax   = 0;
			ss_sprite[delta].ay   = 0;
			continue;
		}

		ss_sprite[delta].ax = DrvSprRAM[offs+1] - ((attr & 0x40) << 2);
		ss_sprite[delta].ay = DrvSprRAM[offs+2];
		ss_sprite[delta].color      = attr & 0x0f;
		ss_sprite[delta].control    = ( JUMP_NEXT | FUNC_NORMALSP | (attr & 0x30));
		ss_sprite[delta].drawMode   = ( COLOR_0 | ECD_DISABLE | COMPO_REP);
		ss_sprite[delta].charSize   = 0x210;  //0x100 16*16
		ss_sprite[delta].charAddr   = 0x220+(code<<4);
	}
}

static INT32 NovaDraw()
{

//	draw_bglayer(DrvBgRAM, (Uint16 *)SCL_VDP2_VRAM_B0, 0, 0x100, 0);

//	nova_draw_sprites(0x000);

//	draw_layer(DrvFgRAM, (Uint16 *)SCL_VDP2_VRAM_A0, 1, 0x000, 0);
//	draw_fglayer(DrvFgRAM, (Uint16 *)SCL_VDP2_VRAM_A0, 1, 0x000, 0);
//	draw_layer(DrvFgRAM, DrvGfxROM0 + 0x0000, 1, 0x000, 1);

	return 0;
}

/*static*/ int DrvFrame()
{
	if (DrvReset) {
		DrvDoReset();
	}

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
//	updateSound();
	DrvDraw();


//PCM_Task(pcm);
	SPR_WaitEndSlaveSH();
	return 0;
}
//-------------------------------------------------------------------------------------------------
/*static*/ void updateSound()
{
	int nSample;
	int n;
	unsigned int deltaSlave;//soundLenSlave;//,titiSlave;
	signed short *nSoundBuffer = (signed short *)0x25a20000;
	deltaSlave    = *(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);

//	soundLenSlave = SOUND_LEN);
	AY8910Update(0, &pAY8910Buffer[0], SOUND_LEN);
	AY8910Update(1, &pAY8910Buffer[3], SOUND_LEN);

	for (n = 0; n < SOUND_LEN; n++) 
	{
		nSample  = pAY8910Buffer[0][n]; // >> 2;
		nSample += pAY8910Buffer[1][n]; // >> 2;
		nSample += pAY8910Buffer[2][n]; // >> 2;
		nSample += pAY8910Buffer[3][n]; // >> 2;
		nSample += pAY8910Buffer[4][n]; // >> 2;
		nSample += pAY8910Buffer[5][n]; // >> 2;

		nSample /=4;

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
		nSoundBuffer[deltaSlave + n] = nSample;//pAY8910Buffer[5][n];//nSample;
	}

	if(deltaSlave>=RING_BUF_SIZE/2)
	{
		deltaSlave=0;
		PCM_Task(pcm); // bon emplacement
	}

	deltaSlave+=SOUND_LEN;

	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = deltaSlave;
}

