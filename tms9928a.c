//#include "tiles_generic.h"
#include "tms9928a.h"
#define		_SPR3_
#include "sega_spr.h"
#define COLADDR (FBUF_ADDR-0x400)
#define COLADDR_SPR	(COLADDR>>3)
#define nScreenWidth 256
#define nScreenHeight 192
#define	RGB( r, g, b )		(0x8000U|((b) << 10)|((g) << 5 )|(r))

//char pattern_lut[256][8];
//extern unsigned char *pTransDraw;
extern unsigned short *colAddr;
extern unsigned short  *ss_map;
extern unsigned int *shared;
#define SS_SPRAM *(&shared + 5)
#define SS_FONT	 *(&shared + 3)
extern SprSpCmd *ss_sprite;

//unsigned int colour_dirty	 =0xffff;
//unsigned int pattern_dirty =0xffff;
unsigned char dirty[24*32*8*4];
unsigned char dirtyc[24*32*8*4];
//unsigned int dirtys[32];

//unsigned int pattern6[0x400];
//unsigned int pattern4[0x400];
static int TMS9928A_palette[16] = {
	0x000000, 0x000000, 0x21c842, 0x5edc78, 0x5455ed, 0x7d76fc, 0xd4524d, 0x42ebf5,
	0xfc5554, 0xff7978, 0xd4c154, 0xe6ce80, 0x21b03b, 0xc95bba, 0xcccccc, 0xffffff
};

