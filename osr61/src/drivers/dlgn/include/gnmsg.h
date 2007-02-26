/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : gnmsg.h
 * Description                  : generic message structures
 *
 *
 **********************************************************************/

#ifndef __GNMSG_H__
#define __GNMSG_H__

#ifdef __STANDALONE__            /* List of required header files */
#include "gndefs.h"
#endif

/* Protocol IOCTL commands */
#define	PM_MEMSZ_CMD			('p' << 8 | 0x1)
#define	PM_DEBUG_CMD			('p' << 8 | 0x2)
#define	PM_DEQUEUE_DPC			('p' << 8 | 0x3)
#define	PM_GET_PENDING_QUEUE_BOARD	('p' << 8 | 0x4)
#define	PM_STATS_CMD			('p' << 8 | 0x5)

/*
 * Generic message types
 */
#define MT_CONTROL               0x0001
#define MT_DEVICE                0x0002
#define MT_BULK                  0x0003
#define MT_VIRTEVENT             0x0004
#define MT_EVENT                 0x0005
#define MT_REPLY                 0x0006
#define MT_ERRORREPLY            0x0007
#define MT_FW_REPLY              0x0008	/* This enables the library to
					** determine the need to do
					** swap endian.
					*/
#define MT_LIVEINSERTION         0x0009 /* rwt */

/*
 * Message idents used in msg_ident
 */
#define MC_GENBASEMSG            0x1000    /* For generic commands */
#define MC_BASEMSGNUM            0x2000    /* For PM-specific commands */

#define MC_BIND                  (MC_GENBASEMSG | 0x0001)
#define MC_BINDCMPLT             MC_BIND
#define MC_UNBIND                (MC_GENBASEMSG | 0x0002)
#define MC_UNBINDCMPLT           MC_UNBIND
#define MC_SETEVMASK             (MC_GENBASEMSG | 0x0003)
#define MC_EVMASKCMPLT           MC_SETEVMASK
#define MC_GETEVMASK             (MC_GENBASEMSG | 0x0004)
#define MC_CUREVMASK             MC_GETEVMASK
#define MC_GETSTATE              (MC_GENBASEMSG | 0x0005)
#define MC_CURSTATE              MC_GETSTATE
#define MC_SETIDLESTATE          (MC_GENBASEMSG | 0x0006)
#define MC_CURIDLESTATE          MC_SETIDLESTATE
#define MC_SETPARM               (MC_GENBASEMSG | 0x0007)
#define MC_SETPARMCMPLT          MC_SETPARM
#define MC_GETPARM               (MC_GENBASEMSG | 0x0008)
#define MC_CURPARM               MC_GETPARM
#define MC_SENDDATA              (MC_GENBASEMSG | 0x0009)
#define MC_RECEIVEDATA           (MC_GENBASEMSG | 0x000A)
#define MC_MOREDATA              (MC_GENBASEMSG | 0x000B)
#define MC_LASTDATA              (MC_GENBASEMSG | 0x000C)
#define MC_BUFFMT                (MC_GENBASEMSG | 0x000D)
#define MC_BUFFUL                (MC_GENBASEMSG | 0x000E)
#define MC_FLUSH                 (MC_GENBASEMSG | 0x000F)
#define MC_SENDEVT               (MC_GENBASEMSG | 0x0010)
#define MC_SENDEVTCMPLT          MC_SENDEVT
#define MC_IPCEVENT              (MC_GENBASEMSG | 0x0011)
#define MC_BOARDREMOVED          (MC_GENBASEMSG | 0x0012)
#ifdef VME_LIVE_INSERTION
#define MC_BOARDBAD              (MC_GENBASEMSG | 0x0013) /* COMTST failed */
#define MC_BOARDDEAD             (MC_GENBASEMSG | 0x0014) /* Board Reset */
#endif /* VME_LIVE_INSERTION */

/* error return codes */
#define GNE_NOHANDLE             0x0011
#define GNE_BADHANDLE            0x0012
#define GNE_BADCMD               0x0013
#define GNE_BADPARM              0x0014
#define GNE_STRAYREPLY           0x0015
#define GNE_NOLDP                0x0016
#define GNE_BADIOCTL             0x0017
#define GNE_BADSTATE             0x0018
#define GNE_MAXCURCMDS           0x0019
#define GNE_TIMEOUT              0x001A
#define GNE_BLOCKING             0x001B

