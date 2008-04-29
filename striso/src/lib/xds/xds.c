/*****************************************************************************

 @(#) $RCSfile: xds.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-29 00:02:02 $

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

 Last Modified $Date: 2008-04-29 00:02:02 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xds.c,v $
 Revision 0.9.2.2  2008-04-29 00:02:02  brian
 - updated headers for release

 Revision 0.9.2.1  2008-04-25 08:33:39  brian
 - added man pages and skeletons

 *****************************************************************************/

#ident "@(#) $RCSfile: xds.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-29 00:02:02 $"

static char const ident[] = "$RCSfile: xds.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2008-04-29 00:02:02 $";

/* This file can be processed with doxygen(1). */

/** @weakgroup xds OpenSS7 XDS Library
  * @{ */

/** @file
  * OpenSS7 X/Open Data Services (XDS) implementation file. */

/**
  * This manual contains documentation of the OpenSS7 XDS Library functions that
  * are generated automatically from the source code with doxygen.  This
  * documentation is intended to be used for maintainers of the OpenSS7 XDS
  * Library and is not intended for users of the OpenSS7 XDS Library.  Users
  * should consult the documentation found in the user manual pages beginning
  * with xds(3).
  */

#include <xds.h>

/** @brief
  * @param session
  * @param invoke_id
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of __xds_ds_abandon().
  */
DS_status
__xds_ds_abandon(OM_private_object session, OM_sint invoke_id)
{
}

/** @fn DS_status ds_abandon(OM_private_object session, OM_sint invoke_id)
  * @param session
  * @param invoke_id
  * @version XDS_1.0
  * @par Alias:
  * This symbol is a strong alias of __xds_ds_abandon().
  */
__asm__(".symver __xds_ds_abandon,ds_abandon@@XDS_1.0");

/** @brief
  * @param session
  * @param context
  * @param name
  * @param entry
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of __xds_ds_add_entry().
  */
DS_status
__xds_ds_add_entry(OM_private_object session, OM_private_object context, OM_object name,
		   OM_object entry, OM_sint * invoke_id_return)
{
}

/** @fn DS_status ds_add_entry(OM_private_object session, OM_private_object context, OM_object name, OM_object entry, OM_sint * invoke_id_return)
  * @param session
  * @param context
  * @param name
  * @param entry
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is a strong alias of __xds_ds_add_entry().
  */
__asm__(".symver __xds_ds_add_entry,ds_add_entry@@XDS_1.0");

/** @brief
  * @param session
  * @param workspace
  * @param bound_session_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of __xds_ds_bind().
  */
DS_status
__xds_ds_bind(OM_object session, OM_workspace workspace, OM_private_object * bound_session_return)
{
}

/** @fn DS_status ds_bind(OM_object session, OM_workspace workspace, OM_private_object * bound_session_return)
  * @param session
  * @param workspace
  * @param bound_session_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is a strong alias of __xds_ds_bind().
  */
__asm__(".symver __xds_ds_bind,ds_bind@@XDS_1.0");

/** @brief
  * @param session
  * @param context
  * @param name
  * @param ava
  * @param result_return
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of __xds_ds_compare().
  */
DS_status
__xds_ds_compare(OM_private_object session, OM_private_object context, OM_object name,
		 OM_object ava, OM_private_object result_return, OM_sint * invoke_id_return)
{
}

/** @fn DS_status ds_compare(OM_private_object session, OM_private_object context, OM_object name, OM_object ava, OM_private_object result_return, OM_sint * invoke_id_return)
  * @param session
  * @param context
  * @param name
  * @param ava
  * @param result_return
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is a strong alias of __xds_ds_compare().
  */
__asm__(".symver __xds_ds_compare,ds_compare@@XDS_1.0");

/** @brief
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of _xds_ds_initialize().
  */
OM_workspace
__xds_ds_initialize(void)
{
}

/** @fn OM_workspace ds_initialize(void)
  * @version XDS_1.0
  * @par Alias:
  * This symbol is a strong alias of _xds_ds_initialize().
  */
__asm__(".symver __xds_ds_initialize,ds_initialize@@XDS_1.0");

/** @brief
  * @param session
  * @param context
  * @param name
  * @param result_return
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of __xds_ds_list().
  */
DS_status
__xds_ds_list(OM_private_object session, OM_private_object context, OM_object name,
	      OM_private_object * result_return, OM_sint * invoke_id_return)
{
}

/** @fn DS_status ds_list(OM_private_object session, OM_private_object context, OM_object name, OM_private_object * result_return, OM_sint * invoke_id_return)
  * @param session
  * @param context
  * @param name
  * @param result_return
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is a strong alias of __xds_ds_list().
  */
__asm__(".symver __xds_ds_list,ds_list@@XDS_1.0");

/** @brief
  * @param session
  * @param context
  * @param name
  * @param changes
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of __xds_ds_modify_entry().
  */
DS_status
__xds_ds_modify_entry(OM_private_object session, OM_private_object context, OM_object name,
		      OM_object changes, OM_sint * invoke_id_return)
{
}

/** @fn DS_status ds_modify_entry(OM_private_object session, OM_private_object context, OM_object name, OM_object changes, OM_sint * invoke_id_return)
  * @param session
  * @param context
  * @param name
  * @param changes
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is a strong alias of __xds_ds_modify_entry().
  */
__asm__(".symver __xds_ds_modify_entry,ds_modify_entry@@XDS_1.0");

/** @brief
  * @param session
  * @param context
  * @param name
  * @param new_RDN
  * @param delete_old_RDN
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of __xds_ds_modify_rdn().
  */
