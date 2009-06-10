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
 *  File Name     : GapCriteria.java
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
This class represents the Gap Criteria DataType
*/
public class GapCriteria  implements java.io.Serializable
{

    private DigitsGenericNumber calledAddressValue;


    private GapOnService gapOnService;


    private int gapAllInTraffic;


    private CalledAddressAndService calledAddressAndService;


    private CallingAddressAndService callingAddressAndService;


	private GapCriteriaChoice gapCriteriaChoice;

/**
Constructors For GapCriteria
*/
	public GapCriteria(DigitsGenericNumber calledAddressValue)
	{
		setCalledAddressValue (calledAddressValue);
	}

	public GapCriteria(GapOnService gapOnService)
	{
		setGapOnService (gapOnService);
	}

	public GapCriteria(int gapAllInTraffic)
	{
		setGapAllInTraffic (gapAllInTraffic);
	}

	public GapCriteria(CalledAddressAndService calledAddressAndService)
	{
		setCalledAddressAndService (calledAddressAndService);
	}

	public GapCriteria(CallingAddressAndService callingAddressAndService)
	{
		setCallingAddressAndService (callingAddressAndService);
	}

//-----------------------------------
/**
Gets Gap Criteria Choice
*/

public GapCriteriaChoice getGapCriteriaChoice()
	{ 
		return gapCriteriaChoice;
	}
/**
Gets Called Address Value
*/

    public DigitsGenericNumber  getCalledAddressValue() throws ParameterNotSetException
    {
        if(gapCriteriaChoice.getGapCriteriaChoice() ==GapCriteriaChoice.CALLED_ADDRESS_VALUE.getGapCriteriaChoice())
		{
        	return calledAddressValue ;
        }
		else
		{
            throw new ParameterNotSetException();
        }  
    }

/**
Sets Called Address Value
*/


    public void setCalledAddressValue (DigitsGenericNumber calledAddressValue)
    {
        this.calledAddressValue = calledAddressValue ;
        gapCriteriaChoice =GapCriteriaChoice.CALLED_ADDRESS_VALUE ;
    }




//-----------------------

/**
Gets Gap On Service
*/

    public GapOnService  getGapOnService() throws ParameterNotSetException
    {
        if(gapCriteriaChoice.getGapCriteriaChoice() ==GapCriteriaChoice.GAP_ON_SERVICE.getGapCriteriaChoice())
		{
        	return gapOnService ;
        }
		else
		{
            throw new ParameterNotSetException();
        }  
    }

/**
Sets Gap On Service
*/


    public void setGapOnService (GapOnService  gapOnService)
    {
        this.gapOnService = gapOnService;
        gapCriteriaChoice =GapCriteriaChoice.GAP_ON_SERVICE; 
    }

//-----------------------

/**
Gets Gap All In Traffic
*/

    public int  getGapAllInTraffic() throws ParameterNotSetException
    {
        if(gapCriteriaChoice.getGapCriteriaChoice() ==GapCriteriaChoice.GAP_ALL_IN_TRAFFIC.getGapCriteriaChoice())
		{
        	return gapAllInTraffic ;
        }
		else
		{
            throw new ParameterNotSetException();
        }   
    }

/**
Sets Gap All In Traffic
*/
    public void setGapAllInTraffic (int gapAllInTraffic)
    {
        this.gapAllInTraffic = gapAllInTraffic ;
        gapCriteriaChoice =GapCriteriaChoice.GAP_ALL_IN_TRAFFIC ;
    }


//-----------------------
/**
Gets Called Address and Service Value
*/

    public CalledAddressAndService  getCalledAddressAndService() throws ParameterNotSetException
    {
        if(gapCriteriaChoice.getGapCriteriaChoice() ==GapCriteriaChoice.CALLED_ADDRESS_AND_SERVICE.getGapCriteriaChoice())
		{
        	return calledAddressAndService ;
        }
		else
		{
            throw new ParameterNotSetException();
        }  
    }

/**
Sets Called Address and Service Value
*/


    public void setCalledAddressAndService (CalledAddressAndService  calledAddressAndService)
    {
        this.calledAddressAndService = calledAddressAndService ;
        gapCriteriaChoice =GapCriteriaChoice.CALLED_ADDRESS_AND_SERVICE;
    }

//-----------------------
/**
Gets Calling Address and Service Value
*/

    public CallingAddressAndService  getCallingAddressAndService() throws ParameterNotSetException
    {
       if(gapCriteriaChoice.getGapCriteriaChoice() ==GapCriteriaChoice.CALLING_ADDRESS_AND_SERVICE.getGapCriteriaChoice())
	   { 
       		return callingAddressAndService ;
       }
	   else
	   {
            throw new ParameterNotSetException();
       }  
    }

/**
Sets Calling Address and Service Value
*/


    public void setCallingAddressAndService (CallingAddressAndService  callingAddressAndService)
    {
        this.callingAddressAndService = callingAddressAndService ;
        gapCriteriaChoice =GapCriteriaChoice.CALLING_ADDRESS_AND_SERVICE;
    }


//-----------------------

}
