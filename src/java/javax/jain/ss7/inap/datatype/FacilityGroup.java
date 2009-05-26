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
 *  File Name     : FacilityGroup.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.*;


/**
This Class defines the FacilityGroup Datatype
*/


public class FacilityGroup implements Serializable 
{

    private int trunkGroupID;

    
    private int privateFacilityID;

    
    private String huntGroup;


    private String routeIndex;


	private FacilityGroupChoice facilityGroupChoice;
	
	private FacilityGroupID facilityGroupID;
	
	
/**
Constructor For FacilityGroup
*/

public FacilityGroup(FacilityGroupID groupID, FacilityGroupID facilityGroupID) 
	{
	if(groupID.getFacilityGroupID() == FacilityGroupID.PRIVATE_FACILITY.getFacilityGroupID())
		setPrivateFacilityID(facilityGroupID.getFacilityGroupID());
	else if(groupID.getFacilityGroupID() == FacilityGroupID.TRUNK_GROUP.getFacilityGroupID())
		setTrunkGroupID(facilityGroupID.getFacilityGroupID());
	}
	
public FacilityGroup(FacilityGroupID groupID, String facilityGroupID) 
{
   if(groupID.getFacilityGroupID() == FacilityGroupID.HUNT_GROUP.getFacilityGroupID())
		setHuntGroup(facilityGroupID);
   else if(groupID.getFacilityGroupID() == FacilityGroupID.ROUTE_INDEX.getFacilityGroupID())
		setRouteIndex(facilityGroupID);
	
	}



	
	
//---------------------------------------------	

/**
Gets  Facility Group Choice
*/

	public FacilityGroupChoice getFacilityGroupChoice()
		{ 
			return facilityGroupChoice;
		}

/**
Gets  Trunk Group ID
*/

    public int getTrunkGroupID() throws ParameterNotSetException
    {
        if(facilityGroupChoice.getFacilityGroupChoice() ==FacilityGroupChoice.TRUNK_GROUP_ID.getFacilityGroupChoice())
		{
        	return trunkGroupID;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }

/**
Sets  Trunk GroupID
*/

    public void  setTrunkGroupID (int trunkGroupID)
    {
        this.trunkGroupID = trunkGroupID;
        facilityGroupChoice =FacilityGroupChoice.TRUNK_GROUP_ID;
    }



//---------------------------------------------	

/**
Gets  Private Facility ID
*/

    public int getPrivateFacilityID() throws ParameterNotSetException
    {
       if( facilityGroupChoice.getFacilityGroupChoice() ==FacilityGroupChoice.PRIVATE_FACILITY_ID.getFacilityGroupChoice())
	   {
       		return privateFacilityID;
       }
	   else
	   {
            throw new ParameterNotSetException();
       }
    }

/**
Sets  Private Facility ID
*/

    public void  setPrivateFacilityID (int privateFacilityID)
    {
        privateFacilityID = privateFacilityID;
        facilityGroupChoice =FacilityGroupChoice.PRIVATE_FACILITY_ID;
    }



//---------------------------------------------	

/**
Gets  Hunt Group
*/

    public String getHuntGroup() throws ParameterNotSetException
    {
       if(facilityGroupChoice.getFacilityGroupChoice() ==FacilityGroupChoice.HUNT_GROUP.getFacilityGroupChoice())
	   { 
       		return huntGroup;
       }
	   else
	   {
            throw new ParameterNotSetException();
       }
    }

/**
Sets  Hunt Group
*/

    public void  setHuntGroup (String huntGroup)
    {
        this.huntGroup = huntGroup;
        facilityGroupChoice =FacilityGroupChoice.HUNT_GROUP;
    }

//---------------------------------------------	

/**
Gets  Route Index
*/

    public String getRouteIndex() throws ParameterNotSetException
    {
        if(facilityGroupChoice.getFacilityGroupChoice() ==FacilityGroupChoice.ROUTE_INDEX.getFacilityGroupChoice())
		{
        	return routeIndex;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }

/**
Sets  Route Index
*/

    public void  setRouteIndex (String routeIndex)
    {
        this.routeIndex = routeIndex;
        facilityGroupChoice =FacilityGroupChoice.ROUTE_INDEX;
    }

//---------------------------------------------	

}
