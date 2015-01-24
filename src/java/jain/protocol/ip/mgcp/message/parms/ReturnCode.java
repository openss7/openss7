// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/jain/protocol/ip/mgcp/message/parms/ReturnCode.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package jain.protocol.ip.mgcp.message.parms;

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

jain.protocol.ip.mgcp.message.parms
Class ReturnCode

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.message.parms.ReturnCode

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class ReturnCode
   extends java.lang.Object
   implements java.io.Serializable

   Defines the return codes that appear in responses to JAIN MGCP Command
   Events. Return codes with integer values between
     * 100 and 199 indicate a provisional response
     * 200 and 299 indicate a successful completion
     * 400 and 499 indicate a transient error
     * 500 and 599 indicate a permanent error

   Return code constants are defined for all return codes that have been
   defined in RFC 2705. Each such constant is also encapsulated in a
   final static object of class ReturnCode, so that a return code can be
   inserted in a response simply by referring to one of these predefined
   ReturnCode objects.

   Invoking getValue() on one of these objects returns the integer value
   that is encapsulated by the object. Invoking getComment() returns the
   comment associated with the return code, as a java.lang.String. Invoking
   toString() (which overrides java.lang.Object.toString()) returns a
   java.lang.String that displays the integer value of the return code, followed by
   the associated comment.

   See Also: 
          Serialized Form
     _________________________________________________________________

   Field Summary
   static ReturnCode CAS_Signaling_Protocol_Error
             ReturnCode object that signifies CAS signaling protocol
   error.
   static int CAS_SIGNALING_PROTOCOL_ERROR
             Signifies CAS signaling protocol error.
   static ReturnCode Connection_Was_Deleted
             ReturnCode object that signifies that connection was
   deleted.
   static int CONNECTION_WAS_DELETED
             Signifies that the connection was deleted.
   static ReturnCode Endpoint_Has_No_Digit_Map
             ReturnCode object that signifies that the endpoint does not
   have a digit map.
   static int ENDPOINT_HAS_NO_DIGIT_MAP
             Signifies that the endpoint does not have a digit map.
   static ReturnCode Endpoint_Insufficient_Resources
             ReturnCode object that signifies that the transaction could
   not be executed because the endpoint does not have sufficient
   resources.
   static int ENDPOINT_INSUFFICIENT_RESOURCES
             Signifies that the transaction could not be executed because
   the endpoint does not have sufficient resources.
   static ReturnCode Endpoint_Is_Restarting
             ReturnCode object that signifies that the transaction could
   not be executed because the endpoint is "restarting."
   static int ENDPOINT_IS_RESTARTING
             Signifies that the transaction could not be executed because
   the endpoint is "restarting."
   static ReturnCode Endpoint_Not_Ready
             ReturnCode object that signifies that the transaction could
   not be executed because the endpoint is not ready.
   static int ENDPOINT_NOT_READY
             Signifies that the transaction could not be executed because
   the endpoint is not ready.
   static ReturnCode Endpoint_Redirected
             ReturnCode object that signifies that the endpoint has been
   redirected to another Call Agent.
   static int ENDPOINT_REDIRECTED
             Signifies that the endpoint has been redirected to another
   Call Agent.
   static ReturnCode Endpoint_Unknown
             ReturnCode object that signifies that the transaction could
   not be executed because the endpoint is unknown.
   static int ENDPOINT_UNKNOWN
             Signifies that the transaction could not be executed because
   the endpoint is unknown.
   static ReturnCode Gateway_Cannot_Detect_Requested_Event
             ReturnCode object that signifies that the transaction could
   not be executed because the gateway is not equipped to detect one of
   the requested events.
   static int GATEWAY_CANNOT_DETECT_REQUESTED_EVENT
             Signifies that the transaction could not be executed because
   the gateway is not equipped to detect one of the requested events.
   static ReturnCode Gateway_Cannot_Generate_Requested_Signal
             ReturnCode object that signifies that the transaction could
   not be executed because the gateway is not equipped to generate one of
   the requested signals.
   static int GATEWAY_CANNOT_GENERATE_REQUESTED_SIGNAL
             Signifies that the transaction could not be executed because
   the gateway is not equipped to generate one of the requested signals.
   static ReturnCode Gateway_Cannot_Send_Specified_Announcement
             ReturnCode object that signifies that the transaction could
   not be executed because the gateway cannot send the specified
   announcement.
   static int GATEWAY_CANNOT_SEND_SPECIFIED_ANNOUNCEMENT
             Signifies that the transaction could not be executed because
   the gateway cannot send the specified announcement.
   static ReturnCode Incompatible_Protocol_Version
             ReturnCode object that signifies incompatible protocol
   version.
   static int INCOMPATIBLE_PROTOCOL_VERSION
             Signifies incompatible protocol version.
   static ReturnCode Incorrect_Connection_ID
             ReturnCode object that signifies that the transaction refers
   to an incorrect connection-id.
   static int INCORRECT_CONNECTION_ID
             Signifies that the transaction refers to an incorrect
   connection-id.
   static ReturnCode Insufficient_Bandwidth
             ReturnCode object that signifies insufficient bandwidth.
   static int INSUFFICIENT_BANDWIDTH
             Signifies insufficient bandwidth.
   static ReturnCode Insufficient_Bandwidth_Now
             ReturnCode object that signifies that there is insufficient
   bandwidth at this time.
   static int INSUFFICIENT_BANDWIDTH_NOW
             Signifies that there is insufficient bandwidth at this time.
   static ReturnCode Insufficient_Resources_Now
             ReturnCode object that signifies that the transaction could
   not be executed because the endpoint does not have sufficient
   resources at this time.
   static int INSUFFICIENT_RESOURCES_NOW
             Signifies that the transaction could not be executed because
   the endpoint does not have sufficient resources at this time.
   static ReturnCode Internal_Hardware_Failure
             ReturnCode object that signifies internal hardware failure.
   static int INTERNAL_HARDWARE_FAILURE
             Signifies internal hardware failure.
   static ReturnCode Internal_Inconsistency_In_LocalConnectionOptions
             ReturnCode object that signifies internal inconsistency in
   LocalConnectionOptions.
   static int INTERNAL_INCONSISTENCY_IN_LOCALCONNECTIONOPTIONS
             Signifies internal inconsistency in LocalConnectionOptions.
   static ReturnCode Missing_RemoteConnectionDescriptor
             ReturnCode object that signifies missing
   RemoteConnectionDescriptor.
   static int MISSING_REMOTECONNECTIONDESCRIPTOR
             Signifies missing RemoteConnectionDescriptor.
   static ReturnCode No_Such_Event_Or_Signal
             ReturnCode object that signifies that there is no such event
   or signal.
   static int NO_SUCH_EVENT_OR_SIGNAL
             Signifies that there is no such event or signal.
   static ReturnCode Phone_Off_Hook
             ReturnCode object that signifies that phone is already off
   hook.
   static int PHONE_OFF_HOOK
             Signifies that the phone is already off hook.
   static ReturnCode Phone_On_Hook
             ReturnCode object that signifies that phone is already on
   hook.
   static int PHONE_ON_HOOK
             Signifies that the phone is already on hook.
   static ReturnCode Protocol_Error
             ReturnCode object that signifies that the transaction could
   not be executed because a protocol error was detected.
   static int PROTOCOL_ERROR
             Signifies that the transaction could not be executed because
   a protocol error was detected.
   static ReturnCode Transaction_Being_Executed
             ReturnCode object that signifies that transaction is
   currently being executed.
   static int TRANSACTION_BEING_EXECUTED
             Signifies that transaction is currently being executed.
   static ReturnCode Transaction_Executed_Normally
             ReturnCode object that signifies that transaction was
   executed normally.
   static int TRANSACTION_EXECUTED_NORMALLY
             Signifies that requested transaction was executed normally.
   static ReturnCode Transient_Error
             ReturnCode object that signifies that the transaction could
   not be executed due to a transient error.
   static int TRANSIENT_ERROR
             Signifies that the transaction could not be executed due to
   a transient error.
   static ReturnCode Trunk_Group_Failure
             ReturnCode object that signifies failure of a grouping of
   trunks (e.g., facility failure).
   static int TRUNK_GROUP_FAILURE
             Signifies failure of a grouping of trunks (e.g., facility
   failure).
   static ReturnCode Unknown_Call_ID
             ReturnCode object that signifies that the transaction refers
   to an unknown call-id.
   static int UNKNOWN_CALL_ID
             Signifies that the transaction refers to an unknown call-id.
   static ReturnCode Unknown_Extension_In_LocalConnectionOptions
             ReturnCode object that signifies unknown extension in
   LocalConnectionOptions.
   static int UNKNOWN_EXTENSION_IN_LOCALCONNECTIONOPTIONS
             Signifies unknown extension in LocalConnectionOptions.
   static ReturnCode Unknown_Or_Illegal_Combination_Of_Actions
             ReturnCode object that signifies an unknown action or
   illegal combination of actions.
   static int UNKNOWN_OR_ILLEGAL_COMBINATION_OF_ACTIONS
             Signifies an unknown action or illegal combination of
   actions.
   static ReturnCode Unrecognized_Extension
             ReturnCode object that signifies that the transaction could
   not be executed because the command contained an unrecognized
   extension
   static int UNRECOGNIZED_EXTENSION
             Signifies that the transaction could not be executed because
   the command contained an unrecognized extension.
   static ReturnCode Unsupported_Or_Invalid_Mode
             ReturnCode object that signifies that mode is unsupported or
   invalid.
   static int UNSUPPORTED_OR_INVALID_MODE
             Signifies that the mode is unsupported or invalid.
   static ReturnCode Unsupported_Or_Unknown_Package
             ReturnCode object that signifies that package is unsupported
   or unknown.
   static int UNSUPPORTED_OR_UNKNOWN_PACKAGE
             Signifies that the package is unsupported or unknown.



   Method Summary
    java.lang.String getComment()
             Gets the return comment string set in this Return Code
   object.
    int getValue()
             Gets the integer-valued code set in this Return Code object.
    java.lang.String toString()
             Returns the return code, followed by the associated comment,
   as a java.lang.String.



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait,
   wait, wait



   Field Detail

  CAS_Signaling_Protocol_Error

public static final ReturnCode CAS_Signaling_Protocol_Error

          ReturnCode object that signifies CAS signaling protocol error.
     _________________________________________________________________

  CAS_SIGNALING_PROTOCOL_ERROR

public static final int CAS_SIGNALING_PROTOCOL_ERROR

          Signifies CAS signaling protocol error.
     _________________________________________________________________

  Connection_Was_Deleted

public static final ReturnCode Connection_Was_Deleted

          ReturnCode object that signifies that connection was deleted.
     _________________________________________________________________

  CONNECTION_WAS_DELETED

public static final int CONNECTION_WAS_DELETED

          Signifies that the connection was deleted.
     _________________________________________________________________

  Endpoint_Has_No_Digit_Map

public static final ReturnCode Endpoint_Has_No_Digit_Map

          ReturnCode object that signifies that the endpoint does not
          have a digit map.
     _________________________________________________________________

  ENDPOINT_HAS_NO_DIGIT_MAP

public static final int ENDPOINT_HAS_NO_DIGIT_MAP

          Signifies that the endpoint does not have a digit map.
     _________________________________________________________________

  Endpoint_Insufficient_Resources

public static final ReturnCode Endpoint_Insufficient_Resources

          ReturnCode object that signifies that the transaction could not
          be executed because the endpoint does not have sufficient
          resources.
     _________________________________________________________________

  ENDPOINT_INSUFFICIENT_RESOURCES

public static final int ENDPOINT_INSUFFICIENT_RESOURCES

          Signifies that the transaction could not be executed because
          the endpoint does not have sufficient resources.
     _________________________________________________________________

  Endpoint_Is_Restarting

public static final ReturnCode Endpoint_Is_Restarting

          ReturnCode object that signifies that the transaction could not
          be executed because the endpoint is "restarting."
     _________________________________________________________________

  ENDPOINT_IS_RESTARTING

public static final int ENDPOINT_IS_RESTARTING

          Signifies that the transaction could not be executed because
          the endpoint is "restarting."
     _________________________________________________________________

  Endpoint_Not_Ready

public static final ReturnCode Endpoint_Not_Ready

          ReturnCode object that signifies that the transaction could not
          be executed because the endpoint is not ready.
     _________________________________________________________________

  ENDPOINT_NOT_READY

public static final int ENDPOINT_NOT_READY

          Signifies that the transaction could not be executed because
          the endpoint is not ready.
     _________________________________________________________________

  Endpoint_Redirected

public static final ReturnCode Endpoint_Redirected

          ReturnCode object that signifies that the endpoint has been
          redirected to another Call Agent.
     _________________________________________________________________

  ENDPOINT_REDIRECTED

public static final int ENDPOINT_REDIRECTED

          Signifies that the endpoint has been redirected to another Call
          Agent.
     _________________________________________________________________

  Endpoint_Unknown

