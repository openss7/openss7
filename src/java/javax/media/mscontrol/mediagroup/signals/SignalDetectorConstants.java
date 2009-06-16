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

javax.media.mscontrol.mediagroup.signals
Interface SignalDetectorConstants

   All Superinterfaces:
          SignalConstants

   All Known Subinterfaces:
          SignalDetector
     _________________________________________________________________

public interface SignalDetectorConstants

   extends SignalConstants

   The Symbols used by the SignalDetector Resource and
   SignalDetectorEvent objects.

   The Symbols that name DTMF, CNG and CED signals are inherited from
   SignalConstants.
     _________________________________________________________________

   Field Summary
   static Symbol ev_FlushBuffer
             FlushBuffer completion event.
   static Symbol ev_Overflow
             The SignalDetector signal buffer has overflowed.
   static Symbol[] ev_Pattern
             Array of pattern Symbols, indicating a pattern matched
   event.
   static Symbol ev_ReceiveSignals
             Completion event for receiveSignals.
   static Symbol ev_SignalDetected
             A single Signal has been detected.
   static Symbol p_Buffering
             Enable/disable buffering of signals.
   static Symbol p_BufferSize
             Size of the signal buffer.
   static Symbol p_DiscardOldest
             Boolean that indicates whether the oldest signals are
   discarded when the signal buffer overflows.
   static Symbol p_DtmfDetectorMode
             DTMF detection mode.
   static Symbol p_Duration
             Maximum time duration for retrieving signals in
   milliseconds.
   static Symbol p_EnabledEvents
             An array of SignalGenerator event Symbols, indicating which
   events are generated and delivered to the application.
   static Symbol p_Filter
             Enable/disable pattern filtering.
   static Symbol p_InitialTimeout
             Maximum time limit for first signal.
   static Symbol p_InterSigTimeout
             Maximum time limit between consecutive signals.
   static Symbol p_Mode
             Enable/disable the signal detector.
   static Symbol[] p_Pattern
             Each p_Pattern[i] defines a pattern to be matched.
   static Symbol p_PatternCount
             Number of pattern definitions supported by this
   SignalDetector resource.
   static Symbol p_Prompt
             Indicates a prompt to be played before the signal detection
   starts.
   static Symbol q_Duration
             Complete because total duration timeout (p_Duration)
   exceeded.
   static Symbol q_InitialTimeout
             Complete because initial signal timeout (p_InitialTimeout)
   exceeded.
   static Symbol q_InterSigTimeout
             Complete because inter signal timeout (p_InterSigTimeout)
   exceeded.
   static Symbol q_NumSignals
             Complete because requested number of signals detected.
   static Symbol[] q_Pattern
             The array of Pattern Symbols which appear as qualifiers.
   static Symbol q_PromptFailed
             Returned by SignalDetectorEvent.getQualifier() to indicate
   that the receiveSignals operation was aborted because the prompt could
   not be played.
   static Symbol rtca_EvalBuffer
             RTC Action to cause the current buffer to be parsed again
   according to the last receiveSignals, play or record RTC.
   static Symbol rtca_FlushBuffer
             RTC Action to cause the buffer to be flushed.
   static Symbol rtca_Stop
             RTC Action to cause the current operation to stop.
   static Symbol rtcc_FlushBuffer
             Buffer has been flushed.
   static Symbol[] rtcc_Pattern
             The rtcc_Pattern[i] is triggered when the corresponding
   ev_Pattern[i] event was issued.
   static Symbol rtcc_ReceiveSignals
             The receiveSignals transaction has completed.
   static Symbol rtcc_SignalDetected
             Some signal was detected.
   static Symbol v_Detecting
             value for p_Mode.
   static Symbol v_Idle
             value for p_Mode.



   Fields inherited from interface
   javax.media.mscontrol.mediagroup.signals.SignalConstants
   p_SymbolChar, v_CED, v_CNG, v_DTMF0, v_DTMF1, v_DTMF2, v_DTMF3,
   v_DTMF4, v_DTMF5, v_DTMF6, v_DTMF7, v_DTMF8, v_DTMF9, v_DTMFA,
   v_DtmfAuto, v_DTMFB, v_DTMFC, v_DTMFD, v_DTMFHash, v_DtmfInBand,
   v_DtmfInBandOutBand, v_DtmfNone, v_DtmfOutBand, v_DTMFStar,
   v_VFURequest



   Field Detail

  ev_FlushBuffer

static final Symbol ev_FlushBuffer

          FlushBuffer completion event. getSignalBuffer returns the
          signals that were discarded or null if that information is not
          available.
     _________________________________________________________________

  ev_Overflow

