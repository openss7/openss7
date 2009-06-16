/*
 * ContainerConstants.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Constants common to all the Container interfaces.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
interface ContainerConstants extends CoderConstants {
    /**
     * Symbol returned from {@link ResourceEvent#getError()} 
     * if there is no error.
     */
    Symbol e_OK				= ESymbol.Error_OK;

    // ContainerConstants
    /** flag bits for parameter 'flags' for OpenObject, CreateObject. */

    /** Create DataObject if does not already exist. */
    final static int b_Create = 1;

    /** Used with b_Create: fail if already exists. */
    final static int b_Exclusive = 2;

    /** With v_AccessCopy or v_AccessWrite; erase contents when opened. */
    final static int b_Truncate = 4;

    /** With v_AccessCopy or v_AccessWrite; write to end of contents. */
    final static int b_Append = 8;

    /** Allow non-exclusive read access. Other processes may read or write. */
    final static Symbol v_AccessRead = ESymbol.Container_AccessRead;

    /** Allow exclusive write access. */
    final static Symbol v_AccessWrite = ESymbol.Container_AccessWrite;

    /** Allow read and write access.  Other processes may read. */
    final static Symbol v_AccessCopy = ESymbol.Container_AccessCopy;

    /** for <tt>position</tt> argument to DataObject.seek(). */
    final static Symbol v_Begin = ESymbol.Container_Begin;
    /** for <tt>position</tt> argument to DataObject.seek(). */
    final static Symbol v_Current = ESymbol.Container_Current; 
    /** for <tt>position</tt> argument to DataObject.seek(). */
    final static Symbol v_End = ESymbol.Container_End;

    /** for <tt>SeekUnits</tt> argument to DataObject.seek(). */
    final static Symbol v_Bytes = ESymbol.Container_Bytes;
    /** for <tt>SeekUnits</tt> argument to DataObject.seek(). */
    final static Symbol v_MilliSeconds = ESymbol.Container_MilliSeconds;
    /** for <tt>SeekUnits</tt> argument to DataObject.seek(). */
    final static Symbol v_MDO = ESymbol.Container_MDO;

    /** boolean value for <tt>ifNoExist</tt> in openDataObject. */
    final static boolean v_Create = true;
    /** boolean value for <tt>ifNoExist</tt> in openDataObject. */
    final static boolean v_Fail = false;

    /**
     * Set this in openObject OptArgs to restrict Media Type 
     * of an existing DataObject.
     * Valid values: a Symbol[] of Symbols from CoderConstants.
     * @see CoderConstants
     */
    final static Symbol k_MediaTypes = ESymbol.Container_MediaTypes;

    /**
     * Set this in createObject to specify media type for new DataObject.
     * <p>
     * Valid values: one of the CoderConstants.
     * <p>
     * <b>Note:</b>If MediaTypes is specified,
     * then this default must be one of the MediaTypes.
     * @see CoderConstants
     */
    final static Symbol k_MediaType = ESymbol.Container_MediaType;

    /** 
     * Used in optargs to set Parameters during createObject(). 
     * Value is a KVSet. 
     */
    final static Symbol k_InputParameters = ESymbol.Container_InputParameters;
}
