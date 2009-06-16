/*
 * RecorderEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines the methods for Recorder Events.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface RecorderEvent extends ResourceEvent, RecorderConstants {
    /**
     * Returns the length of the recording, in milliseconds.
     * This length does not include any elided silence.
     * For pause and resume events, 
     * this value is minus one (<code>-1</code>).
     * @return the duration of the recording or -1
     */
    int getDuration();
}
