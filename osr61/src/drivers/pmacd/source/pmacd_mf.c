/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/

/////////////////////////////////////////////////////////////////////////////
// File Name: pmacd_mf.c
// 
// Defines structures and functions for working with MF's. (Message Frames)
// MFA (Message Frame Address) Queues, and MF Pools 
//
/////////////////////////////////////////////////////////////////////////////

#include "pmacd_mf.h"
#include <asm/io.h>
#include "21554.h"
/*
 *  FunctionName:  pmacd_allocateMFPool()
 *
 *        Inputs:  numberOfMFs - number of MFs to allocate
 *                 sizeOfMFs - size of each message frame.
 *
 *       Outputs:  none.
 *  
 *       Returns:  pointer to a MF pool structure with MF's allocated.
 *                 NULL - if the pool of MFs could not be allocated.
 *
 *   Description:  allocates a contiguous block of message frames, and
 *                 fills in the mfpool structure appropriately.
 *
 *      Comments:
 */
pmacd_mfpool_t *pmacd_allocateMFPool(int numberOfMFs, 
				     int sizeOfMFs){
  pmacd_mfpool_t *mfpool;
  
  // Allocate pool
  mfpool = (pmacd_mfpool_t *)pmacd_malloc(sizeof(pmacd_mfpool_t));
  if(mfpool == NULL) return NULL;

  // Allocate mf block
  mfpool->mfBlockStart = 
    (pmacd_mf_t *)pmacd_mallocPageBlock(sizeOfMFs*numberOfMFs);
  if(mfpool->mfBlockStart == NULL){
    pmacd_free(mfpool, sizeof(pmacd_mfpool_t));
    return(NULL);
  }
  
  // Initialize pool fields
  mfpool->size = sizeOfMFs;
  mfpool->count = numberOfMFs;
  mfpool->mfaQueueAddress = (pmacd_memaddress_t)NULL;
  return(mfpool);
}

/*
 *  FunctionName:  pmacd_freeMFPool()
 *
 *        Inputs:  
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *                 
 *
 *      Comments:  
 */
void pmacd_freeMFPool(pmacd_mfpool_t *mfpool){
  
  if(mfpool == NULL) return;

  if(mfpool->mfBlockStart !=NULL)
    pmacd_freePageBlock((ULONG_T)mfpool->mfBlockStart,
			mfpool->size * mfpool->count);
  
  pmacd_free(mfpool, sizeof(pmacd_mfpool_t));
  return;
}

/*
 *  FunctionName:  pmacd_getMFFromPool()
 *
 *        Inputs:  
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *                 
 *
 *      Comments:  
 */
pmacd_mf_t *pmacd_getMFFromPool(pmacd_mfpool_t *mfPool){
  volatile UINT32_T mfa;
  pmacd_mf_t *mf = (pmacd_mf_t *)NULL;
  
#ifdef LFS
  mfa = readl((caddr_t)mfPool->mfaQueueAddress);
#else
  mfa = readl(mfPool->mfaQueueAddress);
#endif
  if(mfa != I2O_NO_MFA){
    mf = (pmacd_mf_t *)(((ULONG_T)(mfPool->mfBlockStart)) + mfa);
  }
  mfPool->lastMFRetrieved = mf;
  return(mf);
}

/*
 *  FunctionName:  pmacd_returnMFToPool()
 *
 *        Inputs:  
 *
 *       Outputs:  
 *  
 *       Returns:  
 *
 *   Description:  
 *                 
 *
 *      Comments:  
 */
void pmacd_returnMFToPool(pmacd_mfpool_t *mfPool, 
			  pmacd_mf_t *mf){
  UINT32_T mfa;
  if((mf == NULL) || (mfPool == NULL)) return;
  mfa = (UINT32_T)((ULONG_T)mf - (ULONG_T)(mfPool->mfBlockStart));
#ifdef LFS
  writel(mfa, (caddr_t)mfPool->mfaQueueAddress);
#else
  writel(mfa, mfPool->mfaQueueAddress);
#endif
  mfPool->lastMFReturned = mf;
}







