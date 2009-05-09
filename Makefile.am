# =============================================================================
# 
# @(#) $RCSfile$ $Name$($Revision$) $Date$
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
# Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU Affero General Public License as published by the Free
# Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
# details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>, or
# write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
# 02139, USA.
#
# -----------------------------------------------------------------------------
#
# U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
# behalf of the U.S. Government ("Government"), the following provisions apply
# to you.  If the Software is supplied by the Department of Defense ("DoD"), it
# is classified as "Commercial Computer Software" under paragraph 252.227-7014
# of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
# successor regulations) and the Government is acquiring only the license rights
# granted herein (the license rights customarily provided to non-Government
# users).  If the Software is supplied to any unit or agency of the Government
# other than DoD, it is classified as "Restricted Computer Software" and the
# Government's rights in the Software are defined in paragraph 52.227-19 of the
# Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
# the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
# (or any successor regulations).
#
# -----------------------------------------------------------------------------
#
# Commercial licensing and support of this software is available from OpenSS7
# Corporation at a fee.  See http://www.openss7.com/
#
# -----------------------------------------------------------------------------
#
# Last Modified $Date$ by $Author$
#
# =============================================================================

##
## General rules
##
include am/openss7.am

##
## Version file
##
include am/version.am

##
## Help
##
include am/help.am

##
## Problem Reports
##
include am/pr.am

AUTOMAKE_OPTIONS += nostdinc
AM_CPPFLAGS = -I. -I$(top_builddir) -I$(srcdir)

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if RPM_BUILD_USER
## RPM_BUILD_USER

##
## These are for ptys and should be part of the strutil or strio package.
##
bin_PROGRAMS			=
if CONFIG_STREAMS_UTILS
bin_PROGRAMS			+= \
				  strchg \
				  strconf \
				  strreset
endif

sbin_PROGRAMS			= \
				  @PACKAGE_TARNAME@_mknod
if CONFIG_STREAMS_SAD
sbin_PROGRAMS			+= autopush
endif
if CONFIG_STREAMS_SAD_MODULE
sbin_PROGRAMS			+= autopush
endif
if CONFIG_STREAMS_LOG
sbin_PROGRAMS			+= strace strerr
endif
if CONFIG_STREAMS_LOG_MODULE
sbin_PROGRAMS			+= strace strerr
endif
if CONFIG_STREAMS_SC
sbin_PROGRAMS			+= scls strinfo
endif
if CONFIG_STREAMS_SC_MODULE
sbin_PROGRAMS			+= scls strinfo
endif
if CONFIG_STREAMS_PIPE
sbin_PROGRAMS			+= fattach fdetach
endif
if CONFIG_STREAMS_PIPE_MODULE
sbin_PROGRAMS			+= fattach fdetach
endif
if CONFIG_STREAMS_UTILS
sbin_PROGRAMS			+= \
				  insf \
				  strvf
endif

if WITH_SNMP_AGENT
sbin_PROGRAMS			+= streamsd
endif

#dist_bin_SCRIPTS		= scripts/modpost.sh scripts/modsyms.sh

dist_sbin_SCRIPTS		=
if CONFIG_STREAMS_LOG
dist_sbin_SCRIPTS		+= src/util/strclean
endif
if CONFIG_STREAMS_LOG_MODULE
dist_sbin_SCRIPTS		+= src/util/strclean
endif
if CONFIG_STREAMS_UTILS
dist_sbin_SCRIPTS		+= \
				  src/util/strload \
				  src/util/strsetup
endif

pkglibexec_PROGRAMS		= testlog
if CONFIG_STREAMS_STH
pkglibexec_PROGRAMS		+= test-streams
endif
if CONFIG_STREAMS_STH_MODULE
pkglibexec_PROGRAMS		+= test-streams
endif
if CONFIG_STREAMS_CLONE
pkglibexec_PROGRAMS		+= test-clone
endif
if CONFIG_STREAMS_CLONE_MODULE
pkglibexec_PROGRAMS		+= test-clone
endif
if CONFIG_STREAMS_CONNLD
pkglibexec_PROGRAMS		+= test-connld
endif
if CONFIG_STREAMS_CONNLD_MODULE
pkglibexec_PROGRAMS		+= test-connld
endif
if CONFIG_STREAMS_ECHO
pkglibexec_PROGRAMS		+= test-echo
endif
if CONFIG_STREAMS_ECHO_MODULE
pkglibexec_PROGRAMS		+= test-echo
endif
if CONFIG_STREAMS_FIFO
pkglibexec_PROGRAMS		+= test-fifo
endif
if CONFIG_STREAMS_FIFO_MODULE
pkglibexec_PROGRAMS		+= test-fifo
endif
if CONFIG_STREAMS_LOG
pkglibexec_PROGRAMS		+= test-log
endif
if CONFIG_STREAMS_LOG_MODULE
pkglibexec_PROGRAMS		+= test-log
endif
if CONFIG_STREAMS_LOOP
pkglibexec_PROGRAMS		+= test-loop
endif
if CONFIG_STREAMS_LOOP_MODULE
pkglibexec_PROGRAMS		+= test-loop
endif
if CONFIG_STREAMS_NSDEV
pkglibexec_PROGRAMS		+= test-nsdev
endif
if CONFIG_STREAMS_NSDEV_MODULE
pkglibexec_PROGRAMS		+= test-nsdev
endif
if CONFIG_STREAMS_MUX
pkglibexec_PROGRAMS		+= test-mux
endif
if CONFIG_STREAMS_MUX_MODULE
pkglibexec_PROGRAMS		+= test-mux
endif
if CONFIG_STREAMS_NULS
pkglibexec_PROGRAMS		+= test-nuls
endif
if CONFIG_STREAMS_NULS_MODULE
pkglibexec_PROGRAMS		+= test-nuls
endif
if CONFIG_STREAMS_PIPE
pkglibexec_PROGRAMS		+= test-pipe perftest perftestn
endif
if CONFIG_STREAMS_PIPE_MODULE
pkglibexec_PROGRAMS		+= test-pipe perftest perftestn
endif
if CONFIG_STREAMS_PIPEMOD
pkglibexec_PROGRAMS		+= test-pipemod
endif
if CONFIG_STREAMS_PIPEMOD_MODULE
pkglibexec_PROGRAMS		+= test-pipemod
endif
if CONFIG_STREAMS_SAD
pkglibexec_PROGRAMS		+= test-sad
endif
if CONFIG_STREAMS_SAD_MODULE
pkglibexec_PROGRAMS		+= test-sad
endif
if CONFIG_STREAMS_SC
pkglibexec_PROGRAMS		+= test-sc
endif
if CONFIG_STREAMS_SC_MODULE
pkglibexec_PROGRAMS		+= test-sc
endif


