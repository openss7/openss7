/*
 * MPI.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/**
 * Media Provider Interfaces.
 * Defines the roles and interfaces in the S.400 architecture.
 * <ul><li>
 * The <tt>GroupProvider</tt> implements <i>Bind and Release</i> methods, 
 * including the Delegation versions of those methods.
 * The GroupProvider implements the detailed call control for
 * the Media Service framework; it handles inbound and outbound calls.
 * </li><li>
 * The <tt>MediaGroup</tt> implements the <i>media processing</i> methods.
 * This includes the configuration, parameters and user info
 * methods of the MediaService interface, and all the Resource methods.
 * The provider framework allows the MediaGroup to delegate
 * implementation of Resource methods to Resource-specific 
 * implementation objects.
 * </li><li>
 * The <tt>Resource</tt> interface represents the Resource-specific components
 * that implement the media Resource methods.
 * </li><li>
 * The <tt>Owner</tt> models the role of the MediaService object in the
 * provider framework. The Owner is the token of Group ownership <i>and</i>
 * is responsible for dispatching events to the application's Listeners.
 * Any object that is a <tt>source</tt> of a Dispatcher.DispatchableEvent
 * must implement <tt>Owner</tt> and <tt>onEventDone()</tt>.
 * </li></ul>
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
interface MPI 
{
    /**
     * The MPI definition of a GroupProvider. 
     * These methods are invoked in Base_MediaService.
     * <p>
     * The methods are of the form:
     * <br><code>Base.Event methodName(Base.Event, arglist);</code>
     * <p>
     * The semantics of these methods are defined by the corresponding
     * methods in <tt>MediaService</tt> and <tt>Async.MediaService</tt>.
     * <p>
     * The first (event) argument contains the MediaService source
     * and the correct eventID. Typically, the implementation
     * should use, return and done-ify the event as supplied.
     * <p>
     * The provider implementation <i>may</i> create and return its
     * own specialized version of the event, but there are restrictions:
     * <ol> <li>The new event must have the same source and eventID
     * </li><li>Only the <i>original</i> event can effect the binding state.
     * To complete the bind or release operation, the <i>original</i>
     * event must be done-ified with the new MediaGroup binding.
     * That is, the provider may return its own event for the applications use,
     * but the original event must be kept and used to drive the MPI framework.
     * When the operation is complete, <i>both</i> events must contain the 
     * correct information and <i>both</i> must be done-ified.
     * </li></ol>
     * <p>
     * <b>Note:</b> these methods return a <i>Base</i>.<tt>Event</tt> instead of
     * <i>Async</i>.<tt>Event</tt> because <tt>Base.Event</tt> includes 
     * a package accessor method to extract the MediaGroup from the event. 
     * [It is difficult to hide this accessor if it must be
     * a public interface with a public method.]
     * <p>
     * <b>Note:</b> The declaration of <tt>MPI.GroupProvider</tt> ensures
     * that a <tt>MPI.GroupProvider</tt> <i>isa</i> <tt>MediaProvider</tt>. 
     * However, the MPI framework (<tt>Base_MediaService</tt>) 
     * also assumes that the object that implements MediaProvider, 
     * and is used in
     * <br><tt>new BasicMediaService(MediaProvider)</tt>
     * <br>implements the more specialized <tt>MPI.GroupProvider</tt>.
     * This removes the necessity of declaring a public accessor
     * in <tt>javax.telephony.media.MediaProvider</tt>, such as:
     * <br><tt>MPI.GroupProvider getGroupProvider()</tt>
     * which would require a reference from the <tt>media</tt> package
     * to the <tt>provider</tt> package which we want to avoid.
     * <p>
     * If a MediaProvider implementation is outside the MPI framework,
     * then it may/must modify (or discard) <tt>Base_MediaService</tt> accordingly.
     */
    public interface GroupProvider extends MediaProvider {
	Base.BindToCallEvent bindAndConnect(Base.BindToCallEvent event, ConfigSpec cs, 
					    String s1, String s2);
	Base.BindToCallEvent bindToCall(Base.BindToCallEvent event, ConfigSpec cs, 
					Call call);
	Base.BindEvent bindToTerminalName(Base.BindEvent event, ConfigSpec cs, 
					  String s1);
	Base.BindEvent bindToTerminal(Base.BindEvent event, ConfigSpec cs, 
				      Terminal t1);

	Base.BindEvent bindToServiceName(Base.BindEvent event, ConfigSpec cs, 
					 String serviceName);

	Base.DelegationEvent delegateToService(Base.DelegationEvent event,
					       MPI.MediaGroup group, 
					       String serviceName, 
					       int timeout,
					       String catchTags);

	Base.DelegationEvent retrieve(Base.DelegationEvent event, MPI.MediaGroup group, 
				      Symbol cause);

	Base.DelegationEvent releaseDelegated(Base.DelegationEvent event, MPI.MediaGroup group);

	Base.ReleaseEvent release(Base.ReleaseEvent event, MPI.MediaGroup group);

	Base.ReleaseEvent releaseToTag(Base.ReleaseEvent event, MPI.MediaGroup group, 
					String returnTag);

	Base.ReleaseEvent releaseToDestroy(Base.ReleaseEvent event, MPI.MediaGroup group);

	Base.ReleaseEvent releaseToService(Base.ReleaseEvent event,
					   MPI.MediaGroup group,
					   String serviceName, 
					   int timeout);
	/**
	 * Cancel any ongoing Bind or Release.
	 * @param event a Base.Event which has no particular EventID or callback.
	 * but those who care can waitForEventDone().
	 * @param target a Base.BindEvent or Base.ReleaseEvent
	 * depending on which operation is being cancelled
	 */
	Base.Event cancelBindRequest(Base.Event event, Base.Event target);
    }

    /** A MediaService implementation (like Base_MediaService)
     * implements this. 
     * <tt>Base.Event</tt> uses this method to notify the Owner when the
     * event is done.
     * <p>
     * <b>Note:</b> <i>Owner</i> has two meanings:
     * Originally, a MediaService object 'owns' a MediaGroup,
     * and so is the 'Owner'.
     * <br>
     * Additionally, Events must have a 'source', 
     * (which is also a MediaService object),
     * and that 'source' must implement a Dispatcher to process the
     * event when the event is done-ified.
     * <br>
     * So 'Owner' also refers to being the 'source' or 'owner' of Events.
     * 
     * @see Base_Owner#onEventDone
     */
    public
    interface Owner {
	/** Inform Owner that the given event is "done".
	 * <P> 
	 * <b>Note:</b> In the provider package implementation, 
	 * this method is invoked inside <tt>Base.Event.done()</tt>.
	 * The Owner dispatches the event [eventually]
	 * by invoking <tt>Base.Event.dispatch()</tt> on the event.
	 * The <tt>Base_MediaService</tt> class in the provider package 
	 * does this using the EventQueue Dispatcher
	 * which casts the <tt>Base.Event</tt> to 
	 * its <tt>DispachableEvent</tt> interface
	 * which contains the <tt>dispatch()</tt> method.
	 * <p>
	 * @see Base.Event
	 */
	void onEventDone(Base.Event event);

	// MediaService getMediaService();
	// Base.MediaServiceEvent just does a cast; 
	// We promise that the source/owner is a MediaService
	// so MediaServiceEvent.getMediaService() is a casting wrapper.
    }

    /** What a Resource needs to fit in this framework. 
     * To generate a NonTransEvent, a MediaGroup or Resource
     * must know the 'Owner'; the MediaService that will dispatch the Event.
     */
    public
    interface Resource {
	/** This framework informs the Resource when the owner changes.
	 * So the Resource can create events with the correct Source.
	 */
	void setOwner(MPI.Owner owner);
    }

    /**
     * The MPI definition of a MediaGroup.
     * These methods are invoked in <tt>Base_MediaService</tt>.
     * <p>
     * Each of these methods has the same signature as
     * the corresponding method in <tt>Async.MediaService</tt> with
     * the addition of the first argument, 
     * an event with the proper <tt>source</tt> and <tt>eventID</tt>.
     * The return types are likewise restricted to be 
     * a <tt>Base.MediaGroupEvent</tt>. 
     * <p>
     * The event supplied as the first argument is typically returned
     * as is, but an implementor may create a specialized event
     * of the same class, source and same eventID if necessary.
     * <p>
     * The semantics of these methods are defined by 
     * the corresponding methods in MediaService.
     */
    public
    interface MediaGroup extends Resource {
	Base.MediaGroupEvent configure     	(Base.MediaGroupEvent event, ConfigSpec cs);
	Base.MediaGroupEvent getConfiguration	(Base.MediaGroupEvent event);
	Base.MediaGroupEvent getTerminalName	(Base.MediaGroupEvent event);
	Base.MediaGroupEvent getTerminal 	(Base.MediaGroupEvent event);

	Base.MediaGroupEvent getUserValues (Base.MediaGroupEvent event, Symbol[] keys);
	Base.MediaGroupEvent getParameters (Base.MediaGroupEvent event, Symbol[] keys);
	Base.MediaGroupEvent setUserDictionary	(Base.MediaGroupEvent event, Dictionary dict);
	Base.MediaGroupEvent setUserValues (Base.MediaGroupEvent event, Dictionary dict);
	Base.MediaGroupEvent setParameters (Base.MediaGroupEvent event, Dictionary dict);

	// a transaction, but no special accessors. 
	// DEPRECATED in the API!
	Base.MediaGroupEvent triggerRTC 	(Base.MediaGroupEvent event, Symbol rtca);
	Base.MediaGroupEvent stop 		(Base.MediaGroupEvent event);
	/** 
	 * This allows a level of indirection for those MediaGroup
	 * implementations that delegate Resource methods to aggregated objects.
	 * 
	 * @return an Object [Resource] that implements the given Method. 
	 */
	Object getTargetForMethod(java.lang.reflect.Method method);
    }
}
