/*
 * RecorderListener.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines the callback methods for non-transactional Recorder Events. 
 * <p>
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public 
interface RecorderListener extends ResourceListener {
    /** Indicates that an RTC has caused the recorder to pause.
     *
     * @param ev  a RecorderEvent object
     */
    public void onPause(RecorderEvent ev);
	
    /** Indicates that an RTC has caused the recorder to resume. 
     *
     * @param ev  a RecorderEvent object
     */
    public void onResume(RecorderEvent ev);
}
