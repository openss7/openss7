/* hpux.h - site configuration file for HP-UX */

/*
 * $Header: /xtel/isode/isode/config/RCS/hpux.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 *
 * $Log: hpux.h,v $
 * Revision 9.0  1992/06/16  12:08:13  isode
 * Release 8.0
 *
 */

/*
 *                                NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#ifndef _CONFIG_
#define _CONFIG_

#define SYS5                    /* AT&T UNIX */
#define HPUX                    /*   with HP's enhancements */
#define VSPRINTF                /* libc includes vsprintf and vfprintf */

#define TCP                     /* has TCP/IP */
#define SOCKETS                 /*   provided by sockets */

#define	GETDENTS		/* has getdirent(2) call */

#endif
