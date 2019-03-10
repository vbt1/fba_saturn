#include <stdarg.h>
#include "SEGA_DMA.H"
#include "SEGA_SPR.H"
#include    "machine.h"

struct SprSpCmd {                       /* Sprite Command Table             */
    Uint16  control;                    /* control word                     */
    Uint16  link;                       /* command link                     */
    Uint16  drawMode;                   /* draw mode                        */
    Uint16  color;                      /* color info.                      */
    Uint16  charAddr;                   /* character address                */
    Uint16  charSize;                   /* character size                   */
    Sint16  ax;                         /* point A x                        */
    Sint16  ay;                         /* point A y                        */
    Sint16  bx;                         /* point B x                        */
    Sint16  by;                         /* point B y                        */
    Sint16  cx;                         /* point C x                        */
    Sint16  cy;                         /* point C y                        */
    Sint16  dx;                         /* point D x                        */
    Sint16  dy;                         /* point D y                        */
    Uint16  grshAddr;                   /* gouraud shading table address    */
    Uint16  dummy;                      /* dummy area                       */
};
typedef struct SprSpCmd     SprSpCmd;
extern unsigned int nBurnSprites;
extern SprSpCmd *ss_sprite;

#define TVSTAT      (*(volatile unsigned short *)0x25F80004)
#define	RGB( r, g, b )		(0x8000U|((b) << 10)|((g) << 5 )|(r))
//-------------------------------------------------------------------------------------------------------------------------------------
void cleanSprites()
{
	for (unsigned int delta=3; delta<nBurnSprites; delta++)
	{
		ss_sprite[delta].ax   = -16;
		ss_sprite[delta].ay   = -16;
	} 
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*inline*/ /*static*/ int readbit(const unsigned char *src, int bitnum)
{
	return src[bitnum / 8] & (0x80 >> (bitnum % 8));
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void GfxDecode4Bpp(unsigned int num, unsigned int numPlanes, unsigned int xSize, unsigned int ySize, int planeoffsets[], int xoffsets[], int yoffsets[], int modulo, unsigned char *pSrc, unsigned char *pDest)
{
	int c;
//	wait_vblank();
	for (c = 0; c < num; c++) {
		int plane, x, y;
	
		unsigned char *dp = pDest + (c * (xSize/2) * ySize);
		memset(dp, 0, (xSize/2) * ySize);
	
		for (plane = 0; plane < numPlanes; plane++) {
			int planebit = 1 << (numPlanes - 1 - plane);
			int planeoffs = (c * modulo) + planeoffsets[plane];
		
			for (y = 0; y < ySize; y++) {
				int yoffs = planeoffs + yoffsets[y];
				dp = pDest + (c * (xSize/2) * ySize) + (y * (xSize/2));
			
				for (x = 0; x < xSize; x+=2) {
					if (readbit(pSrc, yoffs + xoffsets[x])) dp[x>>1] |= (planebit&0x0f)<<4;
					if (readbit(pSrc, yoffs + xoffsets[x+1])) dp[x>>1] |= (planebit& 0x0f);
				}
			}
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void rotate_tile(unsigned int size,unsigned char flip, unsigned char *target)
{
	unsigned int i,j,k,l=0;
	unsigned char temp[8][8];
	unsigned char rot[8][8];

	for (k=0;k<size;k++)
	{
		for(i=0;i<8;i++)
			for(j=0;j<4;j++)
			{
				temp[i][j<<1]=target[l+(i*4)+j]>>4;
				temp[i][(j<<1)+1]=target[l+(i*4)+j]&0x0f;
			}

		memset(&target[l],0,32);
		
		for(i=0;i<8;i++)
			for(j=0;j<8;j++)
			{
				if(flip)
				 rot[7-i][j]= temp[j][i] ;
				else
				 rot[i][7-j]= temp[j][i] ;
			}

		for(i=0;i<8;i++)
			for(j=0;j<4;j++)
					target[l+(i*4)+j]    = (rot[i][j*2]<<4)|(rot[i][(j*2)+1]&0xf);
		l+=32;
	}	
}
//-------------------------------------------------------------------------------------------------------------------------------------
void init_32_colors(unsigned int *t_pal,unsigned char *color_prom)
{
	unsigned int i;
//	unsigned int t_pal[32];
//	unsigned char *color_prom = Prom;

	for (i = 0;i < 32;i++)
	{
		int bit0,bit1,bit2,r,g,b;

		bit0 = (*color_prom >> 0) & 0x01;
		bit1 = (*color_prom >> 1) & 0x01;
		bit2 = (*color_prom >> 2) & 0x01;
		r =  0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		bit0 = (*color_prom >> 3) & 0x01;
		bit1 = (*color_prom >> 4) & 0x01;
		bit2 = (*color_prom >> 5) & 0x01;
		g = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

		bit0 = 0;
		bit1 = (*color_prom >> 6) & 0x01;
		bit2 = (*color_prom >> 7) & 0x01;
		b = 0x21 * bit0 + 0x47 * bit1 + 0x97 * bit2;

        r =  (r >>3);
        g =  (g >>3);
        b =  (b >>3);

		t_pal[i] = RGB(r,g,b);
		color_prom++;
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------
void wait_vblank(void)
{
     while((TVSTAT & 8) == 0);
     while((TVSTAT & 8) == 8);
}
#if 0
//-------------------------------------------------------------------------------------------------------------------------------------
void vout(char *string, char *fmt, ...)                                         
{                                                                               
   va_list arg_ptr;                                                             
   va_start(arg_ptr, fmt);                                                      
   vsprintf(string, fmt, arg_ptr);                                              
   va_end(arg_ptr);                                                             
}
//-------------------------------------------------------------------------------------------------------------------------------------
void DMA_ScuIndirectMemCopy(void *dst, void *src, Uint32 cnt, Uint32 channel)
{
	typedef struct TC_TRANSFER {
		Uint32 size;
		void *target;
		void *source;
	} TC_transfer __attribute__ ((aligned (8)));

    TC_transfer tc;
    Uint32 msk = get_imask();
    set_imask(15);

    DmaScuPrm prm;
    DmaScuStatus status;

	tc.size = cnt<<1;
    tc.source = (void *)src;
	tc.source += (1<<31);
    tc.target  = (void *)dst;

    prm.dxr = (Uint32)NULL;     // no meaning in indirect mode
    prm.dxw = ((Uint32)&tc);
    prm.dxc = 0; // not matter ? sizeof();
    prm.dxad_r = DMA_SCU_R4;
    prm.dxad_w = DMA_SCU_W2; // pas toucher
    prm.dxmod = DMA_SCU_IN_DIR;
    prm.dxrup = DMA_SCU_KEEP;
    prm.dxwup = DMA_SCU_KEEP;
    prm.dxft = DMA_SCU_F_DMA;
    prm.msk = DMA_SCU_M_DXR    |              DMA_SCU_M_DXW    ;

    do 
	{
        DMA_ScuGetStatus(&status, channel);
    }
	while(status.dxmv == DMA_SCU_MV);

    DMA_ScuSetPrm(&prm, channel);
    DMA_ScuStart(channel);

	set_imask(msk);
    set_imask(msk);                                         /* 割り込みPOP   */
}
#endif
//-------------------------------------------------------------------------------------------------------------------------------------
/*int SlaveInWork()
{
  return ((*(unsigned char *)0xfffffe11 & 0x80) == 0);
}			 */
//-------------------------------------------------------------------------------------------------------------------------------------
/*
static void InitSlaveSH(void)
{
    volatile unsigned short i;

static void **SlaveSHEntry = (void **)0x6000250;   //* BOOT ROMs dispatch address 
static volatile unsigned char *SMPC_COM = (unsigned char *)0x2010001F;   //* SMPC command register 
static volatile unsigned char *SMPC_RET = (unsigned char *)0x2010005f;   //* SMPC result register 
static volatile unsigned char *SMPC_SF  = (unsigned char *)0x20100063;   //* SMPC status flag 
static const unsigned char SMPC_SSHON  = 0x02;          //* SMPC slave SH on command 
static const unsigned char SMPC_SSHOFF = 0x03;          //* SMPC slave SH off command 

    *(volatile unsigned char *)0xfffffe10  = 0x01;    //* TIER FRT INT disable 
//    SPR_SlaveState = 0;                //* set RUNNING state 
    //* SlaveSH のリセット状態を設定する 
    while((*SMPC_SF & 0x01) == 0x01);
    *SMPC_SF = 1;                 //* --- SMPC StatusFlag SET 
    *SMPC_COM = SMPC_SSHOFF;      //* --- Slave SH OFF SET 
    while((*SMPC_SF & 0x01) == 0x01);
    for(i = 0 ; i < 1000; i++);   //* slave reset assert length 
    *(void **)SlaveSHEntry = (void *)0x6000646; //* dispatch address set 
    //* SlaveSH のリセット状態を解除する 
    *SMPC_SF = 1;                 //* --- SMPC StatusFlag SET 
    *SMPC_COM = SMPC_SSHON;       // --- Slave SH ON SET 
    while((*SMPC_SF & 0x01) == 0x01);
}
//-------------------------------------------------------------------------------------------------------------------------------------
void  WaitEndSlave(void)
{
    while((*(volatile unsigned char *)0xfffffe11 & 0x80) != 0x80);
    *(volatile unsigned char *)0xfffffe11 = 0x00; // FTCSR clear 
    *(volatile unsigned short *)0xfffffe92 |= 0x10; // chache parse all 
}
//-------------------------------------------------------------------------------------------------------------------------------------
void stop_slave()
{
		 if(SlaveInWork())
		{
//			 int i = 0;
//			FNT_Print256_2bpp((volatile unsigned char *)SS_FONT,(unsigned char *)"SlaveInWork   yes            ",10,20);	
//			InitSlaveSH();
			*( unsigned short * )0x21800000 = 0xffff;
//			*( unsigned short * )0x21000000 = 0xffff;
			while(SlaveInWork())
			{
				WaitEndSlave();
			}	  
		}
		_smpc_SSHOFF();
}
*/
//-------------------------------------------------------------------------------------------------------------------------------------
