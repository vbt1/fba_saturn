// FB Alpha MSX arcade driver module, by dink. memory mapping code (megarom) from fMSX
//
// TODO:
// 1: Clean-up!
//
// Oddities:
//  VoidRunner and Milk Race freeze when selecting between kbd/joy. (VoidRunner has a kludge, but it doesn't work for Milk Race)
//  Krakout any key starts, can't get into settings

#include    "machine.h"
#include "d_msx.h"

//#define K051649 1
//#define CASSETTE 1
//#define KANJI 1

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvMSX_1942 = {
		"msx", NULL,
		"MSX1 System",
		MSX_1942RomInfo, MSX_1942RomName, MSXInputInfo, MSXDIPInfo,
		DrvInit, DrvExit, DrvFrame, NULL
	};

	memcpy(shared,&nBurnDrvMSX_1942,sizeof(struct BurnDriver));

	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void load_rom()
{
	memset (AllRam, 0, RamEnd - AllRam);

	BurnLoadRom(game + 0x0000, 0, 1);
//	set_memory_map(mapper);
	
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)GFS_IdToName(file_id),26,200);

	DrvDoReset();
#ifdef RAZE	
	z80_set_reg(Z80_REG_IR,0x00);
	z80_set_reg(Z80_REG_PC,0x0000);
	z80_set_reg(Z80_REG_SP,0x00);
	z80_set_reg(Z80_REG_IRQVector,0xff);
#endif	
	PCM_MeStop(pcm);
	memset(SOUND_BUFFER,0x00,RING_BUF_SIZE*8);
	nSoundBufferPos=0;
	PCM_MeStart(pcm);
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ UINT8 update_input1(void)
{
	unsigned int i=0,k;
	UINT8 temp = 0xFF;
	SysDevice	*device;
//	__port = PER_OpenPort();
//	PER_GetPort(__port);
	if(( device = PER_GetDeviceR( &__port[0], 0 )) != NULL )
	{
		pltriggerE[0] = pltrigger[0];
		pltrigger[0] = PER_GetTrigger( device );
		pltriggerE[0] = (pltrigger[0]) ^ (pltriggerE[0]);
		pltriggerE[0] = (pltrigger[0]) & (pltriggerE[0]);

		if((pltriggerE[0] & PER_DGT_S)!=0)
		{
			load_rom();
//			DrvDoReset();
		}

		for(i=10;i<12;i++)
		{
//			if((pltrigger[0] & pad_asign[i])!=0)
			if((pltriggerE[0] & pad_asign[i])!=0)
			{
				switch(pltriggerE[0] & pad_asign[i] )
				{
					case PER_DGT_TR:
					if (file_id<=file_max)	file_id++;
					else							file_id=2;
//#ifdef FONT
						FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,
						(Uint8 *)"            ",26,200);
						FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,
						(Uint8 *)GFS_IdToName(file_id),26,200);					
//#endif
					break;

					case PER_DGT_TL:
					if (file_id>2)	file_id--;
					else				file_id=file_max;
						FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,
						(Uint8 *)"            ",26,200);
						FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,
						(Uint8 *)GFS_IdToName(file_id),26,200);

				    default:
					break;
				}
			}
		}
	}
	else	pltrigger[0] = pltriggerE[0] = 0;

	return 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void	SetVblank2( void ){
	int			imask;


	imask = get_imask();
	set_imask(2);
//	INT_ChgMsk(INT_MSK_NULL,INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT);
	INT_ChgMsk(INT_MSK_NULL, INT_MSK_VBLK_OUT);
//	INT_SetScuFunc(INT_SCU_VBLK_IN,UsrVblankIn2);
	INT_SetScuFunc(INT_SCU_VBLK_OUT,update_input1);
//	INT_ChgMsk(INT_MSK_VBLK_IN | INT_MSK_VBLK_OUT,INT_MSK_NULL);
	INT_ChgMsk(INT_MSK_VBLK_OUT,INT_MSK_NULL);
	set_imask(imask);
	__port = PER_OpenPort();
	
}
//-------------------------------------------------------------------------------------------------------------------------------------
static inline void intkeyOn(INT32 row, INT32 bit) {
	keyRows[row] = ((keyRows[row] & 0xff) | (1 << bit));
}

static inline void intkeyOff(INT32 row, INT32 bit) {
	keyRows[row] = ((keyRows[row] & 0xff) & ~(1 << bit));
}

static UINT8 keyRowGet(INT32 row) { // for ppi to read
	if (row > 11) return 0xff;
	return ~keyRows[row];
}

static void keyInput(UINT8 kchar, UINT8 onoff) { // input from emulator
	INT32 i = 0;
	INT32 gotkey = 0;

	while (charMatrix[i][0] != '\0') {
		if (kchar == charMatrix[i][0]) {
			if (onoff) {
				intkeyOn(charMatrix[i][1], charMatrix[i][2]);
			} else {
				intkeyOff(charMatrix[i][1], charMatrix[i][2]);
			}
			gotkey = 1;
			break;
		}
		i++;
	}
}

void msxKeyCallback(UINT8 code, UINT8 KeyType, UINT8 down)
{
	static INT32 lastshifted = 0;

	//bprintf(0, _T(" %c:%S,"), code, (down==1)?"DOWN":"UP");
	if (SwapSlash && code == '/') code = 0xe0;

	if (lastshifted) memset(&keyRows, 0, sizeof(keyRows));
	keyInput(/*VK_SHIFT*/'\x10', (KeyType & 0xf0));
	keyInput(code, down);
	lastshifted = (KeyType & 0xf0);
	// Note regarding 'lastshifted'.  If shift+key is pressed (f.ex. ") and shift
	// is let up before the key is let up, windows won't send the right keyup message.
	// this causes keys to get stuck.  To kludge around this, we clear the keyboard
	// matrix-buffer when shift is let up. -dink
}

static const char *ROMNames[MAXMAPPERS + 1] =
{ 
  "KonamiGeneric 8k\0", "KonamiGeneric 16k\0", "Konami-SCC\0",
  "Konami\0", "ASCII 8k\0", "ASCII 16k\0",
  "Dooly\0", "Cross Blaim\0", "R-Type\0", "???\0"
};

#ifdef CASSETTE
#define CAS_BLOAD 1
#define CAS_RUN 2
#define CAS_CLOAD 3
#define CAS_CLOADRR 4
#define CAS_WRONGSIDE 5

static const char *CASAutoLoadTypes[] =
{
	"bload \"cas:\", r\x0d", "run \"cas:\"\x0d", "cload\x0drun\x0d",
	"cload\x0drun\x0drun\x0d", "rem Set Tape Side-A in DIPs & reboot!\x0d"
};
#endif

static INT32 InsertCart(UINT8 *cartbuf, INT32 cartsize, INT32 nSlot);
static void PageMap(INT32 CartSlot, const char *cMap); //("0:0:0:0:0:0:0:0")
static void MapMegaROM(UINT8 nSlot, UINT8 nPg0, UINT8 nPg1, UINT8 nPg2, UINT8 nPg3);

#ifdef CASSETTE
static INT32 CASAutoLoadPos = 0;
static INT32 CASAutoLoadTicker = 0;

static void CASSideChange()
{
	curtape = (CASSide) ? game2 : game;
	curtapelen = (CASSide) ? CurRomSizeB : CurRomSizeA;
	CASPos = 0;
	CASSideLast = CASSide;
}

static void CASAutoLoad()
{
	CASAutoLoadPos = 0;
	CASAutoLoadTicker = 0;
	CASFrameCounter = 0;
	CASPos = 0;
}

