/*
 * Base_PlayerEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Base_PlayerEvent defines the  methods used by Player Events.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
class Base_PlayerEvent extends Base.ResourceEvent
    implements Async_PlayerEvent {
    /**
     * Create specific completion or non-transaction event from Player.
     *
     * @param source the Object that created this EventObject.
     * @param eventID a Symbol that identifies this event: 
     * ev_Play
     */
    public Base_PlayerEvent(Object source, Symbol eventID) {
	super(source, eventID);
    }

    /** index into a TVMList, indicating the TVM of interest.
     * @serial 
     */
    protected int index = -1;

    /**
     * Return index (zero based) into TVMList of last TVM played.
     * @return the index of the TVM that stopped.
     */
    public int getIndex() {
	waitForEventDone(); 
	return index;
    }
    
    /** @serial */
    protected int offset = -1;
    /**
     * Return index into a TVM phrase where play stopped.
     * @return the index into a TVM phrase where play stopped, paused, etc.
     */
    public int getOffset() {
	waitForEventDone(); 
	return offset;
    }

    /** @serial */
    protected Symbol changeType = null;

    /**
     * Return the RTC action Symbol that caused this change.
     *
     * @return one of the rtca_ Symbols.
     */
    public Symbol getChangeType() {
	waitForEventDone(); 
	return changeType;
    }

    /** Dispatch this event according to the eventID. */
    public void dispatch(EventListener listener) {
	//System.out.println("dispatch: "+this + "\n\t"+ listener);
	if (isNonTrans) {
	    if(!(listener instanceof PlayerListener)) return;
	    if (eventID.equals(ev_Pause)) {
		((PlayerListener)listener).onPause(this);
	    } else if (eventID.equals(ev_Resume)) {
		((PlayerListener)listener).onResume(this);
	    } else if (eventID.equals(ev_Jump)) {
		((PlayerListener)listener).onJump(this);
	    } else if (eventID.equals(ev_Speed)) {
		((PlayerListener)listener).onSpeedChange(this);
	    } else if (eventID.equals(ev_Volume)) {
		((PlayerListener)listener).onVolumeChange(this);
	    } else if (eventID.equals(ev_Marker)) {
		((PlayerListener)listener).onMarker(this);
	    }
	} else {
	    // check for completion events
	    if (!(listener instanceof Async_PlayerListener)) return;
	    if (eventID.equals(ev_Play)) {
		((Async_PlayerListener)listener).onPlayDone(this);
	    } else if (eventID.equals(ev_Pause)) {
		((Async_PlayerListener)listener).onPauseDone(this);
	    } else if (eventID.equals(ev_Resume)) {
		((Async_PlayerListener)listener).onResumeDone(this);
	    } else if (eventID.equals(ev_Stop)) {
		((Async_PlayerListener)listener).onStopDone(this);
	    } else if (eventID.equals(ev_Jump)) {
		((Async_PlayerListener)listener).onJumpDone(this);
	    } else if (eventID.equals(ev_AdjustSpeed)) {
		((Async_PlayerListener)listener).onAdjustSpeedDone(this);
	    } else if (eventID.equals(ev_AdjustVolume)) {
		((Async_PlayerListener)listener).onAdjustVolumeDone(this);
	    }
	}
    }
}

