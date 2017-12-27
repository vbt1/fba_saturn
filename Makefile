#
#   SEGA SATURN Graphic library make file for GNU

# macro
CC = sh-elf-gcc
AS = sh-elf-as
LD = sh-elf-ld
RM = rm
CONV = sh-elf-objcopy

MAKEFILE = Makefile

#CCFLAGS =  -mhitachi -m2 -std=gnu99 -Wfatal-errors -Os -fno-exceptions -fomit-frame-pointer -D_SH -DMODEL_S -c -I.
CCFLAGS2 = -mno-fsrra -maccumulate-outgoing-args -m2 -std=gnu99 -Wfatal-errors -Os -fno-exceptions -D_SH -DMODEL_S -c -I.
#CCOVLFLAGS = -mno-fsrra -maccumulate-outgoing-args -mrenesas -m2 -std=gnu99 -Wfatal-errors -O2 -fomit-frame-pointer -fno-exceptions -D_SH -DMODEL_S -c
#CCOVLFLAGS = -g -mno-fsrra -maccumulate-outgoing-args -mrenesas -m2 -std=gnu99 -Wfatal-errors -O0 -fomit-frame-pointer -D_SH -DMODEL_S -c
#CCOVLFLAGS = -g -m2 -mrenesas  -std=gnu99 -Wfatal-errors -Os -D_SH -DMODEL_S -c
# pour asm 
#CCOVLFLAGS = -S -fverbose-asm -mno-fsrra -maccumulate-outgoing-args -mrenesas -m2 -std=gnu99 -Wfatal-errors -O2 -fomit-frame-pointer -D_SH -DMODEL_S -c
CCOVLFLAGS = -mno-fsrra -maccumulate-outgoing-args -mrenesas -m2 -std=gnu99 -Wfatal-errors -O2 -fomit-frame-pointer -D_SH -DMODEL_S -c
OLVSCRIPT = root/overlay.lnk
#LDOVLFLAGS = -s -O3 -Xlinker --defsym -Xlinker ___malloc_sbrk_base=0x6040000 -Xlinker --defsym -Xlinker __heap_end=0x60fffff -Xlinker -T$(LDOVLFILE) -Xlinker -Map -Xlinker $(MPOVLFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles  -nostdlib

TARGET    = root/sl.coff
TARGET1  = root/sl.bin
LDFILE	 = ./$(TARGET:.coff=.lnk)
MPFILE     = $(TARGET:.coff=.maps)
LDFLAGS = -m2 -O2 -Xlinker -T$(LDFILE) -Xlinker -Map -Xlinker $(MPFILE) -Xlinker -e -Xlinker 0x6004000 -nostartfiles
#SRCS       = saturn/iapetus/cd/cd.c  burn.c load.c saturn/font.c saturn/file.c saturn/saturn.c saturn/low.s saturn/sc_saturn_vbt.c
SRCS       = saturn/iapetus/cd/cd.c  saturn/low.s load.c burn.c saturn/font.c saturn/file.c saturn/saturn.c 
OBJS2     = strt/strt1_g.o strt/strt2_g.o ../../SBL6/SEGASMP/PER/SMPCLIB/per_x12.o ../../SBL6/SEGASMP/PER/SMPCLIB/per_x22.o $(SRCS:.c=.o)

OVLIMG                 = root/img.coff
OVLIMG1               = root/img.bin
LDOVLIMGFILE     = ./$(OVLIMG:.coff=.lnk)
MPOVLIMGFILE    = $(OVLIMG:.coff=.maps)
LDOVLIMGFLAGS = -m2 -s -O2 -Xlinker -T$(LDOVLIMGFILE) -Xlinker -Map -Xlinker $(MPOVLIMGFILE) -Xlinker -e -Xlinker _ovlstart -nostartfiles
SRCOVLIMG         = saturn/img.c libyaul/libtga/tga.c
OBJOVLIMG         = $(SRCOVLIMG:.c=.o)

OVERLAY	       = root/d_bankp.coff
OVERLAY1     = root/d_bankp.bin
MPOVLFILE    = $(OVERLAY:.coff=.maps)
LDOVLFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVL         = d_bankp.c snd/sn76496.c saturn/ovl.c
#SRCOVL         = d_bankp.c czet.c cz80/cz80.c snd/sn76496.c saturn/ovl.c
OBJOVL         = $(SRCOVL:.c=.o)

OVLNEWS                 = root/d_news.coff
OVLNEWS1               = root/d_news.bin
MPOVLNEWSFILE    = $(OVLNEWS:.coff=.maps)
LDOVLNEWSFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLNEWSFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLNEWS         = d_news.c czet.c cz80/cz80.c snd/msm6295.c burn_sound_c.c saturn/ovl.c
OBJOVLNEWS         = $(SRCOVLNEWS:.c=.o)

OVLGBERET                 = root/d_gberet.coff
OVLGBERET1               = root/d_gberet.bin
MPOVLGBERETFILE    = $(OVLGBERET:.coff=.maps)
LDOVLGBERETFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLGBERETFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLGBERET         = d_gberet.c snd/sn76496.c saturn/ovl.c 
OBJOVLGBERET         = $(SRCOVLGBERET:.c=.o)

