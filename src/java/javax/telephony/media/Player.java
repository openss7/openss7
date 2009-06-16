/*
 * Player.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import java.util.Dictionary;

/**
 * Defines methods for playing media data into a Terminal.
 * A Player extracts data from a media data object (MDO) 
 * (which supplies a stream of media to be played),
 * transcodes the data as necessary for the telephony network,
 * and streams the resultant data out to that network.
 * <p>
 * The <tt>play()</tt> method accepts one or an array of Strings
 * which identify the media streams to be played.  Each media stream
 * may be encoded by any coder type supported by this player resource.
 * <p>
 * A Player may optionally support additional processing features.
 * These features define parameters and runtime controls that affect
 * operation of an ongoing play operation.
 * The presence of these features is indicated by a <tt>true</tt>
 * value for the following attributes:
 * <table border="1" cellpadding="3">
 * <tr><td>Attribute:</td><td>indicates the ability to:</td></tr>
 * <tr><td>a_Pause</td><td>pause and resume in place</td></tr>
 * <tr><td>a_Speed</td><td>change the playback speed</td></tr>
 * <tr><td>a_Volume</td><td>increase and decrease the volume</td></tr>
 * <tr><td>a_Jump</td><td>jump forward and backward while playing</td></tr>
 * </table>
 * Each of these is explained in the <a href="#Attributes">Attributes</a> 
 * section.
 * <p>
 * In addition to the <tt>play()</tt> method, a number of RTC actions
 * may affect the operation of the player.  These can be invoked through
 * Run Time Control {@link RTC} actions, or by the
 * <tt>pausePlayer()</tt>, <tt>resumePlayer()</tt>, <tt>stopPlayer()</tt>
 * <tt>jumpPlayer()</tt>, <tt>adjustPlayerVolume()</tt>, <tt>adjustPlayerSpeed()</tt>
 * methods.
 * <p>
 * Coding types for data objects are defined in {@link CoderConstants}.
 * <p>
 * The MDO to be played is identified by its String name. 
 * The String name is an identifier that may be interpreted 
 * as a path or URL (URI) for the MDO.
 * <p>
 * <b>Note:</b>
 * The format of the String that identifies 
 * an MDO is not defined by this release of specification
 * [although it will be compatible with that used in the ContainerService].
 * The vendor-specific implementation of the MediaService 
 * (for example, the vendor of the server or resource)
 * shall define the supported String formats.
 * <p>
 * For example, an implementation may support URLs or File pathnames;
 * in which case an application could use Strings of the form: 
 * <tt>"file://node/dir/name"</tt> or <tt>"dir/dir/name"</tt>
 * or <tt>"container/container/dataobject"</tt>.
 * <p>
 * If the MDO identifier is a file pathname, it is interpreted in the
 * logical filename-space of the <b>server</b>.  
 * Applications and their prompts need to
 * be installed with correct pathnames.
 * <p>
 * <h4>Operation:</h4>
 *
 * <h5>Player States:</h5>
 * The following is the state diagram for a player.
 * <br>
 * <IMG SRC="doc-files/PlayerStates.gif" ALT="States: IDLE<=>ACTIVE<=>PAUSED">
 * <p>
 * The Player has three states: Idle, Active, and Paused.
 * <p>
 * In the Idle state, the Player is performing no coding or
 * transmit operations. The state transitions to the Active
 * or Paused state when the play() method starts. 
 * The state of the Player after play() starts is determined
 * by the Boolean value of the parameter <tt>p_StartPaused</tt>.
 * The state transitions to Active if <tt>p_StartPaused</tt> is false.
 * The state transitions to Paused if <tt>p_StartPaused</tt> is true.
 * <p>
 * In the Paused state, the player is not transmitting the media stream.
 * It retains its place in the MDO being played, and resumes from
 * the same place when the RTC action, <tt>rtca_Resume</tt> is received. 
 * <p>
 * In the Active state, the Player is busy, actively transmitting the 
 * media stream from a MDO to its output media stream (the Terminal). 
 * The Player continues in this state 
 * until it reaches the end of the MDO list
 * or until
 * RTC actions tell it to pause or stop. 
 * In the Active state, the Player may receive RTC commands to change 
 * the speed or volume of the play operation.
 * It may also receive commands to jump forward or backward in the MDO.
 * <p>
 * <a name="IfBusy"><!-- --></a>
 * If a new play() is attempted while the Player is Active, the result
 * is determined by the value of the <tt>p_IfBusy</tt> parameter.
 * The alternative values for <tt>p_IfBusy</tt> are:<ul>
 * <li><tt>v_Queue</tt>:
 * 	This play is queued (default). It is played 
 * 	when the previous plays have completed.</li>
 * <li><tt>v_Stop</tt>:
 *	The current play and any queued plays are stopped,
 *	(with event qualifier <tt>q_Stop</tt>).
 *	This play begins immediately.</li>
 * <li><tt>v_Fail</tt>:
 *	This play fails, with <tt>(getError() == e_Busy)</tt></li>
 * </ul>
 * <p>
 * <a name="Attributes"><!-- --></a>
 * <h4>Player Attributes</h4>
 * 
 * <h5>Attribute a_Pause</h5>
 *
 * If the attribute <tt>Player.a_Pause</tt> is true, 
 * then the Player supports the pause and resume operations.
 * <ul>
 * <li><tt>rtca_Pause</tt> sets the player state to Paused and
 * {@link PlayerListener#onPause onPause} is invoked if <tt>ev_Pause</tt>
 * is enabled in <tt>p_EnabledEvents</tt>.</li>
 * <li><tt>rtca_Resume</tt> sets the player state to Active,
 * continues to play the stream from it current offset,  and
 * {@link PlayerListener#onResume onResume} is invoked if <tt>ev_Resume</tt>
 * is enabled in <tt>p_EnabledEvents</tt>.</li>
 * </ul>
 * If the value of attribute <tt>a_Pause</tt> is false, then 
 * then the effects of the RTC actions, parameters, and methods
 * discussed in this section are not defined.
 * 
 * <h5>Attribute <tt>a_Speed</tt></h5>
 * 
 * A MDO has a "normal" speed at which its data is to be played.
 * The RTC actions discussed in this section may change the
 * speed up or down. The playback speed achieved by these actions is
 * called the adjusted speed.
 * <p>
 * The units by which speed may be adjusted are
 * percentage change from the normal speed.
 * <p>
 * If the attribute <tt>Player.a_Speed</tt> is true, 
 * then the player supports the following features:
 * <ul><li>
 * The <tt>rtca_SpeedUp</tt> and <tt>rtca_SpeedDown</tt>
 * RTC actions cause the playback speed
 * to be adjusted up or down respectively.
 * The amount of change is governed by the parameter <tt>p_SpeedChange</tt>, 
 * in units of percentage change from the normal speed.
 * </li><li>
 * If the current playback speed is not at the default normal value,
 * the <tt>rtca_ToggleSpeed</tt> RTC action changes the speed to
 * the normal value.
 * If the current playback speed is at the default normal value, the
 * <tt>rtca_ToggleSpeed</tt> RTC action changes the speed to the
 * last adjusted value.
 * </li><li>
 * If a RTC action would result in setting the speed beyond
 * the maximum or minimum allowed values, the command/action is ignored.
 * If a RTC action would result in no change to the playback
 * speed (e.g., changing speed to normal when the speed is already normal,
 * toggling the speed when no adjustment had been made to the speed),
 * the action is ignored.
 * </li><li>
 * The event <tt>ev_Speed</tt> is delivered to 
 * <tt>PlayerListener.onSpeedChange</tt>. This event may be enabled 
 * or disabled by the <tt>p_EnabledEvents</tt> parameter.
 * </ul>
 * If the value of attribute <tt>a_Speed</tt> is false, then 
 * then the effects of the RTC actions, parameters, and methods
 * discussed in this section are not defined.
 * 
 * <h5>Attribute a_Volume</h5>
 *
 * A MDO has a "normal" default volume at which its data is to
 * be played. The RTC actions discussed in this section may
 * change the volume up or down. The playback volume achieved by these
 * actions is called the adjusted volume.
 * Local regulatory restrictions may limit the levels to which volume may
 * be adjusted. It is the responsibility for the client application to
 * ensure that it does not adjust the volume so as to exceed those levels. 
 * <p>
 * The units by which volume may be adjusted are measured in dB from the
 * normal volume.
 * <p>
 * If the value of attribute <tt>a_Volume</tt> is <tt>true</tt>
 * then the Player supports the following features:
 * <ul><li>
 * The <tt>rtca_VolumeUp</tt> and <tt>rtca_VolumeDown</tt> 
 * RTC actions cause the playback volume to be adjusted one quantum
 * up or down respectively.
 * The size of the quantum is governed by the parameter 
 * <tt>p_VolumeChange</tt>, measured in dB from the normal volume.
 * </li><li>
 * If the current playback volume is not at the default normal value, the
 * <tt>rtca_ToggleVolume</tt> RTC action changes the volume to the 
 * normal value.
 * If the current playback volume is at the default normal value, the
 * <tt>rtca_ToggleVolume</tt> RTC action changes the volume to the
 * last adjusted value.
 * </li><li>
 * The event <tt>ev_Volume</tt> is delivered to 
 * <tt>PlayerListener.onVolumeChange</tt>. This event
 * may be enabled or disabled by the <tt>p_EnabledEvents</tt> parameter.
 * </li></ul>
 * 
 * If an RTC action would result in no change to the playback
 * volume (e.g., changing volume to normal when the volume is already normal,
 * toggling the volume when no adjustment had been made to the volume),
 * then the command/action is ignored.
 * Likewise, when the volume is already at its maximum or minimum value,
 * RTC actions that attempt to set it beyond the limit are ignored.
 * <p>
 * If the value of attribute <tt>a_Volume</tt> is false, 
 * then the effects of the RTC actions, parameters, and methods
 * discussed in this section are not defined.
 *
 * <h5> Attribute a_Jump</h5>
 * A Player has a "current location" within a MDO when in the
 * Active or Paused state. The location and the granularity of 
 * changing that location depends on the coder type; being determined
 * by the sample rate and whether the coder is "stateful".
 * For simple Pulse Code Modulation (PCM) coders the location
 * is reported and changed in millisecond units (rounded to the sample rate).
 * <p>
 * If a player resource supports <tt>a_Jump</tt> attribute, then
 * the following features are supported:
 * <ul><li>
 * The <tt>rtca_JumpForward</tt> and <tt>rtca_JumpBackward</tt> RTC actions
 * change the current location one quantum forward or backward respectively.
 * The quantum is set by the parameter <tt>p_JumpTime</tt>.
 * </li><li>
 * The <tt>rtca_JumpStartMDO</tt> and <tt>rtca_JumpEndMDO</tt> RTC actions
 * change the current location to the start or the end of the current MDO.
 * </li><li>
 * The <tt>rtca_JumpForwardMDOs</tt> and <tt>rtca_JumpBackwardMDOs</tt> RTC
 * actions change the current location to the beginning of the MDO that is n
 * MDOs ahead or behind the current MDO in the play list respectively. The
 * value n is set by the parameter <tt>p_JumpMDOIncrement</tt>.
 * </li><li>
 * The <tt>rtca_JumpStartMDOList</tt> and <tt>rtca_JumpEndMDOList</tt> RTC
 * actions change the current location to the beginning or end respectively
 * of the list of MDOs being played.
 * </li></ul>
 * <p>
 * A Jump method or RTC action that goes past the end of the current MDO
 * continues into the next MDO in the MDO list if the
 * coders for the MDOs support the same time increment. If the next
 * MDO in sequence has a different coder type, the current location
 * is set to the beginning of that MDO.
 * <p>
 * If the value of attribute <tt>a_Jump</tt> is false, 
 * then the effects of the RTC actions, parameters, and methods
 * discussed in this section are not defined.  
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface Player extends Resource, PlayerConstants {
    /**
     * Play a sequence of MDOs (Media Data Objects) identified
     * by the streamIDs. This sequence of MDOs is called the "MDO list".
     * <p>
     * This method returns when the MDO list is completely played, or
     * when the playing has been stopped. 
     * (Player Pause does not cause this method to return.)  
     * Play returns a PlayerEvent.
     * The condition that caused the play to stop is included in the
     * completion event. RTC actions can alter the course of the play
     * without the intervention of the application. 
     * Optional arguments can alter the characteristics of the play.
     * <p>
     * Exceptions are thrown if pre- or post- conditions are not satisfied
     * <p><b>Pre-conditions:</b><ol>
     * <li>a Player resource is configured</li>
     * <li>all MDOs are playable</li>
     * </ol>
     * <p><b>Post-conditions:</b> <ol>
     * <li>As indicated by <tt>getQualifier()</tt>:<ul>
     *     <li><tt>q_EndOfData</tt>: All MDOs named in this MDO list 
     *	       have been played.</li>
     *     <li><tt>q_Stop</tt>: play was stopped by the 
     *         <tt>stop()</tt> method 
     *         or because (p_IfBusy=v_Stop)</li>
     *     <li><tt>q_RTC</tt>: play was stopped by 
     *         <tt>Player.rtca_Stop</tt></li>
     *    </ul>
     * </ol>
     * <p>
     *
     * @param streamIDs a String[] naming the MDO list to be played.
     * @param offset number of milliseconds into the MDO list at 
     * which play is to start, offset may span several items in streamIDs.
     * @param rtc Array of RTC that effect this play.
     * @param optargs a Dictionary of optional arguments.
     * @return a PlayerEvent when the operation is complete (or queued).
     * @throws MediaResourceException if requested operation fails.
     * @throws DisconnectedException if Terminal is disconnected.
     */
    PlayerEvent play(String[] streamIDs,
		     int offset, 
		     RTC[] rtc , 
		     Dictionary optargs)
	throws MediaResourceException;

    /**
     * Play a single MDO (Media Data Object) named by streamID.
     * Equivalent to <tt>play()</tt> with a String[] of length one,
     * containing the given streamID.
     *
     * @param streamID a String naming the MDO to be played.
     * @param offset int number milliseconds into the MDO 
     * at which play is to start.
     * @param rtc Array of RTC that effect this play.
     * @param optargs a Dictionary of optional arguments.
     * @return a PlayerEvent when the operation is complete (or queued).
     * @throws MediaResourceException if requested operation fails.
     * @throws DisconnectedException if Terminal is disconnected.
     */
    PlayerEvent play(String streamID,
		     int offset, 
		     RTC[] rtc , 
		     Dictionary optargs) 
	throws MediaResourceException;
    
    /**
     * Adjusts the speed of the current play function. 
     * <p>
     * Parameter <tt>adjustment</tt> specifies the type of 
     * speed adjustment desired.
     * <ul><li>
     * <tt>rtca_SpeedUp</tt> - speed up by <tt>p_SpeedChange</tt>
     * </li><li>
     * <tt>rtca_SpeedDown</tt> - slow down by <tt>p_SpeedChange</tt>
     * </li><li>
     * <tt>rtca_ToggleSpeed</tt> - toggle between normal and adjusted speed.
     * </li><li>
     * <tt>rtca_NormalSpeed</tt> - set to normal speed
     * </li></ul>
     * <p>
     * This is functionally equivalent to the RTC actions.
     * <p>
     * <b>Note:</b> the value of <tt>p_SpeedChange</tt>
     * is in units of percent deviation from normal.
     * <p>
     * <b>Note:</b> this is functionally equivalent to the RTC actions,
     * except that it generates a Completion Event 
     * instead of a non-transactional Event.
     * 
     * @param adjustment an RTC Symbol specifying the type of change.
     * @throws MediaResourceException if speed cannot be adjusted
     * @return a PlayerEvent when the operation completes
     */
    PlayerEvent adjustPlayerSpeed(Symbol adjustment) throws MediaResourceException;

    /**
     * Adjusts the volume the current play function. 
     * <p>
     * Parameter <tt>adjustment</tt> specifies the type of 
     * volume adjustment desired.
     * <ul><li>
     * <tt>rtca_VolumeUp</tt> - volume up by <tt>p_VolumeChange</tt>
     * </li><li>
     * <tt>rtca_VolumeDown</tt> - volume down by <tt>p_VolumeChange</tt>
     * </li><li>
     * <tt>rtca_ToggleVolume</tt> - toggle between normal and adjusted volume
     * </li><li>
     * <tt>rtca_NormalVolume</tt> - set to normal volume
     * </li></ul>
     * <p>
     * This is functionally equivalent to the RTC actions.
     * <p>
     * <b>Note:</b> the value of <tt>p_VolumeChange</tt>
     * in in units of dB from the normal volume.
     * <p>
     * <b>Note:</b> this is functionally equivalent to the RTC actions,
     * except that it generates a Completion Event 
     * instead of a non-transactional Event.
     *
     * @param adjustment an RTC Symbol specifying the type of change.
     * @throws MediaResourceException if volume cannot be adjusted
     * @return a PlayerEvent when the operation completes
     */
    PlayerEvent adjustPlayerVolume(Symbol adjustment) throws MediaResourceException;

    /**
     * Jump forward or backward within a sequence of MDOs during a play.
     * <p>
     * Parameter <tt>jump</tt> specifies the type of jump desired.
     * It is one of the <tt>rtca_Jump&lt;Type&gt;&lt;Units&gt;</tt> Symbols.
     * <p>
     * For example: <tt>rtca_JumpForwardTime</tt>.
     * <p>
     * This is functionally equivalent to the Jump RTC actions,
     * except that it generates a Completion Event 
     * instead of a non-transactional Event.
     * @param jump a Symbol specifying the type of jump.
     * @throws MediaResourceException if the jump cannot be done
     * @return a PlayerEvent when the operation completes
     */
    PlayerEvent jumpPlayer(Symbol jump) throws MediaResourceException;

    /**
     * Pause the current play.
     * <p>
     * This is functionally equivalent to <tt>rtca_Pause</tt>,
     * except that it generates a Completion Event 
     * instead of a non-transactional Event.
     * @throws MediaResourceException if pause cannot be done
     * @return a PlayerEvent when the operation completes
     */
    PlayerEvent pausePlayer() throws MediaResourceException;

    /**
     * Resume the current play.
     * <p>
     * This is functionally equivalent to <tt>rtca_Resume</tt>,
     * except that it generates a Completion Event 
     * instead of a non-transactional Event.
     * @throws MediaResourceException if resume cannot be done
     * @return a PlayerEvent when the operation completes
     */
    PlayerEvent resumePlayer() throws MediaResourceException;

    /**
     * Stop the current play.
     * <p>
     * This is functionally equivalent to <tt>rtca_Stop</tt>,
     * except that it generates a Completion Event 
     * instead of a non-transactional Event.
     * @throws MediaResourceException if stop cannot be done
     * @return a PlayerEvent when the operation completes
     */
    PlayerEvent stopPlayer() throws MediaResourceException;
}
