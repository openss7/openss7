/*****************************************************************************

 @(#) $RCSfile: m3ua_msg.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 01:52:21 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

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

 Last Modified $Date: 2008-04-29 01:52:21 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: m3ua_msg.c,v $
 Revision 0.9.2.4  2008-04-29 01:52:21  brian
 - updated headers for release

 Revision 0.9.2.3  2007/08/14 08:33:53  brian
 - GPLv3 header update

 *****************************************************************************/

#ident "@(#) $RCSfile: m3ua_msg.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 01:52:21 $"

static char const ident[] =
    "$RCSfile: m3ua_msg.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 01:52:21 $";

/*
 *  -------------------------------------------------------------------------
 *
 *  General purpose parameter decoder
 *
 *  -------------------------------------------------------------------------
 *
 *  General purpose parameter extractor for UA messages:  Given the pointer to
 *  the parameter, this will generate a parameter array which indexes messages
 *  of each type and provides their length.
 */

int
m3ua_decode_parms(mp, m3ua_results)
	mblk_t *mp;
	m3ua_parms_t *m3ua_results;
{
	int len;
	uint32_t *p = (uint32_t *) mp->b_rptr;
	struct parm *pp = NULL;

	bzero(m3ua_results, sizeof(*m3ua_results));
	p += 2;			/* skip header */
	for (; p < (uint32_t *) mp->b_wptr && (len = UA_LENGTH(*p)) >= 4; p += (len + 3) >> 2) {
		int ttag, tag = ntohl(*p) >> 16;

		ttag = tag - UA_PARM_BASE;
		if (0 <= ttag && ttag <= UA_PARM_MAX) {
			pp = ((struct ua_parm *) (m3ua_results)) + ttag;
			goto finish_parm;
		}
		ttag = tag - M3UA_PARM_BASE;
		if (0 <= ttag && ttag <= M3UA_PARM_MAX) {
			ttag += UA_PARM_MAX + 1;
			pp = ((struct ua_parm *) (m3ua_results)) + ttag;
			goto finish_parm;
		}
		continue;
	      finish_parm:
		pp->u.wptr = p + 1;
		pp->len = len;
		pp->val = ntohl(p[1]);
	}
	return (0);
}
