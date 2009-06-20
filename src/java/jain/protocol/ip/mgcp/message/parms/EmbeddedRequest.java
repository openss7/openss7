// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date$ by $Author$
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
Class EmbeddedRequest

java.lang.Object
  |
  +--jain.protocol.ip.mgcp.message.parms.EmbeddedRequest

   All Implemented Interfaces:
          java.io.Serializable
     _________________________________________________________________

   public final class EmbeddedRequest
   extends java.lang.Object
   implements java.io.Serializable

   Specifies an embedded request.

   See Also: 
          Serialized Form
     _________________________________________________________________

   Constructor Summary
   EmbeddedRequest(DigitMap digitMap)
             Constructs a new EmbeddedRequest object that contains a
   digit map.
   EmbeddedRequest(EventName[] signalRequests)
             Constructs a new EmbeddedRequest object that contains a list
   of requested signals.
   EmbeddedRequest(EventName[] signalRequests, DigitMap digitMap)
             Constructs a new EmbeddedRequest object that contains a list
   of requested signals and a digit map.
   EmbeddedRequest(RequestedEvent[] requestedEvents)
             Constructs a new EmbeddedRequest object that contains a list
   of RequestedEvent objects.
   EmbeddedRequest(RequestedEvent[] requestedEvents, DigitMap digitMap)
             Constructs a new EmbeddedRequest object that contains a list
   of RequestedEvent objects and a digit map.
   EmbeddedRequest(RequestedEvent[] requestedEvents,
   EventName[] signalRequests)
             Constructs a new EmbeddedRequest object that contains a list
   of RequestedEvent objects and a list of requested signals.
   EmbeddedRequest(RequestedEvent[] requestedEvents,
   EventName[] signalRequests, DigitMap digitMap)
             Constructs a new EmbeddedRequest object that contains a list
   of RequestedEvent objects, a list of signal requests, and a digit map.



   Method Summary
    DigitMap getEmbeddedDigitMap()
             Gets the embedded digit map.
    RequestedEvent[] getEmbeddedRequestList()
             Gets the embedded requested event list.
    EventName[] getEmbeddedSignalRequest()
             Gets the embedded signal request list.
    java.lang.String toString()
             Returns a java.lang.String that expresses an embedded request.



   Methods inherited from class java.lang.Object
   clone, equals, finalize, getClass, hashCode, notify, notifyAll, wait,
   wait, wait



   Constructor Detail

  EmbeddedRequest

public EmbeddedRequest(DigitMap digitMap)

          Constructs a new EmbeddedRequest object that contains a digit
          map.

        Parameters:
                digitMap - The digit map.
     _________________________________________________________________

  EmbeddedRequest

public EmbeddedRequest(EventName[] signalRequests)

          Constructs a new EmbeddedRequest object that contains a list of
          requested signals.

        Parameters:
                signalRequests - The list of signal requests.
     _________________________________________________________________

  EmbeddedRequest

public EmbeddedRequest(EventName[] signalRequests,
                       DigitMap digitMap)

          Constructs a new EmbeddedRequest object that contains a list of
          requested signals and a digit map.

        Parameters:
                signalRequests - The list of signal requests.
                digitMap - The digit map.
     _________________________________________________________________

  EmbeddedRequest

public EmbeddedRequest(RequestedEvent[] requestedEvents)

          Constructs a new EmbeddedRequest object that contains a list of
          RequestedEvent objects.

        Parameters:
                requestedEvents - The list of RequestedEvent objects.
     _________________________________________________________________

  EmbeddedRequest

public EmbeddedRequest(RequestedEvent[] requestedEvents,
                       DigitMap digitMap)

          Constructs a new EmbeddedRequest object that contains a list of
          RequestedEvent objects and a digit map.

        Parameters:
                requestedEvents - The list of RequestedEvent objects.
                digitMap - The digit map.
     _________________________________________________________________

  EmbeddedRequest

public EmbeddedRequest(RequestedEvent[] requestedEvents,
                       EventName[] signalRequests)

          Constructs a new EmbeddedRequest object that contains a list of
          RequestedEvent objects and a list of requested signals.

        Parameters:
                requestedEvents - The list of RequestedEvent objects.
                signalRequests - The list of signal requests.
     _________________________________________________________________

  EmbeddedRequest

public EmbeddedRequest(RequestedEvent[] requestedEvents,
                       EventName[] signalRequests,
                       DigitMap digitMap)

          Constructs a new EmbeddedRequest object that contains a list of
          RequestedEvent objects, a list of signal requests, and a digit
          map.

        Parameters:
                requestedEvents - The list of RequestedEvent objects.
                signalRequests - The list of signal requests.
                digitMap - The digit map.

   Method Detail

  getEmbeddedDigitMap

public DigitMap getEmbeddedDigitMap()

          Gets the embedded digit map.

        Returns:
                The embedded digit map.
     _________________________________________________________________

  getEmbeddedRequestList

public RequestedEvent[] getEmbeddedRequestList()

          Gets the embedded requested event list.

        Returns:
                A list of embedded requested events.
     _________________________________________________________________

  getEmbeddedSignalRequest

public EventName[] getEmbeddedSignalRequest()

          Gets the embedded signal request list.

        Returns:
                A list of embedded signal requests.
     _________________________________________________________________

  toString

public java.lang.String toString()

          Returns a java.lang.String that expresses an embedded request. The java.lang.String
          is the concatenation of the following three Strings, separated
          by commas: "R(<requestedEvents>)"; "S(<signalRequests>)"; and
          "D(<digitMap>)". If the list of requested events, the list of
          signal requests, or the digit map is null-valued, the java.lang.String
          for that entity is omitted from the returned java.lang.String (along with
          the comma delimiter that would precede it, if any).

        Overrides:
                toString in class java.lang.Object
     _________________________________________________________________

   Overview  Package   Class  Tree  Index  Help
   JAIN^TM MGCP API (Final Release) - Version 1.0
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES
   SUMMARY:  INNER | FIELD | CONSTR | METHOD DETAIL:
   FIELD | CONSTR | METHOD
     _________________________________________________________________

                  Copyright © 2000 Sun Microsystems, Inc.
*/