static void CASAutoLoadTick()
{
	if (CASAutoLoadPos == 0xff) return;

	UINT8 CASModeTmp = (CASSide) ? CAS_WRONGSIDE : CASMode;
	UINT8 c = CASAutoLoadTypes[CASModeTmp - 1][CASAutoLoadPos];
	if (!c) {
		CASAutoLoadPos = 0xff;
		return;
	}

	keyInput(c, !(CASAutoLoadTicker & 1));
	if (c == '\"' || c == ':' || c == '&' || c == '!')
		keyInput(0x10/*SHIFT*/, !(CASAutoLoadTicker & 1));
	if (CASAutoLoadTicker & 1) CASAutoLoadPos++;
	CASAutoLoadTicker++;
}

static void CASPatchBIOS(UINT8 *bios)
{
	UINT8 PatchBytes[] = { 0xe1, 0xe4, 0xe7, 0xea, 0xed, 0xf0, 0xf3, 0x00 };
	UINT8 i = 0;

	while (PatchBytes[i] != 0x00) {
		UINT8 *p = bios + PatchBytes[i];
		p[0] = 0xed;
		p[1] = 0xfe;
		p[2] = 0xc9;
		i++;
	}
}
#endif
/*
extern void (*z80edfe_callback)(Z80_Regs *Regs);

static void Z80EDFECallback(Z80_Regs *Regs)
{
	static const UINT8 TapeHeader[8] = { 0x1f, 0xa6, 0xde, 0xba, 0xcc, 0x13, 0x7d, 0x74 };
    #define Z80CF 0x01

	switch (Regs->pc.d - 2)
	{
		case 0x00e1: // TAPION (open & read header)
			{
//				bprintf(0, _T("CAS: Searching header: "));

				Regs->af.b.l |= Z80CF;
				if (CASMode) {
					while (CASPos + 8 < curtapelen) {
						if (!memcmp(curtape + CASPos, TapeHeader, 8)) {
							CASPos+=8;
//							bprintf(0, _T("Found.\n"));
							Regs->af.b.l &= ~Z80CF;
							return;
						}
						CASPos += 1;
					}
//					bprintf(0, _T("Not found.\n"));
					CASPos = 0;
					return;
				}
//				bprintf(0, _T("Tape offline.\n"));

				return;
			}

		  case 0x00e4: // TAPIN (read)
		  {
			  Regs->af.b.l |= Z80CF;

			  if (CASMode) {
				  UINT8 c = curtape[CASPos++];

				  if (CASPos > curtapelen) {
					  CASPos = 0;
				  }
				  else
				  {
					  Regs->af.b.h = c;
					  Regs->af.b.l &= ~Z80CF;
				  }
			  }

			  return;
		  }

		  case 0x00e7: // TAPIOF (stop reading from tape)
		  Regs->af.b.l &= ~Z80CF;
		  return;

		  case 0x00ea: // TAPOON (write header)
//			  bprintf(0, _T("TAPOON"));
			  return;

		  case 0x00ed: // TAPOUT (write byte)
//			  bprintf(0, _T("TAPOUT"));
			  return;

		  case 0x00f0: // TAPOOF (stop writing)
			  Regs->af.b.l &= ~Z80CF;
			  return;

		  case 0x00f3: // STMOTR (motor control)
			  Regs->af.b.l &= ~Z80CF;
			  return;
	}
}
*/

void msxinit(INT32 cart_len)
{
	for(INT32 i = 0; i < MAXSLOTS; i++) {
		ROMMask[i] = 0;
		ROMData[i] = 0;
		ROMType[i] = 0;
		SRAMData[i] = 0;
		SCCReg[i] = 0;
	}

	BIOSSLOT = DEFAULT_BIOSSLOT;
	CARTSLOTA = DEFAULT_CARTSLOTA;
	CARTSLOTB = DEFAULT_CARTSLOTB;
	RAMSLOT = DEFAULT_RAMSLOT;

	if (SwapRamslot) { // Kludge for Toshiba-EMI's Break Out! - needs ram in slot 2
		RAMSLOT = 2;
		CARTSLOTB = 3;
	}

//	bprintf(0, _T("Slots: BIOS %d RAM %d CART1 %d CART2 %d\n"), BIOSSLOT, RAMSLOT, CARTSLOTA, CARTSLOTB);

	memset(EmptyRAM, 0xff, 0x4000); // bus is pulled high for unmapped reads

	for(INT32 PSlot = 0; PSlot < 4; PSlot++) // Point all pages there by default
		for(INT32 Page = 0; Page < 8; Page++)
			MemMap[PSlot][Page] = EmptyRAM;

	RAMPages = 4; // 64k
	RAMMask = RAMPages - 1;
	RAMData = main_mem;
#ifdef CASSETTE
	if (CASMode) {
//		bprintf(0, _T("Cassette mode.\n"));
//		CZetSetEDFECallback(Z80EDFECallback);
		CASPatchBIOS(maincpu);
		CASAutoLoad();
		CASSide = 0; // Always start @ side A
		CASSideChange();
	}
#endif
	// "Insert" BIOS ROM
	ROMData[BIOSSLOT] = maincpu;
	PageMap(BIOSSLOT, "0:1:2:3:e:e:e:e");

	if (!msx_basicmode)
		InsertCart(game, cart_len, CARTSLOTA);

	PSLReg = 0;

	for (INT32 i = 0; i < 4; i++) {
		WriteMode[i] = 0;
		PSL[i] = 0;
		MemMap[RAMSLOT][i * 2] = RAMData + (3 - i) * 0x4000;
		MemMap[RAMSLOT][i * 2 + 1] = MemMap[RAMSLOT][i * 2] + 0x2000;
		RAMMapper[i] = 3 - i;
		RAM[i * 2] = MemMap[BIOSSLOT][i * 2];
		RAM[i * 2 + 1] = MemMap[BIOSSLOT][i * 2 + 1];
	}


	for (INT32 J = 0; J < MAXSLOTS; J++)
		if (((ROMMask[J] + 1) > 4) || (ROMType[J] == MAP_DOOLY))
		{
			INT32 I = ROMMask[J] + 1;
			
			if ((ROMData[J][0] == 'A') && (ROMData[J][1] == 'B')) {
				MapMegaROM(J, 0, 1, 2, 3);
			} else {
				if ((ROMData[J][(I - 2) << 13] == 'A') && (ROMData[J][((I - 2) << 13) + 1] == 'B'))
					MapMegaROM(J, I - 2, I - 1, I - 2, I - 1);
			}
		}
}

static void rtype_do_bank(UINT8 *romdata)
{
	rtype_bank_base[0] = romdata + 15 * 0x4000;
	if (rtype_selected_bank & 0x10)
	{
		rtype_selected_bank &= 0x17;
	}
	rtype_bank_base[1] = romdata + rtype_selected_bank * 0x4000;
}

static void crossblaim_do_bank(UINT8 *romdata)
{
	crossblaim_bank_base[0] = ( crossblaim_selected_bank & 2 ) ? NULL : romdata + ( crossblaim_selected_bank & 0x03 ) * 0x4000;
	crossblaim_bank_base[1] = romdata;
	crossblaim_bank_base[2] = romdata + ( crossblaim_selected_bank & 0x03 ) * 0x4000;
	crossblaim_bank_base[3] = ( crossblaim_selected_bank & 2 ) ? NULL : romdata + ( crossblaim_selected_bank & 0x03 ) * 0x4000;
}

