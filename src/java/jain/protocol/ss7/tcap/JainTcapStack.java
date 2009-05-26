/*
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Copyrights:
 *
 * Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 * 901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 * This product and related documentation are protected by copyright and
 * distributed under licenses restricting its use, copying, distribution, and
 * decompilation. No part of this product or related documentation may be
 * reproduced in any form by any means without prior written authorization of
 * Sun and its licensors, if any.
 *
 * RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 * States Government is subject to the restrictions set forth in DFARS
 * 252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 * The product described in this manual may be protected by one or more U.S.
 * patents, foreign patents, or pending applications.
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Author:
 *
 * AePONA Limited, Interpoint Building
 * 20-24 York Street, Belfast BT15 1AQ
 * N. Ireland.
 *
 *
 * Module Name   : JAIN TCAP API
 * File Name     : JainTcapStack
 * Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 * Approver      : Jain Tcap Edit Group
 *
 * HISTORY
 * Version   Date      Author              Comments
 * 1.1     13/11/2000  Phelim O'Doherty    Deprecated the attach, detach
 *                                         createAttachedProvider and
 *                                         createDetachedProvider methods.
 *                                         Inserted new method createProvider and
 *                                         changed return type of getProviderList
 *                                         from Vector using deprecation.
 *                                         Clarified stack specification and
 *                                         protocol version.
 *                                         Changed return type of SPC.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7.tcap;

import jain.*;
import jain.PeerUnavailableException;
import jain.protocol.ss7.VersionNotSupportedException;

/**
 * This interface defines the methods required to represent a proprietary JAIN
 * TCAP protocol stack, the implementation of which will be vendor specific.
 * Each vendor's protocol stack will have an object that implements this
 * interface to control the creation/deletion of proprietary <CODE>
 * JainTcapProviders</CODE> and the implicit attaching/detaching of those
 * <CODE>JainTcapProviders</CODE> to this JainTcapStack implementation. <P>
 *
 * It must be noted that under the <B>JAIN Naming Convention</B> the
 * lower-level package structure and classname of a proprietary implementation
 * of the <I>jain.protocol.ss7.tcap.JainTcapStack</I> interface <B>must</B> be
 * <I>jain.protocol.ss7.tcap.JainTcapStackImpl</I> . <P>
 *
 * Under the JAIN naming convention, the <B>upper-level package structure</B>
 * (pathname) can be used to differentiate between proprietary implementations
 * from different SS7 Vendors. The pathname used by each SS7 Vendor <B>must be
 * </B>the domain name assigned to the Vendor in reverse order, e.g. Sun
 * Microsystem's would be 'com.sun' <P>
 *
 * It follows that a proprietary implementation of a JainTcapStack will be
 * located at: <BR>
 * <I><pathname>
 *
 * <B>.</B> jain.protocol.ss7.tcap.JainTcapStackImpl</I> <BR>
 * <B>Where:</B> <BR>
 * <I>pathname</I> = reverse domain name, e.g. com.sun' <BR>
 * The resulting Peer JAIN SS7 Object would be located at: <I><B>com.sun</B>
 * jain.protocol.ss7.tcap.JainTcapStackImpl</I> <BR>
 * An application may create a JainTcapStackImpl by invoking the
 * JainSs7Factory.createJainSS7Object() method. The <b>PathName</b> of the
 * vendor specific implementation of which you want to instantiate can be set
 * before calling this method or the default or current pathname may be used.
 * <BR>
 * <BR>
 * For applications that require some means to identify multiple stacks (with
 * multiple SPCs) setStackName() can be used. An application can choose to
 * supply any identifier to this method.</BR> <BR>
 * <b>Note</b> that a JainTcapStack represents a single SPC.</BR>
 *
 * @author     Sun Microsystems Inc.
 * @version    1.1
 * @see        JainTcapProvider
 */
public interface JainTcapStack {

    /**
    *
    * The Signaling Point Code can be implemented in one of the following 4 ways
    *
    * <li>ITU_14bit - Split 3-8-3 across member/cluster/zone.
    * <li>ANSI_24bit - Split 8-8-8 across member/cluster/zone.
    * <li>Chinese_24bit - Split 8-8-8 across member/cluster/zone.
    * <li>Japanese_16bit - Split 5-4-7 across member/cluster/zone.
    *
    * <BR>
    * <i>Note to developers:</i> There is no setSignalingPointCode method visible
    * in the JainTcapStack Interface for security reasons. By hiding the
    * setSignalingPointCode method from applications, 3rd party vendors
    * application developers will not be able to configure the signaling Point
    * Code of a specific stack implemetation of the JAIN TCAP API, unless they
    * use the JAIN OAM specification which provides a standard interface into
    * provisioning the TCAP Layer of a SS7 stack. <BR>
    * A developer can alternatively implement a setSignalingPointCode method
    * behind the JainTcapStack Interface and allow propreitory JAIN TCAP
    * applications to cast this interface to their implementation in order to
    * provision the Signaling Point Code.
    *
    * @return    the signalingPointCode of this JainTcapStack.
    */
    public int[] getSignalingPointCode();

