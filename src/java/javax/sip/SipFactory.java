/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
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
 
 Last Modified $Date$ by $Author$
 */

package javax.sip;

import javax.sip.address.*;
import javax.sip.header.*;
import javax.sip.message.*;

/**
    The SipFactory is a singleton class which applications can use a single access point to obtain
    proprietary implementations of this specification. As the SipFactory is a singleton class there
    will only ever be one instance of the SipFactory. The single instance of the SipFactory can be
    obtained using the getInstance() method. If an instance of the SipFactory already exists it will
    be returned to the application, otherwise a new instance will be created. A peer implementation
    object can be obtained from the SipFactory by invoking the appropriate create method on the
    SipFactory e.g. to create a peer SipStack, an application would invoke the
    createSipStack(Properties) method.  <h5>Naming Convention</h5> Note that the SipFactory utilises
    a naming convention defined by this specification to identify the location of proprietary
    objects that implement this specification. The naming convention is defined as follows: <ul>
    <li> The upper-level package structure referred to by the SipFactory with the attribute pathname
    can be used to differentiate between proprietary implementations from different SIP stack
    vendors. The pathname used by each SIP vendor must be the domain name assigned to that vendor in
    reverse order. For example, the pathname used by Sun Microsystem's would be com.sun.  <li> The
    lower-level package structure and classname of a peer object is also mandated by this
    specification. The lowel-level package must be identical to the package structure defined by
    this specification and the classname is mandated to the interface name appended with the Impl
    post-fix. For example, the lower-level package structure and classname of a proprietary
    implementation of the javax.sip.SipStack interface must be javax.sip.SipStackImpl. </ul> Using
    this naming convention the SipFactory can locate a vendor's implementation of this specification
    without requiring an application to supply a user defined string to each create method in the
    SipFactory. Instead an application merely needs to identify the vendors SIP implementation it
    would like to use by setting the pathname of that vendors implementation. <p> It follows that a
    proprietary implementation of a peer object of this specification can be located at: <p> <code>
    'pathname'.'lower-level package structure and classname'. </code> <p> For example an application
    can use the SipFactory to instantiate a NIST peer SipStack object by setting the pathname to
    gov.nist and calling the createSipStack method. The SipFactory would return a new instance of
    the SipStack object at the following location: gov.nist.javax.sip.SipStackImpl.java Because the
    space of domain names is managed, this scheme ensures that collisions between two different
    vendor's implementations will not happen. For example: a different vendor with a domain name
    'bea.com' would have their peer SipStack object located at com.bea.javax.sip.SipStackImpl.java.
    <p> <h5>Default Namespace:</h5> This specification defines a default namespace for the
    SipFactory, this namespace is the location of the Reference Implementation. The default
    namespace is gov.nist the author of the Reference Implementation, therefore the pathname will
    have the initial value of gov.nist for a new instance of the SipFactory. An application must set
    the pathname of the SipFactory on retrieval of a new instance of the factory in order to use a
    different vendors SIP stack from that of the Reference Implementation. An application can not
    mix different vendor's peer implementation objects.
    @version 1.2.2
    @author Monavacon Limited
  */
