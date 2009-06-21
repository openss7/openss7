/*
 @(#) $RCSfile: SignallingPointMBean.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:27 $ <p>
 
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

package jain.protocol.ss7.oam.mtp3;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface identifies an Signalling Point. The signalling point can represent
  * either a local or remote signalling point. In the case of a local signalling point,
  * this interface is extended by the Signalling Point MBean interface. The remote
  * signalling point can be either an adjacent or a far-end node. <p>
  *
  * The following statistics can be collected for this MBean <ul>
  * <li>MTP3_UNAVAILABILITY_ROUTE_SET
  * <li>MTP3_DUR_UNAVAILABILITY
  * <li>MTP3_ADJ_SP_INACCESSIBLE
  * <li>MTP3_DUR_ADJ_SP_INACCESSIBLE
  * <li>MTP3_MSU_DISCARD_ROUTE_DATA_ERR </ul>
  *
  * The following Alarms can be emitted by this MBean <ul>
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED
  * <li>MTP3_ALARM_SP_INACCESSIBLE
  * <li>MTP3_ALARM_SP_ACCESSIBLE
  * <li>MTP3_ALARM_CONCERNED_SP_PAUSE
  * <li>MTP3_ALARM_CONCERNED_SP_RESUME
  * <li>MTP3_ALARM_CONCERNED_SP_NETWORK_CONGESTED
  * <li>MTP3_ALARM_CONCERNED_SP_USER_UNAVAILABLE
  * <li>MTP3_ALARM_CONCERNED_SP_STOP_NET_CONG </ul>
  *
  * The following Errors can be emitted by this MBean <ul>
  * <li>NON_OAM_RELATED_ERROR </ul>
  *
  * <center> [Mtp3MBeans.jpg] </center><br>
  * <center> Relationship between the MTP3 JAIN OAM MBeans </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface SignallingPointMBean extends Mtp3ManagedObjectMBean {
    /** A Signalling Type constant: Indicates a Signalling End Point.  */
    public static final int TYPE_SEP = 1;
    /** A Signalling Type constant: Indicates a Signalling Transfer Point.  */
    public static final int TYPE_STP = 2;
    /** A Signalling Type constant: Indicates a Signalling Transfer End Point.  */
    public static final int TYPE_STEP = 3;
    /** A Network indicator constant.  */
    public static final int NI_INTERNATIONAL_00 = 0;
    /** A Network indicator constant.  */
    public static final int NI_INTERNATIONAL_01 = 1;
    /** A Network indicator constant.  */
    public static final int NI_NATIONAL_10 = 2;
    /** A Network indicator constant.  */
    public static final int NI_NATIONAL_11 = 3;
    /**
      * Sets the Signalling Point Code of this Signalling Point. The signalling point code
      * is a 9 digit number that takes the format: <p>
      *
      * SSSAAAZZZ <p>
      *
      * where: <p>
      *
      * <br>SSS == Signalling Point ID (member)
      * <br>AAA == Area Id (cluster)
      * <br>ZZZ == Zone Id (network) <p>
      *
      * The permitted ranges for each value are outlined in the table below.
      *
      * <table><tr>
      * <th></th>
      * <th>ANSI</th>
      * <th>ITU</th>
      * </tr><tr>
      * <th>Signaling Point Id<br>(member)</th>
      * <td>000-255</td>
      * <td>000-7</td>
      * </tr><tr>
      * <th>Area ID<br>(cluster)</th>
      * <td>000-255</td>
      * <td>000-255</td>
      * </tr><tr>
      * <th>Zone Id<br>(network)</th>
      * <td>000-255</td>
      * <td>000-7</td>
      * </tr></table>
      *
      * @param pointCode
      * the signalling point code 
      *
      * @exception IllegalArgumentException
      * thrown if the supplied point code is not in the correct format
      */
    public void setSignallingPointCode(Long pointCode)
        throws IllegalArgumentException;
    /**
      * Gets the Signalling Point Code of this Signalling Point. The format of the
      * signalling point code is explained here.
      *
      * @return
      * the Signalling Point Code
      */
    public Long getSignallingPointCode();
    /**
      * Sets the Network Indicator parameter of this Signalling Point (one of the Network
      * Indicator constants).
      *
      * @param networkInd
      * one of the following: <ul>
      * <li>NI_INTERNATIONAL_00
      * <li>NI_INTERNATIONAL_01
      * <li>NI_NATIONAL_10 
      * <li>NI_NATIONAL_11  </ul>
      *
      * @exception IllegalArgumentException
      * if the supplied network indicator is not one of the defined types
      */
    public void setNetworkIndicator(Integer networkInd)
        throws IllegalArgumentException;
    /**
      * Gets the Network Indicator of this Signalling Point (one of the Network Indicator
      * constants).
      *
      * @return
      * one of the following: <ul>
      * <li>NI_INTERNATIONAL_00
      * <li>NI_INTERNATIONAL_01
      * <li>NI_NATIONAL_10 
      * <li>NI_NATIONAL_11  </ul>
      */
    public Integer getNetworkIndicator();
    /**
      * Sets the type of this Signalling Point (one of the Signalling Type constants).
      *
      * @param type
      * one of the following: <ul>
      * <li>TYPE_SEP - a Signalling End Point.
      * <li>TYPE_STP - a Signalling Transfer Point.
      * <li>TYPE_STEP - a Signalling Transfer End Point. </ul>
      *
      * @exception IllegalArgumentException
      * if the supplied type is not one of the defined Signallling Type constants
      */
    public void setType(Integer type)
        throws IllegalArgumentException;
    /**
      * Returns the type of this Signalling point (one of the Signalling Type constants).
      *
      * @return
      * one of the following: <ul>
      * <li>TYPE_SEP - a Signalling End Point. This type of Signalling Point contains MTP
      * and ISUP (and/or other MTP Users), but cannot act as an intermediate MTP transfer
      * node in the SS7 network. (It can act as an SCCP Relay Node.)
      * <li>TYPE_STP - a Signalling Transfer Point. This type of Signalling Point acts as
      * an intermediate node between SEPs to transfer messages through the SS7 network, it
      * only contains MTP1
      * <li>TYPE_STEP - a Signalling Transfer End Point. This the type of signalling Point
      * combines the STP and SEP types, thus acting both as a SEP and as a STP. </ul>
      */
    public Integer getType();
    /**
      * Returns Mtp3Saps using this signalling point
      *
      * @return array of Mtp3Sap ObjectNames
      */
    public ObjectName[] getMtp3Saps();
    /**
      * Returns an Mtp3Sap using this signalling point
      * @param index
      * index of mtp3Sap
      * @return Mtp3Sap ObjectName
      */
    public ObjectName getMtp3Sap(Integer index);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
