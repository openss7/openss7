/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : gndrv.h
 * Description                  : generic driver definitions
 *
 *
 **********************************************************************/

#ifndef __GNDRV_H__
#define __GNDRV_H__

#ifdef __STANDALONE__            /* List of required header files */
#include <sys/types.h>
#include <sys/stream.h>
#include "gndefs.h"
#endif


/*
 * Define the Generic GN_LOGDEV state flags
 */
#define LD_START                 0x0001
#define LD_OPEN                  0x0002
#define LD_QUEUED                0x0004 /* FW event device queued for DPC */
#define LD_TIMEOUT               0x0008
#define LD_SOFT_INT              0x0010 /* device queued for software int */
#ifdef VME_LIVE_INSERTION_B
#define LD_COMTST_SENT		 0x0020 /* NTH_COMTST command sent to fw */
#define LD_OOSVC		 0x0040 /* board out of service */
#endif /* VME_LIVE_INSERTION */

typedef struct gn_logdev         GN_LOGDEV;
typedef struct gn_board          GN_BOARD;

#ifdef sun
#define MAXEVENTS		10
#define D_EVENT_DEVICE		"/dev/dlgn_eventq"

#define D_EVENT_MINOR		0
#define APPLICATION_MINOR	(D_EVENT_MINOR + 1)

#define HOST_MESSAGE_SENT	('h' << 8 | 0)
#define HOST_MESSAGE_NOT_SENT	('h' << 8 | 1)

#define D_EVENT_STRING	"Intel(R) Dialogic(R) software halted.  Please restart the /usr/dialogic/daemon/d_event daemon immediately.\n"
#endif /* sun */


/*
 * Structure for logical device's list of outstanding commands
 * (Non-blocking commands only)
 */
typedef struct ld_cmd {
   mblk_t         *lc_mp;        /* Ptr to message containing cmd */
   unsigned short lc_pendcnt;    /* For timeouts */
   unsigned short lc_maxcnt;     /* For timeouts */
#ifdef sun
   struct ld_cmd  *next;
   struct ld_cmd  *prev;
   unsigned int   flag;		/* flag indicating message status */
#endif /* sun */
} LD_CMD;


/*
 * Structure for driver's current blocking command
 */
typedef struct ld_blockcmd {
   unsigned int  lb_blockflags;    /* Status flags for curr blocking cmd */
   mblk_t         *lb_blockmp;      /* Ptr to mp for current blocking cmd */
} LD_BLOCKCMD;

/*
 * Generic logical device structure. There is one such structure for
 * each logical device on a given BOARD.
 */
struct gn_logdev {
   unsigned char  ld_name[LD_NAMELEN]; /* Logical device name (ASCIIZ) */
   unsigned int  ld_flags;            /* Logical device status flags */
   unsigned short ld_devnum;           /* Device number on this BOARD */
   unsigned short ld_lostevents;       /* # of lost events on this dev */
   unsigned short ld_genstate;         /* Holds Generic State */
   unsigned short ld_idlestate;        /* Holds Generic idle state */
   unsigned int  ld_cmdcnt;           /* Total # of outstanding commands */
#ifdef sun 
   LD_CMD         *ld_cmds;            /* List of outstanding and commands */
#else
   LD_CMD         ld_cmds[GN_MAXCURCMDS]; /* List of outstanding and pending commands */
#endif /* sun  */
   LD_BLOCKCMD    ld_blockcmd;         /* Data for current blocking cmd */
   GN_BOARD       *ld_gbp;             /* ptr to GN_BOARD for this dev */
   GN_LOGDEV      *ld_nldp;            /* ptr to next GN_LOGDEV for this BOARD */
   char           *ld_devdepp;         /* ptr to device-dependent ldp info */
   unsigned int  ld_devdepsz;         /* Size of device-dependent area */
#ifdef NOSEARCH
   void           *ld_smhp;            /* Ptr to the SM_HANDLEs for this ldp */
   unsigned short ld_bindcnt;          /* Number of binds on this ldp */
   unsigned short ld_rfu1;             /* Reserved for future use */
   unsigned int  ld_rfu[4];           /* Reserved for future use */
#endif
#ifdef sun
   GN_LOGDEV      *ld_next_devent;     /* next scheduled firmware event ldp */
   GN_LOGDEV      *ld_prev_devent;     /* prev scheduled firmware event ldp */
   GN_LOGDEV      *ld_next_tevent;     /* next scheduled firmware timeout ldp */
   GN_LOGDEV      *ld_prev_tevent;     /* prev scheduled firmware timeout ldp */
   unsigned int  lbolt;               /* LBOLT when device is scheduled */
   unsigned int  lbolt_delta[3];      /* 0, 1, or greater delta */
#else
   unsigned long  ld_rfu[6];           /* Reserved for future use */
#endif /* sun */
};

