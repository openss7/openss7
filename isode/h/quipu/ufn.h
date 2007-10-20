/* ufn.h - user-friendly naming routines */

/*
 * $Header: /xtel/isode/isode/h/quipu/RCS/ufn.h,v 9.0 1992/06/16 12:23:11 isode Rel $
 *
 *
 * $Log: ufn.h,v $
 * Revision 9.0  1992/06/16  12:23:11  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#ifndef _QUIPUUFN_
#define _QUIPUUFN_

#include "quipu/util.h"
#include "quipu/name.h"
#include "quipu/entry.h"

typedef struct dn_seq * DNS;
#define NULLDNS ((struct dn_seq *) NULL)

typedef struct _envlist {
	DNS	Dns;
	int	Upper;
	int	Lower;
	struct  _envlist * Next;
} * envlist;
#define NULLEL ((envlist) NULL)
envlist	read_envlist ();

extern char ufn_notify;

extern int ufn_flags;
#define	UFN_NULL	0x00
#define	UFN_APPROX	0x01
#define	UFN_WILDHEAD	0x02
#define	UFN_ALL	(UFN_APPROX | UFN_WILDHEAD)

int	ufn_init ();
int	ufn_match ();

#endif
