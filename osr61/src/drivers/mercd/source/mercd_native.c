/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File name                    : mercd_native.c
 * Description                  : Native Driver file entry points
 *
 *
 **********************************************************************/

#if !defined LiS && !defined LIS && !defined LFS
#include <linux/module.h>
#include <linux/version.h>

#include "msd.h"
#define _MERCD_C
#include "msdextern.h"
#undef _MERCD_C


/**********************************************************************
 *  FunctionName:  mercd_open()
 *        Inputs:  inode - information structure for device node
 *                 file  - file structrure for device node.
 *       Outputs:  none.
 *       Returns:  ErrorCode. 0=Success. Negative=Error
 *                 -ENFILE = Could not allocate another Queue.  
 *                           Reached Max Queues allowed.
 *                 -ENOMEM = Ran out of memory trying to allocate Queue.
 *                 -ENODEV = Wrong minor number for device node.
 *   Description:  Called upon file open on the device node.
 *      Comments:  
 **********************************************************************/
int mercd_open(struct inode *inode, struct file *file){
  
   unsigned int 	minor = MINOR(inode->i_rdev);
   static merc_uint_t   dev = 0;
   pmercd_open_block_sT MsdOpenBlock;

   MSD_ENTER_CONTROL_BLOCK_MUTEX_BH();

   if (!MsdControlBlock->maxopen) {
       mercd_zalloc(MsdControlBlock->popen_block_queue, 
                    pmercd_open_block_sT *, MERCD_INITIAL_OPEN_BLOCK_LIST);

	mercd_zalloc(MsdControlBlock->popen_free_block_queue,
                    pmercd_open_block_sT *, (MSD_MAX_OPENTOBE_FREED_INDEX+1) * sizeof(pmercd_open_block_sT));

       if (MsdControlBlock->popen_block_queue == NULL) {

           MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
           printk("mercd_open: memory allocation failed\n");
           return(ENODEV);
       }

       MsdControlBlock->maxopen = MSD_INITIAL_OPEN_ALLOWED;
       MsdControlBlock->open_block_count = 0;

   }

   if (MsdControlBlock->open_block_count >= MsdControlBlock->maxopen) {
       MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
       printk( "mercd_open: too many Opens %d \n", MsdControlBlock->open_block_count);
       return (ENODEV);
   }

   /* find an empty block for this open */
   for (dev %= MsdControlBlock->maxopen; dev < MsdControlBlock->maxopen; dev++)
        if (MsdControlBlock->popen_block_queue[ dev ] == NULL)
            break;

   /* Allocate MsdOpenBlock */
   mercd_zalloc( MsdOpenBlock, pmercd_open_block_sT, sizeof(mercd_open_block_sT) );


   if (MsdOpenBlock == NULL) {
       MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
       printk("mercd_open: memory allocation failed\n");
       return(ENODEV);
   }


   MsdControlBlock->popen_block_queue[dev] = MsdOpenBlock;
   MsdControlBlock->open_block_count++;

   //MERCD_GET_MAJOR_NODE( dev );

   MSD_INIT_MUTEX(&MsdOpenBlock->open_block_mutex, NULL, NULL);

   MSD_INIT_MUTEX(&MsdOpenBlock->readers_queue_mutex, NULL, NULL);

   MsdOpenBlock->clone_device_id = dev;
   MsdOpenBlock->state = MERCD_OPEN_STATE_OPENED;
   MsdOpenBlock->SavedMsg = NULL;
   MsdOpenBlock->kernelBuf = MSD_ALLOCATE_KERNEL_MEMORY(MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER));

   // Array Implementation
   if (MsdControlBlock->arraySz) {
       mercd_zalloc(MsdOpenBlock->readersArray.array, Uint32 *, (sizeof(Uint32 *)*MsdControlBlock->arraySz));
   } else {
       printk("mercd_open: Readers Array size not set.\n");
       MsdControlBlock->arraySz = MSD_ARRAY_SIZE;
       mercd_zalloc(MsdOpenBlock->readersArray.array, Uint32 *, (sizeof(Uint32 *)*MsdControlBlock->arraySz));
   }
   if (MsdOpenBlock->readersArray.array == NULL) {
       printk("mercd_open: memory allocation failed for array\n");
       MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
       return(ENODEV);
   }

   MsdOpenBlock->readersArray.head = 0;
   MsdOpenBlock->readersArray.tail = 0;

   file->private_data = (void *)MsdOpenBlock;

   MsdOpenBlock->up_stream_write_q  = (pmercd_strm_queue_sT) MsdOpenBlock;

   MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();

#ifdef LINUX24	
   MOD_INC_USE_COUNT;
#endif

   INIT_WAITQUEUE(MsdOpenBlock->readers);  
  
   return(0);
}

/**********************************************************************
 *  FunctionName:  mercd_close()
 *        Inputs:  inode - information structure for device node
 *                 file  - file structrure for device node.
 *       Outputs:  none.
 *       Returns:  ErrorCode. 0=Success. Negative=Error
 *                 -ENODEV = Wrong minor number for device node.
 *   Description:  Called upon file close on the device node.
 *      Comments:  
 **********************************************************************/
int mercd_close(struct inode *inode, struct file *file){
  
   unsigned int 	minor = MINOR(inode->i_rdev);
   pmercd_open_block_sT MsdOpenBlock =  (pmercd_open_block_sT)file->private_data;
   pmercd_bind_block_sT BindBlock;
   int i,index;
   PSTRM_MSG       Msg;

   if (MsdOpenBlock == NULL) {
       printk("mercd_close: q->q_ptr null\n");
       return(ENODEV);
   }

   MSD_ENTER_CONTROL_BLOCK_MUTEX_BH();

   MsdOpenBlock->state = MERCD_OPEN_STATE_CLOSE_PEND;

   i = 0 ;
   while (BindBlock = (pmercd_bind_block_sT) MsdOpenBlock->bind_block_q.QueueHead) {
	i++;
	  if (BindBlock->bindhandle == 0) 
              break;
       
	  supp_do_unbind(BindBlock);

	  if(i>40000) {
		printk("Breaking the loop here ..., BindBlock is %x \n",BindBlock);
		break;
	   }
   }


   index = MsdControlBlock->OpenFreeIndex;

   if (MsdControlBlock->popen_free_block_queue[index]) 
       MSD_FREE_KERNEL_MEMORY((pmerc_char_t)MsdControlBlock->popen_free_block_queue[index], sizeof(mercd_open_block_sT));


    MSD_ENTER_MUTEX(&MsdOpenBlock->readers_queue_mutex);
 
    while((Msg = queue_get_msg_Array(&MsdOpenBlock->readersArray, MSD_READER)) != NULL) {
	   mercd_streams_free(Msg, MERCD_STREAMS_BUFFER, MERCD_FORCE);
           queue_commit_msg_Array(&MsdOpenBlock->readersArray, MSD_READER);
    }

    MsdOpenBlock->readersArray.head = 0;
    MsdOpenBlock->readersArray.tail = 0;

    MSD_FREE_KERNEL_MEMORY((Uint32 *)MsdOpenBlock->readersArray.array, sizeof(Uint32 *) *MsdControlBlock->arraySz);
    MSD_FREE_KERNEL_MEMORY(MsdOpenBlock->kernelBuf, MERCURY_HOST_IF_BLK_SIZE + sizeof(USER_HEADER));

    if(MsdOpenBlock->SavedMsg)
      mercd_streams_free(MsdOpenBlock->SavedMsg,MERCD_STREAMS_BUFFER, MERCD_FORCE);

    MSD_EXIT_MUTEX(&MsdOpenBlock->readers_queue_mutex);

    MsdControlBlock->popen_free_block_queue[index] = MsdOpenBlock;

    MsdControlBlock->OpenFreeIndex++;

    MsdControlBlock->OpenFreeIndex %= MSD_MAX_OPENTOBE_FREED_INDEX;

    MsdControlBlock->popen_block_queue[MsdOpenBlock->clone_device_id] = NULL;

    MsdControlBlock->open_block_count--;

    MSD_DESTROY_MUTEX(&MsdOpenBlock->open_block_mutex);

    MSD_DESTROY_MUTEX(&MsdOpenBlock->readers_queue_mutex);

#ifdef LINUX24	
    MOD_DEC_USE_COUNT;
#endif

    MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
  
    return(0);
}

/**********************************************************************
 *  FunctionName:  mercd_read()
 *        Inputs:  file   - file structure for device
 *                 buffer - user buffer to fill in read device data.
 *                 size   - size of the buffer.
 *                 ppos   - offset into device.
 *       Outputs:  buffer - buffer is filled during the function call.
 *       Returns:  ssize_t >= 0 bytes copied from queue (should be size)
 *                 ssize_t <  0 error condition
 *                 -EAGAIN = Queue is empty and the function was asked 
 *                 	      not to block.
 *                 -EINTR  = While blocked waiting for a message the 
 *                 	     function was interrupted by a signal.    
 *                 	     No message will be returned.
 *                 -EFAULT = Memory access error when trying to copy msg 
 *              	     into user buffer.
 *                 -ENOSYS = System call not  supported for that device.  
 *                 	     Queues are the only ones that support read.
 *                 -ENODEV = Invalid Minor number device. Not supported.
 *                 -ESPIPE = Pipe operation  called,  but not supported.
 *                 
 *   Description:  Read data from the mercd device.  Queues are the only 
 * 		    ones that support read. Read message from the queue.
 *
 *      Comments: The readv implementation of linux is broken because it 
 *      	   calls read underneath.  To fix   readv implementation 
 *      	   we will  not block  if  the sent in size is 1.  Since 
 *      	   the size field is not  used, because all messages are 
 *      	   of uniform size. If we don't do this we will possibly 
 *      	   block readv  in the  middle  of  retrieving messages. 
 *      	   readv  should return if it had recieved any messages.  
 *      	   We  have no way to tell if calle  by  readv  or read.
 **********************************************************************/
ssize_t mercd_read(struct file *file, char *buffer, 
		   size_t size, loff_t *ppos)
{
   unsigned int minor = MINOR(file->f_dentry->d_inode->i_rdev);
   pmercd_open_block_sT    MsdOpenBlock =  (pmercd_open_block_sT)file->private_data;  

#ifdef LINUX24  
   // We don't support pread() or pwrite()
   if (ppos != &file->f_pos)
       return -ESPIPE;
#endif

   return(mercd_readQueue(MsdOpenBlock, size, buffer,
            ((file->f_flags & O_NONBLOCK) ? TRUE : FALSE)));
   
   return(0);
}

/**********************************************************************
 *  FunctionName:  mercd_write()
 *
 *        Inputs:  file   - file structure for device
 *                 buffer - buffer containing data to be written
 *                 size   - size of data to be copied into the driver
 *                 ppos   - offset within the device.
 *
 *       Outputs:  none.
 *  
 *       Returns:  ssize_t >= 0 bytes copied from queue (should be size)
 *                 ssize_t <  0 error condition
 *                 -EINVAL = boardNumber out of range or invalid.  
 *                 		No board at that Id.
 *                 -ENOTTY = board not in the Running State. 
 *                 		Messages can only be sent when the board
 *                 		is in the Running State.
 *                 -EFAULT = Memory access error when trying to copy 
 *                 		buffer to board queue.
 *                 -ESPIPE = Pipe operation called, but not supported.
 *                 -ENOSYS = System call not supported for that device.  
 *                 		Queues are the only ones that support write.
 *                 -ENODEV = Invalid Minor numbered device. Not supported.
 *
 *   Description:  Copies the driver message buffer to the inbound board 
 *   		   queue of the board with the BoardNumber boardId. 
 *
 *      Comments:  
 **********************************************************************/
ssize_t mercd_write(struct file *file, const char *buffer, 
		    size_t size, loff_t *ppos)
{
   unsigned int  minor = MINOR(file->f_dentry->d_inode->i_rdev);
   PMDRV_MSG     MdMsg;
   PSTRM_MSG 	 Msg;
   unsigned char *drvrbuffer=NULL;
   unsigned char *savedbuffer=NULL;
   msgb_t        *tempMsg;
   pmercd_open_block_sT MsdOpenBlock = (pmercd_open_block_sT)file->private_data;  
 
#ifdef LINUX24  
   // We don't support pread() or pwrite()
   if (ppos != &file->f_pos)
       return -ESPIPE;
#endif

   // allocate kernel memory since we should not touch user memory in kernel mode
   drvrbuffer = MSD_ALLOCATE_KERNEL_MEMORY(size);
   if (!drvrbuffer) {
       printk("mercd_write: unable to allocate memory %d\n", size);
       return -ENOMEM;
   } 

   // copy user memory into kernel memory
   if (copy_from_user(drvrbuffer, buffer, size)) {
       printk("mercd_write: unable to copy user memory to kernel\n");
       MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
       return -EFAULT;
   }

   MSD_ENTER_MUTEX(&MsdOpenBlock->open_block_mutex);

   MdMsg = (PMDRV_MSG)drvrbuffer;
  
   if (!MsdOpenBlock->SavedMsg) {
       if ((MD_GET_MDMSG_ID(MdMsg) > MSD_MAX_DRV_INTER_MESSAGES) ||
           (MD_GET_MDMSG_ID(MdMsg) <= 0)) {
            printk("mercd_write: Invalid Msg Id %x of size %d \n", MD_GET_MDMSG_ID(MdMsg), size);
            MsdOpenBlock->SavedMsg = 0;
            MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
            MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);
	    return -EINVAL;
       }
   } else {
       int class;

       class = MD_GET_MDMSG_CLASS(MD_EXTRACT_MDMSG_FROM_STRMMSG(MsdOpenBlock->SavedMsg));
	 
       if (class != size) {
	   printk("mercd_write: Unmatching Message Class %d and  %d  %x \n", class, size, 
                  MD_GET_MDMSG_ID((PMDRV_MSG)MD_EXTRACT_MDMSG_FROM_STRMMSG(MsdOpenBlock->SavedMsg)));
           printk("mercd_write: Current Message Class %#x Id %#x\n", MdMsg->MessageClass, MdMsg->MessageId);
           MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
           MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);
	   return -EINVAL;
       }
   }

   if (MsdOpenBlock->SavedMsg) { 
       
       MdMsg = (PMDRV_MSG)MD_EXTRACT_MDMSG_FROM_STRMMSG(MsdOpenBlock->SavedMsg);
       tempMsg = MsdOpenBlock->SavedMsg;
       MsdOpenBlock->SavedMsg = 0;
       MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);

       savedbuffer = mercd_allocator(size);

       if (savedbuffer) {

           // Check for a multi block messages as we can split it into 
           // multiple buffers instead of using the one buffer to allocate
           if (MD_GET_MDMSG_ID(MdMsg) == MID_STREAM_MBLK_SEND) {
               // since we will be in bh level 
	       // we cannot acces user buffer, so... 
               copy_from_user(savedbuffer, buffer, size);
               MSD_ENTER_CONTROL_BLOCK_MUTEX_BH();
               if (mid_strmmgr_mblk_split_send(MsdOpenBlock, tempMsg, 
		              savedbuffer, size) != MD_SUCCESS) {
	           mercd_free(savedbuffer, size, MERCD_FORCE);
                   MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
                   MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
                   return -EINVAL;
               }
	       mercd_free(savedbuffer, size, MERCD_FORCE);
               MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
               MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
               return(size);
           }

           MSD_ENTER_CONTROL_BLOCK_MUTEX_BH();
	   Msg = native_alloc_msg_desr(size, (char *)buffer);
   	   if (!Msg) {
               printk("mercd_write: memory allocation failed %d\n", size);
	       mercd_free(savedbuffer, size, MERCD_FORCE);
               MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
               MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
       	       return -EINVAL;
   	   }
           MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();

       	   Msg->b_datap->db_base = savedbuffer;
       	   Msg->b_datap->db_lim = (char *)savedbuffer + size; 
       	   Msg->b_rptr = (char *)savedbuffer;
       	   Msg->b_wptr = (char *)savedbuffer + size;
	   copy_from_user(savedbuffer, buffer, size);
       
           // Check is there is any contination in the buffer....
           tempMsg->b_cont = Msg;
       	} else {
           printk("mercd_write: memory allocation failed %d\n", size);
           MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
	   return -EINVAL;
       	}
   } else {
       if ((merc_ushort_t)MdMsg->MessageClass) {

	   savedbuffer = mercd_allocator(size);

       	   if (savedbuffer) {
               MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);
               MSD_ENTER_CONTROL_BLOCK_MUTEX_BH();
	       Msg = native_alloc_msg_desr(size, (char *)buffer);
   	       if (!Msg) {
                   printk("mercd_write: memory allocation failed %d (2)\n", size);
		   mercd_free(savedbuffer, size, MERCD_FORCE);
                   MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
           	   MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
       		   return -EINVAL;
   	       }
               MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
	       
               Msg->b_datap->db_base = savedbuffer;
	       Msg->b_datap->db_lim = (char *)savedbuffer + size;
	       Msg->b_rptr = (char *)savedbuffer;
	       Msg->b_wptr = (char *)savedbuffer + size;
	       copy_from_user(savedbuffer, buffer, size);

       	       MdMsg = (PMDRV_MSG)MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);

       	       if ((MD_GET_MDMSG_ID(MdMsg) > MSD_MAX_DRV_INTER_MESSAGES) ||
            		  	           (MD_GET_MDMSG_ID(MdMsg) <= 0)) {
                   MSD_ENTER_CONTROL_BLOCK_MUTEX_BH();
		   mercd_free(savedbuffer, size, MERCD_FORCE);
                   MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
           	   MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
	   	   return -EINVAL;
       	       }

  	       MsdOpenBlock->SavedMsg = Msg;
	   } else {
  	       MsdOpenBlock->SavedMsg = 0; 
               MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
               MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);
	       return -EINVAL;
 	   }

           MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
	   return(size);
       } else { 
		
	   savedbuffer = mercd_allocator(size);
	   if (savedbuffer) {
               MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);
               MSD_ENTER_CONTROL_BLOCK_MUTEX_BH();
	       Msg = native_alloc_msg_desr(size, (char *)buffer);
   	       if (!Msg) {
                   printk("mercd_write: memory allocation failed %d (3)\n", size);
	      	   mercd_free(savedbuffer, size, MERCD_FORCE);
                   MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
           	   MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
       		   return -EINVAL;
   	       }
               MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();

	       Msg->b_datap->db_base = savedbuffer;
	       Msg->b_datap->db_lim = (char *)savedbuffer + size;
	       Msg->b_rptr = (char *)savedbuffer;
               Msg->b_wptr = (char *)savedbuffer + size;
               copy_from_user(savedbuffer, buffer, size);
               tempMsg = Msg;
	   } else {
  	       MsdOpenBlock->SavedMsg = 0; 
               MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
               MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);
	       return -EINVAL;
 	   }
       }
   }

   MdMsg = (PMDRV_MSG)MD_EXTRACT_MDMSG_FROM_STRMMSG(tempMsg);
   MdMsg->MessagePtr = 0;
   MdMsg->MessageFlags = MD_MSG_FLAG_ASYNC;

   if ((MD_GET_MDMSG_ID(MdMsg) > MSD_MAX_DRV_INTER_MESSAGES) ||
                               (MD_GET_MDMSG_ID(MdMsg) <= 0)) {
       MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
       return -EINVAL;
   }

   MSD_ENTER_CONTROL_BLOCK_MUTEX_BH();
   (*mercd_osal_mid_func[MD_GET_MDMSG_ID(MdMsg)])(MsdOpenBlock, tempMsg);
   MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();

   MSD_FREE_KERNEL_MEMORY(drvrbuffer, size);
   return(size);
}


