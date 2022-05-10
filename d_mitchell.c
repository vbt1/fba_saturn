#define CZ80 1
//#define RAZE 1
//#define LOOP 1
//#define SWITCH 1
//#define PCM_MUSIC 1

#include "d_mitchell.h"
//#pragma GCC optimize ("O3")
#define PONY 1

#ifdef PONY	

#include "saturn/pcmstm.h"
#include <sega_sys.h>

int pcm1=0;
Sint16 *nSoundBuffer=NULL;
extern unsigned short frame_x;
extern unsigned short frame_y;
#endif


#define nInterleave  32
#define nBurnSoundLen 128
#define nSegmentLength nBurnSoundLen / nInterleave
#define nCyclesTotal 4500000 / 60
#define nCyclesSegment nCyclesTotal / nInterleave
#define DrvNumColours 0x800 * 2
#define DrvTileMask 0x7fff

#ifndef PCM_MUSIC
SFX *sfx_list = NULL;
#endif



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
	ss_reg   = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
	
	return 0;
}
inline void DrvClearOpposites(unsigned char* nJoystickInputs)
{
	if ((*nJoystickInputs & 0x30) == 0x30) {
		*nJoystickInputs &= ~0x30;
	}
	if ((*nJoystickInputs & 0xc0) == 0xc0) {
		*nJoystickInputs &= ~0xc0;
	}
	return 0;
}

void DrvMakeInputs()
{

	for (int i = 0; i < 12; i++) DrvInput[i] = 0x00;

	for (int  i = 0; i < 8; i++) {
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

void PangMemIndex()
{
	extern unsigned int _malloc_max_ram;
	UINT8 *Next; Next = (unsigned char *)&_malloc_max_ram;
	memset(Next, 0, MALLOC_MAX);

	MSM6295ROM			= Next; Next += 0x20000; //Next; Next += 0x20000;	
//	DrvZ80Code			= Next; Next += 0x50000;//Next; Next += 0x50000;	
	DrvZ80Rom			= Next; Next += 0x50000;
	DrvZ80Code			= (unsigned char *)LOWADDR;//Next; Next += 0x50000;
//	MSM6295ROM			= (unsigned char *)LOWADDR+0x50000; //Next; Next += 0x20000;
	DrvZ80Ram			= Next; Next += 0x02000;
	DrvPaletteRam		= Next; Next += 0x01800;
//	DrvAttrRam			= Next; Next += 0x00800;
	RamStart			= Next-0xd000;
	DrvVideoRam			= Next; Next += 0x01000;
	DrvSpriteRam		= Next; Next += 0x01000;

	CZ80Context			= (UINT8 *)Next; Next += sizeof(cz80_struc);
	map_offset_lut		= (UINT16 *)Next; Next += 2048 * sizeof(UINT16);
	charaddr_lut		= (UINT16 *)Next; Next += 2048 * sizeof(UINT16);
	map_lut				= (UINT16 *)Next; Next += 256 * sizeof(UINT16);
	cram_lut			= (UINT16 *)Next; Next += 4096 * sizeof(UINT16);
	pBuffer				= (int *)Next; Next += nBurnSoundRate * sizeof(int);
	MSM6295Context		= (int *)Next; //Next += 4 * 0x1000 * sizeof(int);
	
	make_lut();
	
// allocation de 808960 octets, 790ko
}

inline void oki_bankswitch(INT32 bank)
{
	DrvOkiBank = bank;

	MSM6295SetBank(0, MSM6295ROM + (bank * 0x40000), 0x00000, 0x3ffff);
}
void DrvDoReset()
{

#ifdef CZ80
//	CZetOpen(0);
//	UINT8 DrvRomBank = 0;
//	CZetMapArea(0x8000, 0xbfff, 0, DrvZ80Rom + 0x10000 + (DrvRomBank * 0x4000));
	if (DrvHasEEPROM) {
		CZetMapMemory2(DrvZ80Code + 0x10000 , DrvZ80Rom + 0x10000 , 0x8000, 0xbfff, MAP_ROM);
	} else {
		CZetMapMemory((unsigned char *)(DrvZ80Rom + 0x10000 ), 0x8000, 0xbfff, MAP_ROM);
	}	
	
	CZetReset();
//	CZetClose();
#else
#ifdef RAZE
//	DrvRomBank = 0;
	z80_init_memmap();
 	z80_map_read (0x8000, 0xbfff, DrvZ80Rom + 0x10000); 

	if (DrvHasEEPROM) {
	 	z80_map_fetch (0x8000, 0xbfff, DrvZ80Code + 0x10000); 
//		z80_map_read  (0x8000, 0xbfff, DrvZ80Code + 0x10000 + (DrvRomBank * 0x4000)); //2 fetch 
		z80_map_read  (0x8000, 0xbfff, DrvZ80Rom + 0x10000); //2 fetch 
	} else {
		z80_map_fetch (0x8000, 0xbfff, DrvZ80Rom + 0x10000); //2 fetch 
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
//	*(Uint16 *)0x25E00000 = colBgAddr[0];
}
unsigned char __fastcall MitchellZ80Read(unsigned short a)
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
void __fastcall MitchellZ80Write(unsigned short a, unsigned char d)
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
			UINT16 x = map_offset_lut[a&0x7ff];
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
				UINT8 *r=(UINT8 *)&RamStart[a&~1];
				UINT16 Code = r[0] | (r[1] << 8);
				Code &= DrvTileMask;
				ss_map2[map_offset_lut[(a>>1)&0x7ff]+1] = Code;
#else
				bg_dirtybuffer[a>>1] = 1;
#endif
			}
		}
		return;
	}

