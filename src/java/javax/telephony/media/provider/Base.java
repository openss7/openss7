/*
 * Base.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import javax.telephony.*;		// get core Objects and Exceptions
import javax.telephony.media.*;
import javax.telephony.media.async.*;
import java.util.Dictionary;
import java.util.EventListener;

/**
 * Implements all flavors of Async Events.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public class Base
{
    // these should not be used, are specifically ambigous
    final static Symbol ev_GetDict           = ESymbol.Group_GetInfo;
    final static Symbol ev_Void              = ESymbol.Group_None;

    // using Group_None as a event ID 
    // when no particular ID is specified, required, obvious
    /**
     * Implement a base class for Async.Event and MediaEvent.
     * Includes in-package methods for provider use.
     * <p>
     * This class can be instantiated, but the resulting event
     * has a no-op dispatch method; which means it goes through
     * the EventQueue, but is not dispatched to any Listener.
     * For most uses, a specialized class with a dispatch to Listener
     * should be used.
     * <p>
     * This models the basic S.200 Event structure.
     * The standard message/event fields are represented in this object,
     * eventID, qualifier, error, suberr, payload,
     * S.200 "status" is represented by the exception field
     * in combination with the error:
     * <table><tt>
     * <tr><td>
     * statusOK:  </td><td> error == Error_OK;</td><td> exception == null
     * </td></tr><tr><td>
     * statusWarn:</td><td> error != Error_OK;</td><td> exception == null
     * </td></tr><tr><td>
     * statusFail:</td><td> error != Error_OK;</td><td> exception != null
     * </td></tr>
     * </tt></table>
     */
    public static class Event extends EventObject
	implements Async.Event, Dispatcher.DispatchableEvent
    {
	/** Note: EventObject.source is a MPI.Owner! */
	/** could be a MediaService or  a ContainerService object */
	// java.lang.Object source;	// inherited from EventObject

	/** true if this is dispatched as a nonTrans event. 
	 * Some eventIDs dispatch differently if they are non-trans.
	 */
	protected boolean isNonTrans = false;

	/** true iff this is service Request event. 
	 * Some eventIDs dispatch differently if they are requests.
	 */
	protected boolean isRequest = false;

	/** embedded event type identifier. From constructor */
	protected Symbol eventID = null;

	/** Indicates why this transaction completed. */
	protected Symbol qual = ESymbol.Any_Standard;  // Qualifier

	/** Indicates why this transaction failed. */
	protected Symbol error = ESymbol.Error_OK;

	/** Additional error information. */
	protected int suberror = 0;

	/** Contains S.200 event payload. */
	protected Dictionary payload = null;

	/** Indicates transaction did not complete successfully. */
	protected Exception exception = null;

	/** Indicates all fields are set, event ready for the EventQueue. 
	 * (not used for any semantic purpose, but marked for clarity...) 
	 */
	transient
	private boolean providerdone = false;

	/** Indicates all callbacks have been run,
	 * and event is ready for notifyAll().
	 * notifyAll() will release those threads that
	 * have done an explicit "event.sync(0, true)"
	 */
	transient
	private boolean listenerdone = false;

	/** an easier to parse/splice subset of toString(). */
	protected java.lang.String toString1() {
	    return "\n\t" +
		"source = " + source + ";\n\t" +
		"eventID = " + eventID + ";\n\t" +
		"exception = " + exception + ";\n\t" +
		"error = " + error + ";\n\t" +
		"suberror = " + suberror + ";\n\t" +
		"qualifier = " + qual + ";\n\t" +
		((payload != null)? ("payload = " + payload+";\n\t"):"") +
		"providerdone = " + providerdone +  ";\n\t" +
		"listenerdone = " + listenerdone +  ";\n\t" +
		"";
	}
	public java.lang.String toString() {
	    return getClass().getName() + "{" + this.toString1() + "}";
	}

	/** Construct specific completion or non-tranaction event. 
	 * @param source the MPI$Owner which dispatches this Dispather.DispatchableEvent.
	 * @param eventID a Symbol that identifies this event.
	 */
	public Event(java.lang.Object source, Symbol eventID) { 
	    super(source);
	    this.eventID = eventID;
	}

	/**
	 * Constructor for an event that is done.
	 * @param source the MediaService object that created this EventObject.
	 * @param eventID a Symbol that identifies the event. 
	 * @param exception an Exception to be thrown in invoking thread.
	 */
	public Event(java.lang.Object source, Symbol eventID, Exception exception) { 
	    this(source, eventID);
	    done(exception);
	}
    
	/** 
	 * Get the MediaService associated with the current event.
	 * <p>Defined in MediaEvent
	 * @return the MediaService associated with this event.
	 * @deprecated "use MediaServiceEvent.getMediaService()"
	 */
	// remove this method in JTAPI-2.0,
	// handled by MediaServiceEvent getMediaService()
	public MediaService getMediaService() {
	    if (source instanceof MediaService)
		return (MediaService)source; 
	    return null;
	}

	/**
	 * Get the Symbol that identifies this event.
	 * <p>Implements MediaEvent
	 * @return the Symbol that identifies this event
	 */
	public Symbol getEventID() {return eventID;}
	
	/**
	 * Get the Qualifier of this event.
	 * @return one of the qualifier Symbols (q_Something)
	 */
	public Symbol getQualifier() {waitForEventDone(); return qual;}
	
	/**
	 * Identify the reason or cause of an error or failure.
	 * <br>If this ResourceEvent <b>is</b> associated with an Exception,
	 * then getError() will identify the problem.
	 * <p>
	 * If this Event <b>is not</b> associated with an Exception,
	 * then getError() returns <code>ESymbol.Error_OK</code>.
	 * In such a case, use getWarning()
	 *
	 * @return one of the Error Symbols 
	 */
	public Symbol getError() {
	    waitForEventDone(); 
	    return (exception != null) ? error : ESymbol.Error_OK;
	}
	
	/**
	 * Return the error code, in the case where it was not
	 * a failure that generates an Exception.
	 * @return one of the Error Symbols 
	 */ 
	public Symbol getWarning() { waitForEventDone(); return error; }
	
	/**
	 * Get the suberror for this event.
	 * @return an int
	 */
	public int getSubError() { waitForEventDone(); return suberror; }
	
	/**
	 * Get the Payload of this event.
	 * @return a Dictionary of [additional] event information.
	 */
	public Dictionary getPayload() { waitForEventDone(); return payload; }

	/** 
	 * package accessor to check for an exception.
	 * sync(...) is not called, and stacktrace is not filled.
	 * Should be called only after providerdone!
	 */
	boolean isNoException() { return (exception == null); }

	/** package accessor to get the exception without sync(...);
	 * used while providerdone but not listenerdone
	 * @deprecated replaced by isNoException()
	 */
	Exception getException(boolean x) { return exception; }

	/** Simple waitForEventDone() and then return any Exception. 
	 * <!-- Stacktrace is filled in to this stack. -->
	 * <p>Implements Async.Event.
	 */
	public Exception getException() { 
	    waitForEventDone(); 
	    // should we enforce hiding of provider's stack trace?
	    // Hmm, if a Listener does getException, you see the Dispatcher stack...
	    // Hmm, this would cause a different stacktrace everytime you get it... 
	    // Ideally, we would cache the StackTrace when
	    // the async method is called, and stuff that in here!
	    // I doubt that Exception supports that...
	    // if (exception != null) { exception.fillInStackTrace(); }
	    return exception; 
	}

	/**
	 * waitForEventDone() and then throw if event holds a RuntimeException. 
	 * <p>Implements Async.Event
	 * <p>
	 * All synchronous API methds come here to synchronize,
	 * and this method does special synchronization to wait
	 * until all the Listeners for an event have run an returned.
	 * <p>
	 * <b>Note:</b>
	 * We believe the synch APIs are the <i>only</i> users of this method.
	 * If this method comes to be used in a context where
	 * this 'Listener' synchronization is not desired,
	 * then we could use the regular waitForEventDone() here
	 * and change Base_MediaService.java to invoke waitForListenersDone()
	 * directly before calling throwIf...Exception().
	 * <p>
	 * <b>Note:</b>
	 * This method catches any InterruptedException and
	 * throws it as a RuntimeException.
	 * If an application needs to catch the InterruptedException
	 * directly, it should use <tt>waitForEventDone(long)</tt>.
	 */
	public void throwIfRuntimeException() {
	    waitForListenersDone();		// wait for consumer done.
	    if (exception instanceof RuntimeException) {
		exception.fillInStackTrace();
		throw (RuntimeException)exception;
	    }
	}
	
	/** Thown when waitForEventDone() is interrupted by another Thread.
	 * Unlike <tt>InterruptedException</tt>
	 * this is a RuntimeException; the compiler does 
	 * not check for a catch.
	 * @see InterruptedException
	 * @see #waitForEventDone()
	 */
	public static final class SyncInterruptedException 
	    extends RuntimeException {
	    SyncInterruptedException() {
		super("InterruptedException during wait()");
	    }
	}

	/** waitForEventDone() and wait for Listener callbacks to return. 
	 * <p>
	 * Used by the synchronous API.
	 * <p>
	 * <b>Note:</b>
	 * This method catches any InterruptedException and
	 * throws it as a RuntimeException.
	 * If an application needs to catch the InterruptedException
	 * directly, it should use <tt>waitForEventDone(long)</tt>.
	 */
	public void waitForListenersDone() {
	    try {
		sync(0, true);
	    } catch (InterruptedException ex) { 
		throw new SyncInterruptedException();
	    }
	}

	/** 
	 * Waits for this event to complete.
	 * <p>Implements Async.Event
	 * <p>
	 * <b>Note:</b>
	 * This method catches any InterruptedException and
	 * throws it as a RuntimeException.
	 * If an application needs to catch the InterruptedException
	 * directly, it should use <tt>waitForEventDone(long)</tt>.
	 * <p>
	 * <b>Note:</b>
	 * Applications  should be aware
	 * that Listener callbacks may run before, during, or after
	 * the thread that calls <i>this</i> method is continued.
	 * In contrast,
	 * the synchonous API methods use waitForListenersDone()
	 * which blocks until all Listeners have been notified.
	 * @throws SyncInterruptedException, a RuntimeException
	 *	if another thread has interrupted the current thread.  
	 */
	public void waitForEventDone() {
	    try {
		sync(0, false);
	    } catch (InterruptedException ex) { 
		throw new SyncInterruptedException();
	    }
	}
	
	/** 
	 * Waits at most <tt>msecs</tt> for this event to complete.
	 * <p>Implements Async.Event
	 * @param msecs the time to wait in milliseconds,
	 * 	msecs = 0 means wait forever.
	 * @throws InterruptedException if another thread has interrupted
	 * 	the current thread.  The <i>interrupted status</i> of the
	 * 	current thread is cleared when this exception is thrown.
	 */
	final
	public void waitForEventDone(long msecs) throws InterruptedException {
	    sync(msecs, false); // 
	}

	/** internal, all purpose wait() method. 
	 * waits for isDone(mode).
	 * <p>
	 * <b>Note:</b> we do not expose this to applications.
	 * see discussion in isDone(boolean);
	 * <p>
	 * @param msecs the time to wait in milliseconds,
	 * 	msecs = 0 means wait forever.
	 * @param mode a boolean true iff invoked from synchonous API call.
	 * @throws InterruptedException if another thread has interrupted
	 * 	the current thread.  The <i>interrupted status</i> of the
	 * 	current thread is cleared when this exception is thrown.
	 * @see #isDone(boolean)
	 */
	synchronized final
	void sync(long msecs, boolean mode) 
	    throws InterruptedException {
	    long base = System.currentTimeMillis();
	    long now = 0;
	    
	    if (msecs < 0) {
		throw new IllegalArgumentException("timeout value is negative");
	    }
	    if (msecs == 0) {
		while (!isDone(mode)) {
		    wait(0);
		}
	    } else {
		while (!isDone(mode)) {
		    long delay = msecs - now;
		    if (delay <= 0) { break; }
		    wait(delay);
		    now = System.currentTimeMillis() - base;
		}
	    }
	}

	/**
	 * Indicates whether this Event has completed.
	 * <p>
	 * <i>Synchronized to ensure notify happens
	 *  before true is returned.</i>
	 * <P>
	 * <b>Note:</b> we do not expose this to applications.
	 * From their point of view, isDone() is providerdone.
	 * isDone(true) is only to be used by the synchronous
	 * API methods [via <tt>waitForListenersDone()</tt>], 
	 * to maintain the invariant that the callbacks
	 * will be processed before the synchronous methods continue.
	 * Apps that have asynchronous access to the async_event
	 * are on their own, and can use isDone().
	 * @return true iff this event has completed.
	 */
	synchronized final
        boolean isDone(boolean mode) {
	    return mode? listenerdone : providerdone;
	}


	/**
	 * Indicates whether this Event has completed.
	 * <p>Implements Async.Event
	 * @return true iff this event has completed.
	 */
	synchronized final
        public boolean isDone() {
	    // equivalent to isDone(false);
	    return providerdone;
	}
	/** extract common fields from an S.200 message Dictionary */
	public void setFields(Dictionary message) {
	    Dictionary eventData;
	    eventData = (Dictionary)message.get(ESymbol.Message_EventData);
	    if (eventData == null) eventData = message;	// use message
	    // Note: (eventData != null)
	    setFields((Symbol)message.get(ESymbol.Message_Qualifier),
		      (Symbol)message.get(ESymbol.Message_Error),
		      ((Integer)message.get(ESymbol.Message_SubError)).intValue(),
		      eventData);
	}

	/** Set extra info fields before done(Exception). 
	 * <p>
	 * <b>Note:</b> this is <b>not</b> synchronized, 
	 * it should be called from the same thread that [later] 
	 * calls <tt>done(Exception)</tt>.
	 * 
	 * @param qual a Symbol that identifies the cause of completion
	 * @param error a Symbol that identifies the cause of failure
	 * @param suberror an int with more info about the error
	 * @param payload a Dictionary of additional information (non null).
	 */
	// should change this to protected?
	// this is public, but should only be used by provider implementer.
	public void setFields(Symbol qual, Symbol error, 
			      int suberror, Dictionary payload) {
	    if (providerdone) return; // just ignore this late attempt.
	    this.qual = qual;
	    this.error = error;
	    this.suberror = suberror;
	    this.payload = payload; // this should not be null!
	    setFields();	// extract event specific fields
	}

	/** Hook for subclasses to extract their data from payload.
	 * invoked by generic setFields(qual, error, suberror, payload) 
	 */
	protected void setFields() {}

	public void setQualifier(Symbol qual) {
	    if (providerdone) return; // just ignore this late attempt.
	    this.qual = qual;
	}
	public void setError(Symbol error) {
	    if (providerdone) return; // just ignore this late attempt.
	    this.error = error;
	}
	public void setSubError(int suberror) {
	    if (providerdone) return; // just ignore this late attempt.
	    this.suberror = suberror;
	}
	public void setPayload(Dictionary payload) {
	    if (providerdone) return; // just ignore this late attempt.
	    this.payload = payload;
	}

	/** 
	 * Set the exception and mark as done. 
	 * Invokes .notifyAll() to release any .wait() invocations,
	 * making event fields available to users.
	 * <p>
	 * Used by implementations of MPI.MediaGroup and MPI.GroupProvider.
	 * <p> Implements MPI.Event
	 */
	// should change this to protected?
	// this is public, but should only be used by provider implementer.
	public synchronized void done(Exception exception) {
	    // lock out additional modifications:
	    if (providerdone) return;
	    providerdone = true; // supplier done, all fields filled.
	    this.exception = exception;
	    // call onEventDone so source can finish its processing
	    // and dispatch event to Listeners
	    // this lets Base_MediaService finish bindAndRelease before
	    // we notify anyone that the event is done.
	    // not likely that EventQueue will proceed to event.done()
	    // but this gives it a fighting chance...
	    ((MPI.Owner)source).onEventDone(this); // to the EventQueue
	    notifyAll(); 	// release threads doing waitForEventDone()
				// the sync-API methods are blocked until done()
	    // onEventDone() -> EventQueue.notifyListeners() -> done()
	}

	/* changed this to the Owner's responsibility: so that source
	 * can invoke callbacks before releasing waitForListenersDone()
	 * <p>
	 * done(Exception) indicates the PacketDispatcher thread is done,
	 * and then the event gets enqueued for the EventDispatcher thread.
	 * when EventDispatcher is done with event, it invokes done()
	 * which releases .waitForListenersDone() methods in the "main" thread.
	 */

	/** Invoked after EventDispatcher thread has processed this event. 
	 * This method marks the event as done for the "main" thread,
	 * and invokes .notifyAll() to release any .waitForListenersDone() 
	 * invocations.
	 * <p>
	 * This dance ensures that Listener callbacks are processed
	 * before the sync-API thread is released.
	 * @see #throwIfRuntimeException
	 */
	// package
	synchronized void done() { 
	    listenerdone = true; // async Listeners have been notified
	    notifyAll();	// now continue the synch API methods
	}

	/**
	 * Invoke the appropriate callback method on the appropriate Listener.
	 * Each sub-class must define an EventListener and callback method
	 * by supplying this method.
	 * <p>
	 * Implements Dispatcher.DispatchableEvent
	 * <p>
	 * Typically, Async.<i>Some</i>Event dispatches to like this:
	 * <br><code>
	 * <br> public void dispatch(EventListener listener) {
	 * <br>     if (listener instanceof Async.SomeListener)
	 * <br>         ((Async.SomeListener)listener).onSomeDone(this);
	 * <br> }	
	 * </code>
	 * <p>Invoked by Base_MediaService
	 * @param listener an EventListener, some kind of MediaListener.
	 */
	public void dispatch(EventListener listener) {
	    // no listener defined for this event
	}
	// arg is declared as java.lang.Object because we get it directly from a Vector.
	// and implementions test and cast anyway.
    }

    /**
     * Base class for other Events;
     * implements <tt>MediaServiceEvent.getMediaService()</tt>.
     */
    public static 
    class MediaServiceEvent extends Event 
	implements javax.telephony.media.MediaServiceEvent {
	/** 
	 * An Event from a MediaService.
	 * @param source a MediaService and MPI$Owner
	 * @param eventID a Symbol that identifies this event.
	 */
	public MediaServiceEvent(java.lang.Object source, Symbol eventID) {
	    super(source, eventID);
	}

	// Create a done MediaServiceEvent
	public MediaServiceEvent(java.lang.Object source, Symbol eventID, Exception exception) { 
	    super(source, eventID, exception);
	}

	/** 
	 * Get the MediaService associated with the current event.
	 * <p>Defined in MediaEvent
	 * @return the MediaService associated with this event.
	 */
	// move this method to Base.MediaServiceEvent, in 2.0
	public MediaService getMediaService() {
	    return (MediaService)source; 
	    // ASSERT source is the Owner.
	}
    }

    abstract
    public static
    class BindAndReleaseEvent extends MediaServiceEvent
	implements Async.BindAndReleaseEvent {
	public BindAndReleaseEvent(java.lang.Object source, Symbol eventID) {
	    super(source, eventID);
	}
	/** The group to which an Owner is bound. */
	protected MPI.MediaGroup mediaGroup = null;

	/**
	 * Internal access to the new MediaGroup binding.
	 * <p>
	 * <b>Note:</b>
	 * This method and the MediaGroup are accessible only
	 * to the provider/implementation.
	 * Used by Base_MediaService.
	 * <p>
	 * <b>Note:</b> This method does <b>not</b> waitForEventDone().
	 * This method is used within the onEventDone() callback
	 * so the event is providerdone but not listenerdone.
	 */
	//package
	MPI.MediaGroup getMediaGroup() { return mediaGroup; }

	/** any BindAndReleaseEvent may have BindCancelledException. */
	public void throwIfBindException() throws MediaBindException {
	    throwIfRuntimeException();
	    if (exception instanceof MediaBindException) {
		exception.fillInStackTrace();
		throw (MediaBindException)exception;
	    }
	}

	/** Used by other than vanilla ReleaseEvent. */
	public void throwIfConfigException() throws MediaConfigException {
	    throwIfRuntimeException();
	    if (exception instanceof MediaConfigException) {
		exception.fillInStackTrace();
		throw (MediaConfigException)exception;
	    }
	}
    }

    /** A release event with no documented exceptions. */ 
    public static
    class ReleaseEvent extends BindAndReleaseEvent 
	implements Async.ReleaseEvent {
	public ReleaseEvent(java.lang.Object source, Symbol eventID) {
	    super(source, eventID);
	}
	public void dispatch(EventListener listener) {
	    if (listener instanceof Async.BindAndReleaseListener)
		((Async.BindAndReleaseListener)listener).onReleaseDone(this);
	}
    }

    /**
     * Implements DelegationEvent and Async.DelegationEvent
     */
    public static
    class DelegationEvent extends BindAndReleaseEvent 
	implements Async.DelegationEvent {
	public DelegationEvent(java.lang.Object source, Symbol eventID) {
	    super(source, eventID);
	}
	/** sometimes delegation returns bound, sometimes not. */
	public void done(Exception exception, MPI.MediaGroup mediaGroup) {
	    this.mediaGroup = mediaGroup;
	    super.done(exception);
	}
	public void dispatch(EventListener listener) {
	    if (listener instanceof Async.DelegationListener) {
		if (eventID.equals(ev_DelegateToService))
		    ((Async.DelegationListener)listener).onDelegateToServiceDone(this);
		else if (eventID.equals(ev_Retrieve))
		    ((Async.DelegationListener)listener).onRetrieveDone(this);
		else if (eventID.equals(ev_ReleaseDelegated))
		    ((Async.DelegationListener)listener).onReleaseDelegatedDone(this);
	    }
	}
	
	public boolean reconfigurationFailed() {
	    waitForEventDone();
	    Boolean configFailed;
	    configFailed = (Boolean)payload.get(ESymbol.Group_HandoffConfig);
	    // configFailed may be null:
	    return (Boolean.TRUE.equals(configFailed));
	}
	
	/** the Tags used in the releaseToTags command.
	 * delivered to the recieving MediaService.
	 */
	public java.lang.String getReturnTag() {
	    waitForEventDone(); 
	    return (java.lang.String)payload.get(ESymbol.Group_Tag);
	}
	
	public Symbol getRetrieveCause() {
	    waitForEventDone(); 
	    return (Symbol)payload.get(ESymbol.Group_Cause);
	}
    }

    /** Completion Event for the bind methods. */
    public static
    class BindEvent extends BindAndReleaseEvent implements Async.BindEvent {
	public BindEvent(java.lang.Object source, Symbol eventID) {
	    super(source, eventID);
	}
	public BindEvent(java.lang.Object source) {
	    super(source, ev_BindToServiceName);
	}

	public void done(Exception exception, MPI.MediaGroup mediaGroup) {
	    this.mediaGroup = mediaGroup;
	    super.done(exception);
	}
	
	public void dispatch(EventListener listener) {
	    if (listener instanceof Async.BindAndReleaseListener)
		((Async.BindAndReleaseListener)listener).onBindDone(this);
	}
	public void throwIfConfigException() throws MediaConfigException {
	    throwIfRuntimeException();
	    if (exception instanceof MediaConfigException){
		exception.fillInStackTrace();
		throw (MediaConfigException)exception;
	    }
	}
    }
    
    /** Completion Event for bindToCall method. 
     * A BindEvent that carries a Call object.
     */
    public static
    class BindToCallEvent extends BindEvent 
	implements Async.BindToCallEvent {
	protected Connection connection = null; // read-only attribute
	public BindToCallEvent(java.lang.Object source, Symbol eventID) {
	    super(source, eventID);
	}
	public BindToCallEvent(java.lang.Object source) {
	    super(source, ev_BindToCall);
	}
	public Connection getConnection() {waitForEventDone(); return connection;}
	public void done(Exception exception, 
			 MPI.MediaGroup mediaGroup,
			 Connection connection) {
	    this.connection = connection;
	    super.done(exception, mediaGroup);
	}
	
	public void throwIfCallException() throws MediaCallException {
	    throwIfRuntimeException();
	    if (exception instanceof MediaCallException){
		exception.fillInStackTrace();
		throw (MediaCallException)exception;
	    }
	}
    }
    
    /** Base class of completion events for Async.MediaService methods. 
     * These events are dispatched to the MediaGroupListener.
     * <p>
     * <i>Is this just for MediaGroup methods?</i>
     */
    public static class MediaGroupEvent extends MediaServiceEvent
	implements Async.MediaGroupEvent {
	java.lang.Object retVal = null;
	public MediaGroupEvent(java.lang.Object source, Symbol eventID) {
	    super(source, eventID);
	}

	public void dispatch(EventListener listener) {
	    if(listener instanceof Async.MediaGroupListener)
		((Async.MediaGroupListener)listener).onMediaGroupDone(this);
	}
	public void done(Exception exception, java.lang.Object retVal){
	    this.retVal = retVal;
	    super.done(exception);
	}

	public Terminal getTerminal() {
	    waitForEventDone();
	    return (Terminal) ((retVal instanceof Terminal) ? retVal : null);
	}
	public java.lang.String getTerminalName() {
	    waitForEventDone();
	    return (java.lang.String) ((retVal instanceof java.lang.String) ? retVal : null);
	}
	public ConfigSpec getConfigSpec() {
	    waitForEventDone();
	    return (ConfigSpec) ((retVal instanceof ConfigSpec) ? retVal : null);
	}
	public Dictionary getDictionary() {
	    waitForEventDone();
	    return (Dictionary) ((retVal instanceof Dictionary) ? retVal : null);
	}
	// used for ev_Configure, was ConfigureEvent
	public void throwIfConfigException() throws MediaConfigException {
	    throwIfRuntimeException();
	    if (exception instanceof MediaConfigException){
		exception.fillInStackTrace();
		throw (MediaConfigException)exception;
	    }
	}
    }

    /** base class for all Async_ResourceEvent classes. 
     * each class should define its own fields, accessors,
     * constructors, dispatch, and done methods.
     */
    /* need to supply eventID and dispatch */
    abstract
    public static class ResourceEvent extends MediaServiceEvent
	implements Async.ResourceEvent 
    {
	/* ResourceEvent fields */
	/** indicates the rtcc that triggered this event to complete.
	 *  valid iff <tt>qual.equals(q_RTC)</tt> 
	 */
	protected Symbol rtcc = null; 		// RTCTrigger

	protected java.lang.String toString1() {
	    return "\n\t" +
		"source = " + source + ";\n\t" +
		"eventID = " + eventID + ";\n\t" +
		"exception = " + exception + ";\n\t" +
		"error = " + error + ";\n\t" +
		"suberror = " + suberror + ";\n\t" +
		"qualifier = " + qual + ";\n\t" +
		((rtcc != null)? ("RTCcondition = " + rtcc+";\n\t"):"") +
		((payload != null)? ("payload = " + payload+";\n\t"):"") +
		"isDone = " + isDone() +  ";\n\t" +
		"";
	}
	
	public ResourceEvent(java.lang.Object source, Symbol eventID) {
	    super(source, eventID);
	}
	
	/**
	 * Construct a ResourceEvent that is already done.
	 * Invokes:<tt>
	 * <br>setFields(qual, error, suberror, payload);
	 * <br>done(exception);
	 * </tt>
	 *
	 * @param source the java.lang.Object that created this EventObject.
	 * @param eventID a Symbol that identifies the event 
	 * @param exception the Exception to throw if completion failed
	 * @param qual a Symbol that identifies the cause of completion
	 * @param error a Symbol that identifies the cause of failure
	 * @param suberror an int with more info about the error
	 * @param payload a Dictionary of additional information.
	 */
	public ResourceEvent( java.lang.Object source, Symbol eventID, 
			      Exception exception,
			      Symbol qual, 
			      Symbol error,
			      int suberror,
			      Dictionary payload ) { 
	    super(source, eventID);
	    setFields( qual, error, suberror, payload );
	    done(exception);
	}
	
	/* ResourceEvent implementation */
	
	/**
	 * Get the RTC Trigger that caused this resource action.
	 * This is non-null only if <tt>getQualifier == ResourceEvent.q_RTC</tt>
	 * <p>
	 * The Symbol returned is one of the rtcc_[Name] Symbols;
	 * possibly a ResourceEvent.ev_[Name] event.
	 * @return a Symbol identifying an RTC Trigger condition.
	 */
	public Symbol getRTCTrigger() {
	    waitForEventDone(); 
	    if ( rtcc == null ) 
		rtcc = (Symbol)payload.get(ESymbol.Group_RTCTrigger);
	    return rtcc;
	}
	
	/* AsyncResourceEvent implementation methods */
	
	/**
	 * used by BasicMediaService to generate Synchronous interface.
	 */
	public void throwIfMediaResourceException()
	    throws MediaResourceException {
	    throwIfRuntimeException(); // a convenience for BasicMediaService
	    if (exception instanceof MediaResourceException) {
		exception.fillInStackTrace();
		throw (MediaResourceException)exception;
	    }
	    // ASSERT (exception == null); maybe even SET(exception = null);
	}
    }
    
    /** The MediaEvent delivered to MediaServiceListener.
     * The various EventIDs are Dispatched to:
     * <ul><li>
     * <tt>ev_disconnected</tt> and <tt>ev_terminalIdle</tt>
     * go to <tt>onDisconnected()</tt>.
     * </li><li>
     * <tt>ev_connected</tt> goes to <tt>onConnected()</tt>.
     * </li><li>
     * <tt>ev_Retrieved</tt> goes to <tt>onRetrieved()</tt>.
     * </li></ul>
     * <p>
     * This is distinct from the ResourceEvent delivered
     * in a DisconnectedException.
     */
    public static class DisconnectedEvent extends MediaServiceEvent {
	/** Create this [non-transactional] Event already done. 
	 * Immediately invokes <tt>onEventDone()</tt> and queues for delivery.
	 * <p>
	 * @param source the Owner MediaService to which event is delivered.
	 * @param eventID a state Symbol: 
	 *        <tt>ev_Disconnected</tt> or <tt>ev_Connected</tt>
	 *        or <tt>ev_Retrieved</tt>.
	 * @param ex is generally null. 
	 * (this event is not returned to sync methods).
	 */
	public DisconnectedEvent(java.lang.Object source, Symbol eventID, Exception ex) {
	    super(source, eventID, ex);
	}
	public DisconnectedEvent(java.lang.Object source, Symbol eventID) {
	    super(source, eventID);
	}
	public DisconnectedEvent(java.lang.Object source) {
	    this(source, ev_Disconnected);
	}
	public void dispatch(EventListener listener) {
	    if(listener instanceof MediaServiceListener)
		if (eventID.equals(ev_Connected)) {
		    ((MediaServiceListener)listener).onConnected(this);
		} else if (eventID.equals(ev_Disconnected)) {
		    ((MediaServiceListener)listener).onDisconnected(this);
		} else if (eventID.equals(ev_TerminalIdle)) {
		    ((MediaServiceListener)listener).onDisconnected(this);
		} else if (eventID.equals(ev_Retrieved)) {
		    ((MediaServiceListener)listener).onRetrieved(this);
		} else {
		    // ignore?
		}
	}
    }

    /**
     * Specialization of MediaServiceEvent for <tt>ev_Delegated</tt>.
     * dispatches to DelegationListener.onDelegated.
     */
    public static class DelegatedEvent extends MediaServiceEvent
	implements DelegationConstants {
	// this might have extended DelegationEvent, but it 
	// shares none of the data/accessors used in DelegationEvent
	// This is more closely related to the DisconnectedEvent
	// like onRetrieved. the DelegationListener could extend MSListener...
	/** Create this [non-transactional] Event already done. 
	 * Immediately invokes onEventDone() and queues for delivery.
	 * <p>
	 * @param source the Owner MediaService to which event is delivered.
	 * @param eventID the Symbol <tt>ev_Delegated</tt>
	 * @param ex an Exception; should be <tt>null</tt> 
	 * (this event is not returned to sync methods).
	 */
	public DelegatedEvent(java.lang.Object source, Symbol eventID, Exception ex) {
	    super(source, eventID, ex);
	}
	/** Create this [non-transactional] Event already done. 
	 * Immediately invokes onEventDone() and queues for delivery.
	 * <p>
	 * eventID is set to <tt>ev_Delegated</tt>
	 * exception is set to <tt>null</tt> 
	 * <p>
	 * @param source the Owner MediaService to which event is delivered.
	 * (this event is not returned to sync methods).
	 */
	public DelegatedEvent(java.lang.Object source) {
	    super(source, ev_Delegated, null);
	}

	public void dispatch(EventListener listener) {
	    if(listener instanceof DelegationListener)
		((DelegationListener)listener).onDelegated(this);
	}
    }
}
