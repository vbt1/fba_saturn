#include "../burnint.h"
#include "ovl.h"
#include "images\main.h"
#include "images\help.h"

int load_img(int id) __attribute__ ((boot,section(".boot")));
//--------------------------------------------------------------------------------------------------------------
static inline void load_main()
{
//tga_load(const uint8_t *file, uint16_t *vram, uint16_t *cram)
	  TVOFF;
	  tga_load(main_tga,(Uint8 *)ss_map,(Uint16 *)SCL_COLRAM_ADDR);
	  TVON;
//decode_map_rle8(test_tga, ss_map, int width, int height,	  const int chan);
}
//--------------------------------------------------------------------------------------------------------------
static inline void load_help()
{
	ss_regs->dispenbl &= 0xfffD;
	ss_regs->dispenbl &= 0xfffe;
//	ss_regs->dispenbl |= 2;
	ss_reg->n0_move_x =  (-16<<16) ;
	SclProcess =1;
	tga_load(help_tga,(Uint8 *)ss_map,(Uint16 *)SCL_COLRAM_ADDR);
	ss_regs->dispenbl |= 1;
//	SclProcess =1;
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
				ss_regs->dispenbl |= 2;
				load_main();
				break;
			case 1:
				load_help();
				break;
	}
	*(Uint16 *)0x25E00000=RGB( 0, 0, 0 );
	SclProcess =1;
	TVON;
}
//--------------------------------------------------------------------------------------------------------------
