/*****************************************************************************

 @(#) File: src/modules/vtoip.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

static char const ident[] = "File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE;

/*
 * This is a Y.1452 VToIP (Voice Trunking over IP) module.  It pushes over a UDP
 * Stream that provides connectivity to the peer VToIP system.  The upper
 * boundary service interface is the MXI interface.  The lower boundary service
 * interface is the UDP-TPI interface.
 *
 * In general, the UDP Stream may be opened, options configured, bound to a
 * local IP address and port number, and connected to a remote IP address and
 * port number.  This module can then be pushed.  Pushing the module will flush
 * the Stream and any data messages received on the Stream will be discarded
 * until the Stream is configured, enabled and connected.
 *
 * Once the module is pushed, the MXI Stream can be linked under the MATRIX
 * multiplexing driver and the channels available in the multiplex facility will
 * be made available to the switching matrix.
 */

#define _SVR4_SOURCE
#define _MPS_SOURCE
#define _SUN_SOURCE

#include <sys/os7/compat.h>

#define VTOIP_DESCRIP	"VTOIP STREAMS Module"
#define VTOIP_EXTRA	"Part of the OpenSS7 VoIP Stack for Linux Fast-STREAMS"
#define VTOIP_REVISION	"OpenSS7 File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE
#define VTOIP_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define VTOIP_DEVICE	"Provides OpenSS7 Voice Trunking over IP (VTOIP)"
#define VTOIP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define VTOIP_LICENSE	"GPL"
#define VTOIP_BANNER	VTOIP_DESCRIP	"\n" \
			VTOIP_EXTRA	"\n" \
			VTOIP_REVISION	"\n" \
			VTOIP_COPYRIGHT	"\n" \
			VTOIP_DEVICE	"\n" \
			VTOIP_CONTACT	"\n"
#define VTOIP_SPLASH	VTOIP_DEVICE	" - " \
			VTOIP_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(VTOIP_CONTACT);
MODULE_DESCRIPTION(VTOIP_DESCRIP);
MODULE_SUPPORTED_DEVICE(VTOIP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(VTOIP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-vtoip");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define VTOIP_MOD_ID	CONFIG_STREAMS_VTOIP_MODID
#define VTOIP_MOD_NAME	CONFIG_STREAMS_VTOIP_NAME

#ifndef VTOIP_MOD_NAME
#define VTOIP_MOD_NAME	"vtoip"
#endif				/* VTOIP_MOD_NAME */

#ifndef VTOIP_MOD_ID
#define VTOIP_MOD_ID	0
#endif				/* VTOIP_MOD_ID */

/*
 *  STREAMS DEFINITIONS
 *  -------------------------------------------------------------------------
 */

#define MOD_ID		VTOIP_MOD_ID
#define MOD_NAME	VTOIP_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	VTOIP_BANNER
#else				/* MODULE */
#define MOD_BANNER	VTOIP_SPLASH
#endif				/* MODULE */

/*
 *  LINUX INITIALIZATION
 *  -------------------------------------------------------------------------
 */

#ifdef LINUX
unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, short, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for VTOIP module.  (0 for allocation.)");

/** vtoipinit - initialize VTOIP
  */
static __init int
vtoipinit(void)
{
	cmn_err(CE_NOTE, MOD_BANNER);
	return (0);
}

/** vtoipexit - terminate VTOIP
  */
static __exit void
vtoipexit(void)
{
	return;
}

module_init(vtoipinit);
module_exit(vtoipexit);

#endif				/* LINUX */
