# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.23 $) $Date: 2004/04/22 12:08:30 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; either version 2 of the License, or (at your option) any later
# version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc., 675 Mass
# Ave, Cambridge, MA 02139, USA.
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
# Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
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
# Last Modified $Date: 2004/04/22 12:08:30 $ by $Author: brian $
#
# =============================================================================

bin_PROGRAMS			= \
				strchg \
				strconf \
				strreset

sbin_PROGRAMS			= \
				autopush \
				fattach \
				fdetach \
				insf \
				scls \
				strace \
				strerr \
				strinfo \
				strvf \
				strmakenodes

sbin_SCRIPTS			= \
				$(srcdir)/src/util/strclean \
				$(srcdir)/src/util/strload \
				$(srcdir)/src/util/strsetup

libexec_PROGRAMS		= \
				test-inet_raw \
				test-inet_tcp \
				test-inet_udp \
				test-timod \
				test-tirdwr \
				test-xnet \
				test-xnet_thr

#LFS_EXTRA_TESTS		= 
#LFS_EXTRA_TOOLS		= strmakenodes

LDADD				= libstreams.la

lib_LTLIBRARIES			= libstreams.la libxnet.la

noinst_LIBRARIES		= \
				libLfSkernel.a \
				libLfSdrivers.a \
				libLfSmodules.a \
				libLfScompat.a

libLfSkernel_a_SOURCES		= \
				src/kernel/strattach.c \
				src/kernel/strattach.h \
				src/kernel/strhead.c \
				src/kernel/strhead.h \
				src/kernel/strmain.c \
				src/kernel/strprocfs.c \
				src/kernel/strprocfs.h \
				src/kernel/strsad.c \
				src/kernel/strsad.h \
				src/kernel/strsched.c \
				src/kernel/strsched.h \
				src/kernel/strspecfs.c \
				src/kernel/strspecfs.h \
				src/kernel/strsyms.c \
				src/kernel/strsysctl.c \
				src/kernel/strsysctl.h \
				src/kernel/strutil.c \
				src/kernel/strutil.h \
				src/kernel/strreg.c \
				src/kernel/strreg.h \
				src/kernel/strdebug.h \
				$(top_builddir)/$(LFS_SC_CONFIG)

libLfSkernel_a_DEPENDENCIES	= $(LFS_SC_CONFIG) $(LFS_SC_MODCONF)
libLfSkernel_a_CPPFLAGS		= \
				-I./src/kernel -I@srcdir@/src/kernel \
				-I./src/drivers -I@srcdir@/src/drivers \
				-I./src/modules -I@srcdir@/src/modules \
				@LFS_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libLfSkernel_a_CFLAGS		= @KERNEL_CFLAGS@

libLfSdrivers_a_SOURCES		= \
				src/drivers/echo.c \
				src/drivers/nuls.c \
				src/drivers/sad.c \
				include/sys/sad.h \
				src/drivers/strclone.c \
				src/drivers/strfifo.c \
				src/drivers/strfifo.h \
				src/drivers/strnsdev.c \
				src/drivers/strpipe.c \
				src/drivers/strpipe.h \
				src/drivers/strsock.c \
				src/drivers/strsock.h \
				src/drivers/strspx.c \
				src/drivers/strsfx.c \
				include/log.h \
				include/sys/log.h \
				include/strlog.h \
				include/sys/strlog.h \
				src/drivers/log.h \
				src/drivers/log.c \
				$(top_builddir)/$(LFS_SC_CONFIG)
#				src/drivers/strinet.c
#				src/drivers/strsocksys.c
#				src/drivers/strunix.c
#				src/drivers/strxnet.c


libLfSdrivers_a_DEPENDENCIES	= $(LFS_SC_CONFIG) $(LFS_SC_MODCONF)
libLfSdrivers_a_CPPFLAGS	= \
				-I./src/kernel -I@srcdir@/src/kernel \
				-I./src/drivers -I@srcdir@/src/drivers \
				-I./src/modules -I@srcdir@/src/modules \
				@LFS_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@ -DEXPORT_SYMTAB
libLfSdrivers_a_CFLAGS		= @KERNEL_CFLAGS@

libLfSmodules_a_SOURCES		= \
				src/modules/connld.c \
				src/modules/pipemod.c \
				src/modules/sockmod.c \
				src/modules/timod.c \
				include/timod.h \
				include/sys/timod.h \
				src/modules/tirdwr.c \
				include/tihdr.h \
				include/sys/tihdr.h \
				src/modules/sc.c \
				include/sys/sc.h \
				$(top_builddir)/$(LFS_SC_CONFIG)


libLfSmodules_a_DEPENDENCIES	= $(LFS_SC_CONFIG) $(LFS_SC_MODCONF)
libLfSmodules_a_CPPFLAGS	= \
				-I./src/kernel -I@srcdir@/src/kernel \
				-I./src/drivers -I@srcdir@/src/drivers \
				-I./src/modules -I@srcdir@/src/modules \
				@LFS_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libLfSmodules_a_CFLAGS		= @KERNEL_CFLAGS@

libLfScompat_a_SOURCES		= \
				src/compat/aix/aixcompat.c \
				src/compat/hpux/hpuxcompat.c \
				src/compat/lis/liscompat.c \
				src/compat/osf/osfcompat.c \
				src/compat/sun/suncompat.c \
				src/compat/svr4/svr4compat.c \
				src/compat/uw7/uw7compat.c \
				$(top_builddir)/$(LFS_SC_CONFIG)

libLfScompat_a_DEPENDENCIES	= $(LFS_SC_CONFIG) $(LFS_SC_MODCONF)
libLfScompat_a_CPPFLAGS		= \
				-I./src/compat -I@srcdir@/src/compat \
				-I./src/kernel -I@srcdir@/src/kernel \
				-I./src/drivers -I@srcdir@/src/drivers \
				-I./src/modules -I@srcdir@/src/modules \
				@LFS_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@ -DEXPORT_SYMTAB
libLfScompat_a_CFLAGS		= @KERNEL_CFLAGS@

streams.o: $(libLfSkernel_a_OBJECTS)
	$(LD) -r -o $@ $^

include am/libraries.am

KERNEL_MODULES			= streams.o $(libLfSdrivers_a_OBJECTS) $(libLfSmodules_a_OBJECTS) $(libLfScompat_a_OBJECTS)
KERNEL_MODCONF			= $(LFS_SC_CONFMOD)
KERNEL_SUBDIR			= streams
KMODCONF_EXT			= streams

include am/kernel.am

libstreams_la_SOURCES		= \
				lib/libc/fattach.c \
				lib/libc/fdetach.c \
				lib/libc/getmsg.c \
				lib/libc/getpmsg.c \
				lib/libc/isastream.c \
				lib/libc/pipe.c \
				lib/libc/putmsg.c \
				lib/libc/putpmsg.c
libstreams_la_LDFLAGS		= -no-undefined -version-info 0:0:0
libstreams_la_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
libstreams_la_CFLAGS		= @USER_CFLAGS@

libxnet_la_SOURCES		= lib/libxnet/xnet.c gettext.h
libxnet_la_LDFLAGS		= -version-info 0:0:0
libxnet_la_CPPFLAGS		= -I$(top_srcdir)/lib/libxnet -I$(top_builddir)/lib/libxnet @LFS_INCLUDES@ @USER_CPPFLAGS@ -I$(top_srcdir)/include/xti -I$(top_builddir)/include/xti
libxnet_la_CFLAGS		= @USER_CFLAGS@
libxnet_la_LIBADD		= libstreams.la

EXTRA_PROGRAMS			= strmakenodes

#bin_PROGRAMS			= timetst strtst $(LFS_EXTRA_TESTS)
#sbin_PROGRAMS			= streams $(LFS_EXTRA_TOOLS)

strchg_SOURCES			= src/util/strchg.c $(top_builddir)/include/sys/config.h
strchg_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
strchg_LDADD			= libstreams.la
strchg_CPPFLAGS			= @LFS_INCLUDES@ @USER_CPPFLAGS@
strchg_CFLAGS			= @USER_CFLAGS@

strconf_SOURCES			= src/util/strconf.c $(top_builddir)/include/sys/config.h
strconf_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
strconf_LDADD			= libstreams.la
strconf_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
strconf_CFLAGS			= @USER_CFLAGS@

strreset_SOURCES		= src/util/strreset.c $(top_builddir)/include/sys/config.h
strreset_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
strreset_LDADD			= libstreams.la
strreset_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
strreset_CFLAGS			= @USER_CFLAGS@

autopush_SOURCES		= src/util/autopush.c $(top_builddir)/include/sys/config.h
autopush_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
autopush_LDFLAGS		= -static libstreams.la
autopush_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
autopush_CFLAGS			= @USER_CFLAGS@

fattach_SOURCES			= src/util/fattach.c $(top_builddir)/include/sys/config.h
fattach_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
fattach_LDFLAGS			= -static libstreams.la
fattach_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
fattach_CFLAGS			= @USER_CFLAGS@

fdetach_SOURCES			= src/util/fdetach.c $(top_builddir)/include/sys/config.h
fdetach_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
fdetach_LDFLAGS			= -static libstreams.la
fdetach_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
fdetach_CFLAGS			= @USER_CFLAGS@

insf_SOURCES			= src/util/insf.c $(top_builddir)/include/sys/config.h
insf_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
insf_LDFLAGS			= -static libstreams.la
insf_CPPFLAGS			= @LFS_INCLUDES@ @USER_CPPFLAGS@
insf_CFLAGS			= @USER_CFLAGS@

scls_SOURCES			= src/util/scls.c $(top_builddir)/include/sys/config.h
scls_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
scls_LDFLAGS			= -static libstreams.la
scls_CPPFLAGS			= @LFS_INCLUDES@ @USER_CPPFLAGS@
scls_CFLAGS			= @USER_CFLAGS@

strace_SOURCES			= src/util/strace.c $(top_builddir)/include/sys/config.h
strace_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
strace_LDFLAGS			= -static libstreams.la
strace_CPPFLAGS			= @LFS_INCLUDES@ @USER_CPPFLAGS@
strace_CFLAGS			= @USER_CFLAGS@

#strclean_SOURCES		= src/util/strclean.c $(top_builddir)/include/sys/config.h
#strclean_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
#strclean_LDFLAGS		= -static libstreams.la
#strclean_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
#strclean_CFLAGS			= @USER_CFLAGS@

strerr_SOURCES			= src/util/strerr.c $(top_builddir)/include/sys/config.h
strerr_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
strerr_LDFLAGS			= -static libstreams.la
strerr_CPPFLAGS			= @LFS_INCLUDES@ @USER_CPPFLAGS@
strerr_CFLAGS			= @USER_CFLAGS@

strinfo_SOURCES			= src/util/strinfo.c $(top_builddir)/include/sys/config.h
strinfo_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
strinfo_LDFLAGS			= -static libstreams.la
strinfo_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
strinfo_CFLAGS			= @USER_CFLAGS@

#strload_SOURCES		= src/util/strload.c $(top_builddir)/include/sys/config.h
#strload_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
#strload_LDFLAGS		= -static libstreams.la
#strload_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
#strload_CFLAGS			= @USER_CFLAGS@

#strsetup_SOURCES		= src/util/strsetup.c $(top_builddir)/include/sys/config.h
#strsetup_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
#strsetup_LDFLAGS		= -static libstreams.la
#strsetup_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
#strsetup_CFLAGS		= @USER_CFLAGS@

strvf_SOURCES			= src/util/strvf.c $(top_builddir)/include/sys/config.h
strvf_DEPENDENCIES		= $(top_builddir)/include/sys/config.h
strvf_LDFLAGS			= -static libstreams.la
strvf_CPPFLAGS			= @LFS_INCLUDES@ @USER_CPPFLAGS@
strvf_CFLAGS			= @USER_CFLAGS@

strmakenodes_SOURCES		= src/util/strmakenodes.c $(top_builddir)/include/sys/config.h
strmakenodes_DEPENDENCIES	= $(top_builddir)/include/sys/config.h
strmakenodes_LDADD		=
strmakenodes_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
strmakenodes_CFLAGS		= @USER_CFLAGS@

test_inet_raw_SOURCES		= src/test/test-inet_raw.c
test_inet_raw_LDFLAGS		= -lstreams
test_inet_raw_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
test_inet_raw_CFLAGS		= @USER_CFLAGS@

test_inet_udp_SOURCES		= src/test/test-inet_udp.c
test_inet_udp_LDFLAGS		= -lstreams
test_inet_udp_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
test_inet_udp_CFLAGS		= @USER_CFLAGS@

test_inet_tcp_SOURCES		= src/test/test-inet_tcp.c
test_inet_tcp_LDFLAGS		= -lstreams
test_inet_tcp_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
test_inet_tcp_CFLAGS		= @USER_CFLAGS@

test_timod_SOURCES		= src/test/test-timod.c
test_timod_LDFLAGS		= -lstreams
test_timod_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
test_timod_CFLAGS		= @USER_CFLAGS@

test_tirdwr_SOURCES		= src/test/test-tirdwr.c
test_tirdwr_LDFLAGS		= -lstreams
test_tirdwr_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
test_tirdwr_CFLAGS		= @USER_CFLAGS@

test_xnet_SOURCES		= src/test/test-xnet.c
test_xnet_LDFLAGS		= -lstreams -lxnet
test_xnet_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
test_xnet_CFLAGS		= @USER_CFLAGS@

test_xnet_thr_SOURCES		= src/test/test-xnet.c
test_xnet_thr_LDFLAGS		= -lstreams -lpthread -lxnet
test_xnet_thr_CPPFLAGS		= @LFS_INCLUDES@ @USER_CPPFLAGS@
test_xnet_thr_CFLAGS		= @USER_CFLAGS@

INET_MKDEV			= @auxdir@/inet_mkdev
INET_RMDEV			= @auxdir@/inet_rmdev
STRCONF_SCRIPT			= @auxdir@/strconf-sh
STRCONF				= $(SHELL) $(STRCONF_SCRIPT)
STRMAKENODES			= ./strmakenodes

@LFS_SC_CONFIG@: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --hconfig=@LFS_SC_CONFIG@ -b@LFS_SC_MAJBASE@ Config.master

@LFS_SC_MODCONF@: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --modconf=@LFS_SC_MODCONF@ -b@LFS_SC_MAJBASE@ Config.master

@LFS_SC_DRVCONF@: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --driverconf=@LFS_SC_DRVCONF@ -b@LFS_SC_MAJBASE@ Config.master

@LFS_SC_CONFMOD@: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --confmodules=@LFS_SC_CONFMOD@ -b@LFS_SC_MAJBASE@ Config.master

@LFS_SC_MKNODES@: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --makenodes=@LFS_SC_MKNODES@ -b@LFS_SC_MAJBASE@ Config.master

@LFS_SC_STSETUP@: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --strsetup=@LFS_SC_STSETUP@ -b@LFS_SC_MAJBASE@ Config.master

@LFS_SC_STRLOAD@: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --strload=@LFS_SC_STRLOAD@ -b@LFS_SC_MAJBASE@ Config.master

Config.master: $(lfs_configs)
	@if test -n "$(lfs_configs)"; then \
		echo "cat $(lfs_configs) > Config.master < /dev/null"; \
		cat $(lfs_configs) > Config.master < /dev/null; \
	else exit 1; fi;

MANMACRO_PFX			= streams
mansrcdir			= $(srcdir)/doc/man

