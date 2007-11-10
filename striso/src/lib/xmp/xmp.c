/*****************************************************************************

 @(#) $RCSfile: xmp.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/11/10 19:41:07 $

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

 Last Modified $Date: 2007/11/10 19:41:07 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmp.c,v $
 Revision 0.9.2.1  2007/11/10 19:41:07  brian
 - documentation updates

 *****************************************************************************/

#ident "@(#) $RCSfile: xmp.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/11/10 19:41:07 $"

static char const ident[] = "$RCSfile: xmp.c,v $ $Name:  $($Revision: 0.9.2.1 $) $Date: 2007/11/10 19:41:07 $";

#include <xom.h>
#include <xmp.h>

MP_status
mp_abandon(OM_private_object session, OM_sint32 invoke_id)
{
}

__asm__(".symver __xmp_mp_abandon,mp_abandon@@XMP_1.0");

MP_status
mp_abort_req(OM_private_object session, OM_private_object context, OM_object argument)
{
}

__asm__(".symver __xmp_mp_abort_req,mp_abort_req@@XMP_1.0");

MP_status
mp_action_req(OM_private_object session, OM_private_object context,
	      OM_object argument, OM_private_object * result_return, OM_sint32 * invoke_id_return)
{
}

__asm__(".symver __xmp_mp_action_req,mp_action_req@@XMP_1.0");

MP_status
mp_action_rsp(OM_private_object session, OM_private_object context,
	      OM_object response, OM_sint32 invoke_id)
{
}

__asm__(".symver __xmp_mp_action_rsp,mp_action_rsp@@XMP_1.0");

MP_status
mp_assoc_req(OM_private_object session, OM_private_object context,
	     OM_object argument, OM_private_object * result_return, OM_sint32 * invoke_id_return)
{
}

__asm__(".symver __xmp_mp_assoc_req,mp_assoc_req@@XMP_1.0");

MP_status
mp_assoc_rsp(OM_private_object session, OM_private_object context,
	     OM_object response, OM_sint32 invoke_id)
{
}

__asm__(".symver __xmp_mp_assoc_rsp,mp_assoc_rsp@@XMP_1.0");

MP_status
mp_bind(OM_object session, OM_workspace workspace, OM_private_object * bound_session_return)
{
}

__asm__(".symver __xmp_mp_bind,mp_bind@@XMP_1.0");

MP_status
mp_cancel_get_req(OM_private_object session, OM_private_object context,
		  OM_object argument, OM_sint32 * invoke_id_return)
{
}

__asm__(".symver __xmp_mp_cancel_get_req,mp_cancel_get_req@@XMP_1.0");

MP_status
mp_cancel_get_rsp(OM_private_object session, OM_private_object context,
		  OM_object response, OM_sint32 invoke_id)
{
}

__asm__(".symver __xmp_mp_cancel_get_rsp,mp_cancel_get_rsp@@XMP_1.0");

MP_status
mp_create_req(OM_private_object session, OM_private_object context,
	      OM_object argument, OM_private_object * result_return, OM_sint32 * invoke_id_return)
{
}

__asm__(".symver __xmp_mp_create_req,mp_create_req@@XMP_1.0");

MP_status
mp_create_rsp(OM_private_object session, OM_private_object context,
	      OM_object response, OM_sint32 invoke_id)
{
}

__asm__(".symver __xmp_mp_create_rsp,mp_create_rsp@@XMP_1.0");

MP_status
mp_delete_req(OM_private_object session, OM_private_object context,
	      OM_object argument, OM_private_objt * result_return, OM_sint32 * invoke_id_return)
{
}

__asm__(".symver __xmp_mp_delete_req,mp_delete_req@@XMP_1.0");

MP_status
mp_delete_rsp(OM_private_object session, OM_private_object context,
	      OM_object response, OM_sint32 invoke_id)
{
}

__asm__(".symver __xmp_mp_delete_rsp,mp_delete_rsp@@XMP_1.0");

OM_sint
mp_error_message(MP_status error, OM_sint length, unsigned char *error_text_return)
{
}

__asm__(".symver __xmp_mp_error_message,mp_error_message@@XMP_1.0");

MP_status
mp_event_report_req(OM_private_object session, OM_private_object context,
		    OM_object argument, OM_private_object * result_return,
		    OM_sint32 * invoke_id_return)
{
}

