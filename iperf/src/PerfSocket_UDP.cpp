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
 * &  Ajay Tirumala <tirumala@ncsa.uiuc.edu> 
 * ------------------------------------------------------------------- 
 * Has routines the Client and Server classes use in common for 
 * performance testing the network. 
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
#include "delay.hpp" 
#include "Listener.hpp" 
#include "util.h" 

const double kSecs_to_usecs = 1e6; 
const int    kBytes_to_Bits = 8; 

using namespace std; 

/* ------------------------------------------------------------------- 
 * Send data using the connected UDP socket, 
 * until a termination flag is received. 
 * Does not close the socket. 
 * ------------------------------------------------------------------- */ 

void PerfSocket::Send_UDP( void ) {
    long currLen; 
    int32_t datagramID = 0; 
    struct UDP_datagram* mBuf_UDP = (struct UDP_datagram*) mBuf; 

    int delay_target; 
    int delay = 0; 
    int adjust; 

    // terminate loop nicely on user interupts 
    sInterupted = false; 
    my_signal( SIGINT, Sig_Interupt ); 

    // compute delay for UDP bandwidth restriction, constrained to [0,1] seconds 
    delay_target = (int) (mBufLen * ((kSecs_to_usecs * kBytes_to_Bits) 
                                     / gSettings->GetUDPRate())); 
    if ( delay_target < 0  || 
         delay_target > (int) 1 * kSecs_to_usecs ) {
        printf( warn_delay_large, delay_target / kSecs_to_usecs ); 
        delay_target = (int) kSecs_to_usecs * 1; 
    }

    // check if we're sending multicast, and set TTL 
    // TODO is this the right place to put this??? 
    SocketAddr remote = getRemoteAddress(); 
    if ( remote.isMulticast() ) {
        McastSetTTL( gSettings->GetMcastTTL() ); 
    }

    Timestamp lastPacketTime; 
    InitTransfer(); 
    // File Input option 
    bool fileInput = gSettings->GetFileInput(); 

    // Indicates if the stream is readable 
    bool canRead; 

    // Due to the UDP timestamps etc, included 
    // reduce the read size by an amount 
    // equal to the header size 
    if ( fileInput )
        extractor->reduceReadSize(sizeof(struct UDP_datagram));

    mStartTime.setnow(); 

    do {
        mPacketTime.setnow(); 

        // Test case: drop 17 packets and send 2 out-of-order: 
        // sequence 51, 52, 70, 53, 54, 71, 72 
        //switch( datagramID ) { 
        //  case 53: datagramID = 70; break; 
        //  case 71: datagramID = 53; break; 
        //  case 55: datagramID = 71; break; 
        //  default: break; 
        //} 

        // store datagram ID into buffer 
        mBuf_UDP->id      = htonl( datagramID++ ); 
        mBuf_UDP->tv_sec  = htonl( mPacketTime.getSecs()); 
        mBuf_UDP->tv_usec = htonl( mPacketTime.getUsecs()); 

        // Read the next data block from 
        // the file if it's file input 
        if ( fileInput ) {
            extractor->getNextDataBlock(mBuf + sizeof(struct UDP_datagram)); 
            canRead = extractor->canRead(); 
        } else
            canRead = true; 

        // perform write 
        currLen = write( mSock, mBuf, mBufLen ); 
        if ( currLen < 0 ) {
            WARN_errno( currLen < 0, "write" ); 
            break; 
        }
        mTotalLen += currLen; 

        // delay between writes 
        // make an adjustment for how long the last loop iteration took 
        // TODO this doesn't work well in certain cases, like 2 parallel streams 
        adjust = delay_target - mPacketTime.subUsec( lastPacketTime ); 
        lastPacketTime = mPacketTime; 

        if ( adjust > 0  ||  delay > 0 ) {
            delay += adjust; 
        }
        if ( delay > 0 ) {
            delay_loop( delay ); 
        }

        // periodically report bandwidths 
        ReportPeriodicBW(); 
    } while ( ! (sInterupted  || 
                 (mMode_time   &&  mPacketTime.after( mEndTime ))  || 
                 (!mMode_time  &&  mTotalLen >= mAmount)) && canRead ); 

    // stop timing 
    mEndTime.setnow(); 
    ReportBW( mTotalLen, 0.0, mEndTime.subSec( mStartTime )); 

    // send a final terminating datagram 
    // Don't count in the mTotalLen. The server counts this one, 
    // but didn't count our first datagram, so we're even now. 
    // The negative datagram ID signifies termination to the server. 
    mPacketTime.setnow(); 

    // store datagram ID into buffer 
    mBuf_UDP->id      = htonl( -datagramID  ); 
    mBuf_UDP->tv_sec  = htonl( mPacketTime.getSecs()); 
    mBuf_UDP->tv_usec = htonl( mPacketTime.getUsecs()); 

    if ( remote.isMulticast() ) {
        write( mSock, mBuf, mBufLen ); 
    } else {
        write_UDP_FIN( mSock, mBuf, mBufLen ); 
    } 

    printf( report_datagrams, mSock, datagramID ); 
} 
// end SendUDP 

