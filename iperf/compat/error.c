/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

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
 * error.c
 * by Mark Gates <mgates@nlanr.net>
 * -------------------------------------------------------------------
 * error handlers
 * ------------------------------------------------------------------- */

#include "headers.h"
#include "util.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32

/* -------------------------------------------------------------------
 * Implement a simple Win32 strerror function for our purposes.
 * These error values weren't handled by FormatMessage;
 * any particular reason why not??
 * ------------------------------------------------------------------- */

struct mesg {
    DWORD       err;
    const char* str;
};

const struct mesg error_mesgs[] =
{
    { WSAEACCES,             "Permission denied"},
    { WSAEADDRINUSE,         "Address already in use"},
    { WSAEADDRNOTAVAIL,      "Cannot assign requested address"},
    { WSAEAFNOSUPPORT,       "Address family not supported by protocol family"},
    { WSAEALREADY,           "Operation already in progress"},
    { WSAECONNABORTED,       "Software caused connection abort"},
    { WSAECONNREFUSED,       "Connection refused"},
    { WSAECONNRESET,         "Connection reset by peer"},
    { WSAEDESTADDRREQ,       "Destination address required"},
    { WSAEFAULT,             "Bad address"},
    { WSAEHOSTDOWN,          "Host is down"},
    { WSAEHOSTUNREACH,       "No route to host"},
    { WSAEINPROGRESS,        "Operation now in progress"},
    { WSAEINTR,              "Interrupted function call."},
    { WSAEINVAL,             "Invalid argument."},
    { WSAEISCONN,            "Socket is already connected."},
    { WSAEMFILE,             "Too many open files."},
    { WSAEMSGSIZE,           "Message too long"},
    { WSAENETDOWN,           "Network is down"},
    { WSAENETRESET,          "Network dropped connection on reset"},
    { WSAENETUNREACH,        "Network is unreachable"},
    { WSAENOBUFS,            "No buffer space available."},
    { WSAENOPROTOOPT,        "Bad protocol option."},
    { WSAENOTCONN,           "Socket is not connected"},
    { WSAENOTSOCK,           "Socket operation on non-socket."},
    { WSAEOPNOTSUPP,         "Operation not supported"},
    { WSAEPFNOSUPPORT,       "Protocol family not supported"},
    { WSAEPROCLIM,           "Too many processes."},
    { WSAEPROTONOSUPPORT,    "Protocol not supported"},
    { WSAEPROTOTYPE,         "Protocol wrong type for socket"},
    { WSAESHUTDOWN,          "Cannot send after socket shutdown"},
    { WSAESOCKTNOSUPPORT,    "Socket type not supported."},
    { WSAETIMEDOUT,          "Connection timed out."},
    { WSATYPE_NOT_FOUND,     "Class type not found."},
    { WSAEWOULDBLOCK,        "Resource temporarily unavailable"},
    { WSAHOST_NOT_FOUND,     "Host not found."},
    { WSA_INVALID_HANDLE,    "Specified event object handle is invalid."},
    { WSA_INVALID_PARAMETER, "One or more parameters are invalid."},
    { WSA_IO_INCOMPLETE,     "Overlapped I/O event object not in signaled state."},
    { WSA_IO_PENDING,        "Overlapped operations will complete later."},
    { WSA_NOT_ENOUGH_MEMORY, "Insufficient memory available."},
    { WSANOTINITIALISED,     "Successful WSAStartup not yet performed."},
    { WSANO_DATA,            "Valid name, no data record of requested type."},
    { WSANO_RECOVERY,        "This is a non-recoverable error."},
    { WSASYSCALLFAILURE,     "System call failure."},
    { WSASYSNOTREADY,        "Network subsystem is unavailable."},
    { WSATRY_AGAIN,          "Non-authoritative host not found."},
    { WSAVERNOTSUPPORTED,    "WINSOCK.DLL version out of range."},
    { WSAEDISCON,            "Graceful shutdown in progress."},
    { WSA_OPERATION_ABORTED, "Overlapped operation aborted."},
    { 0,                     "No error."}

    /* These appeared in the documentation, but didn't compile.
     * { WSAINVALIDPROCTABLE,   "Invalid procedure table from service provider." },
     * { WSAINVALIDPROVIDER,    "Invalid service provider version number." },
     * { WSAPROVIDERFAILEDINIT, "Unable to initialize a service provider." },
     */

}; /* end error_mesgs[] */

const char* winsock_strerror( DWORD inErrno );

/* -------------------------------------------------------------------
 * winsock_strerror
 *
 * returns a string representing the error code. The error messages
 * were taken from Microsoft's online developer library.
 * ------------------------------------------------------------------- */

const char* winsock_strerror( DWORD inErrno ) {
    const char* str = "Unknown error";
    int i;
    for ( i = 0; i < sizeof(error_mesgs); i++ ) {
        if ( error_mesgs[i].err == inErrno ) {
            str = error_mesgs[i].str;
            break;
        }
    }

    return str;
} /* end winsock_strerror */

#endif /* WIN32 */

/* -------------------------------------------------------------------
 * warn
 *
 * Prints message and return
 * ------------------------------------------------------------------- */

void warn( const char *inMessage, const char *inFile, int inLine ) {
    fflush( 0 );

#ifdef NDEBUG
    fprintf( stderr, "%s failed\n", inMessage );
#else

    /* while debugging output file/line number also */
    fprintf( stderr, "%s failed (%s:%d)\n", inMessage, inFile, inLine );
#endif
} /* end warn */

/* -------------------------------------------------------------------
 * warn_errno
 *
 * Prints message and errno message, and return.
 * ------------------------------------------------------------------- */

void warn_errno( const char *inMessage, const char *inFile, int inLine ) {
    int my_err;
    const char* my_str;

    /* get platform's errno and error message */
#ifdef WIN32
    my_err = WSAGetLastError();
    my_str = winsock_strerror( my_err );
#else
    my_err = errno;
    my_str = strerror( my_err );
#endif

    fflush( 0 );

#ifdef NDEBUG
    fprintf( stderr, "%s failed: %s\n", inMessage, my_str );
#else

    /* while debugging output file/line number and errno value also */
    fprintf( stderr, "%s failed (%s:%d): %s (%d)\n",
             inMessage, inFile, inLine, my_str, my_err );
#endif
} /* end warn_errno */

#ifdef __cplusplus
} /* end extern "C" */
#endif
