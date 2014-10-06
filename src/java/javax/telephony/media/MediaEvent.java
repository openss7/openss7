/*
 * MediaEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

import javax.telephony.*;		// for javadoc to find Connection
import java.util.Dictionary;

/**
 * Base interface for all Events in the media pacakge.
 * Indicates the completion of some media package operation.
 * Characterized by an EventID representing the event identity.
 * <p> 
 * This interface defines a set of accessors common to all media events.
 * The accessors may return <tt>null</tt> if the value is not relevant
 * for the particular event being queried.
 * <p>
 * <b>Note:</b>
 * Classes that <i>implement</i> this interface shall follow the standard
 * Java Listener design pattern by extending java.util.EventObject.
 * <p>
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public
interface MediaEvent extends MediaServiceConstants {
    /*
     * <b>Note:</b>
     * Not all MediaEvents come from a MediaService, therefore
     * the method <tt>getMediaService</tt> has been moved to MediaServiceEvent.
     * The deprecated declaration here should be removed in next release.
     */

    // In JTAPI-1.4, moved to MediaServiceEvent;
    // since Container and MessagingService events do not have a MediaService
    /**
     * Get the MediaService associated with the current event.
     * <p>
     * May return <tt>null</tt> if this event is not from a MediaSerivce.  
     * For that reason this method is deprecated, and will be removed
     * in the next release; MediaEvents from a MediaService source
     * are <tt>MediaServiceEvents</tt>, and this method appears there.
     * <p>
     * @return the MediaService which is the source of this event.
     * @ deprecated moved to the subclass MediaServiceEvent
     */
    // MediaService getMediaService();

    /**
     * Get the Symbol that identifies this event.
     * <P>
     * For completion events, this identifies the operation that has completed.
     * For synchronous code this is mostly redundant, but it may be useful
     * for asynchronous Listeners. Further detail about how/why the operation
     * completed is available using {@link ResourceEvent#getQualifier()}.
     * <p>
     * The Symbols returned from by this method have names of the form:
     * <code>ev_Operation</code>.
     * 
     * @return the Symbol that identifies this event
     *
     * @see ResourceEvent
     */
    public Symbol getEventID();

    /**
     * Return the qualifier value for this event.
     * Qualifiers supply additional information
     * about how or why the operation completed.
     * <p>
     * Standard return values are one of:
     * <br><code>q_Standard, q_Duration, q_RTC, q_Stop</code><br>
     * which indicate the reason or mode of completion.
     * Other qualifiers may be documented in the resource event classes.
     * <p>
     * If the associated method or transaction did not succeed,
     * this methods returns <tt>null;</tt> 
     * use {@link #getError()} to determine the cause of failure. 
     * 
     * @return a Symbol that indicates the completion cause 
     * (<tt>q_Something</tt>).
     */
    public Symbol getQualifier();
    
    /**
     * Return a Symbol describing the warning.
     * Return value is non-null if operation completed,
     * but in some unusual or boundary condition.
     * @return a Symbol that identifies the warning.
     */
    public Symbol getWarning();

    /**
     * Return a Symbol describing the error.
     * If there was no error, returns <tt>ErrSym.e_OK</tt>. 
     * @return a Symbol that identifies the error.
     */
    public Symbol getError();

    /**
     * Return the int suberror field for this event.
     * SubError values are defined for some errors
     * and supply additional information about the error.
     * @return the int suberror
     */
    public int getSubError();

    /**
     * Implementation-specific additional event information.
     * For example, this may contain the ECTF S.200
     * payload Dictionary associated with this event.
     * @return a Dictionary of event information.
     */
    public Dictionary getPayload();

    /**
     * Return the Exception associated with this event.
     * <p>
     * If the operation terminated with any kind of exception, 
     * return that Exception.
     * If the operation completed with <tt>statusOK</tt>, 
     * then return <tt>null</tt>.
     * <p>
     * The Exception returned will be one of the Exceptions 
     * declared in the synchronous interface for the request method,
     * or any undeclared RuntimeException.
     * <p>
     * <b>Note:</b> the stacktrace in the Exception may not be useful.
     * The stacktrace may be of the original creator of the Exception
     * (which could be in some other implementation Thread)
     * or may be from the last call to <tt>throwIfSomeException()</tt>.
     * <p>
     * @return any Exception generated by the transaction.
     */
    public Exception getException();
}
