/*****************************************************************************

 @(#) $RCSfile: xhostent.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-05-03 21:22:38 $

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

 $Log: xhostent.c,v $
 Revision 0.9.2.2  2008-05-03 21:22:38  brian
 - updates for release

 Revision 0.9.2.1  2008-05-03 13:04:38  brian
 - added support librarie files

 *****************************************************************************/

#ident "@(#) $RCSfile: xhostent.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-05-03 21:22:38 $"

static char const ident[] = "$RCSfile: xhostent.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-05-03 21:22:38 $";

#include <netx25/x25_proto.h>
#include <netx25/xnetdb.h>
#include <string.h>

/** @brief compare two X.25 addresses
  * @param		x1 a pointer to the structure containing the first X.25
  *			address for checking
  * @param x2		a pointer to the structure containing the second X.25
  *			address for checking
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of equalx25().
  *
  * Compares two X.25 addresses by checking to see whether the two xaddrf
  * structures holding them are the same.
  *
  * Returns true if the two structures are the same and false if they are not.
  */
int
__sx25_equalx25(struct xaddrf *x1, struct xaddrf *x2)
{
	if (x1 == x2)
		goto match;
	if (x1->link_id != x2->link_id)
		goto nomatch;
	if (x1->aflags != x2->aflags)
		goto nomatch;
	if (x1->nsap_len != x2->nsap_len)
		goto nomatch;
	if (x1->DTE_MAC.lsap_len != x2->DTE_MAC.lsap_len)
		goto nomatch;
	if (strncmp((char *) x1->DTE_MAC.lsap_add,
		    (char *) x2->DTE_MAC.lsap_add, x1->DTE_MAC.lsap_len))
		goto nomatch;
	if (strncmp((char *) x1->NSAP, (char *) x2->NSAP, x1->nsap_len))
		goto nomatch;
      match:
	/* everything matched */
	return (1);
      nomatch:
	return (0);
}

/** @fn int equalx25(struct xaddrf *x1, struct xaddrf *x2)
  * @param x1
  * @param x2
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_equalx25().
  */
#pragma weak equalx25
__asm__(".symver __sx25_equalx25,equalx25@@SX25_1.0");

/** @brief convert X.25 address to xaddrf structure.
  * @param cp points to a character string containing the X.25 address for
  * conversion.
  * @param xad points to the xaddrf structure containing the X.25 dot format
  * address.
  * @param lookup determines the level of address checking carried out bfore
  * the address is converted.
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of stox25().
  *
  * Converts an X.25 format address into an xaddrf structure.  Can also be used
  * as a validity check for X.25 addresses.
  *
  * When @lookup is zero (0), no address checking is carried out.  This allows
  * for faster conversion, but means the address may not be valid for the type
  * of network on which it is used.  A non-zero value of @lookup means that the
  * address format is checked with the configruation file for the link.
  *
  * Returns zero (0) upon successful completion.  Returns a negative value when
  * the X.25 address is invalid.
  */
int
__sx25_stox25(unsigned char *cp, struct xaddrf *xad, int lookup)
{
	return (-1);
}

/** @fn int stox25(unsigned char *cp, struct xaddrf *xad, int lookup)
  * @param cp
  * @param xad
  * @param lookup
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_stox25().
  */
#pragma weak stox25
__asm__(".symver __sx25_stox25,stox25@@SX25_1.0");

/** @brief convert xaddrf structure to X.25 address
  * @param xad points to the xaddrf structure for conversion.
  * @param cp points to the string the X.25 address will be written to.
  * @param lookup determines the leve of address checking carried out before
  * the structure is converted.
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of x25tos().
  *
  * Converts an xaddrf structure into an X.25 address.  Before doing so, it
  * checks the validity of the xaddrf structure.
  *
  * A @lookup value of zero (0) indicates no checking is carried out.  This
  * allows for faster conversion, but means the structure may not be valid for
  * the type of network to which it refers.  A non-zero value means that the
  * structure is checked using the configuration files.
  *
  * Zero (0) indicates successful completion.  A negative return value
  * indicates that the structure was invalid.
  */
