/* dgram.h - datagram (CL-mode TS) abstractions */

/* 
 * $Header: /xtel/isode/isode/h/RCS/dgram.h,v 9.0 1992/06/16 12:17:57 isode Rel $
 *
 *
 * $Log: dgram.h,v $
 * Revision 9.0  1992/06/16  12:17:57  isode
 * Release 8.0
 *
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


#ifndef	_DGRAM_
#define	_DGRAM_

#define	MAXDGRAM	8192


int	join_dgram_aux ();
int	read_dgram_socket ();
int	write_dgram_socket ();
int	close_dgram_socket ();
int	select_dgram_socket ();
int	check_dgram_socket ();

#endif
