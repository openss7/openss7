/*
 @(#) src/java/jain/protocol/ss7/oam/mtp3/Mtp3AlarmNotification.java <p>
 
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
  * A Mtp3AlarmNotification is a Notification emitted by a Mtp3 Managed Bean(MBean) to
  * indicate a change in status of the MBean. This state change may result from: <ul>
  *
  * <li>a create/delete/modify operation by an application that expliciltly changes the
  * state of the MBean.
  *
  * <li>a change in the SS7 Network that implicitly changes the state of it's associated
  * MBean.
  *
  * <li>a timer within a Timer MBean expiring.  </ul>
  *
  * This Mtp3AlarmNotification should be sent to all applications that have registered
  * with the Mtp3 MBean as a javax.management.NotificationListener using a filter 'f',
  * where f.isNotificationEnabled(this.getType()) == true. <p>
  *
  * The Notification class extends the java.util.EventObject base class and defines the
  * minimal information contained in a notification. It contains the following fields:
  * <ul>
  *
  * <li>the <em>notification type</em>, which is a string expressed in a dot notation
  * similar to Java properties.
  *
  * <li>a <em>sequence number</em>, which is a serial number identifying a particular
  * instance of notification in the context of the notification source
  *
  * <li>a <em>time stamp</em>, indicating when the notification was generated
  *
  * <li>a <em>message</em> contained in a string, which could be the explanation of the
  * notification for displaying to a user
  *
  * <li><em>userData</em> is used for whatever other data the notification source wishes
  * to communicate to its consumers </ul>
  *
  * Notification sources should use the notification type to indicate the nature of the
  * event to their consumers. When additional information needs to be transmitted to
  * consumers, the source may place it in the message or user data fields. <p>
  *
  * <center> [NotificationHierarchy.jpg] </center><br>
  * <center> Inheritance hierarchy for JAIN OAM Notifications </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class Mtp3AlarmNotification extends OamAlarmNotification {
    /**
      * Alarm Type Constant: Signalling Link Faults and Performance: Indicates that there
      * has been a signalling link failure.
      * <br> Emitted By: LinkMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_failure".
      */
    public static final int MTP3_ALARM_SL_FAILURE = 1;
    /**
      * Alarm Type Constant: Signalling Link Faults and Performance: Indicates a
      * signalling link failure due to an excessive error rate of a signalling link.
      * <br> Emitted By: LinkMBean Reference: (ITU Q.703, 10.2.2)
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_excessive_error_rate".
      */
    public static final int MTP3_ALARM_SL_EXCESSIVE_ERROR_RATE = 2;
    /**
      * Alarm Type Constant: Signalling Link Faults and Performance: Indicates a
      * signalling link failure due to the excessive duration of congestion of a
      * signalling link.
      * <br> Emitted By: LinkMBean Reference: (ITU Q.703, 9.3)
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_excessive_congestion".
      */
    public static final int MTP3_ALARM_SL_EXCESSIVE_CONGESTION = 3;
    /**
      * Alarm Type Constant: Signalling Link Faults and Performance: Indicates a local
      * automatic changeover.
      * <br> Emitted By: LinkMBean Reference: (ITU Q.703, Clause 5)
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_local_auto_changeover".
      */
    public static final int MTP3_ALARM_SL_LOCAL_AUTO_CHANGEOVER = 4;
    /**
      * Alarm Type Constant: Signalling Link Faults and Performance: Indicates a local
      * automatic changeback.
      * <br> Emitted By: LinkMBean Reference: (ITU Q.703, Clause 6)
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_local_auto_changeback".
      */
    public static final int MTP3_ALARM_SL_LOCAL_AUTO_CHANGEBACK = 5;
    /**
      * Alarm Type Constant: Signalling Link Faults and Performance: Indicates the
      * restoration of a signalling link.
      * <br> Emitted By: LinkMBean Reference: (ITU Q.704, 3.2.3)
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_restoration".
      */
    public static final int MTP3_ALARM_SL_RESTORATION = 6;
    /**
      * Alarm Type Constant: Signalling Link Availability: Indicates the end of remote
      * processor outage.
      * <br> Emitted By: LinkMBean Reference: (ITU Q.704, 3.2.7)
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_end_remote_processor_outage".
      */
    public static final int MTP3_ALARM_SL_END_REMOTE_PROCESSOR_OUTAGE = 7;
    /**
      * Alarm Type Constant: Signalling Link Availability: Indicates the start of local
      * inhibition.
      * <br> Emitted By: LinkMBean Reference: (ITU Q.704, Clause 10)
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_start_local_inhibition".
      */
    public static final int MTP3_ALARM_SL_START_LOCAL_INHIBITION = 8;
    /**
      * Alarm Type Constant: Signalling Link Availability: Indicates the stop of local
      * inhibition.
      * <br> Emitted By: LinkMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_stop_local_inhibition".
      */
    public static final int MTP3_ALARM_SL_STOP_LOCAL_INHIBITION = 9;
    /**
      * Alarm Type Constant: Signalling Link Availability: Indicates the start of remote
      * inhibition.
      * <br> Emitted By: LinkMBean Reference: (ITU Q.704, Clause 10)
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_start_remote_inhibition".
      */
    public static final int MTP3_ALARM_SL_START_REMOTE_INHIBITION = 10;
    /**
      * Alarm Type Constant: Signalling Link Availability: Indicates the stop of remote
      * inhibition.
      * <br> Emitted By: LinkMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_stop_remote_inhibition".
      */
    public static final int MTP3_ALARM_SL_STOP_REMOTE_INHIBITION = 11;
    /**
      * Alarm Type Constant: Signalling Linkset and Routeset Availability: Indicates the
      * start of Linkset failure.
      * <br> Emitted By: LinksetMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.ls_start_failure".
      */
    public static final int MTP3_ALARM_LS_START_FAILURE = 12;
    /**
      * Alarm Type Constant: Signalling Linkset and Routeset Availability: Indicates the
      * end of Linkset failure.
      * <br> Emitted By: LinksetMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.ls_end_failure".
      */
    public static final int MTP3_ALARM_LS_END_FAILURE = 13;
    /**
      * Alarm Type Constant: Signalling Linkset and Routeset Availability: Indicates the
      * initiation of broedcast TFP due to failure of a measured linkset.
      * <br> Emitted By: LinksetMBean Reference: (ITU Q.704, Clause 13)
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.ls_broadcast_tfp".
      */
    public static final int MTP3_ALARM_LS_BROADCAST_TFP = 14;
    /**
      * Alarm Type Constant: Signalling Linkset and Routeset Availability: Indicates the
      * initiation of broedcast TFA due to the recovery of a measured linkset.
      * <br> Emitted By: LinksetMBean Reference: (ITU Q.704, Clause 13)
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.ls_broadcast_tfa".
      */
    public static final int MTP3_ALARM_LS_BROADCAST_TFA = 15;
    /**
      * Alarm Type Constant: Signalling Linkset and Routeset Availability: Indicates the
      * change in linkset used to adjacent Signalling Point.
      * <br> Emitted By: LinksetMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.ls_change".
      */
    public static final int MTP3_ALARM_LS_CHANGE = 16;
    /**
      * Alarm Type Constant: Signalling Linkset and Routeset Availability: Indicates the
      * start of unavailability of routeset to a given destination or set of destinations.
      * <br> Emitted By: RoutesetMBean Reference: (ITU Q.704, 11.2.1)
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.rs_start_unavailability".
      */
    public static final int MTP3_ALARM_RS_START_UNAVAILABILITY = 17;
    /**
      * Alarm Type Constant: Signalling Linkset and Routeset Availability: Indicates the
      * end of unavailability of routeset to a given destination or set of destinations.
      * <br> Emitted By: RoutesetMBean Reference: (ITU Q.704, 11.2.2)
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.rs_end_unavailability".
      */
    public static final int MTP3_ALARM_RS_END_UNAVAILABILITY = 18;
    /**
      * Alarm Type Constant: Mtp Signalling Point Status: Indicates that the adjacent
      * Signalling Point is inaccessible.
      * <br> Emitted By: Mtp3SapMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sp_inaccessible".
      */
    public static final int MTP3_ALARM_SP_INACCESSIBLE = 19;
    /**
      * Alarm Type Constant: Mtp Signalling Point Status: Indicates that an adjacent
      * Signalling Point is accessible.
      * <br> Emitted By: Mtp3SapMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sp_accessible".
      */
    public static final int MTP3_ALARM_SP_ACCESSIBLE = 20;
    /**
      * Alarm Type Constant: Link inhibit denied.
      * <br> Emitted By: LinkMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_inhibit_denied".
      */
    public static final int MTP3_ALARM_SL_INHIBIT_DENIED = 21;
    /**
      * Alarm Type Constant: Link uninhibit denied.
      * <br> Emitted By: LinkMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_uninhibit_denied".
      */
    public static final int MTP3_ALARM_SL_UNINHIBIT_DENIED = 22;
    /**
      * Alarm Type Constant: Link remotely blocked.
      * <br> Emitted By: LinkMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.remotely_blocked".
      */
    public static final int MTP3_ALARM_SL_REMOTELY_BLOCKED = 23;
    /**
      * Alarm Type Constant: Link remotely unblocked.
      * <br> Emitted By: LinkMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.remotely_unblocked".
      */
    public static final int MTP3_ALARM_SL_REMOTELY_UNBLOCKED = 24;
    /**
      * Alarm Type Constant: Link locally blocked.
      * <br> Emitted By: LinkMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_locally_blocked".
      */
    public static final int MTP3_ALARM_SL_LOCALLY_BLOCKED = 25;
    /**
      * Alarm Type Constant: Link locally unblocked.
      * <br> Emitted By: LinkMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.sl_locally_unblocked".
      */
    public static final int MTP3_ALARM_SL_LOCALLY_UNBLOCKED = 26;
    /**
      * Alarm Type Constant: Concerned destination point code (DPC) pause.
      * <br> Emitted By: Mtp2SapMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.concerned_sp_pause".
      */
    public static final int MTP3_ALARM_CONCERNED_SP_PAUSE = 27;
    /**
      * Alarm Type Constant: Concerned destination point code (DPC) resume.
      * <br> Emitted By: Mtp2SapMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.concerned_sp_resume".
      */
    public static final int MTP3_ALARM_CONCERNED_SP_RESUME = 28;
    /**
      * Alarm Type Constant: Concerned destination point code (DPC) network congested.
      * <br> Emitted By: Mtp2SapMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.concerned_sp_network_congested".
      */
    public static final int MTP3_ALARM_CONCERNED_SP_NETWORK_CONGESTED = 29;
    /**
      * Alarm Type Constant: Concerned destination point code (DPC) remote user
      * unavailable.
      * <br> Emitted By: Mtp2SapMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.concerned_sp_user_unavailable".
      */
    public static final int MTP3_ALARM_CONCERNED_SP_USER_UNAVAILABLE = 30;
    /**
      * Alarm Type Constant: Concerned destination point code (DPC) stop network
      * congestion.
      * <br> Emitted By: Mtp2SapMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.mtp3.alarm.concerned_sp_stop_net_cong".
      */
    public static final int MTP3_ALARM_CONCERNED_SP_STOP_NET_CONG = 31;
    /**
      * Constructs a new Mtp3AlarmNotification.
      *
      * @param source
      * The Managed MBean that emitted this Notification.
      *
      * @param sequenceNumber
      * The notification sequence number within the source object.
      *
      * @param priority
      * The priority of this Mtp3AlarmNotification.  This may be either (in increasing
      * order of priority): <ol>
      *
      * <li>{@link OamAlarmNotification#PRIORITY_INFORMATIONAL PRIORITY_INFORMATIONAL}
      * - The cause of this event does not affect the functional state
      * of the system and may be ignored.  This Mtp3AlarmNotification is
      * for purely informational purposes only.
      *
      * <li>{@link OamAlarmNotification#PRIORITY_LOW PRIORITY_LOW}
      * - Indicates that the state of the MBean that emitted this
      * Mtp3AlarmNotification has changed as a result of an operation
      * explicitly invoked by an application.
      *
      * <li>{@link OamAlarmNotification#PRIORITY_HIGH PRIORITY_HIGH}
      * - Indicates that the physical network element associated with
      * the MBean that emitted this Mtp3AlarmNotification has gone out
      * of service.  Without corrective action, service reliability can
      * be severely affected.
      *
      * <li>{@link OamAlarmNotification#PRIORITY_CRITICAL PRIORITY_CRITICAL}
      * - Indicates the complete failure of the physical netwrok element
      * associated with the MBean that emitted this
      * Mtp3AlarmNotification.  Immediate recovery action is required.
      * </ol>
      *
      * @param alarmType
      * The type of this alarm. This may be any one of the Alarm Type constants defined in
      * this class, or one of those inherited from OamAlarmNotification.
      *
      * @exception IllegalArgumentException
      * If any of the supplied parameters are invalid.
      */
    public Mtp3AlarmNotification(OamManagedObjectMBean source, long sequenceNumber, int priority, int alarmType) {
    }
    /**
      * Returns a string representation (with details) of classes which extend this class.
      * Over rides standard JAVA toString method.
      */
    public java.lang.String toString() {
        return new java.lang.String("");
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
