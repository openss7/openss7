/*****************************************************************************

 @(#) $Id$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

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

#ifndef __ISODE_RONOT_H__
#define __ISODE_RONOT_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* ronote.h - Additions to properly support ABSTRACT-BIND */

/* 
 * Header: /xtel/isode/isode/h/RCS/ronot.h,v 9.0 1992/06/16 12:17:57 isode Rel
 *
 *
 * Log: ronot.h,v
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

#ifndef	_RoNot_
#define	_RoNot_

#ifndef	_MANIFEST_
#include "manifest.h"
#endif
#ifndef	_GENERAL_
#include "general.h"
#endif

#ifndef	_AcSAP_
#include "acsap.h"		/* definitions for AcS-USERs */
#endif

#ifndef	_RoSAP_
#include "rosap.h"		/* definitions for RoS-USERs */
#endif

#define BIND_RESULT	1	/* indicates a bind result occured */
#define BIND_ERROR	2	/* indicates a bind error occured */

struct RoNOTindication {
	int rni_reason;			/* reason for failure */
#define RBI_ACSE		1	/* ACSE provider failed */
#define RBI_SET_ROSE_PRES	2	/* Failed to set ROS-USER */
#define RBI_ENC_BIND_ARG	3	/* Failed encoding bind argument */
#define RBI_ENC_BIND_RES	4	/* Failed encoding bind result */
#define RBI_ENC_BIND_ERR	5	/* Failed encoding bind error */
#define RBI_ENC_UNBIND_ARG	6	/* Failed encoding unbind argument */
#define RBI_ENC_UNBIND_RES	7	/* Failed encoding unbind result */
#define RBI_ENC_UNBIND_ERR	8	/* Failed encoding unbind error */
#define RBI_DEC_BIND_ARG	9	/* Failed decoding bind argument */
#define RBI_DEC_BIND_RES	10	/* Failed decoding bind result */
#define RBI_DEC_BIND_ERR	11	/* Failed decoding bind error */
#define RBI_DEC_UNBIND_ARG	12	/* Failed decoding unbind argument */
#define RBI_DEC_UNBIND_RES	13	/* Failed decoding unbind result */
#define RBI_DEC_UNBIND_ERR	14	/* Failed decoding unbind error */
#define RBI_DEC_NINFO		15	/* Erroneous number of user infos */

	/* diagnostics from provider */
#define	RB_SIZE	512
	int rni_cc;			/* length */
	char rni_data[RB_SIZE];		/* data */
};

#ifndef	lint
#ifndef	__STDC__
#define	copyRoNOTdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d/**/_cc = min (i, sizeof d -> d/**/_data)) > 0) \
	bcopy (base, d -> d/**/_data, d -> d/**/_cc); \
}
#else
#define	copyRoNOTdata(base,len,d) \
{ \
    register int i = len; \
    if ((d -> d##_cc = min (i, sizeof d -> d##_data)) > 0) \
	bcopy (base, d -> d##_data, d -> d##_cc); \
}
#endif
#else
#define	copyRoNOTdata(base,len,d)	bcopy (base, (char *) d, len)
#endif

#endif

#endif				/* __ISODE_RONOT_H__ */
