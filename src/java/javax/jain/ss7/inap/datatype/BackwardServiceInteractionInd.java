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
 *  File Name     : BackwardServiceInteractionInd.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.*;

/**
This Class defines the BackwardServiceInteractionIndicator Datatype
*/

public class BackwardServiceInteractionInd  implements Serializable 
{

    private String conferenceTreatmentIndicator;
    private boolean isConferenceTreatmentIndicator = false ;
        
    private String callCompletionTreatmentIndicator;
    private boolean isCallCompletionTreatmentIndicator = false ;
    

/**
Gets Conference Treatment Indicator
*/
    public String getConferenceTreatmentIndicator() throws ParameterNotSetException 
    {
        if(isConferenceTreatmentIndicatorPresent())
		{
        	return conferenceTreatmentIndicator;
        }
		else
		{
              throw new ParameterNotSetException();  
        }
    }
/**
Sets Conference Treatment Indicator
*/

    public void setConferenceTreatmentIndicator(String conferenceTreatmentIndicator)
    {
        this.conferenceTreatmentIndicator=conferenceTreatmentIndicator;
        isConferenceTreatmentIndicator=true;        
    }


/**
Indicates if the Conference  Treatment Indicator optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isConferenceTreatmentIndicatorPresent()
    {
        return isConferenceTreatmentIndicator;
    }
    
//----------------------------------------------

/**
Gets Call Completion Treatment Indicator
*/

    public String getCallCompletionTreatmentIndicator() throws ParameterNotSetException  
    {
        if(isCallCompletionTreatmentIndicatorPresent())
		{
        	return callCompletionTreatmentIndicator;
		}
		else
		{
                throw new ParameterNotSetException();
        }
    }
/**
Sets Call Completion Treatment Indicator
*/

    public void setCallCompletionTreatmentIndicator(String callCompletionTreatmentIndicator)
    {
        this.callCompletionTreatmentIndicator=callCompletionTreatmentIndicator;
        isCallCompletionTreatmentIndicator=true;
    }



/**
Indicates if the Call Completion Treatment Indicator optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCallCompletionTreatmentIndicatorPresent()
    {
        return isCallCompletionTreatmentIndicator;
    }

}