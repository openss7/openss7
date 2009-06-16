/*
 * NamedService.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.messaging;

import javax.telephony.media.*;
import java.util.EventListener;
import java.util.Dictionary;


/**
 * Send and receive messages on a messaging connection.
 * <p>
 * A <tt>NamedService</tt> represents a messaging connection between
 * the Service <i>supplier</i> and the Service <i>subscriber</i>.
 * The supplier creates one end of the connection
 * using <tt>registerService()</tt>;
 * the subscriber creates the other end using <tt>openNamedService()</tt>.
 * When a subscriber opens the <tt>NamedService</tt>, the supplier gets an
 * <tt>onServiceOpened()</tt> notification 
 * (eventID equals <tt>ev_ServiceOpened</tt>).
 * That event contains a <tt>NamedService</tt> that represents
 * the subscriber to the provider.
 * <P>
 * Either end of a message connection may send 
 * a <i>Request</i> or an <i>Event</i> to the other end.
 * If a <b>Request</b> is received, it <i>must</i> be replied to
 * using <tt>ServiceRequest.reply()</tt>.
 * <p>
 * Either end may close the connection.  The other party gets an
 * <tt>onServiceClosed()</tt> notification 
 * (eventID equals <tt>ev_ServiceClosed</tt>). 
 * <p>
 * <b>Note:</b> <tt>ServiceEvents</tt> are dispatched to the
 * <tt>NamedServiceListener</tt> that is established when the <tt>NamedService</tt>
 * is created.  
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface NamedService extends MediaListener, MessageConstants {
    /** Send a request on this message connection, and wait for the reply.
     * <p>
     * Maintains a <i>transaction</i> waiting for a reply.
     * When the reply is returned, the transaction is done
     * and <tt>Async_NamedServiceListener.onRequestDone</tt> is invoked
     * <p>
     * <b>Note:</b>
     * If a reply is not expected, consider using <tt>sendEvent()</tt> instead.
     * <p>
     * <b>Note:</b>On the other end, request messages are delivered to 
     * <tt>NamedServiceLisetner.onServiceRequest</tt>
     *
     * @param cmdID the event Symbol to identify the request
     * @param the payload of the Request.
     * @return an ServiceEvent that is isDone().
     *
     * @throws ServiceClosedException if this service is closed.
     * @throws ServiceRequestException if request returns an Exception
     */
    public ServiceEvent request(Symbol cmdID, Dictionary payload) 
	throws ServiceException;

    /** Async form of request. 
     * 
     * When the a reply is returned, the transaction is done
     * and <tt>Async_NamedServiceListener.onRequestDone</tt> is invoked
     * 
     * @param cmdID the event Symbol to identify the request
     * @param the payload of the Request.
     * @return an Async_ServiceEvent.
     * 
     * @throws ServiceClosedException if this service is closed.
     * @see Async_NamedServiceListener
     */
    public Async_ServiceEvent async_request(Symbol cmdID, Dictionary payload) 
	throws ServiceException;
    
    /** Send a non-transactional event on this message connection.
     * <p>
     * <b>Note:</b>This method does not wait for any reply.
     * <p>
     * <b>Note:</b>On the other end, event messages are delivered to 
     * <tt>NamedServiceLisetner.onServiceEvent</tt>
     * 
     * @param eventID the event Symbol to identify the event
     * @param the payload of the Event.
     * @throws ServiceClosedException if this service is closed.
     */
    public void sendEvent(Symbol eventID, Dictionary payload)
	throws ServiceException;

    /** Return true if this NamedService is open.
     * <p>
     * A NamedService is open until it has been close by either end.
     * @return true if this NamedService is open.
     */
    public boolean isOpen();

    /** Terminate messaging connection between Service and User.
     * Release the subscriber from the provider. 
     * Send <tt>ev_ServiceClosed</tt> event to other end.
     */
    public void close();

    /** Async form of close. 
     */
    public Async_ServiceEvent async_close();

    /** 
     * Add a Listener for Events dispatched by this NamedService.
     * <p>
     * To get asynchronous notification of ServiceEvents,
     * addListener(NamedServiceListener)
     * <p>
     * <b>Note:</b> the NamedServiceListener specified when
     * the NamedService is created is automatically added.
     * <p>
     * <b>Note:</b> This method accepts any <tt>java.util.EventListener</tt>
     * but ServiceEvents are delivered <i>only</i> 
     * to a <tt>NamedServiceListener</tt>.  Unless/until the 
     * MessagingService is extended (for example, by a JtapiPeer vendor)
     * to create other classes of events, other Listeners are not invoked.
     * @see NamedServiceListener
     * @see Async_NamedServiceListener
     */
    public void addListener(EventListener listener);
 
    /** 
     * Remove an EventListener from this NamedService.
     * @see NamedServiceListener
     * @see Async_NamedServiceListener
     */
    public void removeListener(EventListener listener);
}    
