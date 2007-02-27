/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_linux_entry.c
//
// Kernel entry functions.
/////////////////////////////////////////////////////////////////////////////

#include "pmacd_linux_entry.h"
#include "pmacd_driver.h"
#include "pmacd_parameters.h"
#include "pmacd_interrupt.h"
#include "pmacd_control.h"
#include "pmacd_stream.h"
#include "pmacd_proc.h"
#include "21554.h"

#define PMACD_MINOR minor

//__initdata ATOMIC_T pmacd_dynamicMemoryUsage;
ATOMIC_T pmacd_dynamicMemoryUsage = { 0 };

// Defined in pmacd_driver.c
//extern pmacd_driver_t pmacd_driver;

// MODULE Parameters
#ifdef LFS
int  pmacd_major_number   = PMACD_MAJOR;
char pmacd_string[]       = "pmac";
char *pmacd_driver_name;
#else
static int  pmacd_major_number   = PMACD_MAJOR;
static char pmacd_string[]       = "pmac";
static char *pmacd_driver_name;
#endif

static long pmacd_msg_size            = PMACD_MF_SIZE;
static long pmacd_mf_pull_option      = pmacd_MFPullOption_AllOnHost;
static long pmacd_queue_size          = PMACD_MF_COUNT;
static long pmacd_streams_per_board   = PMACD_STREAMS_PER_BOARD;
static long pmacd_buffers_per_stream  = PMACD_BUFFERS_PER_STREAM;
static long pmacd_stream_buffer_size  = PMACD_STREAM_BUFFER_SIZE;

MODULE_AUTHOR("Intel Corp. (c) 2001");
MODULE_DESCRIPTION("Packet Media Access Card Driver.\n Version:"__MODULE_STRING(PMACD_VERSION));
#ifdef LINUX24
MODULE_PARM(pmacd_major_number,        "i");
MODULE_PARM(pmacd_loopback_count,      "i");
MODULE_PARM(pmacd_driver_name,         "s");
MODULE_PARM(pmacd_msg_size,            "l");
MODULE_PARM(pmacd_mf_pull_option,      "l");
MODULE_PARM(pmacd_queue_size,          "l");
MODULE_PARM(pmacd_streams_per_board,   "l");
MODULE_PARM(pmacd_buffers_per_stream,  "l");
MODULE_PARM(pmacd_stream_buffer_size,  "l");
#else
module_param(pmacd_major_number,int,0);
module_param(pmacd_driver_name,charp,0);
module_param(pmacd_msg_size,long,0);
module_param(pmacd_mf_pull_option,long,0);
module_param(pmacd_queue_size,long,0);
module_param(pmacd_streams_per_board,long,0);
module_param(pmacd_buffers_per_stream,long,0);
module_param(pmacd_stream_buffer_size,long,0);
#endif
MODULE_PARM_DESC(pmacd_driver_name, "PMAC driver name");
MODULE_LICENSE("GPL");
#ifdef LFS
#ifdef MODULE_ALIAS
MODULE_ALIAS("pmacd");
MODULE_ALIAS("streams-pmacd");
MODULE_ALIAS("streams-pmacdDriver");
#endif
#endif
int RH_Enabled = 0;

// File operations structure used by the OS for calling the driver
// entry points.
static struct file_operations pmacd_fops =
{
  .owner   =   THIS_MODULE,
  .read    =   pmacd_read,
  .write   =   pmacd_write,
  .poll    =   pmacd_poll,
  .open    =   pmacd_open,
  .release =   pmacd_release,
  .ioctl   =   pmacd_ioctl,
  .mmap    =   pmacd_mmap
};


static int pmacd_init(struct pci_dev *pcidev, const struct pci_device_id *ent);
static void pmacd_remove(struct pci_dev *pcidev);
static int pmacd_resume(struct pci_dev *pdev);
#ifdef LINUX24
static int pmacd_suspend(struct pci_dev *pdev, u32 status);
#else
static int pmacd_suspend(struct pci_dev *pdev, pm_message_t status);
#endif

// Devices this driver can handle.  
// (vendor, device, subvendor, subdevice, class, class_mask, driver_data)
// list is zero terminated
static struct pci_device_id pmacd_pci_tbl[] __devinitdata = {
  { PMACD_VENDOR_ID_21554, PMACD_DEVICE_ID_21554, PMACD_SUBSYS_VENDOR_ID, PMACD_SUBSYS_DEVICE_ID, },
  { PMACD_VENDOR_ID_21555, PMACD_DEVICE_ID_21555, PMACD_SUBSYS_VENDOR_ID, PMACD_SUBSYS_DEVICE_ID, },
  { 0,}
};
MODULE_DEVICE_TABLE(pci, pmacd_pci_tbl);

// PCI Driver structure defining pci functions and devices supported by the
// driver.
static struct pci_driver pmacd_pci_driver = {
        .id_table =       pmacd_pci_tbl,
	.name     =       pmacd_string,
       	.probe    =       pmacd_init,
        .remove   =       pmacd_remove,
        .resume   =       pmacd_resume,
        .suspend  =       pmacd_suspend,
};


