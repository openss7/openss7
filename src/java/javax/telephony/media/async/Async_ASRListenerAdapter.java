/*
 * Async_ASRListenerAdapter.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Implements all Async_ASRListener and ASRListener methods.
 * Each method does nothing.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public class Async_ASRListenerAdapter implements Async_ASRListener, ASRListener {
    /* unsolicited events: ASRListener */
    public void onRecognitionStarted(ASREvent event) {}
    public void onIntermediateResultsReady(ASREvent event) {}
    public void onValidUtterance(ASREvent event) {}
    public void onInvalidUtterance(ASREvent event) {}

    /* Basic Operations */
    public void onRecognizeDone(ASREvent event) {}
    public void onGetResultsDone(ASREvent event) {}
    public void onGetFinalResultsDone(ASREvent event) {}

    /* RTC methods */
    public void onStartDone(ASREvent event) {}
    public void onStopDone(ASREvent event) {}
    public void onIdleDone(ASREvent event) {}
    public void onUpdateParametersDone(ASREvent event) {}

    /* Rule Expansion Mehtods */
    public void onGetRuleExpansionDone(ASREvent event) {}
    public void onSetRuleExpansionDone(ASREvent event) {}

    /* Context Methods */
    public void onContextCopyDone(ASREvent event) {}
    public void onContextCreateDone(ASREvent event) {}
    public void onContextGetParametersDone(ASREvent event) {}
    public void onContextRemoveDone(ASREvent event) {}
    public void onContextSetParametersDone(ASREvent event) {}

    /* Training Methods */
    public void onWordCommitDone(ASREvent event) {}
    public void onWordCreateDone(ASREvent event) {}
    public void onWordDeleteLastUtteranceDone(ASREvent event) {}
    public void onWordDeleteTrainingDone(ASREvent event) {}
    public void onWordDestroyDone(ASREvent event) {}
    public void onWordTrainDone(ASREvent event) {}
}
