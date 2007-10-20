/* sunlink7.h - site configuration file for SunLink OSI and X.25 7.0 on
	SunOS 4 */

/* 
 * $Header: /xtel/isode/isode/config/RCS/sunnet7x.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 *
 * $Log: sunnet7x.h,v $
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
#define	SUNOS4			/*   with Sun's enhancements */
#define	WRITEV			/*   real Berkeley UNIX */
#define	BSD43			/*   4.3BSD or later */

#define	VSPRINTF		/* has vprintf(3s) routines */

#define	TCP			/* has TCP/IP (of course) */
#define	SOCKETS			/*   provided by sockets */

#define	X25			/* has X.25 */
#define	SUN_X25			/*   provided by SunLink X.25 */

#define	GETDENTS		/* has getdirent(2) call */
#define	NFS			/* network filesystem -- has getdirentries() */

#endif
