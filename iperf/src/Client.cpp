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
 * Client.cpp
 * by Mark Gates <mgates@nlanr.net>
 * -------------------------------------------------------------------
 * A client thread initiates a connect to the server and handles
 * sending and receiving data, then closes the socket.
 * -------------------------------------------------------------------
 * headers
 * uses
 *   <stdlib.h>
 *   <string.h>
 * ------------------------------------------------------------------- */

#define HEADERS()

#include "headers.h"

#include "Client.hpp"
#include "Locale.hpp"
#include "Settings.hpp"

#include "util.h"

int Client::sNumThreads = 1;
Condition Client::sNum_cond;

/* -------------------------------------------------------------------
 * Store server hostname, optionally local hostname, and socket info.
 * ------------------------------------------------------------------- */

Client::Client( short inPort, bool inUDP, const char *inHostname,
                const char *inLocalhost, bool inPrintSettings )
: PerfSocket( inPort, inUDP ),
Thread() {
    const char *theHost = ((inHostname == NULL) ? "localhost" : inHostname);
    // connect
    Connect( theHost, inLocalhost );

    if ( inPrintSettings ) {
        ReportClientSettings( theHost, inLocalhost );
    }
} // end Client

/* -------------------------------------------------------------------
 * Delete memory (hostname strings).
 * ------------------------------------------------------------------- */

Client::~Client() {
} // end ~Client

/* -------------------------------------------------------------------
 * Connect to the server and send data.
 * ------------------------------------------------------------------- */

void Client::Run( void ) {
#ifdef HAVE_THREAD

    // Barrier
    // wait until the number of anticipated threads have reached this point
    sNum_cond.Lock();
    sNumThreads--;
    sNum_cond.Broadcast();
    while ( sNumThreads > 0 ) {
        sNum_cond.Wait();
    }
    sNum_cond.Unlock();
#endif

    // send data
    if ( mUDP ) {
        Send_UDP();
    } else {
        Send_TCP();
    }
} // end Run
