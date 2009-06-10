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
 *  File Name     : InbandInfo.java
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
 This class represents the InbandInfo DataType
 */

public class InbandInfo implements java.io.Serializable
{

	private MessageID messageID;
	 
    private int numberOfRepetitions;
    private boolean isNumberOfRepetitions = false ;

    private int duration;
	private boolean isDuration = false ;
   
	private int interval;
	private boolean isInterval = false ;
	 
/**
Constructor For InbandInfo
*/
	public InbandInfo(MessageID messageID)
	{
		setMessageID( messageID);
	}
	
//---------------------------------------------	
    
/**
  Gets Message Id
*/

    public MessageID getMessageID()
    {
        return messageID;
    }
/**
  Sets Message Id
/*/

	 public void setMessageID( MessageID messageID) 
    {
        this.messageID = messageID;
    }

//-----------------------

/**
  Gets Number Of Repetitions 
*/

    public int getNumberOfRepetitions() throws ParameterNotSetException
    {
    	 if(isNumberOfRepetitionsPresent())
		 {
         	return numberOfRepetitions;
         }
		 else
		 {
            throw new ParameterNotSetException();
         }   
    }

/**
  Sets Number Of Repetitions
*/
    public void setNumberOfRepetitions(int numberOfRepetitions) throws IllegalArgumentException
    {
    	if((numberOfRepetitions >=1)&&(numberOfRepetitions<=127 ))
    	 { 
         	this.numberOfRepetitions = numberOfRepetitions;
         	isNumberOfRepetitions=true;
         }
       	else
       	{  
        	throw new IllegalArgumentException("ParameterOutOfRange");
        } 
    }
    
/**
Indicates if the Number Of Repetition optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isNumberOfRepetitionsPresent()
    {
        return isNumberOfRepetitions;
    }
    

//-----------------------

/**
  Gets Duration*/

    public int getDuration() throws ParameterNotSetException
    {
        if(isDurationPresent())
		{
        	return duration;
        }
		else
		{
            throw new ParameterNotSetException();
        }   
    }

/**
  Sets Duration 
*/
    public void setDuration(int duration) throws IllegalArgumentException

    {
    	if((duration >=0)&&( duration <=32767))
    	{
        	this.duration = duration;
          	isDuration=true;
        }
        else
        {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        } 
    }
    
/**
Indicates if the Duration  optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/

    public boolean isDurationPresent()
    {
        return isDuration;
    }


//-----------------------

/**
  Gets Interval
*/

    public int getInterval() throws ParameterNotSetException
    {
       if(isIntervalPresent())
	   { 
       		return interval;
       }
	   else
	   {
            throw new ParameterNotSetException();
       }   
    }

/**
  Sets Interval
*/
    public void setInterval(int interval) throws IllegalArgumentException

    { 
     if((interval >=0 )&& (interval<=32767))
     {
     	this.interval = interval;
        isInterval=true;
     }
     else
	 {    
       	throw new IllegalArgumentException("ParameterOutOfRange");
     } 
    }
    
/**
Indicates if the Interval optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isIntervalPresent()
    {
        return isInterval;
    }



//-----------------------
    
}
