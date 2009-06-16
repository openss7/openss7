/*
 * Async.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import javax.telephony.*;		// get core Objects and Exceptions
import javax.telephony.media.*;
import java.util.EventListener;
import java.util.Dictionary;

/**
 * Asynchronous Event and Listener interfaces.
 * <p>
 * What an application sees from an asynchronous event.
 */
public
interface Async {
    /**
     * Asynchronous version of the MediaEvent.
     * Methods aupported by asynchronous events.
     * Other Async Event interfaces extend from this.
     * <p>
     * An Async.Event [that is, a <i>transaction</i>] has methods:<pre>
     *  isDone();               // test for done-ness
     *  waitForEventDone();     // wait until isDone()
     *  waitForListenersDone(); // wait until isDone() and Listeners have returned
     *  getException();         // test failure/success</pre>
     * <p>
     * The MediaEvent methods <tt>getEventID</tt> 
     * and <tt>getSource()</tt> (and various flavors of it)
     * immediately return the requested value.
     * <P>
     * <b>Note:</b> <tt>getException()</tt> and
     * any accessor for the results of an operation 
     * (for example, <tt>getError()</tt>,
     * <tt>ResourceEvent.getQualifier()</tt>
     * or <tt>BindToCallEvent.getCall()</tt>)
     * first <i>waits</i> for the event to complete
     * (using <tt>waitForEventDone()</tt>), 
     * and then return the requested value.
     * If you do not want to wait, then test first using <tt>isDone()</tt>
     * to see if the event is complete.
     * <p>
     * <b>Implementation Note:</b>
     * The Event's source is an MPI.Owner:
     * a MediaService, ContainerService, etc.
     * When an Async.Event completes, a notification is sent 
     * to the Event's source using <tt>(MPI.Owner).onEventDone()</tt>.
     * The owner dispatches the event to any Listeners.
     * See the MPI for more details.
     * @see javax.telephony.media.provider.MPI
     * <p>
     */
    public
    interface Event extends MediaEvent {
	/**
	 * Indicates whether this Event has completed.
	 *
	 * @return true iff this event has completed.
	 */
	boolean isDone();

	/** 
	 * Wait for this asynchronous event to complete.
	 * <p>
	 * <b>Note:</b> to get direct access to any InterruptedException
	 * use waitForEventDone(long) instead.
	 *
	 * @throws a RuntimeException
	 *	if another thread interrupts the current thread.  
	 *      That is, any InterruptedException is wrapped in
	 *      a RuntimeException.
	 */
	void waitForEventDone();

	/** 
	 * Waits at most <tt>msecs</tt> for this event to complete.
	 * <p>Implements Async.Event
	 * @param msecs the time to wait in milliseconds,
	 * 	msecs = 0 means wait forever.
	 * @throws InterruptedException if another thread has interrupted
	 * 	the current thread.  The <i>interrupted status</i> of the
	 * 	current thread is cleared when this Exception is thrown.
	 */
	void waitForEventDone(long msecs) throws InterruptedException;

	/** 
	 * Wait for this event to complete and Listeners to complete.
	 * <p>
	 * This method is used by the synchronous API to wait
	 * for Listeners to complete before continuing the "main" thread.
	 *
	 * @throws a RuntimeException
	 *	if another thread interrupts the current thread.  
	 *      That is, any InterruptedException is wrapped in
	 *      a RuntimeException.
	 */
	void waitForListenersDone();

	/**
	 * waitForListenersDone() and then throw a RuntimeException, if any.
	 * Throw if requested operation resulted in a RuntimeException.
	 * <p>
	 * <b>Note:</b> other events define methods to throw other classes
	 * of Exceptions, and those methods invoke this one internally.
	 * Therefore all the <tt>throwIf()</tt> methods waitForListenersDone,
	 * and also throw any RuntimeException in the event.
	 * 
	 * @throws RuntimeException if the event holds a RuntimeException.
	 */
	void throwIfRuntimeException() throws RuntimeException;
    }
    
    /** An Async.Event from a MediaService; includes getMediaService(). */
    public interface MediaServiceEvent extends Event, javax.telephony.media.MediaServiceEvent {}

