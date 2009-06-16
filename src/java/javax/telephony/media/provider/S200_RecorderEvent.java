/*
 * S200_RecorderEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import javax.telephony.media.*;
import javax.telephony.media.async.*;
import java.util.EventListener;

/**
 * S200_RecorderEvent extends Base_RecorderEvent to
 * extract info from an S.200 Payload.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
class S200_RecorderEvent extends Base_RecorderEvent {
    /**
     * Create specific completion or non-tranaction event from Recorder.
     *
     * @param source the Object that created this EventObject.
     * @param eventID a Symbol that identifies this event: 
     * ev_Record
     */
    public S200_RecorderEvent(Object source, Symbol eventID) {
	super(source, eventID);
	duration = -2;		// mark as unset
    }

    /** Constructor for non-trans vesion */
    public S200_RecorderEvent(Object source, Symbol eventID, boolean isNonTrans)
    {
	this(source, eventID);
	this.isNonTrans = isNonTrans;
    }

    public int getDuration() {
	waitForEventDone(); 
	if (duration == -2) {
	    Integer durationInt;
	    durationInt = (Integer)payload.get(ESymbol.Recorder_MaxDuration);
	    duration = (durationInt != null) ? durationInt.intValue() : -1;
	}
	return duration;
    }
}
