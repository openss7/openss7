//  ==========================================================================
//  
//  @(#) $Id$
//  
//  --------------------------------------------------------------------------
//  
//  Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
//  Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
//  Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>
//  
//  All Rights Reserved.
//  
//  This program is free software; you can redistribute it and/or modify it
//  under the terms of the GNU Affero General Public License as published by
//  the Free Software Foundation; version 3 of the License.
//  
//  This program is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
//  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public
//  License for more details.
//  
//  You should have received a copy of the GNU Affero General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>, or
//  write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
//  02139, USA.
//  
//  --------------------------------------------------------------------------
//  
//  U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
//  behalf of the U.S. Government ("Government"), the following provisions
//  apply to you.  If the Software is supplied by the Department of Defense
//  ("DoD"), it is classified as "Commercial Computer Software" under
//  paragraph 252.227-7014 of the DoD Supplement to the Federal Acquisition
//  Regulations ("DFARS") (or any successor regulations) and the Government is
//  acquiring only the license rights granted herein (the license rights
//  customarily provided to non-Government users).  If the Software is
//  supplied to any unit or agency of the Government other than DoD, it is
//  classified as "Restricted Computer Software" and the Government's rights
//  in the Software are defined in paragraph 52.227-19 of the Federal
//  Acquisition Regulations ("FAR") (or any successor regulations) or, in the
//  cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
//  (or any successor regulations).
//  
//  --------------------------------------------------------------------------
//  
//  Commercial licensing and support of this software is available from
//  OpenSS7 Corporation at a fee.  See http://www.openss7.com/
//  
//  --------------------------------------------------------------------------
//  
//  Last Modified $Date$ by $Author$
//  
//  --------------------------------------------------------------------------
//  
//  $Log$
//  ==========================================================================

package org.openss7.jain.protocol.ss7.tcap;

import java.util.*;

import jain.*;
import jain.protocol.ss7.*;
import jain.protocol.ss7.tcap.*;
// import jain.protocol.ss7.tcap.component.*;
// import jain.protocol.ss7.tcap.dialogue.*;

/**
  * This interface defines the methods required to represent a proprietary JAIN
  * TCAP protocol stack, the implementation of which will be vendor specific.
  * Each vendor's protocol stack will have an object that implements this
  * interface to control the creation/deletion of proprietary <CODE>
  * JainTcapProviders</CODE> and the implicit attaching/detaching of those
  * <CODE>JainTcapProviders</CODE> to this JainTcapStack implementation. <P>
  *
  * For applications that require some means to identify multiple stacks (with
  * multiple SPCs) setStackName() can be used. An application can choose to
  * supply any identifier to this method. <P>
  *
  * <B>Note</B> that a JainTcapStack represents a single SPC.
  *
  * <p><b>Implementation Notes:</b>
  * Note that this could have been a pure Java implementation of the
  * JainTcapStack interface.  However, there is some difficulty with a
  * JainTcapStack representing only a single SPC.  It appears that the
  * JainSS7Factory can be used to create more than one instance of this class.
  * <p>
  *
  * @author     Monavacom Limited
  * @version    $Revision$
  * @see        JainTcapStack
  */
