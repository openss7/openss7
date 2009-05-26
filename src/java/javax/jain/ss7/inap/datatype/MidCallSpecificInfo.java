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
 *  File Name     : MidCallSpecificInfo.java
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
import javax.jain.*;

/**
 This class defines the MidCallSpecificInfo DataType
 */

public class MidCallSpecificInfo implements Serializable
{
	
	private int connectTime;
	private boolean isConnectTime = false ;

	private DigitsGenericDigits oMidCallInfo;
	private boolean isOMidCallInfo = false ;


/**
Gets Connect Time
*/
	public int getConnectTime() throws ParameterNotSetException
    	{
        		if( isConnectTimePresent())
				{
                        return connectTime;
                }
				else
				{
                        throw new ParameterNotSetException();
                }
    	}
/**
Sets Connect Time
*/
	public void setConnectTime(int connectTime)
    	{
        	this.connectTime = connectTime;
            isConnectTime=true;
    	}


/**
Indicates if the Connect Time optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isConnectTimePresent()
    {
        return isConnectTime;

    }



//-----------------------

/**
Gets Mid Call Info
*/
	public DigitsGenericDigits getOMidCallInfo() throws ParameterNotSetException
    	{
        		if(isOMidCallInfoPresent())
				{
                	return oMidCallInfo;
                }
				else
				{
                    throw new ParameterNotSetException();
                }
    	}
/**
Sets Mid Call Info
*/
	public void setOMidCallInfo(DigitsGenericDigits oMidCallInfo)
    	{
        		this.oMidCallInfo = oMidCallInfo;
                isOMidCallInfo=true;
    	}

/**
Indicates if the Mid Call Info optional parameter is present .
Returns:TRUE if present , FALSE otherwise.
*/
    public boolean isOMidCallInfoPresent()
    {
        return isOMidCallInfo;

    }



//-----------------------

}
