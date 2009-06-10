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
 *  File Name     : EntityReleased.java
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
This class represents the EntityReleased Operation.
*/

public class EntityReleased   extends Operation   implements java.io.Serializable
{
	 

    private CSFailure cSFailure;
    private boolean isCSFailure = false ;

    private BCSMFailure bCSMFailure;
    private boolean isBCSMFailure = false ;

/**
Constructor For EntityReleased.
*/

	public EntityReleased() 
	{
		operationCode = OperationCode.ENTITY_RELEASED;
	}

//-----------------------						
/**
Gets Operation Code
*/
    public OperationCode getOperationCode() 
    {
        return operationCode;
    }

//-----------------------

/**
Gets Call Segment Failure
*/

    public CSFailure getCSFailure() throws ParameterNotSetException
    {
        if(isCSFailurePresent())
            return cSFailure;
        else
            throw new ParameterNotSetException();
    }


/**
Sets Call Segment Failure
*/

    public void setCSFailure( CSFailure cSFailure)
    {
        this.cSFailure=cSFailure;
        isCSFailure = true;
    }

/**
Indicates if the Call Segment Failure choice parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/

    public boolean isCSFailurePresent()
    {
        return isCSFailure;
    }

//-----------------------

/**
Gets Basic Call State Model Failure
*/

    public BCSMFailure getBCSMFailure() throws ParameterNotSetException
    {
        if(isBCSMFailurePresent())
                return bCSMFailure;

        else
            throw new ParameterNotSetException();

    }

/**
Sets Basic Call State Model Failure
*/

    public void setBCSMFailure( BCSMFailure bCSMFailure)
    {
        this.bCSMFailure=bCSMFailure;
        isBCSMFailure = true;
    }


/**
Indicates if the Basic Call State Model Failure choice parameter is present .
Returns: TRUE if present , FALSE otherwise.
*/

    public boolean isBCSMFailurePresent()
    {
        return isBCSMFailure;

    }
//-----------------------

}