public class JainTcapStackImpl implements JainTcapStack {
    private String vendorName = "org.openss7";
    private String stackName = "tcap";
    private int stackSpecification = TcapConstants.STACK_SPECIFICATION_ITU_97;
    private Vector providerList = new Vector();
    private int[] signalingPointCode = { 0, 0, 0 };

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
      * <i>Note to developers:</i> There is no setSignalingPointCode method
      * visible in the JainTcapStack Interface for security reasons. By hiding
      * the setSignalingPointCode method from applications, 3rd party vendors
      * application developers will not be able to configure the signaling Point
      * Code of a specific stack implemetation of the JAIN TCAP API, unless they
      * use the JAIN OAM specification which provides a standard interface into
      * provisioning the TCAP Layer of a SS7 stack.
      * <BR>
      * A developer can alternatively implement a setSignalingPointCode method
      * behind the JainTcapStack Interface and allow proprietary JAIN TCAP
      * applications to cast this interface to their implementation in order to
      * provision the Signaling Point Code.
      *
      * <p><b>Implementation Notes:</b>
      * The above would be fine and dandy; however, the JAIN OAM specification
      * never made it.  Rather than dork with this interface, OpenSS7 has
      * implemented the draft JAIN OAM specifications as was originally
      * intended. <p>
      *
      * Also, it would have been smarter to use the JAIN SS7
      * <CODE>SS7Address</CODE> interface and JAIN SS7
      * <CODE>SignalingPointCode</CODE> object here instead of this poorly
      * defined <CODE>int[]</CODE>. <p>
      *
      * The specification does not specify which signalling point code needs to
      * be returned here.  We have the ability to wildcard the signalling point
      * code.  When a response to a dialogue is provided we can assign the
      * responding signalling point code to be the destination point code of the
      * dialogue.  When a dialogue is initiated with a linked operation, the
      * originating point code can be the responding point code of the linked
      * operation.  When an unlinked dialogue is initiated, the originating
      * point code (in the simplest case) can be determined by the SCCP stack
      * based on the remote subsystem.  For finer grained control, the
      * application can use the setSignallingPointCode() method before calling
      * createProvider().
      *
      * So, we simply return whatever was set.  If nothing was set, we return a
      * point code containing all zeros. <p>
      *
      * If a JainTcapProviderImpl instance is created when the
      * signalingPointCode is set to all zeros, it will perform a wildcard bind
      * (if possible).  If a JainTcapProviderImpl() instance is created when the
      * signalingPointCode is set to some other value, and if that value is
      * invalid, the createProvider() method will throw an exception.
      *
      * @return the signalingPointCode of this JainTcapStack.
      */
    public int[] getSignalingPointCode() {
        int[] signalingPointCode = this.signalingPointCode;

        return signalingPointCode;
    }

    public void setSignalingPointCode(int[] signalingPointCode) {
        this.signalingPointCode[0] = signalingPointCode[0];
        this.signalingPointCode[1] = signalingPointCode[1];
        this.signalingPointCode[2] = signalingPointCode[2];
    }

    /**
      * Creates a new Peer (vendor specific) <CODE>JainTcapProvider</CODE> whose
      * reference is exposed by this JainTcapStackImp. This method replaces the
      * createAttachedProvider and createDetachedProvider methods, which have
      * become deprecated in this interface.
      *
      * <BR>
      * <i>Note to developers:</i> The implementation of this method should add
      * the newly created <CODE>JainTcapProvider</CODE> to the <a
      * href="JainTcapStack.html#getProviderList()">providerList</a> once the
      * <CODE>JainTcapProvider</CODE> has been successfully created and set the
      * JainTcapStack reference in the newly create
      * <code>JainTcapProvider</code> to <b>this</b> object. <p>
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
      * @return Peer JAIN TCAP Provider referenced by this JainTcapStackImpl.
      * @exception PeerUnavailableException  thrown if the class is not found
      * for a specific peer implementation
      * @since JAIN TCAP v1.1
      */
    public JainTcapProvider createProvider()
        throws PeerUnavailableException {
        JainTcapProviderImpl provider;

        try {
            provider = new JainTcapProviderImpl(this);
            providerList.add(provider);
        } catch (Exception e) {
            throw new PeerUnavailableException("Could not create JAIN TCAP provider.");
        }
        return provider;
    }

    /**
      * @deprecated As of JAIN TCAP v1.1. This method has been replaced by the
      * {@link #createProvider() createProvider} method in this Interface.
      */
    public JainTcapProvider createAttachedProvider()
        throws PeerUnavailableException, ProviderNotAttachedException {
        return createProvider();
    }

    /**
      * @deprecated As of JAIN TCAP v1.1. This method has been replaced by the
      * {@link #createProvider() createProvider} method in this Interface.
      */
    public JainTcapProvider createDetachedProvider()
        throws PeerUnavailableException {
        return createProvider();
    }

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
    public void deleteProvider(JainTcapProvider providerToBeDeleted)
        throws DeleteProviderException {
        JainTcapProviderImpl providerImpl = (JainTcapProviderImpl) providerToBeDeleted;

        providerList.remove(providerImpl);
        // providerImpl.cleanup();
    }

