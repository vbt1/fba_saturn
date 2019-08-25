// Z80 (Zed Eight-Ty) Interface
#include "burnint.h"
#include "globals.h"
#include "czet.h"
//#define ACB_DRIVER_DATA	 (64)

static cz80_struc* CZetCPUContext = NULL;
//static 
//cz80_struc CZetCPUContext[2];
static cz80_struc* lastCZetCPUContext = NULL;

/*static*/ int nOpenedCPU = -1;
/*static*/ int nCPUCount = 0;

unsigned char __fastcall CZetDummyReadHandler(unsigned short a) { return 0; }
void __fastcall CZetDummyWriteHandler(unsigned short a, unsigned char b) { }
unsigned char __fastcall CZetDummyInHandler(unsigned short a) { return 0; }
void __fastcall CZetDummyOutHandler(unsigned short a, unsigned char b) { }
/*
void CZetSetEDFECallback(void (*pCallback)(Z80_Regs*))
{
	// Can be set before init. it's cleared at exit.
	z80edfe_callback = pCallback;
}
*/
void CZetSetSP(unsigned short data)
{
	lastCZetCPUContext->SP.W = data;
}

void CZetSetReadHandler(unsigned char (__fastcall *pHandler)(unsigned short))
{
	lastCZetCPUContext->Read_Byte = pHandler;
//	lastCZetCPUContext->Read_Word = pHandler;
}

void CZetSetWriteHandler(void (__fastcall *pHandler)(unsigned short, unsigned char))
{
	lastCZetCPUContext->Write_Byte = pHandler;
//	lastCZetCPUContext->Write_Word = pHandler;
}

void CZetSetInHandler(unsigned char (__fastcall *pHandler)(unsigned short))
{
	lastCZetCPUContext->IN_Port = pHandler;
}

void CZetSetOutHandler(void (__fastcall *pHandler)(unsigned short, unsigned char))
{
	lastCZetCPUContext->OUT_Port = pHandler;
}

void CZetNewFrame()
{
	unsigned int i;
	for (i = 0; i < nCPUCount; i++) {
		CZetCPUContext[i].nCyclesTotal = 0;
	}
}

int CZetInit2(int nCount,UINT8 *addr)
{
	CZetCPUContext = (cz80_struc*)addr;
	Cz80_InitFlags();

	unsigned int i;

	for (i = 0; i < nCount; i++) {
		Cz80_Init( &CZetCPUContext[i] );
		CZetCPUContext[i].nInterruptLatch = -1;

		CZetCPUContext[i].Read_Byte = CZetDummyReadHandler;
		CZetCPUContext[i].Write_Byte = CZetDummyWriteHandler;
		CZetCPUContext[i].Read_Word = CZetDummyReadHandler;
		CZetCPUContext[i].Write_Word = CZetDummyWriteHandler;
		CZetCPUContext[i].IN_Port = CZetDummyInHandler;
		CZetCPUContext[i].OUT_Port = CZetDummyOutHandler;
	}
	CZetOpen(0);
	nCPUCount = nCount;
	return 0;
}

unsigned char CZetReadByte(unsigned short address)
{
	if (nOpenedCPU < 0) return 0;

	return 0;
}

void CZetWriteByte(unsigned short address, unsigned char data)
{
	if (nOpenedCPU < 0) return;
}

void CZetClose()
{
	nOpenedCPU = -1;
}

int CZetOpen(int nCPU)
{
	lastCZetCPUContext = &CZetCPUContext[nCPU];
	nOpenedCPU = nCPU;

	return 0;
}

int CZetGetActive()
{
	return nOpenedCPU;
}
#if 0
#define INT_DIGITS 19
static char *itoa(i)
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
#endif

int CZetRun(int nCycles)
{
	if (nCycles <= 0) {
		return 0;
	}

	lastCZetCPUContext->nCyclesTotal += nCycles;
	lastCZetCPUContext->nCyclesSegment = nCycles;
	lastCZetCPUContext->nCyclesLeft = nCycles;

	nCycles = Cz80_Exec(lastCZetCPUContext);
	//nCycles = Doze.nCyclesSegment - Doze.nCyclesLeft;

	lastCZetCPUContext->nCyclesTotal -= lastCZetCPUContext->nCyclesLeft;
	lastCZetCPUContext->nCyclesLeft = 0;
	lastCZetCPUContext->nCyclesSegment = 0;

//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"                 ",4,50);
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(nCycles),40,50);

	return nCycles;
}

