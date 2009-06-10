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
 *  File Name     : ExtensionField.java
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
This class represents the ExtensionField DataType
*/

public class ExtensionField implements java.io.Serializable
{
	
	private ExtensionChoice extensionChoice = null;

	private int criticalityType;	

	private Extension extensionValue;

/**
Constructor For ExtensionField
*/
	public ExtensionField(int criticalityType, Extension extensionValue) 
	{
		setCriticalityType(criticalityType);
		setExtensionValue(extensionValue);
		this.extensionChoice = extensionValue.getExtensionChoice();
	}
//---------------------------------------------	

/**<P>
Gets Extension Type.
<DT> This parameter gets the type of an extension(argument datatype).
<LI>INTERGER_TYPE
<LI>BOOLEAN_TYPE	
<LI>STRING_TYPE
<P>
*/
	public ExtensionChoice getExtensionChoice()
    	{
        		return extensionChoice;
    	}

//---------------------------------------------	

/**
Gets Criticality Type of Data
*/
	public int getCriticalityType()
    	{
        		return criticalityType;
    	}
/**
Sets Criticality Type of Data
*/
	public void setCriticalityType(int criticalityType)
    	{
        		this.criticalityType = criticalityType;
    	}


//---------------------------------------------	

/**
Gets Extension Value
*/
	public Extension getExtensionValue()
    	{
        		return extensionValue;
    	}

/**
Sets Extension Value
*/
	public void setExtensionValue(Extension extensionValue) 
    	{
        		this.extensionValue = extensionValue;
    	}


//---------------------------------------------	

}