/* ------------------------------------------------------------------- 
 * Receieve data from the (connected) UDP socket. 
 * Sends termination flag several times at the end. 
 * Does not close the socket. 
 * ------------------------------------------------------------------- */ 

void PerfSocket::Recv_UDP( void ) {
    bool going = true; 
    long currLen; 
    int32_t datagramID     = 0; 
    int32_t lastDatagramID = 0; 
    struct UDP_datagram* mBuf_UDP  = (struct UDP_datagram*) mBuf; 
    int errorCnt   = 0; 
    int outofOrder = 0; 

    extern Mutex clients_mutex; 

    // for jitter 
    Timestamp sentTime; 
    double transit; 
    double lastTransit = 0.0; 
    double deltaTransit; 

    InitTransfer(); 
    do {
        // perform read 
        currLen = read( mSock, mBuf, mBufLen ); 

        mPacketTime.setnow(); 
        mTotalLen += currLen; 

        // read the datagram ID and sentTime out of the buffer 
        datagramID = ntohl( mBuf_UDP->id ); 
        sentTime.set( ntohl( mBuf_UDP->tv_sec  ), 
                      ntohl( mBuf_UDP->tv_usec )); 

        // TODO functionalize this 
        // from RFC 1889, Real Time Protocol (RTP) 
        // J = J + ( | D(i-1,i) | - J ) / 16 
        transit = mPacketTime.subSec( sentTime ); 
        if ( lastTransit != 0.0 ) {
            deltaTransit = transit - lastTransit; 
            if ( deltaTransit < 0.0 ) {
                deltaTransit = -deltaTransit; 
            }
            mJitter += (1.0/16.0) * (deltaTransit - mJitter); 
        }
        lastTransit = transit; 

        // terminate when datagram begins with negative index 
        // the datagram ID should be correct, just negated 
        if ( datagramID < 0 ) {
            going = false; 
            datagramID = -datagramID; 
        }
        // packet loss occured if the datagram numbers aren't sequential 
        if ( datagramID != lastDatagramID+1 ) {
            if ( datagramID < lastDatagramID+1 ) {
                outofOrder++; 
            } else {
                errorCnt += datagramID - (lastDatagramID+1); 
            } 
        }
        // never decrease datagramID (e.g. if we get an out-of-order packet) 
        if ( datagramID > lastDatagramID ) {
            lastDatagramID = datagramID; 
        }

        // periodically report bandwidths 
        ReportPeriodicBW_Jitter_Loss( errorCnt, outofOrder, datagramID ); 
    } while ( going ); 

    // stop timing 
    mEndTime.setnow(); 
    ReportBW_Jitter_Loss( mTotalLen, 0.0, mEndTime.subSec( mStartTime ), 
                          errorCnt, outofOrder, datagramID ); 

    // send a acknowledgement back only if we're NOT receiving multicast 
    SocketAddr local = getLocalAddress(); 
    if ( ! local.isMulticast() ) {
        // send back an acknowledgement of the terminating datagram 
        write_UDP_AckFIN( mSock, mBuf, mBufLen ); 
    }
    // get the remote address and remove it from the set of clients 
    (clients_mutex).Lock();     
    SocketAddr remote = getRemoteAddress(); 
    iperf_sockaddr peer = *(iperf_sockaddr *) (remote.get_sockaddr()); 
    Multicast_remove_client(peer); 
    (clients_mutex).Unlock(); 
} 
// end RecvUDP 

