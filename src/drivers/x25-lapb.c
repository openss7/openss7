/*****************************************************************************

 @(#) $RCSfile: x25-lapb.c,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-09-02 08:46:39 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2013  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2011-09-02 08:46:39 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: x25-lapb.c,v $
 Revision 1.1.2.4  2011-09-02 08:46:39  brian
 - sync up lots of repo and build changes from git

 Revision 1.1.2.3  2011-01-18 16:55:52  brian
 - added stub drivers and modules

 Revision 1.1.2.2  2010-11-28 14:21:42  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:20:58  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] =
    "$RCSfile: x25-lapb.c,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-09-02 08:46:39 $";

/*
 *  Here is an X.25 lapb (SLP and MLP) driver.  This is a DLPI driver that can push over or link a
 *  CDI stream.  CDI streams can be CD_HLDC or CD_LAN.  CD_HDLC streams need to be raw packet
 *  streams.  This driver will use LAPB over these streams as described in X.25 Clause 2.  CD_LAN
 *  streams need to be raw packet streams with or without SNAP headers.  This driver will use LLC2
 *  over these streams as described under ISO/IEC 8881.
 */

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os7/compat.h>

#define LAPB_DESCRIP	"SVR 4.2 DLPI X25-LAPB Driver for Linux Fast-STREAMS"
#define LAPB_EXTRA      "Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define LAPB_COPYRIGHT	"Copyright (c) 2008-2013  Monavacon Limited.  All Rights Reserved."
#define LAPB_REVISION	"OpenSS7 $RCSfile: x25-lapb.c,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-09-02 08:46:39 $"
#define LAPB_DEVICE	"SVR 4.2MP DLPI Driver (DLPI) for X.25 Link Access Protocol B-Channel (X25-LAPB)"
#define LAPB_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LAPB_LICENSE	"GPL"
#define LAPB_BANNER	LAPB_DESCRIP	"\n" \
			LAPB_EXTRA	"\n" \
			LAPB_COPYRIGHT	"\n" \
			LAPB_REVISION	"\n" \
			LAPB_DEVICE	"\n" \
			LAPB_CONTACT	"\n"
#define LAPB_SPLASH	LAPB_DESCRIP	" - " \
			LAPB_REVISION

#ifndef CONFIG_STREAMS_LAPB_NAME
#error "CONFIG_STREAMS_LAPB_NAME must be defined."
#endif				/* CONFIG_STREAMS_LAPB_NAME */
#ifndef CONFIG_STREAMS_LAPB_MODID
#error "CONFIG_STREAMS_LAPB_MODID must be defined."
#endif				/* CONFIG_STREAMS_LAPB_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(LAPB_CONTACT);
MODULE_DESCRIPTION(LAPB_DESCRIP);
MODULE_SUPPORTED_DEVICE(LAPB_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LAPB_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_LAPB_MODID));
MODULE_ALIAS("streams-driver-lapb");
MODULE_ALIAS("streams-module-lapb");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_LAPB_MAJOR));
MODULE_ALIAS("/dev/streams/lapb");
MODULE_ALIAS("/dev/streams/lapb/*");
MODULE_ALIAS("/dev/streams/clone/lapb");
MODULE_ALIAS("char-major-" __stringify(CONFIG_STREAMS_CLONE_MAJOR) "-" __stringify(CONFIG_STREAMS_LAPB_MAJOR));
MODULE_ALIAS("/dev/lapb");
MODULE_ALIAS("devname:lapb");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* MODULE */
#endif				/* LINUX */

#define LAPB_DRV_ID	CONFIG_STREAMS_LAPB_MODID
#define LAPB_DRV_NAME	CONFIG_STREAMS_LAPB_NAME

#ifndef LAPB_DRV_NAME
#define LAPB_DRV_NAME	"lapb"
#endif				/* LAPB_DRV_NAME */

#ifndef LAPB_DRV_ID
#define LAPB_DRV_ID	0
#endif				/* LAPB_DRV_ID */

/*
 *  STREAMS DEFINITIONS
 *  -------------------------------------------------------------------------
 */

#define DRV_ID		LAPB_DRV_ID
#define DRV_NAME	LAPB_DRV_NAME
#define CMAJORS		LAPB_CMAJORS
#define CMAJOR_0	LAPB_CMAJOR_0
#define UNITS		LAPB_UNITS
#ifdef MODULE
#define DRV_BANNER	LAPB_BANNER
#else				/* MODULE */
#define DRV_BANNER	LAPB_SPLASH
#endif				/* MODULE */

/*
 *  LINUX INITIALIZATION
 *  -------------------------------------------------------------------------
 */

static modID_t modid = DRV_ID;

#ifdef LINUX

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for LAPB.  (0 for allocation.)");

/** lapbinit - initialize LAPB
  */
static __init int
lapbinit(void)
{
	cmn_err(CE_NOTE, DRV_BANNER);
	return (0);
}

/** lapbexit - terminate LAPB
  */
static __exit void
lapbexit(void)
{
	return;
}

module_init(lapbinit);
module_exit(lapbexit);

#endif				/* LINUX */