#LFS_EXTRA_TESTS		= 
#LFS_EXTRA_TOOLS		= @PACKAGE_TARNAME@_mknod

#LDADD				= libstreams.la

lib_LTLIBRARIES			= libstreams.la \
				  libLiS.la \
				  libpLiS.la

lib32_LTLIBRARIES		=

snmpdlmod_LTLIBRARIES		=

if WITH_SNMP_AGENT
snmpdlmod_LTLIBRARIES		+= strMIB.la
endif

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

noinst_LIBRARIES		= \
				libLfS-specfs.a \
				libLfS-streams.a \
				libLfS-drivers.a \
				libLfS-modules.a

libLfS_specfs_a_SOURCES		= \
				src/kernel/strspecfs.h \
				src/kernel/strlookup.h \
				src/kernel/strattach.h \
				src/kernel/strpipe.h \
				src/kernel/strargs.h

if CONFIG_STREAMS_SEPARATE_COMPILE
libLfS_specfs_a_SOURCES		+= \
				src/kernel/strspecfs.c \
				src/kernel/strlookup.c \
				src/kernel/strattach.c \
				src/kernel/strpipe.c
else
libLfS_specfs_a_SOURCES		+= \
				src/kernel/specfs.c
endif

libLfS_specfs_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) \
				  src/kernel/strspecfs.c \
				  src/kernel/strlookup.c \
				  src/kernel/strattach.c \
				  src/kernel/strpipe.c
libLfS_specfs_a_CPPFLAGS	= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libLfS_specfs_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLfS_specfs_a_OBJECTS): $(libLfS_specfs_a_DEPENDENCIES)


libLfS_streams_a_SOURCES	= \
				src/kernel/strreg.h \
				src/kernel/strprocfs.h \
				src/kernel/strsched.h \
				src/kernel/strsysctl.h \
				src/kernel/strutil.h \
				src/kernel/strsad.h \
				src/kernel/strargs.h

if CONFIG_STREAMS_SEPARATE_COMPILE
libLfS_streams_a_SOURCES	+= \
				src/kernel/strmain.c \
				src/kernel/strreg.c \
				src/kernel/strprocfs.c \
				src/kernel/strsched.c \
				src/kernel/strsysctl.c \
				src/kernel/strsyms.c \
				src/kernel/strutil.c \
				src/kernel/strsad.c
else
libLfS_streams_a_SOURCES	+= \
				src/kernel/streams.c
endif

##if CONFIG_STREAMS_STH
##libLfS_streams_a_SOURCES	+= src/modules/sth.c src/modules/sth.h 
##endif
if CONFIG_STREAMS_BUFMOD
libLfS_streams_a_SOURCES	+= src/modules/bufmod.c
endif
if CONFIG_STREAMS_NULLMOD
libLfS_streams_a_SOURCES	+= src/modules/nullmod.c
endif
if CONFIG_STREAMS_PIPEMOD
libLfS_streams_a_SOURCES	+= src/modules/pipemod.c
endif
if CONFIG_STREAMS_CONNLD
libLfS_streams_a_SOURCES	+= src/modules/connld.c
endif
if CONFIG_STREAMS_SC
libLfS_streams_a_SOURCES	+= src/modules/sc.c src/include/sys/sc.h
endif

##if CONFIG_STREAMS_CLONE
##libLfS_streams_a_SOURCES	+= src/drivers/clone.c src/drivers/clone.h
##endif
if CONFIG_STREAMS_FIFO
libLfS_streams_a_SOURCES	+= src/drivers/fifo.c src/drivers/fifo.h
endif
if CONFIG_STREAMS_SAD
libLfS_streams_a_SOURCES	+= src/drivers/sad.c src/include/sys/sad.h
endif
if CONFIG_STREAMS_NSDEV
libLfS_streams_a_SOURCES	+= src/drivers/nsdev.c
endif
if CONFIG_STREAMS_ECHO
libLfS_streams_a_SOURCES	+= src/drivers/echo.c
endif
if CONFIG_STREAMS_MUX
libLfS_streams_a_SOURCES	+= src/drivers/mux.c
endif
if CONFIG_STREAMS_NULS
libLfS_streams_a_SOURCES	+= src/drivers/nuls.c
endif
if CONFIG_STREAMS_PIPE
libLfS_streams_a_SOURCES	+= src/drivers/pipe.c src/drivers/pipe.h
endif
if CONFIG_STREAMS_LOG
libLfS_streams_a_SOURCES	+= src/include/log.h src/include/sys/log.h src/include/strlog.h src/include/sys/strlog.h \
				   src/drivers/log.h src/drivers/log.c
