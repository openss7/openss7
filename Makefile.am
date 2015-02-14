# =============================================================================
# 
# @(#) File: Makefile.am  Version: @VERSION@  Date: @PACKAGE_DATE@
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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
# =============================================================================

ACLOCAL_AMFLAGS = -I m4

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

AUTOMAKE_OPTIONS = nostdinc
AM_CPPFLAGS = -I. -I$(top_builddir) -I$(srcdir)

dist_bin_SCRIPTS		=

dist_sbin_SCRIPTS		=

dist_sysconf_DATA		=

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

if PKG_BUILD_USER
## PKG_BUILD_USER

##
## These are for ptys and should be part of the strutil or strio package.
##
syssbin_PROGRAMS		=

sbin_PROGRAMS			=

sysbin_PROGRAMS			=

bin_PROGRAMS			=

pkglibexec_PROGRAMS		=

#LFS_EXTRA_TESTS		= 
#LFS_EXTRA_TOOLS		= @PACKAGE_TARNAME@_mknod

#LDADD				= libstreams.la

lib_LTLIBRARIES			=

lib32_LTLIBRARIES		=

syslib_LTLIBRARIES		=

syslib32_LTLIBRARIES		=

snmpdlmod_LTLIBRARIES		=

install_libs			= install-libLTLIBRARIES \
				  install-lib32LTLIBRARIES

$(install_libs): install-%LTLIBRARIES: install-sys%LTLIBRARIES

## PKG_BUILD_USER
endif

if PKG_BUILD_KERNEL
## PKG_BUILD_KERNEL

noinst_LIBRARIES		=

KERNEL_OBJECTS			=
KERNEL_MODULES			=

AM_V_KCC = $(am__v_KCC_@AM_V@)
am__v_KCC_ = $(am__v_KCC_@AM_DEFAULT_V@)
am__v_KCC_0 = @echo "  KCC   " $@;

## =====================================================================
if CONFIG_STREAMS_WEAK_MODULES
else
## !CONFIG_STREAMS_WEAK_MODULES
if CONFIG_STREAMS_WRAPPER_MODULE
## CONFIG_STREAMS_WRAPPER_MODULE

noinst_LIBRARIES		+= libOS7wrapper.a

EXTRA_libOS7wrapper_a_SOURCES	= src/kernel/wrapper.c
libOS7wrapper_a_SOURCES		= src/kernel/wrapper.c
libOS7wrapper_a_CC		= $(KCC)
libOS7wrapper_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libOS7wrapper_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOS7wrapper_a_OBJECTS): 	AM_V_CC = $(AM_V_KCC)
$(libOS7wrapper_a_OBJECTS): 	CC = $(KCC)
$(libOS7wrapper_a_OBJECTS): 	$(STRCONF_MODCONF) $(MODVERSIONS_H) $(STRCONF_CONFIG)

KERNEL_OBJECTS			+= $(libOS7wrapper_a_OBJECTS)
KERNEL_MODULES			+= wrapper.o

wrapper.o: stamp-verobjs
	@list=`$(ECHO) " $(libOS7wrapper_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO_Q) "  CCLD  " $@ ; \
	$(ECHO_V) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list

## CONFIG_STREAMS_WRAPPER_MODULE
endif
## !CONFIG_STREAMS_WEAK_MODULES
endif
## =====================================================================

noinst_LIBRARIES		+= libOS7specfs.a

EXTRA_libOS7specfs_a_SOURCES	= src/kernel/strspecfs.c \
				  src/kernel/strlookup.c \
				  src/kernel/strattach.c \
				  src/kernel/strpipe.c \
				  src/kernel/wrapper.c \
				  src/kernel/strlock.c
libOS7specfs_a_SOURCES		= src/kernel/specfs.c \
				  src/kernel/strspecfs.h \
				  src/kernel/strlookup.h \
				  src/kernel/strattach.h \
				  src/kernel/strpipe.h \
				  src/kernel/strargs.h \
				  src/kernel/strlock.h
libOS7specfs_a_CC		= $(KCC)
libOS7specfs_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libOS7specfs_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOS7specfs_a_OBJECTS): 	AM_V_CC = $(AM_V_KCC)
$(libOS7specfs_a_OBJECTS): 	CC = $(KCC)
$(libOS7specfs_a_OBJECTS): 	$(STRCONF_MODCONF) $(MODVERSIONS_H) $(STRCONF_CONFIG)

KERNEL_OBJECTS			+= $(libOS7specfs_a_OBJECTS)
KERNEL_MODULES			+= specfs.o

specfs.o: stamp-verobjs
	@list=`$(ECHO) " $(libOS7specfs_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO_Q) "  CCLD  " $@ ; \
	$(ECHO_V) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list ; \
	$(ECHO_Q) " STRIP  " $@ ; \
	$(ECHO_V) "$(STRIP) -N strlock_region -N strlock_notifier -N strlock_digest -N strlock_expires -N strlock_netdev_notifier -N strlock_options -N strlock_reverify -N strlock_tfm -N strlock_unlocked -N strlock_verify $@" ; \
	$(STRIP) -N strlock_region -N strlock_notifier -N strlock_digest -N strlock_expires -N strlock_netdev_notifier -N strlock_options -N strlock_reverify -N strlock_tfm -N strlock_unlocked -N strlock_verify $@


## =====================================================================

noinst_LIBRARIES		+= libOS7streams.a

EXTRA_libOS7streams_a_SOURCES	= src/kernel/strmain.c \
				  src/kernel/strreg.c \
				  src/kernel/strprocfs.c \
				  src/kernel/strsched.c \
				  src/kernel/strsysctl.c \
				  src/kernel/strsyms.c \
				  src/kernel/strutil.c \
				  src/kernel/strsad.c
libOS7streams_a_SOURCES		= src/kernel/streams.c \
				  src/kernel/strreg.h \
				  src/kernel/strprocfs.h \
				  src/kernel/strsched.h \
				  src/kernel/strsysctl.h \
				  src/kernel/strutil.h \
				  src/kernel/strsad.h \
				  src/kernel/strargs.h \
				  src/kernel/strmain.h
libOS7streams_a_CC		= $(KCC)
libOS7streams_a_CPPFLAGS	= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libOS7streams_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOS7streams_a_OBJECTS): 	AM_V_CC = $(AM_V_KCC)
$(libOS7streams_a_OBJECTS): 	CC = $(KCC)
$(libOS7streams_a_OBJECTS): 	$(STRCONF_MODCONF) $(MODVERSIONS_H) $(STRCONF_CONFIG)

KERNEL_OBJECTS			+= $(libOS7streams_a_OBJECTS)
KERNEL_MODULES			+= streams.o

streams.o: stamp-verobjs
	@list=`$(ECHO) " $(libOS7streams_a_OBJECTS) " | sed -e 's| lib[^[:space:]]*_a-| $(kpre)|g'` ; \
	$(ECHO_Q) "  CCLD  " $@ ; \
	$(ECHO_V) "$(LD) -r -o $@ $$list" ; \
	$(LD) -r -o $@ $$list

## =====================================================================

noinst_LIBRARIES		+= libOS7kernel.a

libOS7kernel_a_SOURCES	        = src/include/sys/os7/allocb.h \
				  src/include/sys/os7/bufpool.h \
				  src/include/sys/os7/bufq.h \
				  src/include/sys/os7/compat.h \
				  src/include/sys/os7/debug.h \
				  src/include/sys/os7/lock.h \
				  src/include/sys/os7/priv.h \
				  src/include/sys/os7/queue.h \
				  src/include/sys/os7/timer.h
libOS7kernel_a_CC		= $(KCC)
libOS7kernel_a_CPPFLAGS		= $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_EXPSYMTAB)
libOS7kernel_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOS7kernel_a_OBJECTS): 	AM_V_CC = $(AM_V_KCC)
$(libOS7kernel_a_OBJECTS): 	CC = $(KCC)
$(libOS7kernel_a_OBJECTS): 	$(STRCONF_MODCONF) $(MODVERSIONS_H) $(STRCONF_CONFIG)

KERNEL_OBJECTS			+= $(libOS7kernel_a_OBJECTS)
KERNEL_MODULES			+= $(libOS7kernel_a_OBJECTS)

if CONFIG_STREAMS_COMPAT_OS7_MODULE
libOS7kernel_a_SOURCES	       += src/kernel/os7compat.c \
				  src/include/sys/os7/ddi.h \
				  src/include/sys/os7/strconf.h \
				  src/include/sys/os7/stream.h
endif
if CONFIG_STREAMS_COMPAT_OS7
libOS7streams_a_SOURCES	       += src/kernel/os7compat.c \
				  src/include/sys/os7/ddi.h \
				  src/include/sys/os7/strconf.h \
				  src/include/sys/os7/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR3_MODULE
libOS7kernel_a_SOURCES	       += src/kernel/svr3compat.c \
				  src/include/sys/svr3/ddi.h \
				  src/include/sys/svr3/strconf.h \
				  src/include/sys/svr3/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR3
libOS7streams_a_SOURCES	       += src/kernel/svr3compat.c \
				  src/include/sys/svr3/ddi.h \
				  src/include/sys/svr3/strconf.h \
				  src/include/sys/svr3/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR4_MODULE
libOS7kernel_a_SOURCES	       += src/kernel/svr4compat.c \
				  src/include/sys/svr4/ddi.h \
				  src/include/sys/svr4/strconf.h \
				  src/include/sys/svr4/stream.h
endif
if CONFIG_STREAMS_COMPAT_SVR4
libOS7streams_a_SOURCES	       += src/kernel/svr4compat.c \
				  src/include/sys/svr4/ddi.h \
				  src/include/sys/svr4/strconf.h \
				  src/include/sys/svr4/stream.h
endif
if CONFIG_STREAMS_COMPAT_MPS_MODULE
libOS7kernel_a_SOURCES	       += src/kernel/mpscompat.c \
				  src/include/sys/mps/ddi.h \
				  src/include/sys/mps/strconf.h \
				  src/include/sys/mps/stream.h
endif
if CONFIG_STREAMS_COMPAT_MPS
libOS7streams_a_SOURCES	       += src/kernel/mpscompat.c \
				  src/include/sys/mps/ddi.h \
				  src/include/sys/mps/strconf.h \
				  src/include/sys/mps/stream.h
endif
if CONFIG_STREAMS_COMPAT_SUN_MODULE
libOS7kernel_a_SOURCES	       += src/kernel/suncompat.c \
				  src/include/sys/sunddi.h \
				  src/include/sys/strsun.h \
				  src/include/sys/sun/ddi.h \
				  src/include/sys/sun/strconf.h \
				  src/include/sys/sun/strsun.h \
				  src/include/sys/sun/stream.h
endif
if CONFIG_STREAMS_COMPAT_SUN
libOS7streams_a_SOURCES	       += src/kernel/suncompat.c \
				  src/include/sys/sunddi.h \
				  src/include/sys/strsun.h \
				  src/include/sys/sun/ddi.h \
				  src/include/sys/sun/strconf.h \
				  src/include/sys/sun/strsun.h \
				  src/include/sys/sun/stream.h
endif
if CONFIG_STREAMS_COMPAT_UW7_MODULE
libOS7kernel_a_SOURCES	       += src/kernel/uw7compat.c \
				  src/include/sys/uw7/ddi.h \
				  src/include/sys/uw7/strconf.h \
				  src/include/sys/uw7/stream.h
endif
if CONFIG_STREAMS_COMPAT_UW7
libOS7streams_a_SOURCES	       += src/kernel/uw7compat.c \
				  src/include/sys/uw7/ddi.h \
				  src/include/sys/uw7/strconf.h \
				  src/include/sys/uw7/stream.h
endif
if CONFIG_STREAMS_COMPAT_OSF_MODULE
libOS7kernel_a_SOURCES	       += src/kernel/osfcompat.c \
				  src/include/sys/osf/ddi.h \
				  src/include/sys/osf/strconf.h \
				  src/include/sys/osf/stream.h
endif
if CONFIG_STREAMS_COMPAT_OSF
libOS7streams_a_SOURCES	       += src/kernel/osfcompat.c \
				  src/include/sys/osf/ddi.h \
				  src/include/sys/osf/strconf.h \
				  src/include/sys/osf/stream.h
endif
if CONFIG_STREAMS_COMPAT_AIX_MODULE
libOS7kernel_a_SOURCES	       += src/kernel/aixcompat.c \
				  src/include/sys/aix/ddi.h \
				  src/include/sys/aix/strconf.h \
				  src/include/sys/aix/stream.h
endif
if CONFIG_STREAMS_COMPAT_AIX
libOS7streams_a_SOURCES	       += src/kernel/aixcompat.c \
				  src/include/sys/aix/ddi.h \
				  src/include/sys/aix/strconf.h \
				  src/include/sys/aix/stream.h
endif
if CONFIG_STREAMS_COMPAT_HPUX_MODULE
libOS7kernel_a_SOURCES	       += src/kernel/hpuxcompat.c \
				  src/include/sys/hpux/ddi.h \
				  src/include/sys/hpux/strconf.h \
				  src/include/sys/hpux/stream.h
endif
if CONFIG_STREAMS_COMPAT_HPUX
libOS7streams_a_SOURCES	       += src/kernel/hpuxcompat.c \
				  src/include/sys/hpux/ddi.h \
				  src/include/sys/hpux/strconf.h \
				  src/include/sys/hpux/stream.h
endif
if CONFIG_STREAMS_COMPAT_IRIX_MODULE
libOS7kernel_a_SOURCES	       += src/kernel/irixcompat.c \
				  src/include/sys/irix/ddi.h \
				  src/include/sys/irix/strconf.h \
				  src/include/sys/irix/stream.h
endif
if CONFIG_STREAMS_COMPAT_IRIX
libOS7streams_a_SOURCES	       += src/kernel/irixcompat.c \
				  src/include/sys/irix/ddi.h \
				  src/include/sys/irix/strconf.h \
				  src/include/sys/irix/stream.h
endif
if CONFIG_STREAMS_COMPAT_MAC_MODULE
libOS7kernel_a_SOURCES	       += src/kernel/maccompat.c \
				  src/include/sys/mac/ddi.h \
				  src/include/sys/mac/strconf.h \
				  src/include/sys/mac/stream.h
endif
if CONFIG_STREAMS_COMPAT_MAC
libOS7streams_a_SOURCES	       += src/kernel/maccompat.c \
				  src/include/sys/mac/ddi.h \
				  src/include/sys/mac/strconf.h \
				  src/include/sys/mac/stream.h
endif

## =====================================================================

noinst_LIBRARIES		+= libOS7modules.a

