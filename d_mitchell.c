#define CZ80 1
//#define RAZE 1
//#define LOOP 1
//#define SWITCH 1
#include "d_mitchell.h"

#define PCM_MUSIC 1
#define nInterleave  32
#define nBurnSoundLen 192
#define nSegmentLength nBurnSoundLen / nInterleave
#define 	nCyclesTotal 4500000 / 60
#define nCyclesSegment nCyclesTotal / nInterleave
#define DrvNumColours 0x800 * 2
#define 	DrvTileMask 0x7fff

int ovlInit(char *szShortName)
{
	cleanBSS();

	struct BurnDriver nBurnDrvPang = {
		"pang", "mitch",
		"Pang (World)",
		PangRomInfo, PangRomName, PangInputInfo, NULL,
		PangInit, DrvExit, DrvFrame//NULL
	};

	struct BurnDriver nBurnDrvSpang = {
		"spang", "mitch",
		"Super Pang (World 900914)",
		SpangRomInfo, SpangRomName, PangInputInfo, NULL,
		SpangInit, DrvExit, DrvFrame//NULL
	};

    if (strcmp(nBurnDrvPang.szShortName, szShortName) == 0)
	{
		memcpy(shared,&nBurnDrvPang,sizeof(struct BurnDriver));
	}
	else
	{
		memcpy(shared,&nBurnDrvSpang,sizeof(struct BurnDriver));
	}
//	ss_map   = (Uint16 *)SS_MAP;
	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}

/*static*/ inline void DrvClearOpposites(unsigned char* nJoystickInputs)
{
	if ((*nJoystickInputs & 0x30) == 0x30) {
		*nJoystickInputs &= ~0x30;
	}
	if ((*nJoystickInputs & 0xc0) == 0xc0) {
		*nJoystickInputs &= ~0xc0;
	}
}

static void DrvMakeInputs()
{

	for (unsigned int i = 0; i < 12; i++) DrvInput[i] = 0x00;

	for (unsigned int  i = 0; i < 8; i++) {
		DrvInput[ 0] |= (DrvInputPort0[ i] & 1) << i;
		DrvInput[ 1] |= (DrvInputPort1[ i] & 1) << i;
		DrvInput[ 2] |= (DrvInputPort2[ i] & 1) << i;
		DrvInput[ 3] |= (DrvInputPort3[ i] & 1) << i;
		DrvInput[ 4] |= (DrvInputPort4[ i] & 1) << i;
		DrvInput[ 5] |= (DrvInputPort5[ i] & 1) << i;
		DrvInput[ 6] |= (DrvInputPort6[ i] & 1) << i;
		DrvInput[ 7] |= (DrvInputPort7[ i] & 1) << i;
		DrvInput[ 8] |= (DrvInputPort8[ i] & 1) << i;
		DrvInput[ 9] |= (DrvInputPort9[ i] & 1) << i;
		DrvInput[10] |= (DrvInputPort10[i] & 1) << i;
		DrvInput[11] |= (DrvInputPort11[i] & 1) << i;
	}
/*
	if (DrvInputType == DRV_INPUT_TYPE_BLOCK) {
	//while(1);
		if (DrvInputPort11[0]) DrvDial1 -= 0x04;
		if (DrvInputPort11[1]) DrvDial1 += 0x04;
		if (DrvDial1 >= 0x100) DrvDial1 = 0;
		if (DrvDial1 < 0) DrvDial1 = 0xfc;
		
		if (DrvInputPort11[2]) DrvDial2 -= 0x04;
		if (DrvInputPort11[3]) DrvDial2 += 0x04;
		if (DrvDial2 >= 0x100) DrvDial2 = 0;
		if (DrvDial2 < 0) DrvDial2 = 0xfc;
	} 
	else*/ 
//		{
//		if (DrvInputType != DRV_INPUT_TYPE_MAHJONG)
		{
			DrvClearOpposites(&DrvInput[1]);
			DrvClearOpposites(&DrvInput[2]);
		}
}

inline void PangMemIndex()
{
	extern unsigned int _malloc_max_ram;
	UINT8 *Next; Next = (unsigned char *)&_malloc_max_ram;
	memset(Next, 0, MALLOC_MAX);
	
	DrvZ80Rom			= Next; Next += 0x50000;
	DrvZ80Code			= (unsigned char *)LOWADDR;//Next; Next += 0x50000;
	DrvSoundRom			= (unsigned char *)0x00250000; //Next; Next += 0x20000;
	DrvZ80Ram			= Next; Next += 0x02000;
	DrvPaletteRam		= Next; Next += 0x01000;
	DrvAttrRam			= Next; Next += 0x00800;
	RamStart			= Next-0xd000;
	DrvVideoRam			= Next; Next += 0x01000;
	DrvSpriteRam		= Next; Next += 0x01000;

	CZ80Context			= (UINT8 *)Next; Next += sizeof(cz80_struc);
	map_offset_lut		= (UINT16 *)Next; Next += 2048 * sizeof(UINT16);
	charaddr_lut		= (UINT16 *)Next; Next += 2048 * sizeof(UINT16);
	map_lut				= (UINT16 *)Next; Next += 256 * sizeof(UINT16);
	cram_lut			= (UINT16 *)Next; Next += 4096 * sizeof(UINT16);
	pBuffer				= (int *)Next; Next += nBurnSoundRate * sizeof(int);
	MSM6295Context		= (int *)Next; Next += 4 * 0x1000 * sizeof(int);
// allocation de 808960 octets, 790ko
}

