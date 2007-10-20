/* osx.h - site configuration file for Olivetti LSX 30xx */

/* 
 * $Header: /xtel/isode/isode/config/RCS/osx.h,v 9.0 1992/06/16 12:08:13 isode Rel $
 *
 *
 * $Log: osx.h,v $
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


#ifndef _CONFIG_
#define _CONFIG_

#define BSD42                   /* Berkeley UNIX */
#define WRITEV                  /* real Berkeley UNIX (sort of) */
#define XOS_2			/*  Olivetti's version */

#define VSPRINTF

#define TCP                     /* has TCP/IP (of course) */
#define SOCKETS                 /* 4.2BSD sockets */
#define NOGOSIP
#define TSBRIDGE

#define NFS
#endif
