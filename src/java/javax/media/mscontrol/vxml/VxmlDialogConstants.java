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

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
/*

     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

javax.media.mscontrol.vxml
Interface VxmlDialogConstants
     _________________________________________________________________

public interface VxmlDialogConstants

   Defines the Symbols used by VxmlDialogEvent.
     _________________________________________________________________

   Field Summary
   static Symbol ev_Disconnect
             Event sent by the vxml interpreter to request to be
   disconnected.
   static Symbol ev_Error
             This event is thrown when a dialog request cannot be
   completed (wrong internal state, ...).
   static Symbol ev_Event
             Event sent by the vxml interpreter when a mid-call event
   arises.
   static Symbol ev_Exited
             Event sent by the vxml interpreter, as a result of executing
   the element.
   static Symbol ev_Prepared
             Event sent by the vxml interpreter to signal the completion
   of a VxmlDialog.prepare(java.net.URL,
   javax.media.mscontrol.resource.Parameters, java.util.Map) request.
   static Symbol ev_Started
             Event sent by the vxml interpreter to signal the completion
   of a VxmlDialog.start(java.util.Map) request.
   static java.lang.String LOCAL_URI
             The session variable that defines the local uri (as per
   https://datatracker.ietf.org/drafts/draft-burke-vxml/ ).
   static Symbol p_AutoStart
             If set to Boolean.TRUE, the dialog will start immediately
   and automatically when it is prepared.
   static Symbol p_ConnectionData
             This optional parameter provides the script with the
   signalling message, or any other signalling data pertaining to the
   remote connection.
   static Symbol p_FetchTimeout
             The interval in ms to wait for the content to be returned
   before throwing an Error.e_Timeout event.
   static Symbol p_MaxAge
             Indicates that the file is willing to use content whose age
   is no greater than the specified time in seconds (cf. max-age in HTTP
   1.1 [RFC2616]).
   static Symbol p_MaxStale
             Indicates that the file is willing to use content that has
   exceeded its expiration time (cf. max-stale in HTTP 1.1 [RFC2616]).
   static java.lang.String PROTOCOL_NAME

   static java.lang.String PROTOCOL_SIP_HEADERS

   static java.lang.String PROTOCOL_SIP_MEDIA

   static java.lang.String PROTOCOL_SIP_REQUESTURI

   static java.lang.String PROTOCOL_VERSION

   static java.lang.String REDIRECT

   static java.lang.String REMOTE_URI



   Field Detail

  p_FetchTimeout

static final Symbol p_FetchTimeout

          The interval in ms to wait for the content to be returned
          before throwing an Error.e_Timeout event.
     _________________________________________________________________

  p_MaxStale

static final Symbol p_MaxStale

          Indicates that the file is willing to use content that has
          exceeded its expiration time (cf. max-stale in HTTP 1.1
          [RFC2616]). If maxstale is assigned a value, then the file is
          willing to accept content that has exceeded its expiration time
          by no more than the specified number of seconds.
     _________________________________________________________________

  p_MaxAge

static final Symbol p_MaxAge

          Indicates that the file is willing to use content whose age is
          no greater than the specified time in seconds (cf. max-age in
          HTTP 1.1 [RFC2616]). The file is not willing to use stale
          content, unless maxstale is also provided.
     _________________________________________________________________

  p_AutoStart

static final Symbol p_AutoStart

          If set to Boolean.TRUE, the dialog will start immediately and
          automatically when it is prepared. The application does not
          need to call the VxmlDialog.start(java.util.Map) method.
          In any case, both ev_Prepared and ev_Started will be received.
     _________________________________________________________________

  p_ConnectionData

static final Symbol p_ConnectionData

          This optional parameter provides the script with the signalling
          message, or any other signalling data pertaining to the remote
          connection.
          In a SIP context, this would be the (raw) SIP message.
          Value is a java.lang.String
     _________________________________________________________________

  ev_Prepared

static final Symbol ev_Prepared

          Event sent by the vxml interpreter to signal the completion of
          a VxmlDialog.prepare(java.net.URL,
          javax.media.mscontrol.resource.Parameters, java.util.Map)
          request.
     _________________________________________________________________

  ev_Started

static final Symbol ev_Started

          Event sent by the vxml interpreter to signal the completion of
          a VxmlDialog.start(java.util.Map) request.
     _________________________________________________________________

  ev_Exited

static final Symbol ev_Exited

          Event sent by the vxml interpreter, as a result of executing
          the element.
     _________________________________________________________________

  ev_Disconnect

static final Symbol ev_Disconnect

          Event sent by the vxml interpreter to request to be
          disconnected.
          (it is expected that the application will then terminate the
          dialog)
     _________________________________________________________________

  ev_Event

static final Symbol ev_Event

          Event sent by the vxml interpreter when a mid-call event
          arises.
          Mid-call events have to be agreed upon by the VxmlDialog and
          the application.
     _________________________________________________________________

  ev_Error

static final Symbol ev_Error

          This event is thrown when a dialog request cannot be completed
          (wrong internal state, ...).
     _________________________________________________________________

  LOCAL_URI

static final java.lang.String LOCAL_URI

          The session variable that defines the local uri (as per
          https://datatracker.ietf.org/drafts/draft-burke-vxml/ ).
          These can be used as keys in the Map argument of
          VxmlDialog.start(java.util.Map) or
          VxmlDialog.prepare(java.net.URL,
          javax.media.mscontrol.resource.Parameters, java.util.Map).

        See Also:
                Constant Field Values
     _________________________________________________________________

  REMOTE_URI

static final java.lang.String REMOTE_URI

        See Also:
                Constant Field Values
     _________________________________________________________________

  REDIRECT

static final java.lang.String REDIRECT

        See Also:
                Constant Field Values
     _________________________________________________________________

  PROTOCOL_NAME

static final java.lang.String PROTOCOL_NAME

        See Also:
                Constant Field Values
     _________________________________________________________________

  PROTOCOL_VERSION

static final java.lang.String PROTOCOL_VERSION

        See Also:
                Constant Field Values
     _________________________________________________________________

  PROTOCOL_SIP_HEADERS

static final java.lang.String PROTOCOL_SIP_HEADERS

        See Also:
                Constant Field Values
     _________________________________________________________________

  PROTOCOL_SIP_REQUESTURI

static final java.lang.String PROTOCOL_SIP_REQUESTURI

        See Also:
                Constant Field Values
     _________________________________________________________________

  PROTOCOL_SIP_MEDIA

static final java.lang.String PROTOCOL_SIP_MEDIA

        See Also:
                Constant Field Values
     _________________________________________________________________

   Overview  Package   Class  Use  Tree  Deprecated  Index  Help
    PREV CLASS   NEXT CLASS FRAMES    NO FRAMES     All Classes
   SUMMARY: NESTED | FIELD | CONSTR | METHOD
   DETAIL: FIELD | CONSTR | METHOD
     _________________________________________________________________

   Copyright © 2007-2008 Oracle and/or its affiliates. © Copyright
   2007-2008 Hewlett-Packard Development Company, L.P. All rights
   reserved. Use is subject to license terms.
*/
package javax.jain.media.mscontrol.vxml;
