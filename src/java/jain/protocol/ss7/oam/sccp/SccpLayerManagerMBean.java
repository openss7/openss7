/*
 @(#) $RCSfile: SccpLayerManagerMBean.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:27 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:27 $ by $Author: brian $
 */

package jain.protocol.ss7.oam.sccp;

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
  * To be recognised as a JMX MBean a class implementing this interface must be called
  * Mtp2LayerManager. <p>
  *
  * Therefore a proprietary implementation of a Mtp2LayerManagerMBean will be located at:
  * <p>
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
  * Consequently, an application may create a Mtp2LayerManagerMBean by invoking: <dl>
  *
  * <dt>JainSS7Factory.createSS7Object(jain.protocol.ss7.oam.mtp2.Mtp2LayerManager).
  * <dd>The PathName of the vendor specific implementation of which you want to
  * instantiate can be set before calling
  *
  * <dt>JainSS7Factory.setPathName("vendorPathname");
  * <dd>or the default or current pathname may be used.</dl>
  *
  * <center> [Instantiation.jpg] </center><br>
  * <center> This illustrates how JAIN OAM MBeans are instantiated </center><br> <p>
  * <center> [SccpMBeans.jpg] </center><br>
  * <center> Relationship between the SCCP JAIN OAM MBean </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface SccpLayerManagerMBean extends OamLayerManagerMBean, SccpManagedObjectMBean {
    /**
      * Sets one of the Active SCCP Routing controls (identified by the specified
      * ObjectName) of this SCCP Layer Manager. If the underlying system supports multiple
      * local signalling points then an instance of the SCCP Routing Control MBean should
      * be created for each local Signalling Point.
      *
      * @param index
      * the index of the SCCP Routing Control to be set.
      *
      * @param routingControl
      * the ObjectName of the routing control
      *
      * @exception TooManyInstancesException
      * if index is greater than the number of active routing controls permitted for an
      * SCCP Layer by this implementation.
      */
    public void setActiveSccpRoutingControl(Integer index, ObjectName routingControl)
        throws TooManyInstancesException;
    /**
      * Returns the ObjectName of the Active SCCP Routing controls of this SCCP Layer
      * Manager. If the underlying system supports multiple local signalling points then
      * an instance of the SCCP Routing Control MBean should have been created for each
      * local Signalling Point. <p>
      *
      * There is a subtle difference between this method and the getSccpRoutingControls()
      * method in that this method returns one the SCCP Routing Controls that are
      * currently in use, whereas the getSccpRoutingControls() method contains all SCCP
      * Routing Controls (both those Routing Controls currently in use and the Routing
      * Controls that have been configured, but not activated for any reason including for
      * backup). Every SCCP Routing Control that was created through the
      * createSccpRoutingControl() method shall be returned by the
      * getSccpRoutingControls() method, but only those Routing Controls that have been
      * activated using the setActiveRoutingControl() method shall be returned by this
      * method.
      *
      * @param index
      * the index of the SCCP Routing Control to be returned.
      *
      * @return
      * the ObjectName of one of the active routing control
      */
    public ObjectName getActiveSccpRoutingControl(Integer index);
    /**
      * Sets the Active SCCP Routing controls (identified by the specified ObjectName) of
      * this SCCP Layer Manager. If the underlying system supports multiple local
      * signalling points then an instance of the SCCP Routing Control MBean should be
      * created for each local Signalling Point.
      *
      * @param routingControls
      * the ObjectNames of all of the routing controls
      *
      * @exception TooManyInstancesException
      * if the size of routingControls is greater than the number of active Routing
      * controls permitted for an SCCP Layer by this implementation.
      */
    public void setActiveSccpRoutingControls(ObjectName[] routingControls)
        throws TooManyInstancesException;
    /**
      * Returns the ObjectNames of all the Active SCCP Routing controls of this SCCP Layer
      * Manager. If the underlying system supports multiple local signalling points then
      * an instance of the SCCP Routing Control MBean should have been created for each
      * local Signalling Point. <p>
      *
      * There is a subtle difference between this method and the getSccpRoutingControls()
      * method in that this method returns only the SCCP Routing Controls that are
      * currently in use, whereas the getSccpRoutingControls() method contains all SCCP
      * Routing Controls (both those Routing Controls currently in use and the Routing
      * Controls that have been configured, but not activated for any reason including for
      * backup). Every SCCP Routing Control that was created through the createTcap()
      * method shall be returned by the getSccpRoutingControls() method, but only those
      * Routing Controls that have been activated using the setActiveRoutingControl()
      * method shall be returned by this method.
      *
      * @return
      * the ObjectNames of all of the active SccpRoutingControlMBeans
      */
    public ObjectName[] getActiveSccpRoutingControls();
    /**
      * Creates a new SccpTimerProfileMBean that will use the correct Timer values for the
      * the protocol variant of this SccpLayerManager. When the Timer Profile is created,
      * the corresponding OamTimerVals should be created for each Timer with the correct
      * min, max and default values set.
      *
      * @return
      * the ObjectName of the newly created SccpTimerProfileMBean
      *
      * @exception TooManyInstancesException
      * if no more SCCP timer Profiles can be created by this implementation
      */
    public ObjectName createSccpTimerProfile()
        throws TooManyInstancesException;
    /**
      * Creates a new Global Title Entry Managed MBean.
      *
      * @param entitySet
      * the ObjectName of the entity set to which this entry translates to
      *
      * @param globalTitleIndicator
      * one of the Global Title Indicator constants: <ul>
      * <li>GlobalTitleEntryMBean.GTINDICATOR_0000
      * <li>GlobalTitleEntryMBean.GTINDICATOR_0001
      * <li>GlobalTitleEntryMBean.GTINDICATOR_0010
      * <li>GlobalTitleEntryMBean.GTINDICATOR_0011
      * <li>GlobalTitleEntryMBean.GTINDICATOR_0100 </ul>
      *
      * @param addressInformation
      * the address information composed of digits in the form of Binary Coded
      * Decimal(BCD).
      *
      * @return
      * the ObjectName of the newly created GlobalTitleEntryMBean
      *
      * @exception IllegalArgumentException
      * if <ul>
      * <li>the supplied global tilte indicator is not one of the defined constants, or
      * <li>the address information is in an unacceptable format </ul>
      *
      * @exception TooManyInstancesException
      * if no more Global Title Entries can be created by this implementation
      */
    public ObjectName createGlobalTitleEntry(ObjectName entitySet, Integer globalTitleIndicator, Byte[] addressInformation)
        throws IllegalArgumentException, TooManyInstancesException;
    /**
      * Creates a new Concerned Area Managed MBean containing the specified remote
      * Subsystems (SccpSaps).
      *
      * @param remoteSubsystems
      * an array of ObjectNames of the remote subsystems to be informed of local (primary
      * broadcast) or remote (secondary broadcast) SCCP Subsystem status changes, or to be
      * informed of the SCCP status after completion of SCCP Restart.
      *
      * @return
      * the ObjectName of the newly created ConcernedAreaMBean
      *
      * @exception TooManyInstancesException
      * if <ul>
      * <li>too many remote subsystems are supplied for the concerned area, or
      * <li>no more Concerned Areas can be created by this implementation </ul>
      */
    public ObjectName createConcernedArea(ObjectName[] remoteSubsystems)
        throws TooManyInstancesException;
    /**
      * Creates a new SCCP Sevice Access Point (SAP) Managed MBean (a local or remote
      * Subsystem)
      *
      * @param ssn
      * the subsystem number
      *
      * @param mtp3Sap
      * the ObjectName of the Mtp3Sap of this subsystem
      *
      * @return
      * the ObjectName of the newly created SccpSapMBean
      *
      * @exception IllegalArgumentException
      * if the supplied subsystem number represents an unnaceptable value
      *
      * @exception TooManyInstancesException
      * if no more SCCP SAPs can be created by this implemntation
      */
    public ObjectName createSccpSap(Integer ssn, ObjectName mtp3Sap)
        throws IllegalArgumentException, TooManyInstancesException;
    /**
      * Creates a new SCCP Entity Set Managed MBean
      *
      * @param destinationSubsystems
      * an array of ObjectNames of the destination Subsystems (SCCP SAPs) of the Entity
      * Set.
      *
      * @param sharingMode
      * the sharing mode determining the distribution of SCCP traffic over the entities in
      * this set. This may be one of: <ul>
      *
      * <li>SccpEntitySetMBean.SM_SOLITARY - there can only be one access point in the
      * set.
      *
      * <li>SccpEntitySetMBean.SM_DUPLI_DOMINANT - the second entity is a backup for the
      * first entity.
      *
      * <li>SccpEntitySetMBean.SM_DUPLI_REPLACEMENT - the second entity is standby for
      * backup for the first entity, but after changeover, the primary and backup roles
      * are swapped.
      *
      * <li>SccpEntitySetMBean.SM_DUPLI_SHARED - the load is shared over both the entities
      * in the set. </ul>
      *
      * @return
      * the ObjectName of the newly created SccpEntitySetMBean
      *
      * @exception IllegalArgumentException
      * if the supplied saring mode is ot one of the defined constants
      *
      * @exception TooManyInstancesException
      * if <ul>
      * <li>too many destination subsystems are supplied for the Entity Set, or
      * <li>no more SCCP Entity Sets can be created by this implementation </ul>
      */
    public ObjectName createSccpEntitySet(ObjectName[] destinationSubsystems, Integer sharingMode)
        throws TooManyInstancesException;
    /**
      * Creates a new Routing Control Managed MBean for the specified Own Signalling Point
      * and (local) Subsystems
      *
      * @param sp
      * the ObjectName of the Own signalling point whose routing is to be controlled by
      * this Routing Control function.
      *
      * @param subsystems
      * an array of ObjectNames of the local controlled by this Routing Control.
      *
      * @return
      * the ObjectName of the newly created SccpRoutingControlMBean
      *
      * @exception TooManyInstancesException
      * if <ul>
      * <li>too many subsystems are supplied for the created SCCP Routing Control, or
      * <li>no more SCCP Routing Control MBeans can be created by this implementation
      * </ul>
      */
    public ObjectName createSccpRoutingControl(ObjectName sp, ObjectName[] subsystems)
        throws TooManyInstancesException;
    /** Deletes the SccpTimerProfileMBean identified by the specified ObjectName. Note
      * that once a SccpTimerProfileMBean has been deleted, it should no longer be
      * returned by the getSccpTimerProfiles() method.  */
    public void deleteSccpTimerProfile(ObjectName timerProfile);
    /** Deletes the Global Title Entry Managed MBean identified by the specified
      * ObjectName. Note that once a GlobalTitleEntryMBean has been deleted, it should no
      * longer be returned by the getGlobalTitleEntries() method.  */
    public void deleteGlobalTitleEntry(ObjectName globalTitleEntry);
    /** Deletes the Concerned Area Managed MBean identified by the specified ObjectName.
      * Note that once a ConcernedAreaMBean has been deleted, it should no longer be
      * returned by the getConcernedAreas() method.  */
    public void deleteConcernedArea(ObjectName concernedArea);
    /** Deletes the SCCP Sevice Access Point (SAP) Managed MBean identified by the
      * specified ObjectName (a local or remote Subsystem). Note that once a SccpSapMBean
      * has been deleted, it should no longer be returned by the getSccpSaps() method.  */
    public void deleteSccpSap(ObjectName subSystem);
    /** Deletes the SCCP Entity Set Managed MBean identified by the specified ObjectName.
      * Note that once a SccpEntitySetMBean has been deleted, it should no longer be
      * returned by the getSccpEntitySet() method.  */
    public void deleteSccpEntitySet(ObjectName entitySet);
    /** Deletes the Routing Control Managed MBean identified by the specified ObjectName.
      * Note that once a SccpRoutingControlMBean has been deleted, it should no longer be
      * returned by the getSccpRoutingControls() method.  */
    public void deleteSccpRoutingControl(ObjectName routingControl);
    /** Returns an array of the ObjectNames of all SCCP SAPs (Local or Remote Subsystems)
      * created by this layer manager */
    public ObjectName[] getSccpSaps();
    /**
      * Returns the ObjectName of one of the SCCP SAPs (Local or Remote Subsystems)
      * created by this layer manager
      *
      * @param index
      * the index of the Subsystem in the list
      */
    public ObjectName getSccpSap(Integer index);
    /** Returns an array of the ObjectNames of all SCCP Timer Profiles created by this
      * layer manager */
    public ObjectName[] getSccpTimerProfiles();
    /**
      * Returns the ObjectName of one of the SCCP Timer Profiles created by this layer
      * manager
      *
      * @param index
      * the index of the SCCP Timer Profile in the list
      */
    public ObjectName getSccpTimerProfile(Integer index);
    /** Returns an array of the ObjectName of all SCCP Routing Controls created by this
      * layer manager */
    public ObjectName[] getSccpRoutingControls();
    /**
      * Returns the ObjectName of one of the SCCP Routing Controls created by this layer
      * manager
      *
      * @param index
      * the index of the SCCP Routing Control in the list
      */
    public ObjectName getSccpRoutingControl(Integer index);
    /** Returns an array of the ObjectNames of all Global Tiltle Entries created by this
      * layer manager */
    public ObjectName[] getGlobalTitleEntries();
    /**
      * Returns the ObjectName of one of the Global Tiltle Entries created by this layer
      * manager
      *
      * @param index
      * the index of the Global Tiltle Entry in the list
      */
    public ObjectName getGlobalTitleEntry(Integer index);
    /** Returns an array of the ObjectName of all SCCP Entity Sets created by this layer
      * manager */
    public ObjectName[] getSccpEntitySets();
    /**
      * Returns the ObjectName of one of the SCCP Entity Sets created by this layer
      * manager
      *
      * @param index
      * the index of the SCCP Entity Set in the list
      */
    public ObjectName getSccpEntitySet(Integer index);
    /** Returns an array of the ObjectName of all Concerned Area MBeans created by this
      * layer manager */
    public ObjectName[] getConcernedAreas();
    /**
      * Returns the ObjectName of one of the Concerned Areas created by this layer manager
      *
      * @param index
      * the index of the Concerned Area in the list
      */
    public ObjectName getConcernedArea(Integer index);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
