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
 *  File Name     : PAbortReason.java
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
This class defines the PAbortReason Class*/

public class PAbortReason  {

    
	// Internal constant available for switch statements 
	
	
	public static final int M_UNRECOGNIZED_MESSAGE_TYPE=0;
    public static final int M_UNRECOGNIZED_TRANSACTION_ID=1;
    public static final int M_BADLY_FORMATTED_TRANSACTION_PORTION=2;
    public static final int M_INCORRECT_TRANSACTION_PORTION=3;
    public static final int M_RESOURCE_LIMITATION=4;


	// internal variable to store the constant value 
	
	private int pAbortReason;
	
	
/**
Constructor for PAbortReason datatype
*/
    
    private PAbortReason(int pAbortReason) 
	{
	
		this.pAbortReason=pAbortReason;
	
	}
	
	
	/**
	* PAbortReason  :UNRECOGNIZED_MESSAGE_TYPE
	*
	*/
	
	public static final PAbortReason UNRECOGNIZED_MESSAGE_TYPE = new PAbortReason (M_UNRECOGNIZED_MESSAGE_TYPE);
	
	
	/**
	* PAbortReason  :UNRECOGNIZED_TRANSACTION_ID
	*
	*/
	
	public static final PAbortReason UNRECOGNIZED_TRANSACTION_ID = new PAbortReason (M_UNRECOGNIZED_TRANSACTION_ID);
	
	

	/**
	* PAbortReason  :BADLY_FORMATTED_TRANSACTION_PORTION
	*
	*/
	
	public static final PAbortReason BADLY_FORMATTED_TRANSACTION_PORTION = new PAbortReason (M_BADLY_FORMATTED_TRANSACTION_PORTION);
	
	

	/**
	* PAbortReason  :INCORRECT_TRANSACTION_PORTION 
	*
	*/
	
	public static final PAbortReason INCORRECT_TRANSACTION_PORTION = new PAbortReason (M_INCORRECT_TRANSACTION_PORTION);
	   

    /**
    * PAbortReason  :RESOURCE_LIMITATION
    *
    */
    
    public static final PAbortReason RESOURCE_LIMITATION = new PAbortReason (M_RESOURCE_LIMITATION);
    
    

    	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	
	public int getPAbortReason ()
	{
	
		return pAbortReason;
	
	}
}
//end class 


