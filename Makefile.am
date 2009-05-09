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

sbin_PROGRAMS			=

sbin_PROGRAMS			+= @PACKAGE_TARNAME@_mknod
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
sbin_PROGRAMS			+= insf strvf
endif

if WITH_SNMP_AGENT
sbin_PROGRAMS			+= streamsd
endif

sbin_PROGRAMS			+= ldlconfig

sbin_PROGRAMS			+= initsock soconfig

EXTRA_PROGRAMS			+= chand
if WITH_SNMP_AGENT
sbin_PROGRAMS			+= chand
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

pkglibexec_PROGRAMS		=

pkglibexec_PROGRAMS		+= testlog
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
pkglibexec_PROGRAMS		+= test-tty
pkglibexec_PROGRAMS		+= ldltest test-np_ip test-ip
pkglibexec_PROGRAMS		+= test-tirdwr test-timod test-xnet test-xnet_thr
pkglibexec_PROGRAMS		+= test-xnsl test-xnsl_thr
pkglibexec_PROGRAMS		+= test-sockpath test-socket test-socklib test-sockmod test-socksys
pkglibexec_PROGRAMS		+= test-inet_raw test-inet_udp test-inet_tcp test-inet_sctp
pkglibexec_PROGRAMS		+= test-sctp test-sctp_n test-sctp_n2 test-sctp_nc test-sctp_ns test-sctp_t test-etsi_n test-etsi_t test-interop
pkglibexec_PROGRAMS		+= test-chan

EXTRA_PROGRAMS			+=

#LFS_EXTRA_TESTS		= 
#LFS_EXTRA_TOOLS		= @PACKAGE_TARNAME@_mknod

#LDADD				= libstreams.la

lib_LTLIBRARIES			= libstreams.la \
				  libLiS.la \
				  libpLiS.la \
				  libcdiapi.la \
				  libdlpiapi.la \
				  libnpiapi.la \
				  libdlpi.la \
				  libxnet.la \
				  libxnsl.la \
				  inetaddr.la \
				  libsockpath.la \
				  libsocket.la \
				  libsocklib.la

lib32_LTLIBRARIES		=

snmpdlmod_LTLIBRARIES		=

if WITH_SNMP_AGENT
snmpdlmod_LTLIBRARIES		+= strMIB.la
snmpdlmod_LTLIBRARIES		+= mxMIB.la
endif

## RPM_BUILD_USER
endif

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

noinst_LIBRARIES		= \
				libLfS-specfs.a \
				libLfS-streams.a \
				libLfS-drivers.a \
				libLfS-modules.a \
				libCOMPATlink.a \
				libCOMPATmods.a \
				libTTYlink.a \
				libTTYmods.a \
				libTTYdrvs.a \
				libXNSmods.a \
				libXNSdrvs.a \
				libXNETmods.a \
				libSOCKlink.a \
				libSOCKmods.a \
				libSOCKdrvs.a \
				libINETmods.a \
				libSCTPsctp.a \
				libSCTPtest.a \
				libCHANlink.a \
				libCHANmods.a \
				libCHANdrvs.a

libLfS_specfs_a_SOURCES		= \
				src/kernel/strspecfs.h \
				src/kernel/strlookup.h \
				src/kernel/strattach.h \
				src/kernel/strpipe.h \
				src/kernel/strargs.h

if CONFIG_STREAMS_SEPARATE_COMPILE
## CONFIG_STREAMS_SEPARATE_COMPILE

libLfS_specfs_a_SOURCES		+= \
				src/kernel/strspecfs.c \
				src/kernel/strlookup.c \
				src/kernel/strattach.c \
				src/kernel/strpipe.c

## CONFIG_STREAMS_SEPARATE_COMPILE
else
## !CONFIG_STREAMS_SEPARATE_COMPILE

libLfS_specfs_a_SOURCES		+= \
				src/kernel/specfs.c

## !CONFIG_STREAMS_SEPARATE_COMPILE
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
## CONFIG_STREAMS_SEPARATE_COMPILE

libLfS_streams_a_SOURCES	+= \
				src/kernel/strmain.c \
				src/kernel/strreg.c \
				src/kernel/strprocfs.c \
				src/kernel/strsched.c \
				src/kernel/strsysctl.c \
				src/kernel/strsyms.c \
				src/kernel/strutil.c \
				src/kernel/strsad.c

## CONFIG_STREAMS_SEPARATE_COMPILE
else
## !CONFIG_STREAMS_SEPARATE_COMPILE

libLfS_streams_a_SOURCES	+= \
				src/kernel/streams.c

## !CONFIG_STREAMS_SEPARATE_COMPILE
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

EXTRA_libCOMPATlink_a_SOURCES 	= \
				  src/kernel/os7compat.c \
				  src/kernel/svr3compat.c \
				  src/kernel/svr4compat.c \
				  src/kernel/mpscompat.c \
				  src/kernel/suncompat.c \
				  src/kernel/uw7compat.c \
				  src/kernel/osfcompat.c \
				  src/kernel/aixcompat.c \
				  src/kernel/hpuxcompat.c \
				  src/kernel/irixcompat.c \
				  src/kernel/liscompat.c \
				  src/kernel/maccompat.c

libCOMPATlink_a_SOURCES		= \
				  src/include/sys/os7/allocb.h \
				  src/include/sys/os7/bufpool.h \
				  src/include/sys/os7/bufq.h \
				  src/include/sys/os7/compat.h \
				  src/include/sys/os7/debug.h \
				  src/include/sys/os7/lock.h \
				  src/include/sys/os7/priv.h \
				  src/include/sys/os7/queue.h \
				  src/include/sys/os7/timer.h

libCOMPATlink_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)

if CONFIG_STREAMS_COMPAT_OS7
libCOMPATlink_a_SOURCES		+= src/kernel/os7compat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/os7/ddi.h src/include/sys/os7/strconf.h src/include/sys/os7/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR3
libCOMPATlink_a_SOURCES		+= src/kernel/svr3compat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/svr3/ddi.h src/include/sys/svr3/strconf.h src/include/sys/svr3/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR4
libCOMPATlink_a_SOURCES		+= src/kernel/svr4compat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/svr4/ddi.h src/include/sys/svr4/strconf.h src/include/sys/svr4/stream.h
endif
if CONFIG_STREAMS_COMPAT_MPS
libCOMPATlink_a_SOURCES		+= src/kernel/mpscompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/mps/ddi.h src/include/sys/mps/strconf.h src/include/sys/mps/stream.h
endif
if CONFIG_STREAMS_COMPAT_SUN
libCOMPATlink_a_SOURCES		+= src/kernel/suncompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/sunddi.h src/include/sys/strsun.h src/include/sys/sun/ddi.h src/include/sys/sun/strconf.h src/include/sys/sun/strsun.h src/include/sys/sun/stream.h
endif
if CONFIG_STREAMS_COMPAT_UW7
libCOMPATlink_a_SOURCES		+= src/kernel/uw7compat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/uw7/ddi.h src/include/sys/uw7/strconf.h src/include/sys/uw7/stream.h
endif
if CONFIG_STREAMS_COMPAT_OSF
libCOMPATlink_a_SOURCES		+= src/kernel/osfcompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/osf/ddi.h src/include/sys/osf/strconf.h src/include/sys/osf/stream.h
endif
if CONFIG_STREAMS_COMPAT_AIX
libCOMPATlink_a_SOURCES		+= src/kernel/aixcompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/aix/ddi.h src/include/sys/aix/strconf.h src/include/sys/aix/stream.h
endif
if CONFIG_STREAMS_COMPAT_HPUX
libCOMPATlink_a_SOURCES		+= src/kernel/hpuxcompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/hpux/ddi.h src/include/sys/hpux/strconf.h src/include/sys/hpux/stream.h
endif
if CONFIG_STREAMS_COMPAT_IRIX
libCOMPATlink_a_SOURCES		+= src/kernel/irixcompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/irix/ddi.h src/include/sys/irix/strconf.h src/include/sys/irix/stream.h
endif
if CONFIG_STREAMS_COMPAT_LIS
libCOMPATlink_a_SOURCES		+= src/kernel/liscompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/lis/ddi.h src/include/sys/lis/strconf.h src/include/sys/lis/stream.h
endif
if CONFIG_STREAMS_COMPAT_MAC
libCOMPATlink_a_SOURCES		+= src/kernel/maccompat.c
libCOMPATlink_a_DEPENDENCIES	+= src/include/sys/mac/ddi.h src/include/sys/mac/strconf.h src/include/sys/mac/stream.h
endif

libCOMPATlink_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libCOMPATlink_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libCOMPATlink_a_OBJECTS):	$(libCOMPATlink_a_SOURCES) $(libCOMPATlink_a_DEPENDENCIES)

EXTRA_libCOMPATmods_a_SOURCES	= \
				  src/kernel/os7compat.c \
				  src/kernel/svr3compat.c \
				  src/kernel/svr4compat.c \
				  src/kernel/mpscompat.c \
				  src/kernel/suncompat.c \
				  src/kernel/uw7compat.c \
				  src/kernel/osfcompat.c \
				  src/kernel/aixcompat.c \
				  src/kernel/hpuxcompat.c \
				  src/kernel/irixcompat.c \
				  src/kernel/liscompat.c \
				  src/kernel/maccompat.c

libCOMPATmods_a_SOURCES		= \
				  src/include/sys/os7/allocb.h \
				  src/include/sys/os7/bufpool.h \
				  src/include/sys/os7/bufq.h \
				  src/include/sys/os7/compat.h \
				  src/include/sys/os7/debug.h \
				  src/include/sys/os7/lock.h \
				  src/include/sys/os7/priv.h \
				  src/include/sys/os7/queue.h \
				  src/include/sys/os7/timer.h

libCOMPATmods_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)

if CONFIG_STREAMS_COMPAT_OS7_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/os7compat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/os7/ddi.h src/include/sys/os7/strconf.h src/include/sys/os7/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR3_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/svr3compat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/svr3/ddi.h src/include/sys/svr3/strconf.h src/include/sys/svr3/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR4_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/svr4compat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/svr4/ddi.h src/include/sys/svr4/strconf.h src/include/sys/svr4/stream.h
endif
if CONFIG_STREAMS_COMPAT_MPS_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/mpscompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/mps/ddi.h src/include/sys/mps/strconf.h src/include/sys/mps/stream.h
endif
if CONFIG_STREAMS_COMPAT_SUN_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/suncompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/sunddi.h src/include/sys/strsun.h src/include/sys/sun/ddi.h src/include/sys/sun/strconf.h src/include/sys/sun/strsun.h src/include/sys/sun/stream.h
endif
if CONFIG_STREAMS_COMPAT_UW7_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/uw7compat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/uw7/ddi.h src/include/sys/uw7/strconf.h src/include/sys/uw7/stream.h
endif
if CONFIG_STREAMS_COMPAT_OSF_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/osfcompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/osf/ddi.h src/include/sys/osf/strconf.h src/include/sys/osf/stream.h
endif
if CONFIG_STREAMS_COMPAT_AIX_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/aixcompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/aix/ddi.h src/include/sys/aix/strconf.h src/include/sys/aix/stream.h
endif
if CONFIG_STREAMS_COMPAT_HPUX_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/hpuxcompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/hpux/ddi.h src/include/sys/hpux/strconf.h src/include/sys/hpux/stream.h
endif
if CONFIG_STREAMS_COMPAT_IRIX_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/irixcompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/irix/ddi.h src/include/sys/irix/strconf.h src/include/sys/irix/stream.h
endif
if CONFIG_STREAMS_COMPAT_LIS_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/liscompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/lis/ddi.h src/include/sys/lis/strconf.h src/include/sys/lis/stream.h
endif
if CONFIG_STREAMS_COMPAT_MAC_MODULE
libCOMPATmods_a_SOURCES		+= src/kernel/maccompat.c
libCOMPATmods_a_SOURCES		+= src/include/sys/mac/ddi.h src/include/sys/mac/strconf.h src/include/sys/mac/stream.h
endif

libCOMPATmods_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libCOMPATmods_a_CFLAGS		= $(KERNEL_CFLAGS) -Wno-deprecated-declarations -DKBUILD_MODNAME=\"$(kpre)`echo $@ | sed -e 's,lib.*_a-,,;s,\.o,,;s,-,_,g'`\"

$(libCOMPATmods_a_OBJECTS):	$(libCOMPATmods_a_DEPENDENCIES)

EXTRA_libTTYlink_a_SOURCES	= 
libTTYlink_a_SOURCES		= 
libTTYlink_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libTTYlink_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libTTYlink_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libTTYlink_a_OBJECTS):	$(libTTYlink_a_SOURCES) $(libTTYlink_a_DEPENDENCIES)

EXTRA_libTTYmods_a_SOURCES	= src/modules/pckt.c \
				  src/modules/ptem.c \
				  src/modules/ttcompat.c \
				  src/modules/ldterm.c
libTTYmods_a_SOURCES		= $(EXTRA_libTTYmods_a_SOURCES)
libTTYmods_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libTTYmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libTTYmods_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libTTYmods_a_OBJECTS):	$(libTTYmods_a_DEPENDENCIES)

EXTRA_libTTYdrvs_a_SOURCES	= src/drivers/pty.c 
libTTYdrvs_a_SOURCES		= $(EXTRA_libTTYdrvs_a_SOURCES)
libTTYdrvs_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libTTYdrvs_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libTTYdrvs_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libTTYdrvs_a_OBJECTS):	$(libTTYdrvs_a_DEPENDENCIES)

EXTRA_libXNSmods_a_SOURCES 	= src/modules/debug.h \
				  src/modules/ip_strm_mod.c \
				  src/modules/ip_strm_mod.h
nodist_libXNSmods_a_SOURCES	= $(EXTRA_libXNSmods_a_SOURCES)
libXNSmods_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libXNSmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libXNSmods_a_CFLAGS		= $(KERNEL_CFLAGS)

nodist_EXTRA_libXNSdrvs_a_SOURCES = src/drivers/debug.h \
				  src/drivers/ip_to_dlpi.c \
				  src/drivers/ldl.c \
				  src/drivers/np_ip.c
dist_EXTRA_libXNSdrvs_a_SOURCES = \
				  src/drivers/cd.c \
				  src/drivers/dl.c \
				  src/drivers/dl.h \
				  src/drivers/dl_ip.c \
				  src/drivers/dlp.c \
				  src/drivers/ip.c \
				  src/drivers/nf.c \
				  src/drivers/np.c \
				  src/drivers/np.h
nodist_libXNSdrvs_a_SOURCES	= $(nodist_EXTRA_libXNSdrvs_a_SOURCES)
libXNSdrvs_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) src/drivers/ip_hooks.h
libXNSdrvs_a_CPPFLAGS		= -I$(top_builddir)/src/drivers $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libXNSdrvs_a_CFLAGS		= $(KERNEL_CFLAGS) -DKBUILD_MODNAME=\"$(kpre)`echo $@ | sed -e 's,lib.*_a-,,;s,\.o,,;s,-,_,g'`\"
libXNSdrvs_a_OBJECTS:		$(libXNSdrvs_a_DEPENDENCIES)

EXTRA_libXNETmods_a_SOURCES 	= src/modules/tirdwr.c \
				  src/modules/timod.c
nodist_libXNETmods_a_SOURCES	= $(EXTRA_libXNETmods_a_SOURCES)
libXNETmods_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libXNETmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libXNETmods_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libXNETmods_a_OBJECTS):	$(libXNETmods_a_DEPENDENCIES)

EXTRA_libSOCKlink_a_SOURCES	= src/modules/sockmod.c \
				  src/drivers/socksys.c
libSOCKlink_a_SOURCES		= 
libSOCKlink_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libSOCKlink_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libSOCKlink_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libSOCKlink_a_OBJECTS):	$(libSOCKlink_a_SOURCES) $(libSOCKlink_a_DEPENDENCIES)

EXTRA_libSOCKmods_a_SOURCES	= src/modules/sockmod.c
libSOCKmods_a_SOURCES		= src/modules/sockmod.c
libSOCKmods_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libSOCKmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libSOCKmods_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libSOCKmods_a_OBJECTS):	$(libSOCKmods_a_DEPENDENCIES)

EXTRA_libSOCKdrvs_a_SOURCES	= src/drivers/socksys.c
libSOCKdrvs_a_SOURCES		= src/drivers/socksys.c
libSOCKdrvs_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libSOCKdrvs_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libSOCKdrvs_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libSOCKdrvs_a_OBJECTS):	$(libSOCKdrvs_a_DEPENDENCIES)

nodist_EXTRA_libINETmods_a_SOURCES = \
				  src/drivers/inet.c \
				  src/drivers/udp.c \
				  src/drivers/rawip.c \
				  src/drivers/tcp.c
dist_EXTRA_libINETmods_a_SOURCES = \
				  src/modules/cmot.c \
				  src/modules/isot.c \
				  src/modules/itot.c \
				  src/modules/lpp.c \
				  src/modules/tcpns.c \
				  src/modules/xot.c
nodist_libINETmods_a_SOURCES	= $(nodist_EXTRA_libINETmods_a_SOURCES)
libINETmods_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) src/drivers/udp_hooks.h
libINETmods_a_CPPFLAGS		= -I$(top_builddir)/src/drivers $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libINETmods_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libINETmods_a_OBJECTS):	$(libINETmods_a_DEPENDENCIES)

nodist_EXTRA_libSCTPtest_a_SOURCES = \
				  src/modules/tpiperf.c
nodist_libSCTPtest_a_SOURCES	= $(nodist_EXTRA_libSCTPtest_a_SOURCES)
libSCTPtest_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libSCTPtest_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS)
libSCTPtest_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libSCTPtest_a_OBJECTS):	$(libSCTPtest_a_DEPENDENCIES)

sctp_source_FILES		= \
				  src/drivers/sctp.c \
				  src/drivers/sctp_cache.c \
				  src/drivers/sctp_cookie.c \
				  src/drivers/sctp_hash.c \
				  src/drivers/sctp_input.c \
				  src/drivers/sctp_msg.c \
				  src/drivers/sctp_n.c \
				  src/drivers/sctp_output.c \
				  src/drivers/sctp_route.c \
				  src/drivers/sctp_t.c \
				  src/drivers/sctp_md5.c \
				  src/drivers/sctp_hmac_md5.c \
				  src/drivers/sctp_sha1.c \
				  src/drivers/sctp_hmac_sha1.c \
				  src/drivers/sctp_crc32c.c \
				  src/drivers/sctp_md5.h \
				  src/drivers/sctp_hmac_md5.h \
				  src/drivers/sctp_sha1.h \
				  src/drivers/sctp_hmac_sha1.h \
				  src/drivers/sctp_crc32c.h \
				  src/drivers/sctp_cache.h \
				  src/drivers/sctp_cookie.h \
				  src/drivers/sctp_defs.h \
				  src/drivers/sctp.h \
				  src/drivers/sctp_hash.h \
				  src/drivers/sctp_input.h \
				  src/drivers/sctp_msg.h \
				  src/drivers/sctp_n.h \
				  src/drivers/sctp_output.h \
				  src/drivers/sctp_route.h \
				  src/drivers/sctp_t.h
sctp2_source_FILES		= \
				  src/drivers/sctp2.c \
				  src/drivers/sctp_compat.h \
				  src/drivers/sctp_md5.c \
				  src/drivers/sctp_md5.h \
				  src/drivers/sctp_hmac_md5.c \
				  src/drivers/sctp_hmac_md5.h \
				  src/drivers/sctp_sha1.c \
				  src/drivers/sctp_sha1.h \
				  src/drivers/sctp_hmac_sha1.c \
				  src/drivers/sctp_hmac_sha1.h \
				  src/drivers/sctp_adler32.c \
				  src/drivers/sctp_adler32.h \
				  src/drivers/sctp_crc32c.c \
				  src/drivers/sctp_crc32c.h
if WITH_SCTP
## WITH_SCTP
nodist_libSCTPsctp_a_SOURCES	= $(sctp_source_FILES)
## WITH_SCTP
endif
if WITH_SCTP2
## WITH_SCTP2
nodist_libSCTPsctp_a_SOURCES	= $(sctp2_source_FILES)
## WITH_SCTP2
endif
libSCTPsctp_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG) src/drivers/sctp_hooks.h
libSCTPsctp_a_CPPFLAGS		= -I$(top_builddir)/src/drivers -I$(top_srcdir)/src/drivers \
				  $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION)
libSCTPsctp_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libSCTPsctp_a_OBJECTS):	$(libSCTPsctp_a_DEPENDENCIES)

if WITH_KO_MODULES
streams_sctp.o: $(libSCTPsctp_a_OBJECTS)
	$(LD) -r -o $@ $^
else
streams-sctp.o: $(libSCTPsctp_a_OBJECTS)
	$(LD) -r -o $@ $^
endif

EXTRA_libCHANlink_a_SOURCES	= 
libCHANlink_a_SOURCES		= src/modules/ch_pmod.c \
				  src/modules/mx_pmod.c \
				  config.h strconf.h
libCHANlink_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libCHANlink_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libCHANlink_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libCHANlink_a_OBJECTS):	$(libCHANlink_a_DEPENDENCIES)

EXTRA_libCHANmods_a_SOURCES	=
libCHANmods_a_SOURCES		= 
libCHANmods_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libCHANmods_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libCHANmods_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libCHANmods_a_OBJECTS):	$(libCHANmods_a_DEPENDENCIES)

EXTRA_libCHANdrvs_a_SOURCES	=
libCHANdrvs_a_SOURCES		=
libCHANdrvs_a_DEPENDENCIES	= $(STRCONF_MODCONF) $(MODVERSIONS_H) $(CONFIG_HEADER) $(STRCONF_CONFIG)
libCHANdrvs_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) $(KERNEL_EXPSYMTAB)
libCHANdrvs_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libCHANdrvs_a_OBJECTS):	$(libCHANdrvs_a_DEPENDENCIES)

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
KERNEL_OBJECTS			= $(libLfS_specfs_a_OBJECTS) \
				  $(libLfS_streams_a_OBJECTS) \
				  $(libLfS_drivers_a_OBJECTS) \
				  $(libLfS_modules_a_OBJECTS) \
				  $(libCOMPATlink_a_OBJECTS) \
				  $(libCOMPATmods_a_OBJECTS) \
				  $(libTTYlink_a_OBJECTS) \
				  $(libTTYmods_a_OBJECTS) \
				  $(libTTYdrvs_a_OBJECTS) \
				  $(libXNSmods_a_OBJECTS) \
				  $(libXNSdrvs_a_OBJECTS) \
				  $(libXNETmods_a_OBJECTS) \
				  $(libSOCKlink_a_OBJECTS) \
				  $(libSOCKmods_a_OBJECTS) \
				  $(libSOCKdrvs_a_OBJECTS) \
				  $(libINETmods_a_OBJECTS) \
				  $(libSCTPtest_a_OBJECTS) \
				  $(libSCTPsctp_a_OBJECTS) \
				  $(libCHANlink_a_OBJECTS) \
				  $(libCHANmods_a_OBJECTS) \
				  $(libCHANdrvs_a_OBJECTS)
KERNEL_MODULES			= specfs.o \
				  streams.o \
				  $(libLfS_drivers_a_OBJECTS) \
				  $(libLfS_modules_a_OBJECTS) \
				  $(libCOMPATmods_a_OBJECTS) \
				  $(libTTYmods_a_OBJECTS) \
				  $(libTTYdrvs_a_OBJECTS) \
				  $(libXNSmods_a_OBJECTS) \
				  $(libXNSdrvs_a_OBJECTS) \
				  $(libXNETmods_a_OBJECTS) \
				  $(libSOCKmods_a_OBJECTS) \
				  $(libSOCKdrvs_a_OBJECTS) \
				  $(libINETmods_a_OBJECTS) \
				  $(libSCTPtest_a_OBJECTS) \
				  $(libCHANlink_a_OBJECTS) \
				  $(libCHANmods_a_OBJECTS) \
				  $(libCHANdrvs_a_OBJECTS)

if WITH_KO_MODULES
KERNEL_MODULES			+= streams_sctp.o
else
KERNEL_MODULES			+= streams-sctp.o
endif

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
				src/lib/streams.VERSION \
				src/lib/cdiapi.VERSION \
				src/lib/dlpiapi.VERSION \
				src/lib/npiapi.VERSION \
				src/lib/dlpi.VERSION \
				src/lib/xnet.VERSION \
				src/lib/xnsl.VERSION \
				src/lib/socklib.VERSION \
				src/lib/socket.VERSION \
				src/lib/sockpath.VERSION \
				src/lib/apli.VERSION \
				src/lib/osi.VERSION \
				src/lib/sx25.VERSION \
				src/lib/xap_rose.VERSION \
				src/lib/xap.VERSION \
				src/lib/xds.VERSION \
				src/lib/xftam.VERSION \
				src/lib/xmap.VERSION \
				src/lib/xmp.VERSION \
				src/lib/xom.VERSION \
				src/lib/xtcap.VERSION

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
libstreams_la_LDFLAGS		= -version-info 0:1:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/streams.VERSION
libstreams_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libstreams_la_CFLAGS		= $(USER_CFLAGS)

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
libLiS_la_LDFLAGS		= -version-info 0:1:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/streams.VERSION
libLiS_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libLiS_la_CFLAGS		= $(USER_CFLAGS)

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
libpLiS_la_LDFLAGS		= -version-info 0:1:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/streams.VERSION
libpLiS_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libpLiS_la_CFLAGS		= $(USER_CFLAGS)

libcdiapi_la_SOURCES		= src/lib/pthread.c \
				  src/lib/cdiapi.c \
				  src/include/cdiapi.h \
				  gettext.h
libcdiapi_la_DEPENDENCIES	= $(top_srcdir)/src/lib/cdiapi.VERSION
libcdiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/cdiapi.VERSION
libcdiapi_la_CPPFLAGS		= -I. -I$(top_srcdir)/src/lib \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libcdiapi_la_CFLAGS		= $(USER_CFLAGS)
libcdiapi_la_LIBADD		= libstreams.la

libdlpiapi_la_SOURCES		= src/lib/pthread.c \
				  src/lib/dlpiapi.c \
				  src/include/dlpiapi.h \
				  gettext.h
libdlpiapi_la_DEPENDENCIES	= $(top_srcdir)/src/lib/dlpiapi.VERSION
libdlpiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/dlpiapi.VERSION
libdlpiapi_la_CPPFLAGS		= -I. -I$(top_srcdir)/src/lib \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libdlpiapi_la_CFLAGS		= $(USER_CFLAGS)
libdlpiapi_la_LIBADD		= libstreams.la

libnpiapi_la_SOURCES		= src/lib/pthread.c \
				  src/lib/npiapi.c \
				  src/include/npiapi.h \
				  gettext.h
libnpiapi_la_DEPENDENCIES	= $(top_srcdir)/src/lib/npiapi.VERSION
libnpiapi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/npiapi.VERSION
libnpiapi_la_CPPFLAGS		= -I. -I$(top_srcdir)/src/lib \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libnpiapi_la_CFLAGS		= $(USER_CFLAGS)
libnpiapi_la_LIBADD		= libstreams.la

libdlpi_la_SOURCES		= src/lib/pthread.c \
				  src/lib/dlpi.c \
				  src/include/libdlpi.h \
				  gettext.h
libdlpi_la_DEPENDENCIES		= $(top_srcdir)/src/lib/dlpi.VERSION
libdlpi_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/dlpi.VERSION
libdlpi_la_CPPFLAGS		= -I. -I$(top_srcdir)/src/lib \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libdlpi_la_CFLAGS		= $(USER_CFLAGS)
libdlpi_la_LIBADD		= libstreams.la

libxnet_la_SOURCES		= src/lib/pthread.c \
				  src/lib/xnet.c \
				  gettext.h
libxnet_la_DEPENDENCIES		= $(top_srcdir)/src/lib/xnet.VERSION
libxnet_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/xnet.VERSION
libxnet_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) \
				  -I$(top_srcdir)/include/xti -I$(top_builddir)/include/xti
libxnet_la_CFLAGS		= $(USER_CFLAGS)
libxnet_la_LIBADD		= libstreams.la

libxnsl_la_SOURCES		= src/lib/pthread.c \
				  src/lib/netselect.c \
				  src/lib/netselect.h \
				  src/lib/netdir.c \
				  src/lib/netdir_inet.c \
				  src/include/netconfig.h \
				  src/include/netdir.h \
				  src/include/sys/netconfig.h \
				  gettext.h
libxnsl_la_DEPENDENCIES		= $(top_srcdir)/src/lib/xnsl.VERSION
libxnsl_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/xnsl.VERSION \
				  -ldl
libxnsl_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS)
libxnsl_la_CFLAGS		= $(USER_CFLAGS)
libxnsl_la_LIBADD		= libstreams.la libxnet.la

inetaddr_la_SOURCES		= src/lib/netdir_inet.c \
				  src/include/netdir.h \
				  src/include/netconfig.h \
				  src/include/sys/netconfig.h
inetaddr_la_DEPENDENCIES	= libxnsl.la
inetaddr_la_LDFLAGS		= -module -avoid-version \
				  -shared
inetaddr_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib \
				  $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) \
				  -DNETDIR_STRONG_DEFS
inetaddr_la_CFLAGS		= $(USER_CFLAGS)
inetaddr_la_LIBADD		= libstreams.la libxnet.la libxnsl.la

libsockpath_la_SOURCES		= src/lib/pthread.c \
				  src/lib/sockpath.c \
				  src/lib/sockpath.h \
				  src/include/sockdb.h \
				  src/include/sys/sockpath.h
libsockpath_la_DEPENDENCIES	= src/lib/sockpath.VERSION
libsockpath_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/sockpath.VERSION
libsockpath_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib \
				  $(PKG_INCLUDES) $(USER_CPPFLAGS)
libsockpath_la_CFLAGS		= $(USER_CFLAGS)
libsockpath_la_LIBADD		=

libsocket_la_SOURCES		= src/lib/pthread.c \
				  src/lib/socket.c \
				  gettext.h
libsocket_la_DEPENDENCIES	= src/lib/socket.VERSION
libsocket_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/socket.VERSION \
				  -static -lc -shared
libsocket_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib \
				  $(PKG_INCLUDES) $(USER_CPPFLAGS)
libsocket_la_CFLAGS		= $(USER_CFLAGS)
libsocket_la_LIBADD		= libstreams.la

libsocklib_la_SOURCES		= src/lib/pthread.c \
				  src/lib/socklib.c \
				  gettext.h
libsocklib_la_DEPENDENCIES	= src/lib/socklib.VERSION
libsocklib_la_LDFLAGS		= -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/socklib.VERSION \
				  -static -lc -shared
libsocklib_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib \
				  $(PKG_INCLUDES) $(USER_CPPFLAGS)
libsocklib_la_CFLAGS		= $(USER_CFLAGS)
libsocklib_la_LIBADD		= libstreams.la libxnet.la libxnsl.la libsockpath.la

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
				  -shared $(SNMP_LDFLAGS)
strMIB_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DMODULE
strMIB_la_CFLAGS		= $(USER_CFLAGS)
strMIB_la_LIBADD		= libstreams.la $(SNMP_LDADD)

mxMIB_la_SOURCES		= src/snmp/ds0Bundle.c \
				  src/snmp/ds0Bundle.h \
				  src/snmp/ds0.c \
				  src/snmp/ds0.h \
				  src/snmp/ds1EXT.c \
				  src/snmp/ds1EXT.h \
				  src/snmp/mxMIB.c \
				  src/snmp/mxMIB.h \
				  src/snmp/phyMIB.c \
				  src/snmp/phyMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h
mxMIB_la_LDFLAGS		= -module -avoid-version \
				  -shared $(SNMP_LDFLAGS)
mxMIB_la_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DMODULE
mxMIB_la_CFLAGS			= $(USER_CFLAGS)
mxMIB_la_LIBADD			= libstreams.la $(SNMP_LDADD)


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
test_clone_CFLAGS		= $(USER_CFLAGS)

test_connld_SOURCES		= src/test/test-connld.c
test_connld_LDFLAGS		=
test_connld_LDADD		= libstreams.la
test_connld_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-connld"'
test_connld_CFLAGS		= $(USER_CFLAGS)

test_echo_SOURCES		= src/test/test-echo.c
test_echo_LDFLAGS		=
test_echo_LDADD			= libstreams.la
test_echo_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-echo"'
test_echo_CFLAGS		= $(USER_CFLAGS)

test_fifo_SOURCES		= src/test/test-fifo.c
test_fifo_LDFLAGS		=
test_fifo_LDADD			= libstreams.la
test_fifo_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-fifo"'
test_fifo_CFLAGS		= $(USER_CFLAGS)

test_log_SOURCES		= src/test/test-log.c
test_log_LDFLAGS		=
test_log_LDADD			= libstreams.la
test_log_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-log"'
test_log_CFLAGS			= $(USER_CFLAGS)

test_loop_SOURCES		= src/test/test-loop.c
test_loop_LDFLAGS		=
test_loop_LDADD			= libstreams.la
test_loop_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-loop"'
test_loop_CFLAGS		= $(USER_CFLAGS)

test_nsdev_SOURCES		= src/test/test-nsdev.c
test_nsdev_LDFLAGS		=
test_nsdev_LDADD		= libstreams.la
test_nsdev_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-nsdev"'
test_nsdev_CFLAGS		= $(USER_CFLAGS)

test_mux_SOURCES		= src/test/test-mux.c
test_mux_LDFLAGS		=
test_mux_LDADD			= libstreams.la
test_mux_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-mux"'
test_mux_CFLAGS			= $(USER_CFLAGS)

test_nuls_SOURCES		= src/test/test-nuls.c
test_nuls_LDFLAGS		=
test_nuls_LDADD			= libstreams.la
test_nuls_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-nuls"'
test_nuls_CFLAGS		= $(USER_CFLAGS)

test_pipe_SOURCES		= src/test/test-pipe.c
test_pipe_LDFLAGS		=
test_pipe_LDADD			= libstreams.la
test_pipe_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-pipe"'
test_pipe_CFLAGS		= $(USER_CFLAGS)

perftest_SOURCES		= src/test/perftest.c
perftest_LDFLAGS		=
perftest_LDADD			= libstreams.la
perftest_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"perftest"'
perftest_CFLAGS			= $(USER_CFLAGS)

perftestn_SOURCES		= src/test/perftest.c
perftestn_LDFLAGS		= 
perftestn_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNATIVE_PIPES -DNAME='"perftestn"'
perftestn_CFLAGS		= $(USER_CFLAGS)

test_pipemod_SOURCES		= src/test/test-pipemod.c
test_pipemod_LDFLAGS		=
test_pipemod_LDADD		= libstreams.la
test_pipemod_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-pipemode"'
test_pipemod_CFLAGS		= $(USER_CFLAGS)

test_sad_SOURCES		= src/test/test-sad.c
test_sad_LDFLAGS		=
test_sad_LDADD			= libstreams.la
test_sad_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sad"'
test_sad_CFLAGS			= $(USER_CFLAGS)

