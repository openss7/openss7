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
 *  File Name     : GapCriteriaChoice.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.constants;


import java.io.*;
import java.util.*;
/**
This class indicates the GapCriteria choice
*/


public class GapCriteriaChoice {

    public static final int M_CALLED_ADDRESS_VALUE =0;
    public static final int M_GAP_ON_SERVICE =2;
    public static final int M_GAP_ALL_IN_TRAFFIC =3;
    public static final int M_CALLED_ADDRESS_AND_SERVICE =29;
	 public static final int M_CALLING_ADDRESS_AND_SERVICE =30;


// internal variable to store the constant value 

	private int gapCriteriaChoice;
	
	

/**
Constructor for GapCriteria choice
*
*/

    private  GapCriteriaChoice(int gapCriteriaChoice) 
	{
	
		this.gapCriteriaChoice=gapCriteriaChoice;
	}
	
/**
* GapCriteriaChoice :CALLED_ADDRESS_VALUE 
*
*/

public static final GapCriteriaChoice CALLED_ADDRESS_VALUE  =new GapCriteriaChoice (M_CALLED_ADDRESS_VALUE);


	
/**
* GapCriteriaChoice :GAP_ON_SERVICE 
*
*/

public static final GapCriteriaChoice  GAP_ON_SERVICE =new GapCriteriaChoice (M_GAP_ON_SERVICE);



	
/**
* GapCriteriaChoice : GAP_ALL_IN_TRAFFIC
*
*/

public static final GapCriteriaChoice  GAP_ALL_IN_TRAFFIC =new GapCriteriaChoice (M_GAP_ALL_IN_TRAFFIC);



	
/**
* GapCriteriaChoice :CALLED_ADDRESS_AND_SERVICE 
*
*/

public static final GapCriteriaChoice  CALLED_ADDRESS_AND_SERVICE =new GapCriteriaChoice (M_CALLED_ADDRESS_AND_SERVICE);



	
/**
* GapCriteriaChoice : CALLING_ADDRESS_AND_SERVICE
*
*/

public static final GapCriteriaChoice CALLING_ADDRESS_AND_SERVICE  =new GapCriteriaChoice (M_CALLING_ADDRESS_AND_SERVICE);




	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getGapCriteriaChoice() 
	{
			
			return gapCriteriaChoice;
	}
	
}

