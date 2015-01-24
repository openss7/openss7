// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) src/java/jain/protocol/ip/mgcp/message/parms/InfoCode.java <p>
 
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
Class InfoCode

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.message.parms.InfoCode

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class InfoCode
   extends java.lang.Object
   implements java.io.Serializable

   Defines the information codes that are used to indicate which
   parameters are to be returned by responses to the audit commands
   (AuditEndpoint and AuditConnection). The RequestedInfo parameter
   passed by these commands is a list of these information codes.

   See Also: 
          Serialized Form
     _________________________________________________________________

   Field Summary
   static int BEARER_INFORMATION
             Signifies that Bearer Information is requested.
   static InfoCode BearerInformation
             Encapsulates the Bearer Information constant.
   static int CALL_IDENTIFIER
             Signifies that CallIdentifier information is requested.
   static InfoCode CallIdentifier
             Encapsulates the CallIdentifier constant.
   static InfoCode Capabilities
             Encapsulates the Capabilities constant.
   static int CAPABILITIES
             Signifies that Capabilities information is requested.
   static int CONNECTION_IDENTIFIER
             Signifies that ConnectionIdentifier information is
   requested.
   static int CONNECTION_MODE
             Signifies that ConnectionMode information is requested.
   static int CONNECTION_PARAMETERS
             Signifies that ConnectionParameters information is
   requested.
   static InfoCode ConnectionIdentifier
             Encapsulates the ConnectionIdentifier constant.
   static InfoCode ConnectionMode
             Encapsulates the ConnectionMode constant.
   static InfoCode ConnectionParameters
             Encapsulates the ConnectionParameters constant.
   static int DETECT_EVENTS
             Signifies that DetectEvents information is requested.
   static InfoCode DetectEvents
             Encapsulates the DetectEvents constant.
   static int DIGIT_MAP
             Signifies that DigitMap information is requested.
   static InfoCode DigitMap
             Encapsulates the DigitMap constant.
   static int EVENT_STATES
             Signifies that EventStates information is requested.
   static InfoCode EventStates
             Encapsulates the EventStates constant.
   static int LOCAL_CONNECTION_DESCRIPTOR
             Signifies that LocalConnectionDescriptor information is
   requested.
   static int LOCAL_CONNECTION_OPTIONS
             Signifies that LocalConnectionOptions information is
   requested.
   static InfoCode LocalConnectionDescriptor
             Encapsulates the LocalConnectionDescriptor constant.
   static InfoCode LocalConnectionOptions
             Encapsulates the LocalConnectionOptions constant.
   static int NOTIFIED_ENTITY
             Signifies that NotifiedEntity information is requested.
   static InfoCode NotifiedEntity
             Encapsulates the NotifiedEntity constant.
   static int OBSERVED_EVENTS
             Signifies that ObservedEvents information is requested.
   static InfoCode ObservedEvents
             Encapsulates the ObservedEvents constant.
   static int QUARANTINE_HANDLING
             Signifies that QuarantineHandling information is requested.
   static InfoCode QuarantineHandling
             Encapsulates the QuarantineHandling constant.
   static int REASON_CODE
             Signifies that ReasonCode information is requested.
   static InfoCode ReasonCode
             Encapsulates the ReasonCode constant.
   static int REMOTE_CONNECTION_DESCRIPTOR
             Signifies that RemoteConnectionDescriptor information is
   requested.
   static InfoCode RemoteConnectionDescriptor
             Encapsulates the RemoteConnectionDescriptor constant.
   static int REQUEST_IDENTIFIER
             Signifies that RequestIdentifier information is requested.
   static int REQUESTED_EVENTS
             Signifies that RequestedEvents information is requested.
   static InfoCode RequestedEvents
             Encapsulates the RequestedEvents constant.
   static InfoCode RequestIdentifier
             Encapsulates the RequestIdentifier constant.
   static int RESTART_DELAY
             Signifies that RestartDelay information is requested.
   static int RESTART_METHOD
             Signifies that RestartMethod information is requested.
   static InfoCode RestartDelay
             Encapsulates the RestartDelay constant.
   static InfoCode RestartMethod
             Encapsulates the RestartMethod constant.
   static int SIGNAL_REQUESTS
             Signifies that SignalRequests information is requested.
   static InfoCode SignalRequests
             Encapsulates the SignalRequests constant.
   static int SPECIFIC_ENDPOINT_ID
             Signifies that SpecificEndpointID information is requested.
   static InfoCode SpecificEndpointID
             Encapsulates the SpecificEndpointID constant.



   Method Summary
    int getInfoCode()
             Get the integer constant that corresponds to this
   information code.
    java.lang.String toString()
             Returns the one- or two-character code name of the parameter
   that this information code represents.



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait,
   wait, wait



   Field Detail

  BEARER_INFORMATION

public static final int BEARER_INFORMATION

          Signifies that Bearer Information is requested.
     _________________________________________________________________

  BearerInformation

public static final InfoCode BearerInformation

          Encapsulates the Bearer Information constant.
     _________________________________________________________________

  CALL_IDENTIFIER

public static final int CALL_IDENTIFIER

          Signifies that CallIdentifier information is requested.
     _________________________________________________________________

  CallIdentifier

public static final InfoCode CallIdentifier

          Encapsulates the CallIdentifier constant.
     _________________________________________________________________

  Capabilities

public static final InfoCode Capabilities

          Encapsulates the Capabilities constant.
     _________________________________________________________________

  CAPABILITIES

public static final int CAPABILITIES

          Signifies that Capabilities information is requested.
     _________________________________________________________________

  CONNECTION_IDENTIFIER

public static final int CONNECTION_IDENTIFIER

          Signifies that ConnectionIdentifier information is requested.
     _________________________________________________________________

  CONNECTION_MODE

public static final int CONNECTION_MODE

          Signifies that ConnectionMode information is requested.
     _________________________________________________________________

  CONNECTION_PARAMETERS

public static final int CONNECTION_PARAMETERS

          Signifies that ConnectionParameters information is requested.
     _________________________________________________________________

  ConnectionIdentifier

public static final InfoCode ConnectionIdentifier

          Encapsulates the ConnectionIdentifier constant.
     _________________________________________________________________

  ConnectionMode

public static final InfoCode ConnectionMode

          Encapsulates the ConnectionMode constant.
     _________________________________________________________________

  ConnectionParameters

public static final InfoCode ConnectionParameters

          Encapsulates the ConnectionParameters constant.
     _________________________________________________________________

  DETECT_EVENTS

public static final int DETECT_EVENTS

          Signifies that DetectEvents information is requested.
     _________________________________________________________________

  DetectEvents

public static final InfoCode DetectEvents

          Encapsulates the DetectEvents constant.
     _________________________________________________________________

  DIGIT_MAP

public static final int DIGIT_MAP

          Signifies that DigitMap information is requested.
     _________________________________________________________________

  DigitMap

public static final InfoCode DigitMap

          Encapsulates the DigitMap constant.
     _________________________________________________________________

  EVENT_STATES

public static final int EVENT_STATES

          Signifies that EventStates information is requested.
     _________________________________________________________________

  EventStates

public static final InfoCode EventStates

          Encapsulates the EventStates constant.
     _________________________________________________________________

  LOCAL_CONNECTION_DESCRIPTOR

public static final int LOCAL_CONNECTION_DESCRIPTOR

          Signifies that LocalConnectionDescriptor information is
          requested.
     _________________________________________________________________

  LOCAL_CONNECTION_OPTIONS

public static final int LOCAL_CONNECTION_OPTIONS

          Signifies that LocalConnectionOptions information is requested.
     _________________________________________________________________

  LocalConnectionDescriptor

