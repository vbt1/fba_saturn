#
#   SEGA SATURN Graphic library make file for GNU

# macro
CC = sh-elf-gcc
AS = sh-elf-as
LD = sh-elf-ld
RM = rm
CONV = sh-elf-objcopy

MAKEFILE = Makefile  # -fuse-linker-plugin -flto 
#CCFLAGS2 = -m2 -Os -Wall -Wextra --save-temps -ffreestanding -fno-web -fno-unit-at-a-time -Wl,--verbose -Wl,--allow-multiple-definition -mno-fsrra -maccumulate-outgoing-args -std=gnu99 -Wfatal-errors -fno-exceptions -D_SH -DMODEL_S -c -I. -Il:/saturn/SBL6/SEGALIB/INCLUDE
CCFLAGS2 = -m2 -Os -Wall --save-temps -Wno-missing-braces -Wextra -fno-unit-at-a-time -Wl,--verbose -Wl,--allow-multiple-definition -std=gnu99 -Wfatal-errors -fno-exceptions -D_SH -DMODEL_S -c -I. -Il:/saturn/SBL6/SEGALIB/INCLUDE

CCOVLFLAGS = -m2 -O2 -Wall --save-temps -Wno-array-bounds -Wno-missing-braces -Wextra -fno-web -fno-unit-at-a-time -Wl,--strip-all -Wl,--verbose -Wl,--allow-multiple-definition -mno-fsrra -maccumulate-outgoing-args -std=gnu99 -Wfatal-errors -fomit-frame-pointer -D_SH -DMODEL_S -c -Il:/saturn/SBL6/SEGALIB/INCLUDE

OLVSCRIPT = root/overlay.lnk
#LDCMNFLAGS = -m2 -O2 -flto -fuse-linker-plugin  -Xlinker -n -Xlinker -S -Xlinker
LDCMNFLAGS = -m2  -O2 -v -Xlinker -n -Xlinker -S -Xlinker -fno-lto -Xlinker
LDCMNFLAGS2 = -m2  -Os -v -Xlinker -S -Xlinker -fno-lto -Xlinker

TARGET    = root/sl.elf
TARGET1  = root/sl.bin
LDFILE	 = ./$(TARGET:.elf=.lnk)
MPFILE     = $(TARGET:.elf=.maps)
LDFLAGS = $(LDCMNFLAGS2) -T$(LDFILE) -Xlinker -Map -Xlinker $(MPFILE) -Xlinker -e -Xlinker 0x6004000 -nostartfiles
SRCS       = saturn/low.s burn.c saturn/font.c saturn/file.c saturn/saturn.c 
OBJS2     = saturn/strt1_vbt.o strt/strt2_g.o ../../SBL6/SEGASMP/PER/SMPCLIB/per_x12.o ../../SBL6/SEGASMP/PER/SMPCLIB/per_x22.o $(SRCS:.c=.o)
#OBJS2     = strt/strt1_g.o strt/strt2_g.o ../../SBL6/SEGASMP/PER/SMPCLIB/per_x12.o ../../SBL6/SEGASMP/PER/SMPCLIB/per_x22.o $(SRCS:.c=.o)

OVLIMG                 = root/img.elf
OVLIMG1               = root/img.bin
LDOVLIMGFILE     = ./$(OVLIMG:.elf=.lnk)
MPOVLIMGFILE    = $(OVLIMG:.elf=.maps)
LDOVLIMGFLAGS = $(LDCMNFLAGS2) -T$(LDOVLIMGFILE) -Xlinker -Map -Xlinker $(MPOVLIMGFILE) -Xlinker -e -Xlinker _ovlstart -nostartfiles
SRCOVLIMG         = saturn/img.c libyaul/libtga/tga.c
OBJOVLIMG         = $(SRCOVLIMG:.c=.o)

