###############################################################################
#   Instructions to Make, for compilation of ISODE documentation tree
###############################################################################

###############################################################################
#
# $Header: /a/vulcan/xtel/isode/isode-master/doc/RCS/Makefile,v 8.0 91/07/17 10:28:54 isode Rel $
#
#
# $Log:	Makefile,v $
# Revision 8.0  91/07/17  10:28:54  isode
# Release 7.0
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


DIRS	=	cn-isdn comments cookbook ds duug ftam ifip88-quipu interim \
		internals1 internals2 internals3 islands iso-migrate isode1 \
		isode2 isode3 isode4 isode5 isode6 issues manual map-migrate \
		memos monterey ncf nordunet osi-posix osi-update \
		practical-osi1 practical-osi2 practical-osi3 practical-osi4 \
		quipu ps reality snmp trek tsap tsaptalk ufn whitepages


##################################################################
# Here it is...
##################################################################

all:;		@for i in $(DIRS); \
		    do (echo "cd $$i; make $(MFLAGS) all"; \
			      cd $$i; make $(MFLAGS) all); \
		    done

inst-all:;	@for i in $(DIRS); \
		    do (echo "cd $$i; make $(MFLAGS) inst-all"; \
			      cd $$i; make $(MFLAGS) inst-all); \
		    done

install:;	@for i in $(DIRS); \
		    do (echo "cd $$i; make $(MFLAGS) install"; \
			      cd $$i; make $(MFLAGS) install); \
		    done

lint:;		@for i in $(DIRS); \
		    do (echo "cd $$i; make $(MFLAGS) lint"; \
			      cd $$i; make $(MFLAGS) lint); \
		    done

clean:;		rm -f _* *.tmp *.imp *.ps
		@for i in $(DIRS) dvi2ps ; \
		    do (echo "cd $$i; make $(MFLAGS) clean"; \
			      cd $$i; make $(MFLAGS) clean); \
		    done

grind:;		itroff -man $(TOPDIR)isode-gen.8
		@for i in $(DIRS); \
		    do (echo "cd $$i; make $(MFLAGS) grind"; \
			      cd $$i; make $(MFLAGS) grind); \
		    done

imp:		true
		@make isode-gen.imp
		@for i in $(DIRS); \
		    do (echo "cd $$i; make $(MFLAGS) imp"; \
			      cd $$i; make $(MFLAGS) imp); \
		    done

ps:		true
		@make isode-gen.ps
		cd manual ; make $(MFLAGS) ps

all-ps:		true
		@make isode-gen.ps
		@for i in $(DIRS); \
		    do (echo "cd $$i; make $(MFLAGS) ps"; \
			      cd $$i; make $(MFLAGS) ps); \
		    done

preview:;	@for i in $(DIRS); \
		    do (echo "cd $$i; make $(MFLAGS) preview"; \
			      cd $$i; make $(MFLAGS) preview); \
		    done

true:;


isode-gen.imp:	$(TOPDIR)isode-gen.8
		itroff -t -man $(TOPDIR)isode-gen.8 | catimp -i isode-gen.tmp
		mv isode-gen.tmp isode-gen.imp

DOTROFF= psroff
#DOTROFF= dotroff

isode-gen.ps:	$(TOPDIR)isode-gen.8
		$(DOTROFF) -t -man $(TOPDIR)isode-gen.8 > isode-gen.tmp
		mv isode-gen.tmp isode-gen.ps