//	DrvZ80Ram[a-0xe000]=d;

	if(a==0xe172)
/*	if(a!=0xe088)	
	if(a!=0xe030)	
	if(a!=0xe14b)	
	if(a!=0xe1bc)	
	if(a!=0xe1e8)			
		if(a!=0xfb1e)
		if(a!=0xfb02)	
		if(a!=0xfb03)			
		if(a!=0xfb2b)			
		if(a!=0xfb2a)			
		if(a!=0xfb22)			
		if(a!=0xfb23)			
		if(a!=0xfd4c)			
		if(a!=0xfc32)	*/		
//	if(a>=0xFB00 && a <=0xFBff)
	if(a>=0xe000 && a<=0xffff) // && (d==48||d==60))
	{
		DrvZ80Ram[a-0xe000]=d;

			if(current_pcm!=d && (d==0 || (d >=0x20 && d <=0x3D)))
//			if(current_pcm!=d && (d==0 || (d >=0x20 && d <=0x3D)))
//			if(d==48)
			{
/*				
				FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"   ",24,40);	
				char buffer [50];
				sprintf (buffer,"%04x %02d   ",a,d);
				if (d!=0)
				{
					
					FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)buffer,24,40);		
					while(1);
				}
*/			
#ifdef PCM_MUSIC				
				PlayStreamPCM(d,current_pcm);
#endif
#ifdef PONY

				if(current_pcm!=0x3D)
				{
					stop_pcm_stream();
				}

				if(d!=0x3D) // 0x3D stop
				{
			//		UpdateStreamPCM(d,&pcmStream, &paraStream);
					char pcm_file[14];

					sprintf(pcm_file, "%03d%s",d,".PCM"); 			
					start_pcm_stream((Sint8*)pcm_file, 5);		
				}
#endif
				current_pcm = d;
			}
		return;
	}

	if (a >= 0xe100 && a <= 0xe1ff) 
	{
		DrvZ80Ram[a-0xe000]=d;
	}
}
unsigned char __fastcall MitchellZ80PortRead(unsigned short a)
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
void __fastcall MitchellZ80PortWrite(unsigned short a, unsigned char d)
{
	a &= 0xff;
	
	switch (a) {
		case 0x00: {
//			DrvFlipScreen = d & 0x04;
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
			UINT8 DrvRomBank = d & 0x0f;
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

void mitchell_decode(int swap_key1, int swap_key2, int addr_key, int xor_key)
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
inline void pang_decode()     { mitchell_decode(0x01234567, 0x76543210, 0x6548, 0x24); }
inline void spang_decode()    { mitchell_decode(0x45670123, 0x45670123, 0x5852, 0x43); }
/*static*/// void block_decode()    { mitchell_decode(0x02461357, 0x64207531, 0x0002, 0x01); }

const unsigned char spang_default_eeprom[128] = {
	0x00, 0x02, 0x00, 0x01, 0x01, 0x00, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x01,
	0xCD, 0x81, 0x0E, 0x10, 0xFD, 0x78, 0x88, 0x81, 0x4D, 0x2E, 0x53, 0xC9, 0xC9, 0xC9, 0xC9, 0xC9,
	0x20, 0x4D, 0x41, 0x44, 0x45, 0x20, 0x49, 0x4E, 0x20, 0x4A, 0x41, 0x50, 0x41, 0x4E, 0x2E, 0x20,
	0x20, 0x20, 0x53, 0x55, 0x50, 0x45, 0x52, 0x20, 0x50, 0x41, 0x4E, 0x47, 0x20, 0x45, 0x31, 0x20,
	0x20, 0x20, 0x43, 0x41, 0x50, 0x43, 0x4F, 0x4D, 0x20, 0x31, 0x39, 0x39, 0x30, 0x2E, 0x20, 0x20,
	0x20, 0x50, 0x72, 0x6F, 0x67, 0x72, 0x61, 0x6D, 0x65, 0x64, 0x20, 0x42, 0x79, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x4D, 0x61, 0x73, 0x61, 0x74, 0x73, 0x75, 0x67, 0x75, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x53, 0x68, 0x69, 0x6E, 0x6F, 0x68, 0x61, 0x72, 0x61, 0x2E, 0x20, 0x20
};

void MitchellMachineInit()
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
	
#ifdef LOOP
	CZetMapMemory(DrvPaletteRam+0x1000,	0xc800, 0xcfff, MAP_RAM);
#else
	CZetMapMemory(DrvPaletteRam+0x1000,	0xc800, 0xcfff, MAP_ROM);	
#endif

	CZetMapMemory(DrvZ80Ram,	0xe000, 0xffff, MAP_ROM);
	CZetMapMemory(DrvZ80Ram,	0xe000, 0xe0ff, MAP_WRITE);
	CZetMapMemory(DrvZ80Ram+0x200, 0xe200, 0xffff, MAP_WRITE|MAP_FETCH);	
/*
	CZetMapMemory(DrvZ80Ram,	0xe000, 0xffff, MAP_ROM);
//	CZetMapMemory(DrvZ80Ram,	0xe000, 0xe0ff, MAP_WRITE);
	CZetMapMemory(DrvZ80Ram+0x0c00, 0xec00, 0xfaff, MAP_WRITE|MAP_FETCH);
	CZetMapMemory(DrvZ80Ram+0x1c00, 0xfc00, 0xffff, MAP_WRITE|MAP_FETCH);		
// 0000FB83
*/
#else

#ifdef RAZE
	z80_init_memmap();
 	z80_map_read  (0x0000, 0x7fff, DrvZ80Rom  + 0x00000); //2 read
 	z80_map_fetch (0x0000, 0x7fff, DrvZ80Code + 0x00000); //1 write
	z80_map_read  (0x0000, 0x7fff, DrvZ80Rom  + 0x00000); //0 fetch

	z80_map_read  (0x8000, 0xbfff, DrvZ80Rom  + 0x10000); //0 read
	z80_map_fetch (0x8000, 0xbfff, DrvZ80Code + 0x10000); 
	z80_map_read  (0x8000, 0xbfff, DrvZ80Rom  + 0x10000); //1 write

	z80_map_fetch (0xc800, 0xcfff, DrvPaletteRam+0x1000); //0 read
	z80_map_read  (0xc800, 0xcfff, DrvPaletteRam+0x1000); //1 write
	z80_map_write (0xc800, 0xcfff, DrvPaletteRam+0x1000); //2 fetch 

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
//	MSM6295ROM = (unsigned char *)0x00250000; //DrvSoundRom;
//	memset(MSM6295ROM,0x00,0x40000);

//	MSM6295Init(0, 1000000 / 132, 10.0, 1);
//	MSM6295ROM = (unsigned char *)0x00250000; //DrvSoundRom;
	MSM6295Init(0, 1000000 / 132, 0, MSM6295Context);// � remettre
//	MSM6295Init(0, 8000, 100, 0);
//	MSM6295ROM = (unsigned char *)0x00250000; //DrvSoundRom;

	EEPROMInit(&MitchellEEPROMIntf);
	DrvHasEEPROM = 1;

//	DrvTileMask = 0x7fff;
//	DrvNumColours = 0x800;
	DrvInput5Toggle = 0;
	nBurnFunction = DrvCalcPalette;
	
#ifdef PONY2
#include "sega_int.h"
//	sound_external_audio_enable(5, 5);
	load_drv(ADX_MASTER_2304);

wait_vblank();
#endif


}
//-------------------------------------------------------------------------------------------------------------------------------------
int PangInit()
{
	int nRet = 0;
	DrvInitSaturn();
#ifndef PONY	
	sfx_list = &sfx_pang[0];
#endif	
	PangMemIndex();
//FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"make_lut   ",80,130);	
//	make_lut();

INT32 CharPlaneOffsets[4]         = { 0x400004, 0x400000, 4, 0 };
INT32 CharXOffsets[8]               = { 0, 1, 2, 3, 8, 9, 10, 11 };
INT32 CharYOffsets[8]               = { 0, 16, 32, 48, 64, 80, 96, 112 };
INT32 SpritePlaneOffsets[4]        = { 0x100004, 0x100000, 4, 0 };
INT32 SpriteXOffsets[16]            = { 0, 1, 2, 3, 8, 9, 10, 11, 256, 257, 258, 259, 264, 265, 266, 267 };
INT32 SpriteYOffsets[16]            = { 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240 };

	unsigned char *DrvTempRom = (unsigned char *)LOWADDR;

	nRet = BurnLoadRom(DrvZ80Rom  + 0x00000,  0, 1); //if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvZ80Rom  + 0x10000,  1, 1); //if (nRet != 0) return 1;

	memset(DrvTempRom, 0xff, 0xc0000);
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  2, 1); //if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000,  3, 1); //if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x80000,  4, 1); //if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0xa0000,  5, 1); //if (nRet != 0) return 1;
	
	GfxDecode4Bpp(0x4000, 4, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, (UINT8 *)SS_CACHE);

//1024Ko en 4bpp/2048 en 8bpp 
	memset(DrvTempRom, 0xff, 0x40000);
//	memset4_fast(DrvSprites, 0xff, 0x40000); // ou 0x80000
	nRet = BurnLoadRom(DrvTempRom + 0x00000,  6, 1); //if (nRet != 0) return 1;
	nRet = BurnLoadRom(DrvTempRom + 0x20000,  7, 1); //if (nRet != 0) return 1;
	nRet = BurnLoadRom(MSM6295ROM + 0x00000, 8, 1); //if (nRet != 0) return 1;

	UINT8 *ss_vram		= (UINT8 *)SS_SPRAM;
	UINT8 *DrvSprites	= (UINT8 *)(ss_vram+0x1100);
	GfxDecode4Bpp(0x0800, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x200, DrvTempRom, DrvSprites);
	swapFirstLastColor(DrvSprites,0x0f,0x40000);

	pang_decode();
	MitchellMachineInit();
//-------------------------------------------------
#ifdef PCM_MUSIC
	stmInit();
	SetStreamPCM();
	PCM_SetPcmStreamNo(pcm, 1);
	PCM_Start(pcmStream);
#endif	
	DrvDoReset();
//-------------------------------------------------
	return 0;
}

int SpangInit()
{
	DrvInitSaturn();
#ifndef PONY	
	sfx_list = &sfx_spang[0];
#endif	
//	Mem = NULL;
	PangMemIndex();
//	make_lut();

INT32 CharPlaneOffsets[4]         = { 0x400004, 0x400000, 4, 0 };
INT32 CharXOffsets[8]               = { 0, 1, 2, 3, 8, 9, 10, 11 };
INT32 CharYOffsets[8]               = { 0, 16, 32, 48, 64, 80, 96, 112 };
INT32 SpritePlaneOffsets[4]        = { 0x100004, 0x100000, 4, 0 };
INT32 SpriteXOffsets[16]            = { 0, 1, 2, 3, 8, 9, 10, 11, 256, 257, 258, 259, 264, 265, 266, 267 };
INT32 SpriteYOffsets[16]            = { 0, 16, 32, 48, 64, 80, 96, 112, 128, 144, 160, 176, 192, 208, 224, 240 };

	unsigned char *DrvTempRom = (unsigned char *)LOWADDR;
 // VBT � remettre
	BurnLoadRom(DrvZ80Rom  + 0x00000,  0, 1); //if (nRet != 0) return 1;
	BurnLoadRom(DrvZ80Rom  + 0x10000,  1, 1); //if (nRet != 0) return 1;
	BurnLoadRom(DrvZ80Rom  + 0x30000,  2, 1); //if (nRet != 0) return 1;
	
	memset(DrvTempRom, 0xff, 0xc0000);
	BurnLoadRom(DrvTempRom + 0x00000,  3, 1); //if (nRet != 0) return 1;
	BurnLoadRom(DrvTempRom + 0x20000,  4, 1); //if (nRet != 0) return 1;
	BurnLoadRom(DrvTempRom + 0x80000,  5, 1); //if (nRet != 0) return 1;
	BurnLoadRom(DrvTempRom + 0xa0000,  6, 1); //if (nRet != 0) return 1;
	GfxDecode4Bpp(0x4000, 4, 8, 8, CharPlaneOffsets, CharXOffsets, CharYOffsets, 0x80, DrvTempRom, (UINT8 *)SS_CACHE);

	memset(DrvTempRom, 0xff, 0x40000);
	BurnLoadRom(DrvTempRom + 0x00000,  7, 1); //if (nRet != 0) return 1;
	BurnLoadRom(DrvTempRom + 0x20000,  8, 1); //if (nRet != 0) return 1;

	UINT8 *ss_vram		= (UINT8 *)SS_SPRAM;
	UINT8 *DrvSprites	= (UINT8 *)(ss_vram+0x1100);
	GfxDecode4Bpp(0x0800, 4, 16, 16, SpritePlaneOffsets, SpriteXOffsets, SpriteYOffsets, 0x200, DrvTempRom, DrvSprites);
	swapFirstLastColor(DrvSprites,0x0f,0x40000);
	
	memset(DrvTempRom, 0x00, 0x50000);
	spang_decode();
	MitchellMachineInit();
	
//	memset(MSM6295ROM, 0x00, 0x50000);
	BurnLoadRom(MSM6295ROM + 0x00000,  9, 1); //if (nRet != 0) return 1;
	
if (!EEPROMAvailable()) EEPROMFill(spang_default_eeprom, 0, 128);
	
//	DrvNVRamSize = 0x0080;
//	DrvNVRamAddress = 0x0000;
//-------------------------------------------------
#ifdef PCM_MUSIC
	stmInit();
	SetStreamPCM();
	PCM_SetPcmStreamNo(pcm, 1);
	PCM_Start(pcmStream);
#endif	
//-------------------------------------------------
	DrvDoReset();
	return 0;
}

//-------------------------------------------------------------------------------------------------------------------------------------
inline void initLayers()
{
    Uint16	CycleTb[]={
		0x01ee, 0x4ee, //A0
		0xeeee, 0xf5ee,	//A1
		0xff4e,0xeeee,   //B0
		0xfff5, 0xeeee  //B1
	};
 	SclConfig	scfg;
// 3 nbg
	scfg.dispenbl      = ON;
#ifndef PONY2	
//	scfg.dispenbl      = OFF;
	scfg.charsize      = SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize     = SCL_PN2WORD; //2word
	scfg.platesize     = SCL_PL_SIZE_1X1; // ou 2X2 ?
	scfg.coltype       = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype      = SCL_CELL;
	scfg.plate_addr[0] = (Uint32)ss_map2;
	scfg.plate_addr[1] = (Uint32)ss_map2;
#else
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 		 = SCL_BITMAP;
	scfg.mapover		 = SCL_OVER_0;
	scfg.plate_addr[0]	 = (Uint32)SS_FONT;
#endif	
	SCL_SetConfig(SCL_NBG0, &scfg);
	
scfg.dispenbl      = ON;
//	scfg.dispenbl 		 = OFF;		  // VBT � decommenter pour ne pas afficher l'�cran de texte
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 		 = SCL_BITMAP;
	scfg.mapover		 = SCL_OVER_0;
	scfg.plate_addr[0]	 = (Uint32)SS_FONT;

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
//	SCL_SetColRam(SCL_NBG1,8,4,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*
static void dummy(void)
{
	return;
}*/
//-------------------------------------------------------------------------------------------------------------------------------------
inline void DrvInitSaturn()
{
	SPR_InitSlaveSH();
//	SPR_RunSlaveSH((PARA_RTN*)dummy,NULL);

#ifdef DEBUG
	GFS_SetErrFunc(errGfsFunc, NULL);
	PCM_SetErrFunc(errPcmFunc, NULL);
#endif
	nBurnSprites  = 131;
	nBurnLinescrollSize = 0x1;
//	TVOFF;
	SS_MAP2  = ss_map2  =(Uint16 *)SCL_VDP2_VRAM_A1;
//	SS_FONT  = (Uint16 *)NULL; //SCL_VDP2_VRAM_B0;// remttre null
	SS_FONT  = (Uint16 *)SCL_VDP2_VRAM_B0;// remttre null
	SS_MAP   = (Uint16 *)NULL;
	SS_CACHE = (Uint8  *)SCL_VDP2_VRAM_A0;

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
#ifndef PONY2
	initPosition();
	initColors();
#endif
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
#ifndef PONY2		
	ss_regs->tvmode = 0x8011;//0x0013;//0x0001; ou 0x0011
	SS_SET_S0PRIN(7);
	SS_SET_N1PRIN(7);

//	SS_SET_N1PRIN(4);
	SS_SET_N2PRIN(5);		
	SS_SET_N0PRIN(6);
#else
//	ss_regs->tvmode = 0x8011;//0x0013;//0x0001; ou 0x0011	
	SS_SET_S0PRIN(0);
	SS_SET_N1PRIN(7);

//	SS_SET_N1PRIN(4);
	SS_SET_N2PRIN(0);		
	SS_SET_N0PRIN(6);	
#endif	
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
int DrvExit()
{
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
//	PCM_MeStop(pcmStream);
	PCM_DestroyStmHandle(pcmStream);
	stmClose(stm);
#endif

#ifdef PONY
//if(stm.pcm_num)

	remove_pcm_stream();
remove_raw_pcm_buffer(pcm1);
#endif

#ifdef LOOP
	bg_dirtybuffer = NULL;
#endif

#ifdef PCM_MUSIC
	PCM_SetPcmStreamNo(pcm, 0);
#endif
	//wait_vblank();
	
	//cleanDATA();
	cleanBSS();

	nSoundBufferPos=0;
	return 0;
}

inline unsigned char pal4bit(unsigned char bits)
{
	bits &= 0x0f;
	return ((bits << 4) | bits)>>3;
}

inline void make_lut(void)
{
    for(UINT32 j = 0; j < 0x1000; j++)
    {
		int r = pal4bit(j >> 8);
		int g = pal4bit(j >> 4);
		int b = pal4bit(j >> 0);

        cram_lut[j] =RGB(r,g,b);
    }

	for (UINT32 i = 0; i<0x800; i++)
	{
		*charaddr_lut++ = 0x220+(i<<4);
		map_offset_lut[i] = ((i&0x3f)|((i>>5)<<5))<<1;
	}
	charaddr_lut-=0x800;

	for (UINT32 i = 0; i<256; i++)
	{
		*map_lut++ = ((i & 0x80)<<7) | (i & 0x7f);
	}
	map_lut-=256;
}


void DrvCalcPalette()
{
#ifndef PONY2		
	if (color_dirty==1)
	{
		//UINT16 *col =(UINT16 *)colBgAddr;
		
		for (UINT32 i = 0; i < DrvNumColours; i += 2) 
		{
			UINT16 Val = DrvPaletteRam[i & ~1] + (DrvPaletteRam[i | 1] << 8);
			colBgAddr[i >> 1] = cram_lut[Val];//CalcCol(Val);
		}
		color_dirty = 0;
	}
#endif	
#ifdef PONY
	sdrv_stm_vblank_rq();
#endif	
}
#ifdef LOOP
void DrvRenderBgLayer()
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
void DrvRenderSpriteLayer()
{
	SprSpCmd *ss_spritePtr = &ss_sprite[3];
	UINT32 Code ;
	UINT32 Attr ;
	UINT8 *spram =&DrvSpriteRam[0x1000 - 0x40];

	for (UINT32 i = 0; i<128; i++) 
	{
		Code = spram[0];
		Attr = spram[1];
		Code += (Attr & 0xe0) << 3;

		ss_spritePtr->ax		= spram[3] + ((Attr & 0x10) << 4);
		ss_spritePtr->ay		= ((spram[2] + 8) & 0xff);// - 8;
		ss_spritePtr->charAddr	= charaddr_lut[Code];
		ss_spritePtr->color     = (Attr & 0x0f)<<4;
		ss_spritePtr++;
		spram-=0x20;
	}
}
#ifdef LOOP
void DrvDraw()
{		
	SPR_RunSlaveSH((PARA_RTN*)DrvRenderBgLayer, NULL);
	DrvRenderSpriteLayer();
	SPR_WaitEndSlaveSH();
}
#endif

#ifdef PONY
void DrvFrame_old();

void DrvFrame()
{
	pcm1 = add_raw_pcm_buffer(0,SOUNDRATE,nBurnSoundLen*20);

	nSoundBuffer = (Sint16 *)(SNDRAM+(m68k_com->pcmCtrl[pcm1].hiAddrBits<<16) | m68k_com->pcmCtrl[pcm1].loAddrBits);

	pcm_stream_init(SOUNDRATE, PCM_TYPE_16BIT);	
	stm.times_to_loop = 10;
	pcm_stream_host(DrvFrame_old);
}

void DrvFrame_old()
#else
void DrvFrame()
#endif
{

	
#ifndef LOOP
	SPR_RunSlaveSH((PARA_RTN*)DrvRenderSpriteLayer, NULL);
#endif
	DrvMakeInputs();

//	nCyclesTotal = 8000000 / 60;
//	nCyclesTotal = 4500000 / 60;
	UINT32 nCyclesDone = 0;

	DrvInput5Toggle = 0;

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
//		nNext = (i + 1) * nCyclesTotal / nInterleave;
//		nCyclesSegment = nNext - nCyclesDone;
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

	signed short *nSoundBuffer = (signed short *)0x25a20000+(nSoundBufferPos<<1);
	MSM6295RenderVBT(0, nSoundBuffer, nBurnSoundLen);
	nSoundBufferPos+=nBurnSoundLen;
  
	if(nSoundBufferPos>=0x1200)
	{
		PCM_NotifyWriteSize(pcm, nSoundBufferPos);
		nSoundBufferPos=0;
		PCM_Task(pcm); // bon emplacement
	}
//	PCM_Task(pcm); 
#endif
#ifdef PONY
	signed short buffer[128];
	signed short *nSoundBuffer2 = (signed short *)nSoundBuffer+(nSoundBufferPos<<1);

	MSM6295RenderVBT(0, buffer, nBurnSoundLen);
	memcpyl(nSoundBuffer2,buffer,nBurnSoundLen<<1);	
	nSoundBufferPos+=nBurnSoundLen;
	
	if(nSoundBufferPos>=nBurnSoundLen*10)
	{
		pcm_play(pcm1, PCM_SEMI, 7);
		nSoundBufferPos=0;
	}
#endif
//	pcm_stream_host(NULL);
#ifndef LOOP
//	DrvRenderSpriteLayer();
	SPR_WaitEndSlaveSH();
#else
	DrvDraw();
#endif
#ifdef PONY
	_spr2_transfercommand();

	frame_x++;
	
	 if(frame_x>=frame_y)
		wait_vblank();	
#endif
}
//-------------------------------------------------------------------------------------------------------------------------------------
