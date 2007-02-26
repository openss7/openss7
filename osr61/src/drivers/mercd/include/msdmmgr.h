/**********@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********************************
* Copyright (C) 2001-2006 Intel Corporation. All Rights Reserved.
*
* This software program is licensed subject to the GNU General
* Public License (GPL). Version 2, June 1991, available at
* http://www.fsf.org/copyleft/gpl.html
*
***********************************@@@SOFT@@@WARE@@@COPY@@@RIGHT@@@**********/
/**********************************************************************
 * File Name                    : msdmmgr.h
 * Description                  : message mamager definitions
 *
 *
 **********************************************************************/

#define MERCD_NOT_FORCE 0
#define MERCD_FORCE 	1
#define MERCD_STREAMS_BUFFER 1
#define MERCD_KERNEL_BUFFER  0

#define mercd_free(Address, Size, Force) MSD_FREE_KERNEL_MEMORY(Address,Size)
#define mercd_allocator(Size) MSD_ALLOCATE_KERNEL_MEMORY(Size)

#ifndef	MERCD_LINUX

#define mercd_zalloc(Address, Type, Size) \
	(Address)= (Type) MSD_ALLOCATE_KERNEL_MEMORY(Size)
#else
#define mercd_zalloc(Address, Type, Size) \
	(Address)= (Type) MSD_ALLOCATE_KERNEL_MEMORY(Size);\
	if ( (Address) ) \
		MSD_ZERO_MEMORY( (Address), Size)
#endif

#define mercd_streams_free(mp, Type, Force) MSD_FREE_MESSAGE(mp)
#define mercd_streams_allocator(Size,Type, Force) allocb(Size, GFP_ATOMIC)

#if !defined LiS && !defined LIS && !defined LFS
// For now, define this way (aa)......
#define allocb(Size,type) 	native_allocb(Size,type)

#define freeb(mp) 		{if( native_freeb(mp) < 0 ) printk("Freeb: Line %d File %s \n", __LINE__, __FILE__);}
#define freemsg(mp) 		{if( native_freemsg(mp) < 0 ) printk("FreeMsg: Line %d File %s \n", __LINE__, __FILE__);}
#define linkb(mp1,mp2)  	native_linkb(mp1,mp2)
#define unlinkb(mp) 		native_unlinkb(mp)
#define dupb(mp)  		native_dupb(mp)
#define bcopy(src,dst,n)        memcpy(dst,src,n)
#endif
