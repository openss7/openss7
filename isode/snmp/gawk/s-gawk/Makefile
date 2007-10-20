###############################################################################
#   Instructions to Make, for compilation of SNMP-capable gawk processes
###############################################################################

###############################################################################
#
# $Header$
#
# Contributed by NYSERNet Inc.  This work was partially supported by the
# U.S. Defense Advanced Research Projects Agency and the Rome Air Development
# Center of the U.S. Air Force Systems Command under contract number
# F30602-88-C-0016.
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

AWKDIR	=	/usr/local/lib/awk/

SHELLS	=	s-netstat s-traceroute
AWKS	=	mib.*


##################################################################
# Here it is...
##################################################################

all:
inst-all:	inst-scripts manuals
install:	inst-all clean
lint:;


##################################################################
# scripts
##################################################################

inst-scripts:;	-mkdir /usr/local/lib/awk
		@for h in $(SHELLS); \
			do $(MAKE)                  TARGET=$$h inst-script;done
		@for h in $(AWKS); \
			do $(MAKE) BINDIR=$(AWKDIR) TARGET=$$h inst-script;done

inst-script:	$(BINDIR)$(TARGET)

$(BINDIR)$(TARGET):	$(TARGET)
		-cp $@ z$(TARGET)
		cp $(TARGET) $@
		-@ls -gls $@
		-@echo ""


################################################################
# manual pages
################################################################

MANUALS	=	s-netstat.1c s-traceroute.1c

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


################################################################
# clean
################################################################

clean:;		rm -f z* _*

grind:;		iprint Makefile
		tgrind -lsh s-*.sh
		iprint mib.*
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
