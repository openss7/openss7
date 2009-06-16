/*
 * Async_SignalGenerator.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Non-blocking methods for SignalGenerator.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface Async_SignalGenerator extends SignalGeneratorConstants {
    /**
     * Non-blocking version of sendSignals();
     * Transmit a series of signals. 
     * <p>
     * Each signal is defined by a Symbol in the Symbol[].
     *
     * @param signals an array of Symbols that defines a signal.
     * @param rtc an array of RTC objects that controls this transactions.
     * @param optargs a Dictionary of optional arguments 
     * @return an Async_SignalGeneratorEvent
     */
    public Async_SignalGeneratorEvent async_sendSignals(Symbol[] signals, 
							RTC[] rtc, 
							Dictionary optargs);
    
    /**
     * Non-blocking version of sendSignals;
     * Transmit a series of signals. 
     * <p>
     * Each signal is defined by a char in the String.
     *
     * @param signals an array of Symbols that defines a signal.
     * @param rtc an array of RTC objects that controls this transactions.
     * @param optargs a Dictionary of optional arguments 
     * @return an Async_SignalGeneratorEvent
     */
    public Async_SignalGeneratorEvent async_sendSignals(String signals, 
							RTC[] rtc,
							Dictionary optargs);
}
