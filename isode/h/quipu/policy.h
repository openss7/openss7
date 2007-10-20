/* policy.h - representation of security policy */

/* 
 * $Header: /xtel/isode/isode/h/quipu/RCS/policy.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 * $Log: policy.h,v $
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
#ifndef QUIPUPOLICY
#define QUIPUPOLICY
#include "psap.h"

struct security_policy {
        OID oid;
        int p_type;
#define POLICY_PARM_ABSENT      0
#define POLICY_PARM_UNKNOWN     1
#define POLICY_PARM_NUMERIC     2
#define POLICY_PARM_ACCESS      3
        union {
                int numeric;
		unsigned access;
        } un;
};

#define NULLPOLICY ((struct security_policy *) 0)

#define POLICY_ACCESS_DETECT	1
#define POLICY_ACCESS_READ 	2
#define POLICY_ACCESS_ADD	4
#define POLICY_ACCESS_WRITE	8
#define POLICY_ACCESS_ALL	15

#endif
