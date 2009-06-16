/*
 * MediaConnectionConstants.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/** Symbol constants used by MediaConnection.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
*/
public
interface MediaConnectionConstants {
    /** EventIDs */
    /** EventID for bridge method. */
    Symbol ev_Bridge			= ESymbol.Group_SecondaryToSecondary;
    /** EventID for join method. */
    Symbol ev_Join			= ESymbol.Group_PrimaryToSecondary;
    /** EventID for loopback. */
    Symbol ev_Loopback			= ESymbol.Group_PrimaryToPrimary;
    /** EventID for disconnect. */
    Symbol ev_Disconnect		= ESymbol.Group_Disconnect;

    /** Possible EventID for the connect method.
     * <p>
     * <b>Note:</b>
     * In practice the EventID for the connect method
     * may be something other than ev_Connect;
     * it maybe the appropriate one of ev_bridge, ev_join, ev_loopback.
     */
    Symbol ev_Connect			= ESymbol.Group_Connect;

    /** EventID for setDataFlow. */ 
    Symbol ev_SetDataFlow		= ESymbol.Group_SetDataFlow;

    /* Type of connection in a Token. */ 
    // Symbol k_ConnectionType		= ESymbol.Group_ConnectionType;

    /** connType for connect().  Secondary to Secondary. */
    Symbol v_Bridge			= ESymbol.Group_SecondaryToSecondary;
    /** connType for connect().  Primary to Secondary. */
    Symbol v_Join			= ESymbol.Group_PrimaryToSecondary;
    /** connType for connect().  Primary to Primary. */
    Symbol v_Loopback			= ESymbol.Group_PrimaryToPrimary;

    // alternate spellings: S.100-R2 nomenclature
    /** connType for connect(). same as v_Bridge. */
    Symbol v_SecondaryToSecondary	= ESymbol.Group_SecondaryToSecondary;
    /** connType for connect(). same as v_Join. */
    Symbol v_PrimaryToSecondary		= ESymbol.Group_PrimaryToSecondary;
    /** connType for connect(). same as v_Loopback. */
    Symbol v_PrimaryToPrimary		= ESymbol.Group_PrimaryToPrimary;

    /** direction of data flow */
    /** data moves from this MediaService to the other. */
    Symbol v_Talk			= ESymbol.Group_Talk;
    /** data moves from other MediaService to this one. */
    Symbol v_Listen			= ESymbol.Group_Listen;
    /** data moves both direction between this MediaService and the other. */
    Symbol v_Both			= ESymbol.Group_Both;
    /** no data movement, the MediaConnection is <i>muted</i>. */
    Symbol v_None			= ESymbol.Group_None;

    /** Special connection constants */
    /** Use this to setDataFlow or disconnect the Primary connection. */
    Symbol v_Primary			= ESymbol.Group_Primary;
    /** Use this to setDataFlow or disconnect All the Secondary connections. */
    Symbol v_AllSecondary		= ESymbol.Group_AllSecondary;
    /** Use this to setDataFlow or disconnect All the connections. */
    Symbol v_AllConnections		= ESymbol.Group_AllConnections;
}
