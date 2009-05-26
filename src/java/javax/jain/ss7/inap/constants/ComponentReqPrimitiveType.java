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
 *  File Name     : ComponenetPrimitiveType.java
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
This class indicates the type of Component Request Primitive to be used.
*/

public class ComponentReqPrimitiveType {

    public static final int M_INVOKE=1;
    public static final int M_R_RESULT_L=2;
    public static final int M_R_RESULT_NL=3;
    public static final int M_U_ERROR=4;
    public static final int M_U_REJECT=5;
    public static final int M_U_CANCEL=8;
    public static final int M_TIMER_RESET=10;
    
    
	
	//internal  variable to store the constant value
	
	private int componentReqPrimitiveType;
	
	
/**
Constructor for ComponentPrimitiveType datatype
*/
    
    private ComponentReqPrimitiveType(int componentReqPrimitiveType)
	{
	
    		this.componentReqPrimitiveType=componentReqPrimitiveType;
			
    }    
	
/*
* ComponentPrimitiveType : INVOKE 
*
*/

public static final ComponentReqPrimitiveType INVOKE=new ComponentReqPrimitiveType (M_INVOKE);



	
/*
* ComponentPrimitiveType :R_RESULT_L
*
*/

public static final ComponentReqPrimitiveType R_RESULT_L=new ComponentReqPrimitiveType (M_R_RESULT_L);



	
/*
* ComponentPrimitiveType :  R_RESULT_NL
*
*/

public static final ComponentReqPrimitiveType  R_RESULT_NL=new ComponentReqPrimitiveType (M_R_RESULT_NL);




	
/*
* ComponentPrimitiveType : U_ERROR
*
*/

public static final ComponentReqPrimitiveType U_ERROR=new ComponentReqPrimitiveType (M_U_ERROR);




	
/*
* ComponentPrimitiveType : U_REJECT
*
*/

public static final ComponentReqPrimitiveType U_REJECT=new ComponentReqPrimitiveType (M_U_REJECT);



	
/*
* ComponentPrimitiveType : U_CANCEL
*
*/

public static final ComponentReqPrimitiveType U_CANCEL=new ComponentReqPrimitiveType (M_U_CANCEL);



/*
* ComponentPrimitiveType : TIMER_RESET
*
*/

public static final ComponentReqPrimitiveType TIMER_RESET=new ComponentReqPrimitiveType (M_TIMER_RESET);





	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getComponentReqPrimitiveType()
	{
	
		return componentReqPrimitiveType;
	
	}

}//end class



