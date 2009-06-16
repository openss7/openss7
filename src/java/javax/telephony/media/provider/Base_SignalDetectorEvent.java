/*
 * Base_SignalDetectorEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
import java.util.Hashtable;
import java.util.EventListener;

/**
 * Base_SignalDetectorEvent defines the methods used by
 * SignalDetectorEvent/AsyncSignalDetectorEvent.
 * <p>
 * a bit hokey, since four event classes share the same class-type:
 * ev_RetrieveSignals, ev_FlushBuffer, ev_Pattern[i], ev_SignalDetected.
 * <p>
 * The Symbols are inherited from SignalDetectorConstants
 * via SignalDetectorEvent via Async_SignalDetectorEvent.
 * <p>
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public abstract
class Base_SignalDetectorEvent extends Base.ResourceEvent
    implements Async_SignalDetectorEvent {

    /**
     * Create specific completion or non-tranaction event from Recorder.
     *
     * @param source the Object that created this EventObject.
     * @param eventID a Symbol that identifies this event: 
     * ev_RetrieveSignals, ev_FlushBuffer, 
     * ev_Pattern[i], ev_SignalDetected.
     */
    public Base_SignalDetectorEvent(Object source, Symbol eventID) {
	super(source, eventID);
    }

    /** in case you have more patterns to choose from. */
    protected static int patternSize = 32;

    /** which I in pattern[I]. 
     * @serial 
     */
    int patternIndex = patternSize;	// size of p_Pattern[]

    /** for eventID = ev_Pattern[i], get index "i" */
    public int getPatternIndex() {
	if (patternIndex < patternSize ) return patternIndex;

	// if a pattern stopped the transaction:
	// find which symbol was used: compute index
	while ( --patternIndex >= 0 ) {
	    if(p_Pattern[patternIndex].equals(eventID)) { break; }
	}
	// fall through with patternIndex = -1;
	return patternIndex;
    }

    /**
     * Get array of signal names.
     * Used for ev_retrieveSignals and ev_Pattern[i].
     * Each DTMF signal has a Symbol "name".
     */
    abstract public Symbol[] getSignalBuffer(); 
    

    /** 
     * Return SignalBuffer as a String.
     * Non-standard signals that have not been defined 
     * by setSignalChar() appear in the String as '?'.
     * 
     * @return a String representing SignalBuffer.
     */
    abstract public String getSignalString();

    /** dispatch this event to Async_SignalDetectorListener */
    public void dispatch(EventListener listener) {
	if(!(listener instanceof SignalDetectorListener)) return;
	if (eventID.equals(ev_SignalDetected)) {
	    ((SignalDetectorListener)listener).onSignalDetected(this);
	} else if (eventID.equals(ev_Overflow)) {
	    ((SignalDetectorListener)listener).onOverflow(this);
	} else if (isPattern(eventID)) {
	    ((SignalDetectorListener)listener).onPatternMatched(this);
	}
	// check completion events
	if(!(listener instanceof Async_SignalDetectorListener)) return;
	if (eventID.equals(ev_FlushBuffer)) {
	    ((Async_SignalDetectorListener)listener).onFlushBufferDone(this);
	} else { // if (eventID.equals(ev_retrieveSignals)) 
	    ((Async_SignalDetectorListener)listener).onRetrieveSignalsDone(this);
	}
    }

    /** true iff this.eventID is one of ev_pattern[i]. */
    protected boolean isPattern(Symbol eventID) {
	for (int i = 0; i < ev_Pattern.length; i++) {
	    if (eventID.equals(ev_Pattern[i])) return true;
	}
	return false;
    }
}
