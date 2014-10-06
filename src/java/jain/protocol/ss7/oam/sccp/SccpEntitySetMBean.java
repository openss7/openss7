/*
 @(#) $RCSfile: SccpEntitySetMBean.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:27 $ <p>
 
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
  * This class identifies the set of access points that result from a global title
  * translation. This set may contain one or two access points that are stored in an
  * array. The distribution of SCCP traffic over the entities in the set depends on the
  * value of the Sharing Mode attribute. <p>
  *
  * NB: The two access points of this entity set are contained in an array where <ul>
  * <li>Sap[0] is known as the first entity
  * <li>Sap[1] is known as the second entity </ul>
  *
  * There are no statistics that can be collected for this MBean <p>
  *
  * The following Alarms can be emitted by this MBean <ul>
  * <li>ALARM_MO_CREATED
  * <li>ALARM_MO_DELETED
  * <li>ALARM_MO_ATTRIBUTE_VALUE_CHANGED </ul>
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
public abstract interface SccpEntitySetMBean extends SccpManagedObjectMBean {
    /** Sharing Mode Constant: "Solitary", indicates that there can only be one access
      * point in the set.  */
    public static final int SM_SOLITARY = 1;
    /** Sharing Mode Constant: "Dupli-Dominant", indicates that the second entity is a
      * backup for the first entity.  */
    public static final int SM_DUPLI_DOMINANT = 2;
    /** Sharing Mode Constant: "Dupli-Replacement", indicates the second entity is standby
      * for backup for the first entity, but after changeover, the primary and backup
      * roles are swapped.  */
    public static final int SM_DUPLI_REPLACEMENT = 3;
    /** Sharing Mode Constant: "Dupli-Loadshared", indicates that the load is shared over
      * both the entities in the set.  */
    public static final int SM_DUPLI_SHARED = 4;
    /**
      * Returns the sharing mode used by this SCCP entity set. The sharing mode determines
      * the distribution of SCCP traffic over the entities in this set.
      *
      * @return
      * one of the defined sharing mode constants
      */
    public Integer getSharingMode();
    /**
      * Sets the sharing mode used by this SCCP entity set. The sharing mode determines
      * the distribution of SCCP traffic over the entities in this set.
      *
      * @param sharingMode
      * one of the defined sharing mode constants
      *
      * @exception IllegalArgumentException
      * if the supplied sharing mode is not one of the defined sharing mode constants
      */
    public void setSharingMode(Integer sharingMode)
        throws IllegalArgumentException;
    /**
      * Sets the destination Subsystems (SCCP SAPs) of this Entity Set identified by the
      * specified ObjectName. The distribution of SCCP traffic over the entities in the
      * set depends on the value of the Sharing Mode attribute. There can be a maximum of
      * two SCCP Access Points in the set, with only one access point if the sharing mode
      * is SM_SOLITARY.
      *
      * @param destinationSubsystems
      * an array of ObjectNames (with a maximum size = 2) of SCCP access points.
      *
      * @exception TooManyInstancesException
      * if the size of destinationSubsystems is greater than 1 if the sharing mode is
      * SM_SOLITARY or if the size of destinationSubsystems is greater than 2 if the
      * sharing mode is SM_DUPLI_DOMINANT, SM_DUPLI_REPLACEMENT, or SM_DUPLI_SHARED.
      */
    public void setDestinationSubsystems(ObjectName[] destinationSubsystems)
        throws TooManyInstancesException;
    /**
      * Returns the ObjectName of the destination Subsystems (SCCP SAPs) of this Entity
      * Set. The distribution of SCCP traffic over the entities in the set depends on the
      * value of the Sharing Mode attribute. There can be a maximum of two SCCP Access
      * Points in the set, with only one access point if the sharing mode is SM_SOLITARY.
      *
      * @return
      * an array (with a maximum size = 2) of SCCP access points.
      */
    public ObjectName[] getDestinationSubsystems();
    /**
      * Sets one of the destination Subsystems (SCCP SAPs) of this Entity Setidentified by
      * the specified ObjectName.
      *
      * @param index
      * the access point array index. There can be a maximum of two SCCP Access Points in
      * the set, with only one access point if the sharing mode is SM_SOLITARY therefore
      * the index can have a value of only 0 or 1.
      *
      * @param destinationSubsystem
      * the ObjectName of an SCCP access point identifying the destination Subsystem.
      *
      * @exception TooManyInstancesException
      * if index is greater than 1 if the sharing mode is SM_SOLITARY or if index is
      * greater than 2 if the sharing mode is SM_DUPLI_DOMINANT, SM_DUPLI_REPLACEMENT, or
      * SM_DUPLI_SHARED.
      */
    public void setDestinationSubsystem(Integer index, ObjectName destinationSubsystem)
        throws TooManyInstancesException;
    /**
      * Returns the ObjectName of one the destination Subsystems (SCCP SAPs) of this
      * Entity Set.
      *
      * @param index
      * the access point array index. There can be a maximum of two SCCP Access Points in
      * the set, with only one access point if the sharing mode is SM_SOLITARY therefore
      * the index can have a value of only 0 or 1.
      *
      * @return
      * the SCCP access point identifying the destination Subsystem.
      */
    public ObjectName getDestinationSubsystem(Integer index);
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
