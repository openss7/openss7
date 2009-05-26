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
 *  File Name     : URejectReqEvent.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


package javax.jain.ss7.inap.event;

import java.util.*;
import java.lang.*;
import javax.jain.ss7.inap.exception.*;
import javax.jain.ss7.inap.constants.*;

/**
 This Class contains information related to the Reject Component being sent
 */	
 
public class URejectReqEvent extends ComponentReqEvent
			implements java.io.Serializable
{

	private int invokeID;
 	private RejectType rejectType;
   private int rejectCode;


/**
Constructs a new URejectReqEvent. 
*/

	public URejectReqEvent(Object source, int invokeID ,RejectType rejecttype , int rejectcode)throws IllegalArgumentException
	{
		super(source);
		componentReqPrimitiveType=ComponentReqPrimitiveType.U_REJECT;
		setInvokeID(invokeID);
		setRejectType(rejecttype);
		setRejectCode(rejectcode);
	}

/**
Gets the type of component primitive .
*/
	public ComponentReqPrimitiveType getComponentReqPrimitiveType()
	{
		return componentReqPrimitiveType;
	}


//---------------------------------------------	

/**
Sets the Invoke ID.
*/
	public void setInvokeID(int invokeID)
	{
		this.invokeID = invokeID;
	}
	

/**
Gets the Invoke ID.
*/
	public int getInvokeID()
	{
		return invokeID;
	}



//---------------------------------------------	

/**
  Gets Reject Type
*/

    public RejectType getRejectType()
    {
        return rejectType;
    }

/**
  Sets Reject Type
*/
    public void setRejectType(RejectType rejectType) 
    {
	   		this.rejectType = rejectType;
    }
    

//-----------------------
	 
/**
  Gets Reject Code
*/

    public int getRejectCode()
    {
        return rejectCode;
    }

/**
  Sets Reject Code
*/
    public void setRejectCode(int rejectCode) throws IllegalArgumentException
    {

		  if (rejectType ==RejectType.INVOKE_PROBLEM)
		  {
          	InvokeProblem y = new InvokeProblem();
			  if (y.contains((Object)(new Integer(rejectCode))))
	   	  {
		       this.rejectCode = rejectCode;
		     }
		     else
			  {
         	   throw new IllegalArgumentException("ParameterOutOfRange");
	        } 

		  }
		  else if (rejectType == RejectType.RETURN_RESULT_PROBLEM)
		  {
            ReturnResultProblem y = new ReturnResultProblem();
			  if (y.contains((Object)(new Integer(rejectCode))))
	   	  {
		        this.rejectCode = rejectCode;
		     }
		     else
			  {
         	   throw new IllegalArgumentException("ParameterOutOfRange");
	        } 

		  }
		  else if (rejectType ==RejectType.RETURN_ERROR_PROBLEM)
		  {
          	ReturnErrorProblem y = new ReturnErrorProblem();
			  if (y.contains((Object)(new Integer(rejectCode))))
	   	  {
		        this.rejectCode = rejectCode;
		     }
		     else
			  {
         	   throw new IllegalArgumentException("ParameterOutOfRange");
	        } 

		  }
		  else throw new IllegalArgumentException("ParameterOutOfRange");	


    }
    
//-----------------------

}
