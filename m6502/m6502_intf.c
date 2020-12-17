//#pragma GCC optimize ("O2")
#include "../burnint.h"
#include "m6502_intf.h"

#define MAX_CPU		1

INT32 M6502Init(INT32 cpu, INT32 type)
{
	for (INT32 j = 0; j < (0x0100); j++) {
		Read[j] = NULL;
		Write[j] = NULL;		
		Fetch[j] = NULL;		
	}
	
	M6502_Init();
	return 0;
}

void M6502SetIRQLine(UINT32 vector, UINT32 status)
{
	if (status == M6502_IRQSTATUS_NONE) {
//		pCurrentCPU->set_irq_line(vector, 0);
		M6502_Clear_Pending_Interrupts();
	}
	if (status == M6502_IRQSTATUS_ACK) {
//		pCurrentCPU->set_irq_line(vector, 1);
		M6502_Cause_Interrupt(vector);
	}
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

void M6502SetReadHandler(UINT16 nStart, UINT16 nEnd,UINT8 (*pHandler)(UINT16))
{
	UINT8 cStart = (nStart >> 8);
	
	for (UINT32 i = cStart; i <= (nEnd >> 8); i++) 
	{	
		rf[i] = pHandler;
	}	
}

void M6502SetWriteHandler(UINT16 nStart, UINT16 nEnd,void (*pHandler)(UINT16, UINT8))
{
	UINT8 cStart = (nStart >> 8);
	
	for (UINT32 i = cStart; i <= (nEnd >> 8); i++) 
	{	
		wf[i] = pHandler;
	}
}

/*
void M6502SetReadPortHandler(UINT8 (*pHandler)(UINT16))
{
	pCurrentCPU->ReadPort = pHandler;
}

void M6502SetWritePortHandler(void (*pHandler)(UINT16, UINT8))
{
	pCurrentCPU->WritePort = pHandler;
}
*/
/*
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
*/

/*
UINT8 M6502ReadByte(UINT16 Address)
{
	// check mem map
	UINT8 * pr = Read[(Address >> 8)];
	return (pr != NULL) ? pr[Address&0xff]:rf[Address>>4](Address);
}

UINT16 M6502ReadWord(UINT16 Address)
{
	UINT8 * pr = Read[(Address >> 8)];
	return (pr != NULL) ? pr[Address&0xff]|pr[(Address&0xff)+1]<<8:rf[Address>>4](Address)|rf[Address>>4](Address+1)<<8;
}

void M6502WriteByte(UINT16 Address, UINT8 Data)
{
	// check mem map
	UINT8 * pr = Write[(Address >> 8)];
	if (pr != NULL) {
		pr[Address & 0xff] = Data;
		return;
	}
	wf[Address>>4](Address, Data);
}
*/
/*
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
}
*/
/*
UINT8 M6502ReadMemIndex(UINT16 Address)
{
	// check mem map
	UINT8 * pr = Read[(Address >> 8)];
	return (pr != NULL) ? pr[Address&0xff]:pCurrentCPU->ReadMemIndex(Address);	
}
*/
/*
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
*/
/*
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
*/
/*
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
*/
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
