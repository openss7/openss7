/*****************************************************************************

 @(#) $Id: sad.h,v 0.9.2.19 2008-04-28 12:54:02 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation; version 3 of the License.

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

 Last Modified $Date: 2008-04-28 12:54:02 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: sad.h,v $
 Revision 0.9.2.19  2008-04-28 12:54:02  brian
 - update file headers for release

 Revision 0.9.2.18  2007/12/15 20:19:50  brian
 - updates

 Revision 0.9.2.17  2007/08/13 22:46:08  brian
 - GPLv3 header updates

 Revision 0.9.2.16  2006/09/23 00:35:53  brian
 - prepared header file for doxygen

 Revision 0.9.2.15  2006/09/18 13:52:41  brian
 - added doxygen markers to sources

 Revision 0.9.2.14  2006/03/03 10:57:11  brian
 - 32-bit compatibility support, updates for release

 Revision 0.9.2.13  2006/02/20 10:59:20  brian
 - updated copyright headers on changed files

 *****************************************************************************/

#ifndef __SYS_STREAMS_SAD_H__
#define __SYS_STREAMS_SAD_H__

#ident "@(#) $RCSfile: sad.h,v $ $Name:  $($Revision: 0.9.2.19 $) Copyright (c) 2001-2008 OpenSS7 Corporation."

#ifndef __SYS_SAD_H__
#warning "Do no include sys/streams/sad.h directly, include sys/sad.h instead."
#endif

/* This file can be processed with doxygen(1). */

/**
 * @ingroup sad STREAMS Administrative Driver sad(4)
 * @{
 * @file
 * STREAMS Administrative Driver, sad(4), header file.
 * Provides the interface between a user program and the kernel for the adminstration of STREAMS
 * modules and drivers.  The sad(4) driver can verify a module list and attempt to load modules,
 * and configure or clear an autopush list.  All references to modules are perofrmed using the name
 * of the module (as a character string), however, communication with drivers requires knowledge of
 * a major device number associated with the driver.  Although this cat be obtained from
 * /proc/devices, it is classically obtained using the STREAMS Configuration Module, sc(4), that can
 * be pushed over either a Null Stream, nuls(4), or this sad(4) driver.
 * @{ */

/**
 * @name SAD IO Control Magic Number
 * @{ */
#define SAD_IOC_MAGIC	'D'	/**< Note: OSF/1 1.2 uses 'A' instead instead of 'D'. */
/** @} */

/**
 * @name SAD IO Control Commands
 * @{ */
#define SAD_SAP		((SAD_IOC_MAGIC << 8) | 0x01)	/**< Set autopush. */
#define SAD_GAP		((SAD_IOC_MAGIC << 8) | 0x02)	/**< Get autopush. */
#define SAD_VML		((SAD_IOC_MAGIC << 8) | 0x03)	/**< Validate modules. */

#define SAD_SAP_SOL	((SAD_IOC_MAGIC << 8) | 0x17)	/**< Set autopush (anchor). */
#define SAD_GAP_SOL	((SAD_IOC_MAGIC << 8) | 0x18)	/**< Get autopush (anchor). */
/** @} */

#ifndef MAX_APUSH
#define MAX_APUSH 8	/**< Maximum number of modules to autopush. */
#endif

#define MAXAPUSH MAX_APUSH /**< For compat w/ some imp's mis-spelling of the constant.  */

#ifdef __LP64__
#ifdef __KERNEL__
/*
 * This structure is only useful within the kernel for performing conversion
 * from 32-bit users to the 64-bit kernel.
 */
struct strapush32 {
	int32_t sap_cmd;
	int32_t sap_major;
	int32_t sap_minor;
	int32_t sap_lastminor;
	u_int32_t sap_npush;
	char sap_list[MAXAPUSH][FMNAMESZ + 1];
	/* Solaris adds sap_anchor which is the integer anchor position. */
	int32_t sap_anchor;
	/* This one is Linux Fast-STREAMS specific. */
	char sap_module[FMNAMESZ + 1];	/* This is mine. */
};
#endif				/* __KERNEL__ */
#endif				/* __LP64__ */

/**
 * @struct strapush include/sys/streams/sad.h <sys/sad.h>
 * @brief STREAMS Administrative Driver Autopush structure.
 * The STREAMS autopush structure is used to issue a STREAMS autopush command to
 * the STREAMS Adminstrative Driver, sad(4).
 */
struct strapush {
	int sap_cmd;			/**< Command. #SAP_CLEAR, #SAP_ONE, #SAP_RANGE,
					     #SAP_ALL, #SAP_CLONE. */
	long sap_major;			/**< Major device number of driver. */
	long sap_minor;			/**< Minor device number of driver. */
	long sap_lastminor;		/**< Last minor device number in a range (#SAP_RANGE). */
	uint sap_npush;			/**< Number of modules in list. */
	char sap_list[MAXAPUSH][FMNAMESZ + 1];	/**< Module list. */
	int sap_anchor;			/**< Anchor position (Solaris). */
	char sap_module[FMNAMESZ + 1];	/**< Module name (Linux Fast-STREAMS). */
};

/**
 * @name SAP Commands
 * @{
 * @def SAP_CLEAR
 *	Clear an autopush entry.
 * @def SAP_ONE
 *	Add an autopush entry for a single minor device number.
 * @def SAP_RANGE
 *	Add an autopush entry for a range of minor device numbers.
 * @def SAP_ALL
 *	Add an autopush entry fo all minor device numbers belonging to a major
 *	device number.
 * @def SAP_CLONE
 *	Mark the minor device number as a clonable minor device.  This permits a
 *	driver to treat one or more minor device numbers associated with a major
 *	deivce number as a clone minor.  This is not possible using the clone(4)
 *	driver for any minor device number other than zero (0).  This value is
 *	not standard SVR4 behaviour; however, the value is provided by a number
 *	of STREAMS implementations: it is provided here and supported by Linux
 *	Fast-STREAMS for compatibility when porting drivers and modules.
 */
#define SAP_CLEAR	0x00	/**< Clear entry. */
#define SAP_ONE		0x01	/**< Add entry for one minor. */
#define SAP_RANGE	0x02	/**< Add entry for range of minors. */
#define SAP_ALL		0x03	/**< Add entry for all minors. */
#define SAP_CLONE	0x04	/**< Mark clonable minor device. */
/** @} */

/** @} */

#endif				/* __SYS_STREAMS_SAD_H__ */
