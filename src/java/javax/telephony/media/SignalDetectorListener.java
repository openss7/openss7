/*
 * SignalDetectorListener.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines the callback methods for non-transactional SignalDetector Events. 
 * <p>
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface SignalDetectorListener extends ResourceListener {
    /**
     * Invoked when the SignalDetector detects a signal.
     * <p>
     * <code>event.getEventID()</code> is <code>ev_SignalDetected</code>.
     * <p>
     * This method is invoked only if 
     * {@link SignalDetectorConstants#p_EnabledEvents p_EnabledEvents}
     * contains the Symbol <code>ev_SignalDetected</code>.
     *
     * @param event  a SignalDetectorEvent object
     * @see SignalDetectorConstants#p_EnabledEvents p_EnabledEvents
     */
    public void onSignalDetected(SignalDetectorEvent event);

    /**
     * Invoked when the SignalDetector matches a pattern.
     * <p>
     * <code>event.getEventID()</code> is <code>ev_Pattern[<b>i</b>]</code>.
     * <p>
     * This method is invoked only if 
     * {@link SignalDetectorConstants#p_EnabledEvents p_EnabledEvents}
     * contains one of the Symbols <code>ev_Pattern[<b>i</b>]</code>.
     *
     * @param event  a SignalDetectorEvent object
     * @see SignalDetectorConstants#p_EnabledEvents p_EnabledEvents
     */
    public void onPatternMatched(SignalDetectorEvent event);

    /**
     * Invoked when the SignalDetector signal buffer overflows.
     * <p>
     * <code>event.getEventID()</code> is <code>ev_Overflow</code>.
     * <p>
     * This method is invoked only if 
     * {@link SignalDetectorConstants#p_EnabledEvents p_EnabledEvents}
     * contains the Symbol <code>ev_Overflow</code>.
     *
     * @param event  a SignalDetectorEvent object
     */
    public void onOverflow(SignalDetectorEvent event);

}
