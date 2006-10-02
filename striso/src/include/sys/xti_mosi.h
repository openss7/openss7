/*****************************************************************************

 @(#) $Id: xti_mosi.h,v 0.9.2.1 2006/10/02 11:31:50 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2006  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2006/10/02 11:31:50 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xti_mosi.h,v $
 Revision 0.9.2.1  2006/10/02 11:31:50  brian
 - changes to get master builds working for RPM and DEB
 - added outside licenses to package documentation
 - added LICENSE automated release file
 - copy MANUAL to source directory
 - add and remove devices in -dev debian subpackages
 - get debian rules working better
 - release library version files
 - added notes to debian changelog
 - corrections for cooked manual pages in spec files
 - added release documentation to spec and rules files
 - copyright header updates
 - moved controlling tty checks in stream head
 - missing some defines for LiS build in various source files
 - added OSI headers to striso package
 - added includes and manual page paths to acincludes for various packages
 - added sunrpc, uidlpi, uinpi and uitpi licenses to documentation and release
   files
 - moved pragma weak statements ahead of declarations
 - changes for master build of RPMS and DEBS with LiS

 Revision 0.9.2.7  2006/09/25 12:04:43  brian
 - updated headers, moved xnsl

 *****************************************************************************/

#ifndef _SYS_XTI_MOSI_H
#define _SYS_XTI_MOSI_H

#ident "@(#) $RCSfile: xti_mosi.h,v $ $Name:  $($Revision: 0.9.2.1 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup xnet
  * @{ */

/** @file
  * XTI Minimal OSI header file.  */

/*
 * Minimal OSI Header File.
 */

#define T_AP_MAX_ADDR	128

/**
  * MOSI address structure.
  */
struct t_mosiaddr {
	t_uscalar_t flags;
	t_scalar_t osi_ap_inv_id;
	t_scalar_t osi_ae_inv_id;
	unsigned int osi_apt_len;
	unsigned int osi_aeq_len;
	unsigned int osi_paddr_len;
	unsigned char osi_addr[T_AP_MAX_ADDR];
};

#define T_ISO_APCO	0x0200
#define T_ISO_APCL	0x0300
#define T_AP_CNTX_NAME	0x1
#define T_AP_PCL	0x2

#define T_OPT_VALEN(opt) (opt->len - sizeof(struct t_opthder)).

/**
  * Presentation Context structure.
  * Presentation context definition and result list option.
  */
struct t_ap_pco_el {
	t_scalar_t count;
	t_scalar_t offset;
};

/**
  * Presentation Context item header.
  */
struct t_ap_pc_item {
	t_scalar_t pci;			/**< Unique odd integer. */
	t_scalar_t res;			/**< Result of negotiation. */
};

/**
  * Presentation Context item element.
  */
struct t_app_syn_off {
	t_scalar_t size;		/**< Length of syntax object identifier contents. */
	t_scalar_t offset;		/**< Offset of object identifier for the syntax. */
};

/**
  * @name Presentation Context Results
  * Values for res of a presentation context item.
  *
  * @{ */
#define T_PCL_ACCEPT		    0x0000	/**< Pres context accepted. */
#define T_PCL_USER_REJ		    0x0100	/**< Pres context rejected by perr application. */
#define T_PCL_PREJ_RSN_NSPEC	    0x0200	/**< Prov reject: no reason specified. */
#define T_PCL_PREJ_A_SYTX_NSUP	    0x0201	/**< Prov reject: abstract syntax not supported. */
#define T_PCL_PREJ_T_SYTX_NSUP	    0x0202	/**< Prov reject: transfer syntax not supported. */
#define T_PCL_PREJ_LMT_DCS_EXCEED   0x0203	/**< Prov reject: local limit on DCS exceeded. */
/** @} */

/**
  * @name Disconnect Reasons
  * Reason codes for disconnection.
  *
  * @{ */
#define T_AC_U_AARE_NONE	    0x0001	/**< Con rej by peer user: no reason given. */
#define T_AC_C_U_AARE_ACN	    0x0002	/**< Con rej: application context name not supported. */
#define T_AC_U_AARE_APT		    0x0003	/**< Con rej: AP title not recognized. */
#define T_AC_U_AARE_AEQ		    0x0005	/**< Con rej: AE qualifier not recognized. */
#define T_AC_U_AARE_PEER_AUTH	    0x000e	/**< Con rej: authentication required. */
#define T_AC_P_ABRT_NSPEC	    0x0011	/**< Aborted by peer: not reason given. */
#define T_AC_P_AARE_VERSION	    0x0012	/**< Con rej: no common version. */
/** @} */

#endif				/* _SYS_XTI_MOSI_H */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
