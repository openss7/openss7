/*
 * Async_DataObject.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.container.async;

import javax.telephony.media.*;
import javax.telephony.media.container.*;
import java.util.EventListener;
import java.util.Dictionary;

/** 
 * Non-blocking methods for DataObject.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface Async_DataObject extends DataObject {

    /** Start listening for DataObject event completion. 
     * @param listener an Async_DataObjectListener
     */
    void addListener(EventListener listener);

    /** Stop listening for DataObject event completion. 
     * @param listener an Async_DataObjectListener
     */
    void removeListener(EventListener listener);

    /**
     * Close this DataObject. 
     */
    Async_DataObjectEvent async_close();

    /**
     * Set stream to new position.
     */
    Async_DataObjectEvent async_seek(Symbol position, int size, Symbol seekUnits);

    /** return the bytes read. */
    Async_DataObjectEvent async_read(int nbytes);

    /** write from within a byte[] */
    Async_DataObjectEvent async_write(byte[] bytes, int offset, int len);

    /** write the whole byte[] (bytes, 0, bytes.length) */
    Async_DataObjectEvent async_write(byte[] bytes); 
}