inline void oki_bankswitch(INT32 bank)
{
	DrvOkiBank = bank;

	MSM6295SetBank(0, DrvSoundRom + (DrvOkiBank * 0x40000), 0x00000, 0x3ffff);
}

/*static*/ int DrvDoReset()
{
#ifdef CZ80
//	CZetOpen(0);
	DrvRomBank = 0;

	if (DrvHasEEPROM) {
		CZetMapMemory2(DrvZ80Code + 0x10000 + (DrvRomBank * 0x4000), DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000), 0x8000, 0xbfff, MAP_ROM);
	} else {
		CZetMapMemory((unsigned char *)(DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000)), 0x8000, 0xbfff, MAP_ROM);
	}	
	
	CZetReset();
//	CZetClose();
#else
#ifdef RAZE
	DrvRomBank = 0;
	z80_init_memmap();
 	z80_map_read (0x8000, 0xbfff, DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000)); 

	if (DrvHasEEPROM) {
	 	z80_map_fetch (0x8000, 0xbfff, DrvZ80Code + 0x10000 + (DrvRomBank * 0x4000)); 
//		z80_map_read  (0x8000, 0xbfff, DrvZ80Code + 0x10000 + (DrvRomBank * 0x4000)); //2 fetch 
		z80_map_read  (0x8000, 0xbfff, DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000)); //2 fetch 
	} else {
		z80_map_fetch (0x8000, 0xbfff, DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000)); //2 fetch 
	}
	z80_end_memmap();  
	z80_reset();
#else
	ZetOpen(0);
	DrvRomBank = 0;
	ZetMapArea(0x8000, 0xbfff, 0, DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000));
	if (DrvHasEEPROM) {
		ZetMapArea(0x8000, 0xbfff, 2, DrvZ80Code + 0x10000 + (DrvRomBank * 0x4000), DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000));
	} else {
		ZetMapArea(0x8000, 0xbfff, 2, DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000));
	}
	ZetReset();
	ZetClose();
#endif	
#endif
//	BurnYM2413Reset();
	MSM6295Reset(0);
	oki_bankswitch(0);

	if (DrvHasEEPROM) EEPROMReset();
	
	DrvPaletteRamBank = 0;
	DrvVideoBank = 0;
//	DrvDialSelected = 0;
	return 0;
}

/*static*/ unsigned char __fastcall MitchellZ80Read(unsigned short a)
{
	if (a >= 0xc000 && a <= 0xc7ff) {
		return DrvPaletteRam[(a - 0xc000) + (DrvPaletteRamBank ? 0x800 : 0x000)];
	}
	
	if (a >= 0xd000 && a <= 0xdfff) {
//		int Offset = a - 0xd000;
		if (DrvVideoBank) {
			return DrvSpriteRam[a&0x0fff];
		} else {
			return DrvVideoRam[a&0x0fff];
		}
	}
	return 0;
}

/*static*/ void __fastcall MitchellZ80Write(unsigned short a, unsigned char d)
{
	if (a >= 0xc000 && a <= 0xc7ff) 
	{
// soustraction plus court que masque (voir asm)	d�pend des cas � tester plus
		a = (a - 0xc000) + (DrvPaletteRamBank ? 0x800 : 0x000);
//		a = (a & 0x07ff) + (DrvPaletteRamBank ? 0x800 : 0x000);
		if(DrvPaletteRam[a] != d)
		{
			DrvPaletteRam[a] = d;
			color_dirty = 1;
		}
		return;
	}
#ifndef LOOP
	if (a >= 0xc800 && a <= 0xcfff) 
	{
		if(RamStart[a] != d)
		{
			RamStart[a] = d;
			int x = map_offset_lut[a&0x7ff];
			ss_map2[x] = map_lut[d]; //((d & 0x80)<<7) | d & 0x7f;
		}
		return;
	}
#endif

	if (a >= 0xd000 && a <= 0xdfff) 
	{
		if (DrvVideoBank) {
			DrvSpriteRam[a&0x0fff] = d;
		} else {
			if(RamStart[a]!=d)
			{
				RamStart[a] = d;
#ifndef LOOP
				a&=~1;
				unsigned int Code = RamStart[a + 0] + (RamStart[a + 1] << 8);
				Code &= DrvTileMask;
				ss_map2[map_offset_lut[(a>>1)&0x7ff]+1] = Code;
#else
				bg_dirtybuffer[a>>1] = 1;
#endif
			}
		}
		return;
	}

	if(a==0xe172)
	{
		DrvZ80Ram[a-0xe000]=d;
//				FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"   ",80,130);	
//				FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)itoa(d),80,130);	
//			if(current_pcm!=d && (d==0 || (d >=0x20 && d <=0x3D)))
			if(current_pcm!=d && (d==0 || (d >=0x20 && d <=0x3D)))
			{
				PlayStreamPCM(d,current_pcm);
				current_pcm = d;
			}
		return;
	}

	if (a >= 0xe100 && a <= 0xe1ff) 
	{
		DrvZ80Ram[a-0xe000]=d;
	}
}

