#include "../burnint.h"
#include "m6502_intf.h"

#define MAX_CPU		1

INT32 nM6502Count = 0;
static INT32 nActiveCPU = 0;

//static M6502Ext *m6502CPUContext[MAX_CPU] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static M6502Ext m6502CPUContext;
static M6502Ext *pCurrentCPU;
static INT32 nM6502CyclesDone[MAX_CPU];
INT32 nM6502CyclesTotal;

static UINT8 M6502ReadPortDummyHandler(UINT16 a)
{
	return 0;
}

static void M6502WritePortDummyHandler(UINT16 a, UINT8 d)
{
}

static UINT8 M6502ReadByteDummyHandler(UINT16 a)
{
	return 0;
}

static void M6502WriteByteDummyHandler(UINT16 a, UINT8 d)
{
}

static UINT8 M6502ReadMemIndexDummyHandler(UINT16 a)
{
	return 0;
}

static void M6502WriteMemIndexDummyHandler(UINT16 a, UINT8 d)
{
}

static UINT8 M6502ReadOpDummyHandler(UINT16 a)
{
	return 0;
}

static UINT8 M6502ReadOpArgDummyHandler(UINT16 a)
{
	return 0;
}

void M6502Reset()
{
#if defined FBA_DEBUG
	if (!DebugCPU_M6502Initted) bprintf(PRINT_ERROR, _T("M6502Reset called without init\n"));
	if (nActiveCPU == -1) bprintf(PRINT_ERROR, _T("M6502Reset called with no CPU open\n"));
#endif

	pCurrentCPU->reset();
}

void M6502NewFrame()
{
#if defined FBA_DEBUG
	if (!DebugCPU_M6502Initted) bprintf(PRINT_ERROR, _T("M6502NewFrame called without init\n"));
#endif
    INT32 i;

	for (i = 0; i < nM6502Count; i++) {
		nM6502CyclesDone[i] = 0;
	}
	nM6502CyclesTotal = 0;
}
/*
static UINT8 M6502CheatRead(UINT32 a)
{
	return M6502ReadByte(a);
}	   */
/*
static cpu_core_config M6502CheatCpuConfig =
{
	M6502Open,
	M6502Close,
	M6502CheatRead,
	M6502WriteRom,
	M6502GetActive,
	M6502TotalCycles,
	M6502NewFrame,
	M6502Run,
	M6502RunEnd,
	M6502Reset,
	1<<16,
	0
};
*/
INT32 M6502Init(INT32 cpu, INT32 type)
{
//	DebugCPU_M6502Initted = 1;
	
	nM6502Count++;
	nActiveCPU = -1;

//	m6502CPUContext[cpu] = (M6502Ext*)BurnMalloc(sizeof(M6502Ext));

	pCurrentCPU = &m6502CPUContext;

	memset(pCurrentCPU, 0, sizeof(M6502Ext));

	switch (type)
	{
		case TYPE_M6502:
		case TYPE_M6504:
			pCurrentCPU->execute = M6502_Execute;
			pCurrentCPU->reset = M6502_Reset;
			pCurrentCPU->init = M6502_Init;
			pCurrentCPU->set_irq_line = NULL;
		break;
/*
		case TYPE_M65C02:
			pCurrentCPU->execute = m65c02_execute;
			pCurrentCPU->reset = m65c02_reset;
			pCurrentCPU->init = m65c02_init;
			pCurrentCPU->set_irq_line = m65c02_set_irq_line;
		break;

		case TYPE_DECO16:
			pCurrentCPU->execute = deco16_execute;
			pCurrentCPU->reset = deco16_reset;
			pCurrentCPU->init = deco16_init;
			pCurrentCPU->set_irq_line = deco16_set_irq_line;
		break;
*/
/*
		case TYPE_N2A03:
			pCurrentCPU->execute = M6502_Execute;
			pCurrentCPU->reset = M6502_Reset;
			pCurrentCPU->init = n2a03_init;		// different
			pCurrentCPU->set_irq_line = m6502_set_irq_line;
		break;
		*/
/*
		case TYPE_M65SC02:
			pCurrentCPU->execute = m65c02_execute;
			pCurrentCPU->reset = m65c02_reset;
			pCurrentCPU->init = m65sc02_init;	// different
			pCurrentCPU->set_irq_line = m65c02_set_irq_line;
		break;

		case TYPE_M6510:
		case TYPE_M6510T:
		case TYPE_M7501:
		case TYPE_M8502:
			pCurrentCPU->execute = m6502_execute;
			pCurrentCPU->reset = m6510_reset;	// different
			pCurrentCPU->init = m6510_init;		// different
			pCurrentCPU->set_irq_line = m6502_set_irq_line;
		break;
*/		
	}

	pCurrentCPU->ReadPort = M6502ReadPortDummyHandler;
	pCurrentCPU->WritePort = M6502WritePortDummyHandler;
	pCurrentCPU->ReadByte = M6502ReadByteDummyHandler;
	pCurrentCPU->WriteByte = M6502WriteByteDummyHandler;
	pCurrentCPU->ReadMemIndex = M6502ReadMemIndexDummyHandler;
	pCurrentCPU->WriteMemIndex = M6502WriteMemIndexDummyHandler;
	pCurrentCPU->ReadOp = M6502ReadOpDummyHandler;
	pCurrentCPU->ReadOpArg = M6502ReadOpArgDummyHandler;
	
	nM6502CyclesDone[cpu] = 0;
	INT32 j;
	for (j = 0; j < (0x0100); j++) {
		Read[j] = NULL;
		Write[j] = NULL;		
		Fetch[j] = NULL;		
	}
	
	nM6502CyclesTotal = 0;
	
	pCurrentCPU->init();

//	CpuCheatRegister(cpu, &M6502CheatCpuConfig);

	return 0;
}

