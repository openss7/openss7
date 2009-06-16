/*
 * NullGroupProvider.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
import javax.telephony.media.container.*;

/** A GroupProvider implementation. 
 * This defines all the methods, but just <i>done-ifies</i>
 * all the events with <tt>null</tt> values, without taking any action.
 * <p>
 * A working GroupProvider should find or create real objects
 * and create Exceptions if necessary.
 * This class can be used as a source code "template",
 * one is not expected to use it as a base class.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
class NullGroupProvider extends Base_Owner implements MPI.GroupProvider
{
    /** implements MediaProvider */
    public ContainerService getContainerService(){
	return null;
    }

    /** implements MediaProvider */
    public MediaService getMediaService() {
	return new BasicMediaService(this);
    }

    protected MPI.MediaGroup newMediaGroup() {
	return new NullMediaGroup();
    }

    /** overly simple, need to treat each bind specially. */
    protected void bindMediaGroup(Base.BindEvent event, ConfigSpec cs) {
	Async.MediaService ms;
	MPI.MediaGroup mg = null;
	Exception ex = null;
	try {
	    ms = (Async.MediaService)event.getMediaService();
	    mg = newMediaGroup();
	    ((NullMediaGroup)mg).configure(cs);
	    mg.setOwner((MPI.Owner)ms);
	} catch (Exception ex1) {
	    ex = ex1;
	}
	event.done(ex, mg);	// BindToCallEvent: (ex, mg, (Connection)null)
    }

    /** implements MPI.GroupProvider */
    public Base.BindToCallEvent bindAndConnect(Base.BindToCallEvent event, 
					       ConfigSpec cs,
					       String s1, String s2)
    { 
	bindMediaGroup(event, cs);
	return event;
    }
    public Base.BindToCallEvent bindToCall(Base.BindToCallEvent event, 
					   ConfigSpec cs, 
					   Call call)
    { 
	bindMediaGroup(event, cs);
	return event;
    }
    public Base.BindEvent bindToTerminalName(Base.BindEvent event, 
					     ConfigSpec cs, 
					     String s1)
    { 
	bindMediaGroup(event, cs);
	return event;
    }
    public Base.BindEvent bindToTerminal(Base.BindEvent event,
					 ConfigSpec cs, 
					 Terminal t1)
    { 
	bindMediaGroup(event, cs);
	return event;
    }
    public Base.BindEvent bindToServiceName(Base.BindEvent event,
					    ConfigSpec cs,
					    String serviceName)
    { 
	bindMediaGroup(event, cs);
	return event;
    }
    public Base.DelegationEvent delegateToService(Base.DelegationEvent event,
						  MPI.MediaGroup group, 
						  String serviceName, 
						  int timeout,
						  String catchTags) 
    {
	// newMS = find MediaService(serviceName), 
	// stack current owner and tags, etc...
	// group.setOwner(newMS);
	event.done(null);
	return event;	
    }
    public Base.DelegationEvent retrieve(Base.DelegationEvent event,
					 MPI.MediaGroup group,
					 Symbol cause) 
    {
	// notify previous owners: onRetrieved()
	group.setOwner((MPI.Owner)event.getMediaService());
	event.done(null);
	return event;	
    }
    public Base.DelegationEvent releaseDelegated(Base.DelegationEvent event,
						 MPI.MediaGroup group) 
    {
	// remove event.getMediaService from owner stack of group
	event.done(null);
	return event;	
    }
    public Base.ReleaseEvent releaseToService(Base.ReleaseEvent event, 
					      MPI.MediaGroup group,
					      String serviceName, 
					      int timeout)
    { 
	// newMS = find MediaService(serviceName)
	// group.setOwner(newMS);
	event.done(null);
	return event;
    }
    public Base.ReleaseEvent release(Base.ReleaseEvent event,
				     MPI.MediaGroup group) {
	// newMS = previous owner
	// group.setOwner(newMS);
	event.done(null);
	return event;	
    }
    public Base.ReleaseEvent releaseToTag(Base.ReleaseEvent event,
					  MPI.MediaGroup group,
					  String returnTags)
    { 
	// newMS = previous owner (of tags)
	// group.setOwner(newMS);
	event.done(null);
	return event;
    }
    public Base.ReleaseEvent releaseToDestroy(Base.ReleaseEvent event,
					      MPI.MediaGroup group)
    { 
	// newMS = previous owner (of tag destroy)
	// group.setOwner(newMS);
	event.done(null);
	return event;
    }
    /**
     * Cancel any ongoing Bind or Release.
     * <p><b>Note:</b>
     * A real implementation should undertake to stop any transactions
     * that are in progress in association with that event.
     * 
     * @param source the MediaService "source" of the return Event.
     * @param target a Base.BindEvent or Base.ReleaseEvent
     * depending on which operation is being cancelled
     *
     * @return a <i>done</i> Async.Event 
     * which has no specified EventID or callback.
     */
    public Base.Event cancelBindRequest(Base.Event event, Base.Event target )
    { 
	target.done(new BindCancelledException("cancelled by application"));
	event.done(null);
	return event;
    }
} 
