###############################################################################
#   Instructions to Make, for compilation of other QUIPU programs
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/quipu/RCS/Makefile,v 9.0 1992/06/16 12:43:19 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:43:19  isode
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

ALLDIR  =	photo image
EVERYDIR=	uips 
EXTRA	=       tools/dsastats
OTHERS	=	tools

DIRS	=	$(ALLDIR) $(EVERYDIR)

##################################################################
# Here it is...
##################################################################

all:;		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) all"; \
			      cd $$i; $(MAKE) all); \
		    done

inst-all:;	@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) inst-all"; \
			      cd $$i; $(MAKE) inst-all); \
		    done

install:;	@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) install"; \
			      cd $$i; $(MAKE) install); \
		    done

everything:;	@for i in $(ALLDIR) $(EXTRA); \
		    do (echo "cd $$i; $(MAKE) all"; \
			      cd $$i; $(MAKE) all); \
		    done
		@for i in $(EVERYDIR); \
		    do (echo "cd $$i; $(MAKE) everything"; \
			      cd $$i; $(MAKE) everything); \
		    done

inst-everything:;  @for i in $(ALLDIR) $(EXTRA); \
		    do (echo "cd $$i; $(MAKE) inst-all"; \
			      cd $$i; $(MAKE) inst-all); \
		    done
		@for i in $(EVERYDIR); \
		    do (echo "cd $$i; $(MAKE) inst-everything"; \
			      cd $$i; $(MAKE) inst-everything); \
		    done

lint:;		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) lint"; \
			      cd $$i; $(MAKE) lint); \
		    done

clean:;		rm -f _*
		@for i in $(DIRS) $(OTHERS); \
		    do (echo "cd $$i; $(MAKE) clean"; \
			      cd $$i; $(MAKE) clean); \
		    done

grind:;		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) grind"; \
			      cd $$i; $(MAKE) grind); \
		    done


##################################################################
# pilot
##################################################################

pilot:;		@for i in uips/dish uips/fred tools/dsaconfig; \
		    do (echo "cd $$i; $(MAKE) all"; \
			      cd $$i; $(MAKE) all); \
		    done


inst-pilot:;	@for i in uips/dish uips/fred tools/dsaconfig; \
		    do (echo "cd $$i; $(MAKE) inst-all"; \
			      cd $$i; $(MAKE) inst-all); \
		    done
