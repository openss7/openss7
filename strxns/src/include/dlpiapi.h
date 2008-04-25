/*****************************************************************************

 @(#) $Id: dlpiapi.h,v 0.9.2.3 2008-04-25 11:39:32 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2008-04-25 11:39:32 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlpiapi.h,v $
 Revision 0.9.2.3  2008-04-25 11:39:32  brian
 - updates to AGPLv3

 Revision 0.9.2.2  2007/08/14 03:31:12  brian
 - GPLv3 header update

 Revision 0.9.2.1  2007/01/05 06:14:53  brian
 - added GCOM api files, doc updates

 *****************************************************************************/

#ifndef __DLPIAPI_H__
#define __DLPIAPI_H__

#ident "@(#) $RCSfile: dlpiapi.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

#ifdef __BEGIN_DECLS
__BEGIN_DECLS
#endif
extern int dlpi_attach_ppa(int dlpi_data, unsigned long ppa);
extern int dlpi_bind_dlsap(int dlpi_data, unsigned long dlsap, int conind_nr);
extern int dlpi_clear_zombies(void);
extern int dlpi_complete_req(int dlpi_data, int request, char *caller, int discard_un_iframes);
extern int dlpi_configure_dlsaps(int dlpi_data, unsigned long local_dlsap,
				 unsigned long remote_dlsap);
extern int dlpi_connect(unsigned long ppa, unsigned long bind_dlsap, unsigned long local_dlsap,
			unsigned long remote_dlsap);
extern int dlpi_connect_req(int dlpi_data, unsigned long peer_sap);
extern int dlpi_connect_wait(int data);
extern void dlpi_decode_ctl(char *p);
extern char *dlpi_decode_disconnect_reason(long reason);
extern int dlpi_connect_req(int dlpi_data);
extern int dlpi_discon_req(int dlpi_data, int reason);
extern int dlpi_disconnect_req(int dlpi_data, int reason);
extern int dlpi_get_a_msg(int dlpi_data, char *buf, int cnt);
extern int dlpi_get_info(char *ptr);
extern int dlpi_get_style(void);
extern int dlpi_init(unsigned int log_optns, char *log_name);
extern int dlpi_init_FILE(unsigned int log_optns, FILE * log_file);
extern int dlpi_listen(unsigned long ppa, unsigned long bind_dlsap, unsigned long local_dlsap,
		       unsigned long remote_dlsap, unsigned fork_options);
extern int dlpi_open_data(void);
extern int dlpi_open_log(void);
extern int dlpi_perror(char *prefix);
extern int dlpi_print_msg(char *msg, unsigned length, int indent);
extern int dlpi_printf(char *fmt, ...);
extern int dlpi_put_both(int dlpi_data, char *hdr_ptr, int hdr_lgth, char *data_ptr, int data_lgth);
extern int dlpi_put_proto(int dlpi_data, int lgth);
extern int dlpi_rcv(int dlpi_data, char *data_ptr, int bfr_len, int flags, long *out_code);
extern int dlpi_rcv_msg(int dlpi_data, char *data_ptr, int data_cnt, int flags);
extern int dlpi_read_data(int dlpi_data, char *buf, int cnt);
extern int dlpi_reset_req(int dlpi_data);
extern int dlpi_reset_res(int dlpi_data);
extern int dlpi_send_attach_req(int dlpi_data, unsigned long ppa);
extern int dlpi_send_bind_req(int dlpi_data, unsigned long dlsap, int conind_nr, int service_mode,
			      int conn_mgnt, int auto_flags);
extern int dlpi_send_connect_req(int dlpi_data, unsigned long peer_sap);
extern int dlpi_send_connect_res(int dlpi_data, unsigned long correlation,
				 unsigned long dlpi_token);
extern int dlpi_send_detach_req(int dlpi_data);
extern int dlpi_send_disconnect_req(int dlpi_data, int reason);
extern int dlpi_send_info_req(int dlpi_data);
extern int dlpi_send_reset_req(int dlpi_data);
extern int dlpi_send_reset_res(int dlpi_data);
extern int dlpi_send_stats_req(int dlpi_data);
extern int dlpi_send_test_req(int dlpi_data, unsigned long pfb, char *datap, int length,
			      unsigned char *addr_ptr, int addr_len);
extern int dlpi_send_test_res(int dlpi_data, unsigned long pfb, char *datap, int length,
			      unsigned char *addr_ptr, int addr_len);
extern int dlpi_send_uic(int dlpi_data, cahr * datap, int data_len, unsigned char *addr_ptr,
			 int addr_len);
extern int dlpi_send_unbind_req(int dlpi_data);
extern int dlpi_send_xid_req(int dlpi_data, unsigned long pfb, char *datap, int length,
			     unsigned char *addr_ptr, int addr_len);
extern int dlpi_send_xid_res(int dlpi_data, unsigned long pfb, char *datap, int length,
			     unsigned char *addr_ptr, int addr_len);
extern int dlpi_set_log_size(long log_size);
extern int dlpi_set_signal_handling(int dlpi_data, dlpi_sig_func_t func, int sig_num,
				    int primitive_mask);
extern int dlpi_set_unnum_frame_handler(int dlpi_data, unnum_frame_t handler);
extern int dlpi_test_req(int dlpi_data, unsigned long pfb, char *datap, int length);
extern int dlpi_test_res(int dlpi_data, unsigned long pfb, char *datap, int length);
extern int dlpi_uic_req(int dlpi_data, char *datap, int length);
extern int dlpi_unbind_dlsap(int dlpi_data);
extern int dlpi_test_res(int dlpi_data, unsigned long pfb, char *datap, int length);
extern int dlpi_xid_res(int dlpi_data, unsigned long pfb, char *datap, int length);
extern int dlpi_write_data(int dlpi_data, char *buf, int cnt);
extern int dlpi_xray_req(int fid, int upa, int on_off, int hi_wat, int lo_wat);

#ifdef __END_DECLS
__END_DECLS
#endif
#endif				/* __DLPIAPI_H__ */
