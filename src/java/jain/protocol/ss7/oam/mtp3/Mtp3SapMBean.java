/*
 @(#) src/java/jain/protocol/ss7/oam/mtp3/Mtp3SapMBean.java <p>
 
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
  * This interface identifies an Mtp3 Service Access Point (SAP). <p>
  *
  * The following statistics can be collected for this MBean <p>
  *
  * The following Alarms can be emitted by this MBean <ul>
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED </ul>
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
public abstract interface Mtp3SapMBean extends Mtp3ManagedObjectMBean {
    /**
      * A SAP Status constant: Indicates that the status of the Access Point is Allowed.
      */
    public static final int STATUS_ALLOWED = 1;
    /**
      * A SAP Status constant: Indicates that the status of the Access Point is
      * Prohibited.
      */
    public static final int STATUS_PROHIBITED = 2;
    /**
      * A SAP Status constant: Indicates that the status of the Access Point is Congested.
      */
    public static final int STATUS_CONGESTED = 3;
    /**
      * Service Indicator constant: Indicates signalling network management messages.
      */
    public static final int SI_SIG_NET_MAN_MESSAGES = 1;
    /**
      * Service Indicator constant: Indicates signalling network testing and maintenance
      * messages.
      */
    public static final int SI_SIG_NET_TEST_MAIN_MESSAGES = 2;
    /**
      * Service Indicator constant: Indicates SCCP.
      */
    public static final int SI_SCCP = 3;
    /**
      * Service Indicator constant: Indicates Telephone User Part.
      */
    public static final int SI_TUP = 4;
    /**
      * Service Indicator constant: Indicates ISDN User Part
      */
    public static final int SI_ISUP = 5;
    /**
      * Service Indicator constant: Indicates Data User Part - call and circuit-related
      * messages.
      */
    public static final int SI_DUP_CALL_CIRCUIT = 6;
    /**
      * Service Indicator constant: Indicates Data User Part - facility registration and
      * cancellation messages.
      */
    public static final int SI_DUP_FACILITY_CANCELLATION = 7;
    /**
      * Service Indicator constant: Indicates reserved for MTP Testing User Part.
      */
    public static final int SI_MTP_TESTING = 8;
    /**
      * Service Indicator constant: Indicates broadband ISDN User Part.
      */
    public static final int SI_BROADBAND_ISUP = 9;
    /**
      * Service Indicator constant: Indicates satellite ISDN User Part.
      */
    public static final int SI_SATELLITE_ISUP = 10;
    /**
      * A Network Indicator constant: Indicates the International Network 00, this has an
      * integer value of 0.
      */
    public static final int NI_INTERNATIONAL_00 = 0;
    /**
      * A Network Indicator constant: Indicates the International Network 01, this has an
      * integer value of 1.
      */
    public static final int NI_INTERNATIONAL_01 = 1;
    /**
      * A Network Indicator constant: Indicates the National Network 10, this has an
      * integer value of 2.
      */
    public static final int NI_NATIONAL_10 = 2;
    /**
      * A Network Indicator constant: Indicates the National Network 11, this has an
      * integer value of 3.
      */
    public static final int NI_NATIONAL_11 = 3;
    /**
      * Sets the Signalling Point of this Service Access Point.
      *
      * @param signallingPoint
      * The signalling point.
      *
      * @exception IllegalArgumentException
      * Thrown if the supplied point code is not in the correct format.
      */
    public void setSignallingPoint(ObjectName signallingPoint)
        throws IllegalArgumentException;
    /**
      * Gets the ObjectName of the Signalling Point of this Service Access Point.
      *
      * @return
      * The Signalling Point Code.
      */
    public ObjectName getSignallingPoint();
    /**
      * Gets an array of ObjectNames of the users of this SAP.
      */
    public ObjectName[] getMtp3SapUsers();
    /**
      * Gets an ObjectName of a particular user of this SAP.
      */
    public ObjectName getMtp3SapUser(Integer index);
    /**
      * Sets the service indicator (one of the service indicator constants). This is used
      * by signalling handling functions to to perform message distribution and, in some
      * applications, to perform message routing. <p>
      *
      * @param serviceIndicator
      * one of the Service Indicator constants, one of the following: <ul>
      * <li>SI_BROADBAND_ISUP
      * <li>SI_DUP_CALL_CIRCUIT
      * <li>SI_DUP_FACILITY_CANCELLATION
      * <li>SI_ISUP
      * <li>SI_MTP_TESTING
      * <li>SI_SATELLITE_ISUP
      * <li>SI_SCCP
      * <li>SI_SIG_NET_MAN_MESSAGES
      * <li>SI_SIG_NET_TEST_MAIN_MESSAGES
      * <li>SI_TUP </ul>
      *
      * @exception IllegalArgumentException
      * If the supplied service indicator is not one of the defined types.
      */
    public void setServiceIndicator(Integer serviceIndicator)
        throws IllegalArgumentException;
    /**
      * Returns the service indicator one of the service indicator constants. This is used
      * by signalling handling functions to to perform message distribution and, in some
      * applications, to perform message routing.
      *
      * @return
      * One of the following service indicator constants: <ul>
      * <li>SI_BROADBAND_ISUP
      * <li>SI_DUP_CALL_CIRCUIT
      * <li>SI_DUP_FACILITY_CANCELLATION
      * <li>SI_ISUP
      * <li>SI_MTP_TESTING
      * <li>SI_SATELLITE_ISUP
      * <li>SI_SCCP
      * <li>SI_SIG_NET_MAN_MESSAGES
      * <li>SI_SIG_NET_TEST_MAIN_MESSAGES
      * <li>SI_TUP </ul>
      */
    public Integer getServiceIndicator();
    /**
      * Returns the status of the signalling point identified by this Service Access Point
      * (one of the SAP Status constants).
      *
      * @return
      * One of the following SAP Status constants  <ul>
      * <li>Allowed - The Access Point is reachable and functioning normally.
      * <li>Prohibited - The Access Point is not reachable. This means that the MTP access
      * point is not accessible
      * <li>Congested - The Access Point is congested. This means that the MTP access
      * point is accessible, but the path to it is heavily loaded. </ul>
      */
    public Integer getSapStatus();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