OVLHIGEMARU                 = root/d_higemaru.coff
OVLHIGEMARU1               = root/d_higema.bin
MPOVLHIGEMARUFILE    = $(OVLHIGEMARU:.coff=.maps)
LDOVLHIGEMARUFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLHIGEMARUFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLHIGEMARU         = d_higemaru.c czet.c cz80/cz80.c snd/ay8910.c  saturn/ovl.c
OBJOVLHIGEMARU         = $(SRCOVLHIGEMARU:.c=.o)

OVLPKUNW                 = root/d_pkunw.coff
OVLPKUNW1               = root/d_pkunw.bin
MPOVLPKUNWFILE    = $(OVLPKUNW:.coff=.maps)
LDOVLPKUNWFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLPKUNWFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLPKUNW         = d_pkunwar.c czet.c cz80/cz80.c snd/ay8910.c saturn/ovl.c
OBJOVLPKUNW         = $(SRCOVLPKUNW:.c=.o)

OVLMITCH                 = root/d_mitch.coff
OVLMITCH1               = root/d_mitch.bin
MPOVLMITCHFILE    = $(OVLMITCH:.coff=.maps)
LDOVLMITCHFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLMITCHFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLMITCH         = d_mitchell.c czet.c cz80/cz80.c kabuki.c snd/msm6295.c burn_sound_c.c eeprom.c saturn/ovl.c
OBJOVLMITCH         = $(SRCOVLMITCH:.c=.o)

OVLGNG                 = root/d_gng.coff
OVLGNG1               = root/d_gng.bin
MPOVLGNGFILE    = $(OVLGNG:.coff=.maps)
LDOVLGNGFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLGNGFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLGNG         = m6809_intf.c m6809.c  d_gng.c saturn/ovl.c
OBJOVLGNG         = $(SRCOVLGNG:.c=.o)

OVLSYS1                 = root/d_sys1.coff
OVLSYS11               = root/d_sys1.bin
MPOVLSYS1FILE    = $(OVLSYS1:.coff=.maps)
LDOVLSYS1FLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSYS1FILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLSYS1         = d_sys1.c czet.c cz80/cz80.c snd/sn76496.c saturn/ovl.c
OBJOVLSYS1         = $(SRCOVLSYS1:.c=.o)

OVLSYS1H                 = root/d_SYS1H.coff
OVLSYS1H1               = root/d_SYS1H.bin
MPOVLSYS1HFILE    = $(OVLSYS1H:.coff=.maps)
LDOVLSYS1HFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSYS1HFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLSYS1H         = d_sys1h.c czet.c cz80/cz80.c snd/sn76496.c mc8123.c saturn/ovl.c
OBJOVLSYS1H         = $(SRCOVLSYS1H:.c=.o)

OVLSYS2                 = root/d_sys2.coff
OVLSYS21               = root/d_sys2.bin
MPOVLSYS2FILE    = $(OVLSYS2:.coff=.maps)
LDOVLSYS2FLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSYS2FILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLSYS2         = d_sys2.c czet.c cz80/cz80.c snd/sn76496.c 8255ppi.c mc8123.c saturn/ovl.c
OBJOVLSYS2         = $(SRCOVLSYS2:.c=.o)

OVLPACM                 = root/d_pacm.coff
OVLPACM1               = root/d_pacm.bin
MPOVLPACMFILE    = $(OVLPACM:.coff=.maps)
LDOVLPACMFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLPACMFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLPACM         = d_pacman.c czet.c snd/namco_snd.c cz80/cz80.c saturn/ovl.c
#SRCOVLPACM         = d_pacman_zet.c zet.c z80.c z80daisy.c namco_snd.c 
OBJOVLPACM         = $(SRCOVLPACM:.c=.o)

OVLSMS                 = root/d_sms.coff 
OVLSMS1               = root/d_sms.bin
MPOVLSMSFILE    = $(OVLSMS:.coff=.maps)
LDOVLSMSFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSMSFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLSMS         = d_sms.c psg_sms.c
OBJOVLSMS         = $(SRCOVLSMS:d_sms.c=sms/d_sms.o)

OVLSMSCZ80                 = root/d_smscz.coff
OVLSMSCZ801               = root/d_smscz.bin
MPOVLSMSCZ80FILE    = $(OVLSMSCZ80:.coff=.maps)
LDOVLSMSCZ80FLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSMSCZ80FILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLSMSCZ80         = d_sms.c psg_sms.c czet.c cz80/cz80.c
OBJOVLSMSCZ80         = $(SRCOVLSMSCZ80:.c=.o)

OVLGG                = root/d_gg.coff
OVLGG1              = root/d_gg.bin
MPOVLGGFILE    = $(OVLGG:.coff=.maps)
LDOVLGGFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLGGFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLGG         = d_sms.c psg_sms.c 
OBJOVLGG         = $(SRCOVLSMS:d_sms.c=gg/d_sms.o)

