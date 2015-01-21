// Burn - Arcade emulator library - internal code

// Standard headers
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "burn.h"
//#define SC_RELEASE 1
#define		_SPR3_
#include "sega_spr.h"
#include "sega_scl2.h"
#include "sega_pcm.h"

#define	RING_BUF_SIZE	(2048L*5)//(2048L*10)
#define SOUND_BUFFER    0x25a20000


#undef  SCL_MAXLINE
#undef  SCL_MAXCELL
#define	SCL_MAXLINE	 256//256
#define	SCL_MAXCELL	32//32

extern void (*nBurnFunction)();
extern Uint32 SclColRamAlloc256[8];
extern SclLineparam lp;

#define OPEN_CSH_VAR(a) (((int)&a | 0x20000000))
#define	RGB( r, g, b )		(0x8000U|((b) << 10)|((g) << 5 )|(r))
extern PcmHn pcm;
extern unsigned char hz;
extern unsigned short palette[8];

extern int nBurnSoundRate;			// sample rate of sound or zero for no sound
extern unsigned int nBurnLinescrollSize;
extern unsigned char nBurnSprites;
extern unsigned int nBurnDrvCount;		// Count of game drivers
extern unsigned int nBurnDrvSelect;	// Which game driver is selected
//extern unsigned int nBurnMallocAddr; // address before loading first game

// ---------------------------------------------------------------------------
// Sound clipping macro
#define BURN_SND_CLIP(A) ((A) < -0x8000 ? -0x8000 : (A) > 0x7fff ? 0x7fff : (A))

// sound routes
#define BURN_SND_ROUTE_LEFT			1
#define BURN_SND_ROUTE_RIGHT		2
#define BURN_SND_ROUTE_BOTH			(BURN_SND_ROUTE_LEFT | BURN_SND_ROUTE_RIGHT)

// ---------------------------------------------------------------------------
// CPU emulation interfaces
#include "czet.h"
typedef union {
#ifdef LSB_FIRST
	struct { UINT8 l,h,h2,h3; } b;
	struct { UINT16 l,h; } w;
#else
	struct { UINT8 h3,h2,h,l; } b;
	struct { UINT16 h,l; } w;
#endif
	UINT32 d;
}	PAIR;

// ---------------------------------------------------------------------------
// Driver information

struct BurnDriver {
	char* szShortName;			// The filename of the zip file (without extension)
	char* szParent;				// The filename of the parent (without extension, NULL if not applicable)
	char*    szFullNameA; 
	int (*GetRomInfo)(struct BurnRomInfo* pri,unsigned int i);		// Function to get the length and crc of each rom
	int (*GetRomName)(char** pszName, unsigned int i, int nAka);	// Function to get the possible names for each rom
	int (*GetInputInfo)(struct BurnInputInfo* pii, unsigned int i);	// Function to get the input info for the game
	int (*GetDIPInfo)(struct BurnDIPInfo* pdi, unsigned int i);		// Function to get the input info for the game
	int (*Init)(); int (*Exit)(); int (*Frame)(); int (*Redraw)(); //int (*AreaScan)(int nAction, int* pnMin);
};

// Standard functions for dealing with ROM and input info structures
#include "stdfunc.h"

// ---------------------------------------------------------------------------

// burn.cpp
int BurnByteswap(UINT8* pm,int nLen);

// load.cpp
int BurnLoadRom(UINT8* Dest,int i, int nGap);
int BurnXorRom(UINT8* Dest,int i, int nGap);
int BurnLoadBitField(UINT8* pDest, UINT8* pSrc, int nField, int nSrcLen);
