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
Interface PlayerConstants

   All Superinterfaces:
          CoderConstants

   All Known Subinterfaces:
          Player
     _________________________________________________________________

public interface PlayerConstants

   extends CoderConstants

   Defines the Symbols used by Player and PlayerEvent.
     _________________________________________________________________

   Field Summary
   static Symbol ev_Pause
             Play has been paused by RTC.
   static Symbol ev_PlayComplete
             Completion event for the play method.
   static Symbol ev_Resume
             Play has been resumed by RTC.
   static Symbol ev_Speed
             Playback speed has been changed due to RTC.
   static Symbol ev_Volume
             Playback volume has been changed due to RTC.
   static Symbol p_AudioCoder
             Symbol identifying the audio coder used.
   static Symbol p_EnabledEvents
             An array of Player event Symbols, indicating which events
   are generated and delivered to the application.
   static Symbol p_FileFormat
             A Symbol identifying the File Format used during a play.
   static Symbol p_IfBusy
             Indicates the action to take if Player is busy when play()
   is invoked.
   static Symbol p_Interval
             When p_RepeatCount is greater than 1, the player waits for
   p_Interval milliseconds between each repetition.
   static Symbol p_JumpMSCIncrement
             Integer number of MSCs to jump forward or backward in the
   MSC list, for either a rtca_JumpForwardMSCs or rtca_JumpBackwardMSCs.
   static Symbol p_JumpTime
             Integer number of milliseconds by which the current MSC
   offset is changed by rtca_JumpForward or rtca_JumpBackward.
   static Symbol p_MaxDuration
             Integer indicating maximum duration of a play request.
   static Symbol p_RepeatCount
             The play request will be repeated p_RepeatCount times.
   static Symbol p_StartOffset
             Indicates the offset in the file, at which the play should
   start.
   static Symbol p_StartPaused
             Boolean indicating that this or subsequent play should be
   started in the Paused state.
   static Symbol p_VolumeChange
             Determines the amount by which the volume is changed by the
   RTC actions rtca_VolumeUp and rtca_VolumeDown.
   static Symbol q_Duration
             Play ended because the maximum play duration time has been
   reached.
   static Symbol q_EndOfData
             Indicate Play complete because all the streams played to
   completion.
   static Symbol rtca_JumpBackward
             Jump backward by the amount specified by p_JumpTime.
   static Symbol rtca_JumpBackwardMSCs
             Jump backward the number of MSCs indicated by
   p_JumpMSCIncrement.
   static Symbol rtca_JumpEndMSC
             Jump to the end of the curent MSC.
   static Symbol rtca_JumpEndMSCList
             Jump to the Last MSC in the MSC List.
   static Symbol rtca_JumpForward
             Jump forward by the amount specified by p_JumpTime.
   static Symbol rtca_JumpForwardMSCs
             Jump forward the number of MSCs indicated by
   p_JumpMSCIncrement.
   static Symbol rtca_JumpStartMSC
             Jump to the start of the current MSC
   static Symbol rtca_JumpStartMSCList
             Jump to the First MSC in the MSC List.
   static Symbol rtca_NormalSpeed
             Set speed to normal.
   static Symbol rtca_NormalVolume
             Set volume to normal.
   static Symbol rtca_Pause
             Pause the current Play operation, maintaining the current
   position in the MSC and MSClist.
   static Symbol rtca_Resume
             Resume the current Play operation if paused.
   static Symbol rtca_SpeedDown
             Decrease speed
   static Symbol rtca_SpeedUp
             Increase speed
   static Symbol rtca_Stop
             Stop the current Play operation.
   static Symbol rtca_StopAll
             Stop the current Play operation and all pending play
   operations in queue.
   static Symbol rtca_ToggleVolume
             Toggle volume between normal and previous adjusted value.
   static Symbol rtca_VolumeDown
             Decrease volume by value of parameter p_VolumeChange.
   static Symbol rtca_VolumeUp
             Increase volume by value of parameter p_VolumeChange.
   static Symbol rtcc_PlayComplete
             Trigger when a Play is completed.
   static Symbol rtcc_PlayStarted
             Trigger when a Play is started.
   static Symbol v_Fail
             value for p_IfBusy: signal an error, throw a
   MediaResourceException.
   static Symbol v_Queue
             value for p_IfBusy: wait for previous requests to complete.
   static Symbol v_Stop
             value for p_IfBusy: stop any previous play.



   Fields inherited from interface
   javax.media.mscontrol.mediagroup.CoderConstants
   v_3G2Format, v_3GPFormat, v_ADPCM_16kG726, v_ADPCM_32k,
   v_ADPCM_32kG726, v_ADPCM_32kOKI, v_ALawPCM_48k, v_ALawPCM_64k, v_AMR,
   v_AMR_WB, v_EVRC, v_G723_1b, v_G729a, v_GSM, v_GSMFormat, v_H263,
   v_H263_1998, v_H264, v_Inferred, v_Linear16Bit_128k,
   v_Linear16Bit_256k, v_Linear8Bit_64k, v_MP4V_ES, v_MuLawPCM_64k,
   v_RawFormat, v_WavFormat



   Field Detail

  p_AudioCoder

