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
void dummy();
#define K051649 1
//#define CASSETTE 1
//#define KANJI 1
//#define DAC 1
#define RAZE 1

#ifdef RAZE
#include "raze\raze.h"
static void __fastcall msx_write_konami4(UINT16 address, UINT8 data);
static void __fastcall msx_write_konami4scc(UINT16 address, UINT8 data);
static void __fastcall msx_write_scc(UINT16 address, UINT8 data);
static void __fastcall msx_write_scc2(UINT16 address, UINT8 data);
static void __fastcall msx_write_ascii8(UINT16 address, UINT8 data);
#endif
static UINT8 msx_ppi8255_portB_read();
static void msx_ppi8255_portA_write(UINT8 data);
static void msx_ppi8255_portC_write(UINT8 data);
static UINT8 ay8910portAread(UINT32 offset);
static void ay8910portAwrite(UINT32 offset, UINT32 data);
static void ay8910portBwrite(UINT32 offset, UINT32 data);

static void SetSlot(UINT8 nSlot);
static void setFetch(UINT32 I, UINT8 *ram);
static void vdp_interrupt(INT32 state);

static void __fastcall msx_write(UINT16 address, UINT8 data);

static UINT8 __fastcall msx_read_port(UINT16 port);
static void __fastcall msx_write_port(UINT16 port, UINT8 data);

static void updateSlaveSound();
static void updateSlaveSoundSCC();
static void Set8PCM();

