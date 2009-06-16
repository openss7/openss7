/*
 * Async_SignalDetectorListener.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/**
 * Defines the callback methods for Async_SignalDetector completion Events. 
 * <p>
 * For these methods, the eventID will correspond to the method name.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
interface Async_SignalDetectorListener extends MediaListener {
    /** Invoked when async_retrieveSignals completes. 
     * @param event a SignalDetectorEvent that isDone().
     */
    void onRetrieveSignalsDone(SignalDetectorEvent event);
    
    /** Invoked when async_flushBuffer completes. 
     * @param event a SignalDetectorEvent that isDone().
     */
    void onFlushBufferDone(SignalDetectorEvent event);
}
