/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdwwinitmngr.c
 * Description                  : WW Driver Manager init functions
 *
 *
 **********************************************************************/

#include "msd.h"
#include "msdpciif.h"
#define _MSDWWMGR_C_
#include "msdextern.h"
#undef _MSDWWMGR_C_



/***************************************************************************
 * Function Name                : mid_wwmgr_set_param
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : none
 * Calling functions            : MID
 * Description                  :
 * Additional comments          : If it is a DM3 WW board, then after
 *                                storing the WW params we just return
 *                                If it is a 3rd Rock Family Board, we
 *                                swith the driver to WW mode and return
 *                                after the INIT complete interrupt.
 ****************************************************************************/
void mid_wwmgr_set_param(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{

  merc_uint_t                            MsgSize;
  PWW_SET_PARAM                          SetParamPtr;
  MD_ACK                                 *AckPtr;
  MD_STATUS                              Status;
  pmercd_adapter_block_sT                padapter;
  REGISTER PMDRV_MSG                     MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  merc_uint_t                            ErrorCode = MD_OK;
  merc_uint_t                            ReturnValue=0;
  merc_uint_t                            BrdNumber;
  merc_uint_t                            SlotNumber;


  MSD_FUNCTION_TRACE("mid_wwmgr_set_param", TWO_PARAMETERS,
                               (merc_ulong_t)MsdOpenBlock, (merc_ulong_t)(Msg));
  Status = MD_SUCCESS;

  MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

  // perform validity checking
  MSD_RETURN_MSG_SIZE_CHECK(MsgSize, WW_SET_PARAM, "SetParam");


  MdMsg->MessageId = MID_SET_WW_PARAM_ACK;
  SetParamPtr = (PWW_SET_PARAM)MD_GET_MDMSG_PAYLOAD(MdMsg);
  AckPtr = (MD_ACK  *)SetParamPtr;

#ifndef MERCD_LINUX
  //In Solaris, the driver returned slot_number is taken as
  //QBoard.boardConfig.boardNumber by MDI library.
  SlotNumber = BrdNumber = SetParamPtr->BoardNumber;
#else
  //In Linux, QBoard.boardConfig.boardNumber is assigned to
  //the value of the Board[] field of pyramid.scd
  BrdNumber = SetParamPtr->BoardNumber;
#endif


  // perform validity checking
  // Check the size of  xxptr

  if (BrdNumber  >= MD_MAX_ADAPTER_COUNT) {
      printk("SetParam: Board number (%d) is > MAX (%d).\n",
                      SetParamPtr->BoardNumber, MD_MAX_ADAPTER_COUNT);
      ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
      Status = MD_FAILURE;
      Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
      if (MD_IS_MSG_ASYNC(Msg))
          supp_process_receive(MsdOpenBlock, Msg);
      else
          goto out;
      return;
  }


#ifndef MERCD_LINUX
  padapter = MsdControlBlock->padapter_block_list[SlotNumber];
#else
  SlotNumber = mercd_adapter_log_to_phy_map_table[BrdNumber];
  padapter = MsdControlBlock->padapter_block_list[SlotNumber];
#endif

  SetParamPtr++;

  if (padapter == NULL) {
      printk("mid_wwmgr_set_param: Invalid Adapter Number!\n");
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      Status = MD_FAILURE;
      Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
      if (MD_IS_MSG_ASYNC(Msg))
          supp_process_receive(MsdOpenBlock, Msg);
      else
          goto out;
      return;
  }

  if ((padapter->state == MERCD_ADAPTER_STATE_SUSPENDED) || 
      (padapter->state == MERCD_ADAPTER_STATE_MAPPED)) {
      printk("mid_wwmgr_set_param: Invalid Adapter State!\n");
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      Status = MD_FAILURE;
      Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
      if (MD_IS_MSG_ASYNC(Msg))
          supp_process_receive(MsdOpenBlock, Msg);
      else
          goto out;
      return;
  }

  //1. Set the padapter->flags.WWFlags
  padapter->flags.WWFlags = MERCD_ADAPTER_WW_MODE_NOTENABLED;

  //Allocate Memory for WW Device Extension
  if (!( padapter->pww_info )) {
      padapter->pww_info = (pmercd_ww_dev_info_sT)mercd_allocator(MERCD_WW_DEV_INFO);
    
      if (padapter->pww_info == NULL) {
	  printk("mid_wwmgr_set_param: memory allocation failed\n");
          ErrorCode=ENOMEM;
          Status = MD_FAILURE;
          Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
          if (MD_IS_MSG_ASYNC(Msg))
              supp_process_receive(MsdOpenBlock, Msg);
          else
              goto out;
      }

      MSD_ZERO_MEMORY(padapter->pww_info, MERCD_WW_DEV_INFO);
      //Store a Back Ref to padapter in pww_info
      padapter->pww_info->padapter = (pmerc_void_t)padapter;
      //Set WW device state
      padapter->pww_info->state = MERCD_ADAPTER_WW_NOTREADY;
      padapter->pww_info->pww_eos_msgq = NULL;
  }

  //For DTI, the default is WW. So after a board shutdown one should
  //able to configure the driver back to WW mode during set_param
  if (padapter->pww_info->pww_param != NULL) {
      pmercd_ww_param_sT pTmpParam;
      pTmpParam = padapter->pww_info->pww_param;
      MSD_LEVEL2_DBGPRINT("set_param: WW parameters already allocd! Over writing \n");

      MSD_ZERO_MEMORY(padapter->pww_info, MERCD_WW_DEV_INFO);
      //Store a Back Ref to padapter in pww_info
      padapter->pww_info->padapter = (pmerc_void_t)padapter;
      //Set WW device state
      padapter->pww_info->state = MERCD_ADAPTER_WW_IN_SHUTDOWN;
      padapter->pww_info->pww_param = pTmpParam;

      MSD_ZERO_MEMORY((pmerc_uchar_t)(padapter->pww_info->pww_param), MERCD_WW_PARAM);
      MsdCopyMemory((pmerc_char_t)SetParamPtr, 
                    (pmerc_char_t)padapter->pww_info->pww_param, MERCD_WW_PARAM);

      if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
          MSD_ZERO_MEMORY((pmerc_uchar_t)(AckPtr), sizeof(MD_ACK));
          //We need to send a MID_SET_WW_PARAM_ACK back
          AckPtr->ErrorCode = MD_OK;
          AckPtr->MessageId = MdMsg->MessageId; /* MID_SET_WW_PARAM_ACK; */
          MD_SET_MSG_WRITE_PTR(Msg,MD_GET_MSG_READ_PTR(Msg)+
                           sizeof(MDRV_MSG)+sizeof(MD_ACK));
          //return the message to user via sync path
          goto out;
      }
  } else {
      //pww_param is not Allocated Yet.  Start Allocating the Structure
      padapter->pww_info->pww_param =
                   (pmercd_ww_param_sT)mercd_allocator(MERCD_WW_PARAM);

      MSD_ZERO_MEMORY((pmerc_uchar_t)(padapter->pww_info->pww_param), MERCD_WW_PARAM);

      //Check if pww_param Alloc failed
      if (padapter->pww_info->pww_param == NULL) {
          MSD_ERR_DBGPRINT("mid_wwmgr_set_param: Adapter WW param alloc failed!\n");
          ErrorCode=ENOMEM;
          Status = MD_FAILURE;
          Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
          if (MD_IS_MSG_ASYNC(Msg))
              supp_process_receive(MsdOpenBlock, Msg);
          else
              goto out;
          return; 
      }

      //Do the bcopy to pww_param
      MsdCopyMemory((pmerc_char_t)SetParamPtr,
                   (pmerc_char_t)padapter->pww_info->pww_param, MERCD_WW_PARAM);
  }

  //We send the MID_SET_WW_PARAM_ACK back if it not a THIRD_ROCK
  //board. If it is a THIRD_ROCK board, we will send a set_wwmode
  //immediately. Check the FAMILY type.
  if (padapter->phw_info->boardFamilyType == THIRD_ROCK_FAMILY) {
      //Before we configure the board to WW, check if bootkernel is up and running
      MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
      Status = mid_wwmgr_check_3rdrock_running(padapter, MERCD_WW_BOOT_KERNEL);
      MSD_ENTER_CONTROL_BLOCK_MUTEX_BH();
       
      if (Status == MD_SUCCESS) {
          //printk("WWSetParam: Found the Sequence..switching to WW\n");
          mid_wwmgr_configure_brd_to_ww(MsdOpenBlock, padapter, Msg);
	  return;
      } else {
          if (Status == MERCD_WW_POST_IN_PROGRESS) {
              //printk("3rd Rock RTK: post in progress state\n");
              ErrorCode = CD_ERR_WW_RTK_POST_IN_PROGRESS;
          } else {
              //printk("3rd Rock RTK: post read error\n");
              ErrorCode = CD_ERR_WW_RTK_POST_LOCATION_READ;
          }
          Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
          if (MD_IS_MSG_ASYNC(Msg))
              supp_process_receive(MsdOpenBlock, Msg);
          else
              goto out;
	  return;
      }
  } else {
      // This is a SRAM_FAMILY Board. Send up MID_SET_WW_PARAM_ACK back
      AckPtr->ErrorCode = MD_OK;
      AckPtr->MessageId = MdMsg->MessageId;   /* MID_SET_WW_PARAM_ACK; */
      MD_SET_MSG_WRITE_PTR(Msg,MD_GET_MSG_READ_PTR(Msg)+
                             sizeof(MDRV_MSG)+sizeof(MD_ACK));
      // return the message to user via sync path
      goto out;
  }


out:

#if defined LiS || defined LIS || defined LFS
  supp_process_sync_receive(MsdOpenBlock, Msg);
#else
  {
  merc_uint_t size = 0;
  pmerc_uchar_t savedbuffer;
  size = MD_GET_MSG_WRITE_PTR(Msg) - MD_GET_MSG_READ_PTR(Msg);
  savedbuffer = mercd_allocator(size);

  //How are we going to address this?? We are allocating
  //to send up an error return. But we fail the alloc.
  if (savedbuffer == NULL) {
      printk("SetParam: Board number (%d) Mem Allocation Fail\n", SetParamPtr->BoardNumber);
      return;
  }
  
  MSD_ZERO_MEMORY(savedbuffer, size);
  MsdCopyMemory(Msg->b_rptr, savedbuffer, size);
  Msg->b_datap->db_base = savedbuffer;
  Msg->b_datap->db_lim = (char *)savedbuffer + size;
  Msg->b_rptr = (char *)savedbuffer;
  Msg->b_wptr = (char *)savedbuffer+size;
  }
  supp_process_receive(MsdOpenBlock, Msg);
#endif /* LiS */
  return;
}


