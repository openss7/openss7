/************************************************************************
*                        QNX Driver Routines 			 	*
*************************************************************************
*									*
* This file contains abstract routine definitions suitable for running	*
* STREAMS as a QNX driver.						*
*									*
* It includes "port-mdep.c" first and then adds in code specific to	*
* the user level environment.						*
*									*
* Author: Mikel Matthews <mikel@gcom.com>				*
*									*
* Copyright (C) 1997  Mikel Matthews, Gcom, Inc <mikel@gcom.com>	*
*									*
************************************************************************/
/*
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330, Cambridge,
 * MA 02139, USA.
 * 
 */

#ident "@(#) LiS qnx-mdep.c 2.1 4/4/99 11:34:24 "

#include "port-mdep.c"


#include <sys/LiS/head.h>		/* for strread, strwrite, etc */

struct file_operations
lis_streams_fops = {
	NULL,			/* lseek   -- no lseek  */
	lis_strread,		/* read    		*/
	lis_strwrite,		/* write to stream      */
	NULL,			/* readdir -- no readdir*/
	NULL,			/* select  		*/
	lis_strioctl,		/* ioctl   		*/
	NULL,			/* mmap    -- no mmap   */
	lis_stropen,		/* open the stream      */
	lis_strclose,		/* close the stream     */
	NULL,			/* fsync   -- no fsync  */
	NULL,			/* fasync  -- no fasync */
	NULL,			/* check_media_change	*/
	NULL,			/* revalidate		*/
	lis_strputpmsg,		/* putpmsg & putmsg	*/
	lis_strgetpmsg,		/* getpmsg & getmsg	*/
	lis_strpoll		/* poll			*/
};

