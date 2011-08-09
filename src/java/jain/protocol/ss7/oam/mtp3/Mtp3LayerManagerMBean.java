/*
 @(#) $RCSfile: Mtp3LayerManagerMBean.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:26 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:26 $ by $Author: brian $
 */

package jain.protocol.ss7.oam.mtp3;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines the methods required for the creation and management of all MTP
  * Level 3 Managed MBeans. <p>
  *
  * The Mtp3LayerManagerMBean acts as a factory for the creation of
  * Mtp3ManagedObjectMBeans and allows a NotificationListener to register as a listener of
  * all Mtp3ManagedObjectMBeans. <p>
  *
  * When creating a MBean an entry must be added to the LookupTable of that MBean's
  * ObjectName and reference, and when a MBean is being deleted the corresponding entry in
  * the LookupTable must be removed.  This is to enable the application to operate
  * successfully without a JMX agent. <p>
  *
  * An instance of this Mtp3LayerManagerMBean should be created through the
  * JainSS7Factory. The JainSS7Factory defines a naming convention to locate a proprietary
  * implementation of this interface. <p>
  *
  * Under the JAIN naming convention, the upper-level package structure (pathname) can be
  * used to differentiate between proprietary implementations from different SS7 Vendors.
  * The pathname used by each SS7 Vendor must be the domain name assigned to the Vendor in
  * reverse order, e.g. Sun Microsystem's would be 'com.sun' <p>
  *
  * In order to be recognised as a JMX MBean a class implementing this interface must be
  * called Mtp3LayerManager. <p>
  *
  * It follows that a proprietary implementation of a Mtp3LayerManagerMBean will be
  * located at: <p>
  *
  * .jain.protocol.ss7.oam.mtp3.Mtp3LayerManager <p>
  *
  * Where: <p>
  *
  * pathname = reverse domain name, e.g. 'com.aepona' <p>
  *
  * The resulting Mtp3LayerManager from AePONA would be located at: <p>
  *
  * com.aepona.jain.protocol.ss7.oam.mtp3.Mtp3LayerManager <p>
  *
  * Consequently, an application may create a Mtp3LayerManagerMBean by invoking:
  * JainSS7Factory.createSS7Object(jain.protocol.ss7.oam.mtp3.Mtp3LayerMan ager). The
  * PathName of the vendor specific implementation of which you want to instantiate can be
  * set before calling JainSS7Factory.setPathName("vendorPathname"); or the default or
  * current pathname may be used. <p>
  *
  * <center> [Instantiation.jpg] </center><br>
  * <center> Illustrates how JAIN OAM MBeans are instantiated </center><br> <p>
  * <center> [Mtp3MBeans.jpg] </center><br>
  * <center> Relationship between the MTP3 JAIN OAM MBeans </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface Mtp3LayerManagerMBean extends OamLayerManagerMBean, Mtp3ManagedObjectMBean {
    /**
      * Creates a new Mtp3TimerProfileMBean that will use the correct Timer values for the
      * the protocol variant of this Mtp3LayerManager. When the Mtp3TimerProfileMBean is
      * created, the corresponding OamTimerVals should be created for each Timer with the
      * correct min, max and default values set.
      *
      * @return
      * The ObjectName of the newly created Mtp3TimerProfileMBean.
      *
      * @exception TooManyInstancesException
      * If no more timer profiles can be created by this implementation.
      */
    public ObjectName createMtp3TimerProfile()
        throws TooManyInstancesException;
    /**
      * Creates a new Link Managed Bean (LinkMBean).
      *
      * @param mtp2Sap
      * The ObjectName of the Mtp2SapMBean defining the hardware characteristics of this
      * LinkMBean.
      *
      * @return
      * The ObjectName of the newly created LinkMBean.
      *
      * @exception TooManyInstancesException
      * If no more MTP2 SAPs can be created.
      */
    public ObjectName createLink(ObjectName mtp2Sap)
        throws TooManyInstancesException;
    /**
      * Creates a new Linkset Managed Bean (LinkSetMBean).
      *
      * @param adjacentSignallingPoint
      * The ObjectName of the Adjacent SignallingPointMBean of this LinkSetMBean.
      *
      * @param links
      * An array of ObjectNames of the LinkMBeans of this LinkSetMBean.
      *
      * @return
      * The ObjectName of the newly created LinkSetMBean.
      *
      * @exception TooManyInstancesException
      * When: <ul>
      *
      * <li>no more LinkSetMBeans can be created, or
      * <li>too many LinkMBeans have been supplied for the LinksetMBean </ul.
      */
    public ObjectName createLinkSet(ObjectName adjacentSignallingPoint, ObjectName[] links)
        throws TooManyInstancesException;
    /**
      * Creates a new Route Managed Bean (RouteMBean).
      *
      * @param destinationSignallingPoint
      * The destination OwnSignallingPointMBean of this RouteMBean.
      *
      * @param linkSets
      * An Array of ObjectNames of the LinksetMBeans that make up this RouteMBean.
      *
      * @return
      * The ObjectName of the newly created RouteMBean.
      *
      * @exception TooManyInstancesException
      * When: <ul>
      * <li>no more RouteMBeans can be created by this implementation, or
      * <li>too many LinkSetMBeans were supplied for the RouteMBean </ul>
      */
    public ObjectName createRoute(ObjectName destinationSignallingPoint, ObjectName[] linkSets)
        throws TooManyInstancesException;
    /**
      * Creates a new Routeset Managed Bean (RouteSetMBean). [NOTE: The destination
      * OwnSignallingPointMBean of the created RoutesetMBean will be the the destination
      * OwnSignallingPointMBean of all of the RouteMBeans supplied in routes
      *
      * @param routes
      * The array of ObjectNames of the RouteMBeans that make up this RouteSetMBean.
      *
      * @return
      * The ObjectName of the newly created RouteSetMBean.
      *
      * @exception TooManyInstancesException
      * When: <ul>
      *
      * <li>no more RouteSetMBeans can be created by this implementation, or
      * <li>too many RouteMBeans were supplied for the RouteSetMBean </ul>
      */
    public ObjectName createRouteSet(ObjectName[] routes)
        throws TooManyInstancesException;
    /**
      * Creates a new Own Signalling Point Managed Bean (OwnSignallingPointMBean).
      *
      * @param pointCode
      * The signalling point code
      *
      * @param type
      * One of the following: <ul>
      * <li>SignallingPointMBean.TYPE_SEP - a Signalling End Point.
      * <li>SignallingPointMBean.TYPE_STP - a Signalling Transfer Point.
      * <li>SignallingPointMBean.TYPE_STEP - a Signalling Transfer End Point. </ul>
      *
      * @param networkInd
      * One of the following Network Indicator constants: <ul>
      * <li>SignallingPointMBean.NI_INTERNATIONAL_00
      * <li>SignallingPointMBean.NI_INTERNATIONAL_01
      * <li>SignallingPointMBean.NI_NATIONAL_10 
      * <li>SignallingPointMBean.NI_NATIONAL_11  </ul>
      *
      * @return
      * The ObjectName of the newly created OwnSignallingPointMBean.
      *
      * @exception TooManyInstancesException
      * If no more own signalling point MBeans can be created by this implementation.
      *
      * @exception IllegalArgumentException
      * If any of the arguments represent an illegal value.
      */
    public ObjectName createOwnSignallingPoint(Long pointCode, Integer type, Integer networkInd)
        throws TooManyInstancesException, IllegalArgumentException;
    /**
      * Creates a new Own Signalling Point Managed Bean (OwnSignallingPointMBean).
      *
      * @param pointCode
      * The signalling point code.
      *
      * @param type
      * One of the following: <ul>
      * <li>SignallingPointMBean.TYPE_SEP - a Signalling End Point.
      * <li>SignallingPointMBean.TYPE_STP - a Signalling Transfer Point.
      * <li>SignallingPointMBean.TYPE_STEP - a Signalling Transfer End Point. </ul>
      *
      * @param networkInd
      * One of the following Network Indicator constants: <ul>
      * <li>SignallingPointMBean.NI_INTERNATIONAL_00
      * <li>SignallingPointMBean.NI_INTERNATIONAL_01
      * <li>SignallingPointMBean.NI_NATIONAL_10 
      * <li>SignallingPointMBean.NI_NATIONAL_11  </ul>

      * @return
      * The ObjectName of the newly created SignallingPointMBean.
      *
      * @exception TooManyInstancesException
      * If no more own signalling point MBeans can be created by this implementation.
      *
      * @exception IllegalArgumentException
      * If any of the arguments represent an illegal value.
      */
    public ObjectName createSignallingPoint(Long pointCode, Integer type, Integer networkInd)
        throws TooManyInstancesException, IllegalArgumentException;
    /**
      * Creates a new Mtp3 Service Access Point MBean (representing a Remote Signalling
      * Point)
      *
      * @param signallingPoint
      * the signalling point
      *
      * @param serviceIndicator
      * one of the following Service Indicator constants: <ul>
      * <li>Mtp3SapMBean.SI_BROADBAND_ISUP
      * <li>Mtp3SapMBean.SI_DUP_CALL_CIRCUIT
      * <li>Mtp3SapMBean.SI_DUP_FACILITY_CANCELLATION
      * <li>Mtp3SapMBean.SI_ISUP
      * <li>Mtp3SapMBean.SI_MTP_TESTING
      * <li>Mtp3SapMBean.SI_SATELLITE_ISUP
      * <li>Mtp3SapMBean.SI_SCCP
      * <li>Mtp3SapMBean.SI_SIG_NET_MAN_MESSAGES
      * <li>Mtp3SapMBean.SI_SIG_NET_TEST_MAIN_MESSAGES
      * <li>Mtp3SapMBean.SI_TUP </ul>
      *
      * @return
      * The ObjectName of the newly created Mtp3SapMBean.
      *
      * @exception TooManyInstancesException
      * If no more Mtp3SapMBeans can be created by this implementation.
      *
      * @exception IllegalArgumentException
      * If any of the arguments represent an illegal value
      */
    public ObjectName createMtp3Sap(ObjectName signallingPoint, Integer serviceIndicator)
        throws TooManyInstancesException, IllegalArgumentException;
    /**
      * Creates a new Screening Table Managed Bean (ScreeningTableMBean).
      *
      * screeningType
      * either: <ul>
      * <li>ScreeningTableMBean.SCR_TYPE_INCOMING_LS_TO_DPC - To inhibit/allow STP access
      * by a combination of designated incoming link sets to designated DPCs.
      * <li>ScreeningTableMBean.SCR_TYPE_OUTGOING_LS_TO_DPC - To inhibit/allow STP access
      * by a combination of designated outgoing link sets to designated DPCs.
      * <li>ScreeningTableMBean.SCR_TYPE_OPC_TO_DPC - To inhibit/allow STP access by
      * examination of OPC and DPC combination in the incoming STP message. </ul>
      *
      * @return
      * The ObjectName of the newly created ScreeningTableMBean.
      *
      * @exception UnsupportedOperationException
      * If the optional SS7 functionality for an STP to identify and to handle
      * unauthorized SS7 messages is not supported by this implementation.
      *
      * @exception TooManyInstancesException
      * If no more screening tables can be created by this implementation.
      */
    public ObjectName createScreeningTable()
        throws UnsupportedOperationException, TooManyInstancesException;
    /**
      * Deletes the Mtp3TimerProfileMBean identified by the specified ObjectName. Note
      * that once a Mtp3TimerProfileMBean has been deleted, it should no longer be
      * returned by the getMtp3TimerProfiles() method.
      */
    public void deleteMtp3TimerProfile(ObjectName mtp3TimerProfile);
    /**
      * Deletes the LinkMBean identified by the specified ObjectName.  Note that once a
      * LinkMBean has been deleted, it should no longer be returned by the getLinks()
      * method.
      */
    public void deleteLink(ObjectName link);
    /**
      * Deletes the LinkSetMBean identified by the specified ObjectName. Note that once a
      * LinksetMBean has been deleted, it should no longer be returned by the
      * getLinksets() method.
      */
    public void deleteLinkSet(ObjectName linkSet);
    /**
      * Deletes the RouteMBean identified by the specified ObjectName.  Note that once a
      * RouteMBean has been deleted, it should no longer be returned by the getRoutes()
      * method.
      */
    public void deleteRoute(ObjectName route);
    /**
      * Deletes the RouteSetMBean identified by the specified ObjectName. Note that once a
      * RoutesetMBean has been deleted, it should no longer be returned by the
      * getRoutesets() method.
      */
    public void deleteRouteSet(ObjectName routeSet);
    /**
      * Deletes the OwnSignallingPointMBean identified by the specified ObjectName. Note
      * that once an OwnSignallingPointMBean has been deleted, it should no longer be
      * returned by the getOwnSignallingPoints() method.
      */
    public void deleteOwnSignallingPoint(ObjectName ownSignallingPoint);
    /**
      * Deletes the SignallingPointMBean identified by the specified ObjectName. Note that
      * once an SignallingPointMBean has been deleted, it should no longer be returned by
      * the getSignallingPoints() method.
      */
    public void deleteSignallingPoint(ObjectName signallingPoint);
    /**
      * Deletes the Mtp3SapMBean identified by the specified ObjectName. Note that once a
      * Mtp3SapMBean has been deleted, it should no longer be returned by the
      * getMtp3Saps() method.
      */
    public void deleteMtp3Sap(ObjectName sap);
    /**
      * Deletes the ScreeningTableMBean identified by the specified ObjectName. Note that
      * once a ScreeningTableMBean has been deleted, it should no longer be returned by
      * the getScreeningTables() method.
      *
      * @exception UnsupportedOperationException
      * If the optional SS7 functionality for an STP to identify and to handle
      * unauthorized SS7 messages is not supported by this implementation.
      */
    public void deleteScreeningTable(ObjectName screeningTable)
        throws UnsupportedOperationException;
    /**
      * Sets one of the active (local) OwnSignallingPointMBeans of this stack.  The
      * underlying stack management system may support multiple signalling points on the
      * the same system. The OwnSignallingPointMBean will be set at the specified position
      * in the set of OwnSignallingPointMBeans of this stack.
      *
      * @param index
      * The index position of the element in the array.
      *
      * @param OwnSignallingPoint
      * The ObjectName of the local OwnSignallingPointMBean.
      *
      * @exception TooManyInstancesException
      * If the supplied index is greater than the number of OwnSignallingPointMBeans
      * permitted by this implementation.
      */
    public void setActiveOwnSignallingPoint(Integer index, ObjectName OwnSignallingPoint)
        throws TooManyInstancesException;
    /**
      * Returns the ObjectName of one of the active (local) OwnSignallingPointMBeans of
      * this stack.  The underlying stack management system may support multiple
      * signalling points on the the same system. The ObjectName of the
      * OwnSignallingPointMBean will be returned from the specified position in the set of
      * OwnSignallingPointMBeans of this stack. <p>
      *
      * There is a subtle difference between this method and the getOwnSignallingPoints()
      * method in that this method returns the ObjectName of one of the
      * OwnSignallingPointMBeans that are currently in use, whereas the
      * getOwnSignallingPoints() method returns the ObjectName of one of the
      * OwnSignallingPointMBeans (both those OwnSignallingPointMBeans currently in use and
      * the OwnSignallingPointMBeans that have been configured are not available for use
      * for any reason including for backup). The ObjectName of every
      * OwnSignallingPointMBean that was created through the createOwnSignallingPoint()
      * method shall be returned by the getOwnSignallingPoints() method, but only the
      * ObjectName of those OwnSignallingPointMBeans that have been activated using the
      * setActiveOwnSignallingPoint() method shall be returned by this method.
      *
      * @param index
      * The index position of the element in the array.
      *
      * @return
      * The ObjectName of the local OwnSignallingPointMBean at the specified postion.
      */
    public ObjectName getActiveOwnSignallingPoint(Integer index);
    /**
      * Sets all of the active (local) OwnSignallingPointMBeans of this stack.  The
      * underlying stack management system may support multiple Signalling points on the
      * the same system.
      *
      * @param OwnSignallingPoints
      * All of the (local) Signalling Points of this stack.
      *
      * @exception TooManyInstancesException
      * If the size of the array is greater than the number of OwnSignallingPointMBeans
      * permitted by this implementation.
      */
    public void setActiveOwnSignallingPoints(ObjectName[] OwnSignallingPoints)
        throws TooManyInstancesException;
    /**
      * Returns the ObjectNames of all of the active (local) OwnSignallingPointMBeans of
      * this stack.  The underlying stack management system may support multiple
      * signalling points on the the same system. <p>
      *
      * There is a subtle difference between this method and the getOwnSignallingPoints()
      * method in that this method returns the ObjectNames of only the
      * OwnSignallingPointMBeans that are currently in use, whereas the
      * getOwnSignallingPoints() method returns the ObjectNames of all
      * OwnSignallingPointMBeans (both those OwnSignallingPointMBeans currently in use and
      * the OwnSignallingPointMBeans that have been configured are not available for use
      * for any reason including for backup). The ObjectNames of every
      * OwnSignallingPointMBean that was created through the createOwnSignallingPoint()
      * method shall be returned by the getOwnSignallingPoints() method, but only the
      * ObjectNames of those OwnSignallingPointMBeans that have been activated using the
      * setActiveOwnSignallingPoint() method shall be returned by this method.
      *
      * @return
      * The ObjectNames of all the (local) OwnSignallingPointMBeans of this stack.
      */
    public ObjectName[] getActiveOwnSignallingPoints();
    /**
      * Returns the ObjectNames of all the Mtp3SapMBeans (Remote Signalling Points)
      * created by this layer manager
      */
    public ObjectName[] getMtp3Saps();
    /**
      * Returns the ObjectName of one of the Mtp3SapMBeans (Remote Signalling Points)
      * created by this layer manager
      *
      * @param index
      * The index of the Remote OwnSignallingPointMBean in the list.
      */
    public ObjectName getMtp3Sap(Integer index);
    /**
      * Returns the ObjectNames of all the Mtp3TimerProfilesMBeans created by this layer
      * manager
      */
    public ObjectName[] getMtp3TimerProfiles();
    /**
      * Returns the ObjectName of one of the Mtp3TimerProfileMBeans created by this layer
      * manager
      *
      * @param index
      * The index of the Mtp3TimerProfileMBean in the list.
      */
    public ObjectName getMtp3TimerProfile(Integer index);
    /**
      * Returns the ObjectNames of all the LinkMBeans created by this layer manager
      */
    public ObjectName[] getLinks();
    /**
      * Returns the ObjectName of one of the LinkMBeans created by this layer manager
      *
      * @param index
      * The index of the LinkMBean in the list.
      */
    public ObjectName getLink(Integer index);
    /**
      * Returns the ObjectNames of all the LinkSetMBeans created by this layer manager
      */
    public ObjectName[] getLinkSets();
    /**
      * Returns the ObjectName of one of the LinkSetMBeans created by this layer manager
      *
      * @param index
      * The index of the LinkSetMBean in the list.
      */
    public ObjectName getLinkSet(Integer index);
    /**
      * Returns the ObjectNames of all of the RouteMBeans created by this layer manager
      */
    public ObjectName[] getRoutes();
    /**
      * Returns the ObjectName of one of the RouteMBeans created by this layer manager
      *
      * @param index
      * The index of the RouteMBean in the list.
      */
    public ObjectName getRoute(Integer index);
    /**
      * Returns the ObjectNames of all the RouteSetMBeans created by this layer manager
      */
    public ObjectName[] getRouteSets();
    /**
      * Returns the ObjectName of one of the RouteSetMBeans created by this layer manager
      *
      * @param index
      * The index of the RouteSetMBean in the list.
      */
    public ObjectName getRouteSet(Integer index);
    /**
      * Returns the ObjectNames of all of the ScreeningTableMBeans created by this layer
      * manager.
      *
      * @exception UnsupportedOperationException
      * If the optional SS7 functionality for an STP to identify and to handle
      * unauthorized SS7 messages is not supported by this implementation.
      */
    public ObjectName[] getScreeningTables()
        throws UnsupportedOperationException;
    /**
      * Returns the ObjectName of one of the ScreeningTableMBeans created by this layer
      * manager
      *
      * @param index
      * The index of the ScreeningTableMBean in the list.
      *
      * @exception UnsupportedOperationException
      * If the optional SS7 functionality for an STP to identify and to handle
      * unauthorized SS7 messages is not supported by this implementation
      */
    public ObjectName getScreeningTable(Integer index)
        throws UnsupportedOperationException;
    /**
      * Returns the ObjectNames of all the OwnSignallingPointMBeans created by this layer
      * manager
      */
    public ObjectName[] getOwnSignallingPoints();
    /**
      * Returns the ObjectNames of one of the OwnSignallingPointMBeans created by this
      * layer manager
      *
      * @param index
      * The index of the OwnSignallingPointMBean in the list.
      */
    public ObjectName getOwnSignallingPoint(Integer index);
    /**
      * Returns the ObjectNames of all the SignallingPointMBeans created by this layer
      * manager
      */
    public ObjectName[] getSignallingPoints();
    /**
      * Returns the ObjectNames of one of the SignallingPointMBeans created by this layer
      * manager
      *
      * @param index
      * The index of the SignallingPointMBean in the list.
      */
    public ObjectName getSignallingPoint(Integer index);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
