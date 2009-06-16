/*
 * Async_TTSPlayerListener.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.async;

import javax.telephony.media.*;
import java.util.Dictionary;

/**
 * Defines the callback methods for Async_Player transactional Events. 
 * <p>
 * For these methods, the eventID will correspond to the method name.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
interface Async_TTSPlayerListener extends Async_PlayerListener {
    /** Invoked when any TTSPlayer async_xxxTTSDictionary method completes. 
     * <p>
     * @param event a PlayerEvent that isDone();
     */
    void onTTSDictionaryDone(PlayerEvent event);
}
