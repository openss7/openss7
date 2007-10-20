/* sys54.h - site configuration file for generic SVR4 */

/* 
 * $Header: /xtel/isode/isode/config/RCS/sys54.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 *
 * $Log: sys54.h,v $
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
#define SVR4			/*   Systems V Release 4 */

/* If you have the UCB extensions ...
#define SVR4_UCB		
*/

#define	VSPRINTF		/* has vprintf(3s) routines */

#define GETDENTS

#define	TCP			/* has TCP/IP */
#define	SOCKETS			/*   provided by sockets */

#endif
