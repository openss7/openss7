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
  * This interface defines the methods of a Link Managed Bean. A number of LinkMBeans are
  * combined together to form a LinksetMBean. Each LinkMBean can be added to a previously
  * defined LinksetMBean by invoking the setLink() method of the LinksetMBean and by
  * passing the ObjectName of the LinkMBean. <p>
  *
  * The following statistics can be collected for this MBean <ul>
  *
  * <li>MTP3_DUR_LINK_IN_SERV
  * <li>MTP3_SL_FAILURE
  * <li>MTP3_NUM_SIG_UNITS_RECEIVED_ERR
  * <li>MTP3_DUR_SL_UNAVAILABILITY </ul>
  *
  * The following Alarms can be emitted by this MBean <ul>
  *
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED
  * <li>MTP3_ALARM_SL_FAILURE
  * <li>MTP3_ALARM_SL_EXCESSIVE_ERROR_RATE
  * <li>MTP3_ALARM_SL_EXCESSIVE_CONGESTION
  * <li>MTP3_ALARM_SL_LOCAL_AUTO_CHANGEOVER
  * <li>MTP3_ALARM_SL_LOCAL_AUTO_CHANGEBACK
  * <li>MTP3_ALARM_SL_RESTORATION
  * <li>MTP3_ALARM_SL_END_REMOTE_PROCESSOR_OUTAGE
  * <li>MTP3_ALARM_SL_START_LOCAL_INHIBITION
  * <li>MTP3_ALARM_SL_STOP_LOCAL_INHIBITION
  * <li>MTP3_ALARM_SL_START_REMOTE_INHIBITION
  * <li>MTP3_ALARM_SL_STOP_REMOTE_INHIBITION
  * <li>MTP3_ALARM_SL_INHIBIT_DENIED
  * <li>MTP3_ALARM_SL_UNINHIBIT_DENIED
  * <li>MTP3_ALARM_SL_REMOTELY_BLOCKED
  * <li>MTP3_ALARM_SL_REMOTELY_UNBLOCKED
  * <li>MTP3_ALARM_SL_LOCALLY_BLOCKED
  * <li>MTP3_ALARM_SL_LOCALLY_UNBLOCKED </ul>
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
public abstract interface LinkMBean extends Mtp3ManagedObjectMBean {
    /** A Discard Level constant: level 1 */
    public static final int DISCARD_LEVEL_1 = 1;
    /** A Discard Level constant: level 2 */
    public static final int DISCARD_LEVEL_2 = 2;
    /** A Discard Level constant: level 3 */
    public static final int DISCARD_LEVEL_3 = 3;
    /** A Congestion Level constant: uncongested */
    public static final int CONG_UNCONGESTED = 0;
    /** A Congestion Level constant: level 1 */
    public static final int CONG_CONGESTED_LEVEL1 = 1;
    /** A Congestion Level constant: level 2 */
    public static final int CONG_CONGESTED_LEVEL2 = 2;
    /** A Congestion Level constant: level 3 */
    public static final int CONG_CONGESTED_LEVEL3 = 3;
    /**
      * Sets the Signalling Link Code (SLC)of this Signalling Link. The SLC is the same
      * value (between 0 and 15) at each end of the link, and is different from that of
      * any other link between the same two adjacent signalling points.
      *
      * @exception IllegalArgumentException
      * If the supplied signalling link code is not an acceptable value.
      */
    public void setSignallingLinkCode(Integer signallingLinkCode)
        throws IllegalArgumentException;
    /**
      * Gets the Signalling Link Code (SLC) parameter of this Signalling Link. The SLC is
      * the same value (between 0 and 15) at each end of the link, and is different from
      * that of any other link between the same two adjacent signalling points.
      */
    public Integer getSignallingLinkCode();
    /**
      * Returns the Maximum Capacity of this Link. The maximum capacity of a signalling
      * link is the maximum load that should be placed on the signalling link. This can be
      * expressed as the number of Message Signal Units (and octets) transportable each
      * way per second. This is a display only attribute.
      *
      * @return
      * The maximum capacity of this Link expressed as the number of Message Signal Units
      * (and octets) per second.
      */
    public Integer getMaxCapacity();
    /**
      * Returns the Current Capacity of this Link. The current capacity of a Link
      * (implementation dependent, consequently an optional attribute) is the maximum load
      * that should be placed on the Link given the present configuration of Signalling
      * Data Links (SDLs) to signalling terminals.
      *
      * Could be used where Signalling Terminal (ST) capacities might be different in an
      * implementation and automatic allocation of SDL to ST can be done, and/or where a
      * signalling terminal could control several Links. The current capacity is the
      * calculated maximum message traffic (MSUs and octets) that can be carried with the
      * currently available links. This is a display only attribute.
      *
      * @return
      * The Current Capacity of this Link.
      */
    public Integer getCurrentCapacity();
    /**
      * Sets the status of this Link (as defined in ITU Reccmmendation Q.704).
      *
      * @param status
      * One of the following: <ul>
      *
      * <li>STATUS_LOCALLY_BLOCKED
      * <li>STATUS_REMOTELY_BLOCKED
      * <li>STATUS_LOCALLY_INHIBITED
      * <li>STATUS_REMOTELY_INHBITED
      * <li>STATUS_FAILED
      * <li>STATUS_DEACTIVATED </ul>
      */
    public void setStatus(Integer status);
    /**
      * Returns whether this Link is enabled or disabled (the operational state of this
      * Link).
      *
      * @return
      * Either: <ul>
      * <li>true if this Link is enabled.
      * <li>false if this Link is disabled. </ul>
      */
    public Boolean isEnabled();
    /**
      * Returns the Locally Blocked Status parameter of this Signalling Link.
      */
    public Boolean isLocallyBlocked();
    /**
      * Sets the operational state of this link.
      *
      * @param enabled
      * The opertaional state (enabled or disabled) either: <ul>
      *
      * <li>true enables this Link.
      * <li>false disables this Link. </ul>
      */
    public void setEnabled(Boolean enabled);
    /**
      * Sets whether it is administratively permitted to route SS7 MSUs towards the
      * Destination Signalling Point via this Link (the Adiministrative State).
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
      * Destination Signalling Point via this Link (the Adiministrative State).
      *
      * @return
      * Either: <ul>
      *
      * <li>true if routing is permitted (unlocked).
      * <li>false otherwise (locked). </ul>
      */
    public Boolean isRoutingAllowed();
    /**
      * Gets the Congestion Level of this Link. This reflects Link congestion. Different
      * values are valid depending on the protocol variant supported. <ul>
      *
      * <li>For ITU, with international option [Rec. Q.704 3.8.2.1 (a)] valid values are: <ul>
      *     <li>CONG_UNCONGESTED
      *     <li>CONG_CONGESTED_LEVEL1 </ul>
      *
      * <li>For ITU, with national option [Rec. Q.704 3.8.2.1 (b)] OR for ANSI valid values are: <ul>
      *     <li>CONG_UNCONGESTED
      *     <li>CONG_CONGESTED_LEVEL1
      *     <li>CONG_CONGESTED_LEVEL2
      *     <li>CONG_CONGESTED_LEVEL3 </ul> </ul>
      *
      * @return
      * Either: <ul>
      *
      * <li>CONG_UNCONGESTED
      * <li>CONG_CONGESTED_LEVEL1
      * <li>CONG_CONGESTED_LEVEL2
      * <li>CONG_CONGESTED_LEVEL3 </ul>
      */
    public Integer getCongestionLevel();
    /**
      * Activates or deactivates this Signalling Link. When the link is originally created
      * it will be deactivated by default.
      *
      * @param active
      * Either: <ul>
      *
      * <li>true This activates the link (puts the link into operation)and allows
      * alignment of the link to take place.
      *
      * <li>false This deactivates the link (causing it to go out of alignment and to stop
      * carrying traffic). </ul>
      */
    public void setActive(Boolean active);
    /**
      * Returns whether this Signalling Link is active or innactive.
      *
      * @return
      * Either: <ul>
      *
      * <li>true The link is active. (the Signalling Link may have been activated for
      * carrying traffic by operator command
      *
      * <li>false The link is inactive (the Link may have been prevented from aligning or
      * carrying traffic autonomously or may have been deactivated by operator command.
      * </ul>
      */
    public Boolean isActive();
    /**
      * Sets the Locally Blocked Status parameter of this Signalling Link.
      */
    public void setLocallyBlocked(Boolean locallyBlocked);
    /**
      * Sets the Link Normal Status parameter of this Signalling Link.
      */
    public void setLinkNormal(Boolean linkNormal);
    /**
      * Sets the Installed Status parameter of this Signalling Link.
      */
    public void setInstalled(Boolean installed);
    /**
      * Sets the Signalling Link testing.
      *
      * @param signallingLinkTestOn
      * TRUE if link testing is on, FALSE otherwise.
      */
    public void setSignallingLinkTestOn(Boolean signallingLinkTestOn);
    /**
      * Returns the Signalling Link testing.
      *
      * @return signallingLinkTestOn
      * TRUE if link testing is on, FALSE otherwise.
      */
    public Boolean isSignallingLinkTestOn();
    /**
      * Sets the periodic testing of this signalling link
      *
      * @param testPeriodically
      * TRUE if periodic link testing is enabled.
      */
    public void setTestPeriodically(Boolean testPeriodically);
    /**
      * Returns whether the link is tested periodically.
      *
      * @return
      * TRUE if the link is tested periodically.
      */
    public Boolean isTestPeriodically();
    /**
      * Sets the value at which a link congestion situation starts for the specified
      * congestion level.
      *
      * @param congestionLevel
      * One of the congestion level constants.
      *
      * @param congestionOnset
      * The value at which a link congestion situation starts for the specified congestion
      * level.
      *
      * @exception IllegalArgumentException
      * If the congestion level is not one of those defined.
      */
    public void setCongestionOnset(Integer congestionLevel, Integer congestionOnset)
        throws IllegalArgumentException;
    /**
      * Returns the value at which a link congestion situation starts for the specified
      * congestion level.
      *
      * @param congestionLevel
      * One of the congestion level constants.
      *
      * @return
      * The value at which a link congestion situation starts for the specified congestion level.
      *
      * @exception IllegalArgumentException
      * If the congestion level is not one of those defined.
      */
    public Integer getCongestionOnset(Integer congestionLevel)
        throws IllegalArgumentException;
    /**
      * Sets the value at which a link congestion situation ends for the specified
      * congestion level.
      *
      * @param congestionLevel
      * One of the congestion level constants.
      *
      * @param congestionAbate
      * The value at which a link congestion situation ends for the specified congestion level.
      *
      * @exception IllegalArgumentException
      * If the congestion level is not one of those defined.
      */
    public void setCongestionAbate(Integer congestionLevel, Integer congestionAbate)
        throws IllegalArgumentException;
    /**
      * Returns the value at which a link congestion situation ends for the specified
      * congestion level.
      *
      * @param congestionLevel
      * One of the congestion level constants.
      *
      * @return
      * The value at which a link congestion situation ends for the specified congestion
      * level.
      *
      * @exception IllegalArgumentException
      * If the congestion level is not one of those defined.
      */
    public Integer getCongestionAbate(Integer congestionLevel)
        throws IllegalArgumentException;
    /**
      * Sets the value at which MSUs are discarded for the specified discard level.
      *
      * @param discardLevel
      * One of the discard level constants.
      *
      * @param discardOnset
      * The value MSUs are discarded for the specified discard level.
      *
      * @exception IllegalArgumentException
      * If the discard level is not one of those defined.
      */
    public void setDiscardOnset(Integer discardLevel, Integer discardOnset)
        throws IllegalArgumentException;
    /**
      * Returns the value at which MSUs are discarded for the specified discard level.
      *
      * @param discardLevel
      * One of the discard level constants.
      *
      * @return
      * The value at which MSUs are discarded for the specified discard level.
      *
      * @exception IllegalArgumentException
      * If the discard level is not one of those defined
      */
    public Integer getDiscardOnset(Integer discardLevel)
        throws IllegalArgumentException;
    /**
      * Sets the value at which MSUs are no longer discarded for the specified discard
      * level.
      *
      * @param discardLevel
      * One of the discard level constants.
      *
      * @param discardAbate
      * The value MSUs are no longer discarded for the specified discard level.
      *
      * @exception IllegalArgumentException
      * If the discard level is not one of those defined.
      */
    public void setDiscardAbate(Integer discardLevel, Integer discardAbate)
        throws IllegalArgumentException;

    /**
      * Returns the value at which MSUs are no longer discarded for the specified discard
      * level.
      *
      * @param discardLevel
      * One of the discard level constants.
      *
      * @return
      * The value at which MSUs are no longer discarded for the specified discard level.
      *
      * @exception IllegalArgumentException
      * If the discard level is not one of those defined.
      */
    public Integer getDiscardAbate(Integer discardLevel)
        throws IllegalArgumentException;
    /**
      * Returns the ObjectName of the Mtp2 Service Access Point MBean (Mtp2SapMBean)
      * associated with this Signalling Link (LinkMBean).
      */
    public ObjectName getMtp2Sap();
    /**
      * Sets the Mtp2 Service Access Point MBean (identified by the specified ObjecName)
      * associated with this Signalling Link (LinkMBean).
      */
    public void setMtp2Sap(ObjectName mtp2SapMBean);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