    /**
    * Creates a new Peer (vendor specific) <CODE>JainTcapProvider</CODE> whose
    * reference is exposed by this JainTcapStackImp. This method replaces the
    * createAttachedProvider and createDetachedProvider methods, which have
    * become deprecated in this interface. <BR>
    * <i>Note to developers:</i> The implementation of this method should add the
    * newly created <CODE>JainTcapProvider</CODE> to the <a
    * href="JainTcapStack.html#getProviderList()">providerList</a> once the
    * <CODE>JainTcapProvider</CODE> has been successfully created and set the
    * JainTcapStack reference in the newly create <code>JainTcapProvider</code>
    * to <b>this</b> object. <p>
    *
    * <b>Example:-</b> <BR>
    * <b>public</b> JainTcapProvider createProvider() <b>throws</b> PeerUnavailableException {
    *      <BLOCKQUOTE><b>try</b> {
    *          <BLOCKQUOTE>  JainTcapProviderImpl myProvider = <b>new</b> JainTcapProviderImpl();</BLOCKQUOTE>
    *      } <b>catch</b>(ClassNotFoundException err) {
    *          <BLOCKQUOTE><b>throw new</b> ss7PeerUnavailableException("Couldn't find peer"); </BLOCKQUOTE>
    *      } <BR>
    *      myProvider.myStack = <b>this</b>;<BR>
    *      <b>return</b> myProvider; </BLOCKQUOTE>
    *  } <p>
    *
    *
    * @return    Peer JAIN TCAP Provider referenced by this JainTcapStackImpl.
    * @exception PeerUnavailableException  thrown if the class is not found
    *           for a specific peer implementation
    * @since     JAIN TCAP v1.1
    */
    public JainTcapProvider createProvider() throws PeerUnavailableException;

    /**
    * @deprecated As of JAIN TCAP v1.1. This method has been replaced by the
    * {@link #createProvider() createProvider} method in this Interface.
    */
    public JainTcapProvider createAttachedProvider() throws PeerUnavailableException, ProviderNotAttachedException;

    /**
    * @deprecated As of JAIN TCAP v1.1. This method has been replaced by the
    * {@link #createProvider() createProvider} method in this Interface.
    */
    public JainTcapProvider createDetachedProvider() throws PeerUnavailableException;

    /**
    * Deletes the specified Peer JAIN TCAP Provider attached to this
    * JainTcapStackImpl. <i>Note to developers:</i> The implementation of this
    * method should remove the specified Peer JAIN TCAP Provider from the <a
    * href="#getProviderList()">getProviderList</a>. <p>
    *
    * <b>Example:-</b> <BR>
    * <b>public void</b> deleteProvider(JainTcapProvider providerToBeDeleted) <b>throws</b>
    *                   DeleteProviderException { <BR>
    *  <BLOCKQUOTE>
    *        if (<b>null</b> == providerToBeDeleted) {
    *
    *        }<BR>
    *        JainTcapProvider[] myProviderList = <b>this</b>.getProviderList(); <BR>
    *        <b>for</b> (<b>int</b> i=0; i&lt;myProviderList.length; i++) {<BR>
    *                <BLOCKQUOTE><b>if</b> (providerToBedeleted == myProviderList[i]){
    *                    <BLOCKQUOTE>myProviderList[i] = <b>null</b>;</BLOCKQUOTE>
    *                } <b>else</b> {
    *                      <BLOCKQUOTE><b>throw new</b> DeleteProviderException("Couldn't find provider"); </BLOCKQUOTE>
    *                }</BLOCKQUOTE>
    *       }<br>
    *       <b>return</b>; </BLOCKQUOTE>
    *  }<p>
    *
    *
    * @param  providerToBeDeleted  the provider to be deleted
    * @exception  DeleteProviderException thrown if not such provider exists
    */
    public void deleteProvider(JainTcapProvider providerToBeDeleted) throws DeleteProviderException;

    /**
    * Deprecated attach and detach methods as the object
    * reference signifying the attach/detach can be associated/nullified with
    * the Provider upon creation and deletion.
    *
    * @deprecated As of JAIN TCAP v1.1. No replacement the attach of the
    * JainTcapProvider occurs implicitly within the
    * {@link #createProvider() createProvider} method of this interface.
    */
    public void attach(JainTcapProvider jainTcapProvider) throws ProviderNotAttachedException;

