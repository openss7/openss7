/************************************************************************
*									*
*                            LiS Putbuf                                 *
*									*
*************************************************************************
*									*
*     Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>	*
*									*
*************************************************************************
*									*
* This file contains the code for the Rsystem "putbuf" mechanism.	*
* This mechanism is a way of capturing debug print strings in a buffer	*
* and delivering the data to the user for printing or logging.		*
*									*
************************************************************************/
/*
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 */

#ident "@(#) LiS sputbuf.c 2.5 2/8/01 21:24:01 "

#include <sys/stream.h>
#include <sys/cmn_err.h>
#include <sys/lislocks.h>
#include <sys/osif.h>

/************************************************************************
*                      Storage Declarations                             *
************************************************************************/

static char		*lis_strm_in ;		/* in pointer */
static char		*lis_strm_out ;		/* out pointer */
static char		*lis_strm_buf ;		/* pointer to buffer */
static char		*lis_strm_end ;		/* pointer to one off end of bfr */
static long		 lis_strm_size ;	/* putbuf size */
static long		 lis_strm_max_bytes ;	/* max bytes in buffer */

static lis_spin_lock_t	 putbuf_lock ;


#define	USER_BUF	1		/* user's buffer */

#ifndef NULL
#define NULL		( (void *) 0 )
#endif

char         lis_strm_print_trace_buf[1024];


/************************************************************************
*                       lis_strm_print_trace                            *
*************************************************************************
*									*
* This routine is suitable for use as an Rsys_print_trace function.	*
*									*
* The technique is to put characters into the putbuf until the input	*
* pointer catches the output pointer.  If the input pointer catches	*
* the output pointer then we increment the output pointer so that	*
* the buffer appears to hold the most recent lis_strm_size entries.	*
*									*
* This routine may be called from interrupt routines.  It is assumed	*
* that its execution may interrupt the execution of the lis_strm_readbuf*
* routine below but not vice versa.					*
*									*
************************************************************************/
void	lis_strm_print_trace(char *msg)
{
    char	c ;
    int		psw ;

    if (lis_strm_in == NULL)
    {
	return ;	/* cheap insurance */
    }

    lis_spin_lock_irqsave(&putbuf_lock, &psw) ;

    while ( (c = *msg++) != 0 )
    {
	if ( (c >= ' ' && c < 0x7F ) || c == '\n' )
	    *lis_strm_in++ = c ;		/* char to buffer */
	else
	    continue;

	if (lis_strm_in == lis_strm_end)  /* need to wrap around in ptr?  */
	    lis_strm_in = lis_strm_buf ;  /* wrap lis_strm_in to start of bfr */

	if (   lis_strm_in == lis_strm_out	/* buffer full */
	    && ++lis_strm_out == lis_strm_end	/* lis_strm_out need to wrap */
	   )
	    lis_strm_out = lis_strm_buf ;	/* wrap lis_strm_out to start of bfr */
    }

    lis_spin_unlock_irqrestore(&putbuf_lock, &psw) ;

} /* lis_strm_print_trace */

/************************************************************************
*                           lis_strm_putc                               *
*************************************************************************
*									*
* This routine can be called by Gputc to funnel that character stream	*
* into the putbuf mechanism.						*
*									*
************************************************************************/
void	lis_strm_putc(char c)
{
    char	chrs[2] ;

    chrs[0] = c ;			/* make a one-char string */
    chrs[1] = 0 ;

    lis_strm_print_trace(chrs) ;		/* funnel through putbuf */

} /* lis_strm_putc */

/************************************************************************
*                          lis_strm_readbuf                             *
*************************************************************************
*									*
* Read characters out of the putbuf into the user's data area.		*
*									*
* This routine could be interrupted by the execution of the		*
* lis_strm_print trace routine.  Therefore, we protect this routine     *
* with interrupt enables and disables.					*
*									*
* Return the number of bytes transferred.				*
*									*
************************************************************************/
long	lis_strm_readbuf(char *usr_ptr, long max_bytes)
{
    int 	psw ;
    long	bytes_in_bfr ;
    long	bytes_to_end ;
    long	bytes_from_front ;
    static int	total_bytes;

    if (lis_strm_size <= 0 || lis_strm_in == NULL || lis_strm_out == NULL)
    {
	return(0) ;
    }

    lis_spin_lock_irqsave(&putbuf_lock, &psw) ;
    bytes_in_bfr = (lis_strm_size + (lis_strm_in - lis_strm_out)) % lis_strm_size ;

    if (bytes_in_bfr > lis_strm_max_bytes)		/* keep running max */
	lis_strm_max_bytes = bytes_in_bfr ;

    if (max_bytes > bytes_in_bfr)		/* we will xfer max_bytes */
	max_bytes = bytes_in_bfr ;

    bytes_to_end = lis_strm_end - lis_strm_out ;/* bytes to end of buffer */

    if (bytes_to_end > max_bytes)	/* won't get to end of bfr */
	bytes_to_end = max_bytes ;	/* take it all in one segment */

    lis_copyout(0, lis_strm_out, usr_ptr, bytes_to_end) ;/* move to user's buffer */
    lis_strm_out += bytes_to_end ;	/* advance lis_strm_out towards end */

    bytes_from_front = max_bytes - bytes_to_end ;	/* wrap around?  */

    if (bytes_from_front > 0)
    {						/* move from front of bfr */
	lis_copyout(0, lis_strm_buf, usr_ptr + bytes_to_end, bytes_from_front) ;
	lis_strm_out = lis_strm_buf + bytes_from_front ; /* set lis_strm_out */
    }

    total_bytes += max_bytes;
    lis_spin_unlock_irqrestore(&putbuf_lock, &psw) ;
    return(max_bytes) ;

} /* lis_strm_reabuf */

/************************************************************************
*                          lis_strm_init                                *
*************************************************************************
*									*
* Initialize putbuf mechanism.						*
*									*
************************************************************************/
int	lis_strm_init(char *bfr, long	size)
{

    lis_strm_size = 0 ;			/* putbuf size */
    lis_strm_in = NULL ;		/* in pointer */
    lis_strm_out = NULL ;		/* out pointer */
    lis_strm_end = NULL ;		/* pointer to one off end of bfr */

    if (size <= 0) return(-1) ;		/* de-initialize */

    if (bfr == NULL)
    {
	return(-1);
    }
    else
    {
	lis_strm_buf = bfr ;
    }

    lis_spin_lock_init(&putbuf_lock, "PutBuf-Lock") ;

    lis_strm_size = size ;			/* putbuf size */
    lis_strm_in = lis_strm_buf ;		/* in pointer */
    lis_strm_out = lis_strm_buf ;		/* out pointer */
    lis_strm_end = lis_strm_buf + lis_strm_size ;/* pointer to one off end of bfr */

    if ( lis_print_trace == NULL )
    {
        lis_print_trace = lis_strm_print_trace ;
    }
    
    return(0) ;

} /* lis_strm_init */
