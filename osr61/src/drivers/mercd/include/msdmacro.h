/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdmacro.h
 * Description                  : Macro Definations
 *
 *
 **********************************************************************/

#ifndef _MDMACRO_
#define _MDMACRO_

#define MD_SET_MDMSG_BIND_HANDLE(msg,bind) \
	msg->MessageAddress.Bind.BindHandle = bind

#define MD_GET_MDMSG_BIND_HANDLE(msg) \
	msg->MessageAddress.Bind.BindHandle 

#define MD_SET_MDMSG_STREAM_HANDLE(msg,stream) \
	msg->MessageAddress.StreamHandle= stream

#define MD_GET_MDMSG_STREAM_HANDLE(msg) \
	(msg->MessageAddress.StreamHandle)

#define MD_EXTRACT_MDMSG_FROM_STRMMSG(msg) ((PMDRV_MSG)(msg->b_rptr))

#define MD_GET_MDMSG_PAYLOAD(msg) ((PMDRV_MSG)msg+1)

#define MD_MAP_BIND_TO_OPEN_CONTEXT(BindHandle) \
	(MsdControlBlock->pbind_block_list[BindHandle]->popen_block)

#define MD_MAP_BINDBLOCK_TO_OPEN_CONTEXT(BindBlock) \
	((BindBlock)->popen_block)

#define MD_MAP_BIND_TO_USER_CONTEXT(BindHandle) \
	(MsdControlBlock->pbind_block_list[BindHandle]->UserContext)

#define MD_GET_MDMSG_CLASS(msg) (msg->MessageClass)

#define MD_SET_MDMSG_CLASS(msg,class) (msg->MessageClass = class)

#define MD_GET_MDMSG_SUBCLASS1(msg) (msg->MessageId & MD_SUBCLASS1_MASK)

#define MD_GET_MDMSG_SUBCLASS2(msg) (msg->MessageId & MD_SUBCLASS2_MASK)

#define MD_GET_MDMSG_ID(msg) (msg->MessageId)

#define MD_SET_MDMSG_ID(msg,id) (msg->MessageId = id)

#define MD_SET_MDMSG_USER_CONTEXT(msg,Context) \
		(msg->MessageAddress.Bind.UserContext = Context)

#define MD_GET_NEXT_SEQUENCE_FOR_STREAM(AdapterBlock,StreamId,Sequence) { \
		MSD_ASSERT(AdapterBlock->StreamIdMapTable[StreamId]); \
		Sequence=AdapterBlock->StreamIdMapTable[StreamId]->BlockSequence++; \
	}

#define MD_IS_MSG_ASYNC(msg) \
		(((PMDRV_MSG)(msg->b_rptr))->MessageFlags & MD_MSG_FLAG_ASYNC)
		
#define MD_IS_MSG_SYNC(msg) \
		(((PMDRV_MSG)(msg->b_rptr))->MessageFlags & MD_MSG_FLAG_SYNC)
		
#define MD_IS_MSG_NO_ACK(msg) \
		(((PMDRV_MSG)(msg->b_rptr))->MessageFlags & MD_MSG_FLAG_NO_ACK)
		
#define MD_IS_MSG_NACK(msg) \
		(((PMDRV_MSG)(msg->b_rptr))->MessageFlags & MD_MSG_FLAG_NACK)
		
#define MD_SET_MSG_NACK(msg) \
		(((PMDRV_MSG)(msg->b_rptr))->MessageFlags |= MD_MSG_FLAG_NACK)
		

#define MD_SET_MSG_B_CONT(msg,ptr)  (msg->b_cont = ptr)

#define MD_GET_MSG_B_CONT(msg)  (msg->b_cont)

#define MD_GET_MSG_READ_PTR(msg) (msg->b_rptr)

#define MD_SET_MSG_READ_PTR(msg,ptr) (msg->b_rptr = (ptr))

#define MD_GET_MSG_WRITE_PTR(msg) (msg->b_wptr)

#define MD_SET_MSG_WRITE_PTR(msg,ptr) (msg->b_wptr = (ptr))

#define MD_GET_MSG_DATABUF_SIZE(msg) \
	(msg->b_datap->db_lim - msg->b_datap->db_base)

#define MD_GET_MSG_DATABUF_LIMIT(msg) \
	(msg->b_datap->db_lim) 

#define MD_SET_MSG_TYPE(msg,type) \
	msg->b_datap->db_type = type

#define MD_GET_MSG_TYPE(msg) \
	(msg->b_datap->db_type)

#define MD_GET_MSG_SIZE(msg) \
	(msg->b_wptr - msg->b_rptr)

#define MSD_DRIVER_STATE_CHECK(state) { \
	if (!(MsdControlBlock->MsdState & state)) { \
		ErrorCode = CD_ERR_BAD_STATE; \
		ReturnValue = MsdControlBlock->MsdState; \
		goto out; \
	} \
}

// Define Driver State Checking and Return Message Size checking micros

#define MSD_RETURN_MSG_SIZE_CHECK(MsgSize, type, where) { \
	if(MsgSize < sizeof(type) || MsgSize < sizeof(MD_ACK)) { \
		MSD_ERR_DBGPRINT("mercd:%s:Return msg too small  (%d < %d, %d).", \
				where, MsgSize, sizeof(type), sizeof(MD_ACK)); \
		ErrorCode = CD_ERR_BAD_MSG_SIZE; \
		ReturnValue = MsgSize; \
		goto out; \
	} \
}

// Macro defination for cheching the SRAM Address
#ifdef LFS
#define CHECK_SRAM_ADDRESS(addr, cPtr, padatpter ) { \
	if ( ((pmerc_uchar_t)(long)addr < (padapter->phost_info->reg_block.HostRamStart)) || \
	     ((pmerc_uchar_t)(long)addr > (padapter->phost_info->reg_block.ClrMsgReadyIntr)) )\
	{ \
		MSD_ERR_DBGPRINT("================================================\n"); \
		MSD_ERR_DBGPRINT("Mercury Kernel Shared RAM Corrupted!\n"); \
		MSD_ERR_DBGPRINT("%s\n", cPtr); \
		MSD_ERR_DBGPRINT("Marking Board number %d INOPERABLE\n", \
				padapter->phw_info->slot_number); \
		MSD_ERR_DBGPRINT("=================================================\n"); \
		padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE; \
		return(MD_FAILURE); \
	} \
}
#else
#define CHECK_SRAM_ADDRESS(addr, cPtr, padatpter ) { \
	if ( ((pmerc_uchar_t)addr < (padapter->phost_info->reg_block.HostRamStart)) || \
	     ((pmerc_uchar_t)addr > (padapter->phost_info->reg_block.ClrMsgReadyIntr)) )\
	{ \
		MSD_ERR_DBGPRINT("================================================\n"); \
		MSD_ERR_DBGPRINT("Mercury Kernel Shared RAM Corrupted!\n"); \
		MSD_ERR_DBGPRINT("%s\n", cPtr); \
		MSD_ERR_DBGPRINT("Marking Board number %d INOPERABLE\n", \
				padapter->phw_info->slot_number); \
		MSD_ERR_DBGPRINT("=================================================\n"); \
		padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE; \
		return(MD_FAILURE); \
	} \
}
#endif
 
#define CHANGE_ADAPTER_STATE(x){ \
		MSD_ERR_DBGPRINT("AdapterState Chaged from %x\n",AdapterBlock->state); \
		AdapterBlock->state = x; \
		MSD_ERR_DBGPRINT("Changed to %x\n",AdapterBlock->state); \
	}


#if defined( MERCD_LINUX )
#	define	MSD_SYSTEM_TICK jiffies
#else 
#	define	MSD_SYSTEM_TICK lbolt
#endif

#endif
