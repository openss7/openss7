/*
 @(#) src/java/jain/protocol/ss7/oam/mtp2/Mtp2SapMBean.java <p>
 
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
import jain.*;

/**
  * This interface identifies an Mtp2 Service Access Point (SAP), an MTP2 SAP contains the
  * attributes of the physical port and link that is connected to the logical signalling
  * link. The timer values to be used with this link can be configured on a per-link
  * basis. <p>
  *
  * The following statistics can be collected for this MBean <ul>
  *
  * <li>MTP2_NUM_SIF_SIO_TRANS
  * <li>MTP2_NUM_SIF_SIO_RECIEVED
  * <li>MTP2_MSU_DISCARD_SL_CONGESTION </ul>
  *
  * The following Alarms can be emitted by this MBean <ul>
  *
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED
  * <li>MTP2_ALARM_SL_ABNORMAL_FIBR_BSNR
  * <li>MTP2_ALARM_SL_EXCESSIVE_DELAY_ACK
  * <li>MTP2_ALARM_SL_START_REMOTE_PROCESSOR_OUTAGE
  * <li>MTP2_ALARM_SL_CONGESTION_START
  * <li>MTP2_ALARM_SL_CONGESTION_STOP
  * <li>MTP2_ALARM_SL_UP_PHYSICAL
  * <li>MTP2_ALARM_SL_DOWN_PHYSICAL
  * <li>MTP2_ALARM_SL_ALIGNED
  * <li>MTP2_ALARM_SL_NOT_ALIGNED
  * <li>MTP2_ALARM_RETRANSMIT_Q_FULL
  * <li>MTP2_ALARM_RETRANSMIT_Q_NOT_FULL
  * <li>MTP2_ALARM_REMOTE_CONGESTION_START </ul>
  *
  * The following Errors can be emitted by this MBean <ul>
  *
  * <li>NON_OAM_RELATED_ERROR </ul>
  *
  * <center> [Mtp2MBeans.jpg] </center><br>
  * <center> Relationship between the MTP2 JAIN OAM MBean </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface Mtp2SapMBean extends Mtp2ManagedObjectMBean {
    /** A port clocking type constant: Indicates that the port connects the transmit clock
      * to the applicable line interface circuit.  */
    public static final int PORT_TYPE_DTE = 0;
    /** A port clocking type constant: Indicates that the port connects the transmit clock
      * to an internal baud rate generator and provides this clock to the applicable line
      * interface circuit.  */
    public static final int PORT_TYPE_DCE = 1;
    /**
      * Returns the ObjectName of the MTP2 Timer Profile MBean containing the MTP2 Timer
      * values to be used with this link.
      *
      * @return
      * The ObjectName of the Mtp2TimerProfileMBean.
      */
    public ObjectName getMtp2TimerProfile();
    /**
      * Sets the MTP2 Timer Profile MBean (specified by its ObjectName) containing the
      * MTP2 Timer values to be used with this link.
      *
      * @param timerProfile
      * The MTP2 Timer Profile.
      */
    public void setMtp2TimerProfile(ObjectName timerProfile);
    /**
      * Sets the clocking type of this port (DTE or DCE).
      *
      * @param portType
      * Either: <ul>
      *
      * <li>PORT_TYPE_DTE - connects the transmit clock to the applicable line interface
      * circuit
      *
      * <li>PORT_TYPE_DCE - connects the transmit clock to an internal baud rate generator
      * and provides this clock to the applicable line interface circuit. </ul>
      *
      * @exception IllegalArgumentException
      * If the supplied port type is not one defined port types.
      */
    public void setPortType(Integer portType)
        throws IllegalArgumentException;
    /**
      * Returns the clocking type of this port (DTE or DCE).
      *
      * @return
      * Either: <ul>
      *
      * <li>PORT_TYPE_DTE - connects the transmit clock to the applicable line interface
      * circuit
      *
      * <li>PORT_TYPE_DCE - connects the transmit clock to an internal baud rate generator
      * and provides this clock to the applicable line interface circuit. </ul>
      */
    public Integer getPortType();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
