/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dlgn.h
 * Description                  : header for dlgn.c
 *
 *
 **********************************************************************/

#ifndef __DLGN_H__
#define __DLGN_H__

#ifdef __STANDALONE__            /* List of required header files */
#include <sys/types.h>
#include <sys/stream.h>
#include "gndrv.h"
#include "gnlibdrv.h"
#endif


#define DLGN_ID                  301   /* Driver ID */


/* SM state flags */
#define ST_SMSTART               0x01
#define ST_SMTIMEOUT             0x02

/* SM handle flags */
#define SH_BOUND                 0x01
#define SH_NOTIFY_NONE           0x02
#define SH_NOTIFY_ALL            0x04
#define SH_NOTIFY_BUT            0x08

/* command timeout processing */
#define GN_TIMESECS              10
#define GN_TIMEOUT               (HZ * GN_TIMESECS)

/* Reply occurrences */
#define RPY_FOUND                0x1
#define RPY_TERMINATOR           0x2

typedef struct sm_handle SM_HANDLE;

struct sm_handle {
   queue_t        *sm_rq;           /* read queue pointer to upstream */
   GN_LOGDEV      *sm_ldp;          /* pointer to ldp for this device */
   unsigned long  sm_flags;         /* e.g. notification for ALL events */
   unsigned long  sm_numevents;     /* number of entries in event masks */
   GN_EVENT       sm_event[GN_MAXEVENTS]; /* event masks */
#ifdef NOSEARCH
   SM_HANDLE      *sm_nxthp;        /* Pointer to next SM_HANDLE for an ldp */
   unsigned long  sm_rfu[3];
#else
   unsigned long  sm_rfu[4];
#endif
} ;


typedef struct sm_state {
   unsigned long  st_flags;
   unsigned long  st_nbound;
   unsigned long  st_count;
   long           st_peak;
   unsigned long  st_tmid;
   unsigned long  st_loopstate;
   unsigned long  st_loop;
   queue_t        *st_qptr[GN_MAXHANDLES];
   unsigned long  st_rfu[4];
} SM_STATE;


#endif


