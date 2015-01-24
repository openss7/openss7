/*
 @(#) src/java/javax/media/mscontrol/resource/MediaEvent.java <p>
 
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

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

javax.media.mscontrol.resource
Interface MediaEvent<S>

   All Known Subinterfaces:
          AllocationEvent, JoinEvent, MixerEvent, NetworkConnectionEvent,
          PlayerEvent, RecorderEvent, ResourceEvent<T>,
          SignalDetectorEvent, SignalGeneratorEvent,
          SpeechRecognitionEvent, StatusEvent, VideoRendererEvent,
          VxmlDialogEvent
     _________________________________________________________________

public interface MediaEvent<S>

   An event from a MediaEventNotifier. Characterized by the source of the
   event, and an EventID representing the event identity.
     _________________________________________________________________

   Method Summary
    Symbol getError()
             Identify the reason or cause of an error or failure.
    java.lang.String getErrorText()
             Return a human-readable error cause, if any.
    Symbol getEventID()
             Get the Symbol that identifies the event nature.
    S getSource()
             Gives access to the source of the Media Event.



   Method Detail

  getSource

S getSource()

          Gives access to the source of the Media Event. Can be for
          example: Player , NetworkConnection, MediaMixer or VxmlDialog
     _________________________________________________________________

  getEventID

Symbol getEventID()

          Get the Symbol that identifies the event nature.

          For completion events, this identifies the operation that has
          completed. For synchronous code this is mostly redundant, but
          it may be useful for asynchronous Listeners.
          It should never return null, even in the case of an error
          event.
          Further detail about the reason for this event is available
          using ResourceEvent.getQualifier().

          The Symbols returned by this method will have names of the
          form: ev_Operation.

        Returns:
                the Symbol identifying the event nature

        See Also:
                ResourceEvent
     _________________________________________________________________

  getError

Symbol getError()

          Identify the reason or cause of an error or failure.

          If this Event is associated with an Error, then getError()
          returns a Symbol that identifies the problem.
          If this Event is not associated with an Error, then getError()
          returns the Symbol e_OK.

          The list of error Symbols is defined in class Error. For each
          more specific event, like NetworkConnectionEvent, specific
          errors are defined, like in NetworkConnectionConstants

        Returns:
                one of the Error Symbols
     _________________________________________________________________

  getErrorText

java.lang.String getErrorText()

          Return a human-readable error cause, if any.
          Enables implementors to provide additional information.
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
