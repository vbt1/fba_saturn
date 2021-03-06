#include "burnint.h"
#include "m6809_intf.h"

//#define MAX_CPU		1

unsigned char *Read[0x100];
unsigned char *Write[0x100];
unsigned char *Fetch[0x100];
//unsigned short *FetchW[0x100];
//unsigned short *ReadW[0x100];

pReadByteHandler ReadByte;
pWriteByteHandler WriteByte;
pReadOpHandler ReadOp;
pReadOpArgHandler ReadOpArg;
extern m6809_Regs m6809;

static unsigned char M6809ReadByteDummyHandler(unsigned short d)
{
	return 0;
}

static void M6809WriteByteDummyHandler(unsigned short d, unsigned char value)
{
}

static unsigned char M6809ReadOpDummyHandler(unsigned short d)
{
	return 0;
}

static unsigned char M6809ReadOpArgDummyHandler(unsigned short d)
{
	return 0;
}

void M6809Reset()
{
	m6809_reset();
}

int M6809Init(int num)
{
	int j;

//	memset(m6809CPUContext, 0, num * sizeof(M6809Ext));
	ReadByte = M6809ReadByteDummyHandler;
	WriteByte = M6809WriteByteDummyHandler;	
	ReadOp = M6809ReadOpDummyHandler;
	ReadOpArg = M6809ReadOpArgDummyHandler;

	for (unsigned int j = 0; j < (0x0100); j++) {
		Read[j] = NULL;
		Write[j] = NULL;
		Fetch[j] = NULL;
//		ReadW[j] = NULL;
//		FetchW[j] = NULL;		
	}	
	
	m6809_init(NULL);
	return 0;
}

void M6809Exit()
{
//	nM6809Count = 0;

//	free(m6809CPUContext);
//	m6809CPUContext = NULL;
}

void M6809Open(int num)
{
	m6809_set_context(&m6809);
	
//	nM6809CyclesTotal = nM6809CyclesDone[nActiveCPU];
}

void M6809Close()
{
	m6809_get_context(&m6809);
}

void M6809SetIRQ(int vector, int status)
{
	if (status == M6809_IRQSTATUS_NONE) {
		m6809_set_irq_line(vector, 0);
	}
	
	if (status == M6809_IRQSTATUS_ACK) {
		m6809_set_irq_line(vector, 1);
	}
	
	if (status == M6809_IRQSTATUS_AUTO) {
		m6809_set_irq_line(vector, 1);
		m6809_execute(0);
		m6809_set_irq_line(vector, 0);
		m6809_execute(0);
	}
}

void M6809RunEnd()
{

}

int M6809MapMemory(unsigned char* pMemory, unsigned short nStart, unsigned short nEnd, int nType)
{
	unsigned short i;
	unsigned char cStart = (nStart >> 8);
//	unsigned char **pMemMap = m6809CPUContext[nActiveCPU].pMemMap;

	for (i = cStart; i <= (nEnd >> 8); ++i) 
	{
		if (nType & M6809_READ)	{
			/*ReadW[i] =*/ Read[i] = pMemory; // + ((i - cStart) << 8);
		}
		if (nType & M6809_WRITE) {
			Write[i] = pMemory; // + ((i - cStart) << 8);
		}
		if (nType & M6809_FETCH) {
			/*FetchW[i] =*/ Fetch[i] = pMemory; // + ((i - cStart) << 8);
		}
		pMemory+=256;
	}
	return 0;
}

void M6809MapMemory2(unsigned char* pMemory, unsigned short nStart, unsigned short nEnd) //, int nType)
{
	unsigned short i;
	nStart >>= 8;
	nEnd >>= 8;
	for (i = nStart; i <= nEnd; ++i) 
	{
		/*FetchW[i] = ReadW[i] =*/ Fetch[i] = Read[i] = pMemory; // + ((i - nStart) << 8);
		pMemory+=256;
	}
}

void M6809SetReadByteHandler(unsigned char (*pHandler)(unsigned short))
{
//	m6809CPUContext[nActiveCPU].ReadByte = pHandler;
	ReadByte = 	pHandler;
}

void M6809SetReadOpHandler(unsigned char (*pHandler)(unsigned short))
{
//	m6809CPUContext[nActiveCPU].ReadOp = pHandler;
	ReadOp = pHandler;
}

void M6809SetReadOpArgHandler(unsigned char (*pHandler)(unsigned short))
{
//	m6809CPUContext[nActiveCPU].ReadOpArg = pHandler;
	ReadOpArg = pHandler;
}

#if 0
void M6809WriteByte(unsigned short Address, unsigned char Data)
{
	// check mem map
	unsigned char * pr = Write[(Address >> 8)];
	if (pr != NULL) {
		pr[Address & 0xff] = Data;
		return;
	}
	DrvGngM6809WriteByte(Address, Data);
}


unsigned char M6809ReadOp(unsigned short Address)
{
	unsigned char * pr = Fetch[(Address >> 8)];

	if (pr != NULL) 
	{
		return pr[Address & 0xff];
	}
	return ReadOp(Address);
}

unsigned char M6809ReadOpArg(unsigned short Address)
{
	// check mem map
	unsigned char * pr = Read[(Address >> 8)];
	if (pr != NULL) {
		return pr[Address & 0xff];
	}
	else
		return ReadOpArg(Address);
}

unsigned short M6809ReadOpArg16(unsigned short Address)
{
	// check mem map
	unsigned char * pr = Read[(Address >> 8)];
	if (pr != NULL) {
		Address&=0xff;		
		return ((pr[Address] <<8) | pr[(Address+1)]);
	}
	else
		return((ReadOpArg(Address)<<8) | ReadOpArg((Address+1)));
}
#endif