# these are modules not yet strapped into this build (yes, there are lots of them)
EXTRA_libOS7modules_a_SOURCES	= src/modules/apmod.c \
				  src/modules/cd.c src/modules/cd.h \
				  src/modules/cd_daed.c \
				  src/modules/cd_hdlc.c \
				  src/modules/cd_old.c \
				  src/modules/cdi_mod.c \
				  src/modules/ch.c \
				  src/modules/ch_x400p.c \
				  src/modules/chmod.c \
				  src/modules/dcc.h \
				  src/modules/dl_sl.c \
				  src/modules/hdlc.c \
				  src/modules/iax.c \
				  src/modules/ip_strm_mod.c src/modules/ip_strm_mod.h \
				  src/modules/isos.c \
				  src/modules/itos.c \
				  src/modules/lapb.c \
				  src/modules/lapd.c \
				  src/modules/lapf.c \
				  src/modules/m2pa_dl.c \
				  src/modules/m2tp.c \
				  src/modules/map_mod.c \
				  src/modules/mapmod.c \
				  src/modules/momod.c \
				  src/modules/mtp3b.c \
				  src/modules/mtp_mod.c \
				  src/modules/mx_sdl.c \
				  src/modules/mx_x400p.c \
				  src/modules/npi.c \
				  src/modules/perf.c \
				  src/modules/rtp_ch.c \
				  src/modules/rtp_test.c \
				  src/modules/s_cdi.c \
				  src/modules/s_lli.c \
				  src/modules/s_llic.c \
				  src/modules/s_nli.c \
				  src/modules/s_nlic.c \
				  src/modules/s_wan.c \
				  src/modules/s_wanc.c \
				  src/modules/sccp_npi.c \
				  src/modules/sccp_tpi.c \
				  src/modules/sccp_tpi_old.c \
				  src/modules/sctpstr.c \
				  src/modules/sdl_cd.c \
				  src/modules/sdt_cd.c \
				  src/modules/sl_dl.c \
				  src/modules/sscop-mod.c \
				  src/modules/sscop2.c \
				  src/modules/sua_as.c \
				  src/modules/tab.c src/modules/tab.h \
				  src/modules/tabold.c src/modules/tabold.h \
				  src/modules/tcmod.c \
				  src/modules/tdm_ip.c \
				  src/modules/test.c \
				  src/modules/testboth.c \
				  src/modules/testmod_mps.c \
				  src/modules/testold.c \
				  src/modules/tp0.c \
				  src/modules/trmod.c \
				  src/modules/tstmod.c src/modules/tstmod.h \
				  src/modules/tua_as.c \
				  src/modules/vtoip.c \
				  src/modules/xos.c \
				  src/modules/xx25.c \
				  src/modules/zaptel.c \
				  src/modules/zaptel-mod.c
libOS7modules_a_SOURCES		= \
				  src/modules/mstr.c \
				  src/modules/pckt.c \
				  src/modules/ptem.c \
				  src/modules/ttcompat.c \
				  src/modules/ldterm.c \
				  src/modules/debug.h \
				  src/modules/sockmod.c \
				  src/modules/tpiperf.c \
				  src/modules/ch_pmod.c \
				  src/modules/mx_pmod.c \
				  src/modules/cdpmod.c \
				  src/modules/dlaix.c \
				  src/modules/cmot.c \
				  src/modules/isot.c \
				  src/modules/itot.c \
				  src/modules/lpp.c \
				  src/modules/tcpns.c \
				  src/modules/xot.c \
				  src/modules/phys.c \
				  src/modules/aal1.c \
				  src/modules/aal2.c \
				  src/modules/aal5.c \
				  src/modules/sscop.c \
				  src/modules/sscopmce.c \
				  src/modules/mtp_npi.c \
				  src/modules/mtp_tpi.c \
				  src/modules/sccp_mod.c \
				  src/modules/npi_sccp.c \
				  src/modules/tpi_sccp.c \
				  src/modules/tr_mod.c \
				  src/modules/tc_mod.c \
				  src/modules/sdl.c \
				  src/modules/spm.c \
				  src/modules/sdl_pmod.c \
				  src/modules/sdl_ch.c \
				  src/modules/sdt.c \
				  src/modules/sdt_pmod.c \
				  src/modules/sl.c \
				  src/modules/sm_mod.c \
				  src/modules/slpmod.c \
				  src/modules/m2pa_sl.c \
				  src/modules/m2ua_as.c \
				  src/modules/m3ua_as.c \
				  src/modules/sdl_sctp.c \
				  src/modules/sdt_sctp.c \
				  src/modules/sdt_tpi.c \
				  src/modules/sl_tpi.c \
				  src/modules/cdi_conv.c \
				  src/modules/dlpi_conv.c \
				  src/modules/npi_conv.c \
				  src/modules/tpi_conv.c
libOS7modules_a_CC		= $(KCC)
libOS7modules_a_CPPFLAGS	= -I$(top_builddir)/src/modules -I$(top_srcdir)/src/modules \
				  $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_EXPSYMTAB)
libOS7modules_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOS7modules_a_OBJECTS):	AM_V_CC = $(AM_V_KCC)
$(libOS7modules_a_OBJECTS):	CC = $(KCC)
$(libOS7modules_a_OBJECTS):	$(STRCONF_MODCONF) $(MODVERSIONS_H) $(STRCONF_CONFIG)

KERNEL_OBJECTS			+= $(libOS7modules_a_OBJECTS)
KERNEL_MODULES			+= $(libOS7modules_a_OBJECTS)

if CONFIG_STREAMS_STH_MODULE
libOS7modules_a_SOURCES	       += src/modules/sth.c src/modules/sth.h
endif
if CONFIG_STREAMS_STH
libOS7streams_a_SOURCES	       += src/modules/sth.c src/modules/sth.h
endif
if CONFIG_STREAMS_SRVMOD_MODULE
libOS7modules_a_SOURCES	       += src/modules/srvmod.c
endif
if CONFIG_STREAMS_SRVMOD
libOS7streams_a_SOURCES	       += src/modules/srvmod.c
endif
if CONFIG_STREAMS_NULLMOD_MODULE
libOS7modules_a_SOURCES	       += src/modules/nullmod.c
endif
if CONFIG_STREAMS_NULLMOD
libOS7streams_a_SOURCES	       += src/modules/nullmod.c
endif
if CONFIG_STREAMS_PIPEMOD_MODULE
libOS7modules_a_SOURCES	       += src/modules/pipemod.c
endif
if CONFIG_STREAMS_PIPEMOD
libOS7streams_a_SOURCES	       += src/modules/pipemod.c
endif
if CONFIG_STREAMS_CONNLD_MODULE
libOS7modules_a_SOURCES	       += src/modules/connld.c
endif
if CONFIG_STREAMS_CONNLD
libOS7streams_a_SOURCES	       += src/modules/connld.c
endif
if CONFIG_STREAMS_SC_MODULE
libOS7modules_a_SOURCES	       += src/modules/sc.c src/include/sys/sc.h
endif
if CONFIG_STREAMS_SC
libOS7streams_a_SOURCES	       += src/modules/sc.c src/include/sys/sc.h
endif
if CONFIG_STREAMS_TESTMOD_MODULE
libOS7modules_a_SOURCES	       += src/modules/testmod.c src/include/sys/testmod.h
endif
if CONFIG_STREAMS_TESTMOD
libOS7streams_a_SOURCES	       += src/modules/testmod.c src/include/sys/testmod.h
endif

if CONFIG_STREAMS_TIMOD_MODULE
libOS7modules_a_SOURCES	       += src/modules/timod.c
endif
if CONFIG_STREAMS_TIMOD
libOS7streams_a_SOURCES	       += src/modules/timod.c
endif
if CONFIG_STREAMS_TIRDWR_MODULE
libOS7modules_a_SOURCES	       += src/modules/tirdwr.c
endif
if CONFIG_STREAMS_TIRDWR
libOS7streams_a_SOURCES	       += src/modules/tirdwr.c
endif
if CONFIG_STREAMS_BUFMOD_MODULE
libOS7modules_a_SOURCES		+= src/modules/bufmod.c src/include/sys/bufmod.h
endif
if CONFIG_STREAMS_BUFMOD
libOS7streams_a_SOURCES		+= src/modules/bufmod.c src/include/sys/bufmod.h
endif
if CONFIG_STREAMS_PFMOD_MODULE
libOS7modules_a_SOURCES		+= src/modules/pfmod.c src/include/sys/pfmod.h
endif
if CONFIG_STREAMS_PFMOD
libOS7streams_a_SOURCES		+= src/modules/pfmod.c src/include/sys/pfmod.h
endif
if CONFIG_STREAMS_NBUF_MODULE
libOS7modules_a_SOURCES		+= src/modules/nit_buf.c src/include/net/nit_buf.h
endif
if CONFIG_STREAMS_NBUF
libOS7streams_a_SOURCES		+= src/modules/nit_buf.c src/include/net/nit_buf.h
endif
if CONFIG_STREAMS_PF_MODULE
libOS7modules_a_SOURCES		+= src/modules/nit_pf.c src/include/net/nit_pf.h
endif
if CONFIG_STREAMS_PF
libOS7streams_a_SOURCES		+= src/modules/nit_pf.c src/include/net/nit_pf.h
endif

## =====================================================================

noinst_LIBRARIES		+= libOS7drivers.a

# these are drivers not yet strapped into this build (yes, there are lots of them)
EXTRA_libOS7drivers_a_SOURCES	= src/drivers/biquad.h \
				  src/drivers/bufq.h \
				  src/drivers/cd-llc.c \
				  src/drivers/cd.c src/drivers/cd.h \
				  src/drivers/cddev.c \
				  src/drivers/cdi.c \
				  src/drivers/ch.c \
				  src/drivers/chmux.c \
				  src/drivers/clns-ip.c \
				  src/drivers/clns-udp.c \
				  src/drivers/cons.c \
				  src/drivers/crc.c \
				  src/drivers/dl-llc.c \
				  src/drivers/dl.c src/drivers/dl.h \
				  src/drivers/dl2.c \
				  src/drivers/dl_dua.c \
				  src/drivers/dl_gr303.c \
				  src/drivers/dl_gr303ua.c \
				  src/drivers/dl_gsma.c \
				  src/drivers/dl_h225.c \
				  src/drivers/dl_ip.c \
				  src/drivers/dl_iua.c \
				  src/drivers/dl_lapb.c \
				  src/drivers/dl_lapd.c \
				  src/drivers/dl_lapf.c \
				  src/drivers/dl_master.c \
				  src/drivers/dl_v52.c \
				  src/drivers/dl_v5ua.c \
				  src/drivers/dld_drv.c \
				  src/drivers/dld_proto.c \
				  src/drivers/dld_str.c \
				  src/drivers/dlp.c \
				  src/drivers/dlpi.c \
				  src/drivers/ecdis.h \
				  src/drivers/fasthdlc.h \
				  src/drivers/h225.c \
				  src/drivers/hdlc.c \
				  src/drivers/ifr.c \
				  src/drivers/ip.c \
				  src/drivers/isdn.c \
				  src/drivers/ixe.c \
				  src/drivers/lcd.c \
				  src/drivers/ldl2.c \
				  src/drivers/llc.c \
				  src/drivers/llc1.c src/drivers/llc1.h \
				  src/drivers/llc2.c \
				  src/drivers/lm.c src/drivers/lm.h \
				  src/drivers/m2ua.c \
				  src/drivers/m3ua.c src/drivers/m3ua.h \
				  src/drivers/m3ua_asp.c src/drivers/m3ua_asp.h \
				  src/drivers/m3ua_data.h \
				  src/drivers/m3ua_lm.c src/drivers/m3ua_lm.h \
				  src/drivers/m3ua_lower.c \
				  src/drivers/m3ua_msg.c src/drivers/m3ua_msg.h \
				  src/drivers/m3ua_mtp.c src/drivers/m3ua_mtp.h \
				  src/drivers/m3ua_mtpp.c \
				  src/drivers/m3ua_mtpu.c \
				  src/drivers/m3ua_nsp.c src/drivers/m3ua_nsp.h \
				  src/drivers/m3ua_sg.c \
				  src/drivers/m3ua_sgp.c \
				  src/drivers/m3ua_spp.c \
				  src/drivers/m3ua_ss7.c \
				  src/drivers/m3ua_tsp.c src/drivers/m3ua_tsp.h \
				  src/drivers/map.c \
				  src/drivers/matrix.c \
				  src/drivers/mcmux.c \
				  src/drivers/mfe.c \
				  src/drivers/mg.c \
				  src/drivers/mlp.c \
				  src/drivers/mtp_mux.c \
				  src/drivers/mtp_sm.c \
				  src/drivers/mx.c \
				  src/drivers/mx_mux.c \
				  src/drivers/mxmux.c \
				  src/drivers/nf.c \
				  src/drivers/np.c src/drivers/np.h \
				  src/drivers/np_mtp.c \
				  src/drivers/np_rtp.c \
				  src/drivers/npi.c \
				  src/drivers/otk6.c \
				  src/drivers/rmux.c src/drivers/rmux.h \
				  src/drivers/rsw.c \
				  src/drivers/rtp.c \
				  src/drivers/sccop_user.h \
				  src/drivers/sccp2.c \
				  src/drivers/sdl_acb56.c \
				  src/drivers/sdl_x400p.c \
				  src/drivers/sdt_acb56.c \
				  src/drivers/sdt_ch.c \
				  src/drivers/sdt_x400p.c \
				  src/drivers/sdtmux.c \
				  src/drivers/sl-mux.c \
				  src/drivers/sl_acb56.c \
				  src/drivers/sl_v400p.c \
				  src/drivers/slm.c \
				  src/drivers/slmux.c \
				  src/drivers/sscop.c src/drivers/sscop.h \
				  src/drivers/sscop_ip.c \
				  src/drivers/sscop_msgs.h \
				  src/drivers/sscop_n.c \
				  src/drivers/sscop_n2.c \
				  src/drivers/sscop_n_prov.h \
				  src/drivers/sscop_n_user.c \
				  src/drivers/sscop_prov.h \
				  src/drivers/sscop_recv.c \
				  src/drivers/sscop_send.h \
				  src/drivers/sscop_t.c \
				  src/drivers/sscop_t_prov.h \
				  src/drivers/sscop_t_user.c \
				  src/drivers/sscop_time.c \
				  src/drivers/sscop_time.h \
				  src/drivers/sscop_udp.c \
				  src/drivers/sscop_user.h \
				  src/drivers/sua.c src/drivers/sua.h \
				  src/drivers/sua_asp.c src/drivers/sua_asp.h \
				  src/drivers/sua_ctrl.h \
				  src/drivers/sua_data.h \
				  src/drivers/sua_lm.c src/drivers/sua_lm.h \
				  src/drivers/sua_lower.c \
				  src/drivers/sua_msg.c src/drivers/sua_msg.h \
				  src/drivers/sua_nsp.c src/drivers/sua_nsp.h \
				  src/drivers/sua_sccp.c src/drivers/sua_sccp.h \
				  src/drivers/sua_send.h \
				  src/drivers/sua_sgp.c \
				  src/drivers/sua_sm.c \
				  src/drivers/sua_spp.c \
				  src/drivers/sua_ss7.c \
				  src/drivers/sua_tsp.c src/drivers/sua_tsp.h \
				  src/drivers/tali.c src/drivers/tali.h \
				  src/drivers/tali_data.h \
				  src/drivers/tali_isup.c \
				  src/drivers/tali_lm.c \
				  src/drivers/tali_msgs.h \
				  src/drivers/tali_mtp.c \
				  src/drivers/tali_sccp.c \
				  src/drivers/tc.c \
				  src/drivers/testdrv_mps.c \
				  src/drivers/tl.c \
				  src/drivers/tp.c \
				  src/drivers/tr.c \
				  src/drivers/trmux.c \
				  src/drivers/tstdrv.c src/drivers/tstdrv.h \
				  src/drivers/tua.c src/drivers/tua.h \
				  src/drivers/tua_asp.c src/drivers/tua_asp.h \
				  src/drivers/tua_data.h \
				  src/drivers/tua_msg.h \
				  src/drivers/tua_sgp.c \
				  src/drivers/tua_spp.c \
				  src/drivers/tua_ss7.c \
				  src/drivers/ua.c \
				  src/drivers/ua_dlpi.h \
				  src/drivers/ua_msg.h \
				  src/drivers/ua_mtp.h \
				  src/drivers/ua_npi.h \
				  src/drivers/ua_sl.h \
				  src/drivers/ua_tci.h \
				  src/drivers/ua_tpi.h \
				  src/drivers/ua_tri.h \
				  src/drivers/v401p.c \
				  src/drivers/vf.c \
				  src/drivers/wan.c \
				  src/drivers/x100p.c \
				  src/drivers/x25-llc.c \
				  src/drivers/x400p.c \
				  src/drivers/x400p_mx.c \
				  src/drivers/zap.c \
				  src/drivers/zapmux.c \
				  src/drivers/zaptel.c src/drivers/zaptel.h
