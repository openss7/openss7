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
 *  File Name     : ResourceAddress.java
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
This class defines the Resource Address Datatype
*/

public class ResourceAddress implements Serializable
{

    private CalledPartyNumber ipRoutingAddress;

    
    private LegID legID;

    
    private IPAddressAndLegID ipAddressAndLegID; 

    
	 private int none;

	 
	 private int callSegmentID;

	 
    private IPAddressAndCallSegmentID ipAddressAndCallSegmentID; 

	
	private ResourceAddressChoice resourceAddressChoice;


/**
Constructors For ResourceAddress
*/
	public ResourceAddress(CalledPartyNumber ipRoutingAddress)
	{
		setIPRoutingAddress(ipRoutingAddress);
	}

	public ResourceAddress(LegID legID)
	{
		setLegID(legID);
	}

	public ResourceAddress(IPAddressAndLegID ipAddressAndLegID)
	{
		setIPAddressAndLegID(ipAddressAndLegID);
	}

	public ResourceAddress(int callSegmentIDOrNone, ResourceAddressID resourceAddressID)
	{
		if(resourceAddressID.getResourceAddressID() == ResourceAddressID.NONE.getResourceAddressID())
			setNone(callSegmentIDOrNone);
		else if(resourceAddressID.getResourceAddressID() == ResourceAddressID.CALL_SEGMENT_ID.getResourceAddressID())
			setCallSegment(callSegmentIDOrNone);
	}

	public ResourceAddress(IPAddressAndCallSegmentID ipAddressAndCallSegmentID)
	{
		setIPAddressAndCallSegmentID(ipAddressAndCallSegmentID);
	}
	
//--------------------------------------	
    

/**
Gets Resource Address Choice
*/
   public ResourceAddressChoice getResourceAddressChoice()
   {
   return resourceAddressChoice;
   
   }


/**
Gets IP Routing Address
*/
    public CalledPartyNumber getIPRoutingAddress() throws ParameterNotSetException
    {
        if(resourceAddressChoice.getResourceAddressChoice()==ResourceAddressChoice.IP_ROUTING_ADDRESS.getResourceAddressChoice()){
         return ipRoutingAddress;
        }else{
               throw new ParameterNotSetException();
              }
    }

/**
Sets IP Routing Address
*/
    public void setIPRoutingAddress(CalledPartyNumber ipRoutingAddress)
    {
        
      this.ipRoutingAddress = ipRoutingAddress;
	  resourceAddressChoice=ResourceAddressChoice.IP_ROUTING_ADDRESS;
     // isIPRoutingAddress=true; 
    }

//----------------------------------------------

/**
Gets Leg ID
*/
    public LegID getLegID() throws ParameterNotSetException
    {
        if(resourceAddressChoice.getResourceAddressChoice()==ResourceAddressChoice.LEG_ID.getResourceAddressChoice()){
        return legID;
        }else{
               throw new ParameterNotSetException();
              }
    }

/**
Sets Leg ID
*/
    public void setLegID(LegID legID)
    {
        this.legID = legID;
        resourceAddressChoice=ResourceAddressChoice.LEG_ID;
		//isLegID=true;
    }

//----------------------------------------------

/**
Gets IP Address And Leg ID
*/
    public IPAddressAndLegID getIPAddressAndLegID() throws ParameterNotSetException
    {
        if(resourceAddressChoice.getResourceAddressChoice()==ResourceAddressChoice.IP_ADDRESS_AND_LEG_ID.getResourceAddressChoice()){
        return ipAddressAndLegID;
        }else{
               throw new ParameterNotSetException();
              } 
    }

/**
Sets IP Address And Leg ID
*/
    public void setIPAddressAndLegID(IPAddressAndLegID ipAddressAndLegID)
    {
        this.ipAddressAndLegID = ipAddressAndLegID;
        resourceAddressChoice=ResourceAddressChoice.IP_ADDRESS_AND_LEG_ID;
		//isIPAddressAndLegID=true;
    }
//----------------------------------------------

/**
Gets None
*/
    public int getNone() throws ParameterNotSetException
    {
       if(resourceAddressChoice.getResourceAddressChoice()==ResourceAddressChoice.NONE.getResourceAddressChoice()){ 
       return none;
        }else{
               throw new ParameterNotSetException();
              }
    }
    
/**
Sets None 
*/
    public void setNone(int none)
    {
        this.none = none;
        resourceAddressChoice=ResourceAddressChoice.NONE;
		//isNone=true;
    }


//----------------------------------------------
/**
Gets Call Segment ID
*/
    public int getCallSegmentID() throws ParameterNotSetException
    {
        if(resourceAddressChoice.getResourceAddressChoice()==ResourceAddressChoice.CALL_SEGMENT_ID.getResourceAddressChoice()){
        return callSegmentID;
        }else{
               throw new ParameterNotSetException();
              }
    }
    
/**
Sets Call Segment ID
*/
    public void setCallSegment(int callSegmentID)
    {
        this.callSegmentID = callSegmentID;
        resourceAddressChoice=ResourceAddressChoice.CALL_SEGMENT_ID;
		//isCallSegmentID=true;
    }


//----------------------------------------------

/**
Gets IP Address And Call Segment ID
*/
    public IPAddressAndCallSegmentID getIPAddressAndCallSegmentID() throws ParameterNotSetException
    {
        if(resourceAddressChoice.getResourceAddressChoice()==ResourceAddressChoice.IP_ADDRESS_AND_CALL_SEGMENT_ID.getResourceAddressChoice()){
         return ipAddressAndCallSegmentID;
        }else{
               throw new ParameterNotSetException();
              } 
    }

/**
Sets IP Address And Call Segment ID
*/
    public void setIPAddressAndCallSegmentID(IPAddressAndCallSegmentID ipAddressAndCallSegmentID)
    {
        this.ipAddressAndCallSegmentID = ipAddressAndCallSegmentID;
        resourceAddressChoice=ResourceAddressChoice.IP_ADDRESS_AND_CALL_SEGMENT_ID;
		//isIPAddressAndCallSegmentID=true;
    }


//----------------------------------------------

}
