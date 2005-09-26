/*****************************************************************************

 @(#) $Id: stropts.h,v 0.9.2.11 2005/09/26 10:08:35 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (C) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (C) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2005/09/26 10:08:35 $ by $Author: brian $

 *****************************************************************************/

#ifndef _STROPTS_H
#define _STROPTS_H
#define _LIS_STROPTS_H

#ident "@(#) $Name:  $($Revision: 0.9.2.11 $) Copyright (c) 1997-2005  Open SS7 Corporation"

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

#include <sys/stropts.h>

#if 0
/* Perform the I/O control operation specified by REQUEST on FD.
   One argument may follow; its presence and type depend on REQUEST.
   Return value depends on REQUEST.  Usually -1 indicates error.  */
extern int ioctl (int __fd, unsigned long int __request, ...) __THROW;
#endif

#include <sys/ioctl.h>

/* Test whether FILDES is associated with a STREAM-based file.  */
extern int isastream (int __fildes) __THROW;

/* Receive next message from a STREAMS file.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
extern int getmsg (int __fildes, struct strbuf *__restrict __ctlptr,
		   struct strbuf *__restrict __dataptr,
		   int *__restrict __flagsp);

/* Receive next message from a STREAMS file, with *FLAGSP allowing to
   control which message.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
extern int getpmsg (int __fildes, struct strbuf *__restrict __ctlptr,
		    struct strbuf *__restrict __dataptr,
		    int *__restrict __bandp, int *__restrict __flagsp);

/* Send a message on a STREAM.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
extern int putmsg (int __fildes, __const struct strbuf *__ctlptr,
		   __const struct strbuf *__dataptr, int __flags);

/* Send a message on a STREAM to the BAND.

   This function is a cancellation point and therefore not marked with
   __THROW.  */
extern int putpmsg (int __fildes, __const struct strbuf *__ctlptr,
		    __const struct strbuf *__dataptr, int __band, int __flags);

/* Attach a STREAMS-based file descriptor FILDES to a file PATH in the
   file system name space.  */
extern int fattach (int __fildes, __const char *__path) __THROW;

/* Detach a name PATH from a STREAMS-based file descriptor.  */
extern int fdetach (__const char *__path) __THROW;

/* Create a one-way communication channel (pipe).
   If successful, two file descriptors are stored in PIPEDES;
   bytes written on PIPEDES[1] can be read from PIPEDES[0].
   Returns 0 if successful, -1 if not.  */
extern int pipe (int __pipedes[2]) __THROW;

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* _STROPTS_H */