typedef struct {
	unsigned char ReadAhead;
	unsigned char Regs[8];
	unsigned char StatusReg;
	unsigned char FirstByte;
	unsigned char latch;
	unsigned char INT;
	int Addr;
	int colour;
	int pattern;
	int nametbl;
	int spriteattribute;
	int spritepattern;
	int colourmask;
	int patternmask;

//	unsigned char *vMem;
	unsigned char vMem[0x4000];
//	unsigned char *dBackMem;
//	unsigned short *tmpbmp;
	unsigned char *tmpbmp;
	int vramsize;
	int model;

	int max_x;
	int min_x;
	int max_y;
	int min_y;

	int LimitSprites;
	int top_border;
	int bottom_border;

	void (*INTCallback)(int);
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

static void change_register(int reg, unsigned char val)
{
	static const unsigned char Mask[8] = { 0x03, 0xfb, 0x0f, 0xff, 0x07, 0x7f, 0x07, 0xff };

	val &= Mask[reg];

	if(tms.Regs[reg] != val)
	{
		tms.Regs[reg] = val;

		switch (reg)
		{
			case 0:
			{
				if (val & 2) {
					tms.colour = ((tms.Regs[3] & 0x80) * 64) & (tms.vramsize - 1);
					tms.colourmask = (tms.Regs[3] & 0x7f) * 8 | 7;
					tms.pattern = ((tms.Regs[4] & 4) * 2048) & (tms.vramsize - 1);
					tms.patternmask = (tms.Regs[4] & 3) * 256 | (tms.colourmask & 255);

				} else {
					tms.colour = (tms.Regs[3] * 64) & (tms.vramsize - 1);
					tms.pattern = (tms.Regs[4] * 2048) & (tms.vramsize - 1);
				}
			}
			break;

			case 1:
			{
					int b = (val & 0x20) && (tms.StatusReg & 0x80);
					if (b != tms.INT) {
						tms.INT = b;
						if (tms.INTCallback) tms.INTCallback (tms.INT);
					}
			}
			break;

			case 2:
				tms.nametbl = (val * 1024) & (tms.vramsize - 1);
								
			break;

			case 3:
			{
				if (tms.Regs[0] & 2) {
					tms.colour = ((val & 0x80) * 64) & (tms.vramsize - 1);
					tms.colourmask = (val & 0x7f) * 8 | 7;
				} else {
					tms.colour = (val * 64) & (tms.vramsize - 1);
				}
				tms.patternmask = (tms.Regs[4] & 3) * 256 | (tms.colourmask & 255);
			}
			break;

			case 4:
			{
				if (tms.Regs[0] & 2) {
					tms.pattern = ((val & 4) * 2048) & (tms.vramsize - 1);
					tms.patternmask = (val & 3) * 256 | 255;
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
}

void TMS9928AReset()
{
	for (int i = 0; i < 8; i++)
		tms.Regs[i] = 0;

	tms.StatusReg = 0;
	tms.nametbl = tms.pattern = tms.colour = 0;
	tms.spritepattern = tms.spriteattribute = 0;
	tms.colourmask = tms.patternmask = 0;
	tms.Addr = tms.ReadAhead = tms.INT = 0;
	tms.FirstByte = 0;
	tms.latch = 0;
}

void TMS9928AInit(int model, int vram, int borderx, int bordery, void (*INTCallback)(int))
{
//	GenericTilesInit();

	tms.model = model;

	tms.INTCallback = INTCallback;

	tms.top_border		= ((tms.model == TMS9929) || (tms.model == TMS9929A)) ? 51 : 27;
	tms.bottom_border	= ((tms.model == TMS9929) || (tms.model == TMS9929A)) ? 51 : 24;

#define MIN(x,y) ((x)<(y)?(x):(y))

	tms.min_x = 15 - MIN(borderx, 15);
	tms.max_x = 15 + 32*8 - 1 + MIN(borderx, 15);
	tms.min_y = tms.top_border - MIN(bordery, tms.top_border);
	tms.max_y = tms.top_border + 24*8 - 1 + MIN(bordery, tms.bottom_border);

#undef MIN

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
/*
                pattern = *patternptr++;
                colour = *colourptr++;
                fg = colour / 16;
                bg = colour & 15;

			   unsigned int color1,color2;
			   char *vbt = &bitmap[(y * 8 + yy) * 128 + (x*4)];
*/				/*
				for (int i=0; i<256; i++)
				{
					for (int j=0; j<8; j++)
					{
						pattern_lut[i][j]=(i*j*2)&0x80;
					}
				} */
					/*
				for (int i=0; i<0x400; i++)
					pattern6[i]=(i>>6)&3;
				for (int i=0; i<0x400; i++)
					pattern4[i]=(i>>4)&3;		  */
/*
			    for (xx=0;xx<4;xx++) 
				{
					color1=(pattern & 0x80) ? fg : bg;
					pattern *= 2;
					color2=(pattern & 0x80) ? fg : bg;
					pattern *= 2;
*/






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
	for (int i = 0; i < 8; i++)
		change_register(i, tms.Regs[i]);

	if (tms.INTCallback) tms.INTCallback(tms.INT);
}

unsigned char TMS9928AReadVRAM()
{
	int b = tms.ReadAhead;
	tms.ReadAhead = tms.vMem[tms.Addr];
	tms.Addr = (tms.Addr + 1) & (tms.vramsize - 1);
	tms.latch = 0;
	return b;
}

void TMS9928AWriteVRAM(int data)
{
	tms.vMem[tms.Addr] = data;
	tms.Addr = (tms.Addr + 1) & (tms.vramsize - 1);
	tms.ReadAhead = data;
	tms.latch = 0;
}

unsigned char TMS9928AReadRegs()
{
	int b = tms.StatusReg;
	tms.StatusReg = 0x1f;
	if (tms.INT) {
		tms.INT = 0;
		if (tms.INTCallback) tms.INTCallback (tms.INT);
	}
	tms.latch = 0;
	return b;
}

void TMS9928AWriteRegs(int data)
{
	if (tms.latch) {
		if (data & 0x80) {
			change_register(data & 0x07, tms.FirstByte);
		} else {
			tms.Addr = ((unsigned short)data << 8 | tms.FirstByte) & (tms.vramsize - 1);
			if (!(data & 0x40)) {
				TMS9928AReadVRAM();
			}
		}

		tms.latch = 0;
	} else {
		tms.FirstByte = data;
		tms.latch = 1;
	}
}

void TMS9928ASetSpriteslimit(int limit)
{
	tms.LimitSprites = limit;
}

static void draw_mode0(unsigned short *bitmap)
{
	unsigned char *patternptr;
	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 0 not supported    ",16,40);
	 return;

	for (int y = 0, name = 0; y < 24; y++)
	{
		for (int x = 0; x < 32; x++, name++)
		{
			int charcode = tms.vMem[tms.nametbl+name];

			patternptr = tms.vMem + tms.pattern + charcode*8;

			int color = tms.vMem[tms.colour+charcode/8];

			int fg = color >> 4;
			int bg = color & 0x0f;

			for (int yy = 0; yy < 8; yy++)
			{
				int bits = *patternptr++;

				for (int xx = 0; xx < 8; xx++, bits<<=1)
				{
					bitmap[(y * 8 + yy) * 256 + (x * 8 + xx)] = (bits & 0x80) ? fg : bg;
				}
			}
		}
	}
}

static void draw_mode1(unsigned short *bitmap)
{
	int pattern,x,y,yy,xx,name,charcode;
	unsigned char fg,bg,*patternptr;
	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 1 not supported    ",16,40);
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

static void draw_mode12(unsigned short *bitmap)
{
	int pattern,x,y,yy,xx,name,charcode;
	unsigned char fg,bg,*patternptr;
	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 12 not supported    ",16,40);
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
    unsigned int fg,bg;
    unsigned char *colourptr,*patternptr;
	unsigned char tab[4];
	unsigned char *vbt,*dirtyptr,*dirtycptr;

	dirtyptr=(unsigned char *)dirty;
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
}

static void draw_mode3(unsigned short *bitmap)
{
    int x,y,yy,yyy,name,charcode;
    unsigned char fg,bg,*patternptr;

	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 3 not supported    ",16,40);
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

static void draw_mode23(unsigned short *bitmap)
{
    int x,y,yy,yyy,name,charcode;
    unsigned char fg,bg,*patternptr;

	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 23 not supported    ",16,40);
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

static void draw_modebogus(unsigned short *bitmap)
{
    unsigned char fg,bg;
    int x,y,n,xx;

	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode bogus not supported    ",16,40);
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
//	unsigned int *dirtysptr=(unsigned int *)dirtys;

    for (unsigned char p=0;p<32;p++) 
	{
        y = *attributeptr++;
		if (y == 208) 
		{
			for (;p<32;++p) 
			{
			ss_spritePtr->charSize   = 0;
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
					vbt[4]=tab[(line>>6)&3];
					vbt[5]=tab[(line>>4)&3];
					vbt[6]=tab[(line>>2)&3];
					vbt[7]=tab[(line>>0)&3];
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


int TMS9928ADraw()
{
	int BackColour = tms.Regs[7] & 15;

	if (!BackColour) BackColour=1;

//	TMS89928aPaletteRecalc();
	colAddr[0] = colAddr[BackColour];
  
	if (!(tms.Regs[1] & 0x40))
	{
	/*	for (int i = 0; i < nScreenWidth * nScreenHeight; i++) 
		{
			pTransDraw[i] = BackColour;
		}			*/
	}
	else
	{
		int mode = ((((tms.model == TMS99x8A) || (tms.model == TMS9929A)) ? (tms.Regs[0] & 2) : 0) | ((tms.Regs[1] & 0x10)>>4) | ((tms.Regs[1] & 8)>>1));

		(*ModeHandlers[mode])(tms.tmpbmp,&tms.vMem[tms.nametbl]);
	  /*
		{
			for (int y = 0; y < 192; y++)
			{
				for (int x = 0; x < 256; x++)
				{
					pTransDraw[(y + tms.top_border) * nScreenWidth + x + 15] = tms.tmpbmp[y * 256 + x];
				}
			}
		}

		{
			for (int y = 0; y < tms.top_border; y++) { // top
				for (int x = 0; x < 15 + 256 + 15; x++) {
					pTransDraw[y * nScreenWidth + x] = BackColour;
				}
			}

			for (int y = tms.top_border+192; y < tms.top_border+192+tms.bottom_border; y++) { // bottom
				for (int x = 0; x < 15 + 256 + 15; x++) {
					pTransDraw[y * nScreenWidth + x] = BackColour;
				}
			}


			for (int y = tms.top_border; y < tms.top_border+192; y++) { // left
				for (int x = 0; x < 15; x++) {
					pTransDraw[y * nScreenWidth + x] = BackColour;
				}
			}

			for (int y = tms.top_border; y < tms.top_border+192; y++) { // right
				for (int x = 15+256; x < 15 + 256 + 15; x++) {
					pTransDraw[y * nScreenWidth + x] = BackColour;
				}
			}
		}
	*/
		if ((tms.Regs[1] & 0x50) == 0x40)
		{
			unsigned int size = (tms.Regs[1] & 2) ? 16 : 8;
			draw_sprites((unsigned char *)tms.vMem + tms.spriteattribute,(unsigned char *)tms.vMem + tms.spritepattern,&ss_sprite[4],size);
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
