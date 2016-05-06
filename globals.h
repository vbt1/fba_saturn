extern Uint32 *shared;
extern unsigned int  nSoundBufferPos;
#define FBA_DRV	 shared
#define SS_MAP	 *(&shared + 1)
#define SS_MAP2	 *(&shared + 2)
#define SS_FONT	 *(&shared + 3)
#define SS_CACHE *(&shared + 4)
#define SS_SPRAM *(&shared + 5)
#define SS_REG *(&shared + 6)
#define SS_REGD *(&shared + 7)
#define SS_REGS *(&shared + 8)
#define SS_N0PRI *(&shared + 9)
#define SS_SPPRI *(&shared + 10)
#define SS_SPRIT *(&shared + 11)
#define SS_OTHR *(&shared + 12)
#define SS_BGMIX *(&shared + 13)
#define SS_SCL *(&shared + 14)
#define SS_SCL1 *(&shared + 15)
