/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdlinuxlocks.h
 * Description                  : locking definitions
 *
 *
 **********************************************************************/

#ifndef _MSDLINUXLOCK_H
#define _MSDLINUXLOCK_H

typedef struct _MSD_MUTEX_BLOCK {
   unsigned long    Flags;
   mercd_mutex_T HwIntrMutex;
   mercd_mutex_T SoftIntrMutex;
   mercd_mutex_T StrmLstIntrMutex;
} MSD_MUTEX_BLOCK, *PMSD_MUTEX_BLOCK;


/*
 * External declarations for the mutex's
 */
#define MAX_MUTEX_ENTRY_TABLE           0xffff

#define MSD_MUTEX_FLAG_SOFT_READY	0x00000001
#define MSD_MUTEX_FLAG_HW_READY		0x00000002
#define MSD_MUTEX_FLAG_STRM_LST_READY	0x00000004
#define MSD_MUTEX_FLAG_DONT_EXIT	0x00000008

/*
 * LiSless defined on the else part
 */

#define printdbg(args...)			//printk

#if defined LiS || defined LIS
#define MSD_INIT_MUTEX(mutex_name, mutexstr, mutexptr) 		\
{	printdbg("INIT %s    (%s) (%d)\n", mutex_name, __FILE__, __LINE__);\
        lis_spin_lock_init(mutex_name, mutexstr);		}

#define MSD_ENTER_CONTROL_BLOCK_MUTEX()    			\
{        lis_spin_lock(&MsdControlBlock->mercd_ctrl_block_mutex);\
	printdbg("ENTER_CONTROL_BLOCK (%s) (%d)\n", __FILE__, __LINE__);}

#define MSD_EXIT_CONTROL_BLOCK_MUTEX()     			\
{	printdbg("EXIT_CONTROL_BLOCK  (%s) (%d)\n", __FILE__, __LINE__);\
        lis_spin_unlock(&MsdControlBlock->mercd_ctrl_block_mutex);}

#define MSD_ENTER_CONFIG_MAP_TABLE_MUTEX() 			\
{        lis_spin_lock(&config_map_table_mutex);		\
	printdbg("ENTER_CONFIG_MAP    (%s) (%d)\n", __FILE__, __LINE__);}

#define MSD_EXIT_CONFIG_MAP_TABLE_MUTEX()  			\
{	printdbg("EXIT_CONFIG_MAP     (%s) (%d)\n", __FILE__, __LINE__);\
        lis_spin_unlock(&config_map_table_mutex);		}

#define MSD_ENTER_OPEN_LIST_MUTEX()        			\
{        lis_spin_lock(&mercd_open_list_mutex) ;		\
	printdbg("ENTER_OPEN_LIST     (%s) (%d)\n", __FILE__, __LINE__);}

#define MSD_EXIT_OPEN_LIST_MUTEX() 	   			\
{	printdbg("EXIT_OPEN_LIST      (%s) (%d)\n", __FILE__, __LINE__);\
        lis_spin_unlock(&mercd_open_list_mutex);		}

#define MSD_ENTER_SOFT_MUTEX(softmutex)    			\
{	lis_spin_lock(softmutex);				\
	printdbg("ENTER_SOFT   (%s) (%d)\n", __FILE__, __LINE__);}

#define MSD_EXIT_SOFT_MUTEX(softmutex)     			\
{	printdbg("EXIT_SOFT    (%s) (%d)\n");			\
	lis_spin_unlock(softmutex);				}

#define MSD_ENTER_MUTEX(ourmutex1) 	   			\
{        lis_spin_lock(ourmutex1);				\
	printdbg("ENTER_MUTEX  (%s) (%d)\n", __FILE__, __LINE__);}

#define MSD_EXIT_MUTEX(ourmutex2)  	   			\
{	printdbg("EXIT_MUTEX   (%s) (%d)\n", __FILE__, __LINE__);\
        lis_spin_unlock(ourmutex2);				}

