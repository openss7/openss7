/*
 * MediaBindException.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * The class of things that go wrong during bind or release.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
abstract 
public class MediaBindException extends MediaServiceException {
    /**
     * Constructs a <code>MediaBindException</code> 
     * with no specified detail message. 
     */
    public MediaBindException() {super();}

    /**
     * Constructs a <code>MediaBindException</code> 
     * with the specified detail message. 
     *
     * @param   s   the detail message.
     */
    public MediaBindException(String s) {super(s);}

    /**
     * Construct a <code>MediaBindException</code>
     * with the specified detail message and Event.
     * 
     * @param s  the detail message describing the error in the event.
     * @param event a MediaEvent 
     */
    public MediaBindException(String s, MediaEvent event) {
	super(s, event);
    }
}
