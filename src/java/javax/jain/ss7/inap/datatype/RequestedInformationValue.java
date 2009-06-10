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
 *  File Name     : RequestedInformationValue.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.exception.*;


/**
This Class defines the RequestedInformationValue Datatype
*/

public class RequestedInformationValue implements java.io.Serializable {

         private int callAttemptElapsedTimeValue;   

	 private String callStopTimeValue;
	 
 	 private int callConnectedElapsedTimeValue;
	  
	 private DigitsGenericNumber calledAddressValue;
	 
	 private Cause releaseCauseValue;

	 private RequestedInformationValueChoice requestedInformationValueChoice;


/**
Constructors For RequestedInformationValue
*/
	public RequestedInformationValue(int callElapsedTimeValue, ElapsedTimeValueID elapsedTimeValueID) throws IllegalArgumentException
	{
	if(elapsedTimeValueID.getElapsedTimeValueID() == ElapsedTimeValueID.CALL_ATTEMPT_ELAPSED_TIME_VALUE.getElapsedTimeValueID())
		setCallAttemptElapsedTimeValue(callElapsedTimeValue);
	else if(elapsedTimeValueID.getElapsedTimeValueID() == ElapsedTimeValueID.CALL_CONNECTED_ELAPSED_TIME_VALUE.getElapsedTimeValueID())
		setCallConnectedElapsedTimeValue(callElapsedTimeValue);
	}

	public RequestedInformationValue(java.lang.String callStopTimeValue)
	{
		setCallStopTimeValue(callStopTimeValue);
	}

	public RequestedInformationValue(DigitsGenericNumber calledAddressValue)
	{
		setCalledAddressValue(calledAddressValue);
	}
		
	public RequestedInformationValue(Cause releaseCauseValue)
	{
		setReleaseCauseValue(releaseCauseValue);
	}

//--------------------------------------

/**
  Gets Call Attempt Elapsed Time Value
*/

    public int getCallAttemptElapsedTimeValue()
    {
        return callAttemptElapsedTimeValue;
    }

/**
  Sets Call Attempt Elapsed Time Value
*/
    public void setCallAttemptElapsedTimeValue(int callAttemptElapsedTimeValue) throws IllegalArgumentException
    {  
      if((callAttemptElapsedTimeValue>=0)&&(callAttemptElapsedTimeValue<=255))
       {
       this.callAttemptElapsedTimeValue = callAttemptElapsedTimeValue;
       requestedInformationValueChoice = RequestedInformationValueChoice.CALL_ATTEMPT_ELAPSED_TIME_VALUE;

       }
       else
       {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        }
    }


//-----------------------

/**
  Gets Call Stop Time Value
*/

    public java.lang.String getCallStopTimeValue()
    {
        return callStopTimeValue;
    }

/**
  Sets Call Stop Time Value
*/
    public void setCallStopTimeValue(java.lang.String callStopTimeValue)
    {
        this.callStopTimeValue= callStopTimeValue;
        requestedInformationValueChoice = RequestedInformationValueChoice.CALL_STOP_TIME_VALUE;

    }


//-----------------------
/**<P>
Gets Call Connected Elapsed Time Value.

<LI>CALL_ATTEMPT_ELAPSED_TIME_VALUE - This parameter gets the duration between
the end of INAP processing of operations initiating call setup ("Connect", "AnalyseInformation",
"CollectInformation", "Continue" and "SelectRoute") and the received answer indication from the
indicated or default called party side.
<LI>CALL_CONNECTED_ELAPSED_TIME_VALUE -This parameter gets the duration between the
received answer indication from the indicated or default called party side and the release of that connection.	
<P>
*/

    public int getCallConnectedElapsedTimeValue()
    {
        return callConnectedElapsedTimeValue;
    }

/**
  Sets Call Connected Elapsed Time Value
*/
    public void setCallConnectedElapsedTimeValue(int callConnectedElapsedTimeValue)throws IllegalArgumentException
    {
    	 if((callConnectedElapsedTimeValue>=0)&&(callConnectedElapsedTimeValue<=2147483647))
    	 {
         this.callConnectedElapsedTimeValue = callConnectedElapsedTimeValue;
         requestedInformationValueChoice = RequestedInformationValueChoice.CALL_CONNECTED_ELAPSED_TIME_VALUE;

        }
        else
        {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        }
    }


//-----------------------

/**
  Gets Called Address Value
*/

    public DigitsGenericNumber getCalledAddressValue()
    {
        return calledAddressValue;
    }

/**
  Sets Called Address Value
*/
    public void setCalledAddressValue(DigitsGenericNumber calledAddressValue)
    {
        this.calledAddressValue = calledAddressValue;
        requestedInformationValueChoice = RequestedInformationValueChoice.CALLED_ADDRESS_VALUE;

    }


//-----------------------

/**
  Gets Release Cause Value
*/

    public Cause getReleaseCauseValue()
    {
        return releaseCauseValue;
    }

/**
  Sets Release Cause Value
*/
    public void setReleaseCauseValue(Cause releaseCauseValue)
    {
     	this.releaseCauseValue = releaseCauseValue;
	   requestedInformationValueChoice = RequestedInformationValueChoice.RELEASE_CAUSE_VALUE;
    }
//-----------------------

/**
  Gets Requested Information Choice
*/

    public RequestedInformationValueChoice getRequestedInformationChoice()
    {
        return requestedInformationValueChoice;
    }


//-----------------------

}

