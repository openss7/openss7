/*****************************************************************************

 @(#) $Id: xti_mtp.h,v 0.9.2.7 2007/02/21 01:09:07 brian Exp $

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

 Last Modified $Date: 2007/02/21 01:09:07 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti_mtp.h,v $
 Revision 0.9.2.7  2007/02/21 01:09:07  brian
 - updating mtp.c driver, better mi_open allocators

 Revision 0.9.2.6  2007/02/13 14:05:30  brian
 - corrected ulong and long for 32-bit compat

 Revision 0.9.2.5  2006/09/25 12:10:09  brian
 - updated and doxygenified headers

 *****************************************************************************/

#ifndef _SYS_XTI_MTP_H
#define _SYS_XTI_MTP_H

#ident "@(#) $RCSfile: xti_mtp.h,v $ $Name:  $($Revision: 0.9.2.7 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup xnet
  * @{ */

/** @file
  * XTI MTP-Specific header file.
  *
  * This file contains specific MTP option and management parameters.  */

#ifndef t_uscalar_t
#define t_uscalar_t	u_int32_t
#define t_scalar_t	int32_t
#endif

/** MTP Address structure. */
typedef struct sockaddr_mtp {
	sa_family_t mtp_family;		/**< Address family. */
	uint16_t mtp_ni;		/**< Network indicator. */
	uint16_t mtp_si;		/**< Service indicator. */
	uint32_t mtp_pc;		/**< Point code. */
	uint8_t mtp_zero[sizeof(struct sockaddr) - sizeof(sa_family_t) -
			 sizeof(uint16_t) * 2 - sizeof(uint32_t)];
} t_mtp_addr_t;

/** XTI MTP Option Level */
#define T_SS7_MTP		2	/**< MTP level (same as SI value). */

/** @name MTP Transport Provider Options
  * @{ */
#define T_MTP_SLS		1	/**< Sequence control parameter. */
#define T_MTP_MP		2	/**< Message priority. */
#define T_MTP_DEBUG		3	/**< Debug. */
/** @} */

/** @name MTP Reason Codes
  * @{ */
#define T_MTP_M_CLUSTER			(0x00000100)
#define T_MTP_M_MEMBER			(0x00001000)
#define T_MTP_AVAILABLE			(0x00000001)
#define T_MTP_RESTRICTED		(0x00000002)
#define T_MTP_PROHIBITED		(0x00000003)
#define T_MTP_CONGESTED(__s)		(0x00000004|((__s)<<16))
#define T_MTP_DEST_AVAILABLE		(T_MTP_AVAILABLE|T_MTP_M_MEMBER)
#define T_MTP_DEST_RESTRICTED		(T_MTP_RESTRICTED|T_MTP_M_MEMBER)
#define T_MTP_DEST_PROHIBITED		(T_MTP_PROHIBITED|T_MTP_M_MEMBER)
#define T_MTP_DEST_CONGESTED(__s)	(T_MTP_CONGESTED(__s)|T_MTP_M_MEMBER)
#define T_MTP_CLUS_AVAILABLE		(T_MTP_AVAILABLE|T_MTP_M_CLUSTER)
#define T_MTP_CLUS_RESTRICTED		(T_MTP_RESTRICTED|T_MTP_M_CLUSTER)
#define T_MTP_CLUS_PROHIBITED		(T_MTP_PROHIBITED|T_MTP_M_CLUSTER)
#define T_MTP_CLUS_CONGESTED(__s)	(T_MTP_CONGESTED(__s)|T_MTP_M_CLUSTER)
#define T_MTP_RESTARTING		(0x00001005)
#define T_MTP_RESTARTED			(0x00001006)
#define T_MTP_USER_PART_UNKNOWN		(0x00001007)
#define T_MTP_USER_PART_UNEQUIPPED	(0x00001008)
#define T_MTP_USER_PART_UNAVAILABLE	(0x00001009)
#define T_MTP_CONGESTION_STATUS(__x)	(((__x)>>16))
/** @} */

/*
 *  MTP Management Options
 */

/** @name MTP Object Types
  * @{ */
#define T_MTP_OBJ_TYPE_SL	1	/**< Signalling link.  */
#define T_MTP_OBJ_TYPE_LK	2	/**< Link. */
#define T_MTP_OBJ_TYPE_LS	3	/**< Link set. */
#define T_MTP_OBJ_TYPE_RT	4	/**< Route. */
#define T_MTP_OBJ_TYPE_RS	5	/**< Route set. */
#define T_MTP_OBJ_TYPE_SP	6	/**< Signalling point. */
#define T_MTP_OBJ_TYPE_NA	7	/**< Network appearance. */
/** @} */

/** @name Signalling Link Structures
  * @{ */
/** Signalling link modify structure. */
typedef struct t_mtp_mod_sl {
	t_uscalar_t sl_id;		/**< Signalling link (mux) id. */
	t_uscalar_t sl_lk_id;		/**< Link set id. */
	t_uscalar_t sl_slc;		/**< Signalling link code in lk. */
} t_mtp_mod_sl_t;

/** Signalling link configuration structure. */
typedef struct t_mtp_cnf_sl {
	t_uscalar_t sl_id;		/**< Signalling link (mux) id. */
	t_uscalar_t sl_t1;		/**< Timer t1 value. */
	t_uscalar_t sl_t2;		/**< Timer t2 value. */
	t_uscalar_t sl_t3;		/**< Timer t3 value. */
	t_uscalar_t sl_t4;		/**< Timer t4 value. */
	t_uscalar_t sl_t5;		/**< Timer t5 value. */
	t_uscalar_t sl_t12;		/**< Timer t12 value. */
	t_uscalar_t sl_t13;		/**< Timer t13 value. */
	t_uscalar_t sl_t14;		/**< Timer t14 value. */
	t_uscalar_t sl_t17;		/**< Timer t17 value. */
	t_uscalar_t sl_t22;		/**< Timer t22 value. */
	t_uscalar_t sl_t23;		/**< Timer t23 value. */
	t_uscalar_t sl_t24;		/**< Timer t24 value. */
	t_uscalar_t sl_t1t;		/**< Timer t1t value. */
	t_uscalar_t sl_t2t;		/**< Timer t2t value. */
	t_uscalar_t sl_t1s;		/**< Timer t1s value. */
} t_mtp_cnf_sl_t;

/** @} */

/** @name Link Set
  * @{ */
/** Link modify structure. */
typedef struct t_mtp_mod_lk {
	t_uscalar_t lk_id;		/**< Link set id. */
	t_uscalar_t lk_ls_id;		/**< Combined link set id. */
	t_uscalar_t lk_adj_pc;		/**< Adjacent signalling point code. */
	t_uscalar_t lk_sls_slot;	/**< Slot of SLS for this link set. */
	t_uscalar_t lk_ni;		/**< Network indicator for link set. */
} t_mtp_mod_lk_t;

/** Link configuration structure. */
typedef struct t_mtp_cnf_lk {
	t_uscalar_t lk_id;		/**< Link set id. */
	t_uscalar_t lk_t7;		/**< Timer t7 value. */
} t_mtp_cnf_lk_t;

/** @} */

/** @name Combined Link Set
  * @{ */
/** Link set modify structure. */
typedef struct t_mtp_mod_ls {
	t_uscalar_t ls_id;		/**< Combined link set id. */
	t_uscalar_t ls_sp_id;		/**< Signalling point id. */
	t_uscalar_t ls_sls_mask;	/**< Mask of bits selecting link set. */
} t_mtp_mod_ls_t;

/** Link set configuratoin structure. */
typedef struct t_mtp_cnf_ls {
	t_uscalar_t ls_id;		/**< Combined link set id. */
} t_mtp_cnf_ls_t;

/** @} */

/** @name Route
  * @{ */
/** Route modify structure. */
typedef struct t_mtp_mod_rt {
	t_uscalar_t rt_id;		/**< Route id. */
	t_uscalar_t rt_rs_id;		/**< Route set id. */
	t_uscalar_t rt_lk_id;		/**< Link set id. */
	t_uscalar_t rt_cost;		/**< Cost in routeset. */
} t_mtp_mod_rt_t;

/** Route configuration structure. */
typedef struct t_mtp_cnf_rt {
	t_uscalar_t rt_id;		/**< Route id. */
	t_uscalar_t rt_t6;		/**< Timer t6 value. */
	t_uscalar_t rt_t8;		/**< Timer t8 value. */
	t_uscalar_t rt_t10;		/**< Timer t10 value. */
	t_uscalar_t rt_t11;		/**< Timer t11 value. */
} t_mtp_cnf_rt_t;

/** @} */

/** @name Route Set
  * @{ */
/** Route set modify structure. */
typedef struct t_mtp_mod_rs {
	t_uscalar_t rs_id;		/**< Route set id. */
	t_uscalar_t rs_sp_id;		/**< Signalling point id. */
	t_uscalar_t rs_dest;		/**< Destination point code. */
} t_mtp_mod_rs_t;

/** Route set configuration structure. */
typedef struct t_mtp_cnf_rs {
	t_uscalar_t rs_id;		/**< Route set id. */
	t_uscalar_t rs_t15;		/**< Timer t15 value. */
	t_uscalar_t rs_t16;		/**< Timer t16 value. */
} t_mtp_cnf_rs_t;

/** @} */

/** @name Signalling Point
  * @{ */
/** Signalling point modify structure. */
typedef struct t_mtp_mod_sp {
	t_uscalar_t sp_id;		/**< Signalling point id. */
	t_uscalar_t sp_na_id;		/**< Network appearance. */
	t_uscalar_t sp_pc;		/**< Point code. */
	t_uscalar_t sp_users;		/**< Mask of equipped users. */
} t_mtp_mod_sp_t;

/** Signalling point configuration structure. */
typedef struct t_mtp_cnf_sp {
	t_uscalar_t sp_id;		/**< Signalling point id. */
	t_uscalar_t sp_t18;		/**< Timer t18 value. */
	t_uscalar_t sp_t19;		/**< Timer t19 value. */
	t_uscalar_t sp_t20;		/**< Timer t20 value. */
	t_uscalar_t sp_t21;		/**< Timer t21 value. */
} t_mtp_cnf_sp_t;

/** @} */

/** @name Network Appearance
  * @{ */
/** Network appearance modify structure. */
typedef struct t_mtp_mod_na {
	t_uscalar_t na_id;		/**< Network appearance id. */
	t_uscalar_t pvar;		/**< Default protocol variant. */
} t_mtp_mod_na_t;

/** Network appearance configuration structure. */
typedef struct t_mtp_cnf_na {
	t_uscalar_t popt;		/**< Default protocol options. */
	t_uscalar_t na_member;		/**< PC member mask. */
	t_uscalar_t na_cluster;		/**< PC cluster mask. */
	t_uscalar_t na_network;		/**< PC network mask. */
} t_mtp_cnf_na_t;

/** @} */

/** @name General Object Unions
  * @{ */
/** MTP Object structure. */
typedef struct t_mtp_obj {
	t_uscalar_t type;		/**< Object type. */
	t_uscalar_t id;			/**< Object identifier. */
} t_mtp_obj_t;

/** MTP Modify structure. */
typedef struct t_mtp_mod {
	t_mtp_obj_t mtp_obj;		/**< Object. */
	union {
		t_mtp_mod_sl_t sl;	/**< Signalling link. */
		t_mtp_mod_lk_t lk;	/**< Link. */
		t_mtp_mod_ls_t ls;	/**< Link set. */
		t_mtp_mod_rt_t rt;	/**< Route. */
		t_mtp_mod_rs_t rs;	/**< Route set. */
		t_mtp_mod_sp_t sp;	/**< Signalling point. */
		t_mtp_mod_na_t na;	/**< Network appearance. */
	} mtp_mod;
} t_mtp_mod_t;

/** MTP Configuration structure. */
typedef struct t_mtp_cnf {
	t_mtp_obj_t mtp_obj;		/**< Object. */
	union {
		t_mtp_cnf_sl_t sl;	/**< Signalling link. */
		t_mtp_cnf_lk_t lk;	/**< Link. */
		t_mtp_cnf_ls_t ls;	/**< Link set. */
		t_mtp_cnf_rt_t rt;	/**< Route. */
		t_mtp_cnf_rs_t rs;	/**< Route set. */
		t_mtp_cnf_sp_t sp;	/**< Signalling point. */
		t_mtp_cnf_na_t na;	/**< Network appearance. */
	} mtp_cnf;
} t_mtp_cnf_t;

/** MTP Control structure. */
typedef struct t_mtp_ctl {
	t_mtp_obj_t mtp_obj;		/**< Object. */
	t_uscalar_t mtp_op;		/**< Operation. */
	union {
		t_uscalar_t val;	/**< Value. */
	} mtp_arg;
} t_mtp_ctl_t;

/** @} */

/** @name MTP Operations
  * @{ */
#define	T_MTP_ADD_OBJECT	15	/**< Use t_mtp_mod_t. */
#define T_MTP_DEL_OBJECT	16	/**< Use t_mtp_id_t. */
#define T_MTP_CNF_OBJECT	17	/**< Use t_mtp_cnf_t. */
#define T_MTP_CTL_OBJECT	18	/**< Use t_mtp_ctl_t. */
/** @} */

/** @} */

#endif				/* _SYS_XTI_MTP_H */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
