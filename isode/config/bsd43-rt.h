/* bsd43-rt.h - site configuration file for RT PC running 4.3BSD UNIX */

/* 
 * $Header: /xtel/isode/isode/config/RCS/bsd43-rt.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 * Contributed by Jacob Rekhter, T.J. Watson Research Center, IBM Corp.
 *
 *
 * $Log: bsd43-rt.h,v $
 * Revision 9.0  1992/06/16  12:08:13  isode
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


#ifndef	_CONFIG_
#define	_CONFIG_

#define	BSD42			/* Berkeley UNIX */
#define	WRITEV			/*   real Berkeley UNIX */
#define	BSD43			/*   4.3BSD or later */

#define	TCP			/* has TCP/IP (of course) */
#define	SOCKETS			/*   provided by sockets */

#define	RT			/* RT/PC */

#if   defined(__STDC__) && defined(__HIGHC__)
				/* hc thinks it's ANSI C, but it isn't! */
#undef	__STDC__
#endif

#endif
