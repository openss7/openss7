/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdwwinitmngrutl.c
 * Description                  : WW Driver Manager init utility
 *
 *
 **********************************************************************/

#include "msd.h"
#include "../include/msdpciif.h"
#define _MSDWWMGR_C_
#include "msdextern.h"
#undef _MSDWWMGR_C_

extern int HCS_Flag;
/***************************************************************************
 * Function Name                : mid_wwmgr_configure_drvr_to_ww
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock,
 *                                Msg
 * Outputs                      : Configures the Board for WW mode operation
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
void
mid_wwmgr_configure_brd_to_ww(PMSD_OPEN_BLOCK MsdOpenBlock, pmercd_adapter_block_sT padapter, PSTRM_MSG Msg)
{
 MD_STATUS                                Status;
 pmercd_ww_dev_info_sT                    pwwDev;
#ifndef LFS
 mercd_osal_timeout_stop_sT               timeoutinfo = { 0 };
 mercd_osal_dpc_register_sT               dpc_dereginfo = { 0 };
 mercd_osal_intr_deregister_sT            intr_deregisterinfo = { 0 };
 mercd_osal_intr_register_sT              intrinfo_reg = { 0 };
 mercd_osal_dpc_register_sT               dpc_reginfo = { 0 };
 mercd_osal_mem_virt_free_sT              meminfo1 = { 0 };
#endif
 mercd_ww_gen_doorbell_sT                 doorbell;
#ifndef LFS
 mercd_dhal_intr_disable_sT               intr_disableinfo = { 0 };
 mercd_dhal_intr_enable_sT                intrinfo = { 0 };
#endif
 merc_uint_t                              ErrorCode;
#ifndef LFS
 merc_uint_t                              szBigMsgBlock;
 merc_uint_t                              needOSResources = 1;
#endif
 merc_uint_t                              numBigMsgs;
 merc_uint_t                              szBMap;
 merc_uint_t                              count;
#if !defined LiS && !defined LIS && !defined LFS
 merc_uint_t                              size;
 pmerc_char_t                             savedbuffer;
#endif


 Status = MD_SUCCESS;

 i_printmsg(
                 "mid_wwmgr_configure_drvr_to_ww 0x%x 0x%x 0x%x %s #%d\n", 
                  (size_t)MsdOpenBlock, (size_t)padapter, (size_t)Msg,
                  __FILE__, __LINE__ 
                  );

   //Get the adapter structure
   if( padapter == NULL) {
          MSD_LEVEL2_DBGPRINT("mid_wwmgr_configure_brd_to_ww: Invalid Adapter!\n");
          ErrorCode = PAM_ERR_WW_PAM_ERR_NO_MEMORY;
#ifdef LFS
          mid_wwmgr_send_setwwmode_ack_to_admin(NULL, ErrorCode);
#else
          mid_wwmgr_send_setwwmode_ack_to_admin(pwwDev, ErrorCode);
#endif
          Status = MD_FAILURE;
          return;
   }

#ifdef LFS
     /* Well, the following was one of the dumbest checks I have seen in a long time.  Testing
      * symbol i386 to determine endianess of the target host is not too smart. */
#ifdef BIG_ENDIAN
     //Enable the BIGEND mode in PLX Big/Little Endian Descr Register
     MsdWWPlxEnableBigEndian(padapter, MERCD_WW_BIGEND_REGISTER_BITS);
#endif
#else
#ifndef i386
     //Enable the BIGEND mode in PLX Big/Little Endian Descr Register
     MsdWWPlxEnableBigEndian(padapter, MERCD_WW_BIGEND_REGISTER_BITS);
#endif
#endif

     //Get WW device
     pwwDev = padapter->pww_info;

    //Leave the Msg on pwwDev->private_data. We need to
    //send this message back to Admin Component that
    //called WWSetMode.
#if !defined LiS && !defined LIS && !defined LFS
    size = MD_GET_MSG_WRITE_PTR(Msg) - MD_GET_MSG_READ_PTR(Msg);
    savedbuffer = mercd_allocator(size);
    //////////////////////////////////////
    //New Mem Safe Guard
    /////////////////////////////////////
    if (savedbuffer == NULL)
     {
       MSD_ERR_DBGPRINT("ERROR: ConfigBrdToWW:\
                         Mem Allocation Failure for savedbuffer\n");
       return;
     }
    /////////////////////////////////////
    MsdCopyMemory(Msg->b_rptr, savedbuffer, size);
    Msg->b_datap->db_base = savedbuffer;
    Msg->b_datap->db_lim = (char *)savedbuffer + size;
    Msg->b_rptr = (char *)savedbuffer;
    Msg->b_wptr = (char *)savedbuffer+size;
#endif

    pwwDev->private_data1 = (pmerc_void_t)Msg;
    pwwDev->private_data2 = (pmerc_void_t)MsdOpenBlock;

     //Set WW device state
     if (pwwDev->state == MERCD_ADAPTER_WW_IN_SHUTDOWN)
         pwwDev->state = MERCD_ADAPTER_WW_NOTREADY;

     //Allocate memory for Performance Counters
     pwwDev->pww_counters =
           (pmerc_perf_counters_sT)mercd_allocator(MERCD_WW_PERF_COUNTERS);

     if (pwwDev->pww_counters == NULL)
      {
       MSD_LEVEL2_DBGPRINT(
               "mid_wwmgr_configure_brd_to_ww: Alloc for pww_counters Failed %s #%d\n",
                __FILE__,
                __LINE__
                );
        ErrorCode = PAM_ERR_WW_PAM_ERR_NO_MEMORY;
        pwwDev->state = MERCD_ADAPTER_WW_FAILED;
        mid_wwmgr_send_setwwmode_ack_to_admin(pwwDev, ErrorCode);
        return;
      }

     MSD_ZERO_MEMORY(pwwDev->pww_counters, sizeof(merc_perf_counters_sT));


     //Allocate contiguous non-paged memory (128Bytes aligned) for MFs
      Status = mid_wwmgr_allocate_mem_for_mf(pwwDev);
      if (Status != MD_SUCCESS) {
        MSD_LEVEL2_DBGPRINT(
               "mid_wwmgr_configure_brd_to_ww: Alloc for MF Failed %s #%d\n", 
                __FILE__, 
                __LINE__
                );
        ErrorCode = PAM_ERR_WW_PAM_ERR_NO_MEMORY;
        pwwDev->state = MERCD_ADAPTER_WW_FAILED;
        mid_wwmgr_send_setwwmode_ack_to_admin(pwwDev, ErrorCode);
        return;
      }

      //Allocate contiguous memory for Big Message blocks
      Status = mid_wwmgr_allocate_mem_for_bigmsgblks(pwwDev);
      if (Status != MD_SUCCESS) {
        MSD_LEVEL2_DBGPRINT(
               "mid_wwmgr_configure_brd_to_ww: Alloc for BIG Msg Blks Failed %s #%d\n", 
               __FILE__,
               __LINE__
               );

        ErrorCode = PAM_ERR_WW_PAM_ERR_NO_MEMORY;
        pwwDev->state = MERCD_ADAPTER_WW_FAILED;
        cmn_err(CE_WARN, "mid_wwmgr_allocate_mem_for_bigmsgblks Alloc Failed..\n");
        mid_wwmgr_send_setwwmode_ack_to_admin(pwwDev, ErrorCode);
        return;
      }

      if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
         padapter->flags.SendTimeoutPending = 0;

         //Save the New Hardware Info Struct
         pwwDev->phw_info = padapter->phw_info;

      }/* THIRD_ROCK_FAMILY Check */

      //Init the Message Q Pending Flag.
      pwwDev->WWMsgPendingQFlag = 0;
      pwwDev->WWCancelBigMsgAckPending = 0;
      pwwDev->WWCloseAckToutCheck = 0;

      //Init Mutex for the Pending Q
      MSD_INIT_MUTEX(&pwwDev->ww_msgpendq_mutex, "Msg PendingQ Mutex", NULL);
      MSD_INIT_MUTEX(&pwwDev->ww_iboundQ_mutex, "InBoundQ Mutex", NULL);
      MSD_INIT_MUTEX(&pwwDev->ww_oboundQ_mutex, "OutBoundQ Mutex", NULL);
      MSD_INIT_MUTEX(&pwwDev->ww_eospendq_mutex, "EOS PendingQ Mutex", NULL);

      /////////////////////////////////////////////////////////////////////
      //Init the global BitMap for BindBlocks w/ pending cancel Msg MF req.
      /////////////////////////////////////////////////////////////////////
      pwwDev->szPendBindCancelMsgMFBitMap = MsdControlBlock->maxbind;
      szBMap = ((pwwDev->szPendBindCancelMsgMFBitMap)/(8 * sizeof(merc_uint_t)));
      if ((pwwDev->szPendBindCancelMsgMFBitMap)%((8*sizeof(merc_uint_t))))
	  szBMap += 1;
      pwwDev->pPendBindCancelMsgMFBitMap = (pmerc_uchar_t)
		   MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY((szBMap) *	(sizeof(merc_uint_t)));

      if (pwwDev->pPendBindCancelMsgMFBitMap == NULL)
       {
        printk( "mid_wwmgr_configure_brd_to_ww: Alloc for pPendBindCancelMsgMFBitMap Failed %s #%d\n", __FILE__, __LINE__);
        ErrorCode = PAM_ERR_WW_PAM_ERR_NO_MEMORY;
        pwwDev->state = MERCD_ADAPTER_WW_FAILED;
        mid_wwmgr_send_setwwmode_ack_to_admin(pwwDev, ErrorCode);
        return;
       }
      MSD_ZERO_MEMORY(pwwDev->pPendBindCancelMsgMFBitMap, 
                        ((szBMap) * (sizeof(merc_uint_t))));

      /////////////////////////////////////////////////////////////////////
      //Init the global BitMap for BindBlocks w/ pending cancel Str MF req.
      /////////////////////////////////////////////////////////////////////
      pwwDev->szPendBindCancelStrMFBitMap = MsdControlBlock->maxbind;
      szBMap = ((pwwDev->szPendBindCancelStrMFBitMap)/(8 * sizeof(merc_uint_t)));
      if ((pwwDev->szPendBindCancelStrMFBitMap)%((8*sizeof(merc_uint_t))))
	   szBMap += 1;
      pwwDev->pPendBindCancelStrMFBitMap = (pmerc_uchar_t)
	      			   MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY((szBMap) *
						   (sizeof(merc_uint_t)));
#if 0
                                   mercd_allocator((szBMap) * (sizeof(merc_uint_t)));
#endif
      if (pwwDev->pPendBindCancelStrMFBitMap == NULL)
       {
         printk( "mid_wwmgr_configure_brd_to_ww: Alloc for pPendBindCancelStrMFBitMap Failed %s #%d\n", __FILE__, __LINE__);
         MSD_LEVEL2_DBGPRINT( "mid_wwmgr_configure_brd_to_ww: Alloc for pPendBindCancelStrMFBitMap %s #%d\n", __FILE__, __LINE__);
        ErrorCode = PAM_ERR_WW_PAM_ERR_NO_MEMORY;
        pwwDev->state = MERCD_ADAPTER_WW_FAILED;
        mid_wwmgr_send_setwwmode_ack_to_admin(pwwDev, ErrorCode);
        return;
       }
      MSD_ZERO_MEMORY(pwwDev->pPendBindCancelStrMFBitMap,
			((szBMap) * (sizeof(merc_uint_t))));
      
      ////////////////////////////////////////////////////////////////////
      //Init the per BindBlock Message Pending BitMap
      ////////////////////////////////////////////////////////////////////
      pwwDev->szPendMsgBitMap = numBigMsgs = pwwDev->pww_param->numberBigMsgBlocks;
      pwwDev->pPendMsgBitMap = (pmerc_uchar_t *)
                             MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY((MsdControlBlock->maxbind) *
                                                              (sizeof(pmerc_uchar_t)));
#if 0
                                       mercd_allocator((MsdControlBlock->maxbind) *
                                        (sizeof(pmerc_uchar_t)));
#endif

      if (pwwDev->pPendMsgBitMap == NULL)
       {
  	 printk( "mid_wwmgr_configure_brd_to_ww: Alloc for pPendMsgBitMap Failed %s #%d\n", __FILE__, __LINE__);
         MSD_LEVEL2_DBGPRINT( "mid_wwmgr_configure_brd_to_ww: Alloc for pPendMsgBitMap %s #%d\n", __FILE__, __LINE__);
         ErrorCode = PAM_ERR_WW_PAM_ERR_NO_MEMORY;
         pwwDev->state = MERCD_ADAPTER_WW_FAILED;
         mid_wwmgr_send_setwwmode_ack_to_admin(pwwDev, ErrorCode);
         return;
       }

      MSD_ZERO_MEMORY(pwwDev->pPendMsgBitMap,
                              (MsdControlBlock->maxbind) *
                                          (sizeof(pmerc_uchar_t)));

      szBMap = ((numBigMsgs)/(8 * sizeof(merc_uint_t)));

      if ((numBigMsgs)%((8*sizeof(merc_uint_t))))
         szBMap += 1;

      for (count = 0; count < MsdControlBlock->maxbind; count++)
       {
          pwwDev->pPendMsgBitMap[count] = (pmerc_uchar_t)
                             MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY(szBMap *
                                                           sizeof(merc_uint_t));
#if 0
                                           mercd_allocator(szBMap *
                                                   sizeof(merc_uint_t));
#endif
          if (pwwDev->pPendMsgBitMap[count] == NULL)
            {
              printk( "mid_wwmgr_configure_brd_to_ww: Alloc for pPendMsgBitMap[%d] Failed %s #%d\n", count, __FILE__, __LINE__);
              MSD_LEVEL2_DBGPRINT( "mid_wwmgr_configure_brd_to_ww: Alloc for pPendMsgBitMap[%d] %s #%d\n", count, __FILE__, __LINE__);
              ErrorCode = PAM_ERR_WW_PAM_ERR_NO_MEMORY;
              pwwDev->state = MERCD_ADAPTER_WW_FAILED;
              mid_wwmgr_send_setwwmode_ack_to_admin(pwwDev, ErrorCode);
              return;
            }
          MSD_ZERO_MEMORY(pwwDev->pPendMsgBitMap[count],
                                       ((szBMap) * (sizeof(merc_uint_t))));
       }

    ////////////////////////////////////////////////////////////////////
    //Init the global BitMap for Streams with Pending Flush MFs
    ////////////////////////////////////////////////////////////////////
    pwwDev->szPendFlushStrBitMap = MsdControlBlock->maxstreams;
    szBMap = ((pwwDev->szPendFlushStrBitMap)/(8 * sizeof(merc_uint_t)));
    if ((pwwDev->szPendFlushStrBitMap)%(8 * sizeof(merc_uint_t)))
     szBMap+=1;
    pwwDev->pPendFlushStrBitMap =  (pmerc_uchar_t)
               MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY((szBMap) * (sizeof(merc_uint_t)));
#if 0
                      mercd_allocator((szBMap) * (sizeof(merc_uint_t)));
#endif
      if (pwwDev->pPendFlushStrBitMap == NULL)
       {
           printk( "mid_wwmgr_configure_brd_to_ww: Alloc for pPendFlushStrBitMap Failed %s #%d\n", __FILE__, __LINE__);
           MSD_LEVEL2_DBGPRINT( "mid_wwmgr_configure_brd_to_ww: Alloc for pPendFlushStrBitMap %s #%d\n", __FILE__, __LINE__);
           ErrorCode = PAM_ERR_WW_PAM_ERR_NO_MEMORY;
           pwwDev->state = MERCD_ADAPTER_WW_FAILED;
           mid_wwmgr_send_setwwmode_ack_to_admin(pwwDev, ErrorCode);
           return;
       }
      MSD_ZERO_MEMORY(pwwDev->pPendFlushStrBitMap, 
                      ((szBMap) *(sizeof(merc_uint_t))));

    ////////////////////////////////////////////////////////////////////
    //Init the global BitMap for Receive StreamBlocks with Pending
    //Receive Data Buffer Requests
    ////////////////////////////////////////////////////////////////////
    pwwDev->szPendReqRcvStrDataBufBitMap = MsdControlBlock->maxstreams; 
    szBMap = ((pwwDev->szPendReqRcvStrDataBufBitMap)/(8 * sizeof(merc_uint_t)));
    if ((pwwDev->szPendReqRcvStrDataBufBitMap)%(8 * sizeof(merc_uint_t)))
        szBMap+=1;
    pwwDev->pPendReqRcvStrDataBufBitMap =  (pmerc_uchar_t)
                MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY((szBMap) * (sizeof(merc_uint_t)));
#if 0
                      mercd_allocator((szBMap) * (sizeof(merc_uint_t)));
