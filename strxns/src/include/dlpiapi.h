/*****************************************************************************

 @(#) $Id: dlpiapi.h,v 0.9.2.5 2008-07-06 14:58:20 brian Exp $

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

 Last Modified $Date: 2008-07-06 14:58:20 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dlpiapi.h,v $
 Revision 0.9.2.5  2008-07-06 14:58:20  brian
 - improvements

 Revision 0.9.2.4  2008-07-01 12:06:40  brian
 - updated manual pages, added new API library headers and impl files

 Revision 0.9.2.3  2008-04-25 11:39:32  brian
 - updates to AGPLv3

 Revision 0.9.2.2  2007/08/14 03:31:12  brian
 - GPLv3 header update

 Revision 0.9.2.1  2007/01/05 06:14:53  brian
 - added GCOM api files, doc updates

 *****************************************************************************/

#ifndef __DLPIAPI_H__
#define __DLPIAPI_H__

#ident "@(#) $RCSfile: dlpiapi.h,v $ $Name:  $($Revision: 0.9.2.5 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* These two definitions clash with the Solaris DLPI library. */
#define dlpi_open	dlpi_open_GCOM
#define dlpi_close	dlpi_close_GCOM

#define DLPI_CTL_BUF_SIZE	5000
#define DLPI_DATA_BUF_SIZE	5000
#define DLPI_DEFAULT_PPA	~0
#define DLPI_LOG_NAME		"/var/spool/dlpi.log"
#define DLPI_N_CONINIDS		1

#define DLPIAPI_SYSERR		(-DL_SYSERR)
#define DLPIAPI_UNSUPPORTED_MSG	(-1024)
#define DLPIAPI_NO_NOTHING	(-1025)
#define DLPIAPI_STYLE_UNKNOWN	(-1026)
#define DLPIAPI_PARAM_ERROR	(-1027)
#define DLPIAPI_NOT_INIT	(-1028)
#define DLPIAPI_OPEN_ERROR	(-1029)
#define DLPIAPI_REJECT		(-1030)
#define DLPIAPI_UNUSABLE	(-1031)
#define DLPIAPI_EINTR		(-1032)
#define DLPIAPI_EAGAIN		(-1033)

#define DLPI_LOG_FILE		(1<< 0)
#define DLPI_LOG_STDERR		(1<< 1)
#define DLPI_LOG_RX_PROTOS	(1<< 2)
#define DLPI_LOG_TX_PROTOS	(1<< 3)
#define DLPI_LOG_ERRORS		(1<< 4)
#define DLPI_LOG_SIGNALS	(1<< 5)
#define DLPI_LOG_RX_DATA	(1<< 6)
#define DLPI_LOG_TX_DATA	(1<< 7)
#define DLPI_LOG_DISCONNECTS	(1<< 8)
#define DLPI_LOG_RESETS		(1<< 9)
#define DLPI_LOG_VERBOSE	(1<<10)
#define DLPI_LOG_DEFAULT \
	( DLPI_LOG_FILE \
	| DLPI_LOG_STDERR \
	| DLPI_LOG_ERRORS \
	| DLPI_LOG_DISCONNECTS \
	| DLPI_LOG_RESETS \
	)

#define dlpi_bind_ack	(_dlpi_bind_ack())
#define dlpi_conn_con	(_dlpi_conn_con())
#define dlpi_conn_ind	(_dlpi_conn_ind())
#define dlpi_ctl_buf	(_dlpi_ctl_buf())
#define dlpi_ctl_cnt	(*_dlpi_ctl_cnt())
#define dlpi_data_buf	(_dlpi_data_buf())
#define dlpi_data_cnt	(*_dlpi_data_cnt())

typedef int (*dlpi_sig_func_t)(int, char *, int, char *, int);
typedef void (*unnum_frame_t)(int, unsigned char *, int, unsigned char *, int);

