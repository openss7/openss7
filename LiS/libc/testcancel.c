/*****************************************************************************

 @(#) $RCSfile: testcancel.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/14 10:47:13 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

 Last Modified $Date: 2007/08/14 10:47:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: testcancel.c,v $
 Revision 0.9.2.4  2007/08/14 10:47:13  brian
 - GPLv3 header update

 *****************************************************************************/

#ident "@(#) $RCSfile: testcancel.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/14 10:47:13 $"

static char const ident[] =
    "$RCSfile: testcancel.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2007/08/14 10:47:13 $";

/* This file can be processed with doxygen(1). */

#include "streams.h"

/** @weakgroup strcalls STREAMS System Calls
  * @{ */

/** @file
  * @brief Thread-safety implementation file.
  *
  * This file provide implementation of functions used for thread-safety as well
  * as for non-recursive versions of the library.  This file is shared by a
  * number of libraries. */

/** @page threadsafety Discussion on Thread-Safety
  *
  * This is a lightweight pthreads replacement for the non-recursive
  * environment.  This module defines a number of POSIX thread functions that in
  * the implementations of the second set.  are used by library code.  They are
  * defined as weak aliases.  This file provides replacement dummy functions for
  * POSIX thread functions to be used when there is no need for thread-safety.
  * When the libpthread library is loaded against a library containing this
  * package, the POSIX thread definitions from libpthread will be used instead.
  * Both defined and undefined weak symbols are used so that it does not matter
  * whether the libpthread library is loaded before or after the library
  * containing this package.
  *
  * The magic of weak functions is a black art.  This one works currently, don't
  * mess with it.
  *
  * When linked to this object, a library can pretty much use whatever
  * cancellation mechanisms, read-write locks and thread specific data they like
  * using the normal POSIX thread function calls as defined in in <pthread.h>
  * and not worry about non-recursive versions.
  */

/** @name Asynchronous Thread Cancellation Functions
  * @brief Functions for asynchronous thread cancellation.
  *
  * These functions are declared in <pthread.h>, but are also declared here as
  * verification against the system headers.  These symbols are declared as weak
  * undefined symbols in glibc and provide thread safety capabilities to glibc
  * from an add-on pthread library.  They are used in the OpenSS7 libraries to
  * detect whether a suitable pthread library is present (loaded).
  *
  * The set of symbols: pthread_testcancel() and pthread_setcanceltype(), are
  * weak @e defined symbols that are overriden when libpthread loads after
  * glibc.
  *
  * The proper approach in implementation is to implement the set as weak @e
  * defined symbols that override glibc's symbols and are still overridden by
  * libpthread strong symbols, and to test the presence of the first set in the
  * implementations of the second set.
  *
  * @{ */
#pragma weak __pthread_testcancel
extern void __pthread_testcancel(void);

#pragma weak __pthread_setcanceltype
extern int __pthread_setcanceltype(int type, int *oldtype);

/** @} */

#pragma weak pthread_testcancel
/** @brief Test for asyncrhonous thread cancellation (while deferred).
  * @par Alias:
  * This function is a weak defined symbol.
  *
  * This is a non-recursive replacement for the pthread library function,
  * pthread_testcancel(3). */
void
pthread_testcancel(void)
{
	/** If the weak undefined symbol, __pthread_testcancel() is loaded, a
	  * suitable threads library exists and the library symbol is called.
	  */
	if (__pthread_testcancel)
		return __pthread_testcancel();
	/** Otherwise, for non-threaded operation, this is a noop. */
	return;
}

/** @brief Thread cancellation type storage.
  *
  * This is a statically allocated internal storage for holding the thread
  * cancellation type when no suitable threads library is loaded
  * (non-thread-safe operation).  */
static int __pthread_canceltype = 0;

#pragma weak pthread_setcanceltype
/** @brief Set or defer thread asynchronous cancellation.
  * @param type cancellation type to set.
  * @param oldtype place to return current type.
  * @par Alias:
  * This function is a weak defined symbol.
  *
  * This is a non-recursive replacement for the pthread library function,
  * pthread_setcanceltype(3). */
int
pthread_setcanceltype(int type, int *oldtype)
{
	/** If the weak undefined symbol, __pthread_setcanceltype() is loaded, a suitable threads
	  * library exists, and the library symbol is called.  */
	if (__pthread_setcanceltype)
		return __pthread_setcanceltype(type, oldtype);
	/** Otherwise, for non-thread operation, this function simply saves new
	  * type and returns the oldtype from a static variable.  One of the
	  * ramifications of this is that the cancel type might not be correct
	  * after a fork(2). */
	if (oldtype != NULL)
		*oldtype = __pthread_canceltype;
	__pthread_canceltype = type;
	return (0);
	/** @return Returns zero (0) on success, error number on failure. */
}

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
