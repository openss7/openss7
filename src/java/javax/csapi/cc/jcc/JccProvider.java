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
 * A <CODE>JccProvider</CODE> represents the telephony software-entity that
 * interfaces with a 
 * telephony subsystem.
 <p>
 The JccProvider acts as 
    a Factory to create standard {@link EventFilter} objects. These 
    standard EventFilter objects should be
    provided by the JCC platform implementation.  It is hoped that these filters
    will meet the needs of many applications, thus sparing them of the need to
    implement them explicitly.  It is also possible that by implementing these 
    on the JCC platform (rather than on the application platform) that the cost 
    of remote filter queries can be eliminated thereby addressing the performance 
    problems.
    <p>    
    Hence, a number of standard filters and filter combiners are proposed. 
    The effect of these standard filters and filter combiners is to allow
    for address ranges in combination with event "masks".  Using these methods, it is possible to create filters 
    that return a given event disposition for address in specific ranges (with holes
    and overlaps), or for specific events, or a combination of both.  
    It is also possible to make filters that combine standard and custom filters. 
    This would make it possible to quickly determine the filter disposition in many
    common cases, using standard filters, and only call a custom filter is unusual 
    cases. We later look at each of these standard filters individually. 

 
 * <H4>Introduction</H4>
 *
 * The telephony subsystem could be a PBX connected to 
 * a server machine, a telephony/fax card in a desktop machine or a networking
 * technology such as IP or ATM.
 *
 * <H4>JccProvider States</H4>
 * 
 * The JccProvider may either be in one of the following states:
 * {@link #IN_SERVICE}, {@link #OUT_OF_SERVICE}, or
 * {@link #SHUTDOWN}. The JccProvider state represents whether any
 * action on that JccProvider may be valid. The following tables describes each
 * state:
 * <p>
 * <TABLE>
 * <TR>
 * <TD WIDTH="20%">
 * {@link #IN_SERVICE}
 * </TD>
 * <TD WIDTH="80%">
 * This state indicates that the JccProvider is currently alive and available for
 * use.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%">
 * {@link #OUT_OF_SERVICE}
 * </TD>
 * <TD WIDTH="80%">
 * This state indicates that a JccProvider is temporarily not available for use.
 * Many methods in this API are invalid when the JccProvider is in
 * this state. JccProviders may come back in service at any time, however, the
 * application can take no direct action to cause this change.
 * </TD>
 * </TR>
 *
 * <TR>
 * <TD WIDTH="20%">
 * {@link #SHUTDOWN}:
 * </TD>
 * <TD WIDTH="80%">
 * This state indicates that a JccProvider is permanently no longer available for
 * use. Most methods in the  API are invalid when the JccProvider
 * is in this state. Applications may use the {@link #shutdown()}
 * method on this interface to cause a JccProvider to move into the
 * {@link #SHUTDOWN} state.
 * </TD>
 * </TR>
 * </TABLE>
 * <p>
 * The following diagram shows the allowable state transitions for the
 * JccProvider.
 * <CENTER>
 *  <H1>Provider FSM</H1>
 * <IMG SRC="doc-files/providerstates.gif" ALIGN="center">
 * </CENTER>
 * <H4>Obtaining a JccProvider</H4>
 *
 * A JccProvider is created and returned by the
 * {@link JccPeer#getProvider(String)} method which is given a string to
 * describe the desired JccProvider. This method sets up any needed communication
 * paths between the application and the JccProvider. The string given is one of
 * the services listed in the {@link JccPeer#getServices()}.
 * 
 * <H4>Listeners and Events</H4>
 *
 * Each time a state changes occurs on a JccProvider, the application is notified
 * via an <EM>event</EM>. This event is reported via the
 * {@link JccProviderListener} interface. Applications instantiate objects
 * which implement this interface and use the
 * {@link #addProviderListener(JccProviderListener)} method to begin the delivery of
 * events.
 * Applications may then query the event object
 * returned for the specific state change, via the {@link JccEvent#getID()}
 * method.
 * When the JccProvider changes state,
 * a {@link JccProviderEvent}
 * is sent to the JccProviderListener, having one of the following event ids:
 * {@link JccProviderEvent#PROVIDER_IN_SERVICE PROVIDER_IN_SERVICE}, {@link JccProviderEvent#PROVIDER_OUT_OF_SERVICE PROVIDER_OUT_OF_SERVICE},
 * and {@link JccProviderEvent#PROVIDER_SHUTDOWN PROVIDER_SHUTDOWN}.
 * A <CODE>JccProviderEvent</CODE> with event id 
 * {@link JccProviderEvent#PROVIDER_EVENT_TRANSMISSION_ENDED PROVIDER_EVENT_TRANSMISSION_ENDED} is delivered to all
 * JccProviderListeners
 * when the JccProvider
 * becomes unobservable and is the final event delivered to the listener for that JccProvider instance. When a 
 JccProvider becomes unobservable, then listener would not be sure of the 
 state of the JccProvider.
 *
 * <H4>Call Objects and Providers</H4>
 *
 * Applications may create a {@link JccCall} object representing new calls using the
 * {@link #createCall()} method. A new JccCall is returned in the
 * {@link JccCall#IDLE} state. Applications may then use this idle JccCall to
 * place new telephone calls. 
 *
 * <H4>Address Objects</H4> 
 A  {@link JccAddress} object represents what we
 * commonly think of as a "telephone number."  
 Unlike JccCall objects, applications may not create JccAddress 
 objects. An address is obtained through {@link #getAddress(String)}. 
 Note that a JccProvider might have only a limited number of addresses in its domain. 
 Hence, if the given address string does not correspond to an address within the domain
 of the provider, then the provider throws an exception. 
 * <H4>Multiple Providers and Multiple Applications</H4>
 *
 * It is not guaranteed or expected that objects
 * instantiated through one JccProvider will be usable with another JccProvider.
 * Therefore, an application that uses two providers must keep all the objects
 * relating to these providers separate. In the future, there may be a
 * mechanism whereby a JccProvider may share objects with another JccProvider if
 * they
 * are speaking to the same telephony hardware, however, such capabilities are
 * not available in this release.
 * <p>
 * Also, multiple applications may request and communicate with the same
 * JccProvider implementation. Typically, since each application executes in its
 * own object space, each will have its own instance of the JccProvider object.
 * These two different JccProvider objects may, in fact, be proxies for a
 * centralized JccProvider instance. Methods in Jcc like {@link #shutdown()} 
 are specified to
 * affect only the invoking applications and have no affect on others. 

<h5>Event Snapshots</h5>

By default, when a {@link JccProviderListener} is added through {@link #addProviderListener(JccProviderListener)},
the first batch of events may be a "snapshot".
That is, if the listener was added after state changes in the Provider, the first batch of events will inform the
application of the current state of the Provider. Note that these snapshot events do NOT provide a history
of all events on the Provider, rather they provide the minimum necessary information to bring the application
up-to-date with the current state of the Provider. 
 @since 1.0
 * @see JccPeer
 * @see JccPeerFactory
 * @see JccProviderListener
 */
public interface JccProvider {

   /**
    This method returns a standard EventFilter which is implemented by the JCC platform.
    This method takes two arrays of eventID integers (values returned from {@link JccEvent#getID()}).  
    For event IDs in the blockEvents array, the filter returns {@link EventFilter#EVENT_BLOCK}.
    For event IDs in notifyEvents, the filter returns {@link EventFilter#EVENT_NOTIFY}.
    If any event ID is not listed in one of the two arrays, the filter returns 
    {@link EventFilter#EVENT_DISCARD}.
    The application is supposed to ensure that  an event ID is not 
    listed in more than one array. If done, the filter may return any one 
    of the listed event dispositions.  
    @param blockEvents is an array containing the list of events for which the application 
    wants to be notified with the call processing blocked.
    @param notifyEvents is an array containing the list of events for which the application 
    wants to be notified without the call processing being blocked.
    @return EventFilter standard EventFilter provided by the JCC platform to enable 
    filtering of events based on the application's requirements.
    @throws ResourceUnavailableException An internal resource for completing this
    call is unavailable. 
    @throws InvalidArgumentException One or more of the provided argument is not valid
    
    */
    
    public EventFilter createEventFilterEventSet(int blockEvents[], int notifyEvents[]) throws 
    ResourceUnavailableException, InvalidArgumentException;

	/**
	This method returns a standard EventFilter which is implemented by the JCC platform.
	For all events that require filtering by this {@link EventFilter}, apply the following:
	<ul>
	<li>If the mid call event type and value are matched and the connection's state (e.g. returned by 
	{@link JccConnection#getState()}) of the connection is {@link JccConnection#CONNECTED}, the filter 
	returns the value matchDisposition. 
	<li>If the mid call event type and value are not matched or the connection's state is not 
	{@link JccConnection#CONNECTED}, then return nomatchDisposition.
	</ul>
	
	@param midCallType an integer that represents the mid call type.  Valid values are defined, i.e. {@link MidCallData#SERVICE_CODE_DIGITS SERVICE_CODE_DIGITS},
	{@link MidCallData#SERVICE_CODE_FACILITY SERVICE_CODE_FACILITY}, {@link MidCallData#SERVICE_CODE_HOOKFLASH SERVICE_CODE_HOOKFLASH}, 
	{@link MidCallData#SERVICE_CODE_RECALL SERVICE_CODE_RECALL}, {@link MidCallData#SERVICE_CODE_U2U SERVICE_CODE_U2U}, and
	{@link MidCallData#SERVICE_CODE_UNDEFINED SERVICE_CODE_UNDEFINED}.
	@param midCallValue a string or regular expression that constrains the mid call value (for the purpose of this specification, the platform 
    will use the Perl5 regular expressions).  
	@param matchDisposition indicates the disposition of a {@link JccConnectionEvent#CONNECTION_MID_CALL}, {@link JccConnection#getMidCallData() getMidCallData()}
	gets access to the {@link MidCallData} object. The disposition should be one of the legal
	dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
	@param nomatchDisposition indicates the disposition of a {@link JccConnectionEvent#CONNECTION_MID_CALL}. This should be one of the legal
	dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
	@return EventFilter standard EventFilter provided by the JCC platform to enable 
	filtering of events based on the application's requirements.    
	@throws ResourceUnavailableException An internal resource for completing this request is unavailable. 
	@throws InvalidArgumentException One or more of the provided argument is not valid
	
	@since 1.0b
	*/
	public EventFilter createEventFilterMidCallEvent(int midCallType, String midCallValue, int matchDisposition, int nomatchDisposition) throws
	ResourceUnavailableException, InvalidArgumentException;

    /**
    This method returns a standard EventFilter which is implemented by the JCC platform.
    For all events that require filtering by this {@link EventFilter}, apply the following:
    <ul>
    <li>If the minimum address length is matched and the connection's state (e.g. returned by {@link JccConnection#getState()}) of the connection is {@link JccConnection#ADDRESS_ANALYZE}, the filter returns the value matchDisposition. 
    <li>If the minimum address length is not matched or the connection's state is not {@link JccConnection#ADDRESS_ANALYZE}, then return nomatchDisposition.
    </ul>
    
    Note that applications may need to remove this filter (through 
    {@link JccCall#removeConnectionListener(JccConnectionListener)} or 
    {@link JccProvider#removeConnectionListener(JccConnectionListener)}) if they are notified once.  Otherwise the 
    filter may be satisfied each time a set of digits is added to the received address and keep firing.  
    If this is not desirable, the application needs to remove the listener as indicated above.
    
    @param minimumAddressLength an integer that represents a minimum address length.  
    @param matchDisposition indicates the disposition of a {@link JccConnectionEvent#CONNECTION_ADDRESS_ANALYZE} where
    the length of the address matches or is greater than the given minimum length. This should be one of the legal
    dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @param nomatchDisposition indicates the disposition of a {@link JccConnectionEvent#CONNECTION_ADDRESS_ANALYZE} where
    the length of the address is less than the given minimum length. This should be one of the legal
    dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @return EventFilter standard EventFilter provided by the JCC platform to enable 
    filtering of events based on the application's requirements.    
    @throws ResourceUnavailableException An internal resource for completing this request is unavailable. 
    @throws InvalidArgumentException One or more of the provided argument is not valid
    
    @since 1.0b
    */
    public EventFilter createEventFilterMinimunCollectedAddressLength(int minimumAddressLength, int matchDisposition, int nomatchDisposition) throws
    ResourceUnavailableException, InvalidArgumentException;

    /**
    This method returns a standard EventFilter which is implemented by the JCC platform.
    For all events that require filtering by this {@link EventFilter}, apply the following:
    <ul>
    <li>If the cause code is matched, the filter returns the value matchDisposition. 
    <li>If the cause code is not matched, then return nomatchDisposition.
    </ul>
    
    @param causeCode an integer that represents a cause code.  Valid cause codes (prefixed by 
    <code>CAUSE_</code>) are defined in {@link JccEvent} and {@link JccCallEvent}.
    @param matchDisposition indicates the disposition of a JCC related event with equal cause code. This should be one of the legal
    dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @param nomatchDisposition indicates the disposition of a JCC related event  with other cause code. This should be one of the legal
    dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @return EventFilter standard EventFilter provided by the JCC platform to enable 
    filtering of events based on the application's requirements.    
    @throws ResourceUnavailableException An internal resource for completing this call is unavailable. 
    @throws InvalidArgumentException One or more of the provided argument is not valid
    
    @since 1.0a
    */
    public EventFilter createEventFilterCauseCode(int causeCode, int matchDisposition, int nomatchDisposition) throws
    ResourceUnavailableException, InvalidArgumentException;

    /**
    This method returns a standard EventFilter which is implemented by the JCC platform.
    This requires a complete ordering of values in JccAddress.  The ordering is arranged 
    by defining the order to be by {@link JccAddress#getName()}'s string order.

    For all events that require filtering by this {@link EventFilter}, apply the following:<br>
    Obtain a string using {@link JccConnection#getAddress()}.{@link JccAddress#getName() getName()}. 
    <ul>
    <li>If the string is between lowAddress and highAddress (inclusive), the filter returns the value matchDisposition. 
    <li>If the string is not in the range specified, then return nomatchDisposition.
    </ul>
	

    @param lowAddress denotes the JccAddress which corresponds to the low end of the 
    range. 
    @param highAddress denotes the JccAddress which corresponds to the high end of the 
    range. 
    @param matchDisposition indicates the disposition of a JCC related event occurring on a
    JccAddress which forms part of the range specified. This should be one of the legal
    dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @param nomatchDisposition indicates the disposition of a JCC related event occurring on a
    JccAddress which DOES not form part of the range specified. This should be one of the legal
    dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @return EventFilter standard EventFilter provided by the JCC platform to enable 
    filtering of events based on the application's requirements.    
        @throws ResourceUnavailableException An internal resource for completing this
        call is unavailable. 
    @throws InvalidArgumentException One or more of the provided argument is not valid
    */

    public EventFilter createEventFilterAddressRange(String lowAddress, String highAddress, int matchDisposition, int nomatchDisposition) throws
    ResourceUnavailableException, InvalidArgumentException;

    /**
    This method returns a standard EventFilter which is implemented by the JCC platform.
    This requires a complete ordering of values in JccAddress.  The ordering is arranged 
    by defining the order to be {@link JccAddress#getName()}'s string order.

    For all events that require filtering by this {@link EventFilter}, apply the following:<br>
    Obtain a string using {@link JccConnection#getAddress()}.{@link JccAddress#getName() getName()}. 
    <ul>
    <li>If this string matches the regular expression addressRegex, the filter returns the value matchDisposition. 
    <li>If no such addresses are matched, then return nomatchDisposition.
    </ul>
	

    @param addressRegex denotes the regular expression (for the purpose of this specification, the platform 
    will use the Perl5 regular expressions). 
    @param matchDisposition indicates the disposition of a JCC related event if the 
    name of the JccAddress matches the regular expression. This should be one of the 
    legal dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @param nomatchDisposition indicates the disposition of a JCC related event if the 
    name of the JccAddress DOES not match the regular expression. This should be one of the 
    legal dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @return EventFilter standard EventFilter provided by the JCC platform to enable 
    filtering of events based on the application's requirements.    
    @throws ResourceUnavailableException An internal resource for completing this
    call is unavailable. 
    @throws InvalidArgumentException One or more of the provided argument is not valid
    */

    public EventFilter createEventFilterAddressRegEx(String addressRegex, int matchDisposition, int nomatchDisposition) throws
    ResourceUnavailableException,InvalidArgumentException ;

    /**
    This method returns a standard EventFilter which is implemented by the JCC platform.
    This filter takes as input an array of EventFilters.
    For a given event, it applies the filters in order.  
    If a filter returns nomatchDisposition, then the next filter is tested.  
    If a filter returns any other disposition, then the filter returns this value and 
    does no further filter evaluation.
    This would normally be called with nomatchDisposition set to {@link EventFilter#EVENT_DISCARD} to 
    process any event (either by notifying or blocking) that any filter wants to 
    process (logical OR).
    @param filters is an array of EventFilters. 
    @param nomatchDisposition indicates the disposition of a JCC related event.
    This should be one of the 
    legal dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @return EventFilter standard EventFilter provided by the JCC platform to enable 
    filtering of events based on the application's requirements.    
        @throws ResourceUnavailableException An internal resource for completing this
        call is unavailable. 
    @throws InvalidArgumentException One or more of the provided argument is not valid

    */

    public EventFilter createEventFilterOr(EventFilter filters[], int nomatchDisposition) throws 
    ResourceUnavailableException, InvalidArgumentException;
     
    /**    
    This method returns a standard EventFilter which is implemented by the JCC platform.
    This filter takes as input an array of EventFilters.
    For a given event, it applies the filters in order. 
    If the values returned from all filters are the same, then this value is returned 
    as the filter value.
    Otherwise, the filter returns nomatchDisposition.
    This means that as soon as any filter returns nomatchDisposition, or as soon as
    two filters return different values, the filter can immediately return 
    nomatchDisposition.
    This would normally be called with nomatchDisposition set to {@link EventFilter#EVENT_DISCARD} to 
    discard any events that any filter wants to discard (logical AND).    
    @param filters is an array of EventFilters. 
    @param nomatchDisposition indicates the disposition of a JCC related event.
    This should be one of the 
    legal dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @return EventFilter standard EventFilter provided by the JCC platform to enable 
    filtering of events based on the application's requirements.    
        @throws ResourceUnavailableException An internal resource for completing this
        call is unavailable. 
    @throws InvalidArgumentException One or more of the provided argument is not valid
     */
    public EventFilter createEventFilterAnd(EventFilter filters[],int nomatchDisposition) throws 
    ResourceUnavailableException, InvalidArgumentException;

    /**
    This method returns a standard EventFilter which is implemented by the JCC platform.
    This requires a complete ordering of values in JccAddress.  The ordering is arranged 
    by defining the order to be {@link JccAddress#getName()}'s string order.
    
    For all events that originate from originating connections and require filtering by this 
    {@link EventFilter}, apply the following:<br>
    Obtain a string, representing a destination address, using {@link JccConnection#getDestinationAddress()}.
    <ul>
    <li>If the destination address is between lowDestAddress and highDestAddress (inclusive), 
    the filter returns the value matchDisposition. 
    <li>If the destination address is not in the range specified or the filter is 
    not operating on an originating connection, then return nomatchDisposition.
    </ul>
	

    @param lowDestAddress denotes the destination JccAddress which corresponds to the low end of the 
    range. 
    @param highDestAddress denotes the destination JccAddress which corresponds to the high end of the 
    range. 
    @param matchDisposition indicates the disposition of a JCC related event occurring on a
    JccAddress which forms part of the range specified. This should be one of the legal
    dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @param nomatchDisposition indicates the disposition of a JCC related event occurring on a
    JccAddress which DOES not form part of the range specified. This should be one of the legal
    dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @return EventFilter standard EventFilter provided by the JCC platform to enable 
    filtering of events based on the application's requirements.    
        @throws ResourceUnavailableException An internal resource for completing this
        call is unavailable. 
    @throws InvalidArgumentException One or more of the provided argument is not valid
    */
	public EventFilter createEventFilterDestAddressRange(String lowDestAddress, String highDestAddress, int matchDisposition, int nomatchDisposition) throws 
    ResourceUnavailableException, InvalidArgumentException;

    /**
    This method returns a standard EventFilter which is implemented by the JCC platform.
    This requires a complete ordering of values in JccAddress.  The ordering is arranged 
    by defining the order to be {@link JccAddress#getName()}'s string order.

    For all events that originate from originating connections and require filtering by this 
    {@link EventFilter}, apply the following:<br>
    Obtain a string, representing a destination address, using {@link JccConnection#getDestinationAddress()}. 
    <ul>
    <li>If this string matches the regular expression destAddressRegex, the filter returns the value matchDisposition. 
    <li>If no such addresses are matched or the filter is not operating on an originating connection, 
    then return nomatchDisposition.
    </ul>
	

    @param destAddressRegex denotes the regular expression (for the purpose of this specification, the platform 
    will use the Perl5 regular expressions). 
    @param matchDisposition indicates the disposition of a JCC related event if the 
    name of the JccAddress matches the regular expression. This should be one of the 
    legal dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @param nomatchDisposition indicates the disposition of a JCC related event if the 
    name of the JccAddress DOES not match the regular expression. This should be one of the 
    legal dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @return EventFilter standard EventFilter provided by the JCC platform to enable 
    filtering of events based on the application's requirements.    
    @throws ResourceUnavailableException An internal resource for completing this
    call is unavailable. 
    @throws InvalidArgumentException One or more of the provided argument is not valid
    */
	public EventFilter createEventFilterDestAddressRegEx(String destAddressRegex, int matchDisposition, int nomatchDisposition) throws 
    ResourceUnavailableException, InvalidArgumentException;

    /**
    This method returns a standard EventFilter which is implemented by the JCC platform.
    This requires a complete ordering of values in JccAddress.  The ordering is arranged 
    by defining the order to be {@link JccAddress#getName()}'s string order.

    For all events that originate from destination connections and require filtering 
    by this {@link EventFilter}, apply the following:<br>
    Obtain a string, representing an originating address, using {@link JccConnection#getOriginatingAddress()}.{@link JccAddress#getName() getName()}.
    <ul>
    <li>If the originating address is between lowAddress and highAddress (inclusive), 
    the filter returns the value matchDisposition. 
    <li>If the originating address is not in the range specified or the filter is not 
    operating on a terminating connection, then return nomatchDisposition.
    </ul>
	

    @param lowOrigAddress denotes the originating JccAddress which corresponds to the low end of the 
    range. 
    @param highOrigAddress denotes the originating JccAddress which corresponds to the high end of the 
    range. 
    @param matchDisposition indicates the disposition of a JCC related event occurring on a
    JccAddress which forms part of the range specified. This should be one of the legal
    dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @param nomatchDisposition indicates the disposition of a JCC related event occurring on a
    JccAddress which DOES not form part of the range specified. This should be one of the legal
    dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @return EventFilter standard EventFilter provided by the JCC platform to enable 
    filtering of events based on the application's requirements.    
        @throws ResourceUnavailableException An internal resource for completing this
        call is unavailable. 
    @throws InvalidArgumentException One or more of the provided argument is not valid
    */
	public EventFilter createEventFilterOrigAddressRange(String lowOrigAddress, String highOrigAddress, int matchDisposition, int nomatchDisposition) throws 
    ResourceUnavailableException, InvalidArgumentException;

    /**
    This method returns a standard EventFilter which is implemented by the JCC platform.
    This requires a complete ordering of values in JccAddress.  The ordering is arranged 
    by defining the order to be {@link JccAddress#getName()}'s string order.
       
    For all events that originate from destination connections and require filtering by 
    this {@link EventFilter}, apply the following:<br>
    Obtain a string, representing an originating address, using {@link JccConnection#getOriginatingAddress()}.{@link JccAddress#getName() getName()}.
    <ul>
    <li>If this string matches the regular expression origAddressRegex, the filter returns the value matchDisposition. 
    <li>If no such addresses are matched or the filter is not operating on an terminating connection, 
    then return nomatchDisposition.
    </ul>
	

    @param origAddressRegex denotes the regular expression for the originating address (for the purpose of this specification, the platform 
    will use the Perl5 regular expressions). 
    @param matchDisposition indicates the disposition of a JCC related event if the 
    name of the JccAddress matches the regular expression. This should be one of the 
    legal dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @param nomatchDisposition indicates the disposition of a JCC related event if the 
    name of the JccAddress DOES not match the regular expression. This should be one of the 
    legal dispositions namely, {@link EventFilter#EVENT_BLOCK}, {@link EventFilter#EVENT_DISCARD} or {@link EventFilter#EVENT_NOTIFY}. 
    @return EventFilter standard EventFilter provided by the JCC platform to enable 
    filtering of events based on the application's requirements.    
    @throws ResourceUnavailableException An internal resource for completing this
    call is unavailable. 
    @throws InvalidArgumentException One or more of the provided argument is not valid
    */
	public EventFilter createEventFilterOrigAddressRegEx(String origAddressRegex, int matchDisposition, int nomatchDisposition) throws 
    ResourceUnavailableException, InvalidArgumentException;

    /**
		Add a call listener to all (future and current) call objects within the domain
		of this provider.  If the listener is added to existing calls, the listener will receive 
		<a href="JccCall.html#event_snapshots">event snapshots</a>.
		The listener added with this method will
		receive events on the call for as long as the implementation can 
		listen to  the Call. In the case that the implementation can no
		longer observe the call, the listeners receives a 
		{@link JccCallEvent#CALL_EVENT_TRANSMISSION_ENDED}.
		The listener receives no more events after it receives the 
		<code>CALL_EVENT_TRANSMISSION_ENDED</code>. 
		
		<H5>Listener Lifetime</H5> 
		The <CODE>JccCallListener</CODE> will 
		receive events until one of the following occurs.
		<OL> 
		<LI>The listener is removed by the application. 
		<LI>The implementation can no longer monitor the call. 
		<LI>The JccCall has completed and moved into the {@link JccCall#INVALID} state.
		</OL> 
		
		<H5>ConnectionListeners</H5> 
		Since {@link JccConnectionListener} inherits from the JccCallListener, it is also 
		possible to add a JccConnectionListener using this method. In such a case, 
		connection events  would also have to be reported to the
		registered listener in addition to the call events. 
		Hence, it is expected that the JCC implementation uses instanceof checks 
		in order to decide if only call events or both call and connection events 
		have to be delivered to the listener. Note that a listener added by this 
		method is expecting all the events without any filtering involved. 
		
		<H5>Multiple Invocations</H5> 
		If an application attempts to add an
		instance of an listener already present on this Call, then a repeated 
		invocation will 
		silently fail, i.e. multiple instances of an listener are not added
		and no exception will be thrown.
		
		@param calllistener JccCallListener object that receives the specified events.
		@throws MethodNotSupportedException The method is not supported.
		@throws ResourceUnavailableException The resource limit for the number of 
		listeners has been exceeded. 
    */

    public void addCallListener( JccCallListener calllistener ) throws MethodNotSupportedException, 
    ResourceUnavailableException;

    /**
		Removes a call listener that was previously registered.  The given listener 
		will no longer receive events generated by the Call objects on this Provider.
		A JccConnectionListener can also be removed using this method. 
		Note that the listeners will stop receiving events for existing calls also. 
		
		Also, if the listener is not currently registered with the Provider, then this 
		method fails silently, i.e. no listener is removed and no exception is thrown.<p>
		
		<B>Post-Conditions:</B> <OL>
		<LI>CALL_EVENT_TRANSMISSION_ENDED is delivered to the application.  
		</OL>
		
		@param calllistener JccCallListener object to be removed.
     */
    public void removeCallListener( JccCallListener calllistener );

    /**
       Add a connection listener to all connections under this JccProvider.
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

       Note that registering for the same event multiple times should not
       result in multiple notifications being sent to an application for
       the same event. Rather, this will result in the last event
       filter being used to determine if events have to be delivered to the
       specified ConnectionListener. 
       <p>
       
       Note that this method is also equivalent to 
       <a href="#addCallListener(JccCallListener,EventFilter)">addCallListener(JccConnectionListener,EventFilter)</A>. 
       since parameter JccConnectionListener is also a JccCallListener. However note that
       JccCallListeners which are not JccConnectionListeners cannot be used as a parameter
       to this method.

       @param connectionlistener JccConnectionListener object that receives the specified events.
       @param filter EventFilter determines if the ConnectionEvent is 
       to be delivered to the specified listener. 
       @throws MethodNotSupportedException The listener cannot be added at this time.
       @throws ResourceUnavailableException The resource limit for the number of 
       listeners has been exceeded. 
       

     */
    public void addConnectionListener( JccConnectionListener connectionlistener, EventFilter filter)throws ResourceUnavailableException, 
    MethodNotSupportedException;;

    /**
		Removes a connection listener that was registered previously.
		The given listener will no longer
		receive events generated by the JccConnection objects related to this JccProvider object through a JccCall object.
		Also, if the listener is not currently registered with the JccProvider, then this 
		method fails silently, i.e. no listener is removed and no exception is thrown.

		<P><B>Post-Conditions:</B> <OL>
		<LI>CALL_EVENT_TRANSMISSION_ENDED is delivered to the application.  
		</OL>

		@param connectionlistener JccConnectionListener object used in the call to addConnectionListener method.
		@since 1.0a
     */
    public void removeConnectionListener( JccConnectionListener connectionlistener);

    /**
     This method imposes or removes load control on calls made to the specified addresses.
        <p>
       The implementation can throw the {@link MethodNotSupportedException} if the 
       platform does not support the load control functionality. <em> Note </em>  that a policy 
       object may be designed to define the policy to be implemented by the platform 
       as a result of this method instead of defining the policy through the given 
       parameters. This might be designed in the future specifications.

       @param address An array of size at most 2. a1[0] denotes the lower
       address of the range while a1[1] denotes the uper address of the range.
       Specifying only one element of the array implies that only an individual address
       is no longer to be the subject of the listener's attention. This constrains the range of addresses
       added to be numerical addresses. For addresses containing non-numerals such as 
       email addresses, we expect that the application would have to add each address
       individually. Note that it is expected that adding a range of non-numerical addresses
       efficiently will be addressed in a future version of this specification.
       @param duration specifies the duration in milliseconds for which the load control should be set. Duration
       of 0 indicates that the load control should be removed. Duration of -1 indicates
       an infinite duration (i.e until disabled by the application). Duration of -2 
       indicates network default duration. 
       @param mechanism specifies the load control mechanism to use (such as admitting one call
       per interval) and any necessary parameters. The contents of this parameter are 
       ignored if the load control duration is set to zero.mech[0] symbolises the call
       admission rate of the call load control mechanism used. mech[1] symbolises the 
       type of call load control mechanism to use. Thus, mech[0] gives the number
       of calls to be admitted per interval and mech[1] denotes the interval (in 
       milliseconds) between calls that are admitted.
       @param treatment specifies the treatment of the calls that are not admitted.The contents of this parameter are 
       ignored if the load control duration is set to zero.
       @throws MethodNotSupportedException If the implementation does not 
       have load control functionality. 
    */    
    public void setCallLoadControl(JccAddress[] address, double duration, double[] mechanism,int[] treatment)
        throws MethodNotSupportedException;

    /**
       Adds a listener to listen to load control related events. Note that the load control
       functionality has to have been specified separately using {@link #setCallLoadControl(JccAddress[],double,double[],int[])}
       method.  
       @param loadcontrollistener The listener implementing the {@link CallLoadControlListener} interface which will
       receive all load control related events. 
       @throws MethodNotSupportedException The listener cannot be added at this time.
       @throws ResourceUnavailableException The resource limit for the number of 
       listeners has been exceeded. 
       @since 1.0a
    */
    public void addCallLoadControlListener(CallLoadControlListener loadcontrollistener) throws
    MethodNotSupportedException, ResourceUnavailableException;

    /**
       Deregisters the load control listener. This results in the listener not receiving
       any load control related events in the future. Note that if loadcontrollistener is not already 
       registered using the {@link #setCallLoadControl(JccAddress[],double,double[],int[])} method then this method 
       fails silently. 
       @param loadcontrollistener The listener implementing the CallLoadControlListener interface which will
       receive all load control related events
    */
    public void removeCallLoadControlListener(CallLoadControlListener loadcontrollistener);
    
    //----------------------------------------------------------------------------------------
    //ADDED FROM JCP
    
    /**
       Returns the state of the JccProvider.
       @return Integer representing the state of the provider. See static int's defined in this object.
       @since 1.0b
     */
    public int getState();

    /**
       Creates a new instance of the call with no connections. The new call object is 
       in the {@link JccCall#IDLE} state. An exception is generated if a new call cannot be
       created for various reasons. This JccProvider must be in the {@link JccProvider#IN_SERVICE} 
       state, otherwise an {@link InvalidStateException} is thrown. 
       
	   <p><B>Pre-conditions:</B><OL>
        <LI>this.getState() == IN_SERVICE<br>
       </OL><B>Post-conditions:</B><OL>
        <LI>this.getState() == IN_SERVICE<br>
        <LI>Assume JccCall call == createCall(); <br>
            call.getState() == IDLE <br>
            call.getConnections() == <CODE>null</CODE> <br>
       </OL>
        
       @return JccCall object representing the new call.
       @throws InvalidStateException If the JccProvider is not in the JccProvider.IN_SERVICE state.
       @throws ResourceUnavailableException An internal resource necessary to create a new Call
       object is unavailable. 
       @throws PrivilegeViolationException If the application does not have the proper
       authority to create a new telephone call object. 
       @throws MethodNotSupportedException The implementation does not support creating
       new JccCall objects. 
		@since 1.0b
     */
    public JccCall createCall()       throws InvalidStateException, ResourceUnavailableException, 
    PrivilegeViolationException, MethodNotSupportedException;

    /**
       Adds a listener to this provider. JccProvider related events are reported via the
       JccProviderListener interface. The JccProvider object will report events to this 
       interface for the lifetime of the JccProvider object or until the listener is 
       removed with the {@link #removeProviderListener(JccProviderListener)} method or until the 
       JccProvider is no longer observable. <p>
       
       If the JccProvider becomes unobservable, a JccProviderEvent with id 
       {@link JccProviderEvent#PROVIDER_EVENT_TRANSMISSION_ENDED PROVIDER_EVENT_TRANSMISSION_ENDED} is delivered to the application as a final
       event. No further events from this JccProvider instance are delivered to the listener unless it is explicitly
       re-added by the application.<p>

       This method is valid anytime and has no pre-conditions. Application must have
       the ability to add listeners to JccProviders so they can monitor the changes in 
       state in the JccProvider. 
       When this method is invoked on a JccProvider in the SHUTDOWN state, 
       the {@link JccProviderEvent#PROVIDER_EVENT_TRANSMISSION_ENDED PROVIDER_EVENT_TRANSMISSION_ENDED} 
       is delivered to the client right away. Further, the 
       {@link JccProviderEvent#PROVIDER_EVENT_TRANSMISSION_ENDED PROVIDER_EVENT_TRANSMISSION_ENDED} 
       event can be delivered even before this method returns. 
       
       <p>
       If an application attempts to add an instance of an listener already
       present on this JccProvider, then repeated attempts to add the instance
       of the listener will silently fail, i.e. multiple instances of an 
       listener are not added and no exception will be thrown.

       <P><B>Post-Conditions:</B>
       <OL>
        <LI>A snapshot of events is delivered to the listener, if appropriate. 
       </OL>

       @param providerlistener JccProviderListener object that receives the specified events.
       @throws MethodNotSupportedException This methods is not supported.
       @throws ResourceUnavailableException The resource limit for the number of 
       listeners has been exceeded. 
       <br>
       @see JccProviderListener
		@since 1.0b
     */
    public void addProviderListener( JccProviderListener providerlistener )  throws ResourceUnavailableException, 
    MethodNotSupportedException;

    /**
       Removes the given listener from the provider. The given listener will no longer
       receive events generated by this JccProvider object. The final event will have
       id {@link JccProviderEvent#PROVIDER_EVENT_TRANSMISSION_ENDED PROVIDER_EVENT_TRANSMISSION_ENDED}. 
       Also, if the listener is not currently registered with the JccProvider, then this 
       method fails silently, i.e. no listener is removed and no exception is thrown. 

       <P><B>Post-Conditions:</B> <OL>
        <LI> JccProviderEvent with id PROVIDER_EVENT_TRANSMISSION_ENDED 
        is delivered to listener.
        </OL>

       @param providerlistener JccProviderListener object being removed.
		@since 1.0b
     */
    public void removeProviderListener( JccProviderListener providerlistener );


    /**
    Returns the unique string name of this JccProvider instance. Each different JccProvider must
    have a unique string associated with it. This is the same string which the 
    application passed to the {@link JccPeer#getProvider(String)} method to create this JccProvider
    instance. 
    @return The unique String name of this Provider. 
		@since 1.0b
    */
    
    public String getName();
    /**
    Returns an {@link JccAddress} object which corresponds to the (telephone) number string
    provided. If the provided name does not correspond to a JccAddress known by the 
    JccProvider and within the JccProvider's domain, {@link InvalidPartyException} is thrown.

       <P><B>Post-Conditions:</B> <OL>
        <LI>Let addres = this.getAddress(addr);
        <LI> Then (addres.getName()).equals(addr) returns true;
        </OL>

    @param address the address string which possibly represents a telephone number.
    @return The JccAddress object which corresponds to the given number. 
    @throws InvalidPartyException This exception, with type {@link InvalidPartyException#UNKNOWN_PARTY}, 
    is thrown if the given number does not correspond to a valid JccAddress under this JccProvider's domain. 
		@since 1.0b
    */
    public JccAddress getAddress(String address) throws InvalidPartyException;
    
    
    /**
    Instructs the JccProvider to shut itself down and provide all necessary cleanup.
    Applications invoke this method when they no
     longer intend to use the JccProvider, most often right before they exit. 
     This method is intended to allow the JccProvider to
     perform any necessary cleanup which would not be taken care of when the 
     Java objects are garbage collected. This
     method causes the JccProvider to move into the {@link #SHUTDOWN} state, 
     in which it will stay indefinitely. <p>

     If the JccProvider is already in the {@link #SHUTDOWN} state, this method does
     nothing. The invocation of this method
     should not affect other applications which are using the same 
     implementation of the JccProvider object. <p>
     
     <B>Post-Conditions:</B> <OL>
     <LI> this.getState() == SHUTDOWN
     </OL>
		@since 1.0b
    */
    public void shutdown();
    /**
        This state indicates that the JccProvider is currently available for use. 
		@since 1.0b
    */
    public static final int IN_SERVICE     = 1;

    /**
        This state indicates that the JccProvider is currently not available for use. Providers
        may come back in service at any time. However, the application can take no direct
        action to cause this change.
		@since 1.0b
    */
    public static final int OUT_OF_SERVICE = 2;

    /**
        This state indicates that the JccProvider is permanently no longer available for use.
		@since 1.0b
     */
    public static final int SHUTDOWN = 3;


    
}

