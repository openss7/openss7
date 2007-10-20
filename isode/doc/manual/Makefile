#########################################################################
# Instructions to Make, for generation of nice-looking papers using LaTeX
#########################################################################

.SUFFIXES:	.pic .c .h .s .ry .py .bnf .make .web .tex .dvi .imp .ps

.pic.tex:;	tpic $<

.c.tex:;	sh -c 'if tgrind -f -lc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.h.tex:;	sh -c 'if tgrind -f -lc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.s.tex:;	sh -c 'if tgrind -f -la68k $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.ry.tex:;	sh -c 'if tgrind -d ../../pepy/grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.py.tex:;	sh -c 'if tgrind -d ../../pepy/grindefs -f -lpepy $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.bnf.tex:;      sh -c 'if tgrind -d ../../pepy/grindefs -f -lbnf $< \
				| sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.make.tex:;	sh -c 'if tgrind -f -lsrc $< | sed -e 1d -e \$$d > $@; \
			then exit 0; else rm $@; exit 1; fi'

.web.tex:;	sh -c 'if weave $<; then exit 0; else rm $@; exit 1; fi'

.tex.dvi:;	sh -c 'if latex \\nonstopmode\\input $<; \
			  then exit 0; else rm $@; exit 1; fi'

.dvi.imp:;	dviimp $(DFLAGS) -i $@ $<

.dvi.ps:;	sh -c 'if dvi2ps $(DFLAGS) -r -b $* -z 10 $<; \
			  then exit 0; else rm $@; exit 1; fi'


#############################################################
# Here it is...
#############################################################

all:		volume1.dvi volume2.dvi volume3.dvi volume4.dvi volume5.dvi

inst-all:;

install:	inst-all clean

lint:;

clean:;		rm -f volume*.imp volume*.ps volume*.dvi volume*.log \
			volume*.blg _*

grind:		volume1.dvi volume2.dvi volume3.dvi volume4.dvi volume5.dvi \
			true
		dvisp $(DFLAGS) volume1
		dvisp $(DFLAGS) volume2
		dvisp $(DFLAGS) volume3
		dvisp $(DFLAGS) volume4
		dvisp $(DFLAGS) volume5

imp:		volume1.imp volume2.imp volume3.imp volume4.imp volume5.imp


ps:		volume1.ps volume2.ps volume3.ps volume4.ps volume5.ps

split:		all true
		dvi2ps $(DFLAGS) -r -b volume1 volume1.dvi
		dvi2ps $(DFLAGS) -r -b volume2 volume2.dvi
		dvi2ps $(DFLAGS) -r -b volume3 volume3.dvi
		dvi2ps $(DFLAGS) -r -b volume4 volume4.dvi
		dvi2ps $(DFLAGS) -r -b volume5 volume5.dvi


#############################################################
# volume1
#############################################################

# customization files, et. al., not included below
volume1.dvi:	volume1.vrsn volume1.bbl volume1.ind

volume1.vrsn:	volume1.tex \
		    manual.tex \
		preface.tex \
		introduction.tex \
		acsap.tex \
		    grind2a-1.tex grind2a-2a.tex grind2a-2b.tex \
		    grind2a-2c.tex grind2a-2d.tex \
		    grind2a-2f.tex grind2a-2g.tex \
		    table2a-1.tex table2a-2.tex \
		rosap.tex \
		    grind2b-2.tex grind2b-3a.tex grind2b-3b.tex \
		    grind2b-3c.tex grind2b-3d.tex grind2b-3e.tex \
		    grind2b-4a.tex grind2b-4b.tex \
		    table2b-1a.tex table2b-1b.tex \
		rtsap.tex \
		    grind2c-2.tex grind2c-3.tex \
		    table2c-1.tex \
		psap.tex \
		    grind3-1.tex grind3-2.tex grind3-3.tex \
		    table3-1.tex table3-2.tex table3-3.tex \
		aliases.tex \
		macros.tex \
		entities.tex \
		objects.tex \
		defining.tex \
		appendix2.tex \
		    grindB-1.tex grindB-2.tex grindB-3.tex \
		    grindB-4.tex grindB-5.tex grindB-6.tex \
		version.sh
		@./version.sh volume1


#############################################################
# volume2
#############################################################

# customization files, et. al., not included below
volume2.dvi:	volume2.vrsn volume2.bbl volume2.ind

