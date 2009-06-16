/*
 * Async_PlayerListenerAdapter.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/** 
 * Implements all Async_PlayerListener and PlayerListener methods.
 * Each method does nothing.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public class Async_PlayerListenerAdapter 
    implements Async_PlayerListener, PlayerListener {
    /* Non-trans events from PlayerListener. */
    public void onPause(PlayerEvent event) {}
    public void onResume(PlayerEvent event) {}
    public void onJump(PlayerEvent event) {}
    public void onSpeedChange(PlayerEvent event) {}
    public void onVolumeChange(PlayerEvent event) {}
    public void onMarker(PlayerEvent event) {}

    /* Completion events */
    public void onPlayDone(PlayerEvent event) {}

    /* RTC methods */
    public void onStopDone(PlayerEvent event) {}
    public void onPauseDone(PlayerEvent event) {}
    public void onResumeDone(PlayerEvent event) {}
    public void onJumpDone(PlayerEvent event) {}
    public void onAdjustSpeedDone(PlayerEvent event) {}
    public void onAdjustVolumeDone(PlayerEvent event) {}
    // or:
    // public void onCommandDone(PlayerEvent event event) {}
}

