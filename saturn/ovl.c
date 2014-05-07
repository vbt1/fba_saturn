//-------------------------------------------------------------------------------------------------------------------------------------
static void rotate_tile(unsigned int size,unsigned char flip, unsigned char *target)
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
					target[l+((7-i)<<2)+((7-j)>>1)]= (temp[8-j-1][i])|temp[8-j-2][i]<<4 ;
				else
					target[l+(i<<2)+(j>>1)]= (temp[8-j-1][i]<<4)|temp[8-j-2][i] ;
			}
//					target[l+(i<<2)+(j>>1)]= (temp[j][i]<<4)|temp[j+1][i] ;
		}
		l+=32;
	}	
}

//-------------------------------------------------------------------------------------------------------------------------------------

static void init_32_colors(unsigned int *t_pal,unsigned char *color_prom)
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
