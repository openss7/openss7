/* aux.h - config file for A/UX running 1.1 */

/* 
 * $Header: /xtel/isode/isode/config/RCS/aux.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 * Contributed by Julian Onions, Nottingham University
 *
 *
 * $Log: aux.h,v $
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
#define AUX			/*   with Apple's enhancements */
#define	WRITEV			/*   that include the writev call */

#define VSPRINTF

#define	TCP			/* has TCP/IP */
#define	SOCKETS			/*   provided by sockets */

#define	NFS			/* network file system -- has getdirentries */

#define	RFINGER		"/usr/ucb/finger"

#endif
