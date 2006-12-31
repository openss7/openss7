# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name:  $($Revision: 0.9.2.18 $) $Date: 2006/12/31 13:57:43 $
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
# Last Modified $Date: 2006/12/31 13:57:43 $ by $Author: brian $
#
# -----------------------------------------------------------------------------
#
# $Log: Makefile.am,v $
# Revision 0.9.2.18  2006/12/31 13:57:43  brian
# - updated documentation for release
#
# Revision 0.9.2.17  2006/05/07 22:12:54  brian
# - updated for NPI-IP driver
#
# Revision 0.9.2.16  2006/05/06 10:22:21  brian
# - added test suite for NPI-IP driver
#
# Revision 0.9.2.15  2006/03/18 09:42:10  brian
# - added in ip driver and manual pages
#
# Revision 0.9.2.14  2006/03/06 11:39:17  brian
# - updated headers
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

MANS_UNUSED = \
	man4/XNS.4

man_MANS = \
	man4/XNS.4 \
	man4/ipcl.4 \
	man4/ipco.4 \
	man4/np_ip.4 \
	man4/ip2xinet.4 \
	man4/ip_strms.4 \
	man4/ip_strm_mod.4 \
	man4/ip_to_dlpi.4 \
	man4/ldl.4 \
	man4/link-driver.4 \
	man5/strxns.5 \
	man7/CD_ABORT_OUTPUT_REQ.7 \
	man7/cd_abort_output_req_t.7 \
	man7/CD_ALLOW_INPUT_REQ.7 \
	man7/cd_allow_input_req_t.7 \
	man7/CD_ATTACH_REQ.7 \
	man7/cd_attach_req_t.7 \
	man7/CD_BAD_FRAME_IND.7 \
	man7/cd_bad_frame_ind_t.7 \
	man7/CD_DETACH_REQ.7 \
	man7/cd_detach_req_t.7 \
	man7/CD_DISABLE_CON.7 \
	man7/cd_disable_con_t.7 \
	man7/CD_DISABLE_REQ.7 \
	man7/cd_disable_req_t.7 \
	man7/CD_ENABLE_CON.7 \
	man7/cd_enable_con_t.7 \
	man7/CD_ENABLE_REQ.7 \
	man7/cd_enable_req_t.7 \
	man7/CD_ERROR_ACK.7 \
	man7/cd_error_ack_t.7 \
	man7/CD_ERROR_IND.7 \
	man7/cd_error_ind_t.7 \
	man7/CD_HALT_INPUT_REQ.7 \
	man7/cd_halt_input_req_t.7 \
	man7/cdi.7 \
	man7/CD_INFO_ACK.7 \
	man7/cd_info_ack_t.7 \
	man7/CD_INFO_REQ.7 \
	man7/CD_MODEM_SIG_IND.7 \
	man7/cd_modem_sig_ind_t.7 \
	man7/CD_MODEM_SIG_POLL.7 \
	man7/cd_modem_sig_poll_t.7 \
	man7/CD_MODEM_SIG_REQ.7 \
	man7/cd_modem_sig_req_t.7 \
	man7/CD_MUX_NAME_REQ.7 \
	man7/cd_mux_name_req_t.7 \
	man7/CD_OK_ACK.7 \
	man7/cd_ok_ack_t.7 \
	man7/CD_READ_REQ.7 \
	man7/cd_read_req_t.7 \
	man7/CD_UNITDATA_ACK.7 \
	man7/cd_unitdata_ack_t.7 \
	man7/CD_UNITDATA_IND.7 \
	man7/cd_unitdata_ind_t.7 \
	man7/CD_UNITDATA_REQ.7 \
	man7/cd_unitdata_req_t.7 \
	man7/CD_WRITE_READ_REQ.7 \
	man7/cd_write_read_req_t.7 \
	man7/DL_ATTACH_REQ.7 \
	man7/dl_attach_req_t.7 \
	man7/DL_BIND_ACK.7 \
	man7/dl_bind_ack_t.7 \
	man7/DL_BIND_REQ.7 \
	man7/dl_bind_req_t.7 \
	man7/DL_CONNECT_CON.7 \
	man7/dl_connect_con_t.7 \
	man7/DL_CONNECT_IND.7 \
	man7/dl_connect_ind_t.7 \
	man7/DL_CONNECT_REQ.7 \
	man7/dl_connect_req_t.7 \
	man7/DL_CONNECT_RES.7 \
	man7/dl_connect_res_t.7 \
	man7/DL_DATA_ACK_IND.7 \
	man7/dl_data_ack_ind_t.7 \
	man7/DL_DATA_ACK_REQ.7 \
	man7/dl_data_ack_req_t.7 \
	man7/DL_DATA_ACK_STATUS_IND.7 \
	man7/dl_data_ack_status_ind_t.7 \
	man7/DL_DATA_IND.7 \
	man7/DL_DATA_REQ.7 \
	man7/DL_DETACH_REQ.7 \
	man7/dl_detach_req_t.7 \
	man7/DL_DISABMULTI_REQ.7 \
	man7/dl_disabmulti_req_t.7 \
	man7/DL_DISCONNECT_IND.7 \
	man7/dl_disconnect_ind_t.7 \
	man7/DL_DISCONNECT_REQ.7 \
	man7/dl_disconnect_req_t.7 \
	man7/DL_ENABMULTI_REQ.7 \
	man7/dl_enabmulti_req_t.7 \
	man7/DL_ERROR_ACK.7 \
	man7/dl_error_ack_t.7 \
	man7/DL_GET_STATISTICS_ACK.7 \
	man7/dl_get_statistics_ack_t.7 \
	man7/DL_GET_STATISTICS_REQ.7 \
	man7/dl_get_statistics_req_t.7 \
	man7/DL_INFO_ACK.7 \
	man7/dl_info_ack_t.7 \
	man7/DL_INFO_REQ.7 \
	man7/dl_info_req_t.7 \
	man7/DL_OK_ACK.7 \
	man7/dl_ok_ack_t.7 \
	man7/DL_PHYS_ADDR_ACK.7 \
	man7/dl_phys_addr_ack_t.7 \
	man7/DL_PHYS_ADDR_REQ.7 \
	man7/dl_phys_addr_req_t.7 \
	man7/dlpi.7 \
	man7/dlpi_eth.7 \
	man7/DL_PROMISCOFF_REQ.7 \
	man7/dl_promiscoff_req_t.7 \
	man7/DL_PROMISCON_REQ.7 \
	man7/dl_promiscon_req_t.7 \
	man7/DL_REPLY_IND.7 \
	man7/dl_reply_ind_t.7 \
	man7/DL_REPLY_REQ.7 \
	man7/dl_reply_req_t.7 \
	man7/DL_REPLY_STATUS_IND.7 \
	man7/dl_reply_status_ind_t.7 \
	man7/DL_REPLY_UPDATE_REQ.7 \
	man7/dl_reply_update_req_t.7 \
	man7/DL_REPLY_UPDATE_STATUS_IND.7 \
	man7/dl_reply_update_status_ind_t.7 \
	man7/DL_RESET_CON.7 \
	man7/dl_reset_con_t.7 \
	man7/DL_RESET_IND.7 \
	man7/dl_reset_ind_t.7 \
	man7/DL_RESET_REQ.7 \
	man7/dl_reset_req_t.7 \
	man7/DL_RESET_RES.7 \
	man7/dl_reset_res_t.7 \
	man7/DL_SET_PHYS_ADDR_REQ.7 \
	man7/dl_set_phys_addr_req_t.7 \
	man7/DL_SUBS_BIND_ACK.7 \
	man7/dl_subs_bind_ack_t.7 \
	man7/DL_SUBS_BIND_REQ.7 \
	man7/dl_subs_bind_req_t.7 \
	man7/DL_SUBS_UNBIND_REQ.7 \
	man7/dl_subs_unbind_req_t.7 \
	man7/DL_TEST_CON.7 \
	man7/dl_test_con_t.7 \
	man7/DL_TEST_IND.7 \
	man7/dl_test_ind_t.7 \
	man7/DL_TEST_REQ.7 \
	man7/dl_test_req_t.7 \
	man7/DL_TEST_RES.7 \
	man7/dl_test_res_t.7 \
	man7/DL_TOKEN_ACK.7 \
	man7/dl_token_ack_t.7 \
	man7/DL_TOKEN_REQ.7 \
	man7/dl_token_req_t.7 \
	man7/DL_UDERROR_IND.7 \
	man7/dl_uderror_ind_t.7 \
	man7/DL_UDQOS_REQ.7 \
	man7/dl_udqos_req_t.7 \
	man7/DL_UNBIND_REQ.7 \
	man7/dl_unbind_req_t.7 \
	man7/DL_UNITDATA_IND.7 \
	man7/dl_unitdata_ind_t.7 \
	man7/DL_UNITDATA_REQ.7 \
	man7/dl_unitdata_req_t.7 \
	man7/DL_XID_CON.7 \
	man7/dl_xid_con_t.7 \
	man7/DL_XID_IND.7 \
	man7/dl_xid_ind_t.7 \
	man7/DL_XID_REQ.7 \
	man7/dl_xid_req_t.7 \
	man7/DL_XID_RES.7 \
	man7/dl_xid_res_t.7 \
	man7/N_BIND_ACK.7 \
	man7/N_bind_ack_t.7 \
	man7/N_BIND_REQ.7 \
	man7/N_bind_req_t.7 \
	man7/N_CONN_CON.7 \
	man7/N_conn_con_t.7 \
	man7/N_CONN_IND.7 \
	man7/N_conn_ind_t.7 \
	man7/N_CONN_REQ.7 \
	man7/N_conn_req_t.7 \
	man7/N_CONN_RES.7 \
	man7/N_conn_res_t.7 \
	man7/N_DATACK_IND.7 \
	man7/N_datack_ind_t.7 \
	man7/N_DATACK_REQ.7 \
	man7/N_datack_req_t.7 \
	man7/N_DATA_IND.7 \
	man7/N_data_ind_t.7 \
	man7/N_DATA_REQ.7 \
	man7/N_data_req_t.7 \
	man7/N_DISCON_IND.7 \
	man7/N_discon_ind_t.7 \
	man7/N_DISCON_REQ.7 \
	man7/N_discon_req_t.7 \
	man7/N_ERROR_ACK.7 \
	man7/N_error_ack_t.7 \
	man7/N_EXDATA_IND.7 \
	man7/N_exdata_ind_t.7 \
	man7/N_EXDATA_REQ.7 \
	man7/N_exdata_req_t.7 \
	man7/N_INFO_ACK.7 \
	man7/N_info_ack_t.7 \
	man7/N_INFO_REQ.7 \
	man7/N_info_req_t.7 \
	man7/N_OK_ACK.7 \
	man7/N_ok_ack_t.7 \
	man7/N_OPTMGMT_REQ.7 \
	man7/N_optmgmt_req_t.7 \
	man7/npi.7 \
	man7/npi_ip.7 \
	man7/N_RESET_CON.7 \
	man7/N_reset_con_t.7 \
	man7/N_RESET_IND.7 \
	man7/N_reset_ind_t.7 \
	man7/N_RESET_REQ.7 \
	man7/N_reset_req_t.7 \
	man7/N_RESET_RES.7 \
	man7/N_reset_res_t.7 \
	man7/N_UDERROR_IND.7 \
	man7/N_uderror_ind_t.7 \
	man7/N_UNBIND_REQ.7 \
	man7/N_unbind_req_t.7 \
	man7/N_UNITDATA_IND.7 \
	man7/N_unitdata_ind_t.7 \
	man7/N_UNITDATA_REQ.7 \
	man7/N_unitdata_req_t.7 \
	man7/soon.7 \
	man8/ldlconfig.8 \
	man8/ldltest.8 \
	man8/strxns_mknod.8 \
	man8/test-np_ip.8
