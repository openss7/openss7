/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : andrv.h
 * Description                  : antares driver access module
 *
 *
 **********************************************************************/

#ifndef __ANDRV_H__
#define __ANDRV_H__

#ifdef __STANDALONE__            /* List of required header files */
#include "gndrv.h"
#include "anlibdrv.h"
#include "typedefs.h"
#endif

#define  AN_DRVRETSZ (sizeof(AN_SNDCMD) - sizeof(GN_CMDMSG))
#define  AN_DRVMSGSZ (sizeof(AN_DRVCMD) - sizeof(GN_CMDMSG))
#define  AN_BDCTLSZ (sizeof(AN_BDCTL) - sizeof(GN_CMDMSG))

typedef struct an_board AN_BOARD;
typedef struct an_logdev AN_LOGDEV;

/************  Irq Level Selection  ********************************/

#define AN_INVALID_IRQ_LVL(i) \
    (    (i)!=3 && (i)!=4  && (i)!=5  && (i)!=7   \
      && (i)!=9 && (i)!=10 && (i)!=11 && (i)!=12)

/*********************** Logical Device Structures ***********************/

#define  AN_REG_TIMEOUT   (1<<1)
/*
 * Antares Logical Device information
 */
struct an_logdev {
    int       dtype;     /* Device type code:  ANDT_.. */
#ifndef _SCO_
    LONG      gnhndl;    /* Queue handle per ldp       */
#endif /* _SCO_ */
    WORD      opencnt;   /* Number of opens on this logical device */
    WORD      error;     /* ANE_.. error numbers  */
    WORD      oflags;    /* ANOF_.. open flag bits */
    WORD      state;     /* ANLS_...  logical device state bits */
    GN_LOGDEV *genldp;    /* .. to get back to GENworld */
    AN_LOGDEV *next_anld; /* next logical device on associated board */
    AN_BOARD *anbp;      /* ptr back to board */
};

/********************** Board Device Structures ***********************/

/*
 * Antares BOARD structure.
 */
struct an_board {
   WORD   btype;        /* Physical Board Type code: ANBT_....     */
   WORD   state;        /* ANBS_... flags (see below)              */
   WORD   error;        /* ANE_.. errno from anlibdrv.h            */
   GN_BOARD  *genbdp;   /* Ptr to device's GN_BOARD structure      */
   AN_LOGDEV *anld_list;  /* Ptr to list of AN_LOGDEV on this board  */
   AN_BOARD  *next_irq;   /* Ptr to next raw AN_BOARD in IRQ list    */
#if defined(_SCO_) || defined(LINUX)
   ulong      boardId;    
#else
   ULONG boardId;        
#endif /* _SCO_ */
};

typedef struct an_rcucor {
#if defined(_SCO_) || defined(LINUX)
    unsigned long       gnhndl;
#else
    LONG       gnhndl;
#endif /* _SCO_ */
    GN_LOGDEV *ldp;
    SHORT rhandle;
    PRC proc;   
    SHORT dpi_ind;
} AN_RCUCOR;

typedef struct an_bdcor {
#if defined(_SCO_) || defined(LINUX)
    unsigned long       gnhndl;
#else
    LONG       gnhndl;
#endif /* _SCO_ */
    GN_LOGDEV *ldp;
    SHORT bd_id;
    SHORT bdstream;
    SHORT bdhandle;
    PRC bdproc;   
    SHORT dpi_ind;
} AN_BDCOR;

/********************** Macros ****************************************/

#define Gbdp2Anbp(n)   ((AN_BOARD *)((n)->bd_devdepp))
#define Gldp2Anbp(n)   Gbdp2Anbp((n)->ld_gbp)
#define Gldp2Anldp(n)  ((AN_LOGDEV *)((n)->ld_devdepp))

#endif
