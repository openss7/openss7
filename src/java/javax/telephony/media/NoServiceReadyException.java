/*
 * NoServiceReadyException.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
  * Thrown by {@link MediaService#releaseToService releaseToService}
  * if the disposition string is mapped to a serviceName,
  * but no MediaService is ready to bind within the specified timeout.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
  */
public 
class NoServiceReadyException extends MediaBindException {
    /**
     * Constructs a <code>NoServiceReadyException</code> 
     * with no specified detail message. 
     */
    public NoServiceReadyException() {super();}

    /**
     * Constructs a <code>NoServiceReadyException</code> 
     * with the specified detail message. 
     *
     * @param   s   the detail message.
     */
    public NoServiceReadyException(String s) {super(s);}

    /**
     * Construct a <code>NoServiceReadyException</code>
     * with the specified detail message and Event.
     * 
     * @param s  the detail message describing the error in the event.
     * @param event a MediaEvent 
     */
    public NoServiceReadyException(String s, MediaEvent event) {
	super(s, event);
    }
}