/*
 *  FunctionName:  pmacd_init_module()
 *
 *        Inputs:  none.
 *
 *       Outputs:  none.
 *  
 *       Returns:  ErrorCode. 0=Success. Negative=Error
 *                 -EIO = Could not get Major Number or allocate all PCI Devices.
 *
 *   Description:  Called by the OS when the driver is first loaded.
 *                 The driver initializes all of its structures, and
 *                 allocates any boards that it finds in the system.
 *
 *      Comments:  
 */
static int __init pmacd_init_module(void)
{
  int returnVal, rc;

  ATOMIC_SET(pmacd_dynamicMemoryUsage,0);
  pmacd_initDriver(pmacd_msg_size,
		   pmacd_mf_pull_option,
		   pmacd_queue_size,
		   pmacd_queue_size,
		   pmacd_stream_buffer_size,
		   pmacd_buffers_per_stream,
		   pmacd_streams_per_board);
  
  // Register Driver
  returnVal = register_chrdev(pmacd_major_number, 
			      pmacd_string,
			      &pmacd_fops);
  if(returnVal < 0){
    pmacd_printError("unable to get major number %i.\n", 
	   pmacd_major_number);
    return (-EIO);
  }
  // If the initial major number was 0, the major number is
  // dynamically allocated and returned as the return value.
  if(!pmacd_major_number)pmacd_major_number = returnVal;
 
  if (ctimod_check_rh()) {
      RH_Enabled = 1;
      pmacd_printDebug("pmacd: RH Enabled 0x%x\n", RH_Enabled);
  }
 
  pmacd_printDebug("Driver Loaded (Major# = %i)\n",
	 pmacd_major_number);
  rc = pci_module_init(&pmacd_pci_driver);

  if ( rc ) { 
     unregister_chrdev(pmacd_major_number, pmacd_string);
     return rc;
  } 

  pmacd_registerProcDriverInfo();  
  return  rc;
  
}

/*
 *  FunctionName:  cleanup_module()
 *
 *        Inputs:  none.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  Called by the OS right before the driver is unloaded.
 *                 Any dynamic structures allocated in the driver are 
 *                 freed, and any other necessary cleanup is performed.
 *
 *      Comments:  
 */
static void __exit pmacd_cleanup_module(void)
{


  // Unregister Driver  
  pmacd_unregisterProcDriverInfo();
  unregister_chrdev(pmacd_major_number, pmacd_string);
  
  pci_unregister_driver(&pmacd_pci_driver);

  pmacd_printDebug("Driver Unloaded (Major# = %i)(DynMem = %i)\n",
		   pmacd_major_number, ATOMIC_GET(pmacd_dynamicMemoryUsage));

  return;
}

module_init(pmacd_init_module);
module_exit(pmacd_cleanup_module);

static void __devexit pmacd_remove(struct pci_dev *pciDev) {
  pmacd_freeBoardFromDriver(pciDev);
  return;
}

static int __devinit pmacd_init(struct pci_dev *pcidev,
                const struct pci_device_id *ent)
{
  pmacd_hs_t* hsd = NULL;
 
  hsd = kmalloc(sizeof(pmacd_hs_t), GFP_KERNEL);
  if (hsd == NULL) {
      pmacd_printDebug("pmacd_probe -> kmalloc error creating device object\n");
      return (0);
  }
  memset(hsd, 0x00, sizeof(pmacd_hs_t));
 
  /* Store a pointer to the device object in the kernels pci_dev object */
  pci_set_drvdata(pcidev, hsd);

  /* Indicate successful probe completition */
  hsd->state = PMACD_PROBED;

  // Find and Configure PCI Devices
  pmacd_configurePCIDevice(pcidev);
  return(0);
}

//========================================================================
//
//NAME                  :       pmacd_resume
//DESCRIPTION           :       Driver resume Routine called by RH or Hotplug
//                              Called for each board
// INPUTS               :       device info
// OUTPUTS              :       none
// RETURNS              :       none
// CALLS                        :
// CAUTIONS             :       Called by OS.
//
//========================================================================
int pmacd_resume(struct pci_dev *pciDev)
{
  pmacd_hs_t *hsd = NULL;
  
  hsd = (pmacd_hs_t*)pci_get_drvdata(pciDev);
  if (hsd == NULL) {
      pmacd_printDebug("pmacd_resume -> error retreiving hsd object\n");
      return(0);
  }

  pmacd_hardResetBoard(hsd->pBoard,FALSE);
  hsd->state = PMACD_RESUMED;
   
  return(0);
}
//========================================================================
//
//NAME                  :       pmacd_suspend 
//DESCRIPTION           :       called by RH or Hotplug 
//                              Called for each board
// INPUTS               :       device info
// OUTPUTS              :       none
// RETURNS              :       none
// CALLS                        :
// CAUTIONS             :       Called by OS.
//
//========================================================================
#ifdef LINUX24
int pmacd_suspend(struct pci_dev *pciDev, u32 status)
#else
int pmacd_suspend(struct pci_dev *pciDev, pm_message_t status)
#endif
{
  pmacd_hs_t *hsd = NULL;
  
  hsd = (pmacd_hs_t*)pci_get_drvdata(pciDev);
  if (hsd == NULL) {
      pmacd_printDebug("pmacd_resume -> error retreiving hsd object\n");
      return(0);
  }

   if (hsd->state != PMACD_RESUMED){
       pmacd_printDebug("pmacd_suspend -> Incorrect state \n");
       return (0);
   }
 
   // Only interrupts needs to be disabled here

  pmacd_quiesceBoard(hsd->pBoard);
  hsd->state = PMACD_SUSPENDED;
  return(0);
} 

