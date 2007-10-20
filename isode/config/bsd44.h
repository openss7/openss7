/* bsd44.h - site configuration file for 4.4BSD UNIX */

/* 
 * $Header: /xtel/isode/isode/config/RCS/bsd44.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 *
 * $Log: bsd44.h,v $
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
#define	BSD44			/*   4.4BSD to be precise! */

#define X25
#define UBC_X25
#define UBC_X25_WRITEV

#define	VSPRINTF		/* has vprintf(3s) routines */

#define	TCP			/* has TCP/IP (of course) */
#define	SOCKETS			/*   provided by sockets */

#define	TP4			/* has TP4 */
#define	BSD_TP4			/*   provided by UCB/UWisc */

#define	GETDENTS		/* has getdirent(2) call */
#define	NFS			/* network file system -- has getdirentries */

#endif
