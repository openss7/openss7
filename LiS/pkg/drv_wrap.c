/*****************************************************************************

 @(#) $RCSfile: drv_wrap.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-04-29 08:33:24 $

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

 Last Modified $Date: 2008-04-29 08:33:24 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: drv_wrap.c,v $
 Revision 0.9.2.5  2008-04-29 08:33:24  brian
 - update headers for Affero release

 Revision 0.9.2.4  2007/08/14 10:47:18  brian
 - GPLv3 header update

 Revision 0.9.2.3  2006/12/18 09:51:09  brian
 - updated headers for release

 Revision 0.9.2.2  2006/10/27 22:38:58  brian
 - changes for 2.6.18 build

 Revision 0.9.2.1  2006/07/08 22:04:49  brian
 - add wrapper files

 *****************************************************************************/

#ident "@(#) $RCSfile: drv_wrap.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-04-29 08:33:24 $"

static char const ident[] =
    "$RCSfile: drv_wrap.c,v $ $Name:  $($Revision: 0.9.2.5 $) $Date: 2008-04-29 08:33:24 $";

#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#define _XX_DESCRIP	"UNIX/SYSTEM V RELEASE 4 STREAMS FOR LINUX"
#define _XX_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define _XX_DEVICE	"LiS Binary Compatibility Wrapper"	/* get more from config */
#define _XX_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define _XX_LICENSE	"Proprietary"
#define _XX_BANNER	_XX_DESCRIP	"\n" \
			_XX_COPYRIGHT	"\n" \
			_XX_DEVICE	"\n" \
			_XX_CONTACT

MODULE_AUTHOR(_XX_CONTACT);
MODULE_DESCRIPTION(_XX_DESCRIP);
MODULE_SUPPORTED_DEVICE(_XX_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(_XX_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-xx");
#endif
#if defined MODULE_ALIAS
MODULE_ALIAS("char-major-" __stringify(@@_CMAJOR_0));
#ifdef @@_CMAJOR_1
MODULE_ALIAS("char-major-" __stringify(@@_CMAJOR_1));
#endif				/* @@_CMAJOR_1 */
#ifdef @@_CMAJOR_2
MODULE_ALIAS("char-major-" __stringify(@@_CMAJOR_2));
#endif				/* @@_CMAJOR_2 */
#ifdef @@_CMAJOR_3
MODULE_ALIAS("char-major-" __stringify(@@_CMAJOR_3));
#endif				/* @@_CMAJOR_3 */
#ifdef @@_CMAJOR_4
MODULE_ALIAS("char-major-" __stringify(@@_CMAJOR_4));
#endif				/* @@_CMAJOR_4 */
#ifdef @@_CMAJOR_5
MODULE_ALIAS("char-major-" __stringify(@@_CMAJOR_5));
#endif				/* @@_CMAJOR_5 */
#endif

extern struct module_info @@info;
extern void FASTCALL(@@init(void));
extern void FASTCALL(@@term(void));

static int _xx_majors[@@_CMAJORS] = {
	@@_CMAJOR_0,
#ifdef @@_CMAJOR_1
	@@_CMAJOR_1,
#endif					/* @@_CMAJOR_1 */
#ifdef @@_CMAJOR_2
	@@_CMAJOR_2,
#endif					/* @@_CMAJOR_2 */
#ifdef @@_CMAJOR_3
	@@_CMAJOR_3,
#endif					/* @@_CMAJOR_3 */
#ifdef @@_CMAJOR_4
	@@_CMAJOR_4,
#endif					/* @@_CMAJOR_4 */
#ifdef @@_CMAJOR_5
	@@_CMAJOR_5,
#endif					/* @@_CMAJOR_5 */
};

static void
_xx_del(void)
{
	int err, mindex;

	for (mindex = @@_CMAJORS - 1; mindex >= 0; mindex--) {
		if (_xx_majors[mindex]) {
			if ((err = lis_unregister_strdev(_xx_majors[mindex])))
				cmn_err(CE_PANIC, "%s: cannot unregister major %d", @@_DRV_NAME);
			_xx_majors[mindex] = 0;
		}
	}
}

static int
_xx_add(void)
{
	int err, mindex = 0;

	cmn_err(CE_NOTE, _XX_BANNER);	/* console splash */
	for (mindex = 0; mindex < @@_CMAJORS; mindex++) {
		if ((err = lis_register_strdev(_xx_majors[mindex], &@@info, @@_UNITS,
					       @@_DRV_NAME)) < 0) {
			if (mindex) {
				cmn_err(CE_WARN, "%s: could not register major %d", @@_DRV_NAME,
					_xx_majors[mindex]);
				continue;
			} else {
				cmn_err(CE_WARN, "%s: could not register driver, err = %d",
					@@_DRV_NAME, err);
				return (err);
			}
		}
		if (_xx_majors[mindex] == 0)
			_xx_majors[mindex] = err;
	}
	return (0);
}

static void __exit
_xx_exit(void)
{
	@@term();		/* only if we have a terminate function */
	_xx_del();
}

static int __init
_xx_init(void)
{
	int err;

	if ((err = _xx_add())) {
		_xx_del();
		return (err);
	}
	@@init();
	return (0);
}

module_init(_xx_init);
module_exit(_xx_exit);