/*
 *  FunctionName:  pmacd_open()
 *
 *        Inputs:  inode - information structure for device node
 *                 file  - file structrure for device node.
 *
 *       Outputs:  none.
 *  
 *       Returns:  ErrorCode. 0=Success. Negative=Error
 *                 -ENFILE = Could not allocate another Queue.  
 *                           Reached Max Queues allowed.
 *                 -ENOMEM = Ran out of memory trying to allocate Queue.
 *                 -ENODEV = Wrong minor number for device node.
 *
 *   Description:  Called upon file open on the device node.
 *
 *      Comments:  
 */
int pmacd_open(struct inode *inode, struct file *file){
#ifdef LINUX24  
  unsigned int minor = PMACD_MINOR(inode->i_rdev);
  MOD_INC_USE_COUNT;
#else
  unsigned int minor = iminor(inode);
#endif
  
  switch (minor){

  case PMACD_QUEUES_MINOR:{
    // Open new queue

    UINT8_T address;
    pmacd_pmbqueue_t *queue;

    MUTEX_ENTER(pmacd_driver.mutex);
    address = pmacd_getFreeQueueAddress();
    if(address == PMACD_MAX_QUEUES){
      pmacd_printError("Max queues (%i) reached: Queue create failed.\n", 
		       (int)PMACD_MAX_QUEUES);
      MUTEX_EXIT(pmacd_driver.mutex);
      return(-ENFILE);
    }
    
    queue = pmacd_allocatePMBQueue(pmacd_driver.parms.msgUnitConfig.msgSize,
				   pmacd_driver.parms.queueHighWaterMark);
    if (queue == NULL) {
      pmacd_printError("pmacd_open:Out Of Memory: Queue create failed.\n");
      MUTEX_EXIT(pmacd_driver.mutex);
      return(-ENOMEM);
    }

    queue->queueAddress = address;    
    pmacd_addQueueToDriver(queue);    
    file->private_data = (void *)queue;
    MUTEX_EXIT(pmacd_driver.mutex);
    break;
  }
  
  case PMACD_STREAMS_MINOR:
    // Resources allocated on attach.
    file->private_data = (void *)NULL;
    break;

  case PMACD_CONTROL_MINOR: 
    break;
    
  default:
    return -ENODEV;
  }
  
  return(0);
}
/*
 *  FunctionName:  pmacd_release()
 *
 *        Inputs:  inode - information structure for device node
 *                 file  - file structrure for device node.
 *
 *       Outputs:  none.
 *  
 *       Returns:  ErrorCode. 0=Success. Negative=Error
 *                 -ENODEV = Wrong minor number for device node.
 *
 *   Description:  Called upon file close on the device node.
 *
 *      Comments:  
 */
int pmacd_release(struct inode *inode, struct file *file){
#ifdef LINUX24  
  int minor = PMACD_MINOR(inode->i_rdev);
  MOD_DEC_USE_COUNT;
#else
  unsigned int minor = iminor(inode);
#endif  
  switch (minor){
  case PMACD_QUEUES_MINOR:{
    int i;
    UINT8_T srcNode;

    // No need to check private_data for validity, because fd will not
    // exist if it is not a valid queue.
    pmacd_pmbqueue_t *queue = (pmacd_pmbqueue_t *)file->private_data;

    // Here we do not have the boardnumber, so we send EN to all valid boards 
    // since the source node s unique only the matching board will respond
    for (i = 0; i < PMACD_MAX_BOARDS; ++i) {
        if (pmacd_driver.boardmap[i] != NULL ) {
    	   if  (queue->queueAddress) {
       	        srcNode = PMACD_HOSTBOARD_FLAG|(queue->queueAddress);
       		pmacd_ExitNotification(i, srcNode);
	    }
	 }
    }

    pmacd_driver.queues[queue->queueAddress] = (pmacd_pmbqueue_t *) NULL;
    ATOMIC_DEC(pmacd_driver.parms.queueCount);
    // freePMBQueue is mutex protected. (it won't free the queue until it
    // gets the queue mutex.
    pmacd_freePMBQueue(queue);
    
    break;
  }

  case PMACD_STREAMS_MINOR:{ 
    pmacd_stream_t *stream = (pmacd_stream_t *)file->private_data;
    if(stream != NULL){
      pmacd_detachStream(stream);
    }
    break;
  }
  
  case PMACD_CONTROL_MINOR: 
    break;

    
  default:
       	        pmacd_printDebug("pmacd_release: No such device\n"); 
    return -ENODEV;
  }
  return(0);
}
/*
 *  FunctionName:  pmacd_read()
 *
 *        Inputs:  file   - file structure for device
 *                 buffer - user buffer to fill in read device data.
 *                 size   - size of the buffer.
 *                 ppos   - offset into device.
 *
 *       Outputs:  buffer - buffer is filled in during the function call.
 *  
 *       Returns:  ssize_t >= 0 number of bytes copied from the queue. (should be size)
 *                 ssize_t <  0 error condition
 *                 -EAGAIN = Queue is empty and the function was asked not to block.
 *                 -EINTR  = While blocked waiting for a message the function was interrupted
 *                           by a signal.  No message will be returned.
 *                 -EFAULT = Memory access error when trying to copy msg into user buffer.
 *                 -ENOSYS = System call not supported for that device.  Queues are the only
 *                           ones that support read.
 *                 -ENODEV = Invalid Minor numbered device. Not supported.
 *                 -ESPIPE = Pipe operation called, but not supported.
 *                 
 *   Description:  Read data from the pmac device.  Queues are the only ones that support
 *                 read. (Read a message from the queue.)
 *
 *      Comments: The readv implementation of linux is broken because it calls read
 *                underneath.  To fix the readv implementation we will not block if 
 *                the sent in size is 1.  
 *                Since the size field is not used, because all messages are of
 *                uniform size. If we don't do this we will possibly block readv in the
 *                middle of retrieving messages. readv should return if it had recieved
 *                any messages.  We have no way to tell if we were called by readv or
 *                read.
 */
ssize_t pmacd_read(struct file *file, char *buffer, 
		   size_t size, loff_t *ppos)
{
#ifdef LINUX24
  int minor = PMACD_MINOR(file->f_dentry->d_inode->i_rdev);
  
  // We don't support pread() or pwrite()
  if(ppos != &file->f_pos) {
    pmacd_printDebug("pmacd_read: Error\n");
    return -ESPIPE;
  }
#else
  unsigned int minor = iminor(file->f_dentry->d_inode);
#endif
  
  switch (minor){
    
  case PMACD_QUEUES_MINOR:
    // To fix the readv implementation we will not block if the sent in size
    // is 1.  Since the size field is not used, because all messages are of
    // uniform size. If we don't do this we will possibly block readv in the
    // middle of retrieving messages. readv should return if it had recieved
    // any messages.  We have no way to tell if we were called by readv or
    // read.
    return(pmacd_readPMBQueue((pmacd_pmbqueue_t *)file->private_data,
	    size,
	    buffer,
	    ((size == 1 || file->f_flags & O_NONBLOCK) ? TRUE : FALSE)
	    ));
    
  case PMACD_STREAMS_MINOR: 
  case PMACD_CONTROL_MINOR: 
    pmacd_printDebug("pmacd_read: Invalid case\n");
    return -ENOSYS;
    
  default:
    pmacd_printDebug("pmacd_read: No such device\n");
    return(-ENODEV);
  }
  
}


/*
 *  FunctionName:  pmacd_write()
 *
 *        Inputs:  file   - file structure for device
 *                 buffer - buffer containing data to be written
 *                 size   - size of data to be copied into the driver
 *                 ppos   - offset within the device.
 *
 *       Outputs:  none.
 *  
 *       Returns:  ssize_t >= 0 number of bytes copied to the queue. (should be size)
 *                 ssize_t <  0 error condition
 *                 -EINVAL = boardNumber out of range or invalid (No board at that Id)
 *                 -ENOTTY = board not in the Running State. (Messages can only be sent when
 *                           the board is in the Running State.)
 *                 -EFAULT = Memory access error when trying to copy buffer to board queue.
 *                 -ESPIPE = Pipe operation called, but not supported.
 *                 -ENOSYS = System call not supported for that device.  Queues are the only
 *                           ones that support write.
 *                 -ENODEV = Invalid Minor numbered device. Not supported.
 *
 *   Description:  Copies the driver message buffer to the inbound board queue of the
 *                 the board with the BoardNumber boardId. 
 *
 *      Comments:  
 */
ssize_t pmacd_write(struct file *file, const char *buffer, 
		    size_t size, loff_t *ppos)
{
#ifdef LINUX24
  int minor = PMACD_MINOR(file->f_dentry->d_inode->i_rdev);
  
  // We don't support pread() or pwrite()
  if(ppos != &file->f_pos) {
    pmacd_printDebug("pmacd_write: Error\n");
    return -ESPIPE;
  }
#else
  unsigned int minor = iminor(file->f_dentry->d_inode);
#endif
  
  switch (minor){
    
    
  case PMACD_QUEUES_MINOR:{
    pmacd_pmbqueue_t *queue = (pmacd_pmbqueue_t *)file->private_data;
    return(pmacd_queueUserMessage(queue, buffer, size));
  }
  
  case PMACD_STREAMS_MINOR: 
  case PMACD_CONTROL_MINOR: 
    pmacd_printDebug("pmacd_write: Invalid case\n");
    return -ENOSYS;
  default:
    pmacd_printDebug("pmacd_write: No such device\n");
  return(-ENODEV);
  }
}


/*
 *  FunctionName:  pmacd_poll()
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
 *      Comments:  Queues support READ and WRITE Polling. Write will always return true.
 *                 It will always queue a message inbound to a board.  Read will only 
 *                 return true if there is a message on the outbound queue.
 *
 *                 Streams support Write and Error reporting polliing.  Write will be 
 *                 returned true if there are no commands active on the stream.  Errors
 *                 will be returned if an error code exists on the stream. (This marks that
 *                 the last stream command failed).
 */  
unsigned int pmacd_poll(struct file *file, struct poll_table_struct *wait)
{
#ifdef LINUX24
  int minor = PMACD_MINOR(file->f_dentry->d_inode->i_rdev);
#else
  unsigned int minor = iminor(file->f_dentry->d_inode);
#endif
  unsigned int mask =  0;

  
  switch (minor){

  case PMACD_QUEUES_MINOR:{
    pmacd_pmbqueue_t *queue = (pmacd_pmbqueue_t *)file->private_data;
    mask = POLLOUT | POLLWRNORM;   // Always writable
    
    poll_wait(file, &queue->readers, wait);
    if (!pmacd_isPMBQueueEmpty(queue))
      mask |=  POLLIN | POLLRDNORM;  // Readable.
    break;
  }

  case PMACD_STREAMS_MINOR:{
    pmacd_stream_t *stream = (pmacd_stream_t *)file->private_data;
    if(stream == NULL){
      mask |= POLLNVAL;
      break;
    }
    
    poll_wait(file, &stream->waitForResponse, wait);

    if(!pmacd_isStreamCommandActive(stream))
      mask |= POLLOUT | POLLWRNORM;   // Will accept a command
    
    if(pmacd_getStreamError(stream) != 0)
      mask |= POLLERR;  // Error on stream at present
    
    break;
  }
  
  case PMACD_CONTROL_MINOR: 
  default:
    pmacd_printDebug("pmacd_write: No such device\n");
  return(-ENODEV);
  }

  return mask;
}

/*
 *  FunctionName:  pmacd_mmap()
 *
 *        Inputs:  file - device file structure.
 *                 vma - virtual memory region description of mmap region.
 *
 *       Outputs:  none.
 *  
 *       Returns:  ErrorCode. 0=Success. Negative=Error
 *                 -EINVAL = VM address not aligned, or VM region not same size as stream
 *                           buffer.
 *                 -ENOTTY = Stream has not been attached to the file handle.  This must
 *                           be done first. (Attach Stream then Mmap stream buffers)
 *                           the board is in the Running State.)
 *                 -ENXIO = remapping of memory region failed.
 *                 -ENOSYS = System call not supported for that device.  Streams are the
 *                           only devices that support mmap.
 *
 *   Description:  Maps kernel buffers into a users virtual address space.
 *
 *      Comments:  Only stream devices support mmap.  The stream buffers are 
 *                 mmapped into the user address space.
 */  
int pmacd_mmap(struct file *file, struct vm_area_struct *vma){
  unsigned long offset = vma->vm_pgoff<<PAGE_SHIFT;
#ifdef LINUX24
  int minor = PMACD_MINOR(file->f_dentry->d_inode->i_rdev);
#else
  unsigned int minor = iminor(file->f_dentry->d_inode);
#endif
  unsigned long size = vma->vm_end - vma->vm_start;
  pmacd_stream_t *stream;
  unsigned long streamSize;
  unsigned long BaseAddress;
  pmacd_board_t *pBoard ;

  // MMAP only supported on streams device node
  if(minor != PMACD_STREAMS_MINOR){
    pmacd_printDebug("Invalid device node\n");
    return -ENOSYS;
  }

  // Verify that stream has been attached to the file handle.
  stream = (pmacd_stream_t *)file->private_data;
  if(stream == NULL) {
    pmacd_printDebug("pmacd_mmap: device is null\n");
    return -ENOTTY;
  }

  pBoard = pmacd_driver.boardmap[stream->boardId];
  if(pBoard == NULL) { 
    pmacd_printDebug("pmacd_mmap: device is null\n");
    return -EINVAL;
  }
 
  if (offset & ~PAGE_MASK){
    pmacd_printDebug("mmap offset not aligned\n");
    return -EINVAL;
  }
  
  streamSize = stream->numberOfBuffers * stream->bufferSize;
  if(size != streamSize){
    pmacd_printDebug("pmacd_mmap: mmap size not same size as stream buffer.\n");
    return -EINVAL;
  }
  
  offset = (unsigned long)stream->buffers;
  
  /* lock vm pages in memory (don't swap) */
  vma->vm_flags |= VM_LOCKED;  
  if ( stream->streamAttributes & PMACD_DOWNLOAD_STREAM ) {
	BaseAddress = pBoard->bus.configBaseAddress ;
  } else {
	BaseAddress = virt_to_phys((void *)((unsigned long)offset)) ;
  }
  if(remap_page_range(vma, vma->vm_start,
    BaseAddress, size, vma->vm_page_prot)){
    pmacd_printDebug("pmacd_mmap: mmap mapping request failed: 0x%lx\n", vma->vm_start);
    return -ENXIO;
  }

  stream->mmapAddress = (void *)vma->vm_start;

  return(0);
}


/*
 *  FunctionName:  pmacd_ioctl()
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
 *   Description:  Kernel driver IOCTL entry point.  Allows for extension functions
 *                 to be added to the driver.
 *
 *      Comments:  Each IOCTL has its own set of error values.  Each should be
 *                 documented in the library.
 */  
