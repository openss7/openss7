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

package org.openss7.javax.jain.ss7.inap;

import java.io.*;
import java.util.*;
import java.lang.*;

import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/**
  * This interface defines the methods required to represent a
  * proprietary JAIN INAP protocol stack, the implementation of which
  * will be vendor specific. 
  * Each vendor's protocol stack will have an object that implements
  * this interface to control the creation/deletion of proprietary
  * <code>JainInapProvider</code> <p>
  *
  * It must be noted that any object that implements the: <ul>
  * <li>JainInapProvider Interface is referred to as JainInapProviderImpl.
  * <li>JainInapStack Interface is referred to as JainInapStackImpl. </ul>
  *
  * An application may create a JainInapStackImpl by invoking the
  * {@link javax.jain.ss7.JainSS7Factory#createSS7Object()} method on
  * the JainSS7Factory object. The <b>PathName</b> of the vendor
  * specific implementation of which you want to instantiate needs to be
  * set on the JainSS7Factory object using the
  * {@link javax.jain.ss7.JainSS7Factory#setPathName()} method before
  * calling the createSS7Object method.
  */
public class JainInapStackImpl implements java.rmi.Remote, JainInapStack {
    /**
      * Creates a new Peer (vendor specific) instance of the class
      * JainInapProviderImpl that is attached to this instance of
      * JainInapStackImpl.
      */
    public JainInapProvider createProvider()
        throws PeerUnavailableException {
        if (m_spcIsSet == false)
            throw new PeerUnavailableException("Signaling Point Code not set.");
        if (m_vendorNameIsSet == false)
            throw new PeerUnavailableException("Vendor Name not set.");
        JainInapProviderImpl inapProvider;
        try {
            inapProvider = new JainInapProviderImpl(this);
            m_providerList.add(inapProvider);
        } catch (Exception e) {
            throw new PeerUnavailableException("Cannot create provider.");
        }
        return inapProvider;
    }
    /**
      * Deletes the specified JainInapProviderImpl attached to this
      * JainInapStackImpl.
      *
      * @param providerToBeDeleted
      * The JainInapProviderImpl to be deleted from this
      * JainInapStackImpl.
      */
    public void deleteProvider(JainInapProvider providerToBeDeleted)
        throws DeleteProviderException {
        int size = m_providerList.size();
        JainInapProviderImpl inapProvider = (JainInapProviderImpl) providerToBeDeleted;
        m_providerList.remove(inapProvider);
        if (size == m_providerList.size())
            throw new DeleteProviderException("Provider not deleted.");
        inapProvider.finalize();
    }
    /**
      * Returns the vector of JainInapProviderImpls that have been
      * created by this JainInapStackImpl.
      * All of the JainInapProviderImpls of this JainInapStackImpl will
      * be proprietary objects belonging to the same stack vendor. Note
      * that the JainInapProviderImpls in this vector may be either
      * attached or detached.
      *
      * @return
      * A Vector containing all the of JainInapProviderImpls created by
      * this JainInapStackImpl.
      */
    public java.util.Vector getProviderList() {
        return m_providerList;
    }
    /**
      * Returns the name of the stack as a string.
      *
      * @return
      * A string describing the Stack Name.
      */
    public String getStackName() {
        return m_stackName;
    }
    /**
      * Sets the name of the stack as a string.
      *
      * @param stackName
      * The new Stack Name value.
      */
    public void setStackName(String stackName) {
        m_stackName = stackName;
        m_stackNameIsSet = true;
    }
    /**
      * Returns the Vendor's name for this stack.
      *
      * @return
      * A string describing the Vendor's name.
      */
    public String getVendorName() {
        return m_vendorName;
    }
    /**
      * Sets the Vendors name for this stack, this name may be the
      * Vendor's domain name i.e, "com.sun".
      */
    public void setVendorName(String vendorName) {
        m_vendorName = vendorName;
        m_vendorNameIsSet = true;
    }
    /**
      * Returns the Signalling Point Code of the JainInapStack.
      *
      * @return
      * The signalingPointCode of this JainInapStack.
      */
    public SignalingPointCode getSignalingPointCode() {
        return m_spc;
    }
    /**
      * Returns the Sub System Number of the JainInapStack.
      *
      * @return
      * The Sub System Number of this JainInapStack.
      */
    public int getSubSystemNumber() {
        return m_subsystemNumber;
    }

    private String m_stackName = "inap";
    private boolean m_stackNameIsSet = false;
    private String m_vendorName = "org.openss7";
    private boolean m_vendorNameIsSet = false;
    private SignalingPointCode m_spc = new SignalingPointCode(0,0,0);
    private boolean m_spcIsSet = false;
    private int m_subsystemNumber = 5;
    private boolean m_subsystemNumberIsSet = false;
    private java.util.Vector m_providerList = new java.util.Vector();
}

// vim: sw=4 et tw=90 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