/**********************************************************************
 *  FunctionName:  mercd_poll()
 *
 *        Inputs:  file - file structure for device
 *                 wait - poll table structure.      
 *     
 *       Outputs:  none.
 *  
 *       Returns:  mask - MASK of poll event flags.
 *
 *   Description:  Poll responds with what conditions exist in the device.
 *                 Both streams and queues support poll.  
 *
 *      Comments:  Queues support READ  and WRITE  Polling. Write will 
 *      	   always return true.  It will always queue a message 
 *      	   inbound to a board.  Read will  only return true if 
 *      	   there is a message on the outbound queue.
 *
 *                 Streams support Write and Error reporting  polliing.  
 *                 Write will be returned true if there are no commands 
 *                 active on the stream.  Errors will be returned if an 
 *                 error code exists on the stream. This marks that the 
 *                 last stream command failed.
 **********************************************************************/  
unsigned int mercd_poll(struct file *file, struct poll_table_struct *wait)
{
   unsigned int 	minor = MINOR(file->f_dentry->d_inode->i_rdev);
   unsigned int 	mask =  0;
   pmercd_open_block_sT MsdOpenBlock =  (pmercd_open_block_sT)file->private_data;  

   poll_wait(file, &(MsdOpenBlock->readers), wait);

	
   if (!MSD_ARRAY_EMPTY(MsdOpenBlock->readersArray)) {
       mask |=  POLLIN | POLLRDNORM | POLLOUT | POLLWRNORM;  // Readable.
   } else {
       mask = 0;
   }
  
   return mask;
}

