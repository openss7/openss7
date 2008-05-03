/*****************************************************************************

 @(#) $RCSfile: padent.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-05-03 21:22:38 $

 -----------------------------------------------------------------------------

 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2000  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

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

 Last Modified $Date: 2008-05-03 21:22:38 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: padent.c,v $
 Revision 0.9.2.2  2008-05-03 21:22:38  brian
 - updates for release

 Revision 0.9.2.1  2008-05-03 13:04:38  brian
 - added support librarie files

 *****************************************************************************/

#ident "@(#) $RCSfile: padent.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-05-03 21:22:38 $"

static char const ident[] = "$RCSfile: padent.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-05-03 21:22:38 $";

#include <netx25/x25_proto.h>
#include <netx25/xnetdb.h>

/** @brief
  * @param p
  * @param strp
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of padtos().
  */
int
__sx25_padtos(struct padent *p, unsigned char *strp)
{
	return (0);
}

/** @fn int padtos(struct padent *p, unsigned char *strp)
  * @param p
  * @param strp
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_padtos().
  */
#pragma weak padtos
__asm__(".symver __sx25_padtos,padtos@@SX25_1.0");

/** @brief
  * @param str_linkid
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of linkidtox25().
  */
unsigned long
__sx25_linkidtox25(unsigned char *str_linkid)
{
	return (0);
}

/** @fn unsigned long linkidtox25(unsigned char *str_linkid)
  * @param str_linkid
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_linkidtox25().
  */
#pragma weak linkidtox25
__asm__(".symver __sx25_linkidtox25,linkidtox25@@SX25_1.0");

/** @brief
  * @param linkid
  * @param str_linkid
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of x25tolinkid().
  */
int
__sx25_x25tolinkid(unsigned long linkid, unsigned char *str_linkid)
{
	return (0);
}

/** @fn int x25tolinkid(unsigned long linkid, unsigned char *str_linkid)
  * @param linkid
  * @param str_linkid
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_x25tolinkid().
  */
#pragma weak x25tolinkid
__asm__(".symver __sx25_x25tolinkid,x25tolinkid@@SX25_1.0");

/** @brief
  * @param linkid
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of getnettype().
  */
int
__sx25_getnettype(unsigned char *linkid)
{
	return (0);
}

/** @fn int getnettype(unsigned char *linkid)
  * @param linkid
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_getnettype().
  */
#pragma weak getnettype
__asm__(".symver __sx25_getnettype,getnettype@@SX25_1.0");

/** @brief
  * @param stayopen
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of setpadent().
  */
void
__sx25_setpadent(int stayopen)
{
}

/** @fn void setpadent(int stayopen)
  * @param stayopen
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_setpadent().
  */
#pragma weak setpadent
__asm__(".symver __sx25_setpadent,setpadent@@SX25_1.0");

/** @brief
  * @param addr
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of getpadbyaddr().
  */
struct padent *
__sx25_getpadbyaddr(char *addr)
{
	return ((struct padent *)0);
}

/** @fn struct padent *getpadbyaddr(char *addr)
  * @param addr
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_getpadbyaddr().
  */
#pragma weak getpadbyaddr
__asm__(".symver __sx25_getpadbyaddr,getpadbyaddr@@SX25_1.0");

/** @brief
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of getpadent().
  */
struct padent *
__sx25_getpadent(void)
{
	return (0);
}

/** @fn struct padent * getpadent(void)
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_getpadent().
  */
#pragma weak getpadent
__asm__(".symver __sx25_getpadent,getpadent@@SX25_1.0");

/** @brief
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of endpadent().
  */
void
__sx25_endpadent(void)
{
}

/** @fn void endpadent(void)
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_endpadent().
  */
#pragma weak endpadent
__asm__(".symver __sx25_endpadent,endpadent@@SX25_1.0");

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS
