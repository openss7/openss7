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
 *  File Name     : FacilityGroupChoice.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.constants;


import java.io.*;
import java.util.*;
import java.lang.*;
/**
This class indicates the FacilityGroup choice
*/


public class FacilityGroupChoice {

    public static final int M_TRUNK_GROUP_ID =0;
    public static final int M_PRIVATE_FACILITY_ID =1;
    public static final int M_HUNT_GROUP =2;
    public static final int M_ROUTE_INDEX =3;
    
	
	// internal variable to store the constant value

	private int facilityGroupChoice;
	
	
/**
Constructor for FacilityGroup choice
*
*/

	private FacilityGroupChoice(int facilityGroupChoice) 
	{
    	
		this.facilityGroupChoice=facilityGroupChoice;
	
	}
   


/**
* FacilityGroupChoice : TRUNK_GROUP_ID 
*
*/

public static final FacilityGroupChoice TRUNK_GROUP_ID  = new FacilityGroupChoice (M_TRUNK_GROUP_ID);


/**
* FacilityGroupChoice : PRIVATE_FACILITY_ID 
*
*/

public static final FacilityGroupChoice  PRIVATE_FACILITY_ID = new FacilityGroupChoice (M_PRIVATE_FACILITY_ID);



/**
* FacilityGroupChoice : HUNT_GROUP
*
*/

public static final FacilityGroupChoice  HUNT_GROUP = new FacilityGroupChoice (M_HUNT_GROUP);



/**
* FacilityGroupChoice : ROUTE_INDEX
*
*/

public static final FacilityGroupChoice  ROUTE_INDEX = new FacilityGroupChoice (M_ROUTE_INDEX);




	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getFacilityGroupChoice()
	{
	
		return facilityGroupChoice;
	
	}
	
}
//end class

