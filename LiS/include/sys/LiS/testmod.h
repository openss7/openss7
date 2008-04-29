/*****************************************************************************

 @(#) $RCSfile: testmod.h,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2008-04-29 08:33:16 $

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

 Last Modified $Date: 2008-04-29 08:33:16 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: testmod.h,v $
 Revision 0.9.2.6  2008-04-29 08:33:16  brian
 - update headers for Affero release

 Revision 0.9.2.5  2007/08/14 10:47:11  brian
 - GPLv3 header update

 Revision 0.9.2.4  2006/12/18 09:50:57  brian
 - updated headers for release

 Revision 0.9.2.3  2006/09/24 21:56:41  brian
 - documentation and library updates

 Revision 0.9.2.2  2006/09/18 13:52:31  brian
 - added doxygen markers to sources

 Revision 0.9.2.1  2005/10/23 05:01:26  brian
 - test programs and modules for POSIX testing

 Revision 0.9.2.5  2005/10/14 12:26:36  brian
 - SC module and scls utility tested

 Revision 0.9.2.4  2005/10/13 10:58:34  brian
 - working up testing of sad(4) and sc(4)

 Revision 0.9.2.3  2005/10/07 09:34:10  brian
 - more testing and corrections

 Revision 0.9.2.2  2005/10/05 09:25:24  brian
 - poll tests, some noxious problem still with poll

 Revision 0.9.2.1  2005/09/25 22:52:10  brian
 - added test module and continuing with testing

 *****************************************************************************/

#ifndef __SYS_STREAMS_TESTMOD_H__
#define __SYS_STREAMS_TESTMOD_H__

#ident "@(#) $RCSfile: testmod.h,v $ $Name:  $($Revision: 0.9.2.6 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

#ifndef __SYS_TESTMOD_H__
#warning "Do no include sys/streams/testmod.h directly, include sys/testmod.h instead."
#endif

/** @addtogroup testmod
  * @{ */

/** @file
  * STREAMS Test Module (testmod(4)) Header File.
  *
  * This file provides the user to STREAMS module interface between the user and
  * the Test Module (testmod(4)) STREAMS module.  This module is primarily for
  * testing in kernel specifics during conformance testsuite runs (see
  * test-streams(8)).  It performs several rather highly specialized functions. */

/**
  * @name Test Module Magic Number
  * STREAMS Test Module magic number for input-output controls.
  * @{ */
#define TM_IOC_MAGIC 'V' /**< The ioctl(2) magic number for testmod(4). */
/** @} */

/**
  * @name Test Module Input-Output Controls
  * @{ */
#define TM_IOC_HANGUP	((TM_IOC_MAGIC << 8) | 0x01) /**< Generate M_HANGUP. */
#define TM_IOC_RDERR	((TM_IOC_MAGIC << 8) | 0x02) /**< Generate M_ERROR (read error). */
#define TM_IOC_WRERR	((TM_IOC_MAGIC << 8) | 0x03) /**< Generate M_ERROR (write error). */
#define TM_IOC_RWERR	((TM_IOC_MAGIC << 8) | 0x04) /**< Generate M_ERROR (read/write error). */
#define TM_IOC_PSIGNAL	((TM_IOC_MAGIC << 8) | 0x05) /**< Generate M_PCPROTO signal. */
#define TM_IOC_NSIGNAL	((TM_IOC_MAGIC << 8) | 0x06) /**< Generate M_PROTO signal. */
#define TM_IOC_IOCTL	((TM_IOC_MAGIC << 8) | 0x07) /**< Generate M_IOCTL. */
#define TM_IOC_COPYIN	((TM_IOC_MAGIC << 8) | 0x08) /**< Try to copy in. */
#define TM_IOC_COPYOUT	((TM_IOC_MAGIC << 8) | 0x09) /**< Try to copy out. */
#define TM_IOC_COPYIO	((TM_IOC_MAGIC << 8) | 0x0a) /**< Try to copy in and out. */
/** @} */

/** @} */

#endif				/* __SYS_STREAMS_TESTMOD_H__ */

// vim: ft=cpp com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
