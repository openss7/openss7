/*
 * SignalDetectorEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/**
 * Defines the methods for SignalDetector Events.
 * <p>
 * <b>Note:</b>
 * For JTAPI 1.3, we extend SignalDetectorEvent to work
 * more easily with signals with multi-character names.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public
interface SignalDetectorEvent extends ResourceEvent, SignalDetectorConstants {
    /**
     * Get signals as a single String.
     * The return value is constructed by concatenating 
     * the single-character String equivalents for each signal Symbol.
     * <p>
     * Each DTMF signal has a single character equivalent, 
     * one of: <code> 0,1,2,3,4,5,6,7,8,9,*,#,A,B,C,D</code>.
     * The fax tones <tt>"CNG"</tt> and <tt>"CED"</tt> are represtented
     * by the characters <tt>'&gt;'</tt> and <tt>'&lt;'</tt> respectively.
     * <p>
     * If the SignalBuffer contains non-DTMF signals
     * that do not have defined char equivalents, 
     * then a <tt>'?'</tt> is inserted in that place.
     * <p>
     * <b>Note:</b>
     * For all the standard (or unknown) signals and signal names,
     * <tt>getSignalString.charAt(n)</tt> corresponds
     * to the <tt>Nth</tt> signal retrieved.
     * However,
     * if a signal is retrieved that has been 
     * assigned a multi-character name (using <tt>p_SymbolChar</tt>)
     * then this correspondence does not hold.
     * For signals with multi-character names, consider using
     * <tt>getSpacedString()</tt> or <tt>getSignalBuffer()</tt>.
     * <p>
     * <b>Note:</b>
     * When getEventID() = <code>ev_flushBuffer</code>
     * and getQualifier() = <code>q_RTC</code>, 
     * some implementations may not return the flushed signals.
     * In that case, this method returns <code>null</code>.
     *
     * @return a String of DTMF signal characters.
     */
    String getSignalString();

    /**
     * Get signals as a single String with a space character
     * between each signal.
     * Signals with multi-character names appear with their full name.
     * <p>
     * For example, two Fax <i>CALLING</i> tones would appear as: "CNG CNG"
     * <p>
     * <b>Note:</b>
     * Returns <tt>null</tt> if there are no signals in the event.
     *
     * @return a String of DTMF Signal chars.
     */
    String getSpacedString();

    /**
     * Get signals as an array of Strings.
     * Each signal appears as its associated String name.
     * Signals with multi-character names appear with their full name.
     * <P>
     * This method is useful for examining Signals that are not
     * represented by single character names. In particular, 
     * extensions to the SignalDetector may use Strings
     * to define new signals.
     * <p>
     * <b>Note:</b>
     * This method is for compatibility with ECTF S.100 SignalDetectors
     * that may recognize signals for which there is no defined Symbol.
     * <P>
     * <b>Note:</b>
     * When <tt>getEventID() == ev_flushBuffer</tt>
     * and <tt>getQualifier() == q_RTC</tt>, 
     * some implementations may not return the flushed signals.
     * In that case, this method returns <code>null</code>.
     *
     * @return a String[], one String for each Signal in the event.
     * @see #getSpacedString
     */
    String[] getStringArray();

    /**
     * Get signals as an array of Symbols.
     * Each signal appears as its associated Symbol.
     * <P>
     * This method is useful for examining Signals that are not
     * represented by single character names. In particular, 
     * extensions to the SignalDetector may use the Symbol namespace
     * to define new signals.
     * <P>
     * <b>Note:</b>
     * When getEventID() = <code>ev_flushBuffer</code>
     * and getQualifier() = <code>q_RTC</code>, 
     * some implementations may not return the flushed signals.
     * In that case, this method returns <code>null</code>.
     *
     * @return a Symbol[], one Symbol for each Signal in the event.
     */
    Symbol[] getSignalBuffer();

    /**
     * When eventID is one of <code>ev_Pattern[<b>i</b>]</code>, 
     * return the int <code><b>i</b></code>.
     * If eventID is not <code>ev_Pattern[i]</code>, 
     * then return minus one (<code>-1</code>).
     * <p>
     * @return an int indicating which pattern was matched.
     */
    int getPatternIndex();

    /* <b>Implementation note:</b>
     * For i < 32 this could be a trivial subtraction operation,
     * because the first 32 ev_Pattern symbols are linear.
     * However, if the vendor has extended the number of
     * available patterns, this method must recognize that
     * and do the right thing.  This could be
     * implemented with a Hashtable, or a linear search,
     * or the vendor may use a piecewise linear approach.
     * <p>
     * Better yet, one could build a SignalDetectorEvent that includes
     * the index directly and this is just the accessor.
     */
}
