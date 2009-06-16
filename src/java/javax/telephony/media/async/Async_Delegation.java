/*
 * Async_Delegation.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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

package javax.telephony.media.async;

import javax.telephony.media.*;

/**
 * Asynchronous versions of the Delegation transaction methods.
 * These methods return Async.Events that <tt>notify</tt> when the 
 * transaction is complete, and are delivered to some MediaListener callback.
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */
public interface Async_Delegation extends DelegationConstants {
    /** Async version of delegateToService.
     * <p>
     * Transfer ownership of the bound MediaGroup to another MediaService, 
     * and stay on the owner stack until the MediaGroup is returned.
     * <p>
     * Invokes {@link Async.DelegationListener#onDelegateToServiceDone onDelegateToServiceDone}
     * when complete.
     *
     * @param serviceName  a String naming the service
     * @param timeout      an int specifying the timeout in milliseconds
     * @param catchTags    a String of tags to match a possible releaseTag
     *
     * @throws NotBoundException if not currently bound to a MediaGroup.
     * @throws MediaBindException a BindInProgressException
     * if bind or release already in progress
     * @throws NotOwnerException if MediaGroup is already delegated.
     *
     * @return an Async.DelegationEvent for onDelegateDone();
     *
     * @see Delegation#delegateToService
     */
    public Async.DelegationEvent async_delegateToService (String serviceName, 
							  int timeout, 
							  String catchTags)
	throws NotBoundException, MediaBindException, NotOwnerException;
    
    /**
     * Cancel the delegation that is in progress on this MediaService.
     * <p>
     * Async version of {@link Delegation#retrieve(Symbol)}.
     * <p>
     * Invokes {@link Async.DelegationListener#onRetrieveDone onRetrieveDone} when complete.
     *
     * @param cause a Symbol that becomes the return value for getRetrieveCause().
     * @return an Async.DelegationEvent for onRetrieveDone()
     * 
     * @throws NotBoundException if not currently bound to a MediaGroup.
     * @throws MediaBindException a BindInProgressException
     * if bind or release is already in progress
     * @throws NotDelegatedException if <tt>delegateToService</tt> is not active.

     * @see Delegation#retrieve
     */
    public Async.DelegationEvent async_retrieve(Symbol cause) 
	throws NotBoundException, MediaBindException, NotDelegatedException;

    /**
     * Cancel the delegation that is in progress on this MediaService.
     * When <tt>delegateToService</tt> completes, this MediaService is not bound.
     * <p>
     * Async version of {@link Delegation#releaseDelegated()}.
     * <p>
     * Invokes {@link Async.DelegationListener#onRetrieveDone onRetrieveDone} when complete.
     *
     * @return an Async.DelegationEvent for onRetrieveDone()
     *
     * @throws NotBoundException if not currently bound to a MediaGroup.
     * @throws MediaBindException a BindInProgressException
     * if bind or release is already in progress
     * @throws NotDelegatedException if <tt>delegateToService</tt> is not active.
     *
     * @see Delegation#releaseDelegated
     */
    public Async.DelegationEvent async_releaseDelegated() 
	throws NotBoundException, MediaBindException, NotDelegatedException;
}
