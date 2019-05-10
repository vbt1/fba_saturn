#define		_SPR3_
#include "sega_spr.h"

#define USE_LUT 1

#define TMS9928A_PALETTE_SIZE           16
typedef enum
{
	TMS_INVALID_MODEL,
	TMS99x8,
	TMS9929,
	TMS99x8A,
	TMS9929A
} tms9928a_model;


typedef struct {
	unsigned char mode;
	unsigned char ReadAhead;
	unsigned char Regs[8];
	unsigned char StatusReg;
	unsigned char FifthSprite;
	unsigned char FirstByte;
	unsigned char latch;
	unsigned char INT;
	unsigned int Addr;
	unsigned int colour;
	unsigned int pattern;
	unsigned int nametbl;
	unsigned int spriteattribute;
	int spritepattern;
	unsigned int colourmask;
	unsigned int patternmask;

//	unsigned char vMem[0x4000];
	unsigned char *vMem;
	unsigned char *tmpbmp;
	int vramsize;
	int model;
	int revA;

	int LimitSprites;
	int top_border;
	int bottom_border;
	int vertical_size;

//unsigned char dirty[24*32*8*4];
unsigned char *dirty;
#ifdef USE_LUT
//unsigned int color_2bpp_lut[0x400];
unsigned int *color_2bpp_lut;
#endif
	void (*INTCallback)(int);
} TMS9928A;


#define nScreenWidth 256
#define nScreenHeight 192
#define	RGB( r, g, b )		(0x8000U|((b) << 10)|((g) << 5 )|(r))

extern unsigned short *colAddr;
extern unsigned short *ss_map;
extern SprSpCmd *ss_sprite;

/* Some defines used in defining the screens */
#define TMS9928A_TOTAL_HORZ                 342
#define TMS9928A_TOTAL_VERT_NTSC            262
#define TMS9928A_TOTAL_VERT_PAL             313

#define TMS9928A_HORZ_DISPLAY_START         (2 + 14 + 8 + 13)
#define TMS9928A_VERT_DISPLAY_START_PAL     16
#define TMS9928A_VERT_DISPLAY_START_NTSC    16

/*static*/ TMS9928A tms;

void  TMS9928AInterrupt();
void TMS9928AInit(int model, int vram, int borderx, int bordery, void (*INTCallback)(int), unsigned char *TMSContext);
void TMS9928AReset();
void TMS9928AExit();
void TMS9928ADraw();

void TMS9928ASetSpriteslimit(int limit);

void TMS9928AWriteRegs(int data);
unsigned char TMS9928AReadRegs();
unsigned char SG_TMS9928AReadVRAM();
void TMS9928AWriteVRAM(int data);
unsigned char TMS9928AReadVRAM(TMS9928A *tms);
