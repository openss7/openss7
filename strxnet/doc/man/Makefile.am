# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.17 $) $Date: 2006/03/06 11:39:09 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 2 of the License.
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
# Last Modified $Date: 2006/03/06 11:39:09 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.17  2006/03/06 11:39:09  brian
# - updated headers
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

tli_man_MANS	= \
	man4/timod.4 \
	man4/tirdwr.4 \
	man7/T_ADDR_ACK.7 \
	man7/T_addr_ack.7 \
	man7/T_ADDR_REQ.7 \
	man7/T_addr_req.7 \
	man7/T_BIND_ACK.7 \
	man7/T_bind_ack.7 \
	man7/T_BIND_REQ.7 \
	man7/T_bind_req.7 \
	man7/T_CAPABILITY_ACK.7 \
	man7/T_capability_ack.7 \
	man7/T_CAPABILITY_REQ.7 \
	man7/T_capability_req.7 \
	man7/T_CONN_CON.7 \
	man7/T_conn_con.7 \
	man7/T_CONN_IND.7 \
	man7/T_conn_ind.7 \
	man7/T_CONN_REQ.7 \
	man7/T_conn_req.7 \
	man7/T_CONN_RES.7 \
	man7/T_conn_res.7 \
	man7/T_DATA_IND.7 \
	man7/T_data_ind.7 \
	man7/T_DATA_REQ.7 \
	man7/T_data_req.7 \
	man7/T_DISCON_IND.7 \
	man7/T_discon_ind.7 \
	man7/T_DISCON_REQ.7 \
	man7/T_discon_req.7 \
	man7/T_ERROR_ACK.7 \
	man7/T_error_ack.7 \
	man7/T_EXDATA_IND.7 \
	man7/T_exdata_ind.7 \
	man7/T_EXDATA_REQ.7 \
	man7/T_exdata_req.7 \
	man7/T_INFO_ACK.7 \
	man7/T_info_ack.7 \
	man7/T_INFO_REQ.7 \
	man7/T_info_req.7 \
	man7/T_OK_ACK.7 \
	man7/T_ok_ack.7 \
	man7/T_OPTDATA_IND.7 \
	man7/T_optdata_ind.7 \
	man7/T_OPTDATA_REQ.7 \
	man7/T_optdata_req.7 \
	man7/T_OPTMGMT_ACK.7 \
	man7/T_optmgmt_ack.7 \
	man7/T_OPTMGMT_REQ.7 \
	man7/T_optmgmt_req.7 \
	man7/T_ORDREL_IND.7 \
	man7/T_ordrel_ind.7 \
	man7/T_ORDREL_REQ.7 \
	man7/T_ordrel_req.7 \
	man7/tpi.7 \
	man7/T_UDERROR_IND.7 \
	man7/T_uderror_ind.7 \
	man7/T_UNBIND_REQ.7 \
	man7/T_unbind_req.7 \
	man7/T_UNITDATA_IND.7 \
	man7/T_unitdata_ind.7 \
	man7/T_UNITDATA_REQ.7 \
	man7/T_unitdata_req.7

xnet_hidden_symbols = \
	man3/_t_errno.3 \
	man3/_t_errstr.3 \
	man3/__xnet_t_accept.3 \
	man3/__xnet_t_addleaf.3 \
	man3/__xnet_t_alloc.3 \
	man3/__xnet_t_bind.3 \
	man3/__xnet_t_close.3 \
	man3/__xnet_t_connect.3 \
	man3/__xnet_t_errno.3 \
	man3/__xnet_t_error.3 \
	man3/__xnet_t_free.3 \
	man3/__xnet_t_getinfo.3 \
	man3/__xnet_t_getprotaddr.3 \
	man3/__xnet_t_getstate.3 \
	man3/__xnet_t_listen.3 \
	man3/__xnet_t_look.3 \
	man3/__xnet_t_open.3 \
	man3/__xnet_t_optmgmt.3 \
	man3/__xnet_t_rcv.3 \
	man3/__xnet_t_rcvconnect.3 \
	man3/__xnet_t_rcvdis.3 \
	man3/__xnet_t_rcvleafchange.3 \
	man3/__xnet_t_rcvrel.3 \
	man3/__xnet_t_rcvrel_r.3 \
	man3/__xnet_t_rcvreldata.3 \
	man3/__xnet_t_rcvopt.3 \
	man3/__xnet_t_rcvudata.3 \
	man3/__xnet_t_rcvuderr.3 \
	man3/__xnet_t_rcvv.3 \
	man3/__xnet_t_rcvvudata.3 \
	man3/__xnet_t_removeleaf.3 \
	man3/__xnet_t_snd.3 \
	man3/__xnet_t_snddis.3 \
	man3/__xnet_t_sndopt.3 \
	man3/__xnet_t_sndrel.3 \
	man3/__xnet_t_sndreldata.3 \
	man3/__xnet_t_sndudata.3 \
	man3/__xnet_t_sndv.3 \
	man3/__xnet_t_sndvopt.3 \
	man3/__xnet_t_sndvudata.3 \
	man3/__xnet_t_strerror.3 \
	man3/__xnet_t_sync.3 \
	man3/__xnet_t_sysconf.3 \
	man3/__xnet_t_unbind.3 \
	man3/__xnet_t_accept_r.3 \
	man3/__xnet_t_addleaf_r.3 \
	man3/__xnet_t_alloc_r.3 \
	man3/__xnet_t_bind_r.3 \
	man3/__xnet_t_close_r.3 \
	man3/__xnet_t_connect_r.3 \
	man3/__xnet_t_error_r.3 \
	man3/__xnet_t_getinfo_r.3 \
	man3/__xnet_t_getprotaddr_r.3 \
	man3/__xnet_t_getstate_r.3 \
	man3/__xnet_t_listen_r.3 \
	man3/__xnet_t_look_r.3 \
	man3/__xnet_t_open_r.3 \
	man3/__xnet_t_optmgmt_r.3 \
	man3/__xnet_t_rcv_r.3 \
	man3/__xnet_t_rcvconnect_r.3 \
	man3/__xnet_t_rcvdis_r.3 \
	man3/__xnet_t_rcvleafchange_r.3 \
	man3/__xnet_t_rcvopt_r.3 \
	man3/__xnet_t_rcvreldata_r.3 \
	man3/__xnet_t_rcvudata_r.3 \
	man3/__xnet_t_rcvuderr_r.3 \
	man3/__xnet_t_rcvv_r.3 \
	man3/__xnet_t_rcvvudata_r.3 \
	man3/__xnet_t_removeleaf_r.3 \
	man3/__xnet_t_snd_r.3 \
	man3/__xnet_t_snddis_r.3 \
	man3/__xnet_t_sndopt_r.3 \
	man3/__xnet_t_sndrel_r.3 \
	man3/__xnet_t_sndreldata_r.3 \
	man3/__xnet_t_sndudata_r.3 \
	man3/__xnet_t_sndv_r.3 \
	man3/__xnet_t_sndvopt_r.3 \
	man3/__xnet_t_sndvudata_r.3 \
	man3/__xnet_t_sync_r.3 \
	man3/__xnet_t_unbind_r.3 \
	man3/__xnet_t_peek.3

xnet_man_MANS	= \
	man3/t_accept.3 \
	man3/t_addleaf.3 \
	man3/t_alloc.3 \
	man3/t_bind.3 \
	man3/t_close.3 \
	man3/t_connect.3 \
	man3/t_errlist.3 \
	man3/t_errno.3 \
	man3/t_error.3 \
	man3/t_errstr.3 \
	man3/t_free.3 \
	man3/t_getinfo.3 \
	man3/t_getprotaddr.3 \
	man3/t_getstate.3 \
	man3/t_listen.3 \
	man3/t_look.3 \
	man3/t_nerr.3 \
	man3/t_open.3 \
	man3/t_optmgmt.3 \
	man3/t_rcv.3 \
	man3/t_rcvconnect.3 \
	man3/t_rcvdis.3 \
	man3/t_rcvleafchange.3 \
	man3/t_rcvopt.3 \
	man3/t_rcvrel.3 \
	man3/t_rcvreldata.3 \
	man3/t_rcvudata.3 \
	man3/t_rcvuderr.3 \
	man3/t_rcvv.3 \
	man3/t_rcvvudata.3 \
	man3/t_removeleaf.3 \
	man3/t_snd.3 \
	man3/t_snddis.3 \
	man3/t_sndopt.3 \
	man3/t_sndrel.3 \
	man3/t_sndreldata.3 \
	man3/t_sndudata.3 \
	man3/t_sndv.3 \
	man3/t_sndvopt.3 \
	man3/t_sndvudata.3 \
	man3/t_strerror.3 \
	man3/t_sync.3 \
	man3/t_sysconf.3 \
	man3/t_unbind.3 \
	man3/xnet.3 \
	man3/xti.3 \
	man8/strxnet_mknod.8\
	man8/test-timod.8 \
	man8/test-tirdwr.8 \
	man8/test-xnet.8 \
	man8/test-xnet_thr.8 \
	man8/test-xnsl.8 \
	man8/test-xnsl_thr.8

xnsl_man_MANS 	= \
	man3/endnetconfig.3 \
	man3/endnetpath.3 \
	man3/freenetconfigent.3 \
	man3/getnetconfig.3 \
	man3/getnetconfigent.3 \
	man3/getnetpath.3 \
	man3/getpeername.3 \
	man3/getsockname.3 \
	man3/getsockopt.3 \
	man3/libxnsl.3 \
	man3/nc_perror.3 \
	man3/nc_sperror.3 \
	man3/_nderror.3 \
	man3/netbuf.3 \
	man3/netconfig.3 \
	man3/netdir.3 \
	man3/netdir_free.3 \
	man3/netdir_getbyaddr.3 \
	man3/netdir_getbyname.3 \
	man3/netdir_options.3 \
	man3/netdir_perror.3 \
	man3/netdir_sperror.3 \
	man3/netpath.3 \
	man3/setnetconfig.3 \
	man3/setnetpath.3 \
	man3/taddr2uaddr.3 \
	man3/uaddr2taddr.3 \
	man3/xnsl.3 \
	man3/inetaddr.3 \
	man5/netconfig.5 \
	man5/netdir.5

sock_man_MANS	= \
	man3/accept.3 \
	man3/bind.3 \
	man3/connect.3 \
	man3/getpeername.3 \
	man3/getsockname.3 \
	man3/getsockopt.3 \
	man3/libsocket.3 \
	man3/listen.3 \
	man3/recv.3 \
	man3/recvfrom.3 \
	man3/recvmsg.3 \
	man3/send.3 \
	man3/sendmsg.3 \
	man3/sendto.3 \
	man3/setsockopt.3 \
	man3/shutdown.3 \
	man3/socket.3 \
	man3/socketpair.3 \
	man3/sockets.3 \
	man4/sockmod.4 \
	man4/socksys.4

man_MANS	=


if WITH_TLI
## WITH_TLI
man_MANS	+= $(tli_man_MANS)
## WITH_TLI
endif
if WITH_XNET
## WITH_XNET
man_MANS	+= $(xnet_man_MANS)
## WITH_XNET
endif
if WITH_XNSL
## WITH_XNSL
man_MANS	+= $(xnsl_man_MANS)
## WITH_XNSL
endif
if WITH_SOCK
## WITH_SOCK
man_MANS	+= $(sock_man_MANS)
## WITH_SOCK
endif

tli_dist_noinst_MANS	= \
	man4/timod.4.man \
	man4/tirdwr.4.man \
	man7/T_ADDR_ACK.7.man \
	man7/T_addr_ack.7.man \
	man7/T_ADDR_REQ.7.man \
	man7/T_addr_req.7.man \
	man7/T_BIND_ACK.7.man \
	man7/T_bind_ack.7.man \
	man7/T_BIND_REQ.7.man \
	man7/T_bind_req.7.man \
	man7/T_CAPABILITY_ACK.7.man \
	man7/T_capability_ack.7.man \
	man7/T_CAPABILITY_REQ.7.man \
	man7/T_capability_req.7.man \
	man7/T_CONN_CON.7.man \
	man7/T_conn_con.7.man \
	man7/T_CONN_IND.7.man \
	man7/T_conn_ind.7.man \
	man7/T_CONN_REQ.7.man \
	man7/T_conn_req.7.man \
	man7/T_CONN_RES.7.man \
	man7/T_conn_res.7.man \
	man7/T_DATA_IND.7.man \
	man7/T_data_ind.7.man \
	man7/T_DATA_REQ.7.man \
	man7/T_data_req.7.man \
	man7/T_DISCON_IND.7.man \
	man7/T_discon_ind.7.man \
	man7/T_DISCON_REQ.7.man \
	man7/T_discon_req.7.man \
	man7/T_ERROR_ACK.7.man \
	man7/T_error_ack.7.man \
	man7/T_EXDATA_IND.7.man \
	man7/T_exdata_ind.7.man \
	man7/T_EXDATA_REQ.7.man \
	man7/T_exdata_req.7.man \
	man7/T_INFO_ACK.7.man \
	man7/T_info_ack.7.man \
	man7/T_INFO_REQ.7.man \
	man7/T_info_req.7.man \
	man7/T_OK_ACK.7.man \
	man7/T_ok_ack.7.man \
	man7/T_OPTDATA_IND.7.man \
	man7/T_optdata_ind.7.man \
	man7/T_OPTDATA_REQ.7.man \
	man7/T_optdata_req.7.man \
	man7/T_OPTMGMT_ACK.7.man \
	man7/T_optmgmt_ack.7.man \
	man7/T_OPTMGMT_REQ.7.man \
	man7/T_optmgmt_req.7.man \
	man7/T_ORDREL_IND.7.man \
	man7/T_ordrel_ind.7.man \
	man7/T_ORDREL_REQ.7.man \
	man7/T_ordrel_req.7.man \
	man7/tpi.7.man \
	man7/T_UDERROR_IND.7.man \
	man7/T_uderror_ind.7.man \
	man7/T_UNBIND_REQ.7.man \
	man7/T_unbind_req.7.man \
	man7/T_UNITDATA_IND.7.man \
	man7/T_unitdata_ind.7.man \
	man7/T_UNITDATA_REQ.7.man \
	man7/T_unitdata_req.7.man

nodist_xnet_hidden_symbols = \
	man3/_t_errno.3.man \
	man3/_t_errstr.3.man \
	man3/__xnet_t_accept.3.man \
	man3/__xnet_t_addleaf.3.man \
	man3/__xnet_t_alloc.3.man \
	man3/__xnet_t_bind.3.man \
	man3/__xnet_t_close.3.man \
	man3/__xnet_t_connect.3.man \
	man3/__xnet_t_errno.3.man \
	man3/__xnet_t_error.3.man \
	man3/__xnet_t_free.3.man \
	man3/__xnet_t_getinfo.3.man \
	man3/__xnet_t_getprotaddr.3.man \
	man3/__xnet_t_getstate.3.man \
	man3/__xnet_t_listen.3.man \
	man3/__xnet_t_look.3.man \
	man3/__xnet_t_open.3.man \
	man3/__xnet_t_optmgmt.3.man \
	man3/__xnet_t_rcv.3.man \
	man3/__xnet_t_rcvconnect.3.man \
	man3/__xnet_t_rcvdis.3.man \
	man3/__xnet_t_rcvleafchange.3.man \
	man3/__xnet_t_rcvrel.3.man \
	man3/__xnet_t_rcvrel_r.3.man \
	man3/__xnet_t_rcvreldata.3.man \
	man3/__xnet_t_rcvopt.3.man \
	man3/__xnet_t_rcvudata.3.man \
	man3/__xnet_t_rcvuderr.3.man \
	man3/__xnet_t_rcvv.3.man \
	man3/__xnet_t_rcvvudata.3.man \
	man3/__xnet_t_removeleaf.3.man \
	man3/__xnet_t_snd.3.man \
	man3/__xnet_t_snddis.3.man \
	man3/__xnet_t_sndopt.3.man \
	man3/__xnet_t_sndrel.3.man \
	man3/__xnet_t_sndreldata.3.man \
	man3/__xnet_t_sndudata.3.man \
	man3/__xnet_t_sndv.3.man \
	man3/__xnet_t_sndvopt.3.man \
	man3/__xnet_t_sndvudata.3.man \
	man3/__xnet_t_strerror.3.man \
	man3/__xnet_t_sync.3.man \
	man3/__xnet_t_sysconf.3.man \
	man3/__xnet_t_unbind.3.man \
	man3/__xnet_t_accept_r.3.man \
	man3/__xnet_t_addleaf_r.3.man \
	man3/__xnet_t_alloc_r.3.man \
	man3/__xnet_t_bind_r.3.man \
	man3/__xnet_t_close_r.3.man \
	man3/__xnet_t_connect_r.3.man \
	man3/__xnet_t_error_r.3.man \
	man3/__xnet_t_getinfo_r.3.man \
	man3/__xnet_t_getprotaddr_r.3.man \
	man3/__xnet_t_getstate_r.3.man \
	man3/__xnet_t_listen_r.3.man \
	man3/__xnet_t_look_r.3.man \
	man3/__xnet_t_open_r.3.man \
	man3/__xnet_t_optmgmt_r.3.man \
	man3/__xnet_t_rcv_r.3.man \
	man3/__xnet_t_rcvconnect_r.3.man \
	man3/__xnet_t_rcvdis_r.3.man \
	man3/__xnet_t_rcvleafchange_r.3.man \
	man3/__xnet_t_rcvopt_r.3.man \
	man3/__xnet_t_rcvreldata_r.3.man \
	man3/__xnet_t_rcvudata_r.3.man \
	man3/__xnet_t_rcvuderr_r.3.man \
	man3/__xnet_t_rcvv_r.3.man \
	man3/__xnet_t_rcvvudata_r.3.man \
	man3/__xnet_t_removeleaf_r.3.man \
	man3/__xnet_t_snd_r.3.man \
	man3/__xnet_t_snddis_r.3.man \
	man3/__xnet_t_sndopt_r.3.man \
	man3/__xnet_t_sndrel_r.3.man \
	man3/__xnet_t_sndreldata_r.3.man \
	man3/__xnet_t_sndudata_r.3.man \
	man3/__xnet_t_sndv_r.3.man \
	man3/__xnet_t_sndvopt_r.3.man \
	man3/__xnet_t_sndvudata_r.3.man \
	man3/__xnet_t_sync_r.3.man \
	man3/__xnet_t_unbind_r.3.man \
	man3/__xnet_t_peek.3.man

xnet_dist_noinst_MANS	= \
	man3/t_accept.3.man \
	man3/t_addleaf.3.man \
	man3/t_alloc.3.man \
	man3/t_bind.3.man \
	man3/t_close.3.man \
	man3/t_connect.3.man \
	man3/t_errlist.3.man \
	man3/t_errno.3.man \
	man3/t_error.3.man \
	man3/t_errstr.3.man \
	man3/t_free.3.man \
	man3/t_getinfo.3.man \
	man3/t_getprotaddr.3.man \
	man3/t_getstate.3.man \
	man3/t_listen.3.man \
	man3/t_look.3.man \
	man3/t_nerr.3.man \
	man3/t_open.3.man \
	man3/t_optmgmt.3.man \
	man3/t_rcv.3.man \
	man3/t_rcvconnect.3.man \
	man3/t_rcvdis.3.man \
	man3/t_rcvleafchange.3.man \
	man3/t_rcvopt.3.man \
	man3/t_rcvrel.3.man \
	man3/t_rcvreldata.3.man \
	man3/t_rcvudata.3.man \
	man3/t_rcvuderr.3.man \
	man3/t_rcvv.3.man \
	man3/t_rcvvudata.3.man \
	man3/t_removeleaf.3.man \
	man3/t_snd.3.man \
	man3/t_snddis.3.man \
	man3/t_sndopt.3.man \
	man3/t_sndrel.3.man \
	man3/t_sndreldata.3.man \
	man3/t_sndudata.3.man \
	man3/t_sndv.3.man \
	man3/t_sndvopt.3.man \
	man3/t_sndvudata.3.man \
	man3/t_strerror.3.man \
	man3/t_sync.3.man \
	man3/t_sysconf.3.man \
	man3/t_unbind.3.man \
	man3/xnet.3.man \
	man3/xti.3.man \
	man8/strxnet_mknod.8.man \
	man8/test-timod.8.man \
	man8/test-tirdwr.8.man \
	man8/test-xnet.8.man \
	man8/test-xnet_thr.8.man \
	man8/test-xnsl.8.man \
	man8/test-xnsl_thr.8.man

xnsl_dist_noinst_MANS 	= \
	man3/endnetconfig.3.man \
	man3/endnetpath.3.man \
	man3/freenetconfigent.3.man \
	man3/getnetconfig.3.man \
	man3/getnetconfigent.3.man \
	man3/getnetpath.3.man \
	man3/getpeername.3.man \
	man3/getsockname.3.man \
	man3/getsockopt.3.man \
	man3/libxnsl.3.man \
	man3/nc_perror.3.man \
	man3/nc_sperror.3.man \
	man3/_nderror.3.man \
	man3/netbuf.3.man \
	man3/netconfig.3.man \
	man3/netdir.3.man \
	man3/netdir_free.3.man \
	man3/netdir_getbyaddr.3.man \
	man3/netdir_getbyname.3.man \
	man3/netdir_options.3.man \
	man3/netdir_perror.3.man \
	man3/netdir_sperror.3.man \
	man3/netpath.3.man \
	man3/setnetconfig.3.man \
	man3/setnetpath.3.man \
	man3/taddr2uaddr.3.man \
	man3/uaddr2taddr.3.man \
	man3/xnsl.3.man \
	man3/inetaddr.3.man \
	man5/netconfig.5.man \
	man5/netdir.5.man

sock_dist_noinst_MANS	= \
	man3/accept.3.man \
	man3/bind.3.man \
	man3/connect.3.man \
	man3/getpeername.3.man \
	man3/getsockname.3.man \
	man3/getsockopt.3.man \
	man3/libsocket.3.man \
	man3/listen.3.man \
	man3/recv.3.man \
	man3/recvfrom.3.man \
	man3/recvmsg.3.man \
	man3/send.3.man \
	man3/sendmsg.3.man \
	man3/sendto.3.man \
	man3/setsockopt.3.man \
	man3/shutdown.3.man \
	man3/socket.3.man \
	man3/socketpair.3.man \
	man3/sockets.3.man \
	man4/sockmod.4.man \
	man4/socksys.4.man

dist_noinst_MANS	= \
	$(tli_dist_noinst_MANS) \
	$(xnet_dist_noinst_MANS) \
	$(xnsl_dist_noinst_MANS) \
	$(sock_dist_noinst_MANS)

## PKG_BUILD_INDEP
endif

include ../../am/man.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
