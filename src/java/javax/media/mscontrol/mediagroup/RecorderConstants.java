/*
 @(#) $RCSfile: RecorderConstants.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:10 $ <p>
 
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
 
 Last Modified $Date: 2011-02-07 07:27:10 $ by $Author: brian $
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
Interface RecorderConstants

   All Superinterfaces:
          CoderConstants

   All Known Subinterfaces:
          Recorder, RecorderEvent
     _________________________________________________________________

public interface RecorderConstants

   extends CoderConstants

   The Symbols used in Recorder and RecorderEvent.
     _________________________________________________________________

   Field Summary
   static Symbol ev_Pause
             Indicates that recording has been paused by RTC.
   static Symbol ev_Record
             Indicates that recording has been completed.
   static Symbol ev_Resume
             Indicates that recording has been resumed by RTC.
   static Symbol ev_Started
             Indicates that the recording has started.
   static Symbol p_Append
             Indicates that recording should append to the end of an
   existing TVM rather than overwrite it.
   static Symbol p_AudioClockrate
             The desired clock rate (or sample rate) of the audio
   encoding
   The value is an Integer, in Hz.
   static Symbol p_AudioCoder
             Symbol identifying the audio Coder used for a new recording.
   static Symbol p_AudioFmtp
             A string-valued list of detailed audio codec parameters, in
   the format described by rfc4566.
   static Symbol p_AudioMaxBitrate
             The maximum accepted bitrate for the audio stream.
   static Symbol p_BeepFrequency
             The frequency of the start beep; iff attribute a_Beep.
   static Symbol p_BeepLength
             Length of Beep preceeding recording; iff attribute a_Beep.
   static Symbol p_EnabledEvents
             An array of Recorder event Symbols, indicating which events
   are generated and delivered to the application.
   static Symbol p_FileFormat
             A Symbol identifying the File Format used during recording.
   static Symbol p_MaxDuration
             Integer indicating the maximum duration (in milliseconds)
   for a record.
   static Symbol p_MinDuration
             Integer indicating minimum duration (in milliseconds) that
   constitutes a valid recording.
   static Symbol p_Prompt
             Indicates a prompt to be played before the recording starts.
   static Symbol p_SignalTruncationOn
             Boolean indicating whether signal(DTMF) truncation is
   enabled.
   static Symbol p_SilenceTerminationOn
             Boolean indicating if a silence will terminate the
   recording.
   static Symbol p_SpeechDetectionMode
             A Symbol identifying the Speech detection mode.
   static Symbol p_StartBeep
             Boolean indicating whether subsequent record will be
   preceded with a beep.
   static Symbol p_StartPaused
             Boolean indicating whether subsequent record will start in
   PAUSE mode.
   static Symbol p_VideoCoder
             Symbol indicating the video coder to use for a new recording
   (if it includes a video track).
   static Symbol p_VideoFmtp
             A string-valued list of detailed video codec parameters
   static Symbol p_VideoMaxBitrate
             The maximum accepted bitrate for the video stream.
   static Symbol q_PromptFailed
             Returned by RecorderEvent.getQualifier() to indicate that
   the record operation was aborted because the prompt could not be
   played.
   static Symbol q_Silence
             Value for RecorderEvent.getQualifier().
   static Symbol rtca_Pause
             Pause the current operation on a recorder, maintaining the
   current position in the Media Stream being recorded.
   static Symbol rtca_Resume
             Resume the current operation on a recorder if paused
   static Symbol rtca_Stop
             Stop the current operation on a recorder
   static Symbol rtcc_RecordComplete
             Trigger when a Record is completed.
   static Symbol v_DetectAllOccurences
             value for p_SpeechDetectionMode.
   static Symbol v_DetectFirstOccurence
             value for p_SpeechDetectionMode.
   static Symbol v_Inactive
             value for p_SpeechDetectionMode.



   Fields inherited from interface
   javax.media.mscontrol.mediagroup.CoderConstants
   v_3G2Format, v_3GPFormat, v_ADPCM_16kG726, v_ADPCM_32k,
   v_ADPCM_32kG726, v_ADPCM_32kOKI, v_ALawPCM_48k, v_ALawPCM_64k, v_AMR,
   v_AMR_WB, v_EVRC, v_G723_1b, v_G729a, v_GSM, v_GSMFormat, v_H263,
   v_H263_1998, v_H264, v_Inferred, v_Linear16Bit_128k,
   v_Linear16Bit_256k, v_Linear8Bit_64k, v_MP4V_ES, v_MuLawPCM_64k,
   v_RawFormat, v_WavFormat



   Field Detail

  ev_Record

static final Symbol ev_Record

          Indicates that recording has been completed. Possible
          qualifiers are: q_RTC, q_Stop, q_Duration, q_Silence,
          q_Standard (specific to ASR part)
     _________________________________________________________________

  ev_Pause

static final Symbol ev_Pause

          Indicates that recording has been paused by RTC.
     _________________________________________________________________

  ev_Resume

static final Symbol ev_Resume

          Indicates that recording has been resumed by RTC.
     _________________________________________________________________

  ev_Started

static final Symbol ev_Started

          Indicates that the recording has started. A video recorder may
          wait for the reception of a stream synchronisation point, and
          this takes a noticeable amount of time.
          Note: This event must be enabled, see p_EnabledEvents
     _________________________________________________________________

  p_Append

static final Symbol p_Append

          Indicates that recording should append to the end of an
          existing TVM rather than overwrite it.
          Default: TRUE

        Since:
                Not supported on this release
     _________________________________________________________________

  p_BeepFrequency

static final Symbol p_BeepFrequency

          The frequency of the start beep; iff attribute a_Beep.
          Default: 400
     _________________________________________________________________

  p_BeepLength

static final Symbol p_BeepLength

          Length of Beep preceeding recording; iff attribute a_Beep.
          Default: 125
     _________________________________________________________________

  p_AudioCoder

static final Symbol p_AudioCoder

          Symbol identifying the audio Coder used for a new recording.
          Default: v_Inferred

          For a list of Coder type Symbols,

        See Also:
                CoderConstants
     _________________________________________________________________

  p_AudioFmtp

static final Symbol p_AudioFmtp

          A string-valued list of detailed audio codec parameters, in the
          format described by rfc4566.
          Example (For a G.723 coder): myParameters.put(p_AudioFmtp,
          "annexa=yes");
          p_AudioCoder must be defined also (and different from
          CoderConstants.v_Inferred), and the string must have a meaning
          for that codec.
          If multiple values are required, use a semi-colon as separator,
          for example: "bitrate=5.3;annexa=no"
          Default value: empty string.
     _________________________________________________________________

  p_AudioMaxBitrate

static final Symbol p_AudioMaxBitrate

          The maximum accepted bitrate for the audio stream.
          The value is an Integer, in bits-per-second units.
     _________________________________________________________________

  p_AudioClockrate

static final Symbol p_AudioClockrate

          The desired clock rate (or sample rate) of the audio encoding
          The value is an Integer, in Hz.
          The default value is 8000 Hz.
     _________________________________________________________________

  p_VideoCoder

static final Symbol p_VideoCoder

          Symbol indicating the video coder to use for a new recording
          (if it includes a video track).
          Default: v_Inferred For a list of Coder type Symbols,

        See Also:
                CoderConstants
     _________________________________________________________________

  p_VideoFmtp

static final Symbol p_VideoFmtp

          A string-valued list of detailed video codec parameters

        See Also:
                p_AudioFmtp
     _________________________________________________________________

  p_VideoMaxBitrate

static final Symbol p_VideoMaxBitrate

          The maximum accepted bitrate for the video stream.
          The value is an Integer, in bits-per-second units.
     _________________________________________________________________

  p_EnabledEvents

static final Symbol p_EnabledEvents

          An array of Recorder event Symbols, indicating which events are
          generated and delivered to the application. If a given eventID
          is not enabled, then no processing is expended to generate or
          distribute that kind of event.

          Value is an array of non-transactional event Symbols. Valid
          event Symbols are:

          + ev_Started.

          Note 1: This parameter controls the generation of events. The
          events cannot be delivered unless a MediaEventListener is added
          using addListener.
          Note 2: The record completion event (ev_Record), and the
          pause/resume events, (ev_Pause and ev_Resume), are always
          generated.
     _________________________________________________________________

  p_MaxDuration

static final Symbol p_MaxDuration

          Integer indicating the maximum duration (in milliseconds) for a
          record.
          Default: ResourceConstants.v_Forever
     _________________________________________________________________

  p_MinDuration

static final Symbol p_MinDuration

          Integer indicating minimum duration (in milliseconds) that
          constitutes a valid recording.
          Default: 0
     _________________________________________________________________

  p_SignalTruncationOn

static final Symbol p_SignalTruncationOn

          Boolean indicating whether signal(DTMF) truncation is enabled.
          Default: True
     _________________________________________________________________

  p_SilenceTerminationOn

static final Symbol p_SilenceTerminationOn

          Boolean indicating if a silence will terminate the recording.
          The record completion event will have a qualifier set to
          q_Silence.
          NOTE: a SpeechDetector resource must be included in the
          MediaGroupConfig for that parameter to operate.
          Default: Boolean.FALSE
     _________________________________________________________________

  p_StartBeep

static final Symbol p_StartBeep

          Boolean indicating whether subsequent record will be preceded
          with a beep.
          Default: Boolean.TRUE
     _________________________________________________________________

  p_StartPaused

static final Symbol p_StartPaused

          Boolean indicating whether subsequent record will start in
          PAUSE mode.
          Default: Boolean.FALSE
     _________________________________________________________________

  p_FileFormat

static final Symbol p_FileFormat

          A Symbol identifying the File Format used during recording.
          Default value: v_Inferred (The media server uses the file
          extension to determine the format)
          Other values: v_WavFormat, v_RawFormat, v_GSMFormat ,
          v_3GPFormat

        See Also:
                CoderConstants
     _________________________________________________________________

  p_SpeechDetectionMode

static final Symbol p_SpeechDetectionMode

          A Symbol identifying the Speech detection mode.

          NOTE: a SpeechDetector resource must be included in the
          MediaGroupConfig for that parameter to operate!
          Default: v_Inactive.
          Possible values: v_Inactive, v_DetectFirstOccurence &
          v_DetectAllOccurences.
     _________________________________________________________________

  v_Inactive

static final Symbol v_Inactive

          value for p_SpeechDetectionMode.

          If a SpeechDetector resource is included in the
          MediaGroupConfig, it won't operate (bypass mode).

        See Also:
                p_SpeechDetectionMode
     _________________________________________________________________

  v_DetectFirstOccurence

static final Symbol v_DetectFirstOccurence

          value for p_SpeechDetectionMode.

          If a SpeechDetector resource is included in the
          MediaGroupConfig, it will detect the first occurence of speech
          (cf. SpeechDetectorConstants to tune the speech detection).

        See Also:
                p_SpeechDetectionMode,
                SpeechDetectorConstants.p_InitialTimeout,
                SpeechDetectorConstants.p_FinalTimeout
     _________________________________________________________________

  v_DetectAllOccurences

static final Symbol v_DetectAllOccurences

          value for p_SpeechDetectionMode.

          If a SpeechDetector resource is included in the
          MediaGroupConfig, it will detect all speech occurences: the
          SpeechDetector will be "re-armed" after each trigger. (cf.
          SpeechDetectorConstants to tune the speech detection).

        See Also:
                p_SpeechDetectionMode,
                SpeechDetectorConstants.p_InitialTimeout,
                SpeechDetectorConstants.p_FinalTimeout
     _________________________________________________________________

  p_Prompt

static final Symbol p_Prompt

          Indicates a prompt to be played before the recording starts.
          The expected value is a URI, or URI[], to the prompt media.
          This parameter transforms the record operation into a
          "playrecord" feature.
          The Player is used for playing the prompt, and Player controls
          (through RTCs) are active during the prompt phase; However no
          PlayerEvent will be sent when the prompt completes, and
          stopping the recorder will also stop the playing, if
          applicable.
          You can think of p_Prompt as a shortcut for:

          + record with p_StartPaused = true
          + + play
          + + RTC(PlayerConstants.rtcc_PlayComplete,
            RecorderConstants.rtca_Resume)
          + + removal of the PlayerEvent
          + + Recorder.stop() causes a Player stop when in the prompt
            phase

          Other Player parameters can be given, for example
          PlayerConstants.p_AudioCoder.
          If the prompt cannot be played, the whole operation is
          terminated with error.
          If the Player is busy, the behavior described by
          PlayerConstants.p_IfBusy is applied.
          Additionally, barge-in (allowing the user to interrupt the
          prompt and start recording immediately) can be enabled with the
          bargeIn RTC.

          Example with prompt and barge-in enabled:

 Parameters options = myMediaSessionFactory.createParameters();
 options.put(RecorderConstants.p_Prompt, URI.create("http://server.com/prompt.3
gp"));

 myRecorder.record(URI.create("http://server.com/messages/user6367/recorded_mes
sage1.3gp"),
                   RTC.bargeIn,
                   options);

     _________________________________________________________________

  q_Silence

static final Symbol q_Silence

          Value for RecorderEvent.getQualifier(). Indicates that the
          record was terminated because silence was detected.
     _________________________________________________________________

  q_PromptFailed

static final Symbol q_PromptFailed

          Returned by RecorderEvent.getQualifier() to indicate that the
          record operation was aborted because the prompt could not be
          played.
          See getError() and getErrorText() for more.
     _________________________________________________________________

  rtca_Stop

static final Symbol rtca_Stop

          Stop the current operation on a recorder
     _________________________________________________________________

  rtca_Resume

static final Symbol rtca_Resume

          Resume the current operation on a recorder if paused
     _________________________________________________________________

  rtca_Pause

static final Symbol rtca_Pause

          Pause the current operation on a recorder, maintaining the
          current position in the Media Stream being recorded.
     _________________________________________________________________

  rtcc_RecordComplete

static final Symbol rtcc_RecordComplete

          Trigger when a Record is completed.
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
