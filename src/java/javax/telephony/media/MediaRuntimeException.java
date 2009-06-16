/*
 * MediaRuntimeException.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * MediaRuntimeException is the parent for JTAPI Media RuntimeExceptions.
 * <p>
 * This is the Runtime counterpart to MediaException.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public abstract
class MediaRuntimeException extends RuntimeException {
    /** The MediaEvent associated with this Exception.
     * Typically a MediaServiceEvent or ContainerEvent, etc.
     * <!-- If using the provider package, it is a Base.Event -->
     */
     private MediaEvent event = null;
    
    /**
     * Constructs a <code>MediaRuntimeException</code> 
     * with no specified detail message. 
     */
    public MediaRuntimeException() {super();}

    /**
     * Constructs a <code>MediaRuntimeException</code> 
     * with the specified detail message. 
     *
     * @param   s   the detail message.
     */
    public MediaRuntimeException(String s) {super(s);}

    /**
     * Construct a <code>MediaRuntimeException</code>
     * with the specified detail message and Event.
     * 
     * @param s  the detail message describing the error in the event.
     * @param event a MediaEvent 
     */
    public MediaRuntimeException(String s, MediaEvent event) {
	super(s);
	this.event = event;
    }

    /** 
     * Return the MediaEvent associated with this MediaRuntimeException.
     * <p>
     * <b>Note:</b> for <i>pre-condition</i> errors, this may return <tt>null</tt>.
     *
     * @return the MediaEvent associated with this Exception.
     */
    public MediaEvent getEvent() {
	return event;
    }
}
