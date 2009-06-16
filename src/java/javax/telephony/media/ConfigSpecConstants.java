/*
 * ConfigSpecConstants.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import javax.telephony.*;		// so javadoc will find Connection
/**
 * Symbols and other constants used in ConfigSpec.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public
interface ConfigSpecConstants {
    /** 
     * This attribute identifies how the group implementation 
     * handles the circumstance when two Resource simultaneously 
     * attempt to transmit data (talk) to the network stream.
     * <p>
     * Valid values are: <table border="1" cellpadding="3">
     * <tr><td>Symbol</td><td>Interpretation</td></tr>
     * 
     * <tr><td valign=top><code>v_LastTalker</code></td>
     * <td>Transmit the <i>last talker</i> that begins transmitting:
     * a new talker interrupts the current talker 
     * (pushing that talker on a stack).
     * When the new talker completes, 
     * pop the stack to reconnect the previous talker.</td></tr>
     * 
     * <tr><td valign=top><code>v_FirstTalker</code></td>
     * <td>Transmit the <i>first talker</i> that begins tranmsitting:
     * a new talker is added to the end of a queue, 
     * when the current talker completes, 
     * connect the next talker from the queue</td></tr>
     *
     * <tr><td valign=top><code>v_MixTalkers</code></td>
     * <td>Transmit both (or all) talkers simultaneously, 
     * by mixing the data streams.</td></tr>
     * </table>
     * <p>
     * The default value is <code>v_LastTalker</code>.
     * <p>
     * <b>Note:</b>
     * It is likely that a particular implementation will support
     * only one type of arbitration for all groups.
     * <p>
     */
    Symbol a_Arbitration = ESymbol.Group_ArbitrationScheme;

    /** Value of <code>a_Arbitration</code> indicating that 
     * last talker has priority. 
     */
    Symbol v_LastTalker = ESymbol.Group_LastTalker; 

    /** Value of <code>a_Arbitration</code> indicating that 
     * the first talker has priority. 
     */
    Symbol v_FirstTalker = ESymbol.Group_FirstTalker; 

    /** Value of <code>a_Arbitration</code> indicating that 
     * simultaneous talkers are mixed together.
     */
    Symbol v_MixTalkers = ESymbol.Group_MixTalkers; 

    /**
     * Attribute that identifies one of several [disjoint] switch fabrics.
     * This can be specified to ensure that two groups are connectable.
     */
    //Symbol a_SwitchFabricId = ESymbol.Group_SwitchFabricId;

    /**
     * Boolean attribute that indicates whether
     * {@link MediaEvent#ev_Disconnected ev_Disconnected}
     * shall trigger stopping resource methods.
     * <p>
     * Default value is <code>Boolean.TRUE</code>
     * <p>
     * If the implementation does not detect 
     * or generate <code>ev_Disconnected</code>
     * then this attribute shall be <code>FALSE</code>.
     */
    Symbol a_StopOnDisconnect = ESymbol.Group_StopOnDisconnect;

    /**
     * Boolean attribute that indicates whether the implementation may reclaim
     * resources when all Connections to the Terminal are disconnected.
     * <p> 
     * Default value is <code>Boolean.TRUE</code>
     * @see MediaEvent#ev_TerminalIdle
     */
    Symbol a_DeallocateOnIdle = ESymbol.Group_DeallocateOnIdle;

    /**
     * The Connection to the local Terminal is brought to 
     * this state before 
     * {@link MediaService#bindToServiceName bindToServiceName} completes.
     * <p>
     * <table border="1" cellpadding="3">
     * <tr><td>Valid values are:</td>
     * <td>Corresponding Connection state:</td></tr>
     * <tr><td><code>v_Alerting 	</code></td>
     * <td>{@link Connection#ALERTING}</td></tr>
     * <tr><td><code>v_Connected	</code></td>
     * <td>{@link Connection#CONNECTED}</td></tr>
     * </table>
     * <p>
     * The default value is <code>v_Connected</code>.
     * <p>
     * <b>Note: </b>
     * This attribute establishes a <i>lower-bound</i> on the state, 
     * but the Connection may have progressed beyond that.  
     * For example, if the <code>ALERTING</code> state is requested,
     * the Connection may be in the <code>CONNNECTED</code> state 
     * (or even the <code>DISCONNECTED</code> state) by the time 
     * <code>bindToService()</code> returns to the application.
     */
    Symbol a_LocalState = ESymbol.SCR_LocalConnectionState;
    
    /**
     * The Connection to the remote Terminal is brought to 
     * this state before 
     * {@link MediaService#bindAndConnect bindAndConnect} completes.
     * <p>
     * <table border="1" cellpadding="3">
     * <tr><td>Valid values are:</td>
     * <td>Corresponding Connection state:</td></tr>
     * <tr><td><code>v_InProgress	</code></td>
     * <td>{@link Connection#INPROGRESS}</td></tr>
     * <tr><td><code>v_Alerting 	</code></td>
     * <td>{@link Connection#ALERTING}</td></tr>
     * <tr><td><code>v_Connected	</code></td>
     * <td>{@link Connection#CONNECTED}</td></tr>
     * </table>
     * <p>
     * The default value is <code>v_Connected</code>.
     * <p>
     * <b>Note: </b>
     * This attribute establishes a <i>lower-bound</i> on the state, 
     * but the Connection may have progressed beyond that.  
     * For example, if the <code>ALERTING</code> state is requested,
     * the Connection may be in the <code>CONNNECTED</code> state 
     * (or even the <code>DISCONNECTED</code> state) by the time 
     * <code>bindAndConnect</code> returns to the application.
     */
    Symbol a_RemoteState = ESymbol.SCR_RemoteConnectionState;

    /**
     * Integer attribute that determines how long (in milliseconds) 
     * the implementation waits for the remote party to answer. 
     * If the remote connetion is not answered within 
     * <code>a_AlertingTimeout</code> milliseconds,
     * then <code>bindAndConnect</code> throws a MediaCallException.
     * <p>
     * The default value for this attribute is vendor specific.
     * The implementation of this timeout is approximate,
     * and may vary by several seconds from the specified value.
     */
    Symbol a_AlertingTimeout = ESymbol.SCR_AlertingTimeout;

    /* should we have an interface CallState?
     * Which has these symbols and methods for
     * connect, anwser, disconnect, maybe split.
     * a simple first-party API for apps that use/view only single terminal.
     */

    /** 
     * Value for {@link #a_LocalState} or {@link #a_RemoteState} 
     * that indentifies the corresponding Connection state.
     * @see Connection#INPROGRESS <code>Connection.INPROGRESS</code>
     */
    Symbol v_InProgress 		= ESymbol.SCR_InProgress;

    /** 
     * Value for {@link #a_LocalState} or {@link #a_RemoteState} 
     * that indentifies the corresponding Connection state.
     * @see Connection#ALERTING <code>Connection.ALERTING</code>
     */
    Symbol v_Alerting 			= ESymbol.SCR_Alerting;

    /** 
     * Value for {@link #a_LocalState} or {@link #a_RemoteState} 
     * that indentifies the corresponding Connection state.
     * @see Connection#CONNECTED <code>Connection.CONNECTED</code>
     */
    Symbol v_Connected 			= ESymbol.SCR_Connected;
}