    /**
    * @deprecated As of JAIN TCAP v1.1. No replacement the detach of the
    * JainTcapProvider occurs implicitly within the
    * {@link #deleteProvider(JainTcapProvider) deleteProvider} method of this interface.
    */
    public void detach(JainTcapProvider jainTcapProvider) throws ProviderNotAttachedException;

    /**
    * Update v1.1: Changed return type of provider List from Vector to Array.
    *
    * Returns the array of Peer JAIN TCAP Providers that have been created by
    * this JainTcapStackImpl. All of the Peer JAIN TCAP Provider s of this
    * JainTcapStackImpl will be proprietary objects belonging to the same stack
    * vendor.
    *
    * @return  an JainTcapProvider Array containing all the of Peer JAIN TCAP
    * Providers created by this JainTcapStackImpl.
    */
    public JainTcapProvider[] getProviderList();

    /**
    * Returns the name of the stack as a string
    *
    * @return  a string describing the Stack Name
    */
    public String getStackName();

    /**
    * Sets the name of the stack as a string
    *
    * @param  stackProtocol  The new Stack Name value
    */
    public void setStackName(String stackName);

    /**
     * Returns the Vendor's name for this stack
     *
     *  @return  a string describing the Vendor's name
     *  @since     JAIN TCAP v1.1
     */
    public String getVendorName();

    /**
     * Sets the Vendors name for this stack, this name will be the Vendor's domain
     * name inverted i.e. "com.sun" similar to the path name of the JAIN Factory.
     *
     * @param  vendorName  The new Vendor's Name
     * @since     JAIN TCAP v1.1
     */
     public void setVendorName(String vendorName);

    /**
    * @deprecated    As of JAIN TCAP v1.1. This method has been replaced by {@link
    * #getStackSpecification() getStackSpecification} in the JainTcapStack
    * Interface.
    *
    * Note: These get/set protocolVersion methods were deprecated to avoid
    * confusion between the stack standards supported and the protocol version field
    * supported by the ANSI 1996 Dialogue Portion
    */
    public int getProtocolVersion();

    /**
    * @deprecated As of JAIN TCAP v1.1. This method has been replaced by
    * {@link #setStackSpecification(int) setStackSpecification} in
    * the JainTcapStack Interface.
    */
    public void setProtocolVersion(int protocolVersion) throws TcapException;

    /**
    * Gets the stack Specification that this Stack is currently supporting. <p>
    *
    * <b>Note to developers</b> :- This should not be confused with the protocol
    * version field supported by the ANSI 1996 Dialogue Portion.
    *
    * @return    the current Protocol Version of this stack. This may be one of the
    *      following values:
    *      <UL>
    *        <LI> STACK_SPECIFICATION_ANSI_92
    *        <LI> STACK_SPECIFICATION_ANSI_96
    *        <LI> STACK_SPECIFICATION_ITU_93
    *        <LI> STACK_SPECIFICATION_ITU_97
    *      </UL>
    *      These values are defined in the TcapConstants class.
    * @since     JAIN TCAP v1.1
    *
    * Note: These get/set protocolVersion methods were deprecated to avoid
    * confusion between the stack standards supported and the protocol version field
    * supported by the ANSI 1996 Dialogue Portion
    */
    public int getStackSpecification();

    /**
    * Sets the Stack specification that this Stack is to support. Invoking this
    * method should initially configure the stack to support the given
    * specification if the specification supported has not already been set. In
    * the event that this Stack is already supporting a specification other than
    * the supplied specification, then the Stack will change the supported
    * specification if possible, or throw an Exception if this is not possible.
    * <p>
    *
    *  <b>Note to developers</b> :- This should not be confused with the protocol
    *  version field supported by the ANSI 1996 Dialogue Portion.
    *
    * @param  stackSpecification one of the following values:
    *      <UL>
    *        <LI> STACK_SPECIFICATION_ANSI_92
    *        <LI> STACK_SPECIFICATION_ANSI_96
    *        <LI> STACK_SPECIFICATION_ITU_93
    *        <LI> STACK_SPECIFICATION_ITU_97
    *      </UL>
    * @exception VersionNotSupportException thrown if the supplied stack specification
    * cannot be supported by this Stack, or if the Stack
    * cannot change supported stack specifications because it is
    * not in a idle state.
    * @since  JAIN TCAP v1.1
    * @see TcapConstants
    */
    public void setStackSpecification(int stackSpecification) throws VersionNotSupportedException;
}

