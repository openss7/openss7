/*                               -*- Mode: C -*- 
 * cmd_err.h --- 
 * Author          : Francisco J. Ballesteros, David Grothe
 * Created On      : Tue May 31 21:40:37 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: cmn_err.h,v 1.2 1996/01/27 00:39:48 dave Exp $
 *
 * Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
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


#ifndef _CMD_ERR_H
#define _CMD_ERR_H 1

#ident "@(#) LiS cmn_err.h 2.5 10/01/04 15:07:57 "

#include <sys/LiS/genconf.h>

#define	CE_CONT		0		/* continue printing */
#define	CE_NOTE		1		/* NOTICE */
#define	CE_WARN		2		/* WARNING */
#define	CE_PANIC	3		/* PANIC */

#ifdef __KERNEL__

void    lis_cmn_err_init(void) ;	/* not exported */
void	lis_cmn_err(int err_lvl, char *fmt, ...) _RP __attribute__ ((format(printf, 2, 3)));
#define	cmn_err		lis_cmn_err

#endif				/* __KERNEL__ */

#endif /*!_CMD_ERR_H*/

/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
