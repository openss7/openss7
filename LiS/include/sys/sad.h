/*****************************************************************************

 @(#) $Id: sad.h,v 1.1.1.1.12.2 2005/07/13 12:01:19 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>

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

 Last Modified $Date: 2005/07/13 12:01:19 $ by $Author: brian $

 *****************************************************************************/

/*
 *  sad.h  - declarations for Streams Administrative Driver and autopush
 */

#ifndef _SYS_SAD_H
#define _SYS_SAD_H

#ident "@(#) $RCSfile: sad.h,v $ $Name:  $($Revision: 1.1.1.1.12.2 $) $Date: 2005/07/13 12:01:19 $"

/* This file can be processed with doxygen(1). */

#ifndef _SYS_STROPTS_H
#include <sys/stropts.h>	/* for FMNAMESZ */
#endif

/** @ingroup strdrv
  * @{
  * @file
  * STREAMS Adminisrative Device sad(4) header file.
  * @{
  */

/**
  * @name sad(4) Driver Devices
  * In LiS, the device is a simple cloneable node.
  * Only root can do admin.
  * @{ */
#define USERDEV  "/dev/sad"	/**< User sad device. */
#define ADMINDEV "/dev/sad"	/**< Admin sad device. */
/** @} */

#define MAXAPUSH	8	/**< Maximum number of autopushed modules. */

/** @name SAD Magic Number
  * @{ */
#define SADIOC	('D' << 8)
/** @} */

/** @name SAD Input-Output Controls
  * @{ */
#define SAD_SAP	(SADIOC + 1)	/**< Set autopush configuration. */
#define SAD_GAP	(SADIOC + 2)	/**< Get autopush configuration. */
#define SAD_VML	(SADIOC + 3)	/**< Validate list of modules. */
/** @} */

/** Common autopush structure. */
struct apcommon {
	unsigned apc_cmd;	/**< Command. */
	long apc_major;		/**< Major device number. */
	long apc_minor;		/**< First minor device number. */
	long apc_lastminor;	/**< Last minor device number. */
	unsigned int apc_npush;	/**< Number of modules to autopush. */
};

/** @name SAP Command Values
  * Values for apcommon.apc_cmd field above.
  * @{ */
#define SAP_CLEAR	0	/**< Clear an entry. */
#define SAP_ONE		1	/**< Add an entry for one minor. */
#define SAP_RANGE	2	/**< Add an entry for range of minors. */
#define SAP_ALL		3	/**< Add an entry for all minors. */
/** @} */

/** STREAMS autopush structure.
  * SAD_SAP and SAD_GAP use this structure. */
struct strapush {
	struct apcommon sap_common;
	char sap_list[MAXAPUSH][FMNAMESZ + 1];
};

/** @name SAP Common Fields
  * Short names for strapush.sap_common fields above.
  * @{ */
#define sap_cmd		sap_common.apc_cmd
#define sap_major	sap_common.apc_major
#define sap_minor	sap_common.apc_minor
#define sap_lastminor	sap_common.apc_lastminor
#define sap_npush	sap_common.apc_npush
/** @} */

/** @}
  * @}
  */

#endif

// vim: ft=cpp com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
