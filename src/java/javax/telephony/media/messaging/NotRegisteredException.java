/*
 * NotRegisteredException.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Indicates that openNamedService or registerService failed.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public class NotRegisteredException extends ServiceException {
    /**
     * Constructs a <code>NotRegisteredException</code> 
     * with no specified detail message. 
     */
    public NotRegisteredException() { super(); }

    /**
     * Constructs a <code>NotRegisteredException</code> 
     * with the specified detail message. 
     *
     * @param   s   the detail message.
     */
    public NotRegisteredException(String s) { super(s); }

    /**
     * Construct a <code>NotRegisteredException</code>
     * with the specified detail message and Event.
     *
     * Wraps the given Event in a NotRegisteredException.
     * This exception is thrown if the Event indicates
     * an error or other exceptional status.
     * 
     * @param s  the detail message describing the error in the event.
     * @param event a ServiceRequestEvent 
     */
    public NotRegisteredException(String s, ServiceEvent event) {
	super(s, event);
    }
}
