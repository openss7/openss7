/*****************************************************************************

 @(#) File: src/drivers/sdtmux.c

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

static char const ident[] = "src/drivers/sdtmux.c (" PACKAGE_ENVR ") " PACKAGE_DATE;

/*
 *  This is an SDT multiplexing driver for multiplexing SDT upper streams into
 *  SDT lower streams for provisioning and automatic allocation of Signalling
 *  Data Terminals.  It translates between Type I SDT streams or attached Type
 *  II SDT streams on the lower boundary to Type II SDT streams on the upper
 *  boundary.  Another purpose for the SDT mux is the ability to perform SDT
 *  monitoring, testing, management, statistics collection and event
 *  collection.
 */
#include <sys/os7/compat.h>

#include <ss7/lmi.h>
#include <ss7/lmi_ioctl.h>
#include <ss7/sdli.h>
#include <ss7/sdli_ioctl.h>
#include <ss7/sdti.h>
#include <ss7/sdti_ioctl.h>

#define SDTM_DESCRIP	"Signalling Data Terminal (SDT) STREAMS Multiplexing Driver"
#define SDTM_EXTRA	"Part of the OpenSS7 SS7 Stack for Linux Fast-STREAMS"
#define SDTM_REVISION	"OpenSS7 src/drivers/sdtmux.c (" PACKAGE_ENVR ") " PACKAGE_DATE
#define SDTM_COPYRIGHT	"Copyright (c) 2008-2015  Monavacon Limited.  All Rights Reserved."
#define SDTM_DEVICE	"Part of the OpenSS7 Stack for STREAMS."
#define SDTM_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SDTM_LICENSE	"GPL"
#define SDTM_BANNER	SDTM_DESCRIP	"\n" \
			SDTM_EXTRA	"\n" \
			SDTM_REVISION	"\n" \
			SDTM_COPYRIGHT	"\n" \
			SDTM_DEVICE	"\n" \
			SDTM_CONTACT
#define SDTM_SPLASH	SDTM_DEVICE	" - " \
			SDTM_REVISION	"\n" \

#ifdef LINUX
MODULE_AUTHOR(SDTM_CONTACT);
MODULE_DESCRIPTION(SDTM_DESCRIP);
MODULE_SUPPORTED_DEVICE(SDTM_DEVICE);
#ifdef MODULE_LICENSE
MODULE_LICENSE(SDTM_LICENSE);
#endif
#if defined MODULE_ALIAS
MODULE_ALIAS("streams-sdtmux");
#endif
#ifdef MODULE_VERSION
MODULE_VERSION(PACKAGE_ENVR);
#endif
#endif				/* LINUX */
