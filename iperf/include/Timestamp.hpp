//  ==========================================================================
//  
//  @(#) $Id$
//  
//  --------------------------------------------------------------------------
//  
//  Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
//  
//  All Rights Reserved.
//  
//  This program is free software; you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by the Free
//  Software Foundation; version 3 of the License.
//  
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
//  for more details.
//  
//  You should have received a copy of the GNU General Public License along
//  with this program.  If not, see <http://www.gnu.org/licenses/>, or write
//  to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,
//  USA.
//  
//  --------------------------------------------------------------------------
//  
//  U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
//  behalf of the U.S. Government ("Government"), the following provisions
//  apply to you.  If the Software is supplied by the Department of Defense
//  ("DoD"), it is classified as "Commercial Computer Software" under
//  paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
//  Regulations ("DFARS") (or any successor regulations) and the Government is
//  acquiring only the license rights granted herein (the license rights
//  customarily provided to non-Government users).  If the Software is
//  supplied to any unit or agency of the Government other than DoD, it is
//  classified as "Restricted Computer Software" and the Government's rights
//  in the Software are defined in paragraph 52.227-19 of the Federal
//  Acquisition Regulations ("FAR") (or any successor regulations) or, in the
//  cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
//  (or any successor regulations).
//  
//  --------------------------------------------------------------------------
//  
//  Commercial licensing and support of this software is available from
//  OpenSS7 Corporation at a fee.  See http://www.openss7.com/
//  
//  --------------------------------------------------------------------------
//  
//  Last Modified $Date$ by $Author$
//  
//  --------------------------------------------------------------------------
//  
//  $Log$
//  ==========================================================================

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
 * Timestamp.hpp
 * by Mark Gates <mgates@nlanr.net>
 * -------------------------------------------------------------------
 * A generic interface to a timestamp.
 * This implementation uses the unix gettimeofday().
 * -------------------------------------------------------------------
 * headers
 * uses
 *   <sys/types.h>
 *   <sys/time.h>
 *   <unistd.h>
 * ------------------------------------------------------------------- */

#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include "headers.h"

/* ------------------------------------------------------------------- */
class Timestamp {
public:
    /* -------------------------------------------------------------------
     * Create a timestamp, with the current time in it.
     * ------------------------------------------------------------------- */
    Timestamp( void ) {
        setnow();
    }

    /* -------------------------------------------------------------------
     * Create a timestamp, with the given seconds/microseconds
     * ------------------------------------------------------------------- */
    Timestamp( long sec, long usec ) {
        set( sec, usec );
    }

    /* -------------------------------------------------------------------
     * Create a timestamp, with the given seconds
     * ------------------------------------------------------------------- */
    Timestamp( double sec ) {
        set( sec );
    }

    /* -------------------------------------------------------------------
     * Set timestamp to current time.
     * ------------------------------------------------------------------- */
    void setnow( void ) {
        gettimeofday( &mTime, NULL );
    }

    /* -------------------------------------------------------------------
     * Set timestamp to the given seconds/microseconds
     * ------------------------------------------------------------------- */
    void set( long sec, long usec ) {
        assert( sec  >= 0 );
        assert( usec >= 0  &&  usec < kMillion );

        mTime.tv_sec  = sec;
        mTime.tv_usec = usec;      
    }

    /* -------------------------------------------------------------------
     * Set timestamp to the given seconds
     * ------------------------------------------------------------------- */
    void set( double sec ) {
        mTime.tv_sec  = (long) sec;
        mTime.tv_usec = (long) ((sec - mTime.tv_sec) * kMillion);
    }

    /* -------------------------------------------------------------------
     * return seconds portion of timestamp
     * ------------------------------------------------------------------- */
    long getSecs( void ) {
        return mTime.tv_sec;
    }

    /* -------------------------------------------------------------------
     * return microseconds portion of timestamp
     * ------------------------------------------------------------------- */
    long getUsecs( void ) {
        return mTime.tv_usec;
    }

    /* -------------------------------------------------------------------
     * return timestamp as a floating point seconds
     * ------------------------------------------------------------------- */
    double get( void ) {
        return mTime.tv_sec + mTime.tv_usec / ((double) kMillion);
    }

    /* -------------------------------------------------------------------
     * subtract the right timestamp from my timestamp.
     * return the difference in microseconds.
     * ------------------------------------------------------------------- */
    long subUsec( Timestamp right ) {
        return(mTime.tv_sec  - right.mTime.tv_sec) * kMillion +
        (mTime.tv_usec - right.mTime.tv_usec);
    }

    /* -------------------------------------------------------------------
     * subtract the right timestamp from my timestamp.
     * return the difference in microseconds.
     * ------------------------------------------------------------------- */
    long subUsec( timeval right ) {
        return(mTime.tv_sec  - right.tv_sec) * kMillion +
        (mTime.tv_usec - right.tv_usec);
    }

    /* -------------------------------------------------------------------
     * subtract the right timestamp from my timestamp.
     * return the difference in seconds as a floating point.
     * ------------------------------------------------------------------- */
    double subSec( Timestamp right ) {
        return(mTime.tv_sec  - right.mTime.tv_sec) +
        (mTime.tv_usec - right.mTime.tv_usec) / ((double) kMillion);
    }

    /* -------------------------------------------------------------------
     * add the right timestamp to my timestamp.
     * ------------------------------------------------------------------- */
    void add( Timestamp right ) {
        mTime.tv_sec  += right.mTime.tv_sec;
        mTime.tv_usec += right.mTime.tv_usec;

        // watch for under- and overflow
        if ( mTime.tv_usec < 0 ) {
            mTime.tv_usec += kMillion;
            mTime.tv_sec--;
        }
        if ( mTime.tv_usec >= kMillion ) {
            mTime.tv_usec -= kMillion;
            mTime.tv_sec++;
        }

        assert( mTime.tv_usec >= 0  &&
                mTime.tv_usec <  kMillion );
    }

    /* -------------------------------------------------------------------
     * add the seconds to my timestamp.
     * TODO optimize?
     * ------------------------------------------------------------------- */
    void add( double sec ) {
        mTime.tv_sec  += (long) sec;
        mTime.tv_usec += (long) ((sec - ((long) sec )) * kMillion);

        // watch for overflow
        if ( mTime.tv_usec >= kMillion ) {
            mTime.tv_usec -= kMillion;
            mTime.tv_sec++;
        }

        assert( mTime.tv_usec >= 0  &&
                mTime.tv_usec <  kMillion );
    }

    /* -------------------------------------------------------------------
     * return true if my timestamp is before the right timestamp.
     * ------------------------------------------------------------------- */
    bool before( Timestamp right ) {
        return mTime.tv_sec < right.mTime.tv_sec  ||
        (mTime.tv_sec == right.mTime.tv_sec &&
         mTime.tv_usec < right.mTime.tv_usec);
    }
    
    /* -------------------------------------------------------------------
     * return true if my timestamp is before the right timestamp.
     * ------------------------------------------------------------------- */
    bool before( timeval right ) {
        return mTime.tv_sec < right.tv_sec  ||
        (mTime.tv_sec == right.tv_sec &&
         mTime.tv_usec < right.tv_usec);
    }

    /* -------------------------------------------------------------------
     * return true if my timestamp is after the right timestamp.
     * ------------------------------------------------------------------- */
    bool after( Timestamp right ) {
        return mTime.tv_sec > right.mTime.tv_sec  ||
        (mTime.tv_sec == right.mTime.tv_sec &&
         mTime.tv_usec > right.mTime.tv_usec);
    }

    /**
     * This function returns the fraction of time elapsed after the beginning 
     * till the end
     */
    double fraction(Timestamp currentTime, Timestamp endTime) {
        if ( (currentTime.after(*this)) && (endTime.after(currentTime)) ) {
            return(((double)currentTime.subUsec(*this)) /
                   ((double)endTime.subUsec(*this)));
        } else {
            return -1.0;
        }
    }


protected:
    enum {
        kMillion = 1000000
    };

    struct timeval mTime;

}; // end class Timestamp

#endif // TIMESTAMP_H