OVLGGCZ                = root/d_ggcz.coff
OVLGGCZ1              = root/d_ggcz.bin
MPOVLGGCZFILE    = $(OVLGGCZ:.coff=.maps)
LDOVLGGCZFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLGGCZFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLGGCZ         = d_sms.c psg_sms.c czet.c cz80/cz80.c
OBJOVLGGCZ         = $(SRCOVLSMS:d_sms.c=ggcz/d_sms.o)

OVLZAXXON                 = root/d_zaxxon.coff
OVLZAXXON1               = root/d_zaxxon.bin
MPOVLZAXXONFILE    = $(OVLZAXXON:.coff=.maps)
LDOVLZAXXONFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLZAXXONFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLZAXXON         = d_ZAXXON.c czet.c cz80/cz80.c saturn/ovl.c
#SRCOVLZAXXON       = d_ZAXXON.c saturn/ovl.c d_zaxxon.s
OBJOVLZAXXON         = $(SRCOVLZAXXON:.c=.o)

OVLTETRIS                 = root/d_tetris.coff
OVLTETRIS1               = root/d_TETRIS.bin
MPOVLTETRISFILE    = $(OVLTETRIS:.coff=.maps)
LDOVLTETRISFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLTETRISFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
#SRCOVLTETRIS         = d_atetris_crab6502.c crab6502/crab6502.c crab6502/m6502_intf.c snd/sn76496.c slapstic.c
SRCOVLTETRIS         = d_atetris_mame6502.c m6502.new/m6502.c m6502.new/m6502_intf.c snd/sn76496.c slapstic.c
#SRCOVLTETRIS         = d_atetris.c m6502/m6502.c m6502_intf.c snd/sn76496.c slapstic.c
#SRCOVLTETRIS         = d_atetris.c m6502/m6502.c m6502_intf.c snd/sn76496.c slapstic.c
#SRCOVLTETRIS         = d_atetris.c Crab6502/Crab6502.c snd/sn76496.c slapstic.c
OBJOVLTETRIS         = $(SRCOVLTETRIS:.c=.o)

OVLVIGIL                 = root/d_vigil.coff
OVLVIGIL1               = root/d_vigil.bin
MPOVLVIGILFILE    = $(OVLVIGIL:.coff=.maps)
LDOVLVIGILFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLVIGILFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLVIGIL         = d_vigilant.c czet.c cz80/cz80.c snd/dac.c snd/ym2151.c saturn/ovl.c
#SRCOVLVIGIL         = d_vigilant.c z80_intf.c z80.c z80daisy.c burn_ym2151.c ym2151.c dac.c burn_sound_c.c saturn/ovl.c
OBJOVLVIGIL         = $(SRCOVLVIGIL:.c=.o)

OVLSG1000                 = root/d_sg1000.coff
OVLSG10001               = root/d_sg1000.bin
MPOVLSG1000FILE    = $(OVLSG1000:.coff=.maps)
LDOVLSG1000FLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSG1000FILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLSG1000         = d_sg1000.c 8255ppi.c tms9928a.c snd/sn76496.c czet.c cz80/cz80.c saturn/ovl.c
#SRCOVLSG1000         = d_sg1000.c 8255ppi.c tms9928a.c snd/sn76496.c saturn/ovl.c
OBJOVLSG1000         = $(SRCOVLSG1000:.c=.o)

OVLBOMBJACK                 = root/d_bombja.coff
OVLBOMBJACK1               = root/d_bombja.bin
MPOVLBOMBJACKFILE    = $(OVLBOMBJACK:.coff=.maps)
LDOVLBOMBJACKFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLBOMBJACKFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLBOMBJACK         = d_bombjack.c czet.c cz80/cz80.c snd/ay8910.c  saturn/ovl.c
OBJOVLBOMBJACK         = $(SRCOVLBOMBJACK:.c=.o)

OVLAPPOOO                 = root/d_appooo.coff
OVLAPPOOO1               = root/d_appooo.bin
MPOVLAPPOOOFILE    = $(OVLAPPOOO:.coff=.maps)
LDOVLAPPOOOFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLAPPOOOFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLAPPOOO         = d_appoooh.c czet.c cz80/cz80.c snd/sn76496.c snd/msm5205.c saturn/ovl.c
OBJOVLAPPOOO         = $(SRCOVLAPPOOO:.c=.o)

OVLBLKTGR                 = root/d_blktgr.coff
OVLBLKTGR1               = root/d_blktgr.bin
MPOVLBLKTGRFILE    = $(OVLBLKTGR:.coff=.maps)
LDOVLBLKTGRFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLBLKTGRFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLBLKTGR         = d_blktiger.c czet.c cz80/cz80.c snd/timer.c snd/ay8910.c snd/fm.c snd/burn_ym2203.c saturn/ovl.c
#SRCOVLBLKTGR         = d_blktiger.c czet.c cz80/cz80.c snd/timer.c saturn/ovl.c
OBJOVLBLKTGR         = $(SRCOVLBLKTGR:.c=.o)

