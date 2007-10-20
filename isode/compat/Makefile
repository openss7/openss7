###############################################################################
#   Instructions to Make, for compilation of UNIX compatibility library
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/compat/RCS/Makefile,v 9.0 1992/06/16 12:07:00 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:07:00  isode
# Release 8.0
#
# 
###############################################################################

###############################################################################
#
#				 NOTICE
#
#    Acquisition, use, and distribution of this module and related
#    materials are subject to the restrictions of a license agreement.
#    Consult the Preface in the User's Manual for the full terms of
#    this agreement.
#
###############################################################################


LIBES	=	libcompat.a
LLIBS   =
HFILES	=	$(HDIR)manifest.h $(HDIR)general.h $(HDIR)config.h


##################################################################
# Here it is...
##################################################################

all:		libcompat mandefs
inst-all:	inst-mandefs inst-isologs # inst-libcompat manuals
install:	inst-all clean
lint:		l-libcompat


################################################################
# libcompat
################################################################

CFILES	=	general.c logger.c select.c signals.c \
		asprintf.c explode.c implode.c isofiles.c isohost.c sel2str.c \
			serror.c sprintb.c str2elem.c str2sel.c str2vec.c \
			tailor.c \
		baduser.c chkpassword.c getpassword.c putenv.c \
		chrcnv.c cmd_srch.c lexequ.c lexnequ.c log_tai.c nochrcnv.c \
			rcmd_srch.c smalloc.c sstr2arg.c strdup.c \
		isoservent.c servbyname.c servbyport.c servbysel.c \
		isoaddrs.c str2saddr.c str2taddr.c saddr2str.c taddr2str.c\
			 na2norm.c norm2na.c na2str.c pa2str.c nsapcmp.c \
		internet.c internet_tli.c \
		dgram.c \
		camtec.c sunlink.c ubcx25.c \
		ccurx25.c hpuxx25.c ultrix25.c x25addr.c \
		isonsapsnpa.c char2bcd.c bcd2char.c
OFILES =	general.o logger.o select.o signals.o \
		asprintf.o explode.o implode.o isofiles.o isohost.o sel2str.o \
			serror.o sprintb.o str2elem.o str2sel.o str2vec.o \
			tailor.o \
		baduser.o chkpassword.o getpassword.o putenv.o \
		chrcnv.o cmd_srch.o lexequ.o lexnequ.o log_tai.o nochrcnv.o \
			rcmd_srch.o smalloc.o sstr2arg.o strdup.o \
		isoservent.o servbyname.o servbyport.o servbysel.o \
		isoaddrs.o str2saddr.o str2taddr.o saddr2str.o taddr2str.o \
			na2norm.o norm2na.o na2str.o pa2str.o nsapcmp.o \
		internet.o internet_tli.o \
		dgram.o \
		camtec.o sunlink.o ubcx25.o \
		ccurx25.o hpuxx25.o ultrix25.o x25addr.o \
		isonsapsnpa.o char2bcd.o bcd2char.o \
		$(OSTRINGS)

inst-libcompat:	$(LIBDIR)libicompat.a $(LINTDIR)llib-licompat

$(LIBDIR)libicompat.a:	libcompat.a
		-rm -f $@
		cp libcompat.a $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $@ -ranlib
		-@ls -gls $@
		-@echo ""

$(LINTDIR)llib-licompat:	llib-lcompat
		-cp $@ zllib-lcompat
		-rm -f $@
		sed -e 's%#include "\(.*\)"%#include "$(INCDIR)\1"%' \
			< llib-lcompat | \
			sed -e 's%#include "/usr/include/\(.*\)"%#include <\1>%' > $@
		@$(UTILDIR)inst-lint.sh $(SYSTEM) $(OPTIONS) $@
		-@ls -gls $@ $@.ln
		-@echo ""

libcompat:	libcompat.a

