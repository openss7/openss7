/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : cfg.h
 * Description                  : GNCFD header
 *
 *
 **********************************************************************/

#ifndef __CFD_H__
#define __CFD_H__

#ifdef __STANDALONE__            /* List of required header files */
#include <sys/types.h>
#include <sys/stream.h>
#include "gndrv.h"
#endif


/*
 * Define the Protocol Module state flags
 */
#define PM_STRUCTINIT            0x0001
#define PM_PMSTART               0x0002

/*
 * Define the Generic GN_BOARD state flags
 */
#define BD_START                 0x0001


#define GNCFG_ID                 31    /* ID for STREAMS */
#define CD_MAXDEV                256   /* Max # opens on the Config Driver */
#define CD_GNCFG                 255   /* Control device number */

/*
 * Defines used by function: cf_clearpm
 */
#define CF_NOSHUT                0x01
#define CF_DOSHUT                0x02
#define CF_NOCLEAR               0x04


/*
 * Macro to round a given number up to the next multiple of four.
 */
#define QUAD_ALIGN(n)            if (((n) % 4) != 0) { \
                                    (n) += (4 - ((n) % 4)); \
                                 } 


typedef struct cf_dev {
   int      id;
   queue_t  *rq;
} CF_DEV;


/*
 * GN_HEADER: This structure contains info on the locations and sizes
 * of the generic and device-dependent board and logdev structures
 * used in the Generic Driver. There is one copy of this structure for
 * every PM in the system.
 */
typedef struct gn_header GN_HEADER;

struct gn_header {
   char           gh_pmname[16];    /* PM name string */
   GN_BOARD       *gh_gbbasep;      /* Start addr of generic BOARD area */
   GN_BOARD       *gh_gbendp;       /* Ending addr of generic BOARD area */
   GN_BOARD       *gh_gbfreep;      /* 1st free block in generic BOARD area */
   char           *gh_dbbasep;      /* Start addr of dev-dep BOARD area */
   char           *gh_dbendp;       /* Ending addr of dev-dep BOARD area */
   char           *gh_dbfreep;      /* 1st free block in dev-dep BOARD area */
   GN_LOGDEV      *gh_glbasep;      /* Start addr of generic LOGDEV area */
   GN_LOGDEV      *gh_glendp;       /* Ending addr of generic LOGDEV area */
   GN_LOGDEV      *gh_glfreep;      /* 1st free block in generic LOGDEV area */
   char           *gh_dlbasep;      /* Start addr of dev-dep LOGDEV area */
   char           *gh_dlendp;       /* Ending addr of dev-dep LOGDEV area */
   char           *gh_dlfreep;      /* 1st free block in dev-dep LOGDEV area */
/*BIT64*/
#ifdef LFS
   unsigned long  gh_bd_devdepsz;   /* BOARD dev-dep. area size (quad-align) */
   unsigned long  gh_ld_devdepsz;   /* LOGDEV dev-dep. area size (quad-align)*/
#else
   unsigned int  gh_bd_devdepsz;   /* BOARD dev-dep. area size (quad-align) */
   unsigned int  gh_ld_devdepsz;   /* LOGDEV dev-dep. area size (quad-align)*/
#endif
   unsigned int  gh_ncurbds;       /* Current # of initialized BOARDs */
   unsigned int  gh_ncurlds;       /* Current # of initialized LOGDEVs */
   unsigned int  gh_maxbds;        /* Max # BOARD structs allocated */
   unsigned int  gh_maxlds;        /* Max # LOGDEV structs allocated */
   unsigned int  gh_flags;         /* Status flags for this PM */
   unsigned short gh_nprotos;       /* Number of protocols on this PM */
   unsigned short gh_pad0;          /* For structure alignment */
#ifdef VME_LIVE_INSERTION
   GN_HEADER      *gh_nghp;         /* Ptr to next GN_HEADER on global list */
#endif
/*BIT64*/
   unsigned int  gh_rfu[4];        /* Reserved for future use */
};


#endif