OVLWIZ                = root/d_wiz.coff
OVLWIZ1              = root/d_wiz.bin
MPOVLWIZFILE    = $(OVLWIZ:.coff=.maps)
LDOVLWIZFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLWIZFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLWIZ         = d_wiz.c czet.c cz80/cz80.c snd/ay8910.c saturn/ovl.c
OBJOVLWIZ         = $(SRCOVLWIZ:.c=.o)

OVLSLPFGHT                = root/d_slpfgh.coff
OVLSLPFGHT1              = root/d_slpfgh.bin
MPOVLSLPFGHTFILE    = $(OVLSLPFGHT:.coff=.maps)
LDOVLSLPFGHTFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSLPFGHTFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLSLPFGHT         = d_slpfgh.c czet.c cz80/cz80.c snd/ay8910.c saturn/ovl.c
OBJOVLSLPFGHT         = $(SRCOVLSLPFGHT:.c=.o)

OVLFREEK                 = root/d_freek.coff
OVLFREEK1               = root/d_freek.bin
MPOVLFREEKFILE    = $(OVLFREEK:.coff=.maps)
LDOVLFREEKFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLFREEKFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLFREEK         = d_freekick.c czet.c cz80/cz80.c snd/sn76496.c mc8123.c 8255ppi.c saturn/ovl.c
OBJOVLFREEK         = $(SRCOVLFREEK:.c=.o)

OVLMSX                 = root/d_msx.coff
OVLMSX1               = root/d_msx.bin
MPOVLMSXFILE    = $(OVLMSX:.coff=.maps)
LDOVLMSXFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLMSXFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
#SRCOVLMSX         = d_msx.c czet.c cz80/cz80.c tms9928a.c snd/k051649.c snd/ay8910.c 8255ppi.c saturn/ovl.c
#SRCOVLMSX         = d_msx.c z80_intf.c z80/z80.c z80/z80daisy.c tms9928a.c snd/ay8910.c snd/dac.c 8255ppi.c saturn/ovl.c
SRCOVLMSX         = d_msx.c tms9928a.c snd/k051649.c snd/ay8910.c snd/dac.c 8255ppi.c saturn/ovl.c
OBJOVLMSX         = $(SRCOVLMSX:.c=.o)

OVLSEGAE                 = root/d_segae.coff
OVLSEGAE1               = root/d_segae.bin
MPOVLSEGAEFILE    = $(OVLSEGAE:.coff=.maps)
LDOVLSEGAEFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSEGAEFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLSEGAE         = d_segae.c czet.c cz80/cz80.c snd/sn76496.c mc8123.c saturn/ovl.c
OBJOVLSEGAE         = $(SRCOVLSEGAE:.c=.o)

OVLSOLOMN                = root/d_solomn.coff
OVLSOLOMN1              = root/d_solomn.bin
MPOVLSOLOMNFILE    = $(OVLSOLOMN:.coff=.maps)
LDOVLSOLOMNFLAGS = -m2 -s -O2 -Xlinker -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSOLOMNFILE) -Xlinker -e -Xlinker _overlaystart -nostartfiles
SRCOVLSOLOMN         = d_solomon.c czet.c cz80/cz80.c snd/ay8910.c saturn/ovl.c
OBJOVLSOLOMN         = $(SRCOVLSOLOMN:.c=.o)

#YAULMEM = libyaul/kernel/lib/memb.c libyaul/kernel/mm/free.c libyaul/kernel/mm/free_r.c libyaul/kernel/mm/malloc.c libyaul/kernel/mm/malloc_r.c  libyaul/kernel/mm/slob.c libyaul/kernel/mm/realloc_r.c
YAULMEM = libyaul/kernel/lib/memb.c libyaul/kernel/mm/free.c libyaul/kernel/mm/malloc.c libyaul/kernel/mm/slob.c

LIBS2 =  ../../SBL6/SEGALIB/LIB/elf/sega_per.a \
../../SBL6/SEGALIB/LIB/vbtelf1/sega_spr.a \
../../SBL6/SEGALIB/SPR/vbtelf1/spr_slv.o \
../../SBL6/SEGALIB/INT/elf/int.o ../../SBL6/SEGALIB/INT/elf/int_trmp.o \
../../SBL6/SEGALIB/GFS/vbtelf/gfs2.o ../../SBL6/SEGALIB/GFS/vbtelf/gfs_cdb2.o \
../../SBL6/SEGALIB/GFS/vbtelf/gfs_cdc2.o ../../SBL6/SEGALIB/GFS/vbtelf/gfs_cdf2.o \
../../SBL6/SEGALIB/GFS/vbtelf/gfs_trn2.o ../../SBL6/SEGALIB/GFS/vbtelf/gfs_buf2.o \
../../SBL6/SEGALIB/GFS/vbtelf/gfs_dir2.o ../../SBL6/SEGALIB/PCM/vbtelf/pcm_audi2.o \
../../SBL6/SEGALIB/PCM/vbtelf/pcm_drv2.o ../../SBL6/SEGALIB/PCM/vbtelf/pcm_time2.o \
../../SBL6/SEGALIB/PCM/vbtelf/pcm_etc2.o ../../SBL6/SEGALIB/PCM/vbtelf/pcm_mp2.o \
../../SBL6/SEGALIB/PCM/vbtelf/pcm_lib2.o cdc/cdcrep.a

LIBSOVL = ../../SBL6/SEGALIB/LIB/vbtelf1/sega_spr.a ../../SBL6/SEGALIB/LIB/vbtelf1/sega_dma.a

all: $(TARGET) $(TARGET1) $(OVERLAY)  $(OVERLAY1) $(OVLIMG)  $(OVLIMG1) \
     $(OVLNEWS)  $(OVLNEWS1) $(OVLGBERET)  $(OVLGBERET1) \
     $(OVLHIGEMARU) $(OVLHIGEMARU1) $(OVLPKUNW) $(OVLPKUNW1) \
     $(OVLMITCH) $(OVLMITCH1) $(OVLGNG) $(OVLGNG1) \
     $(OVLSYS1) $(OVLSYS11) $(OVLSYS1H) $(OVLSYS1H1) \
     $(OVLSYS2) $(OVLSYS21) $(OVLPACM) $(OVLPACM1) \
     $(OVLTETRIS) $(OVLTETRIS1) $(OVLSMS) $(OVLSMS1) \
     $(OVLSMSCZ80) $(OVLSMSCZ801) $(OVLGG) $(OVLGG1) \
     $(OVLGGCZ) $(OVLGGCZ1) $(OVLAPPOOO) $(OVLAPPOOO1) \
     $(OVLBLKTGR) $(OVLBLKTGR1) $(OVLWIZ) $(OVLWIZ1) \
     $(OVLZAXXON) $(OVLZAXXON1) $(OVLVIGIL) $(OVLVIGIL1) \
     $(OVLSLPFGHT) $(OVLSLPFGHT1) $(OVLFREEK) $(OVLFREEK1) \
     $(OVLSG1000) $(OVLSG10001) $(OVLBOMBJACK) $(OVLBOMBJACK1) \
     $(OVLMSX) $(OVLMSX1) $(OVLSEGAE) $(OVLSEGAE1) \
     $(OVLSOLOMN) $(OVLSOLOMN1)

# Use gcc to link so it will automagically find correct libs directory

$(TARGET) : $(OBJS2) $(MAKEFILE) $(OBJECTS) $(LDFILE)
	$(CC) $(LDFLAGS)  $(OBJS2) $(LIBS2) $(LIBSOVL)  -o $@

$(TARGET1) : $(SYSOBJS) $(OBJS2) $(MAKEFILE) $(LDFILE)
	$(CONV) -O binary $(TARGET) $(TARGET1)

$(OVLIMG) : $(OBJOVLIMG) $(MAKEFILE) $(OBJOVLIMG) $(LDOVLIMGFILE)
	$(CC) $(LDOVLIMGFLAGS) $(OBJOVLIMG) $(LIBSOVL) -o $@

$(OVLIMG1) : $(OBJOVLIMG) $(MAKEFILE) $(LDOVLIMGFILE)
	$(CONV) -O binary $(OVLIMG) $(OVLIMG1)

$(OVERLAY) : $(OBJOVL) $(MAKEFILE) $(OBJOVL) $(LDOVLFILE)
	$(CC) $(LDOVLFLAGS) $(OBJOVL) raze/raze.o -o $@
#	$(CC) $(LDOVLFLAGS) $(OBJOVL) -o $@

$(OVERLAY1) : $(OBJOVL) $(MAKEFILE) $(LDOVLFILE)
	$(CONV) -O binary $(OVERLAY) $(OVERLAY1)

$(OVLNEWS) : $(OBJOVLNEWS) $(MAKEFILE) $(OBJOVLNEWS) $(LDOVLNEWSFILE)
	$(CC) $(LDOVLNEWSFLAGS) $(OBJOVLNEWS) $(LIBSOVL) -lm -o $@

$(OVLNEWS1) : $(OBJOVLNEWS) $(MAKEFILE) $(LDOVLNEWSFILE)
	$(CONV) -O binary $(OVLNEWS) $(OVLNEWS1)

$(OVLGBERET) : $(OBJOVLGBERET) $(MAKEFILE) $(OBJOVLGBERET) $(LDOVLGBERETFILE)
	$(CC) $(LDOVLGBERETFLAGS) $(OBJOVLGBERET) $(LIBSOVL) raze/raze.o -o $@

$(OVLGBERET1) : $(OBJOVLGBERET) $(MAKEFILE) $(LDOVLGBERETFILE)
	$(CONV) -O binary $(OVLGBERET) $(OVLGBERET1)

$(OVLHIGEMARU) : $(OBJOVLHIGEMARU) $(MAKEFILE) $(OBJOVLHIGEMARU) $(LDOVLHIGEMARUFILE)
	$(CC) $(LDOVLHIGEMARUFLAGS) $(OBJOVLHIGEMARU) $(LIBSOVL) raze/raze.o -o $@

$(OVLHIGEMARU1) : $(OBJOVLHIGEMARU) $(MAKEFILE) $(LDOVLHIGEMARUFILE)
	$(CONV) -O binary $(OVLHIGEMARU) $(OVLHIGEMARU1)

$(OVLPKUNW) : $(OBJOVLPKUNW) $(MAKEFILE) $(OBJOVLPKUNW) $(LDOVLPKUNWFILE)
	$(CC) $(LDOVLPKUNWFLAGS) $(OBJOVLPKUNW) $(LIBSOVL) raze/raze.o -o $@

$(OVLPKUNW1) : $(OBJOVLPKUNW) $(MAKEFILE) $(LDOVLPKUNWFILE)
	$(CONV) -O binary $(OVLPKUNW) $(OVLPKUNW1)

$(OVLMITCH) : $(OBJOVLMITCH) $(MAKEFILE) $(OBJOVLMITCH) $(LDOVLMITCHFILE)
	$(CC) $(LDOVLMITCHFLAGS) $(OBJOVLMITCH) $(LIBSOVL) -lm -o $@

$(OVLMITCH1) : $(OBJOVLMITCH) $(MAKEFILE) $(LDOVLMITCHFILE)
	$(CONV) -O binary $(OVLMITCH) $(OVLMITCH1)

$(OVLGNG) : $(OBJOVLGNG) $(MAKEFILE) $(OBJOVLGNG) $(LDOVLGNGFILE)
	$(CC) $(LDOVLGNGFLAGS) $(OBJOVLGNG) $(LIBSOVL) raze/raze.o -o $@

$(OVLGNG1) : $(OBJOVLGNG) $(MAKEFILE) $(LDOVLGNGFILE)
	$(CONV) -O binary $(OVLGNG) $(OVLGNG1)

$(OVLSYS1) : $(OBJOVLSYS1) $(MAKEFILE) $(OBJOVLSYS1) $(LDOVLSYS1FILE)
	$(CC) $(LDOVLSYS1FLAGS) $(OBJOVLSYS1) $(LIBSOVL) raze/raze.o -o $@

$(OVLSYS11) : $(OBJOVLSYS1) $(MAKEFILE) $(LDOVLSYS1FILE)
	$(CONV) -O binary $(OVLSYS1) $(OVLSYS11)

$(OVLSYS2) : $(OBJOVLSYS2) $(MAKEFILE) $(OBJOVLSYS2) $(LDOVLSYS2FILE)
	$(CC) $(LDOVLSYS2FLAGS) $(OBJOVLSYS2) $(LIBSOVL) raze/raze.o -o $@

$(OVLSYS21) : $(OBJOVLSYS2) $(MAKEFILE) $(LDOVLSYS2FILE)
	$(CONV) -O binary $(OVLSYS2) $(OVLSYS21)

$(OVLSYS1H) : $(OBJOVLSYS1H) $(MAKEFILE) $(OBJOVLSYS1H) $(LDOVLSYS1HFILE)
	$(CC) $(LDOVLSYS1HFLAGS) $(OBJOVLSYS1H) $(LIBSOVL) raze/raze.o -o $@

$(OVLSYS1H1) : $(OBJOVLSYS1H) $(MAKEFILE) $(LDOVLSYS1HFILE)
	$(CONV) -O binary $(OVLSYS1H) $(OVLSYS1H1)

$(OVLPACM) : $(OBJOVLPACM) $(MAKEFILE) $(OBJOVLPACM) $(LDOVLPACMFILE)
	$(CC) $(LDOVLPACMFLAGS) $(OBJOVLPACM) $(LIBSOVL) -o $@

$(OVLPACM1) : $(OBJOVLPACM) $(MAKEFILE) $(LDOVLPACMFILE)
	$(CONV) -O binary $(OVLPACM) $(OVLPACM1)

$(OVLTETRIS) : $(OBJOVLTETRIS) $(MAKEFILE) $(OBJOVLTETRIS) $(LDOVLTETRISFILE)
	$(CC) $(LDOVLTETRISFLAGS) $(OBJOVLTETRIS) $(LIBSOVL) -o $@

$(OVLTETRIS1) : $(OBJOVLTETRIS) $(MAKEFILE) $(LDOVLTETRISFILE)
	$(CONV) -O binary $(OVLTETRIS) $(OVLTETRIS1)

$(OVLSMS) : $(OBJOVLSMS) $(MAKEFILE) $(OBJOVLSMS) $(LDOVLSMSFILE)
	$(CC) $(LDOVLSMSFLAGS) $(OBJOVLSMS) $(LIBSOVL) raze_sms/raze.o -o $@

$(OVLSMS1) : $(OBJOVLSMS) $(MAKEFILE) $(LDOVLSMSFILE)
	$(CONV) -O binary $(OVLSMS) $(OVLSMS1)

