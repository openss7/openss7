/* sys52-exos.h - site configuration file for SVR2 with EXOS 8044 card */

/* 
 * $Header: /xtel/isode/isode/config/RCS/sys52-exos.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 *
 * $Log: sys52-exos.h,v $
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
#define	EXOS			/*   provided by EXOS 8044 card */

#ifdef	sgi			/* screwy compiler! */
#define	register
#endif

#endif
