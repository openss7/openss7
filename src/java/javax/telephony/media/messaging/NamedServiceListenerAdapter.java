/*
 * NamedServiceListenerAdapter.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/** 
 * Minimal implementation of NamedServiceListener.
 * <p>
 * <b>Note:</b> the implementation of <tt>onServiceRequest()</tt>
 * invokes <tt>reply</tt> on the given event, returning the
 * error/exception <tt>e_NotSupported</tt> to the invoking application.
 * To handle service requests, you <b>must</b> override this method.
 * The Service Request protocol expects/demands that <tt>reply</tt>
 * will be invoked on the given event.
 * <p>
 * The other methods of this implementation do nothing and simply return.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public class NamedServiceListenerAdapter implements NamedServiceListener {
    /** a Transactional Request has arrived. 
     * <p>
     * <b>Note:</b> if a ServiceRequestEvent is received,
     * <tt>event.reply(...)</tt> <i>must</i> be invoked at some point.
     * Failure to reply may confuse or break the 
     * process that sent the request.
     * <p>
     * <b>Note:</b> this Adapter by default sends a reply
     * with an Exception, <tt>e_NotSupported</tt>, <tt>q_Standard</tt>,
     * and a <tt>null</tt> payload.
     * @see ServiceRequestEvent#reply
     */
    public void onServiceRequest(ServiceRequestEvent event) {
	try {
	    event.reply(event.q_Standard, event.e_NotSupported, 0, true, null);
	} catch (ServiceException ex) {
	    // ignore exception!
	}
    }

    // javadoc should copy from NamedServiceListener:
    public void onServiceEvent(ServiceEvent event) {}

    // javadoc should copy from NamedServiceListener:
    public void onServiceOpened(ServiceEvent event) {}

    // javadoc should copy from NamedServiceListener:
    public void onServiceClosed(ServiceEvent event) {}
}
