/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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
 * ReportCSV.c
 * by Kevin Gibbs <kgibbs@nlanr.net>
 *
 * ________________________________________________________________ */

#include "headers.h"
#include "Settings.hpp"
#include "util.h"
#include "Reporter.h"
#include "report_CSV.h"
#include "Locale.h"

void CSV_timestamp( char *timestamp, int length );
 
void CSV_stats( Transfer_Info *stats ) {
    // $TIMESTAMP,$ID,$INTERVAL,$BYTE,$SPEED,$JITTER,$LOSS,$PACKET,$%LOSS
    max_size_t speed = (max_size_t)(((double)stats->TotalLen * 8.0) / (stats->endTime - stats->startTime));
    char timestamp[16];
    CSV_timestamp( timestamp, sizeof(timestamp) );
    if ( stats->mUDP != (char)kMode_Server ) {
        // TCP Reporting
        printf( reportCSV_bw_format, 
                timestamp, 
                (stats->reserved_delay == NULL ? ",,," : stats->reserved_delay),
                stats->transferID, 
                stats->startTime, 
                stats->endTime, 
                stats->TotalLen, 
                speed);
    } else {
        // UDP Reporting
        printf( reportCSV_bw_jitter_loss_format, 
                timestamp, 
                (stats->reserved_delay == NULL ? ",,," : stats->reserved_delay),
                stats->transferID, 
                stats->startTime, 
                stats->endTime, 
                stats->TotalLen, 
                speed,
                stats->jitter*1000.0, 
                stats->cntError, 
                stats->cntDatagrams,
                (100.0 * stats->cntError) / stats->cntDatagrams, stats->cntOutofOrder );
    }
    if ( stats->free == 1 && stats->reserved_delay != NULL ) {
        free( stats->reserved_delay );
    }
}

void *CSV_peer( Connection_Info *stats, int ID ) {
    
    // copy the inet_ntop into temp buffers, to avoid overwriting
    char local_addr[ REPORT_ADDRLEN ];
    char remote_addr[ REPORT_ADDRLEN ];
    char *buf = malloc( REPORT_ADDRLEN*2 + 10 );
    struct sockaddr *local = ((struct sockaddr*)&stats->local);
    struct sockaddr *peer = ((struct sockaddr*)&stats->peer);

    if ( local->sa_family == AF_INET ) {
        inet_ntop( AF_INET, &((struct sockaddr_in*)local)->sin_addr, 
                   local_addr, REPORT_ADDRLEN);
    }
#ifdef HAVE_IPV6
      else {
        inet_ntop( AF_INET6, &((struct sockaddr_in6*)local)->sin6_addr, 
                   local_addr, REPORT_ADDRLEN);
    }
#endif

    if ( peer->sa_family == AF_INET ) {
        inet_ntop( AF_INET, &((struct sockaddr_in*)peer)->sin_addr, 
                   remote_addr, REPORT_ADDRLEN);
    }
#ifdef HAVE_IPV6
      else {
        inet_ntop( AF_INET6, &((struct sockaddr_in6*)peer)->sin6_addr, 
                   remote_addr, REPORT_ADDRLEN);
    }
#endif

    snprintf(buf, REPORT_ADDRLEN*2+10, reportCSV_peer, 
             local_addr, ( local->sa_family == AF_INET ?
                          ntohs(((struct sockaddr_in*)local)->sin_port) :
#ifdef HAVE_IPV6
                          ntohs(((struct sockaddr_in6*)local)->sin6_port)),
#else
                          0),
#endif
            remote_addr, ( peer->sa_family == AF_INET ?
                          ntohs(((struct sockaddr_in*)peer)->sin_port) :
#ifdef HAVE_IPV6
                          ntohs(((struct sockaddr_in6*)peer)->sin6_port)));
#else
                          0));
#endif
    return buf;
}

void CSV_serverstats( Connection_Info *conn, Transfer_Info *stats ) {
    stats->reserved_delay = CSV_peer( conn, stats->transferID );
    stats->free = 1;
    CSV_stats( stats );
}

void CSV_timestamp( char *timestamp, int length ) {
    time_t times;
    struct tm *timest;
    times = time( NULL );
    timest = localtime( &times );
    strftime( timestamp, length,"%Y%m%d%H%M%S", timest );
}
