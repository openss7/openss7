/*
 * Async_ASRListener.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines the callback methods for Async_ASR transactional Events. 
 * <p>
 * For these methods, the eventID will correspond to the method name.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
interface Async_ASRListener extends MediaListener {
    /* Basic Operations */
    /** Invoked when async_recognize completes. 
     * @param event an ASREvent that isDone().
     */
    void onRecognizeDone(ASREvent event);

    /** Invoked when async_getResults completes. 
     * @param event an ASREvent that isDone().
     */
    void onGetResultsDone(ASREvent event);

    /** Invoked when async_getFinalResults completes. 
     * @param event an ASREvent that isDone().
     */
    void onGetFinalResultsDone(ASREvent event);

    /* Transaction methods for RTC */
    /** Invoked when async_startASR completes. 
     * @param event an ASREvent that isDone().
     */
    void onStartDone(ASREvent event);

    /** Invoked when async_stopASR completes. 
     * @param event an ASREvent that isDone().
     */
    void onStopDone(ASREvent event);

    /** Invoked when async_idleASR completes. 
     * @param event an ASREvent that isDone().
     */
    void onIdleDone(ASREvent event);

    /** Invoked when async_updateParameters completes. 
     * @param event an ASREvent that isDone().
     */
    void onUpdateParametersDone(ASREvent event);

    /* Rule Expansion Mehtods */
    /** Invoked when async_getRuleExpansion completes. 
     * @param event an ASREvent that isDone().
     */
    void onGetRuleExpansionDone(ASREvent event);

    /** Invoked when async_setRuleExpansion completes. 
     * @param event an ASREvent that isDone().
     */
    void onSetRuleExpansionDone(ASREvent event);

    /* Context Methods */

    /** Invoked when async_contextCopy completes. 
     * @param event an ASREvent that isDone().
     */
    void onContextCopyDone(ASREvent event);

    /** Invoked when async_contextCreate completes. 
     * @param event an ASREvent that isDone().
     */
    void onContextCreateDone(ASREvent event);

    /** Invoked when async_contextGetParameters completes. 
     * @param event an ASREvent that isDone().
     */
    void onContextGetParametersDone(ASREvent event);

    /** Invoked when async_contextRemove completes. 
     * @param event an ASREvent that isDone().
     */
    void onContextRemoveDone(ASREvent event);

    /** Invoked when async_contextSetParamters completes. 
     * @param event an ASREvent that isDone().
     */
    void onContextSetParametersDone(ASREvent event);

    /* Training Methods */

    /** Invoked when async_wordCommit completes. 
     * @param event an ASREvent that isDone().
     */
    void onWordCommitDone(ASREvent event);

    /** Invoked when async_wordCreate completes. 
     * @param event an ASREvent that isDone().
     */
    void onWordCreateDone(ASREvent event);

    /** Invoked when async_wordDeleteLastUtterance completes. 
     * @param event an ASREvent that isDone().
     */
    void onWordDeleteLastUtteranceDone(ASREvent event);

    /** Invoked when async_wordDeleteTraining completes. 
     * @param event an ASREvent that isDone().
     */
    void onWordDeleteTrainingDone(ASREvent event);

    /** Invoked when async_wordDestroy completes. 
     * @param event an ASREvent that isDone().
     */
    void onWordDestroyDone(ASREvent event);

    /** Invoked when async_wordTrain completes. 
     * @param event an ASREvent that isDone().
     */
    void onWordTrainDone(ASREvent event);
}
