/* ***************************************************************************

 @(#) $RCSfile$ $Name$($Revision$) $Date$

 -----------------------------------------------------------------------------

 Copyright (c) 2008-2009  Monavacon Limited <http://www.monavacon.com/>
 Copyright (c) 2001-2008  OpenSS7 Corporation <http://www.openss7.com/>
 Copyright (c) 1997-2001  Brian F. G. Bidulock <bidulock@openss7.org>

 All Rights Reserved.

 This program is free software: you can redistribute it and/or modify it under
 the terms of the GNU Affero General Public License as published by the Free
 Software Foundation, version 3 of the license.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for more
 details.

 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>, or
 write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA
 02139, USA.

 -----------------------------------------------------------------------------

 U.S. GOVERNMENT RESTRICTED RIGHTS.  If you are licensing this Software on
 behalf of the U.S. Government ("Government"), the following provisions apply
 to you.  If the Software is supplied by the Department of Defense ("DoD"), it
 is classified as "Commercial Computer Software" under paragraph 252.227-7014
 of the DoD Supplement to the Federal Acquisition Regulations ("DFARS") (or any
 successor regulations) and the Government is acquiring only the license rights
 granted herein (the license rights customarily provided to non-Government
 users).  If the Software is supplied to any unit or agency of the Government
 other than DoD, it is classified as "Restricted Computer Software" and the
 Government's rights in the Software are defined in paragraph 52.227-19 of the
 Federal Acquisition Regulations ("FAR") (or any successor regulations) or, in
 the cases of NASA, in paragraph 18.52.227-86 of the NASA Supplement to the FAR
 (or any successor regulations).

 -----------------------------------------------------------------------------

 Commercial licensing and support of this software is available from OpenSS7
 Corporation at a fee.  See http://www.openss7.com/

 -----------------------------------------------------------------------------

 Last Modified $Date$ by $Author$

 -----------------------------------------------------------------------------

 $Log$
 *****************************************************************************/

package jain.protocol.ss7.oam;

import java.lang.Object;;
import java.util.EventObject;
import javax.management.Notification;
import jain.protocol.ss7.oam.OamAlarmNotification;

/**
  * An OamAlarmNotification is a Notification emitted by a JAIN OAM Layer Manager MBean to indicate a change in status
  * of the MBean.  This state change may result from: <p>
  * <li>a create/delete/modify operation by an application that expliciltly changes the state of the MBean.
  * <li>a change in the SS7 Network that implicitly changes the state of it's associated MBean.
  * <li>a timer within a Timer MBean expiring. <p>
  *
  * This Notification should be sent to all applications that have registered with the JAIN OAM Layer Manager MBean as
  * an Oam Notification Listener. <p>
  *
  * The Oam Notifications are to be categorised according to the following priorites (in increasing order of
  * priority): <p>
  *
  * <ol>
  * <li><tt>PRIORITY_INFORMATIONAL</tt> - The cause of this OamAlarmNotification does not affect the functional state
  * of the system and may be ignored.  This OamAlarmNotification is for purely informational purposes only.
  * <li><tt>PRIORITY_LOW</tt> - Indicates that the state of the MBean that emitted this OamAlarmNotification has
  * changed as a result of an operation explicitly invoked by an application.
  * <li><tt>PRIORITY_HIGH</tt> - Indicates that the physical network element associated with the MBean that emitted
  * this OamAlarmNotification has gone out of service.  Without corrective action, service reliability can be severely
  * affected.
  * <li><tt>PRIORITY_CRITICAL</tt> Indicates the complete failure of the physical network element associated with the
  * MBean that emitted this AlarmNotification.  Immediate recovery action is required.
  * <li><tt>PRIORITY_VENDOR_DEFINED</tt> - Indicates that this alarm has a severity that is defined by the SS7 stack
  * vendor. An alarm of this priority may have either none or very severe ramifications and it is the responsibilty of
  * a Stack vendor to inform JAIN OAM application developers the severity of this alarm.
  * </ol> <p>
  *
  * The Notification class extends the java.util.EventObject base class and defines the minimal information contained
  * in a notification. It contains the following fields: <p>
  *
  * <li><i>notification type</i>, which is a string expressed in a dot notation similar to Java properties.
  * <li><i>sequence number</i>, which is a serial number identifying a particular instance of notification in the
  * context of the notification source
  * <li><i>time stamp</i>, indicating when the notification was generated
  * <li><i>message</i> contained in a string, which could be the explanation of the notification for displaying to a
  * user
  * <li><i>userData</i> is used for whatever other data the notification source wishes to communicate to its consumers
  * <p>
  *
  * Notification sources should use the notification type to indicate the nature of the event to their consumers. When
  * additional information needs to be transmitted to consumers, the source may place it in the message or user data
  * fields. <p>
  *
  * <em>IMPORTANT:</em> This API does not define the action required upon receiving an alarm of a particular priority,
  * nor does the API define what priority is associated with each alarm type.  The assignment of an alarm prioritiy
  * (or range of alarm priorities) to each alarm type is implementation dependant,and it the responsibility of the
  * Stack Vendor to make the implementation dependent assignments known.
  */
