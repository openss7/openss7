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
 *  File Name     : VariablePartChoice.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.constants;


import java.io.*;
import java.util.*;
/**
This class indicates the VariablePart choice
*/


public class VariablePartChoice {

	// internal constant avialable for switch statement 
	
	public static final int M_INTEGER =0;
    public static final int M_NUMBER =1;
    public static final int M_TIME =2;
    public static final int M_DATE =3; 
    public static final int M_PRICE =4;
	
	// internal variable to store the constant value 
	
	private int variablePartChoice ;
	
	/**
	* Constructor for VariablePartChoice  datatype 
	*/
	
	private VariablePartChoice (int variablePartChoice)
	{
	
		this.variablePartChoice = variablePartChoice ;
	}
	
	/**
	* VariablePartChoice :INTEGER
	*
	*/
	
	public static final VariablePartChoice INTEGER  = new VariablePartChoice (M_INTEGER);
	
		
	/**
	* VariablePartChoice :NUMBER
	*
	*/
	
	public static final VariablePartChoice NUMBER = new VariablePartChoice (M_NUMBER);
		
	/**
	* VariablePartChoice :TIME
	*
	*/
	
	public static final VariablePartChoice TIME = new VariablePartChoice (M_TIME);
		
	/**
	* VariablePartChoice :DATE
	*
	*/
	
	public static final VariablePartChoice  DATE= new VariablePartChoice (M_DATE);
		
	/**
	* VariablePartChoice :PRICE
	*
	*/
	
	public static final VariablePartChoice PRICE = new VariablePartChoice (M_PRICE);
	
	
	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/

	public int gerVariablePartChoice ()
	{
	
		return variablePartChoice;
	}
    
}
