/*****************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2005  OpenSS7 Corporation <http://www.openss7.com>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software; you can redistribute it and/or modify it under
 the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later
 version.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 details.

 You should have received a copy of the GNU General Public License along with
 this program; if not, write to the Free Software Foundation, Inc., 675 Mass
 Ave, Cambridge, MA 02139, USA.

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

 *****************************************************************************/

#ident "@(#) $RCSfile$ $Name$($Revision$) $Date$"

/*                               -*- Mode: C -*- 
 * stats.c --- streams statistics
 * Author          : Graham Wheeler
 * Created On      : Tue May 31 22:25:19 1994
 * Last Modified By: David Grothe
 * RCS Id          : $Id: stats.c,v 1.1 1995/12/19 15:50:04 dave Exp $
 * Purpose         : provide some stats for LiS
 * ----------------______________________________________________
 *
 *   Copyright (C) 1995  Graham Wheeler
 *   Copyright (C) 1997  David Grothe, Gcom, Inc <dave@gcom.com>
 *
 *    You can reach me by email to
 *    gram@aztec.co.za
 */

/*  -------------------------------------------------------------------  */
/*				 Dependencies                            */

#include <sys/strport.h>
#include <sys/strconfig.h>	/* config definitions */
#include <sys/LiS/share.h>	/* streams shared defs*/
#include <sys/LiS/stats.h>	/* module interface */

/*  -------------------------------------------------------------------  */
/*			   Local functions & macros                      */

/*  -------------------------------------------------------------------  */
/*				  Glob. Vars                             */

/* This are the stats 
 */
lis_atomic_t lis_strstats[STRMAXSTAT][4] ;


/*  -------------------------------------------------------------------  */
/*			Exported functions & macros                      */

/*  -------------------------------------------------------------------  */
/*                          lis_stat_neg				 */

#define	AR	K_ATOMIC_READ
#define	AA	K_ATOMIC_ADD
#define	AS	K_ATOMIC_SET

void	lis_stat_neg(void)
{
    extern int	lis_negstat ;
    lis_negstat++ ;		/* caught statistic going negative */
}

/* increment count for one item
 * STATUS: complete, untested
 */
void 
LisUpCounter(int item, int n)
{
    int		c ;

    AA(&lis_strstats[item][TOTAL], n) ;
    AA(&lis_strstats[item][CURRENT], n) ;
    if ((c = AR(&lis_strstats[item][CURRENT])) >
	     AR(&lis_strstats[item][MAXIMUM]))
	AS(&lis_strstats[item][MAXIMUM], c);

}/*LisUpCounter*/

/*
 * Set a counter to a particular value and keep track of the max.
 */
void LisSetCounter(int item, int val)
{
    int		c ;

    AA(&lis_strstats[item][TOTAL], val) ;
    AS(&lis_strstats[item][CURRENT], val);
    if ((c = AR(&lis_strstats[item][CURRENT])) >
	     AR(&lis_strstats[item][MAXIMUM]))
	AS(&lis_strstats[item][MAXIMUM], c) ;

} /* LisSetCounter */


/*----------------------------------------------------------------------
# Local Variables:      ***
# change-log-default-name: "~/src/prj/streams/src/NOTES" ***
# End: ***
  ----------------------------------------------------------------------*/