test_sc_SOURCES			= src/test/test-sc.c
test_sc_LDFLAGS			=
test_sc_LDADD			= libstreams.la
test_sc_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sc"'
test_sc_CFLAGS			= $(USER_CFLAGS)

test_streams_SOURCES		= src/test/test-streams.c
test_streams_LDFLAGS		=
test_streams_LDADD		= libstreams.la
test_streams_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-streams"'
test_streams_CFLAGS		= $(USER_CFLAGS)

testlog_SOURCES			= src/test/testlog.c
testlog_LDFLAGS			=
testlog_LDADD			= libstreams.la
testlog_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"testlog"'
testlog_CFLAGS			= $(USER_CFLAGS)

test_tty_SOURCES		= src/test/test-tty.c
test_tty_LDFLAGS		=
test_tty_LDADD			= libstreams.la
test_tty_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-tty"'
test_tty_CFLAGS			= $(USER_CFLAGS)
##test_tty_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)

ldlconfig_SOURCES		= src/util/ldlconfig.c
ldlconfig_LDFLAGS		=
ldlconfig_LDADD			= libstreams.la
ldlconfig_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"ldlconfig"'
ldlconfig_CFLAGS		= $(USER_CFLAGS)

ldltest_SOURCES			= src/test/ldltest.c
ldltest_LDFLAGS			=
ldltest_LDADD			= libstreams.la
ldltest_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"ldltest"'
ldltest_CFLAGS			= $(USER_CFLAGS)

test_np_ip_SOURCES		= src/test/test-np_ip.c
test_np_ip_LDFLAGS		=
test_np_ip_LDADD		= libstreams.la
test_np_ip_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-np_ip"'
test_np_ip_CFLAGS		= $(USER_CFLAGS)

test_ip_SOURCES			= src/test/test-ip.c
test_ip_LDFLAGS			=
test_ip_LDADD			= libstreams.la
test_ip_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-ip"'
test_ip_CFLAGS			= $(USER_CFLAGS)

test_tirdwr_SOURCES		= src/test/test-tirdwr.c
test_tirdwr_LDFLAGS		=
test_tirdwr_LDADD		= libstreams.la libxnet.la
test_tirdwr_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-tirdwr"'
test_tirdwr_CFLAGS		= $(USER_CFLAGS)
test_tirdwr_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_timod_SOURCES		= src/test/test-timod.c
test_timod_LDFLAGS		=
test_timod_LDADD		= libstreams.la libxnet.la
test_timod_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-timod"'
test_timod_CFLAGS		= $(USER_CFLAGS)
test_timod_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_xnet_SOURCES		= src/test/test-xnet.c
test_xnet_LDFLAGS		=
test_xnet_LDADD			= libstreams.la libxnet.la
test_xnet_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-xnet"'
test_xnet_CFLAGS		= $(USER_CFLAGS)
test_xnet_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_xnet_thr_SOURCES		= src/test/test-xnet.c
test_xnet_thr_LDADD		= libstreams.la libxnet.la
test_xnet_thr_LDFLAGS		= -lpthread
test_xnet_thr_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-xnet_thr"' -D_REENTRANT
test_xnet_thr_CFLAGS		= $(USER_CFLAGS)
test_xnet_thr_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_xnsl_SOURCES		= src/test/test-xnsl.c
test_xnsl_LDADD			= libstreams.la libxnet.la libxnsl.la
test_xnsl_LDFLAGS		= 
test_xnsl_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-xnsl"'
test_xnsl_CFLAGS		= $(USER_CFLAGS)
test_xnsl_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_xnsl_thr_SOURCES		= src/test/test-xnsl.c
test_xnsl_thr_LDADD		= libstreams.la libxnet.la libxnsl.la
test_xnsl_thr_LDFLAGS		= -lpthread
test_xnsl_thr_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-xnsl_thr"' -D_REENTRANT
test_xnsl_thr_CFLAGS		= $(USER_CFLAGS)
test_xnsl_thr_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_sockpath_SOURCES		= src/test/test-sockpath.c
test_sockpath_LDADD		= libstreams.la libsockpath.la
test_sockpath_LDFLAGS		=
test_sockpath_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sockpath"'
test_sockpath_CFLAGS		= $(USER_CFLAGS)
test_sockpath_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_socket_SOURCES		= src/test/test-socket.c
test_socket_LDADD		= libstreams.la libsocket.la
test_socket_LDFLAGS		=
test_socket_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-socket"'
test_socket_CFLAGS		= $(USER_CFLAGS)
test_socket_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_socklib_SOURCES		= src/test/test-socklib.c
test_socklib_LDADD		= libstreams.la libsocklib.la
test_socklib_LDFLAGS		=
test_socklib_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-socklib"'
test_socklib_CFLAGS		= $(USER_CFLAGS)
test_socklib_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_sockmod_SOURCES		= src/test/test-sockmod.c
test_sockmod_LDADD		= libstreams.la libsocklib.la
test_sockmod_LDFLAGS		=
test_sockmod_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sockmod"'
test_sockmod_CFLAGS		= $(USER_CFLAGS)
test_sockmod_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_socksys_SOURCES		= src/test/test-socksys.c
test_socksys_LDADD		= libstreams.la libsocket.la
test_socksys_LDFLAGS		=
test_socksys_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-socksys"'
test_socksys_CFLAGS		= $(USER_CFLAGS)
test_socksys_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

initsock_SOURCES		= src/util/initsock.c
initsock_LDADD			= libstreams.la
initsock_LDFLAGS		=
initsock_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"initsock"'
initsock_CFLAGS			= $(USER_CFLAGS)
initsock_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)

soconfig_SOURCES		= src/util/soconfig.c
soconfig_LDADD			= libstreams.la
soconfig_LDFLAGS		=
soconfig_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"soconfig"'
soconfig_CFLAGS			= $(USER_CFLAGS)
soconfig_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_inet_raw_SOURCES		= src/test/test-inet_raw.c
test_inet_raw_LDADD		= libstreams.la
test_inet_raw_LDFLAGS		=
test_inet_raw_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-inet_raw"'
test_inet_raw_CFLAGS		= $(USER_CFLAGS)
test_inet_raw_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_inet_udp_SOURCES		= src/test/test-inet_udp.c
test_inet_udp_LDADD		= libstreams.la
test_inet_udp_LDFLAGS		=
test_inet_udp_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-inet_udp"'
test_inet_udp_CFLAGS		= $(USER_CFLAGS)
test_inet_udp_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_inet_tcp_SOURCES		= src/test/test-inet_tcp.c
test_inet_tcp_LDADD		= libstreams.la
test_inet_tcp_LDFLAGS		=
test_inet_tcp_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-inet_tcp"'
test_inet_tcp_CFLAGS		= $(USER_CFLAGS)
test_inet_tcp_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_inet_sctp_SOURCES		= src/test/test-inet_sctp.c
test_inet_sctp_LDADD		= libstreams.la
test_inet_sctp_LDFLAGS		=
test_inet_sctp_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-inet_sctp"'
test_inet_sctp_CFLAGS		= $(USER_CFLAGS)
test_inet_sctp_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_sctp_SOURCES		= src/test/test-sctp.c
test_sctp_LDADD			= libstreams.la
test_sctp_LDFLAGS		=
test_sctp_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sctp"'
test_sctp_CFLAGS		= $(USER_CFLAGS)
test_sctp_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_sctp_n_SOURCES		= src/test/test-sctp_n.c
test_sctp_n_LDADD		= libstreams.la
test_sctp_n_LDFLAGS		=
test_sctp_n_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sctp_n"'
test_sctp_n_CFLAGS		= $(USER_CFLAGS)
test_sctp_n_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_sctp_n2_SOURCES		= src/test/test-sctp_n2.c
test_sctp_n2_LDADD		= libstreams.la
test_sctp_n2_LDFLAGS		=
test_sctp_n2_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sctp_n2"'
test_sctp_n2_CFLAGS		= $(USER_CFLAGS)
test_sctp_n2_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_sctp_nc_SOURCES		= src/test/test-sctp_nc.c
test_sctp_nc_LDADD		= libstreams.la
test_sctp_nc_LDFLAGS		=
test_sctp_nc_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sctp_nc"'
test_sctp_nc_CFLAGS		= $(USER_CFLAGS)
test_sctp_nc_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_sctp_ns_SOURCES		= src/test/test-sctp_ns.c
test_sctp_ns_LDADD		= libstreams.la
test_sctp_ns_LDFLAGS		=
test_sctp_ns_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sctp_ns"'
test_sctp_ns_CFLAGS		= $(USER_CFLAGS)
test_sctp_ns_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_sctp_t_SOURCES		= src/test/test-sctp_t.c
test_sctp_t_LDADD		= libstreams.la
test_sctp_t_LDFLAGS		=
test_sctp_t_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-sctp_t"'
test_sctp_t_CFLAGS		= $(USER_CFLAGS)
test_sctp_t_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_etsi_n_SOURCES		= src/test/test-etsi_n.c
test_etsi_n_LDADD		= libstreams.la
test_etsi_n_LDFLAGS		=
test_etsi_n_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-etsi_n"'
test_etsi_n_CFLAGS		= $(USER_CFLAGS)
test_etsi_n_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_etsi_t_SOURCES		= src/test/test-etsi_t.c
test_etsi_t_LDADD		= libstreams.la
test_etsi_t_LDFLAGS		=
test_etsi_t_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-etsi_t"'
test_etsi_t_CFLAGS		= $(USER_CFLAGS)
test_etsi_t_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_interop_SOURCES		= src/test/test-interop.c
test_interop_LDADD		= libstreams.la
test_interop_LDFLAGS		=
test_interop_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-interop"'
test_interop_CFLAGS		= $(USER_CFLAGS)
test_interop_DEPENDENCIES	= $(CONFIG_HEADER) $(STRCONF_CONFIG)

test_chan_SOURCES		= src/test/test-chan.c
test_chan_LDADD			= libstreams.la
test_chan_LDFLAGS		=
test_chan_CPPFLAGS		= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"test-chan"'
test_chan_CFLAGS		= $(USER_CFLAGS)
test_chan_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)

chand_SOURCES			= src/snmp/chand.c \
				  src/snmp/chand.h \
				  src/snmp/ds0Bundle.c \
				  src/snmp/ds0Bundle.h \
				  src/snmp/ds0.c \
				  src/snmp/ds0.h \
				  src/snmp/ds1EXT.c \
				  src/snmp/ds1EXT.h \
				  src/snmp/mxMIB.c \
				  src/snmp/mxMIB.h \
				  src/snmp/phyMIB.c \
				  src/snmp/phyMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h
chand_LDADD			= libstreams.la $(SNMP_LDADD)
chand_LDFLAGS			= $(SNMP_LDFLAGS)
chand_CPPFLAGS			= $(AM_CPPFLAGS) $(PKG_INCLUDES) $(USER_CPPFLAGS) -DNAME='"chand"'
chand_CFLAGS			= $(USER_CFLAGS)
chand_DEPENDENCIES		= $(CONFIG_HEADER) $(STRCONF_CONFIG)

## PKG_BUILD_ARCH
endif
## RPM_BUILD_USER
endif

EXPORT_SRCFILES			=

if CONFIG_STREAMS_SEPARATE_COMPILE
## CONFIG_STREAMS_SEPARATE_COMPILE

EXPORT_SRCFILES			+= \
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

## CONFIG_STREAMS_SEPARATE_COMPILE
else
## !CONFIG_STREAMS_SEPARATE_COMPILE

EXPORT_SRCFILES			+= \
				  src/kernel/specfs.c \
				  src/kernel/streams.c

## !CONFIG_STREAMS_SEPARATE_COMPILE
endif

if CONFIG_STREAMS_CLONE_MODULE
EXPORT_SRCFILES			+= src/drivers/clone.c
endif
if CONFIG_STREAMS_STH_MODULE
EXPORT_SRCFILES			+= src/modules/sth.c
endif

if CONFIG_STREAMS_COMPAT_AIX
EXPORT_SRCFILES			+= src/kernel/aixcompat.c
endif
if CONFIG_STREAMS_COMPAT_HPUX
EXPORT_SRCFILES			+= src/kernel/hpuxcompat.c
endif
if CONFIG_STREAMS_COMPAT_IRIX
EXPORT_SRCFILES			+= src/kernel/irixcompat.c
endif
if CONFIG_STREAMS_COMPAT_LIS
EXPORT_SRCFILES			+= src/kernel/liscompat.c
endif
if CONFIG_STREAMS_COMPAT_MAC
EXPORT_SRCFILES			+= src/kernel/maccompat.c
endif
if CONFIG_STREAMS_COMPAT_MPS
EXPORT_SRCFILES			+= src/kernel/mpscompat.c
endif
if CONFIG_STREAMS_COMPAT_OS7
EXPORT_SRCFILES			+= src/kernel/os7compat.c
endif
if CONFIG_STREAMS_COMPAT_OSF
EXPORT_SRCFILES			+= src/kernel/osfcompat.c
endif
if CONFIG_STREAMS_COMPAT_SUN
EXPORT_SRCFILES			+= src/kernel/suncompat.c
endif
if CONFIG_STREAMS_COMPAT_SVR3
EXPORT_SRCFILES			+= src/kernel/svr3compat.c
endif
if CONFIG_STREAMS_COMPAT_SVR4
EXPORT_SRCFILES			+= src/kernel/svr4compat.c
endif
if CONFIG_STREAMS_COMPAT_UW7
EXPORT_SRCFILES			+= src/kernel/uw7compat.c
endif
if CONFIG_STREAMS_COMPAT_AIX_MODULE
EXPORT_SRCFILES			+= src/kernel/aixcompat.c
endif
if CONFIG_STREAMS_COMPAT_HPUX_MODULE
EXPORT_SRCFILES			+= src/kernel/hpuxcompat.c
endif
if CONFIG_STREAMS_COMPAT_IRIX_MODULE
EXPORT_SRCFILES			+= src/kernel/irixcompat.c
endif
if CONFIG_STREAMS_COMPAT_LIS_MODULE
EXPORT_SRCFILES			+= src/kernel/liscompat.c
endif
if CONFIG_STREAMS_COMPAT_MAC_MODULE
EXPORT_SRCFILES			+= src/kernel/maccompat.c
endif
if CONFIG_STREAMS_COMPAT_MPS_MODULE
EXPORT_SRCFILES			+= src/kernel/mpscompat.c
endif
if CONFIG_STREAMS_COMPAT_OS7_MODULE
EXPORT_SRCFILES			+= src/kernel/os7compat.c
endif
if CONFIG_STREAMS_COMPAT_OSF_MODULE
EXPORT_SRCFILES			+= src/kernel/osfcompat.c
endif
if CONFIG_STREAMS_COMPAT_SUN_MODULE
EXPORT_SRCFILES			+= src/kernel/suncompat.c
endif
if CONFIG_STREAMS_COMPAT_SVR3_MODULE
EXPORT_SRCFILES			+= src/kernel/svr3compat.c
endif
if CONFIG_STREAMS_COMPAT_SVR4_MODULE
EXPORT_SRCFILES			+= src/kernel/svr4compat.c
endif
if CONFIG_STREAMS_COMPAT_UW7_MODULE
EXPORT_SRCFILES			+= src/kernel/uw7compat.c
endif