#	man4/ip.4
#	man8/test-ip.8

dist_noinst_MANS = \
	man4/XNS.4.man \
	man4/ipcl.4.man \
	man4/ipco.4.man \
	man4/np_ip.4.man \
	man4/ip2xinet.4.man \
	man4/ip_strms.4.man \
	man4/ip_strm_mod.4.man \
	man4/ip_to_dlpi.4.man \
	man4/ldl.4.man \
	man4/link-driver.4.man \
	man5/strxns.5.man \
	man7/CD_ABORT_OUTPUT_REQ.7.man \
	man7/cd_abort_output_req_t.7.man \
	man7/CD_ALLOW_INPUT_REQ.7.man \
	man7/cd_allow_input_req_t.7.man \
	man7/CD_ATTACH_REQ.7.man \
	man7/cd_attach_req_t.7.man \
	man7/CD_BAD_FRAME_IND.7.man \
	man7/cd_bad_frame_ind_t.7.man \
	man7/CD_DETACH_REQ.7.man \
	man7/cd_detach_req_t.7.man \
	man7/CD_DISABLE_CON.7.man \
	man7/cd_disable_con_t.7.man \
	man7/CD_DISABLE_REQ.7.man \
	man7/cd_disable_req_t.7.man \
	man7/CD_ENABLE_CON.7.man \
	man7/cd_enable_con_t.7.man \
	man7/CD_ENABLE_REQ.7.man \
	man7/cd_enable_req_t.7.man \
	man7/CD_ERROR_ACK.7.man \
	man7/cd_error_ack_t.7.man \
	man7/CD_ERROR_IND.7.man \
	man7/cd_error_ind_t.7.man \
	man7/CD_HALT_INPUT_REQ.7.man \
	man7/cd_halt_input_req_t.7.man \
	man7/cdi.7.man \
	man7/CD_INFO_ACK.7.man \
	man7/cd_info_ack_t.7.man \
	man7/CD_INFO_REQ.7.man \
	man7/CD_MODEM_SIG_IND.7.man \
	man7/cd_modem_sig_ind_t.7.man \
	man7/CD_MODEM_SIG_POLL.7.man \
	man7/cd_modem_sig_poll_t.7.man \
	man7/CD_MODEM_SIG_REQ.7.man \
	man7/cd_modem_sig_req_t.7.man \
	man7/CD_MUX_NAME_REQ.7.man \
	man7/cd_mux_name_req_t.7.man \
	man7/CD_OK_ACK.7.man \
	man7/cd_ok_ack_t.7.man \
	man7/CD_READ_REQ.7.man \
	man7/cd_read_req_t.7.man \
	man7/CD_UNITDATA_ACK.7.man \
	man7/cd_unitdata_ack_t.7.man \
	man7/CD_UNITDATA_IND.7.man \
	man7/cd_unitdata_ind_t.7.man \
	man7/CD_UNITDATA_REQ.7.man \
	man7/cd_unitdata_req_t.7.man \
	man7/CD_WRITE_READ_REQ.7.man \
	man7/cd_write_read_req_t.7.man \
	man7/DL_ATTACH_REQ.7.man \
	man7/dl_attach_req_t.7.man \
	man7/DL_BIND_ACK.7.man \
	man7/dl_bind_ack_t.7.man \
	man7/DL_BIND_REQ.7.man \
	man7/dl_bind_req_t.7.man \
	man7/DL_CONNECT_CON.7.man \
	man7/dl_connect_con_t.7.man \
	man7/DL_CONNECT_IND.7.man \
	man7/dl_connect_ind_t.7.man \
	man7/DL_CONNECT_REQ.7.man \
	man7/dl_connect_req_t.7.man \
	man7/DL_CONNECT_RES.7.man \
	man7/dl_connect_res_t.7.man \
	man7/DL_DATA_ACK_IND.7.man \
	man7/dl_data_ack_ind_t.7.man \
	man7/DL_DATA_ACK_REQ.7.man \
	man7/dl_data_ack_req_t.7.man \
	man7/DL_DATA_ACK_STATUS_IND.7.man \
	man7/dl_data_ack_status_ind_t.7.man \
	man7/DL_DATA_IND.7.man \
	man7/DL_DATA_REQ.7.man \
	man7/DL_DETACH_REQ.7.man \
	man7/dl_detach_req_t.7.man \
	man7/DL_DISABMULTI_REQ.7.man \
	man7/dl_disabmulti_req_t.7.man \
	man7/DL_DISCONNECT_IND.7.man \
	man7/dl_disconnect_ind_t.7.man \
	man7/DL_DISCONNECT_REQ.7.man \
	man7/dl_disconnect_req_t.7.man \
	man7/DL_ENABMULTI_REQ.7.man \
	man7/dl_enabmulti_req_t.7.man \
	man7/DL_ERROR_ACK.7.man \
	man7/dl_error_ack_t.7.man \
	man7/DL_GET_STATISTICS_ACK.7.man \
	man7/dl_get_statistics_ack_t.7.man \
	man7/DL_GET_STATISTICS_REQ.7.man \
	man7/dl_get_statistics_req_t.7.man \
	man7/DL_INFO_ACK.7.man \
	man7/dl_info_ack_t.7.man \
	man7/DL_INFO_REQ.7.man \
	man7/dl_info_req_t.7.man \
	man7/DL_OK_ACK.7.man \
	man7/dl_ok_ack_t.7.man \
	man7/DL_PHYS_ADDR_ACK.7.man \
	man7/dl_phys_addr_ack_t.7.man \
	man7/DL_PHYS_ADDR_REQ.7.man \
	man7/dl_phys_addr_req_t.7.man \
	man7/dlpi.7.man \
	man7/dlpi_eth.7.man \
	man7/DL_PROMISCOFF_REQ.7.man \
	man7/dl_promiscoff_req_t.7.man \
	man7/DL_PROMISCON_REQ.7.man \
	man7/dl_promiscon_req_t.7.man \
	man7/DL_REPLY_IND.7.man \
	man7/dl_reply_ind_t.7.man \
	man7/DL_REPLY_REQ.7.man \
	man7/dl_reply_req_t.7.man \
	man7/DL_REPLY_STATUS_IND.7.man \
	man7/dl_reply_status_ind_t.7.man \
	man7/DL_REPLY_UPDATE_REQ.7.man \
	man7/dl_reply_update_req_t.7.man \
	man7/DL_REPLY_UPDATE_STATUS_IND.7.man \
	man7/dl_reply_update_status_ind_t.7.man \
	man7/DL_RESET_CON.7.man \
	man7/dl_reset_con_t.7.man \
	man7/DL_RESET_IND.7.man \
	man7/dl_reset_ind_t.7.man \
	man7/DL_RESET_REQ.7.man \
	man7/dl_reset_req_t.7.man \
	man7/DL_RESET_RES.7.man \
	man7/dl_reset_res_t.7.man \
	man7/DL_SET_PHYS_ADDR_REQ.7.man \
	man7/dl_set_phys_addr_req_t.7.man \
	man7/DL_SUBS_BIND_ACK.7.man \
	man7/dl_subs_bind_ack_t.7.man \
	man7/DL_SUBS_BIND_REQ.7.man \
	man7/dl_subs_bind_req_t.7.man \
	man7/DL_SUBS_UNBIND_REQ.7.man \
	man7/dl_subs_unbind_req_t.7.man \
	man7/DL_TEST_CON.7.man \
	man7/dl_test_con_t.7.man \
	man7/DL_TEST_IND.7.man \
	man7/dl_test_ind_t.7.man \
	man7/DL_TEST_REQ.7.man \
	man7/dl_test_req_t.7.man \
	man7/DL_TEST_RES.7.man \
	man7/dl_test_res_t.7.man \
	man7/DL_TOKEN_ACK.7.man \
	man7/dl_token_ack_t.7.man \
	man7/DL_TOKEN_REQ.7.man \
	man7/dl_token_req_t.7.man \
	man7/DL_UDERROR_IND.7.man \
	man7/dl_uderror_ind_t.7.man \
	man7/DL_UDQOS_REQ.7.man \
	man7/dl_udqos_req_t.7.man \
	man7/DL_UNBIND_REQ.7.man \
	man7/dl_unbind_req_t.7.man \
	man7/DL_UNITDATA_IND.7.man \
	man7/dl_unitdata_ind_t.7.man \
	man7/DL_UNITDATA_REQ.7.man \
	man7/dl_unitdata_req_t.7.man \
	man7/DL_XID_CON.7.man \
	man7/dl_xid_con_t.7.man \
	man7/DL_XID_IND.7.man \
	man7/dl_xid_ind_t.7.man \
	man7/DL_XID_REQ.7.man \
	man7/dl_xid_req_t.7.man \
	man7/DL_XID_RES.7.man \
	man7/dl_xid_res_t.7.man \
	man7/N_BIND_ACK.7.man \
	man7/N_bind_ack_t.7.man \
	man7/N_BIND_REQ.7.man \
	man7/N_bind_req_t.7.man \
	man7/N_CONN_CON.7.man \
	man7/N_conn_con_t.7.man \
	man7/N_CONN_IND.7.man \
	man7/N_conn_ind_t.7.man \
	man7/N_CONN_REQ.7.man \
	man7/N_conn_req_t.7.man \
	man7/N_CONN_RES.7.man \
	man7/N_conn_res_t.7.man \
	man7/N_DATACK_IND.7.man \
	man7/N_datack_ind_t.7.man \
	man7/N_DATACK_REQ.7.man \
	man7/N_datack_req_t.7.man \
	man7/N_DATA_IND.7.man \
	man7/N_data_ind_t.7.man \
	man7/N_DATA_REQ.7.man \
	man7/N_data_req_t.7.man \
	man7/N_DISCON_IND.7.man \
	man7/N_discon_ind_t.7.man \
	man7/N_DISCON_REQ.7.man \
	man7/N_discon_req_t.7.man \
	man7/N_ERROR_ACK.7.man \
	man7/N_error_ack_t.7.man \
	man7/N_EXDATA_IND.7.man \
	man7/N_exdata_ind_t.7.man \
	man7/N_EXDATA_REQ.7.man \
	man7/N_exdata_req_t.7.man \
	man7/N_INFO_ACK.7.man \
	man7/N_info_ack_t.7.man \
	man7/N_INFO_REQ.7.man \
	man7/N_info_req_t.7.man \
	man7/N_OK_ACK.7.man \
	man7/N_ok_ack_t.7.man \
	man7/N_OPTMGMT_REQ.7.man \
	man7/N_optmgmt_req_t.7.man \
	man7/npi.7.man \
	man7/npi_ip.7.man \
	man7/N_RESET_CON.7.man \
	man7/N_reset_con_t.7.man \
	man7/N_RESET_IND.7.man \
	man7/N_reset_ind_t.7.man \
	man7/N_RESET_REQ.7.man \
	man7/N_reset_req_t.7.man \
	man7/N_RESET_RES.7.man \
	man7/N_reset_res_t.7.man \
	man7/N_UDERROR_IND.7.man \
	man7/N_uderror_ind_t.7.man \
	man7/N_UNBIND_REQ.7.man \
	man7/N_unbind_req_t.7.man \
	man7/N_UNITDATA_IND.7.man \
	man7/N_unitdata_ind_t.7.man \
	man7/N_UNITDATA_REQ.7.man \
	man7/N_unitdata_req_t.7.man \
	man7/soon.7.man \
	man8/ldlconfig.8.man \
	man8/ldltest.8.man \
	man8/strxns_mknod.8.man \
	man8/test-np_ip.8.man
#	man4/ip.4.man
#	man8/test-ip.8.man

## PKG_BUILD_INDEP
endif

include ../../am/man.am

## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor
