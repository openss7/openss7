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
 * Settings.cpp
 * by Mark Gates <mgates@nlanr.net>
 * & Ajay Tirumala <tirumala@ncsa.uiuc.edu>
 * -------------------------------------------------------------------
 * Stores and parses the initial values for all the global variables.
 * -------------------------------------------------------------------
 * headers
 * uses
 *   <stdlib.h>
 *   <stdio.h>
 *   <string.h>
 *
 *   <unistd.h>
 * ------------------------------------------------------------------- */

#define HEADERS()

#include "headers.h"

#include "Settings.hpp"
#include "Locale.hpp"

#include "util.h"

#include "gnu_getopt.h"

/* -------------------------------------------------------------------
 * command line options
 *
 * The option struct essentially maps a long option name (--foobar)
 * or environment variable ($FOOBAR) to its short option char (f).
 * ------------------------------------------------------------------- */
#define LONG_OPTIONS()

const struct option long_options[] =
{
{"bandwidth",  required_argument, NULL, 'b'},
{"client",     required_argument, NULL, 'c'},
{"format",     required_argument, NULL, 'f'},
{"help",             no_argument, NULL, 'h'},
{"interval",   required_argument, NULL, 'i'},
{"len",        required_argument, NULL, 'l'},
{"print_mss",        no_argument, NULL, 'm'},
{"num",        required_argument, NULL, 'n'},
{"output",     required_argument, NULL, 'o'},
{"port",       required_argument, NULL, 'p'},
{"server",           no_argument, NULL, 's'},
{"time",       required_argument, NULL, 't'},
{"udp",              no_argument, NULL, 'u'},
{"version",          no_argument, NULL, 'v'},
{"window",     required_argument, NULL, 'w'},

// more esoteric options
{"bind",       required_argument, NULL, 'B'},
{"daemon",           no_argument, NULL, 'D'},
{"file_input", required_argument, NULL, 'F'},
{"stdin_input",      no_argument, NULL, 'I'},
{"mss",        required_argument, NULL, 'M'},
{"nodelay",          no_argument, NULL, 'N'},
{"parallel",   required_argument, NULL, 'P'},
{"remove",        no_argument, NULL, 'R'},
{"tos",        required_argument, NULL, 'S'},
{"ttl",        required_argument, NULL, 'T'},
{"ipv6_domian",       no_argument, NULL, 'V'},
{"suggest_win_size",  no_argument, NULL, 'W'},
{0, 0, 0, 0}
};

#define ENV_OPTIONS()

const struct option env_options[] =
{
{"IPERF_BANDWIDTH",  required_argument, NULL, 'b'},
{"IPERF_CLIENT",     required_argument, NULL, 'c'},
{"IPERF_FORMAT",     required_argument, NULL, 'f'},
// skip help
{"IPERF_INTERVAL",   required_argument, NULL, 'i'},
{"IPERF_LEN",        required_argument, NULL, 'l'},
{"IPERF_PRINT_MSS",        no_argument, NULL, 'm'},
{"IPERF_NUM",        required_argument, NULL, 'n'},
{"IPERF_PORT",       required_argument, NULL, 'p'},
{"IPERF_SERVER",           no_argument, NULL, 's'},
{"IPERF_TIME",       required_argument, NULL, 't'},
{"IPERF_UDP",              no_argument, NULL, 'u'},
// skip version
{"TCP_WINDOW_SIZE",  required_argument, NULL, 'w'},

// more esoteric options
{"IPERF_BIND",       required_argument, NULL, 'B'},
{"IPERF_DAEMON",       no_argument, NULL, 'D'},
{"IPERF_FILE_INPUT", required_argument, NULL, 'F'},
{"IPERF_STDIN_INPUT",      no_argument, NULL, 'I'},
{"IPERF_MSS",        required_argument, NULL, 'M'},
{"IPERF_NODELAY",          no_argument, NULL, 'N'},
{"IPERF_PARALLEL",   required_argument, NULL, 'P'},
{"IPERF_TOS",        required_argument, NULL, 'S'},
{"IPERF_TTL",        required_argument, NULL, 'T'},
{"IPERF_IPV6_DOMAIN",      no_argument, NULL, 'V'},
{"IPERF_SUGGEST_WIN_SIZE", required_argument, NULL, 'W'},
{0, 0, 0, 0}
};

#define SHORT_OPTIONS()

const char short_options[] = "b:c:f:hi:l:mn:o:p:st:uvw:B:DF:IM:NP:RS:T:VW";

