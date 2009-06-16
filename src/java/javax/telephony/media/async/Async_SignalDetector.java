/*
 * Async_SignalDetector.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.async;

import javax.telephony.media.*;
import java.util.Dictionary;

/**
 * Non-blocking methods for SignalDetector.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface Async_SignalDetector extends SignalDetectorConstants {
    /**
     * Non-blocking verions of retrieveSignals();
     * Retrieve some signals from the signal buffer.
     *
     * @param numSignals the int number of signals to be retrieved.
     * @param patterns indicates which Patterns should terminate retrieval.
     * @param rtc an Array of RTC (Run Time Control) objects.
     * The RTC actions may be rtca_Stop or rtca_FlushBuffer.
     * @param optargs a Dictionary of optional arguments.
     * <code>p_Pattern[<b>i</b>]</code> may be set here as temporary parameters.
     * @return an Async_SignalDetectorEvent
     */
	
    Async_SignalDetectorEvent async_retrieveSignals(int numSignals, 
						    Symbol[] patterns, 
						    RTC[] rtc, 
						    Dictionary optargs);
	
    /**
     * Non-blocking version of flushBuffer();
     * Discard any Signals currently in the SignalDetector's internal Buffer.
     * <p>
     * @return a Async_SignalDetectorEvent
     */
    Async_SignalDetectorEvent async_flushBuffer();
}
    