endif
if CONFIG_STREAMS_LOOP
libLfS_streams_a_SOURCES	+= src/drivers/loop.c src/include/sys/loop.h
endif
if CONFIG_STREAMS_SFX
libLfS_streams_a_SOURCES	+= src/drivers/sfx.c
endif
if CONFIG_STREAMS_SPX
libLfS_streams_a_SOURCES	+= src/drivers/spx.c
endif

libLfS_streams_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) \
				  src/kernel/strmain.c \
				  src/kernel/strreg.c \
				  src/kernel/strprocfs.c \
				  src/kernel/strsched.c \
				  src/kernel/strsysctl.c \
				  src/kernel/strsyms.c \
				  src/kernel/strutil.c \
				  src/kernel/strsad.c \
				  src/modules/sth.c \
				  src/drivers/clone.c
libLfS_streams_a_CPPFLAGS	= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libLfS_streams_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLfS_streams_a_OBJECTS): $(libLfS_streams_a_DEPENDENCIES)

libLfS_drivers_a_SOURCES	=

if CONFIG_STREAMS_CLONE_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/clone.c src/drivers/clone.h
endif
if CONFIG_STREAMS_FIFO_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/fifo.c src/drivers/fifo.h
endif
if CONFIG_STREAMS_SAD_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/sad.c src/include/sys/sad.h
endif
if CONFIG_STREAMS_NSDEV_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/nsdev.c
endif
if CONFIG_STREAMS_ECHO_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/echo.c
endif
if CONFIG_STREAMS_MUX_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/mux.c
endif
if CONFIG_STREAMS_NULS_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/nuls.c
endif
if CONFIG_STREAMS_PIPE_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/pipe.c src/drivers/pipe.h
endif
if CONFIG_STREAMS_LOG_MODULE
libLfS_drivers_a_SOURCES	+= src/include/log.h src/include/sys/log.h src/include/strlog.h src/include/sys/strlog.h \
				   src/drivers/log.h src/drivers/log.c
endif
if CONFIG_STREAMS_LOOP_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/loop.c src/include/sys/loop.h
endif
if CONFIG_STREAMS_SFX_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/sfx.c
endif
if CONFIG_STREAMS_SPX_MODULE
libLfS_drivers_a_SOURCES	+= src/drivers/spx.c
endif

libLfS_drivers_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libLfS_drivers_a_CPPFLAGS	= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libLfS_drivers_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLfS_drivers_a_OBJECTS): $(libLfS_drivers_a_DEPENDENCIES)

libLfS_modules_a_SOURCES	=

if CONFIG_STREAMS_STH_MODULE
libLfS_modules_a_SOURCES	+= src/modules/sth.c src/modules/sth.h 
endif
if CONFIG_STREAMS_BUFMOD_MODULE
libLfS_modules_a_SOURCES	+= src/modules/bufmod.c
endif
if CONFIG_STREAMS_NULLMOD_MODULE
libLfS_modules_a_SOURCES	+= src/modules/nullmod.c
endif
if CONFIG_STREAMS_PIPEMOD_MODULE
libLfS_modules_a_SOURCES	+= src/modules/pipemod.c
endif
if CONFIG_STREAMS_CONNLD_MODULE
libLfS_modules_a_SOURCES	+= src/modules/connld.c
endif
if CONFIG_STREAMS_SC_MODULE
libLfS_modules_a_SOURCES	+= src/modules/sc.c src/include/sys/sc.h
endif
if CONFIG_STREAMS_TESTMOD_MODULE
libLfS_modules_a_SOURCES	+= src/modules/testmod.c src/include/sys/testmod.h
endif

libLfS_modules_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libLfS_modules_a_CPPFLAGS	= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libLfS_modules_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libLfS_modules_a_OBJECTS): $(libLfS_modules_a_DEPENDENCIES)

specfs.o: stamp-verobjs
	@list=`$(ECHO) " $(libLfS_specfs_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list

streams.o: stamp-verobjs
	@list=`$(ECHO) " $(libLfS_streams_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list

## RPM_BUILD_KERNEL
endif
## PKG_BUILD_ARCH
endif

EXTRA_TEX			+= \
				scripts/texinfo.cnf \
				scripts/texi/args.texi.in \
				scripts/texi/fdl.texi \
				scripts/texi/gpl.texi \
				scripts/texi/install.texi \
				scripts/texi/lesser.texi \
				scripts/texi/testsuite.texi

EXTRA_DIST			+= $(EXTRA_TEX)
dist_noinst_SCRIPTS		+= scripts/missing2 scripts/missing3
EXTRA_SCRIPTS			+= scripts/missing2 scripts/missing3