/**********************************************************************
 *  FunctionName:  mercd_mmap()
 *
 *        Inputs:  file - device file structure.
 *                 vma - virtual memory region description of mmap region.
 *
 *       Outputs:  none.
 *  
 *       Returns:  ErrorCode. 0=Success. Negative=Error
 *                 -EINVAL = VM address not aligned, or VM region not same 
 *                 	     size as stream buffer.
 *                 -ENOTTY = Stream has not been attached to the file handle.  
 *                 	     This must be done first. (Attach Stream then
 *                 	     Mmap stream buffers the board is in the Running State.)
 *                 -ENXIO  = remapping of memory region failed.
 *                 -ENOSYS = System call not supported for that device.  Streams 
 *                 	     are the only devices that support mmap.
 *
 *   Description:  Maps kernel buffers into a users virtual address space.
 *
 *      Comments:  Only stream devices support mmap.  The stream buffers are 
 *                 mmapped into the user address space.
 **********************************************************************/  
int mercd_mmap(struct file *file, struct vm_area_struct *vma){
#if (LINUX_VERSION_CODE < 0x020400)
   unsigned long offset = vma->vm_offset;
#else
   unsigned long offset = vma->vm_pgoff<<PAGE_SHIFT;
#endif
   unsigned int  minor = MINOR(file->f_dentry->d_inode->i_rdev);
   unsigned int  i; 
   merc_uchar_t  sem;
   pmercd_adapter_block_sT padapter;

   // Verify that stream has been attached to the file handle.
   if (offset > 0 ) {
       printk("mercd_mmap: offset is %d \n", offset);
       return -EINVAL;
   }

   if (mercd_adapter_map[MsdControlBlock->sramDumpCfgId] == 0xFF) {
       printk("mercd_mmap: invaild config id\n");
       return -EINVAL;
   }

   padapter = MsdControlBlock->padapter_block_list[mercd_adapter_map[MsdControlBlock->sramDumpCfgId]];

   if (!padapter) {
       printk("mercd_mmap: no adapter number %d found\n", MsdControlBlock->sramDumpCfgId);
       return -EINVAL;
   }

   if ((padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DM3) &&
       (padapter->phw_info->pciSubSysId != PCI_SUBDEVICE_ID_DISI)) {
       printk("mercd_mmap: non-Sram board\n");
       return -EINVAL;
   }

   if ((padapter->state == MERCD_ADAPTER_STATE_SUSPENDED) ||
          (padapter->state == MERCD_ADAPTER_STATE_MAPPED)) {
       printk("mercd_mmap: Invalid Board State\n");
       return -EINVAL;
   }

   // Request the host ram without asking for interrupt
   padapter->phost_info->reg_block.SetHostRamRequest =
        padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->virt_addr + MERC_SET_HOST_RAM_REQUEST_REG;	
   MERC_GET_SRAM_NO_INTR_LOCK((&padapter->phost_info->reg_block), sem);

   if (!(sem & MERC_HOST_RAM_GRANT_R) || (sem == 0xFF)) {
       printk("mercd_mmap: unable to acquire sram\n");
       return -EINVAL;
   }

   if (remap_page_range(vma, vma->vm_start, 
			padapter->phw_info->virt_map_q[MERCD_PCI_SRAM_MAP]->phys_addr, 
			MsdControlBlock->sramDumpSz, vma->vm_page_prot)) {
       printk("mercd_mmap: remap_page_range failed\n");
       return -EINVAL;
   }
   
   return 0;
}


/**********************************************************************
 *  FunctionName:  mercd_ioctl()
 *
 *        Inputs:  inode - information structure for device node
 *                 file  - file structrure for device node.
 *                 functionId - Ioctl command identifier
 *                 arg - argument that can be used by the ioctl call
 *
 *       Outputs:  none.
 *  
 *       Returns:  ErrorCode. 0=Success. Negative=Error
 *
 *   Description:  Kernel driver IOCTL entry point.  Allows for extension 
 *   		   functions to be added to the driver.
 *
 *      Comments:  Each IOCTL has its own set of error values.  Each 
 *      	   should be documented in the library.
 **********************************************************************/  
int mercd_ioctl(struct inode *inode, struct file *file, 
		unsigned int functionId, unsigned long arg){
  
   pmercd_open_block_sT MsdOpenBlock =  (pmercd_open_block_sT)file->private_data;
   unsigned int 	minor = MINOR(inode->i_rdev);
   int             	timeout_val,len,command;
   int  		size = 0;
   PMDRV_MSG       	MdMsg;
   PSTRM_MSG 	  	Msg;
#ifdef LINUX24
   void *argp = (void *)arg;
#else
   void __user *argp = (void __user *)arg;
#endif

   switch (functionId) {
       case 0x100:
	    //printk("Get command %x \n", command);
	    ctimod_debug_on();
	    return(0);
       case 0x101:
	    //printk("Get command %x \n", command);
	    ctimod_debug_off();
	    return(0);

       case 0x102:
	    //printk("Get command %x \n", command);
	    ctimod_debug_clear();
	    return(0);
       case 0x103:
	    //printk("Get command %x \n", command);
	    ctimod_debug_print();
	    return(0);

 	case 0x104:
	    //printk("Get command %x \n", command);
	    ctimod_debug_toggle();
	    return(0);

      default:
	    break;
   }

   command = functionId >> 24;
   len = (functionId & 0x00FFFF00) >> 8;
   timeout_val = functionId & 0x000000FF;

   // allocate kernel memory since we should not touch user memory in kernel mode
   MdMsg = MSD_ALLOCATE_KERNEL_MEMORY(len);
   if (!MdMsg) {
       printk("mercd_ioctl: unable to allocate memory %d\n", len);
       return -ENOMEM;
   }

   // copy user memory into kernel memory
   if (copy_from_user(MdMsg, argp, len)) {
       printk("mercd_ioctl: unable to copy user memory to kernel\n");
       MSD_FREE_KERNEL_MEMORY(MdMsg, sizeof(MDRV_MSG));
       return -EFAULT;
   }

   MSD_ENTER_CONTROL_BLOCK_MUTEX_BH();

   switch (command) {
       case MD_COMMAND_IOCTL:
	    // We need to cover the message with the mblk and 
	    // put the size around it That way  we do not have 
	    // to change anything around the code.
	    Msg = native_alloc_msg_desr(len, (char *)MdMsg);
  
	    // Protect the driver from any harm user can make
    	    if ((MD_GET_MDMSG_ID(MdMsg) > MSD_MAX_DRV_INTER_MESSAGES) ||
     				          (MD_GET_MDMSG_ID(MdMsg) <= 0))  {
   	       MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
 	       MSD_FREE_KERNEL_MEMORY(MdMsg, len);
	       return -EINVAL;
	    }

	    MdMsg->MessageFlags = MD_MSG_FLAG_SYNC;

	    if (Msg) {
	       (*mercd_osal_mid_func[MD_GET_MDMSG_ID(MdMsg)])(MsdOpenBlock,Msg);
	    } else {
   	       MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
 	       MSD_FREE_KERNEL_MEMORY(MdMsg, len);
	       return -EINVAL;
 	    }

	    break;

       default:
   	    MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
 	    MSD_FREE_KERNEL_MEMORY(MdMsg, len);
	    return -EINVAL;
   }

   MSD_EXIT_CONTROL_BLOCK_MUTEX_BH();
   copy_to_user(argp, MdMsg, len);
   MSD_FREE_KERNEL_MEMORY(MdMsg, len);
   return(0);
}
#endif
