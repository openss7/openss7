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
 *  File Name     : ControlDigits.java
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
This class defines the ControlDigits Datatype
*/

public class ControlDigits implements java.io.Serializable
{

	 private String endOfRecordingDigit;
	 private boolean isEndOfRecordingDigit= false ;
	 
	 private String cancelDigit;
	 private boolean isCancelDigit =false ;

	 private String replayDigit;
	 private boolean isReplayDigit =false ;
	 
	 private String restartRecordingDigit;
	 private boolean isRestartRecordingDigit = false ;
	 
    private boolean restartAllowed = false;
    
    private boolean replayAllowed = false;

/**
Gets End Of Recording Digit
*/
    public java.lang.String getEndOfRecordingDigit() throws ParameterNotSetException
    {
        if(isEndOfRecordingDigitPresent())
		{
        	return endOfRecordingDigit;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }
    
/**
Sets End Of Recording Digit
*/
    public void setEndOfRecordingDigit(java.lang.String endOfRecordingDigit)
    {
        this.endOfRecordingDigit = endOfRecordingDigit;
        isEndOfRecordingDigit=true;         
    }
    
/**
Indicates if the Recording  Digit optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isEndOfRecordingDigitPresent()
    {
        return isEndOfRecordingDigit;
    }
    


//----------------------------------------------

/**
Gets Cancel Digit
*/
    public java.lang.String getCancelDigit() throws ParameterNotSetException
    {
        if(isCancelDigitPresent())
		{
        	return cancelDigit;
        }
		else
		{
        	throw new ParameterNotSetException();
        }
    }
    
/**
Sets Cancel Digit
*/
    public void setCancelDigit(java.lang.String cancelDigit)
    {
        this.cancelDigit = cancelDigit;
        isCancelDigit=true;
    }
    
/**
Indicates if the Cancel Digit optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isCancelDigitPresent()
    {
        return isCancelDigit;
    }
    


//----------------------------------------------

/**
Gets Replay Digit
*/
    public java.lang.String getReplayDigit() throws ParameterNotSetException
    {
        if(isReplayDigitPresent())
		{
        	return replayDigit;
        }
		else
		{
        	throw new ParameterNotSetException();
        }   
    }
    
/**
Sets Replay  Digit
*/
    public void setReplayDigit(java.lang.String replayDigit)
    {
        this.replayDigit = replayDigit;
        isReplayDigit=true; 
    }
    
/**
Indicates if the Replay  Digit optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isReplayDigitPresent()
    {
        return isReplayDigit;
    }


//----------------------------------------------

/**
Gets Restart Recording Digit
*/
    public java.lang.String getRestartRecordingDigit() throws ParameterNotSetException
    {
        if(isRestartRecordingDigitPresent())
		{
        	return restartRecordingDigit;
        }
		else
		{
              throw new ParameterNotSetException();
        } 
    }
    
/**
Sets Restart  Recording Digit
*/
    public void setRestartRecordingDigit(java.lang.String restartRecordingDigit)
    {
        this.restartRecordingDigit = restartRecordingDigit;
        isRestartRecordingDigit=true;
    }
    
/**
Indicates if the Restart Recording  Digit optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isRestartRecordingDigitPresent()
    {
        return isRestartRecordingDigit;
    }
    


//----------------------------------------------

/**
Gets Restart Allowed
*/
    public boolean getRestartAllowed()
    {
        return restartAllowed;
    }
    
/**
Sets Restart Allowed 
*/
    public void setRestartAllowed(boolean restartAllowed)
    {
        this.restartAllowed = restartAllowed;
    }

//--------------------------------------------

/**
Gets Replay Allowed
*/
    public boolean getReplayAllowed()
    {
        return replayAllowed;
    }
    
/**
Sets Replay Allowed 
*/
    public void setReplayAllowed(boolean replayAllowed)
    {
        this.replayAllowed = replayAllowed;
    }

    
//--------------------------------------------

}