nodist_man_MANS = \
	doc/man/man1/strchg.1 \
	doc/man/man1/strconf.1 \
	doc/man/man1/strreset.1 \
	doc/man/man2/fattach.2 \
	doc/man/man2/fcntl.2s \
	doc/man/man2/getmsg.2 \
	doc/man/man2/getpmsg.2 \
	doc/man/man2/pipe.2s \
	doc/man/man2/poll.2s \
	doc/man/man2/putmsg.2 \
	doc/man/man2/putpmsg.2 \
	doc/man/man2/read.2s \
	doc/man/man2/readv.2s \
	doc/man/man2/soon.2 \
	doc/man/man2/write.2s \
	doc/man/man2/writev.2s \
	doc/man/man3/accept.3 \
	doc/man/man3/bind.3 \
	doc/man/man3/connect.3 \
	doc/man/man3/fattach.3 \
	doc/man/man3/fdetach.3 \
	doc/man/man3/getpeername.3 \
	doc/man/man3/getsockname.3 \
	doc/man/man3/getsockopt.3 \
	doc/man/man3/isastream.3 \
	doc/man/man3/libsocket.3 \
	doc/man/man3/listen.3 \
	doc/man/man3/netbuf.3 \
	doc/man/man3/pipe.3 \
	doc/man/man3/recv.3 \
	doc/man/man3/recvfrom.3 \
	doc/man/man3/recvmsg.3 \
	doc/man/man3/send.3 \
	doc/man/man3/sendmsg.3 \
	doc/man/man3/sendto.3 \
	doc/man/man3/setsockopt.3 \
	doc/man/man3/shutdown.3 \
	doc/man/man3/socket.3 \
	doc/man/man3/socketpair.3 \
	doc/man/man3/sockets.3 \
	doc/man/man3/s_pipe.3 \
	doc/man/man3/t_accept.3 \
	doc/man/man3/t_addleaf.3 \
	doc/man/man3/t_alloc.3 \
	doc/man/man3/t_bind.3 \
	doc/man/man3/t_close.3 \
	doc/man/man3/t_connect.3 \
	doc/man/man3/t_errno.3 \
	doc/man/man3/t_error.3 \
	doc/man/man3/t_free.3 \
	doc/man/man3/t_getinfo.3 \
	doc/man/man3/t_getprotaddr.3 \
	doc/man/man3/t_getstate.3 \
	doc/man/man3/t_listen.3 \
	doc/man/man3/t_look.3 \
	doc/man/man3/t_open.3 \
	doc/man/man3/t_optmgmt.3 \
	doc/man/man3/t_rcv.3 \
	doc/man/man3/t_rcvconnect.3 \
	doc/man/man3/t_rcvdis.3 \
	doc/man/man3/t_rcvleafchange.3 \
	doc/man/man3/t_rcvrel.3 \
	doc/man/man3/t_rcvreldata.3 \
	doc/man/man3/t_rcvudata.3 \
	doc/man/man3/t_rcvuderr.3 \
	doc/man/man3/t_rcvv.3 \
	doc/man/man3/t_rcvvudata.3 \
	doc/man/man3/t_removeleaf.3 \
	doc/man/man3/t_snd.3 \
	doc/man/man3/t_snddis.3 \
	doc/man/man3/t_sndopt.3 \
	doc/man/man3/t_sndrel.3 \
	doc/man/man3/t_sndreldata.3 \
	doc/man/man3/t_sndudata.3 \
	doc/man/man3/t_sndv.3 \
	doc/man/man3/t_sndvopt.3 \
	doc/man/man3/t_sndvudata.3 \
	doc/man/man3/t_strerror.3 \
	doc/man/man3/t_sync.3 \
	doc/man/man3/t_sysconf.3 \
	doc/man/man3/t_unbind.3 \
	doc/man/man3/xnet.3 \
	doc/man/man3/xti.3 \
	doc/man/man3/xti_ip.3 \
	doc/man/man3/xti_sctp.3 \
	doc/man/man3/xti_tcp.3 \
	doc/man/man3/xti_udp.3 \
	doc/man/man4/clone.4 \
	doc/man/man4/connld.4 \
	doc/man/man4/connld.old.4 \
	doc/man/man4/echo.4 \
	doc/man/man4/egp.4 \
	doc/man/man4/eth.4 \
	doc/man/man4/fifo.4 \
	doc/man/man4/fifo.4s \
	doc/man/man4/fifo.old.4 \
	doc/man/man4/ggp.4 \
	doc/man/man4/icmp.4 \
	doc/man/man4/idp.4 \
	doc/man/man4/ifnet.4 \
	doc/man/man4/INET.4 \
	doc/man/man4/ip.4 \
	doc/man/man4/ipip.4 \
	doc/man/man4/nsdev.4 \
	doc/man/man4/nuls.4 \
	doc/man/man4/pipe.4 \
	doc/man/man4/pipemod.4 \
	doc/man/man4/pipemod.old.4 \
	doc/man/man4/pup.4 \
	doc/man/man4/rawip.4 \
	doc/man/man4/sad.4 \
	doc/man/man4/sc.4 \
	doc/man/man4/s_fifo.4 \
	doc/man/man4/sfx.4 \
	doc/man/man4/sockmod.4 \
	doc/man/man4/socksys.4 \
	doc/man/man4/spx.4 \
	doc/man/man4/strlog.4 \
	doc/man/man4/tcp.4 \
	doc/man/man4/ticlts.4 \
	doc/man/man4/ticots.4 \
	doc/man/man4/ticotsord.4 \
	doc/man/man4/timod.4 \
	doc/man/man4/tirdwr.4 \
	doc/man/man4/tlclts.4 \
	doc/man/man4/tlcots.4 \
	doc/man/man4/tlcotsod.4 \
	doc/man/man4/udp.4 \
	doc/man/man5/autopush.5 \
	doc/man/man5/specfs.5 \
	doc/man/man5/strsetup.conf.5 \
	doc/man/man7/CD_ABORT_OUTPUT_REQ.7 \
	doc/man/man7/cd_abort_output_req_t.7 \
	doc/man/man7/CD_ALLOW_INPUT_REQ.7 \
	doc/man/man7/cd_allow_input_req_t.7 \
	doc/man/man7/CD_ATTACH_REQ.7 \
	doc/man/man7/cd_attach_req_t.7 \
	doc/man/man7/CD_BAD_FRAME_IND.7 \
	doc/man/man7/cd_bad_frame_ind_t.7 \
	doc/man/man7/CD_DETACH_REQ.7 \
	doc/man/man7/cd_detach_req_t.7 \
	doc/man/man7/CD_DISABLE_CON.7 \
	doc/man/man7/cd_disable_con_t.7 \
	doc/man/man7/CD_DISABLE_REQ.7 \
	doc/man/man7/cd_disable_req_t.7 \
	doc/man/man7/CD_ENABLE_CON.7 \
	doc/man/man7/cd_enable_con_t.7 \
	doc/man/man7/CD_ENABLE_REQ.7 \
	doc/man/man7/cd_enable_req_t.7 \
	doc/man/man7/CD_ERROR_ACK.7 \
	doc/man/man7/cd_error_ack_t.7 \
	doc/man/man7/CD_ERROR_IND.7 \
	doc/man/man7/cd_error_ind_t.7 \
	doc/man/man7/CD_HALT_INPUT_REQ.7 \
	doc/man/man7/cd_halt_input_req_t.7 \
	doc/man/man7/CD_INFO_ACK.7 \
	doc/man/man7/cd_info_ack_t.7 \
	doc/man/man7/CD_INFO_REQ.7 \
	doc/man/man7/CD_MODEM_SIG_IND.7 \
	doc/man/man7/cd_modem_sig_ind_t.7 \
	doc/man/man7/CD_MODEM_SIG_POLL.7 \
	doc/man/man7/cd_modem_sig_poll_t.7 \
	doc/man/man7/CD_MODEM_SIG_REQ.7 \
	doc/man/man7/cd_modem_sig_req_t.7 \
	doc/man/man7/CD_MUX_NAME_REQ.7 \
	doc/man/man7/cd_mux_name_req_t.7 \
	doc/man/man7/CD_OK_ACK.7 \
	doc/man/man7/cd_ok_ack_t.7 \
	doc/man/man7/CD_READ_REQ.7 \
	doc/man/man7/cd_read_req_t.7 \
	doc/man/man7/CD_UNITDATA_ACK.7 \
	doc/man/man7/cd_unitdata_ack_t.7 \
	doc/man/man7/CD_UNITDATA_IND.7 \
	doc/man/man7/cd_unitdata_ind_t.7 \
	doc/man/man7/CD_UNITDATA_REQ.7 \
	doc/man/man7/cd_unitdata_req_t.7 \
	doc/man/man7/CD_WRITE_READ_REQ.7 \
	doc/man/man7/cd_write_read_req_t.7 \
	doc/man/man7/DL_ATTACH_REQ.7 \
	doc/man/man7/dl_attach_req_t.7 \
	doc/man/man7/DL_BIND_ACK.7 \
	doc/man/man7/dl_bind_ack_t.7 \
	doc/man/man7/DL_BIND_REQ.7 \
	doc/man/man7/dl_bind_req_t.7 \
	doc/man/man7/DL_CONNECT_CON.7 \
	doc/man/man7/dl_connect_con_t.7 \
	doc/man/man7/DL_CONNECT_IND.7 \
	doc/man/man7/dl_connect_ind_t.7 \
	doc/man/man7/DL_CONNECT_REQ.7 \
	doc/man/man7/dl_connect_req_t.7 \
	doc/man/man7/DL_CONNECT_RES.7 \
	doc/man/man7/dl_connect_res_t.7 \
	doc/man/man7/DL_DATA_ACK_IND.7 \
	doc/man/man7/dl_data_ack_ind_t.7 \
	doc/man/man7/DL_DATA_ACK_REQ.7 \
	doc/man/man7/dl_data_ack_req_t.7 \
	doc/man/man7/DL_DATA_ACK_STATUS_IND.7 \
	doc/man/man7/dl_data_ack_status_ind_t.7 \
	doc/man/man7/DL_DATA_IND.7 \
	doc/man/man7/DL_DATA_REQ.7 \
	doc/man/man7/DL_DETACH_REQ.7 \
	doc/man/man7/dl_detach_req_t.7 \
	doc/man/man7/DL_DISABMULTI_REQ.7 \
	doc/man/man7/dl_disabmulti_req_t.7 \
	doc/man/man7/DL_DISCONNECT_IND.7 \
	doc/man/man7/dl_disconnect_ind_t.7 \
	doc/man/man7/DL_DISCONNECT_REQ.7 \
	doc/man/man7/dl_disconnect_req_t.7 \
	doc/man/man7/DL_ENABMULTI_REQ.7 \
	doc/man/man7/dl_enabmulti_req_t.7 \
	doc/man/man7/DL_ERROR_ACK.7 \
	doc/man/man7/dl_error_ack_t.7 \
	doc/man/man7/DL_GET_STATISTICS_ACK.7 \
	doc/man/man7/dl_get_statistics_ack_t.7 \
	doc/man/man7/DL_GET_STATISTICS_REQ.7 \
	doc/man/man7/dl_get_statistics_req_t.7 \
	doc/man/man7/DL_INFO_ACK.7 \
	doc/man/man7/dl_info_ack_t.7 \
	doc/man/man7/DL_INFO_REQ.7 \
	doc/man/man7/dl_info_req_t.7 \
	doc/man/man7/DL_OK_ACK.7 \
	doc/man/man7/dl_ok_ack_t.7 \
	doc/man/man7/DL_PHYS_ADDR_ACK.7 \
	doc/man/man7/dl_phys_addr_ack_t.7 \
	doc/man/man7/DL_PHYS_ADDR_REQ.7 \
	doc/man/man7/dl_phys_addr_req_t.7 \
	doc/man/man7/dlpi.7 \
	doc/man/man7/dlpi_eth.7 \
	doc/man/man7/DL_PROMISCOFF_REQ.7 \
	doc/man/man7/dl_promiscoff_req_t.7 \
	doc/man/man7/DL_PROMISCON_REQ.7 \
	doc/man/man7/dl_promiscon_req_t.7 \
	doc/man/man7/DL_REPLY_IND.7 \
	doc/man/man7/dl_reply_ind_t.7 \
	doc/man/man7/DL_REPLY_REQ.7 \
	doc/man/man7/dl_reply_req_t.7 \
	doc/man/man7/DL_REPLY_STATUS_IND.7 \
	doc/man/man7/dl_reply_status_ind_t.7 \
	doc/man/man7/DL_REPLY_UPDATE_REQ.7 \
	doc/man/man7/dl_reply_update_req_t.7 \
	doc/man/man7/DL_REPLY_UPDATE_STATUS_IND.7 \
	doc/man/man7/dl_reply_update_status_ind_t.7 \
	doc/man/man7/DL_RESET_CON.7 \
	doc/man/man7/dl_reset_con_t.7 \
	doc/man/man7/DL_RESET_IND.7 \
	doc/man/man7/dl_reset_ind_t.7 \
	doc/man/man7/DL_RESET_REQ.7 \
	doc/man/man7/dl_reset_req_t.7 \
	doc/man/man7/DL_RESET_RES.7 \
	doc/man/man7/dl_reset_res_t.7 \
	doc/man/man7/DL_SET_PHYS_ADDR_REQ.7 \
	doc/man/man7/dl_set_phys_addr_req_t.7 \
	doc/man/man7/DL_SUBS_BIND_ACK.7 \
	doc/man/man7/dl_subs_bind_ack_t.7 \
	doc/man/man7/DL_SUBS_BIND_REQ.7 \
	doc/man/man7/dl_subs_bind_req_t.7 \
	doc/man/man7/DL_SUBS_UNBIND_REQ.7 \
	doc/man/man7/dl_subs_unbind_req_t.7 \
	doc/man/man7/DL_TEST_CON.7 \
	doc/man/man7/dl_test_con_t.7 \
	doc/man/man7/DL_TEST_IND.7 \
	doc/man/man7/dl_test_ind_t.7 \
	doc/man/man7/DL_TEST_REQ.7 \
	doc/man/man7/dl_test_req_t.7 \
	doc/man/man7/DL_TEST_RES.7 \
	doc/man/man7/dl_test_res_t.7 \
	doc/man/man7/DL_TOKEN_ACK.7 \
	doc/man/man7/dl_token_ack_t.7 \
	doc/man/man7/DL_TOKEN_REQ.7 \
	doc/man/man7/dl_token_req_t.7 \
	doc/man/man7/DL_UDERROR_IND.7 \
	doc/man/man7/dl_uderror_ind_t.7 \
	doc/man/man7/DL_UDQOS_REQ.7 \
	doc/man/man7/dl_udqos_req_t.7 \
	doc/man/man7/DL_UNBIND_REQ.7 \
	doc/man/man7/dl_unbind_req_t.7 \
	doc/man/man7/DL_UNITDATA_IND.7 \
	doc/man/man7/dl_unitdata_ind_t.7 \
	doc/man/man7/DL_UNITDATA_REQ.7 \
	doc/man/man7/dl_unitdata_req_t.7 \
	doc/man/man7/DL_XID_CON.7 \
	doc/man/man7/dl_xid_con_t.7 \
	doc/man/man7/DL_XID_IND.7 \
	doc/man/man7/dl_xid_ind_t.7 \
	doc/man/man7/DL_XID_REQ.7 \
	doc/man/man7/dl_xid_req_t.7 \
	doc/man/man7/DL_XID_RES.7 \
	doc/man/man7/dl_xid_res_t.7 \
	doc/man/man7/N_BIND_ACK.7 \
	doc/man/man7/N_bind_ack_t.7 \
	doc/man/man7/N_BIND_REQ.7 \
	doc/man/man7/N_bind_req_t.7 \
	doc/man/man7/N_CONN_CON.7 \
	doc/man/man7/N_conn_con_t.7 \
	doc/man/man7/N_CONN_IND.7 \
	doc/man/man7/N_conn_ind_t.7 \
	doc/man/man7/N_CONN_REQ.7 \
	doc/man/man7/N_conn_req_t.7 \
	doc/man/man7/N_CONN_RES.7 \
	doc/man/man7/N_conn_res_t.7 \
	doc/man/man7/N_DATACK_IND.7 \
	doc/man/man7/N_datack_ind_t.7 \
	doc/man/man7/N_DATACK_REQ.7 \
	doc/man/man7/N_datack_req_t.7 \
	doc/man/man7/N_DATA_IND.7 \
	doc/man/man7/N_data_ind_t.7 \
	doc/man/man7/N_DATA_REQ.7 \
	doc/man/man7/N_data_req_t.7 \
	doc/man/man7/N_DISCON_IND.7 \
	doc/man/man7/N_discon_ind_t.7 \
	doc/man/man7/N_DISCON_REQ.7 \
	doc/man/man7/N_discon_req_t.7 \
	doc/man/man7/N_ERROR_ACK.7 \
	doc/man/man7/N_error_ack_t.7 \
	doc/man/man7/N_EXDATA_IND.7 \
	doc/man/man7/N_exdata_ind_t.7 \
	doc/man/man7/N_EXDATA_REQ.7 \
	doc/man/man7/N_exdata_req_t.7 \
	doc/man/man7/N_INFO_ACK.7 \
	doc/man/man7/N_info_ack_t.7 \
	doc/man/man7/N_INFO_REQ.7 \
	doc/man/man7/N_info_req_t.7 \
	doc/man/man7/N_OK_ACK.7 \
	doc/man/man7/N_ok_ack_t.7 \
	doc/man/man7/N_OPTMGMT_REQ.7 \
	doc/man/man7/N_optmgmt_req_t.7 \
	doc/man/man7/npi.7 \
	doc/man/man7/npi_sctp.7 \
	doc/man/man7/N_RESET_CON.7 \
	doc/man/man7/N_reset_con_t.7 \
	doc/man/man7/N_RESET_IND.7 \
	doc/man/man7/N_reset_ind_t.7 \
	doc/man/man7/N_RESET_REQ.7 \
	doc/man/man7/N_reset_req_t.7 \
	doc/man/man7/N_RESET_RES.7 \
	doc/man/man7/N_reset_res_t.7 \
	doc/man/man7/N_UDERROR_IND.7 \
	doc/man/man7/N_uderror_ind_t.7 \
	doc/man/man7/N_UNBIND_REQ.7 \
	doc/man/man7/N_unbind_req_t.7 \
	doc/man/man7/N_UNITDATA_IND.7 \
	doc/man/man7/N_unitdata_ind_t.7 \
	doc/man/man7/N_UNITDATA_REQ.7 \
	doc/man/man7/N_unitdata_req_t.7 \
	doc/man/man7/soon.7 \
	doc/man/man7/streamio.7 \
	doc/man/man7/T_ADDR_ACK.7 \
	doc/man/man7/T_addr_ack.7 \
	doc/man/man7/T_ADDR_REQ.7 \
	doc/man/man7/T_addr_req.7 \
	doc/man/man7/T_BIND_ACK.7 \
	doc/man/man7/T_bind_ack.7 \
	doc/man/man7/T_BIND_REQ.7 \
	doc/man/man7/T_bind_req.7 \
	doc/man/man7/T_CAPABILITY_ACK.7 \
	doc/man/man7/T_capability_ack.7 \
	doc/man/man7/T_CAPABILITY_REQ.7 \
	doc/man/man7/T_capability_req.7 \
	doc/man/man7/T_CONN_CON.7 \
	doc/man/man7/T_conn_con.7 \
	doc/man/man7/T_CONN_IND.7 \
	doc/man/man7/T_conn_ind.7 \
	doc/man/man7/T_CONN_REQ.7 \
	doc/man/man7/T_conn_req.7 \
	doc/man/man7/T_CONN_RES.7 \
	doc/man/man7/T_conn_res.7 \
	doc/man/man7/T_DATA_IND.7 \
	doc/man/man7/T_data_ind.7 \
	doc/man/man7/T_DATA_REQ.7 \
	doc/man/man7/T_data_req.7 \
	doc/man/man7/T_DISCON_IND.7 \
	doc/man/man7/T_discon_ind.7 \
	doc/man/man7/T_DISCON_REQ.7 \
	doc/man/man7/T_discon_req.7 \
	doc/man/man7/T_ERROR_ACK.7 \
	doc/man/man7/T_error_ack.7 \
	doc/man/man7/T_EXDATA_IND.7 \
	doc/man/man7/T_exdata_ind.7 \
	doc/man/man7/T_EXDATA_REQ.7 \
	doc/man/man7/T_exdata_req.7 \
	doc/man/man7/T_INFO_ACK.7 \
	doc/man/man7/T_info_ack.7 \
	doc/man/man7/T_INFO_REQ.7 \
	doc/man/man7/T_info_req.7 \
	doc/man/man7/T_OK_ACK.7 \
	doc/man/man7/T_ok_ack.7 \
	doc/man/man7/T_OPTDATA_IND.7 \
	doc/man/man7/T_optdata_ind.7 \
	doc/man/man7/T_OPTDATA_REQ.7 \
	doc/man/man7/T_optdata_req.7 \
	doc/man/man7/T_OPTMGMT_ACK.7 \
	doc/man/man7/T_optmgmt_ack.7 \
	doc/man/man7/T_OPTMGMT_REQ.7 \
	doc/man/man7/T_optmgmt_req.7 \
	doc/man/man7/T_ORDREL_IND.7 \
	doc/man/man7/T_ordrel_ind.7 \
	doc/man/man7/T_ORDREL_REQ.7 \
	doc/man/man7/T_ordrel_req.7 \
	doc/man/man7/tpi.7 \
	doc/man/man7/tpi_ip.7 \
	doc/man/man7/tpi_sctp.7 \
	doc/man/man7/tpi_tcp.7 \
	doc/man/man7/tpi_udp.7 \
	doc/man/man7/T_UDERROR_IND.7 \
	doc/man/man7/T_uderror_ind.7 \
	doc/man/man7/T_UNBIND_REQ.7 \
	doc/man/man7/T_unbind_req.7 \
	doc/man/man7/T_UNITDATA_IND.7 \
	doc/man/man7/T_unitdata_ind.7 \
	doc/man/man7/T_UNITDATA_REQ.7 \
	doc/man/man7/T_unitdata_req.7 \
	doc/man/man8/autopush.8 \
	doc/man/man8/fattach.8 \
	doc/man/man8/fattach.old.8 \
	doc/man/man8/fdetach.8 \
	doc/man/man8/fdetach.old.8 \
	doc/man/man8/insf.8 \
	doc/man/man8/lisbcm.8 \
	doc/man/man8/scls.8 \
	doc/man/man8/soon.8 \
	doc/man/man8/strace.8 \
	doc/man/man8/strclean.8 \
	doc/man/man8/strconf.8 \
	doc/man/man8/strerr.8 \
	doc/man/man8/strinfo.8 \
	doc/man/man8/strload.8 \
	doc/man/man8/strsetup.8 \
	doc/man/man8/strvf.8 \
	doc/man/man9/adjmsg.9 \
	doc/man/man9/allocb.9 \
	doc/man/man9/allocb_physreq.9 \
	doc/man/man9/alloc_proto.9 \
	doc/man/man9/allocq.9 \
	doc/man/man9/appq.9 \
	doc/man/man9/apush_get.9 \
	doc/man/man9/apush_set.9 \
	doc/man/man9/ATOMIC_INT_ADD.9 \
	doc/man/man9/ATOMIC_INT_ALLOC.9 \
	doc/man/man9/ATOMIC_INT_DEALLOC.9 \
	doc/man/man9/ATOMIC_INT_DECR.9 \
	doc/man/man9/ATOMIC_INT_INCR.9 \
	doc/man/man9/ATOMIC_INT_INIT.9 \
	doc/man/man9/ATOMIC_INT_READ.9 \
	doc/man/man9/ATOMIC_INT_SUB.9 \
	doc/man/man9/ATOMIC_INT_WRITE.9 \
	doc/man/man9/attach.9 \
	doc/man/man9/autopush_add.9 \
	doc/man/man9/autopush_del.9 \
	doc/man/man9/autopush_find.9 \
	doc/man/man9/autopush_vml.9 \
	doc/man/man9/backq.9 \
	doc/man/man9/bcanget.9 \
	doc/man/man9/bcanput.9 \
	doc/man/man9/bcanputnext.9 \
	doc/man/man9/bcopy.9 \
	doc/man/man9/bufcall.9 \
	doc/man/man9/bzero.9 \
	doc/man/man9/canenable.9 \
	doc/man/man9/canget.9 \
	doc/man/man9/canput.9 \
	doc/man/man9/canputnext.9 \
	doc/man/man9/cdevsw.9 \
	doc/man/man9/cmn_err.9 \
	doc/man/man9/copyb.9 \
	doc/man/man9/copyin.9 \
	doc/man/man9/copymsg.9 \
	doc/man/man9/copyout.9 \
	doc/man/man9/cred_t.9 \
	doc/man/man9/datab.9 \
	doc/man/man9/datamsg.9 \
	doc/man/man9/DDI.9 \
	doc/man/man9/ddi_create_minor_node.9 \
	doc/man/man9/ddi_driver_major.9 \
	doc/man/man9/ddi_driver_name.9 \
	doc/man/man9/ddi_get_cred.9 \
	doc/man/man9/ddi_getiminor.9 \
	doc/man/man9/ddi_get_instance.9 \
	doc/man/man9/ddi_get_lbolt.9 \
	doc/man/man9/ddi_get_pid.9 \
	doc/man/man9/ddi_get_soft_state.9 \
	doc/man/man9/ddi_get_time.9 \
	doc/man/man9/ddi_remove_minor_node.9 \
	doc/man/man9/ddi_removing_power.9 \
	doc/man/man9/ddi_soft_state.9 \
	doc/man/man9/ddi_soft_state_fini.9 \
	doc/man/man9/ddi_soft_state_free.9 \
	doc/man/man9/ddi_soft_state_init.9 \
	doc/man/man9/ddi_soft_state_zalloc.9 \
	doc/man/man9/ddi_umem_alloc.9 \
	doc/man/man9/ddi_umem_free.9 \
	doc/man/man9/delay.9 \
	doc/man/man9/detach.9 \
	doc/man/man9/drv_getparm.9 \
	doc/man/man9/drv_hztomsec.9 \
	doc/man/man9/drv_hztousec.9 \
	doc/man/man9/drv_msectohz.9 \
	doc/man/man9/drv_priv.9 \
	doc/man/man9/drv_usectohz.9 \
	doc/man/man9/drv_usecwait.9 \
	doc/man/man9/dtimeout.9 \
	doc/man/man9/dupb.9 \
	doc/man/man9/dupmsg.9 \
	doc/man/man9/emajor.9 \
	doc/man/man9/eminor.9 \
	doc/man/man9/enableok.9 \
	doc/man/man9/enableq.9 \
	doc/man/man9/esballoc.9 \
	doc/man/man9/esbbcall.9 \
	doc/man/man9/etoimajor.9 \
	doc/man/man9/_fini.9 \
	doc/man/man9/flushband.9 \
	doc/man/man9/flushq.9 \
	doc/man/man9/fmodsw.9 \
	doc/man/man9/freeb.9 \
	doc/man/man9/freemsg.9 \
	doc/man/man9/freeq.9 \
	doc/man/man9/freezestr.9 \
	doc/man/man9/getadmin.9 \
	doc/man/man9/getemajor.9 \
	doc/man/man9/geteminor.9 \
	doc/man/man9/getinfo.9 \
	doc/man/man9/getmajor.9 \
	doc/man/man9/getmid.9 \
	doc/man/man9/getminor.9 \
	doc/man/man9/getq.9 \
	doc/man/man9/identify.9 \
	doc/man/man9/_info.9 \
	doc/man/man9/_init.9 \
	doc/man/man9/insq.9 \
	doc/man/man9/install_driver.9 \
	doc/man/man9/Intro.9 \
	doc/man/man9/isdatablk.9 \
	doc/man/man9/isdatamsg.9 \
	doc/man/man9/itimeout.9 \
	doc/man/man9/itoemajor.9 \
	doc/man/man9/kmem_alloc.9 \
	doc/man/man9/kmem_free.9 \
	doc/man/man9/kmem_zalloc.9 \
	doc/man/man9/kmem_zalloc_node.9 \
	doc/man/man9/lbolt.9 \
	doc/man/man9/LfS.9 \
	doc/man/man9/lfs_register_strdev.9 \
	doc/man/man9/linkb.9 \
	doc/man/man9/linkmsg.9 \
	doc/man/man9/LiS.9 \
	doc/man/man9/lisbcm.9 \
	doc/man/man9/lis_date.9 \
	doc/man/man9/lis_esbbcall.9 \
	doc/man/man9/lis_find_strdev.9 \
	doc/man/man9lis/lis_add_timer.9lis \
	doc/man/man9lis/lis_adjmsg.9lis \
	doc/man/man9lis/lis_alloc_atomic_fcn.9lis \
	doc/man/man9lis/lis_allocb.9lis \
	doc/man/man9lis/lis_allocb_physreq.9lis \
	doc/man/man9lis/lis_alloc_dma_fcn.9lis \
	doc/man/man9lis/lis_alloc_kernel_fcn.9lis \
	doc/man/man9lis/lis_allocq.9lis \
	doc/man/man9lis/lis_appq.9lis \
	doc/man/man9lis/lis_apush_get.9lis \
	doc/man/man9lis/lis_apushm.9lis \
	doc/man/man9lis/lis_apush_set.9lis \
	doc/man/man9lis/lis_assert_fail.9lis \
	doc/man/man9lis/lis_atomic_add.9lis \
	doc/man/man9lis/lis_atomic_dec.9lis \
	doc/man/man9lis/lis_atomic_dec_and_test.9lis \
	doc/man/man9lis/lis_atomic_inc.9lis \
	doc/man/man9lis/lis_atomic_read.9lis \
	doc/man/man9lis/lis_atomic_set.9lis \
	doc/man/man9lis/lis_atomic_sub.9lis \
	doc/man/man9lis/lis_backq.9lis \
	doc/man/man9lis/lis_backq_fcn.9lis \
	doc/man/man9lis/lis_bcanput.9lis \
	doc/man/man9lis/lis_bcanputnext.9lis \
	doc/man/man9lis/lis_bcanputnext_anyband.9lis \
	doc/man/man9lis/lis_bcopy.9lis \
	doc/man/man9lis/lis_bprintf.9lis \
	doc/man/man9lis/lis_bufcall.9lis \
	doc/man/man9lis/lis_bzero.9lis \
	doc/man/man9lis/lis_canenable.9lis \
	doc/man/man9lis/lis_canput.9lis \
	doc/man/man9lis/lis_canputnext.9lis \
	doc/man/man9lis/lis_can_unload.9lis \
	doc/man/man9lis/lis_check_guard.9lis \
	doc/man/man9lis/lis_check_mem.9lis \
	doc/man/man9lis/lis_check_q_magic.9lis \
	doc/man/man9lis/lis_check_region.9lis \
	doc/man/man9lis/lis_check_umem.9lis \
	doc/man/man9lis/lis_clone_major.9lis \
	doc/man/man9lis/lis_cmn_err.9lis \
	doc/man/man9lis/lis_copyb.9lis \
	doc/man/man9lis/lis_copyin.9lis \
	doc/man/man9lis/lis_copymsg.9lis \
	doc/man/man9lis/lis_copyout.9lis \
	doc/man/man9lis/lis_datamsg.9lis \
	doc/man/man9lis/lis_date.9lis \
	doc/man/man9lis/lis_debug_mask2.9lis \
	doc/man/man9lis/lis_debug_mask.9lis \
	doc/man/man9lis/lis_dec_mod_cnt_fcn.9lis \
	doc/man/man9lis/lis_del_timer.9lis \
	doc/man/man9lis/lis_disable_irq.9lis \
	doc/man/man9lis/lis_dobufcall.9lis \
	doc/man/man9lis/lis_doclose.9lis \
	doc/man/man9lis/lis_down_fcn.9lis \
	doc/man/man9lis/lis_dsecs.9lis \
	doc/man/man9lis/lis_dupb.9lis \
	doc/man/man9lis/lis_dupmsg.9lis \
	doc/man/man9lis/lis_enable_intr.9lis \
	doc/man/man9lis/lis_enable_irq.9lis \
	doc/man/man9lis/lis_enableok.9lis \
	doc/man/man9lis/lis_esballoc.9lis \
	doc/man/man9lis/lis_esbbcall.9lis \
	doc/man/man9lis/lis_fattach.9lis \
	doc/man/man9lis/lis_fdetach.9lis \
	doc/man/man9lis/lis_fdetach_all.9lis \
	doc/man/man9lis/lis_fdetach_stream.9lis \
	doc/man/man9lis/lis_fifo_close_sync.9lis \
	doc/man/man9lis/lis_fifo_open_sync.9lis \
	doc/man/man9lis/lis_fifo_write_sync.9lis \
	doc/man/man9lis/lis_file_inode.9lis \
	doc/man/man9lis/lis_file_str.9lis \
	doc/man/man9lis/lis_find_strdev.9lis \
	doc/man/man9lis/lis_flushband.9lis \
	doc/man/man9lis/lis_flush_print_buffer.9lis \
	doc/man/man9lis/lis_flushq.9lis \
	doc/man/man9lis/lis_fmod_sw.9lis \
	doc/man/man9lis/lis_free.9lis \
	doc/man/man9lis/lis_freeb.9lis \
	doc/man/man9lis/lis_freedb.9lis \
	doc/man/man9lis/lis_free_dma.9lis \
	doc/man/man9lis/lis_free_irq.9lis \
	doc/man/man9lis/lis_free_mem_fcn.9lis \
	doc/man/man9lis/lis_freemsg.9lis \
	doc/man/man9lis/lis_free_pages_fcn.9lis \
	doc/man/man9lis/lis_free_passfp.9lis \
	doc/man/man9lis/lis_freeq.9lis \
	doc/man/man9lis/lis_fstr_sw.9lis \
	doc/man/man9lis/lis_get_fifo.9lis \
	doc/man/man9lis/lis_get_free_pages_atomic_fcn.9lis \
	doc/man/man9lis/lis_get_free_pages_fcn.9lis \
	doc/man/man9lis/lis_get_free_pages_kernel_fcn.9lis \
	doc/man/man9lis/lis_getint.9lis \
	doc/man/man9lis/lis_get_passfp.9lis \
	doc/man/man9lis/lis_get_pipe.9lis \
	doc/man/man9lis/lis_getq.9lis \
	doc/man/man9lis/lis_gettimeofday.9lis \
	doc/man/man9lis/lis_hitime.9lis \
	doc/man/man9lis/lis_inc_mod_cnt_fcn.9lis \
	doc/man/man9lis/lis_init_bufcall.9lis \
	doc/man/man9lis/lis_insq.9lis \
	doc/man/man9lis/lis_in_syscall.9lis \
	doc/man/man9lis/lis_interruptible_sleep_on.9lis \
	doc/man/man9lis/lis_ioc_fattach.9lis \
	doc/man/man9lis/lis_ioc_fdetach.9lis \
	doc/man/man9lis/lis_ioc_pipe.9lis \
	doc/man/man9lis/lis_ioremap.9lis \
	doc/man/man9lis/lis_ioremap_nocache.9lis \
	doc/man/man9lis/lis_iounmap.9lis \
	doc/man/man9lis/lis_isdatablk.9lis \
	doc/man/man9lis/lis_isdatamsg.9lis \
	doc/man/man9lis/lis_jiffies.9lis \
	doc/man/man9lis/lis_kernel_down.9lis \
	doc/man/man9lis/lis_kernel_up.9lis \
	doc/man/man9lis/lis_kernel_version.9lis \
	doc/man/man9lis/lis__kfree.9lis \
	doc/man/man9lis/lis_kfree.9lis \
	doc/man/man9lis/lis_kill_pg.9lis \
	doc/man/man9lis/lis_kill_proc.9lis \
	doc/man/man9lis/lis__kmalloc.9lis \
	doc/man/man9lis/lis_kmalloc.9lis \
	doc/man/man9lis/lis_linkb.9lis \
	doc/man/man9lis/lis_loadable_load.9lis \
	doc/man/man9lis/lis_maj_min_name.9lis \
	doc/man/man9lis/lis_major.9lis \
	doc/man/man9lis/lis_malloc.9lis \
	doc/man/man9lis/lis_mark_mem.9lis \
	doc/man/man9lis/lis_max_mem.9lis \
	doc/man/man9lis/lis_milli_to_ticks.9lis \
	doc/man/man9lis/lis_mknod.9lis \
	doc/man/man9lis/lis_mount.9lis \
	doc/man/man9lis/lis_msecs.9lis \
	doc/man/man9lis/lis_msgdsize.9lis \
	doc/man/man9lis/lis_msgpullup.9lis \
	doc/man/man9lis/lis_msgsize.9lis \
	doc/man/man9lis/lis_msg_type_name.9lis \
	doc/man/man9lis/lis_msgutil.9lis \
	doc/man/man9lis/lis_noenable.9lis \
	doc/man/man9lis/lis_num_cpus.9lis \
	doc/man/man9lis/lis_old_inode.9lis \
	doc/man/man9lis/lis_open_cnt.9lis \
	doc/man/man9lis/lis_osif_cli.9lis \
	doc/man/man9lis/lis_osif_do_gettimeofday.9lis \
	doc/man/man9lis/lis_osif_do_settimeofday.9lis \
	doc/man/man9lis/lis_osif_pci_alloc_consistent.9lis \
	doc/man/man9lis/lis_osif_pci_dac_dma_supported.9lis \
	doc/man/man9lis/lis_osif_pci_dac_dma_sync_single.9lis \
	doc/man/man9lis/lis_osif_pci_dac_dma_to_offset.9lis \
	doc/man/man9lis/lis_osif_pci_dac_dma_to_page.9lis \
	doc/man/man9lis/lis_osif_pci_dac_page_to_dma.9lis \
	doc/man/man9lis/lis_osif_pci_dac_set_dma_mask.9lis \
	doc/man/man9lis/lis_osif_pci_disable_device.9lis \
	doc/man/man9lis/lis_osif_pci_dma_supported.9lis \
	doc/man/man9lis/lis_osif_pci_dma_sync_sg.9lis \
	doc/man/man9lis/lis_osif_pci_dma_sync_single.9lis \
	doc/man/man9lis/lis_osif_pci_enable_device.9lis \
	doc/man/man9lis/lis_osif_pci_find_class.9lis \
	doc/man/man9lis/lis_osif_pci_find_device.9lis \
	doc/man/man9lis/lis_osif_pci_find_slot.9lis \
	doc/man/man9lis/lis_osif_pci_free_consistent.9lis \
	doc/man/man9lis/lis_osif_pci_map_page.9lis \
	doc/man/man9lis/lis_osif_pci_map_sg.9lis \
	doc/man/man9lis/lis_osif_pci_map_single.9lis \
	doc/man/man9lis/lis_osif_pci_module_init.9lis \
	doc/man/man9lis/lis_osif_pci_read_config_byte.9lis \
	doc/man/man9lis/lis_osif_pci_read_config_dword.9lis \
	doc/man/man9lis/lis_osif_pci_read_config_word.9lis \
	doc/man/man9lis/lis_osif_pci_set_dma_mask.9lis \
	doc/man/man9lis/lis_osif_pci_set_master.9lis \
	doc/man/man9lis/lis_osif_pci_unmap_page.9lis \
	doc/man/man9lis/lis_osif_pci_unmap_sg.9lis \
	doc/man/man9lis/lis_osif_pci_unmap_single.9lis \
	doc/man/man9lis/lis_osif_pci_unregister_driver.9lis \
	doc/man/man9lis/lis_osif_pci_write_config_byte.9lis \
	doc/man/man9lis/lis_osif_pci_write_config_dword.9lis \
	doc/man/man9lis/lis_osif_pci_write_config_word.9lis \
	doc/man/man9lis/lis_osif_sg_dma_address.9lis \
	doc/man/man9lis/lis_osif_sg_dma_len.9lis \
	doc/man/man9lis/lis_osif_sti.9lis \
	doc/man/man9lis/lis_OTHER.9lis \
	doc/man/man9lis/lis_OTHERQ.9lis \
	doc/man/man9lis/lis_own_spl.9lis \
	doc/man/man9lis/lis_pcibios_find_class.9lis \
	doc/man/man9lis/lis_pcibios_find_device.9lis \
	doc/man/man9lis/lis_pcibios_init.9lis \
	doc/man/man9lis/lis_pcibios_present.9lis \
	doc/man/man9lis/lis_pcibios_read_config_byte.9lis \
	doc/man/man9lis/lis_pcibios_read_config_dword.9lis \
	doc/man/man9lis/lis_pcibios_read_config_word.9lis \
	doc/man/man9lis/lis_pcibios_strerror.9lis \
	doc/man/man9lis/lis_pcibios_write_config_byte.9lis \
	doc/man/man9lis/lis_pcibios_write_config_dword.9lis \
	doc/man/man9lis/lis_pcibios_write_config_word.9lis \
	doc/man/man9lis/lis_pci_cleanup.9lis \
	doc/man/man9lis/lis_pci_disable_device.9lis \
	doc/man/man9lis/lis_pci_enable_device.9lis \
	doc/man/man9lis/lis_pci_find_class.9lis \
	doc/man/man9lis/lis_pci_find_device.9lis \
	doc/man/man9lis/lis_pci_find_slot.9lis \
	doc/man/man9lis/lis_pci_read_config_byte.9lis \
	doc/man/man9lis/lis_pci_read_config_dword.9lis \
	doc/man/man9lis/lis_pci_read_config_word.9lis \
	doc/man/man9lis/lis_pci_set_master.9lis \
	doc/man/man9lis/lis_pci_write_config_byte.9lis \
	doc/man/man9lis/lis_pci_write_config_dword.9lis \
	doc/man/man9lis/lis_pci_write_config_word.9lis \
	doc/man/man9lis/lis_phys_to_virt.9lis \
	doc/man/man9lis/lis_pipe.9lis \
	doc/man/man9lis/lis_poll_2_1.9lis \
	doc/man/man9lis/lis_poll_bits.9lis \
	doc/man/man9lis/lis_poll_events.9lis \
	doc/man/man9lis/lis_poll_file.9lis \
	doc/man/man9lis/lis_print_block.9lis \
	doc/man/man9lis/lis_print_data.9lis \
	doc/man/man9lis/lis_printk.9lis \
	doc/man/man9lis/lis_print_mem.9lis \
	doc/man/man9lis/lis_print_msg.9lis \
	doc/man/man9lis/lis_print_queue.9lis \
	doc/man/man9lis/lis_print_queues.9lis \
	doc/man/man9lis/lis_print_spl_track.9lis \
	doc/man/man9lis/lis_print_stream.9lis \
	doc/man/man9lis/lis_pullupmsg.9lis \
	doc/man/man9lis/lis_putbq.9lis \
	doc/man/man9lis/lis_putbyte.9lis \
	doc/man/man9lis/lis_putctl1.9lis \
	doc/man/man9lis/lis_putctl.9lis \
	doc/man/man9lis/lis_putnext.9lis \
	doc/man/man9lis/lis_putnextctl1.9lis \
	doc/man/man9lis/lis_putnextctl.9lis \
	doc/man/man9lis/lis_putq.9lis \
	doc/man/man9lis/lis_qattach.9lis \
	doc/man/man9lis/lis_qclose.9lis \
	doc/man/man9lis/lis_qcountstrm.9lis \
	doc/man/man9lis/lis_qdetach.9lis \
	doc/man/man9lis/lis_qenable.9lis \
	doc/man/man9lis/lis_qprocsoff.9lis \
	doc/man/man9lis/lis_qprocson.9lis \
	doc/man/man9lis/lis_qreply.9lis \
	doc/man/man9lis/lis_qsize.9lis \
	doc/man/man9lis/lis_queue_name.9lis \
	doc/man/man9lis/lis_queuerun_cnts.9lis \
	doc/man/man9lis/lis_queues_running.9lis \
	doc/man/man9lis/lis_RD.9lis \
	doc/man/man9lis/lis_recvfd.9lis \
	doc/man/man9lis/lis_register_strdev.9lis \
	doc/man/man9lis/lis_register_strmod.9lis \
	doc/man/man9lis/lis_release_region.9lis \
	doc/man/man9lis/lis_request_dma.9lis \
	doc/man/man9lis/lis_request_irq.9lis \
	doc/man/man9lis/lis_request_region.9lis \
	doc/man/man9lis/lis_rmvb.9lis \
	doc/man/man9lis/lis_rmvq.9lis \
	doc/man/man9lis/lis_runq_cnts.9lis \
	doc/man/man9lis/lis_runq_req_cnt.9lis \
	doc/man/man9lis/lis_rw_lock_alloc_fcn.9lis \
	doc/man/man9lis/lis_rw_lock_free_fcn.9lis \
	doc/man/man9lis/lis_rw_lock_init_fcn.9lis \
	doc/man/man9lis/lis_rw_read_lock_fcn.9lis \
	doc/man/man9lis/lis_rw_read_lock_irq_fcn.9lis \
	doc/man/man9lis/lis_rw_read_lock_irqsave_fcn.9lis \
	doc/man/man9lis/lis_rw_read_unlock_fcn.9lis \
	doc/man/man9lis/lis_rw_read_unlock_irq_fcn.9lis \
	doc/man/man9lis/lis_rw_read_unlock_irqrestore_fcn.9lis \
	doc/man/man9lis/lis_rw_write_lock_fcn.9lis \
	doc/man/man9lis/lis_rw_write_lock_irq_fcn.9lis \
	doc/man/man9lis/lis_rw_write_lock_irqsave_fcn.9lis \
	doc/man/man9lis/lis_rw_write_unlock_fcn.9lis \
	doc/man/man9lis/lis_rw_write_unlock_irq_fcn.9lis \
	doc/man/man9lis/lis_rw_write_unlock_irqrestore_fcn.9lis \
	doc/man/man9lis/lis_safe_canenable.9lis \
	doc/man/man9lis/lis_safe_enableok.9lis \
	doc/man/man9lis/lis_safe_noenable.9lis \
	doc/man/man9lis/lis_safe_OTHERQ.9lis \
	doc/man/man9lis/lis_safe_putmsg.9lis \
	doc/man/man9lis/lis_safe_putnext.9lis \
	doc/man/man9lis/lis_safe_qreply.9lis \
	doc/man/man9lis/lis_safe_RD.9lis \
	doc/man/man9lis/lis_safe_SAMESTR.9lis \
	doc/man/man9lis/lis_safe_WR.9lis \
	doc/man/man9lis/lis_SAMESTR.9lis \
	doc/man/man9lis/lis_secs.9lis \
	doc/man/man9lis/lis_sem_alloc.9lis \
	doc/man/man9lis/lis_sem_destroy.9lis \
	doc/man/man9lis/lis_sem_init.9lis \
	doc/man/man9lis/lis_sendfd.9lis \
	doc/man/man9lis/lis_set_file_str.9lis \
	doc/man/man9lis/lis_setq.9lis \
	doc/man/man9lis/lis_setqsched.9lis \
	doc/man/man9lis/lis_setqsched_cnts.9lis \
	doc/man/man9lis/lis_setqsched_isr_cnts.9lis \
	doc/man/man9lis/lis_sleep_on.9lis \
	doc/man/man9lis/lis_spin_is_locked_fcn.9lis \
	doc/man/man9lis/lis_spin_lock_alloc_fcn.9lis \
	doc/man/man9lis/lis_spin_lock_fcn.9lis \
	doc/man/man9lis/lis_spin_lock_free_fcn.9lis \
	doc/man/man9lis/lis_spin_lock_init_fcn.9lis \
	doc/man/man9lis/lis_spin_lock_irq_fcn.9lis \
	doc/man/man9lis/lis_spin_lock_irqsave_fcn.9lis \
	doc/man/man9lis/lis_spin_trylock_fcn.9lis \
	doc/man/man9lis/lis_spin_unlock_fcn.9lis \
	doc/man/man9lis/lis_spin_unlock_irq_fcn.9lis \
	doc/man/man9lis/lis_spin_unlock_irqrestore_fcn.9lis \
	doc/man/man9lis/lis_spl0_fcn.9lis \
	doc/man/man9lis/lis_splstr_fcn.9lis \
	doc/man/man9lis/lis_splx_fcn.9lis \
	doc/man/man9lis/lis_sprintf.9lis \
	doc/man/man9lis/lis_stdata_cnt.9lis \
	doc/man/man9lis/lis_strclose.9lis \
	doc/man/man9lis/lis_strcount.9lis \
	doc/man/man9lis/lis_stream_utils.9lis \
	doc/man/man9lis/lis_strgetpmsg.9lis \
	doc/man/man9lis/lis_strioctl.9lis \
	doc/man/man9lis/lis_strm_name.9lis \
	doc/man/man9lis/lis_strm_name_from_queue.9lis \
	doc/man/man9lis/lis_stropen.9lis \
	doc/man/man9lis/lis_stropts_file.9lis \
	doc/man/man9lis/lis_strputpmsg.9lis \
	doc/man/man9lis/lis_strqget.9lis \
	doc/man/man9lis/lis_strqset.9lis \
	doc/man/man9lis/lis_strread.9lis \
	doc/man/man9lis/lis_strstats.9lis \
	doc/man/man9lis/lis_strwrite.9lis \
	doc/man/man9lis/lis_testb.9lis \
	doc/man/man9lis/lis_thread_start.9lis \
	doc/man/man9lis/lis_thread_stop.9lis \
	doc/man/man9lis/lis_timeout.9lis \
	doc/man/man9lis/lis_timeout_fcn.9lis \
	doc/man/man9lis/lis_udelay.9lis \
	doc/man/man9lis/lis_umount2.9lis \
	doc/man/man9lis/lis_umount.9lis \
	doc/man/man9lis/lis_unbufcall.9lis \
	doc/man/man9lis/lis_unlink.9lis \
	doc/man/man9lis/lis_unlinkb.9lis \
	doc/man/man9lis/lis_unregister_strdev.9lis \
	doc/man/man9lis/lis_unregister_strmod.9lis \
	doc/man/man9lis/lis_untimeout.9lis \
	doc/man/man9lis/lis_up_fcn.9lis \
	doc/man/man9lis/lis_usecs.9lis \
	doc/man/man9lis/lis_usectohz.9lis \
	doc/man/man9lis/lis_valid_mod_list.9lis \
	doc/man/man9lis/lis_version.9lis \
	doc/man/man9lis/lis_vfree.9lis \
	doc/man/man9lis/lis_virt_to_phys.9lis \
	doc/man/man9lis/lis_vmalloc.9lis \
	doc/man/man9lis/lis_vremap.9lis \
	doc/man/man9lis/lis_vsprintf.9lis \
	doc/man/man9lis/lis_wake_up.9lis \
	doc/man/man9lis/lis_wake_up_interruptible.9lis \
	doc/man/man9lis/lis_WR.9lis \
	doc/man/man9lis/lis_xmsgsize.9lis \
	doc/man/man9lis/lis_zmalloc.9lis \
	doc/man/man9/lis_mknod.9 \
	doc/man/man9/lis_mount.9 \
	doc/man/man9/lis_OTHER.9 \
	doc/man/man9/lis_register_strdev.9 \
	doc/man/man9/lis_register_strmod.9 \
	doc/man/man9/lis_umount2.9 \
	doc/man/man9/lis_umount.9 \
	doc/man/man9/lis_unregister_strdev.9 \
	doc/man/man9/lis_unregsiter_strmod.9 \
	doc/man/man9/lis_version.9 \
	doc/man/man9/LOCK.9 \
	doc/man/man9/LOCK_ALLOC.9 \
	doc/man/man9/LOCK_DEALLOC.9 \
	doc/man/man9/LOCK_OWNED.9 \
	doc/man/man9/major.9 \
	doc/man/man9/makedev.9 \
	doc/man/man9/makedevice.9 \
	doc/man/man9/mapinit.9 \
	doc/man/man9/max.9 \
	doc/man/man9/mblk_t.9 \
	doc/man/man9/mi_bufcall.9 \
	doc/man/man9/mi_close_comm.9 \
	doc/man/man9/min.9 \
	doc/man/man9/mi_next_ptr.9 \
	doc/man/man9/minor.9 \
	doc/man/man9/mi_open_comm.9 \
	doc/man/man9/mknod.9 \
	doc/man/man9/mod_info.9 \
	doc/man/man9/mod_install.9 \
	doc/man/man9/mod_remove.9 \
	doc/man/man9/module_info.9 \
	doc/man/man9/mount.9 \
	doc/man/man9/mp-streams.9 \
	doc/man/man9/MPSTR_QLOCK.9 \
	doc/man/man9/MPSTR_QRELE.9 \
	doc/man/man9/MPSTR_STPLOCK.9 \
	doc/man/man9/MPSTR_STPRELE.9 \
	doc/man/man9/msgb.9 \
	doc/man/man9/msgdsize.9 \
	doc/man/man9/msgphysreq.9 \
	doc/man/man9/msgpullup.9 \
	doc/man/man9/msgpullup_physreq.9 \
	doc/man/man9/msgscgth.9 \
	doc/man/man9/msgsize.9 \
	doc/man/man9/msgutil.9 \
	doc/man/man9/noenable.9 \
	doc/man/man9/OTHER.9 \
	doc/man/man9/OTHERQ.9 \
	doc/man/man9/pcmsg.9 \
	doc/man/man9/physreq.9 \
	doc/man/man9/physreq_t.9 \
	doc/man/man9/pipe.9 \
	doc/man/man9/power.9 \
	doc/man/man9/printf.9 \
	doc/man/man9/probe.9 \
	doc/man/man9/pullupmsg.9 \
	doc/man/man9/put.9 \
	doc/man/man9/putbq.9 \
	doc/man/man9/putctl1.9 \
	doc/man/man9/putctl2.9 \
	doc/man/man9/putctl.9 \
	doc/man/man9/puthere.9 \
	doc/man/man9/putnext.9 \
	doc/man/man9/putnextctl1.9 \
	doc/man/man9/putnextctl2.9 \
	doc/man/man9/putnextctl.9 \
	doc/man/man9/putq.9 \
	doc/man/man9/qattach.9 \
	doc/man/man9/qband.9 \
	doc/man/man9/qbufcall.9 \
	doc/man/man9/qclose.9 \
	doc/man/man9/qcountstrm.9 \
	doc/man/man9/qdetach.9 \
	doc/man/man9/qenable.9 \
	doc/man/man9/qinit.9 \
	doc/man/man9/qinsert.9 \
	doc/man/man9/qopen.9 \
	doc/man/man9/qprocsoff.9 \
	doc/man/man9/qprocson.9 \
	doc/man/man9/qreply.9 \
	doc/man/man9/qsize.9 \
	doc/man/man9/qtimeout.9 \
	doc/man/man9/queclass.9 \
	doc/man/man9/queinfo.9 \
	doc/man/man9/queue.9 \
	doc/man/man9/queue_t.9 \
	doc/man/man9/qunbufcall.9 \
	doc/man/man9/quntimeout.9 \
	doc/man/man9/qwait.9 \
	doc/man/man9/qwait_sig.9 \
	doc/man/man9/qwriter.9 \
	doc/man/man9/RD.9 \
	doc/man/man9/register_inode.9 \
	doc/man/man9/register_inode_major.9 \
	doc/man/man9/register_inode_minor.9 \
	doc/man/man9/register_strdev.9 \
	doc/man/man9/register_strdev_major.9 \
	doc/man/man9/register_strdev_minor.9 \
	doc/man/man9/register_strmod.9 \
	doc/man/man9/rmvb.9 \
	doc/man/man9/rmvq.9 \
	doc/man/man9/RW_ALLOC.9 \
	doc/man/man9/RW_DEALLOC.9 \
	doc/man/man9/RW_RDLOCK.9 \
	doc/man/man9/RW_TRYRDLOCK.9 \
	doc/man/man9/RW_TRYWRLOCK.9 \
	doc/man/man9/RW_UNLOCK.9 \
	doc/man/man9/RW_WRLOCK.9 \
	doc/man/man9/SAMESTR.9 \
	doc/man/man9/setq.9 \
	doc/man/man9/sleep.9 \
	doc/man/man9/SLEEP_ALLOC.9 \
	doc/man/man9/SLEEP_DEALLOC.9 \
	doc/man/man9/SLEEP_LOCK.9 \
	doc/man/man9/SLEEP_LOCKAVAIL.9 \
	doc/man/man9/SLEEP_LOCKOWNED.9 \
	doc/man/man9/SLEEP_LOCK_SIG.9 \
	doc/man/man9/SLEEP_TRYLOCK.9 \
	doc/man/man9/SLEEP_UNLOCK.9 \
	doc/man/man9/soon.9 \
	doc/man/man9/SPG.9 \
	doc/man/man9/spl0.9 \
	doc/man/man9/spl1.9 \
	doc/man/man9/spl2.9 \
	doc/man/man9/spl3.9 \
	doc/man/man9/spl4.9 \
	doc/man/man9/spl5.9 \
	doc/man/man9/spl6.9 \
	doc/man/man9/spl7.9 \
	doc/man/man9/spl.9 \
	doc/man/man9/splstr.9 \
	doc/man/man9/splx.9 \
	doc/man/man9/STREAMS.9 \
	doc/man/man9/streams_close_comm.9 \
	doc/man/man9/streams_get_sleep_lock.9 \
	doc/man/man9/streams_open_comm.9 \
	doc/man/man9/streams_open_ocomm.9 \
	doc/man/man9/streams_put.9 \
	doc/man/man9/streamtab.9 \
	doc/man/man9/stream_utils.9 \
	doc/man/man9/str_install.9 \
	doc/man/man9/str_install_AIX.9 \
	doc/man/man9/str_install_HPUX.9 \
	doc/man/man9/strioccall.9 \
	doc/man/man9/strlog.9 \
	doc/man/man9/strmod_add.9 \
	doc/man/man9/strmod_del.9 \
	doc/man/man9/strqget.9 \
	doc/man/man9/strqset.9 \
	doc/man/man9/str_uninstall.9 \
	doc/man/man9/SV_ALLOC.9 \
	doc/man/man9/SV_BROADCAST.9 \
	doc/man/man9/SV_DEALLOC.9 \
	doc/man/man9/SV_SIGNAL.9 \
	doc/man/man9/SV_WAIT.9 \
	doc/man/man9/SV_WAIT_SIG.9 \
	doc/man/man9/testb.9 \
	doc/man/man9/time.9 \
	doc/man/man9/timeout.9 \
	doc/man/man9/TRYLOCK.9 \
	doc/man/man9/umount2.9 \
	doc/man/man9/umount.9 \
	doc/man/man9/unbufcall.9 \
	doc/man/man9/unfreezestr.9 \
	doc/man/man9/unlink.9 \
	doc/man/man9/unlinkb.9 \
	doc/man/man9/UNLOCK.9 \
	doc/man/man9/unregister_inode.9 \
	doc/man/man9/unregister_inode_major.9 \
	doc/man/man9/unregister_inode_minor.9 \
	doc/man/man9/unregister_strdev.9 \
	doc/man/man9/unregister_strdev_major.9 \
	doc/man/man9/unregister_strdev_minor.9 \
	doc/man/man9/unregister_strmod.9 \
	doc/man/man9/untimeout.9 \
	doc/man/man9/unweldq.9 \
	doc/man/man9/vtop.9 \
	doc/man/man9/wakeup.9 \
	doc/man/man9/wantio.9 \
	doc/man/man9/wantmsg.9 \
	doc/man/man9/weldq.9 \
	doc/man/man9/WR.9 \
	doc/man/man9/xmsgsize.9

noinst_man_MANS = \
	doc/man/man1/strchg.1.man \
	doc/man/man1/strconf.1.man \
	doc/man/man1/strreset.1.man \
	doc/man/man2/fattach.2.man \
	doc/man/man2/fcntl.2s.man \
	doc/man/man2/getmsg.2.man \
	doc/man/man2/getpmsg.2.man \
	doc/man/man2/pipe.2s.man \
	doc/man/man2/poll.2s.man \
	doc/man/man2/putmsg.2.man \
	doc/man/man2/putpmsg.2.man \
	doc/man/man2/read.2s.man \
	doc/man/man2/readv.2s.man \
	doc/man/man2/soon.2.man \
	doc/man/man2/write.2s.man \
	doc/man/man2/writev.2s.man \
	doc/man/man3/accept.3.man \
	doc/man/man3/bind.3.man \
	doc/man/man3/connect.3.man \
	doc/man/man3/fattach.3.man \
	doc/man/man3/fdetach.3.man \
	doc/man/man3/getpeername.3.man \
	doc/man/man3/getsockname.3.man \
	doc/man/man3/getsockopt.3.man \
	doc/man/man3/isastream.3.man \
	doc/man/man3/libsocket.3.man \
	doc/man/man3/listen.3.man \
	doc/man/man3/netbuf.3.man \
	doc/man/man3/pipe.3.man \
	doc/man/man3/recv.3.man \
	doc/man/man3/recvfrom.3.man \
	doc/man/man3/recvmsg.3.man \
	doc/man/man3/send.3.man \
	doc/man/man3/sendmsg.3.man \
	doc/man/man3/sendto.3.man \
	doc/man/man3/setsockopt.3.man \
	doc/man/man3/shutdown.3.man \
	doc/man/man3/socket.3.man \
	doc/man/man3/socketpair.3.man \
	doc/man/man3/sockets.3.man \
	doc/man/man3/s_pipe.3.man \
	doc/man/man3/t_accept.3.man \
	doc/man/man3/t_addleaf.3.man \
	doc/man/man3/t_alloc.3.man \
	doc/man/man3/t_bind.3.man \
	doc/man/man3/t_close.3.man \
	doc/man/man3/t_connect.3.man \
	doc/man/man3/t_errno.3.man \
	doc/man/man3/t_error.3.man \
	doc/man/man3/t_free.3.man \
	doc/man/man3/t_getinfo.3.man \
	doc/man/man3/t_getprotaddr.3.man \
	doc/man/man3/t_getstate.3.man \
	doc/man/man3/t_listen.3.man \
	doc/man/man3/t_look.3.man \
	doc/man/man3/t_open.3.man \
	doc/man/man3/t_optmgmt.3.man \
	doc/man/man3/t_rcv.3.man \
	doc/man/man3/t_rcvconnect.3.man \
	doc/man/man3/t_rcvdis.3.man \
	doc/man/man3/t_rcvleafchange.3.man \
	doc/man/man3/t_rcvrel.3.man \
	doc/man/man3/t_rcvreldata.3.man \
	doc/man/man3/t_rcvudata.3.man \
	doc/man/man3/t_rcvuderr.3.man \
	doc/man/man3/t_rcvv.3.man \
	doc/man/man3/t_rcvvudata.3.man \
	doc/man/man3/t_removeleaf.3.man \
	doc/man/man3/t_snd.3.man \
	doc/man/man3/t_snddis.3.man \
	doc/man/man3/t_sndopt.3.man \
	doc/man/man3/t_sndrel.3.man \
	doc/man/man3/t_sndreldata.3.man \
	doc/man/man3/t_sndudata.3.man \
	doc/man/man3/t_sndv.3.man \
	doc/man/man3/t_sndvopt.3.man \
	doc/man/man3/t_sndvudata.3.man \
	doc/man/man3/t_strerror.3.man \
	doc/man/man3/t_sync.3.man \
	doc/man/man3/t_sysconf.3.man \
	doc/man/man3/t_unbind.3.man \
	doc/man/man3/xnet.3.man \
	doc/man/man3/xti.3.man \
	doc/man/man3/xti_ip.3.man \
	doc/man/man3/xti_sctp.3.man \
	doc/man/man3/xti_tcp.3.man \
	doc/man/man3/xti_udp.3.man \
	doc/man/man4/clone.4.man \
	doc/man/man4/connld.4.man \
	doc/man/man4/connld.old.4.man \
	doc/man/man4/echo.4.man \
	doc/man/man4/egp.4.man \
	doc/man/man4/eth.4.man \
	doc/man/man4/fifo.4.man \
	doc/man/man4/fifo.4s.man \
	doc/man/man4/fifo.old.4.man \
	doc/man/man4/ggp.4.man \
	doc/man/man4/icmp.4.man \
	doc/man/man4/idp.4.man \
	doc/man/man4/ifnet.4.man \
	doc/man/man4/INET.4.man \
	doc/man/man4/ip.4.man \
	doc/man/man4/ipip.4.man \
	doc/man/man4/nsdev.4.man \
	doc/man/man4/nuls.4.man \
	doc/man/man4/pipe.4.man \
	doc/man/man4/pipemod.4.man \
	doc/man/man4/pipemod.old.4.man \
	doc/man/man4/pup.4.man \
	doc/man/man4/rawip.4.man \
	doc/man/man4/sad.4.man \
	doc/man/man4/sc.4.man \
	doc/man/man4/s_fifo.4.man \
	doc/man/man4/sfx.4.man \
	doc/man/man4/sockmod.4.man \
	doc/man/man4/socksys.4.man \
	doc/man/man4/spx.4.man \
	doc/man/man4/strlog.4.man \
	doc/man/man4/tcp.4.man \
	doc/man/man4/ticlts.4.man \
	doc/man/man4/ticots.4.man \
	doc/man/man4/ticotsord.4.man \
	doc/man/man4/timod.4.man \
	doc/man/man4/tirdwr.4.man \
	doc/man/man4/tlclts.4.man \
	doc/man/man4/tlcots.4.man \
	doc/man/man4/tlcotsod.4.man \
	doc/man/man4/udp.4.man \
	doc/man/man5/autopush.5.man \
	doc/man/man5/specfs.5.man \
	doc/man/man5/strsetup.conf.5.man \
	doc/man/man7/CD_ABORT_OUTPUT_REQ.7.man \
	doc/man/man7/cd_abort_output_req_t.7.man \
	doc/man/man7/CD_ALLOW_INPUT_REQ.7.man \
	doc/man/man7/cd_allow_input_req_t.7.man \
	doc/man/man7/CD_ATTACH_REQ.7.man \
	doc/man/man7/cd_attach_req_t.7.man \
	doc/man/man7/CD_BAD_FRAME_IND.7.man \
	doc/man/man7/cd_bad_frame_ind_t.7.man \
	doc/man/man7/CD_DETACH_REQ.7.man \
	doc/man/man7/cd_detach_req_t.7.man \
	doc/man/man7/CD_DISABLE_CON.7.man \
	doc/man/man7/cd_disable_con_t.7.man \
	doc/man/man7/CD_DISABLE_REQ.7.man \
	doc/man/man7/cd_disable_req_t.7.man \
	doc/man/man7/CD_ENABLE_CON.7.man \
	doc/man/man7/cd_enable_con_t.7.man \
	doc/man/man7/CD_ENABLE_REQ.7.man \
	doc/man/man7/cd_enable_req_t.7.man \
	doc/man/man7/CD_ERROR_ACK.7.man \
	doc/man/man7/cd_error_ack_t.7.man \
	doc/man/man7/CD_ERROR_IND.7.man \
	doc/man/man7/cd_error_ind_t.7.man \
	doc/man/man7/CD_HALT_INPUT_REQ.7.man \
	doc/man/man7/cd_halt_input_req_t.7.man \
	doc/man/man7/CD_INFO_ACK.7.man \
	doc/man/man7/cd_info_ack_t.7.man \
	doc/man/man7/CD_INFO_REQ.7.man \
	doc/man/man7/CD_MODEM_SIG_IND.7.man \
	doc/man/man7/cd_modem_sig_ind_t.7.man \
	doc/man/man7/CD_MODEM_SIG_POLL.7.man \
	doc/man/man7/cd_modem_sig_poll_t.7.man \
	doc/man/man7/CD_MODEM_SIG_REQ.7.man \
	doc/man/man7/cd_modem_sig_req_t.7.man \
	doc/man/man7/CD_MUX_NAME_REQ.7.man \
	doc/man/man7/cd_mux_name_req_t.7.man \
	doc/man/man7/CD_OK_ACK.7.man \
	doc/man/man7/cd_ok_ack_t.7.man \
	doc/man/man7/CD_READ_REQ.7.man \
	doc/man/man7/cd_read_req_t.7.man \
	doc/man/man7/CD_UNITDATA_ACK.7.man \
	doc/man/man7/cd_unitdata_ack_t.7.man \
	doc/man/man7/CD_UNITDATA_IND.7.man \
	doc/man/man7/cd_unitdata_ind_t.7.man \
	doc/man/man7/CD_UNITDATA_REQ.7.man \
	doc/man/man7/cd_unitdata_req_t.7.man \
	doc/man/man7/CD_WRITE_READ_REQ.7.man \
	doc/man/man7/cd_write_read_req_t.7.man \
	doc/man/man7/DL_ATTACH_REQ.7.man \
	doc/man/man7/dl_attach_req_t.7.man \
	doc/man/man7/DL_BIND_ACK.7.man \
	doc/man/man7/dl_bind_ack_t.7.man \
	doc/man/man7/DL_BIND_REQ.7.man \
	doc/man/man7/dl_bind_req_t.7.man \
	doc/man/man7/DL_CONNECT_CON.7.man \
	doc/man/man7/dl_connect_con_t.7.man \
	doc/man/man7/DL_CONNECT_IND.7.man \
	doc/man/man7/dl_connect_ind_t.7.man \
	doc/man/man7/DL_CONNECT_REQ.7.man \
	doc/man/man7/dl_connect_req_t.7.man \
	doc/man/man7/DL_CONNECT_RES.7.man \
	doc/man/man7/dl_connect_res_t.7.man \
	doc/man/man7/DL_DATA_ACK_IND.7.man \
	doc/man/man7/dl_data_ack_ind_t.7.man \
	doc/man/man7/DL_DATA_ACK_REQ.7.man \
	doc/man/man7/dl_data_ack_req_t.7.man \
	doc/man/man7/DL_DATA_ACK_STATUS_IND.7.man \
	doc/man/man7/dl_data_ack_status_ind_t.7.man \
	doc/man/man7/DL_DATA_IND.7.man \
	doc/man/man7/DL_DATA_REQ.7.man \
	doc/man/man7/DL_DETACH_REQ.7.man \
	doc/man/man7/dl_detach_req_t.7.man \
	doc/man/man7/DL_DISABMULTI_REQ.7.man \
	doc/man/man7/dl_disabmulti_req_t.7.man \
	doc/man/man7/DL_DISCONNECT_IND.7.man \
	doc/man/man7/dl_disconnect_ind_t.7.man \
	doc/man/man7/DL_DISCONNECT_REQ.7.man \
	doc/man/man7/dl_disconnect_req_t.7.man \
	doc/man/man7/DL_ENABMULTI_REQ.7.man \
	doc/man/man7/dl_enabmulti_req_t.7.man \
	doc/man/man7/DL_ERROR_ACK.7.man \
	doc/man/man7/dl_error_ack_t.7.man \
	doc/man/man7/DL_GET_STATISTICS_ACK.7.man \
	doc/man/man7/dl_get_statistics_ack_t.7.man \
	doc/man/man7/DL_GET_STATISTICS_REQ.7.man \
	doc/man/man7/dl_get_statistics_req_t.7.man \
	doc/man/man7/DL_INFO_ACK.7.man \
	doc/man/man7/dl_info_ack_t.7.man \
	doc/man/man7/DL_INFO_REQ.7.man \
	doc/man/man7/dl_info_req_t.7.man \
	doc/man/man7/DL_OK_ACK.7.man \
	doc/man/man7/dl_ok_ack_t.7.man \
	doc/man/man7/DL_PHYS_ADDR_ACK.7.man \
	doc/man/man7/dl_phys_addr_ack_t.7.man \
	doc/man/man7/DL_PHYS_ADDR_REQ.7.man \
	doc/man/man7/dl_phys_addr_req_t.7.man \
	doc/man/man7/dlpi.7.man \
	doc/man/man7/dlpi_eth.7.man \
	doc/man/man7/DL_PROMISCOFF_REQ.7.man \
	doc/man/man7/dl_promiscoff_req_t.7.man \
	doc/man/man7/DL_PROMISCON_REQ.7.man \
	doc/man/man7/dl_promiscon_req_t.7.man \
	doc/man/man7/DL_REPLY_IND.7.man \
	doc/man/man7/dl_reply_ind_t.7.man \
	doc/man/man7/DL_REPLY_REQ.7.man \
	doc/man/man7/dl_reply_req_t.7.man \
	doc/man/man7/DL_REPLY_STATUS_IND.7.man \
	doc/man/man7/dl_reply_status_ind_t.7.man \
	doc/man/man7/DL_REPLY_UPDATE_REQ.7.man \
	doc/man/man7/dl_reply_update_req_t.7.man \
	doc/man/man7/DL_REPLY_UPDATE_STATUS_IND.7.man \
	doc/man/man7/dl_reply_update_status_ind_t.7.man \
	doc/man/man7/DL_RESET_CON.7.man \
	doc/man/man7/dl_reset_con_t.7.man \
	doc/man/man7/DL_RESET_IND.7.man \
	doc/man/man7/dl_reset_ind_t.7.man \
	doc/man/man7/DL_RESET_REQ.7.man \
	doc/man/man7/dl_reset_req_t.7.man \
	doc/man/man7/DL_RESET_RES.7.man \
	doc/man/man7/dl_reset_res_t.7.man \
	doc/man/man7/DL_SET_PHYS_ADDR_REQ.7.man \
	doc/man/man7/dl_set_phys_addr_req_t.7.man \
	doc/man/man7/DL_SUBS_BIND_ACK.7.man \
	doc/man/man7/dl_subs_bind_ack_t.7.man \
	doc/man/man7/DL_SUBS_BIND_REQ.7.man \
	doc/man/man7/dl_subs_bind_req_t.7.man \
	doc/man/man7/DL_SUBS_UNBIND_REQ.7.man \
	doc/man/man7/dl_subs_unbind_req_t.7.man \
	doc/man/man7/DL_TEST_CON.7.man \
	doc/man/man7/dl_test_con_t.7.man \
	doc/man/man7/DL_TEST_IND.7.man \
	doc/man/man7/dl_test_ind_t.7.man \
	doc/man/man7/DL_TEST_REQ.7.man \
	doc/man/man7/dl_test_req_t.7.man \
	doc/man/man7/DL_TEST_RES.7.man \
	doc/man/man7/dl_test_res_t.7.man \
	doc/man/man7/DL_TOKEN_ACK.7.man \
	doc/man/man7/dl_token_ack_t.7.man \
	doc/man/man7/DL_TOKEN_REQ.7.man \
	doc/man/man7/dl_token_req_t.7.man \
	doc/man/man7/DL_UDERROR_IND.7.man \
	doc/man/man7/dl_uderror_ind_t.7.man \
	doc/man/man7/DL_UDQOS_REQ.7.man \
	doc/man/man7/dl_udqos_req_t.7.man \
	doc/man/man7/DL_UNBIND_REQ.7.man \
	doc/man/man7/dl_unbind_req_t.7.man \
	doc/man/man7/DL_UNITDATA_IND.7.man \
	doc/man/man7/dl_unitdata_ind_t.7.man \
	doc/man/man7/DL_UNITDATA_REQ.7.man \
	doc/man/man7/dl_unitdata_req_t.7.man \
	doc/man/man7/DL_XID_CON.7.man \
	doc/man/man7/dl_xid_con_t.7.man \
	doc/man/man7/DL_XID_IND.7.man \
	doc/man/man7/dl_xid_ind_t.7.man \
	doc/man/man7/DL_XID_REQ.7.man \
	doc/man/man7/dl_xid_req_t.7.man \
	doc/man/man7/DL_XID_RES.7.man \
	doc/man/man7/dl_xid_res_t.7.man \
	doc/man/man7/N_BIND_ACK.7.man \
	doc/man/man7/N_bind_ack_t.7.man \
	doc/man/man7/N_BIND_REQ.7.man \
	doc/man/man7/N_bind_req_t.7.man \
	doc/man/man7/N_CONN_CON.7.man \
	doc/man/man7/N_conn_con_t.7.man \
	doc/man/man7/N_CONN_IND.7.man \
	doc/man/man7/N_conn_ind_t.7.man \
	doc/man/man7/N_CONN_REQ.7.man \
	doc/man/man7/N_conn_req_t.7.man \
	doc/man/man7/N_CONN_RES.7.man \
	doc/man/man7/N_conn_res_t.7.man \
	doc/man/man7/N_DATACK_IND.7.man \
	doc/man/man7/N_datack_ind_t.7.man \
	doc/man/man7/N_DATACK_REQ.7.man \
	doc/man/man7/N_datack_req_t.7.man \
	doc/man/man7/N_DATA_IND.7.man \
	doc/man/man7/N_data_ind_t.7.man \
	doc/man/man7/N_DATA_REQ.7.man \
	doc/man/man7/N_data_req_t.7.man \
	doc/man/man7/N_DISCON_IND.7.man \
	doc/man/man7/N_discon_ind_t.7.man \
	doc/man/man7/N_DISCON_REQ.7.man \
	doc/man/man7/N_discon_req_t.7.man \
	doc/man/man7/N_ERROR_ACK.7.man \
	doc/man/man7/N_error_ack_t.7.man \
	doc/man/man7/N_EXDATA_IND.7.man \
	doc/man/man7/N_exdata_ind_t.7.man \
	doc/man/man7/N_EXDATA_REQ.7.man \
	doc/man/man7/N_exdata_req_t.7.man \
	doc/man/man7/N_INFO_ACK.7.man \
	doc/man/man7/N_info_ack_t.7.man \
	doc/man/man7/N_INFO_REQ.7.man \
	doc/man/man7/N_info_req_t.7.man \
	doc/man/man7/N_OK_ACK.7.man \
	doc/man/man7/N_ok_ack_t.7.man \
	doc/man/man7/N_OPTMGMT_REQ.7.man \
	doc/man/man7/N_optmgmt_req_t.7.man \
	doc/man/man7/npi.7.man \
	doc/man/man7/npi_sctp.7.man \
	doc/man/man7/N_RESET_CON.7.man \
	doc/man/man7/N_reset_con_t.7.man \
	doc/man/man7/N_RESET_IND.7.man \
	doc/man/man7/N_reset_ind_t.7.man \
	doc/man/man7/N_RESET_REQ.7.man \
	doc/man/man7/N_reset_req_t.7.man \
	doc/man/man7/N_RESET_RES.7.man \
	doc/man/man7/N_reset_res_t.7.man \
	doc/man/man7/N_UDERROR_IND.7.man \
	doc/man/man7/N_uderror_ind_t.7.man \
	doc/man/man7/N_UNBIND_REQ.7.man \
	doc/man/man7/N_unbind_req_t.7.man \
	doc/man/man7/N_UNITDATA_IND.7.man \
	doc/man/man7/N_unitdata_ind_t.7.man \
	doc/man/man7/N_UNITDATA_REQ.7.man \
	doc/man/man7/N_unitdata_req_t.7.man \
	doc/man/man7/soon.7.man \
	doc/man/man7/streamio.7.man \
	doc/man/man7/T_ADDR_ACK.7.man \
	doc/man/man7/T_addr_ack.7.man \
	doc/man/man7/T_ADDR_REQ.7.man \
	doc/man/man7/T_addr_req.7.man \
	doc/man/man7/T_BIND_ACK.7.man \
	doc/man/man7/T_bind_ack.7.man \
	doc/man/man7/T_BIND_REQ.7.man \
	doc/man/man7/T_bind_req.7.man \
	doc/man/man7/T_CAPABILITY_ACK.7.man \
	doc/man/man7/T_capability_ack.7.man \
	doc/man/man7/T_CAPABILITY_REQ.7.man \
	doc/man/man7/T_capability_req.7.man \
	doc/man/man7/T_CONN_CON.7.man \
	doc/man/man7/T_conn_con.7.man \
	doc/man/man7/T_CONN_IND.7.man \
	doc/man/man7/T_conn_ind.7.man \
	doc/man/man7/T_CONN_REQ.7.man \
	doc/man/man7/T_conn_req.7.man \
	doc/man/man7/T_CONN_RES.7.man \
	doc/man/man7/T_conn_res.7.man \
	doc/man/man7/T_DATA_IND.7.man \
	doc/man/man7/T_data_ind.7.man \
	doc/man/man7/T_DATA_REQ.7.man \
	doc/man/man7/T_data_req.7.man \
	doc/man/man7/T_DISCON_IND.7.man \
	doc/man/man7/T_discon_ind.7.man \
	doc/man/man7/T_DISCON_REQ.7.man \
	doc/man/man7/T_discon_req.7.man \
	doc/man/man7/T_ERROR_ACK.7.man \
	doc/man/man7/T_error_ack.7.man \
	doc/man/man7/T_EXDATA_IND.7.man \
	doc/man/man7/T_exdata_ind.7.man \
	doc/man/man7/T_EXDATA_REQ.7.man \
	doc/man/man7/T_exdata_req.7.man \
	doc/man/man7/T_INFO_ACK.7.man \
	doc/man/man7/T_info_ack.7.man \
	doc/man/man7/T_INFO_REQ.7.man \
	doc/man/man7/T_info_req.7.man \
	doc/man/man7/T_OK_ACK.7.man \
	doc/man/man7/T_ok_ack.7.man \
	doc/man/man7/T_OPTDATA_IND.7.man \
	doc/man/man7/T_optdata_ind.7.man \
	doc/man/man7/T_OPTDATA_REQ.7.man \
	doc/man/man7/T_optdata_req.7.man \
	doc/man/man7/T_OPTMGMT_ACK.7.man \
	doc/man/man7/T_optmgmt_ack.7.man \
	doc/man/man7/T_OPTMGMT_REQ.7.man \
	doc/man/man7/T_optmgmt_req.7.man \
	doc/man/man7/T_ORDREL_IND.7.man \
	doc/man/man7/T_ordrel_ind.7.man \
	doc/man/man7/T_ORDREL_REQ.7.man \
	doc/man/man7/T_ordrel_req.7.man \
	doc/man/man7/tpi.7.man \
	doc/man/man7/tpi_ip.7.man \
	doc/man/man7/tpi_sctp.7.man \
	doc/man/man7/tpi_tcp.7.man \
	doc/man/man7/tpi_udp.7.man \
	doc/man/man7/T_UDERROR_IND.7.man \
	doc/man/man7/T_uderror_ind.7.man \
	doc/man/man7/T_UNBIND_REQ.7.man \
	doc/man/man7/T_unbind_req.7.man \
	doc/man/man7/T_UNITDATA_IND.7.man \
	doc/man/man7/T_unitdata_ind.7.man \
	doc/man/man7/T_UNITDATA_REQ.7.man \
	doc/man/man7/T_unitdata_req.7.man \
	doc/man/man8/autopush.8.man \
	doc/man/man8/fattach.8.man \
	doc/man/man8/fattach.old.8.man \
	doc/man/man8/fdetach.8.man \
	doc/man/man8/fdetach.old.8.man \
	doc/man/man8/insf.8.man \
	doc/man/man8/lisbcm.8.man \
	doc/man/man8/scls.8.man \
	doc/man/man8/soon.8.man \
	doc/man/man8/strace.8.man \
	doc/man/man8/strclean.8.man \
	doc/man/man8/strconf.8.man \
	doc/man/man8/strerr.8.man \
	doc/man/man8/strinfo.8.man \
	doc/man/man8/strload.8.man \
	doc/man/man8/strsetup.8.man \
	doc/man/man8/strvf.8.man \
	doc/man/man9/adjmsg.9.man \
	doc/man/man9/allocb.9.man \
	doc/man/man9/allocb_physreq.9.man \
	doc/man/man9/alloc_proto.9.man \
	doc/man/man9/allocq.9.man \
	doc/man/man9/appq.9.man \
	doc/man/man9/apush_get.9.man \
	doc/man/man9/apush_set.9.man \
	doc/man/man9/ATOMIC_INT_ADD.9.man \
	doc/man/man9/ATOMIC_INT_ALLOC.9.man \
	doc/man/man9/ATOMIC_INT_DEALLOC.9.man \
	doc/man/man9/ATOMIC_INT_DECR.9.man \
	doc/man/man9/ATOMIC_INT_INCR.9.man \
	doc/man/man9/ATOMIC_INT_INIT.9.man \
	doc/man/man9/ATOMIC_INT_READ.9.man \
	doc/man/man9/ATOMIC_INT_SUB.9.man \
	doc/man/man9/ATOMIC_INT_WRITE.9.man \
	doc/man/man9/attach.9.man \
	doc/man/man9/autopush_add.9.man \
	doc/man/man9/autopush_del.9.man \
	doc/man/man9/autopush_find.9.man \
	doc/man/man9/autopush_vml.9.man \
	doc/man/man9/backq.9.man \
	doc/man/man9/bcanget.9.man \
	doc/man/man9/bcanput.9.man \
	doc/man/man9/bcanputnext.9.man \
	doc/man/man9/bcopy.9.man \
	doc/man/man9/bufcall.9.man \
	doc/man/man9/bzero.9.man \
	doc/man/man9/canenable.9.man \
	doc/man/man9/canget.9.man \
	doc/man/man9/canput.9.man \
	doc/man/man9/canputnext.9.man \
	doc/man/man9/cdevsw.9.man \
	doc/man/man9/cmn_err.9.man \
	doc/man/man9/copyb.9.man \
	doc/man/man9/copyin.9.man \
	doc/man/man9/copymsg.9.man \
	doc/man/man9/copyout.9.man \
	doc/man/man9/cred_t.9.man \
	doc/man/man9/datab.9.man \
	doc/man/man9/datamsg.9.man \
	doc/man/man9/DDI.9.man \
	doc/man/man9/ddi_create_minor_node.9.man \
	doc/man/man9/ddi_driver_major.9.man \
	doc/man/man9/ddi_driver_name.9.man \
	doc/man/man9/ddi_get_cred.9.man \
	doc/man/man9/ddi_getiminor.9.man \
	doc/man/man9/ddi_get_instance.9.man \
	doc/man/man9/ddi_get_lbolt.9.man \
	doc/man/man9/ddi_get_pid.9.man \
	doc/man/man9/ddi_get_soft_state.9.man \
	doc/man/man9/ddi_get_time.9.man \
	doc/man/man9/ddi_remove_minor_node.9.man \
	doc/man/man9/ddi_removing_power.9.man \
	doc/man/man9/ddi_soft_state.9.man \
	doc/man/man9/ddi_soft_state_fini.9.man \
	doc/man/man9/ddi_soft_state_free.9.man \
	doc/man/man9/ddi_soft_state_init.9.man \
	doc/man/man9/ddi_soft_state_zalloc.9.man \
	doc/man/man9/ddi_umem_alloc.9.man \
	doc/man/man9/ddi_umem_free.9.man \
	doc/man/man9/delay.9.man \
	doc/man/man9/detach.9.man \
	doc/man/man9/drv_getparm.9.man \
	doc/man/man9/drv_hztomsec.9.man \
	doc/man/man9/drv_hztousec.9.man \
	doc/man/man9/drv_msectohz.9.man \
	doc/man/man9/drv_priv.9.man \
	doc/man/man9/drv_usectohz.9.man \
	doc/man/man9/drv_usecwait.9.man \
	doc/man/man9/dtimeout.9.man \
	doc/man/man9/dupb.9.man \
	doc/man/man9/dupmsg.9.man \
	doc/man/man9/emajor.9.man \
	doc/man/man9/eminor.9.man \
	doc/man/man9/enableok.9.man \
	doc/man/man9/enableq.9.man \
	doc/man/man9/esballoc.9.man \
	doc/man/man9/esbbcall.9.man \
	doc/man/man9/etoimajor.9.man \
	doc/man/man9/_fini.9.man \
	doc/man/man9/flushband.9.man \
	doc/man/man9/flushq.9.man \
	doc/man/man9/fmodsw.9.man \
	doc/man/man9/freeb.9.man \
	doc/man/man9/freemsg.9.man \
	doc/man/man9/freeq.9.man \
	doc/man/man9/freezestr.9.man \
	doc/man/man9/getadmin.9.man \
	doc/man/man9/getemajor.9.man \
	doc/man/man9/geteminor.9.man \
	doc/man/man9/getinfo.9.man \
	doc/man/man9/getmajor.9.man \
	doc/man/man9/getmid.9.man \
	doc/man/man9/getminor.9.man \
	doc/man/man9/getq.9.man \
	doc/man/man9/identify.9.man \
	doc/man/man9/_info.9.man \
	doc/man/man9/_init.9.man \
	doc/man/man9/insq.9.man \
	doc/man/man9/install_driver.9.man \
	doc/man/man9/Intro.9.man \
	doc/man/man9/isdatablk.9.man \
	doc/man/man9/isdatamsg.9.man \
	doc/man/man9/itimeout.9.man \
	doc/man/man9/itoemajor.9.man \
	doc/man/man9/kmem_alloc.9.man \
	doc/man/man9/kmem_free.9.man \
	doc/man/man9/kmem_zalloc.9.man \
	doc/man/man9/kmem_zalloc_node.9.man \
	doc/man/man9/lbolt.9.man \
	doc/man/man9/LfS.9.man \
	doc/man/man9/lfs_register_strdev.9.man \
	doc/man/man9/linkb.9.man \
	doc/man/man9/linkmsg.9.man \
	doc/man/man9/LiS.9.man \
	doc/man/man9/lisbcm.9.man \
	doc/man/man9/lis_date.9.man \
	doc/man/man9/lis_esbbcall.9.man \
	doc/man/man9/lis_find_strdev.9.man \
	doc/man/man9lis/lis_add_timer.9lis.man \
	doc/man/man9lis/lis_adjmsg.9lis.man \
	doc/man/man9lis/lis_alloc_atomic_fcn.9lis.man \
	doc/man/man9lis/lis_allocb.9lis.man \
	doc/man/man9lis/lis_allocb_physreq.9lis.man \
	doc/man/man9lis/lis_alloc_dma_fcn.9lis.man \
	doc/man/man9lis/lis_alloc_kernel_fcn.9lis.man \
	doc/man/man9lis/lis_allocq.9lis.man \
	doc/man/man9lis/lis_appq.9lis.man \
	doc/man/man9lis/lis_apush_get.9lis.man \
	doc/man/man9lis/lis_apushm.9lis.man \
	doc/man/man9lis/lis_apush_set.9lis.man \
	doc/man/man9lis/lis_assert_fail.9lis.man \
	doc/man/man9lis/lis_atomic_add.9lis.man \
	doc/man/man9lis/lis_atomic_dec.9lis.man \
	doc/man/man9lis/lis_atomic_dec_and_test.9lis.man \
	doc/man/man9lis/lis_atomic_inc.9lis.man \
	doc/man/man9lis/lis_atomic_read.9lis.man \
	doc/man/man9lis/lis_atomic_set.9lis.man \
	doc/man/man9lis/lis_atomic_sub.9lis.man \
	doc/man/man9lis/lis_backq.9lis.man \
	doc/man/man9lis/lis_backq_fcn.9lis.man \
	doc/man/man9lis/lis_bcanput.9lis.man \
	doc/man/man9lis/lis_bcanputnext.9lis.man \
	doc/man/man9lis/lis_bcanputnext_anyband.9lis.man \
	doc/man/man9lis/lis_bcopy.9lis.man \
	doc/man/man9lis/lis_bprintf.9lis.man \
	doc/man/man9lis/lis_bufcall.9lis.man \
	doc/man/man9lis/lis_bzero.9lis.man \
	doc/man/man9lis/lis_canenable.9lis.man \
	doc/man/man9lis/lis_canput.9lis.man \
	doc/man/man9lis/lis_canputnext.9lis.man \
	doc/man/man9lis/lis_can_unload.9lis.man \
	doc/man/man9lis/lis_check_guard.9lis.man \
	doc/man/man9lis/lis_check_mem.9lis.man \
	doc/man/man9lis/lis_check_q_magic.9lis.man \
	doc/man/man9lis/lis_check_region.9lis.man \
	doc/man/man9lis/lis_check_umem.9lis.man \
	doc/man/man9lis/lis_clone_major.9lis.man \
	doc/man/man9lis/lis_cmn_err.9lis.man \
	doc/man/man9lis/lis_copyb.9lis.man \
	doc/man/man9lis/lis_copyin.9lis.man \
	doc/man/man9lis/lis_copymsg.9lis.man \
	doc/man/man9lis/lis_copyout.9lis.man \
	doc/man/man9lis/lis_datamsg.9lis.man \
	doc/man/man9lis/lis_date.9lis.man \
	doc/man/man9lis/lis_debug_mask2.9lis.man \
	doc/man/man9lis/lis_debug_mask.9lis.man \
	doc/man/man9lis/lis_dec_mod_cnt_fcn.9lis.man \
	doc/man/man9lis/lis_del_timer.9lis.man \
	doc/man/man9lis/lis_disable_irq.9lis.man \
	doc/man/man9lis/lis_dobufcall.9lis.man \
	doc/man/man9lis/lis_doclose.9lis.man \
	doc/man/man9lis/lis_down_fcn.9lis.man \
	doc/man/man9lis/lis_dsecs.9lis.man \
	doc/man/man9lis/lis_dupb.9lis.man \
	doc/man/man9lis/lis_dupmsg.9lis.man \
	doc/man/man9lis/lis_enable_intr.9lis.man \
	doc/man/man9lis/lis_enable_irq.9lis.man \
	doc/man/man9lis/lis_enableok.9lis.man \
	doc/man/man9lis/lis_esballoc.9lis.man \
	doc/man/man9lis/lis_esbbcall.9lis.man \
	doc/man/man9lis/lis_fattach.9lis.man \
	doc/man/man9lis/lis_fdetach.9lis.man \
	doc/man/man9lis/lis_fdetach_all.9lis.man \
	doc/man/man9lis/lis_fdetach_stream.9lis.man \
	doc/man/man9lis/lis_fifo_close_sync.9lis.man \
	doc/man/man9lis/lis_fifo_open_sync.9lis.man \
	doc/man/man9lis/lis_fifo_write_sync.9lis.man \
	doc/man/man9lis/lis_file_inode.9lis.man \
	doc/man/man9lis/lis_file_str.9lis.man \
	doc/man/man9lis/lis_find_strdev.9lis.man \
	doc/man/man9lis/lis_flushband.9lis.man \
	doc/man/man9lis/lis_flush_print_buffer.9lis.man \
	doc/man/man9lis/lis_flushq.9lis.man \
	doc/man/man9lis/lis_fmod_sw.9lis.man \
	doc/man/man9lis/lis_free.9lis.man \
	doc/man/man9lis/lis_freeb.9lis.man \
	doc/man/man9lis/lis_freedb.9lis.man \
	doc/man/man9lis/lis_free_dma.9lis.man \
	doc/man/man9lis/lis_free_irq.9lis.man \
	doc/man/man9lis/lis_free_mem_fcn.9lis.man \
	doc/man/man9lis/lis_freemsg.9lis.man \
	doc/man/man9lis/lis_free_pages_fcn.9lis.man \
	doc/man/man9lis/lis_free_passfp.9lis.man \
	doc/man/man9lis/lis_freeq.9lis.man \
	doc/man/man9lis/lis_fstr_sw.9lis.man \
	doc/man/man9lis/lis_get_fifo.9lis.man \
	doc/man/man9lis/lis_get_free_pages_atomic_fcn.9lis.man \
	doc/man/man9lis/lis_get_free_pages_fcn.9lis.man \
	doc/man/man9lis/lis_get_free_pages_kernel_fcn.9lis.man \
	doc/man/man9lis/lis_getint.9lis.man \
	doc/man/man9lis/lis_get_passfp.9lis.man \
	doc/man/man9lis/lis_get_pipe.9lis.man \
	doc/man/man9lis/lis_getq.9lis.man \
	doc/man/man9lis/lis_gettimeofday.9lis.man \
	doc/man/man9lis/lis_hitime.9lis.man \
	doc/man/man9lis/lis_inc_mod_cnt_fcn.9lis.man \
	doc/man/man9lis/lis_init_bufcall.9lis.man \
	doc/man/man9lis/lis_insq.9lis.man \
	doc/man/man9lis/lis_in_syscall.9lis.man \
	doc/man/man9lis/lis_interruptible_sleep_on.9lis.man \
	doc/man/man9lis/lis_ioc_fattach.9lis.man \
	doc/man/man9lis/lis_ioc_fdetach.9lis.man \
	doc/man/man9lis/lis_ioc_pipe.9lis.man \
	doc/man/man9lis/lis_ioremap.9lis.man \
	doc/man/man9lis/lis_ioremap_nocache.9lis.man \
	doc/man/man9lis/lis_iounmap.9lis.man \
	doc/man/man9lis/lis_isdatablk.9lis.man \
	doc/man/man9lis/lis_isdatamsg.9lis.man \
	doc/man/man9lis/lis_jiffies.9lis.man \
	doc/man/man9lis/lis_kernel_down.9lis.man \
	doc/man/man9lis/lis_kernel_up.9lis.man \
	doc/man/man9lis/lis_kernel_version.9lis.man \
	doc/man/man9lis/lis__kfree.9lis.man \
	doc/man/man9lis/lis_kfree.9lis.man \
	doc/man/man9lis/lis_kill_pg.9lis.man \
	doc/man/man9lis/lis_kill_proc.9lis.man \
	doc/man/man9lis/lis__kmalloc.9lis.man \
	doc/man/man9lis/lis_kmalloc.9lis.man \
	doc/man/man9lis/lis_linkb.9lis.man \
	doc/man/man9lis/lis_loadable_load.9lis.man \
	doc/man/man9lis/lis_maj_min_name.9lis.man \
	doc/man/man9lis/lis_major.9lis.man \
	doc/man/man9lis/lis_malloc.9lis.man \
	doc/man/man9lis/lis_mark_mem.9lis.man \
	doc/man/man9lis/lis_max_mem.9lis.man \
	doc/man/man9lis/lis_milli_to_ticks.9lis.man \
	doc/man/man9lis/lis_mknod.9lis.man \
	doc/man/man9lis/lis_mount.9lis.man \
	doc/man/man9lis/lis_msecs.9lis.man \
	doc/man/man9lis/lis_msgdsize.9lis.man \
	doc/man/man9lis/lis_msgpullup.9lis.man \
	doc/man/man9lis/lis_msgsize.9lis.man \
	doc/man/man9lis/lis_msg_type_name.9lis.man \
	doc/man/man9lis/lis_msgutil.9lis.man \
	doc/man/man9lis/lis_noenable.9lis.man \
	doc/man/man9lis/lis_num_cpus.9lis.man \
	doc/man/man9lis/lis_old_inode.9lis.man \
	doc/man/man9lis/lis_open_cnt.9lis.man \
	doc/man/man9lis/lis_osif_cli.9lis.man \
	doc/man/man9lis/lis_osif_do_gettimeofday.9lis.man \
	doc/man/man9lis/lis_osif_do_settimeofday.9lis.man \
	doc/man/man9lis/lis_osif_pci_alloc_consistent.9lis.man \
	doc/man/man9lis/lis_osif_pci_dac_dma_supported.9lis.man \
	doc/man/man9lis/lis_osif_pci_dac_dma_sync_single.9lis.man \
	doc/man/man9lis/lis_osif_pci_dac_dma_to_offset.9lis.man \
	doc/man/man9lis/lis_osif_pci_dac_dma_to_page.9lis.man \
	doc/man/man9lis/lis_osif_pci_dac_page_to_dma.9lis.man \
	doc/man/man9lis/lis_osif_pci_dac_set_dma_mask.9lis.man \
	doc/man/man9lis/lis_osif_pci_disable_device.9lis.man \
	doc/man/man9lis/lis_osif_pci_dma_supported.9lis.man \
	doc/man/man9lis/lis_osif_pci_dma_sync_sg.9lis.man \
	doc/man/man9lis/lis_osif_pci_dma_sync_single.9lis.man \
	doc/man/man9lis/lis_osif_pci_enable_device.9lis.man \
	doc/man/man9lis/lis_osif_pci_find_class.9lis.man \
	doc/man/man9lis/lis_osif_pci_find_device.9lis.man \
	doc/man/man9lis/lis_osif_pci_find_slot.9lis.man \
	doc/man/man9lis/lis_osif_pci_free_consistent.9lis.man \
	doc/man/man9lis/lis_osif_pci_map_page.9lis.man \
	doc/man/man9lis/lis_osif_pci_map_sg.9lis.man \
	doc/man/man9lis/lis_osif_pci_map_single.9lis.man \
	doc/man/man9lis/lis_osif_pci_module_init.9lis.man \
	doc/man/man9lis/lis_osif_pci_read_config_byte.9lis.man \
	doc/man/man9lis/lis_osif_pci_read_config_dword.9lis.man \
	doc/man/man9lis/lis_osif_pci_read_config_word.9lis.man \
	doc/man/man9lis/lis_osif_pci_set_dma_mask.9lis.man \
	doc/man/man9lis/lis_osif_pci_set_master.9lis.man \
	doc/man/man9lis/lis_osif_pci_unmap_page.9lis.man \
	doc/man/man9lis/lis_osif_pci_unmap_sg.9lis.man \
	doc/man/man9lis/lis_osif_pci_unmap_single.9lis.man \
	doc/man/man9lis/lis_osif_pci_unregister_driver.9lis.man \
	doc/man/man9lis/lis_osif_pci_write_config_byte.9lis.man \
	doc/man/man9lis/lis_osif_pci_write_config_dword.9lis.man \
	doc/man/man9lis/lis_osif_pci_write_config_word.9lis.man \
	doc/man/man9lis/lis_osif_sg_dma_address.9lis.man \
	doc/man/man9lis/lis_osif_sg_dma_len.9lis.man \
	doc/man/man9lis/lis_osif_sti.9lis.man \
	doc/man/man9lis/lis_OTHER.9lis.man \
	doc/man/man9lis/lis_OTHERQ.9lis.man \
	doc/man/man9lis/lis_own_spl.9lis.man \
	doc/man/man9lis/lis_pcibios_find_class.9lis.man \
	doc/man/man9lis/lis_pcibios_find_device.9lis.man \
	doc/man/man9lis/lis_pcibios_init.9lis.man \
	doc/man/man9lis/lis_pcibios_present.9lis.man \
	doc/man/man9lis/lis_pcibios_read_config_byte.9lis.man \
	doc/man/man9lis/lis_pcibios_read_config_dword.9lis.man \
	doc/man/man9lis/lis_pcibios_read_config_word.9lis.man \
	doc/man/man9lis/lis_pcibios_strerror.9lis.man \
	doc/man/man9lis/lis_pcibios_write_config_byte.9lis.man \
	doc/man/man9lis/lis_pcibios_write_config_dword.9lis.man \
	doc/man/man9lis/lis_pcibios_write_config_word.9lis.man \
	doc/man/man9lis/lis_pci_cleanup.9lis.man \
	doc/man/man9lis/lis_pci_disable_device.9lis.man \
	doc/man/man9lis/lis_pci_enable_device.9lis.man \
	doc/man/man9lis/lis_pci_find_class.9lis.man \
	doc/man/man9lis/lis_pci_find_device.9lis.man \
	doc/man/man9lis/lis_pci_find_slot.9lis.man \
	doc/man/man9lis/lis_pci_read_config_byte.9lis.man \
	doc/man/man9lis/lis_pci_read_config_dword.9lis.man \
	doc/man/man9lis/lis_pci_read_config_word.9lis.man \
	doc/man/man9lis/lis_pci_set_master.9lis.man \
	doc/man/man9lis/lis_pci_write_config_byte.9lis.man \
	doc/man/man9lis/lis_pci_write_config_dword.9lis.man \
	doc/man/man9lis/lis_pci_write_config_word.9lis.man \
	doc/man/man9lis/lis_phys_to_virt.9lis.man \
	doc/man/man9lis/lis_pipe.9lis.man \
	doc/man/man9lis/lis_poll_2_1.9lis.man \
	doc/man/man9lis/lis_poll_bits.9lis.man \
	doc/man/man9lis/lis_poll_events.9lis.man \
	doc/man/man9lis/lis_poll_file.9lis.man \
	doc/man/man9lis/lis_print_block.9lis.man \
	doc/man/man9lis/lis_print_data.9lis.man \
	doc/man/man9lis/lis_printk.9lis.man \
	doc/man/man9lis/lis_print_mem.9lis.man \
	doc/man/man9lis/lis_print_msg.9lis.man \
	doc/man/man9lis/lis_print_queue.9lis.man \
	doc/man/man9lis/lis_print_queues.9lis.man \
	doc/man/man9lis/lis_print_spl_track.9lis.man \
	doc/man/man9lis/lis_print_stream.9lis.man \
	doc/man/man9lis/lis_pullupmsg.9lis.man \
	doc/man/man9lis/lis_putbq.9lis.man \
	doc/man/man9lis/lis_putbyte.9lis.man \
	doc/man/man9lis/lis_putctl1.9lis.man \
	doc/man/man9lis/lis_putctl.9lis.man \
	doc/man/man9lis/lis_putnext.9lis.man \
	doc/man/man9lis/lis_putnextctl1.9lis.man \
	doc/man/man9lis/lis_putnextctl.9lis.man \
	doc/man/man9lis/lis_putq.9lis.man \
	doc/man/man9lis/lis_qattach.9lis.man \
	doc/man/man9lis/lis_qclose.9lis.man \
	doc/man/man9lis/lis_qcountstrm.9lis.man \
	doc/man/man9lis/lis_qdetach.9lis.man \
	doc/man/man9lis/lis_qenable.9lis.man \
	doc/man/man9lis/lis_qprocsoff.9lis.man \
	doc/man/man9lis/lis_qprocson.9lis.man \
	doc/man/man9lis/lis_qreply.9lis.man \
	doc/man/man9lis/lis_qsize.9lis.man \
	doc/man/man9lis/lis_queue_name.9lis.man \
	doc/man/man9lis/lis_queuerun_cnts.9lis.man \
	doc/man/man9lis/lis_queues_running.9lis.man \
	doc/man/man9lis/lis_RD.9lis.man \
	doc/man/man9lis/lis_recvfd.9lis.man \
	doc/man/man9lis/lis_register_strdev.9lis.man \
	doc/man/man9lis/lis_register_strmod.9lis.man \
	doc/man/man9lis/lis_release_region.9lis.man \
	doc/man/man9lis/lis_request_dma.9lis.man \
	doc/man/man9lis/lis_request_irq.9lis.man \
	doc/man/man9lis/lis_request_region.9lis.man \
	doc/man/man9lis/lis_rmvb.9lis.man \
	doc/man/man9lis/lis_rmvq.9lis.man \
	doc/man/man9lis/lis_runq_cnts.9lis.man \
	doc/man/man9lis/lis_runq_req_cnt.9lis.man \
	doc/man/man9lis/lis_rw_lock_alloc_fcn.9lis.man \
	doc/man/man9lis/lis_rw_lock_free_fcn.9lis.man \
	doc/man/man9lis/lis_rw_lock_init_fcn.9lis.man \
	doc/man/man9lis/lis_rw_read_lock_fcn.9lis.man \
	doc/man/man9lis/lis_rw_read_lock_irq_fcn.9lis.man \
	doc/man/man9lis/lis_rw_read_lock_irqsave_fcn.9lis.man \
	doc/man/man9lis/lis_rw_read_unlock_fcn.9lis.man \
	doc/man/man9lis/lis_rw_read_unlock_irq_fcn.9lis.man \
	doc/man/man9lis/lis_rw_read_unlock_irqrestore_fcn.9lis.man \
	doc/man/man9lis/lis_rw_write_lock_fcn.9lis.man \
	doc/man/man9lis/lis_rw_write_lock_irq_fcn.9lis.man \
	doc/man/man9lis/lis_rw_write_lock_irqsave_fcn.9lis.man \
	doc/man/man9lis/lis_rw_write_unlock_fcn.9lis.man \
	doc/man/man9lis/lis_rw_write_unlock_irq_fcn.9lis.man \
	doc/man/man9lis/lis_rw_write_unlock_irqrestore_fcn.9lis.man \
	doc/man/man9lis/lis_safe_canenable.9lis.man \
	doc/man/man9lis/lis_safe_enableok.9lis.man \
	doc/man/man9lis/lis_safe_noenable.9lis.man \
	doc/man/man9lis/lis_safe_OTHERQ.9lis.man \
	doc/man/man9lis/lis_safe_putmsg.9lis.man \
	doc/man/man9lis/lis_safe_putnext.9lis.man \
	doc/man/man9lis/lis_safe_qreply.9lis.man \
	doc/man/man9lis/lis_safe_RD.9lis.man \
	doc/man/man9lis/lis_safe_SAMESTR.9lis.man \
	doc/man/man9lis/lis_safe_WR.9lis.man \
	doc/man/man9lis/lis_SAMESTR.9lis.man \
	doc/man/man9lis/lis_secs.9lis.man \
	doc/man/man9lis/lis_sem_alloc.9lis.man \
	doc/man/man9lis/lis_sem_destroy.9lis.man \
	doc/man/man9lis/lis_sem_init.9lis.man \
	doc/man/man9lis/lis_sendfd.9lis.man \
	doc/man/man9lis/lis_set_file_str.9lis.man \
	doc/man/man9lis/lis_setq.9lis.man \
	doc/man/man9lis/lis_setqsched.9lis.man \
	doc/man/man9lis/lis_setqsched_cnts.9lis.man \
	doc/man/man9lis/lis_setqsched_isr_cnts.9lis.man \
	doc/man/man9lis/lis_sleep_on.9lis.man \
	doc/man/man9lis/lis_spin_is_locked_fcn.9lis.man \
	doc/man/man9lis/lis_spin_lock_alloc_fcn.9lis.man \
	doc/man/man9lis/lis_spin_lock_fcn.9lis.man \
	doc/man/man9lis/lis_spin_lock_free_fcn.9lis.man \
	doc/man/man9lis/lis_spin_lock_init_fcn.9lis.man \
	doc/man/man9lis/lis_spin_lock_irq_fcn.9lis.man \
	doc/man/man9lis/lis_spin_lock_irqsave_fcn.9lis.man \
	doc/man/man9lis/lis_spin_trylock_fcn.9lis.man \
	doc/man/man9lis/lis_spin_unlock_fcn.9lis.man \
	doc/man/man9lis/lis_spin_unlock_irq_fcn.9lis.man \
	doc/man/man9lis/lis_spin_unlock_irqrestore_fcn.9lis.man \
	doc/man/man9lis/lis_spl0_fcn.9lis.man \
	doc/man/man9lis/lis_splstr_fcn.9lis.man \
	doc/man/man9lis/lis_splx_fcn.9lis.man \
	doc/man/man9lis/lis_sprintf.9lis.man \
	doc/man/man9lis/lis_stdata_cnt.9lis.man \
	doc/man/man9lis/lis_strclose.9lis.man \
	doc/man/man9lis/lis_strcount.9lis.man \
	doc/man/man9lis/lis_stream_utils.9lis.man \
	doc/man/man9lis/lis_strgetpmsg.9lis.man \
	doc/man/man9lis/lis_strioctl.9lis.man \
	doc/man/man9lis/lis_strm_name.9lis.man \
	doc/man/man9lis/lis_strm_name_from_queue.9lis.man \
	doc/man/man9lis/lis_stropen.9lis.man \
	doc/man/man9lis/lis_stropts_file.9lis.man \
	doc/man/man9lis/lis_strputpmsg.9lis.man \
	doc/man/man9lis/lis_strqget.9lis.man \
	doc/man/man9lis/lis_strqset.9lis.man \
	doc/man/man9lis/lis_strread.9lis.man \
	doc/man/man9lis/lis_strstats.9lis.man \
	doc/man/man9lis/lis_strwrite.9lis.man \
	doc/man/man9lis/lis_testb.9lis.man \
	doc/man/man9lis/lis_thread_start.9lis.man \
	doc/man/man9lis/lis_thread_stop.9lis.man \
	doc/man/man9lis/lis_timeout.9lis.man \
	doc/man/man9lis/lis_timeout_fcn.9lis.man \
	doc/man/man9lis/lis_udelay.9lis.man \
	doc/man/man9lis/lis_umount2.9lis.man \
	doc/man/man9lis/lis_umount.9lis.man \
	doc/man/man9lis/lis_unbufcall.9lis.man \
	doc/man/man9lis/lis_unlink.9lis.man \
	doc/man/man9lis/lis_unlinkb.9lis.man \
	doc/man/man9lis/lis_unregister_strdev.9lis.man \
	doc/man/man9lis/lis_unregister_strmod.9lis.man \
	doc/man/man9lis/lis_untimeout.9lis.man \
	doc/man/man9lis/lis_up_fcn.9lis.man \
	doc/man/man9lis/lis_usecs.9lis.man \
	doc/man/man9lis/lis_usectohz.9lis.man \
	doc/man/man9lis/lis_valid_mod_list.9lis.man \
	doc/man/man9lis/lis_version.9lis.man \
	doc/man/man9lis/lis_vfree.9lis.man \
	doc/man/man9lis/lis_virt_to_phys.9lis.man \
	doc/man/man9lis/lis_vmalloc.9lis.man \
	doc/man/man9lis/lis_vremap.9lis.man \
	doc/man/man9lis/lis_vsprintf.9lis.man \
	doc/man/man9lis/lis_wake_up.9lis.man \
	doc/man/man9lis/lis_wake_up_interruptible.9lis.man \
	doc/man/man9lis/lis_WR.9lis.man \
	doc/man/man9lis/lis_xmsgsize.9lis.man \
	doc/man/man9lis/lis_zmalloc.9lis.man \
	doc/man/man9/lis_mknod.9.man \
	doc/man/man9/lis_mount.9.man \
	doc/man/man9/lis_OTHER.9.man \
	doc/man/man9/lis_register_strdev.9.man \
	doc/man/man9/lis_register_strmod.9.man \
	doc/man/man9/lis_umount2.9.man \
	doc/man/man9/lis_umount.9.man \
	doc/man/man9/lis_unregister_strdev.9.man \
	doc/man/man9/lis_unregsiter_strmod.9.man \
	doc/man/man9/lis_version.9.man \
	doc/man/man9/LOCK.9.man \
	doc/man/man9/LOCK_ALLOC.9.man \
	doc/man/man9/LOCK_DEALLOC.9.man \
	doc/man/man9/LOCK_OWNED.9.man \
	doc/man/man9/major.9.man \
	doc/man/man9/makedev.9.man \
	doc/man/man9/makedevice.9.man \
	doc/man/man9/mapinit.9.man \
	doc/man/man9/max.9.man \
	doc/man/man9/mblk_t.9.man \
	doc/man/man9/mi_bufcall.9.man \
	doc/man/man9/mi_close_comm.9.man \
	doc/man/man9/min.9.man \
	doc/man/man9/mi_next_ptr.9.man \
	doc/man/man9/minor.9.man \
	doc/man/man9/mi_open_comm.9.man \
	doc/man/man9/mknod.9.man \
	doc/man/man9/mod_info.9.man \
	doc/man/man9/mod_install.9.man \
	doc/man/man9/mod_remove.9.man \
	doc/man/man9/module_info.9.man \
	doc/man/man9/mount.9.man \
	doc/man/man9/mp-streams.9.man \
	doc/man/man9/MPSTR_QLOCK.9.man \
	doc/man/man9/MPSTR_QRELE.9.man \
	doc/man/man9/MPSTR_STPLOCK.9.man \
	doc/man/man9/MPSTR_STPRELE.9.man \
	doc/man/man9/msgb.9.man \
	doc/man/man9/msgdsize.9.man \
	doc/man/man9/msgphysreq.9.man \
	doc/man/man9/msgpullup.9.man \
	doc/man/man9/msgpullup_physreq.9.man \
	doc/man/man9/msgscgth.9.man \
	doc/man/man9/msgsize.9.man \
	doc/man/man9/msgutil.9.man \
	doc/man/man9/noenable.9.man \
	doc/man/man9/OTHER.9.man \
	doc/man/man9/OTHERQ.9.man \
	doc/man/man9/pcmsg.9.man \
	doc/man/man9/physreq.9.man \
	doc/man/man9/physreq_t.9.man \
	doc/man/man9/pipe.9.man \
	doc/man/man9/power.9.man \
	doc/man/man9/printf.9.man \
	doc/man/man9/probe.9.man \
	doc/man/man9/pullupmsg.9.man \
	doc/man/man9/put.9.man \
	doc/man/man9/putbq.9.man \
	doc/man/man9/putctl1.9.man \
	doc/man/man9/putctl2.9.man \
	doc/man/man9/putctl.9.man \
	doc/man/man9/puthere.9.man \
	doc/man/man9/putnext.9.man \
	doc/man/man9/putnextctl1.9.man \
	doc/man/man9/putnextctl2.9.man \
	doc/man/man9/putnextctl.9.man \
	doc/man/man9/putq.9.man \
	doc/man/man9/qattach.9.man \
	doc/man/man9/qband.9.man \
	doc/man/man9/qbufcall.9.man \
	doc/man/man9/qclose.9.man \
	doc/man/man9/qcountstrm.9.man \
	doc/man/man9/qdetach.9.man \
	doc/man/man9/qenable.9.man \
	doc/man/man9/qinit.9.man \
	doc/man/man9/qinsert.9.man \
	doc/man/man9/qopen.9.man \
	doc/man/man9/qprocsoff.9.man \
	doc/man/man9/qprocson.9.man \
	doc/man/man9/qreply.9.man \
	doc/man/man9/qsize.9.man \
	doc/man/man9/qtimeout.9.man \
	doc/man/man9/queclass.9.man \
	doc/man/man9/queinfo.9.man \
	doc/man/man9/queue.9.man \
	doc/man/man9/queue_t.9.man \
	doc/man/man9/qunbufcall.9.man \
	doc/man/man9/quntimeout.9.man \
	doc/man/man9/qwait.9.man \
	doc/man/man9/qwait_sig.9.man \
	doc/man/man9/qwriter.9.man \
	doc/man/man9/RD.9.man \
	doc/man/man9/register_inode.9.man \
	doc/man/man9/register_inode_major.9.man \
	doc/man/man9/register_inode_minor.9.man \
	doc/man/man9/register_strdev.9.man \
	doc/man/man9/register_strdev_major.9.man \
	doc/man/man9/register_strdev_minor.9.man \
	doc/man/man9/register_strmod.9.man \
	doc/man/man9/rmvb.9.man \
	doc/man/man9/rmvq.9.man \
	doc/man/man9/RW_ALLOC.9.man \
	doc/man/man9/RW_DEALLOC.9.man \
	doc/man/man9/RW_RDLOCK.9.man \
	doc/man/man9/RW_TRYRDLOCK.9.man \
	doc/man/man9/RW_TRYWRLOCK.9.man \
	doc/man/man9/RW_UNLOCK.9.man \
	doc/man/man9/RW_WRLOCK.9.man \
	doc/man/man9/SAMESTR.9.man \
	doc/man/man9/setq.9.man \
	doc/man/man9/sleep.9.man \
	doc/man/man9/SLEEP_ALLOC.9.man \
	doc/man/man9/SLEEP_DEALLOC.9.man \
	doc/man/man9/SLEEP_LOCK.9.man \
	doc/man/man9/SLEEP_LOCKAVAIL.9.man \
	doc/man/man9/SLEEP_LOCKOWNED.9.man \
	doc/man/man9/SLEEP_LOCK_SIG.9.man \
	doc/man/man9/SLEEP_TRYLOCK.9.man \
	doc/man/man9/SLEEP_UNLOCK.9.man \
	doc/man/man9/soon.9.man \
	doc/man/man9/SPG.9.man \
	doc/man/man9/spl0.9.man \
	doc/man/man9/spl1.9.man \
	doc/man/man9/spl2.9.man \
	doc/man/man9/spl3.9.man \
	doc/man/man9/spl4.9.man \
	doc/man/man9/spl5.9.man \
	doc/man/man9/spl6.9.man \
	doc/man/man9/spl7.9.man \
	doc/man/man9/spl.9.man \
	doc/man/man9/splstr.9.man \
	doc/man/man9/splx.9.man \
	doc/man/man9/STREAMS.9.man \
	doc/man/man9/streams_close_comm.9.man \
	doc/man/man9/streams_get_sleep_lock.9.man \
	doc/man/man9/streams_open_comm.9.man \
	doc/man/man9/streams_open_ocomm.9.man \
	doc/man/man9/streams_put.9.man \
	doc/man/man9/streamtab.9.man \
	doc/man/man9/stream_utils.9.man \
	doc/man/man9/str_install.9.man \
	doc/man/man9/str_install_AIX.9.man \
	doc/man/man9/str_install_HPUX.9.man \
	doc/man/man9/strioccall.9.man \
	doc/man/man9/strlog.9.man \
	doc/man/man9/strmod_add.9.man \
	doc/man/man9/strmod_del.9.man \
	doc/man/man9/strqget.9.man \
	doc/man/man9/strqset.9.man \
	doc/man/man9/str_uninstall.9.man \
	doc/man/man9/SV_ALLOC.9.man \
	doc/man/man9/SV_BROADCAST.9.man \
	doc/man/man9/SV_DEALLOC.9.man \
	doc/man/man9/SV_SIGNAL.9.man \
	doc/man/man9/SV_WAIT.9.man \
	doc/man/man9/SV_WAIT_SIG.9.man \
	doc/man/man9/testb.9.man \
	doc/man/man9/time.9.man \
	doc/man/man9/timeout.9.man \
	doc/man/man9/TRYLOCK.9.man \
	doc/man/man9/umount2.9.man \
	doc/man/man9/umount.9.man \
	doc/man/man9/unbufcall.9.man \
	doc/man/man9/unfreezestr.9.man \
	doc/man/man9/unlink.9.man \
	doc/man/man9/unlinkb.9.man \
	doc/man/man9/UNLOCK.9.man \
	doc/man/man9/unregister_inode.9.man \
	doc/man/man9/unregister_inode_major.9.man \
	doc/man/man9/unregister_inode_minor.9.man \
	doc/man/man9/unregister_strdev.9.man \
	doc/man/man9/unregister_strdev_major.9.man \
	doc/man/man9/unregister_strdev_minor.9.man \
	doc/man/man9/unregister_strmod.9.man \
	doc/man/man9/untimeout.9.man \
	doc/man/man9/unweldq.9.man \
	doc/man/man9/vtop.9.man \
	doc/man/man9/wakeup.9.man \
	doc/man/man9/wantio.9.man \
	doc/man/man9/wantmsg.9.man \
	doc/man/man9/weldq.9.man \
	doc/man/man9/WR.9.man \
	doc/man/man9/xmsgsize.9.man



