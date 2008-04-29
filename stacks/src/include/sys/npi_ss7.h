/*****************************************************************************

 @(#) $Id: npi_ss7.h,v 0.9.2.4 2008-04-29 07:10:46 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-04-29 07:10:46 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: npi_ss7.h,v $
 Revision 0.9.2.4  2008-04-29 07:10:46  brian
 - updating headers for release

 *****************************************************************************/

#ifndef _SYS_NPI_SS7_H
#define _SYS_NPI_SS7_H

#ident "@(#) $Name:  $($Revision: 0.9.2.4 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/*
 *  PROTOCOL VARIANTS
 *
 *  Note: these values must be aligned with <ss7/lmi_ioctl.h>
 */
#define N_SS7_PVAR_ITUT	0x0100
#define N_SS7_PVAR_ETSI	0x0200
#define N_SS7_PVAR_ANSI	0x0300
#define N_SS7_PVAR_JTTC	0x0400
#define N_SS7_PVAR_CHIN	0x0500
#define N_SS7_PVAR_SING	0x0600
#define N_SS7_PVAR_SPAN	0x0700
#define N_SS7_PVAR_MASK	0xff00

#define N_SS7_PVAR_88	0x0000
#define N_SS7_PVAR_92	0x0001
#define N_SS7_PVAR_96	0x0002
#define N_SS7_PVAR_00	0x0003
#define N_SS7_PVAR_YR	0x00ff

enum {
	N_SS7_PVAR_ITUT_88 = N_SS7_PVAR_ITUT | N_SS7_PVAR_88,	/* ITU-T protocol variant */
	N_SS7_PVAR_ITUT_93 = N_SS7_PVAR_ITUT | N_SS7_PVAR_92,	/* ITU-T protocol variant */
	N_SS7_PVAR_ITUT_96 = N_SS7_PVAR_ITUT | N_SS7_PVAR_96,	/* ITU-T protocol variant */
	N_SS7_PVAR_ITUT_00 = N_SS7_PVAR_ITUT | N_SS7_PVAR_00,	/* ITU-T protocol variant */
	N_SS7_PVAR_ETSI_88 = N_SS7_PVAR_ETSI | N_SS7_PVAR_88,	/* ETSI protocol variant */
	N_SS7_PVAR_ETSI_93 = N_SS7_PVAR_ETSI | N_SS7_PVAR_92,	/* ETSI protocol variant */
	N_SS7_PVAR_ETSI_96 = N_SS7_PVAR_ETSI | N_SS7_PVAR_96,	/* ETSI protocol variant */
	N_SS7_PVAR_ETSI_00 = N_SS7_PVAR_ETSI | N_SS7_PVAR_00,	/* ETSI protocol variant */
	N_SS7_PVAR_ANSI_88 = N_SS7_PVAR_ANSI | N_SS7_PVAR_88,	/* ANSI protocol variant */
	N_SS7_PVAR_ANSI_92 = N_SS7_PVAR_ANSI | N_SS7_PVAR_92,	/* ANSI protocol variant */
	N_SS7_PVAR_ANSI_96 = N_SS7_PVAR_ANSI | N_SS7_PVAR_96,	/* ANSI protocol variant */
	N_SS7_PVAR_ANSI_00 = N_SS7_PVAR_ANSI | N_SS7_PVAR_00,	/* ANSI protocol variant */
	N_SS7_PVAR_JTTC_94 = N_SS7_PVAR_JTTC | N_SS7_PVAR_92,	/* JTTC protocol variant */
	N_SS7_PVAR_CHIN_00 = N_SS7_PVAR_CHIN | N_SS7_PVAR_00,	/* CHIN protocol variant */
};

enum {
	N_SS7_POPT_MPLEV = 0x00000001,	/* multiple priority/congestion levels */
	N_SS7_POPT_PCR = 0x00000002,	/* preventative cyclic retranlmission */
	N_SS7_POPT_HSL = 0x00000004,	/* high speed links */
	N_SS7_POPT_XSN = 0x00000008,	/* extended sequence numbers */
	N_SS7_POPT_ALL = 0x0000000f	/* all options */
};

#endif				/* _SYS_NPI_SS7_H */