static void Mapper_write(UINT16 address, UINT8 data)
{
	UINT8 Page = address >> 14; // pg. num
	UINT8 PSlot = PSL[Page];

	if (PSlot >= MAXSLOTS) return;

	if (!ROMData[PSlot] && (address == 0x9000))
		SCCReg[PSlot] = (data == 0x3f) ? 1 : 0;

	if (((address & 0xdf00) == 0x9800) && SCCReg[PSlot]) { // Handle Konami-SCC (+)
		UINT16 offset = address & 0x00ff;

#ifdef K051649
		if (offset < 0x80) {
			K051649WaveformWrite(offset, data);
		}
		else
#endif
			if (offset < 0xa0)	{
			offset &= 0xf;

#ifdef K051649
			if (offset < 0xa) {
				K051649FrequencyWrite(offset, data);
			}
			else if (offset < 0xf) {
				K051649VolumeWrite(offset - 0xa, data);
			}
			else {
				K051649KeyonoffWrite(data);
			}
#endif
		}

		return;
	}

	if (!ROMData[PSlot] || !ROMMask[PSlot]) return;

	switch (ROMType[PSlot])
	{
		case MAP_DOOLY:
			dooly_prot = data & 0x07;
			return;

		case MAP_CROSSBL:
			crossblaim_selected_bank = data & 3;
			if (crossblaim_selected_bank == 0) {
				crossblaim_selected_bank = 1;
			}
			crossblaim_do_bank(ROMData[PSlot]);

			return;

		case MAP_RTYPE:
			if (address >= 0x7000 && address < 0x8000)
			{
				rtype_selected_bank = data & 0x1f;
				if (rtype_selected_bank & 0x10)
				{
					rtype_selected_bank &= 0x17;
				}
				rtype_bank_base[1] = ROMData[PSlot] + rtype_selected_bank * 0x4000;
			}

		case MAP_KONGEN8:
			if ((address < 0x4000) || (address > 0xbfff)) break;
			Page = (address - 0x4000) >> 13;
			if (Page == 2) SCCReg[PSlot] = (data == 0x3f) ? 1 : 0;

			data &= ROMMask[PSlot];
			if (data != ROMMapper[PSlot][Page])
			{
				RAM[Page + 2] = MemMap[PSlot][Page + 2] = ROMData[PSlot] + (data << 13);
				ROMMapper[PSlot][Page] = data;
			}
			return;

		case MAP_KONGEN16:
			if ((address < 0x4000) || (address > 0xbfff)) break;
			Page = (address & 0x8000) >> 14;

			data = (data << 1) & ROMMask[PSlot];
			if (data != ROMMapper[PSlot][Page])
			{
				RAM[Page + 2] = MemMap[PSlot][Page + 2] = ROMData[PSlot] + (data << 13);
				RAM[Page + 3] = MemMap[PSlot][Page + 3] = RAM[Page + 2] + 0x2000;
				ROMMapper[PSlot][Page] = data;
				ROMMapper[PSlot][Page + 1] = data + 1;
			}
			return;

		case MAP_KONAMI5:
			if ((address < 0x5000) || (address > 0xb000) || ((address & 0x1fff) != 0x1000)) break;
			Page = (address - 0x5000) >> 13;

			if (Page == 2) SCCReg[PSlot] = (data == 0x3f) ? 1 : 0;

			data &= ROMMask[PSlot];
			if (data != ROMMapper[PSlot][Page])
			{
				RAM[Page + 2] = MemMap[PSlot][Page + 2] = ROMData[PSlot] + (data << 13);
				ROMMapper[PSlot][Page] = data;
			}
			return;

		case MAP_KONAMI4:
			if ((address < 0x6000) || (address > 0xa000) || (address & 0x1fff)) break;
			Page = (address - 0x4000) >> 13;

			data &= ROMMask[PSlot];
			if (data != ROMMapper[PSlot][Page])
			{
				RAM[Page + 2] = MemMap[PSlot][Page + 2] = ROMData[PSlot] + (data << 13);
				ROMMapper[PSlot][Page] = data;
			}
			return;

		case MAP_ASCII8:
			if ((address >= 0x6000) && (address < 0x8000))
			{
				UINT8 *pgPtr;

				Page = (address & 0x1800) >> 11;

				if (data & (ROMMask[PSlot] + 1)) {
					data = 0xff;
					pgPtr = SRAMData[PSlot];
				}
				else
				{
					data &= ROMMask[PSlot];
					pgPtr = ROMData[PSlot] + (data << 13);
				}
				
				if (data != ROMMapper[PSlot][Page])
				{
					MemMap[PSlot][Page + 2] = pgPtr;
					ROMMapper[PSlot][Page] = data;

					if (PSL[(Page >> 1) + 1] == PSlot)
						RAM[Page + 2] = pgPtr;
				}
				return;
			}
			if ((address >= 0x8000) && (address < 0xc000) && (ROMMapper[PSlot][((address >> 13) & 1) + 2] == 0xff))
			{
				RAM[address >> 13][address & 0x1fff] = data;
				return;
			}
			break;

		case MAP_ASCII16:
			if ((address >= 0x6000) && (address < 0x8000) && ((data <= ROMMask[PSlot] + 1) || !(address & 0x0fff)))
			{
				UINT8 *pgPtr;
				Page = (address & 0x1000) >> 11;

				if (data & (ROMMask[PSlot] + 1))
				{
					data = 0xff;
					pgPtr = SRAMData[PSlot];
				}
				else
				{

					data = (data << 1) & ROMMask[PSlot];
					pgPtr = ROMData[PSlot] + (data << 13);
				}

				if (data != ROMMapper[PSlot][Page])
				{
					MemMap[PSlot][Page + 2] = pgPtr;
					MemMap[PSlot][Page + 3] = pgPtr + 0x2000;
					ROMMapper[PSlot][Page] = data;
					ROMMapper[PSlot][Page + 1] = data + 1;

					if (PSL[(Page >> 1) + 1] == PSlot)
					{
						RAM[Page + 2] = pgPtr;
						RAM[Page + 3] = pgPtr + 0x2000;
					}
				}
				return;
			}

			if ((address >= 0x8000) && (address < 0xc000) && (ROMMapper[PSlot][2] == 0xff))
			{
				UINT8 *pgPtr = RAM[address >> 13];
				address &= 0x07ff;
				pgPtr[address + 0x0800] = pgPtr[address + 0x1000] = pgPtr[address + 0x1800] =
					pgPtr[address + 0x2000] = pgPtr[address + 0x2800] = pgPtr[address + 0x3000] =
					pgPtr[address + 0x3800] = pgPtr[address] = data;
				return;
			}
			break;
	}

	//bprintf(0, _T("Unhandled mapper write. 0x%04X: %02X, slot %d\n"), address, data, PSlot);
}

static INT32 Mapper_read(UINT16 address, UINT8 *data)
{
  UINT8 Page = address >> 14;
  UINT8 PSlot = PSL[Page];

  if (PSlot >= MAXSLOTS) return 0;

  if (!ROMData[PSlot] || !ROMMask[PSlot]) return 0;

  switch (ROMType[PSlot])
  {
	  case MAP_CROSSBL:
		  {
			  UINT8 *bank_base = crossblaim_bank_base[address >> 14];

			  if (bank_base != NULL)	{
				  *data = bank_base[address & 0x3fff];
				  return 1;
			  }
		  }
	  case MAP_DOOLY:
		  {
			  if ((address > 0x3fff) && (address < 0xc000)) {
				  UINT8 rb = ROMData[PSlot][address - 0x4000];

				  if (dooly_prot == 0x04) {
					  rb = BITSWAP08(rb, 7, 6, 5, 4, 3, 1, 0, 2);
				  }

				  *data = rb;
				  return 1;
			  }
		  }
	  case MAP_RTYPE:
		  {
			  if (address > 0x3fff && address < 0xc000)
			  {
				  *data = rtype_bank_base[address >> 15][address & 0x3fff];
				  return 1;
			  }
		  }
  }
  return 0;
}