include am/man.am

EXPORT_SRCFILES = \
	src/compat/aix/aixcompat.c \
	src/compat/hpux/hpuxcompat.c \
	src/compat/lis/liscompat.c \
	src/compat/osf/osfcompat.c \
	src/compat/sun/suncompat.c \
	src/compat/svr4/svr4compat.c \
	src/compat/uw7/uw7compat.c \
	src/drivers/strpipe.c \
	src/kernel/strsyms.c

EXPORT_VERFILES = \
	include/sys/versions/aixcompat.ver \
	include/sys/versions/hpuxcompat.ver \
	include/sys/versions/liscompat.ver \
	include/sys/versions/osfcompat.ver \
	include/sys/versions/suncompat.ver \
	include/sys/versions/svr4compat.ver \
	include/sys/versions/uw7compat.ver \
	include/sys/versions/strpipe.ver \
	include/sys/versions/strsyms.ver

PKGINCL = include/sys

GENKSYMS_CPPFLAGS = \
	-I./src/compat -I@srcdir@/src/compat \
	-I./src/kernel -I@srcdir@/src/kernel \
	-I./src/drivers -I@srcdir@/src/drivers \
	-I./src/modules -I@srcdir@/src/modules \
	$(LFS_INCLUDES) $(KERNEL_CPPFLAGS)

