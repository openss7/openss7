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
 *  File Name     : FilteringCriteriaChoice.java
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
This class indicates the FilteringCriteria choice
*/


public class FilteringCriteriaChoice {

    public static final int M_DIALLED_NUMBER =0;
    public static final int M_CALLING_LINE_ID =1;
    public static final int M_SERVICE_KEY =2;
    public static final int M_ADDRESS_AND_SERVICE =30;


// internal variable to store the constant value

	private int filteringCriteriaChoice;
	

/**
Constructor for FilteringCriteria choice
*
*/

    
    private FilteringCriteriaChoice(int filteringCriteriaChoice) 
	{
    	
		this.filteringCriteriaChoice=filteringCriteriaChoice;
	
	}
	
	
/**
*  FilteringCriteriaChoice :DIALLED_NUMBER 
*
*/

public static final FilteringCriteriaChoice  DIALLED_NUMBER  = new FilteringCriteriaChoice (M_DIALLED_NUMBER );


/**
*  FilteringCriteriaChoice :CALLING_LINE_ID
*
*/

public static final FilteringCriteriaChoice CALLING_LINE_ID  = new FilteringCriteriaChoice (M_CALLING_LINE_ID);


/**
*  FilteringCriteriaChoice :SERVICE_KEY
*
*/

public static final FilteringCriteriaChoice SERVICE_KEY   = new FilteringCriteriaChoice (M_SERVICE_KEY);


/**
*  FilteringCriteriaChoice :ADDRESS_AND_SERVICE
*
*/

public static final FilteringCriteriaChoice  ADDRESS_AND_SERVICE = new FilteringCriteriaChoice (M_ADDRESS_AND_SERVICE);



	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getFilteringCriteriaChoice()
	{
	
	return filteringCriteriaChoice;
	
	}

}
//end class