/***************************************************************************
 * Function Name                : mid_wwmgr_get_param
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : none
 * Calling functions            : MID
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void mid_wwmgr_get_param(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
  merc_uint_t                      MsgSize;
  PWW_GET_PARAM                    GetParamPtr;
  MD_ACK*                          AckPtr;
  MD_STATUS                        Status;
  pmercd_adapter_block_sT          padapter;
  REGISTER PMDRV_MSG               MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  merc_uint_t                      ErrorCode = MD_OK;
  pmerc_uchar_t                    DataPtr;
  merc_uint_t                      ReturnValue = 0;
  merc_uint_t                      BrdNumber;
  merc_uint_t                      SlotNumber;


  Status = MD_SUCCESS;

  MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

  // perform validity checking
  MSD_RETURN_MSG_SIZE_CHECK(MsgSize, WW_GET_PARAM, "GetParam");

  GetParamPtr = (PWW_GET_PARAM)MD_GET_MDMSG_PAYLOAD(MdMsg);
  MdMsg->MessageId = MID_GET_WW_PARAM_ACK;
  AckPtr = (MD_ACK*)GetParamPtr;
  DataPtr = (pmerc_uchar_t)(AckPtr+1);

#ifndef MERCD_LINUX
  //In Solaris, the driver returned slot_number is taken as
  //QBoard.boardConfig.boardNumber by MDI library.
  SlotNumber = BrdNumber = GetParamPtr->BoardNumber;
#else
  //In Linux, QBoard.boardConfig.boardNumber is assigned to
  //the value of the Board[] field of pyramid.scd
  BrdNumber = GetParamPtr->BoardNumber;
#endif

  // perform validity checking
  if (GetParamPtr->BoardNumber >= MD_MAX_ADAPTER_COUNT) {
      printk("GetParam: Board number (%d) is > MAX (%d).\n",
              GetParamPtr->BoardNumber, MD_MAX_ADAPTER_COUNT);
      AckPtr->ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
      Status = MD_FAILURE;
      goto out;
  }

#ifndef MERCD_LINUX
  padapter = MsdControlBlock->padapter_block_list[SlotNumber];
#else
  SlotNumber = mercd_adapter_log_to_phy_map_table[BrdNumber];
  padapter = MsdControlBlock->padapter_block_list[SlotNumber];
#endif

  if (padapter == NULL) {
      printk("mid_wwmgr_get_param: Invalid Adapter Number!\n");
      AckPtr->ErrorCode = CD_ERR_ADAPTER_INVALID;
      Status = MD_FAILURE;
      goto out;
  }

  if ((padapter->state == MERCD_ADAPTER_STATE_SUSPENDED) || 
      (padapter->state == MERCD_ADAPTER_STATE_MAPPED)) {
      printk("mid_wwmgr_get_param: Invalid Adapter State!\n");
      AckPtr->ErrorCode = CD_ERR_ADAPTER_INVALID;
      Status = MD_FAILURE;
      goto out;
  }

  // Check if the structure is allocated .....
  if (padapter->pww_info->pww_param == NULL) {
      MSD_LEVEL2_DBGPRINT("mid_wwmgr_set_param: Adapater WW param already allocd!\n");
      AckPtr->ErrorCode = PAM_ERR_WW_PAM_ERR_INVALID_PARAM;
      Status = MD_FAILURE;
      goto out;
  }

  MsdCopyMemory((pmerc_char_t)padapter->pww_info->pww_param, 
                     (pmerc_char_t)DataPtr, MERCD_WW_PARAM);

  // We need to send a MID_GET_WW_PARAM_ACK back
  MSD_LEVEL2_DBGPRINT("get param:  MdMsg->MessageId: %x\n",  MdMsg->MessageId);
  AckPtr->MessageId = MdMsg->MessageId; 
  AckPtr->ErrorCode = MD_OK;

  // return the message to user via sync path
  MD_SET_MSG_WRITE_PTR(Msg,MD_GET_MSG_READ_PTR(Msg)+sizeof(MDRV_MSG)
				    +sizeof(MD_ACK)+MERCD_WW_PARAM);
  supp_process_sync_receive(MsdOpenBlock, Msg);
  return;

out:
  // send back an error/reply message
  Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
  if (MD_IS_MSG_ASYNC(Msg))
      supp_process_receive(MsdOpenBlock, Msg);
  else
      supp_process_sync_receive(MsdOpenBlock, Msg);

  return;
}


/***************************************************************************
 * Function Name                : mid_wwmgr_get_mode
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : none
 * Calling functions            : MID
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void mid_wwmgr_get_mode(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
  merc_uint_t              MsgSize;
  PWW_GET_MODE             GetModePtr;
  MD_ACK*                  AckPtr;
  MD_STATUS                Status;
  pmercd_adapter_block_sT  padapter;

  REGISTER PMDRV_MSG       MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  merc_uint_t              ErrorCode = MD_OK;
  pmerc_uchar_t            DataPtr;
  merc_uint_t              ReturnValue=0;
  merc_uint_t              BrdNumber;
  merc_uint_t              SlotNumber;


  Status = MD_SUCCESS;
  MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

  // perform validity checking
  MSD_RETURN_MSG_SIZE_CHECK(MsgSize, WW_GET_MODE, "GetMode");

  GetModePtr = (PWW_GET_MODE)MD_GET_MDMSG_PAYLOAD(MdMsg);
  MdMsg->MessageId = MID_GET_WW_MODE_ACK;
  AckPtr = (MD_ACK *)GetModePtr;
  DataPtr = (pmerc_uchar_t)(AckPtr+1);

#ifndef MERCD_LINUX
  //In Solaris, the driver returned slot_number is taken as
  //QBoard.boardConfig.boardNumber by MDI library.
  SlotNumber = BrdNumber = GetModePtr->BoardNumber;
#else
  //In Linux, QBoard.boardConfig.boardNumber is assigned to
  //the value of the Board[] field of pyramid.scd
  BrdNumber = GetModePtr->BoardNumber;
#endif

  // perform validity checking
  // Check the size of
  if (GetModePtr->BoardNumber >= MD_MAX_ADAPTER_COUNT) {
      printk("GetMode: Board number (%d) is > MAX (%d).\n",
              GetModePtr->BoardNumber, MD_MAX_ADAPTER_COUNT);
      ErrorCode = CD_ERR_BAD_BOARD_NUMBER;
      Status = MD_FAILURE;
      goto out;
  }

#ifndef MERCD_LINUX
  padapter = MsdControlBlock->padapter_block_list[SlotNumber];
#else
  SlotNumber = mercd_adapter_log_to_phy_map_table[BrdNumber];
  padapter = MsdControlBlock->padapter_block_list[SlotNumber];
#endif

  if (padapter == NULL) {
      printk("mid_wwmgr_get_mode: Invalid Adapter Number!\n");
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      Status = MD_FAILURE;
      goto out;
  }

  if ((padapter->state == MERCD_ADAPTER_STATE_SUSPENDED) || 
      (padapter->state == MERCD_ADAPTER_STATE_MAPPED)) {
      printk("mid_wwmgr_get_mode: Invalid Adapter State!\n");
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      Status = MD_FAILURE;
      goto out;
  }

  bzero((pmerc_char_t)DataPtr, sizeof(merc_uint_t));
  MsdCopyMemory((pmerc_char_t)&padapter->flags.WWFlags, 
	   (pmerc_char_t)DataPtr, sizeof(merc_uint_t));

  // We need to send a MID_GET_WW_MODE_ACK back
  AckPtr->ErrorCode = MD_OK;
  AckPtr->MessageId = MID_GET_WW_MODE_ACK;
  // return the message to user via sync path
  MD_SET_MSG_WRITE_PTR(Msg,MD_GET_MSG_READ_PTR(Msg)+sizeof(MDRV_MSG)
		             +sizeof(MD_ACK) + sizeof(merc_uint_t));
  supp_process_sync_receive(MsdOpenBlock, Msg);
  return;

out:
  // send back an error/reply message
  Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

  if (MD_IS_MSG_ASYNC(Msg))
      supp_process_receive(MsdOpenBlock, Msg);
  else
      supp_process_sync_receive(MsdOpenBlock, Msg);

  return;
}


/***************************************************************************
 * Function Name                : mid_wwmgr_get_mode_operational
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : none
 * Calling functions            : MID
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void mid_wwmgr_get_mode_operational(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
  merc_uint_t              MsgSize;
  PWW_GET_MODE             GetModePtr;
  MD_ACK*                  AckPtr;
  MD_STATUS                Status;
  pmercd_adapter_block_sT  padapter;

  REGISTER PMDRV_MSG       MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  merc_uint_t              ErrorCode = MD_OK;
  merc_uint_t              ReturnValue=0;
  merc_uint_t              BrdNumber;
  merc_uint_t              SlotNumber;

  Status = MD_SUCCESS;

  MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

  // perform validity checking
  MSD_RETURN_MSG_SIZE_CHECK(MsgSize, WW_GET_MODE, "GetModeOperational");

  GetModePtr = (PWW_GET_MODE)MD_GET_MDMSG_PAYLOAD(MdMsg);
  MdMsg->MessageId = MID_GET_WW_MODE_OPERATIONAL_ACK;
  AckPtr = (MD_ACK *)GetModePtr;

  if ( GetModePtr == NULL ) {
      printk("GetModeOperational: GetModePtr is Null\n");
      ErrorCode = PAM_ERR_WW_PAM_ERR_INVALID_PARAM;
      Status = MD_FAILURE;
      goto out;
  }

#ifndef MERCD_LINUX
  //In Solaris, the driver returned slot_number is taken as
  //QBoard.boardConfig.boardNumber by MDI library.
  SlotNumber = BrdNumber = GetModePtr->BoardNumber;
#else
  //In Linux, QBoard.boardConfig.boardNumber is assigned to
  //the value of the Board[] field of pyramid.scd
  BrdNumber = GetModePtr->BoardNumber;
#endif

  // perform validity checking
  // Check the size of
  if ((GetModePtr->BoardNumber >= MD_MAX_ADAPTER_COUNT) || (GetModePtr->BoardNumber < 0)) {
      printk("GetModeOperational: Board number (%d) is > MAX (%d).\n",
                       GetModePtr->BoardNumber, MD_MAX_ADAPTER_COUNT);
      ErrorCode = PAM_ERR_WW_PAM_ERR_INVALID_PARAM;
      Status = MD_FAILURE;
      goto out;
  }

#ifndef MERCD_LINUX
  padapter = MsdControlBlock->padapter_block_list[SlotNumber];
#else
  SlotNumber = mercd_adapter_log_to_phy_map_table[BrdNumber];
  if ((SlotNumber < 0) || (SlotNumber > MD_MAX_CONFIG_ID_COUNT)) {
      printk("mid_wwmgr_get_mode_operational: Invalid ConfigId %d\n", SlotNumber);
      ErrorCode = PAM_ERR_WW_PAM_ERR_INVALID_PARAM;
      Status = MD_FAILURE;
      goto out;
  }
  padapter = MsdControlBlock->padapter_block_list[SlotNumber];
#endif

  if (padapter == NULL) {
      printk("mid_wwmgr_get_mode_operational: Invalid Adapter Number!\n");
      ErrorCode = PAM_ERR_WW_PAM_ERR_INVALID_PARAM;
      Status = MD_FAILURE;
      goto out;
  }

  if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
      (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) {
      ErrorCode = PAM_ERR_WW_PAM_ERR_INVALID_PARAM;
      Status = MD_FAILURE;
      goto out;
  }

  // We need to send a MID_GET_WW_MODE_ACK back
  AckPtr->ErrorCode = MD_OK;
  AckPtr->MessageId = MID_GET_WW_MODE_OPERATIONAL_ACK;
  // return the message to user via sync path
  MD_SET_MSG_WRITE_PTR(Msg,MD_GET_MSG_READ_PTR(Msg)+sizeof(MDRV_MSG)
                             +sizeof(MD_ACK) + sizeof(merc_uint_t));
  supp_process_sync_receive(MsdOpenBlock, Msg);
  return;

out:
  // send back an error/reply message
  Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);

  if (MD_IS_MSG_ASYNC(Msg))
      supp_process_receive(MsdOpenBlock, Msg);
  else
      supp_process_sync_receive(MsdOpenBlock, Msg);
  return;
}


/***************************************************************************
 * Function Name                : mid_wwmgr_set_mode
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : none
 * Calling functions            : MID
 * Description                  :
 * Additional comments          : This calls the respective set_mode functions
 *                              : depending the Board Family
 *
 ****************************************************************************/
