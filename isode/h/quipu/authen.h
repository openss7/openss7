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

#ifndef __ISODE_QUIPU_AUTHEN_H__
#define __ISODE_QUIPU_AUTHEN_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* authen.h - parameters for strong authentication */

/*
 * Header: /xtel/isode/isode/h/quipu/RCS/authen.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 *
 * Log: authen.h,v
 * Revision 9.0  1992/06/16  12:23:11  isode
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

#ifndef QUIPUAUTHEN
#define QUIPUAUTHEN
#include "quipu/name.h"

/* Structures for strong authentication */

struct alg_id {
	OID algorithm;
	PE asn;
	int p_type;
#define ALG_PARM_ABSENT  0
#define ALG_PARM_UNKNOWN 1
#define ALG_PARM_NUMERIC 2
	union {
		int numeric;
	} un;
};

struct random_number {
	int n_bits;
	char *value;
};

struct key_info {
	struct alg_id alg;
	int n_bits;
	char *value;
};

struct validity {
	char *not_before;
	char *not_after;
};

struct signature {
	struct alg_id alg;
	PE encoded;
	int n_bits;
	char *encrypted;
};

struct certificate {
	struct alg_id alg;
	int version;
	int serial;
	DN issuer;
	DN subject;
	struct validity valid;
	struct key_info key;
	struct signature sig;
};

struct certificate_list {
	struct certificate *cert;
	struct certificate *reverse;
	struct certificate_list *next, *prev;
	struct certificate_list *superior;
};

struct revoked_certificate {
	struct alg_id alg;
	DN subject;
	int serial;
	char *revocation_date;
	struct revoked_certificate *next;
};

struct revocation_list {
	struct alg_id alg;
	DN issuer;
	char *last_update;
	char *next_update;		/* For RFC 1040 format only */
	struct revoked_certificate *revoked;
	struct signature sig;
	struct signature sig2;
	char test[1];			/* For pepsy to test for revoked certificate */
};

struct ca_record {
	struct key_info key;
	DN name;
	struct validity valid;
	/* parameters controlling jurisdiction would go here */
	struct ca_record *next;
};

struct protected_password {
	char *passwd;
	int n_octets;
	char is_protected[1];
	char *time1;
	char *time2;
	struct random_number *random1;
	struct random_number *random2;
	struct alg_id *alg_id;		/* NULL - for pepsy */
};

#endif

#endif				/* __ISODE_QUIPU_AUTHEN_H__ */
