/*
 * Async_Player.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.async;

import javax.telephony.media.*;
import java.util.Dictionary;

/**
 * Non-blocking methods for the Player resource.
 * When complete, the Async_PlayerEvents are delivered
 * to Async_PlayerListener.onPlayDone(). 
 * <!-- or Async_PlayerListener.onRTCActionDone() -->
 * or PlayerListener.on&lt;RTCaction&gt;Done()
 * 
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface Async_Player extends PlayerConstants {
    /**
     * Non-blocking version of Play(); 
     * Play an array of MDOs named by Strings.
     * <p>
     * Completion event is sent to Async_PlayerListener.
     *
     * @param streamIDs a String[] naming the MDO list to be played.
     * @param offset number of milliseconds into the MDO list at 
     * which play is to start, offset may span several items in streamIDs.
     * @param rtc Array of RTC that effect this play.
     * @param optargs a Dictionary of optional arguments.
     * @return an Async_PlayerEvent
     * 
     * @see Player#play(String[], int, RTC[], Dictionary)
     */
    Async_PlayerEvent async_play(String[] streamIDs,
				 int offset, 
				 RTC[] rtc,  
				 Dictionary optargs);

    /**
     * Play a single MDO (Media Data Object) named by streamID.
     * Equivalent to <code>async_play()</code> with a String[] 
     * of length one, containing the given streamID.
     *
     * @param streamID a String naming the MDO to be played.
     * @param offset int number milliseconds into the MDO 
     * at which play is to start.
     * @param rtc Array of RTC that effect this play.
     * @param optargs a Dictionary of optional arguments.
     * @return a Player.Event when the operation is complete (or queued).
     * 
     * @see Player#play(String, int, RTC[], Dictionary)
     */
    Async_PlayerEvent async_play(String streamID,
				 int offset, 
				 RTC[] rtc,  
				 Dictionary optargs);

    /** Non-blocking version of Player.adjustPlayerSpeed.
     * @param adjustment  a Symbol specifying the type of adjustment
     * @return Async_PlayerEvent
     * @see Player#adjustPlayerSpeed
     */
    Async_PlayerEvent async_adjustPlayerSpeed(Symbol adjustment);
    /** Non-blocking version of Player.adjustPlayerVolume.
     * @param adjustment  a Symbol specifying the type of adjustment
     * @return Async_PlayerEvent
     * @see Player#adjustPlayerVolume
     */
    Async_PlayerEvent async_adjustPlayerVolume(Symbol adjustment);
    /** Non-blocking version of Player.jumpPlayer.
     * @param jump  a Symbol specifying the type of jump
     * @return Async_PlayerEvent
     * @see Player#jumpPlayer
     */
    Async_PlayerEvent async_jumpPlayer(Symbol jump);
    /** Non-blocking version of Player.pausePlayer.
     * @return Async_PlayerEvent
     * @see Player#pausePlayer
     */
    Async_PlayerEvent async_pausePlayer();
    /** Non-blocking version of Player.resumePlayer.
     * @return Async_PlayerEvent
     * @see Player#resumePlayer
     */
    Async_PlayerEvent async_resumePlayer();
    /** Non-blocking version of Player.stopPlayer.
     * @return Async_PlayerEvent
     * @see Player#stopPlayer
     */
    Async_PlayerEvent async_stopPlayer();
}

