/*
 * DelegationEvent.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

// delegateToService, retrieve, releaseDelegated
/** 
 * Event type for Delegation methods.
 * <p>
 * EventID corresponds to the method that created the event:
 * <ul><li>
 * <tt>ev_DelegateToService</tt>
 * </li><li>
 * <tt>ev_Retrieve</tt>
 * </li><li>
 * <tt>ev_releaseDelegated</tt>
 * </li></ul>
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public
interface DelegationEvent extends MediaServiceEvent, DelegationConstants {

    /**
     * Returns <tt>true</tt> if <tt>delegateToService()</tt> could
     * not reconfigure the MediaGroup when it was <i>returned</i>.
     * <p>
     * <b>Note:</b> the MediaGroup is returned when the new/current owner
     * calls some form of <tt>release()</tt> 
     * or when this MediaService invokes <tt>retrieve()</tt>.
     * @return true if reconfiguration failed when ownership was returned
     */
    boolean reconfigurationFailed();
	
    /** 
     * Return the cause Symbol of the retrieve() 
     * that caused <tt>delegateToService()</tt> to complete.
     * <p>
     * If this MediaService is unbound because the MediaGroup
     * was retrieved by some older owner using <tt>retrieve(Symbol cause)</tt>,
     * then <tt>getRetrieveCause()</tt> returns the <tt>cause</tt> Symbol
     * and <tt>getQualifier()</tt> returns <tt>q_Retrieved</tt>.
     * <p>
     * If the MediaService is unbound because the MediaGroup
     * was released to some older owner using <tt>releaseToTag</tt>,
     * then <tt>getRetrieveCause()</tt> returns the <tt>v_Released</tt>
     * <tt>getQualifier()</tt> returns <tt>q_Released</tt>, and
     * <tt>getReturnTags()</tt> returns the tag String
     * used in <tt>releaseToTag()</tt>.
     * [that is, a tag which was <i>not</i> caught by this MediaService]
     * <p>
     * Valid when:
     * <tt>event.getEventID().equals(ev_DelegateToService) && </tt>
     * <br>
     * <tt>event.getQualifier.equals(q_Retrieved)</tt>
     * <br>or<br>
     * <tt>event.getQualifier.equals(q_Released)</tt>
     * <p>
     * <b>Note:</b> 
     * for the non-trans onRetrieved(<tt>ev_Retrieved</tt>),
     * the retrieve cause is available from MediaServiceEvent.getQualifier().
     *
     * @return the Symbol used when the MediaGroup was retrieved
     * from this MediaService.
     */
    Symbol getRetrieveCause();
	
    /**
     * The <tt>returnTag</tt> of the <tt>releaseToTag()</tt>
     * that caused <tt>delegateToService()</tt> to complete.
     * <p>
     * Valid when:
     * <tt>event.getEventID().equals(ev_DelegateToService) && </tt>
     * <br>
     * <tt>event.getQualifier().equals(q_Released)</tt>
     * <br>or<br>
     * <tt>event.getQualifier().equals(q_Returned)</tt>
     * <p>
     * @return the <tt>returnTag</tt> String used in <tt>releaseToTag()</tt>.
     */
    String getReturnTag();
};

