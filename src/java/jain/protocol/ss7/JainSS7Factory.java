/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
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
 *  AePONA Limited, Interpoint Building
 *  20-24 York Street, Belfast BT15 1AQ
 *  N. Ireland.
 *
 *
 *  Module Name   : JAIN TCAP API
 *  File Name     : JainSS7Factory.java
 *  Originator    : Colm Hayden & Phelim O'Doherty [AePONA]
 *  Approver      : Jain Tcap Edit Group
 *
 *  HISTORY
 *  Version   Date      Author              Comments
 *  1.1     16/10/2000  Phelim O'Doherty    Updated after public release and
 *                                          certification process comments.
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package jain.protocol.ss7;

import jain.InvalidPathNameException;
import java.util.Vector;

/**
 * The JAIN(tm) SS7 Factory is a singleton class by which JAIN SS7 applications
 * can obtain a proprietary (Peer) JAIN SS7 Object.
 * The term 'peer' is Java nomenclature for "a particular platform-specific
 * implementation of a Java interface or API".
 * This term has the same meaning for the JAIN SS7 API specifications.<BR>
 * A Peer JAIN SS7 Object can be obtained from the JAIN SS7 Factory
 * by ethier :
 * <UL>
 *    <LI>instantiating a new Peer JAIN SS7 Object using the
 *        <a href="#createSS7Object(java.lang.String)">createSS7Object()</a> method, <B>or</B>
 *    <LI>selecting from the list of JAIN SS7 Objects returned by the
 *        <a href="#getJainObjectList()">getJainObjectList()</a> method.
 * </UL>
 * <P>
 *
 * The JAIN SS7 Factory utilises a naming convention defined for each JAIN SS7 API
 * to identify the location of proprietary JAIN SS7 Objects.<P>
 * Under this convention the <B>lower-level package structure and classname</B> of a Peer JAIN SS7 Object is
 * mandated by a convention defined within the JAIN SS7 API from which the Object
 * originates. <P>
 * For example: within the JAIN TCAP API, the lower-level package structure and classname of
 * a proprietary implementation of the <I>jain.protocol.ss7.tcap.JainTcapStack</I>
 * interface <B>must</B> be <I>jain.protocol.ss7.tcap.JainTcapStackImpl</I>.<P>
 *
 * Under the JAIN naming convention, the <B>upper-level package structure</B> (pathname)
 * can be used to differentiate between proprietary implementations from different SS7 Vendors.
 * The pathname used by each SS7 Vendor <B>must be</B> the domain name assigned
 * to the Vendor in reverse order, e.g. Sun Microsystem's would be 'com.sun' <P>
 *
 * It follows that a proprietary implementation of a JAIN SS7 Object can be located
 * at: <BR>
 * <I>'pathname'.'lower-level package structure and classname'</I><P>
 * <B>Where:</B><P>
 * <I>pathname</I> = reverse domain name, e.g.  com.sun'<P>
 * <I>lower-level package structure and classname</I> = mandated naming convention for the JAIN SS7
 * Object in question<P>
 * e.g. <I>jain.protocol.ss7.tcap.JainTcapStackImpl</I> is the mandated naming convention
 * for <I>jain.protocol.ss7.tcap.JainTcapStack</I>.<P>
 * The resulting Peer JAIN SS7 Object would be located at: <I><B>com.sun</B>.jain.protocol.ss7.tcap.JainTcapStackImpl</I><BR>
 *
 * Because the space of domain names is managed, this scheme ensures that collisions
 * between two different vendor's implementations will not happen. For example: a
 * different Vendor with a domain name 'foo.com' would have their Peer JainTcapStack Object
 * located at <I><B>com.foo</B>.jain.protocol.ss7.tcap.JainTcapStackImpl</I>.<BR>
 * This is a similar concept to the JAVA conventions used for managing package names.<P>
 *
 * The pathname is defaulted to 'com.sun' but may be set using the
 * <a href="#setPathName()">setPathName()</a> method. This allows a JAIN application
 * to switch between different Vendor implementations, as well as providing the
 * capability to use the default or current pathname.<P>
 *
 * The JAIN SS7 Factory is a <B>Singleton</B> class. This means that there will
 * only be one instance of the class with a global point of access to it. The single
 * instance of the JAIN SS7 Factory can be obtained using the
 * <a href="#getInstance()">getInstance()</a> method.
 * In this way, the JAIN SS7 Factory can acts a single point obtaining JAIN SS7 Objects.
 *
 *
 *
 *
 * @version 1.1
 * @author Sun Microsystems Inc.
 *
 */
public class JainSS7Factory {

