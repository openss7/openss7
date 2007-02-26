/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : sctqueue.c
 * Description                  : queue handling for Timer Module
 *
 *
 **********************************************************************/
 
#ifdef LFS
#define _LIS_SOURCE	1
#define _SVR4_SOURCE	1
#include <sys/os7/compat.h>
#else
#include <sys/LiS/config.h>
#if (CLONE__CMAJOR_0 > 220)
#define DLGC_LIS_FLAG
#include <sys/LiS/module.h>
#endif
#endif

#include "dlgclinux.h"
#include "dlgcos.h"
#include "sctextern.h"
#include "sctqueue.h"

/* Module Global Variables */
long		timer_cnt = TIMER_MAXNUM;

TIMER_INFO	*qfree = (TIMER_INFO *) NULL;
TIMER_INFO	*quse = (TIMER_INFO *) NULL;

#ifdef LFS
extern spinlock_t st_uselock;
extern spinlock_t st_freelock;
#else
extern lis_spin_lock_t st_uselock;
extern lis_spin_lock_t st_freelock;
#endif

/******************************************************************************
 *        NAME: void st_InitLinkList( tinfoq, size )
 * DESCRIPTION: Initialize the linked list of queues.
 *      INPUTS:	TIMER_INFO	*tinfop;	- Ptr to timer queue
 *		long		size;		- Sice of the queue
 *     OUTPUTS:	None
 *     RETURNS: None
 *	 CALLS:
 *    CAUTIONS: None
 *****************************************************************************/

#ifdef LFS
void st_InitLinkList( TIMER_INFO *tinfoq, long size );
#endif
void st_InitLinkList( tinfoq, size )
   TIMER_INFO *tinfoq;
   long size;
{
#ifndef LFS
   TIMER_INFO	*tmp;
#endif
   long		i;

   /* The first in queue */
   tinfoq->prev = (TIMER_INFO *) NULL;
   tinfoq->next = (tinfoq + 1);

   for ( i = 1; i < size - 1; i++ ) {
      (tinfoq + i)->prev = (tinfoq + i - 1);
      (tinfoq + i)->next = (tinfoq + i + 1);
   }

   /* Last in the queue */
   (tinfoq + size - 1)->prev = (tinfoq + size - 2);
   (tinfoq + size - 1)->next = (TIMER_INFO *) NULL;
}


/******************************************************************************
 *        NAME: void st_InitQueue( entries )
 * DESCRIPTION: Initialise the queues
 *		If the number of entries id 0, then initialise it for
 *		the default value. Any other value received then it
 *		will be initialized to that value.
 *      INPUTS: long	entries;	- Number of entries in the queue
 *     OUTPUTS:	None
 *     RETURNS:  0 = success
 *		-1 = error
 *		-2 = Q is already in use.
 *	 CALLS:
 *    CAUTIONS: None
 *****************************************************************************/

int st_InitQueue( entries )
   long entries;
{
   DLGCDECLARESPL(oldspl)
   TIMER_INFO *tmp;

   /*
    * Possible values of entries:
    *	0 => use default, hence initialised first time
    *	anything else is the appropriate value.
    * A negative value is also valid, but it will have
    * no effect on the queue size
    */
   if ( entries ) {
      if ( entries < timer_cnt ) {	/* We already have enough queues */
	 return( 0 );
      } else {				/* We Need to INCREASE the queues */
	 /*
	  * Only increase the queue size if no queue entries are used
	  */
	 if ( quse ) {
	    return( -2 );
	 }

	 /*
	  * Increase the queue size
	  */
	 if ((tmp = (TIMER_INFO *) kmalloc(sizeof(TIMER_INFO)*entries, GFP_KERNEL)) == NULL)
	 {
	    DLGC_MSG2( CE_CONT,
		"SCSAtmr: st_InitQueue(): Cannot allocate structure space" );
	    return( -1 );
	 }
        
	 bzero((char *) tmp, sizeof(TIMER_INFO)*entries);

	 DLGCSPLSTR(oldspl);

	 /*
	  * Create a linked-list of the above allocated memory
	  */
	 st_InitLinkList( tmp, entries );
	 timer_cnt = entries;

	 /*
	  * Successfully initialised, hence need to update qfree
	  *
	  * qfree should always be defined, but this extra check
	  * is here only to make sure that the KERNEL does not
	  * panic ever from this module
	  */
	 if ( qfree ) {
            kfree(qfree);
	 }

	 qfree = tmp;
	 DLGCSPLX(oldspl);
      }
   } else {
      /*
       * If either of the qfree or the quse is non-null, then
       * it implies that the queue has already been initialised
       * and hence must return an error.
       */
      if ( qfree || quse ) {
	 return( -1 );
      }

      DLGCSPLSTR(oldspl);

      /* Use the default number of entries */
      qfree = (TIMER_INFO *) kmalloc((sizeof(TIMER_INFO)*timer_cnt), GFP_KERNEL);
      if (qfree == NULL) {
	 DLGC_MSG2( CE_CONT,
		"SCSAtmr: st_InitQueue(): Cannot allocate structure space" );
	 DLGCSPLX(oldspl);
	 return( -1 );
      }

      /*
       * Create a linked-list of the above allocated memory
       */
      st_InitLinkList( qfree, timer_cnt );

      DLGCSPLX(oldspl);
   }

   return( 0 );
}


