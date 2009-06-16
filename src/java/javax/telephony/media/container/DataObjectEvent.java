/*
 * DataObjectEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.container;

import javax.telephony.media.*;
import java.util.Dictionary;

/**
 * Defines methods for DataObject events.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
interface DataObjectEvent extends ContainerEvent {
    /**
     * Wrap getSource() and cast to a DataObject.
     * @return the DataObject that originated this event.
     */
    DataObject getDataObject();

    /** 
     * Return the ContainerService associated with this event.
     * Shorthand for getDataObject().getContainerService();
     */
    ContainerService getContainerService();

    /**
     * Total number of bytes moved during seek. 
     * Relative to the seek position.
     * @return the number of bytes moved during seek. 
     */
    int getSeekSize();

    /** Array of bytes read from Data Object.
     * @return the array of bytes read
     */
    byte[] getBytesRead();

    /**
     * Number of bytes actually written to Data Object.
     * If less than requested, then an error is indicated
     * and this event should contain an Exception.
     * @return the number of bytes written
     */
    int getBytesWritten();
}
