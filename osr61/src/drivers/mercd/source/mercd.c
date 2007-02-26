/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name 			: mercd.c
 * Description			: Common Entry functions of the 
 *		 		  Unified Mercury Streams Driver 
 *
 *
 ***********************************************************************/
#if defined LiS || defined LIS || defined LFS
#ifdef LFS
#include <sys/kmem.h>
#endif
#ifdef MODULE
#include <linux/module.h>
#endif

#include "msd.h"

#define _MERCD_C
#include "msdextern.h"
#undef _MERCD_C



/*************************************************************************
 *
 *	Module STREAMS functions
 *
 *************************************************************************/

#ifdef LFS
int streamscall mercd_open( queue_t *, dev_t *, int, int, cred_t *);
int streamscall mercd_close(queue_t *, int, cred_t *);
int streamscall mercd_writeput( queue_t *, mblk_t *);
#else
int mercd_open( queue_t *, dev_t *, int, int, cred_t *);
int mercd_close(queue_t *);
int mercd_writeput( queue_t *, mblk_t *);
#endif


/***************************************************************************
 * Function Name		: mercd_open
 * Function Type		: Entry point function
 * Inputs			: q - STREAMS queue
 *				  dev -  major/minor device number
 * 				  flag - normal open() flag
 * 				  sflag - The STREAMS clone open flag
 * Outputs			: none
 * Calling functions		:
 * Description			: Streams Driver Open Routine. This function sets up
 *				  the appropriate driver internal data structures upon
 *				  an open call.  It implements the STREAMS clone open.
 * Additional comments		:
 ****************************************************************************/
#ifdef LFS
int streamscall mercd_open(queue_t *q, dev_t *devp, int flag, int sflag, cred_t *crp)
#else
int mercd_open(q, devp, flag, sflag, crp)
queue_t *q;
dev_t *devp;
int flag;
int sflag;
cred_t *crp;
#endif
{
	static merc_uint_t	dev = 0;
	pmercd_open_block_sT	MsdOpenBlock;

	MSD_FUNCTION_TRACE("mercd_open", FIVE_PARAMETERS, 
	                                 (size_t)q, 
	                                 (size_t)devp,
                                         (size_t)flag,
	                                 (size_t)sflag,
	                                 (size_t)crp);

	/* return minor dev if clone open */

	if ( sflag == CLONEOPEN ) {
		/* 
		 * We are ignoring this for now b/c of 256 minor 
		 * node limitation on LiS/Linux 2.x. 
		 */
	}

	MSD_ENTER_CONTROL_BLOCK_MUTEX();

	if (q->q_ptr) {   /* if already open */
		MSD_ERR_DBGPRINT("mercd_open: (q->q_ptr %#x) is already opened.\n",
		                 q->q_ptr );
		MSD_EXIT_CONTROL_BLOCK_MUTEX();	
		return (EBUSY);
	}

	if( !MsdControlBlock->maxopen) {
		mercd_zalloc( MsdControlBlock->popen_block_queue, 
			      pmercd_open_block_sT *, 
			      MERCD_INITIAL_OPEN_BLOCK_LIST);

		if(MsdControlBlock->popen_block_queue == NULL) {

			MSD_EXIT_CONTROL_BLOCK_MUTEX();
			MSD_ERR_DBGPRINT("mercd_open: memory allocation failed\n");
			return(ENODEV);
		}

		MsdControlBlock->maxopen = MSD_INITIAL_OPEN_ALLOWED;
		MsdControlBlock->open_block_count = 0;

	}
		
	if( MsdControlBlock->open_block_count >= MsdControlBlock->maxopen ) {
		MSD_EXIT_CONTROL_BLOCK_MUTEX();
		MSD_ERR_DBGPRINT( "mercd_open: too many Opens %d \n", MsdControlBlock->open_block_count);
		return (ENODEV);
	}

	/* find an empty block for this open */
	for ( dev %= MsdControlBlock->maxopen; dev < MsdControlBlock->maxopen; dev++ )
		if ( MsdControlBlock->popen_block_queue[ dev ] == NULL )
			break;


	/* Allocate MsdOpenBlock */
	mercd_zalloc( MsdOpenBlock, pmercd_open_block_sT, sizeof(mercd_open_block_sT) );

	if (MsdOpenBlock == NULL) {
	    MSD_EXIT_CONTROL_BLOCK_MUTEX();
	    MSD_ERR_DBGPRINT("mercd_open: memory allocation failed\n"); 
	    return(ENODEV);
	}

	MsdControlBlock->popen_block_queue[dev] = MsdOpenBlock;
	MsdControlBlock->open_block_count++;

#ifndef LFS
	MERCD_GET_MAJOR_NODE( dev );
#endif

	MSD_INIT_MUTEX(&MsdOpenBlock->open_block_mutex, NULL, NULL);

	WR(q)->q_ptr = (pmerc_char_t)MsdOpenBlock;
	q->q_ptr = (pmerc_char_t)MsdOpenBlock;
	MsdOpenBlock->up_stream_write_q = WR(q);
	MsdOpenBlock->clone_device_id = dev;
	MsdOpenBlock->state = MERCD_OPEN_STATE_OPENED;

	qprocson(q); 

#ifndef LFS
 	MOD_INC_USE_COUNT;
#endif

	// made it into a macro since this code is shared across
	// platforms

	MERCD_MAKE_DEVICE( devp, dev % 256 ) ;
	
	//MSD_ERR_DBGPRINT( "major %d, minor %d\n", major(*devp), dev % 256 ) ;

	MSD_EXIT_CONTROL_BLOCK_MUTEX();

	return(0);	
}

