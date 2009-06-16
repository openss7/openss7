/*
 * Async_MediaConnection.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.connection.async;

import javax.telephony.media.connection.*;
import javax.telephony.media.async.*;
import javax.telephony.media.*;

/** Async version of MediaConnection methods. 
 * @see MediaConnection
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface Async_MediaConnection extends MediaConnectionConstants {

    /** Async version of connect. 
     * Completes to {@link Async_MediaConnectionListener}.
     * @see MediaConnection#connect 
     */
    Async_MediaConnectionEvent async_connect(Symbol connType,
					     Connectable otherConnectable,
					     Symbol maxDataFlow, 
					     Symbol initialDataFlow);

    /** Async version of bridge. 
     * Completes to {@link Async_MediaConnectionListener}.
     * @see MediaConnection#bridge 
     */
    Async_MediaConnectionEvent async_bridge(Connectable otherConnectable,
					    Symbol maxDataFlow, 
					    Symbol initialDataFlow);

    /** Async version of join. 
     * Completes to {@link Async_MediaConnectionListener}.
     * @see MediaConnection#join 
     */
    Async_MediaConnectionEvent async_join(Connectable otherConnectable,
					  Symbol maxDataFlow, 
					  Symbol initialDataFlow);

    /** Async version of loopback. 
     * Completes to {@link Async_MediaConnectionListener}.
     * @see MediaConnection#loopback 
     */
    Async_MediaConnectionEvent async_loopback(Connectable otherConnectable,
					      Symbol maxDataFlow, 
					      Symbol initialDataFlow);

    /** Async version of disconnect. 
     * Completes to {@link Async_MediaConnectionListener}.
     * @see MediaConnection#disconnect 
     */
    Async_MediaConnectionEvent async_disconnect(MediaConnection.Token token);

    /** Async version of disconnect. 
     * Completes to {@link Async_MediaConnectionListener}.
     * @see MediaConnection#disconnect 
     */
    Async_MediaConnectionEvent async_disconnect(Symbol connection);

    /** Async version of setDataFlow. 
     * Completes to {@link Async_MediaConnectionListener}.
     * @see MediaConnection#setDataFlow 
     */
    Async_MediaConnectionEvent async_setDataFlow(MediaConnection.Token token, Symbol dataFlow);

    /** Async version of setDataFlow. 
     * Completes to {@link Async_MediaConnectionListener}.
     * @see MediaConnection#setDataFlow 
     */
    Async_MediaConnectionEvent async_setDataFlow(Symbol connection, Symbol dataFlow);
}
