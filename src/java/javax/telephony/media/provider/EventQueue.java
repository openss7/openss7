/*
 * EventQueue.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
import java.util.Vector;

/**
 * Establish a rendevous for event delivery and dispatch. 
 * <p>
 * EventQueue extends Dispatcher, allowing one to post done
 * Events to a queue.
 * The Events will be drained from the Queue by the EventQueue 
 * thread which notifies the Listeners.
 * <p>
 * EventQueue extends Dispatcher to include <tt>enqueueDoneEvent()</tt>.
 * The EventQueue Dispatacher thread calls <tt>getNextEvent()</tt> 
 * which dequeues an Event and returns it to be dispatched by the usual
 * <tt>notifyListeners()</tt> method.
 * <p>
 * Base_MediaService queues Async.Events to this EventQueue from onEventDone().
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
class EventQueue extends Dispatcher {

    /** Events done for this MediaService */
    private Vector doneEvents = new Vector();	// Async.Event

    public String toString() {
	return super.toString() + " {" +
	    "\n\t"+"doneEvents.size() = " + doneEvents.size() +
	    "}";
    }

    /**
     * Queue the given Event to be dispatched by the EventQueue thread.
     */
    public void enqueueDoneEvent(Base.Event event) {
	if (doneEvents == null) return;
	synchronized (doneEvents) {
	    doneEvents.addElement(event);
	    doneEvents.notifyAll();
	}
	//Thread.yield();	// give queue a chance to run
    }
	
    /** Get oldest DoneEvent. */
    // Note: This method equivalent to calling getNextEvent()
    private Base.Event dequeueDoneEvent() {
	if (doneEvents == null) return null;
	synchronized (doneEvents) {
	    while ((doneEvents.size() <= 0)) {
		try { doneEvents.wait(); }
		catch (InterruptedException ex) { 
		    //System.out.println("EventQueue interrupted: "+this);
		}
		if (stopNow) return null; 
		// otherwise, keep looping
	    } 
	    Base.Event retval = (Base.Event)doneEvents.firstElement();
	    doneEvents.removeElementAt(0);
	    return retval;
	}
    }
    
    /** Dequeue and dispatch the oldest Base.Event,
     * which is a DispatchableEvent.
     */
    protected Dispatcher.DispatchableEvent getNextEvent() {
	return (Dispatcher.DispatchableEvent)dequeueDoneEvent();
    }

    /** notifyListeners and signal event.done(). */
    public void notifyListeners(DispatchableEvent event) {
	super.notifyListeners(event);
	if (event instanceof Base.Event)
	    ((Base.Event)event).done(); //  release the waitForListenersDone() method
    }

    /** restart EventQueue. */
    public void start(String name) {
	// re-open the EventQueue
	if (doneEvents == null) doneEvents = new Vector();
	super.start(name);	// rename and restart
    }

    /** Stop Thread, flush doneEvents from queue. */
    public void stop() {
	super.stop();
	if (doneEvents != null)
	    synchronized(doneEvents) { doneEvents = null; }
    }

    /** Flush all Events from this EventQueue. */
    public void flush() {
	if (doneEvents == null) return;
	synchronized(doneEvents) {
	    doneEvents = new Vector();
	}
    }
}
