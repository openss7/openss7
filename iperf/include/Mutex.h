/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

/*--------------------------------------------------------------- 
 * Copyright (c) 1999,2000,2001,2002,2003                              
 * The Board of Trustees of the University of Illinois            
 * All Rights Reserved.                                           
 *--------------------------------------------------------------- 
 * Permission is hereby granted, free of charge, to any person    
 * obtaining a copy of this software (Iperf) and associated       
 * documentation files (the "Software"), to deal in the Software  
 * without restriction, including without limitation the          
 * rights to use, copy, modify, merge, publish, distribute,        
 * sublicense, and/or sell copies of the Software, and to permit     
 * persons to whom the Software is furnished to do
 * so, subject to the following conditions: 
 *
 *     
 * Redistributions of source code must retain the above 
 * copyright notice, this list of conditions and 
 * the following disclaimers. 
 *
 *     
 * Redistributions in binary form must reproduce the above 
 * copyright notice, this list of conditions and the following 
 * disclaimers in the documentation and/or other materials 
 * provided with the distribution. 
 * 
 *     
 * Neither the names of the University of Illinois, NCSA, 
 * nor the names of its contributors may be used to endorse 
 * or promote products derived from this Software without
 * specific prior written permission. 
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
 * NONINFRINGEMENT. IN NO EVENT SHALL THE CONTIBUTORS OR COPYRIGHT 
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, 
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
 * ________________________________________________________________
 * National Laboratory for Applied Network Research 
 * National Center for Supercomputing Applications 
 * University of Illinois at Urbana-Champaign 
 * http://www.ncsa.uiuc.edu
 * ________________________________________________________________ 
 *
 * Mutex.h
 * by Mark Gates <mgates@nlanr.net>
 * -------------------------------------------------------------------
 * An abstract class for locking a mutex (mutual exclusion). If
 * threads are not available, this does nothing.
 * ------------------------------------------------------------------- */
#ifndef MUTEX_H
#define MUTEX_H

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

#include "headers.h"

#ifdef HAVE_POSIX_THREAD
    typedef pthread_mutex_t Mutex;
#else
#ifdef HAVE_WIN32_THREAD
    typedef HANDLE Mutex;
#else
    typedef int Mutex;
#endif
#endif

/* ------------------------------------------------------------------- *
class Mutex {
public:*/
    
    // initialize mutex
#ifdef HAVE_POSIX_THREAD
    #define Mutex_Initialize( MutexPtr ) pthread_mutex_init( MutexPtr, NULL )
#else
#ifdef HAVE_WIN32_THREAD
    #define Mutex_Initialize( MutexPtr ) *MutexPtr = CreateMutex( NULL, false, NULL )
#else
    #define Mutex_Initialize( MutexPtr )
#endif
#endif
    
    // lock the mutex variable
#ifdef HAVE_POSIX_THREAD
    #define Mutex_Lock( MutexPtr ) pthread_mutex_lock( MutexPtr )
#else
#ifdef HAVE_WIN32_THREAD
    #define Mutex_Lock( MutexPtr ) WaitForSingleObject( *MutexPtr, INFINITE )
#else
    #define Mutex_Lock( MutexPtr )
#endif
#endif

    // unlock the mutex variable
#ifdef HAVE_POSIX_THREAD
    #define Mutex_Unlock( MutexPtr ) pthread_mutex_unlock( MutexPtr )
#else
#ifdef HAVE_WIN32_THREAD
    #define Mutex_Unlock( MutexPtr ) ReleaseMutex( *MutexPtr )
#else
    #define Mutex_Unlock( MutexPtr )
#endif
#endif

    // destroy, making sure mutex is unlocked
#ifdef HAVE_POSIX_THREAD
    #define Mutex_Destroy( MutexPtr )  do {         \
        int rc = pthread_mutex_destroy( MutexPtr ); \
        if ( rc == EBUSY ) {                        \
            Mutex_Unlock( MutexPtr );               \
            pthread_mutex_destroy( MutexPtr );      \
        }                                           \
    } while ( 0 )
#else
#ifdef HAVE_WIN32_THREAD
    #define Mutex_Destroy( MutexPtr ) CloseHandle( *MutexPtr )
#else
    #define Mutex_Destroy( MutexPtr )
#endif
#endif

#endif // MUTEX_H
