/*****************************************************************************

 @(#) $RCSfile: sdtmux.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2008-04-29 07:11:10 $

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

 Last Modified $Date: 2008-04-29 07:11:10 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sdtmux.c,v $
 Revision 0.9.2.12  2008-04-29 07:11:10  brian
 - updating headers for release

 Revision 0.9.2.11  2007/08/15 05:20:15  brian
 - GPLv3 updates

 Revision 0.9.2.10  2007/08/14 12:18:45  brian
 - GPLv3 header updates

 *****************************************************************************/

#ident "@(#) $RCSfile: sdtmux.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2008-04-29 07:11:10 $"

static char const ident[] =
    "$RCSfile: sdtmux.c,v $ $Name:  $($Revision: 0.9.2.12 $) $Date: 2008-04-29 07:11:10 $";

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

#define SDTM_DESCRIP	"SIGNALLING DATA TERMINAL (SDT) STREAMS MULTIPLEXING DRIVER."
#define SDTM_COPYRIGHT	"Copyright (c) 1997-2008 OpenSS7 Corporation.  All Rights Reserved."
#define SDTM_DEVICE	"Part of the OpenSS7 Stack for STREAMS."
#define SDTM_CONTACT	"Brian Bidulock <bidulock@openss7.org>"
#define SDTM_LICENSE	"GPL"
#define SDTM_BANNER	SDTM_DESCRIP	"\n" \
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
#endif				/* LINUX */
