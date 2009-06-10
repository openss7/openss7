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
 *  File Name     : InformationToRecord.java
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
import javax.jain.*;


/**
This class defines the InfromationToRecord Datatype
*/

public class InformationToRecord implements java.io.Serializable
{

    private int messageID;
    private boolean isMessageID = false ;

    private int messageDeletionTimeOut;
    private boolean isMessageDeletionTimeOut = false ;
	 
    private int timeToRecord;
	private boolean isTimeToRecord = false ;
	 
	private ControlDigits controlDigits;

/**
Constructor For InformationToRecord
*/
	public InformationToRecord(ControlDigits controlDigits)
	{
		setControlDigits(controlDigits);
	}

//-----------------------------------------
	 
/**
Gets Message ID
*/
    public int getMessageID() throws ParameterNotSetException
    {
        if(isMessageIDPresent())
		{
        	return messageID;
        }
		else
		{
            throw new ParameterNotSetException();
        }   
    }
    
/**
Sets Message ID
*/
    public void setMessageID(int messageID) throws IllegalArgumentException
    {
    	if((messageID>=0)&&(messageID<=2147483647))
    	{
        	this.messageID = messageID;
        	isMessageID=true;
        }
       else
	    {
           throw new IllegalArgumentException("ParameterOutOfRange");
        } 
		
    }

/**
Indicates if the Message ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isMessageIDPresent()
    {
        return isMessageID;
    }


//----------------------------------------------

/**
Gets Message Deletion Time Out
*/
    public int getMessageDeletionTimeOut() throws ParameterNotSetException
    {
        if(isMessageDeletionTimeOutPresent())
		{
        	return messageDeletionTimeOut;
        }
		else
		{
            throw new ParameterNotSetException();
        }   
    }
    
/**
Sets Message Deletion
*/
    public void setMessageDeletionTimeOut(int messageDeletionTimeOut) throws IllegalArgumentException

    {
    	if((messageDeletionTimeOut>=1) &&( messageDeletionTimeOut<=3600))
    	{
        	this.messageDeletionTimeOut = messageDeletionTimeOut;
        	isMessageDeletionTimeOut=true;
        }
        else
        {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        }
    }

/**
Indicates if the Message Deletion Time Out optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isMessageDeletionTimeOutPresent()
    {
        return isMessageDeletionTimeOut;
    }

    
//----------------------------------------------

/**
Gets Time To Record
*/
    public int getTimeToRecord() throws ParameterNotSetException
    {
       if(isTimeToRecordPresent())
	   { 
       		return timeToRecord;
       }
	   else
	   {
            throw new ParameterNotSetException();
       }   
    }
    
/**
Sets Time To Record
*/
    public void setTimeToRecord(int timeToRecord)
    {
        this.timeToRecord = timeToRecord;
        isTimeToRecord=true;
    }

/**
Indicates if the Time To Record optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isTimeToRecordPresent()
    {
        return isTimeToRecord;
    }


//----------------------------------------------

/**
Gets Control Digits
*/
    public ControlDigits getControlDigits()
    {
        return controlDigits;
    }
    
/**
Sets Control Digits
*/
    public void setControlDigits(ControlDigits controlDigits)
    {
        this.controlDigits = controlDigits;
    }

//----------------------------------------------

}
