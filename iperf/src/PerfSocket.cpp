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
 * PerfSocket.cpp
 * by Mark Gates <mgates@nlanr.net>
 *    Ajay Tirumala <tirumala@ncsa.uiuc.edu>
 * -------------------------------------------------------------------
 * Has routines the Client and Server classes use in common for
 * performance testing the network.
 * Changes in version 2.0
 *     for extracting data from files
 * -------------------------------------------------------------------
 * headers
 * uses
 *   <stdlib.h>
 *   <stdio.h>
 *   <string.h>
 *
 *   <sys/types.h>
 *   <sys/socket.h>
 *   <unistd.h>
 *
 *   <arpa/inet.h>
 *   <netdb.h>
 *   <netinet/in.h>
 *   <sys/socket.h>
 * ------------------------------------------------------------------- */


#define HEADERS()

#include "headers.h"

#include "PerfSocket.hpp"
#include "Settings.hpp"
#include "Locale.hpp"
#include "util.h"

bool PerfSocket::sInterupted = false;
Mutex PerfSocket::sReporting;

/* -------------------------------------------------------------------
 * Initialize the count of how many report lines we've output.
 * ------------------------------------------------------------------- */

int PerfSocket::sReportCount = 0;

/* -------------------------------------------------------------------
 * Store socket info.
 * Allocate buffer used for sending and receiving.
 * If input for the stream is from a file,
 * create the Extractor class for getting data from the file
 * ------------------------------------------------------------------- */

PerfSocket::PerfSocket( unsigned short inPort,
                        bool inUDP )
: Socket( inPort, inUDP ) {
    // initialize buffer
    mBufLen = gSettings->GetBufferLen();
    mBuf = new char[ mBufLen ];
    pattern( mBuf, mBufLen );
    sReportCount = 0;
    if ( gSettings->GetFileInput() ) {
        if ( !gSettings->GetStdin() )
            extractor = new Extractor(gSettings->GetFileName(),mBufLen);
        else
            extractor = new Extractor(stdin,mBufLen);

        if ( !extractor->canRead() ) {
            gSettings->SetFileInput(false);
        }
    }
}
// end PerfSocket

/* -------------------------------------------------------------------
 * Delete memory (buffer used for sending and receiving).
 * ------------------------------------------------------------------- */

PerfSocket::~PerfSocket() {
    DELETE_PTR( mBuf );
    if ( gSettings->GetFileInput() )
        DELETE_PTR( extractor );
}
// end ~PerfSocket

/* -------------------------------------------------------------------
 * Initialization done before any transfer.
 * ------------------------------------------------------------------- */

void PerfSocket::InitTransfer( void ) {
    assert( mSock >= 0   );
    assert( mBuf != NULL );
    assert( mBufLen > 0  );

    ReportPeer( mSock );

    // cummulative bytes written
    mTotalLen = 0;

    // UDP jitter calculations
    mJitter = 0.0;

    // for periodic reports of bandwidth and lost datagrams
    double interval = gSettings->GetInterval();
    mPInterval.set( interval );
    mPReporting      = (interval > 0.0);
    mPLastErrorcnt   = 0;
    mPLastDatagramID = 0;
    mPLastTotalLen   = 0;

    // start timers
    mStartTime.setnow();

    mPLastTime = mStartTime;
    mPNextTime = mStartTime;
    mPNextTime.add( mPInterval );

    // setup termination variables
    mMode_time = (gSettings->GetTerminationMode() == kMode_Time);
    if ( mMode_time ) {
        mEndTime = mStartTime;
        mEndTime.add( gSettings->GetTime());
    } else {
        mAmount = gSettings->GetAmount();
    }
}
// end InitTransfer

/* -------------------------------------------------------------------
 * Periodically report the bandwidth.
 * ------------------------------------------------------------------- */