    /**
      * Deprecated attach and detach methods as the object reference signifying
      * the attach/detach can be associated/nullified with the Provider upon
      * creation and deletion.
      *
      * @deprecated As of JAIN TCAP v1.1. No replacement the attach of the
      * JainTcapProvider occurs implicitly within the
      * {@link #createProvider() createProvider} method of this interface.
      */
    public void attach(JainTcapProvider jainTcapProvider)
        throws ProviderNotAttachedException {
    }

    /**
      * @deprecated As of JAIN TCAP v1.1. No replacement the detach of the
      * JainTcapProvider occurs implicitly within the {@link
      * #deleteProvider(JainTcapProvider) deleteProvider} method of this
      * interface.
      */
    public void detach(JainTcapProvider jainTcapProvider)
        throws ProviderNotAttachedException {
    }

    /**
      * Update v1.1: Changed return type of provider List from Vector to Array.
      *
      * Returns the array of Peer JAIN TCAP Providers that have been created by
      * this JainTcapStackImpl. All of the Peer JAIN TCAP Provider s of this
      * JainTcapStackImpl will be proprietary objects belonging to the same
      * stack vendor.
      *
      * @return  an JainTcapProvider Array containing all the of Peer JAIN TCAP
      * Providers created by this JainTcapStackImpl.
      */
    public JainTcapProvider[] getProviderList() {
        JainTcapProvider[] providerList = new JainTcapProvider[this.providerList.size()];
        Enumeration providerListEnum = this.providerList.elements();

        for (int i = 0; providerListEnum.hasMoreElements(); i++) {
            providerList[i] = (JainTcapProvider) providerListEnum.nextElement();
        }
        return providerList;
    }

    /**
      * Returns the name of the stack as a string.
      *
      * <p><b>Implementation Notes:</b>
      * This limitation that a stack only have one SPC is a severe one.
      *
      * @return a string describing the Stack Name
      */
    public String getStackName() {
        return this.stackName;
    }

    /**
      * Sets the name of the stack as a string.
      *
      * @param stackProtocol The new Stack Name value
      */
    public void setStackName(String stackName) {
        this.stackName = stackName;
    }

    /**
      * Returns the Vendor's name for this stack.
      *
      * <p><b>Implementation Notes:</b>
      * This appears to be the only method for handling multiple stacks.
      *
      * @return  a string describing the Vendor's name
      * @since JAIN TCAP v1.1
      */
    public String getVendorName() {
        return this.vendorName;
    }

    /**
      * Sets the Vendors name for this stack, this name will be the Vendor's
      * domain name inverted i.e. "com.sun" similar to the path name of the JAIN
      * Factory.
      *
      * <p><b>Implementation Notes:</b>
      * Why on earth would one want to allow the user to set the vendor name?
      *
      * @param vendorName  The new Vendor's Name
      * @since JAIN TCAP v1.1
      */
    public void setVendorName(String vendorName) {
        this.vendorName = vendorName;
    }

    /**
      * @deprecated    As of JAIN TCAP v1.1. This method has been replaced by
      * {@link #getStackSpecification() getStackSpecification} in the
      * JainTcapStack Interface.
      *
      * Note: These get/set protocolVersion methods were deprecated to avoid
      * confusion between the stack standards supported and the protocol version
      * field supported by the ANSI 1996 Dialogue Portion
      */
    public int getProtocolVersion() {
        return getStackSpecification();
    }

    /**
      * @deprecated As of JAIN TCAP v1.1. This method has been replaced by
      * {@link #setStackSpecification(int) setStackSpecification} in the
      * JainTcapStack Interface.
      */
    public void setProtocolVersion(int protocolVersion)
        throws TcapException {
        try {
            setStackSpecification(protocolVersion);
        } catch (VersionNotSupportedException e) {
            throw new TcapException("Protocol version " + protocolVersion + "not supported");
        }
    }

