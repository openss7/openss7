/*****************************************************************************

 @(#) File: src/modules/isos.c

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

static char const ident[] = "src/modules/isos.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 *  ISO Transport over SCTP (ISOS) module
 */

#define _SVR4_SOURCE	1
#define _MPS_SOURCE	1
#define _SUN_SOURCE	1

#include <sys/os7/compat.h>

/* ----------------------------- */

#define ISOS_DESCRIP	"SVR 4.2 ISO Transport over SCTP for Linux Fast-STREAMS"
#define ISOS_EXTRA	"Part of the OpenSS7 OSI Stack for Linux Fast-STREAMS"
#define ISOS_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define ISOS_REVISION	"OpenSS7 src/modules/isos.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define ISOS_DEVICE	"SVR 4.2MP STREAMS ISO Transport over SCTP (ISOS)"
#define ISOS_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define ISOS_LICENSE	"GPL"
#define ISOS_BANNER	ISOS_DESCRIP	"\n" \
			ISOS_EXTRA	"\n" \
			ISOS_COPYRIGHT	"\n" \
			ISOS_REVISION	"\n" \
			ISOS_DEVICE	"\n" \
			ISOS_CONTACT
#define ISOS_SPLASH	ISOS_DEVICE	" - " \
			ISOS_REVISION

#ifdef LINUX
MODULE_AUTHOR(ISOS_CONTACT);
MODULE_DESCRIPTION(ISOS_DESCRIP);
MODULE_SUPPORTED_DEVICE(ISOS_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(ISOS_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-isos");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#ifndef CONFIG_STREAMS_ISOS_NAME
#error "CONFIG_STREAMS_ISOS_NAME must be defined."
#endif				/* CONFIG_STREAMS_ISOS_NAME */
#ifndef CONFIG_STREAMS_ISOS_MODID
#error "CONFIG_STREAMS_ISOS_MODID must be defined."
#endif				/* CONFIG_STREAMS_ISOS_MODID */

#define ISOS_MOD_ID	CONFIG_STREAMS_ISOS_MODID
#define ISOS_MOD_NAME	CONFIG_STREAMS_ISOS_NAME

#ifndef ISOS_MOD_NAME
#define ISOS_MOD_NAME	"isos"
#endif				/* ISOS_MOD_NAME */

#ifndef ISOS_MOD_ID
#define ISOS_MOD_ID	0
#endif				/* ISOS_MOD_ID */

/*
 *  STREAMS DEFINITIONS
 *  -------------------------------------------------------------------------
 */

#define MOD_ID		ISOS_MOD_ID
#define MOD_NAME	ISOS_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	ISOS_BANNER
#else				/* MODULE */
#define MOD_BANNER	ISOS_SPLASH
#endif				/* MODULE */

/*
 *  LINUX INITIALIZATION
 *  -------------------------------------------------------------------------
 */

#ifdef LINUX
static modID_t modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, ushort, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for ISOS.  (0 for allocation.)");

/** isosinit - initialize ISOS
  */
static __init int
isosinit(void)
{
	cmn_err(CE_NOTE, MOD_BANNER);
	return (0);
}

/** isosexit - terminate ISOS
  */
static __exit void
isosexit(void)
{
	return;
}

module_init(isosinit);
module_exit(isosexit);

#endif				/* LINUX */
