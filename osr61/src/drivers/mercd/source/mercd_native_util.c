/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : mercd_native_util.c
 * Description                  : native functions
 *                                
 *
 ***********************************************************************/

#include "msd.h"
#define _MSDNATIVEALLOC_
#include "msdextern.h"
#undef _MSDNATIVEALLOC_

#ifdef LFS
#define msgb_t mblk_t
#define datab_t dblk_t
#define NATIVE_MAX_POOL_SIZE     3000
#define NATIVE_SEQ_POOL_SIZE     100 
#define NATIVE_MAX_BANK_SIZE     1000
#endif

#include <asm/atomic.h>

#define  QACQUIRE_MSG_POOL_KEY() qDrvAcquirePoolKey()
#define  QRELEASE_MSG_POOL_KEY() clear_bit(0, &qMsgPoolBusyFlag)
#ifdef LFS
#define  QZERO_MSG_POOL(mp) {                   \
   mp->b_next = NULL;                           \
   mp->b_prev = (msgb_t *)0xDEADDEAD;           \
   mp->b_cont = NULL;                           \
   mp->b_rptr = NULL;                           \
   mp->b_wptr = NULL;                           \
   if (mp->b_datap) {                           \
       mp->b_datap->db_base = NULL;             \
       mp->b_datap->db_lim = NULL;              \
       mp->b_datap->db_ref = 0;                 \
       mp->b_datap->db_type = 0;                \
       mp->b_datap->db_size = 0;                \
   }                                            \
}
#else
#define  QZERO_MSG_POOL(mp) {                   \
   mp->b_next = NULL;                           \
   mp->b_prev = 0xDEADDEAD;                     \
   mp->b_cont = NULL;                           \
   mp->b_rptr = NULL;                           \
   mp->b_wptr = NULL;                           \
   if (mp->b_datap) {                           \
       mp->b_datap->db_base = NULL;             \
       mp->b_datap->db_lim = NULL;              \
       mp->b_datap->db_ref = 0;                 \
       mp->b_datap->db_type = 0;                \
       mp->b_datap->db_size = 0;                \
   }                                            \
}
#endif

int      qMsgPoolCount = 0;
int      qMsgPoolBusyFlag = 0;     // Non SMP machine busy flag indicator
int      qMsgPoolCurrCount = 0;
int      qMsgPoolBankCount = 0;
msgb_t  *qMsgPoolBank[NATIVE_MAX_BANK_SIZE];
#ifdef LFS
datab_t  *qDataPoolBank[NATIVE_MAX_BANK_SIZE];
#else
msgb_t  *qDataPoolBank[NATIVE_MAX_BANK_SIZE];
#endif
msgb_t  *qMsgPoolHead = NULL;
msgb_t  *qMsgPoolTail = NULL;
msgb_t  *qMsgPoolStart = NULL;
datab_t *qDataPoolStart = NULL;
mercd_mutex_T qMsgPoolMutex;

void *native_malloc(int nbytes, int class, char *file_name, int line_nr)
{
   return(KALLOC(nbytes, class)) ;
}

void native_free(void *ptr, char *file_name, int line_nr)
{
   printk("anybody using me ..... %s %d \n",file_name, line_nr);
   KFREE(ptr,0) ;
}

void native_udelay(long micro_secs)
{
   udelay(micro_secs) ;
}


// Memory Manager Implementation
#ifdef LFS
void qDrvAcquirePoolKey(void);
#endif
void qDrvAcquirePoolKey()
{
   while (test_and_set_bit(0, &qMsgPoolBusyFlag))
         udelay(1);
}

#ifdef LFS
void qMsgPoolCheck(void);
#endif
void qMsgPoolCheck()
{
   msgb_t *mp = NULL;
   PMDRV_MSG MdMsg = NULL;
   PSTRM_MSG Msg;
   int pool_count, i;

   if (!qMsgPoolCount) {
       return;
   }

   printk("Performing Integrity check \n");
   printk("Msg Pool: Curr %d Initial %d \n", qMsgPoolCurrCount, qMsgPoolCount);

   mp = qMsgPoolHead;
   pool_count = 0;
   for (i = 0; (mp != NULL ); i++) {
        mp = mp->b_next;
        pool_count++;
        if (pool_count > qMsgPoolCount)  
            break;
   }

   if (pool_count > qMsgPoolCount) {
       printk("PoolCheck: Integrity of the queue is compramised found %d was %d \n", 
                                                         pool_count, qMsgPoolCount);
       return;
   }
   printk("PoolCheck: Integrity check found %d was %d \n", pool_count, qMsgPoolCount);

   mp = qMsgPoolStart;
   for (i = 0; i < qMsgPoolCount ; i++) {
#ifdef LFS
        if (mp[i].b_prev == (msgb_t *)(long)0xc0ffee ) {
#else
        if (mp[i].b_prev == 0xc0ffee ) {
#endif
            Msg = (PSTRM_MSG) &mp[i];
            MdMsg = (PMDRV_MSG)MD_EXTRACT_MDMSG_FROM_STRMMSG(Msg);
            printk("Allocated message is 0x%x \n", MD_GET_MDMSG_ID(MdMsg));
        }
   }
}

int qDrvMsgPoolAllocate(int count)
{
   msgb_t *mp;
   datab_t *datap;
   int i, need_key =0;


   if (!qMsgPoolCount) {
       // Allocate a Mutex to protect in a SMP situation 
       need_key = 1;
       MSD_INIT_MUTEX(&qMsgPoolMutex, NULL, NULL );

       MSD_ENTER_MUTEX(&qMsgPoolMutex);

       QACQUIRE_MSG_POOL_KEY();

   } else {
      if (qMsgPoolBankCount == NATIVE_MAX_BANK_SIZE) {
          printk("Msg Pool Allocation failed - reached max pool bank size\n");
          return (-1);
      }
   }

   mercd_zalloc(qMsgPoolStart, msgb_t *, (count * sizeof(msgb_t)) );

   if (qMsgPoolStart == NULL) {
       printk("Msg Pool Allocation failed for %d count\n", count);
       if (need_key) {
           QRELEASE_MSG_POOL_KEY();
           MSD_EXIT_MUTEX(&qMsgPoolMutex);
       }
       return(-1);
   }

   mercd_zalloc(qDataPoolStart, datab_t *,(count * sizeof(datab_t)) );

   if (qDataPoolStart == NULL) {
       printk("Msg Pool Allocation failed for %d count\n", count);
       MSD_FREE_KERNEL_MEMORY(qMsgPoolStart,(count * sizeof(msgb_t)) );
       if (need_key) {
           QRELEASE_MSG_POOL_KEY();
           MSD_EXIT_MUTEX(&qMsgPoolMutex);
       }
       return(-1);
   }

   qMsgPoolBank[qMsgPoolBankCount] = qMsgPoolStart;
   qDataPoolBank[qMsgPoolBankCount] = qDataPoolStart;

   qMsgPoolBankCount++;

   // Connect the Message and Data Pools
   mp = qMsgPoolStart;
   datap = qDataPoolStart;
   for (i = 0; i < count ; i++) {
        mp->b_datap = datap;
        QZERO_MSG_POOL(mp);
        mp++; 
        datap++;
   }
    
   // Link the messages
   mp = qMsgPoolStart;
   for (i = 0; i < (count - 1); i++) {
        mp[i].b_next = &mp[i+1];
   }
   mp[count - 1].b_next = NULL;
   qMsgPoolTail = &mp[count -1];
   qMsgPoolHead = qMsgPoolStart;

   if (qMsgPoolBankCount == 1) {
       qMsgPoolCount = qMsgPoolCurrCount = count;
   } else {
       qMsgPoolCount += count;
       qMsgPoolCurrCount = count;
   }

   if (need_key) {
       QRELEASE_MSG_POOL_KEY();
       MSD_EXIT_MUTEX(&qMsgPoolMutex);
   }

   return(0);
}

int qDrvMsgPoolFree()
{
   int i;

   // Enter the mutex
   MSD_ENTER_MUTEX(&qMsgPoolMutex);

   // Test Set the conditional variable
   QACQUIRE_MSG_POOL_KEY();

   qMsgPoolCheck();

   qMsgPoolStart = qMsgPoolBank[0];
   qDataPoolStart = qDataPoolBank[0];
   MSD_FREE_KERNEL_MEMORY(qDataPoolStart,(NATIVE_MAX_POOL_SIZE * sizeof(datab_t)) );
   MSD_FREE_KERNEL_MEMORY(qMsgPoolStart,(NATIVE_MAX_POOL_SIZE * sizeof(msgb_t)) );

   for (i = 1; i < qMsgPoolBankCount; i++) {
        qMsgPoolStart = qMsgPoolBank[i];
        qDataPoolStart = qDataPoolBank[i];
        MSD_FREE_KERNEL_MEMORY(qDataPoolStart,
                 (NATIVE_SEQ_POOL_SIZE * sizeof(datab_t)));
        MSD_FREE_KERNEL_MEMORY(qMsgPoolStart,
                 (NATIVE_SEQ_POOL_SIZE * sizeof(msgb_t)) );
   }

   // Reset the variables 
   qMsgPoolStart = NULL;
   qMsgPoolTail = NULL;
   qMsgPoolCount = 0;
   qMsgPoolCurrCount = 0;
   qMsgPoolBankCount = 0;

   QRELEASE_MSG_POOL_KEY();

   MSD_EXIT_MUTEX(&qMsgPoolMutex);

   // Destroy the mutex
   MSD_DESTROY_MUTEX(&qMsgPoolMutex);

   return(0);
}

void qDrvPrintMsgPoolStatus()
{
   printk("Msg Pool Status: Curr Avail %d Total Alloc %d \n", qMsgPoolCurrCount, qMsgPoolCount);

}


#ifdef LFS
extern void mid_print_adapter_info(void);
#else
extern void mid_print_adapter_info();
#endif

msgb_t *qMsgPoolAllocate()
{
   msgb_t *mp = NULL;

   MSD_ENTER_MUTEX(&qMsgPoolMutex);

   QACQUIRE_MSG_POOL_KEY();

   if (!qMsgPoolCurrCount || (qMsgPoolHead == NULL)) {
       //printk("PoolAlloc: NO pool available Count %d H 0x%x T 0x%x \n",
       //                 qMsgPoolCurrCount, qMsgPoolHead, qMsgPoolTail);
       if (qDrvMsgPoolAllocate(NATIVE_SEQ_POOL_SIZE) < 0) {
           printk("PoolAlloc: No pool available Count %d H 0x%p T 0x%p \n",
                            qMsgPoolCurrCount, qMsgPoolHead, qMsgPoolTail);
           QRELEASE_MSG_POOL_KEY();
           MSD_EXIT_MUTEX(&qMsgPoolMutex);
           return(NULL);
       }
       //printk("Added 100 more pools curr total %d \n", qMsgPoolCount);
       //mid_print_adapter_info();
    }

    //printk("PoolAlloc: 0x%x Avail %d \n", qMsgPoolHead, qMsgPoolCurrCount);
    mp = qMsgPoolHead;
#ifdef LFS
    if (mp->b_prev != (msgb_t *)0xDEADDEAD ) {
#else
    if (mp->b_prev != 0xDEADDEAD ) {
#endif
#ifdef LFS
        printk("PoolAlloc: Double allocation, Queue compramised found b_prev 0x%p \n", mp->b_prev);
#else
        printk("PoolAlloc: Double allocation, Queue compramised found b_prev 0x%x \n", mp->b_prev);
#endif
        QRELEASE_MSG_POOL_KEY();
        MSD_EXIT_MUTEX(&qMsgPoolMutex);
        return(NULL);
    }

    qMsgPoolHead = qMsgPoolHead->b_next;

    if (qMsgPoolHead == NULL) 
        qMsgPoolTail = NULL;

    mp->b_next = NULL;
    QZERO_MSG_POOL(mp);
#ifdef LFS
    mp->b_prev = (msgb_t *)(long)0xc0ffee;
#else
    mp->b_prev = 0xc0ffee;
#endif

    qMsgPoolCurrCount--;
    QRELEASE_MSG_POOL_KEY();
    MSD_EXIT_MUTEX(&qMsgPoolMutex);

    return(mp);
}

int qMsgPoolFree(msgb_t *mp1)
{
   msgb_t *mp = NULL;

   MSD_ENTER_MUTEX(&qMsgPoolMutex);
   QACQUIRE_MSG_POOL_KEY();

   if (!qMsgPoolCount) {
       QRELEASE_MSG_POOL_KEY();
       MSD_EXIT_MUTEX(&qMsgPoolMutex);
       return(-1);
   }

#ifdef LFS
   if (mp1->b_prev != (msgb_t *)(long)0xc0ffee) {
#else
   if (mp1->b_prev != 0xc0ffee) {
#endif
#ifdef LFS
       printk("PoolFree: Double Free encountered b_prev = 0x%p addr 0x%p \n",
                 mp1->b_prev , mp1);
#else
       printk("PoolFree: Double Free encountered b_prev = 0x%x addr 0x%x \n",
                 mp1->b_prev , mp1);
#endif
       QRELEASE_MSG_POOL_KEY();
       MSD_EXIT_MUTEX(&qMsgPoolMutex);
       return(-1);
   }

   mp = qMsgPoolTail;
   if (mp != NULL) {
       mp->b_next = mp1;
   } else {
       qMsgPoolHead = mp1;
   }

   qMsgPoolTail = mp1;
   mp1->b_next = NULL;
   QZERO_MSG_POOL(mp1);
   qMsgPoolCurrCount++;

   //printk("PoolFree: 0x%x Avail %d \n", qMsgPoolHead, qMsgPoolCurrCount);
   QRELEASE_MSG_POOL_KEY();
   MSD_EXIT_MUTEX(&qMsgPoolMutex);

   return(0);
}

void *native_alloc_atomic_asi(int nbytes, char *file, int line)
{
   return(native_malloc(nbytes, GFP_ATOMIC, file, line)) ;
}

void *native_alloc_kernel_asi(int nbytes, char *file, int line)
{
   // driver running in bh level so cannot allocate GFP_KERNEL, then ...
   return(native_malloc(nbytes, GFP_ATOMIC, file, line)) ;
}

void *native_alloc_dma_asi(int nbytes, char *file, int line)
{
   return(native_malloc(nbytes, GFP_DMA|GFP_ATOMIC, file, line)) ;
}

void *native_free_mem_asi(void *mem_area, char *file, int line)
{
   native_free(mem_area, file, line) ;
   return(NULL) ;
}

msgb_t *native_allocb(int size, int type) 
{
   char *buffer=NULL;
   msgb_t *Msg;

   if (size) {
     if(type & GFP_DMA)
       buffer = (char *)native_alloc_dma(size);
     else if (type == GFP_ATOMIC)
       buffer = (char *)native_alloc_atomic(size);
     else {
       buffer = (char *)native_alloc_kernel(size);
     }
     if(!buffer) {
	printk("MERCD: Unable to allocate buffer returning ... %d\n",size);
 	return(NULL);
     }
   }

   Msg = native_alloc_msg_desr(size, buffer);

   if (Msg == NULL) {
       KFREE(buffer,size);
   }
   return (Msg);
}


void native_linkb(msgb_t *mp1, msgb_t *mp2)
{
    msgb_t *tmp_mp1 = mp1;

    // If ever b_cont is non NULL then the caller
    // mp1 is moved to the last msg
    if (tmp_mp1 && mp2) {
        while (tmp_mp1->b_cont)
               tmp_mp1 = tmp_mp1->b_cont;
        tmp_mp1->b_cont = mp2;
    }
}

msgb_t *native_unlinkb(msgb_t *mp)
{ 
   msgb_t *rtn;

   if (mp == NULL) 
       return NULL;

   rtn = mp->b_cont;
   mp->b_cont = NULL;
   return rtn;
						    
}

msgb_t *native_dupb(msgb_t *mp) {
   // No longer used
   if (mp) {
      msgb_t *bp=NULL;
      mercd_zalloc(bp, msgb_t *, sizeof(msgb_t));
       if (bp) {
           if (mp->b_datap) {
 	       bp->b_datap = mp->b_datap;	          
	       mp->b_datap->db_ref++;
           }
	   bp->b_rptr = mp->b_rptr;
	   bp->b_wptr = mp->b_wptr;
	   bp->b_cont = NULL;
       }
       return bp;
   }

   return NULL;

}

int native_freeb(msgb_t *mp)
{
   if( mp == NULL )  {
      printk("freeb called with NULL \n");
      return (-1);
   }

   if (mp->b_datap) {

       if (mp->b_datap->db_ref > 1) {
	   mp->b_datap->db_ref--;
       } else {
   	   if (mp->b_datap->db_base) {
               MSD_FREE_KERNEL_MEMORY(mp->b_datap->db_base,mp->b_datap->db_size); 
	   }
       }
   }

   return (qMsgPoolFree(mp));

}

int native_freemsg(msgb_t *mp)
{
   msgb_t *crmp = mp;
   msgb_t *crmp1;
   while (crmp) {
      crmp1 = crmp->b_cont;
      crmp->b_cont = NULL;
      if (native_freeb(crmp) < 0) {
	  printk("native_freemsg: breaking for incorrect free\n");
          break;
      }
      crmp = crmp1;
   }  
}

msgb_t * native_alloc_msg_desr(int size, char *Buffer) {
	
   msgb_t *Msg=NULL;

   Msg = qMsgPoolAllocate();
   if (Msg) {
     if (Buffer) {
             Msg->b_datap->db_base = (unsigned char *)Buffer; 
             Msg->b_datap->db_lim = (char *)Buffer + size;
             Msg->b_datap->db_size = size; 
             Msg->b_rptr = (unsigned char *) Buffer; 
             Msg->b_wptr = (unsigned char *) Msg->b_rptr + size; 
             Msg->b_datap->db_ref = 1; 
     } else {
	     qMsgPoolFree(Msg);
	     return (NULL);
     }
     Msg->b_cont = NULL;
   } 
   return(Msg);
}

void native_cmn_err(int err_lvl, char *fmt, ...)
{
   va_list args;
   char    buf[1024];
   char    *p ;

   switch (err_lvl) {
      case CE_CONT:               /* continue printing */
         buf[0] = 0 ;
         break ;
      case CE_NOTE:               /* NOTICE */
         strcpy(buf, "\n NOTICE: ") ;
         break ;
      case CE_WARN:               /* WARNING */
         strcpy(buf, "\n WARNING: ") ;
         break ;
      default:
         return ;
   }

   for (p = buf; *p; p++) ;

   va_start (args, fmt);
   vsprintf (p, fmt, args);
   va_end (args);
   printk("%s", buf);
}
