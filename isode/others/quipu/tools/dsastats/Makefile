# other/quipu/tools/dsastats/Makefile

##############################
# Configuration - see README #
##############################

# The file the DSA logs to
LOGFILE= $(LOGDIR)quipu.log

# The DSAs tailor file
TAILORFILE=$(ETCDIR)quiputailor

# The DN of your organisation
LOCALORGDN=c=GB@o=University College London

# Where to install the scripts
LOCALPATH=$(BINDIR)

# Where to place the two config files
LETCDIR=$(ETCDIR)

#####################################
# Rules...
#####################################

all:	dsastats

install: inst-all clean

dsastats: dsastats.dist
	sed 's#QUIPULOG#$(LOGFILE)#g' dsastats.dist | \
	sed 's#QUIPUTAILOR#$(TAILORFILE)#g' | \
	sed 's#QUIPULOCALORG#$(LOCALORGDN)#g' | \
	sed 's#QUIPUETCDIR#$(LETCDIR)#g' >dsastats
	chmod +x dsastats

inst-all:
	cp dsastats $(LOCALPATH)
	ls -gls $(LOCALPATH)dsastats
	cp quipulocaladds $(LETCDIR)
	ls -gls $(LETCDIR)quipulocaladds
	cp quiputechusers $(LETCDIR)
	ls -gls $(LETCDIR)quiputechusers

clean:

