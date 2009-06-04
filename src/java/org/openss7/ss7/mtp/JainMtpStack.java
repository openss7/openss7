
package org.openss7.ss7.mtp;

/**
  * This interface defines the methods required to represent a
  * proprietary MTP protocol stack, the implementation of which will be
  * vendor specific.
  * Each vendor's protocol stack will have an object that implements
  * this interface to control the creation/deletion of proprietary
  * MtpProviders and the implicit attaching and detaching of those
  * MtpProviders to this JainMtpStack implementation. <p>
  *
  * It must be noted that under the <b>JAIN Naming Convention</b>, the
  * <b>lower-level package structure</b> and <b>classname</b> of a
  * proprietary implementation fo the
  * <i>jain.protocol.ss7.mtp.JainMtpStack</i> interface <b>must</b> be
  * <i>jain.protocol.ss7.mtp.JainMtpStackImpl</i>. <p>
  *
  * Under the JAIN naming convention, the <b>Upper-level package
  * structure</b> (pathname) can be used to differentiate between
  * proprietary implementations from different SS7 vendors.  The
  * pathname used by each SS7 Vendor <b>must be</b> the domain name
  * assigned to the vendor in reverse order; e.g. The OpenSS7 Project's
  * would be "org.openss7". <p>
  *
  * It follows that a proprietary implementation of a JainMtpStack will
  * be located at: <br>
  * <i>&lt;pathname&gt;<b>.</b>jain.protocol.ss7.tcap.JainMtpStackImpl</i>
  * <br>
  * <b>Where:</b> <br>
  * <i>pathname</i> is the reverse domain name, e.g. "org.openss7" <br>
  * The resulting Peer JAIN SS7 Object would be located at:
  * <i><b>org.openss7</b>.jain.protocol.ss7.mtp.JainMtpStackImpl</i>
  * <br>
  * An application may create a JainMtpStackImpl by invoking the
  * {@link jain.protocol.ss7.JainSs7Factory#createJainSS7Object} method.
  * The <b>Pathname</b> of the vendor specific implementation of which
  * you want to instantiate of which you want to instantiate can be set
  * before calling this method or the default or current pathname may be
  * used. <p>
  *
  * For applications that require some means to identify multiple stacks
  * (with multiple SPCs) setStackName() can be used.  An application can
  * choose to supply any identifier to this method. <p>
  *
  * <b>Note</b> that a JainMtpStack represents a single SPC.
  *
  * @author     Monavacon Limited
  * @version    1.2.2
  * @see        JainMtpProvider
  */