public static final ReturnCode Endpoint_Unknown

          ReturnCode object that signifies that the transaction could not
          be executed because the endpoint is unknown.
     _________________________________________________________________

  ENDPOINT_UNKNOWN

public static final int ENDPOINT_UNKNOWN

          Signifies that the transaction could not be executed because
          the endpoint is unknown.
     _________________________________________________________________

  Gateway_Cannot_Detect_Requested_Event

public static final ReturnCode Gateway_Cannot_Detect_Requested_Event

          ReturnCode object that signifies that the transaction could not
          be executed because the gateway is not equipped to detect one
          of the requested events.
     _________________________________________________________________

  GATEWAY_CANNOT_DETECT_REQUESTED_EVENT

public static final int GATEWAY_CANNOT_DETECT_REQUESTED_EVENT

          Signifies that the transaction could not be executed because
          the gateway is not equipped to detect one of the requested
          events.
     _________________________________________________________________

  Gateway_Cannot_Generate_Requested_Signal

public static final ReturnCode Gateway_Cannot_Generate_Requested_Signal

          ReturnCode object that signifies that the transaction could not
          be executed because the gateway is not equipped to generate one
          of the requested signals.
     _________________________________________________________________

  GATEWAY_CANNOT_GENERATE_REQUESTED_SIGNAL

public static final int GATEWAY_CANNOT_GENERATE_REQUESTED_SIGNAL

          Signifies that the transaction could not be executed because
          the gateway is not equipped to generate one of the requested
          signals.
     _________________________________________________________________

  Gateway_Cannot_Send_Specified_Announcement

public static final ReturnCode Gateway_Cannot_Send_Specified_Announcement

          ReturnCode object that signifies that the transaction could not
          be executed because the gateway cannot send the specified
          announcement.
     _________________________________________________________________

  GATEWAY_CANNOT_SEND_SPECIFIED_ANNOUNCEMENT

public static final int GATEWAY_CANNOT_SEND_SPECIFIED_ANNOUNCEMENT

          Signifies that the transaction could not be executed because
          the gateway cannot send the specified announcement.
     _________________________________________________________________

  Incompatible_Protocol_Version

public static final ReturnCode Incompatible_Protocol_Version

          ReturnCode object that signifies incompatible protocol version.
     _________________________________________________________________

  INCOMPATIBLE_PROTOCOL_VERSION

public static final int INCOMPATIBLE_PROTOCOL_VERSION

          Signifies incompatible protocol version.
     _________________________________________________________________

  Incorrect_Connection_ID

public static final ReturnCode Incorrect_Connection_ID

          ReturnCode object that signifies that the transaction refers to
          an incorrect connection-id.
     _________________________________________________________________

  INCORRECT_CONNECTION_ID

public static final int INCORRECT_CONNECTION_ID

          Signifies that the transaction refers to an incorrect
          connection-id.
     _________________________________________________________________

  Insufficient_Bandwidth

public static final ReturnCode Insufficient_Bandwidth

          ReturnCode object that signifies insufficient bandwidth.
     _________________________________________________________________

  INSUFFICIENT_BANDWIDTH

public static final int INSUFFICIENT_BANDWIDTH

          Signifies insufficient bandwidth.
     _________________________________________________________________

  Insufficient_Bandwidth_Now

