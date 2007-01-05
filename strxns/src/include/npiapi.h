/*****************************************************************************

 @(#) $Id: npiapi.h,v 0.9.2.1 2007/01/05 06:14:53 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/01/05 06:14:53 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: npiapi.h,v $
 Revision 0.9.2.1  2007/01/05 06:14:53  brian
 - added GCOM api files, doc updates

 *****************************************************************************/

#ifndef __NPIAPI_H__
#define __NPIAPI_H__

#ident "@(#) $RCSfile: npiapi.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* *INDENT-OFF* */
#ifdef __BEGIN_DECLS
__BEGIN_DECLS
#endif				/* __BEGIN_DECLS */
/* *INDENT-ON* */

extern char *npi_ascii_facil(int fref, char *fval, unsigned int flgth, int marker);
extern int npi_bind_ascii_nsap(int npi_data, char *bind_ascii_nsap, int conind_nr, unsigned flags);
extern int npi_bind_nsap(int npi_data, char *bind_sap, int nsap_lgth, int conind_nr,
			 unsigned flags);
extern int npi_conn_res(int npi_data, N_conn_ind_t * c, long tknval);
extern int npi_connect(char *remote_asp, unsigned bind_flags);
extern int npi_connect_req(int npi_data, char *peer_sap, char *buf, int cnt);
extern int npi_connect_wait(int npi_data);
extern int npi_datack_req(int npi_data);
extern void npi_decode_ctl(char *p);
extern char *npi_decode_primitive(long primitive);
extern char *npi_decode_reason(long reason);
extern int npi_discon_req(int npi_data, int reason, char *buf, int cnt);
extern int npi_discon_req_seq(int npi_data, int reason, long seq, char *buf, int cnt);
extern int npi_ext_bind_nsap(int npi_data, char *nsap, int nsap_lgth, char *rem_nsap, int rem_lgth,
			     long lpa, int conind_nr, unsigned int flags);
extern int npi_ext_bind_nsap_ascii(int npi_data, char *nsap, char *rem_nsap, long lpa,
				   int conind_nr, unsigned int flags);
extern int npi_ext_connect_req(int npi_data, char *peer_sap, char *buf, int cnt, char *fac_ptr,
			       int fac_cnt);
extern int npi_ext_connect_wait(int listen_fid, char *fac_ptr, int fac_cnt);
extern int npi_ext_conn_res(int npi_data, N_conn_ind_t * c, long tknval, char *fac_ptr,
			    int fac_cnt);
extern int npi_ext_listen(char *nsap, unsigned int fork_optns, char *fac_ptr, int fac_cnt);
extern int npi_ext_nbio_complete_listen(int listen_fid, int options, char *fac_ptr, int fac_cnt);
extern int npi_fac_walk(char *facp, unsigned int facl, facil_proc_t * fcn);
extern int npi_flags_connect_wait(int listen_fid, char *fac_ptr, int fac_cnt, int bind_flags);
extern int npi_flags_listen(char *nsap, unsigned int fork_optns, char *fac_ptr, int fac_cnt,
			    int bind_flags);
extern int npi_flow_req(int npi_data, unsigned long flow_incr);
extern int npi_get_a_msg(int npi_data, char *buf, int cnt);
extern int npi_get_a_proto(int npi_data);
extern int npi_get_and_log_facils(int npi_data);
extern int npi_get_facils(int npi_data, char *fac_ptr, int fac_cnt);
extern int npi_info_req(int strm);
extern int npi_init(unsigned int log_optns, char *log_name);
extern int npi_init_FILE(unsigned int log_optns, FILE * log_FILE);
extern int npi_listen(char *nsap, unsigned int fork_optns);
extern int npi_max_sdu(int npi_data);
extern int npi_nbio_complete_listen(int listen_fd, int options);
extern int npi_open_data(void);
extern void npi_perror(char *msg);
extern void npi_print_msg(unsigned char *p, unsigned n, int indent);
extern void npi_printf(char *fmt, ...);
extern int npi_put_data_buf(int npi_data, int lgth);
extern int npi_put_data_proto(int npi_data, char *data_ptr, int data_lgth, long flags);
extern int npi_put_exdata_proto(int npi_data, char *data_ptr, int data_lgth);
extern int npi_put_proto(int npi_data, int lgth);
extern int npi_rcv(int npi_data, char *buf, int cnt, long flags_in, long *flags_out);
extern int npi_read_data(int npi_data, char *buf, int cnt);
extern int npi_reset_req(int npi_data);
extern int npi_reset_res(int npi_data);
extern int npi_send_connect_req(int npi_data, char *peer_sap, char *buf, int cnt);
extern int npi_send_ext_connect_req(int npi_data, char *peer_sap, char *buf, int cnt, char *fac_ptr,
				    int fac_cnt);
extern int npi_send_info_req(int npi_data);
extern int npi_send_reset_req(int npi_data);
extern int npi_send_reset_res(int npi_data);
extern int npi_set_log_size(long nbytes);
extern int npi_set_marks(int fid, unsigned int rd_lo_make, unsigned int rd_hi_mark,
			 unsigned int wr_lo_mark, unsigned int wr_hi_mark);
extern int npi_set_pid(int fid);
extern int npi_set_signal_handling(int fid, npi_sig_func_t func, int sig_num, int primitive_mask);
extern int npi_want_a_proto(int npi_data, int proto_type);
extern int npi_write_data(int npi_data, char *buf, int cnt);
extern char *npi_x25_clear_cause(int cause);
extern char *npi_x25_diagnostic(int diagnostic);
extern char *npi_x25_registration_cause(int cause);
extern char *npi_x25_reset_cause(int cause);
extern char *npi_x25_restart_cause(int cause);
extern int put_npi_proto(int fid, int len);

#define PUE_SUCCESS		0
#define PUE_OPEN_ERROR		-1
#define PUE_MOD_STATS		-2
#define PUE_LWR_STATS		-3
#define PUE_LWR_CONFIG		-4
#define PUE_UPR_STATS		-5
#define PUE_INVALID_HANDLE	-6
#define PUE_EXPECTED_SNA_MODULE	-7
#define PUE_EXPECTED_DLPI_USER	-8
#define PUE_EXPECTED_CDI_USER	-9
#define PUE_PU_LPA_ERROR	-10
#define PUE_PORT_NOT_CONFIGURED	-11
#define PUE_PORT_NOT_IN_USER	-12
#define PUE_PROVIDER_NOT_INIT	-13
#define PUE_PU_NOT_CONFIGURED	-14
#define PUE_PU_UNKNOWN		-15
#define PUE_BOARD_UNKNOWN	-16
#define PUE_LINKAGE_ERROR	-17
#define PUE_CDI_OPEN_ERROR	-18
#define PUE_CDI_UPR_STATS	-19
#define PUE_NO_DLPI_UA		-20
#define PUE_PU_MISMATCH		-21
#define PUE_PU_AMBIGUOUS	-22
#define PUE_UNSUPPORTED_MODULE	-23

extern char *pu_decode_handle(unsigned long pu_handle);
extern int pu_dlpi_upa(int board, int port, int station, int *upa_ptr);
extern int pu_get_pu_id(int board, int port, int station, int *pu_id_ptr,
			unsigned long *pu_hndl_ptr);
extern int pu_get_stats(unsigned long pu_handle, int pu_id, int *pu_up_down_ptr,
			int *link_state_ptr, int *modem_state_ptr);
extern int pu_get_board_info(unsigned long pu_handle, int pu_id, int *board_ptr, int *port_ptr);
extern int pu_id_to_pu_handle(int pu_id, unsigned long *pu_handle_ptr);
extern int pu_id_to_pu_number(unsigned long pu_id);
extern int pu_map_npi_lpa_to_handle(int npi_lpa, int *pu_id_ptr, unsigned long handle_ptr);
extern int pu_strerror(int pu_errnum);

/* *INDENT-OFF* */
#ifdef __END_DECLS
__END_DECLS
#endif				/* __END_DECLS */
/* *INDENT-ON* */

#endif				/* __NPIAPI_H__ */
