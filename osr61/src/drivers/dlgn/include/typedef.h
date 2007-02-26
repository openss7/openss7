/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : typedef.h
 * Description                  : type defines
 *
 *
 **********************************************************************/

#ifndef TRUE
#define TRUE 1						/* Boolean constants */
#endif

#ifndef FALSE
#define FALSE 0
#endif


#ifndef NULL
#define NULL  0
#endif

#ifndef FNULL
#define FNULL (char *)NULL
#endif

#ifndef NO
#define NO    0
#endif

#ifndef YES
#define YES   1
#endif

typedef union {
      BYTE    onebyte[4];
      WORD    twobytes[2];
      UINT32   fourbytes;
   } BYTESWAP;


struct SO {                       /* structures for pointers */
      WORD off;
      WORD seg;
      };

union LXXPTR {                    /* structure for d4get-off/seg */
      struct SO so;
      ADDR *lptr;
      };

union LXXLONG {                   /* structure for d4get-off/seg */
      struct SO so;
      ULONG lptr;
      };


/* Optional Antares driver states */
typedef enum {				
	NotActive,/* When Antares driver is loaded until interrupt is
		activated (end of loading driver) 
		Limits: Everything
		*/
		NotReady,			/* When an_start() until one of the boards is
									addressed well
									Limits: Bulk data protocols, 
											  Most of driver forground functions.
								*/

		Ready,				/* At end of program, until one of the boards
									is found ready (at an_start() 			 
									Limits: Scheduler functions,Queue functions,
											  Most of driver forground functions.
								*/

		Active				/* At least one of the boards is ready.	  
		                     Limits: None.
								*/
				 }	AntaresStates;


