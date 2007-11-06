/*****************************************************************************

 @(#) $Id: ap_lib.h,v 0.9.2.1 2007/11/06 12:16:49 brian Exp $

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

 $Log: ap_lib.h,v $
 Revision 0.9.2.1  2007/11/06 12:16:49  brian
 - added library and header files

 *****************************************************************************/

#ifndef __AP_LIB_H__
#define __AP_LIB_H__

#ident "@(#) $RCSfile: ap_lib.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @weakgroup apli OpenSS7 APLI Library
  * @{ */

/** @file
  * APLI (User) Header File. */

typedef union {
	long l;
	void *v;
} ap_val_t;

/** @type ap_cdl_t used to convey context definition lists.
  *
  * The context definition list comprises a series of elements in the array seqof_ap_cdl.  The
  * number of elements in this array is given as size_ap_cdl.
  *
  * The presentation context identifier is represented by seqof_ap_cdl[i].pci.
  * seqof_ap_cdl[i].a_sytx is a pointer to an objid_t and is used to convey the abstract syntax name
  * associated with the presentation context identifier.  seqof_ap_cdl[i].m_t_sytx is a pointer to
  * an array of objid_t *, which includes the transfer syntaxes that the association-initiator is
  * capable of supporting for the named abstract syntax.  seqof_ap_cdl[i].size_t_sytx is the number
  * of elements in the seqof_ap_cdl[i].m_t_sytx array.
  *
  * THe context definition list is an optional parameter of the CP PPDU.  Setting size to -1
  * indicates that this paramter is not present.  The presentation context definiton list may also
  * be specified as abseny by invoking ap_set_env(3) with AP_PCDL as the attr argument and a NULL
  * pointer as the val argument.
  */
typedef struct {
	int size_ap_cdl;		/* size of array */
	struct seqof_ap_cdl {
		long pci;		/* protocol context identifier */
		objid_t *a_sytx;	/* pointers to abstract syntaxes */
		int size_t_sytx;	/* number of transfer syntaxes */
		objid_t **m_t_sytx;	/* pointers to transfer syntaxes */
	} *m_ap_cdl;			/* array */
} ap_cdl_t;

/** @type ap_cdrl_t used to convey context definition result lists.
  * 
  * The context definition result list comprises a series of elements in the array seqof_ap_cdrl.
  * The number of elements in this array is given as size_ap_cdrl.
  *
  * There is a one-to-one correspondence between the elements of a context definition list and those
  * in the related context definition result list.  For each entry in the presentation context
  * definition list, the sequof_ap_cdrl[i].res field of the corresponding element in the context
  * definition result list must be set by the association-responder to one of ACCPT, USER_REJ, or
  * PROV_REJ.  If seqof_ap_cdrl[i].res is set to ACCPT,  then seqof_ap_cdr[i].t_sytx indicates the
  * transfer syntax selected by the association-responder.  If seqof_ap_cdrl[i].res is set to either
  * USER_REJ or PROV_REJ, seqof_ap_cdrl[i].prov_rsn is set to the reason for the rejection of the
  * abstract syntax.
  *
  * The possible valus for seqof_ap_cdrl[i].prov_rsn are:
  *
  * RSN_NSPEC (reason not specified)
  * A_SYTX_NSUP (abstract sytnax not supported)
  * PROP_T_SYTX_NSUP (proposed transfer syntax not supported)
  * LCL_LMT_DCS_EXCEEDED (local limit on DSC exceeded)
  *
  * The context defintion result list is an optional parameter of the CPA and CPR PPDUs.  Setting
  * the value of size to -1 indicates that this parameter is not present.  The presentation context
  * definition result list may also be specified as absent by invoking ap_set_env() with AP_PCDRL as
  * the attr argument and a NULL pointer as the val argument.
  */
typedef struct {
	int size_ap_cdrl;		/* size of array */
	struct seqof_ap_cdrl {
		long res;		/* result of negotiation */
		objid_t *t_sytx;	/* negotiated transfer syntax */
		long prov_rsn;		/* reason for rejection */
	} *m_ap_cdrl;			/* array */
} ap_cdrl_t;

#define ACCPT			(0) /**< proposed presentation context accepted */
#define USER_REJ		(1) /**< proposed presentation context rejected by user */
#define PROV_REJ		(2) /**< proposed presentation context rejected by provider */

#define RSN_NSPEC		(0) /**< reason unspecified (unknown) */
#define A_SYTX_NSUP		(1) /**< abstract syntax not supported */
#define PROP_T_SYTX_NSUP	(2) /**< proposed transfer syntax not supported */
#define LCL_LMT_DCS_EXCEEDED	(3) /**< local limit on DCS exceeded */

/** @type ap_octet_string_t
  *
  * The length field of this structure indicates the number of octets in the octet string pointed to
  * by data.  To specify a null octet string value, the length field of the ap_octet_string_t
  * sturcture is set to zero (0).
  */
typedef struct {
	long length;			/* number of octets */
	unsigned char *data;		/* octets */
} ap_octet_string_t;

/** @type ap_conn_id_t
  * @brief used to convey session connection identifiers
  *
  * Each member of the ap_conn_id_t structure is a pointer to a structure of type ap_octet_string_t.
  * The user_ref member must be less than or equal to 64 octets.  The comm_ref member must be less
  * than or equal to 64 octets.  The addtl_ref member must be less than or equal to 4 octets.  The
  * absense of a particular member of a connection identifier may b indicated either by setting the
  * correspongin field to NULL, or by specifying a 0 length ap_octet_string_t.
  */
typedef struct {
	ap_octet_string_t *user_ref;	/* user reference */
	ap_octet_string_t *comm_ref;	/* common reference */
	ap_octet_string_t *addtl_ref;	/* additional reference */
} ap_conn_id_t;

/** @type ap_dcn_t
  * @brief used to convey default context names
  *
  * Both a_sytx and t_sytx are pointers to objects of type ap_objid_t: a_sytx points to the abstract
  * syntax for the default presentation context, while t_sytx points to the trasnfer syntax for the
  * default presentation context.
  *
  * The default context name is an optional parameter of the CP PPDU.  Seting both a_sytx and t_sytx
  * to NULL indicates that this parameter is not present.  The default context names may also be
  * specified to be absent by invoking ap_set_env(3) wtih AP_DPCN as the attr argument and a NULL
  * pointer as the val argument.
  */
typedef struct {
	ap_objid_t *a_sytx;		/* abstract syntax name */
	ap_objid_t *t_sytx;		/* trasnfer syntax name */
} ap_dcn_t;

/** @type ap_dcs_t
  * @brief used to convey the defined context set.
  *
  * The size filed of the ap_dcs_t structure indicates the number of eleemnts in the defined context
  * set.  Each element consists of a presentation context identifier (dcs_elt[i].pci), an abstract
  * syntax (dcs_elt[i].a_sytx), and a transfer syntax name (dcs_elt[i].t_sytx).
  */
typedef struct {
	int size;			/* number of elements */
	struct dcs_elt {
		int pci;		/* presentation context identifier */
		objid_t *a_sytx;	/* abstract syntax */
		objid_t *t_sytx;	/* trasnfer syntax */
	} *dcs;				/* array */
} ap_dcs_t;

/** @type ap_paddr_t
  * @brief used to convey presentation addresses.
  *
  * The pointers p_selector, s_selector, and t_selector point to structures of type
  * ap_octet_string_t that contain the presentation, session, and trasnport selectors respectively.
  * The pointer nsaps points to an array of structures of type ap_octet_string_t that contain the
  * network address.  The n_nsaps element is used to specify how many network address components are
  * in the array.
  *
  * To specify a null selector value, the length field of the ap_octet_string_t structure is set to
  * zero (0).  Presentation addresses are restrcited as follows:
  *
  * - Session selectors cannot exceed 16 octets.
  * - Transport selectors cannot exceeed 32 octets.
  * - Network addresses cannot exceeed 20 octets.
  */
typedef struct {
	ap_octet_string_t *p_selector;	/* presentation selector */
	ap_octet_string_t *s_selector;	/* session selector */
	ap_octet_string_t *t_selector;	/* transport selector */
	int n_nsaps;			/* number of network addresses */
	ap_octet_string_t *nsaps;	/* network addresses */
} ap_paddr_t;

/** @type any_t
  * @brief used to convey ASN.1 objects of type ANY.
  *
  * Structures of type any_t are used to convey ASN.1 obejcts of type ANY (for example, the
  * attribute AP_CLD_AEQ).  The pointer udata points to a buffer that contains user data to be
  * conveyed and the integer size specifies the length of this buffer.  For optional PDU parameters
  * of type ANY, setting size to -1 indicates that the parameter is not present.
  * An optional PDU paraemter can also be specified as not present by calling the function
  * ap_set_env(3) with a NULL pointer as the val argument.
  */
typedef struct {
	int size;
	char *udata;
} any_t;

#define AP_MAXOBJBUF 12

/** @type objid_t
  * @brief used to convey ASN.1 objects of type OBJECT IDENTTIFIER.
  *
  * The objid_t structure is used to convey OBJECT IDENTIFIER values.  OBJECT IDENTIFIER values are
  * stored as the contents octets of their encoded form (without the tag or length octets).  If the
  * number of contents octets is greater than MAXOBJBUF, the contents octets are stored beginning at
  * the memory location pointed to by long_buf.  Otherwise, the contents octets are stored in the
  * short_buf array.  In both cases, length gives the number of contents octets in the OBJECT
  * IDENTIFIER encoding.  The absense of an OBJECT IDENTIFIER parameter is indicated by setting the
  * length field to 0.
  */
typedef struct {
	long length;			/* number of value octets in object ID encoding */
	union {
		unsigned char *long_buf;
		unsigned char short_buf[AP_MAXOBJBUF];
	} b;
} objid_t;

typedef struct {
	long udata_length;		/* lenth of user data */
} a_abort_req_cd_t;

typedef struct {
	long src;			/* source of abort request */
	long udata_length;		/* length of user data */
} a_abort_ind_cd_t;

#define AP_ACSE_USER			(FIXME)	/**< abort source is ACSE user */
#define AP_ACSE_PROV			(FIXME)	/**< abort source is ACSE provider */

typedef struct {
	ulong tokens;			/* intiialt token assignement */
	long udata_length;		/* length of user data */
} a_assoc_req_cd_t;

#define AP_DATA_TOK_REQ			(1<< 0)	/* just guessing */
#define AP_DATA_TOK_ACPT		(1<< 1)	/* just guessing */
#define AP_DATA_TOK_CHOICE		(1<< 2)	/* just guessing */

#define AP_SYNCMINOR_TOK_REQ		(1<< 4)	/* just guessing */
#define AP_SYNCMINOR_TOK_ACPT		(1<< 5)	/* just guessing */
#define AP_SYNCMINOR_TOK_CHOICE		(1<< 6)	/* just guessing */

#define AP_MAJACT_TOK_REQ		(1<< 8)	/* just guessing */
#define AP_MAJACT_TOK_ACPT		(1<< 9)	/* just guessing */
#define AP_MAJACT_TOK_CHOICE		(1<<10)	/* just guessing */

#define AP_RELEASE_TOK_REQ		(1<<12)	/* just guessing */
#define AP_RELEASE_TOK_ACPT		(1<<13)	/* just guessing */
#define AP_RELEASE_TOK_CHOICE		(1<<14)	/* just guessing */

typedef struct {
	ulong tokens;			/* initial token assignement */
	long udata_length;		/* length of user data */
} a_assoc_ind_cd_t;

#define AP_ACCEPT			(FIXME)	/**< Accept the association. */
#define AP_REJ_PERM			(FIXME)	/**< Association permanently rejected. */
#define AP_REJ_TRAN			(FIXME)	/**< Association temporarily rejected. */

/* Provider generated reject reasons. */

/* ACSE provider generate reasons. */
#define AP_NCMN_ACSE_VER		(FIXME)	/**< No common version of ACSE protocol supported. */

/* Presentation provider generated reasons. */
#define AP_CLD_PADDR_UNK		(FIXME)	/**< Called presentation address unknown. */
#define AP_DFCN_NSUP			(FIXME)	/**< Default context not supported. */
#define AP_DIAG_NOVAL			(FIXME)	/**< No value was specified for this parameter. */
#define AP_RSN_NOVAL			(FIXME)	/**< No value was supported for this optional parameter. */
#define AP_LCL_LIM_EXCEEDED		(FIXME)	/**< Local limit exceeded. */
#define AP_NO_PSAP_AVAIL		(FIXME)	/**< No PSAP available. */
#define AP_PRES_VER_NSUP		(FIXME)	/**< Protocol version not supported. */
#define AP_SESS_PROV			(FIXME)	/**< Could not establish session connection. */
#define AP_TEMP_CONGESTION		(FIXME)	/**< Temporary congestion. */
#define AP_UDATA_NREAD			(FIXME)	/**< User data not readable. */
#define AP_INVAL_PPDU_PARM		(FIXME)	/**< Invalid presentation protocol data unit parameter. */
#define AP_UNEXPT_PPDU			(FIXME)	/**< Unexpected presentation protocl data unit. */
#define AP_UNEXPT_PPDU_PARM		(FIXME)	/**< Unexpected presentation protocl data unit parameter. */
#define AP_UNEXPT_SSPRIM		(FIXME)	/**< Unexpected session service primitive. */
#define AP_UNREC_PPDU			(FIXME)	/**< Unrecognized presentation protocol data unit. */
#define AP_UNREC_PPDU_PARM		(FIXME)	/**< Unrecognized presentation protocol data unit parameter. */
#define AP_INVAL_PPDU_PARM		(FIXME)	/**< Invalid presentation protocol data unit parameter. */

/* User geneerated reject reasons. */
#define AP_CLD_AEID_NREC		(FIXME)	/**< Called AE invocation identifier not recognized. */
#define AP_CLD_AEQ_NREC			(FIXME)	/**< Called AE qualifier not recognized. */
#define AP_CLD_APID_NREC		(FIXME)	/**< Called AP invocation identifier not recognized. */
#define AP_CLD_APT_NREC			(FIXME)	/**< Called AP title not recognized. */
#define AP_CLG_AEID_NREC		(FIXME)	/**< Calling AE invocation identifier not recognized. */
#define AP_CLG_AEQ_NREC			(FIXME)	/**< Calling AE qualifier not recognized. */
#define AP_CLG_APID_NREC		(FIXME)	/**< Calling AP invocation identifier not recognized. */
#define AP_CLG_APT_NREC			(FIXME)	/**< Calling AP title not recognized. */
#define AP_CNTX_NAME_NSUP		(FIXME)	/**< Calling application context name not supported. */

/* Common reasons. */
#define AP_NRSN				(FIXME)	/**< No reason given. */
#define AP_NULL				(FIXME)	/**< Null. */
#define AP_NSPEC			(FIXME)	/**< The reason for the abort is not specified. */

/* Events */
#define AP_EVT_NOVAL			(FIXME)	/**< No value supplied for this optional parameter. */
#define AP_PEI_AC			(FIXME)	/**< Alter context PPDU. */
#define AP_PEI_ACA			(FIXME)	/**< Alter context acknowledg PPDU. */
#define AP_PEI_ARP			(FIXME)	/**< Abnormal release provider PPDU. */
#define AP_PEI_ARU			(FIXME)	/**< Abnormal release user PPDU. */
#define AP_PEI_CP			(FIXME)	/**< Connect presentation PPDU. */
#define AP_PEI_CPA			(FIXME)	/**< Connect presentation accept PPDU. */
#define AP_PEI_CPR			(FIXME)	/**< Connect presentaiton reject PPDU. */
#define AP_PEI_RS			(FIXME)	/**< Resynchronize PPDU. */
#define AP_PEI_RSA			(FIXME)	/**< Resynchronize acknowledge PPDU. */
#define AP_PEI_S_ACT_START_IND		(FIXME)	/**< session activity start indication. */
#define AP_PEI_S_ACT_RESUME_IND		(FIXME)	/**< session activity resume indication. */
#define AP_PEI_S_ACT_INT_IND		(FIXME)	/**< Session activity interrupt indication. */
#define AP_PEI_S_ACT_INT_CNF		(FIXME)	/**< Session activity interrupt confirmation. */
#define AP_PEI_S_ACT_DISC_IND		(FIXME)	/**< Session activity discard indication. */
#define AP_PEI_S_ACT_DISC_CNF		(FIXME)	/**< Session activity discard confirmation. */
#define AP_PEI_S_ACT_END_IND		(FIXME)	/**< Session activity end indication. */
#define AP_PEI_S_ACT_END_CNF		(FIXME)	/**< Session activity end confirmation. */
#define AP_PEI_S_CTRLGIVE_IND		(FIXME)	/**< Session control give indication. */
#define AP_PEI_S_P_EXCEPT_REP_IND	(FIXME)	/**< Session provider exception report indication. */
#define AP_PEI_S_U_EXCEPT_REP_IND	(FIXME)	/**< Session user exception report indication. */
#define AP_PEI_S_RELEASE_IND		(FIXME)	/**< Session release indication. */
#define AP_PEI_S_RELEASE_CNF		(FIXME)	/**< Session release confirmation. */
#define AP_PEI_S_SYNCMAJOR_IND		(FIXME)	/**< Session syncrhonize-major indication. */
#define AP_PEI_S_SYNCMAJOR_CNF		(FIXME)	/**< Session syncrhonize-major confirmation. */
#define AP_PEI_S_SYNCMINOR_IND		(FIXME)	/**< Session syncrhonize-minor indication. */
#define AP_PEI_S_SYNCMINOR_CNF		(FIXME)	/**< Session syncrhonize-minor confirmation. */
#define AP_PEI_S_TOKENGIVE_IND		(FIXME)	/**< Session token give indication. */
#define AP_PEI_T_TOKENPLEASE_IND	(FIXME)	/**< Session token please indication. */
#define AP_PEI_TC			(FIXME)	/**< Capability data PPDU. */
#define AP_PEI_TCC			(FIXME)	/**< Capability data acknowledge PPDU. */
#define AP_PEI_TD			(FIXME)	/**< Presentation data PPDU. */
#define AP_PEI_TE			(FIXME)	/**< Expedited data PPDU. */
#define AP_PEI_TTD			(FIXME)	/**< Presentation typed data PPDU. */

typedef struct {
	long res;			/* result of negotiation */
	long diag;			/* reason (if rejected) */
	ulong tokens;			/* initial token assignment */
	long udata_length;		/* length of user data */
} a_assoc_rsp_cd_t;

#define AP_ACSE_SERV_USER		(FIXME)	/**< ACSE service user. */
#define AP_ACSE_SERV_PROV		(FIXME)	/**< ACSE service provider. */
#define AP_PRES_SERV_PROV		(FIXME)	/**< Presentation service provider. */

typedef struct {
	long res;			/* result of negotiation */
	long res_src;			/* source of result */
	long diag;			/* reason (if rejected) */
	ulong tokens;			/* initial token assignment */
	long udata_length;		/* length of user data */
} a_assoc_cnf_cd_t;

typedef struct {
	long rsn;			/* reason for abort */
	long evt;			/* event that cause abort */
} a_pabort_req_cd_t;

typedef struct {
	long rsn;			/* reason for abort */
	long evt;			/* event that caused abort */
} a_pabort_ind_cd_t;

#define AP_REL_NORMAL			(FIXME)	/**< Normal release request. */
#define AP_REL_NOTFINISHED		(FXIME)	/**< Indicates the user is not finished with the association. */
#define AP_REL_URGENT			(FIXME)	/**< Urgent release request. */
#define AP_REL_USER_DEF			(FIXME)	/**< A user defined release request. */
#define AP_REL_NOVAL			(FIXME)	/**< No value was specified for this optional parameter. */

typedef struct {
	long rsn;			/* reason for release */
	long udata_length;		/* length of user data */
} a_release_req_cd_t;

typedef struct {
	long rsn;			/* reason for release */
	long udata_length;		/* length of user data */
} a_release_ind_cd_t;

#define AP_REL_AFFIRM			(FIXME)	/**< Indicates acceptance of the release. */
#define AP_REL_NEGATIVE			(FIXME)	/**< Indicates rejection of the release.  Note that
						     this value can only be used if the session
						     negotiated release functional unit has been
						     negotiated. */

typedef struct {
	long res;			/* result of negotiation */
	long rsn;			/* reason for release */
	long udata_length;		/* length of user data */
} a_release_rsp_cd_t;

typedef struct {
	long res;			/* result of release request */
	long rsn;			/* reason for release */
	long udata_length;		/* length of user data */
} a_release_cnf_cd_t;

#define AP_DATA_TOK			(FIXME)	/**< Data token. */
#define AP_MAJACT_TOK			(FIXME)	/**< Synchronize major/activity token. */
#define AP_SYNCMINOR_TOK		(FIXME)	/**< Synchronize minor token. */
#define AP_RELEASE_TOK			(FIXME)	/**< Release token. */

typedef struct {
	ulong tokens;			/* token assignment */
} p_data_req_cd_t;

typedef struct {
	ulong tokens;			/* token assignement */
	long udata_length;		/* length of user data */
} p_tokengive_req_cd_t;

typedef struct {
	ulong tokens;			/* token assignement */
	long udata_length;		/* length of user data */
} p_tokengive_ind_cd_t;

typedef struct {
	ulong tokens;			/* token assignment */
	long udata_length;		/* length of user data */
} p_tokenplease_req_cd_t;

typedef struct {
	ulong tokens;			/* token assignement */
	long udata_length;		/* length of user data */
} p_tokenplease_ind_cd_t;

typedef union {
	a_assoc_req_cd_t assoc_req;
	a_assoc_ind_cd_t assoc_ind;
	a_assoc_rsp_cd_t assoc_rsp;
	a_assoc_cnf_cd_t assoc_cnf;
	a_abort_req_cd_t abort_req;
	a_abort_ind_cd_t abort_ind;
	a_pabort_req_cd_t pabort_req;
	a_pabort_ind_cd_t pabort_ind;
	a_release_req_cd_t release_req;
	a_release_ind_cd_t release_ind;
	a_release_rsp_cd_t release_rsp;
	a_release_cnf_cd_t release_cnf;
	p_data_req_cd_t data_req;
	p_tokengive_req_cd_t tokengive_req;
	p_tokengive_ind_cd_t tokengive_ind;
	p_tokenplease_req_cd_t tokenplease_req;
	p_tokenplease_ind_cd_t tokenplease_ind;
} ap_cdata_t;

/**
  * Service Primtiive Structure Types
  * @{ */
#define A_ASSOC_REQ_CD_T	(FIXME)
#define A_ASSOC_IND_CD_T	(FIXME)
#define A_ASSOC_RSP_CD_T	(FIXME)
#define A_ASSOC_CNF_CD_T	(FIXME)
#define A_ABORT_REQ_CD_T	(FIXME)
#define A_ABORT_IND_CD_T	(FIXME)
#define A_PABORT_REQ_CD_T	(FIXME)
#define A_PABORT_IND_CD_T	(FIXME)
#define A_RELEASE_REQ_CD_T	(FIXME)
#define A_RELEASE_IND_CD_T	(FIXME)
#define A_RELEASE_RSP_CD_T	(FIXME)
#define A_RELEASE_CNF_CD_T	(FIXME)
#define P_DATA_REQ_CD_T		(FIXME)
#define P_TOKENGIVE_REQ_CD_T	(FIXME)
#define P_TOKENGIVE_IND_CD_T	(FIXME)
#define P_TOKENPLEASE_REQ_CD_T	(FIXME)
#define P_TOKENPLEASE_IND_CD_T	(FIXME)
/** @} */

/** @name APLI Environment Attribute STructure Types
  * The following structure are for use with APLI Environment Attributes.
  * @{ */
#define AP_CDATA_T		(FIXME)	/**< Control Data. */
#define AP_CDL_T		(FIXME)	/**< Context Definition List. */
#define AP_CDRL_T		(FIXME)	/**< Context Defintiion Result List. */
#define AP_CONN_ID_T		(FIXME)	/**< Sesion Connection Identifier */
#define AP_DCN_T		(FIXME)	/**< Default Context Name */
#define AP_DCS_T		(FIXME)	/**< Defined Context Set */
#define AP_OCTET_STRING_T	(FIXME)	/**< OCTET STRING */
#define AP_PADDR_T		(FIXME)	/**< Presentation Address */
#define AP_ANY_T		(FIXME)	/**< ANY */
#define AP_OBJID_T		(FIXME)	/**< OBJECT IDENTIFIER */

#define ANY_T			AP_ANY_T
#define OBJID_T			AP_OBJID_T
/** @} */

/** @name APLI Environment Attributes
  * The following APLI environment attribtues can be written with ap_set_env() or read with
  * ap_get_env().
  * @{ */
#define AP_ACSE_AVAIL		(FIXME)
#define AP_ACSE_SEL		(FIXME)
#define AP_BIND_PADDR		(FIXME)
#define AP_CLD_AEID		(FIXME)
#define AP_CLD_AEQ		(FIXME)
#define AP_CLD_APID		(FIXME)
#define AP_CLD_APT		(FIXME)
#define AP_CLD_CONN_ID		(FIXME)
#define AP_CLG_AEID		(FIXME)
#define AP_CLG_AEQ		(FIXME)
#define AP_CLG_APID		(FIXME)
#define AP_CLG_CONN_ID		(FIXME)
#define AP_CNTX_NAME		(FIXME)
#define AP_DCS			(FIXME)
#define AP_DPCN			(FIXME)
#define AP_DPCR			(FIXME)
#define AP_INIT_SYNC_PT		(FIXME)
#define AP_LCL_PADDR		(FIXME)
#define AP_LIB_AVAIL		(FIXME)
#define AP_LIB_SEL		(FIXME)
#define AP_MODE_AVAIL		(FIXME)
#define AP_MODE_SEL		(FIXME)
#define AP_MSTATE		(FIXME)
#define AP_PCDL			(FIXME)
#define AP_PCDRL		(FIXME)
#define AP_PFU_AVAIL		(FIXME)
#define AP_PFU_SEL		(FIXME)
#define AP_PRES_AVAIL		(FIXME)
#define AP_PRES_SEL		(FIXME)
#define AP_REM_PADDR		(FIXME)
#define AP_ROLE_ALLOWED		(FIXME)
#define AP_ROLE_CURRENT		(FIXME)
#define AP_RSP_AEID		(FIXME)
#define AP_RSP_AEQ		(FIXME)
#define AP_RSP_APID		(FIXME)
#define AP_RSP_APT		(FIXME)
#define AP_SESS_AVAIL		(FIXME)
#define AP_SESS_SEL		(FIXME)
#define AP_SFU_AVAIL		(FIXME)
#define AP_SFU_SEL		(FIXME)
#define AP_STATE		(FIXME)
#define AP_STREAM_FLAGS		(FIXME)
#define AP_TOKENS_AVAIL		(FIXME)
#define AP_TOKENS_OWNED		(FIXME)
/** @} */

/** @name ACSE/Presentation Service Primitive Types
  * The following service primitives are passed to ap_snd() or received from ap_rcv().
  * @{ */
#define AP_A_ASSOC_REQ		(FIXME)	/**< A-ASSOCIATE request */
#define AP_A_ASSOC_IND		(FIXME)	/**< A-ASSOCIATE indication */
#define AP_A_ASSOC_RSP		(FIXME)	/**< A-ASSOCIATE response */
#define AP_A_ASSOC_CNF		(FIXME)	/**< A-ASSOCIATE confirmation */
#define AP_A_RELEASE_REQ	(FIXME)	/**< A-RELEASE request */
#define AP_A_RELEASE_IND	(FIXME)	/**< A-RELEASE indication */
#define AP_A_RELEASE_RSP	(FIXME)	/**< A-RELEASE response */
#define AP_A_RELEASE_CNF	(FIXME)	/**< A-RELEASE confirmation */
#define AP_A_ABORT_REQ		(FIXME)	/**< A-ABORT request */
#define AP_A_ABORT_IND		(FIXME)	/**< A-ABORT indication */
#define AP_A_PABORT_REQ		(FIXME)	/**< A-P-ABORT request */
#define AP_A_PABORT_IND		(FIXME)	/**< A-P-ABORT indication */
#define AP_P_DATA_REQ		(FIXME)	/**< P-DATA request */
#define AP_P_DATA_IND		(FIXME)	/**< P-DATA indication */
#define AP_P_TOKENGIVE_REQ	(FIXME)	/**< P-TOKEN-GIVE request */
#define AP_P_TOKENGIVE_IND	(FIXME)	/**< P-TOKEN-GIVE indication */
#define AP_P_TOKENPLEASE_REQ	(FIXME)	/**< P-TOKEN-PLEASE request */
#define AP_P_TOKENPLEASE_IND	(FIXME)	/**< P-TOKEN-PLEASE indication */
/** @} */

#ifndef __KERNEL__

#define A_ASSOC_REQ		AP_A_ASSOC_REQ
#define A_ASSOC_IND		AP_A_ASSOC_IND
#define A_ASSOC_RSP		AP_A_ASSOC_RSP
#define A_ASSOC_CNF		AP_A_ASSOC_CNF
#define A_RELEASE_REQ		AP_A_RELEASE_REQ
#define A_RELEASE_IND		AP_A_RELEASE_IND
#define A_RELEASE_RSP		AP_A_RELEASE_RSP
#define A_RELEASE_CNF		AP_A_RELEASE_CNF
#define A_ABORT_REQ		AP_A_ABORT_REQ
#define A_ABORT_IND		AP_A_ABORT_IND
#define A_PABORT_REQ		AP_A_PABORT_REQ
#define A_PABORT_IND		AP_A_PABORT_IND
#define P_DATA_REQ		AP_P_DATA_REQ
#define P_DATA_IND		AP_P_DATA_IND
#define P_TOKENGIVE_REQ		AP_P_TOKENGIVE_REQ
#define P_TOKENGIVE_IND		AP_P_TOKENGIVE_IND
#define P_TOKENPLEASE_REQ	AP_P_TOKENPLEASE_REQ
#define P_TOKENPLEASE_IND	AP_P_TOKENPLEASE_IND

#endif				/* __KERNEL__ */

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

#ifndef __KERNEL__

/** @var extern int ap_errno;
  * APLI error return.  ap_errno is a modifiable lvalue of type int.  The above definition is
  * typical of a single-threaded environment.  In a multi-threading environment a typical defition
  * of ap_errno is:
  *
  * @code
  * #define ap_errno (*(_ap_errno()))
  * extern int *_ap_errno(void);
  * @endcode
  */
#define ap_errno (*(_ap_errno()))
extern int *_ap_errno();

/** @name APLI Library Functions
  * These are the formal version of the APLI library functions.  They are strong aliased to the
  * thread-safe "__apli_ap_*_r()" version below.  Where a "_r" version does not exist, the
  * "__apli_ap_*()" version below is used in its stead.
  * @{ */
/* *INDENT-OFF* */
/** APLI Library Function: ap_open - Open APLI instance. */
int ap_open(const char *pathname, int oflags);
/** APLI Library Function: ap_init_env - Initialize APLI environment. */
int ap_init_env(int fd, const char *env_file, int flags);
/** APLI Library Function: ap_restore - Restore APLI environment. */
int ap_restore(int fd, FILE *savef);
/** APLI Library Function: ap_get_env - Get APLI environment attribute. */
int ap_get_env(int fd, unsigned long attr, void *val);
/** APLI Library Function: ap_set_env - Set APLI environment attribtue.  */
int ap_set_env(int fd, unsigned long attr, ap_val_t val);
/** APLI Library Function: ap_save - Save an APLI environment. */
int ap_save(int fd, FILE *savef);
/** APLI Library Function: ap_snd - Send APLI service primitive. */
int ap_snd(int fd, unsigned long sptype, ap_cdata_t * cdata, struct osi_buf *ubuf, int flags);
/** APLI Library Function: ap_rcv - Receive APLI service primitive. */
int ap_rcv(int fd, unsigned long *sptype, ap_cdata_t * cdata, struct osi_buf *ubuf, int *flags);
/** APLI Library Function: ap_free - Free memory from APLI data structures. */
int ap_free(unsigned long kind, void *val);
/** APLI Library Function: ap_poll - Poll APLI resources. */
int ap_poll(struct ap_pollfd *fds, int nfds, int timeout);
/** APLI Library Function: ap_error - Return an error message. */
const char *ap_error(void);
/** APLI Library Function: ap_close - Close APLI instance. */
int ap_close(int fd);
/* *INDENT-OFF* */
/** @} */

/** @name Non-Thread-Safe APLI Library Functions
  * These are the non-thread-safe versions of the APLI Library functions.  They can be called by
  * directly using these symbols if thread safety is not required and they may be just a little bit
  * faster than the "ap_" versions (which are the same as the "_r" thread-safe versions).
  * @{ */
int __apli_ap_open(const char *pathname, int oflags);
int __apli_ap_init_env(int fd, const char *env_file, int flags);
int __apli_ap_restore(int fd, FILE *savef);
int __apli_ap_get_env(int fd, unsigned long attr, void *val);
int __apli_ap_set_env(int fd, unsigned long attr, ap_val_t val);
int __apli_ap_save(int fd, FILE *savef);
int __apli_ap_snd(int fd, unsigned long sptype, ap_cdata_t * cdata, struct osi_buf *ubuf, int flags);
int __apli_ap_rcv(int fd, unsigned long *sptype, ap_cdata_t * cdata, struct osi_buf *ubuf, int *flags);
int __apli_ap_free(unsigned long kind, void *val);
int __apli_ap_poll(struct ap_pollfd *fds, int nfds, int timeout);
const char *__apli_ap_error(void);
int __apli_ap_close(int fd);
/** @} */

/** @name Thread-Safe APLI Library Functions
  * These are thread-safe versions of the APLI Library functions.  Functions that do not appear on
  * this list (with an "_r") do not require thread protection (normally because they can contain a
  * thread cancellation point and they do not accept a file descriptor for which read lock
  * protection is required.)
  *
  * These are the thread-safe (reentrant), and asynchronous thread cancellation conforming versions
  * of the APLI Library functions without the "_r" at the end.  Many of these functions contain
  * asyncrhonous thread cancellation deferral because they cannot contain thread cancellation
  * points.
  *
  * Functions that do not appear on this list (with an "_r") do not require thread protection
  * (normally because they can contain a thread cancellation point and they do not accept a file
  * descriptor for which read lock protection is required.)
  * @{ */
int __apli_ap_open_r(const char *pathname, int oflags);
int __apli_ap_init_env_r(int fd, const char *env_file, int flags);
int __apli_ap_restore_r(int fd, FILE *savef);
int __apli_ap_get_env_r(int fd, unsigned long attr, void *val);
int __apli_ap_set_env_r(int fd, unsigned long attr, ap_val_t val);
int __apli_ap_save_r(int fd, FILE *savef);
int __apli_ap_snd_r(int fd, unsigned long sptype, ap_cdata_t * cdata, struct osi_buf *ubuf, int flags);
int __apli_ap_rcv_r(int fd, unsigned long *sptype, ap_cdata_t * cdata, struct osi_buf *ubuf, int *flags);
int __apli_ap_close_r(int fd);
/** @} */

#endif				/* __KERNEL__ */

/** @} */

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* __AP_LIB_H__ */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
