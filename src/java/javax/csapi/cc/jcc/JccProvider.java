/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date$ by $Author$
 */

package javax.csapi.cc.jcc;

/** A JccProvider represents the telephony software-entity that
  * interfaces with a telephony subsystem. <p>
  *
  * The JccProvider acts as a Factory to create standard EventFilter
  * objects. These standard EventFilter objects should be provided by
  * the JCC platform implementation. It is hoped that these filters will
  * meet the needs of many applications, thus sparing them of the need
  * to implement them explicitly. It is also possible that by
  * implementing these on the JCC platform (rather than on the
  * application platform) that the cost of remote filter queries can be
  * eliminated thereby addressing the performance problems. <p>
  *
  * Hence, a number of standard filters and filter combiners are
  * proposed.  The effect of these standard filters and filter combiners
  * is to allow for address ranges in combination with event "masks".
  * Using these methods, it is possible to create filters that return a
  * given event disposition for address in specific ranges (with holes
  * and overlaps), or for specific events, or a combination of both. It
  * is also possible to make filters that combine standard and custom
  * filters. This would make it possible to quickly determine the filter
  * disposition in many common cases, using standard filters, and only
  * call a custom filter is unusual cases. We later look at each of
  * these standard filters individually. <p>
  *
  * <h5>Introduction</h5>
  *
  * The telephony subsystem could be a PBX connected to a server
  * machine, a telephony/fax card in a desktop machine or a networking
  * technology such as IP or ATM. <p>
  *
  * <h5>JccProvider States</h5>
  *
  * The JccProvider may either be in one of the following states:
  * IN_SERVICE, OUT_OF_SERVICE, or SHUTDOWN. The JccProvider state
  * represents whether any action on that JccProvider may be valid. The
  * following tables describes each state: <ul>
  *
  * <li> IN_SERVICE This state indicates that the JccProvider is
  * currently alive and available for use.
  *
  * <li> OUT_OF_SERVICE This state indicates that a JccProvider is
  * temporarily not available for use. Many methods in this API are
  * invalid when the JccProvider is in this state. JccProviders may come
  * back in service at any time, however, the application can take no
  * direct action to cause this change.
  *
  * <li> SHUTDOWN: This state indicates that a JccProvider is
  * permanently no longer available for use. Most methods in the API are
  * invalid when the JccProvider is in this state. Applications may use
  * the shutdown() method on this interface to cause a JccProvider to
  * move into the
  *
  * <li> SHUTDOWN state. </ul>
  *
  * The following diagram shows the allowable state transitions for the
  * JccProvider.
  *
  * <center> Provider FSM </center><br>
  * <center> [providerstates.gif] </center><br>
  *
  * <h5>Obtaining a JccProvider</h5>
  *
  * A JccProvider is created and returned by the
  * JccPeer.getProvider(java.lang.String) method which is given a string to
  * describe the desired JccProvider. This method sets up any needed
  * communication paths between the application and the JccProvider. The
  * string given is one of the services listed in the
  * JccPeer.getServices().
  *
  * <h5>Listeners and Events</h5>
  *
  * Each time a state changes occurs on a JccProvider, the application
  * is notified via an event. This event is reported via the
  * JccProviderListener interface. Applications instantiate objects
  * which implement this interface and use the
  * addProviderListener(JccProviderListener) method to begin the
  * delivery of events. Applications may then query the event object
  * returned for the specific state change, via the JccEvent.getID()
  * method. When the JccProvider changes state, a JccProviderEvent is
  * sent to the JccProviderListener, having one of the following event
  * ids: PROVIDER_IN_SERVICE, PROVIDER_OUT_OF_SERVICE, and
  * PROVIDER_SHUTDOWN. A JccProviderEvent with event id
  * PROVIDER_EVENT_TRANSMISSION_ENDED is delivered to all
  * JccProviderListeners when the JccProvider becomes unobservable and
  * is the final event delivered to the listener for that JccProvider
  * instance. When a JccProvider becomes unobservable, then listener
  * would not be sure of the state of the JccProvider.
  *
  * <h5>Call Objects and Providers</h5>
  *
  * Applications may create a JccCall object representing new calls
  * using the createCall() method. A new JccCall is returned in the
  * JccCall.IDLE state. Applications may then use this idle JccCall to
  * place new telephone calls.
  *
  * <h5>Address Objects</h5>
  *
  * A JccAddress object represents what we commonly think of as a
  * "telephone number." Unlike JccCall objects, applications may not
  * create JccAddress objects. An address is obtained through
  * getAddress(java.lang.String). Note that a JccProvider might have only a
  * limited number of addresses in its domain. Hence, if the given
  * address string does not correspond to an address within the domain
  * of the provider, then the provider throws an exception.
  *
  * <h5>Multiple Providers and Multiple Applications</h5>
  *
  * It is not guaranteed or expected that objects instantiated through
  * one JccProvider will be usable with another JccProvider. Therefore,
  * an application that uses two providers must keep all the objects
  * relating to these providers separate. In the future, there may be a
  * mechanism whereby a JccProvider may share objects with another
  * JccProvider if they are speaking to the same telephony hardware,
  * however, such capabilities are not available in this release. <p>
  *
  * Also, multiple applications may request and communicate with the
  * same JccProvider implementation. Typically, since each application
  * executes in its own object space, each will have its own instance of
  * the JccProvider object. These two different JccProvider objects may,
  * in fact, be proxies for a centralized JccProvider instance. Methods
  * in Jcc like shutdown() are specified to affect only the invoking
  * applications and have no affect on others.
  *
  * <h5>Event Snapshots</h5>
  *
  * By default, when a JccProviderListener is added through
  * addProviderListener(JccProviderListener), the first batch of events
  * may be a "snapshot". That is, if the listener was added after state
  * changes in the Provider, the first batch of events will inform the
  * application of the current state of the Provider. Note that these
  * snapshot events do NOT provide a history of all events on the
  * Provider, rather they provide the minimum necessary information to
  * bring the application up-to-date with the current state of the
  * Provider.
  *
  * @since 1.0
  * @version 1.2.2
  * @author Monavacon Limited
  */
