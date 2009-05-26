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
 *  File Name     : DPSpecificCriteria.java
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
import javax.jain.ss7.inap.exception.*;
import javax.jain.*;

/**
This class defines the DP Specific Criteria Datatype
*/

public class DPSpecificCriteria implements Serializable
{

    private int numberOfDigits;

    
    private int applicationTimer;

    
    private MidCallControlInfo midCallControlInfo;


	private DpSpecificCriteriaChoice dPSpecificCriteriaChoice;
    
/**
Constructors For DPSpecificCriteria
*/
	public DPSpecificCriteria(int criteriaValue,DpSpecificCriteriaChoice dPSpecificCriteriaChoice ) throws IllegalArgumentException
	{
		if (dPSpecificCriteriaChoice.getDpSpecificCriteriaChoice() == DpSpecificCriteriaChoice.NUMBER_OF_DIGITS.getDpSpecificCriteriaChoice())
			setNumberOfDigits(criteriaValue);
		else if (dPSpecificCriteriaChoice.getDpSpecificCriteriaChoice() == DpSpecificCriteriaChoice.APPLICATION_TIMER.getDpSpecificCriteriaChoice())
			setApplicationTimer(criteriaValue);
	}

	public DPSpecificCriteria(MidCallControlInfo midCallControlInfo) throws IllegalArgumentException
	{
		setMidCallControlInfo(midCallControlInfo);
	}
	
//---------------------------------------------	

/**
Gets DPSpecificCriteria Choice
*/

	public DpSpecificCriteriaChoice getDPSpecificCriteriaChoice()
		{ 
			return dPSpecificCriteriaChoice;
		}
			
/**
Gets Number of Digits
*/

    public int getNumberOfDigits() throws ParameterNotSetException
    {
        if(dPSpecificCriteriaChoice == DpSpecificCriteriaChoice.NUMBER_OF_DIGITS)
		{
        	return numberOfDigits ;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
    }

/**
Sets Number of Digits
*/

    public void  setNumberOfDigits(int numberOfDigits) throws IllegalArgumentException 
    {
    	if((numberOfDigits>=1)&&(numberOfDigits<=255))
    	{
        	this.numberOfDigits = numberOfDigits ;
        	dPSpecificCriteriaChoice = DpSpecificCriteriaChoice.NUMBER_OF_DIGITS; 
        }
        else
        {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        }
    }
    

//---------------------------------------------	


/**
Gets Application Timer
*/

    public int getApplicationTimer() throws ParameterNotSetException
    {
        if(dPSpecificCriteriaChoice == DpSpecificCriteriaChoice.APPLICATION_TIMER)
		{
        	return applicationTimer ;
        }
		else
		{
            throw new ParameterNotSetException();
        } 
    }

/**
Sets Application Timer
*/

    public void  setApplicationTimer(int applicationTimer) throws IllegalArgumentException
    {
    	if((applicationTimer>=0)&&(applicationTimer<=2047))
    	{ 
        	this.applicationTimer= applicationTimer;
        	dPSpecificCriteriaChoice = DpSpecificCriteriaChoice.APPLICATION_TIMER;
        }
        else
        {  
           throw new IllegalArgumentException("ParameterOutOfRange");
        }
    }

//---------------------------------------------	

/**
Gets Mid Call Control Information
*/

    public MidCallControlInfo getMidCallControlInfo() throws ParameterNotSetException
    {
        if(dPSpecificCriteriaChoice == DpSpecificCriteriaChoice.MID_CALL_CONTROL_INFO)
		{
        	return midCallControlInfo ;
        }
		else
		{
            throw new ParameterNotSetException();
        }         
    }

/**
Sets Mid Call Control Information
*/

    public void  setMidCallControlInfo(MidCallControlInfo midCallControlInfo)
    {
        this.midCallControlInfo = midCallControlInfo ;
        dPSpecificCriteriaChoice = DpSpecificCriteriaChoice.MID_CALL_CONTROL_INFO;
    }


//---------------------------------------------	

}