/*****************************************************************************

 @(#) $Id: tpi_sccp.h,v 0.9.2.2 2005/05/14 08:30:49 brian Exp $

 -----------------------------------------------------------------------------

     Copyright (C) 1997-2002 OpenSS7 Corporation.  All Rights Reserved.

                                  PUBLIC LICENSE

     This license is provided without fee, provided that the above copy-
     right notice and this public license must be retained on all copies,
     extracts, compilations and derivative works.  Use or distribution of
     this work in a manner that restricts its use except as provided here
     will render this license void.

     The author(s) hereby waive any and all other restrictions in respect
     of their copyright in this software and its associated documentation.
     The authors(s) of this software place in the public domain any novel
     methods or processes which are embodied in this software.

     The author(s) undertook to write it for the sake of the advancement
     of the Arts and Sciences, but it is provided as is, and the author(s)
     will not take any responsibility in it.

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

 Last Modified $Date: 2005/05/14 08:30:49 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_TPI_SCCP_H
#define _SYS_TPI_SCCP_H

#ident "@(#) $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#ifndef T_CURRENT_VERSION

#define T_CURRENT_VERSION 5

/*
 *  Missing from LiS tihdr.h
 */
#ifndef T_MORE
#define T_MORE		0x0001
#endif
#ifndef T_EXPEDITED
#define T_EXPEDITED	0x0002
#endif
#ifndef T_ODF_MORE
#define T_ODF_MORE	T_MORE
#endif
#ifndef T_ODF_EX
#define T_ODF_EX	T_EXPEDITED
#endif
#ifndef XPG4_1
#define XPG4_1		2	/* This indicates that the transport provider conforms to XTI in XPG4 and
				   supports the new primitives T_ADDR_REQ and T_ADDR_ACK */
#endif

/*
 *  Errors in LiS tihdr.h
 */
#define T_optdata_ind T_optdata_ack
#define optdata_ind optdata_ack

#endif				/* T_CURRENT_VERSION */

#endif				/* _SYS_TPI_SCCP_H */
