/*
 * DisconnectedException.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Logically this is a MediaResourceException that is a RuntimeException.
 * <p>
 * Due to the Exception <i>class</i> heirarchy, this can not
 * be more closely related to MediaResourceException.
 * <p>
 * <b>Implementors note:</b>
 * If the Connection becomes disconnected <i>after</i> the method has started,
 * then the resource method completes with
 * <br><code>ResourceEvent().getQualifier()</code> equals <code>q_RTC</code> and
 * <br><code>ResourceEvent().getRTCTrigger()</code> equals <code>rtcc_Disconnected</code>.
 * This should be detected, the error field set to <tt>e_Disconnected</tt>,
 * the event wrapped in an exception, 
 * and thrown to the invoking (synchronous) method.
 * <br>So:
 * <br><code>getResourceEvent().getError()</code> equals <code>e_Disconnected</code> and
 * <br><code>getResourceEvent().getQualifier()</code> equals <code>q_RTC</code> and
 * <br><code>getResourceEvent().getRTCTrigger()</code> equals <code>rtcc_Disconnected</code>.
 * <p>
 * Is the [revised or original] event sent to Listeners?
 */

/**
 * Thrown by any resource method 
 * if the operation stops because the associated Terminal
 * is in the <code>Connection.DISCONNECTED</code> state.
 * <p>
 * Resource operations are stopped or disallowed if the Connection to the 
 * Terminal associated with the MediaService is <code>DISCONNECTED</code>.
 * <p>
 * If the Connection is disconnected,
 * then the resource method throws a DisconnectedException with
 * <br><code>getResourceEvent().getError()</code> equals <code>
 * {@link ResourceConstants#e_Disconnected e_Disconnected}</code>.
 * <p>
 * If the Connection becomes disconnected <i>after</i> the method has started,
 * then the ResourceEvent may contain additional information.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public class
DisconnectedException extends MediaRuntimeException {
    /** 
     * Construct a <code>DisconnectedException</code>
     * with the ResourceEvent and no detail message.
     * @param event a ResourceEvent that indicates <code>DISCONNECTED</code>.
     */
    public DisconnectedException(ResourceEvent event) {
	super(null, event);
    }
    
    /** 
     * Construct a <code>DisconnectedException</code>
     * with the specified detail message and ResourceEvent.
     * @param str the detail message.
     * @param event a ResourceEvent that indicates <code>DISCONNECTED</code>.
     */
    public DisconnectedException(String str, ResourceEvent event) {
	super(str, event);
    }

    /** 
     * Extract the underlying ResourceEvent from this DisconnectedException.
     * Application uses standard ResourceEvent accessors on the event.
     * <p>
     * If an <i>ongoing</i> media operation was terminated, 
     * this ResourceEvent contains <tt>getQualifier()
     * = {@link ResourceEvent#q_Disconnected q_Disconnected}</tt>.
     * If a <b>new</b> media operation can not be <i>started</i>, 
     * this ResourceEvent contains
     * <code>getError()</code> equals <code>e_Disconnected</code>.
     *
     * @return a ResourceEvent that indicates <code>DISCONNECTED</code>.
     */
    public ResourceEvent getResourceEvent() {
	return (ResourceEvent)getEvent();
    }
}