public abstract class OamAlarmNotification extends Notification implements java.io.Serializable, java.lang.Cloneable {
    /** Indicates that this is an Informational Alarm. The cause of this OamAlarmNotification does not affect the
      * functional state of the system and may be ignored. This OamAlarmNotification is for purely informational
      * purposes only.  */
    public static final int PRIORITY_INFORMATIONAL = 0;
    /** Indicates that this is a Low Priority Alarm. The state of the MBean that emitted this OamAlarmNotification has
      * changed as a result of an operation explicitly invoked by an application. */
    public static final int PRIORITY_LOW = 1;
    /** Indicates that this is a High Priority Alarm. The physical network element associated with the MBean that
      * emitted this OamAlarmNotification has gone out of service.  Without corrective action, service reliability can
      * be severely affected. */
    public static final int PRIORITY_HIGH = 2;
    /** Indicates that this is a Critical Alarm. An Alarm of this priority indicates the complete failure of the
      * physical netwrok element associated with the MBean that emitted this Alarm.  Immediate recovery action is
      * required. */
    public static final int PRIORITY_CRITICAL = 3;
    /** Indicates that this alarm has a severity that is defined by the SS7 stack vendor. An alarm of this priority
      * may have either none or very severe ramifications and it is the responsibilty of a Stack vendor to inform JAIN
      * OAM application developers the severity of this alarm. */
    public static final int PRIORITY_VENDOR_DEFINED = 4;

