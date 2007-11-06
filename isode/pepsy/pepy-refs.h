/*****************************************************************************

 @(#) $Id$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ifndef __PEPSY_PEPY_REFS_H__
#define __PEPSY_PEPY_REFS_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* 
 * Header: /xtel/isode/isode/pepsy/RCS/pepy-refs.h,v 9.0 1992/06/16 12:24:03 isode Rel
 *
 *
 * Log: pepy-refs.h,v
 * Revision 9.0  1992/06/16  12:24:03  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */

/*
 * data structure(s) for module T3 (t3.py)
 */

/* bits to use for optional fields */
#define OPT_REAL1	0
#define OPT_INT1	1
#define OPT_INT2	2
#define OPT_ENUM1	3
#define OPT_ENUM2	4
#define OPT_BOOL1	5
#define OPT_BOOL2	6
#define OPT_REAL2	7
#define NUMOPT		8

struct pepy_refs {
	integer t_int;
	integer t_enum;
	struct qbuf *t_qbuf;
	char *t_string;
	char *t_ostring;
	char *t_bstring;
	int t_blen;
	int t_olen;
	char t_bool;
	double t_real;
	OID t_oid;
	PE t_pe;
	PE t_any;
	struct pepy_refs1 *t_def;
	struct pepy_refs1 *t_opt;
	char opt_set[NBITS2NCHARS(NUMOPT)]
};

struct pepy_refs1 {
	integer t_int;
	integer t_int1;
	integer t_enum;
	integer t_enum1;
	struct qbuf *t_qbuf;
	struct qbuf *t_qbuf1;
	char *t_string;
	char *t_string1;
	char *t_ostring;
	char *t_ostring1;
	char *t_bstring;
	int t_blen;
	char *t_bstring1;
	int t_blen1;
	int t_olen;
	int t_olen1;
	char t_bool;
	char t_bool1;
	double t_real;
	double t_real1;
	OID t_oid;
	OID t_oid1;
	PE t_pe;
	PE t_pe1;
	PE t_any;
	char opt_set[NBITS2NCHARS(NUMOPT)]
};

struct rep_elem {
	int r_int;
	char *r_ostring;
	char *r_bstring;
	struct rep_elem *r_next;
};

#define NULLREP_ELEM	(struct rep_elem *)0

struct rep_int {
	int i;
	struct rep_int *r;
};

#define NULLREP_INT	(struct rep_int *)0

struct repeats {
	struct rep_int *rp_sq1;		/* SEQUECE of INTEGER */
	struct rep_elem *rp_sq2;	/* SEQUENCE OF Rep-elem */
	struct rep_int *rp_st1;		/* SET OF INTEGER */
	struct rep_elem *rp_st2;	/* SET OF Rep-elem */

	int rp_choice;
#define RP_INT		1		/* INTEGER */
#define RP_BOOL		2		/* BOOLEAN */
#define RP_OSTRING	3		/* OCTET STRING */
	int rp_int;			/* integer or boolean */
	char rp_bool;			/* integer or boolean */
	char *rp_ostring;
};

#define CD_INT		0
#define CD_C		1
#define CD_D		2
#define NCD_OPT		3

struct codedata {
	PE cd_a;
	PE cd_b;
	PE cd_c;
	PE cd_d;
	integer cd_int;
	integer cd_int1;
	integer cd_int2;
	char *cd_string;
	char *cd_string1;
	char *cd_string2;
	struct codedata *cd_left;
	struct codedata *cd_right;
	char cd_opt_set[NBITS2NCHARS(NCD_OPT)];
	char cd_bool;
	double cd_real;
	OID cd_oid;
	OID cd_oid1;
	OID cd_oid2;
	PE cd_bit;
};

#endif				/* __PEPSY_PEPY_REFS_H__ */
