/*****************************************************************************

 @(#) $RCSfile: x25-llc2.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-01-18 16:55:52 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2011  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2011-01-18 16:55:52 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: x25-llc2.c,v $
 Revision 1.1.2.3  2011-01-18 16:55:52  brian
 - added stub drivers and modules

 Revision 1.1.2.2  2010-11-28 14:21:42  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:20:59  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] =
    "$RCSfile: x25-llc2.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-01-18 16:55:52 $";

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os7/compat.h>

#define LLC2_DESCRIP	"SVR 4.2 DLPI X25-LLC2 DRIVER FOR LINUX FAST-STREAMS"
#define LLC2_EXTRA      "Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define LLC2_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define LLC2_REVISION	"OpenSS7 $RCSfile: x25-llc2.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-01-18 16:55:52 $"
#define LLC2_DEVICE	"SVR 4.2MP DLPI Driver (DLPI) for IEEE 802.2 LLC"
#define LLC2_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LLC2_LICENSE	"GPL"
#define LLC2_BANNER	LLC2_DESCRIP	"\n" \
			LLC2_EXTRA	"\n" \
			LLC2_COPYRIGHT	"\n" \
			LLC2_REVISION	"\n" \
			LLC2_DEVICE	"\n" \
			LLC2_CONTACT	"\n"
#define LLC2_SPLASH	LLC2_DESCRIP	" - " \
			LLC2_REVISION

#ifndef CONFIG_STREAMS_LLC2_NAME
#error "CONFIG_STREAMS_LLC2_NAME must be defined."
#endif				/* CONFIG_STREAMS_LLC2_NAME */
#ifndef CONFIG_STREAMS_LLC2_MODID
#error "CONFIG_STREAMS_LLC2_MODID must be defined."
#endif				/* CONFIG_STREAMS_LLC2_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(LLC2_CONTACT);
MODULE_DESCRIPTION(LLC2_DESCRIP);
MODULE_SUPPORTED_DEVICE(LLC2_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LLC2_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_LLC2_MODID));
MODULE_ALIAS("streams-driver-llc2");
MODULE_ALIAS("streams-module-llc2");
MODULE_ALIAS("streams-major-" __stringify(CONFIG_STREAMS_LLC2_MAJOR));
MODULE_ALIAS("/dev/streams/llc2");
MODULE_ALIAS("/dev/streams/llc2/*");
MODULE_ALIAS("/dev/streams/clone/llc2");
MODULE_ALIAS("char-major-" __stringify(LLC2_MAJOR_0));
MODULE_ALIAS("char-major-" __stringify(LLC2_MAJOR_0) "-*");
MODULE_ALIAS("char-major-" __stringify(LLC2_MAJOR_0) "-0");
MODULE_ALIAS("/dev/llc2");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* MODULE */
#endif				/* LINUX */

#define LLC2_DRV_ID	CONFIG_STREAMS_LLC2_MODID
#define LLC2_DRV_NAME	CONFIG_STREAMS_LLC2_NAME
#define LLC2_CMAJORS	CONFIG_STREAMS_LLC2_NMAJORS
#define LLC2_CMAJOR_0	CONFIG_STREAMS_LLC2_MAJOR
#define LLC2_UNITS	CONFIG_STREAMS_LLC2_NMINORS

#ifndef LLC2_DRV_NAME
#define LLC2_DRV_NAME	"llc2"
#endif				/* LLC2_DRV_NAME */

#ifndef LLC2_DRV_ID
#define LLC2_DRV_ID	0
#endif				/* LLC2_DRV_ID */

/*
 *  STREAMS DEFINITIONS
 *  -------------------------------------------------------------------------
 */

#define DRV_ID		LLC2_DRV_ID
#define DRV_NAME	LLC2_DRV_NAME
#define CMAJORS		LLC2_CMAJORS
#define CMAJOR_0	LLC2_CMAJOR_0
#define UNITS		LLC2_UNITS
#ifdef MODULE
#define DRV_BANNER	LLC2_BANNER
#else				/* MODULE */
#define DRV_BANNER	LLC2_SPLASH
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
MODULE_PARM_DESC(modid, "Module ID for LLC2.  (0 for allocation.)");

/** llc2init - initialize LLC2
  */
static __init int
llc2init(void)
{
	cmn_err(CE_NOTE, DRV_BANNER);
	return (0);
}

/** llc2exit - terminate LLC2
  */
static __exit void
llc2exit(void)
{
	return;
}

module_init(llc2init);
module_exit(llc2exit);

#endif				/* LINUX */
