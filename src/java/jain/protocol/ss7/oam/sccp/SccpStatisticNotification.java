/*
 @(#) src/java/jain/protocol/ss7/oam/sccp/SccpStatisticNotification.java <p>
 
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

package jain.protocol.ss7.oam.sccp;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * An SccpStatisticNotification is a Notification emitted by an SccpManagedObjectMBean
  * containing the value of a particular statistic. <p>
  *
  * This SccpStatisticNotification should be sent to all applications that have registered
  * with the Sccp MBean as a javax.management.NotificationListener using a filter 'f',
  * where f.isNotificationEnabled(this.getType()) == true. <p>
  *
  * The Notification class extends the java.util.EventObject base class and defines the
  * minimal information contained in a notification. It contains the following fields: <p>
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
  * to communicate to its consumers </ul>
  *
  * Notification sources should use the notification type to indicate the nature of the
  * event to their consumers. When additional information needs to be transmitted to
  * consumers, the source may place it in the message or user data fields.
  *
  * <center> [NotificationHierarchy.jpg] </center><br>
  * <center> Inheritance hierarchy for JAIN OAM Notification </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class SccpStatisticNotification extends OamStatisticNotification {
    /**
      * A SCCP Statistic Type constant: Routing Failure - No translation for address of
      * such nature. <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.1
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.routing_fail_no_trans_for_add".
      */
    public static final int SCCP_ROUTING_FAIL_NO_TRANS_FOR_ADD = 1;
    /**
      * A SCCP Statistic Type constant: Routing Failure - No translation for this specific
      * address. <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.2
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.routing_fail_no_trans_for_this_spec_add".
      */
    public static final int SCCP_ROUTING_FAIL_NO_TRANS_FOR_THIS_SPEC_ADD = 2;
    /**
      * A SCCP Statistic Type constant: Routing Failure - Network Failure (Point Code not
      * available). <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.3
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.routing_fail_network_failure".
      */
    public static final int SCCP_ROUTING_FAIL_NETWORK_FAILURE = 3;
    /**
      * A SCCP Statistic Type constant: Routing Failure - Network Congestion. <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.4
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.routing_fail_network_congestion".
      */
    public static final int SCCP_ROUTING_FAIL_NETWORK_CONGESTION = 4;
    /**
      * A SCCP Statistic Type constant: Routing Failure - Subsystem Failure (unavailable).
      * <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.5
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.routing_fail_subsystem_failure".
      */
    public static final int SCCP_ROUTING_FAIL_SUBSYSTEM_FAIL = 5;
    /**
      * A SCCP Statistic Type constant: Routing Failure - Subsystem Congestion. <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.6
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.routing_fail_subsystem_congestion".
      */
    public static final int SCCP_ROUTING_FAIL_SUBSYSTEM_CONGESTION = 6;
    /**
      * A SCCP Statistic Type constant: Routing Failure - Unequipped user (Subsystem). <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.7
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.routing_fail_unequipped_user".
      */
    public static final int SCCP_ROUTING_FAIL_UNEQUIPPED_USER = 7;
    /**
      * A SCCP Statistic Type constant: Syntax Error Detected. <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.8
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.syntax_error_detected".
      */
    public static final int SCCP_SYNTAX_ERROR_DETECTED = 8;
    /**
      * A SCCP Statistic Type constant: Routing Failure - Unqualified. <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.9
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.routing_fail_unqualified".
      */
    public static final int SCCP_ROUTING_FAIL_UNQUALIFIED = 9;
    /**
      * A SCCP Statistic Type constant: Reassembly error - Timer The timer returned by
      * getSccpTimerReassembly() has expired. <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.10
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.reassembly_err_time_t_reass_expiry".
      */
    public static final int SCCP_REASSEMBLY_ERR_TIME_T_REASS_EXPIRY = 10;
    /**
      * A SCCP Statistic Type constant: Reassembly error - Segment received out of
      * sequence (including duplicates, recpt, of non-first segment for which no
      * reassembly process). <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 711
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.reassembly_err_seg_received_out_of_seq".
      */
    public static final int SCCP_REASSEMBLY_ERR_SEG_RECEIVED_OUT_OF_SEQ = 11;
    /**
      * A SCCP Statistic Type constant: Reassembly error - No reassembly space. <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.12
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.reassembly_error_no_reassembly_space".
      */
    public static final int SCCP_REASSEMBLY_ERROR_NO_REASSEMBLY_SPACE = 12;
    /**
      * A SCCP Statistic Type constant: Hop counter violation (XUDT, XUDTS, LUDT, LUDTS
      * and CR). <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.13
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.hop_counter_violation".
      */
    public static final int SCCP_HOP_COUNTER_VIOLATION = 13;
    /**
      * A SCCP Statistic Type constant: Message too large for segmentation. <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.14
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.msg_too_large_for_seg".
      */
    public static final int SCCP_MSG_TOO_LARGE_FOR_SEG = 14;
    /**
      * A SCCP Statistic Type constant: Failure of release complete supervision. <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.15
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.fail_of_release_complete_supervision".
      */
    public static final int SCCP_FAIL_OF_RELEASE_COMPLETE_SUPERVISION = 15;
    /**
      * A SCCP Statistic Type constant: Timer The timer returned by getSccpTimerIAR() has
      * expired. <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.16
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.timer_t_iar_expiry".
      */
    public static final int SCCP_TIMER_T_IAR_EXPIRY = 16;
    /**
      * A SCCP Statistic Type constant: Provider initiated reset of a connection. <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.17
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.provider_ini_resetof_connection".
      */
    public static final int SCCP_PROVIDER_INI_RESET_OF_A_CONNECTION = 17;
    /**
      * A SCCP Statistic Type constant: Provider initiated release of a connection. <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.18
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.provider_ini_release_of_connection".
      */
    public static final int SCCP_PROVIDER_INI_RELEASE_OF_CONNECTION = 18;
    /**
      * A SCCP Statistic Type constant: Segmentation error - Segmentation failed. <p>
      * <br> Measuring: SCCP error performce
      * <br> Collected For: SccpRoutingControlMBean ITU Q.752 Measurement Number: 7.20
      * <br> Units: Notifications / SccpRoutingControlMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.seg_err_seg_fail".
      */
    public static final int SCCP_SEG_ERR_SEG_FAIL = 19;
    /**
      * A SCCP Statistic Type constant: Total (L)(X)UDT messages originated per class and
      * source SSN. <p>
      * <br> Measuring: SCCP Utilization
      * <br> Collected For: SccpSapMBean ITU Q.752 Measurement Number: 9.6
      * <br> Units: Messages / class / SccpSapMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.udt_orig_class_ssn".
      */
    public static final int SCCP_UDT_ORIG_CLASS_SSN = 20;
    /**
      * A SCCP Statistic Type constant: Total (L)(X)UDT messages terminated per class and
      * sink SSN. <p>
      * <br> Measuring: SCCP Utilization
      * <br> Collected For: SccpSapMBean ITU Q.752 Measurement Number: 9.7
      * <br> Units: Messages / class / SccpSapMBean
      * <br> Notification Type: "jain.protocol.ss7.oam.sccp.statictic.udt_term_class_ssn".
      */
    public static final int SCCP_UDT_TERM_CLASS_SSN = 21;
    /**
      * Constructs a new SccpStatisticNotification.
      *
      * @param source
      * The Sccp Layer Manager that emitted this Notification.
      *
      * @param sequenceNumber
      * The notification sequence number within the source object.
      *
      * @param category
      * The category of this SccpStatisticNotification. This may be any of the inherited
      * types: <ul>
      * <li>CATEGORY_FAULT
      * <li>CATEGORY_CONFIGURATION
      * <li>CATEGORY_PERFORMANCE </ul>
      *
      * @param statisticType
      * The type of this statistic. This may be any one of the Statistic Type constants defined in this class.
      *
      * @param statisticValue
      * The statistic value
      *
      * @exception IllegalArgumentException
      * If any of the supplied parameters are invalid
      */
    public SccpStatisticNotification(OamManagedObjectMBean source, long sequenceNumber, int category, int statisticType, int statisticValue) throws IllegalArgumentException { }
    /**
      * Returns a string representation (with details) of classes which extend this class.
      * Over rides standard JAVA toString method.
      */
    public java.lang.String toString() {
        return new java.lang.String("");
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
