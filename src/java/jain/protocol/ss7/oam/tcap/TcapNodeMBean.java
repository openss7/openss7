/*
 @(#) $RCSfile: TcapNodeMBean.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:28 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:28 $ by $Author: brian $
 */

package jain.protocol.ss7.oam.tcap;

import javax.management.*;

import jain.protocol.ss7.oam.*;
import jain.protocol.ss7.*;
import jain.*;

/**
  * This interface defines a local TCAP Node. There will be one instance of a local TCAP
  * per local signalling point. Where multiple logical signalling points exist within the
  * one physical stack, there will be several local TCAP instances. <p>
  *
  * The following statistics can be collected for this MBean <ul>
  * <li>TCAP_TOTAL_RECEIVED
  * <li>TCAP_TOTAL_SENT
  * <li>TCAP_PROT_ERR_TRANS_PORT1
  * <li>TCAP_PROT_ERR_TRANS_PORT2
  * <li>TCAP_PROT_ERR_COMP_PORT </ul>
  *
  * The following Alarms can be emitted by this MBean <ul>
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED
  * <li>TCAP_ALARM_RESOURCE_LIMIT_REACHED </ul>
  *
  * The following Errors can be emitted by this MBean <ul>
  * <li>NON_OAM_RELATED_ERROR
  * <li>TCAP_ERROR_ILLEGAL_TIMER_EXPIRY
  * <li>TCAP_ERROR_MEMORY_ALLOCATION_FAILURE
  * <li>TCAP_ERROR_MSG_MISSING_MANDATORY_PARAM
  * <li>TCAP_ERROR_INVOKE_ID_IN_USE
  * <li>TCAP_ERROR_INVOKE_ID_UNRECOGNISED
  * <li>TCAP_ERROR_DIALOGUE_ID_UNRECOGNISED
  * <li>TCAP_ERROR_MSG_UNEXPECTED_TYPE
  * <li>TCAP_ERROR_DIALOGUE_ID_UNAVAILABLE </ul>
  *
  * <center> [TcapMBeans.jpg] </center><br>
  * <center> Relationship between the TCAP JAIN OAM MBean </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface TcapNodeMBean extends TcapManagedObjectMBean {
    /** Sets the signalling point of this TCAP Node identified by the specified
      * ObjectName.  */
    public void setOwnSignallingPoint(ObjectName ownSignallingPoint);
    /** Returns the ObjectName of the signalling point of this TCAP Node */
    public ObjectName getOwnSignallingPoint();
    /**
      * Sets one of the subsystems (identified by the specified ObjectName of a
      * SccpSapMBean) of this TCAP Node.
      *
      * @param index
      * the position at which the subsystem is to be set
      *
      * @param subsystem
      * the ObjectName of the subystem (SccpSapMBean) to be set for this TCAP node
      *
      * @exception TooManyInstancesException
      * if no more subsystems can be set for this TCAP Node
      */
    public void setSubsystem(Integer index, ObjectName subsystem)
        throws TooManyInstancesException;
    /**
      * Sets all of the subsystems (identified by the specified array of ObjectNames of
      * SccpSapMBeans) of this TCAP Node.
      *
      * @param subsystems
      * the ObjectName of the set of subsystems (SccpSapMBeans) to be set for this TCAP
      * Node
      *
      * @exception TooManyInstancesException
      * if no more subsystems can be set for this TCAP Node
      */
    public void setSubsystems(ObjectName[] subsystems)
        throws TooManyInstancesException;
    /**
      * Returns the ObjectName of one of the subsystems of this TCAP Node
      *
      * @param index
      * the index of the subsystem
      *
      * @return
      * the subsystem at the specified index
      */
    public ObjectName getSubsystem(Integer index);
    /**
      * Returns an array of ObjectNames of the set of subsystems of this TCAP Node
      *
      * @return
      * all of the subsystems of this TCAP Node
      */
    public ObjectName[] getSubsystems();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
