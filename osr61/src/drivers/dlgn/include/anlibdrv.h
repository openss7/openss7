/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : anlibdrv.h
 * Description                  : antares library definitions
 *
 *
 **********************************************************************/

#ifndef __ANLIBDRV_H__
#define __ANLIBDRV_H__

#ifdef __STANDALONE__            /* List of required header files */
#include "gnmsg.h"
#include "typedefs.h"
#endif

/****************** Antares logical Device Types *********************/

#define ANDT_UNBOUND   0   /* Unbound device */
#define ANDT_CFG       1   /* physical board */
#define ANDT_RAW       2   /* physical board */
#if defined(_SCO_) || defined(LINUX)
#define ANDT_PRAW       3   /* physical board */
#else
#define ANDT_IOC       3   /* physical board */
#endif /* _SCO_ */
#define ANDT_MSG       4  /* resource class unit Message device */
#define ANDT_BDSTR     5   /* Bulk Data STReam */

/******************* Antares DAM Open Flag Bits  ************************/
 
#define ANE_ACK 0

/**************************************************************************
 * 
 * Antares DAM-DPI Driver Command
 *
 * Encapsulates generic header and Antares DAM header
 *
 **************************************************************************/

typedef struct an_gnbind {
   GN_CMDMSG gncmdmsg; /* Generic driver header */
   GN_BIND gnbind;
} AN_GNBIND;

typedef struct an_dpicmd {
#if !(defined(_SCO_) || defined(LINUX))
    ADDR msgp[39];
#endif /* _SCO_ */
    BYTE cmd;
    WORD drvrid;
    WORD cnt;
#if defined(_SCO_) || defined(LINUX)
    ADDR msgp[50];
#endif /* _SCO_ */
} AN_DPICMD;

typedef struct an_sndcmd {
   GN_CMDMSG gncmdmsg; /* Generic driver header */
   AN_DPICMD andpicmd;
   ULONG rep;
   SHORT ret;
} AN_SNDCMD;

#if defined(_SCO_) || defined(LINUX)
typedef struct an_snd_playcmd {
   GN_CMDMSG gncmdmsg; /* Generic driver header */
   AN_DPICMD andpicmd;
   ULONG rep;
   SHORT ret;
   BD_CHANNEL anbdchan;
} AN_SND_PLAYCMD;
#else
typedef struct an_ioccmd {
   GN_CMDMSG gncmdmsg; /* Generic driver header */
   AN_DPICMD andpicmd;
} AN_IOCCMD;
#define IOCMDS sizeof(AN_IOCCMD)
#endif /* _SCO_ */

typedef struct an_drvcmd {
   GN_CMDMSG gncmdmsg; /* Generic driver header */
   RECV_MSG ioevt;
   ULONG dpi_ind;
} AN_DRVCMD;

typedef struct an_bdctl {
   GN_CMDMSG gncmdmsg; /* Generic driver header */
   BD_CHANNEL p_bd_entry;
   SHORT loc;
   BYTE opt;
   BYTE cr;
   BYTE dir;
   LONG datalen;
   SHORT dpi_ind;
} AN_BDCTL;
#if defined(_SCO_) || defined(LINUX)
#define AND_BDSEEK    (MC_BASEMSGNUM | 10) /* dtype == ANDT_BDSTR */

#define ANC_SETPARM  (MC_BASEMSGNUM | 0x10)
#define ANC_CMD      (MC_BASEMSGNUM | 0x11)
#define ANC_PLAYCMD  (MC_BASEMSGNUM | 0x12)
#else
#define ANC_SETPARM   (MC_BASEMSGNUM | 0x10)
#define AN_IOC_CMD    (MC_BASEMSGNUM | 0x11)
#define AN_SEND_CMD   (MC_BASEMSGNUM | 0x12)
#endif /* _SCO_ */
#define ANC_RC        (MC_BASEMSGNUM | 0x13)
#define ANC_MSGRECV   (MC_BASEMSGNUM | 0x14)
#define ANC_BDCTL     (MC_BASEMSGNUM | 0x15)

/**************************   Macros    *******************************/
#define NReplies(cmdp)    ((cmdp)->gn_drvhdr.cm_nreplies)
#define ReplyId(cmdp,i) ((cmdp)->gn_drvhdr.cm_reply[(i)].rs_ident)

#endif
