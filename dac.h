typedef signed char INT8;
typedef unsigned char UINT8;
typedef signed short INT16;
typedef unsigned short UINT16;
typedef signed int INT32;
typedef unsigned int UINT32;

void DACUpdate(INT16* Buffer, INT32 Length);
void DACWrite(INT32 Chip, UINT8 Data);
void DACSignedWrite(INT32 Chip, UINT8 Data);
void DACInit(INT32 Num, UINT32 Clock, INT32 bAdd, INT32 (*pSyncCB)());
void DACSetRoute(INT32 Chip, double nVolume, INT32 nRouteDir);
void DACReset();
void DACExit();
INT32 DACScan(INT32 nAction,INT32 *pnMin);
