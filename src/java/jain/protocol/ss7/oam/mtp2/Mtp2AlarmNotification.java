/*
 @(#) src/java/jain/protocol/ss7/oam/mtp2/Mtp2AlarmNotification.java <p>
 
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

/**
  * A Mtp2AlarmNotification is an Notification emitted by a Mtp2 Managed
  * MBean(MBean) to indicate a change in status of the MBean. This state
  * change may result from: <ul>
  *
  * <li>a create/delete/modify operation by an application that
  * expliciltly changes the state of the MBean.
  *
  * <li>a change in the SS7 Network that implicitly changes the state of
  * it's associated MBean.
  *
  * <li>a timer within a Timer MBean expiring.  </ul>
  *
  * This Mtp2AlarmNotification should be sent to all applications that
  * have registered with the Mtp2 MBean as a
  * javax.management.NotificationListener using a filter 'f', where
  * f.isNotificationEnabled(this.getType()) == true. <p>
  *
  * The Notification class extends the java.util.EventObject base class
  * and defines the minimal information contained in a notification. It
  * contains the following fields: <ul>
  *
  * <li> the notification type, which is a string expressed in a dot
  * notation similar to Java properties.
  *
  * <li> a sequence number, which is a serial number identifying a
  * particular instance of notification in the context of the
  * notification source
  *
  * <li> a time stamp, indicating when the notification was generated
  *
  * <li> a message contained in a string, which could be the explanation
  * of the notification for displaying to a user
  *
  * <li> userData is used for whatever other data the notification
  * source wishes to communicate to its consumers </ul>
  *
  * Notification sources should use the notification type to indicate
  * the nature of the event to their consumers. When additional
  * information needs to be transmitted to consumers, the source may
  * place it in the message or user data fields.
  *
  * <center> [NotificationHierarchy.jpg] </center> <br>
  * <center> Inheritance hierarchy for JAIN OAM Notification </center>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class Mtp2AlarmNotification extends OamAlarmNotification {
    /**
      * Alarm Type Constant: Indicates a signalling link failure due to
      * an abnormal FIBR or BSNR of a signalling link.
      * <br>Reference: (ITU Q.703, 5.3)
      * <br>Emitted By: Mtp2Saps
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.alarm.sl_abnormal_fibr_bsnr".
      */
    public static final int MTP2_ALARM_SL_ABNORMAL_FIBR_BSNR = 1;
    /**
      * Alarm Type Constant: Indicates a signalling link failure due to
      * an excessive delay of ack of a signalling link.
      * <br>Reference: (ITU Q.703, 5.3.1)
      * <br>Emitted By: Mtp2Saps
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.alarm.sl_excessive_delay_ack".
      */
    public static final int MTP2_ALARM_SL_EXCESSIVE_DELAY_ACK = 2;
    /**
      * Alarm Type Constant: Indicates the start of remote processor outage.
      * <br>Reference: (ITU Q.704, 3.2.6)
      * <br>Emitted By: Mtp2Saps
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.alarm.sl_start_remote_processor_outage".
      */
    public static final int MTP2_ALARM_SL_START_REMOTE_PROCESSOR_OUTAGE = 3;
    /**
      * Alarm Type Constant: Link entered congestion due to management
      * initiated or unknown reason.
      * <br>Emitted By: Mtp2Saps
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.alarm.sl_congestion_start".
      */
    public static final int MTP2_ALARM_SL_CONGESTION_START = 4;
    /**
      * Alarm Type Constant: Link leaving congestion due to management
      * initiated or unknown reason.
      * <br>Emitted By: Mtp2Saps
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.alarm.sl_congestion_stop".
      */
    public static final int MTP2_ALARM_SL_CONGESTION_STOP = 5;
    /**
      * Alarm Type Constant: Link is up at physical level.
      * <br>Emitted By: Mtp2Saps
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.alarm.sl_up_physical"
      */
    public static final int MTP2_ALARM_SL_UP_PHYSICAL = 6;
    /**
      * Alarm Type Constant: Link is down at physical level.
      * <br>Emitted By: Mtp2Saps
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.alarm.sl_down_physical".
      */
    public static final int MTP2_ALARM_SL_DOWN_PHYSICAL = 7;
    /**
      * Alarm Type Constant: Link aligned.
      * <br>Emitted By: Mtp2Saps
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.alarm.sl_aligned".
      */
    public static final int MTP2_ALARM_SL_ALIGNED = 8;
    /**
      * Alarm Type Constant: Link not aligned.
      * <br>Emitted By: Mtp2Saps
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.alarm".
      */
    public static final int MTP2_ALARM_SL_NOT_ALIGNED = 9;
    /**
      * Alarm Type Constant: Retransmit queue full.
      * <br>Emitted By: Mtp2Saps
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.alarm.retransmit_q_full".
      */
    public static final int MTP2_ALARM_RETRANSMIT_Q_FULL = 10;
    /**
      * Alarm Type Constant: Retransmit queue is no longer full.
      * <br>Emitted By: Mtp2Saps
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.alarm.retransmit_q_not_full".
      */
    public static final int MTP2_ALARM_RETRANSMIT_Q_NOT_FULL = 11;
    /**
      * Alarm Type Constant: Remote end congestion started due to unknown cause.
      * <br>Emitted By: Mtp2Saps
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.alarm.remote_congestion_start".
      */
    public static final int MTP2_ALARM_REMOTE_CONGESTION_START = 12;
    /**
      * Alarm Type Constant: Remote end congestion stopped.
      * <br>Emitted By: Mtp2Saps
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.alarm.remote_congestion_stop".
      */
    public static final int MTP2_ALARM_REMOTE_CONGESTION_STOP = 13;
    /**
      * Constructs a new Mtp2AlarmNotification.
      *
      * @param source
      * The Managed MBean that emitted this Notification.
      *
      * @param sequenceNumber
      * The notification sequence number within the source object.
      *
      * @param priority
      * The priority of this Mtp2AlarmNotification.  This may be either
      * (in increasing order of priority): <ol>
      *
      * <li>{@link OamAlarmNotification#PRIORITY_INFORMATIONAL PRIORITY_INFORMATIONAL}
      *     - The cause of this Notification does not affect the
      *     functional state of the system and may be ignored.  This
      *     Mtp2AlarmNotification is for purely informational purposes
      *     only.
      *
      * <li>{@link OamAlarmNotification#PRIORITY_LOW PRIORITY_LOW}
      *     - Indicates that the state of the MBean that emitted this
      *     Mtp2AlarmNotification has changed as a result of an
      *     operation explicitly invoked by an application.
      *
      * <li>{@link OamAlarmNotification#PRIORITY_HIGH PRIORITY_HIGH}
      *     - Indicates that the physical network element associated
      *     with the MBean that emitted this Mtp2AlarmNotification has
      *     gone out of service.  Without corrective action, service
      *     reliability can be severely affected.
      *
      * <li>{@link OamAlarmNotification#PRIORITY_CRITICAL PRIORITY_CRITICAL}
      *     - Indicates the complete failure of the physical network
      *     element associated with the MBean that emitted this
      *     Mtp2AlarmNotification.  Immediate recovery action is
      *     required. </ol>
      *
      * @param alarmType
      * The type of this alarm. This may be any one of the Alarm Type
      * constants defined in this class, or one of those inherited from
      * OamAlarmNotification.
      *
      * @exception IllegalArgumentException
      * If any of the supplied parameters are invalid
      */
    public Mtp2AlarmNotification(OamManagedObjectMBean source, long sequenceNumber, int priority, int alarmType)
        throws IllegalArgumentException {
    }
    /**
      * Returns a string representation (with details) of classes which
      * extend this class.  Over rides standard JAVA toString method.
      */
    public java.lang.String toString() {
        return new java.lang.String("");
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
