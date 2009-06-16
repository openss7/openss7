/*
 * SignalDetector.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
 * ECTF S.410-R2 Source code distribution.
 *
 * Copyright (c) 2002, Enterprise Computer Telephony Forum (ECTF),
 * All Rights Reserved.
 *
 * Use and redistribution of this file is subject to a License.
 * For terms and conditions see: javax/telephony/media/LICENSE.HTML
 * 
 * In short, you can use this source code if you keep and display
 * the ECTF Copyright and the License conditions. The code is supplied
 * "AS IS" and ECTF disclaims all warranties and liability.
 */

package javax.telephony.media;

import java.util.Dictionary;

/**
 * Interface for detecting signals from the telephony network.
 * <p>
 * The SignalDetector can:
 * <ul>
 * <li>detect single DTMF signals</li>
 * <li>optionally emit an event for each signal detected</li>
 * <li>collect a sequence of signals into a buffer</li>
 * <li>match or detect patterns in the signal buffer</li>
 * <li>optionally emit an event for each pattern matched</li>
 * <li>remove signals from the signal buffer when a pattern is matched</li>
 * <li>remove all signals from the signal buffer</li>
 * <li>emit RTC triggers for signal and pattern detection</li>
 * </ul>
 * <p>
 * The details of the processing a signal are explained 
 * in the <a href="#Operation">Operation</a> section.
 * <p>
 *
 * <h4>Components of a SignalDetector </h4>
 * Conceptually, a SignalDetector has these components:
 * <ul>
 * <li> a signal detector, that detects DTMF or other signals.</li>
 * <li> a signal buffer, that holds some detected signals for 
 * later processing or analysis.</li>
 * <li> a collection of pattern matchers, that identify particular
 * sequences of signals, and generate RTC triggers and events.</li>
 * </ul>
 *
 * <h4>SignalDetector states</h4>
 * IDLE<==>DETECTING.
 * <p>
 * In the DETECTING state, the SignalDetector listens for and identifies
 * any of the defined DTMF and FAX signals.
 * The SignalDetector state is controlled by the parameter 
 * {@link SignalDetectorConstants#p_Mode p_Mode}.
 * By default, the value of <code>p_Mode</code> 
 * equals <code>v_Detecting</code>, and
 * the SignalDetector is in the DETECTING state.
 * <p>
 * In addition to the basic detection of signals controlled by <code>p_Mode</code>,
 * the pattern matchers (each individually) are either
 * activated or deactivated. A pattern matcher is <b>activated</b> 
 * when its pattern Symbol is used in any of the following ways:
 * <ul>
 * <li> as an RTC trigger in an active resource method, </li>
 * <li> as one of the elements in the value of <tt>p_EnabledEvents</tt>, </li>
 * <li> as one of the elements in the value of <tt>p_Filter</tt>, </li>
 * <li> as a terminating condition in <code>retrieveSignals()</code>
 * <br> (that is, it appears in the <tt>patterns</tt> argument).</li>
 * </ul>
 * <b>Note:</b> the pattern stays activated while the condition(s) that 
 * activated it remain in effect, then it is deactivated.
 * <p>
 *
 * <h4>Parameters</h4>
 * Many operations of the SignalDetector are controlled by setting various
 * parameters, using <code>setParameters()</code>. The relevant
 * parameter Symbols are named <code>SignalDetector.p_<i>Something</i></code>.
 * The Symbols used to name parameters and other SignalDetector constants
 * are defined in the {@link SignalDetectorConstants} interface.
 * <p>
 *
 * <h4>Pattern matchers</h4>
 * <p>
 * The value of the read-only parameter <code>p_PatternCount</code> 
 * indicates the number of pattern matchers available,
 * and the number of patterns that can be defined.
 * The default value is 32.
 * If the signal detector is not capable of pattern matching,
 * then the value is zero.
 * An application can request a minimum number of pattern matchers
 * using the attribute <code>a_PatternCount</code>, which default
 * is also 32.
 * <p> 
 * The <a href="#pattern_strings">format of the Strings
 * that define a Pattern</a> are described below.
 * <p>
 * <b>Note:</b> 
 * Each of the 32 pattern matchers has an assigned Symbol.
 * That symbol identifies the pattern as an event, as a parameter,
 * as a qualifier or as a RTC trigger.
 * Pattern Symbols may be designated by any of their names;
 * the Pattern Symbol arrays
 * <ul>
 * <li><code>Symbol[]  ev_Pattern</code></li>
 * <li><code>Symbol[]   p_Pattern</code></li>
 * <li><code>Symbol[]   q_Pattern</code></li>
 * <li><code>Symbol[] rtcc_Pattern</code></li>
 * </ul>
 * are identical.
 * 
 * <h4>Event delivery</h4> 
 * Event delivery is controlled by the parameter <code>p_EnabledEvents</code>,
 * the value of which is a Dictionary of event Symbols
 * (mapped to <tt>Boolean.TRUE</tt>)
 * identifying the events that are sent from the
 * SignalDetector to the {@link SignalDetectorListener}.
 * <p>
 * The events that are controlled by <code>p_EnabledEvents</code> 
 * and the respective SignalDetectorListener methods are:
 * <table border="1" cellpadding="3">
 * <tr><td>Event</td><td>Method</td></tr>
 * <tr><td><code>ev_SignalDetected</code></td><td>onSignalDetected</td></tr>
 * <tr><td><code>ev_Pattern[<b>i</b>]</code></td><td>onPatternMatched</td></tr>
 * <tr><td><code>ev_Overflow</code></td><td>onOverflow</td></tr>
 * </table>
 * <p>
 * <b>Note:</b>
 * <code>p_EnabledEvents</code> controls the <i>delivery</i> of events 
 * to Listeners. The <i>detection</i> of signals, buffering, and pattern 
 * matching for RTC is independent of event delivery; detection is 
 * controlled by <code>p_Mode</code> and is (generally) always enabled.
 * <p>
 *
 * <h4>RTC triggers</h4>
 * <p>
 * The SignalDetector supports RTC triggers for: <ul>
 * <li><code>rtcc_FlushBuffer</code></li>
 * <li><code>rtcc_SignalDetected</code></li>
 * <li><code>rtcc_RetrieveSignals</code></li>
 * <li><code>rtcc_Pattern[<b>i</b>]</code> (the pattern Symbols)</li>
 * </ul>
 * <p>
 * The SignalDetector triggers an RTC action
 * (for example, to affect a <code>play()</code> or <code>record()</code>), 
 * only if the pattern is defined <i>before</i> requesting the RTC
 * (that is, before doing the <code>play()</code> or <code>record()</code>).
 * The SignalDetector does not properly 
 * "arm" an <code>rtcc_Pattern[<b>i</b>]</code> unless the
 * <code>p_Pattern[<b>i</b>]</code> definition is already set.
 *
 * <h4>Signal buffer</h4>
 * When individual signals are detected 
 * (and the value of <code>p_Buffering</code> is <code>true</code>),
 * the signals are stored in the signal buffer. 
 * Signals are kept in the signal buffer until they are removed by
 * by one of <ul>
 * <li><code>retrieveSignals()</code>,</li>
 * <li><code>flushBuffer()</code>,</li>
 * <li>pattern filtering, or</li>
 * <li>signal buffer overflow.</li>
 * </ul>
 * <p>
 * <b>retrieveSignals:</b>
 * {@link SignalDetector#retrieveSignals retrieveSignals(numSignals, ...)}
 * waits for any of the various terminating conditions 
 * and then removes and returns up to <code>numSignals</code> 
 * signals from the signal buffer.
 * <p>
 * <b>flushBuffer:</b>
 * {@link SignalDetector#flushBuffer flushBuffer()} succeeds immediately 
 * and removes all signals from the buffer. 
 * <p>
 * Retrieving or flushing signals means those signals
 * are no longer used for matching patterns.
 * All pattern matchers are reset.
 * <p>
 * <b>Filtering:</b>
 * If a pattern is matched and that pattern Symbol appears in the value
 * of <code>p_Filter</code>, then the signals that matched the pattern are
 * removed from the signal buffer. 
 * <a href="#Pattern_Matching">Pattern matching</a> is explained below.
 * <p>
 * <b>Overflow:</b>
 * The length of the signal buffer is available as the value of
 * the read-only parameter <code>p_BufferSize</code>, 
 * and is at least 35 (long enough to hold any international telephone number).
 * Applications can request support for a longer buffer
 * using the attribute <code>a_BufferSize</code>.
 * <p>
 * If <code>p_BufferSize</code> signals are in the signal buffer
 * then the next signal causes overflow.
 * <a href="#Overflow_processing">Overflow processing</a> is explained below.
 * <p>
 * <b>Note:</b>
 * When a signal is removed from the signal buffer,
 * it is delivered to the application in the associated
 * event. A signal appears in at most one such event.
 * In addition to being in a buffer removal event,
 * a signal may appear in at most one pattern matched event.
 *
 * <a name="Operation"></a>
 * <h3>Operation</h3>
 * When the signal detector detects a signal,
 * the processing is a follows: 
 * <ol>
 * <li> <code>rtcc_SignalDetected</code> triggers any applicable RTCs.</li>
 * <li> if <code>p_EnabledEvents</code> contains <code>ev_SignalDetected</code>,
 * then <tt>SignalDetectorListener.onSignalDetected(event)</tt> is invoked</li>
 * <li> if <code>p_Buffering</code> is true,
 * then the signal is added to the signal buffer.</li>
 * <li> if the signal buffer overflows 
 * then <a href="#Overflow_processing">overflow processing</a> is done.</li>
 * <li> if pattern matchers are activated, 
 * then <a href="#Pattern_matching">pattern matching</a> is done:
 * the newly detected signal is passed to each activated pattern matcher.</li>
 * <li> <code>retrieveSignals</code> may be terminated by a pattern match.</li>
 * <li> the <code>NumSignals</code> signal counter is incremented
 * and <code>retrieveSignals</code> may be terminated
 * with <code>q_NumSignals</code>. 
 * </ol>
 * <p>
 * <a name="Overflow_processing"></a>
 * <h4>Overflow processing</h4>
 * If the signal buffer contains <code>p_BufferSize</code> signals, 
 * then the next signal causes overflow.
 * <p>
 * When overflow occurs, some signal must be discarded. 
 * The Boolean value of parameter <code>p_DiscardOldest</code> 
 * indicates whether the oldest signal should be removed from
 * the signal buffer and discarded, or whether the current, new
 * signal should be discarded.
 * Signals that are discarded are included 
 * in the <code>ev_Overflow</code> event, 
 * but are not included in the signal buffer
 * returned by <code>retrieveSignals()</code>.
 * <p>
 * When the signal buffer overflows, the processing is as follows:
 * <ol>
 * <li> if the value of <code>p_DiscardOldest</code> is true,
 * then the oldest signal is discarded.</li>
 * <li> if the value of <code>p_DiscardOldest</code> is false,
 * then the current signal is discarded.</li>
 * <li> if the value of <code>p_EnabledEvents</code> contains 
 * the symbol <code>ev_Overflow</code>,
 * then {@link SignalDetectorListener#onOverflow onOverflow(event)} 
 * is invoked and the event contains the discarded signal. </li>
 * </ol>
 * <p>
 * <b>Note:</b> even if the current signal is discarded from the 
 * signal buffer, it is still used for pattern matching. 
 * However, if pattern matching is activated <i>after</i> a signal has
 * been discarded, that discarded signal is not used to
 * initialize the pattern matchers.
 * <p>
 *
 * <a name="Pattern_matching"></a>
 * <h4>Pattern matching</h4>
 * A pattern matcher is activated when its pattern Symbol 
 * is used in any of these ways: 
 * <ul>
 * <li> as an RTC trigger in an active resource method, </li>
 * <li> as one of the elements in the value of <tt>p_EnabledEvents</tt>, </li>
 * <li> as one of the elements in the value of <tt>p_Filter</tt>, </li>
 * <li> as a terminating condition in <code>retrieveSignals()</code>
 * <br> (that is, it appears in the <tt>patterns</tt> argument).</li>
 * </ul>
 * <p>
 * When a pattern matcher is activated <!-- (or re-activated)--> by one of the
 * contitions above, it is initialized by <i>scanning</i> the current 
 * signal buffer contents.  This may generate one or more pattern
 * matching events.
 * <ul> 
 * <li> Each signal in the signal buffer is passed in turn to the
 * [newly activated<!-- or re-activated -->] pattern matcher(s).
 * </li></ul>
 * <p>
 * <b>Note:</b>
 * This procedure is called <i>scanning</i> the buffer.
 * Then intent of scanning is that the new pattern matchers match patterns
 * as if the matcher had been activated before the signals entered the buffer.
 * This step is to handle any typeahead that may have come before
 * before the pattern(s) were activated.
 * A pattern matcher <i>scans</i> the buffer <b>only</b> when it is
 * newly activated<!-- or re-activated -->.
 *
 * The application should use <tt>p_Filter</tt> and <tt>flushBuffer</tt>
 * carefully to get the desired effects.
 * 
 * <p>
 * When a signal is passed to a pattern matcher, this procedure is followed:
 * <ol>
 * <li> If the signal contributes to a match for this pattern matcher,
 * then the patterm matching state machine is advanced to record that.</li>
 * <li> If the signal does not contribute to a match for this pattern matcher,
 * then the pattern matching state machine resets to record that.</li>
 * <li> If the signal brings the pattern matcher for
 * pattern[<b>i</b>] to its matched state, then then do 4-8:</li>
 * <li> If this pattern is activated as an RTC trigger, then
 *      RTC trigger <code>rtcc_pattern[<b>i</b>]</code> is generated</li>
 * <li> If the value of <code>p_EnabledEvents</code> contains
 * <code>ev_Pattern[<b>i</b>]</code> then <code>onPatternMatched(event)</code>
 * is invoked on all SignalDetectorListeners, and the <code>event</code>
 * contains the signals that matched the pattern.</li>
 * <li> If the value of <code>p_Filter</code> contains 
 * <code>p_Pattern[<b>i</b>]</code>
 * then the matching signals are removed from the signal buffer <i>and</i>
 * all pattern matchers are reset to their initial (no input) state
 * and this signal is not considered by subsequent pattern matchers.</li>
 * <li> If the value of the <code>patterns</code> argument to 
 * <code>retrieveSignals</code> contains <code>p_Pattern[<b>i</b>]</code> 
 * then <code>retrieveSignals</code> terminates with qualifier
 * <code>q_Pattern[<b>i</b>]</code> and returns the current contents 
 * of the signal buffer.</li>
 * </ol>
 * <p>
 * <b>Note:</b>
 * The intent of step 6 is to ensure that a sequence of signals
 * that is filtered does not contribute to a match in subsequent Patterns.
 * "<i>subsequent</i>" means patterns with index greater than [<b>i</b>].
 * <p>
 *
 * <a name="pattern_Strings"><!-- --></a>
 * <h3>Format of Strings that define a Pattern</h3>
 *
 * Pattern definitions are Strings of the form:
 * <code><br>
 * { [<b>{</b>count<b>}</b>] signal | 
 *   [<b>{</b>count<b>}</b>] <b>[</b>signals<b>]</b> }* 
 * </code>
 * <p>
 * <br><code><b>{ count }</b></code> specifies an optional repeat count. 
 * If not specifed <code>count</code> is 1, 
 * otherwise it is some decimal number.
 * <br><code><b>signal</b></code> names a (DTMF) signal to be detected, 
 * one of:
 * <code>0,1,2,3,4,5,6,7,8,9,*,#,A,B,C,D,CNG,CED,?.</code>
 * <br><code><b>[ signals ]</b></code> is a space separated 
 * list of signal names,
 * that specifies a set of equivalent or alternative signals to be detected. 
 * For example: <code><b>[0 1 2 3]</b></code> 
 * matches any one of the given four signals.
 * <br><code>signal == `<b>?</b>`</code> matches any single signal.
 * <p>
 * The pattern matches a sequence of <code><b>count</b></code> occurences 
 * of a specified <code><b>signal</b></code>,
 * or <code><b>count</b></code> occurences of any of the 
 * named <code><b>signals</b></code>.
 * <p>
 * The Pattern parameters can be set with <code>setParameters</code>
 * or in the <code>optargs</code> argument of retrieveSignals.
 * Each pattern String is set as the value of one of the pattern
 * parameter symbols (<code>p_Pattern[<b>i</b>]</code>). 
 * <p>
 * <b>Example:</b> define some patterns
 * <pre>
 * dict.put(p_Pattern[1], "[1 2 3 4]");  // valid menu choices
 * dict.put(p_Pattern[2], "[5 6 8 9]");  // invalid menu choices
 * dict.put(p_Pattern[3], "0");          // route to live operator
 * dict.put(p_Pattern[4], "{2} #");      // ## hangup
 * dict.put(p_Pattern[5], "{12} [0 1 2 3 4 5 6 7 8 9]"); // Accnt number 
 * setParameters(dict);</pre>
 *
 * <p>
 * <b>Example:</b> enable patterns for event callbacks:
 * <pre>
 * Symbol[] patternSetOne = {p_Pattern[1], p_Pattern[2], p_Pattern[3]};
 * dict.put(SignalDetector.p_EnabledEvents, patternSetOne);
 * setParameters(dict);</pre>
 * <p>
 * <b>Example:</b> enable patterns and wait for a match:
 * <pre>
 * Symbol[] patternSetTwo = {p_Pattern[3], p_Pattern[4]};
 * sigDetEvent = retrieveSignals(-1, patternSetTwo, null, null);</pre>
 * <p>
 * For an extensive example, 
 * see <a href="doc-files/AnswerPhone.java">AnswerPhone.java</a>
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface SignalDetector extends Resource, SignalDetectorConstants {
    /**
     * Retrieve some signals from the signal buffer.
     * This method returns when <code>numSignals</code> signals are 
     * in the signal buffer (and removes them). May return with fewer than
     * <code>numSignals</code> when some other terminating condition 
     * occurs (<code>rtca_Stop</code>, a Pattern is detected, or timeout).
     * In that case, the method returns and removes all the signals 
     * from the signal buffer.  If <code>(numSignals == -1)</code>, then
     * this method returns <i>only</i> when some other termination 
     * condition occurs.
     * <p>
     * The terminating conditions are controlled by the arguments
     * <code>patterns</code> and <code>rtc</code>, and various
     * timeout parameters.
     * <p>
     * <code>patterns</code> is an array of Symbols, with an entry for 
     * each Pattern to enable.
     * If the <code>patterns</code> array contains a pattern Symbol
     * then recognition of that pattern is treated as
     * a terminating condition for <code>retrieveSignals()</code>.
     * If <code>patterns</code> is null, then pattern recognition is not
     * considered as a terminating condition.
     * <p>
     * <code>retrieveSignals</code> also terminates if any of the
     * {@link SignalDetectorConstants#p_InitialTimeout p_InitialTimeout},
     * {@link SignalDetectorConstants#p_InterSigTimeout p_InterSigTimeout},
     * {@link SignalDetectorConstants#p_Duration p_Duration}
     * timeouts are exceeded. These times are relative to the start
     * of the <code>retrieveSignals</code> operation.
     * These parameters can be set with <code>setParameters</code>
     * or in the <code>optargs</code> argument.
     * <p>
     * To get the retrieved signals, invoke <code>getSignalBuffer()</code>
     * on the SignalDetectorEvent returned from <code>retrieveSignals</code>.
     * If <code>retrieveSignals</code> can not return a signal buffer,
     * it throws an Exception, 
     * so <code>retrieveSignals(...).getSignalBuffer()</code> is safe.
     * The Signals are accessed as elements of the <tt>Symbol[]</tt> returned by
     * {@link SignalDetectorEvent#getSignalBuffer getSignalBuffer}
     * or char elements of the String returned by 
     * {@link SignalDetectorEvent#getSignalString getSignalString}.
     * <p>
     * <b>Example:</b>
     * <br><code>
     * 	String nextDTMF = retrieveSignals(1,null,null,null).getSignalBuffer();
     * </code>
     * <p>
     * <b>Note:</b>
     * Due to the effects of overflow and typeahead synchronization,
     * the more reliable way to get the signals that matched a pattern
     * is from the <code>ev_PatternMatched</code> event in the
     * {@link SignalDetectorListener#onPatternMatched onPatternMatched}
     * method.
     * 
     * @param numSignals the int number of signals to be retrieved.
     * @param patterns indicates which Patterns should terminate retrieval.
     * @param rtc an Array of RTC (Run Time Control) objects.
     * The RTC actions may be <code>rtca_Stop</code> 
     * or <code>rtca_FlushBuffer</code>.
     * @param optargs a Dictionary of optional arguments.
     * <code>p_Pattern[<b>i</b>]</code> may be set as transient parameters.
     *
     * @return a SignalDetectorEvent, suitable for for getSignalBuffer().
     *
     * @throws MediaResourceException if requested operation fails.
     */
     
    SignalDetectorEvent retrieveSignals(int numSignals, 
					Symbol[] patterns, 
					RTC[] rtc, 
					Dictionary optargs) 
	throws MediaResourceException;

    /**
     * Remove all signals from the signal buffer.
     * <p>
     * This prevents buffer overflow, and synchronizes the pattern matchers.
     * <p>
     * The signals flushed may be accessed 
     * using Event.getSignalBuffer() or Event.getSignalString().
     * If the flushed signals are not available, 
     * then these methods return <code>null</code>.
     * <p>
     * This is conceptually equivalent to:
     * <code>retrieveSignals((p_BufferSize), null, null, {p_Duration=0});</code>
     * <br> but flushBuffer() also resets all the pattern matchers.
     * <p>
     * <b>Note:</b> 
     * flushBuffer is also available using <code>rtca_FlushBuffer</code>.
     *
     * @return a SignalDetectorEvent containing any signals flushed.
     * @throws MediaResourceException if requested operation fails.
     */
    SignalDetectorEvent flushBuffer() throws MediaResourceException;
}