static void SetSlot(UINT8 nSlot)
{
	UINT8 I, J;

	if (PSLReg != nSlot) {
		PSLReg = nSlot;
		for (J = 0; J < 4; J++)	{
			I = J << 1;
			PSL[J] = nSlot & 3;
			RAM[I] = MemMap[PSL[J]][I];
			RAM[I + 1] = MemMap[PSL[J]][I + 1];
			WriteMode[J] = (PSL[J] == RAMSLOT) && (MemMap[RAMSLOT][I] != EmptyRAM);
			nSlot >>= 2;
		}
	}
}

static void PageMap(INT32 CartSlot, const char *cMap)
{
	for (INT32 i = 0; i < 8; i++) {
		switch (cMap[i << 1]) {
			case 'n': // no change
				break;
			case 'e': // empty page
				{
					MemMap[CartSlot][i] = EmptyRAM;
				}
				break;
		    default: // map page num.
				{
					MemMap[CartSlot][i] = ROMData[CartSlot] + ((cMap[i << 1] - '0') * 0x2000);
					//bprintf(0, _T("pg %X @ %X\n"), i, ((cMap[i << 1] - '0') * 0x2000));
				}
		}
	}
}

static void MapMegaROM(UINT8 nSlot, UINT8 nPg0, UINT8 nPg1, UINT8 nPg2, UINT8 nPg3)
{
  if (nSlot >= MAXSLOTS) return;

  nPg0 &= ROMMask[nSlot];
  nPg1 &= ROMMask[nSlot];
  nPg2 &= ROMMask[nSlot];
  nPg3 &= ROMMask[nSlot];

  MemMap[nSlot][2] = ROMData[nSlot] + nPg0 * 0x2000;
  MemMap[nSlot][3] = ROMData[nSlot] + nPg1 * 0x2000;
  MemMap[nSlot][4] = ROMData[nSlot] + nPg2 * 0x2000;
  MemMap[nSlot][5] = ROMData[nSlot] + nPg3 * 0x2000;

  ROMMapper[nSlot][0] = nPg0;
  ROMMapper[nSlot][1] = nPg1;
  ROMMapper[nSlot][2] = nPg2;
  ROMMapper[nSlot][3] = nPg3;
}

static INT32 GuessROM(UINT8 *buf, INT32 Size)
{
	INT32 i, j;
	INT32 ROMCount[MAXMAPPERS];

	for (i = 0; i < MAXMAPPERS; i++)
		ROMCount[i] = 1;

	ROMCount[MAP_KONGEN8] += 1;
	ROMCount[MAP_ASCII16] -= 1;

	for (i = 0; i < Size-2; i++) {
		switch (buf[i] + (buf[i + 1] << 8) + (buf[i + 2] << 16))
		{
			case 0x500032:
			case 0x900032:
			case 0xB00032:
				{
					ROMCount[MAP_KONAMI5]++;
					break;
				}
			case 0x400032:
			case 0x800032:
			case 0xA00032:
				{
					ROMCount[MAP_KONAMI4]++;
					break;
				}
			case 0x680032:
			case 0x780032:
				{
					ROMCount[MAP_ASCII8]++;
					break;
				}
			case 0x600032:
				{
					ROMCount[MAP_KONAMI4]++;
					ROMCount[MAP_ASCII8]++;
					ROMCount[MAP_ASCII16]++;
					break;
				}
			case 0x700032:
				{
					ROMCount[MAP_KONAMI5]++;
					ROMCount[MAP_ASCII8]++;
					ROMCount[MAP_ASCII16]++;
					break;
				}
			case 0x77FF32:
				{
					ROMCount[MAP_ASCII16]++;
					break;
				}
		}
	}

	for (i = 0, j = 0; j < MAXMAPPERS; j++)
		if (ROMCount[j] > ROMCount[i]) i = j;

	return i;
}

static INT32 IsBasicROM(UINT8 *rom)
{
	return (rom[2] == 0 && rom[3] == 0 && rom[8] && rom[9]);
}

static UINT16 GetRomStart(UINT8* romData, INT32 size)
{
    INT32 pages[3] = { 0, 0, 0 };

    for (INT32 startPage = 0; startPage < 2; startPage++) {
        UINT8* romPtr = romData + 0x4000 * startPage;

        if (size < 0x4000 * startPage + 0x10) {
            continue;
        }
	    if (romPtr[0] == 'A' && romPtr[1] == 'B') {
		    for (INT32 i = 0; i < 4; i++) {
                UINT16 address = romPtr[2 * i + 2] + 256 * (UINT16)romPtr[2 * i + 3];

			    if (address > 0) {
				    UINT16 page = address / 0x4000 - startPage;

                    if (page < 3) {
                        pages[page]++;
				    }
			    }
		    }
        }
	}

    if (pages[1] && (pages[1] >= pages[0]) && (pages[1] >= pages[2])) {
		return 0x4000;
	}

	if (pages[0] && pages[0] >= pages[2]) {
		return 0x0000;
	}

	if (pages[2]) {
		return 0x8000;
	}

	return 0x0000;
}

