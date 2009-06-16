/*
 * Copyright (c) 1997 Sun Microsystems, Inc. All Rights Reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for NON-COMMERCIAL purposes and without
 * fee is hereby granted provided that this copyright notice
 * appears in all copies. Please refer to the file "copyright.html"
 * for further important copyright and licensing information.
 *
 * SUN MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF
 * THE SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
 * TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
 * PARTICULAR PURPOSE, OR NON-INFRINGEMENT. SUN SHALL NOT BE LIABLE FOR
 * ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR
 * DISTRIBUTING THIS SOFTWARE OR ITS DERIVATIVES.
 */

package javax.telephony;

/**
 * The <CODE>JtapiPeerFactory</CODE> class is a class by which applications
 * obtain a Provider object.
 * 
 * <H4> Introduction </H4>
 *
 * Applications use this class to first obtain a 
 * class which implements the <CODE>JtapiPeer</CODE> interface. The
 * <CODE>JtapiPeer</CODE> interface represents a particular vendor's
 * implementation of JTAPI.  The term 'peer' is Java nomenclature for
 * "a particular platform-specific implementation of a Java interface or API".
 * This term has the same meaning for the Java Telephony API. Applications are
 * not permitted to create an instance of the <CODE>JtapiPeerFactory</CODE>
 * class. Through an installation procedure provided by each implementator,
 * a <CODE>JtapiPeer</CODE> class is made available to an application
 * environment.  When applications have a JtapiPeer object for a particular 
 * platform-dependent implementation, they may obtain a Provider object via 
 * that interface. The details of that interface are discussed in the 
 * specification for the <CODE>JtapiPeer</CODE> interface.
 *
 * <H4>Obtaining a JtapiPeer Object</H4>
 *
 * Applications use the <CODE>JtapiPeerFactory.getJtapiPeer()</CODE> method to
 * obtain a JtapiPeer object. The argument to this method is a classname
 * which represents an object which implements the <CODE>JtapiPeer</CODE>
 * interface. This object and the classname under which it can be found must
 * be supplied by the vendor of the implementation. Note that this object is
 * not a Provider, however, this interface is used to obtain Provider objects
 * from that particular implementation.
 * <p>
 * The Java Telephony API places conventions on vendors on the classname
 * they use for their JtapiPeer object. This class name <EM>must</EM> begin
 * with the domain name assigned to the vendor in reverse order. Because the
 * space of domain names is managed, this scheme ensures that collisions
 * between two different vendor's implementations will not happen. For example,
 * an implementation from Sun Microsystem's will have "com.sun" as the prefix
 * to its JtapiPeer class. After the reversed domain name, vendors are free to
 * choose any class hierarchy they desire.
 *
 * <H4>Default JtapiPeer</H4>
 *
 * Additionally, the vendor providing the JtapiPeer class may supply a
 * a <CODE>DefaultJtapiPeer.class</CODE> class file. When placed in the 
 * classpath of applications, this class (which must implement the
 * <CODE>JtapiPeer</CODE> interface) becomes the default JtapiPeer object
 * returned by the <CODE>JtapiPeerFactory.getJtapiPeer()</CODE> method. By 
 * convention the default class name must be <CODE>DefaultJtapiPeer</CODE>.
 * <p>
 * In basic environments, applications and users do not want the burden of
 * finding out the class name in order to use a particular implementation.
 * Therefore, the <CODE>JtapiPeerFactory</CODE> class supports a mechanism for
 * applications to obtain the default implementation for their system. If
 * applications use a <CODE>null</CODE> argument to the
 * <CODE>JtapiPeerFactory.getJtapiPeer()</CODE> method, they will be returned
 * the default installed implementation on their system if it exists.
 * <p>
 * <STRONG>Note:</STRONG> It is the responsibility of implementation vendors
 * to supply a version of a <CODE>DefaultJtapiPeer</CODE> or some means to
 * alias their peer implementation along with a means to place that
 * <CODE>DefaultJtapiPeer</CODE> class in the application classpath.
 * <p>
 * @see javax.telephony.JtapiPeer
 * @version %G% %I%
 */

public class JtapiPeerFactory {

  /**
   * Constructor for JtapiPeerFactory class. This is private because
   * applications are not permitted to create an instance of the
   * JtapiPeerFactory.
   */
  private JtapiPeerFactory() {
  }


  /**
   * Returns an instance of a JtapiPeer object given a fully qualified
   * classname of the class which implements the JtapiPeer object.
   * <p>
   * If no classname is provided (null), a default class named 
   * <CODE>DefaultJtapiPeer</CODE> is chosen as the classname to load.
   * If it does not exist or is not installed in the CLASSPATH as
   * the default, a JtapiPeerUnavailableException exception is thrown.
   * <p>
   * @param jtapiPeerName The classname of the JtapiPeer object class.
   * @return An instance of the JtapiPeer object.
   * @exception JtapiPeerUnavailableException Indicates that the JtapiPeer
   * specified by the classname is not available.
   */
  public synchronized static JtapiPeer getJtapiPeer(String jtapiPeerName)
    throws JtapiPeerUnavailableException {
      
      // If jtapiPeerName is null set it to a default jtapiPeer name
      if (jtapiPeerName == null || jtapiPeerName.length() == 0) {
	jtapiPeerName = getDefaultJtapiPeerName();
      }

      // If the jtapiPeerName is *still* null, then throw an exception
      if (jtapiPeerName == null) {
	throw new JtapiPeerUnavailableException();
      }

      try {
	  Class jtapiPeerClass = Class.forName(jtapiPeerName);
	  
	  // Instantiate an instance of this class and cast it to the JtapiPeer
	  // interface. If the class does not support the JtapiPeer interface,
	  // an exception will be thrown because of the case, in addition to
	  // the exceptions thrown from the newInstance() method.
	  return (JtapiPeer)jtapiPeerClass.newInstance();
      }
      catch (Exception e) {
	String errmsg = "JtapiPeer: " + jtapiPeerName +
	  " could not be instantiated.";
	throw new JtapiPeerUnavailableException(errmsg);
      }
  }


  /**
   * Returns a default JtapiPeer name. This method relies on the
   * existence of a default DefaultJtapiPeer.class in the application
   * classpath.  Note that the name DefaultJtapiPeer is a convention
   * enforced by this method.  Installations can alter this convention
   * by re-implementing this method.  This practice is in general
   * discouraged.
   * 
   * @return the default JTAPI peer name
   */
  private static String getDefaultJtapiPeerName() {
    String JtapiPeerName = "DefaultJtapiPeer";
    return JtapiPeerName;
  }
}
