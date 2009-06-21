/*
 @(#) $RCSfile: JccPeerFactory.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:33 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:33 $ by $Author: brian $
 */

package javax.csapi.cc.jcc;

/** The JccPeerFactory class is a class by which applications obtain a
  * JccProvider object. <p>
  *
  * <h5>Introduction</h5>
  * Applications use this class to first obtain a class which implements
  * the JccPeer interface. The JccPeer interface represents a particular
  * vendor's implementation of JCC. The term 'peer' is Java nomenclature
  * for "a particular platform-specific implementation of a Java
  * interface or API". This term has the same meaning for the Java Core
  * Package API.  Applications are not permitted to create an instance
  * of the JccPeerFactory class. Through an installation procedure
  * provided by each implementator, a JccPeer class is made available to
  * an application environment. When applications have a JccPeer object
  * for a particular platform-dependent implementation, they may obtain
  * a JccProvider object via that interface. The details of that
  * interface are discussed in the specification for the JccPeer
  * interface.
  *
  * <h5>Obtaining a JccPeer Object</h5>
  * Applications use the getJccPeer(java.lang.String) method to obtain a JccPeer
  * object. The argument to this method is a classname which represents
  * an object which implements the JccPeer interface. This object and
  * the classname under which it can be found must be supplied by the
  * vendor of the implementation. Note that this object is not a
  * JccProvider, however, this interface is used to obtain JccProvider
  * objects from that particular implementation. <p>
  *
  * The JCC places conventions on vendors on the classname they use for
  * their JccPeer object. This class name must begin with the domain
  * name assigned to the vendor in reverse order. Because the space of
  * domain names is managed, this scheme ensures that collisions between
  * two different vendor's implementations will not happen. For example,
  * an implementation from Sun Microsystem's will have "com.sun" as the
  * prefix to its JccPeer class. After the reversed domain name, vendors
  * are free to choose any class hierarchy they desire. <p>
  *
  * <h5>Default JccPeer</h5>
  * Additionally, the vendor providing the JccPeer class may supply a a
  * DefaultJccPeer.class class file. When placed in the classpath of
  * applications, this class (which must implement the JccPeer
  * interface) becomes the default JccPeer object returned by the
  * getJccPeer(java.lang.String) method. By convention the default class name must
  * be DefaultJccPeer. <p>
  *
  * In basic environments, applications and users do not want the burden
  * of finding out the class name in order to use a particular
  * implementation. Therefore, the JccPeerFactory class supports a
  * mechanism for applications to obtain the default implementation for
  * their system. If applications use a null argument to the
  * getJccPeer(java.lang.String) method, they will be returned the default
  * installed implementation on their system if it exists. <p>
  *
  * Note: It is the responsibility of implementation vendors to supply a
  * version of a DefaultJccPeer or some means to alias their peer
  * implementation along with a means to place that DefaultJccPeer class
  * in the application classpath.
  *
  * @since 1.0b
  * @version 1.2.2
  * @author Monavacon
  */
public class JccPeerFactory {
    /** Constructor for JccPeerFactory class. This is private because
      * applications are not permitted to create an instance of the
      * JccPeerFactory.  */
    private JccPeerFactory() {
    }
    /** Returns an instance of a JccPeer object given a fully qualified
      * classname of the class which implements the JccPeer object. <p>
      *
      * If no classname is provided (null), a default class named
      * DefaultJccPeer is chosen as the classname to load. If it does
      * not exist or is not installed in the CLASSPATH as the default, a
      * ClassNotFoundException exception is thrown.
      * @param JccPeerName
      * The classname of the JccPeer object class.
      * @return
      * An instance of the JccPeer object.
      * @exception java.lang.ClassNotFoundException
      * Indicates that the JccPeer specified by the classname is not
      * available.  */
    public static JccPeer getJccPeer(java.lang.String JccPeerName)
        throws java.lang.ClassNotFoundException {
        if (JccPeerName == null || JccPeerName.length() == 0) {
            JccPeerName = getDefaultJccPeerName();
        }
        if (JccPeerName == null) {
            throw new ClassNotFoundException();
        }
        try {
            Class JccPeerClass = Class.forName(JccPeerName);
            return (JccPeer)JccPeerClass.newInstance();
        } catch (Exception e) {
            java.lang.String errmsg = "JccPeer: " + JccPeerName + " could not be instantiated.";
            throw new ClassNotFoundException(errmsg);
        }
    }
    /** Returns a default JccPeer name. This method relies on the
      * existence of a default DefaultJccPeer.class in the application
      * classpath.  Note that the name DefaultJccPeer is a convention
      * enforced by this method.  Installations can alter this
      * convention by re-implementing this method.  This practice is in
      * general discouraged.
      * @return
      * The default Jcc peer name.  */
    private static java.lang.String getDefaultJccPeerName() {
        java.lang.String JccPeerName = "DefaultJccPeer";
        return JccPeerName;
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
