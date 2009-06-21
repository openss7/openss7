/*
 @(#) $RCSfile: JainSS7Factory.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:23 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:23 $ by $Author: brian $
 */

package jain.protocol.ss7;

import jain.*;

/**
  * The JAIN(tm) SS7 Factory is a singleton class by which JAIN SS7
  * applications can obtain a proprietary (Peer) JAIN SS7 Object.
  * The term 'peer' is Java nomenclature for "a particular
  * platform-specific implementation of a Java interface or API". This
  * term has the same meaning for the JAIN SS7 API specifications.  A
  * Peer JAIN SS7 Object can be obtained from the JAIN SS7 Factory by
  * ethier: <ul>
  *
  * <li>instantiating a new Peer JAIN SS7 Object using the
  * createSS7Object() method, or
  *
  * <li>selecting from the list of JAIN SS7 Objects returned by the
  * getJainObjectList() method. </ul>
  *
  * The JAIN SS7 Factory utilises a naming convention defined for each
  * JAIN SS7 API to identify the location of proprietary JAIN SS7
  * Objects. <p>
  *
  * Under this convention the lower-level package structure and
  * classname of a Peer JAIN SS7 Object is mandated by a convention
  * defined within the JAIN SS7 API from which the Object originates.
  * <p>
  *
  * For example: within the JAIN TCAP API, the lower-level package
  * structure and classname of a proprietary implementation of the
  * jain.protocol.ss7.tcap.JainTcapStack interface must be
  * jain.protocol.ss7.tcap.JainTcapStackImpl. <p>
  *
  * Under the JAIN naming convention, the upper-level package structure
  * (pathname) can be used to differentiate between proprietary
  * implementations from different SS7 Vendors. The pathname used by
  * each SS7 Vendor must be the domain name assigned to the Vendor in
  * reverse order, e.g. Sun Microsystem's would be 'com.sun' <p>
  *
  * It follows that a proprietary implementation of a JAIN SS7 Object
  * can be located at: 'pathname'.'lower-level package structure and
  * classname' <p>
  *
  * Where: <p>
  *
  * pathname = reverse domain name, e.g. 'com.sun' <p>
  *
  * lower-level package structure and classname = mandated naming
  * convention for the JAIN SS7 Object in question <p>
  *
  * e.g. jain.protocol.ss7.tcap.JainTcapStackImpl is the mandated naming
  * convention for jain.protocol.ss7.tcap.JainTcapStack. <p>
  *
  * The resulting Peer JAIN SS7 Object would be located at:
  * com.sun.jain.protocol.ss7.tcap.JainTcapStackImpl Because the space
  * of domain names is managed, this scheme ensures that collisions
  * between two different vendor's implementations will not happen. For
  * example: a different Vendor with a domain name 'foo.com' would have
  * their Peer JainTcapStack Object located at
  * com.foo.jain.protocol.ss7.tcap.JainTcapStackImpl.  This is a similar
  * concept to the JAVA conventions used for managing package names. <p>
  *
  * The pathname is defaulted to 'com.sun' but may be set using the
  * setPathName() method. This allows a JAIN application to switch
  * between different Vendor implementations, as well as providing the
  * capability to use the default or current pathname. <p>
  *
  * The JAIN SS7 Factory is a Singleton class. This means that there
  * will only be one instance of the class with a global point of access
  * to it.  The single instance of the JAIN SS7 Factory can be obtained
  * using the getInstance() method. In this way, the JAIN SS7 Factory
  * can acts a single point obtaining JAIN SS7 Objects.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class JainSS7Factory {
    /**
      * Returns an instance of a JainSS7Factory.
      * This is a singleton type class so this method is the global
      * access point for the JainSS7Factory.
      * @return
      * The single instance of this singleton JainSS7Factory.
      */
    public synchronized static JainSS7Factory getInstance() {
        if (m_jainSS7Factory != null)
            return m_jainSS7Factory;
        m_jainSS7Factory = new JainSS7Factory();
        return m_jainSS7Factory;
    }
    /**
      * Sets the Pathname that identifies the location of a particular
      * Vendor's implementation of the JAIN SS7 Objects.
      * The pathname must be the domain name assigned to the Vendor
      * providing the implementation in reverse order.
      * @param pathName
      * The reverse domain name of the Vendor. e.g.  Sun Microsystem's would be 'com.sun'.
      */
    public void setPathName(java.lang.String pathName)
        throws InvalidPathNameException {
        m_pathName = pathName;
    }
    /**
      * Returns the current Pathname.
      * The pathname identifies the location of a particular Vendor's
      * implementation of the JAIN SS7 Objects. The pathname will always
      * be the domain name assigned to the Vendor providing the
      * implementation in reverse order.
      * @return
      * The pathname.
      */
    public java.lang.String getPathName() {
        return m_pathName;
    }
    /**
      * Returns an instance of a Peer JAIN SS7 Object identified by the
      * supplied classname.
      * This supplied classname is the lower-level package structure and
      * classname of the required Peer JAIN SS7 Object. This classname
      * is mandated by a convention defined within the JAIN SS7 API from
      * which the Object originates. <p>
      *
      * For example: To create an instance of the
      * jain.protocol.ss7.tcap.JainTcapStack from the JAIN TCAP API, the
      * supplied classname will be
      * jain.protocol.ss7.tcap.JainTcapStackImpl. <p>
      *
      * Under the JAIN naming convention, the upper-level package
      * structure (pathname) can be used to differentiate between
      * proprietary implementations from different SS7 Vendors. <p>
      *
      * The location of the returned JAIN SS7 Object is dependent on the
      * current pathname and the supplied classname. <p>
      *
      * If the specified class does not exist or is not installed in the
      * CLASSPATH a PeerUnavailableException exception is thrown. <p>
      *
      * Once a Peer SS7 Object is created an object reference to the
      * newly created object is stored along with object refernces to
      * any other Peer SS7 Objects that have previously been created by
      * this JainSS7Factory. This list of Peer SS7 Objects may be
      * retrieved at any time using the getJainObjectList() method.
      *
      * @param objectClassName
      * Lower-level package structure and classname of the Peer JAIN SS7
      * Object class that is mandated by a convention defined within the
      * JAIN SS7 API from which the Object originates.
      * @return
      * An instance of a Peer JAIN SS7 Object.
      * @exception PeerUnavailableException
      * Indicates that the Peer JAIN SS7 Object specified by the
      * classname and the current pathname cannot be located.
      */
    public java.lang.Object createSS7Object(java.lang.String objectClassName)
        throws PeerUnavailableException {
        if (objectClassName != null) {
            try {
                Class peerObjectClass = Class.forName(getPathName() + "." + objectClassName);
                java.lang.Object newPeerObject = peerObjectClass.newInstance();
                m_jainObjectList.addElement(newPeerObject);
                return (newPeerObject);
            } catch (Exception e) {
            }
        }
        throw new PeerUnavailableException("Peer: " + m_pathName + "." + objectClassName + " unavailable.");
    }
    /**
      * Returns a Vector containing all of the Peer JAIN Objects that
      * have previously been created using this JainSS7Factory. <p>
      *
      * NOTE: The Vector returned may contain Objects of any type that
      * may have been created through this factory.
      * @return
      * A vector containing all JAIN SS7 Objects created through this
      * JAIN SS7 Factory.
      */
    public java.util.Vector getJainObjectList() {
        return m_jainObjectList;
    }

    protected static JainSS7Factory m_jainSS7Factory = null;
    protected java.lang.String m_pathName = "com.sun";
    protected java.util.Vector m_jainObjectList = new java.util.Vector();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
