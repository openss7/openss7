/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : timer.h
 * Description                  : header for timer module
 *
 *
 **********************************************************************/

#ifndef __SCTTIMER_H__
#define __SCTTIMER_H__

#ifdef __STANDALONE__            /* List of required header files */
#include <sys/types.h>
#include <sys/stream.h>
#include "gnbulk.h"
#endif


/*
 * Defines
 */
#define	SCT_INITED	0x01


/*
 * Timer Structure to store open count information
 */
typedef struct sct_info {
   queue_t	*tinfo_wqp;
   long		tinfo_state;
} SCT_INFO;

#ifdef SOLARIS_24
/*
 * Date structures needed for solaris downloadable drivers.
 */
typedef struct {
  dev_info_t    *sct_devi;
} SCTSTATE;

void sct_msg();
void sct_wqreply(queue_t *, mblk_t *, int);
#endif /* SOLARIS_24 */

/*
 * Define conditional compile entry points to internal debug routines
 */
#ifdef DBGPRT
#   define DEBUG_PRINT_QUEUE(z)      sct_print_queue((z))
#else /* DBGPRT */
#   define DEBUG_PRINT_QUEUE(z)
#endif /* DBGPRT */

#endif /* __SCTTIMER_H__ */

