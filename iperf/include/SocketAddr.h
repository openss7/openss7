/*****************************************************************************

 @(#) $Id$

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
 * Socket.cpp
 * by Ajay Tirumala <tirumala@ncsa.uiuc.edu> 
 *    and Mark Gates <mgates@nlanr.net>
 * ------------------------------------------------------------------- */


#ifndef SOCKET_ADDR_H
#define SOCKET_ADDR_H

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

#include "headers.h"
#include "Settings.hpp"


#ifdef __cplusplus
extern "C" {
#endif
/* ------------------------------------------------------------------- */
    void SockAddr_localAddr( thread_Settings *inSettings );
    void SockAddr_remoteAddr( thread_Settings *inSettings );

    void SockAddr_setHostname( const char* inHostname,
                               iperf_sockaddr *inSockAddr, 
                               int isIPv6 );          // DNS lookup
    void SockAddr_getHostname( iperf_sockaddr *inSockAddr,
                               char* outHostname, 
                               size_t len );   // reverse DNS lookup
    void SockAddr_getHostAddress( iperf_sockaddr *inSockAddr, 
                                  char* outAddress, 
                                  size_t len ); // dotted decimal

    void SockAddr_setPort( iperf_sockaddr *inSockAddr, unsigned short inPort );
    void SockAddr_setPortAny( iperf_sockaddr *inSockAddr );
    unsigned short SockAddr_getPort( iperf_sockaddr *inSockAddr );

    void SockAddr_setAddressAny( iperf_sockaddr *inSockAddr );

    // return pointer to the struct in_addr
    struct in_addr* SockAddr_get_in_addr( iperf_sockaddr *inSockAddr );
#ifdef HAVE_IPV6
    // return pointer to the struct in_addr
    struct in6_addr* SockAddr_get_in6_addr( iperf_sockaddr *inSockAddr );
#endif
    // return the sizeof the addess structure (struct sockaddr_in)
    Socklen_t SockAddr_get_sizeof_sockaddr( iperf_sockaddr *inSockAddr );

    int SockAddr_isMulticast( iperf_sockaddr *inSockAddr );

    int SockAddr_isIPv6( iperf_sockaddr *inSockAddr );

    int SockAddr_are_Equal( struct sockaddr *first, struct sockaddr *second );
    int SockAddr_Hostare_Equal( struct sockaddr *first, struct sockaddr *second );

    void SockAddr_zeroAddress( iperf_sockaddr *inSockAddr );
#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* SOCKET_ADDR_H */
