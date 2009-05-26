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
 *  File Name     : CollectedInfoChoice.java
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
This class indicates the CollectedInfo Choice.
*/


public class CollectedInfoChoice {

    public static final int M_COLLECTED_DIGITS =0;
    public static final int M_IA5INFORMATION =1;
    
	
	
	//internal variable to store the constant value.
	
	private int collectedInfoChoice;
	
	
/**
Constructor for CollectedInfo Choice.

    */
	
    private CollectedInfoChoice(int collectedInfoChoice) 
	{
    	
		this.collectedInfoChoice=collectedInfoChoice;
		
    }    
	
	/**
	* CollectedInfoChoice : COLLECTED_DIGITS
	*
	*/
	
	public static final CollectedInfoChoice COLLECTED_DIGITS=new CollectedInfoChoice (M_COLLECTED_DIGITS);
	
	
	
	/**
	* CollectedInfoChoice : IA5INFORMATION
	*
	*/
	
	public static final CollectedInfoChoice IA5INFORMATION=new CollectedInfoChoice (M_IA5INFORMATION);
	
	
		/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getCollectedInfoChoice()
	{
	
			return collectedInfoChoice;
			
	}
		

}//end class
