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
 *  File Name     : BackwardGVNS.java
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
This Class defines the BackwardGVNS Datatype . (Encoded as in Q.763)
*/
 

public class BackwardGVNS implements Serializable
{

    private int terminatingAccessIndicator;
    
    private int extensionIndicator;
    
    private String extension;
    private boolean isExtension = false ;         

/**
Constructor For BackwardGVNS
*/
	public BackwardGVNS(int terminatingAccessIndicator, int extensionIndicator)
	{
		setTerminatingAccessIndicator(terminatingAccessIndicator);
		setExtensionIndicator(extensionIndicator);
	}

//-----------------------------------    

/**
Gets Terminating Access Indicator
*/

    public int getTerminatingAccessIndicator()
    {
        return terminatingAccessIndicator ;
    }

/**
Sets Terminating Access Indicator
*/

    public void  setTerminatingAccessIndicator (int terminatingAccessIndicator)
    {
        this.terminatingAccessIndicator = terminatingAccessIndicator ;
    }

//-----------------------------------  

/**
Gets Extension Indicator
*/

    public int getExtensionIndicator()
    {
        return extensionIndicator ;
    }

/**
Sets Extension Indicator
*/

    public void  setExtensionIndicator (int extensionIndicator)
    {
        this.extensionIndicator = extensionIndicator ;
    }

//-----------------------------------  
/**
Gets Extension
*/

    public String getExtension() throws ParameterNotSetException 
    {
        if(isExtensionPresent())
		{
           return extension ;
        }
		else
		{
                   throw new ParameterNotSetException();
        }                  
    }

/**
Sets Extension
*/

    public void  setExtension (String extension)
    {
        this.extension = extension ;
        isExtension=true;
    }

/**
Indicates if the Extension optional parameter is present .
Returns: TRUE if present, FALSE otherwise.
*/
    public boolean isExtensionPresent()
    {
        return isExtension;
    }

//-----------------------------------  

}