$(OVLGG) : $(OBJOVLGG) $(MAKEFILE) $(OBJOVLGG) $(LDOVLGGFILE)
	$(CC) $(LDOVLGGFLAGS) $(OBJOVLGG) $(LIBSOVL) raze_sms/raze.o -o $@

$(OVLGG1) : $(OBJOVLGG) $(MAKEFILE) $(LDOVLGGFILE)
	$(CONV) -O binary $(OVLGG) $(OVLGG1)

$(OVLGGCZ) : $(OBJOVLGGCZ) $(MAKEFILE) $(OBJOVLGGCZ) $(LDOVLGGCZFILE)
	$(CC) $(LDOVLGGCZFLAGS) $(OBJOVLGGCZ) $(LIBSOVL) czet.o cz80/cz80.o -o $@

$(OVLGGCZ1) : $(OBJOVLGGCZ) $(MAKEFILE) $(LDOVLGGCZFILE)
	$(CONV) -O binary $(OVLGGCZ) $(OVLGGCZ1)

$(OVLSMSCZ80) : $(OBJOVLSMSCZ80) $(MAKEFILE) $(OBJOVLSMSCZ80) $(LDOVLSMSCZ80FILE)
	$(CC) $(LDOVLSMSCZ80FLAGS) $(OBJOVLSMSCZ80) $(LIBSOVL) -o $@

$(OVLSMSCZ801) : $(OBJOVLSMSCZ80) $(MAKEFILE) $(LDOVLSMSCZ80FILE)
	$(CONV) -O binary $(OVLSMSCZ80) $(OVLSMSCZ801)

$(OVLZAXXON) : $(OBJOVLZAXXON) $(MAKEFILE) $(OBJOVLZAXXON) $(LDOVLZAXXONFILE)
	$(CC) $(LDOVLZAXXONFLAGS) $(OBJOVLZAXXON) $(LIBSOVL) -o $@
#	$(CC) $(LDOVLZAXXONFLAGS) $(OBJOVLZAXXON) $(LIBSOVL) raze/raze.o -o $@

$(OVLZAXXON1) : $(OBJOVLZAXXON) $(MAKEFILE) $(LDOVLZAXXONFILE)
	$(CONV) -O binary $(OVLZAXXON) $(OVLZAXXON1)

$(OVLVIGIL) : $(OBJOVLVIGIL) $(MAKEFILE) $(OBJOVLVIGIL) $(LDOVLVIGILFILE)
#	$(CC) $(LDOVLVIGILFLAGS) $(OBJOVLVIGIL) $(LIBSOVL) -lm -o $@
	$(CC) $(LDOVLVIGILFLAGS) $(OBJOVLVIGIL) $(LIBSOVL) raze/raze.o -lm -o $@

 $(OVLVIGIL1) : $(OBJOVLVIGIL) $(MAKEFILE) $(LDOVLVIGILFILE)
	$(CONV) -O binary $(OVLVIGIL) $(OVLVIGIL1)

$(OVLSG1000) : $(OBJOVLSG1000) $(MAKEFILE) $(OBJOVLSG1000) $(LDOVLSG1000FILE)
	$(CC) $(LDOVLSG1000FLAGS) $(OBJOVLSG1000) $(LIBSOVL) -o $@
#	$(CC) $(LDOVLSG1000FLAGS) $(OBJOVLSG1000) $(LIBSOVL) raze/raze.o -o $@

$(OVLSG10001) : $(OBJOVLSG1000) $(MAKEFILE) $(LDOVLSG1000FILE)
	$(CONV) -O binary $(OVLSG1000) $(OVLSG10001)

$(OVLBOMBJACK) : $(OBJOVLBOMBJACK) $(MAKEFILE) $(OBJOVLBOMBJACK) $(LDOVLBOMBJACKFILE)
	$(CC) $(LDOVLBOMBJACKFLAGS) $(OBJOVLBOMBJACK) $(LIBSOVL) raze/raze.o -o $@

$(OVLBOMBJACK1) : $(OBJOVLBOMBJACK) $(MAKEFILE) $(LDOVLBOMBJACKFILE)
	$(CONV) -O binary $(OVLBOMBJACK) $(OVLBOMBJACK1)

$(OVLAPPOOO) : $(OBJOVLAPPOOO) $(MAKEFILE) $(OBJOVLAPPOOO) $(LDOVLAPPOOOFILE)
	$(CC) $(LDOVLAPPOOOFLAGS) $(OBJOVLAPPOOO) $(LIBSOVL) -lm -o $@

$(OVLAPPOOO1) : $(OBJOVLAPPOOO) $(MAKEFILE) $(LDOVLAPPOOOFILE)
	$(CONV) -O binary $(OVLAPPOOO) $(OVLAPPOOO1)

$(OVLBLKTGR) : $(OBJOVLBLKTGR) $(MAKEFILE) $(OBJOVLBLKTGR) $(LDOVLBLKTGRFILE)
	$(CC) $(LDOVLBLKTGRFLAGS) $(OBJOVLBLKTGR) $(LIBSOVL) -lm raze/raze.o -o $@

