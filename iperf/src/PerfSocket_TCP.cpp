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
 * 
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

/* -------------------------------------------------------------------
 * Send data using the connected TCP socket.
 * Does not close the socket.
 * ------------------------------------------------------------------- */

void PerfSocket::Send_TCP( void ) {
    if ( gSettings->GetSuggestWin() ) {
        Client_Recv_TCP();
        return;
    }

    // terminate loop nicely on user interupts
    sInterupted = false;
    my_signal( SIGINT,  Sig_Interupt );
    my_signal( SIGPIPE, Sig_Interupt );

    int currLen;
    bool fileInput = gSettings->GetFileInput();
    bool canRead;
    InitTransfer();

    do {

        // If the input is from a 
        // file, fill the buffer with
        // data from the file 
        if ( fileInput ) {
            extractor->getNextDataBlock(mBuf);

            // If the first character is 'a'
            // change if to '0' so that the
            // server does not mistake it for
            // a Window Suggest option
            if ( mBuf[0] == 'a' )
                mBuf[0] = '0';
            canRead = extractor->canRead();
        } else
            canRead = true;

        // perform write
        currLen = write( mSock, mBuf, mBufLen );
        mPacketTime.setnow();
        if ( currLen < 0 ) {
            WARN_errno( currLen < 0, "write" );
            break;
        }

        mTotalLen += currLen;

        // periodically report bandwidths
        ReportPeriodicBW();

    } while ( ! (sInterupted  ||
                 (mMode_time   &&  mPacketTime.after( mEndTime ))  ||
                 (!mMode_time  &&  mTotalLen >= mAmount)) && canRead );

    // shutdown sending connection and wait (read)
    // for the other side to shutdown
    shutdown( mSock, SHUT_WR );
    currLen = read( mSock, mBuf, mBufLen );
    WARN_errno( currLen == SOCKET_ERROR, "read on server close" );
    WARN( currLen > 0, "server sent unexpected data" );

    // stop timing
    mEndTime.setnow();
    ReportBW( mTotalLen, 0.0, mEndTime.subSec( mStartTime ));

    if ( gSettings->GetPrintMSS() ) {
        // read the socket option for MSS (maximum segment size)
        ReportMSS( getsock_tcp_mss( mSock ));
    }
}
// end SendTCP

/* -------------------------------------------------------------------
 * Receieve data from the connected TCP socket.
 * Does not close the socket.
 * ------------------------------------------------------------------- */

void PerfSocket::Recv_TCP( void ) {
    // keep track of read sizes -> gives some indication of MTU size
    // on SGI this must be dynamically allocated to avoid seg faults
    int currLen;
    int *readLenCnt = new int[ mBufLen+1 ];
    for ( int i = 0; i <= mBufLen; i++ ) {
        readLenCnt[ i ] = 0;
    }

    InitTransfer();
#ifndef WIN32
    signal (SIGPIPE, SIG_IGN);
#endif

    do {
        // perform read
        currLen = read( mSock, mBuf, mBufLen );

        if ( false ) {
            DELETE_PTR( readLenCnt );
            Server_Send_TCP();
            return;
        }
        mPacketTime.setnow();
        mTotalLen += currLen;

        // count number of reads of each size
        if ( currLen <= mBufLen ) {
            readLenCnt[ currLen ]++;
        }

        // periodically report bandwidths
        ReportPeriodicBW();

    } while ( currLen > 0  &&  sInterupted == false );

    // stop timing
    mEndTime.setnow();
    ReportBW( mTotalLen, 0.0, mEndTime.subSec( mStartTime ));

    if ( gSettings->GetPrintMSS() ) {
        // read the socket option for MSS (maximum segment size)
        ReportMSS( getsock_tcp_mss( mSock ));

        // on WANs the most common read length is often the MSS
        // on fast LANs it is much harder to detect
        int totalReads  = 0;
        for ( currLen = 0; currLen < mBufLen+1; currLen++ ) {
            totalReads += readLenCnt[ currLen ];
        }

        // print each read length that occured > 5% of reads
        int thresh = (int) (0.05 * totalReads);
        printf( report_read_lengths, mSock );
        for ( currLen = 0; currLen < mBufLen+1; currLen++ ) {
            if ( readLenCnt[ currLen ] > thresh ) {
                printf( report_read_length_times, mSock,
                        (int) currLen, readLenCnt[ currLen ],
                        (100.0 * readLenCnt[ currLen ]) / totalReads );
            }
        }
    }
    DELETE_PTR( readLenCnt );
}
// end RecvTCP