/******************************************************************************
 *        NAME: int st_Add2UsedQ( qptr, msgp, timeInTicks, ddd, identifier )
 * DESCRIPTION: Move a queue entry from FreeQ to the UsedQ at the
 *		correct position. The position of the UsedQ is in
 *		ascending order of expirations of the timers.
 *      INPUTS: queue_t	*qptr;		- queue pointer
 *		mblk_t	*msgp;		- msg ptr - RFU
 *		long	timeInTicks;	- Number of ticks before event generated
 *		long	ddd;		- Event to be generated on this ddd
 *		long	identifier;	- Id for the event
 *     OUTPUTS:	None
 *     RETURNS:  0 = success
 *		-1 = error
 *	 CALLS:
 *    CAUTIONS: None
 *****************************************************************************/

int st_Add2UsedQ( qptr, msgp, timeInTicks, ddd, identifier )
   queue_t	*qptr;
   mblk_t	*msgp;
   long		timeInTicks;
   long		ddd;
   long		identifier;
{
   TIMER_INFO	*tmp;
   TIMER_INFO	*newq;
   long		expiretime = 0;
   int		z_flag;
   DLGCDECLARESPL(oldspl)

   if(timeInTicks<0)
     {
	cmn_err(CE_WARN,"timeinticks value is submitted negative %ld \n",timeInTicks);
	return( -1 );
     }


   DLGCSPLSTR(oldspl);


   DLGCMUTEX_ENTER(&st_freelock);
   DLGCSPINLOCK(st_freelock, z_flag);
   /*
    * If no free queue (NO AVAILABLE SPACE) then return an error
    */
   if ( ! qfree ) {
      DLGCSPLX(oldspl);
      DLGCMUTEX_EXIT(&st_freelock);
      DLGCSPINUNLOCK(st_freelock, z_flag);
      DLGC_MSG2( CE_CONT, "SCSAtmr: st_Add2UsedQ(): No free queues" );
      return( -1 );
   }

   newq = qfree;;
   if ( qfree->next ) {
     qfree->next->prev = (TIMER_INFO *) NULL;
   }
   qfree = qfree->next;
   DLGCMUTEX_EXIT(&st_freelock);
   DLGCSPINUNLOCK(st_freelock, z_flag);

   DLGCMUTEX_ENTER(&st_uselock);
   DLGCSPINLOCK(st_uselock, z_flag);
   tmp = quse;
   /*
    * If there is nothing in the queue then just add an
    * entry and return
    */
   if ( ! quse ) {
      quse = newq;
      quse->next = quse->prev = NULL;

      quse->ti_qptr = qptr;
      quse->ti_msgp = msgp;
      quse->ti_timeleft = quse->ti_timeout = timeInTicks;
      quse->ti_ddd = ddd;
      quse->ti_id = identifier;
      DLGCSPLX(oldspl);
      DLGCMUTEX_EXIT(&st_uselock);
      DLGCSPINUNLOCK(st_uselock, z_flag);
      return( 0 );
   }

   expiretime += tmp->ti_timeleft;
   if ( expiretime > timeInTicks ) {
      /*
       * Insert before the first entry and return
       */
      newq->next = quse;
      quse->prev = newq;
      newq->prev = (TIMER_INFO *) NULL;
      quse = newq;

      quse->ti_qptr = qptr;
      quse->ti_msgp = msgp;
      quse->ti_timeout = timeInTicks;
      quse->ti_timeleft = quse->ti_timeout;
      quse->next->ti_timeleft -= quse->ti_timeleft;
      quse->ti_ddd = ddd;
      quse->ti_id = identifier;
      DLGCSPLX(oldspl);
      DLGCMUTEX_EXIT(&st_uselock);
      DLGCSPINUNLOCK(st_uselock, z_flag);
      return( 0 );
   }

   while ( tmp->next ) {
      tmp = tmp->next;
      expiretime += tmp->ti_timeleft;
      if ( expiretime > timeInTicks ) {
	 /*
	  * Insert before this entry and return
	  */
	 newq->next = tmp;
	 newq->prev = tmp->prev;
	 tmp->prev->next = newq;
	 tmp->prev = newq;

	 newq->ti_qptr = qptr;
	 newq->ti_msgp = msgp;
	 newq->ti_timeout = timeInTicks;
	 newq->ti_timeleft = timeInTicks - (expiretime - tmp->ti_timeleft);
	 tmp->ti_timeleft = expiretime - timeInTicks;
	 newq->ti_ddd = ddd;
	 newq->ti_id = identifier;

	 DLGCSPLX(oldspl);
	 DLGCMUTEX_EXIT(&st_uselock);
         DLGCSPINUNLOCK(st_uselock, z_flag);
	 return( 0 );
      }
   }

   /*
    * Insert after this entry and return
    */

   newq->prev = tmp;
   tmp->next = newq;
   newq->next = (TIMER_INFO *) NULL;
   
   newq->ti_qptr = qptr;
   newq->ti_msgp = msgp;
   newq->ti_timeout = timeInTicks;

   newq->ti_timeleft = timeInTicks - expiretime;
   newq->ti_ddd = ddd;
   newq->ti_id = identifier;

   DLGCSPLX(oldspl);
   DLGCMUTEX_EXIT(&st_uselock);
   DLGCSPINUNLOCK(st_uselock, z_flag);
   return( 0 );
}


/******************************************************************************
 *        NAME: int st_Add2FreeQ( infop )
 * DESCRIPTION: Move a queue entry from UsedQ to the FreeQ. The
 *		entry is always inserted at the beginning of the
 *		FreeQ.
 *      INPUTS: TIMER_INFO *infop;	- entry to move
 *     OUTPUTS:	None
 *     RETURNS:  0 = success
 *		-1 = error
 *	 CALLS:
 *    CAUTIONS: None
 *****************************************************************************/

int st_Add2FreeQ( infop )
   TIMER_INFO *infop;
{
   int z_flag;
#ifndef LFS
   TIMER_INFO *tmp;
#endif
   DLGCDECLARESPL(oldspl)

   /*
    * Make sure infop points to a valid entry.
    */
   if ( ! infop ) {
      return( -1 );
   }

   DLGCSPLSTR(oldspl);

   if ( infop->prev ) {
      /*
       * Adjust the timeleft if infop->next is pointing to something,
       * otherwise we are at the end of the queue and hence no
       * need to update the timeleft.
       */
      if ( infop->next ) {
      infop->next->ti_timeleft += infop->ti_timeleft;
      }

      /*
       * Middle of the list to be moved/removed
       */
      infop->prev->next = infop->next;

      if ( infop->next ) {
	 infop->next->prev = infop->prev;
      }

   } else {
      /*
       * We are at the beginning of the list.
       * We must update the next one's timeleft according
       * to how much time has been left from this entry.
       */
      if ( infop->next ) {
	 /*
	  * Update timeleft
	  */
	 infop->next->ti_timeleft += infop->ti_timeleft;
	 infop->next->prev = (TIMER_INFO *) NULL;
      }

      if ( infop == quse ) {
	 quse = infop->next;
      }
   }

   /*
    * Now add to the beginning of the FreeQ list
    */
   DLGCMUTEX_ENTER(&st_freelock);
   DLGCSPINLOCK(st_freelock, z_flag);
   if ( qfree ) {
      qfree->prev = infop;
      infop->next = qfree;
      infop->prev = (TIMER_INFO *) NULL;
   } else {
      infop->next = infop->prev = (TIMER_INFO *) NULL;
   }

   /*
    * NULL the values of the structure.
    */
   infop->ti_timeout = infop->ti_timeleft = infop->ti_ddd = infop->ti_id = 0;
   infop->ti_qptr = (queue_t *) NULL;
   if ( infop->ti_msgp ) {
      freemsg( infop->ti_msgp );
   }
   infop->ti_msgp = (mblk_t *) NULL;

   qfree = infop;

   DLGCSPLX(oldspl);
   DLGCMUTEX_EXIT(&st_freelock);
   DLGCSPINUNLOCK(st_freelock, z_flag);
   return( 0 );
}


/******************************************************************************
 *        NAME: TIMER_INFO * st_FindEntry( qptr, ddd, id )
 * DESCRIPTION: Return a pointer to the timer entry which has the
 *		appropriate ddd and id.
 *		If id == -1, then it implies a wildcard for the id.
 *      INPUTS: queue_t	*qptr;		- queue pointer
 *		long	ddd;		- device ddd
 *		long	id;		- identifier
 *     OUTPUTS:	None
 *     RETURNS: Pointer to entry if found
 *		else a NULL pointer
 *	 CALLS:
 *    CAUTIONS: None
 *****************************************************************************/

TIMER_INFO * st_FindEntry( qptr, ddd, id )
   queue_t *qptr;
   long ddd;
   long id;
{
   TIMER_INFO *tmp = quse;

   /*
    * There is no update of the queue, so we do not need to
    * protect the critical regions?
    */

   while ( tmp ) {
      /*
       * If  id == -1  then use it as a wild card
       */
      if ( id == -1 ) {
	 if ( (ddd == tmp->ti_ddd) && (qptr == tmp->ti_qptr) ) {
	    return( tmp );
	 }
      } else {
	 if ( (ddd == tmp->ti_ddd) && (id == tmp->ti_id) &&
		(qptr == tmp->ti_qptr) ) {
	    return( tmp );
	 }
      }

      tmp = tmp->next;
   }

   return( (TIMER_INFO *) NULL );
}
