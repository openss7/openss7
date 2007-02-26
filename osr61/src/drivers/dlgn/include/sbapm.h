/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : sbapm.h
 * Description                  : sba-device definitions
 *
 *
 **********************************************************************/

#ifndef __SBAPM_H__
#define __SBAPM_H__

#ifdef __STANDALONE__            /* List of required header files */
#include "gndrv.h"
#include "typedefs.h"
#include "srmap.h"
#include "sbadefs.h"
#include "genbootdef.h"
#endif

#ifdef BIG_ENDIAN
#include "endian.h"
#endif

#define MODULE_NAME "SBAPM"
#define SBAPM_ID	401

/* Solaris does not support these functions anymore in 2.6 */

#ifdef SOLARIS_24
#ifndef inb
#define       inb(port)       (*(volatile u_char *)(port))
#endif
 
#ifndef outb
#define       outb(port, value)       (*(u_char *)(port) = (value))
#endif
#endif /* SOLARIS_24 */

#ifdef VME_DEBUG

#define STRACE0(x)	\
		strlog(SBAPM_ID, 0, 0, SL_TRACE, x)
#define STRACE1(x, one)	\
		strlog(SBAPM_ID, 0, 0, SL_TRACE, x, one)
#define STRACE2(x, one, two)	\
		strlog(SBAPM_ID, 0, 0, SL_TRACE, x, one, two)
#define STRACE3(x, one, two, three)	\
		strlog(SBAPM_ID, 0, 0, SL_TRACE, x, one, two, three)

#else

#define STRACE0(x) 
#define STRACE1(x, one) 
#define STRACE2(x, one, two) 
#define STRACE3(x, one, two, three) 

#endif /* VME_DEBUG */

/*
 * Flags for the SBOARD
 */
#define SB_GENBOOT_FLG	0x01		/* Genboot has been booted */
#define	SB_DNLDFWL_FLG	0x02		/* FWL has been loaded	   */

/*
 *  Spring Board Hardware I/O port register definitions
 */

#define DL_MSG_RDY_Bit	 ((unsigned char)0x40)
#define SB_MEMENABLE_Bit ((unsigned char)0x20)

/*
 * PM-defined flags used in ld_flags of GN_LOGDEV
 */
#define LD_DNLDSTART	0x0100


/*
 * Miscellaneous defines
 */
#define PGRPHSHIFT	4		/* Shift to convert paragrpahs->bytes */
#define SB_DRVMSGSZ	(sizeof(SB_DRVCMD) - sizeof(GN_CMDMSG))

#define	D4XD_UNLOCK1	0x0F8
#define	D4XD_UNLOCK2	0x0F5

#define D4XE_PAGEIN	0x0E0
#define D4XE_LOCATE1	0x0E4
#define D4XE_LOCATE2	D4XE_PAGEIN
#define	D4XE_UNLOCK1	0x0F8
#define	D4XE_UNLOCK2	0x0F4

#define D4XE_MAX_ID	0x0020

#define SB_MIN_IOPORT	0x0200
#define SB_MAX_IOPORT	0x0400

/*
 * Device-dependent FWI structure for sba-type devices.
 * SIZE == 16 bytes
 */
typedef struct sb_fwi {
   GN_LOGDEV	*fwi_ldp;		/* ldp associated with this FWI */
   UCHAR	*fwi_addr;		/* Shared RAM virtual address */
   USHORT	 fwi_port;		/* I/O Port Addesss */
   USHORT	 fwi_type;		/* FWI type ! */
   USHORT	 fwi_num;		/* FWI Number ! */
   USHORT	 fwi_rfu[1];
} SB_FWI;


/*
 * Device-dependent LOGDEV structure for sba-type devices.
 * SIZE == 32 bytes
 */
typedef struct sb_logdev {
   ULONG	 dl_type;		/* Device type code */
   ULONG	 dl_opencnt;		/* Number of opens on this device */
   mblk_t	*dl_sndq;		/* local send queue */
   mblk_t	*dl_rcvq;		/* local receive queue */
   SB_FWI	*dl_fwip;
   SB_FWI	*dl_nestfwip;
   ULONG	 dl_rfu[2];
} SB_LOGDEV;


/*
 *  This stores data if a reply is not to be reply is not to be sent immediately
 */
typedef struct sb_timed_reply {
   GN_LOGDEV	*tmr_ldp;
   mblk_t	*tmr_mp;
   long		 tmr_rc;
   ULONG	 tmr_rfu[1];
} SB_TIMED_REPLY;


/*
 * Device-dependent BOARD structure for sba-type devices.
 * SIZE == ?? bytes
 */
typedef struct sb_board {
   ULONG		 db_type;	/* Board type code */
   ULONG		 db_fwver;	/* Version of FW on this board */
   SR_BDSRAM		 db_sr;		/* Board Shared RAM parameters */
   SB_CONFIG		*db_config;
   SB_FWI		 db_fwi[1];
   GNB_BOOTCMD		 db_bootcmd;
   USHORT		 db_timeslot;
   USHORT		 db_flags;
   unsigned char	*virt_sramaddr;
   SB_TIMED_REPLY	 db_timed_reply;
   ULONG		 db_rfu[2];
} SB_BOARD;


/*
 * Extern Declarations
 */
extern SB_BOARD		*mapbd[];
extern int		mapmax;
extern GN_BOARD *Sba_Irq[];


#endif

