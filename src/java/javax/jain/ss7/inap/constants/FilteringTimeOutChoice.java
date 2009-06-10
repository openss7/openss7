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
 *  File Name     : FilteringTimeOutChoice.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.constants;


import java.io.*;
import java.util.*;
/**
This class indicates the FilteringTimeOut choice
*/


public class FilteringTimeOutChoice {

    public static final int M_DURATION =0;
    public static final int M_STOPTIME =1;
	
	
	// internal variable to store the constant value 
	
	private int filteringTimeOutChoice;
	
	/**
Constructor for FilteringTimeOutChoiec datatype
*/
	
	private FilteringTimeOutChoice (int filteringTimeOutChoice)
	{
	
	
	this.filteringTimeOutChoice=filteringTimeOutChoice;
	
	}
	
	
	/**
	*FilteringTimeOutChoice :DURATION
	*
	*/
	
	public static final FilteringTimeOutChoice  DURATION  = new FilteringTimeOutChoice(M_DURATION );
	
	/**
	*FilteringTimeOutChoice :STOPTIME
	*
	*/
	
	public static final FilteringTimeOutChoice STOPTIME  = new FilteringTimeOutChoice(M_STOPTIME );
	
	
	
	/**
	*gets the integer string representation of constant class and 
	 return Integer provides the value of constants
	 */
	 
	 public int getFilteringTimeOutChoice()
	 {
	 	return filteringTimeOutChoice;
	 }
	 
	
	
}