/*static*/ unsigned char __fastcall MitchellZ80PortRead(unsigned short a)
{
	a &= 0xff;
	
	switch (a) {
		case 0x00: {
			return 0xff - DrvInput[0];
			}
		
		case 0x01: {
			return 0xff - DrvInput[1];
		}
		
		case 0x02: {
			return 0xff - DrvInput[2];
		}
		
		case 0x03: {
			return DrvDip[0];
		}
		
		case 0x04: {
			return DrvDip[1];
		}
		
		case 0x05: {
			INT32 Bit = DrvHasEEPROM ? (EEPROMRead() & 0x01) << 7 : 0x80;
			Bit |= 0x01;
			Bit |= 0x08;
			if (DrvInput5Toggle) {
//				Bit |= 0x01;
			} else {
				Bit ^= 0x08;
				Bit ^= 0x01;
			}
			
//			if (DrvPort5Kludge) Bit ^= 0x08;
			
			return ((0xff - DrvInput[3]) & 0x76) | Bit;
		}

		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #1 Port Read => %02X\n"), a);
		}
	}

	return 0xff;
}

/*static*/ void __fastcall MitchellZ80PortWrite(unsigned short a, unsigned char d)
{
	a &= 0xff;
	
	switch (a) {
		case 0x00: {
			DrvFlipScreen = d & 0x04;
/*			if (DrvOkiBank != (d & 0x10)) {
				DrvOkiBank = d & 0x10;
				if (DrvOkiBank) {
					memcpyl(MSM6295ROM, DrvSoundRom + 0x40000, 0x40000);
				} else {
					memcpyl(MSM6295ROM, DrvSoundRom + 0x00000, 0x40000);
				}
			}*/
			if (DrvOkiBank != (d & 0x10)>>4) 
			{
				DrvOkiBank = (d & 0x10)>>4;
				oki_bankswitch(DrvOkiBank);
			}

			DrvPaletteRamBank = d & 0x20;
			return;
		}
		
		case 0x01: {
/*			switch (DrvInputType) 
			{
				case DRV_INPUT_TYPE_BLOCK: 
				{
					if (d == 0x08) 
					{
						DrvDial[0] = DrvDial1;
						DrvDial[1] = DrvDial2;
					} 
					else 
					{
						if (d == 0x80) 
						{
							DrvDialSelected = 0;
						} 
						else 
						{
							DrvDialSelected = 1;
						}
					}
					DrvDialSelected = 0;
					return;
				}
			}*/
//			DrvDialSelected = 0;
			return;
		}
		
		case 0x02: {
			DrvRomBank = d & 0x0f;
#ifdef CZ80
			if (DrvHasEEPROM) {
				CZetMapMemory2(DrvZ80Code + 0x10000 + (DrvRomBank * 0x4000), DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000), 0x8000, 0xbfff, MAP_ROM);
			} else {
				CZetMapMemory((unsigned char *)(DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000)), 0x8000, 0xbfff, MAP_ROM);
			}
#else

#ifdef RAZE
			z80_map_fetch (0x8000, 0xbfff, DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000)); 
			if (DrvHasEEPROM) {
				z80_map_fetch (0x8000, 0xbfff, DrvZ80Code + 0x10000 + (DrvRomBank * 0x4000)); 
		//		z80_map_read  (0x8000, 0xbfff, DrvZ80Code + 0x10000 + (DrvRomBank * 0x4000)); //2 fetch 
				z80_map_read  (0x8000, 0xbfff, DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000)); //2 fetch 
			} else {
				z80_map_read (0x8000, 0xbfff, DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000)); //2 fetch 
			}
#else
			ZetMapArea(0x8000, 0xbfff, 0, DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000));
			if (DrvHasEEPROM) {
				ZetMapArea(0x8000, 0xbfff, 2, DrvZ80Code + 0x10000 + (DrvRomBank * 0x4000), DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000));
			} else {
				ZetMapArea(0x8000, 0xbfff, 2, DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000));
			}