OVERLAY	       = root/d_bankp.elf
OVERLAY1     = root/d_bankp.bin
MPOVLFILE    = $(OVERLAY:.elf=.maps)
LDOVLFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLFILE) -Xlinker -e -Xlinker 0x060DE000 -nostartfiles
SRCOVL         = d_bankp.c snd/sn76496.c load.c saturn/ovl.c
#SRCOVL         = d_bankp.c czet.c cz80/cz80.c snd/sn76496.c saturn/ovl.c
OBJOVL         = $(SRCOVL:.c=.o)

OVLNEWS                 = root/d_news.elf
OVLNEWS1               = root/d_news.bin
MPOVLNEWSFILE    = $(OVLNEWS:.elf=.maps)
LDOVLNEWSFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLNEWSFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLNEWS         = d_news.c czet.c cz80/cz80.c snd/msm6295.c burn_sound_c.c load.c saturn/ovl.c
OBJOVLNEWS         = $(SRCOVLNEWS:.c=.o)

OVLGBERET                 = root/d_gberet.elf
OVLGBERET1               = root/d_gberet.bin
MPOVLGBERETFILE    = $(OVLGBERET:.elf=.maps)
LDOVLGBERETFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLGBERETFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLGBERET         = d_gberet.c czet.c cz80/cz80.c snd/sn76496.c load.c saturn/ovl.c 
OBJOVLGBERET         = $(SRCOVLGBERET:.c=.o)

OVLHIGEMARU                 = root/d_higemaru.elf
OVLHIGEMARU1               = root/d_higema.bin
MPOVLHIGEMARUFILE    = $(OVLHIGEMARU:.elf=.maps)
LDOVLHIGEMARUFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLHIGEMARUFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLHIGEMARU         = d_higemaru.c czet.c cz80/cz80.c snd/ay8910.c  load.c saturn/ovl.c
OBJOVLHIGEMARU         = $(SRCOVLHIGEMARU:.c=.o)

OVLPKUNW                 = root/d_pkunw.elf
OVLPKUNW1               = root/d_pkunw.bin
MPOVLPKUNWFILE    = $(OVLPKUNW:.elf=.maps)
LDOVLPKUNWFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLPKUNWFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLPKUNW         = d_pkunwar.c czet.c cz80/cz80.c snd/ay8910.c load.c saturn/ovl.c
OBJOVLPKUNW         = $(SRCOVLPKUNW:.c=.o)

OVLMITCH                 = root/d_mitch.elf
OVLMITCH1               = root/d_mitch.bin
MPOVLMITCHFILE    = $(OVLMITCH:.elf=.maps)
LDOVLMITCHFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLMITCHFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLMITCH         = d_mitchell.c czet.c cz80/cz80.c kabuki.c snd/msm6295.c burn_sound_c.c eeprom.c load.c saturn/ovl.c saturn/saturn_snd.c 
#saturn/pcmsys.c
OBJOVLMITCH         = $(SRCOVLMITCH:.c=.o)

OVLGNG                 = root/d_gng.elf
OVLGNG1               = root/d_gng.bin
MPOVLGNGFILE    = $(OVLGNG:.elf=.maps)
LDOVLGNGFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLGNGFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLGNG         = m6809_intf.c m6809.c  d_gng.c load.c saturn/ovl.c
OBJOVLGNG         = $(SRCOVLGNG:.c=.o)

OVLSYS1                 = root/d_sys1.elf
OVLSYS11               = root/d_sys1.bin
MPOVLSYS1FILE    = $(OVLSYS1:.elf=.maps)
LDOVLSYS1FLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSYS1FILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLSYS1         = d_sys1.c czet.c cz80sys1/cz80.c snd/sn76496.c load.c saturn/ovl.c 
OBJOVLSYS1         = $(SRCOVLSYS1:.c=.o)

