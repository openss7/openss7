/*
 * MediaTerminal.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import javax.telephony.Terminal;

/** 
 * Interface to mark Terminals with streams that can be connected 
 * to a MediaGroup.
 * These Terminals are suitable for use with 
 * {@link MediaService#bindToTerminal(ConfigSpec, Terminal)}.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface MediaTerminal extends Terminal {
    /*
     * TO DO: embed this as showable text using DHTML!
     *
     * Design note:<br>
     * <p>
     * A MediaTerminal is a Terminal with a S.100 CCR or TAPI-3 
     * media streams and some method to connect to a MediaGroup.
     * [or in TAPI-3 some way for the MSP to connect them to T3 "Terminals"]
     * <p>
     * That is: there is some implementation like a S.100 Server
     * (or a TAPI-3 MSP) that can process a ConfigSpec and deliver
     * an object that implements the requested Resource interfaces.
     * In S.100/S.400 that means delivering a MediaGroup (of Resources).
     * In TAPI-3 that would mean delivering a collection of TAPI-3 "Terminals"
     * <p>
     * The methods that distinguish a MediaTerminal from a Terminal
     * are accessible only to the implementation of the MediaService
     * bind methods, MediaService.configure(), and MediaService.release().
     * <p>
     * Logically, those methods constitute part of the JTAPI Media SPI 
     * in the <tt>javax.telephony.media.provider</tt> package.
     */
}
