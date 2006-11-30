/*****************************************************************************

 @(#) $RCSfile: tua_as.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/11/30 13:17:57 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 2 of the License.

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

 Last Modified $Date: 2006/11/30 13:17:57 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: tua_as.c,v $
 Revision 0.9.2.1  2006/11/30 13:17:57  brian
 - added files from strss7 package

 *****************************************************************************/

#ident "@(#) $RCSfile: tua_as.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/11/30 13:17:57 $"

static char const ident[] = "$RCSfile: tua_as.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/11/30 13:17:57 $";

/*
 *  This is the AS side of TUA implemented as a pushable module that pushes over an SCTP NPI
 *  stream.
 */

#define _LFS_SOURCE 1
#define _DEBUG 1

#include <sys/os7/compat.h>

#include <stdbool.h>

#include <linux/socket.h>
#include <net/ip.h>

#include <sys/npi.h>
#include <sys/npi_sctp.h>

#include <sys/tihdr.h>
#include <sys/tpi_tcap.h>

#define TUA_AS_DESCRIP		"TUA/SCTP SIGNALLING LINK (SL) STREAMS MODULE."
#define TUA_AS_REVISION		"OpenSS7 $RCSfile: tua_as.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2006/11/30 13:17:57 $"
#define TUA_AS_COPYRIGHT	"Copyright (c) 1997-2006 OpenSS7 Corporation.  All Rights Reserved."
#define TUA_AS_DEVICE		"Part of the OpenSS7 Stack for Linux Fast STREAMS."
#define TUA_AS_CONTACT		"Brian Bidulock <bidulock@openss7.org>"
#define TUA_AS_LICENSE		"GPL"
#define TUA_AS_BANNER		TUA_AS_DESCRIP		"\n" \
				TUA_AS_REVISION		"\n" \
				TUA_AS_COPYRIGHT	"\n" \
				TUA_AS_DEVICE		"\n" \
				TUA_AS_CONTACT		"\n"
#define TUA_AS_SPLASH		TUA_AS_DEVICE		" - " \
				TUA_AS_REVISION		"\n"

#ifdef LINUX
MODULE_AUTHOR(TUA_AS_CONTACT);
MODULE_DESCRIPTION(TUA_AS_DESCRIP);
MODULE_SUPPORTED_DEVICE(TUA_AS_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(TUA_AS_LICENSE);
#endif				/* MODULE_LICENSE */
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-tua_as");
#endif
#endif				/* LINUX */

#ifdef LFS
#define TUA_AS_MOD_ID		CONFIG_STREAMS_TUA_AS_MODID
#define TUA_AS_MOD_NAME		CONFIG_STREAMS_TUA_AS_NAME
#endif

