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
 *  File Name     : ReceivedInformation.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


package javax.jain.ss7.inap.operation;

import java.io.*;
import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.datatype.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.*;
/**
This class represents the Received Information Operation.
*/

public class ReceivedInformation  extends Operation   implements Serializable
{
    private DigitsGenericDigits digitsResponse;
 
	  
    private String iA5Response;
 
    private ReceivedInformationChoice receivedInformationChoice;
    


/**
Constructors For ReceivedInformation
*/
	public ReceivedInformation(DigitsGenericDigits digitsResponse)
	{
		operationCode = OperationCode.PROMPT_AND_COLLECT_USER_INFORMATION;
		setDigitsResponse(digitsResponse);
	}

	public ReceivedInformation(String iA5Response)
	{
		operationCode = OperationCode.PROMPT_AND_COLLECT_USER_INFORMATION;
		setIA5Response(iA5Response);
	}


//--------------------------------------------------------------------------

/**
Gets  Received Information Choice
*/

	public ReceivedInformationChoice getReceivedInformationChoice()
		{ 
			return receivedInformationChoice;
		}


//-------------------------------
/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------

/**
Gets Digits Response
*/
    public DigitsGenericDigits getDigitsResponse() throws ParameterNotSetException
    {
        if(receivedInformationChoice.getReceivedInformationChoice() == ReceivedInformationChoice.DIGITS_RESPONSE.getReceivedInformationChoice())
              return digitsResponse;
        else
             throw new ParameterNotSetException();
    }
    
/**
Sets Digits Response
*/
    public void setDigitsResponse(DigitsGenericDigits digitsResponse)
    {
        this.digitsResponse = digitsResponse;
        receivedInformationChoice = ReceivedInformationChoice.DIGITS_RESPONSE;
    }

    
//----------------------------------------------

/**
Gets IA5Response 
*/
    public String getIA5Response() throws ParameterNotSetException
    {
        if(receivedInformationChoice.getReceivedInformationChoice() == ReceivedInformationChoice.IA5_RESPONSE.getReceivedInformationChoice())
               return iA5Response;
        else
             throw new ParameterNotSetException();

    }
    
/**
Sets IA5Response 
*/
    public void setIA5Response(String iA5Response)
    {
        this.iA5Response = iA5Response;
        receivedInformationChoice = ReceivedInformationChoice.IA5_RESPONSE;
 
    }

    
//--------------------------------------------

}
