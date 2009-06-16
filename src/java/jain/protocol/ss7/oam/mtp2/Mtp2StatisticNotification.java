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

package jain.protocol.ss7.oam.mtp2;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * An Mtp2StatisticNotification is a Notification emitted by an Mtp2ManagedObjectMBean
  * containing the value of a particular statistic. <p>
  *
  * This Notification should be sent to all applications that have registered with the
  * Mtp2 MBean as a javax.management.NotificationListener using a filter 'f', where
  * f.isNotificationEnabled(this.getType()) == true. <p>
  *
  * The Notification class extends the java.util.EventObject base class and defines the
  * minimal information contained in a notification. It contains the following fields: <ul>
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
  * <center> Inheritance hierarchy for JAIN OAM Notification </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public class Mtp2StatisticNotification extends OamStatisticNotification {
    /**
      * A MTP2 Statistic Type constant: Number of SIF and SIO octets transmitted.
      * <br>Measuring: Signalling Link Utilization
      * <br>Collected For: Mtp2SapMBean ITU Q.752 Measurement Number: 3.1
      * <br>Units: Octets / Mtp2SapMBean
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.statistic.num_sif_sio_trans".
      */
    public static final int MTP2_NUM_SIF_SIO_TRANS = 1;
    /**
      * A MTP2 Statistic Type constant: Number of SIF and SIO octets received.
      * <br>Measuring: Signalling Link Utilization
      * <br>Collected For: Mtp2SapMBean ITU Q.752 Measurement Number: 3.4
      * <br>Units: Octets / Mtp2SapMBean
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.statistic.num_sif_sio_received".
      */
    public static final int MTP2_NUM_SIF_SIO_RECIEVED = 2;
    /**
      * A MTP2 Statistic Type constant: MSUs discarded due to SL congestion.
      * <br>Measuring: Signalling Link Utilization
      * <br>Collected For: Mtp2SapMBean ITU Q.752 Measurement Number: 3.10
      * <br>Units: MSUs / Mtp2SapMBean
      * <br>Notification Type: "jain.protocol.ss7.oam.mtp2.statistic.msu_discard_sl_congestion".
      */
    public static final int MTP2_MSU_DISCARD_SL_CONGESTION = 3;
    /**
      * Constructs a new Mtp2StatisticNotification.
      *
      * @param source
      * The Managed MBean that emitted this Notification.
      *
      * @param sequenceNumber
      * The notification sequence number within the source object.
      *
      * @param category
      * The priority of this Mtp2StatisticEvent. This may be any of the inherited
      * types: <ul>
      *
      * <li>CATEGORY_FAULT
      * <li>CATEGORY_CONFIGURATION
      * <li>CATEGORY_PERFORMANCE </ul>
      *
      * @param statisticType
      * The type of this statistic. This may be any one of the Statistic Type constants defined in this class.
      *
      * @param statisticValue
      * The statistic value information gathered.
      *
      * @exception IllegalArgumentException
      * If any of the supplied parameters are invalid
      */
    public Mtp2StatisticNotification(OamManagedObjectMBean source, long sequenceNumber,
            int category, int statisticType, int statisticValue)
        throws IllegalArgumentException {
    }
    /**
      * Returns a string representation (with details) of classes which extend this class.
      * Over rides standard JAVA toString method.
      */
    public java.lang.String toString() {
        return new String("");
    }
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
