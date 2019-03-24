// FB Alpha UPL (Ninja Kid 2) driver module
// Based on MAME driver by Roberto Ventura, Leandro Dardini, Yochizo, Nicola Salmoria

//#include "tiles_generic.h"
//#include "z80_intf.h"
#include "d_ninjakd2.h"
#include "mc8123.h"
//#include "burn_ym2203.h"
#define nScreenHeight 192
#define nScreenWidth 256
UINT16 *ss_map3;

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvRobokid = {
		"robokid", "ninkd2",
		"Atomic Robo-kid (World, Type-2)", 
		robokidRomInfo, robokidRomName, DrvInputInfo, RobokidDIPInfo,
		RobokidInit, DrvExit, DrvFrame, NULL
	};

	if (strcmp(nBurnDrvRobokid.szShortName, szShortName) == 0) 
	memcpy(shared,&nBurnDrvRobokid,sizeof(struct BurnDriver));

	ss_reg   = (SclNorscl *)SS_REG;
	ss_regs = (SclSysreg *)SS_REGS;
	ss_regd = (SclDataset *)SS_REGD;
}

/*static*/  void DrvPaletteUpdate(INT32 offset)
{
	offset &= 0x7fe;

	INT32 p = (DrvPalRAM[offset+0] * 256) + DrvPalRAM[offset+1];

	INT32 r = p >> 12;
	INT32 g = (p >> 8) & 0xf;
	INT32 b = (p >> 4) & 0xf;

	r |= r << 4;
	g |= g << 4;
	b |= b << 4;

	if((((offset/2)+1)%16)==0)
	{
		colAddr[(offset/2)-15]= BurnHighCol(r,g,b,0);
	}
	else if(((offset/2)%16)==0)
	{
		colAddr[(offset/2)+15]= BurnHighCol(r,g,b,0);
	}
	else
	{
		colAddr[offset/2] = BurnHighCol(r,g,b,0);
	}
}

/*static*/  void ninjakd2_bankswitch(INT32 data)
{
	INT32 nBank = 0x10000 + (data * 0x4000);

//	nZ80RomBank = data;

	CZetMapMemory(DrvZ80ROM0 + nBank, 	0x8000, 0xbfff, MAP_ROM);
}

/*static*/  void ninjakd2_bgconfig(INT32 sel, INT32 offset, UINT8 data)
{
	switch (offset & 0x07)
	{
		case 0:
			scrollx[sel] = (scrollx[sel] & 0x700) + data;
		return;

		case 1:
			scrollx[sel] = (scrollx[sel] & 0x0ff) + ((data & 0x07) * 256);
		return;

		case 2:
			scrolly[sel] = (scrolly[sel] & 0x100) + data;
		return;

		case 3:
			scrolly[sel] = (scrolly[sel] & 0x0ff) + ((data & 0x01) * 256);
		return;

		case 4:
			tilemap_enable[sel] = data & 0x01;

//nbg2 fg
//sel=0 => nbg1
//sel=1 => nbg0
//sel=2 => nbg3
		if(!sel)
		{
			ss_regs->dispenbl &= 0xfffd;
			ss_regs->dispenbl |= ((data & 0x01)<<1) & 0x0002;
			BGON = ss_regs->dispenbl;
		}
		else
		{
			ss_regs->dispenbl &= 0xfffe;
			ss_regs->dispenbl |= (data & 0x01) & 0x0001;
			BGON = ss_regs->dispenbl;
		}
		return;
	}
}

/*static*/  UINT8 __fastcall ninjakd2_main_read(UINT16 address)
{
	switch (address)
	{
		case 0xc000:
		case 0xc001:
		case 0xc002:
		case 0xdc00:
		case 0xdc01:
		case 0xdc02:
		case 0xf800:
		case 0xf801:
		case 0xf802:
			return DrvInputs[address & 3];

		case 0xc003:
		case 0xc004:
		case 0xdc03:
		case 0xdc04:
		case 0xf803:
		case 0xf804:
			return DrvDips[(address & 7) - 3];
	}

	return 0;
}

/*static*/  void __fastcall ninjakd2_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xc800) {
		DrvPalRAM[address & 0x7ff] = data;
		DrvPaletteUpdate(address);
		return;
	}

	switch (address)
	{
		case 0xc200:
			soundlatch = data;
		return;

		case 0xc201:
		{
			if (data & 0x10)
			{
				CZetClose();
				CZetOpen(1);
				CZetReset();
				CZetClose();
				CZetOpen(0);
			}

			flipscreen = data & 0x80;
		}
		return;

		case 0xc202:
			ninjakd2_bankswitch(data & 0x07);
		return;

		case 0xc203:
			overdraw_enable = data & 0x01;
		return;

		case 0xc208:
		case 0xc209:
		case 0xc20a:
		case 0xc20b:
		case 0xc20c:
			ninjakd2_bgconfig(0, address, data);
		return;
	}
}

/*static*/  void __fastcall mnight_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xf000) {
		DrvPalRAM[address & 0x7ff] = data;
		DrvPaletteUpdate(address);
		return;
	}

	switch (address)
	{
		case 0xfa00:
			soundlatch = data;
		return;

		case 0xfa01:
		{
			if (data & 0x10)
			{
				CZetClose();
				CZetOpen(1);
				CZetReset();
				CZetClose();
				CZetOpen(0);
			}

			flipscreen = data & 0x80;
		}
		return;

		case 0xfa02:
			ninjakd2_bankswitch(data & 0x07);
		return;

		case 0xfa03:
			overdraw_enable = data & 0x01;
		return;

		case 0xfa08:
		case 0xfa09:
		case 0xfa0a:
		case 0xfa0b:
		case 0xfa0c:
			ninjakd2_bgconfig(0, address, data);
		return;
	}
}

/*static*/  void robokid_rambank(INT32 sel, UINT8 data)
{
	UINT8 *ram[3] = { DrvBgRAM0, DrvBgRAM1, DrvBgRAM2 };
	INT32 off[2][3]  = { { 0xd800, 0xd400, 0xd000 }, { 0xc400, 0xc800, 0xcc00 } };

	INT32 nBank = 0x400 * data;

//	nZ80RamBank[sel&3] = data;

	CZetMapMemory(ram[sel&3] + nBank, off[sel>>2][sel&3], off[sel>>2][sel&3] | 0x3ff, MAP_RAM);
}

/*static*/  void __fastcall robokid_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xc000) {
		DrvPalRAM[address & 0x7ff] = data;
		DrvPaletteUpdate(address);
		return;
	}

	switch (address)
	{
		case 0xdc00:
			soundlatch = data;
		return;

		case 0xdc01:
		{
			if (data & 0x10) {
				CZetClose();
				CZetOpen(1);
				CZetReset();
				CZetClose();
				CZetOpen(0);
			}

			flipscreen = data & 0x80;
		}
		return;

		case 0xdc02:
			ninjakd2_bankswitch(data & 0x0f);
		return;

		case 0xdc03:
			overdraw_enable = data & 0x01;
		return;

		case 0xdd00:
		case 0xdd01:
		case 0xdd02:
		case 0xdd03:
		case 0xdd04:
			ninjakd2_bgconfig(0, address, data);
		return;

		case 0xdd05:
			robokid_rambank(0, data & 1);
		return;

		case 0xde00:
		case 0xde01:
		case 0xde02:
		case 0xde03:
		case 0xde04:
			ninjakd2_bgconfig(1, address, data);
		return;

		case 0xde05:
			robokid_rambank(1, data & 1);
		return;

		case 0xdf00:
		case 0xdf01:
		case 0xdf02:
		case 0xdf03:
		case 0xdf04:
			ninjakd2_bgconfig(2, address, data);
		return;

		case 0xdf05:
			robokid_rambank(2, data & 1);
		return;
	}
}
#if	 0
// Copied directly from MAME
/*static*/  UINT8 omegaf_protection_read(INT32 offset)
{
	UINT8 result = 0xff;

	switch (m_omegaf_io_protection[1] & 3)
	{
		case 0:
			switch (offset)
			{
				case 1:
					switch (m_omegaf_io_protection[0] & 0xe0)
					{
						case 0x00:
							if (++m_omegaf_io_protection_tic & 1)
							{
								result = 0x00;
							}
							else
							{
								switch (m_omegaf_io_protection_input)
								{
									// first interrogation
									// this happens just after setting mode 0.
									// input is not explicitly loaded so could be anything
									case 0x00:
										result = 0x80 | 0x02;
										break;

									// second interrogation
									case 0x8c:
										result = 0x80 | 0x1f;
										break;

									// third interrogation
									case 0x89:
										result = 0x80 | 0x0b;
										break;
								}
							}
							break;

						case 0x20:
							result = 0xc7;
							break;

						case 0x60:
							result = 0x00;
							break;

						case 0x80:
							result = 0x20 | (m_omegaf_io_protection_input & 0x1f);
							break;

						case 0xc0:
							result = 0x60 | (m_omegaf_io_protection_input & 0x1f);
							break;
					}
					break;
			}
			break;

		case 1: // dip switches
			switch (offset)
			{
				case 0: result = DrvDips[0]; break;
				case 1: result = DrvDips[1]; break;
				case 2: result = 0x02; break;
			}
			break;

		case 2: // player inputs
			switch (offset)
			{
				case 0: result = DrvInputs[1]; break;
				case 1: result = DrvInputs[2]; break;
				case 2: result = 0x01; break;
			}
			break;
	}

	return result;
}

/*static*/  UINT8 __fastcall omegaf_main_read(UINT16 address)
{
	switch (address)
	{
		case 0xc000:
			return DrvInputs[0];

		case 0xc001:
		case 0xc002:
		case 0xc003:
			return omegaf_protection_read(address - 0xc001);
	}

	return 0;
}

/*static*/  void __fastcall omegaf_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xd800) {
		DrvPalRAM[address & 0x7ff] = data;
		DrvPaletteUpdate(address);
		return;
	}

	switch (address)
	{
		case 0xc000:
			soundlatch = data;
		return;

		case 0xc001:
		{
			if (data & 0x10) {
				CZetClose();
				CZetOpen(1);
				CZetReset();
				CZetClose();
				CZetOpen(0);
			}

			flipscreen = data & 0x80;
		}
		return;

		case 0xc002:
			ninjakd2_bankswitch(data & 0x0f);
		return;

		case 0xc003:
			overdraw_enable = data & 0x01;
		return;

		case 0xc004:
		case 0xc005:
		case 0xc006:
		{
			if (address == 0xc006 && (data & 1) && !(m_omegaf_io_protection[2] & 1))
			{
				m_omegaf_io_protection_input = m_omegaf_io_protection[0];
			}
		
			m_omegaf_io_protection[address - 0xc004] = data;
		}
		return;

		case 0xc100:
		case 0xc101:
		case 0xc102:
		case 0xc103:
		case 0xc104:
			ninjakd2_bgconfig(0, address, data);
		return;

		case 0xc105:
			robokid_rambank(4|0, data & 7);
		return;

		case 0xc200:
		case 0xc201:
		case 0xc202:
		case 0xc203:
		case 0xc204:
			ninjakd2_bgconfig(1, address, data);
		return;

		case 0xc205:
			robokid_rambank(4|1, data & 7);
		return;

		case 0xc300:
		case 0xc301:
		case 0xc302:
		case 0xc303:
		case 0xc304:
			ninjakd2_bgconfig(2, address, data);
		return;

		case 0xc305:
			robokid_rambank(4|2, data & 7);
		return;
	}
}