static INT32 InsertCart(UINT8 *cartbuf, INT32 cartsize, INT32 nSlot)
{
	INT32 Len, Pages, Flat64, BasicROM;
	UINT8 ca, cb;

	if (nSlot >= MAXSLOTS) return 0;

	Len = cartsize >> 13; // Len, in 8k pages

	for (Pages = 1; Pages < Len; Pages <<= 1); // Calculate nearest power of 2 of len

	ROMData[nSlot] = cartbuf;
	SRAMData[nSlot] = game_sram;

	Flat64 = 0;
	BasicROM = 0;

	ca = cartbuf[0];
	cb = cartbuf[1];

	if ((ca == 'A') || (cb == 'B')) {
		BasicROM = IsBasicROM(cartbuf);
	} else {
		ca = cartbuf[0 + 0x4000];
		cb = cartbuf[1 + 0x4000];
		Flat64 = (ca == 'A') && (cb == 'B');
	}

	if ((Len >= 2) && ((ca != 'A') || (cb != 'B'))) { // check last page
		ca = cartbuf[0 + 0x2000 * (Len - 2)];
		cb = cartbuf[1 + 0x2000 * (Len - 2)];
	}

	if ((ca != 'A') || (cb != 'B')) {
//		bprintf(0, _T("MSX Cartridge signature not found!\n"));
		return 0;
	}

	if (Len < Pages) { // rom isn't a valid page-length, so mirror
		memcpy(ROMData[nSlot] + Len * 0x2000,
			   ROMData[nSlot] + (Len - Pages / 2) * 0x2000,
			   (Pages - Len) * 0x2000);
	}

//	bprintf(0, _T("Cartridge %c: %dk "), 'A' + nSlot - CARTSLOTA, Len * 8);


	ROMMask[nSlot]= !Flat64 && (Len > 4) ? (Pages - 1) : 0x00;

//	bprintf(0, _T("%S\n"), (BasicROM) ? "Basic ROM Detected." : "");
/*
	// Override mapper from hardware code
	switch (BurnDrvGetHardwareCode() & 0xff) {
		case HARDWARE_MSX_MAPPER_BASIC:
			BasicROM = 1;
			break;
		case HARDWARE_MSX_MAPPER_ASCII8:
			ROMType[nSlot] = MAP_ASCII8;
			break;
		case HARDWARE_MSX_MAPPER_ASCII16:
			ROMType[nSlot] = MAP_ASCII16;
			break;
		case HARDWARE_MSX_MAPPER_KONAMI:
			ROMType[nSlot] = MAP_KONAMI4;
			break;
		case HARDWARE_MSX_MAPPER_KONAMI_SCC:
			ROMType[nSlot] = MAP_KONAMI5;
			break;
		case HARDWARE_MSX_MAPPER_DOOLY:
			ROMType[nSlot] = MAP_DOOLY;
			ROMMask[nSlot]=3;
			break;
		case HARDWARE_MSX_MAPPER_CROSS_BLAIM:
			ROMType[nSlot] = MAP_CROSSBL;
			crossblaim_selected_bank = 1;
			crossblaim_do_bank(ROMData[nSlot]);
			break;
		case HARDWARE_MSX_MAPPER_RTYPE:
			ROMType[nSlot] = MAP_RTYPE;
			rtype_selected_bank = 15;
			rtype_do_bank(ROMData[nSlot]);
			break;
	default:
		if (ROMMask[nSlot] + 1 > 4) {
			ROMType[nSlot] = GuessROM(ROMData[nSlot], 0x2000 * (ROMMask[nSlot] + 1));
			bprintf(0, _T("Mapper heusitics detected: %S..\n"), ROMNames[ROMType[nSlot]]);
		}
	}
*/

	if (ROMType[nSlot] != MAP_DOOLY) { // set-up non-megarom mirroring & mapping
		switch (Len)
		{
			case 1: // 8k rom-mirroring
				if (BasicROM)
				{ // BasicROM only on page 2
					PageMap(nSlot, "e:e:e:e:0:0:e:e");
				} else
				{ // normal 8k ROM
					PageMap(nSlot, "0:0:0:0:0:0:0:0");
				}
				break;

			case 2: // 16k rom-mirroring
				if (BasicROM)
				{ // BasicROM only on page 2
					PageMap(nSlot, "e:e:e:e:0:1:e:e");
				} else
				{ // normal 16k ROM
					PageMap(nSlot, "0:1:0:1:0:1:0:1");
				}
				break;

			case 3:
			case 4: // 24k & 32k rom-mirroring
				if (GetRomStart(cartbuf, cartsize) != 0x4000) {
					PageMap(nSlot, "0:1:0:1:2:3:2:3"); // normal
				} else {
					PageMap(nSlot, "2:3:0:1:2:3:0:1"); // swapped
//					bprintf(0, _T("Swapped mirroring.\n"));
				}
				break;

		default:
			if (Flat64)
			{ // Flat-64k ROM
				PageMap(nSlot, "0:1:2:3:4:5:6:7");
			}
			break;
		}
//		if (Flat64 || Len < 5)
//			bprintf(0, _T("starting address 0x%04X.\n"),
//					MemMap[nSlot][2][2] + 256 * MemMap[nSlot][2][3]);
	}

	// map gen/16k megaROM pages 0:1:last-1:last
	if ((ROMType[nSlot] == MAP_KONGEN16) && (ROMMask[nSlot] + 1 > 4))
		MapMegaROM(nSlot, 0, 1, ROMMask[nSlot] - 1, ROMMask[nSlot]);

	return 1;
}


static void __fastcall msx_write_port(UINT16 port, UINT8 data)
{
	port &= 0xff;
	switch (port)
	{
		case 0x98:
			TMS9928AWriteVRAM(data);
		return;

		case 0x99:
			TMS9928AWriteRegs(data);
		return;

		case 0xa0:
			AY8910Write(0, 0, data);
		break;

		case 0xa1:
			AY8910Write(0, 1, data);
		break;

		case 0xa8:
		case 0xa9:
		case 0xaa:
		case 0xab:
			ppi8255_w(0, port & 3, data);
			return;

		case 0xd8: // Kanji high bit address selector
			Kana = (Kana & 0x1f800) | (data & 0x3f) << 5;
			KanaByte = 0;
			return;

		case 0xd9: // Kanji low bit address selector
			Kana = (Kana & 0x007e0) | (data & 0x3f) << 11;
			KanaByte = 0;
			return;

#if 0
		// disable the ram-mapper for now (only really used in msx2)
		// causes some issues with games that erraneously write to 0xfc (stardust and utopia demo?)

		case 0xfc: // map ram-page 0x0000, 0x4000, 0x8000, 0xc000
		case 0xfd:
		case 0xfe:
		case 0xff: //bprintf(0, _T("Port %X Data %X.\n"), port, data);
			INT32 PSlot = port - 0xfc;
			data &= RAMMask;
			if (RAMMapper[PSlot] != data) {
//				bprintf(0, _T("Mapped RAM chunk %d @ 0x%X\n"), data, PSlot * 0x4000);
				INT32 Page = PSlot << 1;
				RAMMapper[PSlot] = data;
				MemMap[RAMSLOT][Page] = RAMData + (data << 14);
				MemMap[RAMSLOT][Page + 1] = MemMap[RAMSLOT][Page] + 0x2000;

				if ((PSL[PSlot] == RAMSLOT))	{
					WriteMode[PSlot] = 1;
					RAM[Page] = MemMap[RAMSLOT][Page];
					RAM[Page + 1] = MemMap[RAMSLOT][Page + 1];
				}
			}
			return;
#endif
	}

	//bprintf(0, _T("port[%X] data[%X],"), port, data);
}

static UINT8 __fastcall msx_read_port(UINT16 port)
{
	port &= 0xff;

	switch (port)
	{
		case 0x98:
			return TMS9928AReadVRAM();

		case 0x99:
			return TMS9928AReadRegs() | ((VBlankKludge) ? 0x80 : 0x00);

		case 0xa2:
			return AY8910Read(0);

		case 0xa8:
		case 0xa9:
		case 0xaa:
		case 0xab:
			return ppi8255_r(0, port & 3);

		case 0xd9: {
			UINT8 Kan = (use_kanji) ? kanji_rom[Kana + KanaByte] : 0xff;
			KanaByte = (KanaByte + 1) & 0x1f;
			return Kan;
		}

		case 0xfc: // map ram-page 0x0000, 0x4000, 0x8000, 0xc000
		case 0xfd:
		case 0xfe:
		case 0xff:
			return RAMMapper[port - 0xfc] | ~RAMMask;
	}

	//bprintf(0, _T("port[%X],"), port);

	return 0xff;
}

static UINT8 msx_ppi8255_portB_read()
{
	return keyRowGet(ppiC_row);
}

static void msx_ppi8255_portA_write(UINT8 data)
{
	SetSlot(data);
}

static void msx_ppi8255_portC_write(UINT8 data)
{
	ppiC_row = data & 0x0f;
	if (DrvDips[0] & 0x02)
		DACWrite(0, (data & 0x80) ? 0x80 : 0x00); // Key-Clicker / 1-bit DAC
}

static UINT8 ay8910portAread(UINT32 offset)
{
	if (SwapJoyports) {
		return (Joyselect) ? DrvInputs[0] : DrvInputs[1];
	} else {
		return (Joyselect) ? DrvInputs[1] : DrvInputs[0];
	}
}

static void ay8910portAwrite(UINT32 offset, UINT32 data)
{
	//bprintf(0, _T("8910 portAwrite %X:%X\n"), offset, data);
}

