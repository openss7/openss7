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
 *
 *    You can reach me by email to
 *    gram@aztec.co.za
 */

#ident "@(#) LiS stats.c 2.3 11/10/00 15:54:56 "

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

#define	AR	lis_atomic_read
#define	AA	lis_atomic_add
#define	AS	lis_atomic_set

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
