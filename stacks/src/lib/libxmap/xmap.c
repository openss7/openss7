/*****************************************************************************

 @(#) $RCSfile: xmap.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2009-03-12 15:08:34 $

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
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

 Last Modified $Date: 2009-03-12 15:08:34 $ by $Author: brian $

 -----------------------------------------------------------------------------

 $Log: xmap.c,v $
 Revision 0.9.2.2  2009-03-12 15:08:34  brian
 - map library doc and impl

 Revision 0.9.2.1  2009-03-05 15:51:27  brian
 - new files for map library

 *****************************************************************************/

#ident "@(#) $RCSfile: xmap.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2009-03-12 15:08:34 $"

static char const ident[] = "$RCSfile: xmap.c,v $ $Name:  $($Revision: 0.9.2.2 $) $Date: 2009-03-12 15:08:34 $";

#include <xom.h>
#include <xomi.h>
#include <xmap.h>

#define OMP_O_MAP_DIALOGUE_AS \
	mapP_asId(\x01\x01) /* 0.4.0.0.1.1.1.1 */
	/* { itu-t(0) identified-organization(4) etsi(0) mobile-domain(0) * gsm-Network(1) as-Id(1) map-DialoguePDU(1) version1(1) } */

static OM_return_code
__xmap_om_copy(OM_private_object original, OM_workspace workspace,
	       OM_private_object * copy)
{
}

static OM_return_code
__xmap_om_copy_value(OM_private_object source, OM_type source_type,
		     OM_value_position source_value_position,
		     OM_private_object destination, OM_type destination_type,
		     OM_value_position destination_value_position)
{
}

static OM_return_code
__xmap_om_create(OM_object_identifier class, OM_boolean initialise,
		 OM_workspace workspace, OM_private_object * object)
{
}

static OM_return_code
__xmap_om_decode(OM_private_object encoding, OM_private_object * original)
{
}

static OM_return_code
__xmap_om_delete(OM_object subject)
{
}

static OM_return_code
__xmap_om_encode(OM_private_object original, OM_object_identifier rules,
		 OM_private_object * encoding)
{
}

static OM_return_code
__xmap_om_get(OM_private_object original, OM_exclusions exclusions,
	      OM_type_list included_types, OM_boolean local_strings,
	      OM_value_position initial_value, OM_value_position limiting_value,
	      OM_public_object * copy, OM_value_position * total_number)
{
}

static OM_return_code
__xmap_om_instance(OM_object subject, OM_object_identifier class,
		   OM_boolean * instance)
{
}

static OM_return_code
__xmap_om_put(OM_private_object destination, OM_modification modification,
	      OM_object source, OM_type_list included_types,
	      OM_value_position initial_value, OM_value_position limiting_value)
{
}

static OM_return_code
__xmap_om_read(OM_private_object subject, OM_type type,
	       OM_value_position value_position, OM_boolean local_string,
	       OM_string_length * string_offset, OM_string * elements)
{
}

static OM_return_code
__xmap_om_remove(OM_private_object subject, OM_type type,
		 OM_value_position initial_value,
		 OM_value_position limiting_value)
{
}

static OM_return_code
__xmap_om_write(OM_private_object subject, OM_type type,
		OM_value_position value_position, OM_syntax syntax,
		OM_string_length * string_offset, OM_string elements)
{
}

static struct OMP_functions_body __xmap_om_functions[] = {
	{
	 1,
	 &__xmap_om_copy,
	 &__xmap_om_copy_value,
	 &__xmap_om_create,
	 &__xmap_om_decode,
	 &__xmap_om_delete,
	 &__xmap_om_encode,
	 &__xmap_om_get,
	 &__xmap_om_instance,
	 &__xmap_om_put,
	 &__xmap_om_read,
	 &__xmap_om_remove,
	 &__xmap_om_write},
	{
	 0,
	 }
};

static struct __xmap_comp;
static struct __xmap_assoc;
static struct __xmap_session;
static struct __xmap_workspace;

static struct __xmap_comp {
	OM_descriptor s_desc[2];
	struct __xmap_comp *c_next;
	struct __xmap_comp **c_prev;
	OM_uint32 c_invoke_id;
};

static struct __xmap_assoc {
	OM_descriptor a_desc[2];	/* private object descriptor for assoc */
	struct __xmap_session *a_parent;
	struct __xmap_assoc *a_next;
	struct __xmap_assoc **a_prev;
	OM_descriptor a_comps_desc[2];	/* private object descriptor for list */
	struct __xmap_comp *a_comps;
	OM_uint32 a_assoc_id;
};

static struct __xmap_session {
	OM_descriptor s_desc[2];	/* private object descriptor for session */
	struct __xmap_workspace *s_parent;
	struct __xmap_session *s_next;
	struct __xmap_session **s_prev;
	OM_descriptor s_assocs_desc[2];	/* private object descriptor for list */
	struct __xmap_assoc *s_assocs;
	OM_sint file_descriptor;
};

static struct __xmap_workspace {
	struct OMP_workspace_body body;
	/* additional private structures */
	struct __xmap_workspace *ws_next;
	struct __xmap_workspace **ws_prev;
	MAP_status ws_last_error;
	OM_descriptor ws_sesions_desc[2];	/* private object descriptor for list */
	struct __xmap_session *ws_sessions;
	/* more... */
	 MAP_feature_list[xx];
};

static MAP_status
__xmap_validate_workspace(OM_workspace test_workspace,
			  struct __xmap_workspace **wsp)
{
	struct __xmap_worspace *workspace = (typeof(workspace)) test_workspace;
	struct __xmap_workspace *ws;

	/* do not expect a large number of workspaces */
	for (ws = __xmap_workspaces; ws; ws = ws->ws_next)
		if (ws == workspace) {
			if (wsp != NULL)
				*wsp = ws;
			return (MAP_SUCCESS);
		}
	return (MAP_NO_WORKSPACE);
}

static MAP_status
__xmap_validate_session(OM_private_object session, struct __xmap_session **sp)
{
	struct __xmap_session *s, *s_test;
	struct __xmap_workspace *ws;
	OM_workspace workspace;

	if (session == NULL)
		return (MAP_INVALID_SESSION);
	if (!OM_IS_PRIVATE(*session))
		return (MAP_INVALID_SESSION);
	workspace = OMP_WORKSPACE(session);
	if (__xmap_validate_workspace(workspace, &ws))
		return (MAP_INVALID_SESSION);
	s_test = session->value.string.elements;
	/* probably only a handful of sessions */
	for (s = ws->ws_sessions; s; s = s->s_next) {
		if (s == s_test) {
			if (sp != NULL)
				*sp = s;
			return (MAP_SUCCESS);
		}
	}
	return (MAP_INVALID_SESSION);
}

static MAP_status
__xmap_validate_assoc(OM_private_object assoc, struct __xmap_assoc **ap)
{
	struct __xmap_assoc *a, *a_test;
	struct __xmap_session *s;
	struct __xmap_workspace *ws;
	OM_workspace workspace;
	MAP_status status;

	if (assoc == NULL)
		return (MAP_INVALID_ASSOCIATION);
	if (!OM_IS_PRIVATE(*assoc))
		return (MAP_INVALID_ASSOCIATION);
	workspace = OMP_WORKSPACE(assoc);
	if (__xmap_validate_workspace(workspace, &ws))
		return (MAP_INVALID_ASSOCIATION);
	a_test = assoc->value.string.elements;
	/* probably only a handful of sessions */
	for (s = ws->ws_sessions; s; s = s->s_next) {
		/* this is bad, probably may associations (open trasnasctions) */
		/* need a hash for associations based on dialog_id */
		for (a = ss->s_assocs; a; a = a->a_next) {
			if (a == a_test) {
				if (ap != NULL)
					*ap = a;
				return (MAP_SUCCESS);
			}
		}
	}
	return (MAP_INVALID_ASSOCIATION);
}

static struct __xmap_workspace *__xmap_workspaces = NULL;
static int __xmap_numb_workspaces = 0;

/* OM workspace functions */

/** @brief intialize a new MAP OM workspace
  * @par
  *
  * Initializes a new MAP OM workspace and returns a pointer to the MAP OMP
  * workspace definition.  The MAP OMP workspace consists of all of the
  * information necessary to support a group of MAP User entities.
  *
  * This function performs any necessary intialization of the interface and
  * allocates a workspace.  It must be called before any other management
  * interface functions are called.  It may be called multiple times, in which
  * case each call returns a workspace that is distinct from other workspaces
  * created by Initialize() but not yet deleted by Shutdown().
  *
  * @par Results:
  *	Workspace(Workspace).  Upon successful completion, contains a handle to
  *	a workspace in which OM objects can be created and manipulated.  Object
  *	created in this worksapce, and only such objects, may be used as
  *	arguments to the other management interface functions.  This function
  *	returns NULL if it faile.
  */
OM_workspace
__xmap_map_initialize(void)
{
	struct __xmap_workspace *ws;

	/* allocate a new private workspace structure */
	if ((ws = malloc(sizeof(*ws))) == NULL)
		return (MAP_INSUFFICIENT_RESOURCES);
	memset(wq, 0, sizeof(*wq));
	/* initialize workspace */
	wq->body.functions = __xmap_om_functions;
	/* FIXME: complete other information */
	/* install new workspace in list of workspaces */
	if ((ws->ws_next = __xmap_workspaces))
		ws->ws_next->ws_prev = &ws->ws_next;
	ws->ws_prev = &__xmap_workspaces;
	/* return pointer to public part of workspace */
	return &workspace->body;
}

/** @fn OM_workspace map_initialize(void)
  */
__asm__(".symver __xmap_map_initialize,map_initialize@@XMAP_1.0");

/** @brief shutdown a MAP OM workspace
  * @param workspace workspace to shut down
  *
  * Shuts down a MAP OMP workspace.  This consists of aborting all open
  * dialogues, unbinding and closing all MAP User entities, and freeing the
  * resources associated with the workspace.
  */
void
__xmap_map_shutdown(OM_workspace workspace)
{
	MAP_status status;
	struct __xmap_workspace *ws;

	/* validate workspace */
	if ((status = __xmap_validate_workspace(workspace, &ws)))
		return (status);
	/* destroy sessions */
	while (ws->ws_sessions != NULL)
		__xmap_map_unbind((OM_private_object) ws->ws_sessions);
	/* remove from list */
	if ((*ws->ws_prev = ws->ws_next))
		ws->ws_next->ws_prev = ws->ws_prev;
	ws->ws_next = NULL;
	ws->ws_prev = &ws->ws_next;
	/* deallocate structure */
	free(ws);
}

/** @fn void map_shutdown(OM_workspace workspace)
  * @param workspace
  */
__asm__(".symver __xmap_map_shutdown,map_shutdown@@XMAP_1.0");

/** @brief validate a MAP OMP workspace object
  * @param workspace MAP OMP workspace containing object
  * @param test_object MAP OMP workspace object to test
  * @param bad_argument_return Argument in error
  *
  * Tests the supplied @test_object for validity and returns the argument in
  * error if there is an error.
  */
MAP_status
__xmap_map_validate_object(OM_workspace workspace, OM_object test_object,
			   OM_private_object * bad_argument_return)
{
	MAP_status status;
	struct __xmap_workspace *ws;

	/* check validity of workspace */
	if ((status = __xmap_validate_workspace(workspace, &ws)))
		return (status);
	/* check validity of test object */
	/* return bad argument return and status */
}

/** @fn MAP_status map_validate_object(OM_workspace workspace, OM_object test_object, OM_private_object * bad_argument_return)
  * @param workspace
  * @param test_object
  * @param bad_argument_return
  */
__asm__(".symver __xmap_map_validate_object,map_validate_object@@XMAP_1.0");

/** @brief get additional error return
  * @param workspace MAP OMP workspace
  * @param additional_error_return additional retunred error
  */
MAP_status
__xmap_map_get_last_error(OM_workspace workspace,
			  OM_uint32 * additional_error_return)
{
	MAP_status status;
	struct __xmap_workspace *ws;

	/* check workspace validity */
	if ((status = __xmap_validate_workspace(workspace, &ws)))
		return (status);
	/* obtain last error from workspace private structure */
	status = ws->ws_last_error;
	/* return additional error */
	if (additional_error_return != NULL)
		*additional_error_return = status;
	/* return status */
	return (OM_SUCCESS);
}

/** @fn MAP_status map_get_last_error(OM_workspace workspace, OM_uint32 * additional_error_return)
  * @param worksapce
  * @param additional_error_return
  */
__asm__(".symver __xmap_map_get_last_error,map_get_last_error@@XMAP_1.0");

/** @brief negotiate workspace features
  * @param feature_list list of MAP OMP workspace features to negotiate
  * @param workspace MAP OMP workspace
  *
  * Negotiates MAP OMP workspace features.
  */
MAP_status
__xmap_map_negotiate(MAP_feature_list feature_list[], OM_workspace workspace)
{
	MAP_status status;
	struct __xmap_workspace *ws;

	/* check workspace validity */
	if ((status = __xmap_validate_workspace(workspace, &ws)))
		return (status);
	/* check feature list validity */
	/* negotate features from feature list */
	/* return status */
}

/** @fn MAP_status map_negotiate(MAP_feature_list feature_list[], OM_workspace workspace)
  * @param feature_list
  * @param workspace
  */
__asm__(".symver __xmap_map_negotiate,map_negotiate@@XMAP_1.0");

/** @brief poll for received MAP messages
  * @param bound_sessions_list list of bound sessions to poll
  * @param workspace MAP OMP workspace
  * @param timeout number of milliseconds to wait
  *
  * A value of zero for timeout means to not wait but immediately return the
  * status of all of the bound sessions.  A value of minus one (-1) specifies
  * to wait indefinitely for some message.  The activated member of each of the
  * sessions in the bound session list is set to indicate whether messages are
  * ready and can be obtained using the map_receive() function.  Note that in a
  * multithreaded environments, another thread may have already called
  * map_receive() and consumed the message before the current thread can call
  * map_receive().
  */
MAP_status
__xmap_map_wait(MAP_waiting_sessions bound_sessions_list[],
		OM_workspace workspace, OM_sint32 timeout)
{
	MAP_status status;
	struct __xmap_workspace *ws;

	/* check workspace validity */
	if ((status = __xmap_validate_workspace(workspace, &ws)))
		return (status);
	/* check bound sessions for validity */
	/* assemble list of file descriptors */
	/* call system poll function */
	/* use return from system poll function to set activated flags */
	/* return status */
}

/** @fn MAP_status map_wait(MAP_waiting_sessions bound_sessions_list[], OM_workspace workspace, OM_sint32 timeout)
  * @param bound_sessions_list
  * @param workspace
  * @param timeout
  */
__asm__(".symver __xmap_map_wait,map_wait@@XMAP_1.0");

/** @brief receive a MAP notification
  * @param session bound session on which to receive
  * @param primitive_return  type of event received
  * @param assoc_return association received or NULL
  * @param operation_notification_status_return status of asyncrhonous event
  * @param result_or_argument_return result or arguments object associated with event
  *
  * When the primitive is MAP_OPEN_IND an association is returned and the
  * arguments are of class MAP_C_OPEN_ARGS.
  *
  * When the primitive is MAP_ACCEPT_IND an association is returned and the
  * arguments are of class MAP_C_ACCEPT_ARGS.
  *
  * When the primitive is MAP_REFUSE_IND an assocation is returned and the
  * arguments are of class MAP_C_REFUSE_ARGS.
  *
  * When the primitive is MAP_ISSUE_IND an association is returned and the
  * arguments are of class MAP_C_ISSUE_ARGS.
  *
  * When the primitive is MAP_ABORT_IND an association is returned and the
  * arguments are of class MAP_C_ABORT_ARGS.
  *
  * When the primitive is MAP_NOTICE_IND an association is returned and
  * the arguments are of class MAP_C_NOTICE_ARGS.
  */
MAP_status
__xmap_map_receive(OM_private_object session, OM_sint * primitive_return,
		   OM_private_object * assoc_return,
		   TC_status * operation_notification_status_return,
		   OM_private_object * result_or_argument_return)
{
	MAP_status status;
	struct __xmap_session *s;

	/* check session for validity */
	if ((status = __xmap_validate_session(session, &s)))
		return (status);
	/* obtain file descriptor */
	/* poll file descriptor */
	/* read TC primitive */
	/* translate primitive into operation result or arguments */
	/* create association object if TC-BEGIN indication or identify existing association if
	   TC-CONTINUE, TC-END, TC-ABORT indication, */
	/* return status */
}

/** @fn MAP_status map_receive(OM_private_object session, OM_sint * primitive_return, OM_private_object * assoc_return, TC_status * operation_notification_status_return, OM_private_object * result_or_argument_return)
  * @param session
  * @param primitive_return
  * @param assoc_return
  * @param operation_notification_status_return
  * @param result_or_argument_return
  */
__asm__(".symver __xmap_map_receive,map_receive@@XMAP_1.0");

/* MAP session function */

/** @brief open and bind a MAP session
  * @param session object describing MAP session including address to bind
  * @param workspace initialized MAP OMP workspace
  * @param bound_session_return opened and bound session
  *
  * This function accepts a session specification and an initialized MAP OMP
  * workspace and opens and binds a MAP user as specified.  When successful, a
  * bound session object is returned that may be used in conjunction with other
  * MAP functions.  Note that each bound MAP session consumes a file descriotr.
  * Also, the bound MAP session has additional information associated with it.
  *
  * Attributes in the supplied session object supply information about the
  * association.  Attributes include:
  *
  * Responding-Address, Mode, Application-Context-Name-List.
  *
  * When a Responding-Address is provided, the resulting session will be given
  * MAP-OPEN indication primitives that correspond to the Responding-Address.
  * If the Responding-Address is a wildcard address, the session will be given
  * MAP-OPEN indication primitives that do not correspond to any other session
  * bound to a Responding-Address.
  *
  * Mode specifies whether the default mode is synchronous or asynchronous.
  *
  * When an Application-Context-Name-List is provided in conjunction with a
  * Responding-Address, only MAP-OPEN indication primitives for one of the
  * listed application context names will be delivered to this session.
  *
  * This function performs part of the operations of the MAP-OPEN request
  * primitive of GSM TS 29.002.  That is, it peforms a binding of the
  * SCCP-SAP portion of the originating address.
  */
MAP_status
__xmap_map_bind(OM_object old_session, OM_workspace workspace,
		OM_private_object * bound_session_return)
{
	MAP_status status;
	struct __xmap_workspace *ws;
	struct __xmap_session *s;
	OM_private_object session;

	/* validate workspace */
	if ((status = __xmap_validate_workspace(workspace, &ws)))
		return (status);
	/* check if old_session is usable */
	if (old_session == MAP_DEFAULT_SESSION || !OM_IS_PRIVATE(*old_session)) {
		/* need to create new private session object */
		/* create session object in workspace */
		if ((s == malloc(sizeof(s))) == NULL)
			return (MAP_INSUFFICIENT_RESOURCES);
		memset(s, 0, sizeof(*s));
		session = OMP_EXTERNAL(s);
		session[-1].type = MAP_C_SESSION;
		session[-1].syntax =
		    OM_S_PRIVATE | OM_S_SERVICE_GENERATED | OM_S_OBJECT;
		session[-1].value.string.elements = workspace;
		session[0].type = OM_PRIVATE_OBJECT;
		session[0].syntax =
		    OM_S_PRIVATE | OM_S_SERVICE_GENERATED | OM_S_OBJECT;
		session[0].value.string.elements = s;
		if (old_session == MAP_DEFAULT_SESSION) {
			/* use defaults */
		} else {
			/* use provided values */
		}
		/* bind session to workspace */
		if ((s->s_next = ws->ws_sessions))
			s->s_next->s_prev = &s->s_next;
		s->s_prev = &ws->ws_sessions;
		ws->ws_sessions = s;
	} else {
		/* it is a private object, check for validity */
		if ((status = __xmap_validate_session(old_session, &s)))
			return (status);
		/* check the state of the session */
		if (s->s_fd != -1)
			return (MAP_INVALID_SESSION);
		/* ok, just make a new file descriptor */
		/* move session from existing workspace to new workspace */
		session = OMP_EXTERNAL(s);
	}
	/* check session for valid attributes */
	/* open file descriptor (i.e. open TC driver stream) */
	/* bind Stream to provided address TC_BIND_REQ/TC_BIND_ACK */
	/* close file descriptor and destroy session on error */
	/* return bound session object on success */
	if (bound_session_return != NULL)
		*bound_session_return = session;
	/* return status */
	return (MAP_SUCCESS);
}

/** @fn MAP_status map_bind(OM_object old_session, OM_workspace workspace, OM_private_object * bound_session_return)
  * @param old_session
  * @param workspace
  * @param bound_session_return
  */
__asm__(".symver __xmap_map_bind,map_bind@@XMAP_1.0");

/** @brief unbind and close a MAP session
  * @param session bound session object to close
  *
  * Unbinds and closes a MAP user session.  Upon success, the passed session
  * object is invalidated and no longer available for use by the XMAP interface
  * user.
  */
MAP_status
__xmap_map_unbind(OM_private_object session)
{
	MAP_status status;
	struct __xmap_session *s;

	/* check session for validity */
	if ((status = __xmap_validate_session(session, &s)))
		return (status);
	/* unbind Stream TC_UNBIND_REQ/TC_OK_ACK */
	/* close Stream */
	/* destroy session object */
	/* return status */
	return (MAP_SUCCESS);
}

/** @fn MAP_status map_unbind(OM_private_object session)
  * @param session
  */
__asm__(".symver __xmap_map_unbind,map_unbind@@XMAP_1.0");

/* MAP association functions */

/** @brief opens a new association
  * @param session MAP OMP session within which to open association
  * @param assoc template association
  * @param arguments arguments of the association
  * @param extension extension data for association
  * @param assoc_return open association
  *
  * Opens a new associated and, when successful, returns an private object
  * representing the open association.  This private association object may be
  * used in other MAP association and service functions.
  *
  * When the provided template object is a private object from the appropriate
  * workspace that has previously been closed or aborted, it is opened and
  * returned.  When the provided template object is a public object, a new
  * private object is created and populated with the values from the public
  * object.
  *
  * Arguments consist of arguments to the MAP-OPEN request of GSM TS 29.002, a
  * mandatory application context, a mandatory destination address, a user
  * optional destination reference, a user optional originating address, a user
  * optional originating reference.  This an object of class MAP_C_OPEN_ARGS.
  *
  * Extension consists of a user optional specific-information encoding.
  *
  * This function corresponds to the MAP-OPEN request of GSM TS 29.002.  This
  * function ultimately maps to the TC-BEGIN request primitive of ITU-T Rec.
  * Q.771 once pending service requests and responses are issued.
  */
MAP_status
__xmap_map_open(OM_private_object session, OM_object assoc, OM_object arguments,
		OM_object extension, OM_private_object * assoc_return)
{
	MAP_status status;
	struct __xmap_session *s;
	struct __xmap_assoc *a;
	OM_private_object assoc;

	/* check session for validity */
	if ((status = __xmap_validate_session(session, &s)))
		return (status);
	/* check arguments for validity */
	/* check extension for validity */
	/* create association object */
	if ((a = malloc(sizeof(*a))) == NULL)
		return (MAP_INSUFFICIENT_RESOURCES);
	memset(a, 0, sizeof(*a));
	assoc = OMP_EXTERNAL(a);
	assoc[-1].type = MAP_C_ASSOCIATION;
	assoc[-1].syntax = OM_S_PRIVATE | OM_S_SERVICE_GENERATED | OM_S_OBJECT;
	assoc[-1].value.string.elements = OMP_WORKSPACE(session);
	assoc[0].type = OM_PRIVATE_OBJECT;
	assoc[0].syntax = OM_S_PRIVATE | OM_S_SERVICE_GENERATED | OM_S_OBJECT;
	assoc[0].value.string.elements = a;
	/* bind association object to session */
	if ((a->a_next = s->s_assocs))
		a->a_next->a_prev = &a->a_next;
	a->a_prev = &s->s_assocs;
	s->s_assocs = a;
	/* return created association object */
	if (assoc_return != NULL)
		*assoc_return = assoc;
	/* return status */
	return (MAP_SUCCESS);
}

/** @fn MAP_status map_open(OM_private_object session, OM_object assoc, OM_object arguments, OM_object extension, OM_private_object * assoc_return)
  * @param session
  * @param assoc
  * @param arguments
  * @param extension
  * @param assoc_return
  */
__asm__(".symver __xmap_map_open,map_open@@XMAP_1.0");

/** @brief accept an incoming association
  * @param assoc incoming association to accept
  * @param arguments arguments of the accept
  * 
  * Accepts an incoming association with arguments.  The incoming association
  * object must have been returned from the map_receive() function and not
  * previously accepted, refused, closed or aborted.
  *
  * Arguments consist of arguments to the MAP-OPEN response of GSM TS 29.002, a
  * user optional application context, a user optional responding address, and
  * user optional specific information.  This is an object of class
  * MAP_C_ACCEPT_ARGS.
  *
  * This function corresponds to the MAP-OPEN response (accept) of GSM TS
  * 29.002.  This function ultimately maps to the TC-BEGIN response primitive of
  * ITU-T Rec. Q.771 once pending service requests and responses are issued.
  */
MAP_status
__xmap_map_accept(OM_private_object assoc, OM_object arguments,
		  OM_object extension)
{
	MAP_status status;
	struct __xmap_assoc *a;

	/* check association object for validity */
	if ((status = __xmap_validate_assoc(assoc, &a)))
		return (status);
	/* check arguments for validity */
	/* mark responding arguments in association object */
	/* return status */
}

/** @fn MAP_status map_accept(OM_private_object assoc, OM_object arguments, OM_object extension)
  * @param assoc
  * @param arguments
  * @param extension
  */
__asm__(".symver __xmap_map_accept,map_accept@@XMAP_1.0");

/** @brief refuse an incoming association
  * @param assoc incoming association to refuse
  * @param arguments arguments of the refusal
  *
  * Refuses an incoming association with arguments.  The incoming association
  * object must have been returned from the map_receive() functio and not
  * previously accepted, refused, closed or aborted.
  *
  * Arguments consist of arguments to the MAP-OPEN response of GSM TS 29.002, a
  * user optional application context, a user optional responding address,
  * mandatory refuse reason and user optional specific information.  This is an
  * object of class MAP_C_REFUSE_ARGS.
  *
  * This function corresponds to the MAP-OPEN response (refuse) of GSM TS
  * 29.002.  This function ultimately maps to the TC-END request primitve of
  * ITU-T Rec. Q.771 once pending service requets and respones are issued.
  */
MAP_status
__xmap_map_refuse(OM_private_object assoc, OM_object arguments,
		  OM_object extension)
{
	MAP_status status;
	struct __xmap_assoc *a;

	/* check association object for validity */
	if ((status = __xmap_validate_assoc(assoc, &a)))
		return (status);
	/* check association object and arguments for validity */
	/* mark refusal arguments in association object */
	/* return status */
}

/** @fn MAP_status map_refuse(OM_private_object assoc, OM_object arguments, OM_object extension)
  * @param assoc
  * @param arguments
  * @param extension
  */
__asm__(".symver __xmap_map_refuse,map_refuse@@XMAP_1.0");

/** @brief issue pending service requests and responses
  * @param assoc established association
  * @param arguments of the issue
  * @param result_return results of operations (syncrhonous mode)
  *
  * Issues pending service requests and responses for an association.
  *
  * Arguments consist of a user optional mode.  Arguments are of type
  * MAP_C_ISSUE_ARGS.
  *
  * This function corresponds to the MAP-DELIMIT request primitive of GSM TS
  * 29.002.  This function maps to the TC-BEGIN or TC-CONTINUE request
  * primitives of ITU-T Rec. Q.771.
  */
MAP_status
__xmap_map_issue(OM_private_object assoc, OM_object arguments,
		 OM_private_object * result_return)
{
	MAP_status status;
	struct __xmap_assoc *a;

	/* check association object for validity */
	if ((status = __xmap_validate_assoc(assoc, &a)))
		return (status);
	/* check validty of association object and arguments */
	/* generate TC-BEGIN or TC-CONTINUE request primtive */
	/* in synchronous mode await the result */
	/* return status */
}

/** @fn MAP_status map_issue(OM_private_object assoc, OM_object arguments, OM_private_object * result_return)
  * @param assoc
  * @param arguments
  * @param result_return
  */
__asm__(".symver __xmap_map_issue,map_issue@@XMAP_1.0");

/** @brief close an association
  * @param assoc association to close
  * @param arguments arguments of the close
  *
  * Issues pending service requests and responses for an association and
  * closes the association.
  *
  * Arguments consist of a mandatory release method and user optional specific
  * information.  Arguments are of type MAP_C_CLOSE_ARGS.
  *
  * This function corresponds to the MAP-CLOSE request primitive of GSM TS
  * 29.002.  This function maps to the TC-END request primitive of ITU-T Rec.
  * Q.771.
  */
MAP_status
__xmap_map_close(OM_private_object assoc, OM_object arguments,
		 OM_object extension)
{
	MAP_status status;
	struct __xmap_assoc *a;

	/* check association object for validity */
	if ((status = __xmap_validate_assoc(assoc, &a)))
		return (status);
	/* check validity of association object and arguments */
	/* generate TC-END request primitive with arguments */
	/* destroy association object */
	/* return status */
}

/** @fn MAP_status map_close(OM_private_object assoc, OM_object arguments, OM_object extension)
  * @param assoc
  * @param arguments
  * @param extension
  */
__asm__(".symver __xmap_map_close,map_close@@XMAP_1.0");

/** @brief abort an association
  * @param assoc association to abort
  * @param arguments arguments of the abort
  *
  * Clears pending service requests and responses for an association an aborts
  * the association.
  * 
  * Arguments consist of a mandatory user reason and a user optional diagnostic
  * information.  Arguments are of type MAP_C_ABORT_ARGS.
  *
  * This function corresponds to the  MAP-ABORT requst primtive of GSM TS
  * 29.002.  This function maps to the TC-ABORT request primitive of ITU-T Rec.
  * Q.771.
  */
MAP_status
__xmap_map_abort(OM_private_object assoc, OM_object arguments,
		 OM_object extension)
{
	MAP_status status;
	struct __xmap_assoc *a;

	/* check association object for validity */
	if ((status = __xmap_validate_assoc(assoc, &a)))
		return (status);
	/* check validity of association object and arguments */
	/* generate TC-ABORT request primitive with arguments */
	/* destroy association object */
	/* return status */
}

/** @fn MAP_status map_abort(OM_private_object assoc, OM_object arguments, OM_object extension)
  * @param assoc
  * @param arguments
  * @param extension
  */
__asm__(".symver __xmap_map_abort,map_abort@@XMAP_1.0");

/* MAP service functions */

/** @brief receive MAP service indications and confirmations
  * @param assoc MAP association
  * @param primitive_return received indication or confirmation primitive
  * @param arguments_return received primitive arguments
  * @param component_return received service component
  * @param invoke_id_return invoke id associated with indication or confirmation
  *
  * This function is use to retrieve service indications and confirmations for
  * an association and the arguments of the indication or confirmation.  Also
  * retrieved is the invoke id associated with the service indication or
  * confirmation.
  *
  * This function corresponds to the MAP-XXX indication and MAP-XXX confirmation
  * primtives of GSM TS 29.002.  This function maps from the TC-INVOKE,
  * TC-RESULT, TC-ERROR and TC-CANCEL indication primitives of ITU-T Rec. Q.771.
  */
MAP_status
__xmap_map_service_receive(OM_private_object assoc, OM_sint * primitive_return,
			   OM_private_object * arguments_return,
			   OM_private_object * component_return,
			   OM_uint32 * invoke_id_return)
{
	MAP_status status;
	struct __xmap_assoc *a;

	/* check association object for validity */
	if ((status = __xmap_validate_assoc(assoc, &a)))
		return (status);
	/* check validity of association */
	/* check association object for pending service indications and confirmations */
	/* deliver first pending service indication or confirmation */
	/* return primitive type of sevice indication or confirmation */
	/* return service indication or confirmation arguments */
	/* return invoke identifier */
	/* return status */
}

/** @fn MAP_status map_service_receive(OM_private_object assoc, OM_sint * primitive_return, OM_private_object * arguments_return, OM_private_object * component_return, OM_uint32 * invoke_id_return)
  * @param assoc
  * @param primitive_return
  * @param arguments_return
  * @param component_return
  * @param invoke_id_return
  */
__asm__(".symver __xmap_map_service_receive,map_service_receive@@XMAP_1.0");

/** @brief perform a service request
  * @param assoc MAP association
  * @param arguments arguments of the request
  * @param component component of the service request
  * @param invoke_id_return
  *
  * This function is used to generate a pending service request with arguments.
  * When successful, the function returns the invoke identifier for the service
  * request.
  *
  * Arguments are of type MAP_SERVICE_ARGS.
  *
  * Component is of type MAP_C_INVOKE.
  *
  * Invoke needs: invokeId, [linkedId], operationCode, [parameter]
  *
  * This function corresponds to the MAP-XXX request primitive of GSM TS 29.002.
  * This function maps to the TC-INVOKE request primitive of ITU-T Rec. Q.771.
  */
MAP_status
__xmap_map_service_req(OM_private_object assoc, OM_object arguments,
		       OM_object component, OM_uint32 * invoke_id_return)
{
	MAP_status status;
	struct __xmap_assoc *a;

	/* check association object for validity */
	if ((status = __xmap_validate_assoc(assoc, &a)))
		return (status);
	/* check validity of association and arguments */
	/* create invocation object */
	/* issue TC-INVOKE request primitive */
	/* return invoke identifier of new invocation object */
	/* return status */
}

/** @fn MAP_status map_service_req(OM_private_object assoc, OM_object arguments, OM_object component, OM_uint32 * invoke_id_return)
  * @param assoc
  * @param arguments
  * @param component
  * @param invoke_id_return
  */
__asm__(".symver __xmap_map_service_req,map_service_req@@XMAP_1.0");

/** @brief respond to a service request
  * @param assoc MAP association
  * @param aguments arguments of the response
  * @param component component of the service request
  * @param invoke_id invoke identifier of the service request
  *
  * This function is used to generate a pending service response with arguments.
  * The invoke identifier identifies the service request to which the call is
  * responding.
  *
  * Arguments are of type MAP_SERVICE_ARGS.
  *
  * Component is of type MAP_C_RESULT, MAP_C_ERROR, or MAP_C_REJECT.
  *
  * Result needs: invokeId, [operationCode, parameter]
  * Error needs: invokeId, errorCode, [parameter]
  * Reject needs: invokeId, problem
  *
  * This function corresponds to the MAP-XXX response primiitve of GSM TS
  * 29.002.  This function maps to the TC-RESULT, TC-ERROR, TC-CANCEL or
  * TC-REJECT request primitives of ITU-T Rec. Q.771.
  */
MAP_status
__xmap_map_service_rsp(OM_private_object assoc, OM_object arguments,
		       OM_object component, OM_uint32 invoke_id)
{
	MAP_status status;
	struct __xmap_assoc *a;

	/* check association object for validity */
	if ((status = __xmap_validate_assoc(assoc, &a)))
		return (status);
	/* check invoke_id for validity */
	/* local invocation object by invoke id */
	if ((status = __xmap_validate_comp(invoke_id, &c)))
		return (status);
	/* check validity of arguments */
	/* issue TC-RESULT, TC-ERROR, TC-CANCEL or TC-REJECT request primitive */
	/* destroy invocation object */
	/* return status */
}

/** @fn MAP_status map_service_rsp(OM_private_object assoc, OM_object arguments, OM_object component, OM_uint32 invoke_id)
  * @param assoc
  * @param arguments
  * @param component
  * @param invoke_id
  */
__asm__(".symver __xmap_map_service_rsp,map_service_rsp@@XMAP_1.0");

/** @brief add parameter(s) to a pending service request or response
  * @param assoc open MAP association
  * @param parameter parameters to add
  * @param invoke_id invoke identifier of pending service request or response
  *
  * @assoc is an open MAP association.
  *
  * @parameter is an OM object containing parameters.  This object is either of
  * type MAP_C_PARAMETER or MAP_C_PARAMETER_LIST.  Parameters must be of the
  * correct type and in the correct order for the component to which they are
  * being appended.
  *
  * @invoke_id is the invoke identifier of the component onto which to append
  * parameters.  The component must be a pending component.
  *
  * This function is not really necessary.  OM functions or direct operations
  * can be used to append OM objects or attributes to the service arguments or
  * result list.
  */
MAP_status
__xmap_map_service_parameter(OM_private_object assoc, OM_object parameter,
			     OM_sint invoke_id)
{
}
/** @fn MAP_status map_service_parameter(OM_private_object assoc, OM_object parameter, OM_sint invoke_id)
  * @param assoc
  * @param parameter
  * @param invoke_id
  */
__asm__(".symver __xmap_map_service_parameter,map_service_parameter@@XMAP_1.0");

// vim: com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
