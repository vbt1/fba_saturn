#include "../burnint.h"
#include "ovl.h"
#include "images\main.h"
#include "images\help.h"

int load_img(int id) __attribute__ ((boot,section(".boot")));
//--------------------------------------------------------------------------------------------------------------
static inline void load_main()
{
	TVOFF;
	tga_load(main_tga,(Uint8 *)ss_map,(Uint16 *)SCL_COLRAM_ADDR);
// vbt à remettre
//	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"A : Help  C : Credits",88,211);
	TVON;
}
//--------------------------------------------------------------------------------------------------------------
static inline void load_help()
{
	TVOFF;
	for (int l=0;l<160;l+=10 )
	{
		FNT_Print256_2bpp   ((volatile Uint8 *)SS_FONT,(Uint8 *)"                                        ",20,40+l);
	}
 
	tga_load(help_tga,(Uint8 *)ss_map,(Uint16 *)SCL_COLRAM_ADDR);
	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"          B : Return",88,211);
	TVON;
}
//--------------------------------------------------------------------------------------------------------------
static inline void load_credits()
{
	TVOFF;
	for (int l=0;l<160;l+=10 )
	{
		FNT_Print256_2bpp   ((volatile Uint8 *)SS_FONT,(Uint8 *)"                                        ",20,40+l);
	}
	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Credits (no order)",20,40);

	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"FBalpha team (barry,iq,dink), groepaz",20,60);
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"cafe-alpha for satcom, devcard & help",20,70);
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"redringrico for new saturn sdk",20,80);
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"cgfm2 for his help (SMS Plus)",20,90);
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"Martin Konrad for his support",20,100);
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"Steph for CZ80 & his sound core",20,110);
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"Fox68k for his help on FAZE",20,120);
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"All #smspower members for support",20,130);
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"segaxtreme for docs, tools and help",20,140);
	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"yabause team (guile,mrkotfw,bluecrab)",20,150);

//	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"Screw list",20,170);
//	FNT_Print256_2bpp((volatile Uint8 *)SS_FONT,(Uint8 *)"CyberwarriorX",20,190);

	tga_load(main_tga,(Uint8 *)ss_map,(Uint16 *)SCL_COLRAM_ADDR);
	FNT_Print256_2bppSel((volatile Uint8 *)SS_FONT,(Uint8 *)"          B : Return",88,211);
	TVON;
}
//--------------------------------------------------------------------------------------------------------------
int load_img(int id)
{
	ss_map   = (Uint16 *)SS_MAP;
	ss_reg    = (SclNorscl *)SS_REG;
	ss_regs  = (SclSysreg *)SS_REGS;
	*(Uint16 *)0x25E00000=RGB( 0, 0, 0 );
	TVOFF;
	ss_reg->n0_move_x =  0 ;
	switch(id)
	{
			case 0:
//				ss_regs->dispenbl |= 2;
				load_main();
				break;
			case 1:
				load_help();
				break;
			case 2:
				load_credits();
				break;
	}
	*(Uint16 *)0x25E00000=RGB( 0, 0, 0 );
	SclProcess =1;
	TVON;
}
//--------------------------------------------------------------------------------------------------------------