    /** Listener for Async.BindAndReleaseEvents. 
     * Invoked when Async.MediaService.Bind/Release methods complete.
     */
    public
    interface BindAndReleaseListener extends MediaListener {
	/** Invoked when a BindEvent is done. 
	 * @param event  a MediaServiceEvent object
	 */
	void onBindDone(javax.telephony.media.MediaServiceEvent event); // an Async.BindEvent
	/** Invoked when a ReleaseEvent is done.  
	 * @param event  a MediaServiceEvent object
	 */
	void onReleaseDone(javax.telephony.media.MediaServiceEvent event);// an Async.ReleaseEvent
    }
    /**
     * Listener for Async.DelegationEvents.
     * Invoked when Async_Delegation methods complete.
     */
    // Signatures should probably have used DelegationEvent:
    public 
    interface DelegationListener extends MediaListener /* BindAndReleaseListener */ {
	/**
	 * Indicates that async_delegateToService() has completed.
	 * <p>
	 * If the MediaGroup was returned to this MediaService, 
	 * then <tt>isBound()</tt> and <tt>isOwner()</tt> are <tt>true</tt>:
	 * <ul><li>
	 * If the MediaGroup was returned to this MediaService, but could not
	 * be reconfigured as it was when <tt>delegateToService()</tt>
	 * was called, then <tt>reconfigurationFailed()</tt> is <tt>true</tt>.
	 * </li><li>
	 * If <i>this</i> MediaService used <tt>retrieve()</tt> 
	 * then <tt>event.getQualifier.equals(q_Retrieved)</tt>
	 * and <tt>getMediaService().isBound()</tt> is <tt>true</tt>.
	 * </li></ul>
	 * <p>
	 * If the MediaGroup was returned to some other MediaService,
	 * then <tt>isBound()</tt> and <tt>isOwner()</tt> are <tt>false</tt>:
	 * <ul><li>
	 * If some <i>older</i> owner used <tt>retrieve()</tt>
	 * then <tt>event.getQualifier.equals(q_Retrieved)</tt>,
	 * <tt>event.getRetrieveCause()</tt> is valid,
	 * and <tt>getMediaService().isBound()</tt> is <tt>false</tt>.
	 * </li><li>
	 * If the most recent owner used <tt>releaseToTag()</tt>
	 * and the tags were not caught by this or any more recent owner,
	 * then <tt>event.getQualifier.equals(q_Released)</tt>
	 * and <tt>event.getReturnTags()</tt> is valid.
	 * </li><li>
	 * If this MediaService released using <tt>releaseDelegated</tt>
	 * then <tt>event.getQualifier.equals(q_Removed)</tt>
	 * </li></ul>
	 *
	 * @param event a MediaServiceEvent that isDone().
	 */
	void onDelegateToServiceDone(javax.telephony.media.MediaServiceEvent event);
	
	/** 
	 * Indicates that async_retrieve() has completed.
	 * <p>
	 * @param event a MediaServiceEvent that may contain an Exception
	 * if <tt>async_retrieve()</tt> could not be accomplished.
	 */
	void onRetrieveDone(javax.telephony.media.MediaServiceEvent event);

	/**
	 * Indicates that async_releaseDelegated() has completed.
	 * <p>
	 * @param event a MediaServiceEvent that may contain an Exception
	 * if <tt>async_releaseDelegated()</tt> could not be accomplished.
	 */
	void onReleaseDelegatedDone(javax.telephony.media.MediaServiceEvent event);
    }
    
    
    /** Marks the events from the GroupProvider. 
     * These events contain the new MediaGroup binding.
     * <P>
     * For a ReleaseEvent, the contained MediaGroup is always <tt>null</tt>.
     * And a ReleaseEvent does not carry MediaConfigException.
     * <p>
     * Any BindAndReleaseEvent can throw a BindCancelledException.
     */
    public
    interface BindAndReleaseEvent extends MediaServiceEvent {
	/** Detect and throw MediaBindException and RuntimeException.
	 * <p>Calls <tt>throwIfRuntimeException</tt> internally. 
	 * @throws MediaBindException
	 */
	void throwIfBindException() throws MediaBindException; 

	/**
	 * Detect and throw MediaConfigException and RuntimeException.
	 * Used for MediaGroupEvent from async_configure().
	 * <p>Calls <tt>throwIfRuntimeException</tt> internally. 
	 * @throws MediaConfigException
	 */
	void throwIfConfigException() throws MediaConfigException;
    }

    /**
     * Return event when binding.
     * Contains a MediaGroup or an Exception.
     * <p>
     * Success indicates <tt>(isBound()==true)</tt> and <tt>(isOwner==true)</tt>.
     * <p>
     * These events invoke 
     * {@link Async.BindAndReleaseListener#onBindDone onBindDone}
     * when they are done. 
     * <p>
     * <b>Note:</b>
     * The MediaGroup is accessible only to the provider/implementation.
     */
    public
    interface BindEvent extends BindAndReleaseEvent {};
    
