/*****************************************************************************

 @(#) File: src/modules/lapf.c

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
 *  LAPF module
 */

#define _SVR4_SOURCE
#define _MPS_SOURCE
#define _SUN_SOURCE

#include <sys/os7/compat.h>

#define LAPF_DESCRIP	"LAPF STREAMS Module"
#define LAPF_EXTRA	"Part of the OpenSS7 X.25 Stack for Linux Fast-STREAMS"
#define LAPF_REVISION	"OpenSS7 File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE
#define LAPF_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define LAPF_DEVICE	"Provides OpenSS7 Link Access Protocol Frame Relay (LAPF)"
#define LAPF_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LAPF_LICENSE	"GPL"
#define LAPF_BANNER	LAPF_DESCRIP	"\n" \
			LAPF_EXTRA	"\n" \
			LAPF_REVISION	"\n" \
			LAPF_COPYRIGHT	"\n" \
			LAPF_DEVICE	"\n" \
			LAPF_CONTACT	"\n"
#define LAPF_SPLASH	LAPF_DEVICE	" - " \
			LAPF_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(LAPF_CONTACT);
MODULE_DESCRIPTION(LAPF_DESCRIP);
MODULE_SUPPORTED_DEVICE(LAPF_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LAPF_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-lapf");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define LAPF_MOD_ID	CONFIG_STREAMS_LAPF_MODID
#define LAPF_MOD_NAME	CONFIG_STREAMS_LAPF_NAME

#ifndef LAPF_MOD_NAME
#define LAPF_MOD_NAME	"lapf"
#endif				/* LAPF_MOD_NAME */

#ifndef LAPF_MOD_ID
#define LAPF_MOD_ID	0
#endif				/* LAPF_MOD_ID */

/*
 *  STREAMS DEFINITIONS
 *  -------------------------------------------------------------------------
 */

#define MOD_ID		LAPF_MOD_ID
#define MOD_NAME	LAPF_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	LAPF_BANNER
#else				/* MODULE */
#define MOD_BANNER	LAPF_SPLASH
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
MODULE_PARM_DESC(modid, "Module ID for LAPF module.  (0 for allocation.)");

/** lapfinit - initialize LAPF
  */
static __init int
lapfinit(void)
{
	cmn_err(CE_NOTE, MOD_BANNER);
	return (0);
}

/** lapfexit - terminate LAPF
  */
static __exit void
lapfexit(void)
{
	return;
}

module_init(lapfinit);
module_exit(lapfexit);

#endif				/* LINUX */
