#ifndef _OVL_H_
#define _OVL_H_

//extern Uint32   _bstart, _bend;
extern Uint32 *shared;
extern struct BurnDriver* pDriver[];
//static int nBurnSoundLen;
//static Sint8 *nSoundBuffer = (Sint8 *)0x25a20000;
extern unsigned int  nSoundBufferPos;
static SclNorscl *ss_reg;
static SclSysreg	*ss_regs;
static SclSpPriNumRegister  *ss_SpPriNum;
static SclBgPriNumRegister  *ss_BgPriNum;
static SclOtherPriRegister    *ss_OtherPri;
/*static*/ SprSpCmd				*ss_sprite;
//static struct BurnDriver *fba_drv;

static UINT16  *colAddr; 
static UINT16  *colBgAddr; // color
static UINT16  *colBgAddr2;
static Uint16  *ss_font;
static Uint16  *ss_map;
static Uint16  *ss_map2;
static Fixed32 *ss_scl;

static unsigned char *cache;

//#define	REGADDR	    0x25F80000
#define TVMD        (*(Uint16 *)0x25F80000)
#define TVSTAT      (*(volatile Uint16 *)0x25F80004)
#define TVOFF   TVMD &= 0x7fff
#define TVON     TVMD |= 0x8000

#define FBA_DRV	 shared
#define SS_MAP	 *(&shared + 1)
#define SS_MAP2	 *(&shared + 2)
#define SS_FONT	 *(&shared + 3)
#define SS_CACHE *(&shared + 4)
#define SS_SPRAM *(&shared + 5)
#define SS_REG   *(&shared + 6)
#define SS_REGS   *(&shared + 7)
#define SS_N0PRI *(&shared + 8)
#define SS_SPPRI *(&shared + 9)
#define SS_SPRIT *(&shared + 10)
#define SS_OTHR *(&shared + 11)
#define SS_SCL   *(&shared + 12)

#endif

#define SS_SET_N0PRIN(/* 3 bits */ n0prin) \
  (ss_BgPriNum->PriorityNumberNBG01 \
   = (ss_BgPriNum->PriorityNumberNBG01 & 0xFF00) | ((n0prin)))
#define SS_SET_N1PRIN(/* 3 bits */ n1prin) \
  (ss_BgPriNum->PriorityNumberNBG01 \
   = (ss_BgPriNum->PriorityNumberNBG01 & 0x00FF) | ((n1prin) << 8))
#define SS_SET_N2PRIN(/* 3 bits */ n2prin) \
  (ss_BgPriNum->PriorityNumberNBG23 \
   = (ss_BgPriNum->PriorityNumberNBG23 & 0xFF00) | ((n2prin)))
#define SS_SET_S0PRIN(/* 3 bits */ s0prin) \
  (ss_SpPriNum->PriorityNumberSP01 \
   = (ss_SpPriNum->PriorityNumberSP01 & 0xFFF0) | (s0prin))
#define SS_SET_SPCLMD(/* 1 bit */ spclmd) \
   (ss_OtherPri->SpriteControl \
   = (ss_OtherPri->SpriteControl & 0xFFDF) | ((spclmd) << 5))
