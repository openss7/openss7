###############################################################################
#   Instructions to Make, for compilation of ISODE documentation tree
###############################################################################

###############################################################################
#
# $Header: /a/vulcan/xtel/isode/isode-master/doc/whitepages/RCS/Makefile,v 8.0 91/07/17 10:28:18 isode Rel $
#
#
# $Log:	Makefile,v $
# Revision 8.0  91/07/17  10:28:18  isode
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


DIRS	=	administrator introduction pilot report-1 report-2 report-3 user


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
		@for i in $(DIRS); \
		    do (echo "cd $$i; make $(MFLAGS) clean"; \
			      cd $$i; make $(MFLAGS) clean); \
		    done

grind:;		@for i in $(DIRS); \
		    do (echo "cd $$i; make $(MFLAGS) grind"; \
			      cd $$i; make $(MFLAGS) grind); \
		    done

imp:		true
		@for i in $(DIRS); \
		    do (echo "cd $$i; make $(MFLAGS) imp"; \
			      cd $$i; make $(MFLAGS) imp); \
		    done

ps:		true
		@for i in $(DIRS); \
		    do (echo "cd $$i; make $(MFLAGS) ps"; \
			      cd $$i; make $(MFLAGS) ps); \
		    done

preview:;	@for i in $(DIRS); \
		    do (echo "cd $$i; make $(MFLAGS) preview"; \
			      cd $$i; make $(MFLAGS) preview); \
		    done

true:;