#endif
#endif
			return;
		}
		
		case 0x03: {
//			BurnYM2413Write(1, d);
			return;
		}
		
		case 0x04: {
//			BurnYM2413Write(0, d);
			return;
		}
		
		case 0x05: {
			MSM6295Command(0, d);
			return;
		}
		
		case 0x06:{
			// nop
			return;
		}
		
		case 0x07: {
			DrvVideoBank = d;
			return;
		}
		
		case 0x08 :{
			if (DrvHasEEPROM) EEPROMSetCSLine(d ? 0 : 1);
			return;
		}
		
		case 0x10 :{
			if (DrvHasEEPROM) EEPROMSetClockLine(d ? 0 : 1);
			return;
		}
		
		case 0x18 :{
			if (DrvHasEEPROM) EEPROMWriteBit(d);
			return;
		}
		
		default: {
//			bprintf(PRINT_NORMAL, _T("Z80 #1 Port Write => %02X, %02X\n"), a, d);
		}
	}
}

// Kabuki - we use the module from the CPS-1 Q-Sound games
extern void kabuki_decode(unsigned char *src, unsigned char *dest_op, unsigned char *dest_data, int base_addr, int length, int swap_key1, int swap_key2, int addr_key, int xor_key);

static void mitchell_decode(int swap_key1, int swap_key2, int addr_key, int xor_key)
{
	UINT8 *rom = DrvZ80Rom;
	UINT8 *decrypt = DrvZ80Code;
	UINT32 numbanks = (0x50000 - 0x10000) / 0x4000;

	kabuki_decode(rom, decrypt, rom, 0x0000, 0x8000, swap_key1, swap_key2, addr_key, xor_key);

	rom += 0x10000;
	decrypt += 0x10000;
	for (UINT32 i = 0; i < numbanks; i++)
		kabuki_decode(rom + i * 0x4000,decrypt + i * 0x4000, rom + i * 0x4000, 0x8000, 0x4000, swap_key1, swap_key2, addr_key, xor_key);
}

/*static*/// void mgakuen2_decode() { mitchell_decode(0x76543210, 0x01234567, 0xaa55, 0xa5); }
static void pang_decode()     { mitchell_decode(0x01234567, 0x76543210, 0x6548, 0x24); }
static void spang_decode()    { mitchell_decode(0x45670123, 0x45670123, 0x5852, 0x43); }
/*static*/// void block_decode()    { mitchell_decode(0x02461357, 0x64207531, 0x0002, 0x01); }

static const unsigned char spang_default_eeprom[128] = {
	0x00, 0x02, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01,
	0xCD, 0x81, 0x0E, 0x10, 0xFD, 0x78, 0x88, 0x81, 0x4D, 0x2E, 0x53, 0xC9, 0xC9, 0xC9, 0xC9, 0xC9,
	0x20, 0x4D, 0x41, 0x44, 0x45, 0x20, 0x49, 0x4E, 0x20, 0x4A, 0x41, 0x50, 0x41, 0x4E, 0x2E, 0x20,
	0x20, 0x20, 0x53, 0x55, 0x50, 0x45, 0x52, 0x20, 0x50, 0x41, 0x4E, 0x47, 0x20, 0x45, 0x31, 0x20,
	0x20, 0x20, 0x43, 0x41, 0x50, 0x43, 0x4F, 0x4D, 0x20, 0x31, 0x39, 0x39, 0x30, 0x2E, 0x20, 0x20,
	0x20, 0x50, 0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D, 0x65, 0x64, 0x20, 0x42, 0x79, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x4D, 0x61, 0x73, 0x61, 0x74, 0x73, 0x75, 0x67, 0x75, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x53, 0x68, 0x69, 0x6E, 0x6F, 0x68, 0x61, 0x72, 0x61, 0x2E, 0x20, 0x20
};

