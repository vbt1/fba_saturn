#include "tms9928a.h"
#include "burn.h"
#include <string.h>
extern UINT32 *shared;
//extern UINT8 *TMSContext ;
#define SS_SPRAM *(&shared + 5)
#define SS_FONT	 *(&shared + 3)

int TMS9928A_palette[16] = {
	0x000000, 0x000000, 0x21c842, 0x5edc78, 0x5455ed, 0x7d76fc, 0xd4524d, 0x42ebf5,
	0xfc5554, 0xff7978, 0xd4c154, 0xe6ce80, 0x21b03b, 0xc95bba, 0xcccccc, 0xffffff
};

static void TMS89928aPaletteRecalc()
{
	for (unsigned int i = 0; i < 16; i++) 
	{
		colAddr[i] = RGB((TMS9928A_palette[i] >> 19)&31,(TMS9928A_palette[i] >> 11)&31,(TMS9928A_palette[i] >>3)&31);
	}
}

static void check_interrupt()
{
	UINT32 b = (tms.StatusReg & 0x80 && tms.Regs[1] & 0x20) ? 1 : 0;
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
				tms.colour = ((tms.Regs[3] & 0x80) * 64) & (tms.vramsize);
				tms.pattern = ((tms.Regs[4] & 4) * 2048) & (tms.vramsize);
				update_table_masks();
			} else {
				tms.colour = (tms.Regs[3] * 64) & (tms.vramsize);
				tms.pattern = (tms.Regs[4] * 2048) & (tms.vramsize);
			}
			tms.mode = ( (tms.revA ? (tms.Regs[0] & 2) : 0) | ((tms.Regs[1] & 0x10)>>4) | ((tms.Regs[1] & 8)>>1));
		}
		break;

		case 1:
		{
			tms.mode = ( (tms.revA ? (tms.Regs[0] & 2) : 0) | ((tms.Regs[1] & 0x10)>>4) | ((tms.Regs[1] & 8)>>1));
			check_interrupt();
		}
		break;

		case 2:
			tms.nametbl = (val * 1024) & (tms.vramsize);
		break;

		case 3:
		{
			if (tms.Regs[0] & 2) {
				tms.colour = ((val & 0x80) * 64) & (tms.vramsize);
				update_table_masks();
			} else {
				tms.colour = (val * 64) & (tms.vramsize);
			}
		}
		break;

		case 4:
		{
			if (tms.Regs[0] & 2) {
				tms.pattern = ((val & 4) * 2048) & (tms.vramsize);
				update_table_masks();
			} else {
				tms.pattern = (val * 2048) & (tms.vramsize);
			}
		}
		break;

		case 5:
			tms.spriteattribute = (val * 128) & (tms.vramsize);
		break;

		case 6:
			tms.spritepattern = (val * 2048) & (tms.vramsize);
		break;

		case 7:
			*(unsigned short *)0x25E00000=colAddr[val & 15];
			/* The backdrop is updated at TMS9928A_refresh() */
		break;
	}
}

void TMS9928AReset()
{
	for (UINT32 i = 0; i < 8; i++)
		tms.Regs[i] = 0;

	memset((void *)tms.vMem, 0, tms.vramsize+1);

	memset((void *)tms.tmpbmp, 0, 256 * 192 * sizeof(char));
	memset((void *)tms.dirty,0xFF,192*128);

	tms.StatusReg = 0;
	tms.FifthSprite = 31;
	tms.nametbl = tms.pattern = tms.colour = 0;
	tms.spritepattern = tms.spriteattribute = 0;
	tms.colourmask = tms.patternmask = 0x3fff;
	tms.Addr = tms.ReadAhead = tms.INT = 0;
	tms.FirstByte = 0;
	tms.latch = 0;
	tms.mode = 0;
}

#ifdef USE_LUT
static void make_lut()
{
	unsigned char bg,fg;

	for (int bg=0;bg<16;bg++)
	{
		for (int fg=0;fg<16;fg++)
		{
			unsigned int *position =	 &tms.color_2bpp_lut[(bg|fg<<4)*4];
			position[0] = bg|bg<<4;
			position[1] = fg|bg<<4;
			position[2] = bg|fg<<4;
			position[3] = fg|fg<<4;
		}
	}
}
#endif