##
## Rules for building kernel objects
##
KERNEL_OBJECTS			= $(libLfS_specfs_a_OBJECTS) $(libLfS_streams_a_OBJECTS) $(libLfS_drivers_a_OBJECTS) $(libLfS_modules_a_OBJECTS)
KERNEL_MODULES			= specfs.o streams.o $(libLfS_drivers_a_OBJECTS) $(libLfS_modules_a_OBJECTS)
KERNEL_PRELOADS			=
KERNEL_MODCONF			= $(STRCONF_CONFMOD)
KERNEL_SUBDIR			= $(PACKAGE_LCNAME)
KMODCONF_EXT			= $(PACKAGE_LCNAME)
KERNEL_MODMAPS			=

if WITH_KO_MODULES
## WITH_KO_MODULES
kpre				= streams_
else
kpre				= streams-
## WITH_KO_MODULES
endif

include am/kernel.am

if WITH_KO_MODULES
else
if PKG_BUILD_INDEP
if RPM_BUILD_USER
nodist_modutil_DATA		= \
				src/util/modutils/@PACKAGE_LCNAME@
endif
endif
endif

##
## Rules for building libraries
##
LIBRARY_VERSION_FILES		= \
				  src/lib/streams.VERSION
include am/libraries.am

if RPM_BUILD_USER
## RPM_BUILD_USER

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

libstreams_la_SOURCES		= \
				src/lib/testcancel.c \
				src/lib/putpmsg.c \
				src/lib/putmsg.c \
				src/lib/getpmsg.c \
				src/lib/getmsg.c \
				src/lib/isastream.c \
				src/lib/pipe.c \
				src/lib/fattach.c \
				src/lib/fdetach.c \
				src/lib/strlog.c \
				src/lib/streams.h
libstreams_la_DEPENDENCIES	= $(top_srcdir)/src/lib/streams.VERSION
libstreams_la_LDFLAGS		= -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/streams.VERSION
libstreams_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libstreams_la_CFLAGS		= $(USER_CFLAGS)
##if USING_IOCTL_GETPMSG_PUTPMSG
libstreams_la_LDFLAGS		+= -version-info 0:1:0 -export-dynamic
##else
##libstreams_la_LDFLAGS		+= -version-info 0:0:0 -export-dynamic
##endif

libLiS_la_SOURCES		= \
				src/lib/testcancel.c \
				src/lib/putpmsg.c \
				src/lib/putmsg.c \
				src/lib/getpmsg.c \
				src/lib/getmsg.c \
				src/lib/isastream.c \
				src/lib/pipe.c \
				src/lib/fattach.c \
				src/lib/fdetach.c \
				src/lib/strlog.c \
				src/lib/streams.h
libLiS_la_DEPENDENCIES		= $(top_srcdir)/src/lib/streams.VERSION
libLiS_la_LDFLAGS		= -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/streams.VERSION
libLiS_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libLiS_la_CFLAGS		= $(USER_CFLAGS)
##if USING_IOCTL_GETPMSG_PUTPMSG
libLiS_la_LDFLAGS		+= -version-info 0:1:0 -export-dynamic
##else
##libLiS_la_LDFLAGS		+= -version-info 0:0:0 -export-dynamic
##endif

libpLiS_la_SOURCES		= \
				src/lib/testcancel.c \
				src/lib/putpmsg.c \
				src/lib/putmsg.c \
				src/lib/getpmsg.c \
				src/lib/getmsg.c \
				src/lib/isastream.c \
				src/lib/fattach.c \
				src/lib/fdetach.c \
				src/lib/strlog.c \
				src/lib/streams.h
libpLiS_la_DEPENDENCIES		= $(top_srcdir)/src/lib/streams.VERSION
libpLiS_la_LDFLAGS		= -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/streams.VERSION
libpLiS_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libpLiS_la_CFLAGS		= $(USER_CFLAGS)
##if USING_IOCTL_GETPMSG_PUTPMSG
libpLiS_la_LDFLAGS		+= -version-info 0:1:0 -export-dynamic
##else
##libpLiS_la_LDFLAGS		+= -version-info 0:0:0 -export-dynamic
##endif

strMIB_la_SOURCES		= \
				  src/snmp/strMIB.c \
				  src/snmp/strMIB.h \
				  src/snmp/strExtMIB.c \
				  src/snmp/strExtMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h
strMIB_la_DEPENDENCIES		= libstreams.la
strMIB_la_LDFLAGS		= -module -avoid-version \
				  -shared $(STREAMS_LDFLAGS) $(SNMP_LDFLAGS)
strMIB_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DMODULE
strMIB_la_CFLAGS		= $(USER_CFLAGS)
strMIB_la_LIBADD		= libstreams.la $(SNMP_LDADD)

EXTRA_PROGRAMS			+= @PACKAGE_TARNAME@_mknod

#bin_PROGRAMS			= timetst strtst $(LFS_EXTRA_TESTS)
#sbin_PROGRAMS			= streams $(LFS_EXTRA_TOOLS)

strchg_SOURCES			= src/util/strchg.c
strchg_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strchg_LDFLAGS			=
strchg_LDADD			= libstreams.la
strchg_CPPFLAGS			= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"strchg"'
strchg_CFLAGS			= $(USER_CFLAGS)

strconf_SOURCES			= src/util/strconf.c
strconf_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strconf_LDFLAGS			=
strconf_LDADD			= libstreams.la
strconf_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"strconf"'
strconf_CFLAGS			= $(USER_CFLAGS)

strreset_SOURCES		= src/util/strreset.c
strreset_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strreset_LDFLAGS		=
strreset_LDADD			= libstreams.la
strreset_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"strreset"'
strreset_CFLAGS			= $(USER_CFLAGS)

