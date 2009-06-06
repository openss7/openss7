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
 *  File Name     : MidCallReportType.java
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
The class MidCallReportType indicates whether to report the event when the SSF is in either
 the "monitoring state" only or in any state.
 */

public class MidCallReportType {

	/*
	* internal constant available for switch statements
	*/
	

    public static final int M_IN_MONITORING_STATE = 0;
    public static final int M_IN_ANY_STATE=1;
	
	//internal variable to store the constant value \
	
	private int midCallReportType;
	
	
	
/**
Constructor for MidCallReportType datatype
*/
    
    private MidCallReportType(int midCallReportType)
	{
    	
		this.midCallReportType=midCallReportType;
	}
	
/*
* MidCallReportType :IN_MONITORING_STATE
*
*/

public static final MidCallReportType  IN_MONITORING_STATE = new MidCallReportType (M_IN_MONITORING_STATE);


	
/*
* MidCallReportType :IN_ANY_STATE
*
*/

public static final MidCallReportType   IN_ANY_STATE= new MidCallReportType (M_IN_ANY_STATE);


	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	
	public int getMidCallReportType()
	{
	
		return midCallReportType;
	
	}



}
