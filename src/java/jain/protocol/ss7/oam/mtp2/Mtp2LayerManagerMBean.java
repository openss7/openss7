/*
 @(#) src/java/jain/protocol/ss7/oam/mtp2/Mtp2LayerManagerMBean.java <p>
 
 Copyright &copy; 2008-2015  Monavacon Limited <a href="http://www.monavacon.com/">&lt;http://www.monavacon.com/&gt;</a>. <br>
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
 <a href="http://www.openss7.com/">http://www.openss7.com/</a>
 */

package jain.protocol.ss7.oam.mtp2;

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
  * <center> [Instantiation.jpg] <center><br>
  * <center> This illustrates how JAIN OAM MBeans are instantiated <center><br> <p>
  * <center> [Mtp2MBeans.jpg] <center><br>
  * <center> Relationship between the MTP2 JAIN OAM MBean <center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface Mtp2LayerManagerMBean extends OamLayerManagerMBean, Mtp2ManagedObjectMBean {
    /**
      * Creates a new Mtp2 Timer Profile MBean that will use the correct Timer values for
      * the the protocol variant of this Mtp2LayerManager. When the Timer Profile is
      * created, the corresponding OamTimerVals should be created for each Timer with the
      * correct min, max and default values set.
      *
      * @return
      * The ObjectName of the newly created Mtp2TimerProfileMBean.
      *
      * @exception TooManyInstancesException
      */
    public ObjectName createMtp2TimerProfile()
        throws TooManyInstancesException;
    /**
      * Creates a new Mtp2SapMBean.
      *
      * @param portType
      * Either: <ul>
      *
      * <li>Mtp2SapMBean.PORT_TYPE_DTE - connects the transmit clock to the applicable
      * line interface circuit
      *
      * <li>Mtp2SapMBean.PORT_TYPE_DCE - connects the transmit clock to an internal baud
      * rate generator and provides this clock to the applicable line interface circuit.
      * </ul>
      *
      * @return
      * the ObjectName of the newly created Mtp2SapMBean
      *
      * @exception TooManyInstancesException
      * If the implementation cannot create any more SAPs.
      *
      * @exception IllegalAgrumentException
      * If the supplied port type is not one of the defined types.
      */
    public ObjectName createMtp2Sap(Integer portType)
        throws TooManyInstancesException, IllegalArgumentException;
    /**
      * Deletes the Mtp2TimerProfileMBean identified by the specified ObjectName. Note
      * that once a Mtp2TimerProfileMBean has been deleted, it should no longer be
      * returned by the getMtp2TimerProfiles() method.
      */
    public void deleteMtp2TimerProfile(ObjectName timerProfile);
    /**
      * Deletes the Mtp2SapMBean identified by the specified ObjectName. Note that once a
      * Mtp2SapMBean has been deleted, it should no longer be returned by the
      * getMtp2Saps() method.
      */
    public void deleteMtp2Sap(ObjectName sap);
    /**
      * Returns the ObjectName of one of the the Mtp2 Service Access Points
      * (Mtp2SapMBeans) <p>
      *
      * There is a subtle difference between this method and the getMtp2Saps() method in
      * that this method returns the ObjectName of one of the Mtp2SapMBeans that are
      * currently in use, whereas the getMtp2Saps() method returns the ObjectName of one
      * of all the Mtp2SapMBeans (both those Mtp2SapMBeans currently in use and the
      * Mtp2SapMBeans that have been configured are not available for use for any reason
      * including for backup). Every Mtp2SapMBeans that was created through the
      * createMtp2Sap() method shall be returned by the getMtp2Saps() method, but only
      * those MTP2 SAPs that have been activated using the setActiveMtp2Sap() method shall
      * be returned by this method.
      *
      * @param index
      * The index position of the SAP.
      *
      * @return
      * The ObjectName of the Mtp2SapMBean at the specified position.
      */
    public ObjectName getActiveMtp2Sap(Integer index);
    /**
      * Sets one of the Mtp2 Service Access Points (Mtp2SapMBeans) identified by the
      * specified ObjectName
      *
      * @param index
      * The index position of the SAP.
      *
      * @param mtp2Sap
      * The Mtp2SapMBean to be set at the specified position.
      *
      * @exception TooManyInstancesException
      * If index is greater than the number of Mtp2SapsMBeans permitted by this implementation.
      */
    public void setActiveMtp2Sap(Integer index, ObjectName mtp2Sap)
        throws TooManyInstancesException;
    /**
      * Returns the ObjectNames of all of the the Mtp2 Service Access Point MBeans
      * (Mtp2SapMBeans). <p>
      *
      * There is a subtle difference between this method and the getMtp2Saps() method in
      * that this method returns only the ObjectNames of the Mtp2SapMBeans that are
      * currently in use, whereas the getMtp2Saps() method returns the ObjectNames of all
      * Mtp2SapMBeans (both those Mtp2SapMBeans currently in use and the Mtp2SapMBeans
      * that have been configured are not available for use for any reason including for
      * backup). The ObjectNames of every Mtp2SapMBeans that was created through the
      * createMtp2Sap() method shall be returned by the getMtp2Saps() method, but only
      * those Mtp2SapMBeans that have been activated using the setActiveMtp2Sap() method
      * shall be returned by this method.
      *
      * @return
      * The ObjectNames of all of the Mtp2 Service Access Point MBeans (Mtp2SapMBeans).
      */
    public ObjectName[] getActiveMtp2Saps();
    /**
      * Sets all of the Mtp2 Service Access Points (Mtp2SapMBeans) identified by the
      * specified array of ObjectNames
      *
      * @param mtp2Saps
      * All of the the Mtp2 Service Access Point MBeans.
      *
      * @exception TooManyInstancesException
      * If the size of the array of ObjectNames is greater than the number of
      * Mtp2SapMBeans permitted by this implementation.
      *
      * @exception TooManyInstancesException
      * If no more SAPs can be activated.
      */
    public void setActiveMtp2Saps(ObjectName[] mtp2Saps)
        throws TooManyInstancesException;
    /**
      * Returns the ObjectNames of all of the Mtp2SapMBeans created by this layer manager
      */
    public ObjectName[] getMtp2Saps();
    /**
      * Returns the ObjectName of one of the Mtp2SapMBeans created by this layer manager
      *
      * @param index
      * The index of the MTP2 SAP in the list.
      */
    public ObjectName getMtp2Sap(Integer index);
    /**
      * Returns the ObjectNames of all of the Mtp2TimerProfileMBeans created by this layer
      * manager
      */
    public ObjectName[] getMtp2TimerProfiles();
    /**
      * Returns the ObjectName of one of the Mtp2TimerProfileMBeans created by this layer
      * manager
      *
      * @param index
      * The index of the MTP2 Timer Profiles in the list.
      */
    public ObjectName getMtp2TimerProfile(Integer index);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
