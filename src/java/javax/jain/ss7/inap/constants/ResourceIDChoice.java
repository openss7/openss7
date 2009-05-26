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
 *  File Name     : ResourceIDChoice.java
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
This class indicates the ResourceID choice
*/


public class ResourceIDChoice {


	// Internal constant available for switch statements 
	
    public static final int M_LINE_ID =0;
    public static final int M_FACILITY_GROUP_ID =1;
    public static final int M_FACILITY_GROUP_MEMBER_ID=2;
    public static final int M_TRUNK_GROUP_ID =3;
	
	
	// internal variable to store the constant value 
	
	private int resourceIDChoice ;
	
	/**
	*  Constructor for ResourceIDChoice data type 
	*/
	
	private ResourceIDChoice ( int resourceIDChoice )
	{
	
		 this.resourceIDChoice = resourceIDChoice ;
		
	}
	
	/**
	* ResourceIDChoice  :LINE_ID
	*
	*/
	
	public static final  ResourceIDChoice LINE_ID= new ResourceIDChoice  (M_LINE_ID);
	

	/**
	* ResourceIDChoice  :FACILITY_GROUP_ID
	*
	*/
	
	public static final  ResourceIDChoice FACILITY_GROUP_ID = new ResourceIDChoice  (M_FACILITY_GROUP_ID);

	/**
	* ResourceIDChoice  :FACILITY_GROUP_MEMBER_ID
	*
	*/
	
	public static final  ResourceIDChoice FACILITY_GROUP_MEMBER_ID= new ResourceIDChoice  (M_FACILITY_GROUP_MEMBER_ID);

	/**
	* ResourceIDChoice  :TRUNK_GROUP_ID
	*
	*/
	
	public static final  ResourceIDChoice  TRUNK_GROUP_ID= new ResourceIDChoice  (M_TRUNK_GROUP_ID);
	

	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/

	public int getResourceIDChoice ()
	{
	
		return resourceIDChoice;
	}

}// end class 