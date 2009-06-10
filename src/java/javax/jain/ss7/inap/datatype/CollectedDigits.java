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
 *  File Name     : CollectedDigits.java
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
This class defines the CollectedDigits Datatype
*/

public class CollectedDigits implements java.io.Serializable
{

    private int minimumNbOfDigits = 1;

    private int maximumNbOfDigits;

	 private String endOfReplyDigit;
	 private boolean isEndOfReplyDigit =false ;
	 
	 private String cancelDigit;
	 private boolean isCancelDigit =false ;
	 private String startDigit;
	 private boolean isStartDigit =false ;
	 
    private int firstDigitTimeOut;
    private boolean isFirstDigitTimeOut =false ;
	 
    private int interDigitTimeOut;
    private boolean isInterDigitTimeOut =false;

    private ErrorTreatment errorTreatment = ErrorTreatment.REPORT_ERROR_TO_SCF;
    private boolean isErrorTreatment =false ;

    private boolean interruptableAnnInd = true;
    private boolean isInterruptableAnnInd =false ;
    
    private boolean voiceInformation = false;
    private boolean isVoiceInformation =false ;

    private boolean voiceBack = false;
    private boolean isVoiceBack =false ;


/**
Constructor For CollectedDigits
*/
	public CollectedDigits(int minimumNbOfDigits,int maximumNbOfDigits) throws IllegalArgumentException
	{
		setMinimumNbOfDigits(minimumNbOfDigits);
		setMaximumNbOfDigits(maximumNbOfDigits);
	}
//--------------------------------------

/**
Gets Minimum Number Of Digits
*/
    public int getMinimumNbOfDigits()
    {
        return minimumNbOfDigits;
    }
    
/**
Sets Minimum Number Of Digits
*/
    public void setMinimumNbOfDigits(int minimumNbOfDigits) throws IllegalArgumentException

    {
    	if((minimumNbOfDigits>=1) &&( minimumNbOfDigits <=127))
    	  {
           		this.minimumNbOfDigits = minimumNbOfDigits;
          }
        else
          {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        }
    }

//----------------------------------------------

/**
Gets Maximum Number Of Digits
*/
    public int getMaximumNbOfDigits()
    {
        return maximumNbOfDigits;
    }
    
/**
Sets Maximum Number Of Digits
*/
    public void setMaximumNbOfDigits(int maximumNbOfDigits) throws IllegalArgumentException

