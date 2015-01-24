// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/jain/protocol/ip/mgcp/message/parms/NotificationRequestParms.java <p>
 
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
Class NotificationRequestParms

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.message.parms.NotificationRequestParms

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class NotificationRequestParms
   extends java.lang.Object
   implements java.io.Serializable

   Contains all the parameters that would be sent in a
   NotificationRequest Command Object, except for the endpoint
   identifier, the notified entity, and any encapsulated endpoint
   configuration parameters.

   See Also: 
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   NotificationRequestParms(RequestIdentifier requestIdentifier)
             Constructs a new NotificationRequestParms object.



   Method Summary
    EventName[] getDetectEvents()
             Gets the list of events that the gateway is to detect.
    DigitMap getDigitMap()
             Gets the Digit Map.
    QuarantineHandling getQuarantineHandling()
             Gets the Quarantine Handling.
    RequestedEvent[] getRequestedEvents()
             Gets the Requested Events.
    RequestIdentifier getRequestIdentifier()
             Gets the Request Identifier.
    EventName[] getSignalRequests()
             Gets the Signal Requests.
    void setDetectEvents(EventName[] detectEvents)
             Sets the list of MGCP events that the gateway is to detect.
    void setDigitMap(DigitMap digitMap)
             Sets the Digit Map.
    void setQuarantineHandling(QuarantineHandling quarantineHandling)
             Sets the Quarantine Handling.
    void setRequestedEvents(RequestedEvent[] requestedEvents)
             Sets the Requested Events.
    void setRequestIdentifier(RequestIdentifier requestIdentifier)
             Sets the Request Identifier.
    void setSignalRequests(EventName[] signalRequests)
             Sets the Signal Requests.



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll,
   toString, wait, wait, wait



   Constructor Detail

  NotificationRequestParms

public NotificationRequestParms(RequestIdentifier requestIdentifier)

          Constructs a new NotificationRequestParms object.

        Parameters:
                requestIdentifier - Used to correlate this command with
                the notifications it triggers.

   Method Detail

  getDetectEvents

public EventName[] getDetectEvents()

          Gets the list of events that the gateway is to detect.

        Returns:
                Returns a reference to the list of events that the
                gateway is to detect.
     _________________________________________________________________

  getDigitMap

public DigitMap getDigitMap()

          Gets the Digit Map.

        Returns:
                Returns a reference to the Digit Map.
     _________________________________________________________________

  getQuarantineHandling

public QuarantineHandling getQuarantineHandling()

          Gets the Quarantine Handling.

        Returns:
                Returns a reference to the Quarantine Handling.
     _________________________________________________________________

  getRequestedEvents

public RequestedEvent[] getRequestedEvents()

          Gets the Requested Events.

        Returns:
                Returns a reference to the Requested Events.
     _________________________________________________________________

  getRequestIdentifier

public RequestIdentifier getRequestIdentifier()

          Gets the Request Identifier.

        Returns:
                Returns a reference to the Request Identifier.
     _________________________________________________________________

  getSignalRequests

public EventName[] getSignalRequests()

          Gets the Signal Requests.

        Returns:
                Returns a reference to the Signal Requests.
     _________________________________________________________________

  setDetectEvents

public void setDetectEvents(EventName[] detectEvents)

          Sets the list of MGCP events that the gateway is to detect.

        Parameters:
                detectEvents - Optional parameter. List of events that
                the gateway is requested to detect during the quarantine
                period. If set to null, the events that should be
                detected in the quarantine period are those that were
                listed in the list of events to detect most recently
                received by the gateway.
     _________________________________________________________________

  setDigitMap

public void setDigitMap(DigitMap digitMap)

          Sets the Digit Map.

        Parameters:
                digitMap - Optional parameter. A java.lang.String that contains a
                digit map according to which the gateway will accumulate
                digits. If set to null, the most recent previously
                defined digit map will be used.
     _________________________________________________________________

  setQuarantineHandling

public void setQuarantineHandling(QuarantineHandling quarantineHandling)

          Sets the Quarantine Handling.

        Parameters:
                quarantineHandling - Optional parameter. Specifies
                handling of "quarantine" events; i.e., how events that
                were detected by the gateway before the arrival of this
                command and that have not yet been sent to the Call
                Agent, should be handled. Must be set to null, if none.
     _________________________________________________________________

  setRequestedEvents

public void setRequestedEvents(RequestedEvent[] requestedEvents)

          Sets the Requested Events.

        Parameters:
                requestedEvents - Optional parameter. List of events that
                the gateway is requested to detect and report. Must be
                set to null, if none.
     _________________________________________________________________

  setRequestIdentifier

public void setRequestIdentifier(RequestIdentifier requestIdentifier)

          Sets the Request Identifier.

        Parameters:
                requestIdentifier - Used to correlate this command with
                the notifications it triggers.
     _________________________________________________________________

  setSignalRequests

public void setSignalRequests(EventName[] signalRequests)

          Sets the Signal Requests.

        Parameters:
                signalRequests - Optional parameter. List of signals that
                the gateway is asked to apply to the endpoint.
     _________________________________________________________________

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

                  Copyright (C) 2000 Sun Microsystems, Inc.
*/
