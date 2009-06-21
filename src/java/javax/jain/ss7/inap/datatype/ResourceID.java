/*
 @(#) $RCSfile: ResourceID.java,v $ $Name:  $($Revision: 1.1.2.1 $) $Date: 2009-06-21 11:34:55 $ <p>
 
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
 
 Last Modified $Date: 2009-06-21 11:34:55 $ by $Author: brian $
 */

package javax.jain.ss7.inap.datatype;

import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.*;
import javax.jain.ss7.*;
import javax.jain.*;

/** This class represents the ResourceID Datatype.
  * @version 1.2.2
  * @author Monavacon Limited
  */
public class ResourceID implements java.io.Serializable {
    /** Constructors For ResourceID.  */
    public ResourceID(DigitsGenericDigits lineID) {
        setLineID(lineID);
    }
    /** Constructors For ResourceID.  */
    public ResourceID(FacilityGroup facilityGroup) {
        setFacilityGroup(facilityGroup);
    }
    /** Constructors For ResourceID.  */
    public ResourceID(int group, GroupID groupID) {
        if (groupID == GroupID.FACILITY_GROUP_MEMBER_ID)
            setFacilityGroupMemberID(group);
        else if (groupID == GroupID.TRUNK_GROUP_ID)
            setTrunkGroupID(group);
    }
    /** Gets Resource ID Choice.  */
    public ResourceIDChoice getResourceIDChoice() {
        return resourceIDChoice;
    }
    /** Gets Line ID.  */
    public DigitsGenericDigits getLineID()
        throws ParameterNotSetException {
        if (resourceIDChoice == ResourceIDChoice.LINE_ID)
            return lineID;
        throw new ParameterNotSetException("Line Id: not set.");
    }
    /** Sets Line ID.  */
    public void setLineID(DigitsGenericDigits lineID) {
        this.lineID = lineID;
        resourceIDChoice = ResourceIDChoice.LINE_ID;
    }
    /** Gets Facility Group.  */
    public FacilityGroup getFacilityGroup()
        throws ParameterNotSetException {
        if (resourceIDChoice == ResourceIDChoice.FACILITY_GROUP_ID)
            return facilityGroup;
        throw new ParameterNotSetException("Facility Group: not set.");
    }
    /** Sets Facility Group.  */
    public void setFacilityGroup(FacilityGroup facilityGroup) {
        this.facilityGroup = facilityGroup;
        resourceIDChoice = ResourceIDChoice.FACILITY_GROUP_ID;
    }
    /** Gets Facility Group Member ID.  */
    public int getFacilityGroupMemberID()
        throws ParameterNotSetException {
        if (resourceIDChoice == ResourceIDChoice.FACILITY_GROUP_MEMBER_ID)
            return facilityGroupMemberID;
        throw new ParameterNotSetException("Group Member Id: not set.");
    }
    /** Sets Facility Group Member ID.  */
    public void setFacilityGroupMemberID(int facilityGroupMemberID) {
        this.facilityGroupMemberID = facilityGroupMemberID;
        resourceIDChoice = ResourceIDChoice.FACILITY_GROUP_MEMBER_ID;
    }
    /** Gets Trunk Group ID.  */
    public int getTrunkGroupID()
        throws ParameterNotSetException {
        if (resourceIDChoice == ResourceIDChoice.TRUNK_GROUP_ID)
            return trunkGroupID;
        throw new ParameterNotSetException("Trunk Group Id: not set.");
    }
    /** Sets Trunk Group ID.  */
    public void setTrunkGroupID(int trunkGroupID) {
        this.trunkGroupID = trunkGroupID;
        resourceIDChoice = ResourceIDChoice.TRUNK_GROUP_ID;
    }
    private DigitsGenericDigits lineID;
    private FacilityGroup facilityGroup;
    private int facilityGroupMemberID;
    private int trunkGroupID;
    private ResourceIDChoice resourceIDChoice;
}

// vim: sw=4 et tw=72 com=srO\:/**,mb\:*,ex\:*/,srO\:/*,mb\:*,ex\:*/,b\:TRANS,\://,b\:#,\:%,\:XCOMM,n\:>,fb\:-