int CZetTotalCyclesSlave()
{
	/*
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"                 ",4,30);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(*(unsigned int*)OPEN_CSH_VAR(SS_Z80CY)),40,30);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"                 ",4,50);
FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(lastCZetCPUContext->nCyclesTotal - lastCZetCPUContext->nCyclesLeft),40,50);
*/
	if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
	{
		SPR_WaitEndSlaveSH();
	}	
//	return (*(int*)OPEN_CSH_VAR(SS_Z80CY))- lastCZetCPUContext->nCyclesLeft; //+lastCZetCPUContext->nCyclesSegment;
	return (*(int*)OPEN_CSH_VAR(SS_Z80CY))+1492; //- lastCZetCPUContext->nCyclesLeft; //+lastCZetCPUContext->nCyclesSegment;
}
void CZetRunEndSlave()
{
	SPR_RunSlaveSH((PARA_RTN*)CZetRunEnd,NULL);

	if((*(volatile Uint8 *)0xfffffe11 & 0x80) != 0x80)
	{
		SPR_WaitEndSlaveSH();
	}
}

void CZetRunSlave(int *nCycles)
{
	if (nCycles[0] <= 0) {
		return;
	}
//unsigned int nCyc = *(unsigned int*)OPEN_CSH_VAR(SS_Z80CY);

	lastCZetCPUContext->nCyclesTotal += *nCycles;
	lastCZetCPUContext->nCyclesSegment = *nCycles;
	lastCZetCPUContext->nCyclesLeft = *nCycles;

	unsigned int nCyc = Cz80_Exec(lastCZetCPUContext);

	lastCZetCPUContext->nCyclesTotal -= lastCZetCPUContext->nCyclesLeft;
	lastCZetCPUContext->nCyclesLeft = 0;
	lastCZetCPUContext->nCyclesSegment = 0;

//	*(unsigned int*)0x00200000 = nCyc;
	*(unsigned int*)OPEN_CSH_VAR(SS_Z80CY) = nCyc;
//	*(volatile Uint8 *)0xfffffe11=0x80;
}

void CZetRunAdjust(int nCycles)
{
	if (nCycles < 0 && lastCZetCPUContext->nCyclesLeft < -nCycles) {
		nCycles = 0;
	}

	lastCZetCPUContext->nCyclesTotal += nCycles;
	lastCZetCPUContext->nCyclesSegment += nCycles;
	lastCZetCPUContext->nCyclesLeft += nCycles;
}

void CZetRunEnd()
{
	lastCZetCPUContext->nCyclesTotal -= lastCZetCPUContext->nCyclesLeft;
	lastCZetCPUContext->nCyclesSegment -= lastCZetCPUContext->nCyclesLeft;
//	lastCZetCPUContext->nCyclesTotal += lastCZetCPUContext->nCyclesLeft;
//	lastCZetCPUContext->nCyclesSegment -= lastCZetCPUContext->nCyclesLeft;

	lastCZetCPUContext->nCyclesLeft = 0;
}

// This function will make an area callback ZetRead/ZetWrite
int CZetMemCallback(int nStart, int nEnd, int nMode)
{
	nStart >>= CZ80_FETCH_SFT;
	nEnd += CZ80_FETCH_BANK - 1;
	nEnd >>= CZ80_FETCH_SFT;

	// Leave the section out of the memory map, so the callback with be used
	unsigned int i;

	for (i = nStart; i < nEnd; i++) {
		switch (nMode) {
			case 0:
				lastCZetCPUContext->Read[i] = NULL;
				break;
			case 1:
				lastCZetCPUContext->Write[i] = NULL;
				break;
			case 2:
				lastCZetCPUContext->Fetch[i] = NULL;
				break;
		}
	}
	return 0;
}

int CZetMemEnd()
{
	return 0;
}

void CZetExit2()
{
	unsigned int i;

	for (i = 0; i < nCPUCount; i++) {
		CZetOpen(i);
		CZetRunEnd();
		CZetClose();
//		Cz80_Init( &CZetCPUContext[i] );
		memset(&CZetCPUContext[i], 0, sizeof(cz80_struc));
		CZetCPUContext[i].nInterruptLatch = -1;

		CZetCPUContext[i].Read_Byte = NULL;
		CZetCPUContext[i].Write_Byte = NULL;
		CZetCPUContext[i].Read_Word = NULL;
		CZetCPUContext[i].Write_Word = NULL;
		CZetCPUContext[i].IN_Port = NULL;
		CZetCPUContext[i].OUT_Port = NULL;
	}	
	CZetCPUContext = NULL;

	lastCZetCPUContext = NULL;
	nOpenedCPU = -1;
	nCPUCount = 0;
}

