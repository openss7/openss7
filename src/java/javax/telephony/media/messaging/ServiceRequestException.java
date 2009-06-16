/*
 * ServiceRequestException.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import javax.telephony.media.MediaException;

/**
 * Indicates that the remote service responded with an Exception.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public class ServiceRequestException extends ServiceException {
    /**
     * Constructs a <code>ServiceRequestException</code> 
     * with no specified detail message. 
     */
    public ServiceRequestException() { super(); }

    /**
     * Constructs a <code>ServiceRequestException</code> 
     * with the specified detail message. 
     *
     * @param   s   the detail message.
     */
    public ServiceRequestException(String s) { super(s); }

    /**
     * Construct a <code>ServiceRequestException</code>
     * with the specified detail message and Event.
     *
     * Wraps the given Event in a ServiceRequestException.
     * This exception is thrown if the Event indicates
     * an error or other exceptional status.
     * 
     * @param s  the detail message describing the error in the event.
     * @param event a ServiceEvent
     */
    public ServiceRequestException(String s, ServiceEvent event) {
	super(s, event);
    }
}