static final Symbol p_AudioCoder

          Symbol identifying the audio coder used.
          Default: v_Inferred
          This information is required only for the headerless media file
          formats (raw files).

          For a list of Coder type Symbols,

        See Also:
                CoderConstants
     _________________________________________________________________

  p_IfBusy

static final Symbol p_IfBusy

          Indicates the action to take if Player is busy when play() is
          invoked.

          Valid values: v_Queue, v_Stop, v_Fail.
          Default value is v_Queue.
     _________________________________________________________________

  v_Queue

static final Symbol v_Queue

          value for p_IfBusy: wait for previous requests to complete.

        Since:
                Not supported

        See Also:
                p_IfBusy
     _________________________________________________________________

  v_Stop

static final Symbol v_Stop

          value for p_IfBusy: stop any previous play.

        Since:
                Not supported

        See Also:
                p_IfBusy
     _________________________________________________________________

  v_Fail

static final Symbol v_Fail

          value for p_IfBusy: signal an error, throw a
          MediaResourceException.

        Since:
                Not supported

        See Also:
                p_IfBusy
     _________________________________________________________________

  p_MaxDuration

static final Symbol p_MaxDuration

          Integer indicating maximum duration of a play request.

          Value is an Integer (0 - 1,000,000) milliseconds, or v_Forever.
     _________________________________________________________________

  p_StartOffset

static final Symbol p_StartOffset

          Indicates the offset in the file, at which the play should
          start.
          Value is an Integer, in milliseconds. Default value is zero.
     _________________________________________________________________

  p_StartPaused

static final Symbol p_StartPaused

          Boolean indicating that this or subsequent play should be
          started in the Paused state.
     _________________________________________________________________

  p_EnabledEvents

static final Symbol p_EnabledEvents

          An array of Player event Symbols, indicating which events are
          generated and delivered to the application. If a given eventID
          is not enabled, then no processing is expended to generate or
          distribute that kind of event.

          Valid values: ev_Pause, ev_Resume, ev_Speed, ev_Volume
          Default value = ev_Pause, ev_Resume.

          For a Player listener to receive events, those events must be
          enabled by setting this parameter. This parameter can be set in
          a MediaGroupConfig or by setParameters}.

        See Also:
                ev_Pause, ev_Resume, ev_Speed, ev_Volume
     _________________________________________________________________

  p_JumpMSCIncrement

static final Symbol p_JumpMSCIncrement

          Integer number of MSCs to jump forward or backward in the MSC
          list, for either a rtca_JumpForwardMSCs or
          rtca_JumpBackwardMSCs.

          Valid values: any positive Integer.
          Default value = 1.

        See Also:
                rtca_JumpForwardMSCs, rtca_JumpBackwardMSCs
     _________________________________________________________________

  p_JumpTime

static final Symbol p_JumpTime

          Integer number of milliseconds by which the current MSC offset
          is changed by rtca_JumpForward or rtca_JumpBackward.

          Valid values: any positive Integer.
          Default value = 5000.

        See Also:
                rtca_JumpForward, rtca_JumpBackward
     _________________________________________________________________

  p_RepeatCount

static final Symbol p_RepeatCount

          The play request will be repeated p_RepeatCount times.
          Valid values: an Integer greater or equal to 1.
          Default value = 1.
     _________________________________________________________________

  p_Interval

static final Symbol p_Interval

          When p_RepeatCount is greater than 1, the player waits for
          p_Interval milliseconds between each repetition.
          Valid values: an Integer greater or equal to zero.
          Default value = 0.
     _________________________________________________________________

  p_VolumeChange

static final Symbol p_VolumeChange

          Determines the amount by which the volume is changed by the RTC
          actions rtca_VolumeUp and rtca_VolumeDown. The value is an
          Integer expressing dB of change.
          Default value is 3.

        See Also:
                rtca_VolumeUp, rtca_VolumeDown
     _________________________________________________________________

  p_FileFormat

static final Symbol p_FileFormat

          A Symbol identifying the File Format used during a play.
          Default value: v_Inferred
          Other values: v_WavFormat, v_RawFormat, v_GSMFormat,
          v_3GPFormat

          Note 1: v_RawFormat requires to set also p_Coder.
          For the other formats, the coder is extracted from the file
          header.
          Note 2: The application is not required to specify explicitly
          p_FileFormat, if the file name extension is well-known (e.g.,
          "audiofile.wav", "videofile.3gp", "qtfile.mov", "clip.avi")

        See Also:
                p_AudioCoder
     _________________________________________________________________

  rtca_Stop

static final Symbol rtca_Stop

          Stop the current Play operation.
     _________________________________________________________________

  rtca_StopAll

