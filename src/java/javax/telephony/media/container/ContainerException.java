/*
 * ContainerException.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.container;

import javax.telephony.media.*;

/**
 * Thrown when a ContainerService method fails for various reasons.
 * ContainerSerrviceException contains an embedded ContainerEvent
 * that explains the details of the problem.
 * <p>
 * A ContainerException generally corresponds to a Container error
 * or other error detected by the implementation that prevents
 * the normal completion of a request.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public 
class ContainerException extends MediaException {
    /**
     * Constructs a <code>ContainerException</code> 
     * with no specified detail message. 
     */
    public ContainerException() {super();}

    /**
     * Constructs a <code>ContainerException</code> 
     * with the specified detail message. 
     *
     * @param   s   the detail message.
     */
    public ContainerException(String s) {super(s);}

    /**
     * Construct a <code>ContainerException</code>
     * with the specified detail message and ContainerEvent.
     *
     * Wraps the given ContainerEvent in a ContainerException.
     * This exception is thrown if the ContainerEvent indicates
     * an error or other exceptional status.
     * 
     * @param s  the detail message
     * @param event a ContainerEvent 
     */
    public ContainerException(String s, ContainerEvent event) {
	super(s, event);
    }

    /** 
     * Extract the underlying ContainerEvent from this ContainerException.
     * The application can use
     * {@link ContainerEvent#getError()}
     * to get more information.
     * <p>
     * <b>Note:</b>
     * It is safe to use <code>getContainerEvent().getError()</code>
     * because <code>getContainerEvent() != null</code>
     * and if there is no error, then <code>getError() == e_OK</code>.
     * 
     * @return the encapsulated ContainerEvent
     */
    public ContainerEvent getContainerEvent() { return (ContainerEvent)getEvent(); }

}
