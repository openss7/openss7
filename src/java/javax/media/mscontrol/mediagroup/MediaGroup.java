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

javax.media.mscontrol.mediagroup
Interface MediaGroup

   All Superinterfaces:
          Joinable, JoinableContainer, MediaObject,
          ResourceContainer<MediaGroupConfig>, java.io.Serializable,
          StatusEventNotifier
     _________________________________________________________________

public interface MediaGroup

   extends JoinableContainer, ResourceContainer<MediaGroupConfig>

   Adapter class which provides the asynchronous access to media
   containers (files) and signals (DMTF's).

   A MediaGroup is initially unrelated to any call. Then, a MediaGroup
   can be joined/unjoined to another Joinable: a NetworkConnection, or a
   MediaMixer.

   Most of its methods are documented in classes and interfaces defined
   in contained features :
   Player, Recorder, SignalDetector, SignalGenerator. The access to these
   methods depends on the resource given in the MediaGroupConfig.

   Due to the asynchronous nature of the API, the media resources methods
   (of interfaces Player, Recorder, SignalDetector and SignalGenerator)
   are non-blocking and the result is passed back to the application as
   an event through a listener interface (Callback methods). For example,
   in the play(). method, the result of the play is passed as a
   PlayerEvent event in a call to onEvent().

   Additional/optional capacities include HTTP, ASR, TTS, RTSP ...
     _________________________________________________________________

   Nested Class Summary



   Nested classes/interfaces inherited from interface
   javax.media.mscontrol.Joinable
   Joinable.Direction



   Field Summary



   Fields inherited from interface javax.media.mscontrol.MediaObject
   a_MediaObjectId



   Method Summary
    Player getPlayer()
             shortcut for getResource(Player.class)
    Recorder getRecorder()
             shortcut for getResource(Recorder.class)
    SignalDetector getSignalDetector()
             shortcut for getResource(SignalDetector.class)
    void stop()
             Stops all operations currently in progress on this media
   group.



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



   Method Detail

  getPlayer

Player getPlayer()
                 throws MscontrolException

          shortcut for getResource(Player.class)

        Throws:
                MscontrolException
     _________________________________________________________________

  getRecorder

Recorder getRecorder()
                     throws MscontrolException

          shortcut for getResource(Recorder.class)

        Throws:
                MscontrolException
     _________________________________________________________________

  getSignalDetector

SignalDetector getSignalDetector()
                                 throws MscontrolException

          shortcut for getResource(SignalDetector.class)

        Throws:
                MscontrolException
     _________________________________________________________________

  stop

void stop()

          Stops all operations currently in progress on this media group.

          Those operations that are stopped get a completion event with:
          ResourceEvent.getQualifier() == ResourceEvent.q_Stop
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
package javax.jain.media.mscontrol.mediagroup;
