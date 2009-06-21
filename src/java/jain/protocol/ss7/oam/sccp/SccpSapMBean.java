/*
 @(#) $RCSfile: SccpSapMBean.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:27 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:27 $ by $Author: brian $
 */

package jain.protocol.ss7.oam.sccp;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines a SCCP Access Point (an SCCP Subsystem). This is the Service
  * Access Point (SAP) of the SCCP services to its users and contains the SAP address (the
  * Sub-System number). This SCCP SAP may represent either a: <ul>
  * <li>a Local Subsystem or
  * <li>a Remote Subsystem </ul>
  *
  * The following statistics can be collected for this MBean <ul>
  * <li>SCCP_UDT_ORIG_CLASS_SSN
  * <li>SCCP_UDT_TERM_CLASS_SSN </ul>
  *
  * The following Alarms can be emitted by this MBean <ul>
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED
  * <li>SCCP_ALARM_SSN_UNAVAILABLE_FAILURE
  * <li>SCCP_ALARM_SSN_UNAVAILABLE_MAINTENANCE
  * <li>SCCP_ALARM_SSN_UNAVAILABLE_CONGESTION
  * <li>SCCP_ALARM_SSN_AVAILABLE
  * <li>SCCP_ALARM_SSN_OOS_GRANT
  * <li>SCCP_ALARM_SSN_OOS_DENIED
  * <li>SCCP_ALARM_SSN_START_PROHIBITED
  * <li>SCCP_ALARM_SSN_END_PROHIBITED
  * <li>SCCP_ALARM_SSN_ALLOWED_RX
  * <li>SCCP_ALARM_QOS_TOO_LARGE
  * <li>SCCP_ALARM_QOS_SEGMENTATION_FAILURE
  * <li>SCCP_ALARM_QOS_SSN_UNAVAILABLE
  * <li>SCCP_ALARM_QOS_SSN_CONGESTED
  * <li>SCCP_ALARM_QOS_SSN_UNEQUIPED
  * <li>SCCP_ALARM_QOS_SYNTAX_ERROR </ul>
  *
  * The following Errors can be emitted by this MBean <ul>
  * <li>NON_OAM_RELATED_ERROR </ul>
  *
  * <center> [SccpMBeans.jpg] </center><br>
  * <center> Relationship between the SCCP JAIN OAM MBean </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface SccpSapMBean extends SccpManagedObjectMBean {
    /** A SCCP SAP Availability Status constant: Indicates that the status of the Access
      * Point is Available. This constant has an integer value of 1.  */
    public static final int STATUS_AVAILABLE = 1;
    /** A SCCP SAP Availability Status constant: Indicates that the status of the Access
      * Point is UnAvailable. This constant has an integer value of 2.  */
    public static final int STATUS_UNAVAILABLE = 2;
    /** A SCCP SAP Availability Status constant: Indicates that the status of the Access
      * Point is Congested. This constant has an integer value of 3.  */
    public static final int STATUS_DEGRADED = 3;
    /** A Subsystem Status constant: The local Subsystem has registered and become Allowed */
    public static final int SCCP_STATUS_ALLOWED = 4;
    /** A Subsystem Status constant: The Subsystem has registered (is equipped) but has
      * not yet become allowed, or is prohibited.  */
    public static final int SCCP_STATUS_PROHIBITED = 5;
    /** A Subsystem Status constant: The Subsystem has not registered to the SCCP layer,
      * the Subsystem has deregistered, or the Subsystem has failed.  */
    public static final int SCCP_STATUS_UNEQUIPPED = 6;
    /**
      * Sets the Subsystem Number of this SubsystemMBean.
      *
      * @exception IllegalArgumentException
      * if the supplied subsystem number represents an unacceptable value
      */
    public void setSubsystemNumber(Integer ssn);
    /** Gets the Subsystem Number of this SubsystemMBean.  */
    public Integer getSubsystemNumber();
    /**
      * Sets the MTP3 Access Point representing the Signalling Point (MTP) used by this
      * SCCP.
      *
      * @param mtp3Sap
      * the ObjectName of the MTP3 Access Point of this SCCP
      */
    public void setMtp3Sap(ObjectName mtp3Sap);
    /**
      * Returns the ObjectName of the MTP3 Access Point representing the Signalling Point
      * (MTP) used by this SCCP.
      *
      * @return
      * the ObjectName of the MTP3 Access Point of this SCCP
      */
    public ObjectName getMtp3Sap();
    /**
      * Sets the concerned area to be used by SCCP Management for broadcasting the SCCP
      * status after completion of SCCP Restart.  This concerned area will be used for
      * primary broadcast if this access point is local, and will be used for secondary
      * broadcast if this access point is remote.
      *
      * @param concernedArea
      * the ObjectName of the concerned area to be used for SCCP broadcast.
      */
    public void setConcernedArea(ObjectName concernedArea);
    /**
      * Returns the ObjectName of the concerned area to be used by SCCP Management for
      * broadcasting the SCCP status after completion of SCCP Restart. This concerned area
      * will be used for primary broadcast if this access point is local, and will be used
      * for secondary broadcast if this access point is remote.
      *
      * @return
      * the ObjectName of the concerned area to be used for SCCP broadcast.
      */
    public ObjectName getConcernedArea();
    /**
      * Returns the availability status of this SCCP (one of the SCCP SAP Status
      * constants).
      *
      * @return
      * either: <ul>
      * <li>STATUS_AVAILABLE - The Access Point is reachable and functioning normally.
      * <li>STATUS_UNAVAILABLE - The Access Point is not reachable.
      * <li>STATUS_DEGRADED - The Access Point is congested. </ul>
      */
    public Integer getSapAvailabilityStatus();
    /**
      * Indicates the status of the Local Subsystem. Each SCCP application (Subsystem)
      * registers to the SCCP Layer with a unique Subsystem number. This method indicates
      * the status of that SCCP application (Subsystem).
      *
      * @return
      * either: <ul>
      * <li>SCCP_STATUS_ALLOWED - The local Subsystem has registered and become Allowed
      *
      * <li>SCCP_STATUS_PROHIBITED - The Subsystem has registered (is equipped) but has
      * not yet become allowed, or is prohibited.
      *
      * <li>SCCP_STATUS_UNEQUIPPED - The Subsystem has not registered to the SCCP layer,
      * the Subsystem has deregistered, or the Subsystem has failed. </ul>
      */
    public Integer getSubsystemStatus();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
