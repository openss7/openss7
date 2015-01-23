/*****************************************************************************

 @(#) File: src/lib/xtcap.c

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2015  Monavacon Limited <http://www.monavacon.com/>
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

 *****************************************************************************/

static char const ident[] = "File: " __FILE__ "  Version: " PACKAGE_ENVR "  Date: " PACKAGE_DATE;

/* OM workspace functions */

/** @brief initialize a new TC OM workspace
  *
  * Initializes a new TC OM workspace and returns a pointer to the TC OMP
  * workspace definition.  The TC OM workspace consists of all of the
  * information necessary to support a group of TC User entities.
  * 
  */
OM_workspace
__xtc_tc_initialize(void)
{
}

/** @brief shutdown a TC OM workspace
  * @param workspace workspace to shut down
  *
  * Shuts down a TC OM workspace.  This consists of aborting all open
  * dialogues, unbinding and closing all TC User entities, and freeing the
  * resources associated with the workspace.
  */
void
__xtc_tc_shutdown(OM_workspace workspace)
{
}

TC_status
__xtc_tc_validate_object(OM_workspace workspace, OM_object test_object,
			 OM_private_object * bad_argument_return)
{
}

TC_status
__xtc_tc_get_last_error(OM_workspace workspace,
			OM_uint32 * additional_error_return)
{
}

TC_status
__xtc_tc_negotiate(TC_feature feature_list[], OM_workspace workspace)
{
}

TC_status
__xtc_tc_wait(TC_waiting_entities bound_entity_list[], OM_workspace workspace,
	      OM_uint32 timeout)
{
}

/* TC entity functions */
TC_status
__xtc_tc_receive(OM_private_object entity,
		 OM_sint * primitive_return,
		 OM_private_object *dialog_return,
		 TC_status * operation_notification_status_return,
		 OM_private_object * result_or_argument_return)
{
}

/** @brief open and bind a TC User
  * @param entity object describing TC user including address to bind
  * @param workspace an initialized TC OM workspace
  * @param bound_entity_return opened and bound entity
  *
  * This function accepts an entity specification and an intiialized TC OM
  * workspace and opens and binds a TC user as specified.  When successful, a
  * bound entity object is returned that may be used in conjunction with other
  * TC functions.  Note that each bound TC User has a file descriptor
  * associated with it.  Also, the bound TC User entity has additional
  * information associated with it.
  */
TC_status
__xtc_tc_bind(OM_object entity, OM_workspace workspace,
	      OM_private_object * bound_entity_return)
{
}

/** @brief unbind and close a TC user
  * @param entity bound entity object to close
  *
  * Unbinds and closes a TC user entity.  Upon success, the passed entity
  * object is invalidated and must no longer be used by the XTC interface
  * user.
  */
TC_status
__xtc_tc_unbind(OM_private_object entity)
{
}

/** @brief coordinated out-of-service request
  * @param entity bound entity
  */
TC_status
__xtc_tc_coord_req(OM_private_object entity, OM_private_object *result_return)
{
}

/** @brief coorinated out-of-service response
  * @param entity bound entity
  * @param remote remote entity
  */
TC_status
__xtc_tc_coord_res(OM_private_object entity, OM_private_object remote)
{
}

/** @brief entity state request
  * @param entity bound entity
  * @param arguments state requested
  */
TC_status
__xtc_tc_state_req(OM_private_object entity, OM_object arguments)
{
}

/* TC dialog functions */
TC_status
__xtc_tc_uni_req(
		OM_private_object entity,
		OM_object arguments)
{
}

TC_status
__xtc_tc_begin_req(
		OM_private_object entity,
		OM_private_object dialog,
		OM_object arguments,
		OM_private_object * result_return)
{
}

TC_status
__xtc_tc_begin_res(
		OM_private_object dialog,
		OM_object arguments,
		OM_private_object * result_return)
{
}

TC_status
__xtc_tc_cont_req(
		OM_private_object dialog,
		OM_object arguments,
		OM_private_object * result_return)
{
}

TC_status
__xtc_tc_end_res(
		 OM_private_object dialog,
		 OM_object arguments)
{
}

TC_status
__xtc_tc_end_req(
		 OM_private_object dialog,
		 OM_object arguments)
{
}

TC_status
__xtc_tc_abort_req(
		   OM_private_object dialog,
		   OM_object arguments)
{
}

/* TC component functions */

TC_status
__xtc_tc_receive_components(
		OM_private_object dialog,
		OM_sint *primitive_return,
		OM_private_object *result_return,
		OM_uint32 *invoke_id_return
		)
{
}

TC_status
__xtc_tc_invoke_req(OM_private_object entity,
		    OM_object dialog,
		    OM_object arguments,
		    OM_private_object *dialog_return,
		    OM_uint32 *invoke_id_return)
{
}

TC_status
__xtc_tc_result_req(
		    OM_private_object dialog,
		    OM_object arguments,
		    OM_uint32 invoke_id)
{
}

TC_status
__xtc_tc_error_req(
		   OM_private_object dialog,
		   OM_object arguments,
		   OM_uint32 invoke_id)
{
}

TC_status
__xtc_tc_cancel_req(
		    OM_private_object dialog,
		    OM_object arguments
		    OM_uint32 invoke_id)
{
}

TC_status
__xtc_tc_reject_req(
		    OM_private_object dialog,
		    OM_object arguments,
		    OM_uint32 invoke_id)
{
}

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
