/*
 * Base_ASREvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.provider;

import javax.telephony.media.async.*;
import javax.telephony.media.*;
import java.util.Dictionary;
import java.util.Hashtable;
import java.util.EventListener;

/**
 * Defines the dispatch and accessor methods for ASR Events.
 * <p>
 * See S200_ASREvent for a concrete instantiation.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
abstract
public class Base_ASREvent extends Base.ResourceEvent 
    implements Async_ASREvent {
    /**
     * Create specific completion or non-transaction event from ASR.
     *
     * @param source the Object that created this EventObject.
     * @param eventID a Symbol that identifies this event: 
     * ASR.ev_Recognize
     */
    public Base_ASREvent(Object source, Symbol eventID) {
	super(source, eventID);
    }

    abstract public Symbol getResultPresentation();
    abstract public Dictionary getContextDictionary();
    abstract public String getRuleExpansion();
    abstract public String getWordName();
    abstract public String getContextName();
    abstract public Symbol getReadiness();
    abstract public Symbol getTrainingResult();
    abstract public int getNumberOfSequences();
    abstract public ASREvent.TokenSequence getTokenSequence(int seqNum);

    /** Send ASREvent to proper Listener.Callback method.
     * based on EventID.
     * <p>
     * <b>Note:</b> Although the S.200 packet may have
     * <tt>eventID=ev_RetrieveRecognition</tt>,
     * the S.410 event created in the MPI methods
     * will have <tt>eventID=evGet[Final]Results</tt>.
     */
    public void dispatch(EventListener listener) {
	if (isNonTrans) {
	    if (!(listener instanceof ASRListener)) return;
	    // Non-trans events from ASRListener
	    if (eventID.equals(ev_RecognitionStarted)) {
		((ASRListener)listener).onRecognitionStarted(this);
	    } else if (eventID.equals(ev_ValidUtterance)) {
		((ASRListener)listener).onValidUtterance(this);
	    } else if (eventID.equals(ev_InvalidUtterance)) {
		((ASRListener)listener).onInvalidUtterance(this);
	    } else if (eventID.equals(ev_IntermediateResultsReady)) {
		((ASRListener)listener).onIntermediateResultsReady(this);
	    }
	} else {
	    // check for completion events:
	    if (!(listener instanceof Async_ASRListener)) return;
	    if (eventID.equals(ev_Recognize)) {
		((Async_ASRListener)listener).onRecognizeDone(this);
	    } else if (eventID.equals(ev_GetResults)) {
		((Async_ASRListener)listener).onGetResultsDone(this);
	    } else if (eventID.equals(ev_GetFinalResults)) {
		((Async_ASRListener)listener).onGetFinalResultsDone(this);

		/* Transactional methods for RTC actions */
	    } else if (eventID.equals(ev_Start)) {
		((Async_ASRListener)listener).onStartDone(this);
	    } else if (eventID.equals(ev_Stop)) {
		((Async_ASRListener)listener).onStopDone(this);
	    } else if (eventID.equals(ev_Idle)) {
		((Async_ASRListener)listener).onIdleDone(this);
	    } else if (eventID.equals(ev_UpdateParameters)) {
		((Async_ASRListener)listener).onUpdateParametersDone(this);

		
		/* Rule Expansion Mehtods */
	    } else if (eventID.equals(ev_GetRuleExpansion)) {
		((Async_ASRListener)listener).onGetRuleExpansionDone(this);
	    } else if (eventID.equals(ev_SetRuleExpansion)) {
		((Async_ASRListener)listener).onSetRuleExpansionDone(this);

		/* Context Methods */
	    } else if (eventID.equals(ev_ContextCopy)) {
		((Async_ASRListener)listener).onContextCopyDone(this);
	    } else if (eventID.equals(ev_ContextCreate)) {
		((Async_ASRListener)listener).onContextCreateDone(this);
	    } else if (eventID.equals(ev_ContextGetParameters)) {
		((Async_ASRListener)listener).onContextGetParametersDone(this);
	    } else if (eventID.equals(ev_ContextRemove)) {
		((Async_ASRListener)listener).onContextRemoveDone(this);
	    } else if (eventID.equals(ev_ContextSetParameters)) {
		((Async_ASRListener)listener).onContextSetParametersDone(this);

		/* Training Methods */
	    } else if (eventID.equals(ev_WordCommit)) {
		((Async_ASRListener)listener).onWordCommitDone(this);
	    } else if (eventID.equals(ev_WordCreate)) {
		((Async_ASRListener)listener).onWordCreateDone(this);
	    } else if (eventID.equals(ev_WordDeleteLastUtterance)) {
		((Async_ASRListener)listener).onWordDeleteLastUtteranceDone(this);
	    } else if (eventID.equals(ev_WordDeleteTraining)) {
		((Async_ASRListener)listener).onWordDeleteTrainingDone(this);
	    } else if (eventID.equals(ev_WordDestroy)) {
		((Async_ASRListener)listener).onWordDestroyDone(this);
	    } else if (eventID.equals(ev_WordTrain)) {
		((Async_ASRListener)listener).onWordTrainDone(this);
	    }
	}
    }
}
