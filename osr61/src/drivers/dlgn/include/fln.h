/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/***********************************************************************
 *        FILE: fln.h
 * DESCRIPTION: Common FW link structs/defs between libraries & SRAM PM
 *
 **********************************************************************/

#ifndef __FLN_H__
#define __FLN_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 * FLN defines
 */
#define DEVICE                   0xFF     /* PC_REQ/DL_REQ: cmd in dev area */
#define DEV_BOARD                0xFF     /* HOST_DATA4/DEV_DATA3: bd dev */

#define MF40_ID                  0x10     /* id for mf40 */ 
#define FLN_ID                   0x11     /* id number for fln */ 
#define SX_ID                    0x14     /* id for mitel board */ 
#define SL_ID                    0x15     /* id for nortel board */ 
#define NS_ID                    0x16     /* id for norstar board */

#define NE2_ID                   0x18     /* id for NE2 boards */
#define M1_ID                    0x19     /* id for M1 boards */
#define PA_ID                    0x1A     /* id for PA boards */

#define D82U_ID                  0x1C     /* id for D/82 boards */
#define D162U_ID                 0x1D     /* id for D/162 boards */

#define VR_ID                    0x31     /* id for the VR/40 boards */

#define DB_SEG                   0x8000   /* segment for any attached board */ 
#define MF40_SEG                 DB_SEG   /* segment for mf40 */ 
#define DB_OFFSET                0x100    /* offset for any attached board */ 
#define MF40_OFFSET              DB_OFFSET   /* offset to mf40 header */ 

/*
 * The following are FLN commands
 */
#define FLN_RFU1_CMD             0     /* an empty command */ 
#define FLN_REP_CMD              1     /* report linked board at address */ 
#define FLN_UNLNK_CMD            2     /* unlink the board at address */ 
#define FLN_DNLD_CMD             3     /* download hex file at address */ 
#define FLN_LNK_CMD              4     /* link board at address */ 
#define FLN_NONDUNLK_CMD         5     /* unlink bd but don't erase header */ 
#define FLN_MEMCHK_CMD           6     /* perform non-destructive mem test */ 
#define FLN_READ_CMD             7     /* read the given memory location */ 

/*
 * The following are FLN messages
 */
#define FLN_REP_BOARD            1     /* report if a board at given addr */
#define FLN_UNLINK_CMPLT         2     /* unlink complete message */ 
#define FLN_DNLD_CMPLT           3     /* download complete message */ 
#define FLN_LINK_CMPLT           4     /* link complete message */ 
#define FLN_NOUNLINK_CMPLT       5     /* non destructive unlink complete */ 
#define FLN_MEMCHK_CMPLT         6     /* memory check complete message */ 
#define FLN_READ_CMPLT           7     /* read complete message */ 
#define FLN_ERROR                8     /* an error message for the PC */ 

/*
 * Equates for error codes in memory test
 */
#define MEM_PASS                 0     /* Passed memory test */ 
#define MEM_ERR1                 1     /* Error writing inverse of data */ 
#define MEM_ERR2                 2     /* Error in checksum of inverses */ 
#define MEM_ERR3                 3     /* Error restoring original data */ 
#define MEM_ERR4                 4     /* Error in checksum of restored data*/
#define MEM_ERR5                 5     /* Error in walking 0 bit test */
#define MEM_ERR6                 6     /* Error in walking 1 bit test */


/*
 * Pack to one-byte boundaries
 */
#pragma pack(1)


typedef struct device_area {                      
   unsigned char        host_cmd;      /* the host command for device */ 
   unsigned short int   host_data;     /* the first data field */ 
   unsigned char        host_data2;    /* the second data field */ 
   unsigned short int   host_data3;    /* the last data field */ 
   unsigned char        host_sub;      /* the host sub command for device */ 
   unsigned char        host_data4;    /* rfu */ 
   unsigned char        dev_cmd;       /* the dev command for host */ 
   unsigned char        dev_rfu1;
   unsigned short int   dev_data;      /* the first data field */ 
   unsigned short int   dev_bcnt;      /* the last data field */ 
   unsigned char        dev_sub;       /* the dev sub command for host */ 
   unsigned char        dev_data2;     /* rfu1 */ 
   unsigned char        host_buf [48]; /* 30H bytes for host buffer */ 
   unsigned char        dev_buf [48];  /* 30H bytes for device buffer */ 
} DEV_AREA;

/*
 * Restore to default 4-byte boundary packing
 */
#pragma pack()


#endif

