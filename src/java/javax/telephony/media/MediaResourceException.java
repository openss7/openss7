/*
 * MediaResourceException.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/*
 * In the best of all object language worlds, this could be
 * a simple interface that indicate that <i>some</i> object class
 * is a Exception that includes the method getResourceEvent().
 * However, the java compiler does not work that way; the only
 * way to be an Exception is to derive from the Exception class.
 * Hence, we are constrained here to define an actual class.
 */

/**
 * Thrown when a Resource method fails for various reasons.
 * MediaResourceException contains an embedded ResourceEvent
 * that explains the details of the problem.
 * <p>
 * A MediaResourceException generally corresponds to a Resource error
 * or other error detected by the implementation that prevents
 * the normal completion of a request.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public class MediaResourceException extends MediaException {
    /**
     * Constructs a <code>MediaResourceException</code> 
     * with no specified detail message. 
     */
    public MediaResourceException() {super();}

    /**
     * Constructs a <code>MediaResourceException</code> 
     * with the specified detail message. 
     *
     * @param   s   the detail message.
     */
    public MediaResourceException(String s) {super(s);}
    
    /**
     * Construct a <code>MediaResourceException</code>
     * with the specified ResourceEvent and no detail message.
     *
     * Wraps the given ResourceEvent in a MediaResourceException.
     * This exception is thrown if the ResourceEvent indicates
     * an error or other exceptional status.
     * 
     * @param event a ResourceEvent 
     * @deprecated no real use for this constructor
     */
    public MediaResourceException(ResourceEvent event) {
	super(event.getError().toString(), event);
    }
    
    /**
     * Construct a <code>MediaResourceException</code>
     * with the specified detail message and ResourceEvent.
     * 
     * @param s  the detail message describing the error in the event.
     * @param event a ResourceEvent 
     */
    public MediaResourceException(String s, ResourceEvent event) {
	super(s, event);
    }

    /** 
     * Extract the underlying ResourceEvent from this MediaResourceException.
     * The application can use the ResourceEvent accessor
     * {@link ResourceEvent#getRTCTrigger()} 
     * to get more information.
     * <p>
     * The returned <code>ResourceEvent</code> may have
     * <code>getQualifier() == null</code> indicating an error.
     * <p>
     * <b>Note:</b>
     * It is safe to use <code>getResourceEvent().getError()</code>
     * because <code>getResourceEvent() != null</code>
     * and if there is no error, then <code>getError() == e_OK</code>.
     * 
     * @return getEvent() as a ResourceEvent
     */
    public ResourceEvent getResourceEvent() {return (ResourceEvent)getEvent();}
}
