/* osisec-stub.h - minimal OSI Security Package Interface Definitions */

/* 
 * $Header: /xtel/isode/isode/h/RCS/osisec-stub.h,v 9.0 1992/06/16 12:17:57 isode Rel $
 *
 *
 * $Log: osisec-stub.h,v $
 * Revision 9.0  1992/06/16  12:17:57  isode
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

#ifndef SECURITYSERVICES
#define SECURITYSERVICES

/* x509-concrete.h -  concrete structures for X.509 protocol elements */

#ifndef _X509_CONCRETE_
#define _X509_CONCRETE_

#ifdef OSISEC
#include "osisec/alg_rsa.h"
#include "osisec/alg_md2.h"
#include "osisec/alg_md4.h"
#endif

#include "quipu/authen.h"

/* NEW for OSISEC encryption service */
struct encrypted {
	struct alg_id alg;
	int n_bits;
	char *encrypted;
};

/* NEW for OSISEC bind token service */
struct Nonce {
        char *non_time1;
        char *non_time2;
        struct random_number non_r1;
        struct random_number non_r2;
        struct alg_id non_alg;
};


#endif

#ifndef GENERICALGTYPES
#define GENERICALGTYPES

struct GenericPublicKey;
struct GenericSecretKey;
struct GenericParameters;

struct GenericHash;
struct GenericHashParameters;
struct GenericHashKey;
#endif


struct SecurityServices {
	char           			*serv_name;
	struct signature *	 	(*serv_sign)();
	int				(*serv_verify)();
	int				(*serv_ckpath)();
	int				(*serv_ckfpath)();
	struct certificate_list *	(*serv_mkpath)();
	struct encrypted * 		(*serv_encrypt)();
	int				(*serv_decrypt)();
	struct Nonce *			(*serv_mknonce)();
	int				(*serv_cknonce)();
};

extern struct signature        *nullsigned();
extern int      		nullverify();
extern struct certificate_list *null_mkpath();
extern int			null_ckpath();
extern struct encrypted	       *nullencrypted(); 
extern int      		nulldecrypted();
extern struct Nonce *		nullmknonce();
extern int			nullcknonce();

#define NULLSECURITYSERVICES { \
	"quipusecurityservices", \
	nullsigned, \
	nullverify, \
	null_ckpath, \
	null_ckpath, \
	null_mkpath, \
	nullencrypted, \
	nulldecrypted, \
	nullmknonce, \
	nullcknonce \
}

extern struct SecurityServices	*use_serv_null();
#define SECSERV sec_serv
extern struct SecurityServices *SECSERV;

#define SIGNED_MACRO(serv,a,b,c)	(serv->serv_sign)((a),(b),(c))
#define VERIFY_MACRO(serv,a,b,c,d,e,f,g) (serv->serv_verify)((a),(b),(c),(d),(e),(f),(g))

#define CHECKPATH(serv,a,b,c,d,e,f) 	(serv->serv_ckpath)((a),(b),(c),(d),(e),(f))
#define CHECKFPATH(serv,a,b,c,d,e,f) 	(serv->serv_ckfpath)((a),(b),(c),(d),(e),(f))
#define MAKEPATH(serv) 			(serv->serv_mkpath)()

#define ENCRYPTED_MACRO(serv,a,b,c,d,e)	(serv->serv_encrypt)((a),(b),(c),(d),(e))
#define DECRYPTED_MACRO(serv,a,b,c,d,e,f) (serv->serv_decrypt)((a),(b),(c),(d),(e),(f))

#define MAKE_NONCE(serv,a)	(serv->serv_mknonce)((a))
#define CHECK_NONCE(serv,a) (serv->serv_cknonce)((a))
#endif
