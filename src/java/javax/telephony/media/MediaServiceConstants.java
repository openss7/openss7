/*
 * MediaServiceConstants.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import javax.telephony.*;		// for javadoc to find Connection
import java.util.Dictionary;
import javax.telephony.media.async.*; // for javadoc

/** 
 * Defines the Symbols used as EventID in MediaServiceEvents.
 *
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public interface MediaServiceConstants extends MediaConstants {

    // non-tran MediaServiceListener
    /**
     * A Connection to this MediaService has been disconnected.
     * <p>
     * A MediaServiceEvent with this EventID is sent to 
     * <code>{@link MediaServiceListener#onDisconnected}</code> 
     * if the Connection to this MediaService transitions to the state
     * <code>{@link Connection#DISCONNECTED Connection.DISCONNECTED}</code>.
     * If <code>{@link ConfigSpecConstants#a_StopOnDisconnect} = TRUE</code>
     * then all current resource transactions are terminated with 
     * qualifier {@link ResourceEvent#q_Disconnected q_Disconnected}.
     */
    Symbol ev_Disconnected	= ESymbol.CCR_Idle;

    /** 
     * All Connections to this MediaService have been disconnected. 
     * <p>
     * A MediaServiceEvent with this EventID is sent to 
     * <code>{@link MediaServiceListener#onDisconnected}</code> 
     * when the last Connection to this MediaService transitions to the state
     * <code>{@link Connection#DISCONNECTED Connection.DISCONNECTED}</code>.
     * All current resource transactions are terminated with 
     * qualifier {@link ResourceEvent#q_Disconnected q_Disconnected}.
     */
    Symbol ev_TerminalIdle	= ESymbol.SCR_TerminalIdle;

    /** 
     * The Connection to this MediaService has been connected.
     * <p>
     * A MediaServiceEvent with this EventID is sent to 
     * <code>{@link MediaServiceListener#onConnected}</code> 
     * if the Connection to this MediaService transitions to the state
     * <code>{@link Connection#CONNECTED Connection.CONNECTED}</code>.
     * <p>
     * @since 1.4
     */
    Symbol ev_Connected 	= ESymbol.CCR_Active;


    // MediaServiceEvent BindToCallEvent EventIDs
    /** EventID for bindAndConnect(). */
    Symbol ev_BindAndConnect    = ESymbol.SCR_MakeCall;	   // onBindDone
    /** EventID for bindToCall(). */
    Symbol ev_BindToCall        = ESymbol.SCR_Group;	   // onBindDone

    // MediaServiceEvent BindEvent EventIDs
    /** EventID for bindToServiceName(). */
    Symbol ev_BindToServiceName = ESymbol.SCR_RequestGroup;// onBindDone
    /** EventID for bindToTerminalName(). */
    Symbol ev_BindToTerminalName= ESymbol.Group_Create;	   // onBindDone
    /** EventID for bindToTerminal(). */
    Symbol ev_BindToTerminal    = ESymbol.Group_Create;	   // onBindDone

    // MediaServiceEvent ReleaseEvent EventIDs
    /** EventID for release(). */
    Symbol ev_Release           = ESymbol.Group_Return;    // onReleaseDone
    /** EventID for releaseToTag(). */
    Symbol ev_ReleaseToTag 	= ESymbol.Group_Return;	   // onReleaseDone
    /** EventID for releaseToDestroy(). */
    Symbol ev_ReleaseToDestroy  = ESymbol.Group_Return;    // onReleaseDone
    /** EventID for releaseToService(). */
    Symbol ev_ReleaseToService  = ESymbol.Group_ReleaseToService;// onReleaseDone

    // MediaServiceEvent DelegationEvent EventIDs
    /** EventID for delegateToService(). */
    Symbol ev_DelegateToService = ESymbol.Group_HandOff;   // onDelegateToServiceDone
    /** EventID for retrieve(). */
    Symbol ev_Retrieve	     	= ESymbol.Group_Retrieve;  // onRetrieveDone
    /** EventID for releaseDelegated(). */
    Symbol ev_ReleaseDelegated  = ESymbol.Group_Return;    // onReleaseDelegatedDone

    /**
     * Non-trans event when ownership is terminated because
     * an older owner retrieved the MediaGroup.
     * <p>
     * Delivered to {@link MediaServiceListener#onRetrieved}. 
     * <p>
     * Indicates this MediaService is now unbound.
     *
     * @see Delegation#retrieve
     */
    Symbol ev_Retrieved		= ESymbol.Group_Retrieved; // onRetrieved

    // MediaGroupEvent EventIDs: Async.MediaListener.onMediaGroupDone()
    /** EventID for configure(). */
    Symbol ev_Configure 	= ESymbol.Group_Configure;
    /** EventID for getConfigSpec(). */
    Symbol ev_GetConfigSpec     = ESymbol.Group_GetConfigSpec;
    /** EventID for getConfiguration(). */
    Symbol ev_GetConfiguration  = ESymbol.Group_GetConfigSpec;
    /** EventID for getTerminalName(). */
    Symbol ev_GetTerminalName   = ESymbol.CCR_ResourceName;
    /** EventID for getTerminal(). */
    Symbol ev_GetTerminal 	= ESymbol.CCR_ResourceID;
    /** EventID for MediaService.stop(). */
    Symbol ev_GroupStop	     	= ESymbol.Group_Stop; // ev_Stop? ESymbol.Any_Stop?
    /** EventID for triggerRTC(). */
    Symbol ev_TriggerRTC     	= ESymbol.Group_None;
    /** EventID for getUserValues(). */
    Symbol ev_GetUserValues     = ESymbol.Group_GetInfo;
    /** EventID for setUserValues(). */
    Symbol ev_SetUserValues     = ESymbol.Group_PutInfo;
    /** EventID for setUserDictionary(). */
    Symbol ev_SetUserDictionary = ESymbol.Group_PutInfo;
    /** EventID for getParameters(). */
    Symbol ev_GetParameters     = ESymbol.Group_GetParameters;
    /** EventID for setParameters(). */
    Symbol ev_SetParameters     = ESymbol.Group_SetParameters;
}