libOS7drivers_a_SOURCES		= src/drivers/pty.c \
				  src/drivers/debug.h \
				  src/drivers/ip_to_dlpi.c \
				  src/drivers/ldl.c \
				  src/drivers/np_ip.c \
				  src/drivers/np_udp.c \
				  src/drivers/socksys.c \
				  src/drivers/inet.c \
				  src/drivers/udp.c \
				  src/drivers/rawip.c \
				  src/drivers/tcp.c \
				  src/drivers/x25-llc2.c \
				  src/drivers/x25-lapb.c \
				  src/drivers/x25-plp.c \
				  src/drivers/clns.c \
				  src/drivers/atm.c \
				  src/drivers/mtp.c \
				  src/drivers/mtp_min.c \
				  src/drivers/isup.c \
				  src/drivers/sccp.c \
				  src/drivers/tcap.c \
				  src/drivers/sdlm.c \
				  src/drivers/sl_min.c \
				  src/drivers/sl_mux.c \
				  src/drivers/x400p-ss7.c \
				  src/drivers/x100p-ss7.c \
				  src/drivers/sl_x400p.c \
				  src/include/net/bpf.h \
				  src/include/net/lif.h \
				  src/include/net/nit_buf.h \
				  src/include/net/nit_if.h \
				  src/include/net/nit_pf.h \
				  src/drivers/sl_x100p.c \
				  src/drivers/v400pfw.h \
				  src/drivers/v401pfw.h \
				  src/drivers/x400pfw.h \
				  src/drivers/ua_as.c \
				  src/drivers/m2ua_as.c \
				  src/drivers/m3ua_as.c \
				  src/drivers/sua_as.c \
				  src/drivers/tua_as.c \
				  src/drivers/isua_as.c
libOS7drivers_a_CC		= $(KCC)
libOS7drivers_a_CPPFLAGS	= -I$(top_builddir)/src/drivers -I$(top_srcdir)/src/drivers \
				  $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_EXPSYMTAB)
libOS7drivers_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOS7drivers_a_OBJECTS):	AM_V_CC = $(AM_V_KCC)
$(libOS7drivers_a_OBJECTS):	CC = $(KCC)
$(libOS7drivers_a_OBJECTS):	$(STRCONF_MODCONF) $(MODVERSIONS_H) $(STRCONF_CONFIG)

KERNEL_OBJECTS			+= $(libOS7drivers_a_OBJECTS)
KERNEL_MODULES			+= $(libOS7drivers_a_OBJECTS)

if CONFIG_STREAMS_CLONE_MODULE
libOS7drivers_a_SOURCES	       += src/drivers/clone.c src/drivers/clone.h
endif
if CONFIG_STREAMS_CLONE
libOS7streams_a_SOURCES	       += src/drivers/clone.c src/drivers/clone.h
endif
if CONFIG_STREAMS_ECHO_MODULE
libOS7drivers_a_SOURCES	       += src/drivers/echo.c
endif
if CONFIG_STREAMS_ECHO
libOS7streams_a_SOURCES	       += src/drivers/echo.c
endif
if CONFIG_STREAMS_FIFO_MODULE
libOS7drivers_a_SOURCES	       += src/drivers/fifo.c src/drivers/fifo.h
endif
if CONFIG_STREAMS_FIFO
libOS7streams_a_SOURCES	       += src/drivers/fifo.c src/drivers/fifo.h
endif
if CONFIG_STREAMS_LOG_MODULE
libOS7drivers_a_SOURCES	       += src/drivers/log.c src/include/log.h src/include/sys/log.h src/include/strlog.h src/include/sys/strlog.h src/drivers/log.h
endif
if CONFIG_STREAMS_LOG
libOS7streams_a_SOURCES	       += src/drivers/log.c src/include/log.h src/include/sys/log.h src/include/strlog.h src/include/sys/strlog.h src/drivers/log.h
endif
if CONFIG_STREAMS_LOOP_MODULE
libOS7drivers_a_SOURCES	       += src/drivers/loop.c src/include/sys/loop.h
endif
if CONFIG_STREAMS_LOOP
libOS7streams_a_SOURCES	       += src/drivers/loop.c src/include/sys/loop.h
endif
if CONFIG_STREAMS_NSDEV_MODULE
libOS7drivers_a_SOURCES	       += src/drivers/nsdev.c
endif
if CONFIG_STREAMS_NSDEV
libOS7streams_a_SOURCES	       += src/drivers/nsdev.c
endif
if CONFIG_STREAMS_MUX_MODULE
libOS7drivers_a_SOURCES	       += src/drivers/mux.c
endif
if CONFIG_STREAMS_MUX
libOS7streams_a_SOURCES	       += src/drivers/mux.c
endif
if CONFIG_STREAMS_NULS_MODULE
libOS7drivers_a_SOURCES	       += src/drivers/nuls.c
endif
if CONFIG_STREAMS_NULS
libOS7streams_a_SOURCES	       += src/drivers/nuls.c
endif
if CONFIG_STREAMS_PIPE_MODULE
libOS7drivers_a_SOURCES	       += src/drivers/pipe.c src/drivers/pipe.h
endif
if CONFIG_STREAMS_PIPE
libOS7streams_a_SOURCES	       += src/drivers/pipe.c src/drivers/pipe.h
endif
if CONFIG_STREAMS_SAD_MODULE
libOS7drivers_a_SOURCES	       += src/drivers/sad.c src/include/sys/sad.h
endif
if CONFIG_STREAMS_SAD
libOS7streams_a_SOURCES	       += src/drivers/sad.c src/include/sys/sad.h
endif
if CONFIG_STREAMS_SFX_MODULE
libOS7drivers_a_SOURCES	       += src/drivers/sfx.c
endif
if CONFIG_STREAMS_SFX
libOS7streams_a_SOURCES	       += src/drivers/sfx.c
endif
if CONFIG_STREAMS_SPX_MODULE
libOS7drivers_a_SOURCES	       += src/drivers/spx.c
endif
if CONFIG_STREAMS_SPX
libOS7streams_a_SOURCES	       += src/drivers/spx.c
endif
if CONFIG_STREAMS_NIT_MODULE
libOS7drivers_a_SOURCES	       += src/drivers/nit_if.c src/include/net/nit_if.h
endif
if CONFIG_STREAMS_NIT
libOS7streams_a_SOURCES	       += src/drivers/nit_if.c src/include/net/nit_if.h
endif
if CONFIG_STREAMS_BPF_MODULE
libOS7drivers_a_SOURCES	       += src/drivers/bpf.c src/include/net/bpf.h
endif
if CONFIG_STREAMS_BPF
libOS7streams_a_SOURCES	       += src/drivers/bpf.c src/include/net/bpf.h
endif

## =====================================================================

noinst_LIBRARIES		+= libOS7sctp.a

EXTRA_libOS7sctp_a_SOURCES	= src/drivers/sctp.c src/drivers/sctp.h \
				  src/drivers/sctp_cache.c src/drivers/sctp_cache.h \
				  src/drivers/sctp_cookie.c src/drivers/sctp_cookie.h \
				  src/drivers/sctp_hash.c src/drivers/sctp_hash.h \
				  src/drivers/sctp_input.c src/drivers/sctp_input.h \
				  src/drivers/sctp_msg.c src/drivers/sctp_msg.h \
				  src/drivers/sctp_n.c src/drivers/sctp_n.h \
				  src/drivers/sctp_output.c src/drivers/sctp_output.h \
				  src/drivers/sctp_route.c src/drivers/sctp_route.h \
				  src/drivers/sctp_t.c src/drivers/sctp_t.h \
				  src/drivers/sctp_compat.h \
				  src/drivers/sctp_namespace.h \
				  src/drivers/sctp_defs.h
libOS7sctp_a_SOURCES		= src/drivers/sctp2.c src/drivers/sctp_compat.h \
				  src/drivers/sctp_md5.c src/drivers/sctp_md5.h \
				  src/drivers/sctp_hmac_md5.c src/drivers/sctp_hmac_md5.h \
				  src/drivers/sctp_sha1.c src/drivers/sctp_sha1.h \
				  src/drivers/sctp_hmac_sha1.c src/drivers/sctp_hmac_sha1.h \
				  src/drivers/sctp_adler32.c src/drivers/sctp_adler32.h \
				  src/drivers/sctp_crc32c.c src/drivers/sctp_crc32c.h
libOS7sctp_a_CC			= $(KCC)
libOS7sctp_a_CPPFLAGS		= -I$(top_builddir)/src/drivers -I$(top_srcdir)/src/drivers \
				  $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_BLDFLAGS) $(KERNEL_MODFLAGS) $(PKG_MODFLAGS) \
				  $(KERNEL_NOVERSION) $(KERNEL_EXPSYMTAB)
libOS7sctp_a_CFLAGS		= $(KERNEL_CFLAGS)

$(libOS7sctp_a_OBJECTS):	AM_V_CC = $(AM_V_KCC)
$(libOS7sctp_a_OBJECTS):	CC = $(KCC)
$(libOS7sctp_a_OBJECTS):	$(STRCONF_MODCONF) $(MODVERSIONS_H) $(STRCONF_CONFIG)

KERNEL_OBJECTS			+= $(libOS7sctp_a_OBJECTS)
KERNEL_MODULES			+= $(kpre)sctp.o

$(kpre)sctp.o: $(libOS7sctp_a_OBJECTS)
	$(AM_V_CCLD)$(LD) -r -o $@ $^

## PKG_BUILD_KERNEL
endif
## PKG_BUILD_ARCH
endif

##
# These are from po/POTFILES.in
##
EXTRA_DIST			+= src/lib/apli.c \
				   src/lib/cdiapi.c \
				   src/lib/dlpiapi.c \
				   src/lib/dlpi.c \
				   src/lib/netdir.c \
				   src/lib/netdir_inet.c \
				   src/lib/netselect.c \
				   src/lib/npiapi.c \
				   src/lib/osi.c \
				   src/lib/socket.c \
				   src/lib/socklib.c \
				   src/lib/sockpath.c \
				   src/lib/sun_libdlpi.c \
				   src/lib/xap.c \
				   src/lib/xnet.c \
				   src/lib/xnsl.c

EXTRA_TEX			+= scripts/texinfo.cnf \
				   scripts/texi/agpl3.texi \
				   scripts/texi/args.texi.in \
				   scripts/texi/bsd.texi \
				   scripts/texi/compat.texi \
				   scripts/texi/decmit.texi \
				   scripts/texi/fdl12.texi \
				   scripts/texi/fdl13.texi \
				   scripts/texi/fdl.texi \
				   scripts/texi/funding.texi \
				   scripts/texi/gpl3.texi \
				   scripts/texi/gpl.texi \
				   scripts/texi/hp.texi \
				   scripts/texi/install.texi \
				   scripts/texi/isode.texi \
				   scripts/texi/lesser.texi \
				   scripts/texi/lgpl3.texi \
				   scripts/texi/macros.texi \
				   scripts/texi/mail.texi \
				   scripts/texi/mit.texi \
				   scripts/texi/prims.texi \
				   scripts/texi/quick.texi \
				   scripts/texi/sunrpc.texi \
				   scripts/texi/testsuite.texi \
				   scripts/texi/uidlpi.texi \
				   scripts/texi/uinpi.texi \
				   scripts/texi/ui.texi \
				   scripts/texi/uitpi.texi \
				   scripts/texi/uslbsd.texi \
				   scripts/texi/versions.texi

EXTRA_DIST			+= $(EXTRA_TEX)
dist_noinst_SCRIPTS		+= scripts/missing2 scripts/missing3 scripts/missing4
EXTRA_SCRIPTS			+= scripts/missing2 scripts/missing3 scripts/missing4

##
## Rules for building kernel objects
##
KERNEL_PRELOADS			=
KERNEL_MODCONF			= $(STRCONF_CONFMOD)
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
if PKG_BUILD_USER
nodist_modutil_DATA		= src/util/modutils/@PACKAGE_LCNAME@
endif
endif
endif

##
## Rules for building libraries
##
LIBRARY_VERSION_FILES		= src/lib/streams.VERSION \
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

## ##
## # Rules for creating the TCL package index.
## ##
## include am/tcl.am

if PKG_BUILD_USER
## PKG_BUILD_USER

if PKG_BUILD_ARCH
## PKG_BUILD_ARCH

## =====================================================================

LIB_INCLUDES			= -I$(top_builddir)/src/lib -I$(top_srcdir)/src/lib $(PKG_INCLUDES) $(USER_CPPFLAGS)

