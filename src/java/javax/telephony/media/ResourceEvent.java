/*
 * ResourceEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

/**
 * The simple, synchronous interface to Resource Events.
 * ResourceEvent objects are returned from Resource methods
 * and non-tranactional ResourceEvents are delivered to a 
 * ResourceListener.
 * <p>
 * When a media resource method (for a Player, Recorder, etc.) completes 
 * successfully, it returns a ResourceEvent to the invoking program.
 * If the method does not complete normally (ie, if the post-conditions
 * are not satisfied), then the method throws a
 * {@link MediaResourceException}
 * that contains a ResourceEvent (accessible using getResourceEvent()).
 * In either case, the ResourceEvent can be queried for additional 
 * information.
 *
 * <h4>ResourceEvent properties:</h4>
 *
 * Each ResourceEvent has the MediaEvent properties 
 * (EventID, Qualifier or Error and SubError), plus an RTCTrigger.
 * ResourceEvent defines <i>get</i> method for RTCTrigger.
 * <p>
 * If a media resource generates events that have additional fields,
 * then the Event interface for that resource extends ResourceEvent
 * by defining methods to access the additional information.
 * 
 * <h4>Names for Symbol constants used in ResourceEvent objects:</h4>
 * 
 * The constants (Symbols and ints) defined in {@link ResourceConstants}
 * are inherited by ResourceEvent.
 * Other Resource interfaces may define additional constants.
 * <p>
 * <table border="1" cellpadding="3">
 * <tr><td>The Symbols returned by</td>
 * <td>have names of the form:</td></tr>
 * <tr><td><code>getEventID()</code></td>
 * 		     <td><code>&nbsp;&nbsp;ev_OperationName</code></td></tr>
 * <tr><td><code>getQualifier()</code></td>
 * 		     <td><code>&nbsp;&nbsp;&nbsp;q_ReasonName</code></td></tr>
 * <tr><td><code>getRTCTrigger()</code></td>
 * 		     <td><code>rtcc_ConditionName</code></td></tr>
 * <tr><td><code>getError()</code></td>
 * 		     <td><code>&nbsp;&nbsp;&nbsp;e_ErrorName</code></td></tr>
 * </table>
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.3
 */
public interface ResourceEvent extends MediaServiceEvent, ResourceConstants {
    /**
     * Get the Qualifier of this event.
     * Additional information about how/why a transaction terminated.
     * <p>
     * Standard return values are one of:
     * <br><code>q_Standard, q_Duration, q_RTC, q_Stop</code><br>
     * which indicate the reason or mode of completion.
     * Other qualifiers may be documented in the resource event classes.
     * <p>
     * If the associated method or transaction did not succeed,
     * this methods returns null; use {@link #getError()} instead. 
     * <p>
     * Because many qualifiers are orthogonal to the EventID, 
     * using a contained qualifier is simpler
     * than using a subclassing scheme.
     *
     * @return one of the qualifier Symbols (q_Something)
     */
    // public Symbol getQualifier();
	
    
    /**
     * Get the RTC Trigger that caused this transaction completion.
     * This is non-null iff <code>getQualifier == ResourceEvent.q_RTC</code>
     * <p>
     * The Symbol returned has a name of the form <code>rtcc_Something</code>.
     * 
     * @return a Symbol identifying an RTC trigger condition.
     */
    public Symbol getRTCTrigger();

    /**
     * Identify the reason or cause of an error or failure.
     * <p>
     * If this ResourceEvent <b>is</b> associated with an Exception,
     * then <code>getError()</code> returns a Symbol that identifies 
     * the problem.
     * <br>If this ResourceEvent <b>is not</b> associated with an Exception,
     * then <code>getError()</code> returns the Symbol <code>e_OK</code>.
     * <p>
     * A list of error Symbols is defined in interface {@link ErrSym}.
     * @return one of the error Symbols. 
     */
    // public Symbol getError();    
}