/* -------------------------------------------------------------------
 * defaults
 * ------------------------------------------------------------------- */
#define DEFAULTS()

const long kDefault_UDPRate = 1024 * 1024; // -u  if set, 1 Mbit/sec
const int  kDefault_UDPBufLen = 1470;      // -u  if set, read/write 1470 bytes
// 1470 bytes is small enough to be sending one packet per datagram on ethernet

// 1450 bytes is small enough to be sending one packet per datagram on ethernet
//  **** with IPv6 ****

/* -------------------------------------------------------------------
 * Initialize all settings to defaults.
 * ------------------------------------------------------------------- */

Settings::Settings( void ) {
    // option, default
    mUDPRate    = 0;             // -b,  ie. TCP mode
    mHost       = NULL;          // -c,  none, required for client
    mFormat     = 'a';           // -f,  adaptive bits
    // skip help                 // -h
    mBufLenSet  = false;         // -l,	
    mBufLen     = 8 * 1024;      // -l,  8 Kbyte
    mInterval   = 0;             // -i,  ie. no periodic bw reports
    mPrintMSS   = false;         // -m,  don't print MSS
    // mAmount is time also      // -n,  N/A
    mOutputFileName = NULL;      // -o,  filename
    mPort       = 5001;          // -p,  ttcp port
    mServerMode = kMode_Unknown; // -s,  or -c, none
    mAmount     = -10;           // -t,  10 seconds
    // mUDPRate > 0 means UDP    // -u,  N/A, see kDefault_UDPRate
    // skip version              // -v,
    mTCPWin     = 0;             // -w,  ie. don't set window

    // more esoteric options
    mLocalhost  = NULL;          // -B,  none
    mDaemon     = false;         // -D,  run as a daemon
    mFileInput  = false;         // -F,
    mFileName   = NULL;          // -F,  filename 
    mMSS        = 0;             // -M,  ie. don't set MSS
    mNodelay    = false;         // -N,  don't set nodelay
    mThreads    = 1;             // -P,  single client
    mRemoveService = false;      // -R,
    mTOS        = 0;             // -S,  ie. don't set type of service
    mTTL        = 1;             // -T,  link-local TTL
    mDomain     = kMode_IPv4;    // -V,
    mSuggestWin = false;         // -W,  Suggest the window size.

    mStdin = false;              // default not stdin
    mStdout = true;              // default stdout
} // end Settings

/* -------------------------------------------------------------------
 * Delete memory (hostname string).
 * ------------------------------------------------------------------- */

Settings::~Settings() {
    DELETE_PTR( mHost      );
    DELETE_PTR( mLocalhost );
    DELETE_PTR( mFileName  );
    DELETE_PTR( mOutputFileName );
} // end ~Settings

/* -------------------------------------------------------------------
 * Parses settings from user's environment variables.
 * ------------------------------------------------------------------- */
void Settings::ParseEnvironment( void ) {
    char *theVariable;

    int i = 0;
    while ( env_options[i].name != NULL ) {
        theVariable = getenv( env_options[i].name );
        if ( theVariable != NULL ) {
            Interpret( env_options[i].val, theVariable );
        }
        i++;
    }
} // end ParseEnvironment

/* -------------------------------------------------------------------
 * Parse settings from app's command line.
 * ------------------------------------------------------------------- */

void Settings::ParseCommandLine( int argc, char **argv ) {
    int option;
    while ( (option =
             gnu_getopt_long( argc, argv, short_options,
                              long_options, NULL )) != EOF ) {
        Interpret( option, gnu_optarg );
    }

    for ( int i = gnu_optind; i < argc; i++ ) {
        fprintf( stderr, "%s: ignoring extra argument -- %s\n", argv[0], argv[i] );
    }
} // end ParseCommandLine

/* -------------------------------------------------------------------
 * Interpret individual options, either from the command line
 * or from environment variables.
 * ------------------------------------------------------------------- */

void Settings::Interpret( char option, const char *optarg ) {
    char outarg[100];

    switch ( option ) {
        case 'b': // UDP bandwidth
            GetLowerCaseArg(optarg,outarg);
            mUDPRate = byte_atof(outarg);

            // if -l has already been processed, mBufLenSet is true
            // so don't overwrite that value.
            if ( ! mBufLenSet ) {
                mBufLen = kDefault_UDPBufLen;
            }
            break;

        case 'c': // client mode w/ server host to connect to
            mServerMode = kMode_Client;
            mHost = new char[ strlen( optarg ) + 1 ];
            strcpy( mHost, optarg );
            break;

        case 'f': // format to print in
            mFormat = (*optarg);
            break;

        case 'h': // print help and exit
#ifndef WIN32
            fprintf( stderr, usage_long );
#else
            fprintf(stderr, usage_long1);
            fprintf(stderr, usage_long2);
#endif
            exit(1);
            break;

        case 'i': // specify interval between periodic bw reports
            mInterval = atof( optarg );
            break;

        case 'l': // length of each buffer
            GetUpperCaseArg(optarg,outarg);
            mBufLen = (long) byte_atof(outarg );
            mBufLenSet = true;
            break;

        case 'm': // print TCP MSS
            mPrintMSS = true;
            break;

        case 'n': // bytes of data
            // positive indicates amount mode (instead of time mode)
            GetUpperCaseArg(optarg,outarg);
            mAmount = +byte_atof( outarg );
            break;

        case 'o' : // output the report and other messages into the file
            mStdout = false;
            mOutputFileName = new char[strlen(optarg)+1];
            strcpy( mOutputFileName, optarg);
            break;

        case 'p': // server port
            mPort = atoi( optarg );
            break;

        case 's': // server mode
            mServerMode = kMode_Server;
            break;

        case 't': // seconds to write for
            // negative indicates time mode (instead of amount mode)
            mAmount = -atof( optarg );
            break;

        case 'u': // UDP instead of TCP
            // if -b has already been processed, UDP rate will
            // already be non-zero, so don't overwrite that value
            if ( mUDPRate == 0 ) {
                mUDPRate = kDefault_UDPRate;
            }
            // if -l has already been processed, mBufLenSet is true
            // so don't overwrite that value.
            if ( ! mBufLenSet ) {
                mBufLen = kDefault_UDPBufLen;
            }
            break;

        case 'v': // print version and exit
            fprintf( stderr, version );
            exit(1);
            break;

        case 'w': // TCP window size (socket buffer size)
            GetUpperCaseArg(optarg,outarg);
            mTCPWin = (long)byte_atof(outarg);

            if ( mTCPWin < 2048 ) {
                printf( warn_window_small, mTCPWin );
            }
            break;

            // more esoteric options
        case 'B': // specify bind address
            mLocalhost = new char[ strlen( optarg ) + 1 ];
            strcpy( mLocalhost, optarg );
            break;

        case 'D': // Run as a daemon
            mDaemon = true;
            break;

        case 'F' : // Get the input for the data stream from a file
            mFileInput = true;
            mFileName = new char[strlen(optarg)+1];
            strcpy( mFileName, optarg);
            break;

        case 'I' : // Set the stdin as the input source
            mFileInput = true;
            mStdin     = true;
            mFileName = new char[strlen("<stdin>")+1];
            strcpy( mFileName,"<stdin>");
            break;

        case 'M': // specify TCP MSS (maximum segment size)
            GetUpperCaseArg(optarg,outarg);

            mMSS = (long) byte_atof(outarg );
            break;

        case 'N': // specify TCP nodelay option (disable Jacobson's Algorithm)
            mNodelay = true;
            break;

        case 'P': // number of client threads
            mThreads = atoi( optarg );
            break;

        case 'R':
            mRemoveService = true;
            break;

        case 'S': // IP type-of-service
            // TODO use a function that understands base-2
            // the zero base here allows the user to specify
            // "0x#" hex, "0#" octal, and "#" decimal numbers
            mTOS = strtol( optarg, NULL, 0 );
            break;

        case 'T': // time-to-live for multicast
            mTTL = atoi( optarg );
            break;

        case 'V': // IPv6 Domain
            mDomain = kMode_IPv6;
            break;

        case 'W' :
            mSuggestWin = false;
            printf("The -W option is not available in this release\n");
            break;

        default: // ignore unknown
            break;
    }
} // end Interpret

const void Settings::GetUpperCaseArg(const char *inarg, char *outarg) {

    int len = strlen(inarg);
    strcpy(outarg,inarg);

    if ( (len > 0) && (inarg[len-1] >='a') 
         && (inarg[len-1] <= 'z') )
        outarg[len-1]= outarg[len-1]+'A'-'a';
}

const void Settings::GetLowerCaseArg(const char *inarg, char *outarg) {

    int len = strlen(inarg);
    strcpy(outarg,inarg);

    if ( (len > 0) && (inarg[len-1] >='A') 
         && (inarg[len-1] <= 'Z') )
        outarg[len-1]= outarg[len-1]-'A'+'a';
}
