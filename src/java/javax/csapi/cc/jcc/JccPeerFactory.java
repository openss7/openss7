package javax.csapi.cc.jcc;

/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 2001, 2002 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  Telcordia Technologies, Inc.
 *  Farooq Anjum, John-Luc Bakker, Ravi Jain
 *  445 South Street
 *  Morristown, NJ 07960
 *
 *  Version       : 1.0b
 *  Notes         :
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/**
 * The <CODE>JccPeerFactory</CODE> class is a class by which applications
 * obtain a JccProvider object.
 * 
 * <H4> Introduction </H4>
 *
 * Applications use this class to first obtain a 
 * class which implements the {@link JccPeer} interface. The
 * <CODE>JccPeer</CODE> interface represents a particular vendor's
 * implementation of JCC.  The term 'peer' is Java nomenclature for
 * "a particular platform-specific implementation of a Java interface or API".
 * This term has the same meaning for the Java Core Package API. Applications are
 * not permitted to create an instance of the <CODE>JccPeerFactory</CODE>
 * class. Through an installation procedure provided by each implementator,
 * a <CODE>JccPeer</CODE> class is made available to an application
 * environment.  When applications have a JccPeer object for a particular 
 * platform-dependent implementation, they may obtain a {@link JccProvider} object via 
 * that interface. The details of that interface are discussed in the 
 * specification for the <CODE>JccPeer</CODE> interface.
 *
 * <H4>Obtaining a JccPeer Object</H4>
 *
 * Applications use the {@link #getJccPeer(String)} method to
 * obtain a JccPeer object. The argument to this method is a classname
 * which represents an object which implements the <CODE>JccPeer</CODE>
 * interface. This object and the classname under which it can be found must
 * be supplied by the vendor of the implementation. Note that this object is
 * not a JccProvider, however, this interface is used to obtain JccProvider objects
 * from that particular implementation.
 * <p>
 * The JCC places conventions on vendors on the classname
 * they use for their JccPeer object. This class name <EM>must</EM> begin
 * with the domain name assigned to the vendor in reverse order. Because the
 * space of domain names is managed, this scheme ensures that collisions
 * between two different vendor's implementations will not happen. For example,
 * an implementation from Sun Microsystem's will have "com.sun" as the prefix
 * to its JccPeer class. After the reversed domain name, vendors are free to
 * choose any class hierarchy they desire.
 *
 * <H4>Default JccPeer</H4>
 *
 * Additionally, the vendor providing the JccPeer class may supply a
 * a <CODE>DefaultJccPeer.class</CODE> class file. When placed in the 
 * classpath of applications, this class (which must implement the
 * <CODE>JccPeer</CODE> interface) becomes the default JccPeer object
 * returned by the {@link #getJccPeer(String)} method. By 
 * convention the default class name must be <CODE>DefaultJccPeer</CODE>.
 * <p>
 * In basic environments, applications and users do not want the burden of
 * finding out the class name in order to use a particular implementation.
 * Therefore, the <CODE>JccPeerFactory</CODE> class supports a mechanism for
 * applications to obtain the default implementation for their system. If
 * applications use a <CODE>null</CODE> argument to the
 * {@link #getJccPeer(String)} method, they will be returned
 * the default installed implementation on their system if it exists.
 * <p>
 * <STRONG>Note:</STRONG> It is the responsibility of implementation vendors
 * to supply a version of a <CODE>DefaultJccPeer</CODE> or some means to
 * alias their peer implementation along with a means to place that
 * <CODE>DefaultJccPeer</CODE> class in the application classpath.
 @since 1.0b
 */

public class JccPeerFactory {

  /**
   * Constructor for JccPeerFactory class. This is private because
   * applications are not permitted to create an instance of the
   * JccPeerFactory.
   */
  private JccPeerFactory() {
  }


  /**
   * Returns an instance of a JccPeer object given a fully qualified
   * classname of the class which implements the JccPeer object.
   * <p>
   * If no classname is provided (null), a default class named 
   * <CODE>DefaultJccPeer</CODE> is chosen as the classname to load.
   * If it does not exist or is not installed in the CLASSPATH as
   * the default, a ClassNotFoundException exception is thrown.
   * <p>
   
   * @param JccPeerName The classname of the JccPeer object class.
   * @return An instance of the JccPeer object.
   * @exception ClassNotFoundException Indicates that the JccPeer
   * specified by the classname is not available.
   */
  public synchronized static JccPeer getJccPeer(String JccPeerName)
    throws ClassNotFoundException {
      
      // If JccPeerName is null set it to a default JccPeer name
      if (JccPeerName == null || JccPeerName.length() == 0) {
	JccPeerName = getDefaultJccPeerName();
      }

      // If the JccPeerName is *still* null, then throw an exception
      if (JccPeerName == null) {
	throw new ClassNotFoundException();
      }

      try {
	  Class JccPeerClass = Class.forName(JccPeerName);
	  
	  // Instantiate an instance of this class and cast it to the JccPeer
	  // interface. If the class does not support the JccPeer interface,
	  // an exception will be thrown because of the case, in addition to
	  // the exceptions thrown from the newInstance() method.
	  return (JccPeer)JccPeerClass.newInstance();
      }
      catch (Exception e) {
	String errmsg = "JccPeer: " + JccPeerName +
	  " could not be instantiated.";
	throw new ClassNotFoundException(errmsg);
      }
  }


  /**
   * Returns a default JccPeer name. This method relies on the
   * existence of a default DefaultJccPeer.class in the application
   * classpath.  Note that the name DefaultJccPeer is a convention
   * enforced by this method.  Installations can alter this convention
   * by re-implementing this method.  This practice is in general
   * discouraged.
   * 
   * @return the default Jcc peer name
   */
  private static String getDefaultJccPeerName() {
    String JccPeerName = "DefaultJccPeer";
    return JccPeerName;
  }
}
