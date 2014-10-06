/*
 * MessagingProvider.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/** Provider interface to get a MessagingService. 
 * <p>
 * If your <tt>Provider</tt> or <tt>MediaProvider</tt> implements this interface,
 * then you can get a <tt>MessagingService</tt> and use a <tt>NamedService</tt>.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface MessagingProvider {
    /** Get the MessagingService for this MessagingProvider.
     * @return the MessagingService associated with this MessagingProvider
     */
    public MessagingService getMessagingService();
}
