/*
 @(#) $RCSfile$ $Name$($Revision$) $Date$ <p>
 
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
 
 Last Modified $Date$ by $Author$
 */

package jain.protocol.ss7.oam.mtp3;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines the methods of a Route Managed Bean (RouteMBean). A Route
  * specifies the pre-determined path, consisting of a succession of signalling
  * points/signalling transfer points and the interconnecting signalling links, that a
  * message takes through the signalling network between the origination point and the
  * destination point. ie. A Route specifies the path an MSU should take to an adjacent or
  * far-end node. <p>
  *
  * There are no statistics that can be collected for this MBean <p>
  *
  * The following Alarms can be emitted by this MBean <ul>
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED </ul>
  *
  * The following Errors can be emitted by this MBean <ul>
  * <li>NON_OAM_RELATED_ERROR </ul>
  *
  * <center> [Mtp3MBeans.jpg] </center><br>
  * <center> Relationship between the MTP3 JAIN OAM MBeans </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface RouteMBean extends Mtp3ManagedObjectMBean {
    /**
      * Indicates that this Route is available. A signalling route becomes available when
      * a transfer-allowed message, indicating that signalling traffic towards a
      * particular destination can be transferred via the signalling transfer point
      * sending the concerned message, is received.
      */
    public static final int ROUTE_AVAILABLE = 1;
    /**
      * Indicates that this Route is unavailable. A signalling route becomes unavailable
      * when a transfer-prohibited message, indicating that signalling traffic towards a
      * particular destination cannot be transferred via the signalling transfer point
      * sending the concerned message, is received
      */
    public static final int ROUTE_UNAVAILABLE = 2;
    /**
      * Indicates that this Route is restricted. A signalling route becomes restricted
      * when a transfer-restricted message, indicating that the signalling traffic towards
      * a particular destination is being transferred with some difficulty via the
      * signalling transfer point sending the concerned message is received.
      */
    public static final int ROUTE_RESTRICTED = 3;
    /**
      * Gets the ObjectName of the Signalling point (SignallingPointMBean) that identifies
      * the Destination SignallingPointMBean of this RouteSetMBean. The
      * SignallingPointMBean may represent an adjacent or far-end node.
      *
      * @return
      * The ObjectName of the destination SignallingPointMBean of this RouteSetMBean.
      */
    public ObjectName getDestSignallingPoint();
    /**
      * Sets the Service Access Point (Mtp3SapMBean) that identifies the Destination
      * OwnSignallingPointMBean (identified by the specified ObjectName) of this Routeset.
      * The Mtp3SapMBean may represent an adjacent or far-end node.
      *
      * @param destinationSignallingPoint
      * the destination OwmSignallingPointMBean of this RouteMBean.
      */
    public void setDestSignallingPoint(ObjectName destinationSignallingPoint);
    /**
      * Gets the ObjectName of the Linkset which is intended to be used as first segment
      * of the succession of Linksets in this Route, which form the signalling route on
      * the network level.
      *
      * @return
      * the ObjectName of the first Linkset in this route
      */
    public ObjectName getFirstLinkSet();
    /**
      * Indicates if the respective Destination Signalling Point is accessible via the
      * firstLinkset of this Route (the Operational State).
      *
      * @return
      * either: <ul>
      * <li>true if the Signalling Point is accessible (enabled).
      * <li>false otherwise (disabled). </ul>
      */
    public Boolean isSignallingPointAccessible();
    /**
      * Sets whether it is administratively permitted to route SS7 MSUs towards the
      * Destination Signalling Point via the firstLinkset of this Route (the
      * Adiministrative State).
      *
      * @param allowed
      * either: <ul>
      * <li>true if routing is permitted (unlocked).
      * <li>false otherwise (locked). </ul>
      */
    public void setRoutingAllowed(Boolean allowed);
    /**
      * Returns whether it is administratively permitted to route SS7 MSUs towards the
      * Destination Signalling Point via the firstLinkset of this Route (the
      * Adiministrative State).
      *
      * @return
      * either: <ul>
      * <li>true if routing is permitted (unlocked).
      * <li>false otherwise (locked). </ul>
      */
    public Boolean isRoutingAllowed();
    /**
      * Returns the availability of this Route [ITU Rec. Q.704, 3.4].  This is a display
      * only attribute.
      *
      * @return
      * either: <ul>
      * <li>ROUTE_AVAILABLE
      * <li>ROUTE_UNAVAILABLE
      * <li>ROUTE_RESTRICTED </ul>
      */
    public Integer getAvailability();
    /**
      * Returns the priority of this Route within the containing Routeset. The returned
      * priority determines if this Route is used as current Route. Enabled Routes
      * (isSignallingPointAccessible() == true) contained in the same Routeset are chosen
      * in ascending order as current routes (The lower the value, the higher the
      * priority).
      *
      * @return
      * the priority (The lower the value, the higher the priority)
      */
    public Integer getPriority();
    /**
      * Sets the priority of this Route within the containing Routeset.  The supplied
      * priority will determine if this Route is used as current Route. Enabled Routes
      * (isSignallingPointAccessible() == true) contained in the same Routeset are chosen
      * in ascending order as current routes (The lower the value, the higher the
      * priority).
      *
      * @param priority
      * the priority (The lower the value, the higher the priority)
      *
      * @exception IllegalArgumentException
      * if the supplied priority is unnaceptable
      */
    public void setPriority(Integer priority)
        throws IllegalArgumentException;
    /**
      * Gets an array of ObjectNames of the list of Linksets in this route
      *
      * @return
      * an array of ObjectNames of the linksets in this route
      */
    public ObjectName[] getLinkSets();
    /**
      * Gets the ObjectName of the Linkset at the specified position in the set of
      * Linksets in this route
      *
      * @param index
      * the index position of the element in the array
      *
      * @return
      * the ObjectName of the linkset
      */
    public ObjectName getLinkSet(Integer index);
    /**
      * Sets a Linkset at the specified position in the Linkset (identified by the
      * specified ObjectName) in this route
      *
      * @param index
      * the index position of the element in the array
      *
      * @param linkset
      * the ObjectName of the Linkset to be added
      * 
      * @exception TooManyInstancesException
      * if index is greater than the number of LinksetMBeans permitted in a RouteMBean by
      * this implementation.
      */
    public void setLinkSet(Integer index, ObjectName linkset)
        throws TooManyInstancesException;
    /**
      * Sets the Linksets in this route identified by the specified ObjectName
      *
      * @param linksets
      * the ObjectNames of the Linksets that compose this route
      *
      * @exception TooManyInstancesException
      * if the size of linksets is greater than the number of LinksetMBeans permitted in a
      * RouteMBean by this implementation.
      */
    public void setLinkSets(ObjectName[] linksets)
        throws TooManyInstancesException;
    /**
      * Removes all Linksets from this Linkset
      */
    public void clearLinkSetList();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
