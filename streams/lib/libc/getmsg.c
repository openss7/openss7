/*****************************************************************************

 @(#) $RCSfile: getmsg.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2005/10/03 04:21:59 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2005/10/03 04:21:59 $ by $Author: brian $

 *****************************************************************************/

#ident "@(#) $RCSfile: getmsg.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2005/10/03 04:21:59 $"

static char const ident[] =
    "$RCSfile: getmsg.c,v $ $Name:  $($Revision: 0.9.2.7 $) $Date: 2005/10/03 04:21:59 $";

#define _XOPEN_SOURCE 600
#define _REENTRANT
#define _THREAD_SAFE

#include <sys/types.h>
#include <stropts.h>
#include <unistd.h>

/**
 * @ingroup libLiS
 * @brief get a message from a STREAM.
 * @param fd a file descriptor for the stream.
 * @param ctlptr a pointer to a struct strbuf structure that returns the
 * control part of the retrieved message.
 * @param datptr a pointer to a struct strbuf structuer that returns the data
 * part of the retrieved message.
 * @param flagsp a pointer to an integer flags word that returns the priority
 * of the retrieved message.
 *
 * getmsg() must contain a thread cancellation point (SUS/XOPEN/POSIX).
 * Because getmsg consists of a single call to getpmsg() which has the same
 * characteristics, no protection against asynchronous thread cancellation is
 * required.
 */
int
getmsg(int fd, struct strbuf *ctlptr, struct strbuf *datptr, int *flagsp)
{
	int band = -1;

	return getpmsg(fd, ctlptr, datptr, &band, flagsp);
}
