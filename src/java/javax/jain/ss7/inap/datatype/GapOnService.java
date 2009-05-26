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
 *  File Name     : GapOnService.java
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
This class represents the Gap On Service DataType
*/

public class GapOnService  implements Serializable
{

    private int serviceKey;

    private int dpCriteria;
    private boolean isDpCriteria= false ;

/**
Constructor For GapOnService
*/
	public GapOnService(int serviceKey) throws IllegalArgumentException
	{
		setServiceKey (serviceKey);
	}
	
//---------------------------------------------	

/**
Gets Service Key
*/

    public int  getServiceKey()
    {
        return serviceKey;
    }

/**
Sets Service Key
*/
    public void setServiceKey (int serviceKey) throws IllegalArgumentException
    {
    	if((serviceKey>=0)&&(serviceKey<= 2147483647 ))
    	{
        	this.serviceKey = serviceKey;
        }
        else
        {  
           	throw new IllegalArgumentException("ParameterOutOfRange");
        }
    }


//-----------------------

/**
Gets Dp Criteria
*/

    public int  getDpCriteria() throws ParameterNotSetException
    {
        if(isDpCriteriaPresent())
		{
        	return dpCriteria;
        }
		else
		{
            throw new ParameterNotSetException();
        }   
    }

/**
Sets Dp Criteria
*/
    public void setDpCriteria(int dpCriteria)
    {
        this.dpCriteria = dpCriteria;
        isDpCriteria=true; 
    }

/** Indicates if the Dp Criteria optional parameter is present .
     Returns:TRUE if present , FALSE otherwise.
*/
    public boolean  isDpCriteriaPresent()
    {
        return isDpCriteria;
    }


//-----------------------

}
