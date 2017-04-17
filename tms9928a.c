#include "tms9928a.h"
#include "burn.h"
#define		_SPR3_
#include "sega_spr.h"
#define COLADDR (FBUF_ADDR-0x400)
#define COLADDR_SPR	(COLADDR>>3)
#define nScreenWidth 256
#define nScreenHeight 192
#define	RGB( r, g, b )		(0x8000U|((b) << 10)|((g) << 5 )|(r))

//#define USE_LUT 1

//char pattern_lut[256][8];
//extern unsigned char *pTransDraw;
extern unsigned short *colAddr;
extern unsigned short  *ss_map;
extern unsigned int *shared;
#ifdef USE_LUT
//unsigned int *color_2bpp_lut;
static unsigned int color_2bpp_lut[256*4];
#endif

#define SS_SPRAM *(&shared + 5)
#define SS_FONT	 *(&shared + 3)
extern SprSpCmd *ss_sprite;

//unsigned int colour_dirty	 =0xffff;
//unsigned int pattern_dirty =0xffff;
unsigned int dirty[24*32*8*4];
unsigned char dirtyc[24*32*8*4];
//unsigned int dirtys[32];

//unsigned int pattern6[0x400];
//unsigned int pattern4[0x400];

/* Some defines used in defining the screens */
#define TMS9928A_TOTAL_HORZ                 342
#define TMS9928A_TOTAL_VERT_NTSC            262
#define TMS9928A_TOTAL_VERT_PAL             313

#define TMS9928A_HORZ_DISPLAY_START         (2 + 14 + 8 + 13)
#define TMS9928A_VERT_DISPLAY_START_PAL     16
#define TMS9928A_VERT_DISPLAY_START_NTSC    16

static INT32 TMS9928A_palette[16] = {
	0x000000, 0x000000, 0x21c842, 0x5edc78, 0x5455ed, 0x7d76fc, 0xd4524d, 0x42ebf5,
	0xfc5554, 0xff7978, 0xd4c154, 0xe6ce80, 0x21b03b, 0xc95bba, 0xcccccc, 0xffffff
};

typedef struct {
	UINT8 mode;
	UINT8 ReadAhead;
	UINT8 Regs[8];
	UINT8 StatusReg;
	UINT8 FifthSprite;
	UINT8 FirstByte;
	UINT8 latch;
	UINT8 INT;
	INT32 Addr;
	INT32 colour;
	INT32 pattern;
	INT32 nametbl;
	INT32 spriteattribute;
	INT32 spritepattern;
	INT32 colourmask;
	INT32 patternmask;

//	unsigned char *vMem;
	unsigned char vMem[0x4000];
//	unsigned char *dBackMem;
//	unsigned short *tmpbmp;
	unsigned char *tmpbmp;
	INT32 vramsize;
	INT32 model;

	INT32 LimitSprites;
	INT32 top_border;
	INT32 bottom_border;
	INT32 vertical_size;

	void (*INTCallback)(INT32);
} TMS9928A;

static TMS9928A tms;

//static unsigned int Palette[16]; // high color support

static void TMS89928aPaletteRecalc()
{
//	unsigned short *col =(unsigned short *)COLADDR;
	for (int i = 0; i < 16; i++) 
	{
		colAddr[i] = RGB((TMS9928A_palette[i] >> 19)&31,(TMS9928A_palette[i] >> 11)&31,(TMS9928A_palette[i] >> 3)&31);
	}
}

static void check_interrupt()
{
	INT32 b = (tms.StatusReg & 0x80 && tms.Regs[1] & 0x20) ? 1 : 0;
	if (b != tms.INT) {
		tms.INT = b;
		if (tms.INTCallback) tms.INTCallback (tms.INT);
	}
}

static void update_table_masks()
{
	tms.colourmask = (tms.Regs[3] & 0x7f) * 8 | 7;
	tms.patternmask = (tms.Regs[4] & 3) * 256 |	(tms.colourmask & 0xff);
}

static void change_register(INT32 reg, UINT8 val)
{
	static const UINT8 Mask[8] = { 0x03, 0xfb, 0x0f, 0xff, 0x07, 0x7f, 0x07, 0xff };

	val &= Mask[reg];
	tms.Regs[reg] = val;

	switch (reg)
	{
		case 0:
		{
			if (val & 2) {
				tms.colour = ((tms.Regs[3] & 0x80) * 64) & (tms.vramsize - 1);
				tms.pattern = ((tms.Regs[4] & 4) * 2048) & (tms.vramsize - 1);
				update_table_masks();
			} else {
				tms.colour = (tms.Regs[3] * 64) & (tms.vramsize - 1);
				tms.pattern = (tms.Regs[4] * 2048) & (tms.vramsize - 1);
			}
			tms.mode = (((tms.Regs[1] & 0x10)>>4) | ((tms.Regs[1] & 8)>>1));
		}
		break;

		case 1:
		{
			tms.mode = (((tms.Regs[1] & 0x10)>>4) | ((tms.Regs[1] & 8)>>1));
			check_interrupt();
		}
		break;

		case 2:
			tms.nametbl = (val * 1024) & (tms.vramsize - 1);
		break;

		case 3:
		{
			if (tms.Regs[0] & 2) {
				tms.colour = ((val & 0x80) * 64) & (tms.vramsize - 1);
				update_table_masks();
			} else {
				tms.colour = (val * 64) & (tms.vramsize - 1);
			}
		}
		break;

		case 4:
		{
			if (tms.Regs[0] & 2) {
				tms.pattern = ((val & 4) * 2048) & (tms.vramsize - 1);
				update_table_masks();
			} else {
				tms.pattern = (val * 2048) & (tms.vramsize - 1);
			}
		}
		break;

		case 5:
			tms.spriteattribute = (val * 128) & (tms.vramsize - 1);
		break;

		case 6:
			tms.spritepattern = (val * 2048) & (tms.vramsize - 1);
		break;

		case 7:
			/* The backdrop is updated at TMS9928A_refresh() */
		break;
	}
}

