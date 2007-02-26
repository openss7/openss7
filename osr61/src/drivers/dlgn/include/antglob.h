/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : antglob.h
 * Description                  : antares global definitions
 *
 *
 **********************************************************************/

/* Protocol functions global variables */
#if defined(_SCO_) || defined(LINUX)
BYTE far *p_message; /* Pointer to message buffer in allocated memory */
BYTE far *p_reply;/* Pointer to send message reply buffer in allocated memory */
ULONG far *routebuffer;  /* Buffer for routing */
PRCU far *p_rcu;             /* Pointer to physical rcu's elements data */
O_PRCU far *p_orcu;       /* pointer to pseudo rcu's elements */
BOARDDATA p_board[NBOARDS]; /* Array of Boards data */
BD_CHANNEL far *p_bd; /* Array of bulk data buffer in allocated memory */
CHAR raw_open[MAXOPENRAW];  /* Array of raw_dara */

BYTE far *sch_arr;  /* For scheduler cyclic algorithm */
BYTE far *flsh_arr;
#else
BYTE *p_message; /* Pointer to message buffer in allocated memory */
BYTE *p_reply;/* Pointer to send message reply buffer in allocated memory */
ULONG *routebuffer;  /* Buffer for routing */
PRCU *p_rcu;             /* Pointer to physical rcu's elements data */
O_PRCU *p_orcu;       /* pointer to pseudo rcu's elements */
BOARDDATA p_board[NBOARDS]; /* Array of Boards data */
BD_CHANNEL *p_bd; /* Array of bulk data buffer in allocated memory */
SHORT raw_open[MAXOPENRAW];  /* Array of raw_dara */
#ifdef RTDL_ANT 
BYTE *busy_rcu;	      /* busy_rcu[x] == FALSE ==> rcu x is free */
#endif

BYTE *sch_arr;  /* For scheduler cyclic algorithm */
BYTE *flsh_arr;
#endif /* _SCO_ */

CFG_INFO cfg; /* System configuration structure */

BYTE incindex;    /* increament board number in pseudo array */
BYTE startrcu;    /* start in p_rcu array (needed for sort) */
BYTE ustartrcu;   /* start in p_rcu array (needed for unit sort) */

UINT32 DspCall;	/* Global Time Constants */
UINT32 TokenCall;

ULONG Antares_Hverp; /* major version number. */
ULONG Antares_Lverp; /* minor version number. */

SHORT SchSize;/* Scheduler array size */

#ifdef AIX_ANT
int ANT_INT_LVL;
#endif
