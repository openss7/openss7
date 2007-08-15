/*****************************************************************************

 @(#) $RCSfile: dl2.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/08/15 05:35:41 $

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

 Last Modified $Date: 2007/08/15 05:35:41 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: dl2.c,v $
 Revision 0.9.2.6  2007/08/15 05:35:41  brian
 - GPLv3 updates

 Revision 0.9.2.5  2007/08/14 03:31:07  brian
 - GPLv3 header update

 Revision 0.9.2.4  2007/07/14 01:37:18  brian
 - make license explicit, add documentation

 Revision 0.9.2.3  2007/03/25 19:02:46  brian
 - changes to support 2.6.20-1.2307.fc5 kernel

 Revision 0.9.2.2  2006/12/21 11:33:31  brian
 - updates for release, current development

 Revision 0.9.2.1  2006/12/20 23:10:43  brian
 - added new mux drivers

 *****************************************************************************/

#ident "@(#) $RCSfile: dl2.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/08/15 05:35:41 $"

static char const ident[] =
    "$RCSfile: dl2.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/08/15 05:35:41 $";

/*
 *  This is a multiplexing driver use to support linking DL streams.  When DL streams are linked
 *  under the driver and configured using input-output controls, the DL stream appears as a network
 *  device to Linux.  DL stream can also be opened on the upper multiplex and attached to Linux
 *  network devices.  This permits DL drivers to be exported to Linux and DL users to also use Linux
 *  devices not directly supported with a STREAMS driver.
 *
 *  To manage mapping of upper multiplex DL streams to Linux devices, a specialized intput-output
 *  control and DL primitive are used.  These take as an argument the ascii name of the device and
 *  return a PPA (minor device number) that can be used to open the device directly or can be used
 *  to attach a style 2 DL stream to the device.  To manage mapping of lower multiplex DL streams to
 *  LInux deices, a specialized input-output control is used.  This input-output control takes as an
 *  argument the multiplex id and the ascii device name.  Devices mapped in this way begin life as a
 *  network device of the type revealed by interrogating the DL stream but can be configured using
 *  the normal Linux facilities.
 *
 *  This driver is a replacement for the unsupported LDL driver.
 */

#ifndef HAVE_KTYPE_BOOL
#include <stdbool.h>
#endif

#define _DEBUG 1
//#undef _DEBUG
/
#define _LFS_SOURCE	1
#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#include <sys/os7/compat.h>
#include <sys/dlpi.h>
#include <linux/netdevice.h>
#define ND_DESCRIP	"NETDEV (DL) STREAMS MULTIPLEXING DRIVER."
#define ND_EXTRA	"Part of the OpenSS7 Stack for Linux Fast-STREAMS."
#define ND_REVISION	"OpenSS7 $RCSfile: dl2.c,v $ $Name:  $($Revision: 0.9.2.6 $) $Date: 2007/08/15 05:35:41 $"
#define ND_COPYRIGHT	"Copyright (c) 1997-2006  OpenSS7 Corporation.  All Rights Reserved."
#define ND_DEVICE	"Supports Linux HDLC devices."
#define ND_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define ND_LICENSE	"GPL"
#define ND_BANNER	ND_DESCRIP	"\n" \
			ND_EXTRA	"\n" \
			ND_REVISION	"\n" \
			ND_COPYRIGHT	"\n" \
			ND_DEVICE	"\n" \
			ND_CONTACT	"\n"
#define ND_SPLASH	ND_DESCRIP	" - " \
			ND_REVISION
#ifdef LINUX
    MODULE_AUTHOR(ND_CONTACT);
MODULE_DESCRIPTION(ND_DESCRIP);
MODULE_SUPPORTED_DEVICE(ND_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(ND_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-nd");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE "-"
	       PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#ifdef LFS
#define ND_DRV_ID		CONFIG_STREAMS_ND_MODID
#define ND_DRV_NAME		CONFIG_STREAMS_ND_NAME
#define ND_CMAJORS		CONFIG_STREAMS_ND_NMAJORS
#define ND_CMAJOR_0		CONFIG_STREAMS_ND_MAJOR
#define ND_UNITS		CONFIG_STREAMS_ND_NMINORS
#endif				/* LFS */

#ifdef LINUX
#ifdef MODULE_ALIAS
#ifdef LFS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_ND_MODID));
MODULE_ALIAS("stremas-driver-nd");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_ND_MAJOR));
MODULE_ALIAS("/dev/streams/nd");
MODULE_ALIAS("/dev/streams/nd/*");
MODULE_ALIAS("/dev/streams/clone/nd");
#endif				/* LFS */
MODULE_ALIAS("char-major-" __stringify(ND_CMAJOR_0));
MODULE_ALIAS("char-major-" __stringify(ND_CMAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(ND_CMAJOR_0) "-0");
MODULE_ALIAS("/dev/nd");
#endif				/* MODULE_ALIAS */
#endif				/* LINUX */

#define NDLOGST	    1		/* log ND stat transitions */
#define NDLOGTO	    2		/* log ND timeouts */
#define NDLOGRX	    3		/* log ND received primitives */
#define NDLOGTX	    4		/* log ND issued primitives */
#define NDLOGTE	    5		/* log ND timer events */
#define NDLOGDA	    6		/* log ND data */