static void ay8910portBwrite(UINT32 offset, UINT32 data)
{
	//bprintf(0, _T("B %X:%X\n"), offset, data);
	Joyselect = (data & 0x40) ? 1 : 0;
}

static void vdp_interrupt(INT32 state)
{
	CZetSetIRQLine(0, state ? CZET_IRQSTATUS_ACK : CZET_IRQSTATUS_NONE);
}

static INT32 DrvDoReset()
{
	memset (AllRam, 0, RamEnd - AllRam);

	memset(&keyRows, 0, sizeof(keyRows));
	ppiC_row = 0;

	Kana = 0;
	KanaByte = 0;

	msxinit(CurRomSizeA);

	ppi8255_init(1); // there is no reset, so use this.

	CZetOpen(0);
	CZetReset();
	TMS9928AReset();
	CZetClose();

	AY8910Reset(0);
#ifdef K051649
	K051649Reset();
#endif
	DACReset();

	return 0;
}

static INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;

	maincpu		    = Next; Next += 0x020000;
	game		    = (UINT8 *)0x00200000; //MAX_MSX_CARTSIZE;
#ifdef CASSETTE
	game2		    = Next; Next += MAX_MSX_CARTSIZE;
#endif
#ifdef KANJI
	kanji_rom       = Next; Next += 0x040000;
#endif
	game_sram       = Next; Next += 0x004000;

	AllRam			= Next;

	main_mem		= Next; Next += 0x020000;
	EmptyRAM        = Next; Next += 0x010000;

	RamEnd			= Next;

	pAY8910Buffer[0]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[1]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);
	pAY8910Buffer[2]	= (INT16*)Next; Next += nBurnSoundLen * sizeof(INT16);

	MemEnd			= Next;

	return 0;
}

static void __fastcall msx_write(UINT16 address, UINT8 data)
{
	if (WriteMode[address >> 14]) {
		RAM[address >> 13][address & 0x1fff] = data;
		return;
	}

	if ((address > 0x3fff) && (address < 0xc000))
		Mapper_write(address, data);

}

static UINT8 __fastcall msx_read(UINT16 address)
{
	UINT8 d = 0;

	if (Mapper_read(address, &d)) {
		return d;
	}

	return (RAM[address >> 13][address & 0x1fff]);
}

static INT32 DrvSyncDAC()
{
	return (INT32)(float)(nBurnSoundLen * (CZetTotalCycles() / (3579545.000 / ((Hertz60) ? 60.0 : 50.0))));
}

static INT32 DrvInit()
{
	DrvInitSaturn();
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"initsat complete     ",26,200);
	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"bef malloc      ",26,200);
	if ((AllMem = (UINT8 *)BurnMalloc(nLen)) == NULL) return 1;
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"aft malloc      ",26,200);
	memset(AllMem, 0, nLen);
	MemIndex();

	{
		struct BurnRomInfo ri;

//		bprintf(0, _T("MSXINIT...\n"));
		Hertz60 = (DrvDips[0] & 0x10) ? 1 : 0;
		BiosmodeJapan = (DrvDips[0] & 0x01) ? 1 : 0;
		SwapJoyports = (DrvDips[0] & 0x20) ? 1 : 0;

//		bprintf(0, _T("%Shz mode.\n"), (Hertz60) ? "60" : "50");
//		bprintf(0, _T("BIOS mode: %S\n"), (BiosmodeJapan) ? "Japanese" : "Normal");
//		bprintf(0, _T("%S"), (SwapJoyports) ? "Joystick Ports: Swapped.\n" : "");
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"jp        ",26,200);

//		if (BurnLoadRom(maincpu, 0x80 + BiosmodeJapan, 1)) return 1; // BIOS
		if (BurnLoadRom(maincpu, 1 + BiosmodeJapan, 1)) return 1; // BIOS
#ifdef KANJI
//		use_kanji = (BurnLoadRom(kanji_rom, 0x82, 1) == 0);
		use_kanji = (BurnLoadRom(kanji_rom, 3, 1) == 0);
#endif
//		if (use_kanji)
//			bprintf(0, _T("Kanji ROM loaded.\n"));

		BurnDrvGetRomInfo(&ri, 0);

		if (ri.nLen > MAX_MSX_CARTSIZE) {
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"bad rom size        ",26,200);
//			bprintf(0, _T("Bad MSX1 ROMSize! exiting.. (> %dk) \n"), MAX_MSX_CARTSIZE / 1024);
			return 1;
		}
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"memset         ",26,200);

		memset(game, 0xff, MAX_MSX_CARTSIZE);
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"game load         ",26,200);

		if (BurnLoadRom(game + 0x00000, 0, 1)) return 1;

		CurRomSizeA = ri.nLen;

		BurnDrvGetRomInfo(&ri, 1);

		if (ri.nLen > 0 && ri.nLen < MAX_MSX_CARTSIZE) {
			memset(game2, 0xff, MAX_MSX_CARTSIZE);

			if (BurnLoadRom(game2 + 0x00000, 1, 1)) return 1;

			CurRomSizeB = ri.nLen;
//			bprintf(0, _T("Loaded secondary tape/rom, size: %d.\n"), CurRomSizeB);
		}

		// msxinit(ri.nLen); (in DrvDoReset()! -dink)
	}
//	BurnSetRefreshRate((Hertz60) ? 60.0 : 50.0);

	CZetInit(0);
	CZetOpen(0);

	CZetSetOutHandler(msx_write_port);
	CZetSetInHandler(msx_read_port);
	CZetSetWriteHandler(msx_write);
	CZetSetReadHandler(msx_read);
	CZetClose();

	AY8910Init(0, 3579545/2, nBurnSoundRate, ay8910portAread, NULL, ay8910portAwrite, ay8910portBwrite);
//	AY8910SetAllRoutes(0, 0.15, BURN_SND_ROUTE_BOTH);

#ifdef K051649
	K051649Init(3579545/2);
#endif
//	K051649SetRoute(0.20, BURN_SND_ROUTE_BOTH);

	DACInit(0, 0, 1, DrvSyncDAC);
//	DACSetRoute(0, 0.30, BURN_SND_ROUTE_BOTH);

	TMS9928AInit(TMS99x8A, 0x4000, 0, 0, vdp_interrupt);

	ppi8255_init(1);
	PPI0PortReadB	= msx_ppi8255_portB_read;
	PPI0PortWriteA	= msx_ppi8255_portA_write;
	PPI0PortWriteC	= msx_ppi8255_portC_write;
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"init complete     ",26,200);
	DrvDoReset();

	return 0;
}

static INT32 DrvExit()
{
	TMS9928AExit();
	CZetExit();

	AY8910Exit(0);
#ifdef K051649
	K051649Exit();
#endif
	DACExit();

	ppi8255_exit();

	BurnFree (AllMem);
	AllMem = NULL;

	msx_basicmode = 0;
	BiosmodeJapan = 0;
	CASMode = 0;
	VBlankKludge = 0;
	SwapRamslot = 0;
	SwapButton2 = 0;
	SwapSlash = 0;
	MapCursorToJoy1 = 0;

#ifdef BUILD_WIN32
	cBurnerKeyCallback = NULL;
	nReplayExternalDataCount = 0;
	ReplayExternalData = NULL;
#endif
	return 0;
}

