/*
 @(#) $RCSfile: RouteSetMBean.java,v $ $Name:  $($Revision: 1.1.2.2 $) $Date: 2011-09-02 08:46:43 $ <p>
 
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
 
 Last Modified $Date: 2011-09-02 08:46:43 $ by $Author: brian $
 */

package jain.protocol.ss7.oam.mtp3;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines the methods of a Routeset Managed Bean (RouteSetMBean). A
  * Routeset defines all of the signalling routes that may be used between an originating
  * point and a destination point by a message traversing the signalling network. <p>
  *
  * The destination signalling point of this Routeset can be determined from the
  * destination signalling point of any of its contained routes. <p>
  *
  * There are no statistics that can be collected for this MBean <p>
  *
  * The following Alarms can be emitted by this MBean <ul>
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED
  * <li>MTP3_ALARM_RS_START_UNAVAILABILITY
  * <li>MTP3_ALARM_RS_END_UNAVAILABILITY </ul>
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
public abstract interface RouteSetMBean extends Mtp3ManagedObjectMBean {
    public static final int CONG_UNCONGESTED = 0;
    public static final int CONG_CONGESTED_LEVEL1 = 1;
    public static final int CONG_CONGESTED_LEVEL2 = 2;
    public static final int CONG_CONGESTED_LEVEL3 = 3;
    /**
      * Sets whether or not this Routeset supports Load Sharing.
      *
      * @param loadSharing
      * either: <ul>
      * <li>true enables Load Sharing.
      * <li>false disables Load Sharing </ul>
      */
    public void setLoadSharing(Boolean loadSharing);
    /**
      * Indicates if this Routeset supports Load Sharing.
      *
      * @return
      * either <ul>
      * <li>true if this Routeset supports Load Sharing.
      * <li>false otherwise. </ul>
      */
    public Boolean isLoadSharing();
    /**
      * Indicates if the Destination Signalling Point of this Routeset is accessible (the
      * Operational State).
      *
      * @return
      * either:
      * <li>true if this Routeset's Destination Signalling Point is accessible (enabled).
      * <li>false otherwise (disabled). </ul>
      */
    public Boolean isSignallingPointAccessible();
    /**
      * Sets whether it is administratively permitted to route SS7 MSUs towards the
      * Destination Signalling Point of this Routeset (the Adiministrative State).
      *
      * @param allowed
      * either: <ul>
      * <li>true if routing is permitted (unlocked).
      * <li>false otherwise (locked). </ul>
      */
    public void setRoutingAllowed(Boolean allowed);
    /**
      * Returns whether it is administratively permitted to route SS7 MSUs towards the
      * Destination Signalling Point of this Routeset (the Adiministrative State).
      *
      * @return
      * either: <ul>
      * <li>true if routing is permitted (unlocked).
      * <li>false otherwise (locked). </ul>
      */
    public Boolean isRoutingAllowed();
    /**
      * Gets the Congestion Level of this Routeset. This reflects Routeset congestion.
      * Different vaules are valid depending on the protocol variant supported. <ul>
      *
      * <li>For ITU, with international option [Rec. Q.704 3.8.2.1 (a)] valid values are: <ul>
      *     <li>CONG_UNCONGESTED
      *     <li>CONG_CONGESTED_LEVEL1 </ul>
      * <li>For ITU, with national option [Rec. Q.704 3.8.2.1 (b)] OR for ANSI valid values are: <ul>
      *     <li>CONG_UNCONGESTED
      *     <li>CONG_CONGESTED_LEVEL1
      *     <li>CONG_CONGESTED_LEVEL2
      *     <li>CONG_CONGESTED_LEVEL3 </ul> </ul>
      *
      * @return
      * either: <ul>
      * <li>CONG_UNCONGESTED
      * <li>CONG_CONGESTED_LEVEL1
      * <li>CONG_CONGESTED_LEVEL2
      * <li>CONG_CONGESTED_LEVEL3 </ul>
      */
    public Integer getCongestionLevel();
    /**
      * Returns the ObjectNames RouteMBeans in this routeset
      *
      * @return
      * an array of the ObjectNames of the RouteMBeans in this routeset
      */
    public ObjectName[] getRoutes();
    /**
      * Sets the array of Routes (identified by the specified ObjectName) in this
      * routeset. Each route in this Routeset must have the same Destination Signalling
      * Point.
      *
      * @param routes
      * the routes to be set for this routeset
      *
      * @exception TooManyInstancesException
      * if the size of routes is greater than the number of routes permitted in a Routeset
      * by this implementation.
      */
    public void setRoutes(ObjectName[] routes)
        throws TooManyInstancesException;
    /**
      * Gets the ObjectName of the RouteMBean at the specified position in this RouteSet.
      *
      * @param index
      * the index of the element in the array
      *
      * @return
      * the ObjectName of the RouteMBean at the specified position in this RouteSet.
      */
    public ObjectName getRoute(Integer index);
    /**
      * Sets the route (identified by the specified ObjectName) at the specified position
      * in this routeset. The route to be set must have the same Destination Signalling
      * Point as this routeset.
      *
      * @param index
      * the index of the element in the array
      *
      * @param route
      * the ObjectName of the route to be set for this routeset
      *
      * @exception TooManyInstancesException
      * if index is greater than the number of Routes permitted in a Routeset by this
      * implementation.
      */
    public void setRoute(Integer index, ObjectName route)
        throws TooManyInstancesException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