void M6502Exit()
{
#if defined FBA_DEBUG
	if (!DebugCPU_M6502Initted) bprintf(PRINT_ERROR, _T("M6502Exit called without init\n"));
#endif
	INT32 i;
/*
	for (i = 0; i < MAX_CPU; i++) {
		if (m6502CPUContext[i]) {
			free(m6502CPUContext[i]);
		}
	}
*/
	nM6502Count = 0;
	
//	DebugCPU_M6502Initted = 0;
}

void M6502Open(INT32 num)
{
#if defined FBA_DEBUG
	if (!DebugCPU_M6502Initted) bprintf(PRINT_ERROR, _T("M6502Open called without init\n"));
	if (num >= nM6502Count) bprintf(PRINT_ERROR, _T("M6502Open called with invalid index %x\n"), num);
	if (nActiveCPU != -1) bprintf(PRINT_ERROR, _T("M6502Open called with CPU already open with index %x\n"), num);
#endif

	nActiveCPU = num;

	pCurrentCPU = &m6502CPUContext;

	M6502_SetRegs(&pCurrentCPU->reg);
	
	nM6502CyclesTotal = nM6502CyclesDone[nActiveCPU];
}

void M6502Close()
{
#if defined FBA_DEBUG
	if (!DebugCPU_M6502Initted) bprintf(PRINT_ERROR, _T("M6502Close called without init\n"));
	if (nActiveCPU == -1) bprintf(PRINT_ERROR, _T("M6502Close called with no CPU open\n"));
#endif

	M6502_GetRegs(&pCurrentCPU->reg);
	
	nM6502CyclesDone[nActiveCPU] = nM6502CyclesTotal;

	pCurrentCPU = NULL; // cause problems! 
	
	nActiveCPU = -1;
}

INT32 M6502GetActive()
{
#if defined FBA_DEBUG
	if (!DebugCPU_M6502Initted) bprintf(PRINT_ERROR, _T("M6502GetActive called without init\n"));
	if (nActiveCPU == -1) bprintf(PRINT_ERROR, _T("M6502GetActive called with no CPU open\n"));
#endif

	return nActiveCPU;
}

