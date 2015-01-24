/*****************************************************************************

 @(#) File: src/modules/tstmod.c

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

static char const ident[] = "src/modules/tstmod.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 *  TSTMOD module
 */

#define _SVR4_SOURCE
#define _MPS_SOURCE
#define _SUN_SOURCE

#include <sys/os7/compat.h>

#define TSTMOD_DESCRIP	"TSTMOD STREAMS Module"
#define TSTMOD_EXTRA	"Part of Linux Fast-STREAMS"
#define TSTMOD_REVISION	"OpenSS7 src/modules/tstmod.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define TSTMOD_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define TSTMOD_DEVICE	"Provides OpenSS7 Test Module (TSTMOD)"
#define TSTMOD_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define TSTMOD_LICENSE	"GPL"
#define TSTMOD_BANNER	TSTMOD_DESCRIP	"\n" \
			TSTMOD_EXTRA	"\n" \
			TSTMOD_REVISION	"\n" \
			TSTMOD_COPYRIGHT	"\n" \
			TSTMOD_DEVICE	"\n" \
			TSTMOD_CONTACT	"\n"
#define TSTMOD_SPLASH	TSTMOD_DEVICE	" - " \
			TSTMOD_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(TSTMOD_CONTACT);
MODULE_DESCRIPTION(TSTMOD_DESCRIP);
MODULE_SUPPORTED_DEVICE(TSTMOD_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TSTMOD_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-tstmod");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define TSTMOD_MOD_ID	CONFIG_STREAMS_TSTMOD_MODID
#define TSTMOD_MOD_NAME	CONFIG_STREAMS_TSTMOD_NAME

#ifndef TSTMOD_MOD_NAME
#define TSTMOD_MOD_NAME	"tstmod"
#endif				/* TSTMOD_MOD_NAME */

#ifndef TSTMOD_MOD_ID
#define TSTMOD_MOD_ID	0
#endif				/* TSTMOD_MOD_ID */

/*
 *  STREAMS DEFINITIONS
 *  -------------------------------------------------------------------------
 */

#define MOD_ID		TSTMOD_MOD_ID
#define MOD_NAME	TSTMOD_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	TSTMOD_BANNER
#else				/* MODULE */
#define MOD_BANNER	TSTMOD_SPLASH
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
MODULE_PARM_DESC(modid, "Module ID for TSTMOD module.  (0 for allocation.)");

/** tstmodinit - initialize TSTMOD
  */
static __init int
tstmodinit(void)
{
	cmn_err(CE_NOTE, MOD_BANNER);
	return (0);
}

/** tstmodexit - terminate TSTMOD
  */
static __exit void
tstmodexit(void)
{
	return;
}

module_init(tstmodinit);
module_exit(tstmodexit);

#endif				/* LINUX */