static final Symbol rtca_StopAll

          Stop the current Play operation and all pending play operations
          in queue.
     _________________________________________________________________

  rtca_Resume

static final Symbol rtca_Resume

          Resume the current Play operation if paused.
     _________________________________________________________________

  rtca_Pause

static final Symbol rtca_Pause

          Pause the current Play operation, maintaining the current
          position in the MSC and MSClist. (Media Stream)
     _________________________________________________________________

  rtca_JumpForward

static final Symbol rtca_JumpForward

          Jump forward by the amount specified by p_JumpTime.
          If the resulting offset is past the end of the file, this
          causes a regular completion of the MSC playback.
     _________________________________________________________________

  rtca_JumpBackward

static final Symbol rtca_JumpBackward

          Jump backward by the amount specified by p_JumpTime. If the
          resulting offset is before the beginning of the file, the
          playback starts at the beginning of the file.
     _________________________________________________________________

  rtca_JumpStartMSC

static final Symbol rtca_JumpStartMSC

          Jump to the start of the current MSC
     _________________________________________________________________

  rtca_JumpEndMSC

static final Symbol rtca_JumpEndMSC

          Jump to the end of the curent MSC. Causes a regular termination
          of the MSC playback operation.
     _________________________________________________________________

  rtca_JumpForwardMSCs

static final Symbol rtca_JumpForwardMSCs

          Jump forward the number of MSCs indicated by
          p_JumpMSCIncrement.
     _________________________________________________________________

  rtca_JumpBackwardMSCs

static final Symbol rtca_JumpBackwardMSCs

          Jump backward the number of MSCs indicated by
          p_JumpMSCIncrement.
          Not supported in this release.
     _________________________________________________________________

  rtca_JumpStartMSCList

static final Symbol rtca_JumpStartMSCList

          Jump to the First MSC in the MSC List.
     _________________________________________________________________

  rtca_JumpEndMSCList

static final Symbol rtca_JumpEndMSCList

          Jump to the Last MSC in the MSC List.
          Not supported in this release.
     _________________________________________________________________

  rtca_SpeedUp

static final Symbol rtca_SpeedUp

          Increase speed
     _________________________________________________________________

  rtca_SpeedDown

static final Symbol rtca_SpeedDown

          Decrease speed
     _________________________________________________________________

  rtca_NormalSpeed

static final Symbol rtca_NormalSpeed

          Set speed to normal.
     _________________________________________________________________

  rtca_VolumeUp

static final Symbol rtca_VolumeUp

          Increase volume by value of parameter p_VolumeChange. Note: The
          range of volume control is limited to [-12, +12].
     _________________________________________________________________

  rtca_VolumeDown

static final Symbol rtca_VolumeDown

          Decrease volume by value of parameter p_VolumeChange.
     _________________________________________________________________

  rtca_NormalVolume

static final Symbol rtca_NormalVolume

          Set volume to normal.
     _________________________________________________________________

  rtca_ToggleVolume

static final Symbol rtca_ToggleVolume

          Toggle volume between normal and previous adjusted value.
     _________________________________________________________________

  rtcc_PlayStarted

static final Symbol rtcc_PlayStarted

          Trigger when a Play is started.
     _________________________________________________________________

  rtcc_PlayComplete

static final Symbol rtcc_PlayComplete

          Trigger when a Play is completed.
     _________________________________________________________________

  ev_PlayComplete

static final Symbol ev_PlayComplete

          Completion event for the play method.

          Possible qualifiers are:
          q_RTC, q_Stop, q_Duration, q_Standard, q_EndOfData

        See Also:
                PlayerEvent.getIndex(), PlayerEvent.getOffset(),
                ResourceEvent.getRTCTrigger()
     _________________________________________________________________

  ev_Pause

static final Symbol ev_Pause

          Play has been paused by RTC.

        See Also:
                PlayerEvent.getIndex(), PlayerEvent.getOffset(),
                ResourceEvent.getRTCTrigger()
     _________________________________________________________________

  ev_Resume

static final Symbol ev_Resume

          Play has been resumed by RTC.

        See Also:
                PlayerEvent.getIndex(), PlayerEvent.getOffset(),
                ResourceEvent.getRTCTrigger()
     _________________________________________________________________

  ev_Speed

static final Symbol ev_Speed

          Playback speed has been changed due to RTC.

        See Also:
                PlayerEvent.getChangeType(),
                ResourceEvent.getRTCTrigger()
     _________________________________________________________________

  ev_Volume

static final Symbol ev_Volume

          Playback volume has been changed due to RTC.

        See Also:
                PlayerEvent.getChangeType(),
                ResourceEvent.getRTCTrigger()
     _________________________________________________________________

  q_EndOfData

static final Symbol q_EndOfData

          Indicate Play complete because all the streams played to
          completion.
     _________________________________________________________________

  q_Duration

static final Symbol q_Duration

          Play ended because the maximum play duration time has been
          reached.
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