public class SipFactory extends java.lang.Object {
    /**
        Returns an instance of a SipFactory. This is a singleton class so this method is the global
        access point for the SipFactory.
        @return The single instance of this singleton SipFactory.
      */
    public synchronized static SipFactory getInstance() {
        if (m_jainSipFactory != null)
            return m_jainSipFactory;
        m_jainSipFactory = new SipFactory();
        return m_jainSipFactory;
    }
    /**
        Creates an instance of a SipStack implementation based on the configuration properties
        object passed to this method. The recommended behaviour is to not specify an
        "javax.sip.IP_ADDRESS" property in the Properties argument, in this case the
        "javax.sip.STACK_NAME" uniquely identifies the stack. A new stack instance will be returned
        for each different stack name associated with a specific vendor implementation.  The
        ListeningPoint is used to configure the IP Address argument. For backwards compatability, if
        a "javax.sip.IP_ADDRESS" is supplied, this method ensures that only one instance of a
        SipStack is returned to the application for that IP Address, independent of the number of
        times this method is called. Different SipStack instances are returned for each different IP
        address. <p> See SipStack for the expected format of the properties argument.
        @exception PeerUnavailableException Thrown when the peer class could not be found.
      */
    public SipStack createSipStack(java.util.Properties properties)
        throws PeerUnavailableException {
        try {
            Class peerObjectClass = Class.forName(getPathName() + ".javax.sip.SipStackImpl");
            java.lang.Object newPeerObject = peerObjectClass.newInstance();
            return (SipStack) newPeerObject;
        } catch (Exception e) {
        }
        throw new PeerUnavailableException("Peer: " + m_pathName + ".javax.sip.SipStackImpl unavailable.");
    }
    /**
        Creates an instance of the MessageFactory implementation. This method ensures that only one
        instance of a MessageFactory is returned to the application, no matter how often this method
        is called.
        @exception PeerUnavailableException Thrown when peer class could not be found.
      */
    public synchronized MessageFactory createMessageFactory()
        throws PeerUnavailableException {
        if (m_messageFactory != null)
            return m_messageFactory;
        try {
            Class peerObjectClass = Class.forName(getPathName() + ".javax.sip.message.MessageFactoryImpl");
            java.lang.Object newPeerObject = peerObjectClass.newInstance();
            m_messageFactory = (MessageFactory) newPeerObject;
            return m_messageFactory;
        } catch (Exception e) {
        }
        throw new PeerUnavailableException("Peer: " + m_pathName + ".javax.sip.message.MessageFactoryImpl unavailable.");
    }
    /**
        Creates an instance of the HeaderFactory implementation. This method ensures that only one
        instance of a HeaderFactory is returned to the application, no matter how often this method
        is called.
        @exception PeerUnavailableException Thrown when peer class could not be found.
      */
    public synchronized HeaderFactory createHeaderFactory()
        throws PeerUnavailableException {
        if (m_headerFactory != null)
            return m_headerFactory;
        try {
            Class peerObjectClass = Class.forName(getPathName() + ".javax.sip.header.HeaderFactoryImpl");
            java.lang.Object newPeerObject = peerObjectClass.newInstance();
            m_headerFactory = (HeaderFactory) newPeerObject;
            return m_headerFactory;
        } catch (Exception e) {
        }
        throw new PeerUnavailableException("Peer: " + m_pathName + ".javax.sip.header.HeaderFactoryImpl unavailable.");
    }
    /**
        Creates an instance of the AddressFactory implementation. This method ensures that only one
        instance of an AddressFactory is returned to the application, no matter how often this
        method is called.
        @exception PeerUnavailableException Thrown when peer class could not be found.
      */
    public synchronized AddressFactory createAddressFactory()
        throws PeerUnavailableException {
        if (m_addressFactory != null)
            return m_addressFactory;
        try {
            Class peerObjectClass = Class.forName(getPathName() + ".javax.sip.address.AddressFactoryImpl");
            java.lang.Object newPeerObject = peerObjectClass.newInstance();
            m_addressFactory = (AddressFactory) newPeerObject;
            return m_addressFactory;
        } catch (Exception e) {
        }
        throw new PeerUnavailableException("Peer: " + m_pathName + ".javax.sip.header.AddressFactoryImpl unavailable.");
    }
    /**
        Sets the pathname that identifies the location of a particular vendor's implementation of
        this specification. The pathname must be the reverse domain name assigned to the vendor
        providing the implementation. An application must call resetFactory() before changing
        between different implementations of this specification.
        @param pathName The reverse domain name of the vendor, e.g., Sun Microsystem's would be
        'com.sun'.
      */
    public synchronized void setPathName(java.lang.String pathName) {
        m_pathName = pathName;
    }
    /**
        Returns the current pathname of the SipFactory. The pathname identifies the location of a
        particular vendor's implementation of this specification as defined the naming convention.
        The pathname must be the reverse domain name assigned to the vendor providing this
        implementation. This value is defaulted to gov.nist the location of the Reference
        Implementation.
        @return The string identifying the current vendor implementation.
      */
    public synchronized java.lang.String getPathName() {
        return m_pathName;
    }
    /**
        This method reset's the SipFactory's references to the object's it has created. It allows
        these objects to be garbage collected assuming the application no longer holds references to
        them.  This method must be called to reset the factories references to a specific vendors
        implementation of this specification before it creates another vendors implementation of
        this specification by changing the pathname of the SipFactory.
      */
    public synchronized void resetFactory() {
        m_messageFactory = null;
        m_headerFactory = null;
        m_addressFactory = null;
    }
    private static SipFactory m_jainSipFactory = null;
    private static java.lang.String m_pathName = "com.sun";
    private static MessageFactory m_messageFactory = null;
    private static HeaderFactory m_headerFactory = null;
    private static AddressFactory m_addressFactory = null;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
