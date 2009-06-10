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
 *  File Name     : GapIndicators.java
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

/**
* This class represents the GapIndicators DataType*/ 

public class GapIndicators implements java.io.Serializable
{

	private int duration;

	private int interval;

/**
Constructor For GapIndicators
*/
	public GapIndicators(int duration, int interval) throws IllegalArgumentException
	{
		setDuration(duration);
		setInterval(interval);
	}
	
//---------------------------------------------	

/**
Gets Duration
*/
	public int getDuration()
    	{
        		return duration;
    	}
/**
Sets Duration
*/
	public void setDuration(int duration) throws IllegalArgumentException
    	{
    		if((duration>=-2 )&&(duration <=86400))
    		{
        		this.duration = duration;
        	}
        	else
        	{  
           		throw new IllegalArgumentException("ParameterOutOfRange");
           	} 	
    	}

//-----------------------

/**
Gets Interval
*/
	public int getInterval()
    	{
        		return interval;
    	}
/**
Sets Interval
*/
	public void setInterval(int interval) throws IllegalArgumentException
    	{
    		if((interval>=-1)&&(interval< 60000))
    		{
        		this.interval = interval;
        	}
        	else
        	{  
           		throw new IllegalArgumentException("ParameterOutOfRange");
        	}
    	}

//-----------------------
}
