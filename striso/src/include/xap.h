/*****************************************************************************

 @(#) $Id: xap.h,v 0.9.2.2 2007/11/06 12:16:49 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/11/06 12:16:49 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xap.h,v $
 Revision 0.9.2.2  2007/11/06 12:16:49  brian
 - added library and header files

 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XAP_H__
#define __XAP_H__

#ident "@(#) $RCSfile: xap.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/*
 * xap.h
 */

/*
 * Data structures for X/Open ACSE/Presentation Library environment attributes
 * and cdata parameters.
 */

/*
 * Environment data structures
 */

/*
 * The following ID numbers for each protocol are used to distinguish #defines
 * of various kinds for each layer, such as primitive names, environment
 * attribute names, error codes, etc.
 */
#define AP_ASN1_ID	(11)
#define AP_ID		( 8)	/* Also used to indicate inclusion XAP header file */
#define AP_ACSE_ID	( 7)
#define AP_PRES_ID	( 6)
#define AP_SESS_ID	( 5)
#define AP_TRAN_ID	( 4)
#define AP_OS_ID	( 0)

/*
 * Object Identifier structure
 */
#define AP_MAXOBJBUF 12
#define AP_CK_OBJ_NULL(O) ((O)->length ? 0 : 1)
#define AP_SET_OBJ_NULL(O) ((O)->length = 0)

typedef struct {
	long length;
	union {
		unsigned char short_buf[AP_MAXOBJBUF];
		unsigned char *long_buf;
	} b;
} ap_objid_t;

/* AP_PCDL */
typedef struct {
	long pci;
	ap_objid_t *a_sytx;
	int size_t_sytx;
	ap_objid_t **m_t_sytx;
} ap_cdl_elt_t;

typedef struct {
	int size;
	ap_cdl_elt_t *m_ap_cdl;
} ap_cdl_t;

/* AP_DCN */
typedef struct {
	ap_objid_t *a_sytx;
	ap_objid_t *t_sytx;
} ap_dcn_t;

/* AP_PCDRL */
/* AP_PCDRL.res */
#define AP_ACCEPT (0)
#define AP_USER_REJ (1)
#define AP_PROV_REJ (2)

/* AP_PCDRL.prov_rsn */
#define AP_RSN_NSPEC (0)
#define AP_A_SYTX_NSUP (1)
#define AP_PROP_T_SYTX_NSUP (2)
#define AP_LCL_LMT_DCS_EXCEEDED (3)

typedef struct {
	long res;
	ap_objid_t *t_sytx;
	long prov_rsn;
} ap_cdrl_elt_t;

typedef struct {
	int size;
	ap_cdrl_elt_t *m_ap_cdl;
} ap_cdrl_t;

typedef struct {
	int size;
	unsigned char *udata;
} ap_aeq_t;

typedef struct {
	int size;
	unsigned char *udata;
} ap_apt_t;

typedef struct {
	int size;
	unsigned char *udata;
} ap_aei_api_id_t;

typedef struct {
	long length;
	unsigned char *data;
} ap_octet_string_t;

#define AP_UNKNOWN 0
#define AP_CLNS 1
#define AP_CONS 2
#define AP_RFC1006 3

/* AP_BIND_PADDR, AP_LCL_PADDR, AP_REM_PADDR */
typedef struct {
	ap_octet_string_t nsap;		/* NSAPAddress */
	int nsap_type;			/* AP_UNKNOWN, AP_CLNS, AP_CONS, AP_RFC1006, other = system 
					   dependent */
} ap_nsap_t;

typedef struct {
	ap_octet_string_t *p_selector;
	ap_octet_string_t *s_selector;
	ap_octet_string_t *t_selector;
	int n_nsaps;
	ap_nsap_t *nsaps;
} ap_paddr_t;

/* AP_DCS */
typedef struct {
	long pci;
	ap_objid_t *a_sytx;
	ap_objid_t *t_sytx;
} ap_dcs_elt_t;

typedef struct {
	int size;
	ap_dcs_elt_t *dcs;
} ap_dcs_t;

/* AP_CLD_CONN_ID, AP_CLG_CONN_ID */
typedef struct {
	ap_octet_string_t *user_ref;	/* SS-user Ref. */
	ap_octet_string_t *comm_ref;	/* Common Ref. */
	ap_octet_string_t *addtl_ref;	/* Additional Ref. */
} ap_conn_id_t;

typedef struct {
	ap_octet_string_t *clg_user_ref;	/* Calling SS-user Reference */
	ap_octet_string_t *cld_user_ref;	/* Called SS-user Reference */
	ap_octet_string_t *comm_ref;	/* Common Reference */
	ap_octet_string_t *addtl_ref;	/* Additional Reference */
} ap_old_conn_id_t;

/* AP_QOS */
#define AP_NO 0
#define AP_YES 1
#define AP_PRITOP 0
#define AP_PRIHIGH 1
#define AP_PRIMID 2
#define AP_PRILOW 3
#define AP_PRIDFLT 4

typedef struct {
	long targetvalue;		/* target value */
	long minacceptvalue;		/* limiting acceptable value */
} ap_rate_t;

typedef struct {
	ap_rate_t called;		/* called rate */
	ap_rate_t calling;		/* calling rate */
} ap_reqvalue_t;

typedef struct {
	ap_reqvalue_t maxthrpt;		/* maximum throughput */
	ap_reqvalue_t avgthrpt;		/* average throughput */
} ap_thrpt_t;

typedef struct {
	ap_reqvalue_t maxdel;		/* maximum transit delay */
	ap_reqvalue_t avgdel;		/* average transit delay */
} ap_transdel_t;

typedef struct {
	ap_thrpt_t throughput;		/* throughput */
	ap_transdel_t transdel;		/* transit delay */
	ap_rate_t reserrorrate;		/* residual error rate */
	ap_rate_t transffailprob;	/* transfer failure probability */
	ap_rate_t estfailprob;		/* connection establ failure probability */
	ap_rate_t relfailprob;		/* connection release failure probability */
	ap_rate_t estdelay;		/* connection establishment delay */
	ap_rate_t reldelay;		/* connection release delay */
	ap_rate_t connresil;		/* connection resilience */
	unsigned int protection;	/* protection */
	int priority;			/* priority */
	char optimizedtrans;		/* optimized dialogue transfer value: AP_YES or AP_NO */
	char extcntl;			/* extended control value: AP_YES or AP_NO */
} ap_qos_t;

/* AP_DIAGNOSTIC */
typedef struct {
	long rsn;			/* reason for the abort */
	long evt;			/* event that caused abort */
	long src;			/* source of abort */
	char *error;			/* textual message */
} ap_diag_t;

/*
 * ap_pollfd structure for fds argument to ap_poll().
 */
typedef struct {
	int fd;				/* XAP instance identifier */
	short events;			/* requested events */
	short revents;			/* returned events */
} ap_pollfd_t;

/*
 * Vectored buffer definitions
 */
typedef struct {
	unsigned char *db_base;		/* beginning of buffer */
	unsigned char *db_lim;		/* last octet+1 of buffer */
	unsigned char db_ref;		/* reference count */
} ap_osi_dbuf_t;

typedef struct ap_osi_vbuf ap_osi_vbuf_t;

struct ap_osi_vbuf {
	ap_osi_vbuf_t *b_cont;		/* next message block */
	unsigned char *b_rptr;		/* 1st octet of data */
	unsigned char *b_wptr;		/* 1st free location */
	ap_osi_dbuf_t *b_datap;		/* data block */
};

/*
 * Cdata type definition
 */
typedef struct {
	long udata_length;		/* length of user-data field */
	long rsn;			/* reason for activity or abort/release primitives */
	long evt;			/* event that caused abort */
	long sync_p_sn;			/* synchronization point serial number */
	long sync_type;			/* synchronization type */
	long resync_type;		/* resynchronization type */
	long src;			/* source of abort */
	long res;			/* result of association or release request */
	long res_src;			/* source of result */
	long diag;			/* reason for association rejection */
	unsigned long tokens;		/* tokens identifier: 0 => "tokens absent" */
	ap_a_assoc_env_t *env;		/* environment attribute values */
	ap_octet_string_t act_id;	/* activity identifier */
	ap_octet_string_t old_act_id;	/* old activity identifier */
	ap_old_conn_id_t *old_conn_id;	/* old session connection identifier */
} ap_cdata_t;