void M6502SetIRQLine(INT32 vector, INT32 status)
{
	if (status == M6502_IRQSTATUS_NONE) {
//		pCurrentCPU->set_irq_line(vector, 0);
		M6502_Clear_Pending_Interrupts();
	}
	if (status == M6502_IRQSTATUS_ACK) {
//		pCurrentCPU->set_irq_line(vector, 1);
		M6502_Cause_Interrupt(vector);
	}
/*	
	if (status == M6502_IRQSTATUS_AUTO) {
		pCurrentCPU->set_irq_line(vector, 1);
		pCurrentCPU->execute(0);
		pCurrentCPU->set_irq_line(vector, 0);
		pCurrentCPU->execute(0);
	}
*/	
}

INT32 M6502Run(INT32 cycles)
{
	cycles = M6502_Execute(cycles);
	nM6502CyclesTotal += cycles;
	
	return cycles;
}

void M6502RunEnd()
{
#if defined FBA_DEBUG
	if (!DebugCPU_M6502Initted) bprintf(PRINT_ERROR, _T("M6502RunEnd called without init\n"));
	if (nActiveCPU == -1) bprintf(PRINT_ERROR, _T("M6502RunEnd called with no CPU open\n"));
#endif
}

INT32 M6502MapMemory(UINT8* pMemory, UINT16 nStart, UINT16 nEnd, INT32 nType)
{
#if defined FBA_DEBUG
	if (!DebugCPU_M6502Initted) bprintf(PRINT_ERROR, _T("M6502MapMemory called without init\n"));
	if (nActiveCPU == -1) bprintf(PRINT_ERROR, _T("M6502MapMemory called with no CPU open\n"));
#endif

	UINT8 cStart = (nStart >> 8);
//	UINT8 **pMemMap = pCurrentCPU->pMemMap;
	UINT16 i;

	for (i = cStart; i <= (nEnd >> 8); i++) {
		if (nType & M6502_READ)	{
			Read[i] = pMemory + ((i - cStart) << 8);
		}
		if (nType & M6502_WRITE) {
			Write[i] = pMemory + ((i - cStart) << 8);
		}
		if (nType & M6502_FETCH) {
			Fetch[i] = pMemory + ((i - cStart) << 8);
		}
	}
	return 0;

}

void M6502SetReadPortHandler(UINT8 (*pHandler)(UINT16))
{
	pCurrentCPU->ReadPort = pHandler;
}

void M6502SetWritePortHandler(void (*pHandler)(UINT16, UINT8))
{
	pCurrentCPU->WritePort = pHandler;
}

void M6502SetReadHandler(UINT8 (*pHandler)(UINT16))
{
	pCurrentCPU->ReadByte = pHandler;
}

void M6502SetWriteHandler(void (*pHandler)(UINT16, UINT8))
{
	pCurrentCPU->WriteByte = pHandler;
}

void M6502SetReadMemIndexHandler(UINT8 (*pHandler)(UINT16))
{
	pCurrentCPU->ReadMemIndex = pHandler;
}

void M6502SetWriteMemIndexHandler(void (*pHandler)(UINT16, UINT8))
{
	pCurrentCPU->WriteMemIndex = pHandler;
}

void M6502SetReadOpHandler(UINT8 (*pHandler)(UINT16))
{
	pCurrentCPU->ReadOp = pHandler;
}

void M6502SetReadOpArgHandler(UINT8 (*pHandler)(UINT16))
{
	pCurrentCPU->ReadOpArg = pHandler;
}

UINT8 M6502ReadPort(UINT16 Address)
{
	// check handler
	if (pCurrentCPU->ReadPort != NULL) {
		return pCurrentCPU->ReadPort(Address);
	}
	
	return 0;
}

void M6502WritePort(UINT16 Address, UINT8 Data)
{
	// check handler
	if (pCurrentCPU->WritePort != NULL) {
		pCurrentCPU->WritePort(Address, Data);
	}
}

