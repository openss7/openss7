#################################################################
#                                                               #
#  ISODECL - HULA project for connectionless ISODE              #
#                                             			#
#  module:  	Makeisodecl - Makefile for ISODECL              #
#  author:   	Bill Haggerty                                   #
#  date:     	4/89                                            #
#                                                               #
#  This makes ISODECL.					 	#
#								#
#################################################################
#								#
#			     NOTICE		   		#
#								#
#    Use of this module is subject to the restrictions of the   #
#    ISODE license agreement.					#
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

DIRS	=	tusap susap pusap2 acusap rousap

all clean:;	@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) $@"; \
			      cd $$i; $(MAKE) $@); \
		    done


lint:;

inst-all:;

install:;

grind:;