void PerfSocket::ReportPeriodicBW( void ) {
    if ( mPReporting  &&
         mPacketTime.after( mPNextTime ) ) {

        double inStart = mPLastTime.subSec( mStartTime );
        double inStop = mPacketTime.subSec( mStartTime );

        if ( (long) mPacketTime.subUsec(mPLastTime ) < 500000 ) {
            printf(report_interval_small,mSock,inStart,inStop);
            fflush(stdout);
            mPNextTime.add(mPInterval);
            return;
        }

        ReportBW( mTotalLen - mPLastTotalLen,
                  inStart,inStop);

        mPLastTime = mPacketTime;
        mPNextTime.add( mPInterval );

        mPLastTotalLen   = mTotalLen;
    }
}

/* -------------------------------------------------------------------
 * Periodically report the bandwidth, jitter, and loss.
 * Used by the UDP server only.
 * ------------------------------------------------------------------- */

void PerfSocket::ReportPeriodicBW_Jitter_Loss( int32_t errorCnt,
                                               int32_t outofOrder,
                                               int32_t datagramID ) {
    if ( mPReporting  &&
         mPacketTime.after( mPNextTime ) ) {

        double inStart = mPLastTime.subSec( mStartTime );
        double inStop = mPacketTime.subSec( mStartTime );

        if ( (long) mPacketTime.subUsec(mPLastTime ) < 500000 ) {
            printf(report_interval_small,mSock,inStart,inStop);
            fflush(stdout);
            return;
        }

        ReportBW_Jitter_Loss( mTotalLen - mPLastTotalLen,
                              inStart,
                              inStop,
                              errorCnt - mPLastErrorcnt,
                              outofOrder - mPLastOutofOrder,
                              datagramID - mPLastDatagramID );

        mPLastTime = mPacketTime;
        mPNextTime.add( mPInterval );

        mPLastTotalLen   = mTotalLen;
        mPLastDatagramID = datagramID;
        mPLastErrorcnt   = errorCnt;
        mPLastOutofOrder = outofOrder;
    }
}

/* -------------------------------------------------------------------
 * Report the bandwidth (inBytes / inSeconds).
 * ------------------------------------------------------------------- */

void PerfSocket::ReportBW( max_size_t inBytes,
                           double inStart,
                           double inStop ) {
    sReporting.Lock();

    // print a field header every 20 lines
    if ( --sReportCount <= 0 ) {
        printf( report_bw_header );
        sReportCount = 20;
    }

    char bytes[ 32 ];
    char speed[ 32 ];

    byte_snprintf( bytes, sizeof(bytes), inBytes,
                   toupper( gSettings->GetFormat()));
    byte_snprintf( speed, sizeof(speed),
                   inBytes / (inStop - inStart), gSettings->GetFormat());

    printf( report_bw_format,
            mSock, inStart, inStop, bytes, speed );
    fflush( stdout );

    sReporting.Unlock();
}
// end ReportBW

/* -------------------------------------------------------------------
 * Report the bandwidth (inBytes / inSeconds).
 * ------------------------------------------------------------------- */

void PerfSocket::ReportBW_Jitter_Loss( max_size_t inBytes,
                                       double inStart,
                                       double inStop,
                                       int32_t inErrorcnt,
                                       int32_t inOutofOrder,
                                       int32_t inDatagrams ) {
    sReporting.Lock();

    // print a field header every 20 lines
    if ( --sReportCount <= 0 ) {
        printf( report_bw_jitter_loss_header );
        sReportCount = 20;
    }

    assert( inErrorcnt >= 0 );
    assert( inDatagrams >= 0 );

    char bytes[ 32 ];
    char speed[ 32 ];

    byte_snprintf( bytes, sizeof(bytes), inBytes,
                   toupper( gSettings->GetFormat()));
    byte_snprintf( speed, sizeof(speed),
                   inBytes / (inStop - inStart), gSettings->GetFormat());

    // assume most of the time out-of-order packets are not
    // duplicate packets, so subtract them from the lost packets.
    inErrorcnt -= inOutofOrder;
    printf( report_bw_jitter_loss_format,
            mSock, inStart, inStop, bytes, speed,
            mJitter*1000.0, inErrorcnt, inDatagrams,
            (100.0 * inErrorcnt) / inDatagrams );
    if ( inOutofOrder > 0 ) {
        printf( report_outoforder,
                mSock, inStart, inStop, inOutofOrder );
    }
    fflush( stdout );

    sReporting.Unlock();
}
// end ReportBW_Jitter_Loss

/* -------------------------------------------------------------------
 * Report a socket's peer IP address.
 * ------------------------------------------------------------------- */

void PerfSocket::ReportPeer( int inSock ) {
    sReporting.Lock();

    assert( inSock >= 0 );

    SocketAddr local  = getLocalAddress();
    SocketAddr remote = getRemoteAddress();

    // copy the inet_ntoa into temp buffers, to avoid overwriting
    char local_addr[ REPORT_ADDRLEN ];
    local.getHostAddress( local_addr, sizeof(local_addr));

    char remote_addr[ REPORT_ADDRLEN ];
    remote.getHostAddress( remote_addr, sizeof(remote_addr));

    // note: see also the RecvUDP code, which has this same printf
    printf( report_peer,
            inSock,
            local_addr,  local.getPort(),
            remote_addr, remote.getPort());
    fflush( stdout );

    sReporting.Unlock();
}
// end ReportPeer

/* -------------------------------------------------------------------
 * Report the MSS and MTU, given the MSS (or a guess thereof)
 * ------------------------------------------------------------------- */

// compare the MSS against the (MTU - 40) to (MTU - 80) bytes.
// 40 byte IP header and somewhat arbitrarily, 40 more bytes of IP options.

inline bool checkMSS_MTU( int inMSS, int inMTU );

inline bool checkMSS_MTU( int inMSS, int inMTU ) {
    return(inMTU-40) >= inMSS  &&  inMSS >= (inMTU-80);
}

void PerfSocket::ReportMSS( int inMSS ) {
    sReporting.Lock();

    if ( inMSS <= 0 ) {
        printf( report_mss_unsupported, mSock );
    } else {
        char* net;
        int mtu = 0;

        if ( checkMSS_MTU( inMSS, 1500 ) ) {
            net = "ethernet";
            mtu = 1500;
        } else if ( checkMSS_MTU( inMSS, 4352 ) ) {
            net = "FDDI";
            mtu = 4352;
        } else if ( checkMSS_MTU( inMSS, 9180 ) ) {
            net = "ATM";
            mtu = 9180;
        } else if ( checkMSS_MTU( inMSS, 65280 ) ) {
            net = "HIPPI";
            mtu = 65280;
        } else if ( checkMSS_MTU( inMSS, 576 ) ) {
            net = "minimum";
            mtu = 576;
            printf( warn_no_pathmtu );
        } else {
            mtu = inMSS + 40;
            net = "unknown interface";
        }

        printf( report_mss,
                mSock, inMSS, mtu, net );
    }
    fflush( stdout );

    sReporting.Unlock();
}
// end ReportMSS

/* -------------------------------------------------------------------
 * Report the TCP window size/UDP buffer size and warn if not
 * the same as that requested.
 * ------------------------------------------------------------------- */

void PerfSocket::ReportWindowSize( void ) {
    // sReporting already locked from ReportClient/ServerSettings

    int win = get_tcp_windowsize( mSock );
    int win_requested = gSettings->GetTCPWindowSize();

    char window[ 32 ];
    byte_snprintf( window, sizeof(window), win,
                   toupper( gSettings->GetFormat()));
    printf( "%s: %s", (mUDP ? udp_buffer_size : tcp_window_size), window );

    if ( win_requested == 0 ) {
        printf( " %s", window_default );
    } else if ( win != win_requested ) {
        char request[ 32 ];
        byte_snprintf( request, sizeof(request), win_requested,
                       toupper( gSettings->GetFormat()));
        printf( warn_window_requested, request );
    }
    printf( "\n" );
    fflush( stdout );
}
// end ReportWindowSize

