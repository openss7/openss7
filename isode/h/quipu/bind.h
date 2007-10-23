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

#ifndef __ISODE_QUIPU_BIND_H__
#define __ISODE_QUIPU_BIND_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* bind.h - bind parameters */

/*
 * Header: /xtel/isode/isode/h/quipu/RCS/bind.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 * Log: bind.h,v
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

#ifndef QUIPUBIND
#define QUIPUBIND
#include "quipu/name.h"
#include "quipu/authen.h"

	/* THIS SECTION GIVES BIND PARAMETERS */

/* Directory Bind Argument */

struct ds_bind_arg {
	int dba_version;		/* Treat INT as bitstring */
#define DBA_VERSION_V1988  0
	int dba_auth_type;
#define DBA_AUTH_NONE      0
#define DBA_AUTH_SIMPLE    1
#define DBA_AUTH_STRONG    2
#define DBA_AUTH_EXTERNAL  3
#define DBA_AUTH_PROTECTED 4		/* special case of simple */
	char *dba_time1;		/* Timestamps for protected */
	char *dba_time2;		/* simple authentication.  */
	struct random_number dba_r1;
	struct random_number dba_r2;
	DN dba_dn;
	int dba_passwd_len;		/* len = 0 means no password */
#define DBA_MAX_PASSWD_LEN 512
	char dba_passwd[DBA_MAX_PASSWD_LEN];
	struct signature *dba_sig;	/* signature for strong authen */
	struct certificate_list *dba_cpath;	/* certification path */
	char *dba_vtmp;			/* pepsy */
	int dba_vlen;			/* pepsy */
	struct alg_id dba_alg;
};

struct ds_bind_error {
	int dbe_version;
	int dbe_type;
#define DBE_TYPE_SERVICE 1
#define DBE_TYPE_SECURITY 2
	int dbe_value;			/* takes on values as define in DSE_service or DSE_security
					   according to dbe_type */
	char *dbe_vtmp;			/* pepsy */
	int dbe_vlen;			/* pepsy */

#define DBE_SIZE 512
	int dbe_cc;			/* length */
	char dbe_data[DBE_SIZE];	/* data */

};

#endif

#endif				/* __ISODE_QUIPU_BIND_H__ */