#endif
    if (pwwDev->pPendReqRcvStrDataBufBitMap == NULL)
     {
          printk( "mid_wwmgr_configure_brd_to_ww: Alloc for pPendReqRcvStrDataBufBitMap Failed %s #%d\n", __FILE__, __LINE__);
           MSD_LEVEL2_DBGPRINT(
              "mid_wwmgr_configure_brd_to_ww: Alloc for\
               pPendReqRcvStrDataBufBitMap %s #%d\n", 
               __FILE__,
               __LINE__
              );
           ErrorCode = PAM_ERR_WW_PAM_ERR_NO_MEMORY;
           pwwDev->state = MERCD_ADAPTER_WW_FAILED;
           mid_wwmgr_send_setwwmode_ack_to_admin(pwwDev, ErrorCode);
           return;
     }

    MSD_ZERO_MEMORY(pwwDev->pPendReqRcvStrDataBufBitMap,
                      ((szBMap) *(sizeof(merc_uint_t))));

    ////////////////////////////////////////////////////////////////////
    //Generate a init Doorbell interrupt to the board
    ////////////////////////////////////////////////////////////////////
    doorbell.pwwDevi = pwwDev;
    doorbell.value = MERCD_WW_INIT_INTERRUPT;

    //Set the state as MERCD_ADAPTER_WW_MODE_INIT_INTR_PENDING pwwDev->state
    pwwDev->state = MERCD_ADAPTER_WW_MODE_INIT_INTR_PENDING;
   
    Status=mid_wwmgr_generate_doorbell_to_board(&doorbell);
    //atomic_inc(&pwwDev->pww_counters->init_intr_db);

    //We return to SetWWMode caller  only after we get the
    //2nd interrupt from the board. i.e we ack the admin
    //component in init_msg ready intr, after changing
    //the padapter state to MERCD_ADAPTER_WW_SUCCEDED.
    //Do nothing.
    //Board responds back with WW_INIT_ACK_INTERRUPT
    //Then we need to set WWFlags = MERCD_ADAPTER_WW_INIT_INTR_ACKED )
}

/***************************************************************************
 * Function Name                : mid_wwmgr_read_ww_postlocation
 * Function Type                : manager function
 * Inputs                       : MsdOpenBlock
 * Outputs                      : READ the RTK posted locations for WW
 *                                capability
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
int mid_wwmgr_read_ww_postlocation(pmercd_adapter_block_sT padapter)
{
//1. Read the posted locations at offset 0xf8 and 0xfc in sram.
//2. If they read 0x02 and 0xfd then ww mode is supported
//   Set the adapter flag WWSupported = MERCD_ADAPTER_WW_MODE_ENABLED
//   Set the adapter flag WWStatus = MERCD_ADAPTER_WW_NOTREADY
//3. If the read value is not as specified above, then fail WW mode
//   Set the adapter flag WWSupported = MERCD_ADAPTER_WW_MODE_NOTENABLED
//   Set the adapter flag WWStatus = MERCD_ADAPTER_WW_NOTREADY

#ifndef LFS
        merc_uint_t                    MsgSize;
#endif
        MD_STATUS                      Status;
#ifndef LFS
        merc_uint_t                    ReturnValue = 0;
        pmerc_uchar_t                  PostLocation;
#endif
        merc_uchar_t                   ValLocation1; /* 0xF8 */
        merc_uchar_t                   ValLocation2; /* 0xFC */
        merc_uchar_t                   HostRamReq;
        merc_uchar_t                   DebugVal;
#ifndef LFS
        merc_uint_t                    wait_time = 200;
        merc_ulong_t                   wcount = 0;
#endif


        Status = MD_SUCCESS;
        MSD_FUNCTION_TRACE(
            "mid_wwmgr_read_ww_postlocation 0x%x %s #%d\n", 
             THREE_PARAMETERS, 
             (size_t)padapter,
             __FILE__,
             __LINE__
             );

       MERC_GET_SRAM_LOCK((&padapter->phost_info->reg_block), HostRamReq);
        if(!(HostRamReq & MERC_HOST_RAM_GRANT_FOR_CLR_R))
        {
          MSD_LEVEL2_DBGPRINT(
               "mid_wwmgr_read_ww_postlocation: Failed to get SRAM %s #%d\n", 
               __FILE__,
               __LINE__
               );
           return(Status = MD_FAILURE);
        }

       do {
            MERC_CHECK_DEBUG_ONE((&padapter->phost_info->reg_block), DebugVal);
            ValLocation1 = DebugVal;
            MERC_CHECK_DEBUG_TWO((&padapter->phost_info->reg_block), DebugVal);
            ValLocation2 = DebugVal;

            MSD_LEVEL2_DBGPRINT("ValLocation1 : 0x%x ValLocation2: 0x%x\n",
              ValLocation1, ValLocation2,
              __FILE__, __LINE__
              );

	    // DMV-B
            if ( ((VALID_SRAM_8BIT_POST_CONTENTS) || (VALID_SRAM_16BIT_POST_CONTENTS)) || 
		 ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DMVB)||
		  (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_NEWB)))
             {
		// Cannot invoke schedule timeout
		Status = MD_FAILURE;
                break;
             }
         } while ((VALID_SRAM_8BIT_POST_CONTENTS) || (VALID_SRAM_16BIT_POST_CONTENTS));

        MERC_FREE_SRAM_LOCK((&padapter->phost_info->reg_block));

        //printk("ValLocation1: 0x%x, ValLocation2: 0x%x\n", ValLocation1, ValLocation2);

       if ((VALID_WW_8BIT_POST_CONTENTS) || (VALID_WW_16BIT_POST_CONTENTS))
         {
           padapter->flags.WWFlags = MERCD_ADAPTER_WW_MODE_ENABLED ;
           if (VALID_WW_8BIT_POST_CONTENTS)
                 padapter->rtkMode = 0;
           else
                 padapter->rtkMode = 1;
         }
       else
         if ((VALID_SRAM_8BIT_POST_CONTENTS) || (VALID_SRAM_16BIT_POST_CONTENTS))
           {
             padapter->flags.WWFlags = MERCD_ADAPTER_WW_MODE_NOTENABLED;
             if (VALID_SRAM_8BIT_POST_CONTENTS)
                 padapter->rtkMode = 0;
             else
                  padapter->rtkMode = 1;
           }
       else
            MSD_LEVEL2_DBGPRINT(
               "mid_wwmgr_read_ww_postlocation: Unknown PostLocation Contents %s #%d!\n",
                __FILE__,
                __LINE__
             );

