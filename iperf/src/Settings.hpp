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
 * Settings.hpp
 * by Mark Gates <mgates@nlanr.net>
 * &  Ajay Tirumala <tirumala@ncsa.uiuc.edu>
 * -------------------------------------------------------------------
 * Stores and parses the initial values for all the global variables.
 * -------------------------------------------------------------------
 * headers
 * uses
 *   <stdlib.h>
 *   <assert.h>
 * ------------------------------------------------------------------- */

#ifndef SETTINGS_H
#define SETTINGS_H

#include "headers.h"

#include "Thread.hpp"

/* -------------------------------------------------------------------
 * constants
 * ------------------------------------------------------------------- */

// server/client mode
enum ServerMode {
    kMode_Server,
    kMode_Client,
    kMode_Unknown
};

// protocol mode
const bool kMode_UDP    = false;
const bool kMode_TCP    = true;

// termination mode
const bool kMode_Amount = false;  // transmit fixed amount
const bool kMode_Time   = true;   // transmit fixed time

// domain mode 
const bool kMode_IPv4 = false; 
const bool kMode_IPv6 = true;


class Settings;
extern Settings* gSettings;

/* ------------------------------------------------------------------- */
class Settings {
public:
    // set to defaults
    Settings( void );

    // free associated memory
    ~Settings();

    // parse settings from user's environment variables
    void ParseEnvironment( void );

    // parse settings from app's command line
    void ParseCommandLine( int argc, char **argv );

    // convert to lower case for [KMG]bits/sec
    const void GetLowerCaseArg(const char *,char *);

    // conver to upper case for [KMG]bytes/sec
    const void GetUpperCaseArg(const char *,char *);

    // ---- access settings

    // -b #
    // valid in UDP mode
    double GetUDPRate( void ) const {
        assert( GetProtocolMode() == kMode_UDP );
        return mUDPRate;
    }

    // -c <host>
    // valid in client mode
    const char* GetHost( void ) const {
        assert( GetServerMode() == kMode_Client );
        return mHost;
    }

    // -f [kmKM]
    char GetFormat( void ) const {
        return mFormat;
    }

    // -l #
    int GetBufferLen( void ) const {
        return mBufLen;
    }

    // -m
    // valid in TCP mode
    bool GetPrintMSS( void ) const {
        assert( GetProtocolMode() == kMode_TCP );
        return mPrintMSS;
    }

    // -n #
    // valid in Amount mode (transmit fixed amount of data)
    max_size_t GetAmount( void ) const {
        assert( mAmount > 0 );
        assert( GetTerminationMode() == kMode_Amount );
        return(max_size_t) mAmount;
    }

    // -p #
    unsigned short GetPort( void ) const {
        return mPort;
    }

    // -s or -c
    ServerMode GetServerMode( void ) const {
        return mServerMode;
    }

    // -t #
    // valid in Time mode (transmit for fixed time)
    double GetTime( void ) const {
        assert( mAmount < 0 );
        assert( GetTerminationMode() == kMode_Time );
        return -mAmount;
    }

    // -w #
    int GetTCPWindowSize( void ) const {
        return mTCPWin;
    }

    // ---- more esoteric options

    // -B <host>
    const char* GetLocalhost( void ) const {
        return mLocalhost;
    }

    // -i #
    double GetInterval( void ) const {
        return mInterval;
    }

    // -M #
    // valid in TCP mode
    int GetTCP_MSS( void ) const {
        assert( GetProtocolMode() == kMode_TCP );
        return mMSS;
    }

    // -N
    // valid in TCP mode
    bool GetTCP_Nodelay( void ) const {
        assert( GetProtocolMode() == kMode_TCP );
        return mNodelay;
    }

    // -P #
    // only useful in TCP mode
    int GetClientThreads( void ) const {
        //assert( GetProtocolMode() == kMode_TCP );
        return mThreads;
    }

    // -S #
    int GetTOS( void ) const {
        return mTOS;
    }

    // -T #
    // valid in UDP mode
    u_char GetMcastTTL( void ) const {
        return mTTL;
    }

    // ---- modes

    // differentiate Time and Amount; -t # and -n #
    // note these optimize to just the comparison with zero
    bool GetTerminationMode( void ) const {
        return(mAmount < 0  ?  kMode_Time  :  kMode_Amount);
    }

    // differentiate UDP and TCP; -u or -b # gives UDP
    bool GetProtocolMode( void ) const {
        return(mUDPRate == 0  ?  kMode_TCP  :  kMode_UDP);
    }

    // whether the server runs as a daemon or not
    bool GetDaemonMode(void) const {
        return mDaemon;
    }

    // Whether the client must suggest the window size or not
    bool GetSuggestWin(void) const {
        return mSuggestWin;
    }

    // Set the interval time
    void SetInterval(double interval) {
        mInterval = interval;
    }

    // Set the file input status
    void SetFileInput(bool status) {
        mFileInput = status;
    }


    // Whether the input is the default stream or a
    // file stream
    bool GetFileInput(void) const {
        return mFileInput;
    }

    // return the file name if the input is through a 
    // file stream
    char *GetFileName(void) const {
        if ( mFileInput )
            return mFileName;
        return NULL;
    }

    // return whether the input is from stdin
    bool GetStdin(void) const {
        return mStdin;
    }

    // file stream
    bool GetFileOutput(void) const {
        if ( mStdout )
            return false;
        return true;
    }

    // return the file name if the output is through a 
    // file stream
    char *GetOutputFileName(void) const {
        if ( !mStdout )
            return mOutputFileName;
        return NULL;
    }

    // whether win32 service should be removed or not
    bool GetRemoveService(void) const {
        return mRemoveService;
    }

    // IPv4 or IPv6 domain
    bool GetDomain(void) const {
        return mDomain;
    }


protected:
    void Interpret( char option, const char *optarg );

    // here instead of listing by option order (as elsewhere)
    // I list by type so that the structure alignment will be nice

    char*  mHost;      // -c
    char*  mLocalhost; // -B
    char*  mFileName;  // -F
    char*  mOutputFileName; // -o

    double mUDPRate;   // -b
    double mAmount;    // -n or -t
    double mInterval;  // -i

    int    mBufLen;    // -l
    int    mTCPWin;    // -w
    int    mMSS;       // -m
    int    mThreads;   // -P
    int    mTOS;       // -S

    unsigned short mPort; // -p

    char   mFormat;    // -f
    u_char mTTL;       // --ttl

    ServerMode mServerMode; // -s or -c

    bool   mPrintMSS;  // -M
    bool   mNodelay;   // -N
    bool   mBufLenSet; // if -l was specified.
                       // UDP and TCP have different BufLen defaults.
    bool   mDaemon;    // -d
    bool   mSuggestWin;
    bool   mFileInput; // -F or -I
    bool   mStdin;     // -I
    bool   mStdout;     // -o
    bool   mDomain;    // -V
    bool   mRemoveService; // -R

}; // end class Settings

#endif // SETTINGS_H