UINT8 M6502ReadByte(UINT16 Address)
{
	// check mem map
	UINT8 * pr = Read[(Address >> 8)];
	return (pr != NULL) ? pr[Address&0xff]:atetris_read(Address);
}

UINT16 M6502ReadWord(UINT16 Address)
{
	UINT8 * pr = Read[(Address >> 8)];
	return (pr != NULL) ? pr[Address&0xff]|pr[(Address&0xff)+1]<<8:atetris_read(Address)|atetris_read(Address+1)<<8;
}

void M6502WriteByte(UINT16 Address, UINT8 Data)
{
	// check mem map
	UINT8 * pr = Write[(Address >> 8)];
	if (pr != NULL) {
		pr[Address & 0xff] = Data;
		return;
	}
	atetris_write(Address, Data);
}

void M6502WriteWord(UINT16 Address, UINT8 Data, UINT8 Data2)
{
	// check mem map
	UINT8 * pr = Write[(Address >> 8)];
//	if (pr != NULL) 
	{
		pr[(Address&0xff)] = Data;		
		pr[(Address&0xff)-1] = Data2;
		return;
	}
	/*
	// check handler
	if (pCurrentCPU->WriteByte != NULL) {
		pCurrentCPU->WriteByte(Address, Data);
		pCurrentCPU->WriteByte(Address-1, Data2);		
		return;
	}
	*/
}

UINT8 M6502ReadMemIndex(UINT16 Address)
{
	// check mem map
	UINT8 * pr = Read[(Address >> 8)];
	return (pr != NULL) ? pr[Address&0xff]:pCurrentCPU->ReadMemIndex(Address);	
}

void M6502WriteMemIndex(UINT16 Address, UINT8 Data)
{
	// check mem map
	UINT8 * pr = Write[(Address >> 8)];
	if (pr != NULL) {
		pr[Address & 0xff] = Data;
		return;
	}

	// check handler
	if (pCurrentCPU->WriteMemIndex != NULL) {
		pCurrentCPU->WriteMemIndex(Address, Data);
		return;
	}
}

UINT8 M6502ReadOp(UINT16 Address)
{
	// check mem map
	UINT8 * pr = Fetch[(Address >> 8)];
	return (pr != NULL) ? pr[Address&0xff]:pCurrentCPU->ReadOp(Address);
}

UINT8 M6502ReadOpArg(UINT16 Address)
{
	// check mem map
	UINT8 * pr = Read[(Address >> 8)];
	return (pr != NULL) ? pr[Address&0xff]:atetris_read(Address);
}

void M6502WriteRom(UINT32 Address, UINT8 Data)
{
#if defined FBA_DEBUG
	if (!DebugCPU_M6502Initted) bprintf(PRINT_ERROR, _T("M6502WriteRom called without init\n"));
	if (nActiveCPU == -1) bprintf(PRINT_ERROR, _T("M6502WriteRom called with no CPU open\n"));
#endif

	Address &= 0xffff;

	UINT8 * pr = Read[(Address >> 8)];
	UINT8 * pw = Write[(Address >> 8)];
	UINT8 * pf = Fetch[(Address >> 8)];

	if (pr != NULL) {
		pr[Address & 0xff] = Data;
	}
	
	if (pw != NULL) {
		pw[Address & 0xff] = Data;
	}

	if (pf != NULL) {
		pf[Address & 0xff] = Data;
	}

	// check handler
	if (pCurrentCPU->WriteByte != NULL) {
		pCurrentCPU->WriteByte(Address, Data);
		return;
	}
}
/*
UINT32 M6502GetPC()
{
#if defined FBA_DEBUG
	if (!DebugCPU_M6502Initted) bprintf(PRINT_ERROR, _T("M6502GetPC called without init\n"));
	if (nActiveCPU == -1) bprintf(PRINT_ERROR, _T("M6502GetPC called with no CPU open\n"));
#endif

	return M6502_GetPC();
}
*/
