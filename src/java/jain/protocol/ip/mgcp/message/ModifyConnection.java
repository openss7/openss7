// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/jain/protocol/ip/mgcp/message/ModifyConnection.java <p>
 
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
Class ModifyConnection

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--jain.protocol.ip.mgcp.JainMgcpEvent
              |
              +--jain.protocol.ip.mgcp.JainMgcpCommandEvent
                    |
                    +--jain.protocol.ip.mgcp.message.ModifyConnection

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class ModifyConnection
   extends JainMgcpCommandEvent

   An MGCP ModifyConnection command, expressed as an event object, sent
   by a Call Agent to a Media Gateway to request the Gateway to modify a
   connection between two endpoints. This command elicits a
   ModifyConnectionResponse event.

   See Also: 
          ModifyConnectionResponse, Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source



   Constructor Summary
   ModifyConnection(java.lang.Object source,
   CallIdentifier callIdentifier, EndpointIdentifier endpointIdentifier,
   ConnectionIdentifier connectionIdentifier)
             Constructs a new ModifyConnection (Command) Event object.



   Method Summary
    BearerInformation getBearerInformation()
             Gets the Bearer Information.
    CallIdentifier getCallIdentifier()
             Gets the Call Identifier.
    ConnectionIdentifier getConnectionIdentifier()
             Gets the Connection Identifier.
    LocalOptionValue[] getLocalConnectionOptions()
             Gets the Local Connection Options parameter.
    ConnectionMode getMode()
             Gets the Mode parameter.
    NotificationRequestParms getNotificationRequestParms()
             Gets the Notification Request parameters.
    NotifiedEntity getNotifiedEntity()
             Gets the Notified Entity.
    ConnectionDescriptor getRemoteConnectionDescriptor()
             Gets the Remote Connection Descriptor parameter.
    void setBearerInformation(BearerInformation bearerInformation)
             Sets the Bearer Information.
    void setCallIdentifier(CallIdentifier callIdentifier)
             Sets the Call Identifier.
    void
   setConnectionIdentifier(ConnectionIdentifier connectionIdentifier)
             Sets the Connection Identifier.
    void
   setLocalConnectionOptions(LocalOptionValue[] localConnectionOptions)
             Sets the Local Connection Options parameter.
    void setMode(ConnectionMode mode)
             Sets the Mode parameter.
    void
   setNotificationRequestParms(NotificationRequestParms notificationReque
   stParms)
             Sets the Notification Request parameters.
    void setNotifiedEntity(NotifiedEntity notifiedEntity)
             Sets the Notified Entity.
    void
   setRemoteConnectionDescriptor(ConnectionDescriptor remoteConnectionDes
   criptor)
             Sets the Remote Connection Descriptor parameter.
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

  ModifyConnection

public ModifyConnection(java.lang.Object source,
                        CallIdentifier callIdentifier,
                        EndpointIdentifier endpointIdentifier,
                        ConnectionIdentifier connectionIdentifier)
                 throws java.lang.IllegalArgumentException

          Constructs a new ModifyConnection (Command) Event object.

        Parameters:
                callIdentifier - A globally unique parameter that
                identifies the call (or session) to which this connection
                belongs.
                endpointIdentifier - Name for the endpoint in the gateway
                where this command executes.
                connectionIdentifier - Identifies the connection within
                the endpoint whose characteristics are being modified.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the call
                identifier, the endpoint identifier, or the connection
                identifier is null-valued.

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

  getLocalConnectionOptions

public LocalOptionValue[] getLocalConnectionOptions()

          Gets the Local Connection Options parameter.

        Returns:
                The list of Local Connection Option values.
     _________________________________________________________________

  getMode

public ConnectionMode getMode()

          Gets the Mode parameter.

        Returns:
                The Mode parameter.
     _________________________________________________________________

  getNotificationRequestParms

public NotificationRequestParms getNotificationRequestParms()

          Gets the Notification Request parameters.

        Returns:
                The Notification Request parameters.

        See Also: 
                NotificationRequestParms
     _________________________________________________________________

  getNotifiedEntity

public NotifiedEntity getNotifiedEntity()

          Gets the Notified Entity.

        Returns:
                The Notified Entity.
     _________________________________________________________________

  getRemoteConnectionDescriptor

public ConnectionDescriptor getRemoteConnectionDescriptor()

          Gets the Remote Connection Descriptor parameter.

        Returns:
                The Remote Connection Descriptor parameter.
     _________________________________________________________________

  setBearerInformation

public void setBearerInformation(BearerInformation bearerInformation)

          Sets the Bearer Information.

        Parameters:
                bearerInformation - Optional parameter. The bearer
                information (i.e., the encoding method).
     _________________________________________________________________

  setCallIdentifier

public void setCallIdentifier(CallIdentifier callIdentifier)
                       throws java.lang.IllegalArgumentException

          Sets the Call Identifier.

        Parameters:
                callIdentifier - A globally unique parameter that
                identifies the call (or session) to which this connection
                belongs.

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

  setLocalConnectionOptions

public void setLocalConnectionOptions(LocalOptionValue[] localConnectionOptions
)

          Sets the Local Connection Options parameter.

        Parameters:
                localConnectionOptions - Optional parameter. A list of
                local connection option values. Used by the Call Agent to
                direct the handling of the connection by the gateway.
     _________________________________________________________________

  setMode

public void setMode(ConnectionMode mode)

          Sets the Mode parameter.

        Parameters:
                mode - Optional parameter. Indicates the mode of
                operation for this side of the connection; e.g., send,
                receive, etc.
     _________________________________________________________________

  setNotificationRequestParms

public void setNotificationRequestParms(NotificationRequestParms notificationRe
questParms)

          Sets the Notification Request parameters.

        Parameters:
                notificationRequestParms - Optional parameter. Used to
                encapsulate notification request parameters in this
                command.

        See Also: 
                NotificationRequestParms
     _________________________________________________________________

  setNotifiedEntity

public void setNotifiedEntity(NotifiedEntity notifiedEntity)

          Sets the Notified Entity.

        Parameters:
                notifiedEntity - Optional parameter. Specifies where
                notifications should be sent. If not set explicitly,
                notifications will be sent to originator of this command.
     _________________________________________________________________

  setRemoteConnectionDescriptor

public void setRemoteConnectionDescriptor(ConnectionDescriptor remoteConnection
Descriptor)

          Sets the Remote Connection Descriptor parameter.

        Parameters:
                remoteConnectionDescriptor - Optional parameter. The
                connection descriptor for the remote side of a
                connection, on the other side of the IP network.
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
