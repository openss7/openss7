/*
 @(#) $RCSfile: SpeechDetectorConstants.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-02-07 07:27:10 $ <p>
 
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
Interface SpeechDetectorConstants

   All Known Subinterfaces:
          RecorderEvent
     _________________________________________________________________

public interface SpeechDetectorConstants

   Defines the Symbols used by SpeechDetector and RecorderEvent.
     _________________________________________________________________

   Field Summary
   static Symbol p_BargeIn
             Controls whether the caller can start speaking before
   prompts have ended.
   static Symbol p_FinalTimeout
             Controls the length of a period of silence after callers
   have spoken to conclude they finished.
   static Symbol p_InitialTimeout
             Controls how long the recognizer should wait after the end
   of the prompt for the caller to speak before sending a Recorder event
   (ev_Record, q_NoSpeechTimeout, null, e_OK).
   static Symbol p_Sensitivity
             Sensitivity of the speech detector when looking for speech.
   static Symbol q_EndOfSpeechDetected
             SpeechDetector stopped because end of speech has been
   detected.
   static Symbol q_NoSpeechTimeout
             SpeechDetector stopped because no speech has been detected
   before the initial timout popped.
   static Symbol q_SpeechDetected
             SpeechDetector detected speech.
   static Symbol rtca_PromptDone
             Advise the Speech Detector that the prompt is finished.
   static Symbol rtcc_EndOfSpeechDetected
             Trigger when end of speech has been detected (ie. final
   timeout popped).
   static Symbol rtcc_NoSpeechTimeout
             Trigger when no speech has been detected before the initial
   timout popped.
   static Symbol rtcc_SpeechDetected
             Trigger when speech has been detected.



   Field Detail

  rtca_PromptDone

static final Symbol rtca_PromptDone

          Advise the Speech Detector that the prompt is finished. A
          signal is more likely to occur.

          Default : managed internally.
     _________________________________________________________________

  p_InitialTimeout

static final Symbol p_InitialTimeout

          Controls how long the recognizer should wait after the end of
          the prompt for the caller to speak before sending a Recorder
          event (ev_Record, q_NoSpeechTimeout, null, e_OK).

          This parameter only takes effect after the speech detector has
          been informed that the prompt is done (managed internally).

          Value is an Integer.
          Default: 7000 ms
          Range: 0 to 19999 (ms)

        See Also:
                q_NoSpeechTimeout, rtcc_NoSpeechTimeout
     _________________________________________________________________

  p_FinalTimeout

static final Symbol p_FinalTimeout

          Controls the length of a period of silence after callers have
          spoken to conclude they finished.

          Value is an Integer number of milliseconds.
          Default: 4000 ms
          Range: 0 to 9999 (ms)
     _________________________________________________________________

  p_BargeIn

static final Symbol p_BargeIn

          Controls whether the caller can start speaking before prompts
          have ended.

          Value is a Boolean.
          Default: Boolean.TRUE
     _________________________________________________________________

  p_Sensitivity

static final Symbol p_Sensitivity

          Sensitivity of the speech detector when looking for speech.

          Higher values (more sensitive) tend to detect people who speak
          quietly, but risk false detection of background noise as
          speech. Lower values (less sensitive) tend to avoid accidently
          confusing background noise as speech, but may require some
          callers to speak more loudly.

          Value is a Float.
          Default: 0.5
          Range: 0 to 1
     _________________________________________________________________

  q_NoSpeechTimeout

static final Symbol q_NoSpeechTimeout

          SpeechDetector stopped because no speech has been detected
          before the initial timout popped.

        See Also:
                p_InitialTimeout
     _________________________________________________________________

  q_SpeechDetected

static final Symbol q_SpeechDetected

          SpeechDetector detected speech.
     _________________________________________________________________

  q_EndOfSpeechDetected

static final Symbol q_EndOfSpeechDetected

          SpeechDetector stopped because end of speech has been detected.

        See Also:
                p_FinalTimeout
     _________________________________________________________________

  rtcc_NoSpeechTimeout

static final Symbol rtcc_NoSpeechTimeout

          Trigger when no speech has been detected before the initial
          timout popped.

        See Also:
                p_InitialTimeout
     _________________________________________________________________

  rtcc_SpeechDetected

static final Symbol rtcc_SpeechDetected

          Trigger when speech has been detected.
     _________________________________________________________________

  rtcc_EndOfSpeechDetected

static final Symbol rtcc_EndOfSpeechDetected

          Trigger when end of speech has been detected (ie. final timeout
          popped).

        See Also:
                p_FinalTimeout
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
