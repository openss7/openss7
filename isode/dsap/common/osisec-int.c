/* osisec-int.c - minimal OSI Security Package */

#ifndef	lint
static char *rcsid = "$Header: /xtel/isode/isode/dsap/common/RCS/osisec-int.c,v 9.0 1992/06/16 12:12:39 isode Rel $";
#endif

/* 
 * $Header: /xtel/isode/isode/dsap/common/RCS/osisec-int.c,v 9.0 1992/06/16 12:12:39 isode Rel $
 *
 *
 * $Log: osisec-int.c,v $
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

static struct SecurityServices 	null_serv = NULLSECURITYSERVICES;

struct SecurityServices*
use_serv_null()
{
 	return (&null_serv);
}

/* LINTLIBRARY */

struct signature *
nullsigned(data, type, module)
	char           *data;
	int             type;
	modtyp         *module;
{
        struct signature *result = (struct signature *) calloc((unsigned)1, sizeof(*result));
        if(result != (struct signature *)0) {
            result->alg.algorithm = name2oid("quipuSignatureAlgorithm");
            result->alg.p_type = ALG_PARM_UNKNOWN;
            result->alg.asn = pe_alloc(PE_CLASS_UNIV, PE_FORM_PRIM, PE_PRIM_NULL);
            result->encrypted = NULLCP;
	    result->n_bits = 0;
            return (result);
        }
        return (struct signature *)0;
}


int
nullverify(data, type, module, sig, pubkey, keyparms, hashparms)
	char           *data;
	int             type;
	modtyp         *module;
	struct signature *sig;
	struct GenericPublicKey  *pubkey; 	
	struct GenericParameters *keyparms;  	
	struct GenericHashParameters *hashparms; 
{
	return 0;
}

int
null_ckpath(data, type, module, path, sig, nameptr)
	caddr_t         data;
	int             type;
	modtyp         *module;
	struct certificate_list *path;
	struct signature *sig;
	DN             *nameptr;
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
	char           *concrete;
	int             type;
	modtyp         *module;
	struct GenericPublicKey *publickey;
	struct GenericParameters *parms;	
{
	return (struct encrypted *)0;
}


int
nulldecrypted(enc, type, module, concrete, privkey, parms)
	struct encrypted *enc;
	int             type;
	modtyp         *module;
	char          **concrete;
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

