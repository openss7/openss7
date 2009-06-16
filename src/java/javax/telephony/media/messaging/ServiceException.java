/*
 * ServiceException.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/** A MediaException containing a ServiceEvent.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public class ServiceException extends MediaException {
    /**
     * Constructs a <code>ServiceException</code> 
     * with no specified detail message. 
     */
    public ServiceException() { super(); }

    /**
     * Constructs a <code>ServiceException</code> 
     * with the specified detail message. 
     *
     * @param   s   the detail message.
     */
    public ServiceException(String s) { super(s); }

    /**
     * Construct a <code>ServiceException</code>
     * with the specified detail message and Event.
     *
     * Wraps the given Event in a ServiceException.
     * This exception is thrown if the Event indicates
     * an error or other exceptional status.
     * 
     * @param s  the detail message describing the error in the event.
     * @param event a ServiceEvent 
     */
    public ServiceException(String s, ServiceEvent event) {
	super(s, event);
    }

    /** 
     * Return the ServiceEvent from the method that generated this Exception.
     * The ServiceEvent may contain additional information about what went wrong.
     * @return the ServiceEvent associated with this Exception.
     * @see MediaException#getEvent()
     */
    public ServiceEvent getServiceEvent() {
	return (ServiceEvent)getEvent();
    }
}
