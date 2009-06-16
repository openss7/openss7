/*
 * MediaConnection.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * The MediaService API for inter-Group connections.
 * <p>
 * <b>Note:</b> this is an optional interface.
 * A platform-specific <tt>MediaService</tt> 
 * (not necessarily <tt><b>Basic</b>MediaService</tt>) 
 * implements <tt>MediaConnection</tt> and <tt>Connectable</tt>.
 * Use <tt>(ms instanceof MediaConnection)</tt> to verify that
 * a MediaService object supports this interface.
 * <p>
 * <b>Note:</b> the target object must also implement <tt>Connectable</tt>.
 * Eventually, things other than a MediaService could be <tt>Connectable</tt>.
 * For example, a Conference object,
 * or a reference to a <tt>external</tt> MediaService.
 * Maybe even a JTAPI Call!
 * <p>
 * <b>Note:</b> multi-party conference service can be modeled by a 
 * MediaService that is configured to contain a <i>Conference</i> Resource.
 * To <tt>bridge()</tt> or <tt>join()</tt> to such a  MediaService constitutes
 * "joining the conference". The Conference Resource may also have an API
 * for floor control, volume control, etc.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface MediaConnection extends MediaConnectionConstants {

    /** Each Token represents a connection between the 
     * the MediaGroups of two bound MediaService objects.
     * <p>
     * <b>Note:</b> The Token contains implementation-specific
     * information that identifies the particular connection-streams.
     * Typically, the Token identifies two connected MediaGroups.
     */
    public interface Token {
	/** Indicate the type of connection.
	 * <p>
	 * <b>Note:</b> For disconnect events, Loopback and Bridge 
	 * may be concealed as Primary and Secondary, respectively.
	 * @return one of <tt>v_Bridge</tt>, <tt>v_Join</tt>, <tt>v_Loopback</tt>. 
	 */
	Symbol getType();

	/** Return the DataFLow Symbol for this connection.
	 * @return one of v_Listen, v_Talk, v_Both, v_None
	 */
	Symbol getDataFlow();
	
	/** Return the MaxDataFLow Symbol for this connection.
	 * @return one of v_Listen, v_Talk, v_Both
	 */
	Symbol getMaxDataFlow();

	/** Tokens are equal iff they refer to a
	 * connection between the same two MediaGroups.
	 * <p>
	 * Note: the Token from connect() (or onConnection())
	 * is equal to the Token from the corresponding onConnected().
	 * <p>
	 * Note: the Token from ms1.connect(ms2) is equal to
	 * the Token from ms1.disconnect(ms2)
	 * <p>
	 * Note: the token from ms1.bridge(ms2) is not equal to
	 * the Token from ms2.bridge(ms1)
	 */
	public boolean equals(Object other);
    }
    // API:
    /**
     * Create a connection from this to the other Connectable. 
     * The type of connection is determined by the first argument.
     *
     * @param connType one of v_Bridge, v_Join, v_Loopback.
     * @param otherConnectable a Connectable or null to unattach.
     * @param maxDataFlow a Symbol indicating flow directions to be reserved.
     * @param initialDataFlow a Symbol indicating the initial data flow
     * @return a MediaConnectionEvent
     * @throws MediaConnectionException if requested operation fails.
     */
    Token connect(Symbol connType,
		  Connectable otherConnectable,
		  Symbol maxDataFlow, 
		  Symbol initialDataFlow)
	throws MediaConnectionException;


    /**
     * Create a v_Bridge connection from this to the other Connectable. 
     *
     * @param otherConnectable a Connectable or null to unattach.
     * @param maxDataFlow a Symbol indicating flow directions to be reserved.
     * @param initialDataFlow a Symbol indicating the initial data flow
     * @return a MediaConnectionEvent
     * @throws MediaConnectionException if requested operation fails.
     */
    Token bridge(Connectable otherConnectable,
		 Symbol maxDataFlow, 
		 Symbol initialDataFlow)
	throws MediaConnectionException;

    /**
     * Create a v_Primary connection from this to the other Connectable.
     * The other Connectable gets a v_Secondary connection.
     *
     * @param otherConnectable a Connectable 
     * that accepts a secondary connection.
     * @param maxDataFlow a Symbol indicating flow directions to be reserved.
     * @param initialDataFlow a Symbol indicating the initial data flow
     * @throws MediaConnectionException 
     * @return a MediaConnectionEvent.
     */
    Token join(Connectable otherConnectable,
	       Symbol maxDataFlow, 
	       Symbol initialDataFlow)
	throws MediaConnectionException;

    /**
     * Create a v_Loopback connection from this to the other Connectable. 
     *
     * @param otherConnectable a Connectable or null to unattach.
     * @param maxDataFlow a Symbol indicating flow directions to be reserved.
     * @param initialDataFlow a Symbol indicating the initial data flow
     * @return a MediaConnectionEvent
     * @throws MediaConnectionException if requested operation fails.
     */
    Token loopback(Connectable otherConnectable,
		   Symbol maxDataFlow, 
		   Symbol initialDataFlow)
	throws MediaConnectionException;

    /** 
     * Disconnect the given connection.
     * The other end receives ev_disconnect.
     * @param connection a Token identifying the connection to be disconnected.
     * @throws MediaConnectionException if requested operation fails.
     */
    void disconnect(Token connection)
	throws MediaConnectionException;

    /** 
     * Disconnect the given connection.
     * The other end receives ev_disconnect.
     * @param connection a Symbol identifying the connection(s)
     * to be disconnected.
     * One of:
     * <tt>v_PrimaryConnection</tt>, 
     * <tt>v_AllSecondary</tt>, 
     * <tt>v_AllConnections</tt>
     * @throws MediaConnectionException if requested operation fails.
     */
    void disconnect(Symbol connection)
	throws MediaConnectionException;

    /** 
     * Change the data flow direction of a connection.
     *
     * The dataFlow can be set to one of:
     * v_Both, v_Talk, v_Listen, v_None.
     * The new dataFlow must not exceed the maxDataFlow established
     * when this connection was created.
     * 
     * <p>
     * <b>Note:</b> v_Talk and v_Listen are with respect to
     * the Connectable on which this method is invoked.
     * <p>
     * @param connection a Token identifying the connection to be changed.
     * @param dataFlow a Symbol to indicate the new direction(s) of flow.
     *
     * @throws MediaConnectionException if requested operation fails.
     */
    void setDataFlow(Token connection, Symbol dataFlow)
	throws MediaConnectionException;

    /** 
     * Change the data flow direction of a connection.
     *
     * The dataFlow can be set to one of:
     * <tt>v_Both</tt>, <tt>v_Talk</tt>, <tt>v_Listen</tt>, <tt>v_None</tt>.
     * The new dataFlow must not exceed the maxDataFlow established
     * when this connection was created.
     * 
     * <p>
     * <b>Note:</b> <tt>v_Talk</tt> and <tt>v_Listen</tt> are with respect to
     * the Connectable on which this method is invoked.
     * <p>
     * @param connection a Symbol identifying the connection(s) to be changed.
     * One of:
     * <tt>v_PrimaryConnection</tt>, 
     * <tt>v_AllSecondary</tt>, 
     * <tt>v_AllConnections</tt>
     * @param dataFlow a Symbol to indicate the new direction(s) of flow.
     *
     * @throws MediaConnectionException if requested operation fails.
     */
    void setDataFlow(Symbol connection, Symbol dataFlow)
	throws MediaConnectionException;
}
