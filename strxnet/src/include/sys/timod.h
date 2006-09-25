/*****************************************************************************

 @(#) $Id: timod.h,v 0.9.2.9 2006/09/25 12:04:43 brian Exp $

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

 Last Modified $Date: 2006/09/25 12:04:43 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: timod.h,v $
 Revision 0.9.2.9  2006/09/25 12:04:43  brian
 - updated headers, moved xnsl

 Revision 0.9.2.8  2006/09/24 21:57:22  brian
 - documentation and library updates

 Revision 0.9.2.7  2006/09/22 20:59:27  brian
 - prepared header file for use with doxygen, touching many lines

 Revision 0.9.2.6  2006/09/18 13:52:45  brian
 - added doxygen markers to sources

 Revision 0.9.2.5  2006/02/23 12:00:10  brian
 - corrections for 64bit and 32/64bit compatibility
 - updated headers

 Revision 0.9.2.4  2005/11/04 07:36:33  brian
 - all test cases pass on Linux Fast-STREAMS

 Revision 0.9.2.3  2005/05/14 08:28:29  brian
 - copyright header correction

 Revision 0.9.2.2  2004/05/16 04:12:33  brian
 - Updating strxnet release.

 Revision 1.4  2004/05/14 08:00:02  brian
 - Updated xns, tli, inet, xnet and documentation.

 *****************************************************************************/

#ifndef _SYS_TIMOD_H
#define _SYS_TIMOD_H

#ident "@(#) $RCSfile: timod.h,v $ $Name:  $($Revision: 0.9.2.9 $) Copyright (c) 2001-2006 OpenSS7 Corporation."

/* This file can be processed with doxygen(1). */

/** @addtogroup timod
  * @{ */

/** @file
  * Transport Interface Module (timod) header file.  */

/*
 * Transport Interface "timod" Header File.
 */

/** @name Old (TLI) transport interface input-output controls.
  * @{ */
#define TIMOD			('T'<<8)
#define O_TI_GETINFO		(TIMOD|100)	/**< Get info (1 under OSF). */
#define O_TI_OPTMGMT		(TIMOD|101)	/**< Manage options (2 under OSF). */
#define O_TI_BIND		(TIMOD|102)	/**< Bind TP (3 under OSF). */
#define O_TI_UNBIND		(TIMOD|103)	/**< Unbind TP (4 under OSF). */
/** @} */

/** @name Old ti_flags Flags.
  * @{ */
#define O_TI_USED		0x01	/**< Data structure in use. */
#define O_TI_FATAL		0x02	/**< Fatal M_ERROR_occured. */
#define O_TI_WAITIOCACK		0x04	/**< Waiting for info for ioctl act. */
#define O_TI_MORE		0x08	/**< More data. */
/** @} */

/** Old (TLI) transport interface user structure.
  */
struct _o_ti_user {
	ushort ti_flags;		/**< Flags. */
	int ti_rcvsize;			/**< Receive buffer size. */
	char *ti_rcvbuf;		/**< Receive buffer. */
	int ti_ctlsize;			/**< Control buffer size. */
	char *ti_ctlbuf;		/**< Control buffer. */
	char *ti_lookdbuf;		/**< Look data buffer. */
	char *ti_lookcbuf;		/**< Look ctrl buffer. */
	int ti_lookdsize;		/**< Look data buffer size. */
	int ti_lookcsize;		/**< Look ctrl buffer size. */
	int ti_maxpsz;			/**< TIDU size. */
#ifdef __LP64__
	u_int32_t ti_servtype;		/**< Service type. */
#else					/* __LP64__ */
	long ti_servtype;		/**< Service type. */
#endif					/* __LP64__ */
	int ti_lookflg;			/**< Buffered look flag. */
};

/**
  * @name Old timod Input/Output Controls
  * OSF specific compatibility constants.
  * These clash with termios ioctls if 'T' is used.
  * OSF uses 't' instead.
  * @{
  */
#define _O_TI_GETINFO		(TIMOD|1)	/**< (OSF) Get information from TP. */
#define _O_TI_OPTMGMT		(TIMOD|2)	/**< (OSF) Manage options for TP. */
#define _O_TI_BIND		(TIMOD|3)	/**< (OSF) Bind a Transport Provider. */
#define _O_TI_UNBIND		(TIMOD|4)	/**< (OSF) Unbind a Transport Provider. */
#define _O_TI_GETMYNAME		(TIMOD|5)	/**< (OSF) Get local addresses. */
#define _O_TI_GETPEERNAME	(TIMOD|6)	/**< (OSF) Get remote addresses. */
#define _O_TI_XTI_HELLO		(TIMOD|7)	/**< (OSF) Accept a Transport Connection. */
#define _O_TI_XTI_GET_STATE	(TIMOD|8)	/**< (OSF) Form a Transport Connection. */
#define _O_TI_XTI_CLEAR_EVENT	(TIMOD|9)	/**< (OSF) Synchronize user/kernel data. */
#define _O_TI_XTI_MODE		(TIMOD|10)	/**< (OSF) Get addresses from TP. */
#define _O_TI_TLI_MODE		(TIMOD|11)	/**< (OSF) Get TP capabilities. */
/** @} */

typedef struct xti_state {
	unsigned int xtis_qlen;		/* Saved qlen parameter from t_bind */
} XTIS, *XTISP;

/** 
  * @name timod Input/Output Controls
  * Some of these clash with BSD termios ioctls if 'T' is used.
  * Linux wrongly uses 'T' instead of 't' for BSD ioctls.
  * @{
  */
#define TI_GETINFO		(TIMOD|140)	/**< Get information from TP. */
#define TI_OPTMGMT		(TIMOD|141)	/**< Manage options for TP. */
#define TI_BIND			(TIMOD|142)	/**< Bind a Transport Provider. */
#define TI_UNBIND		(TIMOD|143)	/**< Unbind a Transport Provider. */
#define TI_GETMYNAME		(TIMOD|144)	/**< Get local addresses. */
#define TI_GETPEERNAME		(TIMOD|145)	/**< Get remote addresses. */
#define TI_SETMYNAME		(TIMOD|146)	/**< Accept a Transport Connection. */
#define TI_SETPEERNAME		(TIMOD|147)	/**< Form a Transport Connection. */
#define TI_SYNC			(TIMOD|148)	/**< Synchronize user/kernel data. */
#define TI_GETADDRS		(TIMOD|149)	/**< Get addresses from TP. */
#define TI_CAPABILITY		(TIMOD|150)	/**< Get TP capabilities. */
/** @} */

/** Structure for use with TI_SYNC request.
  */
struct ti_sync_req {
	u_int32_t tsr_flags;
};

/** @name tsr_flags Flags
  * Flags for use with #ti_sync_req#tsr_flags.
  * @{ */
#define TSRF_INFO_REQ		01		/**< Request information. */
#define TSRF_IS_EXP_IN_RCVBUF	02		/**< Is Exp data in Receive buffer? */
#define TSRF_QLEN_REQ		04		/**< Request queue length. */
/** @} */

/** Structure for use with TI_SYNC response.
  */
struct ti_sync_ack {
	t_scalar_t PRIM_type;
	t_scalar_t TSDU_size;
	t_scalar_t ETSDU_size;
	t_scalar_t CDATA_size;
	t_scalar_t DDATA_size;
	t_scalar_t ADDR_size;
	t_scalar_t OPT_size;
	t_scalar_t TIDU_size;
	t_scalar_t SERV_type;
	t_scalar_t CURRENT_state;
	t_scalar_t PROVIDER_flag;

	t_uscalar_t tsa_qlen;
	u_int32_t tsa_flags;
};

/** @name tsa_flags Flags
  * Flags for use with #ti_sync_ack#tsa_flags.
  * @{ */
#define TSA_EXP_QUEUED		01		/**< Expedited data is queued. */
/** @} */

#endif				/* _SYS_TIMOD_H */

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