typedef struct {
	unsigned long mask;		/* bit mask */
	unsigned long mode_sel;		/* AP_MODE_SEL */
	ap_objid_t cntx_name;		/* AP_CNTX_NAME */
	ap_aei_api_id_t clg_aeid;	/* AP_CLG_AEID */
	ap_aeq_t clg_aeq;		/* AP_CLG_AEQ */
	ap_aei_api_id_t clg_apid;	/* AP_CLG_APID */
	ap_apt_t clg_apt;		/* AP_CLG_APT */
	ap_aei_api_id_t cld_aeid;	/* AP_CLD_AEID */
	ap_aeq_t cld_aeq;		/* AP_CLD_AEQ */
	ap_aei_api_id_t cld_apid;	/* AP_CLD_APID */
	ap_apt_t cld_apt;		/* AP_CLD_APT */
	ap_paddr_t rem_paddr;		/* AP_REM_PADDR */
	ap_cdl_t pcdl;			/* AP_PCDL */
	ap_dcn_t dpcn;			/* AP_DPCN */
	ap_qos_t qos;			/* AP_QOS */
	unsigned long a_version_sel;	/* AP_ACSE_SEL */
	unsigned long p_version_sel;	/* AP_PRES_SEL */
	unsigned long s_version_sel;	/* AP_SESS_SEL */
	unsigned long afu_sel;		/* AP_AFU_SEL */
	unsigned long pfu_sel;		/* AP_PFU_SEL */
	unsigned long sfu_sel;		/* AP_SFU_SEL */
	ap_conn_id_t *clg_conn_id;	/* AP_CLG_CONN_ID */
	ap_conn_id_t *cld_conn_id;	/* AP_CLD_CONN_ID */
	unsigned long init_sync_pt;	/* AP_INIT_SYNC_PT */
	unsigned long init_tokens;	/* AP_INIT_TOKENS */
	ap_aei_api_id_t rsp_aeid;	/* AP_RSP_AEID */
	ap_aeq_t rsp_aeq;		/* AP_RSP_AEQ */
	ap_aei_api_id_t rsp_apid;	/* AP_RSP_APID */
	ap_apt_t rsp_apt;		/* AP_RSP_APT */
	ap_cdrl_t pcdrl;		/* AP_PCDRL */
	long dpcr;			/* AP_DPCR */
} ap_a_assoc_env_t;

/*
 * ap_val_t union for val argument to ap_set_env()
 */
typedef union {
	long l;
	void *v;
} ap_val_t;

typedef int (*ap_ualloc_t) (int, ap_osi_vbuf_t **, void **, int, int, unsigned long *);
typedef int (*ap_udealloc_t) (int, ap_osi_vbuf_t *, void *, int, unsigned long *);

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif				/* __BEGIN_DECLS */

int ap_bind(int fd, unsigned long *aperrno_p);

int ap_close(int fd, unsigned long *apperno_p);

const char *ap_error(unsigned long aperrno);

int ap_free(int fd, unsigned long kind, void *val, unsigned long *apperno_p);

int ap_get_env(int fd, unsigned long attr, void *val, unsigned long *aperrno_p);

int app_init_env(int fd, const char *env_file, int flags, unsigned long *apperno_p);

int ap_ioctl(int fd, int request, ap_val_t argument, unsigned long *apperno_p);

int ap_look(int fd, unsigned long *sptype, ap_cdata_t * cdata, ap_osi_vbuf_t ** ubuf, int *flags, unsigned long *apperno_p);

int ap_open(const char *provider, int oflags, ap_ualloc_t ap_user_alloc, ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p);

int ap_poll(ap_pollfd_t fds[], int nfds, int timeout, unsigned long *apperno_p);

int ap_rcv(int fd, unsigned long *sptype, ap_cdata_t * cdata, ap_osi_vbuf_t ** ubuf, int *flags, unsigned long *aperrno_p);

int ap_restore(int fd, FILE * savef, int oflags, ap_ualloc_t ap_user_alloc, ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p);

int ap_save(int fd, FILE *savef, unsigned long *apperno_p);

int ap_set_env(int fd, unsigned long attr, ap_val_t val, unsigned long *apperno_p);

int ap_snd(int fd, unsigned long sptype, ap_cdata_t *cdata, ap_osi_vbuf_t *ubuf, int flags, unsigned long *aperrno_p);

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif				/* __END_DECLS */

#endif				/* __XAP_H__ */
