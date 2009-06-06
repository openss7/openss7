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
 *  File Name     : DialoguePrimitiveType.java
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
This class indicates the type of Dialogue Primitive to be used.
*/

public class DialoguePrimitiveType  {

    public static final int M_BEGIN=1;
    public static final int M_CONTINUE=2;
    public static final int M_END=3;
    public static final int M_UABORT=4;
	public static final int M_PABORT=5;
	public static final int M_NOTICE=6;
	
	//internal variable to store the constant value 
	
	private int dialoguePrimitiveType;
	
	   
    
/**
Constructor for DialoguePrimitiveType datatype
*/
    
    private DialoguePrimitiveType(int dialoguePrimitiveType)
	 {
	 	this.dialoguePrimitiveType=dialoguePrimitiveType;
	 }
	 
	 
	
/*
* DialogurPrimitiveType : BEGIN
*
*/
	public static final DialoguePrimitiveType BEGIN = new DialoguePrimitiveType (M_BEGIN);
	
	
	 
	 
/*
* DialogurPrimitiveType :CONTINUE
*
*/
	public static final DialoguePrimitiveType CONTINUE = new DialoguePrimitiveType (M_CONTINUE);
	
	
	
/*
* DialogurPrimitiveType : END 
*
*/
	public static final DialoguePrimitiveType END = new DialoguePrimitiveType (M_END);
	
	
	
/*
* DialogurPrimitiveType : UABORT
*
*/
	public static final DialoguePrimitiveType UABORT = new DialoguePrimitiveType (M_UABORT);
	
	
	
/*
* DialogurPrimitiveType : PABORT
*
*/
	public static final DialoguePrimitiveType PABORT = new DialoguePrimitiveType (M_PABORT);
	
	
	
/*
* DialogurPrimitiveType : NOTICE
*
*/
	public static final DialoguePrimitiveType NOTICE = new DialoguePrimitiveType (M_NOTICE);
	
	
	
	

	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getDialoguePrimitiveType ()
	{
	
		return dialoguePrimitiveType;
	}
	
    	
    
}
