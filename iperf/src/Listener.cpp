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
 * Listener.cpp
 * by Mark Gates <mgates@nlanr.net> 
 * &  Ajay Tirumala <tirumala@ncsa.uiuc.edu> 
 * ------------------------------------------------------------------- 
 * Listener sets up a socket listening on the server host. For each 
 * connected socket that accept() returns, this creates a Server 
 * socket and spawns a thread for it. 
 * 
 * Changes to the latest version. Listener will run as a daemon 
 * Multicast Server is now Multi-threaded 
 * ------------------------------------------------------------------- 
 * headers 
 * uses 
 *   <stdlib.h> 
 *   <stdio.h> 
 *   <string.h> 
 *   <errno.h> 
 * 
 *   <sys/types.h> 
 *   <unistd.h> 
 * 
 *   <netdb.h> 
 *   <netinet/in.h> 
 *   <sys/socket.h> 
 * ------------------------------------------------------------------- */ 


#define HEADERS() 

#include "headers.h" 
#include "Listener.hpp" 
#include "Server.hpp" 
#include "Locale.hpp" 
#include "Settings.hpp" 
#include "PerfSocket.hpp" 
#include "util.h" 


/* ------------------------------------------------------------------- 
 * Stores local hostname and socket info. 
 * ------------------------------------------------------------------- */ 

using namespace std; 
vector<iperf_sockaddr> clients;
Mutex clients_mutex; 

Listener::Listener( short inPort, bool inUDP, const char *inLocalhost ) 
: PerfSocket( inPort, inUDP ), 
Thread() {
    mLocalhost = NULL; 

    if ( inLocalhost != NULL ) {
        mLocalhost = new char[ strlen( inLocalhost ) + 1 ]; 
        strcpy( mLocalhost, inLocalhost ); 
    }

    // open listening socket 
    Listen( mLocalhost, gSettings->GetDomain() ); 
    ReportServerSettings( inLocalhost ); 

} // end Listener 

/* ------------------------------------------------------------------- 
 * Delete memory (hostname string). 
 * ------------------------------------------------------------------- */ 
Listener::~Listener() {
    DELETE_PTR( mLocalhost  ); 
} // end ~Listener 

/* ------------------------------------------------------------------- 
 * Listens for connections and starts Servers to handle data. 
 * For TCP, each accepted connection spawns a Server thread. 
 * For UDP, handle all data in this thread. 
 * ------------------------------------------------------------------- */ 
void Listener::Run( void ) {

    struct UDP_datagram* mBuf_UDP  = (struct UDP_datagram*) mBuf; 
    if ( mUDP ) {
        // UDP uses listening socket 
        // The server will now run as a multi-threaded server 
        Server *theServer=NULL;
        iperf_sockaddr peer; 

        // Accept each packet, 
        // If there is no existing client, then start  
        // a new thread to service the new client 
        // The main server runs in a single thread 
        // Thread per client model is followed 
        do {
            peer = Accept_UDP(); 

            (clients_mutex).Lock(); 
            bool exist = present(peer); 
            (clients_mutex).Unlock(); 
            int32_t datagramID = ntohl( mBuf_UDP->id ); 
            if ( !exist && datagramID >= 0 ) {
                int rc = connect( mSock, (struct sockaddr*) &peer 
                                  ,sizeof(peer)); 
                FAIL_errno( rc == SOCKET_ERROR, "connect UDP" );       
#ifndef WIN32
                (clients_mutex).Lock(); 
                (clients).push_back(peer); 
                (clients_mutex).Unlock(); 
                theServer = new Server(mPort, mUDP, mSock); 
                theServer->DeleteSelfAfterRun(); 
                theServer->Start(); 
                theServer = NULL; 
                mSock = -1; 
                Listen(mLocalhost, gSettings->GetDomain()); 
            }
#else /* WIN32 */ 
                // WIN 32 Does not handle multiple UDP stream hosts.
                theServer = new Server(mPort, mUDP, mSock); 
                theServer->Run(); 
                DELETE_PTR( theServer ); 

                // create a new socket 
                mSock = -1; 
                Listen( mLocalhost, gSettings->GetDomain() ); 
            }
#endif /* WIN32 */ 
        } while ( true ); 
    } else {
        // TCP uses sockets returned from Accept 
        int connected_sock; 
        do {
            connected_sock = Accept(); 
            if ( connected_sock >= 0 ) {
                // startup the server thread, then forget about it 
                Server* theServer = new Server( mPort, mUDP, connected_sock ); 

                theServer->DeleteSelfAfterRun(); 
                theServer->Start(); 

                theServer = NULL; 
            }
        } while ( connected_sock != INVALID_SOCKET ); 
    } 
} // end Run 


/**-------------------------------------------------------------------- 
 * Run the server as a daemon  
 * --------------------------------------------------------------------*/ 
void Listener::runAsDaemon(const char *pname, int facility) {
#ifndef WIN32 
    pid_t pid; 

    /* Create a child process & if successful, exit from the parent process */ 
    if ( (pid = fork()) == -1 ) {
        printf("error in first child create\n");     
        exit(0); 
    } else if ( pid != 0 ) {
        exit(0); 
    }

    /* Try becoming the session leader, once the parent exits */
    if ( setsid() == -1 ) {           /* Become the session leader */ 
        fputs("Cannot change the session group leader\n",stdout); 
    } else {
    } 
    signal(SIGHUP,SIG_IGN); 


    /* Now fork() and get released from the terminal */  
    if ( (pid = fork()) == -1 ) {
        printf("error\n");   
        exit(0); 
    } else if ( pid != 0 ) {
        exit(0); 
    }

    chdir("."); 
    printf("Running Iperf Server as a daemon\n"); 
    printf("The Iperf daemon process ID : %d\n",((int)getpid())); 
    fflush(stdout);  
    /*umask(0);*/ 

    fclose(stdout); 
    fclose(stdin); 
    openlog(pname,LOG_CONS,facility); 
#else 
    printf("Use the precompiled windows version for service (daemon) option\n"); 
#endif  

} 


/** 
 * Function which checks whether there is an existing connection 
 * from the client which sent the most recent packet 
 */ 
bool Listener::present(iperf_sockaddr peer) {
    struct sockaddr *sap = (struct sockaddr *)&peer;
    struct sockaddr *sac;
    for ( int i=0; i < (int)clients.size(); i++ ) {
        sac = (struct sockaddr *)&clients[i];
        if ( SocketAddr::are_Equal(sap,sac) ) {
            return true;
        }
    }
    return false;
}

