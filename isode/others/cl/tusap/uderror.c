/*
 ****************************************************************
 *                                                              *
 *  HULA project						*
 *                                             			*
 *  program:  	uderror.c                                       *
 *  author:   	kurt dobbins                                    *
 *  date:     	3/89                                            *
 *                                                              *
 *  This program implements the connectionless tsap unitdata    *
 *  error handler.						*
 *								*
 ****************************************************************
 *								*
 *								*
 *			     NOTICE		   		*
 *								*
 *    Acquisition, use, and distribution of this module and     *
 *    related materials are subject to the restrictions of a    *
 *    license agreement.					*
 *								*    
 *    This software is for prototype purposes only.		*
 *								*
 ****************************************************************
 */




/*  
 *  Cause the message array to get allocated
 */

#define	UDERR_ALLOCATE  1


#include <stdio.h>
#include <signal.h>
#include "tpkt.h"
#include "tsap.h"
#include "tailor.h"
#include "uderrors.h"




int	uderror (err)

int	err;


{


}

