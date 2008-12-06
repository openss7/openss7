/*****************************************************************************

 @(#) $RCSfile: aal1.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-12-06 08:20:52 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2008-12-06 08:20:52 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: aal1.c,v $
 Revision 0.9.2.1  2008-12-06 08:20:52  brian
 - added base release files for stratm package

 *****************************************************************************/

#ident "@(#) $RCSfile: aal1.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-12-06 08:20:52 $"

static char const ident[] = "$RCSfile: aal1.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-12-06 08:20:52 $";

/*
 * This is an ATM Adaptation Layer (AAL) Type 1 (AAL1) for passing packets of fixed length across
 * the ATM network.  This is a pushable STREAMS module that may be pushed overa n ATM VC Stream to
 * effect the AAL1 service.  For the upper interface boundary, see MXI.  For the lower interface
 * boundary, see DLPI.
 */

#define AAL1_DESCRIP	"MTP3B-AAL1 STREAMS MODULE."
#define AAL1_REVISION	"OpenSS7 $RCSfile: aal1.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-12-06 08:20:52 $"
#define AAL1_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define AAL1_DEVICE	"Provides OpenSS7 MTP3B-I.432.3-AAL1 module."
#define AAL1_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define AAL1_LICENSE	"GPL"
#define AAL1_BANNER	AAL1_DESCRIP	"\n" \
			AAL1_REVISIOn	"\n" \
			AAL1_COPYRIGHT	"\n" \
			AAL1_DEVICE	"\n" \
			AAL1_CONTACT	"\n"
#define AAL1_SPLASH	AAL1_DEVICE	" - " \
			AAL1_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(AAL1_CONTACT);
MODULE_DESCRIPTION(AAL1_DESCRIP);
MODULE_SUPPORTED_DEVICE(AAL1_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(AAL1_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-aal1");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#ifdef LFS
#define AAL1_MOD_ID	CONFIG_STREAMS_AAL1_MODID
#define AAL1_MOD_NAME	CONFIG_STREAMS_AAL1_NAME
#endif				/* LFS */

#define MOD_ID		AAL1_MOD_ID
#define MOD_NAME	AAL1_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	AAL1_BANNER
#else				/* MODULE */
#define MOD_BANNER	AAL1_SPLASH
#endif				/* MODULE */

static modID_t modid = MOD_ID;

#ifdef LINUX
#ifdef module_param
module_param(modid, ushort, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for AAL1 module. (0 for allocation.)");
#endif				/* LINUX */

static struct module_info a1_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat a1_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat a1_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit a1_rdinit = {
	.qi_putp = a1_rput,
	.qi_srvp = a1_rsrv,
	.qi_qopen = a1_qopen,
	.qi_close = a1_qclose,
	.qi_minfo = &a1_minfo,
	.qi_mstat = &a1_rstat,
};

static struct qinit a1_wrinit = {
	.qi_putp = a1_wput,
	.qi_srvp = a1_wsrv,
	.qi_minfo = &a1_minfo,
	.qi_mstat = &a1_wstat,
};

struct streamtab aal1info = {
	.st_rdinit = &a1_rdinit,
	.st_wrinit = &a1_wrinit,
};

static struct fmodsw a1_fmod = {
	.f_name = MOD_NAME,
	.f_str = &aal1info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static int __init
aal1init(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&a1_fmod, modid)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static int
aal1terminate(void)
{
	int err;

	if ((err = unregister_strmod(&a1_fmod, modid)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	modid = 0;
	return (0);
}

static void __exit
aal1exit(void)
{
	aal1terminate();
}

module_init(aal1init);
module_exit(aal1exit);