void TMS9928AReset()
{
	for (INT32 i = 0; i < 8; i++)
		tms.Regs[i] = 0;

	memset(tms.vMem, 0, tms.vramsize);
//	memset(tms.tmpbmp, 0, tms.tmpbmpsize);

	tms.StatusReg = 0;
	tms.FifthSprite = 31;
	tms.nametbl = tms.pattern = tms.colour = 0;
	tms.spritepattern = tms.spriteattribute = 0;
	tms.colourmask = tms.patternmask = 0x3fff;
	tms.Addr = tms.ReadAhead = tms.INT = 0;
	tms.FirstByte = 0;
	tms.latch = 0;
}

#ifdef USE_LUT
static void make_lut()
{
	unsigned char bg,fg;

	for (int bg=0;bg<16;bg++)
	{
		for (int fg=0;fg<16;fg++)
		{
			unsigned int *position =	 &color_2bpp_lut[(bg|fg<<4)*4];
			position[0] = bg|bg<<4;
			position[1] = fg|bg<<4;
			position[2] = bg|fg<<4;
			position[3] = fg|fg<<4;
		}
	}
}
#endif

void TMS9928AInit(INT32 model, INT32 vram, INT32 borderx, INT32 bordery, void (*INTCallback)(int))
{
//	GenericTilesInit();
	memset(&tms, 0, sizeof(tms));
#ifdef USE_LUT
	make_lut();
#endif
	tms.model = model;

	tms.INTCallback = INTCallback;

	tms.top_border		= TMS9928A_VERT_DISPLAY_START_NTSC;
	tms.bottom_border	= ((tms.model == TMS9929) || (tms.model == TMS9929A)) ? 51 : 24;
	tms.vertical_size   = TMS9928A_TOTAL_VERT_NTSC;

	tms.vramsize = vram;
//	tms.vMem = (unsigned char*)0x25e60000;//(unsigned char*)malloc(tms.vramsize);
//	tms.vMem = (unsigned char*)malloc(tms.vramsize);

//	tms.dBackMem = (unsigned char*)malloc(256 * 192);

//	tms.tmpbmp = (unsigned short*)malloc(256 * 192 * sizeof(short));
//	tms.tmpbmp = (unsigned char*)malloc(256 * 192 * sizeof(char));
//	tms.tmpbmp = (unsigned char*)0x00200000;

	unsigned char *ss_vram = (unsigned char *)SS_SPRAM;
	tms.tmpbmp	= (ss_vram+0x1100+0x10000);
	memset(tms.tmpbmp, 0, 256 * 192 * sizeof(char));
	TMS89928aPaletteRecalc();
	TMS9928AReset ();
	tms.LimitSprites = 1;


	memset(dirty,0xFF,192*128);
}

void TMS9928AExit()
{
	TMS9928AReset();

//	GenericTilesExit();
	tms.INTCallback = NULL;
//	free (tms.tmpbmp);
	tms.tmpbmp = NULL;
//	free (tms.vMem);
//	tms.vMem=NULL;
//	free (tms.dBackMem);
}

void TMS9928APostLoad()
{
	for (INT32 i = 0; i < 8; i++)
		change_register(i, tms.Regs[i]);

	if (tms.INTCallback) tms.INTCallback(tms.INT);
}

UINT8 TMS9928AReadVRAM()
{
	INT32 b = tms.ReadAhead;
	tms.ReadAhead = tms.vMem[tms.Addr];
	tms.Addr = (tms.Addr + 1) & (tms.vramsize - 1);
	tms.latch = 0;
	return b;
}

void TMS9928AWriteVRAM(INT32 data)
{
	tms.vMem[tms.Addr] = data;
	tms.Addr = (tms.Addr + 1) & (tms.vramsize - 1);
	tms.ReadAhead = data;
	tms.latch = 0;
}

UINT8 TMS9928AReadRegs()
{
	INT32 b = tms.StatusReg;
	tms.StatusReg = tms.FifthSprite;
	check_interrupt();
	tms.latch = 0;
	return b;
}

void TMS9928AWriteRegs(INT32 data)
{
	if (tms.latch) {
		/* set high part of read/write address */
		tms.Addr = ((UINT16)data << 8 | (tms.Addr & 0xff)) & (tms.vramsize - 1);
		if (data & 0x80) {
			change_register(data & 0x07, tms.FirstByte);
		} else {
			if (!(data & 0x40)) {
				TMS9928AReadVRAM();
			}
		}

		tms.latch = 0;
	} else {
		/* set low part of read/write address */
		tms.Addr = ((tms.Addr & 0xff00) | data) & (tms.vramsize - 1);
		tms.FirstByte = data;
		tms.latch = 1;
	}
}

void TMS9928ASetSpriteslimit(INT32 limit)
{
	tms.LimitSprites = limit;
}

static inline UINT8 readvmem(INT32 vaddr)
{
	return tms.vMem[vaddr];
}