void mid_wwmgr_set_mode(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
  REGISTER PMDRV_MSG        MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  merc_uint_t               ErrorCode;
  merc_uint_t               ReturnValue=0;
  PWW_SET_MODE              SetModePtr;
  pmercd_adapter_block_sT   padapter;
  merc_uint_t               MsgSize;
  merc_uint_t               BrdNumber;
  merc_uint_t               SlotNumber;


  SetModePtr = (PWW_SET_MODE)MD_GET_MDMSG_PAYLOAD(MdMsg);

#ifndef MERCD_LINUX
  //In Solaris, the driver returned slot_number is taken as
  //QBoard.boardConfig.boardNumber by MDI library.
  SlotNumber = BrdNumber = SetModePtr->BoardNumber;
#else
  //In Linux, QBoard.boardConfig.boardNumber is assigned to
  //the value of the Board[] field of pyramid.scd
  BrdNumber = SetModePtr->BoardNumber;
#endif

  MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);
  //MSD_RETURN_MSG_SIZE_CHECK(MsgSize, WW_SET_MODE, "SetMode");

  // perform validity checking
  // Check the size of
  if ((BrdNumber = SetModePtr->BoardNumber) >= MD_MAX_ADAPTER_COUNT) {
      printk("SetMode: Board number (%d) is > MAX (%d).\n",
            SetModePtr->BoardNumber, MD_MAX_ADAPTER_COUNT);
      ErrorCode = PAM_ERR_WW_PAM_ERR_INVALID_PARAM;
      Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
      if (MD_IS_MSG_ASYNC(Msg))
          supp_process_receive(MsdOpenBlock, Msg);
      else
          supp_process_sync_receive(MsdOpenBlock, Msg);
      return;
  }

