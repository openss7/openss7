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
 *  File Name     : Reconnect.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */


package javax.jain.ss7.inap.operation;

import java.io.*;
import java.util.*;
import javax.jain.ss7.inap.datatype.*;
import javax.jain.ss7.inap.constants.*;
import javax.jain.ss7.inap.exception.*;
import javax.jain.*;


/**
This class represents the Reconnect Operation.
*/

public class Reconnect  extends Operation   implements java.io.Serializable
{
  

    private int notificationDuration;
    private boolean isNotificationDuration = false ;

    private java.lang.String alertingPattern;
    private boolean isAlertingPattern =false ;

    private java.lang.String displayInformation;
    private boolean isDisplayInformation =false ;

    private int callSegmentID;
    private boolean isCallSegmentID =false ;
    
	private ExtensionField extensions[];
  private boolean isExtensions =false;   
     
/**
Constructor For Reconnect
*/

	public Reconnect() 
	{
		operationCode = OperationCode.RECONNECT;
	}

//-----------------------		
/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------

/**
Gets the Notification Duration
*/

    public int getNotificationDuration() throws ParameterNotSetException
    {
        if(isNotificationDurationPresent())         
             return notificationDuration ;
        else
             throw new ParameterNotSetException();        
    }

/**
Sets the Notification Duration
*/

    public void  setNotificationDuration(int notificationDuration) throws IllegalArgumentException

    { 
    
    	if((notificationDuration >=0 )&&( notificationDuration <=2047))
    	  { 
          this.notificationDuration= notificationDuration;
          isNotificationDuration = true;
          }
         else
          {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        }
    }

/**
Indicates if the Notification Duration optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isNotificationDurationPresent()
    {
        return isNotificationDuration;
    }
    
//------------------------------------------------
    
/**
Gets Alerting Pattern
*/
    public java.lang.String getAlertingPattern() throws ParameterNotSetException
    {
        if(isAlertingPatternPresent())
              return alertingPattern;
        else
             throw new ParameterNotSetException();        

    }
/**
Sets Alerting Pattern
*/
    public void setAlertingPattern(java.lang.String alertingPattern)
    {
        this.alertingPattern = alertingPattern;
        isAlertingPattern = true;
    }
/**
Indicates if the Alerting Pattern optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isAlertingPatternPresent()
    {
        return isAlertingPattern;
    }

//-----------------------

/**
Gets Display Information
*/
    public java.lang.String getDisplayInformation() throws ParameterNotSetException
    {
        if(isDisplayInformationPresent())
             return displayInformation;
        else
             throw new ParameterNotSetException();        

    }
/**
Sets Display Information
*/
    public void setDisplayInformation(java.lang.String displayInformation)
    {
        this.displayInformation = displayInformation;
        isDisplayInformation = true; 
    }
/**
Indicates if the Display Information optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isDisplayInformationPresent()
    {
        return isDisplayInformation;
    }

//-----------------------

/**
Gets Call Segment ID
*/
    public int getCallSegmentID() throws ParameterNotSetException
    {
        if(isCallSegmentIDPresent())  
             return callSegmentID;
        else
             throw new ParameterNotSetException();        

    }
/**
Sets Call Segment ID
*/
    public void setCallSegmentID(int callSegmentID)
    {
        this.callSegmentID = callSegmentID;
        isCallSegmentID = true;
    }
/**
Indicates if the Call Segment ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCallSegmentIDPresent()
    {
        return isCallSegmentID;
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
