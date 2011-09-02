/*****************************************************************************

 @(#) $RCSfile: s_cdi.c,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-09-02 08:46:54 $

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

 Last Modified $Date: 2011-09-02 08:46:54 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: s_cdi.c,v $
 Revision 1.1.2.4  2011-09-02 08:46:54  brian
 - sync up lots of repo and build changes from git

 Revision 1.1.2.3  2011-01-18 16:55:53  brian
 - added stub drivers and modules

 Revision 1.1.2.2  2010-11-28 14:22:04  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:40:33  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: s_cdi.c,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-09-02 08:46:54 $";

/*
 * S_CDI is a simple conversion module that converts between a CDI Stream
 * supporting a WAN interface and the SpiderWAN raw HDLC and ASYNC interface.
 * This is simply to provide a conversion module for this utilities or legacy
 * application that need it.  This module may be autopushed onto a clone or
 * non-clone minor device number of a device driver Stream.
 */

#define _SVR4_SOURCE
#define _MPS_SOURCE
#define _SUN_SOURCE

#include <sys/os7/compat.h>

#define S_CDI_DESCRIP	"S_CDI STREAMS MODULE"
#define S_CDI_REVISION	"OpenSS7 $RCSfile: s_cdi.c,v $ $Name:  $($Revision: 1.1.2.4 $) $Date: 2011-09-02 08:46:54 $"
#define S_CDI_COPYRIGHT	"Copyright (c) 2008-2011  Monavacon Limited.  All Rights Reserved."
#define S_CDI_DEVICE	"Provides OpenSS7 SpiderWAN to CDI (S_CDI)"
#define S_CDI_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define S_CDI_LICENSE	"GPL"
#define S_CDI_BANNER	S_CDI_DESCRIP	"\n" \
			S_CDI_REVISION	"\n" \
			S_CDI_COPYRIGHT	"\n" \
			S_CDI_DEVICE	"\n" \
			S_CDI_CONTACT	"\n"
#define S_CDI_SPLASH	S_CDI_DEVICE	" - " \
			S_CDI_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(S_CDI_CONTACT);
MODULE_DESCRIPTION(S_CDI_DESCRIP);
MODULE_SUPPORTED_DEVICE(S_CDI_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(S_CDI_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-s_cdi");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define S_CDI_MOD_ID	CONFIG_STREAMS_S_CDI_MODID
#define S_CDI_MOD_NAME	CONFIG_STREAMS_S_CDI_NAME

#ifndef S_CDI_MOD_NAME
#define S_CDI_MOD_NAME	"s_cdi"
#endif				/* S_CDI_MOD_NAME */

#ifndef S_CDI_MOD_ID
#define S_CDI_MOD_ID	0
#endif				/* S_CDI_MOD_ID */

/*
 *  STREAMS DEFINITIONS
 *  -------------------------------------------------------------------------
 */

#define MOD_ID		S_CDI_MOD_ID
#define MOD_NAME	S_CDI_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	S_CDI_BANNER
#else				/* MODULE */
#define MOD_BANNER	S_CDI_SPLASH
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
MODULE_PARM_DESC(modid, "Module ID for S_CDI module.  (0 for allocation.)");

/** s_cdiinit - initialize S_CDI
  */
static __init int
s_cdiinit(void)
{
	cmn_err(CE_NOTE, MOD_BANNER);
	return (0);
}

/** s_cdiexit - terminate S_CDI
  */
static __exit void
s_cdiexit(void)
{
	return;
}

module_init(s_cdiinit);
module_exit(s_cdiexit);

#endif				/* LINUX */
