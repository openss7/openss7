/*
 * NullMediaGroup.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
import java.util.Dictionary;

/** A MediaGroup implementation. 
 * Just <i>done-ify</i> the events, and return <tt>null</tt>.
 * <p>
 * A working MediaGroup should find or create real objects
 * and create Exceptions if necessary.
 * This class can be used as a source code "template",
 * one is not expected to use it as a base class.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public class NullMediaGroup implements MPI.MediaGroup, MediaServiceConstants {
    protected MPI.Owner owner = null;
    public void setOwner(MPI.Owner owner) {
	this.owner = owner;
    }

    /* invoked by NullGroupProvider for first configuration. */
    public void configure(ConfigSpec cs) throws Exception {}


    public Base.MediaGroupEvent configure(Base.MediaGroupEvent event, ConfigSpec cs)
    { 
	Exception ex=null;
	try { configure(cs); } 
	catch (Exception ex1) { ex = ex1; }
	event.done(ex);
	return event;
    }
    public Base.MediaGroupEvent getConfiguration(Base.MediaGroupEvent event)
    { 
	event.done(null);
	return event;
    }
    public Base.MediaGroupEvent getTerminalName(Base.MediaGroupEvent event)
    { 
	event.done(null);
	return event;
    }
    public Base.MediaGroupEvent getTerminal(Base.MediaGroupEvent event)
    { 
	event.done(null);
	return event;
    }
    public Base.MediaGroupEvent getUserValues(Base.MediaGroupEvent event, Symbol[] keys)
    { 
	event.done(null);
	return event;
    }
    public Base.MediaGroupEvent getParameters(Base.MediaGroupEvent event, Symbol[] keys)
    { 
	event.done(null);
	return event;
    }
    public Base.MediaGroupEvent setUserDictionary(Base.MediaGroupEvent event, Dictionary dict)
    { 
	event.done(null);
	return event;
    }
    public Base.MediaGroupEvent setUserValues(Base.MediaGroupEvent event, Dictionary dict)
    { 
	event.done(null);
	return event;
    }
    public Base.MediaGroupEvent setParameters(Base.MediaGroupEvent event, Dictionary dict)
    { 
	event.done(null);
	return event;
    }
    public Base.MediaGroupEvent triggerRTC(Base.MediaGroupEvent event, Symbol rtca)
    { 
	event.done(null);
	return event;
    }
    public Base.MediaGroupEvent stop(Base.MediaGroupEvent event)
    { 
	event.done(null);
	return event;
    }
    /** assume that this Group implements the media methods. */
    public Object getTargetForMethod(java.lang.reflect.Method method) 
    { return this; }
}
