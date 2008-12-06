/*****************************************************************************

 @(#) $RCSfile: aal2.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-12-06 09:06:06 $

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

 Last Modified $Date: 2008-12-06 09:06:06 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: aal2.c,v $
 Revision 0.9.2.1  2008-12-06 09:06:06  brian
 - added working files

 *****************************************************************************/

#ident "@(#) $RCSfile: aal2.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-12-06 09:06:06 $"

static char const ident[] = "$RCSfile: aal2.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-12-06 09:06:06 $";

/*
 * This is an ATM Adaptation Layer (AAL) Type 2 (AAL2) for passing packets of fixed length across
 * the ATM network.  This is a pushable STREAMS module that may be pushed overa n ATM VC Stream to
 * effect the AAL2 service.  For the upper interface boundary, see MXI.  For the lower interface
 * boundary, see DLPI.
 */

#define AAL2_DESCRIP	"MTP3B-AAL2 STREAMS MODULE."
#define AAL2_REVISION	"OpenSS7 $RCSfile: aal2.c,v $ $Name: OpenSS7-0_9_2 $($Revision: 0.9.2.1 $) $Date: 2008-12-06 09:06:06 $"
#define AAL2_COPYRIGHT	"Copyright (c) 1997-2008  OpenSS7 Corporation.  All Rights Reserved."
#define AAL2_DEVICE	"Provides OpenSS7 MTP3B-I.432.3-AAL2 module."
#define AAL2_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define AAL2_LICENSE	"GPL"
#define AAL2_BANNER	AAL2_DESCRIP	"\n" \
			AAL2_REVISIOn	"\n" \
			AAL2_COPYRIGHT	"\n" \
			AAL2_DEVICE	"\n" \
			AAL2_CONTACT	"\n"
#define AAL2_SPLASH	AAL2_DEVICE	" - " \
			AAL2_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(AAL2_CONTACT);
MODULE_DESCRIPTION(AAL2_DESCRIP);
MODULE_SUPPORTED_DEVICE(AAL2_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(AAL2_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-aal2");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(__stringify(PACKAGE_RPMEPOCH) ":" PACKAGE_VERSION "." PACKAGE_RELEASE
	       PACKAGE_PATCHLEVEL "-" PACKAGE_RPMRELEASE PACKAGE_RPMEXTRA2);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#ifdef LFS
#define AAL2_MOD_ID	CONFIG_STREAMS_AAL2_MODID
#define AAL2_MOD_NAME	CONFIG_STREAMS_AAL2_NAME
#endif				/* LFS */

#define MOD_ID		AAL2_MOD_ID
#define MOD_NAME	AAL2_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	AAL2_BANNER
#else				/* MODULE */
#define MOD_BANNER	AAL2_SPLASH
#endif				/* MODULE */

static modID_t modid = MOD_ID;

#ifdef LINUX
#ifdef module_param
module_param(modid, ushort, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for AAL2 module. (0 for allocation.)");
#endif				/* LINUX */

static struct module_info a2_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat a2_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat a2_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit a2_rdinit = {
	.qi_putp = a2_rput,
	.qi_srvp = a2_rsrv,
	.qi_qopen = a2_qopen,
	.qi_close = a2_qclose,
	.qi_minfo = &a2_minfo,
	.qi_mstat = &a2_rstat,
};

static struct qinit a2_wrinit = {
	.qi_putp = a2_wput,
	.qi_srvp = a2_wsrv,
	.qi_minfo = &a2_minfo,
	.qi_mstat = &a2_wstat,
};

struct streamtab aal2info = {
	.st_rdinit = &a2_rdinit,
	.st_wrinit = &a2_wrinit,
};

static struct fmodsw a2_fmod = {
	.f_name = MOD_NAME,
	.f_str = &aal2info,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static int __init
aal2init(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&a2_fmod, modid)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static int
aal2terminate(void)
{
	int err;

	if ((err = unregister_strmod(&a2_fmod, modid)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	modid = 0;
	return (0);
}

static void __exit
aal2exit(void)
{
	aal2terminate();
}

module_init(aal2init);
module_exit(aal2exit);