    /** Return event when binding to a Call.
     * This contains a Connection.
     * @see Async.MediaService#async_bindToCall
     * @see Async.MediaService#async_bindAndConnect
     */
    public
    interface BindToCallEvent extends BindEvent {
	/** Detect and throw MediaCallException and RuntimeException.
	 * <p>Calls <tt>throwIfRuntimeException</tt> internally. 
	 * @throws MediaCallException
	 */
	void throwIfCallException() throws MediaCallException;

	/** Get the Connection object associated with this method.
	 * @return a JTAPI Connection object. */
	Connection getConnection();
    }

    /** return type from async_release(). 
     * A BindAndReleaseEvent with no extra accessors.
     * <p>
     * Success indicates <tt>(isBound()==false)</tt>.
     * <p>
     * These events invoke 
     * {@link Async.BindAndReleaseListener#onReleaseDone onReleaseDone}
     * when they are done. 
     * <p>
     * The new media group is always null.
     */
    public
    interface ReleaseEvent extends BindAndReleaseEvent {};

    /** Return type from async_delegateToService(). */
    public
    interface DelegationEvent extends javax.telephony.media.DelegationEvent, 
				      BindAndReleaseEvent {
	// extends BindAndReleaseEvent for the throwIf methods.
    };
    
    /**
     * Listener for MediaGroup method transaction completion events.
     * Completion callbacks for <tt>async_configure()</tt>,
     * <tt>async_getTerminal()</tt>, <tt>async_setParameters()</tt>, etc. 
     * <p>
     * <b>Note:</b>
     * These callbacks are also invoked when the synchronous versions
     * of the MediaService methods complete.
     * <p>
     * <b>Note:</b>Bind and Release completions do <i>not</i> come here.
     * Those are delivered to the <tt>Async.BindAndReleaseListener</tt>.
     */
    public
    interface MediaGroupListener extends MediaListener {
	/** 
	 * Universal callback for MediaGroup method completion events.
	 * The eventID indicates which method has completed.
	 * To extract event-specific values, just check <tt>getEventID()</tt>
	 * and then extract the appropriate field.
	 * <p>
	 * The eventID name is "<tt>ev_MethodName</tt>"
	 * except for method <tt>MediaService.stop()</tt>;
	 * for this method, the event is named "<tt>ev_GroupStop</tt>"
	 * to avoid conflict with the Resource events' "<tt>ev_Stop</tt>"
	 * <p>
	 * For example, to get the terminal name String when
	 * <tt>getTerminalName()</tt> completes, use: <pre>
	 *     if (event.getEventID().equals(ev_GetTerminalName)) 
	 *        String name = event.getTerminalName();</pre>
	 * <p>
	 * <b>Note:</b> at this time there is no justification
	 * to define additional callback methods specialized for
	 * each MediaGroup method.
	 * <p>
	 * If an application developer is using these extensively,
	 * it is fairly easy to define an adapter class that
	 * dispatches to application defined methods such as:
	 * <br><tt> void onGetTerminalDone(MediaGroupEvent event);</tt>
	 * <br><tt> void onGetTermNameDone(MediaGroupEvent event);</tt>
	 * <br><tt> void onGetConfigDone(MediaGroupEvent event);</tt>
	 * <br><tt> void onGetDictDone(MediaGroupEvent event);</tt>
	 *
	 * @param event a MediaGroupEvent that is done.
	 */
	void onMediaGroupDone(javax.telephony.media.MediaGroupEvent event);
    }

    /**
     * Marks those Async events that come from the MediaGroup methods.
     * But not those from the GroupProvider or Resources. 
     * <P>
     * These events invoke 
     * {@link Async.MediaGroupListener#onMediaGroupDone onMediaGroupDone}
     * when they are done. 
     */
    public
    interface MediaGroupEvent extends MediaServiceEvent, javax.telephony.media.MediaGroupEvent {
	/**
	 * Detect and throw MediaConfigException and RuntimeException.
	 * Used for MediaGroupEvent from async_configure().
	 * <p>Calls <tt>throwIfRuntimeException</tt> internally. 
	 * @throws MediaConfigException
	 */
	void throwIfConfigException() throws MediaConfigException;
    }

