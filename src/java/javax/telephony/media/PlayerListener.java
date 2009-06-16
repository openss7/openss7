/*
 * PlayerListener.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines the callback methods for non-transactional PlayerEvents. 
 * <p>
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface PlayerListener extends ResourceListener {
    /** 
     * Indicates that an RTC has caused play to pause.
     * @param event a non-transactional PlayerEvent.
     */
    public void onPause(PlayerEvent event);
	
    /** 
     * Indicates that an RTC has caused play to resume.
     * @param event a non-transactional PlayerEvent.
     */
    public void onResume(PlayerEvent event);
	
    /** 
     * Indicates that an RTC has caused a jump.
     * @param event a non-transactional PlayerEvent.
     */
    public void onJump(PlayerEvent event);

    /** 
     * Indicates that an RTC has caused a speed change.
     * @param event a non-transactional PlayerEvent.
     * @see PlayerEvent#getChangeType()
     */
    public void onSpeedChange(PlayerEvent event);
    /** 
     * Indicates that an RTC has caused a volume change.
     * @param event a non-transactional PlayerEvent.
     * @see PlayerEvent#getChangeType()
     */
    public void onVolumeChange(PlayerEvent event);
	
    /** 
     * Indicates that a TTS Marker has been played.
     * @param event a non-transactional PlayerEvent.
     */
    public void onMarker(PlayerEvent event);
	
}
