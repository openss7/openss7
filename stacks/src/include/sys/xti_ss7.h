/*****************************************************************************

 @(#) $Id: xti_ss7.h,v 0.9 2004/01/17 08:08:43 brian Exp $

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

#ifndef _SYS_XTI_SS7_H
#define _SYS_XTI_SS7_H

#ident "@(#) $Name:  $($Revision: 0.9 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#define T_SS7_SS7		0	/* Common to all SS7 Levels */

#define T_SS7_PVAR		1	/* protocol variant */
#define T_SS7_MPLEV		2	/* protocol option */
#define T_SS7_DEBUG		3	/* debug */
#define T_SS7_PCR		4	/* protocol option */
#define T_SS7_HSL		5	/* protocol option */
#define T_SS7_XSN		6	/* protocol option */
#define T_SS7_CLUSTER		7	/* protocol options */
#define T_SS7_SEQ_CTRL		8	/* sequence control parameter */
#define T_SS7_PRIORITY		9	/* message priority */
#define T_SS7_PCLASS		10	/* protocol class */
#define T_SS7_IMPORTANCE	11	/* importance */
#define T_SS7_RET_ERROR		12	/* return on error */

/*
 *  PROTOCOL VARIANTS
 *
 *  Note: these values must be aligned with <ss7/lmi_ioctl.h>
 */
#define T_SS7_PVAR_ITUT88	0x0100
#define T_SS7_PVAR_ITUT93	0x0101
#define T_SS7_PVAR_ITUT96	0x0102
#define T_SS7_PVAR_ITUT00	0x0103
#define T_SS7_PVAR_ETSI88	0x0200
#define T_SS7_PVAR_ETSI93	0x0201
#define T_SS7_PVAR_ETSI96	0x0202
#define T_SS7_PVAR_ETSI00	0x0203
#define T_SS7_PVAR_ANSI88	0x0300
#define T_SS7_PVAR_ANSI92	0x0301
#define T_SS7_PVAR_ANSI96	0x0302
#define T_SS7_PVAR_ANSI00	0x0303
#define T_SS7_PVAR_JTTC94	0x0400

#define T_SS7_POPT_MPLEV	0x0001	/* multiple priority levels */
#define T_SS7_POPT_PCR		0x0002	/* preventative cyclic retransmission */
#define T_SS7_POPT_HSL		0x0004	/* high speed links */
#define T_SS7_POPT_XSN		0x0008	/* extended sequence numbers */
#define T_SS7_POPT_CLUSTER	0x0010	/* cluster routing */
#define T_SS7_POPT_DEBUG	0x0020	/* debug */

#endif				/* _SYS_XTI_SS7_H */
