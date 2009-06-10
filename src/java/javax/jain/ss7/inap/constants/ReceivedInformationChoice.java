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
 *  File Name     : ReceivedInformationChoice.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.constants;


import java.io.*;
import java.util.*;
/**
This class indicates the Received Information choice
*/


public class ReceivedInformationChoice { 

	/*
	* Internal constant available for switch statements
	*/
	
    public static final int M_DIGITS_RESPONSE =0;
    public static final int M_IA5_RESPONSE =1;
    
	// Internal variable to store the constant value 
	
	private int receivedInformationChoice;
	
	
    
/**
*Constructor for Entry choice
*/    
    private  ReceivedInformationChoice(int receivedInformationChoice ) 
	{
    	
		this.receivedInformationChoice =receivedInformationChoice ;
	}
	
	/**
	* ReceivedInformationChoice  :DIGITS_RESPONSE
	*
	*/
	
	public static final ReceivedInformationChoice  DIGITS_RESPONSE= new ReceivedInformationChoice (M_DIGITS_RESPONSE);
	
	
    
	
	/**
	* ReceivedInformationChoice  :IA5_RESPONSE
	*
	*/
	
	public static final ReceivedInformationChoice  IA5_RESPONSE= new ReceivedInformationChoice (M_IA5_RESPONSE);
	
	
	
	
		/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getReceivedInformationChoice ()
	{
	
		return receivedInformationChoice;
	}
	
}