/***************************************************************************
 * Function Name		: mercd_close
 * Function Type		: Entry point function
 * Inputs			: q - STREAMS queue
 * Outputs			: none
 * Calling functions		:
 * Description			: Streams Driver Close Routine. This function
 *				  is called when a close() is done on a opened
 *				  STREAM; The Driver unbinds every handle bound
 *				  through this STREAMS queue
 * Additional comments		:
 ****************************************************************************/
#ifdef LFS
int streamscall mercd_close(queue_t *q, int oflag, cred_t *crp)
#else
int mercd_close(q)
queue_t *q;
#endif
{
	pmercd_open_block_sT	MsdOpenBlock;
	pmercd_bind_block_sT    BindBlock;

	MSD_FUNCTION_TRACE("mercd_close", ONE_PARAMETER, (size_t)(q));
	qprocsoff(q);

	MsdOpenBlock = (PMSD_OPEN_BLOCK)q->q_ptr;

	if(MsdOpenBlock == NULL) {
		MSD_ERR_DBGPRINT("mercd_close: q->q_ptr null\n");
		return(ENODEV);
	}

	MSD_ENTER_MUTEX(&MsdOpenBlock->open_block_mutex);

	MsdOpenBlock->state = MERCD_OPEN_STATE_CLOSE_PEND;

	while((BindBlock = (pmercd_bind_block_sT) MsdOpenBlock->bind_block_q.QueueHead) ) {
		
		/* THIS SHOULD NEVER HAPPEN BUT CHECK ANYWAY  */
		if(BindBlock->bindhandle == 0) {
			break; 
		}

		supp_do_unbind(BindBlock);
	}

	MSD_EXIT_MUTEX(&MsdOpenBlock->open_block_mutex);

	/* free open block */
	MSD_ENTER_CONTROL_BLOCK_MUTEX();
	MsdControlBlock->popen_block_queue[MsdOpenBlock->clone_device_id] = NULL;
	MsdControlBlock->open_block_count--;

	MSD_DESTROY_MUTEX(&MsdOpenBlock->open_block_mutex);
	MSD_FREE_KERNEL_MEMORY((pmerc_char_t)MsdOpenBlock, sizeof(mercd_open_block_sT));
	q->q_ptr = (pmerc_char_t) NULL;

#ifndef LFS
 	MOD_DEC_USE_COUNT;
#endif

	MSD_EXIT_CONTROL_BLOCK_MUTEX();
	return (0);
}

/***************************************************************************
 * Function Name		: mercd_writeput
 * Function Type		: Entry point function
 * Inputs			: q - STREAMS queue
 *				  mp - The current message
 * Outputs			: none
 * Calling functions		:
 * Description			: Streams Driver WritePut Routine. Write
 * 				  Side Put Routine
 * Additional comments		:
 ****************************************************************************/
