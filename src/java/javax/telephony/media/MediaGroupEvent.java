/*
 * MediaGroupEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
import java.util.Dictionary;
import javax.telephony.media.async.*; // for javadoc

/**
 * A MediaEvent from the MediaGroup.
 * <p>
 * These are not visible in the synchronous API,
 * but are delivered to an Async.MediaGroupListener.
 * <p>
 * <b>Note:</b> if a field accessor is called on an Event
 * of the wrong type (that is, with the wrong EventID)
 * the accessor returns <tt>null</tt>.
 * @see Async.MediaGroupListener
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public interface MediaGroupEvent extends MediaServiceEvent {
    /**
     * Return a Terminal from getTerminal().
     * Valid when <tt>(getEventID.equals(ev_GetTerminal))</tt>.
     * @return a Terminal from <tt>getTerminal()</tt>
     */
    Terminal getTerminal();
    /**
     * Return a terminal name String from getTerminalName().
     * Valid when <tt>(getEventID.equals(ev_GetTerminalName))</tt>.
     * @return a terminal name String from <tt>getTerminalName()</tt>
     */
    String getTerminalName();
    /**
     * Return a ConfigSpec from getConfigSpec().
     * Valid when <tt>(getEventID().equals(ev_GetConfigSpec))</tt>.
     * @return a ConfigSpec from getConfigSpec()
     */
    ConfigSpec getConfigSpec();
    /**
     * Return a Dictionary from a MediaGroupEvent.
     * Valid when:
     * <table border="1" cellpadding="3">
     * <tr><td> Method called: </td><td> <tt>getEventID()</tt> equals:</td></tr>
     * <tr><td><tt>getUserValues()</tt></td> <td><tt>ev_GetUserValues</tt></td></tr>
     * <tr><td><tt>getParameters()</tt></td> <td><tt>ev_GetParameters</tt></td></tr>
     * <tr><td><tt>getAttributes()</tt> </td><td><tt>ev_GetAttributes</tt></td></tr>
     * </table>
     * @return a Dictionary from a MediaGroupEvent.
     */
    Dictionary getDictionary();
}
