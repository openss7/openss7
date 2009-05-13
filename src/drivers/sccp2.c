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
 * This is a second generation SCCP driver.  The driver uses the capabilities of the MPS
 * compatibility module to provide facilities for SCCP.  The SCCP driver links MTP Streams under
 * the multiplexing pseudo-device driver and presents SCCP streams to the upper multiplex.  To
 * simplify the state machine, an NPI state machine and interface is used for both the upper and
 * lower multiplex.  The lower multiplex interface is a CLNS interface extended for use with
 * MTP.  The upper multiplex interface is either a CONS or CLNS interface extended for use with
 * SCCP.
 */

#define _MPS_SOURCE
#define _SVR4_SOURCE

#include <sys/os7/compat.h>
#include <sys/sunddi.h>
#include <sys/npi.h> /* NPI interface */
#include <sys/npi_mtp.h> /* NPI extensions for MTP */
#include <sys/npi_sccp.h> /* NPI extensions for SCCP */

static struct cdevsw sccp_cdev = {
	.d_name = DRV_NAME,
	.d_str = &sccpinfo,
	.d_flag = D_MP,
	.d_fop = S_IFCHR,
	.d_kmod = THIS_MODULE,
};