    /**
      * Gets the stack Specification that this Stack is currently supporting.
      * <p>
      *
      * <b>Note to developers</b> :- This should not be confused with the
      * protocol version field supported by the ANSI 1996 Dialogue Portion.
      *
      * @return the current Protocol Version of this stack. This may be one of
      * the following values:
      *      <UL>
      *        <LI> STACK_SPECIFICATION_ANSI_92
      *        <LI> STACK_SPECIFICATION_ANSI_96
      *        <LI> STACK_SPECIFICATION_ITU_93
      *        <LI> STACK_SPECIFICATION_ITU_97
      *      </UL>
      *      These values are defined in the TcapConstants class.
      * @since JAIN TCAP v1.1
      *
      * Note: These get/set protocolVersion methods were deprecated to avoid
      * confusion between the stack standards supported and the protocol version
      * field supported by the ANSI 1996 Dialogue Portion
      */
    public int getStackSpecification() {
        return this.stackSpecification;
    }

    /**
      * Sets the Stack specification that this Stack is to support. Invoking
      * this method should initially configure the stack to support the given
      * specification if the specification supported has not already been set.
      * In the event that this Stack is already supporting a specification other
      * than the supplied specification, then the Stack will change the
      * supported specification if possible, or throw an Exception if this is
      * not possible.  <p>
      *
      * <b>Note to developers</b> :- This should not be confused with the
      * protocol version field supported by the ANSI 1996 Dialogue Portion.
      *
      * <p><b>Implementation Notes:</b>
      * This is also poor.  I am rather disappointed with my colleagues
      * specification of this interface.  They have set each stack to only have
      * one point code.  Setting the specification for an entire point code
      * means that one SCCP subsystem cannot have a different TCAP protocol
      * running on it than another SCCP subsystem.  Because the protocol variant
      * has been broken out by year this is too restrictive.  As it stands, we
      * cannot have one subsystem (e.g. HLR) that runs ANSI 92 and another
      * subsystem (e.g. VLR) that runs ANSI 96. <p>
      *
      * Also, the list above is too restrictive.  For example, Japan has a
      * different specification of an ITU-based TCAP.  Even within ANSI there is
      * the plain jane ANSI format and then there is the Bellcore (Telecordia)
      * version. <p>
      *
      * Furthermore, the OpenSS7 TCAP stack runs multiple protocol variants
      * simultaneously. <p>
      *
      * Disappointing. <p>
      *
      * My workaround for this difficulty is to allow the application to set the
      * specification at any time.  By allowing the specification can be changed
      * at any time, the value set will only be effective when
      * <CODE>createProvider()</CODE> is called.  We will then create a provider
      * of the appropriate variant. <p>
      *
      * This is superior to implementations such as NewNet's.
      *
      * @param  stackSpecification one of the following values:
      *      <UL>
      *        <LI> STACK_SPECIFICATION_ANSI_92
      *        <LI> STACK_SPECIFICATION_ANSI_96
      *        <LI> STACK_SPECIFICATION_ITU_93
      *        <LI> STACK_SPECIFICATION_ITU_97
      *      </UL>
      * @exception VersionNotSupportException thrown if the supplied stack
      * specification cannot be supported by this Stack, or if the Stack cannot
      * change supported stack specifications because it is not in a idle state.
      * @since JAIN TCAP v1.1
      * @see TcapConstants
      */
    public void setStackSpecification(int stackSpecification)
        throws VersionNotSupportedException {
        switch (stackSpecification) {
            case TcapConstants.STACK_SPECIFICATION_ANSI_92:
            case TcapConstants.STACK_SPECIFICATION_ANSI_96:
            case TcapConstants.STACK_SPECIFICATION_ITU_93:
            case TcapConstants.STACK_SPECIFICATION_ITU_97:
                this.stackSpecification = stackSpecification;
                break;
            default:
                throw new VersionNotSupportedException("Stack specification " + stackSpecification + " not supported.");
        }
    }
}

// vim: ft=java tw=72 sw=4 et com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