static void draw_mode0(unsigned char *bitmap,unsigned char *vmem)
{
	unsigned char *patternptr;
	unsigned int tab[4];
	unsigned char *vbt,*dirtycptr;
	unsigned int *dirtyptr;

	dirtyptr=(unsigned char *)dirty;
	dirtycptr=(unsigned char *)dirtyc;

	for (unsigned int y = 0; y < 24; y++)
	{
		for (unsigned int x = 0; x < 32; x++)
		{
			unsigned int charcode = *vmem++;
			patternptr = tms.vMem + tms.pattern + charcode*8;

			unsigned int colour = tms.vMem[tms.colour+charcode/8];

			for (unsigned int yy = 0; yy < 8; yy++)
			{
				unsigned int pattern = *patternptr++;

				if(*dirtyptr!=pattern || *dirtycptr!=colour)
				{
					*dirtyptr=pattern;
					*dirtycptr=colour;

					vbt = (unsigned char *)&bitmap[(y * 8 + yy) * 128 + (x*4)];

					int bg = colour & 0x0f;
					tab[0]=bg|bg<<4;

					if(pattern!=0)
					{
						unsigned int fg = colour >> 4;
						tab[1] = fg|(*tab&0xf0);
						tab[2] = colour;
						tab[3] = fg|(colour&0xf0); 

						vbt[0] = tab[(pattern>>6)&3]; 
						vbt[1] = tab[(pattern>>4)&3]; 
						vbt[2] = tab[(pattern>>2)&3]; 
						vbt[3] = tab[(pattern>>0)&3];
					}
					else
						vbt[0] = vbt[1] = vbt[2] = vbt[3] = *tab;
				}
				++dirtyptr;
				++dirtycptr;
			}
		}
	}
}

static void draw_mode1(unsigned char *bitmap,unsigned char *vmem)
{
	int pattern,x,y,yy,xx,name,charcode;
	unsigned char fg,bg,*patternptr;
//	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 1 not supported    ",16,150);
	 return;

	fg = tms.Regs[7] / 16;
	bg = tms.Regs[7] & 15;

	for (y = 0; y < 191; y++) {
		for (x = 0; x < 7; x++) {
			bitmap[y * 256 + x + 248] = bitmap[y * 256 + x] = bg;
		}
	}

    name = 0;

    for (y=0;y<24;y++) {
        for (x=0;x<40;x++) {
            charcode = tms.vMem[tms.nametbl+name];
            name++;
            patternptr = tms.vMem + tms.pattern + (charcode*8);
            for (yy=0;yy<8;yy++) {
                pattern = *patternptr++;
                for (xx=0;xx<6;xx++) {
			bitmap[(y * 8 + yy) * 256 + (8 + x*6+xx)] = (pattern & 0x80) ? fg : bg;
			pattern *= 2;
                }
            }
        }
    }
}

static void draw_mode12(unsigned char *bitmap,unsigned char *vmem)
{
	int pattern,x,y,yy,xx,name,charcode;
	unsigned char fg,bg,*patternptr;
//	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 12 not supported    ",16,150);
	 return;

	fg = tms.Regs[7] / 16;
	bg = tms.Regs[7] & 15;

	for (y = 0; y < 191; y++) {
		for (x = 0; x < 7; x++) {
			bitmap[y * 256 + x + 248] = bitmap[y * 256 + x] = bg;
		}
	}

    name = 0;
    for (y=0;y<24;y++) {
        for (x=0;x<40;x++) {
            charcode = (tms.vMem[tms.nametbl+name]+(y/8)*256)&tms.patternmask;
            name++;
            patternptr = tms.vMem + tms.pattern + (charcode*8);
            for (yy=0;yy<8;yy++) {
                pattern = *patternptr++;
                for (xx=0;xx<6;xx++) {
			bitmap[(y * 8 + yy) * 256 + (8 + x*6+xx)] = (pattern & 0x80) ? fg : bg;
			pattern *= 2;
                }
            }
        }
    }
}



static void draw_mode2(unsigned char *bitmap,unsigned char *vmem)
{
    unsigned int colour,x,y,yy,pattern,xx,charcode;
    unsigned char *colourptr,*patternptr;
#ifndef USE_LUT
	unsigned int fg,bg;
	unsigned int tab[4];
#else
	unsigned int *tab;
#endif
	unsigned char *vbt,*dirtycptr;
	unsigned int *dirtyptr;
	dirtyptr=(unsigned int *)dirty;
	dirtycptr=(unsigned char *)dirtyc;

    for (y=0;y<24;y++) 
	{
//		unsigned int y8=(y/8)*256;
        for (x=0;x<128;x+=4) 
		{
            charcode = (*vmem++)+(y/8)*256;
//			if(charcode_dirty[x][y]!=charcode|| tms_dirty)
			{
				colour =  (charcode&tms.colourmask);
				pattern = (charcode&tms.patternmask);
				patternptr = tms.vMem+tms.pattern+colour*8;
				colourptr = tms.vMem+tms.colour+pattern*8;

				for (yy=0;yy<8;yy++) 
				{
					pattern = *patternptr++;
					colour = *colourptr++;

					if(*dirtyptr!=pattern || *dirtycptr!=colour)
					{
						*dirtyptr=pattern;
						*dirtycptr=colour;

						vbt = (unsigned char *)&bitmap[(y * 8 + yy) * 128 + x];

						bg = colour & 15;
						tab[0]=bg|bg<<4;

	 					if(pattern!=0)
						{
							fg = colour /16;
							tab[1] = fg|(*tab&0xf0);
							tab[2] = colour;
							tab[3] = fg|(colour&0xf0);

							vbt[0] = tab[(pattern>>6)&3]; 
							vbt[1] = tab[(pattern>>4)&3]; 
							vbt[2] = tab[(pattern>>2)&3]; 
							vbt[3] = tab[(pattern)&3];
						}
						else
						{
							vbt[0] = vbt[1] = vbt[2] = vbt[3] = *tab;
						}
					}
					++dirtyptr;
					++dirtycptr;
				}
			}
        }
    }
}