static INT32 DrvFrame()
{
	static UINT8 lastnmi = 0;

	if (DrvReset) {
		DrvDoReset();
	}

	{ // Compile Inputs
		memset (DrvInputs, 0xff, 2);
		for (INT32 i = 0; i < 8; i++) {
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
		}

		if (SwapButton2)
		{ // Kludge for Xenon and Astro Marine Corps where button #2 is the 'm' key.
			static INT32 lastM = 0;
			if (DrvJoy1[5]) {
				keyInput('m', DrvJoy1[5]);
			} else {
				if (lastM) { // only turn 'm' off once after Button2 is unpressed.
					keyInput('m', DrvJoy1[5]);
				}
			}
			lastM = DrvJoy1[5];
		}

		SwapJoyports = (DrvDips[0] & 0x20) ? 1 : 0;
		MapCursorToJoy1 = (DrvDips[0] & 0x80) ? 1 : 0;

		// Keyboard fun!
		keyInput(0xf1, DrvJoy4[3]); // f1 - f6
		keyInput(0xf2, DrvJoy4[4]);
		keyInput(0xf3, DrvJoy4[5]);
		keyInput(0xf4, DrvJoy4[6]);
		keyInput(0xf5, DrvJoy4[7]);
		keyInput(0xf6, DrvJoy4[8]);

		if (MapCursorToJoy1)
		{ // Mapped to Joy #1
			keyInput(0xf8, DrvJoy1[0]);  // Key UP
			keyInput(0xf9, DrvJoy1[1]); // Key DOWN
			keyInput(0xfa, DrvJoy1[2]); // Key LEFT
			keyInput(0xfb, DrvJoy1[3]); // Key RIGHT
			keyInput(' ', DrvJoy1[4]);
		} else
		{ // Normal Cursor-key function
			keyInput(0xf8, DrvJoy4[9]);  // Key UP
			keyInput(0xf9, DrvJoy4[10]); // Key DOWN
			keyInput(0xfa, DrvJoy4[11]); // Key LEFT
			keyInput(0xfb, DrvJoy4[12]); // Key RIGHT
		}
	}
#ifdef CASSETTE
	{   // detect tape side changes
		CASSide = (DrvDips[0] & 0x40) ? 1 : 0;
		if (CASSideLast != CASSide) {
//			bprintf(0, _T("Tape change: Side %c\n"), (CASSide) ? 'B' : 'A');
			CASSideChange();
		}

		// cassette auto-load keyboard stuffing
		if (CASMode && CASFrameCounter > 250 && CASFrameCounter & 2)
			CASAutoLoadTick();
		CASFrameCounter++;
	}
#endif

	INT32 nInterleave = 256;
	INT32 nCyclesTotal[1] = { 3579545 / ((Hertz60) ? 60 : 50) };
	INT32 nCyclesDone[1] = { 0 };
	INT32 nSoundBufferPos = 0;

	CZetNewFrame();
	CZetOpen(0);

	if (DrvNMI && !lastnmi) {
		CZetNmi();
		lastnmi = DrvNMI;
	} else lastnmi = DrvNMI;

	for (INT32 i = 0; i < nInterleave; i++)
	{
		nCyclesDone[0] += CZetRun(nCyclesTotal[0] / nInterleave);

//		TMS9928AScanline(i);

		// Render Sound Segment
		volatile signed short *	pBurnSoundOut = (signed short *)0x25a20000;
//		if (pBurnSoundOut) {
			INT32 nSegmentLength = nBurnSoundLen / nInterleave;
			INT16* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
//			AY8910Render(&pAY8910Buffer[0], pSoundBuf, nSegmentLength, 0);
// vbt à remettre
#ifdef K051649
			K051649Update(pSoundBuf, nSegmentLength);
#endif
			nSoundBufferPos += nSegmentLength;
//		}
	}

	CZetClose();

	TMS9928AInterrupt();
	TMS9928ADraw();

	// Make sure the buffer is entirely filled.
	volatile signed short *	pBurnSoundOut = (signed short *)0x25a20000;
//	if (pBurnSoundOut) {
		INT32 nSegmentLength = nBurnSoundLen - nSoundBufferPos;
		INT16* pSoundBuf = pBurnSoundOut + (nSoundBufferPos << 1);
		if (nSegmentLength) {
//			AY8910Render(&pAY8910Buffer[0], pSoundBuf, nSegmentLength, 0);
// bt à remettre
#ifdef K051649
			K051649Update(pSoundBuf, nSegmentLength);
#endif
		}
		DACUpdate(pBurnSoundOut, nBurnSoundLen);
//	}

//	if (pBurnDraw) {
		TMS9928ADraw();
//	}

	return 0;
}
/*
static INT32 DrvScan(INT32 nAction, INT32 *pnMin)
{
	struct BurnArea ba;

	if (pnMin) {
		*pnMin = 0x029708;
	}

	if (nAction & ACB_VOLATILE) {
		memset(&ba, 0, sizeof(ba));
		ba.Data	  = AllRam;
		ba.nLen	  = RamEnd - AllRam;
		ba.szName = "All Ram";
		BurnAcb(&ba);

		ZetScan(nAction);
		AY8910Scan(nAction, pnMin);
		K051649Scan(nAction, pnMin);
		DACScan(nAction,pnMin);
		TMS9928AScan(nAction, pnMin);
		ppi8255_scan();

		SCAN_VAR(RAMMapper);
		SCAN_VAR(ROMMapper);
		SCAN_VAR(WriteMode);
		SCAN_VAR(PSL);
		SCAN_VAR(PSLReg);
		SCAN_VAR(SCCReg);

		SCAN_VAR(dooly_prot);
		SCAN_VAR(crossblaim_selected_bank);
		SCAN_VAR(rtype_selected_bank);

		SCAN_VAR(CASPos);
		SCAN_VAR(CASAutoLoadPos);
		SCAN_VAR(CASAutoLoadTicker);
		SCAN_VAR(CASFrameCounter);
	}

	if (nAction & ACB_WRITE) {
		if (RAMMask) { // re-map ram
			for (INT32 i = 0; i < 4; i++) {
				RAMMapper[i] &= RAMMask;
				MemMap[RAMSLOT][i * 2] = RAMData + RAMMapper[i] * 0x4000;
				MemMap[RAMSLOT][i * 2 + 1] = MemMap[RAMSLOT][i * 2] + 0x2000;
			}
		}

		for (INT32 i = 0; i < MAXSLOTS; i++)
			if (ROMData[i] && ROMMask[i]) {
				MapMegaROM(i, ROMMapper[i][0], ROMMapper[i][1], ROMMapper[i][2], ROMMapper[i][3]);
				crossblaim_do_bank(ROMData[i]);
				rtype_do_bank(ROMData[i]);
			}

		for (INT32 i = 0; i < 4; i++) {
			RAM[2 * i] = MemMap[PSL[i]][2 * i];
			RAM[2 * i + 1] = MemMap[PSL[i]][2 * i + 1];
		}
	}

	return 0;
}

INT32 MSXGetZipName(char** pszName, UINT32 i)
{
	static char szFilename[MAX_PATH];
	char* pszGameName = NULL;

	if (pszName == NULL) {
		return 1;
	}

	if (i == 0) {
		pszGameName = BurnDrvGetTextA(DRV_NAME);
	} else {
		if (i == 1 && BurnDrvGetTextA(DRV_BOARDROM)) {
			pszGameName = BurnDrvGetTextA(DRV_BOARDROM);
		} else {
			pszGameName = BurnDrvGetTextA(DRV_PARENT);
		}
	}

	if (pszGameName == NULL) {
		*pszName = NULL;
		return 1;
	}
   // remove msx_
	for (UINT32 j = 0; j < strlen(pszGameName); j++) {
		szFilename[j] = pszGameName[j + 4];
	}

	*pszName = szFilename;

	return 0;
}
*/
/*
// MSX1 BIOS
static struct BurnRomInfo msx_msxRomDesc[] = {
    { "msx.rom",     0x8000, 0xa317e6b4, BRF_BIOS }, // 0x80 - standard bios
    { "msxj.rom",    0x8000, 0x071135e0, BRF_BIOS | BRF_OPT }, // 0x81 - japanese bios
    { "kanji.rom",   0x40000, 0x1f6406fb, BRF_BIOS | BRF_OPT }, // 0x82 - kanji support
};

STD_ROM_PICK(msx_msx)
STD_ROM_FN(msx_msx)

struct BurnDriver BurnDrvmsx_msx = {
	"msx_msx", NULL, NULL, NULL, "1982",
	"MSX1 System BIOS\0", "BIOS only", "MSX", "MSX",
	NULL, NULL, NULL, NULL,
	BDF_BOARDROM, 0, HARDWARE_MSX, GBF_BIOS, 0,
	MSXGetZipName, msx_msxRomInfo, msx_msxRomName, NULL, NULL, MSXInputInfo, MSXDIPInfo,
	DrvInit, DrvExit, DrvFrame, TMS9928ADraw, DrvScan, NULL, TMS9928A_PALETTE_SIZE,
	272, 228, 4, 3
};
*/
static INT32 SwapSlashDrvInit()
{
	SwapSlash = 1;
	return DrvInit();
}

