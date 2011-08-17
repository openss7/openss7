/*****************************************************************************

 @(#) $RCSfile: cmot.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-01-18 16:55:52 $

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

 $Log: cmot.c,v $
 Revision 1.1.2.3  2011-01-18 16:55:52  brian
 - added stub drivers and modules

 Revision 1.1.2.2  2010-11-28 14:22:01  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:40:30  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] =
    "$RCSfile: cmot.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-01-18 16:55:52 $";

/*
 *  CMISE over TCP (CMOT and CMIP).
 */

#define _SVR4_SOURCE    1
#define _MPS_SOURCE     1
#define _SUN_SOURCE     1

#include <sys/os7/compat.h>

/* ---------------------- */

#define CMOT_DESCRIP	"CMOT STREAMS MODULE"
#define CMOT_EXTRA      "Part of the OpenSS7 OSI Stack for Linux Fast-STREAMS"
#define CMOT_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define CMOT_REVISION	"OpenSS7 $RCSfile: cmot.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-01-18 16:55:52 $"
#define CMOT_DEVICE	"SVR 4.2MP CMISE over TCP (CMOT)"
#define CMOT_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define CMOT_LICENSE	"GPL"
#define CMOT_BANNER	CMOT_DESCRIP	"\n" \
			CMOT_EXTRA	"\n" \
			CMOT_COPYRIGHT	"\n" \
			CMOT_REVISION	"\n" \
			CMOT_DEVICE	"\n" \
			CMOT_CONTACT	"\n"
#define CMOT_SPLASH	CMOT_DESCRIP	" - " \
			CMOT_REVISION

#ifndef CONFIG_STREAMS_CMOT_NAME
#error "CONFIG_STREAMS_CMOT_NAME must be defined."
#endif				/* CONFIG_STREAMS_CMOT_NAME */
#ifndef CONFIG_STREAMS_CMOT_MODID
#error "CONFIG_STREAMS_CMOT_MODID must be defined."
#endif				/* CONFIG_STREAMS_CMOT_MODID */

#ifdef LINUX
#ifdef MODULE
MODULE_AUTHOR(CMOT_CONTACT);
MODULE_DESCRIPTION(CMOT_DESCRIP);
MODULE_SUPPORTED_DEVICE(CMOT_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(CMOT_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-cmot");
MODULE_ALIAS("streams-module-cmot");
MODULE_ALIAS("streams-modid-" __stringify(CONFIG_STREAMS_CMOT_MODID));
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* MODULE */
#endif				/* LINUX */

#define CMOT_MOD_ID	CONFIG_STREAMS_CMOT_MODID
#define CMOT_MOD_NAME	CONFIG_STREAMS_CMOT_NAME

#ifndef CMOT_MOD_NAME
#define CMOT_MOD_NAME	"cmot"
#endif				/* CMOT_MOD_NAME */

#ifndef CMOT_MOD_ID
#define CMOT_MOD_ID	0
#endif				/* CMOT_MOD_ID */

/*
 *  STREAMS DEFINITIONS
 *  -------------------------------------------------------------------------
 */

#define MOD_ID		CMOT_MOD_ID
#define MOD_NAME	CMOT_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	CMOT_BANNER
#else				/* MODULE */
#define MOD_BANNER	CMOT_SPLASH
#endif				/* MODULE */

/*
 *  LINUX INITIALIZATION
 *  --------------------
 */


#ifdef LINUX
unsigned short modid = MOD_ID;

#ifndef module_param
MODULE_PARM(modid, "h");
#else				/* module_param */
module_param(modid, short, 0444);
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for CMOT module.  (0 for allocation.)");

/** cmotinit - initialize CMOT
  */
static __init int
cmotinit(void)
{
	cmn_err(CE_NOTE, MOD_BANNER);
	return (0);
}

/** cmotexit - terminate CMOT
  */
static __exit void
cmotexit(void)
{
	return;
}

module_init(cmotinit);
module_exit(cmotexit);

#endif				/* LINUX */
