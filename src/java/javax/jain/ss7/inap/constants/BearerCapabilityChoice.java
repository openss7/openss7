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
 *  File Name     : BearerCapabilityChoice.java
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
This class indicates the Bearer Capability choice
*/


public class BearerCapabilityChoice {

    public static final int M_BEARER_CAP =0;
    public static final int M_TMR =1;
   
   
   // Internal variable to store Constant value 
   
   private int bearerCapabilityChoice;
   
   
    
/**
Constructor for Entry choice   
        
*/

private BearerCapabilityChoice(int bearerCapabilityChoice)
	{

		this.bearerCapabilityChoice= bearerCapabilityChoice;

	}


/**
*	BearerCapabilityChoice :BEARER_CAP
*
*/

public static final BearerCapabilityChoice BEARER_CAP=new BearerCapabilityChoice (M_BEARER_CAP);



/**
*	BearerCapabilityChoice :TMR
*
*/

public static final BearerCapabilityChoice TMR=new BearerCapabilityChoice (M_TMR);


/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getBearerCapabilityChoice()
	
	{
			return bearerCapabilityChoice;
	
	}
	
}