static void MitchellMachineInit()
{
#ifdef CZ80
	CZetInit2(1,CZ80Context);
//	CZetInit(1);
//	CZetOpen(0);
	CZetSetReadHandler(MitchellZ80Read);
	CZetSetWriteHandler(MitchellZ80Write);
	CZetSetInHandler(MitchellZ80PortRead);
	CZetSetOutHandler(MitchellZ80PortWrite);
	CZetMapMemory2(DrvZ80Code + 0x00000, DrvZ80Rom + 0x00000, 0x0000, 0x7fff, MAP_ROM);
	CZetMapMemory2(DrvZ80Code + 0x10000, DrvZ80Rom + 0x10000, 0x8000, 0xbfff, MAP_ROM);
	
	CZetMapArea(0xc800, 0xcfff, 0, DrvAttrRam                               );
#ifdef LOOP
	CZetMapArea(0xc800, 0xcfff, 1, DrvAttrRam                               );
#endif
	CZetMapArea(0xc800, 0xcfff, 2, DrvAttrRam                               );
	CZetMapArea(0xe000, 0xffff, 0, DrvZ80Ram                                );
//	CZetMapArea(0xe000, 0xffff, 1, DrvZ80Ram                                );
	CZetMapArea(0xe000, 0xe0ff, 1, DrvZ80Ram                               );
	CZetMapArea(0xe200, 0xffff, 1, DrvZ80Ram+0x200                     );
	CZetMapArea(0xe000, 0xffff, 2, DrvZ80Ram                                );
	CZetMemEnd();
//	CZetClose();
#else

#ifdef RAZE
	z80_init_memmap();
 	z80_map_read  (0x0000, 0x7fff, DrvZ80Rom  + 0x00000); //2 read
 	z80_map_fetch (0x0000, 0x7fff, DrvZ80Code + 0x00000); //1 write
	z80_map_read  (0x0000, 0x7fff, DrvZ80Rom  + 0x00000); //0 fetch

	z80_map_read  (0x8000, 0xbfff, DrvZ80Rom  + 0x10000); //0 read
	z80_map_fetch (0x8000, 0xbfff, DrvZ80Code + 0x10000); 
	z80_map_read  (0x8000, 0xbfff, DrvZ80Rom  + 0x10000); //1 write

	z80_map_fetch (0xc800, 0xcfff, DrvAttrRam); //0 read
	z80_map_read  (0xc800, 0xcfff, DrvAttrRam); //1 write
	z80_map_write (0xc800, 0xcfff, DrvAttrRam); //2 fetch 

	z80_map_fetch (0xe000, 0xffff, DrvZ80Ram); //0 read
	z80_map_read  (0xe000, 0xffff, DrvZ80Ram); //1 write
	z80_map_write (0xe000, 0xffff, DrvZ80Ram); //2 fetch 

	z80_add_write(0xc000, 0xc7ff, 1, (void *)&MitchellZ80Read);
	z80_add_write(0xd000, 0xdfff, 1, (void *)&MitchellZ80Read);
	z80_add_read (0xc000, 0xc7ff, 1, (void *)&MitchellZ80Write);
	z80_add_read (0xd000, 0xdfff, 1, (void *)&MitchellZ80Write);

	z80_end_memmap();

	z80_set_in((unsigned char (*)(unsigned short))&MitchellZ80PortRead);
	z80_set_out((void (*)(unsigned short, unsigned char))&MitchellZ80PortWrite);
#endif	
#endif
//	BurnYM2413Init(4000000, 1.0);
//	BurnYM2413IncreaseVolume(200);
	MSM6295ROM = (unsigned char *)0x00250000; //DrvSoundRom;
	memset(MSM6295ROM,0x00,0x40000);

//	MSM6295Init(0, 1000000 / 132, 10.0, 1);
//	MSM6295ROM = (unsigned char *)0x00250000; //DrvSoundRom;
	MSM6295Init(0, 1000000 / 132, 10.0, 0, MSM6295Context);// � remettre
//	MSM6295Init(0, 8000, 100, 0);
	MSM6295ROM = (unsigned char *)0x00250000; //DrvSoundRom;

	EEPROMInit(&MitchellEEPROMIntf);
	DrvHasEEPROM = 1;

//	DrvTileMask = 0x7fff;
//	DrvNumColours = 0x800;
	DrvInput5Toggle = 0;
	nBurnFunction = DrvCalcPalette;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static int PangInit()
{
	int nRet = 0;
	DrvInitSaturn();
	sfx_list = &sfx_pang[0];
	PangMemIndex();
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"make_lut   ",80,130);	
	make_lut();

static INT32 CharPlaneOffsets[4]         = { 0x400004, 0x400000, 4, 0 };
static INT32 CharXOffsets[8]               = { 0, 1, 2, 3, 8, 9, 10, 11 };
static INT32 CharYOffsets[8]               = { 0, 16, 32, 48, 64, 80, 96, 112 };
static INT32 SpritePlaneOffsets[4]        = { 0x100004, 0x100000, 4, 0 };
static INT32 SpriteXOffsets[16]            = { 0, 1, 2, 3, 8, 9, 10, 11, 256, 257, 258, 259, 264, 265, 266, 267 };
static INT32 SpriteYOffsets[16]            = { 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240 };

	unsigned char *DrvTempRom = (unsigned char *)LOWADDR;
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"BurnLoadRom   ",80,130);	
	nRet = BurnLoadRom(DrvZ80Rom  + 0x00000,  0, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvZ80Rom  + 0x10000,  1, 1); if (nRet != 0) return 1;

//	memset(DrvTempRom, 0xff, 0xc0000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  2, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000,  3, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x80000,  4, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0xa0000,  5, 1); if (nRet != 0) return 1;

//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"GfxDecode4Bpp   ",80,130);	
	GfxDecode4Bpp(0x4000, 4, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, cache);

//1024Ko en 4bpp/2048 en 8bpp 
	memset(DrvTempRom, 0xff, 0x40000);
//	memset4_fast(DrvSprites, 0xff, 0x40000); // ou 0x80000
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  6, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000,  7, 1); if (nRet != 0) return 1;

	UINT8 *ss_vram		= (UINT8 *)SS_SPRAM;
	UINT8 *DrvSprites	= (UINT8 *)(ss_vram+0x1100);
	GfxDecode4Bpp(0x0800, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x200, DrvTempRom, DrvSprites);
	swapFirstLastColor(DrvSprites,0x0f,0x40000);

//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"pang_decode   ",80,130);	
	pang_decode();
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"MitchellMachineInit   ",80,130);	
	MitchellMachineInit();
	nRet = BurnLoadRom(DrvSoundRom + 0x00000, 8, 1); if (nRet != 0) return 1;
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"DrvDoReset   ",80,130);	
	DrvDoReset();
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"stmInit   ",80,130);	
//-------------------------------------------------
#ifdef PCM_MUSIC
	stmInit();
	SetStreamPCM();
	PCM_Start(pcmStream);
#endif	
//-------------------------------------------------
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"PCM_Start   ",80,130);	
	return 0;
}

static int SpangInit()
{
	int nRet = 0;
	DrvInitSaturn();
	sfx_list = &sfx_spang[0];
	
//	Mem = NULL;
	PangMemIndex();
	make_lut();

static INT32 CharPlaneOffsets[4]         = { 0x400004, 0x400000, 4, 0 };
static INT32 CharXOffsets[8]               = { 0, 1, 2, 3, 8, 9, 10, 11 };
static INT32 CharYOffsets[8]               = { 0, 16, 32, 48, 64, 80, 96, 112 };
static INT32 SpritePlaneOffsets[4]        = { 0x100004, 0x100000, 4, 0 };
static INT32 SpriteXOffsets[16]            = { 0, 1, 2, 3, 8, 9, 10, 11, 256, 257, 258, 259, 264, 265, 266, 267 };
static INT32 SpriteYOffsets[16]            = { 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240 };

	unsigned char *DrvTempRom = (unsigned char *)LOWADDR;
 // VBT � remettre
	nRet = BurnLoadRom(DrvZ80Rom  + 0x00000,  0, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvZ80Rom  + 0x10000,  1, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvZ80Rom  + 0x30000,  2, 1); if (nRet != 0) return 1;
	
	memset(DrvTempRom, 0xff, 0xc0000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  3, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000,  4, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x80000,  5, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0xa0000,  6, 1); if (nRet != 0) return 1;
	GfxDecode4Bpp(0x4000, 4, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, cache);

	memset(DrvTempRom, 0xff, 0x40000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  7, 1); if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000,  8, 1); if (nRet != 0) return 1;

	UINT8 *ss_vram		= (UINT8 *)SS_SPRAM;
	UINT8 *DrvSprites	= (UINT8 *)(ss_vram+0x1100);
	GfxDecode4Bpp(0x0800, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x200, DrvTempRom, DrvSprites);
	swapFirstLastColor(DrvSprites,0x0f,0x40000);
	
	memset4_fast(DrvTempRom, 0x00, 0x50000);
	spang_decode();
	MitchellMachineInit();
	
	memset4_fast(DrvSoundRom, 0x00, 0x50000);
	nRet = BurnLoadRom(DrvSoundRom + 0x00000,  9, 1); if (nRet != 0) return 1;
	
if (!EEPROMAvailable()) EEPROMFill(spang_default_eeprom, 0, 128);
	
//	DrvNVRamSize = 0x0080;
//	DrvNVRamAddress = 0x0000;
	DrvDoReset();
//-------------------------------------------------
#ifdef PCM_MUSIC
	stmInit();
	SetStreamPCM();
	PCM_Start(pcmStream);
