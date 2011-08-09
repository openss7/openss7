/*
 @(#) $RCSfile: JainTcapStack.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:29 $ <p>
 
 Copyright &copy; 2008-2009  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a> <p>
 
 Last Modified $Date: 2009-06-21 11:34:29 $ by $Author: brian $
 */

package jain.protocol.ss7.tcap;

import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines the methods required to represent a
  * proprietary JAIN TCAP protocol stack, the implementation of which
  * will be vendor specific. Each vendor's protocol stack will have an
  * object that implements this interface to control the
  * creation/deletion of proprietary JainTcapProviders and the implicit
  * attaching/detaching of those JainTcapProviders to this JainTcapStack
  * implementation. <p>
  * It must be noted that under the JAIN Naming Convention the
  * lower-level package structure and classname of a proprietary
  * implementation of the jain.protocol.ss7.tcap.JainTcapStack interface
  * must be jain.protocol.ss7.tcap.JainTcapStackImpl. <p>
  * Under the JAIN naming convention, the upper-level package structure
  * (pathname) can be used to differentiate between proprietary
  * implementations from different SS7 Vendors. The pathname used by
  * each SS7 Vendor must be the domain name assigned to the Vendor in
  * reverse order, e.g. Sun Microsystem's would be 'com.sun' <p>
  * It follows that a proprietary implementation of a JainTcapStack will
  * be located at: <br>
  * <code>jain.protocol.ss7.tcap.JainTcapStackImpl</code> <br>
  * Where: <br>
  * <i>pathname = reverse domain name, e.g. 'com.sun'</i><br>
  * The resulting Peer JAIN SS7 Object would be located at: <br>
  * <code>com.sun.jain.protocol.ss7.tcap.JainTcapStackImpl</code><p>
  * An application may create a JainTcapStackImpl by invoking the
  * JainSs7Factory.createJainSS7Object() method. The PathName of the
  * vendor specific implementation of which you want to instantiate can
  * be set before calling this method or the default or current pathname
  * may be used. <p>
  * For applications that require some means to identify multiple stacks
  * (with multiple SPCs) setStackName() can be used. An application can
  * choose to supply any identifier to this method.  Note that a
  * JainTcapStack represents a single SPC.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public interface JainTcapStack {
    /** The Signaling Point Code can be implemented in one of the following 4 ways. <ol>
      * <li> ITU_14bit - Split 3-8-3 across member/cluster/zone.
      * <li> ANSI_24bit - Split 8-8-8 across member/cluster/zone.
      * <li> Chinese_24bit - Split 8-8-8 across member/cluster/zone.
      * <li> Japanese_16bit - Split 5-4-7 across member/cluster/zone.
      * </ol>
      * <em>Note to developers:</em> There is no setSignalingPointCode
      * method visible in the JainTcapStack Interface for security
      * reasons. By hiding the setSignalingPointCode method from
      * applications, 3rd party vendors application developers will not
      * be able to configure the signaling Point Code of a specific
      * stack implemetation of the JAIN TCAP API, unless they use the
      * JAIN OAM specification which provides a standard interface into
      * provisioning the TCAP Layer of a SS7 stack.  A developer can
      * alternatively implement a setSignalingPointCode method behind
      * the JainTcapStack Interface and allow propreitory JAIN TCAP
      * applications to cast this interface to their implementation in
      * order to provision the Signaling Point Code.
      * @return
      * The signalingPointCode of this JainTcapStack.  */
    public int[] getSignalingPointCode();
    /** Creates a new Peer (vendor specific) JainTcapProvider whose
      * reference is exposed by this JainTcapStackImp. This method
      * replaces the createAttachedProvider and createDetachedProvider
      * methods, which have become deprecated in this interface.  Note
      * to developers: The implementation of this method should add the
      * newly created JainTcapProvider to the providerList once the
      * JainTcapProvider has been successfully created and set the
      * JainTcapStack reference in the newly create JainTcapProvider to
      * this object. <p>
      * Example:-
      * <br><code>&nbsp;public JainTcapProvider createProvider() throws</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;PeerUnavailableException {</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;try {</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;JainTcapProviderImpl myProvider = new JainTcapProviderImpl();</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;} catch (ClassNotFoundException err) {</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;throw new ss7PeerUnavailableException("Couldn't find peer");</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;}</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;myProvider.myStack = this;</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;return myProvider;</code>
      * <br><code>&nbsp;}</code>
      * @return
      * Peer JAIN TCAP Provider referenced by this JainTcapStackImpl.
      * @exception PeerUnavailableException
      * Thrown if the class is not found for a specific peer
      * implementation.
      * @since JAIN TCAP v1.1 */
    public JainTcapProvider createProvider()
        throws PeerUnavailableException;
    /** @deprecated
      * As of JAIN TCAP v1.1. This method has been replaced by the
      * createProvider method in this Interface.  */
    public JainTcapProvider createAttachedProvider()
        throws PeerUnavailableException, ProviderNotAttachedException;
    /** @deprecated
      * As of JAIN TCAP v1.1. This method has been replaced by the
      * createProvider method in this Interface.  */
    public JainTcapProvider createDetachedProvider()
        throws PeerUnavailableException;
    /** Deletes the specified Peer JAIN TCAP Provider attached to this
      * JainTcapStackImpl. Note to developers: The implementation of
      * this method should remove the specified Peer JAIN TCAP Provider
      * from the getProviderList. <p>
      * Example:-
      * <br><code>&nbsp;public void deleteProvider(JainTcapProvider</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;providerToBeDeleted) throws DeleteProviderException {</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;if (null == providerToBeDeleted) { }</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;JainTcapProvider[] myProviderList = this.getProviderList();</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;for (int i=0; i&lt;myProviderList.length; i++) {</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;if (providerToBedeleted == * myProviderList[i]) {</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;myProviderList[i] = null;</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;} else {</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;throw new DeleteProviderException("Couldn't find provider");</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;}</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;}</code>
      * <br><code>&nbsp;&nbsp;&nbsp;&nbsp;return;</code>
      * <br><code>&nbsp;}</code>
      * @param providerToBeDeleted
      * The provider to be deleted.
      * @exception DeleteProviderException
      * Thrown if not such provider exists.  */
    public void deleteProvider(JainTcapProvider providerToBeDeleted)
        throws DeleteProviderException;
    /** @deprecated
      * As of JAIN TCAP v1.1. No replacement the attach of the
      * JainTcapProvider occurs implicitly within the createProvider
      * method of this interface.
      *
      * Deprecated attach and detach methods as the object reference
      * signifying the attach/detach can be associated/nullified with
      * the Provider upon creation and deletion.  */
    public void attach(JainTcapProvider jainTcapProvider)
        throws ProviderNotAttachedException;
    /** @deprecated
      * As of JAIN TCAP v1.1. No replacement the detach of the
      * JainTcapProvider occurs implicitly within the deleteProvider
      * method of this interface.  */
    public void detach(JainTcapProvider jainTcapProvider)
        throws ProviderNotAttachedException;
    /** Update v1.1: Changed return type of provider List from Vector to
      * Array. Returns the array of Peer JAIN TCAP Providers that have
      * been created by this JainTcapStackImpl. All of the Peer JAIN
      * TCAP Provider s of this JainTcapStackImpl will be proprietary
      * objects belonging to the same stack vendor.
      * @return A JainTcapProvider Array containing all the of Peer JAIN
      * TCAP Providers created by this JainTcapStackImpl.  */
    public JainTcapProvider[] getProviderList();
    /** Returns the name of the stack as a string.
      * @return
      * A string describing the Stack Name.  */
    public java.lang.String getStackName();
    /** Sets the name of the stack as a string.
      * @param stackName
      * The new Stack Name value.  */
    public void setStackName(java.lang.String stackName);
    /** Returns the Vendor's name for this stack.
      * @return
      * A string describing the Vendor's name.
      * @since JAIN TCAP v1.1 */
    public java.lang.String getVendorName();
    /** Sets the Vendors name for this stack, this name will be the
      * Vendor's domain name inverted i.e, "com.sun" similar to the path
      * name of the JAIN Factory.
      * @param vendorName
      * The new Vendor's Name.
      * @since JAIN TCAP v1.1 */
    public void setVendorName(java.lang.String vendorName);
    /** @deprecated
      * As of JAIN TCAP v1.1. This method has been replaced by
      * getStackSpecification in the JainTcapStack Interface. Note:
      * These get/set protocolVersion methods were deprecated to avoid
      * confusion between the stack standards supported and the protocol
      * version field supported by the ANSI 1996 Dialogue Portion.  */
    public int getProtocolVersion();
    /** @deprecated
      * As of JAIN TCAP v1.1. This method has been replaced by
      * setStackSpecification in the JainTcapStack Interface.  */
    public void setProtocolVersion(int protocolVersion)
        throws TcapException;
    /** Gets the stack Specification that this Stack is currently
      * supporting. <p>
      * <em>Note to developers:-</em> This should not be confused with
      * the protocol version field supported by the ANSI 1996 Dialogue
      * Portion.
      * @return
      * The current Protocol Version of this stack. This may be one of
      * the following values: <ul>
      * <li>STACK_SPECIFICATION_ANSI_92
      * <li>STACK_SPECIFICATION_ANSI_96
      * <li>STACK_SPECIFICATION_ITU_93
      * <li>STACK_SPECIFICATION_ITU_97 </ul>
      * These values are defined in the TcapConstants class.
      * @since JAIN TCAP v1.1
      * Note: These get/set protocolVersion methods were deprecated to
      * avoid confusion between the stack standards supported and the
      * protocol version field supported by the ANSI 1996 Dialogue
      * Portion.  */
    public int getStackSpecification();
    /** Sets the Stack specification that this Stack is to support.
      * Invoking this method should initially configure the stack to
      * support the given specification if the specification supported
      * has not already been set. In the event that this Stack is
      * already supporting a specification other than the supplied
      * specification, then the Stack will change the supported
      * specification if possible, or throw an Exception if this is not
      * possible. <p>
      * <em>Note to developers:-</em> This should not be confused with
      * the protocol version field supported by the ANSI 1996 Dialogue
      * Portion.
      * @param stackSpecification
      * One of the following values: <ul>
      * <li>STACK_SPECIFICATION_ANSI_92
      * <li>STACK_SPECIFICATION_ANSI_96
      * <li>STACK_SPECIFICATION_ITU_93
      * <li>STACK_SPECIFICATION_ITU_97 </ul>
      * @exception VersionNotSupportException
      * Thrown if the supplied stack specification cannot be supported
      * by this Stack, or if the Stack cannot change supported stack
      * specifications because it is not in a idle state.
      * @since JAIN TCAP v1.1
      * @see TcapConstants */
    public void setStackSpecification(int stackSpecification)
        throws VersionNotSupportedException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
