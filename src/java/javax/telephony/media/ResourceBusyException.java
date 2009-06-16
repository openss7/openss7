/*
 * ResourceBusyException.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media;

/**
 * Thrown when a resource operation cannot be completed 
 * because the resource is already busy.
 * <p>
 * For example, this is thrown by <tt>Player.play()</tt>
 * when the Player is busy and <tt>(Player.p_IfBusy = v_Fail)</tt>.
 * 
 * @deprecated not used or required by the spec, 
 * just use <tt>getError().equals(e_Busy)</tt>.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public
class ResourceBusyException extends MediaResourceException {
    /**
     * Constructs a <code>ResourceBusyException</code> 
     * with no specified detail message. 
     */
    public ResourceBusyException() {super();}

    /**
     * Constructs a <code>ResourceBusyException</code> 
     * with the specified detail message. 
     *
     * @param   s   the detail message.
     */
    public ResourceBusyException(String s) {super(s);}

    /**
     * Construct a <code>ResourceBusyException</code>
     * with the specified detail message and Event.
     * 
     * @param s  the detail message describing the error in the event.
     * @param event a MediaEvent 
     */
    public ResourceBusyException(String s, ResourceEvent event) {
	super(s, event);
    }
}
