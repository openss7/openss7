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
 *  File Name     : CallGap.java
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
This class represents the CallGap Operation.
*/

public class CallGap  extends Operation implements java.io.Serializable
{
    
    private GapCriteria gapCriteria;

    private GapIndicators gapIndicators;

    private ControlType controlType;
    private boolean isControlType = false ;

    private GapTreatment gapTreatment;
    private boolean isGapTreatment = false ;
    
	private ExtensionField extensions[];
  private boolean isExtensions = false ; 
    
/**
Constructor For CallGap
*/
	public CallGap(GapCriteria gapCriteria,GapIndicators gapIndicators)
	{
		operationCode = OperationCode.CALL_GAP;
		setGapCriteria(gapCriteria);
		setGapIndicators(gapIndicators);
	}

//----------------------------------------------    

/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------


/**
Gets Gap Criteria
*/
    public GapCriteria getGapCriteria() 
    {
        return gapCriteria;
    }
/**
Sets Gap Criteria
*/
    public void setGapCriteria(GapCriteria gapCriteria)
    {
        this.gapCriteria = gapCriteria;
    }

//-----------------------

/**
Gets Gap Indicators 
*/
    public GapIndicators getGapIndicators()
    {
        return gapIndicators;
    }
/**
Sets Gap Indicators 
*/
    public void setGapIndicators(GapIndicators gapIndicators)
    {
        this.gapIndicators = gapIndicators;
    }

//-----------------------

/**<P>
Gets Control Type.
<DT> This parameter indicates indicates the reason for activating call gapping. 
<LI>SCP_OVERLOAD - indicates that a control mechanism in the SCP has detected a congestion situation.
<LI>MANUALLY_INITIATED - indicates that the service and or network/service management centre has detected a congestion situation.
<LI>DESTINATION_OVERLOAD - indicates that a control mechanism  has detected a congestion situation at the destination.
<P>
*/

    public ControlType getControlType() throws ParameterNotSetException
    {
       if(isControlTypePresent())
              return controlType;
       else
           throw new ParameterNotSetException();
    }
/**
Sets Control Type
*/
    public void setControlType(ControlType controlType) 

       {
    	this.controlType =  controlType;
       	isControlType = true;

    }
/**
Indicates if the Control Type optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isControlTypePresent()
    {
        return isControlType;
    }

//-----------------------

/**
Gets Gap Treatment  
*/
    public GapTreatment getTreatment() throws ParameterNotSetException
    {
       if(isGapTreatmentPresent())
          return gapTreatment;
       else
           throw new ParameterNotSetException();

    }
/**
Sets Gap Treatment  
*/
    public void setGapTreatment(GapTreatment gapTreatment)
    {
        this.gapTreatment = gapTreatment;
        isGapTreatment = true;
    }

/**
Indicates if the Gap Treatment optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isGapTreatmentPresent()
    {
        return isGapTreatment;
    }

//-----------------------



/**
Gets Extensions Parameter
*/

    public ExtensionField[] getExtensions() throws ParameterNotSetException
    {
       if(isExtensionsPresent())
            return extensions;
       else
           throw new ParameterNotSetException();

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
