/*
 * Async_RecorderListenerAdapter.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Implements all Async_RecorderListener and RecorderListener methods.
 * Each method does nothing.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public class Async_RecorderListenerAdapter 
    implements Async_RecorderListener, RecorderListener {
    /* Non-trans events from RecorderListener. */
    public void onPause(RecorderEvent event) {}
    public void onResume(RecorderEvent event) {}

    /* completion events */
    public void onRecordDone(RecorderEvent event) {}

    /* RTC methods */
    public void onStopDone(RecorderEvent event) {}
    public void onPauseDone(RecorderEvent event) {}
    public void onResumeDone(RecorderEvent event) {}
    // or:
    // public void onCommandDone(RecorderEvent event) {}
}