autopush_SOURCES		= src/util/autopush.c
autopush_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
autopush_LDFLAGS		=
autopush_LDADD			= libstreams.la
autopush_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"autopush"'
autopush_CFLAGS			= $(USER_CFLAGS)

fattach_SOURCES			= src/util/fattach.c
fattach_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
fattach_LDFLAGS			=
fattach_LDADD			= libstreams.la
fattach_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"fattach"'
fattach_CFLAGS			= $(USER_CFLAGS)

fdetach_SOURCES			= src/util/fdetach.c 
fdetach_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
fdetach_LDFLAGS			=
fdetach_LDADD			= libstreams.la
fdetach_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"fdetach"'
fdetach_CFLAGS			= $(USER_CFLAGS)

insf_SOURCES			= src/util/insf.c
insf_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
insf_LDFLAGS			=
insf_LDADD			= libstreams.la
insf_CPPFLAGS			= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"insf"'
insf_CFLAGS			= $(USER_CFLAGS)

scls_SOURCES			= src/util/scls.c
scls_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
scls_LDFLAGS			=
scls_LDADD			= libstreams.la
scls_CPPFLAGS			= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"scls"'
scls_CFLAGS			= $(USER_CFLAGS)

strace_SOURCES			= src/util/strace.c
strace_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strace_LDFLAGS			=
strace_LDADD			= libstreams.la
strace_CPPFLAGS			= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"strace"'
strace_CFLAGS			= $(USER_CFLAGS)

##strclean_SOURCES		= src/util/strclean.c
##strclean_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
##strclean_LDFLAGS		=
##strclean_LDADD		= libstreams.la
##strclean_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"strclean"'
##strclean_CFLAGS		= $(USER_CFLAGS)

##@srcdir@/src/util/strclean.c:
##	touch $@

##@srcdir@/src/util/strclean: @srcdir@/src/util/strclean.c
##	touch $@

strerr_SOURCES			= src/util/strerr.c
strerr_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strerr_LDFLAGS			=
strerr_LDADD			= libstreams.la
strerr_CPPFLAGS			= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"strerr"'
strerr_CFLAGS			= $(USER_CFLAGS)

strinfo_SOURCES			= src/util/strinfo.c
strinfo_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strinfo_LDFLAGS			=
strinfo_LDADD			= libstreams.la
strinfo_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"strinfo"'
strinfo_CFLAGS			= $(USER_CFLAGS)

##strload_SOURCES		= src/util/strload.c
##strload_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
##strload_LDFLAGS		=
##strload_LDADD			= libstreams.la
##strload_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"strload"'
##strload_CFLAGS		= $(USER_CFLAGS)

##@srcdir@/src/util/strload.c:
##	touch $@

##@srcdir@/src/util/strload: @srcdir@/src/util/strload.c
##	touch $@

##strsetup_SOURCES		= src/util/strsetup.c
##strsetup_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
##strsetup_LDFLAGS		=
##strsetup_LDADD		= libstreams.la
##strsetup_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"strsetup"'
##strsetup_CFLAGS		= $(USER_CFLAGS)

##@srcdir@/src/util/strsetup.c:
##	touch $@

##@srcdir@/src/util/strsetup: @srcdir@/src/util/strsetup.c
##	touch $@

strvf_SOURCES			= src/util/strvf.c
strvf_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)
strvf_LDFLAGS			=
strvf_LDADD			= libstreams.la
strvf_CPPFLAGS			= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"strvf"'
strvf_CFLAGS			= $(USER_CFLAGS)

@PACKAGE_TARNAME@_mknod_SOURCES		= src/util/@PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)
@PACKAGE_TARNAME@_mknod_LDADD		=
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"@PACKAGE_TARNAME@_mknod"'
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS)

streamsd_SOURCES		= src/snmp/streamsd.c \
				  src/snmp/streamsd.h \
				  src/snmp/strMIB.c \
				  src/snmp/strMIB.h \
				  src/snmp/strExtMIB.c \
				  src/snmp/strExtMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h
streamsd_LDFLAGS		= $(SNMP_LDFLAGS)
streamsd_LDADD			= libstreams.la $(SNMP_LDADD)
streamsd_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"streamsd"'
streamsd_CFLAGS			= $(USER_CFLAGS)

test_clone_SOURCES		= src/test/test-clone.c
test_clone_LDFLAGS		=
test_clone_LDADD		= libstreams.la
test_clone_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-clone"'
test_clone_CFLAGS		= $(LFS_CFLAGS)

test_connld_SOURCES		= src/test/test-connld.c
test_connld_LDFLAGS		=
test_connld_LDADD		= libstreams.la
test_connld_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-connld"'
test_connld_CFLAGS		= $(LFS_CFLAGS)

test_echo_SOURCES		= src/test/test-echo.c
test_echo_LDFLAGS		=
test_echo_LDADD			= libstreams.la
test_echo_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-echo"'
test_echo_CFLAGS		= $(LFS_CFLAGS)

test_fifo_SOURCES		= src/test/test-fifo.c
test_fifo_LDFLAGS		=
test_fifo_LDADD			= libstreams.la
test_fifo_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-fifo"'
test_fifo_CFLAGS		= $(LFS_CFLAGS)

test_log_SOURCES		= src/test/test-log.c
test_log_LDFLAGS		=
test_log_LDADD			= libstreams.la
test_log_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-log"'
test_log_CFLAGS			= $(LFS_CFLAGS)

