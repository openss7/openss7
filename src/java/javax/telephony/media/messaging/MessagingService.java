/*
 * MessagingService.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
import java.util.Dictionary;
import java.util.EventListener;

/**
 * Holds the methods that extend MediaProvider for 
 * Service registry and Message delivery.
 * <p>
 * The MessagingService allows cooperating applications
 * to send commands and event to each other.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface MessagingService {
    /**
     * Find a service and establish a messaging connection.
     * <p>
     * When the connection is closed 
     * (by any of <tt>releaseService()</tt>, <tt>unregisterService()</tt>, 
     * or session death), 
     * the other end is notified via <tt>onServiceClosed()</tt>.
     * @param serviceName the String name to be registered
     * @param listener for Events and Requests from the service.
     * @param attributes a Dictionary to restrict the choice of 
     * providers that supply <tt>serviceName</tt>
     * @throws NotRegisteredException if name is not registered
     */
    public NamedService openNamedService(String serviceName, 
					 NamedServiceListener listener,
					 Dictionary attributes)
	throws ServiceException;

    /**
     * Async form of openNamedService.
     * @param serviceName the String name to be registered
     * @param listener for Events and Requests from the service.
     * @param attributes a Dictionary to restrict the choice of 
     * providers that supply <tt>serviceName</tt>
     * Async form of openNamedService.
     * @return an Async_ServiceEvent object
     * @throws ServiceException if serviceName cannot be opened.
     */
    public Async_ServiceEvent async_openNamedService(String serviceName, 
						     NamedServiceListener listener,
						     Dictionary attributes)
	throws ServiceException; 
    
    /**
     * Register this MediaProvider as a NamedService with the Server.
     * Other applications can find this service using openNamedService().
     * <p>
     * The <tt>attributes</tt> are stored as part of the registration. 
     * The supplied attributes restrict when <tt>openNamedService</tt> 
     * will find this registered service. For this service to be found,
     * the <tt>attributes</tt> supplied to <tt>openNamedService</tt> must be 
     * a superset of the attributes supplied to <tt>registerService</tt>.
     * <p>
     * A <tt>NamedService</tt> is created in the service supplier when 
     * another application (the service <i>subscriber</i>) connects
     * to this <tt>serviceName</tt> (using <tt>openNamedService</tt>).
     * The given <tt>NamedServiceListener</tt> is added 
     * to each such <tt>NamedService</tt> and 
     * is used for two purposes:
     * <ul><li>
     * The listener receives
     * <tt>onServiceOpened()</tt> and <tt>onServiceClosed()</tt> notifications
     * for each <tt>NamedService</tt> created on this <tt>serviceName</tt>.
     * </li><li>
     * The listener receives
     * <tt>onServiceEvent()</tt> and <tt>onServiceRequest()</tt> notfications
     * for each message delivered to the <tt>NamedService</tt>.
     * </li></ul>
     * <b>Note:</b> other listeners can be added to the <tt>NamedService</tt>
     * and this initial listener can be removed. 
     * 
     * @param serviceName the String by which this NamedService can be opened.
     * @param listener a NamedServiceListener for new message connections.
     * @param attributes a Dictionary of selection attributes
     * @throws NotRegisteredException if could not register
     * @see NamedServiceListener
     */
    public void registerService(String serviceName, 
				NamedServiceListener listener,
				Dictionary attributes)
	throws ServiceException;

    /**
     * Async form of registerService.
     * @param serviceName the String by which this NamedService can be opened.
     * @param listener a NamedServiceListener for new message connections.
     * @param attributes a Dictionary of selection attributes
     * @return an Async_ServiceEvent object
     * @throws ServiceException if serviceName cannot be registered.
     */
    public Async_ServiceEvent async_registerService(String serviceName, 
					     NamedServiceListener listener,
					     Dictionary attributes)
	throws ServiceException;
    
    /** 
     * Unregister the serviceName, preventing further openings.
     * @param serviceName the String by which this NamedService was registered.
     * @throws ServiceException if serviceName cannot be unregistered.
     */
    public void unregisterService(String serviceName)
	throws ServiceException;

    /**
     * Async form of unregisterService. 
     * @param serviceName the String by which this NamedService was registered.
     */
    public Async_ServiceEvent async_unregisterService(String serviceName)
	throws ServiceException;
    
    /**
     * Unregister the given serviceName and close all 
     * NamedService connections to that serviceName.
     * @param serviceName the String by which this NamedService can be opened.
     * @throws ServiceException if serviceName cannot be closed.
     */
    public void close(String serviceName) throws ServiceException;
    /**
     * Async form of close. 
     * @param serviceName the String by which this NamedService can be opened.
     * @return an Async_ServiceEvent object
     * @throws ServiceException if serviceName cannot be closed.
     */
    public Async_ServiceEvent async_close(String serviceName) 
	throws ServiceException;

    
    // This would be useless! if you can create from an event class, 
    // you don't need this: just set the target and done-ify
    // If you cannot create an event, then there is no way to figure out
    // which class of event to use for the given eventID,
    // and hence no way to set up the proper dispatch method.
    // And I do not want to itemize all eventID symbols to dispatch!
    /**
     * Create and send a message to target MediaProvider/NamedService.
     * <p>
     * No need to register and open a message connection
     * if you just want to talk to yourself.
     * <p>
     * Any MPI.Owner with onEventDone() can be the target.
     * 
     * @throws IllegalArgumentException if event is badly formed 
     * or incorrectly addressed.
     */
    // public void putEvent(MPI.Owner target, Symbol eventID, Dictionary payload);


    /** 
     * Add a Listener for Events dispatched by this MessagingService.
     * <p>
     * To get asynchronous notification of MessagingService method completion, use
     * <br><tt>addListener(Async_MessagingServiceListener)</tt>
     * <p>
     * <b>Note:</b> This method accepts any <tt>java.util.EventListener</tt>
     * but ServiceEvents are delivered <i>only</i> 
     * to a <tt>Async_MessagingServiceListener</tt>.
     * Unless/until the MessagingService is extended
     * (for example, by a JtapiPeer vendor)
     * to create other classes of events, other Listeners are not invoked.
     *
     * @see Async_MessagingServiceListener
     */
    public void addListener(EventListener listener);
 
    /** 
     * Remove an EventListener from this NamedService.
     * @see Async_MessagingServiceListener
     */
    public void removeListener(EventListener listener);
}
