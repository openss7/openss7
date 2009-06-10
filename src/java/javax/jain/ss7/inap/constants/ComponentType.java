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
 *  File Name     : ComponentType.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.constants;


import java.io.*;
import java.util.*;

/**
This class Component Type indicates the type of event that is reported to the SSF.
*/

public class ComponentType  {

    public static final int M_ANY=0;
    public static final int M_INVOKE=1;
    public static final int M_R_RESULT=2;
    public static final int M_R_ERROR=3;
    public static final int M_R_REJECT=4;
    public static final int M_R_RESULT_NOT_LAST=7;
    
	//internal variable to store the constant value
	
	private int componentType;
	
	
/**
Constructor for ComponentType datatype
*/
    
    private ComponentType(int componentType) 
	{
    	this.componentType=componentType;
		
    }    
	
	
/*
* ComponentType : ANY
*
*/

public static final ComponentType ANY =new ComponentType (M_ANY);



/*
* ComponentType : INVOKE
*
*/

public static final ComponentType INVOKE =new ComponentType (M_INVOKE);



/*
* ComponentType : R_RESULT
*
*/

public static final ComponentType R_RESULT =new ComponentType (M_R_RESULT);


/*
* ComponentType : R_ERROR
*
*/

public static final ComponentType R_ERROR =new ComponentType (M_R_ERROR);


/*
* ComponentType : R_REJECT
*
*/

public static final ComponentType R_REJECT =new ComponentType (M_R_REJECT);


/*
* ComponentType :R_RESULT_NOT_LAST
*
*/

public static final ComponentType R_RESULT_NOT_LAST =new ComponentType (M_R_RESULT_NOT_LAST);


	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getComponentType()
	{
	
		return componentType;
	
	}


}
