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

package jain.protocol.ss7.oam.sccp;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * A SccpAlarmNotification is an Notification emitted by a Sccp Managed Bean (MBean) to
  * indicate a change in status of the MBean. This state change may result from : <ul>
  *
  * <li>a create/delete/modify operation by an application that expliciltly changes the
  * state of the MBean.
  *
  * <li>a change in the SS7 Network that implicitly changes the state of it's associated
  * MBean.
  *
  * <li>a timer within a Timer MBean expiring. </ul>
  *
  * This SccpAlarmNotification should be sent to all applications that have registered
  * with the Sccp MBean as a javax.management.NotificationListener using a filter 'f',
  * where f.isNotificationEnabled(this.getType()) == true. <p>
  *
  * The Notification class extends the java.util.EventObject base class and defines the
  * minimal information contained in a notification. It contains the following fields:
  * <ul>
  *
  * <li> the <em>notification type</em>, which is a string expressed in a dot notation
  * similar to Java properties.
  *
  * <li> a <em>sequence number</em>, which is a serial number identifying a particular
  * instance of notification in the context of the notification source
  *
  * <li> a <em>time stamp</em>, indicating when the notification was generated
  *
  * <li> a <em>message</em> contained in a string, which could be the explanation of the
  * notification for displaying to a user
  *
  * <li> <em>userData</em> is used for whatever other data the notification source wishes
  * to communicate to its consumers <ul>
  *
  * Notification sources should use the notification type to indicate the nature of the
  * event to their consumers. When additional information needs to be transmitted to
  * consumers, the source may place it in the message or user data fields. <p>
  *
  * <center> [NotificationHierarchy.jpg] </center><br>
  * <center> Inheritance hierarchy for JAIN OAM Notification </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class SccpAlarmNotification extends OamAlarmNotification {
    /**
      * Alarm Type Constant: Sccp Subsystem Availability: Indicates the start of a local
      * SCCP unavailable due to failure. <p>
      * <br> Emitted By: SccpSapMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.ssn_unavailable_failure".
      */
    public static final int SCCP_ALARM_SSN_UNAVAILABLE_FAILURE = 1;
    /**
      * Alarm Type Constant: Sccp Subsystem Availability: Indicates the start of a local
      * SCCP unavailable due to maintenance. <p>
      * <br> Emitted By: SccpSapMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.ssn_unavailable_maintenance".
      */
    public static final int SCCP_ALARM_SSN_UNAVAILABLE_MAINTENANCE = 2;
    /**
      * Alarm Type Constant: Sccp Subsystem Availability: Indicates the start of a local
      * SCCP unavailable due to congestion. <p>
      * <br> Emitted By: SccpSapMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.ssn_unavailable_congestion".
      */
    public static final int SCCP_ALARM_SSN_UNAVAILABLE_CONGESTION = 3;
    /**
      * Alarm Type Constant: Sccp Subsystem Availability: Indicates that a local SCCP has
      * become available, after being unavailable for any reason. <p>
      * <br> Emitted By: SccpSapMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.ssn_available".
      */
    public static final int SCCP_ALARM_SSN_AVAILABLE = 4;
    /**
      * Alarm Type Constant: Sccp Subsystem Availability: Indicates that a subsystem of
      * out-of-service grant message has benn received. <p>
      * <br> Emitted By: SccpSapMBean Reference: (ITU Q.714, 5.3.5.3)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.ssn_oos_grant".
      */
    public static final int SCCP_ALARM_SSN_OOS_GRANT = 5;
    /**
      * Alarm Type Constant: Sccp Subsystem Availability: Indicates that a subsystem of
      * out-of-service request has been denied. <p>
      * <br> Emitted By: SccpSapMBean Reference: (ITU Q.714, 5.3.5.3)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.ssn_oos_denied".
      */
    public static final int SCCP_ALARM_SSN_OOS_DENIED = 6;
    /**
      * Alarm Type Constant: Sccp Subsystem Availability: Indicates the start of a
      * Subsystem prohibited. <p>
      * <br> Emitted By: SccpSapMBean Reference: (ITU Q.714, 5.3.2)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.ssn_start_prohibited".
      */
    public static final int SCCP_ALARM_SSN_START_PROHIBITED = 7;
    /**
      * Alarm Type Constant: Sccp Subsystem Availability: Indicates the end of a Subsystem
      * prohibited. <p>
      * <br> Emitted By: SccpSapMBean Reference: (ITU Q.714, 5.3.2)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.ssn_end_prohibited".
      */
    public static final int SCCP_ALARM_SSN_END_PROHIBITED = 8;
    /**
      * Alarm Type Constant: Sccp Subsystem Availability: Indicates that a Subsystem
      * allowed message has been received. <p>
      * <br> Emitted By: SccpSapMBean Reference: (ITU q.714, 5.3.3)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.ssn_allowed_rx".
      */
    public static final int SCCP_ALARM_SSN_ALLOWED_RX = 9;
    /**
      * Alarm Type Constant: Quality Of Service: Too Large For Segmentation. This
      * notification is sent by the SCLC when the length of the user data is greater than
      * 3952 octets. <p>
      * <br> Emitted By: SccpSapMBean Reference: (ITU Q.752, 7.14)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_too_large".
      */
    public static final int SCCP_ALARM_QOS_TOO_LARGE = 10;
    /**
      * Alarm Type Constant: Quality Of Service: Reassembly TimeOut .  This notification
      * is sent by one of the Reassembly Timers when this timer expires. This measurement
      * is obligatory at SCCP nodes where SCCP segmentation and reassembly is supported.
      * <p>
      * <br> Emitted By: SccpRoutingControlMBean Reference: (ITU Q.752/7.10)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_reassembly_timeout".
      */
    public static final int SCCP_ALARM_QOS_REASSEMBLY_TIMEOUT = 11;
    /**
      * Alarm Type Constant: Quality Of Service: Segment Out Of Order .  This notification
      * is generated if one of the reassembly processes receives a segment that is out of
      * order. This measurement is obligatory at SCCP nodes where SCCP segmentation and
      * reassembly is supported. <p>
      * <br> Emitted By: SccpRoutingControlMBean Reference: (ITU Q.752/7.11)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_segment_out_of_order".
      */
    public static final int SCCP_ALARM_QOS_SEGMENT_OUT_OF_ORDER = 12;
    /**
      * Alarm Type Constant: Quality Of Service: No Reassembly Space.  This notification
      * is sent by the SCLC to indicate a resource limitation when the first segment of a
      * sequence is received.  This measurement is obligatory at SCCP nodes where SCCP
      * segmentation and reassembly is supported. <p>
      * <br> Emitted By: SccpRoutingControlMBean Reference: (ITU Q.752/7.12)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_no_reassembly_space".
      */
    public static final int SCCP_ALARM_QOS_NO_REASSEBLY_SPACE = 13;
    /**
      * Alarm Type Constant: Quality Of Service: No Segmentation Support . This
      * notification is sent by the SCLC when a message has to be segmented but the
      * segmentation function is not implemented. <p>
      * <br> Emitted By: SccpRoutingControlMBean Reference: (ITU Q.752/7.19)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_no_segmentation_support".
      */
    public static final int SCCP_ALARM_QOS_NO_SEGEMENTATION_SUPPORT = 14;
    /**
      * Alarm Type Constant: Quality Of Service: Segmentation Failure.  This notification
      * is sent by the SCLC to indicate that segmentation failed due to, for example, a
      * lack of resources. <p>
      * <br> Emitted By: SccpSapMBean Reference: (ITU Q.752/7.20)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_segmentation_failure".
      */
    public static final int SCCP_ALARM_QOS_SEGMENTATION_FAILURE = 15;
    /**
      * Alarm Type Constant: Quality Of Service: Reassembly Failure. <p>
      * <br> Emitted By: SccpRoutingControlMBean Reference: (ITU Q.752/7.21)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_reassembly_failure".
      */
    public static final int SCCP_ALARM_QOS_REASSEMBLY_FAILURE = 16;
    /**
      * Alarm Type Constant: Alarm Type Constant: No Translator For Address. The failure
      * occurs when the translation of the Global Title fails because the type of address
      * is unknown to the translation function. This measurement is only required at SCCP
      * nodes with global title translation capabilities. <p>
      * <br> Emitted By: SccpRoutingControlMBean Reference: (ITU Q.752/7.1)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_no_translation_for_addr".
      */
    public static final int SCCP_ALARM_QOS_NO_TRANSLATOR_FOR_ADDR = 17;
    /**
      * Alarm Type Constant: Quality Of Service: No Rule For Address .  The failure occurs
      * when the translation of the Global Title fails because the address cannot be
      * translated although the type is known to the translation function. This
      * measurement is only required at SCCP nodes with global title translation
      * capabilities. <p>
      * <br> Emitted By: SccpRoutingControlMBean Reference: (ITU Q.752/7.2)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_no_rule_for_addr".
      */
    public static final int SCCP_ALARM_QOS_NO_RULE_FOR_ADDR = 18;
    /**
      * Alarm Type Constant: Quality Of Service: Point Code Not Available . The failure
      * occurs when the translation of the Global Title fails because no available route
      * could be found for the concerned destination address, due to failures in MTP
      * and/or SCCP. <p>
      * <br> Emitted By: SccpRoutingControlMBean Reference: (ITU Q.752/7.31)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_pc_not_available".
      */
    public static final int SCCP_ALARM_QOS_PC_NOT_AVAILABLE = 19;
    /**
      * Alarm Type Constant: Quality Of Service: Point Code Congested .  The failure
      * occurs when the translation of the Global Title fails because no available route
      * could be found for the concerned destination address, due to congestion in MTP
      * and/or SCCP. <p>
      * <br> Emitted By: SccpRoutingControlMBean Reference: (ITU Q.752/7.4)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_pc_congested".
      */
    public static final int SCCP_ALARM_QOS_PC_CONGESTED = 20;
    /**
      * Alarm Type Constant: Quality Of Service: Subsystem Unavailable.  The failure
      * occurs when the translation of the Global Title fails because no available route
      * could be found for the concerned destination address, due to failures in the SCCP
      * subsystem. <p>
      * <br> Emitted By: SccpSapMBean Reference: (ITU Q.752/7.5)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_ssn_unavailable".
      */
    public static final int SCCP_ALARM_QOS_SSN_UNAVAILABLE = 21;
    /**
      * Alarm Type Constant: Quality Of Service: Subsystem Congested .  The failure occurs
      * when the translation of the Global Title fails because no available route could be
      * found for the concerned destination address, due to congestion in the SCCP
      * subsystem. <p>
      * <br> Emitted By: SccpSapMBean Reference: (ITU Q.752/7.6)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_ssn_congested".
      */
    public static final int SCCP_ALARM_QOS_SSN_CONGESTED = 22;
    /**
      * Alarm Type Constant: Quality Of Service: Unequiped Subsystem.  The failure occurs
      * when the received SSN is unequipped in the local node. <p>
      * <br> Emitted By: SccpSapMBean Reference: (ITU Q.752/7.7)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_ssn_unequiped".
      */
    public static final int SCCP_ALARM_QOS_SSN_UNEQUIPED = 23;
    /**
      * Alarm Type Constant: Quality Of Service: Syntax Error Detected . The failure
      * occurs when syntax errors are detected in the SCCP message. <p>
      * <br> Emitted By: SccpSapMBean Reference: (ITU Q.752/7.8)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_syntax_error".
      */
    public static final int SCCP_ALARM_QOS_SYNTAX_ERROR = 24;
    /**
      * Alarm Type Constant: Quality Of Service: Routing Failure (no reason or
      * unqualified). <p>
      * <br> Emitted By: SccpRoutingControlMBean Reference: (ITU Q.752/7.9)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_routing_failure".
      */
    public static final int SCCP_ALARM_QOS_ROUTING_FAILURE = 25;
    /**
      * Alarm Type Constant: Quality Of Service: Hop Counter Violation.  A Hop Counter
      * Violation occurs when the Hop Counter is zero.  The Hop Counter is used to detect
      * circular routings. This counter is decremented each time a Global Title
      * translation occurs. The Hop Counter is only part of specific SCCP messages:
      * Extended Unitdata, Extended Unitdata Service, CR and LUDT(S). <p>
      * <br> Emitted By: SccpRoutingControlMBean Reference: (ITU Q.752/7.13)
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.alarm.qos_hop_counter_violation".
      */
    public static final int SCCP_ALARM_QOS_HOP_COUNTER_VIOLATION = 26;
    /**
      * Constructs a new SccpAlarmNotification.
      *
      * @param source
      * The Managed MBean that emitted this Notification.
      *
      * @param sequenceNumberThe
      * notification sequence number within the source object.
      *
      * @param priority
      * The priority of this SccpAlarmNotification.  This may be either (in increasing
      * order of priority): <ol>
      *
      * <li>{@link OamAlarmNotification#PRIORITY_INFORMATIONAL PRIORITY_INFORMATIONAL}
      * - The cause of this event does not affect the functional state
      * of the system and may be ignored.  This SccpAlarmNotification is
      * for purely informational purposes only.
      *
      * <li>{@link OamAlarmNotification#PRIORITY_LOW PRIORITY_LOW}
      * - Indicates that the state of the MBean that emitted this
      * SccpAlarmNotification has changed as a result of an operation
      * explicitly invoked by an application.
      *
      * <li>{@link OamAlarmNotification#PRIORITY_HIGH PRIORITY_HIGH}
      * - Indicates that the physical network element associated with
      * the MBean that emitted this SccpAlarmNotification has gone out
      * of service.  Without corrective action, service reliability can
      * be severely affected.
      *
      * <li>{@link OamAlarmNotification#PRIORITY_CRITICAL PRIORITY_CRITICAL}
      * - Indicates the complete failure of the physical netwrok element
      * associated with the MBean that emitted this
      * SccpAlarmNotification.  Immediate recovery action is required.
      * </ol>
      *
      * @param alarmType
      * The type of this alarm. This may be any one of the Alarm Type constants defined in
      * this class, or one of those inherited from OamAlarmNotification.
      *
      * @exception IllegalArgumentException
      * If any of the supplied parameters are invalid
      */
    public SccpAlarmNotification(OamManagedObjectMBean source, long sequenceNumber, int priority, int alarmType)
        throws IllegalArgumentException {
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
