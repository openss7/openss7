/*****************************************************************************

 @(#) $Id: xti_ip.h,v 0.9 2004/04/05 12:37:53 brian Exp $

 -----------------------------------------------------------------------------

     Copyright (C) 1997-2004 OpenSS7 Corporation.  All Rights Reserved.

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/04/05 12:37:53 $ by $Author: brian $

 *****************************************************************************/

#ifndef _XTI_IP_H
#define _XTI_IP_H

#ident "@(#) $RCSfile: xti_ip.h,v $ $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2004 OpenSS7 Corporation."

#ifdef __BEGIN_DECLS
/* *INDENT-OFF* */
__BEGIN_DECLS
/* *INDENT-ON* */
#endif

/*
 * IP level
 */
#define T_INET_IP		0	/* IP level (same as protocol number) */

/*
 * IP level Options
 */
#define T_IP_OPTIONS		1	/* IP per-packet options */
#define T_IP_TOS		2	/* IP per-packet type of service */
#define T_IP_TTL		3	/* IP per-packet time to live */
#define T_IP_REUSEADDR		4	/* allow local address reuse */
#define T_IP_DONTROUTE		5	/* just use interface addresses */
#define T_IP_BROADCAST		6	/* permit sending of broadcast msgs */

/*
 *  IP_TOS precedence levels
 */
#define T_ROUTINE		0
#define T_PRIORITY		1
#define T_IMMEDIATE		2
#define T_FLASH			3
#define T_OVERRIDEFLASH		4
#define T_CRITIC_ECP		5
#define T_INETCONTROL		6
#define T_NETCONTROL		7

/*
 *  IP_TOS type of service
 */
#define T_NOTOS			0
#define T_LDELAY		(1<<4)
#define T_HITHRPT		(1<<3)
#define T_HIREL			(1<<2)
#define T_LOCOST		(1<<1)

#define SET_TOS(prec, tos) \
	(((0x7 & (prec)) << 5) | ((T_NOTOS|T_LDELAY|T_HITHRPT|T_HIREL|T_LOCOST) & (tos)))

#ifdef __END_DECLS
/* *INDENT-OFF* */
__END_DECLS
/* *INDENT-ON* */
#endif

#endif				/* _XTI_IP_H */