int
__sx25_x25tos(struct xaddrf *xad, unsigned char *cp, int lookup)
{
	return (-1);
}

/** @fn int x25tos(struct xaddrf *xad, unsigned char *cp, int lookup)
  * @param xad
  * @param cp
  * @param lookup
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_x25tos().
  */
#pragma weak x25tos
__asm__(".symver __sx25_x25tos,x25tos@@SX25_1.0");

/** @brief
  * @param stayopen
  * @verion SX25_1.0
  * @par Alias:
  * This function is an implementation of setxhostent();
  */
void
__sx25_setxhostent(int stayopen)
{
}

/** @fn void setxhostent(int stayopen)
  * @param stayopen
  * @verion SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_setxhostent().
  */
#pragma weak setxhostent
__asm__(".symver __sx25_setxhostent,setxhostent@@SX25_1.0");

/** @brief get X.25 host name by address
  * @param addr		A pointer to an xaddrf strcuture contiaining the
  *			address of the host whose entry is to be retrieved.
  * @param len		The length in bytes of addr.
  * @param type		The address type required.  This is always CCITT_X25.
  * @verion SX25_1.0
  * @par Alias:
  * This function is an implementation of getxhostbyaddr();
  *
  * This function searches the xhosts file for an entry with a matching X.25
  * host address.  By default, this file is located in the /etc/x25/ directory.
  * It returns a pointer to an xhostent structure containing information on the
  * entry.
  *
  * Returns a pointer to static storage.  You must copy the value to keep and
  * reuse it.  A return value of NULL indicates the address suspplied is either
  * invalid or unknown.
  */
struct xhostent *
__sx25_getxhostbyaddr(char *addr, int len, int type)
{
	return (NULL);
}

/** @fn struct xhostent *getxhostbyaddr(char *addr, int len, int type)
  * @param addr
  * @param len
  * @param type
  * @verion SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_getxhostbyaddr().
  */
#pragma weak getxhostbyaddr
__asm__(".symver __sx25_getxhostbyaddr,getxhostbyaddr@@SX25_1.0");

/** @brief get X.25 address by name
  * @param name A pointer to the address of a string containing the name of the
  * host whose entry is to be retrieved.
  * @verion SX25_1.0
  * @par Alias:
  * This function is an implementation of getxhostbyname();
  *
  * This function searches the xhosts file for an entry with a matching host
  * name.  By default, this file is located in the /etc/x25/ directory.  It
  * returns a pointer to an xhostent structure containing information on the
  * entry.
  *
  * Returns a pointer to an xhostent structure.  A return value of NULL
  * indicates the name supplied is either invalid or unknown.
  */
struct xhostent *
__sx25_getxhostbyname(char *name)
{
	return (NULL);
}

/** @fn struct xhostent *getxhostbyname(char *name)
  * @param name
  * @verion SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_getxhostbyname().
  */
#pragma weak getxhostbyname
__asm__(".symver __sx25_getxhostbyname,getxhostbyname@@SX25_1.0");

/** @brief
  * @verion SX25_1.0
  * @par Alias:
  * This function is an implementation of getxhostent();
  *
  * This function reads the next line of the xhosts file, typically at
  * /etc/x25/xhosts.  It opens the file if necessary.
  *
  * Returns a pointer to an xhostent structure.  A return value of NULL
  * indicates an error.
  */
struct xhostent *
__sx25_getxhostent(void)
{
	return (NULL);
}

/** @fn struct xhostent *getxhostent(void)
  * @verion SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_getxhostent().
  */
#pragma weak getxhostent
__asm__(".symver __sx25_getxhostent,getxhostent@@SX25_1.0");

/** @brief
  * @verion SX25_1.0
  * @par Alias:
  * This function is an implementation of endxhostent();
  *
  * This function closes the xhosts file.  By default, this file is located in
  * the /etc/x25/ directory.
  */
void
__sx25_endxhostent(void)
{
	return;
}

/** @fn void endxhostent(void)
  * @verion SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_endxhostent().
  */
#pragma weak endxhostent
__asm__(".symver __sx25_endxhostent,endxhostent@@SX25_1.0");

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS
