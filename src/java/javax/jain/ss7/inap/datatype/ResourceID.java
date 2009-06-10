/*
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Copyrights:
 *
 *  Copyright - 1999 Sun Microsystems, Inc. All rights reserved.
 *  901 San Antonio Road, Palo Alto, California 94043, U.S.A.
 *
 *  This product and related documentation are protected by copyright and
 *  distributed under licenses restricting its use, copying, distribution, and
 *  decompilation. No part of this product or related documentation may be
 *  reproduced in any form by any means without prior written authorization of
 *  Sun and its licensors, if any.
 *
 *  RESTRICTED RIGHTS LEGEND: Use, duplication, or disclosure by the United
 *  States Government is subject to the restrictions set forth in DFARS
 *  252.227-7013 (c)(1)(ii) and FAR 52.227-19.
 *
 *  The product described in this manual may be protected by one or more U.S.
 *  patents, foreign patents, or pending applications.
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Author:
 *
 *  Mahindra British Telecom
 *  155 , Bombay - Pune Road 
 *  Pimpri ,
 *  Pune - 411 018.
 *
 *  Module Name   : JAIN INAP API
 *  File Name     : ResourceID.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;

import java.io.*;
import java.util.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.*;
/** 
This class represents the ResourceID Datatype.
*/


public class ResourceID implements java.io.Serializable
{
	 private DigitsGenericDigits lineID;


	 private FacilityGroup facilityGroup;


    private int facilityGroupMemberID;


    private int trunkGroupID;


	private ResourceIDChoice resourceIDChoice;   


/**
Constructors For ResourceID
*/
	public ResourceID(DigitsGenericDigits lineID)
	{
		setLineID(lineID);
	}

	public ResourceID(FacilityGroup facilityGroup)
	{
		setFacilityGroup(facilityGroup);
	}

	public ResourceID(int group, GroupID groupID)
	{
		if(groupID.getGroupID() == GroupID.FACILITY_GROUP_MEMBER_ID.getGroupID())
			setFacilityGroupMemberID(group);
		else if(groupID.getGroupID() ==GroupID.TRUNK_GROUP_ID.getGroupID())
			setTrunkGroupID(group);
	}

//--------------------------------------
/**
Gets Resource ID Choice
*/

	public ResourceIDChoice getResourceIDChoice()
		{ return resourceIDChoice;
		}

/**
Gets Line ID
*/
    public DigitsGenericDigits getLineID() throws ParameterNotSetException
    {
        if(resourceIDChoice.getResourceIDChoice()==ResourceIDChoice.LINE_ID.getResourceIDChoice()){
         return lineID;
        }else{
               throw new ParameterNotSetException();
              } 
    }
/**
Sets Line ID
*/
    public void setLineID(DigitsGenericDigits lineID)
    {
        this.lineID = lineID;
        resourceIDChoice=ResourceIDChoice.LINE_ID;
    }


/**
Gets Facility Group
*/
    public FacilityGroup getFacilityGroup() throws ParameterNotSetException
    {
      if(resourceIDChoice.getResourceIDChoice()==ResourceIDChoice.FACILITY_GROUP_ID.getResourceIDChoice()){  
      return facilityGroup;
        }else{
               throw new ParameterNotSetException();
              } 
    }
/**
Sets Facility Group
*/
    public void setFacilityGroup(FacilityGroup facilityGroup)
    {
        this.facilityGroup = facilityGroup;
        resourceIDChoice=ResourceIDChoice.FACILITY_GROUP_ID;
    }


/**
Gets Facility Group Member ID
*/
    public int getFacilityGroupMemberID() throws ParameterNotSetException
    {
        if(resourceIDChoice.getResourceIDChoice()==ResourceIDChoice.FACILITY_GROUP_MEMBER_ID.getResourceIDChoice()){
        return facilityGroupMemberID;
        }else{
               throw new ParameterNotSetException();
              } 
    }
/**
Sets Facility Group Member ID
*/
    public void setFacilityGroupMemberID(int facilityGroupMemberID)
    {
        this.facilityGroupMemberID = facilityGroupMemberID;
        resourceIDChoice=ResourceIDChoice.FACILITY_GROUP_MEMBER_ID;
    }

//-----------------------------------    

/**
Gets Trunk Group ID
*/
    public int getTrunkGroupID() throws ParameterNotSetException
    {
       if(resourceIDChoice.getResourceIDChoice()==ResourceIDChoice.TRUNK_GROUP_ID.getResourceIDChoice()){ 
       return trunkGroupID;
        }else{
               throw new ParameterNotSetException();
              } 
    }
/**
Sets Trunk Group ID
*/
    public void setTrunkGroupID(int trunkGroupID)
    {
        this.trunkGroupID = trunkGroupID;
        resourceIDChoice=ResourceIDChoice.TRUNK_GROUP_ID;
    }

//-----------------------------------    

}
