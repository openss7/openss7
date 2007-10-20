##########################################################################
#   Instructions to Make, for compilation of RFCs
##########################################################################

.SUFFIXES:	.rf .txt

.rf.txt:	$<
	nroff -me -Tlpr $< | \
		tr "\010" "~" | \
		sed -e "/^/s/_~//g" | \
		rf-page 57 > $@


##################################################################
# Here it is...
##################################################################

all:;

inst-all:;

install:	inst-all clean

uninstall:;

lint:;

clean:;		rm -f _*

grind:;		iprint *.txt

imp:;

preview:;

ps:;