    { 
        if((maximumNbOfDigits >=1 )&& (maximumNbOfDigits <=127))
          {
          		this.maximumNbOfDigits = maximumNbOfDigits;
          }
        else
          {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        }
    }

//----------------------------------------------

/**
Gets End Of Reply Digit
*/
    public java.lang.String getEndOfReplyDigit() throws ParameterNotSetException
    {
        if(isEndOfReplyDigitPresent())
		{
        	return endOfReplyDigit;
        }
		else
		{
        	throw new ParameterNotSetException();
        }
    }
    
/**
Sets End Of Reply Digit
*/
    public void setEndOfReplyDigit(java.lang.String endOfReplyDigit)
    {
        
         this.endOfReplyDigit = endOfReplyDigit;
         isEndOfReplyDigit=true; 
    }
    
/**
Indicates if the End Of Reply Digit optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/   
 public boolean isEndOfReplyDigitPresent()
    {
        return isEndOfReplyDigit;
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
         }else
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
Gets Start Digit
*/
    public java.lang.String getStartDigit() throws ParameterNotSetException
    {
        if(isStartDigitPresent())
		{ 
        	return startDigit;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }
    
/**
Sets Start Digit
*/
    public void setStartDigit(java.lang.String startDigit)
    {
        this.startDigit = startDigit;
        isStartDigit=true;      
    }
    
/**
Indicates if the Start Digit optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/    
public boolean isStartDigitPresent()
    {
        return isStartDigit;
    }


//----------------------------------------------

/**
Gets First Digit Time Out
*/
    public int getFirstDigitTimeOut() throws ParameterNotSetException
    {
        if(isFirstDigitTimeOutPresent())
		{
			return firstDigitTimeOut;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }
    
/**
Sets First Digit Time Out
*/
    public void setFirstDigitTimeOut(int firstDigitTimeOut)throws IllegalArgumentException

    {
    	 if((firstDigitTimeOut >=1 )&&( firstDigitTimeOut <=127))
    	   {
           		this.firstDigitTimeOut = firstDigitTimeOut;
            	isFirstDigitTimeOut=true;
           
           }
         else
          	{  
           		throw new IllegalArgumentException("ParameterOutOfRange");
        	}
    }

/**
Indicates if the First Digit Time Out optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/    
  public boolean isFirstDigitTimeOutPresent()
    {
        return isFirstDigitTimeOut;
    }


    
//----------------------------------------------

/**
Gets Inter Digit Time Out
*/
    public int getInterDigitTimeOut() throws ParameterNotSetException
    {
        if(isInterDigitTimeOutPresent())
		{
        	return interDigitTimeOut;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }
    
/**
Sets Inter Digit Time Out
*/
    public void setInterDigitTimeOut(int interDigitTimeOut)throws IllegalArgumentException

    {
    	if((interDigitTimeOut >=1)&& (interDigitTimeOut <=127))
    	  	{
          		this.interDigitTimeOut = interDigitTimeOut;
          		isInterDigitTimeOut=true;
          	}
       else
          	{  
           		throw new IllegalArgumentException("ParameterOutOfRange");
        	}
    }

/**
Indicates if the Inter  Digit Time Out optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
    
*/
 public boolean isInterDigitTimeOutPresent()
    {
        return isInterDigitTimeOut;
    }


//----------------------------------------------

/**<P>
Gets Error Treatment.
<DT> This parameter gets what specific action has been taken by the SRF in the event of error conditions occurring. 
<LI>REPORT_ERROR_TO_SCF
<LI>HELP	
<LI>REPEAT_PROMPT
<P>
*/

    public ErrorTreatment getErrorTreatment() throws ParameterNotSetException
    {
        if(isErrorTreatmentPresent())
		{
        	return errorTreatment;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }
    
/**
Sets Error Treatment
*/
    public void setErrorTreatment(ErrorTreatment errorTreatment)

       {
			this.errorTreatment = errorTreatment;
			isErrorTreatment = true;

    }

/**
Indicates if the Error Treatment optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/

    public boolean isErrorTreatmentPresent()
    {
        return isErrorTreatment;
    }



//----------------------------------------------

/**
Gets Interruptable Ann Ind
*/
    public boolean getInterruptableAnnInd() throws ParameterNotSetException
    {
        if(isInterruptableAnnIndPresent())
		{
        	return interruptableAnnInd;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }
    
/**
Sets Interruptable Ann Ind 
*/
    public void setInterruptableAnnInd(boolean interruptableAnnInd)
    {
        this.interruptableAnnInd = interruptableAnnInd;
        isInterruptableAnnInd=true;
    }

/**
Indicates if the Interruptable Ann Ind  optional parameter is present .
Returns: TRUE if present, FALSE otherwise.

*/
    public boolean isInterruptableAnnIndPresent()
    {
        return isInterruptableAnnInd;
    }

    
//--------------------------------------------

/**
Gets Voice Information
*/
    public boolean getVoiceInformation() throws ParameterNotSetException
    {
       if(isVoiceInformationPresent())
	   { 
       		return voiceInformation;
       }
	   else
	   {
       	    throw new ParameterNotSetException();
       }
    }
    
/**
Sets Voice Information 
*/
    public void setVoiceInformation(boolean voiceInformation)
    {
        this.voiceInformation = voiceInformation;
        isVoiceInformation=true;
    }

/**
Indicates if the Voice Information optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/

    public boolean isVoiceInformationPresent()
    {
        return isVoiceInformation;
    }

    
//--------------------------------------------

/**
Gets Voice Back
*/
    public boolean getVoiceBack() throws ParameterNotSetException
    {
        if(isVoiceBackPresent())
		{
        	return voiceBack;
        }
		else
		{
            throw new ParameterNotSetException();
        }
    }
    
/**
Sets Voice Back 
*/
    public void setVoiceBack(boolean voiceBack)
    {
        this.voiceBack = voiceBack;
        isVoiceBack=true; 
    }

/**
Indicates if the Voice Back optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isVoiceBackPresent()
    {
        return isVoiceBack;
    }


    
//--------------------------------------------

}