static void draw_mode2_v1(unsigned char *bitmap,unsigned char *vmem)
{
    unsigned int colour,x,y,yy,pattern,xx,charcode;
    unsigned char *colourptr,*patternptr;
#ifndef USE_LUT
	unsigned int fg,bg;
	unsigned int tab[4];
#else
	unsigned int *tab;
#endif
	unsigned char *vbt,*dirtycptr;
	unsigned int *dirtyptr;

	dirtyptr=(unsigned int *)dirty;
	dirtycptr=(unsigned char *)dirtyc;

    for (y=0;y<24;y++) 
	{
//		unsigned int y8=(y/8)*256;
        for (x=0;x<32;x++) 
		{
            charcode = (*vmem++)+(y/8)*256;
//			if(charcode_dirty[x][y]!=charcode|| tms_dirty)
			{
				colour =  (charcode&tms.colourmask);
				pattern = (charcode&tms.patternmask);
				patternptr = tms.vMem+tms.pattern+colour*8;
				colourptr = tms.vMem+tms.colour+pattern*8;

				for (yy=0;yy<8;yy++) 
				{

					pattern = *patternptr++;
					colour = *colourptr++;

					if(*dirtyptr!=pattern || *dirtycptr!=colour)
					{
						*dirtyptr=pattern;
						*dirtycptr=colour;
						vbt = (unsigned char *)&bitmap[(y * 8 + yy) * 128 + (x*4)];

#ifndef USE_LUT
						bg = colour & 15;
						tab[0]=bg|bg<<4;
#else
						
#endif
	 					if(pattern!=0)
						{	  
#ifndef USE_LUT
							fg = colour /16;
							tab[1] = fg|(*tab&0xf0);
							tab[2] = colour;
							tab[3] = fg|(colour&0xf0);
#else
							tab = &color_2bpp_lut[colour*4];
#endif
							vbt[0] = tab[(pattern>>6)&3]; 
							vbt[1] = tab[(pattern>>4)&3]; 
							vbt[2] = tab[(pattern>>2)&3]; 
							vbt[3] = tab[(pattern)&3];
						}
						else
#ifndef USE_LUT
							vbt[0] = vbt[1] = vbt[2] = vbt[3] = *tab;
#else
							vbt[0] = vbt[1] = vbt[2] = vbt[3] = color_2bpp_lut[colour*4];
#endif
					}
					++dirtyptr;
					++dirtycptr;
				}
			}
        }
    }
}


static void draw_mode3(unsigned char *bitmap,unsigned char *vmem)
{
    int x,y,yy,yyy,name,charcode;
    unsigned char fg,bg,*patternptr;

//	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 3 not supported    ",16,150);
	 return;

    name = 0;
    for (y=0;y<24;y++) {
        for (x=0;x<32;x++) {
            charcode = tms.vMem[tms.nametbl+name];
            name++;
            patternptr = tms.vMem+tms.pattern+charcode*8+(y&3)*2;
            for (yy=0;yy<2;yy++) {
                fg = (*patternptr / 16);
                bg = ((*patternptr++) & 15);
                for (yyy=0;yyy<4;yyy++) {
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+0)] = fg;
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+1)] = fg;
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+2)] = fg;
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+3)] = fg;
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+4)] = fg;
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+5)] = fg;
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+6)] = fg;
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+7)] = fg;
                }
            }
        }
    }
}

static void draw_mode23(unsigned char *bitmap,unsigned char *vmem)
{
    int x,y,yy,yyy,name,charcode;
    unsigned char fg,bg,*patternptr;

//	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 23 not supported    ",16,150);
	 return;

    name = 0;
    for (y=0;y<24;y++) {
        for (x=0;x<32;x++) {
            charcode = tms.vMem[tms.nametbl+name];
            name++;
            patternptr = tms.vMem + tms.pattern +
                ((charcode+(y&3)*2+(y/8)*256)&tms.patternmask)*8;
            for (yy=0;yy<2;yy++) {
                fg = (*patternptr / 16);
                bg = ((*patternptr++) & 15);
                for (yyy=0;yyy<4;yyy++) {
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+0)] = fg;
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+1)] = fg;
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+2)] = fg;
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+3)] = fg;
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+4)] = fg;
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+5)] = fg;
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+6)] = fg;
			bitmap[(y*8+yy*4+yyy) * 256 + (x*8+7)] = fg;
                }
            }
        }
    }
}

static void draw_modebogus(unsigned char *bitmap,unsigned char *vmem)
{
    unsigned char fg,bg;
    int x,y,n,xx;

//	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode bogus not supported    ",16,150);
	 return;

    fg = tms.Regs[7] / 16;
    bg = tms.Regs[7] & 15;

    for (y=0;y<192;y++) {
        xx=0;
        n=8; while (n--) bitmap[y * 256 + (xx++)] = bg;
        for (x=0;x<40;x++) {
            n=4; while (n--) bitmap[y * 256 + (xx++)] = fg;
            n=2; while (n--) bitmap[y * 256 + (xx++)] = bg;
        }
        n=8; while (n--) bitmap[y * 256 + (xx++)] = bg;
    }
}

