/*
 * MediaConnectionEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.connection;

import javax.telephony.media.*;

/**
 * Completion Event from connect() and related methods. 
 * <p>
 * All interesting properties are wrapped in the enclosed Token. 
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
interface MediaConnectionEvent extends MediaEvent, 
				       MediaConnectionConstants,
				       MediaConnection.Token {
    /** Return a condensed version of this Event.
     * with enough information to represent the connection.
     * @return the Token value
     */
    MediaConnection.Token getToken(); // keep Token, release/GC the Event
}
