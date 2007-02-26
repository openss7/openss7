/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: msdqueue.c
 * Description			: Queue functions 
 *
 *
 *
 **********************************************************************/

#include "msd.h"
#define _MSDQUEUE_C_
#include "msdextern.h"
#undef _MSDQUEUE_C_

typedef merc_void_t (*FUNCTION)(merc_uint_t);

/***************************************************************************
 * Function Name		: queue_put_Q
 * Function Type		: Queue Support Function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			:  Streams Driver PutQueue Routine. This
 *				   routine is used to queue an entry
 * Additional comments		:
 ****************************************************************************/
void queue_put_Q(PMSD_QUEUE Queue, PMSD_GEN_MSG MsgPtr)
{
	PMSD_GEN_MSG	Ptr;

	MSD_FUNCTION_TRACE("queue_put_Q", TWO_PARAMETERS, (size_t)Queue, 
							(size_t)MsgPtr);

	// verify queue sanity 
// printk("QueueHead = 0x%x, QTail = 0x%x\n", Queue->QueueHead, Queue->QueueTail);

	 MSD_ASSERT(((Queue->QueueHead == NULL) && (Queue->QueueTail == NULL))
		|| ((Queue->QueueHead != NULL) && (Queue->QueueTail != NULL)));
#if 1 
/* Why this line below ? Dataloop fails...*/
	MSD_ASSERT(MsgPtr->Next == NULL);
#endif

	if(!Queue->QueueTail){	// empty queue 
		Queue->QueueHead = (pmerc_void_t)MsgPtr;
		Ptr = MsgPtr;
		// look for the last message in the chain
		while(Ptr->Next) {	
			Ptr = Ptr->Next;
		}
		Queue->QueueTail = (pmerc_void_t)Ptr;
		return;
	}

	Ptr = (PMSD_GEN_MSG)Queue->QueueTail;

	Ptr->Next = MsgPtr;


	Queue->QueueTail = (pmerc_void_t)MsgPtr;

}

/***************************************************************************
 * Function Name		: queue_put_sec_Q
 * Function Type		: Queue Support Function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver PutSecondaryQuque Routine.
 *				  This routine is used to put an entry on the
 *				  secondary queue
 * Additional comments		:
 ****************************************************************************/
void queue_put_sec_Q(PMSD_QUEUE Queue, PMSD_GEN_TWIN_LINK_MSG MsgPtr)
{
	PMSD_GEN_TWIN_LINK_MSG	Ptr;

	MSD_FUNCTION_TRACE("queue_put_sec_Q", 
		TWO_PARAMETERS, (size_t)Queue, (size_t)MsgPtr);

	// verify queue sanity 
	MSD_ASSERT(((Queue->QueueHead == NULL) && (Queue->QueueTail == NULL))||
		((Queue->QueueHead != NULL) && (Queue->QueueTail != NULL)));
	MSD_ASSERT(MsgPtr->SecondNext == NULL);

	if(!Queue->QueueTail){		// empty queue 
		Queue->QueueHead = (pmerc_void_t)MsgPtr;
		Ptr = MsgPtr;
		while(Ptr->SecondNext)	// look for the last message in chain
			Ptr = Ptr->SecondNext;
		Queue->QueueTail = (pmerc_void_t)Ptr;
		return;
	}

	Ptr = (PMSD_GEN_TWIN_LINK_MSG)Queue->QueueTail;

	Ptr->SecondNext = MsgPtr;

	Queue->QueueTail = (pmerc_void_t) MsgPtr;

}

/***************************************************************************
 * Function Name		: queue_remove_from_Q
 * Function Type		: Queue Support Function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver RemoveFromQueue Routine.
 *				  This routine is used to remove any entry
 *				  from the specified queue.
 * Additional comments		:
 ****************************************************************************/
md_status_t queue_remove_from_Q(PMSD_QUEUE Queue, PMSD_GEN_MSG MsgPtr)
{
	PMSD_GEN_MSG	Ptr;
	PMSD_GEN_MSG	OldPtr;

	MSD_FUNCTION_TRACE("queue_remove_from_Q", TWO_PARAMETERS, (size_t)Queue, 
						(size_t)MsgPtr);
	// verify queue sanity 
	MSD_ASSERT(MsgPtr);
	MSD_ASSERT(((Queue->QueueHead == NULL) && (Queue->QueueTail == NULL))||
		((Queue->QueueHead != NULL) && (Queue->QueueTail != NULL)));
	// the next pointer is nulled before returning 
	if((Ptr = Queue->QueueHead) == NULL){
		return(MD_FAILURE);
	}

	if(Ptr == MsgPtr){
		if((Queue->QueueHead = Ptr->Next)==NULL) {
			Queue->QueueTail = NULL;
		}
		Ptr->Next = NULL;
		return(MD_SUCCESS);
	}
		
	while(Ptr != MsgPtr){
		if(Ptr->Next == NULL){
			return(MD_FAILURE);
		}
		OldPtr = Ptr;
		Ptr = Ptr->Next;
	}

	OldPtr->Next = Ptr->Next;
	if(Ptr->Next==NULL) {
		Queue->QueueTail = OldPtr;
	}
	
	Ptr->Next = NULL;
	return(MD_SUCCESS);
}

