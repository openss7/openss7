/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dxdrv.h
 * Description                  : SRAM protocol specific
 *
 *
 **********************************************************************/

#ifndef __DXDRV_H__
#define __DXDRV_H__

#ifdef __STANDALONE__            /* List of required header files */
#include "typedefs.h"
#include "gndrv.h"
#include "d40defs.h"
#include "srmap.h"
#include "dxlibdrv.h"
#endif


/*
 * PM-defined flags used in ld_flags of GN_LOGDEV
 */
#define LD_BULKQUEUED            0x0001
#define LD_PRSTATUS    		 0x0010
#define LD_BULKSEND              0x0100
#define LD_BULKRCV               0x0200
#define LD_BULKSTART             0x0400
#define LD_BULKDONE              0x0800
#define LD_PUTDONE               0x1000
#define LD_STOP                  0x2000
#define LD_SENDLASTBUFF          0x4000
#define LD_WAITMORE              0x8000


#define DX_NOSTOPCMD             0x0000
#define DX_STOPCMD               0x0001


/*
 * Firmwae Assert defines
 */
#define FW_DBG_SIZE		 64
#define FW_DBG_2K_OFFSET	 0x1E00
#define FW_DBG_4K_OFFSET	 0x3E00
#define FW_DBG_8K_OFFSET	 0x7E00


typedef struct dx_board DX_BOARD;
typedef struct sr_irqlist SR_IRQLIST;


/*
 * SR_IRQLIST: Structure containing info on boards to be checked by
 * the SRAM PM at interrupt time.
 */
struct sr_irqlist {
   SR_IRQLIST *si_nextp;         /* Ptr to next SR_IRQLIST on chain */
   GN_BOARD   *si_bdp;           /* Ptr to device's GN_BOARD structure */
   DX_BOARD   *si_dxbdp;         /* Ptr to device's DX_BOARD structure */
   D4XSRAM    *si_bdaddr;        /* Ptr to device's shared RAM area */
#ifndef VME_SPAN
   USHORT     si_ioport;         /* Device's IO port */
   USHORT     si_rfu[7];         /* Reserved for future use */
#else
   ULONG      si_ioport;         /* Device's IO port */
   ULONG      si_rfu[3];         /* Reserved for future use */
#endif /* VME_SPAN */
};


/*
 * Device-dependent FWI structure for dxxx-type devices
 */
typedef struct dx_fwi {
   GN_LOGDEV   *fwi_ldp;         /* ldp associated with this FWI */
   UCHAR       *fwi_addr;        /* Shared RAM virtual address */
   USHORT      fwi_type;         /* IO_DEVFWI if bd; IO_CHNFWI if chn */
   USHORT      fwi_num;          /* 0 if bd, chn# if chn */
   ULONG       fwi_rfu[1];
} DX_FWI;


/*
 * Device-dependent LOGDEV structure for dxxx-type devices.
 */
typedef struct dx_logdev {
   ULONG       dl_type;          /* Device type code */
   ULONG       dl_opencnt;       /* Number of opens on this device */
   mblk_t      *dl_sndq;         /* local send queue */
#ifdef VME_SPAN
   mblk_t      *dl_sndq_tmp;     /* The temporary send queue stores
                                 ** outgoing messages that arrive after
                                 ** the first one was queued waiting for
                                 ** board to be ready, i.e. PC_REQ equal 0.
                                 */
#endif /* VME_SPAN */
   mblk_t      *dl_rcvq;         /* local receive queue */
   ULONG       dl_sndcnt;        /* send que current data count */
   ULONG       dl_rcvcnt;        /* rcv que current data count */
   ULONG       dl_maxrcvsz;      /* total #bytes to receive */
   ULONG       dl_totrcvcnt;     /* total #bytes received so far */
   USHORT      dl_lowater;       /* PM low-water mark; limit is 64K max */
   USHORT      dl_hiwater;       /* PM high-water mark; limit is 64K max */
   DX_FWI      *dl_fwip;
   DX_FWI      *dl_nestfwip;
   DX_DEVSTOP  dl_stop;          /* Device's STOP command structure */
   ULONG       dl_dtiparm[DTIP_MAXPARM];
   ULONG       dl_handle;        /* Device handle obtained from GN_CMDMSG */
   ULONG       dl_rfu[4];
} DX_LOGDEV;
 

#define dl_stopcmd            dl_stop.ds_cmd
#define dl_stopflag           dl_stop.ds_flag
#define dl_stopfwi            dl_stop.ds_fwi

#define dl_t1errevt           dl_dtiinfo.dti_t1errevt
#define dl_idlest             dl_dtiinfo.dti_idlest
#define dl_idletyp            dl_dtiinfo.dti_idletyp
#define dl_sigmod             dl_dtiinfo.dti_sigmod


/*
 * Device-dependent BOARD structure for dxxx-type devices.
 */
struct dx_board {
   ULONG       db_logtype;       /* Logical board type code */
   ULONG       db_phystype;      /* Physical device type */
   ULONG       db_vr160bd;       /* VR160 daughterbd ID (VR160 only; else 0) */
   ULONG       db_fwver;         /* Version of FW on this board */
   SR_BDSRAM   db_sr;            /* Board shared RAM parameters */
   SR_IRQLIST  db_irq;           /* PM IRQ list info */
   DX_FWI      db_fwi[5];        /* FWI's for this board */
#ifdef sun	/* This is not required for ISA */
   GN_BOARD    *gbp_same_fwi_addr;
#endif /* sun */
               /* Pointer to the next GN_BOARD that shares the same
               ** shared RAM address.  For instance, the Span card
               ** has the DTI and the first voice virtual board sharing 
               ** the same shared RAM area.  This field is critically
               ** important for the VME Span card in which the DTI
               ** and the first virtual boards maintain the same pending
               ** commands queue in which device commands are queued
               ** because the DTI/first-voice-virtual board is busy.  In
               ** the VME Span card, pending commands for the DTI virtual 
               ** board will go to the that of the first voice virtual board.
               ** The default value of this variable is to point back to its
               ** own GN_BOARD.  The value will only be changed to that of the
               ** voice virtual board that uses the same shared RAM address.
               ** When PC_REQZERO_INT comes in, the commands can be
               ** dispatched from the gbp_same_fwi_addr board in the order 
               ** they were queued, FIFO.
               **
               ** In Summary, when two virtual boards share the same
               ** shared RAM addresses, their pending queues must be
               ** collapsed into one.  This way, the order of the
               ** commands going to the board from the application
               ** can be maintained.
               */
#ifdef _ISDN
   USHORT      db_nfasflag;      /* Board using NFAS??? B or D channel. */
   USHORT      db_isdnintfcid;   /* ISDN interface id. */
   GN_BOARD   *db_bchanbdp;      /* Ptr to B channel GN_BOARD structure */
   GN_BOARD   *db_dchanbdp;      /* Ptr to D channel GN_BOARD structure */
#ifdef BRI_SUPPORT
   USHORT      db_dslnum;        /* BRI board DSL number */
#endif /* BRI_SUPPORT */
#else
   ULONG       db_rfu[4];
#endif /* _ISDN */
};

/*
 * Macros for easier structure member access
 */
#define db_physaddr              db_sr.br_physaddr
#define db_sramaddr              db_sr.br_sramaddr
#define db_ioport                db_sr.br_ioport
#define db_totmem                db_sr.br_totmem
#ifdef PCI_SPAN
#define db_pcibrdid              db_sr.br_pcibrdid
#endif /* PCI_SPAN */

#ifdef _ISDN
/*
 * Flag to specify if NFAS is in use and the type of NFAS board i.e.
 * B Channel or D Channel.
 */
#ifdef LFS
#define IN_NFAS                  (0x0001)
#define IN_NFAS_B                (IN_NFAS | 0x0002)
#define IN_NFAS_D                (IN_NFAS | 0x0004)
#else
#define IN_NFAS                  0x0001
#define IN_NFAS_B                IN_NFAS | 0x0002
#define IN_NFAS_D                IN_NFAS | 0x0004
#endif
#endif /* _ISDN */


#endif

