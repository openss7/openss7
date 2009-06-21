/*
 @(#) $RCSfile: NetworkConnection.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:35:27 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:35:27 $ by $Author: brian $
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
Interface NetworkConnection

   All Superinterfaces:
          Joinable, JoinableContainer,
          MediaEventNotifier<NetworkConnectionEvent>, MediaObject,
          ResourceContainer<NetworkConnectionConfig>,
          java.io.Serializable, StatusEventNotifier
     _________________________________________________________________

public interface NetworkConnection

   extends JoinableContainer, ResourceContainer<NetworkConnectionConfig>,
          MediaEventNotifier<NetworkConnectionEvent>

   A NetworkConnection is a JoinableContainer that drives network ports.
   SDP session descriptions are used to setup a NetworkConnection.
   It handles multiple streams, each of them described by an SDP media
   description.
 ------------------------------------------------------------------------------
--
 -                                           stream(Media Description) <-rtp---
>-
 -NetworkConnection(Session Description)     stream(Media Description) <-rtp---
>-
 -                                           ....
 -
 ------------------------------------------------------------------------------
--


   Actually two SessionDescriptions are needed: A local
   SessionDescription describes the local side of a media connection
   (from media server standpoint) as opposed to the remote side,
   described by the remote SessionDescription.
 Remote Enpoint                                     Media Server
   v
   |
  ----                                                 ---------
  -  -                                                 -       -
  -  -   Remote side <-------RTP------>  Local side    -       -
  ----                                                 ---------

   A NetworkConnection can be created with createContainer(ConfigSymbol)
   (see also the other versions of this method, in MediaSession).
   Example:
  NetworkConnection myNC = myMediaSession.createContainer(NetworkConnectionConf
ig.c_Basic);

     _________________________________________________________________

   Nested Class Summary



   Nested classes/interfaces inherited from interface
   javax.media.mscontrol.Joinable
   Joinable.Direction



   Field Summary



   Fields inherited from interface javax.media.mscontrol.MediaObject
   a_MediaObjectId



   Method Summary
    javax.sdp.SessionDescription getLocalSessionDescription()
             This method gives access to the Local Session Description
   for media streams.
    java.lang.String getRawLocalSessionDescription()
             Same as getLocalSessionDescription(), with a result as a
   plain java.lang.String.
    java.lang.String getRawRemoteSessionDescription()
             Same as getRemoteSessionDescription(), with a result as a
   plain java.lang.String.
    javax.sdp.SessionDescription getRemoteSessionDescription()
             This method gives access to the Remote Session Description
   for media streams .
    void modify(javax.sdp.SessionDescription aLocalSessionDescription,
   javax.sdp.SessionDescription aRemoteSessionDescription)
             This method behaves exactly like modify(java.lang.String, java.lang.String).
    void modify(java.lang.String aLocalSessionDescription,
   java.lang.String aRemoteSessionDescription)
             The modify method is used to drive the NetworkConnection.



   Methods inherited from interface
   javax.media.mscontrol.JoinableContainer
   getJoinableStream, getJoinableStreams



   Methods inherited from interface javax.media.mscontrol.Joinable
   getJoinees, getJoinees, join, joinInitiate, unjoin, unjoinInitiate



   Methods inherited from interface
   javax.media.mscontrol.StatusEventNotifier
   addListener, getMediaSession, removeListener



   Methods inherited from interface
   javax.media.mscontrol.resource.ResourceContainer
   confirm, getConfig, getResource, triggerRTC



   Methods inherited from interface javax.media.mscontrol.MediaObject
   getParameters, getURI, release, setParameters



   Methods inherited from interface
   javax.media.mscontrol.resource.MediaEventNotifier
   addListener, getMediaSession, removeListener



   Method Detail

  modify

void modify(javax.sdp.SessionDescription aLocalSessionDescription,
            javax.sdp.SessionDescription aRemoteSessionDescription)
            throws javax.sdp.SdpException,
                   NetworkConnectionException,
                   ResourceNotAvailableException

          This method behaves exactly like modify(java.lang.String, java.lang.String). Please
          refer to it for documentation (including return type and
          exceptions ).

          Note: The ports are indicated by integers in
          SessionDescription; the choose symbol ("$") must be replaced by
          -1 in this case.

        Throws:
                javax.sdp.SdpException
                NetworkConnectionException
                ResourceNotAvailableException
     _________________________________________________________________

  modify

void modify(java.lang.String aLocalSessionDescription,
            java.lang.String aRemoteSessionDescription)
            throws javax.sdp.SdpException,
                   NetworkConnectionException,
                   ResourceNotAvailableException

          The modify method is used to drive the NetworkConnection. A
          NetworkConnection is fully defined by the local and remote
          SDPs.
          These SDPs are commonly exchanged using SIP.

          Different modify() use-cases, to be used depending if the SDP
          offer is local or remote:

   localSDP value remoteSDP value use-case
   1 "$" or "" X value Provide an offer of remote SDP, request answer of
   local SDP
   2 "$" or "" null Request an offer of local SDP. Need to wait for a
   second modify call to set remoteSDP and send ACK to the media server
   3 null X value second modify call, to provide remote SDP answer (2.
   already occured, localSDP must be set and is unchanged)
   4 null null No error, but nothing to do

          Relationship with SIP signalling messages:

 a) incoming INVITE with SDP
    remoteUA                       MediaServer
    ===================================================
           -------INVITE----------->
                  remoteSDP
                                   modify("$", remoteSDP)
                                   getLocalSDP()
           <------ 200 OK ----------
                  localSDP

           ------- ACK ------------>

 b) incoming INVITE without SDP
    remoteUA                       MediaServer
    =====================================================
           -------INVITE---------->
                                   modify("$", null)
                                   getLocalSDP()
           <------ 200 OK ---------
                  localSDP

           ------- ACK ----------->
                  remoteSDP
                                   modify(null, remoteSDP)

 c) outgoing INVITE with SDP
    remoteUA                       MediaServer
    ======================================================
                                   modify("$", null)
                                   getLocalSDP()
           <-----INVITE------------
                 localSDP

           ------ 200 OK --------->
                 remoteSDP
                                   modify(null, remoteSDP)
           <------ ACK ------------

  d) outgoing INVITE without SDP
    remoteUA                       MediaServer
    ======================================================
           <-----INVITE------------

           ------ 200 OK --------->
                 remoteSDP
                                   modify("$", remoteSDP)
                                   getLocalSDP()
           <------ ACK ------------
                 localSDP


          (this is provided as a help in understanding, but the
          NetworkConnection has no dependency on any signalling protocol,
          including SIP)

          See also modify(SessionDescription, SessionDescription) to use
          SessionDescription instead of raw Strings.

        Parameters:
                aLocalSessionDescription - The Local Session description
                describes the local stream characteritics as opened on
                the media server. If the value is "" or "$", all
                parameters like IP, port, codecs are under the media
                server control.
                Setting the value to a non-empty string forces the media
                server to comply with this description. Any field in the
                string can be CHOOSE ("$") values as defined by MEGACO .
                To create an outgoing call with a PCMU codec offered:
                Local Session Description =
                v=0
                m=audio $ RTP/AVP 0
                c=IN IP4 $
                Remote Media Description = null
                After a succesfull modify with these parameters, the
                method getLocalSessionDescription() returns:
                v=0
                m=audio 1234 RTP/AVP 0
                c=IN IP4 192.168.145.1
                The media server has opened an rtp session on ip
                192.168.145.1 and port 1234 and is ready to receive PCMU
                media.
                aRemoteSessionDescription - The remote session
                description describes the stream as opened by the remote
                endpoint. This description is typically received from a
                SIP message.
                To receive an incoming call:
                Local Session Description = "" or "$"; the media server
                will populate the LocalSessionDescription according to
                the offers/answer 
                Remote Media Description = (typically received in the SIP
                INVITE) :
                v=0
                m=audio 1234 RTP/AVP 0
                c=IN IP4 192.168.145.2
                After a succesfull call to modify with these parameters,
                method getLocalSessionDescription() returns:
                v=0
                m=audio 5678 RTP/AVP 0
                c=IN IP4 192.168.0.2
                At this stage, an rtp connection is established between
                192.168.0.2:5678 and 192.168.145.2:1234
                Note 1: For both local and remote descriptions, a null
                value means that the modify operation does not change the
                existing value.
                Note 2: The SessionDescription's can contain multiple
                m-lines; in this case the NetworkConnection will create
                multiple streams. Those streams can be joined altogether
                or individually, see JoinableContainer and
                JoinableStream.

                The method runs asynchronously, and the caller is
                notified of the completion by a NetworkConnectionEvent.
                This event contains a qualifier, among: q_Standard
                (normal completion), q_OfferPartiallyAccepted,
                q_ResourcePartiallyAvailable.
                "Partially accepted offer" usually means that audio is
                OK, but video is not available.

        Throws:
                javax.sdp.SdpException
                NetworkConnectionException
                ResourceNotAvailableException
     _________________________________________________________________

  getLocalSessionDescription

javax.sdp.SessionDescription getLocalSessionDescription()
                                                        throws NetworkConnectio
nException

          This method gives access to the Local Session Description for
          media streams.

        Returns:
                A SessionDescription
                After a modify, the returned SDP can be sent to a remote
                endpoint (using SIP).
                The Local Session Description describes what the media
                server is willing to accept, according to the
                offer/answer algorithm.

        Throws:
                NetworkConnectionException - if no session Description is
                available (for example, before a modify)
     _________________________________________________________________

  getRawLocalSessionDescription

java.lang.String getRawLocalSessionDescription()
                                               throws NetworkConnectionExceptio
n

          Same as getLocalSessionDescription(), with a result as a plain
          java.lang.String.

        Throws:
                NetworkConnectionException
     _________________________________________________________________

  getRemoteSessionDescription

javax.sdp.SessionDescription getRemoteSessionDescription()
                                                         throws NetworkConnecti
onException

          This method gives access to the Remote Session Description for
          media streams .

        Returns:
                A SessionDescription, as set by the last sucessfull call
                to modify method.

        Throws:
                NetworkConnectionException - if no session description is
                available ( before a succesfull modify)
     _________________________________________________________________

  getRawRemoteSessionDescription

java.lang.String getRawRemoteSessionDescription()
                                                throws NetworkConnectionExcepti
on

          Same as getRemoteSessionDescription(), with a result as a plain
          java.lang.String.

        Returns:
                java.lang.String

        Throws:
                NetworkConnectionException
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