/***************************************************************************
 * Function Name		: queue_get_sec_Q
 * Function Type		: Queue Support Function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver GetSecondQueue Routine.
 *			 	  This routine is used to aquire a message 
 *				  from secondary queue.
 * Additional comments		:
 ****************************************************************************/
PMSD_GEN_TWIN_LINK_MSG queue_get_sec_Q(PMSD_QUEUE Queue)
{
	PMSD_GEN_TWIN_LINK_MSG	Ptr;

	// verify queue sanity 
	MSD_ASSERT(((Queue->QueueHead == NULL) && (Queue->QueueTail == NULL))||
		((Queue->QueueHead != NULL) && (Queue->QueueTail != NULL)));
	if(!Queue->QueueTail){	// empty queue 
		return(NULL);
	}

	Ptr = (PMSD_GEN_TWIN_LINK_MSG)Queue->QueueHead;

	// CHECK FOR last item in queue 
	if((Queue->QueueHead = (pmerc_void_t)Ptr->SecondNext) == NULL) 
		Queue->QueueTail = NULL;

	Ptr->SecondNext = NULL;
	return(Ptr);
}

/***************************************************************************
 * Function Name		: queue_put_msg_Q
 * Function Type		: Queue Support Function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver PutMessageQueue Routine.
 *				  This routine is used to put a message on
 *				  the primary queue.
 * Additional comments		:
 ****************************************************************************/
void queue_put_msg_Array(PMSD_ARRAY Array, PSTRM_MSG MsgPtr, int caller)
{
      Uint32 nextTail = 0;
       
      nextTail =  (Array->tail + 1) % MsdControlBlock->arraySz;

      if( nextTail == Array->head ) {
          printk("queue_put_msg_array: Full cond Head %d Tail %d. Caller %d \n",
                     Array->head, Array->tail, caller);
	  MSD_FREE_MESSAGE(MsgPtr);
          return;
      }
      Array->array[Array->tail] = (Uint32 *)MsgPtr;
      Array->tail = nextTail;

      if( caller & 0x8000 ) 
#ifdef LFS
        printk("0x%x:Put: H %d T %d Msg 0x%p \n", caller, Array->head, nextTail, MsgPtr);
#else
        printk("0x%x:Put: H %d T %d Msg 0x%x \n", caller, Array->head, nextTail, MsgPtr);
#endif

      return;
}
          
void queue_put_msg_Q(PMSD_QUEUE Queue, PSTRM_MSG MsgPtr)
{

	PSTRM_MSG	Ptr;

	MSD_FUNCTION_TRACE("queue_put_msg_Q", TWO_PARAMETERS, (size_t)Queue,
				(size_t)MsgPtr);
	// verify queue sanity 
	MSD_ASSERT(((Queue->QueueHead == NULL) && (Queue->QueueTail == NULL))||
		((Queue->QueueHead != NULL) && (Queue->QueueTail != NULL)));
//	printk("Ptr (%x), ptr->b_next (%x) QTail (%x)\n", Ptr, Ptr->b_next, Queue->QueueTail);

	if(!Queue->QueueTail){	// empty queue 
		Queue->QueueHead = (pmerc_void_t)MsgPtr;
		Ptr = MsgPtr;

		while(Ptr->b_next) // look for the last message in chain
			Ptr = Ptr->b_next;
		Queue->QueueTail = (pmerc_void_t)Ptr;
		return;
	}
	Ptr = (PSTRM_MSG)Queue->QueueTail;
//       printk("msdqueue:queue_put_msq_Q: Possible Corruption on the Line below...\n");
	Ptr->b_next = MsgPtr;
//       printk("msdqueue:queue_put_msq_Q: Possible Corruption on the Line Above...\n");
	Queue->QueueTail = (pmerc_void_t) MsgPtr;
}

/***************************************************************************
 * Function Name		: queue_get_msg_Q
 * Function Type		: Queue Support Function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver GetMessageQueue Routine.
 *				  This routine is used to aquire a message
 *				  from primary queue.
 * Additional comments		:
 ****************************************************************************/
PSTRM_MSG queue_get_msg_Array(PMSD_ARRAY Array, int caller)
{
   PSTRM_MSG	Ptr;
   Uint32 head, tail;

    head = Array->head; tail = Array->tail;
   if( tail == head ) return(NULL);

    Ptr = (PSTRM_MSG)Array->array[head];

    if( Ptr == NULL ) 
#ifdef LFS
        printk("Get_array: Ret NULL for Index Head %d Tail %d Val 0x%p\n",
                  head, tail, Ptr);
#else
        printk("Get_array: Ret NULL for Index Head %d Tail %d Val 0x%x\n",
                  head, tail, Ptr);
#endif
    if( caller & 0x8000 ) 
#ifdef LFS
        printk("0x%x: Get: H %d T %d Msg 0x%p \n", caller, head, tail, Ptr);
#else
        printk("0x%x: Get: H %d T %d Msg 0x%x \n", caller, head, tail, Ptr);
#endif

     return(Ptr);

}

PSTRM_MSG queue_get_msg_Q(PMSD_QUEUE Queue)
{
	PSTRM_MSG	Ptr;

	MSD_FUNCTION_TRACE("queue_get_msg_Q", ONE_PARAMETER, (size_t)Queue);
	// verify queue sanity 
	MSD_ASSERT(((Queue->QueueHead == NULL) && (Queue->QueueTail == NULL))||
		((Queue->QueueHead != NULL) && (Queue->QueueTail != NULL)));
	if(!Queue->QueueTail){	// empty queue 
		return(NULL);
	}

	Ptr = (PSTRM_MSG)Queue->QueueHead;

	// CHECK FOR EMPTY queue 
	if((Queue->QueueHead = (pmerc_void_t)Ptr->b_next) == NULL) 
		Queue->QueueTail = NULL;

	Ptr->b_next= NULL;
	return(Ptr);
}

/***************************************************************************
 * Function Name		: queue_put_bk_msg_Q
 * Function Type		: Queue Support Function
 * Inputs			: 
 * Outputs			: 
 * Calling functions		:
 * Description			: Streams Driver PutBackMessageQueue Routine.
 *				  This routine is used to put a message back 
 *				  on the primary queue again.
 * Additional comments		:
 ****************************************************************************/
void queue_commit_msg_Array(PMSD_ARRAY Array, int caller)
{
   Uint32 nextHead = 0;

   nextHead = Array->head;
   nextHead = (nextHead + 1) % MsdControlBlock->arraySz;
   Array->head = nextHead;

   if (caller & 0x8000) 
       printk("0x%x: Commit: H %d T %d \n", caller, nextHead, Array->tail);

   return;

}

void queue_put_bk_msg_Array(PMSD_ARRAY Array, PSTRM_MSG MsgPtr, int caller)
{   
   Uint32 head;
   head = Array->head;
   Array->array[head] = (Uint32 *)MsgPtr;
   if (caller & 0x8000) 
#ifdef LFS
       printk("0x%x: PutBK: H %d T %d Msg 0x%p \n", caller, head, Array->tail, MsgPtr);
#else
       printk("0x%x: PutBK: H %d T %d Msg 0x%x \n", caller, head, Array->tail, MsgPtr);
#endif

   return;
}
 
void queue_put_bk_msg_Q(PMSD_QUEUE Queue, PSTRM_MSG MsgPtr)
{
	PSTRM_MSG	Ptr;

	MSD_FUNCTION_TRACE("queue_put_bk_msg_Q", TWO_PARAMETERS, 
				(size_t)Queue, (size_t)MsgPtr);
	// verify queue sanity 
	MSD_ASSERT(((Queue->QueueHead == NULL) && (Queue->QueueTail == NULL))
		|| ((Queue->QueueHead != NULL) && (Queue->QueueTail != NULL)));
	if(!Queue->QueueTail){	// empty queue 
		Queue->QueueHead = (pmerc_void_t)MsgPtr;
		Ptr = MsgPtr;
		while(Ptr->b_next)	// look for the last message in chain
			Ptr = Ptr->b_next;
		Queue->QueueTail = (pmerc_void_t)Ptr;
		return;
	}
	MsgPtr->b_next = (PSTRM_MSG)Queue->QueueHead;
	Queue->QueueHead= (pmerc_void_t)MsgPtr;
}
