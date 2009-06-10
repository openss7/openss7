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

package javax.jain.protocol.ss7;

import javax.jain.*;

/**
  * The JAIN SS7 Factory is a class by which applications obtain
  * instances of proprietary (Peer) JAIN SS7 Objects.
  * In the Jain MAP API the factory is used to instantiate stack objects
  * for the different capabilities of the Jain MAP API. <p>
  *
  * The createSS7Object method of this factory is used. It is necessary
  * to first set the Path Name to the specific vendor's implementation
  * of JAIN SS7, e.g. Sun Microsystem's would be 'com.sun'. The fully
  * qualified Class Name for the Peer Object to be created must be
  * provided as a parameter in the createSS7Object method, e.g. a
  * JainMapMessageTransactionStack object would be
  * 'jain.protocol.ss7.map.transaction.JainMapMessageTransactionStackImpl'.
  * <p>
  *
  * The following naming convention is used: an object that implements
  * the JainMapStack interface is referred to as JainMapStackImpl. <p>
  *
  * Note: Some Jain API's use object specific creation methods in the
  * factory but the Jain MAP API only relies on the generic
  * createSS7Object method. <p>
  *
  * The term 'peer' is Java nomenclature for "a particular
  * platform-specific implementation of a Java interface or API". This
  * term has the same meaning for the JAIN SS7 API specifications. <p>
  *
  * The JAIN SS7 API specifications restrict the Path Name each vendor
  * uses and relies on the conventions used for managing package names.
  * The class name must begin with the domain name assigned to the
  * vendor in reverse order. Because the space of domain names is
  * managed, this scheme ensures that collisions between two different
  * vendor's implementations will not happen. For example, an
  * implementation from Sun Microsystem's will have "com.sun" as the
  * prefix to its' Peers fully qualified Class Names, for example
  * JainMapMessageTransactionStackImpl would be
  * 'com.sun.jain.protocol.ss7.map.transaction.JainMapMessageTransactionStackImpl'.
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class JainSS7Factory {
    /**
      * Returns an instance of a JainSS7Factory.
      * This is a singleton type class so this method is the global
      * access point for the JainSS7Factory.
      *
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
      *
      * Sets the pathname that identifies the location of a particular
      * vendor's implementation of the JAIN SS7 Objects.
      * The pathname must be the domain name assigned to the vendor
      * providing the implementation in reverse order.
      *
      * @param pathName
      * A string with the path name, e.g. "com.sun".
      */
    public void setPathName(String pathName) {
        m_pathName = pathName;
    }
    /**
      * Returns the current Pathname.
      * The pathname identifies the location of a particular vendor's
      * implementation of the JAIN SS7 Objects. The pathname will always
      * be the domain name assigned to the vendor providing the
      * implementation in reverse order.
      *
      * @return
      * A string containgin the path name, e.g. "com.sun".
      */
    public String getPathName() {
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
      * jain.protocol.ss7.map.transaction.JainMapMessageTransactionStack
      * from the JAIN MAP API, the supplied classname shall be
      * jain.protocol.ss7.map.transaction.JainMapMessageTransactionStackImpl.
      * <p>
      *
      * Under the JAIN naming convention, the upper-level package
      * structure (referred to as the "path name") can be used to
      * differentiate between proprietary implementations from different
      * SS7 vendors. <p>
      *
      * The location of the returned JAIN SS7 Object is dependent on the
      * current path name and the supplied classname. <p>
      *
      * If the specified class does not exist or is not installed in the
      * CLASSPATH a SS7PeerUnavailableException exception is thrown. <p>
      *
      * Once a Peer SS7 Object is created an object reference to the
      * newly created object is stored along with object refernces to
      * any other Peer SS7 Objects that have previously been created by
      * this JainSS7Factory. This list of Peer SS7 Objects may be
      * retrieved at any time using the getJainObjectList method.
      *
      * <h4>Implementation Notes:</h4>
      * We have really fooled the compiler here.  It is impossible for
      * the compiler to know which exceptions are thrown by the class,
      * an instance of which is being created.  Note that this
      * javax.jain.protcol.ss7.JainSS7Factory has a slightly different
      * interface than the  jain.protocol.ss7.JainSS7Factory that is
      * more standard (part of the released JAIN TCAP).  There is
      * essentially nothing stopping the later from being used instead
      * of this factory when creating objects.
      *
      * @param objectClassName
      * A string containing the lower-level package structure and
      * classname of the Peer JAIN SS7 Object class that is mandated by
      * a convention defined within the JAIN SS7 API from which the
      * Object originates.
      *
      * @return
      * A new instance of the specified Peer JAIN SS7 Object class.
      *
      * @exception SS7PeerUnavailableException
      * Indicates that the Peer JAIN SS7 Object specified by the
      * classname and the current pathname cannot be located.
      */
    public synchronized Object createSS7Object(String objectClassName)
        throws SS7PeerUnavailableException {
        if (objectClassName != null) {
            try {
                Class peerObjectClass = Class.forName(getPathName() + "." + objectClassName);
                Object newPeerObject = peerObjectClass.newInstance();
                m_jainObjectList.addElement(newPeerObject);
                return (newPeerObject);
            } catch (Exception e) {
            }
        }
        throw new SS7PeerUnavailableException("Peer: " + m_pathName + "." + objectClassName + " unavailable.");
    }
    /**
      * Returns a Vector containing all of the Peer JAIN Objects that
      * have previously been created using this JainSS7Factory.
      *
      * @return
      * A vector containing all JAIN SS7 Objects created through this
      * JAIN SS7 Factory. The Vector returned may contain objects of any
      * type that may have been created through this factory.
      */
    public synchronized java.util.Vector getJainObjectList() {
        return m_jainObjectList;
    }

    protected static JainSS7Factory m_jainSS7Factory;
    protected String m_pathName = "com.sun";
    protected java.util.Vector m_jainObjectList = new java.util.Vector();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
