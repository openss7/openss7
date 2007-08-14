/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2007  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program.  If not, see <http://www.gnu.org/licenses/>, or write to the
 Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

#ident "@(#) $RCSfile: qnx-mdep.c,v $ $Name:  $($Revision: 1.1.1.1.12.2 $) $Date: 2005/07/13 12:01:18 $"

static char const ident[] = "$RCSfile$ $Name$($Revision$) $Date$";

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

#include "port-mdep.c"

#include <sys/LiS/head.h>	/* for strread, strwrite, etc */

struct file_operations lis_streams_fops = {
	NULL,				/* lseek -- no lseek */
	lis_strread,			/* read */
	lis_strwrite,			/* write to stream */
	NULL,				/* readdir -- no readdir */
	NULL,				/* select */
	lis_strioctl,			/* ioctl */
	NULL,				/* mmap -- no mmap */
	lis_stropen,			/* open the stream */
	lis_strclose,			/* close the stream */
	NULL,				/* fsync -- no fsync */
	NULL,				/* fasync -- no fasync */
	NULL,				/* check_media_change */
	NULL,				/* revalidate */
	lis_strputpmsg,			/* putpmsg & putmsg */
	lis_strgetpmsg,			/* getpmsg & getmsg */
	lis_strpoll			/* poll */
};