#define MSD_ENTER_HW_MUTEX(ConfigId, flags) \
    lis_spin_locksave(&MsdControlBlock->padapter_block_list[mercd_adapter_map[ConfigId]]->phw_info->intr_info->intr_mutex, flags);

#define MSD_EXIT_HW_MUTEX(ConfigId, flags)  \
    lis_spin_unlockrestore(&MsdControlBlock->padapter_block_list[mercd_adapter_map[ConfigId]]->phw_info->intr_info->intr_mutex, flags);

#else /* LiS less */

#define MSD_INIT_MUTEX(mutex_name, mutexstr, mutexptr)  \
        spin_lock_init(mutex_name);

#define MSD_ENTER_CONTROL_BLOCK_MUTEX() \
{ printdbg("ENTERING_CONTROL_BLOCK (%s) (%d) cpu: %d\n", __FILE__, __LINE__,\
          smp_processor_id()); \
  spin_lock_irq(&MsdControlBlock->mercd_ctrl_block_mutex); \
  printdbg("ENTER_CONTROL_BLOCK (%s) (%d) cpu: %d\n", __FILE__, __LINE__,\
             smp_processor_id());}

#define MSD_EXIT_CONTROL_BLOCK_MUTEX() \
{ printdbg("EXIT_CONTROL_BLOCK (%s) (%d) cpu:(%d)\n", __FILE__, __LINE__, smp_processor_id()); \
  spin_unlock_irq(&MsdControlBlock->mercd_ctrl_block_mutex); }

#define MSD_ENTER_CONTROL_BLOCK_MUTEX_BH() \
{ printdbg("ENTERING_CONTROL_BLOCK_BH (%s) (%d) cpu: %d\n", __FILE__, __LINE__,\
           smp_processor_id()); \
  spin_lock_bh(&MsdControlBlock->mercd_ctrl_block_mutex); \
  printdbg("ENTER_CONTROL_BLOCK (%s) (%d) cpu: %d\n", __FILE__, __LINE__,\
           smp_processor_id()); }

#define MSD_EXIT_CONTROL_BLOCK_MUTEX_BH() \
{ printdbg("EXIT_CONTROL_BLOCK (%s) (%d) cpu:(%d)\n", __FILE__, __LINE__, smp_processor_id()); \
  spin_unlock_bh(&MsdControlBlock->mercd_ctrl_block_mutex); }

#define MSD_SPIN_TRYLOCK(spinlock)   spin_trylock(spinlock)

#define MSD_ENTER_CONFIG_MAP_TABLE_MUTEX() \
{ printdbg("ENTERING_CONFIG_MAP (%s) (%d) cpu: %d\n", __FILE__, __LINE__,\
           smp_processor_id()); \
  spin_lock(&config_map_table_mutex);\
  printdbg("ENTER_CONFIG_MAP (%s) (%d) cpu: %d\n", __FILE__, __LINE__,\
           smp_processor_id()); }

#define MSD_EXIT_CONFIG_MAP_TABLE_MUTEX() \
{ spin_unlock(&config_map_table_mutex); \
  printdbg("EXIT_CONFIG_MAP (%s) (%d)\n", __FILE__, __LINE__); }

#define MSD_ENTER_OPEN_LIST_MUTEX() \
{ printdbg("ENTERING_OPEN_LIST (%s) (%d) cpu: %d\n", __FILE__, __LINE__,\
            smp_processor_id()); \
  spin_lock(&mercd_open_list_mutex); \
  printdbg("ENTER_OPEN_LIST (%s) (%d) cpu: %d\n", __FILE__, __LINE__,\
             smp_processor_id()); }

#define MSD_EXIT_OPEN_LIST_MUTEX() \
{ spin_unlock(&mercd_open_list_mutex); \
  printdbg("EXIT_OPEN_LIST (%s) (%d)\n", __FILE__, __LINE__); }