OVLSYS1H                 = root/d_SYS1H.elf
OVLSYS1H1               = root/d_SYS1H.bin
MPOVLSYS1HFILE    = $(OVLSYS1H:.elf=.maps)
LDOVLSYS1HFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSYS1HFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLSYS1H         = d_sys1h.c czet.c cz80sys1/cz80.c snd/sn76496.c mc8123.c load.c saturn/ovl.c saturn/saturn_ext.c
OBJOVLSYS1H         = $(SRCOVLSYS1H:.c=.o)

OVLSYS2                 = root/d_sys2.elf
OVLSYS21               = root/d_sys2.bin
MPOVLSYS2FILE    = $(OVLSYS2:.elf=.maps)
LDOVLSYS2FLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSYS2FILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLSYS2         = d_sys2.c czet.c cz80sys1/cz80.c snd/sn76496.c 8255ppi.c mc8123.c load.c saturn/ovl.c saturn/saturn_ext.c
OBJOVLSYS2         = $(SRCOVLSYS2:.c=.o)

OVLPACM                 = root/d_pacm.elf
OVLPACM1               = root/d_pacm.bin
MPOVLPACMFILE    = $(OVLPACM:.elf=.maps)
LDOVLPACMFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLPACMFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLPACM         = d_pacman.c czet.c snd/namco_snd.c cz80/cz80.c load.c saturn/ovl.c
#SRCOVLPACM         = d_pacman_zet.c zet.c z80.c z80daisy.c namco_snd.c 
OBJOVLPACM         = $(SRCOVLPACM:.c=.o)

OVLSMS                 = root/d_sms.elf 
OVLSMS1               = root/d_sms.bin
MPOVLSMSFILE    = $(OVLSMS:.elf=.maps)
LDOVLSMSFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSMSFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLSMS         = d_sms.c psg_sms.c 
OBJOVLSMS         = $(SRCOVLSMS:d_sms.c=sms/d_sms.o)

OVLSMSCZ80                 = root/d_smscz.elf
OVLSMSCZ801               = root/d_smscz.bin
MPOVLSMSCZ80FILE    = $(OVLSMSCZ80:.elf=.maps)
LDOVLSMSCZ80FLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSMSCZ80FILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLSMSCZ80         = d_sms.c psg_sms.c czet.c cz80/cz80.c
OBJOVLSMSCZ80         = $(SRCOVLSMSCZ80:.c=.o)

OVLGG                = root/d_gg.elf
OVLGG1              = root/d_gg.bin
MPOVLGGFILE    = $(OVLGG:.elf=.maps)
LDOVLGGFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLGGFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLGG         = d_sms.c psg_sms.c 
OBJOVLGG         = $(SRCOVLGG:d_sms.c=gg/d_sms.o)

OVLGGCZ                = root/d_ggcz.elf
OVLGGCZ1              = root/d_ggcz.bin
MPOVLGGCZFILE    = $(OVLGGCZ:.elf=.maps)
LDOVLGGCZFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLGGCZFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLGGCZ         = psg_sms.c
SRCOVLGGCZ2        = d_sms.c
OBJOVLGGCZ         = $(SRCOVLGGCZ::.c=.o) $(SRCOVLGGCZ2:d_sms.c=ggcz/d_sms.o) 

OVLZAXXON                 = root/d_zaxxon.elf
OVLZAXXON1               = root/d_zaxxon.bin
MPOVLZAXXONFILE    = $(OVLZAXXON:.elf=.maps)
LDOVLZAXXONFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLZAXXONFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLZAXXON         = d_zaxxon.c czet.c cz80/cz80.c load.c saturn/ovl.c
#SRCOVLZAXXON       = d_zaxxon.c saturn/ovl.c d_zaxxon.s
OBJOVLZAXXON         = $(SRCOVLZAXXON:.c=.o)

