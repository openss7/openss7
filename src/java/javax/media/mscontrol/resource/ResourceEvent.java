/*
 @(#) $RCSfile: ResourceEvent.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:12 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:12 $ by $Author: brian $
 */

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

javax.media.mscontrol.resource
Interface ResourceEvent<T>

   All Superinterfaces:
          MediaEvent<T>

   All Known Subinterfaces:
          MixerEvent, NetworkConnectionEvent, PlayerEvent, RecorderEvent,
          SignalDetectorEvent, SignalGeneratorEvent,
          SpeechRecognitionEvent, VideoRendererEvent
     _________________________________________________________________

public interface ResourceEvent<T>

   extends MediaEvent<T>

   The simple, synchronous interface to Resource events. ResourceEvent
   objects are returned from Resource methods and delivered to a
   ResourceListener.

   When a media resource method (for a Player, Recorder, etc.) completes
   successfully, it returns a ResourceEvent to the invoking program. If
   the method does not complete normally (ie, if the post-conditions are
   not satisfied), then the method throws a MediaResourceException that
   contains a ResourceEvent (accessible using getResourceEvent()). In
   either case, the ResourceEvent can be queried for additional
   information.

    ResourceEvent properties:

   Each resource event has read-only properties: EventID, an Error or
   Qualifier, and possibly an RTCTrigger. ResourceEvent defines get
   methods for each.

   If a media resource generates events that have additional fields, then
   the event interface for that resource extends ResourceEvent by
   defining methods to access the additional information.

    Names for Symbol constants used in ResourceEvent objects:

   The constants (Symbols and ints) that are used in ResourceEvent events
   defined in ResourceConstants, and inherited by ResourceEvent and other
   Resource events. Other resource event interfaces define additional
   constants.

   The Symbols returned by have names of the form:
   getEventID()              ev_OperationName
   getQualifier()             q_ReasonName
   getRTCTrigger()         rtcc_ConditionName
   getError()                 e_ErrorName
     _________________________________________________________________

   Method Summary
    Symbol getQualifier()
             Get additional information about how/why a transaction
   terminated, the reason that causes this event.
    Symbol getRTCTrigger()
             Get the RTC Trigger that caused this transaction completion.



   Methods inherited from interface
   javax.media.mscontrol.resource.MediaEvent
   getError, getErrorText, getEventID, getSource



   Method Detail

  getQualifier

Symbol getQualifier()

          Get additional information about how/why a transaction
          terminated, the reason that causes this event.

          Standard return values are one of:
          q_Standard, q_Duration, q_RTC, q_Stop
          which indicate the reason or mode of completion. Other
          qualifiers may be documented in the resource event classes.

          If the associated method or transaction did not succeed, this
          methods returns null; use MediaEvent.getError() instead.

          Because many qualifiers are orthogonal to the EventID, using a
          contained qualifier is simpler than using a subclassing scheme.

        Returns:
                one of the qualifier Symbols (q_Something)
     _________________________________________________________________

  getRTCTrigger

Symbol getRTCTrigger()

          Get the RTC Trigger that caused this transaction completion.
          This is non-null iff getQualifier == ResourceEvent.q_RTC

          The Symbol returned has a name of the form rtcc_Something.

        Returns:
                a Symbol identifying an RTC trigger condition.
     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

   Copyright (C) 2007-2008 Oracle and/or its affiliates. (C) Copyright
   2007-2008 Hewlett-Packard Development Company, L.P. All rights
   reserved. Use is subject to license terms.
*/
package javax.jain.media.mscontrol.resource;
