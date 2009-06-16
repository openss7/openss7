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
  * This interface contains a list of remote Subsystems (SccpSaps) to be informed of local
  * (primary broadcast) or remote (secondary broadcast) SCCP Subsystem status changes, or
  * to be informed of the SCCP status after completion of SCCP Restart. <p>
  *
  * SCCP management procedures utilize the concept of a "concerned" Subsystem. In this
  * context, a "concerned" Subsystem means an entity with an immediate need to be informed
  * of a particular Subsystem status change, independently of whether SCCP communication
  * is in progress between the "concerned" Subsystem and the Subsystem affected by the
  * status change. <p>
  *
  * Each SCCP Service Access Point (SAP) (which may represent either a local or remote
  * Subsystem) has a Concerned Area associated with it.  When the status of the SCCP SAP
  * changes, the Concerned Area indicates the Subsystems that are to be informed. <p>
  *
  * There are no statistics that can be collected for this MBean <p>
  *
  * The following Alarms can be emitted by this MBean <ul>
  *
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED </ul>
  *
  * The following Errors can be emitted by this MBean <ul>
  *
  * <li>NON_OAM_RELATED_ERROR </ul>
  *
  * <center> [SccpMBeans.jpg] </center><br>
  * <center> Relationship between the SCCP JAIN OAM MBean </center><br>
  *
  * @author Monavacon Limited
  * @version 1.2.2
  */
public abstract interface ConcernedAreaMBean extends SccpManagedObjectMBean {
    /**
      * Sets one of the Remote Subsystems to be informed of local or remote subsystem
      * status changes.
      *
      * @param index
      * The index at which the Subsystem is to be set in the array.
      *
      * @param remoteSccp
      * The ObjectName of the remote Subsystem.
      *
      * @exception TooManyInstancesException
      * If index is greater than the number of remote subsystems permitted in a Concerned
      * Area by this implementation.
      */
    public void setRemoteSubsystem(Integer index, ObjectName remoteSccp)
        throws TooManyInstancesException;
    /**
      * Returns an ObjectName of the remote Subsystem from the list of Subsystems to be
      * informed of local or remote subsystem status changes .
      *
      * @param index
      * The index of the Subsystem.
      *
      * @return
      * The remote Subsystem.
      */
    public ObjectName getRemoteSubsystem(Integer index);
    /**
      * Sets all of the Subsystems (identified by the specified array of ObjectNames) to
      * be informed of local or remote subsystem status changes.
      *
      * @param remoteSccps
      * An array of ObjectNames of the remote Subsystem list.
      *
      * @exception TooManyInstancesException
      * If the size of remoteSccps is greater than the number of remote subsystems
      * permitted in a Concerned Area by this implementation.
      */
    public void setRemoteSubsystems(ObjectName[] remoteSccps)
        throws TooManyInstancesException;
    /**
      * Returns an array of ObjectNames of the all the Subsystems to be informed of local
      * or remote subsystem status changes
      *
      * @return
      * The remote Subsystem list.
      */
    public ObjectName[] getRemoteSubsystems();
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