static INT32 SwapButton2DrvInit()
{
	SwapButton2 = 1;
	return DrvInit();
}

static INT32 SwapRamDrvInit()
{
	SwapRamslot = 1;
	return DrvInit();
}

static INT32 BasicDrvInit()
{
	msx_basicmode = 1;
	return DrvInit();
}
#ifdef CASSETTE
static INT32 CasBloadDrvInit()
{
	msx_basicmode = 1;
	CASMode = CAS_BLOAD;
	return DrvInit();
}

static INT32 CasRunSwapButton2DrvInit()
{
	msx_basicmode = 1;
	SwapButton2 = 1;
	CASMode = CAS_RUN;
	return DrvInit();
}

static INT32 CasRunDrvInit()
{
	msx_basicmode = 1;
	CASMode = CAS_RUN;
	return DrvInit();
}

static INT32 CasCloadDrvInit()
{
	msx_basicmode = 1;
	CASMode = CAS_CLOAD;
	return DrvInit();
}

static INT32 CasCloadRRDrvInit()
{ // special for perez the mouse :)
	msx_basicmode = 1;
	CASMode = CAS_CLOADRR;
	return DrvInit();
}

static INT32 VoidrunnerDrvInit()
{
	msx_basicmode = 1;
	VBlankKludge = 1;
	CASMode = CAS_RUN;
	return DrvInit();
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
//	colBgAddr		= (Uint16*)SCL_AllocColRam(SCL_NBG0,OFF);
	colAddr			= (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	(Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
//	SCL_SetColRam(SCL_NBG1,0,8,palette);
	SCL_SetColRam(SCL_NBG1,8,8,palette);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initLayers(void)
{
//    SclConfig	config;
// **29/01/2007 : VBT sauvegarde cycle patter qui fonctionne jusqu'à maintenant

	Uint16	CycleTb[]={
		  // VBT 04/02/2007 : cycle pattern qui fonctionne just test avec des ee
		0xff5e, 0xffff, //A1
		0xffff, 0xffff,	//A0
		0x04ee, 0xffff,   //B1
		0xffff, 0xffff  //B0
	};
 	SclConfig	scfg;

//	SCL_InitConfigTb(&scfg);
	scfg.dispenbl		= OFF;
/*	
	scfg.charsize		= SCL_CHAR_SIZE_1X1;//OK du 1*1 surtout pas toucher
	scfg.pnamesize	= SCL_PN1WORD;
	scfg.flip				= SCL_PN_10BIT;
	scfg.platesize	= SCL_PL_SIZE_1X1;
	scfg.coltype		= SCL_COL_TYPE_16;
	scfg.datatype	= SCL_CELL;
	scfg.patnamecontrl =  0x000c;// VRAM B1 ‚ÌƒIƒtƒZƒbƒg 
	scfg.plate_addr[0] = ss_map;		 */
	SCL_SetConfig(SCL_NBG0, &scfg);
	SCL_SetConfig(SCL_NBG2, &scfg);
	
/********************************************/	

//	SCL_InitConfigTb(&scfg);
	scfg.dispenbl 	 = ON;
	scfg.bmpsize 		 = SCL_BMP_SIZE_512X256;
	scfg.coltype 		 = SCL_COL_TYPE_16;//SCL_COL_TYPE_256;
	scfg.datatype 	 = SCL_BITMAP;
	scfg.mapover       = SCL_OVER_0;
	scfg.plate_addr[0] = ss_font;
	SCL_SetConfig(SCL_NBG1, &scfg);
	SCL_SetCycleTable(CycleTb);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void initPosition(void)
{
	SCL_Open();
	ss_reg->n1_move_x = 0;
	ss_reg->n1_move_y = 0;
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*void dummy()
{

} */
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn()
{
//	InitCDsms();
//	SPR_InitSlaveSH();
//	SPR_RunSlaveSH((PARA_RTN*)dummy, NULL);
	nBurnSprites  = 4+32;//131;//27;
	nBurnLinescrollSize = 0;
	nSoundBufferPos = 0;//sound position à renommer

	SS_CACHE = cache      =(Uint8  *)SCL_VDP2_VRAM_B1;
	SS_MAP     = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_FONT   = ss_font    =(Uint16 *)SCL_VDP2_VRAM_A1;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;
//	ss_scl			= (Fixed32 *)SS_SCL;
//	UINT8 *ss_vram = (UINT8 *)SS_SPRAM;
//	pTransDraw	= (ss_vram+0x1100);
	file_id			= 2; // bubble bobble
//	file_max		= getNbFiles();

//	SaturnInitMem();
	int nLen = MemEnd - (UINT8 *)0;
//	SaturnMem = (UINT8 *)malloc(nLen);
//	bp_lut		= (UINT32 *)malloc(0x10000*sizeof(UINT32));
//	SaturnInitMem();
	ss_sprite[3].ax = 0;
	ss_sprite[3].ay = 0;

	ss_sprite[3].color          = 0x0;
	ss_sprite[3].charAddr    = 0x2220;// 0x2000 => 0x80 sprites <<6
	ss_sprite[3].control       = ( JUMP_NEXT | FUNC_NORMALSP); // | DIR_LRTBREV); // | flip);
	ss_sprite[3].drawMode = ( COLOR_0 | ECD_DISABLE | COMPO_REP); //256 colors
	ss_sprite[3].charSize    = 0x20C0;  // 256x*192y
	
//	initLayers();
	
    SS_SET_N0PRIN(0);
    SS_SET_N1PRIN(6);
    SS_SET_S0PRIN(5);

	initLayers();
	initColors();
//	initPosition();
//	initSprites(256+48-1,192+16-1,256-1,192-1,48,16);

//	initSprites(256-1,192-1,0,0,0,0);

//	 initScrolling(ON,SCL_VDP2_VRAM_B0+0x4000);
//	drawWindow(32,192,192,14,52);
	nBurnFunction = update_input1;
	drawWindow(0,192,192,2,66);
	SetVblank2();
}
//-------------------------------------------------------------------------------------------------------------------------------------