#ifdef sun

typedef LD_CMD	BD_CMD;
typedef LD_CMD	HOST_CMDS;

typedef struct bdeventlist {
   int            bdeventcnt;
   GN_LOGDEV      *ldp_head;
   GN_LOGDEV      *ldp_tail;
   unsigned long  max_bdeventcnt;
   unsigned long  max_morebdeventcnt;
} DEVICE_EVTLIST;

queue_t *DLGN_EVENTQ;	/* Global read queue for scheduling background task */

#endif /* sun */

/*
 * Macros for easy access to blocking command info.
 */
#define ld_blockflags            ld_blockcmd.lb_blockflags
#define ld_blockmp               ld_blockcmd.lb_blockmp



/*
 * Generic board device structure. There is one such structure for each
 * BOARD installed on the system.
 */
struct gn_board {
   unsigned char  bd_name[GB_NAMELEN]; /* Name field from xxxcfg file */
   unsigned char  bd_sub[GB_SUBLEN];   /* Subdev field from xxxcfg file */
   unsigned short bd_flags;            /* Generic flags for this BOARD */
   unsigned short bd_boardid;          /* BOARD id number */
   unsigned short bd_nlogdevs;         /* # logical devices in this BOARD */
   unsigned short bd_pmid;             /* Driver ID this BOARD's PM */
   unsigned short bd_nprotos;          /* # protocols on this BOARD's PM */
   unsigned short bd_irq;              /* IRQ level used by this BOARD */
   GN_BOARD       *bd_nxtirqp;         /* Link ptr for IRQ linked list */
   GN_LOGDEV      *bd_ldp;             /* Ptr to 1st GN_LOGDEV for this BOARD*/
#ifdef sun
/* BIT64 */
   unsigned int  bd_cmdcnt;           /* Pending commands count */
   BD_CMD         *bd_cmds;            /* Pending commands array */
                          /* Similar to the device outstanding commands
                          ** which are saved in the generic device structure
                          ** LDP, pending commands stay in this structure
                          ** instead of the device dependent structure.
                          */
#endif /* sun */
#ifdef VME_LIVE_INSERTION
   GN_BOARD       *bd_nbdp;            /* Ptr to next GN_BOARD on global list*/
#endif
   char           *bd_devdepp;         /* Ptr to device-dependent BOARD info */
/* BIT64 */
   unsigned int  bd_devdepsz;         /* Size of device-dependent area */
   unsigned int  bd_rfu[4];           /* Reserved for future use */
};

#define CLEAR_STATS_TO_ZERO     1
#ifdef _STATS_
/* The following structure defines the driver statistics information */


typedef struct _SRAM_DRV_STATISTICS {
        unsigned int Flags;
        unsigned int InterruptCounter;
        unsigned int DriverCommandsCounter;
        unsigned int FirmwareCommandsCounter;
        unsigned int TotalBytesReadCounter;
        unsigned int TotalBytesWrittenCounter;
        unsigned int TotalBytesTransferredCounter;
        unsigned int BusyWaitCounter;
        unsigned int LostMessagesToFW;
        unsigned int LostMessagesFromFW;
        unsigned int NumberOfReplies;
        unsigned int NumberOfUnSolEvents;
        unsigned int NumberOfFWErrors;
        unsigned int NumberOfDriverFailedMessages;
        unsigned int PeakTicks;
} SRAM_DRV_STATISTICS ;

#define SR_STARTSTATS 1
#define SR_GETSTATS   2
#define SR_CLEARSTATS 3
#define SR_STOPSTATS  4

typedef struct pm_stats {
   int cmd;
   SRAM_DRV_STATISTICS stat;
   unsigned char pmname[8];
} PM_STATS;
#endif

#endif

