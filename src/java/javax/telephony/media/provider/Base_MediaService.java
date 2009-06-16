/*
 * Base_MediaService.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import javax.telephony.*;	// get core objects and Exceptions
import javax.telephony.media.*;
import javax.telephony.media.async.*;
import java.util.EventListener;
import java.util.Dictionary;

// Generated: %?% %D% %?% Fri Oct 18 15:39:28 PDT 2002% %?%
/* %?gen% 
%(import javax.telephony.media.provider 
  javax.telephony.media.async
  javax.telephony.media 
  javax.telephony
  java.util)%
%(implements MediaService Async$MediaService Delegation Async_Delegation)%
%(javagen "Base_MediaService.scm")%
%?gen% 
%?gen%
 */
/**
 * Reference implementation of MediaService.
 * This implementation of <tt>MediaService</tt> and <tt>Async.MediaService</tt>
 * delegates to <tt>MPI.GroupProvider</tt> and <tt>MPI.MediaGroup</tt>,
 * but manages the sync/async method layering, Listener and event dispatching.
 * <p>
 * Communication back to the <tt>MediaService</tt> is via <tt>Async.Event</tt>s,
 * which extend <tt>MediaEvent</tt> to include completion and notification 
 * methods.
 * The <tt>Base.Event</tt> classes are used within the provider package, 
 * and they contain additional event processing and framework control
 * methods.
 * <p>
 * <b>Note:</b><br>
 * Why <tt>Base_MediaService</tt> sits between MediaService and the MPI:
 * <p>
 * <tt>Base_MediaService</tt> provides a controlled implementation between the
 * application and the MediaGroup implementation. The <tt>MediaService</tt> 
 * and <tt>MPI</tt> interfaces can be used reliably by each side
 * (application developer and <tt>MediaGroup</tt> implementor), 
 * while the <tt>Base_MediaService</tt> class can be modified to provide 
 * services and adaptations for various frameworks.
 * <tt>Base_MediaService</tt> enforces protocols and invariants that the 
 * <tt>MediaGroup</tt> would not trust the application to implement rigorously, 
 * and that the application and/or <tt>MediaGroup</tt> would find tedious to 
 * implement explicitly.
 * <p>
 * <b>Note:</b> provider implementers are encouraged to consult the
 * source code for this and other provider package classes.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public class Base_MediaService extends Base_Owner 
     implements MPI.Owner,
		MediaService, Async.MediaService, Delegation, Async_Delegation
{
    public String toString() {
	return super.toString()
	    + ":" + ((_group==null) ? "unBound" : _group.toString());
    }

    /** The GroupProvider object to work with */
    private MPI.GroupProvider _provider = null;
    
    /** The MediaGroup bound to this MediaService or null. 
     * <p>
     * <br>if((_event != null) && (!_event.isDone())) then BindInProgress.
     * <br>if (_group != null) then isBound().
     * <br>if (_group == null) then !isBound().
     */
    volatile
    private MPI.MediaGroup _group = null;
    
    /**
     * Verifies whether service is bound to a group, or a bind is in progress.
     * Methods setting _event are synchronized. 
     * <p>
     * <br>(_event == null) then no BindInProgress, _group is bound or null. 
     * <br>(_event != null) then BindInProgress
     * <br>(_event != null) && (event.isDone()) waiting for onBindDone()
     */
    volatile
    private Base.BindAndReleaseEvent _event = null;

    /**
     * Holds a DelegationEvent when delegation is in progress. 
     * <br><tt>(_devent == null)</tt> then no DelegateToService:
     * isOwner() = isBound()
     * <br><tt>(_devent != null)</tt> then in DelegateToService:
     * isOwner() = !_event.isDone()
     * <br><tt>((_devent != null) && (_event==null)):</tt> then onDelegated(), 
     * still waiting for <tt>onDelegateToServiceDone()</tt>
     */
    volatile
    private Base.DelegationEvent _devent = null;

    /** holds a DelegationEvent while one of the special 'cancel Delegation'
     * commands is in progress.
     */
    // potential addition if we need to avoid duplicate retrieve/releaseDelegated
    // volatile private Base.DelegationEvent _revent = null;

    /** for retrieve/releaseDelegated. */
    private
    MPI.MediaGroup checkInDelegation() 
	throws NotBoundException, BindInProgressException, NotDelegatedException {
	if (_group == null) throw new NotBoundException();
	if (_devent == null) throw new NotDelegatedException();
	// we could do this, but probably not necessary
	// no serious danger from duplicate retrieve/releaseDelegated requests?
	// if (_revent != null) throw new BindInProgressException();
	if (_event != null) throw new BindInProgressException();
	return _group;
    }
    /** for Bind methods. */
    private
    void checkBindable() throws AlreadyBoundException, BindInProgressException {
	if (_group != null) throw new AlreadyBoundException();
	if (_devent != null) throw new NotOwnerException();
	if (_event != null) throw new BindInProgressException();
    }
    /** for DelegateToService and Release methods. */
    private 
    void checkReleaseable()
	throws NotBoundException, BindInProgressException, NotOwnerException {
	if (_group == null) throw new NotBoundException();
	if (_devent != null) throw new NotOwnerException();
	if (_event != null) throw new BindInProgressException();
    }
    /**
     * Returns the bound MediaGroup, for MediaGroup methods.
     * If no MediaGroup is bound, throw the RuntimeException NotBoundException.
     * <p>
     * This is "private" scoped so the group object can not be
     * accessed outside of the provider package.
     * <p>
     * Called from within synchronized methods
     * 
     * @throws NotBoundException if not currently bound to a MediaGroup
     * @return the MediaGroup to which this MediaService is bound.
     */
    private
    MPI.MediaGroup checkGroup() throws NotBoundException, NotOwnerException { 
	if (_group == null) throw new NotBoundException();
	if (_devent != null) throw new NotOwnerException();
	return _group;
    }

    // Not necessary to synchronize these?
    // Because we don't care if we lose a race/window.
    // and if these are used in debugging, we don't want to
    // wait on a stuck lock... ?

    /**
     * @return true iff bind or release is in progress.
     */
    public boolean isBinding() { return (_event != null); }

    /**
     * @return true iff this MediaService is bound to a MediaGroup.
     */
    public boolean isBound() { return (_group != null); }

    /**
     * @return true iff this MediaService if delegation in progress
     */
    public boolean isDelegated() { return (_devent != null); }

    /**
     * @return true iff this MediaService is bound and not delegated.
     */
    // Note: setGroup() insures that if (_group != null) then (_event == null).
    // However: while delegated (_group != null) && (_event != null) !
    // In that case, we also set && (_devent != null) to indicate delegation.
    // So, checking (_event == null) may be redundant, but it closes any gaps...
    // This returns false when delegation is initiated.
    public boolean isOwner() {
	return ((_group != null) && (_devent == null) && (_event == null));
    }

    /** special Listener to invoke setGroup when Bind and Release events are done. */ 
    private
    BindAndReleaseListener _bindAndReleaseListener = new BindAndReleaseListener();

    /** Handle all async bind and release events.
     * When event is done, call setGroup(event),
     * which updates _group and releases _event.
     * <p>
     * <b>Note:</b> better if we process these <i>before</i>
     * invoking the application Listeners; 
     * thereby ensuring that the binding operations are complete.
     * <br>5/4/2001 change onDoneEvent to invoke these before app Listeners.
     * <p>
     * <b>Note:</b> we also monitor the non-trans <tt>onRetrieved()</tt>
     * and use that to mark the group unbound.
     */
    class BindAndReleaseListener 
	implements Async.BindAndReleaseListener,
		   Async.DelegationListener,
		   DelegationListener,
		   MediaServiceListener {
	public void onBindDone(MediaServiceEvent event) {
	    setGroup(event);	// set _group
	}
	public void onReleaseDone(MediaServiceEvent event) {
	    setGroup(event);	// clear _group
	}
	public void onDelegateToServiceDone(MediaServiceEvent event) {
	    setGroup(event);	// clear _devent (always)
	}
	public void onRetrieveDone(MediaServiceEvent event) {
	    setGroup(event);	// doneify _devent (if noException)
	}
	public void onDelegated(MediaServiceEvent event) {
	    setGroupDelegated(); // clear _event (it isDone)
	}
	public void onRetrieved(MediaServiceEvent event) {
	    setGroupRetrieved(); // clear _event, _devent, _group!
	}
	public void onReleaseDelegatedDone(MediaServiceEvent event) {
	    setGroup(event);	// clear _devent, _event, _group!
				// doneify _devent (if noException)
	}
	// proforma: implments Listener
	public void onConnected(MediaServiceEvent event) {}
	// proforma: implments Listener
	public void onDisconnected(MediaServiceEvent event) {}
    }
    
    /** If event is a BindAndReleaseEvent, invoke callback in _bindAndReleaseListener */
    public void onEventDone(Base.Event event) {
	// call setGroup in *this* thread, not on the Dispatcher thread:
	event.dispatch(_bindAndReleaseListener);
	// then queue to deliver to application Listeners
	super.onEventDone(event);
    }
    

    // For Retrieve done or ReleaseDelegated done
    // GP should:
    // _devent.setFields(group/null, ...); _devent.done(exc=null);
    // _event.setFields(group/null, ...); _event.done(exc?)
    // can done-ify in either order! [req'd because of async completion]

    /** 
     * Update _group and release _event.
     * Guarantee that _group is the latest and current binding.
     * Called from BindAndReleaseListener
     * <p>
     * <b>Implementors Note:</b>
     * This code is subtle, do not modify without a <i>complete</i>
     * understanding of the interactions.
     * This code is designed to work when <tt>_event</tt> and <tt>_devent</tt>
     * done-if in <tt>either</tt> order.
     * <p>
     * A MediaService is allowed to have one outstanding
     * bind/release request (<tt>_event</tt>).
     * Or one outstanding <tt>delegateToService</tt> request (the <tt>_devent</tt>)
     * and one of either retrieve/releaseDelegated.
     * These constraints are enforced by <tt>checkBindable</tt>, 
     * <tt>checkReleaseable</tt>, <tt>checkInDelegation</tt>.
     * 
     * @param event a Base.BindAndReleaseEvent with new value for _group 
     */
    synchronized
    void setGroup(MediaServiceEvent event) {
	Base.BindAndReleaseEvent baseEvent = (Base.BindAndReleaseEvent)event;

	if ((event == _event) || (event == _devent)) {
	    if (event == _devent) 
		_devent = null; // stop "isDelegated"
	    if (baseEvent.isNoException()) {
		// using *package* interface here:
		// the only place _group is set:
		_group = baseEvent.getMediaGroup(); 
	    }
	    if (event == _event)
		_event = _devent; // pop delegated or stop "BindInProgress"
	} else {
	    // ignore old/erroneous binding events
	    // ignore events that complete before _event is set!
	}
    }

    /** Send a ReleaseEvent(ev_Retrieved) to make this group unbound.
     * <p>
     * GroupProvider invokes this by dispatching a
     * DisconnectedEvent with <tt>(eventID==ev_Retrieved)</tt>.
     */
    synchronized
    void setGroupRetrieved() {
	// ignore any extant delegation request
	// ignore any extant bind/release 
	// as they will eventually return failed,
	// and so will not effect the binding status anyway.
	_devent = null;
	_event = new Base.ReleaseEvent(this, ev_Retrieved);
	// next: set _group = null; _event=_devent=null;
	setGroup(_event);
    }

    /** Clear BindInProgress indication.
     * Other checkRoutines will consider <tt>(_devent!=null)</tt> before
     * checking _event.
     */
    synchronized
    void setGroupDelegated() {
	_event = null;		// clear BindInProgress indication
    }

    /* Event and Listener stuff: inherited from Base_Owner */

    /**
     * Add a MediaListener to this MediaService.
     * <p>Implements deprecated MediaService method.
     * @deprecated
     */ 
    public void addMediaListener(MediaListener listener) {
	addListener(listener);	// eventQueue.addListener() in Base_Owner
    }

    /**
     * Remove a MediaListener from this MediaService.
     * <p>Implements deprecated MediaService method.
     * @deprecated
     */ 
    public void removeMediaListener(MediaListener listener) {
	removeListener(listener); // eventQueue.removeListener() in Base_Owner
    }

    /** Stop transactions, release MediaGroup, and stop EventQueue. 
     * <p>
     * <i>Should this be published?</i>
     * <i>Should this be removed?</i>
     */
    // package
    void reset(long msecs) {
        Async.Event ev = async_cancelBindRequest();
        if (ev != null) {
            try { ev.waitForEventDone(msecs); }
            catch (InterruptedException ex) {} // Oh well...
        }
	if (isBound()) {
	    try {
                async_stop().waitForEventDone(msecs); // ignore Exceptions
                async_release().waitForEventDone(msecs); // ignore Exceptions
	    }
	    catch (MediaBindException ex) {} // yes, we *are* bound
            catch (InterruptedException ex) {} // Oh well...
        }
        eventQueue.stop();      // stop callback delivery 
        eventQueue.flush();     // any undelivered Events
    }

    /** Restart the EventQueue. 
     * <p>
     * <i>Should this be published?</i>
     * <i>Should this be removed?</i>
     */
    // package
    void restart() { restart(true); }

    /** Restart the EventQueue. 
     * <p>
     * <i>Should this be published?</i>
     * <i>Should this be removed?</i>
     */
    // package
    void restart(boolean flush) {
	if (flush) eventQueue.flush();
	eventQueue.start();
    }

    /** Make this MediaService totally unusable.
     * Reset and release all resources, including the EventQueue and Provider.
     * <p>
     * <br><b>Note:</b> this method may be of no use
     * <br><b>Note:</b> this method may be dangerous (closes common eventQueue!)
     * <p>
     * <i>Should this be published?</i>
     * <i>Should this be removed?</i>
     * @throws MediaBindException if a group is bound
     */
    // package
    synchronized void close() throws MediaBindException  {
	checkBindable();	// close only valid if unbound!
	reset(300);		// main effect is eventQueue.stop()
	eventQueue.close();	// danger may be shared!
	eventQueue = null;	// drop reference, let it GC.
	_provider = null;
	_event = null;		// redundant (because is bindable)
	_group = null;		// redundant (because is bindable)
    }

    /** Associates this MediaService object with its MediaProvider.
     * Called as part of the Constructor.
     * @param provider a MediaProvider that implements MPI.GroupProvider.
     */
    synchronized
    protected void initMediaService(MediaProvider provider) {
	// addListener(_bindAndReleaseListener);
	// 5/4/2001 dispatch to _bindAndReleaseListener directly in onEventDone()

	// mark this group as unbound, no bind in progress:
	_group = null;
	_event = null;
	_provider = (MPI.GroupProvider)provider;
	// _provider = provider.getGroupProvider(); 
	// ASSERT: (provider instanceof MPI.GroupProvider)
    }
    
    /**
     * Get a MediaProvider from the given Strings.
     * <p>
     * If the given <tt>peerName</tt> is <tt>null</tt>: 
     * <ol><li>
     * Check <tt>System.getProperty("Jtapi.MediaPeer")</tt>.
     * If that is non-null, instantiate that MediaPeer.
     * </li><li>
     * Check <tt>System.getProperty("Jtapi.Peer", "DefaultJtapiPeer")</tt>,
     * See if that <tt>JtapiPeer.getProvider()</tt> produces a MediaPeer.
     * [this step may be elided if Jtapi integration is not expected]
     * </li><li>
     * Check <tt>"javax.telephony.media.provider.NullMediaPeer"</tt>.
     * That should produce <tt>NullMediaProvider</tt>.
     * </li></ol>
     * <p>
     * <b>Note:</b> This method can be used as a MediaProvider Factory.
     * For example: <br>
     * <tt>MediaProvider mp = BasicMediaService.findMediaProvider(...);</tt>
     * 
     * @param peerName name of the MediaPeer Class
     * @param providerString a String that identifies
     * the (MediaPeer-specific) MediaProvider and other information.
     * 
     * @throws ClassNotFoundException if MediaPeer class is not found
     * @throws IllegalAccessException if MediaPeer class is not accessible
     * @throws InstantiationException if MediaPeer class does not instantiate
     * @throws ProviderUnavailableException if Provider is not available
     * @throws ClassCastException if Provider instance is not a MediaProvider
     */ 
    public static
    MediaProvider findMediaProvider(String peerName, String providerString) 
    throws ClassNotFoundException, 
        InstantiationException,
        IllegalAccessException,
        ProviderUnavailableException {

	// First, look for a designated Jtapi.MediaPeer:
	if (peerName == null) {
	    // peerName = "javax.telephony.media.provider.NullMediaPeer";
	    peerName = System.getProperty("Jtapi.MediaPeer", peerName); 
	}
	// Ok, see if it MediaPeer will produce a MediaProvider:
	if (peerName != null) {
	    return ((MediaPeer)Class.forName(peerName).newInstance())
		.getMediaProvider(providerString);
	}

	/**************************************************/
	// Second, try the installation default JtapiPeer
	// to see if it can supply a Provider that is a MediaProvider
	if (peerName == null) { 
	    peerName = "DefaultJtapiPeer";
	    peerName = System.getProperty("Jtapi.Peer", peerName);
	}
	try {
	    Provider p = ((JtapiPeer)(Class.forName(peerName)).newInstance())
		.getProvider(providerString);
	    if (p instanceof MediaProvider)
		return ((MediaProvider)p);
	} catch (ClassNotFoundException exc) {
	    // this peer does work.
	    // ClassNotFoundException
	}
	// if class is found, the other Exceptions are thrown
	/**************************************************/

	// Finally, go back to NullMediaPeer:
	peerName = "javax.telephony.media.provider.NullMediaPeer";

	// See if this MediaPeer will produce a MediaProvider:
	return ((MediaPeer)Class.forName(peerName).newInstance())
	    .getMediaProvider(providerString);
    }

    /**
     * Return the MediaProvider used by this MediaService.
     * @return the MediaProvider used by this MediaService.
     */
    public MediaProvider getMediaProvider() {
	return _provider;
    }

    /**
     * Create an unbound MediaService, using services associated
     * with the supplied MediaProvider object.
     * The MediaProvider object may be a JTAPI core Provider,
     * or may be obtained by other vendor-specific means.
     *
     * @param provider a MediaProvider instance
     */
    public Base_MediaService(MediaProvider provider) {
	initMediaService(provider);
    }

    /**
     * Create an unbound MediaService, using services identified 
     * as the installation default.
     */
    public Base_MediaService() {
	// note: can't use super(null, null) or super(findMediaProvieer(null, null))
	// because we want to catch the Exception:
	try {
	    initMediaService(findMediaProvider(null, null));
	} catch (Exception ex) {
	    throw new RuntimeException("MediaProvider not found:" + ex);
	}
    }

    /**
     * Create an unbound MediaService, using services identified
     * by the two String arguments.
     *
     * @param peerName the name of a Class that implements MediaPeer.
     * @param providerString a "login" string for that MediaPeer.
     *
     * @throws ClassNotFoundException if MediaPeer class is not found
     * @throws IllegalAccessException if MediaPeer class is not accessible
     * @throws InstantiationException if MediaPeer class does not instantiate
     * @throws ProviderUnavailableException if Provider is not available
     * @throws ClassCastException if Provider instance is not a MediaProvider
     */
    public Base_MediaService(String peerName, String providerString) 
	throws ClassNotFoundException, 
	       InstantiationException, // hope to remove in Jtapi-2.0
	       IllegalAccessException,	// hope to remove in Jtapi-2.0
	       ProviderUnavailableException {
	initMediaService(findMediaProvider(peerName, providerString));
    }

    /** the MPI.GroupProvider methods. */

 
    /**
     * Revoke previous bind or release method on this MediaService.
     * <p>
     * <b>Note:</b>
     * This functions identically to async_cancelBindRequest,
     * but does not return the Event [it is part of the synchronous API].  
     */
    public void cancelBindRequest() {
	async_cancelBindRequest();
    }
    
    /** Asynchronous version of cancelBindRequest.
     * There is no listener method defined for this,
     * but applications that care to know when the event
     * has completed can start a thread to .waitForEventDone()
     * on the returned Event.
     * <p>
     * Applications that do <i>not</i> care about the completion
     * status may as well use cancelBindRequest.
     */
    public Async.Event async_cancelBindRequest() {
	Base.Event target;
	Base.Event event = new Base.Event( this, ESymbol.Session_Stop );
	synchronized (this) {		// to access _event, _group
	    if((_group != null) || (_event == null) || 
	       ((_event != null) && (_event.isDone()))) {
		event.setQualifier( ESymbol.Error_NoTranStopped );
		event.done(null);
		return event;
	    }
	    target = _event;
	}
	return _provider.cancelBindRequest(event, target);
    }

    /*********** Delegation methods ************/
    // Directly implemented methods, not generated:

    public DelegationEvent delegateToService(String serviceName, int timeout, String catchTags)
	throws MediaBindException,
	       MediaConfigException 
    {
	Async.DelegationEvent event;
	event = async_delegateToService(serviceName, timeout, catchTags);
	event.throwIfBindException();
	event.throwIfConfigException();
	return event;
    }

    synchronized
    public Async.DelegationEvent async_delegateToService(String serviceName, 
							 int timeout, 
							 String catchTags)
	throws MediaBindException 
    {
	checkReleaseable();	// throw NotBoundException, BindInProgress, NotOwnerException;
	Base.DelegationEvent event;
	// propose an event for this method:
	event = new Base.DelegationEvent(this, ev_DelegateToService);
	_event = event;		// setting _event marks "isBinding"
	_devent = event;	// setting _devent marks "isDelegated"

	// get the actual event from the provider:
	event = _provider.delegateToService(event, _group, serviceName, timeout, catchTags); 
	return event;
    }

    public void retrieve(Symbol cause)
	throws NotBoundException, MediaBindException, NotDelegatedException
    {
	Async.DelegationEvent event;
	event = async_retrieve(cause);
	event.throwIfBindException();
	return;
    }

    synchronized
    public Async.DelegationEvent async_retrieve(Symbol cause)
	throws NotBoundException, BindInProgressException, NotDelegatedException
    {
	checkInDelegation();	// throw NotBoundException, BindInProgress, NotDelegated;
	Base.DelegationEvent event;
	event = new Base.DelegationEvent(this, ev_Retrieve);
	//_event = event;		// setting _event marks isBinding
	//_revent = event;		// indicate retrieve in progress
	event = _provider.retrieve(event, _group, cause); 
	return event;
    }

    public void releaseDelegated()
	throws NotBoundException, MediaBindException, NotDelegatedException
    {
	Async.DelegationEvent event;
	event = async_releaseDelegated();
	event.throwIfBindException();
	return;
    }

    synchronized
    public Async.DelegationEvent async_releaseDelegated()
	throws NotBoundException, BindInProgressException, NotDelegatedException
    {
	checkInDelegation();	// throw NotBound, BindInProgress, NotDelegated;
	Base.DelegationEvent event;
	event = new Base.DelegationEvent(this, ev_ReleaseDelegated);
	// _event = event;		// setting _event marks isBinding
	//_revent = event;		// indicate releaseDelegated in progress
	event = _provider.releaseDelegated(event, _group); 
	return event;
    }

    /* Generate the Bind and Release Methods 
     * %?gen% %(BindAndReleaseMethods gen.intfs)
     *%/
%$methods<
    public %$retnType %$methName(%, %<%$parmType %$localvar%>)%<%Z<
	throws %>%,
	       %;%$excpType%> 
    {
	%$async_retnType event;
	event = %$async_methName(%, %$localvar);%<%;
	event.%$throwIfExceptions();%>%;
	return%$returnValue%;
    }

    synchronized
    public %$async_retnType %$async_methName(%, %<%$parmType %$localvar%>)%<%Z<
	throws %>%,
	       %;%$async_excpType%> 
    {
	%$checkBindableOrReleaseable%;
	%$Base_retnType event;
	event = new %$Base_retnType(this, %$EventID);
	_event = event;		// setting _event marks isBinding
	event = _provider.%$methName(event%$_group%,, %$localvar); 
	return event;
    }
%>%;// %?gen% 
     */

    public void bindAndConnect(ConfigSpec configspec0, String string1, String string2)
	throws MediaBindException,
	       MediaConfigException,
	       MediaCallException 
    {
	Async.BindToCallEvent event;
	event = async_bindAndConnect(configspec0, string1, string2);
	event.throwIfBindException();
	event.throwIfConfigException();
	event.throwIfCallException();
	return;
    }

    synchronized
    public Async.BindToCallEvent async_bindAndConnect(ConfigSpec configspec0, String string1, String string2)
	throws MediaBindException 
    {
	checkBindable();  // throw AlreadyBoundException
	Base.BindToCallEvent event;
	event = new Base.BindToCallEvent(this, ev_BindAndConnect);
	_event = event;		// setting _event marks isBinding
	event = _provider.bindAndConnect(event, configspec0, string1, string2); 
	return event;
    }

    public Connection bindToCall(ConfigSpec configspec0, Call call1)
	throws MediaBindException,
	       MediaConfigException,
	       MediaCallException 
    {
	Async.BindToCallEvent event;
	event = async_bindToCall(configspec0, call1);
	event.throwIfBindException();
	event.throwIfConfigException();
	event.throwIfCallException();
	return event.getConnection();
    }

    synchronized
    public Async.BindToCallEvent async_bindToCall(ConfigSpec configspec0, Call call1)
	throws MediaBindException 
    {
	checkBindable();  // throw AlreadyBoundException
	Base.BindToCallEvent event;
	event = new Base.BindToCallEvent(this, ev_BindToCall);
	_event = event;		// setting _event marks isBinding
	event = _provider.bindToCall(event, configspec0, call1); 
	return event;
    }

    public void bindToServiceName(ConfigSpec configspec0, String string1)
	throws MediaBindException,
	       MediaConfigException 
    {
	Async.BindEvent event;
	event = async_bindToServiceName(configspec0, string1);
	event.throwIfBindException();
	event.throwIfConfigException();
	return;
    }

    synchronized
    public Async.BindEvent async_bindToServiceName(ConfigSpec configspec0, String string1)
	throws MediaBindException 
    {
	checkBindable();  // throw AlreadyBoundException
	Base.BindEvent event;
	event = new Base.BindEvent(this, ev_BindToServiceName);
	_event = event;		// setting _event marks isBinding
	event = _provider.bindToServiceName(event, configspec0, string1); 
	return event;
    }

    public void bindToTerminalName(ConfigSpec configspec0, String string1)
	throws MediaBindException,
	       MediaConfigException 
    {
	Async.BindEvent event;
	event = async_bindToTerminalName(configspec0, string1);
	event.throwIfBindException();
	event.throwIfConfigException();
	return;
    }

    synchronized
    public Async.BindEvent async_bindToTerminalName(ConfigSpec configspec0, String string1)
	throws MediaBindException 
    {
	checkBindable();  // throw AlreadyBoundException
	Base.BindEvent event;
	event = new Base.BindEvent(this, ev_BindToTerminalName);
	_event = event;		// setting _event marks isBinding
	event = _provider.bindToTerminalName(event, configspec0, string1); 
	return event;
    }

    public void bindToTerminal(ConfigSpec configspec0, Terminal terminal1)
	throws MediaBindException,
	       MediaConfigException 
    {
	Async.BindEvent event;
	event = async_bindToTerminal(configspec0, terminal1);
	event.throwIfBindException();
	event.throwIfConfigException();
	return;
    }

    synchronized
    public Async.BindEvent async_bindToTerminal(ConfigSpec configspec0, Terminal terminal1)
	throws MediaBindException 
    {
	checkBindable();  // throw AlreadyBoundException
	Base.BindEvent event;
	event = new Base.BindEvent(this, ev_BindToTerminal);
	_event = event;		// setting _event marks isBinding
	event = _provider.bindToTerminal(event, configspec0, terminal1); 
	return event;
    }

    public void release()
	throws NotBoundException,
	       MediaBindException 
    {
	Async.ReleaseEvent event;
	event = async_release();
	event.throwIfBindException();
	return;
    }

    synchronized
    public Async.ReleaseEvent async_release()
	throws MediaBindException 
    {
	checkReleaseable(); // throw NotBoundException
	Base.ReleaseEvent event;
	event = new Base.ReleaseEvent(this, ev_Release);
	_event = event;		// setting _event marks isBinding
	event = _provider.release(event, _group); 
	return event;
    }

    public void releaseToService(String string0, int int1)
	throws NotBoundException,
	       MediaBindException,
	       MediaConfigException 
    {
	Async.ReleaseEvent event;
	event = async_releaseToService(string0, int1);
	event.throwIfBindException();
	event.throwIfConfigException();
	return;
    }

    synchronized
    public Async.ReleaseEvent async_releaseToService(String string0, int int1)
	throws MediaBindException 
    {
	checkReleaseable(); // throw NotBoundException
	Base.ReleaseEvent event;
	event = new Base.ReleaseEvent(this, ev_ReleaseToService);
	_event = event;		// setting _event marks isBinding
	event = _provider.releaseToService(event, _group, string0, int1); 
	return event;
    }

    public void releaseToTag(String string0)
	throws MediaBindException 
    {
	Async.ReleaseEvent event;
	event = async_releaseToTag(string0);
	event.throwIfBindException();
	return;
    }

    synchronized
    public Async.ReleaseEvent async_releaseToTag(String string0)
	throws NotBoundException,
	       MediaBindException 
    {
	checkReleaseable(); // throw NotBoundException
	Base.ReleaseEvent event;
	event = new Base.ReleaseEvent(this, ev_ReleaseToTag);
	_event = event;		// setting _event marks isBinding
	event = _provider.releaseToTag(event, _group, string0); 
	return event;
    }

    public void releaseToDestroy()
	throws MediaBindException 
    {
	Async.ReleaseEvent event;
	event = async_releaseToDestroy();
	event.throwIfBindException();
	return;
    }

    synchronized
    public Async.ReleaseEvent async_releaseToDestroy()
	throws MediaBindException 
    {
	checkReleaseable(); // throw NotBoundException
	Base.ReleaseEvent event;
	event = new Base.ReleaseEvent(this, ev_ReleaseToDestroy);
	_event = event;		// setting _event marks isBinding
	event = _provider.releaseToDestroy(event, _group); 
	return event;
    }