__asm__(".symver __xmp_mp_event_report_req,mp_event_report_req@@XMP_1.0");

MP_status
mp_event_report_rsp(OM_private_object session, OM_private_object context,
		    OM_object response, OM_sint32 invoke_id)
{
}

__asm__(".symver __xmp_mp_event_report_rsp,mp_event_report_rsp@@XMP_1.0");

MP_status
mp_get_assoc_info(OM_private_object receive_result_or_argument,
		  OM_uint request_mask, OM_uint result_mask,
		  OM_public_object * pres_layer_args, OM_public_object * acse_args,
		  OM_public_object * cmip_assoc_args, OM_public_object * standard_externals)
{
}

__asm__(".symver __xmp_mp_get_assoc_info,mp_get_assoc_info@@XMP_1.0");

MP_status
mp_get_last_error(OM_workspace workspace, OM_uint32 * additional_error_return)
{
}

__asm__(".symver __xmp_mp_get_last_error,mp_get_last_error@@XMP_1.0");

MP_status
mp_get_next_req(OM_private_object session, OM_private_object context,
		OM_object argument, OM_private_object * result_return, OM_sint32 * invoke_id_return)
{
}

__asm__(".symver __xmp_mp_get_next_req,mp_get_next_req@@XMP_1.0");

MP_status
mp_get_req(OM_private_object session, OM_private_object context,
	   OM_object argument, OM_private_object * result_return, OM_sint32 * invoke_id_return)
{
}

__asm__(".symver __xmp_mp_get_req,mp_get_req@@XMP_1.0");

MP_status
mp_get_rsp(OM_private_object session, OM_private_object context,
	   OM_object response, OM_sint32 invoke_id)
{
}

__asm__(".symver __xmp_mp_get_rsp,mp_get_rsp@@XMP_1.0");

OM_workspace
mp_initialize(void)
{
}

__asm__(".symver __xmp_mp_initialize,mp_initialize@@XMP_1.0");

MP_status
mp_negotiate(MP_feature feature_list[], OM_workspace workspace)
{
}

__asm__(".symver __xmp_mp_negotiate,mp_negotiate@@XMP_1.0");

MP_status
mp_receive(OM_private_object session, OM_private_object context,
	   OM_sint * primitive_return, OM_sint * completion_flag_return,
	   MP_status * operation_notification_status_return,
	   OM_private_object * result_or_argument_return, OM_sint32 * invoke_id_return)
{
}

__asm__(".symver __xmp_mp_receive,mp_receive@@XMP_1.0");

MP_status
mp_release_req(OM_private_object session, OM_private_object context,
	       OM_object argument, OM_private_object * result_return, OM_sint32 * invoke_id_return)
{
}

__asm__(".symver __xmp_mp_release_req,mp_release_req@@XMP_1.0");

MP_status
mp_release_rsp(OM_private_object session, OM_private_object context,
	       OM_object response, OM_sint32 invoke_id)
{
}

__asm__(".symver __xmp_mp_release_rsp,mp_release_rsp@@XMP_1.0");

MP_status
mp_set_req(OM_private_object session, OM_private_object context,
	   OM_object argument, OM_private_object * result_return, OM_sint32 * invoke_id_return)
{
}

__asm__(".symver __xmp_mp_set_req,mp_set_req@@XMP_1.0");

MP_status
mp_set_rsp(OM_private_object session, OM_private_object context,
	   OM_object response, OM_sint32 invoke_id)
{
}

__asm__(".symver __xmp_mp_set_rsp,mp_set_rsp@@XMP_1.0");

void
mp_shutdown(OM_workspace workspace)
{
}

__asm__(".symver __xmp_mp_shutdown,mp_shutdown@@XMP_1.0");

MP_status
mp_unbind(OM_private_object session)
{
}

__asm__(".symver __xmp_mp_unbind,mp_unbind@@XMP_1.0");

MP_status
mp_validate_object(OM_workspace workspace, OM_object test_object,
		   OM_private_object * bad_argument_return)
{
}

__asm__(".symver __xmp_mp_validate_object,mp_validate_object@@XMP_1.0");

MP_status
mp_wait(MP_waiting_sessions bound_session_list[], OM_workspace workspace, OM_uint32 timeout)
{
}

__asm__(".symver __xmp_mp_wait,mp_wait@@XMP_1.0");

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
