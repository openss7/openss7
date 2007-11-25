/*****************************************************************************

 @(#) $Id: xap.h,v 0.9.2.3 2007/11/10 19:40:52 brian Exp $

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

 Last Modified $Date: 2007/11/10 19:40:52 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xap.h,v $
 Revision 0.9.2.3  2007/11/10 19:40:52  brian
 - documentation updates

 Revision 0.9.2.2  2007/11/06 12:16:49  brian
 - added library and header files

 Revision 0.9.2.1  2007/09/29 14:08:34  brian
 - added new files

 *****************************************************************************/

#ifndef __XAP_H__
#define __XAP_H__

#ident "@(#) $RCSfile: xap.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

#include <sys/poll.h>

/*
 * xap.h
 */

/** @weakgroup xap OpenSS7 XAP Library
  * @{ */

/** @file
  * XAP (User) Header File. */

/*
 * Data structures for X/Open ACSE/Presentation Library environment attributes and cdata parameters.
 */

/*
 * Environment data structures
 */

/*
 * The following ID numbers for each protocol are used to distinguish #defines of various kinds for
 * each layer, such as primitive names, environment attribute names, error codes, etc.
 */
#define AP_ASN1_ID	(11)
#define AP_ID		( 8)	/* Also used to indicate inclusion XAP header file */
#define AP_ACSE_ID	( 7)
#define AP_PRES_ID	( 6)
#define AP_SESS_ID	( 5)
#define AP_TRAN_ID	( 4)
#define AP_OS_ID	( 0)


/*
 * ap_val_t union for val argument to ap_set_env()
 */
typedef union {
	long l;
	void *v;
} ap_val_t;

/*
 * Object Identifier structure
 */
#define AP_MAXOBJBUF 12
#define AP_CK_OBJ_NULL(O)	((O)->length ? 0 : 1)
#define AP_SET_OBJ_NULL(O)	((O)->length = 0)

/** @type ap_objid_t
  * @brief Used to convey OBJECT IDENTIFIER values.
  *
  * The ap_objid_t structure is used to convey OBJECT IDENTIFIER values.  OBJECT IDENTIFIER values
  * are stored as the contents octets of their encoded form (without the tag or length octets), in a
  * similar fashion to XOM.  If the number of contents octets is greater than MXOBJBUF, the contents
  * octets are stored beginning at the memory location pointed to by long_buf.  Otherwise, the
  * contents octets are stored in the short_buf array.  In both cases, length gives the number of
  * contents octets in the OBJECT IDENTIFIER encoding.  The absense of an OBJECT IDENTIFIER
  * parameter is indicated by setting the length field to 0.
  *
  * Routines to create and operat on objects of this type may be provided as part of an
  * Encode/Decode Library, but such routines are not part of the XAP specification.
  */
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

/** @brief Use to convey context defninition lists.
  *
  * The context definition list comprises a series of elements in the array, m_ap_cdl.
  * The number of elements in this array is given as size.
  *
  * The presentation context identifier is represented by m_ap_cdl[i].pci.
  * m_ap_cdl[i].a_sytx is a pointer to an ap_objid_t and is used to convey the abstract syntax name
  * associated with the presentation context identifer.  m_ap_cdl[i].m_t_sytx is a pointer to
  * an array of type ap_objid_t * which includes the transfer syntaxes that the
  * association-initiator is capable of supporting for the named abstract syntax.
  * m_ap_cdl[i].size_t_sytx is the number of elements in the m_ap_cdl[i].m_t_sytx array.
  *
  * The context definition list is an optional parameter of the CP PPDU.  Setting size to minus one
  * (-1) specifies that this parameter is not present.  The presentation context definition list may
  * also be specified to be absent by invoking ap_set_env() with AP_PCDL as the attr argument to
  * that function and a NULL pointer as the val argument to that function.
  *
  * This structure is used with the AP_PCDL environment attribute.
  */
typedef struct {
	int size;
	ap_cdl_elt_t *m_ap_cdl;
} ap_cdl_t;

/* AP_PCDRL */

typedef struct {
	long res;
	ap_objid_t *t_sytx;
	long prov_rsn;
} ap_cdrl_elt_t;

/** @brief Used to convey context definnition result lists.
  *
  * The context definition result list comprises a series of elements in the array, mp_ap_cdrl.
  * The number of elements in this array is given as size.
  *
  * There is a one-to-one correspondence between the elements of a context definition list and those
  * in the related context definition result list.  For each entry in the presentation context
  * definition list, the m_ap_cdrl[i].res field of the corresponding element in the presentation
  * context definition result list must be set by the association-responder to one of AP_ACCEPT,
  * AP_USER_REJ or AP_PROV_REJ.  If m_ap_cdlr[i].res is set to AP_ACCESSP, then m_ap_cdrl[i].t_sytx
  * indicates the transfer syntax selected by the association-responder.  If m_ap_cdrl[i].res is
  * set to either AP_USER_REJ or AP_PROV_REJ, m_ap_cdrl[i].prov_rsn is set to the reason for the
  * rejection of the abstract syntax.
  *
  * The possible values for ap_cdrl[i].prov_rsn are:
  *
  * AP_RSN_NSPEC (reason not specified),
  * AP_A_SYTX_NSUP (abstract syntax not supported),
  * AP_PROP_T_SYTX_NSUP (proposed transfer syntaxes not supported), and
  * AP_LCL_LMT_DCS_EXCEEDED (local limit on DCS exceeded).
  *
  * The context definition result list is an optional parameter of the CPA and CPR PPDUs.  Setting
  * the value of size to -1 indicates that this parameter is not present.  The presentation context
  * definition result list may also be specified as absent by invoking ap_set_env() with AP_PCDRL as
  * the attr argument and a NULL pointer as the val argument to the function.
  *
  * This structure is used with the AP_PCDRL environment attribute.
  */
typedef struct {
	int size;
	ap_cdrl_elt_t *m_ap_cdl;
} ap_cdrl_t;

/* AP_PCDRL.res */
#define AP_ACCEPT		(0)	/**< Proposed presentation context accepted. */
#define AP_USER_REJ		(1)	/**< Proposed presentation context rejected by user. */
#define AP_PROV_REJ		(2)	/**< Proposed presentation context rejected by provider. */

