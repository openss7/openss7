/*
 * Async_PlayerListener.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines the callback methods for Async_Player transactional Events. 
 * <p>
 * For these methods, the eventID will correspond to the method name.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
interface Async_PlayerListener extends MediaListener {
    /** Invoked when async_play completes. 
     * <p>
     * @param event a PlayerEvent that isDone();
     */
    void onPlayDone(PlayerEvent event);

    /** Invoked when async_stop completes. 
     * <p>
     * @param event a PlayerEvent that isDone();
     */
    void onStopDone(PlayerEvent event);

    /** Invoked when async_pause completes. 
     * <p>
     * @param event a PlayerEvent that isDone();
     */
    void onPauseDone(PlayerEvent event);

    /** Invoked when async_resume completes. 
     * <p>
     * @param event a PlayerEvent that isDone();
     */
    void onResumeDone(PlayerEvent event);

    /** Invoked when async_jump completes. 
     * <p>
     * @param event a PlayerEvent that isDone();
     */
    void onJumpDone(PlayerEvent event);

    /** Invoked when async_adjustspeed completes. 
     * <p>
     * @param event a PlayerEvent that isDone();
     */
    void onAdjustSpeedDone(PlayerEvent event);

    /** Invoked when async_adjustvolume completes. 
     * <p>
     * @param event a PlayerEvent that isDone();
     */
    void onAdjustVolumeDone(PlayerEvent event);


    // or:
    /**
     * Invoked when a transactional command completes.
     * <p> 
     * Provoked by any of: pausePlayer, resumePlayer, stopPlayer,
     * adjustPlayerSpeed, adjustPlayerVolume, jumpPlayer.
     *
     * @param event a PlayerEvent that isDone();
     */
    // void onCommandDone(PlayerEvent event);

}
