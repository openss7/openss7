/* sys52-rt.h - site configuration file for RT PC running AIX */

/*
 * $Header: /xtel/isode/isode/config/RCS/sys52-rt.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 * Contributed by by Jacob Rekhter, T.J. Watson Research Center, IBM Corp.
 *
 *
 * $Log: sys52-rt.h,v $
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
#define	AIX			/*   with IBMs running AIX */
#define	RT			/*   on the RT/PC */

#define	VSPRINTF		/* has vprintf(3s) routines */

#define	TCP			/* has TCP/IP */
#define	SOCKETS			/*   provided by sockets */

#endif