int pmacd_ioctl(struct inode *inode, struct file *file, 
		unsigned int functionId, unsigned long arg){
#ifdef LINUX24
  int minor = PMACD_MINOR(inode->i_rdev);
#else
  unsigned int minor = iminor(inode);
#endif

  switch (minor){
  case PMACD_CONTROL_MINOR:{
    
    switch(functionId){
      
    case IOCTL_PMACD_REPORT_BOARDS:
      return(pmacd_reportBoards((pmacd_reportBoards_t *)arg));
      
    case IOCTL_PMACD_GET_PARM:
      return(pmacd_getDriverParm((pmacd_ioctlParm_t *)arg));

    case IOCTL_PMACD_SET_PARM:
      return(pmacd_setDriverParm((pmacd_ioctlParm_t *)arg));

    case IOCTL_PMACD_GET_BOARD_PARM:
      return(pmacd_getBoardParm((pmacd_ioctlBoardParm_t *)arg));

    case IOCTL_PMACD_SET_BOARD_PARM:
      return(pmacd_setBoardParm((pmacd_ioctlBoardParm_t *)arg)); 
      
    case IOCTL_PMACD_RSAEBIMBSCM:{
      pmacd_board_t *pBoard = pmacd_driver.boardmap[(pmacd_boardId_t)arg];
      if(pBoard == NULL) {
	pmacd_printDebug("pmacd_ioctl: IOCTL_PMACD_RSAEBIMBSCM:Invalid Board Structure\n");
        return -EINVAL;
      }
      return(pmacd_ResetSystemAndEnterBoot(pBoard));
    }
    case IOCTL_PMACD_INIT_BOARD:{
      pmacd_board_t *pBoard = pmacd_driver.boardmap[(pmacd_boardId_t)arg];
      if(pBoard == NULL) {
	pmacd_printDebug("pmacd_ioctl: IOCTL_PMACD_INIT_BOARD:Invalid Board Structure\n");
        return -EINVAL;
      }
      return(pmacd_initBoard(pBoard));
    }
    
    case IOCTL_PMACD_CONFIGURE_BOARD:{
      pmacd_board_t *pBoard = pmacd_driver.boardmap[(pmacd_boardId_t)arg];
      if(pBoard == NULL) {
	pmacd_printDebug("pmacd_ioctl: IOCTL_PMACD_CONFIGURE_BOARD:Invalid Board Structure\n");
        return -EINVAL;
      }
      // Set msg timer
      // (Done here to avoid including pmacd_driver in pmacd_board.c
      //  which would cause a circular dependency)
         pBoard->sendMessagesTimer.expires = PMACD_GET_CURRENT_TICK() + 
	        (pmacd_driver.parms.msgUnitConfig.inboundTimer*HZ)/1000; 
      return(pmacd_configureBoard(pBoard, &(pmacd_driver.parms.msgUnitConfig)));
    }

    case IOCTL_PMACD_HARD_RESET_BOARD:{
      pmacd_board_t *pBoard = pmacd_driver.boardmap[(pmacd_boardId_t)arg];
      if(pBoard == NULL) {
	pmacd_printDebug("pmacd_ioctl: IOCTL_PMACD_HARD_RESET_BOARD:Invalid Board Structure\n");
        return -EINVAL;
      }
      return(pmacd_hardResetBoard(pBoard,TRUE));
    }
    
    case IOCTL_PMACD_RESET_BOARD:{
      pmacd_board_t *pBoard = pmacd_driver.boardmap[(pmacd_boardId_t)arg];
      if(pBoard == NULL) {
	pmacd_printDebug("pmacd_ioctl: IOCTL_PMACD_RESET_BOARD:Invalid Board Structure\n");
        return -EINVAL;
      }
      return(pmacd_resetBoard(pBoard));
    }
    
    case IOCTL_PMACD_START_BOARD:{
      pmacd_board_t *pBoard = pmacd_driver.boardmap[(pmacd_boardId_t)arg];
      if(pBoard == NULL) {
	pmacd_printDebug("pmacd_ioctl: IOCTL_PMACD_START_BOARD:Invalid Board Structure\n");
        return -EINVAL;
      }
      if(pmacd_getControlState(&(pBoard->control)) 
	 !=  pmacd_BoardState_Configured) return -ENOTTY;
      return(pmacd_startBoard(pBoard));
    }
    
    case IOCTL_PMACD_STOP_BOARD:{
      pmacd_board_t *pBoard = pmacd_driver.boardmap[(pmacd_boardId_t)arg];
      if(pBoard == NULL) {
	pmacd_printDebug("pmacd_ioctl: IOCTL_PMACD_STOP_BOARD:Invalid Board Structure\n");
        return -EINVAL;
      }
      return(pmacd_stopBoard(pBoard));
    }
    
    case IOCTL_PMACD_QUIESCE_BOARD:{
      pmacd_board_t *pBoard = 
	pmacd_driver.boardmap[(pmacd_boardId_t)arg];
      if(pBoard == NULL) {
	pmacd_printDebug("pmacd_ioctl: IOCTL_PMACD_QUIESCE_BOARD:Invalid Board Structure\n");
        return -EINVAL;
      }
      return(pmacd_quiesceBoard(pBoard));
    }

    case IOCTL_PMACD_GET_DIAG_COUNTS:
    case IOCTL_PMACD_RESET_DIAG_COUNTS:  
      return -ENOSYS;
      
    default:
      return -EINVAL;      
    }
  }
  
  case PMACD_STREAMS_MINOR:{
    switch(functionId){

    case IOCTL_PMACD_ATTACH_STREAM:{
      pmacd_attachStream_t attach;
      pmacd_board_t *board;
      pmacd_stream_t *stream;
      int retValue;
      
      // Check that no stream is attached to this handle.
      stream = (pmacd_stream_t *)file->private_data;
      if(stream != NULL){
	pmacd_printDebug("stream in use .\n");
	return -EBUSY;
      }

      // Copy parameter structure from user space.
      pmacd_memcpy_UtoK(&retValue, 
			&attach, 
			(void *)arg, 
			sizeof(pmacd_attachStream_t));  
      if(retValue){
	pmacd_printDebug("copy error.\n");
	return -EFAULT;
      }

      // Check that the boardId is valid, and 
      // points to a valid board.
      if((attach.boardId > PMACD_MAX_BOARDID)||
	 (attach.contextId < 1)){
	pmacd_printDebug("contextId invalid.\n");
	return -EINVAL;
      }

      board = pmacd_driver.boardmap[attach.boardId];
      if(board == NULL) {
	pmacd_printDebug("board NULL.\n");
	return -EINVAL;
      } 

      // Attach the stream with the given parameters.
      retValue = pmacd_attachStream(attach.boardId,
				    attach.contextId,
				    attach.termId,
				    attach.streamAttributes,
				    &(board->streams),
				    &stream);
      if(retValue)return(retValue);

      // Copy output parameters to user space.
      attach.bufferSize =      stream->bufferSize;
      attach.numberOfBuffers = stream->numberOfBuffers;
      
      pmacd_memcpy_KtoU(&retValue, 
			(void *)arg,
			&attach,
			sizeof(pmacd_attachStream_t));
      if(retValue){
	pmacd_detachStream(stream);
	return -EFAULT;
      }
      
      // Store stream handle in private data
      file->private_data = (void *)stream;
      return(0);
    }
    
    case IOCTL_PMACD_DETACH_STREAM:{
      pmacd_detachStream_t detach;
      int retValue;
      pmacd_stream_t *stream = (pmacd_stream_t *)file->private_data;

      if(stream == NULL)return -ENOTTY;
      
      // Copy parameter structure from user space
      pmacd_memcpy_UtoK(&retValue, 
			&detach, 
			(void *)arg, 
			sizeof(pmacd_detachStream_t));  
      if(retValue)return -EFAULT;
      
      detach.bufferSize      = stream->bufferSize;
      detach.numberOfBuffers = stream->numberOfBuffers;
      detach.startAddress    = stream->mmapAddress;

      // Copy output parameters to user space.
      pmacd_memcpy_KtoU(&retValue, 
			(void *)arg,
			&detach,   
			sizeof(pmacd_detachStream_t));
      if(retValue)return -EFAULT;
      
      return(pmacd_detachStream(stream));
    }
    
    case IOCTL_PMACD_START_STREAM:{
      pmacd_stream_t *stream = (pmacd_stream_t *)file->private_data;
      if(stream == NULL)return -ENOTTY;
      return(pmacd_startStream(stream));
    }

    case IOCTL_PMACD_GET_PARM:
    case IOCTL_PMACD_SET_PARM:
    case IOCTL_PMACD_GET_DIAG_COUNTS:
    case IOCTL_PMACD_RESET_DIAG_COUNTS:
      return -ENOSYS;
      
    default:
	pmacd_printDebug("Invalid IOCTL 0x%x\n", functionId);
      return -EINVAL;
    }
  }
  
  case PMACD_QUEUES_MINOR:{
    switch(functionId){
    case IOCTL_PMACD_GET_DIAG_COUNTS:
    case IOCTL_PMACD_RESET_DIAG_COUNTS:
      return -ENOSYS;
      
    default:
      return -EINVAL;
    }
  }
  
  default:
    return -ENODEV;
  }
  
  return(0);
}
/*
 *  FunctionName:  pmacd_register_irq()
 *
 *        Inputs:  board - pointer to the board structure
 *                         to register the irq with.
 *
 *       Outputs:  none.
 *  
 *       Returns:  none.
 *
 *   Description:  Register the irq handler for the board with the OS.
 *
 *      Comments: This function is included in pmacd_linux_entry.c
 *                because driver name is only exists in the scope of
 *                this file.
 */  
