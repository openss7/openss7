/*
 * Base_RecorderEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Base_RecorderEvent defines the  methods used by RecorderEvent.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
class Base_RecorderEvent extends Base.ResourceEvent 
    implements Async_RecorderEvent {
    /**
     * Create specific completion or non-tranaction event from Recorder.
     *
     * @param source the Object that created this EventObject.
     * @param eventID a Symbol that identifies this event: 
     * ev_Record
     */
    public Base_RecorderEvent(Object source, Symbol eventID) 
    {super(source, eventID);}

    /** how much was recorded.
     * @serial 
     */
    protected int duration = -1;
    
    public int getDuration() {
	waitForEventDone(); 
	return duration;
    }

    /** Dispatch this event according to the eventID.
     * but only if the given Listener is of the correct class.
     */
    public void dispatch(EventListener listener) {
	if (isNonTrans) {
	    if(!(listener instanceof RecorderListener)) return;
	    if (eventID.equals(ev_Pause)) {
		((RecorderListener)listener).onPause(this);
	    } else if (eventID.equals(ev_Resume)) {
		((RecorderListener)listener).onResume(this);
	    }
	} else {
	    if(!(listener instanceof Async_RecorderListener)) { 
		return;
	    } else if (eventID.equals(ev_Record)) {
		((Async_RecorderListener)listener).onRecordDone(this);
	    } else if (eventID.equals(ev_Pause)) {
		((Async_RecorderListener)listener).onPauseDone(this);
	    } else if (eventID.equals(ev_Resume)) {
		((Async_RecorderListener)listener).onResumeDone(this);
	    } else if (eventID.equals(ev_Stop)) {
		((Async_RecorderListener)listener).onStopDone(this);
	    }
	}
    }
}
