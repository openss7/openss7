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
  * This interface defines the methods required by the SCCP Routing Control Function It
  * defines methods to maintain management information concerning the SCCP routing
  * control. SCCP routing control (SCRC): Upon receipt of a message from the MTP or from
  * the SCCP connection-oriented/connectionless control, the SCCP routing control function
  * provides the necessary routing functions to either: <ul>
  *
  * <li>forward the message to the MTP for transfer, or
  *
  * <li>pass the message the SCCP connection-oriented/connectionless control
  * function.</ul>
  *
  * One routing control shall exist per local Signalling Point, where multiple local
  * Signalling Points are allowed. Therefore each instance of this Routing Control MBean
  * shall have one Local (Own) Signalling Point associated with it. <p>
  *
  * The following statistics can be collected for this MBean <ul>
  *
  * <li>SCCP_ROUTING_FAIL_NO_TRANS_FOR_ADD
  * <li>SCCP_ROUTING_FAIL_NO_TRANS_FOR_THIS_SPEC_ADD
  * <li>SCCP_ROUTING_FAIL_NETWORK_FAILURE
  * <li>SCCP_ROUTING_FAIL_NETWORK_CONGESTION
  * <li>SCCP_ROUTING_FAIL_SUBSYSTEM_FAIL
  * <li>SCCP_ROUTING_FAIL_SUBSYSTEM_CONGESTION
  * <li>SCCP_ROUTING_FAIL_UNEQUIPPED_USER
  * <li>SCCP_SYNTAX_ERROR_DETECTED
  * <li>SCCP_ROUTING_FAIL_UNQUALIFIED
  * <li>SCCP_REASSEMBLY_ERR_TIME_T_REASS_EXPIRY
  * <li>SCCP_REASSEMBLY_ERR_SEG_RECEIVED_OUT_OF_SEQ
  * <li>SCCP_REASSEMBLY_ERROR_NO_REASSEMBLY_SPACE
  * <li>SCCP_HOP_COUNTER_VIOLATION
  * <li>SCCP_MSG_TOO_LARGE_FOR_SEG
  * <li>SCCP_FAIL_OF_RELEASE_COMPLETE_SUPERVISION
  * <li>SCCP_TIMER_T_IAR_EXPIRY
  * <li>SCCP_PROVIDER_INI_RESET_OF_A_CONNECTION
  * <li>SCCP_PROVIDER_INI_RELEASE_OF_CONNECTION
  * <li>SCCP_SEG_ERR_SEG_FAIL </ul>
  *
  * The following Alarms can be emitted by this MBean <ul>
  *
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED
  * <li>SCCP_ALARM_SSN_UNAVAILABLE_FAILURE
  * <li>SCCP_ALARM_QOS_TOO_LARGE
  * <li>SCCP_ALARM_QOS_REASSEMBLY_TIMEOUT
  * <li>SCCP_ALARM_QOS_SEGMENT_OUT_OF_ORDER
  * <li>SCCP_ALARM_QOS_NO_REASSEBLY_SPACE
  * <li>SCCP_ALARM_QOS_NO_SEGEMENTATION_SUPPORT
  * <li>SCCP_ALARM_QOS_REASSEMBLY_FAILURE
  * <li>SCCP_ALARM_QOS_NO_TRANSLATOR_FOR_ADDR
  * <li>SCCP_ALARM_QOS_NO_RULE_FOR_ADDR
  * <li>SCCP_ALARM_QOS_PC_NOT_AVAILABLE
  * <li>SCCP_ALARM_QOS_PC_CONGESTED
  * <li>SCCP_ALARM_QOS_SYNTAX_ERROR
  * <li>SCCP_ALARM_QOS_ROUTING_FAILURE
  * <li>SCCP_ALARM_QOS_HOP_COUNTER_VIOLATION </ul>
  *
  * The following Errors can be emitted by this MBean <ul>
  *
  * <li>NON_OAM_RELATED_ERROR
  * <li>SCCP_ERROR_MSG_SYNTAX
  * <li>SCCP_ERROR_MSG_INVALID
  * <li>SCCP_ERROR_MSG_PARAM_OUT_OF_RANGE
  * <li>SCCP_ERROR_MSG_DECODING
  * <li>SCCP_ERROR_MSG_INVALID_TYPE
  * <li>SCCP_ERROR_ROUTING_ERROR </ul>
  *
  * <center> [SccpMBeans.jpg] </center><br>
  * <center> Relationship between the SCCP JAIN OAM MBean </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface SccpRoutingControlMBean extends SccpManagedObjectMBean {
    /**
      * Sets the local Signalling Point (identified by the specified ObjectName) governed
      * by this routing control.
      *
      * @param OwnSignallingPoint
      * the ObjectName of the local Signalling Point
      */
    public void setOwnSignallingPoint(ObjectName ownSignallingPoint);
    /**
      * Returns the ObjectName of the local Signalling Point governed by this routing
      * control.
      *
      * @return
      * the ObjectName of the local Signalling Point governed by this routing control
      */
    public ObjectName getOwnSignallingPoint();
    /**
      * Returns the ObjectName of the SCCP Timer Profile MBean containing the SCCP Timer
      * values
      *
      * @return
      * the ObjectName of the SCCP Timer Profile MBean
      */
    public ObjectName getSccpTimerProfile();
    /**
      * Sets the SCCP Timer Profile containing the SCCP Timer values
      *
      * @param timerProfile
      * the ObjectName of the SCCP Timer Profile
      */
    public void setSccpTimerProfile(ObjectName timerProfile);
    /**
      * Returns an array of the ObjectNames of all Global Titles of this SCCP.
      *
      * @return
      * an array of all the ObjectName of all Global Title Entries of this SCCP.
      */
    public ObjectName[] getGlobalTitleEntries();
    /**
      * Returns the ObjectName of the specified Global Title entry of this SCCP.
      *
      * @return
      * an array of the ObjectName of the specified Global Title Entry of this SCCP.
      */
    public ObjectName getGlobalTitleEntry(Integer index);
    /**
      * Sets the list of Global Title Entries associated with this Routing Control.
      *
      * @param gtEntriesthe
      * ObjectName of the Global Titles to be added.
      *
      * @exception TooManyInstancesException
      * if the size of gtEntries is greater than the number of Global Title Entries
      * permitted for a Routing Control by this implementation.
      */
    public void setGlobalTitleEntries(ObjectName[] gtEntries)
        throws TooManyInstancesException;
    /**
      * Set this Global Title Entry as the specified element in the Global Title Entry
      * Array of this Routing Control.
      * 
      * @param index
      * the index of the element to be added.
      *
      * @param gtEntry
      * the ObjectName of the Global Title Entry to be added.
      *
      * @exception TooManyInstancesException
      * if index is greater than the number of Routing controls permitted for an SCCP
      * Layer by this implementation.
      */
    public void setGlobalTitleEntry(Integer index, ObjectName gtEntry)
        throws TooManyInstancesException;
    /**
      * Indicates if a Global Title (identified by the specified ObjectName) is assocaited
      * with this SCCP.
      *
      * @return <ul>
      * <li>true if the specified Global Title is associated with this SCCP.
      * <li>false otherwise. </ul>
      */
    public Boolean hasGlobalTitle(ObjectName globalTitleEntry);
    /**
      * Returns an array of ObjectNames of all the Subsystems (SCCP Access Points)
      * associated with the SCCP Routing Control (local signalling Point). These may
      * represent either Local or Remote Subsystems
      *
      * @return
      * an array of ObjectNames of SCCP Access Points (Subsystems)
      */
    public ObjectName[] getSubsystems();
    /**
      * Returns the ObjectName of one of the Subsystems (SCCP Access Points) associated
      * with the SCCP Routing Control (local signalling Point). These may represent either
      * Local or Remote Subsystems
      *
      * @param index
      * the index of the Subsystem within the set of local Subsystems.
      *
      * @return
      * the ObjectName of the SCCP Access Point (Subsystem)
      */
    public ObjectName getSubsystem(Integer index);
    /**
      * Sets all of the Subsystems (SCCP Access Points) associated with the SCCP Routing
      * Control (local signalling Point). These may represent either Local or Remote
      * Subsystems
      *
      * @param subsystems
      * the ObjectName of the SCCP Access Points
      *
      * @exception TooManyInstancesException
      * if the size of subsystems is greater than the number of Subsystems permitted by
      * this implementation.
      */
    public void setSubsystems(ObjectName[] subsystems)
        throws TooManyInstancesException;
    /**
      * Sets one of the Subsystems (SCCP Access Points) associated with the SCCP Routing
      * Control (local signalling Point). These may represent either Local or Remote
      * Subsystems
      *
      * @param index
      * the index of the Subsystem within the set of Subsystems.
      *
      * @param subsystem
      * the ObjectName of the SCCP Access Point to be added.
      *
      * @exception TooManyInstancesException
      * if index is greater than the number of Subsystems permitted by this
      * implementation.
      */
    public void setSubsystem(Integer index, ObjectName subsystem)
        throws TooManyInstancesException;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
