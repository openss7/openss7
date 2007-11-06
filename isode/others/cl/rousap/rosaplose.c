/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

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

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/* rosaplose.c - ROPM: you lose */

#ifndef	lint
static char *rcsid = "Header: /f/iso/rosap/RCS/rosaplose.c,v 5.0 88/07/21 14:56:12 mrose Rel";
#endif

/* 
 * Header: /f/iso/rosap/RCS/rosaplose.c,v 5.0 88/07/21 14:56:12 mrose Rel
 *
 * Based on an TCP-based implementation by George Michaelson of University
 * College London.
 *
 *
 * $Log$
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

/* LINTLIBRARY */

#include <stdio.h>
#include <varargs.h>
#include "ropkt.h"

/*  */

#ifndef	lint
int
ropktlose(va_alist)
	va_dcl
{
	int reason, result, value;
	register struct assocblk *acb;
	register struct RoSAPindication *roi;
	va_list ap;

	va_start(ap);

	acb = va_arg(ap, struct assocblk *);
	roi = va_arg(ap, struct RoSAPindication *);
	reason = va_arg(ap, int);

	result = _rosaplose(roi, reason, ap);

	va_end(ap);

#ifdef HULA
	return result;
#else
	if (acb == NULLACB || acb->acb_fd == NOTOK || acb->acb_ropktlose == NULLIFP)
		return result;

	switch (reason) {
	case ROS_PROTOCOL:
		value = ABORT_PROTO;
		break;

	case ROS_CONGEST:
		value = ABORT_TMP;
		break;

	default:
		value = ABORT_LSP;
		break;
	}

	(*acb->acb_ropktlose) (acb, value);

	return result;
#endif
}

#else
/* VARARGS */

int
ropktlose(acb, roi, reason, what, fmt)
	struct assocblk *acb;
	struct RoSAPindication *roi;
	int reason;
	char *what, *fmt;
{
	return ropktlose(acb, roi, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
int
rosapreject(va_alist)
	va_dcl
{
	int reason, result;
	register struct assocblk *acb;
	struct RoSAPindication rois;
	register struct RoSAPindication *roi;
	va_list ap;

	va_start(ap);

	acb = va_arg(ap, struct assocblk *);
	roi = va_arg(ap, struct RoSAPindication *);
	reason = va_arg(ap, int);

	result = _rosaplose(roi, reason, ap);

	va_end(ap);

	if (RoURejectRequestAux(acb, NULLIP, reason - REJECT_GENERAL_BASE,
				REJECT_GENERAL, 0, &rois) == NOTOK
	    && ROS_FATAL(rois.roi_preject.rop_reason)) {
		*roi = rois;	/* struct copy */
		result = NOTOK;
	}

	return result;
}
#else
/* VARARGS */

int
rosapreject(acb, roi, reason, what, fmt)
	struct assocblk *acb;
	struct RoSAPindication *roi;
	int reason;
	char *what, *fmt;
{
	return rosapreject(acb, roi, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
int
rosaplose(va_alist)
	va_dcl
{
	int reason, result;
	struct RoSAPindication *roi;

	va_list(ap);

	va_start(ap);

	roi = va_arg(ap, struct RoSAPindication *);
	reason = va_arg(ap, int);

	result = _rosaplose(roi, reason, ap);

	va_end(ap);

	return result;
}
#else
/* VARARGS */

int
rosaplose(roi, reason, what, fmt)
	struct RoSAPindication *roi;
	int reason;
	char *what, *fmt;
{
	return rosaplose(roi, reason, what, fmt);
}
#endif

/*  */

#ifndef	lint
static int
_rosaplose(roi, reason, ap)		/* what, fmt, args ... */
	register struct RoSAPindication *roi;
	int reason;
	va_list ap;
{
	register char *bp;
	char buffer[BUFSIZ];
	register struct RoSAPpreject *rop;

	if (roi) {
		bzero((char *) roi, sizeof *roi);
		roi->roi_type = ROI_PREJECT;
		rop = &roi->roi_preject;

		asprintf(bp = buffer, ap);
		bp += strlen(bp);

		rop->rop_reason = reason;
		copyRoSAPdata(buffer, bp - buffer, rop);
	}

	return NOTOK;
}
#endif
