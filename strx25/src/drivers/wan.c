/*****************************************************************************

 @(#) $RCSfile: wan.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-18 16:43:13 $

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

 Last Modified $Date: 2008-06-18 16:43:13 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: wan.c,v $
 Revision 0.9.2.1  2008-06-18 16:43:13  brian
 - added new files for NLI and DLPI

 *****************************************************************************/

#ident "@(#) $RCSfile: wan.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-18 16:43:13 $"

static char const ident[] = "$RCSfile: wan.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-06-18 16:43:13 $";

/*
 * This file is the WAN (Wide Area Network, Raw HDLC) driver.  It does not link
 * any Streams beneath it and is a pseudo device driver.  It may open internal
 * Linux native Raw HDLC devices.  Normally WAN Streams are linked beneath the
 * LAPB (X.25), IFR (Frame Relay), SDLC/HDLC, or some other link layer driver.
 */