public interface JainMtpStack {
    /** The Signaling Point Code can be implemented in one of the
      * following 4 ways: <p>
      * <ul>
      *     <li>ITU_14bit - Split 3-8-3 across member/cluster/zone.
      *     <li>ANSI_24bit - Split 8-8-8 across member/cluster/zone.
      *     <li>Chinese_24bit - Split 8-8-8 across member/cluster/zone.
      *     <li>Japanese_16bit - Split 5-4-7 across member/cluster/zone.
      * </ul> <p>
      *
      * <i>Note to developers:</i> There is no setSignalingPointCode
      * method visible in the JainMtpStack Interface for security
      * reasons. By hiding the setSignalingPointCode method from
      * applications, 3rd party vendors application developers will not
      * be able to configure the signaling Point Code of a specific
      * stack implemetation of the JAIN MTP API, unless they use the
      * JAIN OAM specification which provides a standard interface into
      * provisioning the MTP Layer of a SS7 stack. <p>
      *
      * A developer can alternatively implement a setSignalingPointCode
      * method behind the JainMtpStack Interface and allow propreitory
      * JAIN MTP applications to cast this interface to their
      * implementation to provision the Signaling Point Code.
      *
      * @return The signalingPointCode of this JainMtpStack.
      */
    public SignalingPointCode getSignalingPointCode();
    /**
      * Creates a new Peer (vendor specific) JainMtpProvider whose
      * reference is exposed by this JainMtpStackImp. <p>
      * <i>Note to developers:</i> The implementation of this method
      * should add the newly created JainMtpProvider to the {@link
      * #getProviderList} once the JainMtpProvider has been
      * successfully created and set the JainMtpStack reference in the
      * newly create JainMtpProvider to <b>this</b> object. <p>
      *
      * <b>Example:-</b> <BR>
      * <b>public</b> JainMtpProvider createProvider() <b>throws</b> PeerUnavailableException {
      *      <BLOCKQUOTE><b>try</b> {
      *          <BLOCKQUOTE>  JainMtpProviderImpl myProvider = <b>new</b> JainMtpProviderImpl();</BLOCKQUOTE>
      *      } <b>catch</b>(ClassNotFoundException err) {
      *          <BLOCKQUOTE><b>throw new</b> ss7PeerUnavailableException("Couldn't find peer"); </BLOCKQUOTE>
      *      } <BR>
      *      myProvider.myStack = <b>this</b>;<BR>
      *      <b>return</b> myProvider; </BLOCKQUOTE>
      *  } <p>
      *
      *
      * @return     Peer JAIN MTP Provider referenced by this
      *             JainMtpStackImpl.
      *
      * @exception  PeerUnavailableException  Thrown if the class is not
      *             found for a specific peer implementation.
      */
    public JainMtpProvider createProvider()
        throws PeerUnavailableException;
    /**
      * Deletes the specified Peer JAIN MTP Provider attached to this
      * JainMtpStackImpl. <i>Note to developers:</i> The implementation
      * of this method should remove the specified Peer JAIN MTP
      * Provider from the {@link #getProviderList}. <p>
      *
      * <b>Example:-</b> <BR>
      * <b>public void</b> deleteProvider(JainMtpProvider providerToBeDeleted) <b>throws</b>
      *                   DeleteProviderException { <BR>
      *  <BLOCKQUOTE>
      *        if (<b>null</b> == providerToBeDeleted) {
      *
      *        }<BR>
      *        JainMtpProvider[] myProviderList = <b>this</b>.getProviderList(); <BR>
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
      * @param  providerToBeDeleted  the provider to be deleted
      * @exception  DeleteProviderException thrown if not such provider exists
      */
    public void deleteProvider(JainMtpProvider providerToBeDeleted)
        throws DeletedProviderException;
    /**
      * Returns the array of Peer JAIN MTP Providers that have been
      * created by this JainMtpStackImpl. All of the Peer JAIN MTP
      * Provider s of this JainMtpStackImpl will be proprietary objects
      * belonging to the same stack vendor.
      *
      * @return  an JainMtpProvider Array containing all the of Peer
      * JAIN MTP Providers created by this JainMtpStackImpl.
      */
    public JainMtpProvider[] getProviderList();
    /**
      * Returns the name of the stack as a string
      *
      * @return  A string describing the Stack Name.
      */
    public String getStackName();
    /**
      * Sets the name of the stack as a string
      *
      * @param stackProtocol The new Stack Name value.
      */
    public void setStackName(String stackname);
    /**
      * Returns the Vendor's name for this stack
      *
      * @return A string describing the Vendor's name.
      */
    public String getVendorName();
    /**
      * Sets the Vendors name for this stack, this name will be the
      * Vendor's domain name inverted i.e, "org.openss7" similar to the
      * path name of the JAIN Factory.
      *
      * @param vendorName  The new Vendor's name.
      */
    public void setVendorName(String vendorname);
    /**
      * Gets the stack Specification that this Stack is currently
      * supporting. <p>
      *
      * <b>Note to developers</b> :- This should not be confused with
      * the protocol version field supported by the ANSI 1996 Dialogue
      * Portion.
      *
      * @return    the current Protocol Version of this stack. This may
      * be one of the following values:
      * <ul>
      *     <li> STACK_SPECIFICATION_ANSI_92
      *     <li> STACK_SPECIFICATION_ANSI_96
      *     <li> STACK_SPECIFICATION_ITU_93
      *     <li> STACK_SPECIFICATION_ITU_97
      * </ul>
      * @see MtpConstants
      */
    public int getStackSpecification();
    /**
      * Sets the Stack specification that this Stack is to support.
      * Invoking this method should initially configure the stack to
      * support the given specification if the specification supported
      * has not already been set. In the event that this Stack is
      * already supporting a specification other than the supplied
      * specification, then the Stack will change the supported
      * specification if possible, or throw an Exception if this is not
      * possible.  <p>
      *
      * <b>Note to developers</b> :- This should not be confused with
      * the protocol version field supported by the ANSI 1996 Dialogue
      * Portion.
      *
      * @param  stackSpecification one of the following values:
      * <ul>
      *     <li> STACK_SPECIFICATION_ANSI_92
      *     <li> STACK_SPECIFICATION_ANSI_96
      *     <li> STACK_SPECIFICATION_ITU_93
      *     <li> STACK_SPECIFICATION_ITU_97
      * </ul>
      * @exception VersionNotSupportException thrown if the supplied
      * stack specification cannot be supported by this Stack, or if the
      * Stack cannot change supported stack specifications because it is
      * not in a idle state.
      * @see MtpConstants
      */
    public void setStackSpecification(int stackSpecification)
        throws VersionNotSupportedException;
    public void setSignalingPointCode(SignalingPointCode spc)
        throws ParameterInvalidException;
    public SignalingPointCode getSignalingPointCode()
        throws ParameterNotSetException;
    public void setNetworkIndicator(NetworkIndicator nwInd)
        throws ParameterInvalidException;
    public NetworkIndicator getNetworkIndicator()
        throws ParameterNotSetException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

