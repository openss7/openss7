/*
 * Async_Recorder.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Non-blocking methods for Recorder resource.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface Async_Recorder extends RecorderConstants {
    /** 
     * Non-blocking verions of record();
     * <p>
     * Record data from the Terminal into the named
     * Media Data Object (MDO).
     * Encode and store data from the telephony network.
     * <code>record()</code> encodes the media stream
     * from the bound Terminal, and stores the result in 
     * the Media Stream Container identified by 
     * the given <code>streamID</code>.
     * <p>
     * @param streamID the String name of a MDO where recording is to be stored
     * @param rtc an Array of RTC (Run-Time-Control) objects
     * @param optargs, a Dictionary of optional arguments 
     *
     * @return an Async_RecorderEvent with additional information.
     *
     * @see Recorder#record
     */
    Async_RecorderEvent async_record(String streamID, RTC[] rtc, Dictionary optargs); 
    
	
    /** Non-blocking version of Recorder.pauseRecorder.
     * @return an Async_RecorderEvent
     * @see Recorder#pauseRecorder
     */
    Async_RecorderEvent async_pauseRecorder();

    /** Non-blocking version of Recorder.resumeRecorder.
     * @return an Async_RecorderEvent
     * @see Recorder#resumeRecorder
     */
    Async_RecorderEvent async_resumeRecorder();
    /** Non-blocking version of Recorder.stopRecorder.
     * @return an Async_RecorderEvent
     * @see Recorder#stopRecorder
     */
    Async_RecorderEvent async_stopRecorder();
}
