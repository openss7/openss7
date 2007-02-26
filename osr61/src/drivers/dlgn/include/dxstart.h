/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : dxstart.h
 * Description                  : SRAM prptocol startup/shutdown
 *
 *
 **********************************************************************/

#ifndef __DXSTART_H__
#define __DXSTART_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif


/*
 * DX_INITBD: Device-dependent board initialization structure. Used
 * with vox PM at PM startup time.
 */
typedef struct dx_initbd {
   unsigned int  dib_type;              /* Board type code */
   unsigned char  *dib_physaddr;         /* Board physical address */
#ifndef VME_SPAN
   unsigned short dib_totmem;            /* Board shared memory length */
   unsigned short dib_ioport;            /* Board IO port address */
#ifdef BRI_SUPPORT
   unsigned short  dib_dslnum;           /* BRI board DSL number */
#endif /* BRI_SUPPORT */
   unsigned short dib_rfu[2];
#else
   unsigned int dib_totmem;            /* Board shared memory length */
   unsigned int dib_ioport;            /* Board IO port address */
   unsigned int dib_rfu[2];
#endif /* VME_SPAN */
} DX_INITBD;


#endif

