/*****************************************************************************

 @(#) $Id: $

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

 Last Modified $Date: $ by $Author: $

 *****************************************************************************/

/*                               -*- Mode: C -*- 
 * <strmsg.h> --- Linux STREAMS message types
 * Author          : gram & nemo
 * Created On      : Mon Oct 17 11:37:12 1994
 * RCS Id          ; $Id: strmsg.h,v 1.2 1996/01/27 00:40:02 dave Exp $
 * Last Modified By: David Grothe
 * Purpose         : Provide Streams message types
 *                 :
 *
 *    Copyright (C) 1995  Graham Wheeler, Francisco J. Ballesteros
 *    Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach us by email to any of
 *    gram@aztec.co.za, nemo@ordago.uc3m.es

 */

#ifndef _STR_MSG_H
#define _STR_MSG_H 1

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

/*  *******************************************************************  */
/*                               Dependencies                            */

#ifndef _LIS_CONFIG_H
#include <sys/strconfig.h>	/* streams config symbols are here */
#endif
#ifndef _MSG_H
#include <sys/LiS/msg.h>	/* streams msg symbols & types */
#endif
#ifndef _MSGUTL_H
#include <sys/LiS/msgutl.h>	/* streams msg utilities */
#endif
#ifndef _QUEUE_H
#include <sys/LiS/queue.h>	/* streams queue symbols & types */
#endif
#ifndef _MOD_H
#include <sys/LiS/mod.h>	/* streams module symbols & types */
#endif

/************************************************************************
*                          STREAMS Message Types                        *
*************************************************************************
*									*
* Contributed by Brian Bidulock who researched the numberings from 	*
* various STREAMS environments.						*
*									*
************************************************************************/

/* 
 *  Message type compatibility:
 *      S - Solaris
 *      U - UnixWare
 *      A - AIX
 *      O - OSF/1.2
 *      H - HP-UX
 *      M - Mac OT (AIX)
 *      L - LiS
 *  Message direction:
 *      v - downwards only
 *      ^ - upwards only
 *      | - both directions
 *      - - not a message
 *      ? - unknown
 */
/* The OSF numbering mismatch of QNORM messages looks like a typo in the
 * stream.h header files.  If the OSF number were interpreted in octal
 * instead of hex, the numbering would be identical to the others! Note
 * that the MAC OT (AIX) did not make this error.
 */
typedef enum msg_type {
	QNORM = 0x00,			/* - S U O A H M L */
	M_DATA = 0x00,		/* | S U O A H M L */
	M_PROTO = 0x01,		/* | S U O A H M L */
	M_BREAK = 0x08,		/* v S U O(0x10) A H M L(0x02) */
	M_PASSFP = 0x09,	/* | S U O(0x11) A H M L(0x06) */
	M_EVENT = 0x0a,		/* ? S */
	M_SIG = 0x0b,		/* ^ S U O(0x13) A H M L(0x09) */
	M_DELAY = 0x0c,		/* v S U O(0x14) A H M L(0x04) */
	M_CTL = 0x0d,		/* | S U O(0x15) A H M L(0x03) */
	M_IOCTL = 0x0e,		/* v S U O(0x16) A H M L(0x05) */
	M_SETOPTS = 0x10,	/* ^ S U O(0x20) A H M L(0x08) */
	M_RSE = 0x11,		/* | S U O(0x21) A H M L(0x07) */
	M_TRAIL = 0x12,		/* ? U */
	M_BACKWASH = 0x13,	/* v A */
	QPCTL = 0x80,		/* - S U O A H M L(0x0a) */
	M_IOCACK = 0x81,	/* ^ S U O A H M L(0x0f) */
	M_IOCNAK = 0x82,	/* ^ S U O A H M L(0x10) */
	M_PCPROTO = 0x83,	/* | S U O A H M L(0x12) */
	M_PCSIG = 0x84,		/* ^ S U O A H M L(0x14) */
	M_READ = 0x85,		/* v S U O(0x8b) A H(0x8b) M(0x8b) L(0x15) */
	M_FLUSH = 0x86,		/* | S U O A H M L(0x0d) */
	M_STOP = 0x87,		/* v S U O A H M L(0x16) */
	M_START = 0x88,		/* v S U O A H M L(0x17) */
	M_HANGUP = 0x89,	/* ^ S U O A H M L(0x0e) */
	M_ERROR = 0x8a,		/* ^ S U O A H M L(0x0c) */
	M_COPYIN = 0x8b,	/* ^ S U O(0x8d) A H(0x8d) M(0x8c) L(0x0a) */
	M_COPYOUT = 0x8c,	/* ^ S U O(0x8e) A H(0x8e) M(0x8d) L(0x0b) */
	M_IOCDATA = 0x8d,	/* v S U O(0x8f) A H(0x8f) M(0x8e) L(0x11) */
	M_PCRSE = 0x8e,		/* | S U O(0x90) A H(0x90) M(0x90) L(0x13) */
	M_STOPI = 0x8f,		/* v S U O(0x91) A H(0x91) M(0x91) L(0x19) */
	M_STARTI = 0x90,	/* v S U O(0x92) A H(0x92) M(0x92) L(0x18) */

	/* the rest of these are all over the board, only M_UNHANGUP is common, they have been
	   renumbered so that at least they don't overlap */

	M_PCCTL = 0x91,		/* | U */
	M_PCSETOPTS = 0x92,	/* ^ U */
	M_PCEVENT = 0x93,	/* ? S(0x91) */
	M_UNHANGUP = 0x94,	/* ^ S(0x92) O */
	M_NOTIFY = 0x95,	/* ^ O(0x93) H(0x93) */
	M_HPDATA = 0x96,	/* ^ H(0x8c) M(0x93) */
	M_LETSPLAY = 0x97,	/* ^ A */
	M_DONTPLAY = 0x98,	/* v A */
	M_BACKDONE = 0x99,	/* v A */

} msg_type_t;

/*  -------------------------------------------------------------------  */
/*
 * Flag for transparent ioctls (actually every ioctl will be transparent:)
 */
#define TRANSPARENT     (unsigned int)(-1)

/*  *******************************************************************  */

/* Max ioctl block size
 */
#define MAXIOCBSZ       1024

/*  *******************************************************************  */
/* cpyblk_t (copyrec,M_COPYIN,M_COPYOUT) flags
 */
#define STRCANON        0x01	/* b_cont data block contains */
					/* canonical format specifier */
#define RECOPY          0x02	/* perform I_STR copyin again, */
					/* this time using canonical */
					/* format specifier */

/*  *******************************************************************  */

/* flags for  stroptions (M_SETOPTS)  message 
 */
#define SO_ALL          0xffff	/* set all old options */
#define SO_READOPT      0x0001	/* set read option */
#define SO_WROFF        0x0002	/* set write offset */
#define SO_MINPSZ       0x0004	/* set min packet size */
#define SO_MAXPSZ       0x0008	/* set max packet size */
#define SO_HIWAT        0x0010	/* set high water mark */
#define SO_LOWAT        0x0020	/* set low water mark */
#define SO_MREADON      0x0040	/* set read notification ON */
#define SO_MREADOFF     0x0080	/* set read notification OFF */
#define SO_NDELON       0x0100	/* old TTY semantics for NDELAY reads/writes */
#define SO_NDELOFF      0x0200	/* STREAMS semantics for NDELAY reads/writes */
#define SO_ISTTY        0x0400	/* the stream is acting as a terminal */
#define SO_ISNTTY       0x0800	/* the stream is not acting as a terminal */
#define SO_TOSTOP       0x1000	/* stop on background writes to this stream */
#define SO_TONSTOP      0x2000	/* do not stop on background writes to stream */
#define SO_BAND         0x4000	/* water marks affect band */
#define SO_DELIM        0x8000	/* messages are delimited */
#define SO_NODELIM      0x010000	/* turn off delimiters */
#define SO_STRHOLD      0x020000	/* enable strwrite message coalescing */

