/* sys52-win.h - site configuration file for SVR2 with WIN/TCP */

/* 
 * $Header: /xtel/isode/isode/config/RCS/sys52-win.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 *
 * $Log: sys52-win.h,v $
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

#define	SYS5			/* AT&T UNIX */

#define	VSPRINTF		/* has vprintf(3s) routines */

#define	TCP			/* has TCP/IP */
#define	SOCKETS			/*   provided by sockets */
#define	WIN			/*   emulated by WIN/TCP for SVR2 */

#endif