include am/genksyms.am

nobase_pkginclude_HEADERS = include/stropts.h include/tihdr.h include/timod.h \
	include/sys/aixddi.h include/sys/cmn_err.h include/sys/ddi.h include/sys/debug.h include/sys/dki.h \
	include/sys/hpuxddi.h include/sys/kmem.h include/sys/lisddi.h include/sys/osfddi.h include/sys/sad.h \
	include/sys/sc.h \
	include/sys/sockmod.h include/sys/spec_fs_i.h include/sys/strconf.h include/sys/strdebug.h \
	include/sys/stream.h include/sys/strlog.h include/sys/stropts.h include/sys/strsubr.h include/sys/sunddi.h \
	include/sys/svr4ddi.h include/sys/tihdr.h include/sys/timod.h include/sys/uw7ddi.h \
	include/sys/modversions.h \
	include/sys/versions/aixcompat.ver \
	include/sys/versions/hpuxcompat.ver \
	include/sys/versions/liscompat.ver \
	include/sys/versions/osfcompat.ver \
	include/sys/versions/suncompat.ver \
	include/sys/versions/svr4compat.ver \
	include/sys/versions/uw7compat.ver \
	include/sys/versions/strpipe.ver \
	include/sys/versions/strsyms.ver

nobase_include_HEADERS = include/xti.h include/xti_inet.h \
	include/xti/config.h include/xti/tihdr.h include/xti/timod.h include/xti/xti_atm.h include/xti/xti.h \
	include/xti/xti_inet.h include/xti/xti_ip.h include/xti/xti_local.h include/xti/xti_mosi.h \
	include/xti/xti_osi.h include/xti/xti_sctp.h include/xti/xti_tcp.h include/xti/xti_udp.h

include am/headers.am

noinst_SCRIPTS = \
	$(STRCONF_SCRIPT)

install-devices: $(STRMAKENODES) $(INET_MKDEV)
	$(STRMAKENODES)
	DESTDIR=$(DESTDIR) $(INET_MKDEV)

uninstall-devices: $(STRMAKENODES) $(INET_RMDEV)
	$(STRMAKENODES) --remove
	DESTDIR=$(DESTDIR) $(INET_RMDEV)

EXTRA_DIST = streams.spec.in streams.spec doc/man/streams.refs doc/man/streams.macros \
	$(noinst_SCRIPTS) $(sbin_SCRIPTS) $(lfs_configs) openss7-logo-small.gif openss7-logo-small.xpm \
	$(INET_MKDEV) $(INET_RMDEV) m4/kernel.m4 m4/rpm.m4 $(noinst_man_MANS)

SUBDIRS = intl m4 po doc

#
# Rules for making rpms
#
AM_RPMFLAGS = --define "_kversion @PACKAGE_KVERSION@"
AM_SRPMFLAGS =

include am/rpm.am

#
# General rules
#
include am/openss7.am

# vim: ft=automake
