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

javax.media.mscontrol.networkconnection
Interface NetworkConnectionConstants
     _________________________________________________________________

public interface NetworkConnectionConstants

   Defines the Symbols used by NetworkConnection.
     _________________________________________________________________

   Field Summary
   static Symbol e_ModifyOffersRejected
             Error sent by media server to reject a modify request in
   case the offer/answer procedure does not find any matching codec for
   any stream.
   static Symbol e_ResourceNotAvailable
             Error sent by media server to reject a modify request in
   case of resource shortage.
   static Symbol ev_LocalSessionDescriptionModified
             Event sent by the media server to indicate that it requests
   a change in the local SDP.
   static Symbol ev_Modify
             Event sent by media server to acknowlege a modify request.
   static Symbol p_SIPHeaders
             This parameter allows the application to pass to the
   NetworkConnection some SIP Headers that may be of interest for media
   transport, like user-agent identification.
   static Symbol q_OfferPartiallyAccepted
             ResourceEvent.getQualifier() returns this Symbol to indicate
   that the offer is partialy accepted (at least 1 Media Description is
   acccepted)
   The application must use
   NetworkConnection.getLocalSessionDescription() to know what was
   accepted and what was rejected.
   static Symbol q_ResourcePartiallyAvailable
             ResourceEvent.getQualifier() returns this Symbol to indicate
   that a media resource cannot be allocated from the Media Server.



   Field Detail

  ev_Modify

static final Symbol ev_Modify

          Event sent by media server to acknowlege a modify request.
          This event may trigger a positive signaling answer.
          NetworkConnection.getLocalSessionDescription() contains a valid
          session description from the media server side (at least one m
          line is accepted). More refined data can be access from the
          qualifier associated to the event.
     _________________________________________________________________

  ev_LocalSessionDescriptionModified

static final Symbol ev_LocalSessionDescriptionModified

          Event sent by the media server to indicate that it requests a
          change in the local SDP.
          (IP address migration or other replication/high-availability
          feature)
     _________________________________________________________________

  p_SIPHeaders

static final Symbol p_SIPHeaders

          This parameter allows the application to pass to the
          NetworkConnection some SIP Headers that may be of interest for
          media transport, like user-agent identification.
          The expected value is a Map<String, String>, where each entry
          is a (header-name, header-value).
     _________________________________________________________________

  q_OfferPartiallyAccepted

static final Symbol q_OfferPartiallyAccepted

          ResourceEvent.getQualifier() returns this Symbol to indicate
          that the offer is partialy accepted (at least 1 Media
          Description is acccepted)
          The application must use
          NetworkConnection.getLocalSessionDescription() to know what was
          accepted and what was rejected.
     _________________________________________________________________

  q_ResourcePartiallyAvailable

static final Symbol q_ResourcePartiallyAvailable

          ResourceEvent.getQualifier() returns this Symbol to indicate
          that a media resource cannot be allocated from the Media
          Server. (but at least one Media Description is properly
          allocated)
          The application must use
          NetworkConnection.getLocalSessionDescription() to know more.
     _________________________________________________________________

  e_ModifyOffersRejected

static final Symbol e_ModifyOffersRejected

          Error sent by media server to reject a modify request in case
          the offer/answer procedure does not find any matching codec for
          any stream.
          This event should trigger a negative signaling answer ( >2xx ).
          NetworkConnection.getLocalSessionDescription() return null
     _________________________________________________________________

  e_ResourceNotAvailable

static final Symbol e_ResourceNotAvailable

          Error sent by media server to reject a modify request in case
          of resource shortage.
          This event should trigger a negative signaling answer ( >2xx ).
          NetworkConnection.getLocalSessionDescription() return null
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
package javax.jain.media.mscontrol.networkconnection;
