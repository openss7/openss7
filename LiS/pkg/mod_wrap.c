/*****************************************************************************

 @(#) $RCSfile: mod_wrap.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/27 22:38:58 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2006/10/27 22:38:58 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: mod_wrap.c,v $
 Revision 0.9.2.2  2006/10/27 22:38:58  brian
 - changes for 2.6.18 build

 Revision 0.9.2.1  2006/07/08 22:04:50  brian
 - add wrapper files

 *****************************************************************************/

#ident "@(#) $RCSfile: mod_wrap.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/27 22:38:58 $"

static char const ident[] = "$RCSfile: mod_wrap.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2006/10/27 22:38:58 $";

#include <linux/autoconf.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>

#include <sys/kmem.h>
#include <sys/stream.h>
#include <sys/strconf.h>
#include <sys/ddi.h>

#define _XX_DESCRIP	"UNIX/SYSTEM V RELEASE 4 STREAMS FOR LINUX"
#define _XX_COPYRIGHT	"Copyright (c) 1997-2005 OpenSS7 Corporation.  All Rights Reserved."
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
MODULE_ALIAS("streams-" @@_MOD_NAME);
#endif

extern struct module_info @@info;
extern void FASTCALL(@@init(void));
extern void FASTCALL(@@term(void));

static void
_xx_del(void)
{
	int err;

	if ((err = lis_unregister_strmod(&@@info)) < 0) {
		cmn_err(CE_PANIC, "%s: cannot unregister module, err = %d", @@_MOD_NAME, err);
		return (err);
	}
	return (0);
}

static int
__xx_add(void)
{
	int err;

	cmn_err(CE_NOTE, _XX_BANNER);	/* console splash */
	if ((err = lis_register_strmod(&@@info, @@_MOD_NAME)) < 0) {
		cmn_err(CE_WARN, "%s: could not register module, err = %d", @@_MOD_NAME, err);
		return (err);
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
