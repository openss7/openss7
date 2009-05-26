package javax.csapi.cc.jcc;

/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 2001, 2002 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  Telcordia Technologies, Inc.
 *  Farooq Anjum, John-Luc Bakker, Ravi Jain
 *  445 South Street
 *  Morristown, NJ 07960
 *
 *  Version       : 1.1
 *  Notes         :
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/**
 A JccCall is a transient association of (zero or more) addresses for the 
 purposes of engaging in a real-time communications interchange. The call
 and its associated connection and address objects describe the control
 and media flows taking place in some underlying "real world" communication 
 network. Other parties involved in the call may also exert control over it,
 thus the membership and state of the endpoints may change without 
 explicit request by the Jcc application. The {@link JccProvider} adjusts the call, 
 address and connection objects to reflect the results of these 
 combined command actions. 
 * <H4>Introduction</H4>
 * A JccCall can have zero or more {@link JccConnection}s.
 * A two-party call has two JccConnections, and a conference call
 * has three or  more JccConnections. Each JccConnection models the relationship
 * between a JccCall and an JccAddress, where an JccAddress identifies a particular
 * party or set of parties on a call.
 *
 * <H4>Creating JccCall Objects</H4>
 *
 * Applications create instances of a JccCall object with the 
 * {@link JccProvider#createCall()} method, which returns a JccCall object that
 * has zero Connections and is in the {@link JccCall#IDLE} state.  The JccCall
 * maintains a  reference to its JccProvider for the life of that JccCall object.
 * The JccProvider object instance does not change throughout the lifetime of
 * the JccCall object. The JccProvider associated with a JccCall is obtained via the
 * {@link #getProvider()} method.
 *
 * <H4>JccCall States</H4>
 *
 * A JccCall has a <EM>state</EM> which is obtained via the
 * {@link #getState()} method. This state describes the current
 * progress of a telephone call, where is it in its life cycle, and how many
 * connections exist on the call. The JccCall state may be one of three values:
 * {@link #IDLE}, {@link #ACTIVE}, or
 * {@link #INVALID}. The following is a description of each state:
 *
 * <TABLE>
 * <TR>
 * <TD WIDTH="20%">{@link #IDLE}</TD>
 * <TD WIDTH="80%">
 * This is the initial state for all calls. In this state, the JccCall has zero
 * connections, that is {@link #getConnections()} <EM>must</EM>
 * return null.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%">{@link #ACTIVE}</TD>
 * <TD WIDTH="80%">
 * A call with some current ongoing activity is in this state. JccCalls with one
 * or more associated JccConnections must be in this state. If a JccCall is in this
 * state, the <CODE>getConnections()</CODE> method must return an array
 * of size at least one.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%">{@link #INVALID}</TD>
 * <TD WIDTH="80%">
 * This is the final state for all calls. JccCall objects which lose all of their
 * JccConnection objects (via a transition of the JccConnection object into the
 * {@link JccConnection#DISCONNECTED} state) moves into this state. Calls
 * in this state have zero JccConnections and these JccCall objects may not be used
 * for any future action. In this state, the
 * <CODE>getConnections()</CODE>
 * <EM>must</EM> return null.
 * </TD>
 * </TR>
 * </TABLE>
 *
 * <H4>JccCall State Transitions</H4>
 *
 * Consider the transitions given below as the ones that would be visible to the application 
 * developer in the package considered. This may be either because there is a method in that 
 * class/interface that provides that behavior (read transition) or because of events in the 
 * underlying platform. Note that we are not making any statements to the effect that the other 
 * transitions (ones not shown) are disallowed. The application developer would have to keep 
 * in mind the possibility that the "invisible" transitions can surface (become visible) in 
 * inherited classes/interfaces.
 *
 * <CENTER>
 * <H1>JccCall FSM</H1>
 * <IMG SRC="doc-files/callstates.gif">
 * </CENTER>
 * <H4>JccCall and JccConnection objects</H4>
 *
 * A JccCall maintains a list of the JccConnections on that JccCall. Applications obtain
 * an array of JccConnections associated with the JccCall via the
 * <CODE>getConnections()</CODE> method. A JccCall retains a reference to a
 * JccConnection only if it is not in the {@link JccConnection#DISCONNECTED}
 * state. Therefore, if a JccCall has a reference to a JccConnection, then that
 * JccConnection must not be in the <CODE>JccConnection.DISCONNECTED</CODE> state.
 * When a JccConnection moves into the <CODE>JccConnection.DISCONNECTED</CODE> state
 * (e.g. when a party hangs up), the JccCall loses its reference to that
 * JccConnection which is no longer reported via the
 * <CODE>JccCall.getConnections()</CODE> method.
 *

 * 
 * <H4>Listeners and Events</H4>
 *
 * The {@link JccCallListener} or {@link JccConnectionListener} interface reports all events pertaining to
 * the JccCall object. Events delivered to this interface must implement or extend the
 * {@link JccCallEvent} interface. Applications can add listeners to a JccCall
 * object
 * via the {@link #addCallListener(JccCallListener)} method.
 * <p>
 * Connection-related events can be reported
 * via the 
 <CODE>JccConnectionListener</CODE> interface. 
  These events include the
 * creation of these objects and their state changes. Events which are
 * reported
 * via the <CODE>JccConnectionListener</CODE> interface pertaining to JccConnections or JccCalls
 *  extend the {@link JccConnectionEvent} interface.
 *
 
       <H5><A NAME="event_snapshots">Event Snapshots</A></H5> 
       By default, when an listener is added 
       to a telephone call, the first batch of events may be a "snapshot".
       That is, if the listener was added after state changes in the Call,
       the first batch of events will inform the application of the 
       current state of the Call. Note that these snapshot events do NOT 
       provide a history of all events on the Call, rather they provide 
       the minimum necessary information to bring the application 
       up-to-date with the current state of the Call. 
 *
 
 * <H5>When Event Transmission Ends</H5>
 *
 * At times it may become impossible for the implementation to
 * report events to an application.  In this case,
 * a {@link JccCallEvent#CALL_EVENT_TRANSMISSION_ENDED} is delivered to
 * an object registered as a <CODE>JccCallListener</CODE>
 * (or an extension of that interface).
 * This is the final event received by the Listener from the call.

     
     @since 1.0

*/
public interface JccCall {
    /**
       Add a connection listener to all connections under this call. 
       This method applies to any JccConnection 
       that already exists when the method is called as well as any JccConnections 
       that will be added in the future to this JccCall object.
       This method
       behaves similar to the addCallListener(conListener, filter) method 
       on this interface where conListener is in fact a JccConnectionListener. 
       When conListener is not a JccConnectionListener but is only a JccCallListener, 
       a similar behavior as addCallListener(conListener, newfilter)
       can be obtained with this method using a JccConnectionListener and a 
       different EventFilter which filters
       all the JccConnection related events in addition to the events being filtered
       by newfilter. 
       Note though that using this method only 
       JccConnectionListeners can be added. 
       <p>

       Registering a single listener twice will result in the
       last filter being used for the purposes of consultation to 
       determine the events that the application is interested in.
       Note that registering for the same event multiple times should not
       result in multiple notifications being sent to an application for
       the same event.

       <P><B>Post-Conditions:</B>
       <OL>
        <LI>A snapshot of events is delivered to the listener, if appropriate. 
       </OL>

       @param cl JccConnectionListener object that receives the specified events.
       @param filter EventFilter determines if the event is to be 
       delivered to the specified listener. 
        @throws MethodNotSupportedException The listener cannot be added at this time.
        @throws ResourceUnavailableException The resource limit for the number of 
        listeners has been exceeded. 
       <br>
       */
     