#ifndef MERCD_LINUX
  padapter = MsdControlBlock->padapter_block_list[SlotNumber];
#else
  SlotNumber = mercd_adapter_log_to_phy_map_table[BrdNumber];
  padapter = MsdControlBlock->padapter_block_list[SlotNumber];
#endif

  if (padapter == NULL) {
      printk("mid_wwmgr_set_mode: Invalid Adapter Number!\n");
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
      if (MD_IS_MSG_ASYNC(Msg))
          supp_process_receive(MsdOpenBlock, Msg);
      else
          supp_process_sync_receive(MsdOpenBlock, Msg);
      return;
  }

  if ((padapter->state == MERCD_ADAPTER_STATE_SUSPENDED) || 
      (padapter->state == MERCD_ADAPTER_STATE_MAPPED)) {
      printk("mid_wwmgr_set_mode: Invalid Adapter State!\n");
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
      if (MD_IS_MSG_ASYNC(Msg))
          supp_process_receive(MsdOpenBlock, Msg);
      else
          supp_process_sync_receive(MsdOpenBlock, Msg);
      return;
  }

  if (padapter->pww_info->pww_param == NULL) {
      printk("mid_wwmgr_set_mode: Invalid Adapter Number!\n");
      ErrorCode = PAM_ERR_WW_PAM_ERR_NO_WW_PARM;
      Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
      if (MD_IS_MSG_ASYNC(Msg))
          supp_process_receive(MsdOpenBlock, Msg);
      else
          supp_process_sync_receive(MsdOpenBlock, Msg);
      return;
  }

  //Check the boardFamilyType, if it is valid
  if (padapter->phw_info->boardFamilyType == INVALID_BOARD_FAMILY) {
      cmn_err(CE_WARN, "set_mode: INVALID_BOARD_FAMILY\n");
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
      if (MD_IS_MSG_ASYNC(Msg))
          supp_process_receive(MsdOpenBlock, Msg);
      else
          supp_process_sync_receive(MsdOpenBlock, Msg);
      return;
  } /*INVALID_BOARD_FAMILY*/