/* ------------------------------------------------------------------- 
 * Do the equivalent of an accept() call for UDP sockets. This waits 
 * on a listening UDP socket until we get a datagram. Connect the 
 * UDP socket for efficiency. 
 * Verify that the datagram ID starts at zero. TODO is that needed still? 
 * ------------------------------------------------------------------- */ 

iperf_sockaddr PerfSocket::Accept_UDP( void ) {
    iperf_sockaddr peer; 

    Socklen_t peerlen; 
    int rc; 

    peerlen = sizeof(peer); 
    rc = recvfrom( mSock, mBuf, mBufLen, 0, 
                   (struct sockaddr*) &peer, &peerlen ); 

    FAIL_errno( rc == SOCKET_ERROR, "recvfrom" );       
    return peer; 
} 

/* ------------------------------------------------------------------- 
 * Send a datagram on the socket. The datagram's contents should signify 
 * a FIN to the application. Keep re-transmitting until an 
 * acknowledgement datagram is received. 
 * ------------------------------------------------------------------- */ 

void PerfSocket::write_UDP_FIN( int sock, void* buf, int len ) {
    int rc; 
    fd_set readSet; 
    struct timeval timeout; 

    int count = 0; 
    while ( count < 10 ) {
        count++; 

        // write data 
        write( sock, buf, len ); 

        // wait until the socket is readable, or our timeout expires 
        FD_ZERO( &readSet ); 
        FD_SET( sock, &readSet ); 
        timeout.tv_sec  = 0; 
        timeout.tv_usec = 250000; // quarter second, 250 ms 

        rc = select( sock+1, &readSet, NULL, NULL, &timeout ); 
        FAIL_errno( rc == SOCKET_ERROR, "select" ); 

        if ( rc == 0 ) {
            // select timed out 
            continue; 
        } else {
            // socket ready to read 
            rc = read( sock, buf, len ); 
            FAIL_errno( rc < 0, "read" ); 
            return; 
        } 
    } 

    printf( warn_no_ack, mSock, count ); 
} 
// end write_UDP_FIN 

/* ------------------------------------------------------------------- 
 * Send an AckFIN (a datagram acknowledging a FIN) on the socket, 
 * then select on the socket for some time. If additional datagrams 
 * come in, probably our AckFIN was lost and they are re-transmitted 
 * termination datagrams, so re-transmit our AckFIN. 
 * ------------------------------------------------------------------- */ 

void PerfSocket::write_UDP_AckFIN( int sock, void* buf, int len ) {

    int rc; 

    fd_set readSet; 
    FD_ZERO( &readSet ); 

    struct timeval timeout; 

    int count = 0; 
    while ( count < 10 ) {
        count++; 

        // write data 
        write( sock, buf, len ); 

        // wait until the socket is readable, or our timeout expires 
        FD_SET( sock, &readSet ); 
        timeout.tv_sec  = 1; 
        timeout.tv_usec = 0; 

        rc = select( sock+1, &readSet, NULL, NULL, &timeout ); 
        FAIL_errno( rc == SOCKET_ERROR, "select" ); 

        if ( rc == 0 ) {
            // select timed out 
            return; 
        } else {
            // socket ready to read 
            rc = read( sock, buf, len ); 
#ifndef WIN32
            FAIL_errno( rc < 0, "read" ); 
            continue; 
#else
            return;
#endif
        } 
    } 

    printf( warn_ack_failed, mSock, count ); 
} 
// end write_UDP_AckFIN 

void PerfSocket::Multicast_remove_client( iperf_sockaddr peer ) {
    extern vector<iperf_sockaddr> clients; 
    std::vector<iperf_sockaddr>::iterator client_iter = clients.begin();

    for ( int i=0; i < (int)(clients).size(); i++ ) {
        struct sockaddr *sap = (struct sockaddr *)&peer, *sac = (struct sockaddr *) &(*client_iter);
        if ( SocketAddr::are_Equal(sap,sac) ) {
            (clients).erase(client_iter);                                                   
            return;
        }
        client_iter++;
    } 
    return; 
}

