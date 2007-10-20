/* general.h - general compatibility */

/* 
 * $Header: /xtel/isode/isode/h/RCS/general.h,v 9.0 1992/06/16 12:17:57 isode Rel $
 *
 *
 * $Log: general.h,v $
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

#ifdef	SOCKETS
#define	SWABLIB
#endif

#ifdef	WINTLI
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

#ifdef  SVR4_UCB
#ifndef SVR4
#define SVR4
#endif
#endif

#ifdef  SVR4
#ifndef SVR3
#define SVR3
#endif
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
#define	BSDFORK
#undef	BSDLIBC
#undef	BSDSTRS
#undef	SWABLIB
#endif

#ifdef	pyr
#undef	SWABLIB
#endif

#ifdef	XOS
#undef	SWABLIB
#endif

#ifdef	XOS_2
#undef	SWABLIB
#endif

#ifdef  apollo
#undef  SWABLIB
#endif

#ifdef	AUX
#undef	BSDFORK
#define BSDSTRS
#undef SWABLIB
#define BSDLIBC
#endif 

#if defined(_AIX) && defined(SYS5)
#define BSDSTRS
#endif

#ifndef	BSDFORK
#define	vfork	fork
#endif

/*    STRINGS */

#if defined(SVR4) || defined (__NeXT__)
#include <stdio.h>
#include <stdlib.h>

#else /* SVR4 */

#if	defined(BSDSTRS) && !defined(BSD44) && (!defined(BSD43) || defined(SUNOS4) || defined(vax) || defined(RT) || (defined(mips) && defined(ultrix))) && !defined(XOS_2)
#if !(defined(__STDC__) && defined(__GNUC__) && defined(mips) && defined(ultrix))
char   *sprintf ();
#endif
#else
int     sprintf ();
#endif

char   *getenv ();
char   *mktemp ();

#endif /* SVR4 */

#ifdef __STDC__
#ifndef __NeXT__
#include <malloc.h>
#endif
#else
#if defined(BSD44) || defined(_AIX)
void   *calloc (), *malloc (), *realloc ();
void free();
#else
char   *calloc (), *malloc (), *realloc ();
int free();
#endif /* defined BSD44 */
#endif


#ifndef	BSDSTRS
#define	index	strchr
#define	rindex	strrchr
#include <string.h>
#else
#include <strings.h>
#endif

#if	defined(SYS5) && !defined(_AIX) && !defined(XOS) && !defined(XOS_2)
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


#define	str2vec(s,v)	str2vecX ((s), (v), 0, NULLIP, NULL, 1)

int	str2vecX ();

/*    STR2ELEM */

#define	NELEM	100

int	str2elem ();

/*    STR2SEL */

int	str2sel ();
char   *sel2str ();

/*    GETPASS */

char   *getpassword ();

/*    BADUSER */

int	baduser ();

/*   UTILITIES */

extern char chrcnv[], nochrcnv[];


int	lexequ (), lexnequ ();

int	log_tai ();

int	sstr2arg ();

void	(*set_smalloc_handler()) ();
char    *smalloc (), *strdup ();

/*    MISC */

char   *sys_errname ();

#ifdef	lint
#define	insque(e,p)	INSQUE ((char *) (e), (char *) (p))
#define	remque(e)	REMQUE ((char *) (e))
#endif


void	asprintf (), _asprintf ();

void	isodetailor ();		/* also in tailor.h */


/*  time */

#ifndef makedev
#include <sys/types.h>
#endif
#ifndef	OSX
#ifndef	BSD42
#ifdef _AIX
#include <sys/time.h>		/* Why is AIX always different ! */
#endif
#include <time.h>
#else  /* BSD42 */
#ifndef	timerisset
#include <sys/time.h>
#endif /* timerisset */
#endif /* BSD42 */
#endif /* OSX */

#ifndef __STDC__
extern time_t time ();
#endif


/*  ntohs etc */

#ifndef	ntohs
unsigned short	ntohs ();
#endif
#ifndef	htons
unsigned short	htons ();
#endif
#ifndef	ntohl
unsigned long	ntohl ();
#endif
#ifndef	htonl
unsigned long	htonl ();
#endif

int	char2bcd ();
int	bcd2char ();

#endif