void CZetMapMemory(unsigned char *Mem, int nStart, int nEnd, int nFlags)
{
//	UINT8 cStart = (nStart >> 8);
	int s = nStart >> CZ80_FETCH_SFT;
//	UINT8 **pMemMap = ZetCPUContext[nOpenedCPU]->pZetMemMap;
	int e = (nEnd + CZ80_FETCH_BANK - 1) >> CZ80_FETCH_SFT;

	for (unsigned int i = s; i < e; i++) 
	{
		if (nFlags & (1 << 0)) lastCZetCPUContext->Read[i] = Mem - nStart; // READ
		if (nFlags & (1 << 1)) lastCZetCPUContext->Write[i] = Mem - nStart; // WRITE
		if (nFlags & (1 << 2)) 	lastCZetCPUContext->Fetch[i] = Mem - nStart; // OP
		if (nFlags & (1 << 3)) 	lastCZetCPUContext->FetchData[i] = Mem - nStart; // ARG			
	}
}

int CZetMapArea(int nStart, int nEnd, int nMode, unsigned char *Mem)
{
	int s = nStart >> CZ80_FETCH_SFT;
	int e = (nEnd + CZ80_FETCH_BANK - 1) >> CZ80_FETCH_SFT;

	// Put this section in the memory map, giving the offset from Z80 memory to PC memory
	for (unsigned int i = s; i < e; i++) {
		switch (nMode) {
			case 0:  // read
				lastCZetCPUContext->Read[i] = Mem - nStart;
				break;
			case 1: // write
				lastCZetCPUContext->Write[i] = Mem - nStart;
				break;
			case 2: // fetch
				lastCZetCPUContext->Fetch[i] = Mem - nStart;
				lastCZetCPUContext->FetchData[i] = Mem - nStart;
				break;
		}
	}
	return 0;
}

int CZetMapArea2(int nStart, int nEnd, int nMode, unsigned char *Mem01, unsigned char *Mem02)
{
	int s = nStart >> CZ80_FETCH_SFT;
	int e = (nEnd + CZ80_FETCH_BANK - 1) >> CZ80_FETCH_SFT;

	if (nMode != 2) {
		return 1;
	}

	// Put this section in the memory map, giving the offset from Z80 memory to PC memory
	unsigned int i;

	for (i = s; i < e; i++) {
		lastCZetCPUContext->Fetch[i] = Mem01 - nStart;
		lastCZetCPUContext->FetchData[i] = Mem02 - nStart;
	}
	return 0;
}

int CZetReset()
{
	Cz80_Reset( lastCZetCPUContext );

	return 0;
}

int CZetGetPC(int n)
{
	if (n < 0) {
		return Cz80_Get_PC(lastCZetCPUContext);
	} else {
		return Cz80_Get_PC(&CZetCPUContext[n]);
	}
}
int CZetDe(int n)
{
	if (n < 0) {
		return Cz80_Get_DE(lastCZetCPUContext);
	} else {
		return Cz80_Get_DE(&CZetCPUContext[n]);
	}
}

int CZetBc(int n)
{
	if (n < 0) {
		return Cz80_Get_BC(lastCZetCPUContext);
	} else {
		return Cz80_Get_BC(&CZetCPUContext[n]);
	}
}

int CZetHL(int n)
{
	if (n < 0) {
		return Cz80_Get_HL(lastCZetCPUContext);
	} else {
		return Cz80_Get_HL(&CZetCPUContext[n]);
	}
}

void CZetSetIRQLine(const int line, const int status)
{
	switch (status)
	{
/*	case CZET_IRQSTATUS_HOLD:
		lastCZetCPUContext->nInterruptLatch = line | 1;
		CZetRun(100);
		lastCZetCPUContext->nInterruptLatch = 0 | 0;
//		CZetRun(0);
		return;*/
	default:
		lastCZetCPUContext->nInterruptLatch = line | status;
		return;
	}
}

int CZetNmi()
{
	int nCycles = Cz80_Set_NMI(lastCZetCPUContext);
	lastCZetCPUContext->nCyclesTotal += nCycles;

	return nCycles;
}

int CZetIdle(int nCycles)
{
	lastCZetCPUContext->nCyclesTotal += nCycles;

	return nCycles;
}

int CZetSegmentCycles()
{
	return lastCZetCPUContext->nCyclesSegment - lastCZetCPUContext->nCyclesLeft;
}

int CZetTotalCycles()
{
//		int vbt = lastCZetCPUContext->nCyclesTotal - lastCZetCPUContext->nCyclesLeft;

//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"                 ",4,30);
//FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)itoa(vbt),40,30);
	return lastCZetCPUContext->nCyclesTotal - lastCZetCPUContext->nCyclesLeft;
}