static final Symbol ev_Overflow

          The SignalDetector signal buffer has overflowed. A
          non-transactional event is delivered to
          MediaEventListener.onEvent(javax.media.mscontrol.resource.Media
          Event)

          getSignalBuffer returns the signal that has been discarded or
          null if that information is not available.
     _________________________________________________________________

  ev_Pattern

static final Symbol[] ev_Pattern

          Array of pattern Symbols, indicating a pattern matched event.
          An event with one of these eventIDs is delivered to
          MediaEventListener.onEvent(javax.media.mscontrol.resource.Media
          Event)

          The eventID indicates which of the patterns was matched.
          getSignalBuffer returns the signals that matched the pattern.

        See Also:
                SignalDetectorEvent.getPatternIndex()
     _________________________________________________________________

  ev_ReceiveSignals

static final Symbol ev_ReceiveSignals

          Completion event for receiveSignals. getSignalBuffer returns
          the received signals.
     _________________________________________________________________

  ev_SignalDetected

static final Symbol ev_SignalDetected

          A single Signal has been detected. getSignalBuffer()[0] returns
          the signal that has been detected or null if that information
          is not available (has been flushed or p_Buffering is set to
          true).
     _________________________________________________________________

  p_Buffering

static final Symbol p_Buffering

          Enable/disable buffering of signals.

          valid values: true and false. true enables buffering, false
          disables buffering.
          Default value: true.

          Pattern matching requires that this parameter be true.
     _________________________________________________________________

  p_BufferSize

static final Symbol p_BufferSize

          Size of the signal buffer. Read-only parameter containing the
          number of signals that can be stored in the signal buffer.
     _________________________________________________________________

  p_DiscardOldest

static final Symbol p_DiscardOldest

          Boolean that indicates whether the oldest signals are discarded
          when the signal buffer overflows.

          Valid values: true and false.
          Default value: false.

        Since:
                Not supported
     _________________________________________________________________

  p_EnabledEvents

static final Symbol p_EnabledEvents

          An array of SignalGenerator event Symbols, indicating which
          events are generated and delivered to the application. If a
          given eventID is not enabled, then no processing is expended to
          generate or distribute that kind of event.

          Value is an array of non-transactional event Symbols. Valid
          event Symbols are:

          + ev_Overflow.
          + ev_Pattern[i]
          + ev_SignalDetected.

          (ev_ReceiveSignals is always sent and cannot be disabled).
          For a MediaEventListener to receive events, the events must be
          enabled by setting this parameter. This parameter can be set by
          setParameters.

          Note: This parameter controls the generation of events. The
          events cannot be delivered unless a MediaEventListener is added
          using addListener()
     _________________________________________________________________

  p_Filter

static final Symbol p_Filter

          Enable/disable pattern filtering.

          Value of this parameter is an Array of ev_Pattern[i] Symbols.
          When a Pattern in the Array is matched, the matched signals are
          removed from the signal buffer.
     _________________________________________________________________

  p_Duration

static final Symbol p_Duration

          Maximum time duration for retrieving signals in milliseconds.
          Stop receiveSignals after this time limit.

          Valid values: an int [0, 100000] milliseconds or FOREVER.
          Default value: FOREVER
     _________________________________________________________________

  p_InitialTimeout

static final Symbol p_InitialTimeout

          Maximum time limit for first signal. Stop receiveSignals if the
          first signal is not detected within this many milliseconds.

          Valid values: an Integer [0, 100000] or FOREVER which indicates
          no limit.
          Default value: FOREVER.
     _________________________________________________________________

  p_InterSigTimeout

static final Symbol p_InterSigTimeout

          Maximum time limit between consecutive signals. Stop
          receiveSignals if another signal is not detected within this
          many milliseconds after detecting the previous signal.

          Valid values: an Integer [0, 100000] or FOREVER which indicates
          no limit.
          Default value: FOREVER.
     _________________________________________________________________

  p_Mode

static final Symbol p_Mode

          Enable/disable the signal detector. Setting mode to v_Idle
          disables the signal detector while keeping it allocated.
          Setting mode to v_Active enable the signal detector.

          Caution:

          setting this value may reset p_Pattern

          Valid values: v_Idle or v_Detecting.
          Default value: v_Detecting
     _________________________________________________________________

  v_Idle

static final Symbol v_Idle

          value for p_Mode.
     _________________________________________________________________

  v_Detecting

static final Symbol v_Detecting

          value for p_Mode.
     _________________________________________________________________

  p_DtmfDetectorMode

static final Symbol p_DtmfDetectorMode

          DTMF detection mode. Specifies which mode is used to detect the
          DTMF

          valid values: v_DtmfInBand , v_DtmfOutBand,
          v_DtmfInBandOutBand, v_DtmfAuto
     _________________________________________________________________

  p_Pattern

static final Symbol[] p_Pattern

          Each p_Pattern[i] defines a pattern to be matched. An array of
          Pattern Symbols to be used as keys in a Parameters map.

          For each p_Pattern[i], the valid value is either a string, or a
          URI defining a grammar, see pattern definition

          These parameters may be set in setParameters, or the optargs
          argument of receiveSignals.

          Note: In this release, these parameters cannot be used in the
          map of customisation parameters of
          MediaConfig.createCustomizedClone(Parameters)
     _________________________________________________________________

  p_PatternCount

static final Symbol p_PatternCount

          Number of pattern definitions supported by this SignalDetector
          resource. For p_Pattern[i], i ranges from [0, p_PatternCount].

          This is a read-only parameter. The value is implementation
          dependent.
     _________________________________________________________________

  p_Prompt

static final Symbol p_Prompt

          Indicates a prompt to be played before the signal detection
          starts.
          The expected value is a URI, or URI[], to the prompt media.
          This parameter transforms the receiveSignals operation into a
          "prompt-and-collect" feature.
          The Player is used for playing the prompt, and Player controls
          (through RTCs) are active during the prompt phase; However no
          PlayerEvent will be sent when the prompt completes, and
          stopping the receiveSignals will also stop the playing, if
          applicable.
          Other Player parameters can be given, for example
          PlayerConstants.p_AudioCoder.
          If the prompt cannot be played, the whole operation is
          terminated with error.
          If the Player is busy, the behavior described by
          PlayerConstants.p_IfBusy is applied.
          Additionally, barge-in (allowing the user to interrupt the
          prompt and start recording immediately) can be enabled with the
          bargeIn RTC.

          Example: Prompt with "http://server.com/prompt.3gp", and
          collect 4 DTMFs, barge-in allowed :

 Parameters options = myMediaSessionFactory.createParameters();
 options.put(SignalDetectorConstants.p_Prompt, URI.create("http://server.com/pr
ompt.3gp"));

 mySignalDetector.receiveSignals(4, null, RTC.bargeIn, options);

     _________________________________________________________________

  q_Pattern

static final Symbol[] q_Pattern

          The array of Pattern Symbols which appear as qualifiers.

          q_Pattern[i] is the qualifier in ev_ReceiveSignals when
          receiveSignals terminates because that pattern was matched. The
          actual signals that matched the pattern are available using
          getSignalBuffer.
     _________________________________________________________________

  q_Duration

static final Symbol q_Duration

          Complete because total duration timeout (p_Duration) exceeded.
     _________________________________________________________________

  q_InitialTimeout

static final Symbol q_InitialTimeout

          Complete because initial signal timeout (p_InitialTimeout)
          exceeded.
     _________________________________________________________________

  q_InterSigTimeout

static final Symbol q_InterSigTimeout

          Complete because inter signal timeout (p_InterSigTimeout)
          exceeded.
     _________________________________________________________________

  q_NumSignals

static final Symbol q_NumSignals

          Complete because requested number of signals detected.
     _________________________________________________________________

  q_PromptFailed

static final Symbol q_PromptFailed

          Returned by SignalDetectorEvent.getQualifier() to indicate that
          the receiveSignals operation was aborted because the prompt
          could not be played.
          See getError() and getErrorText() for more.
     _________________________________________________________________

  rtca_FlushBuffer

static final Symbol rtca_FlushBuffer

          RTC Action to cause the buffer to be flushed.
     _________________________________________________________________

  rtca_EvalBuffer

static final Symbol rtca_EvalBuffer

          RTC Action to cause the current buffer to be parsed again
          according to the last receiveSignals, play or record RTC.

        Since:
                Not supported
     _________________________________________________________________

  rtca_Stop

static final Symbol rtca_Stop

          RTC Action to cause the current operation to stop.
     _________________________________________________________________

  rtcc_SignalDetected

static final Symbol rtcc_SignalDetected

          Some signal was detected.
     _________________________________________________________________

  rtcc_FlushBuffer

static final Symbol rtcc_FlushBuffer

          Buffer has been flushed.
     _________________________________________________________________

  rtcc_ReceiveSignals

static final Symbol rtcc_ReceiveSignals

          The receiveSignals transaction has completed.
     _________________________________________________________________

  rtcc_Pattern

static final Symbol[] rtcc_Pattern

          The rtcc_Pattern[i] is triggered when the corresponding
          ev_Pattern[i] event was issued.
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
package javax.jain.media.mscontrol.mediagroup.signals;
