/*****************************************************************************

 @(#) $Id: xti_mtp.h,v 0.9.2.3 2006/03/03 12:06:09 brian Exp $

 -----------------------------------------------------------------------------

     Copyright (C) 1997-2002 OpenSS7 Corporation.  All Rights Reserved.

                                  PUBLIC LICENSE

     This license is provided without fee, provided that the above copy-
     right notice and this public license must be retained on all copies,
     extracts, compilations and derivative works.  Use or distribution of
     this work in a manner that restricts its use except as provided here
     will render this license void.

     The author(s) hereby waive any and all other restrictions in respect
     of their copyright in this software and its associated documentation.
     The authors(s) of this software place in the public domain any novel
     methods or processes which are embodied in this software.

     The author(s) undertook to write it for the sake of the advancement
     of the Arts and Sciences, but it is provided as is, and the author(s)
     will not take any responsibility in it.

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

 Last Modified $Date: 2006/03/03 12:06:09 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_XTI_MTP_H
#define _SYS_XTI_MTP_H

#ident "@(#) $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#ifndef t_uscalar_t
#define t_uscalar_t	u_int32_t
#define t_scalar_t	int32_t
#endif

typedef struct sockaddr_mtp {
	sa_family_t mtp_family;		/* Address family */
	uint16_t mtp_ni;		/* Network indicator */
	uint16_t mtp_si;		/* Service indicator */
	uint32_t mtp_pc;		/* Point code */
	uint8_t mtp_zero[sizeof(struct sockaddr) - sizeof(sa_family_t) -
			 sizeof(uint16_t) * 2 - sizeof(uint32_t)];
} t_mtp_addr_t;

#define T_SS7_MTP		2	/* MTP level (same as SI value) */

/*
 *  MTP Transport Provider Options
 */
#define T_MTP_SLS		1	/* sequence control parameter */
#define T_MTP_MP		2	/* message priority */
#define T_MTP_DEBUG		3	/* debug */

#define T_MTP_M_CLUSTER			(0x00000100)
#define T_MTP_M_MEMBER			(0x00001000)
#define T_MTP_AVAILABLE			(0x00000001)
#define T_MTP_PROHIBITED		(0x00000002)
#define T_MTP_CONGESTED(__s)		(0x00000003|((__s)<<16))
#define T_MTP_DEST_AVAILABLE		(T_MTP_AVAILABLE|T_MTP_M_MEMBER )
#define T_MTP_DEST_PROHIBITED		(T_MTP_PROHIBITED|T_MTP_M_MEMBER )
#define T_MTP_DEST_CONGESTED(__s)	(T_MTP_CONGESTED(__s)|T_MTP_M_MEMBER )
#define T_MTP_CLUS_AVAILABLE		(T_MTP_AVAILABLE|T_MTP_M_CLUSTER)
#define T_MTP_CLUS_PROHIBITED		(T_MTP_PROHIBITED|T_MTP_M_CLUSTER)
#define T_MTP_CLUS_CONGESTED(__s)	(T_MTP_CONGESTED(__s)|T_MTP_M_CLUSTER)
#define T_MTP_RESTARTING		(0x00001004)
#define T_MTP_RESTARTED			(0x00001005)
#define T_MTP_USER_PART_UNKNOWN		(0x00001006)
#define T_MTP_USER_PART_UNEQUIPPED	(0x00001007)
#define T_MTP_USER_PART_UNAVAILABLE	(0x00001008)
#define T_MTP_CONGESTION_STATUS(__x)	(((__x)>>16))

/*
 *  MTP Management Options
 */
#define T_MTP_OBJ_TYPE_SL	1
#define T_MTP_OBJ_TYPE_LK	2
#define T_MTP_OBJ_TYPE_LS	3
#define T_MTP_OBJ_TYPE_RT	4
#define T_MTP_OBJ_TYPE_RS	5
#define T_MTP_OBJ_TYPE_SP	6
#define T_MTP_OBJ_TYPE_NA	7
/*
 *  Signalling Link
 */
typedef struct t_mtp_mod_sl {
	t_uscalar_t sl_id;		/* signalling link (mux) id */
	t_uscalar_t sl_lk_id;		/* link set id */
	t_uscalar_t sl_slc;		/* signalling link code in lk */
} t_mtp_mod_sl_t;

typedef struct t_mtp_cnf_sl {
	t_uscalar_t sl_id;		/* signalling link (mux) id */
	t_uscalar_t sl_t1;		/* timer t1 value */
	t_uscalar_t sl_t2;		/* timer t2 value */
	t_uscalar_t sl_t3;		/* timer t3 value */
	t_uscalar_t sl_t4;		/* timer t4 value */
	t_uscalar_t sl_t5;		/* timer t5 value */
	t_uscalar_t sl_t12;		/* timer t12 value */
	t_uscalar_t sl_t13;		/* timer t13 value */
	t_uscalar_t sl_t14;		/* timer t14 value */
	t_uscalar_t sl_t17;		/* timer t17 value */
	t_uscalar_t sl_t22;		/* timer t22 value */
	t_uscalar_t sl_t23;		/* timer t23 value */
	t_uscalar_t sl_t24;		/* timer t24 value */
	t_uscalar_t sl_t1t;		/* timer t1t value */
	t_uscalar_t sl_t2t;		/* timer t2t value */
	t_uscalar_t sl_t1s;		/* timer t1s value */
} t_mtp_cnf_sl_t;

/*
 *  Link Set
 */
