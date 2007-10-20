/* cheetah.h - site configuration file for cheetah (SunOS) */

/* 
 * $Header: /xtel/isode/isode/config/RCS/cheetah.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 *
 * $Log: cheetah.h,v $
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

#define	VSPRINTF		/* has vprintf(3s) routines */

#define	TCP			/* has TCP/IP (of course) */
#define	SOCKETS			/*   provided by sockets */

#define	NFS			/* network filesystem -- has getdirentries() */

#define	ANON	"anon"		/* guest login for ftam */

#endif
