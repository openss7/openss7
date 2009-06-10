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
 *  File Name     : VariableMessage.java
 *  Approver      : Jain Inap Edit Group
 *  Version       : 1.0
 *
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
package javax.jain.ss7.inap.datatype;


import java.io.*;
import java.util.*;
import javax.jain.ss7.inap.constants.*;
/**
 This class defines the Variable Message DataType
 */

public class VariableMessage implements java.io.Serializable{

	 private int elementaryMessageID;

	 private VariablePart variableParts[];
	 
	 
/**
Constructor For VariableMessage
*/
	public VariableMessage(int elementaryMessageID, VariablePart variableParts[])
	{
		setElementaryMessageID(elementaryMessageID);
		setVariableParts(variableParts);
	}
	
//------------------------------------------	

/**
  Gets Elementary Message Id 
*/

    public int getElementaryMessageID()
    {
        return elementaryMessageID;
    }

/**
  Sets Elementary Message Id
*/
    public void setElementaryMessageID(int elementaryMessageID)
    {
        this.elementaryMessageID = elementaryMessageID;
    }
    
//-----------------------
	 
/**
  Gets Variable Parts 
*/

    public VariablePart[] getVariableParts()
    {
        return variableParts;
    }

/**
Gets a particular Variable Parts
*/

    public VariablePart getVariablePart(int index)
    {
        return variableParts[index];
    }

/**
  Sets Variable Parts
*/
    public void setVariableParts(VariablePart variableParts[])
    {
        this.variableParts = variableParts;
    } 

/**
Sets a particular Variable Part*/

    public void  setVariablePart(int index , VariablePart variablePart) 
    {
        variableParts[index] = variablePart;
    }    
    
//-----------------------
    
}
