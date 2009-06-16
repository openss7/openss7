/*
 * Async_MediaConnectionListener.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/** Transaction completion Listener for connect() and related methods.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public 
interface Async_MediaConnectionListener extends MediaListener {
    /**
     * Connect complete. 
     * <tt>event.getEventID()</tt> is one of the connection type Symbols:
     * <br>{<tt>ev_Bridge</tt>, <tt>ev_Join</tt>, <tt>ev_Loopback</tt>}. 
     * <p>
     * <tt>event.getToken()</tt> identifies the connection:
     * <ul><li>
     * the other Group,
     * </li><li>
     * connection type, 
     * </li><li>
     * dataFlow, 
     * </li><li>
     * maxDataFlow.
     * </li></ul>
     *
     * @param event the MediaConnectionEvent that is done.
     */
    void onConnectDone( MediaConnectionEvent event );

    /**
     * Disconnect complete. 
     * event.getEventID() = <tt>ev_Disconnect</tt>. 
     * <p>
     * The event.getToken() identifies the connection and Group
     * which has been disconnected.
     * Token.dataFlow and Token.maxDataFlow are <tt>null</tt>.
     * 
     * <p>
     * <tt>event.getEventID() = connType = ev_Disconnect</tt>.
     *
     * @param event the MediaConnectionEvent that is done.
     */
    void onDisconnectDone( MediaConnectionEvent event );

    /**
     * SetDataFlow complete. 
     * event.getEventID() = <tt>ev_Disconnect</tt>. 
     * <p>
     * The event.getToken() identifies the connection and Group
     * which has been disconnected.
     * Token.dataFlow and Token.maxDataFlow are <tt>null</tt>.
     * 
     * <p>
     * <tt>event.getEventID() = connType = ev_Disconnect</tt>.
     *
     * @param event the MediaConnectionEvent that is done.
     */
    void onSetDataFlowDone( MediaConnectionEvent event );
}

