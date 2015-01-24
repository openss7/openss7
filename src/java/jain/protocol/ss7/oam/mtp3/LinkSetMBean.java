/*
 @(#) src/java/jain/protocol/ss7/oam/mtp3/LinkSetMBean.java <p>
 
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

package jain.protocol.ss7.oam.mtp3;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines the methods of a Linkset to an adjacent node.  Managed Bean.
  * <p>
  *
  * There are no statistics that can be collected for this MBean <p>
  *
  * The following Alarms can be emitted by this MBean <ul>
  *
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED
  * <li>MTP3_ALARM_LS_START_FAILURE
  * <li>MTP3_ALARM_LS_END_FAILURE
  * <li>MTP3_ALARM_LS_BROADCAST_TFP
  * <li>MTP3_ALARM_LS_BROADCAST_TFA
  * <li>MTP3_ALARM_LS_CHANGE </ul>
  *
  * The following Errors can be emitted by this MBean <ul>
  *
  * <li>NON_OAM_RELATED_ERROR </ul>
  *
  * <center> [Mtp3MBeans.jpg] </center><br>
  * <center> Relationship between the MTP3 JAIN OAM MBeans </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface LinkSetMBean extends Mtp3ManagedObjectMBean {
    /**
      * Gets the ObjectName of the Adjacent OwnSignallingPointMBean of this Linkset.
      *
      * @return
      * The ObjectName of the Adjacent OwnSignallingPointMBean.
      */
    public ObjectName getAdjacentSignallingPoint();
    /**
      * Sets the Adjacent OwnSignallingPointMBean (identified by the specified ObjectName)
      * of this LinksetMBean.
      *
      * @param adjacentSignallingPoint
      * The Adjacent Signalling Point of this Linkset.
      */
    public void setAdjacentSignallingPoint(ObjectName adjacentSignallingPoint);
    /**
      * Sets the Maximum Capacity of this Linkset. The maximum capacity of a signalling
      * linkset is the maximum load that should be placed on the linkset, when all
      * possible links are active and are working in service.
      *
      * @param capacity
      * The maximum capacity of the Linkset.
      *
      * @exception IllegalArgumentException
      * If the supplied capacity is unacceptable.
      */
    public void setMaxCapacity(Integer capacity)
        throws IllegalArgumentException;
    /**
      * Returns the Maximum Capacity of this Linkset. The maximum capacity of a signalling
      * linkset is the maximum load that should be placed on the linkset, when all
      * possible links are active and are working in service.
      *
      * @return
      * The maximum capacity of the Linkset.
      */
    public Integer getMaxCapacity();
    /**
      * Returns the Current Capacity of this Linkset. The current capacity of the
      * signalling linkset is the sum of the current capacities of the signalling links
      * (or the maximum capacities when the current capacities of the links of the linkset
      * are not defined) which are active, working and in service. This is a display only
      * attribute.
      *
      * @return
      * The Current Capacity of the Linkset.
      */
    public Integer getCurrentCapacity();
    /**
      * Activates or deactivates all the Links in this Linkset.
      *
      * @param active - either: <ul>
      *
      * <li>true This activates all the Links in this Linkset.
      * <li>false This deactivates all the Links in this Linkset. </ul>
      */
    public void setAllLinksActive(Boolean active);
    /**
      * Returns whether all the Links in this Linkset are active or innactive.
      *
      * @return
      * Either: <ul>
      *
      * <li>true All the links are active
      * <li>false All the links are innactive </ul>
      */
    public Boolean isAllLinksActive();
    /**
      * Gets the Number of this Linkset.
      *
      * @return
      * The Number of this Linkset.
      */
    public Integer getLinkCount();
    /**
      * Returns whether this Linkset is enabled or disabled (the operational state of this
      * Linkset).
      *
      * @return
      * Either: <ul>
      *
      * <li>true if this Linkset is enabled.
      * <li>false if this Linkset is disabled. </ul>
      */
    public Boolean isEnabled();
    /**
      * Sets the operational state of this Linkset.
      *
      * @param enabled
      * The opertaional state (enabled or disabled) either: <ul>
      *
      * <li>true enables this Linkset.
      * <li>false disables this Linkset. </ul>
      */
    public void setEnabled(Boolean enabled);
    /**
      * Sets whether it is administratively permitted to route SS7 MSUs towards the
      * Destination Signalling Point via this Linkset (the Adiministrative State).
      *
      * @param allowed
      * Either: <ul>
      *
      * <li>true if routing is permitted (unlocked).
      * <li>false otherwise (locked). </ul>
      */
    public void setRoutingAllowed(Boolean allowed);
    /**
      * Returns whether it is administratively permitted to route SS7 MSUs towards the
      * Destination Signalling Point via this Linkset (the Adiministrative State).
      *
      * @return
      * Either: <ul>
      *
      * <li>true if routing is permitted (unlocked).
      * <li>false otherwise (locked). </ul>
      */
    public Boolean isRoutingAllowed();
    /**
      * This attribute indicates whether the periodic test procedure of ITU Recommendation
      * Q.707 is applied to the links in this signalling linkset.
      *
      * @return
      * Either: <ul>
      *
      * <li>true if the links are tested periodically.
      * <li>false otherwise. </ul>
      */
    public Boolean isTestedPeriodically();
    /**
      * Gets the ObjectNames of the list of signalling linkMBeans of this linkSetMBean
      *
      * @return
      * An array of ObjectNames of all of the signalling linkMBeans in this linkSetMBean.
      */
    public ObjectName[] getLinks();
    /**
      * Sets the Signalling LinkMBean at the specified position in the set of links of
      * this linkSetMBean
      *
      * @param index
      * The index position of the element in the array.
      *
      * @param link
      * The ObjectName of Signalling linkMBean to be added to this linkSetMBean.
      *
      * @exception TooManyInstancesException
      * If the supplied index is greater than the number of Links permitted by this
      * implementation.
      */
    public void setLink(Integer index, ObjectName link)
        throws TooManyInstancesException;
    /**
      * Gets the ObjectName of the signalling link at the specified position in the set of
      * links of this linkset
      *
      * @param index
      * The index position of the element in the array.
      *
      * @return
      * The Link at the specified position in the Linkset.
      */
    public ObjectName getLink(Integer index);
    /**
      * Sets the Signalling Links of this linkset
      *
      * @param links
      * The Signalling links to be added to this linkset.
      *
      * @exception TooManyInstancesException
      * If the size of links is greater than the number of Links permitted by this
      * implementation.
      */
    public void setLinks(ObjectName[] links)
        throws TooManyInstancesException;
    /**
      * Returns an array of ObjectNames of the LinkSetMBeans with which this Linkset is
      * combined.
      */
    public ObjectName[] getCombinedLinkSets();
    /**
      * Returns the ObjectName of the LinkSet at the specified position in the array of
      * the LinkSetMBeans with which this Linkset is combined.
      *
      * @param index
      * The index of the Linkset in the Combined Linkset array.
      */
    public ObjectName getCombinedLinkSet(Integer index);
    /**
      * Sets the LinksetMBean at the specified position in the combined linkset of this
      * linkset
      *
      * @param index
      * The index position of the element in the array.
      *
      * @param linkSet
      * The linkset to be combined with this linkset.
      *
      * @exception TooManyInstancesException
      * If the supplied index is greater than the number of LinksetMBeans permitted in a
      * combined Linkset by this implementation.
      */
    public void setCombinedLinkSet(Integer index, ObjectName linkSet)
        throws TooManyInstancesException;
    /**
      * Sets the combined Linkset of this linkset
      *
      * @param linkSets
      * The Signalling linksets to be combined with this linkset.
      *
      * @exception TooManyInstancesException
      * If the size of linksets is greater than the number of LinksetMBeans permitted in a
      * combined Linkset by this implementation.
      */
    public void setCombinedLinkSets(ObjectName[] linkSets)
        throws TooManyInstancesException;
    /**
      * Returns the number of links that should be active (alligned and ready for service)
      * in this Linkset at all times. This attribute represents the number of normally
      * active signalling links in this Linkset. i.e. - the number of links in the Linkset
      * that are to be aligned and ready for service at all times.
      *
      * @return
      * The specified number of active links.
      */
    public Integer getActiveLinkCount();
    /**
      * Sets the number of links that should be active (alligned and ready for service) in
      * this Linkset at all times. This attribute represents the number of normally active
      * signalling links in this Linkset. i.e. - the number of links in the Linkset that
      * are to be aligned and ready for service at all times.
      *
      * @param numberOfActiveLinks
      * The required number of active links.
      *
      * @exception IllegalArgumentException
      * If the specified number represents an unnacceptable value.
      */
    public void setActiveLinkCount(Integer numberOfActiveLinks)
        throws IllegalArgumentException;
    /**
      * Returns whether this Linkset is a combined Linkset
      */
    public Boolean isCombinedLinkSet();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
