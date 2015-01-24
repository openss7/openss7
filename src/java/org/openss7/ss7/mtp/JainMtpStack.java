/*
 @(#) src/java/org/openss7/ss7/mtp/JainMtpStack.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
 Copyright &copy; 2001-2008  OpenSS7 Corporation <a href="http://www.openss7.com/">&lt;http://www.openss7.com/&gt;</a>. <br>
 Copyright &copy; 1997-2001  Brian F. G. Bidulock <a href="mailto:bidulock@openss7.org">&lt;bidulock@openss7.org&gt;</a>. <p>
 
 All Rights Reserved. <p>
 
 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license. <p>
 
 This program is distributed in the hope that it will be useful, but <b>WITHOUT
 ANY WARRANTY</b>; without even the implied warranty of <b>MERCHANTABILITY</b>
 or <b>FITNESS FOR A PARTICULAR PURPOSE</b>.  See the GNU Affero General Public
 License for more details. <p>
 
 You should have received a copy of the GNU Affero General Public License along
 with this program.  If not, see
 <a href="http://www.gnu.org/licenses/">&lt;http://www.gnu.org/licenses/&gt</a>,
 or write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA. <p>
 
 <em>U.S. GOVERNMENT RESTRICTED RIGHTS</em>.  If you are licensing this
 Software on behalf of the U.S. Government ("Government"), the following
 provisions apply to you.  If the Software is supplied by the Department of
 Defense ("DoD"), it is classified as "Commercial Computer Software" under
 paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
 Regulations ("DFARS") (or any successor regulations) and the Government is
 acquiring only the license rights granted herein (the license rights
 customarily provided to non-Government users).  If the Software is supplied to
 any unit or agency of the Government other than DoD, it is classified as
 "Restricted Computer Software" and the Government's rights in the Software are
 defined in paragraph 52.227-19 of the Federal Acquisition Regulations ("FAR")
 (or any successor regulations) or, in the cases of NASA, in paragraph
 18.52.227-86 of the NASA Supplement to the FAR (or any successor regulations). <p>
 
 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package org.openss7.ss7.mtp;

/** This interface defines the methods required to represent a
  * proprietary MTP protocol stack, the implementation of which will be
  * vendor specific.
  * Each vendor's protocol stack will have an object that implements
  * this interface to control the creation/deletion of proprietary
  * MtpProviders and the implicit attaching and detaching of those
  * MtpProviders to this JainMtpStack implementation. <p>
  *
  * It must be noted that under the <b>Jain Naming Convention</b>, the
  * <b>lower-level package structure</b> and <b>classname</b> of a
  * proprietary implementation fo the
  * <i>jain.protocol.ss7.mtp.JainMtpStack</i> interface <b>must</b> be
  * <i>jain.protocol.ss7.mtp.JainMtpStackImpl</i>. <p>
  *
  * Under the Jain naming convention, the <b>Upper-level package
  * structure</b> (pathname) can be used to differentiate between
  * proprietary implementations from different SS7 vendors.  The
  * pathname used by each SS7 Vendor <b>must be</b> the domain name
  * assigned to the vendor in reverse order; e.g. The OpenSS7 Project's
  * would be "org.openss7". <p>
  *
  * It follows that a proprietary implementation of a JainMtpStack will
  * be located at: <br>
  * <i>&lt;pathname&gt;<b>.</b>jain.protocol.ss7.tcap.JainMtpStackImpl</i> <br>
  * <b>Where:</b> <br>
  * <i>pathname</i> is the reverse domain name, e.g. "org.openss7" <br>
  * The resulting Peer Jain SS7 Object would be located at:
  * <i><b>org.openss7</b>.jain.protocol.ss7.mtp.JainMtpStackImpl</i> <br>
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
  * <b>Note:</b> a JainMtpStack represents a single SPC.
  *
  * @author     Monavacon Limited
  * @version    1.2.2
  * @see        JainMtpProvider
  */
public interface JainMtpStack {
    /** The Signaling Point Code can be implemented in one of the
      * following 4 ways: * <ul>
      *     <li>ITU_14bit - Split 3-8-3 across member/cluster/zone;
      *     <li>ANSI_24bit - Split 8-8-8 across member/cluster/zone;
      *     <li>Chinese_24bit - Split 8-8-8 across member/cluster/zone;
      *     <li>Japanese_16bit - Split 5-4-7 across member/cluster/zone. </ul>
      * <em>Note to developers:</em> There is no setSignalingPointCode
      * method visible in the JainMtpStack Interface for security
      * reasons. By hiding the setSignalingPointCode method from
      * applications, 3rd party vendors application developers will not
      * be able to configure the signaling Point Code of a specific
      * stack implemetation of the Jain MTP API, unless they use the
      * Jain OAM specification which provides a standard interface into
      * provisioning the MTP Layer of a SS7 stack. <p>
      * A developer can alternatively implement a setSignalingPointCode
      * method behind the JainMtpStack Interface and allow propreitory
      * Jain MTP applications to cast this interface to their
      * implementation to provision the Signaling Point Code.
      * @return The signalingPointCode of this JainMtpStack.  */
    public javax.jain.ss7.SignalingPointCode getSignalingPointCode();
    /** Creates a new Peer (vendor specific) JainMtpProvider whose
      * reference is exposed by this JainMtpStackImp. <p>
      * <em>Note to developers:</em> The implementation of this method
      * should add the newly created JainMtpProvider to the {@link
      * #getProviderList} once the JainMtpProvider has been successfully
      * created and set the JainMtpStack reference in the newly create
      * JainMtpProvider to <b>this</b> object. <p>
      * <b>Example:-</b> <br>
      * <b>public</b> JainMtpProvider createProvider() <b>throws</b> javax.jain.PeerUnavailableException {
      *      <blockquote><b>try</b> {
      *          <blockquote>  JainMtpProviderImpl myProvider = <b>new</b> JainMtpProviderImpl();</blockquote>
      *      } <b>catch</b>(ClassNotFoundException err) {
      *          <blockquote><b>throw new</b> ss7PeerUnavailableException("Couldn't find peer"); </blockquote>
      *      } <br>
      *      myProvider.myStack = <b>this</b>;<br>
      *      <b>return</b> myProvider; </blockquote>
      *  } <p>
      * @return
      * Peer Jain MTP Provider referenced by this JainMtpStackImpl.
      * @exception javax.jain.PeerUnavailableException
      * Thrown when the class is not found for a specific peer
      * implementation.
      * @exception javax.jain.ParameterNotSetException
      * Thrown when a mandatory stack parameter is not set.  */
    public JainMtpProvider createProvider()
        throws javax.jain.PeerUnavailableException,
               javax.jain.ParameterNotSetException;
    /** Deletes the specified Peer Jain MTP Provider attached to this
      * JainMtpStackImpl. <p>
      * <em>Note to developers:</em> The implementation of this method
      * should remove the specified Peer Jain MTP Provider from the
      * {@link #getProviderList}. <p>
      * <b>Example:-</b> <br>
      * <b>public void</b> deleteProvider(JainMtpProvider providerToBeDeleted) <b>throws</b>
      *                   DeleteProviderException { <br>
      *  <blockquote>
      *        if (<b>null</b> == providerToBeDeleted) {
      *
      *        }<br>
      *        JainMtpProvider[] myProviderList = <b>this</b>.getProviderList(); <br>
      *        <b>for</b> (<b>int</b> i=0; i&lt;myProviderList.length; i++) {<br>
      *                <blockquote><b>if</b> (providerToBedeleted == myProviderList[i]){
      *                    <blockquote>myProviderList[i] = <b>null</b>;</blockquote>
      *                } <b>else</b> {
      *                      <blockquote><b>throw new</b> DeleteProviderException("Couldn't find provider"); </blockquote>
      *                }</blockquote>
      *       }<br>
      *       <b>return</b>; </blockquote>
      *  }<p>
      * @param  providerToBeDeleted
      * The provider to be deleted.
      * @exception  DeleteProviderException
      * Thrown when no such provider exists.  */
    public void deleteProvider(JainMtpProvider providerToBeDeleted)
        throws DeletedProviderException;
    /** Returns the array of Peer Jain MTP Providers that have been
      * created by this JainMtpStackImpl.
      * All of the Peer Jain MTP Provider s of this JainMtpStackImpl
      * will be proprietary objects belonging to the same stack vendor.
      * @return
      * A JainMtpProvider Array containing all the of Peer Jain MTP
      * Providers created by this JainMtpStackImpl.  */
    public JainMtpProvider[] getProviderList();
    /** Returns the name of the stack as a string
      * @return
      * A string describing the Stack Name.  */
    public java.lang.String getStackName();
    /** Sets the name of the stack as a string.
      * @param stackName
      * The new Stack Name value.  */
    public void setStackName(java.lang.String stackName);
    /** Returns the Vendor's name for this stack
      * @return
      * A string describing the Vendor's name.  */
    public java.lang.String getVendorName();
    /** Sets the Vendors name for this stack, this name will be the
      * Vendor's domain name inverted i.e, "org.openss7" similar to the
      * path name of the Jain Factory.
      * @param vendorName
      * The new Vendor's name.  */
    public void setVendorName(java.lang.String vendorName);
    /** Gets the stack Specification that this Stack is currently
      * supporting. <p>
      * <b>Note to developers:-</b> This should not be confused with the
      * protocol version field supported by the ANSI 1996 Dialogue
      * Portion.
      * @return
      * The current Protocol Version of this stack. This may be one of
      * the following values: * <ul>
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ANSI_92 STACK_SPECIFICATION_ANSI_92}
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ANSI_96 STACK_SPECIFICATION_ANSI_96}
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ITU_93 STACK_SPECIFICATION_ITU_93}
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ITU_97 STACK_SPECIFICATION_ITU_97} </ul>
      * @see MtpConstants */
    public int getStackSpecification();
    /** Sets the Stack specification that this Stack is to support.
      * Invoking this method should initially configure the stack to
      * support the given specification if the specification supported
      * has not already been set. In the event that this Stack is
      * already supporting a specification other than the supplied
      * specification, then the Stack will change the supported
      * specification if possible, or throw an Exception if this is not
      * possible.  <p>
      * <b>Note to developers:-</b> This should not be confused with the
      * protocol version field supported by the ANSI 1996 Dialogue
      * Portion.
      * @param  stackSpecification
      * One of the following values: * <ul>
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ANSI_92 STACK_SPECIFICATION_ANSI_92}
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ANSI_96 STACK_SPECIFICATION_ANSI_96}
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ITU_93 STACK_SPECIFICATION_ITU_93}
      * <li>{@link MtpConstants#STACK_SPECIFICATION_ITU_97 STACK_SPECIFICATION_ITU_97} </ul>
      * @exception javax.jain.VersionNotSupportException
      * Thrown when the supplied stack specification cannot be supported
      * by this Stack, or if the Stack cannot change supported stack
      * specifications because it is not in a idle state.
      * @see MtpConstants */
    public void setStackSpecification(int stackSpecification)
        throws javax.jain.VersionNotSupportedException;
    public void setSignalingPointCode(javax.jain.ss7.SignalingPointCode signalingPointCode)
        throws javax.jain.protocol.ss7.SS7InvalidParamException;
    public javax.jain.ss7.SignalingPointCode getSignalingPointCode()
        throws javax.jain.ParameterNotSetException;
    public void setNetworkIndicator(javax.jain.ss7.NetworkIndicator networkIndicator)
        throws javax.jain.protocol.ss7.SS7InvalidParamException;
    public javax.jain.ss7.NetworkIndicator getNetworkIndicator()
        throws javax.jain.ParameterNotSetException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