$(OVLBLKTGR1) : $(OBJOVLBLKTGR) $(MAKEFILE) $(LDOVLBLKTGRFILE)
	$(CONV) -O binary $(OVLBLKTGR) $(OVLBLKTGR1)

$(OVLWIZ) : $(OBJOVLWIZ) $(MAKEFILE) $(OBJOVLWIZ) $(LDOVLWIZFILE)
	$(CC) $(LDOVLWIZFLAGS) $(OBJOVLWIZ) $(LIBSOVL) raze/raze.o -o $@

$(OVLWIZ1) : $(OBJOVLWIZ) $(MAKEFILE) $(LDOVLWIZFILE)
	$(CONV) -O binary $(OVLWIZ) $(OVLWIZ1)

$(OVLSLPFGHT) : $(OBJOVLSLPFGHT) $(MAKEFILE) $(OBJOVLSLPFGHT) $(LDOVLSLPFGHTFILE)
	$(CC) $(LDOVLSLPFGHTFLAGS) $(OBJOVLSLPFGHT) $(LIBSOVL) -o $@

$(OVLSLPFGHT1) : $(OBJOVLSLPFGHT) $(MAKEFILE) $(LDOVLSLPFGHTFILE)
	$(CONV) -O binary $(OVLSLPFGHT) $(OVLSLPFGHT1)

$(OVLFREEK) : $(OBJOVLFREEK) $(MAKEFILE) $(OBJOVLFREEK) $(LDOVLFREEKFILE)
	$(CC) $(LDOVLFREEKFLAGS) $(OBJOVLFREEK) $(LIBSOVL) -o $@

$(OVLFREEK1) : $(OBJOVLFREEK) $(MAKEFILE) $(LDOVLFREEKFILE)
	$(CONV) -O binary $(OVLFREEK) $(OVLFREEK1)

$(OVLMSX) : $(OBJOVLMSX) $(MAKEFILE) $(OBJOVLMSX) $(LDOVLMSXFILE)
#	$(CC) $(LDOVLMSXFLAGS) $(OBJOVLMSX) $(LIBSOVL)  -o $@
	$(CC) $(LDOVLMSXFLAGS) $(OBJOVLMSX) $(LIBSOVL)  raze/raze.o -o $@

$(OVLMSX1) : $(OBJOVLMSX) $(MAKEFILE) $(LDOVLMSXFILE)
	$(CONV) -O binary $(OVLMSX) $(OVLMSX1)

$(OVLSEGAE) : $(OBJOVLSEGAE) $(MAKEFILE) $(OBJOVLSEGAE) $(LDOVLSEGAEFILE)
	$(CC) $(LDOVLSEGAEFLAGS) $(OBJOVLSEGAE) $(LIBSOVL) -o $@

$(OVLSEGAE1) : $(OBJOVLSEGAE) $(MAKEFILE) $(LDOVLSEGAEFILE)
	$(CONV) -O binary $(OVLSEGAE) $(OVLSEGAE1)

$(OVLSOLOMN) : $(OBJOVLSOLOMN) $(MAKEFILE) $(OBJOVLSOLOMN) $(LDOVLSOLOMNFILE)
	$(CC) $(LDOVLSOLOMNFLAGS) $(OBJOVLSOLOMN) $(LIBSOVL) raze/raze.o -o $@

$(OVLSOLOMN1) : $(OBJOVLSOLOMN) $(MAKEFILE) $(LDOVLSOLOMNFILE)
	$(CONV) -O binary $(OVLSOLOMN) $(OVLSOLOMN1)

# suffix
.SUFFIXES: .asm

burn.o: burn.c
	$(CC) $< $(DFLAGS) $(CCFLAGS2) -o $@

saturn/saturn.o: saturn/saturn.c
	$(CC) $< $(DFLAGS) $(CCFLAGS2) -o $@

saturn/sc_saturn_vbt.o: saturn/sc_saturn_vbt.c
	$(CC) $< $(DFLAGS) $(CCFLAGS2) -o $@

saturn/file.o: saturn/file.c
	$(CC) $< $(DFLAGS) $(CCFLAGS2) -o $@

saturn/font.o: saturn/font.c
	$(CC) $< $(DFLAGS) $(CCFLAGS2) -o $@
.c.o:
	$(CC) $< $(DFLAGS) $(EXTRA_FLAGS) $(CCOVLFLAGS) -o $@
sms/%.o : %.c
	$(CC) $< $(DFLAGS) -DRAZE=1 $(CCOVLFLAGS) -o $@
gg/%.o : %.c
	$(CC) $< $(DFLAGS) -DRAZE=1 -DGG=1 $(CCOVLFLAGS) -o $@
ggcz/%.o : %.c
	$(CC) $< $(DFLAGS) -DGG=1 $(CCOVLFLAGS) -o $@

clean:
	$(RM) $(OBJS) $(TARGET:.coff=.*)
