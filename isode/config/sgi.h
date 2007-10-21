/* cognac.h - site configuration file for CoGNAC ISODE distribution */
/* works on SunOS 4.0 and IRIX 4.0 */

/* 
 * $Header: /usr/tmp/isode-8.0/config/RCS/sgi.h,v 1.1 1992/06/26 15:57:36 simon Exp $
 *
 *
 * $Log: sgi.h,v $
 * Revision 1.1  1992/06/26  15:57:36  simon
 * Initial revision
 *
 * Revision 8.0  91/07/17  12:20:40  isode
 * Release 7.0
 * 
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
#ifndef SVR3
#define	SVR3			/*   Systems V Release 3 */
#endif
#define BSD43
#define TERMIOS
#define RFINGER "/usr/bsd/finger"

#define	TCP			/* has TCP/IP (of course) */
#define	SOCKETS			/*   provided by sockets */
#define BIND			/* our libc uses BIND */

#define	GETDENTS		/* has getdirent(2) call */
#define	NFS			/* network filesystem -- has getdirentries */
#define TEMPNAM			/* has tempnam (3) call */
#define	VSPRINTF		/* has vprintf(3s) routines */

#endif
