/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdwwdefs.h
 * Description                  : ww definitions
 *
 *
 **********************************************************************/

#ifndef _MSDWWDEFS_H_
#define _MSDWWDEFS_H_

//Adapter States for WW mode
#define MERCD_ADAPTER_WW_MODE_STATE   0x0001

//States of padapter->flags.WWFlags
//Anytime WWFlags can have
// 1. in SRAM mode : WWFlags = MERCD_ADAPTER_WW_MODE_NOTENABLED
// 2. When we give a SetMode for WW:
//          -if it can find the sequence in diagnostic location
//           then WWFlags = MERCD_ADAPTER_WW_MODE_ENABLED
//          -after we receive the first init doorbell
//           WWFlags = (MERCD_ADAPTER_WW_MODE_ENABLED |
//                     MERCD_ADAPTER_WW_I20_MESSAGING_READY)
// 3. Any time the RCV BIG MSG timer kicks on then:
//           WWFlags = (MERCD_ADAPTER_WW_MODE_ENABLED |
//                      MERCD_ADAPTER_WW_I20_MESSAGING_READY |
//                      MERCD_ADAPTER_WW_RCV_BIGMSGBUF_POSTING_ON)

#define MERCD_ADAPTER_WW_MODE_ENABLED              (MERCD_ADAPTER_WW_MODE_STATE)
#define MERCD_ADAPTER_WW_MODE_NOTENABLED           (MERCD_ADAPTER_WW_MODE_STATE << 1)
#define MERCD_ADAPTER_WW_I20_MESSAGING_READY       (MERCD_ADAPTER_WW_MODE_STATE << 2)

//States of pwwDev->state
#define MERCD_ADAPTER_WW_MODE_INIT_INTR_PENDING    (MERCD_ADAPTER_WW_MODE_STATE << 3)
#define MERCD_ADAPTER_WW_MODE_INIT_INTR_ACKED      (MERCD_ADAPTER_WW_MODE_STATE << 4)
#define MERCD_ADAPTER_WW_MODE_INIT_MSG_READY_INTR_PENDING \
                                                   (MERCD_ADAPTER_WW_MODE_STATE << 5)
#define MERCD_ADAPTER_WW_MODE_INIT_MSG_READY_INTR_ACKED \
                                                   (MERCD_ADAPTER_WW_MODE_STATE << 6)
#define MERCD_ADAPTER_WW_SUCCEDED                  (MERCD_ADAPTER_WW_MODE_STATE << 7)
#define MERCD_ADAPTER_WW_FAILED                    (MERCD_ADAPTER_WW_MODE_STATE << 8)
#define MERCD_ADAPTER_WW_NOTREADY                  (MERCD_ADAPTER_WW_MODE_STATE << 9)
#define MERCD_ADAPTER_WW_IN_SHUTDOWN               (MERCD_ADAPTER_WW_MODE_STATE << 10)
#define MERCD_ADAPTER_WW_MIN_POST_ENABLE           (MERCD_ADAPTER_WW_MODE_STATE << 11)

