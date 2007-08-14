/*****************************************************************************

 @(#) $Id: netconfig.h,v 0.9.2.2 2007/08/14 04:56:51 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2007/08/14 04:56:51 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: netconfig.h,v $
 Revision 0.9.2.2  2007/08/14 04:56:51  brian
 - GPLv3 header update

 Revision 0.9.2.1  2006/09/25 12:30:55  brian
 - added files for new strnsl package

 Revision 0.9.2.3  2006/09/24 21:57:21  brian
 - documentation and library updates

 Revision 0.9.2.2  2006/09/22 20:54:24  brian
 - prepared header file for use with doxygen, touching many lines

 Revision 0.9.2.1  2006/09/18 00:03:13  brian
 - added libxnsl source files

 *****************************************************************************/

#ifndef __NETCONFIG_H__
#define __NETCONFIG_H__

#ident "@(#) $RCSfile: netconfig.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @weakgroup nsf
  * @{ */

/** @file src/include/netconfig.h netconfig.h
  * User level network selection facility header file.
  * This file contains declarations for the network selection facility library
  * subroutines, most of which utilize the struct netconfig structure. */

#include <sys/netconfig.h>

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

/** @name Network Selection Facility API Functions
 *  API functions for the network selection facility.
 *  @{ */
/** Bind to and rewind network configuration database;
  * a strong alias of __nsl_setnetconfig(). */
extern void *setnetconfig(void);

/** Get an entry from the network configuration database;
  * a strong alias of __nsl_getnetconfig(). */
extern struct netconfig *getnetconfig(void *handle);

/** Get an entry by network identifier from the network configuration database
  * a strong alias of __nsl_getnetconfigent(). */
extern struct netconfig *getnetconfigent(const char *netid);

/** Free a struct netconfig entry returned by getnetconfig();
  * a strong alias of __nsl_freenetconfigent(). */
extern void freenetconfigent(struct netconfig *netconfig);

/** Release the network configuration database;
  * a strong alias of __nsl_endnetconfig(). */
extern int endnetconfig(void *handle);

/** Bind to and rewind the path filtered Network Selection database;
  * a strong alias of __nsl_setnetpath(). */
extern void *setnetpath(void);

/** Get an entry from the network configuration database;
  * a strong alias of __nsl_getnetpath(). */
extern struct netconfig *getnetpath(void *handle);

/** Release the network configuration database;
  * a strong alias of __nsl_endnetpath(). */
extern int endnetpath(void *handle);

/** Print a network selection function error message;
  * a strong alias of __nsl_nc_perror(). */
extern void nc_perror(const char *msg);

/** Return a network selection function error message;
  * a strong alias of __nsl_nc_sperror(). */
extern char *nc_sperror(void);

/** @} */

/** @name Name Selection Facility API Functions
  * These are the internal implementation of the functions.  The formal functions from
  * <netconfig.h> are strong aliased to these.
  * @{ */
extern void *__nsl_setnetconfig(void);
extern struct netconfig *__nsl_getnetconfig(void *handle);
extern struct netconfig *__nsl_getnetconfigent(const char *netid);
extern void __nsl_freenetconfigent(struct netconfig *netconfig);
extern int __nsl_endnetconfig(void *handle);
extern void *__nsl_setnetpath(void);
extern struct netconfig *__nsl_getnetpath(void *handle);
extern int __nsl_endnetpath(void *handle);
extern void __nsl_nc_perror(const char *msg);
extern char *__nsl_nc_sperror(void);

/** @} */

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

/** @} */

#endif				/* __NETCONFIG_H__ */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
