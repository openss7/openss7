/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

package org.openss7.javax.jain.ss7.isup;

import java.util.*;

import javax.jain.ss7.isup.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This interface defines methods for representing a proprietary JAIN ISUP protocol stack
  * implementation.
  * This interface and the JainIsupProvider interface will be build by the vendor
  * implementing the JAIN ISUP API.  Each vendor's protocol stack will have an object that
  * implements this interface to control the creation/deletion of proprietary
  * JainIsupProvider objects and the attaching and detaching of that JainIsupProvider to
  * this JainIsupStack implementation.  As per JAIN naming convention, the lower level
  * package structure and class name of the proprietary implementation of the
  * <i>javax.jain.ss7.isup.JainIsupStack</i> interface <b>must</b> be
  * <i>javax.jain.isup.JainIsupStackImpl</i>.  For distinguishing between the
  * implementations of different vendors, the uper level package structure is used, that
  * should be the domain assigned to the vendor in reverse order.  This ia also as per
  * JAIN naming convention.  As an example, OpenSS7's implementation of JainIsupStack
  * interface would be <i>org.openss7.javax.jain.ss7.isup.JainIsupStackImpl</i>.  The
  * proprietary implementation of a JainIsupStack will be located at;
  * <em>pathname.</em><i>javax.jain.ss7.isup</i> where <em>pathname</em> is the reverse
  * domain name; e.g. <i>org.openss7</i>. <p>
  *
  * An application can create a JainIsupStackImpl object by calling the
  * JainSS7Factory.createJainSS7Object method by specifying the object name after setting
  * the <em>pathname</em> by invoking the JainSS7Factory.setPathName method. <p>
  *
  * <em>Note:</em><br> A JainIsupStack represents a single point code. <p>
  *
  * <em>Implementation Notes:</em><br>
  * This class doesn't really do anything.  All that it does is provide a mechainsm
  * whereby the user can specify some properties of the ISUP stack before creating a
  * provider with those properties.  The properties that can be specified include the
  * network indicator, signalling point code, stack specification (protocol variant), and
  * vendor name.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class JainIsupStackImpl implements java.rmi.Remote, JainIsupStack {
    /**
      * The default constructor.
      * There may be multiple instances of this class.  Each instance
      * represents a unique combination of NetworkIndicator,
      * SignalingPointCode, and StackSpecification.  Because the
      * application may not have sufficient knowledge to set these
      * values, each new instance of this class will default to a new
      * unique and supported combination of the three values.  If no
      * more unique combinations exist, the constructor should fail.
      */
    public JainIsupStackImpl() {
        /* set defaults for now */
        m_nwInd = NetworkIndicator.NI_INTERNATIONAL_00;
        m_nwIndIsSet = true;
        m_spc = new SignalingPointCode(0,0,0);
        m_spcIsSet = true;
        m_stackSpec = IsupConstants.ISUP_PV_ITU_1993;
        m_stackSpecIsSet = true;
        m_vendorName = "org.openss7";
        m_vendorNameIsSet = true;
    }
    /**
      * Creates a vendor specific implementation (Peer) of JainIsupProvider and returns
      * its reference.
      * This newly created provider is then attached to the JainIsupStackImpl object to
      * send and receive messages between the Provider and the stack.  <p>
      *
      * <em>Note to Developers:</em><br> The implementation of this method should add the
      * newly created JainIsupProvider object to the getProviderList.
      *
      * @return
      * The newly created JAIN ISUP Provider.
      *
      * @exception ParameterNotSetException
      * Thrown if the stack has not been initialized with either signalling point code or
      * network indicator stack specification or vendor name.
      */
    public JainIsupProvider createProvider()
        throws ParameterNotSetException {
        if (m_nwIndIsSet == false)
            throw new ParameterNotSetException("Network Indicator not set.");
        if (m_spcIsSet == false)
            throw new ParameterNotSetException("Signaling Point Code not set.");
        if (m_stackSpecIsSet == false)
            throw new ParameterNotSetException("Stack Specificaiton not set.");
        if (m_vendorNameIsSet == false)
            throw new ParameterNotSetException("Vendor Name not set.");
        JainIsupProviderImpl isupProvider;
        try {
            isupProvider = new JainIsupProviderImpl(this);
            m_providerList.add(isupProvider);
        } catch (Exception e) {
            throw new ParameterNotSetException("Cannot create provider.");
        }
        return isupProvider;
    }
    /**
      * Deletes a vendor specific implemetnation (Peer) of JainIsupProvider.
      * This is a complementary operation to the createProvider method.  This provider is
      * first detsached from the JainIsupStackImpl object and then deleted. <p>
      *
      * <em>Note to Developers:</em><br> The implementation of this method should delete
      * the specified JainIsupProvider object from the getProviderList.
      *
      * @param providerToBeDeleted
      * The Peer JAIN ISUP Provider to be deleted.
      *
      * @exception DeleteProviderException
      * Thrown when the Peer Provider cannot be deleted because it has already been
      * deleted, or was never registered.
      */
    public void deleteProvider(JainIsupProvider providerToBeDeleted)
        throws DeleteProviderException {
        int size = m_providerList.size();
        JainIsupProviderImpl isupProvider = (JainIsupProviderImpl) providerToBeDeleted;
        m_providerList.remove(isupProvider);
        if (size == m_providerList.size())
            throw new DeleteProviderException();
        isupProvider.finalize();
    }
    /**
      * Returns a list of JAIN ISUP Providers that have been created by this
      * JainIsupStackImpl.
      * The Providers may be in attached or detached condition.  All the Peer Providers
      * are proprietary objects belonging to the same stack vendor.
      */
    public JainIsupProvider[] getProviderList() {
        JainIsupProvider[] providerList = new JainIsupProvider[m_providerList.size()];
        Iterator i = m_providerList.iterator();
        for (int n=0; i.hasNext(); n++) {
            providerList[n] = (JainIsupProvider)i.next();
        }
        return providerList;
    }
    /**
      * Sets the Signaling point code of this stack.
      * If the signallign point code parameter has been already set the createProvider
      * method has been called to attach the Provider with the underlying stack, then
      * calling the setSignalingPointCode method would throw the
      * ParameterAlreadySetException.
      *
      * @param spc
      * The signaling point code.
      *
      * @exception ParameterRangeInvalidException
      * Thrown when a value is out of range.
      *
      * @exception ParameterAlreadySetException
      * Thrown when the signalling point code has already been set and createProvider
      * method has been called with that Signaling Point Code.
      */
    public void setSignalingPointCode(SignalingPointCode spc)
        throws ParameterRangeInvalidException, ParameterAlreadySetException {
        if (m_providerList.size() != 0)
            throw new ParameterAlreadySetException();
        // TODO: need to check signaling point code for validity?
        m_spc = spc;
        m_spcIsSet = true;
    }
    /**
      * The Signalling Point Code will uniquely identify the JainIsupStack.
      * The Signaling Point Code is the mechanism of addressing a node in the SS7 network.
      * Refer to SignalingPointCode for details.
      *
      * @return
      * The SignalingPointCode of this JainIsupStack.
      *
      * @exception ParameterNotSetException
      * Thrown when the signaling point code is not set.
      */
    public SignalingPointCode getSignalingPointCode()
        throws ParameterNotSetException {
        if (m_spcIsSet)
            return m_spc;
        throw new ParameterNotSetException("Signaling Point Code not set.");
    }
    /**
      * Sets the name of the stack as a string.
      *
      * @param stackName
      * The stack name.
      */
    public void setStackName(java.lang.String stackName) {
        m_stackName = stackName;
        m_stackNameIsSet = true;
    }
    /**
      * Gets the name of the stack as a string.
      *
      * @return
      * The stack name.
      */
    public java.lang.String getStackName() {
        return m_stackName;
    }
    /**
      * Sets the vendor name for this stack.
      * This name will be the vendor's domain name inverted.  So, when OpenSS7 is the SS7
      * stack vendor, then vendor name will be <code>"org.openss7"</code> (similar to the
      * path name of the JAIN Factory).
      *
      * @param vendorName
      * The vendor's name.
      *
      * @exception ParameterAlreadySetException
      * Thrown when the vendor name is already set, and createProvider has been called for
      * that vendor name.
      */
    public void setVendorName(java.lang.String vendorName)
        throws ParameterAlreadySetException {
        if (m_providerList.size() != 0)
            throw new ParameterAlreadySetException();
        m_vendorName = vendorName;
        m_vendorNameIsSet = true;
    }
    /**
      * Gets the vendor name of this stack.
      * This name will be the vendor's domain name inverted.  So, when OpenSS7 is the SS7
      * stack vendor, then vendor name will be <code>"org.openss7"</code> (similar to the
      * path name of the JAIN Factory).
      *
      * @return
      * The vendor's name for this stack.
      */
    public java.lang.String getVendorName() {
        return m_vendorName;
    }
    /**
      * Gets the stack specification.
      *
      * @return
      * The stack specification that this stack is currently supporting.  This may be one
      * of the following values;<ul>
      * <li><code>IsupConstants.ISUP_PV_ANSI_1992</code>
      * <li><code>IsupConstants.ISUP_PV_ANSI_1995</code>
      * <li><code>IsupConstants.ISUP_PV_ITU_1993</code></ul>
      */
    public int getStackSpecification() {
        return m_stackSpec;
    }
    /**
      * Sets the stack specification.
      * Invoking this method should intially configure the stack to support this
      * specification.  If the stack is already supporting a specification, then an
      * exception will be thrown.  The stack is considered to be already supporting a
      * specification when the createProvider method has already been called to attach the
      * Provider to the underlying stack.
      *
      * @param stackSpec
      * The stack specification.  This may be one of the following values;<ul>
      * <li><code>IsupConstants.ISUP_PV_ANSI_1992</code>
      * <li><code>IsupConstants.ISUP_PV_ANSI_1995</code>
      * <li><code>IsupConstants.ISUP_PV_ITU_1993</code></ul>
      *
      * @exception VersionNotSupportedException
      * Thrown when the protocol version is out of range or unsupported.
      *
      * @exception ParameterAlreadySetException
      * Thrown wen the stack specification has been already set and the createProvider
      * method has been called for that Stack Specification.
      */
    public void setStackSpecification(int stackSpec)
        throws VersionNotSupportedException, ParameterAlreadySetException {
        if (m_providerList.size() != 0)
            throw new ParameterAlreadySetException();
        switch (stackSpec) {
            case IsupConstants.ISUP_PV_ANSI_1992:
            case IsupConstants.ISUP_PV_ANSI_1995:
            case IsupConstants.ISUP_PV_ITU_1993:
                m_stackSpec = stackSpec;
                m_stackSpecIsSet = true;
                break;
            default:
                throw new VersionNotSupportedException("Stack Specification " + stackSpec + " out of range.");
        }
    }
    /**
      * Sets the network indicator.
      * @param nwInd
      * The network indicator.<ul>
      * <li><code>NetworkIndicator.NI_INTERNATIONAL_00</code> Network indicator international.
      * <li><code>NetworkIndicator.NI_INTERNATIONAL_01</code> Network indicator international spare.
      * <li><code>NetworkIndicator.NI_NATIONAL_10</code> Network indicator national.
      * <li><code>NetworkIndicator.NI_NATIONAL_11</code> Network indicator national reserved.</ul>
      *
      * @exception ParameterRangeInvalidException
      * Thrown when the value is out of range.
      *
      * @exception ParameterAlreadySetException
      * Thrown when the network indicator is already set and the createProvider method has
      * been called for that Network Indicator.
      */
    public void setNetworkIndicator(NetworkIndicator nwInd)
        throws ParameterRangeInvalidException, ParameterAlreadySetException {
        if (m_providerList.size() != 0)
            throw new ParameterAlreadySetException();
        switch (nwInd.intValue()) {
            case NetworkIndicator.M_NI_INTERNATIONAL_00:
            case NetworkIndicator.M_NI_INTERNATIONAL_01:
            case NetworkIndicator.M_NI_NATIONAL_10:
            case NetworkIndicator.M_NI_NATIONAL_11:
                m_nwInd = nwInd;
                m_nwIndIsSet = true;
                break;
            default:
                throw new ParameterRangeInvalidException("Network Indicator value " + nwInd + " out of range.");
        }
    }
    /**
      * Gets the Network Indicator.
      *
      * @return
      * The current Network Indicator.  This may be one of the following values;<ul>
      * <li><code>NetworkIndicator.NI_INTERNATIONAL_00</code> Network indicator
      * international.
      * <li><code>NetworkIndicator.NI_INTERNATIONAL_01</code> Network indicator
      * international spare.
      * <li><code>NetworkIndicator.NI_NATIONAL_10</code> Network indicator national.
      * <li><code>NetworkIndicator.NI_NATIONAL_11</code> Network indicator national
      * reserved.</ul>
      */
    public NetworkIndicator getNetworkIndicator() {
        return m_nwInd;
    }

    /** The set signalling point code. */
    private SignalingPointCode m_spc;
    /** Whether the signalling point code is user-set. */
    private boolean m_spcIsSet = false;
    /** The set stack name. */
    private java.lang.String m_stackName = "isup";
    /** Whether the stack name is user-set. */
    private boolean m_stackNameIsSet = false;
    /** The set vendor name. */
    private java.lang.String m_vendorName = "org.openss7";
    /** Whether the vendor name is user-set. */
    private boolean m_vendorNameIsSet = false;
    /** The set stack specification. */
    private int m_stackSpec;
    /** Whether the stack specification is user set. */
    private boolean m_stackSpecIsSet = false;
    /** The set network indicator. */
    private NetworkIndicator m_nwInd;
    /** Whether the network indicator is user set. */
    private boolean m_nwIndIsSet = false;
    /** A provider list. */
    private Vector m_providerList = new Vector();
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
