// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/jain/protocol/ip/mgcp/message/NotificationRequest.java <p>
 
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
Class NotificationRequest

java.lang.Object
  |
  +--java.util.EventObject
        |
        +--jain.protocol.ip.mgcp.JainMgcpEvent
              |
              +--jain.protocol.ip.mgcp.JainMgcpCommandEvent
                    |
                    +--jain.protocol.ip.mgcp.message.NotificationRequest

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class NotificationRequest
   extends JainMgcpCommandEvent

   An MGCP NotificationRequest command, expressed as an event object,
   sent by a Call Agent to a Media Gateway to request the Gateway to
   notify the Call Agent of specified events, to apply signals to
   designated endpoints, to handle "quarantined" events in specified
   ways, to detect events during a "quarantine" period, and to configure
   designated endpoints. This command elicits a
   NotificationRequestResponse event.

   See Also: 
          NotificationRequestResponse, Serialized Form
     _________________________________________________________________

   Fields inherited from class java.util.EventObject
   source



   Constructor Summary
   NotificationRequest(java.lang.Object source,
   EndpointIdentifier endpointIdentifier,
   RequestIdentifier requestIdentifier)
             Constructs a new NotificationRequest (Command) Event object.



   Method Summary
    BearerInformation getBearerInformation()
             Gets the Bearer Information.
    EventName[] getDetectEvents()
             Gets the list of events the gateway is to detect.
    DigitMap getDigitMap()
             Gets the Digit Map.
    NotifiedEntity getNotifiedEntity()
             Gets the Notified Entity.
    QuarantineHandling getQuarantineHandling()
             Gets the Quarantine Handling.
    RequestedEvent[] getRequestedEvents()
             Gets the Requested Events.
    RequestIdentifier getRequestIdentifier()
             Gets the Request Identifier.
    EventName[] getSignalRequests()
             Gets the Signal Requests.
    void setBearerInformation(BearerInformation bearerInformation)
             Sets the Bearer Information.
    void setDetectEvents(EventName[] detectEvents)
             Sets the list of MGCP events that the gateway is to detect.
    void setDigitMap(DigitMap digitMap)
             Sets the Digit Map.
    void setNotifiedEntity(NotifiedEntity notifiedEntity)
             Sets the Notified Entity.
    void setQuarantineHandling(QuarantineHandling quarantineHandling)
             Sets the Quarantine Handling.
    void setRequestedEvents(RequestedEvent[] requestedEvents)
             Sets the Requested Events.
    void setRequestIdentifier(RequestIdentifier requestIdentifier)
             Sets the Request Identifier.
    void setSignalRequests(EventName[] signalRequests)
             Sets the Signal Requests.
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

  NotificationRequest

public NotificationRequest(java.lang.Object source,
                           EndpointIdentifier endpointIdentifier,
                           RequestIdentifier requestIdentifier)
                    throws java.lang.IllegalArgumentException

          Constructs a new NotificationRequest (Command) Event object.

        Parameters:
                endpointIdentifier - Name for the endpoint in the gateway
                where this command executes.
                requestIdentifier - Used to correlate this command with
                the notifications it triggers.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the
                endpoint identifier or the request identifier is
                null-valued.

   Method Detail

  getBearerInformation

public BearerInformation getBearerInformation()

          Gets the Bearer Information.

        Returns:
                Returns the Bearer Information parameter.
     _________________________________________________________________

  getDetectEvents

public EventName[] getDetectEvents()

          Gets the list of events the gateway is to detect.

        Returns:
                Returns the list of events the gateway is to detect.
     _________________________________________________________________

  getDigitMap

public DigitMap getDigitMap()

          Gets the Digit Map.

        Returns:
                Returns the Digit Map.
     _________________________________________________________________

  getNotifiedEntity

public NotifiedEntity getNotifiedEntity()

          Gets the Notified Entity.

        Returns:
                Returns the Notified Entity.
     _________________________________________________________________

  getQuarantineHandling

public QuarantineHandling getQuarantineHandling()

          Gets the Quarantine Handling.

        Returns:
                Returns the Quarantine Handling.
     _________________________________________________________________

  getRequestedEvents

public RequestedEvent[] getRequestedEvents()

          Gets the Requested Events.

        Returns:
                Returns the Requested Events.
     _________________________________________________________________

  getRequestIdentifier

public RequestIdentifier getRequestIdentifier()

          Gets the Request Identifier.

        Returns:
                Returns the Request Identifier.
     _________________________________________________________________

  getSignalRequests

public EventName[] getSignalRequests()

          Gets the Signal Requests.

        Returns:
                Returns the Signal Requests.
     _________________________________________________________________

  setBearerInformation

public void setBearerInformation(BearerInformation bearerInformation)

          Sets the Bearer Information.

        Parameters:
                bearerInformation - Optional parameter. The bearer
                information (i.e., the encoding method).
     _________________________________________________________________

  setDetectEvents

public void setDetectEvents(EventName[] detectEvents)

          Sets the list of MGCP events that the gateway is to detect.

        Parameters:
                detectEvents - Optional parameter. List of events that
                the gateway is requested to detect during the quarantine
                period. If not set explicitly, the events that should be
                detected in the quarantine period are those that were
                listed in the DetectEvents list most recently received by
                the gateway.
     _________________________________________________________________

  setDigitMap

public void setDigitMap(DigitMap digitMap)

          Sets the Digit Map.

        Parameters:
                digitMap - Optional parameter. A java.lang.String that contains a
                digit map according to which the gateway will accumulate
                digits.
     _________________________________________________________________

  setNotifiedEntity

public void setNotifiedEntity(NotifiedEntity notifiedEntity)

          Sets the Notified Entity.

        Parameters:
                notifiedEntity - Optional parameter. Specifies where
                notifications should be sent. If not set explicitly,
                notifications will be sent to originator of this command.
     _________________________________________________________________

  setQuarantineHandling

public void setQuarantineHandling(QuarantineHandling quarantineHandling)

          Sets the Quarantine Handling.

        Parameters:
                quarantineHandling - Optional parameter. Specifies
                handling of "quarantine" events; i.e., how events that
                were detected by the gateway before the arrival of this
                command and that have not yet been sent to the Call
                Agent, should be handled.
     _________________________________________________________________

  setRequestedEvents

public void setRequestedEvents(RequestedEvent[] requestedEvents)

          Sets the Requested Events.

        Parameters:
                requestedEvents - Optional parameter. List of events that
                the gateway is requested to detect and report.
     _________________________________________________________________

  setRequestIdentifier

public void setRequestIdentifier(RequestIdentifier requestIdentifier)
                          throws java.lang.IllegalArgumentException

          Sets the Request Identifier.

        Parameters:
                requestIdentifier - Used to correlate this command with
                the notifications it triggers.

        Throws:
                java.lang.IllegalArgumentException - Thrown if the
                request identifier is null-valued.
     _________________________________________________________________

  setSignalRequests

public void setSignalRequests(EventName[] signalRequests)

          Sets the Signal Requests.

        Parameters:
                signalRequests - Optional parameter. List of signals that
                the gateway is asked to apply to the endpoint.
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