## =====================================================================

syslib_LTLIBRARIES		+= libstreams.la

libstreams_la_SOURCES		= src/lib/testcancel.c \
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
libstreams_la_CPPFLAGS		= $(LIB_INCLUDES)
libstreams_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
libstreams_la_LDFLAGS		= $(USER_LDFLAGS) -version-info 0:1:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/streams.VERSION

## =====================================================================

lib_LTLIBRARIES			+= libcdiapi.la

libcdiapi_la_SOURCES		= src/lib/pthread.c \
				  src/lib/cdiapi.c \
				  src/include/cdiapi.h \
				  gettext.h
libcdiapi_la_CPPFLAGS		= $(LIB_INCLUDES)
libcdiapi_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
libcdiapi_la_LIBADD		= libstreams.la
libcdiapi_la_LDFLAGS		= $(USER_LDFLAGS) -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/cdiapi.VERSION

## =====================================================================

lib_LTLIBRARIES			+= libdlpiapi.la

libdlpiapi_la_SOURCES		= src/lib/pthread.c \
				  src/lib/dlpiapi.c \
				  src/include/dlpiapi.h \
				  gettext.h
libdlpiapi_la_CPPFLAGS		= $(LIB_INCLUDES)
libdlpiapi_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
libdlpiapi_la_LIBADD		= libstreams.la
libdlpiapi_la_LDFLAGS		= $(USER_LDFLAGS) -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/dlpiapi.VERSION

## =====================================================================

lib_LTLIBRARIES			+= libnpiapi.la

libnpiapi_la_SOURCES		= src/lib/pthread.c \
				  src/lib/npiapi.c \
				  src/include/npiapi.h \
				  gettext.h
libnpiapi_la_CPPFLAGS		= $(LIB_INCLUDES)
libnpiapi_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
libnpiapi_la_LIBADD		= libstreams.la
libnpiapi_la_LDFLAGS		= $(USER_LDFLAGS) -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/npiapi.VERSION

## =====================================================================

lib_LTLIBRARIES			+= libdlpi.la

libdlpi_la_SOURCES		= src/lib/pthread.c \
				  src/lib/dlpi.c \
				  src/include/libdlpi.h \
				  gettext.h
libdlpi_la_CPPFLAGS		= $(LIB_INCLUDES)
libdlpi_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
libdlpi_la_LIBADD		= libstreams.la
libdlpi_la_LDFLAGS		= $(USER_LDFLAGS) -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/dlpi.VERSION

## =====================================================================

lib_LTLIBRARIES			+= libxnet.la

libxnet_la_SOURCES		= src/lib/pthread.c \
				  src/lib/xnet.c \
				  gettext.h
libxnet_la_CPPFLAGS		= $(LIB_INCLUDES)
libxnet_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
libxnet_la_LIBADD		= libstreams.la
libxnet_la_LDFLAGS		= $(USER_LDFLAGS) -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/xnet.VERSION

## =====================================================================

lib_LTLIBRARIES			+= libxnsl.la

libxnsl_la_SOURCES		= src/lib/pthread.c \
				  src/lib/netselect.c \
				  src/lib/netselect.h \
				  src/lib/netdir.c \
				  src/lib/netdir_inet.c \
				  src/include/netconfig.h \
				  src/include/netdir.h \
				  src/include/sys/netconfig.h \
				  gettext.h
libxnsl_la_CPPFLAGS		= $(LIB_INCLUDES)
libxnsl_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
libxnsl_la_LIBADD		= libstreams.la libxnet.la
libxnsl_la_LDFLAGS		= $(USER_LDFLAGS) -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/xnsl.VERSION \
				  -ldl

## =====================================================================

lib_LTLIBRARIES			+= inetaddr.la

inetaddr_la_SOURCES		= src/lib/netdir_inet.c \
				  src/include/netdir.h \
				  src/include/netconfig.h \
				  src/include/sys/netconfig.h
inetaddr_la_CPPFLAGS		= $(LIB_INCLUDES) -DNETDIR_STRONG_DEFS
inetaddr_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
inetaddr_la_LIBADD		= libstreams.la libxnet.la libxnsl.la
inetaddr_la_LDFLAGS		= $(USER_LDFLAGS) -module -export-dynamic -release $(VERSION) -shared

## =====================================================================

lib_LTLIBRARIES			+= libsockpath.la

libsockpath_la_SOURCES		= src/lib/pthread.c \
				  src/lib/sockpath.c \
				  src/lib/sockpath.h \
				  src/include/sockdb.h \
				  src/include/sys/sockpath.h
libsockpath_la_CPPFLAGS		= $(LIB_INCLUDES)
libsockpath_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
libsockpath_la_LIBADD		= libstreams.la
libsockpath_la_LDFLAGS		= $(USER_LDFLAGS) -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/sockpath.VERSION

## =====================================================================

lib_LTLIBRARIES			+= libsocket.la

libsocket_la_SOURCES		= src/lib/pthread.c \
				  src/lib/socket.c \
				  gettext.h
libsocket_la_CPPFLAGS		= $(LIB_INCLUDES)
libsocket_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
libsocket_la_LIBADD		= libstreams.la
libsocket_la_LDFLAGS		= $(USER_LDFLAGS) -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/socket.VERSION \
				  -static -lc -shared

## =====================================================================

lib_LTLIBRARIES			+= libsocklib.la

libsocklib_la_SOURCES		= src/lib/pthread.c \
				  src/lib/socklib.c \
				  gettext.h
libsocklib_la_CPPFLAGS		= $(LIB_INCLUDES)
libsocklib_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
libsocklib_la_LIBADD		= libstreams.la libxnet.la libxnsl.la libsockpath.la
libsocklib_la_LDFLAGS		= $(USER_LDFLAGS) -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/socklib.VERSION \
				  -static -lc -shared

## =====================================================================

lib_LTLIBRARIES			+= libsx25.la

libsx25_la_SOURCES		= src/lib/padent.c \
				  src/lib/x25config.c \
				  src/lib/xhostent.c \
				  src/include/netx25/x25_control.h \
				  src/include/netx25/x25_proto.h \
				  src/include/netx25/xnetdb.h
libsx25_la_CPPFLAGS		= $(LIB_INCLUDES)
libsx25_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
libsx25_la_LIBADD		= libstreams.la
libsx25_la_LDFLAGS		= $(USER_LDFLAGS) -version-info 0:0:0 -export-dynamic \
				  -Wl,--version-script -Wl,$(lib_abs_srcdir)/src/lib/sx25.VERSION

## =====================================================================
## 
## xtitcldir = $(pkgtcllibdir)/xti
## 
## xtitcl_DATA			= src/tcl/xti/pkgIndex.tcl \
## 				  src/tcl/xti/xti.itcl
## 
## xtitcl_LTLIBRARIES		= xti.la
## 
## xti_la_SOURCES			= src/tcl/xti/xti_wrap.c
## xti_la_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS) $(TCL_CPPFLAGS) -DMODULE
## xti_la_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
## xti_la_LIBADD			= libstreams.la libxnet.la
## xti_la_LDFLAGS			= $(USER_LDFLAGS) -module -export-dynamic -release $(VERSION) -shared
## 
## =====================================================================

if WITH_SNMP
if WITH_SNMP_AGENT
snmpdlmod_LTLIBRARIES		+= strMIB.la
endif
endif

strMIB_la_SOURCES		= src/snmp/strMIB.c \
				  src/snmp/strMIB.h \
				  src/snmp/strExtMIB.c \
				  src/snmp/strExtMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h
strMIB_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) $(SNMP_CPPFLAGS) -DMODULE
strMIB_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
strMIB_la_LIBADD		= libstreams.la $(SNMP_LDADD)
strMIB_la_LDFLAGS		= $(USER_LDFLAGS) -module -export-dynamic -release $(VERSION) -shared $(SNMP_LDFLAGS)

## =====================================================================

if WITH_SNMP
if WITH_SNMP_AGENT
snmpdlmod_LTLIBRARIES		+= mxMIB.la
endif
endif

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
mxMIB_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) $(SNMP_CPPFLAGS) -DMODULE
mxMIB_la_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
mxMIB_la_LIBADD			= libstreams.la $(SNMP_LDADD)
mxMIB_la_LDFLAGS		= $(USER_LDFLAGS) -module -export-dynamic -release $(VERSION) -shared $(SNMP_LDFLAGS)

## =====================================================================

if WITH_SNMP
if WITH_SNMP_AGENT
snmpdlmod_LTLIBRARIES		+= ss7MIB.la
endif
endif

ss7MIB_la_SOURCES		= src/snmp/ss7MIB.c \
				  src/snmp/ss7MIB.h \
				  src/snmp/mtpMIB.c \
				  src/snmp/mtpMIB.h \
				  src/snmp/mtpOmMIB.c \
				  src/snmp/mtpOmMIB.h \
				  src/snmp/sccpMIB.c \
				  src/snmp/sccpMIB.h \
				  src/snmp/sccpOmMIB.c \
				  src/snmp/sccpOmMIB.h \
				  src/snmp/tcapMIB.c \
				  src/snmp/tcapMIB.h \
				  src/snmp/tcapOmMIB.c \
				  src/snmp/tcapOmMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h
ss7MIB_la_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) $(SNMP_CPPFLAGS) -DMODULE
ss7MIB_la_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
ss7MIB_la_LIBADD		= libstreams.la $(SNMP_LDADD)
ss7MIB_la_LDFLAGS		= $(USER_LDFLAGS) -module -export-dynamic -release $(VERSION) -shared $(SNMP_LDFLAGS)

## =====================================================================

UTIL_INCLUDES			= -I$(top_builddir)/src/util -I$(top_srcidr)/src/util $(PKG_INCLUDES) $(USER_CPPFLAGS)

## =====================================================================

strchg_SOURCES			= src/util/strchg.c
strchg_CPPFLAGS			= $(UTIL_INCLUDES)
strchg_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
strchg_LDFLAGS			= $(USER_LDFLAGS)
strchg_LDADD			= libstreams.la

sysbin_PROGRAMS			+= strchg

## =====================================================================

strconf_SOURCES			= src/util/strconf.c
strconf_CPPFLAGS		= $(UTIL_INCLUDES)
strconf_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
strconf_LDFLAGS			= $(USER_LDFLAGS)
strconf_LDADD			= libstreams.la

sysbin_PROGRAMS			+= strconf

## =====================================================================

strreset_SOURCES		= src/util/strreset.c
strreset_CPPFLAGS		= $(UTIL_INCLUDES)
strreset_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
strreset_LDFLAGS		= $(USER_LDFLAGS)
strreset_LDADD			= libstreams.la

sysbin_PROGRAMS			+= strreset

## =====================================================================
#
#asn_SOURCES			= src/util/asnyac.y src/util/asnlex.l asnyac.h
#asn_CPPFLAGS			= $(UTIL_INCLUDES)
#asn_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
#asn_LDFLAGS			= $(USER_LDFLAGS)
#
#EXTRA_PROGRAMS			+= asn
##bin_PROGRAMS			+= asn
#
## =====================================================================

gdmo_SOURCES			= src/util/gdmoyac.y src/util/gdmolex.l gdmoyac.h
gdmo_CPPFLAGS			= $(UTIL_INCLUDES)
gdmo_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
gdmo_LDFLAGS			= $(USER_LDFLAGS)

bin_PROGRAMS			+= gdmo

## =====================================================================

strlog_SOURCES			= src/util/strlog.c
strlog_CPPFLAGS			= $(UTIL_INCLUDES)
strlog_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
strlog_LDFLAGS			= $(USER_LDFLAGS)
strlog_LDADD			= -lrt

sysbin_PROGRAMS			+= strlog

## =====================================================================

autopush_SOURCES		= src/util/autopush.c
autopush_CPPFLAGS		= $(UTIL_INCLUDES)
autopush_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
autopush_LDFLAGS		= $(USER_LDFLAGS)
autopush_LDADD			= libstreams.la

syssbin_PROGRAMS		+= autopush

## =====================================================================

fattach_SOURCES			= src/util/fattach.c
fattach_CPPFLAGS		= $(UTIL_INCLUDES)
fattach_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
fattach_LDFLAGS			= $(USER_LDFLAGS)
fattach_LDADD			= libstreams.la

syssbin_PROGRAMS		+= fattach

## =====================================================================

fdetach_SOURCES			= src/util/fdetach.c
fdetach_CPPFLAGS		= $(UTIL_INCLUDES)
fdetach_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
fdetach_LDFLAGS			= $(USER_LDFLAGS)
fdetach_LDADD			= libstreams.la

syssbin_PROGRAMS		+= fdetach

## =====================================================================

insf_SOURCES			= src/util/insf.c
insf_CPPFLAGS			= $(UTIL_INCLUDES)
insf_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
insf_LDFLAGS			= $(USER_LDFLAGS)
insf_LDADD			= libstreams.la

syssbin_PROGRAMS		+= insf

## =====================================================================

scls_SOURCES			= src/util/scls.c
scls_CPPFLAGS			= $(UTIL_INCLUDES)
scls_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
scls_LDFLAGS			= $(USER_LDFLAGS)
scls_LDADD			= libstreams.la

sbin_PROGRAMS			+= scls

## =====================================================================

strace_SOURCES			= src/util/strace.c
strace_CPPFLAGS			= $(UTIL_INCLUDES)
strace_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
strace_LDFLAGS			= $(USER_LDFLAGS)
strace_LDADD			= libstreams.la

sbin_PROGRAMS			+= strace

## =====================================================================

##strclean_SOURCES		= src/util/strclean.c
##strclean_CPPFLAGS		= $(UTIL_INCLUDES)
##strclean_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
##strclean_LDFLAGS		= $(USER_LDFLAGS)
##strclean_LDADD		= libstreams.la

#sbin_PROGRAMS			+= strclean

##@srcdir@/src/util/strclean.c:
##	touch $@

##@srcdir@/src/util/strclean: @srcdir@/src/util/strclean.c
##	touch $@

dist_sbin_SCRIPTS		+= src/util/strclean

## =====================================================================

strerr_SOURCES			= src/util/strerr.c
strerr_CPPFLAGS			= $(UTIL_INCLUDES)
strerr_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
strerr_LDFLAGS			= $(USER_LDFLAGS)
strerr_LDADD			= libstreams.la

sbin_PROGRAMS			+= strerr

## =====================================================================

strinfo_SOURCES			= src/util/strinfo.c
strinfo_CPPFLAGS		= $(UTIL_INCLUDES)
strinfo_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
strinfo_LDFLAGS			= $(USER_LDFLAGS)
strinfo_LDADD			= libstreams.la

sbin_PROGRAMS			+= strinfo

## =====================================================================