public static final ReturnCode Insufficient_Bandwidth_Now

          ReturnCode object that signifies that there is insufficient
          bandwidth at this time.
     _________________________________________________________________

  INSUFFICIENT_BANDWIDTH_NOW

public static final int INSUFFICIENT_BANDWIDTH_NOW

          Signifies that there is insufficient bandwidth at this time.
     _________________________________________________________________

  Insufficient_Resources_Now

public static final ReturnCode Insufficient_Resources_Now

          ReturnCode object that signifies that the transaction could not
          be executed because the endpoint does not have sufficient
          resources at this time.
     _________________________________________________________________

  INSUFFICIENT_RESOURCES_NOW

public static final int INSUFFICIENT_RESOURCES_NOW

          Signifies that the transaction could not be executed because
          the endpoint does not have sufficient resources at this time.
     _________________________________________________________________

  Internal_Hardware_Failure

public static final ReturnCode Internal_Hardware_Failure

          ReturnCode object that signifies internal hardware failure.
     _________________________________________________________________

  INTERNAL_HARDWARE_FAILURE

public static final int INTERNAL_HARDWARE_FAILURE

          Signifies internal hardware failure.
     _________________________________________________________________

  Internal_Inconsistency_In_LocalConnectionOptions

public static final ReturnCode Internal_Inconsistency_In_LocalConnectionOptions

          ReturnCode object that signifies internal inconsistency in
          LocalConnectionOptions.
     _________________________________________________________________

  INTERNAL_INCONSISTENCY_IN_LOCALCONNECTIONOPTIONS

public static final int INTERNAL_INCONSISTENCY_IN_LOCALCONNECTIONOPTIONS

          Signifies internal inconsistency in LocalConnectionOptions.
     _________________________________________________________________

  Missing_RemoteConnectionDescriptor

public static final ReturnCode Missing_RemoteConnectionDescriptor

          ReturnCode object that signifies missing
          RemoteConnectionDescriptor.
     _________________________________________________________________

  MISSING_REMOTECONNECTIONDESCRIPTOR

public static final int MISSING_REMOTECONNECTIONDESCRIPTOR

          Signifies missing RemoteConnectionDescriptor.
     _________________________________________________________________

  No_Such_Event_Or_Signal

public static final ReturnCode No_Such_Event_Or_Signal

          ReturnCode object that signifies that there is no such event or
          signal.
     _________________________________________________________________

  NO_SUCH_EVENT_OR_SIGNAL

public static final int NO_SUCH_EVENT_OR_SIGNAL

          Signifies that there is no such event or signal.
     _________________________________________________________________

  Phone_Off_Hook

public static final ReturnCode Phone_Off_Hook

          ReturnCode object that signifies that phone is already off
          hook.
     _________________________________________________________________

  PHONE_OFF_HOOK

public static final int PHONE_OFF_HOOK

          Signifies that the phone is already off hook.
     _________________________________________________________________

  Phone_On_Hook

public static final ReturnCode Phone_On_Hook

          ReturnCode object that signifies that phone is already on hook.
     _________________________________________________________________

  PHONE_ON_HOOK

public static final int PHONE_ON_HOOK

          Signifies that the phone is already on hook.
     _________________________________________________________________

  Protocol_Error

public static final ReturnCode Protocol_Error

          ReturnCode object that signifies that the transaction could not
          be executed because a protocol error was detected.
     _________________________________________________________________

  PROTOCOL_ERROR

public static final int PROTOCOL_ERROR

          Signifies that the transaction could not be executed because a
          protocol error was detected.
     _________________________________________________________________

  Transaction_Being_Executed

public static final ReturnCode Transaction_Being_Executed

          ReturnCode object that signifies that transaction is currently
          being executed.
     _________________________________________________________________

  TRANSACTION_BEING_EXECUTED

public static final int TRANSACTION_BEING_EXECUTED

          Signifies that transaction is currently being executed.
     _________________________________________________________________

  Transaction_Executed_Normally

public static final ReturnCode Transaction_Executed_Normally

          ReturnCode object that signifies that transaction was executed
          normally.
     _________________________________________________________________

  TRANSACTION_EXECUTED_NORMALLY

public static final int TRANSACTION_EXECUTED_NORMALLY

          Signifies that requested transaction was executed normally.
     _________________________________________________________________

  Transient_Error

public static final ReturnCode Transient_Error

          ReturnCode object that signifies that the transaction could not
          be executed due to a transient error.
     _________________________________________________________________

  TRANSIENT_ERROR

public static final int TRANSIENT_ERROR

          Signifies that the transaction could not be executed due to a
          transient error.
     _________________________________________________________________

  Trunk_Group_Failure

public static final ReturnCode Trunk_Group_Failure

          ReturnCode object that signifies failure of a grouping of
          trunks (e.g., facility failure).
     _________________________________________________________________

  TRUNK_GROUP_FAILURE

public static final int TRUNK_GROUP_FAILURE

          Signifies failure of a grouping of trunks (e.g., facility
          failure).
     _________________________________________________________________

  Unknown_Call_ID

public static final ReturnCode Unknown_Call_ID

          ReturnCode object that signifies that the transaction refers to
          an unknown call-id.
     _________________________________________________________________

  UNKNOWN_CALL_ID

public static final int UNKNOWN_CALL_ID

          Signifies that the transaction refers to an unknown call-id.
     _________________________________________________________________

  Unknown_Extension_In_LocalConnectionOptions

public static final ReturnCode Unknown_Extension_In_LocalConnectionOptions

          ReturnCode object that signifies unknown extension in
          LocalConnectionOptions.
     _________________________________________________________________

  UNKNOWN_EXTENSION_IN_LOCALCONNECTIONOPTIONS

public static final int UNKNOWN_EXTENSION_IN_LOCALCONNECTIONOPTIONS

          Signifies unknown extension in LocalConnectionOptions.
     _________________________________________________________________

  Unknown_Or_Illegal_Combination_Of_Actions

public static final ReturnCode Unknown_Or_Illegal_Combination_Of_Actions

          ReturnCode object that signifies an unknown action or illegal
          combination of actions.
     _________________________________________________________________

  UNKNOWN_OR_ILLEGAL_COMBINATION_OF_ACTIONS

public static final int UNKNOWN_OR_ILLEGAL_COMBINATION_OF_ACTIONS

          Signifies an unknown action or illegal combination of actions.
     _________________________________________________________________

  Unrecognized_Extension

public static final ReturnCode Unrecognized_Extension

          ReturnCode object that signifies that the transaction could not
          be executed because the command contained an unrecognized
          extension
     _________________________________________________________________

  UNRECOGNIZED_EXTENSION

public static final int UNRECOGNIZED_EXTENSION

          Signifies that the transaction could not be executed because
          the command contained an unrecognized extension.
     _________________________________________________________________

  Unsupported_Or_Invalid_Mode

public static final ReturnCode Unsupported_Or_Invalid_Mode

          ReturnCode object that signifies that mode is unsupported or
          invalid.
     _________________________________________________________________

  UNSUPPORTED_OR_INVALID_MODE

public static final int UNSUPPORTED_OR_INVALID_MODE

          Signifies that the mode is unsupported or invalid.
     _________________________________________________________________

  Unsupported_Or_Unknown_Package

public static final ReturnCode Unsupported_Or_Unknown_Package

          ReturnCode object that signifies that package is unsupported or
          unknown.
     _________________________________________________________________

  UNSUPPORTED_OR_UNKNOWN_PACKAGE

public static final int UNSUPPORTED_OR_UNKNOWN_PACKAGE

          Signifies that the package is unsupported or unknown.

   Method Detail

  getComment

public java.lang.String getComment()

          Gets the return comment string set in this Return Code object.

        Returns:
                The string to be used as the return comment.
     _________________________________________________________________

  getValue

public int getValue()

          Gets the integer-valued code set in this Return Code object.

        Returns:
                The integer value of the return code.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Returns the return code, followed by the associated comment, as
          a java.lang.String.

        Overrides:
                toString in class java.lang.Object
     _________________________________________________________________

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

                  Copyright (C) 2000 Sun Microsystems, Inc.
*/
