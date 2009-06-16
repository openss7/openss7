/*
 * NotBoundException.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Thrown when a resource operation is attempted
 * on a MediaService that is not bound to a MediaGroup.
 * This is a RuntimeException and may be thrown by
 * any Resource method, even though it is not declared.
 * <p>
 * <b>Note:</b> if the platform supports Delegation
 * then the MediaGroup may be <i>retrieved</i> at any time.
 * Good practice argues against such a surprise, and
 * it might be nice if there was an attribute to indicate
 * if 'retrieve' should be enabled for an application.
 * But until that is specified one must be aware of the possibility.
 * @see Delegation#retrieve
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public
class NotBoundException extends MediaRuntimeException {
    /**
     * Constructs a <code>NotBoundException</code> 
     * with no specified detail message. 
     */
    public NotBoundException() {super();}

    /**
     * Constructs a <code>NotBoundException</code> 
     * with the specified detail message. 
     *
     * @param   s   the detail message.
     */
    public NotBoundException(String s) {super(s);}

    /**
     * Construct a <code>NotBoundException</code>
     * with the specified detail message and Event.
     * 
     * @param s  the detail message describing the error in the event.
     * @param event a MediaEvent 
     */
    public NotBoundException(String s, MediaEvent event) {
	super(s, event);
    }
}
