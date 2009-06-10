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
 *  File Name     : ErrorTreatment.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.constants;


import java.io.*;
import java.util.*;

/**The Error Treatment class defines what specific action should be taken by the SRF in the event of error conditions occurring. 
The default value is Report_Error_To_SCF.
*/

public class ErrorTreatment {

    public static final int M_REPORT_ERROR_TO_SCF=0;
    public static final int M_HELP=1;
    public static final int M_REPEAT_PROMPT=2;
    
	
	//internal variable to store the constant value 
	
	private int errorTreatment;
	
	
/**
Constructor for ErrorTreatment datatype
*/
    
	
    private ErrorTreatment(int errorTreatment) 
	{
    	
		this.errorTreatment=errorTreatment;
		
	}

/**
* ErrorTreatment : REPORT_ERROR_TO_SCF 
*
*/


public static final ErrorTreatment REPORT_ERROR_TO_SCF =new ErrorTreatment (M_REPORT_ERROR_TO_SCF);




/**
* ErrorTreatment : HELP 
*
*/


public static final ErrorTreatment HELP =new ErrorTreatment (M_HELP);




/**
* ErrorTreatment : REPEAT_PROMPT 
*
*/


public static final ErrorTreatment REPEAT_PROMPT =new ErrorTreatment (M_REPEAT_PROMPT);




	/**
	* Gets the integer String representation of the Constant class 
	*
	* @ return  Integer provides value of Constant
	*
	*/
	
	public int getErrorTreatment()
	{
	
		return errorTreatment;
	
	}
    
}

