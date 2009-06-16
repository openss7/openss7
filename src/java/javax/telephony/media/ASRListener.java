/*
 * ASRListener.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines the callback methods for non-transactional ASR Events. 
 * <p>
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public 
interface ASRListener extends ResourceListener {
    /* unsolicited events */
    /** paused recognition has started due to RTC.
     * @param ev an ASREvent object
     */
    public void onRecognitionStarted(ASREvent ev);

    /** Intermediate recognition results are ready. 
     * @param ev an ASREvent object
     */
    public void onIntermediateResultsReady(ASREvent ev);

    /** a valid utterance has been recognized. 
     * @param ev an ASREvent object
     */
    public void onValidUtterance(ASREvent ev);

    /** an invalid utterance has been recognized. 
     * @param ev an ASREvent object
     */
    public void onInvalidUtterance(ASREvent ev);

    /* RTC methods */
    // public void onStart(Async_ASREvent event); // -> onRecognitionStarted
    // public void onStop(Async_ASREvent event);  // -> onRecognizeDone
    // public void onIdle(Async_ASREvent event);  // -> ???
    // public void onUpdateParameters(Async_ASREvent event); // -> ???
}


