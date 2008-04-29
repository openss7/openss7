/*****************************************************************************

 @(#) $RCSfile: sua_msg.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 01:52:21 $

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

 $Log: sua_msg.c,v $
 Revision 0.9.2.4  2008-04-29 01:52:21  brian
 - updated headers for release

 Revision 0.9.2.3  2007/08/14 08:33:55  brian
 - GPLv3 header update

 *****************************************************************************/

#ident "@(#) $RCSfile: sua_msg.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 01:52:21 $"

static char const ident[] =
    "$RCSfile: sua_msg.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 01:52:21 $";

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
sua_decode_parms(mblk_t *mp, sua_parms_t * sua_results)
{
	int len;
	uint32_t *p = (uint32_t *) mp->b_rptr;
	struct ua_parm *pp = NULL;

	bzero(sua_results, sizeof(*sua_results));
	p += 2;			/* skip header */
	for (; p < (uint32_t *) mp->b_wptr && (len = UA_LENGTH(*p)) >= 4; p += (len + 3) >> 2) {
		int ttag, tag = ntohl(*p) >> 16;

		ttag = tag - UA_PARM_BASE;
		if (0 <= ttag && ttag <= UA_PARM_MAX) {
			pp = ((struct ua_parm *) (sua_results)) + ttag;
			pp->u.wptr = p + 1;
			pp->len = len;
			pp->val = ntohl(p[1]);
		} else {
			ttag = tag - SUA_PARM_BASE;
			if (0 <= ttag && ttag <= SUA_PARM_MAX) {
				int offset;
				struct sua_sparm_results *sp;

				ttag += UA_PARM_MAX + 1;
				pp = ((struct ua_parm *) (sua_results)) + ttag;
				pp->u.wptr = p + 1;
				pp->len = len;
				pp->val = ntohl(p[1]);
				switch (ttag) {
				case 1:	/* srce_addr */
					sp = &sua_results->srce;
					offset = UA_PARM_MAX + 1 + SUA_PARM_MAX + 1;
					break;
				case 2:	/* dest_addr */
					sp = &sua_results->dest;
					offset =
					    UA_PARM_MAX + 1 + SUA_PARM_MAX + 1 + SUA_SPARM_MAX + 1;
					break;
				default:
					continue;
				}
				{
					int len2;
					struct ua_parm *p2 = p + 2;

					for (; p2 < p + (len + 3) >> 2 &&
					     (len2 = UA_LENGTH(*p2)) >= 4; p2 += (len2 + 3) >> 2) {
						int stag = ntohl(*p2) >> 16 - SUA_SPARM_BASE;

						if (0 <= stag && stag <= SUA_SPARM_MAX) {
							stag += offset;
							pp = ((struct ua_parm *) (sp)) + stag;
							pp->u.wptr = p2 + 1;
							pp->len = len2;
							pp->val = ntohl(p2[1]);
						}
					}
				}
			}
		}
	}
	return (0);
}
