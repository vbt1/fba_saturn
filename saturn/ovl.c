#define	RGB( r, g, b )		(0x8000U|((b) << 10)|((g) << 5 )|(r))
//-------------------------------------------------------------------------------------------------------------------------------------
/*inline*/ /*static*/ int readbit(const unsigned char *src, int bitnum)
{
	return src[bitnum / 8] & (0x80 >> (bitnum % 8));
}
//-------------------------------------------------------------------------------------------------------------------------------------
/*static*/ void GfxDecode4Bpp(int num, int numPlanes, int xSize, int ySize, int planeoffsets[], int xoffsets[], int yoffsets[], int modulo, unsigned char *pSrc, unsigned char *pDest)
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
/*
void rotate_tile(unsigned int size,unsigned char flip, unsigned char *target)
{
	unsigned int i,j,k,l=0;
	unsigned char temp[8][8];

	for (k=0;k<size;k++)
	{
		for(i=0;i<8;i++)
			for(j=0;j<4;j++)
			{
				temp[i][j<<1]=target[l+(i<<2)+j]>>4;
				temp[i][(j<<1)+1]=target[l+(i<<2)+j]&0x0f;
			}
  
		for(i=0;i<8;i++)
		{
			for(j=0;j<8;j+=2)
			{
				if(flip)
//					target[l+(i<<2)+(j>>1)]= (temp[j][i]<<4)|temp[j+1][i] ;
					target[l+((7-i)<<2)+((7-j)>>1)]= (temp[7-j][i])|temp[7-j-1][i]<<4 ;
				else
					target[l+(i<<2)+(j>>1)]= (temp[7-j][i]<<4)|temp[7-j-1][i] ;
			}
//					target[l+(i<<2)+(j>>1)]= (temp[j][i]<<4)|temp[j+1][i] ;
		}		 
		l+=32;
	}	
}		*/
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
