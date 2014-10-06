/*****************************************************************************

 @(#) $RCSfile: xmap_sms.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2010-11-28 14:22:00 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2010  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

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

 Last Modified $Date: 2010-11-28 14:22:00 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap_sms.c,v $
 Revision 1.1.2.2  2010-11-28 14:22:00  brian
 - remove #ident, protect _XOPEN_SOURCE

 Revision 1.1.2.1  2009-06-21 11:37:47  brian
 - added files to new distro

 *****************************************************************************/

static char const ident[] = "$RCSfile: xmap_sms.c,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2010-11-28 14:22:00 $";

#include <xom.h>
#include <xomi.h>
#include <xmap.h>
#include <xmap_sms.h>

/** @brief receive an SMS service indication or confirmation
  * @param assoc open MAP association
  * @param primitive_return return primitive value
  * @param arguments_or_result_return returned primitive argument or result
  * @param invoke_id_return returned invoke identifier for primitive
  *
  * This function may be used to receive an SMS service indication or
  * confirmation primitive from an association.
  */
MAP_status
map_sms_service_rcv(OM_private_object assoc,
		    OM_sint * primitive_return,
		    OM_private_object * arguments_or_result_return,
		    OM_sint * invoke_id_return)
{
}

MAP_status
map_sms_service_req(OM_private_object assoc, OM_object arguments,
		    OM_object component, OM_sint * invoke_id_return)
{
}

MAP_status
map_sms_service_rsp(OM_private_object assoc, OM_object arguments,
		    OM_object component, OM_sint invoke_id)
{
}

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
