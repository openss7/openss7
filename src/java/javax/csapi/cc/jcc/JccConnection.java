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
The purpose of a JccConnection object is to describe
the relationship between a {@link JccCall} object and a {@link JccAddress} object.
A JccConnection object exists if the JccAddress is a part of the telephone call.
Each JccConnection has a <EM>state</EM> which describes the particular stage
of the relationship between the JccCall and JccAddress. These states and their
meanings are described below. Applications use the
{@link #getCall()} and {@link #getAddress()}
methods to obtain the JccCall and JccAddress associated with this JccConnection,
respectively. <p>

From one perspective, an application may view a JccCall
only in terms of the JccAddress/JccConnection objects which are part of the
JccCall. This is termed a <EM>logical</EM> view of the Call.  In this
logical view, a telephone
call is viewed as two or more endpoint addresses in communication. The
JccConnection object describes the state of each of these endpoint addresses
with respect to the JccCall.

<H4>JccCalls and JccAddresses</H4>
JccConnection objects
are immutable in terms of their JccCall and JccAddress references. In other
words, the JccCall and JccAddress object references do not change throughout
the lifetime of the JccConnection object instance. The same JccConnection
object may not be used in another telephone call. The existence of a
JccConnection implies that its JccAddress is associated with its JccCall in the
manner described by the JccConnection's state. <p>

Although a JccConnection's
JccAddress and JccCall references remain valid throughout the lifetime of the
JccConnection object, the same is not true for the JccCall and JccAddress object's
references to this JccConnection.
Particularly, when a  JccConnection moves into
the {@link #DISCONNECTED} state, it is no longer listed by the
{@link JccCall#getConnections()} method.
Typically, when a JccConnection moves into the
<CODE>DISCONNECTED</CODE> state, the application loses its
references to it to facilitate its garbage collection.

 <H4>Connection States</H4>
 Below is a description of each JccConnection state in
 real-world terms. These real-world descriptions have no bearing on the
 specifications of methods, they only serve to provide a more intuitive
 understanding of what is going on. Several methods in this specification
 state pre-conditions based upon the state of the Connection.<p>

 <TABLE CELLPADDING=2> <TR> <TD WIDTH="15%">
 {@link #IDLE}</TD> <TD WIDTH="85%">
 This state is the initial state for all new Connections. Connections
 which are in the <CODE>IDLE</CODE> state are not actively
 part of a telephone call, yet their references to the Call and Address
 objects are valid. Connections typically do not stay in the
 <CODE>IDLE</CODE> state for long, quickly transitioning to
 other states. </TD> </TR> <TR> <TD WIDTH="15%">

 {@link #DISCONNECTED}</TD> <TD WIDTH="85%"> This state
 implies it is no longer part of the telephone call, although its
 references to Call and Address still remain valid. A Connection in this
 state is interpreted as once previously belonging to this telephone call.
 </TD> </TR> <TR> <TD WIDTH="15%">


 <TR> <TD WIDTH="15%">{@link #AUTHORIZE_CALL_ATTEMPT}</TD>
 <TD WIDTH="85%">
 This state implies that the originating or terminating terminal needs to be
        authorized for the call.
 </TD> </TR> <TR> <TD WIDTH="15%">

 <TR> <TD WIDTH="15%">{@link #ADDRESS_COLLECT}</TD>
 <TD WIDTH="85%">  This state implies that more information needs to be
        collected for the call.
 </TD> </TR> <TR> <TD WIDTH="15%">

 <TR> <TD WIDTH="15%">{@link #ADDRESS_ANALYZE}</TD>
 <TD WIDTH="85%">
 This state implies that the address needs to be
        analyzed to determine the resources needed.
 </TD> </TR> <TR> <TD WIDTH="15%">

 <TR> <TD WIDTH="15%">{@link #CALL_DELIVERY}</TD>
 <TD WIDTH="85%">
 This state implies that the resources (such as network links) needed to
 deliver signaling messages are being used.
 </TD> </TR> <TR> <TD WIDTH="15%">


 <TR> <TD WIDTH="15%">{@link #ALERTING}</TD>
 <TD WIDTH="85%"> This state implies that the Address is being notified
 of an incoming call. </TD> </TR> <TR> <TD WIDTH="15%">

 {@link #CONNECTED}</TD> <TD WIDTH="85%"> This state
 implies that a Connection and its Address is actively part of a
 telephone call. In common terms, two people talking to one another are
 represented by two Connections in the <CODE>CONNECTED</CODE>
 state. </TD> </TR> <TR> <TD WIDTH="15%">

   {@link #FAILED}</TD> <TD WIDTH="85%"> This state indicates that a Connection to that
 end of the call has failed for some reason. One reason why a Connection
 would be in the <CODE>FAILED</CODE> state is because the
 party was busy. Connections that are in the <code>FAILED</code> state are still connected to the call.
 </TD> </TR> </TABLE>


 <H4>Connection State Transitions</H4>
 *
 * Consider the transitions given below as the ones that would be visible to the application
 * developer in the package considered. This may be either because there is a method in that
 * class/interface that provides that behavior (read transition) or because of events in the
 * underlying platform. Note that we are not making any statements to the effect that the other
 * transitions (ones not shown) are disallowed. The application developer would have to keep
 * in mind the possibility that the "invisible" transitions can surface (become visible) in
 * inherited classes/interfaces.
 <P>

 <CENTER>
 <H1>JccConnection FSM</H1>
 <IMG SRC="doc-files/connection.gif" ALIGN="center">
 </CENTER>


 <P>



 <H4>Events--Blocking and non-Blocking</H4>

 An event is generated corresponding to every state change in the finite state machine.
 (FSM). All events pertaining to the JccConnection object are reported via the
{@link JccCallListener} interface on the JccCall object associated with
this JccConnection.
<p>
 All the events on the JccConnection are expected to be blockable. In other words, after
 sending each event to the listener the implementation can either suspend processing or continue with
 processing.
 The implementation suspends processing if the event is to be fired in a
 blocking mode and the implementation continues with processing if the event is to be
 fired in a non-blocking mode.
 When the implementation suspends processing, only the traversal of the
 finite state machine by the corresponding JccConnection object is suspended. All
 external events received for the blocked JccConnection object would have to be
 queued and handled. Thus, the finite state machine state of the
 corresponding JccConnection object does not change when processing is suspended.
<p>
 In case of a blocking event, the implementation is expected to suspend processing either
 until the application uses a valid API method call or until a timeout occurs
 (<a href="package-summary.html#methodInvocation">table 2</a> contains a column that lists the methods
 that implicitly resume processing). An example of a valid API method that implicitly
 resumes processing is {@link #release(int)}. Typically, the
 "informational" methods like {@link #isBlocked()}, {@link JccConnection#getAddress() getAddress()},
 etc. do not cause the resumption of call processing if it had been suspended.
 <p>
 The listeners are expected to specify the mode in which they are to be notified of the
 events. Note that the events are sent out only when the state is reached. Hence, when
 processing is suspended the connection is in some state. The order of event delivery,
 whether all the notifications to be done before the blocking events are reported to
 the listener etc, is undefined.

 <H5> EventFilters </H5>
 Listeners can be added with
 and without the use of EventFilters.
  Listeners added without filters are
 expected to receive events in the notification mode. Listeners added with
 filters will receive only those events which are not "discarded" by the EventFilter.


 <H5> Multiple Listeners </H5>
 Multiple listeners can also be added for the same event. In case
 there are multiple listeners registered for  an event and all of them request
 just the notification of an event without suspension of call processing the
 JCC implementation informs all the registered listeners.
 On the other hand if some listeners have registered for event notification and
 the other listeners have requested blocking, the platform informs all listeners
 about the event while keeping the call processing suspended(without changing any state). In such a case, if one of the
 registered listeners invokes a valid API method, then this specification offers no
 restrictions on  call processing resumption/suspension. In other words in such a case, the
 result is highly dependent on the platform.

   @since 1.0
 */

public interface JccConnection {

 /**
 Replaces address information onto an existing JccConnection. The address replaced
 on a connection is what is normally thought of as the destination address. Note
 that the address (and hence JccAddress) associated with this JccConnection itself
 is not changed.
 This method is used when a telephone address string has been dialed and
 address translation is needed in order to  place the telephone call.
 The translated addressing information is provided as the
 argument to this method.

        <p>
       <B>Pre-Conditions:</B> <OL>
        <LI>this.getState() == ADDRESS_COLLECT or ADDRESS_ANALYZE<br>
        <LI>this.getCall().getState() == ACTIVE<br>
        </OL>
       <B>Post-Conditions:</B> <OL>
        <LI>this.getState() == CALL_DELIVERY <br>
        <LI>this.getCall().getState() == ACTIVE<br>
        </OL>

        @throws MethodNotSupportedException The implementation does not support this method.
        @throws InvalidStateException Some object required by this method is not
        in a valid state as designated by the pre-conditions for this method.
        @throws ResourceUnavailableException An internal resource for completing this
        call is unavailable.
        @throws PrivilegeViolationException The application does not have the
        proper authority to call this method.
        @throws InvalidPartyException The given Addresses are not valid.

        @param address indicates the String representation of the  translated addresses.

 */

    public void selectRoute(String address) throws MethodNotSupportedException, InvalidStateException,
    ResourceUnavailableException, PrivilegeViolationException, InvalidPartyException;



    /**
        Drops a JccConnection from an active telephone call. If
        successful, the associated
        JccAddress will be released from the call and the JccConnection moves
        to the
        {@link JccConnection#DISCONNECTED} state following which it may be deleted.
        Valid cause codes (prefixed by <code>CAUSE_</code>) for the integer that is
        named causeCode are defined in {@link JccEvent} and {@link JccCallEvent}.

        The JccConnection's {@link JccAddress} is no longer associated with the
        telephone call. This method does not necessarily drop the entire
        telephone call, only the particular JccConnection on the telephone
        call. This method provides the ability to disconnect a specific
        party from a telephone call, which is especially useful in
        telephone calls consisting of three or more parties. Invoking
        this method may result in the entire telephone call being dropped,
        which is a permitted outcome of this method. In that case, the
        appropriate events are delivered to the application, indicating
        that more than just a single JccConnection has been dropped from the
        telephone call.

          As a result of this method returning successfully,
          a {@link JccConnectionEvent#CONNECTION_DISCONNECTED} event for
          this JccConnection is delivered to the registered listeners.
        <br>
         <H5>Dropping Additional Connections</H5>
         Additional JccConnections may be dropped indirectly as a result of
         this method. For example, dropping the destination JccConnection of
         a two-party call may result in the entire telephone call being
         dropped. It is up to the implementation to determine which
         JccConnections are dropped as a result of this method.
         Implementations should not, however, drop additional JccConnections
         representing additional parties
         if it does not reflect the natural response of the underlying
         telephone hardware.

         <p> <B>Pre-conditions:</B>
         <OL>
        <LI>this.getState() != IDLE or DISCONNECTED<br>
        <LI>((this.getCall()).getProvider()).getState() == IN_SERVICE <br>
        <LI> (this.getCall()).getState() == ACTIVE <br>
        </OL>
         <p> <B>Post-conditions:</B> <OL>
        <LI>this.getState() == DISCONNECTED<br>
        <LI>((this.getCall()).getProvider()).getState() == IN_SERVICE <br>
        <LI> CONNECTION_DISCONNECTED event is delivered for to the registered
        listeners.
        <LI> CALL_INVALID event is also delivered if all the JccConnections are dropped indirectly as
        a result of this method.
        </OL>

       @param causeCode an integer that represents a cause code.  Valid values
       are defined in {@link JccEvent} and {@link JccCallEvent}, they are typically prefixed
       by <code>CAUSE_</code>.

       @throws InvalidStateException If either of the JccConnection, {@link JccCall} or {@link JccProvider}
       objects is not in the proper states as given by this method's precondition.
       @throws PrivilegeViolationException  The application does not have the authority or
       permission to disconnect the JccConnection. For example, the JccAddress associated
       with this JccConnection may not be controllable in the JccProvider's domain.
       @throws ResourceUnavailableException An internal resource to drop the connection is
       not available.
       @throws InvalidArgumentException The given release cause code is invalid.
       @since 1.0a

    */
    public void release(int causeCode) throws PrivilegeViolationException,
    ResourceUnavailableException, InvalidStateException, InvalidArgumentException;

    /**
    This method causes the call to be answered.  <p>This method can only be invoked on a terminating connection object.

        <p> <B>Pre-conditions:</B> <OL>
        <LI>getAddress() != getOriginatingAddress()
        <LI>this.getState() == CALL_DELIVERY or ALERTING
        <LI>((this.getCall()).getProvider()).getState() == IN_SERVICE
        <LI> (this.getCall()).getState() == ACTIVE
        </OL>
         <p> <B>Post-conditions:</B> <OL>
        <LI>this.getState() == CONNECTED
        <LI>((this.getCall()).getProvider()).getState() == IN_SERVICE
        <LI> CONNECTION_CONNECTED event is delivered for to the registered
        listeners.
        <LI> (this.getCall()).getState() == ACTIVE <br>
        </OL>

        @throws MethodNotSupportedException The implementation does not support this method.
	   @throws PrivilegeViolationException This could include trying to answer an outgoing call leg
	   in a case where it is leaving the domain of the local call control platform.
       @throws ResourceUnavailableException An internal resource to answer the connection is
       not available.
       @throws InvalidStateException If either of the JccConnection, {@link JccCall} or {@link JccProvider}
       objects is not in the proper states as given by this method's precondition.
    */
    public void answer() throws PrivilegeViolationException,ResourceUnavailableException,InvalidStateException,MethodNotSupportedException;

    /**
	    This method requests the platform to continue processing. The call processing has
	    been suspended due to the firing of a blocking event (trigger) and this method causes
	    the processing to continue.

        <p> <B>Pre-conditions:</B> <OL>
		 <LI>this.isBlocked() == true<br>
		 <LI>((this.getCall()).getProvider()).getState() == IN_SERVICE <br>
		 <LI> (this.getCall()).getState() == ACTIVE <br>
        </OL>
        <p> <B>Post-conditions:</B> <OL>
		 <LI>((this.getCall()).getProvider()).getState() == IN_SERVICE <br>
		 <LI> (this.getCall()).getState() == ACTIVE <br>
        </OL>

        @throws PrivilegeViolationException  The application does not have the authority or
        permission to invoke this methods.
		@throws ResourceUnavailableException An internal resource is not available.
		@throws InvalidStateException If either of the JccConnection, {@link JccCall} or {@link JccProvider}
		objects is not in the proper states as given by this method's precondition.
    */

    public void continueProcessing() throws PrivilegeViolationException,ResourceUnavailableException,InvalidStateException;

    /**
    This method will allow transmission on all associated bearer connections or media
    channels to and from other parties in the call. The JccConnection object must be in
    the {@link JccConnection#CONNECTED} state for this method to complete successfully.

        <p> <B>Pre-conditions:</B> <OL>
		 <LI>this.getState() == CONNECTED<br>
		 <LI>((this.getCall()).getProvider()).getState() == IN_SERVICE <br>
		 <LI> (this.getCall()).getState() == ACTIVE <br>
        </OL>
        <p> <B>Post-conditions:</B> <OL>
		 <LI>this.getState() == CONNECTED
		 <LI>((this.getCall()).getProvider()).getState() == IN_SERVICE <br>
		 <LI> (this.getCall()).getState() == ACTIVE <br>
        </OL>

        @throws PrivilegeViolationException  The application does not have the authority or
        permission to invoke this methods.
		@throws ResourceUnavailableException An internal resource is not available.
		@throws InvalidStateException If either of the JccConnection, {@link JccCall} or {@link JccProvider}
		objects is not in the proper states as given by this method's precondition.
    */
    public void attachMedia() throws PrivilegeViolationException,ResourceUnavailableException,InvalidStateException;

    /**
    This method will detach the JccConnection from the call, i.e., this will prevent
    transmission on any associated bearer connections or media channels to and from
    other parties in the call. The JccConnection object must be in the {@link JccConnection#CONNECTED} state
    for this method to complete successfully.

        <p> <B>Pre-conditions:</B> <OL>
		 <LI>this.getState() == CONNECTED<br>
		 <LI>((this.getCall()).getProvider()).getState() == IN_SERVICE <br>
		 <LI> (this.getCall()).getState() == ACTIVE <br>
        </OL>
        <p> <B>Post-conditions:</B> <OL>
		 <LI>this.getState() == CONNECTED
		 <LI>((this.getCall()).getProvider()).getState() == IN_SERVICE <br>
		 <LI> (this.getCall()).getState() == ACTIVE <br>
        </OL>

        @throws PrivilegeViolationException  The application does not have the authority or
        permission to invoke this methods.
		@throws ResourceUnavailableException An internal resource is not available.
		@throws InvalidStateException If either of the JccConnection, {@link JccCall} or {@link JccProvider}
		objects is not in the proper states as given by this method's precondition.
    */
    public void detachMedia() throws PrivilegeViolationException,ResourceUnavailableException,InvalidStateException;

    /**
    Returns a boolean value indicating if the JccConnection is currently blocked due to
    a blocking event having been fired to a listener registered for that blocking event.
    The method returns false once a valid API call is made after the firing of a
    blocking event or until after the expiry of a timeout.
    @return boolean indicating if the connection is blocked due to a blocking event.
    */
    public boolean isBlocked();

    /**
    Returns the last redirected address associated with this JccConnection. The last redirected
    address is the address at which the current
     JccCall was placed immediately before the current address. This is common if a JccCall is
     forwarded to several addresses before
     being answered. If the last redirected address is unknown or not yet known, this method
     returns null.
    @return the address to which the call was last associated and redirection on
    which caused the current Address to be associated with the call through the connection.
    */
    public String getLastAddress();


    /**
    Returns the original address associated with this JccConnection. This would be the
    first address to which the call was placed. The current address  might be
    different from this due to multiple forwardings. If this address is unknown or
    not yet known, this method returns null.
    @return the address which was called initially.
    */

    public String getOriginalAddress();

    /**
    Returns the address of the destination party.
	This method will return the address of the destination party when invoked upon an
	originating connection and only if the connections state ({@link #getState()}) is
	either in {@link #ADDRESS_COLLECT}, {@link #ADDRESS_ANALYZE}, or {@link #CALL_DELIVERY}.
	It will return null otherwise.
    @return the address of the destination party.
    */
    public String getDestinationAddress();

    /**
    Returns the address of the originating party.
	This method will return the address of the originating party when invoked upon a
	terminating connection. If there is no originating party the method will return null.
	If the method is invoked on an originating connection object, the method's return value
	will equal the return value of {@link JccConnection#getAddress()}.
    @return the address of the originating party (if any).
    */
    public JccAddress getOriginatingAddress();

    /**
		Returns the redirected address.
		Only after the event with id. {@link JccConnectionEvent#CONNECTION_DISCONNECTED
		CONNECTION_DISCONNECTED} with cause code {@link JccCallEvent#CAUSE_REDIRECTED
		CAUSE_REDIRECTED} occured and the connection returned by
		{@link JccConnectionEvent#getConnection()} is <code>this</code> and it is a
		terminating connection, this method will return the address of the party to
		which this connection is redirected.  In all other cases this method returns
		<code>null</code>.
		@return the address to which the call is redirected or <code>null</code> if
		the call is not redirected.
		@since 1.0a
    */
    public String getRedirectedAddress();

    /**
		Routes this JccConnection to the target address associated with this
		JccConnection object.
		
		<p><B>Pre-Conditions:</B> <OL>
		<LI>this.getState() == IDLE or AUTHORIZE_CALL_ATTEMPT
		<LI>this.getCall().getState() == ACTIVE
		</OL>
		<B>Post-Conditions:</B> <OL>
		<LI>this.getState() == AUTHORIZE_CALL_ATTEMPT (at least)
		<LI>this.getCall().getState() == ACTIVE
		</OL>
		
		@throws InvalidStateException Some object required by this method is not
		in a valid state as designated by the pre-conditions for this method.
		@throws ResourceUnavailableException An internal resource for completing this
		call is unavailable.
		@throws PrivilegeViolationException The application does not have the
		proper authority to call this method.
		@throws MethodNotSupportedException The implementation does not support this method.
		@throws InvalidPartyException The given Addresses are not valid.
		@throws InvalidArgumentException The provided argument is not valid.
		@param attachmedia indicates if the media has to be attached after the connection
		is routed. TRUE causes the media to be attached, FALSE causes the media not
		to be attached in which case a separate call to {@link JccConnection#attachMedia()} must be made in
		order to attach the media to this connection.
    */
    public void routeConnection(boolean attachmedia )      throws InvalidStateException,
    ResourceUnavailableException, PrivilegeViolationException,
    MethodNotSupportedException, InvalidPartyException, InvalidArgumentException;

    /**
    	This methods gives access to service code type and service code value.  It can
    	be invoked after the event with id {@link JccConnectionEvent#CONNECTION_MID_CALL} occured.

		<p><B>Pre-Conditions:</B> <OL>
		<LI>this.getState() == CONNECTED
		<LI>this.getCall().getState() == ACTIVE
		</OL>
		<B>Post-Conditions:</B> <OL>
		<LI>this.getState() == CONNECTED
		<LI>this.getCall().getState() == ACTIVE
		</OL>
    	
		@throws InvalidStateException Some object required by this method is not
		in a valid state as designated by the pre-conditions for this method.
		@throws ResourceUnavailableException An internal resource for completing this
		call is unavailable, e.g. no mid call data is available at this time.
		@throws MethodNotSupportedException The implementation does not support this method.
		@return the mid call data; the service code type and service code value
	*/
	public MidCallData getMidCallData() throws InvalidStateException,
    ResourceUnavailableException, MethodNotSupportedException;

    /**
        Represents the connection AUTHORIZE_CALL_ATTEMPT state. This state
        implies that the originating or terminating terminal needs to be
        authorized for the call.
        <p>

        <em>Entry criteria  </em>  An indication that the originating or terminating
        terminal needs to be authorized for the call. <br>
        <em > Functions: </em> The originating or terminating terminal characteristics
        should be verified using the calling party's identity and service
        profile. The authority/ability of the party to place the call with
        given properties is verified. The types of authorization may vary
        for different types of originating and terminating resources.  <br>
        <em> Exit criteria: </em>
        The JccConnection object exits this state on receiving indication of
        the success or failure of the authorization process. The originating JccConnection might
        move to the {@link #ADDRESS_COLLECT} state while the terminating JccConnection has to
        move to the {@link #CALL_DELIVERY} state or beyond. Thus, the terminating JccConnection
        cannot be either in the {@link #ADDRESS_COLLECT} or the {@link #ADDRESS_ANALYZE} states.

    */
    public static final int AUTHORIZE_CALL_ATTEMPT    = 2;

    /**
        Represents the connection ADDRESS_COLLECT state.
        <p>

        <em>Entry criteria  </em>  The JccConnection object
        enters this state with the originating party having been authorized
        for this call.  <br>
        <em > Functions: </em> In this state the initial information package is
        collected from the originating party. Information is examined
        according to dialing plan to determine the end of collection. No
        further action may be required if en bloc signaling method is in
        use.  <br>
        <em> Exit criteria: </em>
        This state is exited either because the complete initial information
        package or dialing string has been collected from the originating party
        or because of failure to collect information or even due to reception
        of invalid information from the caller. Timeout and abandon indications
        may also cause the exit from this state.

    */
    public static final int ADDRESS_COLLECT         = 3;

    /**
		Represents the connection ADDRESS_ANALYZE state.

		<p><b>The entry criteria have changed since JCC 1.0a</b></p>
		
		<p><em>Entry criteria  </em> This state is
		entered on the availability of complete or partial initial information package/dialing
		string from the originating party.
		
		The request (with {@link JccProvider#createEventFilterMinimunCollectedAddressLength(int,int,int)})
		to collect a variable number of address digits and report them to the application (through
		{@link JccConnectionListener#connectionAddressAnalyze(JccConnectionEvent)})) is handled within
		this state. The collection of more digits as requested and the reporting of received digits to
		the application (when the digit collect criteria are met) is done in this state. This action is
		recursive, e.g. the application could ask for 3 digits to be collected and when reported, requests
		can be done repeatedly, e.g. the application may for example request first for 3 digits to be
		collected and when reported request further digits.

		<br>
		<em > Functions: </em>
		The information collected is analyzed and/or translated according to
		a dialing plan to determine routing address and call type (e.g. local
		exchange call, transit exchange call, international exchange call).   <br>
		<em> Exit criteria: </em>
		This state is exited on the availability of routing address. Invalid
		information and Abandon indications also cause transition out of this
		state. Exception criteria such as abandon, etc. will cause exit from this state.
    */
    public static final int ADDRESS_ANALYZE          = 4;

    /**
        Represents the connection CALL_DELIVERY state. <p>

		<em> Entry criteria: </em> <ul><li>This state is entered on the
		originating side when the routing address and call type are available.
		<li>On the terminating side this state is entered when the termination
		attempt to the address is authorized.</ul>
		<em> Function: </em> <ul><li>On the originating side this
		state involves selecting of the route as well as sending an
		indication of the desire to set up a call to the specified
		called party. This results in progressing
		the Terminating Connection object.
		<li>On the terminating side this state is involves checking
		the busy/idle status of the terminating access and also
		informing the terminating terminal
		of an incoming call.</ul>
		<em> Exit criteria: </em> <ul><li>This state is exited on the
		originating side when the Terminating Connection object has
		started progressing.
		<li>This state is exited on the terminating side when the
		terminating party is being alerted or the call is accepted.</ul>
    */
    public static final int CALL_DELIVERY           = 5;


//------------------------------------------------------------------------------------------
//FROM JCP

    /**
        Retrieves the state of the JccConnection object.

        @return Integer representing the state of the call.
        See static int's defined in this object.
     */
    public int getState();
    /**
        Retrieves the Jcccall that is associated with this JccConnection. This
        JccCall reference remains valid throughout the lifetime of the JccConnection
        object despite the state of the JccConnection object. This JccCall
        reference does not change once the JccConnection object has been
        created.

        @return JccCall object holding this connection.
    */
    public JccCall getCall();

    /**
    Returns the JccAddress associated with this JccConnection. This JccAddress
    object remains valid throughout the lifetime of the JccConnection
    object despite the state of the JccConnection object. This JccAddress
    reference does not change once the JccConnection object has been
    created.

    @return JccAddress object associated with this JccConnection object.
    */
    public JccAddress getAddress();

    /**
        The <CODE>JccConnection.DISCONNECTED</CODE> state implies it is no
        longer part of the telephone call, although its references to Call and
        Address still remain valid.<p>

        <em> Entry criteria: </em> This state
        is entered when a disconnect indication is received from the
        corresponding party or the application.<br>

        <em> Function: </em> The (bearer) connection to the party
        is disconnected and depending on the incoming network connection, appropriate
        backward signaling takes place. <br>

        <em> Exit criteria: </em>
    */
    public static final int DISCONNECTED              = 0;

        /**
        The <CODE>JccConnection.IDLE</CODE> state is the initial state for all
        new JccConnection objects.
         A JccConnection object in the
        IDLE state while not yet actively participating in a call can
        still reference a JccCall and JccAddress object. <p>

        <em>Entry criteria  </em>  Start of a new call.           <br>

        <em> Functions: </em> Interface (line/trunk) is idled. <br>

        <em> Exit criteria: </em> An indication of the desire to place an outgoing
        call or when the indication of an incoming call is received.

    */
    public static final int IDLE                      = 1;


    /**
        The <CODE>JccConnection.ALERTING</CODE> state implies that the Address is
        being notified of an incoming call.

        <p>
        <em> Entry criteria: </em> This state
        is entered when the terminating party is being alerted of an incoming
        call. <br>

        <em> Function: </em> An indication is sent to the originating party that the terminating
        party is being alerted.<br>

        <em> Exit criteria: </em> This state is exited when the call is accepted
         and answered by the terminating party. Exception criteria such as
         callrejected, NoAnswer and Abandon if possible all cause exit
         from this state.
    */
    public static final int ALERTING                  = 6;

    /**
        The <CODE>JccConnection.CONNECTED</CODE> state implies that
        originating and terminating connection objects and the associated
        Address objects are actively part of a call.   <p>

        <em> Entry criteria: </em><ul><li>On the originating side
        this state is entered after the terminating connection object started progressing.
        <li>On the terminating side this state is entered
        when the Call is accepted and answered by the terminating party.</ul>

        <em> Function: </em> In this state several processes related to
        message accounting/charging, call supervision etc. may be initiated
         if such a capability is provided by the implementation. <br>

        <em> Exit criteria: </em> Exception
        criteria such as disconnect (and suspend for JCC) cause exit from this state.
    */
    public static final int CONNECTED                 = 7;

    /**
        The <CODE>JccConnection.FAILED</CODE> state indicates that a Connection
        to that end of the call has failed for some reason.
        One
        reason why a JccConnection would be in the FAILED state is due to the
        fact that the party was busy. Connections that are in the <code>FAILED</code>
        state are still connected to the call.<p>

        <em> Entry criteria: </em> This state is entered when an exception condition
        is encountered. <br>

        <em> Function: </em> Default handling of the exception condition is provided. <br>

        <em> Exit criteria: </em> Default handling of the exception condition by
        the implementation is completed.
    */
    public static final int FAILED                    = 9;
}

