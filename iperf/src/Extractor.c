/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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
 * Extractor.cpp
 * by Ajay Tirumala (tirumala@ncsa.uiuc.edu)
 * -------------------------------------------------------------------
 * Extract data from a file, used to measure the transfer rates
 * for various stream formats. 
 *
 * E.g. Use a gzipped file to measure the transfer rates for 
 * compressed data
 * Use an MPEG file to measure the transfer rates of 
 * Multimedia data formats
 * Use a plain BMP file to measure the transfer rates of 
 * Uncompressed data
 *
 * This is beneficial especially in measuring bandwidth across WAN
 * links where data compression takes place before data transmission 
 * ------------------------------------------------------------------- 
 */

#include "Extractor.h"


/**
 * Constructor
 * @arg fileName   Name of the file 
 * @arg size       Block size for reading
 * Open the file and set the block size
 */
void Extractor_Initialize ( char *fileName, int inSize, thread_Settings *mSettings ) {

    if ( (mSettings->Extractor_file = fopen (fileName, "rb")) == NULL ) {
        fprintf( stderr, "Unable to open the file stream\n");
        fprintf( stderr, "Will use the default data stream\n");
        return;
    }
    mSettings->Extractor_size =  inSize;
}


/**
 * Constructor
 * @arg fp         File Pointer 
 * @arg size       Block size for reading
 * Set the block size,file pointer
 */
void Extractor_InitializeFile ( FILE *fp, int inSize, thread_Settings *mSettings ) {
    mSettings->Extractor_file = fp;
    mSettings->Extractor_size =  inSize;
}


/**
 * Destructor - Close the file
 */
void Extractor_Destroy ( thread_Settings *mSettings ) {
    if ( mSettings->Extractor_file != NULL )
        fclose( mSettings->Extractor_file );
}


/*
 * Fetches the next data block from 
 * the file
 * @arg block     Pointer to the data read
 * @return        Number of bytes read
 */
int Extractor_getNextDataBlock ( char *data, thread_Settings *mSettings ) {
    if ( Extractor_canRead( mSettings ) ) {
        return(fread( data, 1, mSettings->Extractor_size, 
                      mSettings->Extractor_file ));
    }
    return 0;
}

/**
 * Function which determines whether
 * the file stream is still readable
 * @return boolean    true, if readable; false, if not
 */
int Extractor_canRead ( thread_Settings *mSettings ) {
    return(( mSettings->Extractor_file != NULL ) 
           && !(feof( mSettings->Extractor_file )));
}

/**
 * This is used to reduce the read size
 * Used in UDP transfer to accomodate the
 * the header (timestamp)
 * @arg delta         Size to reduce
 */
void Extractor_reduceReadSize ( int delta, thread_Settings *mSettings ) {
    mSettings->Extractor_size -= delta;
}



















































