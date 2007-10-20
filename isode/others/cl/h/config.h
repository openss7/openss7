/* sys52-exos.h - site configuration file for SYS5 release 2 with EXOS 8044 */

/* 
 * $Header: /f/iso/config/RCS/sys52-exos.h,v 5.0 88/07/21 14:31:21 mrose Rel $
 *
 *
 * $Log$
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

#define	TCP			/* has TCP/IP */
#define	EXOS			/* EXOS card */

#ifdef	sgi			/* screwy compiler! */
#define	register
#endif

#define	VSPRINTF		/* has vprintf(3s) routines */

#endif
