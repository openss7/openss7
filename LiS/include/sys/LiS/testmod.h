/*****************************************************************************

 @(#) $Id: testmod.h,v 0.9.2.2 2006/09/18 13:52:31 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2006/09/18 13:52:31 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: testmod.h,v $
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

#ident "@(#) $RCSfile: testmod.h,v $ $Name:  $($Revision: 0.9.2.2 $) Copyright (c) 2001-2005 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

#ifndef __SYS_TESTMOD_H__
#warning "Do no include sys/streams/testmod.h directly, include sys/testmod.h instead."
#endif

#define TM_IOC_MAGIC 'V'

#define TM_IOC_HANGUP		((TM_IOC_MAGIC << 8) | 0x01)
#define TM_IOC_RDERR		((TM_IOC_MAGIC << 8) | 0x02)
#define TM_IOC_WRERR		((TM_IOC_MAGIC << 8) | 0x03)
#define TM_IOC_RWERR		((TM_IOC_MAGIC << 8) | 0x04)
#define TM_IOC_PSIGNAL		((TM_IOC_MAGIC << 8) | 0x05)
#define TM_IOC_NSIGNAL		((TM_IOC_MAGIC << 8) | 0x06)
#define TM_IOC_IOCTL		((TM_IOC_MAGIC << 8) | 0x07)
#define TM_IOC_COPYIN		((TM_IOC_MAGIC << 8) | 0x08)
#define TM_IOC_COPYOUT		((TM_IOC_MAGIC << 8) | 0x09)
#define TM_IOC_COPYIO		((TM_IOC_MAGIC << 8) | 0x0a)

#endif				/* __SYS_STREAMS_TESTMOD_H__ */
