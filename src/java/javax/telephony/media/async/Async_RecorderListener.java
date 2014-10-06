/*
 * Async_RecorderListener.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines the callback methods for Async_Recorder transactional Events. 
 * <p>
 * For these methods, the eventID will correspond to the method name.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
interface Async_RecorderListener extends MediaListener {
    /** Invoked when async_record completes. 
     * @param event a RecorderEvent that isDone().
     */
    void onRecordDone(RecorderEvent event);

    /* RTC Commands */

    /** Invoked when async_stop completes. 
     * @param event a RecorderEvent that isDone().
     */
    void onStopDone(RecorderEvent event);

    /** Invoked when async_pause completes. 
     * @param event a RecorderEvent that isDone().
     */
    void onPauseDone(RecorderEvent event);

    /** Invoked when async_resume completes. 
     * @param event a RecorderEvent that isDone().
     */
    void onResumeDone(RecorderEvent event);

    // or:
    /**
     * Invoked when a transactional command completes.
     * <p> 
     * Provoked by any of: pauseRecorder, resumeRecorder, stopRecorder.
     *
     * @param event a RecorderEvent that isDone();
     */
    // void onCommandDone(RecorderEvent event);
}
