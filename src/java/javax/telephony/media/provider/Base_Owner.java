/*
 * Base_Owner.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import java.lang.ref.WeakReference;
import java.util.EventListener;
import javax.telephony.media.*;

/**
 * A class that owns an EventQueue, and is an MPI.Owner to DispatchableEvents.
 * Implements the basic structures to manage the EventQueue and dispatch events.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public class Base_Owner implements MPI.Owner {

    /** an EventQueue object for events from this Owner/Source. */
    protected EventQueue eventQueue = null;

    /** Set a Dispatcher.ExceptionHandler for this Owner/Source. */
    public void setExceptionHandler(Dispatcher.ExceptionHandler handler) {
	this.handler = handler;
    }
    /** ExceptionHandler if set by user.
     * If not set, use Dispatcher.printDispatcherException.
     */
    Dispatcher.ExceptionHandler handler = null;    

    /** 
     * Action may be modified by using setExceptionHandler().
     * Or subclass can override initEventQueue() and replace
     * the _dispatcherExceptionHandler.
     * <p>
     * <b>Note:</b> since an instance of this is put into each
     * EventQueue/Dispatcher, and the EventQueue has an active Thread
     * we must use only a WeakReference to the Base_Owner,
     * else the Base_Owner will never be GC'd (unless the Thread dies).
     * <p>
     * This handler does WeakReference, so users of setExceptionHandler
     * do not need to worry about a reference from the EventQueue Thread.
     */
    protected static
    class DispatcherExceptionHandler implements Dispatcher.ExceptionHandler {
	WeakReference wrown = null;
	DispatcherExceptionHandler(Base_Owner owner) {
	    wrown = new WeakReference(owner);
	}
	public void onDispatcherException(Dispatcher disp,
					  Exception ex, 
					  EventListener listener, 
					  Dispatcher.DispatchableEvent event) {
	    Base_Owner owner = (Base_Owner)wrown.get();
	    // should not be possible for owner to GC while someone holds an event...
	    // but just in case:
	    Dispatcher.ExceptionHandler handler = (owner != null) ? owner.handler : null;
	    if (handler != null) try {
		handler.onDispatcherException(disp, ex, listener, event);
	    } catch (Exception ex2) {
		System.err.println ("!Exception in Handler: "+handler+": "+ex2);
		Dispatcher.printDispatcherException("Owner: ", disp, ex, listener, event);
	    } else {
		Dispatcher.printDispatcherException("Owner: ", disp, ex, listener, event);
	    }
	}
    }
    /** Holds an instance of the DispatcherExceptionHandler */
    private DispatcherExceptionHandler _dispatcherExceptionHandler = 
	new DispatcherExceptionHandler(this);

    /** Invoked by the no-arg constructor.
     * <br>if(eventQueue == null) eventQueue = new EventQueue();
     * <br>eventQueue.setExceptionHandler(_dispatcherExceptionHandler)
     * <br>eventQueue.start(obj);
     * @param an object to qualify the eventQueue.Thread name
     * (typically: this.Base_Owner).
     */
    protected void initEventQueue(Object nameObj) {
	// subclass can supply their own specialized EventQueue:
	if (eventQueue == null) eventQueue = new EventQueue();
	eventQueue.setExceptionHandler(_dispatcherExceptionHandler);
	eventQueue.start(nameObj);
    }

    /** invokes initEventQueue() */
    protected Base_Owner() { initEventQueue(this); }

    /** Shut down the EventQueue.
     * close/stop the Dispatcher Thread.
     */
    protected void finalize() {
	eventQueue.close();
	_dispatcherExceptionHandler = null;
	// super.finalize();		// Object.finalize() does nothing
    }

    /**
     * Add EventListener to this object's EventQueue.
     * @param listener the EventListener to be added
     */ 
    public void addListener( EventListener listener ) {
	eventQueue.addListener(listener);
    }

    /**
     * Remove EventListener from this object's EventQueue.
     * @param listener the EventListener to be removed
     */ 
    public void removeListener( EventListener listener ) {
	eventQueue.removeListener(listener);
    }

    /** 
     * To be a Base.Event "source" you must be an Owner.
     * <p>
     * Implements MPI.Owner
     * <p>
     * The vendor's implementation notifies this MPI.Owner that some
     * Async.Event has completed.  
     * This Owner must deliver the event to the application, 
     * invoking the various Listeners.
     * <p>
     * This <tt>MPI.Owner</tt> implementation uses the
     * the EventQueue and Dispatcher in this package;
     * which assume that the <tt>Async.Event</tt> implements
     * <tt>Dispatcher.DispatchableEvent;</tt> so the event
     * knows how to recognize and invoke its own 
     * Listener and Callback method.
     * <p>
     * <b>Note:</b>We would rather this was <i>not</i> a public method,
     * but the Java <tt>interface</tt> allows only public methods.
     * If an application Listener were to invoke this, it would
     * make a nice infinite loop...
     * @param event a done Event that will be queued for dispatch
     * to application Listeners.
     */
    /*
     * @see EventQueue
     * @see Dispatcher
     */
    public void onEventDone(Base.Event event) {
	if (eventQueue != null) {
	    eventQueue.enqueueDoneEvent(event);
	}
    }
}
