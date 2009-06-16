/*
 * ServiceRequestEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.messaging;

import javax.telephony.media.*;
import java.util.Dictionary;


/**
 * A ServiceEvent representing a Request from a NamedService.
 * <p>
 * ServiceRequestEvent events are delivered to a NamedServiceListener.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface ServiceRequestEvent extends ServiceEvent {

    /** Reply to sender.
     * <p>
     * Supply <tt>Message_</tt> components directly. 
     * 
     * @param qualifier a qualifier Symbol (default <tt>q_Standard</tt>)
     * @param error an Error Symbol (if no error, use <tt>Error_OK</tt>)
     * @param suberror an int (use <tt>Symbol.hashCode()</tt> if necessary)
     * @param isException boolean <tt>true</tt> only if <tt>error</tt>
     * indicates an Exception
     * @param payload the EventData to be sent.
     * @throws ServiceClosedException if the service is closed.
     */
    public void reply(Symbol qualifier, Symbol error, int suberror, 
		      boolean isException, Dictionary payload)
	throws ServiceException;

    /**
     * Reply to sender.
     * <p>
     * The reply message is constructed using fields extracted
     * from the <tt>message</tt> Dictionary:
     * <table border="1" cellpadding="3">
     * <tr><td>
     * Expected Type	</td><td>ESymbol Key 	  </td><td>Default Value</td>
     * </tr><tr><td>
     * Symbol 		</td><td>Message_Qualifier</td><td>Any_Standard</td>
     * </tr><tr><td>
     * Symbol/Integer	</td><td>Message_Error	  </td><td>Any_Null/0</td>
     * </tr><tr><td>
     * Symbol/Integer 	</td><td>Message_SubError </td><td>Any_Null/0</td>
     * </tr><tr><td>
     * Boolean		</td><td>Message_Status	  </td><td>Boolean.FALSE</td>
     * </tr><tr><td>
     * Dictionary 	</td><td>Message_EventData</td><td>message</td>
     * </tr></table>
     * <p>
     * <p><b>Note:</b> if  <tt>Message_Error</tt> is not set, 
     * or is <tt>Error_OK</tt>, or <tt>Integer(0)</tt> or <tt>Any_Null</tt>
     * (the equivalent Symbol value for zero),
     * then the error field is set to 0 
     * (reported in S.410 as <tt>Error_OK</tt>)
     * and status is set to <tt>statusOK</tt> 
     * (<tt>Message_Status</tt> is not consulted).
     *
     * <p><b>Note:</b> <tt>Message_Status</tt> indicates 
     * whether the Error denotes an Exception. 
     * The status value sent in the message is computed as
     * <pre>((Error is e_OK|null) ? Succeed : (Status ? Fail : Warn))</pre>
     * 
     * <p><b>Note:</b> if <tt>Message_EventData</tt> is not specified, 
     * then the given <tt>message</tt> Dictionary
     * (including the <tt>Message_</tt> keys)
     * is used as <tt>EventData</tt>, and reported in S.410 using getPayload().
     * 
     * @param message a Dictionary of event message components.
     * @throws ServiceClosedException if the service is closed.
     */
    public void reply(Dictionary message) throws ServiceException;
}