libcompat.a:	compatvrsn.o
		-rm -f $@
		@$(UTILDIR)make-lib.sh $(SYSTEM) $(ARFLAGS) $@ $(OFILES) \
			compatvrsn.o
		-@rm -f $(TOPDIR)libcompat.a $(TOPDIR)llib-lcompat
		-@$(LN) libcompat.a $(TOPDIR)libcompat.a
		-@$(LN) llib-lcompat $(TOPDIR)llib-lcompat
		-@ls -l $@
		-@echo "COMPAT library built normally"

compatvrsn.c:	$(OFILES)
		@$(UTILDIR)version.sh compat > $@

l-libcompat:;	$(LINT) $(LFLAGS) -DBINPATH=\"$(BINDIR)\" \
			-DETCPATH=\"$(ETCDIR)\" -DSBINPATH=\"$(SBINDIR)\" \
			-DLOGPATH=\"$(LOGDIR)\" $(CFILES) compatvrsn.c \
			| grep -v "warning: possible pointer alignment problem"

general.o:	$(HFILES)
logger.o:	$(HFILES) $(HDIR)logger.h $(HDIR)tailor.h
select.o:	$(HFILES) $(HDIR)logger.h $(HDIR)tailor.h
signals.o:	$(HFILES)
asprintf.o:	$(HFILES)
explode.o:	$(HFILES)
implode.o:	$(HFILES)
isofiles.o:	$(HFILES) $(HDIR)tailor.h $(HDIR)logger.h
isohost.o:	$(HFILES) $(HDIR)internet.h $(HDIR)tailor.h $(HDIR)logger.h
sel2str.o:	$(HFILES) $(HDIR)isoaddrs.h
serror.o:	$(HFILES)
sprintb.o:	$(HFILES)
str2elem.o:	$(HFILES)
str2sel.o:	$(HFILES) $(HDIR)tailor.h $(HDIR)logger.h
str2vec.o:	$(HFILES)
tailor.o:	$(HFILES) $(HDIR)isoaddrs.h $(HDIR)tailor.h $(HDIR)logger.h \
		    $(TOPDIR)config/CONFIG.make tailor.c
		$(CC) $(LIBCFLAGS) -DBINPATH=\"$(BINDIR)\" \
			-DETCPATH=\"$(ETCDIR)\" -DSBINPATH=\"$(SBINDIR)\" \
			-DLOGPATH=\"$(LOGDIR)\" -c tailor.c
baduser.o:	$(HFILES) $(HDIR)tailor.h $(HDIR)logger.h
chkpassword.o:	$(HFILES)
getpassword.o:	$(HFILES)
putenv.o:	$(HFILES)
chrcnv.o:	$(HFILES)
cmd_srch.o:	$(HFILES) $(HDIR)cmd_srch.h
lexequ.o:	$(HFILES)
lexnequ.o:	$(HFILES)
log_tai.o:	$(HFILES) $(HDIR)logger.h
nochrcnv.o:	$(HFILES)
rcmd_srch.o:	$(HFILES) $(HDIR)cmd_srch.h
smalloc.o:	$(HFILES) $(HDIR)tailor.h
sstr2arg.o:	$(HFILES)
strdup.o:	$(HFILES)
isoservent.o:	$(HFILES) $(HDIR)isoservent.h $(HDIR)tailor.h $(HDIR)logger.h
servbyname.o:	$(HFILES) $(HDIR)isoservent.h $(HDIR)tailor.h $(HDIR)logger.h
servbyport.o:	$(HFILES) $(HDIR)isoservent.h $(HDIR)tailor.h $(HDIR)logger.h
servbysel.o:	$(HFILES) $(HDIR)isoservent.h $(HDIR)tailor.h $(HDIR)logger.h
isoaddrs.o:	$(HFILES) $(HDIR)isoaddrs.h $(HDIR)internet.h $(HDIR)tailor.h \
			$(HDIR)logger.h
str2saddr.o:	$(HFILES) $(HDIR)isoaddrs.h
str2taddr.o:	$(HFILES) $(HDIR)isoaddrs.h
saddr2str.o:	$(HFILES) $(HDIR)isoaddrs.h
taddr2str.o:	$(HFILES) $(HDIR)isoaddrs.h
nsapcmp.o:	$(HFILES) $(HDIR)isoaddrs.h
na2norm.o:	$(HFILES) $(HDIR)isoaddrs.h $(HDIR)internet.h $(HDIR)tailor.h \
			$(HDIR)logger.h
norm2na.o:	$(HDIR)psap.h $(HFILES) $(HDIR)isoaddrs.h $(HDIR)tailor.h \
			$(HDIR)logger.h
na2str.o:	$(HFILES) $(HDIR)isoaddrs.h
pa2str.o:	$(HFILES) $(HDIR)isoaddrs.h
internet.o:	$(HFILES) $(HDIR)internet.h
internet_tli.o:	$(HFILES) $(HDIR)internet.h
dgram.o:	$(HFILES) $(HDIR)tailor.h $(HDIR)logger.h \
			$(HDIR)dgram.h $(HDIR)internet.h $(HDIR)tp4.h
camtec.o:	$(HFILES) $(HDIR)x25.h $(HDIR)tailor.h $(HDIR)logger.h \
			$(HDIR)tpkt.h $(HDIR)tsap.h
sunlink.o:	$(HFILES) $(HDIR)isoaddrs.h $(HDIR)x25.h $(HDIR)tailor.h \
			 $(HDIR)logger.h
ubcx25.o:	$(HFILES) $(HDIR)x25.h $(HDIR)tailor.h $(HDIR)logger.h \
			$(HDIR)tpkt.h $(HDIR)tsap.h
ultrix.o:	$(HFILES) $(HDIR)x25.h $(HDIR)tailor.h $(HDIR)logger.h \
			$(HDIR)tpkt.h $(HDIR)tsap.h
hpuxx25.o:	$(HFILES) $(HDIR)x25.h $(HDIR)tailor.h $(HDIR)logger.h \
			$(HDIR)tpkt.h $(HDIR)tsap.h
x25addr.o:	$(HFILES) $(HDIR)x25.h $(HDIR)tailor.h $(HDIR)logger.h \
			$(HDIR)tpkt.h $(HDIR)tsap.h
char2bcd.o:	$(HFILES)
bcd2char.o:	$(HFILES)

################################################################
# mandefs
################################################################

inst-mandefs:	$(UTILDIR)inst-man.sh

mandefs:	$(UTILDIR)inst-man.sh

$(UTILDIR)inst-man.sh: inst-man.sh Makefile $(TOPDIR)config/CONFIG.make
		-cp $@ zxinst-man.sh
		-rm -f $@
		cat inst-man.sh | \
			sed -e 's%@(BINDIR)%$(BINDIR)%' | \
			sed -e 's%@(ETCDIR)%$(ETCDIR)%' | \
			sed -e 's%@(INCDIR)%$(INCDIR)%' | \
			sed -e 's%@(LOGDIR)%$(LOGDIR)%' | \
			sed -e 's%@(SBINDIR)%$(SBINDIR)%' | \
			sed -e 's%@(MANDIR)%$(MANDIR)%' > $@
		chmod a+x $@
		-@ls -gls $@
		-@echo ""


################################################################
# isologs
################################################################

inst-isologs:	$(SBINDIR)isologs

$(SBINDIR)isologs: isologs.sh Makefile
		-cp $@ zxisologs
		-rm -f $@
		sed -e 's%@(ETCDIR)%$(ETCDIR)%' < isologs.sh | \
			sed -e 's%@(LOGDIR)%$(LOGDIR)%' > $@
		chmod a+rx $@
		-@ls -gls $@
		-@echo ""


################################################################
# manual pages
################################################################

MANUALS	=	libicompat.3

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# clean
################################################################

clean:;		rm -f *.o *.a z* _* core compatvrsn.c 

grind:;		iprint Makefile
		tgrind -lc $(CFILES) compatvrsn.c llib-lcompat
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve
