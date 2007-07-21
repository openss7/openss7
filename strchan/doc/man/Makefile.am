# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2007/07/21 20:43:46 $
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
# Last Modified $Date: 2007/07/21 20:43:46 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.7  2007/07/21 20:43:46  brian
# - added manual pages, corrections
#
# Revision 0.9.2.6  2007/01/08 16:23:14  brian
# - final updates for release
#
# Revision 0.9.2.5  2007/01/05 02:53:56  brian
# - updated documentation for release
#
# Revision 0.9.2.4  2006/12/22 07:17:33  brian
# - updated documentation for release (make check target)
#
# Revision 0.9.2.3  2006/12/20 23:07:33  brian
# - updates for release and current development
#
# Revision 0.9.2.2  2006/10/14 06:48:32  brian
# - updates to include files from strss7 package
#
# Revision 0.9.2.1  2006/10/13 07:57:31  brian
# - added initial STREAMS Channel package
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

man_MANS = \
	man4/cd.4 \
	man4/cd_daed.4 \
	man4/cd_hdlc.4 \
	man4/ch.4 \
	man4/ch_ioctl.4 \
	man4/ch-pmod.4 \
	man4/chi_ioctl.4 \
	man4/MG.4 \
	man4/mg.4 \
	man4/mg_ioctl.4 \
	man4/mgi_ioctl.4 \
	man4/mg-lm.4 \
	man4/mx.4 \
	man4/mx_ioctl.4 \
	man4/mx-pmod.4 \
	man4/mxi_ioctl.4 \
	man4/mx_sdl.4 \
	man4/v401p.4 \
	man5/strchan.5 \
	man7/CH_ATTACH_REQ.7 \
	man7/CH_attach_req.7 \
	man7/CH_attach_req_t.7 \
	man7/CH_CONNECT_CON.7 \
	man7/CH_connect_con.7 \
	man7/CH_connect_con_t.7 \
	man7/CH_CONNECT_REQ.7 \
	man7/CH_connect_req.7 \
	man7/CH_connect_req_t.7 \
	man7/CH_DATA_IND.7 \
	man7/CH_data_ind.7 \
	man7/CH_data_ind_t.7 \
	man7/CH_DATA_REQ.7 \
	man7/CH_data_req.7 \
	man7/CH_data_req_t.7 \
	man7/CH_DETACH_REQ.7 \
	man7/CH_detach_req.7 \
	man7/CH_detach_req_t.7 \
	man7/CH_DISABLE_CON.7 \
	man7/CH_disable_con.7 \
	man7/CH_disable_con_t.7 \
	man7/CH_DISABLE_IND.7 \
	man7/CH_disable_ind.7 \
	man7/CH_disable_ind_t.7 \
	man7/CH_DISABLE_REQ.7 \
	man7/CH_disable_req.7 \
	man7/CH_disable_req_t.7 \
	man7/CH_DISCONNECT_CON.7 \
	man7/CH_disconnect_con.7 \
	man7/CH_disconnect_con_t.7 \
	man7/CH_DISCONNECT_IND.7 \
	man7/CH_disconnect_ind.7 \
	man7/CH_disconnect_ind_t.7 \
	man7/CH_DISCONNECT_REQ.7 \
	man7/CH_disconnect_req.7 \
	man7/CH_disconnect_req_t.7 \
	man7/CH_ENABLE_CON.7 \
	man7/CH_enable_con.7 \
	man7/CH_enable_con_t.7 \
	man7/CH_ENABLE_REQ.7 \
	man7/CH_enable_req.7 \
	man7/CH_enable_req_t.7 \
	man7/CH_ERROR_ACK.7 \
	man7/CH_error_ack.7 \
	man7/CH_error_ack_t.7 \
	man7/CH_EVENT_IND.7 \
	man7/CH_event_ind.7 \
	man7/CH_event_ind_t.7 \
	man7/chi.7 \
	man7/CH_INFO_ACK.7 \
	man7/CH_info_ack.7 \
	man7/CH_info_ack_t.7 \
	man7/CH_INFO_REQ.7 \
	man7/CH_info_req.7 \
	man7/CH_info_req_t.7 \
	man7/CH_OK_ACK.7 \
	man7/CH_ok_ack.7 \
	man7/CH_ok_ack_t.7 \
	man7/CH_OPTMGMT_ACK.7 \
	man7/CH_optmgmt_ack.7 \
	man7/CH_optmgmt_ack_t.7 \
	man7/CH_OPTMGMT_REQ.7 \
	man7/CH_optmgmt_req.7 \
	man7/CH_optmgmt_req_t.7 \
	man7/MG_ABORT_REQ.7 \
	man7/MG_abort_req.7 \
	man7/MG_abort_req_t.7 \
	man7/MG_ACTION_CON.7 \
	man7/MG_action_con.7 \
	man7/MG_action_con_t.7 \
	man7/MG_ACTION_IND.7 \
	man7/MG_action_ind.7 \
	man7/MG_action_ind_t.7 \
	man7/MG_ACTION_REQ.7 \
	man7/MG_action_req.7 \
	man7/MG_action_req_t.7 \
	man7/MG_ATTACH_ACK.7 \
	man7/MG_attach_ack.7 \
	man7/MG_attach_ack_t.7 \
	man7/MG_ATTACH_REQ.7 \
	man7/MG_attach_req.7 \
	man7/MG_attach_req_t.7 \
	man7/MG_CONN_CON.7 \
	man7/MG_conn_con.7 \
	man7/MG_conn_con_t.7 \
	man7/MG_CONN_REQ.7 \
	man7/MG_conn_req.7 \
	man7/MG_conn_req_t.7 \
	man7/MG_DATA_IND.7 \
	man7/MG_data_ind.7 \
	man7/MG_data_ind_t.7 \
	man7/MG_DATA_REQ.7 \
	man7/MG_data_req.7 \
	man7/MG_data_req_t.7 \
	man7/MG_DETACH_REQ.7 \
	man7/MG_detach_req.7 \
	man7/MG_detach_req_t.7 \
	man7/MG_DISCON_CON.7 \
	man7/MG_discon_con.7 \
	man7/MG_discon_con_t.7 \
	man7/MG_DISCON_IND.7 \
	man7/MG_discon_ind.7 \
	man7/MG_discon_ind_t.7 \
	man7/MG_DISCON_REQ.7 \
	man7/MG_discon_req.7 \
	man7/MG_discon_req_t.7 \
	man7/MG_ERROR_ACK.7 \
	man7/MG_error_ack.7 \
	man7/MG_error_ack_t.7 \
	man7/mgi.7 \
	man7/mgi_mg.7 \
	man7/MG_INFO_ACK.7 \
	man7/MG_info_ack.7 \
	man7/MG_info_ack_t.7 \
	man7/MG_INFO_REQ.7 \
	man7/MG_info_req.7 \
	man7/MG_info_req_t.7 \
	man7/MG_JOIN_CON.7 \
	man7/MG_join_con.7 \
	man7/MG_join_con_t.7 \
	man7/MG_JOIN_REQ.7 \
	man7/MG_join_req.7 \
	man7/MG_join_req_t.7 \
	man7/MG_LEAVE_CON.7 \
	man7/MG_leave_con.7 \
	man7/MG_leave_con_t.7 \
	man7/MG_LEAVE_IND.7 \
	man7/MG_leave_ind.7 \
	man7/MG_leave_ind_t.7 \
	man7/MG_LEAVE_REQ.7 \
	man7/MG_leave_req.7 \
	man7/MG_leave_req_t.7 \
	man7/MG_NOTIFY_IND.7 \
	man7/MG_notify_ind.7 \
	man7/MG_notify_ind_t.7 \
	man7/MG_NOTIFY_REQ.7 \
	man7/MG_notify_req.7 \
	man7/MG_notify_req_t.7 \
	man7/MG_OK_ACK.7 \
	man7/MG_ok_ack.7 \
	man7/MG_ok_ack_t.7 \
	man7/MG_OPTMGMT_ACK.7 \
	man7/MG_optmgmt_ack.7 \
	man7/MG_optmgmt_ack_t.7 \
	man7/MG_OPTMGMT_REQ.7 \
	man7/MG_optmgmt_req.7 \
	man7/MG_optmgmt_req_t.7 \
	man7/MX_ATTACH_REQ.7 \
	man7/MX_attach_req.7 \
	man7/MX_attach_req_t.7 \
	man7/MX_CONNECT_CON.7 \
	man7/MX_connect_con.7 \
	man7/MX_connect_con_t.7 \
	man7/MX_CONNECT_REQ.7 \
	man7/MX_connect_req.7 \
	man7/MX_connect_req_t.7 \
	man7/MX_DATA_IND.7 \
	man7/MX_data_ind.7 \
	man7/MX_data_ind_t.7 \
	man7/MX_DATA_REQ.7 \
	man7/MX_data_req.7 \
	man7/MX_data_req_t.7 \
	man7/MX_DETACH_REQ.7 \
	man7/MX_detach_req.7 \
	man7/MX_detach_req_t.7 \
	man7/MX_DISABLE_CON.7 \
	man7/MX_disable_con.7 \
	man7/MX_disable_con_t.7 \
	man7/MX_DISABLE_IND.7 \
	man7/MX_disable_ind.7 \
	man7/MX_disable_ind_t.7 \
	man7/MX_DISABLE_REQ.7 \
	man7/MX_disable_req.7 \
	man7/MX_disable_req_t.7 \
	man7/MX_DISCONNECT_CON.7 \
	man7/MX_disconnect_con.7 \
	man7/MX_disconnect_con_t.7 \
	man7/MX_DISCONNECT_IND.7 \
	man7/MX_disconnect_ind.7 \
	man7/MX_disconnect_ind_t.7 \
	man7/MX_DISCONNECT_REQ.7 \
	man7/MX_disconnect_req.7 \
	man7/MX_disconnect_req_t.7 \
	man7/MX_ENABLE_CON.7 \
	man7/MX_enable_con.7 \
	man7/MX_enable_con_t.7 \
	man7/MX_ENABLE_REQ.7 \
	man7/MX_enable_req.7 \
	man7/MX_enable_req_t.7 \
	man7/MX_ERROR_ACK.7 \
	man7/MX_error_ack.7 \
	man7/MX_error_ack_t.7 \
	man7/mxi.7 \
	man7/MX_INFO_ACK.7 \
	man7/MX_info_ack.7 \
	man7/MX_info_ack_t.7 \
	man7/MX_INFO_REQ.7 \
	man7/MX_info_req.7 \
	man7/MX_info_req_t.7 \
	man7/MX_OK_ACK.7 \
	man7/MX_ok_ack.7 \
	man7/MX_ok_ack_t.7 \
	man7/MX_OPTMGMT_ACK.7 \
	man7/MX_optmgmt_ack.7 \
	man7/MX_optmgmt_ack_t.7 \
	man7/MX_OPTMGMT_REQ.7 \
	man7/MX_optmgmt_req.7 \
	man7/MX_optmgmt_req_t.7 \
	man8/test-chan.8 \
	man8/strchan_mknod.8 \
	man9/zaptel.9 \
	man9/zt_alarm_notify.9 \
	man9/zt_dtmf_tone.9 \
	man9/zt_dynamic_receive.9 \
	man9/zt_dynamic_register.9 \
	man9/zt_dynamic_unregister.9 \
	man9/zt_ec_chunk.9 \
	man9/zt_ec_span.9 \
	man9/zt_hooksig.9 \
	man9/zt_init_tone_state.9 \
	man9/zt_lboname.9 \
	man9/zt_qevent_lock.9 \
	man9/zt_qevent_nolock.9 \
	man9/zt_rbsbits.9 \
	man9/zt_receive.9 \
	man9/zt_register.9 \
	man9/zt_set_dynamic_ioctl.9 \
	man9/zt_tone_nextsample.9 \
	man9/zt_transmit.9 \
	man9/zt_txtone_nextsample.9 \
	man9/zt_unregister.9

dist_noinst_MANS = \
	man4/cd.4.man \
	man4/cd_daed.4.man \
	man4/cd_hdlc.4.man \
	man4/ch.4.man \
	man4/ch_ioctl.4.man \
	man4/ch-pmod.4.man \
	man4/chi_ioctl.4.man \
	man4/MG.4.man \
	man4/mg.4.man \
	man4/mg_ioctl.4.man \
	man4/mgi_ioctl.4.man \
	man4/mg-lm.4.man \
	man4/mx.4.man \
	man4/mx_ioctl.4.man \
	man4/mx-pmod.4.man \
	man4/mxi_ioctl.4.man \
	man4/mx_sdl.4.man \
	man4/v401p.4.man \
	man5/strchan.5.man \
	man7/CH_ATTACH_REQ.7.man \
	man7/CH_attach_req.7.man \
	man7/CH_attach_req_t.7.man \
	man7/CH_CONNECT_CON.7.man \
	man7/CH_connect_con.7.man \
	man7/CH_connect_con_t.7.man \
	man7/CH_CONNECT_REQ.7.man \
	man7/CH_connect_req.7.man \
	man7/CH_connect_req_t.7.man \
	man7/CH_DATA_IND.7.man \
	man7/CH_data_ind.7.man \
	man7/CH_data_ind_t.7.man \
	man7/CH_DATA_REQ.7.man \
	man7/CH_data_req.7.man \
	man7/CH_data_req_t.7.man \
	man7/CH_DETACH_REQ.7.man \
	man7/CH_detach_req.7.man \
	man7/CH_detach_req_t.7.man \
	man7/CH_DISABLE_CON.7.man \
	man7/CH_disable_con.7.man \
	man7/CH_disable_con_t.7.man \
	man7/CH_DISABLE_IND.7.man \
	man7/CH_disable_ind.7.man \
	man7/CH_disable_ind_t.7.man \
	man7/CH_DISABLE_REQ.7.man \
	man7/CH_disable_req.7.man \
	man7/CH_disable_req_t.7.man \
	man7/CH_DISCONNECT_CON.7.man \
	man7/CH_disconnect_con.7.man \
	man7/CH_disconnect_con_t.7.man \
	man7/CH_DISCONNECT_IND.7.man \
	man7/CH_disconnect_ind.7.man \
	man7/CH_disconnect_ind_t.7.man \
	man7/CH_DISCONNECT_REQ.7.man \
	man7/CH_disconnect_req.7.man \
	man7/CH_disconnect_req_t.7.man \
	man7/CH_ENABLE_CON.7.man \
	man7/CH_enable_con.7.man \
	man7/CH_enable_con_t.7.man \
	man7/CH_ENABLE_REQ.7.man \
	man7/CH_enable_req.7.man \
	man7/CH_enable_req_t.7.man \
	man7/CH_ERROR_ACK.7.man \
	man7/CH_error_ack.7.man \
	man7/CH_error_ack_t.7.man \
	man7/CH_EVENT_IND.7.man \
	man7/CH_event_ind.7.man \
	man7/CH_event_ind_t.7.man \
	man7/chi.7.man \
	man7/CH_INFO_ACK.7.man \
	man7/CH_info_ack.7.man \
	man7/CH_info_ack_t.7.man \
	man7/CH_INFO_REQ.7.man \
	man7/CH_info_req.7.man \
	man7/CH_info_req_t.7.man \
	man7/CH_OK_ACK.7.man \
	man7/CH_ok_ack.7.man \
	man7/CH_ok_ack_t.7.man \
	man7/CH_OPTMGMT_ACK.7.man \
	man7/CH_optmgmt_ack.7.man \
	man7/CH_optmgmt_ack_t.7.man \
	man7/CH_OPTMGMT_REQ.7.man \
	man7/CH_optmgmt_req.7.man \
	man7/CH_optmgmt_req_t.7.man \
	man7/MG_ABORT_REQ.7.man \
	man7/MG_abort_req.7.man \
	man7/MG_abort_req_t.7.man \
	man7/MG_ACTION_CON.7.man \
	man7/MG_action_con.7.man \
	man7/MG_action_con_t.7.man \
	man7/MG_ACTION_IND.7.man \
	man7/MG_action_ind.7.man \
	man7/MG_action_ind_t.7.man \
	man7/MG_ACTION_REQ.7.man \
	man7/MG_action_req.7.man \
	man7/MG_action_req_t.7.man \
	man7/MG_ATTACH_ACK.7.man \
	man7/MG_attach_ack.7.man \
	man7/MG_attach_ack_t.7.man \
	man7/MG_ATTACH_REQ.7.man \
	man7/MG_attach_req.7.man \
	man7/MG_attach_req_t.7.man \
	man7/MG_CONN_CON.7.man \
	man7/MG_conn_con.7.man \
	man7/MG_conn_con_t.7.man \
	man7/MG_CONN_REQ.7.man \
	man7/MG_conn_req.7.man \
	man7/MG_conn_req_t.7.man \
	man7/MG_DATA_IND.7.man \
	man7/MG_data_ind.7.man \
	man7/MG_data_ind_t.7.man \
	man7/MG_DATA_REQ.7.man \
	man7/MG_data_req.7.man \
	man7/MG_data_req_t.7.man \
	man7/MG_DETACH_REQ.7.man \
	man7/MG_detach_req.7.man \
	man7/MG_detach_req_t.7.man \
	man7/MG_DISCON_CON.7.man \
	man7/MG_discon_con.7.man \
	man7/MG_discon_con_t.7.man \
	man7/MG_DISCON_IND.7.man \
	man7/MG_discon_ind.7.man \
	man7/MG_discon_ind_t.7.man \
	man7/MG_DISCON_REQ.7.man \
	man7/MG_discon_req.7.man \
	man7/MG_discon_req_t.7.man \
	man7/MG_ERROR_ACK.7.man \
	man7/MG_error_ack.7.man \
	man7/MG_error_ack_t.7.man \
	man7/mgi.7.man \
	man7/mgi_mg.7.man \
	man7/MG_INFO_ACK.7.man \
	man7/MG_info_ack.7.man \
	man7/MG_info_ack_t.7.man \
	man7/MG_INFO_REQ.7.man \
	man7/MG_info_req.7.man \
	man7/MG_info_req_t.7.man \
	man7/MG_JOIN_CON.7.man \
	man7/MG_join_con.7.man \
	man7/MG_join_con_t.7.man \
	man7/MG_JOIN_REQ.7.man \
	man7/MG_join_req.7.man \
	man7/MG_join_req_t.7.man \
	man7/MG_LEAVE_CON.7.man \
	man7/MG_leave_con.7.man \
	man7/MG_leave_con_t.7.man \
	man7/MG_LEAVE_IND.7.man \
	man7/MG_leave_ind.7.man \
	man7/MG_leave_ind_t.7.man \
	man7/MG_LEAVE_REQ.7.man \
	man7/MG_leave_req.7.man \
	man7/MG_leave_req_t.7.man \
	man7/MG_NOTIFY_IND.7.man \
	man7/MG_notify_ind.7.man \
	man7/MG_notify_ind_t.7.man \
	man7/MG_NOTIFY_REQ.7.man \
	man7/MG_notify_req.7.man \
	man7/MG_notify_req_t.7.man \
	man7/MG_OK_ACK.7.man \
	man7/MG_ok_ack.7.man \
	man7/MG_ok_ack_t.7.man \
	man7/MG_OPTMGMT_ACK.7.man \
	man7/MG_optmgmt_ack.7.man \
	man7/MG_optmgmt_ack_t.7.man \
	man7/MG_OPTMGMT_REQ.7.man \
	man7/MG_optmgmt_req.7.man \
	man7/MG_optmgmt_req_t.7.man \
	man7/MX_ATTACH_REQ.7.man \
	man7/MX_attach_req.7.man \
	man7/MX_attach_req_t.7.man \
	man7/MX_CONNECT_CON.7.man \
	man7/MX_connect_con.7.man \
	man7/MX_connect_con_t.7.man \
	man7/MX_CONNECT_REQ.7.man \
	man7/MX_connect_req.7.man \
	man7/MX_connect_req_t.7.man \
	man7/MX_DATA_IND.7.man \
	man7/MX_data_ind.7.man \
	man7/MX_data_ind_t.7.man \
	man7/MX_DATA_REQ.7.man \
	man7/MX_data_req.7.man \
	man7/MX_data_req_t.7.man \
	man7/MX_DETACH_REQ.7.man \
	man7/MX_detach_req.7.man \
	man7/MX_detach_req_t.7.man \
	man7/MX_DISABLE_CON.7.man \
	man7/MX_disable_con.7.man \
	man7/MX_disable_con_t.7.man \
	man7/MX_DISABLE_IND.7.man \
	man7/MX_disable_ind.7.man \
	man7/MX_disable_ind_t.7.man \
	man7/MX_DISABLE_REQ.7.man \
	man7/MX_disable_req.7.man \
	man7/MX_disable_req_t.7.man \
	man7/MX_DISCONNECT_CON.7.man \
	man7/MX_disconnect_con.7.man \
	man7/MX_disconnect_con_t.7.man \
	man7/MX_DISCONNECT_IND.7.man \
	man7/MX_disconnect_ind.7.man \
	man7/MX_disconnect_ind_t.7.man \
	man7/MX_DISCONNECT_REQ.7.man \
	man7/MX_disconnect_req.7.man \
	man7/MX_disconnect_req_t.7.man \
	man7/MX_ENABLE_CON.7.man \
	man7/MX_enable_con.7.man \
	man7/MX_enable_con_t.7.man \
	man7/MX_ENABLE_REQ.7.man \
	man7/MX_enable_req.7.man \
	man7/MX_enable_req_t.7.man \
	man7/MX_ERROR_ACK.7.man \
	man7/MX_error_ack.7.man \
	man7/MX_error_ack_t.7.man \
	man7/mxi.7.man \
	man7/MX_INFO_ACK.7.man \
	man7/MX_info_ack.7.man \
	man7/MX_info_ack_t.7.man \
	man7/MX_INFO_REQ.7.man \
	man7/MX_info_req.7.man \
	man7/MX_info_req_t.7.man \
	man7/MX_OK_ACK.7.man \
	man7/MX_ok_ack.7.man \
	man7/MX_ok_ack_t.7.man \
	man7/MX_OPTMGMT_ACK.7.man \
	man7/MX_optmgmt_ack.7.man \
	man7/MX_optmgmt_ack_t.7.man \
	man7/MX_OPTMGMT_REQ.7.man \
	man7/MX_optmgmt_req.7.man \
	man7/MX_optmgmt_req_t.7.man \
	man8/test-chan.8.man \
	man8/strchan_mknod.8.man \
	man9/zaptel.9.man \
	man9/zt_alarm_notify.9.man \
	man9/zt_dtmf_tone.9.man \
	man9/zt_dynamic_receive.9.man \
	man9/zt_dynamic_register.9.man \
	man9/zt_dynamic_unregister.9.man \
	man9/zt_ec_chunk.9.man \
	man9/zt_ec_span.9.man \
	man9/zt_hooksig.9.man \
	man9/zt_init_tone_state.9.man \
	man9/zt_lboname.9.man \
	man9/zt_qevent_lock.9.man \
	man9/zt_qevent_nolock.9.man \
	man9/zt_rbsbits.9.man \
	man9/zt_receive.9.man \
	man9/zt_register.9.man \
	man9/zt_set_dynamic_ioctl.9.man \
	man9/zt_tone_nextsample.9.man \
	man9/zt_transmit.9.man \
	man9/zt_txtone_nextsample.9.man \
	man9/zt_unregister.9.man

MANS_UNUSED = \
	man4/ch.4 \
	man4/ch_ioctl.4 \
	man4/mg_ioctl.4 \
	man4/mx_ioctl.4 \
	man4/mx_sdl.4 \
	man4/v401p.4 \
	man5/strchan.5 \
	man9/zaptel.9 \
	man9/zt_alarm_notify.9 \
	man9/zt_dtmf_tone.9 \
	man9/zt_dynamic_receive.9 \
	man9/zt_dynamic_register.9 \
	man9/zt_dynamic_unregister.9 \
	man9/zt_ec_chunk.9 \
	man9/zt_ec_span.9 \
	man9/zt_hooksig.9 \
	man9/zt_init_tone_state.9 \
	man9/zt_lboname.9 \
	man9/zt_qevent_lock.9 \
	man9/zt_qevent_nolock.9 \
	man9/zt_rbsbits.9 \
	man9/zt_receive.9 \
	man9/zt_register.9 \
	man9/zt_set_dynamic_ioctl.9 \
	man9/zt_tone_nextsample.9 \
	man9/zt_transmit.9 \
	man9/zt_txtone_nextsample.9 \
	man9/zt_unregister.9

## PKG_BUILD_INDEP
endif

include ../../am/man.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
