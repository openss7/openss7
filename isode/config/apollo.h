/* apollo.h - site configuration file for Apollo */

/*
 * $Header: /xtel/isode/isode/config/RCS/apollo.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 * Contributed by John Brezak, Apollo Computer, Inc.
 *
 *
 * $Log: apollo.h,v $
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

#define VSPRINTF		/* has vprintf(3s) routines */

#define	TCP			/* has TCP/IP (of course) */
#define	SOCKETS			/*   provided by sockets */

#ifdef	notdef			/* Don Preuss at Apollo says no longer needed*/
#ifdef __STDC__                 /* thinks it's ANSI C, but it isn't! */
#undef __STDC__
#endif
#endif

#endif
