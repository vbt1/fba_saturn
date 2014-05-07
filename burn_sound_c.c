#include "burnint.h"
//#include "burn_sound.h"

#define CLIP(A) ((A) < -0x8000 ? -0x8000 : (A) > 0x7fff ? 0x7fff : (A))
//#define CLIP(A) ((A) < -0x5000 ? -0x5000 : (A) > 0x4fff ? 0x4fff : (A))

void BurnSoundCopyClamp_C(int *Src, short *Dest, int Len)
{
	Len *= 2;
	while (Len--) {
		*Dest = CLIP((*Src >> 8));
		Src++;
		Dest++;
	}
}

void BurnSoundCopyClamp_Add_C(int *Src, short *Dest, int Len)
{
	Len *= 2;
		while (Len--) {
		*Dest = CLIP((*Src >> 8) + *Dest);
		Src++;
		Dest++;
	}
}
/*
// Byteswaps an area of memory
int BurnByteswap(UINT8* pMem, int nLen)
{
	unsigned int i;
	nLen >>= 1;
	for ( i = 0; i < nLen; i++, pMem += 2) {
		UINT8 t = pMem[0];
		pMem[0] = pMem[1];
		pMem[1] = t;
	}

	return 0;
}
*/
void BurnSoundCopyClamp_Mono_C(int *Src, short *Dest, int Len)
{
	while (Len--) {
		Dest[0] = CLIP((*Src >> 8));
//		Dest[1] = CLIP((*Src >> 8));
				   
		Src++;
//		Dest += 2;
		Dest += 1;
	}
}

void BurnSoundCopyClamp_Mono_Add_C(int *Src, short *Dest, int Len)
{
	while (Len--) {
		Dest[0] = CLIP((*Src >> 8) + Dest[0]);
		Dest[1] = CLIP((*Src >> 8) + Dest[1]);
		Src++;
		Dest += 2;
	}
}

#undef CLIP
