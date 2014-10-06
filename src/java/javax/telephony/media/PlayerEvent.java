/*
 * PlayerEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines the methods for Player Events.
 * May be the completion of a Player Resouce operation,
 * or may signal the occurence of a non-transaction event.
 * 
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface PlayerEvent extends ResourceEvent, PlayerConstants {

    /**
     * Return index in to MDO List, indicating which MDO was stopped.
     *
     * @return the int index into the MDO List, 
     * indicating which MDO was stopped, paused, etc.
     * 
     */
    public int getIndex();
    
    /**
     * Return index into a MDO where play stopped.
     * Typically this is milliseconds into an audio MDO.
     *
     * @return the int index into a MDO phrase where play stopped, paused, etc.
     */
    public int getOffset();

    /**
     * Returns the RTC action Symbol that caused this change in the Player.
     * <p>
     * The return value is non-null only for non-transactional events
     * as delivered to onSpeedChange(), onVolumeChange(), onJump().
     * The return value indicates the type of change in Speed, Volume or Position.
     * <p>
     * Valid return values are the RTC action Symbols:
     * <table border="1" cellpadding="3">
     * <tr><td>
     * rtca_SpeedUp</td><td>rtca_SpeedDown</td><td>rtca_ToggleSpeed</td><td>rtca_NormalSpeed
     * </td></tr><tr><td>
     * rtca_VolumeUp</td><td>rtca_VolumeDown</td><td>rtca_ToggleVolume</td><td>rtca_NormalVolume
     * </td></tr><tr><td>
     * rtca_JumpForwardTime</td><td>rtca_JumpBackwardTime
     * </td><td>&nbsp;</td><td>&nbsp;
     * </td></tr><tr><td>
     * rtca_JumpForwardMDOs</td><td>rtca_JumpBackwardMDOs
     * </td><td>
     * rtca_JumpStartMDO</td><td>rtca_JumpEndMDO
     * </td></tr><tr><td>&nbsp;
     * </td><td>&nbsp;</td><td>
     * rtca_JumpStartMDOList</td><td>rtca_JumpEndMDOList
     * </td></tr><tr><td>
     * rtca_JumpForwardSentences</td><td>rtca_JumpBackwardSentences
     * </td><td>
     * rtca_JumpStartSentence</td><td>rtca_JumpEndSentence
     * </td></tr><tr><td>
     * rtca_JumpForwardWords</td><td>rtca_JumpBackwardWords 
     * </td><td>&nbsp;</td><td>&nbsp;
     * </td></tr>
     * </table>
     *
     * @return one of the <tt>rtca_</tt> Symbols.
     *
     * @see PlayerConstants#rtca_SpeedDown
     */
    public Symbol getChangeType();
}
