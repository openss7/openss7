/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : sbadefs.h
 * Description                  : SRAM board definitions
 *
 *
 **********************************************************************/

#ifndef __SBADEFS_H__
#define __SBADEFS_H__

#ifdef __STANDALONE__            /* List of required header files */
#include "sbdmsg.h"
#endif

/*
 * Ensure all structures are packed.
 */
#pragma pack(1)

/*
 * Springboard memory map.
 */
#define SB_CFGOFF   0x1E80      /* SBA config struct offset from last 'board' */
#define SB_DNLDOFF  0x200       /* SBA dnld struct offset from last 'board' */
#define SB_BUFSIZ   0x0600      /* SBA download buffer size */


/*
 * Springboard configuration structure
 */
typedef struct sb_config {
    unsigned short  cfg_tslot;              /* The first timeslot to be used */
    unsigned char   cfg_rfu[0x40 - 0x02];   /* Reserved for future use */
} SB_CONFIG;

#ifdef BIG_ENDIAN
#define SB_CONFIG_FORMAT	"2[62]1"
#endif /* BIG_ENDIAN */

/*
 * Springboard download structure
 */
typedef struct sbabuff {
    unsigned char   sb_buff[SB_BUFSIZ]; /* SBA download buffer */
    unsigned char   sb_fill[0x01700];
    SB_MESSAGE      sb_pc_to_sba;       /* PC_TO_SBA message area */
    SB_MESSAGE      sb_sba_to_pc;       /* SBA_TO_PC message area */
} SBABUFF;

/*
 * PC to Springboard commands
 */
#define PCSB_DNLD       0               /* Download */
#define PCSB_START      1               /* Start downloaded code */

#define DTI_CMD		0x02
#define DTI_START	0x15

/*
 * FirmWare commands
 */
#define	SB_DNLDGENBOOT		(MC_BASEMSGNUM + 1)
#define	SB_GENBOOTCMPLT		 SB_DNLDGENBOOT
#define SB_STARTGBOOT		(MC_BASEMSGNUM + 2)
#define	SB_STARTGBCMPLT		 SB_STARTGBOOT
#define	SB_DNLDFWL		(MC_BASEMSGNUM + 3)
#define SB_DNLDFWLCMPLT		 SB_DNLDFWL
#define	SB_DEVSEND		(MC_BASEMSGNUM + 4)


/*
 * Miscellaneous defines
 */
#define SB_REQ          0xF0            /* pc_req/dl_req value for SBA */

#define	SB_MESSAGE_FORMAT	"22222222"

/*
 * Restore default packing alignment
 */
#pragma pack()

#endif


