/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : mercd.h
 * Description                  : main driver include
 *
 *
 **********************************************************************/

#ifndef _MERCD_H
#define _MERCD_H

 
/* Defining SPARC Solaris VME/PCI */
 
#ifdef MERCD_SOLARIS
 
#ifndef MERCD_OS_DEFINED
#define MERCD_OS_DEFINED
#define INITIALIZE_OSAL         initialize_solaris_osal
#endif /* MERCD_OS_DEFINED */
 
#ifdef MERCD_PCI
 
#ifndef MERCD_PLATFORM_DEFINED
#define MERCD_PLATFORM_DEFINED
#define INITIALIZE_DHAL         initialize_solaris_pci_dhal
#endif /* MERCD_PLATFORM_DEFINED */
 
#endif /* MERCD_PCI */
 
#ifdef MERCD_VME
 
#ifndef MERCD_PLATFORM_DEFINED
#define MERCD_PLATFORM_DEFINED
#define INITIALIZE_DHAL         initialize_solaris_vme_dhal
#endif /* MERCD_PLATFORM_DEFINED */
 
#endif /* MERCD_VME */
 
#endif /* MERCD_SOLARIS */
 
/* Defining for Unixware 7 DDI 7 */
 
#ifdef MERCD_UNIXWARE
 
#ifndef MERCD_OS_DEFINED
#define MERCD_OS_DEFINED
#define INITIALIZE_OSAL         initialize_unixware_osal
#endif /* MERCD_OS_DEFINED */
 
#ifdef MERCD_PCI
 
#ifndef MERCD_PLATFORM_DEFINED
#define MERCD_PLATFORM_DEFINED
#define INITIALIZE_DHAL         initialize_unixware_pci_dhal
#endif /* MERCD_PLATFORM_DEFINED */
 
#endif /* MERCD_PCI */

#endif /* MERCD_UNIXWARE */

#ifdef MERCD_LINUX

#ifndef MERCD_OS_DEFINED
#define MERCD_OS_DEFINED
#define INITIALIZE_OSAL         initialize_linux_osal
#endif /* MERCD_OS_DEFINED */

#ifdef MERCD_PCI

#ifndef MERCD_PLATFORM_DEFINED
#define MERCD_PLATFORM_DEFINED
#define INITIALIZE_DHAL         initialize_linux_pci_dhal
#endif /* MERCD_PLATFORM_DEFINED */

#endif /* MERCD_PCI */

#endif /* MERCD_LINUX */

#endif // _MERCD_H

