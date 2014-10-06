/*
 * NamedServiceListener.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/** 
 * Listener for non-tranactional events from a NamedService. 
 * <p>
 * A NamedServiceListener <i>must</i> must be supplied
 * to <tt>openNamedService()</tt>.
 * <p>
 * A NamedService on either end of a Messaging connection
 * may receive Request or Event messages from the other end.
 * Those messages are delivered to this Listener.
 * <p>
 * Service specific information can be accessed from 
 * the Dictionary returned from <tt>event.getPayload()</tt>.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface NamedServiceListener extends MediaListener, MessageConstants {
    /** a Transactional Service Request has arrived. 
     * <p>
     * <b>Note:</b> if a  ServiceRequestEvent is received,
     * <tt>event.reply(...)</tt> <i>must</i> be invoked at some point.
     * Failure to reply may confuse or break the 
     * process that sent the request.
     * @see ServiceRequestEvent#reply
     */
    public void onServiceRequest(ServiceRequestEvent event);

    /** a Non-Transactional Event has arrived. */
    public void onServiceEvent(ServiceEvent event);

    /** Indicates a NamedService connection has been opened.
     * EventID = <tt>ev_ServiceOpened</tt>.
     * <p>
     * The application can get a NamedService object
     * representing this connection via
     * <tt>event.getNamedService()</tt>
     * <P>
     * <b>Note:</b> other listeners can be added to the <tt>NamedService</tt>
     * and the initial listener can be removed. For example,
     * <pre>event.getNamedService().addListener(newListener)</pre>
     * should direct all subsequent events to the <tt>newListener</tt>.
     */
    public void onServiceOpened(ServiceEvent event);

    /** Indicates the service connection has been closed.
     * EventID = <tt>ev_ServiceClosed</tt>. 
     * <p>
     * Sent to the end that was still open.
     * On reciept, the associated NamedService is closed.
     */
    public void onServiceClosed(ServiceEvent event);
}