static void (*const ModeHandlers[])(unsigned char *bitmap,unsigned char *vmem) = {
        draw_mode0, draw_mode1, draw_mode2,  draw_mode12,
        draw_mode3, draw_modebogus, draw_mode23,
        draw_modebogus
};

void draw_sprites(unsigned char *attributeptr,unsigned char *spritepatternptr, SprSpCmd *ss_spritePtr,unsigned int size)
{
	unsigned char *patternptr,c,yy,xx;
	short x,y;
	unsigned short line;
	tms.StatusReg = 0x80;

	unsigned char *ss_vram = (unsigned char *)SS_SPRAM+0x1100;
	unsigned char tab[4];
	tab[0]=0x00;

    for (unsigned char p=0;p<32;p++) 
	{
        y = *attributeptr++;
		if (y == 208) 
		{
			for (;p<32;++p) 
			{
			ss_spritePtr->charSize   = 0;
			ss_spritePtr->ax			= 0;
			ss_spritePtr->ay			= 0;
			ss_spritePtr->control		= 0;
			++ss_spritePtr;
			}
			break;
		}
        if (y > 208) 
		{
            y=-(~y&255);
        } 
		else 
		{
            y++;
        }
		if (y <0) 
		{
			ss_spritePtr->charSize	= 0;
			ss_spritePtr->ax			= 0;
			ss_spritePtr->ay			= 0;
			ss_spritePtr->control		= 0;
			++ss_spritePtr;
			attributeptr+=3;
			continue;
		}	  
		x = *attributeptr++;
        patternptr = spritepatternptr+ ((size == 16) ? *attributeptr & 0xfc : *attributeptr) * 8;
        ++attributeptr;
        c = (*attributeptr& 0x0f);
        if (*attributeptr & 0x80) x -= 32;
        ++attributeptr;

		ss_spritePtr->ax				= x;
		ss_spritePtr->ay				= y;
//		ss_spritePtr->color			= c;
//		if(*dirtysptr!=&patternptr[0])
		{
//			*dirtysptr = &patternptr[0];

			ss_spritePtr->charSize		= (size==8)? 0x108:0x210;
			ss_spritePtr->drawMode	= ( COLOR_0| ECD_DISABLE | COMPO_REP);
			ss_spritePtr->control			= (JUMP_NEXT | FUNC_NORMALSP);
			ss_spritePtr->charAddr		= 0x220+(p<<4);
			++ss_spritePtr;

			tab[1]=c;
			tab[2]=(c<<4);
			tab[3]=c|tab[2];
			/* draw sprite (not enlarged) */
			for (yy=0;yy<size;yy++) 
			{
				line = (patternptr[yy]<<8)| + patternptr[yy+16];

				if(line!=0)
				{
					unsigned char *vbt = (unsigned char *)&ss_vram[(yy*(size>>1))+(p<<7)];

					vbt[0]=tab[(line>>14)&3];
					vbt[1]=tab[(line>>12)&3];
					vbt[2]=tab[(line>>10)&3];
					vbt[3]=tab[(line>>8)&3];
					if(size>8)
					{
						vbt[4]=tab[(line>>6)&3];
						vbt[5]=tab[(line>>4)&3];
						vbt[6]=tab[(line>>2)&3];
						vbt[7]=tab[(line>>0)&3];
					}
				}
				else
				{
					unsigned int *vbt = (unsigned int *)&ss_vram[(yy*(size>>1))+(p<<7)];
					vbt[0]=vbt[1]=0;
				}
			} 
		}
//		++dirtysptr;
	}
	tms.StatusReg |= 0x40;
}


void draw_sprites_large(unsigned char *attributeptr,unsigned char *spritepatternptr, SprSpCmd *ss_spritePtr,unsigned int size)
{
	unsigned char *patternptr,c,yy,xx;
	short x,y;
	unsigned short line;
	tms.StatusReg = 0x80;

	unsigned char *ss_vram = (unsigned char *)SS_SPRAM+0x1100;
	unsigned char tab[4];
	tab[0]=0x00;

    for (unsigned char p=0;p<32;p++) 
	{
        y = *attributeptr++;
		if (y == 208) 
		{
			for (;p<32;++p) 
			{
			ss_spritePtr->charSize   = 0;
			ss_spritePtr->control		= 0;
			++ss_spritePtr;
			}
			break;
		}
        if (y > 208) 
		{
            y=-(~y&255);
        } 
		else 
		{
            y++;
        }
		if (y <0) 
		{
			ss_spritePtr->charSize	= 0;
			ss_spritePtr->ax			= 0;
			ss_spritePtr->ay			= 0;
			ss_spritePtr->charAddr	= 0;
			ss_spritePtr->control		= 0;
			++ss_spritePtr;
			attributeptr+=3;
			continue;
		}	  
		x = *attributeptr++;
        patternptr = spritepatternptr+ ((size == 16) ? *attributeptr & 0xfc : *attributeptr) * 8;
        ++attributeptr;
        c = (*attributeptr& 0x0f);
        if (*attributeptr & 0x80) x -= 32;
        ++attributeptr;

		ss_spritePtr->ax				= x;
		ss_spritePtr->ay				= y;
//		ss_spritePtr->bx				= x+size*2;
//		ss_spritePtr->by				= y+size*2;
		ss_spritePtr->cx				= x+size*2;
		ss_spritePtr->cy				= y+size*2;	 
		ss_spritePtr->control			= (JUMP_NEXT | ZOOM_NOPOINT | FUNC_SCALESP);
//		ss_spritePtr->color			= c;
//		if(*dirtysptr!=&patternptr[0])
		{
//			*dirtysptr = &patternptr[0];

			ss_spritePtr->charSize		= (size==8)? 0x108:0x210;
			ss_spritePtr->drawMode	= ( COLOR_0| ECD_DISABLE | COMPO_REP);		
			ss_spritePtr->charAddr		= 0x220+(p<<4);
			++ss_spritePtr;

			tab[1]=c;
			tab[2]=(c<<4);
			tab[3]=c|tab[2];
			/* draw sprite (not enlarged) */
			for (yy=0;yy<size;yy++) 
			{
				line = (patternptr[yy]<<8)| + patternptr[yy+16];

				if(line!=0)
				{
					unsigned char *vbt = (unsigned char *)&ss_vram[(yy*(size>>1))+(p<<7)];

					vbt[0]=tab[(line>>14)&3];
					vbt[1]=tab[(line>>12)&3];
					vbt[2]=tab[(line>>10)&3];
					vbt[3]=tab[(line>>8)&3];
					if(size>8)
					{
						vbt[4]=tab[(line>>6)&3];
						vbt[5]=tab[(line>>4)&3];
						vbt[6]=tab[(line>>2)&3];
						vbt[7]=tab[(line>>0)&3];
					}
				}
				else
				{
					unsigned int *vbt = (unsigned int *)&ss_vram[(yy*(size>>1))+(p<<7)];
					vbt[0]=vbt[1]=0;
				}
			} 
		}
//		++dirtysptr;
	}
	tms.StatusReg |= 0x40;
}

static void TMS9928AScanline_INT(INT32 vpos)
{
//FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"TMS9928AScanline_INT        ",16,150);

	UINT16 BackColour = tms.Regs[7] & 0xf;
//	UINT16 *p = tms.tmpbmp + (vpos * TMS9928A_TOTAL_HORZ);
	UINT8 *p = tms.tmpbmp + (vpos * TMS9928A_TOTAL_HORZ);

	INT32 y = vpos - tms.top_border;

	if ( y < 0 || y >= 192 || !(tms.Regs[1] & 0x40) )
	{
//FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"TMS9928AScanline_INT no draw        ",16,150);

		/* Draw backdrop colour */
// vbt inutile
//		for ( INT32 i = 0; i < TMS9928A_TOTAL_HORZ; i++ )
//			p[i] = BackColour;

		/* vblank is set at the last cycle of the first inactive line */
		if ( y == 193 )
		{
			tms.StatusReg |= 0x80;
			check_interrupt();
		}
	}
	else
	{
FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"TMS9928AScanline_INT draw           ",16,150);

		/* Draw regular line */

		/* Left border */
FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"TMS9928AScanline_INT bef bord           ",16,150);

//		for ( INT32 i = 0; i < TMS9928A_HORZ_DISPLAY_START; i++ )
//			p[i] = BackColour;

FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"TMS9928AScanline_INT aft bord           ",16,150);

		/* Active display */

		switch( tms.mode )
		{
		case 0:             /* MODE 0 */
			{
FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 0 not supported    ",16,150);
				UINT16 addr = tms.nametbl + ( ( y & 0xF8 ) << 2 );

				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START; x < TMS9928A_HORZ_DISPLAY_START + 256; x+= 8, addr++ )
				{
					UINT8 charcode = readvmem( addr );
					UINT8 pattern =  readvmem( tms.pattern + ( charcode << 3 ) + ( y & 7 ) );
					UINT8 colour =  readvmem( tms.colour + ( charcode >> 3 ) );
					UINT16 fg = (colour >> 4) ? (colour >> 4) : BackColour;
					UINT16 bg = (colour & 15) ? (colour & 15) : BackColour;

					for ( INT32 i = 0; i < 8; pattern <<= 1, i++ )
						p[x+i] = ( pattern & 0x80 ) ? fg : bg;
				}
			}
			break;

		case 1:             /* MODE 1 */
			{
FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 1 not supported    ",16,150);
				UINT16 addr = tms.nametbl + ( ( y >> 3 ) * 40 );
				UINT16 fg = (tms.Regs[7] >> 4) ? (tms.Regs[7] >> 4) : BackColour;
				UINT16 bg = BackColour;

				/* Extra 6 pixels left border */
				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START; x < TMS9928A_HORZ_DISPLAY_START + 6; x++ )
					p[x] = bg;

				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START + 6; x < TMS9928A_HORZ_DISPLAY_START + 246; x+= 6, addr++ )
				{
					UINT16 charcode =  readvmem( addr );
					UINT8 pattern =  readvmem( tms.pattern + ( charcode << 3 ) + ( y & 7 ) );

					for ( INT32 i = 0; i < 6; pattern <<= 1, i++ )
						p[x+i] = ( pattern & 0x80 ) ? fg : bg;
				}

				/* Extra 10 pixels right border */
				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START + 246; x < TMS9928A_HORZ_DISPLAY_START + 256; x++ )
					p[x] = bg;
			}
			break;

		case 2:             /* MODE 2 */
			{
FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 2 not supported    ",16,150);
				UINT16 addr = tms.nametbl + ( ( y >> 3 ) * 32 );

				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START; x < TMS9928A_HORZ_DISPLAY_START + 256; x+= 8, addr++ )
				{
					UINT16 charcode =  readvmem( addr ) + ( ( y >> 6 ) << 8 );
					UINT8 pattern =  readvmem( tms.pattern + ( ( charcode & tms.patternmask ) << 3 ) + ( y & 7 ) );
					UINT8 colour =  readvmem( tms.colour + ( ( charcode & tms.colourmask ) << 3 ) + ( y & 7 ) );
					UINT16 fg = (colour >> 4) ? (colour >> 4) : BackColour;
					UINT16 bg = (colour & 15) ? (colour & 15) : BackColour;

					for ( INT32 i = 0; i < 8; pattern <<= 1, i++ )
						p[x+i] = ( pattern & 0x80 ) ? fg : bg;
				}
			}
			break;

		case 3:             /* MODE 1+2 */
			{
FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 1+2 not supported    ",16,150);
				UINT16 addr = tms.nametbl + ( ( y >> 3 ) * 40 );
				UINT16 fg = (tms.Regs[7] >> 4) ? (tms.Regs[7] >> 4) : BackColour;
				UINT16 bg = BackColour;

				/* Extra 6 pixels left border */
				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START; x < TMS9928A_HORZ_DISPLAY_START + 6; x++ )
					p[x] = bg;

				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START + 6; x < TMS9928A_HORZ_DISPLAY_START + 246; x+= 6, addr++ )
				{
					UINT16 charcode = (  readvmem( addr ) + ( ( y >> 6 ) << 8 ) ) & tms.patternmask;
					UINT8 pattern = readvmem( tms.pattern + ( charcode << 3 ) + ( y & 7 ) );

					for ( INT32 i = 0; i < 6; pattern <<= 1, i++ )
						p[x+i] = ( pattern & 0x80 ) ? fg : bg;
				}

				/* Extra 10 pixels right border */
				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START + 246; x < TMS9928A_HORZ_DISPLAY_START + 256; x++ )
					p[x] = bg;
			}
			break;

		case 4:             /* MODE 3 */
			{
FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 3 not supported    ",16,150);
				UINT16 addr = tms.nametbl + ( ( y >> 3 ) * 32 );

				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START; x < TMS9928A_HORZ_DISPLAY_START + 256; x+= 8, addr++ )
				{
					UINT8 charcode =  readvmem( addr );
					UINT8 colour =  readvmem( tms.pattern + ( charcode << 3 ) + ( ( y >> 2 ) & 7 ) );
					UINT16 fg = (colour >> 4) ? (colour >> 4) : BackColour;
					UINT16 bg = (colour & 15) ? (colour & 15) : BackColour;

					p[x+0] = p[x+1] = p[x+2] = p[x+3] = fg;
					p[x+4] = p[x+5] = p[x+6] = p[x+7] = bg;
				}
			}
			break;

		case 5: case 7:     /* MODE bogus */
			{
FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode bogus not supported    ",16,150);
				UINT16 fg = (tms.Regs[7] >> 4) ? (tms.Regs[7] >> 4) : BackColour;
				UINT16 bg = BackColour;

				/* Extra 6 pixels left border */
				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START; x < TMS9928A_HORZ_DISPLAY_START + 6; x++ )
					p[x] = bg;

				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START + 6; x < TMS9928A_HORZ_DISPLAY_START + 246; x+= 6 )
				{
					p[x+0] = p[x+1] = p[x+2] = p[x+3] = fg;
					p[x+4] = p[x+5] = bg;
				}

				/* Extra 10 pixels right border */
				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START + 246; x < TMS9928A_HORZ_DISPLAY_START + 256; x++ )
					p[x] = bg;
			}
			break;

		case 6:             /* MODE 2+3 */
			{
FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 23 not supported    ",16,150);
				UINT16 addr = tms.nametbl + ( ( y >> 3 ) * 32 );

				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START; x < TMS9928A_HORZ_DISPLAY_START + 256; x+= 8, addr++ )
				{
					UINT8 charcode =  readvmem( addr );
					UINT8 colour =  readvmem( tms.pattern + ( ( ( charcode + ( ( y >> 2 ) & 7 ) + ( ( y >> 6 ) << 8 ) ) & tms.patternmask ) << 3 ) );
					UINT16 fg = (colour >> 4) ? (colour >> 4) : BackColour;
					UINT16 bg = (colour & 15) ? (colour & 15) : BackColour;

					p[x+0] = p[x+1] = p[x+2] = p[x+3] = fg;
					p[x+4] = p[x+5] = p[x+6] = p[x+7] = bg;
				}
			}
			break;
		}

		/* Draw sprites */
		if ( ( tms.Regs[1] & 0x50 ) != 0x40 )
		{
			/* sprites are disabled */
			tms.FifthSprite = 31;
		}
		else
		{
			UINT8 sprite_size = ( tms.Regs[1] & 0x02 ) ? 16 : 8;
			UINT8 sprite_mag = tms.Regs[1] & 0x01;
			UINT8 sprite_height = sprite_size * ( sprite_mag + 1 );
			UINT8 spr_drawn[32+256+32] = { 0 };
			UINT8 num_sprites = 0;
			UINT8 fifth_encountered = 0;

			for ( UINT16 sprattr = 0; sprattr < 128; sprattr += 4 )
			{
				INT32 spr_y =  readvmem( tms.spriteattribute + sprattr + 0 );

				tms.FifthSprite = sprattr / 4;

				/* Stop processing sprites */
				if ( spr_y == 208 )
					break;

				if ( spr_y > 0xE0 )
					spr_y -= 256;

				/* vert pos 255 is displayed on the first line of the screen */
				spr_y++;

				/* is sprite enabled on this line? */
				if ( spr_y <= y && y < spr_y + sprite_height )
				{
					INT32 spr_x =  readvmem( tms.spriteattribute + sprattr + 1 );
					UINT8 sprcode =  readvmem( tms.spriteattribute + sprattr + 2 );
					UINT8 sprcol =  readvmem( tms.spriteattribute + sprattr + 3 );
					UINT16 pataddr = tms.spritepattern + ( ( sprite_size == 16 ) ? sprcode & ~0x03 : sprcode ) * 8;

					num_sprites++;

					/* Fifth sprite encountered? */
					if ( num_sprites == 5 )
					{
						fifth_encountered = 1;
						break;
					}

					if ( sprite_mag )
						pataddr += ( ( ( y - spr_y ) & 0x1F ) >> 1 );
					else
						pataddr += ( ( y - spr_y ) & 0x0F );

					UINT8 pattern =  readvmem( pataddr );

					if ( sprcol & 0x80 )
						spr_x -= 32;

					sprcol &= 0x0f;

					for ( INT32 s = 0; s < sprite_size; s += 8 )
					{
						for ( INT32 i = 0; i < 8; pattern <<= 1, i++ )
						{
							INT32 colission_index = spr_x + ( sprite_mag ? i * 2 : i ) + 32;

							for ( INT32 z = 0; z <= sprite_mag; colission_index++, z++ )
							{
								/* Check if pixel should be drawn */
								if ( pattern & 0x80 )
								{
									if ( colission_index >= 32 && colission_index < 32 + 256 )
									{
										/* Check for colission */
										if ( spr_drawn[ colission_index ] )
											tms.StatusReg |= 0x20;
										spr_drawn[ colission_index ] |= 0x01;

										if ( sprcol )
										{
											/* Has another sprite already drawn here? */
											if ( ! ( spr_drawn[ colission_index ] & 0x02 ) )
											{
												spr_drawn[ colission_index ] |= 0x02;
												p[ TMS9928A_HORZ_DISPLAY_START + colission_index - 32 ] = sprcol;
											}
										}
									}
								}
							}
						}

						pattern =  readvmem( pataddr + 16 );
						spr_x += sprite_mag ? 16 : 8;
					}
				}
			}

			/* Update sprite overflow bits */
			if (~tms.StatusReg & 0x40)
			{
				tms.StatusReg = (tms.StatusReg & 0xe0) | tms.FifthSprite;
				if (fifth_encountered && ~tms.StatusReg & 0x80)
					tms.StatusReg |= 0x40;
			}
		}

		/* Right border */
		for ( INT32 i = TMS9928A_HORZ_DISPLAY_START + 256; i < TMS9928A_TOTAL_HORZ; i++ )
			p[i] = BackColour;
	}
}

