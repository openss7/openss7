/*
 * ServiceEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/** 
 * A MediaEvent from a MessagingService or NamedService.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface ServiceEvent extends MediaEvent, MessageConstants {

    /** Get the associated MessagingService instance.
     * @return a MessagingService instance.
     */
    public MessagingService getMessagingService();

    /** Get the NamedService instance, if any.
     * @return the NamedService instance associated with this event.
     */
    public NamedService getNamedService(); // like a 'Socket'

    /** Get the name of the NamedService.
     * @return the <tt>serviceName</tt> String
     * from <tt>openNamedService</tt> or <tt>registerService</tt>
     */
    public String getServiceName();

    /**
     * Send an Event to the NamedService of this event. 
     * <p>
     * Shorthand for <pre>event.getNamedService().sendEvent(eventID, payload);</pre>
     * This method fails if <tt>getNamedService()</tt> returns null.
     * 
     * @param eventID a Symbol to identify the event being sent.
     * @param payload a Dictionary of eventData for the event being sent.
     * @throws ServiceClosedException if the service is closed.
     */
    public void sendEvent(Symbol eventID, Dictionary payload)
	throws ServiceException;

}
