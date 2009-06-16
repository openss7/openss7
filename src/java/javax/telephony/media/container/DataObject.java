/*
 * DataObject.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/** An open Media Data Object,
 * as returned by <tt>ContainerService.openDataObject()</tt>.
 * <p>
 * An open DataObject can be used to read/write data.
 * These are <b>not</b> used directly for play() and record().
 * Data written using <tt>DataObject.write()</tt> can be played
 * by passing the String <b>name</b> of the MDO to <tt>play()</tt>.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface DataObject extends ContainerConstants {

    /** Return the ContainerService for this DataObject.
     * @return the ContainerService for this DataObject.
     */
    ContainerService getContainerService();

    /** The assigned coder type for an existing DataObject.
     * @return the assigned coder type for the DataObject.
     */
    Symbol getMediaType();

    /** close this DataObject.
     * @throws ContainerException if close fails
     */
    void close() throws ContainerException;

    /** set stream to new position.
     * <p>
     * First move to the indicated <tt>position</tt>,
     * one of <tt>v_Begin</tt>, <tt>v_Current</tt>, <tt>v_End</tt>.
     * Then translate <tt>offset</tt> and <tt>seekUnits</tt>
     * and skip the appropriate number of bytes forward or backward from there.
     * <p>
     * If <tt>offset</tt> is negative, then seek backward
     * from <tt>position</tt> toward beginning of MDO.
     * <p>
     * Returns the number of bytes actually skipped (from the position).
     * [althought for some <tt>seekUnits</tt>, that may not be very meaningful]
     * 
     * @param position one of the Symbols <tt>v_Begin</tt>, <tt>v_End</tt>, <tt>v_Current</tt>.
     * @param offset int number of units to move from position.
     * @param seekUnits one of the Symbols <tt>v_Bytes</tt>, <tt>v_MilliSeconds</tt>, <tt>v_MDO</tt>
     * @throws ContainerException if seek fails
     * @return int number of bytes moved by the seek after going to position.
     */
    int seek(Symbol position, int offset, Symbol seekUnits) throws ContainerException;

    /** read bytes from this DataObject and return them.
     * @param nbytes the int number of bytes to read
     * @return an array of the bytes read
     * @throws ContainerException if the bytes are not read
     */
    byte[] read(int nbytes) throws ContainerException;

    /** write from within a byte[].
     * @param bytes a byte array containing the bytes to write
     * @param offset int index of the first byte to write
     * @param len int number of bytes to write
     * @throws ContainerException if the bytes are not written
     */
    void write(byte[] bytes, int offset, int len) throws ContainerException;

    /** write the whole byte[].
     * Equivalent to <tt>write(bytes, 0, bytes.length)</tt>
     * @param bytes a byte array containing the bytes to write
     * @throws ContainerException if the bytes are not written
     */
    void write(byte[] bytes) throws ContainerException; 
}