int pmacd_register_irq(pmacd_board_t *board){
  
  int status = 0;

  if (request_irq(board->bus.irq,
		     &pmacd_handleInterrupt, 
		     SA_SHIRQ,
		     pmacd_string,
		     board)) {
	pmacd_printDebug("pmacd_register_irq(): Requested IRQ %d is busy\n", board->bus.irq);
	status = -EAGAIN;
   }
   return status;
}
/*
 *  FunctionName:  pmacd_queueUserMessage()
 *
 *        Inputs:  appQueue - pointer to the application queue of the application sending
 *                            the message. (needed for source address)
 *                 buffer   - pointer to user buffer containing the message to be sent to the
 *                            board. (User PMB contains board address also).
 *                 size     - size of message to be copied to the board.
 *
 *       Outputs:  none.
 *  
 *       Returns:  ssize_t >= 0 number of bytes copied to the queue. (should be size)
 *                 ssize_t <  0 error condition
 *                 -EINVAL = boardNumber out of range or invalid (No board at that Id)
 *                 -ENOTTY = board not in the Running State. (Messages can only be sent when
 *                           the board is in the Running State.)
 *                 -EFAULT = Memory access error when trying to copy buffer to board queue.
 *                 -ENOSYS = System call not supported for that device.  Queues are the only
 *                           ones that support read.
 *                 -ENODEV = Invalid Minor numbered device. Not supported.
 *
 *   Description:  Copies the user message buffer to the inbound board queue of the
 *                 the board embedded in the user buffer queue.
 *
 *      Comments:  
 */  
