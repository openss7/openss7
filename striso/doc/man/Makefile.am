# =============================================================================
# 
# @(#) $RCSfile: Makefile.am,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.7 $) $Date: 2007-11-10 19:40:47 $
#
# -----------------------------------------------------------------------------
#
# Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
# Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>
#
# All Rights Reserved.
#
# This program is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free Software
# Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
# FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License along with
# this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
# Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
# Last Modified $Date: 2007-11-10 19:40:47 $ by $Author: brian $
#
# =============================================================================

include ../../am/openss7.am

MANMACRO_PFX			= @PACKAGE_TARNAME@

if PKG_BUILD_INDEP
## PKG_BUILD_INDEP

MANS_UNUSED = \
	man4/iso-ip.4 \
	man4/iso-udp.4 \
	man4/isos.4 \
	man4/xol.4 \
	man5/striso.5

man_MANS = \
	man3/ap_bind.3 \
	man3/ap_close.3 \
	man3/ap_error.3 \
	man3/ap_free.3 \
	man3/ap_get_env.3 \
	man3/ap_init_env.3 \
	man3/ap_ioctl.3 \
	man3/ap_look.3 \
	man3/ap_open.3 \
	man3/ap_poll.3 \
	man3/ap_rcv.3 \
	man3/ap_restore.3 \
	man3/ap_save.3 \
	man3/ap_set_env.3 \
	man3/ap_snd.3 \
	man3/ds_abandon.3 \
	man3/ds_add_entry.3 \
	man3/ds_bind.3 \
	man3/ds_compare.3 \
	man3/ds_initialize.3 \
	man3/ds_list.3 \
	man3/ds_modify_entry.3 \
	man3/ds_modify_rdn.3 \
	man3/ds_read.3 \
	man3/ds_receive_result.3 \
	man3/ds_remove_entry.3 \
	man3/ds_search.3 \
	man3/ds_shutdown.3 \
	man3/ds_unbind.3 \
	man3/ds_version.3 \
	man3/getmyclnsnsap.3 \
	man3/getmyconsnsap.3 \
	man3/getnamebynsap.3 \
	man3/getnsapbyname.3 \
	man3/gettselbyname.3 \
	man3/mp_abandon.3 \
	man3/mp_abort.3 \
	man3/mp_abort_req.3 \
	man3/mp_action_req.3 \
	man3/mp_action_rsp.3 \
	man3/mp_assoc_req.3 \
	man3/mp_assoc_rsp.3 \
	man3/mp_bind.3 \
	man3/mp_cancel_get_req.3 \
	man3/mp_cancel_get_rsp.3 \
	man3/mp_create_req.3 \
	man3/mp_create_rsp.3 \
	man3/mp_delete_req.3 \
	man3/mp_delete_rsp.3 \
	man3/mp_error_message.3 \
	man3/mp_event_report_req.3 \
	man3/mp_event_report_rsp.3 \
	man3/mp_get_assoc_info.3 \
	man3/mp_get_last_error.3 \
	man3/mp_get_next_req.3 \
	man3/mp_get_req.3 \
	man3/mp_get_rsp.3 \
	man3/mp_initialize.3 \
	man3/mp_negotiate.3 \
	man3/mp_receive.3 \
	man3/mp_release_req.3 \
	man3/mp_release_rsp.3 \
	man3/mp_set_req.3 \
	man3/mp_set_rsp.3 \
	man3/mp_shutdown.3 \
	man3/mp_unbind.3 \
	man3/mp_validate_object.3 \
	man3/mp_wait.3 \
	man3/ms_bind.3 \
	man3/ms_cancel_submission.3 \
	man3/ms_check_alert.3 \
	man3/ms_delete.3 \
	man3/ms_fetch.3 \
	man3/ms_initialize.3 \
	man3/ms_list.3 \
	man3/ms_receive_result.3 \
	man3/ms_register.3 \
	man3/ms_register_ms.3 \
	man3/ms_shutdown.3 \
	man3/ms_submit.3 \
	man3/ms_summarize.3 \
	man3/ms_unbind.3 \
	man3/ms_wait.3 \
	man3/nsap2net.3 \
	man3/om_copy.3 \
	man3/om_copy_value.3 \
	man3/om_create.3 \
	man3/om_decode.3 \
	man3/om_delete.3 \
	man3/om_encode.3 \
	man3/om_get.3 \
	man3/om_instance.3 \
	man3/om_put.3 \
	man3/om_read.3 \
	man3/om_remove.3 \
	man3/om_write.3 \
	man3/tsap2net.3 \
	man3/apli.3 \
	man3/APLI.3 \
	man3/osi.3 \
	man3/OSI.3 \
	man3/XAP-ROSE.3 \
	man3/xap.3 \
	man3/XAP.3 \
	man3/xds.3 \
	man3/XDS.3 \
	man3/xftam.3 \
	man3/XFTAM.3 \
	man3/xmp.3 \
	man3/XMP.3 \
	man3/XMS.3 \
	man3/xom.3 \
	man3/XOM.3 \
	man3/XX25.3 \
	man4/clns.4 \
	man4/cmot.4 \
	man4/iso-ip.4 \
	man4/isos.4 \
	man4/isot.4 \
	man4/iso-lan.4 \
	man4/iso-udp.4 \
	man4/itos.4 \
	man4/itot.4 \
	man4/lapb.4 \
	man4/lpp.4 \
	man4/ns-tcp.4 \
	man4/plp.4 \
	man4/tcpns.4 \
	man4/tp0.4 \
	man4/tp1.4 \
	man4/tp2.4 \
	man4/tp3.4 \
	man4/tp4.4 \
	man4/tp.4 \
	man4/x25-lapb.4 \
	man4/x25-plp.4 \
	man4/x25_lapb.4 \
	man4/x25_plp.4 \
	man4/xol.4 \
	man4/xot.4 \
	man5/striso.5 \
	man7/npi_osi.7 \
	man8/gdmo.8 \
	man8/striso_mknod.8