#ifdef DOWNLOADER_CHANGE
  //WWPENDING: We had to ifdef this check while doing the admin integration.
  if (padapter->state != MERCD_ADAPTER_STATE_DOWNLOADED) {
      cmn_err(CE_NOTE, "Adapter is NOT in DOWNLOADED State\n");
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
      if(MD_IS_MSG_ASYNC(Msg))
        supp_process_receive(MsdOpenBlock, Msg);
      else
        supp_process_sync_receive(MsdOpenBlock, Msg);
     return;
  }
#endif

  if (padapter->phw_info->boardFamilyType == THIRD_ROCK_FAMILY) {
      mid_wwmgr_set_mode_3rdrock_family(MsdOpenBlock, padapter, Msg);
  } else if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
       	     (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) {
      /* SRAM FAMILY - Non WW */
      mid_wwmgr_set_mode_sram_family(MsdOpenBlock, padapter, Msg);
  } else {
      // DMV-B
      merc_uint_t                 ErrorCode = MD_OK;
      ErrorCode = MD_OK;
      padapter->rtkMode = 1;
      MdMsg->MessageId = MID_SET_WW_MODE_ACK;
      
      if (padapter->rtkMode == 0)
          ReturnValue = 0x1; //8BIT RTK
      else
          ReturnValue = 0x2; //16BIT RTK

      Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
      if (MD_IS_MSG_ASYNC(Msg))
          supp_process_receive(MsdOpenBlock, Msg);
      else {
           merc_uint_t size = 0;
           pmerc_uchar_t savedbuffer;
           size = MD_GET_MSG_WRITE_PTR(Msg) - MD_GET_MSG_READ_PTR(Msg);
           savedbuffer = mercd_allocator(size);
           if (savedbuffer == NULL) {
               printk("ERROR SetModeSramFamily: Alloc Buffer for savedbuffer FAILED\n");
               return;
           }
          
           MSD_ZERO_MEMORY(savedbuffer, size);
           MsdCopyMemory(Msg->b_rptr, savedbuffer, size);
           Msg->b_datap->db_base = savedbuffer;
           Msg->b_datap->db_lim = (char *)savedbuffer + size;
           Msg->b_rptr = (char *)savedbuffer;
           Msg->b_wptr = (char *)savedbuffer+size;
      }
      supp_process_receive(MsdOpenBlock, Msg);
  }
}


/***************************************************************************
 * Function Name                : mid_wwmgr_set_mode_sram_family
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : none
 * Calling functions            : MID
 * Description                  :
 * Additional comments          : Set Mode for SRAM Family Boards
 ****************************************************************************/
void mid_wwmgr_set_mode_sram_family(PMSD_OPEN_BLOCK MsdOpenBlock,
                              pmercd_adapter_block_sT padapter, PSTRM_MSG Msg)
{
  PWW_SET_MODE                   SetModePtr;
  MD_STATUS                      Status;
  REGISTER PMDRV_MSG             MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  merc_uint_t                    ErrorCode = MD_OK;
  merc_uint_t                    ReturnValue=0;
#ifndef LFS
  mercd_dhal_ww_plxmap_sT        plxmapinfo = { 0 };
#endif
  pmercd_ww_dev_info_sT          pwwDev;

  
  Status = MD_SUCCESS;

  SetModePtr = (PWW_SET_MODE)MD_GET_MDMSG_PAYLOAD(MdMsg);
  MdMsg->MessageId = MID_SET_WW_MODE_ACK;

  pwwDev = padapter->pww_info;

  //Read the post-location to determine if WW mode is supported
  //For a THIRD_ROCK Family Boards, we dont need to read the post location.
  //Because for a 3rd rock family mid_wwmgr_configure_brd_to_ww is called 
  //immeidiately following qWWSetParam. By then RTK is not started yet. 
  //We check this location only for SRAM Family of Boards, where the 
  //admin issues qWWSetMode after RTK starts.

  // DMV-B
  if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
      (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) {
      Status =  mid_wwmgr_read_ww_postlocation(padapter);
  } else {
      padapter->flags.WWFlags =  MERCD_ADAPTER_WW_MODE_NOTENABLED;
      Status = MD_SUCCESS;
  }

  //For a SRAM Family Board,not supporting WW mode (eg: 1.8 hardware)
  //we return from the following error conditions since we would not
  //have found the WW sequence in the respective diagnostic locations.
  if (Status != MD_SUCCESS) {
      MSD_LEVEL2_DBGPRINT("mid_wwmgr_configure_drvr_to_ww: postloc read failure!\n");
      ErrorCode = CD_ERR_WW_RTK_POST_LOCATION_READ;

      if (padapter->rtkMode == 0)
          ReturnValue = 0x1; //8BIT RTK
      else
          ReturnValue = 0x2; //16BIT RTK

      Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
      if (MD_IS_MSG_ASYNC(Msg))
          supp_process_receive(MsdOpenBlock, Msg);
      else {
#if defined LiS || defined LIS || defined LFS
          supp_process_sync_receive(MsdOpenBlock, Msg);
#else
          {
          merc_uint_t size = 0;
          pmerc_uchar_t savedbuffer;
          size = MD_GET_MSG_WRITE_PTR(Msg) - MD_GET_MSG_READ_PTR(Msg);
          savedbuffer = mercd_allocator(size);
          if (savedbuffer == NULL) {
              printk("ERROR SetModeSramFamily: Alloc Buffer for savedbuffer FAILED\n");
              return;
          }
           
          MSD_ZERO_MEMORY(savedbuffer, size);
          MsdCopyMemory(Msg->b_rptr, savedbuffer, size);
          Msg->b_datap->db_base = savedbuffer;
          Msg->b_datap->db_lim = (char *)savedbuffer + size;
          Msg->b_rptr = (char *)savedbuffer;
          Msg->b_wptr = (char *)savedbuffer+size;
          }
	  supp_process_receive(MsdOpenBlock, Msg);
#endif /* LiS */
      }
      return;
  }

  if (padapter->flags.WWFlags ==  MERCD_ADAPTER_WW_MODE_NOTENABLED) {
      MSD_LEVEL2_DBGPRINT("mid_wwmgr_configure_drvr_to_ww: WW mode Not Enabled!\n");
      Status = MD_FAILURE;
      ErrorCode = CD_ERR_WW_RTK_POST_LOCATION_READ;
      pwwDev->state = MERCD_ADAPTER_WW_FAILED;

      if (padapter->rtkMode == 0)
          ReturnValue = 0x1; //8BIT RTK
      else
          ReturnValue = 0x2; //16BIT RTK

      Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
      if (MD_IS_MSG_ASYNC(Msg))
          supp_process_receive(MsdOpenBlock, Msg);
      else {
#if defined LiS || defined LIS || defined LFS
          supp_process_sync_receive(MsdOpenBlock, Msg);
#else
          {
          merc_uint_t size = 0;
          pmerc_uchar_t savedbuffer;
          size = MD_GET_MSG_WRITE_PTR(Msg) - MD_GET_MSG_READ_PTR(Msg);
          savedbuffer = mercd_allocator(size);
          if (savedbuffer == NULL) {
              printk("ERROR SetModeSramFamily: memory allocation failed\n");
              return;
          }
          

          MSD_ZERO_MEMORY(savedbuffer, size);
          MsdCopyMemory(Msg->b_rptr, savedbuffer, size);
          Msg->b_datap->db_base = savedbuffer;
          Msg->b_datap->db_lim = (char *)savedbuffer + size;
          Msg->b_rptr = (char *)savedbuffer;
          Msg->b_wptr = (char *)savedbuffer+size;
          }
	  // DMV-B
          if ((padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DMVB) &&
              (padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_NEWB)) {
	      supp_process_receive(MsdOpenBlock, Msg);
          }
#endif /* LiS */
      }
         return;
  }

  //WW specific sequence found at the posted location.
  //Which means here you may be dealing with a 1.9 hardware
  //or a DTI board.

  mid_wwmgr_configure_brd_to_ww(MsdOpenBlock, padapter, Msg);
}


/***************************************************************************
 * Function Name                : mid_wwmgr_set_mode_3rdrock_family
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock, padapter, Msg
 *
 * Outputs                      : none
 * Calling functions            : MID
 * Description                  :
 * Additional comments          : Set Mode for 3rd Rock Family Boards
 ****************************************************************************/
void mid_wwmgr_set_mode_3rdrock_family(PMSD_OPEN_BLOCK MsdOpenBlock,
                    pmercd_adapter_block_sT padapter, PSTRM_MSG Msg)
{
  PWW_SET_MODE                   SetModePtr;
  MD_STATUS                      Status;
  REGISTER PMDRV_MSG             MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  merc_uint_t                    ErrorCode = MD_OK;
  merc_uint_t                    ReturnValue=0;
  MD_ACK*                        AckPtr;
  merc_uint_t                    szBMap;

  
  Status = MD_SUCCESS;

  SetModePtr = (PWW_SET_MODE)MD_GET_MDMSG_PAYLOAD(MdMsg);
  MdMsg->MessageId = MID_SET_WW_MODE_ACK;

  //Now we will go ahead and read the post location contents
  //for a 3rd rock family boards
  //printk("mid_wwmgr_set_mode_3rdrock_family...\n");
  MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
  Status = mid_wwmgr_check_3rdrock_running(padapter, MERCD_WW_RUNTIME_KERNEL);
  MSD_ENTER_CONTROL_BLOCK_MUTEX_BH();

  if (Status == MD_SUCCESS) {
      //Since RTK is running send the BIG Msg Block MF.
      //Clear the RCV BIG Msg bitmap.
      //By now, boot kernel is out of the picture. So dont
      //bother about using the bitmap utility routines. Just
      //do a bzero and it will serve the purpose.
      szBMap = ((padapter->pww_info->pww_param->numberBigMsgBlocks)
                                      / (8 * sizeof(merc_uint_t)));
      if ((padapter->pww_info->pww_param->numberBigMsgBlocks)
                               % ((8*sizeof(merc_uint_t)))) {
          szBMap += 1;
      }

      MSD_ZERO_MEMORY(padapter->pww_info->BigMsgRcvMemStr.pBitMapValue,
                                       (szBMap * sizeof(merc_uint_t)));

      if (padapter->phw_info->pciSubSysId != SUBSYSID_ROZETTA_21554)
          msgutl_ww_build_and_send_msgblk_mf(padapter->pww_info);

      //Send an ACK back to admin
      AckPtr = (MD_ACK*)MD_GET_MDMSG_PAYLOAD(MdMsg);
      AckPtr->ErrorCode = ErrorCode;
      AckPtr->MessageId = MdMsg->MessageId;

      if (padapter->rtkMode == 0)
          AckPtr->ReturnValue = 0x2; //8BIT RTK
      else
          AckPtr->ReturnValue = 0x2; //16BIT RTK

      MD_SET_MSG_WRITE_PTR(Msg,MD_GET_MSG_READ_PTR(Msg)+
                       sizeof(MDRV_MSG)+sizeof(MD_ACK));
#if defined LiS || defined LIS || defined LFS
      supp_process_sync_receive(MsdOpenBlock, Msg);
#else
      {
      merc_uint_t size = 0;
      pmerc_uchar_t savedbuffer;
      size = MD_GET_MSG_WRITE_PTR(Msg) - MD_GET_MSG_READ_PTR(Msg);
      savedbuffer = mercd_allocator(size);
      if (savedbuffer == NULL) {
          printk("ERROR SetMode3rdRockFamily: memory allocation failed\n");
          return;
      }
      
      MSD_ZERO_MEMORY(savedbuffer, size);
      MsdCopyMemory(Msg->b_rptr, savedbuffer, size);
      Msg->b_datap->db_base = savedbuffer;
      Msg->b_datap->db_lim = (char *)savedbuffer + size;
      Msg->b_rptr = (char *)savedbuffer;
      Msg->b_wptr = (char *)savedbuffer+size;
      }
      supp_process_receive(MsdOpenBlock, Msg);
#endif /* LiS */
      return;
  } else {
      if (Status == MERCD_WW_POST_IN_PROGRESS) {
          cmn_err(CE_WARN, "3rd Rock RTK: post in progress state\n");
          ErrorCode = CD_ERR_WW_RTK_POST_IN_PROGRESS;
      } else {
          cmn_err(CE_WARN, "3rd Rock RTK: post read error\n");
          ErrorCode = CD_ERR_WW_RTK_POST_LOCATION_READ;
      }
      padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
      Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
      if (MD_IS_MSG_ASYNC(Msg))
          supp_process_receive(MsdOpenBlock, Msg);
      else {
#if defined LiS || defined LIS || defined LFS
          supp_process_sync_receive(MsdOpenBlock, Msg);
#else
	  supp_process_receive(MsdOpenBlock, Msg);
#endif
      }

      return;
  }

  //For a 3rd Rock Family of boards, the driver would have
  //already switched over to WW mode, which happens during
  //the WWSetParam context itself. So, no need to call
  //mid_wwmgr_configure_brd_to_ww
  return;
}


