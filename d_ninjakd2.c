// FB Alpha UPL (Ninja Kid 2) driver module
// Based on MAME driver by Roberto Ventura, Leandro Dardini, Yochizo, Nicola Salmoria

#include "d_ninjakd2.h"

#define nInterleave 10
#define nCyclesTotal 6000000 / 60
#define nCycleSegment nCyclesTotal / nInterleave
//unsigned int vbt=0;
unsigned int bg_cache[3][0x1000];
unsigned int nextTile[3]={0,0,0};


int ovlInit(char *szShortName)
{
	cleanBSS();

	struct BurnDriver nBurnDrvRobokid = {
		"robokid", "ninkd2",
		"Atomic Robo-kid (World, Type-2)", 
		robokidRomInfo, robokidRomName, DrvInputInfo, RobokidDIPInfo,
		RobokidInit, DrvExit, DrvFrame
	};

	struct BurnDriver nBurnDrvNinjakd2a = {
		"ninkd2a", "ninkd2", 
		"Ninja-Kid II (set 2, bootleg?)",
		ninjakd2aRomInfo, ninjakd2aRomName, DrvInputInfo, Ninjakd2DIPInfo,
		Ninjakd2DecryptedInit, DrvExit, DrvFrame
	};
	
	struct BurnDriver nBurnDrvMnight = {
		"mnight", "ninkd2", 
		"Mutant Night",
		mnightRomInfo, mnightRomName, DrvInputInfo, MnightDIPInfo,
		MnightInit, DrvExit, DrvFrame
	};	
/*	
	struct BurnDriver nBurnDrvOmegafs = {
		"omegafs", "ninkd2",
		"Omega Fighter Special",
		omegafsRomInfo, omegafsRomName, OmegafInputInfo, OmegafDIPInfo,
		OmegafInit, DrvExit, DrvFrame
	};
	
	struct BurnDriver nBurnDrvOmegaf = {
		"omegaf", "ninkd2",
		"Omega Fighter",
		omegafRomInfo, omegafRomName, OmegafInputInfo, OmegafDIPInfo,
		OmegafInit, DrvExit, DrvFrame
	};	
*/
	if (!strcmp(nBurnDrvRobokid.szShortName, szShortName)) 
	memcpy(shared,&nBurnDrvRobokid,sizeof(struct BurnDriver));

	if (!strcmp(nBurnDrvNinjakd2a.szShortName, szShortName)) 
	memcpy(shared,&nBurnDrvNinjakd2a,sizeof(struct BurnDriver));
/*
	if (!strcmp(nBurnDrvOmegafs.szShortName, szShortName)) 
	memcpy(shared,&nBurnDrvOmegafs,sizeof(struct BurnDriver));
*/
	if (!strcmp(nBurnDrvMnight.szShortName, szShortName)) 
	memcpy(shared,&nBurnDrvMnight,sizeof(struct BurnDriver));

	ss_reg   = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
	ss_regd  = (SclDataset *)SS_REGD;
}

void DrvOverDraw()
{
//	SCL_SetFrameInterval(-1);
//	SPR_WRITE_REG(SPR_W_FBCR, SPR_FBCR_MANUAL | 0);
	if(overdraw_enable)
		SPR_WRITE_REG(SPR_W_FBCR, SPR_FBCR_MANUAL);
	else
		SPR_WRITE_REG(SPR_W_FBCR, SPR_FBCR_AUTO);
}

void DrvPaletteUpdate(INT32 offset)
{
	offset &= 0x7fe;

	UINT32 p = ((DrvPalRAM[offset+0] << 8) + DrvPalRAM[offset+1])>>4;
	offset>>=1;

	if(((offset+1)%16)==0)
	{
		colAddr[(offset)-15]= cram_lut[p];
	}
	else if((offset%16)==0)
	{
		colAddr[(offset)+15]= cram_lut[p];
	}
	else
	{
		colAddr[offset] = cram_lut[p];
	}
}

void ninjakd2_bankswitch(INT32 data)
{
	UINT32 nBank = 0x10000 + (data * 0x4000);

//	nZ80RomBank = data;

	CZetMapMemory(DrvZ80ROM0 + nBank, 	0x8000, 0xbfff, MAP_ROM);
}

void ninjakd2_bgconfig(UINT32 sel, UINT32 offset, UINT8 data)
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

UINT8 __fastcall ninjakd2_main_read(UINT16 address)
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

void __fastcall ninjakd2_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xc800) {
		DrvPalRAM[address & 0x7ff] = data;
		DrvPaletteUpdate(address^1);
		return;
	}

	if(address>= 0xd000 && address <= 0xd7ff)
	{
		address-= 0xd000;
		if(DrvFgRAM[address]!=data)
		{
			DrvFgRAM[address] = data;
			UINT32 sx = ((address>>1) & 0x1f);
			UINT32 off2s = sx | (address/0x40) <<6;

			address &=0x7fe;
			UINT8 *vram = &DrvFgRAM[address];
			UINT32 attr  = vram[1];
			UINT32 code  = vram[0] + ((attr & 0xc0) << 2);
			
			code |= (attr & 0x20) << 3;
			ss_map[off2s] = (attr & 0x0f) <<12 | code & 0xfff;
		}
		return;
	}

	if(address>= 0xd800 && address <= 0xdfff)
	{
		address-= 0xd800;
		if(DrvBgRAM[address]!=data)
		{
			DrvBgRAM[address] = data;
			address &=0x7fe;
			UINT8 *vram = &DrvBgRAM[address];
			UINT32 attr  = vram[1];
			UINT32 code  = vram[0] + ((attr & 0xc0) << 2);
			
			UINT16 *map = (UINT16 *)&ss_map2[address];
			map[0] = (attr & 0x0f);
			map[1] = 0x400 + (code<<2) & 0xffff;
		}
		return;
	}	

	switch (address)
	{
		case 0xc200:
			soundlatch = data;
		return;

		case 0xc201:
		/*{
			if (data & 0x10)
			{
				CZetReset(1);
			}

			*flipscreen = data & 0x80;
		}*/
		return;

		case 0xc202:
			ninjakd2_bankswitch(data & 0x07);
		return;

		case 0xc203:
			if(overdraw_enable != data & 0x01)
			{
				overdraw_enable = data & 0x01;
			}
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

void __fastcall mnight_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xf000) {
		DrvPalRAM[address & 0x7ff] = data;
		DrvPaletteUpdate(address);
		return;
	}

	if(address>= 0xe800 && address <= 0xefff)
	{
		address-= 0xe800;
		if(DrvFgRAM[address]!=data)
		{
			DrvFgRAM[address] = data;
			UINT32 sx = ((address>>1) & 0x1f);
			UINT32 off2s = sx | (address/0x40) <<6;

			address &=0x7fe;
			UINT8 *vram = &DrvFgRAM[address];
			UINT32 attr  = vram[1];
			UINT32 code  = vram[0] + ((attr & 0xc0) << 2);
			
			code |= (attr & 0x20) << 3;
			ss_map[off2s] = (attr & 0x0f) <<12 | code & 0xfff;
		}
		return;
	}

	if(address>= 0xe000 && address <= 0xe7ff)
	{
		address-= 0xe000;
		if(DrvBgRAM[address]!=data)
		{
			DrvBgRAM[address] = data;
			address &=0x7fe;
			UINT8 *vram = &DrvBgRAM[address];
			UINT32 attr  = vram[1];
			UINT32 code  = vram[0] + ((attr & 0xc0) << 2) + ((attr & 0x10) << 6);
			
			UINT16 *map = (UINT16 *)&ss_map2[address];
			map[0] = (attr & 0x0f);
			map[1] = 0x400 + (code<<2) & 0xffff;
		}
		return;
	}	

	switch (address)
	{
		case 0xfa00:
			soundlatch = data;
		return;

		case 0xfa01:
		/*{
			if (data & 0x10)
			{
				ZetReset(1);
			}

			*flipscreen = data & 0x80;
		}*/
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

void robokid_rambank(INT32 sel, UINT8 data)
{
	UINT8 *ram[3] = { DrvBgRAM0, DrvBgRAM1, DrvBgRAM2 };
	UINT32 off[2][3]  = { { 0xd800, 0xd400, 0xd000 }, { 0xc400, 0xc800, 0xcc00 } };

	UINT32 nBank = 0x400 * data;
	CZetMapMemory(ram[sel&3] + nBank, off[sel>>2][sel&3], off[sel>>2][sel&3] | 0x3ff, MAP_RAM);
}

void __fastcall robokid_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xc000) {
		DrvPalRAM[address & 0x7ff] = data;
		DrvPaletteUpdate(address^1);
		return;
	}
	
	if(address>= 0xc800 && address <= 0xcfff)
	{
		address-= 0xc800;
		if(DrvFgRAM[address]!=data)
		{
			DrvFgRAM[address] = data;
			UINT32 sx = ((address>>1) & 0x1f);
			UINT32 off2s = sx | (address/0x40) <<6;

			address &=0x7fe;
			UINT8 *vram = &DrvFgRAM[address];
			UINT32 attr  = vram[1];
			UINT32 code  = vram[0] + ((attr & 0xc0) << 2);
			
			code |= (attr & 0x20) << 3;
			ss_map[off2s] = (attr & 0x0f) <<12 | code & 0xfff;
		}
		return;
	}	

	switch (address)
	{
		case 0xdc00:
			soundlatch = data;
		return;

		case 0xdc01:
		{
		/*	if (data & 0x10) {
				CZetClose();
				CZetOpen(1);
				CZetReset();
				CZetClose();
				CZetOpen(0);
			}
		*/
//			flipscreen = data & 0x80;
		}
		return;

		case 0xdc02:
			ninjakd2_bankswitch(data & 0x0f);
		return;

		case 0xdc03:
		
//		if(overdraw_enable != data & 0x01)
		{
			overdraw_enable = data & 0x01;
// VBT : 			astuce Niddy pour émuler l'overdraw de sprites
//			SCL_SetFrameInterval(-overdraw_enable);		
//			SCL_SetFrameInterval(-1);		
		}
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
#if 0
// Copied directly from MAME
UINT8 omegaf_protection_read(INT32 offset)
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

UINT8 __fastcall omegaf_main_read(UINT16 address)
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

void __fastcall omegaf_main_write(UINT16 address, UINT8 data)
{
	if ((address & 0xf800) == 0xd800) {
		DrvPalRAM[address & 0x7ff] = data;
		DrvPaletteUpdate(address^1);
		return;
	}

	switch (address)
	{
		case 0xc000:
			soundlatch = data;
		return;

		case 0xc001:
		{
/*			if (data & 0x10) {
				CZetClose();
				CZetOpen(1);
				CZetReset();
				CZetClose();
				CZetOpen(0);
			}
*/
//			flipscreen = data & 0x80;
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

void ninjakd2_sample_player(INT16 *dest, INT32 len)
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

void __fastcall 30/03/2019(UINT16 address, UINT8 data)
{
	data = data;

	switch (address)
	{
		case 0xf000:
			ninjakd2_sample_offset = data << 8;
		return;
	}
}

UINT8 __fastcall ninjakd2_sound_read(UINT16 address)
{
	switch (address)
	{
		case 0xe000:
			return soundlatch;
	}

	return 0;
}

void __fastcall ninjakd2_sound_write_port(UINT16 port, UINT8 data)
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

void ninjakd2_sound_init()
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

INT32 DrvDoReset()
{
	memset(DrvZ80RAM0, 0, RamEnd-DrvZ80RAM0);

	CZetOpen(0);
	CZetReset();
	CZetClose();

	memset (scrollx, 0, 3 * sizeof(INT16));
	memset (scrolly, 0, 3 * sizeof(INT16));

	soundlatch = 0;
	overdraw_enable = 0;
	memset (tilemap_enable, 0, 3);
	previous_coin[0] = previous_coin[1] = 0;

	return 0;
}

INT32 MemIndex(UINT32 game)
{
	UINT8 *Next; Next = AllMem;

	DrvZ80ROM0		= (UINT8 *)Next; Next += 0x050000;
	if(!game)
	{	
		DrvGfxROM4Data1	= (UINT8 *)Next; Next += 0x060000;
	}
//	DrvGfxROM2	 	= (UINT8 *)cache+0x08000;// bg1 //Next; Next += 0x100000;
//	DrvGfxROM3	 	= (UINT8 *)cache+0x28000;//bg2  //Next; Next += 0x100000;
//	DrvGfxROM4		= (UINT8 *)cache+0x58000;//bg3 // Next; Next += 0x100000;

	CZ80Context		= (UINT8 *)Next; Next += (sizeof(cz80_struc));	
	cram_lut 		= (UINT16 *)Next; Next += 0x1000 * (sizeof(UINT16));

	if(!game)
	{
		DrvZ80RAM0	= (UINT8 *)0x2F0000;//Next; Next += 0x001a00;
		DrvSprRAM	= (UINT8 *)0x2F2200;//Next; Next += 0x000600;
		DrvPalRAM	= (UINT8 *)0x2F2800;//Next; Next += 0x000800;
		DrvFgRAM	= (UINT8 *)0x2F3000;//Next; Next += 0x000800;
		DrvBgRAM0	= (UINT8 *)0x2F3800;//Next;
		DrvBgRAM	= (UINT8 *)0x2F5800;//Next; Next += 0x002000;
		DrvBgRAM1	= (UINT8 *)0x2F7800;//Next; Next += 0x002000;
		DrvBgRAM2	= (UINT8 *)0x2F9800;//Next; Next += 0x002000;

		RamEnd		= DrvBgRAM2+0x002000;
	}
	else
	{
		DrvGfxROM4Data1	= (UINT8 *)Next; Next += 0x040000;
		DrvZ80RAM0	= (UINT8 *)Next; Next += 0x002200;
		DrvSprRAM	= (UINT8 *)Next; Next += 0x000600;
		DrvPalRAM	= (UINT8 *)Next; Next += 0x000800;
		DrvFgRAM	= (UINT8 *)Next; Next += 0x000800;
		DrvBgRAM0	= (UINT8 *)Next; Next += 0x002000;
		DrvBgRAM	= (UINT8 *)Next; Next += 0x002000;
		DrvBgRAM1	= (UINT8 *)Next; Next += 0x002000;
		DrvBgRAM2	= (UINT8 *)Next; Next += 0x002000;

		RamEnd		= (UINT8 *)Next;		
	}
	return 0;
}

INT32 DrvGfxDecode(UINT8 *rom, UINT32 len, UINT32 type)
{
	UINT32 Plane[4]   = { STEP4(0,1) };
	UINT32 XOffs0[16] = { STEP8(0,4), STEP8(32*8,4) };
	UINT32 XOffs1[16] = { STEP8(0,4), STEP8(64*8,4) };
	UINT32 YOffs0[16] = { STEP8(0,32), STEP8(64*8,32) };
	UINT32 YOffs1[16] = { STEP16(0,32) };

	UINT8 *tmp = (UINT8*)0x00200000;

	switch (type)
	{
		case 0:
			memcpyl (tmp, rom, len);
			GfxDecode4Bpp((len * 2) / ( 8 *  8), 4,  8,  8, Plane, XOffs0, YOffs0, 0x100, tmp, rom);
			break;			
		case 1:
			memcpyl (tmp, rom, len);
			GfxDecode4Bpp((len * 2) / (16 * 16), 4, 16, 16, Plane, XOffs0, YOffs0, 0x400, tmp, rom);
			tile16x16toSaturn((len * 2) / (16 * 16), rom);			
			break;			
		case 10:
			// decode pour sprites		
			memcpyl (tmp, rom, len);
			GfxDecode4Bpp((len * 2) / (16 * 16), 4, 16, 16, Plane, XOffs0, YOffs0, 0x400, tmp, rom);
			break;				
		case 2:
			// decode pour sprites
			memcpyl (tmp, rom, len);
			GfxDecode4Bpp((len * 2) / (16 * 16), 4, 16, 16, Plane, XOffs1, YOffs1, 0x400, tmp, rom);
			break;
		case 4:
			// decode pour sprites
			memcpyl (tmp, rom, len);
			GfxDecode4Bpp((len * 2) / (16 * 16), 4, 16, 16, Plane, XOffs0, YOffs0, 0x400, tmp, rom);
			break;			
		case 3:
			// decode pour bg1, bg2 et 3
			tmp = (UINT8 *)cache; // utilisation temporaire de la vram
			memcpyl (tmp, rom, len);

			GfxDecode4Bpp((len * 2) / (16 * 16), 4, 16, 16, Plane, XOffs1, YOffs1, 0x400, tmp, rom);
			tile16x16toSaturn((len * 2) / (16 * 16), rom);
			break;		
	}
	return 0;
}

void lineswap_gfx_roms(UINT8 *rom, INT32 len, const INT32 bit)
{
	UINT8 *tmp = (UINT8*)DrvGfxROM4Data1;

	const INT32 mask = (1 << (bit + 1)) - 1;

	for (INT32 sa = 0; sa < len; sa++)
	{
		const INT32 da = (sa & ~mask) | ((sa << 1) & mask) | ((sa >> bit) & 1);
		tmp[da] = rom[sa];
	}

	memcpy (rom, tmp, len);
}

void gfx_unscramble(INT32 gfxlen)
{
	UINT8 *DrvGfxROM0	= (UINT8 *)cache;// fg
	UINT8 *ss_vram 		= (UINT8 *)SS_SPRAM;
	UINT8 *DrvGfxROM1	= (UINT8 *)(ss_vram+0x1100); // sprites
	UINT8 *DrvGfxROM2	= (UINT8 *)(UINT8 *)cache+0x08000; // bg
		
	lineswap_gfx_roms(DrvGfxROM0, 0x08000, 13);
	lineswap_gfx_roms(DrvGfxROM1, gfxlen, 14);
	lineswap_gfx_roms(DrvGfxROM2, gfxlen, 14);
}

/*static*/ void DrvCalculatePalette()
{
	for (UINT32 i = 0; i < 0x800; i+=2)
	{
		DrvPaletteUpdate(i);
	}
}

INT32 Ninjakd2CommonInit()
{
	DrvInitSaturnS(1);
	DrvDraw = Ninjakd2Draw;
	AllMem = NULL;
	MemIndex(1);

	if ((AllMem = (UINT8 *)BurnMalloc(MALLOC_MAX)) == NULL) return 1;
	memset(AllMem, 0, MALLOC_MAX);
	MemIndex(1);
	
	make_lut();
	{
		if (BurnLoadRom(DrvZ80ROM0 + 0x00000,  0, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x10000,  1, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x18000,  2, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x20000,  3, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x28000,  4, 1)) return 1;

//		if (BurnLoadRom(DrvZ80ROM1 + 0x00000,  5, 1)) return 1;
		UINT8 *DrvGfxROM0	= (UINT8 *)cache;// fg
		UINT8 *ss_vram 		= (UINT8 *)SS_SPRAM;
		UINT8 *DrvGfxROM1	= (UINT8 *)(ss_vram+0x1100); // sprites
		UINT8 *DrvGfxROM2	= (UINT8 *)(UINT8 *)cache+0x08000; // bg
//text		
		if (BurnLoadRom(DrvGfxROM0 + 0x00000,  6, 1)) return 1;
		
// sprites
		if (BurnLoadRom(DrvGfxROM1 + 0x00000,  7, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x10000,  8, 1)) return 1;
//bg		
		if (BurnLoadRom(DrvGfxROM2 + 0x00000,  9, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x10000, 10, 1)) return 1;

//		if (BurnLoadRom(DrvSndROM  + 0x00000, 11, 1)) return 1;

		gfx_unscramble(0x20000);
// vbt texte sans decode ??		
//		DrvGfxDecode(DrvGfxROM0, 0x08000, 0);
		swapFirstLastColor(DrvGfxROM0,0x0f,0x08000);

		DrvGfxDecode(DrvGfxROM1, 0x20000, 4); // sprites
		swapFirstLastColor(DrvGfxROM1,0x0f,0x20000);
		
		DrvGfxDecode(DrvGfxROM2, 0x20000, 1); // bg0
		swapFirstLastColor(DrvGfxROM2,0x0f,0x20000);
	}

	CZetInit2(1,CZ80Context);
	CZetOpen(0);
	CZetMapMemory(DrvZ80ROM0,			0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory(DrvZ80ROM0 + 0x10000, 0x8000, 0xbfff, MAP_ROM);
	CZetMapMemory(DrvPalRAM,			0xc800, 0xcdff, MAP_ROM);
//	CZetMapMemory(DrvFgRAM,				0xd000, 0xd7ff, MAP_RAM);
	CZetMapMemory(DrvFgRAM,				0xd000, 0xd7ff, MAP_ROM);
//	CZetMapMemory(DrvBgRAM,				0xd800, 0xdfff, MAP_RAM);
	CZetMapMemory(DrvBgRAM,				0xd800, 0xdfff, MAP_ROM);
	CZetMapMemory(DrvZ80RAM0,			0xe000, 0xf9ff, MAP_RAM);
	CZetMapMemory(DrvSprRAM,			0xfa00, 0xffff, MAP_RAM);
	CZetSetWriteHandler(ninjakd2_main_write);
	CZetSetReadHandler(ninjakd2_main_read);
	CZetClose();

//	ninjakd2_sound_init();

	DrvDoReset();
	DrvCalculatePalette();
	nBurnFunction = DrvOverDraw;	
	drawWindow(0,192,192,2,62);
	return 0;
}

INT32 Ninjakd2DecryptedInit()
{
	INT32 nRet = Ninjakd2CommonInit();

	/*if (nRet == 0)
	{
		memcpy (DrvZ80ROM1 + 0x10000, DrvZ80ROM1, 0x10000);
		memcpy (DrvZ80ROM1, DrvZ80ROM1 + 0x08000, 0x08000);
	}*/

	return nRet;
}

INT32 MnightInit()
{
	DrvInitSaturnS(1);
	DrvDraw = MnightDraw;
	AllMem = NULL;
	MemIndex(1);

	if ((AllMem = (UINT8 *)BurnMalloc(MALLOC_MAX)) == NULL) return 1;
	memset((void *)0x2F0000, 0, 0x9c00);
	memset(AllMem, 0, MALLOC_MAX);
	MemIndex(1);
	
	make_lut();
	{
		if (BurnLoadRom(DrvZ80ROM0 + 0x00000,  0, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x10000,  1, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x18000,  2, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x20000,  3, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x28000,  4, 1)) return 1;

//		if (BurnLoadRom(DrvZ80ROM1 + 0x00000,  5, 1)) return 1;
//		memcpy (DrvZ80ROM1 + 0x10000, DrvZ80ROM1, 0x10000);

		UINT8 *DrvGfxROM0	= (UINT8 *)cache;// fg
		UINT8 *ss_vram 		= (UINT8 *)SS_SPRAM;
		UINT8 *DrvGfxROM1	= (UINT8 *)(ss_vram+0x1100); // sprites
		UINT8 *DrvGfxROM2	= (UINT8 *)(UINT8 *)cache+0x08000; // bg

		if (BurnLoadRom(DrvGfxROM0 + 0x00000,  6, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x00000,  7, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x10000,  8, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x20000,  9, 1)) return 1;
		memcpyl (DrvGfxROM1 + 0x30000, DrvGfxROM1 + 0x20000, 0x10000);
		
		if (BurnLoadRom(DrvGfxROM2 + 0x00000, 10, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x10000, 11, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM2 + 0x20000, 12, 1)) return 1;

		gfx_unscramble(0x40000);

//		DrvGfxDecode(DrvGfxROM0, 0x08000, 0); // text
		swapFirstLastColor(DrvGfxROM0,0x0f,0x08000);
		
		DrvGfxDecode(DrvGfxROM1, 0x40000, 10); // sprites
		swapFirstLastColor(DrvGfxROM1,0x0f,0x40000);			

		DrvGfxDecode(DrvGfxROM2, 0x40000, 1); // bg0
		swapFirstLastColor(DrvGfxROM2,0x0f,0x40000);	
	}

	CZetInit2(1,CZ80Context);
	CZetOpen(0);
	CZetMapMemory(DrvZ80ROM0,			0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory(DrvZ80ROM0 + 0x10000, 0x8000, 0xbfff, MAP_ROM);
	CZetMapMemory(DrvZ80RAM0,			0xc000, 0xd9ff, MAP_RAM);
	CZetMapMemory(DrvSprRAM,			0xda00, 0xdfff, MAP_RAM);
//	CZetMapMemory(DrvBgRAM,				0xe000, 0xe7ff, MAP_RAM);
	CZetMapMemory(DrvBgRAM,				0xe000, 0xe7ff, MAP_ROM);
//	CZetMapMemory(DrvFgRAM,				0xe800, 0xefff, MAP_RAM);
	CZetMapMemory(DrvFgRAM,				0xe800, 0xefff, MAP_ROM);
	CZetMapMemory(DrvPalRAM,			0xf000, 0xf5ff, MAP_ROM);
	CZetSetWriteHandler(mnight_main_write);
	CZetSetReadHandler(ninjakd2_main_read);
	CZetClose();

//	ninjakd2_sound_init();

	DrvDoReset();
	DrvCalculatePalette();
	nBurnFunction = DrvOverDraw;
	drawWindow(0,192,192,2,62);	
	return 0;
}

INT32 RobokidInit()
{
	DrvInitSaturnS(0);
	DrvDraw = RobokidDraw;
	AllMem = NULL;
	MemIndex(0);

	if ((AllMem = (UINT8 *)BurnMalloc(MALLOC_MAX)) == NULL) return 1;
	memset((void *)0x2F0000, 0, 0x9c00);
	memset(AllMem, 0, MALLOC_MAX);
	MemIndex(0);
	
	make_lut();
	{
		if (BurnLoadRom(DrvZ80ROM0 + 0x10000,  0, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x20000,  1, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x30000,  2, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x40000,  3, 1)) return 1;
		memcpyl (DrvZ80ROM0, DrvZ80ROM0 + 0x10000, 0x10000);

//		if (BurnLoadRom(DrvZ80ROM1 + 0x10000,  4, 1)) return 1;
//		memcpy (DrvZ80ROM1, DrvZ80ROM1 + 0x10000, 0x10000);
		UINT8 *ss_vram 		= (UINT8 *)SS_SPRAM;
		UINT8 *DrvGfxROM1	= (UINT8 *)(ss_vram+0x1100); // sprites
	
		if (BurnLoadRom(DrvGfxROM1 + 0x00000,  6, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x10000,  7, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x20000,  8, 1)) return 1;
		if (BurnLoadRom(DrvGfxROM1 + 0x30000,  9, 1)) return 1;
// pour sprites
		DrvGfxDecode(DrvGfxROM1, 0x40000, 2);
		swapFirstLastColor(DrvGfxROM1,0x0f,0x40000);
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

// bg2 utile
		DrvGfxDecode((UINT8*)tmp, 0x80000, 3);
		swapFirstLastColor(tmp,0x0f,0x80000);
// tile 0xc00 à vider 
//		memset(&tmp[0x60000],0x00,0x80);

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
		swapFirstLastColor(tmp,0x0f,0x70000);

//DrvGfxROM4 trouver 400k de ram !!!
		tmp = (UINT8*)DrvGfxROM4Data1;
		if (BurnLoadRom(tmp + 0x00000, 25, 1)) return 1;
		if (BurnLoadRom(tmp + 0x10000, 26, 1)) return 1;
		if (BurnLoadRom(tmp + 0x20000, 27, 1)) return 1;
		if (BurnLoadRom(tmp + 0x30000, 28, 1)) return 1;
		if (BurnLoadRom(tmp + 0x40000, 29, 1)) return 1;
		if (BurnLoadRom(tmp + 0x50000, 30, 1)) return 1;

		DrvGfxDecode((UINT8*)tmp, 0x60000, 3);
		swapFirstLastColor(tmp,0x0f,0x60000);
//text
		UINT8 *DrvGfxROM0	 	= (UINT8 *)cache;// fg
		if (BurnLoadRom(DrvGfxROM0 + 0x00000,  5, 1)) return 1;
		swapFirstLastColor(DrvGfxROM0,0x0f,0x10000);
	}

	CZetInit2(1,CZ80Context);
	CZetOpen(0);
	CZetMapMemory(DrvZ80ROM0,			0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory(DrvZ80ROM0 + 0x10000, 0x8000, 0xbfff, MAP_ROM);
	CZetMapMemory(DrvPalRAM,			0xc000, 0xc7ff, MAP_ROM);
//	CZetMapMemory(DrvFgRAM,				0xc800, 0xcfff, MAP_RAM);
	CZetMapMemory(DrvFgRAM,				0xc800, 0xcfff, MAP_ROM);
	CZetMapMemory(DrvBgRAM2,			0xd000, 0xd3ff, MAP_RAM);
	CZetMapMemory(DrvBgRAM1,			0xd400, 0xd7ff, MAP_RAM);
	CZetMapMemory(DrvBgRAM0,			0xd800, 0xdbff, MAP_RAM);
	CZetMapMemory(DrvZ80RAM0,			0xe000, 0xf9ff, MAP_RAM);
	CZetMapMemory(DrvSprRAM,			0xfa00, 0xffff, MAP_RAM);
	CZetSetWriteHandler(robokid_main_write);
	CZetSetReadHandler(ninjakd2_main_read);
	CZetClose();

//	ninjakd2_sound_init();

	DrvDoReset();
	DrvCalculatePalette();
	nBurnFunction = DrvOverDraw;
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
#if 0
INT32 OmegafInit()
{
	DrvInitSaturnS(0);
	DrvDraw = Ninjakd2Draw;
	AllMem = NULL;
	MemIndex(0);

	if ((AllMem = (UINT8 *)BurnMalloc(MALLOC_MAX)) == NULL) return 1;
	memset((void *)0x2F0000, 0, 0x9c00);
	memset(AllMem, 0, MALLOC_MAX);
	MemIndex(0);
	
	make_lut();	
	
	{
		if (BurnLoadRom(DrvZ80ROM0 + 0x10000,  0, 1)) return 1;
		if (BurnLoadRom(DrvZ80ROM0 + 0x30000,  1, 1)) return 1;
		memcpyl (DrvZ80ROM0, DrvZ80ROM0 + 0x10000, 0x10000);

//		if (BurnLoadRom(DrvZ80ROM1 + 0x10000,  2, 1)) return 1;
//		memcpyl (DrvZ80ROM1, DrvZ80ROM1 + 0x10000, 0x10000);
		
		UINT8 *ss_vram 		= (UINT8 *)SS_SPRAM;
		UINT8 *DrvGfxROM1	= (UINT8 *)(ss_vram+0x1100); // sprites

		if (BurnLoadRom(DrvGfxROM1 + 0x00000,  4, 1)) return 1;

// pour sprites
		DrvGfxDecode(DrvGfxROM1, 0x40000, 2);
		swapFirstLastColor(DrvGfxROM1,0x0f,0x40000);

//DrvGfxROM3
		UINT8 *tmp = (UINT8*)0x00270000;
		if (BurnLoadRom(tmp + 0x00000,  6, 1)) return 1;

// bg2 utile
		DrvGfxDecode((UINT8*)tmp, 0x80000, 3);
		swapFirstLastColor(tmp,0x0f,0x80000);
		
//DrvGfxROM2
		tmp = (UINT8*)0x00200000;

		if (BurnLoadRom(tmp + 0x00000,  5, 1)) return 1;

		DrvGfxDecode((UINT8*)tmp, 0x70000, 3);
		swapFirstLastColor(tmp,0x0f,0x70000);
/*
//DrvGfxROM4
		tmp = (UINT8*)DrvGfxROM4Data1;

		if (BurnLoadRom(tmp + 0x00000,  7, 1)) return 1;
		DrvGfxDecode((UINT8*)tmp, 0x60000, 3);
		swapFirstLastColor(tmp,0x0f,0x60000);
*/
//text
		UINT8 *DrvGfxROM0	 	= (UINT8 *)cache;// fg
		if (BurnLoadRom(DrvGfxROM0 + 0x00000,  3, 1)) return 1;
		swapFirstLastColor(DrvGfxROM0,0x0f,0x10000);
		
//		DrvGfxDecode(DrvGfxROM0, 0x08000, 0);
//		DrvGfxDecode(DrvGfxROM1, 0x20000, 2);
//		DrvGfxDecode(DrvGfxROM2, 0x80000, 2);
//		DrvGfxDecode(DrvGfxROM3, 0x80000, 2);
//		DrvGfxDecode(DrvGfxROM4, 0x80000, 2);
	}

	CZetInit2(1,CZ80Context);
	CZetOpen(0);
	CZetMapMemory(DrvZ80ROM0,			0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory(DrvZ80ROM0 + 0x10000,	0x8000, 0xbfff, MAP_ROM);
	CZetMapMemory(DrvBgRAM0,			0xc400, 0xc7ff, MAP_RAM);
	CZetMapMemory(DrvBgRAM1,			0xc800, 0xcbff, MAP_RAM);
	CZetMapMemory(DrvBgRAM2,			0xcc00, 0xcfff, MAP_RAM);
	CZetMapMemory(DrvFgRAM,				0xd000, 0xd7ff, MAP_RAM);
	CZetMapMemory(DrvPalRAM,			0xd800, 0xdfff, MAP_ROM);
	CZetMapMemory(DrvZ80RAM0,			0xe000, 0xf9ff, MAP_RAM);
	CZetMapMemory(DrvSprRAM,			0xfa00, 0xffff, MAP_RAM);
	CZetSetWriteHandler(omegaf_main_write);
	CZetSetReadHandler(omegaf_main_read);
	CZetClose();

//	ninjakd2_sound_init();
//	BurnYM2203SetRoute(0, BURN_SND_YM2203_YM2203_ROUTE,   0.80, BURN_SND_ROUTE_BOTH);
//	BurnYM2203SetRoute(1, BURN_SND_YM2203_YM2203_ROUTE,   0.80, BURN_SND_ROUTE_BOTH);

//	GenericTilesInit();

	DrvDoReset();

	return 0;
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
void initLayersS(UINT8 game)
{
    Uint16	CycleTb[]={
		0xffff,0x4567,  //A0 // nbg1 et 2 ok
		0xffff, 0x4ff7,	//A1
		0x0123,0x4ff7 ,  //B0
		0xffff,0x4ff7   //B1
	};
 	SclConfig	scfg;

	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN1WORD;
	scfg.flip          = SCL_PN_12BIT; 
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.patnamecontrl =  0x0000;// VRAM A0 ?~I?t?Z?b?g 
	scfg.plate_addr[0] = (Uint32)ss_map;
	scfg.plate_addr[1] = (Uint32)ss_map;
	SCL_SetConfig(SCL_NBG2, &scfg); // fg

	scfg.dispenbl      = ON;
	scfg.pnamesize   = SCL_PN2WORD;
	scfg.charsize      = SCL_CHAR_SIZE_2X2;//OK du 1*1 surtout pas toucher
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.patnamecontrl =  0x0000;// VRAM A0 +0x10000?~I?t?Z?b?g 
	scfg.plate_addr[0] = (Uint32)ss_map2;
	scfg.plate_addr[1] = (Uint32)ss_map2;
	scfg.plate_addr[2] = (Uint32)ss_map2;
	scfg.plate_addr[3] = (Uint32)ss_map2;

	SCL_SetConfig(SCL_NBG1, &scfg); // bg0
	scfg.dispenbl      = ON;
	scfg.plate_addr[0] = (Uint32)ss_font; // bg1
	scfg.plate_addr[1] = (Uint32)ss_font;
	scfg.plate_addr[2] = (Uint32)ss_font;
	scfg.plate_addr[3] = (Uint32)ss_font;
		
	if(!game)
	{
		SCL_SetConfig(SCL_NBG0, &scfg);
		scfg.plate_addr[0] = (Uint32)ss_map3; // bg2
		scfg.plate_addr[1] = (Uint32)ss_map3;
		scfg.plate_addr[2] = (Uint32)ss_map3;
		scfg.plate_addr[3] = (Uint32)ss_map3;
		SCL_SetConfig(SCL_NBG3, &scfg);
	}
	else
	{
		scfg.bmpsize 	   = SCL_BMP_SIZE_512X256;
		scfg.datatype 	   = SCL_BITMAP;
		scfg.mapover	   = SCL_OVER_0;
		scfg.coltype	   = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;	
		SCL_SetConfig(SCL_NBG0, &scfg);
		
		scfg.dispenbl      = OFF;
		SCL_SetConfig(SCL_NBG3, &scfg);
	}

	SCL_SetCycleTable(CycleTb);

	memset((void *)SCL_VDP2_VRAM_B0,0x00,0x2000);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initPosition()
{
	SCL_Open();
	ss_reg->n1_move_y =  (32<<16) ;
//	ss_reg->n1_move_x =  (-8<<16) ;
	ss_reg->n2_move_x =   (-8);
	ss_reg->n2_move_y =  32 ;
	ss_reg->n1_move_x =  (0<<16) ;
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initColorsS(UINT8 game)
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	if(game)
	{
		colAddr = (Uint16*)SCL_AllocColRam(SCL_NBG1,ON);
	}
	else
	{
		colAddr = (Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
		SCL_AllocColRam(SCL_NBG3,OFF);		
	}
	
	SCL_AllocColRam(SCL_SPR,OFF);   // 0x200 pour sprites atomic robokid // 0x100 pour njkid 2
	SCL_AllocColRam(SCL_NBG2,OFF); // 0x300 pour fg atomic robokid // 0x200 pour njkid 2
	
	if(game)
	{
		SCL_AllocColRam(SCL_NBG3,OFF);
		SCL_AllocColRam(SCL_NBG0,OFF);
		SCL_SetColRam(SCL_NBG0,8,8,palette);	
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DrvInitSaturnS(UINT8 game)
{
	//nBurnSprites  = 196;
	nBurnSprites  = 100;
	cleanSprites();
	
	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite           = (SprSpCmd *)SS_SPRIT;

 	SS_MAP  = ss_map	=(Uint16 *)SCL_VDP2_VRAM_B1+0x0000;		    // fg
	SS_MAP2 = ss_map2	=(Uint16 *)SCL_VDP2_VRAM_B1+0x1000;			// bg0
	if(game)
		SS_FONT = ss_font	=(Uint16 *)SCL_VDP2_VRAM_B0;			// window
	else
		SS_FONT = ss_font	=(Uint16 *)SCL_VDP2_VRAM_B1+0xc000;			// bg1
	ss_map3				=(Uint16 *)SCL_VDP2_VRAM_B1+0xa000;			// bg2

	SS_CACHE= cache		=(Uint8  *)SCL_VDP2_VRAM_A0;

	SS_SET_S0PRIN(4);
	if(game)	
		SS_SET_N0PRIN(7);
	else
		SS_SET_N0PRIN(3);
		
	SS_SET_N2PRIN(6);
	SS_SET_N1PRIN(2);
	SS_SET_N3PRIN(5);
	initPosition();

	initColorsS(game);
	initLayersS(game);

	initSprites(264-1,216-1,0,0,8,-32);
	
	if(game)
	{
		FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Loading. Please Wait",24,40);	
		drawWindow(0,192,192,2,62);
	}
//	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
	//*(unsigned int*)OPEN_CSH_VAR(SOUND_LEN) = 128;
}
//-------------------------------------------------------------------------------------------------------------------------------------
void tile16x16toSaturn (unsigned int num, unsigned char *pDest)
{
	unsigned int c;
	for (c = 0; c < num; c++) 
	{
		unsigned char new_tile[128];
		UINT8 *dpM = pDest + (c * 128);
		memcpyl(new_tile,dpM,128);
		unsigned int i=0,j=0,k=0;

		for (k=0;k<2;k++)
		{
			for (i=0;i<32;i+=4,j+=8)
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
			dpM+=64;
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------

 INT32 DrvExit()
{
	nBurnFunction = NULL;
	wait_vblank();	
	DrvDoReset();
	CZetExit2();

	previous_bank[0]=previous_bank[1]=previous_bank[2]=0;
	CZ80Context	=  NULL;
	DrvDraw = NULL;

	RamEnd = DrvZ80ROM0 = NULL;
	DrvGfxROM4Data1 = DrvZ80RAM0 = DrvSprRAM = DrvPalRAM = NULL;
	DrvFgRAM = DrvBgRAM = DrvBgRAM0 = DrvBgRAM1 = DrvBgRAM2 = NULL;
	cram_lut = NULL;
	free(AllMem);
	AllMem = NULL;
	ss_map3 = NULL;

/*
	memset(DrvJoy1,0x00,8);
	memset(DrvJoy2,0x00,8);
	memset(DrvJoy3,0x00,8);
	DrvDips[0] = DrvDips[1] = 0;
	DrvInputs[0] = DrvInputs[1] = DrvInputs[2] = 0;
*/
	nSoundBufferPos=0;
	cleanDATA();
	cleanBSS();
	
	return 0;
}
/*
void draw_mnight_bg_layer()
{
//	INT32 xscroll = (scrollx[0] + 0) & 0x1ff;
//	INT32 yscroll = (scrolly[0] + 32) & 0x1ff;

	for (INT32 offs = 0; offs < 32 * 32; offs++)
	{
//		UINT32 sx = (offs & 0x1f);
//		UINT32 sy = (offs / 0x20)<<5;

		UINT32 attr  = DrvBgRAM[offs*2+1];
		UINT32 code  = DrvBgRAM[offs*2+0] + ((attr & 0xc0) << 2) + ((attr & 0x10) << 6);
		UINT32 flipy = attr & 0x20;
 
		ss_map2[offs*2] = (attr & 0x0f);
		ss_map2[(offs*2)+1] = 0x400 + (code<<2) & 0xffff;		
	}
}
*/
 void draw_robokid_bg_layer(INT32 width)
{
//	if (tilemap_enable[sel] == 0) return;
	UINT32 wide = (width) ? 128 : 32;

	UINT8 attr;
	UINT32 code;
		
	for (UINT32 offs = 0; offs < wide * 32; offs++)
	{
		UINT32 sx = (offs % wide);
		UINT32 sy = (offs / wide);

		UINT32 ofst = (sx & 0x0f) + (sy * 16) + ((sx & 0x70) * 0x20);
		UINT32 offs2 = (sx | sy <<5)*2;			
//-----------------------------------------------------------------------------------------------------------------
// nbg1
		attr = DrvBgRAM0[ofst * 2 + 1];
		code = DrvBgRAM0[ofst * 2 + 0] + ((attr & 0xc0) << 2) + ((attr & 0x10) << 7) + ((attr & 0x20) << 5);
		
		if(bg_cache[0][code]==0)
		{
			bg_cache[0][code]=(nextTile[0]<<2);
			UINT8 *toto = (UINT8*)0x00200000+(code*128);
			memcpyl((UINT8 *)cache+0x08000+bg_cache[0][code]*32,&toto[0],128);

			if(nextTile[0]++>0xfff)
				nextTile[0]=0;
		}
		ss_map2[offs2] = (attr & 0x0f);
		ss_map2[offs2+1] = (0x400+bg_cache[0][code]);
//-----------------------------------------------------------------------------------------------------------------
// nbg0			
		attr = DrvBgRAM1[ofst * 2 + 1];
		code = DrvBgRAM1[ofst * 2 + 0] + ((attr & 0xc0) << 2) + ((attr & 0x10) << 7) + ((attr & 0x20) << 5);
		
		if(bg_cache[1][code]==0)
		{
			bg_cache[1][code]=(nextTile[1]<<2);
			UINT8 *toto = (UINT8*)0x00270000+(code*128);
			memcpyl((UINT8 *)cache+0x28000+bg_cache[1][code]*32,&toto[0],128);

			if(nextTile[1]++>0xfff)
				nextTile[1]=0;
		}
		ss_font[offs2] = (attr & 0x0f);
		ss_font[offs2+1] = (0x1400+bg_cache[1][code]);			
//-----------------------------------------------------------------------------------------------------------------
// nbg3			
		attr = DrvBgRAM2[ofst * 2 + 1];
		code = DrvBgRAM2[ofst * 2 + 0] + ((attr & 0xc0) << 2) + ((attr & 0x10) << 7) + ((attr & 0x20) << 5);
		
		if(bg_cache[2][code]==0)
		{
			bg_cache[2][code]=(nextTile[2]<<2);
			UINT8 *toto = (UINT8*)DrvGfxROM4Data1+(code*128);
			memcpyl((UINT8 *)cache+0x48000+bg_cache[2][code]*32,&toto[0],128);

			if(nextTile[2]++>0xfff)
				nextTile[2]=0;
		}
		ss_map3[offs2] = (attr & 0x0f);
		ss_map3[offs2+1] = (0x2400+bg_cache[2][code]); // si 0x20000 pour bg2
//-----------------------------------------------------------------------------------------------------------------
	}
}
/*
inline void draw_bg_layer()
{


	for (UINT32 offs = 0; offs < 32 * 32; offs++)
	{
//		UINT32 sx = (offs & 0x1f);
//		UINT32 sy = (offs / 0x20)<<5;

		UINT32 attr  = DrvBgRAM[offs*2+1];
		UINT32 code  = DrvBgRAM[offs*2+0] + ((attr & 0xc0) << 2);
		UINT32 flipx = attr & 0x10;
		UINT32 flipy = attr & 0x20;
 
		ss_map2[offs*2] = (attr & 0x0f);
		ss_map2[(offs*2)+1] = 0x400 + (code<<2) & 0x0fff;
	}
}

void draw_fg_layer()
{
	for (UINT32 offs = (32 * 4); offs < (32 * 32) - (32 * 4); offs++)
	{
		UINT32 sx = (offs & 0x1f);

		UINT8 attr  = DrvFgRAM[offs*2+1];
		UINT16 code  = DrvFgRAM[offs*2+0] + ((attr & 0xc0) << 2);
		UINT32 off2s = sx | (offs / 0x20) <<6;

		code |= (attr & 0x20) << 3;
		ss_map[off2s] = (attr & 0x0f) <<12 | code & 0xfff;
	}
}
*/
SprSpCmd ss_spriteBuff[96];

 void draw_sprites(UINT32 robokid)
{
	int const big_xshift = robokid ? 0 : 1;
	int const big_yshift = robokid ? 1 : 0;

	UINT8* sprptr = DrvSprRAM + 11;
	unsigned int sprites_drawn = 0;
	SprSpCmd *ss_spritePtr;

	
	if(!overdraw_enable)
	{
		ss_spritePtr = &ss_sprite[3];		
//		nBurnSprites  = 100;
		cleanSprites();
		
//		SprSpCmd *ss_spritePtrBuff;
//		ss_spritePtrBuff = &ss_spriteBuff[0];		

	// en cas d'overdraw : envoyer ceux en cours + ceux de la frame précédente
	// à faire !!!

		while (1)
		{
			if (sprptr[2] & 0x02)
			{
				int sx = sprptr[1] - ((sprptr[2] & 0x01) << 8);
				int sy = sprptr[0];

				int code = sprptr[3] + ((sprptr[2] & 0xc0) << 2) + ((sprptr[2] & 0x08) << 7);

				unsigned int flip = (sprptr[2] & 0x30);
				unsigned int color = sprptr[4] & 0x0f;
				unsigned int big = (sprptr[2] & 0x04) >> 2;
/*	
				if (*flipscreen)
				{
					sx = 240 - 16*big - sx;
					sy = 240 - 16*big - sy;
//					flipx ^= 1;
//					flipy ^= 1;
				}
*/	
				if (big)
				{
					unsigned int flipx = (sprptr[2] & 0x10) >> 4;
					unsigned int flipy = (sprptr[2] & 0x20) >> 5;
				
					code &= ~3;
//					code ^= flipx << big_xshift;
//					code ^= flipy << big_yshift;
					code ^= flipy << big_xshift;
					code ^= flipx << big_yshift;
				}

				for (unsigned int y = 0; y <= big; ++y)
				{
					for (unsigned int x = 0; x <= big; ++x)
					{
						unsigned int tile = code ^ (y << big_xshift) ^ (x << big_yshift);

						ss_spritePtr->control		= ( JUMP_NEXT | FUNC_NORMALSP) | flip;
						ss_spritePtr->drawMode		= ( ECD_DISABLE | COMPO_REP);

						ss_spritePtr->ax			= sx + (x<<4);
						ss_spritePtr->ay			= sy + (y<<4);
						ss_spritePtr->charSize		= 0x210;
						ss_spritePtr->color			= color<<4;
						ss_spritePtr->charAddr		= 0x220+(tile<<4);
//						memcpy(ss_spritePtrBuff,ss_spritePtr,sizeof(SprSpCmd));						
						*ss_spritePtr++;
//						*ss_spritePtrBuff++;						

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
	else
	{
		ss_spritePtr = &ss_sprite[3+96];
//		nBurnSprites  = 196;

		SprSpCmd *ss_spritePtrBuff;
		ss_spritePtrBuff = &ss_spriteBuff[0];
		memcpy(&ss_sprite[3],&ss_spriteBuff[0],sizeof(SprSpCmd)*96);

		while (1)
		{
			if (sprptr[2] & 0x02)
			{
				int sx = sprptr[1] - ((sprptr[2] & 0x01) << 8);
				int sy = sprptr[0];

				int code = sprptr[3] + ((sprptr[2] & 0xc0) << 2) + ((sprptr[2] & 0x08) << 7);

				unsigned int flip = (sprptr[2] & 0x30);
				unsigned int color = sprptr[4] & 0x0f;
				unsigned int big = (sprptr[2] & 0x04) >> 2;
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
					unsigned int flipx = (sprptr[2] & 0x10) >> 4;
					unsigned int flipy = (sprptr[2] & 0x20) >> 5;					
					code &= ~3;
					code ^= flipx << big_xshift;
					code ^= flipy << big_yshift;
				}

				for (unsigned int y = 0; y <= big; ++y)
				{
					for (unsigned int x = 0; x <= big; ++x)
					{
						unsigned int tile = code ^ (x << big_xshift) ^ (y << big_yshift);

						ss_spritePtr->control		= ( JUMP_NEXT | FUNC_NORMALSP) | flip;
						ss_spritePtr->drawMode		= ( ECD_DISABLE | COMPO_REP);

						ss_spritePtr->ax			= sx + (x<<4);
						ss_spritePtr->ay			= sy + (y<<4);
						ss_spritePtr->charSize		= 0x210;
						ss_spritePtr->color			= color<<4;
						ss_spritePtr->charAddr		= 0x220+(tile<<4);
						memcpy(ss_spritePtrBuff,ss_spritePtr,sizeof(SprSpCmd));

						*ss_spritePtr++;
						*ss_spritePtrBuff++;

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
}

void Ninjakd2Draw()
{
	ss_reg->n1_move_x =  (scrollx[0]-8)<<16;
	ss_reg->n1_move_y =  (scrolly[0]+32)<<16;
	
//	if (tilemap_enable[0])
//		draw_bg_layer();
	draw_sprites(0); // 0x100

//	draw_fg_layer();
}


void MnightDraw()
{
	ss_reg->n1_move_x =  (scrollx[0]-8)<<16;
	ss_reg->n1_move_y =  (scrolly[0]+32)<<16;	

	draw_sprites(0);

//	if (tilemap_enable[0] == 0)
//		BurnTransferClear();

//	if (tilemap_enable[0])
//		draw_mnight_bg_layer();

//	draw_copy_sprites();

//	draw_fg_layer();
}

void RobokidDraw()
{
//	DrvGfxROM2	 	= (UINT8 *)cache+0x08000;// bg1 //Next; Next += 0x100000;
//	DrvGfxROM3	 	= (UINT8 *)cache+0x28000;//bg2  //Next; Next += 0x100000;
//	DrvGfxROM4		= (UINT8 *)cache+0x58000;//bg3 // Next; Next += 0x100000;
/*
	if (tilemap_enable[0])prepare_robokid_bg_layer(0, DrvBgRAM0, 0);
	if (tilemap_enable[1])prepare_robokid_bg_layer(1, DrvBgRAM1, 0);
	if (tilemap_enable[2])prepare_robokid_bg_layer(2, DrvBgRAM2, 0);
*/
	draw_robokid_bg_layer(0);

	draw_sprites(1);
	
//	draw_fg_layer();

	ss_reg->n0_move_x =  (scrollx[1]-8)<<16;
	ss_reg->n0_move_y =  (scrolly[1]+32)<<16;
	ss_reg->n1_move_x =  (scrollx[0]-8)<<16;
	ss_reg->n1_move_y =  (scrolly[0]+32)<<16;
	ss_reg->n3_move_x =  (scrollx[2]-8);
	ss_reg->n3_move_y =  (scrolly[2]+32);

	for (UINT32 offs = 0; offs < (64 * 32); offs++)
	{
		UINT32 sx = (offs & 0x3f);

//		UINT8 attr  = DrvFgRAM[offs*2+1];
//		UINT16 code  = DrvFgRAM[offs*2+0] + ((attr & 0xc0) << 2);
		UINT16 off2s = sx | (offs / 0x40) <<6;

		if((sx<32)&& (offs / 0x40)<28)
		{

		}
		else
			ss_map[off2s] = 0x5;
	}
	
}

 inline void DrvClearOpposites(UINT8* nJoystickInputs)
{ // for active LOW
	if ((*nJoystickInputs & 0x03) == 0x00) {
		*nJoystickInputs |= 0x03;
	}
	if ((*nJoystickInputs & 0x0c) == 0x00) {
		*nJoystickInputs |= 0x0c;
	}
}

int vbt=0;

 INT32 DrvFrame()
{
//	if (DrvReset) {
//		DrvDoReset();
//	}
//
#if 0
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
#endif
	{
	//	if(overdraw_enable)
//			SCL_SetFrameInterval(-1);
		
		memset (DrvInputs, 0xff, 3);

		for (UINT32 i = 0; i < 8; i++) {
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

	for (UINT32 i = 0; i < nInterleave; i++)
	{

		CZetOpen(0);
		CZetRun(nCycleSegment);

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


	DrvDraw();
	
	return 0;
}
//-------------------------------------------------------------------------------------------------
void make_lut()
{
	for (UINT32 i = 0; i < 0x1000; i++) 
	{
		UINT32 r = i >> 8;
		UINT32 g = (i >> 4) & 0xf;
		UINT32 b = (i >> 0) & 0xf;

		r |= r << 4;
		g |= g << 4;
		b |= b << 4;

		cram_lut[i] = BurnHighCol(r, g, b, 0);
	}
}
//-------------------------------------------------------------------------------------------------