//extern UINT8 *tmpbmp;
void TMS9928AInit(INT32 model, INT32 vram, INT32 borderx, INT32 bordery, void (*INTCallback)(int), UINT8 *TMSContext)
{
//	GenericTilesInit();
	memset(&tms, 0, sizeof(tms));
	tms.model = model;
	tms.revA = 1;

	tms.INTCallback = INTCallback;

	tms.top_border		= TMS9928A_VERT_DISPLAY_START_NTSC;
	tms.bottom_border	= ((tms.model == TMS9929) || (tms.model == TMS9929A)) ? 51 : 24;
	tms.vertical_size   = TMS9928A_TOTAL_VERT_NTSC;

	tms.vramsize = vram -1;
	unsigned char *ss_vram = (UINT8 *)SS_SPRAM;
	tms.tmpbmp			 = (ss_vram+0x1100+0x10000);
	tms.vMem				 = (UINT8 *)TMSContext;
	tms.dirty				 = (UINT8 *)TMSContext+0x4000;
	tms.color_2bpp_lut	 = (UINT32 *)(TMSContext+0xA000);

#ifdef USE_LUT
	make_lut();
#endif
	TMS89928aPaletteRecalc();
	TMS9928AReset ();
	tms.LimitSprites = 1;
}

void TMS9928AExit()
{
	TMS9928AReset();
//	memset (tms.dirty,0x00,24*32*8*4);
//	memset (tms.color_2bpp_lut,0x00,0x400*sizeof(int));
	tms.tmpbmp			 = NULL;
	tms.vMem				 = NULL;
	tms.dirty				 = NULL;
	tms.color_2bpp_lut	 = NULL;
	tms.INTCallback	 = NULL;
}

void TMS9928APostLoad()
{
	for (INT32 i = 0; i < 8; i++)
		change_register(i, tms.Regs[i]);

	if (tms.INTCallback) tms.INTCallback(tms.INT);
}

UINT8 SG_TMS9928AReadVRAM()
{
	UINT32 b = tms.ReadAhead;
	tms.ReadAhead = tms.vMem[tms.Addr];
	tms.Addr = (tms.Addr + 1) & (tms.vramsize);
	tms.latch = 0;
	return b;
}

/*static*/ UINT8 TMS9928AReadVRAM(TMS9928A *tms)
{
	UINT32 b = tms->ReadAhead;
	tms->ReadAhead = tms->vMem[tms->Addr];
	tms->Addr = (tms->Addr + 1) & (tms->vramsize);
	tms->latch = 0;
	return b;
}

void TMS9928AWriteVRAM(INT32 data)
{
	tms.vMem[tms.Addr] = data;
	tms.Addr = (tms.Addr + 1) & (tms.vramsize);
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
		tms.Addr = ((UINT16)data << 8 | (tms.Addr & 0xff)) & (tms.vramsize);
		if (data & 0x80) {
			change_register(data & 0x07, tms.FirstByte);
		} else {
			if (!(data & 0x40)) {
				TMS9928AReadVRAM(&tms);
			}
		}

		tms.latch = 0;
	} else {
		/* set low part of read/write address */
		tms.Addr = ((tms.Addr & 0xff00) | data) & (tms.vramsize);
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

static void draw_mode0(unsigned char *bitmap,unsigned char *vmem, TMS9928A *tmstmp, unsigned int *v)
{
//	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 0     ",16,150);
	unsigned char *patternptr;
	unsigned int tab[4];
	unsigned char *vbt;
	unsigned int *dirtyptr;

	dirtyptr=(unsigned int *)tms.dirty;

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

				if(*dirtyptr!=(pattern | colour << 8))
				{
					*dirtyptr=(pattern | colour << 8);

					vbt = (unsigned char *)&bitmap[(y * 8 + yy) * 128 + (x*4)];

					unsigned int bg = colour & 0x0f;
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
			}
		}
	}
}

static void draw_mode1(unsigned char *bitmap,unsigned char *vmem, TMS9928A *tmstmp, unsigned int *v)
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

static void draw_mode12(unsigned char *bitmap,unsigned char *vmem, TMS9928A *tmstmp, unsigned int *v)
{
	int pattern,x,y,yy,xx,name,charcode;
	unsigned char fg,bg,*patternptr;
//	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 12 not supported    ",16,150);
	 return;

	fg = tms.Regs[7] >> 4;
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
static void draw_mode2(unsigned short *bitmap,unsigned char *vmem, TMS9928A *tms, unsigned int *color_2bpp_lut)
{
//	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 2 supported    ",16,150);

	unsigned int *dirtyptr    = (unsigned int *)tms->dirty;

	for (unsigned int y=0;y<24;y++) 
	{
		unsigned int val = ((y /8)<<8);

		for (unsigned int x=0;x<64;x+=2) 
		{
			unsigned int charcode     = (*vmem++)+val;
			unsigned short pattern    = (charcode&tms->patternmask)*8;
			unsigned int colour     = (charcode&tms->colourmask)*8;
			unsigned short *vbt		  = bitmap + x;
			unsigned char *patternptr = tms->vMem+tms->pattern+colour;
			unsigned char *colourptr  = tms->vMem+tms->colour+pattern;

//			unsigned int *vbt = (unsigned int *)bitmap + x;

			for (unsigned int yy=0;yy<8;yy++) 
			{
				colour  = *colourptr++;
				pattern = *patternptr++;

				if(*dirtyptr!=(pattern | colour<<8))
				{
// MartinMan : maybe you can combine the two dirty memories into one, and then only do one comparison? (*dirtyboth != (pattern | colour << 8)) 
					*dirtyptr=(pattern | colour<<8);

					unsigned int *tab = &color_2bpp_lut[colour<<2];
//					*vbt = (tab[(pattern>>4)&3]|tab[(pattern>>6)&3]<<8)<<16|tab[(pattern)&3]|tab[(pattern>>2)&3]<<8;
					vbt[1] = tab[(pattern)&3]|tab[(pattern>>2)&3]<<8;
					pattern>>=4;
					vbt[0] = tab[(pattern)&3]|tab[(pattern>>2)&3]<<8;
				}				
				++dirtyptr;
				vbt+=64;
			}
        }
		bitmap+=512;
    }
}


static void draw_mode3(unsigned char *bitmap,unsigned char *vmem, TMS9928A *tmstmp, unsigned int *v)
{
//	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 3 supported    ",16,150);

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
                fg = (*patternptr >> 4);
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

static void draw_mode23(unsigned char *bitmap,unsigned char *vmem, TMS9928A *tmstmp, unsigned int *v)
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
                fg = (*patternptr >> 4);
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

static void draw_modebogus(unsigned char *bitmap,unsigned char *vmem, TMS9928A *tmstmp, unsigned int *v)
{
    unsigned char fg,bg;
    int x,y,n,xx;

//	 FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode bogus not supported    ",16,150);
	 return;

    fg = tms.Regs[7] >> 4;
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

static void (*const ModeHandlers[])(unsigned char *bitmap,unsigned char *vmem, TMS9928A *tms, unsigned int *color_lut) = {
        draw_mode0, draw_mode1, draw_mode2,  draw_mode12,
        draw_mode3, draw_modebogus, draw_mode23,
        draw_modebogus
};

static void draw_sprites(unsigned char *attributeptr,unsigned char *spritepatternptr, SprSpCmd *ss_spritePtr,unsigned int size)
{
	short x,y;
	tms.StatusReg = 0x80;

	unsigned char *ss_vram = (unsigned char *)SS_SPRAM+0x1100;

    for (unsigned char p=0;p<32;p++) 
	{
        y = *attributeptr++;
		if (y == 208) 
		{
			for (;p<32;++p) 
			{
			ss_spritePtr->charSize  = 0;
			ss_spritePtr->ax		= 0;
			ss_spritePtr->ay		= 0;
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
			ss_spritePtr->ax		= 0;
			ss_spritePtr->ay		= 0;
			++ss_spritePtr;
			attributeptr+=3;
			continue;
		}	  
		x = *attributeptr++;
        unsigned char *patternptr = spritepatternptr+ ((size == 16) ? *attributeptr & 0xfc : *attributeptr) * 8;
        ++attributeptr;

        unsigned char c = (*attributeptr& 0x0f);
        if (*attributeptr & 0x80) x -= 32;
        ++attributeptr;

		ss_spritePtr->ax				= x;
		ss_spritePtr->ay				= y;
//		if(*dirtysptr!=&patternptr[0])
		{
//			*dirtysptr = &patternptr[0];

			ss_spritePtr->charSize		= (size==8)? 0x108:0x210;
			ss_spritePtr->drawMode	= ( COLOR_0| ECD_DISABLE | COMPO_REP);
			ss_spritePtr->control		= 0;
			ss_spritePtr->charAddr		= 0x220+(p<<4);
			++ss_spritePtr;

			unsigned int *tab = &tms.color_2bpp_lut[c<<6];
			unsigned char *vbt = (unsigned char *)&ss_vram[(p<<7)];

			/* draw sprite (not enlarged) */
			for (unsigned char yy=0;yy<size;yy++) 
			{
				unsigned short line = (patternptr[0]<<8)| + patternptr[16];

				if(line!=0)
				{
					if(size>8)
					{
						vbt[4]=tab[(line>>6)&3];
						vbt[5]=tab[(line>>4)&3];
						vbt[6]=tab[(line>>2)&3];
						vbt[7]=tab[(line)&3];
					}
					line>>=8;
					vbt[2]=tab[(line>>2)&3];
					vbt[3]=tab[(line)&3];
					line>>=4;
					vbt[0]=tab[(line>>2)&3];
					vbt[1]=tab[(line)&3];

				}
				else
				{
					vbt[0]=vbt[1]=vbt[2]=vbt[3]=0;
					if(size>8)
					{
						vbt[4]=vbt[5]=vbt[6]=vbt[7]=0;
					}
				}
				vbt+=size>>1;
				++patternptr;
			} 
		}
	}
	tms.StatusReg |= 0x40;
}

static void draw_sprites_large(unsigned char *attributeptr,unsigned char *spritepatternptr, SprSpCmd *ss_spritePtr,unsigned int size)
{
	short x,y;
	tms.StatusReg = 0x80;

	unsigned char *ss_vram = (unsigned char *)SS_SPRAM+0x1100;

    for (unsigned char p=0;p<32;p++) 
	{
        y = *attributeptr++;
		if (y == 208) 
		{
			for (;p<32;++p) 
			{
			ss_spritePtr->charSize  = 0;
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
			ss_spritePtr->ax		= 0;
			ss_spritePtr->ay		= 0;
			++ss_spritePtr;
			attributeptr+=3;
			continue;
		}	  
		x = *attributeptr++;
        unsigned char *patternptr = spritepatternptr+ ((size == 16) ? *attributeptr & 0xfc : *attributeptr) * 8;
        ++attributeptr;
        unsigned char c = (*attributeptr& 0x0f);
        if (*attributeptr & 0x80) x -= 32;
        ++attributeptr;

		ss_spritePtr->ax				= x;
		ss_spritePtr->ay				= y;
		ss_spritePtr->cx				= x+(size<<1);
		ss_spritePtr->cy				= y+(size<<1);	 
		ss_spritePtr->control			= (JUMP_NEXT | ZOOM_NOPOINT | FUNC_SCALESP);
		{
//			*dirtysptr = &patternptr[0];

			ss_spritePtr->charSize		= (size==8)? 0x108:0x210;
			ss_spritePtr->drawMode	= ( COLOR_0| ECD_DISABLE | COMPO_REP);		
			ss_spritePtr->charAddr		= 0x220+(p<<4);
			++ss_spritePtr;

			unsigned int *tab = &tms.color_2bpp_lut[c<<6];
			unsigned char *vbt = (unsigned char *)&ss_vram[(p<<7)];

			/* draw sprite (enlarged) */
			for (unsigned char yy=0;yy<size;yy++) 
			{
				unsigned short line = (patternptr[0]<<8)| + patternptr[16];

				if(line!=0)
				{
					if(size>8)
					{
						vbt[4]=tab[(line>>6)&3];
						vbt[5]=tab[(line>>4)&3];
						vbt[6]=tab[(line>>2)&3];
						vbt[7]=tab[(line)&3];
					}
					line>>=8;
					vbt[2]=tab[(line>>2)&3];
					vbt[3]=tab[(line)&3];
					line>>=4;
					vbt[0]=tab[(line>>2)&3];
					vbt[1]=tab[(line)&3];
				}
				else
				{
					vbt[0]=vbt[1]=vbt[2]=vbt[3]=0;
					if(size>8)
					{
						vbt[4]=vbt[5]=vbt[6]=vbt[7]=0;
					}
				}
				vbt+=size>>1;
				++patternptr;
			} 
		}
	}
	tms.StatusReg |= 0x40;
}

static void TMS9928AScanline_INT(INT32 vpos)
{
	UINT16 BackColour = tms.Regs[7] & 0xf;
	INT32 y = vpos - tms.top_border;

	if ( y < 0 || y >= 192 || !(tms.Regs[1] & 0x40) )
	{
		/* Draw backdrop colour */
		/* vblank is set at the last cycle of the first inactive line */
		if ( y == 193 )
		{
			tms.StatusReg |= 0x80;
			check_interrupt();
		}
	}
	else
	{
		UINT8 *p = tms.tmpbmp +y*128;
		/* Draw regular line */
		/* Left border */
		/* Active display */

		switch( tms.mode )
		{
		case 0:             /* MODE 0 */
			{
//FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 0 not supported    ",16,190);

				UINT16 addr = tms.nametbl + ( ( y & 0xF8 ) << 2 );

//				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START; x < TMS9928A_HORZ_DISPLAY_START + 256; x+= 8, addr+=2 )
				for ( UINT32 x = 0; x < 32; x++, addr++, p+=4 )
				{
					UINT8 charcode = readvmem( addr );
					UINT8 pattern =  readvmem( tms.pattern + ( charcode * 8 ) + ( y & 7 ) );
					UINT8 colour =  readvmem( tms.colour + ( charcode / 8 ) );
					UINT32 bg = colour & 0x0f;
					UINT32 tab[4];

					tab[0]=bg|bg<<4;

					if(pattern!=0)
					{
						UINT32 fg = colour >> 4;
						tab[1] = fg|(*tab&0xf0);
						tab[2] = colour;
						tab[3] = fg|(colour&0xf0); 

						p[0] = tab[(pattern>>6)&3]; 
						p[1] = tab[(pattern>>4)&3]; 
						p[2] = tab[(pattern>>2)&3]; 
						p[3] = tab[(pattern>>0)&3];
					}
					else
						p[0] = p[1] = p[2] = p[3] = *tab;
				}
			}
			break;

		case 1:             /* MODE 1 */
			{
//FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 1 supported    ",16,190);

				UINT16 addr = tms.nametbl + ( ( y / 8 ) * 40 );
				UINT32 tab[4];

				tab[0] = 0;
				tab[1] = tms.Regs[7] >> 4;
				tab[2] = tms.Regs[7]&0xf0;
				tab[3] = tab[1]|tab[2]; 

				/* Extra 6 pixels left border */

				for ( UINT32 x = 0; x < 41; x++, addr++, p+=3 )
				{
					UINT16 charcode =  readvmem( addr );
					UINT8 pattern =  readvmem( tms.pattern + ( charcode * 8 ) + ( y & 7 ) );

					if(pattern!=0)
					{
						p[0] = tab[(pattern>>6)&3]; 
						p[1] = tab[(pattern>>4)&3]; 
						p[2] = tab[(pattern>>2)&3];
					}
					else
						p[0] = p[1] = p[2] = *tab;
				}
				/* Extra 10 pixels right border */
				p[0]=p[1]=p[2]=p[3]=p[4]=0;
			}
			break;

		case 2:             /* MODE 2 */
			{
//FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 2 supported    ",16,190);

				UINT16 addr = tms.nametbl + ( ( y / 8 ) * 32 );

//				for ( UINT32 x = TMS9928A_HORZ_DISPLAY_START; x < TMS9928A_HORZ_DISPLAY_START + 256; x+= 8, addr++ )
				for ( UINT32 x = 0; x < 32; x++, addr++, p+=4 )
				{
					UINT16 charcode =  readvmem( addr ) + ( ( y >> 6 ) << 8 );
					UINT8 pattern =  readvmem( tms.pattern + ( ( charcode & tms.patternmask ) * 8 ) + ( y & 7 ) );
					UINT32 colour =  readvmem( tms.colour + ( ( charcode & tms.colourmask ) * 8 ) + ( y & 7 ) );
					UINT32 fg = colour >> 4;
					UINT32 bg = colour & 15;
					UINT32 tab[4];

					tab[0]=bg|bg<<4;

					if(pattern!=0)
					{
						fg = colour >> 4;
						tab[1] = fg|(*tab&0xf0);
						tab[2] = colour;
						tab[3] = fg|(colour&0xf0);

						p[0] = tab[(pattern>>6)&3]; 
						p[1] = tab[(pattern>>4)&3]; 
						p[2] = tab[(pattern>>2)&3]; 
						p[3] = tab[(pattern>>0)&3];
					}
					else
					{
						p[0] = p[1] = p[2] = p[3] = *tab;
					}
				}
			}
			break;

		case 3:             /* MODE 1+2 */
			{
//FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 1+2 not supported    ",16,190);
				UINT16 addr = tms.nametbl + ( ( y / 8 ) * 40 );
				UINT16 fg = (tms.Regs[7] >> 4) ? (tms.Regs[7] >> 4) : BackColour;
				UINT16 bg = BackColour;

				/* Extra 6 pixels left border */
				for ( UINT32 x = TMS9928A_HORZ_DISPLAY_START; x < TMS9928A_HORZ_DISPLAY_START + 6; x++ )
					p[x] = bg;

				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START + 6; x < TMS9928A_HORZ_DISPLAY_START + 246; x+= 6, addr++ )
				{
					UINT16 charcode = (  readvmem( addr ) + ( ( y >> 6 ) << 8 ) ) & tms.patternmask;
					UINT8 pattern = readvmem( tms.pattern + ( charcode * 8 ) + ( y & 7 ) );

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
//FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 3 not supported    ",16,190);
				UINT16 addr = tms.nametbl + ( ( y / 8 ) * 32 );

//				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START; x < TMS9928A_HORZ_DISPLAY_START + 256; x+= 8, addr++ )
				for ( INT32 x = 0; x < 256; x++, addr++, p+=4 )
				{
					UINT8 charcode =  readvmem( addr );
					UINT8 colour =  readvmem( tms.pattern + ( charcode * 8 ) + ( ( y >> 2 ) & 7 ) );
//					UINT16 fg = (colour >> 4) ? (colour >> 4) : BackColour;
//					UINT16 bg = (colour & 15) ? (colour & 15) : BackColour;
					UINT32 fg = colour >> 4;
					UINT32 bg = colour & 15;
//					p[x+0] = p[x+1] = p[x+2] = p[x+3] = fg;
//					p[x+4] = p[x+5] = p[x+6] = p[x+7] = bg;
					p[0] = p[1] = fg | colour & 0xf0;
					p[2] = p[3] = bg| bg <<4;
				}
			}
			break;

		case 5: case 7:     /* MODE bogus */
			{
//FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode bogus not supported    ",16,190);
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
//FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Draw Mode 23 not supported    ",16,190);
				UINT16 addr = tms.nametbl + ( ( y / 8 ) * 32 );

				for ( INT32 x = TMS9928A_HORZ_DISPLAY_START; x < TMS9928A_HORZ_DISPLAY_START + 256; x+= 8, addr++ )
				{
					UINT8 charcode =  readvmem( addr );
					UINT8 colour =  readvmem( tms.pattern + ( ( ( charcode + ( ( y >> 2 ) & 7 ) + ( ( y >> 6 ) << 8 ) ) & tms.patternmask ) * 8 ) );
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
//		for ( INT32 i = TMS9928A_HORZ_DISPLAY_START + 256; i < TMS9928A_TOTAL_HORZ; i++ )
//			p[i] = BackColour;
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

void TMS9928ADraw()
{
	if ((tms.Regs[1] & 0x40))
	{
		unsigned int mode = ( (tms.Regs[0] & 2) | ((tms.Regs[1] & 0x10)>>4) | ((tms.Regs[1] & 8)>>1));

		(*ModeHandlers[mode])(tms.tmpbmp,&tms.vMem[tms.nametbl],&tms,tms.color_2bpp_lut);
//		draw_mode2(tms.tmpbmp,&tms.vMem[tms.nametbl],&tms,tms.color_2bpp_lut);

		if ((tms.Regs[1] & 0x50) == 0x40)
		{
			unsigned int size = (tms.Regs[1] & 2) ? 16 : 8;
			unsigned int large = (int)(tms.Regs[1] & 1);

			if(!large)
					draw_sprites((unsigned char *)tms.vMem + tms.spriteattribute,(unsigned char *)tms.vMem + tms.spritepattern,&ss_sprite[4],size);
			else
					draw_sprites_large((unsigned char *)tms.vMem + tms.spriteattribute,(unsigned char *)tms.vMem + tms.spritepattern,&ss_sprite[4],size);

// a enlever
//	tms.StatusReg |= 0x40;
		} 
	}
}

void TMS9928AInterrupt()
{
	int  b = (tms.Regs[1] & 0x20) != 0;

	tms.StatusReg |= 0x80;

	if (b != tms.INT) {
		tms.INT = b;
		if (tms.INTCallback) tms.INTCallback (tms.INT);
	}
}
