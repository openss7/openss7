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

import jain.protocol.ss7.tcap.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines the methods required to represent a
  * proprietary JAIN TCAP protocol stack, the implementation of which
  * will be vendor specific.  Each vendor's protocol stack will have an
  * object that implements this interface to control the
  * creation/deletion of proprietary <code> JainTcapProviders</code> and
  * the implicit attaching/detaching of those
  * <code>JainTcapProviders</code> to this JainTcapStack implementation.
  * <p>
  *
  * It must be noted that under the <b>JAIN Naming Convention</b> the
  * lower-level package structure and classname of a proprietary
  * implementation of the <i>jain.protocol.ss7.tcap.JainTcapStack</i>
  * interface <b>must</b> be
  * <i>jain.protocol.ss7.tcap.JainTcapStackImpl</i> . <p>
  *
  * Under the JAIN naming convention, the <b>upper-level package
  * structure</b> (pathname) can be used to differentiate between
  * proprietary implementations from different SS7 Vendors. The pathname
  * used by each SS7 Vendor <b>must be</b> the domain name assigned to
  * the Vendor in reverse order, e.g. Sun Microsystem's would be
  * 'com.sun' <p>
  *
  * It follows that a proprietary implementation of a JainTcapStack will
  * be located at: <br>
  * <i>&lt;pathname&gt;<b>.</b>jain.protocol.ss7.tcap.JainTcapStackImpl</i>
  * <br> <b>Where:</b> <br> <i>pathname</i> = reverse domain name, e.g.
  * com.sun' <br> The resulting Peer JAIN SS7 Object would be located
  * at: <i><b>com.sun</b> jain.protocol.ss7.tcap.JainTcapStackImpl</i>
  * <br> An application may create a JainTcapStackImpl by invoking the
  * JainSs7Factory.createJainSS7Object() method. The <b>PathName</b> of
  * the vendor specific implementation of which you want to instantiate
  * can be set before calling this method or the default or current
  * pathname may be used. <p>
  *
  * For applications that require some means to identify multiple stacks
  * (with multiple SPCs) setStackName() can be used. An application can
  * choose to supply any identifier to this method. <p>
  *
  * <b>Note</b> that a JainTcapStack represents a single SPC.
  *
  * @author     Monavacon Limited
  * @version    1.2.2
  * @see        JainTcapStack
  * @see        JainTcapProvider
  */
public class JainTcapStackImpl implements java.rmi.Remote, JainTcapStack {
    /**
      * The constructor.
      * This default constructor is called by JainSS7Factory when an
      * instance of this vendor-implementation object is instantiated.
      * <p>
      *
      * There may be multiple instalces of this class.  Each instance
      * represents a unique combination of Signalling Point Code, Stack
      * Specification and Vendor Name.  Because the application may not
      * have sufficient knowledge to set these values, each new instance
      * of this class will default to a new unique and supported
      * combination of the three values.  If no more unique combinations
      * exist, the constructor should fail.
      */
    public JainTcapStackImpl() {
        m_spc = new int[3];
        m_spc[0] = m_spc[1] = m_spc[2] = 0;
        m_spcIsSet = true;
        m_stackSpecification = TcapConstants.STACK_SPECIFICATION_ITU_97;
        m_stackSpecificationIsSet = true;
        m_vendorName = "org.openss7";
        m_vendorNameIsSet = true;
    }
    /**
      * The Signaling Point Code can be implemented in one of the
      * following 4 ways <ul>
      * <li>ITU_14bit - Split 3-8-3 across member/cluster/zone.
      * <li>ANSI_24bit - Split 8-8-8 across member/cluster/zone.
      * <li>Chinese_24bit - Split 8-8-8 across member/cluster/zone.
      * <li>Japanese_16bit - Split 5-4-7 across member/cluster/zone.</ul>
      *
      * <i>Note to developers:</i><br> There is no setSignalingPointCode
      * method visible in the JainTcapStack Interface for security
      * reasons. By hiding the setSignalingPointCode method from
      * applications, 3rd party vendors application developers will not
      * be able to configure the signaling Point Code of a specific
      * stack implemetation of the JAIN TCAP API, unless they use the
      * JAIN OAM specification which provides a standard interface into
      * provisioning the TCAP Layer of a SS7 stack. <p>
      *
      * A developer can alternatively implement a setSignalingPointCode
      * method behind the JainTcapStack Interface and allow propreitory
      * JAIN TCAP applications to cast this interface to their
      * implementation in order to provision the Signaling Point Code.
      *
      * @return
      * the signalingPointCode of this JainTcapStack.
      */
    public int[] getSignalingPointCode() {
        return m_spc;
    }

