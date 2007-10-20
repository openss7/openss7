/* ultrix.h - site configuration file for Ultrix version greater than 3.1 */

/* 
 * $Header: /xtel/isode/isode/config/RCS/ultrix.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 *
 * $Log: ultrix.h,v $
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
#define BSD43			/* 4.3BSD networking or later */

#define	VSPRINTF		/* has vprintf(3s) routines */

#define	TCP			/* has TCP/IP (of course) */
#define	SOCKETS			/*   provided by sockets */
#define BIND			/* has h_addr_list in netdb.h */

#define	GETDENTS		/* has getdirent(2) call */

#endif
