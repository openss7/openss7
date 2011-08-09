/*
 @(#) $RCSfile: OwnSignallingPointMBean.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:26 $ <p>
 
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
  * This interface defines the methods of a Local Signalling Point Managed Bean(MBean).
  * More than one local signalling point may be supported by the underlying stack
  * management system. This interface extends the MTP-3 Service Access Point MBean
  * interface. <p>
  *
  * There are no statistics that can be collected for this MBean <p>
  *
  * The following Alarms can be emitted by this MBean <ul>
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED </ul>
  *
  * The following Errors can be emitted by this MBean <ul>
  * <li>NON_OAM_RELATED_ERROR
  * <li>MTP3_ERROR_ROUTING_ERROR </ul>
  *
  * <center> [Mtp3MBeans.jpg] </center><br>
  * <center> Relationship between the MTP3 JAIN OAM MBeans </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface OwnSignallingPointMBean extends Mtp3ManagedObjectMBean, SignallingPointMBean {
    /**
      * Sets the Backup OwnSignallingPointMBean of this OwnSignallingPointMBean identified by the specified ObjectName.
      * @param backup
      * The ObjectName of the Backup OwnSignallingPointMBean.
      */
    public void setBackup(ObjectName backup);
    /**
      * Gets the ObjectName of the Backup OwnSignallingPointMBean of this
      * OwnSignallingPointMBean.
      * @return
      * The ObjectName of the Backup OwnSignallingPointMBean
      */
    public ObjectName getBackup();
    /**
      * Sets the RouteSetMBean (identified by the specified ObjectName) to the set of
      * RouteSetMBeans of this OwnSignallingPointMBean at the the specified position
      *
      * @param index
      * the index position of the element in the array
      *
      * @param routeSet
      * the ObjectName of the RouteSetMBean to be added.
      *
      * @exception TooManyInstancesException
      * If index is greater than the number of RouteSetMBeans permitted for an
      * OwnSignallingPointMBean by this implementation.
      */
    public void setRouteSet(Integer index, ObjectName routeSet)
        throws TooManyInstancesException;
    /**
      * Sets all of the RouteSetMBeans associated with this OwnSignallingPointMBean.
      *
      * @param routeSets
      * An array of all the ObjectNames of the RouteSetMBeans associated with this
      * OwnSignallingPointMBean.
      *
      * @exception TooManyInstancesException
      * If the size of routesets is greater than the number of RouteSetMBeans permitted
      * for a OwnSignallingPointMBean by this implementation.
      */
    public void setRouteSets(ObjectName[] routeSets)
        throws TooManyInstancesException;
    /**
      * Returns an array of ObjectNames of all the RouteSetMBeans associated with this
      * OwnSignallingPointMBean.
      *
      * @return
      * An array of all the RouteSetMBeans associated with this OwnSignallingPointMBean.
      */
    public ObjectName[] getRouteSets();
    /**
      * Returns the ObjectName of the RouteSetMBean at the specified position in all of
      * the RouteSetMBeans associated with this OwnSignallingPointMBean.
      *
      * @param index
      * The index position of the element in the array.
      *
      * @return
      * The RouteSetMBean at the specified position.
      */
    public ObjectName getRouteSet(Integer index);
    /**
      * Returns an array of ObjectNames of all the ScreeningTableMBeans associated with
      * this OwnSignallingPointMBean.
      *
      * @return
      * An array of ObjectNames of all the ScreeningTableMBeans.
      */
    public ObjectName[] getScreeningTables();
    /**
      * Returns the ObjectName ScreeningTableMBean at the specified position in the array
      * of all of the ScreeningTableMBeans associated with this OwnSignallingPointMBean.
      *
      * @param index
      * The index position of the element in the array.
      *
      * @return
      * The ObjectName of the ScreeningTableMBean at the specified position.
      */
    public ObjectName getScreeningTable(Integer index);
    /**
      * Sets all of the ScreeningTablesMBeans associated with this
      * OwnSignallingPointMBean.
      *
      * @param screeningTables
      * An array of ObjectNames of all ScreeningTableMBeans.
      *
      * @exception TooManyInstancesException
      * If the size of screeningTables is greater than the number of ScreeningTableMBeans
      * permitted for an OwnSignallingPointMBean by this implementation.
      *
      * @exception IllegalOperationException
      * If a screening table is being set for anything other than an STP.
      */
    public void setScreeningTables(ObjectName[] screeningTables)
        throws TooManyInstancesException, IllegalOperationException;
    /**
      * Sets the ScreeningTablesMBean (identified by the specified ObjectName) at the
      * specified position in the array of all of the ScreeningTablesMBeans associated
      * with this OwnSignallingPointMBean.
      *
      * @param index
      * the index position of the element in the array
      *
      * @param screeningTable
      * the ObjectName of the ScreeningTablesMBean to be set at the specified position
      *
      * @exception TooManyInstancesException
      * if index is greater than the number of ScreeningTablesMBeans permitted for a
      * OwnSignallingPointMBean by this implementation.
      *
      * @exception IllegalOperationException
      * if a screening table is being set for anything other than an STP.
      */
    public void setScreeningTable(Integer index, ObjectName screeningTable)
        throws TooManyInstancesException, IllegalOperationException;
    /**
      * Returns the ObjectName of the Mtp3TimerProfileMBean containing the MTP3 Timer
      * values to be used with this OwnSignallingPointMBean
      *
      * @return
      * The ObjectName of the MTtp3TimerProfileMBean.
      */
    public ObjectName getMtp3TimerProfile();
    /**
      * Sets the Mtp3TimerProfileMBean (identified by the specified ObjectName) containing
      * the MTP3 Timer values to be used with this OwnSignallingPointMBean
      *
      * @param timerProfile
      * The MTP3 Timer Profile MBean.
      */
    public void setMtp3TimerProfile(ObjectName timerProfile);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
