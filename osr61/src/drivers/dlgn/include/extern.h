/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dlgnextern.h
 * Description                  : extern functions
 *
 *
 **********************************************************************/

/* Protocol functions global variables extern declarations */
#if defined(_SCO_) || defined(LINUX)
extern BYTE far *p_message; /* Pointer to message buffer in allocated memory */
extern BYTE far *p_reply;   /* Pointer to send message reply buffer in allocated memory */
extern ULONG far *routebuffer;  /* Buffer for routing */
extern PRCU far *p_rcu;          /* Pointer to physical rcu's elements data */
extern O_PRCU far *p_orcu;       /* pointer to pseudo rcu's elements */
extern BD_CHANNEL far *p_bd; /* Array of bulk data buffer in allocated memory */
extern CHAR raw_open[];          /* Array of raw_dara */

extern BYTE far *sch_arr;        /* For scheduler cyclic algorithm */
extern BYTE far *flsh_arr;
#else
extern BYTE *p_message; /* Pointer to message buffer in allocated memory */
extern BYTE *p_reply;   /* Pointer to send message reply buffer in allocated memory */
extern ULONG *routebuffer;  /* Buffer for routing */
extern PRCU *p_rcu;		 /* Pointer to physical rcu's elements data */
extern O_PRCU *p_orcu;	 /* pointer to pseudo rcu's elements */
extern BD_CHANNEL *p_bd; /* Array of bulk data buffer in allocated memory */
extern SHORT raw_open[];		 /* Array of raw_dara */
#ifdef RTDL_ANT 
extern BYTE *busy_rcu;     /* array of flags for each rcu in p_rcu: 
                              free/busy entry */
#endif

extern BYTE *sch_arr;	 /* For scheduler cyclic algorithm */
extern BYTE *flsh_arr;
#endif /* _SCO_ */

extern BOARDDATA p_board[];      /* Array of Boards data */
extern CFG_INFO cfg;	 /* System configuration structure */
extern BYTE incindex;	 /* increament board number in pseudo array */
extern BYTE startrcu;	 /* start in p_rcu array (needed for sort) */
extern BYTE ustartrcu;	 /* start in p_rcu array (needed for unit sort) */

extern UINT32 DspCall;		 /* Global Time Constants */
extern UINT32 TokenCall;

extern ULONG Antares_Hverp;	 /* major version number. */
extern ULONG Antares_Lverp;	 /* minor version number. */
extern SHORT SchSize;		 /* Scheduler array size */

#if defined(_SCO_) || defined(LINUX)
extern CHAR huge *BDstartaddress; /* Bulk data buffers start address */
#endif /* _SCO_ */