##strload_SOURCES		= src/util/strload.c
##strload_CPPFLAGS		= $(UTIL_INCLUDES)
##strload_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
##strload_LDFLAGS		= $(USER_LDFLAGS)
##strload_LDADD			= libstreams.la

#sbin_PROGRAMS			+= strload

##@srcdir@/src/util/strload.c:
##	touch $@

##@srcdir@/src/util/strload: @srcdir@/src/util/strload.c
##	touch $@

dist_sbin_SCRIPTS		+= src/util/strload

## =====================================================================

##strsetup_SOURCES		= src/util/strsetup.c
##strsetup_CPPFLAGS		= $(UTIL_INCLUDES)
##strsetup_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
##strsetup_LDFLAGS		= $(USER_LDFLAGS)
##strsetup_LDADD		= libstreams.la

#sbin_PROGRAMS			+= strsetup

##@srcdir@/src/util/strsetup.c:
##	touch $@

##@srcdir@/src/util/strsetup: @srcdir@/src/util/strsetup.c
##	touch $@

dist_sbin_SCRIPTS		+= src/util/strsetup

## =====================================================================

strvf_SOURCES			= src/util/strvf.c
strvf_CPPFLAGS			= $(UTIL_INCLUDES)
strvf_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
strvf_LDFLAGS			= $(USER_LDFLAGS)
strvf_LDADD			= libstreams.la

sbin_PROGRAMS			+= strvf

## =====================================================================

dist_noinst_SCRIPTS		+= src/gdmo/gdmocheck

## =====================================================================

dist_noinst_SCRIPTS		+= src/gdmo/snmpcheck

## =====================================================================

dist_noinst_SCRIPTS		+= src/gdmo/asn1check

## =====================================================================

@PACKAGE_TARNAME@_mknod_SOURCES		= src/util/@PACKAGE_TARNAME@_mknod.c
@PACKAGE_TARNAME@_mknod_CPPFLAGS	= $(UTIL_INCLUDES)
@PACKAGE_TARNAME@_mknod_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
@PACKAGE_TARNAME@_mknod_LDFLAGS		= $(USER_LDFLAGS)

sbin_PROGRAMS			+= @PACKAGE_TARNAME@_mknod

## =====================================================================

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
streamsd_CPPFLAGS		= $(PKG_INCLUDES) $(USER_CPPFLAGS) $(SNMP_CPPFLAGS)
streamsd_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
streamsd_LDADD			= libstreams.la $(SNMP_LDADD)
streamsd_LDFLAGS		= $(USER_LDFLAGS) $(SNMP_LDFLAGS)

if WITH_SNMP
if WITH_SNMP_AGENT
sbin_PROGRAMS			+= streamsd
endif
endif
EXTRA_PROGRAMS			+= streamsd

## =====================================================================

ldlconfig_SOURCES		= src/util/ldlconfig.c
ldlconfig_CPPFLAGS		= $(UTIL_INCLUDES)
ldlconfig_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
ldlconfig_LDFLAGS		= $(USER_LDFLAGS)
ldlconfig_LDADD			= libstreams.la

sbin_PROGRAMS			+= ldlconfig

## =====================================================================

initsock_SOURCES		= src/util/initsock.c
initsock_CPPFLAGS		= $(UTIL_INCLUDES)
initsock_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
initsock_LDFLAGS		= $(USER_LDFLAGS)
initsock_LDADD			= libstreams.la

sbin_PROGRAMS			+= initsock

## =====================================================================

soconfig_SOURCES		= src/util/soconfig.c
soconfig_CPPFLAGS		= $(UTIL_INCLUDES)
soconfig_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
soconfig_LDFLAGS		= $(USER_LDFLAGS)
soconfig_LDADD			= libstreams.la

syssbin_PROGRAMS		+= soconfig

## =====================================================================

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
chand_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS) $(SNMP_CPPFLAGS)
chand_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
chand_LDADD			= libstreams.la $(SNMP_LDADD)
chand_LDFLAGS			= $(USER_LDFLAGS) $(SNMP_LDFLAGS)

if WITH_SNMP
if WITH_SNMP_AGENT
sbin_PROGRAMS			+= chand
endif
endif
EXTRA_PROGRAMS			+= chand

## =====================================================================

xmond_SOURCES			= src/snmp/xmond.c \
				  src/snmp/xmond.h \
				  src/snmp/xmonMIB.c \
				  src/snmp/xmonMIB.h \
				  src/snmp/ds_agent.h \
				  src/snmp/header_complex.h \
				  src/snmp/mib_modules.h \
				  src/snmp/struct.h \
				  src/snmp/util_funcs.h
xmond_CPPFLAGS			= $(PKG_INCLUDES) $(USER_CPPFLAGS) $(SNMP_CPPFLAGS)
xmond_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
xmond_LDADD			= libstreams.la $(SNMP_LDADD)
xmond_LDFLAGS			= $(USER_LDFLAGS) $(SNMP_LDFLAGS) -lpcap

if WITH_SNMP
if WITH_SNMP_AGENT
if WITH_LIB_PCAP
sbin_PROGRAMS			+= xmond
endif
endif
endif
EXTRA_PROGRAMS			+= xmond

## =====================================================================

ifrdlci_SOURCES			= src/util/ifrdlci.c
ifrdlci_CPPFLAGS		= $(UTIL_INCLUDES)
ifrdlci_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
ifrdlci_LDFLAGS			= $(USER_LDFLAGS)
ifrdlci_LDADD			= libstreams.la

sbin_PROGRAMS			+= ifrdlci

## =====================================================================

ifrstat_SOURCES			= src/util/ifrstat.c
ifrstat_CPPFLAGS		= $(UTIL_INCLUDES)
ifrstat_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
ifrstat_LDFLAGS			= $(USER_LDFLAGS)
ifrstat_LDADD			= libstreams.la

sbin_PROGRAMS			+= ifrstat

## =====================================================================

ifrtune_SOURCES			= src/util/ifrtune.c
ifrtune_CPPFLAGS		= $(UTIL_INCLUDES)
ifrtune_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
ifrtune_LDFLAGS			= $(USER_LDFLAGS)
ifrtune_LDADD			= libstreams.la

sbin_PROGRAMS			+= ifrtune

## =====================================================================

ixemap_SOURCES			= src/util/ixemap.c
ixemap_CPPFLAGS			= $(UTIL_INCLUDES)
ixemap_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
ixemap_LDFLAGS			= $(USER_LDFLAGS)
ixemap_LDADD			= libstreams.la

sbin_PROGRAMS			+= ixemap

## =====================================================================

ixetune_SOURCES			= src/util/ixetune.c
ixetune_CPPFLAGS		= $(UTIL_INCLUDES)
ixetune_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
ixetune_LDFLAGS			= $(USER_LDFLAGS)
ixetune_LDADD			= libstreams.la

sbin_PROGRAMS			+= ixetune

## =====================================================================

linkadd_SOURCES			= src/util/linkadd.c
linkadd_CPPFLAGS		= $(UTIL_INCLUDES)
linkadd_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
linkadd_LDFLAGS			= $(USER_LDFLAGS)
linkadd_LDADD			= libstreams.la

sbin_PROGRAMS			+= linkadd

## =====================================================================

linkdel_SOURCES			= src/util/linkdel.c
linkdel_CPPFLAGS		= $(UTIL_INCLUDES)
linkdel_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
linkdel_LDFLAGS			= $(USER_LDFLAGS)
linkdel_LDADD			= libstreams.la

sbin_PROGRAMS			+= linkdel

## =====================================================================

linklist_SOURCES		= src/util/linklist.c
linklist_CPPFLAGS		= $(UTIL_INCLUDES)
linklist_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
linklist_LDFLAGS		= $(USER_LDFLAGS)
linklist_LDADD			= libstreams.la

sbin_PROGRAMS			+= linklist

## =====================================================================

linkreset_SOURCES		= src/util/linkreset.c
linkreset_CPPFLAGS		= $(UTIL_INCLUDES)
linkreset_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
linkreset_LDFLAGS		= $(USER_LDFLAGS)
linkreset_LDADD			= libstreams.la

sbin_PROGRAMS			+= linkreset

## =====================================================================

linkstart_SOURCES		= src/util/linkstart.c
linkstart_CPPFLAGS		= $(UTIL_INCLUDES)
linkstart_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
linkstart_LDFLAGS		= $(USER_LDFLAGS)
linkstart_LDADD			= libstreams.la

sbin_PROGRAMS			+= linkstart

## =====================================================================

linkstate_SOURCES		= src/util/linkstate.c
linkstate_CPPFLAGS		= $(UTIL_INCLUDES)
linkstate_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
linkstate_LDFLAGS		= $(USER_LDFLAGS)
linkstate_LDADD			= libstreams.la

sbin_PROGRAMS			+= linkstate

## =====================================================================

linkstop_SOURCES		= src/util/linkstop.c
linkstop_CPPFLAGS		= $(UTIL_INCLUDES)
linkstop_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
linkstop_LDFLAGS		= $(USER_LDFLAGS)
linkstop_LDADD			= libstreams.la

sbin_PROGRAMS			+= linkstop

## =====================================================================

lltune_SOURCES			= src/util/lltune.c
lltune_CPPFLAGS			= $(UTIL_INCLUDES)
lltune_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
lltune_LDFLAGS			= $(USER_LDFLAGS)
lltune_LDADD			= libstreams.la

EXTRA_PROGRAMS			+= lltune
#sbin_PROGRAMS			+= lltune

## =====================================================================

mlptune_SOURCES			= src/util/mlptune.c
mlptune_CPPFLAGS		= $(UTIL_INCLUDES)
mlptune_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
mlptune_LDFLAGS			= $(USER_LDFLAGS)
mlptune_LDADD			= libstreams.la

sbin_PROGRAMS			+= mlptune

## =====================================================================

nuimap_SOURCES			= src/util/nuimap.c
nuimap_CPPFLAGS			= $(UTIL_INCLUDES)
nuimap_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
nuimap_LDFLAGS			= $(USER_LDFLAGS)
nuimap_LDADD			= libstreams.la

sbin_PROGRAMS			+= nuimap

## =====================================================================

pvcmap_SOURCES			= src/util/pvcmap.c
pvcmap_CPPFLAGS			= $(UTIL_INCLUDES)
pvcmap_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
pvcmap_LDFLAGS			= $(USER_LDFLAGS)
pvcmap_LDADD			= libstreams.la

sbin_PROGRAMS			+= pvcmap

## =====================================================================

vcstat_SOURCES			= src/util/vcstat.c
vcstat_CPPFLAGS			= $(UTIL_INCLUDES)
vcstat_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
vcstat_LDFLAGS			= $(USER_LDFLAGS)
vcstat_LDADD			= libstreams.la

sbin_PROGRAMS			+= vcstat

## =====================================================================

wantune_SOURCES			= src/util/wantune.c
wantune_CPPFLAGS		= $(UTIL_INCLUDES)
wantune_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
wantune_LDFLAGS			= $(USER_LDFLAGS)
wantune_LDADD			= libstreams.la

sbin_PROGRAMS			+= wantune

## =====================================================================

x25diags_SOURCES		= src/util/x25diags.c
x25diags_CPPFLAGS		= $(UTIL_INCLUDES)
x25diags_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
x25diags_LDFLAGS		= $(USER_LDFLAGS)
x25diags_LDADD			= libstreams.la

sbin_PROGRAMS			+= x25diags

## =====================================================================

x25file_SOURCES			= src/util/x25file.c
x25file_CPPFLAGS		= $(UTIL_INCLUDES)
x25file_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
x25file_LDFLAGS			= $(USER_LDFLAGS)
x25file_LDADD			= libstreams.la

sbin_PROGRAMS			+= x25file

## =====================================================================

x25info_SOURCES			= src/util/x25info.c
x25info_CPPFLAGS		= $(UTIL_INCLUDES)
x25info_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
x25info_LDFLAGS			= $(USER_LDFLAGS)
x25info_LDADD			= libstreams.la

sbin_PROGRAMS			+= x25info

## =====================================================================

x25netd_SOURCES			= src/util/x25netd.c
x25netd_CPPFLAGS		= $(UTIL_INCLUDES)
x25netd_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
x25netd_LDFLAGS			= $(USER_LDFLAGS)
x25netd_LDADD			= libstreams.la

sbin_PROGRAMS			+= x25netd

## =====================================================================

x25route_SOURCES		= src/util/x25route.c
x25route_CPPFLAGS		= $(UTIL_INCLUDES)
x25route_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
x25route_LDFLAGS		= $(USER_LDFLAGS)
x25route_LDADD			= libstreams.la

sbin_PROGRAMS			+= x25route

## =====================================================================

x25stat_SOURCES			= src/util/x25stat.c
x25stat_CPPFLAGS		= $(UTIL_INCLUDES)
x25stat_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
x25stat_LDFLAGS			= $(USER_LDFLAGS)
x25stat_LDADD			= libstreams.la

sbin_PROGRAMS			+= x25stat

## =====================================================================

x25trace_SOURCES		= src/util/x25trace.c
x25trace_CPPFLAGS		= $(UTIL_INCLUDES)
x25trace_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
x25trace_LDFLAGS		= $(USER_LDFLAGS)
x25trace_LDADD			= libstreams.la

sbin_PROGRAMS			+= x25trace

## =====================================================================

x25tune_SOURCES			= src/util/x25tune.c
x25tune_CPPFLAGS		= $(UTIL_INCLUDES)
x25tune_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
x25tune_LDFLAGS			= $(USER_LDFLAGS)
x25tune_LDADD			= libstreams.la

EXTRA_PROGRAMS			+= x25tune
#sbin_PROGRAMS			+= x25tune

## =====================================================================

x32map_SOURCES			= src/util/x32map.c
x32map_CPPFLAGS			= $(UTIL_INCLUDES)
x32map_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
x32map_LDFLAGS			= $(USER_LDFLAGS)
x32map_LDADD			= libstreams.la

sbin_PROGRAMS			+= x32map

## =====================================================================

ss7statsd_SOURCES		= src/util/ss7statsd.c
ss7statsd_CPPFLAGS		= $(UTIL_INCLUDES)
ss7statsd_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
ss7statsd_LDFLAGS		= $(USER_LDFLAGS)
ss7statsd_LDADD			= libstreams.la

sbin_PROGRAMS			+= ss7statsd

## =====================================================================

slconfig_SOURCES		= src/util/slconf_yac.y \
				  src/util/slconf_lex.l \
				  src/util/slconf.c \
				  slconf_yac.h
slconfig_CPPFLAGS		= $(UTIL_INCLUDES)
slconfig_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
slconfig_LDFLAGS		= $(USER_LDFLAGS)
slconfig_LDADD			= libstreams.la