    /**
     * Extends ResourceEvent to include asynchronous event methods.
     * <p>
     * The MediaEvent methods 
     * (<tt>getEventID()</tt>, <tt>getMediaService()</tt>)
     * immediately return the requested values.
     * <p>
     * The implementation of the other Resource.Event accessors 
     * (getQualifier(), getRTCTrigger(), and getError())
     * <b>wait</b> for the event to complete, then return the requested value.
     * If you do not want to wait, then test first using <tt>isDone()</tt>
     * to see if the event is complete.
     */
    public
    interface ResourceEvent extends MediaServiceEvent, javax.telephony.media.ResourceEvent
    {
	/** 
	 * Detect and throw MediaResourceExceptions
	 * and RuntimeExceptions contained in the Event.
	 * <p>Calls <tt>throwIfRuntimeException</tt> internally. 
	 * @throws MediaResourceException if contained in this event
	 */
	void throwIfMediaResourceException() throws MediaResourceException;
    }

    /** 
     * Asynchronous methods for a MediaService.
     * <p>
     * Async.MediaService defines a method corresponding to each
     * synchronous MediaService method.
     * For each synchronous <tt>methodName(arglist)</tt>, 
     * the corresponding asynchronous methods is named:
     * <tt>async_methodName</tt>, and has exactly the same
     * <tt>arglist</tt> as the synchronous <tt>methodName</tt>.
     * <p>
     * The return values are always some <tt>Async.Event</tt>.
     * The asynchronous methods throw the subset of Exceptions
     * that are detected before attempting the operation of the method.
     * The other Exceptions are delivered in the returned <tt>Async.Event</tt>
     * when that event is done.
     */
    public
    interface MediaService {
	/**
	 * Async version of bindAndConnect.

	 * @param cs  a ConfigSpec object
	 * @param s1  a String specifing the origAddr
	 * @param s2  a String specifing the dialDigits

	 * @return an Async.BindToCallEvent for onBindDone();
	 * @throws MediaBindException for preconditions; 
	 * <tt>AlreadyBoundException</tt> or
	 * <tt>BindInProgressException</tt>
	 * @see MediaService#bindAndConnect
	 */
	BindToCallEvent async_bindAndConnect(ConfigSpec cs, String s1, String s2)
	    throws MediaBindException;
	/**
	 * Async version of bindToCall.
	 *
	 * @param cs    a ConfigSpec object
	 * @param call  a Call object
	 * @return an Async.BindToCallEvent for onBindDone();
	 * @throws MediaBindException for preconditions; 
	 * <tt>AlreadyBoundException</tt> or
	 * <tt>BindInProgressException</tt>
	 * @see MediaService#bindToCall
	 */
	BindToCallEvent async_bindToCall(ConfigSpec cs, Call call)
	    throws MediaBindException;
	/**
	 * Async version of bindToServiceName.
	 * @param cs    a ConfigSpec object
	 * @param serviceName a String
	 * @throws MediaBindException for preconditions; 
	 * <tt>AlreadyBoundException</tt> or
	 * <tt>BindInProgressException</tt>
	 * @return an Async.BindEvent for onBindDone();
	 * @see MediaService#bindToServiceName
	 */
	BindEvent async_bindToServiceName(ConfigSpec cs, String serviceName)
	    throws MediaBindException;
	/**
	 * Async version of bindToTerminalName.
	 * @param cs    a ConfigSpec object
	 * @param s1	a terminal name String
	 * @throws MediaBindException for preconditions; 
	 * <tt>AlreadyBoundException</tt> or
	 * <tt>BindInProgressException</tt>
	 * @return an Async.BindEvent for onBindDone();
	 * @see MediaService#bindToTerminalName
	 */
	BindEvent async_bindToTerminalName(ConfigSpec cs, String s1)
	    throws MediaBindException;
	/**
	 * Async version of bindToTerminal.
	 * @param cs    a ConfigSpec object
	 * @param t1	a JTAPI Terminal object
	 * @throws MediaBindException for preconditions; 
	 * <tt>AlreadyBoundException</tt> or
	 * <tt>BindInProgressException</tt>
	 * @return an Async.BindEvent for onBindDone();
	 * @see MediaService#bindToTerminal
	 */
	BindEvent async_bindToTerminal(ConfigSpec cs, Terminal t1)
	    throws MediaBindException;
	
	/**
	 * Async version of release().
	 * @throws MediaBindException a <tt>BindInProgressException</tt>
	 * @return an Async.ReleaseEvent for onReleaseDone();
	 * @see MediaService#release
	 */
	ReleaseEvent async_release() throws MediaBindException;

