#include <sl_def.h>
#include "sega_scl2.h"
#include "sega_dma.h"
#include "..\globals.h"
#include "machine.h"

/*static*/ //SclWinscl	*ss_regw = NULL;
//-------------------------------------------------------------------------------------------------------------------------------------:/*
static void SCL_SetWindowSub(Uint32 surfaces,Uint32 enable,Uint8 *contrl,Uint8 on,Uint8 off)
{
//    if( (enable & SCL_NBG0) || (enable & SCL_RBG1) )
    if( enable & SCL_NBG0 )
    {
//	if( (surfaces & SCL_NBG0) || (surfaces & SCL_RBG1) )
	if( surfaces & SCL_NBG0 )
				contrl[1] |= on;
	else			contrl[1] &= off;
    }

//    if( (enable & SCL_NBG1) || (enable & SCL_EXBG) )
    if( enable & SCL_NBG1 )
    {
//	if( (surfaces & SCL_NBG1) || (surfaces & SCL_EXBG) )
	if( surfaces & SCL_NBG1 )
				contrl[0] |= on;
	else			contrl[0] &= off;
    }

    if(enable & SCL_SPR)
    {
	if(surfaces & SCL_SPR)	contrl[4] |= on;
	else			contrl[4] &= off;
    }
}
//-------------------------------------------------------------------------------------------------------------------------------------
void	SCL_SetWindow(Uint8 win,Uint32 logic,Uint32 enable,Uint32 area,
		Uint16 sx,Uint16 sy,Uint16 ex,Uint16 ey)
{
    Uint16	*sxy,*exy;
    Uint8	*contrl;
    Uint8	en_on,en_off;
    Uint8	ar_on,ar_off;

	SclWinscl *ss_regw = (SclWinscl *)SS_REGW;

    switch(win)
    {
	case SCL_W0:
		ss_regw->linewin0_addr = 0;
		sxy = &ss_regw->win0_start[0];
		exy = &ss_regw->win0_end[0];
		en_on  = 0x02;
		en_off = 0xfd;
		ar_on  = 0x01;
		ar_off = 0xfe;
		break;
	case SCL_W1:
		ss_regw->linewin1_addr = 0;
		sxy = &ss_regw->win1_start[0];
		exy = &ss_regw->win1_end[0];
		en_on  = 0x08;
		en_off = 0xf7;
		ar_on  = 0x04;
		ar_off = 0xfb;
		break;
	default:
		return;
		break;
    }

    contrl = (Uint8 *)&ss_regw->wincontrl[0];

	sxy[0] = sx*2;
	exy[0] = ex*2;

	sxy[1] = sy;
    exy[1] = ey;

    if(logic | enable)	SCL_SetWindowSub(logic,enable,contrl,0x80,0x7f);

    if(area | enable)	SCL_SetWindowSub(area,enable,contrl,ar_on,ar_off);

    SCL_SetWindowSub(enable,0xffffffff,contrl,en_on,en_off);
	SCL_Open();
//    if(SclProcess == 0)	SclProcess = 1;
}





void SDMA_ScuCst(Uint32 ch, void *dst, void *src, Uint32 cnt)
{
    DmaScuPrm prm;                              /* 転送パラメータ            */
                                                /*****************************/

    prm.dxr = (Uint32)src;
    prm.dxw = (Uint32)dst;
    prm.dxc = cnt;
    prm.dxad_r = DMA_SCU_R4;
    prm.dxad_w = DMA_SCU_W2;
    prm.dxmod = DMA_SCU_DIR;
    prm.dxrup = DMA_SCU_KEEP;
    prm.dxwup = DMA_SCU_REN;
    prm.dxft = DMA_SCU_F_DMA;
    prm.msk = DMA_SCU_M_DXR   |
              DMA_SCU_M_DXW;

    DMA_ScuSetPrm(&prm, ch);
    DMA_ScuStart(ch);
}

Uint32 SDMA_ScuResult(Uint32 ch)
{
    DmaScuStatus status;

    DMA_ScuGetStatus(&status, ch);
        if(status.dxmv == DMA_SCU_MV)
            return(DMA_SCU_BUSY);
        return(DMA_SCU_END);
}

#if 1
typedef struct TC_TRANSFER {
    Uint32 size;
    void *target;
    void *source;
} TC_transfer; // __attribute__ ((aligned (8)));

void DMA_ScuIndirectMemCopy(void *dst, void *src, Uint32 cnt)
{
    Uint32 msk;
    DmaScuPrm prm;
	TC_transfer vbt[16]  __attribute__ ((aligned (8)));
// ????
    msk = get_imask();
    set_imask(15);

//    TC_transfer *tc = (TC_transfer *)BurnMalloc (16*sizeof(TC_transfer));
    TC_transfer *tc = (TC_transfer *)&vbt[0];

    tc->size = cnt;
    tc->source = &src[0];
    tc->target = &dst[0];

tc++;
    tc->size = cnt;
    tc->source = ((Uint32)src+cnt)+1<<31;
    tc->target = (void *)dst+cnt;
/*
    tc[1].size = cnt;
    tc[1].source = ((Uint32)src)+1<<31;
    tc[1].target = dst;
*/

    prm.dxr = (Uint32)NULL;     // no meaning in indirect mode
    prm.dxw = ((Uint32)src);
    prm.dxc = 0; // not matter ? sizeof();
    prm.dxad_r = DMA_SCU_R4;

//    if(((prm.dxw >= ADR_B_BUS_START) & (prm.dxw < ADR_B_BUS_END)) ||
//       ((prm.dxw >= CADR_B_BUS_START) & (prm.dxw < CADR_B_BUS_END))){
       prm.dxad_w = DMA_SCU_W2;
//    }else
//	{
//       prm.dxad_w = DMA_SCU_W4;
//    }

    prm.dxmod = DMA_SCU_IN_DIR;
    prm.dxrup = DMA_SCU_KEEP;
    prm.dxwup = DMA_SCU_KEEP;
    prm.dxft = DMA_SCU_F_DMA;
    prm.msk = DMA_SCU_M_DXR    |
              DMA_SCU_M_DXW    ;
    // set end code
 //   (tc->source + nTransfer - 1)->source += 1 << 31;
//    src[tc.size - 1] += 1 << 31;

    DMA_ScuSetPrm(&prm, DMA_SCU_CH0);
//    DMA_ScuStart(DMA_SCU_CH0); // on garde ou pas ???

	set_imask(msk);

//    dma_start_flg = ON;                         /* DMAはスタートしている     */
//    set_imask(msk);                                         /* 割り込みPOP   */
}
#endif
