/*
 * SignalGenerator.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines a method for sending signals out to the telephony network.
 * These methods generate the familiar DTMF tones.
 * <p>
 * Vendor-specific extensions may be used to generate other tones.
 * 
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface SignalGenerator extends Resource, SignalGeneratorConstants {
	
    /**
     * Transmit a series of signals out the Terminal to the network. 
     * <p>
     * Each signal is defined by a single char in the String.
     * <p>
     * <b>Note:</b> The AIA inserts spaces for presentation at the 
     * S.200/S.300 layers
     *
     * @param signals an String containing chars representing each signal.
     * @param rtc an array of RTC objects that controls this transactions.
     * @param optargs a Dictionary of optional arguments 
     * @return a SignalGeneratorEvent.
     * @exception MediaResourceException if this request fails. 
     * @throws DisconnectedException if Terminal is disconnected.
     */
    SignalGeneratorEvent sendSignals(String signals, RTC[] rtc,
				     Dictionary optargs)
	throws MediaResourceException;

    /**
     * Transmit a series of signals out the Terminal to the network. 
     * <p>
     * Each signal is defined by a Symbol in the Symbol[].
     * <p>
     * <b>Note:</b> This method may be less efficient than using 
     * the String form of sendSignals.
     *
     * @param signals an array of Symbols that defines a signal.
     * @param rtc an array of RTC objects that controls this transactions.
     * @param optargs a Dictionary of optional arguments 
     * @return a SignalGeneratorEvent
     * @exception MediaResourceException if this request fails. 
     * @throws DisconnectedException if Terminal is disconnected.
     */
    SignalGeneratorEvent sendSignals(Symbol[] signals, RTC[] rtc, 
				     Dictionary optargs)
	throws MediaResourceException;
}
