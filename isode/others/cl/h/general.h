/* general.h - general compatibility */

/* 
 * $Header: /f/iso/h/RCS/general.h,v 5.0 88/07/21 14:38:59 mrose Rel $
 *
 *
 * $Log$
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


#ifndef	_GENERAL_
#define	_GENERAL_

#ifndef	_CONFIG_
#include "config.h"		/* system-specific configuration */
#endif


/* target-dependent defines:

	BSDFORK -	target has BSD vfork
	BSDLIBC -	target has real BSD libc
	BSDSTRS -	target has BSD strings
	SWABLIB -	target has byteorder(3n) routines
 */

#ifdef	NATIVE
#define	SWABLIB
#endif

#ifdef	EXOS
#define	SWABLIB
#endif


#ifdef	BSD42
#define	BSDFORK
#define	BSDLIBC
#define	BSDSTRS
#endif

#ifdef	ROS
#undef	BSDFORK
#undef	BSDLIBC
#define	BSDSTRS
#ifndef	BSD42
#define	BSD42
#endif
#undef	SWABLIB
#endif

#ifdef	SYS5
#undef	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#endif

#ifdef	sgi
#undef	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#endif

#ifdef	HPUX
#undef	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#undef	SWABLIB
#endif

#ifdef	HPUX
#undef	SWABLIB
#endif

#ifdef	pyr
#undef	SWABLIB
#endif


#ifndef	BSDFORK
#define	vfork	fork
#endif

/*    STRINGS */

#ifndef	BSDSTRS
#define	index	strchr
#define	rindex	strrchr
#endif

char   *index ();
char   *mktemp ();
char   *rindex ();
#ifdef	BSDSTRS
char   *sprintf ();
#else
int     sprintf ();
#endif
char   *strcat ();
int     strcmp ();
char   *strcpy ();
int	strlen ();
char   *strncat ();
int     strncmp ();
char   *strncpy ();

char   *getenv ();
char   *calloc (), *malloc (), *realloc ();

#if	defined(SYS5) && !defined(AIX)
#include <memory.h>

#define	bcopy(b1,b2,length)	(void) memcpy ((b2), (b1), (length))
#define	bcmp(b1,b2,length)	memcmp ((b1), (b2), (length))
#define	bzero(b,length)		(void) memset ((b), 0, (length))
#endif

/*    HEXIFY */

int	explode (), implode ();

/*    SPRINTB */

char   *sprintb ();

/*    STR2VEC */

#define	NVEC	100
#define	NSLACK	10


#define	str2vec(s,v)	str2vecX ((s), (v), 0, NULLIP, NULL)

int	str2vecX ();

/*    STR2ELEM */

#define	NELEM	20

int	str2elem ();

/*    STR2SEL */

int	str2sel ();
char   *sel2str ();

/*    MISC */

char   *sys_errname ();

#ifdef	lint
#define	insque(e,p)	INSQUE ((char *) (e), (char *) (p))
#define	remque(e)	REMQUE ((char *) (e))
#endif


void	asprintf (), _asprintf ();

#endif
