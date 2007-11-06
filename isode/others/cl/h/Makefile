###############################################################################
#   Instructions to Make, for compilation of ISODE header files
###############################################################################

###############################################################################
#
# $Header$
#
#
# $Log$
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

DIRS	=	$(QUIPU) empty

HEADERS	=	config.h \
		general.h manifest.h internet.h isoaddrs.h x25.h cons.h \
		tp4.h \
		isoservent.h logger.h tailor.h \
		tpkt.h tsap.h \
		spkt.h ssap.h \
		ppkt.h psap2.h psap.h \
		acpkt.h acsap.h \
		rtpkt.h rtsap.h \
		ropkt.h rosap.h \
		rosy.h \
		fpkt.h ftam.h \
		sys.dirent.h dirent.h


##################################################################
# Here it is...
##################################################################

all:;		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) all"; \
			      cd $$i; $(MAKE) all); \
		    done

inst-all:;	-mkdir $(INCDIRM)
		@for h in $(HEADERS); do $(MAKE) TARGET=$$h inst-target; done
		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) inst-all"; \
			      cd $$i; $(MAKE) inst-all); \
		    done


inst-target:	$(INCDIR)$(TARGET)

$(INCDIR)$(TARGET):	$(TARGET)
		-cp $@ z$(TARGET)
		sed -e 's%#include "\([^/]*\)"%#include "$(INCDIR)\1"%' \
			< $(TARGET) > $@
		-@ls -gls $@
		-@echo ""

install:	inst-all clean

lint:;		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) lint"; \
			      cd $$i; $(MAKE) lint); \
		    done

clean:;		rm -f z* _*
		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) clean"; \
			      cd $$i; $(MAKE) clean); \
		    done

grind:;		tgrind -lc $(HEADERS)
		@for i in $(DIRS); \
		    do (echo "cd $$i; $(MAKE) grind"; \
			      cd $$i; $(MAKE) grind); \
		    done