OVLTETRIS                 = root/d_tetris.elf
OVLTETRIS1               = root/d_tetris.bin
MPOVLTETRISFILE    = $(OVLTETRIS:.elf=.maps)
LDOVLTETRISFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLTETRISFILE) -Xlinker -e -Xlinker boot -nostartfiles
#SRCOVLTETRIS         = d_atetris_crab6502.c crab6502/crab6502.c crab6502/m6502_intf.c snd/sn76496.c slapstic.c
#SRCOVLTETRIS         = d_atetris_mame6502.c m6502.new/m6502.c m6502.new/m6502_intf.c snd/sn76496.c slapstic.c load.c saturn/ovl.c
SRCOVLTETRIS         = d_atetris_mame6502.c m6502/m6502.c m6502/m6502_intf.c slapstic.c load.c saturn/ovl.c
#SRCOVLTETRIS         = d_atetris_mame6502.c lib6502/lib6502.c lib6502/lib6502_intf.c slapstic.c load.c saturn/ovl.c
#SRCOVLTETRIS         = d_atetris.c m6502/m6502.c m6502_intf.c snd/sn76496.c slapstic.c
#SRCOVLTETRIS         = d_atetris.c m6502/m6502.c m6502_intf.c snd/sn76496.c slapstic.c
#SRCOVLTETRIS         = d_atetris.c Crab6502/Crab6502.c snd/sn76496.c slapstic.c load.c saturn/ovl.c
OBJOVLTETRIS         = $(SRCOVLTETRIS:.c=.o)

OVLVIGIL                 = root/d_vigil.elf
OVLVIGIL1               = root/d_vigil.bin
MPOVLVIGILFILE    = $(OVLVIGIL:.elf=.maps)
LDOVLVIGILFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLVIGILFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLVIGIL         = d_vigilant.c czet.c cz80/cz80.c load.c saturn/ovl.c 
# snd/dac.c snd/ym2151.c
#SRCOVLVIGIL         = d_vigilant.c z80_intf.c z80.c z80daisy.c burn_ym2151.c ym2151.c dac.c burn_sound_c.c load.c saturn/ovl.c
OBJOVLVIGIL         = $(SRCOVLVIGIL:.c=.o)

OVLSG1000                 = root/d_sg1000.elf
OVLSG10001               = root/d_sg1000.bin
MPOVLSG1000FILE    = $(OVLSG1000:.elf=.maps)
LDOVLSG1000FLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSG1000FILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLSG1000         = d_sg1000.c 8255ppi.c tms9928a.c snd/sn76496.c czet.c cz80/cz80.c load.c saturn/ovl.c
#SRCOVLSG1000         = d_sg1000.c 8255ppi.c tms9928a.c snd/sn76496.c saturn/ovl.c
OBJOVLSG1000         = $(SRCOVLSG1000:.c=.o)

OVLBOMBJACK                 = root/d_bombja.elf
OVLBOMBJACK1               = root/d_bombja.bin
MPOVLBOMBJACKFILE    = $(OVLBOMBJACK:.elf=.maps)
LDOVLBOMBJACKFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLBOMBJACKFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLBOMBJACK         = d_bombjack.c czet.c cz80/cz80.c snd/ay8910.c load.c saturn/ovl.c
OBJOVLBOMBJACK         = $(SRCOVLBOMBJACK:.c=.o)

OVLAPPOOO                 = root/d_appooo.elf
OVLAPPOOO1               = root/d_appooo.bin
MPOVLAPPOOOFILE    = $(OVLAPPOOO:.elf=.maps)
LDOVLAPPOOOFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLAPPOOOFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLAPPOOO         = d_appoooh.c czet.c cz80/cz80.c snd/sn76496.c snd/msm5205.c load.c saturn/ovl.c
OBJOVLAPPOOO         = $(SRCOVLAPPOOO:.c=.o)