#ifdef LFS
int streamscall mercd_writeput(queue_t *q, mblk_t *mp)
#else
int mercd_writeput(q, mp)
queue_t *q;
mblk_t *mp;
#endif
{

	pmercd_open_block_sT	MsdOpenBlock;
	merc_uint_t	dtype;

	MSD_FUNCTION_TRACE("mercd_writeput", TWO_PARAMETERS, (size_t)(q), (size_t)(mp));

	MSD_ASSERT(q->q_ptr);
	MsdOpenBlock = (PMSD_OPEN_BLOCK)(q->q_ptr);

	dtype = mp->b_datap->db_type;

	switch (dtype) {
	case M_PROTO:
	{
		PMDRV_MSG	MdMsg;

		MdMsg = (PMDRV_MSG)MD_EXTRACT_MDMSG_FROM_STRMMSG(mp);  
		MdMsg->MessagePtr = 0;		
		/* Set the message flag to async, since only the ASYNC type of
		 * messages have the type as M_PROTO */
		MdMsg->MessageFlags = MD_MSG_FLAG_ASYNC;	

		if((MD_GET_MDMSG_ID(MdMsg) > MSD_MAX_DRV_INTER_MESSAGES) || 
		   (MD_GET_MDMSG_ID(MdMsg) <= 0)) {

			MSD_ERR_DBGPRINT("Bad Message id type is supplied, drpopping the message  %x \n", MD_GET_MDMSG_ID(MdMsg));
			break;
		 }

		/* send to Mercury Main Driver */
		(*mercd_osal_mid_func[MD_GET_MDMSG_ID(MdMsg)])(MsdOpenBlock, mp);

		break;
	}
	case M_IOCTL:
	{
		PMDRV_MSG	MdMsg;
		struct iocblk *iocp;

		 /* There should always be some data attached.
		    Sende err to user. */
		if(mp->b_cont == NULL){
			MSD_ERR_DBGPRINT("mercd_writeput: No data portion for IOCTL\n");
			iocp = (struct iocblk *)mp->b_rptr;
			mp->b_datap->db_type = M_IOCACK;
			iocp->ioc_rval = -1;
			iocp->ioc_count = 0;    
			iocp->ioc_error = 0;
			qreply(q, mp);
			break;
		}
	
		/* assume first m_blk is iocmd */
		MdMsg = (PMDRV_MSG)MD_EXTRACT_MDMSG_FROM_STRMMSG(mp->b_cont);
		MdMsg->MessagePtr = (pmerc_void_t)mp;
				 /* save the ptr to iocmd msg for later use */
		MdMsg->MessageFlags = 0;
		/* clear the flags to protect the user 
		 * send data portion to Main Mercury Driver 
		 * Set the message flag to sync, since only the SYNC type of
		 * messages have the type as M_IOCTL*/
		MdMsg->MessageFlags = MD_MSG_FLAG_SYNC;	

	
#if 0

		// The following if statement is for DEBUG purposes only.....
		if( iocp->ioc_cmd == 0x100 ) {
			// MercdDumpStreamValues(*(int *)(mp->b_cont->b_rptr));
			iocp = (struct iocblk *)mp->b_rptr;
			mp->b_datap->db_type = M_IOCACK;
			iocp->ioc_rval = 0;
			iocp->ioc_count = 0;
			iocp->ioc_error = 0;
			qreply(q, mp);
			break;
		}

#endif
        MSD_LEVEL2_DBGPRINT("mercd_writeput: MDMSG  = 0x%x....\n", MD_GET_MDMSG_ID(MdMsg));

		// Protect the driver from any harm user can make
		if((MD_GET_MDMSG_ID(MdMsg) > MSD_MAX_DRV_INTER_MESSAGES) || 
		   (MD_GET_MDMSG_ID(MdMsg) <= 0)) {

			MSD_ERR_DBGPRINT("Bad Message id type is supplied %x \n", MD_GET_MDMSG_ID(MdMsg));
			mp->b_datap->db_type = M_IOCNAK;
			qreply(q, mp);
			break;
		 }
			

		(*mercd_osal_mid_func[MD_GET_MDMSG_ID(MdMsg)])(MsdOpenBlock, 
					mp->b_cont);

		break;
	}

	case M_FLUSH: 
	{

		supp_flush_stream_Q(q, mp);
		break;
	}

	default:
	{
		MSD_ERR_DBGPRINT("mercd_writeput: Bad msg type\n");
		MSD_FREE_MESSAGE(mp);
		break;
	}
	}	 /* Switch */
	return (0);
}

/***************************************************************************
 * Function Name		: mercd_rsvr
 * Function Type		: Entry point function
 * Inputs			: q - STREAMS queue
 *				  mp - The current message
 * Outputs			: none
 * Calling functions		:
 * Description			: Streams Driver ReadSrv Routine. Read
 * 				  Service Routine
 * Additional comments		: Workaround for LiS SMP
 ****************************************************************************/
#ifdef LFS
int streamscall mercd_rsrv(queue_t *q)
#else
int mercd_rsrv (q)
queue_t *q;
#endif
{
	mblk_t *mp;

        if (!q) {
   	    MSD_ERR_DBGPRINT("mercd_rsrv: Queue is NULL\n");
	    return (0);
	}
	
	while ((mp  = getq (q))) {
	       putnext(RD(q),mp);
	}

   return (0); 
}
#endif /* LiS */
