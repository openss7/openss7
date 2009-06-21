/*
 @(#) $RCSfile: JainIPFactory.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:16 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:16 $ by $Author: brian $
 */

package jain.protocol.ip;

/**
    The JAIN IP Factory is a class through which applications obtain a proprietary (Peer) JAIN IP
    Stack Object. The createObject method of this class provides this capability. To use this
    method, the Path Name must first be set to indicate which specific vendor's implementation
    should be used for instantiating the JAIN IP Stack Object. For example, Sun Microsystem's
    implementation would be identified by the Path Name "com.sun". Then, the Class Name for the Peer
    Object you would like to create must be supplied. For example, the Class Name for a
    JainMgcpStack Object would be "jain.protocol.ip.mgcp.JainMgcpStackImpl".  It should be noted
    that any object that implements the: JainXXXXStack interface - is referred to as
    JainXXXXStackImpl JainXXXXProvider interface - is referred to as JainXXXXProviderImpl where
    "XXXX" identifies a specific JAIN IP protocol; e.g., "XXXX" = "Mgcp" for the Media Gateway
    Control Protocol (MGCP).  The term 'Peer' is Java nomenclature for "a particular
    platform-specific implementation of a Java interface or API." This term has the same meaning for
    the JAIN IP API specifications.  Applications are not permitted to create an instance of the
    JainIPFactory class. Through an installation procedure provided by each implementor, a 'Peer
    JAIN IP' class is made available to an application environment. When applications have
    instantiated a Peer JAIN IP Stack Object for a particular platform-dependent implementation,
    they may obtain a JAIN IP Provider object via the interface implemented by the Stack object.
    The JAIN IP API specifications restrict the Path Name each vendor uses and rely on the
    conventions used for managing package names.  The Class Name must begin with the domain name
    assigned to the vendor in reverse order. Because the space of domain names is managed, this
    scheme ensures that collisions between two different vendor implementations will not happen. For
    example, an implementation from Sun Microsystems, Inc. will have "com.sun" as the prefix to its
    Peer's Class Names; i.e., JainMgcpStackImpl would be fully qualified by
    'com.sun.jain.protocol.ip.mgcp.JainMgcpStackImpl'.
    @version 1.2.2
    @author Monavacon Limited
  */
public class JainIPFactory extends java.lang.ClassLoader {
    /**
        Returns an instance of a Peer JAIN IP Object, given the Class Name of the class for which
        the object is being instantiated.  An example of a Class Name is
        "jain.protocol.ip.mgcp.JainMgcpStackImpl".  Proprietary implementations must adhere to the
        following naming convention: a class that implements a JAIN IP API interface with the name
        "InterfaceName" must be named "InterfaceNameImpl". For example, a vendor implementation of
        the JainMgcpStack interface must be called "JainMgcpStackImpl".  The "PathName" must be set
        before invoking this method, so that the correct proprietary implementation will be used in
        the instantiation. If not set explicitly, "PathName" will be set to "com.sun".
        @param objectClassName The Class Name of the object to be instantiated.
        @exception IPPeerUnavailableException Thrown when the specified class does not exist or is
        not installed in the CLASSPATH.
      */
    public java.lang.Object createIPObject(java.lang.String objectClassName)
        throws IPPeerUnavailableException {
        if (objectClassName != null) {
            try {
                Class peerObjectClass = Class.forName(getPathName() + "." + objectClassName);
                java.lang.Object newPeerObject = peerObjectClass.newInstance();
                m_jainObjectList.addElement(newPeerObject);
                return (newPeerObject);
            } catch (Exception e) {
            }
        }
        throw new IPPeerUnavailableException("Peer: " + m_pathName + "." + objectClassName + " unavailable.");
    }
    /**
        This method locates, loads, and defines the desired class. It is a protected method intended
        only for the use of the createIPObject method of the JainIPFactory.
        @param fqClassName The fully-qualified name of the class, formed of the stored path name and
        the objectClassName that was passed as a parameter to the createIPObject method, joined by a
        "." character.
      */
    protected java.lang.Class findClass(java.lang.String fqClassName)
        throws java.lang.ClassNotFoundException {
        super.findClass(fqClassName);
    }
    /**
        Returns an instance of a JainIPFactory. This is a singleton-type class, so this method is
        the only way to access it. Note that this method is static.
      */
    public static JainIPFactory getInstance() {
        return m_ipFactory;
    }
    /**
        Returns a vector of Peer JAIN IP Objects that have been created by this instance of the JAIN
        IP Factory.
        @return A vector of Peer JAIN IP Objects.
      */
    public java.util.Vector getJainIPObject() {
        return m_jainObjectList;
    }
    /**
        The Path Name specifies where the JainIPFactory can find a specific vendor's implementation
        of the JAIN IP API specification for the factory to instantiate the objects using the
        createIPObject method. The 'pathName' string will take the form 'com.CompanyName'. The
        default pathName that will be returned if none was set is 'com.sun'. 'pathName' will be all
        lower case.
        @return The Path Name as a string.
      */
    public java.lang.String getPathName() {
        return m_pathName;
    }
    /**
        The Path Name specifies where the JainIPFactory can find a specific vendor's implementation
        of the JAIN IP API specification for the factory to instantiate the objects using the
        createIPObject method.
        @param pathName The 'pathName' string must take the form 'com.CompanyName'. The default
        pathName is 'com.sun'. The stored path name will be forced to lower case.
      */
    public void setPathName(java.lang.String pathName) {
        m_pathName = pathName;
    }
    private JainIPFactory() {
        super();
    }
    private static JainIPFactory m_ipFactory = new JainIPFactory();
    private static java.lang.String m_pathName = "com.sun";
    private static java.util.Vector m_jainObjectList = new java.util.Vector();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