test_loop_SOURCES		= src/test/test-loop.c
test_loop_LDFLAGS		=
test_loop_LDADD			= libstreams.la
test_loop_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-loop"'
test_loop_CFLAGS		= $(LFS_CFLAGS)

test_nsdev_SOURCES		= src/test/test-nsdev.c
test_nsdev_LDFLAGS		=
test_nsdev_LDADD		= libstreams.la
test_nsdev_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-nsdev"'
test_nsdev_CFLAGS		= $(LFS_CFLAGS)

test_mux_SOURCES		= src/test/test-mux.c
test_mux_LDFLAGS		=
test_mux_LDADD			= libstreams.la
test_mux_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-mux"'
test_mux_CFLAGS			= $(LFS_CFLAGS)

test_nuls_SOURCES		= src/test/test-nuls.c
test_nuls_LDFLAGS		=
test_nuls_LDADD			= libstreams.la
test_nuls_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-nuls"'
test_nuls_CFLAGS		= $(LFS_CFLAGS)

test_pipe_SOURCES		= src/test/test-pipe.c
test_pipe_LDFLAGS		=
test_pipe_LDADD			= libstreams.la
test_pipe_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-pipe"'
test_pipe_CFLAGS		= $(LFS_CFLAGS)

perftest_SOURCES		= src/test/perftest.c
perftest_LDFLAGS		=
perftest_LDADD			= libstreams.la
perftest_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"perftest"'
perftest_CFLAGS			= $(LFS_CFLAGS)

perftestn_SOURCES		= src/test/perftest.c
perftestn_LDFLAGS		= 
perftestn_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNATIVE_PIPES -DNAME='"perftestn"'
perftestn_CFLAGS		= $(LFS_CFLAGS)

test_pipemod_SOURCES		= src/test/test-pipemod.c
test_pipemod_LDFLAGS		=
test_pipemod_LDADD		= libstreams.la
test_pipemod_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-pipemode"'
test_pipemod_CFLAGS		= $(LFS_CFLAGS)

test_sad_SOURCES		= src/test/test-sad.c
test_sad_LDFLAGS		=
test_sad_LDADD			= libstreams.la
test_sad_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sad"'
test_sad_CFLAGS			= $(LFS_CFLAGS)

test_sc_SOURCES			= src/test/test-sc.c
test_sc_LDFLAGS			=
test_sc_LDADD			= libstreams.la
test_sc_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sc"'
test_sc_CFLAGS			= $(LFS_CFLAGS)

test_streams_SOURCES		= src/test/test-streams.c
test_streams_LDFLAGS		=
test_streams_LDADD		= libstreams.la
test_streams_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-streams"'
test_streams_CFLAGS		= $(LFS_CFLAGS)

testlog_SOURCES			= src/test/testlog.c
testlog_LDFLAGS			=
testlog_LDADD			= libstreams.la
testlog_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"testlog"'
testlog_CFLAGS			= $(LFS_CFLAGS)

## PKG_BUILD_ARCH
endif
## RPM_BUILD_USER
endif

if CONFIG_STREAMS_SEPARATE_COMPILE
EXPORT_SRCFILES			= \
				  src/kernel/strattach.c \
				  src/kernel/strlookup.c \
				  src/kernel/strpipe.c \
				  src/kernel/strreg.c \
				  src/kernel/strsad.c \
				  src/kernel/strsched.c \
				  src/kernel/strspecfs.c \
				  src/kernel/strsysctl.c \
				  src/kernel/strutil.c \
				  src/kernel/strsyms.c

if CONFIG_STREAMS_CLONE
EXPORT_SRCFILES			+= src/drivers/clone.c
endif
if CONFIG_STREAMS_STH
EXPORT_SRCFILES			+= src/modules/sth.c
endif
else
EXPORT_SRCFILES			= \
				  src/kernel/specfs.c \
				  src/kernel/streams.c
endif

if CONFIG_STREAMS_CLONE_MODULE
EXPORT_SRCFILES			+= src/drivers/clone.c
endif
if CONFIG_STREAMS_STH_MODULE
EXPORT_SRCFILES			+= src/modules/sth.c
endif

if CONFIG_STREAMS_SEPARATE_COMPILE
EXPORT_VERFILES			= \
				  $(PKGINCL)/versions/strattach.ver \
				  $(PKGINCL)/versions/strlookup.ver \
				  $(PKGINCL)/versions/strpipe.ver \
				  $(PKGINCL)/versions/strreg.ver \
				  $(PKGINCL)/versions/strsad.ver \
				  $(PKGINCL)/versions/strsched.ver \
				  $(PKGINCL)/versions/strspecfs.ver \
				  $(PKGINCL)/versions/strsysctl.ver \
				  $(PKGINCL)/versions/strutil.ver \
				  $(PKGINCL)/versions/strsyms.ver
if CONFIG_STREAMS_CLONE
EXPORT_VERFILES			+= $(PKGINCL)/versions/clone.ver
endif
if CONFIG_STREAMS_STH
EXPORT_VERFILES			+= $(PKGINCL)/versions/sth.ver
endif
else
EXPORT_VERFILES			= \
				  $(PKGINCL)/versions/specfs.ver \
				  $(PKGINCL)/versions/streams.ver
endif

if CONFIG_STREAMS_CLONE_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/clone.ver
endif
if CONFIG_STREAMS_STH_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/sth.ver
endif

GENKSYMS_CPPFLAGS		= \
				  -I./src/kernel -I$(srcdir)/src/kernel \
				  -I./src/drivers -I$(srcdir)/src/drivers \
				  -I./src/modules -I$(srcdir)/src/modules \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS)

