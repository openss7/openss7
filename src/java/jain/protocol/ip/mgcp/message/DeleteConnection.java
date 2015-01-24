// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/jain/protocol/ip/mgcp/message/DeleteConnection.java <p>
 
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

package jain.protocol.ip.mgcp.message;

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

jain.protocol.ip.mgcp.message
Class DeleteConnection

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--jain.protocol.ip.mgcp.JainMgcpEvent
              |
              +--jain.protocol.ip.mgcp.JainMgcpCommandEvent
                    |
                    +--jain.protocol.ip.mgcp.message.DeleteConnection

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class DeleteConnection
   extends JainMgcpCommandEvent

   An MGCP DeleteConnection command. Sent by a Call Agent to request a
   Gateway to delete a connection, or multiple connections. Sent by a
   Gateway to a Call Agent to signify that a connection can no longer be
   sustained. This command elicits a DeleteConnectionResponse event.

   See Also: 
          DeleteConnectionResponse, Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source



   Constructor Summary
   DeleteConnection(java.lang.Object source,
   CallIdentifier callIdentifier, EndpointIdentifier endpointIdentifier)
             Constructs a new DeleteConnection (Command) Event object,
   used by a Call Agent to delete all connections that relate to a given
   call.
   DeleteConnection(java.lang.Object source,
   CallIdentifier callIdentifier, EndpointIdentifier endpointIdentifier,
   ConnectionIdentifier connectionIdentifier)
             Constructs a new DeleteConnection (Command) Event object,
   used by a Call Agent to delete a single connection.
   DeleteConnection(java.lang.Object source,
   CallIdentifier callIdentifier, EndpointIdentifier endpointIdentifier,
   ConnectionIdentifier connectionIdentifier, ReasonCode reasonCode,
   ConnectionParm[] connectionParms)
             Constructs a new DeleteConnection (Command) Event object,
   used by a VOIP Gateway to indicate to a Call Agent that a connection
   can no longer be sustained.
   DeleteConnection(java.lang.Object source,
   EndpointIdentifier endpointIdentifier)
             Constructs a new DeleteConnection (Command) Event object,
   used by a Call Agent to delete all connections related to a specified
   endpoint.



   Method Summary
    BearerInformation getBearerInformation()
             Gets the Bearer Information.
    CallIdentifier getCallIdentifier()
             Gets the Call Identifier.
    ConnectionIdentifier getConnectionIdentifier()
             Gets the Connection Identifier.
    ConnectionParm[] getConnectionParms()
             Gets the Connection Parameters.
    NotificationRequestParms getNotificationRequestParms()
             Gets the Notification Request parameters.
    ReasonCode getReasonCode()
             Gets the Reason Code.
    void setBearerInformation(BearerInformation bearerInformation)
             Sets the Bearer Information.
    void setCallIdentifier(CallIdentifier callIdentifier)
             Sets the Call Identifier.
    void
   setConnectionIdentifier(ConnectionIdentifier connectionIdentifier)
             Sets the Connection Identifier.
    void setConnectionParms(ConnectionParm[] connectionParms)
             Sets the Connection Parameters.
    void
   setNotificationRequestParms(NotificationRequestParms notificationReque
   stParms)
             Sets the Notification Request parameters.
    void setReasonCode(ReasonCode reasonCode)
             Sets the Reason Code.
    java.lang.String toString()
             Overrides java.lang.Object.toString().



   Methods inherited from class
   jain.protocol.ip.mgcp.JainMgcpCommandEvent
   BuildCommandHeader, getEndpointIdentifier, setEndpointIdentifier



   Methods inherited from class jain.protocol.ip.mgcp.JainMgcpEvent
   BuildListParmLine, getObjectIdentifier, getTransactionHandle,
   setTransactionHandle



   Methods inherited from class java.util.EventObject
   getSource



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait,
   wait, wait



   Constructor Detail

  DeleteConnection

public DeleteConnection(java.lang.Object source,
                        CallIdentifier callIdentifier,
                        EndpointIdentifier endpointIdentifier)
                 throws java.lang.IllegalArgumentException

          Constructs a new DeleteConnection (Command) Event object, used
          by a Call Agent to delete all connections that relate to a
          given call.

        Parameters:
                callIdentifier - Identifies the call for which the
                connections are being deleted.
                endpointIdentifier - Name for the endpoint in the gateway
                where this command executes.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the call
                identifier or the endpoint identifier is null-valued.
     _________________________________________________________________

  DeleteConnection

public DeleteConnection(java.lang.Object source,
                        CallIdentifier callIdentifier,
                        EndpointIdentifier endpointIdentifier,
                        ConnectionIdentifier connectionIdentifier)
                 throws java.lang.IllegalArgumentException

          Constructs a new DeleteConnection (Command) Event object, used
          by a Call Agent to delete a single connection.

        Parameters:
                callIdentifier - Identifies the call for which the
                connection is being deleted.
                endpointIdentifier - Name for the endpoint in the gateway
                where this command executes.
                connectionIdentifier - Identifies the connection within
                the endpoint whose characteristics are being modified.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the call
                identifier, the endpoint identifier, or the connection
                identifier is null-valued.
     _________________________________________________________________

  DeleteConnection

