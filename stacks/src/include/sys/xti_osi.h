/*****************************************************************************

 @(#) $Id: xti_osi.h,v 0.9 2004/01/17 08:08:43 brian Exp $

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
 Federal Acquisition Regulations ("FAR") (or any success regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/01/17 08:08:43 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_XTI_OSI_H
#define _SYS_XTI_OSI_H

#ident "@(#) $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

/* SPECIFIC ISO OPTION AND MANAGEMENT PARAMETERS */

/*
 * Definition of the ISO transport classes
 */

#define T_CLASS0	0
#define T_CLASS1	1
#define T_CLASS2	2
#define T_CLASS3	3
#define T_CLASS4	4

/*
 * Definition of the priorities.
 */

#define T_PRITOP	0
#define T_PRIHIGH	1
#define T_PRIMID	2
#define T_PRILOW	3
#define T_PRIDFLT	4

/*
 * Definitions of the protection levels
 */
#define T_NOPROTECT		1
#define T_PASSIVEPROTECT	2
#define T_ACTIVEPROTECT		4

/*
 * rate structure.
 */
struct rate {
	t_scalar_t targetvalue;		/* target value */
	t_scalar_t minacceptvalue;	/* value of minimum acceptable quality */
};

/*
 * reqvalue structure.
 */
struct reqvalue {
	struct rate called;		/* called rate */
	struct rate calling;		/* calling rate */
};

/*
 * thrpt structure.
 */
struct thrpt {
	struct reqvalue maxthrpt;	/* maximum throughput */
	struct reqvalue avgthrpt;	/* average throughput */
};

/*
 * transdel structure
 */
struct transdel {
	struct reqvalue maxdel;		/* maximum transit delay */
	struct reqvalue avgdel;		/* average transit delay */
};

#define T_ISO_TP	0x0100

/*
 *  Options for Quality of Service and Expedited Data (ISO 8072:1994)
 */
#define T_TCO_THROUGHPUT	0x0001
#define T_TCO_TRANSDEL		0x0002
#define T_TCO_RESERRORRATE	0x0003
#define T_TCO_TRANSFFAILPROB	0x0004
#define T_TCO_ESTFAILPROB	0x0005
#define T_TCO_RELFAILPROB	0x0006
#define T_TCO_ESTDELAY		0x0007
#define T_TCO_RELDELAY		0x0008
#define T_TCO_CONNRESIL		0x0009
#define T_TCO_PROTECTION	0x000a
#define T_TCO_PRIORITY		0x000b
#define T_TCO_EXPD		0x000c

/*
 *  Management Options
 */
#define T_TCO_LTPDU		0x0100
#define T_TCO_ACKTIME		0x0200
#define T_TCO_REASTIME		0x0300
#define T_TCO_EXTFORM		0x0400
#define T_TCO_FLOWCTRL		0x0500
#define T_TCO_CHECKSUM		0x0600
#define T_TCO_NETEXP		0x0700
#define T_TCO_NETRECPTCF	0x0800
#define T_TCO_PREFCLASS		0x0900
#define T_TCO_ALTCLASS1		0x0a00
#define T_TCO_ALTCLASS2		0x0b00
#define T_TCO_ALTCLASS3		0x0c00
#define T_TCO_ALTCLASS4		0x0d00

#define T_TCL_TRANSDEL		0x000d
#define T_TCL_RESERRORRATE	T_TCO_RESERRORRATE
#define T_TCL_PROTECTION	T_TCO_PROTECTION
#define T_TCL_PRIORITY		T_TCO_PRIORITY
#define T_TCL_CHECKSUM		T_TCO_CHECKSUM

#endif				/* _SYS_XTI_OSI_H */
