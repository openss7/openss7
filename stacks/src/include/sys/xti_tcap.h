/*****************************************************************************

 @(#) $Id: xti_tcap.h,v 0.9.2.1 2004/08/21 10:14:40 brian Exp $

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

 Last Modified $Date: 2004/08/21 10:14:40 $ by $Author: brian $

 *****************************************************************************/

#ifndef _SYS_XTI_TCAP_H
#define _SYS_XTI_TCAP_H

#ident "@(#) $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 1997-2002 OpenSS7 Corporation."

#define T_SS7_TCAP		0x13	/* TCAP level (same as SI value) */

/*
 *  TCAP Transport Provider Options
 */
#define T_TCAP_PVAR		1	/* protocol variant */
#define T_TCAP_MPLEV		2	/* protocol options */
#define T_TCAP_DEBUG		3	/* debug */

#define T_TCAP_SEQ_CTRL		8	/* sequence control parameter */
#define T_TCAP_PRIORITY		9	/* message priority */

#define T_TCAP_PCLASS		10	/* protocol class */
#define T_TCAP_IMPORTANCE	11	/* importance */
#define T_TCAP_RET_ERROR	12	/* return on error */

#define T_TCAP_PCLASS_1		0x01
#define T_TCAP_PCLASS_2		0x02
#define T_TCAP_PCLASS_3		0x04
#define T_TCAP_PCLASS_4		0x08
#define T_TCAP_PCLASS_5		0x10
#define T_TCAP_PCLASS_ALL	0x1f

#define T_TCAP_CLUST			(0x0100)
#define T_TCAP_DEST_AVAILABLE		(0x1001)
#define T_TCAP_DEST_PROHIBITED		(0x1002)
#define T_TCAP_DEST_CONGESTED		(0x1003)
#define T_TCAP_CLUS_AVAILABLE		(T_TCAP_DEST_AVAILABLE |T_TCAP_CLUST)
#define T_TCAP_CLUS_PROHIBITED		(T_TCAP_DEST_PROHIBITED|T_TCAP_CLUST)
#define T_TCAP_CLUS_CONGESTED		(T_TCAP_DEST_CONGESTED |T_TCAP_CLUST)
#define T_TCAP_RESTARTING		(0x1004)
#define T_TCAP_USER_PART_UNKNOWN	(0x1005)
#define T_TCAP_USER_PART_UNEQUIPPED	(0x1006)
#define T_TCAP_USER_PART_UNAVAILABLE	(0x1007)

#endif				/* _SYS_XTI_TCAP_H */
