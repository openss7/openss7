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
 *  File Name     : RequestReportUTSI.java
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
import javax.jain.*;

/**
This class represents the RequestReportUTSI Operation.
*/

public class RequestReportUTSI  extends Operation   implements java.io.Serializable
{

    

    private RequestedUTSI requestedUTSIList[];

	 private LegID legID;
	 private boolean isLegID = false ;
	 
	private ExtensionField extensions[];
    private boolean isExtensions  = false ;  	 
	 
/**
Constructor For RequestReportUTSI
*/
	public RequestReportUTSI(RequestedUTSI requestedUTSIList[])
	{
		operationCode = OperationCode.REQUEST_REPORT_UTSI;
		setRequestedUTSIList(requestedUTSIList);
	}
	
//--------------------------------------	
/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------

/**
Gets Requested UTSI List
*/

    public RequestedUTSI[] getRequestedUTSIList()
    {
        return requestedUTSIList;
    }

/**
Gets a particular Requested UTSI
*/

    public RequestedUTSI getRequestedUTSI(int index)
    {
        return requestedUTSIList[index];
    }

/**
Sets Requested UTSI List*/

    public void  setRequestedUTSIList(RequestedUTSI requestedUTSIList[])
    {
        this.requestedUTSIList = requestedUTSIList;
    }


/**
Sets a particular Requested UTSI
*/

    public void  setRequestedUTSI(int index ,RequestedUTSI requestedUTSI)
    {
        requestedUTSIList[index] = requestedUTSI;
    }

//----------------------------------------------

/**
Gets Leg ID
*/
    public LegID getLegID() throws ParameterNotSetException
    {
        if(isLegIDPresent()){
        return legID;
        }else{
               throw new ParameterNotSetException();
            }  
    }
    
/**
Sets Leg ID
*/
    public void setLegID(LegID legID)
    {
        this.legID = legID;
        isLegID=true;
    }
    
/**
Indicates if the Leg ID optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isLegIDPresent()
    {
        return isLegID;
    }

//-----------------------

/**
Gets Extensions Parameter
*/

    public ExtensionField[] getExtensions() throws ParameterNotSetException
    {
        if(isExtensionsPresent()){
         return extensions;
        }else{
               throw new ParameterNotSetException();
            }  
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
        isExtensions=true;
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