/* -------------------------------------------------------------------
 * Report all the client settings: port, UDP/TCP, local and remote
 * addresses and ports, multicast TTL, TCP window size.
 * ------------------------------------------------------------------- */

void PerfSocket::ReportClientSettings( const char* inHost,
                                       const char* inLocalhost ) {
    sReporting.Lock();

    // print settings
    printf( seperator_line );
    printf( client_port, inHost, (mUDP ? "UDP" : "TCP"), mPort );

    if ( inLocalhost != NULL ) {
        SocketAddr local = getLocalAddress();
        char addr[ REPORT_ADDRLEN ];
        local.getHostAddress( addr, sizeof(addr));
        printf( bind_address, addr );
    }

    if ( mUDP ) {
        printf( client_datagram_size, mBufLen );

        SocketAddr remote = getRemoteAddress();
        if ( remote.isMulticast() ) {
            printf( multicast_ttl, gSettings->GetMcastTTL());
        }
    }

    ReportWindowSize();
    printf( seperator_line );
    fflush( stdout );

    sReporting.Unlock();
}

/* -------------------------------------------------------------------
 * Report all the server settings (prior to accept): port, UDP/TCP,
 * local address, multicast group, TCP window size.
 * ------------------------------------------------------------------- */

void PerfSocket::ReportServerSettings( const char* inLocalhost ) {
    sReporting.Lock();

    // print settings
    printf( seperator_line );
    printf( server_port, (mUDP ? "UDP" : "TCP"), mPort );

    if ( inLocalhost != NULL ) {
        SocketAddr local = getLocalAddress();
        char addr[ REPORT_ADDRLEN ];
        local.getHostAddress( addr, sizeof(addr));

        printf( bind_address, addr );

        if ( local.isMulticast() ) {
            printf( join_multicast, addr );
        }
    }
    if ( mUDP ) {
        printf( server_datagram_size, mBufLen );
    }
    ReportWindowSize();
    printf( seperator_line );
    fflush( stdout );

    sReporting.Unlock();
}

/* -------------------------------------------------------------------
 * Signal handler sets the sInterupted flag, so the object can
 * respond appropriately.. [static]
 * ------------------------------------------------------------------- */

void PerfSocket::Sig_Interupt( int inSigno ) {
    sInterupted = true;
}

/* -------------------------------------------------------------------
 * Set socket options before the listen() or connect() calls.
 * These are optional performance tuning factors.
 * TODO should probably reporting setting these.
 * ------------------------------------------------------------------- */

void PerfSocket::SetSocketOptions( void ) {
  // set the TCP window size (socket buffer sizes)
  // also the UDP buffer size
  // must occur before call to accept() for large window sizes
  set_tcp_windowsize( mSock, gSettings->GetTCPWindowSize() );

#ifdef IP_TOS

    // set IP TOS (type-of-service) field
    if ( gSettings->GetTOS() ) {
        int  tos = gSettings->GetTOS();
        Socklen_t len = sizeof(tos);
        int rc = setsockopt( mSock, IPPROTO_IP, IP_TOS,
                             (char*) &tos, len );
        FAIL_errno( rc == SOCKET_ERROR, "setsockopt IP_TOS" );
    }
#endif

    if ( ! mUDP ) {
        // set the TCP maximum segment size
        setsock_tcp_mss( mSock, gSettings->GetTCP_MSS() );

#ifdef TCP_NODELAY

        // set TCP nodelay option
        if ( gSettings->GetTCP_Nodelay() ) {
            int nodelay = 1;
            Socklen_t len = sizeof(nodelay);
            int rc = setsockopt( mSock, IPPROTO_TCP, TCP_NODELAY,
                                 (char*) &nodelay, len );
            FAIL_errno( rc == SOCKET_ERROR, "setsockopt TCP_NODELAY" );
        }
#endif
    }
}
// end SetSocketOptions