	/**
	 * Async version of releaseToService().
	 *
	 * @param serviceName  a String
	 * @param timeout      an int specifying timeout in milliseconds
	 * @throws MediaBindException a <tt>BindInProgressException</tt>
	 * @return an Async.ReleaseEvent for onReleaseDone();
	 * @see MediaService#releaseToService
	 */
	ReleaseEvent async_releaseToService(String serviceName, int timeout)
	    throws MediaBindException;
	
	/**
	 * Async version of releaseToTag().
	 * @param returnTag a String of tags to match delegated <tt>catchTags</tt>
	 * @throws NotBoundException if not currently bound to a MediaGroup.
	 * @throws MediaBindException a BindInProgressException
	 * @return an Async.ReleaseEvent for onReleaseDone();
	 * @see MediaService#releaseToTag
	 */
	ReleaseEvent async_releaseToTag(String returnTag) 
	    throws NotBoundException, MediaBindException;
	
	/** 
	 * Async version of releaseToDestroy().
	 * @throws MediaBindException a BindInProgressException
	 * @return an Async.ReleaseEvent for onReleaseDone();
	 * @see MediaService#releaseToDestroy
	 */
	ReleaseEvent async_releaseToDestroy() throws MediaBindException;
	
	/**
	 * Initiate a cancelBindRequest().
	 * This method is defined to complete immediately, 
	 * there is no Listener for the completion of the Event.
	 * <p>
	 * Operationally identical to cancelBindRequest(), but returns an event.
	 *
	 * @return an Async.Event that is already done().
	 * @see MediaService#cancelBindRequest
	 */
	Event async_cancelBindRequest(); // Event or void? isa Transaction?
	
	/** Async version of MediaService.configure().
	 *
	 * @return a MediaGroupEvent object
	 * @param cs a ConfigSpec object
	 * @see MediaService#configure
	 */
	MediaGroupEvent async_configure(ConfigSpec cs);
	/** Async version of MediaService.getConfiguration().
	 *
	 * @return a MediaGroupEvent object
	 * @see MediaService#getConfiguration
	 */
	MediaGroupEvent async_getConfiguration();
	/** Async version of MediaService.getTerminalName().
	 *
	 * @return a MediaGroupEvent object
	 * @see MediaService#getTerminalName
	 */
	MediaGroupEvent async_getTerminalName();
	/** Async version of MediaService.getTerminal().
	 *
	 * @return a MediaGroupEvent object
	 * @see MediaService#getTerminal
	 */
	MediaGroupEvent async_getTerminal();
	
	// a transaction, but no special accessors
	/** Trigger an RTC action from the application. 
	 *
	 * @param rtca an RTC action Symbol
	 * @return a MediaGroupEvent object
	 * @deprecated "use transactional Resource commands". 
	 * @see MediaService#triggerRTC
	 */
	MediaGroupEvent async_triggerRTC(Symbol rtca);
	/** Async version of MediaService.stop().
	 * @return a MediaGroupEvent object
	 * @see MediaService#stop
	 */
	MediaGroupEvent async_stop();
	
	/** Async version of MediaService.getUserValues().
	 * @param keys an array of Symbols indicating parameters to get
	 * @return a MediaGroupEvent object
	 * @see MediaService#getUserValues
	 */
	MediaGroupEvent async_getUserValues(Symbol[] keys);
	/** Async version of MediaService.setUserValues().
	 * @param dict a Dictionary of user values
	 * @return a MediaGroupEvent object
	 * @see MediaService#setUserValues
	 */
	MediaGroupEvent async_setUserValues(Dictionary dict);
	/** Async version of MediaService.setUserDictionary().
	 * @param dict a Dictionary of user values
	 * @return a MediaGroupEvent object
	 * @see MediaService#setUserDictionary
	 */
	MediaGroupEvent async_setUserDictionary(Dictionary dict);
	/** Async version of MediaService.getParameters().
	 * @param keys an array of Symbols indicating parameters to get
	 * @return a MediaGroupEvent object
	 * @see MediaService#getParameters
	 */
	MediaGroupEvent async_getParameters(Symbol[] keys);
	/** Async version of MediaService.setParameters().
	 * @param dict a Dictionary of parameters to set
	 * @return a MediaGroupEvent object
	 * @see MediaService#setParameters
	 */
	MediaGroupEvent async_setParameters(Dictionary dict);
    }
}
