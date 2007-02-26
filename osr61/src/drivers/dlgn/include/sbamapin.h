/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : sbamapin.c
 * Description                  : sba-device mapping
 *
 *
 **********************************************************************/
 
#ifndef __SBAMAPIN_H__
#define __SBAMAPIN_H__

#ifdef __STANDALONE__            /* List of required header files */
#endif



#ifdef __cplusplus
extern "C" {			//  C++ function binding
#define extern
#endif


#if (defined(__STDC__) || defined(__cplusplus))

extern int	      sba_chkport( unsigned short IOid, unsigned char *MemAddr);
#if !defined VME_SPAN && !defined LFS
#ifdef PCI_SPAN
extern int sba_mapin( unsigned short NewBoard, unsigned short PciBoardId );
#else
extern unsigned short sba_mapin( unsigned short NewBoard );
#endif /* PCI_SPAN */
#endif /* VME_SPAN */

#else

extern int	      sba_chkport();
#ifndef VME_SPAN
#ifdef PCI_SPAN
extern int sba_mapin( );
#else
extern unsigned short sba_mapin( );
#endif /* PCI_SPAN */
#endif /* VME_SPAN */

#endif


#ifdef __cplusplus
}
#undef extern
#endif


#endif
/*+*************************************************************************\
 ****			End of File:	sbamapin.h			****
\***************************************************************************/
