##########################################################################
#   Instructions to Make, for compilation of ISODE processes
##########################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/RCS/Makefile,v 9.0 1992/06/16 12:05:01 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:05:01  isode
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


##################################################################
# Here it is...
##################################################################

LIBDIRS	=	h compat dirent tsap ssap psap pepsy pepy psap2 \
		acsap rosap rtsap ronot rosy 
DIRS	=	$(LIBDIRS) support imisc
OTHERS	=	dsap ftam ftam2 ftam-ftp ftp-ftam others psap2-lpp quipu \
		snmp vt


all:;		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) all"; \
			      cd $$i; $(MAKE) all); \
		    done

inst-all:	make-dirs
		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) inst-all"; \
			      cd $$i; $(MAKE) inst-all); \
		    done

make-dirs:;	@for i in $(BINDIR) $(SBINDIR) $(ETCDIR) $(LOGDIR) \
			$(LIBDIR) $(LINTDIR) ; \
		    do (echo $$i X | \
			sed -e 's%\(.*\)/ X%if test ! -d \1; then mkdir \1; chmod 755 \1; else true; fi%' | \
			sh -v); \
		    done
		@-echo $(LOGDIR) X | \
			sed -e 's%\(.*\)/ X%chmod 777 \1%' | \
			sh -v
		

install:	inst-all clean

lint:;		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) lint"; \
			      cd $$i; $(MAKE) lint); \
		    done

clean:;		rm -f *.a *.so* llib-l* _* *.tmp config/_* util/inst-man.sh
		@for i in $(DIRS) $(OTHERS); \
		    do (echo "cd $$i; $(MAKE) clean"; \
			      cd $$i; $(MAKE) clean); \
		    done

test:;		PATH=$$PATH:${BINDIR} sh isode-test

grind:;		@for i in $(DIRS) $(OTHERS); \
		    do (echo "cd $$i; $(MAKE) grind"; \
			      cd $$i; $(MAKE) grind); \
		    done