OVLBLKTGR                 = root/d_blktgr.elf
OVLBLKTGR1               = root/d_blktgr.bin
MPOVLBLKTGRFILE    = $(OVLBLKTGR:.elf=.maps)
LDOVLBLKTGRFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLBLKTGRFILE) -Xlinker -e -Xlinker boot -nostartfiles
#SRCOVLBLKTGR         = d_blktiger.c czet.c cz80/cz80.c snd/timer.c snd/ay8910.c snd/fm.c snd/burn_ym2203.c load.c saturn/ovl.c
SRCOVLBLKTGR         = d_blktiger.c load.c saturn/ovl.c saturn/saturn_snd.c
#SRCOVLBLKTGR         = d_blktiger.c czet.c cz80/cz80.c snd/timer.c load.c saturn/ovl.c
OBJOVLBLKTGR         = $(SRCOVLBLKTGR:.c=.o)

OVLWIZ                = root/d_wiz.elf
OVLWIZ1              = root/d_wiz.bin
MPOVLWIZFILE    = $(OVLWIZ:.elf=.maps)
LDOVLWIZFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLWIZFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLWIZ         = d_wiz.c czet.c cz80/cz80.c snd/ay8910.c load.c saturn/ovl.c saturn/saturn_ext.c
OBJOVLWIZ         = $(SRCOVLWIZ:.c=.o)

OVLSLPFGHT                = root/d_slpfgh.elf
OVLSLPFGHT1              = root/d_slpfgh.bin
MPOVLSLPFGHTFILE    = $(OVLSLPFGHT:.elf=.maps)
LDOVLSLPFGHTFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSLPFGHTFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLSLPFGHT         = d_slpfgh.c czet.c cz80/cz80.c snd/ay8910.c load.c saturn/ovl.c
OBJOVLSLPFGHT         = $(SRCOVLSLPFGHT:.c=.o)

OVLFREEK                 = root/d_freek.elf
OVLFREEK1               = root/d_freek.bin
MPOVLFREEKFILE    = $(OVLFREEK:.elf=.maps)
LDOVLFREEKFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLFREEKFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLFREEK         = d_freekick.c czet.c cz80/cz80.c snd/sn76496.c mc8123.c 8255ppi.c load.c saturn/ovl.c
OBJOVLFREEK         = $(SRCOVLFREEK:.c=.o)

OVLMSX                 = root/d_msx.elf
OVLMSX1               = root/d_msx.bin
MPOVLMSXFILE    = $(OVLMSX:.elf=.maps)
LDOVLMSXFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLMSXFILE) -Xlinker -e -Xlinker boot -nostartfiles
#SRCOVLMSX         = d_msx.c czet.c cz80/cz80.c tms9928a.c snd/k051649.c snd/ay8910.c 8255ppi.c saturn/ovl.c
#SRCOVLMSX         = d_msx.c z80_intf.c z80/z80.c z80/z80daisy.c tms9928a.c snd/ay8910.c snd/dac.c 8255ppi.c saturn/ovl.c
SRCOVLMSX         = d_msx.c tms9928a.c snd/k051649.c snd/ay8910.c 8255ppi.c load.c saturn/ovl.c
OBJOVLMSX         = $(SRCOVLMSX:.c=.o)

OVLSEGAE                 = root/d_segae.elf
OVLSEGAE1               = root/d_segae.bin
MPOVLSEGAEFILE    = $(OVLSEGAE:.elf=.maps)
LDOVLSEGAEFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSEGAEFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLSEGAE         = d_segae.c czet.c cz80/cz80.c snd/sn76496.c mc8123.c load.c saturn/ovl.c 
#saturn/saturn_ext.c 
OBJOVLSEGAE         = $(SRCOVLSEGAE:.c=.o)

OVLSOLOMN                = root/d_solomn.elf
OVLSOLOMN1              = root/d_solomn.bin
MPOVLSOLOMNFILE    = $(OVLSOLOMN:.elf=.maps)
LDOVLSOLOMNFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSOLOMNFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLSOLOMN         = d_solomon.c czet.c cz80/cz80.c snd/ay8910.c load.c saturn/ovl.c
OBJOVLSOLOMN         = $(SRCOVLSOLOMN:.c=.o)