    public void setSignalingPointCode(int[] signalingPointCode) {
        int nmember, cluster, network;
        try { nmember = signalingPointCode[0]; } catch (Exception e) { nmember = 0; }
        try { cluster = signalingPointCode[1]; } catch (Exception e) { cluster = 0; }
        try { network = signalingPointCode[2]; } catch (Exception e) { network = 0; }
        m_spc = new int[3];
        m_spc[0] = nmember;
        m_spc[1] = cluster;
        m_spc[2] = network;
        m_spcIsSet = true;
    }
    /**
      * Creates a new Peer (vendor specific)
      * <CODE>JainTcapProvider</CODE> whose reference is exposed by this
      * JainTcapStackImp.
      * This method replaces the createAttachedProvider and
      * createDetachedProvider methods, which have become deprecated in
      * this interface. <p>
      *
      * <i>Note to developers:</i><br> The implementation of this method
      * should add the newly created <CODE>JainTcapProvider</CODE> to
      * the {@link JainTcapStack.html#getProviderList()} once the
      * <CODE>JainTcapProvider</CODE> has been successfully created and
      * set the JainTcapStack reference in the newly create
      * <code>JainTcapProvider</code> to <b>this</b> object. <p>
      *
      * <b>Example:-</b><br>
      * <b>public</b> JainTcapProvider createProvider() <b>throws</b> PeerUnavailableException {
      *    <BLOCKQUOTE><b>try</b> {
      *        <BLOCKQUOTE>  JainTcapProviderImpl myProvider = <b>new</b> JainTcapProviderImpl();</BLOCKQUOTE>
      *    } <b>catch</b>(ClassNotFoundException err) {
      *        <BLOCKQUOTE><b>throw new</b> ss7PeerUnavailableException("Couldn't find peer"); </BLOCKQUOTE>
      *    } <BR>
      *    myProvider.myStack = <b>this</b>;<BR>
      *    <b>return</b> myProvider; </BLOCKQUOTE>
      * } <p>
      *
      * @return
      * Peer JAIN TCAP Provider referenced by this JainTcapStackImpl.
      *
      * @exception PeerUnavailableException
      * thrown if the class is not found for a specific peer
      * implementation
      *
      * @since JAIN TCAP v1.1
      */
    public JainTcapProvider createProvider()
        throws PeerUnavailableException {
        if (m_spcIsSet == false)
            throw new PeerUnavailableException("Signaling Point Code not set.");
        if (m_stackSpecificationIsSet == false)
            throw new PeerUnavailableException("Stack Specification not set.");
        if (m_vendorNameIsSet == false)
            throw new PeerUnavailableException("Vendor Name not set.");
        JainTcapProviderImpl tcapProvider;
        try {
            tcapProvider = new JainTcapProviderImpl(this);
            m_providerList.add(tcapProvider);
        } catch (Exception e) {
            throw new PeerUnavailableException("Cannot create provider.");
        }
        return tcapProvider;
    }
    /**
      * @deprecated
      * As of JAIN TCAP v1.1. This method has been replaced by the
      * {@link #createProvider() createProvider} method in this
      * Interface.
      */
    public JainTcapProvider createAttachedProvider()
        throws PeerUnavailableException, ProviderNotAttachedException {
        return createProvider();
    }
    /**
      * @deprecated
      * As of JAIN TCAP v1.1. This method has been replaced by the
      * {@link #createProvider() createProvider} method in this
      * Interface.
      */
    public JainTcapProvider createDetachedProvider()
        throws PeerUnavailableException {
        return createProvider();
    }
    /**
      * Deletes the specified Peer JAIN TCAP Provider attached to this
      * JainTcapStackImpl. <p>
      *
      * <i>Note to developers:</i><br> The implementation of this method
      * should remove the specified Peer JAIN TCAP Provider from the
      * {@link * #getProviderList()}. <p>
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
      * @param providerToBeDeleted
      * the provider to be deleted
      *
      * @exception DeleteProviderException
      * thrown if not such provider exists
      */
    public void deleteProvider(JainTcapProvider providerToBeDeleted)
        throws DeleteProviderException {
        int size = m_providerList.size();
        JainTcapProviderImpl tcapProvider = (JainTcapProviderImpl) providerToBeDeleted;
        m_providerList.remove(tcapProvider);
        if (size == m_providerList.size())
            throw new DeleteProviderException("Provider not deleted.");
        tcapProvider.finalize();
    }
    /**
      * Deprecated attach and detach methods as the object reference
      * signifying the attach/detach can be associated/nullified with
      * the Provider upon creation and deletion.
      *
      * @deprecated
      * As of JAIN TCAP v1.1. No replacement the attach of the
      * JainTcapProvider occurs implicitly within the
      * {@link #createProvider() createProvider} method of this
      * interface.
      */
    public void attach(JainTcapProvider jainTcapProvider)
        throws ProviderNotAttachedException {
        throw new ProviderNotAttachedException("Deprecated function.");
    }
    /**
      * @deprecated
      * As of JAIN TCAP v1.1. No replacement the detach of the
      * JainTcapProvider occurs implicitly within the
      * {@link #deleteProvider(JainTcapProvider) deleteProvider}
      * method of this interface.
      */
    public void detach(JainTcapProvider jainTcapProvider)
        throws ProviderNotAttachedException {
        throw new ProviderNotAttachedException("Deprecated function.");
    }
    /**
      * Update v1.1: Changed return type of provider List from Vector to
      * Array. <p>
      *
      * Returns the array of Peer JAIN TCAP Providers that have been
      * created by this JainTcapStackImpl. All of the Peer JAIN TCAP
      * Provider s of this JainTcapStackImpl will be proprietary objects
      * belonging to the same stack vendor.
      *
      * @return
      * an JainTcapProvider Array containing all the of Peer JAIN TCAP
      * Providers created by this JainTcapStackImpl.
      */
    public JainTcapProvider[] getProviderList() {
        JainTcapProvider[] providerList = new JainTcapProvider[m_providerList.size()];
        Iterator i = m_providerList.iterator();
        for (int n=0; i.hasNext(); n++) {
            providerList[n] = (JainTcapProvider)i.next();
        }
        return providerList;
    }
    /**
      * Returns the name of the stack as a string
      *
      * @return
      * a string describing the Stack Name
      */
    public String getStackName() {
        return m_stackName;
    }
    /**
      * Sets the name of the stack as a string
      *
      * @param stackProtocol
      * The new Stack Name value
      */
    public void setStackName(String stackName) {
        m_stackName = stackName;
        m_stackNameIsSet = true;
    }
    /**
      * Returns the Vendor's name for this stack
      *
      * @return
      * a string describing the Vendor's name
      *
      * @since JAIN TCAP v1.1
      */
    public String getVendorName() {
        return m_vendorName;
    }
    /**
      * Sets the Vendors name for this stack, this name will be the
      * Vendor's domain name inverted i.e, "com.sun" similar to the path
      * name of the JAIN Factory.
      *
      * @param vendorName
      * The new Vendor's Name
      *
      * @since JAIN TCAP v1.1
      */
    public void setVendorName(String vendorName) {
        m_vendorName = vendorName;
        m_vendorNameIsSet = true;
    }
    /**
      * @deprecated
      * As of JAIN TCAP v1.1. This method has been replaced by
      * {@link #getStackSpecification() getStackSpecification} in the
      * JainTcapStack Interface. <p>
      *
      * Note: These get/set protocolVersion methods were deprecated to
      * avoid confusion between the stack standards supported and the
      * protocol version field supported by the ANSI 1996 Dialogue
      * Portion
      */
    public int getProtocolVersion() {
        return getStackSpecification();
    }
    /**
      * @deprecated
      * As of JAIN TCAP v1.1. This method has been replaced by
      * {@link #setStackSpecification(int) setStackSpecification} in the
      * JainTcapStack Interface.
      */
    public void setProtocolVersion(int protocolVersion)
        throws TcapException {
        try {
            setStackSpecification(protocolVersion);
        } catch (VersionNotSupportedException e) {
            throw new TcapException("Protocol version " + protocolVersion + " not supported.");
        }
    }
    /**
      * Gets the stack Specification that this Stack is currently
      * supporting. <p>
      *
      * <b>Note to developers:-</b><br> This should not be confused with
      * the protocol version field supported by the ANSI 1996 Dialogue
      * Portion.
      *
      * @return
      * The current Protocol Version of this stack. This may be one of
      * the following values: <ul>
      * <li> STACK_SPECIFICATION_ANSI_92
      * <li> STACK_SPECIFICATION_ANSI_96
      * <li> STACK_SPECIFICATION_ITU_93
      * <li> STACK_SPECIFICATION_ITU_97 </ul>
      * These values are defined in the TcapConstants class.
      *
      * @since JAIN TCAP v1.1
      *
      * Note: These get/set protocolVersion methods were deprecated to
      * avoid confusion between the stack standards supported and the
      * protocol version field supported by the ANSI 1996 Dialogue
      * Portion
      */
    public int getStackSpecification() {
        return m_stackSpecification;
    }
    /**
      * Sets the Stack specification that this Stack is to support.
      * Invoking this method should initially configure the stack to
      * support the given specification if the specification supported
      * has not already been set. In the event that this Stack is
      * already supporting a specification other than the supplied
      * specification, then the Stack will change the supported
      * specification if possible, or throw an Exception if this is not
      * possible. <p>
      *
      * <b>Note to developers:-</b><br>
      * This should not be confused with the protocol version field
      * supported by the ANSI 1996 Dialogue Portion.
      *
      * @param stackSpecification
      * One of the following values: <ul>
      * <li> STACK_SPECIFICATION_ANSI_92
      * <li> STACK_SPECIFICATION_ANSI_96
      * <li> STACK_SPECIFICATION_ITU_93
      * <li> STACK_SPECIFICATION_ITU_97 </ul>
      *
      * @exception VersionNotSupportException
      * Thrown if the supplied stack specification cannot be supported
      * by this Stack, or if the Stack cannot change supported stack
      * specifications because it is not in a idle state.
      *
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
                m_stackSpecification = stackSpecification;
                m_stackSpecificationIsSet = true;
                break;
            default:
                throw new VersionNotSupportedException("Stack specification " + stackSpecification + " not supported.");
        }
    }

    /** The set signalling point code. */
    private int[] m_spc = new int[3];
    /** Whether the signalling point code is user-set. */
    private boolean m_spcIsSet = false;
    /** The set stack name. */
    private String m_stackName = "tcap";
    /** Whether the stack name is user-set. */
    private boolean m_stackNameIsSet = false;
    /** The set vendor name. */
    private String m_vendorName = "org.openss7";
    /** Whether the vendor name is user-set. */
    private boolean m_vendorNameIsSet = false;
    /** The set stack specification. */
    private int m_stackSpecification = TcapConstants.STACK_SPECIFICATION_ITU_97;
    /** Whether the stack specification is user set. */
    private boolean m_stackSpecificationIsSet = false;
    /** A provider list. */
    private Vector m_providerList = new Vector();
}

// vim: ft=java tw=72 sw=4 et com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS
