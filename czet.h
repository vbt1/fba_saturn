// Z80 (Zed Eight-Ty) Interface
 #include "cz80/cz80m.h"

#ifndef FASTCALL
 #undef __fastcall
 #define __fastcall
#endif

#define MAP_READ		1
#define MAP_WRITE		2
#define MAP_FETCHOP		4
#define MAP_FETCHARG		8
#define MAP_FETCH		(MAP_FETCHOP|MAP_FETCHARG)
#define MAP_ROM			(MAP_READ|MAP_FETCH)
#define MAP_RAM			(MAP_ROM|MAP_WRITE)

// #define CZET_IRQSTATUS_NONE 0 //CZ80_IRQSTATUS_NONE
// #define CZET_IRQSTATUS_AUTO 2//CZ80_IRQSTATUS_AUTO
// #define CZET_IRQSTATUS_ACK  1//CZ80_IRQSTATUS_ACK
  #define CZET_IRQSTATUS_NONE CZ80_IRQSTATUS_NONE
 #define CZET_IRQSTATUS_AUTO CZ80_IRQSTATUS_AUTO
 #define CZET_IRQSTATUS_ACK  CZ80_IRQSTATUS_ACK
 #define CZET_IRQSTATUS_HOLD  4
 #define CZetRaiseIrq(n) CZetSetIRQLine(n, CZET_IRQSTATUS_AUTO)
 #define CZetLowerIrq() CZetSetIRQLine(0, CZET_IRQSTATUS_NONE)

int CZetGetPC(int n);
int CZetInit2(int nCount,UINT8 *addr);
void CZetExit2();
void CZetNewFrame();
int CZetOpen(int nCPU);
void CZetClose();
int CZetMemCallback(int nStart,int nEnd,int nMode);
int CZetMemEnd();
void CZetMapMemory(unsigned char *Mem, int nStart, int nEnd, int nFlags);
void CZetMapMemory2(unsigned char *Mem, unsigned char *Mem02, int nStart, int nEnd, int nFlags);
int CZetMapArea(int nStart, int nEnd, int nMode, unsigned char *Mem);
int CZetMapArea2(int nStart, int nEnd, int nMode, unsigned char *Mem01, unsigned char *Mem02);
void CZetReset();
int CZetPc(int n);
int CZetBc(int n);
int CZetHL(int n);
int CZetScan(int nAction);
int CZetRun(int nCycles);
void CZetRunEnd();
void CZetSetIRQLine(const int line, const int status);
int CZetNmi();
int CZetIdle(int nCycles);
int CZetSegmentCycles();
int CZetTotalCycles();

#define MAX_CPUS	4

void CZetSetReadHandler(unsigned char (__fastcall *pHandler)(unsigned short));
void CZetSetWriteHandler(void (__fastcall *pHandler)(unsigned short, unsigned char));
void CZetSetInHandler(unsigned char (__fastcall *pHandler)(unsigned short));
void CZetSetOutHandler(void (__fastcall *pHandler)(unsigned short, unsigned char));