public static final InfoCode LocalConnectionDescriptor

          Encapsulates the LocalConnectionDescriptor constant.
     _________________________________________________________________

  LocalConnectionOptions

public static final InfoCode LocalConnectionOptions

          Encapsulates the LocalConnectionOptions constant.
     _________________________________________________________________

  NOTIFIED_ENTITY

public static final int NOTIFIED_ENTITY

          Signifies that NotifiedEntity information is requested.
     _________________________________________________________________

  NotifiedEntity

public static final InfoCode NotifiedEntity

          Encapsulates the NotifiedEntity constant.
     _________________________________________________________________

  OBSERVED_EVENTS

public static final int OBSERVED_EVENTS

          Signifies that ObservedEvents information is requested.
     _________________________________________________________________

  ObservedEvents

public static final InfoCode ObservedEvents

          Encapsulates the ObservedEvents constant.
     _________________________________________________________________

  QUARANTINE_HANDLING

public static final int QUARANTINE_HANDLING

          Signifies that QuarantineHandling information is requested.
     _________________________________________________________________

  QuarantineHandling

public static final InfoCode QuarantineHandling

          Encapsulates the QuarantineHandling constant.
     _________________________________________________________________

  REASON_CODE

public static final int REASON_CODE

          Signifies that ReasonCode information is requested.
     _________________________________________________________________

  ReasonCode

public static final InfoCode ReasonCode

          Encapsulates the ReasonCode constant.
     _________________________________________________________________

  REMOTE_CONNECTION_DESCRIPTOR

public static final int REMOTE_CONNECTION_DESCRIPTOR

          Signifies that RemoteConnectionDescriptor information is
          requested.
     _________________________________________________________________

  RemoteConnectionDescriptor

public static final InfoCode RemoteConnectionDescriptor

          Encapsulates the RemoteConnectionDescriptor constant.
     _________________________________________________________________

  REQUEST_IDENTIFIER

public static final int REQUEST_IDENTIFIER

          Signifies that RequestIdentifier information is requested.
     _________________________________________________________________

  REQUESTED_EVENTS

public static final int REQUESTED_EVENTS

          Signifies that RequestedEvents information is requested.
     _________________________________________________________________

  RequestedEvents

public static final InfoCode RequestedEvents

          Encapsulates the RequestedEvents constant.
     _________________________________________________________________

  RequestIdentifier

public static final InfoCode RequestIdentifier

          Encapsulates the RequestIdentifier constant.
     _________________________________________________________________

  RESTART_DELAY

public static final int RESTART_DELAY

          Signifies that RestartDelay information is requested.
     _________________________________________________________________

  RESTART_METHOD

public static final int RESTART_METHOD

          Signifies that RestartMethod information is requested.
     _________________________________________________________________

  RestartDelay

public static final InfoCode RestartDelay

          Encapsulates the RestartDelay constant.
     _________________________________________________________________

  RestartMethod

public static final InfoCode RestartMethod

          Encapsulates the RestartMethod constant.
     _________________________________________________________________

  SIGNAL_REQUESTS

public static final int SIGNAL_REQUESTS

          Signifies that SignalRequests information is requested.
     _________________________________________________________________

  SignalRequests

public static final InfoCode SignalRequests

          Encapsulates the SignalRequests constant.
     _________________________________________________________________

  SPECIFIC_ENDPOINT_ID

public static final int SPECIFIC_ENDPOINT_ID

          Signifies that SpecificEndpointID information is requested.
     _________________________________________________________________

  SpecificEndpointID

public static final InfoCode SpecificEndpointID

          Encapsulates the SpecificEndpointID constant.

   Method Detail

  getInfoCode

public int getInfoCode()

          Get the integer constant that corresponds to this information
          code.

        Returns:
                One of the predefined constants that represent an
                information code.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Returns the one- or two-character code name of the parameter
          that this information code represents.

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