EXTRA_PROGRAMS			+= slconfig
sbin_PROGRAMS			+= slconfig

## =====================================================================

mtpconfig_SOURCES		= src/util/mtpconf_yac.y \
				  src/util/mtpconf_lex.l \
				  mtpconf_yac.h
mtpconfig_CPPFLAGS		= $(UTIL_INCLUDES)
mtpconfig_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
mtpconfig_LDFLAGS		= $(USER_LDFLAGS)
mtpconfig_LDADD			= libstreams.la

sbin_PROGRAMS			+= mtpconfig

## =====================================================================

x400config_SOURCES		= src/util/x400yac.y \
				  src/util/x400lex.l \
				  src/util/x400config.h \
				  x400yac.h
x400config_CPPFLAGS		= $(UTIL_INCLUDES)
x400config_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
x400config_LDFLAGS		= $(USER_LDFLAGS)
x400config_LDADD		= libstreams.la

sbin_PROGRAMS			+= x400config

## =====================================================================

sdlconfig_SOURCES		= src/util/sdlconf.c \
				  src/util/sdlyac.y \
				  src/util/sdllex.l \
				  sdlyac.h
sdlconfig_CPPFLAGS		= $(UTIL_INCLUDES)
sdlconfig_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
sdlconfig_LDFLAGS		= $(USER_LDFLAGS)
sdlconfig_LDADD			= libstreams.la

sbin_PROGRAMS			+= sdlconfig

## =====================================================================

soconf_SOURCES			= src/util/soconf.c
soconf_CPPFLAGS			= $(UTIL_INCLUDES)
soconf_LDFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
soconf_LDADD			= libstreams.la

sbin_PROGRAMS			+= soconf

## =====================================================================

mxconfig_SOURCES		= src/util/mxconf.c
mxconfig_CPPFLAGS		= $(UTIL_INCLUDES)
mxconfig_LDFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
mxconfig_LDADD			= libstreams.la

sbin_PROGRAMS			+= mxconfig

## =====================================================================

slconfd_SOURCES			= src/util/slconfd.c
slconfd_CPPFLAGS		= $(UTIL_INCLUDES)
slconfd_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
slconfd_LDFLAGS			= $(USER_LDFLAGS)
slconfd_LDADD			= libstreams.la

sbin_PROGRAMS			+= slconfd

## =====================================================================

slmon_SOURCES			= src/util/slmon.c
slmon_CPPFLAGS			= $(UTIL_INCLUDES)
slmon_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
slmon_LDFLAGS			= $(USER_LDFLAGS)
slmon_LDADD			= libstreams.la

sbin_PROGRAMS			+= slmon

## =====================================================================

ss7capd_SOURCES			= src/util/ss7capd.c
ss7capd_CPPFLAGS		= $(UTIL_INCLUDES)
ss7capd_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
ss7capd_LDFLAGS			= $(USER_LDFLAGS)
ss7capd_LDADD			= libstreams.la

sbin_PROGRAMS			+= ss7capd

## =====================================================================

m2paconfig_SOURCES		= src/util/m2payac.y \
				  src/util/m2palex.l \
				  m2payac.h
m2paconfig_CPPFLAGS		= $(UTIL_INCLUDES)
m2paconfig_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
m2paconfig_LDFLAGS		= $(USER_LDFLAGS)
m2paconfig_LDADD		= libstreams.la

sbin_PROGRAMS			+= m2paconfig

## =====================================================================

addnsap_SOURCES			= src/util/addnsap.c
addnsap_CPPFLAGS		= $(UTIL_INCLUDES)
addnsap_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
addnsap_LDFLAGS			= $(USER_LDFLAGS)
addnsap_LDADD			= libstreams.la

sbin_PROGRAMS			+= addnsap

## =====================================================================

delnsap_SOURCES			= src/util/delnsap.c
delnsap_CPPFLAGS		= $(UTIL_INCLUDES)
delnsap_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
delnsap_LDFLAGS			= $(USER_LDFLAGS)
delnsap_LDADD			= libstreams.la

sbin_PROGRAMS			+= delnsap

## =====================================================================

nsap_SOURCES			= src/util/nsap.c
nsap_CPPFLAGS			= $(UTIL_INCLUDES)
nsap_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
nsap_LDFLAGS			= $(USER_LDFLAGS)
nsap_LDADD			= libstreams.la

sbin_PROGRAMS			+= nsap

## =====================================================================

dtemap_SOURCES			= src/util/dtemap.c
dtemap_CPPFLAGS			= $(UTIL_INCLUDES)
dtemap_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
dtemap_LDFLAGS			= $(USER_LDFLAGS)
dtemap_LDADD			= libstreams.la

sbin_PROGRAMS			+= dtemap

## =====================================================================

esq_SOURCES			= src/util/esq.c
esq_CPPFLAGS			= $(UTIL_INCLUDES)
esq_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
esq_LDFLAGS			= $(USER_LDFLAGS)
esq_LDADD			= libstreams.la

sbin_PROGRAMS			+= esq

## =====================================================================

ipconfig_SOURCES		= src/util/ipconfig.c
ipconfig_CPPFLAGS		= $(UTIL_INCLUDES)
ipconfig_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
ipconfig_LDFLAGS		= $(USER_LDFLAGS)
ipconfig_LDADD			= libstreams.la

sbin_PROGRAMS			+= ipconfig

## =====================================================================

ipstat_SOURCES			= src/util/ipstat.c
ipstat_CPPFLAGS			= $(UTIL_INCLUDES)
ipstat_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
ipstat_LDFLAGS			= $(USER_LDFLAGS)
ipstat_LDADD			= libstreams.la

sbin_PROGRAMS			+= ipstat

## =====================================================================

isq_SOURCES			= src/util/isq.c
isq_CPPFLAGS			= $(UTIL_INCLUDES)
isq_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
isq_LDFLAGS			= $(USER_LDFLAGS)
isq_LDADD			= libstreams.la

sbin_PROGRAMS			+= isq

## =====================================================================

mac_SOURCES			= src/util/mac.c
mac_CPPFLAGS			= $(UTIL_INCLUDES)
mac_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
mac_LDFLAGS			= $(USER_LDFLAGS)
mac_LDADD			= libstreams.la

sbin_PROGRAMS			+= mac

## =====================================================================

netitle_SOURCES			= src/util/netitle.c
netitle_CPPFLAGS		= $(UTIL_INCLUDES)
netitle_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
netitle_LDFLAGS			= $(USER_LDFLAGS)
netitle_LDADD			= libstreams.la

sbin_PROGRAMS			+= netitle

## =====================================================================

pingllc_SOURCES			= src/util/pingllc.c
pingllc_CPPFLAGS		= $(UTIL_INCLUDES)
pingllc_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
pingllc_LDFLAGS			= $(USER_LDFLAGS)
pingllc_LDADD			= libstreams.la

sbin_PROGRAMS			+= pingllc

## =====================================================================

rtgconfig_SOURCES		= src/util/rtgconfig.c
rtgconfig_CPPFLAGS		= $(UTIL_INCLUDES)
rtgconfig_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
rtgconfig_LDFLAGS		= $(USER_LDFLAGS)
rtgconfig_LDADD			= libstreams.la

sbin_PROGRAMS			+= rtgconfig

## =====================================================================

rtgstat_SOURCES			= src/util/rtgstat.c
rtgstat_CPPFLAGS		= $(UTIL_INCLUDES)
rtgstat_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
rtgstat_LDFLAGS			= $(USER_LDFLAGS)
rtgstat_LDADD			= libstreams.la

sbin_PROGRAMS			+= rtgstat

## =====================================================================

rtl_SOURCES			= src/util/rtl.c
rtl_CPPFLAGS			= $(UTIL_INCLUDES)
rtl_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
rtl_LDFLAGS			= $(USER_LDFLAGS)
rtl_LDADD			= libstreams.la

sbin_PROGRAMS			+= rtl

## =====================================================================

sndcmap_SOURCES			= src/util/sndcmap.c
sndcmap_CPPFLAGS		= $(UTIL_INCLUDES)
sndcmap_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
sndcmap_LDFLAGS			= $(USER_LDFLAGS)
sndcmap_LDADD			= libstreams.la

sbin_PROGRAMS			+= sndcmap

## =====================================================================

tp24config_SOURCES		= src/util/tp24config.c
tp24config_CPPFLAGS		= $(UTIL_INCLUDES)
tp24config_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
tp24config_LDFLAGS		= $(USER_LDFLAGS)
tp24config_LDADD		= libstreams.la

sbin_PROGRAMS			+= tp24config

## ---------------------------------------------------------------------
## pkglibexec_PROGRAMS
## ---------------------------------------------------------------------
## =====================================================================

TEST_INCLUDES			= -I$(top_builddir)/src/test -I$(top_srcdir)/src/test $(PKG_INCLUDES) $(USER_CPPFLAGS)

## =====================================================================

test_clone_SOURCES		= src/test/test-clone.c
test_clone_CPPFLAGS		= $(TEST_INCLUDES)
test_clone_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_clone_LDFLAGS		= $(USER_LDFLAGS)
test_clone_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-clone

## =====================================================================

test_connld_SOURCES		= src/test/test-connld.c
test_connld_CPPFLAGS		= $(TEST_INCLUDES)
test_connld_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_connld_LDFLAGS		= $(USER_LDFLAGS)
test_connld_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-connld

## =====================================================================

test_echo_SOURCES		= src/test/test-echo.c
test_echo_CPPFLAGS		= $(TEST_INCLUDES)
test_echo_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_echo_LDFLAGS		= $(USER_LDFLAGS)
test_echo_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-echo

## =====================================================================

test_fifo_SOURCES		= src/test/test-fifo.c
test_fifo_CPPFLAGS		= $(TEST_INCLUDES)
test_fifo_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_fifo_LDFLAGS		= $(USER_LDFLAGS)
test_fifo_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-fifo

## =====================================================================

test_log_SOURCES		= src/test/test-log.c
test_log_CPPFLAGS		= $(TEST_INCLUDES)
test_log_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
test_log_LDFLAGS		= $(USER_LDFLAGS)
test_log_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-log

## =====================================================================

test_loop_SOURCES		= src/test/test-loop.c
test_loop_CPPFLAGS		= $(TEST_INCLUDES)
test_loop_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_loop_LDFLAGS		= $(USER_LDFLAGS)
test_loop_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-loop

## =====================================================================

test_nsdev_SOURCES		= src/test/test-nsdev.c
test_nsdev_CPPFLAGS		= $(TEST_INCLUDES)
test_nsdev_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_nsdev_LDFLAGS		= $(USER_LDFLAGS)
test_nsdev_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-nsdev

## =====================================================================

test_mux_SOURCES		= src/test/test-mux.c
test_mux_CPPFLAGS		= $(TEST_INCLUDES)
test_mux_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
test_mux_LDFLAGS		= $(USER_LDFLAGS)
test_mux_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-mux

## =====================================================================

test_nuls_SOURCES		= src/test/test-nuls.c
test_nuls_CPPFLAGS		= $(TEST_INCLUDES)
test_nuls_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_nuls_LDFLAGS		= $(USER_LDFLAGS)
test_nuls_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-nuls

## =====================================================================

test_pipe_SOURCES		= src/test/test-pipe.c
test_pipe_CPPFLAGS		= $(TEST_INCLUDES)
test_pipe_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_pipe_LDFLAGS		= $(USER_LDFLAGS)
test_pipe_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-pipe

## =====================================================================

perftest_SOURCES		= src/test/perftest.c
perftest_CPPFLAGS		= $(TEST_INCLUDES)
perftest_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
perftest_LDFLAGS		= $(USER_LDFLAGS)
perftest_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= perftest

## =====================================================================

perftestn_SOURCES		= src/test/perftest.c
perftestn_CPPFLAGS		= $(TEST_INCLUDES) -DNATIVE_PIPES
perftestn_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
perftestn_LDFLAGS		= $(USER_LDFLAGS)

pkglibexec_PROGRAMS		+= perftestn

## =====================================================================

test_pipemod_SOURCES		= src/test/test-pipemod.c
test_pipemod_CPPFLAGS		= $(TEST_INCLUDES)
test_pipemod_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_pipemod_LDFLAGS		= $(USER_LDFLAGS)
test_pipemod_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-pipemod

## =====================================================================

test_sad_SOURCES		= src/test/test-sad.c
test_sad_CPPFLAGS		= $(TEST_INCLUDES)
test_sad_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
test_sad_LDFLAGS		= $(USER_LDFLAGS)
test_sad_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-sad

## =====================================================================

test_sc_SOURCES			= src/test/test-sc.c
test_sc_CPPFLAGS		= $(TEST_INCLUDES)
test_sc_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
test_sc_LDFLAGS			= $(USER_LDFLAGS)
test_sc_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-sc

## =====================================================================

test_streams_SOURCES		= src/test/test-streams.c
test_streams_CPPFLAGS		= $(TEST_INCLUDES)
test_streams_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_streams_LDFLAGS		= $(USER_LDFLAGS)
test_streams_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-streams

## =====================================================================

testlog_SOURCES			= src/test/testlog.c
testlog_CPPFLAGS		= $(TEST_INCLUDES)
testlog_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
testlog_LDFLAGS			= $(USER_LDFLAGS)
testlog_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= testlog

## =====================================================================

test_tty_SOURCES		= src/test/test-tty.c
test_tty_CPPFLAGS		= $(TEST_INCLUDES)
test_tty_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
test_tty_LDFLAGS		= $(USER_LDFLAGS)
test_tty_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-tty

## =====================================================================

ldltest_SOURCES			= src/test/ldltest.c
ldltest_CPPFLAGS		= $(TEST_INCLUDES)
ldltest_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
ldltest_LDFLAGS			= $(USER_LDFLAGS)
ldltest_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= ldltest

## =====================================================================

test_np_ip_SOURCES		= src/test/test-np_ip.c
test_np_ip_CPPFLAGS		= $(TEST_INCLUDES)
test_np_ip_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_np_ip_LDFLAGS		= $(USER_LDFLAGS)
test_np_ip_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-np_ip

## =====================================================================

test_ip_SOURCES			= src/test/test-ip.c
test_ip_CPPFLAGS		= $(TEST_INCLUDES)
test_ip_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
test_ip_LDFLAGS			= $(USER_LDFLAGS)
test_ip_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-ip

## =====================================================================

test_tirdwr_SOURCES		= src/test/test-tirdwr.c
test_tirdwr_CPPFLAGS		= $(TEST_INCLUDES)
test_tirdwr_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_tirdwr_LDFLAGS		= $(USER_LDFLAGS)
test_tirdwr_LDADD		= libstreams.la libxnet.la

pkglibexec_PROGRAMS		+= test-tirdwr

## =====================================================================

test_timod_SOURCES		= src/test/test-timod.c
test_timod_CPPFLAGS		= $(TEST_INCLUDES)
test_timod_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_timod_LDFLAGS		= $(USER_LDFLAGS)
test_timod_LDADD		= libstreams.la libxnet.la

pkglibexec_PROGRAMS		+= test-timod

## =====================================================================

test_bufmod_SOURCES		= src/test/test-bufmod.c
test_bufmod_CPPFLAGS		= $(TEST_INCLUDES)
test_bufmod_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_bufmod_LDFLAGS		= $(USER_LDFLAGS)
test_bufmod_LDADD		= libstreams.la libxnet.la

pkglibexec_PROGRAMS		+= test-bufmod

## =====================================================================

test_pfmod_SOURCES		= src/test/test-pfmod.c
test_pfmod_CPPFLAGS		= $(TEST_INCLUDES)
test_pfmod_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_pfmod_LDFLAGS		= $(USER_LDFLAGS)
test_pfmod_LDADD		= libstreams.la libxnet.la

pkglibexec_PROGRAMS		+= test-pfmod

## =====================================================================

test_nbuf_SOURCES		= src/test/test-nbuf.c
test_nbuf_CPPFLAGS		= $(TEST_INCLUDES)
test_nbuf_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_nbuf_LDFLAGS		= $(USER_LDFLAGS)
test_nbuf_LDADD			= libstreams.la libxnet.la

pkglibexec_PROGRAMS		+= test-nbuf

## =====================================================================

test_pf_SOURCES			= src/test/test-pf.c
test_pf_CPPFLAGS		= $(TEST_INCLUDES)
test_pf_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
test_pf_LDFLAGS			= $(USER_LDFLAGS)
test_pf_LDADD			= libstreams.la libxnet.la

pkglibexec_PROGRAMS		+= test-pf

## =====================================================================

test_xnet_SOURCES		= src/test/test-xnet.c
test_xnet_CPPFLAGS		= $(TEST_INCLUDES)
test_xnet_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_xnet_LDFLAGS		= $(USER_LDFLAGS)
test_xnet_LDADD			= libstreams.la libxnet.la

pkglibexec_PROGRAMS		+= test-xnet

## =====================================================================

test_xnet_thr_SOURCES		= src/test/test-xnet.c
test_xnet_thr_CPPFLAGS		= $(TEST_INCLUDES) -D_REENTRANT
test_xnet_thr_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_xnet_thr_LDADD		= libstreams.la libxnet.la
test_xnet_thr_LDFLAGS		= $(USER_LDFLAGS) -lpthread

pkglibexec_PROGRAMS		+= test-xnet_thr

## =====================================================================

test_xnsl_SOURCES		= src/test/test-xnsl.c
test_xnsl_CPPFLAGS		= $(TEST_INCLUDES)
test_xnsl_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_xnsl_LDFLAGS		= $(USER_LDFLAGS)
test_xnsl_LDADD			= libstreams.la libxnet.la libxnsl.la

pkglibexec_PROGRAMS		+= test-xnsl

## =====================================================================

test_xnsl_thr_SOURCES		= src/test/test-xnsl.c
test_xnsl_thr_CPPFLAGS		= $(TEST_INCLUDES) -D_REENTRANT
test_xnsl_thr_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_xnsl_thr_LDADD		= libstreams.la libxnet.la libxnsl.la
test_xnsl_thr_LDFLAGS		= $(USER_LDFLAGS) -lpthread

pkglibexec_PROGRAMS		+= test-xnsl_thr

## =====================================================================

test_sockpath_SOURCES		= src/test/test-sockpath.c
test_sockpath_CPPFLAGS		= $(TEST_INCLUDES)
test_sockpath_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_sockpath_LDFLAGS		= $(USER_LDFLAGS)
test_sockpath_LDADD		= libstreams.la libsockpath.la

pkglibexec_PROGRAMS		+= test-sockpath

## =====================================================================

test_socket_SOURCES		= src/test/test-socket.c
test_socket_CPPFLAGS		= $(TEST_INCLUDES)
test_socket_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_socket_LDFLAGS		= $(USER_LDFLAGS)
test_socket_LDADD		= libstreams.la libsocket.la

pkglibexec_PROGRAMS		+= test-socket

## =====================================================================

test_socklib_SOURCES		= src/test/test-socklib.c
test_socklib_CPPFLAGS		= $(TEST_INCLUDES)
test_socklib_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_socklib_LDFLAGS		= $(USER_LDFLAGS)
test_socklib_LDADD		= libstreams.la libsocklib.la

pkglibexec_PROGRAMS		+= test-socklib

## =====================================================================

test_sockmod_SOURCES		= src/test/test-sockmod.c
test_sockmod_CPPFLAGS		= $(TEST_INCLUDES)
test_sockmod_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_sockmod_LDFLAGS		= $(USER_LDFLAGS)
test_sockmod_LDADD		= libstreams.la libsocklib.la

pkglibexec_PROGRAMS		+= test-sockmod

## =====================================================================

test_socksys_SOURCES		= src/test/test-socksys.c
test_socksys_CPPFLAGS		= $(TEST_INCLUDES)
test_socksys_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_socksys_LDFLAGS		= $(USER_LDFLAGS)
test_socksys_LDADD		= libstreams.la libsocket.la

pkglibexec_PROGRAMS		+= test-socksys

## =====================================================================

test_inet_raw_SOURCES		= src/test/test-inet_raw.c
test_inet_raw_CPPFLAGS		= $(TEST_INCLUDES)
test_inet_raw_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_inet_raw_LDFLAGS		= $(USER_LDFLAGS)
test_inet_raw_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-inet_raw

## =====================================================================

test_inet_udp_SOURCES		= src/test/test-inet_udp.c
test_inet_udp_CPPFLAGS		= $(TEST_INCLUDES)
test_inet_udp_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_inet_udp_LDFLAGS		= $(USER_LDFLAGS)
test_inet_udp_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-inet_udp

## =====================================================================

test_inet_tcp_SOURCES		= src/test/test-inet_tcp.c
test_inet_tcp_CPPFLAGS		= $(TEST_INCLUDES)
test_inet_tcp_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_inet_tcp_LDFLAGS		= $(USER_LDFLAGS)
test_inet_tcp_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-inet_tcp

## =====================================================================

test_inet_sctp_SOURCES		= src/test/test-inet_sctp.c
test_inet_sctp_CPPFLAGS		= $(TEST_INCLUDES)
test_inet_sctp_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_inet_sctp_LDFLAGS		= $(USER_LDFLAGS)
test_inet_sctp_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-inet_sctp

## =====================================================================

test_sctp_SOURCES		= src/test/test-sctp.c
test_sctp_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_sctp_LDFLAGS		= $(USER_LDFLAGS)
test_sctp_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-sctp

## =====================================================================

test_sctp_n_SOURCES		= src/test/test-sctp_n.c
test_sctp_n_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_n_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_sctp_n_LDFLAGS		= $(USER_LDFLAGS)
test_sctp_n_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-sctp_n

## =====================================================================

test_sctp_n2_SOURCES		= src/test/test-sctp_n2.c
test_sctp_n2_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_n2_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_sctp_n2_LDFLAGS		= $(USER_LDFLAGS)
test_sctp_n2_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-sctp_n2

## =====================================================================

test_sctp_nc_SOURCES		= src/test/test-sctp_nc.c
test_sctp_nc_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_nc_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_sctp_nc_LDFLAGS		= $(USER_LDFLAGS)
test_sctp_nc_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-sctp_nc

## =====================================================================

test_sctp_ns_SOURCES		= src/test/test-sctp_ns.c
test_sctp_ns_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_ns_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_sctp_ns_LDFLAGS		= $(USER_LDFLAGS)
test_sctp_ns_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-sctp_ns

## =====================================================================

test_sctp_t_SOURCES		= src/test/test-sctp_t.c
test_sctp_t_CPPFLAGS		= $(TEST_INCLUDES)
test_sctp_t_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_sctp_t_LDFLAGS		= $(USER_LDFLAGS)
test_sctp_t_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-sctp_t

## =====================================================================

test_etsi_n_SOURCES		= src/test/test-etsi_n.c
test_etsi_n_CPPFLAGS		= $(TEST_INCLUDES)
test_etsi_n_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_etsi_n_LDFLAGS		= $(USER_LDFLAGS)
test_etsi_n_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-etsi_n

## =====================================================================

test_etsi_t_SOURCES		= src/test/test-etsi_t.c
test_etsi_t_CPPFLAGS		= $(TEST_INCLUDES)
test_etsi_t_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_etsi_t_LDFLAGS		= $(USER_LDFLAGS)
test_etsi_t_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-etsi_t

## =====================================================================

test_interop_SOURCES		= src/test/test-interop.c
test_interop_CPPFLAGS		= $(TEST_INCLUDES)
test_interop_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_interop_LDFLAGS		= $(USER_LDFLAGS)
test_interop_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-interop

## =====================================================================

test_chan_SOURCES		= src/test/test-chan.c
test_chan_CPPFLAGS		= $(TEST_INCLUDES)
test_chan_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_chan_LDFLAGS		= $(USER_LDFLAGS)
test_chan_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-chan

## =====================================================================

test_isdn_SOURCES		= src/test/test-isdn.c
test_isdn_CPPFLAGS		= $(TEST_INCLUDES)
test_isdn_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_isdn_LDFLAGS		= $(USER_LDFLAGS)
test_isdn_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-isdn

## =====================================================================

test_atm_SOURCES		= src/test/test-atm.c
test_atm_CPPFLAGS		= $(TEST_INCLUDES)
test_atm_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
test_atm_LDFLAGS		= $(USER_LDFLAGS)
test_atm_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-atm

## =====================================================================

test_sl_SOURCES			= src/test/test-sl.c
test_sl_CPPFLAGS		= $(TEST_INCLUDES)
test_sl_CFLAGS			= $(USER_CFLAGS) $(USER_DFLAGS)
test_sl_LDFLAGS			= $(USER_LDFLAGS)
test_sl_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-sl

## =====================================================================

test_tcpc_SOURCES		= src/test/test-tcpc.c
test_tcpc_CPPFLAGS		= $(TEST_INCLUDES)
test_tcpc_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_tcpc_LDFLAGS		= $(USER_LDFLAGS)
test_tcpc_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-tcpc

## =====================================================================

test_tcps_SOURCES		= src/test/test-tcps.c
test_tcps_CPPFLAGS		= $(TEST_INCLUDES)
test_tcps_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_tcps_LDFLAGS		= $(USER_LDFLAGS)
test_tcps_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-tcps

## =====================================================================

test_udpc_SOURCES		= src/test/test-udpc.c
test_udpc_CPPFLAGS		= $(TEST_INCLUDES)
test_udpc_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_udpc_LDFLAGS		= $(USER_LDFLAGS)
test_udpc_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-udpc

## =====================================================================

test_udps_SOURCES		= src/test/test-udps.c
test_udps_CPPFLAGS		= $(TEST_INCLUDES)
test_udps_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_udps_LDFLAGS		= $(USER_LDFLAGS)
test_udps_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-udps

## =====================================================================

test_q784_SOURCES		= src/test/test-q784.c
test_q784_CPPFLAGS		= $(TEST_INCLUDES)
test_q784_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_q784_LDFLAGS		= $(USER_LDFLAGS)
test_q784_LDADD			= libstreams.la

EXTRA_PROGRAMS			+= test-q784
#pkglibexec_PROGRAMS		+= test-q784

## =====================================================================

#perftest_SOURCES		= src/test/perftest.c
#perftest_CPPFLAGS		= $(TEST_INCLUDES)
#perftest_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
#perftest_LDFLAGS		= $(USER_LDFLAGS)
#perftest_LDADD			= libstreams.la

#pkglibexec_PROGRAMS		+= perftest

## =====================================================================

test_q781_SOURCES		= src/test/test-q781.c
test_q781_CPPFLAGS		= $(TEST_INCLUDES)
test_q781_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_q781_LDFLAGS		= $(USER_LDFLAGS)
test_q781_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-q781

## =====================================================================

test_q781_pipe_SOURCES		= src/test/test-q781-pipe.c
test_q781_pipe_CPPFLAGS		= $(TEST_INCLUDES)
test_q781_pipe_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_q781_pipe_LDFLAGS		= $(USER_LDFLAGS)
test_q781_pipe_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-q781-pipe

## =====================================================================

test_x100p_SOURCES		= src/test/test-x100p.c
test_x100p_CPPFLAGS		= $(TEST_INCLUDES)
test_x100p_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_x100p_LDFLAGS		= $(USER_LDFLAGS)
test_x100p_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x100p

## =====================================================================

test_x400p_SOURCES		= src/test/test-x400p.c
test_x400p_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_x400p_LDFLAGS		= $(USER_LDFLAGS)
test_x400p_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p

## =====================================================================

test_usage_SOURCES		= src/util/usage.c
test_usage_CPPFLAGS		= $(UTIL_INCLUDES)
test_usage_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_usage_LDFLAGS		= $(USER_LDFLAGS)
test_usage_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-usage

## =====================================================================

test_x400p_cap2_SOURCES		= src/test/test-x400p-cap2.c
test_x400p_cap2_CPPFLAGS	= $(TEST_INCLUDES)
test_x400p_cap2_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_x400p_cap2_LDFLAGS		= $(USER_LDFLAGS)
test_x400p_cap2_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-cap2

## =====================================================================

test_x400p_cap_SOURCES		= src/test/test-x400p-cap.c
test_x400p_cap_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_cap_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_x400p_cap_LDFLAGS		= $(USER_LDFLAGS)
test_x400p_cap_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-cap

## =====================================================================

test_x400p_hsl_SOURCES		= src/test/test-x400p-hsl.c
test_x400p_hsl_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_hsl_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_x400p_hsl_LDFLAGS		= $(USER_LDFLAGS)
test_x400p_hsl_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-hsl

## =====================================================================

test_x400p_perf_SOURCES		= src/test/test-x400p-perf.c
test_x400p_perf_CPPFLAGS	= $(TEST_INCLUDES)
test_x400p_perf_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_x400p_perf_LDFLAGS		= $(USER_LDFLAGS)
test_x400p_perf_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-perf

## =====================================================================

test_x400p_sdl_SOURCES		= src/test/test-x400p-sdl.c
test_x400p_sdl_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_sdl_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_x400p_sdl_LDFLAGS		= $(USER_LDFLAGS)
test_x400p_sdl_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-sdl

## =====================================================================

test_x400p_sdt2_SOURCES		= src/test/test-x400p-sdt2.c
test_x400p_sdt2_CPPFLAGS	= $(TEST_INCLUDES)
test_x400p_sdt2_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_x400p_sdt2_LDFLAGS		= $(USER_LDFLAGS)
test_x400p_sdt2_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-sdt2

