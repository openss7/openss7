/*
 * Base_MediaConnectionEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import java.util.EventListener;
import javax.telephony.media.*;
import javax.telephony.media.async.*;
import javax.telephony.media.connection.*;
import javax.telephony.media.connection.async.*;

/** 
 * Minimal or reference implementation of MediaConnectionEvent. 
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
class Base_MediaConnectionEvent extends Base.MediaGroupEvent 
    implements Async_MediaConnectionEvent {
    
    /** this constructor for a non-Trans version of MediaConnectionEvent.
     * <p><b>Note:</b>
     * Surely, we could use a subclass, but for now,
     * we just set a flag.
     */
    public Base_MediaConnectionEvent(Object source, Symbol eventID, 
				     boolean isNonTrans) {
	super(source, eventID);
	this.isNonTrans = isNonTrans;
    }

    public Base_MediaConnectionEvent(Object source, Symbol eventID) {
	super(source, eventID);
    }
    /** Do not use, specify the real eventID; from the Token.type. */
    public Base_MediaConnectionEvent(Object source) {
	super(source, ev_Disconnect);
    }
    // originally dispatched to Async.MediaGroupListener.onMediaGroupDone().
    /** dispatch to MediaConnectionListener.
     * either onConnection(event) or onDisconnection(event)
     * <p>
     * the eventID is set to the more specific connection type.
     */
    public void dispatch( EventListener listener ) {
	// connection events multiplex trans and non-trans
	// we need to sort them out to their Listeners
	if (isNonTrans) {
	    if (!( listener instanceof MediaConnectionListener )) return;
	    if ( eventID.equals( ESymbol.Group_Disconnect )) {
		((MediaConnectionListener)listener).onDisconnect(this);
	    } else if ( eventID.equals( ESymbol.Group_SetDataFlow )) {
		((MediaConnectionListener)listener).onSetDataFlow(this);
	    } else {		// is a connection event
		eventID = token.getType(); // use more specific eventID
		((MediaConnectionListener)listener).onConnect(this);
	    }
	} else {
	    if (!( listener instanceof Async_MediaConnectionListener )) return;
	    if ( eventID.equals( ESymbol.Group_Disconnect )) {
		((Async_MediaConnectionListener)listener).onDisconnectDone(this);
	    } else if ( eventID.equals( ESymbol.Group_SetDataFlow )) {
		((Async_MediaConnectionListener)listener).onSetDataFlowDone(this);
	    } else {		// is a connection event
		eventID = token.getType(); // use more specific eventID
		((Async_MediaConnectionListener)listener).onConnectDone(this);
	    }
	}
    }

    protected void setFields() {
	type 	    = (Symbol)payload.get( ESymbol.Group_ConnectionType );
	maxDataFlow = (Symbol)payload.get( ESymbol.Group_MaxDataFlow );
	dataFlow    = (eventID.equals( ESymbol.Group_SetDataFlow )) ?
	    (Symbol)payload.get( ESymbol.Group_NewDataFlow ) :
	    (Symbol)payload.get( ESymbol.Group_InitialDataFlow );
    }

    protected Symbol type = null;
    protected Symbol maxDataFlow = null;
    protected Symbol dataFlow = null;

    public Symbol getType() 		{ return type; }
    public Symbol getMaxDataFlow() 	{ return maxDataFlow; }
    public Symbol getDataFlow() 	{ return dataFlow; }


    /** contains all the useful info about a connection. */
    protected MediaConnection.Token token = null;

    /** sub-classes may override to make specialized token. */
    public MediaConnection.Token getToken() { 
	waitForEventDone(); 
	if (token == null) token = new Token( type, maxDataFlow, dataFlow );
	return token;
    }


    public void done(Exception exception, MediaConnection.Token token) {
	this.token = token;
	super.done(exception);
    }
    public void throwIfConnectionException() throws MediaConnectionException {
	throwIfRuntimeException();
	if (exception instanceof MediaConnectionException) {
	    exception.fillInStackTrace();
	    throw (MediaConnectionException)exception;
	}
    }

    
    /** A minimal structure to implement a MediaConnection.Token.
     * <p>
     * <b>Note:</b>
     * A provider implementer can extend this with fields 
     * that indicate which objects are actually connected.
     * Also provide a suitable .equals(Object) method.
     * two Tokens are equal if the refer to the same media
     * path connection: same source and sink of media.
     */
    public static
    class Token implements MediaConnection.Token {
	public String toString() {
	    return getClass().getName()+"{type="+ type+" dataFlow="+dataFlow+"}";
	}
	
	protected Token() {}
	
	protected Token( Symbol type, Symbol maxDataFlow, Symbol dataFlow ) {
	    this.type = type;
	    this.maxDataFlow = maxDataFlow;
	    this.dataFlow = dataFlow;
	}
	
	protected Symbol type = null;
	protected Symbol dataFlow = null;
	protected Symbol maxDataFlow = null;
	
	/** Indicates data flow of this connection.
	 * @return one of <tt>v_Listen</tt>, <tt>v_Talk</tt>, 
	 * <tt>v_Both</tt>, <tt>v_None</tt> 
	 */
	public Symbol getDataFlow() {
	    return( dataFlow );
	}

	/** @return one of <tt>v_Listen</tt>, <tt>v_Talk</tt>, <tt>v_Both</tt> */
	public Symbol getMaxDataFlow() {
	    return( maxDataFlow );
	}

	/** Indicate the type of connection.
	 * <p>
	 * <b>Note:</b> For disconnect events, Bridge and Loopback 
	 * may be concealed as Secondary and Primary, respectively.
	 * @return one of <tt>v_Primary</tt>, <tt>v_Secondary</tt>,
	 * <tt>v_Bridge</tt>, <tt>v_Loopback</tt> 
	 */
	public Symbol getType() {
	    return( type );
	}
    }
}
