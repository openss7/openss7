/*
 * NoResourceException.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Thrown if a resource method is invoked on a MediaService
 * when the corresponding resource is not configured into the MediaService.
 * <p>
 * This typically indicates that the {@link ConfigSpec} used to 
 * {@link MediaService#configure} the MediaService is incorrect; 
 * or the application has otherwise failed
 * to achieve the correct configuration.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public
class NoResourceException extends MediaRuntimeException {
    /**
     * Constructs a <code>NoResourceException</code> 
     * with no specified detail message. 
     */
    public NoResourceException() {super();}

    /**
     * Constructs a <code>NoResourceException</code> 
     * with the specified detail message. 
     *
     * @param   s   the detail message.
     */
    public NoResourceException(String s) {super(s);}

    /**
     * Construct a <code>NoResourceException</code>
     * with the specified detail message and Event.
     * 
     * @param s  the detail message describing the error in the event.
     * @param event a MediaEvent 
     */
    public NoResourceException(String s, MediaEvent event) {
	super(s, event);
    }
}

