/*
 * Async_TTSPlayer.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Non-blocking methods for the TTS Player resource.
 * When complete, these Async_PlayerEvents are delivered
 * to Async_TTSPlayerListener.onTTSDictionaryDone().
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface Async_TTSPlayer extends Async_Player {
    /**
     * Non-blocking version of loadTTSDictionary(); 
     * <p>
     * Load a collection of TTS dictionaries into a Player.
     * The dictionary is placed at the top of the dictionary stack,
     * and is deactivated.
     *
     * @param dictNames a String[] of DataObject fullNames.
     * @param optargs a Dictionary of optional args.
     * @return a Async_PlayerEvent
     */
    Async_PlayerEvent async_loadTTSDictionary(String[] dictNames, Dictionary optargs);

    /**
     * Non-blocking version of activateTTSDictionary(); 
     * <p>
     * Activates a collection of TTS dictionaries.
     * The dictionaries must already be loaded.
     *
     * @param dictNames a String[] of DataObject fullNames.
     * @param optargs a Dictionary of optional args.
     * @return a Async_PlayerEvent
     */
    Async_PlayerEvent async_activateTTSDictionary(String[] dictNames, Dictionary optargs);

    /**
     * Non-blocking version of deactivateTTSDictionary(); 
     * <p>
     * Deactivates a collection of loaded TTS dictionaries.
     * 
     * @param dictNames a String[] of DataObject fullNames.
     * @param optargs a Dictionary of optional args.
     * @return a Async_PlayerEvent
     */
    Async_PlayerEvent async_deactivateTTSDictionary(String dictNames[], Dictionary optargs);

    /**
     * Non-blocking version of unloadTTSDictionary(); 
     * <p>
     * Unloads a collection of TTS dictionaries from a Player.
     * @param dictNames a String[] of DataObject fullNames.
     * @param optargs a Dictionary of optional args.
     * @return a Async_PlayerEvent
     */
    Async_PlayerEvent async_unloadTTSDictionary(String dictNames[], Dictionary optargs);
}
