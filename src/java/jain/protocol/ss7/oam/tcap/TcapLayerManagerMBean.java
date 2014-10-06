/*
 @(#) $RCSfile: TcapLayerManagerMBean.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:28 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:28 $ by $Author: brian $
 */

package jain.protocol.ss7.oam.tcap;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines the methods required for the creation and management of all MTP
  * Level 2 Managed MBeans. <p>
  *
  * The Mtp2LayerManagerMBean acts as a factory for the creation of
  * Mtp2ManagedObjectMBeans and allows a NotificationListener to register as a listener of
  * all Mtp2ManagedObjectMBeans. <p>
  *
  * When creating a MBean an entry must be added to the LookupTable of that MBean's
  * ObjectName and reference, and when a MBean is being deleted the corresponding entry in
  * the LookupTable must be removed.  This is to enable the application to operate
  * successfully without a JMX agent. <p>
  *
  * An instance of this Mtp2LayerManagerMBean should be created through the
  * JainSS7Factory. The JainSS7Factory defines a naming convention to locate a proprietary
  * implementation of this interface. <p>
  *
  * Under the JAIN naming convention, the upper-level package structure (pathname) can be
  * used to differentiate between proprietary implementations from different SS7 Vendors.
  * The pathname used by each SS7 Vendor must be the domain name assigned to the Vendor in
  * reverse order, e.g. Sun Microsystem's would be 'com.sun' <p>
  *
  * In order to be recognised as a JMX MBean a class implementing this interface must be
  * called Mtp2LayerManager. <p>
  *
  * It follows that a proprietary implementation of a Mtp2LayerManagerMBean will be
  * located at: <p>
  *
  * .jain.protocol.ss7.oam.mtp2.Mtp2LayerManager <p>
  *
  * Where: <p>
  *
  * pathname = reverse domain name, e.g. 'com.aepona' <p>
  *
  * The resulting Mtp2LayerManager from AePONA would be located at: <p>
  *
  * com.aepona.jain.protocol.ss7.oam.mtp2.Mtp2LayerManager <p>
  *
  * Consequently, an application may create a Mtp2LayerManagerMBean by invoking:
  * JainSS7Factory.createSS7Object(jain.protocol.ss7.oam.mtp2.Mtp2LayerMan ager). The
  * PathName of the vendor specific implementation of which you want to instantiate can be
  * set before calling JainSS7Factory.setPathName("vendorPathname"); or the default or
  * current pathname may be used. <p>
  *
  * <center> [Instantiation.jpg] </center><br>
  * <center> This illustrates how JAIN OAM MBeans are instantiated </center><br> <p>
  * <center> [TcapMBeans.jpg] </center><br>
  * <center> Relationship between the TCAP JAIN OAM MBean </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface TcapLayerManagerMBean extends OamLayerManagerMBean, TcapManagedObjectMBean {
    /**
      * Sets one of the active TCAP layers (identified by the specified ObjectName)
      * managed by this TCAP Layer Manager. There will be one instance of a local TCAP per
      * local signalling point. Where multiple logical signalling points exist within the
      * one physical stack, there will be several local TCAP instances.
      *
      * @param index
      * the position of the TCAP layer in the array of TCAP layers managed by this TCAP
      * Layer Manager.
      *
      * @param tcapLayer
      * the ObjectName of the TCAP Layer
      *
      * @exception TooManyInstancesException
      * if no more TCAP Nodes can be activated by this implementation
      */
    public void setActiveTcapNode(Integer index, ObjectName tcapLayer)
        throws TooManyInstancesException;
    /**
      * Returns the ObjectName of one of the active TCAP layers managed by this TCAP Layer
      * Manager. There will be one instance of a local TCAP per local signalling point.
      * Where multiple logical signalling points exist within the one physical stack,
      * there will be several local TCAP instances. <p>
      *
      * There is a subtle difference between this method and the getTcapNodes() method in
      * that this method returns only the TCAPs that are currently in use, whereas the
      * getTcapNodes() method contains all TCAPs (both those TcapNodeMBeans currently in
      * use and the TcapNodeMBeans that have been configured, but not activated for any
      * reason including for backup). Every TcapNodeMBean that was created through the
      * createTcapNode() method shall be returned by the getTcapNodes() method, but only
      * those TCAPs that have been activated using the setActiveTcapNodes() method shall
      * be returned by this method.
      *
      * @param index
      * the position of the TCAP layer in the array of TCAP layers managed by this TCAP
      * Layer Manager.
      *
      * @return
      * the ObjectName of one of the active TCAPs
      */
    public ObjectName getActiveTcapNode(Integer index);
    /**
      * Sets all of the active TCAP layers (identified by the specified array of
      * ObjectNames) managed by this TCAP Layer Manager. There will be one instance of a
      * local TCAP per local signalling point. Where multiple logical signalling points
      * exist within the one physical stack, there will be several local TCAP instances.
      *
      * @param tcapLayers
      * an array of the ObjectNames of all of the TCAP Layer
      *
      * @exception TooManyInstancesException
      * if no more tcap nodes can be actvated by this implementation
      */
    public void setActiveTcapNodes(ObjectName[] tcapLayers)
        throws TooManyInstancesException;
    /**
      * Returns an array of ObjectNames of all the active TCAP layers managed by this TCAP
      * Layer Manager. There will be one instance of a local TCAP per local signalling
      * point. Where multiple logical signalling points exist within the one physical
      * stack, there will be several local TCAP instances. <p>
      *
      * There is a subtle difference between this method and the getTcapNodes() method in
      * that this method returns only the TCAPs that are currently in use, whereas the
      * getTcapNodes() method contains all TCAPs (both those TcapNodeMBeans currently in
      * use and the TcapNodeMBeans that have been configured, but not activated for any
      * reason including for backup). Every TcapNodeMBean that was created through the
      * createTcapNode() method shall be returned by the getTcapNodes() method, but only
      * those TCAPs that have been activated using the setActiveTcapNodes() method shall
      * be returned by this method.
      *
      * @return
      * an array of ObjectNames of all the TCAP Layers
      */
    public ObjectName[] getActiveTcapNodes();
    /**
      * Creates a new TcapNodeMBean for the specified local signalling point and
      * subsystem.
      *
      * @param sp
      * the ObjectName of the signalling point of the TCAP layer
      *
      * @param subsystems
      * an array of ObjectNames of the susbsystems of the TCAP layer.
      *
      * @return
      * the ObjectName of the newly created TcapNodeMBean.
      *
      * @exception TooManyInstancesException
      * if <ul>
      * <li>too many subsystems were supplied for the signalling point, or
      * <li>no more Tcap Nodes can be created by this implementation </ul>
      */
    public ObjectName createTcapNode(ObjectName sp, ObjectName[] subsystems)
        throws TooManyInstancesException;
    /** Deletes the specified TcapNodeMBean identified by the specified ObjectName Note
      * that once a TcapNodeMBean has been deleted, it should no longer be returned by the
      * getTcapNodes() method.  */
    public void deleteTcapNode(ObjectName tcap);
    /**
      * Returns the ObjectName of one of the TCAP layers managed by this TCAP Layer
      * Manager. Every TcapNodeMBean that was created through the createTcapNode() method
      * shall be returned by the getTcapNodes() method, but only those TCAPs that have
      * been activated using the setActiveTcapNodes() method shall by the
      * getActiveTcapNode() method.
      *
      * @param index
      * the position of the TCAP layer in the array of TCAP layers managed by this TCAP
      * Layer Manager.
      *
      * @return
      * the ObjectName of one of the created TCAPs
      */
    public ObjectName getTcapNode(Integer index);
    /**
      * Returns an array of ObjectNames of all the TCAP layers managed by this TCAP Layer
      * Manager. Every TcapNodeMBean that was created through the createTcapNode() method
      * shall be returned by the getTcapNodes() method, but only those TCAPs that have
      * been activated using the setActiveTcapNodes() method shall by the
      * getActiveTcapNode() method.
      *
      * @return
      * an array of ObjectNames of all created TcapNodeMBeans.
      */
    public ObjectName[] getTcapNodes();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
