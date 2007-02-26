/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/***********************************************************************
 *        FILE: fx12low.h
 * DESCRIPTION: FAX FW command/reply defines 
 *
 **********************************************************************/

#ifndef __FX12LOW_H__
#define __FX12LOW_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 * FAX device type
 */
#define FAX_ID                   0x34  /* FAX device number */

/* Numberof bytes of status sent by FW along with Send C and Rcv C msgs */
#define FX_STAT_SZ               12

#define FL_MSB                   1     /* MS bit in byte filled first */
#define FL_LSB                   2     /* LS bit in byte filled first */

/* Defines for resolution on receive side: resln field of FX_STATUS */
#define D_FINE                   0
#define D_COARSE                 1

/* define for poll indication in Rcv_phase_D_final msg */
#define D_POLL                   3

/*
 * FX_STATUS
 * FAX channel status block
 */
typedef struct fx_status {
  unsigned short pc_resol;
  unsigned short pc_width;
  unsigned short pc_baud;
  unsigned short pc_scanlines;
  unsigned short pc_protstat;
  unsigned short pc_badscanlines;
  unsigned short pc_coding;
} FX_STATUS;

/*
 * host to board : Fax commands
 */
#define RD_FAX_VER               1
#define RD_FAX_CPR               2
#define UP_FAX_CPRM              3
#define RD_FAX_CPRM              4
#define UP_FAX_BPRM              5
#define RD_FAX_BPRM              6
#define GET_FAX_ST               7
#define SEND_FAX                 8
#define RCV_FAX                  9
#define STOP_FAX                 10
#define CLEAN_FAX                11
#define SET_INIT_STATE           12
#define LOAD_FONT                13

/*
 * board to host : Fax responses
 */
#define FAX_VER_CMPLT            1
#define FAX_CPR_CMPLT            2
#define UP_CPRM_CMPLT            3
#define RD_CPRM_CMPLT            4
#define UP_BPRM_CMPLT            5
#define RD_BPRM_CMPLT            6
#define FAX_ERROR                7
#define GET_ST_CMPLT             8
#define SEND_C_CMPLT             9
#define SEND_D_CMPLT             10
#define SEND_DE_CMPLT            11
#define SEND_E_CMPLT             12
#define RCV_C_CMPLT              13
#define RCV_D_CMPLT              14
#define RCV_DE_CMPLT             15
#define RCV_E_CMPLT              16
#define FX_B0FULL                17
#define FX_B1FULL                18
#define FX_B0EMPTY               19
#define FX_B1EMPTY               20
#define CLEAN_CMPLT              21
#define INIT_STATE_CMPLT         22
#define SEND_B_CMPLT             23
#define RCV_B_CMPLT              24
#define SEND_C_PP_CMPLT          25
#define LOAD_FONT_CMPLT          26

/*051101, Add*/
#define SEND_B_CMPLT_1           27
#define RCV_B_CMPLT_1            28 
#define SEND_D_CMPLT_1           29
#define RCV_D_CMPLT_1            30 
#define SEND_E_CMPLT_1           31
#define RCV_E_CMPLT_1            32 
#define CLEAN_CMPLT_1            33 
/*END*/

/* 
 * fax error codes
 */
#define FX_CBLK_ERR              1
#define FX_GBLK_ERR              2
#define FX_CPRM_ID_ERR           3 
#define FX_GPRM_ID_ERR           4 
#define FX_SUB_CMD_ERR           5
#define FX_CH_ERR                6
#define FX_NULL_ERR              7
#define FX_MULTI_ERR             8
#define FX_TS_ERR                9
#define FX_NO_INIT_STATE         10

#define S_CLEAN                  ((event_flags)0x00010000)

#define FX_S_RCV                 0     /* MAPLE will be a Receiver */
#define FX_S_TRS                 1     /* MAPLE will be a Transmitter */
#define FX_NO_STATE              2

#define FX_OK2POLL               0x02  /* Polling allowed (for rcvfax) */

/*
 * board to host : Fax response masks for Protocol Module
 */
#define FAX_ERROR_RPY           ((FAX_ID << 8)|FAX_ERROR)
#define FX_SEND_C_CMPLT_RPY     ((FAX_ID << 8)|SEND_C_CMPLT)
#define FX_SEND_C_PP_CMPLT_RPY  ((FAX_ID << 8)|SEND_C_PP_CMPLT)
#define FX_SEND_E_CMPLT_RPY     ((FAX_ID << 8)|SEND_E_CMPLT)     
#define FX_RCV_C_CMPLT_RPY      ((FAX_ID << 8)|RCV_C_CMPLT)
#define FX_RCV_E_CMPLT_RPY      ((FAX_ID << 8)|RCV_E_CMPLT)
#define FX_B0FULL_RPY           ((FAX_ID << 8)|FX_B0FULL)
#define FX_B1FULL_RPY           ((FAX_ID << 8)|FX_B1FULL)
#define FX_B0EMPTY_RPY          ((FAX_ID << 8)|FX_B0EMPTY)
#define FX_B1EMPTY_RPY          ((FAX_ID << 8)|FX_B1EMPTY)
#define FX_LOAD_FONT_CMPLT_RPY  ((FAX_ID << 8)|LOAD_FONT_CMPLT)


#endif

