# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2004/04/05 12:39:05 $
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
# Last Modified $Date: 2004/04/05 12:39:05 $ by $Author: brian $
#
# =============================================================================

if PUBLIC_RELEASE

libexec_PROGRAMS		= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp \
				  test-tirdwr \
				  test-timod \
				  test-xnet \
				  test-xnet_thr

EXTRA_PROGRAMS			= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp \
				  test-tirdwr \
				  test-timod \
				  test-xnet \
				  test-xnet_thr

sbin_PROGRAMS			= \
				  xnetmakenodes

xnetmakenodes_SOURCES		= xnetmakenodes.c
xnetmakenodes_DEPENDENCIES	= strconf.h
xnetmakenodes_LDDADD		=
xnetmakenodes_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
xnetmakenodes_CFLAGS		= @USER_CFLAGS@

test_inet_raw_SOURCES		= src/test/test-inet_raw.c
test_inet_raw_LDFLAGS		= -lLiS
test_inet_raw_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_inet_raw_CFLAGS		= @USER_CFLAGS@

test_inet_tcp_SOURCES		= src/test/test-inet_tcp.c
test_inet_tcp_LDFLAGS		= -lLiS
test_inet_tcp_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_inet_tcp_CFLAGS		= @USER_CFLAGS@

test_inet_udp_SOURCES		= src/test/test-inet_udp.c
test_inet_udp_LDFLAGS		= -lLiS
test_inet_udp_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_inet_udp_CFLAGS		= @USER_CFLAGS@

test_tirdwr_SOURCES		= src/test/test-tirdwr.c
test_tirdwr_LDFLAGS		= -lLiS
test_tirdwr_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_tirdwr_CFLAGS		= @USER_CFLAGS@

test_timod_SOURCES		= src/test/test-timod.c
test_timod_LDFLAGS		= -lLiS
test_timod_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_timod_CFLAGS		= @USER_CFLAGS@

test_xnet_SOURCES		= src/test/test-xnet.c
test_xnet_LDFLAGS		= -lLiS -lxnet
test_xnet_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_xnet_CFLAGS		= @USER_CFLAGS@

test_xnet_thr_SOURCES		= src/test/test-xnet.c
test_xnet_thr_LDFLAGS		= -lLiS -lpthread -lxnet
test_xnet_thr_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@ -D_REENTRANT
test_xnet_thr_CFLAGS		= @USER_CFLAGS@

ADDON_LIBS			= 
ADDON_OBJS			= 

lib_LTLIBRARIES			= libxnet.la

libxnet_la_SOURCES		= src/lib/xnet.c gettext.h
libxnet_la_LDFLAGS		= -lLiS -version-info 0:0:0
libxnet_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib @XNET_INCLUDES@ @USER_CPPFLAGS@ -I$(top_srcdir)/include/xti -I$(top_builddir)/include/xti
libxnet_la_CFLAGS		= @USER_CFLAGS@
libxnet_la_LIBADD		= 

noinst_LIBRARIES		= libXNETmods.a libXNETdrvs.a $(ADDON_LIBS)
EXTRA_LIBRARIES			= 

nodist_EXTRA_libXNETmods_a_SOURCES = \
				  src/modules/tirdwr.c \
				  src/modules/timod.c \
				  ./config.h ./strconf.h

mods_source_FILES		= $(nodist_EXTRA_libXNETmods_a_SOURCES)

nodist_libXNETmods_a_SOURCES	= $(mods_source_FILES)

libXNETmods_a_DEPENDENCIES	= ./config.h ./strconf.h
libXNETmods_a_CPPFLAGS		= -I./src/modules -I@srcdir@/src/modules -imacros ./strconf.h @XNET_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libXNETmods_a_CFLAGS		= @KERNEL_CFLAGS@

nodist_EXTRA_libXNETdrvs_a_SOURCES = \
				  src/drivers/inet.c \
				  src/drivers/bufq.h \
				  src/drivers/debug.h \
				  ./config.h ./strconf.h

drvs_source_FILES		= $(nodist_EXTRA_libXNETdrvs_a_SOURCES)

nodist_libXNETdrvs_a_SOURCES	= $(drvs_source_FILES)

libXNETdrvs_a_DEPENDENCIES	= ./config.h ./strconf.h
libXNETdrvs_a_CPPFLAGS		= -I./src/drivers -I@srcdir@/src/drivers -imacros ./strconf.h @XNET_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libXNETdrvs_a_CFLAGS		= @KERNEL_CFLAGS@

KERNEL_MODULES			= $(libXNETmods_a_OBJECTS) $(libXNETdrvs_a_OBJECTS) $(ADDON_OBJS)
EXTRA_KERNEL_MODULES		= 

else

libexec_PROGRAMS		= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp \
				  test-tirdwr \
				  test-timod \
				  test-xnet \
				  test-xnet_thr

EXTRA_PROGRAMS			= \
				  test-inet_raw \
				  test-inet_tcp \
				  test-inet_udp \
				  test-tirdwr \
				  test-timod \
				  test-xnet \
				  test-xnet_thr

sbin_PROGRAMS			= \
				  xnetmakenodes

xnetmakenodes_SOURCES		= xnetmakenodes.c
xnetmakenodes_DEPENDENCIES	= strconf.h
xnetmakenodes_LDDADD		=
xnetmakenodes_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
xnetmakenodes_CFLAGS		= @USER_CFLAGS@

test_inet_raw_SOURCES		= src/test/test-inet_raw.c
test_inet_raw_LDFLAGS		= -lLiS
test_inet_raw_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_inet_raw_CFLAGS		= @USER_CFLAGS@

test_inet_tcp_SOURCES		= src/test/test-inet_tcp.c
test_inet_tcp_LDFLAGS		= -lLiS
test_inet_tcp_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_inet_tcp_CFLAGS		= @USER_CFLAGS@

test_inet_udp_SOURCES		= src/test/test-inet_udp.c
test_inet_udp_LDFLAGS		= -lLiS
test_inet_udp_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_inet_udp_CFLAGS		= @USER_CFLAGS@

test_tirdwr_SOURCES		= src/test/test-tirdwr.c
test_tirdwr_LDFLAGS		= -lLiS
test_tirdwr_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_tirdwr_CFLAGS		= @USER_CFLAGS@

test_timod_SOURCES		= src/test/test-timod.c
test_timod_LDFLAGS		= -lLiS
test_timod_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_timod_CFLAGS		= @USER_CFLAGS@

test_xnet_SOURCES		= src/test/test-xnet.c
test_xnet_LDFLAGS		= -lLiS -lxnet
test_xnet_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@
test_xnet_CFLAGS		= @USER_CFLAGS@

test_xnet_thr_SOURCES		= src/test/test-xnet.c
test_xnet_thr_LDFLAGS		= -lLiS -lpthread -lxnet
test_xnet_thr_CPPFLAGS		= @XNET_INCLUDES@ @USER_CPPFLAGS@ -D_REENTRANT
test_xnet_thr_CFLAGS		= @USER_CFLAGS@

ADDON_LIBS			= 
ADDON_OBJS			= 

lib_LTLIBRARIES			= libxnet.la

libxnet_la_SOURCES		= src/lib/xnet.c gettext.h
libxnet_la_LDFLAGS		= -lLiS -version-info 0:0:0
libxnet_la_CPPFLAGS		= -I$(top_srcdir)/src/lib -I$(top_builddir)/src/lib @XNET_INCLUDES@ @USER_CPPFLAGS@ -I$(top_srcdir)/include/xti -I$(top_builddir)/include/xti
libxnet_la_CFLAGS		= @USER_CFLAGS@
libxnet_la_LIBADD		= 

noinst_LIBRARIES		= libXNETmods.a $(ADDON_LIBS)
EXTRA_LIBRARIES			= 

nodist_EXTRA_libXNETmods_a_SOURCES = \
				  src/modules/tirdwr.c \
				  src/modules/timod.c \
				  ./config.h ./strconf.h

mods_source_FILES		= $(nodist_EXTRA_libXNETmods_a_SOURCES)

nodist_libXNETmods_a_SOURCES	= $(mods_source_FILES)

libXNETmods_a_DEPENDENCIES	= ./config.h ./strconf.h
libXNETmods_a_CPPFLAGS		= -I./src/modules -I@srcdir@/src/modules -imacros ./strconf.h @XNET_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libXNETmods_a_CFLAGS		= @KERNEL_CFLAGS@

nodist_EXTRA_libXNETdrvs_a_SOURCES = \
				  src/drivers/inet.c \
				  src/drivers/bufq.h \
				  src/drivers/debug.h \
				  ./config.h ./strconf.h

drvs_source_FILES		= $(nodist_EXTRA_libXNETdrvs_a_SOURCES)

nodist_libXNETdrvs_a_SOURCES	= $(drvs_source_FILES)

libXNETdrvs_a_DEPENDENCIES	= ./config.h ./strconf.h
libXNETdrvs_a_CPPFLAGS		= -I./src/drivers -I@srcdir@/src/drivers -imacros ./strconf.h @XNET_INCLUDES@ @KERNEL_CPPFLAGS@ @KERNEL_MODFLAGS@
libXNETdrvs_a_CFLAGS		= @KERNEL_CFLAGS@

KERNEL_MODULES			= $(libXNETmods_a_OBJECTS) $(libXNETdrvs_a_OBJECTS) $(ADDON_OBJS)
EXTRA_KERNEL_MODULES		= 

endif

include am/libraries.am

KERNEL_MODCONF			= conf.modules
KERNEL_SUBDIR			= xnet
KMODCONF_EXT			= xnet

MANMACRO_PFX			= strxnet
mansrcdir			= $(srcdir)/doc/man

include am/kernel.am

nodist_man_MANS = \
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
	doc/man/man3/xti.3 \
	doc/man/man3/xxx.3 \
	doc/man/man4/timod.4 \
	doc/man/man4/tirdwr.4 \
	doc/man/man4/xxx.4 \
	doc/man/man5/xxx.5 \
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
	doc/man/man7/T_UDERROR_IND.7 \
	doc/man/man7/T_uderror_ind.7 \
	doc/man/man7/T_UNBIND_REQ.7 \
	doc/man/man7/T_unbind_req.7 \
	doc/man/man7/T_UNITDATA_IND.7 \
	doc/man/man7/T_unitdata_ind.7 \
	doc/man/man7/T_UNITDATA_REQ.7 \
	doc/man/man7/T_unitdata_req.7 \
	doc/man/man7/xxx.7 \
	doc/man/man8/xxx.8

noinst_man_MANS = \
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
	doc/man/man3/xti.3.man \
	doc/man/man3/xxx.3.man \
	doc/man/man4/timod.4.man \
	doc/man/man4/tirdwr.4.man \
	doc/man/man4/xxx.4.man \
	doc/man/man5/xxx.5.man \
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
	doc/man/man7/T_UDERROR_IND.7.man \
	doc/man/man7/T_uderror_ind.7.man \
	doc/man/man7/T_UNBIND_REQ.7.man \
	doc/man/man7/T_unbind_req.7.man \
	doc/man/man7/T_UNITDATA_IND.7.man \
	doc/man/man7/T_unitdata_ind.7.man \
	doc/man/man7/T_UNITDATA_REQ.7.man \
	doc/man/man7/T_unitdata_req.7.man \
	doc/man/man7/xxx.7.man \
	doc/man/man8/xxx.8.man

include am/man.am

nobase_include_HEADERS = \
	src/include/ticlts.h \
	src/include/ticots.h \
	src/include/ticotsord.h \
	src/include/tihdr.h \
	src/include/timod.h \
	src/include/tirdwr.h \
	src/include/xti.h \
	src/include/xti_inet.h \
	src/include/sys/ticlts.h \
	src/include/sys/ticots.h \
	src/include/sys/ticotsord.h \
	src/include/sys/tihdr.h \
	src/include/sys/timod.h \
	src/include/sys/tirdwr.h \
	src/include/sys/tli.h \
	src/include/sys/tpi.h \
	src/include/sys/xti.h \
	src/include/sys/xti_inet.h \
	src/include/sys/xti_ip.h \
	src/include/sys/xti_sctp.h \
	src/include/sys/xti_tcp.h \
	src/include/sys/xti_udp.h \
	src/include/xti/xti.h \
	src/include/xti/xti_inet.h \
	src/include/xti/xti_ip.h \
	src/include/xti/xti_sctp.h \
	src/include/xti/xti_tcp.h \
	src/include/xti/xti_udp.h

dist_noinst_SCRIPTS = \
	$(auxdir)/strconf-sh

strconf_FILES = \
	src/modules/lis.conf \
	src/drivers/lis.conf

#
#   Need to override the following two automake rules to strip the first two
#   path components (src/include) from the header file path name for install.
#   This just saves us from doing a recursive make into the src/include directory.
#
install-nobase_includeHEADERS: $(nobase_include_HEADERS)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(includedir)
	@list='$(nobase_include_HEADERS)' ; for p in $$list ; do \
		if test -f "$$p" ; then d= ; else d="$(srcdir)/" ; fi ; \
		f="`echo $$p | sed -e 's|^[^/]*/[^/]*/||'`" ; \
		echo "$(nobase_includeHEADERS_INSTALL) $$d$$p $(DESTDIR)$(includedir)/$$f" ; \
		$(nobase_includeHEADERS_INSTALL) $$d$$p $(DESTDIR)$(includedir)/$$f ; \
	done

uninstall-nobase_includeHEADERS:
	@$(NORMAL_UNINSTALL)
	@list='$(nobase_include_HEADERS)' ; for p in $$list ; do \
		f="`echo $$p | sed -e 's|^[^/]*/[^/]*/||'`" ; \
		echo "rm -f $(DESTDIR)$(includedir)/$$f" ; \
		rm -f $(DESTDIR)$(includedir)/$$f ; \
	done

STRCONF_SCRIPT	    = $(auxdir)/strconf-sh
STRCONF		    = $(SHELL) $(STRCONF_SCRIPT) -b190
STRMAKENODES	    = xnetmakenodes

Config.master: $(strconf_FILES)
	cat /dev/null > Config.master
	@list='$(strconf_FILES)' ; cat /dev/null > Config.master ; for f in $$list ; do \
		if test -f "$$f" ; then d= ; else d="$(srcdir)/" ; fi ; \
		echo "cat $$d$$f >> Config.master" ; \
		cat $$d$$f >> Config.master ; \
	done

conf.modules: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --confmodules=conf.modules Config.master

strconf.h: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --hconfig=strconf.h Config.master

devices.lst: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --strmknods=devices.lst Config.master

xnetmakenodes.c: Config.master $(STRCONF_SCRIPT)
	$(STRCONF) --makenodes=xnetmakenodes.c Config.master

clean-config:
	rm -f Config.master conf.modules strconf.h devices.lst xnetmakenodes.c

.PHONY: clean-config

clean-local: clean-config

$(libXNETmods_a_OBJECTS): strconf.h
$(libXNETdrvs_a_OBJECTS): strconf.h

install-devices: $(STRMAKENODES)
	$(STRMAKENODES)

uninstall-devices: $(STRMAKENODES)
	$(STRMAKENODES) --remove

all-am: conf.modules strconf.h devices.lst

xnet_source_FILES		= $(mods_source_FILES) $(drvs_source_FILES)

EXTRA_DOCS = 

EXTRA_DIST = \
	strxnet.spec \
	openss7-logo-small.gif \
	$(noinst_man_MANS) \
	doc/man/strxnet.macros \
	doc/man/strxnet.refs \
	$(strconf_FILES) \
	$(xnet_source_FILES) \
	$(EXTRA_DOCS)

SUBDIRS = intl m4 po doc

#
#   Rules for making rpms
#
AM_RPMFLAGS = --define "_kversion @kversion@"
AM_SRPMFLAGS =

include am/rpm.am

#
#   General rules
#

include am/openss7.am

# vim: ft=automake