void TMS9928AScanline(INT32 vpos)
{
	if (vpos==0) // draw the border on the first scanline, border shouldn't use any cpu
	{            // to render.  this keeps cv defender's radar working.
		for (INT32 i = 0; i < tms.top_border+1; i++)
		{
			TMS9928AScanline_INT(i);
		}
	} else {
		TMS9928AScanline_INT(vpos + tms.top_border);
	}
}

INT32 TMS9928ADrawMSX()
{
//	TMS89928aPaletteRecalc();
	UINT16 BackColour = tms.Regs[7] & 0xf;
	*(unsigned short *)0x25E00000=colAddr[BackColour];
/*
	{
		for (INT32 y = 0; y < nScreenHeight; y++)
		{
			for (INT32 x = 0; x < nScreenWidth; x++)
			{
				pTransDraw[y * nScreenWidth + x] = tms.tmpbmp[y * TMS9928A_TOTAL_HORZ + ((TMS9928A_HORZ_DISPLAY_START/2)+10)+x];
			}
		}
	}

	BurnTransferCopy(Palette);
*/
	return 0;
}

INT32 TMS9928ADraw()
{
	int BackColour = tms.Regs[7] & 15;

	if (!BackColour) BackColour=1;

	*(unsigned short *)0x25E00000=colAddr[BackColour];

	if ((tms.Regs[1] & 0x40))
	{
		int mode = ((((tms.model == TMS99x8A) || (tms.model == TMS9929A)) ? (tms.Regs[0] & 2) : 0) | ((tms.Regs[1] & 0x10)>>4) | ((tms.Regs[1] & 8)>>1));

		(*ModeHandlers[mode])(tms.tmpbmp,&tms.vMem[tms.nametbl]);

		if ((tms.Regs[1] & 0x50) == 0x40)
		{
			unsigned int size = (tms.Regs[1] & 2) ? 16 : 8;
			unsigned int large = (int)(tms.Regs[1] & 1);

			if(!large)
					draw_sprites((unsigned char *)tms.vMem + tms.spriteattribute,(unsigned char *)tms.vMem + tms.spritepattern,&ss_sprite[4],size);
			else
					draw_sprites_large((unsigned char *)tms.vMem + tms.spriteattribute,(unsigned char *)tms.vMem + tms.spritepattern,&ss_sprite[4],size);
		}
	}

//	BurnTransferCopy(Palette);

	return 0;
}

int TMS9928AInterrupt()
{
	int  b = (tms.Regs[1] & 0x20) != 0;

	tms.StatusReg |= 0x80;

	if (b != tms.INT) {
		tms.INT = b;
		if (tms.INTCallback) tms.INTCallback (tms.INT);
	}

	return b;
}