ssize_t pmacd_queueUserMessage(pmacd_pmbqueue_t *appQueue,
			       const char *buffer,
			       size_t size){
  pmacd_board_t *pBoard;
  pmacd_msgheader_t *pmbheader;
  pmacd_boardnumber_t boardNumber;
  pmacd_mfpool_t *mfPool;
  pmacd_mf_t *mf;
  pmacd_pmbqueue_t *overflowQueue;
  int retValue;
  pmacd_boardstate_t state;
  struct timeval tv;
 
  pmacd_getBoardNumberFromUPMB(&retValue, buffer,&boardNumber);
  if(retValue)return -EFAULT;
  
  if(boardNumber > PMACD_MAX_BOARDID){
    return -EINVAL;
  }
  
  pBoard = pmacd_driver.boardmap[boardNumber];
  if(pBoard == NULL)return -EINVAL;
 
  state = pmacd_getControlState(&(pBoard->control));
   if (( state != pmacd_BoardState_Running) && 
       ( state != pmacd_BoardState_Configured) )
       return -ENOTTY;
  
  mfPool = pBoard->inboundMFs;
  overflowQueue = pBoard->overflowQueue;
  
  // Move any overflow queue messages to the mfs first.
  if(!pmacd_isPMBQueueEmpty(pBoard->overflowQueue)){
    MUTEX_ENTER_BH(pBoard->inboundMfaQueue.mutex);
    pmacd_moveOverflowMsgsToMfs(mfPool, 
				&(pBoard->inboundMfaQueue),
				pBoard->overflowQueue);
    MUTEX_EXIT_BH(pBoard->inboundMfaQueue.mutex);
  }

  // If all of the overflow queue messages were moved over
  // try and copy the new message into a MF.
  if(pmacd_isPMBQueueEmpty(pBoard->overflowQueue) &&
     ((mf = pmacd_getMFFromPool(mfPool)) != (pmacd_mf_t *)NULL)){
    // got an MFcopy directly to MF.
    pmacd_copyUPMBToMF(&retValue, buffer, mf, 
		       pmacd_driver.parms.msgUnitConfig.msgSize);
    if(retValue)return -EFAULT;
    
    // Insert application queue address into message
    pmbheader = (pmacd_msgheader_t *)mf;
    pmbheader->srcNode = PMACD_HOSTBOARD_FLAG|(appQueue->queueAddress);

    if (  pmbheader->srcNode & 0x40 ) {
       do_gettimeofday(&tv);
       pmacd_printDebug("pmacd_queueUserMessage: srcNode=0x%x, %9d:%06d\n",  (pmbheader->srcNode & ~PMACD_HOSTBOARD_FLAG) - 1, tv.tv_sec, tv.tv_usec);
    }

    MUTEX_ENTER_BH(pBoard->inboundMfaQueue.mutex);
    pmacd_addToMfaQueue(&(pBoard->inboundMfaQueue), (pmacd_mfa_t)mf);
    MUTEX_EXIT_BH(pBoard->inboundMfaQueue.mutex);
    // If we are in ASAP mode we need to check if we have reached
    // the threshold.

    if(pmacd_driver.parms.msgUnitConfig.inboundTransferMode ==
       pmacd_MsgTransferMode_ASAP){	
      if(pmacd_driver.parms.msgUnitConfig.inboundThreshold == 
	 ATOMIC_GET(pBoard->inboundMfaQueue.count)){
	del_timer(&(pBoard->sendMessagesTimer));
	pmacd_sendMessagesToBoard((ULONG_T)pBoard);
      }
    }
  }else{
    // No more MFs copy to overflow queue.
    pmbheader = (pmacd_msgheader_t *)pmacd_getPMBTailFromPMBQueue(overflowQueue);
    if(!pmacd_copyUPMBToPMBQueue(buffer, overflowQueue)){
      return -EFAULT;
    }
    pmbheader->srcNode = PMACD_HOSTBOARD_FLAG|(appQueue->queueAddress);
  }    
  return(size);
}









