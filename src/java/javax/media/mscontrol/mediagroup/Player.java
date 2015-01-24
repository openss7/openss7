/*
 @(#) src/java/javax/media/mscontrol/mediagroup/Player.java <p>
 
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

javax.media.mscontrol.mediagroup
Interface Player

   All Superinterfaces:
          CoderConstants, MediaEventNotifier<PlayerEvent>,
          PlayerConstants, Resource<MediaGroup>, ResourceConstants
     _________________________________________________________________

public interface Player

   extends Resource<MediaGroup>, PlayerConstants,
          MediaEventNotifier<PlayerEvent>

   Defines methods for playing a media stream.
   A Player extracts data from a media stream container (MSC), transcodes
   the data as necessary, and streams the resultant data out of the
   MediaGroup.
   If the MediaGroup is joined to a NetworkConnection, the stream is sent
   out on the network.

   The play() method accepts one, or an array, of URI's which identify
   the media streams to be played. Each media stream may be encoded by
   any coder type supported by this player resource.

   When the media stream container has an audio and a video track, and
   the MediaGroup (containing the Player) has been configured with audio
   and video capabilities, then both tracks are played, with
   synchronisation.

   Text-to-Speech (TTS) functionality is available by playing a ssml
   document.
   See http://www.w3.org/TR/speech-synthesis/
   (See below an example of TTS usage)

   In addition to the play method, a number of RTC actions affect the
   operation of the player. These can be invoked through a Run Time
   Control RTC.

  Player controls

   A Player may optionally support additional processing features. These
   features define parameters and runtime controls that affect operation
   of an ongoing play operation. The presence of these features is
   indicated by a true value for the following attributes:

   Attribute: indicates the ability to:
   a_Pause    pause and resume in place
   a_Speed    change the playback speed
   a_Volume   increase and decrease the volume
   a_Jump     jump forward and backward while playing

   Coding types for data objects are defined in CoderConstants.

    Operation:

      Player States:

   The following is the state diagram for a player.
   States: IDLE<=>ACTIVE<=>PAUSED

   The Player has three states: Idle, Active, and Paused.

   In the Idle state, the Player is performing no coding or transmit
   operations. The state transitions to the Active or Paused state when
   the play() method starts. The state of the Player after play() starts
   is determined by the Boolean value of the parameter p_StartPaused. The
   state transitions to Active if p_StartPaused is false. The state
   transitions to Paused if p_StartPaused is true.

   In the Paused state, the player is not transmitting the media stream.
   It retains its place in the MSC being played, and resumes from the
   same place when the RTC action, rtca_Resume is received.

   In the Active state, the Player is busy, actively transmitting the
   media stream from a MSC to its output media stream (the Terminal). The
   Player continues in this state until it reaches the end of the MSC
   list or until RTC actions tell it to pause or stop. In the Active
   state, the Player may receive RTC commands to change the speed or
   volume of the play operation. It may also receive commands to jump
   forward or backward in the MSC.
   To stop the Player, you can use
   MediaGroup.triggerRTC(PlayerConstants.rtca_Stop).

   If a new play() is attempted while the Player is Active, the result is
   determined by the value of the p_IfBusy parameter. The alternative
   values for p_IfBusy are:
     * v_Queue: This play is queued (default). It is played when the
       previous plays have completed.
     * v_Stop: The current play and any queued plays are stopped, (with
       event qualifier q_Stop). This play begins immediately.
     * v_Fail: This play fails, with (getError()==Error.e_Busy)

  Text To Speech (TTS) support

   Examples:
myPlayer.play(URI.create("http://server.com/prompt.ssml"), null, null);


   or, using the data URI scheme (see RFC 2397):
myPlayer.play(URI.create("data:application/ssml+xml,"+
                                       "<?xml version=\"1.0\"?>"+
                                        "<speak>"+
                                           "<voice>"+
                                              "Hello, world"+
                                           "</voice>"+
                                        "</speak>"),
                                        null, null);


   See Also:
          for parameters and RTCs
     _________________________________________________________________

   Field Summary



   Fields inherited from interface
   javax.media.mscontrol.resource.ResourceConstants
   e_OK, FOREVER, q_Duration, q_RTC, q_Standard, q_Stop, rtcc_TriggerRTC,
   v_Forever



   Fields inherited from interface
   javax.media.mscontrol.mediagroup.PlayerConstants
   ev_Pause, ev_PlayComplete, ev_Resume, ev_Speed, ev_Volume,
   p_AudioCoder, p_EnabledEvents, p_FileFormat, p_IfBusy, p_Interval,
   p_JumpMSCIncrement, p_JumpTime, p_MaxDuration, p_RepeatCount,
   p_StartOffset, p_StartPaused, p_VolumeChange, q_Duration, q_EndOfData,
   rtca_JumpBackward, rtca_JumpBackwardMSCs, rtca_JumpEndMSC,
   rtca_JumpEndMSCList, rtca_JumpForward, rtca_JumpForwardMSCs,
   rtca_JumpStartMSC, rtca_JumpStartMSCList, rtca_NormalSpeed,
   rtca_NormalVolume, rtca_Pause, rtca_Resume, rtca_SpeedDown,
   rtca_SpeedUp, rtca_Stop, rtca_StopAll, rtca_ToggleVolume,
   rtca_VolumeDown, rtca_VolumeUp, rtcc_PlayComplete, rtcc_PlayStarted,
   v_Fail, v_Queue, v_Stop



   Fields inherited from interface
   javax.media.mscontrol.mediagroup.CoderConstants
   v_3G2Format, v_3GPFormat, v_ADPCM_16kG726, v_ADPCM_32k,
   v_ADPCM_32kG726, v_ADPCM_32kOKI, v_ALawPCM_48k, v_ALawPCM_64k, v_AMR,
   v_AMR_WB, v_EVRC, v_G723_1b, v_G729a, v_GSM, v_GSMFormat, v_H263,
   v_H263_1998, v_H264, v_Inferred, v_Linear16Bit_128k,
   v_Linear16Bit_256k, v_Linear8Bit_64k, v_MP4V_ES, v_MuLawPCM_64k,
   v_RawFormat, v_WavFormat



   Method Summary
    void play(java.net.URI[] streamIDs, RTC[] rtc, Parameters optargs)
             Play a sequence of MSCs (a composite prompt), identified by
   the streamIDs.
    void play(java.net.URI streamID, RTC[] rtc, Parameters optargs)
             Play a single MSC (Media Stream Container) named by
   streamID.



   Methods inherited from interface
   javax.media.mscontrol.resource.Resource
   getContainer, stop



   Methods inherited from interface
   javax.media.mscontrol.resource.MediaEventNotifier
   addListener, getMediaSession, removeListener



   Method Detail

  play

void play(java.net.URI[] streamIDs,
          RTC[] rtc,
          Parameters optargs)
          throws MscontrolException

          Play a sequence of MSCs (a composite prompt), identified by the
          streamIDs. This sequence of MSCs is called the "MSC list".

          This method returns as soon as the MSC list rendering is
          started (does not block until the end of the playing). When the
          play operation is complete, the result is passed in a
          PlayerEvent event through the MediaEventListener Interface. The
          condition that caused the play to stop is included in the
          completion event.
          RTC actions can alter the course of the play without the
          intervention of the application.
          Optional arguments can alter the characteristics of the play.

          Post-conditions:

         1. As indicated by getQualifier():
               o q_EndOfData: All MSCs named in this MSC list have been
                 played.
               o q_Stop: play was stopped by the stop() method or because
                 (p_IfBusy=v_Stop)
               o q_RTC: play was stopped by PlayerConstants.rtca_Stop

        Parameters:
                streamIDs - a URI[] naming the MSC list to be played.
                rtc - Array of RTC that may affect this play.
                optargs - a Parameters map of optional arguments.

        Throws:
                MscontrolException - if requested operation fails.

        See Also:
                MediaEventListener
     _________________________________________________________________

  play

void play(java.net.URI streamID,
          RTC[] rtc,
          Parameters optargs)
          throws MscontrolException

          Play a single MSC (Media Stream Container) named by streamID.
          Equivalent to play() with a URI[] of length one, containing the
          given streamID.

        Throws:
                MscontrolException
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
package javax.jain.media.mscontrol.mediagroup;
