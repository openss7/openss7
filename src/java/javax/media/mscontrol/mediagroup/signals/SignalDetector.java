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
Interface SignalDetector

   All Superinterfaces:
          MediaEventNotifier<SignalDetectorEvent>,
          Resource<ResourceContainer<? extends MediaConfig>>,
          ResourceConstants, SignalConstants, SignalDetectorConstants
     _________________________________________________________________

public interface SignalDetector

   extends Resource<ResourceContainer<? extends MediaConfig>>,
          SignalDetectorConstants,
          MediaEventNotifier<SignalDetectorEvent>

   Interface for detecting signals from the network. The SignalDetector
   can:
     * detect single DTMF signals
     * optionally emit an event for each signal detected
     * collect a sequence of signals into a buffer
     * match or detect patterns in the signal buffer
     * optionally emit an event for each pattern matched
     * remove signals from the signal buffer when a pattern is matched
     * remove all signals from the signal buffer
     * emit RTC triggers for signal and pattern detection
     * patterns can be either a simple string of DTMF signals, or an
       arbitrary complex grammar.
     * grammars can be dtmf grammars (mandatory), or voice grammars
       (SRGS), requiring ASR (optional feature)

   The details of the processing of a signal are explained in the
   Operation section.

    Components of a SignalDetector

   Conceptually, a SignalDetector has these components:
     * a signal detector, that detects DTMF or other signals.
     * a signal buffer, that holds some detected signals for later
       processing or analysis.
     * a collection of pattern matchers, that identify particular
       sequences of signals or voice, and generate RTC triggers and
       events.
     * Possibly, a speech detector, to support
       SpeechRecognitionEvent.v_StartOfSpeech.
     * Possibly, a speech recognizer to support voice grammars.

    SignalDetector states

   IDLE<==>DETECTING.

   In the DETECTING state, the SignalDetector listens for and identifies
   any of the defined DTMF and FAX signals. The SignalDetector state is
   controlled by the parameter p_Mode. By default, the value of p_Mode
   equals v_Detecting, and the SignalDetector is in the DETECTING state.

   In addition to the basic detection of signals controlled by p_Mode,
   the pattern matchers (each individually) are either activated or
   deactivated. A pattern matcher is activated when its pattern Symbol is
   used in any of the following ways:
     * as an RTC trigger in an active resource method,
     * as one of the elements in the value of p_EnabledEvents,
     * as one of the elements in the value of p_Filter,
     * as a terminating condition in receiveSignals() (that is, it
       appears in the patterns argument).

   Note: the pattern stays activated while the condition(s) that
   activated it remain in effect, then it is deactivated.

    Parameters

   Many operations of the SignalDetector are controlled by setting
   various parameters, using setParameters(). The relevant parameter
   Symbols are named SignalDetector.p_Something. The Symbols used to name
   parameters and other SignalDetector constants are defined in the
   SignalDetectorConstants interface.

    Pattern matchers

   The value of the read-only parameter p_PatternCount indicates the
   number of pattern matchers available, and the number of patterns that
   can be defined. The default value is 32. If the signal detector is not
   capable of pattern matching, then the value is zero. An application
   can request a minimum number of pattern matchers using the attribute
   a_PatternCount, which default is also 32.

   The ways to define a Pattern are described below.

   Note: Each of the 32 pattern matchers has an assigned Symbol. That
   symbol identifies the pattern as an event, as a parameter, as a
   qualifier or as a RTC trigger. Pattern Symbols may be designated by
   any of their names; the Pattern Symbol arrays
     * Symbol[] ev_Pattern
     * Symbol[] p_Pattern
     * Symbol[] q_Pattern
     * Symbol[] rtcc_Pattern

   are identical.

    Event delivery

   Event delivery is controlled by the parameter p_EnabledEvents, the
   value of which is an array of event Symbols identifying the events
   that are sent from the SignalDetector to the
   MediaEventListener.onEvent method. .

   The events that are controlled by p_EnabledEvents are:
     * ev_SignalDetected
     * ev_Pattern[i]
     * ev_Overflow

   Note 1: p_EnabledEvents controls the delivery of events to Listeners.
   The detection of signals, buffering, and pattern matching for RTC is
   independent of event delivery; detection is controlled by p_Mode and
   is (generally) always enabled.
   Note 2: ev_ReceiveSignals is not controlled by p_EnabledEvents: the
   receiveSignals operation always sends an event when it completes.

   The SignalDetector sends events of type SignalDetectorEvent, or its
   extension SpeechRecognitionEvent when matching a pattern defined by a
   voice grammar.

    RTC triggers

   The SignalDetector supports RTC triggers for:
     * rtcc_FlushBuffer
     * rtcc_SignalDetected
     * rtcc_RetrieveSignals
     * rtcc_Pattern[i] (the pattern Symbols)

   The SignalDetector triggers an RTC action (for example, to affect a
   play() or record()), only if the pattern is defined before requesting
   the RTC (that is, before doing the play() or record()). The
   SignalDetector does not properly "arm" an rtcc_Pattern[i] unless the
   p_Pattern[i] definition is already set.

    Signal buffer

   When individual signals are detected (and the value of p_Buffering is
   true), the signals are stored in the signal buffer. Signals are kept
   in the signal buffer until they are removed by one of
     * receiveSignals(),
     * flushBuffer(),
     * pattern filtering, or
     * signal buffer overflow.

   receiveSignals: receiveSignals(numSignals, ...)
   waits for any of the various terminating conditions and then removes
   and returns up to numSignals signals from the signal buffer.

   flushBuffer: flushBuffer() succeeds immediately and removes all
   signals from the buffer.

   Retrieving or flushing signals means those signals are no longer used
   for matching patterns. All pattern matchers are reset.

   Filtering: If a pattern is matched and that pattern Symbol appears in
   the value of p_Filter, then the signals that matched the pattern are
   removed from the signal buffer. Pattern matching is explained below.

   Overflow: The length of the signal buffer is available as the value of
   the read-only parameter p_BufferSize, and is at least 35 (long enough
   to hold any international telephone number). Applications can request
   support for a longer buffer using the attribute a_BufferSize.

   If p_BufferSize signals are in the signal buffer then the next signal
   causes overflow. Overflow processing is explained below.

   Note: When a signal is removed from the signal buffer, it is delivered
   to the application in the associated event. A signal appears in at
   most one such event. In addition to being in a buffer removal event, a
   signal may appear in at most one pattern matched event.

  Operation

   When the signal detector detects a signal, the processing is as
   follows:
    1. if p_Buffering is true, then the signal is added to the signal
       buffer.
    2. rtcc_SignalDetected triggers any applicable RTCs.
    3. if p_EnabledEvents contains ev_SignalDetected, then
       MediaEventListener.onEvent(event) is invoked
    4. if the signal buffer overflows then overflow processing is done.
    5. if pattern matchers are enabled, then pattern matching is done.
    6. receiveSignals may be terminated by a pattern match.
    7. the NumSignals signal counter is incremented and receiveSignals
       may be terminated with q_NumSignals.

    Overflow processing

   If the signal buffer contains p_BufferSize signals, then the next
   signal causes overflow.

   When overflow occurs, some signal must be discarded. The Boolean value
   of parameter p_DiscardOldest indicates whether the oldest signal
   should be removed from the signal buffer and discarded, or whether the
   current, new signal should be discarded. Signals that are discarded
   are included in the ev_Overflow event, but are not included in the
   signal buffer returned by receiveSignals().

   When the signal buffer overflows, the processing is as follows:
    1. if the value of p_DiscardOldest is true, then the oldest signal is
       discarded.
    2. if the value of p_DiscardOldest is false, then the current signal
       is discarded.
    3. if the value of p_EnabledEvents contains the symbol ev_Overflow,
       then MediaEventListener. onEvent(SignalDetectorEvent) method is
       invoked and the event contains the discarded signal.

   Note: even if the current signal is discarded from the signal buffer,
   it is still used for pattern matching. However, if pattern matching is
   enabled after a signal has been discarded, it is not used to
   initialize the pattern matchers.

    Pattern matching

   A pattern matcher is activated when its pattern Symbol is used in any
   of these ways:
     * as an RTC trigger in an active resource method,
     * as one of the elements in the value of p_EnabledEvents,
     * as one of the elements in the value of p_Filter,
     * as a terminating condition in receiveSignals() (that is, it
       appears in the patterns argument).

   When a pattern matcher is activated by one of the conditions above, it
   is initialized by scanning the current signal buffer contents. This
   may generate one or more pattern matching events.
     * Each signal in the signal buffer is passed in turn to the [newly
       activated] pattern matcher(s).

   Note: This procedure is called scanning the buffer. Then intent of
   scanning is that the new pattern matchers match patterns as if the
   matcher had been activated before the signals entered the buffer. This
   step is to handle any typeahead that may have come before before the
   pattern(s) were activated. A pattern matcher scans the buffer only
   when it is newly activated. The application should use p_Filter and
   flushBuffer carefully to get the desired effects.

   When a signal is passed to a pattern matcher, this procedure is
   followed:
    1. If the signal contributes to a match for this pattern matcher,
       then the patterm matching state machine is advanced to record
       that.
    2. If the signal does not contribute to a match for this pattern
       matcher, then the pattern matching state machine resets to record
       that.
    3. If the signal brings the pattern matcher for pattern[i] to its
       matched state, then then do 4-7:
    4. If this patterSignalDetectorEventn is activated as an RTC trigger,
       then RTC trigger rtcc_pattern[i] is generated
    5. If the value of p_EnabledEvents contains ev_Pattern[i] then
       onPatternMatched(event) is invoked on all SignalDetector
       listeners, and the event contains the signals that matched the
       pattern.
    6. If the value of p_Filter contains p_Pattern[i] then the matching
       signals are removed from the signal buffer and all pattern
       matchers are reset to their initial (no input) state and this
       signal is not considered by subsequent pattern matchers.
    7. If the value of the patterns argument to receiveSignals contains
       p_Pattern[i] then receiveSignals terminates with qualifier
       q_Pattern[i] and returns the current contents of the signal
       buffer.

   Note: The intent of step 6 is to ensure that a sequence of signals
   that is filtered does not contribute to a match in subsequent
   Patterns. "subsequent" means patterns with index greater than [i].

  How to define a Pattern

   Patterns can be defined in three ways:
     * by a java.lang.String of characters, among 0,1,2,3,4,5,6,7,8,9,*,#,A,B,C,D
       For example: "2" will match a single DTMF key, and "04#" will
       match the sequence of 3 keys.
       patterns.put(p_Pattern[1], "2");
     * by a URL referencing a SRGS grammar.
       For example: patterns.put(p_Pattern[3], new
       URL("http://server.com/grammar.grxml#rule1");
       The data URI scheme (see RFC 2397) can be used to inline simple
       grammars:
patterns.put(p_Pattern[3], URI.create("data:application/srgs+xml,"+
                                       "<?xml version=\"1.0\"?>"+
                                          "grammar mode=\"dtmf\">"+
                                              "<rule scope=\"public\">"+
                                                "<item repeat=\"4\">"+
                                                  "<one-of>"+
                                                    "<item> 0 </item>"+
                                                    "<item> 1 </item>"+
                                                    "<item> 2 </item>"+
                                                  "</one-of>"+
                                                "</item>"+
                                              "</rule>"+
                                           "</grammar>");
       SRGS grammars in "voice" mode involve Automatic Speech
       Recognition. This requires a speech recognition resource in the
       MediaConfig.
     * by a Symbol indicating the start of speech, see
       SpeechRecognitionEvent.v_StartOfSpeech, or the end of speech, see
       SpeechRecognitionEvent.v_EndOfSpeech.
       This requires a speech detection resource in the MediaConfig (not
       necessarily a speech recognizer).

   Recognition starts when the pattern is activated, and there is no
   buffering of previous voice samples.

   The Pattern parameters can be set with setParameters or in the optargs
   argument of receiveSignals. Each pattern is set as the value of one of
   the pattern parameter symbols (p_Pattern[i]).

   Example 1: define some patterns
 params.put(p_Pattern[1], "1"); // first menu entry
 params.put(p_Pattern[2], "2"); // second menu entry
 params.put(p_Pattern[3], "##"); // route to live operator
 params.put(p_Pattern[4], new URL("http://server.com/grammar.grxml#account_numb
er)); // Accnt number
 myMG.setParameters(params);


   Example 2: enable patterns for event callbacks:
 Symbol[] patternList = { p_Pattern[1], p_Pattern[2], p_Pattern[3] };
 params.put(SignalDetector.p_EnabledEvents, patternList);
 myMG.setParameters(params);


   Example 3: enable patterns and wait for a match:
 Symbol[] patternSetTwo = { p_Pattern[3], p_Pattern[4] };
 receiveSignals(-1, patternSetTwo, null, null);


   For an extensive example, see the "voicemail" code sample.
     _________________________________________________________________

   Field Summary



   Fields inherited from interface
   javax.media.mscontrol.resource.ResourceConstants
   e_OK, FOREVER, q_Duration, q_RTC, q_Standard, q_Stop, rtcc_TriggerRTC,
   v_Forever



   Fields inherited from interface
   javax.media.mscontrol.mediagroup.signals.SignalDetectorConstants
   ev_FlushBuffer, ev_Overflow, ev_Pattern, ev_ReceiveSignals,
   ev_SignalDetected, p_Buffering, p_BufferSize, p_DiscardOldest,
   p_DtmfDetectorMode, p_Duration, p_EnabledEvents, p_Filter,
   p_InitialTimeout, p_InterSigTimeout, p_Mode, p_Pattern,
   p_PatternCount, p_Prompt, q_Duration, q_InitialTimeout,
   q_InterSigTimeout, q_NumSignals, q_Pattern, q_PromptFailed,
   rtca_EvalBuffer, rtca_FlushBuffer, rtca_Stop, rtcc_FlushBuffer,
   rtcc_Pattern, rtcc_ReceiveSignals, rtcc_SignalDetected, v_Detecting,
   v_Idle



   Fields inherited from interface
   javax.media.mscontrol.mediagroup.signals.SignalConstants
   p_SymbolChar, v_CED, v_CNG, v_DTMF0, v_DTMF1, v_DTMF2, v_DTMF3,
   v_DTMF4, v_DTMF5, v_DTMF6, v_DTMF7, v_DTMF8, v_DTMF9, v_DTMFA,
   v_DtmfAuto, v_DTMFB, v_DTMFC, v_DTMFD, v_DTMFHash, v_DtmfInBand,
   v_DtmfInBandOutBand, v_DtmfNone, v_DtmfOutBand, v_DTMFStar,
   v_VFURequest



   Method Summary
    void flushBuffer()
             Remove all signals from the signal buffer.
    void receiveSignals(int numSignals, Symbol[] patterns, RTC[] rtc,
   Parameters optargs)
             Retrieve some signals from the signal buffer.



   Methods inherited from interface
   javax.media.mscontrol.resource.Resource
   getContainer, stop



   Methods inherited from interface
   javax.media.mscontrol.resource.MediaEventNotifier
   addListener, getMediaSession, removeListener



   Method Detail

  receiveSignals

void receiveSignals(int numSignals,
                    Symbol[] patterns,
                    RTC[] rtc,
                    Parameters optargs)
                    throws MscontrolException

          Retrieve some signals from the signal buffer. The operation is
          complete when numSignals signals are in the signal buffer (the
          operation removes them). May return with fewer than numSignals
          when some other terminating condition occurs (rtca_Stop, a
          Pattern is detected, or timeout). In that case, the operation
          terminates and removes all the signals from the signal buffer.
          If (numSignals == -1), then this operation terminates only when
          some other termination condition occurs.

          The terminating conditions are controlled by the arguments
          patterns and rtc, and various timeout parameters.

          patterns is an array of Symbols, with an entry for each Pattern
          to enable. If the patterns array contains a pattern Symbol then
          recognition of that pattern is treated as a terminating
          condition for receiveSignals(). If patterns is null, then
          pattern recognition is not considered as a terminating
          condition.

          receiveSignals also terminates if any of the p_InitialTimeout,
          p_InterSigTimeout, p_Duration timeouts are exceeded. These
          times are relative to the start of the receiveSignals
          operation. These parameters can be set with setParameters or in
          the optargs argument.

          When the operation completes, a SignalDetectorEvent is sent
          through the MediaEventListener.
          In this event, getEventID() returns
          SignalDetectorConstants.ev_ReceiveSignals.
          The received signals are returned by getSignalString() or
          getSignalBuffer().
          If the operation was terminated because a pattern matched, then
          getQualifier() returns q_Pattern[i].

          If the matching pattern is defined by a voice grammar, then the
          event is extended to a SpeechRecognitionEvent.

        Parameters:
                numSignals - the int number of signals to be retrieved.
                patterns - indicates which Patterns should terminate
                retrieval.
                rtc - an Array of RTC (Run Time Control) objects. The RTC
                actions may be rtca_Stop or rtca_FlushBuffer.
                optargs - a Parameters map of optional arguments.
                p_Pattern[i] may be set as transient parameters.

        Throws:
                MediaResourceException - if this request fails; usually
                because of a badly formed request, or implementation
                cannot process the request.
                MscontrolException

        See Also:
                MediaEventListener
     _________________________________________________________________

  flushBuffer

void flushBuffer()
                 throws MscontrolException

          Remove all signals from the signal buffer.

          This prevents buffer overflow, and synchronizes the pattern
          matchers.

          The signals flushed may be accessed using
          SignalDetectorEvent.getSignalBuffer() or
          SignalDetectorEvent.getSignalString(). If the flushed signals
          are not available, then these methods return null.

          This is conceptually equivalent to:
          receiveSignals((p_BufferSize), null, null, {p_Duration=0});

          Note: flushBuffer is also available using rtca_FlushBuffer.

        Throws:
                MscontrolException - if this request fails.

        See Also:
                MediaEventListener
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