#define MSD_ENTER_MUTEX(ourmutex1) \
{ printdbg("ENTERING_MUTEX(%s) (%d) cpu: %d\n", __FILE__, __LINE__,\
             smp_processor_id()); \
  spin_lock(ourmutex1); \
  printdbg("ENTER_MUTEX(%s) (%d) cpu: %d\n", __FILE__, __LINE__,\
             smp_processor_id()); }

#define MSD_EXIT_MUTEX(ourmutex2)  \
{ printdbg("EXIT_MUTEX(%s) (%d) cpu: %d\n", __FILE__, __LINE__, smp_processor_id()); \
  spin_unlock(ourmutex2); }

#define MSD_ENTER_MUTEX_IRQ(ourmutex1) \
{ printdbg("ENTERING_MUTEX(%s) (%d) cpu: %d\n", __FILE__, __LINE__,\
             smp_processor_id()); \
  spin_lock_irq(ourmutex1); \
  printdbg("ENTER_MUTEX(%s) (%d) cpu: %d\n", __FILE__, __LINE__,\
             smp_processor_id()); }

#define MSD_EXIT_MUTEX_IRQ(ourmutex2)  \
{ printdbg("EXIT_MUTEX(%s) (%d)\n", __FILE__, __LINE__); \
  spin_unlock_irq(ourmutex2); }

#define MSD_ENTER_HW_MUTEX(ConfigId, flags) \
{ spin_lock_irqsave(&MsdControlBlock->padapter_block_list[mercd_adapter_map[ConfigId]]->phw_info->intr_info->intr_mutex, flags); \
  printdbg("ENTER_HW_MUTEX (%s) (%d)\n", __FILE__, __LINE__); }

#define MSD_EXIT_HW_MUTEX(ConfigId, flags)  \
{ spin_unlock_irqrestore(&MsdControlBlock->padapter_block_list[mercd_adapter_map[ConfigId]]->phw_info->intr_info->intr_mutex, flags); \
  printdbg("EXIT_HW_MUTEX (%s) (%d)\n", __FILE__, __LINE__); }

#endif /* LiS */

    

#define MSD_DESTROY_MUTEX(x) 
#define MSD_INIT_CV(x)	
#define MSD_SIGNAL_CV(x)	
#define MSD_DESTROY_CV(x) 
/* 

Wed May 23 14:18:36 EDT 2001

Louai Azzarouk: 
I have changed to "-1" so that I can make the "streamblock->state" 
equals to "MERCD_STREAM_STATE_FREED" in msdbndmgr.c:mid_bndmgr_unbind(). This 
is necessary for SMP/Linux to work correctly (causes a panic).
	
*/

#define MSD_CV_TIMED_WAIT(x,y,z)  -1

#if !defined LiS && !defined LIS && !defined LFS
#define WAITQUEUE_T wait_queue_head_t
#define INIT_WAITQUEUE(waitqueue) (init_waitqueue_head(&(waitqueue)))
#define ALLOCATE_LOCAL_WAITQUEUE DECLARE_WAITQUEUE((__localwait),current)

#define ADD_TO_WAITQUEUE(waitqueue) add_wait_queue(&(waitqueue), &__localwait);\
                                        (current->state) = TASK_INTERRUPTIBLE
#define REMOVE_FROM_WAITQUEUE(waitqueue) remove_wait_queue(&(waitqueue),&__localwait); \
	 current->state = TASK_RUNNING
#define SLEEP_ON_WAITQUEUE(waitqueue) current->state = TASK_INTERRUPTIBLE; schedule()
#define SLEEP_ON_WAITQUEUE_TIMEOUT(waitqueue, timeout)\
         current->state = TASK_INTERRUPTIBLE; schedule_timeout((timeout)*HZ)

#define WAKEUP_WAITQUEUE(waitqueue) wake_up_interruptible(&(waitqueue))
#define DESTROY_WAITQUEUE(waitqueue)
#endif /* LiS */
    
#endif /* _MSDLINUXLOCK_H */
