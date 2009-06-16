/*
 * Delegation.java Version-1.4, 2002/11/22 09:26:10 -0800 (Fri)
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
 * An optional MediaService interface for structured handoff and return.
 * Delegation allows an application to pass control of the media processing
 * to another MediaService and then regain control.
 * <P>
 * {@link #delegateToService(String, int, String)} is similar to 
 * {@link MediaService#releaseToService(String, int) releaseToService(String, int)}:
 * the MediaGroup of this MediaService is bound to a new MediaService.
 * However, the MediaGroup is <i>not</i> released from
 * the invoking MediaService; instead,
 * this MediaService is pushed onto a stack of previous <i>Owners</i>.
 * <p>
 * A MediaService on the <i>ownerstack</i> is still <i>bound</i> to the
 * MediaGroup, but is not the current Owner of that MediaGroup;
 * it is not allowed to invoke most operations on the MediaGroup.
 * The method {@link #isOwner()} indicates if this MediaSerice
 * is currently the Owner of its MediaGroup. If the MediaService
 * is not bound then <tt>isOwner</tt> returns <tt>false</tt>.
 * <p>
 * <tt>delegateToService</tt>
 * also associates a String of <i>catchTags</i>
 * with this MediaService on the ownerstack of the MediaGroup.
 * These tags are used when processing {@link MediaService#releaseToTag(String)}.
 * <p>
 * <h4> Returning from delegateToService </h4>
 *
 * <tt>delegateToService</tt> <i>waits</i> until the MediaGroup is
 * returned to this MediaService.
 * While <tt>delegateToService</tt> is active,
 * there are two ways to stop waiting for the MediaGroup to be returned:
 * <ul><li>
 * <tt>retrieve()</tt> 
 * causes <tt>delegateToService</tt> to return 
 * by re-acquiring the MediaGroup.
 * </li><li>
 * <tt>releaseDelegated()</tt>
 * causes <tt>delegateToService</tt> to return <i>unbound</i>
 * by removing this MediaService from the ownerstack.
 * </li></ul>
 * <p>
 * When the MediaGroup is returned to this MediaService,
 * the MediaProvider attempts to reconfigure it to the configuration
 * in effect when <tt>delegateToService</tt> was initiated.
 * If that reconfiguration fails, then the MediaGroup
 * is configured with no Resources [<tt>ConfigSpec.emptyConfigSpec</tt>]
 * and the method
 * {@link DelegationEvent#reconfigurationFailed()} returns <tt>true</tt>.
 * <p>
 * <h4> Catch and Return tag usage </h4>
 * When the current owner <i>releases</i> the MediaGroup 
 * (using {@link MediaService#releaseToTag(String)} or one of its variants),
 * the stack is popped and ownership returns to a previous owner.
 * The particular owner to regain ownership is determined by
 * the <i>returnTag</i> supplied in the release method,
 * and the <tt>catchTags</tt> supplied in the <tt>delegateToService</tt> methods.
 * The process is as follows:
 * <p>
 * Each previous owner on the stack (that is, each MediaService that
 * has an open call to <tt>delegateToService</tt>),
 * supplies a String of <i>catchTags</i> which is associated with
 * that MediaService on the owner stack.  When the current
 * owner releases the MediaGroup using <tt>releaseToTag(String)</tt>,
 * the stack of previous owners is searched from the most recent owner
 * to the least recent owner:
 * <ul><li>
 * If the <tt>catchTags</tt> of a previous owner match
 * the <tt>returnTags</tt> supplied in <tt>releaseToTag(String)</tt>,
 * then that MediaService becomes the new owner;
 * its call to <tt>delegateToService()</tt> returns, 
 * <tt>onDelegationDone()</tt> is invoked on the <tt>Async_DelegationListener</tt>,
 * and that MediaService once again <tt>isOwner</tt> of the MediaGroup.
 * It is <i>still</i> bound to the 
 * </li><li>
 * If the <tt>catchTags</tt> of the previous owner do <b>not</b> match
 * the <tt>returnTags</tt> supplied in <tt>releaseToTag(String)</tt>,
 * then that MediaService is removed from the stack of previous owners;
 * its call to <tt>delegateToService()</tt> returns,
 * <tt>onDelegationDone()</tt> is invoked on the Async_DelegationListener,
 * and that MediaService is <b>not</b> bound to the MediaGroup.
 * </li></ul>
 * <p>
 * If the <i>returnTags</i> do not match the <tt>catchTags</tt> 
 * of any previous owner, then
 * <ul><li>
 * all open calls to <tt>delegateToService</tt> return, 
 * </li><li>
 * <tt>onDelegationDone()</tt> is invoked (with ev_Released) 
 * on the Async_DelegationListener of all previous owners,
 * </li><li>
 * all of the previous owners are unbound from the MediaGroup,
 * </li><li>
 * any Call/Connection to the MediaGroup is disconnected,
 * </li><li>
 * the MediaGroup is destroyed.
 * </li></ul>
 * In effect, the eldest owner on the stack catches <i>all</i> tags,
 * and implements the <i>disconnect/destroy</i> behavior. 
 * This is the normal and expected effect when <tt>releaseToDestroy()</tt>
 * or <tt>releaseToTag(tag_DestroyRequest)</tt> is called,
 * or when <tt>release()</tt> is called and there is no previous owner.
 * <p>
 * <h4> Catch and Return tag Values</h4>
 *
 * The <tt>catchTags</tt> and <tt>returnTags</tt> are Strings.  
 * By convention, each tag is formatted as the name of an S.100 Symbol:
 * "<tt>Object_VENDOR_ItemName</tt>".
 * Multiple tags are specified by concatenating the basic tag strings 
 * with a whitespace separator.  
 * Whitespace is not allowed <i>within</i> a tag name.
 * <p>
 * Some catch/return tags are pre-assigned, and have special
 * interpretations:
 * <table border="1" cellpadding="3">
 * <tr><td><tt>Tag Name:</tt></td><td>Used for:</td></tr>
 * <tr><td><tt>tag_NormalReturn</tt></td><td>simple release</td></tr>
 * <tr><td><tt>tag_DestroyRequest</tt></td><td>release and terminate call</td></tr>
 * <tr><td><tt>tag_RemoveSession</tt></td><td>release with no delegation</td></tr>
 * <tr><td><tt>tag_SessionDeath</tt></td><td>special release if owner dies</td></tr>
 * <tr><td><tt>tag_CatchAll</tt></td><td>catch any/all tags</td></tr>
 * <tr><td><tt>tag_CatchNone</tt></td><td>never catches</td></tr>
 * <tr><td><tt>tag_NeverReturn</tt></td><td>never catches</td></tr>
 * <tr><td><tt>null</tt> or <tt>()</tt></td><td>same as tag_NormalReturn</td></tr>
 * </table>
 * <p>
 * <b>Note:</b> catching only <tt>tag_CatchNone</tt> keeps this MediaService
 * bound to the MediaGroup, but ensures that ownership will not return
 * to this MediaService. When <tt>delegateToService()</tt> completes
 * this <tt>MediaService.isBound()</tt> will be <tt>false</tt>, 
 * and the MediaGroup will be passed to some previous owner, 
 * (the GroupProvider if none other).
 * <p>
 * <b>Note:</b> some tags are useful only for <tt>delegateToService()</tt>.
 * Releasing to <tt>tag_CatchNone</tt> has no useful effect.
 * <p>
 * <h4>isOwner() and retrieve()</h4>
 * 
 * While the MediaGroup is delegated, the MediaService is <i>bound</i>
 * (<tt>isBound()</tt> returns <tt>true</tt>), but the MediaService is 
 * not the <i>owner</i> (<tt>isOwner()</tt> returns <tt>false</tt>).
 * Most MediaService methods and Resource methods are prohibited
 * unless the MediaService is the owner.  Those methods throw
 * <tt>NotOwnerException</tt> (a RuntimeException) if invoked when
 * the MediaService is not the current owner.
 * <p>
 * To regain ownership, one must either wait for delegation to complete, 
 * or use <tt>retrieve()</tt>.
 * {@link #retrieve(Symbol)} is like <tt>releaseToTag(String)</tt> in that
 * it causes a non-local (that is, it may skip levels on the ownership stack) 
 * transfer of ownership. 
 * However, <tt>retrieve(Symbol)</tt> is initiated by a previous owner
 * (which MediaService becomes the new, current owner). In contrast,
 * <tt>releaseToTag(String)</tt> is initiated by the current owner.
 *
 * <p>
 * <h4>Delegation is Optional</h4>
 * <b>Note:</b> 
 * Implementing the Delegation interface is optional. 
 * If the MediaService does <b>not</b> otherwise implement this interface, then
 * <table border="1" cellpadding="3">

 * <tr><td><tt>delegateToService(String,int,String)</tt></td>
 * <td>throws <tt>NoServiceAssignedException</tt></td></tr>

 * <tr><td><tt>retrieve()</tt></td>
 * <td>has no effect</td></tr>

 * <tr><td><tt>releaseDelegated()</tt></td>
 * <td>throws <tt>NotDelegatedException</tt></td></tr>

 * <tr><td><tt>releaseToTag(String)</tt></td><td>same as <tt>release()</tt>, 
 * because there are no previous owners</td></tr>

 * <tr><td><tt>releaseToDestroy()</tt></td><td>same as <tt>release()</tt>, 
 * because there are no previous owners</td></tr>

 * <tr><td><tt>isOwner()</tt></td><td>same as <tt>isBound()</tt></td></tr>
 * </table>
 * <p>
 * <h4>Conclusion</h4>
 * This interface is intended for use in application frameworks that:
 * <ul><li>
 * coordinate a suite of co-developed applications, or
 * </li><li>
 * use a generic server, but interpose on the standard <tt>release()</tt>
 * </li></ul>  
 * Developers creating such frameworks or creating applications
 * for such frameworks may need to use this interface.
 * In that case, the framework developer or application developer 
 * should document any non-standard tags being used.
 * <p>
 * In other cases, applications should only need the MediaService 
 * methods <tt>release()</tt> and <tt>releaseToService()</tt>.
 * <p>
 *
 * @author  Jeff Peck
 * @since   JTAPI-1.4
 */

public interface Delegation extends DelegationConstants {
    /**
     * Transfer ownership of the bound MediaGroup to another MediaService, 
     * and wait for the MediaGroup to be returned.
     * This MediaService is bound to the group throughout the operation,
     * but is not the <i>owner</i> while waiting for the MediaGroup
     * to be returned.
     * <p>
     * A new owner is identified by the given <tt>serviceName</tt>. 
     * see {@link MediaService#releaseToService(String, int)} for details.
     * <p>
     * Unlike <tt>releaseToService()</tt>, <tt>delegateToService()</tt>
     * does not release the current MediaService. 
     * Instead, the MediaService continues to be <i>bound</i>
     * but it is not the <i>owner</i>; 
     * it is not allowed to invoke Resource methods.
     * <P>
     * If the MediaGroup is <b>not</b> returned to this MediaService 
     * when <tt>delegateToService</tt> completes
     * (by the action of <tt>releaseToTag</tt> or <tt>retrieve</tt> 
     * or <tt>releaseDelegated</tt>), then no Exception is thrown.
     * <p>
     * If <tt>delegateToService</tt> returns without the MediaGroup because
     * if was retrieved by a older/previous owner, then
     * <ul><li>
     * <tt>isBound()</tt> returns <tt>false</tt>,
     * </li><li>
     * <tt>getRetrieveCause()</tt> has the value assigned by <tt>retrieve()</tt>,
     * </li><li>
     * and <tt>MediaServiceListener.onRetrieved()</tt> is invoked.
     * </li></ul>
     *
     * If <tt>delegateToService</tt> returns without the MediaGroup
     * because it was released by the newest/current owner 
     * to some <tt>Tag</tt> not caught by this MediaService, then
     * <ul><li>
     * <tt>isBound()</tt> returns <tt>false</tt>,
     * </li><li>
     * {@link DelegationEvent#getReturnTag()} returns a non-null value.
     * </li></ul>
     * 
     * Applications should check <tt>isBound()</tt> or <tt>getReturnTag()</tt> 
     * after returning from <tt>delegateToService</tt>
     * (or when <tt>onDelegationDone</tt> is invoked
     * on the Async.DelegationListener).
     * <p>
     * <b>Note:</b>
     * <tt>cancelBindRequest()</tt> terminates <tt>delegateToService</tt> only
     * if it is processed before the MediaGroup is bound to some other owner.
     * To cancel delegation <i>after</i> MediaGroup is delegated, 
     * use <tt>retrieve()</tt>.
     * <br><b>See also:</b> {@link DelegationListener#onDelegated onDelegated()}
     * <p>
     * <b>Note:</b>
     * <tt>delegateToService(serviceName, timeout, tag_RemoveSession)</tt> 
     * is roughly equivalent to <tt>releaseToService(serviceName, timeout)</tt>.
     * They differ in the type of the return event and the associated
     * MediaListener invocation. 
     * Using <tt>tag_RemoveSession</tt> in this way may work,
     * but is not supported. 
     * <tt>releaseToService()</tt> is the preferred form to use.
     * <p>
     * @param serviceName the service name of the recipient.
     * @param timeout the time to wait for a service to become ready.
     * @param catchTags a String of tags to control <tt>releaseToTag(String)</tt>;
     * <br><tt>catchTags = null</tt> is equivalent to <tt>tag_NormalReturn</tt>.
     * @return a DelegationEvent that indicates how the delegation terminated.
     *
     * @throws NotBoundException if not currently bound to a MediaGroup
     * @throws NotOwnerException if delegateToService in progress.
     *
     * @throws MediaBindException one of:
     * <tt>BindInProgressException</tt>, 
     * <tt>NoServiceAssignedException</tt>,
     * <tt>NoServiceReadyException</tt>, 
     * <tt>BindCancelledException</tt>
     * @throws BindCancelledException if interrupted by <tt>cancelBindRequest</tt>.
     *
     * @throws NoServiceAssignedException
     * if <tt>serviceName</tt> is not recognised
     * or is not mapped to any known serviceName.
     * @throws NoServiceReadyException 
     * if none of the MediaServices registered to <tt>serviceName</tt> are ready
     * and none become ready within <code>timeout</code> milliseconds.
     *
     * @throws MediaConfigException 
     * if the MediaGroup could not be configured for the recipient service.
     *
     * @see MediaService#releaseToService(String, int)
     * @see javax.telephony.media.async.Async.DelegationListener#onDelegateToServiceDone(MediaServiceEvent)
     */
    DelegationEvent delegateToService(String serviceName, int timeout, String catchTags)
	throws MediaBindException,
	       MediaConfigException; 

    /*
     * should not get "bind" in progress: get NotBound instead...
     * but may get "release" in progress. 
     * 
     * <b>Note:</b> <tt>cancelBindRequest()</tt> terminates <tt>delegateToService</tt> 
     * only if it is processed before the MediaGroup is bound to some other owner.
     * to cancel delegation after MediaGroup is delegated, use retrieve().
     */

    /*
     * Implementation note: an application may issue delegateToService() 
     * and then issue releaseToTag(tag_RemoveSession)
     * to get off the ownerstack.
     * But if it has <i>just</i> been given ownership
     * (by the current owner's releaseToTag(someTag)),
     * and has not yet recieved notification that it became the owner,
     * then there could be a nasty side-effect:
     * The previous owner thinks it has sent the MediaGroup to someTag,
     * [and it did] but this MediaService has issued release(remove)
     * without knowing that the group was returned to someTag;
     * and without doing any appropriate processing.
     * Therefore, server implementors are advised to notice when
     * return(someTag) is done and release(remove) arrives
     * as <i>the next command</i> on the MediaGroup.
     * in such a case, the server should *restart* the release(someTag)
     * instead of processing the release(remove).
     */

    /**
     * Terminate the delegation that is in progress on this MediaService.
     * On successful completion, this MediaService is owner of the bound MediaGroup.
     * <p>
     * The sequence of processing is: 
     * <ul><li>
     * Ongoing media operations terminate with <tt>MediaResourceException</tt>.
     * </li><li>
     * The current owner (<tt>MediaService</tt>) of the MediaGroup
     * invokes the <tt>onRetrieved(MediaServiceEvent)</tt> method of
     * any DelegationListener added to that MediaService.
     * </li><li>
     * The <tt>delegateToService()</tt> method of any intervening MediaSevice 
     * terminates with <tt>isBound()</tt> false and <tt>getReturnTag()</tt> set.
     * </li><li>
     * <tt>delegateToService()</tt> completes and
     * <tt>isBound()</tt> and <tt>isOwner()</tt> are <tt>true</tt>.
     * </li></ul>
     * <p>
     * If <tt>retrieve</tt> is processed when this 
     * <tt>MediaService.isOwner()</tt> is <tt>true</tt>,
     * because <tt>delegateToService</tt> has returned (or was never invoked),
     * this method succeeds with no other effect.
     * <p>
     * If <tt>retrieve()</tt> is processed when
     * this <tt>MediaService.isBound()</tt> is <tt>false</tt>,
     * then it throws <tt>NotBoundException</tt>.
     * <p>
     * <b>Note:</b> the state of <tt>isOwner()</tt> and <tt>isBound()</tt> 
     * may change between the time <tt>retrieve()</tt> is invoked
     * and when it is processed by the platform.
     * Other owners may <tt>release</tt> or <tt>retrieve</tt> the MediaGroup.
     * <p>
     * <b>Note:</b>
     * This method may have undesirable effects on the application
     * to which the MediaGroup has been delegated, or may have undersirable
     * effects on the user interface experience of a caller.  
     * This function is intended to be used within suites of coordinated 
     * applicatations where the effects are understood and acceptable.
     * <i>If you do not know the effect of this method on the application
     *  which is currently the owner, then do not use this method.</i>
     * <p>
     * <h5> Future Enhancement Proposal </h5>
     * In some future release, <tt>retrieve()</tt> could be implemented by 
     * adding <tt>tag_Retrieval</tt> to the catchTags of the invoking MediaService,
     * and delivering an event to the current owner using 
     * <tt>MediaServiceListener.onRetrieved(ev_RetrieveRequest)</tt>.
     * This instructs the current owner to invoke
     * <tt>releaseToTag(tag_Retrieval)</tt> at its earliest convenience.
     * <p>
     * <b>Note:</b> the current owner (or any owner between the current and 
     * the requesting owner) could delay retrieval
     * by setting an parameter <tt>p_RetrieveDelay</tt> to the 
     * maximum milliseconds to wait (up to a framework determined maximum)
     * before responding to the retrieval request.
     * That would give the owner time to say goodbye, 
     * and issue <tt>releaseToTag(tab_Retrieval)</tt>.
     * The framework could intervene after the specified delay 
     * to enforce timely retrieval.
     * The current specification corresponds to
     * <tt>p_RetrieveDelay = 0</tt>, which is appropriate for retrieving
     * from a music-on-hold application when a live agent is available.
     * <p>
     * @param cause a Symbol that becomes the return value
     * for <tt>getRetrieveCause()</tt>.
     *
     * @throws NotBoundException if not currently bound to a MediaGroup
     * @throws MediaBindException a <tt>BindInProgressException</tt>
     * if a bind or release is in progress.     
     * @throws NotDelegatedException (a RuntimeException) if
     * <tt>delegateToService</tt> has completed or not been called.
     */
    void retrieve(Symbol cause) 
	throws NotBoundException, MediaBindException, NotDelegatedException;

    /**
     * Release while delegation is active.
     * Remove this MediaService from the ownerstack of the bound MediaGroup.
     * Terminates delegateToService, leaving this MediaService not bound.
     * <p>
     * <br>pre-conditions: <tt>(isBound() && isDelegated())</tt>
     * <br>post-conditions: <tt>(!isBound() && !isDelegated())</tt>
     * <p>
     * <b>Note:</b> may throw an Exception if the pre-conditions are not
     * true when the request is actually processed by the platform.
     * Processing by other owners may change these properties between
     * the time the method is invoked and when it is processed.
     * <p>
     * <b>Note:</b>
     * <tt>releaseDelegated()</tt> can succeed only if it is processed
     * while <tt>delegateToService</tt> is active on this MediaService.
     * This is the one case where the platform may refuse to release.
     * This restriction protects against a race when the application
     * tries to get off the ownerstack <i>just after</i> 
     * (but before the app has been informed that) the MediaService has
     * regained ownership. [that is, the previous/current owner just released
     * the MediaGroup to this MediaService.]
     * With this restriction, when the application eventually
     * gets the <tt>onDelegateDone()</tt> notification, it is and will continue
     * to be the owner; when the platform eventually processes
     * <tt>releaseDelegated()</tt> it will throw <tt>NotDelegatedException</tt>.
     *
     * @throws NotBoundException if not currently bound to a MediaGroup
     * @throws MediaBindException a <tt>BindInProgressException</tt>
     * if a bind or release is in progress.     
     * @throws NotDelegatedException (a RuntimeException) if
     * <tt>delegateToService</tt> has completed or not been called.
     */
    void releaseDelegated() 
	throws NotBoundException, MediaBindException, NotDelegatedException;

    /**
     * Return true if this MediaService is delegating ownership 
     * so some other MediaService.
     * 
     * @return true if <tt>delegateToService</tt> is active. 
     */
    boolean isDelegated();
    
    /**
     * Return true if this MediaService is currently the owner 
     * of a bound MediaGroup.
     * <P>
     * <tt>isOwner()</tt> is: <tt>(isBound() && !isDelegated())</tt>
     *
     * @return true if this MediaService is bound and not delegated.
     */
    boolean isOwner();
}