typedef struct t_mtp_mod_lk {
	t_uscalar_t lk_id;		/* link set id */
	t_uscalar_t lk_ls_id;		/* combined link set id */
	t_uscalar_t lk_adj_pc;		/* adjacent signalling point code */
	t_uscalar_t lk_sls_slot;	/* slot of SLS for this link set */
	t_uscalar_t lk_ni;		/* network indicator for link set */
} t_mtp_mod_lk_t;

typedef struct t_mtp_cnf_lk {
	t_uscalar_t lk_id;		/* link set id */
	t_uscalar_t lk_t7;		/* timer t7 value */
} t_mtp_cnf_lk_t;

/*
 *  Combined Link Set
 */
typedef struct t_mtp_mod_ls {
	t_uscalar_t ls_id;		/* combined link set id */
	t_uscalar_t ls_sp_id;		/* signalling point id */
	t_uscalar_t ls_sls_mask;	/* mask of bits selecting link set */
} t_mtp_mod_ls_t;

/*
 *  Route
 */
typedef struct t_mtp_mod_rt {
	t_uscalar_t rt_id;		/* route id */
	t_uscalar_t rt_rs_id;		/* route set id */
	t_uscalar_t rt_lk_id;		/* link set id */
	t_uscalar_t rt_cost;		/* cost in routeset */
} t_mtp_mod_rt_t;

typedef struct t_mtp_cnf_rt {
	t_uscalar_t rt_id;		/* route id */
	t_uscalar_t rt_t6;		/* timer t6 value */
	t_uscalar_t rt_t8;		/* timer t8 value */
	t_uscalar_t rt_t10;		/* timer t10 value */
	t_uscalar_t rt_t11;		/* timer t11 value */
} t_mtp_cnf_rt_t;

/*
 *  Route Set
 */
typedef struct t_mtp_mod_rs {
	t_uscalar_t rs_id;		/* route set id */
	t_uscalar_t rs_sp_id;		/* signalling point id */
	t_uscalar_t rs_dest;		/* destination point code */
} t_mtp_mod_rs_t;

typedef struct t_mtp_cnf_rs {
	t_uscalar_t rs_id;		/* route set id */
	t_uscalar_t rs_t15;		/* timer t15 value */
	t_uscalar_t rs_t16;		/* timer t16 value */
} t_mtp_cnf_rs_t;

/*
 *  Signalling Point
 */
typedef struct t_mtp_mod_sp {
	t_uscalar_t sp_id;		/* signalling point id */
	t_uscalar_t sp_na_id;		/* network appearance */
	t_uscalar_t sp_pc;		/* point code */
	t_uscalar_t sp_users;		/* mask of equipped users */
} t_mtp_mod_sp_t;

typedef struct t_mtp_cnf_sp {
	t_uscalar_t sp_id;		/* signalling point id */
	t_uscalar_t sp_t18;		/* timer t18 value */
	t_uscalar_t sp_t19;		/* timer t19 value */
	t_uscalar_t sp_t20;		/* timer t20 value */
	t_uscalar_t sp_t21;		/* timer t21 value */
} t_mtp_cnf_sp_t;

/*
 *  Network Appearance
 */
typedef struct t_mtp_mod_na {
	t_uscalar_t na_id;		/* network appearance id */
	t_uscalar_t pvar;		/* default protocol variant */
} t_mtp_mod_na_t;

typedef struct t_mtp_cnf_na {
	t_uscalar_t popt;		/* default protocol options */
	t_uscalar_t na_member;		/* PC member mask */
	t_uscalar_t na_cluster;		/* PC cluster mask */
	t_uscalar_t na_network;		/* PC network mask */
} t_mtp_cnf_na_t;

/*
 *  General Object Unions
 */
typedef struct t_mtp_obj {
	t_uscalar_t type;
	t_uscalar_t id;
} t_mtp_obj_t;

typedef struct t_mtp_mod {
	t_mtp_obj_t mtp_obj;
	union {
		t_mtp_mod_sl_t sl;
		t_mtp_mod_sl_t lk;
		t_mtp_mod_sl_t ls;
		t_mtp_mod_sl_t rt;
		t_mtp_mod_sl_t rs;
		t_mtp_mod_sl_t sp;
		t_mtp_mod_sl_t na;
	} mtp_mod;
} t_mtp_mod_t;

typedef struct t_mtp_cnf {
	t_mtp_obj_t mtp_obj;
	union {
		t_mtp_cnf_sl_t sl;
		t_mtp_cnf_sl_t lk;
		t_mtp_cnf_sl_t ls;
		t_mtp_cnf_sl_t rt;
		t_mtp_cnf_sl_t rs;
		t_mtp_cnf_sl_t sp;
		t_mtp_cnf_sl_t na;
	} mtp_cnf;
} t_mtp_cnf_t;

typedef struct t_mtp_ctl {
	t_mtp_obj_t mtp_obj;
	t_uscalar_t mtp_op;		/* operation */
	union {
		t_uscalar_t val;	/* value */
	} mtp_arg;
} t_mtp_ctl_t;

#define	T_MTP_ADD_OBJECT	15	/* use t_mtp_mod_t */
#define T_MTP_DEL_OBJECT	16	/* use t_mtp_id_t */
#define T_MTP_CNF_OBJECT	17	/* use t_mtp_cnf_t */
#define T_MTP_CTL_OBJECT	18	/* use t_mtp_ctl_t */

#endif				/* _SYS_XTI_MTP_H */
