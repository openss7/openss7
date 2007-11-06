###############################################################################
#   Instructions to Make, for compilation of MITRE FTAM/FTP gateway
###############################################################################

###############################################################################
#
# $Header: /xtel/isode/isode/ftam-ftp/RCS/Makefile,v 9.0 1992/06/16 12:15:29 isode Rel $
#
#
# $Log: Makefile,v $
# Revision 9.0  1992/06/16  12:15:29  isode
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


###############################################################################
# Generation Rules for program modules
###############################################################################

PEPYPATH=	-DPEPYPATH

.SUFFIXES:	.py .c .o

.c.o:;		$(CC) $(CFLAGS) -DBRIDGE -I$(FTAM) -c $*.c


###############################################################################
# Programs and Libraries
###############################################################################

DLIBES	=	$(TOPDIR)libftam.a $(TOPDIR)libisode.a
LIBES	=	$(TOPDIR)libftam.a $(LIBISODE)
LLIBS	=	$(TOPDIR)llib-lftam $(TOPDIR)llib-lisode


###############################################################################
# Files
###############################################################################

FTAM	=	$(TOPDIR)ftam2/
HFILES	=	$(FTAM)ftamsystem.h $(FTAM)ftamsbr.h $(FTAM)DOCS-types.h
CFILES	=	$(FTAM)ftamd.c $(FTAM)ftamsystem.c $(FTAM)ftamd-manage.c \
			$(FTAM)ftamd-select.c $(FTAM)ftamd-trans.c \
			$(FTAM)ftamsbr.c 
OFILES	=	ftamd.o ftamsystem.o ftamd-manage.o ftamd-select.o \
			ftamd-trans.o ftamsbr.o
DOCS-O	=	$(FTAM)DOCS_tables.o
DOCS-C	=	$(FTAM)DOCS_tables.c


##################################################################
# Here it is...
##################################################################

all:		ftamd-ftp 
inst-all:	inst-ftamd-ftp manuals
install:	inst-all clean
lint:		l-ftamd-ftp


##################################################################
# ftamd-ftp
##################################################################

inst-ftamd-ftp:	$(SBINDIR)iso.ftam-ftp

$(SBINDIR)iso.ftam-ftp:	xftamd-ftp
		-cp $@ zxftamd-ftp
		-rm -f $@
		cp xftamd-ftp $@
		-@ls -gls $@
		-@echo ""

ftamd-ftp:	xftamd-ftp

xftamd-ftp:	$(OFILES) $(DOCS-O) ftp.o ftp_lib.o $(DLIBES)
		$(LDCC) $(LDFLAGS) -o $@ $(OFILES) $(DOCS-O) ftp.o ftp_lib.o \
			$(LIBES) $(LSOCKET)

l-ftamd-ftp:;	$(LINT) $(LFLAGS) -DBRIDGE -I$(FTAM) $(CFILES) ftp.c \
			$(DOCS-C) ftp_lib.c $(LLIBS) \
			| grep -v "warning: possible pointer alignment problem"

ftamd.o:	$(HFILES) $(FTAM)ftamd.c
		$(CC) $(CFLAGS) -DBRIDGE -I$(FTAM) -c $(FTAM)ftamd.c

ftamsystem.o:	$(HFILES) $(HDIR)logger.h $(FTAM)ftamsystem.c
		$(CC) $(CFLAGS) -DBRIDGE -I$(FTAM) -c $(FTAM)ftamsystem.c

ftamd-manage.o:	$(HFILES) $(FTAM)ftamd-manage.c
		$(CC) $(CFLAGS) -DBRIDGE -I$(FTAM) -c $(FTAM)ftamd-manage.c

ftamd-select.o:	$(HFILES) $(FTAM)ftamd-select.c
		$(CC) $(CFLAGS) -DBRIDGE -I$(FTAM) -c $(FTAM)ftamd-select.c

ftamd-trans.o:	$(HFILES) $(HDIR)usr.dirent.h $(HDIR)sys.dirent.h \
			$(FTAM)ftamd-trans.c
		$(CC) $(CFLAGS) -DBRIDGE -I$(FTAM) -c $(FTAM)ftamd-trans.c

ftamsbr.o:	$(HFILES) $(FTAM)ftamsbr.c
		$(CC) $(CFLAGS) -DBRIDGE -I$(FTAM) -c $(FTAM)ftamsbr.c

ftp.o:		ftp_var.h
ftp_lib.o:	ftp_var.h


##################################################################
# manual pages
##################################################################

MANUALS	=	ftamd-ftp.8c

manuals:;	@$(UTILDIR)inst-man.sh $(MANOPTS) $(MANUALS)
		-@echo ""


##################################################################
# clean
##################################################################

clean:;		rm -f *.o x* z* _* core

grind:;		iprint Makefile
		tgrind -lc ftp_var.h ftp.c ftp_lib.c
		@echo $(MANUALS) | \
			tr " " "\012" | \
			sed -e "s%.*%itroff -man &%" | \
			sh -ve

true:;
