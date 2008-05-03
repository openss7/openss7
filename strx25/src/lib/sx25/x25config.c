/*****************************************************************************

 @(#) $RCSfile: x25config.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-05-03 21:22:38 $

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

 $Log: x25config.c,v $
 Revision 0.9.2.2  2008-05-03 21:22:38  brian
 - updates for release

 Revision 0.9.2.1  2008-05-03 13:04:38  brian
 - added support librarie files

 *****************************************************************************/

#ident "@(#) $RCSfile: x25config.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-05-03 21:22:38 $"

static char const ident[] = "$RCSfile: x25config.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-05-03 21:22:38 $";

struct link_data;
struct config_ident;
struct LINK_config_data;
struct X25_config_data;
struct MLP_config_data;
struct LAPB_config_data;
struct LLC2_config_data;
struct WAN_config_data;

/** @brief
  * @param lptr
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of x25_find_link_parameters().
  */
int
__sx25_find_link_parameters(struct link_data **lptr)
{
	return (0);
}

/** @fn int x25_find_link_parameters(struct link_data **lptr)
  * @param lptr
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_find_link_parameters().
  */
#pragma weak x25_find_link_parameters
__asm__(".symver __sx25_find_link_parameters,x25_find_link_parameters@@SX25_1.0");

/** @brief
  * @param linkid
  * @param ipt
  * @param lpt
  * @param xpt
  * @param mpt
  * @param lbp
  * @param l2p
  * @param wpt
  * @param flags
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of x25_read_config_parameters().
  */
int
__sx25_read_config_parameters(int linkid,
			      struct config_ident *ipt,
			      struct LINK_config_data *lpt,
			      struct X25_config_data *xpt,
			      struct MLP_config_data *mpt,
			      struct LAPB_config_data *lbp,
			      struct LLC2_config_data *l2p, struct WAN_config_data *wpt, int *flags)
{
	return (0);
}

/** @fn int x25_read_config_parameters(int linkid, struct config_ident *ipt, struct LINK_config_data *lpt, struct X25_config_data *xpt, struct MLP_config_data *mpt, struct LAPB_config_data *lbp, struct LLC2_config_data *l2p, struct WAN_config_data *wpt, int *flags)
  * @param linkid
  * @param ipt
  * @param lpt
  * @param xpt
  * @param mpt
  * @param lbp
  * @param l2p
  * @param wpt
  * @param flags
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_read_config_parameters().
  */
#pragma weak x25_read_config_parameters
__asm__(".symver __sx25_read_config_parameters,x25_read_config_parameters@@SX25_1.0");

/** @brief
  * @param filename
  * @param ipt
  * @param lpt
  * @param xpt
  * @param mpt
  * @param lbp
  * @param l2p
  * @param wpt
  * @param flags
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of x25_read_config_parameters_file().
  */
int
__sx25_read_config_parameters_file(char *filename,
				   struct config_ident *ipt,
				   struct LINK_config_data *lpt,
				   struct X25_config_data *xpt,
				   struct MLP_config_data *mpt,
				   struct LAPB_config_data *lbp,
				   struct LLC2_config_data *l2p,
				   struct WAN_config_data *wpt, int *flags)
{
	return (0);
}

/** @fn int x25_read_config_parameters_file(char *filename, struct config_ident *ipt, struct LINK_config_data *lpt, struct X25_config_data *xpt, struct MLP_config_data *mpt, struct LAPB_config_data *lbp, struct LLC2_config_data *l2p, struct WAN_config_data *wpt, int *flags)
  * @param filename
  * @param ipt
  * @param lpt
  * @param xpt
  * @param mpt
  * @param lbp
  * @param l2p
  * @param wpt
  * @param flags
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_read_config_parameters_file().
  */
#pragma weak x25_read_config_parameters_file
__asm__(".symver __sx25_read_config_parameters_file,x25_read_config_parameters_file@@SX25_1.0");

/** @brief
  * @param linkid
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of x25_save_link_parameters().
  */
int
__sx25_save_link_parameters(struct link_data *linkid)
{
	return (0);
}

/** @fn int x25_save_link_parameters(struct link_data *linkid)
  * @param linkid
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_save_link_parameters().
  */
#pragma weak x25_save_link_parameters
__asm__(".symver __sx25_save_link_parameters,x25_save_link_parameters@@SX25_1.0");

/** @brief
  * @param func
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of x25_set_parse_error_function().
  */
int (*__sx25_set_parse_error_function(int (*func)(char *))) (char *)
{
	return ((int (*)(char *))0);
}

/** @fn int (*) (char *) x25_set_parse_error_function(int (*) (char *) func)
  * @param func
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_set_parse_error_function().
  */
#pragma weak x25_set_parse_error_function
__asm__(".symver __sx25_set_parse_error_function,x25_set_parse_error_function@@SX25_1.0");

/** @brief
  * @param linkid
  * @param ipt
  * @param lpt
  * @param xpt
  * @param mpt
  * @param lbp
  * @param l2p
  * @param wpt
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of x25_write_config_parameters().
  */
int
__sx25_write_config_parameters(int linkid,
			       struct config_ident *ipt,
			       struct LINK_config_data *lpt,
			       struct X25_config_data *xpt,
			       struct MLP_config_data *mpt,
			       struct LAPB_config_data *lbp,
			       struct LLC2_config_data *l2p, struct WAN_config_data *wpt)
{
	return (0);
}

/** @fn int x25_write_config_parameters(int linkid, struct config_ident *ipt, struct LINK_config_data *lpt, struct X25_config_data *xpt, struct MLP_config_data *mpt, struct LAPB_config_data *lbp, struct LLC2_config_data *l2p, struct WAN_config_data *wpt)
  * @param linkid
  * @param ipt
  * @param lpt
  * @param xpt
  * @param mpt
  * @param lbp
  * @param l2p
  * @param wpt
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_write_config_parameters().
  */
#pragma weak x25_write_config_parameters
__asm__(".symver __sx25_write_config_parameters,x25_write_config_parameters@@SX25_1.0");

/** @brief
  * @param filename
  * @param ipt
  * @param lpt
  * @param xpt
  * @param mpt
  * @param lbp
  * @param l2p
  * @param wpt
  * @version SX25_1.0
  * @par Alias:
  * This function is an implementation of x25_write_config_parameters_file().
  */
int
__sx25_write_config_parameters_file(char *filename,
				    struct config_ident *ipt,
				    struct LINK_config_data *lpt,
				    struct X25_config_data *xpt,
				    struct MLP_config_data *mpt,
				    struct LAPB_config_data *lbp,
				    struct LLC2_config_data *l2p, struct WAN_config_data *wpt)
{
	return (0);
}

/** @fn int x25_write_config_parameters_file(char *filename, struct config_ident *ipt, struct LINK_config_data *lpt, struct X25_config_data *xpt, struct MLP_config_data *mpt, struct LAPB_config_data *lbp, struct LLC2_config_data *l2p, struct WAN_config_data *wpt)
  * @param filename
  * @param ipt
  * @param lpt
  * @param xpt
  * @param mpt
  * @param lbp
  * @param l2p
  * @param wpt
  * @version SX25_1.0
  * @par Alias:
  * This symbol is a weak alias of __sx25_write_config_parameters_file().
  */
#pragma weak x25_write_config_parameters_file
__asm__(".symver __sx25_write_config_parameters_file,x25_write_config_parameters_file@@SX25_1.0");

// vim: com=sr0\:/**,mb\:*,ex\:*/,sr0\:/*,mb\:*,ex\:*/,b\:TRANS