    /**
    * Constructor for JainSS7Factory class. This is private because
    * applications are not permitted to create an instance of the
    * JainSS7Factory.
    */
    private JainSS7Factory() {
        jainObjectList = new Vector();
    }

    /**
    * Returns an instance of a JainSS7Factory.
    * This is a singleton type class so this method is the
    * global access point for the JainSS7Factory.
    * @return the single instance of this singleton JainSS7Factory
    */
    public synchronized static  JainSS7Factory getInstance () {
        if(myFactory == null) {
           myFactory = new JainSS7Factory();
        }
        return myFactory;
    }

    /**
    * Sets the Pathname that identifies the location of a particular Vendor's
    * implementation of the JAIN SS7 Objects.
    * The pathname <B>must be</B> the domain name assigned
    * to the Vendor providing the implementation in reverse order.
    * @param <var>pathname</var> the reverse domain name of the Vendor. e.g. Sun Microsystem's would be 'com.sun'
    */
    public void setPathName (String pathname) throws InvalidPathNameException {
        pathName = pathname;
    }

    /**
    * Returns the current Pathname. The pathname identifies the location of a particular Vendor's
    * implementation of the JAIN SS7 Objects.
    * The pathname <B>will always be</B> the domain name assigned
    * to the Vendor providing the implementation in reverse order.
    * @return the pathname
    */
    public String getPathName () {
	    return pathName;
    }

    /**
    * Returns an instance of a Peer JAIN SS7 Object identified by the supplied classname.
    * This supplied classname is the <B>lower-level package structure and classname</B>
    * of the required Peer JAIN SS7 Object. This classname is mandated by a convention
    * defined within the JAIN SS7 API from which the Object originates. <P>
    * For example: To create an instance of the <I>jain.protocol.ss7.tcap.JainTcapStack</I>
    * from the JAIN TCAP API, the supplied classname <B>will</B> be
    * <I>jain.protocol.ss7.tcap.JainTcapStackImpl</I>.<P>
    * <P>
    *
    * Under the JAIN naming convention, the <B>upper-level package structure</B> (pathname)
    * can be used to differentiate between proprietary implementations from different SS7 Vendors.
    * <p>
    * The location of the returned JAIN SS7 Object is dependent on the current
    * pathname and the supplied classname.
    * <P>
    * If the specified class does not exist or is not installed in the CLASSPATH
    * a PeerUnavailableException exception is thrown.
    * <p>
    * Once a Peer SS7 Object is created an object reference to the
    * newly created object is stored along with object refernces to any
    * other Peer SS7 Objects that have previously been created by
    * this <CODE>JainSS7Factory</CODE>. This list of Peer SS7 Objects
    * may be retrieved at any time using the <a href="#getJainObjectList()">getJainObjectList()</a> method.
    * <p>
    * @param objectClassName lower-level package structure and classname of the Peer JAIN SS7 Object class
    * that is mandated by a convention defined within the JAIN SS7 API from which the Object originates.
    * @return An instance of a Peer JAIN SS7 Object.
    * @exception PeerUnavailableException Indicates that the Peer JAIN SS7 Object
    * specified by the classname and the current pathname cannot be located.
    */
    public synchronized Object createSS7Object(String objectClassName) throws jain.PeerUnavailableException {

        //  If the stackClassName is null, then throw an exception
        if (objectClassName == null) {
            throw new jain.PeerUnavailableException();
        }
        try {
            Class peerObjectClass = Class.forName(getPathName()+"."+objectClassName);

            // Create a new instance of the class represented by this Class object.
            Object newPeerObject = peerObjectClass.newInstance();

            // Add the Peer Object to the list of Jain SS7 Objects created by this Factory
            jainObjectList.addElement(newPeerObject);

            return(newPeerObject);

        } catch (Exception e) {
            String errmsg = "The Peer JAIN SS7 Object: " + objectClassName +
	                " could not be instantiated. Ensure the Path Name has been set.";
        throw new jain.PeerUnavailableException(errmsg);
        }
    }

    /**
    * Returns a Vector containing all of the Peer JAIN Objects that have previously been created using
    * this <CODE>JainSS7Factory</CODE>.
    * <P>
    * <B>NOTE:</B> The Vector returned may contain Objects of any type that may have been created
    * through this factory.
    * @return a vector containing all JAIN SS7 Objects created through this JAIN SS7 Factory.
    */
    public synchronized Vector getJainObjectList() {
        return jainObjectList;
    }

    private static JainSS7Factory myFactory;
    private String pathName= "com.sun";
    private Vector jainObjectList = null;
}
