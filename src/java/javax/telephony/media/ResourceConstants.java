/*
 * ResourceConstants.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines constants used by all Resources.
 * These are inherited by every <i>Resource</i> interface,
 * so they are accessible from any Resource class or instance.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public interface ResourceConstants extends MediaConstants {
    /** Special int value (-1) used to indicate infinite timeout. */
    int		FOREVER 		= -1;
	
    /** Integer value for FOREVER, when used in a value in a Dictionary. */
    Integer	v_Forever 		= new Integer(FOREVER);
	
    /**
     * Symbol returned from {@link ResourceEvent#getError()}
     * when an operation fails because the associated Terminal
     * is in the <code>Connection.DISCONNECTED</code> state.
     */
    Symbol e_Disconnected		= ESymbol.Error_Disconnected;

    /**
     * Qualifier: Completion caused by a Run-Time Control.
     */
    Symbol q_RTC 			= ESymbol.Group_RTC;

    /**
     * Qualifier: this operation was terminated because 
     * the Terninal's Connection to the current Call is in the
     * <code>Connection.DISCONNECTED</code> state.
     * <p>
     * <b>Note:</b>
     * Some implementations may use <code>q_RTC</code> 
     * and <code>rtcc_Disconnected</code>. 
     * 
     * @see #e_Disconnected
     */
    Symbol q_Disconnected		= ESymbol.CCR_Idle;

    /**
     * RTC Trigger: Operation terminated because the Connection to the
     * associated Terminal is in the <code>Connection.DISCONNECTED</code> state.
     * This is delivered in a ResourceEvent via DisconnectedException.
     * @see javax.telephony.Connection#DISCONNECTED Connection.DISCONNECTED
     */
    Symbol rtcc_Disconnected		= ESymbol.CCR_Idle;

    /** 
     * The Symbol returned from {@link ResourceEvent#getRTCTrigger()}
     * representing the RTC Condition for ResourceEvents triggered by 
     * {@link MediaService#triggerRTC(Symbol)}.
     */
    Symbol rtcc_TriggerRTC 		= ESymbol.Group_RTCTrigger;

}
