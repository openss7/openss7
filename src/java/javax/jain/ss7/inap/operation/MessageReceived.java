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
 *  File Name     : MessageReceived.java
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
import javax.jain.ss7.inap.exception.*;
import javax.jain.*;

/**
This class represents the Message Received Operation.
*/

public class MessageReceived  extends Operation   implements Serializable
{

	 private ReceivedStatus receivedStatus;
	 
	 
	
      private int recordedMessageID;
	 private boolean isRecordedMessageID = false ;

     private int recordedMessageUnits;
	 private boolean isRecordedMessageUnits = false ;
	 
	private ExtensionField extensions[];
  private boolean isExtensions = false ;  	 

/**
Constructor For Message Received
*/
	
	public MessageReceived(ReceivedStatus receivedStatus) 
	{
		operationCode=OperationCode.PROMPT_AND_RECEIVE_MESSAGE;
		setReceivedStatus(receivedStatus);
	}

//-------------------------------

//--------------------------------------	
/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------





/**<P>
Gets Received Status.
<DT> This parameter gets the result of the recording. 
<LI>MESSAGE_COMPLETE - The recording was successfully completed.
<LI>MESSAGE_INTERRUPTED - The user has abandoned the recording.	
<LI>MESSAGE_TIMEOUT - The maximum recording time has been exceeded.
<P>
*/
    public ReceivedStatus getReceivedStatus() 
    {
        return receivedStatus;
    }
    
/**
Sets Received Status
*/
    public void setReceivedStatus(ReceivedStatus receivedStatus) 

        {
         this.receivedStatus	 = receivedStatus;
    }

//-----------------------

/**
Gets Recorded MessageID
*/
    public int getRecordedMessageID() throws ParameterNotSetException
    {
        if(isRecordedMessageIDPresent()) 
            return recordedMessageID;
        else 
            throw new ParameterNotSetException(); 
    }
    
/**
Sets Recorded MessageID
*/
    public void setRecordedMessageID(int recordedMessageID) throws IllegalArgumentException
    {
    	 if((recordedMessageID>=0)&&(recordedMessageID<=2147483647))
    	 {
            this.recordedMessageID = recordedMessageID;
            isRecordedMessageID = true;
         }
        else
        {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        }
    }

/**
Indicates if the Recorded MessageID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isRecordedMessageIDPresent()
    {
        return isRecordedMessageID;
    }
  
//-----------------------
  
/**
Gets Recorded Message Units
*/
    public int getRecordedMessageUnits() throws ParameterNotSetException
    {
        if(isRecordedMessageUnitsPresent())
            return recordedMessageUnits;
        else 
            throw new ParameterNotSetException(); 
     
    }
    
/**
Sets Recorded Message Units
*/
    public void setRecordedMessageUnits(int recordedMessageUnits)
    {
        this.recordedMessageUnits = recordedMessageUnits;
        isRecordedMessageUnits = true;
    }

/**
Indicates if the Recorded Message Units optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isRecordedMessageUnitsPresent()
    {
        return isRecordedMessageUnits;
    }

//-----------------------

/**
Gets Extensions Parameter
*/

    public ExtensionField[] getExtensions() throws ParameterNotSetException
    {
        if(isExtensionsPresent())
             return extensions;
        else 
            throw new ParameterNotSetException(); 

    }

/**
Gets a particular  Extension Parameter
*/

    public ExtensionField getExtension(int index)
    {
        return extensions[index];
    }

/**
Sets Extensions Parameter
*/

    public void  setExtensions (ExtensionField extensions[])
    {
        this.extensions = extensions ;
        isExtensions = true;
    }

/**
Sets a particular Extensions Parameter
*/

    public void  setExtension (int index , ExtensionField extension)
    {
        this.extensions[index] = extension ;
    }

/**
Indicates if the Extensions optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isExtensionsPresent()
    {
        return isExtensions;
    }
    
//-----------------------



}
