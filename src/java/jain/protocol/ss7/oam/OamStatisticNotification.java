/*
 @(#) $RCSfile: OamStatisticNotification.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:25 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:25 $ by $Author: brian $
 */

package jain.protocol.ss7.oam;

import javax.management.*;

import jain.protocol.ss7.*;
import jain.*;

/**
  * An OamStatisticNotification is a Notification emitted by a JAIN OAM Managed
  * Bean(MBean) to indicate a statistic measurement of a specified MBean. <p>
  *
  * The Notification class extends the java.util.EventObject base class and defines the
  * minimal information contained in a notification. It contains the following fields:
  * <ul>
  *
  * <li> the notification type, which is a string expressed in a dot notation similar to
  * Java properties.
  *
  * <li> a sequence number, which is a serial number identifying a particular instance of
  * notification in the context of the notification source
  *
  * <li> a time stamp, indicating when the notification was generated
  *
  * <li> a message contained in a string, which could be the explanation of the
  * notification for displaying to a user
  *
  * <li> userData is used for whatever other data the notification source wishes to
  * communicate to its consumers </ul>
  *
  * Notification sources should use the notification type to indicate the nature of the
  * event to their consumers. When additional information needs to be transmitted to
  * consumers, the source may place it in the message or user data fields. <p>
  *
  * The purpose of management is to provide a service, and this can be classified as
  * initial provisioning, maintaining existing service, and expansion or contraction of
  * the service. OSI defines the categories of fault management, configuration management,
  * performance management, accounting management and security management. Of these, the
  * first three categories will be supported by this release of JAIN OAM API
  * Specification. <p>
  *
  * <center> [NotificationHierarchy.jpg] </center><br>
  * <center> Inheritance hierarchy for JAIN OAM Notification </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract class OamStatisticNotification extends javax.management.Notification implements java.io.Serializable, java.lang.Cloneable {
    protected OamStatisticNotification() {
        super("",null,0);
    }
    /**
      * A Statistic Category constant: Fault management encompasses fault detection,
      * location, isolation and the correction of abnormal operation of the SS7 network.
      * Correction of faults can in some instances require fault diagnosis. Faults can
      * cause the network to fail to meet operational objectives (e.g. visible faults
      * might reduce the network's traffic capacity, latent faults would reduce the
      * network's reliability).
      */
    public static final int CATEGORY_FAULT = 1;
    /**
      * A Statistic Category constant: Configuration management controls the resources of,
      * and collects and provides data for, the signaling network and its components. This
      * facilitates the preparation for, and initialization of, signaling services, and
      * allows such services to be started, continued, and stopped.
      */
    public static final int CATEGORY_CONFIGURATION = 2;
    /**
      * A Statistic Category constant: This enables the behavior of network resources and
      * the effectiveness of communication activities in the network to be evaluated.  
      */
    public static final int CATEGORY_PERFORMANCE = 3;
    /**
      * Sets the category of this OamStatisticNotification.
      *
      * @param category
      * one of the following: <ul>
      * <li>CATEGORY_FAULT
      * <li>CATEGORY_CONFIGURATION
      * <li>CATEGORY_PERFORMANCE </ul>
      *
      * @exception IllegalArgumentException
      * if the suplied category is not one of the defined types
      */
    public void setCategory(int category) throws IllegalArgumentException {
        switch (category) {
            case CATEGORY_FAULT:
            case CATEGORY_CONFIGURATION:
            case CATEGORY_PERFORMANCE:
                m_category = category;
                break;
            default:
                throw new IllegalArgumentException();
        }
    }
    /**
      * Returns the category of this OamStatisticNotification.
      *
      * @return
      * one of the following: <ul>
      * <li>CATEGORY_FAULT
      * <li>CATEGORY_CONFIGURATION
      * <li>CATEGORY_PERFORMANCE </ul>
      */
    public int getCategory() {
        return m_category;
    }
    /**
      * Sets the value of the statistic being sent in this Statistic Notification.
      *
      * @param statisticValue
      * the statistic value
      */
    public void setStatisticValue(int statisticValue) {
        m_statisticValue = statisticValue;
    }
    /**
      * Returns the value of the statistic being sent in this Statistic Notification.
      *
      * @return
      * the statistic value
      */
    public int getStatisticValue() {
        return m_statisticValue;
    }
    /**
      * Returns the Type of this Statistic
      *
      * @return
      * one of the statistic type values defined in any of the defined statistic types.
      */
    public int getStatisticType() {
        return m_statisticType;
    }

    protected int m_category;
    protected int m_statisticValue;
    protected int m_statisticType;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