/* AP_PCDRL.prov_rsn */
#define AP_RSN_NSPEC		(0)	/**< Reason not specified (unknown). */
#define AP_A_SYTX_NSUP		(1)	/**< Abstract syntax not supported. */
#define AP_PROP_T_SYTX_NSUP	(2)	/**< Proposed transfer syntaxes not supported. */
#define AP_LCL_LMT_DCS_EXCEEDED	(3)	/**< Local limit on DCS exceeded. */

/** @type ap_octet_string_t
  * @brief Octet-String
  *
  * The length member of this structure indicates the number of octets in the octet string pointed
  * to by the data member.  To specify a null octet string value, the length field of the
  * ap_octet_string_t structure is set to zero (0).
  */
typedef struct {
	long length;			/**< Number of octets. */
	unsigned char *data;		/**< Octets. */
} ap_octet_string_t;

/** @brief Used to convey objects specified as ASN.1 type AE-Qualifier.
  *
  * udata is a pointer to a buffer of user encoded AE-Qualifier contents octets; size is the length
  * of that buffer in octets.
  *
  * For optional PDU parameters of type AE-Qualifier, setting size to minus one (-1) indicatest that
  * the parameter is not present.  Also, an optional parameter that corresponds to an environment
  * attribute may be specified to be absent by invoking ap_set_env() with a NULL pointer as the val
  * argument to that function.
  */
typedef struct {
	int size;		/**< Buffer size in bytes. */
	unsigned char *udata;	/**< Buffer with user encoded AE-Qualifier. */
} ap_aeq_t;

/** @brief Used to convey objects specified as ASN.1 type AP-title.
  *
  * udata is a pointer to a buffer of user encoded AP-Title contents octets; size is the length of
  * that buffer in octets.
  *
  * For optional PDU parameters of type AP-title, setting size to minus one (-1) indicatest that the
  * parameter is not present.  Also, an optional parameter that corresponds to an environment
  * attribute may be specified to be absent by invoking ap_set_env() with a NULL pointer as the val
  * argument to that function.
  */
typedef struct {
	int size;		/**< Buffer size in bytes. */
	unsigned char *udata;	/**< Buffer with user encoded AP-title. */
} ap_apt_t;

/** @brief Use to convey AE/AP invocation identifier.
  *
  * The structue is used to convey application entity or process identifier values.  Application
  * entity or process identifier values are stored in their encoded form including "tag" and
  * "length".  The absense of an application entity or process identifier parameter is indicated by
  * setting the size field to minus one (-1).
  */
typedef struct {
	int size;		/**< Buffer size in bytes. */
	unsigned char *udata;	/**< Buffer with user encoded AE-identifier or AP-identifier. */
} ap_aei_api_id_t;

/* AP_BIND_PADDR, AP_LCL_PADDR, AP_REM_PADDR */
typedef struct {
	ap_octet_string_t nsap;		/* NSAPAddress */
	int nsap_type;			/* AP_UNKNOWN, AP_CLNS, AP_CONS, AP_RFC1006, other = system 
					   dependent */
} ap_nsap_t;

#define AP_UNKNOWN	0	/* Unknown NSAP type. */
#define AP_CLNS		1	/* NSAP belongs to a CLNP network. */
#define AP_CONS		2	/* NSAP belongs to an X.25 (CONS) network. */
#define AP_RFC1006	3	/* NSAP belongs to an RFC 1006 network. */

/** @type ap_paddr_t
  * @brief Conveys the presentation address.
  *
  * The p_selector, s_selector and t_selector are pointers to octet strings corresponding to the
  * presentation, session and transport selectors respectively.  These octet strings are represented
  * by the ap_octet_string_t structure.  The length field of this structure indicates how many
  * octets are in the octet string pointed to by data.
  *
  * To specify a null selector value, the length field of the ap_octet_string_t structure is set to
  * zero (0).
  *
  * For selector wildcard presentation addresses, non-specified selector values are
  * indicated by setting the corresponding pointer(s) in the ap_paddr_t structure to NULL.
  *
  * The n_nsaps element is used to specify how many network address components are in the array,
  * nsaps.
  *
  * Each element of the nsaps array is an ap_octet_string_t structure with an associated nsap_type
  * which identifies the type of network (or system dependent values) to which the NSAP refers.  An
  * nsap_type of AP_UNKNOWN indicates that the network type is not known.  Note that the concept of
  * a nsap_type is new for XAP and was not present in APLI.
  *
  * When multiple network address components are included in a presentation address, the specific
  * network address(es) chosen by the provider and the manner by which it is selected for intiating
  * or listening to connections is not specified by XAP and is a local implementation issue.  When
  * used to represent a wildcard address, the value of n_nsaps may be zero.  In this case, all
  * locally defined network addresses are implied.  In all other cases, n_nsaps must be a positive
  * value.
  *
  * Presentation addresses are restricted as follows:
  *
  * � Session selectors cannot exceed 16 octets.
  * � Transport selectors cannot exceed 32 octets.
  * � Remote addresses may not have more than 8 Network addresses.
  * � Individual Network addresses cannot exceed 20 octets.
  *
  * Note that implementations may impose further restrictions on local addresses.
  */
