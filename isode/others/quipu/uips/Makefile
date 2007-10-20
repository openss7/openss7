###############################################################################
#   Instructions to Make, for compilation of QUIPU user interface programs
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/others/quipu/uips/RCS/Makefile,v 9.0 1992/06/16 12:44:16 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:44:16  isode
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


DIRS	=	dish
OTHERS  =	fred pod sd manage de doog


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

everything:;	@for i in $(DIRS) $(OTHERS); \
		    do (echo "cd $$i; $(MAKE) all"; \
			      cd $$i; $(MAKE) all); \
		    done

inst-everything:;	@for i in $(DIRS) $(OTHERS); \
		    do (echo "cd $$i; $(MAKE) inst-all"; \
			      cd $$i; $(MAKE) inst-all); \
		    done

lint:;		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) lint"; \
			      cd $$i; $(MAKE) lint); \
		    done

lint-everything:;	@for i in $(DIRS) $(OTHERS); \
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
