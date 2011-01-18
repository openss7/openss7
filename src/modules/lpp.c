/*****************************************************************************

 @(#) $RCSfile: lpp.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-01-18 16:55:53 $

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

 Last Modified $Date: 2011-01-18 16:55:53 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: lpp.c,v $
 Revision 1.1.2.3  2011-01-18 16:55:53  brian
 - added stub drivers and modules

 Revision 1.1.2.2  2010-11-28 14:22:02  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:40:31  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] =
    "$RCSfile: lpp.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-01-18 16:55:53 $";

/*
 *  Lightweight Presentation Protocol (LPP) module
 */

#define _SVR4_SOURCE
#define _MPS_SOURCE
#define _SUN_SOURCE

#include <sys/os7/compat.h>

#define LPP_DESCRIP	"LPP STREAMS MODULE"
#define LPP_REVISION	"OpenSS7 $RCSfile: lpp.c,v $ $Name:  $($Revision: 1.1.2.3 $) $Date: 2011-01-18 16:55:53 $"
#define LPP_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define LPP_DEVICE	"Provides OpenSS7 Lightweight Presentation Protocol (LPP)"
#define LPP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define LPP_LICENSE	"GPL"
#define LPP_BANNER	LPP_DESCRIP	"\n" \
			LPP_REVISION	"\n" \
			LPP_COPYRIGHT	"\n" \
			LPP_DEVICE	"\n" \
			LPP_CONTACT	"\n"
#define LPP_SPLASH	LPP_DEVICE	" - " \
			LPP_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(LPP_CONTACT);
MODULE_DESCRIPTION(LPP_DESCRIP);
MODULE_SUPPORTED_DEVICE(LPP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(LPP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-lpp");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "."
	       PACKAGE_RELEASE PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define LPP_MOD_ID	CONFIG_STREAMS_LPP_MODID
#define LPP_MOD_NAME	CONFIG_STREAMS_LPP_NAME

#ifndef LPP_MOD_NAME
#define LPP_MOD_NAME	"lpp"
#endif				/* LPP_MOD_NAME */

#ifndef LPP_MOD_ID
#define LPP_MOD_ID	0
#endif				/* LPP_MOD_ID */

/*
 *  STREAMS DEFINITIONS
 *  -------------------------------------------------------------------------
 */

#define MOD_ID		LPP_MOD_ID
#define MOD_NAME	LPP_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	LPP_BANNER
#else				/* MODULE */
#define MOD_BANNER	LPP_SPLASH
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
MODULE_PARM_DESC(modid, "Module ID for LPP module.  (0 for allocation.)");

/** lppinit - initialize LPP
  */
static __init int
lppinit(void)
{
	cmn_err(CE_NOTE, MOD_BANNER);
	return (0);
}

/** lppexit - terminate LPP
  */
static __exit void
lppexit(void)
{
	return;
}

module_init(lppinit);
module_exit(lppexit);

#endif				/* LINUX */
