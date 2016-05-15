// burn_ym2203.h
#include "../driver.h"
//extern "C" {
 #include "ay8910.h"
 #include "fm.h"
//}
//#include "timer.h"
#define nBurnSoundLen 128

extern void BurnYM2203UpdateRequest();

int BurnYM2203Init(int num, int nClockFrequency, FM_IRQHANDLER IRQCallback, int (*StreamCallback)(int), double (*GetTimeCallback)(), int bAddSignal);
void BurnYM2203SetRoute(int nChip, int nIndex, double nVolume, int nRouteDir);
void BurnYM2203SetLeftVolume(int nChip, int nIndex, double nLeftVolume);
void BurnYM2203SetRightVolume(int nChip, int nIndex, double nRightVolume);
void BurnYM2203Reset();
void BurnYM2203Exit();
extern void (*BurnYM2203Update)(signed short* pSoundBuf, int nSegmentEnd);
void BurnYM2203Scan(int nAction, int* pnMin);

extern int bYM2203UseSeperateVolumes;

#define BURN_SND_YM2203_YM2203_ROUTE		0
#define BURN_SND_YM2203_AY8910_ROUTE_1		1
#define BURN_SND_YM2203_AY8910_ROUTE_2		2
#define BURN_SND_YM2203_AY8910_ROUTE_3		3

#define BurnYM2203SetAllRoutes(i, v, d)							\
	BurnYM2203SetRoute(i, BURN_SND_YM2203_YM2203_ROUTE  , v, d);	\
	BurnYM2203SetRoute(i, BURN_SND_YM2203_AY8910_ROUTE_1, v, d);	\
	BurnYM2203SetRoute(i, BURN_SND_YM2203_AY8910_ROUTE_2, v, d);	\
	BurnYM2203SetRoute(i, BURN_SND_YM2203_AY8910_ROUTE_3, v, d)

#define BurnYM2203SetPSGVolume(i, v) \
	BurnYM2203SetRoute(i, BURN_SND_YM2203_AY8910_ROUTE_1, v, BURN_SND_ROUTE_BOTH); \
	BurnYM2203SetRoute(i, BURN_SND_YM2203_AY8910_ROUTE_2, v, BURN_SND_ROUTE_BOTH); \
	BurnYM2203SetRoute(i, BURN_SND_YM2203_AY8910_ROUTE_3, v, BURN_SND_ROUTE_BOTH);

#define BurnYM2203Read(i, a) YM2203Read(i, a)

#define BurnYM2203Write(i, a, n) YM2203Write(i, a, n)
#define BurnYM2203SetPorts(c, read0, read1, write0, write1)	AY8910SetPorts(c, read0, read1, write0, write1)