public DeleteConnection(java.lang.Object source,
                        CallIdentifier callIdentifier,
                        EndpointIdentifier endpointIdentifier,
                        ConnectionIdentifier connectionIdentifier,
                        ReasonCode reasonCode,
                        ConnectionParm[] connectionParms)
                 throws java.lang.IllegalArgumentException

          Constructs a new DeleteConnection (Command) Event object, used
          by a VOIP Gateway to indicate to a Call Agent that a connection
          can no longer be sustained.

        Parameters:
                callIdentifier - Identifies the call for which the
                connection is being deleted.
                endpointIdentifier - Name for the endpoint in the gateway
                where this command executes.
                connectionIdentifier - Identifies the connection within
                the endpoint whose characteristics are being modified.
                reasonCode - Identifies the reason that the connection
                can no longer be sustained.
                connectionParms - A list of connection parameters that
                describe the status of the connection.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the call
                identifier, the endpoint identifier, the connection
                identifier, the reason code, or the list of connection
                parameters (or any of the entries in that list) is
                null-valued.
     _________________________________________________________________

  DeleteConnection

public DeleteConnection(java.lang.Object source,
                        EndpointIdentifier endpointIdentifier)
                 throws java.lang.IllegalArgumentException

          Constructs a new DeleteConnection (Command) Event object, used
          by a Call Agent to delete all connections related to a
          specified endpoint.

        Parameters:
                endpointIdentifier - Name for the endpoint in the gateway
                where this command executes.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the
                endpoint identifier is null-valued.

   Method Detail

  getBearerInformation

public BearerInformation getBearerInformation()

          Gets the Bearer Information.

        Returns:
                Returns a reference to the Bearer Information parameter.
     _________________________________________________________________

  getCallIdentifier

public CallIdentifier getCallIdentifier()

          Gets the Call Identifier.

        Returns:
                The Call Identifier.
     _________________________________________________________________

  getConnectionIdentifier

public ConnectionIdentifier getConnectionIdentifier()

          Gets the Connection Identifier.

        Returns:
                The Connection Identifier.
     _________________________________________________________________

  getConnectionParms

public ConnectionParm[] getConnectionParms()

          Gets the Connection Parameters.

        Returns:
                The list of Connection Parameters.
     _________________________________________________________________

  getNotificationRequestParms

public NotificationRequestParms getNotificationRequestParms()

          Gets the Notification Request parameters.

        Returns:
                The Notification Request parameters.

        See Also: 
                NotificationRequestParms
     _________________________________________________________________

  getReasonCode

public ReasonCode getReasonCode()

          Gets the Reason Code.

        Returns:
                The Reason Code.
     _________________________________________________________________

  setBearerInformation

public void setBearerInformation(BearerInformation bearerInformation)

          Sets the Bearer Information.

        Parameters:
                bearerInformation - Optional parameter. The bearer
                information parameter (i.e., encoding method).
     _________________________________________________________________

  setCallIdentifier

public void setCallIdentifier(CallIdentifier callIdentifier)
                       throws java.lang.IllegalArgumentException

          Sets the Call Identifier.

        Parameters:
                callIdentifier - Identifies the call on which the
                connection is being deleted.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the call
                identifier is null-valued.
     _________________________________________________________________

  setConnectionIdentifier

public void setConnectionIdentifier(ConnectionIdentifier connectionIdentifier)
                             throws java.lang.IllegalArgumentException

          Sets the Connection Identifier.

        Parameters:
                connectionIdentifier - Identifies the connection within
                the endpoint whose characteristics are being modified.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the
                connection identifier is null-valued.
     _________________________________________________________________

  setConnectionParms

public void setConnectionParms(ConnectionParm[] connectionParms)
                        throws java.lang.IllegalArgumentException

          Sets the Connection Parameters.

        Parameters:
                connectionParms - A list of connection parameters that
                describe the status of the connection.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the
                connection parameters list is null-valued, or if any of
                the entries in the list is null-valued.
     _________________________________________________________________

  setNotificationRequestParms

public void setNotificationRequestParms(NotificationRequestParms notificationRe
questParms)

          Sets the Notification Request parameters.

        Parameters:
                notificationRequestParms - Optional parameter.
                Encapsulated Notification Request parameters.

        See Also: 
                NotificationRequestParms
     _________________________________________________________________

  setReasonCode

public void setReasonCode(ReasonCode reasonCode)
                   throws java.lang.IllegalArgumentException

          Sets the Reason Code.

        Parameters:
                reasonCode - Identifies the reason that the connection
                can no longer be sustained.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the reason
                code parameter is null-valued.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Overrides java.lang.Object.toString().

        Overrides:
                toString in class java.util.EventObject
     _________________________________________________________________

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

                  Copyright (C) 2000 Sun Microsystems, Inc.
*/