dist_noinst_MANS = \
	man3/ap_bind.3.man \
	man3/ap_close.3.man \
	man3/ap_error.3.man \
	man3/ap_free.3.man \
	man3/ap_get_env.3.man \
	man3/ap_init_env.3.man \
	man3/ap_ioctl.3.man \
	man3/ap_look.3.man \
	man3/ap_open.3.man \
	man3/ap_poll.3.man \
	man3/ap_rcv.3.man \
	man3/ap_restore.3.man \
	man3/ap_save.3.man \
	man3/ap_set_env.3.man \
	man3/ap_snd.3.man \
	man3/ds_abandon.3.man \
	man3/ds_add_entry.3.man \
	man3/ds_bind.3.man \
	man3/ds_compare.3.man \
	man3/ds_initialize.3.man \
	man3/ds_list.3.man \
	man3/ds_modify_entry.3.man \
	man3/ds_modify_rdn.3.man \
	man3/ds_read.3.man \
	man3/ds_receive_result.3.man \
	man3/ds_remove_entry.3.man \
	man3/ds_search.3.man \
	man3/ds_shutdown.3.man \
	man3/ds_unbind.3.man \
	man3/ds_version.3.man \
	man3/getmyclnsnsap.3.man \
	man3/getmyconsnsap.3.man \
	man3/getnamebynsap.3.man \
	man3/getnsapbyname.3.man \
	man3/gettselbyname.3.man \
	man3/mp_abandon.3.man \
	man3/mp_abort.3.man \
	man3/mp_abort_req.3.man \
	man3/mp_action_req.3.man \
	man3/mp_action_rsp.3.man \
	man3/mp_assoc_req.3.man \
	man3/mp_assoc_rsp.3.man \
	man3/mp_bind.3.man \
	man3/mp_cancel_get_req.3.man \
	man3/mp_cancel_get_rsp.3.man \
	man3/mp_create_req.3.man \
	man3/mp_create_rsp.3.man \
	man3/mp_delete_req.3.man \
	man3/mp_delete_rsp.3.man \
	man3/mp_error_message.3.man \
	man3/mp_event_report_req.3.man \
	man3/mp_event_report_rsp.3.man \
	man3/mp_get_assoc_info.3.man \
	man3/mp_get_last_error.3.man \
	man3/mp_get_next_req.3.man \
	man3/mp_get_req.3.man \
	man3/mp_get_rsp.3.man \
	man3/mp_initialize.3.man \
	man3/mp_negotiate.3.man \
	man3/mp_receive.3.man \
	man3/mp_release_req.3.man \
	man3/mp_release_rsp.3.man \
	man3/mp_set_req.3.man \
	man3/mp_set_rsp.3.man \
	man3/mp_shutdown.3.man \
	man3/mp_unbind.3.man \
	man3/mp_validate_object.3.man \
	man3/mp_wait.3.man \
	man3/ms_bind.3.man \
	man3/ms_cancel_submission.3.man \
	man3/ms_check_alert.3.man \
	man3/ms_delete.3.man \
	man3/ms_fetch.3.man \
	man3/ms_initialize.3.man \
	man3/ms_list.3.man \
	man3/ms_receive_result.3.man \
	man3/ms_register.3.man \
	man3/ms_register_ms.3.man \
	man3/ms_shutdown.3.man \
	man3/ms_submit.3.man \
	man3/ms_summarize.3.man \
	man3/ms_unbind.3.man \
	man3/ms_wait.3.man \
	man3/nsap2net.3.man \
	man3/om_copy.3.man \
	man3/om_copy_value.3.man \
	man3/om_create.3.man \
	man3/om_decode.3.man \
	man3/om_delete.3.man \
	man3/om_encode.3.man \
	man3/om_get.3.man \
	man3/om_instance.3.man \
	man3/om_put.3.man \
	man3/om_read.3.man \
	man3/om_remove.3.man \
	man3/om_write.3.man \
	man3/tsap2net.3.man \
	man3/apli.3.man \
	man3/APLI.3.man \
	man3/osi.3.man \
	man3/OSI.3.man \
	man3/XAP-ROSE.3.man \
	man3/xap.3.man \
	man3/XAP.3.man \
	man3/xds.3.man \
	man3/XDS.3.man \
	man3/xftam.3.man \
	man3/XFTAM.3.man \
	man3/xmp.3.man \
	man3/XMP.3.man \
	man3/XMS.3.man \
	man3/xom.3.man \
	man3/XOM.3.man \
	man3/XX25.3.man \
	man4/clns.4.man \
	man4/cmot.4.man \
	man4/iso-ip.4.man \
	man4/isos.4.man \
	man4/isot.4.man \
	man4/iso-lan.4.man \
	man4/iso-udp.4.man \
	man4/itos.4.man \
	man4/itot.4.man \
	man4/lapb.4.man \
	man4/lpp.4.man \
	man4/ns-tcp.4.man \
	man4/plp.4.man \
	man4/tcpns.4.man \
	man4/tp0.4.man \
	man4/tp1.4.man \
	man4/tp2.4.man \
	man4/tp3.4.man \
	man4/tp4.4.man \
	man4/tp.4.man \
	man4/x25-lapb.4.man \
	man4/x25-plp.4.man \
	man4/x25_lapb.4.man \
	man4/x25_plp.4.man \
	man4/xol.4.man \
	man4/xot.4.man \
	man5/striso.5.man \
	man7/npi_osi.7.man \
	man8/gdmo.8.man \
	man8/striso_mknod.8.man

## PKG_BUILD_INDEP
endif

include ../../am/man.am

# =============================================================================
#
# $Log: Makefile.am,v $
# Revision 0.9.2.7  2007-11-10 19:40:47  brian
# - documentation updates
#
# Revision 0.9.2.6  2007/08/14 07:04:55  brian
# - GNUv3 header update
#
# Revision 0.9.2.5  2006/12/29 05:51:44  brian
# - changes for successful master build
#
# Revision 0.9.2.4  2006/10/12 20:25:14  brian
# - manual page fixups for release
#
# Revision 0.9.2.3  2006/10/03 13:52:16  brian
# - changes to pass make check target
# - added some package config.h files
# - removed AUTOCONFIG_H from Makefile.am's
# - source code changes for compile
# - added missing manual pages
# - renamed conflicting manual pages
# - parameterized include Makefile.am
# - updated release notes
#
# Revision 0.9.2.2  2006/07/11 12:32:03  brian
# - added ISO and other implementations to distribution
#
# Revision 0.9.2.1  2006/04/08 03:39:14  brian
# - working up striso package
#
# =============================================================================
## vim: ft=automake comments=b\:#,b\:##,b\:#\! formatoptions+=tcqlor nocin nosi
