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
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 *
 *    You can reach us by email to any of
 *    gram@aztec.co.za, nemo@ordago.uc3m.es

 */

#ifndef _STR_MSG_H
#define _STR_MSG_H 1

#ident "@(#) LiS strmsg.h 2.3 3/6/00 16:37:42 "

/*  *******************************************************************  */
/*                               Dependencies                            */

#ifndef _LIS_CONFIG_H
#include <sys/strconfig.h>	/* streams config symbols are here */
#endif
#ifndef _MSG_H
#include <sys/LiS/msg.h>	/* streams msg symbols & types */
#endif
#ifndef _MSGUTL_H
#include <sys/LiS/msgutl.h>	/* streams msg utilities  */
#endif
#ifndef _QUEUE_H
#include <sys/LiS/queue.h>	/* streams queue symbols & types */
#endif
#ifndef _MOD_H
#include <sys/LiS/mod.h>	/* streams module symbols & types */
#endif

/*  *******************************************************************  */
/*                                 Symbols                               */

/*  -------------------------------------------------------------------  */
/* STREAMS normal priority message types
 */

#define QNORM		0

#define M_DATA		0
#define M_PROTO		1
#define M_BREAK		2
#define M_CTL		3
#define M_DELAY		4
#define M_IOCTL		5
#define M_PASSFP	6
#define M_RSE		7
#define M_SETOPTS	8
#define M_SIG		9

/*  -------------------------------------------------------------------  */
/* STREAMS high priority message types
 */

#define QPCTL		10

#define M_COPYIN	(QPCTL+0)
#define M_COPYOUT	(QPCTL+1)
#define M_ERROR		(QPCTL+2)
#define M_FLUSH		(QPCTL+3)
#define M_HANGUP	(QPCTL+4)
#define M_IOCACK	(QPCTL+5)
#define M_IOCNAK	(QPCTL+6)
#define M_IOCDATA	(QPCTL+7)
#define M_PCPROTO	(QPCTL+8)
#define M_PCRSE		(QPCTL+9)
#define M_PCSIG		(QPCTL+10)
#define M_READ		(QPCTL+11)
#define M_STOP		(QPCTL+12)
#define M_START		(QPCTL+13)
#define M_STARTI	(QPCTL+14)
#define M_STOPI		(QPCTL+15)


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
#define STRCANON        0x01            /* b_cont data block contains */
                                        /* canonical format specifier */
#define RECOPY          0x02            /* perform I_STR copyin again, */
                                        /* this time using canonical */
                                        /* format specifier */

/*  *******************************************************************  */

/* flags for  stroptions (M_SETOPTS)  message 
 */
#define SO_ALL          0xffff  /* set all old options */
#define SO_READOPT      0x0001  /* set read option */
#define SO_WROFF        0x0002  /* set write offset */
#define SO_MINPSZ       0x0004  /* set min packet size */
#define SO_MAXPSZ       0x0008  /* set max packet size */
#define SO_HIWAT        0x0010  /* set high water mark */
#define SO_LOWAT        0x0020  /* set low water mark */
#define SO_MREADON      0x0040  /* set read notification ON */
#define SO_MREADOFF     0x0080  /* set read notification OFF */
#define SO_NDELON       0x0100  /* old TTY semantics for NDELAY reads/writes */
#define SO_NDELOFF      0x0200  /* STREAMS semantics for NDELAY reads/writes */
#define SO_ISTTY        0x0400  /* the stream is acting as a terminal */
#define SO_ISNTTY       0x0800  /* the stream is not acting as a terminal */
#define SO_TOSTOP       0x1000  /* stop on background writes to this stream */
#define SO_TONSTOP      0x2000  /* do not stop on background writes to stream*/
#define SO_BAND         0x4000  /* water marks affect band */
#define SO_DELIM        0x8000  /* messages are delimited */
#define SO_NODELIM      0x010000 /* turn off delimiters */
#define SO_STRHOLD      0x020000 /* enable strwrite message coalescing */


/*  *******************************************************************  */
/*                                  Types                                */

typedef unsigned long str_ulong;
typedef unsigned short str_ushort;

/*  *******************************************************************  */


/* M_IOCTL message type.
 */

typedef
struct iocblk {
  SHARE
        int     ioc_cmd;        /* ioctl command type */
	cred_t *ioc_cr;		/* credentials */
        uint	ioc_id;		/* ioctl id */
        uint	ioc_count;	/* count of bytes in data field */
        int     ioc_error;	/* error code */
        int     ioc_rval;	/* return value  */
	long	ioc_filler[4];	/* SVR4 compatibility */
} iocblk_t;

#define	ioc_uid	ioc_cr->cr_uid
#define ioc_gid	ioc_cr->cr_gid

/*  *******************************************************************  */

/*  Sent to mux drivers to indicate a link.
 */
typedef
struct linkblk {
  EXPORT
        struct queue *l_qtop;   /* lowest level write queue of upper stream */
                                /* (set to NULL for persistent links) */
        struct queue *l_qbot;   /* highest level write queue of lower stream */
        int     l_index;        /* index for lower stream. */
	long	l_pad[5];	/* SVR4 compatibility */
} linkblk_t;

/*  *******************************************************************  */

/*  M_COPYIN and M_COPYOUT message types.
 */

typedef 
struct copyreq {
  SHARE
        int     cq_cmd;                 /* ioctl command (from ioc_cmd) */
	cred_t *cq_cr;			/* credentials */
        uint    cq_id;                  /* ioctl id (from ioc_id) */
        caddr_t cq_addr;                /* address to copy data to/from */
        uint    cq_size;                /* number of bytes to copy */
        int     cq_flag;                /* see above */
        mblk_t *cq_private;             /* private state information */
	long	cq_filler[4];		/* SVR4 compatibility */
} copyreq_t;

#define	cq_uid	cq_cr->cr_uid
#define cq_gid	cq_cr->cr_gid

/*  *******************************************************************  */

/* M_IOCDATA message type.
 */
typedef
struct copyresp {
  SHARE
        int     cp_cmd;                 /* ioctl command (from ioc_cmd) */
	cred_t *cp_cr;			/* credentials */
        uint    cp_id;                  /* ioctl id (from ioc_id) */
        caddr_t cp_rval;                /* status of request: 0 -> success */
                                        /*              non-zero -> failure */
	uint	cp_pad1;		/* reserved */
	int	cp_pad2;		/* reserved */
        mblk_t *cp_private;             /* private state information */
	long	cp_filler[4];		/* SVR4 compatibility */
} copyresp_t;

#define	cp_uid	cp_cr->cr_uid
#define cp_gid	cp_cr->cr_gid



/*  *******************************************************************  */

/* M_SETOPTS message.  This is sent upstream
 * by a module or driver to set stream head options.
 */
typedef
struct stroptions {
  SHARE
        str_ulong   so_flags;               /* options to set */
        short   so_readopt;             /* read option */
        str_ushort  so_wroff;               /* write offset */
        long    so_minpsz;              /* minimum read packet size */
        long    so_maxpsz;              /* maximum read packet size */
        str_ulong   so_hiwat;               /* read queue high water mark */
        str_ulong   so_lowat;               /* read queue low water mark */
        unsigned char so_band;          /* band for water marks */
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
#endif /*!_STR_MSG_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/