#ifndef LFS
out:
#endif
   return(Status);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_check_3rdrock_running
 * Function Type                : manager function
 * Inputs                       : pwwDev
 * Outputs                      : READ the RTK posted locations for 3rd rock
 *                              : Family of Boards. We poll up to a maximum of
 *                              : 250 milli seconds.
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
int mid_wwmgr_check_3rdrock_running(pmercd_adapter_block_sT padapter, 
                                                      merc_uint_t KernelType)
{
 MD_STATUS Status;
 merc_ulong_t wcount = 0;
 merc_uint_t wait_time;

 if (padapter->flags.PostFlags == MD_MIN_POST_ENABLE)
	wait_time = 1200;
 else
	wait_time = 3000;

 Status = MD_FAILURE;

 do{
       Status = mid_wwmgr_read_3rdrock_postlocation(padapter, KernelType);
        if (Status != MD_SUCCESS) {
	   if (Status == MERCD_WW_POST_IN_PROGRESS) {
	       if (++wcount > wait_time ){
		   padapter->flags.WWFlags = MERCD_ADAPTER_WW_MODE_NOTENABLED;
		   break;		// still need to read the config rom below
	       }

	       set_current_state(TASK_UNINTERRUPTIBLE);
               schedule_timeout((20*HZ)/1000);
               set_current_state(TASK_RUNNING);
	   } else {
	       padapter->flags.WWFlags = MERCD_ADAPTER_WW_MODE_NOTENABLED;
	       break;			// still need to read the config rom below
	   }
	} /* !MD_SUCCESS */
	
   } while (Status != MD_SUCCESS);

 if (Status == MD_SUCCESS) {
      //By now MERCD_ADAPTER_WW_I20_MESSAGING_READY would have already
      //been set. CAUTION: Do not overwrite that bit.
      padapter->flags.WWFlags &= ~MERCD_ADAPTER_WW_MODE_NOTENABLED;
      padapter->flags.WWFlags |= MERCD_ADAPTER_WW_MODE_ENABLED ;
    }

 //Read and store the config rom
 padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->start_offset =
 MERC_READ_CONFIG_ROM_TRI_BYTES(padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr +
                                                                    MERC_CR_OFFSET_SRAM_START);

 MsdRegReadUcharBuffer((pmerc_char_t)padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr,
                     (pmerc_char_t)padapter->phost_info->merc_config_rom, MSD_CONFIG_ROM_MAX_SIZE);

 return (Status);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_read_3rdrock_postlocation
 * Function Type                : manager function
 * Inputs                       : pwwDev
 * Outputs                      : READ the RTK posted location for for 3rd
 *                              : rock Family of Boards
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
int mid_wwmgr_read_3rdrock_postlocation(pmercd_adapter_block_sT padapter, 
                                        merc_uint_t KernelType)
{
#ifdef LFS
 pmerc_ulong_t        regOffset = NULL;
#else
 pmerc_ulong_t        regOffset;
#endif
 merc_uint_t          regValue;
 merc_uchar_t         ValLocation1; /* 0xF8 */
 merc_uchar_t         ValLocation2; /* 0xFC */
 MD_STATUS            Status = MD_SUCCESS;

 MSD_FUNCTION_TRACE( "mid_wwmgr_read_3rdrock_postlocation 0x%x %s #%d\n", 
                THREE_PARAMETERS, (size_t)padapter, __FILE__, __LINE__);

   //printk("mid_wwmgr_read_3rdrock_postlocation...\n");

   switch (padapter->phw_info->pciSubSysId)
    {
    case SUBSYSID_21554:
    case SUBSYSID_ROZETTA_21554:
       //The post location is a 32 bit scratchpad register
#ifdef LFS
     regOffset = (pmerc_ulong_t)(padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr + 
                                        MERCD_3RDROCK_21554_POST_LOC);
#else
     regOffset = (merc_ulong_t)(padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr + 
                                        MERCD_3RDROCK_21554_POST_LOC);
#endif
              break;
      case SUBSYSID_80321:
#ifdef LFS
              regOffset = (pmerc_ulong_t)padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr +
                                                          MERCD_3RDROCK_80321_POST_LOC;
#else
              regOffset = (merc_ulong_t)padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr +
                                                          MERCD_3RDROCK_80321_POST_LOC;
#endif
              break;
      default:
            MSD_LEVEL2_DBGPRINT(
                "mid_wwmgr_read_3rdrock_postlocation:Unknown pciSubSysId %s #%d\n",
                 __FILE__, 
                 __LINE__
                 );
            break;
    }

   regValue = *((volatile merc_uint_t *)(regOffset));

   ValLocation1 = (merc_uchar_t)(regValue & 0x000000ff);
   ValLocation2 = (merc_uchar_t)((regValue & 0x0000ff00) >> 8);

   //printk("3rd Rock1: ValLocation1: 0x%x, ValLocation2: 0x%x\n", ValLocation1, ValLocation2);

   switch (KernelType)
    {
       case MERCD_WW_BOOT_KERNEL:
           if (VALID_WW_8BIT_POST_CONTENTS) 
              {
                Status = MD_SUCCESS;

              }
            else if (VALID_WW_16BIT_POST_CONTENTS)  {
                ValLocation1 = MERCD_WW_POST_IN_OPERATION;
                Status = MD_FAILURE;
                }else
                 Status = MD_FAILURE;
            break;

       case MERCD_WW_RUNTIME_KERNEL:
            if ((VALID_WW_8BIT_POST_CONTENTS) || (VALID_WW_16BIT_POST_CONTENTS))
              {
                Status = MD_SUCCESS;
                if (VALID_WW_8BIT_POST_CONTENTS)
                   padapter->rtkMode = 1;
                else
                   padapter->rtkMode = 1;
               }
             else
               Status = MD_FAILURE;

              break;

       default:
               printk("UnKnown Kernel Type..\n");
               return(MD_FAILURE);
               break;

    } /* switch */


     if (Status == MD_FAILURE)
      {
         if ((ValLocation1 == MERCD_WW_POST_IN_OPERATION) ||
             (ValLocation1 == MERCD_WW_POST_NOT_STARTED) ||
             (ValLocation1 == MERCD_WW_INIT_POST_IN_PROGRESS))
          {
             Status = MERCD_WW_POST_IN_PROGRESS;
          }
        else
         {
           MSD_LEVEL2_DBGPRINT("ValLocation1: 0x%x ValLocation2: 0x%x %s #%d\n",
                                ValLocation2,
                                __FILE__,
                                __LINE__
                              );
           Status = MD_FAILURE;
         }
      } /* Status == MD_FAILURE */

 return(Status);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_clear_3rdrock_postlocation
 * Function Type                : manager function
 * Inputs                       : pwwDev
 * Outputs                      : CLEAR the 3rdrock POST Location
 *                              : 
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 *
 *                               This function resets the lower order byte or 
 *                               the FC location of the POST location to
 *                               a pre-defined value. This value is also taken 
 *                               as a POST_IN_PROGRESS value and the
 *                               driver verifies this value also when it checks 
 *                               for the post status. This is used when we reset 
 *                               the board and then before sending the switch to WW, 
 *                               we read the post location to make sure that the 
 *                               board came up. If we still see a POST_IN_PROGRESS
 *                               then we return a DEVICE_NOT_READY in which case 
 *                               the admin loops around before trying again.
 ****************************************************************************/
int mid_wwmgr_clear_3rdrock_postlocation(pmercd_adapter_block_sT padapter)
{
#ifdef LFS
  pmerc_ulong_t        regOffset = NULL;
#else
  pmerc_ulong_t        regOffset;
#endif
  merc_uint_t          regValue;
  MD_STATUS            Status;

  Status = MD_SUCCESS;

  MSD_FUNCTION_TRACE(
               "mid_wwmgr_clear_3rdrock_postlocation 0x%x %s #%d\n",
                THREE_PARAMETERS, (size_t)padapter, __FILE__, __LINE__);

   switch (padapter->phw_info->pciSubSysId)
    {
     case SUBSYSID_21554:
     case SUBSYSID_ROZETTA_21554:
       //The post location is a 32 bit scratchpad register
#ifdef LFS
       regOffset = (pmerc_ulong_t)(padapter->phw_info->\
                   virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
                   MERCD_3RDROCK_21554_POST_LOC);
#else
       regOffset = (merc_ulong_t)(padapter->phw_info->\
                   virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+\
                   MERCD_3RDROCK_21554_POST_LOC);
#endif
       break;
     case SUBSYSID_80321:
#ifdef LFS
       regOffset = (pmerc_ulong_t)padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr +
                                          MERCD_3RDROCK_80321_POST_LOC;
#else
       regOffset = (merc_ulong_t)padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr +
                                          MERCD_3RDROCK_80321_POST_LOC;
#endif
              break;
     default:
            MSD_LEVEL2_DBGPRINT(
                "mid_wwmgr_read_3rdrock_postlocation:Unknown pciSubSysId %s #%d\n",
                 __FILE__,
                 __LINE__
                 );
            break;
    }

   //Write the predefined value into the scratch pad registers
   regValue = *((volatile merc_uint_t *)(regOffset));
   //printk("Contents of PostLocation before CLEAR: 0x%x\n", regValue);
   *((volatile merc_uchar_t *)(regOffset)) = MERCD_WW_INIT_POST_IN_PROGRESS;

   // Enabling Min Post
   if (padapter->flags.PostFlags == MD_MIN_POST_ENABLE) {
      *((volatile merc_uchar_t *)(regOffset)+0x4) = 0x1;
   }

   regValue = *((volatile merc_uint_t *)(regOffset));
   //printk("Contents of PostLocation after CLEAR: 0x%x\n", regValue);
   return(MD_SUCCESS);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_allocate_mem_for_mf
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      : Allocate Memory for MF
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
int mid_wwmgr_allocate_mem_for_mf(pmercd_ww_dev_info_sT pwwDev)
{
MD_STATUS Status;
merc_ulong_t totalMemReqd=0;
merc_ulong_t memSzInBoundQ=0;
mercd_dhal_ww_msgmf_alloc_sT mf_alloc = {0};
#ifndef LFS
pmerc_uchar_t paddr;
#endif

     MSD_FUNCTION_TRACE("mid_wwmgr_allocate_mem_for_mf 0x%x %s #%d\n", 
                         THREE_PARAMETERS, (size_t)pwwDev, __FILE__, __LINE__);
     Status = MD_SUCCESS;
     //1.  calculate the total_mem required for both in-bound
     //    and out-bound Qs

     totalMemReqd = ( pwwDev->pww_param->numberMFAInboundQ *
                     pwwDev->pww_param->sizeofMFAInboundQ );

     totalMemReqd += ( pwwDev->pww_param->numberMFAOutboundQ *
                     pwwDev->pww_param->sizeofMFAOutboundQ);
     totalMemReqd += 0x100; // Add 256 extra bytes of memory

     pwwDev->mfaBaseAddressDescr.size = totalMemReqd;
     mf_alloc.pwwDevi = pwwDev;
     mf_alloc.resource = (pmerc_void_t)&(pwwDev->mfaBaseAddressDescr);
     //2. For MFs Allocate Memory and Initialize mfaBaseAddressDescr
     (*mercd_dhal_func[MERCD_DHAL_WW_MF_ALLOC])((pmerc_void_t)&mf_alloc);
     if (mf_alloc.ret != MD_SUCCESS)
     {
       MSD_LEVEL2_DBGPRINT(
            "mid_wwmgr_allocate_mem_for_mf: MF Mem Alloc Failed %s #%d\n",
             __FILE__,
             __LINE__
             );
       Status = MD_FAILURE;
       goto out;
     }

      //3. Setup the inBoundQMfAddressDescr
       pwwDev->inBoundQMfAddress = (pmerc_uchar_t)pwwDev->mfaBaseAddressDescr.host_address;

      //4. Clear the Memory for inBoundQMfs
      MSD_ZERO_MEMORY(pwwDev->inBoundQMfAddress, (pwwDev->pww_param->numberMFAInboundQ * 
                                                 pwwDev->pww_param->sizeofMFAInboundQ));

      //5. Setup the outBoundQMfAddressDescr
       memSzInBoundQ =  (pwwDev->pww_param->numberMFAInboundQ *
                      pwwDev->pww_param->sizeofMFAInboundQ );
       pwwDev->outBoundQMfAddress =
                      (pmerc_uchar_t)((size_t)pwwDev->mfaBaseAddressDescr.host_address +
                                         memSzInBoundQ);

      //6. Clear the Memory for outBoundQMfs
      MSD_ZERO_MEMORY(pwwDev->outBoundQMfAddress, (pwwDev->pww_param->numberMFAOutboundQ * 
                                                   pwwDev->pww_param->sizeofMFAOutboundQ));

out:

return (Status);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_allocate_mem_for_bigmsgblks
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      : Allocate Memory for Msg Blocks
 * Calling functions            :
 * Description                  : Allocs memory for BigMsgRcvMemStr
 *                              : and BigMsgSndMemStr. For BigMsgRcvMemStr
 *                              : allocates the pBigMsgDescr table with DMAble
 *                              : memory (as Stream memory OR kmallocd memory)
 *                              : which board can DMA the data to.
 *                              :
 * Additional comments          :
 ****************************************************************************/
int mid_wwmgr_allocate_mem_for_bigmsgblks(pmercd_ww_dev_info_sT pwwDev)
{
   MD_STATUS                              Status;
#ifndef LFS
   merc_ulong_t                           totalMemReqd = 0;
   mercd_dhal_ww_rcv_copybigmsg_kmem_sT   mb_alloc     = {0};
#endif
   mercd_dhal_ww_rcv_copybigmsg_str_sT    dir_copy = {0};
   merc_ulong_t                           numBigMsgs;
   merc_uint_t                            szBMap;
   merc_uint_t                            count;
   pmercd_ww_dma_descr_sT           pbigMsgDmaDescr;

   Status = MD_SUCCESS;

   MSD_FUNCTION_TRACE(
        "mid_wwmgr_allocate_mem_for_bigmsgblks 0x%x %s #%d\n", 
         THREE_PARAMETERS, 
         (size_t)pwwDev,
         __FILE__,
         __LINE__
         );

   ////////////////////////////////////////////////////////////
   //1. Allocs Memory for pBigMsgDescr table in BigMsgRcvMemStr
   ////////////////////////////////////////////////////////////

   //SCHEME1 : try to alloc streams buffers
    dir_copy.pwwDevi = pwwDev;
    dir_copy.purpose = MERCD_WW_BIGMSG_RCV_DESCR_TABLE_ALLOC;
    (*mercd_dhal_func[MERCD_DHAL_WW_RCV_BIGMSG_DIRCOPY_FROM_KERNEL])((void *)&(dir_copy));
    Status = dir_copy.ret;

    MSD_FUNCTION_TRACE("mid_wwmgr_allocate_mem_for_bigmsgblks", ONE_PARAMETER, (size_t)pwwDev);

   //WWPENDING: Think about the SCHEME2 for linux. In SCHEME1, as soon we fail
   //allocating the mblk we are returning failure now.
   //SCHEME2 : Looks like SCHEME1 is not wholly successful, so apply SCHEME2 for thos
   //BIG blk descriptors for which SCHEME1 is failed
    if(Status == MD_FAILURE)
     {
       //mb_alloc.pwwDevi = pwwDev;
       //mb_alloc.resource = (pmerc_void_t)&(pwwDev->BigMsgRcvMemStr);
       //mb_alloc.purpose = MERCD_WW_BIGMSG_RCV_DESCR_TABLE_ALLOC;
       //(*mercd_dhal_func[MERCD_DHAL_WW_RCV_COPY_FROM_DMAABLE_BUFFERS])((pmerc_void_t)&mb_alloc);
       //Status = mb_alloc.ret;
       if (Status != MD_SUCCESS)
       {
         MSD_LEVEL2_DBGPRINT("mid_wwmgr_allocate_mem_for_bigmsgblks: RCV MSGBLK Mem Alloc Failed\n");
         Status = MD_FAILURE;
         goto out;
       }
    }

    ////////////////////////////////////////
    //2. Init the mutex for BigMsgRcvMemStr
    ////////////////////////////////////////
    MSD_INIT_MUTEX(&pwwDev->BigMsgRcvMemStr.rcvbigmsg_mutex, "Rcv Big Msg Block Mutex", NULL);

    /////////////////////////////////////////////////////////////////////////
    // 3. Allocs Memory for pBigMsgDescr table in BigMsgSndMemStr.
    // The initial number of descriptors allocated in pBigMsgDescr table
    // is equal to pwwDev->pww_param->numberBigMsgBlocks. Additional
    // number will be allocated on demand
    /////////////////////////////////////////////////////////////////////////

    numBigMsgs = pwwDev->pww_param->numberBigMsgBlocks;
    pwwDev->BigMsgSndMemStr.pbigMsgDescr = (pmercd_ww_dma_descr_sT *)
                                     MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY(numBigMsgs *
                                                           sizeof(pmercd_ww_dma_descr_sT));

#if 0
                                           mercd_allocator(numBigMsgs *
                                                sizeof(pmercd_ww_dma_descr_sT));
#endif

    if (pwwDev->BigMsgSndMemStr.pbigMsgDescr == NULL)
    {
       printk("mid_wwmgr_allocate_mem_for_bigmsgblks: SND MSGBLK Mem Alloc Failed\n");
       MSD_LEVEL2_DBGPRINT("mid_wwmgr_allocate_mem_for_bigmsgblks: SND MSGBLK Mem Alloc Failed\n");
       Status = MD_FAILURE;
       goto out;
    }

   MSD_ZERO_MEMORY(pwwDev->BigMsgSndMemStr.pbigMsgDescr, 
		numBigMsgs * sizeof(pmercd_ww_dma_descr_sT));

   //Now alloc each individual descriptor and store it at its corresponding index
   for (count = 0; count < numBigMsgs; count++)
   {
     pbigMsgDmaDescr = (pmercd_ww_dma_descr_sT)
                         MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY(sizeof(mercd_ww_dma_descr_sT));
#if 0
                                   mercd_allocator(sizeof(mercd_ww_dma_descr_sT));
#endif
     if (pbigMsgDmaDescr == NULL)
      {
  	printk("mid_wwmgr_allocate_mem_for_bigmsgblks: pwwDev->BigMsgSndMemStr.pbigMsgDescr[%d] Mem Alloc Failed\n", count);
        MSD_LEVEL2_DBGPRINT("mid_wwmgr_allocate_mem_for_bigmsgblks: pwwDev->BigMsgSndMemStr.pbigMsgDescr[%d] Mem Alloc Failed\n", count);
        Status = MD_FAILURE;
        goto out;
      }
     MSD_ZERO_MEMORY(pbigMsgDmaDescr, MERCD_WW_BIGMSG_DESCR );
     pwwDev->BigMsgSndMemStr.pbigMsgDescr[count] = pbigMsgDmaDescr;

   }

    ////////////////////////////////////////
    //4. Init the mutex for BigMsgSndMemStr
    ////////////////////////////////////////
    MSD_INIT_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex, "Snd Big Msg Block Mutex", NULL);

   ////////////////////////////////////////////////////////////////////////
   //5. Initialize the pBitMapValue for BigMsgRcvMemStr, BigMsgSndMemStr
   ////////////////////////////////////////////////////////////////////////

   szBMap = ((numBigMsgs)/(8 * sizeof(merc_uint_t)));
   if ((numBigMsgs)%((8*sizeof(merc_uint_t))))
       szBMap += 1;

   pwwDev->BigMsgRcvMemStr.pBitMapValue = (pmerc_uchar_t)
                           MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY(szBMap * sizeof(merc_uint_t));
#if 0
                                       mercd_allocator(szBMap * sizeof(merc_uint_t));
#endif
   pwwDev->BigMsgRcvMemStr.szBitMap =
             pwwDev->BigMsgRcvMemStr.avail_counter =
                     pwwDev->BigMsgRcvMemStr.free_counter = vRcvFreeDesc = numBigMsgs;
   if (pwwDev->BigMsgRcvMemStr.pBitMapValue == NULL)
    {
       printk("mid_wwmgr_allocate_mem_for_bigmsgblks: BigMsgRcvMemStr.pBitMapValue Alloc Failed\n");
       MSD_LEVEL2_DBGPRINT("mid_wwmgr_allocate_mem_for_bigmsgblks: BigMsgRcvMemStr.pBitMapValue Alloc Failed\n");
       Status = MD_FAILURE;
       goto out;
    }

   MSD_ZERO_MEMORY( pwwDev->BigMsgRcvMemStr.pBitMapValue, (szBMap * sizeof(merc_uint_t)));

   pwwDev->BigMsgSndMemStr.pBitMapValue = (pmerc_uchar_t)
                           MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY(szBMap * sizeof(merc_uint_t));

#if 0
                                       mercd_allocator(szBMap * sizeof(merc_uint_t));
#endif
   if (pwwDev->BigMsgSndMemStr.pBitMapValue == NULL)
    {
       printk("mid_wwmgr_allocate_mem_for_bigmsgblks: BigMsgSndMemStr.pBitMapValue Alloc Failed\n");
       MSD_LEVEL2_DBGPRINT("mid_wwmgr_allocate_mem_for_bigmsgblks: BigMsgSndMemStr.pBitMapValue Alloc Failed\n");
       Status = MD_FAILURE;
       goto out;
    }

   pwwDev->BigMsgSndMemStr.szBitMap =
            pwwDev->BigMsgSndMemStr.avail_counter =
               pwwDev->BigMsgSndMemStr.free_counter = vSndFreeDesc = numBigMsgs;

   MSD_ZERO_MEMORY(pwwDev->BigMsgSndMemStr.pBitMapValue, (szBMap * sizeof(merc_uint_t)));

out:
return (Status);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_generate_doorbell_to_board
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      : Generate a Door Bell intr to PLX
 *                                This is the init intr to the board
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
int mid_wwmgr_generate_doorbell_to_board(pmercd_ww_gen_doorbell_sT doorbell)
{
MD_STATUS Status;
pmercd_ww_dev_info_sT pwwDev;
#ifndef LFS
pmerc_uchar_t reg;
pmerc_uchar_t plxBase;
#endif
merc_ulong_t val;
#ifndef LFS
merc_ulong_t val1;
merc_ulong_t val2;
#endif
pmercd_adapter_block_sT padapter;
#ifndef LFS
volatile pmerc_uchar_t loc;
#endif

    Status = MD_SUCCESS;
    MSD_FUNCTION_TRACE(
         "mid_wwmgr_generate_doorbell_to_board 0x%x %s #%d\n", 
          THREE_PARAMETERS, (size_t)doorbell, __FILE__, __LINE__);
    pwwDev = doorbell->pwwDevi;
    val = doorbell->value;
    padapter = (pmercd_adapter_block_sT)pwwDev->padapter;

    if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
                   MsdWWPlxGenerateDoorBell(padapter, val)
    } else  {
      switch(padapter->phw_info->pciSubSysId)
       {
         case SUBSYSID_21554:
         case SUBSYSID_ROZETTA_21554:
                   MsdWW21554GenerateDoorBell(padapter, val)
                   break;

         case SUBSYSID_80321:
                   MsdWW80321GenerateDoorBell(padapter, val)
                   break;

         default:
                   MSD_LEVEL2_DBGPRINT(CE_WARN, "Dont know Whom to send the DB!!!!\n");
                   break;

       } /*switch*/
    }


    return(Status);
}


/***************************************************************************
 * Function Name                : mid_wwmgr_send_setwwmode_ack_to_admin
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 *
 * Calling functions            :
 * Description                  :
 * Additional comments          : This will return an ACK back to the admin
 *                              : that initiated the driver switch to WW.
 *                              : NOTE: we are sending this ack, after the
 *                              : second interrupt and WW init complete.
 *                              : Also, the type of ACK that is sent up depends
 *                              : on who initiated the driver switch to WW.
 *                              : For SRAM Family Boards, it is the qWWSetMode
 *                              : API that is initiating the driver switch.
 *                              : But, for 3rd Rock Family Boards, it is the
 *                              : qWWSetParm that is initiating the driver
 *                              : switch. We will just return the respective
 *                              : ACKs to the admin i.e MID_SET_WW_MODE_ACK
 *                              : to qWWSetMode, MID_SET_WW_PARAM_ACK to
 *                              : qWWSetParam.
 ****************************************************************************/
int
mid_wwmgr_send_setwwmode_ack_to_admin(pmercd_ww_dev_info_sT pwwDev, merc_uint_t ErrorCode)
{
 //PWW_SET_MODE           SetModePtr;
 MD_ACK*                  AckPtr;
 MD_STATUS                Status;
 merc_uint_t              ReturnValue=0;
 pmercd_open_block_sT     OpenBlockContext;
 PSTRM_MSG                Msg;
 PMDRV_MSG                MdMsg;
 pmercd_adapter_block_sT  padapter;

 MSD_FUNCTION_TRACE(
                "mid_wwmgr_send_setwwmode_ack_to_admin 0x%x 0x%x %s #%d\n", 
                 FOUR_PARAMETERS, (size_t)pwwDev, (size_t)ErrorCode,
                 __FILE__, __LINE__
                 );

 Status = MD_SUCCESS;
 padapter = (pmercd_adapter_block_sT)pwwDev->padapter;

 if (padapter->rtkMode == 0)
     ReturnValue = 0x1;
 else
     ReturnValue = 0x2;


 Msg = (PSTRM_MSG)pwwDev->private_data1;
#ifdef LFS
 OpenBlockContext = (pmercd_open_block_sT)pwwDev->private_data2;
#else
 OpenBlockContext = (PSTRM_MSG)pwwDev->private_data2;
#endif

 if ((Msg == NULL) || (OpenBlockContext == NULL))
   {                                                           
    Status = MD_FAILURE;      
    return(Status);  
   }                       

 MdMsg = MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
 if (OpenBlockContext->state == MERCD_OPEN_STATE_CLOSE_PEND) {
    MSD_LEVEL2_DBGPRINT(
          "mid_wwmgr_send_setwwmode_ack_to_admin: 0x%x %s #%d\n",
           OpenBlockContext->state,
           __FILE__,
           __LINE__
          );

    Status = MD_FAILURE;
    return(Status);
  }

 //MdMsg->MessageId = MID_SET_WW_MODE_ACK;
 if (ErrorCode == MD_OK) {
    //SetModePtr = (PWW_SET_MODE)MD_GET_MDMSG_PAYLOAD(MdMsg);
    //AckPtr = (MD_ACK*)SetModePtr;
    //As usual we send the Ack in the payload after MDRV_MSG.
    //This is the same for set_mode and set_parm.
    AckPtr = (MD_ACK*)MD_GET_MDMSG_PAYLOAD(MdMsg);
    AckPtr->ErrorCode = ErrorCode;
    AckPtr->MessageId = MdMsg->MessageId;       /* MID_SET_WW_MODE_ACK or
                                                   MID_SET_WW_PARAM_ACK */
    AckPtr->ReturnValue = ReturnValue;
    MD_SET_MSG_WRITE_PTR(Msg,MD_GET_MSG_READ_PTR(Msg)+
    sizeof(MDRV_MSG)+sizeof(MD_ACK));
#if defined LiS || defined LIS || defined LFS
    supp_process_sync_receive(OpenBlockContext, Msg);
#else
    // DMV-B
    if ((padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DMVB) &&
        (padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_NEWB)) {
        //printk("2");qDrvPrintMsgPoolStatus();
        supp_process_receive(OpenBlockContext, Msg);
    } else {
        if (supp_snd_cfg_rply(padapter, MERC_HOSTIF_CFG_OK) != MD_SUCCESS) {
            printk("WW: setwwmode SendCfgReply Failed\n");
        }
        MSD_FREE_MESSAGE(Msg); 
    }
#endif
 }
 else
  {
   // send back an error/reply message
   Msg = supp_drvr_mk_ack(Msg, MD_GET_MDMSG_ID(MdMsg), ErrorCode, ReturnValue);
#if defined LiS || defined LIS || defined LFS
   if(MD_IS_MSG_ASYNC(Msg))
      supp_process_receive(OpenBlockContext, Msg);
   else
      supp_process_sync_receive(OpenBlockContext, Msg);
#else
   supp_process_receive(OpenBlockContext, Msg);
#endif

  }

 //Clear private data for future reuse.
 pwwDev->private_data1 = NULL;
 pwwDev->private_data2 = NULL;

return(Status);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_post_init_intr_processing
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 * Additional comments          : Based on the Struct:
 * typedef struct _MSD_PROBE_BOARD_CONFIG {
 *       merc_ushort_t  BoardType;
 *       merc_ushort_t  PLXVenId;
 *       merc_ushort_t  PLXDevId;
 *       merc_ushort_t  SubId;
 *       merc_ushort_t  SubVenId;
 *       merc_uint_t    VendorId;
 *       merc_uint_t    SecondaryVendorId;
 *       merc_uint_t    PrimaryBoardId;
 *       merc_ushort_t  pci_reg_prop_num;
 *       merc_ushort_t  pci_reg_prop_sram;
 *       merc_ushort_t  pci_assignedaddr_prop_sram;
 *       merc_ushort_t  pci_assignedaddr_prop_num;
 * } MSD_PROBE_BOARD_CONFIG, *PMSD_PROBE_BOARD_CONFIG;
 ****************************************************************************/
int mid_wwmgr_post_init_intr_processing(pmercd_ww_dev_info_sT pwwDev)
{
MD_STATUS Status;
pmercd_adapter_block_sT padapter;
#ifndef LFS
merc_int_t RegPropLen, AssignedPropLen;
merc_uint_t *RegArr, *AssignedAddrs;
mercd_osal_mem_virt_free_sT  meminfo1 = { 0 };
pmercd_virt_map_sT  pvirt_map;
mercd_osal_mem_phys_virt_sT meminfo = { 0 } ;
pmerc_uchar_t BridgeBase;
#endif
mercd_ww_gen_doorbell_sT doorbell;
mercd_dhal_ww_switch_plxmap_sT switchplx = { 0 };

    MSD_FUNCTION_TRACE(
                        "mid_wwmgr_post_init_intr_processing 0x%x %s #%d\n", 
                        THREE_PARAMETERS, 
                        (size_t)pwwDev,
                        __FILE__, __LINE__
                      );

    Status = MD_SUCCESS;

    padapter = (pmercd_adapter_block_sT)pwwDev->padapter;

   //Increment the Counter
   //atomic_inc(&pwwDev->pww_counters->init_intr_db_response);


   //We heard from the board, so change the state.
        pwwDev->state = MERCD_ADAPTER_WW_MODE_INIT_INTR_ACKED;


   if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
     //Switch the mappings for SRAM Family of boards, where the mapped
     //space at BAR3 takes effect in WW mode. We mapped BAR3 in
     //WWSetMode, even before switching the drvier to WW mode. We dont have
     //to do this for 3rd rock family of boards, since there are only
     //two mappings, BAR0 for PPB and BAR2 for SRAM.

     //    Free the mappings of plx in SRAM mode and place the new mappings
     //    of plx (from BAR3) at  padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]
     //    from pwwDev->plx_space_tree. SRAM mappings remain the same

      switchplx.pwwDevi = pwwDev;
      switchplx.ConfigId = padapter->adapternumber;
      (*mercd_dhal_func[MERCD_DHAL_WW_SWITCH_PLXMAP])((pmerc_void_t)&switchplx);
    }/* THIRD_ROCK_FAMILY */

   //NOTE: The offsets of inBoundQ reg and outBoundQ reg
   //are the same for all the bridges, Plx, 21554, 80321

   //Store the inBoundQRegAddress, outBoundQRegAddress in pwwDev
   pwwDev->inBoundQRegAddress =
                                      (pmerc_void_t)(((pmercd_virt_map_sT)(padapter->phw_info->
                                      virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr +
                                      MERCD_WW_INBOUND_QUEUE_REGISTER_ADDRESS);
   pwwDev->outBoundQRegAddress =
                                       (pmerc_void_t)(((pmercd_virt_map_sT)(padapter->phw_info->
                                       virt_map_q[MERCD_PCI_BRIDGE_MAP]))->virt_addr +
                                       MERCD_WW_OUTBOUND_QUEUE_REGISTER_ADDRESS);

   //Send the Initialization MF
   Status = mid_wwmgr_send_init_mf_to_board(pwwDev);

   if (Status != MD_SUCCESS)
        {
         cmn_err(CE_CONT, "mid_wwmgr_post_init_intr_processing FAILED\n");
         MSD_LEVEL2_DBGPRINT("Init MF Failed %s #%d\n", __FILE__, __LINE__);
         pwwDev->state = MERCD_ADAPTER_WW_FAILED;
         Status = MD_FAILURE;
         return(Status);
        }

   //Set the pwwDev->state
   pwwDev->state = MERCD_ADAPTER_WW_MODE_INIT_MSG_READY_INTR_PENDING;

   // Kick off our timer also.....
   time_ww_chk_snd_timer(padapter);

   //Send the Init Msg Rdy Interrupt to the Board
   doorbell.pwwDevi = pwwDev;
   doorbell.value = MERCD_WW_INIT_MSG_READY;

   Status = mid_wwmgr_generate_doorbell_to_board(&doorbell);
   //atomic_inc(&pwwDev->pww_counters->init_msg_ready_intr_db);

 return(Status);
}

/*****************************************************************************
 * Function Name                : mid_wwmgr_post_init_msg_ready_intr_processing
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      : stop WW mode timers
 * Calling functions            :
 * Description                  :
 * Additional comments          : This is called when we hear back from the board
 *                                for MERCD_WW_INIT_MSG_READY that was sent to
 *                                board earlier. We are in
 *                                MERCD_ADAPTER_WW_MODE_INIT_MSG_READY_INTR_PENDING
 *                                state.
 ****************************************************************************/
int mid_wwmgr_post_init_msg_ready_intr_processing(pmercd_ww_dev_info_sT pwwDev)
{

 MD_STATUS                                Status;
 pmercd_adapter_block_sT                padapter;
#ifndef LFS
 pmerc_uchar_t                           plxBase;
 mercd_ww_gen_doorbell_sT               doorbell;
 mercd_osal_timeout_start_sT timeoutinfo = { 0 };
 pmercd_open_block_sT           OpenBlockContext;
 PSTRM_MSG                                   Msg;
#endif
 merc_uint_t                           ErrorCode;
#ifdef LFS
 volatile pmerc_ulong_t         	     loc;
#else
 volatile pmerc_uchar_t         	     loc;
#endif

    MSD_FUNCTION_TRACE(
              "mid_wwmgr_post_init_msg_ready_intr_processing 0x%x %s #%d\n", 
               THREE_PARAMETERS, (size_t)pwwDev,
               __FILE__,
               __LINE__
               );

    //printk("mid_wwmgr_post_init_msg_ready_intr_processing\n"); 

    Status = MD_SUCCESS;
    padapter = (pmercd_adapter_block_sT)pwwDev->padapter;

    //Increment the Counter
    //atomic_inc(&pwwDev->pww_counters->init_msg_ready_intr_db_response);

    // HCS Support
    if (HCS_Flag) {
	if (padapter->phw_info->boardFamilyType == THIRD_ROCK_FAMILY) {
	    MSD_LEVEL2_DBGPRINT("HCS: Init 3rdRock offset %#x w/ %d\n", MERCURY_HOST_IF_HCS_INIT_21554, MERCURY_HOST_IF_HCS);
	    loc = (volatile pmerc_ulong_t)((pmercd_virt_map_sT)
		  (padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+MERCURY_HOST_IF_HCS_INIT_21554));
	    writel(MERCURY_HOST_IF_HCS, loc);
	} else if ((padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_DMVB) ||
                   (padapter->phw_info->pciSubSysId == PCI_SUBDEVICE_ID_NEWB)) {
	    MSD_LEVEL2_DBGPRINT("HCS: Init DMVB offset %#x w/ %d\n", MERCURY_HOST_IF_HCS_INIT_ON_PLX, MERCURY_HOST_IF_HCS);
	    loc = (volatile pmerc_ulong_t)((pmercd_virt_map_sT)
		  (padapter->phw_info->virt_map_q[MERCD_PCI_BRIDGE_MAP]->virt_addr+MERCURY_HOST_IF_HCS_INIT_ON_PLX));
	    writel(MERCURY_HOST_IF_HCS, loc);
	}
    }

    //1. We heard from the board, so change the state
    pwwDev->state = MERCD_ADAPTER_WW_MODE_INIT_MSG_READY_INTR_ACKED;
    Status = msgutl_ww_build_and_send_msgblk_mf(pwwDev);

   if (Status != MD_SUCCESS)
   {
    printk("mid_wwmgr_post_init_msg_ready_intr_processing: init MSG Blk FAILED\n");
    MSD_LEVEL2_DBGPRINT("Init MSG Blk MF Failed %s #%d\n", __FILE__, __LINE__);
    pwwDev->state = MERCD_ADAPTER_WW_FAILED;
    Status = MD_FAILURE;
    return(Status);
   }

   pwwDev->state = MERCD_ADAPTER_WW_SUCCEDED;


  //We need to send an ack to the admin component that sent the
  //qWWSetMode. In mid_wwmgr_set_mode after calling
  //mid_wwmgr_configure_brd_to_ww, so far we have not returned
  //, and now is the time to send an ack back.
  ErrorCode = MD_OK;
  Status = mid_wwmgr_send_setwwmode_ack_to_admin(pwwDev, ErrorCode);

  if (Status != MD_SUCCESS) {
   cmn_err(CE_WARN, "mid_wwmgr_post_init_msg_ready_intr_processing : Ack to Admin failed\n");
   MSD_LEVEL2_DBGPRINT("Ack To Admin Failed %s #%d\n", __FILE__, __LINE__);
   pwwDev->state = MERCD_ADAPTER_WW_FAILED;
   return (Status);
  }

 return(Status);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_send_init_mf_to_board
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 *
 * Calling functions            :
 * Description                  :
 * Additional comments          :
 ****************************************************************************/
int mid_wwmgr_send_init_mf_to_board(pmercd_ww_dev_info_sT pwwDev)
{
MD_STATUS Status;
#ifndef LFS
merc_ww_init_mf_sT init_mf;
#endif
merc_ulong_t physOffset;
pmerc_uchar_t pmfAddress;
#ifndef LFS
pmerc_uchar_t plxBase;
#endif
merc_ulong_t inBoundQphysAddress;
merc_ulong_t outBoundQPhysicalAddress;
pmercd_adapter_block_sT padapter;
merc_long_t mercFifoEof;
merc_uint_t index;
merc_ulong_t remapReg;
#ifndef LFS
pmerc_ulong_t loc;
#endif
mercd_ww_build_initMF_sT initMfSt;


 MSD_FUNCTION_TRACE(
         "mid_wwmgr_send_init_mf_to_board 0x%x %s #%d\n", 
          THREE_PARAMETERS, 
          (size_t)pwwDev, __FILE__, __LINE__
          );

 Status = MD_SUCCESS;
 padapter = (pmercd_adapter_block_sT)pwwDev->padapter;

 //Check if we have a mapped inBoundQMfAddress
 if (pwwDev->inBoundQMfAddress == NULL)
 {
     MSD_LEVEL2_DBGPRINT
      (
        "mid_wwmgr_send_init_mf_to_board:  inBoundQMfAddressDescr NULL %s #%d\n",
         __FILE__, __LINE__
      );
     Status = MD_FAILURE;
     return(Status);
 }

 //Build the init MF..we use MF index 0
      pmfAddress = pwwDev->inBoundQMfAddress;
      physOffset = (pwwDev->pww_param->numberMFAInboundQ *
                    pwwDev->pww_param->sizeofMFAInboundQ );
      inBoundQphysAddress = (size_t)pwwDev->mfaBaseAddressDescr.board_address;



#ifdef LFS
      outBoundQPhysicalAddress = /* inBoundQ Phys Addr + Offset */
           (merc_ulong_t)(inBoundQphysAddress + physOffset);
#else
      outBoundQPhysicalAddress = /* inBoundQ Phys Addr + Offset */
           (pmerc_uchar_t)(inBoundQphysAddress + physOffset);
#endif


      initMfSt.pwwDevi = pwwDev;
      initMfSt.pmfAddress = pmfAddress;
      initMfSt.outBoundQPhysAddr = outBoundQPhysicalAddress;

      Status = msgutl_ww_build_init_mf(&initMfSt);

 //Tell PLX about this  init MF by sending physical address of the
 //inBound MF to the inBoundQRegAddress. Do two 16bit writes,
 //b'cos PLX can only handle 16bit reads at a time on inBound Q.

#ifdef i386
     MsdWWPutinBoundQReg(padapter, inBoundQphysAddress);
     MsdWWPutinBoundQReg(padapter, inBoundQphysAddress>>16);

     //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
     //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
#else


     MsdWWPutinBoundQReg(padapter, inBoundQphysAddress>>16);
     MsdWWPutinBoundQReg(padapter, inBoundQphysAddress);

     //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
     //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
#endif


 //Perform the inBoundQ  FIFO initialization. Leave index=0,
 //since it is our init MF
      mercFifoEof = 0xFFFFFFFF;

      for (index = 1; index < pwwDev->pww_param->numberMFAInboundQ; index++)
       {
         //Write the index into inBoundQRegAddress
        MsdWWPutinBoundQReg(padapter, index);
        //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
       }

 //Initialize the next 4 enteries in the FIFO as -1. This will tell the kernel to stop.
        for (index = 0;index <4;index++)
        {
        MsdWWPutinBoundQReg(padapter, mercFifoEof);
        //atomic_inc(&pwwDev->pww_counters->total_inboundq_writes);
        }

 //Perform the outBoundQ  FIFO initialization
      for (index = 0; index < pwwDev->pww_param->numberMFAOutboundQ; index++)
       {
         //Write the index into outBoundQRegAddress
        MsdWWPutoutBoundQReg(padapter, index);
        //atomic_inc(&pwwDev->pww_counters->total_outboundq_writes);
       }

 //Now write the 2 MSBs of the inBoundQ MF Physical Address to the Re-Map Register
 //Only for SRAM Family Boards only. For THIRD_ROCK Family Boards, there is no
 //Remap Register
  if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
      remapReg = MsdWWPlxGetRemapRegister(padapter);
      remapReg = remapReg & 0x0000FFFF;
      remapReg |= (inBoundQphysAddress & 0xFFFF0000);
      MsdWWPlxPutRemapRegister(padapter, remapReg);
     }
return (Status);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_shutdown_adapter
 * Function Type                : WW manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : WW specific shutdown
 * Additional comments          :
 ****************************************************************************/
int mid_wwmgr_shutdown_adapter(pmercd_ww_dev_info_sT pwwDev, merc_uint_t initiator)
{
	MD_STATUS                      Status;
#ifndef LFS
	merc_uint_t		       break_cnt = 0;
#endif
	pmercd_adapter_block_sT        padapter;
        mercd_dhal_ww_switch_plxmap_sT switchplx = { 0 };
#ifndef LFS
	volatile pmerc_uchar_t         loc;
#endif


	Status = MD_PENDING;

	padapter = pwwDev->padapter;

	//Cancel all pending requests in WW mode, and release their resources:
	//1. Messaging: The big messages that are waiting for replies
	//2. Streaming: The stream data on Q waiting to be released.
	//The could be like:
	//   - send down the CENACEL interrupt doorbell to the board
	//   - Board sends back CANCEL interrupt doorbell ack.
	//Then we go ahead and remove all the host specific WW memory
	//If not then we free our memory after waiting 5 seconds

	if (initiator == 0) {
	    if (padapter->phw_info->boardFamilyType != THIRD_ROCK_FAMILY) {
	        //Send Cancel Interrupt doorbell to tell the board not to access host memory
	        MsdWWPlxGenerateDoorBell(padapter, MERCD_WW_CANCEL_REQUESTS_INTERRUPT);

	        switchplx.pwwDevi = padapter->pww_info;
                switchplx.ConfigId = padapter->adapternumber;

                (*mercd_dhal_func[MERCD_DHAL_WW_SWITCH_PLXMAP])((pmerc_void_t)&switchplx);
             } else {
                /* This is a Third rock board. Find out whether we have a
                 * Yavapai or the 21554/21555 */
                switch(padapter->phw_info->pciSubSysId) {
                    case SUBSYSID_21554:
                    case SUBSYSID_ROZETTA_21554:
                        MsdWW21554GenerateDoorBell(padapter,MERCD_WW_CANCEL_REQUESTS_INTERRUPT);
                        break;
                    case SUBSYSID_80321:
                        MsdWW80321GenerateDoorBell(padapter,MERCD_WW_CANCEL_REQUESTS_INTERRUPT);
                        break;
                    default:
                        break;
                }

             }

	     // We can no longer use a schedule timeout in here. What we
	     // will do is keep the timer running and check for this state
	     // in the timer. In the timer, we will keep a track of this
             // state and if we do not get back the cancel interrupt ack
             // within say 5 seconds we will simulate the cancel ack

        }

	return (Status);
}

/***************************************************************************
 * Function Name                : msd_wwmgr_process_reset_interrupt_from_board
 * Function Type                : WW manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  : Board Generated a Reset Interrupt
 * Additional comments          :
 ****************************************************************************/
int mid_wwmgr_process_reset_interrupt_from_board(pmercd_ww_dev_info_sT pwwDev)
{
	 MD_STATUS                      Status;

	 Status = MD_SUCCESS;
	 //Deallocate all the WW specific resources. If it is 1.9 HW
	 //put the board back to WW mode of operation ( not implemented
	 // yet ). Cancel all pending requests for messaging and streaming.

	 Status = mid_wwmgr_shutdown_adapter(pwwDev, 1);

	 return (Status);
}

/*********************************************************************************
 * Function Name         : msd_wwmgr_process_cancel_request_intr_ack_from_board
 * Function Type         : WW manager function
 * Inputs                :
 * Outputs               :
 * Calling functions     :
 * Description           : Board Acks Back for CANCEL REQUEST
 * Additional comments   :
 **********************************************************************************/
int mid_wwmgr_process_cancel_request_intr_ack_from_board(pmercd_ww_dev_info_sT pwwDev)
{
	mercd_osal_timeout_stop_sT         timeoutinfo = { 0 };
#ifndef LFS
	mercd_osal_dpc_register_sT         dpc_dereginfo = { 0 };
	mercd_dhal_intr_disable_sT         intr_disableinfo = { 0 };
	mercd_dhal_intr_enable_sT          intrinfo = { 0 };
	mercd_osal_intr_deregister_sT      intr_deregisterinfo = { 0 };
        mercd_osal_intr_register_sT        intrinfo_reg = { 0 };
	merc_uint_t                        ret;
#endif
	pmercd_adapter_block_sT            padapter;
	MD_STATUS                          Status;
	mercd_dhal_ww_dealloc_dmadescr_sT descrFree;
        merc_uint_t                        cnt0 = 0;
#ifndef LFS
        merc_uint_t                        cnt1 = 0;
#endif
	merc_uint_t                        szBMap = 0;
	merc_uint_t                        count= 0;



	MSD_FUNCTION_TRACE(
	"msd_wwmgr_process_cancel_request_intr_ack_from_board 0x%x %s #%d\n",
	ONE_PARAMETER, (size_t)pwwDev
	);

	Status = MD_SUCCESS;

	padapter = pwwDev->padapter;

	if (padapter->flags.WWFlags  & MERCD_ADAPTER_WW_MODE_NOTENABLED) {
	    printk("WW: received cancel ack after simulation \n");
	    return (MD_SUCCESS);
        }

	padapter->rtkMode = 0;

	//Soft reset the adapter
	reset_adapter(padapter);


	//Stop WW mode timers
	if (padapter->flags.SendTimeoutPending & MERC_ADAPTER_FLAG_SEND_TIMEOUT_PEND)
	{
#if defined LiS || defined LIS || defined LFS
	  timeoutinfo.Handle = &padapter->phw_info->timer_info->timeout_id;
#else
	  timeoutinfo.Handle = (struct timer_list *)&padapter->phw_info->timer_info->timeout_id;
#endif /* LiS */
	  (*mercd_osal_func[MERCD_OSAL_TIMEOUT_STOP])((void *)&timeoutinfo);
	  padapter->flags.SendTimeoutPending = 0;
	  if (timeoutinfo.ret != MD_SUCCESS)
	   {
	     MSD_LEVEL2_DBGPRINT("SRAM timesr stop Failed %s #%d\n", __FILE__, __LINE__);
	     MSD_ENTER_MUTEX(&padapter->adapter_block_mutex);
	     padapter->state = MERCD_ADAPTER_STATE_OUT_OF_SERVICE;
	     MSD_EXIT_MUTEX(&padapter->adapter_block_mutex);
	     return(MD_FAILURE);
	   }
	} /* SendTimeoutPending */


	//Free the perf counters
	if (pwwDev->pww_counters)
        {
         //printk("CancelRequest: Bef free1..\n");
	 mercd_free(pwwDev->pww_counters, MERCD_WW_PERF_COUNTERS, MERCD_FORCE);
         //printk("CancelRequest: Aft free1..\n");
        }
	pwwDev->pww_counters = NULL;

	//Free the MF Mem Pool
	(*mercd_dhal_func[MERCD_DHAL_WW_MF_DEALLOC])((pmerc_void_t)pwwDev);


	//Free the Memory for the Rcv BIG Msg Blocks. While allocating we
	//always tried to allocate streams buffers. If it is not successful,
	//we just allocated a DMAble memory space. Hence need to take
	//care of both.
	descrFree.pwwDevi = pwwDev;
	descrFree.resource = pwwDev->BigMsgRcvMemStr.pbigMsgDescr;
        descrFree.purpose = 0;
        descrFree.extradata = 0;
	(*mercd_dhal_func[MERCD_DHAL_WW_DEALLOC_DESC_TABLE])((pmerc_void_t)&descrFree);

	//Free the memory for RCV BIGMSG descriptor table's bitmap
        if (pwwDev->BigMsgRcvMemStr.pBitMapValue)
         {
           //printk("CancelRequest: Bef free2..\n");
	   mercd_free(pwwDev->BigMsgRcvMemStr.pBitMapValue,
	           pwwDev->BigMsgRcvMemStr.szBitMap,
	           MERCD_FORCE);
           //printk("CancelRequest: Aft free2..\n");
         }
	pwwDev->BigMsgRcvMemStr.pBitMapValue = NULL;
	pwwDev->BigMsgRcvMemStr.szBitMap = 0;
	pwwDev->BigMsgRcvMemStr.avail_counter = 0;
	pwwDev->BigMsgRcvMemStr.free_counter = 0;
	MSD_DESTROY_MUTEX(&pwwDev->BigMsgRcvMemStr.rcvbigmsg_mutex);

	//WWPENDING: Similarly deallocate the Memory for Snd BIG Msg Blocks
	descrFree.pwwDevi = pwwDev;
	descrFree.resource = pwwDev->BigMsgSndMemStr.pbigMsgDescr;
        descrFree.purpose = 0;
        descrFree.extradata = 0;
	(*mercd_dhal_func[MERCD_DHAL_WW_DEALLOC_DESC_TABLE])((pmerc_void_t)&descrFree);

	//Free the memory for SND BIGMSG descriptor table's bitmap
        if (pwwDev->BigMsgSndMemStr.pBitMapValue)
        {
         //printk("CancelRequest: Bef free3..\n");
	 mercd_free(pwwDev->BigMsgSndMemStr.pBitMapValue,
	           pwwDev->BigMsgSndMemStr.szBitMap,
	           MERCD_FORCE);
         //printk("CancelRequest: Aft free3..\n");
        }

	pwwDev->BigMsgSndMemStr.pBitMapValue = NULL;
	pwwDev->BigMsgSndMemStr.szBitMap = 0;
	pwwDev->BigMsgSndMemStr.avail_counter = 0;
	pwwDev->BigMsgSndMemStr.free_counter = 0;
	pwwDev->BigMsgSndMemStr.next= NULL;
	MSD_DESTROY_MUTEX(&pwwDev->BigMsgSndMemStr.sndbigmsg_mutex);

        ////////////////////////////////////////////////////////////
        //Dealloc the pending messages on padapter's snd_msg_array
        {
         PSTRM_MSG Msg = 0;
         while((Msg = queue_get_msg_Array(&padapter->snd_msg_Array, MSD_SENDER)) != NULL)
          {
            MSD_FREE_MESSAGE(Msg);
            queue_commit_msg_Array(&padapter->snd_msg_Array, MSD_SENDER);
          }
          padapter->snd_msg_Array.head = 0;
          padapter->snd_msg_Array.tail = 0;
        }
        ////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////
        //Dealloc the memory specific to streaming
        for (cnt0 = 0; cnt0< MSD_MAX_STREAM_ID; ++cnt0)
        {
          pmercd_stream_connection_sT StreamBlock = 0;
          PSTRM_MSG Msg = 0;
          if(( StreamBlock = padapter->pstream_connection_list[cnt0]) != NULL) 
            {
              if (StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY)
              {
                while((Msg = StreamBlock->SendStreamQueue) != NULL)
                 {
                   StreamBlock->SendStreamQueue = Msg->b_next;
                   //printk("CancelRequest: Bef free5..\n");
                   MSD_FREE_MESSAGE(Msg);
                   //printk("CancelRequest: Aft free5..\n");
                 }
                MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);
                queue_remove_from_Q(&padapter->stream_adapter_list,
                              (PMSD_GEN_MSG)StreamBlock);
                padapter->pstream_connection_list[cnt0] = NULL;
                MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
                if( StreamBlock->pbind_block )
                  StreamBlock->pbind_block->stream_connection_ptr = NULL;

                ////////////////////////////////////////
                //Free the Write Side Descriptors
                ////////////////////////////////////////
                //Free the StreamBlock DMA Descriptors and
                //its index table
                if (StreamBlock->pSendStrmDmaDesc) 
                {
	         descrFree.pwwDevi = pwwDev;
	         descrFree.resource = StreamBlock->pSendStrmDmaDesc;
                 descrFree.purpose = MERCD_WW_STREAM_DESCR_DEALLOC;
                 descrFree.extradata = (pmerc_void_t)StreamBlock;
	         (*mercd_dhal_func[MERCD_DHAL_WW_DEALLOC_DESC_TABLE])
                                         ((pmerc_void_t)&descrFree);
		 //Just to Make sure
		 StreamBlock->pSendStrmDmaDesc = NULL;
                }

                //Free the SND DMA Descriptor Bit Map
                {
                  szBMap = ((StreamBlock->szSendBitMap)/(8 * sizeof(merc_uint_t)));
                  if ((StreamBlock->szSendBitMap)%((8*sizeof(merc_uint_t))))
                    szBMap += 1;
                     //printk("CancelRequest: Bef free6..\n");
                    if(StreamBlock->pSendBitMapValue)
                      mercd_free(StreamBlock->pSendBitMapValue,
                         (szBMap * sizeof(merc_uint_t)), MERCD_FORCE);
                     //printk("CancelRequest: Aft free6..\n");
		     StreamBlock->pSendBitMapValue = NULL;
                 }
             
              //Destroy the mutex
              MSD_DESTROY_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex);
             }
            else
             { 
                /* STREAM_OPEN_F_RECEIVE_ONLY */

                ////////////////////////////////////////
                //Free the Read Side Descriptors
                ////////////////////////////////////////
                MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);
                queue_remove_from_Q(&padapter->stream_adapter_list,
                              (PMSD_GEN_MSG)StreamBlock);
                padapter->pstream_connection_list[cnt0] = NULL;
                MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
                if( StreamBlock->pbind_block )
                   StreamBlock->pbind_block->stream_connection_ptr = NULL;

                if (StreamBlock->pRcvStrmDmaDesc)
                 {
                   descrFree.pwwDevi = pwwDev;
                   descrFree.resource = StreamBlock->pRcvStrmDmaDesc;
                   descrFree.purpose = MERCD_WW_STREAM_DESCR_DEALLOC;
                   descrFree.extradata = (pmerc_void_t)StreamBlock;
                   (*mercd_dhal_func[MERCD_DHAL_WW_DEALLOC_DESC_TABLE])
                                         ((pmerc_void_t)&descrFree);
		   StreamBlock->pRcvStrmDmaDesc = NULL;
                 }

                //Free the Rcv DMA Descriptor Bit Map
                 {
                   szBMap = ((StreamBlock->szRcvBitMap)/(8 * sizeof(merc_uint_t)));
                   if ((StreamBlock->szRcvBitMap)%((8*sizeof(merc_uint_t))))
                      szBMap += 1;
                    //printk("CancelRequest: Bef free8..\n");
                   if(StreamBlock->pRcvBitMapValue) {
                    mercd_free(StreamBlock->pRcvBitMapValue,
                         (szBMap * sizeof(merc_uint_t)), MERCD_FORCE);
                    //printk("CancelRequest: Aft free8..\n");
		    StreamBlock->pRcvBitMapValue = NULL;
		   }
                 }

                //printk("CancelRequest: Bef free9..\n");
                if (StreamBlock->pWWReadEosMsg) {
                     MSD_FREE_MESSAGE(StreamBlock->pWWReadEosMsg);
 		     StreamBlock->pWWReadEosMsg = NULL;
                }
                //printk("CancelRequest: Aft freeA..\n");

                MSD_DESTROY_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex);
              }
              
	      MSD_DESTROY_MUTEX(&StreamBlock->stream_block_global_mutex);
              // free the stream block memory space
              //printk("CancelRequest: Bef freeB..\n");
              mercd_free(StreamBlock, 
                         sizeof(mercd_stream_connection_sT),
                         MERCD_FORCE);
              //printk("CancelRequest: Aft freeB..\n");
            } /* StreamBlock not NULL */
        } /* cnt */
        ////////////////////////////////////////////////////////////
	
        //////////////////////////////////////////////////////////
        //Free the other pWWDev Specific bitmaps for Streaming
        //
        ///////////////////////////////////////////////////////////

        //Free global BitMap for the BindBlocks having pending 
        //cancel Msg MF request
        if (pwwDev->pPendBindCancelMsgMFBitMap)
         {
           szBMap = ((pwwDev->szPendBindCancelMsgMFBitMap)/(8 * sizeof(merc_uint_t)));
           if ((pwwDev->szPendBindCancelMsgMFBitMap)%((8*sizeof(merc_uint_t))))
              szBMap += 1;

            //printk("CancelRequest: Bef freeC..\n");
            mercd_free(pwwDev->pPendBindCancelMsgMFBitMap,
                       ((szBMap) * (sizeof(merc_uint_t))),
                        MERCD_FORCE);
	    pwwDev->pPendBindCancelMsgMFBitMap = NULL;
            //printk("CancelRequest: Aft freeC..\n");
         }

        //Free global BitMap for the BindBlocks having pending 
        //cancel Str MF request
        if (pwwDev->pPendBindCancelStrMFBitMap)
         {

           szBMap = ((pwwDev->szPendBindCancelStrMFBitMap)/(8 * sizeof(merc_uint_t)));
           if ((pwwDev->szPendBindCancelStrMFBitMap)%((8*sizeof(merc_uint_t))))
              szBMap += 1;
            //printk("CancelRequest: Bef freeD..\n");
            mercd_free(pwwDev->pPendBindCancelStrMFBitMap,
                       ((szBMap) * (sizeof(merc_uint_t))),
                        MERCD_FORCE);
	    pwwDev->pPendBindCancelStrMFBitMap = NULL;
            //printk("CancelRequest: Aft freeD..\n");
         }

        //Free BitMap that gives us the pending messages to board per 
        //BindBlock
        if (pwwDev->pPendMsgBitMap)
        {
           pmerc_uchar_t ptr = 0; /****/
           szBMap = ((pwwDev->pww_param->numberBigMsgBlocks)/(8 * sizeof(merc_uint_t)));
           if ((pwwDev->pww_param->numberBigMsgBlocks)%(8 * sizeof(merc_uint_t)))
             szBMap += 1;
           for (count = 0; count < pwwDev->szPendBindCancelMsgMFBitMap; count++)
           {
             ptr = pwwDev->pPendMsgBitMap[count];
   
             //printk("CancelRequest: Bef freeE..\n");
             if (ptr) {
               mercd_free(ptr, (szBMap * sizeof(merc_uint_t)), MERCD_FORCE);
              }
	     pwwDev->pPendMsgBitMap[count] = NULL;
            //printk("CancelRequest: Aft freeE..\n");
           }
          
           //printk("CancelRequest: Bef freeF..\n");
           if (pwwDev->pPendMsgBitMap)
               mercd_free(pwwDev->pPendMsgBitMap, (MsdControlBlock->maxbind) *
                     (sizeof(pmerc_uchar_t)), MERCD_FORCE);

	   pwwDev->pPendMsgBitMap = NULL;
           //printk("CancelRequest: Aft freeF..\n");
        }

        //Free global bit for all BindBlocks with cancel Msg MF pending
        if(pwwDev->pPendFlushStrBitMap)
        {
           szBMap = ((pwwDev->szPendFlushStrBitMap)/(8 * sizeof(merc_uint_t)));
           if ((pwwDev->szPendFlushStrBitMap)%(8 * sizeof(merc_uint_t)))
              szBMap+=1;
 
           //printk("CancelRequest: Bef free10..\n");
           mercd_free(pwwDev->pPendFlushStrBitMap,
                      ((szBMap)*(sizeof(merc_uint_t))),
                      MERCD_FORCE); 
	   pwwDev->pPendFlushStrBitMap = NULL;
           //printk("CancelRequest: Aft free10..\n");
        }

        //Free the global BitMap for Receive StreamBlocks with Pending
        if (pwwDev->pPendReqRcvStrDataBufBitMap)
         {
            szBMap = ((pwwDev->szPendReqRcvStrDataBufBitMap)/(8 * sizeof(merc_uint_t)));
            if ((pwwDev->szPendReqRcvStrDataBufBitMap)%(8 * sizeof(merc_uint_t)))
               szBMap+=1;

           //printk("CancelRequest: Bef free11..\n");
            mercd_free(pwwDev->pPendReqRcvStrDataBufBitMap, 
                       (szBMap) * (sizeof(merc_uint_t)),
                       MERCD_FORCE);
	    pwwDev->pPendReqRcvStrDataBufBitMap = NULL;
           //printk("CancelRequest: Aft free11..\n");
         }
	
	//Destroy other global mutexes
        MSD_DESTROY_MUTEX(&pwwDev->ww_msgpendq_mutex);
        MSD_DESTROY_MUTEX(&pwwDev->ww_iboundQ_mutex);
        MSD_DESTROY_MUTEX(&pwwDev->ww_oboundQ_mutex);
        MSD_DESTROY_MUTEX(&pwwDev->ww_eospendq_mutex);
	////////////////////////////////////////////////////////////

	//Change the State to WW Not Enabled, WWFlags should still have
	//the MERCD_ADAPTER_WW_I20_MESSAGING_READY bit set for 3rdRock. 

	//We are leaving the adapter in Mapped State
        padapter->flags.WWFlags |= MERCD_ADAPTER_WW_MODE_NOTENABLED;
  	pwwDev->state = MERCD_ADAPTER_WW_IN_SHUTDOWN;
	padapter->state = MERCD_ADAPTER_STATE_INIT;

return (Status);
}

/*********************************************************************************
 * Function Name         : msd_wwmgr_process_cancel_request_intr_ack_from_board
 * Function Type         : WW manager function
 * Inputs                :
 * Outputs               :
 * Calling functions     :
 * Description           : Board Acks Back for CANCEL REQUEST
 * Additional comments   :
 **********************************************************************************/
int mid_wwmgr_process_error_interrupt_from_board(pmercd_ww_dev_info_sT pwwDev) 
{
   merc_int_t		   i;
   pmercd_adapter_block_sT padapter;

   padapter = pwwDev->padapter;
 
   if (padapter->panicDump == NULL)
       mercd_zalloc(padapter->panicDump, pmerc_uchar_t, MD_PANIC_DUMP_MAX_SIZE);

   if (!padapter->panicDump) {
       printk("mid_wwmgr_process_error_interrupt: unable to allocate memory\n");
       return (MD_FAILURE);
   }

   (void)supp_read_brd_panic_buf(padapter, (pmerc_uchar_t)padapter->panicDump);

   // dump the panic location as done for sram
   printk("mercd: Got Error Interrupt from WW board %d. Sram Crash dump...\n", padapter->adapternumber);
   for (i=0; i<MD_PANIC_DUMP_MAX_SIZE; i++) {
        if (!(i%8))
            printk("\n");
        printk("%c ", padapter->panicDump[i]);
   }
   printk("\n");

   return(MD_SUCCESS); 
}


/*********************************************************************************
 * Function Name         : mid_wwmgr_internal_close
 * Function Type         : WW manager function
 * Inputs                :
 * Outputs               :
 * Calling functions     :
 * Description           : 
 * Additional comments   :
 **********************************************************************************/
int mid_wwmgr_internal_close(pmercd_adapter_block_sT padapter, pmercd_bind_block_sT  BindBlock)
{
     pmercd_stream_connection_sT StreamBlock;
     merc_uint_t                 szBMap = 0;
     PSTRM_MSG                   Msg = 0;

     StreamBlock = BindBlock->stream_connection_ptr;

     if ((StreamBlock) && (BindBlock->WWCancelStrmAckPending)) {
	 BindBlock->WWCancelStrmAckPending = 0;
	 bnd_dispatch_close_strm(BindBlock);
	 StreamBlock->state = MERCD_STREAM_STATE_FREED;
	 BindBlock->stream_connections = 0;
	 BindBlock->stream_connection_ptr = NULL;
	 BindBlock->flags &= ~MSD_BIND_FLAG_STREAM_OPEN_PEND;
	 BindBlock->state = MERCD_BIND_STATE_RELEASED;
     }

     // Always try t0 Send Death notice if required
     bnd_snd_death_msg2brd(MSD_ONLY_ONE_BIND_DEATH, BindBlock);

     MsdControlBlock->pbind_block_list[BindBlock->bindhandle] = NULL;

#ifdef DRVR_STATISTICS
     MsdControlBlock->bind_count--;
#endif
 
     MSD_DESTROY_MUTEX(&BindBlock->bind_block_mutex);

     szBMap = ((MSD_MAX_BOARD_ID_COUNT)/(8 * sizeof(merc_uint_t)));


     mercd_free(BindBlock->pWWMsgsToAdapters, szBMap, MERCD_FORCE);
     mercd_free(BindBlock, MERCD_BIND_BLOCK, MERCD_FORCE);


     //////////////////////////////////////////////////////////////////////
     //Let us not wait for a close stream ack and clean the StreamBlock.
     //There is no point in waiting for close ack as this is the abnormal
     //termination where application itself exited already.
     /////////////////////////////////////////////////////////////////////
     if (StreamBlock)
      {
        MSD_ENTER_MUTEX(&padapter->snd_data_mutex);
        while((Msg = StreamBlock->SendStreamQueue) != NULL)
         {
           StreamBlock->SendStreamQueue = Msg->b_next;
           MSD_FREE_MESSAGE(Msg);
           Msg = NULL;
        }
        StreamBlock->SendStreamQueue=NULL;
        MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);


        // finally, remove the stream block from adapter block stream list
        if(queue_remove_from_Q(&padapter->stream_adapter_list,
                            (PMSD_GEN_MSG)StreamBlock) != MD_SUCCESS) {
         MSD_ERR_DBGPRINT("InteralClose:RemoveFromQ failed for StreamBlock."
                 " StreamId=%d\n",StreamBlock->id);
         MSD_ASSERT(0);
        }
       padapter->pstream_connection_list[StreamBlock->id] = NULL;


       MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
       MSD_EXIT_MUTEX(&padapter->snd_data_mutex);

       // free the stream block memory space
       mercd_free(StreamBlock, sizeof(mercd_stream_connection_sT), MERCD_FORCE);
#ifdef DRVR_STATISTICS
       MsdControlBlock->open_streams--;
       padapter->streams_count--;
#endif
      //////////////////////////////////////////////////////////////////////
     }
     return (MD_SUCCESS);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_allocate_mem_for_rcvdatablks
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      : Allocate Memory for Rcv Data Blocks
 * Calling functions            :
 * Description                  : Allocs memory for pRcvStrmDmaDesc
 *                              :
 * Additional comments          :
 ****************************************************************************/
int 
mid_wwmgr_allocate_mem_for_rcvdatablks(pmercd_stream_connection_sT StreamBlock,
                                       pmercd_adapter_block_sT padapter)
{
   MD_STATUS                              Status = MD_SUCCESS;
   mercd_dhal_ww_rcv_copybigmsg_str_sT    dir_copy = {0};
   merc_uint_t                            szBMap = 0;
#ifndef LFS
   mercd_ww_dmaDscr_Index_sT      dmaDescSt = {0};
#endif


   MSD_FUNCTION_TRACE(
        "mid_wwmgr_allocate_mem_for_rcvdatablks0x%x %s #%d\n",
         THREE_PARAMETERS,
         (size_t)StreamBlock,
         __FILE__,
         __LINE__
         );

   //////////////////////////////////////////////////////////////////
   //1. Allocs Memory for pRcvStrmDmaDesc table for this StreamBlock
   //////////////////////////////////////////////////////////////////

   //SCHEME1 : try to alloc streams buffers for Rcv Data
   if (StreamBlock->WWRcvStrDataBufPostInAdvance)
   {
     dir_copy.pwwDevi = (pmercd_ww_dev_info_sT)StreamBlock;
     dir_copy.purpose = MERCD_WW_STRMDATA_RCV_DESCR_TABLE_ALLOC;
     dir_copy.ret = MD_SUCCESS;
     //Here we construct the index table and post the databufs into the table
     (*mercd_dhal_func[MERCD_DHAL_WW_RCV_STREAM_DIRCOPY_FROM_KERNEL])((void *)&(dir_copy));
     Status = dir_copy.ret;
     if (Status == MD_SUCCESS)
      {
       StreamBlock->WWRcvStrDataBufAllocd = 1;
      }
     else
       StreamBlock->WWRcvStrDataBufAllocd = 0;
   }
   else
   { //Just allocate the recv stream index table without posting 
     //the databuffers so that when a databuf request comes the
     //index table is ready with pdmadesc at each index. 
     Status = mid_wwmgr_allocate_mem_for_rcvdatablks_index_table(StreamBlock);
     StreamBlock->WWRcvStrDataBufAllocd = 0;

     ///////////////////////////////////////////////////
     //What if you never heard from board for a databuf
     //request ? Then post it from the timer. This is
     //the experimental code, never tested enuff.
     ///////////////////////////////////////////////////
#if 0
      MSD_ENTER_MUTEX(&padapter->pww_info->ww_msgpendq_mutex)
      dmaDescSt.pBitMap = padapter->pww_info->pPendReqRcvStrDataBufBitMap;
      dmaDescSt.szBitMap = padapter->pww_info->szPendReqRcvStrDataBufBitMap;
      dmaDescSt.index = StreamBlock->id;
      Status = msgutl_ww_mark_dmadescr_used(&dmaDescSt);
      padapter->pww_info->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_FROM_BD_DATABUF_MF_BIT);
      //StreamBlock->WWRcvStrDataBufPostPending = 1;
      MSD_EXIT_MUTEX(&padapter->pww_info->ww_msgpendq_mutex)
#endif
     //////////////////////////////////////////////////

   }

   //No SCHEME2 mumbo-jumbo at this point...

    if(Status == MD_FAILURE)
      {
        goto out;
      }

#if 0
    ///////////////////////////////////////////////
    //For Debug Purposes
    ///////////////////////////////////////////////
 {
   merc_uint_t i = 0;
   pmercd_ww_dma_descr_sT     pdmadescr;

   printk("Done allocating the StrmData Rcv Descr.szRcvBitMap: %d\n",
            StreamBlock->szRcvBitMap);

  for (i=0; i< StreamBlock->szRcvBitMap; i++)
     {
        pdmadescr = StreamBlock->pRcvStrmDmaDesc[i];
       printk("pdmadescr @ i: %d is 0x%x\n", i, pdmadescr);
       printk("pdmadescr->next @ i: %d is 0x%x\n", i, pdmadescr->next);
       printk("pdmadescr @ i %d  size: %d\n", i, pdmadescr->size);
       printk("pdmadescr->next @ i%d  size: %d\n", i, pdmadescr->next->size);

     }
  }
#endif


   //////////////////////////////////////////////////
   //2. Init the mutex mercd_ww_rcv_strm_block_mutex
   //////////////////////////////////////////////////
   MSD_INIT_MUTEX(&StreamBlock->mercd_ww_rcv_strm_block_mutex,
                   "Rcv Stream Mutex", NULL);

    /////////////////////////////////////////////////
    //3. Init the pRcvBitMapValue
    /////////////////////////////////////////////////
    StreamBlock->szRcvBitMap = StreamBlock->WWRcvNumBufs;
    szBMap = ((StreamBlock->WWRcvNumBufs)/(8 * sizeof(merc_uint_t)));
    if((StreamBlock->WWRcvNumBufs)%(8 * sizeof(merc_uint_t)))
       szBMap +=1;

     StreamBlock->pRcvBitMapValue = (pmerc_uchar_t)
            MSD_ALLOCATE_ATOMIC_KERNEL_MEMORY(szBMap * sizeof(merc_uint_t));
#if 0
                            mercd_allocator(szBMap * sizeof(merc_uint_t));

#endif
     if (StreamBlock->pRcvBitMapValue == NULL)
      {
        printk("ERROR:MemForRcvStrmDataBlks Failed\n");
        MSD_ERR_DBGPRINT("ERROR:MemForRcvStrmDataBlks Failed\n");
        Status = MD_FAILURE;
        goto out;
      }


     MSD_ZERO_MEMORY(StreamBlock->pRcvBitMapValue, (szBMap * sizeof(merc_uint_t)));

out:
    return (Status);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_send_rcvstrm_datablocks_to_board
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      : Send Rcv Stream Data Blocks to Board
 * Calling functions            :
 * Description                  : 
 *                              :
 * Additional comments          :
 *                              : mid_wwmgr_send_rcvstrm_datablocks_to_board
 *                              : is called either from strat_streams api
 *                              : or generic_timeout. The call to this routine
 *                              : is wrapped around ww_msgpendq_mutex.So no
 *                              : no need to acquire this lock in this or
 *                              : other routines called by this one
 *                              : 
 ****************************************************************************/
int
mid_wwmgr_send_rcvstrm_datablocks_to_board(pmercd_adapter_block_sT     padapter,
                                           pmercd_stream_connection_sT StreamBlock)
{
  MD_STATUS                    Status;
  pmercd_bind_block_sT      BindBlock;
  pmercd_ww_dev_info_sT        pwwDev;
  mercd_ww_dmaDscr_Index_sT  dmaDescSt = {0};
  

  Status = MD_SUCCESS;
  
  pwwDev = padapter->pww_info;

  //Check for the Adapter State
  if ((padapter->state == MERCD_ADAPTER_STATE_SHUTDOWN_PROGRESS) ||
      (!(pwwDev->mfaBaseAddressDescr.host_address)))
   {
     //No need to post the rcv data buffers. Just return.
     return(MD_SUCCESS);
   }

  if((StreamBlock->state == MERCD_STREAM_STATE_CLOSE_PEND) ||
     (StreamBlock->state == MERCD_STREAM_STATE_TERMINATE_PEND) || 
     (StreamBlock->state == MERCD_STREAM_STATE_BROKEN)   ||
     (StreamBlock->state == MERCD_STREAM_STATE_INTERNAL_CLOSE_PEND))
   
   {
     //No need to post the rcv data buffers. Just return.
     return(MD_SUCCESS);
   }

  BindBlock = StreamBlock->pbind_block;

  //Check to see if the databuffer posting is underway
  //if (pwwDev->WWMsgPendingQFlag & MERCD_ADAPTER_WW_FROM_BD_DATABUF_MF_BIT)
  if (StreamBlock->WWRcvStrDataBufPostPending)
   {
    //No need to post the rcv data buffers. Just return
    return(MD_SUCCESS); 
   }

  //Check to see if there are any cancel requests pending
  if ((BindBlock->WWCancelStrmAckPending)   || 
      (BindBlock->WWCancelStrmMFPending)    ||
      (BindBlock->WWCancelBigMsgAckPending) ||
      (BindBlock->WWCancelBigMsgMFPending)  ||
      (BindBlock->WWFlushMFPending)         ||
      (BindBlock->WWFlushAckPending))
   {
    //No need to post the rcv data buffers. Just return
    return(MD_SUCCESS); 
   }

  //Initially we have this question: 
  //Do we need to really do this!!!!!!
  //Why do we need to starve the board at this point.
  //Anyway the fairness in the timer would have
  //taken care of sending the priority messages first.
  //We dont need to do this here.
  //if(!MSD_QUEUE_EMPTY(&padapter->snd_msg_queue))
  // {
  //   goto out;
  // }


 ////////////////////////////////////////////////////////////////////////
 //There is no Rcv side MF limitation: Hence commented out. Remove it
 //in future
 ////////////////////////////////////////////////////////////////////////
#if 0
  //First check to see if there are any available MFs for this StreamBlock
  MSD_ENTER_MUTEX(&StreamBlock->stream_block_global_mutex);
  if (StreamBlock->activeWWMfs >= StreamBlock->maxWWMfs)
         {
            //No MFs for the current StreamBlock.
            MSD_EXIT_MUTEX(&StreamBlock->stream_block_global_mutex);
            goto out;
         }
   MSD_EXIT_MUTEX(&StreamBlock->stream_block_global_mutex);
#endif
 ////////////////////////////////////////////////////////////////////////


  Status = msgutl_ww_build_and_send_recv_stream_datablk_mf(pwwDev, StreamBlock);


  if (Status != MD_SUCCESS)
    {
     //We would have already started the timer in the build routine
     //and would have set StreamBlock->WWRcvStrDataBufPostPending
    }

  return(MD_SUCCESS);

#ifndef LFS
out:
#endif
  //We already acquired this mutex either in start_streams
  //or generic_timeout
  //MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
  dmaDescSt.pBitMap = pwwDev->pPendReqRcvStrDataBufBitMap;
  dmaDescSt.szBitMap = pwwDev->szPendReqRcvStrDataBufBitMap;
  dmaDescSt.index = StreamBlock->id;
  Status = msgutl_ww_mark_dmadescr_used(&dmaDescSt);
  pwwDev->WWMsgPendingQFlag |= (MERCD_ADAPTER_WW_FROM_BD_DATABUF_MF_BIT);
  StreamBlock->WWRcvStrDataBufPostPending = 1;
  //MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)

  return (MD_SUCCESS);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_alloc_streams_descriptors
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      : Alloc WW Specifics for Streaming
 * Calling functions            :
 * Description                  :
 *                              :
 * Additional comments          :
 ****************************************************************************/
int
mid_wwmgr_alloc_streams_descriptors ( pmercd_adapter_block_sT     padapter,
                         pmercd_stream_connection_sT StreamBlock)
{
  merc_uint_t i = 0;
  merc_uint_t szBMap = 0;
  pmercd_ww_dma_descr_sT pdmaDescr = 0;
  MD_STATUS Status = MD_SUCCESS;

  if ((!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY)) ||
      ((padapter->pww_info->state != MERCD_ADAPTER_WW_SUCCEDED)) ||
      (StreamBlock->WWMode == 0))
#ifdef LFS
  return (0);
#else
  return;
#endif

  //Allocate the StreamBlock Specific Global Mutex
  MSD_INIT_MUTEX(&StreamBlock->stream_block_global_mutex,
                 "StreamBlock Global Mutex\N", NULL);
  StreamBlock->pbind_block->WWCancelStrmMFPending = 0;
  StreamBlock->pbind_block->WWCancelStrmAckPending = 0;
  StreamBlock->pbind_block->WWCancelBigMsgMFPending = 0;
  StreamBlock->pbind_block->WWCancelBigMsgAckPending = 0;
  StreamBlock->pbind_block->WWFlushMFPending = 0;
  StreamBlock->pbind_block->WWFlushAckPending = 0;


  if (StreamBlock->flags & (STREAM_OPEN_F_SEND_ONLY))
  {

    MSD_INIT_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex,
		  "Send Stream Mutex\n", NULL);

    ///////////////////////////////////////////////
    //Send Stream DMA Descr Allocation
    ///////////////////////////////////////////////

    StreamBlock->pSendStrmDmaDesc = (pmercd_ww_dma_descr_sT *)
	mercd_allocator(MERCD_WW_MAX_MFS_PER_STREAM * sizeof(pmercd_ww_dma_descr_sT));

    if (StreamBlock->pSendStrmDmaDesc == NULL)
      {
        printk("AllocStrmDescriptors ERROR: pSendStrmDmaDesc Alloc Failed\n"); 
        return(MD_FAILURE);
      }

    MSD_ZERO_MEMORY(StreamBlock->pSendStrmDmaDesc,
	  ( MERCD_WW_MAX_MFS_PER_STREAM * sizeof(pmercd_ww_dma_descr_sT)));
    for (i = 0; i < MERCD_WW_MAX_MFS_PER_STREAM; i++)
     {
	pdmaDescr = (pmercd_ww_dma_descr_sT)
	     mercd_allocator(sizeof(mercd_ww_dma_descr_sT));
        if (pdmaDescr == NULL)
         {
      	   printk("AllocStrmDescriptors ERROR: pSendStrmDmaDesc[%d] Alloc Failed\n", i); 
           return(MD_FAILURE);
         }
	MSD_ZERO_MEMORY(pdmaDescr, sizeof(mercd_ww_dma_descr_sT));
	StreamBlock->pSendStrmDmaDesc[i] = pdmaDescr;
     }

    //Allocate the StreamBlock->pSendBitMapValue
    szBMap = ((MERCD_WW_MAX_MFS_PER_STREAM)/(8 * sizeof(merc_uint_t)));

    if ((MERCD_WW_MAX_MFS_PER_STREAM)%((8*sizeof(merc_uint_t))))
       szBMap += 1;
    StreamBlock->szSendBitMap = MERCD_WW_MAX_MFS_PER_STREAM;
    StreamBlock->pSendBitMapValue = (pmerc_uchar_t)
	  mercd_allocator(szBMap *  sizeof(merc_uint_t));
    if (StreamBlock->pSendBitMapValue == NULL)
       return(MD_FAILURE);

    //printk("alloc_streams_descriptors: StreamBlock->pSendBitMapValue :0x%x\n",
    //        StreamBlock->pSendBitMapValue);

    MSD_ZERO_MEMORY(StreamBlock->pSendBitMapValue,
	    (szBMap * sizeof(merc_uint_t)));
    //printk("mid_wwmgr_alloc_streams_descriptors..done allcoiing pSendBitMapValue\n");
	  
  }
 else
  {
   StreamBlock->pWWReadEosMsg = supp_alloc_buf(
					       sizeof(MDRV_MSG)+
					       MD_MAX_DRIVER_BODY_SZ,
                                               GFP_DMA
					       );
   if (StreamBlock->pWWReadEosMsg == NULL)
     {
      printk("AllocStrmDescriptors ERROR: pWWReadEosMsg Alloc Failed\n");
      return(MD_FAILURE);
     }

   StreamBlock->pWWReadEosMsg->b_cont = supp_alloc_buf
					  (
					   sizeof(MERC_HOSTIF_MSG)+
					   sizeof(HIF_SESS_CLOSED), 
                                           GFP_DMA
					  );
   if (StreamBlock->pWWReadEosMsg->b_cont == NULL)
     {
      printk("AllocStrmDescriptors ERROR: pWWReadEosMsg->b_cont Alloc Failed\n");
      return(MD_FAILURE);
     }
   StreamBlock->stats.ww_rcv_dblk_sequence_num = 0;

   if (!(StreamBlock->WWRcvStrDataBufAllocd))
   {
     Status = mid_wwmgr_allocate_mem_for_rcvdatablks(StreamBlock, padapter);
     if (Status != MD_SUCCESS) 
      {
         MSD_LEVEL2_DBGPRINT( "set_param: Alloc for Rcv DATA Blks Failed %s #%d\n", __FILE__, __LINE__);
      return (MD_FAILURE);
      }
     else
      {
	//Set this to 0 during start streams. We
	//use this flag to avoid any possible race
	//conditions while posting the buffers 
	//from timers. This is a replenishment 
        //pending flag.
        StreamBlock->WWRcvStrDataBufPostPending = 0;
      }
   }
 } /* StreamFlags */

return (MD_SUCCESS);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_close_streams
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      : Dealloc WW Specifics for Streaming
 * Calling functions            :
 * Description                  :
 *                              :
 * Additional comments          :
 ****************************************************************************/
int
mid_wwmgr_close_streams ( pmercd_adapter_block_sT     padapter,
                         pmercd_stream_connection_sT StreamBlock,
			 merc_uint_t Normal)
{
  pmercd_ww_dev_info_sT                                pwwDev;
  merc_uint_t                                          szBMap;
#ifndef LFS
  pmercd_ww_dma_descr_sT   pdmaDescr, npdmaDescr, nnpdmaDescr;
#endif
  MD_STATUS                               Status = MD_SUCCESS;
  mercd_ww_dmaDscr_Index_sT                   dmaDescSt = {0};
#ifndef LFS
  merc_uint_t                                       count = 0;
#endif
  mercd_dhal_ww_dealloc_dmadescr_sT                 descrFree;

  pwwDev = padapter->pww_info;

  if ((!(padapter->flags.WWFlags &  MERCD_ADAPTER_WW_I20_MESSAGING_READY)) ||
      (padapter->pww_info->state != MERCD_ADAPTER_WW_SUCCEDED))
#ifdef LFS
    return (0);
#else
    return;
#endif

  if ((!padapter) || (!StreamBlock))
#ifdef LFS
    return (0);
#else
    return;
#endif


  if (Normal)
     MSD_ENTER_MUTEX(&pwwDev->ww_msgpendq_mutex)
  dmaDescSt.pBitMap = pwwDev->pPendReqRcvStrDataBufBitMap;
  dmaDescSt.szBitMap = pwwDev->szPendReqRcvStrDataBufBitMap;
  dmaDescSt.index = StreamBlock->id;
  Status = msgutl_ww_mark_dmadescr_free(&dmaDescSt);

  if (Normal)
     MSD_EXIT_MUTEX(&pwwDev->ww_msgpendq_mutex)

  if ((StreamBlock->pRcvStrmDmaDesc) &&
     (StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY))
   {
	 ////////////////////////////////////////////////////////////
     descrFree.pwwDevi = pwwDev;
     descrFree.resource = StreamBlock->pRcvStrmDmaDesc;
     descrFree.purpose = MERCD_WW_STREAM_DESCR_DEALLOC;
     descrFree.extradata = (pmerc_void_t)StreamBlock;
     (*mercd_dhal_func[MERCD_DHAL_WW_DEALLOC_DESC_TABLE])
                                         ((pmerc_void_t)&descrFree);
	 ////////////////////////////////////////////////////////////////
       
    if (StreamBlock->pRcvBitMapValue)
     {
       szBMap = ((StreamBlock->szRcvBitMap)/(8 * sizeof(merc_uint_t)));
      if ((StreamBlock->szRcvBitMap)%(8 * sizeof(merc_uint_t)))
         szBMap+=1;
      mercd_free(StreamBlock->pRcvBitMapValue,
                                       szBMap, 
                                  MERCD_FORCE);
      StreamBlock->pRcvBitMapValue = NULL;
    }

    //Just to Makesure
    if (StreamBlock->pRcvStrmDmaDesc)
	StreamBlock->pRcvStrmDmaDesc = NULL;

   if (StreamBlock->pWWReadEosMsg)
      MSD_FREE_MESSAGE(StreamBlock->pWWReadEosMsg);

#if 0
   if (StreamBlock->pWWFlushCloseMsg)
      MSD_FREE_MESSAGE(StreamBlock->pWWFlushCloseMsg);
#endif

  } /* Read StreamBlock */
else /* Write StreamBlock */
   if ((StreamBlock->pSendStrmDmaDesc) &&
       (StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY))
      {
	   //////////////////////////////////////////////////////////////
	   descrFree.pwwDevi = pwwDev;
	   descrFree.resource = StreamBlock->pSendStrmDmaDesc;
           descrFree.purpose = MERCD_WW_STREAM_DESCR_DEALLOC;
	   descrFree.extradata = (pmerc_void_t)StreamBlock;
	   (*mercd_dhal_func[MERCD_DHAL_WW_DEALLOC_DESC_TABLE])
                                         ((pmerc_void_t)&descrFree);
		////////////////////////////////////////////////////////////////

       if (StreamBlock->pSendBitMapValue)
        {
         szBMap = ((StreamBlock->szSendBitMap)/(8 * sizeof(merc_uint_t)));
         if ((StreamBlock->szSendBitMap)%(8 * sizeof(merc_uint_t)))
            szBMap+=1;
         mercd_free(StreamBlock->pSendBitMapValue,
                                           szBMap,
                                     MERCD_FORCE);
         StreamBlock->pSendBitMapValue = NULL;
         //StreamBlock->pSendStrmDmaDesc = NULL;
        }

       //Just to Make sure
       if (StreamBlock->pSendStrmDmaDesc) 
	  StreamBlock->pSendStrmDmaDesc = NULL; 

       MSD_DESTROY_MUTEX(&StreamBlock->mercd_ww_snd_strm_block_mutex);
    } /* Write StreamBlock */

   MSD_DESTROY_MUTEX(&StreamBlock->stream_block_global_mutex);
   return (MD_SUCCESS);
}

/***************************************************************************
 * Function Name                : mid_wwmgr_process_abnormal_termination
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      : 
 * Calling functions            :
 * Description                  :
 *                              :
 * Additional comments          :
 ****************************************************************************/
int
mid_wwmgr_process_abnormal_termination ( pmercd_ww_dev_info_sT pwwDev)

{
    merc_uint_t count                         = 0;
    pmercd_adapter_block_sT                 padapter;
#ifndef LFS
    pmerc_uchar_t                         pbitmap;
#endif
    mercd_ww_dmaDscr_Index_sT     dmaDescSt = {0};
    mercd_dhal_ww_free_desc_sT   freeDescSt = {0};
    merc_ushort_t                dmaDescIndex = 0; 
    pmercd_bind_block_sT                BindBlock;
    MD_STATUS Status                 = MD_SUCCESS;
    pwwDev->WWCancelBigMsgAckPending          = 0;


    padapter = (pmercd_adapter_block_sT)pwwDev->padapter;
    

     //i_bprintf(pwwDev->pPendReqBindBitMap,  pwwDev->szPendReqBindBitMap);

     for (count = 0; count < MsdControlBlock->maxbind; count++)
      {
        BindBlock = MsdControlBlock->pbind_block_list[count];
        if (BindBlock == NULL)
          {
            //printk("WWTimer: BindBlock is NULL This Should Never Happen\n");
             continue;
           }
        else
          {
            if (BindBlock->WWCancelBigMsgAckPending == 0)
             {
              //printk("This BindBlock %d !WWCancelBigMsgAckPending\n",
              //        BindBlock->bindhandle);
              //i_bprintf(pwwDev->pPendMsgBitMap[count], 4);
               continue;
              }
             else
              {
                //printk("WWCancelBigMsgAckPending for BindBlock: %d\n", 
                //       BindBlock->bindhandle);
                //i_bprintf(pwwDev->pPendMsgBitMap[count], 4);
               }
           }

           //If per BindBlock WWCancelBigMsgAckPending,
           //at this point we dont need to wait for it from FW

           //Free the BigMsgs to Board that are waiting for a
           //Read Ack from Board
           //printk("WWTimer: WWCancelBigMsgAckPending..\n");
           if((BindBlock->WWPendReadAckForToBoardBigMsgs) &&
              (BindBlock->WWCancelBigMsgAckPending))
            {
              //printk("BindBlock with BindHandle: %d has pending messages\n", count);
#ifdef LFS
              for (dmaDescIndex = 0; 
                   dmaDescIndex < pwwDev->szPendMsgBitMap;
                   dmaDescIndex++)
#else
              for (dmaDescIndex = 0; 
                   dmaDescIndex < pwwDev->szPendMsgBitMap, 
                   BindBlock->WWPendReadAckForToBoardBigMsgs; 
                   dmaDescIndex++)
#endif
               {
                 dmaDescSt.pBitMap = pwwDev->pPendMsgBitMap[count];
                 dmaDescSt.szBitMap = pwwDev->szPendMsgBitMap;
                 dmaDescSt.index = dmaDescIndex;
                 Status =  msgutl_ww_check_if_dmadescr_used(&dmaDescSt);
                 if (Status == MD_SUCCESS)
                  {
                   // pwwDev->BigMsgSndMemStr.pbigMsgDescr[dmaDescIndex];
                   freeDescSt.pwwDevi = pwwDev;
     	           freeDescSt.index = dmaDescIndex;
                   freeDescSt.StreamBlock = 0;
                   freeDescSt.purpose = 0;
                   //printk("dmaDescIndex: %d is used\n", dmaDescIndex);
                   //i_bprintf(dmaDescSt.pBitMap, dmaDescSt.szBitMap);
                   (*mercd_dhal_func[MERCD_DHAL_WW_DEALLOC_SND_BIGMSG_DESC])
                                                  ((pmerc_void_t)&freeDescSt);
                   if (freeDescSt.ret == MD_FAILURE)
                    {
			BindBlock->WWPendReadAckForToBoardBigMsgs = 0;
			break;
                    }

                    //printk("WWTimer: DOne freeing SND_BIGMSG_DESC..\n");

                    //Mark the global Message Pending Bitmap as free
                    Status = msgutl_ww_mark_dmadescr_free(&dmaDescSt);
                    BindBlock->WWPendReadAckForToBoardBigMsgs--;

                    //printk("WWPendReadAckForToBoardBigMsgs: %d\n",
                    //       BindBlock->WWPendReadAckForToBoardBigMsgs);

                    //Now we are left with the only descriptor at dmaDescIndex
                    //Mark this as free in the bitmap.
                    dmaDescSt.pBitMap = pwwDev->BigMsgSndMemStr.pBitMapValue;
                    dmaDescSt.szBitMap = pwwDev->BigMsgSndMemStr.szBitMap;
	       	    dmaDescSt.index = dmaDescIndex;
                    Status = msgutl_ww_mark_dmadescr_free(&dmaDescSt);
                     
                    //printk("BigMsgSndMemStr.pBitMapValue marked free..\n");
                       
                   }
                } /* dmaDescIndex */
             }

             //BindBlock->WWCancelBigMsgAckPending = 0;
             pwwDev->WWCancelBigMsgAckPending = 0;

             //Before performing the internal close
             //Free the bit in the global bitmap of
             //StreamBlocks with peinding Rcv DATABUF
             //Posting.
             if(BindBlock->stream_connection_ptr)
              {
                dmaDescSt.pBitMap = pwwDev->pPendReqRcvStrDataBufBitMap;
                dmaDescSt.szBitMap = pwwDev->szPendReqRcvStrDataBufBitMap;
                dmaDescSt.index = ((pmercd_stream_connection_sT)
                                  (BindBlock->stream_connection_ptr))->id;
                Status = msgutl_ww_mark_dmadescr_free(&dmaDescSt);
                mid_wwmgr_close_streams(padapter, BindBlock->stream_connection_ptr, 0);
              }
             mid_wwmgr_internal_close(padapter, BindBlock);
      } /* for */
     
      //Start a timer to find if there are any more StreaBlocks for which
      //there is no CLOSE_ACK received
#ifdef LFS
    return (0);
#endif
}

/***************************************************************************
 * Function Name                : mid_wwmgr_send_pending_cancel_bigmsg_mfs
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 *                              :
 * Additional comments          :
 ****************************************************************************/
int
mid_wwmgr_send_pending_cancel_bigmsg_mfs( pmercd_ww_dev_info_sT pwwDev)
{
     merc_uint_t count                            = 0;
     mercd_ww_dmaDscr_Index_sT        dmaDescSt = {0};
     pmercd_bind_block_sT                   BindBlock;
     pmercd_adapter_block_sT                 padapter;
     MD_STATUS Status                    = MD_SUCCESS;


     pwwDev->WWCancelBigMsgAckPending = 0;
     dmaDescSt.pBitMap = pwwDev->pPendBindCancelMsgMFBitMap;
     dmaDescSt.szBitMap = pwwDev->szPendBindCancelMsgMFBitMap;

     padapter = (pmercd_adapter_block_sT)pwwDev->padapter;

     for (count = 0; count < pwwDev->szPendBindCancelMsgMFBitMap; count++)
      {
         dmaDescSt.index = count;
         Status = msgutl_ww_check_if_dmadescr_used(&dmaDescSt);
 
         if (Status == MD_SUCCESS)
           {
             Status = msgutl_ww_mark_dmadescr_free(&dmaDescSt);
             BindBlock = MsdControlBlock->pbind_block_list[count];
             if (BindBlock == NULL)
                printk("WWTimer: BindBlock NULL (Should Never Happen)\n");

             if ((BindBlock->WWCancelBigMsgMFPending) && 
                (BindBlock->WWPendReadAckForToBoardBigMsgs))
              {
                Status = msgutl_ww_build_and_send_cancel_bigmsg_mf(padapter->pww_info,
                                                                   BindBlock);
                if (Status == MD_SUCCESS)
                {
                  BindBlock->WWCancelBigMsgMFPending = 0; 
                  BindBlock->WWCancelBigMsgAckPending = 1;
                  pwwDev->WWCancelBigMsgAckPending= 1;
                }
               else
                {
                  //Put this BindBlock on the global BitMap for BindBlocks
                  //with Pending Cancel Msg MF.
                  Status = msgutl_ww_mark_dmadescr_used(&dmaDescSt);
                }
             }
           }
      }
#ifdef LFS
    return (0);
#endif
}

/***************************************************************************
 * Function Name                : mid_wwmgr_send_pending_cancel_stream_mfs
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 *                              :
 * Additional comments          :
 ****************************************************************************/
int
mid_wwmgr_send_pending_cancel_stream_mfs( pmercd_ww_dev_info_sT pwwDev)
{
     merc_uint_t count = 0;
     mercd_ww_dmaDscr_Index_sT  dmaDescSt = {0};
     pmercd_bind_block_sT                    BindBlock;
     MD_STATUS Status = MD_SUCCESS;
     dmaDescSt.pBitMap = pwwDev->pPendBindCancelStrMFBitMap;
     dmaDescSt.szBitMap = pwwDev->szPendBindCancelStrMFBitMap;


     for(count=0; count < pwwDev->szPendBindCancelStrMFBitMap; count++)
      {
         dmaDescSt.index = count;
         Status = msgutl_ww_check_if_dmadescr_used(&dmaDescSt);
         if (Status == MD_SUCCESS)
          {
            Status = msgutl_ww_mark_dmadescr_free(&dmaDescSt);
            BindBlock = MsdControlBlock->pbind_block_list[count];
            if (BindBlock == NULL)
               continue;
            if (BindBlock->WWCancelStrmMFPending) 
            {
               Status = msgutl_ww_build_and_send_cancel_stream_mf(BindBlock);
               if (Status == MD_SUCCESS)
                {
                  BindBlock->WWCancelStrmAckPending = 1;
                  BindBlock->WWCancelStrmMFPending = 0;
                }
               else
                {
                 //Put this BindBlock on the global BitMap for BindBlocks
                 //with Pending Cancel Str MF.
                 Status = msgutl_ww_mark_dmadescr_used(&dmaDescSt);
                }
            }
          } /* Status */
     } /* for */
#ifdef LFS
    return (0);
#endif
}

/***************************************************************************
 * Function Name                : mid_wwmgr_send_pending_flush_stream_mfs
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 *                              :
 * Additional comments          :
 ****************************************************************************/
int
mid_wwmgr_send_pending_flush_stream_mfs( pmercd_ww_dev_info_sT pwwDev)
{
	pmercd_adapter_block_sT    padapter;
        pmercd_stream_connection_sT           StreamBlock;
         mercd_ww_dmaDscr_Index_sT             flushDscrSt;
         mercd_ww_flush_streamMF                   flushMf;
        merc_uint_t count = 0;
        MD_STATUS Status = MD_SUCCESS;

        padapter = (pmercd_adapter_block_sT)pwwDev->padapter;
	//This should never happen
	if(MSD_QUEUE_EMPTY(&padapter->snd_msg_queue) &&
	!(padapter->stream_adapter_list.QueueHead))
	{
	printk("Pending Q, flush bit set, But Q empty\n");
	}

	flushDscrSt.pBitMap = pwwDev->pPendFlushStrBitMap;
	flushDscrSt.szBitMap = pwwDev->szPendFlushStrBitMap;
	for (count = 0; count < pwwDev->szPendFlushStrBitMap; count++)
	{
		flushDscrSt.index = count;
		Status = msgutl_ww_check_if_dmadescr_used(&flushDscrSt); 
		if (Status == MD_FAILURE)
		continue;

		flushMf.streamid = count;
		flushMf.padapter = (pmerc_void_t)padapter;
		Status = msgutl_ww_build_and_send_streams_flush_mf(&flushMf);

		if (Status == MD_SUCCESS)
		{
		msgutl_ww_mark_dmadescr_free(&flushDscrSt); 
		//Now move the pending flush/close message to the
		//head of padapter->snd_msg_queue 
		StreamBlock = padapter->pstream_connection_list[count];
		if (StreamBlock == NULL)
		  continue;

		if (StreamBlock->pWWFlushCloseMsg)
		  {
		    queue_put_bk_msg_Q(&padapter->snd_msg_queue, 
				    StreamBlock->pWWFlushCloseMsg);
		    StreamBlock->pWWFlushCloseMsg = NULL;
		    //Now schedule to send these Close messages at the head of the
		    //padapter->snd_msg_queue
		    snd_ww_msgs2adapter(padapter, MERCD_ADAPTER_WW_SEND_MSG_FLUSH_ONLY);
		  }

		}
	} /* for */
#ifdef LFS
    return (0);
#endif
}

/********************************************************************************
 * Function Name                : mid_wwmgr_send_pending_recv_streams_databuf_mfs
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 *                              :
 * Additional comments          :
 *********************************************************************************/
int
mid_wwmgr_send_pending_recv_streams_databuf_mfs( pmercd_ww_dev_info_sT pwwDev)
{
     merc_uint_t count                             = 0;
     merc_uint_t count1                            = 0;
     mercd_ww_dmaDscr_Index_sT  dmaDescSt         = {0};
     mercd_ww_dmaDscr_Index_sT  dmaDescSt1        = {0};
     mercd_dhal_ww_rcv_copybigmsg_str_sT dir_copy = {0};
     pmercd_stream_connection_sT             StreamBlock;
     pmercd_adapter_block_sT                    padapter;
     
     MD_STATUS Status = MD_SUCCESS;
     dmaDescSt.pBitMap = pwwDev->pPendReqRcvStrDataBufBitMap;
     dmaDescSt.szBitMap = pwwDev->szPendReqRcvStrDataBufBitMap;

     padapter = (pmercd_adapter_block_sT)pwwDev->padapter;
   
      for(count=0; count < pwwDev->szPendReqRcvStrDataBufBitMap; count++)
      {
         dmaDescSt.index = count;
         Status = msgutl_ww_check_if_dmadescr_used(&dmaDescSt);
         if (Status == MD_SUCCESS)
          {
            msgutl_ww_mark_dmadescr_free(&dmaDescSt);
            StreamBlock = padapter->pstream_connection_list[count];

            if (StreamBlock == NULL)
              {
               printk("DataBuf Post Pend Bit set for: %d But StreamBlock 0\n", count);
               continue;
              }
            if ((StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY) ||
                (!(StreamBlock->WWRcvStrDataBufPostPending)))
               continue;

            StreamBlock->WWRcvStrDataBufPostPending = 0;

            if (StreamBlock->WWRcvStrDataBufPostMethod == 
			    MERCD_WW_RCVSTRM_DATABUF_POST_DEFERRED)
            {
              //First for the descriptors with no buffers, we allocate
              //them first. 
              for (count1 = 0; count1 < StreamBlock->szRcvBitMap; count1++)
               {
                 dmaDescSt1.pBitMap   = StreamBlock->pRcvBitMapValue; 
                 dmaDescSt1.szBitMap  = StreamBlock->szRcvBitMap;
                 dmaDescSt1.index = count1;
                 Status = msgutl_ww_check_if_dmadescr_used(&dmaDescSt1);
                 if (Status !=MD_SUCCESS)
                  {
                   dir_copy.pwwDevi = (pmercd_ww_dev_info_sT)StreamBlock;
                   dir_copy.purpose = MERCD_WW_STRMDATA_RCV_DESCR_ALLOC;
                   dir_copy.index = dmaDescSt1.index;
                   //printk("Timer: Alloc Rcv DataBuf for Index: %d\n", dmaDescSt1.index);
                   (*mercd_dhal_func[MERCD_DHAL_WW_RCV_STREAM_DIRCOPY_FROM_KERNEL])\
                                           ((void *)&(dir_copy));
                   Status = dir_copy.ret;
                  }
	       }
             } /* MERCD_WW_RCVSTRM_DATABUF_POST_DEFERRED */
            Status = mid_wwmgr_send_rcvstrm_datablocks_to_board(padapter, StreamBlock);
          }
      }
#ifdef LFS
    return (0);
#endif
}

/********************************************************************************
 * Function Name                : mid_wwmgr_cleanup_unacked_closed_streams
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 *                              :
 * Additional comments          : At this point Not being used
 *********************************************************************************/
int
mid_wwmgr_cleanup_unacked_closed_streams( pmercd_ww_dev_info_sT pwwDev)
{

#ifndef LFS
    merc_uint_t count                         = 0;
    pmerc_uchar_t                         pbitmap;
    mercd_ww_dmaDscr_Index_sT     dmaDescSt = {0};
    mercd_dhal_ww_free_desc_sT   freeDescSt = {0};
    merc_ushort_t                dmaDescIndex = 0; 
    pmercd_bind_block_sT                BindBlock;
    MD_STATUS Status                 = MD_SUCCESS;
#endif
    pmercd_adapter_block_sT              padapter;
    merc_uint_t                          cnt0 = 0;
    pmercd_stream_connection_sT       StreamBlock;

    padapter = (pmercd_adapter_block_sT)pwwDev->padapter;

    //printk("mid_wwmgr_cleanup_unacked_closed_streams...\n");

    //Dealloc the memory specific to streaming
    for (cnt0 = 0; cnt0< MSD_MAX_STREAM_ID; ++cnt0)
      {
#ifndef LFS
          PSTRM_MSG Msg;
#endif
           MSD_ENTER_MUTEX(&padapter->stream_connection_mutex);
          if(( StreamBlock = padapter->pstream_connection_list[cnt0]) != NULL)
            {
                if ((StreamBlock->WWMode) && 
                    (StreamBlock->pbind_block == NULL) &&
                    (StreamBlock->state == MERCD_STREAM_STATE_FREED))
                  {
                    //printk("cleanup_unacked_closed_streams: found streamid: %d\n", StreamBlock->id);
                    mid_wwmgr_close_streams(padapter, StreamBlock, 0);
                    
                    // finally, remove the stream block from adapter block stream list
                    if(queue_remove_from_Q(&padapter->stream_adapter_list,
                           (PMSD_GEN_MSG)StreamBlock) != MD_SUCCESS) 
                     {
                       MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
                       MSD_ERR_DBGPRINT("CLOSE_ACK:RemoveFromQ failed for StreamBlock. StreamId=%d\n",StreamBlock->id);
                       MSD_ASSERT(0);
                     }
                     padapter->pstream_connection_list[cnt0] = NULL;
                     // free the stream block memory space
                     mercd_free(StreamBlock, sizeof(mercd_stream_connection_sT), MERCD_FORCE);
                  }
             }
          MSD_EXIT_MUTEX(&padapter->stream_connection_mutex);
      } /*for*/
#ifdef LFS
    return (0);
#endif
}

/********************************************************************************
 * Function Name                : mid_wwmgr_check_abnormal_termination
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 *                              :
 * Additional comments          :
 *********************************************************************************/
int
mid_wwmgr_check_abnormal_termination(pmercd_stream_connection_sT  StreamBlock)
{
  pmercd_bind_block_sT    tmpBindBlock;
  pmercd_open_block_sT  tmpOpenBlockContext;
  tmpBindBlock = StreamBlock->pbind_block;

  if (!tmpBindBlock)
    {
      printk("check_abnormal: BindBlock is Null..\n");
      return(MD_FAILURE);
    }

  if (tmpBindBlock)
   {
      if (tmpBindBlock->WWCancelBigMsgAckPending)
	 {
	   printk("check_abnormal: WWCancelBigMsgAckPending..\n");
	   return(MD_FAILURE);
	 }
   }

  tmpOpenBlockContext  = tmpBindBlock->popen_block;
  if (!tmpOpenBlockContext)
   {
    printk("check_abnormal: tmpOpenBlockContext is NULL..\n");
    return(MD_FAILURE);
   }
  else
   if (tmpOpenBlockContext->state == MERCD_OPEN_STATE_CLOSE_PEND)
     {
       printk("check_abnormal:\n tmpOpenBlockContext state CLOSE_PEND\n");
       return(MD_FAILURE);
     }
  return(MD_SUCCESS);
}

/********************************************************************************
 * Function Name                : mid_wwmgr_dump_streamblock_info
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 *                              :
 * Additional comments          : This dumps the StreamBlock info
 *********************************************************************************/
void mid_wwmgr_dump_streamblock_info(pmercd_stream_connection_sT StreamBlock)
{

  if (StreamBlock == NULL)
   {
    return;
   }
  printk("Here is the StreamBlock Information with StreamId: %d\n", StreamBlock->id);
  printk("//////////////////////////////////////////////////////////\n");
  printk("WWMode %d\n", StreamBlock->WWMode);
  if (StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY)
    {
      printk("%d is a Receive StreamBlock\n", StreamBlock->id);

      if (StreamBlock->pWWReadEosMsg)
         printk("EOS is not received Yet\n");
      else
         printk("EOS is already received\n");

      if (StreamBlock->WWRcvStrDataBufPostPending)
        printk("RcvStrDataBufPostPending is Set\n");
      else
        printk("RcvStrDataBufPostPending is Not Set\n");

      printk("WWRcvNumBufs: %d\n", StreamBlock->WWRcvNumBufs);

      if (StreamBlock->pRcvBitMapValue) {
       printk("Dump of pRcvBitMapValue: \n");
       i_bprintf(StreamBlock->pRcvBitMapValue, 1);
      }

      printk("Dumping the 4 DMA Descriptors:\n");
      printk("Descriptor: 1\n");
      printk("Descriptor: 2\n");
      printk("Descriptor: 3\n");
      printk("Descriptor: 4\n");
    }
   else
    {
      if (StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY)
       {
         printk("%d is a Send StreamBlock\n", StreamBlock->id);
	 if (StreamBlock->pSendBitMapValue) 
	  {
            printk("Dump of pSendBitMapValue: \n");
            i_bprintf(StreamBlock->pSendBitMapValue, 1);
	  }

         if (StreamBlock->writeDMAPending)
            printk("There are some Write Data Blocks waiting for DMA completion\n");
         else
            printk("There are NO Write Data Blocks waiting for DMA completion\n");

         printk("Dumping the 10 DMA Descriptors:\n");
      }
    }
}


/********************************************************************************
 * Function Name                : mid_wwmgr_check_cancel_strm_mf_needed
 * Function Type                : manager function
 * Inputs                       :
 * Outputs                      :
 * Calling functions            :
 * Description                  :
 *                              :
 * Additional comments          :
 *********************************************************************************/

MD_STATUS mid_wwmgr_check_cancel_strm_mf_needed(pmercd_bind_block_sT BindBlock)
{
  merc_uint_t szBMap = 0;
  merc_uint_t cnt = 0;
  pmerc_uint_t ptr = NULL;
  pmercd_stream_connection_sT StreamBlock = NULL;

  if (BindBlock == NULL)
     return (MD_FAILURE);

  StreamBlock = BindBlock->stream_connection_ptr;

  if (StreamBlock == NULL)
     return (MD_FAILURE);

  if (StreamBlock->flags & STREAM_OPEN_F_SEND_ONLY)
  {
    ptr = (pmerc_uint_t)StreamBlock->pSendBitMapValue;

    if (ptr == NULL)
     return(MD_FAILURE);

    szBMap = ((StreamBlock->szSendBitMap)/(8 * sizeof(merc_uint_t)));
    if ((StreamBlock->szSendBitMap)%((8*sizeof(merc_uint_t))))
       szBMap += 1;
    // printk("This is WStreamBlock szBMap: %d\n", szBMap);
  }
  else
  {
     if (StreamBlock->flags & STREAM_OPEN_F_RECEIVE_ONLY)
      {
         ptr = (pmerc_uint_t)StreamBlock->pRcvBitMapValue;
	 if (ptr == NULL)
	    return(MD_FAILURE);
	 szBMap = ((StreamBlock->szRcvBitMap)/(8 * sizeof(merc_uint_t)));
	 if ((StreamBlock->szRcvBitMap)%((8*sizeof(merc_uint_t))))
		 szBMap += 1;
      //   printk("This is RStreamBlock szBMap: %d\n", szBMap);
      }
  }

  for (cnt=0; cnt < szBMap; cnt++)
   {
     //Just the deref is enuff
     if (*ptr) {
       //printk("*ptr: 0x%x\n", *ptr);
       BindBlock->WWCancelStrmMFPending = 1;
       break;
     }
  else
    ptr++;
 }

 if (BindBlock->WWCancelStrmMFPending)
  return (MD_SUCCESS);
 else
  return (MD_FAILURE);
}


/**************************************************************************************
 * Function Name        :  mid_wwmgr_allocate_mem_for_rcvdatablks_index_table
 * Function Type        : 
 *                      : 
 * Inputs               :
 * Outputs              :
 * Calling functions    :
 * Description          :
 * Additional comments  : This caters to the Receive Streams
 ***************************************************************************************/
int mid_wwmgr_allocate_mem_for_rcvdatablks_index_table(pmercd_stream_connection_sT StreamBlock)
{
#ifndef LFS
 pmercd_adapter_block_sT                  padapter;
#endif
 pmercd_ww_dma_descr_sT                    pdmadescr;
#ifndef LFS
 merc_ulong_t                                 size;
#endif
 merc_uint_t                                 count;
 merc_uint_t                           start_index;
 merc_uint_t                             end_index;
 MD_STATUS                               Status;

 MSD_FUNCTION_TRACE("mid_wwmgr_allocate_mem_for_rcvdatablks_index_table", 
                      ONE_PARAMETER, 
                     (size_t) StreamBlock);

 Status = MD_SUCCESS;
 StreamBlock->pRcvStrmDmaDesc = (pmercd_ww_dma_descr_sT *)
                                 mercd_allocator(StreamBlock->WWRcvNumBufs *
                                    sizeof(pmercd_ww_dma_descr_sT));
 if (StreamBlock->pRcvStrmDmaDesc == NULL)
   {
       printk("mid_wwmgr_allocate_mem_for_rcvdatablks_index_table pRcvStrmDmaDesc Alloc Failed\n");
          Status = MD_FAILURE;
          return(Status);
    }

 MSD_ZERO_MEMORY(StreamBlock->pRcvStrmDmaDesc, (StreamBlock->WWRcvNumBufs *  
				    sizeof(pmercd_ww_dma_descr_sT)));
 start_index = 0;
 end_index = StreamBlock->WWRcvNumBufs;

////////////////////////////////////////////////////////////////
 //Get into each index of the table 
 for ( count = start_index; count < end_index; count++)
 {
       //Alloc pmercd_dma_descr_sT
       pdmadescr = (pmercd_ww_dma_descr_sT) mercd_allocator(MERCD_WW_BIGMSG_DESCR);

       if (pdmadescr == NULL)
        {
          printk("mid_wwmgr_allocate_mem_for_rcvdatablks_index_table pdmadescr Alloc Failed\n");
          Status = MD_FAILURE;
          return(Status);
        }

       //Bzero the pdmadescr memory..
       MSD_ZERO_MEMORY(pdmadescr, MERCD_WW_BIGMSG_DESCR);
       //We Need One More for BStream
       if (!(StreamBlock->type ==  STREAM_OPEN_F_GSTREAM))
         {
          pdmadescr->next = (pmercd_ww_dma_descr_sT) 
                            mercd_allocator(MERCD_WW_BIGMSG_DESCR);

          if (pdmadescr->next == NULL)
           {
             printk("mid_wwmgr_allocate_mem_for_rcvdatablks_index_table pdmadescr->next Alloc Failed\n");
             Status = MD_FAILURE;
             return(Status);
           }

          //Bzero the pdmadescr->next memory..
          MSD_ZERO_MEMORY(pdmadescr->next, MERCD_WW_BIGMSG_DESCR);
         }

         StreamBlock->pRcvStrmDmaDesc[count] = pdmadescr;
  }/* for */

  return (Status = MD_SUCCESS);
}