void PerfSocket::Client_Recv_TCP(void) {
    // terminate loop nicely on user interupts

    sInterupted = false;
    //my_signal( SIGINT,  Sig_Interupt );
    //my_signal( SIGPIPE, Sig_Interupt );
#ifndef WIN32
    signal (SIGPIPE, SIG_IGN);
#endif

    int currLen;
    InitTransfer();
    double fract = 0.0;
    mStartTime.setnow();
    long  endSize = get_tcp_windowsize(mSock), startSize=endSize, loopLen =0, prevLen =0;
    Timestamp prevTime;
    prevTime.setnow();



    /* Periodic reporting is done here in the loop itself, if Suggest Window Size option is set*/
    mPReporting = false;

    /* Send the first packet indicating that the server has to send data */
    mBuf[0] = 'a';
    currLen = write( mSock, mBuf, mBufLen );
    if ( currLen < 0 ) {
        WARN_errno( currLen < 0, "write" );
        return;
    }

    do {
        // perform read
        currLen = read( mSock, mBuf, mBufLen );
        mPacketTime.setnow();
        if ( currLen < 0 ) {
            WARN_errno( currLen < 0, "read" );
            break;
        }

        mTotalLen += currLen;
        loopLen +=currLen;

        // periodically report bandwidths
        ReportPeriodicBW();

        double nFract = mStartTime.fraction(mPacketTime,mEndTime);
        if ( nFract > (fract + 0.1) ) {
            printf(seperator_line);
            ReportWindowSize();
            ReportBW( loopLen, prevTime.subSec(mStartTime),  mPacketTime.subSec( mStartTime));
            fract +=0.1;
            if ( startSize != endSize ) {
                /* Change the window size only if the data transfer has changed at least by 5% */
                if ( loopLen < prevLen ) {

                    if ( ( ((double)(prevLen - loopLen)) /  
                           ((double)prevLen))  > 0.05         
                       ) {
                        endSize = startSize + (endSize - startSize)/2;
                    }

                } else {
                    if ( ( ((double)(loopLen - prevLen)) /       
                           ((double)prevLen) ) > 0.05          
                       ) {
                        startSize = endSize;
                        endSize = endSize*2;
                        prevLen = loopLen;

                    }
                }
            } else {
                endSize = endSize*2;
                prevLen = loopLen;
            }

            /** Reset the variables after setting new window size */
            prevTime.setnow();
            loopLen = 0 ;
            //shutdown(mSock,SHUT_RDWR);
            close(mSock);
            mSock = -1;
            Connect(gSettings->GetHost(),gSettings->GetLocalhost());
            mBuf[0] = 'a';
            if ( set_tcp_windowsize(mSock,endSize) == -1 ) {
                printf(unable_to_change_win);
            }
            if ( get_tcp_windowsize(mSock) != endSize ) {
                printf(unable_to_change_win);
            }
            write( mSock, mBuf, mBufLen );
        }
    } while ( ! (sInterupted  ||
                 (mMode_time   &&  mPacketTime.after( mEndTime ))  ||
                 (!mMode_time  &&  mTotalLen >= mAmount))
            );


    printf( seperator_line );
    ReportWindowSize();
    ReportBW( loopLen, prevTime.subSec(mStartTime),  mPacketTime.subSec( mStartTime));

    printf( seperator_line);
    printf( opt_estimate);
    if ( loopLen > prevLen )
        set_tcp_windowsize(mSock,endSize);
    else
        set_tcp_windowsize(mSock,startSize);

    ReportWindowSize();
    printf( seperator_line );

    // stop timing
    mEndTime.setnow();

    ReportBW( mTotalLen, 0.0, mEndTime.subSec( mStartTime ));

    if ( gSettings->GetPrintMSS() ) {
        // read the socket option for MSS (maximum segment size)
        ReportMSS( getsock_tcp_mss( mSock ));
    }
    //close(mSock);
    //mSock = -1;
}


void PerfSocket::Server_Send_TCP(void) {

    int currLen;
    int writeLenCnt[8193];

    if ( mBufLen > 8193 )
        mBufLen = 8193;
    for ( int i = 0; i <= mBufLen; i++ ) {
        writeLenCnt[ i ] = 0;
    }

#ifndef WIN32
    signal(SIGPIPE,SIG_IGN);
#endif

    do {
        // perform write
        currLen = write( mSock, mBuf, mBufLen );
        mPacketTime.setnow();
        mTotalLen += currLen;

        // count number of reads of each size
        if ( currLen <= mBufLen ) {
            writeLenCnt[ currLen ]++;
        }

        // periodically report bandwidths
        ReportPeriodicBW();

    } while ( currLen > 0  &&  sInterupted == false );

    if ( sInterupted != false )
        printf("The stream was interrupted\n");

    // stop timing
    mEndTime.setnow();
    ReportBW( mTotalLen, 0.0, mEndTime.subSec( mStartTime ));

    if ( gSettings->GetPrintMSS() ) {
        // read the socket option for MSS (maximum segment size)
        ReportMSS( getsock_tcp_mss( mSock ));

        // on WANs the most common read length is often the MSS
        // on fast LANs it is much harder to detect
        int totalWrites  = 0;
        for ( currLen = 0; currLen < mBufLen+1; currLen++ ) {
            totalWrites += writeLenCnt[ currLen ];
        }

        // print each read length that occured > 5% of reads
        int thresh = (int) (0.05 * totalWrites);
        printf( report_read_lengths, mSock );
        for ( currLen = 0; currLen < mBufLen+1; currLen++ ) {
            if ( writeLenCnt[ currLen ] > thresh ) {
                printf( report_read_length_times, mSock,
                        (int) currLen, writeLenCnt[ currLen ],
                        (100.0 * writeLenCnt[ currLen ]) / totalWrites );
            }
        }
    }
}








