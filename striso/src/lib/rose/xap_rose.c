/*****************************************************************************

 @(#) $RCSfile: xap_rose.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-04-25 08:33:39 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-04-25 08:33:39 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xap_rose.c,v $
 Revision 0.9.2.1  2008-04-25 08:33:39  brian
 - added man pages and skeletons

 *****************************************************************************/

#ident "@(#) $RCSfile: xap_rose.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-04-25 08:33:39 $"

static char const ident[] = "$RCSfile: xap_rose.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-04-25 08:33:39 $";

/* This file can be processed with doxygen(1). */

/** @weakgroup rose OpenSS7 XAP-ROSE Library
  * @{ */

/** @file
  * Openss7 X/Open ACSE/Presentation ROSE (XAP-ROSE) implementation file. */

/**
  * This manual contains documentation of the OpenSS7 XAP-ROSE Library functions
  * that are generated automatically from the source code with doxygen.  This
  * documentation is intended to be used for maintainers of the OpenSS7 XAP-ROSE
  * Library and is not intended for users of the OpenSS7 XAP-ROSE Library.
  * Users should consult the documentation found in the user manual pages
  * beginning with xap-rose(3).
  *
  * <h2>Thread Safety</h2>
  * The OpenSS7 XAP-ROSE Library is designed to be thread-safe.  This is
  * accomplished in a number of ways.  Thread-safety depends on the use of glibc
  * and the pthreads library.
  *
  * Glibc2 provides lightweight thread-specific data use by this library.
  *
  * Glibc2 also provides some weak undefined aliases for POSIX thread functions
  * to perform its own thread-safety.  When the pthread library (libpthread) is
  * linked with glibc2, these functions call libpthread functions instead of
  * internal dummy routines.  The same approach is taken for the OpenSS7
  * XAP-ROSE Library.  The library uses weak defined and undefined aliases that
  * automatically invoke libpthread functions when libpthread is (dynamically)
  * linked and uses dummy functions when it is not.  This maintains maximum
  * efficiency when libpthread is not dynamically linked, but provides full
  * thread safety when it is.
  *
  * Libpthread behaves in some strange ways with regard to thread cancellation.
  * Because libpthread uses Linux clone processes for threads, cancellation of a
  * thread is accomplished by sending a signal to the thread process.  This does
  * not directly result in cancellation, but will result in the failure of a
  * system call with the EINTR error code.  It is ncessary to test for
  * cancellation upon error return from system calls to perform the actual
  * cancellation of the thread.
  *
  * The XAP-ROSE specification (OpenGroup XAP-ROSE 3) lists the following
  * functions as being thread cancellation points:
  *
  * Other XAP-ROSE functions are not permitted by XNS 5.2 to be thread
  * cancellation points.  Any function that cannot be a thread cancellation
  * point needs to have its cancellation status deferred if it internally
  * invokes a function that permits thread cancellation.  Functions that do not
  * permit thread cancellation are:
  *
  * Locks and asyncrhonous thread cancellation present challenges:
  *
  * Functions that act as thread cancellation points must push routines onto the
  * function stack executed at exit of the thread to release the locks held by
  * the function.  These are performed with weak definitions of POSIX thread
  * library functions.
  *
  * Functions that do not act as thread cancellation points must defer thread
  * cancellation before taking locks and then release locks before thread
  * cancellation is restored.
  *
  * The above are the techniques used by glibc2 for the same purpose and is the
  * same technique that is used by the OpenSS7 XAP-ROSE Library.
  */

#include <xap_rose.h>

/** @brief Initialize the ROSE user environment.
  * @param fd		A file descriptor returned from a call to ap_open(3).
  * @param aperrno_p	A poiner to a memory extent into which to return an
  *			error code if the call fails.
  * @version XAP_ROSE_1.0
  * @par Alias:
  * This is an implementation of symbol ap_ro_init().
  */
int
__xro_ap_ro_init(int fd, unsigned long *aperrno_p)
{
	return (0);
}

/** @fn int ap_ro_init(int fd, unsigned long *aperrno_p)
  * @param fd		A file descriptor returned from a call to ap_open(3).
  * @param aperrno_p	A poiner to a memory extent into which to return an
  *			error code if the call fails.
  * @version XAP_ROSE_1.0
  * @par Alias:
  * This symbol is a strong alias of ap_ro_init().
  */
__asm__(".symver __xro_ap_ro_init,ap_ro_init@@XAP_ROSE_1.0");

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