// %?gen%

    /* Now, the MPI.MediaGroup methods
     * %?gen% %(MediaGroupMethods gen.intfs)
     *%/
%$methods<
    public %$retnType %$methName(%, %<%$parmType %$localvar%>)%<%Z<
	throws %>%,
	       %;%$excpType%> 
    {
	%$async_retnType event = %$async_methName(%, %$localvar);
	%$event.throwIfException%;	// waitForListenersDone()
	%$return%;
    }

    public %$async_retnType %$async_methName(%, %<%$parmType %$localvar%>)
    {
	%$Base_retnType event = new %$Base_retnType(this, %$EventID);
	return checkGroup().%$methName(event%,, %$localvar);
    }

%>%;// %?gen% 
     */

    public void configure(ConfigSpec configspec0)
	throws NotBoundException,
	       MediaConfigException 
    {
	Async.MediaGroupEvent event = async_configure(configspec0);
	event.throwIfConfigException();	// waitForListenersDone()
	return;
    }

    public Async.MediaGroupEvent async_configure(ConfigSpec configspec0)
    {
	Base.MediaGroupEvent event = new Base.MediaGroupEvent(this, ev_Configure);
	return checkGroup().configure(event, configspec0);
    }


    public ConfigSpec getConfiguration()
	throws NotBoundException 
    {
	Async.MediaGroupEvent event = async_getConfiguration();
	event.throwIfRuntimeException();	// waitForListenersDone()
	return event.getConfigSpec();
    }

    public Async.MediaGroupEvent async_getConfiguration()
    {
	Base.MediaGroupEvent event = new Base.MediaGroupEvent(this, ev_GetConfiguration);
	return checkGroup().getConfiguration(event);
    }


    public String getTerminalName()
	throws NotBoundException 
    {
	Async.MediaGroupEvent event = async_getTerminalName();
	event.throwIfRuntimeException();	// waitForListenersDone()
	return event.getTerminalName();
    }

    public Async.MediaGroupEvent async_getTerminalName()
    {
	Base.MediaGroupEvent event = new Base.MediaGroupEvent(this, ev_GetTerminalName);
	return checkGroup().getTerminalName(event);
    }


    public Terminal getTerminal()
	throws NotBoundException 
    {
	Async.MediaGroupEvent event = async_getTerminal();
	event.throwIfRuntimeException();	// waitForListenersDone()
	return event.getTerminal();
    }

    public Async.MediaGroupEvent async_getTerminal()
    {
	Base.MediaGroupEvent event = new Base.MediaGroupEvent(this, ev_GetTerminal);
	return checkGroup().getTerminal(event);
    }


    public void triggerRTC(Symbol symbol0)
	throws NotBoundException 
    {
	Async.MediaGroupEvent event = async_triggerRTC(symbol0);
	/* until this is spec'd as a sync method */
	// event.throwIfRuntimeException();	// waitForListenersDone()
	return;
    }

    public Async.MediaGroupEvent async_triggerRTC(Symbol symbol0)
    {
	Base.MediaGroupEvent event = new Base.MediaGroupEvent(this, ev_TriggerRTC);
	return checkGroup().triggerRTC(event, symbol0);
    }


    public void stop()
	throws NotBoundException 
    {
	Async.MediaGroupEvent event = async_stop();
	event.throwIfRuntimeException();	// waitForListenersDone()
	return;
    }

    public Async.MediaGroupEvent async_stop()
    {
	Base.MediaGroupEvent event = new Base.MediaGroupEvent(this, ev_GroupStop);
	return checkGroup().stop(event);
    }


    public Dictionary getUserValues(Symbol[] symbol0)
	throws NotBoundException 
    {
	Async.MediaGroupEvent event = async_getUserValues(symbol0);
	event.throwIfRuntimeException();	// waitForListenersDone()
	return event.getDictionary();
    }

    public Async.MediaGroupEvent async_getUserValues(Symbol[] symbol0)
    {
	Base.MediaGroupEvent event = new Base.MediaGroupEvent(this, ev_GetUserValues);
	return checkGroup().getUserValues(event, symbol0);
    }


    public void setUserValues(Dictionary dictionary0)
	throws NotBoundException 
    {
	Async.MediaGroupEvent event = async_setUserValues(dictionary0);
	event.throwIfRuntimeException();	// waitForListenersDone()
	return;
    }

    public Async.MediaGroupEvent async_setUserValues(Dictionary dictionary0)
    {
	Base.MediaGroupEvent event = new Base.MediaGroupEvent(this, ev_SetUserValues);
	return checkGroup().setUserValues(event, dictionary0);
    }


    public void setUserDictionary(Dictionary dictionary0)
	throws NotBoundException 
    {
	Async.MediaGroupEvent event = async_setUserDictionary(dictionary0);
	event.throwIfRuntimeException();	// waitForListenersDone()
	return;
    }

    public Async.MediaGroupEvent async_setUserDictionary(Dictionary dictionary0)
    {
	Base.MediaGroupEvent event = new Base.MediaGroupEvent(this, ev_SetUserDictionary);
	return checkGroup().setUserDictionary(event, dictionary0);
    }


    public Dictionary getParameters(Symbol[] symbol0)
	throws NotBoundException 
    {
	Async.MediaGroupEvent event = async_getParameters(symbol0);
	event.throwIfRuntimeException();	// waitForListenersDone()
	return event.getDictionary();
    }

    public Async.MediaGroupEvent async_getParameters(Symbol[] symbol0)
    {
	Base.MediaGroupEvent event = new Base.MediaGroupEvent(this, ev_GetParameters);
	return checkGroup().getParameters(event, symbol0);
    }


    public void setParameters(Dictionary dictionary0)
	throws NotBoundException 
    {
	Async.MediaGroupEvent event = async_setParameters(dictionary0);
	event.throwIfRuntimeException();	// waitForListenersDone()
	return;
    }

    public Async.MediaGroupEvent async_setParameters(Dictionary dictionary0)
    {
	Base.MediaGroupEvent event = new Base.MediaGroupEvent(this, ev_SetParameters);
	return checkGroup().setParameters(event, dictionary0);
    }

// %?gen%

    /** 
     * Dispatch the given Request to the bound Group.
     * This is a general extension mechanism to send commands
     * to the bound group.
     * <p>
     * This is used by BasicMediaService to invoke Resource methods.
     * BasicMediaService can see this method, 
     * but it (nor any user defined extension classes) 
     * does not have direct access to checkGroup().
     * <p>
     * <b>Note:</b> any Exceptions from method.invoke() or 
     * from the requested method are wrapped into a RuntimeException
     * by request.invoke().  MediaResourceExceptions are cached
     * in the completion event and extracted by BasicMediaService
     * or the application's Listener code.
     * <p>
     * This corresponds to the S.100 function: CTgrp_execute(traninfo);
     * <p>
     * @param request a Request for a method invocation on this MediaGroup.
     * @return the Object returned from the method invocation,
     * typically a Base.Event: MediaEvent, ResourceEvent or other variant.
     *
     * @see Request.invoke(MPI.MediaGroup)
     */
    protected Object invokeGroupMethod(Request request)
	throws RuntimeException {
	return request.invoke(checkGroup());
    }
}
// %?% Generator done: %(println (new Date))% %?% Generator done: % %?%
