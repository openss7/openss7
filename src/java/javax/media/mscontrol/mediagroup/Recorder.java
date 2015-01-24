/*
 @(#) src/java/javax/media/mscontrol/mediagroup/Recorder.java <p>
 
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
Interface Recorder

   All Superinterfaces:
          CoderConstants, MediaEventNotifier<RecorderEvent>,
          RecorderConstants, Resource<MediaGroup>, ResourceConstants
     _________________________________________________________________

public interface Recorder

   extends Resource<MediaGroup>, RecorderConstants,
          MediaEventNotifier<RecorderEvent>

   Defines a method for recording media stream(s) into a
   MediaStreamContainer (usually a data file).
   A Recorder retrieves the media stream from the Joinable object to
   which the MediaGroup is joined; In the simple case, it is a
   NetworkConnection, and the data come from the network. Transcoding is
   automatically included, when the codecs don't match.

   By default, the file format is determined by the file name extension,
   and the codec is the one of the incoming stream.
   Alternately, the application can request a variety of codecs, see
   CoderConstants.

   When the record method returns, the record operation is requested to
   start.

   When the recording is finished, a RecorderEvent is delivered through
   the onEvent() callback.

   Run Time Controls can be used to stop recording.

    States:

   The following is the state diagram for a basic Recorder:
   States: IDLE<=>ACTIVE<=>PAUSED

   The recorder is either Idle, Active, or Paused.

   record(URI, RTC[], Parameters) fails unless the state is Idle, and
   then the state becomes Active or Paused. The state of the Recorder
   after record() starts is determined by the Boolean value of the
   parameter p_StartPaused: The state transitions to Active if
   p_StartPaused is false, else to Paused.

   When recording stops, the state becomes Idle.
   Recording stops for a variety of reasons.

   rtca_Pause has no effect unless state is Recording, and then the state
   becomes Paused, and ev_Pause is sent to MediaEventListener
   onEvent(RecorderEvent) method.

   rtca_Resume has no effect unless state is Paused, and then the state
   becomes Recording, and ev_Resume is sent to MediaEventListener
   onEvent(RecorderEvent). Support of recording to local files is
   mandatory, other protocol (http...) are optional, defined by other
   packages.

    Speech detection operations

   The following is the state diagram for a Recorder associated with a
   speech detector (as defined by the MediaGroupConfig, see for example
   IVR configSpec).
   States: IDLE<=>PRE-SPEECH-PAUSED<=>ACTIVE<=>POST-SPEECH-PAUSED

   In that MediaGroupConfig case, both state diagrams should be taken
   into account.

   SpeechDetector parameters, specific RecorderEvent qualifiers &
   supported RTC triggers are defined in the SpeechDetectorConstants
   interface.

   When the MediaStreamContainer can store an audio and a video track,
   and the MediaGroup (containing the Recorder) has been configured with
   audio and video capabilities, then both tracks are recorded, with
   synchronisation.
     _________________________________________________________________

   Field Summary



   Fields inherited from interface
   javax.media.mscontrol.resource.ResourceConstants
   e_OK, FOREVER, q_Duration, q_RTC, q_Standard, q_Stop, rtcc_TriggerRTC,
   v_Forever



   Fields inherited from interface
   javax.media.mscontrol.mediagroup.RecorderConstants
   ev_Pause, ev_Record, ev_Resume, ev_Started, p_Append,
   p_AudioClockrate, p_AudioCoder, p_AudioFmtp, p_AudioMaxBitrate,
   p_BeepFrequency, p_BeepLength, p_EnabledEvents, p_FileFormat,
   p_MaxDuration, p_MinDuration, p_Prompt, p_SignalTruncationOn,
   p_SilenceTerminationOn, p_SpeechDetectionMode, p_StartBeep,
   p_StartPaused, p_VideoCoder, p_VideoFmtp, p_VideoMaxBitrate,
   q_PromptFailed, q_Silence, rtca_Pause, rtca_Resume, rtca_Stop,
   rtcc_RecordComplete, v_DetectAllOccurences, v_DetectFirstOccurence,
   v_Inactive



   Fields inherited from interface
   javax.media.mscontrol.mediagroup.CoderConstants
   v_3G2Format, v_3GPFormat, v_ADPCM_16kG726, v_ADPCM_32k,
   v_ADPCM_32kG726, v_ADPCM_32kOKI, v_ALawPCM_48k, v_ALawPCM_64k, v_AMR,
   v_AMR_WB, v_EVRC, v_G723_1b, v_G729a, v_GSM, v_GSMFormat, v_H263,
   v_H263_1998, v_H264, v_Inferred, v_Linear16Bit_128k,
   v_Linear16Bit_256k, v_Linear8Bit_64k, v_MP4V_ES, v_MuLawPCM_64k,
   v_RawFormat, v_WavFormat



   Method Summary
    void record(java.net.URI streamID, RTC[] rtc, Parameters optargs)
             Record data into the named Media Stream Container (MSC),
   designated by a java.net.URI class.



   Methods inherited from interface
   javax.media.mscontrol.resource.Resource
   getContainer, stop



   Methods inherited from interface
   javax.media.mscontrol.resource.MediaEventNotifier
   addListener, getMediaSession, removeListener



   Method Detail

  record

void record(java.net.URI streamID,
            RTC[] rtc,
            Parameters optargs)
            throws MscontrolException

          Record data into the named Media Stream Container (MSC),
          designated by a java.net.URI class.

          If the MSC does not exist, it is created. If the MSC does
          exist, the recording is made using the coder type associated
          with that MSC, in case of a WAV file, otherwise, the coder type
          specified by p_AudioCoder will be used. If the Recorder does
          not support the indicated Coder, this method throws a
          MediaResourceException, indicating Error.e_Coder.

          This method is non-blocking. When the recording is complete,
          the result is passed in a RecorderEvent event through the
          MediaEventListener Interface. The condition that caused
          recording to stop is available from the completion event using
          getQualifier.

          The optargs argument specifies values for Recorder parameters,
          overridden for a single method invocation. For example,
          p_AudioCoder or p_StartBeep. See RecorderConstants for more.

          Post-conditions: Record completes normally due to one of:

         1. Duration time reached
         2. A silence exceeding p_FinalTimeout or p_InitialTimeout is
            detected
         3. stop() method invoked
         4. RecorderConstants.rtca_Stop triggered

        Parameters:
                streamID - the URI where data is to be stored
                rtc - an Array of RTC (Run-Time-Control) objects
                optargs - a Parameters map of optional arguments, see
                RecorderConstants

        Throws:
                MscontrolException - if requested operation fails.

        See Also:
                onEvent({@link
                javax.media.mscontrol.mediagroup.RecorderEvent})
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