/*  *******************************************************************  */
/*                                  Types                                */

typedef unsigned long str_ulong;
typedef unsigned short str_ushort;

/*  *******************************************************************  */

/* M_IOCTL message type.
 */

typedef struct iocblk {
	SHARE int ioc_cmd;		/* ioctl command type */
	cred_t *ioc_cr;			/* credentials */
	uint ioc_id;			/* ioctl id */
	uint ioc_count;			/* count of bytes in data field */
	int ioc_error;			/* error code */
	int ioc_rval;			/* return value */
	long ioc_filler[4];		/* SVR4 compatibility */
} iocblk_t;

#define	ioc_uid	ioc_cr->cr_uid
#define ioc_gid	ioc_cr->cr_gid

/*  *******************************************************************  */

/*  Sent to mux drivers to indicate a link.
 */
typedef struct linkblk {
	EXPORT struct queue *l_qtop;	/* lowest level write queue of upper stream */
	/* (set to NULL for persistent links) */
	struct queue *l_qbot;		/* highest level write queue of lower stream */
	int l_index;			/* index for lower stream. */
	long l_pad[5];			/* SVR4 compatibility */
} linkblk_t;

/*  *******************************************************************  */

/*  M_COPYIN and M_COPYOUT message types.
 */

typedef struct copyreq {
	SHARE int cq_cmd;		/* ioctl command (from ioc_cmd) */
	cred_t *cq_cr;			/* credentials */
	uint cq_id;			/* ioctl id (from ioc_id) */
	caddr_t cq_addr;		/* address to copy data to/from */
	uint cq_size;			/* number of bytes to copy */
	int cq_flag;			/* see above */
	mblk_t *cq_private;		/* private state information */
	long cq_filler[4];		/* SVR4 compatibility */
} copyreq_t;

#define	cq_uid	cq_cr->cr_uid
#define cq_gid	cq_cr->cr_gid

/*  *******************************************************************  */

/* M_IOCDATA message type.
 */
typedef struct copyresp {
	SHARE int cp_cmd;		/* ioctl command (from ioc_cmd) */
	cred_t *cp_cr;			/* credentials */
	uint cp_id;			/* ioctl id (from ioc_id) */
	caddr_t cp_rval;		/* status of request: 0 -> success */
	/* non-zero -> failure */
	uint cp_pad1;			/* reserved */
	int cp_pad2;			/* reserved */
	mblk_t *cp_private;		/* private state information */
	long cp_filler[4];		/* SVR4 compatibility */
} copyresp_t;

#define	cp_uid	cp_cr->cr_uid
#define cp_gid	cp_cr->cr_gid

/*  *******************************************************************  */

/* M_SETOPTS message.  This is sent upstream
 * by a module or driver to set stream head options.
 */
typedef struct stroptions {
	SHARE str_ulong so_flags;	/* options to set */
	short so_readopt;		/* read option */
	str_ushort so_wroff;		/* write offset */
	long so_minpsz;			/* minimum read packet size */
	long so_maxpsz;			/* maximum read packet size */
	str_ulong so_hiwat;		/* read queue high water mark */
	str_ulong so_lowat;		/* read queue low water mark */
	unsigned char so_band;		/* band for water marks */
} stroptions_t;

/*  *******************************************************************  */

/* The structure for M_EVENT & M_PCEVENT is not defined. I think we are
 * not going to support such things as a general event subsytem, etc.:
 *
 * The structure for M_EVENT and M_PCEVENT messages is sent upstream
 * by a module or driver to have the stream head manage an event
 * It is also contained in the first M_DATA
 * block of an M_IOCTL message for the I_STREV and I_UNSTREV ioctls.
 *
 * struct str_evmsg { ... };
 */

/*  -------------------------------------------------------------------  */

/* Is this msg type a high priority one?
 */
#define lis_hipri(t)	((t)>=QPCTL)

/*  *******************************************************************  */
#endif				/* !_STR_MSG_H */

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