int ovlInit(char *szShortName)
{
	struct BurnDriver nBurnDrvMSX_1942 = {
		"msx", NULL,
		"MSX1 System",
		MSX_1942RomInfo, MSX_1942RomName, MSXInputInfo, MSXDIPInfo,
		DrvInit, DrvExit, DrvFrame, NULL
	};

	memcpy(shared,&nBurnDrvMSX_1942,sizeof(struct BurnDriver));

	ss_reg   = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
	file_id	 = 2;
}
//-------------------------------------------------------------------------------------------------------------------------------------
#if 1
int stop =0;
static void load_rom()
{
	stop=0;

	for(unsigned int i=0;i<8;i++)
	{
		PCM_MeStop(pcm8[i]);
	}
	memset4_fast(SOUND_BUFFER,0x00,0x4000*9);

	memset(game, 0xff, MAX_MSX_CARTSIZE);
	DrvDips[0] = 0x11;
	Hertz60 = (DrvDips[0] & 0x10) ? 1 : 0;
	BiosmodeJapan = (DrvDips[0] & 0x01) ? 1 : 0;
	SwapJoyports = (DrvDips[0] & 0x20) ? 1 : 0;

	struct BurnRomInfo ri;

	BurnDrvGetRomInfo(&ri, 0);
	ri.nLen		 = GetFileSize(file_id);
	GFS_Load(file_id, 0, game, ri.nLen);
	CurRomSizeA = ri.nLen;

	Hertz60 = (DrvDips[0] & 0x10) ? 1 : 0;
	BiosmodeJapan = (DrvDips[0] & 0x01) ? 1 : 0;
	SwapJoyports = (DrvDips[0] & 0x20) ? 1 : 0;

#ifdef RAZE
	z80_init_memmap();

 	z80_map_fetch (0x0000, 0x3fff, maincpu); 
	z80_map_read  (0x0000, 0x3fff, maincpu);
	z80_end_memmap();   

	z80_set_in((unsigned char (*)(unsigned short))&msx_read_port);
	z80_set_out((void (*)(unsigned short, unsigned char))&msx_write_port);
#else
	CZetExit2();
	memset(CZ80Context,0x00,0x1080);
	CZetInit2(1,CZ80Context);
	CZetOpen(0);
	CZetMapArea(0x0000, 0x3fff, 0, maincpu);
    CZetMapArea(0x0000, 0x3fff, 2, maincpu);

	CZetSetOutHandler(msx_write_port);
	CZetSetInHandler(msx_read_port);

	CZetSetWriteHandler(msx_write);
//	CZetClose();
#endif
	nSoundBufferPos=0;
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;
	
	for(unsigned int i=0;i<8;i++)
	{
		PCM_MeStart(pcm8[i]);
	}

	TMS9928AExit();
	TMS9928AInit(TMS99x8A, 0x4000, 0, 0, vdp_interrupt);

	ppi8255_init(1);
	PPI0PortReadB	= msx_ppi8255_portB_read;
	PPI0PortWriteA	= msx_ppi8255_portA_write;
	PPI0PortWriteC	= msx_ppi8255_portC_write;

	DrvDoReset();
}
#endif
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
			stop=1;
			SPR_InitSlaveSH();
			return;
		}

		for(i=10;i<12;i++)
		{
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
					else				file_id=file_max+1;
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
static UINT8 keyRowGet(INT32 row) { // for ppi to read
	if (row > 11) return 0xff;
	return ~keyRows[row];
}
static INT32 InsertCart(UINT8 *cartbuf, INT32 cartsize, INT32 nSlot);
static void PageMap(INT32 CartSlot, const char *cMap); //("0:0:0:0:0:0:0:0")
static void MapMegaROM(UINT8 nSlot, UINT8 nPg0, UINT8 nPg1, UINT8 nPg2, UINT8 nPg3);

#ifdef CASSETTE
static inline void intkeyOn(INT32 row, INT32 bit) {
	keyRows[row] = ((keyRows[row] & 0xff) | (1 << bit));
}

static inline void intkeyOff(INT32 row, INT32 bit) {
	keyRows[row] = ((keyRows[row] & 0xff) & ~(1 << bit));
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

void msxinit(INT32 cart_len)
{
	for(UINT32 i = 0; i < MAXSLOTS; i++) {
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

	memset4_fast(EmptyRAM, 0xff, 0x4000); // bus is pulled high for unmapped reads

	unsigned int *nSoundBuffer = (unsigned int *)0x25a28000;
	memset(nSoundBuffer[0x8000],0x00,nBurnSoundLen * sizeof(INT16) * 8 * 8);

	for(UINT32 PSlot = 0; PSlot < 4; PSlot++) // Point all pages there by default
	{
		for(UINT32 Page = 0; Page < 8; Page++)
		{
			MemMap[PSlot][Page] = EmptyRAM;
		}
	}

	DrvNMI = 0;
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

//	if (!msx_basicmode)
		InsertCart(game, cart_len, CARTSLOTA);

	PSLReg = 99;

	for (UINT32 i = 0; i < 4; i++) {
		WriteMode[i] = 0;
		PSL[i] = 0;
		MemMap[RAMSLOT][i * 2] = RAMData + (3 - i) * 0x4000;
		MemMap[RAMSLOT][i * 2 + 1] = MemMap[RAMSLOT][i * 2] + 0x2000;
		RAMMapper[i] = 3 - i;
		RAM[i * 2] = MemMap[BIOSSLOT][i * 2];
		RAM[i * 2 + 1] = MemMap[BIOSSLOT][i * 2 + 1];
	}

	for (UINT32 J = 0; J < MAXSLOTS; J++)
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
//#ifndef RAZE
//#ifndef RAZE
#ifndef RAZE
	if (!ROMData[PSlot] && (address == 0x9000))
		SCCReg[PSlot] = (data == 0x3f) ? 1 : 0;

#ifdef K051649

	if (((address & 0xdf00) == 0x9800) && SCCReg[PSlot]) { // Handle Konami-SCC (+)
		UINT16 offset = address & 0x00ff;


		if (offset < 0x80) {
			cnt++;
			K051649WaveformWrite(offset, data);
		}
		else
			if (offset < 0xa0)	{
			offset &= 0xf;

			if (offset < 0xa) {
				cnt2++;
				K051649FrequencyWrite(offset, data);
			}
			else if (offset < 0xf) {
				cnt3++;
				K051649VolumeWrite(offset - 0xa, data);
			}
			else {
				K051649KeyonoffWrite(data);
			}
		}
		return;
	}
#endif
#endif
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
				setFetch(Page + 2, RAM[Page + 2]);
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
				setFetch(Page + 2, RAM[Page + 2]);
				setFetch(Page + 3, RAM[Page + 3]);

				ROMMapper[PSlot][Page] = data;
				ROMMapper[PSlot][Page + 1] = data + 1;
			}
			return;

		case MAP_KONAMI5:
			if ((address < 0x5000) || (address > 0xb000) || ((address & 0x1fff) != 0x1000))
		{	
		break;
		}
			Page = (address - 0x5000) >> 13;

			if (Page == 2) SCCReg[PSlot] = (data == 0x3f) ? 1 : 0;

			data &= ROMMask[PSlot];
			if (data != ROMMapper[PSlot][Page])
			{
				RAM[Page + 2] = MemMap[PSlot][Page + 2] = ROMData[PSlot] + (data << 13);
				setFetch(Page + 2,RAM[Page + 2]);
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
				setFetch(Page + 2,RAM[Page + 2]);
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
					{
						RAM[Page + 2] = pgPtr;
						setFetch(Page + 2, pgPtr);
					}
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
						setFetch(Page + 2, pgPtr);
						setFetch(Page + 3, pgPtr + 0x2000);
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

//-----------------------------------------------------------------------------------------------------------------------------
static void Mapper_write_scc(UINT16 address, UINT8 data)
{
	UINT8 Page = address >> 14; // pg. num
	UINT8 PSlot = PSL[Page];

	if (PSlot >= MAXSLOTS) return;
//#ifndef RAZE
#ifdef RAZE
	if (!ROMData[PSlot] && (address == 0x9000))
		SCCReg[PSlot] = (data == 0x3f) ? 1 : 0;

#ifdef K051649

	if (((address & 0xdf00) == 0x9800) && SCCReg[PSlot]) 
	{ // Handle Konami-SCC (+)
		UINT16 offset = address & 0x00ff;

		if (offset < 0x80) {
			K051649WaveformWrite(offset, data);
		}
		else
			if (offset < 0xa0)	{
			offset &= 0xf;

			if (offset < 0xa) {
				K051649FrequencyWrite(offset, data);
			}
			else if (offset < 0xf) {
				K051649VolumeWrite(offset - 0xa, data);
			}
			else {
				K051649KeyonoffWrite(data);
			}
		}
		return;
	}
#endif
#endif
	if (!ROMData[PSlot] || !ROMMask[PSlot]) return;

	if ((address < 0x5000) || (address > 0xb000) || ((address & 0x1fff) != 0x1000))
	{	
		return;
	}
	Page = (address - 0x5000) >> 13;

	if (Page == 2) SCCReg[PSlot] = (data == 0x3f) ? 1 : 0;

	data &= ROMMask[PSlot];
	if (data != ROMMapper[PSlot][Page])
	{
		RAM[Page + 2] = MemMap[PSlot][Page + 2] = ROMData[PSlot] + (data << 13);
		setFetch(Page + 2,RAM[Page + 2]);
		ROMMapper[PSlot][Page] = data;
	}

	return;
}

//-----------------------------------------------------------------------------------------------------------------------------
setFetchKonGen8()
{
#ifndef RAZE
// bank 1 ---------------------------------------------------------------------------
		CZetMapArea(	0x4000, 0x5fff, 0, &RAM[2][0x0000] );
//		CZetMapArea(	0x4000, 0x5fff, 1, NULL); //&RAM[2][0x0000] );
		CZetMapArea(	0x4000, 0x5fff, 2, &RAM[2][0x0000] );
// bank 2 ---------------------------------------------------------------------------
		CZetMapArea(	0x6000, 0x7fff, 0, &RAM[3][0x0000] );
//		CZetMapArea(	0x6000, 0x7fff, 1, NULL); //&RAM[3][0x0000] );
		CZetMapArea(	0x6000, 0x7fff, 2, &RAM[3][0x0000] );
// bank 3 ---------------------------------------------------------------------------
		CZetMapArea(	0x8000, 0x9fff, 0, &RAM[4][0x0000] );
//		CZetMapArea(	0x8000, 0x9fff, 1, NULL); //&RAM[4][0x0000] );
		CZetMapArea(	0x8000, 0x9fff, 2, &RAM[4][0x0000] );
// bank 4 ---------------------------------------------------------------------------
		CZetMapArea(	0xa000, 0xbfff, 0, &RAM[5][0x0000] );
//		CZetMapArea(	0xa000, 0xbfff, 1, NULL); //&RAM[5][0x0000] );
		CZetMapArea(	0xa000, 0xbfff, 2, &RAM[5][0x0000] );
// end ------------------------------------------------------------------------------
//	if(WriteMode[3])
	{
//		CZetMapArea(	0xc000, 0xdfff, 1, &RAM[6][0x0000]);
//		CZetMapArea(	0xe000, 0xffff, 1, &RAM[7][0x0000]);
	}
/*	else
	{
		CZetMapArea(	0xc000, 0xdfff, 1, NULL);
		CZetMapArea(	0xe000, 0xffff, 1, NULL);
	}
*/
		CZetMapArea(	0xc000, 0xdfff, 0, &RAM[6][0x0000]);
		CZetMapArea(	0xc000, 0xdfff, 1, &RAM[6][0x0000]);
		CZetMapArea(	0xc000, 0xdfff, 2, &RAM[6][0x0000] );

		CZetMapArea(	0xe000, 0xffff, 0, &RAM[7][0x0000]);
		CZetMapArea(	0xe000, 0xffff, 1, &RAM[7][0x0000]);
		CZetMapArea(	0xe000, 0xffff, 2, &RAM[7][0x0000] );



#else
// bank 1 ---------------------------------------------------------------------------
		z80_map_read(	0x4000, 0x5fff, &RAM[2][0x0000] );
		z80_map_write(	0x4000, 0x5fff, &RAM[2][0x0000] );
		z80_map_fetch(	0x4000, 0x5fff, &RAM[2][0x0000] );
// bank 2 ---------------------------------------------------------------------------
		z80_map_read(	0x6000, 0x7fff, &RAM[3][0x0000] );
		z80_map_write(	0x6000, 0x7fff, &RAM[3][0x0000] );
		z80_map_fetch(	0x6000, 0x7fff, &RAM[3][0x0000] );
// bank 3 ---------------------------------------------------------------------------
		z80_map_read(	0x8000, 0x9fff, &RAM[4][0x0000] );
		z80_map_write(	0x8000, 0x9fff, &RAM[4][0x0000] );
		z80_map_fetch(	0x8000, 0x9fff, &RAM[4][0x0000] );
// bank 4 ---------------------------------------------------------------------------
		z80_map_read(	0xa000, 0xbfff, &RAM[5][0x0000] );
		z80_map_write(	0xa000, 0xbfff, &RAM[5][0x0000] );
		z80_map_fetch(	0xa000, 0xbfff, &RAM[5][0x0000] );
// end ------------------------------------------------------------------------------
/*
		z80_map_read(	0xc000, 0xdfff, &RAM[6][0x0000]);
		z80_map_fetch(	0xc000, 0xdfff, &RAM[6][0x0000] );

		z80_map_read(	0xe000, 0xffff, &RAM[7][0x0000]);
		z80_map_fetch(	0xe000, 0xffff, &RAM[7][0x0000] );
*/
	if(WriteMode[3])
	{
		z80_map_write(	0xc000, 0xdfff, &RAM[6][0x0000]);
		z80_map_write(	0xe000, 0xffff, &RAM[7][0x0000]);
	}
	else
	{
		z80_map_write(	0xc000, 0xdfff, NULL);
		z80_map_write(	0xe000, 0xffff, NULL);
	}
#endif
}
//-----------------------------------------------------------------------------------------------------------------------------
void setFetchRType()
{
	#ifndef RAZE
// bank 1 ---------------------------------------------------------------------------
		CZetMapArea(	0x4000, 0x5fff, 0, &RAM[2][0x0000] );
		CZetMapArea(	0x4000, 0x5fff, 1, NULL );
		CZetMapArea(	0x4000, 0x5fff, 2, &RAM[2][0x0000] );
// bank 2 ---------------------------------------------------------------------------
		CZetMapArea(	0x6000, 0x7fff, 0, &RAM[3][0x0000] );
		CZetMapArea(	0x6000, 0x7fff, 1, NULL );
		CZetMapArea(	0x6000, 0x7fff, 2, &RAM[3][0x0000] );
// bank 3 ---------------------------------------------------------------------------
		CZetMapArea(	0x8000, 0x9fff, 0, &RAM[4][0x0000] );
		CZetMapArea(	0x8000, 0x9fff, 1, NULL );
		CZetMapArea(	0x8000, 0x9fff, 2, &RAM[4][0x0000] );
// bank 4 ---------------------------------------------------------------------------
		CZetMapArea(	0xA000, 0xbfff, 0, &RAM[5][0x0000] );
		CZetMapArea(	0xA000, 0xbfff, 1, NULL );
		CZetMapArea(	0xA000, 0xbfff, 2, &RAM[5][0x0000] );
// end ------------------------------------------------------------------------------
#else
// bank 1 ---------------------------------------------------------------------------
		z80_map_read(	0x4000, 0x5fff, &RAM[2][0x0000] );
		z80_map_fetch(	0x4000, 0x5fff, &RAM[2][0x0000] );
// bank 2 ---------------------------------------------------------------------------
		z80_map_read(	0x6000, 0x7fff, &RAM[3][0x0000] );
		z80_map_fetch(	0x6000, 0x7fff, &RAM[3][0x0000] );
// bank 3 ---------------------------------------------------------------------------
		z80_map_read(	0x8000, 0x9fff, &RAM[4][0x0000] );
		z80_map_fetch(	0x8000, 0x9fff, &RAM[4][0x0000] );
// bank 4 ---------------------------------------------------------------------------
		z80_map_read(	0xa000, 0xbfff, &RAM[5][0x0000] );
		z80_map_fetch(	0xa000, 0xbfff, &RAM[5][0x0000] );
// end ------------------------------------------------------------------------------
		z80_map_read(	0xc000, 0xdfff, &RAM[6][0x0000]);
		z80_map_write(	0xc000, 0xdfff, &RAM[6][0x0000]);
		z80_map_fetch(	0xc000, 0xdfff, &RAM[6][0x0000] );

		z80_map_read(	0xe000, 0xffff, &RAM[7][0x0000]);
		z80_map_write(	0xe000, 0xffff, &RAM[7][0x0000]);
		z80_map_fetch(	0xe000, 0xffff, &RAM[7][0x0000] );
#endif
}
//-----------------------------------------------------------------------------------------------------------------------------
void setFetchAscii8()
{
/*
 Since the size of the mapper is 8Kb, the memory banks are:

	Bank 1: 4000h - 5FFFh
	Bank 2: 6000h - 7FFFh
	Bank 3: 8000h - 9FFFh
	Bank 4: A000h - BFFFh

And the address to change banks:

	Bank 1: 6000h - 67FFh (6000h used)
	Bank 2: 6800h - 6FFFh (6800h used)
	Bank 3: 7000h - 77FFh (7000h used)
	Bank 4: 7800h - 7FFFh (7800h used)
*/
#ifndef RAZE
// bank 1 ---------------------------------------------------------------------------
		CZetMapArea(	0x4000, 0x5fff, 0, &RAM[2][0x0000] );
		CZetMapArea(	0x4000, 0x5fff, 1, &RAM[2][0x0000] );
		CZetMapArea(	0x4000, 0x5fff, 2, &RAM[2][0x0000] );
// bank 2 ---------------------------------------------------------------------------
		CZetMapArea(	0x6000, 0x7fff, 0, &RAM[3][0x0000] );
		CZetMapArea(	0x6000, 0x7fff, 1, NULL );
		CZetMapArea(	0x6000, 0x7fff, 2, &RAM[3][0x0000] );
// bank 3 ---------------------------------------------------------------------------
		CZetMapArea(	0x8000, 0x9fff, 0, &RAM[4][0x0000] );
		CZetMapArea(	0x8000, 0x9fff, 1, &RAM[4][0x0000] );
		CZetMapArea(	0x8000, 0x9fff, 2, &RAM[4][0x0000] );
// bank 4 ---------------------------------------------------------------------------
		CZetMapArea(	0xA000, 0xbfff, 0, &RAM[5][0x0000] );
		CZetMapArea(	0xA000, 0xbfff, 1, &RAM[5][0x0000] );
		CZetMapArea(	0xA000, 0xbfff, 2, &RAM[5][0x0000] );
// end ------------------------------------------------------------------------------
		CZetMapArea(	0xc000, 0xdfff, 0, &RAM[6][0x0000]);
		CZetMapArea(	0xc000, 0xdfff, 2, &RAM[6][0x0000] );

		CZetMapArea(	0xe000, 0xffff, 0, &RAM[7][0x0000]);
		CZetMapArea(	0xe000, 0xffff, 2, &RAM[7][0x0000] ); 

	if(WriteMode[3])
	{
		CZetMapArea(	0xc000, 0xdfff, 1, &RAM[6][0x0000]);
		CZetMapArea(	0xe000, 0xffff, 1, &RAM[7][0x0000]);
	}
	else
	{
		CZetMapArea(	0xc000, 0xdfff, 1, NULL);
		CZetMapArea(	0xe000, 0xffff, 1, NULL);
	}
// end ------------------------------------------------------------------------------
#else
// bank 1 ---------------------------------------------------------------------------
		z80_map_read(	0x4000, 0x5fff, &RAM[2][0x0000] );
		z80_map_write(	0x4000, 0x5fff, &RAM[2][0x0000] );
		z80_map_fetch(	0x4000, 0x5fff, &RAM[2][0x0000] );
// bank 2 ---------------------------------------------------------------------------
		z80_map_read(	0x6000, 0x7fff, &RAM[3][0x0000] );
		z80_map_write(	0x6000, 0x7fff, NULL );
		z80_map_fetch(	0x6000, 0x7fff, &RAM[3][0x0000] );
// bank 3 ---------------------------------------------------------------------------
		z80_map_read(	0x8000, 0x9fff, &RAM[4][0x0000] );
		z80_map_write(	0x8000, 0x9fff, &RAM[4][0x0000] );
		z80_map_fetch(	0x8000, 0x9fff, &RAM[4][0x0000] );
// bank 4 ---------------------------------------------------------------------------
		z80_map_read(	0xA000, 0xbfff, &RAM[5][0x0000] );
		z80_map_write(	0xA000, 0xbfff, &RAM[5][0x0000] );
		z80_map_fetch(	0xA000, 0xbfff, &RAM[5][0x0000] );
// end ------------------------------------------------------------------------------
		z80_map_read(	0xc000, 0xdfff, &RAM[6][0x0000]);
		z80_map_fetch(	0xc000, 0xdfff, &RAM[6][0x0000] );

		z80_map_read(	0xe000, 0xffff, &RAM[7][0x0000]);
		z80_map_fetch(	0xe000, 0xffff, &RAM[7][0x0000] ); 

	if(WriteMode[3])
	{
		z80_map_write(	0xc000, 0xdfff, &RAM[6][0x0000]);
		z80_map_write(	0xe000, 0xffff, &RAM[7][0x0000]);
	}
	else
	{
		z80_map_write(	0xc000, 0xdfff, NULL);
		z80_map_write(	0xe000, 0xffff, NULL);
	}

#endif
}
//-----------------------------------------------------------------------------------------------------------------------------
void setFetchAscii16()
{
/*
 Since the size of the mapper is 16Kb, the memory banks are:

	Bank 1: 4000h - 7FFFh
	Bank 2: 8000h - BFFFh

And the address to change banks:

	Bank 1: 6000h - 67FFh (6000h used)
	Bank 2: 7000h - 77FFh (7000h and 77FFh used)
*/
#ifndef RAZE
// bank 1 ---------------------------------------------------------------------------
		CZetMapArea(	0x4000, 0x5fff, 0, &RAM[2][0x0000] );
		CZetMapArea(	0x4000, 0x5fff, 1, &RAM[2][0x0000] );
		CZetMapArea(	0x4000, 0x5fff, 2, &RAM[2][0x0000] );
// bank 2 ---------------------------------------------------------------------------
		CZetMapArea(	0x6000, 0x7fff, 0, &RAM[3][0x0000] );
		CZetMapArea(	0x6000, 0x67ff, 1, NULL );
		CZetMapArea(	0x6800, 0x6fff, 1, &RAM[3][0x0800] );
		CZetMapArea(	0x7000, 0x77ff, 1, NULL );
		CZetMapArea(	0x7800, 0x7fff, 1, &RAM[3][0x1800] );
		CZetMapArea(	0x6000, 0x7fff, 2, &RAM[3][0x0000] );
// bank 3 ---------------------------------------------------------------------------
		CZetMapArea(	0x8000, 0x9fff, 0, &RAM[4][0x0000] );
		CZetMapArea(	0x8000, 0x9fff, 1, &RAM[4][0x0000] );
		CZetMapArea(	0x8000, 0x9fff, 2, &RAM[4][0x0000] );
// bank 4 ---------------------------------------------------------------------------
		CZetMapArea(	0xA000, 0xbfff, 0, &RAM[5][0x0000] );
		CZetMapArea(	0xA000, 0xbfff, 1, &RAM[5][0x0000] );
		CZetMapArea(	0xA000, 0xbfff, 2, &RAM[5][0x0000] );
// end ------------------------------------------------------------------------------
		CZetMapArea(	0xc000, 0xdfff, 0, &RAM[6][0x0000]);
//		z80_map_write(	0xc000, 0xdfff, &RAM[6][0x0000]);
		CZetMapArea(	0xc000, 0xdfff, 2, &RAM[6][0x0000] );

		CZetMapArea(	0xe000, 0xffff, 0, &RAM[7][0x0000]);
//		z80_map_write(	0xe000, 0xffff, &RAM[7][0x0000]);
		CZetMapArea(	0xe000, 0xffff, 2, &RAM[7][0x0000] ); 

	if(WriteMode[3])
	{
		CZetMapArea(	0xc000, 0xdfff, 1, &RAM[6][0x0000]);
		CZetMapArea(	0xe000, 0xffff, 1, &RAM[7][0x0000]);
	}
	else
	{
		CZetMapArea(	0xc000, 0xdfff, 1, NULL);
		CZetMapArea(	0xe000, 0xffff, 1, NULL);
	}

#else
// bank 1 ---------------------------------------------------------------------------
		z80_map_read(	0x4000, 0x5fff, &RAM[2][0x0000] );
		z80_map_write(	0x4000, 0x5fff, &RAM[2][0x0000] );
		z80_map_fetch(	0x4000, 0x5fff, &RAM[2][0x0000] );
// bank 2 ---------------------------------------------------------------------------
		z80_map_read(	0x6000, 0x7fff, &RAM[3][0x0000] );
		z80_map_write(	0x6000, 0x67ff, NULL );
		z80_map_write(	0x6800, 0x6fff, &RAM[3][0x0800] );
		z80_map_write(	0x7000, 0x77ff, NULL );
		z80_map_write(	0x7800, 0x7fff, &RAM[3][0x1800] );
		z80_map_fetch(	0x6000, 0x7fff, &RAM[3][0x0000] );
// bank 3 ---------------------------------------------------------------------------
		z80_map_read(	0x8000, 0x9fff, &RAM[4][0x0000] );
		z80_map_write(	0x8000, 0x9fff, &RAM[4][0x0000] );
		z80_map_fetch(	0x8000, 0x9fff, &RAM[4][0x0000] );
// bank 4 ---------------------------------------------------------------------------
		z80_map_read(	0xA000, 0xbfff, &RAM[5][0x0000] );
		z80_map_write(	0xA000, 0xbfff, &RAM[5][0x0000] );
		z80_map_fetch(	0xA000, 0xbfff, &RAM[5][0x0000] );
// end ------------------------------------------------------------------------------
		z80_map_read(	0xc000, 0xdfff, &RAM[6][0x0000]);
//		z80_map_write(	0xc000, 0xdfff, &RAM[6][0x0000]);
		z80_map_fetch(	0xc000, 0xdfff, &RAM[6][0x0000] );

		z80_map_read(	0xe000, 0xffff, &RAM[7][0x0000]);
//		z80_map_write(	0xe000, 0xffff, &RAM[7][0x0000]);
		z80_map_fetch(	0xe000, 0xffff, &RAM[7][0x0000] ); 

	if(WriteMode[3])
	{
		z80_map_write(	0xc000, 0xdfff, &RAM[6][0x0000]);
		z80_map_write(	0xe000, 0xffff, &RAM[7][0x0000]);
	}
#endif
}
//-----------------------------------------------------------------------------------------------------------------------------
void setFetchKonami4SCC()
{
/*
 Since the size of the mapper is 8Kb, the memory banks are:

	Bank 1: 4000h - 5FFFh
	Bank 2: 6000h - 7FFFh
	Bank 3: 8000h - 9FFFh
	Bank 4: A000h - BFFFh

And the address to change banks:

	Bank 1: 5000h - 57FFh (5000h used)
	Bank 2: 7000h - 77FFh (7000h used)
	Bank 3: 9000h - 97FFh (9000h used)
	Bank 4: B000h - B7FFh (B000h used)

*/
#ifndef RAZE
// bank 1 ---------------------------------------------------------------------------
		CZetMapArea(	0x4000, 0x5fff, 0, &RAM[2][0x0000] );
		CZetMapArea(	0x4000, 0x4fff, 1, &RAM[2][0x0000] );
		CZetMapArea(	0x5800, 0x5fff, 1, &RAM[2][0x1800] );
		CZetMapArea(	0x4000, 0x5fff, 2, &RAM[2][0x0000] );
// bank 2 ---------------------------------------------------------------------------
		CZetMapArea(	0x6000, 0x7fff, 0, &RAM[3][0x0000] );
		CZetMapArea(	0x6000, 0x6fff, 1, &RAM[3][0x0000] );
		CZetMapArea(	0x7800, 0x7fff, 1, &RAM[3][0x1800] );
		CZetMapArea(	0x6000, 0x7fff, 2, &RAM[3][0x0000] );
// bank 3 ---------------------------------------------------------------------------
		CZetMapArea(	0x8000, 0x9fff, 0, &RAM[4][0x0000] );
		CZetMapArea(	0x8000, 0x8fff, 1, &RAM[4][0x0000] );
		CZetMapArea(	0x9800, 0x9fff, 1, &RAM[4][0x1800] );
		CZetMapArea(	0x8000, 0x9fff, 2, &RAM[4][0x0000] );
// bank 4 ---------------------------------------------------------------------------
		CZetMapArea(	0xa000, 0xbfff, 0, &RAM[5][0x0000] );
		CZetMapArea(	0xa000, 0xafff, 1, &RAM[5][0x0000] );
		CZetMapArea(	0xb800, 0xbfff, 1, &RAM[5][0x1800] );
		CZetMapArea(	0xa000, 0xbfff, 2, &RAM[5][0x0000] );
// end ------------------------------------------------------------------------------
		CZetMapArea(	0xc000, 0xdfff, 0, &RAM[6][0x0000]);
		CZetMapArea(	0xc000, 0xdfff, 1, &RAM[6][0x0000]);
		CZetMapArea(	0xc000, 0xdfff, 2, &RAM[6][0x0000] );

		CZetMapArea(	0xe000, 0xffff, 0, &RAM[7][0x0000]);
		CZetMapArea(	0xe000, 0xffff, 1, &RAM[7][0x0000]);
		CZetMapArea(	0xe000, 0xffff, 2, &RAM[7][0x0000] );
#else
// bank 1 ---------------------------------------------------------------------------
		z80_map_read(	0x4000, 0x5fff, &RAM[2][0x0000] );
		z80_map_write(	0x4000, 0x4fff, &RAM[2][0x0000] );
		z80_map_write(	0x5800, 0x5fff, &RAM[2][0x1800] );
		z80_map_fetch(	0x4000, 0x5fff, &RAM[2][0x0000] );
// bank 2 ---------------------------------------------------------------------------
		z80_map_read(	0x6000, 0x7fff, &RAM[3][0x0000] );
		z80_map_write(	0x6000, 0x6fff, &RAM[3][0x0000] );
		z80_map_write(	0x7800, 0x7fff, &RAM[3][0x1800] );
		z80_map_fetch(	0x6000, 0x7fff, &RAM[3][0x0000] );
// bank 3 ---------------------------------------------------------------------------
		z80_map_read(	0x8000, 0x9fff, &RAM[4][0x0000] );
		z80_map_write(	0x8000, 0x8fff, &RAM[4][0x0000] );
		z80_map_write(	0x9800, 0x9fff, &RAM[4][0x1800] );
		z80_map_fetch(	0x8000, 0x9fff, &RAM[4][0x0000] );
// bank 4 ---------------------------------------------------------------------------
		z80_map_read(	0xa000, 0xbfff, &RAM[5][0x0000] );
		z80_map_write(	0xa000, 0xafff, &RAM[5][0x0000] );
		z80_map_write(	0xb800, 0xbfff, &RAM[5][0x1800] );
		z80_map_fetch(	0xa000, 0xbfff, &RAM[5][0x0000] );
// end ------------------------------------------------------------------------------
		z80_map_read(	0xc000, 0xdfff, &RAM[6][0x0000]);
		z80_map_write(	0xc000, 0xdfff, &RAM[6][0x0000]);
		z80_map_fetch(	0xc000, 0xdfff, &RAM[6][0x0000] );

		z80_map_read(	0xe000, 0xffff, &RAM[7][0x0000]);
		z80_map_write(	0xe000, 0xffff, &RAM[7][0x0000]);
		z80_map_fetch(	0xe000, 0xffff, &RAM[7][0x0000] );
#endif
}
//-----------------------------------------------------------------------------------------------------------------------------
void setFetchKonami4()
{
/*
 Since the size of the mapper is 8Kb, the memory banks are:

	Bank 1: 4000h - 5FFFh
	Bank 2: 6000h - 7FFFh
	Bank 3: 8000h - 9FFFh
	Bank 4: A000h - BFFFh

And the address to change banks:

	Bank 1: <none>
	Bank 2: 6000h - 7FFFh (6000h used)
	Bank 3: 8000h - 9FFFh (8000h used)
	Bank 4: A000h - BFFFh (A000h used)
*/
#ifndef RAZE
// bank 1 ---------------------------------------------------------------------------
		CZetMapArea(	0x4000, 0x5fff, 0, &RAM[2][0x0000] );
		CZetMapArea(	0x4000, 0x5fff, 1, &RAM[2][0x0000] );
		CZetMapArea(	0x4000, 0x5fff, 2, &RAM[2][0x0000] );
// bank 2 ---------------------------------------------------------------------------
		CZetMapArea(	0x6000, 0x7fff, 0, &RAM[3][0x0000] );
		CZetMapArea(	0x6000, 0x7fff, 2, &RAM[3][0x0000] );
// bank 3 ---------------------------------------------------------------------------
		CZetMapArea(	0x8000, 0x9fff, 0, &RAM[4][0x0000] );
		CZetMapArea(	0x8000, 0x9fff, 2, &RAM[4][0x0000] );
// bank 4 ---------------------------------------------------------------------------
		CZetMapArea(	0xa000, 0xbfff, 0, &RAM[5][0x0000] );
		CZetMapArea(	0xa000, 0xbfff, 2, &RAM[5][0x0000] );
// end ------------------------------------------------------------------------------
		CZetMapArea(	0xc000, 0xdfff, 0, &RAM[6][0x0000]);
		CZetMapArea(	0xc000, 0xdfff, 2, &RAM[6][0x0000] );

		CZetMapArea(	0xe000, 0xffff, 0, &RAM[7][0x0000]);
		CZetMapArea(	0xe000, 0xffff, 2, &RAM[7][0x0000] );

//	if(WriteMode[3])
	{
		CZetMapArea(	0xc000, 0xdfff, 1, &RAM[6][0x0000]);
		CZetMapArea(	0xe000, 0xffff, 1, &RAM[7][0x0000]);
	}
#else
// bank 1 ---------------------------------------------------------------------------
		z80_map_read(	0x4000, 0x5fff, &RAM[2][0x0000] );
		z80_map_write(	0x4000, 0x5fff, &RAM[2][0x0000] );
		z80_map_fetch(	0x4000, 0x5fff, &RAM[2][0x0000] );
// bank 2 ---------------------------------------------------------------------------
		z80_map_read(	0x6000, 0x7fff, &RAM[3][0x0000] );
		z80_map_fetch(	0x6000, 0x7fff, &RAM[3][0x0000] );
// bank 3 ---------------------------------------------------------------------------
		z80_map_read(	0x8000, 0x9fff, &RAM[4][0x0000] );
		z80_map_fetch(	0x8000, 0x9fff, &RAM[4][0x0000] );
// bank 4 ---------------------------------------------------------------------------
		z80_map_read(	0xa000, 0xbfff, &RAM[5][0x0000] );
		z80_map_fetch(	0xa000, 0xbfff, &RAM[5][0x0000] );
// end ------------------------------------------------------------------------------
		z80_map_read(	0xc000, 0xdfff, &RAM[6][0x0000]);
		z80_map_fetch(	0xc000, 0xdfff, &RAM[6][0x0000] );

		z80_map_read(	0xe000, 0xffff, &RAM[7][0x0000]);
		z80_map_fetch(	0xe000, 0xffff, &RAM[7][0x0000] );

	if(WriteMode[3])
	{
		z80_map_write(	0xc000, 0xdfff, &RAM[6][0x0000]);
		z80_map_write(	0xe000, 0xffff, &RAM[7][0x0000]);
	}
	else
	{
		z80_map_write(	0xc000, 0xdfff, NULL);
		z80_map_write(	0xe000, 0xffff, NULL);
	
	}
#endif
}
//-----------------------------------------------------------------------------------------------------------------------------
static void setFetch(UINT32 I, UINT8 *ram)
{
	UINT32 addr1=0x2000*I;
#ifndef RAZE
	CZetMapArea(addr1, addr1+0x1fff, 0, ram); // working with zet
	CZetMapArea(addr1, addr1+0x1fff, 2, ram); // working with zet
#else
//	z80_map_read (addr1, addr1+0x1fff, &RAM[I][0x0000] ); // working with zet
	z80_map_fetch(addr1, addr1+0x1fff, ram); // working with zet
	z80_map_read(addr1, addr1+0x1fff,  ram);
#endif
}
//-----------------------------------------------------------------------------------------------------------------------------

static void SetSlot(UINT8 nSlot)
{
	UINT32 I, J;

	if (PSLReg != nSlot) 
	{
		PSLReg = nSlot;
		for (J = 0; J < 4; J++)	{
			I = J << 1;
			PSL[J] = nSlot & 3;
			RAM[I] = MemMap[PSL[J]][I];
			RAM[I + 1] = MemMap[PSL[J]][I + 1];
			WriteMode[J] = (PSL[J] == RAMSLOT) && (MemMap[RAMSLOT][I] != EmptyRAM);
			setFetch(I, RAM[I]);
			setFetch(I+1, RAM[I+1]);
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

	ROMMask[nSlot]= !Flat64 && (Len > 4) ? (Pages - 1) : 0x00;

	if (ROMMask[nSlot] + 1 > 4) 
	{
		ROMType[nSlot] = GuessROM(ROMData[nSlot], 0x2000 * (ROMMask[nSlot] + 1));
	}

	UINT8 filename[12];

	strcpy(filename,GFS_IdToName(file_id));	
	if (   strcmp(filename, "VALIS.ROM") == 0
		|| strcmp(filename, "1942.ROM") == 0
		|| strcmp(filename, "HYDLIDE3.ROM") == 0
		|| strcmp(filename, "DRAGONQ1.ROM") == 0
		|| strcmp(filename, "DRAGONQ2.ROM") == 0
		|| strcmp(filename, "FNTSYZON.ROM") == 0
		|| strcmp(filename, "XANADU.ROM") == 0
		)
		ROMType[nSlot] = MAP_ASCII8;
	if (   strcmp(filename, "GOLVEL.ROM") == 0 
		|| strcmp(filename, "CRAZE.ROM") == 0
		|| strcmp(filename, "GALLFRCE.ROM") == 0
		|| strcmp(filename, "EGGRLND2.ROM") == 0
		|| strcmp(filename, "VAXOL.ROM") == 0
		|| strcmp(filename, "ADVENKID.ROM") == 0)
		ROMType[nSlot] = MAP_ASCII16;
	if (   strcmp(filename, "1942K.ROM") == 0 
		|| strcmp(filename, "PENGADV.ROM") == 0
		|| strcmp(filename, "ROBOCOP.ROM") == 0
		|| strcmp(filename, "VALISK.ROM") == 0
		|| strcmp(filename, "SLAYDOKK.ROM") == 0
		)
		ROMType[nSlot] = MAP_KONAMI4;
	if (   strcmp(filename, "SALAMAND.ROM") == 0 
		|| strcmp(filename, "SALAMANK.ROM") == 0
		|| strcmp(filename, "MRMOLE.ROM") == 0
		|| strcmp(filename, "GRADIUS2.ROM") == 0
		|| strcmp(filename, "A1SPIRIT.ROM") == 0
		|| strcmp(filename, "F1SPIRIT.ROM") == 0
		)
		ROMType[nSlot] = MAP_KONAMI5;


	if (   strcmp(filename, "INDY500.ROM") == 0
		|| strcmp(filename, "IKARATE.ROM") == 0
		)
		ROMType[nSlot] = MAP_KONGEN16;
/*
<derek> invasion of the zombie monsters
<derek> chick fighter ;)
<derek> The Goonies
<gamezfan> Goonies :)
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

			case 3: // MAP_KONAMI4
			case 4: // 24k & 32k rom-mirroring
				if (GetRomStart(cartbuf, cartsize) != 0x4000) {
					PageMap(nSlot, "0:1:0:1:2:3:2:3"); // normal
				} else {
					PageMap(nSlot, "2:3:0:1:2:3:0:1"); // swapped
				}
				break;

		default:
			if (Flat64)
			{ // Flat-64k ROM
				PageMap(nSlot, "0:1:2:3:4:5:6:7");
			}
			break;
		}
	}

	// map gen/16k megaROM pages 0:1:last-1:last
	if ((ROMType[nSlot] == MAP_KONGEN16) && (ROMMask[nSlot] + 1 > 4))
	{
		MapMegaROM(nSlot, 0, 1, ROMMask[nSlot] - 1, ROMMask[nSlot]);
	}
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
	}
}

static UINT8 __fastcall msx_read_port(UINT16 port)
{
	port &= 0xff;

	switch (port)
	{
		case 0x98:
			return SG_TMS9928AReadVRAM();

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
#ifdef DAC
	if (DrvDips[0] & 0x02)
	{
		DACWrite(0, (data & 0x80) ? 0x80 : 0x00); // Key-Clicker / 1-bit DAC
	}
#endif
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
#ifdef RAZE
	if(state)
		z80_raise_IRQ(0);
	else
		z80_lower_IRQ();
#else
	CZetSetIRQLine(0, state ? ZET_IRQSTATUS_ACK : ZET_IRQSTATUS_NONE);
#endif
}

static void DrvDoReset()
{
	memset (AllRam, 0, RamEnd - AllRam);

	memset(&keyRows, 0, sizeof(keyRows));
	ppiC_row = 0;

	Kana = 0;
	KanaByte = 0;

	SwapJoyports = 0;
	Joyselect = 0;
	Hertz60 = 0;
	BiosmodeJapan = 0;
	VBlankKludge = 0;

	msxinit(CurRomSizeA);

	PSLReg = 99;

	for (UINT8 J = 0; J < 4; J++)	
	{
		UINT8 I = J << 1;
		setFetch(I,   RAM[I]);
		setFetch(I+1, RAM[I+1]);
	}

	switch (ROMType[CARTSLOTA])
	{
		case MAP_KONAMI4:
			setFetchKonami4();
#ifdef RAZE
			if(WriteMode[3])
				z80_add_write(0x6000, 0xbfff, 1, (void *)&msx_write);
			else
				z80_add_write(0x6000, 0xffff, 1, (void *)&msx_write);
#endif
			break;

		case MAP_KONAMI5:
			setFetchKonami4SCC();
#ifdef RAZE
			z80_add_write(0x5000, 0x8fff, 1, (void *)&msx_write_scc2);
			z80_add_write(0x9000, 0x9fff, 1, (void *)&msx_write_scc);
			z80_add_write(0xa000, 0xffff, 1, (void *)&msx_write_scc2);
//			z80_add_write(0x5000, 0x8fff, 1, (void *)&msx_write_konami4scc);
//			z80_add_write(0x9000, 0x9fff, 1, (void *)&msx_write_scc);
//			z80_add_write(0xa000, 0xbfff, 1, (void *)&msx_write_konami4scc);
#endif
			break;

		case MAP_RTYPE:
			setFetchRType();
#ifdef RAZE
			z80_add_write(0x4000, 0xffff, 1, (void *)&msx_write);
#endif
			break;

		case MAP_KONGEN8:
			setFetchKonGen8();
#ifdef RAZE
			if(WriteMode[3])
				z80_add_write(0x4000, 0xbfff, 1, (void *)&msx_write);
			else
//				z80_add_write(0x4000, 0xbfff, 1, (void *)NULL);
//			else
				z80_add_write(0x4000, 0xffff, 1, (void *)&msx_write);
//				z80_add_write(0xc000, 0xffff, 1, (void *)EmptyRAM);
#endif
			break;

		case MAP_KONGEN16:
			
			break;

		case MAP_ASCII8:
			setFetchAscii8();
#ifdef RAZE
			if(WriteMode[3])
				z80_add_write(0x6000, 0xbfff, 1, (void *)&msx_write);
			else
				z80_add_write(0x6000, 0xffff, 1, (void *)&msx_write);
#endif
			break;

		case MAP_ASCII16:
			setFetchAscii16();
#ifdef RAZE
//			z80_add_write(0x6000, 0xffff, 1, (void *)&msx_write);
			if(WriteMode[3])
				z80_add_write(0x6000, 0xbfff, 1, (void *)&msx_write);
			else
				z80_add_write(0x6000, 0xffff, 1, (void *)&msx_write);
#endif
			break;
	}

	ppi8255_init(1); // there is no reset, so use this.
#ifdef RAZE
	z80_reset();
#else
	CZetOpen(0);
	CZetReset();
#endif
	TMS9928AReset();
#ifndef RAZE
	CZetClose();
#endif
	AY8910Reset(0);
#ifdef K051649
	K051649Reset();
#endif
#ifdef DAC
	DACReset();
#endif
}

static INT32 MemIndex()
{
	UINT8 *Next; Next = AllMem;
	maincpu		    = Next; Next += 0x020000;
	game		    = (UINT8 *)0x00200000; //MAX_MSX_CARTSIZE;

#ifdef CASSETTE
	game2		    = (UINT8 *)0x00280000; //MAX_MSX_CARTSIZE;
#endif

#ifdef KANJI
	kanji_rom       = Next; Next += 0x040000;
#endif
	game_sram       = Next; Next += 0x004000;

	AllRam			= Next;

	main_mem		= Next; Next += 0x020000;
	EmptyRAM        = Next; Next += 0x010000;

	RamEnd			= Next;
//	tmpbmp				= Next; Next += (256*192);

#ifdef K051649
	SCCMixerBuffer	= (INT16*)Next; Next += 2 * 7680L * sizeof(INT16);
	SCCMixerTable	= (INT16*)Next; Next += 512 * 5 * sizeof(INT16);
#endif
	CZ80Context		= Next; Next += 0x1080;
	MemEnd			= Next;
	return 0;
}
#ifdef RAZE
static void __fastcall msx_write_konami4(UINT16 address, UINT8 data)
{
	UINT32 Page = address >> 14; // pg. num
	UINT32 PSlot = PSL[Page];

	if (PSlot >= MAXSLOTS) return;

//	if (!ROMData[PSlot] || !ROMMask[PSlot]) return;

	Page = (address - 0x4000) >> 13;

	data &= ROMMask[PSlot];
	if (data != ROMMapper[PSlot][Page])
	{
		RAM[Page + 2] = MemMap[PSlot][Page + 2] = ROMData[PSlot] + (data << 13);
		setFetch(Page + 2, RAM[Page + 2]);
		ROMMapper[PSlot][Page] = data;
	}
}

static void __fastcall msx_write_scc(UINT16 address, UINT8 data)
{
	UINT8 Page = address >> 14; // pg. num
	UINT32 PSlot = PSL[Page];

	if (PSlot >= MAXSLOTS) return;

	if (!ROMData[PSlot] || !ROMMask[PSlot]) { return; }

	if (!ROMData[PSlot] && (address == 0x9000))
		SCCReg[PSlot] = (data == 0x3f) ? 1 : 0;

#ifdef K051649
	if (((address & 0xdf00) == 0x9800) && SCCReg[PSlot]) // Handle Konami-SCC (+)
	{ // Handle Konami-SCC (+)
		UINT16 offset = address & 0x00ff;


		if (offset < 0x80) {
			K051649WaveformWrite(offset, data);
		}
		else
			if (offset < 0xa0)	{
			offset &= 0xf;

			if (offset < 0xa) {
				K051649FrequencyWrite(offset, data);
			}
			else if (offset < 0xf) {
				K051649VolumeWrite(offset - 0xa, data);
			}
			else {
				K051649KeyonoffWrite(data);
			}
		}
		return;
	}

	Page = (address - 0x5000) >> 13;

	if (Page == 2) SCCReg[PSlot] = (data == 0x3f) ? 1 : 0;

	data &= ROMMask[PSlot];
	if (data != ROMMapper[PSlot][Page])
	{
		RAM[Page + 2] = MemMap[PSlot][Page + 2] = ROMData[PSlot] + (data << 13);
		setFetch(Page + 2,RAM[Page + 2]);
		ROMMapper[PSlot][Page] = data;
	}

#endif
}

static void __fastcall msx_write_konami4scc(UINT16 address, UINT8 data)
{
	UINT32 Page = address >> 14; // pg. num
	UINT32 PSlot = PSL[Page];

	if (PSlot >= MAXSLOTS) return;

//	if (!ROMData[PSlot] || !ROMMask[PSlot]) return;

	Page = (address - 0x5000) >> 13;

	if (Page == 2) SCCReg[PSlot] = (data == 0x3f) ? 1 : 0;
	
	data &= ROMMask[PSlot];
	if (data != ROMMapper[PSlot][Page])
	{
		RAM[Page + 2] = MemMap[PSlot][Page + 2] = ROMData[PSlot] + (data << 13);
		setFetch(Page + 2,RAM[Page + 2]);
		ROMMapper[PSlot][Page] = data;
	}
}

static void __fastcall msx_write_ascii8(UINT16 address, UINT8 data)
{
	UINT32 Page = address >> 14; // pg. num
	UINT32 PSlot = PSL[Page];

	if (PSlot >= MAXSLOTS) return;

	if (!ROMData[PSlot] || !ROMMask[PSlot]) return;

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
			{
				RAM[Page + 2] = pgPtr;
				setFetch(Page + 2, pgPtr);
	//						setFetchAscii8();
			}
		}
		return;
	}

	if ((address >= 0x8000) && (address < 0xc000) && (ROMMapper[PSlot][((address >> 13) & 1) + 2] == 0xff))
	{
		RAM[address >> 13][address & 0x1fff] = data;
		return;
	}
}
#endif

static void __fastcall msx_write(UINT16 address, UINT8 data)
{
	if (WriteMode[address >> 14]) 
	{
		RAM[address >> 13][address & 0x1fff] = data;
		return;
	}

	if ((address > 0x3fff) && (address < 0xc000))
	{
		Mapper_write(address, data);
	}
}

static void __fastcall msx_write_scc2(UINT16 address, UINT8 data)
{
	if (WriteMode[address >> 14]) 
	{
		RAM[address >> 13][address & 0x1fff] = data;
		return;
	}

	if ((address > 0x3fff) && (address < 0xc000))
	{
		Mapper_write_scc(address, data);
	}
}

#ifdef DAC
static INT32 DrvSyncDAC()
{
#ifdef RAZE
	return (INT32)(float)(nBurnSoundLen * (z80_get_cycles_elapsed() / (3579545.000 / ((Hertz60) ? 60.0 : 50.0))));
#else
	return (INT32)(float)(nBurnSoundLen * (CZetTotalCycles() / (3579545.000 / ((Hertz60) ? 60.0 : 50.0))));
#endif
}
#endif

static INT32 DrvInit()
{
	DrvInitSaturn();
	AllMem = NULL;
	MemIndex();
	INT32 nLen = MemEnd - (UINT8 *)0;

	if ((AllMem = (UINT8 *)BurnMalloc(nLen)) == NULL) return 1;
	memset(AllMem, 0, nLen);
	MemIndex();

	{
		struct BurnRomInfo ri;

		DrvDips[0] = 0x11;

		Hertz60 = (DrvDips[0] & 0x10) ? 1 : 0;
		BiosmodeJapan = (DrvDips[0] & 0x01) ? 1 : 0;
		SwapJoyports = (DrvDips[0] & 0x20) ? 1 : 0;

//		if (BurnLoadRom(maincpu, 0x80 + BiosmodeJapan, 1)) return 1; // BIOS
	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Loading. Please Wait",24,40);

		if (BurnLoadRom(maincpu, 1 + BiosmodeJapan, 1)) return 1; // BIOS
#ifdef KANJI
//		use_kanji = (BurnLoadRom(kanji_rom, 0x82, 1) == 0);
		use_kanji = (BurnLoadRom(kanji_rom, 3, 1) == 0);
		use_kanji = 1;
#endif
		memset(game, 0xff, MAX_MSX_CARTSIZE);

		BurnDrvGetRomInfo(&ri, 0);
		ri.nLen = GetFileSize(file_id);

		GFS_Load(file_id, 0, game, ri.nLen);
		CurRomSizeA = ri.nLen;

#ifdef CASSETTE
		BurnDrvGetRomInfo(&ri, 1);

		if (ri.nLen > 0 && ri.nLen < MAX_MSX_CARTSIZE) {
			memset(game2, 0xff, MAX_MSX_CARTSIZE);

			if (BurnLoadRom(game2 + 0x00000, 1, 1)) return 1;

			CurRomSizeB = ri.nLen;
		}
#endif
	}
#ifdef RAZE
	z80_init_memmap();

 	z80_map_fetch (0x0000, 0x3fff, maincpu); 
	z80_map_read  (0x0000, 0x3fff, maincpu);
	z80_end_memmap();   

	z80_set_in((unsigned char (*)(unsigned short))&msx_read_port);
	z80_set_out((void (*)(unsigned short, unsigned char))&msx_write_port);

#else
	CZetInit2(1,CZ80Context);
	CZetOpen(0);
	CZetMapArea(0x0000, 0x3fff, 0, maincpu);
    CZetMapArea(0x0000, 0x3fff, 2, maincpu);

	CZetSetOutHandler(msx_write_port);
	CZetSetInHandler(msx_read_port);

	CZetSetWriteHandler(msx_write);
	CZetClose();
#endif
	AY8910Init(0, 3579545/2, nBurnSoundRate, ay8910portAread, NULL, ay8910portAwrite, ay8910portBwrite);

#ifdef K051649
	K051649Init(3579545/2,SCCMixerBuffer,SCCMixerTable);
#endif

#ifdef DAC
	DACInit(0, 0, 1, DrvSyncDAC);
#endif
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"TMS9928AInit      ",26,210);

	TMS9928AInit(TMS99x8A, 0x4000, 0, 0, vdp_interrupt);

	ppi8255_init(1);
	PPI0PortReadB	= msx_ppi8255_portB_read;
	PPI0PortWriteA	= msx_ppi8255_portA_write;
	PPI0PortWriteC	= msx_ppi8255_portC_write;
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"after init        ",4,80);
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"DrvDoReset      ",26,210);

	DrvDoReset();
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"after reset        ",4,80);
	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"                    ",24,40);

	return 0;
}
/*
void cleanmemmap()
{
	for(INT32 PSlot = 0; PSlot < 4; PSlot++)
	{
		for(INT32 Page = 0; Page < 8; Page++)
		{
			MemMap[PSlot][Page] = NULL;
		}
	}

	for(INT32 Page = 0; Page < 8; Page++)
	{
		RAM[Page] = NULL;
	}
#ifdef RAZE
	switch (ROMType[CARTSLOTA])
	{
		case MAP_KONAMI4:
			setFetchKonami4();
			z80_add_write(0x6000, 0xffff, 1, (void *)NULL);
			z80_add_write(0x6000, 0xbfff, 1, (void *)NULL);
		break;

		case MAP_KONAMI5:
			setFetchKonami4SCC();
			z80_add_write(0x5000, 0x8fff, 1, (void *)NULL);
			z80_add_write(0x9000, 0x9fff, 1, (void *)NULL);
			z80_add_write(0xa000, 0xbfff, 1, (void *)NULL);
			break;

		case MAP_RTYPE:
			setFetchRType();
			z80_add_write(0x4000, 0xffff, 1, (void *)NULL);
			break;

		case MAP_KONGEN8:
			setFetchKonGen8();
			z80_add_write(0x4000, 0xffff, 1, (void *)NULL);
			z80_add_write(0x4000, 0xbfff, 1, (void *)NULL);
			break;

		case MAP_KONGEN16:
			break;

		case MAP_ASCII8:
			setFetchAscii8();
			z80_add_write(0x6000, 0xffff, 1, (void *)NULL);
			z80_add_write(0x6000, 0xbfff, 1, (void *)NULL);
			break;

		case MAP_ASCII16:
			setFetchAscii16();
			z80_add_write(0x6000, 0xffff, 1, (void *)NULL);
			z80_add_write(0x6000, 0xbfff, 1, (void *)NULL);
			break;
	}
#endif
	for (UINT8 J = 0; J < 4; J++)	
	{
		UINT8 I = J << 1;
		WriteMode[J] = 0;
		PSL[J] = 0;
		RAMMapper[J] = 3 - J;
		setFetch(I,   RAM[I]);
		setFetch(I+1, RAM[I+1]);
	}
	PSLReg = 99;

#ifdef RAZE
	z80_set_in((unsigned char (*)(unsigned short))NULL);
	z80_set_out((void (*)(unsigned short, unsigned char))NULL);

	z80_add_read(0x0000, 0xffff, 1, (void *)NULL);
	z80_add_write(0x0000, 0xffff, 1, (void *)NULL);
	z80_lower_IRQ();

	for (unsigned int x=0; x<20;x++)
	{
		z80_set_reg(x,0);
	}
#endif
}
*/

static INT32 DrvExit()
{
	nBurnFunction = NULL;
#ifdef RAZE
	z80_stop_emulating();
	z80_add_write(0x4000, 0xffff, 1, (void *)NULL);
	z80_set_in((unsigned char (*)(unsigned short))NULL);
	z80_set_out((void (*)(unsigned short, unsigned char))NULL);

#else
	CZetExit2();
#endif

//cleanmemmap();

	for(unsigned int i=0;i<8;i++)
	{
		PCM_MeStop(pcm8[i]);
	}
	memset4_fast(SOUND_BUFFER,0x00,0x4000*9);

	TMS9928AExit();
#ifdef K051649
	K051649Exit();
#endif
	AY8910Exit(0);
#ifdef DAC
	DACExit();
#endif
	ppi8255_exit();

//	for (int i = 0; i < 3; i++) {
//		pAY8910Buffer[i] = NULL;
//	}

	maincpu = game = game_sram = AllRam = main_mem = NULL;
	EmptyRAM = RAMData = MemEnd = RamEnd = NULL;

#ifdef K051649
	SCCMixerBuffer	= NULL;
	SCCMixerTable	= NULL;
#endif
	/*tmpbmp =*/ CZ80Context		= NULL;

	BurnFree (AllMem);
	AllMem = NULL;
	
	DrvReset = 0;
	DrvNMI = 0;

	msx_basicmode = 0;
	BiosmodeJapan = 0;
#ifdef CASSETTE
	CASMode = 0;
#endif
	VBlankKludge = 0;

#ifdef BUILD_WIN32
	cBurnerKeyCallback = NULL;
	nReplayExternalDataCount = 0;
	ReplayExternalData = NULL;
#endif
	return 0;
}

static void DrvFrame()
{
	//	SPR_InitSlaveSH();

	if (stop==0)
	{
		 UINT8 lastnmi = 0;

	// Compile Inputs
		memset (DrvInputs, 0xff, 2);
		for (UINT32 i = 0; i < 8; i++) 
		{
			DrvInputs[0] ^= (DrvJoy1[i] & 1) << i;
			DrvInputs[1] ^= (DrvJoy2[i] & 1) << i;
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

//	UINT32 nInterleave = 256; //256; 
	UINT32 nCyclesTotal = 3579545 / ((Hertz60) ? 60 : 50);

#ifdef RAZE
		if (DrvNMI && !lastnmi) 
		{
			z80_cause_NMI();
			 z80_emulate(1);
			lastnmi = DrvNMI;
		} else lastnmi = DrvNMI;
#else
		CZetNewFrame();
		CZetOpen(0);

		if (DrvNMI && !lastnmi) 
		{
			CZetNmi();
			lastnmi = DrvNMI;
		} else lastnmi = DrvNMI;
#endif
//	unsigned char *ss_vram = (unsigned char *)SS_SPRAM;

//	DMA_ScuIndirectMemCopy((ss_vram+0x1100+0x10000),tmpbmp,0x4000,0);
	switch (ROMType[CARTSLOTA])
	{
		case MAP_KONAMI5:
			SPR_RunSlaveSH((PARA_RTN*)updateSlaveSoundSCC, NULL);
			break;
		default:
			SPR_RunSlaveSH((PARA_RTN*)updateSlaveSound, NULL);
			break;
	}

	//	for (UINT32 i = 0; i < nInterleave; i++)
		{
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"z80_emulate      ",26,210);

#ifdef RAZE
			/*nCyclesDone +=*/ z80_emulate(nCyclesTotal);// / nInterleave);
#else
			/*nCyclesDone +=*/ CZetRun(nCyclesTotal); // / nInterleave);
#endif
	//		TMS9928AScanline(i);
		}
#ifndef RAZE
		CZetClose();
#endif
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"TMS9928AInterrupt      ",26,210);

	TMS9928AInterrupt();
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"SPR_WaitEndSlaveSH      ",26,210);

	SPR_WaitEndSlaveSH();

#ifdef DAC
			volatile signed short *	pBurnSoundOut = (signed short *)0x25a20000;
			DACUpdate(pBurnSoundOut, nBurnSoundLen);
#endif
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT, (Uint8 *)"end SPR_WaitEndSlaveSH      ",26,210);

	}
	else
		load_rom();
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void updateSlaveSound()
{
	unsigned int deltaSlave    = *(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);
	unsigned short *nSoundBuffer1 = (unsigned short *)0x25a24000+deltaSlave;
//	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"updateSlaveSound   ",24,50);

	AY8910UpdateDirect(0, &nSoundBuffer1[0], nBurnSoundLen);
	deltaSlave+=nBurnSoundLen;

	if(deltaSlave>=RING_BUF_SIZE>>1)
	{
		PCM_NotifyWriteSize(pcm8[0], deltaSlave);
		PCM_NotifyWriteSize(pcm8[1], deltaSlave);
		PCM_NotifyWriteSize(pcm8[2], deltaSlave);

		deltaSlave=0;
		PCM_Task(pcm8[0]); // bon emplacement
		PCM_Task(pcm8[1]); // bon emplacement
		PCM_Task(pcm8[2]); // bon emplacement
	}
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = deltaSlave;
	TMS9928ADraw();
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void updateSlaveSoundSCC()
{
	unsigned int deltaSlave    = *(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos);
	unsigned short *nSoundBuffer1 = (unsigned short *)0x25a24000+deltaSlave;

	AY8910UpdateDirect(0, &nSoundBuffer1[0], nBurnSoundLen);
#ifdef K051649 
	K051649UpdateDirect(&nSoundBuffer1[0x6000], nBurnSoundLen);
#endif
	deltaSlave+=nBurnSoundLen;

	if(deltaSlave>=RING_BUF_SIZE>>1)
	{
		for (unsigned int i=0;i<8;i++)
		{
			PCM_NotifyWriteSize(pcm8[i], deltaSlave);
			PCM_Task(pcm8[i]); // bon emplacement
		}
		deltaSlave=0;
	}
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = deltaSlave;
	TMS9928ADraw();
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void initColors()
{
	memset(SclColRamAlloc256,0,sizeof(SclColRamAlloc256));
	colAddr			= (Uint16*)SCL_AllocColRam(SCL_SPR,OFF);
	(Uint16*)SCL_AllocColRam(SCL_NBG1,OFF);
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
void dummy()
{

}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void DrvInitSaturn()
{
	SPR_InitSlaveSH();
	SPR_RunSlaveSH((PARA_RTN*)dummy, NULL);

	nBurnSprites  = 4+32;//131;//27;
	nBurnLinescrollSize = 0;
	nSoundBufferPos = 0;//sound position à renommer

	SS_CACHE = cache      =(Uint8  *)SCL_VDP2_VRAM_B1;
	SS_MAP     = ss_map   =(Uint16 *)SCL_VDP2_VRAM_B0;
	SS_FONT   = ss_font    =(Uint16 *)SCL_VDP2_VRAM_A1;

	ss_BgPriNum     = (SclBgPriNumRegister *)SS_N0PRI;
	ss_SpPriNum     = (SclSpPriNumRegister *)SS_SPPRI;

	ss_sprite		= (SprSpCmd *)SS_SPRIT;

	ss_sprite[3].ax = 0;
	ss_sprite[3].ay = 0;

	ss_sprite[3].color          = 0x0;
	ss_sprite[3].charAddr    = 0x2220;// 0x2000 => 0x80 sprites <<6
	ss_sprite[3].control       = ( JUMP_NEXT | FUNC_NORMALSP); // | DIR_LRTBREV); // | flip);
	ss_sprite[3].drawMode = ( COLOR_0 | ECD_DISABLE | COMPO_REP); //4bpp
	ss_sprite[3].charSize    = 0x20C0;  // 256x*192y

    SS_SET_N0PRIN(0);
    SS_SET_N1PRIN(6);
    SS_SET_S0PRIN(5);

	initLayers();
	initColors();

	Set8PCM();

	nBurnFunction = update_input1;
	*(unsigned int*)OPEN_CSH_VAR(nSoundBufferPos) = 0;

	drawWindow(0,192,192,0,64);
	SetVblank2();
	wait_vblank();
}
//-------------------------------------------------------------------------------------------------------------------------------------
static PcmHn createHandle(PcmCreatePara *para)
{
	PcmHn pcm;

	pcm = PCM_CreateMemHandle(para);
	if (pcm == NULL) {
		return NULL;
	}
	return pcm;
}
//-------------------------------------------------------------------------------------------------------------------------------------
static void Set8PCM()
{
	PcmCreatePara	para[8];
	PcmInfo 		info[8];
	PcmStatus	*st;
	static PcmWork g_movie_work[8];

	for (int i=0; i<8; i++)
	{
		PCM_PARA_WORK(&para[i]) = (struct PcmWork *)&g_movie_work[i];
		PCM_PARA_RING_ADDR(&para[i]) = (Sint8 *)PCM_ADDR+0x40000+(0x4000*(i+1));
		PCM_PARA_RING_SIZE(&para[i]) = RING_BUF_SIZE;
		PCM_PARA_PCM_ADDR(&para[i]) = PCM_ADDR+(0x4000*(i+1));
		PCM_PARA_PCM_SIZE(&para[i]) = PCM_SIZE;

		memset((Sint8 *)SOUND_BUFFER,0,SOUNDRATE*16);
		st = &g_movie_work[i].status;
		st->need_ci = PCM_ON;
	 
		PCM_INFO_FILE_TYPE(&info[i]) = PCM_FILE_TYPE_NO_HEADER;			
		PCM_INFO_DATA_TYPE(&info[i])=PCM_DATA_TYPE_RLRLRL;//PCM_DATA_TYPE_LRLRLR;
		PCM_INFO_FILE_SIZE(&info[i]) = RING_BUF_SIZE;//SOUNDRATE*2;//0x4000;//214896;
		PCM_INFO_CHANNEL(&info[i]) = 0x01;
		PCM_INFO_SAMPLING_BIT(&info[i]) = 16;

		PCM_INFO_SAMPLING_RATE(&info[i])	= SOUNDRATE;//30720L;//44100L;
		PCM_INFO_SAMPLE_FILE(&info[i]) = RING_BUF_SIZE;//SOUNDRATE*2;//30720L;//214896;
		pcm8[i] = createHandle(&para[i]);

		PCM_SetPcmStreamNo(pcm8[i], i);

		PCM_SetInfo(pcm8[i], &info[i]);
		PCM_ChangePcmPara(pcm8[i]);

		PCM_MeSetLoop(pcm8[i], 0);//SOUNDRATE*120);
		PCM_Start(pcm8[i]);
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