#define NO_NOTIFY                0x00
#define NOTIFY                   0x01

/* Reply occurrences */
#define IOF_RECEIVED             0x0001
#define IOF_TERMINATOR           0x0002
#define IOF_INTERMEDIATE         0x0004
#define IOF_IGNORE               0x0008
#define IOF_OPTIONAL             0x0010
#define IOF_FINAL                0x0020
#define IOF_NOFWCMD              0x0040
#define IOF_NEWSTATE             0x0080

/* parameters to MC_FLUSH */
#define GN_FLUSHW                0x0001   /* flush all send data */
#define GN_FLUSHR                0x0002   /* flush all receive data */


typedef struct gn_flush {
   unsigned long gf_cmd;         /* Contains flush command */
} GN_FLUSH;


typedef union gn_devaddr {
   struct {
      unsigned long dad_devtype;    /* FW Device type identifier */
      unsigned char dad_board;      /* Board no. to send message to */
      unsigned char dad_devnum;     /* FW Device instance number */
      unsigned char dad_chanum;     /* Channel number on device */
      unsigned char dad_rfu;
   } da_dyn;
   struct {
      unsigned long das_handle;     /* Handle to use for OS entry */
      unsigned long das_index;      /* Handle index */
   } da_stat;
} GN_DEVADDR;


/* Macros for easier GN_DEVADDR union member access */
#define  da_board                da_dyn.dad_board
#define  da_devtype              da_dyn.dad_devtype
#define  da_devnum               da_dyn.dad_devnum
#define  da_chanum               da_dyn.dad_chanum
#define  da_handle               da_stat.das_handle
#define  da_index                da_stat.das_index


typedef struct gn_reply {
   unsigned short rs_actions;    /* Per-reply flags */
   unsigned short rs_results;    /* Per-reply results */
   unsigned short rs_ident;      /* Message id of reply */
   unsigned short rs_info;       /* Info for reply processing */
} GN_REPLY;


typedef struct gn_drvmsg {
   GN_DEVADDR     dm_msg_addr;      /* Device address */
   unsigned short dm_msg_type;      /* Type of the message */
   unsigned short dm_protocol;      /* Protocol ID */
   unsigned short dm_msg_ident;     /* Message identifier/meaning */
   unsigned short dm_errcode;       /* Error code */
   unsigned long  dm_commandid;     /* Command identifier/meaning */
   unsigned char  dm_rfu[16];       /* Reserved for future use */
   unsigned long  dm_datalen[1];    /* Size of data block following this
                                       structure */
} GN_DRVMSG;


typedef struct gn_cmdmsg {
   GN_DEVADDR     cm_address;       /* Target device */
   unsigned short cm_msg_type;      /* Message type identifier */
   unsigned short cm_flags;         /* Message flag bits */
   unsigned long  cm_timeout;       /* Completion timeout */
   unsigned short cm_state;         /* State to set when cmd issued */
   unsigned short cm_nreplies;      /* Number of expected replies */
   GN_REPLY       cm_reply[CMD_MAXREPLIES];  /* Expected replies */
   GN_REPLY       cm_event;         /* For unexpected reply/event */
   unsigned char  cm_rfu[16];       /* Reserved for future use */
   GN_DRVMSG      cm_fwmsg;         /* Message to start command */
} GN_CMDMSG;


/* Macros for easier GN_CMDMSG structure member access */
#define fw_msg_addr              cm_fwmsg.dm_msg_addr
#define fw_commandid             cm_fwmsg.dm_commandid
#define fw_msg_type              cm_fwmsg.dm_msg_type
#define fw_protocol              cm_fwmsg.dm_protocol
#define fw_msg_ident             cm_fwmsg.dm_msg_ident
#define fw_errcode               cm_fwmsg.dm_errcode
#define fw_rfu                   cm_fwmsg.dm_rfu
#define fw_datalen               cm_fwmsg.dm_datalen


#endif

