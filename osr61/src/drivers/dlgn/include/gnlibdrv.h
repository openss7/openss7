/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : gnlibdrv.h
 * Description                  : generic library-driver definitions
 *
 *
 **********************************************************************/

#ifndef __GNLIBDRV_H__
#define __GNLIBDRV_H__

#ifdef __STANDALONE__            /* List of required header files */
#include "gndefs.h"
#endif


typedef struct gn_event {
   unsigned short ge_event_id;         /* event id */
   unsigned short ge_disposition;      /* disposition for this event */
} GN_EVENT;


typedef struct gn_bind {
   int  gb_handle;
   int  gb_datalen;
   char gb_devname[LD_NAMELEN];
} GN_BIND;


typedef struct gn_evstruct {
   unsigned long  gv_numevents;
   GN_EVENT       gv_event[GN_MAXEVENTS];
} GN_EVSTRUCT;


typedef struct gn_stateinfo {
   unsigned short st_idlestate;
   unsigned short st_newstate;
   unsigned short st_retstate;
   unsigned short st_rfu;
} GN_STATEINFO;

/*
 * Flag defines and structure used to pass event information from a library
 * to GDD for Inter-Process Event Communication.
 */

#define EVFL_SENDSELF         0x01     /* Send event to self process */
#define EVFL_SENDOTHERS       0x02     /* Send event to other processes */
#define EVFL_SENDALL          0x03     /* Send event to all processes */

#define SND_EVTDATASZ         0x0100

typedef struct gn_sendevtinfo {
   unsigned long  snd_evttype;
   unsigned char  snd_evtdata[SND_EVTDATASZ];
   short          snd_evtlen;
   unsigned short snd_evtflags;
} GN_SENDEVTINFO;


#endif