## =====================================================================

test_x400p_sdt_SOURCES		= src/test/test-x400p-sdt.c
test_x400p_sdt_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_sdt_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_x400p_sdt_LDFLAGS		= $(USER_LDFLAGS)
test_x400p_sdt_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-sdt

## =====================================================================

test_x400p_sl_SOURCES		= src/test/test-x400p-sl.c
test_x400p_sl_CPPFLAGS		= $(TEST_INCLUDES)
test_x400p_sl_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_x400p_sl_LDFLAGS		= $(USER_LDFLAGS)
test_x400p_sl_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-x400p-sl

## =====================================================================

test_sigtran_SOURCES		= src/test/test-sigtran.c
test_sigtran_CPPFLAGS		= $(TEST_INCLUDES)
test_sigtran_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_sigtran_LDFLAGS		= $(USER_LDFLAGS)
test_sigtran_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-sigtran

## =====================================================================

test_m2pa_SOURCES		= src/test/test-m2pa.c
test_m2pa_CPPFLAGS		= $(TEST_INCLUDES)
test_m2pa_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_m2pa_LDFLAGS		= $(USER_LDFLAGS)
test_m2pa_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-m2pa

## =====================================================================

test_m2ua_as_SOURCES		= src/test/test-m2ua_as.c
test_m2ua_as_CPPFLAGS		= $(TEST_INCLUDES)
test_m2ua_as_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_m2ua_as_LDFLAGS		= $(USER_LDFLAGS)
test_m2ua_as_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-m2ua_as

## =====================================================================

test_m3ua_raw_SOURCES		= src/test/test-m3ua-raw.c
test_m3ua_raw_CPPFLAGS		= $(TEST_INCLUDES)
test_m3ua_raw_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_m3ua_raw_LDFLAGS		= $(USER_LDFLAGS)
test_m3ua_raw_LDADD		= libstreams.la

pkglibexec_PROGRAMS		+= test-m3ua-raw

## =====================================================================

test_voip_SOURCES		= src/test/test-voip.c
test_voip_CPPFLAGS		= $(TEST_INCLUDES)
test_voip_CFLAGS		= $(USER_CFLAGS) $(USER_DFLAGS)
test_voip_LDFLAGS		= $(USER_LDFLAGS)
test_voip_LDADD			= libstreams.la

pkglibexec_PROGRAMS		+= test-voip

## =====================================================================

## PKG_BUILD_ARCH
endif
## PKG_BUILD_USER
endif

EXPORT_SRCFILES			=

if CONFIG_STREAMS_SEPARATE_COMPILE
## CONFIG_STREAMS_SEPARATE_COMPILE

EXPORT_SRCFILES			+= src/kernel/strattach.c \
				   src/kernel/strlookup.c \
				   src/kernel/strpipe.c \
				   src/kernel/strreg.c \
				   src/kernel/strsad.c \
				   src/kernel/strsched.c \
				   src/kernel/strspecfs.c \
				   src/kernel/strsysctl.c \
				   src/kernel/strutil.c \
				   src/kernel/strsyms.c

## CONFIG_STREAMS_SEPARATE_COMPILE
else
## !CONFIG_STREAMS_SEPARATE_COMPILE

EXPORT_SRCFILES			+= src/kernel/specfs.c \
				   src/kernel/streams.c

## !CONFIG_STREAMS_SEPARATE_COMPILE
endif

EXPORT_SRCFILES			+= src/drivers/nit_if.c \
				   src/drivers/bpf.c \
				   src/drivers/clone.c \
				   src/modules/sth.c \
				   src/kernel/aixcompat.c \
				   src/kernel/hpuxcompat.c \
				   src/kernel/irixcompat.c \
				   src/kernel/maccompat.c \
				   src/kernel/mpscompat.c \
				   src/kernel/os7compat.c \
				   src/kernel/osfcompat.c \
				   src/kernel/suncompat.c \
				   src/kernel/svr3compat.c \
				   src/kernel/svr4compat.c \
				   src/kernel/uw7compat.c \
				   src/kernel/wrapper.c

EXPORT_VERFILES			=

if CONFIG_STREAMS_SEPARATE_COMPILE
## CONFIG_STREAMS_SEPARATE_COMPILE

EXPORT_VERFILES			+= $(PKGINCL)/versions/strattach.ver \
				   $(PKGINCL)/versions/strlookup.ver \
				   $(PKGINCL)/versions/strpipe.ver \
				   $(PKGINCL)/versions/strreg.ver \
				   $(PKGINCL)/versions/strsad.ver \
				   $(PKGINCL)/versions/strsched.ver \
				   $(PKGINCL)/versions/strspecfs.ver \
				   $(PKGINCL)/versions/strsysctl.ver \
				   $(PKGINCL)/versions/strutil.ver \
				   $(PKGINCL)/versions/strsyms.ver
## CONFIG_STREAMS_SEPARATE_COMPILE
else
## CONFIG_STREAMS_SEPARATE_COMPILE

EXPORT_VERFILES			+= $(PKGINCL)/versions/specfs.ver \
				   $(PKGINCL)/versions/streams.ver

## CONFIG_STREAMS_SEPARATE_COMPILE
endif

EXPORT_VERFILES			+= $(PKGINCL)/versions/nit_if.ver \
				   $(PKGINCL)/versions/bpf.ver \
				   $(PKGINCL)/versions/clone.ver \
				   $(PKGINCL)/versions/sth.ver \
				   $(PKGINCL)/versions/aixcompat.ver \
				   $(PKGINCL)/versions/hpuxcompat.ver \
				   $(PKGINCL)/versions/irixcompat.ver \
				   $(PKGINCL)/versions/maccompat.ver \
				   $(PKGINCL)/versions/mpscompat.ver \
				   $(PKGINCL)/versions/os7compat.ver \
				   $(PKGINCL)/versions/osfcompat.ver \
				   $(PKGINCL)/versions/suncompat.ver \
				   $(PKGINCL)/versions/svr3compat.ver \
				   $(PKGINCL)/versions/svr4compat.ver \
				   $(PKGINCL)/versions/uw7compat.ver \
				   $(PKGINCL)/versions/wrapper.ver

GENKSYMS_CPPFLAGS		= -I./src/kernel -I$(srcdir)/src/kernel \
				  -I./src/drivers -I$(srcdir)/src/drivers \
				  -I./src/modules -I$(srcdir)/src/modules \
				  $(PKG_INCLUDES) $(KERNEL_CPPFLAGS) $(KERNEL_CFLAGS)

EXTRA_DIST			+= $(top_srcdir)/scripts/hot.lds

##
## Rules for generating versioned kernel symbols
##
include am/genksyms.am

##
## Rules for generating STREAMS configuration
##
include am/strconf.am

#INET_MKDEV			= scripts/inet_mkdev
#INET_RMDEV			= scripts/inet_rmdev
#
#if PKG_BUILD_INDEP
### these go with the base package
#dist_sbin_SCRIPTS 		+= $(INET_MKDEV) $(INET_RMDEV)
#endif
#
#AM_INSTALLCHECK_STD_OPTIONS_EXEMPT += $(INET_MKDEV) $(INET_RMDEV)
#EXTRA_SCRIPTS += $(INET_MKDEV) $(INET_RMDEV)
#
#if PKG_BUILD_KERNEL
### PKG_BUILD_KERNEL
#
#install-inet-devices: $(INET_MKDEV)
#	@if test -r $(DESTDIR)$(rootdir)/etc/modules.conf ; then \
#		echo 'DESTDIR=$(DESTDIR) $(srcdir)/$(INET_MKDEV)' ; \
#		DESTDIR=$(DESTDIR) $(srcdir)/$(INET_MKDEV) ; \
#	fi
#
#uninstall-inet-devices: $(INET_RMDEV)
#	@if test -r $(DESTDIR)$(rootdir)/etc/modules.conf ; then \
#		echo 'DESTDIR=$(DESTDIR) $(srcdir)/$(INET_RMDEV)' ; \
#		DESTDIR=$(DESTDIR) $(srcdir)/$(INET_RMDEV) ; \
#	fi
#
### PKG_BUILD_KERNEL
#else
### ! PKG_BUILD_KERNEL
#
#install-inet-devices:
#
#uninstall-inet-devices:
#
### ! PKG_BUILD_KERNEL
#endif
#
#INSTALL_EXEC_HOOK += install-inet-devices
#
#UNINSTALL_HOOK += uninstall-inet-devices
#
#.PHONY: install-inet-devices uninstall-inet-devices

if PKG_BUILD_USER
## PKG_BUILD_USER

SUBDIRS = src

if PKG_BUILD_ARCH
SUBDIRS += po
CLEANFILES += po/stamp-po
endif

SUBDIRS += .

if WITH_32BIT_LIBS
SUBDIRS += lib32
endif

if WITH_PERL
SUBDIRS += src/perl
endif

if WITH_TCL
SUBDIRS += src/tcl
endif

if WITH_JAVA
SUBDIRS += src/java
endif

SUBDIRS += tests

if DEVELOPMENT
SUBDIRS += doc
endif

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

if INSTALL_INITSCRIPTS
## INSTALL_INITSCRIPTS

if INSTALL_SYSVINIT
## INSTALL_SYSVINIT

if WITH_RCSD_DIRECTORY
## WITH_RCSD_DIRECTORY

dist_initrd_SCRIPTS		= src/util/init/specfs.sh \
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

dist_initrd_SCRIPTS		= src/util/init/specfs \
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

dist_sysconf_DATA		+= src/util/sysconfig/streams.conf

if CONFIG_STREAMS_UTILS
dist_sysconf_DATA		+= strsetup.conf \
				   strload.conf
endif

dist_config_DATA		= src/util/sysconfig/specfs \
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

## INSTALL_SYSVINIT
endif

if INSTALL_SYSTEMD
## INSTALL_SYSTEMD

dist_config_DATA		= src/util/sysconfig/specfs \
				  src/util/sysconfig/streams \
				  src/util/sysconfig/@PACKAGE_LCNAME@

dist_systemd_SCRIPTS		= src/util/init/specfs \
				  src/util/init/streams \
				  src/util/init/@PACKAGE_LCNAME@

dist_systemdsys_DATA		= src/util/systemd/dev-streams.mount \
				  src/util/systemd/specfs.service \
				  src/util/systemd/streams.service \
				  src/util/systemd/streams.target \
				  src/util/systemd/openss7.service \
				  src/util/systemd/streams-shutdown.target

if CONFIG_STREAMS_LOG
dist_systemdsys_DATA		+= src/util/systemd/strace.service \
				   src/util/systemd/strerr.service
endif
if CONFIG_STREAMS_LOG_MODULE
dist_systemdsys_DATA		+= src/util/systemd/strace.service \
				   src/util/systemd/strerr.service
endif

dist_sysctld_DATA		= src/util/sysconfig/streams.conf

dist_modulesloadd_DATA		= src/util/systemd/streams.conf

## INSTALL_SYSTEMD
endif

## INSTALL_INITSCRIPTS
endif

if WITH_UPDATE_CA_CERTS
#dist_lcacert_DATA		= scripts/OPENSS7-CA-CERT
else
#dist_sslcert_DATA		= scripts/OPENSS7-CA-CERT
endif

dist_snmpmib_DATA		= 

if WITH_SNMP
## WITH_SNMP
if WITH_SNMP_AGENT
## WITH_SNMP_AGENT
dist_snmpmib_DATA		+= src/mibs/ALARM-MIB.mib \
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
## WITH_SNMP
endif

## PKG_BUILD_INDEP
endif

## PKG_BUILD_USER
endif

##
## Rules for init scripts
##
include am/init.am

##
## Rules for CA certs
##
include am/ssl.am

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
export RPMXOPTS="--nodeps"
export DEBXOPTS="-d"

AM_RPMFLAGS			= --define "_kversion $(kversion)"
AM_SRPMFLAGS			= $(AM_RPMFLAGS)

include am/rpm.am

##
## Rules for making deb packages
##
include am/deb.am

##
## Rules for making arch packages
##
include am/pkg.am

##
## Rules for repo install sources
##
include am/repo.am

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

EXTRA_DIST			+= src/python/yum-plugins/fedorakmod.py \
				   src/python/yum-plugins/fedorakmod.conf

if PKG_BUILD_INDEP
##
## Some notes on things
##
dist_doc_DATA			+= NOTES \
				   NOTES.cos53 \
				   NOTES.kmod \
				   NOTES.kmp \
				   NOTES.ksyms \
				   NOTES.repo \
				   NOTES.rpm \
				   NOTES.sles
##
## Some files that are part of a GNU distribution.
##
dist_doc_DATA			+= ABOUT-NLS \
				   COPYING \
				   ChangeLog \
				   INSTALL
endif

##
## Build cache files for intelligent warnings.
##
EXTRA_DIST			+= cache/arch-os-i686-config.cache \
				   cache/arch-os-x86_64-config.cache \
				   cache/centos-5.5-x86_64-config.cache \
				   cache/centos-5-x86_64-config.cache \
				   cache/centos-6-x86_64-config.cache \
				   cache/debian-lenny-i386-config.cache \
				   cache/debian-lenny-i686-config.cache \
				   cache/debian-squeeze-amd64-config.cache \
				   cache/debian-squeeze-i386-config.cache \
				   cache/debian-squeeze-i686-config.cache \
				   cache/debian-squeeze-x86_64-config.cache \
				   cache/debian-wheezy-i386-config.cache \
				   cache/fedora-15-x86_64-config.cache \
				   cache/mageia-1-x86_64-config.cache \
				   cache/mandriva-2010.2-i586-config.cache \
				   cache/mandriva-2010.2-i686-config.cache \
				   cache/mandriva-2011.0-x86_64-config.cache \
				   cache/mes-5.2-x86_64-config.cache \
				   cache/openSUSE-11.4-x86_64-config.cache \
				   cache/oracle-6-x86_64-config.cache \
				   cache/puias-6-x86_64-config.cache \
				   cache/redhat-6.0-x86_64-config.cache \
				   cache/redhat-7.2-i686-config.cache \
				   cache/rhel-6-x86_64-config.cache \
				   cache/salix-13.37-i686-config.cache \
				   cache/scientific-5-x86_64-config.cache \
				   cache/scientific-6-x86_64-config.cache \
				   cache/slackware-13.37-x86_64-config.cache \
				   cache/sle-11-x86_64-config.cache \
				   cache/suse-11-x86_64-config.cache \
				   cache/ubuntu-lucid-amd64-config.cache \
				   cache/ubuntu-lucid-x86_64-config.cache

MAINTAINERCLEANFILES		+= $(top_srcdir)/aclocal.m4 \
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
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