EXPORT_VERFILES			=

if CONFIG_STREAMS_SEPARATE_COMPILE
## CONFIG_STREAMS_SEPARATE_COMPILE

EXPORT_VERFILES			+= \
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

## CONFIG_STREAMS_SEPARATE_COMPILE
else
## CONFIG_STREAMS_SEPARATE_COMPILE

EXPORT_VERFILES			+= \
				  $(PKGINCL)/versions/specfs.ver \
				  $(PKGINCL)/versions/streams.ver

## CONFIG_STREAMS_SEPARATE_COMPILE
endif

if CONFIG_STREAMS_CLONE_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/clone.ver
endif
if CONFIG_STREAMS_STH_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/sth.ver
endif

if CONFIG_STREAMS_COMPAT_AIX
EXPORT_VERFILES			+= $(PKGINCL)/versions/aixcompat.ver
endif
if CONFIG_STREAMS_COMPAT_HPUX
EXPORT_VERFILES			+= $(PKGINCL)/versions/hpuxcompat.ver
endif
if CONFIG_STREAMS_COMPAT_IRIX
EXPORT_VERFILES			+= $(PKGINCL)/versions/irixcompat.ver
endif
if CONFIG_STREAMS_COMPAT_LIS
EXPORT_VERFILES			+= $(PKGINCL)/versions/liscompat.ver
endif
if CONFIG_STREAMS_COMPAT_MAC
EXPORT_VERFILES			+= $(PKGINCL)/versions/maccompat.ver
endif
if CONFIG_STREAMS_COMPAT_MPS
EXPORT_VERFILES			+= $(PKGINCL)/versions/mpscompat.ver
endif
if CONFIG_STREAMS_COMPAT_OS7
EXPORT_VERFILES			+= $(PKGINCL)/versions/os7compat.ver
endif
if CONFIG_STREAMS_COMPAT_OSF
EXPORT_VERFILES			+= $(PKGINCL)/versions/osfcompat.ver
endif
if CONFIG_STREAMS_COMPAT_SUN
EXPORT_VERFILES			+= $(PKGINCL)/versions/suncompat.ver
endif
if CONFIG_STREAMS_COMPAT_SVR3
EXPORT_VERFILES			+= $(PKGINCL)/versions/svr3compat.ver
endif
if CONFIG_STREAMS_COMPAT_SVR4
EXPORT_VERFILES			+= $(PKGINCL)/versions/svr4compat.ver
endif
if CONFIG_STREAMS_COMPAT_UW7
EXPORT_VERFILES			+= $(PKGINCL)/versions/uw7compat.ver
endif
if CONFIG_STREAMS_COMPAT_AIX_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/aixcompat.ver
endif
if CONFIG_STREAMS_COMPAT_HPUX_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/hpuxcompat.ver
endif
if CONFIG_STREAMS_COMPAT_IRIX_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/irixcompat.ver
endif
if CONFIG_STREAMS_COMPAT_LIS_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/liscompat.ver
endif
if CONFIG_STREAMS_COMPAT_MAC_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/maccompat.ver
endif
if CONFIG_STREAMS_COMPAT_MPS_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/mpscompat.ver
endif
if CONFIG_STREAMS_COMPAT_OS7_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/os7compat.ver
endif
if CONFIG_STREAMS_COMPAT_OSF_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/osfcompat.ver
endif
if CONFIG_STREAMS_COMPAT_SUN_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/suncompat.ver
endif
if CONFIG_STREAMS_COMPAT_SVR3_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/svr3compat.ver
endif
if CONFIG_STREAMS_COMPAT_SVR4_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/svr4compat.ver
endif
if CONFIG_STREAMS_COMPAT_UW7_MODULE
EXPORT_VERFILES			+= $(PKGINCL)/versions/uw7compat.ver
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

INET_MKDEV			= scripts/inet_mkdev
INET_RMDEV			= scripts/inet_rmdev

if PKG_BUILD_ARCH
if RPM_BUILD_USER
## these go with the utilities package
dist_sbin_SCRIPTS 		+= $(INET_MKDEV) $(INET_RMDEV)
endif
endif

AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += $(INET_MKDEV) $(INET_RMDEV)
EXTRA_SCRIPTS += $(INET_MKDEV) $(INET_RMDEV)

if RPM_BUILD_KERNEL
## RPM_BUILD_KERNEL

install-inet-devices: $(INET_MKDEV)
	@if test -r $(DESTDIR)$(rootdir)/etc/modules.conf ; then \
		echo 'DESTDIR=$(DESTDIR) $(srcdir)/$(INET_MKDEV)' ; \
		DESTDIR=$(DESTDIR) $(srcdir)/$(INET_MKDEV) ; \
	fi

uninstall-inet-devices: $(INET_RMDEV)
	@if test -r $(DESTDIR)$(rootdir)/etc/modules.conf ; then \
		echo 'DESTDIR=$(DESTDIR) $(srcdir)/$(INET_RMDEV)' ; \
		DESTDIR=$(DESTDIR) $(srcdir)/$(INET_RMDEV) ; \
	fi

## RPM_BUILD_KERNEL
else
## ! RPM_BUILD_KERNEL

install-inet-devices:

uninstall-inet-devices:

## ! RPM_BUILD_KERNEL
endif

INSTALL_EXEC_HOOK += install-inet-devices

UNINSTALL_HOOK += uninstall-inet-devices

.PHONY: install-inet-devices uninstall-inet-devices

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
				  src/util/init/streams.sh \
				  src/util/init/@PACKAGE_LCNAME@.sh
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
				  src/util/init/streams \
				  src/util/init/@PACKAGE_LCNAME@
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
				  src/util/sysconfig/streams.conf \
				  src/util/sysconfig/@PACKAGE_LCNAME@.conf \
				  src/util/sysconfig/sock2path
if CONFIG_STREAMS_UTILS
dist_sysconf_DATA		+= \
				  strsetup.conf \
				  strload.conf
endif

dist_config_DATA		= \
				  src/util/sysconfig/specfs \
				  src/util/sysconfig/streams \
				  src/util/sysconfig/@PACKAGE_LCNAME@
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
## Rules for net config files
##
include am/netconfig.am
include am/sock2path.am

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
