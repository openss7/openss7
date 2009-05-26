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
 *  File Name     : ResourceAddressChoice.java
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
This class indicates the ResourceAddress choice
*/


public class ResourceAddressChoice {


	/*
	* Internal constatn available for switch statements 
	*/
	
	
	public static final int M_IP_ROUTING_ADDRESS =0;
    public static final int M_LEG_ID =1;
	public static final int M_IP_ADDRESS_AND_LEG_ID =2;
	public static final int M_NONE =3;
    public static final int M_CALL_SEGMENT_ID =4;
	public static final int M_IP_ADDRESS_AND_CALL_SEGMENT_ID =5;
    
	
	//internal variable to store the constant value  
	
	private int resourceAddressChoice ;
	
	/*
	* Constructor for ResourceAddressChoice Datatype 
	*/
	
	private ResourceAddressChoice (int resourceAddressChoice)
	{
	
		 this.resourceAddressChoice=resourceAddressChoice;
		
	}
	
		/**
	* ResourceAddressChoice :IP_ROUTING_ADDRESS
	*
	*/
	
	public static final ResourceAddressChoice IP_ROUTING_ADDRESS = new ResourceAddressChoice (M_IP_ROUTING_ADDRESS);
	
	
	/**
	* ResourceAddressChoice :LEG_ID
	*
	*/
	
	public static final ResourceAddressChoice  LEG_ID= new ResourceAddressChoice (M_LEG_ID);
	
	
		/**
	* ResourceAddressChoice :IP_ADDRESS_AND_LEG_ID
	*
	*/
	
	public static final ResourceAddressChoice  IP_ADDRESS_AND_LEG_ID= new ResourceAddressChoice (M_IP_ADDRESS_AND_LEG_ID);
	
		/**
	* ResourceAddressChoice :NONE
	*
	*/
	
	public static final ResourceAddressChoice NONE = new ResourceAddressChoice (M_NONE);
	
		/**
	* ResourceAddressChoice :CALL_SEGMENT_ID
	*
	*/
	
	public static final ResourceAddressChoice  CALL_SEGMENT_ID= new ResourceAddressChoice (M_CALL_SEGMENT_ID);
	
		/**
	* ResourceAddressChoice :IP_ADDRESS_AND_CALL_SEGMENT_ID
	*
	*/
	
	public static final ResourceAddressChoice  IP_ADDRESS_AND_CALL_SEGMENT_ID= new ResourceAddressChoice (M_IP_ADDRESS_AND_CALL_SEGMENT_ID);
	
		/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getResourceAddressChoice ()
	{
	
		return resourceAddressChoice;
	
	}	
	
	   
}// end class 
