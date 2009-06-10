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
 *  File Name     : ReportUTSI.java
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
 This class represents the ReportUTSI Operation.
*/


public class ReportUTSI  extends Operation   implements java.io.Serializable
{
    

    private String uSIServiceIndicator;
    
    private LegID legID;

    private String uSIInformation;
    
	private ExtensionField extensions[];
  private boolean isExtensions = false ;      
    
    
/**
Constructor For ReportUTSI
*/
	public ReportUTSI(java.lang.String uSIServiceIndicator, java.lang.String uSIInformation, LegID legID)
	{
		operationCode = OperationCode.REPORT_UTSI;
		setUSIServiceIndicator(uSIServiceIndicator);
		setLegID(legID);
		setUSIInformation(uSIInformation);
	}

//-----------------------------------    
/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------


/**Gets USI Service Indicator*/


    public java.lang.String getUSIServiceIndicator()
    {
        return uSIServiceIndicator;
    }

/**Sets USI Service Indicator*/

    public void setUSIServiceIndicator( String uSIServiceIndicator)
    {
       this.uSIServiceIndicator=uSIServiceIndicator;
    }

//----------------------------------------------------------------------------------

/**
Gets Leg ID
*/
    public LegID getLegID()
    {
        return legID;
    }
    
/**
Sets Leg ID
*/
    public void setLegID(LegID legID)
    {
        this.legID = legID;
    }
/**

//---------------------------------------------------------------------------------

/**Gets USI Information*/


    public java.lang.String getUSIInformation()
    {
        return uSIInformation;
    }

/**Sets USI Information*/

    public void setUSIInformation( String uSIInformation)
    {
       this.uSIInformation=uSIInformation;
    }
//---------------

/**
Gets Extensions Parameter
*/

    public ExtensionField[] getExtensions() throws ParameterNotSetException
    {
        if(isExtensionsPresent())
               return extensions;
        else
              throw new  ParameterNotSetException();
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


