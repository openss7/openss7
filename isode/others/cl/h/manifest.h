/* manifest.h - manifest constants */

/* 
 * $Header: /f/iso/h/RCS/manifest.h,v 5.0 88/07/21 14:39:08 mrose Rel $
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


#ifndef	_MANIFEST_
#define	_MANIFEST_

#ifndef	_CONFIG_
#include "config.h"		/* system-specific configuration */
#endif


/* target-dependent defines:

	SYS5NLY -	target has SYS5 types only, no BSD types

	BSDSIGS	-	target supports BSD signals

 */

#ifdef	BSD42
#undef	SYS5NLY
#define	BSDSIGS
#endif 


#ifdef	ROS
#undef	SYS5NLY
#define	BSDSIGS

#ifndef	BSD42
#define	BSD42
#endif

#endif


#ifdef	SYS5
#define	SYS5NLY
#undef	BSDSIGS

#ifdef	WIN
#undef	SYS5NLY
#endif

#ifdef	sun
#undef	SYS5NLY
#endif

#ifdef	sgi
#undef	SYS5NLY
#undef	BSDSIGS
#endif

#ifdef	HPUX
#undef	SYS5NLY
#define	BSDSIGS
#endif

#ifdef	AIX
#undef	SYS5NLY
#define	BSDSIGS
#define	SIGEMT	SIGUSR1


int   (*_signal ()) ();

#define	signal	_signal
#endif

#endif


#ifdef	NSIG

#ifndef	sigmask
#define	sigmask(s)	(1 << ((s) - 1))
#endif

#ifdef	SIGPOLL
#define	_SIGIO	SIGPOLL
#else
#ifdef	SIGUSR1
#define	_SIGIO	SIGUSR1
#else
#define	_SIGIO	SIGEMT
#endif
#endif

/* HULA removed #if  defined(BSDSIGS) || !defined(SIGPOLL) */
/* HULA inserted following line instead */
#if	defined(BSDSIGS) 
typedef	int	SBV;
#define	sigioblock()	sigblock (sigmask (_SIGIO))
#define	sigiomask(s)	sigsetmask (s)
#else
#define	SBV	SFP
#define	sigioblock()	sigset (_SIGIO, SIG_HOLD)
#define	sigiomask(s)	sigset (_SIGIO, s)

#define	sigpause(s)	pause ()
#endif

#endif

/*    TYPES */

#ifndef	NOTOK
#define	NOTOK		(-1)
#define	OK		0
#define	DONE		1
#endif


#ifndef	NULLCP
typedef char *CP;
#define	NULLCP		((char *) 0)
#define	NULLVP		((char **) 0)
#endif


#ifndef	makedev
#include <sys/types.h>
typedef struct fd_set { int fds_bits[1]; } fd_set;     /* billy, for HULA */ 
#if	defined(S5R3) && defined(WIN)
#include "sys/inet.h"
#ifndef	NFDBITS
typedef struct fd_set { int fds_bits[1]; } fd_set;
#endif
#endif

#ifdef	SYS5NLY
typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned int	u_int;
typedef unsigned long	u_long;
#endif
#endif


#ifndef FD_SET
#define	FD_SETSIZE	    (sizeof (fd_set) * 8)

#define FD_SET(f,s)	    ((s)->fds_bits[0] |= (1 << (f)))
#define FD_CLR(f,s)	    ((s)->fds_bits[0] &= ~(1 << (f)))
#define FD_ISSET(f,s)	    ((s)->fds_bits[0] & (1 << (f)))
#define FD_ZERO(s)	    ((s)->fds_bits[0] = 0)
#endif

#define	NULLFD		((fd_set *) 0)


typedef int	*IP;
#define	NULLIP		((IP) 0)


typedef	int	(*IFP) ();
#define	NULLIFP		((IFP) 0)


typedef void   (*VFP) ();
#define	NULLVFP		((VFP) 0)


#ifndef	S5R3
#define	SFP	IFP
#else
#define	SFP	VFP
#endif


struct udvec {			/* looks like a BSD iovec... */
    caddr_t uv_base;
    int	    uv_len;

    int	    uv_inline;
};


struct qbuf {
    struct qbuf *qb_forw;	/* doubly-linked list */
    struct qbuf *qb_back;	/*   .. */

    int	    qb_len;		/* length of data */
    char   *qb_data;		/* current pointer into data */
    char    qb_base[1];		/* extensible... */
};

#define	QBFREE(qb) \
{ \
    register struct qbuf *QB, \
			 *QP; \
 \
    for (QB = (qb) -> qb_forw; QB != (qb); QB = QP) { \
	QP = QB -> qb_forw; \
	remque (QB); \
	free ((char *) QB); \
    } \
}


#ifndef	min
#define	min(a, b)	((a) < (b) ? (a) : (b))
#define	max(a, b)	((a) > (b) ? (a) : (b))
#endif


#ifdef SYS5

#ifndef	WIN
#ifndef	sun
#define	getdtablesize()	_NFILE
#endif
#else
#define	getdtablesize()	(_NFILE - 1)
#endif

#ifdef	RT
#define	ntohs(x)	(x)
#define	htons(x)	(x)
#define	ntohl(x)	(x)
#define	htonl(x)	(x)
#endif

#endif

#endif
