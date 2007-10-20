/* sys52-sun.h - site configuration file for SUN's SVR2 Compatiblity Package */

/* 
 * $Header: /xtel/isode/isode/config/RCS/sys52-sun.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 *
 * $Log: sys52-sun.h,v $
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

#define	SYS5			/* AT&T UNIX emulation */

#define	VSPRINTF		/* has vprintf(3s) routines */

#define	TCP			/* has TCP/IP */
#define	SOCKETS			/*   provided by sockets */

#define	NFS			/* network filesystem -- has getdirentries() */

#endif