EXTRA_DIST			+= \
				  $(top_srcdir)/scripts/hot.lds

##
## Rules for generating versioned kernel symbols
##
include am/genksyms.am

##
## Rules for generating STREAMS configuration
##
include am/strconf.am

if RPM_BUILD_USER
## RPM_BUILD_USER

SUBDIRS = src

if PKG_BUILD_ARCH
SUBDIRS += intl po
CLEANFILES += po/stamp-po
endif

SUBDIRS += .

if WITH_32BIT_LIBS
SUBDIRS += lib32
endif

SUBDIRS += tests

if DEVELOPMENT
if DOCUMENTATION
SUBDIRS += doc
endif
endif

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

if INSTALL_INITSCRIPTS
## INSTALL_INITSCRIPTS

if WITH_RCSD_DIRECTORY
## WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= \
				  src/util/init/specfs.sh \
				  src/util/init/streams.sh
if CONFIG_STREAMS_LOG
dist_initrd_SCRIPTS		+= src/util/init/strace.sh \
				   src/util/init/strerr.sh
endif
if CONFIG_STREAMS_LOG_MODULE
dist_initrd_SCRIPTS		+= src/util/init/strace.sh \
				   src/util/init/strerr.sh
endif

## WITH_RCSD_DIRECTORY
else
## ! WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= \
				  src/util/init/specfs \
				  src/util/init/streams
if CONFIG_STREAMS_LOG
dist_initrd_SCRIPTS		+= src/util/init/strace \
				   src/util/init/strerr
endif
if CONFIG_STREAMS_LOG_MODULE
dist_initrd_SCRIPTS		+= src/util/init/strace \
				   src/util/init/strerr
endif

## ! WITH_RCSD_DIRECTORY
endif

dist_sysconf_DATA		= \
				  src/util/sysconfig/streams.conf
if CONFIG_STREAMS_UTILS
dist_sysconf_DATA		+= \
				  strsetup.conf \
				  strload.conf
endif

dist_config_DATA		= \
				  src/util/sysconfig/specfs \
				  src/util/sysconfig/streams
if CONFIG_STREAMS_LOG
dist_config_DATA		+= src/util/sysconfig/strace \
				   src/util/sysconfig/strerr
endif
if CONFIG_STREAMS_LOG_MODULE
dist_config_DATA		+= src/util/sysconfig/strace \
				   src/util/sysconfig/strerr
endif

## INSTALL_INITSCRIPTS
endif

dist_snmpmib_DATA		= 

