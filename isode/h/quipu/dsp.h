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

#ifndef __ISODE_QUIPU_DSP_H__
#define __ISODE_QUIPU_DSP_H__

#ident "@(#) $RCSfile$ $Name$($Revision$) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* dsp.h - a few DSP things */

/*
 * Header: /xtel/isode/isode/h/quipu/RCS/dsp.h,v 9.0 1992/06/16 12:23:11 isode Rel
 *
 *
 * Log: dsp.h,v
 * Revision 9.0  1992/06/16  12:23:11  isode
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

#ifndef DSP
#define DSP

#include "isoaddrs.h"
#include "quipu/name.h"

struct op_progress {			/* represents OperationProgress */
	int op_resolution_phase;
#define OP_PHASE_NOTDEFINED     -1
#define OP_PHASE_NOTSTARTED     1
#define OP_PHASE_PROCEEDING     2
#define OP_PHASE_COMPLETED      3
	int op_nextrdntoberesolved;
};

struct access_point {			/* represents AccessPoint */
	DN ap_name;
	struct PSAPaddr *ap_address;
	/* from ISODE */
	/* In INCA, this may be left out */
	struct access_point *ap_next;
};

#define NULLACCESSPOINT ((struct access_point *) NULL)

				/* Continuation Ref definded in DSP */
				/* represents ContinuationReference */
typedef struct continuation_ref {
	DN cr_name;
	struct op_progress cr_progress;
	int cr_rdn_resolved;
#define CR_RDNRESOLVED_NOTDEFINED       -1
	int cr_aliasedRDNs;
#define CR_NOALIASEDRDNS -1
	int cr_reftype;
#define RT_UNDEFINED    -1
#define RT_SUPERIOR     1
#define RT_SUBORDINATE  2
#define RT_CROSS        3
#define RT_NONSPECIFICSUBORDINATE       4
	struct access_point *cr_accesspoints;
	struct continuation_ref *cr_next;
	/* for chaining Continuation Refs */
	/* They usually occur in SETs */
} continuation_ref, *ContinuationRef;

#define NULLCONTINUATIONREF ((ContinuationRef) 0)

struct trace_info {
	DN ti_target;
	DN ti_dsa;
	struct op_progress ti_progress;
	struct trace_info *ti_next;
};

#define NULLTRACEINFO ((struct trace_info *) 0)
struct trace_info *ti_cpy();

	/* THIS SECTION GIVES VARIOUS COMMON STRUCTURES */

typedef struct svccontrol {		/* represents ServiceControls */
	int svc_options;
#define SVC_OPT_PREFERCHAIN             0X001
#define SVC_OPT_CHAININGPROHIBIT        0X002
#define SVC_OPT_LOCALSCOPE              0X004
#define SVC_OPT_DONTUSECOPY             0X008
#define SVC_OPT_DONTDEREFERENCEALIAS    0X010
	int svc_prio;
#define SVC_PRIO_LOW    0
#define SVC_PRIO_MED    1
#define SVC_PRIO_HIGH   2
	int svc_timelimit;		/* time limit in second */
#define SVC_NOTIMELIMIT -1
	int svc_sizelimit;
#define SVC_NOSIZELIMIT -1
	int svc_scopeofreferral;	/* Parameter for DSP only */
#define SVC_REFSCOPE_NONE       -1
#define SVC_REFSCOPE_DMD        0
#define SVC_REFSCOPE_COUNTRY    1
	char *svc_tmp;			/* pepsy */
	int svc_len;			/* pepsy */
} svccontrol, ServiceControl;

#endif

#endif				/* __ISODE_QUIPU_DSP_H__ */
