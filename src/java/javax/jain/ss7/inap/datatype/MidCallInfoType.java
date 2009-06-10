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
 *  File Name     : MidCallInfoType.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.*;


/**
This class defines the MidCallInfoType Datatype
*/

public class MidCallInfoType implements java.io.Serializable
{

    private DigitsGenericDigits iNServiceControlCodeLow;
    
    private DigitsGenericDigits iNServiceControlCodeHigh;
    private boolean isINServiceControlCodeHigh = false ;

/**
Constructor For MidCallInfoType
*/
	public MidCallInfoType(DigitsGenericDigits iNServiceControlCodeLow)
	{
		setINServiceControlCodeLow(iNServiceControlCodeLow);
	}
	
//---------------------------------------------	


/**
Gets IN Service Control Code Low
*/

    public DigitsGenericDigits getINServiceControlCodeLow()
    {
        return iNServiceControlCodeLow;
    }

/**
Sets IN Service Control Code Low
*/

    public void  setINServiceControlCodeLow(DigitsGenericDigits iNServiceControlCodeLow)
    {
       this.iNServiceControlCodeLow= iNServiceControlCodeLow;
    }

//---------------------------------------------	

/**
Gets IN Service Control Code High
*/

    public DigitsGenericDigits getINServiceControlCodeHigh() throws ParameterNotSetException
    {
       if(isINServicecontrolcodehighPresent())
	   { 
       		return iNServiceControlCodeHigh;
       }
	   else
	   {
            throw new ParameterNotSetException();
       }   
    }

/**
Sets IN Service Control Code High
*/

    public void  setINServiceControlCodeHigh(DigitsGenericDigits iNServiceControlCodeHigh)
    {
       this.iNServiceControlCodeHigh= iNServiceControlCodeHigh;
       isINServiceControlCodeHigh=true; 
    }


/**
Indicates if the IN Service Control Code High optional parameter is present .
Returns: TRUE if present , false otherwise.
*/

    public boolean isINServicecontrolcodehighPresent()
    {
        return isINServiceControlCodeHigh;

    }

//---------------------------------------------	

}