typedef struct {
	ap_octet_string_t *p_selector;	/* presentation selector */
	ap_octet_string_t *s_selector;	/* session selector */
	ap_octet_string_t *t_selector;	/* transport selector */
	int n_nsaps;			/* number of network addresses */
	ap_nsap_t *nsaps;		/* network addresses */
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

/** @type ap_conn_id_t
  * @brief used to convey session connection identifiers.
  *
  * Each member of the ap_conn_id_t structure is a pointer to a structure of type ap_octet_string_t.
  * The user_ref member must be less thatn or equal to 64 octets.  The comm_ref member must be less
  * than or equal to 64 octets.  The addtl_ref member must be less than or equal to 4 octets.  The
  * absense of a particular member of a connection identifier may be indicated either by setting the
  * corresponding field to NULL, or by specifying a zero (0) length ap_octet_string_t.
  *
  * This structure is used with the AP_CLD_CONN_ID and AP_CLG_CONN_ID environment attributes.
  */
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
#define AP_NO		0
#define AP_YES		1

/* ap_qos_t.priority */
#define AP_PRITOP	0
#define AP_PRIHIGH	1
#define AP_PRIMID	2
#define AP_PRILOW	3
#define AP_PRIDFLT	4

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

/** @type ap_qos_t
  * @brief Specifies the range of acceptable values for the quality of service requirements of an association.
  *
  * The fields of the ap_qos_t structure specify the ranges of acceptable values for the quality of
  * service requirements of an association.
  */
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

/* ap_diag_t.src values: */
#define AP_ACSE_SERV_PROV	(FIXME)
#define AP_PRES_SERV_PROV	(FIXME)
#define AP_SESS_SERV_PROV	(FIXME)
#define AP_TRAN_SERV_PROV	(FIXME)

/* ap_diag_t.rsn values: */
/* ap_diag_t.src == AP_ACSE_SERV_PROV */
#define AP_NSPEC		(FIXME)
#define AP_UNREC_APDU		(FIXME)
#define AP_UNEXPT_APDU		(FIXME)
#define AP_UNREC_APDU_PARM	(FIXME)
#define AP_UNEXPT_APDU_PARM	(FIXME)
#define AP_INVAL_APDU_PARM	(FIXME)
/* ap_diag_t.src == AP_TRAN_SERV_PROV */
#define AP_TRAN_DISCONNECT	(FIXME)

/* ap_diag_t.evt values: */
/* ap_diag_t.src == AP_ACSE_SERV_PROV */
#define AP_AEI_AARQ		(FIXME)	/**< Associate request APDU. */
#define AP_AEI_AARE		(FIXME)	/**< Associate response APDU. */
#define AP_AEI_RLRQ		(FIXME)	/**< Associate release request APDU. */
#define AP_AEI_RLRE		(FIXME)	/**< Associate release response APDU. */
#define AP_AEI_ABRT		(FIXME)	/**< Associate abort APDU. */

/* If the src field is set to AP_TRAN_SERV_PROV and the rsn field is set to AP_TRAN_DISCONNECT, the
 * evt field will be set to an implementation-defined diagnostic value such as that returned by the
 * XTI t_rcvdis(3) call. */

/*
 * ap_pollfd structure for fds argument to ap_poll().
 */
struct ap_pollfd {
	int fd;				/* XAP instance identifier */
	short events;			/* requested events */
	short revents;			/* returned events */
};

typedef struct pollfd ap_pollfd_t;

#define AP_POLLIN	POLLIN
#define AP_POLLPRI	POLLPRI
#define AP_POLLOUT	POLLOUT
#define AP_POLLRDNORM	POLLRDNORM
#define AP_POLLRDBAND	POLLRDBAND
#define AP_POLLWRNORM	POLLWRNORM
#define AP_POLLWRBAND	POLLWRBAND

#define AP_POLLERR	POLLERR
#define AP_POLLHUP	POLLHUP
#define AP_POLLNVAL	POLLNVAL
#define AP_POLLMSG	POLLMSG
#define AP_POLLURG	POLLNVAL

#define AP_INFTIM	(-1)

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
 * These are the bit definitions for the ap_a_assoc_env_t.mask field.  I have no idea if these line
 * up with other implementations.  Only source level compatibility is attempted.  Binaries are
 * guaranteed to break.
 */
#define AP_MODE_SEL_BIT		(1<< 0)
#define AP_CNTX_NAME_BIT	(1<< 1)
#define AP_CLG_AEID_BIT		(1<< 2)
#define AP_CLG_AEQ_BIT		(1<< 3)
#define AP_CLG_APID_BIT		(1<< 4)
#define AP_CLG_APT_BIT		(1<< 5)
#define AP_CLD_AEID_BIT		(1<< 6)
#define AP_CLD_AEQ_BIT		(1<< 7)
#define AP_CLD_APID_BIT		(1<< 8)
#define AP_CLD_APT_BIT		(1<< 9)
#define AP_REM_PADDR_BIT	(1<<10)
#define AP_PCDL_BIT		(1<<11)
#define AP_DPCN_BIT		(1<<12)
#define AP_QOS_BIT		(1<<13)
#define AP_A_VERSION_SEL_BIT	(1<<14)
#define AP_ACSE_SEL_BIT		AP_A_VERSION_SEL_BIT	/* Error in XAP spec. */
#define AP_P_VERSION_SEL_BIT	(1<<15)
#define AP_PRES_SEL_BIT		AP_P_VERSION_SEL_BIT	/* Error in XAP spec. */
#define AP_S_VERSION_SEL_BIT	(1<<16)
#define AP_SESS_SEL_BIT		AP_S_VERSION_SEL_BIT	/* Error in XAP spec. */
#define AP_AFU_SEL_BIT		(1<<17)
#define AP_PFU_SEL_BIT		(1<<18)
#define AP_SFU_SEL_BIT		(1<<19)
#define AP_CLG_CONN_ID_BIT	(1<<20)
#define AP_CLD_CONN_ID_BIT	(1<<21)
#define AP_INIT_SYNC_PT_BIT	(1<<22)
#define AP_INIT_TOKENS_BIT	(1<<23)
#define AP_RSP_AEID_BIT		(1<<24)
#define AP_RSP_AEQ_BIT		(1<<25)
#define AP_RSP_APID_BIT		(1<<26)
#define AP_RSP_APT_BIT		(1<<27)
#define AP_PCDRL_BIT		(1<<28)
#define AP_DPCR_BIT		(1<<29)
/* only two bits left! */

/** @name XAP Environment Attributes
  * The following XAP environment attributes can be written with ap_set_env() or read with
  * ap_get_env().
  *
  * I have no idea if these line up with other implementations as they are not described in the
  * standards documents.  Only source compatibility is attempted.  Binaries are guaranteed to break
  * as the values of these symbolic constants are not standardized.  Anyone wanting to contribute a
  * compatible list for a given architecture is welcomed to do so.
  * @{ */
#define AP_ACSE_AVAIL		((AP_ACSE_ID<<16) |  1)
#define AP_ACSE_SEL		((AP_ACSE_ID<<16) |  2)
#define AP_BIND_PADDR		((AP_PRES_ID<<16) |  3)
#define AP_CLD_AEID		((AP_ACSE_ID<<16) |  4)
#define AP_CLD_AEQ		((AP_ACSE_ID<<16) |  5)
#define AP_CLD_APID		((AP_ACSE_ID<<16) |  6)
#define AP_CLD_APT		((AP_ACSE_ID<<16) |  7)
#define AP_CLD_CONN_ID		((AP_SESS_ID<<16) |  8)
#define AP_CLG_AEID		((AP_ACSE_ID<<16) |  9)
#define AP_CLG_AEQ		((AP_ACSE_ID<<16) | 10)
#define AP_CLG_APID		((AP_ACSE_ID<<16) | 11)
#define AP_CLG_APT		((AP_ACSE_ID<<16) | 12)
#define AP_CLG_CONN_ID		((AP_SESS_ID<<16) | 13)
#define AP_CNTX_NAME		((AP_ACSE_ID<<16) | 14)
#define AP_DCS			((AP_PRES_ID<<16) | 15)
#define AP_DPCN			((AP_PRES_ID<<16) | 16)
#define AP_DPCR			((AP_PRES_ID<<16) | 17)
#define AP_INIT_SYNC_PT		((AP_SESS_ID<<16) | 18)
#define AP_LCL_PADDR		((AP_PRES_ID<<16) | 19)
#define AP_LIB_AVAIL		((AP_ID     <<16) | 20)
#define AP_LIB_SEL		((AP_ID     <<16) | 21)
#define AP_MODE_AVAIL		((AP_ACSE_ID<<16) | 22)
#define AP_MODE_SEL		((AP_ACSE_ID<<16) | 23)
#define AP_MSTATE		((AP_ID	    <<16) | 24)
#define AP_PCDL			((AP_PRES_ID<<16) | 25)
#define AP_PCDRL		((AP_PRES_ID<<16) | 26)
#define AP_PFU_AVAIL		((AP_PRES_ID<<16) | 27)
#define AP_PFU_SEL		((AP_PRES_ID<<16) | 28)
#define AP_PRES_AVAIL		((AP_PRES_ID<<16) | 29)
#define AP_PRES_SEL		((AP_PRES_ID<<16) | 30)
#define AP_REM_PADDR		((AP_PRES_ID<<16) | 31)
#define AP_ROLE_ALLOWED		((AP_ACSE_ID<<16) | 32)
#define AP_ROLE_CURRENT		((AP_ACSE_ID<<16) | 33)
#define AP_RSP_AEID		((AP_ACSE_ID<<16) | 34)
#define AP_RSP_AEQ		((AP_ACSE_ID<<16) | 35)
#define AP_RSP_APID		((AP_ACSE_ID<<16) | 36)
#define AP_RSP_APT		((AP_ACSE_ID<<16) | 37)
#define AP_SESS_AVAIL		((AP_SESS_ID<<16) | 38)
#define AP_SESS_SEL		((AP_SESS_ID<<16) | 39)
#define AP_SFU_AVAIL		((AP_SESS_ID<<16) | 40)
#define AP_SFU_SEL		((AP_SESS_ID<<16) | 41)
#define AP_STATE		((AP_ACSE_ID<<16) | 42)
#define AP_FLAGS		((AP_ID     <<16) | 43)	/* Called AP_STREAM_FLAGS in APLI. */
#define AP_TOKENS_AVAIL		((AP_SESS_ID<<16) | 44)
#define AP_TOKENS_OWNED		((AP_SESS_ID<<16) | 45)
#define AP_AFU_AVAIL		((AP_ACSE_ID<<16) | 46)	/* Not APLI. */
#define AP_AFU_SEL		((AP_ACSE_ID<<16) | 47)	/* Not APLI. */
#define AP_COPYENV		((AP_ID     <<16) | 48)	/* Not APLI. */
#define AP_DIAGNOSTIC		((AP_ACSE_ID<<16) | 49)	/* Not APLI. */
#define AP_INIT_TOKENS		((AP_SESS_ID<<16) | 50)	/* Not APLI. */
#define AP_OLD_CONN_ID		((AP_SESS_ID<<16) | 51)	/* Not APLI. */
#define AP_OPT_AVAIL		((AP_SESS_ID<<16) | 52)	/* Not APLI. */
#define AP_QLEN			((AP_TRAN_ID<<16) | 53)	/* Not APLI. */
#define AP_QOS			((AP_TRAN_ID<<16) | 54)	/* Not APLI. */
#define AP_SESS_OPT_AVAIL	((AP_SESS_ID<<16) | 55)	/* Not APLI. */
/** @} */

/* bit values for AP_ACSE_AVAIL and AP_ACSE_SEL */
#define AP_ACSEVER1		(1<<0)

/* values for AP_DPCR */
#define AP_DPCR_NOVAL		(-1)
//#define AP_ACCEPT		0
//#define AP_USER_REJ		1
//#define AP_PROV_REJ		2

/* values for AP_INIT_SYNC_PT */
#define AP_MIN_SYNCP		0
#define AP_MAX_SYNCP		999999

/* bit values for AP_LIB_AVAIL and AP_LIB_SEL */
/* Strangely, this value cannot be used to determine the difference betweent the APLI version of the
 * library and the XAP version of the library. */
#define AP_LIBVER1		(1<<0)

/* bit values for AP_MODE_AVAIL and AP_MODE_SEL */
#define AP_NORMAL_MODE		(1<<0)
#define AP_X410_MODE		(1<<1)
#define AP_ROSE_MODE		(1<<2)

/* bit values for AP_MSTATE */
#define AP_SNDMORE		(1<<0)
#define AP_RCVMORE		(1<<1)

/* bit values for AP_PRES_SEL */
#define AP_PRESVER1		(1<<0)

/* bit values for AP_ROLE_ALLOWED and AP_ROLE_CURRENT */
#define AP_INITIATOR		(1<<0)
#define AP_RESPONDER		(1<<1)

/* bit values for AP_SESS_AVAIL and AP_SESS_SEL */
#define AP_SESSVER1		(1<<0)
#define AP_SESSVER2		(1<<1)

/* bit values for AP_SESS_OPT_AVAIL */
#define AP_UCBC			(1<<0)
#define AP_UCEC			(1<<1)

/* bit values for AP_SFU_AVAIL and AP_SFU_SEL */
#define AP_SESS_DUPLEX		(1<< 0)
#define AP_SESS_HALFDUPLEX	(1<< 1)
#define AP_SESS_XDATA		(1<< 2)
#define AP_SESS_TDATA		(1<< 3)
#define AP_SESS_MINORSYNC	(1<< 4)
#define AP_SESS_MAJORSYNC	(1<< 5)
#define AP_SESS_RESYNC		(1<< 6)
#define AP_SESS_EXCEPT		(1<< 7)
#define AP_SESS_ACTMGMT		(1<< 8)
#define AP_SESS_NEGREL		(1<< 9)	/* Not APLI. */
#define AP_SESS_CDATA		(1<<10)	/* Not APLI. */
#define AP_SESS_DATASEP		(1<<11)	/* Not APLI. */

/* values for AP_STATE */
#define AP_UNBOUND		(1<< 0)
#define AP_IDLE			(1<< 1)
#define AP_DATA_XFER		(1<< 2)
#define AP_WASSOCrsp_ASSOCind	(1<< 3)
#define AP_WASSOCcnf_ASSOCreq	(1<< 4)
#define AP_WRELrsp_RELind	(1<< 5) /* Not APLI. */
#define AP_WRELcnf_RELreq	(1<< 6) /* Not APLI. */
#define AP_WRESYNrsp_RESYNind	(1<< 7) /* Not APLI. */
#define AP_WRESYNcnf_RESYNreq	(1<< 8) /* Not APLI. */
#define AP_WRELrsp_RELind_init	(1<< 9) /* Not APLI. */
#define AP_WRELcnf_RELreq_rsp	(1<<10) /* Not APLI. */
#define AP_WACTDrsp_ACTDind	(1<<11) /* Not APLI. */
#define AP_WACTDcnf_ACTDreq	(1<<12) /* Not APLI. */
#define AP_WACTErsp_ACTEind	(1<<13) /* Not APLI. */
#define AP_WACTEcnf_ACTEreq	(1<<14) /* Not APLI. */
#define AP_WACTIrsp_ACTIind	(1<<15) /* Not APLI. */
#define AP_WACTIcnf_ACTIreq	(1<<16) /* Not APLI. */
#define AP_WSYNCMArsp_SYNCMAind	(1<<17) /* Not APLI. */
#define AP_WSYNCMAcnf_SYNCMAreq	(1<<18) /* Not APLI. */
#define AP_WCDATArsp_CDATAind	(1<<19) /* Not APLI. */
#define AP_WCDATAcnf_CDATAreq	(1<<20) /* Not APLI. */
#define AP_WRECOVERYind		(1<<21) /* Not APLI. */
#define AP_WRECOVERYreq		(1<<22) /* Not APLI. */

/* values for AP_FLAGS */
#define AP_NDELAY		O_NDELAY

/* bit values for AP_TOKENS_AVAIL and AP_TOKENS_OWNED */
#define AP_DATA_TOK		(1<<0)
#define AP_SYNCMINOR_TOK	(1<<1)
#define AP_MAJACT_TOK		(1<<2)
#define AP_RELEASE_TOK		(1<<3)

/* values for AP_COPYENV */
#define TRUE			1
#define FALSE			0

/* bit values for AP_INIT_TOKENS */
#define AP_DATA_TOK_REQ		(1<< 0)
#define AP_DATA_TOK_ACPT	(1<< 1)
#define AP_DATA_TOK_CHOICE	(1<< 2)
#define AP_SYNCMINOR_TOK_REQ	(1<< 3)
#define AP_SYNCMINOR_TOK_ACPT	(1<< 4)
#define AP_SYNCMINOR_TOK_CHOICE	(1<< 5)
#define AP_MAJACT_TOK_REQ	(1<< 6)
#define AP_MAJACT_TOK_ACPT	(1<< 7)
#define AP_MAJACT_TOK_CHOICE	(1<< 8)
#define AP_RELEASE_TOK_REQ	(1<< 9)
#define AP_RELEASE_TOK_ACPT	(1<<10)
#define AP_RELEASE_TOK_CHOICE	(1<<11)

/* bit values for AP_OPT_AVAIL */
#define AP_NSAP_WILD		(1<<0)
#define AP_TSEL_WILD		(1<<1)
#define AP_SSEL_WILD		(1<<2)
#define AP_PSEL_WILD		(1<<3)


/*
 *  sptypes for primitives.  I have no idea if these line up with other implemetnations as they are
 *  not described in the standards documents.  Only source compatibility is attempted.  Binaries are
 *  guaranteed to break as the value of these sybolic constants are not standardized.
 */
#define A_ASSOC_REQ		((AP_ACSE_ID<<16) | 0x01)
#define A_ASSOC_IND		((AP_ACSE_ID<<16) | 0x02)
#define A_ASSOC_RSP		((AP_ACSE_ID<<16) | 0x03)
#define A_ASSOC_CNF		((AP_ACSE_ID<<16) | 0x04)
#define A_RELEASE_REQ		((AP_ACSE_ID<<16) | 0x05)
#define A_RELEASE_IND		((AP_ACSE_ID<<16) | 0x06)
#define A_RELEASE_RSP		((AP_ACSE_ID<<16) | 0x07)
#define A_RELEASE_CNF		((AP_ACSE_ID<<16) | 0x08)
#define A_ABORT_REQ		((AP_ACSE_ID<<16) | 0x09)
#define A_ABORT_IND		((AP_ACSE_ID<<16) | 0x0A)
#define A_PABORT_REQ		((AP_ACSE_ID<<16) | 0x0B)
#define A_PABORT_IND		((AP_ACSE_ID<<16) | 0x0C)

#define P_DATA_REQ		((AP_PRES_ID<<16) | 0x01)
#define P_DATA_IND		((AP_PRES_ID<<16) | 0x02)
#define P_TOKENGIVE_REQ		((AP_PRES_ID<<16) | 0x03)
#define P_TOKENGIVE_IND		((AP_PRES_ID<<16) | 0x04)
#define P_TOKENPLEASE_REQ	((AP_PRES_ID<<16) | 0x05)
#define P_TOKENPLEASE_IND	((AP_PRES_ID<<16) | 0x06)

#define P_CDATA_REQ		((AP_PRES_ID<<16) | 0x07)	/* Not APLI. */
#define P_CDATA_IND		((AP_PRES_ID<<16) | 0x08)	/* Not APLI. */
#define P_CDATA_RSP		((AP_PRES_ID<<16) | 0x09)	/* Not APLI. */
#define P_CDATA_CNF		((AP_PRES_ID<<16) | 0x0A)	/* Not APLI. */
#define P_TDATA_REQ		((AP_PRES_ID<<16) | 0x0B)	/* Not APLI. */
#define P_TDATA_IND		((AP_PRES_ID<<16) | 0x0C)	/* Not APLI. */
#define P_XDATA_REQ		((AP_PRES_ID<<16) | 0x0D)	/* Not APLI. */
#define P_XDATA_IND		((AP_PRES_ID<<16) | 0x0E)	/* Not APLI. */
#define P_CTRLGIVE_REQ		((AP_PRES_ID<<16) | 0x0F)	/* Not APLI. */
#define P_CTRLGIVE_IND		((AP_PRES_ID<<16) | 0x10)	/* Not APLI. */
#define P_SYNCMINOR_REQ		((AP_PRES_ID<<16) | 0x11)	/* Not APLI. */
#define P_SYNCMINOR_IND		((AP_PRES_ID<<16) | 0x12)	/* Not APLI. */
#define P_SYNCMINOR_RSP		((AP_PRES_ID<<16) | 0x13)	/* Not APLI. */
#define P_SYNCMINOR_CNF		((AP_PRES_ID<<16) | 0x14)	/* Not APLI. */
#define P_SYNCMAJOR_REQ		((AP_PRES_ID<<16) | 0x15)	/* Not APLI. */
#define P_SYNCMAJOR_IND		((AP_PRES_ID<<16) | 0x16)	/* Not APLI. */
#define P_SYNCMAJOR_RSP		((AP_PRES_ID<<16) | 0x17)	/* Not APLI. */
#define P_SYNCMAJOR_CNF		((AP_PRES_ID<<16) | 0x18)	/* Not APLI. */
#define P_RESYNC_REQ		((AP_PRES_ID<<16) | 0x19)	/* Not APLI. */
#define P_RESYNC_IND		((AP_PRES_ID<<16) | 0x1A)	/* Not APLI. */
#define P_RESYNC_RSP		((AP_PRES_ID<<16) | 0x1B)	/* Not APLI. */
#define P_RESYNC_CNF		((AP_PRES_ID<<16) | 0x1C)	/* Not APLI. */
#define P_UXREPORT_REQ		((AP_PRES_ID<<16) | 0x1D)	/* Not APLI. */
#define P_UXREPORT_IND		((AP_PRES_ID<<16) | 0x1E)	/* Not APLI. */
#define P_PXREPORT_IND		((AP_PRES_ID<<16) | 0x1F)	/* Not APLI. */
#define P_ACTSTART_REQ		((AP_PRES_ID<<16) | 0x20)	/* Not APLI. */
#define P_ACTSTART_IND		((AP_PRES_ID<<16) | 0x21)	/* Not APLI. */
#define P_ACTEND_REQ		((AP_PRES_ID<<16) | 0x22)	/* Not APLI. */
#define P_ACTEND_IND		((AP_PRES_ID<<16) | 0x23)	/* Not APLI. */
#define P_ACTEND_RSP		((AP_PRES_ID<<16) | 0x24)	/* Not APLI. */
#define P_ACTEND_CNF		((AP_PRES_ID<<16) | 0x25)	/* Not APLI. */
#define P_ACTINTR_REQ		((AP_PRES_ID<<16) | 0x26)	/* Not APLI. */
#define P_ACTINTR_IND		((AP_PRES_ID<<16) | 0x27)	/* Not APLI. */
#define P_ACTINTR_RSP		((AP_PRES_ID<<16) | 0x28)	/* Not APLI. */
#define P_ACTINTR_CNF		((AP_PRES_ID<<16) | 0x29)	/* Not APLI. */
#define P_ACTDISCARD_REQ	((AP_PRES_ID<<16) | 0x2A)	/* Not APLI. */
#define P_ACTDISCARD_IND	((AP_PRES_ID<<16) | 0x2B)	/* Not APLI. */
#define P_ACTDISCARD_RSP	((AP_PRES_ID<<16) | 0x2C)	/* Not APLI. */
#define P_ACTDISCARD_CNF	((AP_PRES_ID<<16) | 0x2D)	/* Not APLI. */

/** @name ACSE/Presentation Library Error Codes
  * The following error codes are place in aperrno by ap_() functions.
  * @{ */
#define APErr(n) ((AP_ID<<8)|n)
#define AP_ACCES		APErr( 1) /**< Request to bind to specified address denied. */
#define AP_AGAIN		APErr( 2) /**< Request not completed. */
#define AP_BADATTRVAL		APErr( 3) /**< Bad value for environment attribute. */
#define AP_BADCD_ACT_ID		APErr( 4) /**< Cdata field value invalid: act_id. */
#define AP_BADCD_DIAG		APErr( 5) /**< Cdata field value invalid: diag. */
#define AP_BADCD_EVT		APErr( 6) /**< Cdata field value invalid: evt. */
#define AP_BADCD_OLD_ACT_ID	APErr( 7) /**< Cdata field value invalid: old_act_id. */
#define AP_BADCD_OLD_CONN_ID	APErr( 8) /**< Cdata field value invalid: old_conn_id. */
#define AP_BADCD_RES		APErr( 9) /**< Cdata field value invalid: res. */
#define AP_BADCD_RES_SRC	APErr(10) /**< Cdata field value invalid: res_src. */		/* APLI only. */
#define AP_BADCD_RESYNC_TYPE	APErr(11) /**< Cdata field value invalid: resync_type. */
#define AP_BADCD_RSN		APErr(12) /**< Cdata field value invalid: rsn. */
#define AP_BADCD_SRC		APErr(13) /**< Cdata field value invalid: src. */		/* APLI only. */
#define AP_BADCD_SYNC_P_SN	APErr(14) /**< Cdata field value invalid: sync_p_sn. */
#define AP_BADCD_SYNC_TYPE	APErr(15) /**< Cdata field value invalid: sync_type. */
#define AP_BADCD_TOKENS		APErr(16) /**< Cdata field value invalid: tokens. */
#define AP_BADENC		APErr(17) /**< Bad encoding choice in enveloping function. */	/* APLI only. */
#define AP_BADENV		APErr(18) /**< A mandatory attribute is not set. */
#define AP_BADF			APErr(19) /**< Not a presentation service endpoint. */
#define AP_BADFLAGS		APErr(20) /**< The specified combination of flags is invalid. */
#define AP_BADFREE		APErr(21) /**< Could not free structure members. */
#define AP_BADKIND		APErr(22) /**< Unknown structure type. */
#define AP_BADLSTATE		APErr(23) /**< Instance in bad state for that command. */
#define AP_BADPARSE		APErr(24) /**< Attribute parse failed. */
#define AP_BADPRIM		APErr(25) /**< Unrecognized primitive from user. */
#define AP_BADREF		APErr(26) /**< Bad reference in enveloping function. */		/* APLI only. */
#define AP_BADRESTR		APErr(27) /**< Attributes not restore due to more bit on. */
#define AP_BADROLE		APErr(28) /**< Request invalid due to value of AP_ROLE. */
#define AP_BADSAVE		APErr(29) /**< Attributes not saved due to more bit on. */
#define AP_BADSAVEF		APErr(30) /**< Invalid FILE pointer. */
#define AP_BADUBUF		APErr(31) /**< Bad length for user data. */
#define AP_HANGUP		APErr(32) /**< Association closed or aborted. */
#define AP_INTERNAL		APErr(33) /**< Internal error. */				/* APLI only. */
#define AP_LOOK			APErr(34) /**< A pending event requires attention. */
#define AP_NOATTR		APErr(35) /**< No such attribute. */
#define AP_NOENV		APErr(36) /**< No environment for that fd. */
#define AP_NOERROR		(0)       /**< No error. */					/* APLI only. */
#define AP_NOMEM		APErr(37) /**< Could not allocate enough memory. */
#define AP_NOREAD		APErr(38) /**< Attribute is not readable. */
#define AP_NOSET		APErr(39) /**< Attribute is not setable. */			/* APLI only. */
#define AP_NOWRITE		APErr(40) /**< Attribute is not writable. */
#define AP_PDUREJ		APErr(41) /**< Invalid PDU rejected. */

/* The following are XAP only (not APLI). */
#define AP_SUCCESS		(0)       /**< Success. */
#define AP_AGAIN_DATA_PENDING	APErr(42) /**< XAP was unable to complete the requested action.  Try
					       again.  There is an event available for the user to
					       receive. */
#define AP_BADALLOC		APErr(43) /**< The ap_user_alloc/ap_user_dealloc argumnent combination
					       was invalid. */
#define AP_BADASLSYN		APErr(44) /**< The transfer syntaxes proposed for the ACSE syntax are
					       not supported. */
#define AP_BADDATA		APErr(45) /**< User data not allowed on this service. */
#define AP_BADNSAP		APErr(46) /**< The format of the NSAP portion of the Presentation
					       Address is not supported. */
#define AP_DATA_OVERFLOW	APErr(47) /**< User data and presentation service pci exceeds 512
					       bytes on session V1 or the length of user data
					       exceeds a locally defined limit, as state in the CSQ. */
#define AP_NOBUF		APErr(48) /**< Could not allocate enough buffers. */
#define AP_NODATA		APErr(49) /**< An attempt was made to send a primitive with no user
					       data. */
#define AP_NO_PRECEDENCE	APErr(50) /**< The resynchronization requested by the local user does
					       not have precedence over the one requested by the
					       remote user. */
#define AP_NOT_SUPPORTED	APErr(51) /**< The action requested is not supported by this
					       implementaiton of XAP. */
#define AP_SUCCESS_DATA_PENDING	APErr(52) /**< The requested action was completed successfully.  There
					       is an event available for the user to receive. */
/** @> */


typedef int (*ap_ualloc_t) (int, ap_osi_vbuf_t **, void **, int, int, unsigned long *);
typedef int (*ap_udealloc_t) (int, ap_osi_vbuf_t *, void *, int, unsigned long *);

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif				/* __BEGIN_DECLS */

/** @name XAP Library Functions
  * These are the more formal version fo the XAP library functions.  They are strong aliased to the
  * thread-safe "__xap_ap_*_r()" version below.  Where a "_r" version does not exist, the
  * "__xap_ap_*()" version below is used in its stead.
  * @{ */
/* *INDENT-OFF* */
/** XAP Library Function: ap_bind - Bind an XAP instance. */
int ap_bind(int fd, unsigned long *aperrno_p);
/** XAP Library Function: ap_close - Close XAP instance. */
int ap_close(int fd, unsigned long *apperno_p);
/** XAP Library Function: ap_error - Return an error message. */
const char *ap_error(unsigned long aperrno);
/** XAP Library Function: ap_free - Free memory from XAP data structures. */
int ap_free(int fd, unsigned long kind, void *val, unsigned long *apperno_p);
/** XAP Library Function: ap_get_env - Get XAP environment attribute. */
int ap_get_env(int fd, unsigned long attr, void *val, unsigned long *aperrno_p);
/** XAP Library Function: ap_init_env - Initialize an XAP environment. */
int ap_init_env(int fd, const char *env_file, int flags, unsigned long *apperno_p);
/** XAP Library Function: ap_ioctl - Control an XAP provider. */
int ap_ioctl(int fd, int request, ap_val_t argument, unsigned long *apperno_p);
/** XAP Library Function: ap_look - Look ahead at waiting XAP service primitive. */
int ap_look(int fd, unsigned long *sptype, ap_cdata_t * cdata, ap_osi_vbuf_t ** ubuf, int *flags, unsigned long *apperno_p);
/** XAP Library Function: ap_open - Open XAP instance. */
int ap_open(const char *provider, int oflags, ap_ualloc_t ap_user_alloc, ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p);
/** XAP Library Function: ap_poll - Poll XAP resources. */
int ap_poll(ap_pollfd_t fds[], int nfds, int timeout, unsigned long *apperno_p);
/** XAP Library Function: ap_rcv - Receive XAP service primitive. */
int ap_rcv(int fd, unsigned long *sptype, ap_cdata_t * cdata, ap_osi_vbuf_t ** ubuf, int *flags, unsigned long *aperrno_p);
/** XAP Library Function: ap_restore - Restore an XAP environment. */
int ap_restore(int fd, FILE * savef, int oflags, ap_ualloc_t ap_user_alloc, ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p);
/** XAP Library Function: ap_save - Save an XAP environment. */
int ap_save(int fd, FILE *savef, unsigned long *apperno_p);
/** XAP Library Function: ap_set_env - Set XAP environment attribute. */
int ap_set_env(int fd, unsigned long attr, ap_val_t val, unsigned long *apperno_p);
/** XAP Library Function: ap_snd - Send XAP service primitive. */
int ap_snd(int fd, unsigned long sptype, ap_cdata_t *cdata, ap_osi_vbuf_t *ubuf, int flags, unsigned long *aperrno_p);
/* *INDENT-ON* */
/** @} */

/** @name Non-Thread-Saft XAP Library Functions
  * These are the non-thread-safe versions of the XAP Library functions.  They can be called by
  * directly using thes symbols if thread safety is not requred and they my be just a little bit
  * faster that the "ap_" versions (which are the same as the "_r" thread-safe versions).
  * @{ */
const char *__xap_ap_error(unsigned long aperrno);
int __xap_ap_free(int fd, unsigned long kind, void *val, unsigned long *apperno_p);
int __xap_ap_get_env(int fd, unsigned long attr, void *val, unsigned long *aperrno_p);
int __xap_ap_init_env(int fd, const char *env_file, int flags, unsigned long *apperno_p);
int __xap_ap_ioctl(int fd, int request, ap_val_t argument, unsigned long *apperno_p);
int __xap_ap_look(int fd, unsigned long *sptype, ap_cdata_t * cdata, ap_osi_vbuf_t ** ubuf,
		  int *flags, unsigned long *apperno_p);
int __xap_ap_open(const char *provider, int oflags, ap_ualloc_t ap_user_alloc,
		  ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p);
int __xap_ap_poll(ap_pollfd_t fds[], int nfds, int timeout, unsigned long *apperno_p);
int __xap_ap_rcv(int fd, unsigned long *sptype, ap_cdata_t * cdata, ap_osi_vbuf_t ** ubuf,
		 int *flags, unsigned long *aperrno_p);
int __xap_ap_restore(int fd, FILE * savef, int oflags, ap_ualloc_t ap_user_alloc,
		     ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p);
int __xap_ap_save(int fd, FILE * savef, unsigned long *apperno_p);
int __xap_ap_set_env(int fd, unsigned long attr, ap_val_t val, unsigned long *apperno_p);
int __xap_ap_snd(int fd, unsigned long sptype, ap_cdata_t * cdata, ap_osi_vbuf_t * ubuf, int flags,
		 unsigned long *aperrno_p);
/** @} */

/** @name Thread-Safe XAP Library Functions
  * These are thread-safe versions of the XAP Library functions.  Functions that do not appear on
  * this list (with an "_r") do not require thread protection (normally because they can contain a
  * thread cancellation point and they do not accept a file descriptor for which read lock
  * protection is required).
  *
  * These are the thread-safe (reentrant), and asynchronous thread cancellation conforming versions
  * of the XAP Library functions without the "_r" at the end.  Many of these functions contain
  * asynchronous thread cacellation deferral because they cannot thread cancellation.
  *
  * Functions that do not appear on this list (with an "_r") do not require thread protection
  * (normally because they can contain thread cancellation points and they do not accept a file
  * descriptor for which read lock protection is required.)
  * @{ */
int __xap_ap_get_env_r(int fd, unsigned long attr, void *val, unsigned long *aperrno_p);
int __xap_ap_init_env_r(int fd, const char *env_file, int flags, unsigned long *apperno_p);
int __xap_ap_ioctl_r(int fd, int request, ap_val_t argument, unsigned long *apperno_p);
int __xap_ap_look_r(int fd, unsigned long *sptype, ap_cdata_t * cdata, ap_osi_vbuf_t ** ubuf,
		    int *flags, unsigned long *apperno_p);
int __xap_ap_open_r(const char *provider, int oflags, ap_ualloc_t ap_user_alloc,
		    ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p);
int __xap_ap_poll_r(ap_pollfd_t fds[], int nfds, int timeout, unsigned long *apperno_p);
int __xap_ap_rcv_r(int fd, unsigned long *sptype, ap_cdata_t * cdata, ap_osi_vbuf_t ** ubuf,
		   int *flags, unsigned long *aperrno_p);
int __xap_ap_restore_r(int fd, FILE * savef, int oflags, ap_ualloc_t ap_user_alloc,
		       ap_udealloc_t ap_user_dealloc, unsigned long *aperrno_p);
int __xap_ap_save_r(int fd, FILE * savef, unsigned long *apperno_p);
int __xap_ap_set_env_r(int fd, unsigned long attr, ap_val_t val, unsigned long *apperno_p);
int __xap_ap_snd_r(int fd, unsigned long sptype, ap_cdata_t * cdata, ap_osi_vbuf_t * ubuf,
		   int flags, unsigned long *aperrno_p);
/** @} */

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif				/* __END_DECLS */

/** @} */

#endif				/* __XAP_H__ */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
