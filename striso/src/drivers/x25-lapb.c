/*****************************************************************************

 @(#) $RCSfile: x25-lapb.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/14 07:05:03 $

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

 Last Modified $Date: 2007/08/14 07:05:03 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: x25-lapb.c,v $
 Revision 0.9.2.2  2007/08/14 07:05:03  brian
 - GNUv3 header update

 Revision 0.9.2.1  2006/04/12 20:36:01  brian
 - added some experimental drivers

 *****************************************************************************/

#ident "@(#) $RCSfile: x25-lapb.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/14 07:05:03 $"

static char const ident[] =
    "$RCSfile: x25-lapb.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2007/08/14 07:05:03 $";

/*
 *  Here is an X.25 lapb (SLP and MLP) driver.  This is a DLPI driver that can push over or link a
 *  CDI stream.  CDI streams can be CD_HLDC or CD_LAN.  CD_HDLC streams need to be raw packet
 *  streams.  This driver will use LAPB over these streams as described in X.25 Clause 2.  CD_LAN
 *  streams need to be raw packet streams with or without SNAP headers.  This driver will use LLC2
 *  over these streams as described under ISO/IEC 8881.
 */