#endif	
//-------------------------------------------------
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------
inline void initLayers()
{
    Uint16	CycleTb[]={
		0x01ff, 0x4ff, //A0
		0xffff, 0xf5ee,	//A1
		0xff4e,0xffff,   //B0
		0xfff5, 0xffff  //B1
	};
 	SclConfig	scfg;
// 3 nbg
	scfg.dispenbl      = ON;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD; //2word
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.plate_addr[0] = (Uint32)ss_map2;
	scfg.plate_addr[1] = 0x00;
	SCL_SetConfig(SCL_NBG0, &scfg);

	scfg.dispenbl 		 = OFF;		  // VBT � decommenter pour ne pas afficher l'�cran de texte
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
//	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 		 = SCL_BITMAP;
	scfg.mapover		 = SCL_OVER_0;
	scfg.plate_addr[0]	 = (Uint32)ss_font;

// 3 nbg	
	SCL_SetConfig(SCL_NBG1, &scfg); 

	scfg.dispenbl 		 = OFF;
	SCL_SetConfig(SCL_NBG2, &scfg); 
	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void initPosition()
{
	SCL_Open();
	ss_reg->n0_move_y =  (8<<16) ;
	ss_reg->n0_move_x =  (80<<16) ;
	SCL_Close();
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colBgAddr = (Uint16*)SCL_AllocColRam(SCL_NBG0,ON);
	SCL_SetColRam(SCL_NBG1,8,4,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void dummy(void)
{
	return;
}
//-------------------------------------------------------------------------------------------------------------------------------------
inline void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);

#ifdef DEBUG
	GFS_SetErrFunc(errGfsFunc, NULL);
	PCM_SetErrFunc(errPcmFunc, NULL);
#endif
	nBurnSprites  = 131;
	nBurnLinescrollSize = 0x0;
//	TVOFF;
	SS_MAP2    = ss_map2  =(Uint16 *)SCL_VDP2_VRAM_A1;
	SS_FONT    = ss_font     = (Uint16 *)NULL; //SCL_VDP2_VRAM_B0;// remttre null
//	SS_FONT    = ss_font     = (Uint16 *)SCL_VDP2_VRAM_B0;// remttre null
	SS_MAP      = ss_map    = (Uint16 *)NULL;
	SS_CACHE = cache       =(Uint8  *)SCL_VDP2_VRAM_A0;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
//	ss_scl			= (Fixed32 *)SS_SCL;

//	 lp.h_enbl=OFF;	 
//	TVMD = 0x0011;

	ss_regs->tvmode = 0x8011;//0x0013;//0x0001; ou 0x0011
//	ss_reg->linecontrl = (lp.h_enbl << 1) & 0x0002;
//	SclProcess =1;
#ifdef LOOP
	memset(bg_dirtybuffer,1,4096);
#endif
	color_dirty = 1;

	initLayers();
	initPosition();
	initColors();

	initSprites(352-1,240-1,0,0,-80,-16);

	SprSpCmd *ss_spritePtr;
	ss_spritePtr = &ss_sprite[3];
	
	for (UINT32 i = 0; i <128; i++) 
	{
		ss_spritePtr->control   = ( JUMP_NEXT | FUNC_NORMALSP);
		ss_spritePtr->drawMode  = ( ECD_DISABLE | COMPO_REP);	// 16 couleurs
		ss_spritePtr->charSize  = 0x210;  //0x100 16*16
		ss_spritePtr++;		
	}

	{
	ss_regs->tvmode = 0x8011;//0x0013;//0x0001; ou 0x0011
	SS_SET_S0PRIN(7);
	SS_SET_N1PRIN(7);

//	SS_SET_N1PRIN(4);
	SS_SET_N2PRIN(5);		
	SS_SET_N0PRIN(6);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
static int DrvExit()
{
	nBurnFunction = NULL;
	wait_vblank();
	DrvDoReset();	
	MSM6295Exit(0);
#ifdef CZ80
	CZetExit2();
#else
#ifndef RAZE
	ZetExit();
#endif
#endif
	MSM6295ROM = NULL;
	if (DrvHasEEPROM) EEPROMExit();
#ifdef PCM_MUSIC
	STM_ResetTrBuf(stm);
	PCM_MeStop(pcmStream);
	PCM_DestroyStmHandle(pcmStream);
	stmClose(stm);
#endif
/*	MSM6295Context = NULL;
	CZ80Context = DrvZ80Rom = DrvZ80Code = DrvSoundRom = DrvZ80Ram = NULL;
	RamStart = DrvPaletteRam = DrvAttrRam = DrvVideoRam = DrvSpriteRam = MSM6295ROM = NULL;
	charaddr_lut = map_offset_lut = map_lut = cram_lut = NULL;
	pBuffer = NULL;
//	free(Mem);
//	Mem = NULL;
*/
#ifdef LOOP
	bg_dirtybuffer = NULL;
#endif

	cleanDATA();
	cleanBSS();

	nSoundBufferPos=0;
	return 0;
}

inline unsigned char pal4bit(unsigned char bits)
{
	bits &= 0x0f;
	return ((bits << 4) | bits)>>3;
}

static void make_lut(void)
{
    UINT32 j,mx, my;
    for(j = 0; j < 4096; j++)
    {
		int r = pal4bit(j >> 8);
		int g = pal4bit(j >> 4);
		int b = pal4bit(j >> 0);

        cram_lut[j] =RGB(r,g,b);
    }

	j = 0;
	for (my = 0; my < 64; my+=2) 
	{
		for (mx = 0; mx < 128; mx+=2) 
		{
			map_offset_lut[j] = mx|(my<<6);
			j++;
		}
	}

	for (UINT32 i = 0; i<0x800; i++)
	{
		charaddr_lut[i] = 0x220+(i<<4);
	}

	for (UINT32 i = 0; i<256; i++)
	{
		map_lut[i] = ((i & 0x80)<<7) | i & 0x7f;
	}
}


static void DrvCalcPalette()
{
	if (color_dirty==1)
	{
		for (UINT32 i = 0; i < DrvNumColours; i += 2) 
		{
			unsigned int Val = DrvPaletteRam[i & ~1] + (DrvPaletteRam[i | 1] << 8);
			colBgAddr[i >> 1] = cram_lut[Val];//CalcCol(Val);
		}
		color_dirty = 0;
	}
//	DrvMakeInputs();
}
#ifdef LOOP
/*static*/ void DrvRenderBgLayer()
{
	unsigned int Code, Attr, x, TileIndex = 0;
	UINT16 *map;
	
	for (TileIndex=0;TileIndex<0x800 ; TileIndex++)
	{
			if(bg_dirtybuffer[TileIndex]==1)
			{
				bg_dirtybuffer[TileIndex]=0;
				Attr = DrvAttrRam[TileIndex];
				Code = DrvVideoRam[(TileIndex<<1) + 0] + (DrvVideoRam[(TileIndex<<1) + 1] << 8);
				Code &= DrvTileMask;

				x = map_offset_lut[TileIndex];
				map = &ss_map2[x];				
				map[0] = ((Attr & 0x80)<<7) | Attr & 0x7f;
				map[1] = Code;	
			}
	}
}
#endif
static void DrvRenderSpriteLayer()
{
	SprSpCmd *ss_spritePtr = &ss_sprite[3];
	UINT32 Code ;
	UINT32 Attr ;

	for (int Offset = 0x1000 - 0x40; Offset >= 0; Offset -= 0x20) 
	{
		Code = DrvSpriteRam[Offset + 0];
		Attr = DrvSpriteRam[Offset + 1];
		Code += (Attr & 0xe0) << 3;

		ss_spritePtr->ax		= DrvSpriteRam[Offset + 3] + ((Attr & 0x10) << 4);
		ss_spritePtr->ay		= ((DrvSpriteRam[Offset + 2] + 8) & 0xff);// - 8;
		ss_spritePtr->charAddr	= charaddr_lut[Code];
		ss_spritePtr->color     = (Attr & 0x0f)<<4;
		ss_spritePtr++;
	}
}
#ifdef LOOP
static void DrvDraw()
{			 
	SPR_RunSlaveSH((PARA_RTN*)DrvRenderBgLayer, NULL);
	DrvRenderSpriteLayer();
	SPR_WaitEndSlaveSH();
}
#endif

/*static*/ int DrvFrame()
{
#ifndef LOOP
	SPR_RunSlaveSH((PARA_RTN*)DrvRenderSpriteLayer, NULL);
#endif
//	int nInterleave = 10;

//	if (DrvReset) DrvDoReset();

	DrvMakeInputs();

//	nCyclesTotal = 8000000 / 60;
//	nCyclesTotal = 4500000 / 60;
	UINT32 nCyclesDone = 0;

	DrvInput5Toggle = 0;
	
#ifdef CZ80
//	CZetNewFrame();
#endif
	for (unsigned int i = 0; i < nInterleave; i++) 
	{
#ifdef CZ80
		nCyclesDone += CZetRun(nCyclesSegment);

		if (i == 0 || i == 29) 
		{
			CZetSetIRQLine(0, CZET_IRQSTATUS_AUTO);
			DrvInput5Toggle = (i == 29);
		}
#else
		nNext = (i + 1) * nCyclesTotal / nInterleave;
		nCyclesSegment = nNext - nCyclesDone;
		nCyclesDone += z80_emulate(nCyclesSegment);

		if (i == 4) 
		{
			z80_raise_IRQ(0);
			nCyclesDone += z80_emulate(500);
//			z80_lower_IRQ(0);
//			z80_emulate(0);
		}
		if (i == 7) DrvInput5Toggle = 1;
		if (i == 9) 
		{
			z80_raise_IRQ(0);
			nCyclesDone += z80_emulate(500);
//			z80_lower_IRQ(0);
//			z80_emulate(0);
		}
#endif
	}
#ifdef PCM_MUSIC
	playMusic(&pcmStream);
#endif

	signed short *nSoundBuffer = (signed short *)0x25a20000;
	MSM6295RenderVBT(0, &nSoundBuffer[nSoundBufferPos], nBurnSoundLen);
	nSoundBufferPos+=nBurnSoundLen;
  
	if(nSoundBufferPos>=RING_BUF_SIZE/2)
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos);
		nSoundBufferPos=0;
		PCM_Task(pcm); // bon emplacement
	}
//	PCM_Task(pcm); 

#ifndef LOOP
//	DrvRenderSpriteLayer();
	SPR_WaitEndSlaveSH();
#else
	DrvDraw();
#endif
	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------