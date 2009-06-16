/*
 * ContainerEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * Defines methods for ContainerService events.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
interface ContainerEvent extends MediaEvent, ContainerConstants {
    /** 
     * Wrap getSource() and cast to ContainerService. 
     * @return the ContainerService that originated this event.
     */
    ContainerService getContainerService();

    /**
     * Get the Symbol that identifies this event.
     * <P>
     * For completion events, this identifies the operation that has completed.
     * For synchronous code this is mostly redundant, but it may be useful
     * for asynchronous Listeners. Details about how/why the operation
     * completed is available using {@link #getError()}.
     * <p>
     * The Symbols returned from by this method will have names of the form:
     * <code>ev_Operation</code>.
     * 
     * @return the Symbol that identifies this event
     */
    public Symbol getEventID();

    /**
     * Identify the reason or cause of an error or failure.
     * <p>
     * If this ContainerEvent <b>is</b> associated with an Exception,
     * then <code>getError()</code> returns a Symbol that identifies 
     * the problem.
     * <br>If this ContainerEvent <b>is not</b> associated with an Exception,
     * then <code>getError()</code> returns the Symbol <code>e_OK</code>.
     * <p>
     * A list of error Symbols is defined in interface {@link ErrSym}.
     * @return one of the error Symbols. 
     */
    public Symbol getError();    

    /** for getContainerInfo.
     * @return a Dictionary of container info
     */
    public Dictionary getInfoDict();

    /** for getParameters.
     * @return a Dictionary of container parameters
     */
    public Dictionary getParamDict();

    /** for listObjects.
     * @return a String[] of MDOs in the container
     */
    public String[] getObjectList();

    /** from openDataObject.
     * @return the DataObject that was opened.
     */
    public DataObject getDataObject();
}
