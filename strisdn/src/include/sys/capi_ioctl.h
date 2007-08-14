/*****************************************************************************

 @(#) $Id: capi_ioctl.h,v 0.9.2.3 2007/08/14 07:41:28 brian Exp $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; version 3 of the License.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

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

 Last Modified $Date: 2007/08/14 07:41:28 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: capi_ioctl.h,v $
 Revision 0.9.2.3  2007/08/14 07:41:28  brian
 - GPLv3 header update

 Revision 0.9.2.2  2007/06/17 01:57:01  brian
 - updates for release, remove any later language

 *****************************************************************************/

#ifndef __CAPI_IOCTL_H__
#define __CAPI_IOCTL_H__

#ident "@(#) $RCSfile: capi_ioctl.h,v $ $Name:  $($Revision: 0.9.2.3 $) Copyright (c) 2001-2007 OpenSS7 Corporation."

/* This file can be processed by doxygen(1). */

#define CAPI_IOC_MAGIC	'C'

typedef struct capi_register_params {
	uint32_t maxLogicalConnection;	/* maximum number of logical connections */
	uint32_t maxBDataBlocks;	/* maximum number of received data blocks */
	uint32_t maxBDataLength;	/* maximum size of application data block */
} capi_register_params_t;

#define CAPI_REGISTER		_IOW( CAPI_IOC_MAGIC, 0x01, capi_register_params_t )
#define CAPI_GET_MANUFACTURER	_IOR( CAPI_IOC_MAGIC, 0x06, 64 )

typedef struct capi_version {
	uint32_t major_version;
	uint32_t minor_version;
	uint32_t vendor_major_version;
	uint32_t vendor_minor_version;
} capi_version_t;

#define CAPI_GET_VERSION	_IOR( CAPI_IOC_MAGIC, 0x07, capi_version_t )
#define CAPI_GET_SERIAL_NUMBER	_IOR( CAPI_IOC_MAGIC, 0x08, 8 )

typedef union capi_profile_params {
	struct {
		uint32_t controller_number;
	} request;
	struct {
		uint16_t num_controllers;
		uint16_t num_b_channels;
		uint32_t global_operations;
		uint32_t b1_protocol_support;
		uint32_t b2_protocol_support;
		uint32_t b3_protocol_support;
		uint8_t reserved[24];
		uint8_t vendor_specific[20];
	} result;
	capi_profile_t result;
} capi_profile_params_t;

#define CAPI_GET_PROFILE	_IOWR( CAPI_IOC_MAGIC, 0x09, capi_profile_params_t )

#endif				/* __CAPI_IOCTL_H__ */