once-only:;	@touch h/*.h h/quipu/*.h
		@touch dsap/dsaptailor dsap/oidtable.*.local quipu/quiputailor

distribution:	clean once-only READ-ME COSINE
		@cd doc; make clean

READ-ME:	isode-gen.8
		nroff -man isode-gen.8 > isode-gen.tmp
		mv isode-gen.tmp $@

#QUIPU-READ-ME:	quipu-new.8
#		nroff -ms quipu-new.8 > quipu-new.tmp
#		mv quipu-new.tmp $@

inst-partial:;	cd h; $(MAKE) inst-all
		cd compat; $(MAKE) inst-all
		cd pepsy; $(MAKE) inst-all
		cd pepy; $(MAKE) inst-all
		cd support; $(MAKE) macros inst-libisode


##################################################################
# everything
##################################################################

everything:	all true
		./make all-ftam all-ftam-ftp all-vt all-quipu all-snmp \
			all-lpp
		@echo "a great album by mtr's favorite rock band"
		@echo "sh \"whois -name mtr -info 'favorite rock band'\""

inst-everything: inst-all true
		./make inst-ftam inst-ftam-ftp inst-vt inst-quipu inst-snmp \
			inst-lpp

lint-everything: lint true
		./make lint-ftam lint-ftam-ftp lint-vt lint-quipu lint-snmp \
			lint-lpp 

true:;


##################################################################
# ftam
##################################################################

FTAM	=	ftam ftam2


all-ftam:;	@for i in $(FTAM); \
		    do (echo "cd $$i; $(MAKE) all"; \
			      cd $$i; $(MAKE) all); \
		    done

inst-ftam:;	@for i in $(FTAM); \
		    do (echo "cd $$i; $(MAKE) inst-all"; \
			      cd $$i; $(MAKE) inst-all); \
		    done

install-ftam:;	@for i in $(FTAM); \
		    do (echo "cd $$i; $(MAKE) install"; \
			      cd $$i; $(MAKE) install); \
		    done

lint-ftam:;	@for i in $(FTAM); \
		    do (echo "cd $$i; $(MAKE) lint"; \
			      cd $$i; $(MAKE) lint); \
		    done


##################################################################
# ftam-ftp
##################################################################

FTAM-FTP=	ftam-ftp ftp-ftam


all-ftam-ftp:;	@for i in $(FTAM-FTP); \
		    do (echo "cd $$i; $(MAKE) all"; \
			      cd $$i; $(MAKE) all); \
		    done

inst-ftam-ftp:;	@for i in $(FTAM-FTP); \
		    do (echo "cd $$i; $(MAKE) inst-all"; \
			      cd $$i; $(MAKE) inst-all); \
		    done

install-ftam-ftp:;	@for i in $(FTAM-FTP); \
		    do (echo "cd $$i; $(MAKE) install"; \
			      cd $$i; $(MAKE) install); \
		    done

lint-ftam-ftp:;	@for i in $(FTAM-FTP); \
		    do (echo "cd $$i; $(MAKE) lint"; \
			      cd $$i; $(MAKE) lint); \
		    done


##################################################################
# vt
##################################################################

VT	=	vt


all-vt:;	@for i in $(VT); \
		    do (echo "cd $$i; $(MAKE) all"; \
			      cd $$i; $(MAKE) all); \
		    done

inst-vt:;	@for i in $(VT); \
		    do (echo "cd $$i; $(MAKE) inst-all"; \
			      cd $$i; $(MAKE) inst-all); \
		    done

install-vt:;	@for i in $(VT); \
		    do (echo "cd $$i; $(MAKE) install"; \
			      cd $$i; $(MAKE) install); \
		    done

lint-vt:;	@for i in $(VT); \
		    do (echo "cd $$i; $(MAKE) lint"; \
			      cd $$i; $(MAKE) lint); \
		    done


##################################################################
# quipu
##################################################################

QUIPU	=	dsap quipu


all-quipu:;	cd h/quipu; $(MAKE) all
		@for i in $(QUIPU); \
		    do (echo "cd $$i; $(MAKE) all"; \
			      cd $$i; $(MAKE) all); \
		    done
		cd acsap; $(MAKE) dased
		cd support; $(MAKE) iaed

inst-quipu:;	cd h/quipu; $(MAKE) inst-all
		@for i in $(QUIPU); \
		    do (echo "cd $$i; $(MAKE) inst-all"; \
			      cd $$i; $(MAKE) inst-all); \
		    done
		cd acsap; $(MAKE) inst-dased man-dased
		cd support; $(MAKE) inst-iaed man-iaed

install-quipu:;	cd h/quipu; $(MAKE) install
		@for i in $(QUIPU); \
		    do (echo "cd $$i; $(MAKE) install"; \
			      cd $$i; $(MAKE) install); \
		    done
		cd acsap; $(MAKE) inst-dased man-dased
		cd support; $(MAKE) inst-iaed man-iaed

lint-quipu:;	cd h/quipu; $(MAKE) lint
		@for i in $(QUIPU); \
		    do (echo "cd $$i; $(MAKE) lint"; \
			      cd $$i; $(MAKE) lint); \
		    done		
		cd acsap; $(MAKE) l-dased
		cd support; $(MAKE) l-iaed


##################################################################
# snmp
##################################################################

SNMP	=	snmp


all-snmp:;	@for i in $(SNMP); \
		    do (echo "cd $$i; $(MAKE) all"; \
			      cd $$i; $(MAKE) all); \
		    done

inst-snmp:;	@for i in $(SNMP); \
		    do (echo "cd $$i; $(MAKE) inst-all"; \
			      cd $$i; $(MAKE) inst-all); \
		    done

install-snmp:;	@for i in $(SNMP); \
		    do (echo "cd $$i; $(MAKE) install"; \
			      cd $$i; $(MAKE) install); \
		    done

lint-snmp:;	@for i in $(SNMP); \
		    do (echo "cd $$i; $(MAKE) lint"; \
			      cd $$i; $(MAKE) lint); \
		    done


##################################################################
# lpp
##################################################################

LPP	=	h compat psap pepsy pepy acsap rosap ronot rosy psap2-lpp
LPP-AUX	=	support imisc


all-lpp:;	@for i in $(LPP); \
		    do (echo "cd $$i; $(MAKE) all"; \
			      cd $$i; $(MAKE) all); \
		    done
		@for i in $(LPP-AUX); \
		    do (echo "cd $$i; $(MAKE) all-lpp"; \
			      cd $$i; $(MAKE) all-lpp); \
		    done

inst-lpp:;	@for i in $(LPP); \
		    do (echo "cd $$i; $(MAKE) inst-all"; \
			      cd $$i; $(MAKE) inst-all); \
		    done
		@for i in $(LPP-AUX); \
		    do (echo "cd $$i; $(MAKE) inst-lpp"; \
			      cd $$i; $(MAKE) inst-lpp); \
		    done

install-lpp:;	@for i in $(LPP); \
		    do (echo "cd $$i; $(MAKE) install"; \
			      cd $$i; $(MAKE) install); \
		    done
		@for i in $(LPP-AUX); \
		    do (echo "cd $$i; $(MAKE) install-lpp"; \
			      cd $$i; $(MAKE) install-lpp); \
		    done

lint-lpp:;	@for i in $(LPP); \
		    do (echo "cd $$i; $(MAKE) lint"; \
			      cd $$i; $(MAKE) lint); \
		    done
		@for i in $(LPP-AUX); \
		    do (echo "cd $$i; $(MAKE) lint-lpp"; \
			      cd $$i; $(MAKE) lint-lpp); \
		    done


##################################################################
# COSINE PARADISE project
##################################################################

PARADISEDUA =		others/quipu/uips/de

paradise:		paradise-dsa paradise-dua

inst-paradise:		inst-paradise-dsa inst-paradise-dua

install-paradise:	inst-paradise-dsa inst-paradise-dua clean

paradise-isode:;	@for i in $(LIBDIRS); \
				do (echo "cd $$i; $(MAKE) all"; \
				cd $$i; $(MAKE) all); \
			done
			touch support/macros.local support/objects.local
			cd support; $(MAKE) libisode isomacros isobjects
			cd dsap; $(MAKE) all

paradise-inst-isode:	make-dirs
			cd support; $(MAKE) $(LIBDIR)libisode.a macros objects
			cd dsap; $(MAKE) $(LIBDIR)libdsap.a \
			   	 inst-dsaptailor inst-oidtable
		
paradise-dsa:		paradise-isode
			cd quipu; $(MAKE) all

inst-paradise-dsa:	paradise-inst-isode
			cd quipu; $(MAKE) inst-all

install-paradise-dsa:	inst-paradise-dua clean

paradise-dua:		paradise-isode
			@for i in $(PARADISEDUA); \
			do (echo "cd $$i; $(MAKE) all"; \
			      	cd $$i; $(MAKE) all); \
			done

inst-paradise-dua:	paradise-inst-isode
			cd $(PARADISEDUA); $(MAKE) inst-all

install-paradise-dua:	inst-paradise-dua clean

COSINE:			paradise-gen.8
			nroff -man paradise-gen.8 > paradise-gen.tmp
			mv paradise-gen.tmp $@

##################################################################
# image
##################################################################

image:;		find . -name RCS -a -exec rm -rf {} \; -a -prune
		find . -name version.local -exec rm -f {} \;
		find . -name '*.rej' -o -name '*.orig' -o -name '*.log' \
		     -o -name core -o -name '*~' -o -name '#*#' -print
		rm -f CHECK-OUT TODO config/CONFIG.make h/config.h \
			support/aliases.local support/entities.local \
			support/macros.local support/objects.local \
			support/services.local timestamp*
		cp dsap/common/protected.c /tmp/protected.c
		-unifdef -UHAVE_PROTECTED < /tmp/protected.c \
			> dsap/common/protected.c
		rm -f /tmp/protected.c


##################################################################
# zap
##################################################################

zap:;	rm -rf $(INCDIRM)
	rm -f $(BINDIR)pepy  \
	    $(BINDIR)posy    \
	    $(BINDIR)rosy
	rm -f $(LIBDIR)libicompat.a \
	    $(LIBDIR)libdirent.a   \
	    $(LIBDIR)libtsap.a     \
	    $(LIBDIR)libssap.a     \
	    $(LIBDIR)libpsap.a     \
	    $(LIBDIR)libpepy.a     \
	    $(LIBDIR)libpsap2.a    \
	    $(LIBDIR)libacsap.a    \
	    $(LIBDIR)librtsap.a    \
	    $(LIBDIR)libronot.a    \
	    $(LIBDIR)librosap.a    \
	    $(LIBDIR)librosy.a     \
	    $(LIBDIR)libisode.a    \
	    $(LIBDIR)libdsap.a     \
	    $(LIBDIR)libquipu.a
	rm -f $(LINTDIR)llib-licompat* \
	    $(LINTDIR)llib-ldirent*   \
	    $(LINTDIR)llib-ltsap*     \
	    $(LINTDIR)llib-lssap*     \
	    $(LINTDIR)llib-lpsap*     \
	    $(LINTDIR)llib-lpsap2*    \
	    $(LINTDIR)llib-lacsap*    \
	    $(LINTDIR)llib-lrtsap*    \
	    $(LINTDIR)llib-lronot*    \
	    $(LINTDIR)llib-lrosap*    \
	    $(LINTDIR)llib-lrosy*     \
	    $(LINTDIR)llib-lisode*    \
	    $(LINTDIR)llib-ldsap*     \
	    $(LINTDIR)llib-lquipu*