/*static*/  void ninjakd2_sample_player(INT16 *dest, INT32 len)
{
	if (ninjakd2_sample_offset == -1) return;

	for (INT32 i = 0; i < len; i++)
	{
		UINT16 ofst =  ninjakd2_sample_offset + ((i * 271) / len);

		if (DrvSndROM[ofst] == 0) {
			ninjakd2_sample_offset = -1;
			break;
		}

		INT32 sample = BURN_SND_CLIP(((DrvSndROM[ofst]<<7) * 45) / 100);

		dest[i*2+0] = BURN_SND_CLIP(dest[i*2+0]+sample);
		dest[i*2+1] = BURN_SND_CLIP(dest[i*2+1]+sample);
	}

	if (ninjakd2_sample_offset != -1)
		ninjakd2_sample_offset += 271;
}

/*static*/  void __fastcall ninjakd2_sound_write(UINT16 address, UINT8 data)
{
	data = data;

	switch (address)
	{
		case 0xf000:
			ninjakd2_sample_offset = data << 8;
		return;
	}
}

/*static*/  UINT8 __fastcall ninjakd2_sound_read(UINT16 address)
{
	switch (address)
	{
		case 0xe000:
			return soundlatch;
	}

	return 0;
}

/*static*/  void __fastcall ninjakd2_sound_write_port(UINT16 port, UINT8 data)
{
	switch (port & 0xff)
	{
		case 0x00:
		case 0x01:
		case 0x80:
		case 0x81:
//			BurnYM2203Write((port >> 7) & 1, port & 1, data);
		return;
	}
}

/*static*/  void ninjakd2_sound_init()
{
//	CZetInit(1);
	CZetOpen(1);

//	CZetMapMemory(DrvZ80ROM1, 0x0000, 0xbfff, MAP_ROM);

	CZetMapArea(0x0000, 0xbfff, 0, DrvZ80ROM1);
	CZetMapArea2(0x0000, 0xbfff, 2, DrvZ80ROM1 + 0x10000, DrvZ80ROM1);

	CZetMapMemory(DrvZ80RAM1,		0xc000, 0xc7ff, MAP_RAM);
	CZetSetOutHandler(ninjakd2_sound_write_port);
	CZetSetWriteHandler(ninjakd2_sound_write);
	CZetSetReadHandler(ninjakd2_sound_read);
	CZetClose();
}
#endif

/*static*/  INT32 DrvDoReset()
{
	memset(AllRam, 0, RamEnd - AllRam);

	CZetOpen(0);
	CZetReset();
	CZetClose();
/*
	CZetOpen(1);
	CZetReset();
//	BurnYM2203Reset();
	CZetClose();
*/
	memset (scrollx, 0, 3 * sizeof(UINT16));
	memset (scrolly, 0, 3 * sizeof(UINT16));

//	nZ80RomBank = 0;
//	memset (nZ80RamBank, 0, 3);

	overdraw_enable = 0;
	memset (tilemap_enable, 0, 3);

//	memset (m_omegaf_io_protection, 0, 3);
//	m_omegaf_io_protection_input = 0;
//	m_omegaf_io_protection_tic = 0;

//	ninjakd2_sample_offset = -1;

	previous_coin[0] = previous_coin[1] = 0;

//	HiscoreReset();

	return 0;
}

/*static*/  INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;
	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;

	DrvZ80ROM0				= Next; Next += 0x050000;
//	DrvZ80ROM1				= Next; Next += 0x020000;
	DrvGfxROM4Data1		= Next; Next += 0x050000;

	DrvGfxROM0	 	= (UINT8 *)cache;// fg //Next; Next += 0x010000;
	DrvGfxROM1		= (UINT8 *)(ss_vram+0x1100); // sprites //(UINT8*)cache+0x010000;//Next; Next += 0x080000;
	DrvGfxROM2	 	= (UINT8 *)cache+0x08000;// bg1 //Next; Next += 0x100000;
	DrvGfxROM3	 	= (UINT8 *)cache+0x28000;//bg2  //Next; Next += 0x100000;
	DrvGfxROM4		= (UINT8 *)cache+0x58000;//bg3 // Next; Next += 0x100000;
/*
	DrvZ80Key	= Next; Next += 0x002000;
	DrvSndROM	= Next; Next += 0x010000;
*/
//	DrvPalette	= (UINT32*)Next; Next += 0x0400 * sizeof(UINT32);
//	DrvPalette		= (UINT16*)colAddr;
	AllRam			= Next;
	CZ80Context	= Next; Next += (0x1080);

	DrvZ80RAM0	= 0x2F0000;//Next; Next += 0x001a00;
//	DrvZ80RAM1	= 0x2F1a00;//Next; Next += 0x000800; // vbt : r?cuperer ces 0x800 de ram
	DrvSprRAM	= 0x2F2200;//Next; Next += 0x000600;
	DrvPalRAM	= 0x2F2800;//Next; Next += 0x000800;
	DrvFgRAM	= 0x2F3000;//Next; Next += 0x000800;
	DrvBgRAM0	= 0x2F3800;//Next;
	DrvBgRAM	= 0x2F5800;//Next; Next += 0x002000;
	DrvBgRAM1	= 0x2F7800;//Next; Next += 0x002000;
	DrvBgRAM2	= 0x2F9800;//Next; Next += 0x002000;
/*
	DrvZ80RAM0	= Next; Next += 0x001a00;
//	DrvZ80RAM1	= Next; Next += 0x000800; // vbt : récuperer ces 0x800 de ram

	DrvSprRAM	= Next; Next += 0x000600;
	DrvPalRAM	= Next; Next += 0x000800;
	DrvFgRAM	= Next; Next += 0x000800;
	DrvBgRAM0	= Next;
	DrvBgRAM	= Next; Next += 0x002000;
	DrvBgRAM1	= Next; Next += 0x002000;
	DrvBgRAM2	= Next; Next += 0x002000;
*/
//	soundlatch	= Next; Next += 0x000001;
//	flipscreen	= Next; Next += 0x000001;

//	pSpriteDraw	= (UINT16*)Next; Next += 256 * 256 * sizeof(UINT16);

	RamEnd		= Next;

	MemEnd		= Next;

	return 0;
}

/*static*/  INT32 DrvGfxDecode(UINT8 *rom, INT32 len, INT32 type)
{
	UINT32 Plane[4]   = { STEP4(0,1) };
	UINT32 XOffs0[16] = { STEP8(0,4), STEP8(32*8,4) };
	UINT32 XOffs1[16] = { STEP8(0,4), STEP8(64*8,4) };
	UINT32 YOffs0[16] = { STEP8(0,32), STEP8(64*8,32) };
	UINT32 YOffs1[16] = { STEP16(0,32) };

//	UINT8 *tmp = (UINT8*)BurnMalloc(len);
	UINT8 *tmp = (UINT8*)0x00200000; //malloc(0x40000);

//	if (tmp == NULL) {
//		return 1;
//	}


	switch (type)
	{
		case 0:
				memcpy (tmp, rom, len);

			GfxDecode4Bpp((len * 2) / ( 8 *  8), 4,  8,  8, Plane, XOffs0, YOffs0, 0x100, tmp, rom);
		break;

		case 1:
				memcpy (tmp, rom, len);

			GfxDecode4Bpp((len * 2) / (16 * 16), 4, 16, 16, Plane, XOffs0, YOffs0, 0x400, tmp, rom);
		break;

		case 2: // decode pour sprites
			memcpy (tmp, rom, len);

			GfxDecode4Bpp((len * 2) / (16 * 16), 4, 16, 16, Plane, XOffs1, YOffs1, 0x400, tmp, rom);
		break;
		case 3: // decode pour bg1
			tmp = (UINT8*)DrvGfxROM0;
			memcpy (tmp, rom, len);

			GfxDecode4Bpp((len * 2) / (16 * 16), 4, 16, 16, Plane, XOffs1, YOffs1, 0x400, tmp, rom);
			tile16x16toSaturn(1,(len * 2) / (16 * 16), rom);
		break;

		case 4: // decode pour bg2
			tmp = (UINT8*)DrvGfxROM0;
			memcpy (tmp, rom, len);

			GfxDecode4Bpp((len * 2) / (16 * 16), 4, 16, 16, Plane, XOffs1, YOffs1, 0x400, tmp, rom);
			tile16x16toSaturn(1,(len * 2) / (16 * 16), rom);
		break;
	}
	return 0;
}

/*static*/  void lineswap_gfx_roms(UINT8 *rom, INT32 len, const INT32 bit)
{
	UINT8 *tmp = (UINT8*)BurnMalloc(len);

	const INT32 mask = (1 << (bit + 1)) - 1;

	for (INT32 sa = 0; sa < len; sa++)
	{
		const INT32 da = (sa & ~mask) | ((sa << 1) & mask) | ((sa >> bit) & 1);
		tmp[da] = rom[sa];
	}

	memcpy (rom, tmp, len);

	BurnFree (tmp);
}

/*static*/  INT32 RobokidInit()
{
	DrvInitSaturn();

	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;

	if ((AllMem = (UINT8 *)BurnMalloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();

	{
		if (BurnLoadRom(DrvZ80ROM0 + 0x10000,  0, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x20000,  1, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x30000,  2, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x40000,  3, 1)) return 1;
		memcpy (DrvZ80ROM0, DrvZ80ROM0 + 0x10000, 0x10000);

//		if (BurnLoadRom(DrvZ80ROM1 + 0x10000,  4, 1)) return 1;
//		memcpy (DrvZ80ROM1, DrvZ80ROM1 + 0x10000, 0x10000);

		if (BurnLoadRom(DrvGfxROM1 + 0x00000,  6, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x10000,  7, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x20000,  8, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x30000,  9, 1)) return 1;
// pas besoin : sprites
//		tile16x16toSaturn(1,(0x40000 * 2) / (16 * 16), DrvGfxROM1);

		DrvGfxDecode(DrvGfxROM1, 0x40000, 2);

		for (UINT32 i=0;i<0x40000;i++ )
		{
			if ((DrvGfxROM1[i]& 0x0f)     ==0x00)DrvGfxROM1[i] = DrvGfxROM1[i] & 0xf0 | 0xf;
			else if ((DrvGfxROM1[i]& 0x0f)==0x0f) DrvGfxROM1[i] = DrvGfxROM1[i] & 0xf0;

			if ((DrvGfxROM1[i]& 0xf0)       ==0x00)DrvGfxROM1[i] = 0xf0 | DrvGfxROM1[i] & 0x0f;
			else if ((DrvGfxROM1[i]& 0xf0)==0xf0) DrvGfxROM1[i] = DrvGfxROM1[i] & 0x0f;
		}
//DrvGfxROM3
		UINT8 *tmp = (UINT8*)0x00270000;
		if (BurnLoadRom((UINT8*)tmp + 0x00000, 17, 1)) return 1;
		if (BurnLoadRom((UINT8*)tmp + 0x10000, 18, 1)) return 1;
		if (BurnLoadRom((UINT8*)tmp + 0x20000, 19, 1)) return 1;
		if (BurnLoadRom((UINT8*)tmp + 0x30000, 20, 1)) return 1;
		if (BurnLoadRom((UINT8*)tmp + 0x40000, 21, 1)) return 1;
		if (BurnLoadRom((UINT8*)tmp + 0x50000, 22, 1)) return 1;
		if (BurnLoadRom((UINT8*)tmp + 0x60000, 23, 1)) return 1;
		if (BurnLoadRom((UINT8*)tmp + 0x70000, 24, 1)) return 1;

		DrvGfxDecode((UINT8*)tmp, 0x80000, 4);
		for (UINT32 i=0;i<0x80000;i++ )
		{
			if ((tmp[i]& 0x0f)     ==0x00)tmp[i] = tmp[i] & 0xf0 | 0xf;
			else if ((tmp[i]& 0x0f)==0x0f) tmp[i] = tmp[i] & 0xf0;

			if ((tmp[i]& 0xf0)       ==0x00)tmp[i] = 0xf0 | tmp[i] & 0x0f;
			else if ((tmp[i]& 0xf0)==0xf0) tmp[i] = tmp[i] & 0x0f;
		}
// tile 0xc00 à vider 
		for (UINT32 i=0x60000;i<0x60080;i++ )
		{
				tmp[i] = 0x00;
		}

//		memset(DrvGfxROM0,0x00,0x80000); // inutile
//DrvGfxROM2
		tmp = (UINT8*)0x00200000;
		if (BurnLoadRom((UINT8*)tmp + 0x00000, 10, 1)) return 1;
		if (BurnLoadRom((UINT8*)tmp + 0x10000, 11, 1)) return 1;
		if (BurnLoadRom((UINT8*)tmp + 0x20000, 12, 1)) return 1;
		if (BurnLoadRom((UINT8*)tmp + 0x30000, 13, 1)) return 1;
		if (BurnLoadRom((UINT8*)tmp + 0x40000, 14, 1)) return 1;
		if (BurnLoadRom((UINT8*)tmp + 0x50000, 15, 1)) return 1;
		if (BurnLoadRom((UINT8*)tmp + 0x60000, 16, 1)) return 1;

		DrvGfxDecode((UINT8*)tmp, 0x70000, 3);

		for (UINT32 i=0;i<0x60000;i++ )
		{
			if ((tmp[i]& 0x0f)     ==0x00)tmp[i] = tmp[i] & 0xf0 | 0xf;
			else if ((tmp[i]& 0x0f)==0x0f) tmp[i] = tmp[i] & 0xf0;

			if ((tmp[i]& 0xf0)       ==0x00)tmp[i] = 0xf0 | tmp[i] & 0x0f;
			else if ((tmp[i]& 0xf0)==0xf0) tmp[i] = tmp[i] & 0x0f;
		}

//		memset(DrvGfxROM0,0x00,0x70000);  on vire
//DrvGfxROM4 trouver 400k de ram !!!
		tmp = (UINT8*)DrvGfxROM4Data1;
		if (BurnLoadRom(tmp + 0x00000, 25, 1)) return 1;
		if (BurnLoadRom(tmp + 0x10000, 26, 1)) return 1;
		if (BurnLoadRom(tmp + 0x20000, 27, 1)) return 1;
		if (BurnLoadRom(tmp + 0x30000, 28, 1)) return 1;
		if (BurnLoadRom(tmp + 0x40000, 29, 1)) return 1;
//		if (BurnLoadRom(tmp + 0x50000, 30, 1)) return 1;

//		DrvGfxDecode((UINT8*)tmp, 0x60000, 3);
		DrvGfxDecode((UINT8*)tmp, 0x50000, 3);

		for (UINT32 i=0;i<0x50000;i++ )
		{
			if ((tmp[i]& 0x0f)     ==0x00)tmp[i] = tmp[i] & 0xf0 | 0xf;
			else if ((tmp[i]& 0x0f)==0x0f) tmp[i] = tmp[i] & 0xf0;

			if ((tmp[i]& 0xf0)       ==0x00)tmp[i] = 0xf0 | tmp[i] & 0x0f;
			else if ((tmp[i]& 0xf0)==0xf0) tmp[i] = tmp[i] & 0x0f;
		}
/*
		memset(DrvGfxROM0,0x00,0x60000);
*/
//DrvGfxROM0
		if (BurnLoadRom(DrvGfxROM0 + 0x00000,  5, 1)) return 1;

		for (UINT32 i=0;i<0x10000;i++ )
		{
			if ((DrvGfxROM0[i]& 0x0f)     ==0x00)DrvGfxROM0[i] = DrvGfxROM0[i] & 0xf0 | 0xf;
			else if ((DrvGfxROM0[i]& 0x0f)==0x0f) DrvGfxROM0[i] = DrvGfxROM0[i] & 0xf0;

			if ((DrvGfxROM0[i]& 0xf0)       ==0x00)DrvGfxROM0[i] = 0xf0 | DrvGfxROM0[i] & 0x0f;
			else if ((DrvGfxROM0[i]& 0xf0)==0xf0) DrvGfxROM0[i] = DrvGfxROM0[i] & 0x0f;
		}
//		memset(DrvGfxROM2,0x00,0x70000); // inutile


//		DrvGfxDecode(DrvGfxROM0, 0x08000, 0); // deja du 4bpp !!!


//		DrvGfxDecode((UINT8*)0x00200000, 0x70000, 3);
//		memset(DrvGfxROM2,0x00,0x70000);
//		DrvGfxDecode(DrvGfxROM3, 0x20000, 3);
//		DrvGfxDecode(DrvGfxROM4, 0x20000, 3);
	}

//	memset4_fast((UINT8*)0x00200000,0x00000000,0x40000);

	CZetInit2(1,CZ80Context);
//CZetInit(1);
	CZetOpen(0);
	CZetMapMemory(DrvZ80ROM0,		0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory(DrvZ80ROM0 + 0x10000, 	0x8000, 0xbfff, MAP_ROM);
	CZetMapMemory(DrvPalRAM,			0xc000, 0xc7ff, MAP_ROM);
	CZetMapMemory(DrvFgRAM,			0xc800, 0xcfff, MAP_RAM);
	CZetMapMemory(DrvBgRAM2,			0xd000, 0xd3ff, MAP_RAM);
	CZetMapMemory(DrvBgRAM1,			0xd400, 0xd7ff, MAP_RAM);
	CZetMapMemory(DrvBgRAM0,			0xd800, 0xdbff, MAP_RAM);
	CZetMapMemory(DrvZ80RAM0,		0xe000, 0xf9ff, MAP_RAM);
	CZetMapMemory(DrvSprRAM,			0xfa00, 0xffff, MAP_RAM);
	CZetSetWriteHandler(robokid_main_write);
	CZetSetReadHandler(ninjakd2_main_read);
	CZetClose();

//	ninjakd2_sound_init();
//	BurnYM2203SetPSGVolume(0, 0.03);
//	BurnYM2203SetPSGVolume(1, 0.03);

//	GenericTilesInit();
//memset(ss_map,0x11,0x10000);
	DrvDoReset();
	DrvCalculatePalette();

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
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.patnamecontrl =  0x0000;// VRAM A0 ?~I?t?Z?b?g 
	scfg.plate_addr[0] = ss_map;
	scfg.plate_addr[1] = ss_map;
	SCL_SetConfig(SCL_NBG2, &scfg); // fg

	scfg.pnamesize   = SCL_PN2WORD;
	scfg.charsize      = SCL_CHAR_SIZE_2X2;//OK du 1*1 surtout pas toucher
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.patnamecontrl =  0x0000;// VRAM A0 +0x10000?~I?t?Z?b?g 
	scfg.plate_addr[0] = (Uint32)ss_map2;
	scfg.plate_addr[1] = (Uint32)ss_map2;
	scfg.plate_addr[2] = (Uint32)ss_map2;
	scfg.plate_addr[3] = (Uint32)ss_map2;

	SCL_SetConfig(SCL_NBG1, &scfg); // bg0

	scfg.plate_addr[0] = (Uint32)ss_font; // bg1
	scfg.plate_addr[1] = (Uint32)ss_font;
	scfg.plate_addr[2] = (Uint32)ss_font;
	scfg.plate_addr[3] = (Uint32)ss_font;

	SCL_SetConfig(SCL_NBG0, &scfg);

	scfg.plate_addr[0] = (Uint32)ss_map3; // bg2
	scfg.plate_addr[1] = (Uint32)ss_map3;
	scfg.plate_addr[2] = (Uint32)ss_map3;
	scfg.plate_addr[3] = (Uint32)ss_map3;
//	scfg.dispenbl      = OFF;

	SCL_SetConfig(SCL_NBG3, &scfg);

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
/*static*/ void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
//	colBgAddr = (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	colAddr = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
//	SCL_AllocColRam(SCL_NBG2,OFF);
SCL_AllocColRam(SCL_NBG3,OFF);
SCL_AllocColRam(SCL_SPR,OFF);   // 0x200 pour sprites atomic robokid
SCL_AllocColRam(SCL_NBG2,OFF); // 0x300 pour fg atomic robokid
//SCL_AllocColRam(SCL_NBG0,OFF); 

//	SCL_SetColRam(SCL_NBG0,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn(INT32 i)
{
	cleanSprites();
	SPR_InitSlaveSH();
	nBurnSprites  = 100;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite           = (SprSpCmd *)SS_SPRIT;

 	SS_MAP  = ss_map		=(Uint16 *)SCL_VDP2_VRAM_B1+0x0000;		    // fg
	SS_MAP2 = ss_map2	=(Uint16 *)SCL_VDP2_VRAM_B1+0x1000;			// bg0
	SS_FONT = ss_font		=(Uint16 *)SCL_VDP2_VRAM_B1+0xc000;			// bg1
	ss_map3						=(Uint16 *)SCL_VDP2_VRAM_B1+0xa000;			// bg2

	SS_CACHE= cache		=(Uint8  *)SCL_VDP2_VRAM_A0;

	SS_SET_S0PRIN(4);
	SS_SET_N0PRIN(5);
	SS_SET_N2PRIN(7);
	SS_SET_N1PRIN(3);
	SS_SET_N3PRIN(6);
	initPosition();

		initColors();
		SCL_Open();
		ss_reg->n1_move_x =  (0<<16) ;

	initLayers();

	initSprites(264-1,216-1,0,0,8,-32);
	drawWindow(0,192,192,2,62);
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
	//*(unsigned int*)OPEN_CSH_VAR(SOUND_LEN) = 128;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void tile16x16toSaturn (unsigned char reverse, unsigned int num, unsigned char *pDest)
{
	unsigned int c;
	for (c = 0; c < num; c++) 
	{
		unsigned char new_tile[128];
		UINT8 *dpM = pDest + (c * 128);
		memcpyl(new_tile,dpM,128);
		unsigned int i=0,j=0,k=0;

		for (k=0;k<128;k+=64)
		{
			dpM = pDest + ((c * 128)+k);

			for (i=0;i<32;i+=4,j+=8)
			{
				if(reverse)
				{
					dpM[i]=new_tile[j];
					dpM[i+1]=new_tile[j+1];
					dpM[i+2]=new_tile[j+2];
					dpM[i+3]=new_tile[j+3];
					dpM[i+32]=new_tile[j+4];
					dpM[i+33]=new_tile[j+5];
					dpM[i+34]=new_tile[j+6];
					dpM[i+35]=new_tile[j+7];
				}
				else
				{
					dpM[i+32]=new_tile[j];
					dpM[i+33]=new_tile[j+1];
					dpM[i+34]=new_tile[j+2];
					dpM[i+35]=new_tile[j+3];
					dpM[i+0]=new_tile[j+4];
					dpM[i+1]=new_tile[j+5];
					dpM[i+2]=new_tile[j+6];
					dpM[i+3]=new_tile[j+7];
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------

/*static*/  INT32 DrvExit()
{
	CZetExit2();
	nSoundBufferPos=0;

	CZ80Context	=  NULL;
	MemEnd = AllRam = RamEnd = DrvZ80ROM0 = DrvGfxROM0 = DrvGfxROM1 = DrvGfxROM2 = NULL;
	DrvGfxROM3 = DrvGfxROM4 = DrvGfxROM4Data1 = DrvZ80RAM0 = DrvSprRAM = DrvPalRAM = NULL;
	DrvFgRAM = DrvBgRAM = DrvBgRAM0 = DrvBgRAM1 = DrvBgRAM2 = NULL;
	soundlatch = flipscreen = overdraw_enable = DrvReset = 0;
	free(AllMem);
	AllMem = NULL;

	return 0;
}

/*static*/  void DrvCalculatePalette()
{
	for (INT32 i = 0; i < 0x800; i+=2)
	{
		DrvPaletteUpdate(i);
	}
}

INT16 previous_bank[3]={-1,-1,-1};

#define INT_DIGITS 19
 char *itoa(i)
     int i;
{
  /* Room for INT_DIGITS digits, - and '\0' */
  static char buf[INT_DIGITS + 2];
  char *p = buf + INT_DIGITS + 1;	/* points to terminating '\0' */
  if (i >= 0) {
    do {
      *--p = '0' + (i % 10);
      i /= 10;
    } while (i != 0);
    return p;
  }
  else {			/* i < 0 */
    do {
      *--p = '0' - (i % 10);
      i /= 10;
    } while (i != 0);
    *--p = '-';
  }
  return p;
}
/*static*/  void draw_robokid_bg_layer(INT32 sel, UINT8 *ram, UINT8 *rom, INT32 width, INT32 transp)
{
	if (tilemap_enable[sel] == 0) return;

	INT32 wide = (width) ? 128 : 32;
//	INT32 xscroll = scrollx[sel] & ((wide * 16) - 1);
//	INT32 yscroll = (scrolly[sel] + 32) & 0x1ff;

UINT16* tmp = (UINT16*)0x00200000;

		UINT32 sx1 = (64 % wide);
		UINT32 sy1 = (64 / wide);

		UINT32 ofst1 = (sx1 & 0x0f) + (sy1 * 16) + ((sx1 & 0x70) * 0x20);
		UINT32 attr1  = ram[ofst1 * 2 + 1];

	if(previous_bank[sel]!=(((attr1 & 0x10) << 7) + ((attr1 & 0x20) << 5))		)
	{
		previous_bank[sel] = (((attr1 & 0x10) << 7) + ((attr1 & 0x20) << 5));
//				previous_bank[sel] = 0;
//				previous_bank[sel] = 0x400;
//0x800*128= position tile à 0x800
//(attr1 & 0x10) << 7 = 0x800
//(attr1 & 0x20) << 5 = 0x400
//		if(!sel)

		switch(sel)
		{
			case 0:
				memcpy(rom,(UINT8*)0x00200000+(previous_bank[sel]*128),0x20000);
				break;
			case 1:
				memcpy(rom,(UINT8*)0x00270000+(previous_bank[sel]*128),0x30000);
				break;
			case 2:
				memcpy(rom,(UINT8*)DrvGfxROM4Data1+(previous_bank[sel]*128),0x20000);
				break;
		}

//xxxx
//				memcpy(DrvGfxROM3,(UINT8*)0x00270000,0x20000);

//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"                   ",20,60);
//		char toto[100];
//		char *titi = &toto[0];
//		titi=itoa(previous_bank);
//		FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)titi,20,60);
	}

	for (UINT32 offs = 0; offs < wide * 32; offs++)
	{
		UINT32 sx = (offs % wide);
		UINT32 sy = (offs / wide);

		INT32 ofst = (sx & 0x0f) + (sy * 16) + ((sx & 0x70) * 0x20);
		INT32 attr  = ram[ofst * 2 + 1];
//		INT32 code  = ram[ofst * 2 + 0] + ((attr & 0x10) << 7) + ((attr & 0x20) << 5) + ((attr & 0xc0) << 2);
		UINT32 code  = ram[ofst * 2 + 0] + ((attr & 0xc0) << 2);

			int offs2 = (sx | sy <<5)*2;

		switch(sel)
		{
			case 0:
				ss_map2[offs2] = (attr & 0x0f);
				ss_map2[offs2+1] = (0x400+(code<<2));
				break;
			case 1:
				code  = ram[ofst * 2 + 0] + ((attr & 0x10) << 7) + ((attr & 0x20) << 5) + ((attr & 0xc0) << 2);
				ss_font[offs2] = (attr & 0x0f);
				ss_font[offs2+1] = (0x1400+((code)<<2));
				break;
			case 2:
//				previous_bank[sel] = 0;
				code  = ram[ofst * 2 + 0] + ((attr & 0x10) << 7) + ((attr & 0x20) << 5) + ((attr & 0xc0) << 2);
				ss_map3[offs2] = (attr & 0x0f);
//				ss_map3[offs2+1] = (0x2400+((code)<<2)); // si 0x20000 pour bg2
				ss_map3[offs2+1] = (0x2C00+((code)<<2));
				break;
		}
	}
}

/*static*/  void draw_fg_layer(INT32 color_offset)
{
	for (INT32 offs = (32 * 4); offs < (32 * 32) - (32 * 4); offs++)
	{
		UINT32 sx = (offs & 0x1f);

		UINT8 attr  = DrvFgRAM[offs*2+1];
		UINT16 code  = DrvFgRAM[offs*2+0] + ((attr & 0xc0) << 2);
		UINT16 off2s = sx | (offs / 0x20) <<6;

		code |= (attr & 0x20) << 3;
		ss_map[off2s] = (attr & 0x0f) <<12 | code & 0xfff;
	}
}

/*static*/  void draw_sprites(INT32 color_offset, INT32 robokid)
{
	int const big_xshift = robokid ? 1 : 0;
	int const big_yshift = robokid ? 0 : 1;

	UINT8* sprptr = DrvSprRAM + 11;
	int sprites_drawn = 0;
	int delta = 3;
	
	cleanSprites();

	while (1)
	{
		if (sprptr[2] & 0x02)
		{
			int sx = sprptr[1] - ((sprptr[2] & 0x01) << 8);
			int sy = sprptr[0];

			int code = sprptr[3] + ((sprptr[2] & 0xc0) << 2) + ((sprptr[2] & 0x08) << 7);
			int flipx = (sprptr[2] & 0x10) >> 4;
			int flipy = (sprptr[2] & 0x20) >> 5;
			int flip = (sprptr[2] & 0x30);
			int const color = sprptr[4] & 0x0f;
			int const big = (sprptr[2] & 0x04) >> 2;
/*
			if (*flipscreen)
			{
				sx = 240 - 16*big - sx;
				sy = 240 - 16*big - sy;
				flipx ^= 1;
				flipy ^= 1;
			}
*/
			if (big)
			{
				code &= ~3;
				code ^= flipx << big_xshift;
				code ^= flipy << big_yshift;
			}

			for (int y = 0; y <= big; ++y)
			{
				for (int x = 0; x <= big; ++x)
				{
					int const tile = code ^ (x << big_xshift) ^ (y << big_yshift);

					ss_sprite[delta].control		= ( JUMP_NEXT | FUNC_NORMALSP) | flip;
					ss_sprite[delta].drawMode	= ( ECD_DISABLE | COMPO_REP);

					ss_sprite[delta].ax					= sx + 16*x;
					ss_sprite[delta].ay					= (sy + 16*y) + 32;
					ss_sprite[delta].charSize		= 0x210;
					ss_sprite[delta].color			    = color<<4;//Colour<<4;
					ss_sprite[delta].charAddr		= 0x220+(tile<<4);
					delta++;

					++sprites_drawn;

					if (sprites_drawn >= 96)
						break;
				}
			}
		}
		else
		{
			++sprites_drawn;

			if (sprites_drawn >= 96)
				break;
		}

		sprptr += 16;
	}
}

/*static*/  INT32 RobokidDraw()
{
	draw_robokid_bg_layer(0, DrvBgRAM0, DrvGfxROM2, 0, 0);
	draw_robokid_bg_layer(1, DrvBgRAM1, DrvGfxROM3, 0, 1);
	draw_robokid_bg_layer(2, DrvBgRAM2, DrvGfxROM4, 0, 1);

	draw_sprites(0x200, 1);

	draw_fg_layer(0x300);

	ss_reg->n0_move_x =  scrollx[1]<<16;
	ss_reg->n0_move_y =  scrolly[1]<<16;
	ss_reg->n1_move_x =  scrollx[0]<<16;
	ss_reg->n1_move_y =  scrolly[0]<<16;
	ss_reg->n3_move_x =  scrollx[2];
	ss_reg->n3_move_y =  scrolly[2];

	for (INT32 offs = 0; offs < (64 * 32); offs++)
	{
		UINT32 sx = (offs & 0x3f);

		UINT8 attr  = DrvFgRAM[offs*2+1];
		UINT16 code  = DrvFgRAM[offs*2+0] + ((attr & 0xc0) << 2);
		UINT16 off2s = sx | (offs / 0x40) <<6;

		if((sx<32)&& (offs / 0x40)<28)
		{

		}
		else
			ss_map[off2s] = 0x5;
	}
	return 0;
}

/*static*/  inline void DrvClearOpposites(UINT8* nJoystickInputs)
{ // for active LOW
	if ((*nJoystickInputs & 0x03) == 0x00) {
		*nJoystickInputs |= 0x03;
	}
	if ((*nJoystickInputs & 0x0c) == 0x00) {
		*nJoystickInputs |= 0x0c;
	}
}
unsigned int vbt=0;
/*static*/  INT32 DrvFrame()
{
//	if (DrvReset) {
//		DrvDoReset();
//	}
//
if(vbt<1000)
	vbt++;
else
	{
DrvZ80RAM0[0x0E108-0xe000]= 0x12;	//Infinite Credits
DrvZ80RAM0[0x0E190-0xe000]= 0x99;	//Infinite Time
DrvZ80RAM0[0x0E021-0xe000]= 0x05;	//Infinite Lives
DrvZ80RAM0[0xF0C2-0xe000]= 0x01;	//Always Have Fly
DrvZ80RAM0[0xF0C3-0xe000]= 0x01;	//Always Have Speed Up
//DrvZ80ROM0[0x0534E]=0xC9;
//DrvZ80ROM0[0x0534F]=0xB7;
/*
DrvZ80ROM0[0x06051]=0x30;
DrvZ80ROM0[0x060C4]=0x30;
DrvZ80ROM0[0x134CC]=0x30;
DrvZ80ROM0[0x13566]=0x30;
DrvZ80ROM0[0x13566]=0x30;
*/

	}

	{
		memset (DrvInputs, 0xff, 3);

		for (INT32 i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
			DrvInputs[2] ^= (DrvJoy3[i] & 1) << i;
		}

		DrvClearOpposites(&DrvInputs[1]);
		DrvClearOpposites(&DrvInputs[2]);

		previous_coin[0] = (DrvInputs[0] & 0x40) ? 0 : (previous_coin[0] + 1);
		previous_coin[1] = (DrvInputs[0] & 0x80) ? 0 : (previous_coin[1] + 1);
		if (previous_coin[0] >= 4) DrvInputs[0] |= 0x40;
		if (previous_coin[1] >= 4) DrvInputs[0] |= 0x80;
	}

//	CZetNewFrame();

	INT32 nCycleSegment;
	INT32 nInterleave = 10;
	INT32 nCyclesTotal[2] = { 6000000 / 60, 5000000 / 60 };
	INT32 nCyclesDone[2] = { 0, 0 };

	for (INT32 i = 0; i < nInterleave; i++)
	{
		nCycleSegment = nCyclesTotal[0] / nInterleave;

		CZetOpen(0);
		nCyclesDone[0] += CZetRun(nCycleSegment);

		if (i == (nInterleave-1))
		{
//			CZetSetVector(0xd7);
//			CZetRaiseIrq(0xd7);
//			CZetSetIRQLine(0, CZET_IRQSTATUS_AUTO);
			CZetRaiseIrq(0xd7);
			CZetSetIRQLine(0xd7, 0);
		}

		CZetClose();
	}
	RobokidDraw();
	return 0;
}
