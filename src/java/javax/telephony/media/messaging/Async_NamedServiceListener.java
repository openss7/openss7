/*
 * Async_NamedServiceListener.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.messaging;

import javax.telephony.media.*;
import java.util.Dictionary;


/**
 * Listener for Asynchronous completion of NamedService methods.
 * <p>
 * Use <tt>NamedService.addListener()</tt> 
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface Async_NamedServiceListener extends MediaListener {
    /**
     * Asynchronous notification that request() has completed.
     * Typically because <tt>sendRequest</tt> was completed using reply() on the remote end.
     */
    public void onRequestDone(ServiceEvent event);

    /**
     * Asynchronous notification that close() has completed.
     * <P>
     * <tt>(event.getEventID().equals(ev_Close))</tt>
     */
    public void onCloseDone(ServiceEvent event);
}