if WITH_SNMP_AGENT
## WITH_SNMP_AGENT
dist_snmpmib_DATA		+= \
				  src/mibs/ALARM-MIB.mib \
				  src/mibs/ARC-MIB.mib \
				  src/mibs/ATM-APS-MIB.mib \
				  src/mibs/ATMF-CES.mib \
				  src/mibs/ATMF-CES-MIB.mib \
				  src/mibs/ATM-FORUM-ADDR-REG.mib \
				  src/mibs/ATM-FORUM-AUTO-CONFIG.mib \
				  src/mibs/ATM-FORUM-MIB.mib \
				  src/mibs/ATM-FORUM-SNMP-M4-MIB.mib \
				  src/mibs/ATM-FORUM-SRVC-REG.mib \
				  src/mibs/ATM-FORUM-TC-MIB.mib \
				  src/mibs/ATM-SOFT-PVC-MIB.mib \
				  src/mibs/ATM-SOFT-PVC-MIB.old.mib \
				  src/mibs/ATM-TRACE-MIB.mib \
				  src/mibs/CLNS-MIB.mib \
				  src/mibs/HC-PerfHist-TC-MIB.mib \
				  src/mibs/IMA-MIB.mib \
				  src/mibs/LAN-EMULATION-CLIENT-MIB.mib \
				  src/mibs/MPOA-MIB.mib \
				  src/mibs/OPENSS7-ACSE-MIB.mib \
				  src/mibs/OPENSS7-CDR2-MIB.mib \
				  src/mibs/OPENSS7-CDR-MIB.mib \
				  src/mibs/OPENSS7-CLNS-MIB.mib \
				  src/mibs/OPENSS7-CONS-MIB.mib \
				  src/mibs/OPENSS7-DL-MIB.mib \
				  src/mibs/OPENSS7-GMI-MIB.mib \
				  src/mibs/OPENSS7-ISO10733-MIB.mib \
				  src/mibs/OPENSS7-ISUP-MIB.mib \
				  src/mibs/OPENSS7-ISUP-OM-MIB.mib \
				  src/mibs/OPENSS7-LAPB-MIB.mib \
				  src/mibs/OPENSS7-LLC2-MIB.mib \
				  src/mibs/OPENSS7-M2PA-MIB.mib \
				  src/mibs/OPENSS7-M2UA-ASP-MIB.mib \
				  src/mibs/OPENSS7-M2UA-MIB.mib \
				  src/mibs/OPENSS7-M2UA-SL-MIB.mib \
				  src/mibs/OPENSS7-M3UA-ASP-MIB.mib \
				  src/mibs/OPENSS7-M3UA-SGP-MIB.mib \
				  src/mibs/OPENSS7-M3UA-TC.mib \
				  src/mibs/OPENSS7-MAP-MIB.mib \
				  src/mibs/OPENSS7-METRIC-MIB.mib \
				  src/mibs/OPENSS7-MIB.mib \
				  src/mibs/OPENSS7-MTP-MIB.mib \
				  src/mibs/OPENSS7-MTP-OM-MIB2.mib \
				  src/mibs/OPENSS7-MTP-OM-MIB.mib \
				  src/mibs/OPENSS7-MTP-TC.mib \
				  src/mibs/OPENSS7-MX-MIB.mib \
				  src/mibs/OPENSS7-PHY-MIB.mib \
				  src/mibs/OPENSS7-Q821-MIB.mib \
				  src/mibs/OPENSS7-SCCP2-MIB.mib \
				  src/mibs/OPENSS7-SCCP-MIB.mib \
				  src/mibs/OPENSS7-SCCP-OM-MIB.mib \
				  src/mibs/OPENSS7-SIGTRAN-MIB.mib \
				  src/mibs/OPENSS7-SIGTRAN-TC.mib \
				  src/mibs/OPENSS7-SL-MIB.mib \
				  src/mibs/OPENSS7-SL-MUX-MIB.mib \
				  src/mibs/OPENSS7-SMI.mib \
				  src/mibs/OPENSS7-SMI-MIB.mib \
				  src/mibs/OPENSS7-SNARE-MIB.mib \
				  src/mibs/OPENSS7-SS7-MIB.mib \
				  src/mibs/OPENSS7-SS7-OM-MIB.mib \
				  src/mibs/OPENSS7-SS7-TC.mib \
				  src/mibs/OPENSS7-STREAMS-EXT-MIB.mib \
				  src/mibs/OPENSS7-STREAMS-MIB.mib \
				  src/mibs/OPENSS7-TCAP2-MIB.mib \
				  src/mibs/OPENSS7-TCAP-MIB.mib \
				  src/mibs/OPENSS7-TCAP-OM-MIB.mib \
				  src/mibs/OPENSS7-TC.mib \
				  src/mibs/OPENSS7-TMN-SMI.mib \
				  src/mibs/OPENSS7-TMN-TC.mib \
				  src/mibs/OPENSS7-UMC-MIB.mib \
				  src/mibs/OPENSS7-X25-LLC-MIB.mib \
				  src/mibs/OPENSS7-X25-PLP-MIB.mib \
				  src/mibs/OPENSS7-X400P-MIB.mib \
				  src/mibs/OPENSS7-X400P-MX-MIB.mib \
				  src/mibs/PerfHist-TC-MIB.mib \
				  src/mibs/PNNI-MIB.mib \
				  src/mibs/PNNI-MOB-EXT-MIB.mib \
				  src/mibs/RFC1381-MIB.mib \
				  src/mibs/RFC1382-MIB.mib \
				  src/mibs/RFC1513-MIB.mib \
				  src/mibs/RFC2127-MIB.txt \
				  src/mibs/RFC2128-MIB.txt \
				  src/mibs/RFC2494-EXT-MIB.mib \
				  src/mibs/RFC2494-MIB.txt \
				  src/mibs/RFC2495-MIB.txt \
				  src/mibs/RFC2496-MIB.txt \
				  src/mibs/RFC2514-MIB.mib \
				  src/mibs/RFC2515-MIB.mib \
				  src/mibs/RFC3593-MIB.txt \
				  src/mibs/RFC3873-MIB.txt \
				  src/mibs/RFC3877-MIB.txt \
				  src/mibs/RFC3878-MIB.txt \
				  src/mibs/RFC3895-MIB.txt \
				  src/mibs/RFC3896-MIB.txt \
				  src/mibs/RFC4133-MIB.mib \
				  src/mibs/RFC4502-MIB.mib \
				  src/mibs/RFC4805-EXT-MIB.mib \
				  src/mibs/RFC4805-MIB.txt \
				  src/mibs/RS-232-MIB.mib

## WITH_SNMP_AGENT
endif

## PKG_BUILD_INDEP
endif

## RPM_BUILD_USER
endif

##
## Rules for init scripts
##
include am/init.am

##
## Rules for lsms
##
include am/lsm.am

##
## Rules for archives
##
include am/archive.am

##
## Rules for making rpms
##
AM_RPMFLAGS			= --define "_kversion $(kversion)"
AM_SRPMFLAGS			= $(AM_RPMFLAGS)

include am/rpm.am

##
## Rules for making deb packages
##
include am/deb.am

##
## Rules for making release files
##
MANUAL_FILE			= doc/manual/$(PACKAGE).txt

include am/release.am

##
## Rules for doxygen documentation.
##
include am/doxy.am

##
## Rules for SNMP modules.
##
SNMPORDER			= 01
include am/snmp.am

EXTRA_DIST			+= \
				  debian/README.Debian \
				  debian/TODO.Debian

MAINTAINERCLEANFILES		+= \
				  $(top_srcdir)/aclocal.m4 \
				  $(top_srcdir)/config.h.in \
				  $(top_srcdir)/configure \
				  $(top_srcdir)/COPYING \
				  $(top_srcdir)/INSTALL \
				  $(top_srcdir)/scripts/compile \
				  $(top_srcdir)/scripts/config.guess \
				  $(top_srcdir)/scripts/config.sub \
				  $(top_srcdir)/scripts/depcomp \
				  $(top_srcdir)/scripts/install-sh \
				  $(top_srcdir)/scripts/ltmain.sh \
				  $(top_srcdir)/scripts/missing \
				  $(top_srcdir)/scripts/texinfo.tex

# =============================================================================
#
# $Log$
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
