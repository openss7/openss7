/* aix.h - site configuration file for IBM AIX 3.2 on RS6000 Workstation */

/* 
 * $Header: /xtel/isode/isode/config/RCS/aix.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 *
 * $Log: aix.h,v $
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

#define	VSPRINTF		/* has vprintf(3s) routines */

#define GETDENTS

#define	TCP			/* has TCP/IP */
#define	SOCKETS			/*   provided by sockets */

/*				If you have OSIMF you can do TP4
				Using the XTI package (alpha test).
				Add -lxti to LSOCKET in CONFIG.make
				Contact bug-isode@xtel.co.uk for latest info.
#define TP4
#define XTI_TP
*/

#define aiws			/* AIX RS6000 workstation ONLY */

#endif

