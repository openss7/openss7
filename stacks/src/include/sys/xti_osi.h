/*****************************************************************************

 @(#) $RCSfile: xti_osi.h,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2004/05/12 08:01:39 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2004  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This library is free software; you can redistribute it and/or modify it under
 the terms of the GNU Lesser General Public License as published by the Free
 Software Foundation; either version 2.1 of the License, or (at your option)
 any later version.

 This library is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Lesser Public License for more
 details.

 You should have received a copy of the GNU Lesser General Public License
 along with this library; if not, write to the Free Software Foundation, Inc.,
 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

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

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date: 2004/05/12 08:01:39 $ by $Author: brian $

 $Log: xti_osi.h,v $
 Revision 0.9.2.1  2004/05/12 08:01:39  brian
 - Added in xti library and STREAMS modules.

 Revision 1.1.2.1  2004/04/13 12:12:52  brian
 - Rearranged header files.

 *****************************************************************************/

#ifndef _SYS_XTI_OSI_H
#define _SYS_XTI_OSI_H

#ident "@(#) $RCSfile: xti_osi.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 1997-2004 OpenSS7 Corporation."

/*
   SPECIFIC ISO OPTION AND MANAGEMENT PARAMETERS 
 */

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
