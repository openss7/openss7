/*****************************************************************************

 @(#) File: src/modules/mtp3b.c

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

static char const ident[] = "src/modules/mtp3b.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 * This is an MTP3b module for use by MTP3b.  It can be pushed over an SSCOPE SSCF module to provide
 * the service needed by MTP and B-ISUP.  The lower boundary is the SSCOPE-SSCF interface (basically
 * an extension of NPI), the upper boundary is the SLI (Signalling Link Interface).  For the lower
 * interface boundary, see Q.2210 (SSCOPE) and Q.2211 (SSCOPE-MCE) and NPI.  For the upper interface
 * boundary, see Q.2140 and Q.703 and SLI.
 */

#define MTP3B_DESCRIP	"MTP3B-MTP3B STREAMS Module"
#define MTP3B_EXTRA	"Part of the OpenSS7 ATM Stack for Linux Fast-STREAMS"
#define MTP3B_REVISION	"OpenSS7 File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE
#define MTP3B_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define MTP3B_DEVICE	"Provides OpenSS7 MTP3B-I.432.3-MTP3B module."
#define MTP3B_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define MTP3B_LICENSE	"GPL"
#define MTP3B_BANNER	MTP3B_DESCRIP	"\n" \
			MTP3B_EXTRA	"\n" \
			MTP3B_REVISIOn	"\n" \
			MTP3B_COPYRIGHT	"\n" \
			MTP3B_DEVICE	"\n" \
			MTP3B_CONTACT	"\n"
#define MTP3B_SPLASH	MTP3B_DEVICE	" - " \
			MTP3B_REVISION	"\n"

#ifdef LINUX
MODULE_AUTHOR(MTP3B_CONTACT);
MODULE_DESCRIPTION(MTP3B_DESCRIP);
MODULE_SUPPORTED_DEVICE(MTP3B_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(MTP3B_LICENSE);
#endif				/* MODULE_LICENSE */
#ifdef MODULE_ALIAS
MODULE_ALIAS("streams-mtp3b");
#endif				/* MODULE_ALIAS */
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif				/* MODULE_VERSION */
#endif				/* LINUX */

#define MTP3B_MOD_ID	CONFIG_STREAMS_MTP3B_MODID
#define MTP3B_MOD_NAME	CONFIG_STREAMS_MTP3B_NAME

#define MOD_ID		MTP3B_MOD_ID
#define MOD_NAME	MTP3B_MOD_NAME
#ifdef MODULE
#define MOD_BANNER	MTP3B_BANNER
#else				/* MODULE */
#define MOD_BANNER	MTP3B_SPLASH
#endif				/* MODULE */

static modID_t modid = MOD_ID;

#ifdef LINUX
#ifdef module_param
module_param(modid, ushort, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for MTP3B module. (0 for allocation.)");
#endif				/* LINUX */

static modID_t modid = MOD_ID;

#ifdef LINUX
#ifdef module_param
module_param(modid, ushort, 0444);
#else				/* module_param */
MODULE_PARM(modid, "h");
#endif				/* module_param */
MODULE_PARM_DESC(modid, "Module ID for MTP3B module. (0 for allocation.)");
#endif				/* LINUX */

static struct module_info mt_minfo = {
	.mi_idnum = MOD_ID,
	.mi_idname = MOD_NAME,
	.mi_minpsz = STRMINPSZ,
	.mi_maxpsz = STRMAXPSZ,
	.mi_hiwat = STRHIGH,
	.mi_lowat = STRLOW,
};

static struct module_stat mt_rstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));
static struct module_stat mt_wstat __attribute__ ((__aligned__(SMP_CACHE_BYTES)));

static struct qinit mt_rdinit = {
	.qi_putp = mt_rput,
	.qi_srvp = mt_rsrv,
	.qi_qopen = mt_qopen,
	.qi_close = mt_qclose,
	.qi_minfo = &mt_minfo,
	.qi_mstat = &mt_rstat,
};

static struct qinit mt_wrinit = {
	.qi_putp = mt_wput,
	.qi_srvp = mt_wsrv,
	.qi_minfo = &mt_minfo,
	.qi_mstat = &mt_wstat,
};

struct streamtab mtp3binfo = {
	.st_rdinit = &mt_rdinit,
	.st_wrinit = &mt_wrinit,
};

static struct fmodsw mt_fmod = {
	.f_name = MOD_NAME,
	.f_str = &mtp3binfo,
	.f_flag = D_MP,
	.f_kmod = THIS_MODULE,
};

static int __init
mtp3binit(void)
{
	int err;

	cmn_err(CE_NOTE, MOD_BANNER);
	if ((err = register_strmod(&mt_fmod, modid)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	if (modid == 0)
		modid = err;
	return (0);
}

static int
mtp3bterminate(void)
{
	int err;

	if ((err = unregister_strmod(&mt_fmod, modid)) < 0) {
		cmn_err(CE_WARN, "%s: could not unregister module %d, err = %d\n", MOD_NAME,
			(int) modid, -err);
		return (err);
	}
	modid = 0;
	return (0);
}

static void __exit
mtp3bexit(void)
{
	mtp3bterminate();
}

module_init(mtp3binit);
module_exit(mtp3bexit);