#ifdef __BEGIN_DECLS
__BEGIN_DECLS
#endif
extern int dlpi_attach_ppa(int fd, ulong ppa);
extern int dlpi_bind_dlsap(int fd, ulong dlsap, int conind_nr);
extern int dlpi_clear_zombies(void);
extern int dlpi_complete_req(int fd, int request, char *caller, int discard_un_iframes);
extern int dlpi_configure_dlsaps(int fd, ulong local_dlsap, ulong remote_dlsap);
extern int dlpi_connect(ulong ppa, ulong bind_dlsap, ulong local_dlsap, ulong remote_dlsap);
extern int dlpi_connect_req(int fd, ulong peer_sap);
extern int dlpi_connect_wait(int data);
extern void dlpi_decode_ctl(char *p);
extern char *dlpi_decode_disconnect_reason(long reason);
extern int dlpi_discon_req(int fd, int reason);
extern int dlpi_disconnect_req(int fd, int reason);
extern int dlpi_get_a_msg(int fd, char *buf, int cnt);
extern int dlpi_get_info(char *ptr);
extern int dlpi_get_style(void);
extern int dlpi_init(uint log_optns, char *log_name);
extern int dlpi_init_FILE(uint log_optns, FILE *log_file);
extern int dlpi_listen(ulong ppa, ulong bind_dlsap, ulong local_dlsap, ulong remote_dlsap,
		       uint fork_options);
extern int dlpi_open_data(void);
extern int dlpi_open_log(void);
extern int dlpi_perror(char *prefix);
extern int dlpi_print_msg(char *msg, uint length, int indent);
extern int dlpi_printf(char *fmt, ...);
extern int dlpi_put_both(int fd, char *hdr_ptr, int hdr_lgth, char *data_ptr, int data_lgth);
extern int dlpi_put_proto(int fd, int lgth);
extern int dlpi_rcv(int fd, char *data_ptr, int bfr_len, int flags, long *out_code);
extern int dlpi_rcv_msg(int fd, char *data_ptr, int data_cnt, int flags);
extern int dlpi_read_data(int fd, char *buf, int cnt);
extern int dlpi_reset_req(int fd);
extern int dlpi_reset_res(int fd);
extern int dlpi_send_attach_req(int fd, ulong ppa);
extern int dlpi_send_bind_req(int fd, ulong dlsap, int conind_nr, int service_mode, int conn_mgnt,
			      int auto_flags);
extern int dlpi_send_connect_req(int fd, ulong peer_sap);
extern int dlpi_send_connect_res(int fd, ulong correlation, ulong dlpi_token);
extern int dlpi_send_detach_req(int fd);
extern int dlpi_send_disconnect_req(int fd, int reason);
extern int dlpi_send_info_req(int fd);
extern int dlpi_send_reset_req(int fd);
extern int dlpi_send_reset_res(int fd);
extern int dlpi_send_stats_req(int fd);
extern int dlpi_send_test_req(int fd, ulong pfb, char *datap, int length, unsigned char *addr_ptr,
			      int addr_len);
extern int dlpi_send_test_res(int fd, ulong pfb, char *datap, int length, unsigned char *addr_ptr,
			      int addr_len);
extern int dlpi_send_uic(int fd, char * datap, int data_len, unsigned char *addr_ptr, int addr_len);
extern int dlpi_send_unbind_req(int fd);
extern int dlpi_send_xid_req(int fd, ulong pfb, char *datap, int length, unsigned char *addr_ptr,
			     int addr_len);
extern int dlpi_send_xid_res(int fd, ulong pfb, char *datap, int length, unsigned char *addr_ptr,
			     int addr_len);
extern int dlpi_set_log_size(long log_size);
extern int dlpi_set_signal_handling(int fd, dlpi_sig_func_t func, int sig_num, int primitive_mask);
extern int dlpi_set_unnum_frame_handler(int fd, unnum_frame_t handler);
extern int dlpi_test_req(int fd, ulong pfb, char *datap, int length);
extern int dlpi_test_res(int fd, ulong pfb, char *datap, int length);
extern int dlpi_uic_req(int fd, char *datap, int length);
extern int dlpi_unbind_dlsap(int fd);
extern int dlpi_test_res(int fd, ulong pfb, char *datap, int length);
extern int dlpi_xid_res(int fd, ulong pfb, char *datap, int length);
extern int dlpi_write_data(int fd, char *buf, int cnt);
extern int dlpi_xray_req(int fd, int upa, int on_off, int hi_wat, int lo_wat);

#ifdef __END_DECLS
__END_DECLS
#endif
#endif				/* __DLPIAPI_H__ */