//Pending Messages Priority Bits: refered by pwwDev->WWMsgPendingQFlag
#define MERCD_ADAPTER_WW_PENDING_MSG_Q_MASK   0x0001
#define MERCD_ADAPTER_WW_CANCEL_FROM_BD_BIGMSGBUF_MF_BIT (MERCD_ADAPTER_WW_PENDING_MSG_Q_MASK)
#define MERCD_ADAPTER_WW_CANCEL_STREAM_MF_BIT (MERCD_ADAPTER_WW_PENDING_MSG_Q_MASK << 1)
#define MERCD_ADAPTER_WW_FLUSH_STREAM_MF_BIT (MERCD_ADAPTER_WW_PENDING_MSG_Q_MASK << 2)
#define MERCD_ADAPTER_WW_FROM_BD_BIGMSGBUF_MF_BIT (MERCD_ADAPTER_WW_PENDING_MSG_Q_MASK << 3)
#define MERCD_ADAPTER_WW_FROM_BD_BIGMSGBUF_MF_POSTING_BIT (MERCD_ADAPTER_WW_PENDING_MSG_Q_MASK << 4)
#define MERCD_ADAPTER_WW_NORMAL_MSG_MF_BIT (MERCD_ADAPTER_WW_PENDING_MSG_Q_MASK << 5)
#define MERCD_ADAPTER_WW_FROM_BD_DATABUF_MF_BIT (MERCD_ADAPTER_WW_PENDING_MSG_Q_MASK << 6)
#define MERCD_ADAPTER_WW_MULTI_EOS_MF_BIT (MERCD_ADAPTER_WW_PENDING_MSG_Q_MASK << 7)
#define MERCD_ADAPTER_WW_DATA_WITH_NOEOS_BIT (MERCD_ADAPTER_WW_PENDING_MSG_Q_MASK << 8)
#define MERCD_ADAPTER_WW_FLUSH_STREAM_MSG_PEND_BIT (MERCD_ADAPTER_WW_PENDING_MSG_Q_MASK << 9)

//Some misc with respect messages
#define MERCD_ADAPTER_WW_SEND_MSG_FLUSH_ONLY        0x1
#define MERCD_ADAPTER_WW_SEND_MSG_ALL               0x2

//Misc with respect to streaming
#define MERCD_ADAPTER_WW_FLAG_SEND_DATA_PEND 0x1

//WW Post location content definitons
#define MERCD_WW_POSTLOCATION_F8_8BIT_CONTENT   0x02
#define MERCD_WW_POSTLOCATION_FC_8BIT_CONTENT   0xFD
#define MERCD_WW_POSTLOCATION_F8_16BIT_CONTENT  0x02
#define MERCD_WW_POSTLOCATION_FC_16BIT_CONTENT  0x7D

//SRAM Post Location contents
#define MERCD_SRAM_POSTLOCATION_F8_8BIT_CONTENT    0x03
#define MERCD_SRAM_POSTLOCATION_FC_8BIT_CONTENT    0xFC
#define MERCD_SRAM_POSTLOCATION_F8_16BIT_CONTENT   0x03
#define MERCD_SRAM_POSTLOCATION_FC_16BIT_CONTENT   0x7C

//Misc Posting Status
#define MERCD_WW_POST_NOT_STARTED           0x00
#define MERCD_WW_POST_IN_OPERATION          0x01
//Since MD_SUCCESS == 0 and MD_FAILRE == 1
#define MERCD_WW_POST_IN_PROGRESS           0x2
#define MERCD_WW_INIT_POST_IN_PROGRESS      0x81

//Post is taking place for which kernel
#define MERCD_WW_BOOT_KERNEL               0x01
#define MERCD_WW_RUNTIME_KERNEL            0x02

// Interrupt Types
#define MERCD_WW_INIT_INTERRUPT                      0x00000001
#define MERCD_WW_INIT_MSG_READY                      0x00000100
#define MERCD_WW_INIT_ACK_INTERRUPT                  0x00008001
#define MERCD_WW_INIT_COMPLETE_INTERRUPT             0x00000002
#define MERCD_WW_MESSAGEREADY_INTERRUPT              0x00000004
#define MERCD_WW_RESET_INTERRUPT_TO_BOARD            0x00000008
#define MERCD_WW_RESET_INTERRUPT_ACK_FROM_BOARD      0x00008008
#define MERCD_WW_RESET_INTERRUPT_FROM_BOARD          0x00000010
#define MERCD_WW_RESET_INTERRUPT_ACK_TO_BOARD        0x00008010
#define MERCD_WW_CANCEL_REQUESTS_INTERRUPT           0x00000020
#define MERCD_WW_CANCEL_REQUEST_INTERRUPT_ACK        0x00008020
#define MERCD_WW_ERROR_INTERRUPT                     0x00000040
#define MERCD_WW_SOFT_RESET_BOARD                    0x00000080

#define MERCD_WW_INIT_ALL_INTR_MASK ( MERCD_WW_INIT_ACK_INTERRUPT | \
                                      MERCD_WW_INIT_COMPLETE_INTERRUPT | \
                                      MERCD_WW_MESSAGEREADY_INTERRUPT | \
                                      MERCD_WW_RESET_INTERRUPT_ACK_FROM_BOARD | \
                                      MERCD_WW_CANCEL_REQUEST_INTERRUPT_ACK | \
                                      MERCD_WW_RESET_INTERRUPT_FROM_BOARD )

//MF Generic Classification
#define MERCD_WW_BIG_MESSAGE                               0x01
#define MERCD_WW_SMALL_MESSAGE                             0x02

// Various MF Types --Per Se Protocol
#define MERCD_WW_CONFIGURATION_MSG_TO_BOARD                 0x00010000
#define MERCD_WW_MSG_BLOCK_INIT_MSG                         0x00020000
#define MERCD_WW_SMALL_MSG_TO_BOARD                         0x00030000
#define MERCD_WW_SMALL_MSG_FROM_BOARD                       0x00040000
#define MERCD_WW_BIG_MSG_TO_BOARD                           0x00050000
#define MERCD_WW_BIG_MSG_FROM_BOARD                         0x00060000
#define MERCD_WW_BIG_MSG_READ_ACK_FROM_BOARD                0x80050000
#define MERCD_WW_BIG_MSG_CANCELLATION_TO_BOARD              0x00070000
#define MERCD_WW_BIG_MSG_CANCELLATION_ACK_FROM_BOARD        0x80070000
#define MERCD_WW_READ_REQUEST                               0x00080000
#define MERCD_WW_READ_REQUEST_COMPLETION                    0x80080000
#define MERCD_WW_WRITE_REQUEST                              0x00090000
#define MERCD_WW_WRITE_REQUEST_COMPLETION                   0x80090000
#define MERCD_WW_STREAM_REQ_CANCEL_TO_BOARD                 0x000A0000
#define MERCD_WW_STREAM_REQ_CANCEL_ACK_FROM_BOARD           0x800A0000
#define MERCD_WW_STREAM_FLUSH_REQUEST_TO_BOARD              0x000B0000
#define MERCD_WW_STREAM_FLUSH_REQUEST_ACK_FROM_BOARD        0x800B0000
#define MERCD_WW_DATA_BUFFER_REQUEST_FROM_BOARD             0x000C0000
#define MERCD_WW_EOS_MSG_TO_BOARD                           0x000D0000
#define MERCD_WW_EOS_READ_STREAM_FROM_BOARD                 0x000E0000
#define MERCD_WW_MULTIPLE_STREAM_EOS_TO_BOARD_MSG           0x00100000
#define MERCD_WW_BIG_XMSG_TO_BOARD                          0x00110000
#define MERCD_WW_BIG_XMSG_READ_ACK_FROM_BOARD               0x80110000
#define MERCD_WW_BIG_MSG_FROM_BOARD_INMF_START              0x00120000
#define MERCD_WW_BIG_MSG_FROM_BOARD_INMF_CONTINUE           0x00130000
#define MERCD_WW_SESS_CLOSE                                 0x800E0000

// MF header size
#define MERCD_WW_MF_HEADER_SIZE 8

//Write Stream MF Header Size
//Type + StreamId                    = 8
//Context                            = 8
//StreamFlags                        = 8
//Bytes Transferred + Container Size = 8
#define MERCD_WW_WRITE_MF_HEADER_SIZE 32



// BIG Message Block Origin and Owenership
#define MERCD_WW_DMA_DESCR_FLAG  0x00000001 /* 1=host 0=board Ownership*/
#define MERCD_WW_DMA_DESCR_VBIT  (MERCD_WW_DMA_DESCR_FLAG) /* 1=host 0=board Ownership*/
#define MERCD_WW_DMA_DESCR_SBIT  (MERCD_WW_DMA_DESCR_FLAG << 1) /* Origin */
#define MERCD_WW_DMA_DESCR_CBIT  (MERCD_WW_DMA_DESCR_FLAG << 2) /* Origin */
#define MERCD_WW_DMA_DESCR_EBIT  (MERCD_WW_DMA_DESCR_FLAG << 3) /* Origin */
#define MERCD_WW_DMA_DESCR_PBIT  (MERCD_WW_DMA_DESCR_FLAG << 4) /* Origin */

//Rcv BIG Msg Table Allocation
#define MERCD_WW_BIGMSG_RCV_DESCR_TABLE_ALLOC   0x50201
#define MERCD_WW_BIGMSG_RCV_DESCR_ALLOC         0x50202
//Rcv STRM Data Tble Allocation
#define MERCD_WW_STRMDATA_RCV_DESCR_TABLE_ALLOC 0x50203
#define MERCD_WW_STRMDATA_RCV_DESCR_ALLOC       0x50204

//Rcv BIG Msg MF specific
#define MERCD_WW_BIGMSG_BUF_INIT 0x1
#define MERCD_WW_BIGMSG_BUF_RECLAIM 0x2

//Rcv BIG Msg New PROTOCOL Specifics
#define MERCD_WW_BIGMSG_MAX_BODY_SIZE  0x800 /*2k*/

//Write Stream Specific
#define MERCD_WW_GSTRM_WRITE_REQUEST  0x1
#define MERCD_WW_BSTRM_WRITE_REQUEST  0x2
#define MERCD_WW_STRM_WRITE_COMPLETE  0x3
#define MERCD_WW_STREAM_DESCR_DEALLOC 0x1
#define MERCD_WW_MAX_MFS_PER_STREAM   10

//Read Stream Specific
#define MERCD_WW_READ_REQ_COMPLETION_DUE_TO_STREAM_CANCEL 0x00000080

//We arrived at number 10, as recommended by Kernel Group.
#define MERCD_WW_MAX_PRE_ALLOCATED_RECV_STREAM_BUFFERS  10
//Rcv Stream Databuf Post Method
#define MERCD_WW_RCVSTRM_DATABUF_POST_DEFERRED   0x60201
#define MERCD_WW_RCVSTRM_DATABUF_POST_IMMEDIATE  0x60202

//Forced UnBind: UnBind Called without driver close
#define MERCD_WW_FLAG_FORCED_UNBIND 0x4

#define MERCD_WW_MAX_RESET_TIME 200

//WW macros
#define VALID_WW_8BIT_POST_CONTENTS ((ValLocation1 == MERCD_WW_POSTLOCATION_F8_8BIT_CONTENT) && (ValLocation2 == MERCD_WW_POSTLOCATION_FC_8BIT_CONTENT))
#define VALID_WW_16BIT_POST_CONTENTS ((ValLocation1 == MERCD_WW_POSTLOCATION_F8_16BIT_CONTENT) && (ValLocation2 == MERCD_WW_POSTLOCATION_FC_16BIT_CONTENT))

#define VALID_SRAM_8BIT_POST_CONTENTS ((ValLocation1 == MERCD_SRAM_POSTLOCATION_F8_8BIT_CONTENT) && (ValLocation2 == MERCD_SRAM_POSTLOCATION_FC_8BIT_CONTENT))
#define VALID_SRAM_16BIT_POST_CONTENTS ((ValLocation1 == MERCD_SRAM_POSTLOCATION_F8_16BIT_CONTENT) && (ValLocation2 == MERCD_SRAM_POSTLOCATION_FC_16BIT_CONTENT))

#endif /*_MSDWWDEFS_H_*/
