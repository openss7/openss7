/*
 @(#) $RCSfile: RecorderEvent.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:10 $ <p>
 
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
Interface RecorderEvent

   All Superinterfaces:
          CoderConstants, MediaEvent<Recorder>, RecorderConstants,
          ResourceEvent<Recorder>, SpeechDetectorConstants
     _________________________________________________________________

public interface RecorderEvent

   extends ResourceEvent<Recorder>, RecorderConstants,
          SpeechDetectorConstants

   Defines the methods used in Recorder events.

   Recorder events include when an RTC triggers a pause or resume.
     _________________________________________________________________

   Field Summary



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



   Fields inherited from interface
   javax.media.mscontrol.mediagroup.SpeechDetectorConstants
   p_BargeIn, p_FinalTimeout, p_InitialTimeout, p_Sensitivity,
   q_EndOfSpeechDetected, q_NoSpeechTimeout, q_SpeechDetected,
   rtca_PromptDone, rtcc_EndOfSpeechDetected, rtcc_NoSpeechTimeout,
   rtcc_SpeechDetected



   Method Summary
    int getDuration()
             Returns the length of the recording, in milliseconds.



   Methods inherited from interface
   javax.media.mscontrol.resource.ResourceEvent
   getQualifier, getRTCTrigger



   Methods inherited from interface
   javax.media.mscontrol.resource.MediaEvent
   getError, getErrorText, getEventID, getSource



   Method Detail

  getDuration

int getDuration()

          Returns the length of the recording, in milliseconds. This
          length does not include any elided silence. For pause and
          resume events, this value is minus one (-1).
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
