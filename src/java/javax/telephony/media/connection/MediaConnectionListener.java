/*
 * MediaConnectionListener.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/** For non-Transactional connection events.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
interface MediaConnectionListener extends MediaListener {
    /** Notification that a connection has been made to
     * the Group bound to this MediaService.
     * The method that created the connection is indicated by
     * the EventID = {ev_Bridge, ev_Join, ev_Loopback}. 
     * <p>
     * The Token identifies the other Group,
     * and contains the initialDataFlow and maxDataFlow.
     * Token.getType() == event.getEventID().
     * @param event a MediaConnectionEvent that identifies the connection/Token.
     */ 
    void onConnect(MediaConnectionEvent event);

    /** 
     * Notification that some connection has been removed.
     * <p>
     * The Token identifies the connection and Group
     * which has been disconnected.
     * dataFlow and maxDataFlow are <tt>null</tt>.
     * <p>EventID = connType = ev_Disconnect.
     * @param event a MediaConnectionEvent that identifies the connection/Token.
     */
    void onDisconnect(MediaConnectionEvent event);

    /** DataFlow for connnection has changed.
     * event.getEventID() = <tt>ev_SetDataFlow</tt>. 
     * <p>
     * The event.getToken() identifies the connection 
     * which has been effected.
     * Token.dataFlow and Token.maxDataFlow are <tt>null</tt>.
     * 
     * <p>event.getEventID() = connType = ev_Disconnect.
     *
     * @param event a MediaConnectionEvent that identifies the connection/Token.
     */
    void onSetDataFlow(MediaConnectionEvent event);
}
