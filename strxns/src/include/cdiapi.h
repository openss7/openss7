/*****************************************************************************

 @(#) $Id: cdiapi.h,v 0.9.2.1 2007/01/05 06:14:53 brian Exp $

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

 $Log: cdiapi.h,v $
 Revision 0.9.2.1  2007/01/05 06:14:53  brian
 - added GCOM api files, doc updates

 *****************************************************************************/

#ifndef __CDIAPI_H__
#define __CDIAPI_H__

#ident "@(#) $RCSfile: cdiapi.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

#ifdef __BEGIN_DECLS
__BEGIN_DECLS
#endif
extern int cdi_allow_input_req(int fid, int *state_ptr);
extern int cdi_attach_req(int fid, long ppa, int *state_ptr);
extern void cid_decode_ctl(char *p);
extern char *cdi_decode_modem_sigs(unsigned sigs);
extern int cdi_detach_req(int fid, int *state_ptr);
extern int cdi_disable_req(int fid, unsigned long disposal, int *state_ptr);
extern int cdi_enable_req(int fid, int *state_ptr);
extern int cdi_get_a_msg(int fid, char *buf, int size);
extern int cdi_get_modem_sigs(int fid, int flag);
extern int cdi_init(int log_optns, char *log_name);
extern int cdi_init_FILE(int log_optns, FILE * filestream);
extern int cdi_modem_sig_poll(int fid);
extern int cdi_modem_sig_req(int fid, unsigned sigs);
extern int cdi_open_data(void);
extern int cdi_perror(char *msg);
extern int cdi_printf(char *fmt, ...);
extern void cdi_print_msg(unsigned char *p, unsigned n, int indent);
extern int cdi_put_allow_input_req(int fid);
extern int cdi_put_attach_req(int fid, long ppa);
extern int cdi_put_both(int fid, char *header, int hdr_length, char *data_ptr, int data_length,
			int flags);
extern int cdi_put_data(int fid, char *data_ptr, int length, long flags);
extern int cdi_put_detach_req(int fid);
extern int cdi_put_disable_req(int fid, unsigned long disposal);
extern int cdi_put_enable_req(int fid);
extern int cdi_put_frame(int fid, unsigned char address, unsigned char control, unsigned char *ptr,
			 int count);
extern int cdi_put_proto(int cid, int length, long flags);
extern int cdi_rcv_msg(int fid, char *data_ptr, int bfr_len, long flags);
extern int cdi_read_data(int cdi_data, char *buf, int cnt);
extern int cdi_set_log_size(long nbytes);
extern int cdi_wait_ack(int fid, unsigned long primitive, int *state_ptr);
extern int cdi_write_data(int cdi_data, char *buf, int cnt);
extern int cdi_xray_req(int fid, int upa, int on_off, int hi_wat, int lo_wat);

#ifdef __END_DECLS
__END_DECLS
#endif
#include <sys/cdi.h>
#endif				/* __CDIAPI_H__ */