public interface JccProvider {
    /** This state indicates that the JccProvider is currently available
      * for use.
      * @since 1.0b */
    public static final int IN_SERVICE = 1;
    /** This state indicates that the JccProvider is currently not
      * available for use. Providers may come back in service at any
      * time. However, the application can take no direct action to
      * cause this change.
      * @since 1.0b */
    public static final int OUT_OF_SERVICE = 2;
    /** This state indicates that the JccProvider is permanently no
      * longer available for use.
      * @since 1.0b */
    public static final int SHUTDOWN = 3;
    /** This method returns a standard EventFilter which is implemented
      * by the JCC platform. This method takes two arrays of eventID
      * integers (values returned from JccEvent.getID()). For event IDs
      * in the blockEvents array, the filter returns
      * EventFilter.EVENT_BLOCK. For event IDs in notifyEvents, the
      * filter returns EventFilter.EVENT_NOTIFY. If any event ID is not
      * listed in one of the two arrays, the filter returns
      * EventFilter.EVENT_DISCARD. The application is supposed to ensure
      * that an event ID is not listed in more than one array.  If done,
      * the filter may return any one of the listed event dispositions.
      * @param blockEvents
      * An array containing the list of events for which the application
      * wants to be notified with the call processing blocked.
      * @param notifyEvents
      * An array containing the list of events for which the application
      * wants to be notified without the call processing being blocked.
      * @return
      * EventFilter standard EventFilter provided by the JCC platform to
      * enable filtering of events based on the application's
      * requirements.
      * @exception ResourceUnavailableException
      * An internal resource for completing this call is unavailable.
      * @exception InvalidArgumentException
      * One or more of the provided argument is not valid */
    public EventFilter createEventFilterEventSet(int[] blockEvents, int[] notifyEvents)
        throws ResourceUnavailableException, InvalidArgumentException;
    /** This method returns a standard EventFilter which is implemented
      * by the JCC platform. For all events that require filtering by
      * this EventFilter, apply the following: <ul>
      *
      * <li>If the mid call event type and value are matched and the
      * connection's state (e.g. returned by JccConnection.getState())
      * of the connection is JccConnection.CONNECTED, the filter returns
      * the value matchDisposition.
      *
      * <li>If the mid call event type and value are not matched or the
      * connection's state is not JccConnection.CONNECTED, then return
      * nomatchDisposition. </ul>
      * 
      * @param midCallType
      * An integer that represents the mid call type. Valid values are
      * defined, i.e. SERVICE_CODE_DIGITS, SERVICE_CODE_FACILITY,
      * SERVICE_CODE_HOOKFLASH, SERVICE_CODE_RECALL, SERVICE_CODE_U2U,
      * and SERVICE_CODE_UNDEFINED.
      * @param midCallValue
      * A string or regular expression that constrains the mid call
      * value (for the purpose of this specification, the platform will
      * use the Perl5 regular expressions).
      * @param matchDisposition
      * Indicates the disposition of a
      * JccConnectionEvent.CONNECTION_MID_CALL, getMidCallData() gets
      * access to the MidCallData object. The disposition should be one
      * of the legal dispositions namely, EventFilter.EVENT_BLOCK,
      * EventFilter.EVENT_DISCARD or EventFilter.EVENT_NOTIFY.
      * @param nomatchDisposition
      * Indicates the disposition of a
      * JccConnectionEvent.CONNECTION_MID_CALL. This should be one of
      * the legal dispositions namely, EventFilter.EVENT_BLOCK,
      * EventFilter.EVENT_DISCARD or EventFilter.EVENT_NOTIFY.
      * @return
      * EventFilter standard EventFilter provided by the JCC platform to
      * enable filtering of events based on the application's
      * requirements.
      * @exception ResourceUnavailableException
      * An internal resource for completing this request is unavailable.
      * @exception InvalidArgumentException
      * One or more of the provided argument is not valid
      * @since 1.0b */
    public EventFilter createEventFilterMidCallEvent(int midCallType,
            java.lang.String midCallValue, int matchDisposition,
            int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    /** This method returns a standard EventFilter which is implemented
      * by the JCC platform. For all events that require filtering by
      * this EventFilter, apply the following:<ul>
      *
      * <li>If the minimum address length is matched and the
      * connection's state (e.g. returned by JccConnection.getState())
      * of the connection is JccConnection.ADDRESS_ANALYZE, the filter
      * returns the value matchDisposition.
      *
      * <li>If the minimum address length is not matched or the
      * connection's state is not JccConnection.ADDRESS_ANALYZE, then
      * return nomatchDisposition.</ul>
      *
      * Note that applications may need to remove this filter (through
      * JccCall.removeConnectionListener(JccConnectionListener) or
      * removeConnectionListener(JccConnectionListener)) if they are
      * notified once. Otherwise the filter may be satisfied each time a
      * set of digits is added to the received address and keep firing.
      * If this is not desirable, the application needs to remove the
      * listener as indicated above.
      * @param minimumAddressLength
      * An integer that represents a minimum address length.
      * @param matchDisposition
      * Indicates the disposition of a
      * JccConnectionEvent.CONNECTION_ADDRESS_ANALYZE where the length
      * of the address matches or is greater than the given minimum
      * length. This should be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @param nomatchDisposition
      * Indicates the disposition of a
      * JccConnectionEvent.CONNECTION_ADDRESS_ANALYZE where the length
      * of the address is less than the given minimum length. This
      * should be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @return
      * EventFilter standard EventFilter provided by the JCC platform to
      * enable filtering of events based on the application's
      * requirements.
      * @exception ResourceUnavailableException
      * An internal resource for completing this request is unavailable.
      * @exception InvalidArgumentException
      * One or more of the provided argument is not valid
      * @since 1.0b */
    public EventFilter createEventFilterMinimunCollectedAddressLength(
            int minimumAddressLength, int matchDisposition,
            int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    /** This method returns a standard EventFilter which is implemented
      * by the JCC platform. For all events that require filtering by
      * this EventFilter, apply the following:<ul>
      *
      * <li>If the cause code is matched, the filter returns the value
      * matchDisposition.
      *
      * <li>If the cause code is not matched, then return
      * nomatchDisposition.</ul>
      *
      * @param causeCode
      * An integer that represents a cause code.  Valid cause codes
      * (prefixed by CAUSE_) are defined in JccEvent and JccCallEvent.
      * @param matchDisposition
      * Indicates the disposition of a JCC related event with equal
      * cause code. This should be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @param nomatchDisposition
      * Indicates the disposition of a JCC related event with other
      * cause code. This should be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @return
      * EventFilter standard EventFilter provided by the JCC platform to
      * enable filtering of events based on the application's
      * requirements.
      * @exception ResourceUnavailableException
      * An internal resource for completing this call is unavailable.
      * @exception InvalidArgumentException
      * One or more of the provided argument is not valid
      * @since 1.0a */
    public EventFilter createEventFilterCauseCode(int causeCode,
            int matchDisposition, int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    /** This method returns a standard EventFilter which is implemented
      * by the JCC platform. This requires a complete ordering of values
      * in JccAddress. The ordering is arranged by defining the order to
      * be by JccAddress.getName()'s string order. For all events that
      * require filtering by this EventFilter, apply the following:
      * Obtain a string using JccConnection.getAddress().getName(). <ul>
      *
      * <li>If the string is between lowAddress and highAddress
      * (inclusive), the filter returns the value matchDisposition.
      *
      * <li>If the string is not in the range specified, then return
      * nomatchDisposition.</ul>
      * @param lowAddress
      * Denotes the JccAddress which corresponds to the low end of the
      * range.
      * @param highAddress
      * Denotes the JccAddress which corresponds to the high end of the
      * range.
      * @param matchDisposition
      * Indicates the disposition of a JCC related event occurring on a
      * JccAddress which forms part of the range specified. This should
      * be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @param nomatchDisposition
      * Indicates the disposition of a JCC related event occurring on a
      * JccAddress which DOES not form part of the range specified. This
      * should be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @return
      * EventFilter standard EventFilter provided by the JCC platform to
      * enable filtering of events based on the application's
      * requirements.
      * @exception ResourceUnavailableException
      * An internal resource for completing this call is unavailable.
      * @exception InvalidArgumentException
      * One or more of the provided argument is not valid */
    public EventFilter createEventFilterAddressRange(
            java.lang.String lowAddress, java.lang.String highAddress,
            int matchDisposition, int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    /** This method returns a standard EventFilter which is implemented
      * by the JCC platform. This requires a complete ordering of values
      * in JccAddress. The ordering is arranged by defining the order to
      * be JccAddress.getName()'s string order. For all events that
      * require filtering by this EventFilter, apply the following:
      * Obtain a string using JccConnection.getAddress().getName().<ul>
      * <li>If this string matches the regular expression addressRegex,
      * the filter returns the value matchDisposition.
      * <li>If no such addresses are matched, then return nomatchDisposition.</ul>
      * @param addressRegex
      * Denotes the regular expression (for the purpose of this
      * specification, the platform will use the Perl5 regular
      * expressions).
      * @param matchDisposition
      * Indicates the disposition of a JCC related event if the name of
      * the JccAddress matches the regular expression. This should be
      * one of the legal dispositions namely, EventFilter.EVENT_BLOCK,
      * EventFilter.EVENT_DISCARD or EventFilter.EVENT_NOTIFY.
      * @param nomatchDisposition
      * Indicates the disposition of a JCC related event if the name of
      * the JccAddress DOES not match the regular expression. This
      * should be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @return
      * EventFilter standard EventFilter provided by the JCC platform to
      * enable filtering of events based on the application's
      * requirements.
      * @exception ResourceUnavailableException
      * An internal resource for completing this call is unavailable.
      * @exception InvalidArgumentException
      * One or more of the provided argument is not valid */
    public EventFilter createEventFilterAddressRegEx(
            java.lang.String addressRegex, int matchDisposition,
            int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    /** This method returns a standard EventFilter which is implemented
      * by the JCC platform. This filter takes as input an array of
      * EventFilters. For a given event, it applies the filters in
      * order. If a filter returns nomatchDisposition, then the next
      * filter is tested. If a filter returns any other disposition,
      * then the filter returns this value and does no further filter
      * evaluation. This would normally be called with
      * nomatchDisposition set to EventFilter.EVENT_DISCARD to process
      * any event (either by notifying or blocking) that any filter
      * wants to process (logical OR).
      * @param filters
      * An array of EventFilters.
      * @param nomatchDisposition
      * Indicates the disposition of a JCC related event. This should be
      * one of the legal dispositions namely, EventFilter.EVENT_BLOCK,
      * EventFilter.EVENT_DISCARD or EventFilter.EVENT_NOTIFY.
      * @return
      * EventFilter standard EventFilter provided by the JCC platform to
      * enable filtering of events based on the application's
      * requirements.
      * @exception ResourceUnavailableException
      * An internal resource for completing this call is unavailable.
      * @exception InvalidArgumentException
      * One or more of the provided argument is not valid */
    public EventFilter createEventFilterOr(EventFilter[] filters,
            int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    /** This method returns a standard EventFilter which is implemented
      * by the JCC platform. This filter takes as input an array of
      * EventFilters. For a given event, it applies the filters in
      * order. If the values returned from all filters are the same,
      * then this value is returned as the filter value. Otherwise, the
      * filter returns nomatchDisposition. This means that as soon as
      * any filter returns nomatchDisposition, or as soon as two filters
      * return different values, the filter can immediately return
      * nomatchDisposition. This would normally be called with
      * nomatchDisposition set to EventFilter.EVENT_DISCARD to discard
      * any events that any filter wants to discard (logical AND).
      * @param filters
      * An array of EventFilters.
      * @param nomatchDisposition
      * Indicates the disposition of a JCC related event. This should be
      * one of the legal dispositions namely, EventFilter.EVENT_BLOCK,
      * EventFilter.EVENT_DISCARD or EventFilter.EVENT_NOTIFY.
      * @return
      * EventFilter standard EventFilter provided by the JCC platform to
      * enable filtering of events based on the application's
      * requirements.
      * @exception ResourceUnavailableException
      * An internal resource for completing this call is unavailable.
      * @exception InvalidArgumentException
      * One or more of the provided argument is not valid */
    public EventFilter createEventFilterAnd(EventFilter[] filters,
            int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    /** This method returns a standard EventFilter which is implemented
      * by the JCC platform. This requires a complete ordering of values
      * in JccAddress. The ordering is arranged by defining the order to
      * be JccAddress.getName()'s string order. For all events that
      * originate from originating connections and require filtering by
      * this EventFilter, apply the following: Obtain a string,
      * representing a destination address, using
      * JccConnection.getDestinationAddress(). <ul>
      * <li>If the destination address is between lowDestAddress and
      * highDestAddress (inclusive), the filter returns the value
      * matchDisposition.
      * <li>If the destination address is not in the range specified or
      * the filter is not operating on an originating connection, then
      * return nomatchDisposition.</ul>
      * @param lowDestAddress
      * Denotes the destination JccAddress which corresponds to the low
      * end of the range.
      * @param highDestAddress
      * Denotes the destination JccAddress which corresponds to the high
      * end of the range.
      * @param matchDisposition
      * Indicates the disposition of a JCC related event occurring on a
      * JccAddress which forms part of the range specified. This should
      * be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @param nomatchDisposition
      * Indicates the disposition of a JCC related event occurring on a
      * JccAddress which DOES not form part of the range specified. This
      * should be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @return
      * EventFilter standard EventFilter provided by the JCC platform to
      * enable filtering of events based on the application's
      * requirements.
      * @exception ResourceUnavailableException
      * An internal resource for completing this call is unavailable.
      * @exception InvalidArgumentException
      * One or more of the provided argument is not valid */
    public EventFilter createEventFilterDestAddressRange(
            java.lang.String lowDestAddress,
            java.lang.String highDestAddress, int matchDisposition,
            int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    /** This method returns a standard EventFilter which is implemented
      * by the JCC platform. This requires a complete ordering of values
      * in JccAddress. The ordering is arranged by defining the order to
      * be JccAddress.getName()'s string order. For all events that
      * originate from originating connections and require filtering by
      * this EventFilter, apply the following: Obtain a string,
      * representing a destination address, using
      * JccConnection.getDestinationAddress(). <ul>
      * <li>If this string matches the regular expression
      * destAddressRegex, the filter returns the value matchDisposition.
      * <li>If no such addresses are matched or the filter is not
      * operating on an originating connection, then return
      * nomatchDisposition. </ul>
      * @param destAddressRegex
      * Denotes the regular expression (for the purpose of this
      * specification, the platform will use the Perl5 regular
      * expressions).
      * @param matchDisposition
      * Indicates the disposition of a JCC related event if the name of
      * the JccAddress matches the regular expression. This should be
      * one of the legal dispositions namely, EventFilter.EVENT_BLOCK,
      * EventFilter.EVENT_DISCARD or EventFilter.EVENT_NOTIFY.
      * @param nomatchDisposition
      * Indicates the disposition of a JCC related event if the name of
      * the JccAddress DOES not match the regular expression. This
      * should be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @return
      * EventFilter standard EventFilter provided by the JCC platform to
      * enable filtering of events based on the application's
      * requirements.
      * @exception ResourceUnavailableException
      * An internal resource for completing this call is unavailable.
      * @exception InvalidArgumentException
      * One or more of the provided argument is not valid */
    public EventFilter createEventFilterDestAddressRegEx(
            java.lang.String destAddressRegex, int matchDisposition,
            int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    /** This method returns a standard EventFilter which is implemented
      * by the JCC platform. This requires a complete ordering of values
      * in JccAddress. The ordering is arranged by defining the order to
      * be JccAddress.getName()'s string order. For all events that
      * originate from destination connections and require filtering by
      * this EventFilter, apply the following: Obtain a string,
      * representing an originating address, using
      * JccConnection.getOriginatingAddress().getName(). <ul>
      * <li>If the originating address is between lowAddress and
      * highAddress (inclusive), the filter returns the value
      * matchDisposition.
      * <li>If the originating address is not in the range specified or
      * the filter is not operating on a terminating connection, then
      * return nomatchDisposition.</ul>
      * @param lowOrigAddress
      * Denotes the originating JccAddress which corresponds to the low
      * end of the range.
      * @param highOrigAddress
      * Denotes the originating JccAddress which corresponds to the high
      * end of the range.
      * @param matchDisposition
      * Indicates the disposition of a JCC related event occurring on a
      * JccAddress which forms part of the range specified. This should
      * be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @param nomatchDisposition
      * Indicates the disposition of a JCC related event occurring on a
      * JccAddress which DOES not form part of the range specified. This
      * should be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @return
      * EventFilter standard EventFilter provided by the JCC platform to
      * enable filtering of events based on the application's
      * requirements.
      * @exception ResourceUnavailableException
      * An internal resource for completing this call is unavailable.
      * @exception InvalidArgumentException
      * One or more of the provided argument is not valid */
    public EventFilter createEventFilterOrigAddressRange(
            java.lang.String lowOrigAddress,
            java.lang.String highOrigAddress, int matchDisposition,
            int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    /** This method returns a standard EventFilter which is implemented
      * by the JCC platform. This requires a complete ordering of values
      * in JccAddress. The ordering is arranged by defining the order to
      * be JccAddress.getName()'s string order. For all events that
      * originate from destination connections and require filtering by
      * this EventFilter, apply the following: Obtain a string,
      * representing an originating address, using
      * JccConnection.getOriginatingAddress().getName(). <ul>
      * <li>If this string matches the regular expression
      * origAddressRegex, the filter returns the value matchDisposition.
      * <li>If no such addresses are matched or the filter is not
      * operating on an terminating connection, then return
      * nomatchDisposition.</ul>
      * @param origAddressRegex
      * Denotes the regular expression for the originating address (for
      * the purpose of this specification, the platform will use the
      * Perl5 regular expressions).
      * @param matchDisposition
      * Indicates the disposition of a JCC related event if the name of
      * the JccAddress matches the regular expression. This should be
      * one of the legal dispositions namely, EventFilter.EVENT_BLOCK,
      * EventFilter.EVENT_DISCARD or EventFilter.EVENT_NOTIFY.
      * @param nomatchDisposition
      * Indicates the disposition of a JCC related event if the name of
      * the JccAddress DOES not match the regular expression. This
      * should be one of the legal dispositions namely,
      * EventFilter.EVENT_BLOCK, EventFilter.EVENT_DISCARD or
      * EventFilter.EVENT_NOTIFY.
      * @return
      * EventFilter standard EventFilter provided by the JCC platform to
      * enable filtering of events based on the application's
      * requirements.
      * @exception ResourceUnavailableException
      * An internal resource for completing this call is unavailable.
      * @exception InvalidArgumentException
      * One or more of the provided argument is not valid */
    public EventFilter createEventFilterOrigAddressRegEx(
            java.lang.String origAddressRegex, int matchDisposition,
            int nomatchDisposition)
        throws ResourceUnavailableException, InvalidArgumentException;
    /** Add a call listener to all (future and current) call objects
      * within the domain of this provider. If the listener is added to
      * existing calls, the listener will receive event snapshots. The
      * listener added with this method will receive events on the call
      * for as long as the implementation can listen to the Call. In the
      * case that the implementation can no longer observe the call, the
      * listeners receives a JccCallEvent.CALL_EVENT_TRANSMISSION_ENDED.
      * The listener receives no more events after it receives the
      * CALL_EVENT_TRANSMISSION_ENDED. <p>
      *
      * <h5>Listener Lifetime</h5>
      * The JccCallListener will receive events until one of the
      * following occurs.<ol>
      * <li>The listener is removed by the application.
      * <li>The implementation can no longer monitor the call.
      * <li>The JccCall has completed and moved into the JccCall.INVALID
      * state.</ol>
      *
      * <h5>ConnectionListeners</h5>
      * Since JccConnectionListener inherits from the JccCallListener,
      * it is also possible to add a JccConnectionListener using this
      * method. In such a case, connection events would also have to be
      * reported to the registered listener in addition to the call
      * events. Hence, it is expected that the JCC implementation uses
      * instanceof checks in order to decide if only call events or both
      * call and connection events have to be delivered to the listener.
      * Note that a listener added by this method is expecting all the
      * events without any filtering involved.
      *
      * <h5>Multiple Invocations</h5>
      * If an application attempts to add an instance of an listener
      * already present on this Call, then a repeated invocation will
      * silently fail, i.e. multiple instances of an listener are not
      * added and no exception will be thrown.
      * @param calllistener
      * JccCallListener object that receives the specified events.
      * @exception MethodNotSupportedException
      * The method is not supported.
      * @exception ResourceUnavailableException
      * The resource limit for the number of listeners has been
      * exceeded.  */
    public void addCallListener(JccCallListener calllistener)
        throws MethodNotSupportedException, ResourceUnavailableException;
    /** Removes a call listener that was previously registered. The
      * given listener will no longer receive events generated by the
      * Call objects on this Provider. A JccConnectionListener can also
      * be removed using this method. Note that the listeners will stop
      * receiving events for existing calls also. Also, if the listener
      * is not currently registered with the Provider, then this method
      * fails silently, i.e. no listener is removed and no exception is
      * thrown. <p>
      *
      * Post-Conditions:<ol>
      * <li>CALL_EVENT_TRANSMISSION_ENDED is delivered to the
      * application.</ol>
      * @param calllistener
      * JccCallListener object to be removed.  */
    public void removeCallListener(JccCallListener calllistener);
    /** Add a connection listener to all connections under this
      * JccProvider. This method behaves similar to the
      * addCallListener(conListener, filter) method on this interface
      * where conListener is in fact a JccConnectionListener. When
      * conListener is not a JccConnectionListener but is only a
      * JccCallListener, a similar behavior as
      * addCallListener(conListener, newfilter) can be obtained with
      * this method using a JccConnectionListener and a different
      * EventFilter which filters all the JccConnection related events
      * in addition to the events being filtered by newfilter. Note
      * though that using this method only JccConnectionListeners can be
      * added. <p>
      *
      * Note that registering for the same event multiple times should
      * not result in multiple notifications being sent to an
      * application for the same event. Rather, this will result in the
      * last event filter being used to determine if events have to be
      * delivered to the specified ConnectionListener. <p>
      *
      * Note that this method is also equivalent to
      * addCallListener(JccConnectionListener,EventFilter). since
      * parameter JccConnectionListener is also a JccCallListener.
      * However note that JccCallListeners which are not
      * JccConnectionListeners cannot be used as a parameter to this
      * method.
      *
      * @param connectionlistener
      * JccConnectionListener object that receives the specified events.
      * @param filter
      * EventFilter determines if the ConnectionEvent is to be delivered
      * to the specified listener.
      * @exception MethodNotSupportedException
      * The listener cannot be added at this time.
      * @exception ResourceUnavailableException
      * The resource limit for the number of listeners has been exceeded.  */
    public void addConnectionListener(
            JccConnectionListener connectionlistener, EventFilter
            filter)
        throws ResourceUnavailableException, MethodNotSupportedException;
    /** Removes a connection listener that was registered previously.
      * The given listener will no longer receive events generated by
      * the JccConnection objects related to this JccProvider object
      * through a JccCall object. Also, if the listener is not currently
      * registered with the JccProvider, then this method fails
      * silently, i.e. no listener is removed and no exception is
      * thrown. <p>
      *
      * Post-Conditions:<ol>
      * <li>CALL_EVENT_TRANSMISSION_ENDED is delivered to the
      * application.</ol>
      * @param connectionlistener
      * JccConnectionListener object used in the call to
      * addConnectionListener method.
      * @since 1.0a */
    public void removeConnectionListener(JccConnectionListener connectionlistener);
    /** This method imposes or removes load control on calls made to the
      * specified addresses. <p>
      *
      * The implementation can throw the MethodNotSupportedException if
      * the platform does not support the load control functionality.
      * Note that a policy object may be designed to define the policy
      * to be implemented by the platform as a result of this method
      * instead of defining the policy through the given parameters.
      * This might be designed in the future specifications.
      *
      * @param address
      * An array of size at most 2. a1[0] denotes the lower address of
      * the range while a1[1] denotes the uper address of the range.
      * Specifying only one element of the array implies that only an
      * individual address is no longer to be the subject of the
      * listener's attention.  This constrains the range of addresses
      * added to be numerical addresses. For addresses containing
      * non-numerals such as email addresses, we expect that the
      * application would have to add each address individually.  Note
      * that it is expected that adding a range of non-numerical
      * addresses efficiently will be addressed in a future version of
      * this specification.
      * @param duration
      * Specifies the duration in milliseconds for which the load
      * control should be set. Duration of 0 indicates that the load
      * control should be removed.  Duration of -1 indicates an infinite
      * duration (i.e until disabled by the application). Duration of -2
      * indicates network default duration.
      * @param mechanism
      * Specifies the load control mechanism to use (such as admitting
      * one call per interval) and any necessary parameters. The
      * contents of this parameter are ignored if the load control
      * duration is set to zero.mech[0] symbolises the call admission
      * rate of the call load control mechanism used. mech[1] symbolises
      * the type of call load control mechanism to use. Thus, mech[0]
      * gives the number of calls to be admitted per interval and
      * mech[1] denotes the interval (in milliseconds) between calls
      * that are admitted.
      * @param treatment
      * Specifies the treatment of the calls that are not admitted.The
      * contents of this parameter are ignored if the load control
      * duration is set to zero.
      * @exception MethodNotSupportedException
      * If the implementation does not have load control functionality.  */
    public void setCallLoadControl(JccAddress[] address,
            double duration, double[] mechanism, int[] treatment)
        throws MethodNotSupportedException;
    /** Adds a listener to listen to load control related events. Note
      * that the load control functionality has to have been specified
      * separately using
      * setCallLoadControl(JccAddress[],double,double[],int[]) method.
      * @param loadcontrollistener
      * The listener implementing the CallLoadControlListener interface
      * which will receive all load control related events.
      * @exception MethodNotSupportedException
      * The listener cannot be added at this time.
      * @exception ResourceUnavailableException
      * The resource limit for the number of listeners has been
      * exceeded.
      * @since 1.0a */
    public void addCallLoadControlListener(CallLoadControlListener loadcontrollistener) throws MethodNotSupportedException, ResourceUnavailableException;
    /** Deregisters the load control listener. This results in the
      * listener not receiving any load control related events in the
      * future. Note that if loadcontrollistener is not already
      * registered using the
      * setCallLoadControl(JccAddress[],double,double[],int[]) method
      * then this method fails silently.
      * @param loadcontrollistener
      * The listener implementing the CallLoadControlListener interface
      * which will receive all load control related events.  */
    public void removeCallLoadControlListener(CallLoadControlListener loadcontrollistener);
    /** Returns the state of the JccProvider.
      * @return
      * Integer representing the state of the provider. See static int's
      * defined in this object.
      * @since 1.0b */
    public int getState();
    /** Creates a new instance of the call with no connections. The new
      * call object is in the JccCall.IDLE state. An exception is
      * generated if a new call cannot be created for various reasons.
      * This JccProvider must be in the IN_SERVICE state, otherwise an
      * InvalidStateException is thrown. <p>
      *
      * Pre-conditions:<ol>
      * <li>this.getState() == IN_SERVICE</ol>
      *
      * Post-conditions:<ol>
      * <li>this.getState() == IN_SERVICE
      * <li>Assume JccCall call == createCall(); call.getState() == IDLE
      * call.getConnections() == null <ol>
      * @return
      * JccCall object representing the new call.
      * @exception InvalidStateException
      * If the JccProvider is not in the JccProvider.IN_SERVICE state.
      * @exception ResourceUnavailableException
      * An internal resource necessary to create a new Call object is
      * unavailable.
      * @exception PrivilegeViolationException
      * If the application does not have the proper authority to create
      * a new telephone call object.
      * @exception MethodNotSupportedException
      * The implementation does not support creating new JccCall objects.
      * @since 1.0b */
    public JccCall createCall()
        throws InvalidStateException, ResourceUnavailableException,
                          PrivilegeViolationException,
                          MethodNotSupportedException;
    /** Adds a listener to this provider. JccProvider related events are
      * reported via the JccProviderListener interface. The JccProvider
      * object will report events to this interface for the lifetime of
      * the JccProvider object or until the listener is removed with the
      * removeProviderListener(JccProviderListener) method or until the
      * JccProvider is no longer observable. <p>
      *
      * If the JccProvider becomes unobservable, a JccProviderEvent with
      * id PROVIDER_EVENT_TRANSMISSION_ENDED is delivered to the
      * application as a final event. No further events from this
      * JccProvider instance are delivered to the listener unless it is
      * explicitly re-added by the application. <p>
      *
      * This method is valid anytime and has no pre-conditions.
      * Application must have the ability to add listeners to
      * JccProviders so they can monitor the changes in state in the
      * JccProvider. When this method is invoked on a JccProvider in the
      * SHUTDOWN state, the PROVIDER_EVENT_TRANSMISSION_ENDED is
      * delivered to the client right away. Further, the
      * PROVIDER_EVENT_TRANSMISSION_ENDED event can be delivered even
      * before this method returns. <p>
      *
      * If an application attempts to add an instance of an listener
      * already present on this JccProvider, then repeated attempts to
      * add the instance of the listener will silently fail, i.e.
      * multiple instances of an listener are not added and no exception
      * will be thrown. <p>
      *
      * Post-Conditions:<ol>
      * <li>A snapshot of events is delivered to the listener, if
      * appropriate.</ol>
      * @param providerlistener
      * JccProviderListener object that receives the specified events.
      * @exception MethodNotSupportedException
      * This methods is not supported.
      * @exception ResourceUnavailableException
      * The resource limit for the number of listeners has been
      * exceeded.
      * @since 1.0b
      * @see JccProviderListener */
    public void addProviderListener(
            JccProviderListener providerlistener)
        throws ResourceUnavailableException,
                          MethodNotSupportedException;
    /** Removes the given listener from the provider. The given listener
      * will no longer receive events generated by this JccProvider
      * object. The final event will have id
      * PROVIDER_EVENT_TRANSMISSION_ENDED. Also, if the listener is not
      * currently registered with the JccProvider, then this method
      * fails silently, i.e. no listener is removed and no exception is
      * thrown. <p>
      *
      * Post-Conditions: <ol>
      * <li>JccProviderEvent with id PROVIDER_EVENT_TRANSMISSION_ENDED
      * is delivered to listener.</ol>
      * @param providerlistener
      * JccProviderListener object being removed.
      * @since 1.0b */
    public void removeProviderListener(JccProviderListener providerlistener);
    /** Returns the unique string name of this JccProvider instance.
      * Each different JccProvider must have a unique string associated
      * with it. This is the same string which the application passed to
      * the JccPeer.getProvider(java.lang.String) method to create this
      * JccProvider instance.
      * @return
      * The unique java.lang.String name of this Provider.
      * @since 1.0b */
    public java.lang.String getName();
    /** Returns an JccAddress object which corresponds to the
      * (telephone) number string provided. If the provided name does
      * not correspond to a JccAddress known by the JccProvider and
      * within the JccProvider's domain, InvalidPartyException is
      * thrown. <p>
      *
      * Post-Conditions:<ol>
      * <li>Let addres = this.getAddress(addr);
      * <li>Then (addres.getName()).equals(addr) returns true;</ol>
      * @param address
      * The address string which possibly represents a telephone number.
      * @return
      * The JccAddress object which corresponds to the given number.
      * @exception InvalidPartyException
      * This exception, with type InvalidPartyException.UNKNOWN_PARTY,
      * is thrown if the given number does not correspond to a valid
      * JccAddress under this JccProvider's domain.
      * @since 1.0b */
    public JccAddress getAddress(java.lang.String address)
        throws InvalidPartyException;
    /** Instructs the JccProvider to shut itself down and provide all
      * necessary cleanup. Applications invoke this method when they no
      * longer intend to use the JccProvider, most often right before
      * they exit. This method is intended to allow the JccProvider to
      * perform any necessary cleanup which would not be taken care of
      * when the Java objects are garbage collected. This method causes
      * the JccProvider to move into the SHUTDOWN state, in which it
      * will stay indefinitely. <p>
      *
      * If the JccProvider is already in the SHUTDOWN state, this method
      * does nothing. The invocation of this method should not affect
      * other applications which are using the same implementation of
      * the JccProvider object. <p>
      *
      * Post-Conditions:<ol>
      * <li>this.getState() == SHUTDOWN</ol>
      * @since 1.0b */
    public void shutdown();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