/***************************************************************************
 * Function Name                : mid_wwmgr_start_streams
 * Function Type                : manager function
 * Inputs                       : 
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          : WW Streaming Change
 *                              : This API gets called by MDI for every 
 *                              : successful stream open.
 ****************************************************************************/
void mid_wwmgr_start_streams(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
  merc_uint_t                  MsgSize;
  PWW_STREAM_START_READ        StartReadPtr;
  MD_ACK                       *AckPtr;
  MD_STATUS                    Status;
  pmercd_adapter_block_sT      padapter;
  REGISTER PMDRV_MSG           MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  merc_uint_t                  ErrorCode = MD_OK;
  merc_uint_t                  ReturnValue=0;
  merc_uint_t                  StreamId;
#ifndef LFS
  merc_uint_t                  SlotNumber;
#endif
  pmercd_stream_connection_sT  StreamBlock;
  pmercd_bind_block_sT         BindBlock;
#ifdef _8_BIT_INSTANCE
  MSD_HANDLE                   BindHandle = 0;
#else
  MBD_HANDLE                   BindHandle = 0;
#endif


  Status = MD_SUCCESS;

  MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

  // perform validity checking
  MSD_RETURN_MSG_SIZE_CHECK(MsgSize, WW_STREAM_START_READ, "StartReadStreams");


  MdMsg->MessageId = MID_START_WW_STREAM_READ_ACK;
  StartReadPtr = (PWW_STREAM_START_READ)MD_GET_MDMSG_PAYLOAD(MdMsg);
  AckPtr = (MD_ACK  *)StartReadPtr;

  StreamId= StartReadPtr->StreamId;
  BindHandle = StartReadPtr->bind;

  BindBlock = MsdControlBlock->pbind_block_list[BindHandle];
  StreamBlock = BindBlock->stream_connection_ptr;

  //printk("mid_wwmgr_start_streams: StreamId: %d StreamBlock 0x%x BindHandle: %d\n",
  //        StreamId, StreamBlock, BindHandle);


  if ((StreamBlock == NULL) || (StreamId != StreamBlock->id)) {
      printk("mid_wwmgr_start_streams: StreamBlock is NULL...\n");
      printk("mid_wwmgr_start_streams: StreamId: %d StreamBlock->id: %d\n",
            StreamId, StreamBlock->id);
      ErrorCode=CD_ERR_ADAPTER_INVALID;
      Status = MD_FAILURE;
      goto out;
  }

  padapter = StreamBlock->padapter_block;

  if (padapter == NULL) {
      printk("mid_wwmgr_start_streams: Invalid Adapter Number!\n");
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      Status = MD_FAILURE;
      goto out;
  }

  if ((padapter->state == MERCD_ADAPTER_STATE_SUSPENDED) || 
      (padapter->state == MERCD_ADAPTER_STATE_MAPPED)) {
      printk("mid_wwmgr_start_streams: Invalid Adapter State!\n");
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      Status = MD_FAILURE;
      goto out;
  }

  // DMV-B
  if ((((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DM3) ||
        (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DISI)) &&
      (padapter->flags.WWFlags & MERCD_ADAPTER_WW_MODE_NOTENABLED)) ||
      (padapter->pww_info == NULL)) {
      MSD_LEVEL2_DBGPRINT("mid_wwmgr_start_streams: padapter->pww_info is NULL\n");
      MSD_LEVEL2_DBGPRINT("mid_wwmgr_start_streams: WWFlags: 0x%x\n", padapter->flags.WWFlags);
      Status = MD_SUCCESS;
      MSD_ZERO_MEMORY(AckPtr, sizeof(MD_ACK));
      AckPtr->ErrorCode = MD_OK;
      //printk("AckPtr->ErrorCode: 0x%x\n", AckPtr->ErrorCode);
      AckPtr->MessageId = MdMsg->MessageId;   /* MID_START_WW_STREAM_READ_ACK */
      MD_SET_MSG_WRITE_PTR(Msg,MD_GET_MSG_READ_PTR(Msg)+
                      sizeof(MDRV_MSG)+sizeof(MD_ACK));
      // return the message to user via sync path
      supp_process_sync_receive(MsdOpenBlock, Msg);
      return;
  }

  if (padapter->pww_info->state != MERCD_ADAPTER_WW_SUCCEDED) {
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      Status = MD_FAILURE;
      goto out;
  }

  MSD_ENTER_MUTEX(&padapter->pww_info->ww_msgpendq_mutex);

  Status = mid_wwmgr_alloc_streams_descriptors(padapter, StreamBlock);
  if (Status != MD_SUCCESS) {
      printk("mid_wwmgr_open_streams  failed..\n");
      ErrorCode = CD_ERR_NO_MEMORY;
      MSD_EXIT_MUTEX(&padapter->pww_info->ww_msgpendq_mutex);
      goto out;
  }


  //if (StreamBlock->WWRcvStrDataBufPostInAdvance == 0)
  //   printk("WWRcvStrDataBufPostInAdvance is 0 hence not posting the buffers..id: %d\n",
  //           StreamBlock->id);

  //Send the buffers down to the board upto 10 datablocks.
  if ((StreamBlock->WWRcvStrDataBufPostInAdvance) && 
             (StreamBlock->WWRcvStrDataBufAllocd)) {
      Status = mid_wwmgr_send_rcvstrm_datablocks_to_board(padapter, StreamBlock);  

      if (Status == MD_FAILURE) {
          printk("mid_wwmgr_start_streams: Could not send Rcv Strm Datablocks\n");
          ErrorCode=ENOMEM;
          Status = MD_FAILURE;
          MSD_EXIT_MUTEX(&padapter->pww_info->ww_msgpendq_mutex);
          goto out;
      }
  }

  MSD_EXIT_MUTEX(&padapter->pww_info->ww_msgpendq_mutex);

  //Send an Ack Back
  Status = MD_SUCCESS;
  AckPtr->ErrorCode = MD_OK;
  AckPtr->MessageId = MdMsg->MessageId;   /* MID_START_WW_STREAM_READ_ACK */
  MD_SET_MSG_WRITE_PTR(Msg,MD_GET_MSG_READ_PTR(Msg)+
                   sizeof(MDRV_MSG)+sizeof(MD_ACK));
  //return the message to user via sync path
  supp_process_sync_receive(MsdOpenBlock, Msg);

  return;

out: 
  Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
  if (MD_IS_MSG_ASYNC(Msg))
     supp_process_receive(MsdOpenBlock, Msg);
  else
     supp_process_sync_receive(MsdOpenBlock, Msg);
  return;
}


/***************************************************************************
 * Function Name                : mid_wwmgr_debug_streams
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      : none
 * Calling functions            :
 * Description                  :
 * Additional comments          : WW Streaming Change: Debug Util
 *                              : Applications can call this API on a timeout
 *                              : or error conditions while streaming to get
 *                              : some state information about the stream
 *                              : at the driver level
 ****************************************************************************/
void mid_wwmgr_debug_streams(PMSD_OPEN_BLOCK MsdOpenBlock,PSTRM_MSG Msg)
{
  merc_uint_t                  MsgSize;
  pmerc_uchar_t                StartPtr;
  MD_ACK                       *AckPtr;
  pmercd_adapter_block_sT      padapter;
  REGISTER PMDRV_MSG           MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
  merc_uint_t                  ErrorCode = MD_OK;
  merc_uint_t                  ReturnValue=0;
  merc_uint_t                  StreamId;
#ifndef LFS
  merc_uint_t                  SlotNumber;
#endif
  pmercd_stream_connection_sT  StreamBlock;
  pmercd_bind_block_sT         BindBlock;
#ifdef _8_BIT_INSTANCE
  MSD_HANDLE                   BindHandle = 0;
#else
  MBD_HANDLE                   BindHandle = 0;
#endif


#ifndef LFS
  merc_uint_t tout_dbg = 0x8;
#endif

  printk("mid_wwmgr_debug_streams...\n");

  MsgSize = MD_GET_MSG_SIZE(Msg) - sizeof(MDRV_MSG);

  // perform validity checking
  MSD_RETURN_MSG_SIZE_CHECK(MsgSize, WW_STREAM_START_READ, "DebugStreams");


  MdMsg->MessageId = 0;
#ifdef LFS
  StartPtr = (pmerc_uchar_t)MD_GET_MDMSG_PAYLOAD(MdMsg);
#else
  StartPtr = (pmerc_uint_t)MD_GET_MDMSG_PAYLOAD(MdMsg);
#endif
  AckPtr = (MD_ACK  *)StartPtr;

  BindHandle = *((pmerc_uint_t)(StartPtr));
  StartPtr += sizeof(merc_uint_t);
  StreamId= *((pmerc_uint_t)(StartPtr));

  BindBlock = MsdControlBlock->pbind_block_list[BindHandle];
  StreamBlock = BindBlock->stream_connection_ptr;

#ifdef LFS
  printk("mid_wwmgr_debug_streams: StreamId: %d StreamBlock 0x%p BindHandle: %d\n",
         StreamId, StreamBlock, BindHandle);
#else
  printk("mid_wwmgr_debug_streams: StreamId: %d StreamBlock 0x%x BindHandle: %d\n",
         StreamId, StreamBlock, BindHandle);
#endif

  if ((StreamBlock == NULL) || (StreamId != StreamBlock->id)) {
      printk("mid_wwmgr_debug_streams: StreamBlock is NULL\n");
      printk("mid_wwmgr_debug_streams: StreamId: %d StreamBlock->id: %d\n",
            StreamId, StreamBlock->id);
      ErrorCode=CD_ERR_ADAPTER_INVALID;
      goto out;
  }


  padapter = StreamBlock->padapter_block;

  if (padapter == NULL) {
      printk("mid_wwmgr_debug_streams: padapter is NULL\n");
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      goto out;
  }

  if ((padapter->state == MERCD_ADAPTER_STATE_SUSPENDED) || 
      (padapter->state == MERCD_ADAPTER_STATE_MAPPED)) {
      printk("mid_wwmgr_debug_streams: Invalid Adapter State\n");
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      goto out;
  }

  if ((padapter->flags.WWFlags & MERCD_ADAPTER_WW_MODE_NOTENABLED) ||
      (padapter->pww_info == NULL)) {
      printk("mid_wwmgr_debug_streams: padapter->pww_info is NULL\n");
      printk("mid_wwmgr_debug_streams: WWFlags: 0x%x\n", padapter->flags.WWFlags);
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      goto out;
  }

  if (padapter->pww_info->state != MERCD_ADAPTER_WW_SUCCEDED) {
      ErrorCode = CD_ERR_ADAPTER_INVALID;
      goto out;
  }

  mid_wwmgr_dump_streamblock_info(StreamBlock);

  //Send an Ack Back
  AckPtr->ErrorCode = MD_OK;
  AckPtr->MessageId = MdMsg->MessageId;   
  MD_SET_MSG_WRITE_PTR(Msg,MD_GET_MSG_READ_PTR(Msg)+
                      sizeof(MDRV_MSG));
  // return the message to user via sync path
  supp_process_sync_receive(MsdOpenBlock, Msg);
  return;

out:
  Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
  if (MD_IS_MSG_ASYNC(Msg))
      supp_process_receive(MsdOpenBlock, Msg);
  else
      supp_process_sync_receive(MsdOpenBlock, Msg);
  return;
}
