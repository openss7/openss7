/*
 * Base_SignalGeneratorEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Base_SignalGeneratorEvent defines the  methods used by SignalGeneratorEvent.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
class Base_SignalGeneratorEvent extends Base.ResourceEvent 
    implements Async_SignalGeneratorEvent {
    /**
     * Create specific completion or non-tranaction event from SignalGenerator.
     *
     * @param source the Object that created this EventObject.
     * @param eventID a Symbol that identifies this event: 
     * ev_SendSignals
     */
    public Base_SignalGeneratorEvent(Object source, Symbol eventID) 
    {super(source, eventID);}

    /** Dispatch this event to Async_SignalGeneratorListener */
    public void dispatch(EventListener listener) {
	if(!(listener instanceof Async_SignalGeneratorListener)) return;
	if(eventID.equals(ev_SendSignals)) {
	    ((Async_SignalGeneratorListener)listener).onSendSignalsDone(this);
	}
    }
}
