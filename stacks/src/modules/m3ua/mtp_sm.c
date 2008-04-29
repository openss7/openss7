/*****************************************************************************

 @(#) $RCSfile: mtp_sm.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 07:11:01 $

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

 Last Modified $Date: 2008-04-29 07:11:01 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mtp_sm.c,v $
 Revision 0.9.2.4  2008-04-29 07:11:01  brian
 - updating headers for release

 Revision 0.9.2.3  2007/08/14 12:18:03  brian
 - GPLv3 header updates

 Revision 0.9.2.2  2007/06/17 01:56:18  brian
 - updates for release, remove any later language

 Revision 0.9.2.1  2004/08/21 10:14:45  brian
 - Force checkin on branch.

 Revision 0.9  2004/01/17 08:20:21  brian
 - Added files for 0.9 baseline autoconf release.

 Revision 0.8.2.1  2002/10/18 03:27:43  brian
 Indentation changes only.

 Revision 0.8  2002/04/02 08:20:42  brian
 Started Linux 2.4 development branch.

 Revision 0.7  2001/07/29 10:47:37  brian
 Split up M3UA files.

 *****************************************************************************/

#ident "@(#) $RCSfile: mtp_sm.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 07:11:01 $"

static char const ident[] = "$RCSfile: mtp_sm.c,v $ $Name:  $($Revision: 0.9.2.4 $) $Date: 2008-04-29 07:11:01 $";
