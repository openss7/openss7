/*****************************************************************************

 @(#) $Id: sl_mux.h,v 0.9.2.7 2008-04-29 07:10:45 brian Exp $

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

 Last Modified $Date: 2008-04-29 07:10:45 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sl_mux.h,v $
 Revision 0.9.2.7  2008-04-29 07:10:45  brian
 - updating headers for release

 Revision 0.9.2.6  2007/09/06 11:16:17  brian
 - testing updates

 Revision 0.9.2.5  2007/08/14 12:17:10  brian
 - GPLv3 header updates

 Revision 0.9.2.4  2007/08/03 13:35:01  brian
 - manual updates, put ss7 modules in public release

 Revision 0.9.2.3  2007/07/21 20:09:43  brian
 - added pass structure

 Revision 0.9.2.2  2007/02/13 14:05:28  brian
 - corrected ulong and long for 32-bit compat

 Revision 0.9.2.1  2007/01/21 20:20:10  brian
 - added documetation

 *****************************************************************************/

#ifndef __SS7_SL_MUX_H__
#define __SS7_SL_MUX_H__

#ident "@(#) $RCSfile: sl_mux.h,v $ $Name:  $($Revision: 0.9.2.7 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

#define SLMUX_CLEI_MAX	    32

struct slmux_ppa {
	int slm_index;
	u_int32_t slm_ppa;
	char slm_clei[SLMUX_CLEI_MAX];
};

struct slmux_ppa_list {
	int slm_list_num;
	struct slmux_ppa slm_list_array[0];
	/* followed by array elements */
};

struct slmux_mon {
	struct slmux_ppa mon_ppa;
	int mon_dir;
	unsigned char mon_msg_type;
	unsigned char mon_msg_band;
	unsigned short mon_msg_flags;
	unsigned char mon_msg_data[0];
	/* followed by encapsulated control part */
};

#define SLM_IOC_MAGIC	    'x'

#define SLM_IOCSPPA	_IOWR(	SLM_IOC_MAGIC,	 1,	struct slmux_ppa	)
#define SLM_IOCGPPA	_IOR(	SLM_IOC_MAGIC,	 2,	struct slmux_ppa	)
#define SLM_IOCLPPA	_IOR(	SLM_IOC_MAGIC,	 3,	struct slmux_ppa_list	)
#define SLM_IOCSMON	_IOR(	SLM_IOC_MAGIC,	 4,	struct slmux_ppa	)
#define SLM_IOCCMON	_IOR(	SLM_IOC_MAGIC,	 5,	struct slmux_ppa	)

#endif				/* __SS7_SL_MUX_H__ */