    public void addConnectionListener( JccConnectionListener cl, EventFilter filter )     throws ResourceUnavailableException, 
    MethodNotSupportedException; 

    /**
		Removes the connection listener from all connections under this call. Note that 
		if the listener is currently not registered then this method fails silently. 
		
		@param cl JccConnectionListener object that was registered using a corresponding
		addConnectionListener method. 
    */

    public void removeConnectionListener( JccConnectionListener cl);

    /**
        This method requests the release of the call object and associated connection
        objects. Thus this method is equivalent to using the {@link JccConnection#release(int)}
        method on each JccConnection which is part of the Call. Typically each JccConnection 
        associated with this call will move into the {@link JccConnection#DISCONNECTED} state. 
        The call will also be terminated in the network. If the application
        has registered as a listener then it receives the {@link JccCallEvent#CALL_EVENT_TRANSMISSION_ENDED}
        event.  <p>
        Valid cause codes (prefixed by <code>CAUSE_</code>) for the integer that is 
        named causeCode are defined in {@link JccEvent} and {@link JccCallEvent}.

       <p> <B>Pre-conditions:</B> <OL>
        <LI>(this.getProvider()).getState() == IN_SERVICE <br>
        <LI> this.getState() == ACTIVE
        </OL>
       <p> <B>Post-conditions:</B> <OL>
        <LI>(this.getProvider()).getState() == IN_SERVICE <br>
        <LI> this.getState() == INVALID
        <LI>CALL_EVENT_TRANSMISSION_ENDED event delivered to the 
        valid Calllisteners. 
        <LI>Appropriate ConnectionEvents are also delivered to the ConnectionListeners. 
        </OL>

       @param causeCode an integer that represents a cause code.  Valid values 
       are defined in {@link JccEvent} and {@link JccCallEvent}, they are typically prefixed 
       by <code>CAUSE_</code>.

        @throws PrivilegeViolationException The application does not have 
        the authority or permission to disconnect the Call. For example, 
         an  Address associated with this Call may not be controllable 
        in the Provider's domain. 
        @throws ResourceUnavailableException An internal resource required 
        to drop a connection is not available. 
        @throws InvalidStateException Some object required for the 
        successful invocation of this method is not in the proper state as 
        given by this method's pre-conditions. 
		@throws InvalidArgumentException The given release cause code is invalid. 
		@since 1.0a
    */
    public void release(int causeCode) throws PrivilegeViolationException, 
    ResourceUnavailableException, InvalidStateException, InvalidArgumentException;

