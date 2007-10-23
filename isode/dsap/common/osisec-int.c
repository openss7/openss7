/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* osisec-int.c - minimal OSI Security Package */

#ifndef	lint
static char *rcsid =
    "Header: /xtel/isode/isode/dsap/common/RCS/osisec-int.c,v 9.0 1992/06/16 12:12:39 isode Rel";
#endif

/* 
 * Header: /xtel/isode/isode/dsap/common/RCS/osisec-int.c,v 9.0 1992/06/16 12:12:39 isode Rel
 *
 *
 * Log: osisec-int.c,v
 * Revision 9.0  1992/06/16  12:12:39  isode
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

#include "config.h"
#include "psap.h"
#include "pepsy.h"
#include "quipu/ds_error.h"
#include "osisec-stub.h"

static struct SecurityServices null_serv = NULLSECURITYSERVICES;

struct SecurityServices *
use_serv_null()
{
	return (&null_serv);
}

/* LINTLIBRARY */

struct signature *
nullsigned(data, type, module)
	char *data;
	int type;
	modtyp *module;
{
	struct signature *result = (struct signature *) calloc((unsigned) 1, sizeof(*result));

	if (result != (struct signature *) 0) {
		result->alg.algorithm = name2oid("quipuSignatureAlgorithm");
		result->alg.p_type = ALG_PARM_UNKNOWN;
		result->alg.asn = pe_alloc(PE_CLASS_UNIV, PE_FORM_PRIM, PE_PRIM_NULL);
		result->encrypted = NULLCP;
		result->n_bits = 0;
		return (result);
	}
	return (struct signature *) 0;
}

int
nullverify(data, type, module, sig, pubkey, keyparms, hashparms)
	char *data;
	int type;
	modtyp *module;
	struct signature *sig;
	struct GenericPublicKey *pubkey;
	struct GenericParameters *keyparms;
	struct GenericHashParameters *hashparms;
{
	return 0;
}

int
null_ckpath(data, type, module, path, sig, nameptr)
	caddr_t data;
	int type;
	modtyp *module;
	struct certificate_list *path;
	struct signature *sig;
	DN *nameptr;
{
	return (DSE_SC_AUTHENTICATION);
}

struct certificate_list *
null_mkpath()
{
	return (struct certificate_list *) 0;
}

struct encrypted *
nullencrypted(concrete, type, module, publickey, parms)
	char *concrete;
	int type;
	modtyp *module;
	struct GenericPublicKey *publickey;
	struct GenericParameters *parms;
{
	return (struct encrypted *) 0;
}

int
nulldecrypted(enc, type, module, concrete, privkey, parms)
	struct encrypted *enc;
	int type;
	modtyp *module;
	char **concrete;
	struct GenericSecretKey *privkey;
	struct GenericParameters *parms;
{
	return 0;
}

struct Nonce *
nullmknonce(previous)
	struct Nonce *previous;
{
	return ((struct Nonce *) 0);
}

int
nullcknonce(nonce)
	struct Nonce *nonce;
{
	return (DSE_SC_AUTHENTICATION);
}
