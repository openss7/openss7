/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/***********************************************************************
 *        FILE: dxlibdrv.h
 * DESCRIPTION: Common structs/defs between libraries and the SRAM PM
 *
 **********************************************************************/

#ifndef __DXLIBDRV_H__
#define __DXLIBDRV_H__

#ifdef __STANDALONE__            /* List of required header files */
//#include "typedefs.h"
#include "gnmsg.h"
#include "d40defs.h"
#endif


typedef struct dsp_info {
   ULONG dcb_dsp;
} DSP_INFO;

/*
 * Miscellaneous defines
 */
#define MAXDATA			 0x1000
#define DV_SEND                  0x07
#define DV_MAXCHANONBD           12    /* EFN: FIX LATER? */


/*
 * FWI defines
 */
#define IO_CHNFWI                0x0000   /* Channel buffer area FWI type */
#define IO_DEVFWI                0x0001   /* Device area FWI type */
#define IO_CHNDATA               0x0100   /* Extra channel-area data */
#define IO_DEVDATA               0x0200   /* Extra dev-area data */

/*
 * Unique D/xxx H/W identifier (m.s.b. of dl_prod)
 */
#define DXXX_SIGN                0x80

/*
 * D/xxx command group identifiers
 */
#define DXXX_BD_ID               0x40
#define DXXX_CH_ID               0x30

/* 
 * Commands available to send to D/xxx (group DXX_BD_ID)
 */
#define DXXX_READFWVR_CMD        0x1   /* read FW version */
#define DXXX_READCOPY_CMD        0x2   /* read diagolic copyright */
#define DXXX_WRITEPARM_CMD       0x5   /* update parameter */
#define DXXX_READPARM_CMD        0x6   /* read parameter */
#define DXXX_READSERL_CMD        0x7   /* read boards serial number */

/*
 * Commands available to send to D/xxx (group DXX_CH_ID)
 */
#define DXXX_ASSIGNTS_CMD        0x7   /* assign timeslot */

/* 
 * Replies available from D/xxx (group DXXX_BD_ID)
 */
#define DXXX_READFWVR_RPY        0x1   /* read FW version complete */
#define DXXX_READCOPY_RPY        0x2   /* read diagolic cpyrght cmplt */
#define DXXX_WRITEPARM_RPY       0x5   /* update parameter complete */
#define DXXX_READPARM_RPY        0x6   /* read parameter complete */
#define DXXX_ERROR               0x7   /* error in last command */
#define DXXX_READSERL_RPY        0x8   /* read boards serial number */

/* 
 * Replies available from D/xxx (group DXXX_CH_ID)
 */
#define DXXX_ASSIGNTS_RPY        0x8   /* T.S. assigned */

/*
 * Error replies 
 */
#define  DX_CHAN_BUSY            0x1   /* busy exec channel blkng fn */
#define  DX_GLOB_BUSY            0x2   /* busy exec global blkng fn */
#define  DX_INVL_CHAN_PARM       0x3   /* invalid channel param num */
#define  DX_INVL_GLOB_PARM       0x4   /* invlaid global param num */
#define  DX_INVL_SUB_CMD         0x5   /* invalid sub command number */
#define  DX_INVL_CHAN_NUM        0x6   /* invalid channel number */
#define  DX_MISS_NULL            0x7   /* no null term in char string */


/*
 * Device-specific error codes
 */
#define DLE_BADRANGE             1     /* Bad/overlapping phys mem range */
#define DLE_TIMEOUT              2     /* Timed out waiting for reply */
#define DLE_BADSIZE              3     /* Message too big or too small */
#define DLE_BADBOARD             4     /* Board missing or dead */
#define DLE_NOMEM                5     /* Can't map or allocate memory */
#define DLE_BADCMD               6     /* Invalid or undefined command */
#define DLE_BADPARAM             7     /* Invalid command parameter */
#define DLE_FWERROR              8     /* FW returned an error as reply */
#define DLE_SKIPREPLY            9     /* A required reply was skipped */
#define DLE_TOOBIG               10    /* Reply too big for user buffer */
#define DLE_BADSTATE             11    /* An invalid state was detected */

/*
 * DTI parameter info support
 */
#define DTIP_MAXPARM             12
#define DTIP_IDLESTATE           0
#define DTIP_IDLETYPE            1
#define DTIP_SIGMODE             2
#define DTIP_T1ERREVT            3
#define DTIP_PDIGEVT             4

#define DP_READ                  0
#define DP_WRITE                 1
#define DP_ADDBITS               2
#define DP_DELBITS               3

#define DTI_BDDEV                0
#define DTI_TSDEV                1


/*
 * DXXX_DATA: D4X-dependent data area for library-to-driver msgs
 */
typedef struct dxxx_data {
   ULONG    dxxx_maxrepsize;
   USHORT   dxxx_bufsz;
   USHORT   dxxx_offset;
   ULONG dxxx_rfu[2];
} DXXX_DATA;


/*
 * DX_DEVDEP: union of all D4X-dependent data areas that may be put into
 * a driver-to-library message. This union is incorporated into the 
 * DL_DRVCMD message structure.
 */
typedef union dx_devdep {
   DXXX_DATA  dv_data;
} DX_DEVDEP;


#define dx_maxrepsize            dx_devdata.dv_data.dxxx_maxrepsize
#define dx_bufsz                 dx_devdata.dv_data.dxxx_bufsz     
#define dx_offset                dx_devdata.dv_data.dxxx_offset    


/*
 * DL_DRVCMD
 */
typedef struct dl_drvcmd {
   GN_CMDMSG   gn_drvhdr;        /* Generic driver header */
   DX_DEVDEP   dx_devdata;       /* Device dependent data area */
   DX_CMND     dc_cmnd;          /* Command message to send to D/xxx */
   DX_REPLY    dc_reply;         /* Reply msg area for driver to fill */
   ULONG       dc_part2len;      /* Size of 2nd part of data packet */
   ULONG       dc_threshold;     /* Play/record msg-sending threshold */
   ULONG       dc_lostevents;    /* Play/record under/overflow count */
   UCHAR       dc_rfu[12];       /* Reserved for future use */
} DL_DRVCMD;


/*
 * DX_DEVINFO -- Device information structure
 */
typedef struct dl_devinfo {
   ULONG di_type;                   /* Device type */
   UCHAR di_name[LD_NAMELEN];       /* Device name */
   struct {
      UCHAR ci_bdname[LD_NAMELEN];  /* It's on this board */
      ULONG ci_chno;                /* Channel no. on board */
   } di_c;                          /* Info for a CHANNEL */
   struct {
      ULONG  bi_paddr;              /* Physical board address */
      ULONG  bi_vaddr;              /* Virtual board address */
      ULONG  bi_btype;              /* Board type: D40 or 41 */
      ULONG  bi_vers;               /* Firmware revision no. */
      USHORT bi_nchans;             /* No. of channels on board */
      USHORT bi_irq;                /* Irq in use by this board */
      UCHAR  bi_chnames[DV_MAXCHANONBD][LD_NAMELEN]; 
   } di_b;                          /* Info for a BOARD */
   D4XSRAM   bdi_d4xsram;
} DX_DEVINFO;

typedef struct sr_dump {
	unsigned char sr_data[MAXDATA];
} SR_DUMP;

/*
 * Macros for easier access to the structure members
 */
#define chi_bdname               di_c.ci_bdname
#define chi_chno                 di_c.ci_chno

#define bdi_chnames              di_b.bi_chnames
#define bdi_paddr                di_b.bi_paddr
#define bdi_vaddr                di_b.bi_vaddr
#define bdi_nchans               di_b.bi_nchans
#define bdi_btype                di_b.bi_btype
#define bdi_vers                 di_b.bi_vers
#define bdi_irq                  di_b.bi_irq


/*
 * Macros for encoding and decoding device area replies.
 */
#define DV_REPENCODE(cmd,sub)   (((cmd) << 8) | (sub))
#define DV_REPDECODE(reply)     (((reply).dl_cmd << 8) | (reply).dl_sub)


/*
 * Device-dependent structure for programmable stop command
 */
typedef struct {
   DX_CMND  ds_cmd;
   ULONG    ds_flag;
   ULONG    ds_fwi;
} DX_DEVSTOP;


/*
 * Device-dependendt structure for DTI parameter information.
 */
typedef struct {
   UCHAR dt_parmid;
   UCHAR dt_cmdid;
   UCHAR dt_devtype;
   UCHAR dt_rfu;
   ULONG dt_data;
} DTI_PARMINFO;


#endif