    /** Alarm Type Constant: Indicates that a timer has expired. <p>
      * <em>IMPORTANT</em> If an Alarm of this type is sent then the Timer Id of the Timer that caused this event
      * should be set so that the application receiving the Notification can determine which Timer caused the Alarm.
      * Each Timer defined within this API has an associated Timer Id specified
      * in the comment describing that Timer. <br>
      * Emitted By: All OamTimerProfilesMBean <br>
      * When: A Timer contained in the timer profile expires <br>
      * Notification Type: "jain.protocol.ss7.oam.alarm.timer_expired" */
    public static final int ALARM_TIMER_EXPIRED = 0;
    /** Alarm Type Constant: Indicates that an instance of a Managed MBean has been created. <br>
      * Reference: "ITU-T Rec. M.3100 (1992)" createDeleteNotificationPackage (I,C) <br>
      * Emitted By: All OamLayerManagerMBean <br>
      * When: The creation of the MBean has been sucessfully committed. <br>
      * Notification Type: "jain.protocol.ss7.oam.alarm.created" */
    public static final int ALARM_MO_CREATED = 1;
    /** Alarm Type Constant: Indicates that an instance of a Managed MBean has been deleted. <br>
      * Reference: "ITU-T Rec. M.3100 (1992)" createDeleteNotificationPackage (I,C) <br>
      * Emitted By: All OamLayerManageMBean <br>
      * When: The deletion of the MBean has been sucessfully committed. <br>
      * Notification Type: "jain.protocol.ss7.oam.alarm.deleted" */
    public static final int ALARM_MO_DELETED = 2;
    /** Alarm Type Constant: Indicates that an attribute value changed. <br>
      * Reference: "ITU-T Rec. M.3100 (1992)" attributeValueChangeNotificationPackage (I,C) <br>
      * Emitted By: All OamLayerManagerMBean <br>
      * When: The modification of an attribute of the MBean has been sucessfully committed. This Alarm will be fired
      * once for every attribute whose value has changed. <br>
      * Notification Type: "jain.protocol.ss7.oam.alarm.attribute_value_changed"
      */
    public static final int ALARM_MO_ATTRIBUTE_VALUE_CHANGED = 3;
    /**
      * Sets the Priority of this Alarm.
      * @param priority - one of the following (in increasing order of priority):
      * <ol>
      * <li><tt>PRIORITY_INFORMATIONAL</tt> - The cause of this OamAlarmNotification does not affect the functional
      * state of the system and may be ignored.  This OamAlarmNotification is for purely informational purposes only.
      * <li><tt>PRIORITY_LOW</tt> - Indicates that the state of the MBean that emitted this OamAlarmNotification has
      * changed as a result of an operation explicitly invoked by an application.
      * <li><tt>PRIORITY_HIGH</tt> - Indicates that the physical network element associated with the MBean that
      * emitted this OamAlarmNotification has gone out of service.  Without corrective action, service reliability can
      * be severely affected.
      * <li><tt>PRIORITY_CRITICAL</tt> - Indicates the complete failure of the physical network element associated
      * with the MBean that emitted this OamAlarmNotification.  Immediate recovery action is required.
      * <li><tt>PRIORITY_VENDOR_DEFINED</tt> - Indicates that this alarm has a severity that is defined by the SS7
      * stack vendor. An alarm of this priority may have either none or very severe ramifications and it is the
      * responsibilty of a Stack vendor to inform JAIN OAM application developers the severity of this alarm.
      * </ol>
      * @exception java.lang.IllegalArgumentException  If the supplied priority is not one of the defined types.
      */
    public void setPriority(int priority) throws java.lang.IllegalArgumentException {
    }
    /**
      * Returns the Priority of this OamAlarmNotification.
      * @return One of the following (in increasing order of priority):
      * <ol>
      * <li><tt>PRIORITY_INFORMATIONAL</tt> - The cause of this OamAlarmNotification does not affect the functional
      * state of the system and may be ignored.  This OamAlarmNotification is for purely informational purposes only.
      * <li><tt>PRIORITY_LOW</tt> - Indicates that the state of the MBean that emitted this OamAlarmNotification has
      * changed as a result of an operation explicitly invoked by an application.
      * <li><tt>PRIORITY_HIGH</tt> - Indicates that the physical network element associated with the MBean that
      * emitted this OamAlarmNotification has gone out of service.  Without corrective action, service reliability can
      * be severely affected.
      * <li><tt>PRIORITY_CRITICAL</tt> - Indicates the complete failure of the physical network element associated
      * with the MBean that emitted this OamAlarmNotification.  Immediate recovery action is required.
      * <li><tt>PRIORITY_VENDOR_DEFINED</tt> - Indicates that this alarm has a severity that is defined by the SS7
      * stack vendor. An alarm of this priority may have either none or very severe ramifications and it is the
      * responsibilty of a Stack vendor to inform JAIN OAM application developers the severity of this alarm.
      * </ol>
      */
    public int getPriority() {
    }
    /**
      * Returns the Type of this Alarm
      * @return One of the alarm type values defined in this OamAlarmNotification or any of the defined alarm types.
      */
    public int getAlarmType() {
    }
    /**
      * Returns the timestamp indicating the time at which this alarm was emitted.
      * @return The time at which the alarm was emitted.
      */
    public java.util.Date getTimestamp() {
    }
    /**
      * Returns the Timer Id of the Timer that caused the Timer Expired Alarm [when ALARM_TIMER_EXPIRED ==
      * getAlarmType()]. Each Timer defined within this API has an associated Timer Id specified in the comment
      * describing that Timer. This method returns that Timer Id associated with the particular timer that has
      * expired.  If this method is invoked when the Alarm is not a Timer Expired Alarm [when ALARM_TIMER_EXPIRED !=
      * getAlarmType()] then 0 is returned.
      * @return either
      * <li>the Timer Id of the the Timer that caused the Timer Expired Alarm
      * [when ALARM_TIMER_EXPIRED == getAlarmType()], or
      * <li>0 - [when ALARM_TIMER_EXPIRED != getAlarmType()]
      */
    public int getTimerId() {
    }
    /**
      * Sets the Timer Id that indicates the Timer that caused a Timer Expired Notification. Whenever a new
      * OamAlarmNotification is sent to indicate that a timer has expired [ALARM_TIMER_EXPIRED == getAlarmType()],
      * then this value should be set so that the application receiving the Notification can determine which Timer
      * caused the event to be sent.
      * @param timerId  The Id of the Timer that caused a Timer Expired Notification.
      * @exception java.lang.IllegalArgumentException  If the supplied timer ID does not represent a legal Timer ID.
      */
    public void setTimerId(int timerId) throws java.lang.IllegalArgumentException {
    }
}

// vim: sw=4 et tw=0 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-