DS_status
__xds_ds_modify_rdn(OM_private_object session, OM_private_object context, OM_object name,
		    OM_object new_RDN, OM_boolean delete_old_RDN, OM_sint * invoke_id_return)
{
}

/** @fn DS_status ds_modify_rdn(OM_private_object session, OM_private_object context, OM_object name, OM_object new_RDN, OM_boolean delete_old_RDN, OM_sint * invoke_id_return)
  * @param session
  * @param context
  * @param name
  * @param new_RDN
  * @param delete_old_RDN
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is a strong alias of __xds_ds_modify_rdn().
  */
__asm__(".symver __xds_ds_modify_rdn,ds_modify_rdn@@XDS_1.0");

/** @brief
  * @param session
  * @param context
  * @param name
  * @param selection
  * @param result_return
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of __xds_ds_read().
  */
DS_status
__xds_ds_read(OM_private_object session, OM_private_object context, OM_object name,
	      OM_object selection, OM_private_object * result_return, OM_sint * invoke_id_return)
{
}

/** @fn DS_status ds_read(OM_private_object session, OM_private_object context, OM_object name, OM_object selection, OM_private_object * result_return, OM_sint * invoke_id_return)
  * @param session
  * @param context
  * @param name
  * @param selection
  * @param result_return
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is a strong alias of __xds_ds_read().
  */
__asm__(".symver __xds_ds_read,ds_read@@XDS_1.0");

/** @brief
  * @param session
  * @param invoke_id
  * @param completion_flag_return
  * @param operation_status_return
  * @param result_return
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of __xds_ds_receive_result().
  */
DS_status
__xds_ds_receive_result(OM_private_object session, OM_sint invoke_id,
			OM_uint * completion_flag_return, DS_status * operation_status_return,
			OM_private_object * result_return, OM_sint * invoke_id_return)
{
}

/** @fn DS_status ds_receive_result(OM_private_object session, OM_sint invoke_id, OM_uint * completion_flag_return, DS_status * operation_status_return, OM_private_object * result_return, OM_sint * invoke_id_return)
  * @param session
  * @param invoke_id
  * @param completion_flag_return
  * @param operation_status_return
  * @param result_return
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is a strong alias of __xds_ds_receive_result().
  */
__asm__(".symver __xds_ds_receive_result,ds_receive_result@@XDS_1.0");

/** @brief
  * @param session
  * @param context
  * @param name
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of __xds_ds_remove_entry().
  */
DS_status
__xds_ds_remove_entry(OM_private_object session, OM_private_object context, OM_object name,
		      OM_sint * invoke_id_return)
{
}

/** @fn DS_status ds_remove_entry(OM_private_object session, OM_private_object context, OM_object name, OM_sint * invoke_id_return)
  * @param session
  * @param context
  * @param name
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is a strong alias of __xds_ds_remove_entry().
  */
__asm__(".symver __xds_ds_remove_entry,ds_remove_entry@@XDS_1.0");

/** @brief
  * @param session
  * @param context
  * @param name
  * @param subset
  * @param filter
  * @param search_aliases
  * @param selection
  * @param result_return
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of __xds_ds_search().
  */
DS_status
__xds_ds_search(OM_private_object session, OM_private_object context, OM_object name,
		OM_sint subset, OM_object filter, OM_boolean search_aliases,
		OM_object selection, OM_private_object * result_return, OM_sint * invoke_id_return)
{
}

/** @fn DS_status ds_search(OM_private_object session, OM_private_object context, OM_object name, OM_sint subset, OM_object filter, OM_boolean search_aliases, OM_object selection, OM_private_object * result_return, OM_sint * invoke_id_return)
  * @param session
  * @param context
  * @param name
  * @param subset
  * @param filter
  * @param search_aliases
  * @param selection
  * @param result_return
  * @param invoke_id_return
  * @version XDS_1.0
  * @par Alias:
  * This symbol is a strong alias of __xds_ds_search().
  */
__asm__(".symver __xds_ds_search,ds_search@@XDS_1.0");

/** @brief
  * @param workspace
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of __xds_ds_shutdown().
  */
DS_status
__xds_ds_shutdown(OM_workspace workspace)
{
}

/** @fn DS_status ds_shutdown(OM_workspace workspace)
  * @param workspace
  * @version XDS_1.0
  * @par Alias:
  * This symbol is strong alias of __xds_ds_shutdown().
  */
__asm__(".symver __xds_ds_shutdown,ds_shutdown@@XDS_1.0");

/** @brief
  * @param session
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of __xds_ds_unbind().
  */
DS_status
__xds_ds_unbind(OM_private_object session)
{
}

/** @fn DS_status ds_unbind(OM_private_object session)
  * @param session
  * @version XDS_1.0
  * @par Alias:
  * This symbol is a strong alias of __xds_ds_unbind().
  */
__asm__(".symver __xds_ds_unbind,ds_unbind@@XDS_1.0");

/** @brief
  * @param feature_list
  * @param workspace
  * @version XDS_1.0
  * @par Alias:
  * This symbol is an implementation of __xds_ds_version().
  */
DS_status
__xds_ds_version(DS_feature feature_list[], OM_workspace workspace)
{
}

/** @fn DS_status ds_version(DS_feature feature_list[], OM_workspace workspace)
  * @param feature_list
  * @param workspace
  * @version XDS_1.0
  * @par Alias:
  * This symbol is a strong alias of __xds_ds_version().
  */
__asm__(".symver __xds_ds_version,ds_version@@XDS_1.0");

/** @} */

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
