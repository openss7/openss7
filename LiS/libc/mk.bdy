#
# common makefile for all environments
#
# This file is included from a target-specific makefile
# in a subdirectory below this directory.
# The makefile including this file should have defined:
#   - The LiS configuration.
#     config.mk in the top of the source tree should
#     have been included.
#   - LIBOBJS variable.
#     Contains the names of the object files that should be in the
#     library.
#

LIBFILE_SHARED = lib$(LIBBASE).so
LIBFILE_PSHARED = libp$(LIBBASE).so
LIBFILE_STATIC = lib$(LIBBASE).a

LIB_SHARED = $(LIBDIR)/$(LIBFILE_SHARED)
LIB_PSHARED = $(LIBDIR)/$(LIBFILE_PSHARED)
LIB_STATIC = $(LIBDIR)/$(LIBFILE_STATIC)

CC += -c $(XOPTS)


all: $(LIB_SHARED) $(LIB_PSHARED) $(LIB_STATIC)

$(LIB_SHARED): $(LIBOBJS)
	$(LD) -shared -o $@ $^ -lc

$(LIB_PSHARED): $(PRELDOBJS)
	$(LD) -shared -o $@ $^ -lc

$(LIB_STATIC): $(LIBOBJS)
	-rm -f $@
	$(AR) r $@ $^
	$(RANLIB) $@

%.o: %.c
	$(CC) -o $@ $<

%.o: $(LIBDIR)/%.c
	$(CC) -o $@ $<

clean:
	-rm -f *.o
	-rm -f $(LIB)

realclean: clean
	-rm -f .depend
	-rm -f $(LIB_SHARED)
	-rm -f $(LIB_STATIC)

#
# You may have to be root to do the install and uninstall targets
#
install: all
	install -d $(INST_LIB)
	install	$(LIB_SHARED) $(INST_LIB)/$(LIBFILE_SHARED)
	install	$(LIB_PSHARED) $(INST_LIB)/$(LIBFILE_PSHARED)
	install	$(LIB_STATIC) $(INST_LIB)/$(LIBFILE_STATIC)
	$(LIBDIR)/Install $(INST_LIB)/$(LIBFILE_PSHARED)

#
# Remove all the library files from the standard directories.
#
uninstall:
	$(LIBDIR)/Uninstall $(INST_LIB)/$(LIBFILE_PSHARED)
	-rm -f $(INST_LIB)/$(LIBFILE_SHARED)
	-rm -f $(INST_LIB)/$(LIBFILE_STATIC)
	-rm -f $(INST_LIB)/$(LIBFILE_PSHARED)

