// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/jain/protocol/ip/mgcp/message/Notify.java <p>
 
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
Class Notify

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--jain.protocol.ip.mgcp.JainMgcpEvent
              |
              +--jain.protocol.ip.mgcp.JainMgcpCommandEvent
                    |
                    +--jain.protocol.ip.mgcp.message.Notify

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class Notify
   extends JainMgcpCommandEvent

   An MGCP Notify command, expressed as an event object, sent by a Media
   Gateway to a Call Agent to notify the latter of events that have been
   detected by the Gateway. Elicits a NotifyResponse Event Object.

   See Also: 
          NotifyResponse, Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source



   Constructor Summary
   Notify(java.lang.Object source, EndpointIdentifier endpointIdentifier,
   RequestIdentifier requestIdentifier, EventName[] observedEvents)
             Constructs a new Notify (Command) Event object.



   Method Summary
    NotifiedEntity getNotifiedEntity()
             Gets the Notified Entity.
    EventName[] getObservedEvents()
             Gets the Observed Events.
    RequestIdentifier getRequestIdentifier()
             Gets the Request Identifier.
    void setNotifiedEntity(NotifiedEntity notifiedEntity)
             Sets the Notified Entity.
    void setObservedEvents(EventName[] observedEvents)
             Sets the Observed Events.
    void setRequestIdentifier(RequestIdentifier requestIdentifier)
             Sets the Request Identifier.
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

  Notify

public Notify(java.lang.Object source,
              EndpointIdentifier endpointIdentifier,
              RequestIdentifier requestIdentifier,
              EventName[] observedEvents)
       throws java.lang.IllegalArgumentException

          Constructs a new Notify (Command) Event object.

        Parameters:
                endpointIdentifier - Name for the endpoint in the gateway
                for which the notification is taking place.
                requestIdentifier - Used to correlate this command with
                the notification request that triggers it.
                observedEvents - A list of events that the gateway
                detected.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the
                endpoint identifier, the request identifier, or the list
                of observed events (or any entry in that list) is
                null-valued.

   Method Detail

  getNotifiedEntity

public NotifiedEntity getNotifiedEntity()

          Gets the Notified Entity.

        Returns:
                Returns the Notified Entity.
     _________________________________________________________________

  getObservedEvents

public EventName[] getObservedEvents()

          Gets the Observed Events.

        Returns:
                Returns a reference to the Observed Events.
     _________________________________________________________________

  getRequestIdentifier

public RequestIdentifier getRequestIdentifier()

          Gets the Request Identifier.

        Returns:
                Returns the Request Identifier.
     _________________________________________________________________

  setNotifiedEntity

public void setNotifiedEntity(NotifiedEntity notifiedEntity)

          Sets the Notified Entity.

        Parameters:
                notifiedEntity - Optional parameter. Specifies where
                notifications should be sent. If not set explicitly,
                notifications will be sent to originator of this command.
     _________________________________________________________________

  setObservedEvents

public void setObservedEvents(EventName[] observedEvents)
                       throws java.lang.IllegalArgumentException

          Sets the Observed Events.

        Parameters:
                observedEvents - A list of events that the gateway
                detected.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the list
                of observed events, or any entry in that list, is
                null-valued.
     _________________________________________________________________

  setRequestIdentifier

public void setRequestIdentifier(RequestIdentifier requestIdentifier)
                          throws java.lang.IllegalArgumentException

          Sets the Request Identifier.

        Parameters:
                requestIdentifier - Used to correlate this command with
                the notification request that triggers it.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the
                request identifier is null-valued.
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