OVLSIDARM                 = root/d_sidarm.elf
OVLSIDARM1               = root/d_sidarm.bin
MPOVLSIDARMFILE    = $(OVLSIDARM:.elf=.maps)
LDOVLSIDARMFLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLSIDARMFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLSIDARM         = d_sidarm.c czet.c cz80/cz80.c saturn/ovl.c load.c saturn/saturn_snd.c saturn/saturn_ext.c
OBJOVLSIDARM         = $(SRCOVLSIDARM:.c=.o)

OVLNINKD2                 = root/d_ninkd2.elf
OVLNINKD21               = root/d_ninkd2.bin
MPOVLNINKD2FILE    = $(OVLNINKD2:.elf=.maps)
LDOVLNINKD2FLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLNINKD2FILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLNINKD2         = d_ninjakd2.c czet.c cz80/cz80.c load.c saturn/ovl.c
OBJOVLNINKD2         = $(SRCOVLNINKD2:.c=.o)

OVL1943                 = root/d_1943.elf
OVL19431               = root/d_1943.bin
MPOVL1943FILE    = $(OVL1943:.elf=.maps)
LDOVL1943FLAGS = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVL1943FILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVL1943         = d_1943.c czet.c cz80/cz80.c watchdog.c load.c saturn/ovl.c saturn/saturn_ext.c
OBJOVL1943         = $(SRCOVL1943:.c=.o)

OVLJEDI            = root/d_jedi.elf
OVLJEDI1           = root/d_jedi.bin
MPOVLJEDIFILE      = $(OVLJEDI:.elf=.maps)
LDOVLJEDIFLAGS 	   = $(LDCMNFLAGS) -T$(OLVSCRIPT) -Xlinker -Map -Xlinker $(MPOVLJEDIFILE) -Xlinker -e -Xlinker boot -nostartfiles
SRCOVLJEDI         = d_jedi.c m6502/m6502.c m6502/m6502_intf.c  load.c saturn/ovl.c
OBJOVLJEDI         = $(SRCOVLJEDI:.c=.o)

#YAULMEM = libyaul/kernel/lib/memb.c libyaul/kernel/mm/free.c libyaul/kernel/mm/free_r.c libyaul/kernel/mm/malloc.c libyaul/kernel/mm/malloc_r.c  libyaul/kernel/mm/slob.c libyaul/kernel/mm/realloc_r.c
YAULMEM = libyaul/kernel/lib/memb.c libyaul/kernel/mm/free.c libyaul/kernel/mm/malloc.c libyaul/kernel/mm/slob.c

LIBS2 =  ../../SBL6/SEGALIB/LIB/elf/sega_per.a \
../../SBL6/SEGALIB/LIB/vbtelf4/sega_gfs.a \
../../SBL6/SEGALIB/SPR/vbtelf4/spr_slv.o \
../../SBL6/SEGALIB/LIB/vbtelf4/sega_int.a \
../../SBL6/SEGALIB/LIB/vbtelf4/cdcrep.a \
../../SBL6/SEGALIB/PCM/vbtelf4/pcm_lib.o \
../../SBL6/SEGALIB/PCM/vbtelf4/pcm_mp.o ../../SBL6/SEGALIB/PCM/vbtelf4/pcm_audi.o \
../../SBL6/SEGALIB/PCM/vbtelf4/pcm_drv.o ../../SBL6/SEGALIB/PCM/vbtelf4/pcm_time.o \
../../SBL6/SEGALIB/PCM/vbtelf4/pcm_etc.o ../../SBL6/SEGALIB/PCM/vbtelf4/pcm_aif.o \
../../SBL6/SEGALIB/PCM/vbtelf4/pcm_gfs.o

 LIBSTM = ../../SBL6/SEGALIB/PCM/vbtelf4/pcm_stm.o ../../SBL6/SEGALIB/LIB/vbtelf4/sega_stm.a ../../SBL6/SEGALIB/LIB/vbtelf4/sega_dma.a

LIBSOVL =  ../../SBL6/SEGALIB/LIB/vbtelf4/sega_dma.a
sl: $(TARGET) $(TARGET1) 

drv:  $(OVERLAY)  $(OVERLAY1) $(OVLIMG)  $(OVLIMG1) \
     $(OVLBLKTGR) $(OVLBLKTGR1) $(OVLWIZ) $(OVLWIZ1) \
	 $(OVLNEWS)  $(OVLNEWS1) $(OVLGBERET)  $(OVLGBERET1) \
     $(OVLHIGEMARU) $(OVLHIGEMARU1) $(OVLPKUNW) $(OVLPKUNW1) \
     $(OVLMITCH) $(OVLMITCH1) $(OVLGNG) $(OVLGNG1) \
     $(OVLSYS1) $(OVLSYS11) $(OVLSYS1H) $(OVLSYS1H1) \
     $(OVLSYS2) $(OVLSYS21) $(OVLPACM) $(OVLPACM1) \
     $(OVLAPPOOO) $(OVLAPPOOO1) \
     $(OVLZAXXON) $(OVLZAXXON1)  \
     $(OVLSLPFGHT) $(OVLSLPFGHT1) $(OVLFREEK) $(OVLFREEK1) \
     $(OVLSG1000) $(OVLSG10001) $(OVLBOMBJACK) $(OVLBOMBJACK1) \
     $(OVLMSX) $(OVLMSX1) $(OVLSEGAE) $(OVLSEGAE1) \
     $(OVLSOLOMN) $(OVLSOLOMN1) $(OVLSIDARM) $(OVLSIDARM1) \
     $(OVLNINKD2) $(OVLNINKD21) $(OVL1943) $(OVL19431) \
	 $(OVLTETRIS) $(OVLTETRIS1) \
     $(OVLSMS) $(OVLSMS1) $(OVLGGCZ) $(OVLGGCZ1) \
     $(OVLSMSCZ80) $(OVLSMSCZ801) $(OVLGG) $(OVLGG1) 	 

# Use gcc to link so it will automagically find correct libs directory

$(TARGET) : $(OBJS2) $(MAKEFILE) $(OBJECTS) $(LDFILE)
	$(CC) $(LDFLAGS) $(OBJS2) $(LIBS2) $(LIBSOVL)  -o $@

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
	$(CC) $(LDOVLNEWSFLAGS) $(OBJOVLNEWS) $(LIBSOVL) -o $@

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
	$(CC) $(LDOVLPKUNWFLAGS) $(OBJOVLPKUNW) $(LIBSOVL) -o $@

$(OVLPKUNW1) : $(OBJOVLPKUNW) $(MAKEFILE) $(LDOVLPKUNWFILE)
	$(CONV) -O binary $(OVLPKUNW) $(OVLPKUNW1)

$(OVLMITCH) : $(OBJOVLMITCH) $(MAKEFILE) $(OBJOVLMITCH) $(LDOVLMITCHFILE)
	$(CC) $(LDOVLMITCHFLAGS) $(OBJOVLMITCH) $(LIBSOVL) $(LIBSTM) -o $@

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
#	$(CC) $(LDOVLVIGILFLAGS) $(OBJOVLVIGIL) $(LIBSOVL) -o $@
#	$(CC) $(LDOVLVIGILFLAGS) $(OBJOVLVIGIL) $(LIBSOVL) raze/raze.o -lm -o $@
	$(CC) $(LDOVLVIGILFLAGS) $(OBJOVLVIGIL) $(LIBSOVL) raze/raze.o -o $@

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
	$(CC) $(LDOVLAPPOOOFLAGS) $(OBJOVLAPPOOO) $(LIBSOVL) -o $@

$(OVLAPPOOO1) : $(OBJOVLAPPOOO) $(MAKEFILE) $(LDOVLAPPOOOFILE)
	$(CONV) -O binary $(OVLAPPOOO) $(OVLAPPOOO1)

$(OVLBLKTGR) : $(OBJOVLBLKTGR) $(MAKEFILE) $(OBJOVLBLKTGR) $(LDOVLBLKTGRFILE)
	$(CC) $(LDOVLBLKTGRFLAGS) $(OBJOVLBLKTGR) $(LIBSOVL) $(LIBSTM) raze/raze.o -o $@
#	$(CC) $(LDOVLBLKTGRFLAGS) $(OBJOVLBLKTGR) $(LIBSOVL) $(LIBSTM) raze/raze.o -o $@

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

$(OVLSIDARM) : $(OBJOVLSIDARM) $(MAKEFILE) $(OBJOVLSIDARM) $(LDOVLSIDARMFILE)
	$(CC) $(LDOVLSIDARMFLAGS) $(OBJOVLSIDARM) $(LIBSOVL) $(LIBSTM) -o $@

$(OVLSIDARM1) : $(OBJOVLSIDARM) $(MAKEFILE) $(LDOVLSIDARMFILE)
	$(CONV) -O binary $(OVLSIDARM) $(OVLSIDARM1)

$(OVLNINKD2) : $(OBJOVLNINKD2) $(MAKEFILE) $(OBJOVLNINKD2) $(LDOVLNINKD2FILE)
	$(CC) $(LDOVLNINKD2FLAGS) $(OBJOVLNINKD2) $(LIBSOVL) -o $@

$(OVLNINKD21) : $(OBJOVLNINKD2) $(MAKEFILE) $(LDOVLNINKD2FILE)
	$(CONV) -O binary $(OVLNINKD2) $(OVLNINKD21)

$(OVL1943) : $(OBJOVL1943) $(MAKEFILE) $(OBJOVL1943) $(LDOVL1943FILE)
	$(CC) $(LDOVL1943FLAGS) $(OBJOVL1943) $(LIBSOVL) -o $@

$(OVL19431) : $(OBJOVL1943) $(MAKEFILE) $(LDOVL1943FILE)
	$(CONV) -O binary $(OVL1943) $(OVL19431)

$(OVLJEDI) : $(OBJOVLJEDI) $(MAKEFILE) $(OBJOVLJEDI) $(LDOVLJEDIFILE)
	$(CC) $(LDOVLJEDIFLAGS) $(OBJOVLJEDI) $(LIBSOVL) -o $@

$(OVLJEDI1) : $(OBJOVLJEDI) $(MAKEFILE) $(LDOVLJEDIFILE)
	$(CONV) -O binary $(OVLJEDI) $(OVLJEDI1)

# suffix
.SUFFIXES: .asm

burn.o: burn.c
	$(CC) $< $(DFLAGS) $(CCFLAGS2) -o $@

saturn/low.o: saturn/low.s
	$(CC) $< $(DFLAGS) $(CCFLAGS2) -o $@

saturn/strt1_vbt.o: saturn/strt1_vbt.s
	$(CC) $< $(DFLAGS) $(CCFLAGS2) -o $@
	
saturn/saturn.o: saturn/saturn.c
	$(CC) $< $(DFLAGS) $(CCFLAGS2) -o $@

saturn/sc_saturn_vbt.o: saturn/sc_saturn_vbt.c
	$(CC) $< $(DFLAGS) $(CCFLAGS2) -o $@

saturn/file.o: saturn/file.c
	$(CC) $< $(DFLAGS) $(CCFLAGS2) -o $@

saturn/font.o: saturn/font.c
	$(CC) $< $(DFLAGS) $(CCFLAGS2) -o $@

saturn/img.o: saturn/img.c
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
	$(RM) $(OBJS) $(TARGET:.elf=.*)