    /**
    The application calls this method to supervise a call. The application can set a 
    granted connection time for this call. If an application calls this function before
    it calls a {@link #routeCall(String,String,String,String)}, the timer measurement 
    will start as soon as the call is answered by the called party.  A call supervision 
    event is also sent if the call is terminated before the supervision event occurs.
    <p>
    The call supervision events are then reported to the listener registered using this method.
    The events related to call supervision are reported only to the listeners registered using this
    method. 
    <p>
    <em> Note </em> that a policy object may be designed to define the policy to be 
    implemented by the platform as a result of this method instead of defining 
    the policy through the given parameters. This might be designed in the 
    future specifications. 

    
    @param calllistener JccCallListener object that receives the specified events.
    @param time specifies the granted time in milliseconds for the connection. When specified
    as 0, volume based supervision is applied. Either bytes(volume) or time should be specified.
    @param treatment defines the treatment of the call by the call control service when 
    the call supervision timer expires. The values which may be combined using a logical
    OR function are:
    <OL>
    <LI><code>0x01</code> to release the call when the call supervision timer expires.<br> 
    <LI><code>0x02</code> to notify the application when the call supervision timer expires. <br>
    <LI><code>0x04</code> to send a warning tone to the controlling party when a call supervision timer
    expires. If call release is requested, then the call will be released following the 
    tone after an administered time period. 
    </OL>
    
    @throws MethodNotSupportedException if the implementation does not support this
    method. 
    @since 1.0b
    */
    public void superviseCall(JccCallListener calllistener,double time,int treatment) throws 
    MethodNotSupportedException;

	/**
		Places a call from an originating address to a destination address string. 
		
		<p>The Call must be in the {@link JccCall#IDLE} state (and therefore have no 
		existing associated JccConnections and the Provider must be in the 
		{@link JccProvider#IN_SERVICE} state. The successful effect of this method 
		is to place the call and create and return two JccConnections associated with 
		this Call. 
		
		<h5>Method Arguments</h5>
		
		This method has two arguments. The first argument is the originating Address 
		for the Call. The second argument is a destination string whose value represents 
		the address to which the call is placed. This destination address must be valid and 
		complete. 
		
		<h5>Method Post-conditions</h5>
		
		This method returns successfully when the Provider can successfully initiate the 
		placing of the call. As a result, when the JccCall.connect() method returns, the JccCall 
		will be in the {@link JccCall#ACTIVE} state and exactly two JccConnections will be 
		created and returned. The JccConnection associated with the originating endpoint is 
		the first JccConnection in the returned array.  This JccConnection will execute the 
		originating JccConnection's Final State Diagram (see 
		<a href="package-summary.html#TOConnections">table 3</a>). The JccConnection associated 
		with the destination endpoint is the second JccConnection in the returned array and 
		will execute the terminating JccConnection's Final State Diagram. These 
		two JccConnections must at least be in the {@link JccConnection#IDLE} state. That is, if one of 
		the Connections progresses beyond the IDLE state while this method is completing, this
		Connection may be in a state other than the IDLE. This state must be reflected by an 
		event sent to the application. 
		
		<p><B>Pre-Conditions:</B> <OL>
		<LI>(this.getProvider()).getState() == JccProvider.IN_SERVICE 
		<LI>this.getState() == JccCall.IDLE 
		</OL>
		
		<B>Post-Conditions:</B> <OL>
		<LI>(this.getProvider()).getState() == JccProvider.IN_SERVICE 
		<LI>this.getState() == JccCall.ACTIVE 
		<LI>Let Connection c[] = this.getConnections() 
		<LI>c.length == 2 
		<LI>c[0].getState() == JccConnection.IDLE (at least) 
		<LI>c[1].getState() == JccConnection.IDLE (at least) 
		</OL>
		
		@param origaddr The originating Address for this call.
		@param dialedDigits The destination address string for this call.
		
		@return array of Connections
		
		@throws ResourceUnavailableException An internal resource necessary for placing 
		the call is unavailable.
		@throws PrivilegeViolationException The application does not have the proper 
		authority to place a call.
		@throws InvalidPartyException Either the originator or the destination does not 
		represent a valid party required to place a call.
		@throws InvalidStateException Some object required by this method is not in a valid 
		state as designated by the pre-conditions for this method.
		@throws MethodNotSupportedException The implementation does not support this method.
		@since 1.0a
	*/
	public JccConnection[] connect(JccAddress origaddr, String dialedDigits) throws 
	ResourceUnavailableException, PrivilegeViolationException, InvalidPartyException, 
	InvalidStateException, MethodNotSupportedException;

	/**
		Creates a new JccConnection and attaches it to this 
		JccCall. The JccConnection object is associated with an JccAddress 
		object corresponding to the string given as an input parameter. Note that 
		following this operation the returned JccConnection object must 
		still be routed to the JccAddress which can be accomplished using the 
		{@link JccConnection#routeConnection(boolean)}. 
		
		<h5><A NAME="createConnectionSemantics">Examples</a></h5>
		<UL>
		<LI></code>createConnection("B","A",null,null)</code>
		will result in  third party setup of a Connection.
		A reference to the  created connection i.e "B"'s connection will be returned.
		A getAddress() method invocation on the returned JccConnection reference object
		results in the JccAddress corresponding to party "B" being returned.
		Party "B" will have to be routed to in this case.	    
		<LI><code>createConnection("C","A","B","B")</code> will be applied in case the number of party "B" is translated into
		the number of party "C", while party "A" dialed party "B". <code>createConnection("C","A","B",null)</code> results in the 
		same behavior.   Note that the connection with address "B" is automatically moved to
		{@link JccConnection#DISCONNECTED DISCONNECTED} with cause code 
		{@link JccCallEvent#CAUSE_REDIRECTED CAUSE_REDIRECTED} after the connection to "C" is routed.
		A reference to the  created connection i.e "C"'s connection will be returned.
		A getAddress() method invocation on the returned JccConnection reference object
		results in the JccAddress corresponding to party "C" being returned.
		Party "C" will have to be routed to in this case.	    
		<LI><code>createConnection("C","A","B","D")</code> will be applied in case the number of party "D" is translated into
		the number of party "C", while party "A" dialed party "B".
		A reference to the  created connection i.e "C"'s connection will be returned.
		A getAddress() method invocation on the returned JccConnection reference object
		results in the JccAddress corresponding to party "C" being returned.
		Party "C" will have to be routed to in this case. Note that the connection with address "B" is automatically moved to
		{@link JccConnection#DISCONNECTED DISCONNECTED} with cause code 
		{@link JccCallEvent#CAUSE_REDIRECTED CAUSE_REDIRECTED} after the connection to "C" is routed.
		
		</UL>
		<p> <B>Pre-conditions:</B> <OL>
		<LI>(this.getProvider()).getState() == IN_SERVICE
		<LI> this.getState() != INVALID
		</OL>
		<p> <B>Post-conditions:</B> <OL>
		<LI> let conn = createConnection(..);
		<LI> conn.getState() == IDLE
		<LI> this.getState() == ACTIVE
		<LI>(this.getProvider()).getState() == IN_SERVICE
		</OL>
		
		@return the created JccConnection object
		
		@param targetAddress specifies the address to which the call 
		should be routed.
		@param originatingAddress specifies the address of the originating (calling)
		party for this leg of the call. This is optional and can be set to null. 
		@param originalCalledAddress specifies the initial destination address to which
		this leg of the call was initiated. This is optional and can be set to null.
		@param redirectingAddress specifies the last address from which this leg of the call
		was redirected. This is optional and can be set to null. 
		
		@throws InvalidArgumentException An InvalidArgumentException 
		indicates an argument passed to the method is invalid.
		@throws InvalidStateException Some object required by this method is not 
		in a valid state as designated by the pre-conditions for this method.
		@throws ResourceUnavailableException An internal resource necessary
		for creating the Connection object is unavailable. 
		@throws PrivilegeViolationException The application does not have 
		the proper authority to create the Connection.
		@throws MethodNotSupportedException The implementation does not 
		support this method
		@throws InvalidPartyException In case one of the parties given is invalid.
	*/
	public JccConnection createConnection(String targetAddress, String originatingAddress, 
	String originalCalledAddress, String redirectingAddress)         throws InvalidStateException, 
	ResourceUnavailableException, PrivilegeViolationException, 
	MethodNotSupportedException, InvalidArgumentException, InvalidPartyException;  
	
	/**
		This method requests routing of a call to the targetAddress given as an input parameter.

		Creates a new JccConnection and attaches it to this 
		JccCall. The JccConnection object is associated with an JccAddress 
		object corresponding to the string given as an input parameter.
		Note that the 
		addresses are passed as strings. The implementation is expected to find the JccAddress 
		object corresponding to the string assuming that the JccAddress is local to the JccProvider.
		The given string may not correspond to any JccAddress object in the JccProvider's 
		domain which would be the case for a call to a remote Address.     
		This method
		is equivalent to the {@link #createConnection(String,String,String,String)}, 
		<a href="JccConnection.html#routeConnection(boolean)">JccConnection.routeConnection(FALSE)</A>
		and {@link JccConnection#attachMedia()} or is also
		equivalent to {@link #createConnection(String,String,String,String)} and 
		<a href="JccConnection.html#routeConnection(boolean)">JccConnection.routeConnection(TRUE)</A>. 
		
		<h5><A NAME="routeCallSemantics">Examples</a></h5>
		<UL>
		<LI><code>routeCall("B","A",null,null)</code>
		will result in  third party setup of a Connection.
		A reference to the  created connection i.e "B"'s connection will be returned.
		A getAddress() method invocation on the returned JccConnection reference object
		results in the JccAddress corresponding to party "B" being returned.
		
		<LI><code>routeCall("C","A","B","B")</code> will be applied in case the 
		number of party "B" is translated into the number of party "C", while party "A" 
		dialed party "B". <code>routeCall("C","A",null,"B")</code> and <code>routeCall("C","A","B",null)</code> 
		result in the same behavior.  Note that in all three cases the connection with address "B" is 
		automatically moved to
		{@link JccConnection#DISCONNECTED DISCONNECTED} with cause code 
		{@link JccCallEvent#CAUSE_REDIRECTED CAUSE_REDIRECTED}.
		A reference to the  created connection i.e "C"'s connection will be returned.
		A getAddress() method invocation on the returned JccConnection reference object
		results in the JccAddress corresponding to party "C" being returned.
		
		<LI><code>routeCall("D","A","B","C")</code> will be applied in case the number 
		of party "C" is translated into the number of party "D", while party "A" dialed party B.
		A reference to the  created connection, i.e. "D"'s connection, will be returned.
		A getAddress() method invocation on the returned JccConnection reference object
		results in the JccAddress corresponding to party "D" being returned.  
		Note that the connection with address "C" is automatically moved to
		{@link JccConnection#DISCONNECTED DISCONNECTED} with cause code 
		{@link JccCallEvent#CAUSE_REDIRECTED CAUSE_REDIRECTED}.
		
		</UL>
		<p> <B>Pre-conditions:</B> <OL>
		<LI>(this.getProvider()).getState() == IN_SERVICE
		<LI> this.getState() != INVALID
		</OL>
		<p> <B>Post-conditions:</B> <OL>
		<LI> let conn = routeCall(..);
		<LI>conn.getState() == IDLE (at least)
		<LI>this.getState() == ACTIVE
		<LI>(this.getProvider()).getState() == IN_SERVICE
		</OL>
		
		@param targetAddress specifies the address to which the call 
		should be routed.
		@param originatingAddress specifies the address of the originating (calling)
		party for this leg of the call. This is optional and can be set to null.
		@param originalCalledAddress specifies the initial destination address to which
		this leg of the call was initiated. This is optional and can be set to null.
		@param redirectingAddress specifies the last address from which this leg of the call
		was redirected. This is optional and can be set to null. 
		
		@throws InvalidArgumentException An InvalidArgumentException 
		indicates an argument passed to the method is invalid.
		@throws InvalidStateException Some object required by this method is not 
		in a valid state as designated by the pre-conditions for this method.
		@throws ResourceUnavailableException An internal resource necessary
		for creating the Connection object is unavailable. 
		@throws PrivilegeViolationException The application does not have 
		the proper authority to create the Connection.
		@throws MethodNotSupportedException The implementation does not 
		support this method
		@throws InvalidPartyException In case one of the parties given is invalid. 
		@return the created JccConnection object
	*/
	public JccConnection routeCall(String targetAddress, String originatingAddress, 
	String originalDestinationAddress, String redirectingAddress) throws InvalidStateException, 
	ResourceUnavailableException, PrivilegeViolationException,MethodNotSupportedException, 
	InvalidPartyException, InvalidArgumentException;  
	
	//--------------------------------------------------------------------------------
	//FROM JCP
	
    /**
        Retrieves the state of the call. The state will be either {@link #IDLE}, 
        {@link #ACTIVE} or {@link #INVALID}. 
        @return Integer representing the state of the call.
     */
    public int getState();

    /**
		Add a listener to this call. This also reports all state changes in the state of 
		the Call and Connection objects. The listener added with this method will 
		report events on the call for as long as the implementation can listen to 
		the Call. In the case that the call becomes unobservable
		the listener receives a {@link JccCallEvent#CALL_EVENT_TRANSMISSION_ENDED} event.
		 
		
		<H5>CallListeners from Provider</H5> 
		There may be additional call listeners on the call which were not
		added by this method. These listeners may have become part of the 
		call via the {@link javax.csapi.cc.jcc.JccProvider#addCallListener(JccCallListener)} 
		or similar methods.
		
		<H5>ConnectionListeners</H5> 
		Since JccConnectionListener inherits from the JccCallListener, it is also 
		possible to add a JccConnectionListener using this method. In such a case, 
		connection events  would also have to be reported to the
		registered listener in addition to the call events. 
		Hence, it is expected that the JCC implementation uses instanceof checks 
		in order to decide if only call events or both call and connection events 
		have to be delivered to the listener. Note that a listener added by this 
		method is expecting all the events without any filtering involved. 
		
		<H5>Multiple Invocations</H5> 
		If an application attempts to add an instance of a listener 
		already present on this Call, there are two possible outcomes:
		<OL>
		<LI>If the listener was added by the application using this 
		method, then a repeated invocation will silently fail, i.e. 
		multiple instances of an listener are not added and no exception
		will be thrown. 
		<LI>If the listener is part of the call because 
		an application invoked <CODE>JccProvider.addCallListener()</CODE>
		either of these methods 
		modifies the behavior of that listener as if it were added via 
		this method instead. 
		</OL>
		
		<P><B>Post-Conditions:</B>
		<OL>
		<LI>A snapshot of events is delivered to the listener, if appropriate. 
		</OL>
		
		@param calllistener JccCallListener object that receives the specified events.
		@throws MethodNotSupportedException The listener cannot be added at this time.
		@throws ResourceUnavailableException The resource limit for the number of 
		listeners has been exceeded. 
     */

    public void addCallListener( JccCallListener calllistener)     throws ResourceUnavailableException, 
    MethodNotSupportedException;


    /**
        Removes a listener from this call. If the listener is not part of the Call 
        for the given address(es), then this method fails silently, i.e. no listener is 
        removed and no exception is thrown.  Note that the listeners will stop 
        receiving events for existing calls also. 

		<p>This method has different effects depending upon how the 
		listener was added to the Call, as follows:
		<OL> <LI>If the listener was added via 
		{@link JccCall#addCallListener(JccCallListener)}, this method removes the 
		listener until it is re-applied by the application. 
		<LI>If the listener was added via 
		{@link javax.csapi.cc.jcc.JccProvider#addCallListener(JccCallListener)} 
		or simular methods, this method removes
		the listener for this call only. It does not affect whether this 
		listener will be added to future calls coming in to the JccProvider.
		</OL>

        @param calllistener JccCall Listener object.
     */
    public void removeCallListener( JccCallListener calllistener) ;


    /**
        Retrieves the provider handling this call object. The Provider reference does not
        change once the Call object has been created, despite the state of the 
        Call object. 
        
        @return JccProvider object managing this call.
    */
    public JccProvider getProvider();

    /**
        Retrieves an array of  connections asssociated with this call. None of the 
        Connections returned will be in the {@link JccConnection#DISCONNECTED} state. Further, 
        if the Call is in the {@link #IDLE} or {@link #INVALID} state, 
        this method returns null.
        
        <p><B>Post-Conditions:</B> <OL> 
         <LI>JccConnection[] conn = getConnections()<br>
         <LI>if this.getState() == IDLE then conn=null <br>
         <LI>if this.getState() == INVALID then conn=null <br>
         <LI>if this.getState() == ACTIVE then conn.length >=1 <br>
         <LI>For all i, conn[i].getState() != DISCONNECTED<br>
        </OL>

        @return Array of Connections for this call.
    */
    public JccConnection[] getConnections();

    /**
        JccCall.IDLE state indicates the Call has zero Connections. This is the initial 
        state of all Call objects.
    */
    public static final int IDLE      = 1;

    /**
        JccCall.ACTIVE state indicates the Call has one or more Connections
        none of which is in the {@link JccConnection#DISCONNECTED} state.  The 
        Call object transitions into this state from the {@link #IDLE} state only.  
    */
    public static final int ACTIVE    = 2;


    /**
        The JccCall.INVALID state indicates that the Call has lost all of its connections, 
        that is, all of its Connection objects have moved into the {@link JccConnection#DISCONNECTED}
        state and are no longer associated with the Call. 
         A Call in this state cannot
        be used for future actions.  
    */
    public static final int INVALID  = 3;

}
 
