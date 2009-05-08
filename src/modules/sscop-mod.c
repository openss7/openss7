/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

/*
 * This is an SSCOPE module for use with MTP3b.  It can be pushed over an AAL5 Stream to provide the
 * SSCF interface needed by MTP3b.  For the upper interface boundary, see Q.2210 (SSCOPE) and NPI.
 * For the lower interface boundary, see DLPI.
 */

#define SSCOP_DESCRIP	"MTP3B-SSCOP STREAMS MODULE."
#define SSCOP_REVISION	"OpenSS7 $RCSfile: sscop.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-12-06 09:06:06 $"
#define SSCOP_COPYRIGHT	"Copyright (c) 2008-2009  Monavacon Limited.  All Rights Reserved."
#define SSCOP_DEVICE	"Provides OpenSS7 MTP3B-I.432.3-SSCOP module."
#define SSCOP_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SSCOP_LICENSE	"GPL"
#define SSCOP_BANNER	SSCOP_DESCRIP	"\n" \
			SSCOP_REVISION	"\n" \
			SSCOP_COPYRIGHT	"\n" \
			SSCOP_DEVICE	"\n" \
			SSCOP_CONTACT	"\n"
#define SSCOP_SPLASH	SSCOP_DEVICE	" - " \
			SSCOP_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(SSCOP_CONTACT);
MODULE_DESCRIPTION(SSCOP_DESCRIP);
MODULE_SUPPORTED_DEVICE(SSCOP_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SSCOP_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-sscop");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#ifdef LFS
#define SSCOP_MOD_ID	CONFIG_STREAMS_SSCOP_MODID
#define SSCOP_MOD_NAME	CONFIG_STREAMS_SSCOP_NAME
#endif				/* LFS */

#define MOD_ID		SSCOP_MOD_ID
#define MOD_NAME	SSCOP_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	SSCOP_BANNER
#else				/* MODULE */
#define MOD_BANNER	SSCOP_SPLASH
#endif				/* MODULE */

static modID_t modid = MOD_ID;

#ifdef LINUX
#ifdef module_param
module_param(modid, ushort, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for SSCOP module. (0 for allocation.)");
#endif				/* LINUX */

static struct module_info sc_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat sc_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat sc_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit sc_rdinit = {
	.qi_putp = sc_rput,
	.qi_srvp = sc_rsrv,
	.qi_qopen = sc_qopen,
	.qi_close = sc_qclose,
	.qi_minfo = &sc_minfo,
	.qi_mstat = &sc_rstat,
};

static struct qinit sc_wrinit = {
	.qi_putp = sc_wput,
	.qi_srvp = sc_wsrv,
	.qi_minfo = &sc_minfo,
	.qi_mstat = &sc_wstat,
};

struct streamtab sscopinfo = {
	.st_rdinit = &sc_rdinit,
	.st_wrinit = &sc_wrinit,
};

static struct fmodsw sc_fmod = {
	.f_name = MOD_NAME,
	.f_str = &sscopinfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static int __init
sscopinit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&sc_fmod, modid)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static int
sscopterminate(void)
{
	int err;

	if ((err = unregister_strmod(&sc_fmod, modid)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	modid = 0;
	return (0);
}

static void __exit
sscopexit(void)
{
	sscopterminate();
}

module_init(sscopinit);
module_exit(sscopexit);

