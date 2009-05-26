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
 *  File Name     : Cause.java
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
This class defines the Cause Datatype.(Encoded as in Q.763)
*/

public class Cause implements Serializable
{

    private int location;
    
    private int codingStandard;
    
    private String causeValue;
    
    private String diagnostics;
    private boolean isdiagnostics =false ;
    
/**
Constructor For Cause
*/
	public Cause(int location, int codingStandard, String causeValue)
	{
		setLocation (location);
		setCodingStandard (codingStandard);
		setCauseValue (causeValue);
	}

//---------------------------------------
   
/**
Gets  Location
*/

    public int getLocation ()
    {
        return location ;
    }
/**
Sets  Location
*/

public void setLocation ( int location)
    {
        this.location =location ;
    }

//---------------------------------------

/**
Gets  Coding Standard
*/
    public int getCodingStandard ()
    {
        return codingStandard;
    }

/**
Sets  Coding Standard
*/
    public void setCodingStandard ( int codingStandard)
    {
        this.codingStandard =codingStandard ;
    }

//---------------------------------------

/**
Gets  Cause Value
*/
    public String getCauseValue ()
    {
        return causeValue ;
    }

/**
Sets  Cause Value
*/
    public void setCauseValue ( String  causeValue )
    {
        this.causeValue =causeValue ;
    }
//---------------------------------------

/**
Gets  Dignostics
*/

    public String getDiagnostics () throws ParameterNotSetException
    {
        if(isDiagnosticsPresent())
		{
        	return diagnostics ;
        }
		else
		{
            throw new ParameterNotSetException();
        }           
    }

/**
Sets  Dignostics
*/

    public void setDiagnostics( String  diagnostics )
    {
        this.diagnostics =diagnostics ;
        isdiagnostics=true;
    }


/**
Indicates if this choice parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/

    public boolean isDiagnosticsPresent()
    {
        return isdiagnostics;
    }

//---------------------------------------

}