volume2.vrsn:	volume2.tex \
		    manual.tex \
		preface.tex \
		introduction.tex \
		psap2.tex \
		    grind5a-1.tex grind5a-2a.tex grind5a-2b.tex grind5a-3.tex \
		    table5a-1.tex table5a-2.tex \
		ssap.tex \
		    grind5b-1.tex grind5b-2.tex grind5b-3a.tex grind5b-3b.tex \
		    grind5b-4.tex \
		    table5b-1.tex table5b-2.tex table5b-3.tex table5b-4.tex \
		    table5b-5.tex \
		tsap.tex \
		    grind6-1.tex grind6-2.tex grind6-3.tex \
		    table6-1.tex \
		services.tex \
		tailoring.tex \
		logging.tex \
		tswitch.tex \
		version.sh
		@./version.sh volume2


#############################################################
# volume3
#############################################################

# customization files, et. al., not included below
volume3.dvi:	volume3.vrsn volume3.bbl volume3.ind

volume3.vrsn:	volume3.tex \
		    manual.tex \
		preface.tex \
		introduction.tex \
		libftam.tex \
		    tableftam-1.tex tableftam-2.tex tableftam-3.tex \
		    tableftam-4.tex tableftam-5.tex \
		documents.tex \
		unixftam.tex \
		ftamftp.tex \
		unixvt.tex \
		imisc.tex \
		    grindimisc-1.tex \
		version.sh
		@./version.sh volume3


#############################################################
# volume4
#############################################################

# customization files, et. al., not included below
volume4.dvi:	volume4.vrsn volume4.bbl volume4.ind

volume4.vrsn:	volume4.tex \
		    manual.tex \
		preface.tex \
		introduction.tex \
		concepts.tex \
		discipline.tex \
		rosy.tex \
		    grindimage-1.tex \
		posy.tex \
		    grindposy-1.tex grindposy-2.tex grindposy-3.tex \
		    grindposy-4.tex \
		pepy.tex \
		    diagram4-1.tex \
		    grind4-1.tex  grind4-2.tex grind4-3.tex grind4-4.tex \
		    grind4-5a.tex grind4-5b.tex grind4-6a.tex \
		    grind4-6b.tex grind4-7.tex grind4-8a.tex grind4-8b.tex \
		    grind4-9a.tex grind4-9b.tex grind4-10.tex \
		pepsy.tex \
		    grind-pepsy1.tex pepsy-single.tex grind-repeat.tex \
		    grind-choice.tex grind-optional.tex grind-fn.tex \
		    grind-encfn.tex grind-decfn.tex \
		librosy.tex \
		initiator.tex \
		    ryinit-estab.tex ryinit-invoke.tex ryinit-async.tex \
		    ryinit-release.tex ryinit-error.tex ryinitiator-h.tex \
		    ryinitiator-c.tex ryinit-example.tex \
		responder.tex \
		    ryresponder-h.tex ryresp-assoc.tex ryresp-invoke.tex \
		    ryresp-error.tex \
		passwd.tex \
		    passwd-ops.tex passwd-make.tex passwd-init.tex \
		    passwd-resp.tex \
		version.sh
		@./version.sh volume4


#############################################################
# volume5
#############################################################

# customization files, et. al., not included below
volume5.dvi:    volume5.vrsn volume5.bbl volume5.ind 

ASN = asn-start.py na.py schema.py inherit.py \
	getedb.py acl.py protected.py asn-end.py

asn-summary.py: $(ASN)
	cat $(ASN) > $@

volume5.vrsn:   volume5.tex \
		    manual.tex \
		preface.tex \
		introduction.tex \
		q-introduction.tex \
		q-dsa.tex \
		    tableq-1.tex \
		    inherit.tex \
		q-dish.tex \
		    figureq-dsa.tex \
		    figureq-dit.tex \
		    figureq-entry.tex \
		q-sd.tex \
		q-pod.tex \
		q-de.tex \
		q-dsap.tex \
		q-quipu.tex \
		q-syntax.tex \
		q-secman.tex \
		q-security.tex \
		q-fred.tex \
		q-secdes.tex \
			acl.tex \
			na.tex \
			ns.tex \
			schema.tex \
			getedb.tex \
		q-operations.tex \
		q-async-dap.tex \
		q-appendix-a.tex \
		    tableq-2.tex \
		q-appendix-b.tex \
		    oidtable.tex \
		    name.tex \
		    edb.tex \
		q-appendix-c.tex \
		    na-defs.tex \
		q-appendix-e.tex \
			asn-summary.tex \
		version.sh
		@./version.sh volume5


#############################################################
# miscellany
#############################################################

biblio:;	bibtex volume1
		bibtex volume2
		bibtex volume3
		bibtex volume4
		bibtex volume5

preview:	volume1.dvi volume2.dvi volume3.dvi volume4.dvi volume5.dvi \
			true
		xdvi volume1.dvi
		xdvi volume2.dvi
		xdvi volume3.dvi
		xdvi volume4.dvi
		xdvi volume5.dvi

index:;		idx2ind -l volume1 volume2 volume3 volume4 volume5

true:;
